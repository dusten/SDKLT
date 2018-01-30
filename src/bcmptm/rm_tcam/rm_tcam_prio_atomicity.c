/*! \file rm_tcam_prio_atomicity.c
 *
 * Low Level Functions for Atomicity support of TCAMs
 * This file contains low level functions for Atomicity support
 * of TCAMs during TCAM entry Update.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_fp.h"
#include "rm_tcam_fp_utils.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private variables
 */
static uint32_t entry_words[BCMDRD_CONFIG_MAX_UNITS]
                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS][BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t *entry_words_row[BCMDRD_CONFIG_MAX_UNITS]
                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];

/* This routine updates the existing entry along with
 * changing its priority (if any)
 * This is called only for prio_only type TCAMs.
 */

static int
rm_tcam_prio_only_entry_update_prio(int unit,
                               uint32_t req_flags,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_rm_tcam_req_t *req_info,
                               bcmptm_rm_tcam_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags,
                               uint32_t found_index)
{

    uint32_t *entry_hash = NULL;
    uint32_t hash_val;
    uint32_t target_index = -1;
    int rv = SHR_E_NONE;
    uint8_t entry_type;
    uint32_t idx;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Find Index to Insert the TCAM entry */
    rv = bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                       req_flags, ltid, ltid_info,
                                       1, pt_dyn_info,
                                       req_info, rsp_info,
                                       rsp_ltid, rsp_flags, &target_index);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
    if (rv == SHR_E_RESOURCE) {
        /*call compress API and call allocate again */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_compress(unit,
                                                                 ltid,
                                                                 ltid_info,
                                                                 pt_dyn_info,
                                                                 req_info->entry_attrs));

        SHR_IF_ERR_VERBOSE_EXIT(
                 bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                                               req_flags,
                                                               ltid,
                                                               ltid_info,
                                                               1,
                                                               pt_dyn_info,
                                                               req_info,
                                                               rsp_info,
                                                               rsp_ltid,
                                                               rsp_flags,
                                                               &target_index));
    }
    /* Insert the new entry at target_idx. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_write(unit, ltid, ltid_info,
                                                    req_flags, 1,
                                                    target_index, pt_dyn_info,
                                                    req_info,
                                                    rsp_ltid, rsp_flags,
                                                    rsp_info->rsp_entry_sid));

    /* Since original entry may have got moved, lookup to find found_index */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_lookup(unit,
                                          req_flags, ltid, ltid_info,
                                          pt_dyn_info,
                                          req_info, rsp_info,
                                          rsp_ltid, rsp_flags,
                                          &found_index));


    /* Clear the data in SW and HW state */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_clear(unit, ltid, ltid_info,
                                                    req_flags, 0, found_index,
                                                    pt_dyn_info, req_info,
                                                    rsp_ltid, rsp_flags,
                                                    rsp_info->rsp_entry_sid));

   /* Update the SW state in two steps. */

    /* 1. Update the TCAM entry information. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid,
                                                       ltid_info,
                                                       pt_dyn_info->tbl_inst,
                                                       &entry_info));

    entry_type = ltid_info->hw_entry_info->num_key_rows;

    for (idx = target_index; idx < (target_index + entry_type); idx++) {
        entry_info[idx].entry_pri = req_info->entry_pri;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = entry_type;
    }

    /* 2. Update the TCAM SW hash table. */
    rv = bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit, ltid,
                                                      ltid_info,
                                                      req_info->ekw,
                                                      NULL, &hash_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_hash_get(unit, ltid,
                                                 ltid_info,
                                                 pt_dyn_info->tbl_inst,
                                                 &entry_hash);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    entry_info[target_index].offset = entry_hash[hash_val];
    entry_hash[hash_val] = target_index;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_read(unit,
                                               ltid, ltid_info, req_flags,
                                               target_index,
                                               pt_dyn_info,
                                               req_info, rsp_info,
                                               rsp_ltid, rsp_flags));

    rsp_info->rsp_entry_pri = req_info->entry_pri;


exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();

}

/* This routine updates the existing entry along with
 * changing its priority (if any)
 * This is called only for prio_eid type TCAMs.
 */

