/*! \file bcmbd_cmicx_sbusdma.c
 *
 * CMICx SBUSDMA routines
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
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_internal.h>
#include "bcmbd_cmicx_sbusdma.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SBUSDMA


#define BCMBD_CMICX_SLAVE_OFFSET        BCMBD_CMICX_PCIE_SLAVE_OFFSET

static cmicx_sbusdma_dev_t sbusdma_dev[SBUSDMA_DEV_NUM_MAX];

static uint32_t
cmicx_sbusdma_intr[CMICX_SBUSDMA_CMC_MAX][CMICX_SBUSDMA_CMC_CHAN] = {
    {
        CMICX_IRQ_CMC0_SBUSDMA_CH0_DONE,
        CMICX_IRQ_CMC0_SBUSDMA_CH1_DONE,
        CMICX_IRQ_CMC0_SBUSDMA_CH2_DONE,
        CMICX_IRQ_CMC0_SBUSDMA_CH3_DONE
    },
    {
        CMICX_IRQ_CMC1_SBUSDMA_CH0_DONE,
        CMICX_IRQ_CMC1_SBUSDMA_CH1_DONE,
        CMICX_IRQ_CMC1_SBUSDMA_CH2_DONE,
        CMICX_IRQ_CMC1_SBUSDMA_CH3_DONE
    }
};

/*******************************************************************************
 * Private functions
 */

/*!
 * Allocate a SBUSDMA channel
 */
inline static int
cmicx_sbusdma_chan_get(cmicx_sbusdma_dev_t *dev, int *cmc, int *chan)
{
    int ch;

    while (1) {
        sal_spinlock_lock(dev->lock);
        if (!dev->bm_cmc) {
            sal_spinlock_unlock(dev->lock);
            return SHR_E_UNAVAIL;
        }
        for (ch = 0; ch < CMICX_SBUSDMA_CHAN_MAX; ch++) {
            if ((1 << ch) & dev->bm_chan) {
                *cmc = ch / CMICX_SBUSDMA_CMC_CHAN;
                *chan = ch % CMICX_SBUSDMA_CMC_CHAN;
                dev->bm_chan &= ~(1 << ch);
                sal_spinlock_unlock(dev->lock);
                return SHR_E_NONE;
            }
        }
        dev->status |= SBUSDMA_DS_CHAN_BUSY;
        sal_spinlock_unlock(dev->lock);
        sal_sem_take(dev->sem, SAL_SEM_FOREVER);
    }
}

/*!
 * Release a SBUSDMA channel
 */
inline static int
cmicx_sbusdma_chan_put(cmicx_sbusdma_dev_t *dev, int cmc, int chan)
{
    int ch;

    if ((cmc < 0 || cmc >= CMICX_SBUSDMA_CMC_MAX) ||
        (chan < 0 || chan >= CMICX_SBUSDMA_CHAN_MAX)) {
        return SHR_E_PARAM;
    }

    sal_spinlock_lock(dev->lock);
    ch = chan + cmc * CMICX_SBUSDMA_CMC_CHAN;
    dev->bm_chan |= 1 << ch;
    if (dev->status & SBUSDMA_DS_CHAN_BUSY) {
        dev->status &= ~SBUSDMA_DS_CHAN_BUSY;
        sal_spinlock_unlock(dev->lock);
        sal_sem_give(dev->sem);
        return SHR_E_NONE;
    }
    sal_spinlock_unlock(dev->lock);

    return SHR_E_NONE;
}

/*!
 * Set up SBUSDMA configurations
 */
