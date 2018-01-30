/*! \file rm_tcam_fp_utils.c
 *
 * Util functions for FP based TCAMs
 * This file contains low level utility functions for FP based TCAMs
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
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include "rm_tcam.h"
#include "rm_tcam_fp.h"
#include "rm_tcam_fp_utils.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

#define BCMPTM_RM_TCAM_MAX_DEPTH_PARTS 12


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_prio_fp_entry_info_get(int unit, bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 void *attrs,
                                 bcmptm_rm_tcam_prio_entry_info_t **entry_info)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->stage_id,
                                                     &stage_ptr));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));

    *entry_info = NULL;

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        offset = group_ptr->presel_entry_info_seg;
    } else {
        offset = group_ptr->entry_info_seg;
    }

    *entry_info = (bcmptm_rm_tcam_prio_entry_info_t*)
                   ((uint8_t *)stage_ptr + offset);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_hash_get(int unit,
                                      bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      void *attrs,
                                      int **entry_hash)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_hash, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->stage_id,
                                                     &stage_ptr));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));

    *entry_hash = NULL;

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        offset = group_ptr->presel_entry_hash_seg;
    } else {
        offset = group_ptr->entry_hash_seg;
    }

    *entry_hash = (int*)((uint8_t *)stage_ptr + offset);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_flags_get(int unit, bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       void *attrs,
                                       uint32_t *flags)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(flags, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));
    *flags = group_ptr->flags;

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        *flags &= ~BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        *flags &= ~BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        *flags |= BCMPTM_RM_TCAM_F_AGGR_VIEW;
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_prio_fp_entry_free_count_get(int unit, bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            void *attrs,
                                            uint32_t *free_entries)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;
    uint8_t slice_id_prev;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(free_entries, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                                     &num_slices,
                                                     entry_attrs->pipe_id,
                                                     entry_attrs->stage_id,
                                                     &slice_ptr));
    slice_id = group_ptr->primary_slice_id;

    /* Initialize the free count value to 0 */
    *free_entries = 0;
    /* presel entry */
    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        *free_entries = slice_ptr[slice_id].free_presel_entries;
    } else {
        /* aggregate the free count for all depth slices corrsponding to
         * to the given group.
         */
        do {
            /* Add the free entries count in current slice */
            *free_entries = (*free_entries) + slice_ptr[slice_id].free_entries;

            slice_id_prev = slice_id;
            /* go to the next slice if exists */
            slice_id = slice_id +
                        slice_ptr[slice_id].next_slice_id_offset;

        } while (slice_id < num_slices
                 && (slice_id != slice_id_prev));
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_valid_count_get(int unit, bcmltd_sid_t ltid,
                                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                                        void *attrs,
                                        uint32_t *num_entries)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    uint32_t num_presels_per_slice = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(num_entries, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_presel_entries_per_slice_get(
                                              unit, ltid,
                                              ltid_info, entry_attrs->pipe_id,
                                              0,
                                              &num_presels_per_slice));
        *num_entries = num_presels_per_slice;
    } else {
        *num_entries = group_ptr->num_entries;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_hash_size_get(int unit, bcmltd_sid_t ltid,
                                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                                           void *attrs,
                                           uint32_t *entry_hash_size)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(entry_hash_size, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));

    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        *entry_hash_size = group_ptr->presel_entry_hash_size;
    } else {
        *entry_hash_size = group_ptr->entry_hash_size;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_free_count_incr(int unit, bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         uint32_t ent_logical_index,
                                         void *attrs)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;
    uint8_t slice_id_prev;
    uint32_t num_entries_per_slice;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));
    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->stage_id,
                                                     &stage_fc));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                                     &num_slices,
                                                     entry_attrs->pipe_id,
                                                     entry_attrs->stage_id,
                                                     &slice_ptr));
    slice_id = group_ptr->primary_slice_id;

    /* increment the entries count in current slice */
    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        slice_ptr[slice_id].free_presel_entries += 1;
        group_ptr->num_presel_entries -= 1;
    } else {
        /* get the number of entries in that slice */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                       unit, ltid, ltid_info,
                                       slice_id, entry_attrs->pipe_id,
                                       entry_attrs->group_mode,
                                       &num_entries_per_slice));

        /* Traverse till the right depth */
        while (ent_logical_index >= num_entries_per_slice) {
            ent_logical_index = ent_logical_index - num_entries_per_slice;
            if (slice_ptr[slice_id].next_slice_id_offset == 0) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            slice_id = slice_id + slice_ptr[slice_id].next_slice_id_offset;
            /* get the number of entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                           unit, ltid, ltid_info,
                                           slice_id, entry_attrs->pipe_id,
                                           entry_attrs->group_mode,
                                           &num_entries_per_slice));
        }
        /* update the free count for all slices corrsponding to
         * to the given group.
         */
        do {
            slice_ptr[slice_id].free_entries += 1;
            /* got he next slice if exists */
            slice_id_prev = slice_id;
            slice_id = slice_id +
                            slice_ptr[slice_id].right_slice_id_offset;

        } while (slice_id < num_slices
               && (slice_id != slice_id_prev));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_free_count_decr(int unit, bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         uint32_t ent_logical_index,
                                         void *attrs)
{
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;
    uint8_t slice_id_prev;
    uint32_t num_entries_per_slice;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->group_id,
                                                     entry_attrs->stage_id,
                                                     &group_ptr));
    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                                     entry_attrs->stage_id,
                                                     &stage_fc));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                                     &num_slices,
                                                     entry_attrs->pipe_id,
                                                     entry_attrs->stage_id,
                                                     &slice_ptr));
    slice_id = group_ptr->primary_slice_id;
    /* presel entry */
    if (SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        slice_ptr[slice_id].free_presel_entries -= 1;
        group_ptr->num_presel_entries += 1;
    } else {
        /* get the number of entries in that slice */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                   unit, ltid, ltid_info,
                                   slice_id, entry_attrs->pipe_id,
                                   entry_attrs->group_mode,
                                   &num_entries_per_slice));

        /* Traverse till the right depth */
        while (ent_logical_index >= num_entries_per_slice) {
            ent_logical_index = ent_logical_index - num_entries_per_slice;
            if (slice_ptr[slice_id].next_slice_id_offset == 0) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            slice_id = slice_id + slice_ptr[slice_id].next_slice_id_offset;
            /* get the number of entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                       unit, ltid, ltid_info,
                                       slice_id, entry_attrs->pipe_id,
                                       entry_attrs->group_mode,
                                       &num_entries_per_slice));
        }

        /* update the free count for all slices corrsponding to
         * to the given group.
         */
        do {
            /* decrement the entries count in current slice */
            slice_ptr[slice_id].free_entries -= 1;
            slice_id_prev = slice_id;
            /* go to the next slice if exists */
            slice_id = slice_id +
                        slice_ptr[slice_id].right_slice_id_offset;

        } while (slice_id < num_slices
           && (slice_id != slice_id_prev));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmptm_rm_tcam_fp_entry_tcam_width_parts_count
 *
 * Purpose:
 *    Get number of tcam entries needed to accomodate a SW entry.
 * Parameters:
 *    unit        - (IN)  BCM device number.
 *    group_mode  - (IN)  Entry group mode
 *    group_mode  - (IN)  Stage flags
 *    part_count  - (OUT) Entry parts count.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(int unit,
                                    uint32_t group_mode,
                                    uint32_t stage_flags,
                                    int *part_count)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(part_count, SHR_E_PARAM);

    /* Check group flags settings and determine number of TCAM entry parts. */
    if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE) {
        *part_count = 1;
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA) {
        if (stage_flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) {
            /*
             * Part value is same for Narrow and Wide TCAMs
             * (IFP_TCAM and IFP_TCAM_WIDE)
             */
            *part_count = 1;
        } else {
            *part_count = 2;
        }
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTER) {
        *part_count = 2;
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_TRIPLE) {
        *part_count = 3;
    } else if (group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) {
        *part_count = 4;
    } else {
        *part_count = 0;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmptm_rm_tcam_fp_entry_tcam_depth_parts_count
 *
 * Purpose:
 *    Get number of tcam entries needed to accomodate a SW entry.
 * Parameters:
 *    unit        - (IN)  BCM device number.
 *    sid         - (IN)  logical id
 *    entry attrs - (IN)  entry attrs structure
 *    part_count  - (OUT) Entry depth parts count.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmptm_rm_tcam_fp_entry_tcam_depth_parts_count(int unit,
                      bcmltd_sid_t ltid,
                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                      bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                      int *part_count)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    uint8_t slice_id;          /* Slice ID */
    uint8_t num_slices;        /* Number of Slices */

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(part_count, SHR_E_PARAM);

    /* Initialize field group pointer. */
    rv = bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attr->group_id, entry_attr->stage_id,
                                   &fg);
    /* group not created yet */
    if (rv != SHR_E_NONE) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &slice_fc));

    *part_count = 0;
    slice_id = fg->primary_slice_id;
    do {
        /* Increment the depth count */
        (*part_count)++;
        /* go to the next slice if exists */
        slice_id = slice_id +
                    slice_fc[slice_id].next_slice_id_offset;

    } while (slice_id < num_slices
                 && (0 != slice_fc[slice_id].next_slice_id_offset));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_pipe_count_get(int unit, bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         void *attrs,
                                         uint8_t *pipe_count)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(pipe_count, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_pipes_count_get(unit, ltid, ltid_info,
                                                  pipe_count));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_fp_entry_total_count_get(int unit, bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         void *attrs,
                                         uint32_t *total_count)
{
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(total_count, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t*) attrs;
    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                                entry_attr->stage_id,
                                                &stage_fc));
    *total_count = (stage_fc->max_entries_per_slice
                       * (stage_fc->num_slices / stage_fc->num_pipes));

exit:
    SHR_FUNC_EXIT();
}

