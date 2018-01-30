/*! \file bcmptm_walr.c
 *
 * Schan Fifo related utils
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_sleep.h>
#include <sal/sal_mutex.h>
#include <sal/sal_time.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_schanfifo.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include "./bcmptm_wal_local.h"
#include "./bcmptm_walr_local.h"


/*******************************************************************************
 * Defines
 */
#define BCMPTM_CFG_WALR_THREAD_PRIO -1 /* Use default thread prio, stack size */
#define BCMPTM_CFG_WALR_THREAD_STOP_WAIT_USEC (10 * SECOND_USEC)

#define BCMPTM_CFG_WALR_MSG_RETRY_COUNT_SER 1000
#define BCMPTM_CFG_WALR_MSG_RETRY_COUNT_WRITE_EXE0 10
#define BCMPTM_CFG_WALR_MSG_RETRY_COUNT_WRITE_EXE1 10
#define BCMPTM_CFG_WALR_MSG_RETRY_COUNT_SLAM_EXE 3

#define BCMPTM_CFG_WALR_SERC_SCF_MUTEX_WAIT_USEC 1000000
#define BCMPTM_CFG_WALR_BD_SCF_MUTEX_WAIT_USEC 1000000
#define BCMPTM_CFG_WALR_BD_SCF_MUTEX_RETRY_COUNT 5

#define BCMPTM_CFG_WALR_GIVE_MUTEX_BEFORE_RETRY FALSE

/* Use schanfifo (if avail) when num_ops in msg > this threshold */
#define BCMPTM_CFG_WALR_USE_SCF_THR 4

#define BSL_LOG_MODULE BSL_LS_BCMPTM_WAL

#define BCMBD_SCF_START_TRUE TRUE
#define BCMBD_SCF_START_FALSE FALSE

#define SLAM_CWORDS_ARRAY_SIZE 4

#define WALR_MSG_DQ_IDX bcmptm_wal_msg_dq_idx[unit]
#define WALR_MSG (bcmptm_wal_msg_dq_ptr[unit])
#define WALR_MSG_DQ_READY (bcmptm_wal_msg_dq_ready_ptr[unit])

#define WALR_PRE_MSG_CBF(walr_msg)\
do {\
    if (walr_msg->need_cbf_before_msg) {\
        tmp_rv = bcmptm_wal_rdr_msg_done(unit, walr_msg, NULL);\
        if (tmp_rv != SHR_E_NONE) {\
            LOG_ERROR(BSL_LOG_MODULE,\
                (BSL_META_U(unit, "pre_cbf got rv=%0d\n"),\
                 tmp_rv));\
        }\
    }\
} while (0)

#define WALR_POST_MSG_CBF_OK(walr_msg)\
do {\
    sal_memset(&cbf_status, 0, sizeof(bcmptm_walr_status_t));\
    tmp_rv = bcmptm_wal_rdr_msg_done(unit, walr_msg, &cbf_status);\
    if (tmp_rv != SHR_E_NONE) {\
        LOG_ERROR(BSL_LOG_MODULE,\
            (BSL_META_U(unit, "post_cbf got rv=%0d\n"),\
             tmp_rv));\
    }\
} while (0)

#define BCMBD_SCF_OPS_SEND(ch, walr_msg, flags)\
req_entry_words = bcmptm_wal_words_buf_bptr[unit] + (walr_msg->words_buf_idx);\
req_entry_words_pa = bcmptm_wal_words_buf_bpa[unit] +\
                     4*(walr_msg->words_buf_idx);\
assert((req_entry_words_pa & 0xF) == 0);\
tmp_rv = bcmbd_schanfifo_ops_send(unit, (ch), walr_msg->num_ops,\
                                  req_entry_words, walr_msg->req_entry_wsize,\
                                  (flags))\


/*******************************************************************************
 * Typedefs
 */
typedef enum {
    SCF_THREAD_STATE_STOP = 0,
    SCF_THREAD_STATE_RUN
} scf_thread_state_t;

/*! \brief Descr of wal reader thread context */
typedef struct scf_thread_info_s {
    int unit;                 /*!< Logical device ID.  */
    scf_thread_state_t state; /*!< Thread state. */
} scf_thread_info_t;

typedef enum {
    CMD_STATE_IDLE = 0,
    CMD_STATE_CH_A_BUSY,
    CMD_STATE_CH_A_BUSY_CH_B_WAIT,
} cmd_state_t;


/*******************************************************************************
 * Private variables
 */
static scf_thread_info_t *scf_th_info_p[BCMDRD_CONFIG_MAX_UNITS];