static void
cmicx_sbusdma_data_setup(cmicx_sbusdma_dev_t *dev,
                         bcmbd_sbusdma_work_t *work,
                         bcmbd_sbusdma_data_t *data,
                         cmicx_sbusdma_desc_t *desc)
{
    CMIC_CMC_SBUSDMA_REQUESTr_t req;

    /* Set up the request word */
    CMIC_CMC_SBUSDMA_REQUESTr_CLR(req);
    if (work->flags & SBUSDMA_WF_DEC_ADDR) {
        CMIC_CMC_SBUSDMA_REQUESTr_DECRf_SET(req, 1);
    }
    if (work->flags & SBUSDMA_WF_SGL_DATA) {
        CMIC_CMC_SBUSDMA_REQUESTr_REQ_SINGLEf_SET(req, 1);
    }
    if (work->flags & SBUSDMA_WF_SGL_ADDR) {
        CMIC_CMC_SBUSDMA_REQUESTr_INCR_NOADDf_SET(req, 1);
    }
    CMIC_CMC_SBUSDMA_REQUESTr_INCR_SHIFTf_SET(req, data->addr_gap);
    if (data->pend_clocks) {
        CMIC_CMC_SBUSDMA_REQUESTr_IGNORE_SBUS_EARLYACKf_SET(req, 1);
        CMIC_CMC_SBUSDMA_REQUESTr_PEND_CLOCKSf_SET(req, data->pend_clocks);
    }
    if (dev->flags & SBUSDMA_CF_WR_BIGENDIAN) {
        CMIC_CMC_SBUSDMA_REQUESTr_HOSTMEMWR_ENDIANESSf_SET(req, 1);
    }
    if (dev->flags & SBUSDMA_CF_RD_BIGENDIAN) {
        CMIC_CMC_SBUSDMA_REQUESTr_HOSTMEMRD_ENDIANESSf_SET(req, 1);
    }
    if (work->flags & SBUSDMA_WF_WRITE_CMD) {
        CMIC_CMC_SBUSDMA_REQUESTr_REQ_WORDSf_SET(req, data->data_width);
    } else {
        CMIC_CMC_SBUSDMA_REQUESTr_REP_WORDSf_SET(req, data->data_width);
    }

    /* Configure the descriptor */
    desc->request = req.v[0];
    desc->count = data->op_count;
    desc->opcode = data->op_code;
    desc->address = data->start_addr;
    desc->hostaddr_lo = (uint32_t)data->buf_paddr;
    desc->hostaddr_hi = ((uint32_t)(data->buf_paddr >> 32)) |
                        BCMBD_CMICX_SLAVE_OFFSET;
    desc->reserved = 0;
}

/*!
 * Report SBUSDMA status
 */
static void
cmicx_sbusdma_status_report(cmicx_sbusdma_dev_t *dev, int cmc, int ch)
{
    int unit = dev->unit;
    CMIC_CMC_SBUSDMA_STATUSr_t stat;
    CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_REQUESTr_t creq;
    CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_COUNTr_t ccnt;
    CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_OPCODEr_t copc;
    CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESSr_t cssa;
    CMIC_CMC_SBUSDMA_CUR_SBUS_ADDRESSr_t csa;

    READ_CMIC_CMC_SBUSDMA_STATUSr(unit, cmc, ch, &stat);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "STATUS: 0x%08x\n"), stat.v[0]));
    READ_CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_REQUESTr(unit, cmc, ch, &creq);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "CONFIG REQUEST: 0x%08x\n"), creq.v[0]));
    READ_CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_COUNTr(unit, cmc, ch, &ccnt);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "CONFIG COUNT: 0x%08x\n"), ccnt.v[0]));
    READ_CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_OPCODEr(unit, cmc, ch, &copc);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "CONFIG OPCODE: 0x%08x\n"), copc.v[0]));
    READ_CMIC_CMC_SBUSDMA_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESSr(unit, cmc, ch,
                                                                 &cssa);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "CONFIG START ADDR: 0x%08x\n"), cssa.v[0]));
    READ_CMIC_CMC_SBUSDMA_CUR_SBUS_ADDRESSr(unit, cmc, ch, &csa);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "CUR SBUS ADDR: 0x%08x\n"), csa.v[0]));
}

/*!
 * Print SBUSDMA error information
 */
