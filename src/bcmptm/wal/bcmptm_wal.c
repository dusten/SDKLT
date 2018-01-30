/*! \file bcmptm_wal.c
 *
 * This file implements WAL sub-block of PTM
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
#include <sal/sal_sleep.h>
#include <sal/sal_assert.h>
#include <sal/sal_mutex.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_schanfifo.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "./bcmptm_wal_local.h"
#include "./bcmptm_walr_local.h"


/*******************************************************************************
 * Defines
 */
#define HAL_DMA_CRASH_RECOVERY_SUPPORTED FALSE 

/* Configuration - can be different per unit */
#define BCMPTM_CFG_WAL_DMA_OP_COUNT_THR 2
#define BCMPTM_CFG_WAL_SLAM_OP_COUNT_THR 5

#define BCMPTM_CFG_WAL_TRANS_MAX_COUNT 1448
#define BCMPTM_CFG_WAL_MSG_MAX_COUNT 1448
#define BCMPTM_CFG_WAL_WORDS_BUF_MAX_COUNT 491520
#define BCMPTM_CFG_WAL_OPS_INFO_MAX_COUNT 49152
#define BCMPTM_CFG_WAL_UNDO_OPS_INFO_MAX_COUNT 49152
#define BCMPTM_CFG_WAL_UNDO_WORDS_BUF_MAX_COUNT 491520
/* Max number of (cwords, dwords) that can be accommodated in one schan_fifo.
 * WAL writer will chop off ops for a single transaction into multiple chunks
 * each of which will have req_entry_wsize <= what is defined below */
#define BCMPTM_CFG_WAL_MAX_WORDS_IN_MSG 352 /* 16 max size writes = 16 * 22
                                               words */

/* Max number of ops that can be accommodated in one schan_fifo.
 * WAL writer will chop off ops for a single transaction into multiple chunks
 * each of which will have num_ops <= what is defined below */
#define BCMPTM_CFG_WAL_MAX_OPS_IN_MSG 1000000 /* No limit */

/* Max number of cwords in any Write or Read op
 * For XGS devices it is 2.
 * WAL can handle variable for every op */
#define BCMPTM_CFG_WAL_MAX_CWORDS_WSIZE 4 /* For XGS */

/* Max number of msgs in trans
 * - This option is dont-care for normal mode
 * - This option is meaningful only in auto-commit mode.
 * - When in auto-commit mode, trans will get auto-committed when num_msgs in
 * trans becomes equal to this value.
 */
#define BCMPTM_CFG_WAL_MAX_MSGS_IN_TRANS 1000000 /* No limit */

#define BCMPTM_CFG_WAL_SCF_NUM_CHANS 2
/* 0  => use pio,
 * 1  => use only one schanfifo channel,
 * >1 => use two channels */

#define BCMPTM_CFG_WAL_SCF_MAX_POLLS 0 /* use default */

#define BSL_LOG_MODULE BSL_LS_BCMPTM_WAL
/* #define BCMPTM_PRINTF_DEBUG */

#define WAL_USLEEP_TIME 1000 /* uSec */
#define WAL_AVAIL_SPACE_RETRY_COUNT_MAX 10000
#define WAL_AVAIL_SPACE_FOR_SLAM_RETRY_COUNT_MAX 1000
#define WAL_EMPTY_CHECK_RETRY_COUNT 1000000
#define WAL_EMPTY_CHECK_USLEEP_TIME 100000 /* uSec */

/* Declare error if cannot get lock for WSTATE after this time.
 * No retries */
#define WSTATE_LOCK_WAIT_USEC 1000000
#define SLICE_MODE_LOCK_WAIT_USEC 1000000
#define ENTRY_MODE_LOCK_WAIT_USEC 1000000

#define WAL_WORDS_BUF_BPTR          bcmptm_wal_words_buf_bptr[unit]
#define WAL_OPS_INFO_BPTR           bcmptm_wal_ops_info_bptr[unit]
#define WAL_TRANS_BPTR              wal_trans_base_ptr[unit]
#define UNDO_OPS_INFO_BPTR          undo_ops_info_base_ptr[unit]
#define UNDO_WORDS_BUF_BPTR         undo_words_buf_base_ptr[unit]
#define WSTATE_BPTR                 wstate_base_ptr[unit]

#define WSTATE(field) (WSTATE_BPTR->field)

#define WAL_MSG_IDX_MAX (cfg_wal_msg_max_count[unit] - 1)
#define WAL_OPS_INFO_IDX_MAX (cfg_wal_ops_info_max_count[unit] - 1)
#define WAL_TRANS_IDX_MAX (cfg_wal_trans_max_count[unit] - 1)
#define WAL_WORDS_BUF_IDX_MAX (cfg_wal_words_buf_max_count[unit] - 1)
#define WAL_UNDO_OPS_INFO_IDX_MAX (cfg_undo_ops_info_max_count[unit] - 1)
#define WAL_UNDO_WORDS_BUF_IDX_MAX (cfg_undo_words_buf_max_count[unit] - 1)

#define C_TRANS_IDX_ILL 0xFFFFFFFF
#define C_MSG_IDX_ILL 0xFFFFFFFF

#define BCMPTM_CFG_WAL_ALIGN_EN TRUE
/* #define BCMPTM_CFG_WAL_ALIGN_EN bcmptm_wal_scf_num_chans[unit] */

#define SLICE_MAX 511 /* Any number > 511 means -1 */


/*******************************************************************************
 * Typedefs
 */
typedef struct wal_trans_s {
    uint32_t cseq_id;
    uint32_t f_msg_idx;       /* 1st msg in this trans - for HA replay (idx for wal_msgs[]) */
    uint32_t num_msgs;        /* total num of messages in this trans - for HA replay */

    /* needed for quick abort and also for error checking */
    uint32_t num_ops;         /* total num of ops in this trans */
    uint32_t num_words;       /* total num of words in this trans */
    uint32_t slam_msg_count; /* total num of slam_msgs in this trans */

    bcmptm_trans_cb_f notify_fn_ptr;
    void *notify_info;
} wal_trans_t;

typedef enum {
    TRANS_STATE_IDLE = 0,      /* will need to alloc new trans and new msg */
    TRANS_STATE_IN_MSG,        /* must add new op to msg */
    TRANS_STATE_NEED_NEW_MSG,  /* will need to alloc new msg */
    TRANS_STATE_BLOCKING_READ  /* will need to alloc new msg */
} wal_trans_state_t;

typedef struct wstate_s {
    bool mreq_state_in_trans;
    uint32_t mreq_cseq_id;

    wal_trans_state_t c_trans_state;
    uint32_t c_trans_idx; /* Needed to init trans_idx field in msg.
                             Also needed for abort */
    uint32_t c_msg_idx;   /* Needed when we have trans with multi-messages */
    uint32_t c_msg_commit_count; /* num of msgs committed */

    /* next: where next msg, op, etc will be written */
    uint32_t n_trans_idx;
    uint32_t n_msg_idx;
    uint32_t n_wal_ops_info_idx;
    uint32_t n_words_buf_idx;
    uint32_t n_undo_ops_info_idx;
    uint32_t n_undo_words_buf_idx;
    uint32_t n_umsg_idx;

    uint32_t avail_trans_count;
    uint32_t avail_msg_count;
    uint32_t avail_wal_ops_info_count;
    uint32_t avail_words_buf_count;
    uint32_t avail_undo_ops_info_count;
    uint32_t avail_undo_words_buf_count;

    uint32_t n_rdr_msg_idx;  /* Next msg_idx that rdr is expecting to be
                                serviced */
    uint32_t rdr_msg_count;  /* Num of msgs received for given transaction */
    bool rdr_ignore_op_err;

    /* Info to speedup Abort
     * idx can be restored by setting
     *     n_var_idx = a_var_idx;
     * But for restoring avail_counts, we must traverse each msg and release
     * resources (we cannot store a_avail_count - because WAL reader may
     * have * released resources for other msgs/trans) */
    /* uint32_t a_trans_idx; same as c_trans_idx */
    uint32_t a_msg_idx;
    uint32_t a_wal_ops_info_idx;
    uint32_t a_words_buf_idx;
    uint32_t a_umsg_idx;
} wstate_t;

/*! Info to be stored in WAL to allow undo of ptcache and also help with SER
 * correction.
 */
typedef struct wal_undo_ops_s {
    /*! sid to be used for ptcache_undo */
    bcmdrd_sid_t undo_sid;

    /*! dyn_info to be used for ptcache_undo */
    bcmbd_pt_dyn_info_t undo_dyn_info;

    

    /*! cache_rsp_vbit for ptcache_undo */
    bool undo_cache_vbit;

    /*! dfid for ptcache_undo */
    uint16_t undo_dfid;

    /*! ltid for ptcache_undo */
    bcmltd_sid_t undo_ltid;
} wal_undo_ops_t;


/*******************************************************************************
 * Private variables
 */
