/*! \file bcmbd_cmicx_intr.c
 *
 * CMICx interrupt handler framework.
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

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_acc.h>

#include <bcmbd/bcmbd_cmicx_intr.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

#define BCMBD_CMICX_MAX_INTR_REGS       8

#define MAX_UNITS       BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTR_REGS   BCMBD_CMICX_MAX_INTR_REGS
#define MAX_INTRS       (32 * MAX_INTR_REGS)
#define MAX_CPUS        1

#define INTR_CNT_MAX    100000

#define TXRX_INTR_MASK  0xffffffff

#define IIOWIN_SIZE     0x1000

#define IIOWIN_OFFS(_a) \
    ((_a) & (IIOWIN_SIZE - 1))

#define INTR_VALID(_i) ((unsigned int)(_i) < MAX_INTRS)

#define INTR_MASK_LOCK(_u)
#define INTR_MASK_UNLOCK(_u)

typedef struct bcmbd_intr_reg_s {
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;
    uint32_t mask_val;
    uint32_t kmask;
} bcmbd_intr_reg_t;

typedef struct bcmbd_intr_reg_info_s {
    unsigned int num_intr_regs;
    bcmbd_intr_reg_t intr_regs[MAX_INTR_REGS];
} bcmbd_intr_reg_info_t;

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

typedef struct bcmbd_intr_mask_update_s {
    int unit;
    int irq_num;
    bcmbd_intr_reg_t *intr_reg;
    uint32_t mask_or;
    uint32_t mask_and;
} bcmbd_intr_mask_update_t;


static bool intr_threaded[MAX_UNITS];
static bool intr_connected[MAX_UNITS];
static bool use_knet_intr[MAX_UNITS];
static bcmbd_intr_reg_info_t intr_reg_info[MAX_UNITS];
static bcmbd_intr_vect_info_t intr_vect_info[MAX_UNITS];

static bcmbd_intr_reg_t *
ireg_get(int unit, unsigned int intr_num)
{
    unsigned int rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;

    ri = &intr_reg_info[unit];
    for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
        ireg = &ri->intr_regs[rdx];
        if ((intr_num & ~0x1f) == (ireg->intr_base & ~0x1f)) {
            return ireg;
        }
    }
    return NULL;
}

static void
ireg_mask_write(int unit, int irq_num, bcmbd_intr_reg_t *ireg)
{
    if (ireg->kmask) {
        if (use_knet_intr[unit]) {
            /* Kernel driver owns all bits or none */
            return;
        }
    }
    BCMDRD_IPROC_WRITE(unit, ireg->mask_reg, ireg->mask_val);
}

/*
 * Synchronized update of interrupt mask register.
 *
 * This function should be called via bcmdrd_hal_intr_sync to ensure
 * that it does not execute concurrently with the interrupt handler.
 */
static int
ireg_mask_update(void *data)
{
    bcmbd_intr_mask_update_t *imu = (bcmbd_intr_mask_update_t *)data;

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);

    /* Update interrupt mask value */
    imu->intr_reg->mask_val &= imu->mask_and;
    imu->intr_reg->mask_val |= imu->mask_or;

    /* Write to hardware */
    ireg_mask_write(imu->unit, imu->irq_num, imu->intr_reg);

    INTR_MASK_UNLOCK(unit);

    return 0;
}

/*
 * Update interrupt mask register.
 *
 * This function contains the bulk of the work for
 * bcmbd_cmicx_intr_enable and bcmbd_cmicx_intr_disable.
 */
static void
bcmbd_cmicx_intr_update(int unit, int intr_num, bool enable)
{
    int irq_num = 0;
    int intr_bit;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_mask_update_t intr_mask_update, *imu = &intr_mask_update;

    ireg = ireg_get(unit, intr_num);
    if (ireg) {
        /* Set up sync structure */
        sal_memset(imu, 0, sizeof(*imu));
        imu->unit = unit;
        imu->intr_reg = ireg;
        intr_bit = 1 << (intr_num & 0x1f);
        imu->mask_and = ~intr_bit;
        if (enable) {
            imu->mask_or = intr_bit;
        }

        /* Synchronize update with interrupt context */
        bcmdrd_hal_intr_sync(unit, irq_num, ireg_mask_update, imu);
    }
}