static void
cmicx_sbusdma_error_report(cmicx_sbusdma_dev_t *dev,
                           CMIC_CMC_SBUSDMA_STATUSr_t stat)
{
    int unit = dev->unit;

    if (CMIC_CMC_SBUSDMA_STATUSr_DESCRD_ERRORf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error while reading descriptor from "
                              "host memory.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_SBUSACK_TIMEOUTf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "sbus ack not received within configured "
                              "time.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_SBUSACK_ERRORf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus ack with error "
                              "bit set.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_SBUSACK_NACKf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus nack with error "
                              "bit set.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_SBUSACK_WRONG_OPCODEf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack has wrong opcode.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_SBUSACK_WRONG_BEATCOUNTf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack data size is not the same as "
                              "in rep_words fields.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_SER_CHECK_FAILf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack with SER_CHECK_FAIL set.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_HOSTMEMRD_ERRORf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Error while copying SBUSDMA data from "
                              "Host Memory.\n")));
    }
    if (CMIC_CMC_SBUSDMA_STATUSr_HOSTMEMWR_ERRORf_GET(stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "DMA operation encountered a schan response "
                              "error or host side error.\n")));
    }
}

/*!
 * Reset SBUSDMA channel
 */
static void
cmicx_sbusdma_chan_reset(cmicx_sbusdma_dev_t *dev, int cmc, int ch)
{
    int unit = dev->unit;
    CMIC_CMC_SBUSDMA_CONTROLr_t ctl;
    CMIC_CMC_SBUSDMA_STATUSr_t stat;
    int retry = CMICX_SBUSDMA_RETRY;

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        return;
    }

    /* Configure the SBUSDMA registers */
    CMIC_CMC_SBUSDMA_CONTROLr_CLR(ctl);
    CMIC_CMC_SBUSDMA_CONTROLr_STARTf_SET(ctl, 1);
    CMIC_CMC_SBUSDMA_CONTROLr_ABORTf_SET(ctl, 1);
    if (dev->flags & SBUSDMA_CF_DESC_BIGENDIAN) {
        CMIC_CMC_SBUSDMA_CONTROLr_DESCRIPTOR_ENDIANESSf_SET(ctl, 1);
    }
    WRITE_CMIC_CMC_SBUSDMA_CONTROLr(unit, cmc, ch, ctl);

    /* Wait the reset complete */
    do {
        READ_CMIC_CMC_SBUSDMA_STATUSr(unit, cmc, ch, &stat);
        if (CMIC_CMC_SBUSDMA_STATUSr_DONEf_GET(stat)) {
            break;
        }
        sal_usleep(CMICX_SBUSDMA_WAIT);
    } while (retry--);
    if (retry <= 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Sbusdma abort failed on %d/%d\n"),
                   cmc, ch));
    }

    CMIC_CMC_SBUSDMA_CONTROLr_CLR(ctl);
    WRITE_CMIC_CMC_SBUSDMA_CONTROLr(unit, cmc, ch, ctl);
}

/*!
 * Wait for SBUSDMA operation complete
 */
