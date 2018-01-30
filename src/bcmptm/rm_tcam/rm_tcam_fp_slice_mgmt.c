/*! \file rm_tcam_fp_slice_mgmt.c
 *
 * Low Level Functions for FP Slice Management
 * This file contains low level functions for FP based TCAMs
 * to do Slice allocate opertaions for Single/Double/Triple wide
 * groups.
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
#include "rm_tcam.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_fp.h"
#include "rm_tcam_fp_entry_mgmt.h"
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/bcmdrd_field.h>
#include "rm_tcam_fp_utils.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM
/* Maximum value supported for a LT partition in hardware. */
#define FP_MAX_LT_PART_PRIO (15)


/*******************************************************************************
 * Private Functions
 */
/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_reserve_single_slice
 * Purpose:
 *     Validate and return a free slice if it fits for
 *     Single or Intra Slice Double wide mode group.
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
bcmptm_rm_tcam_fp_group_reserve_single_slice(int unit, uint32_t group_mode,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice)
{
    uint8_t slice_idx;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice, SHR_E_PARAM);

    /* Loop through the slice segment and return the slice
     * number to use
     */
    for (slice_idx = 0; slice_idx < num_slices; slice_idx++) {
        /* check the in USE flag of slice */
        if (!(slice_fc->slice_flags & BCMPTM_SLICE_IN_USE)) {
            *slice = slice_idx;
            found = TRUE;
            break;
        }
        slice_fc = slice_fc + 1;
    }
    /* If no free slice is found,
     * return E_RESOURCE error
     */
    if (found == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    /* Mark the slice flag to in use
     * correct the number of entries in slice if needed
     */
    slice_fc->slice_flags |= BCMPTM_SLICE_IN_USE;
    if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_reserve_double_slice
 * Purpose:
 *     Validate and return primary slice idx if it can be
 *     used Double wide mode group.
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
bcmptm_rm_tcam_fp_group_reserve_double_slice(int unit, uint32_t group_mode,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice)
{
    uint8_t slice_idx;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    bool found = FALSE;
    uint8_t slice_bundle_size;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice, SHR_E_PARAM);

    slice_bundle_size = stage_fc->slice_bundle_size;

    /* Loop through the slice segment and return the slice
     * number to use
     */
    for (slice_idx = 0; slice_idx < (num_slices - 1); slice_idx++) {
        /*
         * Double wide group is created using consecutive slices within a memory
         * macro.But consecutive slices across a memory macro boundary cannot be
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
                    *slice = slice_idx;
                    found = TRUE;
                    break;
                }
            }
        }
        slice_fc = slice_fc + 1;
    }
    /* If no free slice is found,
     * return E_RESOURCE error
     */
    if (found == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    /* Mark the slice flag to in use
     * correct the number of entries in slice
     */
    /* primary slice */
    slice_fc->slice_flags |= BCMPTM_SLICE_IN_USE;
    if (((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) &&
         !(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART)) ||
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART)) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
    }
    slice_fc->right_slice_id_offset = 1;
    /* secondary slice */
    slice_next->slice_flags |= BCMPTM_SLICE_IN_USE;
    if (((group_mode == BCMPTM_RM_TCAM_GRP_MODE_QUAD) &&
         !(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART)) ||
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART)) {
        slice_next->num_entries =  slice_next->num_entries >> 1;
    }
    slice_next->left_slice_id_offset = -1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_reserve_triple_slice
 * Purpose:
 *     Validate and return primary slice idx if it can be
 *     used Double wide mode group.
 * Parameters:
 *     unit          - (IN  BCM device number.
 *     stage_fc      - (IN) Field stage structure.
 *     num_slices    - (IN) number of slices for this stage
 *     slice_fc      - (IN/OUT) Field Slice structure. Update flags
 *     slice         - (OUT) Primary slice id.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_reserve_triple_slice(int unit,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice)
{
    uint8_t slice_idx;
    bcmptm_rm_tcam_slice_t *slice_next = NULL;
    bcmptm_rm_tcam_slice_t *slice_next_next = NULL;
    bool found = FALSE;
    uint8_t slice_bundle_size;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice, SHR_E_PARAM);

    slice_bundle_size = stage_fc->slice_bundle_size;

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
         * macro.But consecutive slices across a memory macro boundary cannot be
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
                        *slice = slice_idx;
                        found = TRUE;
                        break;
                    }
                }
            }
        }
        slice_fc = slice_fc + 1;
    }
    /* If no free slice is found,
     * return E_RESOURCE error
     */
    if (found == FALSE) {
       SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    /* Mark the slice flag to in use
     * correct the number of entries in slice
     */
    /* primary slice */
    slice_fc->slice_flags |= BCMPTM_SLICE_IN_USE;
    if (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
        slice_fc->num_entries = slice_fc->num_entries >> 1;
    }
    slice_fc->right_slice_id_offset = 1;

    /* Secondary slice */
    slice_next->slice_flags |= BCMPTM_SLICE_IN_USE;
    if (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
        slice_next->num_entries = slice_next->num_entries >> 1;
    }
    slice_next->left_slice_id_offset = -1;
    slice_next->right_slice_id_offset = 1;

    /* Third slice */
    slice_next_next->slice_flags |= BCMPTM_SLICE_IN_USE;
    if (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) {
        slice_next_next->num_entries = slice_next_next->num_entries >> 1;
    }
    slice_next_next->left_slice_id_offset = -1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_reserve_slice
 * Purpose:
 *     Validate and return primary slice idx based on group mode
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
bcmptm_rm_tcam_fp_group_reserve_slice(int unit, uint32_t group_mode,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc,
                               int8_t num_slices,
                               bcmptm_rm_tcam_slice_t *slice_fc,
                               uint8_t *slice)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Input params check */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice, SHR_E_PARAM);

    if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode) {
        /* Return E_CONFIG if the requested group mode is Intra-Slice
         * Double and Slices are not Intra Slice capable.
         */
        if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == group_mode
             && !(slice_fc->slice_flags & BCMPTM_SLICE_INTRASLICE_CAPABLE)) {
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
        }
        /* Reserve single slice for this new Field Group. */
        rv = bcmptm_rm_tcam_fp_group_reserve_single_slice(unit, group_mode,
                             num_slices, slice_fc, slice);
    } else if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == group_mode
        || (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode
        && stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE)) {
        /* Return E_CONFIG if the requested group mode is Quad
         * and Slices are not Intra Slice capable.
         */
        if (BCMPTM_RM_TCAM_GRP_MODE_QUAD == group_mode
             && !(slice_fc->slice_flags & BCMPTM_SLICE_INTRASLICE_CAPABLE)) {
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
        }
        /* Reserve single slice for this new Field Group. */
        rv = bcmptm_rm_tcam_fp_group_reserve_double_slice(unit, group_mode,
                             stage_fc, num_slices, slice_fc, slice);
    } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == group_mode
            && stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        /* Reserve single slice for this new Field Group. */
        rv = bcmptm_rm_tcam_fp_group_reserve_triple_slice(unit,
                             stage_fc, num_slices, slice_fc, slice);
    } else {
        rv = SHR_E_CONFIG;
    }

    /* No resources */
    if (rv != SHR_E_NONE) {
        SHR_RETURN_VAL_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_check_slice_sharing
 * Purpose:
 *     Validate and return primary group id and slice_sharing flag
 *     if the newly created group can share the resources of
 *     existing group.
 * Parameters:
 *    unit           - (IN)  BCM device number.
 *    ltid           - (IN)  Logical Table enum that is accessing the table
 *    entry_attr     - (IN)  Entry attrs structure
 *    slice_fc       - (IN/OUT) Field Slice structure. Update flags
 *    grp_ptr        - (IN/OUT) Field group structure. Update flags
 *    pri_group_id   - (OUT) Primary group index
 *    slice_shared   - (OUT) Slice Shared TRUE/FALSE
 * Returns:
 *     SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_check_slice_sharing(int unit, bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                     bcmptm_rm_tcam_slice_t *slice_fc,
                     bcmptm_rm_tcam_fp_group_t *grp_ptr,
                     bool *slice_shared,
                     int8_t *pri_group_id)
{
    uint32_t idx;
    bool sharing_feasible = FALSE;
    uint32_t num_presels_per_slice = 0;
    uint32_t total_presel_entries = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);


    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid,
                                   ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_presel_entries_per_slice_get(
                                             unit, ltid,
                                             ltid_info,
                                             entry_attr->pipe_id,
                                             0,
                                             &num_presels_per_slice));
    }

    sharing_feasible = 0;
    /* over all presel entries cant go beyond 32 */
    /* if group has only default presel entry, tht group cant be shared. */
    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if ((grp_ptr[idx].valid == TRUE) &&
            (grp_ptr[idx].pipe_id == entry_attr->pipe_id)) {

            /* For EM no need to same priority groups. groups should belong
               to same slice id. slice id is recevied from BCMFP */
            if (((stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH
                  && grp_ptr[idx].primary_slice_id == entry_attr->slice_id)
                || (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)
                  && grp_ptr[idx].priority == entry_attr->group_prio))
                && grp_ptr[idx].group_mode == entry_attr->group_mode) {

                if (grp_ptr[idx].flags & BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT
                    && stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
                    SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
                }
                if (grp_ptr[idx].flags & BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT) {
                    continue;
                }
                if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
                    if (grp_ptr[idx].group_ltid == entry_attr->group_ltid) {
                        /* groups can be shared */
                        sharing_feasible = 1;
                        break;
                    }
                } else {
                    /* number presel entries cant go beyond 32 */
                    total_presel_entries = grp_ptr[idx].num_presel_entries
                                           + entry_attr->num_presel_ids;
                    if (total_presel_entries <= num_presels_per_slice) {
                        /* groups can be shared */
                        sharing_feasible = 1;
                        break;
                    }
                }
            }
        }
    }
    if (sharing_feasible) {
        *slice_shared = TRUE;
        idx = *pri_group_id
            = slice_fc[grp_ptr[idx].primary_slice_id].primary_grp_id;

        while (grp_ptr[idx].next_shared_group_id != -1) {
            idx = grp_ptr[idx].next_shared_group_id;
        }
        grp_ptr[idx].next_shared_group_id = entry_attr->group_id;
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_lt_priority_alloc
 * Purpose:
 *    Allocate logical table action priority for the group.
 * Parameters:
 *    unit        - (IN) BCM device number.
 *    lt_config   - (IN/OUT) update group's priority.
 *    num_groups  - (IN) number of groups per pipe
 *    fg_priority - (IN) group's input priority.
 *    lt_id       - (IN) Logical Table Identifier.
 * Returns:
 *    SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_lt_priority_alloc(int unit,
                                          bcmptm_rm_tcam_lt_config_t *lt_config,
                                          uint16_t num_groups,
                                          int fg_priority, int lt_id)
{
    int lt_idx;                                 /* Logical table iterator.   */
    bcmptm_rm_tcam_lt_config_t *lt_info = NULL;
                                                /* LT information.           */
    int action_prio = 0;                        /* LT action priority value. */
    int idx;                                    /* Iterator.                 */

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(lt_config, SHR_E_PARAM);

    /* make a copy of the lt_config structure to sort */
    SHR_ALLOC(lt_info, num_groups * sizeof(bcmptm_rm_tcam_lt_config_t),
                      "LT Config info");

    for (idx =0; idx < num_groups; idx++){
        sal_memcpy(&lt_info[idx], &lt_config[idx],
                           sizeof(bcmptm_rm_tcam_lt_config_t));
    }
    /*
     * Initialize LT information for the new logical table ID, for it to be
     * in the correct location after the sort.
     */
    lt_info[lt_id].priority = fg_priority;
    lt_info[lt_id].lt_action_pri = 0;
    lt_info[lt_id].valid = TRUE;

    /* Sort the logical tables array based on group priority value. */
    sal_qsort(lt_info, num_groups, sizeof(bcmptm_rm_tcam_lt_config_t),
              bcmptm_rm_tcam_fp_lt_group_priority_compare);
    /*
     * Assign LT action priority based on the position of LT ID in the sorted
     * array.
     */
    action_prio = (num_groups - 1);
    for (idx = (num_groups - 1); idx >= 0; idx--) {
        if (FALSE == lt_info[idx].valid) {
            continue;
        }
        lt_info[idx].lt_action_pri = action_prio--;
    }

    for (idx = 0; idx < num_groups; idx++) {
        if (FALSE == lt_info[idx].valid) {
            continue;
        }
        lt_idx = lt_info[idx].lt_id;
        lt_config[lt_idx].lt_action_pri =
                    lt_info[idx].lt_action_pri;
    }

    SHR_FREE(lt_info);
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_priority_set
 * Purpose:
 *    Update lt_config structure with group priority
 *    Also set's group's slice's enable glags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    stage_fc       - (IN) Field stage structure.
 *    pri_slice_idx  - (IN) Primary slice index
 *    entry_attr     - (IN) Entry attrs structure
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_priority_set(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                              uint8_t pri_slice_idx,
                              bcmptm_rm_tcam_slice_t *slice_fc,
                              bool slice_shared)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    bcmptm_rm_tcam_fp_group_t  *pri_fg = NULL;
                               /* Field Group structure pointer. */
    int parts_count = -1;                         /* Number of entry parts.   */
    int part;
    uint8_t slice, slice_prev, slice_number;      /* LT Slice number.         */
    int lt_part_prio = FP_MAX_LT_PART_PRIO;       /* LT part priority value.  */
    uint32_t num_groups_per_pipe;
    int16_t pri_grp_id = -1;
    uint8_t  intraslice = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit,
                 ltid,
                 ltid_info,
                 entry_attr->pipe_id, stage_fc->stage_id,
                 &lt_config));

    /* Get number of groups per pipe */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                          unit, ltid, ltid_info,
                          entry_attr->pipe_id,
                          &num_groups_per_pipe));

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        /* Allocate LT action priority. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_priority_alloc(unit,
                                   lt_config,
                                   num_groups_per_pipe,
                                   entry_attr->group_prio,
                                   entry_attr->group_ltid));
        lt_config[entry_attr->group_ltid].valid = TRUE;
        lt_config[entry_attr->group_ltid].priority = entry_attr->group_prio;
        lt_config[entry_attr->group_ltid].group_ltid = entry_attr->group_ltid;
        lt_config[entry_attr->group_ltid].pipe_id = entry_attr->pipe_id;
    } else {
        /* Allocate LT action priority. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_priority_alloc(unit,
                                   lt_config,
                                   stage_fc->num_groups,
                                   entry_attr->group_prio,
                                   entry_attr->group_id));
        lt_config[entry_attr->group_id].valid = TRUE;
        lt_config[entry_attr->group_id].priority = entry_attr->group_prio;
        lt_config[entry_attr->group_id].group_ltid = entry_attr->group_ltid;
        lt_config[entry_attr->group_id].pipe_id = entry_attr->pipe_id;
    }

    if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        /* Get number of entry parts. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(
                                                   unit,
                                                   entry_attr->group_mode,
                                                   stage_fc->flags,
                                                   &parts_count));
        slice = pri_slice_idx;
        /* if the ressources are shared, get the primary_group_id */
        if (TRUE == slice_shared) {
            /* get the primary grp id from primary slice */
            pri_grp_id = slice_fc[slice].primary_grp_id;
            /* Initialize field group pointer. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid,
                                                   ltid_info,
                                                   pri_grp_id,
                                                   entry_attr->stage_id,
                                                   &pri_fg));
        }

        if (!(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) &&
            (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
            (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
            || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
            intraslice = 1;
            parts_count = parts_count >> intraslice;
        }
        do
        {
            slice_prev = slice;
            /* Iterate over entry parts and program hardware for each slice. */
            for (part = 0; part < parts_count; part++) {

                /* Get slice number to which the entry part belongs to. */
                slice_number = slice + part;
                /* if groups are sharing resources, use the lt_pri from
                 * primary group
                 */
                if ((TRUE == slice_shared) && (pri_fg != NULL)) {
                    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                        lt_config[entry_attr->group_ltid].lt_part_pri[slice_number]
                          = lt_config[pri_fg->group_ltid].lt_part_pri[slice_number];
                    } else {
                        lt_config[entry_attr->group_id].lt_part_pri[slice_number]
                          = lt_config[pri_fg->group_id].lt_part_pri[slice_number];
                    }

                } else {

                    /* Set slice mode in hardware based on Group mode/flags. */
                    SHR_IF_ERR_VERBOSE_EXIT(
                        bcmptm_rm_tcam_fp_slice_enable_set(unit,
                                            ltid,
                                            ltid_info,
                                            entry_attr->pipe_id,
                                            entry_attr->stage_id,
                                            entry_attr->group_mode,
                                            slice_number, 1));
                    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                        lt_config[entry_attr->group_ltid].lt_part_pri[slice_number]
                                                    = lt_part_prio;
                    } else {
                        lt_config[entry_attr->group_id].lt_part_pri[slice_number]
                                                    = lt_part_prio;
                    }
                }
                /*
                 * Initialize slice free entries count value to number of
                 * entries in the slice.
                 */
                if (-1 == slice_fc[slice_number].primary_grp_id) {
                    slice_fc[slice_number].primary_grp_id =
                                   entry_attr->group_id;
                    slice_fc[slice_number].free_entries =
                                   slice_fc[slice_number].num_entries;
                    slice_fc[slice_number].free_presel_entries =
                            slice_fc[slice_number].num_presel_entries;
                }

                /* Update slice LT_ID mapping in group's slice. */
                if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                    lt_config[entry_attr->group_ltid].lt_part_map
                      |= (1 << slice_number);
                } else {
                    lt_config[entry_attr->group_id].lt_part_map
                      |= (1 << slice_number);
                }
            }

            slice = slice + slice_fc[slice].next_slice_id_offset;

        } while (slice != slice_prev);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_sindex_remap(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               void *entry_attr,
                               uint8_t slice_id,
                               uint32_t sindex,
                               uint32_t *remap_index)
{
    uint8_t idx = 0;
    uint32_t num_entries = 0;
    uint32_t slice_local_index = 0;
    uint32_t num_entries_per_slice = 0;
    bcmptm_rm_tcam_entry_attrs_t *fp_entry_attr = entry_attr;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(remap_index, SHR_E_PARAM);

    *remap_index = sindex;

    /* Accumulate the number of entries in all slices
     * till the target slice_id. */
    for (idx = 0; idx < slice_id; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                        unit, ltid, ltid_info,
                                        idx, fp_entry_attr->pipe_id, 0,
                                        &num_entries_per_slice));

            num_entries += num_entries_per_slice;
    }

    /* Get the number of entries in the target slice_id. */
    num_entries_per_slice = 0;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                   unit, ltid, ltid_info,
                                   slice_id, fp_entry_attr->pipe_id, 0,
                                   &num_entries_per_slice));
    if (num_entries) {
        slice_local_index = sindex % num_entries;
    } else {
        slice_local_index = sindex;
    }

    if (sindex % 2) {
       /* remap_index is addition of
        * (Sum of the entries in all slices till the targte slice),
        * (Index of the entry local to target slice),
        * (Number of entries in target slice divided by 2)
        */

       *remap_index = num_entries +
                      (slice_local_index / 2) +
                      (num_entries_per_slice / 2);
    } else {
       /* remap_index is addition of
        * (Sum of the entries in all slices till the targte slice),
        * (Index of the entry local to target slice)
        */
       *remap_index = num_entries +
                      (slice_local_index / 2);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_lt_part_prio_value_get
 * Purpose:
 *     Configure and return a LT Partition Priority
 *     for the given LT ID from a group.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     lt_config    - (IN)  LT config Structure.
 *     fg_ltid      - (IN)  Field group LT id.
 *     num_slices   - (IN)  Number of slices
 *     lt_part_prio - (OUT) LT Partition priority value.
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_group_lt_part_prio_value_get(int unit,
                                  bcmptm_rm_tcam_lt_config_t *lt_config,
                                  uint8_t fg_ltid, uint8_t num_slices,
                                  uint8_t *lt_part_prio)
{
    int part_prio = FP_MAX_LT_PART_PRIO;
    int rv = SHR_E_NONE;
    uint8_t slice;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(lt_part_prio, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(lt_config, SHR_E_PARAM);

    /* Parse the existing lt config for the group
     * and get the least lt part prio existing
     */
    for (slice = 0; slice < num_slices; slice++) {
        if (lt_config[fg_ltid].lt_part_pri[slice] != 0) {
            if (part_prio > lt_config[fg_ltid].lt_part_pri[slice]) {
                part_prio = lt_config[fg_ltid].lt_part_pri[slice];
            }
        }
    }

    /* Assign the least priority to the latest slice. */
    *lt_part_prio = part_prio - 1;
    if (*lt_part_prio <= 0) {
       rv = SHR_E_INTERNAL;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_partition_priority_set
 * Purpose:
 *    Update lt_config structure with partition priority
 *    Also set's group's slice's enable flags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    stage_fc       - (IN) Field stage structure.
 *    entry_attr          - (IN) Entry attrs structure
 *    new_slice_idx  - (IN) New slice index
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_partition_priority_set(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                              uint8_t new_slice_idx,
                              bcmptm_rm_tcam_slice_t *slice_fc)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    int parts_count = -1;                         /* Number of entry parts.   */
    int part;
    uint8_t slice_number;                         /* LT Slice number.         */
    uint8_t pri_slice_id;                         /* LT Slice number.         */
    uint8_t lt_part_prio = 0;                     /* LT part priority value.  */
    uint8_t num_slices;
    bcmptm_rm_tcam_fp_group_t  *grp_ptr = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    int8_t next_fg_id = -1;
    uint8_t num_groups = 0;
    uint8_t  intraslice = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid, ltid_info,
                 entry_attr->pipe_id, stage_fc->stage_id,
                 &lt_config));
    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));
    /* get number of slices for this pipe and stage */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_slices_count_get(unit, ltid,
                         ltid_info,
                         entry_attr->pipe_id,
                         &num_slices));
    /* calculate/get LT partition priority for new slice. */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_part_prio_value_get(unit,
                              lt_config,
                              entry_attr->group_ltid,
                              num_slices, &lt_part_prio));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_part_prio_value_get(unit,
                              lt_config,
                              entry_attr->group_id, num_slices, &lt_part_prio));
    }

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                       entry_attr->group_mode, stage_fc->flags,
                                       &parts_count));

    if (!(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
        intraslice = 1;
        parts_count = parts_count >> intraslice;
    }

    /* Initialize field group pointer. */
    grp_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attr->group_id,
                                   entry_attr->stage_id,
                                   &fg));

    /* get the primary slice of the group */
    pri_slice_id = fg->primary_slice_id;

    /* get the first group id which is primary group sharing the slices */
    next_fg_id = slice_fc[pri_slice_id].primary_grp_id;

    do {
        fg =  (grp_ptr + next_fg_id);

        /* Iterate over entry parts and program hardware for each slice. */
        for (part = 0; part < parts_count; part++) {

            /* Get slice number to which the entry part belongs to. */
            slice_number = new_slice_idx + part;

            if (0 == num_groups) {
                /* Set slice mode in hardware based on Group mode/flags. */
                SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_enable_set(unit, ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->stage_id,
                                          fg->group_mode,
                                          slice_number, 1));
            }
            /*
             * Initialize the primary_group_id for the slice.
             * Initialize slice free entries count value to number of
             * entries in the slice.
             */
            if (-1 == slice_fc[slice_number].primary_grp_id) {
                slice_fc[slice_number].primary_grp_id =
                                entry_attr->group_id;
                slice_fc[slice_number].free_entries =
                               slice_fc[slice_number].num_entries;
                slice_fc[slice_number].free_presel_entries =
                               slice_fc[slice_number].num_presel_entries;
            }

            if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {

                lt_config[fg->group_ltid].lt_part_map |= (1 << slice_number);
                lt_config[fg->group_ltid].lt_part_pri[slice_number]
                  = lt_part_prio;
                lt_config[fg->group_ltid].group_ltid = fg->group_ltid;
                lt_config[fg->group_ltid].pipe_id = fg->pipe_id;
            } else {

                lt_config[fg->group_id].lt_part_map |= (1 << slice_number);
                lt_config[fg->group_id].lt_part_pri[slice_number]
                  = lt_part_prio;
                lt_config[fg->group_id].group_ltid = fg->group_ltid;
                lt_config[fg->group_id].pipe_id = fg->pipe_id;
            }
        }

        next_fg_id = fg->next_shared_group_id;

        /* Install LT partition Priority. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                           ltid, ltid_info, fg->group_ltid,
                                           entry_attr->pipe_id,
                                           entry_attr->stage_id));

        /* update the num entries in group structure */
        fg->num_entries = fg->num_entries +
                                 slice_fc[new_slice_idx].num_entries;
        num_groups++;

    } while (next_fg_id != -1);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_partition_priority_reset
 * Purpose:
 *    Reset lt_config structure with partition priority
 *    Also Reset's group's slice's enable flags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    entry_attr     - (IN) Entry attrs structure
 *    new_slice_idx  - (IN) New slice index
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