static int
rm_tcam_prio_entry_update_prio(int unit,
                               uint32_t req_flags,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_rm_tcam_req_t *req_info,
                               bcmptm_rm_tcam_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags,
                               uint32_t found_index)
{

    uint32_t entry_hash_size = 0;
    int *entry_hash = NULL;
    uint16_t hash_val;
    uint16_t target_index = -1;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_size_get(
                                            unit, ltid, ltid_info,
                                            req_info->entry_attrs,
                                            &entry_hash_size));
    /* Find Index to Insert the TCAM entry */
    SHR_IF_ERR_VERBOSE_EXIT((bcmptm_rm_tcam_prio_entry_index_allocate(unit,
                                       req_flags, ltid, ltid_info,
                                       1, pt_dyn_info,
                                       req_info, rsp_info,
                                       rsp_ltid, rsp_flags, &target_index)));

    /* Insert the new entry at target_idx. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_write(unit, ltid, ltid_info,
                                                    req_flags, 1,
                                                    target_index,
                                                    pt_dyn_info, req_info,
                                                    rsp_ltid, rsp_flags,
                                                    rsp_info->rsp_entry_sid));

    /* Since original entry may have got moved, lookup to find found_index */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_lookup(unit,
                                          req_flags, ltid, ltid_info,
                                          pt_dyn_info,
                                          req_info, rsp_info,
                                          rsp_ltid, rsp_flags,
                                          &found_index));

    /* Clear the data in SW and HW state */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_clear(unit, ltid, ltid_info,
                                                    req_flags, 0, found_index,
                                                    pt_dyn_info, req_info,
                                                    rsp_ltid, rsp_flags,
                                                    rsp_info->rsp_entry_sid));

    /* Update the SW state in two steps. */

    /* 1. Update the TCAM entry information. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid,
                                                   ltid_info,
                                                   req_info->entry_attrs,
                                                   &entry_info));

    entry_info[target_index].entry_id = req_info->entry_id;
    entry_info[target_index].entry_pri = req_info->entry_pri;
    entry_info[target_index].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;

    /* 2. Update the TCAM SW hash table. */
    hash_val = (entry_info[target_index].entry_id)
                % (entry_hash_size);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_get(unit,
                                                       ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_hash));


    entry_info[target_index].offset = entry_hash[hash_val];
    entry_hash[hash_val] = target_index;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_read(unit,
                                               ltid, ltid_info, req_flags,
                                               target_index, pt_dyn_info,
                                               req_info, rsp_info,
                                               rsp_ltid, rsp_flags));

    rsp_info->rsp_entry_pri = req_info->entry_pri;

    if (ltid_info->pt_type == LT_PT_TYPE_FP ) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_incr(unit,
                                                         ltid, ltid_info,
                                                         found_index,
                                                         req_info->entry_attrs));
    }
exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();

}

/* This routines does an entry (key + data) update operation for
 * PRIO_ONLY type TCAMs
 */
