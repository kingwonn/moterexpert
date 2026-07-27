/*
 * prot_fsm.c — 统一保护状态机 + 快速判据聚合 (§7.1 五层防线, 附录 B P1)
 *
 * 修复会议暴露的"策略互斥"缺陷: 所有判据持续并行评估,
 * 状态机取最严者仲裁; 带迟滞与冷却计时; N 次故障锁存。
 * 快任务只做轻量判据聚合(无浮点除法/无循环), 慢任务做仲裁。
 */
#include "mg_internal.h"

mg_ctx_t g_mg;

void mg_init(const mg_config_t *cfg, const mg_port_t *port)
{
    static const mg_ctx_t zero;
    g_mg = zero;
    g_mg.cfg = cfg; g_mg.port = port;
    g_mg.t_cu = g_mg.t_fe = g_mg.t_amb = 30.0f;
    g_mg.psi_est = -1.0f;
    g_mg.p_lim_thermal = cfg->p_full;
    g_mg.state = MG_ST_NORMAL;
    g_mg.out.run_permitted = true;
    g_mg.out.p_limit_w = cfg->p_full;
    g_mg.out.iq_limit_amp = cfg->i_max_amp;
}

/* ---------------- 快任务: 判据聚合(电流环周期或 1kHz) ---------------- */
void mg_task_fast(const mg_fast_in_t *in)
{
    const mg_config_t *c = g_mg.cfg;

    float i2 = in->id * in->id + in->iq * in->iq;
    float trip2 = c->i_trip_amp * c->i_trip_amp;
    if (i2 > trip2) g_mg.flag_oc = true;               /* 软件镜像; 硬件CBC另有一层 */
    if (i2 > g_mg.i_amp_pk * g_mg.i_amp_pk) {
        /* 记录周期峰值(黑匣子用), 慢任务读取后衰减 */
        g_mg.i_amp_pk = i2 > 0.0f ? __builtin_sqrtf(i2) : 0.0f;
    }

    if (in->udc > c->udc_ov) g_mg.flag_ov = true;
    if (in->udc < c->udc_uv && in->udc > 20.0f) g_mg.flag_uv = true;
    if (!in->observer_ok && in->we > 3000.0f) g_mg.flag_sync = true;

    /* 堵转: 低速大电流持续 */
    float i_stall = c->stall_iq_frac * c->i_max_amp;
    if (in->we < 0.05f * 11519.0f && i2 > i_stall * i_stall) {
        if (g_mg.stall_cnt_ms < 60000u) g_mg.stall_cnt_ms++;
        if (g_mg.stall_cnt_ms > c->stall_ms) g_mg.flag_stall = true;
    } else if (g_mg.stall_cnt_ms > 0) g_mg.stall_cnt_ms--;
}

/* ---------------- 慢任务: 仲裁(100ms) ---------------- */
static void enter_fault(mg_fault_t fc)
{
    if (g_mg.first_fault == MG_FC_NONE) g_mg.first_fault = fc;
    g_mg.fault_count++;
    mg_bb_on_fault(fc);
    g_mg.state = (g_mg.fault_count >= g_mg.cfg->fault_lock_n)
                 ? MG_ST_LOCKED : MG_ST_FAULT;
    g_mg.cooldown_cnt = (uint16_t)(g_mg.cfg->cooldown_s * 10u);
}

void mg_fsm_100ms(const mg_slow_in_t *in)
{
    const mg_config_t *c = g_mg.cfg;
    (void)in;

    if (g_mg.state == MG_ST_LOCKED) { g_mg.out.run_permitted = false; goto out; }

    /* 1) 立即封波类(任一命中即故障, 并行评估, 不互斥) */
    if (g_mg.flag_oc)               { enter_fault(MG_FC_OVERCURRENT); }
    else if (g_mg.flag_ov)          { enter_fault(MG_FC_OVERVOLT); }
    else if (g_mg.flag_stall)       { enter_fault(MG_FC_STALL); }
    else if (g_mg.flag_sync)        { enter_fault(MG_FC_LOSS_SYNC); }
    else if (g_mg.t_cu > c->t_trip) { enter_fault(MG_FC_OVERTEMP_CU); }
    else if (g_mg.psi_est > 0.0f && g_mg.t_mag > 120.0f)
                                    { enter_fault(MG_FC_OVERTEMP_MAG); }

    switch (g_mg.state) {
    case MG_ST_FAULT:
        g_mg.out.run_permitted = false;
        if (g_mg.cooldown_cnt > 0) g_mg.cooldown_cnt--;
        else if (g_mg.t_cu < 70.0f) {          /* 冷却完才允许重试(禁止热重启) */
            g_mg.state = MG_ST_COOLDOWN;
        }
        break;
    case MG_ST_COOLDOWN:
        g_mg.out.run_permitted = true;          /* 允许重启但保持降载 */
        g_mg.out.p_limit_w = c->p_floor;
        if (g_mg.t_cu < 60.0f) { g_mg.state = MG_ST_NORMAL;
                                 g_mg.flag_oc = g_mg.flag_stall = g_mg.flag_sync
                                              = g_mg.flag_ov = g_mg.flag_uv = false; }
        break;
    default: {
        /* 2) 分级降载(热托管已在 governor 产生 p_lim; 这里定名义状态+迟滞) */
        float t = g_mg.t_cu;
        mg_state_t ns = g_mg.state;
        if      (t > c->t_hold + 3.0f) ns = MG_ST_DERATE2;
        else if (t > c->t_knee + 3.0f) ns = MG_ST_DERATE1;
        else if (t > c->t_knee - 5.0f || g_mg.flag_blocked || g_mg.flag_uv)
                                       ns = MG_ST_WARN;
        else if (t < c->t_knee - 8.0f) ns = MG_ST_NORMAL;   /* 迟滞回落 */
        g_mg.state = ns;
        g_mg.out.run_permitted = true;
        if (g_mg.flag_uv) g_mg.out.p_limit_w = c->p_floor;  /* 欠压先降载 */
        break; }
    }
out:
    g_mg.out.state = g_mg.state;
    g_mg.out.fault = g_mg.first_fault;
    g_mg.i_amp_pk *= 0.9f;                     /* 峰值记录衰减 */
}

void mg_task_100ms(const mg_slow_in_t *in)
{
    mg_thermal_100ms(in);
    mg_governor_100ms(in);
    mg_fsm_100ms(in);
    /* governor 输出可能被 FSM 的欠压/冷却分支进一步收紧: 取最小 */
    if (g_mg.out.p_limit_w > g_mg.p_lim_thermal)
        g_mg.out.p_limit_w = g_mg.p_lim_thermal;
}

void mg_task_1s(void) { mg_bb_task_1s(); }

mg_out_t mg_outputs(void) { return g_mg.out; }