static int
bcmptm_rm_tcam_fp_group_partition_priority_reset(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                              uint8_t new_slice_idx,
                              bcmptm_rm_tcam_slice_t *slice_fc,
                              uint8_t group_compress)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    int parts_count = -1;      /* Number of entry parts.   */
    int part;
    uint8_t slice_number;      /* LT Slice number.         */
    uint8_t lt_part_prio = 0;  /* LT part priority value.  */
    uint8_t num_slices;
    int prev_slice_offset;
    uint32_t num_entries_per_slice = 0;
    uint32_t num_presels_per_slice = 0;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *pri_fg = NULL;
    uint8_t intraslice = 0;
    int32_t group_id = -1;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid, ltid_info,
                                        entry_attr->pipe_id,
                                        entry_attr->stage_id,
                                        &lt_config));
    /* get number of slices for this pipe and stage */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slices_count_get(unit, ltid, ltid_info,
                                        entry_attr->pipe_id,
                                        &num_slices));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                        entry_attr->group_mode,
                                        stage_fc->flags,
                                        &parts_count));

    if (!(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
        intraslice = 1;
        parts_count = parts_count >> intraslice;
    }

    /* Iterate over entry parts and program hardware for each slice. */
    for (part = 0; part < parts_count; part++) {

        /* Get slice number to which the entry part belongs to. */
        slice_number = new_slice_idx + part;

        /* to reset the num_entries in slice, we use the default value
         * of group_mode as 0 to get 512 entries
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             slice_number, entry_attr->pipe_id,
                                             0,
                                             &num_entries_per_slice));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_presel_entries_per_slice_get(
                                             unit, ltid, ltid_info,
                                             entry_attr->pipe_id,
                                             slice_number,
                                             &num_presels_per_slice));
        /*
         * Initialize slice free entries count value to number of
         * entries in the slice.
         */
        if (slice_fc[slice_number].free_entries ==
                            slice_fc[slice_number].num_entries) {
            if (1 == group_compress) {
                group_id = slice_fc[slice_number].primary_grp_id;
                while (group_id != -1) {

                    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit,
                                       ltid, ltid_info,
                                       (uint32_t) group_id,
                                       entry_attr->stage_id,
                                       &pri_fg));
                    /* decrease the group's num_entires by
                     * slice's num_entires.
                     */
                    if (part == 0) {
                        pri_fg->num_entries = pri_fg->num_entries -
                                     slice_fc[slice_number].num_entries;
                    }
                    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                        lt_config[pri_fg->group_ltid].lt_part_map
                                            &= ~(1 << slice_number);
                        lt_config[pri_fg->group_ltid].lt_part_pri[slice_number]
                                             = lt_part_prio;
                    } else {
                        lt_config[group_id].lt_part_map
                                            &= ~(1 << slice_number);
                        lt_config[group_id].lt_part_pri[slice_number]
                                             = lt_part_prio;
                    }
                    group_id = pri_fg->next_shared_group_id;
                }
            }
            /* Set slice mode in hardware based on Group mode/flags. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_slice_enable_set(unit, ltid, ltid_info,
                              entry_attr->pipe_id,
                              entry_attr->stage_id,
                              entry_attr->group_mode,
                              slice_number, 0));
            slice_fc[slice_number].primary_grp_id = -1;
            slice_fc[slice_number].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
            slice_fc[slice_number].num_entries = num_entries_per_slice;
            slice_fc[slice_number].free_entries = num_entries_per_slice;
            slice_fc[slice_number].num_presel_entries = num_presels_per_slice;
            slice_fc[slice_number].free_presel_entries = num_presels_per_slice;

            /* Adjust slice offsets */
            slice_fc[slice_number].next_slice_id_offset = 0;
            prev_slice_offset = slice_fc[slice_number].prev_slice_id_offset;

            if (prev_slice_offset !=0) {
                slice_fc[slice_number + prev_slice_offset].next_slice_id_offset
                  = 0;
                slice_fc[slice_number].prev_slice_id_offset = 0;
            }
            slice_fc[slice_number].left_slice_id_offset = 0;
            slice_fc[slice_number].right_slice_id_offset = 0;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_per_group_compress
 * Purpose:
 *     Compress field group entries in order to free slices used by the group.
 * Paramters:
 *     unit          - (IN) BCM device number
 *     ltid          - (IN) Logical Table enum that is accessing the table
 *     fg            - (IN) Field group structure
 *     slice_ptr     - (IN) Field slice structure.
 *     entry_info    - (IN) Field entry structure.
 *     entry_attr    - (IN) Entry attrs structure
 *
 * Returns:
 *     SHR_E_XXX
 */
