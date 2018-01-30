/*! \file bcmbd_cmicx_schanfifo.c
 *
 * CMICx SCHAN FIFO routines
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
#include <sal/sal_time.h>
#include <sal/sal_sem.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_schanfifo.h>
#include <bcmbd/bcmbd_internal.h>
#include "bcmbd_cmicx_schanfifo.h"

/* BSL source for SCHAN FIFO */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SCHANFIFO


#define BCMBD_CMICX_SLAVE_OFFSET        BCMBD_CMICX_PCIE_SLAVE_OFFSET

/* Interrupt number */
#define BCMBD_CMICX_SCHANFIFO_INTR      CMICX_IRQ_SCHAN_FIFO_CH0_DONE
#define CMICX_SCHANFIFO_INTR_NUM(chan)  (BCMBD_CMICX_SCHANFIFO_INTR + chan)

static cmicx_schanfifo_dev_t schanfifo_dev[SCHANFIFO_DEV_NUM_MAX];

/*!
 * Reset SCHAN FIFO channel.
 */
static void
cmicx_schanfifo_chan_reset(cmicx_schanfifo_dev_t *dev, int ch)
{
    CMIC_SCHANFIFO_CTRLr_t ctl;
    CMIC_SCHANFIFO_STATUSr_t stat;
    int retry = dev->polls;

    READ_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, &ctl);
    CMIC_SCHANFIFO_CTRLr_STARTf_SET(ctl, 1);
    CMIC_SCHANFIFO_CTRLr_ABORTf_SET(ctl, 1);
    WRITE_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, ctl);

    /* Wait for completion */
    while (retry--) {
        READ_CMIC_SCHANFIFO_STATUSr(dev->unit, ch, &stat);
        if (CMIC_SCHANFIFO_STATUSr_DONEf_GET(stat)) {
            break;
        }
    }
    if (retry < 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(dev->unit, "Schan fifo abort failed on chan%d\n"),
                   ch));
    }

    CMIC_SCHANFIFO_CTRLr_CLR(ctl);
    WRITE_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, ctl);
}

/*!
 * Configure SCHAN FIFO registers.
 */
static void
cmicx_schanfifo_reg_config(cmicx_schanfifo_dev_t *dev, int ch)
{
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_UPPERr_t rau;
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_LOWERr_t ral;
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_UPPERr_t sau;
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_LOWERr_t sal;

    /* Set host memory address for response messages */
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_UPPERr_SET(rau, BCMBD_CMICX_SLAVE_OFFSET |
                                                                             dev->resp_paddr[ch] >> 32);
    WRITE_CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_UPPERr(dev->unit, ch, rau);
    CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_LOWERr_SET(ral, dev->resp_paddr[ch]);
    WRITE_CMIC_SCHANFIFO_RESP_HOSTMEM_START_ADDR_LOWERr(dev->unit, ch, ral);

    /* Set host memory address for summary messages */
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_UPPERr_SET(sau, BCMBD_CMICX_SLAVE_OFFSET |
                                                                                dev->sum_paddr[ch] >> 32);
    WRITE_CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_UPPERr(dev->unit, ch, sau);
    CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_LOWERr_SET(sal, dev->sum_paddr[ch]);
    WRITE_CMIC_SCHANFIFO_SUMMARY_HOSTMEM_START_ADDR_LOWERr(dev->unit, ch, sal);

    /* Pre-configure control register */
    CMIC_SCHANFIFO_CTRLr_CLR(dev->ctl_reg[ch]);
    if (dev->flags & SCHANFIFO_CF_BIGENDIAN) {
        CMIC_SCHANFIFO_CTRLr_RESPONSE_ENDIANESSf_SET(dev->ctl_reg[ch], 1);
        CMIC_SCHANFIFO_CTRLr_SUMMARY_ENDIANESSf_SET(dev->ctl_reg[ch], 1);
    }
    CMIC_SCHANFIFO_CTRLr_AXI_IDf_SET(dev->ctl_reg[ch], CMICX_SCHANFIFO_AXI_ID);
    CMIC_SCHANFIFO_CTRLr_SUMMARY_UPDATE_INTERVALf_SET(dev->ctl_reg[ch], CMICX_SCHANFIFO_SUM_UI);
    if (!(dev->flags & SCHANFIFO_IF_IGNORE_SER_ABORT)) {
        CMIC_SCHANFIFO_CTRLr_ENABLE_RESP_MEM_ECCERR_BASED_ABORTf_SET(dev->ctl_reg[ch], 1);
    }
}