static sal_mutex_t scf_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool walr_has_scf_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool serc_needs_scf_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool serc_has_scf_mutex[BCMDRD_CONFIG_MAX_UNITS];
static shr_thread_ctrl_t *walr_tc[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
/* Will be called only when we know that walr_has_scf_mutex is FALSE */
static int
walr_scf_mutex_take(int unit)
{
    int tmp_rv;
    SHR_FUNC_ENTER(unit);
    tmp_rv = sal_mutex_take(scf_mutex[unit], BCMPTM_CFG_WALR_BD_SCF_MUTEX_WAIT_USEC);
    if (tmp_rv == SHR_E_NONE) {
        walr_has_scf_mutex[unit] = TRUE;
    }
    SHR_RETURN_VAL_EXIT(tmp_rv);
exit:
    SHR_FUNC_EXIT();
}

/* Will be called only when we know that walr_has_scf_mutex is TRUE */
static int
walr_scf_mutex_give(int unit)
{
    int tmp_rv;
    SHR_FUNC_ENTER(unit);
    
    tmp_rv = sal_mutex_give(scf_mutex[unit]);
    if (tmp_rv == SHR_E_NONE) {
        walr_has_scf_mutex[unit] = FALSE;
    }
    SHR_RETURN_VAL_EXIT(tmp_rv);
exit:
    SHR_FUNC_EXIT();
}

static int
retry_prep(int unit)
{
    uint32_t retry_count = 0;
    int tmp_rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Give up WAL reader lock */
    if (BCMPTM_CFG_WALR_GIVE_MUTEX_BEFORE_RETRY) {
        SHR_IF_ERR_EXIT(walr_scf_mutex_give(unit));
        /* Give serc thread a chance to take up CPU */
        sal_usleep(1000);
    }
    /* Wait for lock - but not forever */
    while (!walr_has_scf_mutex[unit] &&
           (retry_count < BCMPTM_CFG_WALR_BD_SCF_MUTEX_RETRY_COUNT)) {
        tmp_rv = walr_scf_mutex_take(unit);
        if (tmp_rv != SHR_E_NONE) {
            retry_count++;
        }
    }
    /* Must not retry if we dont have scf_mutex */
    SHR_IF_ERR_MSG_EXIT(
        (!walr_has_scf_mutex[unit] ? SHR_E_FAIL : SHR_E_NONE),
        (BSL_META_U(unit, "BD could not get scf_mutex, for "
                    "msg_retry\n")));
exit:
    SHR_FUNC_EXIT();
}

/* Use PIO to send ops to HW.
 *
 * ASSUME: Caller has taken scf_mutex before calling this function.
 */
static int
walr_msg_cmd_write_exe0(int unit, bcmptm_wal_msg_t *walr_msg)
{
    uint32_t i, retry_count = 0, *req_entry_words = NULL;
    bcmptm_wal_ops_info_t *ops_info_array;
    bcmptm_walr_status_t cbf_status;
    int tmp_rv = SHR_E_NONE;
    bool msg_retry = FALSE;
    SHR_FUNC_ENTER(unit);
    assert(walr_has_scf_mutex[unit]);

    WALR_PRE_MSG_CBF(walr_msg); /* pre_cbf */
    SHR_IF_ERR_MSG_EXIT(tmp_rv, 
        (BSL_META_U(unit, "bcmptm_walr.write_pio_exe.pre_cbf GOT rv=%0d from "
                    "rdr_pre_msg func\n"), tmp_rv));

    do {
        sal_memset(&cbf_status, 0, sizeof(bcmptm_walr_status_t));
        req_entry_words = bcmptm_wal_words_buf_bptr[unit] +
                          (walr_msg->words_buf_idx);
        ops_info_array = bcmptm_wal_ops_info_bptr[unit] +
                         walr_msg->ops_info_idx;
        for (i = 0; i < walr_msg->num_ops; i++) {
            tmp_rv = bcmbd_pt_write(unit,
                                    ops_info_array->sid,
                                    &ops_info_array->dyn_info,
                                    NULL, /* ovrr_info */
                                    req_entry_words + ops_info_array->op_ctrl_wsize);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "bcmptm_walr.write_exe0.bd_pt_write "
                                "failed(rv=%0d, op_index=%0d)\n"),
                     tmp_rv, i));
                break; /* from for loop */
            }
            cbf_status.num_good_ops++;
            cbf_status.good_req_words_wsize += ops_info_array->op_wsize;
            req_entry_words += ops_info_array->op_wsize;
            ops_info_array++;
        }
        if (cbf_status.num_good_ops != walr_msg->num_ops) {
            cbf_status.rv = tmp_rv;
        }
        tmp_rv = bcmptm_wal_rdr_msg_done(unit, walr_msg, &cbf_status);
        if (tmp_rv == SHR_E_PARTIAL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "bcmptm_walr.write_exe0.cbf GOT rv=E_PARTIAL from "
                            "rdr_msg_done. Will retry the msg\n")));
            retry_count++;
            if (retry_count > BCMPTM_CFG_WALR_MSG_RETRY_COUNT_WRITE_EXE0) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "bcmptm_walr.write_exe0.failed even "
                                "after %0u attempts, num_ops_in_msg=%0d\n"),
                                retry_count, walr_msg->num_ops));
                SHR_RETURN_VAL_EXIT(tmp_rv);
            } else {
                msg_retry = TRUE;
                SHR_IF_ERR_EXIT(retry_prep(unit));
            }
        } else {
            SHR_IF_ERR_MSG_EXIT(tmp_rv,
                (BSL_META_U(unit, "bcmptm_walr.write_exe0.cbf GOT rv=%0d from "
                            "rdr_msg_done\n"), tmp_rv));
            
        }
    } while (msg_retry);