static int
bcmptm_rm_tcam_fp_per_group_compress(int unit,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        bcmptm_rm_tcam_fp_group_t *fg,
                        bcmptm_rm_tcam_slice_t *slice_ptr,
                        bcmptm_rm_tcam_prio_entry_info_t *entry_info,
                        bcmptm_rm_tcam_entry_attrs_t *entry_attr)
{
    int eidx;                   /* Empty slot index.                */
    uint32_t idx;                    /* Slice entry iterator.            */
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint8_t count_free_slices = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_info, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_ptr, SHR_E_PARAM);

    eidx = -1;
    /* set tbl Instance depending on pipe number*/
    pt_dyn_info.tbl_inst = entry_attr->pipe_id;

    for (idx = 0; idx < fg->num_entries; idx++) {
        /* Find an empty slot. */
        if (entry_info[idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
            if (eidx == -1) {
                eidx = idx;
             }
            continue;
        }
        if (eidx != -1) {
            /*
             * move the current entry to empty index found
             * Move the entry in hardware.
             */
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_entry_move(unit, ltid, ltid_info, 0,
                                                    idx, eidx,
                                                    (void *)entry_attr,
                                                    &pt_dyn_info));
             /* increment the free count for idx logical id slice */
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_incr(unit,
                                                     ltid, ltid_info, idx,
                                                     (void *)entry_attr));
             /* decrement the free count for eidx logical id slice */
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_decr(unit,
                                                     ltid, ltid_info, eidx,
                                                     (void *)entry_attr));
             idx = eidx;
             eidx = -1;
        }
    }
    /* Free the unused slices */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_free_unused_slices(unit, ltid,
                                            ltid_info, entry_attr,
                                            fg, slice_ptr,
                                            &count_free_slices,
                                            1/* group compress flag */));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_segment_compress(unit,
                                            ltid, ltid_info, fg->group_id,
                                           entry_attr->pipe_id,
                                          entry_attr->stage_id,
                                          count_free_slices, 0));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief To convert logical entry_id to
 * \ Physical entry id
 *
 * \param [in]  unit Logical device id
 * \param [in]  ltid Logical Table enum that is accessing the table
 * \param [in]  entry attrs structure
 * \param [in]  logical entry idx.
 * \param [in]  width part number.
 * \param [out] depth part number.
 * \param [out] physical entry idx.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