static void
bcmbd_cmicx_intr(int unit)
{
    unsigned int idx, rdx;
    unsigned int intr_num;
    unsigned int active_intr;
    unsigned int intr_cnt;
    uint32_t mask;
    uint32_t istatus;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;
    bcmbd_intr_vect_t *ivect;
    bcmbd_intr_vect_info_t *vi;

    /* Count total number of interrupts processed */
    intr_cnt = 0;

    do {
        /* Count number of interrupts processed in this iteration */
        active_intr = 0;

        ri = &intr_reg_info[unit];
        vi = &intr_vect_info[unit];
        for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
            ireg = &ri->intr_regs[rdx];
            if (ireg->mask_val == 0) {
                /* No interrupts enabled in this mask */
                continue;
            }
            BCMDRD_IPROC_READ(unit, ireg->status_reg, &istatus);
            /* Mask off interrupts handled by kernel driver */
            if (use_knet_intr[unit]) {
                istatus &= ~ireg->kmask;
            }
            /* Mask off inactive interrupts */
            istatus &= ireg->mask_val;
            if (istatus == 0) {
                /* No more pending interrupts in this register */
                continue;
            }
            for (idx = 0, mask = 0x1; idx < 32; idx++, mask <<= 1) {
                if ((istatus & mask) == 0) {
                    continue;
                }
                active_intr++;
                intr_num = ireg->intr_base + idx;
                ivect = &vi->intr_vects[intr_num];
                if (ivect->func) {
                    ivect->func(unit, ivect->param);
                } else {
                    /* No handler for enabled interrupt? */
                    bcmbd_cmicx_intr_disable(unit, intr_num);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "No interrupt handler for "
                                          "intr %u\n"),
                               intr_num));
                }
                /* Prevent infinite loop */
                if (++intr_cnt > INTR_CNT_MAX) {
                    bcmbd_cmicx_intr_disable(unit, intr_num);
                    intr_cnt = 0;
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Cannot clear interrupt for "
                                          "intr %u\n"),
                               intr_num));
                }
            }
        }
    } while (active_intr);
}

static void
bcmbd_cmicx_isr(void *data)
{
    int unit = (unsigned long)data;
    int irq_num = 0;
    unsigned int rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;

    bcmbd_cmicx_intr(unit);

    if (intr_threaded[unit]) {
        /* Threaded interrupt handlers need to re-enable interrupts */
        ri = &intr_reg_info[unit];
        for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
            ireg = &ri->intr_regs[rdx];
            ireg_mask_write(unit, irq_num, ireg);
        }
    }
}

int
bcmbd_cmicx_intr_func_set(int unit, unsigned int intr_num,
                          bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *vi;

    if (!INTR_VALID(intr_num)) {
        return SHR_E_PARAM;
    }

    vi = &intr_vect_info[unit].intr_vects[intr_num];
    vi->func = intr_func;
    vi->param= intr_param;

    return SHR_E_NONE;
}

void
bcmbd_cmicx_intr_enable(int unit, int intr_num)
{
    bcmbd_cmicx_intr_update(unit, intr_num, true);
}

void
bcmbd_cmicx_intr_disable(int unit, int intr_num)
{
    bcmbd_cmicx_intr_update(unit, intr_num, false);
}

void
bcmbd_cmicx_intr_disable_deferred(int unit, int intr_num)
{
    int irq_num = 0;
    int intr_bit;
    bcmbd_intr_reg_t *ireg;

    ireg = ireg_get(unit, intr_num);
    if (ireg) {
        intr_bit = 1 << (intr_num & 0x1f);
        ireg->mask_val &= ~intr_bit;
        /* Only update software structure if threaded interrupt handler */
        if (!intr_threaded[unit]) {
            ireg_mask_write(unit, irq_num, ireg);
        }
    }
}

