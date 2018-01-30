/*! \file bcmbd_cmicd_fifodma.c
 *
 * CMICd FIFODMA routines
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
#include <shr/shr_debug.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_thread.h>
#include <sal/sal_time.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_devlist.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicd_acc.h>
#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmbd/bcmbd_internal.h>
#include "bcmbd_cmicd_fifodma.h"

#define BSL_LOG_MODULE  BSL_LS_BCMBD_FIFODMA

static uint32_t cmicd_fifodma_intr[] = {
    CMICD_IRQ_FIFO_CH0_DMA_INTR,
    CMICD_IRQ_FIFO_CH1_DMA_INTR,
    CMICD_IRQ_FIFO_CH2_DMA_INTR,
    CMICD_IRQ_FIFO_CH3_DMA_INTR
};

static cmicd_fifodma_dev_t fifodma_dev[FIFODMA_DEV_NUM_MAX];

/*!
 * Check FIFODMA status
 */
static int
cmicd_fifodma_stat_check(cmicd_fifodma_dev_t *dev, CMIC_CMC_FIFO_RD_DMA_STATr_t *sta)
{
    int unit = dev->unit;
    int rv = SHR_E_NONE;

    if (CMIC_CMC_FIFO_RD_DMA_STATr_ERRORf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error happened on FIFODMA.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_HOSTMEMWR_ERRORf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error while copying data to host memory.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_SBUSACK_WRONG_BEATCOUNTf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack data size is not the same as "
                              "in rep_words fields.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_SBUSACK_WRONG_OPCODEf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack has wrong opcode.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_SBUSACK_NACKf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus nack with error bit set.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_SBUSACK_ERRORf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus ack with error bit set.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_SBUSACK_TIMEOUTf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "sbus ack not received within configured time.\n")));
        rv = SHR_E_IO;
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_ECC_2BIT_CHECK_FAILf_GET(*sta)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack with ECC_2BIT_CHECK_FAIL set.\n")));
        rv = SHR_E_IO;
    }

    return rv;
}

/*!
 * Start FIFODMA device
 */
static int
cmicd_fifodma_start(cmicd_fifodma_dev_t *dev, int chan)
{
    int unit = dev->unit;
    bcmbd_fifodma_work_t *work = &dev->ctrl->work[chan];
    CMIC_CMC_FIFO_RD_DMA_CFGr_t cfg;
    CMIC_CMC_FIFO_RD_DMA_OPCODEr_t opc;
    CMIC_CMC_FIFO_RD_DMA_SBUS_START_ADDRESSr_t ssa;
    CMIC_CMC_FIFO_RD_DMA_HOSTMEM_START_ADDRESSr_t hsa;
    CMIC_CMC_FIFO_RD_DMA_HOSTMEM_THRESHOLDr_t hth;
    uint32_t sel;
    int cmc, ch;

    cmc = chan / CMICD_FIFODMA_CMC_CHAN;
    ch = chan % CMICD_FIFODMA_CMC_CHAN;

    /* Get the encoding for host memory entries */
    for (sel = 0; sel <= CMICD_FIFODMA_SEL_MAX; sel++) {
        if (work->data->num_entries == (uint32_t)(64 << sel)) {
            break;
        }
    }
    if (sel > CMICD_FIFODMA_SEL_MAX) {
        return SHR_E_PARAM;
    }

    /* Configure FIFODMA registers */
    CMIC_CMC_FIFO_RD_DMA_CFGr_CLR(cfg);
    if (dev->flags & FIFODMA_CF_BIGENDIAN) {
        CMIC_CMC_FIFO_RD_DMA_CFGr_ENDIANESSf_SET(cfg, 1);
    }
    CMIC_CMC_FIFO_RD_DMA_CFGr_HOST_NUM_ENTRIES_SELf_SET(cfg, sel);
    CMIC_CMC_FIFO_RD_DMA_CFGr_BEAT_COUNTf_SET(cfg, work->data->data_width);
    CMIC_CMC_FIFO_RD_DMA_CFGr_NACK_FATALf_SET(cfg, 1);
    CMIC_CMC_FIFO_RD_DMA_CFGr_TIMEOUT_COUNTf_SET(cfg, 0x3fff);
    WRITE_CMIC_CMC_FIFO_RD_DMA_CFGr(unit, cmc, ch, cfg);
    CMIC_CMC_FIFO_RD_DMA_OPCODEr_OPCODEf_SET(opc, work->data->op_code);
    WRITE_CMIC_CMC_FIFO_RD_DMA_OPCODEr(unit, cmc, ch, opc);
    CMIC_CMC_FIFO_RD_DMA_SBUS_START_ADDRESSr_ADDRESSf_SET(ssa, work->data->start_addr);
    WRITE_CMIC_CMC_FIFO_RD_DMA_SBUS_START_ADDRESSr(unit, cmc, ch, ssa);
    CMIC_CMC_FIFO_RD_DMA_HOSTMEM_START_ADDRESSr_ADDRESSf_SET(hsa, dev->ctrl->fifo[chan].buf_paddr);
    WRITE_CMIC_CMC_FIFO_RD_DMA_HOSTMEM_START_ADDRESSr(unit, cmc, ch, hsa);
    CMIC_CMC_FIFO_RD_DMA_HOSTMEM_THRESHOLDr_THRESHOLDf_SET(hth, work->data->thresh_entries);
    WRITE_CMIC_CMC_FIFO_RD_DMA_HOSTMEM_THRESHOLDr(unit, cmc, ch, hth);

    /* Enable interrupt if in interrupt mode */
    if (work->flags & FIFODMA_WF_INT_MODE) {
        dev->ctrl->fifo[chan].intr_enabled = 1;
        bcmbd_cmicd_intr_enable(unit, cmc, cmicd_fifodma_intr[ch]);
    }

    /* Start FIFODMA */
    CMIC_CMC_FIFO_RD_DMA_CFGr_ENABLEf_SET(cfg, 1);
    WRITE_CMIC_CMC_FIFO_RD_DMA_CFGr(unit, cmc, ch, cfg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma started on %d/%d\n"),
                 cmc, ch));

    return SHR_E_NONE;
}

