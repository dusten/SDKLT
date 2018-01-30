/*! \file rm_tcam_fp_entry_mgmt.c
 *
 * Low Level Functions for FP TCAM entry management
 * This file contains low level functions for FP based TCAMs
 * to do operations like TCAM entry Insert/Lookup/Delete/Move.
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
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_fp.h"
#include "rm_tcam_fp_entry_mgmt.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private Functions
 */
static int
bcmptm_rm_tcam_fp_presel_entries_copy(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint32_t req_flags,
                                 uint8_t new_slice_id,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 void *attrs)
{
    bcmltd_sid_t rsp_ltid = -1;
    int16_t from_idx = -1;
    int16_t to_idx = -1;
    uint16_t idx = 0;
    uint32_t num_presel_per_slice;
    int8_t pri_slice_id = -1;
    uint8_t part = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    uint32_t rsp_flags = 0;
    uint32_t rsp_entry_sid = 0;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices = 0;
    uint32_t entry_count = 0;
    uint8_t slice_id = 0;
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    bcmptm_rm_tcam_req_t req_info;
    bcmptm_rm_tcam_rsp_t rsp_info;
    uint32_t *entry_key_words = NULL;
    uint32_t *entry_data_words = NULL;
    uint32_t *ekw_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    uint32_t *edw_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    bcmptm_rm_tcam_lt_info_t presel_ltid_info;
    bcmltd_sid_t  presel_ltid;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    /* make a copy of the entry_attrs structure*/
    SHR_ALLOC(entry_attrs, sizeof(bcmptm_rm_tcam_entry_attrs_t),
                      "Entry attrs copy");

    sal_memcpy(entry_attrs, attrs,
                           sizeof(bcmptm_rm_tcam_entry_attrs_t));

    /* set the presel entry flag */
    SHR_BITSET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY);

    /* Fetch the TCAM entry information for the givem LTID. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                  (void *)entry_attrs,
                                  &entry_info));
    /* get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                   unit, ltid, ltid_info, entry_attrs->pipe_id,
                                   new_slice_id,
                                   &num_presel_per_slice));
    /* get the group pointer */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attrs->group_id,
                                   entry_attrs->stage_id,
                                   &fg));
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attrs->pipe_id,
                                   entry_attrs->stage_id,
                                   &slice_fc));

    /* get the primary slice index */
    pri_slice_id = fg->primary_slice_id;

    slice_id = pri_slice_id;
    while (slice_id < num_slices
                  && (0 != slice_fc[slice_id].next_slice_id_offset)) {
        entry_count += num_presel_per_slice;
        slice_id += slice_fc[slice_id].next_slice_id_offset;
        if (slice_id == new_slice_id) {
            break;
        }
    }

    if (slice_id != new_slice_id) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(entry_key_words,
              (sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS),
              "Entry key words");
    SHR_ALLOC(entry_data_words,
              (sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS),
              "Entry Data words");
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
        ekw_row[part] = entry_key_words + (part * BCMPTM_MAX_PT_FIELD_WORDS);
        edw_row[part] = entry_data_words + (part * BCMPTM_MAX_PT_FIELD_WORDS);
    }
    req_info.entry_attrs = (void *)entry_attrs;
    req_info.rsp_ekw_buf_wsize = entry_words_size;
    req_info.rsp_edw_buf_wsize = entry_words_size;
    req_info.ekw = ekw_row;
    req_info.edw = edw_row;
    rsp_info.rsp_ekw = ekw_row;
    rsp_info.rsp_edw = edw_row;
    if (ltid_info->rm_more_info->shared_lt_count) {
        presel_ltid = ltid_info->rm_more_info->shared_lt_info[0];
        sal_memset(&presel_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_cmdproc_ptrm_info_get(unit, presel_ltid, 0, 0,
                                         ((void *)&presel_ltid_info),
                                         NULL, NULL));
    } else {
        presel_ltid = ltid;
        presel_ltid_info = *ltid_info;
    }

    /* loop for all presel entries per slice and
     * copy the non-empty entries from primary
     * slice to the newly expanded slice. copy for the
     * width parts of the TCAM are taken care by hw
     * read/write functions
     */
    for (idx = 0; idx < num_presel_per_slice; idx++) {

        /* calculate the from and to indexs */
        from_idx = idx;
        to_idx = entry_count + idx;

        /* If the entry is empty skip it. */
        if (entry_info[from_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
            continue;
        }

        sal_memset(entry_key_words, 0,
                   sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
                   BCMPTM_MAX_PT_FIELD_WORDS);
        sal_memset(entry_data_words, 0,
                   sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
                   BCMPTM_MAX_PT_FIELD_WORDS);

        /* Read the entry words from from_idx. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_entry_read(unit,
                                           presel_ltid, &presel_ltid_info,
                                           req_flags, from_idx,
                                           pt_dyn_info,
                                           &req_info, &rsp_info,
                                           &rsp_ltid, &rsp_flags));

        /* Write the entry words to to_idx. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_entry_write(unit,
                                            presel_ltid, &presel_ltid_info,
                                            req_flags, 1,
                                            to_idx,
                                            pt_dyn_info, &req_info,
                                            &rsp_ltid, &rsp_flags,
                                            &rsp_entry_sid));
    }

exit:
    SHR_FREE(entry_attrs);
    SHR_FREE(entry_key_words);
    SHR_FREE(entry_data_words);
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_presel_entry_insert(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags)
{

    uint16_t idx = 0;
    uint16_t found_idx = 0;
    uint32_t num_presel_per_slice;
    uint16_t pri_index = 0;
    uint32_t found_index = 0;
    uint8_t slice_id = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices;        /* Number of Slices */
    uint8_t update_type = 0;
    int rv = SHR_E_NONE;
    uint8_t found = 0;
    uint32_t entry_count = 0;
    uint8_t mode = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(req_info->entry_attrs);

    mode = attrs->group_mode;

   /* Check if entry already exists or not. */
    if (req_info->same_key == true) {
        /* If already looked up, use the results */
        if (same_key_info[unit].index != -1) {
            found_index = same_key_info[unit].index;
            rv = SHR_E_NONE;
        } else {
            found_index = -1;
            rv = SHR_E_NOT_FOUND;
        }
    } else {
        /* Check if entry id exists or not. */
        rv = bcmptm_rm_tcam_prio_entry_lookup(unit,
                                              req_flags, ltid, ltid_info,
                                              pt_dyn_info,
                                              req_info, rsp_info,
                                              rsp_ltid, rsp_flags,
                                              &found_index);

        if (SHR_FAILURE(rv) && rv != SHR_E_NOT_FOUND) {
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    if (rv == SHR_E_NONE) {
        found = 1;
        if (req_info->entry_pri == rsp_info->rsp_entry_pri) {
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
        }
    }

    /* Insert the primary entry */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_insert(unit,
                                          req_flags, ltid, ltid_info,
                                          pt_dyn_info, req_info,
                                          rsp_info, rsp_ltid,
                                          rsp_flags));
    /* get the group pointer */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   attrs->group_id,
                                   attrs->stage_id,
                                   &fg));

    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, attrs->pipe_id,
                                   attrs->stage_id,
                                   &slice_fc));

    /* get the primary slice index */
    slice_id = fg->primary_slice_id;
    /* get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                   unit, ltid, ltid_info, attrs->pipe_id,
                                   slice_id,
                                   &num_presel_per_slice));

    pri_index = same_key_info[unit].index;

    while (slice_id < num_slices
                  && (0 != slice_fc[slice_id].next_slice_id_offset)) {

        entry_count += num_presel_per_slice;
         /* go to the next slice if exists */
         slice_id = slice_id +
                     slice_fc[slice_id].next_slice_id_offset;

         /* get the number of presel entries in that slice */
         SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                       unit, ltid, ltid_info, attrs->pipe_id,
                                       slice_id,
                                       &num_presel_per_slice));

         idx = entry_count + pri_index;
         found_idx = entry_count + found_index;

         /* Data only update */
         if ((update_type & BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
             bcmdrd_feature_is_real_hw(unit) &&
             (ltid_info->hw_entry_info[mode].sid_data_only != NULL)) {
             SHR_RETURN_VAL_EXIT(bcmptm_rm_tcam_entry_update_data_only(unit,
                                               req_flags, ltid, ltid_info,
                                               pt_dyn_info,
                                               req_info, rsp_info,
                                               rsp_ltid, rsp_flags,
                                               found_idx));
         } else {

             /* Write the entry words to HW */
             SHR_IF_ERR_VERBOSE_EXIT(
                 bcmptm_rm_tcam_prio_entry_write(unit, ltid, ltid_info,
                                                 req_flags, 1,
                                                 idx,
                                                 pt_dyn_info, req_info,
                                                 rsp_ltid, rsp_flags,
                                                 rsp_info->rsp_entry_sid));
             /* Key or prio update */
             if (found) {
                /* Clear the data in HW */
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_rm_tcam_prio_entry_clear(unit, ltid, ltid_info,
                                                    req_flags,
                                                    BCMPTM_RM_TCAM_ENTRY_OP_HW_ONLY,
                                                    found_idx,
                                                    pt_dyn_info, req_info,
                                                    rsp_ltid, rsp_flags,
                                                    rsp_info->rsp_entry_sid));
            }
        }
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_presel_entry_delete(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags)
{

    uint16_t idx = 0;
    uint32_t num_presel_per_slice;
    uint16_t pri_index = 0;
    uint8_t slice_id = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices;        /* Number of Slices */
    uint32_t entry_count = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(req_info->entry_attrs);

    /* Delete the primary entry */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_delete(unit,
                                          req_flags, ltid, ltid_info,
                                          pt_dyn_info, req_info,
                                          rsp_info, rsp_ltid,
                                          rsp_flags));
    /* get the group pointer */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   attrs->group_id,
                                   attrs->stage_id,
                                   &fg));

    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, attrs->pipe_id,
                                   attrs->stage_id,
                                   &slice_fc));

    /* get the primary slice index */
    slice_id = fg->primary_slice_id;
    /* get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                   unit, ltid, ltid_info, attrs->pipe_id,
                                   slice_id,
                                   &num_presel_per_slice));

    pri_index = same_key_info[unit].index;

    while (slice_id < num_slices
                  && (0 != slice_fc[slice_id].next_slice_id_offset)) {

         entry_count += num_presel_per_slice;
         /* go to the next slice if exists */
         slice_id = slice_id +
                     slice_fc[slice_id].next_slice_id_offset;

         /* get the number of presel entries in that slice */
         SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                       unit, ltid, ltid_info, attrs->pipe_id,
                                       slice_id,
                                       &num_presel_per_slice));

         idx = entry_count + pri_index;

         /* Clear the data in HW */
         SHR_IF_ERR_VERBOSE_EXIT(
             bcmptm_rm_tcam_prio_entry_clear(unit, ltid, ltid_info,
                                             req_flags,
                                             BCMPTM_RM_TCAM_ENTRY_OP_HW_ONLY,
                                             idx,
                                             pt_dyn_info, req_info,
                                             rsp_ltid, rsp_flags,
                                             rsp_info->rsp_entry_sid));
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_entry_segment_check(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                int pipe_id,
                                int segment_id,
                                bool *free_segment)
{

    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint32_t idx = 0;
    uint32_t num_entries_per_slice = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    int  *entry_hash = NULL;
    uint32_t num_entries = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    (*free_segment) = 1;

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   stage_id,
                                   &stage_fc));

    num_entries_per_slice = stage_fc->max_entries_per_slice;
    num_entries = (stage_fc->num_slices / stage_fc->num_pipes)
                   * num_entries_per_slice;

    entry_info = (bcmptm_rm_tcam_prio_entry_info_t*)
               (((uint8_t *)stage_fc) + (stage_fc->entry_info_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * segment_id))
               * sizeof(bcmptm_rm_tcam_prio_entry_info_t))));

    entry_hash = (int*) (((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * segment_id))
               * sizeof(int))));

    for (idx = 0; idx < num_entries_per_slice; idx++) {
        if (entry_info[idx].entry_id != BCMPTM_RM_TCAM_PRIO_INVALID) {
            (*free_segment) = 0;
            break;
        }
    }

    if ((*free_segment) == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < num_entries_per_slice; idx++) {
         if(entry_hash[idx] != BCMPTM_RM_TCAM_OFFSET_INVALID) {
            (*free_segment) = 0;
         }
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_fp_entry_segment_clear(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                int pipe_id,
                                int segment_id)
{

    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint32_t idx = 0;
    uint32_t num_entries_per_slice = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    int  *entry_hash = NULL;
    uint32_t num_entries = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   stage_id,
                                   &stage_fc));

    num_entries_per_slice = stage_fc->max_entries_per_slice;
    num_entries = (stage_fc->num_slices / stage_fc->num_pipes)
                   * num_entries_per_slice;

    entry_info = (bcmptm_rm_tcam_prio_entry_info_t*)
               (((uint8_t *)stage_fc) + (stage_fc->entry_info_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * segment_id))
               * sizeof(bcmptm_rm_tcam_prio_entry_info_t))));

    entry_hash = (int*) (((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * segment_id))
               * sizeof(int))));

    for (idx = 0; idx < num_entries_per_slice; idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }

    for (idx = 0; idx < num_entries_per_slice; idx++) {
        entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_fp_entry_segment_move(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                int pipe_id,
                                int from_id, int to_id)
{

    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint32_t num_entries = 0;
    uint32_t num_entries_per_slice = 0;
    uint8_t *from_ptr = NULL;
    uint8_t *to_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   stage_id,
                                   &stage_fc));

    num_entries_per_slice = stage_fc->max_entries_per_slice;
    num_entries = (stage_fc->num_slices / stage_fc->num_pipes)
                   * num_entries_per_slice;

    from_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_info_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * to_id))
               * sizeof(bcmptm_rm_tcam_prio_entry_info_t)));
    to_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_info_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * from_id))
               * sizeof(bcmptm_rm_tcam_prio_entry_info_t)));

    sal_memcpy(from_ptr, to_ptr,
               (num_entries_per_slice
               * sizeof(bcmptm_rm_tcam_prio_entry_info_t)));

    from_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * to_id))
               * sizeof(int)));

    to_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg
               + (((pipe_id * num_entries)
               + (num_entries_per_slice * from_id))
               * sizeof(int)));

    sal_memcpy(from_ptr, to_ptr,
               (num_entries_per_slice
               * sizeof(int)));

