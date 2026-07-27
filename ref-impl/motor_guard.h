/*
 * motor_guard.h — 高速风筒电机安全防护参考实现（公共接口）
 *
 * 覆盖: 双节点热模型+电阻锚定 / 温度闭环功率整形(Governor) /
 *       上电受控自检 / 故障黑匣子 / 统一保护状态机
 *
 * 目标: Cortex-M0 级电机 SoC (凌鸥 LKS32MC / 中微 CMS32M 等), C99, 无动态内存。
 *       浮点用量集中在 10 Hz/1 Hz 慢任务, M0 软浮点可承受;
 *       峰岹 FU68xx(8051) 移植需将慢任务改 Q15, 接口不变。
 *
 * 集成方式: 宿主固件在既有 FOC 环外调用三个周期任务:
 *   mg_task_fast()  — 电流环周期或 1 kHz: 快速保护判据、快环形记录
 *   mg_task_100ms() — 热模型、Governor、状态机
 *   mg_task_1s()    — 黑匣子慢记录、趋势维护
 * 输出经 mg_outputs() 读取: 功率上限、iq 上限、运行许可、故障码。
 */
#ifndef MOTOR_GUARD_H
#define MOTOR_GUARD_H

#include <stdint.h>
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/* 配置(全部参数集中于此; 默认值见 motor_guard_config.c, 出处为全书章节) */
/* ------------------------------------------------------------------ */
typedef struct {
    /* 电机个体参数(产线 EOL 写入, §7.4.1) */
    float r_phase_25c;      /* 相电阻 @25°C [Ω]  规格: 6.1 ±10% */
    float ke_25c;           /* ψf=ke (p=1) [V·s/rad] 规格: 0.0057 ±10% */
    float alpha_cu;         /* 铜温度系数 [1/K] 0.00393 */
    float alpha_ke;         /* ke 温度系数 [1/K] -0.00135 (§8.7) */

    /* 热模型(初值按 §11 反推, 以 E1 实验标定为准) */
    float c_cu;             /* 绕组节点热容 [J/K] */
    float r_cf;             /* 绕组→铁/壳 热阻 [K/W] */
    float c_fe;             /* 铁/壳节点热容 [J/K] */
    float r_fa;             /* 铁/壳→机内环境 热阻 [K/W] */
    float r_fa_blocked;     /* 堵风时的 r_fa (×2~5) */
    float k_anchor;         /* 电阻测温锚定增益 0.1~0.3 */
    float kfe;              /* 铁损系数 P_fe = kfe*we^2 [W·s²/rad²] */
    float p_hf_hiline;      /* 高压端纹波附加损耗 [W] (fPWM 扫频后精化) */
    float udc_hiline_thr;   /* 判定"高压端"的母线阈值 [V] */

    /* Governor(§11.3.2) */
    float t_knee;           /* 介入温度 [°C] 95 */
    float t_hold;           /* 平台温度 [°C] 105 (磁体120红线-耦合裕量, §8.2.3) */
    float t_trip;           /* 封波温度 [°C] 115 */
    float p_full;           /* 满功率 [W] 140 */
    float p_floor;          /* 最低保障功率 [W] 100 */
    float kp_gov;           /* [W/K] */
    float ki_gov;           /* [W/(K·s)] */

    /* 保护阈值(§7.2) */
    float i_trip_amp;       /* 硬件逐拍限流镜像值 [A 幅值] 2.5 */
    float i_max_amp;        /* 稳态电流幅值上限 [A] (热托管下的绝对兜底) */
    float udc_ov, udc_uv;   /* 母线过压/欠压 [V] */
    float imbalance_warn;   /* 三相不平衡告警 3% */
    float imbalance_fail;   /* 三相不平衡失败 8% */
    float r_window;         /* 自检 R 相对个体基线允许偏差 15% */
    float stall_iq_frac;    /* 堵转判据: iq>此比例×i_max 持续 */
    uint16_t stall_ms;      /*           持续时间 [ms] 500 */
    uint8_t  fault_lock_n;  /* 连续故障锁存次数 3 */
    uint16_t cooldown_s;    /* 故障后冷却时间 [s] 30 */
} mg_config_t;

/* ------------------------------------------------------------------ */
/* 输入 */
/* ------------------------------------------------------------------ */
typedef struct {            /* 快任务输入(电流环周期或 1kHz) */
    float ia, ib, ic;       /* 相电流瞬时值 [A] */
    float id, iq;           /* dq 电流 [A 幅值] */
    float we;               /* 电角速度 [rad/s] (p=1 时=机械) */
    float udc;              /* 母线电压 [V] */
    bool  observer_ok;      /* 观测器收敛标志(残差判据由宿主给出) */
} mg_fast_in_t;

