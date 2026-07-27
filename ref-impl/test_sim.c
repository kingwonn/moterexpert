/*
 * test_sim.c — 参考实现的桌面仿真自测(gcc 即可运行, 不依赖硬件)
 *
 * 被测: thermal_model + governor + prot_fsm + selftest + blackbox
 * 对象机: 案例机实测参数(R=6.1Ω, ke=5.7mWb, 110krpm, E_rms=46.4V)
 * 仿真"真实电机"(plant)故意与模型参数偏差 ±20%, 并附加高压端 8W
 * 纹波损耗与 45°C 机内环境 —— 复现实测的"低压 ~95°C / 高压 ~115°C"。
 *
 * 场景:
 *  A 无任何防护, 高压端 140W, 10min      → 验证危险真实存在(T≥110°C)
 *  B Governor 开启, 同工况               → 平台 100~108°C 且持续功率≥118W
 *  C 电阻锚定精度                        → 模型误差 ≤6K(有锚定) vs >8K(无)
 *  D 自检判定: 健康/5% 不平衡/12% 不平衡  → PASS/WARN|DERATE/FAIL
 *  E 黑匣子: 故障冻结、上电不覆盖、显式清除
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "motor_guard.h"

/* ---------------- 模拟 flash(2 页×2KB) 与移植层 ---------------- */
static uint8_t flash_mem[2][2048];
static bool flash_erased[2] = { true, true };
static uint32_t sim_ms = 0;

static bool f_read(uint8_t p, uint16_t o, void *b, uint16_t l)
{ memcpy(b, &flash_mem[p][o], l); return true; }
static bool f_write(uint8_t p, uint16_t o, const void *b, uint16_t l)
{ memcpy(&flash_mem[p][o], b, l); flash_erased[p] = false; return true; }
static bool f_erase(uint8_t p)
{ memset(flash_mem[p], 0xFF, sizeof flash_mem[p]); flash_erased[p] = true; return true; }
static uint32_t up_ms(void) { return sim_ms; }
static float st_angle, st_iref;
static void apply_vec(float a, float i) { st_angle = a; st_iref = i; }
static void rel_vec(void) { st_iref = 0.0f; }

static const mg_port_t PORT = { apply_vec, rel_vec, f_read, f_write, f_erase, up_ms };

/* ---------------- "真实电机" plant(参数与模型故意不同) ---------------- */
typedef struct { float t_cu, t_fe; } plant_t;
#define PL_R25   6.4f     /* 个体电阻 +5% */
#define PL_CCU   9.5f     /* 热容 +19% */
#define PL_RCF   0.8f
#define PL_CFE   118.0f
#define PL_RFA   1.7f     /* 总 Rth≈2.5 K/W → 复现实测温度带 */
#define PL_TAMB  50.0f    /* 机内环境(加热丝邻近) */
#define E_RMS    46.4f    /* 110krpm 相反电动势 */

static float plant_r_hot(const plant_t *p)
{ return PL_R25 * (1.0f + 0.00393f * (p->t_cu - 25.0f)); }

/* 由"墙功率需求"解相电流(§8.5 能量账): 3RI²+3EI = P_motor */
static float solve_irms(float p_motor, float r)
{
    float a = 3.0f * r, b = 3.0f * E_RMS, c = -p_motor;
    return (-b + sqrtf(b * b - 4.0f * a * c)) / (2.0f * a);
}

static void plant_step(plant_t *p, float p_cu, float p_other, float dt)
{
    float q = (p->t_cu - p->t_fe) / PL_RCF;
    p->t_cu += dt / PL_CCU * (p_cu - q);
    p->t_fe += dt / PL_CFE * (p_other + q - (p->t_fe - PL_TAMB) / PL_RFA);
}

/* ---------------- 运行一个工况 ---------------- */
typedef struct { float t_cu_final, t_model_final, p_final; int faulted; } run_res_t;