exit:
    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_fp_entry_segment_update(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                int pipe_id,
                                bcmptm_rm_tcam_fp_group_t *grp,
                                int segment_id)
{

    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint32_t num_entries = 0;
    uint32_t num_entries_per_slice = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   stage_id,
                                   &stage_fc));

    num_entries_per_slice = stage_fc->max_entries_per_slice;
    num_entries = (stage_fc->num_slices / stage_fc->num_pipes)
                   * num_entries_per_slice;

    grp->entry_info_seg = (stage_fc->entry_info_seg)
                           + (((num_entries_per_slice * segment_id)
                           + (pipe_id * num_entries))
                           * sizeof(bcmptm_rm_tcam_prio_entry_info_t));
    grp->entry_hash_seg = (stage_fc->entry_hash_seg)
                           + (((num_entries_per_slice * segment_id)
                           + (num_entries * pipe_id))
                           * sizeof(int));
exit:
    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_fp_entry_segment_expand(int unit,
                     bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     bcmptm_rm_tcam_entry_attrs_t *entry_attr)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *grp_ptr[32] = {NULL};
                               /* Field Group structure pointer. */

    int primary_segment_id = -1;
    int first_free_segment_id = -1;
    int grp_count = 0;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    uint16_t num_slices = 0;
    int seg_id = -1;
    int new_segment_id = -1;
    uint8_t found = 0;
    bcmptm_fp_grp_seg_bmp_t *group_segments = NULL;
    int pipe_id;
    int idx = 0;
    int grp_id = -1;
    bcmptm_fp_entry_seg_bmp_t *stage_entry_seg_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);

    pipe_id = entry_attr->pipe_id;
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                         entry_attr->stage_id,
                                         &stage_fc));
    num_slices = stage_fc->num_slices / stage_fc->num_pipes;

    SHR_ALLOC(group_segments, (sizeof(bcmptm_fp_grp_seg_bmp_t)
              * num_slices), "FP entry seg");

    stage_entry_seg_bmp = (bcmptm_fp_entry_seg_bmp_t*)
                           (((uint8_t *)stage_fc)
                           + stage_fc->entry_seg_bmp_offset);


    /* Clear group_segment bitmap */
    for (idx = 0; idx < stage_fc->num_groups; idx++) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                  SHR_BITCLR(group_segments[seg_id].w, idx);
             }
    }

    /* Create group segment bitmap */
    fg = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
         if ((fg->valid == 1) && (fg->pipe_id == entry_attr->pipe_id)) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                 if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                     SHR_BITSET(group_segments[seg_id].w,
                                fg->group_id);
                 }
             }
         }
         fg++;
    }

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attr->group_id, entry_attr->stage_id,
                                   &fg));


    /* Find primary segment_id */
    for (seg_id = (num_slices - 1); seg_id >= 0; seg_id--) {
         if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
            break;
         }
    }

    if (seg_id == -1) {
       SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } else {
       primary_segment_id = seg_id;
    }

    if (!(SHR_BITGET(stage_entry_seg_bmp[pipe_id].w,
          primary_segment_id))) {
       SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    new_segment_id = primary_segment_id+1;

    if (!(SHR_BITGET(stage_entry_seg_bmp[pipe_id].w, new_segment_id))) {
       /* Next segment is free to be used
        * Hence no moves required
        */
    } else {
        /* Segment moves are needed */
        /* To check if free segments are available */
        for (seg_id = new_segment_id;
             seg_id < num_slices; seg_id++) {
             if (!(SHR_BITGET(stage_entry_seg_bmp[pipe_id].w,seg_id))) {
                found = 1;
                break;
             }
        }
        if (found == 0) {
            /* No free segments are available.
             * Ideally should not hit this condition.
             */
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        first_free_segment_id = seg_id;

        /* Move all segments from new_segment_id till
           first_free_segment_id */
        seg_id = (first_free_segment_id);
        while (seg_id > new_segment_id) {
            grp_count = 0;
            for (idx = 0; idx < stage_fc->num_groups; idx++) {
                if (SHR_BITGET(group_segments[seg_id - 1].w, idx)) {
                    /* Get the field group pointer for all groups
                       in current segment.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit,
                                           ltid, ltid_info,
                                           idx,
                                           entry_attr->stage_id,
                                           &grp_ptr[grp_count]));
                    grp_count++;
                }
            }

            if (grp_count == 0) {
                /* No groups are available.
                * Ideally should not hit this condition.
                */
               SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }


            /* Move segment from prev to curr */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_segment_move(unit,
                                                     ltid, ltid_info,
                                                     entry_attr->stage_id,
                                                     pipe_id,
                                                     seg_id - 1, seg_id));

            /* Update bitmaps */
            SHR_BITCLR(stage_entry_seg_bmp[pipe_id].w, seg_id - 1);
            SHR_BITSET(stage_entry_seg_bmp[pipe_id].w, seg_id);
            for (grp_id = 0; grp_id < grp_count; grp_id++) {
                SHR_BITCLR(grp_ptr[grp_id]->entry_seg_bmp.w, seg_id - 1);
                SHR_BITSET(grp_ptr[grp_id]->entry_seg_bmp.w, seg_id);
            }

            /* Update the segment pointers in group */
            for (grp_id = 0; grp_id < grp_count; grp_id++) {
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_segment_update(
                                                     unit, ltid, ltid_info,
                                                     entry_attr->stage_id,
                                                     pipe_id,
                                                     grp_ptr[grp_id],
                                                     seg_id));
            }

            seg_id--;
        }

    }

    /* Cleanup new segment */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_segment_clear(unit, ltid, ltid_info,
                                                    entry_attr->stage_id,
                                                    pipe_id,
                                                    new_segment_id));

    grp_count = 0;
    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if (SHR_BITGET(group_segments[primary_segment_id].w, idx)) {
            /* Get the field group pointer for all groups
               in current segment.
             */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit,
                                   ltid, ltid_info,
                                   idx,
                                   entry_attr->stage_id,
                                   &grp_ptr[grp_count]));
            grp_count++;
        }
    }

    /* Update auto expansion data to group and stage */
    SHR_BITSET(stage_entry_seg_bmp[pipe_id].w,new_segment_id);
    for (grp_id = 0; grp_id < grp_count; grp_id++) {
        SHR_BITSET(grp_ptr[grp_id]->entry_seg_bmp.w, new_segment_id);
    }