/*!
 * Stop FIFODMA device
 */
static int
cmicd_fifodma_stop(cmicd_fifodma_dev_t *dev, int chan)
{
    int unit = dev->unit;
    CMIC_CMC_FIFO_RD_DMA_CFGr_t cfg;
    CMIC_CMC_FIFO_RD_DMA_STATr_t sta;
    int cmc, ch;
    int retry = CMICD_FIFODMA_RETRY;

    cmc = chan / CMICD_FIFODMA_CMC_CHAN;
    ch = chan % CMICD_FIFODMA_CMC_CHAN;

    /* Configure FIFODMA registers */
    READ_CMIC_CMC_FIFO_RD_DMA_CFGr(unit, cmc, ch, &cfg);
    if (!CMIC_CMC_FIFO_RD_DMA_CFGr_ENABLEf_GET(cfg)) {
        return SHR_E_NONE;
    }
    CMIC_CMC_FIFO_RD_DMA_CFGr_ABORTf_SET(cfg, 1);
    WRITE_CMIC_CMC_FIFO_RD_DMA_CFGr(unit, cmc, ch, cfg);

    /* Wait abort complete */
    do {
        READ_CMIC_CMC_FIFO_RD_DMA_STATr(unit, cmc, ch, &sta);
        if (CMIC_CMC_FIFO_RD_DMA_STATr_DONEf_GET(sta)) {
            break;
        }
        sal_usleep(CMICD_FIFODMA_WAIT);
    } while (retry--);
    if (retry <= 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fifodma abort failed on %d/%d\n"), cmc, ch));
    }

    CMIC_CMC_FIFO_RD_DMA_CFGr_CLR(cfg);
    WRITE_CMIC_CMC_FIFO_RD_DMA_CFGr(unit, cmc, ch, cfg);

    /* Disable interrupt if in interrupt mode */
    if (dev->ctrl->fifo[ch].intr_enabled) {
        bcmbd_cmicd_intr_disable_deferred(unit, cmc, cmicd_fifodma_intr[ch]);
        dev->ctrl->fifo[ch].intr_enabled = 0;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma stopped on %d/%d\n"),
                 cmc, ch));

    return SHR_E_NONE;
}

/*!
 * Pop FIFODMA device
 */