static uint32_t cfg_wal_trans_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_msg_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_ops_info_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_words_buf_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_undo_ops_info_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_undo_words_buf_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_max_words_in_msg[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_max_ops_in_msg[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_max_cwords_wsize[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_max_msgs_in_trans[BCMDRD_CONFIG_MAX_UNITS];

static wal_trans_t *wal_trans_base_ptr[BCMDRD_CONFIG_MAX_UNITS];
static wal_undo_ops_t *undo_ops_info_base_ptr[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t *undo_words_buf_base_ptr[BCMDRD_CONFIG_MAX_UNITS];

static wstate_t *wstate_base_ptr[BCMDRD_CONFIG_MAX_UNITS];

static sal_mutex_t wstate_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool have_wstate_mutex[BCMDRD_CONFIG_MAX_UNITS];

static int cfg_wal_mode[BCMDRD_CONFIG_MAX_UNITS];

static uint8_t mc_count[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_RM_MC_GROUP_COUNT_MAX];

static bool wal_disable_undo[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t wal_scf_cmd_mem_wsize[BCMDRD_CONFIG_MAX_UNITS];

static bool wal_msg_dq_enable[BCMDRD_CONFIG_MAX_UNITS];
static bool dma_avail[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Global vars exported
 */
uint64_t bcmptm_wal_words_buf_bpa[BCMDRD_CONFIG_MAX_UNITS];
uint32_t *bcmptm_wal_words_buf_bptr[BCMDRD_CONFIG_MAX_UNITS];
bcmptm_wal_ops_info_t *bcmptm_wal_ops_info_bptr[BCMDRD_CONFIG_MAX_UNITS];

bcmptm_wal_msg_t *bcmptm_wal_msg_bptr[BCMDRD_CONFIG_MAX_UNITS];

bool *bcmptm_wal_msg_dq_ready_bptr[BCMDRD_CONFIG_MAX_UNITS];
bool *bcmptm_wal_msg_dq_ready_ptr[BCMDRD_CONFIG_MAX_UNITS];

uint32_t bcmptm_wal_msg_dq_idx[BCMDRD_CONFIG_MAX_UNITS];
bcmptm_wal_msg_t *bcmptm_wal_msg_dq_ptr[BCMDRD_CONFIG_MAX_UNITS];

int bcmptm_wal_scf_num_chans[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
/* Use sbusdma to execute read
 *
 * ASSUME: Caller has taken scf_mutex before calling this function.
 */
#define DMA_CWORDS_ARRAY_SIZE 4
static int
wal_dma_read(int unit,
             bcmdrd_sid_t sid,
             bcmbd_pt_dyn_info_t *pt_dyn_info,
             void *pt_ovrr_info,
             uint32_t entry_count,
             uint32_t *rsp_entry_words,
             size_t rsp_buf_wsize)
{
    uint32_t cwords_array[DMA_CWORDS_ARRAY_SIZE], num_cwords, pt_entry_wsize;
    int tmp_rv = SHR_E_NONE;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmbd_pt_cmd_hdr_get(unit,
                             sid,
                             pt_dyn_info,
                             pt_ovrr_info,
                             bcmdrd_pt_is_counter(unit, sid) ?
                             BCMBD_PT_CMD_COUNT :
                             BCMBD_PT_CMD_READ,
                             DMA_CWORDS_ARRAY_SIZE,
                             cwords_array,
                             &num_cwords));
    assert(num_cwords != 0);
    pt_entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Prepare dma_data */
    sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
    dma_data.start_addr = cwords_array[1]; /* Addr beat */
    dma_data.data_width = pt_entry_wsize;  /* Num of data beats */
     /* dma_data.addr_gap = 0; */          /* Gap between addresses */
    dma_data.op_code = cwords_array[0];    /* Opcode beat */
    dma_data.op_count = entry_count;       /* Num of locations */
    dma_data.buf_paddr = bcmptm_wal_words_buf_bpa[unit];
        /* Phys addr for slam data.
         * Must be on 4 Byte boundary.
         * Should be on 16 Byte boundary for optimum performance.
         * For read, wal is empty, so words_buf_bpa is pa for DMA. */
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
    dma_work.flags = SBUSDMA_WF_READ_CMD; /* | SBUSDMA_WF_INT_MODE */
    /* dma_work.state = 0; */

    tmp_rv = bcmbd_sbusdma_light_work_init(unit, &dma_work);
    if (tmp_rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
        (BSL_META_U(unit, "bcmptm_wal_dma_read.sbusdma_light_work "
                    "returned rv=%0d\n"), tmp_rv));
    } else {
        tmp_rv = bcmbd_sbusdma_work_execute(unit, &dma_work);
        if (tmp_rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "bcmptm_wal_dma_read.sbusdma_work_"
                            "execute returned rv=%0d\n"), tmp_rv));
        } else { /* Copy data from DMA buf (WAL) to user buffer */
            uint32_t exp_buf_wsize = entry_count * pt_entry_wsize;
            SHR_IF_ERR_MSG_EXIT(
                (rsp_buf_wsize < exp_buf_wsize) ? SHR_E_PARAM : SHR_E_NONE,
                (BSL_META_U(unit, "rsp_buf_wsize is not enough\n")));
            sal_memcpy(rsp_entry_words, WAL_WORDS_BUF_BPTR, 4 * exp_buf_wsize);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/* Must be called only when WSTATE(c_trans_state) is NEED_NEW_MSG,IN_MSG
 */
static int
wal_ptcache_undo(int unit, wal_trans_t *trans_ptr)
{
    uint32_t c_msg_idx, entry_count,
             f_undo_ops_info_idx, c_undo_ops_info_idx, c_undo_num_ops,
             f_undo_words_buf_idx, c_undo_words_buf_idx, c_undo_num_words;
    bcmptm_wal_msg_t *c_msg_ptr;
    wal_undo_ops_t *c_undo_ops_info_ptr;
    SHR_FUNC_ENTER(unit);

    c_msg_idx = WSTATE(c_msg_idx); /* last (latest) msg */
        /* c_msg_idx is last msg in trans even if c_trans_state=NEED_NEW_MSG */
    c_msg_ptr = BCMPTM_WAL_MSG_BPTR + c_msg_idx;

    /* Need to traverse the ops (and hence msgs) from latest (last)
     * to oldest (first) */
    do { /* foreach trans.msg */
        /* ops in msg */
        f_undo_ops_info_idx = c_msg_ptr->undo_ops_info_idx; /* 1st op */
        switch (c_msg_ptr->cmd) {
        case BCMPTM_WAL_MSG_CMD_WRITE:
            c_undo_num_ops = c_msg_ptr->num_ops;
            entry_count = 1;
            break;
        case BCMPTM_WAL_MSG_CMD_SLAM:
            c_undo_num_ops = 1;
            entry_count = c_msg_ptr->num_ops;
            break;
        default:
            c_undo_num_ops = 0;
            entry_count = 0;
            break;
        } /* c_msg_ptr->cmd */

        /* words in msg */
        f_undo_words_buf_idx = c_msg_ptr->undo_words_buf_idx; /* 1st word */
        c_undo_num_words = c_msg_ptr->undo_num_words;

        /* In each msg:
         * - all undo_words will be consecutive (no wraparound guaranteed)
         * - all undo_ops will be consecutive (no wraparound guaranteed)
         * - first undo_op in msg is oldest
         * - last  undo_op in msg is latest
         * - we want to go from latest to oldest
         */

        /* Work on msg only if there are undo ops */
        if (c_undo_num_ops) {
            assert(c_undo_num_words);

            /* last op in msg */
            c_undo_ops_info_idx = f_undo_ops_info_idx + (c_undo_num_ops - 1);
                /* Note: RHS can become 0xFFFFFFFF, hence used () on RHS
                 *       But this requires 1st RHS term to be really big which
                 *       implies we have 4G deep ops array -impossible */
            c_undo_ops_info_ptr = UNDO_OPS_INFO_BPTR + c_undo_ops_info_idx;

            /* last+1 word in msg */
            /* Note: Will following work if RHS is 0 ?
             *       It requires 1st RHS term to be really big which implies we
             *       have 4G deep words array - impossible. */
            c_undo_words_buf_idx = f_undo_words_buf_idx + c_undo_num_words;
            do { /* foreach msg.op */
                if (c_msg_ptr->cmd == BCMPTM_WAL_MSG_CMD_SLAM) {
                    c_undo_words_buf_idx = f_undo_words_buf_idx;
                } else {
                    /* 1st word of this op (is also last+1 word of prev op) */
                    c_undo_words_buf_idx -=
                        bcmdrd_pt_entry_wsize(unit,
                                              c_undo_ops_info_ptr->undo_sid);
                }
                assert(c_undo_ops_info_idx >= f_undo_ops_info_idx);
                assert(c_undo_words_buf_idx >= f_undo_words_buf_idx);

                SHR_IF_ERR_EXIT(
                    bcmptm_ptcache_update(unit,
                                          c_undo_ops_info_ptr->undo_sid,
                                          &(c_undo_ops_info_ptr->undo_dyn_info),
                                          NULL, 
                                          entry_count,
                                          UNDO_WORDS_BUF_BPTR +
                                          c_undo_words_buf_idx,

                                          c_undo_ops_info_ptr->undo_cache_vbit,
                                          c_undo_ops_info_ptr->undo_ltid,
                                          c_undo_ops_info_ptr->undo_dfid));

                if (c_undo_ops_info_idx == f_undo_ops_info_idx) {
                    SHR_IF_ERR_EXIT(
                        (c_undo_words_buf_idx != f_undo_words_buf_idx) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
                    break; /* done with all ops in this msg */
                } else {
                    c_undo_ops_info_idx--;
                    c_undo_ops_info_ptr--;
                }
            } while (1); /* foreach msg.op */
        } /* process ops in cur msg */

        /* Go to earlier(older) msg
         * (ops, data_words) in single msg cannot cross wraparound boundary
         * But, msgs in trans can cross wraparound boundary */
        if (c_msg_idx == trans_ptr->f_msg_idx) {
            /* Finished processing ops for first (oldest) msg */
            break; /* done */
        } else {
            if (c_msg_idx == 0) {
                c_msg_idx = WAL_MSG_IDX_MAX;
                c_msg_ptr = BCMPTM_WAL_MSG_BPTR + c_msg_idx;
            } else {
                c_msg_idx--;
                c_msg_ptr--;
            }
        }
    } while (1); /* foreach trans.msg */

exit:
    SHR_FUNC_EXIT();
}

static void
print_wstate_info(int unit, bool error, const char *msg_prefix)
{
#if defined(BCMPTM_PRINTF_DEBUG)
    wstate_t *wstate_ptr = WSTATE_BPTR;
    BSL_LOG(BSL_LOG_MODULE | (error ? BSL_ERROR : BSL_VERBOSE),
            (BSL_META_U(unit,
        "%s: Wstate_info: c_trans_state=%s, c_trans_idx=%0u, c_msg_idx=%0u, "
        "c_msg_commit_count=%0u, "

        "n_trans_idx=%0u, "
        "n_msg_idx=%0u, "
        "n_wal_ops_info_idx=%0u, "
        "n_words_buf_idx=%0u, "
        "n_undo_ops_info_idx=%0u, "
        "n_undo_words_buf_idx=%0u, "
        "n_umsg_idx=%0u, "

        "avail_trans_count=%0u, "
        "avail_msg_count=%0u, "
        "avail_wal_ops_info_count=%0u, "
        "avail_words_buf_count=%0u, "
        "avail_undo_ops_info_count=%0u, "
        "avail_undo_words_buf_count=%0u, "

        "n_rdr_msg_idx=%0u, "
        "rdr_msg_count=%0u, "

        "a_msg_idx=%0u, "
        "a_wal_ops_info_idx=%0u, "
        "a_words_buf_idx=%0u, "
        "a_umsg_idx=%0u \n\n"),

        msg_prefix,
        (WSTATE(c_trans_state) == TRANS_STATE_IN_MSG ? "IN_MSG" :
         WSTATE(c_trans_state) == TRANS_STATE_NEED_NEW_MSG ? "NEED_NEW_MSG"
                                                           : "IDLE"),
        WSTATE(c_trans_idx),
        WSTATE(c_msg_idx),
        WSTATE(c_msg_commit_count),

        wstate_ptr->n_trans_idx,
        WSTATE(n_msg_idx),
        WSTATE(n_wal_ops_info_idx),
        WSTATE(n_words_buf_idx),
        WSTATE(n_undo_ops_info_idx),
        WSTATE(n_undo_words_buf_idx),
        WSTATE(n_umsg_idx),

        WSTATE(avail_trans_count),
        WSTATE(avail_msg_count),
        WSTATE(avail_wal_ops_info_count),
        WSTATE(avail_words_buf_count),
        WSTATE(avail_undo_ops_info_count),
        WSTATE(avail_undo_words_buf_count),

        WSTATE(n_rdr_msg_idx),
        WSTATE(rdr_msg_count),

        WSTATE(a_msg_idx),
        WSTATE(a_wal_ops_info_idx),
        WSTATE(a_words_buf_idx),
        WSTATE(a_umsg_idx)));
#endif
}

static void
print_msg_info(int unit, bool error, uint32_t msg_idx,
               bcmptm_wal_msg_t *wal_msg_ptr,
               const char *msg_prefix)
{
    BSL_LOG(BSL_LOG_MODULE | (error ? BSL_ERROR : BSL_VERBOSE),
       (BSL_META_U(unit,
        "%s: Msg_info: umsg_idx=%0u, trans_idx=%0u, msg_idx=%0u, cmd=%s, "
        "words_buf_idx=%0u, req_entry_wsize=%0u, "
        "undo_ops_info_idx=%0u, "
        "num_ops=%0u, undo_words_buf_idx=%0u, "
        "undo_num_words=%0u, num_words_skipped=%0u \n\n"),
        msg_prefix, wal_msg_ptr->umsg_idx, wal_msg_ptr->trans_idx, msg_idx,
        wal_msg_ptr->cmd == BCMPTM_WAL_MSG_CMD_WRITE ? "WRITE" :
        wal_msg_ptr->cmd == BCMPTM_WAL_MSG_CMD_SLAM ? "SLAM" :
        wal_msg_ptr->cmd == BCMPTM_WAL_MSG_CMD_NOP ? "NOP" : "THREAD_EXIT",
        wal_msg_ptr->words_buf_idx, wal_msg_ptr->req_entry_wsize,
        wal_msg_ptr->undo_ops_info_idx,
        wal_msg_ptr->num_ops, wal_msg_ptr->undo_words_buf_idx,
        wal_msg_ptr->undo_num_words, wal_msg_ptr->num_words_skipped));
}

static void
print_trans_info(int unit, bool error, uint32_t trans_idx,
                 wal_trans_t *trans_ptr, const char *msg_prefix)
{
    BSL_LOG(BSL_LOG_MODULE | (error ? BSL_ERROR : BSL_VERBOSE),
       (BSL_META_U(unit,
        "%s: Trans_info: trans_idx=%0u, cseq_id=%0u, "
        "f_msg_idx=%0u, num_msgs=%0u, num_ops=%0u, "
        "num_words=%0u, "
        "slam_msg_count=%0u, "
        "notify_fn=%s \n\n"),
        msg_prefix, trans_idx, trans_ptr->cseq_id, trans_ptr->f_msg_idx,
        trans_ptr->num_msgs, trans_ptr->num_ops,
        trans_ptr->num_words,
        trans_ptr->slam_msg_count,
        trans_ptr->notify_fn_ptr ? "yes" : "no"));
}

/* !! MUST NOT USE in WAL reader context */
static int
wal_wstate_lock(int unit)
{
    int tmp_rv;
    if (have_wstate_mutex[unit]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! Already have wstate_mutex. Should not be "
                        "requesting again \n")));
        tmp_rv = SHR_E_INTERNAL;
    } else {
        tmp_rv = sal_mutex_take(wstate_mutex[unit],
                                WSTATE_LOCK_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            have_wstate_mutex[unit] = TRUE;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Could not get wstate_mutex \n")));
        }
    }
    return tmp_rv;
}

/* !! MUST NOT USE in WAL reader context */
static int
wal_wstate_unlock(int unit)
{
    int tmp_rv;
    if (!have_wstate_mutex[unit]) {
        tmp_rv = SHR_E_INTERNAL;
    } else {
        tmp_rv = sal_mutex_give(wstate_mutex[unit]);
        if (tmp_rv == SHR_E_NONE) {
            have_wstate_mutex[unit] = FALSE;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Could not release wstate_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
wal_empty_check(int unit, bool show_not_empty_values,
                bool *empty, wal_trans_state_t *wal_c_trans_state)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(wal_wstate_lock(unit));
    *empty = FALSE;
    *wal_c_trans_state = WSTATE(c_trans_state);
    if ((WSTATE(avail_trans_count) == cfg_wal_trans_max_count[unit])
        && (WSTATE(avail_msg_count) == cfg_wal_msg_max_count[unit])
        && (WSTATE(avail_wal_ops_info_count) == cfg_wal_ops_info_max_count[unit])
        && (WSTATE(avail_words_buf_count) == cfg_wal_words_buf_max_count[unit])
        && (WSTATE(avail_undo_ops_info_count) == cfg_undo_ops_info_max_count[unit])
        && (WSTATE(avail_undo_words_buf_count) == cfg_undo_words_buf_max_count[unit])
        && (WSTATE(n_msg_idx) == WSTATE(n_rdr_msg_idx))
       ) {
        *empty = TRUE;
    } else if (show_not_empty_values) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "wal_empty_check() failed... \n"
            "avail_trans_count: obs = %0u, exp = %0u \n,"
            "avail_msg_count: obs = %0u, exp = %0u \n,"
            "avail_wal_ops_info_count: obs = %0u, exp = %0u \n,"
            "avail_words_buf_count: obs = %0u, exp = %0u \n,"
            "avail_undo_ops_info_count: obs = %0u, exp = %0u \n,"
            "avail_undo_words_buf_count: obs = %0u, exp = %0u \n,"
            "c_trans_state: obs = %0d\n,"
            "n_msg_idx = %0u, n_rdr_msg_idx = %0u \n\n"),
            WSTATE(avail_trans_count), cfg_wal_trans_max_count[unit],
            WSTATE(avail_msg_count), cfg_wal_msg_max_count[unit],
            WSTATE(avail_wal_ops_info_count), cfg_wal_ops_info_max_count[unit],
            WSTATE(avail_words_buf_count), cfg_wal_words_buf_max_count[unit],
            WSTATE(avail_undo_ops_info_count), cfg_undo_ops_info_max_count[unit],
            WSTATE(avail_undo_words_buf_count), cfg_undo_words_buf_max_count[unit],
            WSTATE(c_trans_state),
            WSTATE(n_msg_idx), WSTATE(n_rdr_msg_idx)));
    }
    SHR_IF_ERR_EXIT(wal_wstate_unlock(unit));
exit:
    SHR_FUNC_EXIT();
}

static int
wal_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Free non-HA mems */
    if (BCMPTM_WAL_MSG_DQ_READY_BPTR != NULL) {
        sal_free(BCMPTM_WAL_MSG_DQ_READY_BPTR);
        BCMPTM_WAL_MSG_DQ_READY_BPTR = NULL;
    }

    /* Do not free HAL_DMA mem. Application will do that. */
    if ((bcmptm_wal_scf_num_chans[unit] || dma_avail[unit]) &&
        WAL_WORDS_BUF_BPTR &&
        !HAL_DMA_CRASH_RECOVERY_SUPPORTED) {
        size_t req_size;
        uint64_t pa;
        req_size = cfg_wal_words_buf_max_count[unit] * sizeof(uint32_t);
        req_size += (4 * sizeof(uint32_t));
        pa = (bcmptm_wal_words_buf_bpa[unit] - 16);
        bcmdrd_hal_dma_free(unit, req_size, WAL_WORDS_BUF_BPTR - 4, pa);
    }
    WAL_WORDS_BUF_BPTR = NULL;
    bcmptm_wal_words_buf_bpa[unit] = 0;

    /* Do not free HA mems. Application will do that. */
    WAL_TRANS_BPTR = NULL;
    BCMPTM_WAL_MSG_BPTR = NULL;
    WAL_OPS_INFO_BPTR = NULL;
    UNDO_OPS_INFO_BPTR = NULL;
    UNDO_WORDS_BUF_BPTR = NULL;
    WSTATE_BPTR = NULL;

/* exit: */
    SHR_FUNC_EXIT();
}

static int
do_alloc(int unit, bool warm, uint32_t in_req_size, const char *wal_sub_name,
         uint8_t sub_id, uint32_t ref_sign, uint32_t **alloc_ptr)
{
    uint32_t req_size, alloc_size, obs_sign;
    SHR_FUNC_ENTER(unit);
    *alloc_ptr = NULL;
    req_size = in_req_size + sizeof(uint32_t);
    alloc_size = req_size;
    *alloc_ptr = shr_ha_mem_alloc(unit,
                                  BCMMGMT_PTM_COMP_ID,
                                  sub_id,
                                  &alloc_size);
    SHR_NULL_CHECK(*alloc_ptr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm) {
        obs_sign = **alloc_ptr;
        SHR_IF_ERR_MSG_EXIT(
            obs_sign != ref_sign ? SHR_E_FAIL : SHR_E_NONE,
            (BSL_META_U(unit, "WB: %s: signature mismatch, "
                        "exp=0x%8x, obs=0x%8x\n"),
             wal_sub_name, ref_sign, obs_sign));
    } else { /* init ref_sign */
        sal_memset(*alloc_ptr, 0, alloc_size);
        **alloc_ptr = ref_sign;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "%s: req bytes = %0u, alloc bytes = %0u, \n"),
         wal_sub_name, req_size, alloc_size));
exit:
    SHR_FUNC_EXIT();
}