static int
cmicx_sbusdma_op_wait(cmicx_sbusdma_dev_t *dev,
                      bcmbd_sbusdma_work_t *work,
                      int cmc,
                      int ch)
{
    int unit = dev->unit;
    CMIC_CMC_SBUSDMA_STATUSr_t stat;
    CMIC_CMC_SBUSDMA_CONTROLr_t ctl;
    int retry = CMICX_SBUSDMA_RETRY;
    int rv = SHR_E_NONE;
    int wait = SECOND_USEC;

    if (work->flags & SBUSDMA_WF_INT_MODE) {
        /* Wait the interrupt raise */
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL)) {
            wait *= 100;
        }
        if (sal_sem_take(dev->ctrl->intr[ch + cmc * CMICX_SBUSDMA_CMC_CHAN],
                         wait)) {
            /* The SBUSDMA operation timeout */
            work->state = SBUSDMA_WORK_TIMEOUT;
            rv = SHR_E_TIMEOUT;
        } else {
            READ_CMIC_CMC_SBUSDMA_STATUSr(unit, cmc, ch, &stat);
            if (CMIC_CMC_SBUSDMA_STATUSr_DONEf_GET(stat)) {
                if (CMIC_CMC_SBUSDMA_STATUSr_ERRORf_GET(stat)) {
                    /* Report the details if the SBUSDMA operation failed */
                    cmicx_sbusdma_error_report(dev, stat);
                    work->state = SBUSDMA_WORK_ERROR;
                    rv = SHR_E_FAIL;
                }
            } else {
                /* The SBUSDMA operation timeout */
                work->state = SBUSDMA_WORK_TIMEOUT;
                rv = SHR_E_TIMEOUT;
            }
            CMIC_CMC_SBUSDMA_CONTROLr_CLR(ctl);
            WRITE_CMIC_CMC_SBUSDMA_CONTROLr(unit, cmc, ch, ctl);
        }
    } else {
        /* Poll the transaction done indicator */
        while (retry--) {
            READ_CMIC_CMC_SBUSDMA_STATUSr(unit, cmc, ch, &stat);
            if (CMIC_CMC_SBUSDMA_STATUSr_DONEf_GET(stat)) {
                if (CMIC_CMC_SBUSDMA_STATUSr_ERRORf_GET(stat)) {
                    /* Report the details if the SBUSDMA operation failed */
                    cmicx_sbusdma_error_report(dev, stat);
                    work->state = SBUSDMA_WORK_ERROR;
                    rv = SHR_E_FAIL;
                }
                break;
            }
            sal_usleep(CMICX_SBUSDMA_WAIT);
        }
        if (retry <= 0) {
            /* The SBUSDMA operation timeout */
            work->state = SBUSDMA_WORK_TIMEOUT;
            rv = SHR_E_TIMEOUT;
        }
    }

    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_TIMEOUT) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Sbusdma timeout on %d/%d\n"),
                       cmc, ch));

            /* Reset the SBUSDMA channel if timeout happened */
            cmicx_sbusdma_chan_reset(dev, cmc, ch);
        }

        /* Report the SBUSDMA operation status */
        cmicx_sbusdma_status_report(dev, cmc, ch);
    } else {
        work->state = SBUSDMA_WORK_DONE;
    }

    return rv;
}

/*!
 * Operate in SBUSDMA register mode
 */
static int
cmicx_sbusdma_reg_op(cmicx_sbusdma_dev_t *dev, bcmbd_sbusdma_work_t *work)
{
    int unit = dev->unit;
    CMIC_CMC_SBUSDMA_CONTROLr_t ctl;
    CMIC_CMC_SBUSDMA_REQUESTr_t req;
    CMIC_CMC_SBUSDMA_COUNTr_t cnt;
    CMIC_CMC_SBUSDMA_OPCODEr_t opc;
    CMIC_CMC_SBUSDMA_SBUS_START_ADDRESSr_t ssa;
    CMIC_CMC_SBUSDMA_HOSTMEM_START_ADDRESS_LOr_t hsal;
    CMIC_CMC_SBUSDMA_HOSTMEM_START_ADDRESS_HIr_t hsah;
    cmicx_sbusdma_desc_t desc = {0};
    int cmc, ch;
    int rv;

    /* Allocate a SBUSDMA channel */
    rv = cmicx_sbusdma_chan_get(dev, &cmc, &ch);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Configure the SBUSDMA registers */
    CMIC_CMC_SBUSDMA_CONTROLr_CLR(ctl);
    if (dev->flags & SBUSDMA_CF_DESC_BIGENDIAN) {
        CMIC_CMC_SBUSDMA_CONTROLr_DESCRIPTOR_ENDIANESSf_SET(ctl, 1);
    }
    WRITE_CMIC_CMC_SBUSDMA_CONTROLr(unit, cmc, ch, ctl);

    /* Prepare the configuration data */
    cmicx_sbusdma_data_setup(dev, work, work->data, &desc);

    CMIC_CMC_SBUSDMA_REQUESTr_SET(req, desc.request);
    WRITE_CMIC_CMC_SBUSDMA_REQUESTr(unit, cmc, ch, req);
    CMIC_CMC_SBUSDMA_COUNTr_SET(cnt, desc.count);
    WRITE_CMIC_CMC_SBUSDMA_COUNTr(unit, cmc, ch, cnt);
    CMIC_CMC_SBUSDMA_OPCODEr_OPCODEf_SET(opc, desc.opcode);
    WRITE_CMIC_CMC_SBUSDMA_OPCODEr(unit, cmc, ch, opc);
    CMIC_CMC_SBUSDMA_SBUS_START_ADDRESSr_SET(ssa, desc.address);
    WRITE_CMIC_CMC_SBUSDMA_SBUS_START_ADDRESSr(unit, cmc, ch, ssa);
    CMIC_CMC_SBUSDMA_HOSTMEM_START_ADDRESS_LOr_SET(hsal, desc.hostaddr_lo);
    WRITE_CMIC_CMC_SBUSDMA_HOSTMEM_START_ADDRESS_LOr(unit, cmc, ch, hsal);
    CMIC_CMC_SBUSDMA_HOSTMEM_START_ADDRESS_HIr_SET(hsah, desc.hostaddr_hi);
    WRITE_CMIC_CMC_SBUSDMA_HOSTMEM_START_ADDRESS_HIr(unit, cmc, ch, hsah);

    /* Enable the interrupt if in interrupt mode */
    if (work->flags & SBUSDMA_WF_INT_MODE) {
        bcmbd_cmicx_intr_enable(unit, cmicx_sbusdma_intr[cmc][ch]);
    }

    /* Start the SBUSDMA operation */
    CMIC_CMC_SBUSDMA_CONTROLr_STARTf_SET(ctl, 1);
    WRITE_CMIC_CMC_SBUSDMA_CONTROLr(unit, cmc, ch, ctl);

    work->state = SBUSDMA_WORK_STARTED;

    /* Wait the SBUSDMA operation complete */
    rv = cmicx_sbusdma_op_wait(dev, work, cmc, ch);

    /* Release the SBUSDMA channel */
    cmicx_sbusdma_chan_put(dev, cmc, ch);

    return rv;
}