int bcmptm_rm_tcam_fp_tindex_to_sindex(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                                 uint32_t rm_flags,
                                 uint32_t logical_idx,
                                 uint8_t width_part,
                                 uint8_t *depth_part,
                                 uint32_t *phy_idx)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    uint8_t slice_id;          /* Slice ID */
    uint8_t prev_slice_id;     /* Slice ID */
    uint8_t num_slices;        /* Number of Slices */
    uint32_t num_entries_per_slice = 0;
    uint32_t num_data_entries_per_slice = 0;
    uint8_t  intraslice = 0;
    uint32_t start_index;
    uint32_t end_index;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attr->group_id, entry_attr->stage_id,
                                   &fg));

    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &slice_fc));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));

    /* Detect if a legacy arch based intraslice wit two TCAM's */
    if (!(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
        intraslice = 1;
    }

    slice_id = fg->primary_slice_id + (width_part >> intraslice);

    *depth_part = 0;

    prev_slice_id = -1;

    do {
        /* check whether the entry is presel entry */
        if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
            /* get the number of presel entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                       unit, ltid, ltid_info, entry_attr->pipe_id,
                                       slice_id,
                                       &num_entries_per_slice));
            num_data_entries_per_slice = num_entries_per_slice;
        } else {
            /* get the number of entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                       unit, ltid, ltid_info,
                                       slice_id, entry_attr->pipe_id, 0,
                                       &num_data_entries_per_slice));
            /* get the number of entries per slice, depending on 80-bit mode
             * or 160 bit mode */
            if ((BCMPTM_RM_TCAM_GRP_MODE_SINGLE != entry_attr->group_mode
                 && stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) ||
                   intraslice) {
                num_entries_per_slice = num_data_entries_per_slice >> 1;
            } else {
                num_entries_per_slice = num_data_entries_per_slice;
            }
        }

        /* calculate the depth part and correct slice id */
        if (logical_idx < num_entries_per_slice) {
            break;
        }

        if (!SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
            (*depth_part)++;
        }

        prev_slice_id = slice_id;

        slice_id = slice_id + slice_fc[slice_id].next_slice_id_offset;
        logical_idx = logical_idx - num_entries_per_slice;

    } while(prev_slice_id != slice_id);

    if (prev_slice_id == slice_id) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_stage_attr_slice_start_end_index_get(unit, ltid,
                                                            ltid_info, slice_id,
                                                            entry_attr->pipe_id,
                                                            &start_index,
                                                            &end_index));

    /**************************************************************************
     *        IFP TCAM                              |  IFP PT
     **************************************************************************
     *        |Presel    |Presel    |               |Presel  |Presel    |
     *        |Intra     |non-Intra |Legacy         |Intra   |non-Intra |Legacy
     **************************************************************************
     *SINGLE  |idx       |idx       |idx            |idx     |idx       |idx
     **************************************************************************
     *INTRA   |idx/2     |NA        |idx,           |idx     |NA        |idx
     *                              |idx + se/2
     **************************************************************************
     *INTER   |idx1/2,   |idx1,     |idx1, idx2     |idx1    |idx1      |idx1
              |idx2/2    |idx2
     **************************************************************************
     *TRIPLE  |idx1/2,   |idx1,     |idx1,idx1+se/2 |idx1    |idx1      |idx1
     *        |idx2/2,   |idx2,
     *        |idx3/3    |idx3
     **************************************************************************
     *QUAD    |NA        |NA        |idx1, idx1+se/2|idx1    |idx1      |idx1
     *                              |idx2, idx2+se/2
 */

    /**************************************************************************
     *        VFP TCAM                 |  VFP PT
     **************************************************************************
     *                         Legacy
     **************************************************************************
     *SINGLE  |idx                     |idx
     **************************************************************************
     *INTRA   |idx,                    |idx
     *        |idx + se/2
     **************************************************************************
     *INTER   |idx1,                   |idx1
     *        |idx2
     **************************************************************************
     *TRIPLE  |NA                      |NA
     **************************************************************************
     *QUAD    |idx1, idx1+se/2         |idx1
     *        |idx2, idx2+se/2
 */

    /**************************************************************************
     *        EFP TCAM                 |  EFP PT
     **************************************************************************
     *                         Legacy
     **************************************************************************
     *SINGLE  |idx                     |idx
     **************************************************************************
     *INTRA   |NA                      |NA
     **************************************************************************
     *INTER   |idx1,                   |idx1
     *        |idx2
     **************************************************************************
     *TRIPLE  |NA                      |NA
     **************************************************************************
     *QUAD    |NA                      |NA
 */

    /* calculate physical index */
    /* For Wide mode number of entries in each slice is half
     * but not for policy table */
    if ((rm_flags != BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW)
        && (BCMPTM_RM_TCAM_GRP_MODE_SINGLE != entry_attr->group_mode)
         && (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE)
         && (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART)) {
        *phy_idx = (start_index / 2) + logical_idx;
    } else {
        *phy_idx = start_index + logical_idx;
    }

    /* PRESEL will be inserted with LT as main TCAM.
     * So current ltid_info can't be relied on.
     */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        *phy_idx = (slice_id * num_data_entries_per_slice) + logical_idx;
    } else if (intraslice && width_part % 2) {
        *phy_idx = *phy_idx + ((end_index - start_index + 1) / 2);
    }

    /* Remap the Index if required. To handle Special HW case.
     * RMTCAM index allocation algorithm always considers
     * lowest index as highest prioirty. remap allocated physical index
     * as per HW.
     */
    if (stage_fc->flags & BCMPTM_STAGE_ENTRY_INDEX_REMAP &&
        entry_attr->group_mode == BCMPTM_RM_TCAM_GRP_MODE_SINGLE &&
        !SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_fp_sindex_remap(unit,
                                           ltid, ltid_info,
                                           (void *)entry_attr,
                                           slice_id,
                                           *phy_idx,
                                           phy_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_priority_reset
 * Purpose:
 *    Reset lt_config structure when a group is deleted
 *    Also clear group's slice's enable flags
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    stage_fc       - (IN) Field stage structure.
 *    pri_slice_idx  - (IN) Primary slice index
 *    fg             - (IN) Field group structure.
 *    slice_fc       - (IN) Field slice structure.
 * Returns:
 *    SHR_E_XXX
 */

int
bcmptm_rm_tcam_fp_group_priority_reset(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_fp_group_t *fg,
                              bcmptm_rm_tcam_slice_t *slice_fc)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    int parts_count = -1;    /* Number of entry parts.   */
    int part;
    uint8_t slice_number;    /* LT Slice number.         */
    uint8_t depth_slice_idx; /* LT Slice number.         */
    uint8_t depth_part;      /* LT Slice number.         */
    uint8_t intraslice = 0;
    int curr_group_id;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid, ltid_info,
                                        pipe_id, stage_fc->stage_id,
                                        &lt_config));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                       fg->group_mode, stage_fc->flags,
                                       &parts_count));

    if (!(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == fg->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == fg->group_mode)) {
        intraslice = 1;
        parts_count = parts_count >> intraslice;
    }

    depth_slice_idx = fg->primary_slice_id;
    depth_part = 0;

    do {

        depth_slice_idx = depth_slice_idx + depth_part;
        /* Iterate over entry parts and program hardware for each slice. */
        for (part = 0; part < parts_count; part++) {

            /* Get slice number to which the entry part belongs to. */
            slice_number = depth_slice_idx + part;

            if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                lt_config[fg->group_ltid].lt_part_pri[slice_number] = 0;
            } else {
                lt_config[fg->group_id].lt_part_pri[slice_number] = 0;
            }

            /* update the primary group id, if primary group
             * is getting deleted
             */
             curr_group_id = fg->group_id;
            if (slice_fc[slice_number].primary_grp_id == curr_group_id
                && fg->next_shared_group_id != -1) {
                slice_fc[slice_number].primary_grp_id =
                                               fg->next_shared_group_id;
            }
        }

        depth_part = slice_fc[depth_slice_idx].next_slice_id_offset;

    } while(depth_part !=0);

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        lt_config[fg->group_ltid].valid = FALSE;
        lt_config[fg->group_ltid].priority = 0;
        lt_config[fg->group_ltid].lt_part_map = 0;
        lt_config[fg->group_ltid].lt_action_pri = 0;
        lt_config[fg->group_ltid].group_ltid = 0;
        lt_config[fg->group_ltid].pipe_id = 0;
    } else {
        lt_config[fg->group_id].valid = FALSE;
        lt_config[fg->group_id].priority = 0;
        lt_config[fg->group_id].lt_part_map = 0;
        lt_config[fg->group_id].lt_action_pri = 0;
        lt_config[fg->group_id].group_ltid = 0;
        lt_config[fg->group_id].pipe_id = 0;
    }
    /* Install LT partition Priority. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                 ltid, ltid_info, fg->group_ltid,
                                 pipe_id, stage_fc->stage_id));
    fg->group_ltid = -1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_group_free_unused_slices
 * Purpose:
 *     Unallocate group unused slices.
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     ltid           - (IN) Logical Table enum that is accessing the table
 *     entry_attr     - (IN) Entry attr structure.
 *     fg             - (IN) Field group structure.
 *     slice_ptr      - (IN) Field slice structure.
 *     clear_presel   - (IN) Flag to free presel entires
 * Returns:
 *     SHR_E_XXX
 */