static int
wal_alloc(int unit, bool warm)
{
    uint32_t req_size, alloc_size, ref_sign, obs_sign, *alloc_ptr;
    uint64_t pa, *alloc_ptr64;
    SHR_FUNC_ENTER(unit);

    /* WAL_TRANS */
    req_size = cfg_wal_trans_max_count[unit] * sizeof(wal_trans_t);
    SHR_IF_ERR_EXIT(
        do_alloc(unit, warm, req_size, "WAL_TRANS",
                 BCMPTM_HA_SUBID_WAL_TRANS, BCMPTM_HA_SIGN_WAL_TRANS,
                 &alloc_ptr));
    WAL_TRANS_BPTR = (wal_trans_t *)(alloc_ptr + 1);

    /* WAL_MSG */
    req_size = cfg_wal_msg_max_count[unit] * sizeof(bcmptm_wal_msg_t);
    SHR_IF_ERR_EXIT(
        do_alloc(unit, warm, req_size, "WAL_MSG",
                 BCMPTM_HA_SUBID_WAL_MSG, BCMPTM_HA_SIGN_WAL_MSG,
                 &alloc_ptr));
    BCMPTM_WAL_MSG_BPTR = (bcmptm_wal_msg_t *)(alloc_ptr + 1);

    /* WAL_MSG_DQ_READY - Non_HA, Non_DMA mem (intentionally).
     * Will always 0 out contents (even for WB/ISSU/crash).
     * Hence no signature. */
    req_size = cfg_wal_msg_max_count[unit] * sizeof(bool);
    alloc_size = req_size;
    BCMPTM_WAL_MSG_DQ_READY_BPTR = sal_alloc(req_size, "WAL_MSG_DQ_READY array");
    SHR_NULL_CHECK(BCMPTM_WAL_MSG_DQ_READY_BPTR, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    sal_memset(BCMPTM_WAL_MSG_DQ_READY_BPTR, 0, alloc_size);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "WAL_MSG_DQ_READY: req bytes = %0u, alloc bytes = %0u, \n"),
         req_size, alloc_size));

    /* WAL_OPS_INFO */
    req_size = cfg_wal_ops_info_max_count[unit] * sizeof(bcmptm_wal_ops_info_t);
    SHR_IF_ERR_EXIT(
        do_alloc(unit, warm, req_size, "WAL_OPS_INFO",
                 BCMPTM_HA_SUBID_WAL_OPS_INFO, BCMPTM_HA_SIGN_WAL_OPS_INFO,
                 &alloc_ptr));
    WAL_OPS_INFO_BPTR = (bcmptm_wal_ops_info_t *)(alloc_ptr + 1);

    /* UNDO_OPS_INFO */
    if (wal_disable_undo[unit]) {
        UNDO_OPS_INFO_BPTR = NULL;
    } else {
        req_size = cfg_undo_ops_info_max_count[unit] * sizeof(wal_undo_ops_t);
        SHR_IF_ERR_EXIT(
            do_alloc(unit, warm, req_size, "WAL_UNDO_OPS_INFO",
                     BCMPTM_HA_SUBID_UNDO_OPS_INFO,
                     BCMPTM_HA_SIGN_WAL_UNDO_OPS_INFO, &alloc_ptr));
        UNDO_OPS_INFO_BPTR = (wal_undo_ops_t *)(alloc_ptr + 1);
    }

    /* UNDO_WORDS_BUF */
    if (wal_disable_undo[unit]) {
        UNDO_WORDS_BUF_BPTR = NULL;
    } else {
        req_size = cfg_undo_words_buf_max_count[unit] * sizeof(uint32_t);
        SHR_IF_ERR_EXIT(
            do_alloc(unit, warm, req_size, "WAL_UNDO_WORDS_BUF",
                     BCMPTM_HA_SUBID_UNDO_WORDS_BUF,
                     BCMPTM_HA_SIGN_WAL_UNDO_WORDS_BUF, &alloc_ptr));
        UNDO_WORDS_BUF_BPTR = alloc_ptr + 1;
    }

    /* WSTATE */
    req_size = sizeof(wstate_t);
    SHR_IF_ERR_EXIT(
        do_alloc(unit, warm, req_size, "WAL_WSTATE",
                 BCMPTM_HA_SUBID_WAL_WSTATE, BCMPTM_HA_SIGN_WAL_WSTATE,
                 &alloc_ptr));
    WSTATE_BPTR = (wstate_t *)(alloc_ptr + 1);


    
    req_size = cfg_wal_words_buf_max_count[unit] * sizeof(uint32_t);
    req_size += (4 * sizeof(uint32_t));
                /* Note: We want WAL_WORDS_BUF_BPTR on 16 Byte boundary.
                 *       So waste first 16 Bytes to store:
                 *       4 Byte signature, 4 Byte unused,
                 *       8 Byte wal_words_buf_bpa */
    alloc_size = req_size;
    alloc_ptr = NULL;
    if (bcmptm_wal_scf_num_chans[unit] || dma_avail[unit]) {
        alloc_ptr = bcmdrd_hal_dma_alloc(unit, req_size,
                                         "WAL_WORDS_BUF",
                                         &pa);
        alloc_size = req_size; /* assume alloc_ptr is not NULL (we got the mem) */
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        WAL_WORDS_BUF_BPTR = alloc_ptr + 4; /* Skip first 16 Bytes */
        ref_sign = BCMPTM_HA_SIGN_WAL_WORDS_BUF;
        alloc_ptr64 = (uint64_t *)(alloc_ptr + 2);
        if (warm && HAL_DMA_CRASH_RECOVERY_SUPPORTED) {
            obs_sign = *alloc_ptr;
            SHR_IF_ERR_MSG_EXIT(
                obs_sign != ref_sign ? SHR_E_FAIL : SHR_E_NONE,
                (BSL_META_U(unit, "WB: WAL.words_buf array signature mismatch, "
                            "exp=0x%8x, obs=0x%8x\n"),
                 ref_sign, obs_sign));
            /* Note: For WARM
             *       call hal_dma_alloc();
             *       stored prev_wal_words_buf_pa must match pa;
             *       do bcmdrd_hal_dma_alloc();
             *       assert(prev_wal_words_buf_pa == pa);
             *       Main point:
             *       alloc_ptr will change but physical address and
             *       contents (signature) of wal_words_buf should be same
             *       (preserved).
             */
            SHR_IF_ERR_MSG_EXIT(
                *alloc_ptr64 != (pa + 16) ? SHR_E_FAIL : SHR_E_NONE,
                (BSL_META_U(unit, "WB: WAL.words_buf array physical_addr "
                            "mismatch, exp= %" PRIx64 "obs= %" PRIx64 "\n"),
                 *alloc_ptr64, pa + 16));

            /* Re-init wal_words_buf_bpa */
            bcmptm_wal_words_buf_bpa[unit] = pa + 16; /* Skip first 16 Bytes */
        } else { /* init ref_sign */
            sal_memset(alloc_ptr, 0, alloc_size);
            bcmptm_wal_words_buf_bpa[unit] = pa + 16; /* Skip first 16 Bytes */
            *alloc_ptr64 = bcmptm_wal_words_buf_bpa[unit];
            *alloc_ptr = ref_sign;
        }
    } else { /* native_sim, no DMA, no schan_fifo */
        alloc_ptr = shr_ha_mem_alloc(unit,
                                     BCMMGMT_PTM_COMP_ID,
                                     BCMPTM_HA_SUBID_WAL_WORDS_BUF,
                                     &alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        WAL_WORDS_BUF_BPTR = alloc_ptr + 4; /* Skip first 16 Bytes */
        ref_sign = BCMPTM_HA_SIGN_WAL_WORDS_BUF;
        if (warm) {
            obs_sign = *alloc_ptr;
            SHR_IF_ERR_MSG_EXIT(
                obs_sign != ref_sign ? SHR_E_FAIL : SHR_E_NONE,
                (BSL_META_U(unit, "WB: WAL.words_buf array signature mismatch, "
                            "exp=0x%8x, obs=0x%8x\n"),
                 ref_sign, obs_sign));
        } else { /* init ref_sign */
            sal_memset(alloc_ptr, 0, alloc_size);
            bcmptm_wal_words_buf_bpa[unit] = 0; /* dont_care */
            *alloc_ptr = ref_sign;
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "WAL_WORDS_BUF: req bytes = %0u, alloc bytes = %0u, \n"),
         req_size, alloc_size));

exit:
    if (SHR_FUNC_ERR()) {
        wal_free(unit);
        SHR_FUNC_EXIT();
    }

    req_size = 0;

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "sizeof wal_trans_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(wal_trans_t), cfg_wal_trans_max_count[unit]));
    req_size += (uint32_t)sizeof(wal_trans_t) *  cfg_wal_trans_max_count[unit];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "sizeof bcmptm_wal_msg_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(bcmptm_wal_msg_t), cfg_wal_msg_max_count[unit]));
    req_size += (uint32_t)sizeof(bcmptm_wal_msg_t) * cfg_wal_msg_max_count[unit];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "sizeof bcmptm_wal_dq_ready_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(bool), cfg_wal_msg_max_count[unit]));
    req_size += (uint32_t)sizeof(bool) * cfg_wal_msg_max_count[unit];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "sizeof bcmptm_wal_ops_info_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(bcmptm_wal_ops_info_t), cfg_wal_ops_info_max_count[unit]));
    req_size += (uint32_t)sizeof(bcmptm_wal_ops_info_t) * cfg_wal_ops_info_max_count[unit];

    if (!wal_disable_undo[unit]) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sizeof wal_undo_ops_t = %0u Bytes, count = %0d\n"),
             (uint32_t) sizeof(wal_undo_ops_t), cfg_undo_ops_info_max_count[unit]));
        req_size += (uint32_t)sizeof(wal_undo_ops_t) * cfg_undo_ops_info_max_count[unit];

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sizeof undo_words = %0u Bytes, count = %0d\n"),
             (uint32_t) sizeof(uint32_t), cfg_undo_words_buf_max_count[unit]));
        req_size += (uint32_t)sizeof(uint32_t) * cfg_undo_words_buf_max_count[unit];
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "sizeof wal_words = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(uint32_t), cfg_wal_words_buf_max_count[unit]));
    req_size += (uint32_t)sizeof(uint32_t) * cfg_wal_words_buf_max_count[unit];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Total WAL size = %0d Bytes\n"), req_size));

    SHR_FUNC_EXIT();
}

/* Following does wraparound of n_words_buf_idx.
 * It ALSO decrements avail_words_buf_count.
 * Takes wstate_lock before accessing avail_words_count.
 * Leaves with wstate_locked.
 *
 * - Must give up the lock to wstate_lock while waiting for space to
 *   become available - because otherwise wal_reader cannot release
 *   the resources */
static int
avail_words_skip(int unit,
                 uint32_t num_words_req,
                 bool *close_msg_before_this_op,
                 uint32_t *num_words_skipped)
{
    uint32_t num_words_till_max_idx; /* may not be available */
    uint32_t retry_count = 0;
    bool words_fail = FALSE, done = FALSE;
    SHR_FUNC_ENTER(unit);

    /* Default */
    *close_msg_before_this_op = FALSE;
    /* avail_skip() will be called by align_skip() with non-zero
     * num_words_skipped */

    do {
        if (!have_wstate_mutex[unit]) {
            SHR_IF_ERR_EXIT(wal_wstate_lock(unit));
        }
        if (WSTATE(avail_words_buf_count) < num_words_req) {
            retry_count++;
            SHR_IF_ERR_EXIT(wal_wstate_unlock(unit));
            sal_usleep(WAL_USLEEP_TIME);
        } else {
            done = TRUE; /* keep the wstate_lock */
        }
    } while (!done && (retry_count < WAL_AVAIL_SPACE_RETRY_COUNT_MAX));
    if (!done) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Not enough space in words buffer. \n")));
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    num_words_till_max_idx = (cfg_wal_words_buf_max_count[unit] -
                              WSTATE(n_words_buf_idx));
    words_fail = (num_words_till_max_idx < num_words_req);

    if (words_fail) {
        *close_msg_before_this_op = TRUE;
        WSTATE(n_words_buf_idx) = 0;
        WSTATE(avail_words_buf_count) -= num_words_till_max_idx;
        *num_words_skipped += num_words_till_max_idx;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Coverage: close_msg due to words wrap = %s, "
                        "skipped_words = %0u \n"),
                        words_fail? "TRUE" : "FALSE",
                        *num_words_skipped));
    }

    /* This function leaves with wstate_locked */
    SHR_IF_ERR_EXIT(!have_wstate_mutex[unit] ? SHR_E_INTERNAL : SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}


static int
align_skip(int unit,
           uint32_t num_words_req,
           uint32_t *num_words_skipped)
{
    uint8_t lsb, inc_val;
    SHR_FUNC_ENTER(unit);
    lsb = WSTATE(n_words_buf_idx) & 0x3;
    if (lsb) {
        inc_val = 4 - lsb; /* quad-word alignment */

        assert(*num_words_skipped == 0);
        assert(WSTATE(avail_words_buf_count >= inc_val));
        assert((WSTATE(n_words_buf_idx) + inc_val) <=
               cfg_wal_words_buf_max_count[unit]);

        *num_words_skipped += inc_val;
        WSTATE(avail_words_buf_count) -= inc_val; /* can become 0 */
        WSTATE(n_words_buf_idx) += inc_val;
        if (WSTATE(n_words_buf_idx) == cfg_wal_words_buf_max_count[unit]) {
            /* Example:
             * max_count=20, max_buf_idx=18, req_entry_words=6
             *
             * If buf_idx was 17, 18, 19 lsb is 1,2,3 and inc_val is 3,2,1
             * so buf_idx + inc_val = 20, and thus buf_idx should become 0
             */
            WSTATE(n_words_buf_idx) = 0;
        } else { /* check boundary_wrap after align_skip */
            /* Example:
             * max_count=20, max_buf_idx=18, req_entry_words=6, avail_count=20
             *
             * n_buf_idx was 15 when align_skip() is called.
             * lsb=3, inc_val=1, new n_buf_idx=16, new avail_count=19
             *
             * Now avail_count(19) > req_entry_words(6), BUT, we cannot start
             * msg at n_buf_idx=16 because this would mean 6 words will include
             * boundary_wrap. Hence we need to do wraparound_skip AFTER
             * align_skip.
             *
             * 'if' portion of this else already made n_buf_idx cross
             * wrap_boundary, hence we don't need to do wraparound_skip in 'if'
             * portion.
             */
            bool close_msg_before_this_op;
            SHR_IF_ERR_EXIT(
                avail_words_skip(unit,
                                 num_words_req,
                                 &close_msg_before_this_op, /* x */
                                 num_words_skipped));
        } /* check boundary_wrap after align_skip */
    } /* lsb */
exit:
    SHR_FUNC_EXIT();
}

/* Caller guarantees that:
 * 1. avail words is >= num_words_req
 * 2. we dont have 'max_boundary' in avail words space */
static int
wstate_words_buf_idx_inc(int unit, uint32_t num_words_req,
                         bool *close_msg_after_this_op)
{
    uint32_t num_words_till_max_idx;
    SHR_FUNC_ENTER(unit);

    num_words_till_max_idx =
        (cfg_wal_words_buf_max_count[unit] - WSTATE(n_words_buf_idx));

    SHR_IF_ERR_EXIT(
        (num_words_till_max_idx < num_words_req) ? SHR_E_INTERNAL : SHR_E_NONE);

    if (num_words_till_max_idx == num_words_req) {
        WSTATE(n_words_buf_idx) = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        /* num_words_till_max_id > num_words_req so ok to inc buf_idx */
        WSTATE(n_words_buf_idx) += num_words_req;
        /* *close_msg_after_this_op = FALSE; Intentionally commented because
         * wstate_wal_ops_info_idx_inc() can also set it */
    }
exit:
    SHR_FUNC_EXIT();
}

/* Caller guarantees that:
 * 1. avail words is >= num_words_req
 * 2. we dont have 'max_boundary' in avail words space */
static int
wstate_undo_words_buf_idx_inc(int unit, uint32_t num_undo_words_req,
                              bool *close_msg_after_this_op)
{
    uint32_t num_undo_words_till_max_idx;
    SHR_FUNC_ENTER(unit);

    num_undo_words_till_max_idx =
        (cfg_undo_words_buf_max_count[unit] -
         WSTATE(n_undo_words_buf_idx));

    SHR_IF_ERR_EXIT(
        (num_undo_words_till_max_idx < num_undo_words_req) ? SHR_E_INTERNAL
                                                           : SHR_E_NONE);

    if (num_undo_words_till_max_idx == num_undo_words_req) {
        WSTATE(n_undo_words_buf_idx) = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        /* num_words_till_max_id > num_words_req so ok to inc buf_idx */
        WSTATE(n_undo_words_buf_idx) += num_undo_words_req;
        /* *close_msg_after_this_op = FALSE; Intentionally commented because
         * wstate_wal_ops_info_idx_inc() can also set it */
    }
exit:
    SHR_FUNC_EXIT();
}

static int
wstate_trans_idx_inc(int unit)
{
    if (WSTATE(n_trans_idx) == WAL_TRANS_IDX_MAX) {
        WSTATE(n_trans_idx) = 0;
    } else {
        WSTATE(n_trans_idx) += 1;
    }
    return SHR_E_NONE;
}

static int
wstate_msg_idx_inc(int unit)
{
    if (WSTATE(n_msg_idx) == WAL_MSG_IDX_MAX) {
        WSTATE(n_msg_idx) = 0;
    } else {
        WSTATE(n_msg_idx) += 1;
    }
    return SHR_E_NONE;
}

static int
wstate_wal_ops_info_idx_inc(int unit, bool *close_msg_after_this_op)
{
    if (WSTATE(n_wal_ops_info_idx) == WAL_OPS_INFO_IDX_MAX) {
        WSTATE(n_wal_ops_info_idx) = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        WSTATE(n_wal_ops_info_idx) += 1;
        /* *close_msg_after_this_op = FALSE;
         * Intentionally commented because wstate_undo_ops_info_idx_inc()
         * can also set it */
    }
    return SHR_E_NONE;
}

static int
wstate_undo_ops_info_idx_inc(int unit, bool *close_msg_after_this_op)
{
    if (WSTATE(n_undo_ops_info_idx) == WAL_UNDO_OPS_INFO_IDX_MAX) {
        WSTATE(n_undo_ops_info_idx) = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        WSTATE(n_undo_ops_info_idx) += 1;
        /* *close_msg_after_this_op = FALSE; Intentionally commented because
         * wstate_wal_ops_info_idx_inc() can also set it */
    }
    return SHR_E_NONE;
}

