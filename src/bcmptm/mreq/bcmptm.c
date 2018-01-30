/*! \file bcmptm.c
 *
 * Interface functions for PTM
 *
 * This file contains the top-level interface func for PTM
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
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_sleep.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_um_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_wal_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MREQ

#define STOP_WAL_RDR_FALSE FALSE
#define STOP_WAL_RDR_TRUE TRUE

#define BCMPTM_MREQ_USLEEP_TIME 1000 /* uSec */
#define BCMPTM_MREQ_HW_READ_RETRY_COUNT_MAX 1000

#define BCMPTM_MREQ_RETRY_PREP do {\
    SHR_IF_ERR_EXIT(!(flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) ?\
                    SHR_E_INTERNAL : SHR_E_NONE);\
    LOG_WARN(BSL_LOG_MODULE,\
        (BSL_META_U(unit, "!! ERROR: %s from HW "\
                    "saw SER failure. Will retry. \n"),\
         req_op_name));\
    retry_count++;\
    (void)(bcmptm_mreq_unlock(unit));\
    have_mreq_lock[unit] = FALSE;\
    sal_usleep(BCMPTM_MREQ_USLEEP_TIME);\
    SHR_IF_ERR_EXIT(bcmptm_mreq_lock(unit));\
    have_mreq_lock[unit] = TRUE;\
} while (0)

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static uint32_t cp_entry_buf[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_MAX_MREQ_ENTRY_WORDS];
static bcmptm_lt_stats_increment_cb lt_stats_cb[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_pt_status_mreq_keyed_lt_cb \
              lt_mreq_keyed_cb[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_pt_status_mreq_indexed_lt_cb \
              lt_mreq_indexed_cb[BCMDRD_CONFIG_MAX_UNITS];
static bool have_mreq_lock[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/* Following will be called for every NOP request from LTM
 * It changes mreq_state_in_trans to FALSE */
static int
proc_nop(int unit, uint32_t flags, uint32_t cseq_id,
         bcmptm_trans_cb_f notify_fn_ptr, void *notify_info)
{
    bcmptm_trans_cmd_t trans_cmd;
    SHR_FUNC_ENTER(unit);

    if (bcmptm_wal_autocommit_enabled(unit)) {
        /* Send notification to TRM */
        if ((flags & BCMPTM_REQ_FLAGS_COMMIT) && notify_fn_ptr) {
            notify_fn_ptr(cseq_id, SHR_E_NONE, notify_info);
        }
        /* Ignore commit - all ops were already auto-committed
         * Ignore abort - TRM does abort of trans even if LOOKUP returns NOT_FOUND,
         * so if bcmptm returns NOT_SUPPORTED - SQA will complain.
         */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (flags & BCMPTM_REQ_FLAGS_COMMIT) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Commit: trans_id = %0d\n"), cseq_id));
        /* Zero:
         *     cseq_id stored in cmdproc is meaningful ONLY when
         *     mreq_state_in_trans == TRUE
         *
         * Case 1:
         *     We receive commit when mreq_state_in_trans is TRUE.
         *     Some HW acc was done for current trans and we get commit.
         *     WAL writer is not IDLE in such case - and hence notification
         *     will come from WAL reader when ops are sent to HW.
         *     Here cseq_id of commit must match what we had stored in cmdproc.
         *
         * Case 2:
         *     We receive commit when mreq_state_in_trans is TRUE.
         *     No HW acc was done for current trans (only reads/lookups from
         *     PTcache or some IMM table were done in current trans) - so WAL
         *     writer is IDLE - and hence notification must come from WAL writer
         *     when we issue wal_commit.
         *     Here cseq_id of commit must match what we had stored in cmdproc.
         *
         * Case 3:
         *     We receive commit when mreq_state_in_trans is FALSE - no question
         *     of checking cseq_id with what is stored in cmdproc.
         *     There were no HW ops in current trans, nor any READs/LOOKUPs, etc
         *     - but only accesses to some IMM based LT. In this case WAL writer
         *     is also in IDLE. We cannot distinguish between Case 3 and Case 4
         *     (below). We must search previously committed cseq_id in WAL
         *     starting from oldest to newest - and we will not find any
         *     matching trans with this cseq_id - and in such case we must
         *     simply return notification (from wal_commit)
         *
         * Case 4: (crash recovery)
         *     mreq_state_in_trans will be FALSE - no question of checking
         *     cseq_id with what is stored in cmdproc.
         *     We can receive commit for older trans and also for last
         *     uncommitted trans. Must search previously committed cseq_id
         *     (in WAL - starting from oldest to newest) and start re-sending
         *     msgs for older trans when commit(cseq_id) matches trans_id in
         *     WAL.
         *     Note: there are several cases for WAL in Case 4:
         *           Case 4a: WAL state is IDLE - because crash had occurred
         *                    when there were no uncommitted ops in WAL
         *           Case 4b: WAL state is not_IDLE - because crash had occurred
         *                    when there were uncommitted ops in WAL
         *
         * Question: How does WAL writer know to send notification immediately?
         *
         * Case 2: WAL writer sends notification if
         *         (WAL state == IDLE) && (mreq_state_in_trans = TRUE)
         *
         * Case 3: WAL writer sends notification if
         *         (WAL state == IDLE) && (mreq_state_in_trans = FALSE) &&
         *         (commit_cseq_id not found in WAL)
         *
         * Case 4:  (mreq_state_in_trans = FALSE)
         *         There are lot of cases here. FIXME
 */

        trans_cmd = BCMPTM_TRANS_CMD_COMMIT;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! NOP: received with COMMIT \n")));
    } else if (flags & BCMPTM_REQ_FLAGS_ABORT) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Abort: trans_id = %0d\n"), cseq_id));
        trans_cmd = BCMPTM_TRANS_CMD_ABORT;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! NOP: received with ABORT \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! NOP: received without COMMIT/ABORT \n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Send trans_cmd first to cmdproc(WAL) on purpose:
     * If user issues Blocking_Read_from_HW, it must follow it
     * up with commit. wal_abort() will flag error otherwise. */
    SHR_IF_ERR_EXIT(bcmptm_wal_trans_cmd(unit, flags,
                                         trans_cmd,
                                         cseq_id, notify_fn_ptr,
                                         notify_info));
    /* And then to rsrc_mgrs */
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_cmd(unit, flags, cseq_id, trans_cmd));
    SHR_IF_ERR_EXIT(bcmptm_rm_tcam_trans_cmd(unit, flags, cseq_id, trans_cmd));
    SHR_IF_ERR_EXIT(bcmptm_rm_tcam_um_trans_cmd(unit, flags, cseq_id, trans_cmd));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_trans_cmd(unit, flags, cseq_id, trans_cmd));

exit:
    SHR_FUNC_EXIT();
}

static int
ptm_check_ltm_ptrs(int unit,
                   bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
                   bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
                   unsigned int num_ekw,
                   unsigned int num_edw)
{
    unsigned int i;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltm_req_info->entry_key_words, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_req_info->entry_data_words, SHR_E_PARAM);
    /* ltm_req_info->pdd_l_dtyp can be NULL. */
    for (i = 0; i < num_ekw; i++) {
        SHR_NULL_CHECK(ltm_req_info->entry_key_words[i], SHR_E_PARAM);
    }
    for (i = 0; i < num_edw; i++) {
        SHR_NULL_CHECK(ltm_req_info->entry_data_words[i], SHR_E_PARAM);
    }

    SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_key_words, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_data_words, SHR_E_PARAM);
    
    for (i = 0; i < num_ekw; i++) {
        SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_key_words[i], SHR_E_PARAM);
    }
    for (i = 0; i < num_edw; i++) {
        SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_data_words[i], SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mreq_rm_alpm(int unit,
             uint32_t flags,
             bcmltd_sid_t ltm_req_ltid,
             void *pt_dyn_info,
             bcmptm_op_type_t req_op,
             bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
             uint32_t cseq_id,

             bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
             bcmltd_sid_t *ltm_rsp_ltid,
             uint32_t *ltm_rsp_flags)
{
#define LEVELS 3
    bcmptm_rm_alpm_req_info_t alpm_req = {0};
    bcmptm_rm_alpm_rsp_info_t alpm_rsp = {0};

    uint32_t alpm_index[LEVELS] = {0};
    uint32_t alpm_sid[LEVELS] = {0};
    bcmltd_sid_t alpm_rsp_ltid;
    uint32_t alpm_rsp_flags;
    uint8_t i;
    int rv;

    SHR_FUNC_ENTER(unit);
    /* Build alpm_req */
    alpm_req.ekw = ltm_req_info->entry_key_words[0];
    alpm_req.edw = ltm_req_info->entry_data_words[0];
    if (ltm_req_info->pdd_l_dtyp) {
        alpm_req.dfield_format_id =
            *(ltm_req_info->pdd_l_dtyp); /* 1st element */
    }
    alpm_req.same_key = ltm_req_info->same_key;
    alpm_req.rsp_ekw_buf_wsize = ltm_req_info->rsp_entry_key_buf_wsize;
    alpm_req.rsp_edw_buf_wsize = ltm_req_info->rsp_entry_data_buf_wsize;

    /* Init ptrs in alpm_rsp */
    alpm_rsp.rsp_ekw = ltm_rsp_info->rsp_entry_key_words[0];
    alpm_rsp.rsp_edw = ltm_rsp_info->rsp_entry_data_words[0];

    alpm_rsp.rsp_entry_index = alpm_index;
    alpm_rsp.rsp_entry_sid = alpm_sid;

    /* Post req to rsrc_mgr */
    rv = bcmptm_rm_alpm_req(unit,
                            flags,
                            cseq_id,
                            ltm_req_ltid,
                            (bcmbd_pt_dyn_info_t *)pt_dyn_info,
                            req_op,
                            &alpm_req,

                            &alpm_rsp,
                            &alpm_rsp_ltid,
                            &alpm_rsp_flags);

    if (rv == SHR_E_NOT_FOUND || rv == SHR_E_BUSY) {
        SHR_RETURN_VAL_EXIT(rv);
    } else if (rv == SHR_E_PARAM) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Build ltm_rsp_info */
    /* rsp_entry_key_words, rsp_entry_data_words,
       already filled by rsrc_mgr */

    /* Return rsp_pdd_l_dtyp. */
    if (ltm_rsp_info->rsp_pdd_l_dtyp) {
        *(ltm_rsp_info->rsp_pdd_l_dtyp) = alpm_rsp.rsp_dfield_format_id;
    }
    ltm_rsp_info->rsp_entry_pipe = 0;
    ltm_rsp_info->rsp_entry_pri = 0;
    
    ltm_rsp_info->rsp_entry_index_cnt = alpm_rsp.rsp_entry_count;
    ltm_rsp_info->rsp_entry_sid_cnt = alpm_rsp.rsp_entry_count;
    for (i = 0; i < alpm_rsp.rsp_entry_count; i++) {
        ltm_rsp_info->rsp_entry_index[i] = alpm_rsp.rsp_entry_index[i];
        ltm_rsp_info->rsp_entry_sid[i] = alpm_rsp.rsp_entry_sid[i];
    }
    ltm_rsp_info->aggregate_view = TRUE;

    /* Finally */
    *ltm_rsp_ltid = alpm_rsp_ltid;
    *ltm_rsp_flags = alpm_rsp_flags;
exit:
    SHR_FUNC_EXIT();
}

static int
mreq_rm_hash(int unit,
             uint32_t flags,
             bcmltd_sid_t ltm_req_ltid,
             void *pt_dyn_info,
             bcmptm_op_type_t req_op,
             bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
             uint32_t cseq_id,
             const lt_mreq_info_t *lrd_info,

             bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
             bcmltd_sid_t *ltm_rsp_ltid,
             uint32_t *ltm_rsp_flags)
{
    bcmptm_rm_hash_req_t hash_req_info;
    bcmptm_rm_hash_rsp_t hash_rsp_info;
    uint32_t i, j, *ekw, *edw;
    int tmp_rv;
    SHR_FUNC_ENTER(unit);

    /* clear rsrc_mgr req, rsp info */
    sal_memset(&hash_req_info, 0, sizeof(bcmptm_rm_hash_req_t));
    sal_memset(&hash_rsp_info, 0, sizeof(bcmptm_rm_hash_rsp_t));

    if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
        /* lrd checks */
        SHR_IF_ERR_EXIT(lrd_info->num_ekw != 1? SHR_E_INTERNAL
                                              : SHR_E_NONE);
        SHR_IF_ERR_EXIT(lrd_info->num_edw != 1? SHR_E_INTERNAL
                                              : SHR_E_NONE);
        SHR_IF_ERR_EXIT(
            ptm_check_ltm_ptrs(unit,
                               ltm_req_info, ltm_rsp_info,
                               1, 1));
        SHR_IF_ERR_EXIT(
            lrd_info->aggr_ekw_edw > BCMPTM_MAX_MREQ_ENTRY_WORDS?
            SHR_E_INTERNAL : SHR_E_NONE);

        i = 0; /* only 1st row is useful for aggr LTs */
        ekw = ltm_req_info->entry_key_words[i];
        edw = ltm_req_info->entry_data_words[i];
        for (j = 0; j < lrd_info->aggr_ekw_edw; j++) {
            cp_entry_buf[unit][j] = ekw[j] | edw[j];
        }
        hash_req_info.entry_words = &cp_entry_buf[unit][0];
        if (ltm_req_info->pdd_l_dtyp) {
            hash_req_info.dfield_format_id =
                *(ltm_req_info->pdd_l_dtyp); /* 1st element */
        }
    } else { /* Non Aggr type LT */
        /* for hash tables, we must have aggr_ekw_edw non-zero */
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } /* Non Aggr type LT */

    /* Build remaining hash_req_info */
    hash_req_info.same_key = ltm_req_info->same_key;

    /* Pass entry_attrs to RM hash. */
    if ((lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
        (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)){
        hash_req_info.entry_attrs = ltm_req_info->entry_attrs;
    }
    /* Post req to rsrc_mgr */
    tmp_rv =
        bcmptm_rm_hash_req(unit,
                           flags,
                           cseq_id,
                           ltm_req_ltid,
                           (bcmbd_pt_dyn_info_t *)pt_dyn_info,
                           req_op,
                           &hash_req_info,
                           ltm_req_info->rsp_entry_key_buf_wsize,

                           ltm_rsp_info->rsp_entry_key_words[0],
                           &hash_rsp_info,
                           ltm_rsp_ltid,
                           ltm_rsp_flags);
    if (tmp_rv == SHR_E_NOT_FOUND || tmp_rv == SHR_E_BUSY) {
        SHR_RETURN_VAL_EXIT(tmp_rv);
    } else if (tmp_rv == SHR_E_PARAM) {
        SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
    } else {
        SHR_IF_ERR_EXIT(tmp_rv);
    }

    i = 0; /* only 1st row is useful for aggr LTs */

    /* Ensure that edw is big enough to hold ekw */
    SHR_IF_ERR_EXIT(
        (ltm_req_info->rsp_entry_data_buf_wsize <
         ltm_req_info->rsp_entry_key_buf_wsize)? SHR_E_INTERNAL
                                               : SHR_E_NONE);
    /* Copy */
    sal_memcpy(ltm_rsp_info->rsp_entry_data_words[i], /* dst */
               ltm_rsp_info->rsp_entry_key_words[i], /* src */
               ltm_req_info->rsp_entry_key_buf_wsize * 4);

    /* Return pdd_l_dtyp */
    if (ltm_rsp_info->rsp_pdd_l_dtyp) {
        *(ltm_rsp_info->rsp_pdd_l_dtyp) =
            hash_rsp_info.rsp_dfield_format_id;
    }

    /* Build remaining ltm_rsp_info */
    ltm_rsp_info->rsp_entry_pipe = 0;
    ltm_rsp_info->rsp_entry_pri = 0;

    
    ltm_rsp_info->rsp_entry_index_cnt = 1;
    ltm_rsp_info->rsp_entry_index[0] = hash_rsp_info.rsp_entry_index;
    ltm_rsp_info->rsp_entry_sid_cnt = 1;
    ltm_rsp_info->rsp_entry_sid[0] = hash_rsp_info.rsp_entry_sid;
    ltm_rsp_info->aggregate_view =
        lrd_info->aggr_ekw_edw? TRUE : FALSE;
exit:
    SHR_FUNC_EXIT();
}

static int
mreq_rm_tcam(int unit,
             uint32_t flags,
             bcmltd_sid_t ltm_req_ltid,
             void *pt_dyn_info,
             bcmptm_op_type_t req_op,
             bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
             uint32_t cseq_id,
             const lt_mreq_info_t *lrd_info,

             bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
             bcmltd_sid_t *ltm_rsp_ltid,
             uint32_t *ltm_rsp_flags)
{
    bcmptm_rm_tcam_req_t tcam_req_info;
    bcmptm_rm_tcam_rsp_t tcam_rsp_info;
    uint32_t i, j, *ekw, *edw, *tcam_aggr_buf[1];
    int tmp_rv, idx;
    SHR_FUNC_ENTER(unit);

    /* Clear rsrc_mgr req, rsp info */
    sal_memset(&tcam_req_info, 0, sizeof(bcmptm_rm_tcam_req_t));
    sal_memset(&tcam_rsp_info, 0, sizeof(bcmptm_rm_tcam_rsp_t));

    if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
        /* lrd checks */
        SHR_IF_ERR_EXIT(lrd_info->num_ekw != 1? SHR_E_INTERNAL
                                              : SHR_E_NONE);
        SHR_IF_ERR_EXIT(lrd_info->num_edw != 1? SHR_E_INTERNAL
                                              : SHR_E_NONE);
        SHR_IF_ERR_EXIT(
            ptm_check_ltm_ptrs(unit,
                               ltm_req_info, ltm_rsp_info,
                               1, 1));
        SHR_IF_ERR_EXIT(
            lrd_info->aggr_ekw_edw > BCMPTM_MAX_MREQ_ENTRY_WORDS?
            SHR_E_INTERNAL : SHR_E_NONE);

        i = 0; /* only 1st row is useful for aggr LTs */
        ekw = ltm_req_info->entry_key_words[i];
        edw = ltm_req_info->entry_data_words[i];
        for (j = 0; j < lrd_info->aggr_ekw_edw; j++) {
            cp_entry_buf[unit][j] = ekw[j] | edw[j];
        }
        tcam_aggr_buf[0] = cp_entry_buf[unit];
        tcam_req_info.ekw = tcam_aggr_buf;
        tcam_req_info.edw = NULL; /* should be dont_care */
    } else { /* Non Aggr type LT */
        /* lrd checks */
        SHR_IF_ERR_EXIT(
            ptm_check_ltm_ptrs(unit,
                               ltm_req_info, ltm_rsp_info,
                               lrd_info->num_ekw,
                               lrd_info->num_edw));

        /* Must not combine ekw, edw */
        tcam_req_info.ekw = ltm_req_info->entry_key_words;
        tcam_req_info.edw = ltm_req_info->entry_data_words;
    } /* Non Aggr type LT */

    /* Build remaining tcam_req_info */
    tcam_req_info.entry_id = ltm_req_info->entry_id;
    tcam_req_info.entry_pri = ltm_req_info->entry_pri;
    tcam_req_info.same_key = ltm_req_info->same_key;
    tcam_req_info.rsp_ekw_buf_wsize =
        ltm_req_info->rsp_entry_key_buf_wsize;
    tcam_req_info.rsp_edw_buf_wsize =
        ltm_req_info->rsp_entry_data_buf_wsize;
    tcam_req_info.dfield_format_id = ltm_req_info->pdd_l_dtyp;

    if (lrd_info->pt_type == LT_PT_TYPE_FP) {
        tcam_req_info.entry_attrs = ltm_req_info->entry_attrs;
    }

    /* Init ptrs in tcam_rsp_info */
    tcam_rsp_info.rsp_ekw = ltm_rsp_info->rsp_entry_key_words;
    tcam_rsp_info.rsp_edw = ltm_rsp_info->rsp_entry_data_words;
    tcam_rsp_info.rsp_dfield_format_id =
        ltm_rsp_info->rsp_pdd_l_dtyp;

    /* Post req to rsrc_mgr */
    tmp_rv =
        bcmptm_rm_tcam_req(unit,
                           flags,
                           cseq_id,
                           ltm_req_ltid,
                           (bcmbd_pt_dyn_info_t *)pt_dyn_info,
                           req_op,
                           &tcam_req_info,

                           &tcam_rsp_info,
                           ltm_rsp_ltid,
                           ltm_rsp_flags);
    if (tmp_rv == SHR_E_NOT_FOUND || tmp_rv == SHR_E_BUSY) {
        SHR_RETURN_VAL_EXIT(tmp_rv);
    } else if (tmp_rv == SHR_E_PARAM) {
        SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
    } else {
        SHR_IF_ERR_EXIT(tmp_rv);
    }

    if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
        /* rsrc_mgr returns key, data fields in rsp_ekw */

        i = 0; /* only 1st row is useful for aggr LTs */

        /* Ensure that edw is big enough to hold ekw */
        SHR_IF_ERR_EXIT(
            (ltm_req_info->rsp_entry_data_buf_wsize <
             ltm_req_info->rsp_entry_key_buf_wsize)?
            SHR_E_INTERNAL : SHR_E_NONE);
        /* Copy */
        sal_memcpy(ltm_rsp_info->rsp_entry_data_words[i], /* dst */
                   ltm_rsp_info->rsp_entry_key_words[i], /* src */
                   ltm_req_info->rsp_entry_key_buf_wsize * 4);
    } else { /* Non Aggr type LT */
        /* rsrc_mgr returns key, data fields in rsp_ekw, rsp_edw
           respectively. */

        /* We must not copy rsp_ekw into rsp_edw */

    } /* Non Aggr type LT */

    /* Build remaining ltm_rsp_info */
    ltm_rsp_info->rsp_entry_pipe = tcam_rsp_info.rsp_entry_pipe;
    ltm_rsp_info->rsp_entry_pri = tcam_rsp_info.rsp_entry_pri;

    if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
        ltm_rsp_info->rsp_entry_index_cnt = 1;
        ltm_rsp_info->rsp_entry_index[0] = tcam_rsp_info.rsp_entry_index[0];
        ltm_rsp_info->rsp_entry_sid_cnt = 1;
        ltm_rsp_info->rsp_entry_sid[0] = tcam_rsp_info.rsp_entry_sid[0];
    } else {
        /* KEY related info */
        if ((tcam_rsp_info.rsp_entry_index_count > BCMPTM_RSP_MAX_INDEX) ||
            (tcam_rsp_info.rsp_entry_data_index_count > BCMPTM_RSP_MAX_INDEX)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        for (idx = 0 ; idx < tcam_rsp_info.rsp_entry_index_count; idx++) {
            ltm_rsp_info->rsp_entry_index[idx] =
                tcam_rsp_info.rsp_entry_index[idx];
            ltm_rsp_info->rsp_entry_sid[idx] =
                tcam_rsp_info.rsp_entry_sid[idx];
        }
        ltm_rsp_info->rsp_entry_index_cnt =
            tcam_rsp_info.rsp_entry_index_count;;
        ltm_rsp_info->rsp_entry_sid_cnt = tcam_rsp_info.rsp_entry_index_count;

        /* Data related info */
        for (idx = 0 ; idx < tcam_rsp_info.rsp_entry_data_index_count; idx++) {
            ltm_rsp_info->rsp_entry_index_data[idx] =
                tcam_rsp_info.rsp_entry_data_index[idx];
            ltm_rsp_info->rsp_entry_sid_data[idx] =
                tcam_rsp_info.rsp_entry_data_sid[idx];
        }
        ltm_rsp_info->rsp_entry_index_data_cnt =
            tcam_rsp_info.rsp_entry_data_index_count;;
        ltm_rsp_info->rsp_entry_sid_data_cnt =
            tcam_rsp_info.rsp_entry_data_index_count;

    }

    ltm_rsp_info->aggregate_view =
        lrd_info->aggr_ekw_edw? TRUE : FALSE;
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_mreq_indexed_lt(int unit,
                       uint32_t flags,
                       bcmdrd_sid_t sid,
                       void *in_pt_dyn_info,
                       void *pt_ovrr_info,
                       bcmptm_misc_info_t *misc_info,
                       bcmptm_op_type_t req_op,
                       uint32_t *entry_words,
                       size_t rsp_entry_wsize,
                       bcmltd_sid_t req_ltid,
                       uint32_t cseq_id,
                       bcmptm_trans_cb_f notify_fn_ptr,
                       void *notify_info,

                       uint32_t *rsp_entry_words,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    const char *pt_name = "INVALIDm";
    char *req_op_name = "NOP";
    int tmp_rv;
    uint32_t retry_count = 0;
    bool rsp_entry_cache_vbit = FALSE;
    uint16_t rsp_dfield_format_id = 0;

    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    if (req_op != BCMPTM_OP_NOP) {
        SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
        SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    }
    /* SHR_NULL_CHECK(pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    pt_name = bcmdrd_pt_sid_to_name(unit, sid);
    *rsp_flags = 0;
    pt_dyn_info = (bcmbd_pt_dyn_info_t *)in_pt_dyn_info;

    if (req_op != BCMPTM_OP_NOP) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "mreq_indexed_lt: trans_id = %0d\n"), cseq_id));
        /* Debug checks - must not be done during runtime */
        if (!bcmdrd_pt_is_valid(unit, sid) ||
            (pt_dyn_info->index < bcmdrd_pt_index_min(unit, sid)) ||
            (pt_dyn_info->index > bcmdrd_pt_index_max(unit, sid)) ||
            !bcmdrd_pt_index_valid(unit, sid, pt_dyn_info->tbl_inst,
                                   pt_dyn_info->index)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    switch (req_op) {
        case BCMPTM_OP_WRITE:
            if (!have_mreq_lock[unit]) {
                SHR_IF_ERR_EXIT(bcmptm_mreq_lock(unit));
                have_mreq_lock[unit] = TRUE;
            }

            /* update mreq_state, check cseq_id */
            SHR_IF_ERR_EXIT(bcmptm_wal_mreq_state_update(unit, cseq_id, flags));

            req_op_name = "WRITE";
            tmp_rv =
                bcmptm_cmdproc_write(unit, /* will check *entry_words */
                                     flags,
                                     cseq_id,
                                     sid,
                                     pt_dyn_info,
                                     pt_ovrr_info,
                                     misc_info,
                                     entry_words,
                                     TRUE, /* write_hw */
                                     TRUE, /* write_cache */
                                     TRUE, /* set_cache_vbit */
                                     BCMPTM_RM_OP_NORMAL, NULL,
                                     req_ltid,
                                     BCMPTM_DFID_MREQ, /* Note */

                                     rsp_flags);
            if (tmp_rv == SHR_E_UNAVAIL) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! ERROR: WRITE: sid = %s is not "
                                "cacheable - should not be on modeled "
                                "path !!\n"),
                     pt_name));
            }
            SHR_IF_ERR_EXIT(tmp_rv);

            *rsp_ltid = req_ltid;
            break; /* WRITE */

        case BCMPTM_OP_READ:
            if (!have_mreq_lock[unit]) {
                SHR_IF_ERR_EXIT(bcmptm_mreq_lock(unit));
                have_mreq_lock[unit] = TRUE;
            }

            /* update mreq_state, check cseq_id */
            SHR_IF_ERR_EXIT(bcmptm_wal_mreq_state_update(unit, cseq_id, flags));

            req_op_name = "READ";
            do {
                tmp_rv =
                    bcmptm_cmdproc_read(unit, /* will check *rsp_entry_words */
                                        flags,
                                        cseq_id,
                                        sid,
                                        pt_dyn_info,
                                        pt_ovrr_info,
                                        misc_info,
                                        rsp_entry_wsize,

                                        rsp_entry_words,
                                        &rsp_entry_cache_vbit,
                                        rsp_ltid,
                                        &rsp_dfield_format_id,
                                        rsp_flags);
                if (tmp_rv == SHR_E_BUSY) {
                    BCMPTM_MREQ_RETRY_PREP;
                    continue; /* retry */
                } else if (tmp_rv == SHR_E_UNAVAIL) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "!! ERROR: READ: sid = %s is not "
                                    "cacheable - should not be on modeled "
                                    "path !!\n"),
                         pt_name));
                }
            } while ((tmp_rv == SHR_E_BUSY) &&
                     (retry_count < BCMPTM_MREQ_HW_READ_RETRY_COUNT_MAX));
            SHR_IF_ERR_EXIT(tmp_rv); /* case when retry_count is reached
                                        and other errors */

            /* If read was from cache */
            if (!(flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) &&
                (rsp_dfield_format_id != BCMPTM_DFID_RSP_NOT_SUPPORTED)) {
                
                LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! READ: Did not expect modeled "
                                "path index table (sid = %s) to support "
                                "DFID !!\n"),
                     pt_name));
                /* SHR_IF_ERR_EXIT(SHR_E_UNAVAIL); */
            }

            /* Read was from cache, and cache entry is not valid */
            if (!rsp_entry_cache_vbit &&
                !(flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! Read from PTcache on modeled "
                                "path is returning HW default values - "
                                "as entry (sid, tbl_inst, index) = %s, %0d,"
                                " %0d was never written prior to read \n"),
                     pt_name, pt_dyn_info->tbl_inst, pt_dyn_info->index));

                /* Not asserting error allows multiple LTs to share same PT
                 * entry - because for such LTs, LTM must always do
                 * read-modify-write */
                /* SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND); */

                /* For non-cacheable PTs, we will not be here - because
                   cmdproc_read will return SHR_E_UNAVAIL

                   Even if we init cache with correct 'null entry value',
                   we could still get vbit=FALSE for cacheable PTs - and
                   this indicates that there was no previous_writer (data
                   may / may not be correct depending on whether this is an
                   overlay of wider view, DATA portion of TCAM_DATA view,
                   etc). Even with vbit=TRUE, data could be incorrect. For
                   example if we read DATA_ONLY view of cacheable TCAM+DATA
                   mem then contents of DATA_ONLY are never updated and
                   hence they are not correct. Similarly, if 4x entry was
                   written, 1x reads will return vbit=FALSE.

                   For overlay mems, we can get cache_vbit as FALSE, but data is
                   still valid.
                   For TCAM mems where we don't implement overlay cache_vbit is
                   FALSE, and data is also not valid

                   For regular index tables, PTcache will return null value
                   of entry if cache_vbit=FALSE.
                   - PTcache is init with null entries and PTcache will
                     return entry data from PTcache even when cache_vbit
                     is FALSE */
            }
            break; /* READ */

        case BCMPTM_OP_NOP:
            /* *rsp_ltid = req_ltid; */ /* dont_care for NOP */
            *rsp_flags = 0; 
            tmp_rv = proc_nop(unit, flags, cseq_id, notify_fn_ptr,
                              notify_info); /* COMMIT/ABORT */
            
            if (have_mreq_lock[unit]) {
                (void)(bcmptm_mreq_unlock(unit));
                have_mreq_lock[unit] = FALSE;
            }
            SHR_IF_ERR_EXIT(tmp_rv);
            break; /* NOP */

        default: /* ill for this API */
            *rsp_ltid = req_ltid;
            *rsp_flags = 0; 

            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    } /* req_op */

    /* Debug display */
    if (req_op != BCMPTM_OP_NOP) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Table_Type=%s, req_op=%s, req_sid=%s, "
                        "req_tbl_inst=%0d, req_index=%0d\n"),
             "INDEX", req_op_name, pt_name,
             pt_dyn_info->tbl_inst,
             pt_dyn_info->index));
    }