int
bcmptm_rm_tcam_fp_group_free_unused_slices(int unit,
                            bcmltd_sid_t ltid,
                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                            bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                            bcmptm_rm_tcam_fp_group_t *fg,
                            bcmptm_rm_tcam_slice_t *slice_ptr,
                            uint8_t *count_free_slices,
                            uint8_t group_compress)
{
    uint8_t fs;         /* Field slice structure pointer. */
    uint8_t fs_next;    /* Field slice structure pointer. */
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    bcmptm_rm_tcam_fp_group_t *pri_fg = NULL;
    int32_t pri_group_id = -1;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));
    fs = fg->primary_slice_id;
    pri_group_id = slice_ptr[fs].primary_grp_id;
    fs_next =0;
    do {
        fs = fs + fs_next;
        fs_next = slice_ptr[fs].next_slice_id_offset;

        if (slice_ptr[fs].num_entries == slice_ptr[fs].free_entries) {
            /* if it a primary slice and some presel entries are present,
             * don't free the slice.
             */
            if (fs == fg->primary_slice_id &&
                (slice_ptr[fs].num_presel_entries
                          != slice_ptr[fs].free_presel_entries)) {
                continue;
            }
            /* count the freed slices */
            *count_free_slices = (*count_free_slices) + 1;
            /* decrease the group's num_entires by
             * slice's num_entires for group delete case here.
             * num_entires will be corrected for group compress case
             * in group_partition_priority_reset function for all the
             * shared groups.
             */
            if (0 == group_compress) {
                fg->num_entries = fg->num_entries - slice_ptr[fs].num_entries;
            }
            /* Reset selcodes for the slice */
            if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
                SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_selcodes_reset(unit,
                            ltid, ltid_info, entry_attr, fs));
            }

            /* clear the h/w presel entries */
            if ((stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL)
                && (1 == group_compress)) {
                SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_presel_entries_clear(unit,
                                         ltid, ltid_info, fs, entry_attr));
            }
            /* Reset the partition priority for the slice. */
            SHR_IF_ERR_VERBOSE_EXIT(
                  bcmptm_rm_tcam_fp_group_partition_priority_reset(unit,
                                                             ltid, ltid_info,
                                                             entry_attr,
                                                             fs, slice_ptr,
                                                             group_compress));
        }
    } while(fs_next != 0);

    if (*count_free_slices > 0 &&
        (1 == group_compress)) {
    /* for group's sharing slices is compressed, and some slices are freed,
     * update the partition priority for all the groups sharing and install
     * the new partition priorities
     */
        while (pri_group_id != -1) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid,
                                  ltid_info, (uint32_t) pri_group_id,
                                   entry_attr->stage_id,
                                   &pri_fg));
            /* Install LT partition Priority. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_partition_prio_install(
                                     unit, ltid,
                                     ltid_info, pri_fg->group_ltid,
                                     entry_attr->pipe_id,
                                     entry_attr->stage_id));
            pri_group_id = pri_fg->next_shared_group_id;
        }
    }

 exit:
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_free_group_resources
 * Purpose:
 *    Release the group's resources when no entries are
 *    present in the group
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    ltid           - (IN) Logical Table enum that is accessing the table
 *    entry_attr     - (IN) Entry attrs structure
 * Returns:
 *    SHR_E_XXX
 */