static int
wstate_rdr_msg_idx_inc(int unit)
{
    if (WSTATE(n_rdr_msg_idx) == WAL_MSG_IDX_MAX) {
        WSTATE(n_rdr_msg_idx) = 0;
    } else {
        WSTATE(n_rdr_msg_idx) += 1;
    }
    return SHR_E_NONE;
}

/* Will be called by SCF logic before ops for wal_msg are sent to HW.
 * Must be called only once per wal_msg.
 * Will execute in WAL reader context.
 *
 * For case when emc_group_id == smc_group_id, we will
 * take mutex once and inc count twice.
 */
static int
wal_rdr_pre_msg(int unit, bcmptm_wal_msg_t *walr_msg)
{
    int tmp_rv;
    bcmptm_wal_slice_mode_info_t smc_word;
    sal_mutex_t smc_mutex, emc_mutex;
    uint32_t smc_group_id = 0, emc_group_id = 0;
    SHR_FUNC_ENTER(unit);
    smc_word = walr_msg->smc_word;
    SHR_IF_ERR_EXIT((!smc_word.f.slice_change_en &&
                     !smc_word.f.entry_change_en) ? SHR_E_INTERNAL
                                                  : SHR_E_NONE);
    smc_group_id = smc_word.f.slice_group_id;
    if (smc_word.f.slice_change_en) {
        if (mc_count[unit][smc_group_id] == 0) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_slice_mode_mutex_info(unit, smc_group_id, &smc_mutex));
            tmp_rv = sal_mutex_take(smc_mutex, ENTRY_MODE_LOCK_WAIT_USEC);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! Could not take slice_mode_mutex \n")));
            }
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        mc_count[unit][smc_group_id] += 1;
    }
    if (smc_word.f.entry_change_en) {
        SHR_IF_ERR_EXIT(
            bcmptm_ser_entry_mode_mutex_info(unit, &emc_group_id, &emc_mutex));
        if (mc_count[unit][emc_group_id] == 0) {
            tmp_rv = sal_mutex_take(emc_mutex, ENTRY_MODE_LOCK_WAIT_USEC);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! Could not take entry_mode_mutex \n")));
            }
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        mc_count[unit][emc_group_id] += 1;
    }
exit:
    /* Only error could be in:
     * 1. retrieving group_id- in this case we don't take mutex and did not inc
     *    count
     * 2. taking mutex - in this case we did not inc count
     */
    SHR_FUNC_EXIT();
}

/* Will be called by WAL reader for every attempt of sending wal_msg.
 * Will execute in WAL reader context.
 *
 * For case when emc_group_id == smc_group_id, we will
 * give mutex once and dec count twice.
 */
static int
wal_rdr_post_msg(int unit, bcmptm_wal_msg_t *walr_msg,
                 uint32_t num_ops_sent, bool msg_done)
{
    int tmp_rv;
    bcmptm_wal_slice_mode_info_t smc_word;
    sal_mutex_t smc_mutex, emc_mutex;
    uint32_t smc_group_id = 0, emc_group_id = 0;
    SHR_FUNC_ENTER(unit);
    smc_word = walr_msg->smc_word;
    smc_group_id = smc_word.f.slice_group_id;
    /* ser slice_mode update happens only when all ops in msg are sent to HW */
    if (msg_done && smc_word.f.slice_change_en) {
        bcmptm_rm_slice_change_t rm_slice_info;
        rm_slice_info.slice_num0 =
            (smc_word.f.slice_num0 > SLICE_MAX ? -1 : smc_word.f.slice_num0);
        rm_slice_info.slice_num1 =
            (smc_word.f.slice_num1 > SLICE_MAX ? -1 : smc_word.f.slice_num1);
        rm_slice_info.slice_mode = smc_word.f.slice_mode;
        rm_slice_info.slice_group_id = smc_word.f.slice_group_id;
        SHR_IF_ERR_EXIT(bcmptm_ser_slice_mode_change(unit, &rm_slice_info));
        if (mc_count[unit][smc_group_id] == 1) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_slice_mode_mutex_info(unit, smc_group_id, &smc_mutex));
            tmp_rv = sal_mutex_give(smc_mutex);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! Could not give slice_mode_mutex \n")));
            }
            SHR_IF_ERR_EXIT(tmp_rv);
            mc_count[unit][smc_group_id] = 0;
        } else {
            mc_count[unit][smc_group_id] -= 1;
        }
    }
    /* Because ops are released for every partial msg attempt,
     * ser entry_mode updates must happen for every msg attempt.
     *
     * Release of emc_mutex and counts, however, must be done ONLY once when
     * msg is done.
     */
    if (smc_word.f.entry_change_en) {
        uint32_t i;
        bcmptm_rm_entry_mode_change_t emc_info;
        bcmptm_wal_ops_info_t *ops_info_array =
            WAL_OPS_INFO_BPTR + walr_msg->ops_info_idx;
        for (i = 0; i < num_ops_sent; i++) {
            if (ops_info_array->op_type == BCMPTM_RM_OP_EM_WIDE ||
                ops_info_array->op_type == BCMPTM_RM_OP_EM_NARROW) {
                emc_info.index = (ops_info_array->dyn_info).index;
                emc_info.op_type = ops_info_array->op_type;
                SHR_IF_ERR_EXIT(bcmptm_ser_entry_mode_change(unit, &emc_info));
            }
            ops_info_array++;
        }
        if (msg_done) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_entry_mode_mutex_info(unit, &emc_group_id, &emc_mutex));
            if (mc_count[unit][emc_group_id] == 1) {
                tmp_rv = sal_mutex_give(emc_mutex);
                if (tmp_rv != SHR_E_NONE) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "!! Could not give entry_mode_mutex \n")));
                }
                SHR_IF_ERR_EXIT(tmp_rv);
                mc_count[unit][emc_group_id] = 0;
            } else {
                mc_count[unit][emc_group_id] -= 1;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
undo_info_get(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *pt_dyn_info, void *pt_ovrr_info,
              uint32_t entry_count, uint32_t undo_entry_buf_wsize,

              /* Outputs */
              uint32_t *undo_entry_words_ptr,
              wal_undo_ops_t *undo_ops_info_ptr)
{
    SHR_FUNC_ENTER(unit);
    sal_memset(undo_ops_info_ptr, 0, sizeof(wal_undo_ops_t));
    undo_ops_info_ptr->undo_sid = sid;
    undo_ops_info_ptr->undo_dyn_info = *pt_dyn_info;
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_read(unit, sid, pt_dyn_info, pt_ovrr_info,
                            entry_count,
                            undo_entry_buf_wsize,

                            undo_entry_words_ptr,
                            &(undo_ops_info_ptr->undo_cache_vbit),
                            &(undo_ops_info_ptr->undo_dfid),
                            &(undo_ops_info_ptr->undo_ltid)));
exit:
    SHR_FUNC_EXIT();
}

/* Set dq_ready for all msgs of a transaction */
static int
wal_send_all_msgs(int unit, wal_trans_t *trans_ptr, bool resend)
{
    uint32_t i, c_msg_idx, c_trans_idx;
    bcmptm_wal_msg_t *c_wal_msg_ptr;
    bool *c_wal_msg_dq_ready_ptr;
    uint32_t c_num_msgs;
    SHR_FUNC_ENTER(unit);

    /* Lock is needed if we are using any var that can be changed by
     * wal_rdr_msg_done() */
    /* SHR_IF_ERR_EXIT(wal_wstate_lock(unit)); */

    c_msg_idx = trans_ptr->f_msg_idx;

    c_wal_msg_ptr = BCMPTM_WAL_MSG_BPTR + c_msg_idx;
    c_wal_msg_dq_ready_ptr = BCMPTM_WAL_MSG_DQ_READY_BPTR + c_msg_idx;

    c_trans_idx = c_wal_msg_ptr->trans_idx;
    c_num_msgs = trans_ptr->num_msgs;

    print_trans_info(unit, FALSE, c_trans_idx, trans_ptr, "WCM");

    /* From this point on, we must not reference contents of wal_msg_ptr,
     * trans_ptr except for debug. Because the moment we set dq_ready for first
     * msg in trans, wal_rdr can change wal_msg, trans */

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Will commit %0u msgs for trans_idx = %0u \n"),
                    c_num_msgs, c_trans_idx));

    /* Enable wal rdr to dequeue msgs */
    if (!wal_msg_dq_enable[unit]) {
        /* Verify WAL reader thread is in RUN state. */
        SHR_IF_ERR_EXIT(bcmptm_walr_run_check(unit));

        /* Initialize dq_idx for WAL reader. */
        bcmptm_wal_msg_dq_idx[unit] = c_msg_idx;
        bcmptm_wal_msg_dq_ptr[unit] = c_wal_msg_ptr;
        bcmptm_wal_msg_dq_ready_ptr[unit] = BCMPTM_WAL_MSG_DQ_READY_BPTR +
                                            c_msg_idx;
        /* Then enable dq */
        wal_msg_dq_enable[unit] = TRUE;
    }

    /* Following will do nothing if c_num_msgs = 0
     * This is legal case when user issues Blocking_read_from_HW and then
     * issues commit */
    for (i = 0; i < c_num_msgs; i++) {
        /* Must print msg before dq_ready is set - else context may switch to
         * wal_rdr and may result in release of this msg - so wal_msg members
         * will get zeroed out */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Cmt, umsg_idx, %0u, skip_words, %0u, "
                        "words_buf_idx, %0u, words_sent, %0u\n"),
             c_wal_msg_ptr->umsg_idx,
             c_wal_msg_ptr->num_words_skipped,
             c_wal_msg_ptr->words_buf_idx,
             c_wal_msg_ptr->req_entry_wsize));

        assert(*c_wal_msg_dq_ready_ptr == FALSE);
        if (resend) {
            assert(c_wal_msg_ptr->committed == TRUE);
        } else {
            assert(c_wal_msg_ptr->committed == FALSE);
            c_wal_msg_ptr->committed = TRUE;
        }
        *c_wal_msg_dq_ready_ptr = TRUE;
        SHR_IF_ERR_MSG_EXIT(
            bcmptm_walr_wake(unit),
            (BSL_META_U(unit, "Got error when waking up WAL reader thread.\n")));
        WSTATE(c_msg_commit_count) += 1;

        /* next msg */
        if (c_msg_idx == WAL_MSG_IDX_MAX) {
            c_wal_msg_ptr = BCMPTM_WAL_MSG_BPTR;
            c_wal_msg_dq_ready_ptr = BCMPTM_WAL_MSG_DQ_READY_BPTR;
            c_msg_idx = 0;
        } else {
            c_wal_msg_ptr++;
            c_wal_msg_dq_ready_ptr++;
            c_msg_idx++;
        }
    } /* for each msg */
    SHR_IF_ERR_EXIT((WSTATE(c_msg_commit_count) != c_num_msgs) ? SHR_E_INTERNAL
                                                               : SHR_E_NONE);
    /* SHR_IF_ERR_EXIT(wal_wstate_unlock(unit)); */
exit:
    SHR_FUNC_EXIT();
}

/* Look for in_cseq_id in pending WAL trans.
 * If found: Re-commit. Notification will come when ops are sent to HW.
 * Else:     send immediate notification if requested.
 */
static int
wal_trans_id_unknown(int unit,
                     uint32_t in_cseq_id,
                     bcmptm_trans_cb_f notify_fn_ptr,
                     void *notify_info)
{
    bcmptm_wal_msg_t *o_wal_msg_ptr;
    uint32_t o_trans_idx;
    wal_trans_t *o_trans_ptr;
    /* wstate_t *wstate_ptr; for gdb debug only */
    SHR_FUNC_ENTER(unit);
    /* wstate_ptr = WSTATE_BPTR; */

    SHR_IF_ERR_EXIT(wal_wstate_lock(unit)); /* freeze n_rdr_msg_idx */

    if (WSTATE(n_rdr_msg_idx) == WSTATE(n_msg_idx)) {
        /* WAL is empty */
        
        if (notify_fn_ptr) {
            notify_fn_ptr(in_cseq_id, SHR_E_NONE, notify_info);
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "====== Did not find matching trans_id\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE); /* Can send NOT_FOUND, if
                                              caller so desires */
    } /* WAL is empty */

    o_wal_msg_ptr = BCMPTM_WAL_MSG_BPTR + WSTATE(n_rdr_msg_idx);
    o_trans_idx = o_wal_msg_ptr->trans_idx;
    o_trans_ptr = WAL_TRANS_BPTR + o_trans_idx;

    do {
        /* No more trans in WAL */
        if (o_trans_idx == WSTATE(n_trans_idx)) {
            
            if (notify_fn_ptr) {
                notify_fn_ptr(in_cseq_id, SHR_E_NONE, notify_info);
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "====== Did not find matching trans_id\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_NONE); /* Can send NOT_FOUND, if
                                                  caller so desires */
        }

        /* Pending trans in WAL cannot be empty */
        SHR_IF_ERR_EXIT(!o_trans_ptr->num_msgs ? SHR_E_INTERNAL : SHR_E_NONE);

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "trans_idx=%0d, in_cseq_id=%0d, pending trans "
                        "cseq_id = %0d\n"),
             o_trans_idx, in_cseq_id, o_trans_ptr->cseq_id));

        /* Re-commit case */
        if (in_cseq_id == o_trans_ptr->cseq_id) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "====== Found matching trans_id\n")));

            /* re-record notify info */
            o_trans_ptr->notify_fn_ptr = notify_fn_ptr;
            o_trans_ptr->notify_info = notify_info;

            /* re-send all messages for old trans to wal rdr */
            SHR_IF_ERR_EXIT(wal_send_all_msgs(unit, o_trans_ptr,
                                              TRUE)); /* resend */
        } /* found an old trans */

        /* Try next trans */
        if (o_trans_idx == WAL_TRANS_IDX_MAX) {
            o_trans_idx = 0;
            o_trans_ptr = WAL_TRANS_BPTR;
        } else {
            o_trans_idx++;
            o_trans_ptr++;
        }
    } while (1);