exit:
    SHR_FUNC_EXIT();
}

/* Use single schanfifo chan (ch0) to send ops to HW.
 *
 * ASSUME: Caller has taken scf_mutex before calling this function.
 */
static int
walr_msg_cmd_write_exe1(int unit, bcmptm_wal_msg_t *walr_msg)
{
    uint32_t num_good_ops, *req_entry_words = NULL;
    uint64_t req_entry_words_pa;
    bcmptm_walr_status_t cbf_status;
    int tmp_rv = SHR_E_NONE, ch = 0;
    bool msg_retry = FALSE;
    uint32_t retry_count = 0;
    SHR_FUNC_ENTER(unit);
    assert(walr_has_scf_mutex[unit]);

    WALR_PRE_MSG_CBF(walr_msg); /* pre_cbf */
    SHR_IF_ERR_MSG_EXIT(tmp_rv, 
        (BSL_META_U(unit, "bcmptm_walr.write_ch0_exe.pre_cbf GOT rv=%0d from "
                    "rdr_pre_msg func\n"), tmp_rv));

    do {
        BCMBD_SCF_OPS_SEND(ch, walr_msg, SCHANFIFO_OF_SET_START);

        sal_memset(&cbf_status, 0, sizeof(bcmptm_walr_status_t));
        if (tmp_rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "wal_rdr.write_exe1.ops_send "
                            "failed(rv=%0d)\n"),
                 tmp_rv));
            cbf_status.rv = tmp_rv;
        } else { /* wait for done */
            tmp_rv = bcmbd_schanfifo_status_get(unit, ch, walr_msg->num_ops,
                                                (SCHANFIFO_OF_WAIT_COMPLETE |
                                                 SCHANFIFO_OF_CLEAR_START),
                                                &num_good_ops,
                                                NULL); /* resp_buff */
            if ((tmp_rv != SHR_E_NONE) || (num_good_ops != walr_msg->num_ops)) {
                cbf_status.rv = SHR_E_FAIL;
                cbf_status.num_good_ops = 0; 
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "wal_rdr.write_exe1.status_get "
                                "failed(rv=%0d, num_good_ops=%0d)\n"),
                     tmp_rv, num_good_ops));
            }
        }
        tmp_rv = bcmptm_wal_rdr_msg_done(unit, walr_msg, &cbf_status);
        if (tmp_rv == SHR_E_PARTIAL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "bcmptm_walr.write_exe1.cbf GOT rv=E_PARTIAL from "
                            "rdr_msg_done. Will retry the msg\n")));
            /* schan_fifo is configured to not abort channel on SER events, so
             * keep retrying for all other error cases */
            retry_count++;
            if (retry_count > BCMPTM_CFG_WALR_MSG_RETRY_COUNT_WRITE_EXE1) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "bcmptm_walr.write_exe1.failed even "
                                "after %0u attempts\n"),
                                retry_count));
                SHR_RETURN_VAL_EXIT(tmp_rv);
            } else {
                msg_retry = TRUE;
                /* schanfifo driver does reset of failed channel. */
                SHR_IF_ERR_EXIT(retry_prep(unit));
            }
        } else {
            SHR_IF_ERR_MSG_EXIT(tmp_rv,
                (BSL_META_U(unit, "bcmptm_walr.write_exe.cbf GOT rv=%0d from "
                            "rdr_msg_done\n"), tmp_rv));
            
        }
    } while (msg_retry);
exit:
    SHR_FUNC_EXIT();
}

/* Use sbusdma to execute write
 *
 * ASSUME: Caller has taken scf_mutex before calling this function.
 */
