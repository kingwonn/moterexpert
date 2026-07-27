/* mg_internal.h — 模块间共享状态(仅参考实现内部使用) */
#ifndef MG_INTERNAL_H
#define MG_INTERNAL_H
#include "motor_guard.h"

typedef struct {
    const mg_config_t *cfg;
    const mg_port_t   *port;

    /* 热模型状态 */
    float t_cu, t_fe, t_amb;    /* [°C] */
    float r_hot;                /* 当前热态相电阻 [Ω] */
    bool  airflow_blocked;

    /* 磁链温度计 */
    float psi_est;              /* 慢滤波磁链 [Wb] */
    float t_mag;                /* [°C] */

    /* Governor */
    float p_lim_thermal;        /* 热托管功率上限 [W] */
    float gov_integ;

    /* 快速判据聚合(fast → 100ms 传递) */
    float i_amp_pk;             /* 周期内电流幅值峰值 */
    float imbalance;            /* 三相不平衡度(慢更新) */
    uint16_t stall_cnt_ms;
    bool  flag_oc, flag_stall, flag_sync, flag_ov, flag_uv, flag_blocked;

    /* 状态机 */
    mg_state_t state;
    mg_fault_t first_fault;
    uint16_t cooldown_cnt;      /* [100ms ticks] */
    uint8_t  fault_count;

    mg_out_t out;
} mg_ctx_t;

extern mg_ctx_t g_mg;

void mg_thermal_100ms(const mg_slow_in_t *in);
void mg_governor_100ms(const mg_slow_in_t *in);
void mg_fsm_100ms(const mg_slow_in_t *in);
void mg_bb_task_1s(void);
void mg_bb_on_fault(mg_fault_t fc);

#endif