exit:
    if (have_wstate_mutex[unit]) {
        (void) sal_mutex_give(wstate_mutex[unit]);
        have_wstate_mutex[unit] = FALSE;
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
uint32_t
bcmptm_wal_next_msg_idx(int unit, uint32_t c_msg_idx)
{
    if (c_msg_idx == WAL_MSG_IDX_MAX) {
        return 0;
    } else {
        return ++c_msg_idx;
    }
}

/* Will always be called at end of every msg sending attempt.
 *
 * Can also be called before sending of msg.
 *
 * Will execute in WAL rdr context !! */
int
bcmptm_wal_rdr_msg_done(int unit, bcmptm_wal_msg_t *walr_msg,
                        bcmptm_walr_status_t *scf_status)
{
    wal_trans_t *trans_ptr;
    uint32_t obs_wal_msg_idx;
    int tmp_rv = SHR_E_NONE;
    bool rdr_has_wstate_lock = FALSE;
    bcmptm_trans_cb_f trans_notify_fn;
    bool slam_msg = FALSE, release_trans = FALSE;
    uint32_t release_num_ops, release_words_buf_count;
    SHR_FUNC_ENTER(unit);

    trans_ptr = WAL_TRANS_BPTR + walr_msg->trans_idx;
    slam_msg = (walr_msg->cmd == BCMPTM_WAL_MSG_CMD_SLAM);

    if (walr_msg->need_cbf_before_msg && !slam_msg) {
        /* Means callback is BEFORE the msg.
         * These pre_msg callbacks need not happen in normal order - so we should
         * not be checking for expected msg_idx */
        SHR_IF_ERR_EXIT(
            wal_rdr_pre_msg(unit, walr_msg));
        /* cbf before msg must happen only ONCE */
        walr_msg->need_cbf_before_msg = 0;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    obs_wal_msg_idx = walr_msg - BCMPTM_WAL_MSG_BPTR;
    if (obs_wal_msg_idx != WSTATE(n_rdr_msg_idx)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "rdr_msg_done: exp_msg_idx = %0u, obs_wal_msg_idx"
                        " = %0u\n"),
             WSTATE(n_rdr_msg_idx), obs_wal_msg_idx));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT(trans_ptr->num_msgs < 1 ? SHR_E_INTERNAL
                                            : SHR_E_NONE);

    if (BCMPTM_CFG_WAL_ALIGN_EN && ((walr_msg->words_buf_idx) & 0x3)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "rdr_msg_done: align_ERROR, msg.words_buf_idx = "
                        "%0u\n"),
             walr_msg->words_buf_idx));
    }

    /* print_wstate_info(unit, FALSE, "RDR"); */
    print_msg_info(unit, FALSE, WSTATE(n_rdr_msg_idx), walr_msg, "RDR");
    if (SHR_FAILURE(scf_status->rv)) { /* SER event */
        
        uint32_t num_ops_sent, num_words_sent;

        SHR_IF_ERR_EXIT(
            sal_mutex_take(wstate_mutex[unit], WSTATE_LOCK_WAIT_USEC));
        rdr_has_wstate_lock = TRUE;

        LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(unit, "rdr_msg_done (umsg_idx %0u) saw SER failure \n"),
             walr_msg->umsg_idx));
        switch (walr_msg->cmd) {
            case BCMPTM_WAL_MSG_CMD_WRITE:
                if (bcmptm_wal_scf_num_chans[unit]) {
                    /* Retry all ops - no release of any resources */
                    SHR_RETURN_VAL_EXIT(SHR_E_PARTIAL);
                    break; /* from switch */
                }
                num_ops_sent = scf_status->num_good_ops;
                num_words_sent = scf_status->good_req_words_wsize;

                /* Following assumes, sending of ops pauses at the
                 * errored op. This is true only when PIO is being used.
                 *
                 * num_ops_sent could be 0 (very first op saw SER event),
                 * or non-zero */
                if (WSTATE(rdr_ignore_op_err)) {
                    /* Note: (ops_info_array + num_ops_sent)
                     *       provides failed ops_info */
                    num_words_sent += (WAL_OPS_INFO_BPTR +
                                       walr_msg->ops_info_idx +
                                       num_ops_sent)->op_wsize;
                    /* Now we can inc num_ops_sent */
                    num_ops_sent++;
                    if (num_ops_sent == walr_msg->num_ops) {
                        /* Above inc resulted in ALL ops sent for msg.
                         * We must treat it as if msg was completed
                         * successfully (and not exit the function) */
                        break; /* Do not return from function yet */
                    }
                }

                /* Following will be executed only if rdr_ignore_op_err is 0
                 * and will eventually signal the bd to retry sending of ops
                 * for this msg */

                /* Update WSTATE vars */
                /* WSTATE(avail_trans_count); */
                /* WSTATE(avail_msg_count); */
                WSTATE(avail_wal_ops_info_count) += num_ops_sent;
                WSTATE(avail_words_buf_count) +=
                    (num_words_sent + walr_msg->num_words_skipped);
                SHR_IF_ERR_EXIT(WSTATE(rdr_msg_count) == trans_ptr->num_msgs ?
                                SHR_E_INTERNAL : SHR_E_NONE);
                /* WSTATE(rdr_msg_count); */
                /* SHR_IF_ERR_EXIT(wstate_rdr_msg_idx_inc(unit)); */
                /* Note:
                 * - From above code, it appears that skipped_words will be
                 *   released every time wal_rdr_msg_done() is called for
                 *   same msg. BUT, this is not the case. Skipped words vars
                 *   are reset to 0 (below) - which means - skipped_words will
                 *   be released ONLY the first time when wal_rdr_msg_done() is
                 *   invoked for a msg even if no (0) ops were sent successfully
                 *   for this msg.
                 */

                /* Update trans vars (needed for error checking) */
                trans_ptr->num_ops -= num_ops_sent;
                trans_ptr->num_words -=
                    (num_words_sent + walr_msg->num_words_skipped);
                /* Must not send notification - not yet done with this msg */

                /* Update entry_mode (BUT NOT slice_mode).
                 * Must be done before update of msg vars, release of ops */
                SHR_IF_ERR_EXIT(
                    wal_rdr_post_msg(unit, walr_msg, num_ops_sent, FALSE));

                /* Update wal_msg vars */

                /* walr_msg->trans_idx; */
                walr_msg->num_words_skipped = 0; /* must */
                /* walr_msg->umsg_idx; */
                

                /* Update wal_msg vars */
                /* walr_msg->cmd; */
                walr_msg->words_buf_idx += num_words_sent;
                    /* Must NOT add num_words_skipped !! */
                walr_msg->req_entry_wsize -= num_words_sent;
                walr_msg->num_ops -= num_ops_sent;
                walr_msg->ops_info_idx += num_ops_sent;

                /* wal rdr will retry sending remaining ops of the msg. */
                SHR_RETURN_VAL_EXIT(SHR_E_PARTIAL);
                /* Important to understand:
                 * Case when ignore_op_err = 0,
                 *      we will retry failed WRITE, READ - and all other ops of
                 *      this msg.
                 * Case when ignore_op_err = 1,
                 *      we will skip failed WRITE and retry only remaining ops
                 *      of this msg.
                 *      we will skip failed READ.
                 */
                break;

            case BCMPTM_WAL_MSG_CMD_SLAM:
                /* wal rdr will retry SLAM */
                SHR_RETURN_VAL_EXIT(SHR_E_PARTIAL);
                break;

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "rdr_msg_done cannot support SER"
                                "event for unknown type msg_cmd = %d !! \n"),
                     walr_msg->cmd));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                break;
        } /* msg_cmd */
        /* Means we ignored err on last op of msg */
    } /* SER event */

    { /* successful completion */
        /* If we are here as continuation for failure condition, we already have
         * wstate_lock - ok to be less efficient when we have errors.
         *
         * If we are here for success case, we don't have lock. This is the
         * mainline hi-performance path. Must take lock only when we release
         * resources (increment WSTATE(avail_counts)).
         */
        switch (walr_msg->cmd) {
            case BCMPTM_WAL_MSG_CMD_WRITE:
            case BCMPTM_WAL_MSG_CMD_SLAM:
                /* Update slice_mode, entry_mode.
                 * Must be done before update of msg vars, release of ops */
                if (!slam_msg) {
                    SHR_IF_ERR_EXIT(
                        wal_rdr_post_msg(unit, walr_msg, walr_msg->num_ops,
                                         TRUE));
                }

                /* Optional - needed for error checking. */
                /* Don't modify trans_ptr->f_msg_idx, num_msg. */
                trans_ptr->num_ops -= slam_msg ? 1 : walr_msg->num_ops;
                trans_ptr->num_words -=
                    (walr_msg->req_entry_wsize +
                     walr_msg->num_words_skipped);
                if (slam_msg) {
                    trans_ptr->slam_msg_count -= 1;
                }

                WSTATE(rdr_msg_count) += 1;
                if (WSTATE(rdr_msg_count) == trans_ptr->num_msgs) {
                    print_trans_info(unit, FALSE, walr_msg->trans_idx,
                                     trans_ptr, "RDR");
                    release_trans = TRUE;
                    WSTATE(rdr_msg_count) = 0; /* cleanup */
                    if (trans_ptr->num_ops || trans_ptr->num_words ||
                        trans_ptr->slam_msg_count) {
                        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Trans has non-zero rsrc_counts "
                                        "even when last msg is done !! \n")));
                        print_trans_info(unit, TRUE, walr_msg->trans_idx,
                                         trans_ptr, "RDR");
                        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
                    }

                    /* Call notification function */
                    if (trans_ptr->notify_fn_ptr) {
                        trans_notify_fn = trans_ptr->notify_fn_ptr;
                        trans_notify_fn(trans_ptr->cseq_id, SHR_E_NONE,
                                        trans_ptr->notify_info);
                    }

                    
                    sal_memset(trans_ptr, 0, sizeof(wal_trans_t));
                } /* all msgs done */

                /* Cleanup wal_msg */
                release_num_ops = slam_msg ? 1 : walr_msg->num_ops;
                release_words_buf_count =
                    (walr_msg->req_entry_wsize +
                     walr_msg->num_words_skipped);
                sal_memset(walr_msg, 0, sizeof(bcmptm_wal_msg_t));

                SHR_IF_ERR_EXIT(wstate_rdr_msg_idx_inc(unit));

                /* Take wstate_lock before releasing resources. */
                if (!rdr_has_wstate_lock) {
                    SHR_IF_ERR_EXIT(
                        sal_mutex_take(wstate_mutex[unit], WSTATE_LOCK_WAIT_USEC));
                    rdr_has_wstate_lock = TRUE;
                }
                WSTATE(avail_wal_ops_info_count) += release_num_ops;
                WSTATE(avail_words_buf_count) += release_words_buf_count;
                if (release_trans) {
                    WSTATE(avail_trans_count) += 1;
                }
                WSTATE(avail_msg_count) += 1;
                break; /* CMD_WRITE, SLAM */

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "rdr_msg_done cannot support unknown "
                                "type msg_cmd = %d !! \n"), walr_msg->cmd));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                break;
        } /* msg_cmd */
    } /* successful completion */

    /* Update WAL read pointers */

    /* Release WAL resources */

    /* Notification */

exit:
    if (rdr_has_wstate_lock) {
        tmp_rv = sal_mutex_give(wstate_mutex[unit]);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "rdr_msg_done could not release wstate_mutex "
                            "!!\n")));
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_cfg_set(int unit)
{
    switch (cfg_wal_mode[unit]) {
    case -1:
        /* Auto-commit settings that were used till 3/28/2017
         * Can be useful to recreate the problem that IDC reported.
         * One op per msg.
         * One msg per trans
         */
        cfg_wal_trans_max_count[unit] = 10;
        cfg_wal_msg_max_count[unit] = 100;
        cfg_wal_words_buf_max_count[unit] = (100*40);
        cfg_wal_ops_info_max_count[unit] = 100;
        cfg_undo_ops_info_max_count[unit] = 100;
        cfg_undo_words_buf_max_count[unit] = 100*40;
        cfg_wal_max_words_in_msg[unit] = 80;
        cfg_wal_max_ops_in_msg[unit] = 1;
        cfg_wal_max_cwords_wsize[unit] = 4;
        cfg_wal_max_msgs_in_trans[unit] = 1;
        break;
    case -2:
        /* Auto-commit settings that give more WAL coverage:
         * 5 ops per msg
         * 3 msgs per trans
         * Almost ALL utptm tests use this mode.
         */
        cfg_wal_trans_max_count[unit] = BCMPTM_CFG_WAL_TRANS_MAX_COUNT;
        cfg_wal_msg_max_count[unit] = BCMPTM_CFG_WAL_MSG_MAX_COUNT;
        cfg_wal_words_buf_max_count[unit] = BCMPTM_CFG_WAL_WORDS_BUF_MAX_COUNT;
        cfg_wal_ops_info_max_count[unit] = BCMPTM_CFG_WAL_OPS_INFO_MAX_COUNT;
        cfg_undo_ops_info_max_count[unit] = BCMPTM_CFG_WAL_UNDO_OPS_INFO_MAX_COUNT;
        cfg_undo_words_buf_max_count[unit] = BCMPTM_CFG_WAL_UNDO_WORDS_BUF_MAX_COUNT;
        cfg_wal_max_words_in_msg[unit] = 100;
        cfg_wal_max_ops_in_msg[unit] = 5;
        cfg_wal_max_cwords_wsize[unit] = 2;
        cfg_wal_max_msgs_in_trans[unit] = 3;
        break;
    case 1:
        /* NOT auto-commit mode
         * Just for utptm tests - to create lot of messages in trans
         * Only utptm_test_trans.c uses this mode */
        cfg_wal_trans_max_count[unit] = BCMPTM_CFG_WAL_TRANS_MAX_COUNT;
        cfg_wal_msg_max_count[unit] = BCMPTM_CFG_WAL_MSG_MAX_COUNT;
        cfg_wal_words_buf_max_count[unit] = BCMPTM_CFG_WAL_WORDS_BUF_MAX_COUNT;
        cfg_wal_ops_info_max_count[unit] = BCMPTM_CFG_WAL_OPS_INFO_MAX_COUNT;
        cfg_undo_ops_info_max_count[unit] = BCMPTM_CFG_WAL_UNDO_OPS_INFO_MAX_COUNT;
        cfg_undo_words_buf_max_count[unit] = BCMPTM_CFG_WAL_UNDO_WORDS_BUF_MAX_COUNT;
        cfg_wal_max_words_in_msg[unit] = BCMPTM_CFG_WAL_MAX_WORDS_IN_MSG;
        cfg_wal_max_ops_in_msg[unit] = 2;
        cfg_wal_max_cwords_wsize[unit] = BCMPTM_CFG_WAL_MAX_CWORDS_WSIZE;
        cfg_wal_max_msgs_in_trans[unit] = BCMPTM_CFG_WAL_MAX_MSGS_IN_TRANS;
        break;
    default:
        /* Real Commit mode
         * Used by all SQA tests */
        cfg_wal_trans_max_count[unit] = BCMPTM_CFG_WAL_TRANS_MAX_COUNT;
        cfg_wal_msg_max_count[unit] = BCMPTM_CFG_WAL_MSG_MAX_COUNT;
        cfg_wal_words_buf_max_count[unit] = BCMPTM_CFG_WAL_WORDS_BUF_MAX_COUNT;
        cfg_wal_ops_info_max_count[unit] = BCMPTM_CFG_WAL_OPS_INFO_MAX_COUNT;
        cfg_undo_ops_info_max_count[unit] = BCMPTM_CFG_WAL_UNDO_OPS_INFO_MAX_COUNT;
        cfg_undo_words_buf_max_count[unit] = BCMPTM_CFG_WAL_UNDO_WORDS_BUF_MAX_COUNT;
        cfg_wal_max_words_in_msg[unit] = BCMPTM_CFG_WAL_MAX_WORDS_IN_MSG;
        cfg_wal_max_ops_in_msg[unit] = BCMPTM_CFG_WAL_MAX_OPS_IN_MSG;
        cfg_wal_max_cwords_wsize[unit] = BCMPTM_CFG_WAL_MAX_CWORDS_WSIZE;
        cfg_wal_max_msgs_in_trans[unit] = BCMPTM_CFG_WAL_MAX_MSGS_IN_TRANS;

        /* If using real schanfifo */
        if (bcmptm_wal_scf_num_chans[unit]) {
            uint8_t lsb, inc_val;
            /* Make words_buf_max_count multiple of 4 when using schanfifo */
            lsb = cfg_wal_words_buf_max_count[unit] & 0x3;
            inc_val = 4 - lsb;
            cfg_wal_words_buf_max_count[unit] += inc_val;

            /* use HW defined cmd_mem_wsize */
            cfg_wal_max_words_in_msg[unit] = wal_scf_cmd_mem_wsize[unit];
        }
        break;
    } /* cfg_wal_mode */

    return SHR_E_NONE;
}

int
bcmptm_wal_cseq_id_check(int unit, uint32_t in_cseq_id)
{
    SHR_FUNC_ENTER(unit);
    if (bcmptm_wal_autocommit_enabled(unit)) {
        
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    } else if (WSTATE(mreq_state_in_trans)) { /* normal mode */
        SHR_IF_ERR_MSG_EXIT(
            ((in_cseq_id != WSTATE(mreq_cseq_id)) ? SHR_E_INTERNAL : SHR_E_NONE),
            (BSL_META_U(unit, "Received cseq_id = %0u, expected cseq_id = %0u\n"),
             in_cseq_id, WSTATE(mreq_cseq_id)));
    } /* normal mode */
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_trans_cmd(int unit, uint32_t flags,
                     bcmptm_trans_cmd_t trans_cmd,
                     uint32_t in_cseq_id, bcmptm_trans_cb_f notify_fn_ptr,
                     void* notify_info)
{
    uint32_t rsp_flags; 
    SHR_FUNC_ENTER(unit);

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            SHR_IF_ERR_EXIT(bcmptm_wal_cseq_id_check(unit, in_cseq_id));

            SHR_IF_ERR_EXIT(
                bcmptm_wal_commit(unit, flags, WSTATE(mreq_state_in_trans),
                                  in_cseq_id, notify_fn_ptr,
                                  notify_info, &rsp_flags));
            WSTATE(mreq_state_in_trans) = FALSE;
            break;
        case BCMPTM_TRANS_CMD_ABORT:
            SHR_IF_ERR_EXIT(bcmptm_wal_cseq_id_check(unit, in_cseq_id));

            if (!WSTATE(mreq_state_in_trans)) {
                /* LTM manages index tables and has the 'valid' state info for every
                 * 'key'(index) of such table. Lookup for such table may not create
                 * any PTM request - so PTM may be in IDLE.
                 *
                 * Thus, if lookup was targeted for indexed table to LTM and it
                 * fails, TRM will still issue Abort in this case - which LTM will
                 * forward to PTM - even when PTM and WAL were in IDLE.
                 * So getting Abort when mreq, PTRM, WAL are in IDLE is not an
                 * error case.
                 */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! NOP: received with ABORT even when there "
                                "were no prior ops \n")));
            }

            SHR_IF_ERR_EXIT(
                bcmptm_wal_abort(unit, flags, in_cseq_id, &rsp_flags));
                /* Note: wal_abort() will also Undo changes to PTcache */

            WSTATE(mreq_state_in_trans) = FALSE;

            break;
        case BCMPTM_TRANS_CMD_CSEQ_ID_STORE:
            WSTATE(mreq_cseq_id) = in_cseq_id;
            break;
        default: /* BCMPTM_TRANS_CMD_IDLE_CHECK */
            if (bcmptm_wal_autocommit_enabled(unit)) {
                /* We already ensured that LTM (and hence TRM) has stopped,
                 * so we know LTM is not issuing more requests */

                WSTATE(mreq_state_in_trans) = FALSE;
                WSTATE(mreq_cseq_id) = 0;
            } else if (WSTATE(mreq_state_in_trans)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "!! ERROR: mreq_state NOT IDLE when "
                                    "stop was requested \n")));
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
    } /* trans_cmd */