static int
rm_tcam_prio_only_entry_update(int unit,
                              uint32_t req_flags,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmbd_pt_dyn_info_t *pt_dyn_info,
                              bcmptm_rm_tcam_req_t *req_info,
                              bcmptm_rm_tcam_rsp_t *rsp_info,
                              bcmltd_sid_t *rsp_ltid,
                              uint32_t *rsp_flags,
                              uint8_t update_type,
                              uint16_t found_index)
{

    int target_idx     = BCMPTM_RM_TCAM_INDEX_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_req_t tmp_req_info;
    uint8_t mode = 0;
    int num_parts = -1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                               pt_dyn_info->tbl_inst,
                                               &tcam_info));

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)req_info->entry_attrs;
        mode = entry_attrs->group_mode;
    } else {
        mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    num_parts = ltid_info->hw_entry_info[mode].num_key_rows;

    if (num_parts < ltid_info->hw_entry_info[mode].num_data_rows) {
        num_parts = ltid_info->hw_entry_info[mode].num_data_rows;
    }

    /*
     * Trigger SW atomicity either if
     * 1. HW doesn't support atomicity or
     * 2. HW supports atomicity but entry modification
     *    is in either key or both data + key and num key parts is more then 1.
     *    Refer below table where atomicity can't be achieved
     *    even when HW supports it.
     *                Agrr      |     Non_aggr
     *   ekw        1  | >1     |     1   | >1
     *   -------------------------------------
     *   key only   Y  | N      |     Y   | N
     *   key+Data   Y  | N      |     N   | N
     *   Data only  Y  | Y      |     Y   | Y
     */

    if ((ltid_info->rm_more_info->hw_atomicity_support == 0) ||
        (ltid_info->rm_more_info->hw_atomicity_support &&
         (update_type != BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
         ((num_parts > 1) || (ltid_info->non_aggr && (update_type == 0))))) {
        /* Entry update using backup entry creation */
        SHR_RETURN_VAL_EXIT
            (rm_tcam_prio_only_entry_update_prio(unit, req_flags, ltid,
                                                 ltid_info, pt_dyn_info,
                                                 req_info, rsp_info, rsp_ltid,
                                                 rsp_flags, found_index));

    } else {
        /* HW Atomicity can be acheived, update entry at the same index */
        /* Read exiting entry at found_index. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                ltid, ltid_info, req_flags,
                                                found_index,
                                                pt_dyn_info,
                                                req_info, rsp_info,
                                                rsp_ltid, rsp_flags));

        sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
        tmp_req_info.entry_attrs = req_info->entry_attrs;
        tmp_req_info.ekw = rsp_info->rsp_ekw;
        tmp_req_info.edw = rsp_info->rsp_edw;
        tmp_req_info.rsp_ekw_buf_wsize = req_info->rsp_ekw_buf_wsize;
        tmp_req_info.rsp_edw_buf_wsize = req_info->rsp_edw_buf_wsize;

        /* Delete entry at found_index in SW only mode. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_only_entry_clear(unit, ltid, ltid_info,
                                                 req_flags,
                                                 BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY,
                                                 found_index,
                                                 pt_dyn_info,
                                                 &tmp_req_info,
                                                 rsp_ltid, rsp_flags,
                                                 rsp_info->rsp_entry_sid));
        /* Update the entry at found_index. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_only_entry_write(unit, ltid, ltid_info,
                                                 req_flags, 0,
                                                 found_index,
                                                 pt_dyn_info,
                                                 req_info,
                                                 rsp_ltid, rsp_flags,
                                                 rsp_info->rsp_entry_sid));
        target_idx = found_index;

    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                   ltid, ltid_info, req_flags,
                                                   target_idx,
                                                   pt_dyn_info,
                                                   req_info, rsp_info,
                                                   rsp_ltid, rsp_flags));

    rsp_info->rsp_entry_pri = req_info->entry_pri;

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();

}

/* This routines does an entry (key + data) update operation for
 * PRIO_EID type TCAMs
 */

static int
rm_tcam_prio_entry_update(int unit,
                              uint32_t req_flags,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmbd_pt_dyn_info_t *pt_dyn_info,
                              bcmptm_rm_tcam_req_t *req_info,
                              bcmptm_rm_tcam_rsp_t *rsp_info,
                              bcmltd_sid_t *rsp_ltid,
                              uint32_t *rsp_flags,
                              uint8_t update_type,
                              int found_index)
{

    int target_idx     = BCMPTM_RM_TCAM_INDEX_INVALID;
    bcmptm_rm_tcam_req_t tmp_req_info;
    uint8_t mode = 0;
    int num_parts = -1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)req_info->entry_attrs;
        mode = entry_attrs->group_mode;
    } else {
        mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    num_parts = ltid_info->hw_entry_info[mode].num_key_rows;

    if (num_parts < ltid_info->hw_entry_info[mode].num_data_rows) {
        num_parts = ltid_info->hw_entry_info[mode].num_data_rows;
    }

    /*
     * Trigger SW atomicity either if
     * 1. HW doesn't support atomicity or
     * 2. HW supports atomicity but entry modification
     *    is in either key or both data + key and num key parts is more then 1.
     *    Refer below table where atomicity can't be achieved
     *    even when HW supports it.
     *                Agrr      |     Non_aggr
     *   ekw        1  | >1     |     1   | >1
     *   -------------------------------------
     *   key only   Y  | N      |     Y   | N
     *   key+Data   Y  | N      |     N   | N
     *   Data only  Y  | Y      |     Y   | Y
     */

    if ((ltid_info->rm_more_info->hw_atomicity_support == 0) ||
        (ltid_info->rm_more_info->hw_atomicity_support &&
         (update_type != BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
         ((num_parts > 1) || (ltid_info->non_aggr && (update_type == 0))))) {
        /* Entry update using backup entry creation */
        SHR_RETURN_VAL_EXIT
            (rm_tcam_prio_entry_update_prio(unit, req_flags, ltid, ltid_info,
                                            pt_dyn_info, req_info, rsp_info,
                                            rsp_ltid, rsp_flags, found_index));
    } else {
        /* HW Atomicity can be acheived, update entry at the same index */
        sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
        tmp_req_info.entry_attrs = req_info->entry_attrs;
        tmp_req_info.entry_id = req_info->entry_id;

        /* Delete entry at found_index in SW only mode. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_entry_clear(unit, ltid, ltid_info,
                                            req_flags,
                                            BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY,
                                            found_index,
                                            pt_dyn_info,
                                            &tmp_req_info,
                                            rsp_ltid, rsp_flags,
                                            rsp_info->rsp_entry_sid));

        /* Update the entry at found_index. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_entry_write(unit, ltid, ltid_info,
                                            req_flags, 0,
                                            found_index,
                                            pt_dyn_info,
                                            req_info,
                                            rsp_ltid, rsp_flags,
                                            rsp_info->rsp_entry_sid));
        target_idx = found_index;
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_read(unit,
                                                   ltid, ltid_info,
                                                   req_flags,
                                                   target_idx,
                                                   pt_dyn_info,
                                                   req_info, rsp_info,
                                                   rsp_ltid, rsp_flags));

    rsp_info->rsp_entry_pri = req_info->entry_pri;

    if (ltid_info->pt_type != LT_PT_TYPE_FP ) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_incr(unit,
                                                         ltid, ltid_info,
                                                         found_index,
                                                         req_info->entry_attrs));
    }


exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();

}


/*******************************************************************************
 * Public Functions
 */
/* This routine helps to identify if the update requested
 * for an entry is a change in data portion and key portion
 * not getting updated.
 * The output variable update_type = 2 for data portion only update.
 * The output variable update_type = 1 for key portion only change.
 * The output variable update_type = 0 for both key and data change.
 */
int
bcmptm_rm_tcam_update_type_get(int unit,
                        uint32_t req_flags,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        bcmbd_pt_dyn_info_t *pt_dyn_info,
                        bcmptm_rm_tcam_req_t *req_info,
                        bcmptm_rm_tcam_rsp_t *rsp_info,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags,
                        uint32_t found_index,
                        uint8_t *update_type)
{

    uint8_t part = 0;
    uint8_t word = 0;
    uint16_t key_length = 0;
    uint16_t data_length = 0;
    uint32_t num_words = 0;
    uint32_t rsp_ekw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t rsp_edw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t req_ekw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t req_edw[BCMPTM_MAX_PT_FIELD_WORDS];
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;

    /* Log the function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Read the entry data */
    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                   ltid, ltid_info, req_flags,
                                                   found_index,
                                                   pt_dyn_info,
                                                   req_info, rsp_info,
                                                   rsp_ltid, rsp_flags));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_read(unit,
                                                   ltid, ltid_info, req_flags,
                                                   found_index,
                                                   pt_dyn_info,
                                                   req_info, rsp_info,
                                                   rsp_ltid, rsp_flags));
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_lt_hw_entry_info_get(unit, ltid_info,
                                            req_info->entry_attrs,
                                            &hw_entry_info));

    key_length = hw_entry_info->key_size;
    /* If data change is found set data only indicator */
    for (part = 0; part < hw_entry_info->num_data_rows; part++) {
        sal_memset(rsp_edw, 0, sizeof(rsp_edw));
        sal_memset(req_edw, 0, sizeof(req_edw));
        data_length = hw_entry_info->data_end_bit[part]  -
                      hw_entry_info->data_start_bit + 1;

        if (ltid_info->non_aggr) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_data_get(unit,
                              rsp_info->rsp_edw[part],
                              0, data_length, rsp_edw));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_data_get(unit,
                              req_info->edw[part],
                              0, data_length, req_edw));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_data_get(unit,
                              rsp_info->rsp_ekw[part],
                              hw_entry_info->data_start_bit,
                              data_length, rsp_edw));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_data_get(unit,
                              req_info->ekw[part],
                              hw_entry_info->data_start_bit,
                              data_length, req_edw));
        }

        num_words = BCMDRD_BYTES2WORDS((data_length + 7) / 8);
        for (word = 0; word < num_words; word++) {
            if (req_edw[word] != rsp_edw[word]) {
                *update_type = BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE;
                break;
            }
        }
    }

    /* If key change is found unset data only indicator */
    for (part = 0; part < hw_entry_info->num_key_rows; part++) {
        sal_memset(rsp_ekw, 0, sizeof(rsp_ekw));
        sal_memset(req_ekw, 0, sizeof(req_ekw));

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_data_get(unit,
                        rsp_info->rsp_ekw[part],
                        0, key_length, rsp_ekw));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_data_get(unit,
                        req_info->ekw[part],
                        0, key_length, req_ekw));

        num_words = BCMDRD_BYTES2WORDS((key_length + 7) / 8);
        for (word = 0; word < num_words; word++) {
            if (req_ekw[word] != rsp_ekw[word]) {
                if (*update_type & BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) {
                    *update_type = 0;
                } else {
                    *update_type = BCMPTM_RM_TCAM_ENTRY_KEY_ONLY_UPDATE;
                }
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    }

    /* If no  changes are found, return as Exists */
    if (*update_type == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/* This routine is called to update only the data portion
 * in the TCAM
 */
int
bcmptm_rm_tcam_entry_update_data_only(int unit,
                               uint32_t req_flags,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_rm_tcam_req_t *req_info,
                               bcmptm_rm_tcam_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags,
                               uint32_t found_index)
{

    uint8_t part = 0;
    uint32_t width = 0;
    uint16_t data_length = 0;
    uint8_t slice_row = 0;
    uint32_t sindex = -1;
    int num_words = -1;
    uint32_t rm_flags = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
         sal_memset(entry_words[unit][part], 0 ,
                    sizeof(entry_words[unit][part]));
         entry_words_row[unit][part] = entry_words[unit][part];
    }

    /* set the data only view */
    rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;

    if (!(ltid_info->non_aggr)) {
         width = ltid_info->num_ekw;
         /* Extract data portion */
         for (part = 0; part < width; part++) {

             if (ltid_info->pt_type != LT_PT_TYPE_FP ) {
                  SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_tindex_to_sindex(unit,
                                                                  ltid,
                                                                  ltid_info,
                                                                  found_index,
                                                                  &sindex,
                                                                  &slice_row));
              } else {
                  SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                   ltid, ltid_info,
                                                   req_info->entry_attrs,
                                                   rm_flags,
                                                   found_index, part,
                                                   &slice_row,
                                                   &sindex));
              }
              num_words = bcmdrd_pt_entry_wsize(unit,
                  ltid_info->hw_entry_info->sid_data_only[slice_row][part]);

              data_length =
                    (ltid_info->hw_entry_info->data_end_bit[part]
                     - ltid_info->hw_entry_info->data_start_bit + 1);

              SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_data_get(unit,
                             req_info->ekw[part],
                             ltid_info->hw_entry_info->data_start_bit,
                             data_length,
                             entry_words_row[unit][part]));
         }
    } else {

        width = ltid_info->num_edw;
        /* Copy data portion from request */
        for (part = 0; part < width; part++) {
            if (ltid_info->pt_type != LT_PT_TYPE_FP ) {
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_tindex_to_sindex(unit, ltid,
                                                                 ltid_info,
                                                                 found_index,
                                                                 &sindex,
                                                                 &slice_row));
             } else {
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                         ltid,
                                                         ltid_info,
                                                         req_info->entry_attrs,
                                                         rm_flags,
                                                         found_index, part,
                                                         &slice_row,
                                                         &sindex));
             }
             num_words = bcmdrd_pt_entry_wsize(unit,
                 ltid_info->hw_entry_info->sid_data_only[slice_row][part]);

             sal_memcpy(entry_words_row[unit][part], req_info->edw[part],
                        (sizeof(uint32_t) * num_words));
        }
    }

    /*  Write the associated data to index. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit,
                                            ltid, ltid_info,
                                            rm_flags, req_flags,
                                            found_index,
                                            entry_words_row[unit],
                                            pt_dyn_info,
                                            NULL, req_info->entry_attrs,
                                            rsp_ltid, rsp_flags,
                                            rsp_info->rsp_entry_sid));

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                   ltid, ltid_info, req_flags,
                                                   found_index,
                                                   pt_dyn_info,
                                                   req_info, rsp_info,
                                                   rsp_ltid, rsp_flags));
    } else {
         SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_read(unit,
                                                   ltid, ltid_info, req_flags,
                                                   found_index,
                                                   pt_dyn_info,
                                                   req_info, rsp_info,
                                                   rsp_ltid, rsp_flags));
    }

    rsp_info->rsp_entry_pri = req_info->entry_pri;

exit:

    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/* This is the main routine called in case of an existing entry
 * in the TCAM to do an update of its related information.
 * Update could be for key portion, data portion or priority
 */
