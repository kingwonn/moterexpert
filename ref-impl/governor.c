/*
 * governor.c — 温度闭环功率整形 Thermal Governor (§11.3.2)
 *
 * 冷机满功率; T_cu 或折算后的 T_mag 越过 t_knee 后 PI 介入,
 * 把温度稳在 t_hold 平台; 输出 p_limit 与随转速换算的 iq 上限。
 * 磁体红线(120°C, §8.7)按耦合裕量映射到 t_hold/t_trip。
 */
#include "mg_internal.h"

static float clampf(float x, float lo, float hi)
{ return x < lo ? lo : (x > hi ? hi : x); }

void mg_governor_100ms(const mg_slow_in_t *in)
{
    const mg_config_t *c = g_mg.cfg;

    /* 控制变量: 取绕组估温与"磁体估温折算到绕组尺度"的较严者 */
    float t_ctl = g_mg.t_cu;
    if (g_mg.psi_est > 0.0f) {
        float t_mag_scaled = g_mg.t_mag + (c->t_hold - (120.0f - 10.0f)); /* 磁体红线120-10K裕量 */
        if (t_mag_scaled > t_ctl) t_ctl = t_mag_scaled;
    }

    if (t_ctl < c->t_knee) {
        g_mg.p_lim_thermal = c->p_full;                 /* 冷机: 满血 */
        g_mg.gov_integ *= 0.98f;                        /* 积分缓释 */
    } else {
        /* 标准 PI 稳温: 目标 t_hold, 抗积分饱和 */
        float err = t_ctl - c->t_hold;                  /* >0 = 过热 */
        g_mg.gov_integ += c->ki_gov * err * 0.1f;
        g_mg.gov_integ = clampf(g_mg.gov_integ, 0.0f, c->p_full - c->p_floor);
        float cut = c->kp_gov * err + g_mg.gov_integ;
        if (cut < 0.0f) cut = 0.0f;
        g_mg.p_lim_thermal = clampf(c->p_full - cut, c->p_floor, c->p_full);
    }

    /* iq 上限: iq_max = P/(1.5·p·ψf·ωm), p=1 (§7.2 P5) */
    float psi = (g_mg.psi_est > 0.0f) ? g_mg.psi_est : c->ke_25c;
    float iq_max = c->i_max_amp;
    if (in->we > 2000.0f) {
        iq_max = g_mg.p_lim_thermal / (1.5f * psi * in->we);
        iq_max = clampf(iq_max, 0.0f, c->i_max_amp);
    }

    g_mg.out.p_limit_w   = g_mg.p_lim_thermal;
    g_mg.out.iq_limit_amp = iq_max;
}