static int
walr_msg_cmd_slam_exe(int unit, bcmptm_wal_msg_t *walr_msg)
{
    bcmptm_wal_ops_info_t *ops_info_array;
    uint32_t cwords_array[SLAM_CWORDS_ARRAY_SIZE], num_cwords, retry_count = 0;
    bcmptm_walr_status_t cbf_status;
    int tmp_rv = SHR_E_NONE;
    bool msg_retry = FALSE;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    SHR_FUNC_ENTER(unit);
    assert(walr_has_scf_mutex[unit]);
    ops_info_array = bcmptm_wal_ops_info_bptr[unit] +
                     walr_msg->ops_info_idx;
    SHR_IF_ERR_EXIT(
        bcmbd_pt_cmd_hdr_get(unit,
                             ops_info_array->sid,
                             &(ops_info_array->dyn_info),
                             NULL, /* pt_ovrr_info */
                             bcmdrd_pt_is_counter(unit, ops_info_array->sid) ?
                             BCMBD_PT_CMD_COUNT :
                             BCMBD_PT_CMD_WRITE,
                             SLAM_CWORDS_ARRAY_SIZE,
                             cwords_array,
                             &num_cwords));
    assert(num_cwords != 0);

    /* Prepare dma_data */
    sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
    dma_data.start_addr = cwords_array[1]; /* Addr beat */
    dma_data.data_width = bcmdrd_pt_entry_wsize(unit, ops_info_array->sid);
                                           /* Num of data beats */
     /* dma_data.addr_gap = 0; */          /* Gap between addresses */
    dma_data.op_code = cwords_array[0];    /* Opcode beat */
    dma_data.op_count = walr_msg->num_ops; /* Num of locations */
    dma_data.buf_paddr = bcmptm_wal_words_buf_bpa[unit] +
                         4*(walr_msg->words_buf_idx);
    assert((dma_data.buf_paddr & 0xF) == 0);\
        /* Phys addr for slam data.
         * Must be on 4 Byte boundary.
         * Should be on 16 Byte boundary for optimum performance. */
    /* dma_data.jump_item = 0; */
    /* dma_data.attrs = 0; */
    dma_data.pend_clocks = 0; 

    /* Prepare dma_work */
    sal_memset(&dma_work, 0, sizeof(bcmbd_sbusdma_work_t));
    dma_work.data = &dma_data;
    dma_work.items = 1;
    /* dma_work.link = 0; */
    /* dma_work.desc = 0; */
    /* dma_work.pc = 0; */
    /* dma_work.pc_data = 0; */
    /* dma_work.cb = 0; */
    /* dma_work.cb_data = 0; */
    /* dma_work.priv = 0; */
    dma_work.flags = SBUSDMA_WF_WRITE_CMD; /* | SBUSDMA_WF_INT_MODE */
    /* dma_work.state = 0; */

    WALR_PRE_MSG_CBF(walr_msg); /* pre_cbf */
    SHR_IF_ERR_MSG_EXIT(tmp_rv, 
        (BSL_META_U(unit, "bcmptm_walr.write_slam_exe.pre_cbf GOT rv=%0d from "
                    "rdr_pre_msg func\n"), tmp_rv));

    sal_memset(&cbf_status, 0, sizeof(bcmptm_walr_status_t));
    do {
        tmp_rv = bcmbd_sbusdma_light_work_init(unit, &dma_work);
        if (tmp_rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "bcmptm_walr.slam_exe.sbusdma_light_work "
                        " returned rv=%0d\n"), tmp_rv));
            cbf_status.rv = tmp_rv;
        } else {
            tmp_rv = bcmbd_sbusdma_work_execute(unit, &dma_work);
            if (tmp_rv != SHR_E_NONE) {
                cbf_status.rv = tmp_rv;
                cbf_status.num_good_ops = 0; 
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "bcmptm_walr.slam_exe.sbusdma_work_"
                                "execute returned rv=%0d\n"), tmp_rv));
            }
        }
        tmp_rv = bcmptm_wal_rdr_msg_done(unit, walr_msg, &cbf_status);
        if (tmp_rv == SHR_E_PARTIAL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "bcmptm_walr.slam_exe.cbf GOT rv=E_PARTIAL from "
                            "rdr_msg_done. Will retry the msg\n")));
            retry_count++;
            if (retry_count > BCMPTM_CFG_WALR_MSG_RETRY_COUNT_SLAM_EXE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "bcmptm_walr.slam_exe.failed even "
                                "after %0u attempts\n"),
                                retry_count));
                SHR_RETURN_VAL_EXIT(tmp_rv);
            } else {
                msg_retry = TRUE;
                /* schanfifo driver does reset of failed channel. */
                SHR_IF_ERR_EXIT(retry_prep(unit));
            }
        } else {
            SHR_IF_ERR_MSG_EXIT(tmp_rv,
                (BSL_META_U(unit, "bcmptm_walr.slam_exe.cbf GOT rv=%0d from "
                            "rdr_msg_done\n"), tmp_rv));
            
        }
    } while (msg_retry);
exit:
    SHR_FUNC_EXIT();
}

/* - For each unit there will be separate thread_func instance - and also this
 *   function will never exit.
 *   Thus all local vars declared in this function are per unit variables -
 *   implicitly.
 *
 * - shr_thr framework will call thread_func (below) with arg = th_info_p
 *   (last param specified for shr_thread_start()
 */
