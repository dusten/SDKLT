/*! \file bcmltm_fa_index.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Logical Table index calculations
 *
 * This file contains the LTM FA stage functions for Logical Table
 * index calculations when the LT index does not match a PT index.
 * This case may arise when per-port, per-pipe, or other table instance
 * cases results in multiple physical resources for a given PT index.
 * The LT index tracks the in-use and allocation bitmaps, and must
 * account for the true physical resources involved.
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

/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_lt_index(int unit,
                        bcmltm_lt_md_t *lt_md,
                        bcmltm_lt_state_t *lt_state,
                        bcmltm_entry_t *lt_entry,
                        uint32_t *ltm_buffer,
                        void *node_cookie)
{
    uint32_t mem_param, lt_index, index_multiplier;
    bcmltm_fa_lt_index_t *idx_info;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_FA_LT_INDEX(node_cookie);

    mem_param = ltm_buffer[idx_info->mem_param_offset];

    if ((mem_param < lt_state->current_param_min) ||
        (mem_param > lt_state->current_param_max)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    lt_index = ltm_buffer[idx_info->lt_index_offset];

    if ((lt_index < lt_state->current_index_min) ||
        (lt_index > lt_state->current_index_max)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    index_multiplier = idx_info->index_multiplier;

    ltm_buffer[idx_info->track_index_offset] =
        lt_index + (mem_param * index_multiplier);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_traverse_index(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    uint32_t mem_param, lt_index, index_multiplier, mp_max;
    uint32_t start_bit, next_bit, max_bit;
    uint32_t min_index;
    bcmltm_fa_lt_index_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_FA_LT_INDEX(node_cookie);

    index_multiplier = idx_info->index_multiplier;
    mp_max = lt_state->current_param_max;
    min_index = lt_md->params->index_min;

    if (lt_entry->flags & BCMLTM_ENTRY_FLAG_TRAVERSE_START) {
        /* No key fields provided, find first entry */
        start_bit = min_index;
    } else {
        /* Last key fields are provided */
        mem_param = ltm_buffer[idx_info->mem_param_offset];

        if (mem_param > mp_max) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        lt_index = ltm_buffer[idx_info->lt_index_offset];

        start_bit =
            lt_index + (mem_param * index_multiplier) + 1;
    }

    max_bit = ((mp_max + 1) * index_multiplier) - 1;

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    
    for (next_bit = start_bit;
         next_bit <= max_bit;
         next_bit++) {
        /* Search in-use bitmap for next entry */
        if (SHR_BITGET(inuse_bitmap, next_bit)) {
            break;
        }
    }

    if (next_bit > max_bit) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Update the WB locations with the search results */
    ltm_buffer[idx_info->track_index_offset] = next_bit;
    ltm_buffer[idx_info->lt_index_offset] =
        next_bit % index_multiplier;
    ltm_buffer[idx_info->mem_param_offset] = next_bit / index_multiplier;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lt_alloc_index(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    uint32_t mem_param, lt_index, index_multiplier;
    bcmltm_fa_lt_index_t *idx_info;
    uint32_t next_bit, max_bit;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_FA_LT_INDEX(node_cookie);

    mem_param = ltm_buffer[idx_info->mem_param_offset];
    index_multiplier = idx_info->index_multiplier;

    if ((mem_param < lt_state->current_param_min) ||
        (mem_param > lt_state->current_param_max)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (ltm_buffer[idx_info->index_absent_offset] == BCMLTM_INDEX_ABSENT) {
        /* Determine available index */
        max_bit = (mem_param  * index_multiplier) +
            lt_state->current_index_max;

        SHR_IF_ERR_EXIT
            (bcmltm_state_data_get(unit, lt_state,
                                   BCMLTM_STATE_TYPE_INUSE_BITMAP,
                                   (void *) &inuse_bitmap));

        
        for (next_bit = (mem_param  * index_multiplier) +
                 lt_state->current_index_min;
             next_bit <= max_bit;
             next_bit++) {
            /* Search in-use bitmap for next free entry */
            if (!SHR_BITGET(inuse_bitmap, next_bit)) {
                break;
            }
        }

        /* No free entry found */
        if (next_bit > max_bit) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        }

        /* Update the WB locations with the search results */
        ltm_buffer[idx_info->track_index_offset] = next_bit;
        ltm_buffer[idx_info->lt_index_offset] =
            next_bit % index_multiplier;
    } else {
        /* Standard index error checking */
        lt_index = ltm_buffer[idx_info->lt_index_offset];

        if ((lt_index < lt_state->current_index_min) ||
            (lt_index > lt_state->current_index_max)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        ltm_buffer[idx_info->track_index_offset] =
            lt_index + (mem_param * index_multiplier);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lt_global_alloc_index(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    uint32_t mem_param, lt_index, index_multiplier;
    bcmltm_fa_lt_index_t *idx_info;
    uint32_t next_bit, max_bit;
    SHR_BITDCL *global_inuse_bitmap;
    uint32_t global_ltid;
    bcmltm_lt_state_t *global_lt_state;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_FA_LT_INDEX(node_cookie);

    mem_param = ltm_buffer[idx_info->mem_param_offset];
    index_multiplier = idx_info->index_multiplier;

    if ((mem_param < lt_state->current_param_min) ||
        (mem_param > lt_state->current_param_max)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (ltm_buffer[idx_info->index_absent_offset] == BCMLTM_INDEX_ABSENT) {
        /* Determine available index */
        max_bit = (mem_param  * index_multiplier) +
            lt_state->current_index_max;

        /*
         * Since this is a shared table, the global in-use bitmap
         * is used to seek a free entry.
         */
        global_ltid = lt_md->params->global_inuse_ltid;

        /* Global LT state */
        SHR_IF_ERR_EXIT
            (bcmltm_state_lt_get(unit, global_ltid, &global_lt_state));

        SHR_IF_ERR_EXIT
            (bcmltm_state_data_get(unit, global_lt_state,
                                   BCMLTM_STATE_TYPE_GLOBAL_BITMAP,
                                   (void *) &global_inuse_bitmap));

        
        for (next_bit = (mem_param  * index_multiplier) +
                 lt_state->current_index_min;
             next_bit <= max_bit;
             next_bit++) {
            /* Search in-use bitmap for next free entry */
            if (!SHR_BITGET(global_inuse_bitmap, next_bit)) {
                break;
            }
        }

        /* No free entry found */
        if (next_bit > max_bit) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        }

        /* Update the WB locations with the search results */
        ltm_buffer[idx_info->track_index_offset] = next_bit;
        ltm_buffer[idx_info->lt_index_offset] =
            next_bit % index_multiplier;
    } else {
        /* Standard index error checking */
        lt_index = ltm_buffer[idx_info->lt_index_offset];

        if ((lt_index < lt_state->current_index_min) ||
            (lt_index > lt_state->current_index_max)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        ltm_buffer[idx_info->track_index_offset] =
            lt_index + (mem_param * index_multiplier);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_index_keys_bounds(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, ltid;
    uint32_t key_num;
    bcmltm_index_key_bounds_t *index_key_bounds = NULL;
    bcmltm_key_bound_info_t *cur_field_bound = NULL;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data;
    const bcmlrd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    /* Initialize in case of error */
    field_val[0] = field_val[1] = 0;

    ltid = lt_md->params->ltid;
    key_num = lt_md->params->index_key_num;

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_KEY_BOUNDS,
                               (void *) &index_key_bounds));

    for (fix = 0; fix < key_num; fix++) {
        cur_field_bound = &(index_key_bounds->key_bounds[fix]);

        api_field_data =
            bcmltm_api_find_field(lt_entry->in_fields,
                                  cur_field_bound->api_field_id,
                                  cur_field_bound->field_idx);
        if (api_field_data == NULL) {
            /* Field existence is verified elsewhere */
            continue;
        }

        if ((api_field_data->data < cur_field_bound->current_key_min) ||
             (api_field_data->data > cur_field_bound->current_key_max)) {
            /* Out of bounds index key field */
            tbl = bcmlrd_table_get(ltid);
            if (tbl != NULL) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "LTM %s: Key field %s"
                                        " value 0x%" PRIx64
                                        " out of bounds"
                                        "(0x%" PRIx64 " - 0x%" PRIx64 ")\n"),
                             tbl->name,
                             tbl->field[cur_field_bound->api_field_id].name,
                             api_field_data->data,
                             cur_field_bound->current_key_min,
                             cur_field_bound->current_key_min));
            }
            
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}