static int
cmicd_fifodma_pop(cmicd_fifodma_dev_t *dev, int chan, uint32_t *ptr, uint32_t *num)
{
    int unit = dev->unit;
    fifodma_chan_t *fifo = &dev->ctrl->fifo[chan];
    CMIC_CMC_FIFO_RD_DMA_STATr_t sta;
    CMIC_CMC_FIFO_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEMr_t nov;
    CMIC_CMC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr_t nor;
    uint32_t head, tail, size, entries;
    char *bc, *ba;
    int cmc, ch;
    int rv;

    cmc = chan / CMICD_FIFODMA_CMC_CHAN;
    ch = chan % CMICD_FIFODMA_CMC_CHAN;

    /* Check status */
    READ_CMIC_CMC_FIFO_RD_DMA_STATr(unit, cmc, ch, &sta);
    rv = cmicd_fifodma_stat_check(dev, &sta);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Get ready entries */
    READ_CMIC_CMC_FIFO_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEMr(unit, cmc, ch, &nov);
    entries = CMIC_CMC_FIFO_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEMr_ENTRYCOUNTf_GET(nov);
    if (!entries) {
        return SHR_E_EMPTY;
    }

    /* Copy to buffer cache */
    tail = fifo->ptr;
    fifo->ptr = (fifo->ptr + entries) % fifo->num_entries;
    head = fifo->ptr;
    size = tail * fifo->data_width * sizeof(uint32_t);
    bc = (char *)fifo->buf_cache + size;
    ba = (char *)fifo->buf_addr + size;
    if (head > tail) {
        size = entries * fifo->data_width * sizeof(uint32_t);
        sal_memcpy(bc, ba, size);
    } else {
        size = (fifo->num_entries - tail) * fifo->data_width * sizeof(uint32_t);
        sal_memcpy(bc, ba, size);
        if (head > 0) {
            bc = (char *)fifo->buf_cache;
            ba = (char *)fifo->buf_addr;
            size = head * fifo->data_width * sizeof(uint32_t);
            sal_memcpy(bc, ba, size);
        }
    }

    /* Acknowledge ready entries */
    CMIC_CMC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr_CLR(nor);
    CMIC_CMC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr_ENTRYCOUNTf_SET(nor, entries);
    WRITE_CMIC_CMC_FIFO_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr(unit, cmc, ch, nor);

    /* Enable interrupt if in interrupt mode */
    if (fifo->intr_enabled) {
        bcmbd_cmicd_intr_enable(unit, cmc, cmicd_fifodma_intr[ch]);
    }

    *ptr = tail;
    *num = entries;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma popped %d entries on %d/%d\n"),
                 entries, cmc, ch));

    return SHR_E_NONE;
}

/*!
 * Interrupt service routine.
 */
static void
cmicd_fifodma_work_isr(int unit, int chan)
{
    cmicd_fifodma_dev_t *dev = &fifodma_dev[unit];
    fifodma_ctrl_t *ctrl = dev->ctrl;
    int cmc, ch;

    cmc = chan / CMICD_FIFODMA_CMC_CHAN;
    ch = chan % CMICD_FIFODMA_CMC_CHAN;

    /* Disable interrupt */
    bcmbd_cmicd_intr_disable_deferred(unit, cmc, cmicd_fifodma_intr[ch]);

    ctrl->intr_notify(unit, chan);
}

/*!
 * Handle CMICd FIFODMA device.
 */
static void
bcmbd_cmicd_fifodma_handle(fifodma_ctrl_t *ctrl, int chan)
{
    int unit = ctrl->unit;
    CMIC_CMC_FIFO_RD_DMA_STATr_t sta;
    CMIC_CMC_FIFO_RD_DMA_STAT_CLRr_t stc;
    int cmc, ch;

    cmc = chan / CMICD_FIFODMA_CMC_CHAN;
    ch = chan % CMICD_FIFODMA_CMC_CHAN;

    /* Clear status */
    CMIC_CMC_FIFO_RD_DMA_STAT_CLRr_CLR(stc);
    READ_CMIC_CMC_FIFO_RD_DMA_STATr(unit, cmc, ch, &sta);
    if (CMIC_CMC_FIFO_RD_DMA_STATr_HOSTMEM_OVERFLOWf_GET(sta)) {
        CMIC_CMC_FIFO_RD_DMA_STAT_CLRr_HOSTMEM_OVERFLOWf_SET(stc, 1);
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_SBUSACK_TIMEOUTf_GET(sta)) {
        CMIC_CMC_FIFO_RD_DMA_STAT_CLRr_HOSTMEM_TIMEOUTf_SET(stc, 1);
    }
    if (CMIC_CMC_FIFO_RD_DMA_STATr_ECC_2BIT_CHECK_FAILf_GET(sta)) {
        CMIC_CMC_FIFO_RD_DMA_STAT_CLRr_ECC_2BIT_CHECK_FAILf_SET(stc, 1);
    }
    if (CMIC_CMC_FIFO_RD_DMA_STAT_CLRr_GET(stc)) {
        WRITE_CMIC_CMC_FIFO_RD_DMA_STAT_CLRr(unit, cmc, ch, stc);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Fifodma interrupt handle on %d/%d\n"),
                 cmc, ch));
}

