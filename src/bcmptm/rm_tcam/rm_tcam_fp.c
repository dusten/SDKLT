/*! \file rm_tcam_fp.c
 *
 * Low Level Functions for FP based TCAMs
 * This file contains low level functions for FP based TCAMs
 * Initialization for the FP sub module
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
#include "bcmptm/bcmptm_ser_misc_internal.h"
#include "rm_tcam.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_fp.h"
#include "rm_tcam_fp_utils.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private Functions
 */
static int
bcmptm_rm_tcam_fp_group_init(int unit,
                             bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    uint8_t *start_ptr = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);

    start_ptr = (uint8_t *)(stage_ptr);

    group_ptr = (bcmptm_rm_tcam_fp_group_t*) (start_ptr
                 + stage_ptr->group_info_seg);

    for (idx = 0;idx < stage_ptr->num_groups; idx++) {
        sal_memset(group_ptr, 0, sizeof(bcmptm_rm_tcam_fp_group_t));
        group_ptr->next_shared_group_id = -1;
        group_ptr++;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_slice_init(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    uint8_t *start_ptr = NULL;
    bcmptm_rm_tcam_slice_t *slice_ptr = NULL; /* Slice info. */
    uint8_t slice = 0;                    /* Slice iterator. */
    int mem_sz;            /* Memory allocation buffer size. */
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    uint8_t num_slices = 0;
    int pipe_id = 0;
    int *slice_depth = NULL;
    bcmdrd_sid_t sid_list[2];
    uint32_t slice_group_id;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);

    /* Initialize stage instance slices pointer. */
    start_ptr = (uint8_t *)(stage_ptr);

    slice_ptr = (bcmptm_rm_tcam_slice_t*) (start_ptr
                 + stage_ptr->slice_seg);

    /* Initialze stage slice data structure. */
    mem_sz = stage_ptr->num_slices * sizeof(bcmptm_rm_tcam_slice_t);
    sal_memset(slice_ptr, 0, mem_sz);

    /* Fetch the number of slices per pipe in this stage */
    num_slices = (stage_ptr->num_slices / stage_ptr->num_pipes);

    for (pipe_id = 0; pipe_id < stage_ptr->num_pipes; pipe_id++) {
        /* Initialize stage slices info. */
        for (slice = 0; slice < num_slices; slice++) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                        unit, ltid, ltid_info,
                                        slice, pipe_id, 0,
                                        &num_entries_per_slice));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_presel_entries_per_slice_get(
                                           unit, ltid, ltid_info, pipe_id,
                                           slice,
                                           &num_presels_per_slice));

            slice_ptr->slice_id = slice;
            slice_ptr->num_entries = num_entries_per_slice;
            slice_ptr->num_presel_entries = num_presels_per_slice;
            slice_ptr->next_slice_id_offset = 0;
            slice_ptr->prev_slice_id_offset = 0;
            slice_ptr->left_slice_id_offset = 0;
            slice_ptr->right_slice_id_offset = 0;
            slice_ptr->primary_grp_id = -1;

            /* Enaable Intra Slice & auto expansion based on stage flags. */
            if (stage_ptr->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
                slice_ptr->slice_flags |= BCMPTM_SLICE_INTRASLICE_CAPABLE;
            }
            if (stage_ptr-> flags & BCMPTM_STAGE_AUTOEXPANSION) {
                slice_ptr->slice_flags |= BCMPTM_SLICE_SUPPORT_AUTO_EXPANSION;
            }
            slice_ptr = slice_ptr + 1;
        }
    }

    if (stage_ptr->flags & BCMPTM_STAGE_MULTI_VIEW_TCAM) {
        SHR_ALLOC(slice_depth, (num_slices * sizeof(int)), "slice depth");
        if (slice_depth == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }

        slice_ptr = (bcmptm_rm_tcam_slice_t*) (start_ptr
                     + stage_ptr->slice_seg);

        for (slice = 0; slice < num_slices; slice++) {
            slice_depth[slice] = slice_ptr->num_entries;
            slice_ptr = slice_ptr + 1;
        }

        sid_list[0] = ltid_info->hw_entry_info[0].sid[0][0];
        sid_list[1] = ltid_info->hw_entry_info[1].sid[0][0];

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_ser_slice_group_register(unit, slice_depth, num_slices,
                                            sid_list, 2,
                                            BCMPTM_TCAM_SLICE_DISABLED, 0,
                                            &slice_group_id));
        stage_ptr->slice_group_id = slice_group_id;
    }