exit:
    SHR_FUNC_EXIT();
}

/* Following will be called for every non-NOP request from LTM
 * It must be called even for READ, LOOKUP, GET_FIRST, GET_NEXT, etc ops
 * It changes mreq_state_in_trans to TRUE.
 */
int
bcmptm_wal_mreq_state_update(int unit, uint32_t in_cseq_id, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);
    if (WSTATE(mreq_state_in_trans)) { /* check cseq_id */
        SHR_IF_ERR_EXIT(
            bcmptm_wal_cseq_id_check(unit, in_cseq_id));
    } else { /* record cseq_id for new trans */
        WSTATE(mreq_cseq_id) = in_cseq_id;
        WSTATE(mreq_state_in_trans) = TRUE;
    }
exit:
    SHR_FUNC_EXIT();
}

/* Will execute in SERC context */
int
bcmptm_wal_ignore_op_err(int unit, bool enable)
{
    int tmp_rv;
    bool got_wstate_lock = FALSE;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(
        sal_mutex_take(wstate_mutex[unit], WSTATE_LOCK_WAIT_USEC));
    got_wstate_lock = TRUE;
    WSTATE(rdr_ignore_op_err) = enable;
exit:
    if (got_wstate_lock) {
        tmp_rv = sal_mutex_give(wstate_mutex[unit]);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Ignore_op_err could not take "
                                  "wstate_mutex !!\n")));
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Ignore_op_err could not take wstate_mutex !!\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_write(int unit,
                 uint32_t flags,
                 uint32_t in_cseq_id, 
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 void *pt_ovrr_info,
                 bcmptm_misc_info_t *misc_info,
                 uint32_t *entry_words,
                 bcmptm_rm_op_t op_type,
                 bcmptm_rm_slice_change_t *slice_mode_change_info,

                 uint32_t *rsp_flags)
{
    uint32_t w_trans_idx, w_msg_idx, w_wal_ops_info_idx, w_words_buf_idx,
             w_undo_ops_info_idx, w_undo_words_buf_idx;
    uint32_t pt_entry_wsize, max_pt_entry_wsize;
    uint32_t undo_entry_wsize = 0;
    uint32_t *w_words_ptr, w_num_words;
    uint32_t entry_count = 1, num_cwords = 0;
    bool done = FALSE, close_msg_before_this_op = FALSE,
         close_msg_after_this_op = FALSE, slam_en;
    uint32_t num_words_skipped = 0, retry_count = 0,
             retry_count_max;
    uint32_t req_trans_count = 0, req_msg_count = 0,
             req_undo_ops_info_count = 0;
    uint32_t tmp_words_buf_idx;
    wal_trans_t *w_trans_ptr;
    bcmptm_wal_msg_t *w_msg_ptr;
    bcmptm_wal_ops_info_t *w_wal_ops_info_ptr;
    wal_undo_ops_t *w_undo_ops_info_ptr;
    bcmptm_wal_slice_mode_info_t smc_word;
    /* wstate_t *wstate_ptr; for gdb debug only */
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    retry_count_max = WAL_AVAIL_SPACE_RETRY_COUNT_MAX;
    slam_en = (misc_info && misc_info->dma_enable);
    if (slam_en) {
        /* Error checks */
        SHR_IF_ERR_EXIT(slice_mode_change_info ? SHR_E_INTERNAL : SHR_E_NONE);
        SHR_IF_ERR_EXIT(op_type ? SHR_E_INTERNAL : SHR_E_NONE);
        SHR_IF_ERR_MSG_EXIT(
            !dma_avail[unit] ? SHR_E_UNAVAIL : SHR_E_NONE,
            (BSL_META_U(unit, "DMA support is not available.\n")));

        /* Main */
        entry_count = misc_info->dma_entry_count;
        retry_count_max = WAL_AVAIL_SPACE_FOR_SLAM_RETRY_COUNT_MAX;
    }

    /* wstate_ptr = WSTATE_BPTR; printf("wstate_ptr=%p\n", wstate_ptr); */
    print_wstate_info(unit, FALSE, "WR ");

    /* dwords needed for this entry */
    pt_entry_wsize = entry_count * bcmdrd_pt_entry_wsize(unit, sid);
                                /* without cwords */
    assert(pt_entry_wsize != 0);
    if (slam_en) {
        max_pt_entry_wsize = pt_entry_wsize;
    } else {
        max_pt_entry_wsize = pt_entry_wsize + cfg_wal_max_cwords_wsize[unit];
    }

    if (!wal_disable_undo[unit]) {
        undo_entry_wsize = pt_entry_wsize; /* without cwords */
        req_undo_ops_info_count = 1; /* we need this info for all wal_writes to
                                        support abort.
                                        Is 1 even for slam type msg. */
    }

    /* - Adjust avail words_count, undo_words_count to avoid wraparound
     *   over 'max_idx'. Adjust WSTATE avail counts, and wrap idx as
     *   necessary.
     *
     * - Amount of skipped info is important for releasing resources.
     */
    tmp_words_buf_idx = WSTATE(n_words_buf_idx);
        /* RHS may get modified by avail_words_skip().
         * LHS needed for abort, and also for words_skip_undo */
    SHR_IF_ERR_EXIT(
        avail_words_skip(unit,
                         max_pt_entry_wsize, /* cwords + dwords */
                         &close_msg_before_this_op,
                         &num_words_skipped));
    /* lock was taken by avail_words_skip */
    assert(have_wstate_mutex[unit]);

    /* Do we need new trans and/or new msg? */
    switch (WSTATE(c_trans_state)) {
        case TRANS_STATE_IN_MSG:
            req_trans_count = 0;
            if ((op_type == BCMPTM_RM_OP_SLICE_MODE_CHANGE) || slam_en) {
                /* Only one slice_mode_change per wal_msg.
                 * slice_mode_change is 1st op in msg.
                 * Use dedicated msg for SLAM. */
                close_msg_before_this_op = TRUE;
            }
            if (close_msg_before_this_op) {
                print_msg_info(unit, FALSE, WSTATE(c_msg_idx),
                               BCMPTM_WAL_MSG_BPTR + WSTATE(c_msg_idx), "WR");
                req_msg_count = 1;
            } else { /* check if adding this op will cause schan_fifo viol */
                uint32_t tmp_num_words, tmp_num_ops;
                bool words_max, ops_max;
                w_msg_ptr = BCMPTM_WAL_MSG_BPTR + (WSTATE(c_msg_idx));
                tmp_num_words = w_msg_ptr->req_entry_wsize;
                tmp_num_ops = w_msg_ptr->num_ops;
                words_max = (tmp_num_words + max_pt_entry_wsize) >
                              cfg_wal_max_words_in_msg[unit];
                ops_max = ((tmp_num_ops + 1) > cfg_wal_max_ops_in_msg[unit]);
                close_msg_before_this_op = words_max || ops_max;
                if (close_msg_before_this_op) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Coverage: close_msg due to "
                                    "msg_words_max = %s, msg_ops_max = %s \n"),
                                    words_max? "TRUE" : "FALSE",
                                    ops_max? "TRUE" : "FALSE"));
                    print_msg_info(unit, FALSE, WSTATE(c_msg_idx),
                                   BCMPTM_WAL_MSG_BPTR + WSTATE(c_msg_idx), "WR");
                    req_msg_count = 1;
                }
            }
            /* It may appear that we are doing avail_skip_words() twice:
             * - once before entering switch(c_trans_state) and
             * - also as part of align_skip() below.
             * But, if avail_skip_words() before entering switch() did any skip,
             * then align_skip() below would do nothing.
             */
            if (close_msg_before_this_op && BCMPTM_CFG_WAL_ALIGN_EN) {
                SHR_IF_ERR_EXIT(align_skip(unit,
                                           max_pt_entry_wsize,
                                           &num_words_skipped));
            } /* else {
                 Allow next op in same msg to be consecutive in words_buf
            } */
            break;
        case TRANS_STATE_BLOCKING_READ: /* like TRANS_STATE_NEED_NEW_MSG */
            /* Don't allow any write ops after BLOCKING_HW_READ */

            /* Revert skipped words - we already have wstate_lock.
             * We can optimize by not doing avail_words_skip() if we know state
             * is BLOCKING_READ. But it is ok - because we are in BLOCKING_READ
             * state only for debug - so this is rare condition. */
            WSTATE(n_words_buf_idx) = tmp_words_buf_idx;
            WSTATE(avail_words_buf_count) += num_words_skipped;
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "WRITE not allowed after BLOCKING_HW_READ"
                            " in same Atomic Transaction \n")));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            break;
        case TRANS_STATE_NEED_NEW_MSG:
            req_trans_count = 0;
            req_msg_count = 1; /* even if close_msg_before is 0 */
            if (BCMPTM_CFG_WAL_ALIGN_EN) {
                /* Following will do nothing if close_msg_before is set */
                SHR_IF_ERR_EXIT(align_skip(unit, max_pt_entry_wsize,
                                           &num_words_skipped));
            }
            /* ASSUME:
             * single op can never violate BCMPTM_WAL_OPS_IN MSG_MAX,
             * cfg_wal_max_words_in_msg limits */
            break;
        default: /* TRANS_STATE_IDLE */
            req_trans_count = 1;
            req_msg_count = 1; /* even if close_msg_before is 0 */
            if (BCMPTM_CFG_WAL_ALIGN_EN) {
                /* Following will do nothing if close_msg_before is set */
                SHR_IF_ERR_EXIT(align_skip(unit, max_pt_entry_wsize,
                                           &num_words_skipped));
            }
            /* ASSUME:
             * single op can never violate BCMPTM_WAL_OPS_IN MSG_MAX,
             * cfg_wal_max_words_in_msg limits */
            break;
    } /* trans_state */

    done = FALSE;
    retry_count = 0;
    do {
        /* We could be here with have_wstate_mutex = TRUE if we just
         * entered do_while loop.
         *
         * We could be here with have_wstate_mutex = FALSE if we did not
         * have resource(s) and are in 2nd, 3rd, etc iterations of do_while
         * loop.
         *
         * And both are ok and hence we should not call wal_wstate_lock()
         * if have_wstate_mutex is already TRUE
         */
        if (!have_wstate_mutex[unit]) {
            SHR_IF_ERR_EXIT(wal_wstate_lock(unit));
        }

        if ((WSTATE(avail_trans_count) >= req_trans_count) &&
            (WSTATE(avail_msg_count) >= req_msg_count) &&
            (WSTATE(avail_wal_ops_info_count) >= 1) &&
            (WSTATE(avail_words_buf_count) >= max_pt_entry_wsize) &&
            (WSTATE(avail_undo_ops_info_count) >= req_undo_ops_info_count) &&
            (WSTATE(avail_undo_words_buf_count) >= undo_entry_wsize)) {

            /* Record undo values of idx - new trans only */
            if (req_trans_count) {
                WSTATE(a_msg_idx) = WSTATE(n_msg_idx);
                WSTATE(a_wal_ops_info_idx) = WSTATE(n_wal_ops_info_idx);
                WSTATE(a_words_buf_idx) = tmp_words_buf_idx;
                WSTATE(a_umsg_idx) = WSTATE(n_umsg_idx);
            }

            /* WAL_WORDS_BUF - determine num_cwords & write cwords */
            w_words_buf_idx = WSTATE(n_words_buf_idx);
            w_words_ptr = WAL_WORDS_BUF_BPTR + w_words_buf_idx;
            if (bcmptm_wal_scf_num_chans[unit] && !slam_en) {
                SHR_IF_ERR_EXIT(
                    bcmbd_pt_cmd_hdr_get(unit, sid, pt_dyn_info,
                                         pt_ovrr_info,
                                         BCMBD_PT_CMD_WRITE,
                                         
                                         cfg_wal_max_cwords_wsize[unit],
                                         w_words_ptr,
                                         &num_cwords));
                assert(num_cwords != 0);
                w_words_ptr += num_cwords;
            }

            /* Decrement the counts and give up lock
             * - We don't need to block WAL reader from releasing space while
             *   writing msg.
             * - Observe: we are now using actual pt_entry_wsize and
             *            undo_entry_wsize
             */
            w_num_words = (num_cwords + pt_entry_wsize);
            WSTATE(avail_trans_count) -= req_trans_count;
            WSTATE(avail_msg_count) -= req_msg_count;
            WSTATE(avail_wal_ops_info_count) -= 1;
            WSTATE(avail_words_buf_count) -= w_num_words;
            WSTATE(avail_undo_ops_info_count) -= req_undo_ops_info_count;
            WSTATE(avail_undo_words_buf_count) -= undo_entry_wsize;
            SHR_IF_ERR_EXIT(wal_wstate_unlock(unit));
            /* Now we are out of critical code section */


            /* WAL_WORDS_BUF - write dwords */
            if (bcmdrd_pt_attr_is_cam(unit, sid)) {
                SHR_IF_ERR_EXIT( /* KM -> TCAM format conversion */
                    bcmptm_pt_tcam_km_to_xy(unit, sid,
                                            entry_count,   /* 1 for !SLAM */
                                            entry_words,   /* km_entry */
                                            w_words_ptr)); /* xy_entry */
            } else {
                sal_memcpy(w_words_ptr, entry_words, 4 * pt_entry_wsize);
            }

            /* UNDO_OPS_INFO, UNDO_WORDS_BUF */
            w_undo_ops_info_idx = WSTATE(n_undo_ops_info_idx);
            w_undo_ops_info_ptr = UNDO_OPS_INFO_BPTR + w_undo_ops_info_idx;
            w_undo_words_buf_idx = WSTATE(n_undo_words_buf_idx);
            if (req_undo_ops_info_count) {
                SHR_IF_ERR_EXIT(
                    undo_info_get(unit, sid, pt_dyn_info, pt_ovrr_info,
                                  entry_count, undo_entry_wsize,

                                  UNDO_WORDS_BUF_BPTR + w_undo_words_buf_idx,
                                  w_undo_ops_info_ptr));
            }

            /* WAL_OPS_INFO */
            w_wal_ops_info_idx = WSTATE(n_wal_ops_info_idx);
            w_wal_ops_info_ptr = WAL_OPS_INFO_BPTR + w_wal_ops_info_idx;
            w_wal_ops_info_ptr->op_wsize = w_num_words;
            w_wal_ops_info_ptr->op_ctrl_wsize = num_cwords;
            w_wal_ops_info_ptr->sid = sid;
            w_wal_ops_info_ptr->dyn_info = *pt_dyn_info;
            w_wal_ops_info_ptr->op_type = op_type;

            if (req_trans_count) {
                w_trans_idx = WSTATE(n_trans_idx);
                w_msg_idx = WSTATE(n_msg_idx);
            } else if (req_msg_count) {
                w_trans_idx = WSTATE(c_trans_idx);
                w_msg_idx = WSTATE(n_msg_idx);
            } else {
                w_trans_idx = WSTATE(c_trans_idx);
                w_msg_idx = WSTATE(c_msg_idx);
            }
            w_msg_ptr = BCMPTM_WAL_MSG_BPTR + w_msg_idx;
            if (req_msg_count) { /* new msg */
                /* WAL_MSG: init */
                w_msg_ptr->cmd = slam_en ? BCMPTM_WAL_MSG_CMD_SLAM
                                         : BCMPTM_WAL_MSG_CMD_WRITE;
                w_msg_ptr->words_buf_idx = w_words_buf_idx;
                w_msg_ptr->req_entry_wsize = w_num_words;
                w_msg_ptr->num_ops = slam_en ? entry_count : 1; /* Note */
                w_msg_ptr->ops_info_idx = w_wal_ops_info_idx;

                smc_word.entry = 0;
                switch (op_type) {
                    case BCMPTM_RM_OP_EM_NARROW:
                    case BCMPTM_RM_OP_EM_WIDE:
                        w_msg_ptr->need_cbf_before_msg = 1;

                        smc_word.f.entry_change_en = 1;
                        break;
                    case BCMPTM_RM_OP_SLICE_MODE_CHANGE:
                        w_msg_ptr->need_cbf_before_msg = 1;

                        SHR_NULL_CHECK(slice_mode_change_info, SHR_E_PARAM);

                        assert(slice_mode_change_info->slice_num0 <= SLICE_MAX);
                        assert(slice_mode_change_info->slice_num1 <= SLICE_MAX);
                        assert(slice_mode_change_info->slice_mode < 8);
                        assert(slice_mode_change_info->slice_group_id < 8);

                        smc_word.f.slice_num0 = slice_mode_change_info->slice_num0;
                        smc_word.f.slice_num1 = slice_mode_change_info->slice_num1;
                        smc_word.f.slice_mode = slice_mode_change_info->slice_mode;
                        smc_word.f.slice_change_en = 1;
                        smc_word.f.slice_group_id = slice_mode_change_info->slice_group_id;
                        break;
                    default:
                        w_msg_ptr->need_cbf_before_msg = 0;
                        break;
                }
                w_msg_ptr->smc_word = smc_word;

                w_msg_ptr->undo_ops_info_idx = w_undo_ops_info_idx;
                w_msg_ptr->undo_words_buf_idx = w_undo_words_buf_idx;
                w_msg_ptr->undo_num_words = undo_entry_wsize;
                w_msg_ptr->trans_idx = w_trans_idx;
                w_msg_ptr->num_words_skipped = num_words_skipped;
                w_msg_ptr->umsg_idx = WSTATE(n_umsg_idx);
            } else { /* we are in same msg (cannot be slam) */
                /* WAL_MSG: update */
                w_msg_ptr->req_entry_wsize += w_num_words;
                w_msg_ptr->num_ops += 1;

                if (op_type == BCMPTM_RM_OP_EM_WIDE ||
                    op_type == BCMPTM_RM_OP_EM_NARROW) {
                    w_msg_ptr->need_cbf_before_msg = 1;

                    smc_word = w_msg_ptr->smc_word; /* could be non-zero */
                    smc_word.f.entry_change_en = 1;
                    w_msg_ptr->smc_word = smc_word;
                }
                w_msg_ptr->undo_num_words += undo_entry_wsize;
                w_msg_ptr->num_words_skipped += num_words_skipped;
            } /* we are in same msg */

            /* WAL_TRANS */
            w_trans_ptr = WAL_TRANS_BPTR + w_trans_idx;
            if (req_trans_count) { /* New Trans */
                w_trans_ptr->f_msg_idx = w_msg_idx; /* 1st msg of trans */
                w_trans_ptr->cseq_id = WSTATE(mreq_cseq_id); /* only for 1st op of trans */

                w_trans_ptr->num_msgs = 1;
                w_trans_ptr->num_ops = 1; /* even for slam msg */
                w_trans_ptr->num_words = (w_num_words + num_words_skipped);
                w_trans_ptr->slam_msg_count = slam_en ? 1 : 0;
            } else {
                w_trans_ptr->num_msgs += req_msg_count;
                w_trans_ptr->num_ops += 1; /* even for slam msg */
                w_trans_ptr->num_words += (w_num_words + num_words_skipped);
                if (slam_en) {
                    w_trans_ptr->slam_msg_count += 1;
                }
            }

            /* Update WSTATE(c_vars) and WSTATE(n_vars) */
            if (req_trans_count) {
                WSTATE(c_trans_idx) = w_trans_idx;
                SHR_IF_ERR_EXIT(wstate_trans_idx_inc(unit));
                WSTATE(c_msg_commit_count) = 0;
            }
            if (req_msg_count) {
                WSTATE(c_msg_idx) = w_msg_idx;
                SHR_IF_ERR_EXIT(wstate_msg_idx_inc(unit));
                WSTATE(n_umsg_idx) += 1; /* ok to wrap */
            }
            SHR_IF_ERR_EXIT(
                wstate_wal_ops_info_idx_inc(unit, &close_msg_after_this_op));
            SHR_IF_ERR_EXIT(
                wstate_words_buf_idx_inc(unit, w_num_words,
                                         &close_msg_after_this_op));
            if (req_undo_ops_info_count) {
                SHR_IF_ERR_EXIT(
                    wstate_undo_ops_info_idx_inc(unit, &close_msg_after_this_op));
                SHR_IF_ERR_EXIT(
                    wstate_undo_words_buf_idx_inc(unit, undo_entry_wsize,
                                                  &close_msg_after_this_op));
            }
            if (slam_en) {
                close_msg_after_this_op = TRUE; /* Use dedicated msg for SLAM */
            }

            /* Update WSTATE(trans_state) */
            /* if (c_trans_state == IDLE) {
             *     we allocated new msg and added one op to it
             *     c_trans_state = IN_MSG;
             * } else if (c_trans_state == NEED_NEW_MSG) {
             *     we allocated new msg and added one op to it
             *     c_trans_state = IN_MSG;
             * } else {
             *     we added one more op to existing msg
             *     c_trans_state = IN_MSG;
             * }
             */
            WSTATE(c_trans_state) = TRANS_STATE_IN_MSG;

            /* Update trans_state (again) if needed
             * This is the only way, we can get to NEED_NEW_MSG state */
            if (close_msg_after_this_op) {
                WSTATE(c_trans_state) = TRANS_STATE_NEED_NEW_MSG;
                print_msg_info(unit, FALSE, WSTATE(c_msg_idx),
                               w_msg_ptr, "WR");
                /* Note - WSTATE(c_msg_idx) is still pointing to last msg
                 *        and not to new msg. We have not allocated new msg
                 *        resource - we are only remembering that we will need
                 *        to start with new_msg when we get the next op
                 */

                
            }

            done = TRUE;
        } else { /* no space, wait */
            retry_count++;
            SHR_IF_ERR_EXIT(wal_wstate_unlock(unit));
            if ((retry_count % 1000) == 0) {
                LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "\n"
                     "avail_trans_count=%0u, req_trans_count=%0u, "
                     "avail_msg_count=%0u, req_msg_count=%0u, "
                     "avail_wal_ops_info_count=%0u, req_wal_ops_info_count=%0u, "
                     "avail_words_buf_count=%0u, max_pt_entry_wsize=%0u, "
                     "avail_undo_ops_info_count=%0u, req_undo_ops_info_count=%0u, "
                     "avail_undo_words_buf_count=%0u, undo_entry_wsize=%0u, "
                     "retry_count=%0d\n"),
                      WSTATE(avail_trans_count), req_trans_count,
                      WSTATE(avail_msg_count), req_msg_count,
                      WSTATE(avail_wal_ops_info_count), 1,
                      WSTATE(avail_words_buf_count), max_pt_entry_wsize,
                      WSTATE(avail_undo_ops_info_count), req_undo_ops_info_count,
                      WSTATE(avail_undo_words_buf_count), undo_entry_wsize,
                      retry_count));
                print_wstate_info(unit, FALSE, "WR ");
            }
            sal_usleep(WAL_USLEEP_TIME);
        } /* no space, wait */
    } while (!done && (retry_count < retry_count_max));
    if (!done) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Not enough space in WAL buffers for %s \n"),
             slam_en ? "SLAM" : "WRITE"));
        SHR_IF_ERR_EXIT(wal_wstate_lock(unit));
        WSTATE(n_words_buf_idx) = tmp_words_buf_idx;
        WSTATE(avail_words_buf_count) += num_words_skipped;
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT(have_wstate_mutex[unit] ? SHR_E_INTERNAL : SHR_E_NONE);

    *rsp_flags = 0; 

    if (bcmptm_wal_autocommit_enabled(unit)) {
        if (w_trans_ptr->num_msgs == cfg_wal_max_msgs_in_trans[unit]) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Coverage: Trans_idx = %0d Auto_Committed "
                            "due to trans_msg_max = %d \n"),
                            WSTATE(c_trans_idx), cfg_wal_max_msgs_in_trans[unit]));
            SHR_IF_ERR_EXIT(
                bcmptm_wal_commit(unit, flags, TRUE, WSTATE(mreq_cseq_id),
                                  NULL, NULL, rsp_flags));
        }
    }

