/*
 * thermal_model.c — 双节点 RC 热网络 + 电阻锚定 + 磁链温度计 (§7.3, §9.3)
 *
 * 通道A(快,相对): 前向欧拉积分, 100ms 步长, 时间常数秒~分钟级, 数值稳定。
 * 通道B(慢,绝对): rs_online 反演绕组温度, 小增益锚定积分器, 消参数误差。
 * 磁体通道: ψ̂f=(uq-R·iq)/ωe 慢滤波 → ke 温度系数反演磁体温度。
 */
#include "mg_internal.h"

#define DT 0.1f

static float clampf(float x, float lo, float hi)
{ return x < lo ? lo : (x > hi ? hi : x); }

void mg_thermal_100ms(const mg_slow_in_t *in)
{
    const mg_config_t *c = g_mg.cfg;

    /* 0) 机内环境温度: NTC 有效则跟随, 否则保守常数 */
    if (in->t_ntc > -90.0f)
        g_mg.t_amb += 0.02f * (in->t_ntc - g_mg.t_amb);

    /* 1) 热态电阻(温度-电阻耦合闭环) */
    g_mg.r_hot = c->r_phase_25c * (1.0f + c->alpha_cu * (g_mg.t_cu - 25.0f));

    /* 2) 损耗估计(幅值不变约定: P_cu = 1.5·(id²+iq²)·R, §3.4) */
    float i2   = in->id_flt * in->id_flt + in->iq_flt * in->iq_flt;
    float p_cu = 1.5f * i2 * g_mg.r_hot;
    float p_fe = c->kfe * in->we * in->we;
    /* 高压端纹波附加损耗(§11.1): 随母线线性内插, fPWM 扫频标定后精化 */
    float p_hf = 0.0f;
    if (in->udc > c->udc_hiline_thr)
        p_hf = c->p_hf_hiline * clampf((in->udc - 180.0f) / (325.0f - 180.0f), 0.0f, 1.2f);

    /* 3) 双节点积分(堵风时切换 r_fa) */
    float r_fa = g_mg.airflow_blocked ? c->r_fa_blocked : c->r_fa;
    float q_cf = (g_mg.t_cu - g_mg.t_fe) / c->r_cf;
    g_mg.t_cu += (DT / c->c_cu) * (p_cu - q_cf);
    g_mg.t_fe += (DT / c->c_fe) * (p_fe + p_hf + q_cf - (g_mg.t_fe - g_mg.t_amb) / r_fa);
    g_mg.t_cu = clampf(g_mg.t_cu, -20.0f, 220.0f);
    g_mg.t_fe = clampf(g_mg.t_fe, -20.0f, 220.0f);

    /* 4) 通道B锚定: 电阻测温 T=T0+(R/R0-1)/α, 小增益拉回积分漂移 */
    if (in->rs_online > 0.5f * c->r_phase_25c) {
        float t_meas = 25.0f + (in->rs_online / c->r_phase_25c - 1.0f) / c->alpha_cu;
        float d = c->k_anchor * (t_meas - g_mg.t_cu);
        g_mg.t_cu += d;
        g_mg.t_fe += d;     /* 参数偏差主要驻留铁/壳节点, 同步修正防止回拉 */
    }

    /* 5) 磁链温度计(仅闭环稳速有效: ωe 足够大且观测器正常) */
    if (in->we > 3000.0f && in->iq_flt > 0.05f) {
        float psi = (in->uq - g_mg.r_hot * in->iq_flt) / in->we;
        if (psi > 0.5f * c->ke_25c && psi < 1.5f * c->ke_25c) {
            if (g_mg.psi_est <= 0.0f) g_mg.psi_est = psi;
            g_mg.psi_est += 0.05f * (psi - g_mg.psi_est);       /* ~2s 滤波 */
            g_mg.t_mag = 25.0f + (1.0f - g_mg.psi_est / c->ke_25c) / (-c->alpha_ke);
            g_mg.t_mag = clampf(g_mg.t_mag, -20.0f, 200.0f);
        }
    }

    /* 6) 堵风检测(§8.3 事实二: 本机越堵功率越大 → 高于期望曲线判堵) */
    if (in->p_expected > 10.0f) {
        float p_elec = 1.5f * (in->ud * in->id_flt + in->uq * in->iq_flt);
        g_mg.flag_blocked = (p_elec > in->p_expected * 1.18f + 8.0f);
        g_mg.airflow_blocked = g_mg.flag_blocked;
    }

    g_mg.out.t_cu_est  = g_mg.t_cu;
    g_mg.out.t_mag_est = (g_mg.psi_est > 0.0f) ? g_mg.t_mag : g_mg.t_fe;
}