static void
scf_thread_func(shr_thread_ctrl_t *tc, void *arg)
{
    scf_thread_info_t *th_info_p = (scf_thread_info_t *)arg;
    int unit, tmp_rv = SHR_E_NONE;
    uint8_t tx_ch = 0, rx_ch = 0, old_rx_ch = 0;
    uint32_t retry_count, *req_entry_words, n_msg_dq_idx = 0;
    uint32_t num_good_ops;
    uint64_t req_entry_words_pa;
    bool thread_done = FALSE, *n_walr_msg_dq_ready = NULL;
    bcmptm_wal_msg_t *n_walr_msg = NULL;
    bcmptm_walr_status_t cbf_status;
    cmd_state_t cmd_state = CMD_STATE_IDLE;

    th_info_p->state = SCF_THREAD_STATE_RUN;
    unit = th_info_p->unit;

    /* Must wait for nudge from WAL writer to enter endless loop.
     * WAL writer will set init value for dq_msg_idx and only then wake up WAL
     * reader to start the dqueue of msgs. */
    shr_thread_sleep(tc, SHR_THREAD_FOREVER);
    if (shr_thread_stopping(tc)) {
        thread_done = TRUE;
    }
    while (!thread_done) {
        if (walr_has_scf_mutex[unit] && (cmd_state == CMD_STATE_IDLE)) {
            /* Entertain pause request only when not in middle of sending
             * ops for msg to HW. This also makes sure that ops are not pending
             * in schanfifo channel. */
            if (serc_needs_scf_mutex[unit]) {
                walr_scf_mutex_give(unit);
                /* Give serc thread a chance to take up CPU */
                sal_usleep(1000);
            }
        }
        
        retry_count = 0;
        while (!walr_has_scf_mutex[unit] &&
               (retry_count < BCMPTM_CFG_WALR_BD_SCF_MUTEX_RETRY_COUNT)) {
            tmp_rv = walr_scf_mutex_take(unit);
            if (tmp_rv != SHR_E_NONE) {
                retry_count++;
            }
        }

        /* Must not proceed if we dont have scf_mutex */
        if (!walr_has_scf_mutex[unit]) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "WALR should be having scf_mutex, but it does "
                            "not \n")));
            break; /* from while(!thread_done) loop */
        }

        /* Expect WALR_MSG_DQ_IDX, DQ_PTR, DQ_READY_PTR are set correctly */

        switch (cmd_state) {
/* -------------------------------------------------------------------------- */
        case CMD_STATE_IDLE:
            if (*WALR_MSG_DQ_READY) {
                switch (WALR_MSG->cmd) {
                case BCMPTM_WAL_MSG_CMD_SLAM:
                    *WALR_MSG_DQ_READY = FALSE; /* for current msg */
                    /* Must do clear of dq_ready before calling slam_exe
                     * because msg freed by post_cbf can be reused by
                     * writer and resent */
                    tmp_rv = walr_msg_cmd_slam_exe(unit, WALR_MSG);
                    assert(tmp_rv == 0); 

                    /* Next msg prep */
                    WALR_MSG_DQ_IDX =
                        bcmptm_wal_next_msg_idx(unit, WALR_MSG_DQ_IDX);
                    if (WALR_MSG_DQ_IDX) {
                        WALR_MSG_DQ_READY++;
                        WALR_MSG++;
                    } else {
                        WALR_MSG_DQ_READY = BCMPTM_WAL_MSG_DQ_READY_BPTR;
                        WALR_MSG = BCMPTM_WAL_MSG_BPTR;
                    }
                    break;
                case BCMPTM_WAL_MSG_CMD_WRITE:
                    switch (BCMPTM_WAL_SCF_NUM_CHANS) {
                    case 0:
                    case 1:
                        *WALR_MSG_DQ_READY = FALSE; /* for current msg */
                        /* Must do clear of dq_ready before calling exe0, exe1
                         * because msg freed by post_cbf can be reused by
                         * writer and resent */

                        if (BCMPTM_WAL_SCF_NUM_CHANS &&
                            (WALR_MSG->num_ops > BCMPTM_CFG_WALR_USE_SCF_THR)) {

                            tmp_rv = walr_msg_cmd_write_exe1(unit, WALR_MSG);
                        } else {
                            tmp_rv = walr_msg_cmd_write_exe0(unit, WALR_MSG);
                        }
                        assert(tmp_rv == 0); 

                        /* Next msg prep */
                        WALR_MSG_DQ_IDX =
                            bcmptm_wal_next_msg_idx(unit, WALR_MSG_DQ_IDX);
                        if (WALR_MSG_DQ_IDX) {
                            WALR_MSG_DQ_READY++;
                            WALR_MSG++;
                        } else {
                            WALR_MSG_DQ_READY = BCMPTM_WAL_MSG_DQ_READY_BPTR;
                            WALR_MSG = BCMPTM_WAL_MSG_BPTR;
                        }
                        break;
                    default: /* 2 */
                        WALR_PRE_MSG_CBF(WALR_MSG); /* pre_cbf */
                        assert(tmp_rv == 0); 

                        /* Send_ops to idle_ch with start=1 */
                        tx_ch = 0; /* always start with ch0 when in IDLE */
                        BCMBD_SCF_OPS_SEND(tx_ch, WALR_MSG,
                                           SCHANFIFO_OF_SET_START);
                        assert(tmp_rv == 0); 
                        *WALR_MSG_DQ_READY = FALSE;
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "\tSent ops to ch%0d with start=1"
                                        " (msg_id=%0d)\n"),
                             tx_ch, WALR_MSG_DQ_IDX));

                        /* Next cmd_state */
                        cmd_state = CMD_STATE_CH_A_BUSY;
                        rx_ch = 0; /* for status_get (busy_ch) */
                        tx_ch = 1; /* for next ops_send (idle_ch) */

                        /* Next msg prep - none */
                        break; /* 2 */
                    } /* BCMPTM_WAL_SCF_NUM_CHANS */
                    break; /* BCMPTM_WAL_MSG_CMD_WRITE */

                default: /* Error */
                    
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Unknown msg_cmd %0d\n"),
                         WALR_MSG->cmd));
                    break;
                } /* switch (WALR_MSG->cmd) */
            } else { /* No more msgs (for now) */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "msg_dq_ready is FALSE (msg_idx=%0d)\n"),
                     WALR_MSG_DQ_IDX));
                /* Give up mutex before entering deep sleep */
                walr_scf_mutex_give(unit);
                shr_thread_sleep(tc, SHR_THREAD_FOREVER);
                if (shr_thread_stopping(tc)) {
                    thread_done = TRUE;
                }
            }
        break; /* CMD_STATE_IDLE */