exit:
    SHR_FREE(slice_depth);
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_lt_config_init(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    bcmptm_rm_tcam_lt_config_t *lt_config_ptr = NULL; /* Lt config info. */
    uint32_t num_groups_per_pipe = 0;
    int mem_sz;                        /* Memory allocation buffer size. */
    uint32_t lt_idx;                  /* Logical Table iterator.         */
    int pipe;                         /* Instance iterator.              */


    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);


    lt_config_ptr = (bcmptm_rm_tcam_lt_config_t*) (((uint8_t *)stage_ptr)
                     + stage_ptr->lt_config_seg);

    /* Initialze stage slice data structure. */
    /* num pipes * num groups per pipe * size of structure */
    mem_sz = (stage_ptr->num_groups * stage_ptr->num_pipes)
               * sizeof(bcmptm_rm_tcam_lt_config_t);

    sal_memset(lt_config_ptr, 0, mem_sz);


    /* Initialize for all pipes and all logical tables. */
    for (pipe = 0; pipe < stage_ptr->num_pipes; pipe++) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                          unit, ltid, ltid_info, pipe, &num_groups_per_pipe));
        for (lt_idx = 0; lt_idx < num_groups_per_pipe; lt_idx++) {
            /* Initialize Logical Table ID. */
           lt_config_ptr->lt_id = lt_idx;
           if (stage_ptr->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
               lt_config_ptr->lt_id = lt_idx + (num_groups_per_pipe * pipe);
           }
           lt_config_ptr = lt_config_ptr + 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_validate_single_slice
 * Purpose:
 *     Validate and return  slice count if the group can
 *     expand to Single or Intra Slice Double wide mode group.
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     group mode    - (IN) group mode
 *     num_slices    - (IN) number of slices for this stage
 *     slice_fc      - (IN/OUT) Field Slice structure. Update flags
 *     slice         - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_validate_single_slice(int unit,
                                          uint32_t group_mode,
                                          uint8_t num_slices,
                                          bcmptm_rm_tcam_slice_t *slice_fc,
                                          uint8_t *slice_count,
                                          uint32_t *max_entries)
{
    uint8_t slice_idx;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    *slice_count = 0;
    *max_entries = 0;
    /* Loop through the slice segment and return the slice
     * number to use
     */
    for (slice_idx = 0; slice_idx < num_slices; slice_idx++) {
        /* check the in USE flag of slice */
        if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
            (*slice_count)++;
            *max_entries += slice_fc->num_entries;
        }
        slice_fc = slice_fc + 1;
    }

exit:
    SHR_FUNC_EXIT();
}
/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_validate_double_slice
 * Purpose:
 *     Validate and return slice count if it can be
 *     expanded to double wide mode group.
 * Parameters:
 *     unit          - (IN  BCM device number.
 *     group mode    - (IN) group mode
 *     stage_fc      - (IN) Field stage structure.
 *     num_slices    - (IN) number of slices for this stage
 *     slice_fc      - (IN/OUT) Field Slice structure. Update flags
 *     slice         - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_validate_double_slice(int unit, uint32_t group_mode,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice_count,
                               uint32_t *max_entries)
{
    uint8_t slice_idx;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    uint8_t slice_bundle_size;
    uint8_t slice_ratio = 1;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    slice_bundle_size = stage_fc->slice_bundle_size;
    *slice_count = 0;
    *max_entries = 0;
    if ((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) ||
        ((stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) &&
        (stage_fc->flags & BCMPTM_SLICE_INTRASLICE_CAPABLE))) {
        slice_ratio = 2;
    }
    /* Loop through the slice segment and return the slice
     * number to use
     */
    for (slice_idx = 0; slice_idx < (num_slices - 1); slice_idx++) {
        /*
         * Double wide group is created using consecutive slices within a memory
         * macro. But consecutive slices across a memory macro boundary cannot be
         * paried to create a double wide group. Skip slices 2, 5, 8 and 11 from
         * allocating for Double Wide Group base slice.
         */
        if ((slice_bundle_size - (slice_idx % slice_bundle_size)) >= 2) {
            /* For legacy Arch slice should be even */
            if (!(stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL)
                    && (slice_idx % 2)) {
                slice_fc = slice_fc + 1;
                continue;
            }
            /* check the in USE flag of slice */
            if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
                /* check whether the next slice is free */
                slice_next = slice_fc + 1;
                if (slice_next != NULL
                    && (!(slice_next->slice_flags & BCMPTM_SLICE_IN_USE))) {
                    (*slice_count)++;
                    *max_entries += (slice_fc->num_entries / slice_ratio);
                    slice_fc = slice_fc + 1;
                    slice_idx = slice_idx + 1;
                }
            }
        }
        slice_fc = slice_fc + 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_validate_triple_slice
 * Purpose:
 *     Validate and return slice count if it can be
 *     expanded to triple wide mode group.
 * Parameters:
 *     unit          - (IN  BCM device number.
 *     stage_fc      - (IN) Field stage structure.
 *     num_slices    - (IN) number of slices for this stage
 *     slice_fc      - (IN) Field Slice structure. Update flags
 *     slice_count   - (OUT) slice count.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_validate_triple_slice(int unit,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice_count,
                               uint32_t *max_entries)
{
    uint8_t slice_idx;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    bcmptm_rm_tcam_slice_t *slice_next_next = NULL;
    uint8_t slice_bundle_size;
    uint8_t slice_ratio = 1;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    slice_bundle_size = stage_fc->slice_bundle_size;
    *slice_count = 0;
    *max_entries = 0;
    if (stage_fc->flags & BCMPTM_SLICE_INTRASLICE_CAPABLE) {
        slice_ratio = 2;
    }
    /* Loop through the slice segment and return the slice
     * number to use
     */
    /* As we need 3 contiguous slices, we can stop the loop at
     * Max slices - 2 as after this there can never exist 3
     * contiguous slices
     */
    for (slice_idx = 0; slice_idx < (num_slices - 2); slice_idx++) {
        /*
         * Triple wide group is created using consecutive slices within a memory
         * macro. But consecutive slices across a memory macro boundary cannot be
         * paried to create a Triple wide group.
         */
        if ((slice_bundle_size - (slice_idx % slice_bundle_size)) >= 3) {
            /* check the in USE flag of slice */
            if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
                /* check whether the next slice is free */
                slice_next = slice_fc + 1;
                if (slice_next != NULL
                    && (!(slice_next->slice_flags & BCMPTM_SLICE_IN_USE))) {
                    /* check whether the next slice is free */
                    slice_next_next = slice_next + 1;
                    if (slice_next_next != NULL
                        && (!(slice_next_next->slice_flags
                        & BCMPTM_SLICE_IN_USE))) {
                        (*slice_count)++;
                        *max_entries += (slice_fc->num_entries / slice_ratio);
                        slice_fc = slice_fc + 2;
                        slice_idx = slice_idx + 2;
                    }
                }
            }
        }
        slice_fc = slice_fc + 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief calculate the number of slices the given group can auto expand
 *
 * \param [in] unit BCM device number.
 * \param [in] ltid Logical Table Enum
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] entry_attrs entry attrs structure
 * \param [out] slice_count number of slices
 * \param [out] max_entries max number of entries
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
static int
bcmptm_rm_tcam_fp_validate_group_auto_expansion(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   bcmptm_rm_tcam_entry_attrs_t *entry_attrs,
                                   uint8_t *slice_count,
                                   uint32_t *max_entries)
{
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attrs, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_count, SHR_E_PARAM);
    SHR_NULL_CHECK(max_entries, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attrs->stage_id,
                                   &stage_fc));
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attrs->pipe_id,
                                   entry_attrs->stage_id,
                                   &slice_fc));
        /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attrs->group_id,
                                   entry_attrs->stage_id,
                                   &fg));

    if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == fg->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == fg->group_mode) {
        /* count the number of single slices that can be used
         * for this Field Group. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_validate_single_slice(unit,
                                          fg->group_mode, num_slices,
                                          slice_fc, slice_count, max_entries));
        if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == fg->group_mode) {
            *max_entries = (*max_entries) / 2 ;
        }
    } else  if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == fg->group_mode
        || (BCMPTM_RM_TCAM_GRP_MODE_QUAD == fg->group_mode
        && stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE)) {
        /* count the number of double slices that can be used
         * for this Field Group. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_validate_double_slice(unit,
                                          fg->group_mode, stage_fc, num_slices,
                                          slice_fc, slice_count, max_entries));

    } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == fg->group_mode
            && stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        /* count the number of triple slices that can be used
         * for this Field Group. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_validate_triple_slice(unit,
                                          stage_fc, num_slices,
                                          slice_fc, slice_count, max_entries));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }

 exit:
    SHR_FUNC_EXIT();

}
/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_fp_calc_mem_required(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                    void *mem_size)
{
    int size = 0;
    uint8_t num_slices = 0;
    uint32_t num_groups_per_pipe = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    uint32_t max_num_entries_per_slice = 0;
    uint32_t max_num_presels_per_slice = 0;
    uint8_t num_pipes = 0;
    uint32_t *total_size = mem_size;
    uint8_t pipe = 0;
    uint8_t slice_id = 0;
    uint32_t stage_flags = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_pipes_count_get(unit,
                                                           ltid, ltid_info,
                                                           &num_pipes));
    /* This should always be the first structure
     * memory allocation for every LTID
     */
    /* memory required to save Transaction related info */
    size += sizeof(bcmptm_rm_tcam_trans_info_t);

    size += sizeof(bcmptm_rm_tcam_fp_stage_t);
    size += (num_pipes * sizeof(bcmptm_fp_entry_seg_bmp_t));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_stage_flags_get(unit,
                                                  ltid, ltid_info,
                                                  &stage_flags));
    for (pipe = 0; pipe < num_pipes; pipe ++) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                                                 unit,
                                                 ltid, ltid_info,
                                                 pipe,
                                                 &num_groups_per_pipe));
        size += (num_groups_per_pipe
                  * sizeof(bcmptm_rm_tcam_fp_group_t));
        /* LT config info is common for both EM and Ingress stages */
        if (!(stage_flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
            size += (num_groups_per_pipe
                  * sizeof(bcmptm_rm_tcam_lt_config_t));
        }

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                                                          ltid, ltid_info,
                                                          pipe,
                                                          &num_slices));
        size += (num_slices * sizeof(bcmptm_rm_tcam_slice_t));

        max_num_entries_per_slice = 0;
        max_num_presels_per_slice = 0;

        for (slice_id = 0; slice_id < num_slices; slice_id++) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                               unit, ltid, ltid_info,
                                               slice_id, pipe, 0,
                                               &num_entries_per_slice));

            if (max_num_entries_per_slice < num_entries_per_slice) {
                max_num_entries_per_slice = num_entries_per_slice;
            }

            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_presel_entries_per_slice_get(
                                               unit, ltid, ltid_info, pipe,
                                               slice_id,
                                               &num_presels_per_slice));

              if (max_num_presels_per_slice < num_presels_per_slice) {
                  max_num_presels_per_slice = num_presels_per_slice;
              }
        }

        size += ((max_num_entries_per_slice * num_slices)
                   * sizeof(int));
        size += ((max_num_entries_per_slice * num_slices)
                * sizeof(bcmptm_rm_tcam_prio_entry_info_t));

        size += (((max_num_presels_per_slice * num_slices)
                   / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)
                   * sizeof(int));
        size += ((max_num_presels_per_slice * num_slices)
                * sizeof(bcmptm_rm_tcam_prio_entry_info_t));

    }

    *total_size += size;
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_init_metadata(int unit, bool warm,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *start_pointer,
                                uint32_t *offset)
{
    uint8_t *start_ptr = (uint8_t*)start_pointer;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;
    uint32_t current_ptr = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    int  *entry_hash = NULL;
    bcmptm_rm_tcam_prio_entry_info_t *presel_entry_info = NULL;
    int  *presel_entry_hash = NULL;
    uint8_t num_slices = 0;
    uint8_t total_num_slices = 0;
    uint32_t total_num_groups = 0;
    uint32_t num_groups_per_pipe = 0;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    uint32_t max_num_entries_per_slice = 0;
    uint32_t max_num_presels_per_slice = 0;
    uint32_t num_entries = 0;
    uint32_t num_presels = 0;
    uint8_t num_pipes = 0;
    uint8_t slice_bundle_size = 0;
    uint32_t stage_flags = 0;
    uint32_t idx = 0;
    uint8_t pipe = 0;
    uint8_t slice_id = 0;
    int rv = SHR_E_NONE;
    uint8_t *start_offset_ptr = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(start_pointer, SHR_E_PARAM);
    SHR_NULL_CHECK(offset, SHR_E_PARAM);

    trans_info = (bcmptm_rm_tcam_trans_info_t *) (start_ptr + *offset);

    current_ptr = sizeof(bcmptm_rm_tcam_trans_info_t);

    stage_ptr = (bcmptm_rm_tcam_fp_stage_t*) (start_ptr + *offset
                                  + sizeof(bcmptm_rm_tcam_trans_info_t));

    /* Initalize only during coldboot. */
    if (warm == FALSE) {
        trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
        sal_memset(stage_ptr, 0, sizeof(bcmptm_rm_tcam_fp_stage_t));
    } else {
        if (trans_info->sign_word != BCMPTM_HA_SIGN_LT_OFFSET_INFO) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    start_offset_ptr = (uint8_t*) trans_info;

    num_entries   = 0;
    num_presels   = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_pipes_count_get(unit,
                                                  ltid, ltid_info,
                                                  &num_pipes));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_slice_bundle_size_get(unit,
                                                  ltid, ltid_info,
                                                  &slice_bundle_size));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_stage_flags_get(unit,
                                                  ltid, ltid_info,
                                                  &stage_flags));
    /* Initalize only during coldboot. */
    if (warm == FALSE) {
        stage_ptr->num_pipes = num_pipes;
        stage_ptr->slice_bundle_size = slice_bundle_size;
        stage_ptr->flags = stage_flags;
    } else {
        /*
         * Ensure that num_pipes and slice_bundle_size is not changed
         * during warmboot.
         */
        if (stage_ptr->num_pipes != num_pipes ||
            stage_ptr->slice_bundle_size != slice_bundle_size) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    for (pipe = 0; pipe < num_pipes; pipe ++) {

         SHR_IF_ERR_VERBOSE_EXIT(
             bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                                        unit,
                                        ltid, ltid_info,
                                        pipe,
                                        &num_groups_per_pipe));

         total_num_groups += num_groups_per_pipe;

         SHR_IF_ERR_VERBOSE_EXIT(
             bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                                                 ltid, ltid_info,
                                                 pipe,
                                                 &num_slices));
         total_num_slices += num_slices;

         for (slice_id = 0; slice_id < num_slices;
              slice_id++) {
              SHR_IF_ERR_VERBOSE_EXIT(
               bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                      unit, ltid, ltid_info,
                                      slice_id, pipe, 0,
                                      &num_entries_per_slice));

              if (max_num_entries_per_slice < num_entries_per_slice) {
                  max_num_entries_per_slice = num_entries_per_slice;
              }

              SHR_IF_ERR_VERBOSE_EXIT(
                  bcmptm_rm_tcam_presel_entries_per_slice_get(
                                                 unit, ltid, ltid_info, pipe,
                                                 slice_id,
                                                 &num_presels_per_slice));

              if (max_num_presels_per_slice < num_presels_per_slice) {
                  max_num_presels_per_slice = num_presels_per_slice;
              }
         }
    }

    if (total_num_slices == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Initalize only during coldboot. */
    if (warm == FALSE) {
        stage_ptr->num_groups = total_num_groups;
        stage_ptr->num_slices = total_num_slices;
    } else {
        /*
         * Ensure that num_groups and num_slices is not changed
         * during warmboot.
         */
        if (stage_ptr->num_groups != total_num_groups ||
            stage_ptr->num_slices != total_num_slices) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }
    num_entries = max_num_entries_per_slice * stage_ptr->num_slices;
    num_presels = max_num_presels_per_slice * stage_ptr->num_slices;

    /* Initalize only during coldboot. */
    if (warm == FALSE) {
        stage_ptr->entry_hash_size = (num_entries);
        stage_ptr->presel_entry_hash_size =
            (num_presels / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);

        stage_ptr->max_entries_per_slice = (num_entries
                                            / stage_ptr->num_slices);
        stage_ptr->max_presel_entries_per_slice = (num_presels
                                            / stage_ptr->num_slices);
    } else {
        /*
         * Ensure that num_entries and hash size is not changed
         * during warmboot.
         */
        if ((stage_ptr->entry_hash_size != num_entries) ||
            (stage_ptr->presel_entry_hash_size !=
             (num_presels / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)) ||
            (stage_ptr->max_entries_per_slice !=
             (num_entries / stage_ptr->num_slices)) ||
            (stage_ptr->max_presel_entries_per_slice !=
             (num_presels / stage_ptr->num_slices))) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    current_ptr += sizeof(bcmptm_rm_tcam_fp_stage_t);

    stage_ptr->entry_seg_bmp_offset = current_ptr
                                      - ((uint8_t*)stage_ptr - start_offset_ptr);
    current_ptr += (num_pipes * sizeof(bcmptm_fp_entry_seg_bmp_t));

    stage_ptr->slice_seg = current_ptr
                           - ((uint8_t*)stage_ptr - start_offset_ptr);
    current_ptr += (stage_ptr->num_slices
                    * sizeof(bcmptm_rm_tcam_slice_t));

    stage_ptr->group_info_seg = current_ptr
                                - ((uint8_t*)stage_ptr - start_offset_ptr);
    current_ptr +=
        (stage_ptr->num_groups
         * sizeof(bcmptm_rm_tcam_fp_group_t));

    /* LT config info is common for both EM and Ingress stages */
    if (!(stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        stage_ptr->lt_config_seg = current_ptr
                               - ((uint8_t*)stage_ptr - start_offset_ptr);
        current_ptr +=
                       (stage_ptr->num_groups
                        * sizeof(bcmptm_rm_tcam_lt_config_t));
    }

    stage_ptr->entry_hash_seg = current_ptr
                                - ((uint8_t*)stage_ptr - start_offset_ptr);
    current_ptr +=
        (stage_ptr->entry_hash_size * sizeof(uint32_t));

    stage_ptr->entry_info_seg = current_ptr
                                - ((uint8_t*)stage_ptr - start_offset_ptr);
    current_ptr +=
    (num_entries * sizeof(bcmptm_rm_tcam_prio_entry_info_t));

    stage_ptr->presel_entry_hash_seg = current_ptr
                                     - ((uint8_t*)stage_ptr - start_offset_ptr);
    current_ptr +=
        (stage_ptr->presel_entry_hash_size * sizeof(uint32_t));

    stage_ptr->presel_entry_info_seg = current_ptr
                                     - ((uint8_t*)stage_ptr - start_offset_ptr);
    current_ptr +=
    (num_presels * sizeof(bcmptm_rm_tcam_prio_entry_info_t));

    stage_ptr = (bcmptm_rm_tcam_fp_stage_t*) (start_ptr + *offset
                                  + sizeof(bcmptm_rm_tcam_trans_info_t));

    /* Initalize only during coldboot. */
    if (warm == FALSE) {
        /* slice init */
        rv = bcmptm_rm_tcam_fp_slice_init(unit, ltid, ltid_info, stage_ptr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        /* group init */
        rv = bcmptm_rm_tcam_fp_group_init(unit, stage_ptr);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        /* Lt config init */
        /* LT config info is common for both EM and Ingress stages */
        if (!(stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
            rv = bcmptm_rm_tcam_fp_lt_config_init(unit, ltid, ltid_info, stage_ptr);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        /* entry info init */
        entry_info =
            (bcmptm_rm_tcam_prio_entry_info_t*) ((uint8_t *)stage_ptr
                 + stage_ptr->entry_info_seg);

        for (idx = 0; idx < num_entries; idx++) {
            entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
            entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
            entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        }

        /* hash init */
        entry_hash = (int *) ((uint8_t *)stage_ptr
                                   + stage_ptr->entry_hash_seg);
        for (idx = 0; idx < stage_ptr->entry_hash_size; idx++) {
            entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
        }

        /* presel entry info init */
        presel_entry_info =
            (bcmptm_rm_tcam_prio_entry_info_t*) ((uint8_t *)stage_ptr
                 + stage_ptr->presel_entry_info_seg);

        for (idx = 0; idx < num_presels; idx++) {
            presel_entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
            presel_entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
            presel_entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        }

        /* presel hash init */
        presel_entry_hash = (int *) ((uint8_t *)stage_ptr
                                   + stage_ptr->presel_entry_hash_seg);
        for (idx = 0; idx < stage_ptr->presel_entry_hash_size; idx++) {
            presel_entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
        }
    }

    (*offset) += current_ptr;
    (trans_info->ltid_size) = current_ptr;

    if (warm == FALSE) {
        trans_info->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_tcam_fp_stage_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  bcmptm_rm_tcam_fp_stage_id_t stage_id,
                  bcmptm_rm_tcam_fp_stage_t **stage_fc)
{
    void *tcam_info = NULL;
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);

    /* Fetch the tcam information for the given TCAM LTID. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_info_get(unit, ltid, ltid_info,
                                  &tcam_info));

    *stage_fc = (bcmptm_rm_tcam_fp_stage_t *)((uint8_t *)tcam_info
                              + sizeof(bcmptm_rm_tcam_trans_info_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_slice_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  uint8_t *num_slices,
                  int pipe_id,
                  bcmptm_rm_tcam_fp_stage_id_t stage_id,
                  bcmptm_rm_tcam_slice_t **slice_fc)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    /* Fetch the corresponding stage info */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   stage_id, &stage_fc));

    /* Fetch the number of slices in this stage */
    *num_slices = (stage_fc->num_slices / stage_fc->num_pipes);

    /* For global mode, use the memory created for pipe 0
     * As only either global mode or pipe mode can exists
     * at a gvien time
     */
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    /* Fetch the slice segment start pointer */
    *slice_fc = (bcmptm_rm_tcam_slice_t*) (((uint8_t *)stage_fc)
                 + stage_fc->slice_seg
                 + ((*num_slices) * (uint32_t)pipe_id
                    * sizeof(bcmptm_rm_tcam_slice_t)));

    /* Log the function exit. */
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_group_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  uint32_t group_id,
                  bcmptm_rm_tcam_fp_stage_id_t stage_id,
                  bcmptm_rm_tcam_fp_group_t **group_fc)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    uint8_t found = 0;
    uint8_t idx;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(group_fc, SHR_E_PARAM);

    /* Fetch the corresponding stage info */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   stage_id, &stage_fc));

    /* Fetch the group segment start pointer */
    group_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    /* Loop through the group segment to get the
     * group info requested
     */
    for (idx =0;idx < stage_fc->num_groups; idx++) {
        if ((group_id == group_ptr->group_id)
             && (group_ptr->valid == 1)) {
            *group_fc = group_ptr;
            found = 1;
            break;
        }
        group_ptr = group_ptr + 1;
    }

    /* if group not found. */
    if (0 == found) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_stage_attr_stage_flags_get(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint32_t *stage_flags)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_flags, SHR_E_PARAM);

    *stage_flags = ltid_info->rm_more_info[0].flags;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_slices_count_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint8_t *num_slices)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_slices, SHR_E_PARAM);

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    *num_slices = ltid_info->rm_more_info[0].slice_count[pipe_id];
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_stage_attr_entries_per_slice_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t slice_id,
                                  int pipe_id,
                                  bcmptm_rm_tcam_group_mode_t grp_mode,
                                  uint32_t *num_entries_per_slice)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_entries_per_slice, SHR_E_PARAM);
    *num_entries_per_slice = 0;

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if (slice_id >= ltid_info->rm_more_info[0].slice_count[pipe_id]) {
       SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if ((ltid_info->hw_entry_info[grp_mode].num_key_rows == 0)
        && (ltid_info->hw_entry_info[grp_mode].num_data_rows
        == 0)) {
        *num_entries_per_slice = 0;
    } else {

        switch (grp_mode) {
            case BCMPTM_RM_TCAM_GRP_MODE_SINGLE:
                 *num_entries_per_slice =
                   ltid_info->rm_more_info[0].entry_count[pipe_id][slice_id];
                 break;
            case BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA:
            case BCMPTM_RM_TCAM_GRP_MODE_TRIPLE:
            case BCMPTM_RM_TCAM_GRP_MODE_QUAD:
                 *num_entries_per_slice =
                  ltid_info->rm_more_info[0].entry_count[pipe_id][slice_id];

                 if ((ltid_info->rm_more_info[0].flags)
                      & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
                     *num_entries_per_slice = (*num_entries_per_slice)/2;
                 }
                 break;
            case BCMPTM_RM_TCAM_GRP_MODE_DBLINTER:
                 if ((ltid_info->rm_more_info[0].flags)
                      & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
                      *num_entries_per_slice =
                      ltid_info->rm_more_info[0].entry_count[pipe_id][slice_id];
                  } else {
                      *num_entries_per_slice =
                       ltid_info->rm_more_info[0].entry_count[pipe_id][slice_id];

                      if ((ltid_info->rm_more_info[0].flags)
                          & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
                          *num_entries_per_slice = (*num_entries_per_slice)/2;
                      }
                  }
                  break;
            default:
                 SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }
exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_slice_start_end_index_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t slice_id,
                                  int pipe_id,
                                  uint32_t *start_index,
                                  uint32_t *end_index)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(start_index, SHR_E_PARAM);
    SHR_NULL_CHECK(end_index, SHR_E_PARAM);

    *start_index = 0;
    *end_index = 0;

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if ((slice_id >= ltid_info->rm_more_info[0].slice_count[pipe_id])
        || !(ltid_info->rm_more_info[0].slice_info[slice_id].valid)) {
       SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    *start_index = ltid_info->rm_more_info[0].slice_info[slice_id].start_index;
    *end_index = ltid_info->rm_more_info[0].slice_info[slice_id].end_index;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_pipes_count_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *num_pipes)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_pipes, SHR_E_PARAM);

    *num_pipes = ltid_info->rm_more_info[0].pipe_count;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_slice_bundle_size_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *slice_bundle_size)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_bundle_size, SHR_E_PARAM);

    *slice_bundle_size = ltid_info->rm_more_info[0].bundle_size;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint32_t *num_groups_per_pipe)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_groups_per_pipe, SHR_E_PARAM);


    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    *num_groups_per_pipe = ltid_info->rm_more_info[0].group_count[pipe_id];

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_fp_lt_config_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  int pipe_id,
                  bcmptm_rm_tcam_fp_stage_id_t stage_id,
                  bcmptm_rm_tcam_lt_config_t **lt_config)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    uint32_t num_groups_per_pipe = 0;
    bcmltd_sid_t ing_ltid;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fetch the corresponding stage info */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit,
                                   ltid, ltid_info,
                                   stage_id, &stage_fc));
    /* EM and IFP share same lt config info */
    if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
        if (ltid_info->rm_more_info->shared_lt_count) {
            ing_ltid = ltid_info->rm_more_info->shared_lt_info[0];
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit,
                                   ing_ltid, ltid_info,
                                   stage_id, &stage_fc));
        }
    }
    /* For global mode, use the memory created for pipe 0
     * As only either global mode or pipe mode can exists
     * at a gvien time
     */
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    /* get the number of groups in this pipe */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                          unit, ltid, ltid_info, pipe_id,
                          &num_groups_per_pipe));

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        num_groups_per_pipe = 0;
    }

    /* Fetch the slice segment start pointer */
    *lt_config = (bcmptm_rm_tcam_lt_config_t*) (((uint8_t *)stage_fc)
                       + stage_fc->lt_config_seg
                       + ((num_groups_per_pipe) * pipe_id
                       * sizeof(bcmptm_rm_tcam_lt_config_t)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_presel_entries_per_slice_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint8_t slice_id,
                                  uint32_t *num_presel_entries_per_slice)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_presel_entries_per_slice, SHR_E_PARAM);

    *num_presel_entries_per_slice = 0;

    /* For global mode, use the memory created for pipe 0
     * As only either global mode or pipe mode can exists
     * at a gvien time
     */
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if (slice_id >= ltid_info->rm_more_info[0].slice_count[pipe_id]) {
       SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (ltid_info->rm_more_info[0].flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        *num_presel_entries_per_slice =
           (ltid_info->rm_more_info[0].presel_count[pipe_id][slice_id]);
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_max_groups_per_mode_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  bcmptm_rm_tcam_group_mode_t grp_mode,
                                  uint32_t *num_groups_per_mode)
{

    int div_factor[BCMPTM_RM_TCAM_GRP_MODE_COUNT] = {1, 1, 2, 3, 2};
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_groups_per_mode, SHR_E_PARAM);

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }
    if ((ltid_info->hw_entry_info[grp_mode].num_key_rows == 0)
        && (ltid_info->hw_entry_info[grp_mode].num_data_rows
        == 0)) {
        *num_groups_per_mode = 0;
    } else {
        *num_groups_per_mode =
            ((ltid_info->rm_more_info[0].slice_count[pipe_id]
              / ltid_info->rm_more_info[0].bundle_size)
              * (ltid_info->rm_more_info[0].bundle_size
              / div_factor[grp_mode]));
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_max_action_resolution_ids_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint32_t *num_action_res_ids)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(num_action_res_ids, SHR_E_PARAM);

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    *num_action_res_ids = ltid_info->rm_more_info[0].num_action_res_ids;

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_tcam_max_entries_per_group_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  bcmptm_rm_tcam_entry_attrs_t *entry_attrs,
                                  bool auto_expansion,
                                  uint32_t *max_entries_per_group)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    uint32_t free_entries = 0;
    uint8_t slice_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid,
                                   ltid_info,
                                   entry_attrs->group_id,
                                   entry_attrs->stage_id,
                                   &fg));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_get(unit, ltid,
                                            ltid_info,
                                            (void *)entry_attrs,
                                            &free_entries));

    if (auto_expansion == FALSE) {
        *max_entries_per_group = free_entries - 1;
    } else {
        /* calculate the number of slices this group can expand to */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_fp_validate_group_auto_expansion(unit, ltid,
                                                            ltid_info,
                                                            entry_attrs,
                                                            &slice_count,
                                                            max_entries_per_group));
        /* get the number of entries that can be created
         * in this group mode with out auto expansion of group
         */
        *max_entries_per_group = free_entries + *max_entries_per_group - 1;
    }

 exit:
    SHR_FUNC_EXIT();

}