int
bcmbd_cmicx_intr_regs_clr(int unit)
{
    bcmbd_intr_reg_info_t *ri;

    ri = &intr_reg_info[unit];
    sal_memset(ri, 0, sizeof(*ri));

    return SHR_E_NONE;
}

int
bcmbd_cmicx_intr_reg_set(int unit, unsigned int intr_base,
                         uint32_t status_offs, uint32_t mask_offs,
                         uint32_t kmask)
{
    bcmbd_intr_reg_info_t *ri;
    bcmbd_intr_reg_t *ireg;
    unsigned int rdx;

    if (!INTR_VALID(intr_base)) {
        return SHR_E_PARAM;
    }

    /* Look for existing entry (unlikely) */
    ri = &intr_reg_info[unit];
    ireg = NULL;
    for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
        if (ri->intr_regs[rdx].intr_base == intr_base) {
            ireg = &ri->intr_regs[rdx];
            break;
        }
    }

    /* Use next available entry */
    if (ireg == NULL) {
        if (ri->num_intr_regs >= COUNTOF(ri->intr_regs)) {
            return SHR_E_RESOURCE;
        }
        ireg = &ri->intr_regs[ri->num_intr_regs++];
    }

    /* Initialize interrupt register entry */
    sal_memset(ireg, 0, sizeof(*ireg));
    ireg->intr_base = intr_base;
    ireg->status_reg = status_offs;
    ireg->mask_reg = mask_offs;
    ireg->kmask = kmask;

    return SHR_E_NONE;
}

static struct cmicx_iregs_s {
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg[MAX_CPUS];
} cmicx_iregs[] = {
    { IPROC_IRQ_BASE2, INTC_INTR_RAW_STATUS_REG2r_OFFSET,
      { INTC_INTR_ENABLE_REG2r_OFFSET } },
    { IPROC_IRQ_BASE3, INTC_INTR_RAW_STATUS_REG3r_OFFSET,
      { INTC_INTR_ENABLE_REG3r_OFFSET } },
    { IPROC_IRQ_BASE4, INTC_INTR_RAW_STATUS_REG4r_OFFSET,
      { INTC_INTR_ENABLE_REG4r_OFFSET, } },
    { IPROC_IRQ_BASE5, INTC_INTR_RAW_STATUS_REG5r_OFFSET,
      { INTC_INTR_ENABLE_REG5r_OFFSET, } },
    { IPROC_IRQ_BASE6, INTC_INTR_RAW_STATUS_REG6r_OFFSET,
      { INTC_INTR_ENABLE_REG6r_OFFSET, } },
    { IPROC_IRQ_BASE7, INTC_INTR_RAW_STATUS_REG7r_OFFSET,
      { INTC_INTR_ENABLE_REG7r_OFFSET, } },
    { 0 }
};