/* -------------------------------------------------------------------------- */
        case CMD_STATE_CH_A_BUSY:
            /* When in CH_A_BUSY state
             *     WALR_MSG, WALR_MSG_DQ_READY point to busy_ch_A
             *     (which was sent first to HW).
             *
             *     n_walr_msg, n_walr_msg_dq_ready can be dont_care
             */
            /* rx_ch is busy_ch */
            /* tx_ch is idle_ch */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\nSTATE_CH_A_BUSY: busy_ch=%0d, "
                            "idle_ch=%0d\n"),
                 rx_ch, tx_ch));

            /* Strategy: Attempt to keep HW busy by sending ops to idle_ch
             *           and then check status of busy_ch */

            /* Check if next msg is ready for dq */
            n_msg_dq_idx = bcmptm_wal_next_msg_idx(unit, WALR_MSG_DQ_IDX);
            if (n_msg_dq_idx) {
                n_walr_msg_dq_ready = WALR_MSG_DQ_READY + 1;
                n_walr_msg = WALR_MSG + 1;
            } else {
                n_walr_msg_dq_ready = BCMPTM_WAL_MSG_DQ_READY_BPTR;
                n_walr_msg = BCMPTM_WAL_MSG_BPTR;
            }

            if (*n_walr_msg_dq_ready &&
                (n_walr_msg->cmd == BCMPTM_WAL_MSG_CMD_WRITE) &&
                (n_walr_msg->num_ops > BCMPTM_CFG_WALR_USE_SCF_THR) &&
                !serc_needs_scf_mutex[unit]) {

                WALR_PRE_MSG_CBF(n_walr_msg); /* pre_cbf */
                assert(tmp_rv == 0); 

                /* Send_ops to idle_ch with start=0 */
                BCMBD_SCF_OPS_SEND(tx_ch, n_walr_msg, 0);
                assert(tmp_rv == 0); 
                *n_walr_msg_dq_ready = FALSE; /* for next msg */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "\tSent ops to ch%0d with start=0 "
                                "(msg_id=%0d)\n"), tx_ch, n_msg_dq_idx));

                /* Next cmd_state */
                cmd_state = CMD_STATE_CH_A_BUSY_CH_B_WAIT;
                /* use rx_ch for status_get (busy_ch) */
                /* use tx_ch for setting start = 1 (wait_ch) */

                /* Next msg prep - none */
            } else {
                /* Next msg not ready or next msg that does not need to
                 * be sent to scf or serc is asking to pause wall reader.
                 *
                 * If next msg was SLAM/PIO, it cannot be started to
                 * preserve ORDER. Must wait for busy_ch completion.
                 *
                 * If next msg is not valid, it is ok to simplify.
                 */

                /* Wait for busy_ch done, and clear start for busy_ch */
                num_good_ops = 0;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "\tPolling status for ch%0d (wait_done, "
                                "clear_start=1)\n"), rx_ch));
                tmp_rv =
                    bcmbd_schanfifo_status_get(unit, rx_ch,
                                               WALR_MSG->num_ops,
                                               (SCHANFIFO_OF_WAIT_COMPLETE |
                                                SCHANFIFO_OF_CLEAR_START),
                                               &num_good_ops,
                                               NULL); /* resp_buff */
                if ((tmp_rv == SHR_E_NONE) &&
                    (num_good_ops == WALR_MSG->num_ops)) {

                    WALR_POST_MSG_CBF_OK(WALR_MSG); /* post_cbf */
                    assert(tmp_rv == 0); 

                    /* Next cmd_state */
                    cmd_state = CMD_STATE_IDLE;
                    /* tx_ch, rx_ch not meaningful in IDLE */

                    /* Next msg prep */
                    WALR_MSG_DQ_IDX = n_msg_dq_idx;
                    WALR_MSG_DQ_READY = n_walr_msg_dq_ready;
                    WALR_MSG = n_walr_msg;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "\nSTATE_IDLE: msg_idx=%0d\n"),
                         WALR_MSG_DQ_IDX));
                } else {
                    
                } /* Non-WRITE WALR_MSG->cmd */
            }
            break; /* CMD_STATE_CH_A_BUSY */