/*!
 * Start CMICd FIFODMA device.
 */
static int
bcmbd_cmicd_fifodma_start(fifodma_ctrl_t *ctrl, int chan)
{
    cmicd_fifodma_dev_t *dev = &fifodma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicd_fifodma_start(dev, chan);
}

/*!
 * Stop CMICd FIFODMA device.
 */
static int
bcmbd_cmicd_fifodma_stop(fifodma_ctrl_t *ctrl, int chan)
{
    cmicd_fifodma_dev_t *dev = &fifodma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicd_fifodma_stop(dev, chan);
}

/*!
 * Pop CMICd FIFODMA device.
 */
static int
bcmbd_cmicd_fifodma_pop(fifodma_ctrl_t *ctrl, int chan, uint32_t *ptr, uint32_t *num)
{
    cmicd_fifodma_dev_t *dev = &fifodma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    return cmicd_fifodma_pop(dev, chan, ptr, num);
}

static const fifodma_ops_t cmicd_fifodma_ops = {
    .fifodma_handle     = bcmbd_cmicd_fifodma_handle,
    .fifodma_start      = bcmbd_cmicd_fifodma_start,
    .fifodma_stop       = bcmbd_cmicd_fifodma_stop,
    .fifodma_pop        = bcmbd_cmicd_fifodma_pop
};

/*!
 * Initialize FIFODMA device
 */
int
bcmbd_cmicd_fifodma_init(int unit)
{
    fifodma_ctrl_t *ctrl = bcmbd_fifodma_ctrl_get(unit);
    cmicd_fifodma_dev_t *dev = &fifodma_dev[unit];
    int cmc, ch;

    SHR_FUNC_ENTER(unit);

    if (!ctrl || !ctrl->active) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(dev, 0, sizeof(*dev));
    dev->unit = unit;
    dev->ctrl = ctrl;
    dev->bm_cmc = 0x1 & CMICD_FIFODMA_CMC_MASK; /* Only use CMC0 for now */

    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        dev->flags |= FIFODMA_CF_BIGENDIAN;
    }

    /* Resolve all the available channels */
    for (cmc = 0; cmc < CMICD_FIFODMA_CMC_MAX; cmc++) {
        if (1 << cmc & dev->bm_cmc) {
            dev->bm_chan |= CMICD_FIFODMA_CHAN_MASK << (cmc * CMICD_FIFODMA_CMC_CHAN);
            for (ch = 0; ch < CMICD_FIFODMA_CMC_CHAN; ch++) {
                /* Connect respective interrupt */
                bcmbd_cmicd_intr_func_set(unit, cmc, cmicd_fifodma_intr[ch],
                                          (bcmbd_intr_f)cmicd_fifodma_work_isr,
                                          ch + cmc * CMICD_FIFODMA_CMC_CHAN);
            }
        }
    }

    ctrl->ops = (fifodma_ops_t *)&cmicd_fifodma_ops;

    dev->inited = 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Clean up FIFODMA device
 */
int
bcmbd_cmicd_fifodma_cleanup(int unit)
{
    fifodma_ctrl_t *ctrl = bcmbd_fifodma_ctrl_get(unit);
    cmicd_fifodma_dev_t *dev = &fifodma_dev[unit];
    int cmc, ch;

    if (!ctrl) {
        return SHR_E_UNAVAIL;
    }

    if (!dev->inited) {
        return SHR_E_NONE;
    }

    dev->inited = 0;

    ctrl->ops = NULL;

    /* Disable all interrupts and channels */
    for (cmc = 0; cmc < CMICD_FIFODMA_CMC_MAX; cmc++) {
        if (1 << cmc & dev->bm_cmc) {
            for (ch = 0; ch < CMICD_FIFODMA_CMC_CHAN; ch++) {
                if (ctrl->fifo[ch].intr_enabled) {
                    bcmbd_cmicd_intr_disable_deferred(unit, cmc, cmicd_fifodma_intr[ch]);
                    ctrl->fifo[ch].intr_enabled = 0;
                }
                if (ctrl->fifo[ch].started) {
                    cmicd_fifodma_stop(dev, ch + cmc * CMICD_FIFODMA_CMC_CHAN);
                }
            }
        }
    }

    return SHR_E_NONE;
}