exit:
    if (wstate_mutex[unit]) {
        (void)wal_wstate_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_read(int unit,
                uint32_t flags,
                uint32_t in_cseq_id, 
                bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *pt_dyn_info,
                void *pt_ovrr_info,
                bcmptm_misc_info_t *misc_info,
                size_t rsp_entry_wsize,

                uint32_t *rsp_entry_words,
                uint32_t *rsp_flags)
{
/* - bcmptm_wal_read() is called only when DONT_USE_CACHE flag is set
 *
 * - Default behavior: All uncommitted ops, if any, will be committed,
 *   and we will wait for all ops to finish, issue PIO read,
 *   return read data to caller. Thus reads are BLOCKING by default.
 *
 * - Note: read op is executed 'in order' meaning read will be
 *   performed when all preceding ops in WAL have finished executing.
 */
    bool slam_en;
    int tmp_rv;
    /* wstate_t *wstate_ptr; for gdb debug only */
    wal_trans_t *c_trans_ptr;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    *rsp_flags = 0; 

    /* wstate_ptr = WSTATE_BPTR; */
    print_wstate_info(unit, FALSE, "WAL_RD_REQ ");

    /* req_dwords needed for this entry - none for read */

    /* undo_words needed for this entry - none for read */

    /* wstate_ptr = WSTATE_BPTR; */
    switch (WSTATE(c_trans_state)) {
        case TRANS_STATE_NEED_NEW_MSG:
        case TRANS_STATE_IN_MSG:
            c_trans_ptr = WAL_TRANS_BPTR + WSTATE(c_trans_idx);
            /* Send all messages for current transaction to wal rdr */
            SHR_IF_ERR_EXIT(wal_send_all_msgs(unit, c_trans_ptr,
                                              FALSE)); /* not resend */
            break;
        default: /* BLOCKING_READ, IDLE */
            break;
    }
    WSTATE(c_trans_state) = TRANS_STATE_BLOCKING_READ;

    /* Release prev_data(undo) resources - were needed only for Abort.
     * These are not released by cbf. */
    /* WSTATE: restore n_idx */
    WSTATE(n_undo_ops_info_idx) = 0;
    WSTATE(n_undo_words_buf_idx) = 0;
    /* WSTATE: restore avail counts */
    WSTATE(avail_undo_ops_info_count) = cfg_undo_ops_info_max_count[unit];
    WSTATE(avail_undo_words_buf_count) = cfg_undo_words_buf_max_count[unit];

    
    SHR_IF_ERR_EXIT(bcmptm_wal_drain(unit,
                                     FALSE)); /* wal_empty_end_check */

    /* Align word_buf_idx */
    WSTATE(n_words_buf_idx) = 0; /* WAL is empty, so simplify */

    /* Now this trans has 0 messages.
     * WAL reader thread has released all resources for this transaction and
     * also cleared contents of trans pointed by WSTATE(c_trans_idx)
     *
     * In fact, entire WAL is empty (all trans before BLOCKING_HW_READ were
     * also sent).
     *
     * - WSTATE(a_vars) are all useless when in such state (because
     * - WSTATE(c_trans_state) was set to BLOCKING_READ earlier.
     *   everything has already been committed - nothing to undo)
     * - Possibilities of what we can get after BLOCKING_HW_READ:
     *   - BLOCKING_HW_READ for trans when num_msgs = 0 (ok - this means we
     *   can support multiple BLOCKING_HW_READs in single transaction)
     *   - COMMIT for trans when num_msgs = 0 (ok)
     *   - wal_write for trans when num_msgs = 0 (NOT ok)
     *   - ABORT for trans when num_msgs = 0 (NOT ok - wal pushed all
     *   uncommitted ops to hw and it will be incorrect for rsrc_mgr to do
     *   abort - as it will leave HW and rsrc_mgr out of sync.
     * - In summary, once user sends a BLOCKING_HW_READ, it can only send
     * more BLOCKING_HW_READs or COMMIT. Everything else is ILLEGAL. This
     * feature is intended for debug only.
     */

    slam_en = misc_info && misc_info->dma_enable && dma_avail[unit];
    if (slam_en) {
        tmp_rv = wal_dma_read(unit,
                              sid,
                              pt_dyn_info,
                              pt_ovrr_info,
                              misc_info->dma_entry_count,
                              rsp_entry_words,
                              rsp_entry_wsize);
    } else {
        /* Issue read from HW (pio) */
        tmp_rv = bcmbd_pt_read(unit,
                               sid,
                               pt_dyn_info,
                               pt_ovrr_info,
                               rsp_entry_words, /* read_data */
                               rsp_entry_wsize);

    }

    /* TCAM format -> KM conversion */
    if (bcmdrd_pt_attr_is_cam(unit, sid) &&
        !(flags & BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM)
       ) {
        SHR_IF_ERR_EXIT(
            bcmptm_pt_tcam_xy_to_km(unit, sid,
                                    slam_en ? misc_info->dma_entry_count : 1,
                                    rsp_entry_words,   /* xy_entry */
                                    rsp_entry_words)); /* km_entry */
        /* Note: we are requesting called func to REPLACE xy words
         * with km words to save on un-necessary copy from km words array
         * back to rsp words array.
         */
    }

    if (bcmptm_wal_autocommit_enabled(unit)) {
        WSTATE(c_trans_state) = TRANS_STATE_IDLE;
        WSTATE(c_trans_idx) = C_TRANS_IDX_ILL; /* x */
        WSTATE(c_msg_idx) = C_MSG_IDX_ILL;     /* x */
        WSTATE(c_msg_commit_count) = 0;
    }

    /* If read completed but with error then let this be retried - BUT at
     * mreq level - as we may be blocking SERC from doing correction. */
    SHR_IF_ERR_EXIT(BCMPTM_SHR_FAILURE_SER(tmp_rv) ? SHR_E_BUSY : SHR_E_NONE);
    SHR_IF_ERR_EXIT(tmp_rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_commit(int unit,
                  uint32_t flags,
                  bool mreq_state_in_trans,
                  uint32_t in_cseq_id,
                  bcmptm_trans_cb_f notify_fn_ptr,
                  void *notify_info,

                  uint32_t *rsp_flags)
{
    
    wal_trans_t *c_trans_ptr;
    /* wstate_t *wstate_ptr; for gdb debug only */
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /* wstate_ptr = WSTATE_BPTR; */
    switch (WSTATE(c_trans_state)) {
        case TRANS_STATE_NEED_NEW_MSG:
        case TRANS_STATE_IN_MSG:
            /* No need to take wstate_lock */
            c_trans_ptr = WAL_TRANS_BPTR + WSTATE(c_trans_idx);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Committing cseq_id=%0d, trans_idx=%0d\n"),
                 in_cseq_id, WSTATE(c_trans_idx)));

            if (!bcmptm_wal_autocommit_enabled(unit)) { /* normal mode */
                SHR_IF_ERR_EXIT(!mreq_state_in_trans?
                                SHR_E_INTERNAL : SHR_E_NONE);
                
                SHR_IF_ERR_EXIT((in_cseq_id != c_trans_ptr->cseq_id) ?
                                SHR_E_UNAVAIL : SHR_E_NONE);
            } 

            /* Record notify info */
            c_trans_ptr->notify_fn_ptr = notify_fn_ptr;
            c_trans_ptr->notify_info = notify_info;

            /* Send all messages for current transaction to wal rdr */
            SHR_IF_ERR_EXIT(wal_send_all_msgs(unit, c_trans_ptr, FALSE));

            /* Reset c_trans related WSTATE vars */
            WSTATE(c_trans_state) = TRANS_STATE_IDLE;
            WSTATE(c_trans_idx) = C_TRANS_IDX_ILL; /* x */
            WSTATE(c_msg_idx) = C_MSG_IDX_ILL;     /* x */
            WSTATE(c_msg_commit_count) = 0;

            /* Release prev_data(undo) resources - were needed only for Abort */
            /* WSTATE: restore n_idx */
            WSTATE(n_undo_ops_info_idx) = 0;
            WSTATE(n_undo_words_buf_idx) = 0;
            /* WSTATE: restore avail counts */
            WSTATE(avail_undo_ops_info_count) = cfg_undo_ops_info_max_count[unit];
            WSTATE(avail_undo_words_buf_count) = cfg_undo_words_buf_max_count[unit];
            break;
        case TRANS_STATE_BLOCKING_READ:
            /* All resources (msgs, trans) were already released */
            

            /* Reset c_trans related WSTATE vars */
            WSTATE(c_trans_state) = TRANS_STATE_IDLE;
            WSTATE(c_trans_idx) = C_TRANS_IDX_ILL; /* x */
            WSTATE(c_msg_idx) = C_MSG_IDX_ILL;     /* x */
            WSTATE(c_msg_commit_count) = 0;

            if (notify_fn_ptr) {
                notify_fn_ptr(in_cseq_id, SHR_E_NONE, notify_info);
            }
            break;
        default: /* TRANS_STATE_IDLE */
            /* In auto-commit mode, if we get commit from LTM or unit-test
             * just ignore the commit request */

            SHR_IF_ERR_EXIT(
                (WSTATE(c_trans_state) != TRANS_STATE_IDLE ? SHR_E_INTERNAL
                                                           : SHR_E_NONE));
            if (mreq_state_in_trans) {
                /* Even though there were no WAL ops in cur trans, there
                 * were READ, LOOKUP, GET_FIRST, GET_NEXT - so we
                 * return notification immediately if requested (will
                 * be out of order w.r.t what comes from WAL reader */

                /* Call notification function */
                if (notify_fn_ptr) {
                    notify_fn_ptr(in_cseq_id, SHR_E_NONE, notify_info);
                }
            } else {
                /* Even though there were no WAL ops in cur trans, there may
                 * have been ops for IMM tables.
                 * Also, this could be crash recovery case.
                 * One way to know if this is crash recovery: search for
                 * in_cseq_id in list of pending WAL trans. If it does not exist
                 * then this is IMM case else this is re-commit for previous
                 * trans.
                 */

                 SHR_IF_ERR_EXIT(
                     wal_trans_id_unknown(unit, in_cseq_id, notify_fn_ptr,
                                          notify_info));
            }
            break;
    }

    *rsp_flags = 0; 

exit:
    SHR_FUNC_EXIT();
}

/* We cannot support wal_abort(*) in auto-commit mode */
int
bcmptm_wal_abort(int unit,
                 uint32_t flags,
                 uint32_t in_cseq_id,

                 uint32_t *rsp_flags)
{
    wal_trans_t *c_trans_ptr;
    bcmptm_wal_msg_t *f_wal_msg_ptr;
    /* wstate_t *wstate_ptr; for gdb debug only */

    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /* wstate_ptr = WSTATE_BPTR; */

    switch (WSTATE(c_trans_state)) {
        case TRANS_STATE_BLOCKING_READ:
            
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "ABORT received after "
                            "BLOCKING_HW_READ in same Atomic Transaction "
                            "\n")));

            WSTATE(c_trans_state) = TRANS_STATE_IDLE;
            WSTATE(c_trans_idx) = C_TRANS_IDX_ILL; /* x */
            WSTATE(c_msg_idx) = C_MSG_IDX_ILL;     /* x */
            WSTATE(c_msg_commit_count) = 0;

            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            /* break; - will be unreachable */
        case TRANS_STATE_IN_MSG:
        case TRANS_STATE_NEED_NEW_MSG:
            c_trans_ptr = WAL_TRANS_BPTR + WSTATE(c_trans_idx);
            SHR_IF_ERR_EXIT((in_cseq_id != c_trans_ptr->cseq_id) ?
                            SHR_E_UNAVAIL : SHR_E_NONE);
            f_wal_msg_ptr = BCMPTM_WAL_MSG_BPTR + c_trans_ptr->f_msg_idx;

            /* cseq_id check */
            SHR_IF_ERR_EXIT((in_cseq_id != c_trans_ptr->cseq_id) ?
                            SHR_E_FAIL : SHR_E_NONE);

            if (!wal_disable_undo[unit]) {
                SHR_IF_ERR_EXIT(wal_ptcache_undo(unit, c_trans_ptr));
            }

            /* WSTATE: restore n_idx */
            WSTATE(n_trans_idx) = WSTATE(c_trans_idx);
            WSTATE(n_msg_idx) = WSTATE(a_msg_idx);
            WSTATE(n_wal_ops_info_idx) = WSTATE(a_wal_ops_info_idx);
            WSTATE(n_words_buf_idx) = WSTATE(a_words_buf_idx);
            WSTATE(n_undo_ops_info_idx) = 0;
            WSTATE(n_undo_words_buf_idx) = 0;
            WSTATE(n_umsg_idx) = WSTATE(a_umsg_idx);

            /* WSTATE: restore avail counts */
            SHR_IF_ERR_EXIT(wal_wstate_lock(unit));
            WSTATE(avail_trans_count) += 1;
            WSTATE(avail_msg_count) += c_trans_ptr->num_msgs;
            WSTATE(avail_wal_ops_info_count) += c_trans_ptr->num_ops;
            WSTATE(avail_words_buf_count) += c_trans_ptr->num_words;
            WSTATE(avail_undo_ops_info_count) = cfg_undo_ops_info_max_count[unit];
            WSTATE(avail_undo_words_buf_count) = cfg_undo_words_buf_max_count[unit];
            SHR_IF_ERR_EXIT(wal_wstate_unlock(unit));

            /* WSTATE: c_vars */
            WSTATE(c_trans_state) = TRANS_STATE_IDLE;
            WSTATE(c_trans_idx) = C_TRANS_IDX_ILL; /* x */
            WSTATE(c_msg_idx) = C_MSG_IDX_ILL;     /* x */
            WSTATE(c_msg_commit_count) = 0;

            /* WSTATE: a_idx */
            WSTATE(a_msg_idx) = (uint32_t)(-1);
            WSTATE(a_wal_ops_info_idx) = (uint32_t)(-1);
            WSTATE(a_words_buf_idx) = (uint32_t)(-1);

            /* Clean wal_msg */
            sal_memset(f_wal_msg_ptr, 0,
                       sizeof(bcmptm_wal_msg_t) * c_trans_ptr->num_msgs);

            /* Finally clean trans */
            sal_memset(c_trans_ptr, 0, sizeof(wal_trans_t));
            break; /* NEED_NEW_MSG, IN_MSG */

        default: /* TRANS_STATE_IDLE */
            /* Nothing to release */
            SHR_IF_ERR_EXIT(
                (WSTATE(c_trans_state) != TRANS_STATE_IDLE ? SHR_E_INTERNAL
                                                           : SHR_E_NONE));
            break;
    } /* trans_state */

    *rsp_flags = 0; 

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_serc_data_find(int unit,
                          uint32_t flags,
                          bcmdrd_sid_t req_sid,
                          bcmbd_pt_dyn_info_t *req_dyn_info,
                          void *req_ovrr_info,
                          size_t rsp_entry_wsize,
                          int req_slice_num,
                          bcmptm_rm_slice_mode_t req_slice_mode,

                          uint32_t *rsp_entry_words)
{
    SHR_FUNC_ENTER(unit);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "====== Did not find matching entry in WAL\n")));

    SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL); /* restore data not in WAL */
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_drain(int unit, bool end_check)
{
    int retry_count;
    wal_trans_t *c_trans_ptr;
    uint32_t rsp_flags;
    bool empty = FALSE;
    wal_trans_state_t wal_c_trans_state;
    SHR_FUNC_ENTER(unit);

    if (bcmptm_wal_autocommit_enabled(unit)) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "In wal drain \n")));

        /* We could have uncommitted messages, so commit these first */
        if (WSTATE(c_trans_state) != TRANS_STATE_IDLE &&
            WSTATE(c_trans_state) != TRANS_STATE_BLOCKING_READ) {
            c_trans_ptr = WAL_TRANS_BPTR + WSTATE(c_trans_idx);
            SHR_IF_ERR_EXIT(
                bcmptm_wal_commit(unit, 0, TRUE, c_trans_ptr->cseq_id,
                                  NULL, NULL, &rsp_flags));
            /* Note: sending 0 as flags ensures that bcmptm_wal_commit does not
             *       again call bcmptm_wal_drain.
             *
             *       In any case, above code is for 'fake' TRANS_AUTO_COMMIT mode - so
             *       in normal case, bcmptm_wal_drain  does not call bcmptm_wal_commit
             */
        }
    }

    retry_count = 0;
    wal_c_trans_state = WSTATE(c_trans_state);
    do {
        SHR_IF_ERR_EXIT(
            wal_empty_check(unit,
                            (retry_count == WAL_EMPTY_CHECK_RETRY_COUNT),
                            /* show_not_empty_values */
                            &empty, &wal_c_trans_state));
        if (empty) {
            if (end_check) {
                if (wal_c_trans_state != TRANS_STATE_IDLE) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "WAL is empty, but wal_c_trans_state "
                                    "is not IDLE (%0d) !!\n"),
                         wal_c_trans_state));
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
                }
            } else {
                if ((wal_c_trans_state != TRANS_STATE_IDLE)
                    && (wal_c_trans_state != TRANS_STATE_BLOCKING_READ)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "WAL is empty, but wal_c_trans_state "
                                    "is not IDLE or BLOCKING_READ (%0d) !!\n"),
                         wal_c_trans_state));
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
                }
            }
            break;
        }
        retry_count++;
        if (retry_count > WAL_EMPTY_CHECK_RETRY_COUNT) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "WAL not empty !!\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Waiting for WAL to become empty\n")));
            sal_usleep(WAL_EMPTY_CHECK_USLEEP_TIME);
        }
    } while (retry_count <= WAL_EMPTY_CHECK_RETRY_COUNT);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_init(int unit, bool warm)
{
    const bcmcfg_feature_ctl_config_t *feature_conf;
    int tmp_rv, num_chans = 0;
    size_t cmd_mem_wsize = 0;
    SHR_FUNC_ENTER(unit);

    dma_avail[unit] = !bcmdrd_feature_is_sim(unit);

    /* schanfifo driver init */
    if (BCMPTM_CFG_WAL_SCF_NUM_CHANS && !bcmdrd_feature_is_sim(unit)) {
        tmp_rv = bcmbd_schanfifo_info_get(unit, &num_chans, &cmd_mem_wsize);
        if (tmp_rv == SHR_E_NONE) {
            assert(num_chans > 0);
            assert(cmd_mem_wsize > 0);
            SHR_IF_ERR_EXIT(
                bcmbd_schanfifo_init(unit,
                                     BCMPTM_CFG_WAL_SCF_MAX_POLLS,
                                     SCHANFIFO_IF_POLL_WAIT |
                                     SCHANFIFO_IF_IGNORE_SER_ABORT |
                                     SCHANFIFO_IF_CCMDMA_WR));
            if (num_chans > BCMPTM_CFG_WAL_SCF_NUM_CHANS) {
                bcmptm_wal_scf_num_chans[unit] = BCMPTM_CFG_WAL_SCF_NUM_CHANS;
            } else {
                bcmptm_wal_scf_num_chans[unit] = num_chans;
            }
            wal_scf_cmd_mem_wsize[unit] = cmd_mem_wsize;
        } /* else - scf is not avail */
    }

    /* cfg_wal_mode[unit] = 0; */
        /* By default, cfg_wal_mode[unit] will be 0.
         * User must call bcmptm_wal_mode_sel() before init
         * to choose one of the auto-commit modes */

    SHR_IF_ERR_EXIT(bcmptm_wal_cfg_set(unit));
        /* will adjust numbers as per wal_mode */

    feature_conf = bcmcfg_feature_ctl_config_get();
    if ((feature_conf == NULL) || (feature_conf->dis_stomic_trans == 0)) {
        wal_disable_undo[unit] = FALSE; /* must be done before alloc */
    } else {  /* Feature is disabled */
        wal_disable_undo[unit] = TRUE; /* must be done before alloc */
    }
    SHR_IF_ERR_EXIT(wal_alloc(unit, warm));

    wstate_mutex[unit] = sal_mutex_create("WAL_WSTATE_LOCK");
    SHR_NULL_CHECK(wstate_mutex[unit], SHR_E_MEMORY);
    have_wstate_mutex[unit] = FALSE;

    WSTATE(avail_msg_count) = cfg_wal_msg_max_count[unit];
    WSTATE(avail_words_buf_count) = cfg_wal_words_buf_max_count[unit];
    WSTATE(avail_wal_ops_info_count) = cfg_wal_ops_info_max_count[unit];
    WSTATE(avail_trans_count) = cfg_wal_trans_max_count[unit];
    WSTATE(avail_undo_ops_info_count) = cfg_undo_ops_info_max_count[unit];
    WSTATE(avail_undo_words_buf_count) = cfg_undo_words_buf_max_count[unit];

    WSTATE(mreq_state_in_trans) = FALSE;
    WSTATE(mreq_cseq_id) = 0; /* dont_care when mreq_state_in_trans is FALSE */

    /* We are always working in mode where modeled path will not stop on errors
     */
    WSTATE(rdr_ignore_op_err) = TRUE;

    /* All of these become meaningful only when wal_send_all_msgs()
     * is called */
    bcmptm_wal_msg_dq_idx[unit] = 0;
    bcmptm_wal_msg_dq_ptr[unit] = NULL;
    bcmptm_wal_msg_dq_ready_ptr[unit] = NULL;
    wal_msg_dq_enable[unit] = FALSE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_cleanup(int unit)
{
    int i;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(wal_free(unit));

    sal_mutex_destroy(wstate_mutex[unit]);
    have_wstate_mutex[unit] = FALSE;

    cfg_wal_mode[unit] = 0;

    for (i = 0; i < BCMPTM_RM_MC_GROUP_COUNT_MAX; i++) {
        mc_count[unit][i] = 0;
    }
    wal_disable_undo[unit] = FALSE;

    wal_scf_cmd_mem_wsize[unit] = 0;

    wal_msg_dq_enable[unit] = FALSE;
    dma_avail[unit] = FALSE;

    bcmptm_wal_msg_dq_ready_ptr[unit] = NULL;
    bcmptm_wal_msg_dq_idx[unit] = 0;
    bcmptm_wal_msg_dq_ptr[unit] = NULL;
    /* WAL reader cannot start draining the 'committed' msgs as soon as
     * wal_rdr thread is enabled - WAL writer must wake up reader only
     * AFTER it initializes the msg_dq_idx, msg_dq_ptr, msg_dq_ready_ptr.
     *
     * For crash-recovery cases this allows WAL writer a chance to
     * re-initialize 'walr_dq_idx' and re-commit the messages when it gets
     * 're-commit for old transaction' from TRM.
     */

    /* schanfifo_cleanup will happen as part of schanfifo_detach, so no
     * need for WAL to call this.
     */
    bcmptm_wal_scf_num_chans[unit] = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_mode_sel(int unit, int wal_mode)
{
    SHR_FUNC_ENTER(unit);

    cfg_wal_mode[unit] = wal_mode;

    if (wal_mode < 0) {
        LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Will operate WAL in Auto-Commit mode (%0d)\n"),
             wal_mode));
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

bool
bcmptm_wal_autocommit_enabled(int unit)
{
    if (cfg_wal_mode[unit] < 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void
bcmptm_wal_dma_avail(int unit, bool read_op, uint32_t entry_count,
                     bool *wal_dma_avail)
{
    if (wal_dma_avail) { /* !NULL */
        if (read_op){
            *wal_dma_avail = dma_avail[unit] &&
                             (entry_count >= BCMPTM_CFG_WAL_DMA_OP_COUNT_THR);
        } else {
            *wal_dma_avail = dma_avail[unit] &&
                             (entry_count >= BCMPTM_CFG_WAL_SLAM_OP_COUNT_THR);
        }
    }
}