/* -------------------------------------------------------------------------- */
        case CMD_STATE_CH_A_BUSY_CH_B_WAIT:
            /* When in CH_A_BUSY_CH_B_WAIT state
             *     WALR_MSG, WALR_MSG_DQ_READY are pointing to busy_ch_A
             *     (which was sent first to HW).
             *
             *     n_walr_msg, n_walr_msg_dq_ready are pointing to wait_ch_B
             *     (which was sent next to HW).
             */

            /* use rx_ch for status_get (busy_ch) */
            /* use tx_ch for setting start = 1 (wait_ch) */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\nSTATE_CH_A_BUSY_CH_B_WAIT: busy_ch=%0d"
                            ", wait_ch=%0d\n"), rx_ch, tx_ch));

            /* Wait for busy_ch done, and clear start */
            num_good_ops = 0;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\tPolling status for ch%0d (wait_done, "
                            "clear_start=1)\n"), rx_ch));
            tmp_rv = bcmbd_schanfifo_status_get(unit, rx_ch,
                                                WALR_MSG->num_ops,
                                                (SCHANFIFO_OF_WAIT_COMPLETE |
                                                 SCHANFIFO_OF_CLEAR_START),
                                                &num_good_ops,
                                                NULL); /* resp_buff */
            if ((tmp_rv == SHR_E_NONE) && (num_good_ops == WALR_MSG->num_ops)) {
                /* First make HW busy and then process cbf of done_ch */

                /* Set START for wait_ch */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "\tMaking start=1 for ch%0d\n"), tx_ch));
                (void)bcmbd_schanfifo_set_start(unit, tx_ch, BCMBD_SCF_START_TRUE);
                /* Above makes wait_ch (tx_ch) as the new busy_ch */

                /* post_cbf for busy_ch */
                WALR_POST_MSG_CBF_OK(WALR_MSG); /* post_cbf */
                assert(tmp_rv == 0); 
                /* Now (previously) busy_ch (rx_ch) becomes new idle_ch */

                /* Next cmd_state */
                cmd_state = CMD_STATE_CH_A_BUSY;
                old_rx_ch = rx_ch; /* want to xchange rx_ch, tx_ch values */
                rx_ch = tx_ch;     /* for status_get (busy_ch) */
                tx_ch = old_rx_ch; /* for next ops_send (idle_ch) */

                /* Next msg prep
                 * - make WALR_MSG, WALR_MSG_DQ_READY to new busy_ch */
                WALR_MSG_DQ_IDX = n_msg_dq_idx;
                WALR_MSG_DQ_READY = n_walr_msg_dq_ready;
                WALR_MSG = n_walr_msg;
            } else {
                
            }
            break; /* CMD_STATE_CH_A_BUSY_CH_B_WAIT */

/* -------------------------------------------------------------------------- */
        default: /* Unknown cmd_state */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Unknown cmd_state %0d\n"), cmd_state));
            thread_done = TRUE;
            break; /* unknown cmd_state */
        } /* cmd_state */
    } /* while (!thread_done) */

    /* Can be here only if we exit while loop */
    th_info_p->state = SCF_THREAD_STATE_STOP;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "WAL reader thread function exited !!\n")));
}


/******************************************************************************
 * Public Functions
 */