exit:
    
    SHR_FUNC_EXIT();
}

static char *
get_req_op_name(bcmptm_op_type_t req_op)
{
    switch (req_op) {
        case BCMPTM_OP_GET_FIRST:
            return "GET_FIRST";
        case BCMPTM_OP_GET_NEXT:
            return "GET_NEXT";
        case BCMPTM_OP_LOOKUP:
            return "LOOKUP";
        case BCMPTM_OP_DELETE:
            return "DELETE";
        case BCMPTM_OP_INSERT:
            return "INSERT";
        case BCMPTM_OP_NOP:
            return "NOP";
        default:
            return "UNKNOWN";
    }
}

int
bcmptm_mreq_keyed_lt(int unit,
                     uint32_t flags,
                     bcmltd_sid_t ltm_req_ltid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     bcmptm_op_type_t req_op,
                     bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
                     uint32_t cseq_id,

                     bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
                     bcmltd_sid_t *ltm_rsp_ltid,
                     uint32_t *ltm_rsp_flags)
{
    const lt_mreq_info_t *lrd_info = NULL;
    uint32_t retry_count = 0;
    const char *pt_name = "INVALIDm";
    char *req_op_name = "NOP";
    char *req_table_type = "UNKNOWN";
    int tmp_rv;
    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(ltm_req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_rsp_flags, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "mreq_keyed_lt: trans_id = %0d\n"), cseq_id));
    switch (req_op) {
        case BCMPTM_OP_GET_FIRST:
        case BCMPTM_OP_GET_NEXT:
        case BCMPTM_OP_LOOKUP:
        case BCMPTM_OP_DELETE:
        case BCMPTM_OP_INSERT:
            if (req_op != BCMPTM_OP_LOOKUP &&
                req_op != BCMPTM_OP_GET_FIRST &&
                req_op != BCMPTM_OP_GET_NEXT) {
                flags &= ~BCMPTM_REQ_FLAGS_DONT_USE_CACHE;
                    /* this flag is only for LOOKUP or READ or TRAVERSE */
            }

            if (!have_mreq_lock[unit]) {
                SHR_IF_ERR_EXIT(bcmptm_mreq_lock(unit));
                have_mreq_lock[unit] = TRUE;
            }

            /* update mreq_state, check cseq_id */
            SHR_IF_ERR_EXIT(bcmptm_wal_mreq_state_update(unit, cseq_id, flags));

            /* query for LT info */
            SHR_IF_ERR_MSG_EXIT(
                bcmlrd_lt_mreq_info_get(unit, ltm_req_ltid, &lrd_info),
                (BSL_META_U(unit, "Logical Table info needed by RM is not "
                            "available for Logical Table ID %0u\n"),
                 ltm_req_ltid));
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);

            do {
                switch (lrd_info->pt_type) {
                    case LT_PT_TYPE_HASH:
                    case LT_PT_TYPE_FP_EM:
                    case LT_PT_TYPE_HASH_MPLS_DECAP:
                        req_table_type = "HASH";
                        tmp_rv = mreq_rm_hash(unit,
                                              flags,
                                              ltm_req_ltid,
                                              pt_dyn_info,
                                              req_op,
                                              ltm_req_info,
                                              cseq_id,
                                              lrd_info,

                                              ltm_rsp_info,
                                              ltm_rsp_ltid,
                                              ltm_rsp_flags);
                        break; /* rm_hash */

                    case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
                    case LT_PT_TYPE_TCAM_PRI_ONLY:
                    case LT_PT_TYPE_FP:
                        req_table_type = "TCAM_PRI";
                        tmp_rv = mreq_rm_tcam(unit,
                                              flags,
                                              ltm_req_ltid,
                                              pt_dyn_info,
                                              req_op,
                                              ltm_req_info,
                                              cseq_id,
                                              lrd_info,

                                              ltm_rsp_info,
                                              ltm_rsp_ltid,
                                              ltm_rsp_flags);
                        break; /* rm_tcam */

                    case LT_PT_TYPE_LPM_V4:
                    case LT_PT_TYPE_LPM_V6:
                        req_table_type = "ALPM";
                        tmp_rv = mreq_rm_alpm(unit,
                                              flags,
                                              ltm_req_ltid,
                                              pt_dyn_info,
                                              req_op,
                                              ltm_req_info,
                                              cseq_id,

                                              ltm_rsp_info,
                                              ltm_rsp_ltid,
                                              ltm_rsp_flags);
                        break;

                    default: /* ill pt_type */
                        tmp_rv = SHR_E_UNAVAIL;
                        /* break; */
                } /* pt_type */

                if (tmp_rv == SHR_E_BUSY) {
                    BCMPTM_MREQ_RETRY_PREP;
                    continue; /* retry */
                } else if (tmp_rv == SHR_E_NOT_FOUND) {
                    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
                } else if (tmp_rv == SHR_E_PARAM) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                } else {
                    SHR_IF_ERR_EXIT(tmp_rv);
                }
            } while ((tmp_rv == SHR_E_BUSY) &&
                     (retry_count < BCMPTM_MREQ_HW_READ_RETRY_COUNT_MAX));
            SHR_IF_ERR_EXIT(tmp_rv); /* case when retry_count is reached */

            /* Debug display */
            pt_name = bcmdrd_pt_sid_to_name(unit, ltm_rsp_info->rsp_entry_sid[0]);
            req_op_name = get_req_op_name(req_op);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Table_Type=%s, req_op=%s, rsp_sid=%s, "
                            "rsp_index=%0d, rsp_entry_pri=%0d\n"),
                 req_table_type, req_op_name, pt_name,
                 ltm_rsp_info->rsp_entry_index[0],
                 ltm_rsp_info->rsp_entry_pri));
            break; /* LOOKUP, DELETE, INSERT */

        default: /* ill req_op */
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
            /* break; */
    } /* req_op */

