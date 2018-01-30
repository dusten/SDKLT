/*! \file bcm56960_a0_bd_mmu_intr.c
 *
 * The MMU interrupts are daisy-chained on a single CMIC hardware
 * interrupt, which means that the hardware interrupt handler must
 * inspect additional registers to determine the actual source(s) of
 * the interrupt.
 *
 * The MMU interrupt status spans seven registers, which are organized
 * as follows:
 *
 *  - MMU_GCFG_GLB_CPU_INT_STAT (1 register instance)
 *  - MMU_XCFG_XPE_CPU_INT_STAT (4 register instances)
 *  - MMU_SCFG_SC_CPU_INT_STAT (2 register instances)
 *
 * Furthermore, a top-level register (MMU_GCFG_ALL_CPU_INT_STAT)
 * contains a status bit for each of the seven interrupt status
 * registers in order to indicate which registers have pending
 * interrupts. The purpose of the top-level register is to avoid
 * reading all seven interrupt status registers when a hardware
 * interrupt is received.
 *
 * Note that both the MMU interrupt status registers and the top-level
 * register are SOC registers, which should not be read directly from
 * interrupt context. Instead, the primary interrupt handler will mask
 * the CMIC interrupt and start a thread that executes the
 * second-level interrupt handler. When the second-level interrupt
 * handler has no more work to do, the CMIC interrupt is re-enabled.
 */