/*!
 * Operate in SBUSDMA descriptor mode
 */
static int
cmicx_sbusdma_desc_op(cmicx_sbusdma_dev_t *dev, bcmbd_sbusdma_work_t *work)
{
    int unit = dev->unit;
    CMIC_CMC_SBUSDMA_CONTROLr_t ctl;
    CMIC_CMC_SBUSDMA_DESC_START_ADDRESS_LOr_t dsal;
    CMIC_CMC_SBUSDMA_DESC_START_ADDRESS_HIr_t dsah;
    cmicx_sbusdma_desc_addr_t *da = NULL;
    uint32_t paddr32;
    int cmc, ch;
    int rv;

    /* Allocate a SBUSDMA channel */
    rv = cmicx_sbusdma_chan_get(dev, &cmc, &ch);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    da = (cmicx_sbusdma_desc_addr_t *)work->desc;

    /* Configure the SBUSDMA registers */
    CMIC_CMC_SBUSDMA_CONTROLr_CLR(ctl);
    CMIC_CMC_SBUSDMA_CONTROLr_MODEf_SET(ctl, 1);
    if (dev->flags & SBUSDMA_CF_DESC_BIGENDIAN) {
        CMIC_CMC_SBUSDMA_CONTROLr_DESCRIPTOR_ENDIANESSf_SET(ctl, 1);
    }
    /* Always enable descriptor prefetch for performance optimization */
    CMIC_CMC_SBUSDMA_CONTROLr_DESC_PREFETCH_ENABLEf_SET(ctl, 1);
    WRITE_CMIC_CMC_SBUSDMA_CONTROLr(unit, cmc, ch, ctl);

    paddr32 = (uint32_t)da->paddr;
    CMIC_CMC_SBUSDMA_DESC_START_ADDRESS_LOr_SET(dsal, paddr32);
    WRITE_CMIC_CMC_SBUSDMA_DESC_START_ADDRESS_LOr(unit, cmc, ch, dsal);

    paddr32 = ((uint32_t)(da->paddr >> 32)) | BCMBD_CMICX_SLAVE_OFFSET;
    CMIC_CMC_SBUSDMA_DESC_START_ADDRESS_HIr_SET(dsah, paddr32);
    WRITE_CMIC_CMC_SBUSDMA_DESC_START_ADDRESS_HIr(unit, cmc, ch, dsah);

    /* Enable the interrupt if in interrupt mode */
    if (work->flags & SBUSDMA_WF_INT_MODE) {
        bcmbd_cmicx_intr_enable(unit, cmicx_sbusdma_intr[cmc][ch]);
    }

    /* Start the SBUSDMA operation */
    CMIC_CMC_SBUSDMA_CONTROLr_STARTf_SET(ctl, 1);
    WRITE_CMIC_CMC_SBUSDMA_CONTROLr(unit, cmc, ch, ctl);

    work->state = SBUSDMA_WORK_STARTED;

    /* Wait the SBUSDMA operation complete */
    rv = cmicx_sbusdma_op_wait(dev, work, cmc, ch);

    /* Release the SBUSDMA channel */
    cmicx_sbusdma_chan_put(dev, cmc, ch);

    return rv;
}

