/* motor_guard_config.c — 默认参数表(出处: 全书章节, [E1] 表示需实验标定) */
#include "motor_guard.h"

const mg_config_t MG_DEFAULT_CFG = {
    /* 电机个体参数(规格书 RIM.B02713100-04, 第 8 章; 产线写入个体值覆盖) */
    .r_phase_25c   = 6.1f,      /* 线间 12.2Ω / 2 */
    .ke_25c        = 0.0057f,   /* p=1, ψf=ke */
    .alpha_cu      = 0.00393f,
    .alpha_ke      = -0.00135f, /* §8.7 磁体报告与规格书两点标定 */

    /* 热模型(§11.1 由实测温差反推: Rth_total≈2.2K/W@机内环境45°C; [E1]) */
    .c_cu          = 8.0f,      /* τ1≈5.6s  [E1] */
    .r_cf          = 0.7f,
    .c_fe          = 100.0f,    /* τ2≈150s ≈ 2.5min, 与"3-5min到稳态"观测一致 [E1] */
    .r_fa          = 1.5f,
    .r_fa_blocked  = 4.5f,      /* 堵风×3 [E1] */
    .k_anchor      = 0.2f,
    .kfe           = 4.5e-8f,   /* ≈6W @ 11519rad/s [估计值] */
    .p_hf_hiline   = 8.0f,      /* 高压端纹波附加(马达侧), fPWM扫频后精化 §11.2 */
    .udc_hiline_thr= 250.0f,

    /* Governor(§11.3.2) */
    .t_knee = 95.0f, .t_hold = 105.0f, .t_trip = 115.0f,
    .p_full = 140.0f, .p_floor = 100.0f,
    .kp_gov = 3.0f,  .ki_gov  = 0.15f,

    /* 保护阈值(§7.2; i_trip 需按 IPM SOA 复核) */
    .i_trip_amp = 2.5f,
    .i_max_amp  = 1.35f,        /* 绝对兜底(140W 需≈1.2A 幅值, §8.5) */
    .udc_ov = 400.0f, .udc_uv = 110.0f,
    .imbalance_warn = 0.03f, .imbalance_fail = 0.08f,
    .r_window = 0.15f,
    .stall_iq_frac = 0.8f, .stall_ms = 500,
    .fault_lock_n = 3, .cooldown_s = 30,
};
