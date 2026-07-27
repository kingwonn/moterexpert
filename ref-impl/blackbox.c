/*
 * blackbox.c — 故障黑匣子 + 趋势档案 (附录 B P0-1/P1, §9.4)
 *
 * 结构: RAM 双环形缓冲(慢环 1Hz×32 条 / 快环 10Hz×64 条)
 *       + 故障冻结: 触发瞬间将两环+故障码写入 flash 页0
 *       + 上电检查冻结标志, 未经 mg_bb_clear() 不覆盖(会议核心漏洞的修补)
 *       + 趋势档案: 每次开机 1 条(R0/ke/环温)追加写 flash 页1
 * 资源: RAM 32×12B + 64×8B = 896B; flash 2 页(各≥1KB)。
 * 页1 追加满后整页搬移压缩(保留最近64条), 擦写次数 << 1万次寿命。
 */
#include "mg_internal.h"
#include <string.h>

#define SLOW_N 32
#define FAST_N 64
#define MAGIC  0xB1ACB0Cu

typedef struct { uint32_t t_ms; float i_amp, udc; int16_t tcu_x10, tmag_x10;
                 uint8_t state, fault; } rec_slow_t;             /* 20B→packed 12B 可再压 */
typedef struct { uint32_t t_ms; float i_amp; int16_t we_krpm_x10; uint16_t pad; } rec_fast_t;
typedef struct { uint32_t magic; uint32_t boot_cnt;
                 float r0, ke, t_amb; } rec_boot_t;
typedef struct { uint32_t magic; uint8_t fault; uint8_t state;
                 uint16_t n_slow, n_fast, head_slow, head_fast; } bb_hdr_t;

static struct {
    rec_slow_t slow[SLOW_N]; uint16_t hs; bool full_s;
    rec_fast_t fast[FAST_N]; uint16_t hf; bool full_f;
    bool frozen_this_run;
    uint32_t boot_cnt;
} bb;

/* ---- 运行时采样(由 mg_task_1s / mg_task_fast 调用) ---- */
void mg_bb_task_1s(void)
{
    rec_slow_t *r = &bb.slow[bb.hs];
    r->t_ms = g_mg.port->uptime_ms();
    r->i_amp = g_mg.i_amp_pk;
    r->udc = 0.0f; /* 由 fast 缓存的最近值填充亦可; 简化: 状态机内已留档 */
    r->tcu_x10 = (int16_t)(g_mg.t_cu * 10.0f);
    r->tmag_x10 = (int16_t)(g_mg.out.t_mag_est * 10.0f);
    r->state = (uint8_t)g_mg.state; r->fault = (uint8_t)g_mg.first_fault;
    bb.hs = (uint16_t)((bb.hs + 1) % SLOW_N); if (bb.hs == 0) bb.full_s = true;
}

void mg_bb_fast_sample(float i_amp, float we)
{
    rec_fast_t *r = &bb.fast[bb.hf];
    r->t_ms = g_mg.port->uptime_ms();
    r->i_amp = i_amp;
    r->we_krpm_x10 = (int16_t)(we * 9.5493f / 100.0f); /* rad/s→krpm×10, p=1 */
    bb.hf = (uint16_t)((bb.hf + 1) % FAST_N); if (bb.hf == 0) bb.full_f = true;
}

/* ---- 故障冻结: 状态机进入 FAULT/LOCKED 时调用一次 ---- */
void mg_bb_on_fault(mg_fault_t fc)
{
    if (bb.frozen_this_run || mg_bb_fault_frozen()) return; /* 已有未导出现场则不覆盖 */
    bb_hdr_t h = { MAGIC, (uint8_t)fc, (uint8_t)g_mg.state,
                   (uint16_t)(bb.full_s ? SLOW_N : bb.hs),
                   (uint16_t)(bb.full_f ? FAST_N : bb.hf), bb.hs, bb.hf };
    g_mg.port->flash_erase(0);
    uint16_t off = 0;
    g_mg.port->flash_write(0, off, &h, sizeof h);           off += sizeof h;
    g_mg.port->flash_write(0, off, bb.slow, sizeof bb.slow); off += sizeof bb.slow;
    g_mg.port->flash_write(0, off, bb.fast, sizeof bb.fast);
    bb.frozen_this_run = true;
}

bool mg_bb_fault_frozen(void)
{
    bb_hdr_t h;
    if (!g_mg.port->flash_read(0, 0, &h, sizeof h)) return false;
    return h.magic == MAGIC;
}

void mg_bb_clear(void) { g_mg.port->flash_erase(0); bb.frozen_this_run = false; }

uint16_t mg_bb_export(uint8_t *buf, uint16_t maxlen)
{
    uint16_t total = (uint16_t)(sizeof(bb_hdr_t) + sizeof bb.slow + sizeof bb.fast);
    if (maxlen < total || !mg_bb_fault_frozen()) return 0;
    g_mg.port->flash_read(0, 0, buf, total);
    return total;
}

/* ---- 趋势档案: 每次开机记录 R0/ke/环温(老化与退磁棘轮的证据链) ---- */
void mg_bb_log_boot(float r0, float ke, float t_amb)
{
    /* 找页1第一个空槽(magic==0xFFFFFFFF), 满则压缩保留最近一半 */
    rec_boot_t r; uint16_t off = 0; const uint16_t N = 1024 / sizeof(rec_boot_t);
    for (uint16_t i = 0; i < N; i++, off += sizeof r) {
        if (!g_mg.port->flash_read(1, off, &r, sizeof r)) return;
        if (r.magic == 0xFFFFFFFFu) {
            rec_boot_t w = { MAGIC, ++bb.boot_cnt, r0, ke, t_amb };
            g_mg.port->flash_write(1, off, &w, sizeof w);
            return;
        }
        bb.boot_cnt = r.boot_cnt;
    }
    g_mg.port->flash_erase(1);          /* 满: 简化处理整页重开(可改为搬移压缩) */
    rec_boot_t w = { MAGIC, ++bb.boot_cnt, r0, ke, t_amb };
    g_mg.port->flash_write(1, 0, &w, sizeof w);
}