/*!
 * Write commmands.
 */
static void
cmicx_schanfifo_cmds_write(cmicx_schanfifo_dev_t *dev, int ch, uint32_t num_cmds,
                           uint32_t *cmds_buff, size_t cmds_wsize)
{
    CMIC_SCHANFIFO_COMMAND_MEMORYr_t cmd;
    uint32_t mi;

    /* DMA write */
    if (dev->flags & SCHANFIFO_IF_CCMDMA_WR) {
        /* DMA write sucess and return */
    }

    /* PIO wirte */
    for (mi = 0; mi < cmds_wsize; mi++) {
        CMIC_SCHANFIFO_COMMAND_MEMORYr_SET(cmd, cmds_buff[mi]);
        WRITE_CMIC_SCHANFIFO_COMMAND_MEMORYr(dev->unit, ch, mi, cmd);
    }
}

/*!
 * Set SCHAN FIFO start.
 */
static void
cmicx_schanfifo_start_set(cmicx_schanfifo_dev_t *dev, int ch, bool start)
{
    CMIC_SCHANFIFO_CTRLr_t ctl;

    if (start) {
        CMIC_SCHANFIFO_CTRLr_SET(ctl, dev->ctl_reg[ch].v[0]);
        CMIC_SCHANFIFO_CTRLr_STARTf_SET(ctl, 1);
    } else {
        CMIC_SCHANFIFO_CTRLr_CLR(ctl);
    }
    WRITE_CMIC_SCHANFIFO_CTRLr(dev->unit, ch, ctl);
}

/*!
 * Report SCHAN FIFO errors.
 */
static void
cmicx_schanfifo_error_report(cmicx_schanfifo_dev_t *dev, int ch,
                             CMIC_SCHANFIFO_STATUSr_t *stat)
{
    int unit = dev->unit;

    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_NACKf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received sbus nack.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_MESSAGE_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received error message.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_MESSAGE_ERR_CODEf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "H/W received error message code.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_ACK_OPCODE_MISMATCHf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Received sbus ack has wrong opcode.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_MEM_ECC_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "ECC error from command memory.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_RESP_MEM_ECC_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "ECC error from response memory.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_OPCODE_IS_INVALIDf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid sbus command opcode.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_DLEN_IS_INVALIDf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid sbus command length.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_SBUS_TIMEOUTf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Sbus ack not received within configured time.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_ACK_DATA_BEAT_GT20f_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Data beats was greater than 20.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_AXI_RESP_ERRf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "AXI response error.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_AXI_SLAVE_ABORTf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "AXI slave abort.\n")));
    }
    if (CMIC_SCHANFIFO_STATUSr_ERR_TYPE_CMD_MEM_RD_ACCESS_INVALIDf_GET(*stat)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid access for command memory read.\n")));
    }
}

/*!
 * Check SCHAN FIFO status.
 */