int
bcmptm_rm_tcam_prio_entry_update(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 uint32_t found_index)
{

    uint8_t update_type = 0;
    int rv = SHR_E_NONE;
    uint8_t mode = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /*Identify if its a Prio change */
    if (req_info->entry_pri != rsp_info->rsp_entry_pri) {
        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            SHR_RETURN_VAL_EXIT(rm_tcam_prio_only_entry_update_prio(unit,
                                              req_flags, ltid, ltid_info,
                                              pt_dyn_info,
                                              req_info, rsp_info,
                                              rsp_ltid, rsp_flags,
                                              found_index));

        } else {
            SHR_RETURN_VAL_EXIT(rm_tcam_prio_entry_update_prio(unit,
                                              req_flags, ltid, ltid_info,
                                              pt_dyn_info,
                                              req_info, rsp_info,
                                              rsp_ltid, rsp_flags,
                                              found_index));
        }
    }

    /* Identify if its a key or data change */
    rv = (bcmptm_rm_tcam_update_type_get(unit,
                                  req_flags, ltid, ltid_info,
                                  pt_dyn_info,
                                  req_info, rsp_info,
                                  rsp_ltid, rsp_flags,
                                  found_index, &update_type));
    if (rv == SHR_E_EXISTS) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if ((ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY)
        && (!(update_type & BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE))) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)req_info->entry_attrs;
        mode = entry_attrs->group_mode;
    } else {
        mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    /* Data only update only when data_only SID is available */
    if ((update_type & BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
        bcmdrd_feature_is_real_hw(unit) &&
        (ltid_info->hw_entry_info[mode].sid_data_only != NULL)) {
        SHR_RETURN_VAL_EXIT(bcmptm_rm_tcam_entry_update_data_only(unit,
                                          req_flags, ltid, ltid_info,
                                          pt_dyn_info,
                                          req_info, rsp_info,
                                          rsp_ltid, rsp_flags,
                                          found_index));
    } else {
        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            SHR_RETURN_VAL_EXIT(rm_tcam_prio_only_entry_update(unit,
                                              req_flags, ltid, ltid_info,
                                              pt_dyn_info,
                                              req_info, rsp_info,
                                              rsp_ltid, rsp_flags,
                                              update_type,
                                              found_index));
        } else {
            SHR_RETURN_VAL_EXIT(rm_tcam_prio_entry_update(unit,
                                              req_flags, ltid, ltid_info,
                                              pt_dyn_info,
                                              req_info, rsp_info,
                                              rsp_ltid, rsp_flags,
                                              update_type,
                                              found_index));
        }
    }

exit:

    /* Log the function exit. */
    SHR_FUNC_EXIT();

}