/*!
 * Interrupt service routine
 */
static void
cmicx_sbusdma_work_isr(int unit, uint32_t queue)
{
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[unit];
    sbusdma_ctrl_t *ctrl = dev->ctrl;
    int cmc, ch;

    /* Disable the interrupt */
    cmc = queue / CMICX_SBUSDMA_CMC_CHAN;
    ch = queue % CMICX_SBUSDMA_CMC_CHAN;
    bcmbd_cmicx_intr_disable_deferred(unit, cmicx_sbusdma_intr[cmc][ch]);

    sal_sem_give(ctrl->intr[queue]);
}

/*!
 * Initialize light work
 */
static int
bcmbd_cmicx_sbusdma_light_work_init(sbusdma_ctrl_t *ctrl,
                                    bcmbd_sbusdma_work_t *work)
{
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[ctrl->unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    /* Initialize the light work */
    work->desc = NULL;
    work->link = NULL;
    work->state = SBUSDMA_WORK_INITED;

    return SHR_E_NONE;
}

/*!
 * Create batch work
 */
static int
bcmbd_cmicx_sbusdma_batch_work_create(sbusdma_ctrl_t *ctrl,
                                      bcmbd_sbusdma_work_t *work)
{
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[ctrl->unit];
    cmicx_sbusdma_desc_addr_t *da = NULL;
    cmicx_sbusdma_desc_t *desc = NULL;
    uint64_t jump_paddr;
    uint8_t remn;
    int i;

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    /* Make sure it is batch work */
    if (work->items <= 0) {
        return SHR_E_UNAVAIL;
    }

    /* Allocate resources for the batch work */
    work->desc = sal_alloc(sizeof(cmicx_sbusdma_desc_addr_t),
                           "bcmbdSbusDmaDesc");
    if (!work->desc) {
        return SHR_E_MEMORY;
    }
    da = (cmicx_sbusdma_desc_addr_t *)work->desc;
    da->size = sizeof(cmicx_sbusdma_desc_t) * work->items;
    da->addr = bcmdrd_hal_dma_alloc(dev->unit, da->size, "bcmbdSbusDmaDesc",
                                    &da->paddr);
    if (!da->addr) {
        sal_free(work->desc);
        return SHR_E_MEMORY;
    }
    sal_memset(da->addr, 0, da->size);

    /* Configure the SBUSDMA descriptors */
    desc = (cmicx_sbusdma_desc_t *)da->addr;
    for (i = 0; i < work->items; i++, desc++) {
        cmicx_sbusdma_data_setup(dev, work, &work->data[i], desc);
        remn = (work->items - i) > (int)CMICX_SBUSDMA_DESC_BURST_MAX ?
               CMICX_SBUSDMA_DESC_BURST_MAX : (uint8_t)(work->items - i - 1);
        desc->control |= CMICX_SBUSDMA_DESC_CTRL_REMN(remn);
        if (i == (work->items - 1)) {
            desc->control |= CMICX_SBUSDMA_DESC_CTRL_LAST;
        }
        if ((work->data[i].attrs & SBUSDMA_DA_APND_BUF) && (i != 0)) {
            desc->control |= CMICX_SBUSDMA_DESC_CTRL_APND;
        }
        if (work->data[i].attrs & SBUSDMA_DA_JUMP_TO) {
            if ((work->data[i].jump_item > i) &&
                (work->data[i].jump_item < work->items)) {
                jump_paddr = da->paddr +
                    (work->data[i].jump_item * sizeof(cmicx_sbusdma_desc_t));
                desc->hostaddr_lo = (uint32_t)jump_paddr;
                desc->hostaddr_hi = (uint32_t)(jump_paddr >> 32);
                desc->control |= CMICX_SBUSDMA_DESC_CTRL_JUMP;
            } else {
                bcmdrd_hal_dma_free(dev->unit, da->size, da->addr, da->paddr);
                sal_free(work->desc);
                work->desc = NULL;
                return SHR_E_UNAVAIL;
            }
        }
        if (work->data[i].attrs & SBUSDMA_DA_SKIP_OVER) {
            if (i != work->items - 1) {
                desc->control |= CMICX_SBUSDMA_DESC_CTRL_SKIP;
            } else {
                bcmdrd_hal_dma_free(dev->unit, da->size, da->addr, da->paddr);
                sal_free(work->desc);
                work->desc = NULL;
                return SHR_E_UNAVAIL;
            }
        }
    }

    work->link = NULL;
    work->state = SBUSDMA_WORK_CREATED;

    return SHR_E_NONE;
}

/*!
 * Delete batch work
 */
static int
bcmbd_cmicx_sbusdma_batch_work_delete(sbusdma_ctrl_t *ctrl,
                                      bcmbd_sbusdma_work_t *work)
{
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[ctrl->unit];
    cmicx_sbusdma_desc_addr_t *da = NULL;
    int retry = CMICX_SBUSDMA_RETRY * 10;

    /* Wait till the batch work can be deleted */
    while (work->state >= SBUSDMA_WORK_QUEUED &&
           work->state <= SBUSDMA_WORK_STARTED &&
           retry--) {
        sal_usleep(CMICX_SBUSDMA_WAIT);
    }
    if (retry <= 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(dev->unit, "Cannot delete the pended work.\n")));
        return SHR_E_INTERNAL;
    }

    /* Delete the batch work */
    if (work->desc) {
        da = (cmicx_sbusdma_desc_addr_t *)work->desc;
        bcmdrd_hal_dma_free(dev->unit, da->size, da->addr, da->paddr);
        sal_free(work->desc);
        work->desc = NULL;
    }

    work->state = SBUSDMA_WORK_DESTROYED;

    return SHR_E_NONE;
}