static int
cmicx_schanfifo_status_check(cmicx_schanfifo_dev_t *dev, int ch)
{
    CMIC_SCHANFIFO_STATUSr_t stat;

    READ_CMIC_SCHANFIFO_STATUSr(dev->unit, ch, &stat);
    if (CMIC_SCHANFIFO_STATUSr_DONEf_GET(stat)) {
        if (CMIC_SCHANFIFO_STATUSr_ERRORf_GET(stat)) {
            cmicx_schanfifo_error_report(dev, ch, &stat);
            return SHR_E_FAIL;
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_UNAVAIL;
    }
}

/*!
 * Check SCHAN FIFO summary.
 */
static int
cmicx_schanfifo_summary_check(cmicx_schanfifo_dev_t *dev, int ch, uint32_t cmd_id)
{
    uint16_t *sum = (uint16_t *)dev->sum_addr[ch];

    /* Not done yet */
    if (!(sum[cmd_id] & 0x8000)) {
        return SHR_E_UNAVAIL;
    }

    /* Done or with SER error */
    if (!(sum[cmd_id] & 0x6000) ||
        ((sum[cmd_id] & 0x6000) == 0x2000 &&
         (dev->flags & SCHANFIFO_IF_IGNORE_SER_ABORT))) {
        return SHR_E_NONE;
    }

    /* Abort with other errors */
    cmicx_schanfifo_chan_reset(dev, ch);

    return SHR_E_FAIL;
}

/*!
 * Scan SCHAN FIFO summary.
 */
static void
cmicx_schanfifo_summary_scan(cmicx_schanfifo_dev_t *dev, int ch,
                             uint32_t num_cmds, uint32_t *done_cmds)
{
    uint32_t cn;
    int rv;

    for (cn = 0; cn < num_cmds; cn++) {
        rv = cmicx_schanfifo_summary_check(dev, ch, cn);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }

    *done_cmds = cn;
}

/*!
 * Interrupt service routine.
 */
static void
cmicx_schanfifo_isr(cmicx_schanfifo_dev_t *dev, int ch)
{
    /* Disable interrupt */
    bcmbd_cmicx_intr_disable_deferred(dev->unit, CMICX_SCHANFIFO_INTR_NUM(ch));

    sal_sem_give(dev->intr[ch]);
}

/*!
 * Get SCHAN FIFO information.
 */
static int
bcmbd_cmicx_schanfifo_info_get(schanfifo_ctrl_t *ctrl,
                               int *num_chans, size_t *cmd_mem_wsize)
{
    *num_chans = ctrl->channels;
    *cmd_mem_wsize = ctrl->cmds_wsize;

    return SHR_E_NONE;
}

/*!
 * Initialize SCHAN FIFO.
 */
static int
bcmbd_cmicx_schanfifo_dev_init(schanfifo_ctrl_t *ctrl,
                               uint32_t max_polls, uint32_t flags)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];
    int ch;

    dev->polls = max_polls ? max_polls : CMICX_SCHANFIFO_POLLS_DFLT;
    dev->flags |= flags;

    for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
        /* Pre-configure registers */
        cmicx_schanfifo_reg_config(dev, ch);
    }

    return SHR_E_NONE;
}

/*!
 * Send operations to SCHAN FIFO channel.
 */
static int
bcmbd_cmicx_schanfifo_ops_send(schanfifo_ctrl_t *ctrl,
                               int chan, uint32_t num_ops, uint32_t *req_buff,
                               size_t req_wsize, uint32_t flags)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];

    /* Write commands */
    cmicx_schanfifo_cmds_write(dev, chan, num_ops, req_buff, req_wsize);

    /* Pre-configure control register */
    CMIC_SCHANFIFO_CTRLr_NUMB_OF_COMMANDSf_SET(dev->ctl_reg[chan], num_ops);

    /* Enable interrupt if in interrupt mode */
    if (!(dev->flags & SCHANFIFO_IF_POLL_WAIT)) {
        bcmbd_cmicx_intr_enable(dev->unit, CMICX_SCHANFIFO_INTR_NUM(chan));
    }

    if (flags & SCHANFIFO_OF_SET_START) {
        /* Start operation */
        cmicx_schanfifo_start_set(dev, chan, TRUE);
    }

    return SHR_E_NONE;
}