int
bcmptm_rm_tcam_fp_free_group_resources(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             bcmptm_rm_tcam_entry_attrs_t *entry_attr)
{
    uint8_t count_free_slices = 0;
    int32_t pri_grp_id;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *pri_fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    uint8_t num_slices;        /* Number of Slices */
    int32_t curr_grp_id;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &slice_ptr));
    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attr->group_id,
                                   entry_attr->stage_id,
                                   &fg));

    /* Reset group priority */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_priority_reset(unit, ltid,
                                   ltid_info,
                                   entry_attr->pipe_id,
                                   stage_fc, fg,
                                   slice_ptr));
    if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_fp_group_free_unused_slices(unit,
                                   ltid,
                                   ltid_info,
                                   entry_attr, fg, slice_ptr,
                                   &count_free_slices, 0));

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_fp_entry_segment_compress(unit,
                                   ltid,
                                   ltid_info,
                                   fg->group_id,
                                   entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   count_free_slices, 1));
        /* fg->group_id == pri_grp_id
         * corresponding changes to pri_grp_id in slice structure is done in
         * bcmptm_rm_tcam_fp_group_priority_reset function
         * nothing to handle here
         */
        /* get the primary group Id after deleting the current group */
        pri_grp_id = slice_ptr[fg->primary_slice_id].primary_grp_id;

    } else {

        pri_grp_id = slice_ptr[fg->primary_slice_id].primary_grp_id;
        curr_grp_id = fg->group_id;

        if (pri_grp_id == curr_grp_id
            && fg->next_shared_group_id == -1) {
            pri_grp_id = -1;
        }
    }

    /* if the group's are sharing slices, remove the current
     * group pointer from the next_shared_group_id
     */
    if (pri_grp_id != -1) {
        /* Initialize field primary group pointer. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   (uint32_t) pri_grp_id,
                                   entry_attr->stage_id,
                                   &pri_fg));

        /* traverse the sharing groups till the current group Id
         * is found.
         */
        curr_grp_id = fg->group_id;
        while (pri_fg->next_shared_group_id != curr_grp_id
               && pri_fg->next_shared_group_id != -1) {
            pri_fg = pri_fg + 1;
        }
        if (pri_fg->next_shared_group_id == curr_grp_id) {
            pri_fg->next_shared_group_id = fg->next_shared_group_id;
        }
    }

    if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
        if (-1 == pri_grp_id) {
            /* clear the in use flag and clear the left/right offsets */
            if (fg->group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTER) {
                slice_ptr[0].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
                slice_ptr[0].free_presel_entries =
                                   slice_ptr[0].num_presel_entries;
                slice_ptr[0].left_slice_id_offset = 0;
                slice_ptr[0].right_slice_id_offset = 0;
                slice_ptr[1].slice_flags &= ~(BCMPTM_SLICE_IN_USE);
                slice_ptr[1].free_presel_entries =
                                   slice_ptr[1].num_presel_entries;
                slice_ptr[1].left_slice_id_offset = 0;
                slice_ptr[1].right_slice_id_offset = 0;
            } else {
                slice_ptr[fg->primary_slice_id].slice_flags &=
                             ~(BCMPTM_SLICE_IN_USE);
                slice_ptr[fg->primary_slice_id].free_presel_entries =
                             slice_ptr[fg->primary_slice_id].num_presel_entries;
                slice_ptr[fg->primary_slice_id].left_slice_id_offset = 0;
                slice_ptr[fg->primary_slice_id].right_slice_id_offset = 0;
            }
        }
    }

    fg->valid = FALSE;

    /* Install LT Action Priority. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_action_prio_install(unit,
                      ltid, ltid_info, entry_attr->pipe_id, stage_fc));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_compress
 * Purpose:
 *     Compress the groups, which no longer has entries
 *     Compress the groups, if any slices can be freed depending
 *     on the number of entries present in it.
 * Paramters:
 *     unit          - (IN) BCM device number
 *     ltid          - (IN) Logical Table enum that is accessing the table
 *     attrs         - (IN) Entry attrs structure
 *
 * Returns:
 *     SHR_E_XXX
 */