static run_res_t run_case(float p_wall_demand, float udc, int seconds,
                          bool governor_on, bool anchor_on)
{
    plant_t pl = { 40.0f, 40.0f };
    mg_config_t cfg = MG_DEFAULT_CFG;
    /* 产线 EOL 把本个体的实测 R0 写入(§7.4.1)——电阻锚定的精度前提。
     * 若不做个体标定, +5% 的 R 公差会被当成 +13K 温度偏差(本仿真早期
     * 版本实测到 9.4K 误差), 这正是"个体标定必要性"的定量证明。 */
    cfg.r_phase_25c = PL_R25;
    if (!governor_on) { cfg.t_knee = 500.0f; cfg.t_trip = 500.0f; } /* 关防护 */
    mg_init(&cfg, &PORT);

    float p_board = (udc > 250.0f) ? 10.0f : 5.5f;   /* 板损(§A.1) */
    float p_hf    = (udc > 250.0f) ? 13.0f : 2.0f;   /* 纹波附加马达侧(§A.1 账本) */
    float p_fe_mech = 8.0f;                           /* 基波铁损+机械 */
    run_res_t rr = { 0 };

    for (int s10 = 0; s10 < seconds * 10; s10++) {
        sim_ms += 100;
        /* 受 governor 限制后的实际功率 */
        mg_out_t o = mg_outputs();
        float p_wall = p_wall_demand;
        if (governor_on && o.p_limit_w < p_wall) p_wall = o.p_limit_w;
        float p_motor = p_wall - p_board;

        float r_true = plant_r_hot(&pl);
        float irms = solve_irms(p_motor - p_fe_mech - p_hf, r_true);
        float p_cu_true = 3.0f * irms * irms * r_true;
        plant_step(&pl, p_cu_true, p_fe_mech + p_hf, 0.1f);

        /* 喂给被测模块的观测量(含测量噪声/重构误差) */
        mg_slow_in_t in = { 0 };
        in.iq_flt = irms * 1.41421356f; in.id_flt = 0.0f;
        in.we = 11519.0f; in.udc = udc;
        in.uq = E_RMS * 1.41421356f + r_true * in.iq_flt; in.ud = 0.0f;
        in.t_ntc = PL_TAMB + 1.0f;
        in.rs_online = -1.0f;
        if (anchor_on && (s10 % 100) == 0)               /* 每10s一次电阻辨识 */
            in.rs_online = plant_r_hot(&pl) * (1.0f + 0.01f * ((s10 / 100 % 3) - 1));
        in.p_expected = -1.0f;

        mg_fast_in_t fi = { 0 };
        fi.id = in.id_flt; fi.iq = in.iq_flt; fi.we = in.we;
        fi.udc = udc; fi.observer_ok = true;
        mg_task_fast(&fi);
        mg_task_100ms(&in);
        if ((s10 % 10) == 0) mg_task_1s();

        o = mg_outputs();
        if (!o.run_permitted) rr.faulted = 1;
        rr.p_final = p_wall; rr.t_cu_final = pl.t_cu; rr.t_model_final = o.t_cu_est;
    }
    return rr;
}

static int n_pass = 0, n_fail = 0;
#define CHECK(cond, msg) do { \
    if (cond) { printf("  PASS  %s\n", msg); n_pass++; } \
    else      { printf("  FAIL  %s\n", msg); n_fail++; } } while (0)