/*!
 * Set start for SCHAN FIFO channel.
 */
static int
bcmbd_cmicx_schanfifo_start_set(schanfifo_ctrl_t *ctrl,
                                int chan, bool start)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];

    cmicx_schanfifo_start_set(dev, chan, start);

    return SHR_E_NONE;
}

/*!
 * Get SCHAN FIFO channel status.
 */
static int
bcmbd_cmicx_schanfifo_status_get(schanfifo_ctrl_t *ctrl,
                                 int chan, uint32_t num_ops, uint32_t flags,
                                 uint32_t *done_ops, uint32_t **resp_buff)
{
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[ctrl->unit];
    int retry = dev->polls;
    int rv;

    if (resp_buff) {
        *resp_buff = dev->resp_addr[chan];
    }

    /* One time check */
    if (!(flags & SCHANFIFO_OF_WAIT_COMPLETE)) {
        rv = cmicx_schanfifo_summary_check(dev, chan, num_ops - 1);
        if (SHR_SUCCESS(rv)) {
            *done_ops = num_ops;
        }
        if (flags & SCHANFIFO_OF_CLEAR_START) {
            cmicx_schanfifo_start_set(dev, chan, FALSE);
        }
        return rv;
    }

    if (dev->flags & SCHANFIFO_IF_POLL_WAIT) {
        /* Wait for completion */
        while (retry--) {
            rv = cmicx_schanfifo_status_check(dev, chan);
            if (rv != SHR_E_UNAVAIL) {
                break;
            }
        }
        if (retry < 0) {
            rv = SHR_E_TIMEOUT;
        }
    } else {
        /* Wait for interrupt */
        if (sal_sem_take(dev->intr[chan], CMICX_SCHANFIFO_WAIT_TIME)) {
            rv = SHR_E_TIMEOUT;
        } else {
            rv = cmicx_schanfifo_status_check(dev, chan);
            if (rv == SHR_E_UNAVAIL) {
                rv = SHR_E_TIMEOUT;
            }
            cmicx_schanfifo_start_set(dev, chan, FALSE);
        }
    }

    if (SHR_SUCCESS(rv)) {
        *done_ops = num_ops;
    } else {
        /* Scan summaries to get done ones */
        cmicx_schanfifo_summary_scan(dev, chan, num_ops, done_ops);
    }

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(dev->unit, "Schan fifo operation failed @chan%d.\n"),
                   chan));
        if (rv == SHR_E_TIMEOUT) {
            cmicx_schanfifo_chan_reset(dev, chan);
        }
    } else {
        if (flags & SCHANFIFO_OF_CLEAR_START) {
            cmicx_schanfifo_start_set(dev, chan, FALSE);
        }
    }

    return rv;
}

static const schanfifo_ops_t cmicx_schanfifo_ops = {
    .info_get           = bcmbd_cmicx_schanfifo_info_get,
    .dev_init           = bcmbd_cmicx_schanfifo_dev_init,
    .ops_send           = bcmbd_cmicx_schanfifo_ops_send,
    .start_set          = bcmbd_cmicx_schanfifo_start_set,
    .status_get         = bcmbd_cmicx_schanfifo_status_get
};

/*!
 * Initialize SCHAN FIFO device.
 */