exit:
    
    SHR_FUNC_EXIT();
}

void
bcmptm_lt_stat_increment_register(int unit,
                                  bcmptm_lt_stats_increment_cb lt_stat_cb)
{
    lt_stats_cb[unit] = lt_stat_cb;
}

bcmptm_lt_stats_increment_cb
bcmptm_lt_stat_increment_get(int unit)
{
    return lt_stats_cb[unit];
}

void
bcmptm_pt_status_mreq_keyed_lt_register(int unit,
                      bcmptm_pt_status_mreq_keyed_lt_cb pt_status_cb)
{
    lt_mreq_keyed_cb[unit] = pt_status_cb;
}

void
bcmptm_pt_status_mreq_indexed_lt_register(int unit,
                      bcmptm_pt_status_mreq_indexed_lt_cb pt_status_cb)
{
    lt_mreq_indexed_cb[unit] = pt_status_cb;
}

int
bcmptm_ltm_mreq_indexed_lt(int unit,
                           uint32_t flags,
                           bcmdrd_sid_t sid,
                           void *pt_dyn_info,
                           void *pt_ovrr_info,
                           bcmptm_misc_info_t *misc_info,
                           bcmptm_op_type_t req_op,
                           uint32_t *entry_words,
                           size_t rsp_entry_wsize,
                           bcmltd_sid_t req_ltid,
                           uint32_t cseq_id,
                           bcmptm_trans_cb_f notify_fn_ptr,
                           void *notify_info,
                           uint32_t *rsp_entry_words,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags)
{
    SHR_FUNC_ENTER(unit);

    if (lt_stats_cb[unit] != NULL) {
        lt_stats_cb[unit](unit, req_ltid,
                          TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                flags,
                                sid,
                                pt_dyn_info,
                                pt_ovrr_info,
                                misc_info,
                                req_op,
                                entry_words,
                                rsp_entry_wsize,
                                req_ltid,
                                cseq_id,
                                notify_fn_ptr,
                                notify_info,
                                rsp_entry_words,
                                rsp_ltid,
                                rsp_flags));

    if (lt_mreq_indexed_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_mreq_indexed_cb[unit](unit, req_ltid, sid,
                                      pt_dyn_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        if (lt_stats_cb[unit] != NULL) {
            lt_stats_cb[unit](unit, req_ltid,
                              TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ltm_mreq_keyed_lt(int unit,
                         uint32_t flags,
                         bcmltd_sid_t req_ltid,
                         void *pt_dyn_info,
                         void *pt_ovrr_info,
                         bcmptm_op_type_t req_op,
                         bcmptm_keyed_lt_mreq_info_t *req_info,
                         uint32_t cseq_id,
                         bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                         bcmltd_sid_t *rsp_ltid,
                         uint32_t *rsp_flags)
{
    SHR_FUNC_ENTER(unit);

    if (lt_stats_cb[unit] != NULL) {
        lt_stats_cb[unit](unit, req_ltid,
                          TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_mreq_keyed_lt(unit,
                              flags,
                              req_ltid,
                              pt_dyn_info,
                              pt_ovrr_info,
                              req_op,
                              req_info,
                              cseq_id,
                              rsp_info,
                              rsp_ltid,
                              rsp_flags));

    if (lt_mreq_keyed_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_mreq_keyed_cb[unit](unit, req_ltid, pt_dyn_info, rsp_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        if (lt_stats_cb[unit] != NULL) {
            lt_stats_cb[unit](unit, req_ltid,
                              TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_mreq_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    

/* exit: */
    SHR_FUNC_EXIT();
}


int
bcmptm_mreq_stop(int unit)
{
    bcmptm_trans_cmd_t trans_cmd = BCMPTM_TRANS_CMD_IDLE_CHECK;
    uint32_t flags = 0;
    int tmp_rv;
    SHR_FUNC_ENTER(unit);

    if (bcmptm_wal_autocommit_enabled(unit)) {
        /* rsrc_mgrs - check they are in idle */

        /* RM code already supports trans_cmd - so RMs will get to IDLE state
         * only when COMMIT is passed to RMs.
         *
         * But RM unittests dont issue any commit - they rely on auto-commit mode
         * of WAL - and thus there are no commits to RMs and hence RMs in
         * unit-tests end up in non-IDLE state.
         *
         * Hence, following will reduce severity to WARN and not create failure
         * when in auto-commit mode.
         */
        tmp_rv = bcmptm_rm_hash_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Hash rsrc mgr is not in IDLE \n")));
        }
        tmp_rv = bcmptm_rm_tcam_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Tcam rsrc mgr is not in IDLE \n")));
        }
        tmp_rv = bcmptm_rm_tcam_um_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Tcam_um rsrc mgr is not in IDLE \n")));
        }
        tmp_rv = bcmptm_rm_alpm_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! ALPM rsrc mgr is not in IDLE \n")));
        }
    } else {
        SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_cmd(unit, flags, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_tcam_trans_cmd(unit, flags, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_tcam_um_trans_cmd(unit, flags, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_trans_cmd(unit, flags, 0, trans_cmd));
    }

    /* We already ensured that LTM (and hence TRM) has stopped, so we know LTM
     * is not issuing more requests */

    /* wal_mreq_idle_check */
    SHR_IF_ERR_EXIT(
        bcmptm_wal_trans_cmd(unit, flags, trans_cmd, 0, NULL, NULL));

    /* wal drain */
    SHR_IF_ERR_EXIT(bcmptm_wal_drain(unit, STOP_WAL_RDR_TRUE));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_mreq_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    

/* exit: */
    SHR_FUNC_EXIT();
}