int bcmptm_rm_tcam_fp_group_compress(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *attrs)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
                               /* Field Slice structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *grp_ptr = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t slice_id;          /* Slice iterator */
    uint8_t slice_id_prev;     /* Slice iterator */
    uint32_t idx;
    uint32_t num_entries_per_slice = 0;
    uint32_t free_entries = 0;
    uint32_t total_entries = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    /* make a copy of the entry_attr structure*/
    SHR_ALLOC(entry_attr, sizeof(bcmptm_rm_tcam_entry_attrs_t),
                      "Entry attrs copy");

    sal_memcpy(entry_attr, attrs,
                           sizeof(bcmptm_rm_tcam_entry_attrs_t));

    /* Reset the presel entry flag */
    SHR_BITCLR(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &slice_ptr));

    /* Initialize field group pointer. */
    grp_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if ((grp_ptr[idx].valid == TRUE) &&
            (grp_ptr[idx].pipe_id == entry_attr->pipe_id)) {
            /* get number of entires per group */
            total_entries = grp_ptr[idx].num_entries;
            /* get number of free entires per group */
            slice_id = grp_ptr[idx].primary_slice_id;
            /* get num entries per slice */
            num_entries_per_slice = slice_ptr[slice_id].num_entries;
            /* Initialize the free count value to 0 */
            free_entries = 0;
            /* aggregate the free count for all depth slices corrsponding to
            * to the given group.
            */
            do {
               /* Add the free entries count in current slice */
               free_entries = free_entries + slice_ptr[slice_id].free_entries;

               slice_id_prev = slice_id;
               /* go to the next slice if exists */
               slice_id = slice_id +
                        slice_ptr[slice_id].next_slice_id_offset;

            } while (slice_id < num_slices
                 && (slice_id != slice_id_prev));

            slice_id = grp_ptr[idx].primary_slice_id;

            /* Initialize the grp_ptr in entry_attr structure */
            entry_attr->group_id = grp_ptr[idx].group_id;
            entry_attr->group_prio = grp_ptr[idx].priority;
            entry_attr->group_mode = grp_ptr[idx].group_mode;
            entry_attr->group_ltid = grp_ptr[idx].group_ltid;

            if ((free_entries == total_entries) &&
                (slice_ptr[slice_id].num_presel_entries
                         == slice_ptr[slice_id].free_presel_entries)) {
                /* group exists but no entries in it
                 * clear the slices associated with group
                 * change the group's valid value to false
                 * clear the primary_slice_id, num_entires etc
                 * reset the group priority and partition prio
                 */
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_free_group_resources(
                                                           unit,
                                                           ltid, ltid_info,
                                                           entry_attr));
                 continue;
            }
            /* calculate whether a slice/slices can be freed
             * from this group */
            if (free_entries > num_entries_per_slice) {
                /* calculate the number of depth slice
                 * that can be freed
                  */
                offset = grp_ptr[idx].entry_info_seg;
                entry_info = (bcmptm_rm_tcam_prio_entry_info_t*)
                               ((uint8_t *)stage_fc + offset);


                bcmptm_rm_tcam_fp_per_group_compress(unit, ltid, ltid_info,
                                                &grp_ptr[idx], slice_ptr,
                                                entry_info, entry_attr);
            }
        }
    }
    /* Install LT Action Priority. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_action_prio_install(unit,
                    ltid, ltid_info, entry_attr->pipe_id, stage_fc));

exit:
    SHR_FREE(entry_attr);
    SHR_FUNC_EXIT();
}

/*!
 * \brief To allocate slice for ifp group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table
 * \param [in] entry attrs structure
 * \param [out] primary_silce_id to be updated in
 *               field group structure
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage/group exists
 * \retval SHR_E_RESOURCE No slice avaliable to allocate
 */