int
bcmbd_cmicx_schanfifo_init(int unit)
{
    schanfifo_ctrl_t *ctrl = bcmbd_schanfifo_ctrl_get(unit);
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[unit];
    int ch;

    SHR_FUNC_ENTER(unit);

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        SHR_EXIT();
    }

    if (!ctrl || !ctrl->active) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(dev, 0, sizeof(*dev));
    dev->unit = unit;
    dev->ctrl = ctrl;
    dev->polls = CMICX_SCHANFIFO_POLLS_DFLT;

    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        dev->flags |= SCHANFIFO_CF_BIGENDIAN;
    }

    for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
        /* Allocate response buffer */
        dev->resp_size[ch] = CMICX_SCHANFIFO_CMDS_MAX * CMICX_SCHANFIFO_RESP_SIZE;
        dev->resp_addr[ch] = bcmdrd_hal_dma_alloc(unit, dev->resp_size[ch],
                                                  "bcmbdSchanFifoResponse",
                                                  &dev->resp_paddr[ch]);
        if (!dev->resp_addr[ch]) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Allocate summary buffer */
        dev->sum_size[ch] = CMICX_SCHANFIFO_CMDS_MAX * CMICX_SCHANFIFO_SUM_SIZE;
        dev->sum_addr[ch] = bcmdrd_hal_dma_alloc(unit, dev->sum_size[ch],
                                                 "bcmbdSchanFifoSummary",
                                                 &dev->sum_paddr[ch]);
        if (!dev->sum_addr[ch]) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Create semaphore for interrupt mode */
        dev->intr[ch] = sal_sem_create("schanfifo_intr_sem", SAL_SEM_BINARY, 0);
        if (!dev->intr[ch]) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }

        /* Connect respective interrupt */
        bcmbd_cmicx_intr_func_set(unit, CMICX_SCHANFIFO_INTR_NUM(ch),
                                  (bcmbd_intr_f)cmicx_schanfifo_isr, ch);
    }

    ctrl->channels = CMICX_SCHANFIFO_CHAN_MAX;
    ctrl->cmds_wsize = CMICX_SCHANFIFO_CMDS_WSIZE;
    ctrl->ops = (schanfifo_ops_t *)&cmicx_schanfifo_ops;

    dev->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
            if (dev->resp_addr[ch]) {
                bcmdrd_hal_dma_free(unit, dev->resp_size[ch], dev->resp_addr[ch],
                                    dev->resp_paddr[ch]);
                dev->resp_addr[ch] = NULL;
            }
            if (dev->sum_addr[ch]) {
                bcmdrd_hal_dma_free(unit, dev->sum_size[ch], dev->sum_addr[ch],
                                    dev->sum_paddr[ch]);
                dev->sum_addr[ch] = NULL;
            }
            if (dev->intr[ch]) {
                sal_sem_destroy(dev->intr[ch]);
                dev->intr[ch] = NULL;
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * Clean up SCHAN FIFO device.
 */
int
bcmbd_cmicx_schanfifo_cleanup(int unit)
{
    schanfifo_ctrl_t *ctrl = bcmbd_schanfifo_ctrl_get(unit);
    cmicx_schanfifo_dev_t *dev = &schanfifo_dev[unit];
    int ch;

    if (!ctrl) {
        return SHR_E_UNAVAIL;
    }

    if (!dev->inited) {
        return SHR_E_NONE;
    }

    dev->inited = 0;

    ctrl->ops = NULL;

    /* Free all resources */
    for (ch = 0; ch < CMICX_SCHANFIFO_CHAN_MAX; ch++) {
        if (!(dev->flags & SCHANFIFO_IF_POLL_WAIT)) {
            bcmbd_cmicx_intr_disable_deferred(unit, CMICX_SCHANFIFO_INTR_NUM(ch));
        }
        if (dev->resp_addr[ch]) {
            bcmdrd_hal_dma_free(unit, dev->resp_size[ch], dev->resp_addr[ch],
                                dev->resp_paddr[ch]);
            dev->resp_addr[ch] = NULL;
        }
        if (dev->sum_addr[ch]) {
            bcmdrd_hal_dma_free(unit, dev->sum_size[ch], dev->sum_addr[ch],
                                dev->sum_paddr[ch]);
            dev->sum_addr[ch] = NULL;
        }
        if (dev->intr[ch]) {
            sal_sem_destroy(dev->intr[ch]);
            dev->intr[ch] = NULL;
        }
    }

    return SHR_E_NONE;
}

