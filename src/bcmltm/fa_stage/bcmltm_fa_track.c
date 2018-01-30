/*! \file bcmltm_fa_track.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Logical Table Tracking Index calculations
 *
 * This file contains the LTM FA stage functions for Logical Table
 * Tracking Index calculations.
 * The Tracking Index is a combination of the device parameters
 * required to identify a unique device resource corresponding to
 * a unique set of LT API Key fields.
 *
 * The Key fields are copied or Transformed into the device physical
 * parameters.  These may include memory index, register array index,
 * register port, pipeline number, PT select, and overlay select.
 * However many there may be, the resulting Tracking Index should
 * fit within a single 32-bit value.
 *
 * The Tracking Index is the offset into the in-use, the
 * global in-use, and the valid entry bitmaps.
 *
 * The format of the Tracking Index is zero or more fields
 * appended together to form a single 32-bit value.
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

#include <shr/shr_debug.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_FIELDADAPTATION


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief An FA tree node to determine Tracking index for w/Allocation LTs.
 *
 * An FA tree node which calculates the Tracking index, from the input API
 * fields provided.  If allocatable key fields are not present, a free
 * Tracking Index is determined and returned at completion of the INSERT
 * operation.  One set of Working Buffer physical field values are
 * updated in such a case.
 * This function is only applicable for Index w/Allocation
 * Logical Tables.
 *
 * This core implementation handles either local and global inuse bitmaps.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_alloc_track_core(int unit,
                                bool global,
                                bcmltm_lt_state_t *lt_state,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    uint32_t next_bit;
    bcmltm_state_type_t inuse_type;
    SHR_BITDCL *inuse_bitmap;
    SHR_BITDCL *valid_bitmap;
    bcmltm_track_index_t *track_info;
    uint32_t tracking_index, track_max, track_mask;

    SHR_FUNC_ENTER(unit);

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    tracking_index = ltm_buffer[track_info->track_index_offset];

    if (ltm_buffer[track_info->index_absent_offset] == BCMLTM_INDEX_ABSENT) {
        /* Determine available index */

        /* Preserve the non-allocatable portion of the Tracking Index */
        track_mask = track_info->track_alloc_mask;
        track_max = (tracking_index & ~track_mask) | track_mask;

        /* Retrieve the necessary inuse bitmap */
        inuse_type = global ? BCMLTM_STATE_TYPE_GLOBAL_BITMAP :
            BCMLTM_STATE_TYPE_INUSE_BITMAP;

        SHR_IF_ERR_EXIT
            (bcmltm_state_data_get(unit, lt_state,
                                   inuse_type,
                                   (void *) &inuse_bitmap));

        /* Retrieve the valid bitmap */
        SHR_IF_ERR_EXIT
            (bcmltm_state_data_get(unit, lt_state,
                                   BCMLTM_STATE_TYPE_VALID_BITMAP,
                                   (void *) &valid_bitmap));

        
        for (next_bit = tracking_index;
             next_bit <= track_max;
             next_bit++) {
            /* Check valid bitmap for legal entry */
            if (!SHR_BITGET(valid_bitmap, next_bit)) {
                continue;
            }
            /* Check in-use bitmap for next free entry */
            if (!SHR_BITGET(inuse_bitmap, next_bit)) {
                break;
            }
        }

        /* No free entry found */
        if (next_bit > track_max) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        }

        tracking_index = next_bit;

        /* Update the WB Tracking Index location with the search result */
        ltm_buffer[track_info->track_index_offset] = tracking_index;
    } else {
        track_max = track_info->track_index_max;

        if (tracking_index > track_max) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_wbc_to_track(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_track_index_t *track_info;
    bcmltm_track_index_field_t *cur_field;
    uint32_t field_ix, field_num, tracking_index, field_val;

    SHR_FUNC_ENTER(unit);

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    field_num = track_info->track_index_field_num;
    tracking_index = 0;

    for (field_ix = 0; field_ix < field_num; field_ix++) {
        cur_field = &(track_info->track_index_fields[field_ix]);
        field_val = ltm_buffer[cur_field->field_offset];
        if (field_val > cur_field->field_mask) {
            /* This error should have been caught by field range checks. */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        tracking_index |= (field_val << cur_field->field_shift);
    }

    ltm_buffer[track_info->track_index_offset] = tracking_index;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_track_to_wbc(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_track_index_t *track_info;
    bcmltm_track_index_field_t *cur_field;
    uint32_t field_ix, field_num, tracking_index, field_val;

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    field_num = track_info->track_index_field_num;
    tracking_index = ltm_buffer[track_info->track_index_offset];

    for (field_ix = 0; field_ix < field_num; field_ix++) {
        cur_field = &(track_info->track_index_fields[field_ix]);
        field_val = (tracking_index >> cur_field->field_shift) &
            cur_field->field_mask;
        ltm_buffer[cur_field->field_offset] = field_val;
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_alloc_track_to_wbc(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    bcmltm_track_index_t *track_info;
    bcmltm_track_index_field_t *cur_field;
    uint32_t field_ix, field_num, tracking_index, field_val;

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);
    if (ltm_buffer[track_info->index_absent_offset] != BCMLTM_INDEX_ABSENT) {
        /* Nothing to do */
        return SHR_E_NONE;
    }

    field_num = track_info->track_index_field_num;
    tracking_index = ltm_buffer[track_info->track_index_offset];

    for (field_ix = 0; field_ix < field_num; field_ix++) {
        cur_field = &(track_info->track_index_fields[field_ix]);
        if (!cur_field->allocatable) {
            /* Do not overwrite non-allocatable device parameters */
            continue;
        }
        field_val = (tracking_index >> cur_field->field_shift) &
            cur_field->field_mask;
        ltm_buffer[cur_field->field_offset] = field_val;
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_traverse_track(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    uint32_t next_bit;
    SHR_BITDCL *inuse_bitmap;
    bcmltm_track_index_t *track_info;
    uint32_t tracking_index, track_max;

    SHR_FUNC_ENTER(unit);

    track_info = BCMLTM_FA_TRACK_INDEX(node_cookie);

    tracking_index = ltm_buffer[track_info->track_index_offset];
    track_max = track_info->track_index_max;

    /*
     * If this is the first TRAVERSE op, then the current minimum values
     * of the API Key fields should be placed in the physical parameters
     * so bcmltm_fa_node_wbc_to_track can produce the minimum
     * Tracking Index for use here.
     * Do not increment in that case.
     */
    if ((lt_entry->flags & BCMLTM_ENTRY_FLAG_TRAVERSE_START) == 0) {
        /* Not the first TRAVERSE, so increment tracking_index */
        tracking_index++;
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    
    for (next_bit = tracking_index;
         next_bit <= track_max;
         next_bit++) {
        /* Search in-use bitmap for next entry */
        if (SHR_BITGET(inuse_bitmap, next_bit)) {
            break;
        }
    }

    if (next_bit > track_max) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Update the WB Tracking Index location with the search result */
    ltm_buffer[track_info->track_index_offset] = next_bit;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_alloc_track(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie)
{
    return bcmltm_fa_node_alloc_track_core(unit, FALSE, lt_state,
                                           ltm_buffer, node_cookie);
}

int
bcmltm_fa_node_global_alloc_track(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    return bcmltm_fa_node_alloc_track_core(unit, TRUE, lt_state,
                                           ltm_buffer, node_cookie);
}