int
bcmptm_walr_run_check(int unit)
{
    scf_thread_info_t *th_info_p;
    SHR_FUNC_ENTER(unit);
    th_info_p = scf_th_info_p[unit];
    SHR_IF_ERR_MSG_EXIT(
        (th_info_p->state != SCF_THREAD_STATE_RUN) ? SHR_E_INTERNAL : SHR_E_NONE,
        (BSL_META_U(unit, "WAL reader thread is not in RUN state\n")));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_walr_wake(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(shr_thread_wake(walr_tc[unit]));
exit:
    SHR_FUNC_EXIT();
}

/* Following will execute in SERC context */
int
bcmptm_walr_lock_req_serc(int unit)
{
    int tmp_rv;
    SHR_FUNC_ENTER(unit);
    if (serc_has_scf_mutex[unit]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! SERC already has scf_mutex. Should not be "
                        "requesting again \n")));
        tmp_rv = SHR_E_INTERNAL;
        SHR_RETURN_VAL_EXIT(tmp_rv);
    } else {
        serc_needs_scf_mutex[unit] = TRUE;

        tmp_rv = sal_mutex_take(scf_mutex[unit], BCMPTM_CFG_WALR_SERC_SCF_MUTEX_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_scf_mutex[unit] = TRUE;
            serc_needs_scf_mutex[unit] = FALSE; /* note */
        }
        SHR_RETURN_VAL_EXIT(tmp_rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/* Following will execute in SERC context */
int
bcmptm_walr_unlock_req_serc(int unit)
{
    int tmp_rv;
    SHR_FUNC_ENTER(unit);
    if (!serc_has_scf_mutex[unit]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! SERC does not have scf_mutex. Should not be "
                        "requesting unlock \n")));
        tmp_rv = SHR_E_INTERNAL;
        SHR_RETURN_VAL_EXIT(tmp_rv);
    } else {
        tmp_rv = sal_mutex_give(scf_mutex[unit]);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_scf_mutex[unit] = FALSE;
        }
        SHR_RETURN_VAL_EXIT(tmp_rv);
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_walr_init(int unit)
{
    scf_thread_info_t *th_info_p = NULL;
    SHR_FUNC_ENTER(unit);

    if (scf_th_info_p[unit] != NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    /* Schan_fifo lock create */
    scf_mutex[unit] = sal_mutex_create("SCHAN_FIFO_LOCK");
    SHR_NULL_CHECK(scf_mutex[unit], SHR_E_MEMORY);
    walr_has_scf_mutex[unit] = FALSE;
    serc_needs_scf_mutex[unit] = FALSE;
    serc_has_scf_mutex[unit] = FALSE;

    /* Alloc space for th_info struct */
    SHR_ALLOC(th_info_p, sizeof(scf_thread_info_t), "SCF_THREAD_HANDLE");
    SHR_NULL_CHECK(th_info_p, SHR_E_MEMORY);
    sal_memset(th_info_p, 0, sizeof(scf_thread_info_t));

    /* Init th_info */
    th_info_p->unit = unit;
    th_info_p->state = SCF_THREAD_STATE_STOP;
    scf_th_info_p[unit] = th_info_p;

    /* Create and start WAL reader thread */
    walr_tc[unit] = shr_thread_start("bcmptmWalReader",
                                            BCMPTM_CFG_WALR_THREAD_PRIO,
                                            scf_thread_func,
                                            th_info_p);
    SHR_IF_ERR_MSG_EXIT(
        ((walr_tc[unit] == NULL) ? SHR_E_FAIL : SHR_E_NONE),
        (BSL_META_U(unit, "Failed to create WAL reader thread.\n")));
exit:
    if (SHR_FUNC_ERR()) {
        if (walr_tc[unit]) {
            SHR_IF_ERR_MSG_EXIT(
                shr_thread_stop(walr_tc[unit],
                                BCMPTM_CFG_WALR_THREAD_STOP_WAIT_USEC),
                (BSL_META_U(unit, "WAL reader thread stop failed ")));
            walr_tc[unit] = NULL;
        }
        if (th_info_p) {
            SHR_FREE(th_info_p);
        }
        scf_th_info_p[unit] = NULL;

        if (scf_mutex[unit] != NULL) {
            sal_mutex_destroy(scf_mutex[unit]);
            scf_mutex[unit] = NULL;
        }
        walr_has_scf_mutex[unit] = FALSE;
        serc_needs_scf_mutex[unit] = FALSE;
        serc_has_scf_mutex[unit] = FALSE;
    } else {
    }
    SHR_FUNC_EXIT();
}

/* WAL reader cleanup must execute AFTER WAL writer cleanup.
 */
int
bcmptm_walr_cleanup(int unit)
{
    scf_thread_info_t *th_info_p;
    SHR_FUNC_ENTER(unit);

    th_info_p = scf_th_info_p[unit];
    if (th_info_p == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    /* walr_cleanup() is called AFTER wal_cleanup() is done.
     * This ensures that WAL is empty - and so ok to stop WAL reader thread.
     */
    if (walr_tc[unit]) {
        SHR_IF_ERR_MSG_EXIT(
            shr_thread_stop(walr_tc[unit],
                            BCMPTM_CFG_WALR_THREAD_STOP_WAIT_USEC),
            (BSL_META_U(unit, "WAL reader thread stop failed ")));
        SHR_IF_ERR_MSG_EXIT(
            (th_info_p->state != SCF_THREAD_STATE_STOP ? SHR_E_INTERNAL
                                                       : SHR_E_NONE),
            (BSL_META_U(unit, "WAL reader thread state is not STOP !!")));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    walr_tc[unit] = NULL;

    /* Release th_info struct */
    SHR_FREE(th_info_p);
    scf_th_info_p[unit] = NULL;

    /* Schan_fifo lock destroy */
    if (scf_mutex[unit] != NULL) {
        sal_mutex_destroy(scf_mutex[unit]);
    }
    walr_has_scf_mutex[unit] = FALSE;
    serc_needs_scf_mutex[unit] = FALSE;
    serc_has_scf_mutex[unit] = FALSE;

exit:
    SHR_FUNC_EXIT();
}