/*!
 * Execute work
 */
static int
bcmbd_cmicx_sbusdma_work_execute(sbusdma_ctrl_t *ctrl,
                                 bcmbd_sbusdma_work_t *work)
{
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[ctrl->unit];
    int rv;

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    if (!work->desc) {
        /* Execute the light work */
        rv = cmicx_sbusdma_reg_op(dev, work);
    } else {
        /* Execute the batch work */
        rv = cmicx_sbusdma_desc_op(dev, work);
    }

    return rv;
}

static const sbusdma_ops_t cmicx_sbusdma_ops = {
    .light_work_init        = bcmbd_cmicx_sbusdma_light_work_init,
    .batch_work_create      = bcmbd_cmicx_sbusdma_batch_work_create,
    .batch_work_delete      = bcmbd_cmicx_sbusdma_batch_work_delete,
    .work_execute           = bcmbd_cmicx_sbusdma_work_execute,
};

/*******************************************************************************
 * Public Functions
 */

/*!
 * Execute light work
 */
int
bcmbd_cmicx_sbusdma_reg_op(int unit, bcmbd_sbusdma_work_t *work)
{
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[unit];

    if (!dev->inited) {
        return SHR_E_UNAVAIL;
    }

    /* Initialize the light work */
    work->desc = NULL;
    work->link = NULL;
    work->state = SBUSDMA_WORK_INITED;

    return cmicx_sbusdma_reg_op(dev, work);
}

/*!
 * Initialize SBUSDMA device
 */