typedef struct {            /* 慢任务输入(100ms) */
    float id_flt, iq_flt;   /* 滤波后 dq 电流 [A 幅值] */
    float ud, uq;           /* 施加的 dq 电压(死区补偿后) [V] */
    float we;               /* 电角速度 [rad/s] */
    float udc;              /* 母线电压 [V] */
    float t_ntc;            /* NTC 温度 [°C] (机内环境/板温, <-90 表示无效) */
    float rs_online;        /* 在线辨识电阻 [Ω] (<=0 表示本周期无更新) */
    float p_expected;       /* EOL 标定的本转速期望功率 [W] (<=0 不启用堵风判) */
} mg_slow_in_t;

/* ------------------------------------------------------------------ */
/* 输出 */
/* ------------------------------------------------------------------ */
typedef enum {
    MG_ST_NORMAL = 0, MG_ST_WARN, MG_ST_DERATE1, MG_ST_DERATE2,
    MG_ST_COOLDOWN, MG_ST_FAULT, MG_ST_LOCKED
} mg_state_t;

typedef enum {
    MG_FC_NONE = 0, MG_FC_OVERCURRENT, MG_FC_OVERVOLT, MG_FC_UNDERVOLT,
    MG_FC_STALL, MG_FC_LOSS_SYNC, MG_FC_OVERTEMP_CU, MG_FC_OVERTEMP_MAG,
    MG_FC_IMBALANCE, MG_FC_SELFTEST, MG_FC_BLOCKED_AIR
} mg_fault_t;

typedef struct {
    mg_state_t state;
    mg_fault_t fault;       /* 首触发故障码(黑匣子同步记录) */
    float p_limit_w;        /* 当前允许功率上限 [W] → 速度环外层钳位 */
    float iq_limit_amp;     /* 当前 iq 上限 [A 幅值] */
    float t_cu_est;         /* 估计绕组温度 [°C] */
    float t_mag_est;        /* 估计磁体温度 [°C] (磁链温度计) */
    bool  run_permitted;    /* false → 宿主必须封波 */
} mg_out_t;

/* ------------------------------------------------------------------ */
/* 移植层(宿主实现; 全部为简单原语) */
/* ------------------------------------------------------------------ */
typedef struct {
    /* 自检用: 以电流闭环施加锁定矢量(角度 rad, 电流指令 A), 即 I/F 启动的原语 */
    void (*apply_locked_vector)(float angle_rad, float id_ref);
    void (*release_vector)(void);
    /* 黑匣子用: 页式 flash/EEPROM 原语(页大小≥256B, 两页) */
    bool (*flash_read)(uint8_t page, uint16_t off, void *buf, uint16_t len);
    bool (*flash_write)(uint8_t page, uint16_t off, const void *buf, uint16_t len);
    bool (*flash_erase)(uint8_t page);
    uint32_t (*uptime_ms)(void);
} mg_port_t;

/* ------------------------------------------------------------------ */
/* API */
/* ------------------------------------------------------------------ */
extern const mg_config_t MG_DEFAULT_CFG;

void      mg_init(const mg_config_t *cfg, const mg_port_t *port);
void      mg_task_fast(const mg_fast_in_t *in);
void      mg_task_100ms(const mg_slow_in_t *in);
void      mg_task_1s(void);
mg_out_t  mg_outputs(void);

/* ---- 上电自检(阻塞式步进: 宿主在预定位阶段每 1ms 调一次直至完成) ---- */
typedef enum { MG_SELF_RUNNING=0, MG_SELF_PASS, MG_SELF_WARN,
               MG_SELF_DERATE, MG_SELF_FAIL } mg_selftest_verdict_t;
typedef struct {
    mg_selftest_verdict_t verdict;
    float r_est[3];         /* 三个角度下的等效相电阻 [Ω] */
    float imbalance;        /* 最大偏离比 */
    float r_avg;            /* 平均值(写入趋势档案) */
} mg_selftest_t;
void  mg_selftest_start(float i_test_amp, float t_ambient_c);
mg_selftest_verdict_t mg_selftest_step_1ms(float id_meas, float ud_applied);
const mg_selftest_t *mg_selftest_result(void);

/* ---- 黑匣子 ---- */
void  mg_bb_log_boot(float r0, float ke, float t_amb);  /* 每次开机档案 */
bool  mg_bb_fault_frozen(void);                          /* 上电检查: 有未导出现场? */
void  mg_bb_clear(void);                                 /* 售后导出后显式清除 */
uint16_t mg_bb_export(uint8_t *buf, uint16_t maxlen);    /* 导出冻结现场 */

#endif /* MOTOR_GUARD_H */