/*
 * Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
 * Broadcom Limited and/or its subsidiaries.
 * 
 * Broadcom Switch Software License
 * 
 * This license governs the use of the accompanying Broadcom software. Your 
 * use of the software indicates your acceptance of the terms and conditions 
 * of this license. If you do not agree to the terms and conditions of this 
 * license, do not use the software.
 * 1. Definitions
 *    "Licensor" means any person or entity that distributes its Work.
 *    "Software" means the original work of authorship made available under 
 *    this license.
 *    "Work" means the Software and any additions to or derivative works of 
 *    the Software that are made available under this license.
 *    The terms "reproduce," "reproduction," "derivative works," and 
 *    "distribution" have the meaning as provided under U.S. copyright law.
 *    Works, including the Software, are "made available" under this license 
 *    by including in or with the Work either (a) a copyright notice 
 *    referencing the applicability of this license to the Work, or (b) a copy 
 *    of this license.
 * 2. Grant of Copyright License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    copyright license to reproduce, prepare derivative works of, publicly 
 *    display, publicly perform, sublicense and distribute its Work and any 
 *    resulting derivative works in any form.
 * 3. Grant of Patent License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    patent license to make, have made, use, offer to sell, sell, import, and 
 *    otherwise transfer its Work, in whole or in part. This patent license 
 *    applies only to the patent claims licensable by Licensor that would be 
 *    infringed by Licensor's Work (or portion thereof) individually and 
 *    excluding any combinations with any other materials or technology.
 *    If you institute patent litigation against any Licensor (including a 
 *    cross-claim or counterclaim in a lawsuit) to enforce any patents that 
 *    you allege are infringed by any Work, then your patent license from such 
 *    Licensor to the Work shall terminate as of the date such litigation is 
 *    filed.
 * 4. Redistribution
 *    You may reproduce or distribute the Work only if (a) you do so under 
 *    this License, (b) you include a complete copy of this License with your 
 *    distribution, and (c) you retain without modification any copyright, 
 *    patent, trademark, or attribution notices that are present in the Work.
 * 5. Derivative Works
 *    You may specify that additional or different terms apply to the use, 
 *    reproduction, and distribution of your derivative works of the Work 
 *    ("Your Terms") only if (a) Your Terms provide that the limitations of 
 *    Section 7 apply to your derivative works, and (b) you identify the 
 *    specific derivative works that are subject to Your Terms. 
 *    Notwithstanding Your Terms, this license (including the redistribution 
 *    requirements in Section 4) will continue to apply to the Work itself.
 * 6. Trademarks
 *    This license does not grant any rights to use any Licensor's or its 
 *    affiliates' names, logos, or trademarks, except as necessary to 
 *    reproduce the notices described in this license.
 * 7. Limitations
 *    Platform. The Work and any derivative works thereof may only be used, or 
 *    intended for use, with a Broadcom switch integrated circuit.
 *    No Reverse Engineering. You will not use the Work to disassemble, 
 *    reverse engineer, decompile, or attempt to ascertain the underlying 
 *    technology of a Broadcom switch integrated circuit.
 * 8. Termination
 *    If you violate any term of this license, then your rights under this 
 *    license (including the license grants of Sections 2 and 3) will 
 *    terminate immediately.
 * 9. Disclaimer of Warranty
 *    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 *    KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
 *    NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
 *    THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
 *    IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
 * 10. Limitation of Liability
 *    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
 *    THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
 *    SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
 *    INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
 *    OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
 *    (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
 *    LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
 *    COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
 *    THE POSSIBILITY OF SUCH DAMAGES.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_thread.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_sem.h>
#include <sal/sal_time.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/chip/bcm56960_a0_acc.h>
#include <bcmbd/chip/bcm56960_a0_cmic_intr.h>
#include <bcmbd/chip/bcm56960_a0_mmu_intr.h>

#include "bcm56960_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS       MAX_MMU_INTR
#define MAX_INST        4
#define NUM_XPE_INST    4
#define NUM_SC_INST     2

#define CMIC_MMU_INTR   MMU_TO_CMIC_MEMFAIL_INTR

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(mmu_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(mmu_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static bool mmu_intr_connected[MAX_UNITS];
static uint32_t mmu_intr_mask[MAX_UNITS];
static sal_spinlock_t mmu_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t mmu_intr_vect_info[MAX_UNITS];
static shr_thread_ctrl_t *mmu_thread_ctrl[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
mmu_intr_update(int unit, unsigned int intr_num, int enable)
{
    int ioerr = 0;
    MMU_GCFG_ALL_CPU_INT_ENr_t top_en;
    MMU_GCFG_GLB_CPU_INT_ENr_t glb_en;
    MMU_XCFG_XPE_CPU_INT_ENr_t xpe_en;
    MMU_SCFG_SC_CPU_INT_ENr_t sc_en;
    uint32_t intr_mask;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    if (intr_num >= SC_BIT_BASE) {
        intr_mask = mmu_intr_mask[unit];
        intr_mask = (intr_mask & SC_BIT_MASK) >> SC_BIT_BASE;
        MMU_SCFG_SC_CPU_INT_ENr_SET(sc_en, intr_mask);
        ioerr += WRITE_MMU_SCFG_SC_CPU_INT_ENr(unit, 0, sc_en);
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_GCFG_ALL_CPU_INT_ENr(unit, 0, &top_en);
            MMU_GCFG_ALL_CPU_INT_ENr_SC_R_CPU_INT_ENf_SET(top_en, enable);
            MMU_GCFG_ALL_CPU_INT_ENr_SC_S_CPU_INT_ENf_SET(top_en, enable);
            ioerr += WRITE_MMU_GCFG_ALL_CPU_INT_ENr(unit, 0, top_en);
        }
    } else if (intr_num >= XPE_BIT_BASE) {
        intr_mask = mmu_intr_mask[unit];
        intr_mask = (intr_mask & XPE_BIT_MASK) >> XPE_BIT_BASE;
        MMU_XCFG_XPE_CPU_INT_ENr_SET(xpe_en, intr_mask);
        ioerr += WRITE_MMU_XCFG_XPE_CPU_INT_ENr(unit, 0, xpe_en);
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_GCFG_ALL_CPU_INT_ENr(unit, 0, &top_en);
            MMU_GCFG_ALL_CPU_INT_ENr_XPE_R_A_CPU_INT_ENf_SET(top_en, enable);
            MMU_GCFG_ALL_CPU_INT_ENr_XPE_S_A_CPU_INT_ENf_SET(top_en, enable);
            MMU_GCFG_ALL_CPU_INT_ENr_XPE_R_B_CPU_INT_ENf_SET(top_en, enable);
            MMU_GCFG_ALL_CPU_INT_ENr_XPE_S_B_CPU_INT_ENf_SET(top_en, enable);
            ioerr += WRITE_MMU_GCFG_ALL_CPU_INT_ENr(unit, 0, top_en);
        }
    } else {
        intr_mask = mmu_intr_mask[unit];
        intr_mask = (intr_mask & GLB_BIT_MASK) >> GLB_BIT_BASE;
        MMU_GCFG_GLB_CPU_INT_ENr_SET(glb_en, intr_mask);
        ioerr += WRITE_MMU_GCFG_GLB_CPU_INT_ENr(unit, 0, glb_en);
        if (enable || intr_mask == 0) {
            ioerr += READ_MMU_GCFG_ALL_CPU_INT_ENr(unit, 0, &top_en);
            MMU_GCFG_ALL_CPU_INT_ENr_GLB_CPU_INT_ENf_SET(top_en, enable);
            ioerr += WRITE_MMU_GCFG_ALL_CPU_INT_ENr(unit, 0, top_en);
        }
    }
    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "MMU interupt %u I/O error\n"),
                   intr_num));
    }

    INTR_MASK_UNLOCK(unit);
}

static void
mmu_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Enable MMU intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    mmu_intr_mask[unit] |= (1 << intr_num);
    mmu_intr_update(unit, intr_num, 1);
}

static void
mmu_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Disable MMU intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }
    mmu_intr_mask[unit] &= ~(1 << intr_num);
    mmu_intr_update(unit, intr_num, 0);
}

static void
mmu_intr_clear(int unit, unsigned int intr_num)
{
    uint32_t intr_inst;
    uint32_t intr_mask;
    MMU_GCFG_GLB_CPU_INT_CLEARr_t glb_clr;
    MMU_XCFG_XPE_CPU_INT_CLEARr_t xpe_clr;
    MMU_SCFG_SC_CPU_INT_CLEARr_t sc_clr;

    intr_inst = MMU_INTR_INST_GET(intr_num);
    intr_num = MMU_INTR_NUM_GET(intr_num);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Clear MMU intr %u (%u)\n"),
                 intr_num, intr_inst));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    if (intr_num >= SC_BIT_BASE) {
        intr_mask = 1 << (intr_num - SC_BIT_BASE);
        MMU_SCFG_SC_CPU_INT_CLEARr_SET(sc_clr, intr_mask);
        WRITE_MMU_SCFG_SC_CPU_INT_CLEARr(unit, intr_inst, sc_clr);
    } else if (intr_num >= XPE_BIT_BASE) {
        intr_mask = 1 << (intr_num - XPE_BIT_BASE);
        MMU_XCFG_XPE_CPU_INT_CLEARr_SET(xpe_clr, intr_mask);
        WRITE_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, intr_inst, xpe_clr);
    } else {
        intr_mask = 1 << (intr_num - GLB_BIT_BASE);
        MMU_GCFG_GLB_CPU_INT_CLEARr_SET(glb_clr, intr_mask);
        WRITE_MMU_GCFG_GLB_CPU_INT_CLEARr(unit, intr_inst, glb_clr);
    }
}

static void
mmu_intr(int unit)
{
    MMU_GCFG_ALL_CPU_INT_STATr_t top_lvl;
    MMU_GCFG_GLB_CPU_INT_STATr_t glb_stat;
    MMU_XCFG_XPE_CPU_INT_STATr_t xpe_stat;
    MMU_SCFG_SC_CPU_INT_STATr_t sc_stat;
    uint32_t mmu_stat[MAX_INST];
    uint32_t inst_mask;
    uint32_t intr_stat;
    unsigned int inst;
    unsigned int intr_num;

    /* Start with an empty interrupt source matrix */
    sal_memset(mmu_stat, 0, sizeof(mmu_stat));

    /* Read top-level register to figure out what to do next */
    if (READ_MMU_GCFG_ALL_CPU_INT_STATr(unit, 0, &top_lvl) != 0) {
        return;
    }

    /* The GLB interrupt status only has a single instance */
    if (MMU_GCFG_ALL_CPU_INT_STATr_GLB_CPU_INT_STATf_GET(top_lvl)) {
        inst = 0;
        if (READ_MMU_GCFG_GLB_CPU_INT_STATr(unit, inst, &glb_stat) == 0) {
            /* Add GLB interrupt status to interrupt source matrix */
            intr_stat = MMU_GCFG_GLB_CPU_INT_STATr_GET(glb_stat);
            mmu_stat[0] |= (intr_stat << GLB_BIT_BASE);
        }
    }

    /* Determine which XPE register instances have pending interrupts */
    inst_mask = 0;
    if (MMU_GCFG_ALL_CPU_INT_STATr_XPE_R_A_CPU_INT_STATf_GET(top_lvl)) {
        inst_mask |= (1 << 0);
    }
    if (MMU_GCFG_ALL_CPU_INT_STATr_XPE_S_A_CPU_INT_STATf_GET(top_lvl)) {
        inst_mask |= (1 << 1);
    }
    if (MMU_GCFG_ALL_CPU_INT_STATr_XPE_R_B_CPU_INT_STATf_GET(top_lvl)) {
        inst_mask |= (1 << 2);
    }
    if (MMU_GCFG_ALL_CPU_INT_STATr_XPE_S_B_CPU_INT_STATf_GET(top_lvl)) {
        inst_mask |= (1 << 3);
    }

    /* Read XPE register instances with pending interrupts */
    for (inst = 0; inst < NUM_XPE_INST; inst++) {
        if (inst_mask) {
            if ((inst_mask & (1 << inst)) == 0) {
                /* This instance has no pending interrupts */
                continue;
            }
            if (READ_MMU_XCFG_XPE_CPU_INT_STATr(unit, inst, &xpe_stat) == 0) {
                /* Add XPE interrupt status to interrupt source matrix */
                intr_stat = MMU_XCFG_XPE_CPU_INT_STATr_GET(xpe_stat);
                mmu_stat[inst] |= (intr_stat << XPE_BIT_BASE);
            }
        }
    }

    /* Determine which SC register instances have pending interrupts */
    inst_mask = 0;
    if (MMU_GCFG_ALL_CPU_INT_STATr_SC_R_CPU_INT_STATf_GET(top_lvl)) {
        inst_mask |= (1 << 0);
    }
    if (MMU_GCFG_ALL_CPU_INT_STATr_SC_S_CPU_INT_STATf_GET(top_lvl)) {
        inst_mask |= (1 << 1);
    }

    /* Read SC register instances with pending interrupts */
    for (inst = 0; inst < NUM_SC_INST; inst++) {
        if (inst_mask) {
            if ((inst_mask & (1 << inst)) == 0) {
                /* This instance has no pending interrupts */
                continue;
            }
            if (READ_MMU_SCFG_SC_CPU_INT_STATr(unit, inst, &sc_stat) == 0) {
                /* Add SC interrupt status to interrupt source matrix */
                intr_stat = MMU_SCFG_SC_CPU_INT_STATr_GET(sc_stat);
                mmu_stat[inst] |= (intr_stat << SC_BIT_BASE);
            }
        }
    }

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (inst = 0; inst < MAX_INST; inst++) {
        if (mmu_stat[inst] == 0) {
            /* No active interrupts for this instance */
            continue;
        }
        for (intr_num = 0; intr_num < MAX_INTRS; intr_num++) {
            if ((mmu_stat[inst] & (1 << intr_num))) {
                bcmbd_intr_vect_t *iv;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "MMU interrupt %u, instance %u\n"),
                           intr_num, inst));
                iv = &mmu_intr_vect_info[unit].intr_vects[intr_num];
                if (iv->func) {
                    uint32_t param = iv->param;
                    /* Add interrupt instance to callback parameter */
                    MMU_INTR_INST_SET(param, inst);
                    iv->func(unit, param);
                } else {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "No handler for MMU interrupt %u\n"),
                               intr_num));
                    mmu_intr_disable(unit, intr_num);
                }
            }
        }
    }
}