exit:
    SHR_FREE(group_segments);
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_fp_entry_insert(int unit,
                                   uint32_t req_flags,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                                   bcmptm_rm_tcam_req_t *req_info,
                                   bcmptm_rm_tcam_rsp_t *rsp_info,
                                   bcmltd_sid_t *rsp_ltid,
                                   uint32_t *rsp_flags)
{
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    uint8_t new_slice_id;
    int rv = SHR_E_NONE;
    bool new_group = FALSE;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    if (req_info->entry_attrs != NULL) {
        entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)(req_info->entry_attrs);
    } else {
         SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL) {
        /* Check if entry already exists or not. */
        if (req_info->same_key == true) {
            /* If already looked up, use the results */
            if (same_key_info[unit].index != -1) {
                /* Write the entry words to HW for
                 * all pipes than primary pipe
                 */
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_rm_tcam_prio_entry_write(unit, ltid, ltid_info,
                                              req_flags, 1,
                                              same_key_info[unit].index,
                                              pt_dyn_info, req_info,
                                              rsp_ltid, rsp_flags,
                                              rsp_info->rsp_entry_sid));

                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            } else {
                SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
            }
        }
    }

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                       entry_attr->stage_id,
                                       &stage_fc));
    /* if stage doesnt support presel and trying to insert
     * presel entry, throw config error
     */
    if (!(stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) &&
        SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }
    /* get field group structure */
    rv = bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                          entry_attr->group_id,
                                          entry_attr->stage_id,
                                          &group_ptr);
    if (rv == SHR_E_NOT_FOUND) {
         rv = bcmptm_rm_tcam_fp_slice_allocate(unit, ltid, ltid_info,
                                              entry_attr);
         /* That means this is the all entries in the group are used up,
          * expand the group by allocating slices based on the group mode
          * and priority.
          */
         if (rv == SHR_E_RESOURCE) {
         /* Compress the existing groups to make
          * space for the new one.
          */
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_compress(unit,
                                                  ltid,
                                                  ltid_info,
                                                  req_info->entry_attrs));
             /* Try the slice expand again after compression */
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_allocate(unit,
                                                  ltid,
                                                  ltid_info,
                                                  entry_attr));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        /* get the group pointer */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid,
                                                      ltid_info,
                                                      entry_attr->group_id,
                                                      entry_attr->stage_id,
                                                      &group_ptr));
         new_group = TRUE;

    } else {
        if (group_ptr->group_mode != entry_attr->group_mode ||
            group_ptr->priority != entry_attr->group_prio ||
            group_ptr->group_ltid != entry_attr->group_ltid ||
            group_ptr->pipe_id != entry_attr->pipe_id) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }

    /* get the presel entry flag */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        /* insert presel entry */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entry_insert(unit, req_flags,
                                                ltid, ltid_info,
                                                pt_dyn_info, req_info,
                                                rsp_info, rsp_ltid,
                                                rsp_flags));
        /* Set the default presel entry flag to group */
        if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT)) {
            group_ptr->flags |= BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT;
        }

    } else {
         rv = bcmptm_rm_tcam_prio_entry_insert(unit, req_flags,
                                               ltid, ltid_info,
                                               pt_dyn_info, req_info,
                                               rsp_info, rsp_ltid,
                                               rsp_flags);
         SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_FULL);
         if (rv == SHR_E_FULL &&
             (group_ptr->flags & BCMPTM_RM_TCAM_F_AUTO_EXPANSION)) {

             /* That means this is the all entries in the group are used up,
              * expand the group by allocating slices based on the group mode
              * and priority.
              */
             rv = bcmptm_rm_tcam_fp_slice_expand(unit, ltid, ltid_info,
                                                entry_attr, &new_slice_id);
             SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);

             if (rv == SHR_E_RESOURCE) {
                 /* Compress the existing groups to make
                  * space for the new one.
                  */
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_compress(unit, ltid,
                                                        ltid_info,
                                                        req_info->entry_attrs));
                 /* Try the slice expand again after compression */
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_expand(unit, ltid,
                                                                ltid_info,
                                                                entry_attr,
                                                                &new_slice_id));
             }

             /* Expand entry segments for Info and hash */
             rv = bcmptm_rm_tcam_fp_entry_segment_expand(unit, ltid, ltid_info,
                                                         entry_attr);

             SHR_IF_ERR_VERBOSE_EXIT(rv);

             /* copy all the presel entries from primary
              * slice to newly expanded slice
              */
             if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_presel_entries_copy(unit,
                                                     ltid, ltid_info,
                                                     req_flags,
                                                     new_slice_id, pt_dyn_info,
                                                     req_info->entry_attrs));
             }

             /* Try the entry insert after expanding the group */
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_insert(unit, req_flags,
                                                   ltid, ltid_info,
                                                   pt_dyn_info, req_info,
                                                   rsp_info, rsp_ltid,
                                                   rsp_flags));
         }
    }
    /* For 1st entry insert in Fixed key group update slice id in attrs */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED &&
        new_group == TRUE) {
        entry_attr->slice_id = group_ptr->primary_slice_id;
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_entry_delete(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags)
{
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (req_info->entry_attrs != NULL) {
        entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)(req_info->entry_attrs);
    } else {
         SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    /* get the presel entry flag */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entry_delete(unit,
                                                req_flags, ltid, ltid_info,
                                                pt_dyn_info, req_info,
                                                rsp_info, rsp_ltid,
                                                rsp_flags));
        /* if the default presel entry deleted,
         * Reset the default presel entry flag in the group structure
         */
        if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT)) {
            /* get the group pointer */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid,
                                                     ltid_info,
                                                     entry_attr->group_id,
                                                     entry_attr->stage_id,
                                                     &group_ptr));
            group_ptr->flags &= ~BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT;
        }
    } else {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_delete(unit, req_flags,
                                                         ltid, ltid_info,
                                                         pt_dyn_info, req_info,
                                                         rsp_info, rsp_ltid,
                                                         rsp_flags));
    }
    /* release the group resources if it is last entry of group */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP)) {
        /* release the group's resources */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_free_group_resources(unit,
                                                 ltid, ltid_info, entry_attr));
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_fp_entry_lookup(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 uint32_t *index)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_lookup(unit,
                                                     req_flags, ltid, ltid_info,
                                                     pt_dyn_info,
                                                     req_info, rsp_info,
                                                     rsp_ltid, rsp_flags,
                                                     index));
exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_fp_entry_move(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               uint8_t partition,
                               uint32_t prefix,
                               uint32_t from_idx,
                               uint32_t to_idx,
                               void *entry_attrs,
                               bcmbd_pt_dyn_info_t *pt_dyn_info)
{

    uint16_t from_index = 0;
    uint16_t to_index = 0;
    uint32_t num_presel_per_slice;
    uint16_t pri_from_index = 0;
    uint16_t pri_to_index = 0;
    uint8_t slice_id = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices;        /* Number of Slices */

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(entry_attrs);

    /* Move primary entry */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_move(unit, ltid, ltid_info,
                                                   partition, prefix,
                                                   from_idx,
                                                   to_idx,
                                                   attrs,
                                                   pt_dyn_info));

    /* get the presel entry flag */
    if (SHR_BITGET(attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {

       /* get the group pointer */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                      attrs->group_id,
                                      attrs->stage_id,
                                      &fg));

       /* Initialize field slice pointer. */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                      &num_slices, attrs->pipe_id,
                                      attrs->stage_id,
                                      &slice_fc));

       /* get the primary slice index */
       slice_id = fg->primary_slice_id;
       /* get the number of presel entries in that slice */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                      unit, ltid, ltid_info, attrs->pipe_id,
                                      slice_id,
                                      &num_presel_per_slice));

       pri_from_index = (from_idx) - (slice_id * num_presel_per_slice);
       pri_to_index = (to_idx) - (slice_id * num_presel_per_slice);

       while (slice_id < num_slices
                     && (0 != slice_fc[slice_id].next_slice_id_offset)) {

            /* go to the next slice if exists */
            slice_id = slice_id +
                        slice_fc[slice_id].next_slice_id_offset;
            /* get the number of presel entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                          unit, ltid, ltid_info, attrs->pipe_id,
                                          slice_id,
                                          &num_presel_per_slice));

            from_index = (slice_id * num_presel_per_slice) + pri_from_index;
            to_index = (slice_id * num_presel_per_slice) + pri_to_index;

           /* Move expanded entry */
           SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_move(unit, ltid, ltid_info,
                                                          partition, prefix,
                                                          from_index,
                                                          to_index,
                                                          entry_attrs,
                                                          pt_dyn_info));

        }

    }
exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_group_entry_segment_get(int unit,
                                          bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                          int pipe_id,
                                          int *segment_id)
{
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    uint16_t num_slices = 0;
    int seg_id = -1;
    bcmptm_fp_entry_seg_bmp_t *entry_seg_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                         stage_id,
                                         &stage_fc));

    entry_seg_bmp = (bcmptm_fp_entry_seg_bmp_t*) (((uint8_t *)stage_fc)
                     + stage_fc->entry_seg_bmp_offset);

    num_slices = stage_fc->num_slices / stage_fc->num_pipes;

    for (seg_id = 0; seg_id < num_slices; seg_id++) {
         if (!(SHR_BITGET((entry_seg_bmp[pipe_id]).w,seg_id))) {
            SHR_BITSET((entry_seg_bmp[pipe_id]).w,seg_id);
            break;
         }
    }

    *segment_id = seg_id;

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_tcam_fp_presel_entries_clear(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t slice_id,
                                 bcmptm_rm_tcam_entry_attrs_t *entry_attrs)
{
    bcmltd_sid_t rsp_ltid = -1;
    uint16_t idx = -1, index;
    uint32_t num_presel_per_slice;
    uint8_t part = 0;
    uint32_t rm_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t rsp_entry_sid = 0;
    uint32_t req_flags = 0;
    uint32_t entry_words
                  [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS][BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t *entry_words_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_attrs, SHR_E_PARAM);

    /* set the presel entry flag */
    SHR_BITSET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY);

    /* Fetch the TCAM entry information for the givem LTID. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                  (void *)entry_attrs,
                                  &entry_info));
    /* get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                   unit, ltid, ltid_info, entry_attrs->pipe_id,
                                   slice_id,
                                   &num_presel_per_slice));

    /* set tbl Instance depending on pipe number*/
    pt_dyn_info.tbl_inst = entry_attrs->pipe_id;

    /* loop for all presel entries per slice and
     * clear the non-empty entries from primary
     * slice. clear for the
     * width parts of the TCAM are taken care by hw
     * read/write functions
     */
    for (idx = 0; idx < num_presel_per_slice; idx++) {

        /* calculate the indexs */
        index = (slice_id * num_presel_per_slice) + idx;

        /* if entry is invalid no need to clear it. */
        if (entry_info[index].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
            continue;
        }

        /* Update the HW with NULL data. */
        for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
            sal_memset(entry_words[part], 0,
                   sizeof(entry_words[part]));
            entry_words_row[part] = entry_words[part];
        }

        /* Write the entry words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                                rm_flags, req_flags, index,
                                                entry_words_row,
                                                &pt_dyn_info,
                                                NULL, (void *) entry_attrs,
                                                &rsp_ltid, &rsp_flags,
                                                &rsp_entry_sid));
        /* Update the TCAM entry information. */
        entry_info[index].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
        entry_info[index].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[index].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;

    }
exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_entry_segment_compress(int unit,
                     bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     uint32_t group_id,
                     int pipe,
                     bcmptm_rm_tcam_fp_stage_id_t stage_id,
                     uint8_t num_slices_freed,
                     bool group_delete)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *group_ptr = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *group_ptr_update = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *grp_ptr[32] = {NULL};
                               /* Field Group structure pointer. */
    bool free_segment = 0;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    int seg_id = -1;
    int segments_free = 0;
    int first_free_segment_id = -1;
    int last_segment_id = -1;
    int first_segment_id = -1;
    bcmptm_fp_entry_seg_bmp_t *stage_entry_seg_bmp = NULL;
    bcmptm_fp_grp_seg_bmp_t *group_segments = NULL;
    uint8_t num_slices = 0;
    int grp_count = 0;
    uint8_t found = 0;
    int idx = 0;
    uint32_t grp_idx = 0;
    int grp_id = -1;
    int pipe_id = 0;
    uint8_t clear_flag ;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                         stage_id,
                                         &stage_fc));
    num_slices = stage_fc->num_slices / stage_fc->num_pipes;

    SHR_ALLOC(group_segments, (sizeof(bcmptm_fp_grp_seg_bmp_t)
              * num_slices), "FP entry seg");

    /* Clear group_segment bitmap */
    sal_memset(group_segments, 0 , (sizeof(bcmptm_fp_grp_seg_bmp_t) * num_slices));

    stage_entry_seg_bmp = (bcmptm_fp_entry_seg_bmp_t*)
                           (((uint8_t *)stage_fc)
                           + stage_fc->entry_seg_bmp_offset);

    /* Create group segment bitmap */
    fg = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
         if ((fg->valid == 1) && (fg->pipe_id == pipe)) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                 if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                     SHR_BITSET(group_segments[seg_id].w,
                                fg->group_id);
                 }
             }
         }
         fg++;
    }

    if (pipe == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    } else {
        pipe_id = pipe;
    }

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   group_id, stage_id,
                                   &fg));

    if (!group_delete) {
        for (seg_id = 0; seg_id < num_slices; seg_id++) {
            free_segment = 0;
            if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                /* check for free segment */
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_rm_tcam_fp_entry_segment_check(
                                                          unit,
                                                          ltid, ltid_info,
                                                          stage_id,
                                                          pipe_id,
                                                          seg_id,
                                                          &free_segment));
                /* If free clear bitmaps for all associated groups */
                if (free_segment && (segments_free <= num_slices_freed)) {
                    segments_free++;
                    /* Update auto compression data to stage */
                    SHR_BITCLR(stage_entry_seg_bmp[pipe_id].w, seg_id);
                    for (idx = 0; idx < stage_fc->num_groups; idx++) {
                         if (SHR_BITGET(group_segments[seg_id].w, idx)) {
                             /* Get the field group pointer for all groups
                                in current segment.
                              */
                             group_ptr = NULL;
                             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(
                                                         unit,
                                                         ltid, ltid_info,
                                                         idx,
                                                         stage_id,
                                                         &group_ptr));
                             /* Update auto compression data to group */
                             SHR_BITCLR(group_ptr->entry_seg_bmp.w,seg_id);

                         }
                    }
                }
            }
        }

    } else {
        for (seg_id = 0; seg_id < num_slices; seg_id++) {
            clear_flag = TRUE;
            if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                SHR_BITCLR(fg->entry_seg_bmp.w,seg_id);
                /* Check if any other group is using the same
                 * seg id
                 */
                for (grp_idx = 0; grp_idx < stage_fc->num_groups; grp_idx++) {
                    if ((grp_idx != fg->group_id) && SHR_BITGET(group_segments[seg_id].w, grp_idx)) {
                         clear_flag = FALSE;
                    }
                }
                /* Mark seg id is free only if not shared with
                 * other groups
                 */
                if (clear_flag) {
                    SHR_BITCLR(stage_entry_seg_bmp[pipe_id].w, seg_id);
                }
            }
        }
        fg->entry_info_seg = 0;
        fg->entry_hash_size =0;
        fg->entry_hash_seg = 0;
        fg->presel_entry_info_seg = 0;
        fg->presel_entry_hash_size = 0;
        fg->presel_entry_hash_seg = 0;

    }

    /* Find first free segment */
    for (seg_id = 0;
         seg_id < num_slices; seg_id++) {
         if (!(SHR_BITGET(stage_entry_seg_bmp[pipe_id].w,seg_id))) {
            first_free_segment_id = seg_id;
            found = 1;
            break;
         }
    }
    /* some segments are freed */
    if (found == 1) {
        /* find first segment block to move */
        for (seg_id = first_free_segment_id;
             seg_id < num_slices; seg_id++) {
             if ((SHR_BITGET(stage_entry_seg_bmp[pipe_id].w,seg_id))) {
                first_segment_id = seg_id;
                break;
             }
        }

        /* All segments are free, no moves needed */
        if (first_segment_id == -1) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }

        /* find last segment block to move */
        for (seg_id = first_free_segment_id;
             seg_id < num_slices; seg_id++) {
             if ((SHR_BITGET(stage_entry_seg_bmp[pipe_id].w,seg_id))) {
                last_segment_id = seg_id;
             }
        }
    } else {
        /* No free segments (Eg: slice sharing groups) */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }


    seg_id = first_segment_id;
    while (seg_id <= last_segment_id) {
          grp_count = 0;
            for (idx = 0; idx < stage_fc->num_groups; idx++) {
                if (SHR_BITGET(group_segments[seg_id].w, idx)) {
                    /* Get the field group pointer for all groups
                       in current segment.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit,
                                           ltid, ltid_info,
                                           idx,
                                           stage_id,
                                           &grp_ptr[grp_count]));
                    grp_count++;
                }
            }

            if (grp_count == 0) {
                /* No groups are available.
                * Ideally should not hit this condition.
                */
               SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }


         /* Move segment from seg_id to first_free_segment */
         SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_segment_move(unit,
                                                 ltid, ltid_info,
                                                 stage_id,
                                                 pipe_id,
                                                 seg_id,
                                                 first_free_segment_id));
         /* Cleanup previous segment */
         SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_segment_clear(unit, ltid,
                                                         ltid_info,
                                                         stage_id,
                                                         pipe_id,
                                                         seg_id));
         /* Update bitmaps */
         SHR_BITCLR(stage_entry_seg_bmp[pipe_id].w, seg_id);
         SHR_BITSET(stage_entry_seg_bmp[pipe_id].w, first_free_segment_id);
         for (grp_id = 0; grp_id < grp_count; grp_id++) {
             SHR_BITCLR(grp_ptr[grp_id]->entry_seg_bmp.w, seg_id);
             SHR_BITSET(grp_ptr[grp_id]->entry_seg_bmp.w,
                        first_free_segment_id);
         }

         /* Move next */
         seg_id++;
         first_free_segment_id++;
    }

     /* Fetch the group segment start pointer */
    group_ptr_update = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

   /* Update all groups to point to first segment in its list */
    for (idx =0;idx < stage_fc->num_groups; idx++) {
        if ((group_ptr_update->valid == 1)
             && (group_ptr_update->pipe_id == pipe)) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                 if (SHR_BITGET(group_ptr_update->entry_seg_bmp.w,seg_id)) {
                     SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_segment_update(
                                    unit, ltid, ltid_info,
                                    stage_id,
                                    pipe_id,
                                    group_ptr_update,
                                    seg_id));
                     break;

                 }
             }
        }

        group_ptr_update = group_ptr_update + 1;
    }


exit:
    SHR_FREE(group_segments);
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}