int
bcmbd_cmicx_intr_regs_init(int unit)
{
    int rv;
    unsigned int idx;
    unsigned int cpu_no = 0;
    unsigned int intr_base;
    uint32_t status_reg;
    uint32_t mask_reg;
    uint32_t kmask;

    for (idx = 0; idx < COUNTOF(cmicx_iregs); idx++) {
        status_reg = cmicx_iregs[idx].status_reg;
        if (status_reg == 0) {
            /* All done */
            break;
        }
        mask_reg = cmicx_iregs[idx].mask_reg[cpu_no];
        intr_base = cmicx_iregs[idx].intr_base;
        kmask = 0;
        if (status_reg == INTC_INTR_RAW_STATUS_REG4r_OFFSET ||
            status_reg == INTC_INTR_RAW_STATUS_REG6r_OFFSET) {
            /* Set KNET controlled interrupt bits */
            kmask = TXRX_INTR_MASK;
        }
        rv = bcmbd_cmicx_intr_reg_set(unit, intr_base,
                                      status_reg, mask_reg, kmask);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return SHR_E_NONE;
}

int
bcmbd_cmicx_intr_init(int unit)
{
    int rv;

    
    rv = bcmbd_cmicx_intr_regs_clr(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    rv = bcmbd_cmicx_intr_regs_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    rv = bcmbd_cmicx_lp_intr_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    return rv;
}

int
bcmbd_cmicx_intr_disconnect(int unit)
{
    int irq_num = 0;
    int rv;

    if (!intr_connected[unit]) {
        /* Not connected */
        return SHR_E_NONE;
    }

    rv = bcmdrd_hal_intr_disconnect(unit, irq_num);

    if (SHR_SUCCESS(rv)) {
        intr_connected[unit] = false;
    }

    return rv;
}

int
bcmbd_cmicx_intr_connect(int unit)
{
    int irq_num = 0;
    int rv;
    uint64_t phys_addr;
    uint32_t axi_addr;
    unsigned int num_iregs, rdx;
    bcmbd_intr_reg_t *ireg;
    bcmbd_intr_reg_info_t *ri;
    bcmdrd_hal_intr_info_t intr_info;
    bcmdrd_dev_t *dev;
    void (*isr_func)(void *data);
    void *isr_data;

    if (intr_connected[unit]) {
        /* Already connected */
        return SHR_E_NONE;
    }

    sal_memset(&intr_info, 0, sizeof(intr_info));

    dev = bcmdrd_dev_get(unit);
    if (dev == NULL) {
        return SHR_E_INTERNAL;
    }

    /* Base address of device register window (CMIC) */
    intr_info.piowin_addr = dev->io.phys_addr[0];
    intr_info.piowin_size = BCMBD_CMICX_IOMEM_SIZE;

    /* Base address of interrupt controller window */
    phys_addr = dev->io.phys_addr[1];
    axi_addr = INTC_INTR_RAW_STATUS_REG0r_OFFSET;
    phys_addr = bcmdrd_hal_iproc_phys_base_get(unit, phys_addr, axi_addr);
    intr_info.iiowin_addr = phys_addr;
    intr_info.iiowin_size = IIOWIN_SIZE;

    num_iregs = 0;
    ri = &intr_reg_info[unit];
    for (rdx = 0; rdx < ri->num_intr_regs; rdx++) {
        if (num_iregs >= COUNTOF(intr_info.irq_regs)) {
            return SHR_E_RESOURCE;
        }
        ireg = &ri->intr_regs[rdx];
        /* coverity[overrun-local : FALSE] */
        intr_info.irq_regs[num_iregs].status_reg = IIOWIN_OFFS(ireg->status_reg);
        intr_info.irq_regs[num_iregs].mask_reg = IIOWIN_OFFS(ireg->mask_reg);
        if (use_knet_intr[unit]) {
            intr_info.irq_regs[num_iregs].kmask = ireg->kmask;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Enable KNET interrupts\n")));
        }
        num_iregs++;
    }
    intr_info.num_irq_regs = num_iregs;

    rv = bcmdrd_hal_intr_configure(unit, irq_num, &intr_info);

    intr_threaded[unit] = SHR_SUCCESS(rv) ? true : false;
    isr_func = bcmbd_cmicx_isr;
    isr_data = (void *)(unsigned long)unit;

    rv = bcmdrd_hal_intr_connect(unit, irq_num, isr_func, isr_data);

    if (SHR_SUCCESS(rv)) {
        intr_connected[unit] = true;
    }

    return rv;
}

int
bcmbd_cmicx_intr_knet_enable_set(int unit, bool enable)
{
    if (bcmdrd_dev_get(unit) == NULL) {
        return SHR_E_INTERNAL;
    }

    if (enable == use_knet_intr[unit]) {
        /* No change */
        return SHR_E_NONE;
    }

    use_knet_intr[unit] = enable;

    /* Currently this function is a no-op */

    return SHR_E_NONE;
}

int
bcmbd_cmicx_intr_knet_enable_get(int unit, bool *enabled)
{
    if (bcmdrd_dev_get(unit) == NULL) {
        return SHR_E_INTERNAL;
    }

    if (enabled) {
        *enabled = use_knet_intr[unit];
    }

    return SHR_E_NONE;
}