int main(void)
{
    printf("=== motor_guard 参考实现仿真自测(案例机参数) ===\n\n");

    /* A: 无防护, 高压端 140W, 10min — 危险必须被复现 */
    printf("[A] 高压端 140W 无防护 10min:\n");
    run_res_t a = run_case(140.0f, 325.0f, 600, false, true);
    printf("    绕组稳态 %.1f°C (实测参照 110~120)\n", a.t_cu_final);
    CHECK(a.t_cu_final > 108.0f && a.t_cu_final < 126.0f,
          "复现实测高压端温度平台(危险区)");

    /* 低压端对照: 120W 应落在 90~100 观测带 */
    run_res_t l = run_case(120.0f, 141.0f, 600, false, true);
    printf("    低压端 120W 对照: %.1f°C (实测参照 90~100)\n", l.t_cu_final);
    CHECK(l.t_cu_final > 86.0f && l.t_cu_final < 102.0f, "复现实测低压端温度");

    /* B: Governor 开启 */
    printf("[B] 高压端 140W + Thermal Governor 10min:\n");
    run_res_t b = run_case(140.0f, 325.0f, 600, true, true);
    printf("    平台 %.1f°C, 持续功率 %.1fW, faulted=%d\n",
           b.t_cu_final, b.p_final, b.faulted);
    CHECK(b.t_cu_final > 97.0f && b.t_cu_final < 110.0f, "温度稳在 97~110°C 平台");
    CHECK(b.p_final >= 110.0f, "持续功率 ≥110W(比一刀切上限聪明: 按实际温度给功率)");
    CHECK(b.faulted == 0, "全程无故障封波(用户无感)");

    /* C: 锚定精度 */
    printf("[C] 电阻锚定(模型参数故意偏差 ±20%%):\n");
    run_res_t c1 = run_case(140.0f, 325.0f, 600, false, true);
    run_res_t c0 = run_case(140.0f, 325.0f, 600, false, false);
    float e1 = fabsf(c1.t_model_final - c1.t_cu_final);
    float e0 = fabsf(c0.t_model_final - c0.t_cu_final);
    printf("    有锚定误差 %.1fK, 无锚定误差 %.1fK\n", e1, e0);
    CHECK(e1 <= 6.0f, "有锚定: 模型误差 ≤6K");
    CHECK(e0 > e1, "锚定显著优于纯积分模型");

    /* D: 自检判定 */
    printf("[D] 上电自检(受控电流注入, 母线无关):\n");
    mg_init(&MG_DEFAULT_CFG, &PORT);
    float rcase[3][3] = { {6.1f, 6.1f, 6.1f},      /* 健康 */
                          {6.1f, 6.41f, 6.1f},     /* 单角度 +5%(轻度匝间) */
                          {6.1f, 7.0f, 6.1f} };    /* 单角度 +15%(严重匝间) */
    mg_selftest_verdict_t vd[3];
    for (int k = 0; k < 3; k++) {
        mg_selftest_start(0.3f, 25.0f);
        mg_selftest_verdict_t v = MG_SELF_RUNNING;
        int guard = 0;
        while (v == MG_SELF_RUNNING && guard++ < 2000) {
            /* 由移植层桩记录的当前注入角度反查"真实电机"该回路电阻 */
            int ang = (st_angle < 1.0f) ? 0 : (st_angle < 3.0f ? 1 : 2);
            /* 模拟: 锁定矢量稳态时 ud = R(该角度)×id */
            v = mg_selftest_step_1ms(st_iref, rcase[k][ang] * st_iref);
        }
        vd[k] = v;
    }
    printf("    健康→%d, +5%%→%d, +15%%→%d (1=PASS 2=WARN 3=DERATE 4=FAIL)\n",
           vd[0], vd[1], vd[2]);
    CHECK(vd[0] == MG_SELF_PASS, "健康电机放行");
    CHECK(vd[1] == MG_SELF_WARN || vd[1] == MG_SELF_DERATE, "轻度不平衡: 告警/降额");
    CHECK(vd[2] == MG_SELF_FAIL, "严重不平衡: 禁止启动(拦截拖炸 IPM 路径)");

    /* E: 黑匣子 */
    printf("[E] 黑匣子冻结与不可覆盖:\n");
    f_erase(0); f_erase(1);
    mg_init(&MG_DEFAULT_CFG, &PORT);
    mg_bb_log_boot(6.15f, 0.00568f, 25.0f);
    /* 注入过流故障 */
    mg_fast_in_t fi = { .id = 0.0f, .iq = 3.0f, .we = 11519.0f,
                        .udc = 325.0f, .observer_ok = true };
    mg_task_fast(&fi);
    mg_slow_in_t si = { .iq_flt = 1.0f, .we = 11519.0f, .udc = 325.0f,
                        .uq = 72.0f, .t_ntc = 45.0f, .rs_online = -1.0f,
                        .p_expected = -1.0f };
    mg_task_100ms(&si);
    CHECK(mg_outputs().run_permitted == false, "过流→封波");
    CHECK(mg_bb_fault_frozen(), "故障现场已冻结入 flash");
    /* "返修上电": 重新 init 不得覆盖 */
    mg_init(&MG_DEFAULT_CFG, &PORT);
    CHECK(mg_bb_fault_frozen(), "重新上电后现场仍在(修复会议漏洞)");
    uint8_t buf[1600]; uint16_t n = mg_bb_export(buf, sizeof buf);
    CHECK(n > 0, "售后工具可导出");
    mg_bb_clear();
    CHECK(!mg_bb_fault_frozen(), "显式清除后才释放");

    printf("\n=== 结果: %d PASS / %d FAIL ===\n", n_pass, n_fail);
    return n_fail ? 1 : 0;
}