int
bcmptm_rm_tcam_fp_slice_allocate(int unit,
                     bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     bcmptm_rm_tcam_entry_attrs_t *entry_attr)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_fp_group_t  *grp_ptr = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    int segment_id = -1;       /* Segment for entry info/hash */
    int pipe_id = 0; /* Pipe Id */
    uint32_t num_entries = 0;  /* Total pphysical entries */
    uint32_t num_presels = 0;  /* Total presel entries */
    uint8_t slice = -1;        /* Slice Id */
    uint8_t curr_slice_id = -1;        /* Slice Id */
    uint8_t prev_slice_id = -1;        /* Slice Id */
    bool slice_sharing = FALSE;/* Slice sharing indicator */
    bcmptm_rm_tcam_fp_group_t  *primary_fg = NULL;
                               /* Primary group in slice sharing */
    int8_t pri_grp_id = -1;   /* Primary group Id */

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &slice_fc));
    /* Initialize field group pointer. */
    grp_ptr = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);
    fg = (grp_ptr + entry_attr->group_id);

    /* check the validity of group */
    if (TRUE == fg->valid) {
       /* not a new group. Group already exists.
        * incorrect call to slice allocate
        */
       SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* check whether their exists a group already with same priority
     * and group_mode in same stage and pipe. if exists, group's (new
     * and existing) can share the same resources
     */
        /* groups with default presel entry cant share slices */
    rv = bcmptm_rm_tcam_fp_check_slice_sharing(unit, ltid, ltid_info,
                                    entry_attr,
                                    slice_fc, grp_ptr,
                                    &slice_sharing,
                                    &pri_grp_id);
    /* No resources */
    if (rv != SHR_E_NONE) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    if (slice_sharing == TRUE) {
        slice = grp_ptr[pri_grp_id].primary_slice_id;
    }

    if (slice_sharing == FALSE) {
        /* for EM, if group_mode is single check whether the slice ID
           is in USE. if it is already in use, return E_CONFIG.
           for EM, if group_mode is Double check whether both the slices
           are free. throw E_CONFIG otherwise
           */
        if (stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) {
            if (entry_attr->group_mode == BCMPTM_RM_TCAM_GRP_MODE_DBLINTER) {
                if (slice_fc[0].slice_flags & BCMPTM_SLICE_IN_USE
                    || slice_fc[1].slice_flags & BCMPTM_SLICE_IN_USE) {
                    SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
                }
                slice = 0;
                slice_fc[0].slice_flags |= BCMPTM_SLICE_IN_USE;
                slice_fc[0].free_presel_entries =
                        slice_fc[0].num_presel_entries;
                slice_fc[1].slice_flags |= BCMPTM_SLICE_IN_USE;
                slice_fc[1].free_presel_entries =
                        slice_fc[1].num_presel_entries;
                slice_fc[0].right_slice_id_offset = 1;
                slice_fc[1].left_slice_id_offset = -1;
                slice_fc[0].primary_grp_id = entry_attr->group_id;
                slice_fc[1].primary_grp_id = entry_attr->group_id;
            } else {
                slice = entry_attr->slice_id;
                if (!(slice_fc[slice].slice_flags & BCMPTM_SLICE_IN_USE)) {
                    slice_fc[slice].slice_flags |= BCMPTM_SLICE_IN_USE;
                    slice_fc[slice].free_presel_entries =
                                     slice_fc[slice].num_presel_entries;
                    slice_fc[slice].right_slice_id_offset = 0;
                    slice_fc[slice].left_slice_id_offset = 0;
                    slice_fc[slice].primary_grp_id = entry_attr->group_id;
                } else {
                    SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
                }
            }
        } else {
            rv = bcmptm_rm_tcam_fp_group_reserve_slice(unit,
                                                   entry_attr->group_mode,
                                                   stage_fc,
                                                   num_slices,
                                                   slice_fc,
                                                   &slice);
            /* No resources */
            if (rv != SHR_E_NONE) {
                SHR_RETURN_VAL_EXIT(rv);
            }
        }
    }

    /* Install selcodes in allocated slice */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_selcodes_install(unit,
                                              ltid, ltid_info,
                                               entry_attr, slice));
        /* Incase shared slice was already autoexpanded
         * Copy selcodes to all the expanded slices
         */
        if ((slice_sharing == TRUE) &&
                (slice_fc[slice].next_slice_id_offset != 0)) {
            curr_slice_id = slice + slice_fc[slice].next_slice_id_offset;
            do {
                prev_slice_id = curr_slice_id;
                SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_selcodes_copy(unit,
                                                      ltid, ltid_info,
                                                      entry_attr,
                                                      slice,
                                                      curr_slice_id));
                curr_slice_id = curr_slice_id
                              + slice_fc[curr_slice_id].next_slice_id_offset;
            } while ((curr_slice_id != prev_slice_id)
                    && (curr_slice_id < num_slices));
        }
    }

    /* Initialize the fg structure with slice info */
    fg->group_id = entry_attr->group_id;
    fg->primary_slice_id = slice;
    fg->priority = entry_attr->group_prio;
    fg->group_mode = entry_attr->group_mode;
    fg->group_ltid = entry_attr->group_ltid;
    fg->num_presel_entries = entry_attr->num_presel_ids;
    fg->pipe_id = entry_attr->pipe_id;
    fg->num_entries = 0;
    fg->next_shared_group_id = -1;

    /*
     * Set Action prority and partition priority for the
     * group. Also set's the ING_CONFIGr (slice enable)
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_priority_set(unit, ltid, ltid_info,
            stage_fc, entry_attr, slice, slice_fc, slice_sharing));

    /* Install LT Action Priority. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_lt_action_prio_install(unit,
                      ltid, ltid_info, entry_attr->pipe_id, stage_fc));

    /* Install LT partition Priority. */
    if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_fp_group_lt_partition_prio_install(unit,
                                           ltid, ltid_info,
                                           entry_attr->group_ltid,
                                           entry_attr->pipe_id,
                                           entry_attr->stage_id));
    }
    pipe_id = entry_attr->pipe_id;
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    if (slice_sharing == TRUE) {
        primary_fg = &grp_ptr[pri_grp_id];
        fg->num_entries = primary_fg->num_entries;
        sal_memcpy(&(fg->entry_seg_bmp),
                   &(primary_fg->entry_seg_bmp),
                   sizeof(bcmptm_fp_entry_seg_bmp_t));
        fg->entry_info_seg = primary_fg->entry_info_seg;
        fg->entry_hash_size = primary_fg->entry_hash_size;
        fg->entry_hash_seg = primary_fg->entry_hash_seg;
        fg->presel_entry_info_seg = primary_fg->presel_entry_info_seg;
        fg->presel_entry_hash_size = primary_fg->presel_entry_hash_size;
        fg->presel_entry_hash_seg = primary_fg->presel_entry_hash_seg;

    } else {
        if (!(stage_fc->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
            fg->num_entries = slice_fc[slice].num_entries;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_fp_group_entry_segment_get(unit,
                                                      ltid,
                                                      ltid_info,
                                                      entry_attr->stage_id,
                                                      pipe_id,
                                                      &segment_id));

            num_entries = (stage_fc->max_entries_per_slice
                           * (stage_fc->num_slices / stage_fc->num_pipes));

            /* Segment for an entry is decided by its pipe
             * and the segment_id inside the pipe.
             * Total entry space is divided into pipe count of parts.
             * In every part, one segment is allocated for the group.
             */

            SHR_BITSET(fg->entry_seg_bmp.w,segment_id);
            fg->entry_info_seg = (stage_fc->entry_info_seg)
                               + (((stage_fc->max_entries_per_slice
                                  * segment_id)
                               + (pipe_id * num_entries))
                               * sizeof(bcmptm_rm_tcam_prio_entry_info_t));
            fg->entry_hash_size = (stage_fc->max_entries_per_slice);
            fg->entry_hash_seg = (stage_fc->entry_hash_seg)
                               + (((fg->entry_hash_size * segment_id)
                               + ((num_entries) * pipe_id))
                               * sizeof(int));
        }
        num_presels = (stage_fc->max_presel_entries_per_slice
                       * (stage_fc->num_slices / stage_fc->num_pipes));
        fg->presel_entry_info_seg = (stage_fc->presel_entry_info_seg)
                               + (((stage_fc->max_presel_entries_per_slice
                               * fg->primary_slice_id)
                               + (pipe_id * num_presels))
                               * sizeof(bcmptm_rm_tcam_prio_entry_info_t));
        fg->presel_entry_hash_size = (stage_fc->max_presel_entries_per_slice
                                      / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
        fg->presel_entry_hash_seg = (stage_fc->presel_entry_hash_seg)
                               + (((fg->presel_entry_hash_size
                                    * fg->primary_slice_id)
                               + ((num_presels
                               / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)
                               * pipe_id)) * sizeof(int));
    }

    fg->flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
    fg->flags |= BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;

    /* update group flags */
    if (slice_fc[slice].slice_flags & BCMPTM_SLICE_SUPPORT_AUTO_EXPANSION) {
        fg->flags = fg->flags | BCMPTM_RM_TCAM_F_AUTO_EXPANSION;
    }

    /* set the valid bit */
    fg->valid = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To expand an existing group
 *  Allocate new slice/slices for existing ifp group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table
 * \param [in] entry attrs structure
 * \param [out] updated the num_entires in
 *               field group structure
 * \param [out] new slice id allocated
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_RESOURCE No slice availble to expand
 */
int
bcmptm_rm_tcam_fp_slice_expand(int unit,
                     bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                     uint8_t *new_slice_id)
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
                               /* Field Slice structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
                               /* Field Stage structure pointer. */
    uint8_t num_slices;        /* Num of Slices for Field stage  */
    uint8_t new_slice;         /* Slice iterator */
    uint8_t new_slice_part;    /* Slice iterator */
    uint8_t slice_id;          /* Slice iterator */
    uint8_t slice_id_prev;     /* Slice iterator */
    int parts_count = -1;
    int part;
    uint8_t  intraslice = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameters check. */
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                   entry_attr->stage_id,
                                   &stage_fc));
    /* Initialize field slice pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_info_get(unit, ltid, ltid_info,
                                   &num_slices, entry_attr->pipe_id,
                                   entry_attr->stage_id,
                                   &slice_fc));

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_info_get(unit, ltid, ltid_info,
                                   entry_attr->group_id,
                                   entry_attr->stage_id,
                                   &fg));

    /* check whether the group under auto expansion
     * is indeed the right one
     */
    if (fg->group_id != entry_attr->group_id ||
        fg->priority != entry_attr->group_prio) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    
     /* Reserve slice/slices */
    rv = bcmptm_rm_tcam_fp_group_reserve_slice(unit,
                                               entry_attr->group_mode,
                                               stage_fc,
                                               num_slices,
                                               slice_fc,
                                               &new_slice);
    /* No resources */
    if (rv != SHR_E_NONE) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    /* update the new_slice_id*/
    *new_slice_id = new_slice;

    /* Install selcodes in allocated slice */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_selcodes_copy(unit,
                                              ltid, ltid_info, entry_attr,
                                              fg->primary_slice_id,
                                              new_slice));
    }

    /*
     * Set partition priority for the
     * group. Also set's the ING_CONFIGr (slice enable)
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_partition_priority_set(unit, ltid,
            ltid_info,
            stage_fc, entry_attr, new_slice, slice_fc));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                               entry_attr->group_mode,
                                               stage_fc->flags,
                                               &parts_count));

    if (!(stage_fc->flags & BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE) &&
        (BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode)) {
        intraslice = 1;
        parts_count = parts_count >> intraslice;
    }

    /* if the group is already auto expanded,
     * go to the last slice and update the slice
     * offset to append the newly allocated slice/slices.
     *
     *   ----> parts
     *   |
     *   |
     *  depth (below is a representation of
     *         auto expanded triple wide group)
     *   ___     ___     ___
     *  |___|-->|___|-->|___|
     *    |       |       |
     *   _|_     _|_     _|_
     *  |_ _|-->|_ _|-->|_ _|
 */
    for (part = 0; part < parts_count; part++) {
        slice_id = fg->primary_slice_id + part;
        new_slice_part = new_slice + part;
        do {
            /* go to the next slice if exists */
            slice_id_prev = slice_id;
            slice_id = slice_id +
                        slice_fc[slice_id].next_slice_id_offset;

        } while (slice_id < num_slices
                 && (slice_id_prev != slice_id));

        slice_fc[slice_id].next_slice_id_offset =
                                 (new_slice_part - slice_id);

        slice_fc[new_slice_part].prev_slice_id_offset =
                                 (slice_id - new_slice_part);
    }

exit:
    SHR_FUNC_EXIT();
}

