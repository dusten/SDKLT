/*! \file rm_hash_traverse.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement traversal support for hash
 * table.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "rm_hash_internal.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Private variables
 */

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Compare the key portion of two entries.
 *
 * This function extracts and compares the key fields from the entry buffer.
 *
 * \param [in] ent_words_a Entry a to be compared.
 * \param [in] ent_words_b Entry b to be compared.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure
 *             corresponding to a LT.
 *
 * \retval 1  The former is larger than the later.
 * \retval -1 The former is smaller than the later.
 * \retval 0  The former is equal to the later.
 */
static int
rm_hash_travs_key_compare(const uint32_t *ent_words_a,
                          const uint32_t *ent_words_b,
                          const bcmptm_rm_hash_more_info_t *more_info)
{
    uint8_t idx;
    int sbit, ebit;
    uint32_t key_field_a[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t key_field_b[BCMPTM_MAX_PT_FIELD_WORDS];
    const bcmlrd_hw_field_list_t *key_fields = more_info->hw_key_fields;

    for (idx = 0; idx < key_fields->num_fields; idx++) {
        uint16_t num_words, word;
        sal_memset(key_field_a, 0, sizeof(key_field_a));
        sal_memset(key_field_b, 0, sizeof(key_field_b));
        sbit = key_fields->field_start_bit[idx];
        ebit = key_fields->field_start_bit[idx] + key_fields->field_width[idx] - 1;
        bcmdrd_field_get(ent_words_a, sbit, ebit, key_field_a);
        bcmdrd_field_get(ent_words_b, sbit, ebit, key_field_b);
        num_words = (key_fields->field_width[idx] + 31) / 32;
        for (word = 0; word < num_words; word++) {
            if (key_field_a[word] > key_field_b[word]) {
                return 1;
            }
            if (key_field_a[word] < key_field_b[word]) {
                return -1;
            }
        }
    }
    return 0;
}

/*!
 * \brief Get the first element of sorted entry snapshot list.
 *
 * This function get the first element of sorted entry snapshot list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 * \param [in] tbl_snap_idx Table instance.
 * \param [in] ent_snap_base Buffer to record the first elem of sorted snap list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_sorted_ent_snap_base_get(int unit,
                                       rm_hash_lt_travs_info_t *travs_info,
                                       int tbl_snap_idx,
                                       rm_hash_ent_snap_t ***ent_snap_base)
{
    rm_hash_tbl_snap_t *tbl_snap = NULL;

    SHR_FUNC_ENTER(unit);

    tbl_snap = travs_info->tbl_snap_array[tbl_snap_idx];
    *ent_snap_base = tbl_snap->sorted_ent_snap_base;

    SHR_FUNC_EXIT();
}

/*!
 *\brief Get the entry snapshot pointer for the given entry key.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 * \param [in] tbl_snap_idx Table instance for a hash LT.
 * \param [in] ent_words Pointer to buffer containing entry key.
 * \param [out] ent_snap_ptr Buffer recording the entry snapshot pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_sorted_ent_snap_ptr_get(int unit,
                                      rm_hash_lt_ctrl_t *lt_ctrl,
                                      const bcmptm_rm_hash_more_info_t *more_info,
                                      int tbl_snap_idx,
                                      uint32_t *ent_words,
                                      rm_hash_ent_snap_t ***ent_snap_ptr)
{
    uint32_t start = 0, end = 0, idx = 0, mid = 0;
    rm_hash_lt_travs_info_t *travs_info = lt_ctrl->travs_info;
    rm_hash_tbl_snap_t *tbl_snap = NULL;
    rm_hash_ent_snap_t **ent_snap_base = NULL, **curr = NULL;
    int cmp_result;
    uint32_t num_ent_snap = 0;

    SHR_FUNC_ENTER(unit);

    tbl_snap = travs_info->tbl_snap_array[tbl_snap_idx];
    num_ent_snap = tbl_snap->ent_snap_cnt;
    SHR_IF_ERR_EXIT
        (rm_hash_travs_sorted_ent_snap_base_get(unit,
                                                lt_ctrl->travs_info,
                                                tbl_snap_idx,
                                                &ent_snap_base));
    end = num_ent_snap - 1;
    while (idx < num_ent_snap) {
        mid = (start + end) / 2;
        curr = ent_snap_base + mid;
        cmp_result = rm_hash_travs_key_compare(ent_words,
                                               (*curr)->ent_words,
                                               more_info);
        if (cmp_result == 0) {
            *ent_snap_ptr = curr;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else if (cmp_result < 0) {
            end = mid - 1;
            if ((mid == 0) || (end < start)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
        } else {
            start = mid + 1;
            if ((mid == num_ent_snap) || (start > end)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
        }
        idx++;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free memory space for the snapshot of a LT.
 *
 * This function takes snapshot for the specified LT for the following
 * GET_NEXT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 * \param [in] tbl_snap_idx Table instance snapshot index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_snapshot_free(int unit,
                            rm_hash_lt_ctrl_t *lt_ctrl,
                            int tbl_snap_idx)
{
    int idx;
    rm_hash_lt_travs_info_t *travs_info = lt_ctrl->travs_info;

    SHR_FUNC_ENTER(unit);

    if (lt_ctrl->lt_info.rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    if (travs_info != NULL) {
        if (travs_info->tbl_snap_array[tbl_snap_idx] != NULL) {
            sal_free(travs_info->tbl_snap_array[tbl_snap_idx]);
            travs_info->tbl_snap_array[tbl_snap_idx] = NULL;
        }
        for (idx = 0; idx < travs_info->tbl_snap_cnt; idx++) {
            if (travs_info->tbl_snap_array[idx] != NULL) {
                break;
            }
        }
        if (idx == travs_info->tbl_snap_cnt) {
            /*
             * All the table instance snapshots for this LT has been released.
             * Release traverse info for this LT.
             */
            sal_free(lt_ctrl->travs_info);
            lt_ctrl->travs_info = NULL;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate memory space for the snapshot of a LT.
 *
 * This function takes snapshot for the specified LT for the following
 * GET_NEXT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 * \param [in] tbl_snap_idx Table snapshot instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_snapshot_alloc(int unit,
                             rm_hash_lt_ctrl_t *lt_ctrl,
                             rm_hash_grp_info_t *grp_info,
                             int tbl_snap_idx)
{
    uint8_t idx;
    const bcmptm_rm_hash_hw_entry_info_t *ent_info = NULL, *nent_info = NULL;
    bcmdrd_sid_t sid;
    int tbl_inst_cnt = 0;
    uint32_t ent_idx_cnt, travs_info_size;
    uint32_t tbl_snap_size;
    rm_hash_lt_travs_info_t *travs_info = NULL;
    rm_hash_tbl_snap_t * tbl_snap = NULL;
    int index_min, index_max;

    SHR_FUNC_ENTER(unit);

    if (lt_ctrl->lt_info.rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    nent_info = lt_ctrl->lt_info.hw_entry_info + grp_info->vhei_idx[0];
    /*
     * Try to find the narrowest memory view, as it has the maximum
     * entry count.
     */
    for (idx = 1; idx < grp_info->num_vhei; idx++) {
        ent_info = lt_ctrl->lt_info.hw_entry_info + grp_info->vhei_idx[idx];
        if (ent_info->num_base_entries < nent_info->num_base_entries) {
            nent_info = ent_info;
        }
    }
    sid = nent_info->sid[0];
    if (lt_ctrl->travs_info == NULL) {
        tbl_inst_cnt = bcmdrd_pt_num_tbl_inst(unit, sid);
        if (tbl_inst_cnt < 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        travs_info_size = sizeof(rm_hash_tbl_snap_t *) * tbl_inst_cnt +
                          sizeof(rm_hash_lt_travs_info_t);
        travs_info = sal_alloc(travs_info_size, "LT traverse info");
        if (travs_info == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
        sal_memset(travs_info, 0, travs_info_size);
        lt_ctrl->travs_info = travs_info;
        lt_ctrl->travs_info->tbl_snap_cnt = tbl_inst_cnt;
    }
    index_min = bcmdrd_pt_index_min(unit, sid);
    index_max = bcmdrd_pt_index_max(unit, sid);
    if ((index_min) < 0 || (index_max < 0) || (index_min > index_max)) {
        ent_idx_cnt = 0;
    } else {
        ent_idx_cnt = (1 + index_max - index_min);
    }
    
    tbl_snap_size = (sizeof(rm_hash_ent_snap_t) + sizeof(rm_hash_ent_snap_t *)) *
                    ent_idx_cnt + sizeof(rm_hash_tbl_snap_t);
    tbl_snap = sal_alloc(tbl_snap_size, "Table instance snapshot");
    if (tbl_snap == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    sal_memset(tbl_snap, 0, tbl_snap_size);
    lt_ctrl->travs_info->tbl_snap_array[tbl_snap_idx] = tbl_snap;
    tbl_snap->ent_snap_max = ent_idx_cnt;
    tbl_snap->sorted_ent_snap_base = (rm_hash_ent_snap_t **)(tbl_snap->ent_snap_array + ent_idx_cnt);

exit:
    if (SHR_FUNC_ERR()) {
        (void)rm_hash_travs_snapshot_free(unit, lt_ctrl, tbl_snap_idx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an entry snapshot into the lt snapshot structure
 *
 * This function get the first element of sorted entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 * \param [in] ent_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] tbl_inst Table instance.
 * \param [in] ent_words Entry key words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_ent_snap_insert(int unit,
                              rm_hash_lt_ctrl_t *lt_ctrl,
                              const bcmptm_rm_hash_hw_entry_info_t *ent_info,
                              int tbl_snap_idx,
                              uint32_t *ent_words)
{
    rm_hash_lt_travs_info_t *travs_info = NULL;
    rm_hash_tbl_snap_t *tbl_snap = NULL;
    rm_hash_ent_snap_t *ent_snap = NULL;
    uint32_t *ent_snap_words = NULL, idx, num_move, k;
    rm_hash_ent_snap_t **ent_snap_base = NULL;

    SHR_FUNC_ENTER(unit);

    travs_info = lt_ctrl->travs_info;
    tbl_snap = travs_info->tbl_snap_array[tbl_snap_idx];
    ent_snap = &tbl_snap->ent_snap_array[tbl_snap->ent_snap_cnt];
    ent_snap_words = ent_snap->ent_words;
    sal_memcpy(ent_snap_words, ent_words, (ent_info->entry_bitsize + 7) / 8);
    SHR_IF_ERR_EXIT
        (rm_hash_travs_sorted_ent_snap_base_get(unit,
                                                lt_ctrl->travs_info,
                                                tbl_snap_idx,
                                                &ent_snap_base));
    for (idx = 0; idx < tbl_snap->ent_snap_cnt; idx++) {
        if (rm_hash_travs_key_compare(ent_snap->ent_words,
                                      ent_snap_base[idx]->ent_words,
                                      lt_ctrl->lt_info.rm_more_info) < 0) {
            break;
        }
    }
    num_move = tbl_snap->ent_snap_cnt - idx;
    for (k = tbl_snap->ent_snap_cnt - 1; num_move > 0; k--, num_move--) {
        ent_snap_base[k + 1] = ent_snap_base[k];
    }
    ent_snap_base[idx] = ent_snap;
    tbl_snap->ent_snap_cnt++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Take the snapshot of a LT.
 *
 * This function takes snapshot for the specified LT for the following
 * GET_NEXT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure corresponding
 *             to a LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_snapshot_build(int unit,
                             rm_hash_lt_ctrl_t *lt_ctrl,
                             rm_hash_grp_info_t *grp_info,
                             bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    uint8_t idx;
    const bcmptm_rm_hash_hw_entry_info_t *ent_info = NULL;
    bcmdrd_sid_t sid;
    int index_min, index_max, ent_index;
    int tbl_inst_cnt = 0, tbl_snap_idx;
    uint32_t entry[BCMPTM_MAX_PT_ENTRY_WORDS];
    rm_hash_lt_travs_info_t *travs_info = lt_ctrl->travs_info;

    SHR_FUNC_ENTER(unit);

    if (lt_ctrl->lt_info.rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    if (travs_info == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    for (idx = 0; idx < grp_info->num_vhei; idx++) {
        ent_info = lt_ctrl->lt_info.hw_entry_info + grp_info->vhei_idx[idx];
        sid = ent_info->sid[0];
        index_min = bcmdrd_pt_index_min(unit, sid);
        index_max = bcmdrd_pt_index_max(unit, sid);
        tbl_inst_cnt = bcmdrd_pt_num_tbl_inst(unit, sid);
        if (tbl_inst_cnt < 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else if (tbl_inst_cnt == 1) {
            tbl_snap_idx = 0;
        } else {
            tbl_snap_idx = pt_dyn_info->tbl_inst;
        }
        if (travs_info->tbl_snap_array[tbl_snap_idx] == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        for (ent_index = index_min; ent_index <= index_max; ent_index++) {
            bool cache_valid = FALSE;
            bcmltd_sid_t ltid;
            uint16_t dfield_format_id;
            uint32_t rsp_flags;
            bcmbd_pt_dyn_info_t dyn_info = *pt_dyn_info;
            dyn_info.index = ent_index;
            sal_memset(entry, 0, sizeof(entry));
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_read(unit,
                                     BCMPTM_REQ_FLAGS_NO_FLAGS,
                                     0,
                                     sid,
                                     &dyn_info,
                                     NULL,
                                     NULL, /* misc_info */
                                     BCMPTM_MAX_PT_ENTRY_WORDS,
                                     entry,
                                     &cache_valid,
                                     &ltid,
                                     &dfield_format_id,
                                     &rsp_flags));
            if ((cache_valid == TRUE) && (ltid == lt_ctrl->ltid)) {
                SHR_IF_ERR_EXIT
                    (rm_hash_travs_ent_snap_insert(unit,
                                                   lt_ctrl,
                                                   ent_info,
                                                   tbl_snap_idx,
                                                   entry));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Process GET_FIRST opcode.
 *
 * This function handle GET_FIRST opcode
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 *
 *
 * \retval TRUE LT is eligible for traverse operation
 * \retval FALSE LT is not eligible for traverse operation.
 */
static int
rm_hash_travs_get_first_op_proc(int unit,
                                uint32_t req_flags,
                                bcmltd_sid_t req_ltid,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_hash_req_t *req_info,
                                size_t rsp_entry_buf_wsize,
                                uint32_t *rsp_entry_words,
                                bcmptm_rm_hash_rsp_t *rsp_info,
                                bcmltd_sid_t *rsp_ltid,
                                uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    const bcmptm_rm_hash_hw_entry_info_t *ent_info = NULL;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;
    bcmdrd_sid_t sid, grp_sid;
    int tbl_inst_cnt;
    int tbl_snap_idx;
    rm_hash_ent_snap_t **ent_snap_base = NULL;
    rm_hash_ent_snap_t *ent_snap = NULL;
    rm_hash_tbl_snap_t *tbl_inst_snap = NULL;
    rm_hash_grp_info_t *grp_info = NULL;
    bcmptm_rm_hash_req_t tmp_req_info;
    uint8_t hei_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {0};
    uint8_t hei_cnt = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);
    if (lt_ctrl->lt_info.rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /* Get more info based on req_info */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_more_info_get(unit,
                                          req_info,
                                          lt_ctrl,
                                          &more_info));
    grp_info = &lt_ctrl->grp_info[0];

    /* Get software logical bucket list and valid base bucket bitmap */
    if (lt_ctrl->num_grp > 1) {
        /* This LT mapps to multiple physical SIDs */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_group_sid_get(unit,
                                          &lt_ctrl->lt_info,
                                          more_info,
                                          &grp_sid));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_grp_info_get(unit,
                                                 grp_sid,
                                                 lt_ctrl,
                                                 &grp_info));
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_given_key_ent_info_get(unit,
                                               &lt_ctrl->lt_info,
                                               more_info,
                                               &hei_cnt,
                                               hei_list));

    ent_info = lt_ctrl->lt_info.hw_entry_info + grp_info->vhei_idx[0];
    sid = ent_info->sid[0];
    tbl_inst_cnt = bcmdrd_pt_num_tbl_inst(unit, sid);
    if (tbl_inst_cnt < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    } else if (tbl_inst_cnt == 1) {
        tbl_snap_idx = 0;
    } else {
        tbl_snap_idx = pt_dyn_info->tbl_inst;
    }
    /*
     * Examine if this is used for terminating the previous traversal
     * prematurely.
     */
    if ((lt_ctrl->travs_info != NULL) &&
        (lt_ctrl->travs_info->tbl_snap_array[tbl_snap_idx] != NULL)) {
        SHR_IF_ERR_EXIT
            (rm_hash_travs_snapshot_free(unit, lt_ctrl, tbl_snap_idx));
    }
    /*
     * The snapshot for the specified table instance needs to
     * be allocated and built.
     */
    SHR_IF_ERR_EXIT
        (rm_hash_travs_snapshot_alloc(unit,
                                      lt_ctrl,
                                      grp_info,
                                      tbl_snap_idx));
    SHR_IF_ERR_EXIT
        (rm_hash_travs_snapshot_build(unit, lt_ctrl, grp_info, pt_dyn_info));
    tbl_inst_snap = lt_ctrl->travs_info->tbl_snap_array[tbl_snap_idx];
    if (tbl_inst_snap->ent_snap_cnt == 0) {
        /* There is no valid entry for this LT. */
        SHR_IF_ERR_EXIT
            (rm_hash_travs_snapshot_free(unit, lt_ctrl, tbl_snap_idx));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    /* Retrieve the first entry snapshot (smallest entry) address */
    SHR_IF_ERR_EXIT
        (rm_hash_travs_sorted_ent_snap_base_get(unit,
                                                lt_ctrl->travs_info,
                                                tbl_snap_idx,
                                                &ent_snap_base));
    ent_snap = *ent_snap_base;
    /*
     * Lookup the first entry snapshot in the PTcache for data portion.
     * Stuff the req_info before invoking lookup operation.
     */
    sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
    tmp_req_info.entry_words = ent_snap->ent_words;
    tmp_req_info.same_key = FALSE;
    tmp_req_info.entry_attrs = req_info->entry_attrs;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lookup(unit,
                               req_flags,
                               req_ltid,
                               pt_dyn_info,
                               &tmp_req_info,
                               rsp_entry_buf_wsize,
                               rsp_entry_words,
                               rsp_info,
                               rsp_ltid,
                               rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Process GET_NEXT opcode.
 *
 * This function handle GET_NEXT opcode
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] cseq_id Current sequence id.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_travs_get_next_op_proc(int unit,
                               uint32_t req_flags,
                               bcmltd_sid_t req_ltid,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_rm_hash_req_t *req_info,
                               size_t rsp_entry_buf_wsize,
                               uint32_t *rsp_entry_words,
                               bcmptm_rm_hash_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    const bcmptm_rm_hash_hw_entry_info_t *ent_info = NULL;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;
    bcmdrd_sid_t sid, grp_sid;
    int tbl_inst_cnt;
    int tbl_snap_idx;
    int tmp_rv;
    rm_hash_ent_snap_t **ent_snap_ptr = NULL, **ent_snap_base = NULL;
    rm_hash_tbl_snap_t *tbl_snap = NULL;
    bcmptm_rm_hash_req_t tmp_req_info;
    rm_hash_ent_snap_t *ent_snap = NULL;
    rm_hash_grp_info_t *grp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);
    if (lt_ctrl->lt_info.rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /* Get more info based on req_info */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_more_info_get(unit,
                                          req_info,
                                          lt_ctrl,
                                          &more_info));
    grp_info = &lt_ctrl->grp_info[0];

    /* Get software logical bucket list and valid base bucket bitmap */
    if (lt_ctrl->num_grp > 1) {
        /* This LT mapps to multiple physical SIDs */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_group_sid_get(unit,
                                          &lt_ctrl->lt_info,
                                          more_info,
                                          &grp_sid));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_grp_info_get(unit,
                                                 grp_sid,
                                                 lt_ctrl,
                                                 &grp_info));
    }

    ent_info = lt_ctrl->lt_info.hw_entry_info + grp_info->vhei_idx[0];
    sid = ent_info->sid[0];
    tbl_inst_cnt = bcmdrd_pt_num_tbl_inst(unit, sid);
    if (tbl_inst_cnt < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    } else if (tbl_inst_cnt == 1) {
        tbl_snap_idx = 0;
    } else {
        tbl_snap_idx = pt_dyn_info->tbl_inst;
    }
    if ((lt_ctrl->travs_info == NULL) ||
        (lt_ctrl->travs_info->tbl_snap_array[tbl_snap_idx] == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /*
     * User must specify the "curr" entry key for GET_NEXT operation.
     * Search the key in the snapshot.
     */
    tmp_rv = rm_hash_travs_sorted_ent_snap_ptr_get(unit,
                                                   lt_ctrl,
                                                   more_info,
                                                   tbl_snap_idx,
                                                   req_info->entry_words,
                                                   &ent_snap_ptr);
    if (tmp_rv == SHR_E_NOT_FOUND) {
        /*
         * User specifies a non-existing entry key,
         * snapshot will be released in this case.
         */
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "A non-existing entry was specified...\n")));
        SHR_IF_ERR_EXIT
            (rm_hash_travs_snapshot_free(unit, lt_ctrl, tbl_snap_idx));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_IF_ERR_EXIT(tmp_rv);
    SHR_IF_ERR_EXIT
        (rm_hash_travs_sorted_ent_snap_base_get(unit,
                                                lt_ctrl->travs_info,
                                                tbl_snap_idx,
                                                &ent_snap_base));
    tbl_snap = lt_ctrl->travs_info->tbl_snap_array[tbl_snap_idx];
    if ((ent_snap_ptr - ent_snap_base) >= ((int)tbl_snap->ent_snap_cnt - 1)) {
        /*
         * The "curr" entry snap is already the last entry snapshot,
         * the snapshot will be released in this case.
         */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "curr key is the last...\n")));
        SHR_IF_ERR_EXIT
            (rm_hash_travs_snapshot_free(unit, lt_ctrl, tbl_snap_idx));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    ent_snap_ptr++;
    for (; (ent_snap_ptr - ent_snap_base) < (int)tbl_snap->ent_snap_cnt; ent_snap_ptr++) {
        ent_snap = *ent_snap_ptr;
        /*
         * Presently, when we get GET_NEXT op, traverse will check for validity
         * of each next entry till it finds a successful valid entry.
         * For example, after the GET_FIRST, if we had deleted ALL entries except
         * for very last - we will issue N lookups and then realize that only the
         * last is valid entry.
         *
         * Another possibility was to do following: With every DELETE, we search
         * for entry in snapshot and mark valid=0 and then during the GET_NEXT
         * we simply walk and check valid bits in snapshot.
         *
         * 2nd approach will slow down the DELETE, but will speed up GET_NEXT.
         * 1st approach will slow down only the GET_NEXT.
         *
         * The choice for the 1st approach is based on the following assumption:
         * The delete op will be a higher priority operation than traverse op,
         * so the delete should be implemented in an efficient way.
         * Another benefit of 1st approach is that it will decouple the implementation
         * of delete and traverse. In another word, delete will be independent with
         * traverse and vice verse.
         */
        sal_memset(&tmp_req_info, 0, sizeof(tmp_req_info));
        tmp_req_info.entry_words = ent_snap->ent_words;
        tmp_req_info.same_key = FALSE;
        tmp_req_info.entry_attrs = req_info->entry_attrs;
        tmp_rv = bcmptm_rm_hash_lookup(unit,
                                       req_flags,
                                       req_ltid,
                                       pt_dyn_info,
                                       &tmp_req_info,
                                       rsp_entry_buf_wsize,
                                       rsp_entry_words,
                                       rsp_info,
                                       rsp_ltid,
                                       rsp_flags);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            /*
             * The entry has been deleted after the snapshot is created.
             * This is an expected behavior, and the next element will be
             * retrieved and evaluated.
             */
            continue;
        } else {
            SHR_RETURN_VAL_EXIT(tmp_rv);
        }
    }
    if ((ent_snap_ptr - ent_snap_base) >= (int)tbl_snap->ent_snap_cnt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "There is no valid entry...\n")));
        SHR_IF_ERR_EXIT
            (rm_hash_travs_snapshot_free(unit, lt_ctrl, tbl_snap_idx));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int bcmptm_rm_hash_travs_cmd_proc(int unit,
                                  uint32_t req_flags,
                                  uint32_t cseq_id,
                                  bcmltd_sid_t req_ltid,
                                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                                  bcmptm_op_type_t req_op,
                                  bcmptm_rm_hash_req_t *req_info,
                                  size_t rsp_entry_buf_wsize,
                                  uint32_t *rsp_entry_words,
                                  bcmptm_rm_hash_rsp_t *rsp_info,
                                  bcmltd_sid_t *rsp_ltid,
                                  uint32_t *rsp_flags)
{
    int tmp_rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info->entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);

    switch (req_op){
    case BCMPTM_OP_GET_FIRST:
        tmp_rv = rm_hash_travs_get_first_op_proc(unit,
                                                 req_flags,
                                                 req_ltid,
                                                 pt_dyn_info,
                                                 req_info,
                                                 rsp_entry_buf_wsize,
                                                 rsp_entry_words,
                                                 rsp_info,
                                                 rsp_ltid,
                                                 rsp_flags);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            SHR_RETURN_VAL_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_GET_NEXT:
        tmp_rv = rm_hash_travs_get_next_op_proc(unit,
                                                req_flags,
                                                req_ltid,
                                                pt_dyn_info,
                                                req_info,
                                                rsp_entry_buf_wsize,
                                                rsp_entry_words,
                                                rsp_info,
                                                rsp_ltid,
                                                rsp_flags);
        if ((tmp_rv == SHR_E_NOT_FOUND) || (tmp_rv == SHR_E_UNAVAIL)) {
            SHR_RETURN_VAL_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}
