/*! \file bcmbd_cmicx_schan.c
 *
 * S-Channel (internal command bus) support
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

#include <sal/sal_assert.h>
#include <sal/sal_spinlock.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_dump.h>

#include "bcmbd_cmicx_schan.h"

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SCHAN

/*******************************************************************************
 * Local definitions
 */

/*! Maximum polls for S-channel operation to complete */
#ifndef BCMBD_CMICX_SCHAN_MAX_POLLS
#define BCMBD_CMICX_SCHAN_MAX_POLLS     100
#endif

/*! S-channel control */
typedef struct schan_ctrl_s {
    sal_spinlock_t lock;
    int ch_map;
} schan_ctrl_t;

/*! Per-unit S-channel control object */
static schan_ctrl_t schan_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Local functions
 */

/*
 * Resets the CMIC S-Channel interface. This is required when we sent
 * a message and did not receive a response after the poll count was
 * exceeded.
 */
static void
schan_reset(int unit, int ch)
{
    CMIC_SCHAN_CTRLr_t ctrl;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "S-channel reset\n")));

    READ_CMIC_SCHAN_CTRLr(unit, ch, &ctrl);

    /* Toggle S-Channel abort bit in CMIC_SCHAN_CTRL register */
    CMIC_SCHAN_CTRLr_ABORTf_SET(ctrl, 1);
    WRITE_CMIC_SCHAN_CTRLr(unit, ch, ctrl);

    SAL_CONFIG_MEMORY_BARRIER;

    CMIC_SCHAN_CTRLr_ABORTf_SET(ctrl, 0);
    WRITE_CMIC_SCHAN_CTRLr(unit, ch, ctrl);

    SAL_CONFIG_MEMORY_BARRIER;
}

/*
 * Gets an idle S-channel thru which the PIO operation will be
 * performed.
 */
static void
schan_get(int unit, int *ch)
{
    int i;

    while (1) {
        sal_spinlock_lock(schan_ctrl[unit].lock);
        for (i = 0; i < CMICX_SCHAN_NUM_MAX; i++) {
           if (schan_ctrl[unit].ch_map & 0x01 << i) {
               *ch = i;
               schan_ctrl[unit].ch_map &= ~(0x01 << i);
               sal_spinlock_unlock(schan_ctrl[unit].lock);
               return;
           }
       }
       sal_spinlock_unlock(schan_ctrl[unit].lock);
    }
}

/*
 * Puts an S-channel into the idle channel pool.
 */
static void
schan_put(int unit, int ch)
{
    sal_spinlock_lock(schan_ctrl[unit].lock);
    schan_ctrl[unit].ch_map |= (0x01 << ch);
    sal_spinlock_unlock(schan_ctrl[unit].lock);
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_schan_init(int unit)
{
    if (schan_ctrl[unit].lock) {
        return SHR_E_INTERNAL;
    }
    schan_ctrl[unit].lock = sal_spinlock_create("schan lock");
    if (schan_ctrl[unit].lock == NULL) {
        return SHR_E_RESOURCE;
    }
    schan_ctrl[unit].ch_map = (0x01 << CMICX_SCHAN_NUM_MAX) -1;
    return SHR_E_NONE;
}

int
bcmbd_cmicx_schan_cleanup(int unit)
{
    if (schan_ctrl[unit].lock) {
        sal_spinlock_destroy(schan_ctrl[unit].lock);
        schan_ctrl[unit].lock = NULL;
        schan_ctrl[unit].ch_map = 0;
    }
    return SHR_E_NONE;
}

int
bcmbd_cmicx_schan_op(int unit,
                     schan_msg_t *msg,
                     int dwc_write, int dwc_read)
{
    int i, rv = SHR_E_NONE;
    int ch = 0;
    uint32_t polls = 0;
    CMIC_SCHAN_CTRLr_t ctrl;
    uint32_t msg_addr;

    assert(dwc_write <= CMIC_SCHAN_WORDS_ALLOC);
    assert(dwc_read <= CMIC_SCHAN_WORDS_ALLOC);

    schan_get(unit, &ch);

    /* S-Channel message buffer address */
    msg_addr = CMIC_SCHAN_MESSAGEr_OFFSET + (ch * 0x100);

    /* Write raw S-Channel Data: dwc_write words */
    for (i = 0; i < dwc_write; i++) {
        BCMBD_CMICX_WRITE(unit, msg_addr + i*4, msg->dwords[i]);
    }

    /* Debug output */
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();
        bcmbd_cmicx_dump_data(pb, msg->dwords, dwc_write);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel write:%s\n"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    /* Tell CMIC to start */
    READ_CMIC_SCHAN_CTRLr(unit, ch, &ctrl);
    CMIC_SCHAN_CTRLr_MSG_STARTf_SET(ctrl, 1);
    WRITE_CMIC_SCHAN_CTRLr(unit, ch, ctrl);

    SAL_CONFIG_MEMORY_BARRIER;

    /* Poll for completion */
    for (polls = 0; polls < BCMBD_CMICX_SCHAN_MAX_POLLS; polls++) {
        READ_CMIC_SCHAN_CTRLr(unit, ch, &ctrl);
        if (CMIC_SCHAN_CTRLr_MSG_DONEf_GET(ctrl)) {
            break;
        }
    }

    /* Check for timeout and error conditions */
    if (polls == BCMBD_CMICX_SCHAN_MAX_POLLS) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel timeout\n")));
        rv = SHR_E_TIMEOUT;
    }

    if (CMIC_SCHAN_CTRLr_NACKf_GET(ctrl)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel NAK\n")));
        rv = SHR_E_FAIL;
    }

    if (CMIC_SCHAN_CTRLr_SER_CHECK_FAILf_GET(ctrl)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel SER error\n")));
        rv = SHR_E_FAIL;
    }

    if (CMIC_SCHAN_CTRLr_TIMEOUTf_GET(ctrl)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "S-channel TO error\n")));
        rv = SHR_E_FAIL;
    }

    CMIC_SCHAN_CTRLr_MSG_DONEf_SET(ctrl, 0);
    WRITE_CMIC_SCHAN_CTRLr(unit, ch, ctrl);

    SAL_CONFIG_MEMORY_BARRIER;

    if (rv == SHR_E_TIMEOUT) {
        schan_reset(unit, ch);
    } else {
        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            BCMBD_CMICX_READ(unit, msg_addr + 4*i, &msg->dwords[i]);
        }

        /* Debug output */
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_t *pb = shr_pb_create();
            bcmbd_cmicx_dump_data(pb, msg->dwords, dwc_read);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "S-channel read:%s\n"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
    }

    schan_put(unit, ch);

    return rv;
}
