/*
 * selftest.c — 上电受控绕组自检 (§7.4.2, 附录 B P1)
 *
 * 关键设计(回应 150 机型高压过流教训): 不打恒压脉冲, 复用 FOC 的
 * 电流闭环原语 apply_locked_vector(angle, id_ref) —— 电流天然受限,
 * 与母线电压无关, 85~265VAC 全范围同一套参数。
 *
 * 流程: 三个空间角度(0°/120°/240°)依次注入 I_TEST, 各稳态后取
 * ud/id 平均 → 等效相电阻 R[k]。三角度分别加权 A/B/C 相回路,
 * 匝间/相间/连接异常表现为 R 不对称或整体越窗。
 * 判定: 对比"个体出厂基线×温度补偿", 分四级(§7.4 分级判定)。
 */
#include "mg_internal.h"

#define SETTLE_MS   40      /* 电气时间常数 L/R≈0.1ms, 40ms 足够热稳采样 */
#define AVG_MS      40
#define N_ANGLE     3

typedef enum { ST_IDLE, ST_SETTLE, ST_AVG, ST_NEXT, ST_DONE } st_phase_t;

static struct {
    st_phase_t phase;
    uint8_t  k;
    uint16_t ms;
    float    i_test, t_amb;
    float    acc_u, acc_i; uint16_t n;
    mg_selftest_t res;
} st;

static const float ANG[N_ANGLE] = { 0.0f, 2.0943951f, 4.1887902f };

void mg_selftest_start(float i_test_amp, float t_ambient_c)
{
    static const mg_selftest_t zero_res;
    st.res = zero_res;
    st.phase = ST_SETTLE; st.k = 0; st.ms = 0;
    st.i_test = i_test_amp; st.t_amb = t_ambient_c;
    st.acc_u = st.acc_i = 0.0f; st.n = 0;
    st.res.verdict = MG_SELF_RUNNING;
    g_mg.port->apply_locked_vector(ANG[0], i_test_amp);
}

mg_selftest_verdict_t mg_selftest_step_1ms(float id_meas, float ud_applied)
{
    const mg_config_t *c = g_mg.cfg;
    if (st.phase == ST_IDLE || st.phase == ST_DONE) return st.res.verdict;
    st.ms++;

    switch (st.phase) {
    case ST_SETTLE:
        if (st.ms >= SETTLE_MS) { st.phase = ST_AVG; st.ms = 0; }
        break;
    case ST_AVG:
        st.acc_u += ud_applied; st.acc_i += id_meas; st.n++;
        if (st.ms >= AVG_MS) {
            /* 锁定矢量稳态: ud = R·id (无反电动势、无 L·di/dt) */
            st.res.r_est[st.k] = (st.acc_i > 0.05f) ? st.acc_u / st.acc_i : 999.0f;
            st.k++; st.ms = 0; st.acc_u = st.acc_i = 0.0f; st.n = 0;
            if (st.k >= N_ANGLE) { st.phase = ST_NEXT; }
            else g_mg.port->apply_locked_vector(ANG[st.k], st.i_test);
        }
        break;
    case ST_NEXT: {
        g_mg.port->release_vector();
        /* 温度补偿后的个体基线窗口 */
        float r_ref = c->r_phase_25c * (1.0f + c->alpha_cu * (st.t_amb - 25.0f));
        float avg = (st.res.r_est[0] + st.res.r_est[1] + st.res.r_est[2]) / 3.0f;
        float dev = 0.0f;
        for (int i = 0; i < N_ANGLE; i++) {
            float d = st.res.r_est[i] - avg; if (d < 0) d = -d;
            if (d > dev) dev = d;
        }
        st.res.r_avg = avg;
        st.res.imbalance = (avg > 0.05f) ? dev / avg : 1.0f;
        float win = (avg - r_ref) / r_ref; if (win < 0) win = -win;

        if (st.res.imbalance >= c->imbalance_fail || win >= 2.0f * c->r_window)
            st.res.verdict = MG_SELF_FAIL;          /* 严重异常: 禁止启动 */
        else if (st.res.imbalance >= c->imbalance_warn || win >= c->r_window)
            st.res.verdict = MG_SELF_DERATE;        /* 中度: 降额+记录 */
        else if (st.res.imbalance >= 0.6f * c->imbalance_warn)
            st.res.verdict = MG_SELF_WARN;          /* 轻微: 记录趋势 */
        else
            st.res.verdict = MG_SELF_PASS;
        st.phase = ST_DONE;
        break; }
    default: break;
    }
    return st.res.verdict;
}

const mg_selftest_t *mg_selftest_result(void) { return &st.res; }