static void
mmu_thread(shr_thread_ctrl_t *tc, void *arg)
{
    int unit = (uintptr_t)arg;
    int cmc = 0;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        mmu_intr(unit);
        bcmbd_cmicd_intr_enable(unit, cmc, CMIC_MMU_INTR);
    }
}

static void
mmu_isr(int unit, uint32_t param)
{
    int cmc = 0;

    /* Disable interrupt and wake up MMU thread */
    bcmbd_cmicd_intr_disable_deferred(unit, cmc, CMIC_MMU_INTR);
    shr_thread_wake(mmu_thread_ctrl[unit]);
}

static int
mmu_intr_init(int unit)
{
    shr_thread_ctrl_t *tc;
    void *arg;

    if (mmu_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    mmu_intr_lock[unit] = sal_spinlock_create("bcmbdMmuIntr");
    if (mmu_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (mmu_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdMmuIntr", -1, mmu_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    mmu_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
mmu_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;
    int cmc = 0;

    bcmbd_cmicd_intr_disable(unit, cmc, CMIC_MMU_INTR);
    mmu_intr_connected[unit] = false;

    if (mmu_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(mmu_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        mmu_thread_ctrl[unit] = NULL;
    }

    if (mmu_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(mmu_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        mmu_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
mmu_intr_func_set(int unit, unsigned int intr_num,
                  bcmbd_intr_f intr_func, uint32_t intr_param)
{
    int cmc = 0;
    bcmbd_intr_vect_t *iv;

    if (!mmu_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for MMU interrupt %u\n"),
                   intr_num));
        bcmbd_cmicd_intr_func_set(unit, cmc, CMIC_MMU_INTR, mmu_isr, 0);
        bcmbd_cmicd_intr_enable(unit, cmc, CMIC_MMU_INTR);
        mmu_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &mmu_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

int
bcm56960_a0_bd_mmu_intr_drv_init(int unit)
{
    int rv;
    bcmbd_intr_drv_t *idrv;

    idrv = bcmbd_mmu_intr_drv_get(unit);

    if (idrv == NULL) {
        return SHR_E_FAIL;
    }
    idrv->intr_func_set = mmu_intr_func_set;
    idrv->intr_enable = mmu_intr_enable;
    idrv->intr_disable = mmu_intr_disable;
    idrv->intr_clear = mmu_intr_clear;

    rv = mmu_intr_init(unit);
    if (SHR_FAILURE(rv)) {
        (void)mmu_intr_cleanup(unit);
    }
    return rv;
}

int
bcm56960_a0_bd_mmu_intr_drv_cleanup(int unit)
{
    return mmu_intr_cleanup(unit);
}