int
bcmbd_cmicx_sbusdma_init(int unit)
{
    sbusdma_ctrl_t *ctrl = bcmbd_sbusdma_ctrl_get(unit);
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[unit];
    int cmc, ch;

    SHR_FUNC_ENTER(unit);

    if (!ctrl || !ctrl->active) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(dev, 0, sizeof(*dev));
    dev->unit = unit;
    dev->ctrl = ctrl;
    dev->bm_cmc = CMICX_SBUSDMA_CMC_MASK;

    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        dev->flags |= SBUSDMA_CF_DESC_BIGENDIAN |
                      SBUSDMA_CF_WR_BIGENDIAN |
                      SBUSDMA_CF_RD_BIGENDIAN;
    }

    /* Resolve all the available channels */
    for (cmc = 0; cmc < CMICX_SBUSDMA_CMC_MAX; cmc++) {
        if (1 << cmc & dev->bm_cmc) {
            dev->bm_chan |=
                (CMICX_SBUSDMA_CHAN_MASK << (cmc * CMICX_SBUSDMA_CMC_CHAN));
            for (ch = 0; ch < CMICX_SBUSDMA_CMC_CHAN; ch++) {
                /* Connect respective interrupt */
                bcmbd_cmicx_intr_func_set(unit,
                                          cmicx_sbusdma_intr[cmc][ch],
                                          (bcmbd_intr_f)cmicx_sbusdma_work_isr,
                                          ch + cmc * CMICX_SBUSDMA_CMC_CHAN);
            }
        }
    }

    dev->sem = sal_sem_create("sbusdma_dev_sem", SAL_SEM_BINARY, 0);
    if (!dev->sem) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    dev->lock = sal_spinlock_create("sbusdma_dev_lock");
    if (!dev->lock) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    dev->inited = 1;

    ctrl->bm_grp = dev->bm_cmc;
    ctrl->bm_que = dev->bm_chan;
    ctrl->ops = (sbusdma_ops_t *)&cmicx_sbusdma_ops;

exit:
    if (SHR_FUNC_ERR()) {
        if (dev->lock) {
            sal_spinlock_destroy(dev->lock);
            dev->lock = NULL;
        }

        if (dev->sem) {
            sal_sem_destroy(dev->sem);
            dev->sem = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * Cleanup SBUSDMA device
 */
int
bcmbd_cmicx_sbusdma_cleanup(int unit)
{
    sbusdma_ctrl_t *ctrl = bcmbd_sbusdma_ctrl_get(unit);
    cmicx_sbusdma_dev_t *dev = &sbusdma_dev[unit];
    int cmc, ch;
    int retry = CMICX_SBUSDMA_RETRY * 10;

    if (!ctrl) {
        return SHR_E_UNAVAIL;
    }

    if (!dev->inited) {
        return SHR_E_NONE;
    }

    dev->inited = 0;

    /* Drain all channels */
    sal_spinlock_lock(dev->lock);
    dev->bm_cmc = 0;
    while (dev->bm_chan != ctrl->bm_que && retry--) {
        sal_spinlock_unlock(dev->lock);
        sal_usleep(CMICX_SBUSDMA_WAIT);
        sal_spinlock_lock(dev->lock);
    }
    sal_spinlock_unlock(dev->lock);
    if (retry <= 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Cannot clean up all channels.\n")));
    }

    /* Disable all interrupts */
    for (cmc = 0; cmc < CMICX_SBUSDMA_CMC_MAX; cmc++) {
        if (1 << cmc & ctrl->bm_grp) {
            for (ch = 0; ch < CMICX_SBUSDMA_CMC_CHAN; ch++) {
                bcmbd_cmicx_intr_disable_deferred(unit,
                                                  cmicx_sbusdma_intr[cmc][ch]);
            }
        }
    }

    if (dev->lock) {
        sal_spinlock_destroy(dev->lock);
        dev->lock = NULL;
    }

    if (dev->sem) {
        sal_sem_destroy(dev->sem);
        dev->sem = NULL;
    }

    return SHR_E_NONE;
}

