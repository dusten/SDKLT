/*! \file bcmltm_ee_model_index.c
 *
 * Logical Table Manager Execution Engine
 *
 * Logical table simple index basic implementation
 *
 * This file contains the LTM EE stage functions for LT operations
 * on simple index tables.
 *
 * ALLOC  - not supported for simple index LTs
 * INSERT
 * LOOKUP
 * DELETE
 * UPDATE
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
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_EXECUTIONENGINE


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Common logic for LTM-PTM interface of modeled index PT table ops.
 *
 * This function implements the common logic for the LTM-PTM interface
 * to dispatch WRITE or READ ops for modeled index physical tables.
 * The PTM interface for each op is common, so the preparatory logic
 * is very similar.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 * \param [in] pt_op The PTM op type to be executed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_ee_node_model_index_opcode(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie,
                                  bcmptm_op_type_t pt_op)
{
    uint32_t ptix, opix;
    uint32_t pt_flags;
    uint32_t pt_count, pt_sbus_count;
    uint8_t word_size;
    uint32_t *cur_entry;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offset_t param_offset;
    bcmltm_wb_ptm_entry_index_offset_t entry_offset;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint32_t rsp_flags;
    uint32_t req_flags;
    uint32_t ltid, ptid;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    pt_count = BCMLTM_PT_LIST_NUM(node_cookie);

    for (ptix = 0; ptix < pt_count; ptix++) {
        cur_mem_args = BCMLTM_PT_LIST_MEM_ARGS(node_cookie, ptix);
        pt_op_list = BCMLTM_PT_LIST_OPS_LIST(node_cookie, ptix);
        pt_sbus_count = pt_op_list->num_pt_ops;
        word_size = pt_op_list->word_size;
        pt_flags = pt_op_list->flags;

        /*
         * Clear the PT changed flag if PTM operation is not a write.
         *
         * The PT changed logic only applies to a PTM write operation.
         * Currently the PT op list node cookie is shared among
         * several opcodes, so it is necessary to clear this flag
         * when the operation is not a PTM write.
         * If in the future the PT op list node cookie is created
         * per opcode, this explicit clearing of this flag can be removed.
         */
        if (pt_op != BCMPTM_OP_WRITE) {
            pt_flags &= ~BCMLTM_PT_OP_FLAGS_PT_CHANGED;
        }

        /* Permit HW forced (bypass PT cache) */
        req_flags = cur_mem_args->flags;
        if (lt_entry->flags & BCMLTM_ENTRY_FLAG_HW_GET) {
            req_flags |= BCMPTM_REQ_FLAGS_DONT_USE_CACHE;
        }

        for (opix = 0; opix < pt_sbus_count; opix++) {
            /* Get PTM parameter offsets */
            (void)bcmltm_wb_ptm_param_offset_get(
                pt_op_list->param_offsets[opix],
                &param_offset);

            /* Check PT entry changed flag */
            if (pt_flags & BCMLTM_PT_OP_FLAGS_PT_CHANGED) {
                if (ltm_buffer[param_offset.pt_changed] !=
                    BCMLTM_PT_ENTRY_CHANGED_VALUE) {
                    continue;
                }
            }

            /* Fresh copy for any adjustments with PT_OP_FLAGS */
            sal_memcpy(&cur_pt_dyn_info, cur_mem_args->pt_dyn_info,
                       sizeof(bcmbd_pt_dyn_info_t));

            bcmltm_pt_dyn_info_get(pt_flags, &param_offset,
                                   ltm_buffer, &cur_pt_dyn_info);

            if (pt_flags & BCMLTM_PT_OP_FLAGS_ACCTYPE) {
                /*
                 * Implementation pending
                 */
                SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            }

            if (pt_flags & BCMLTM_PT_OP_FLAGS_PDD_BITS) {
                /*
                 * Implementation pending
                 */
                SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            }

            /* Check if a LTA Transform has modified the SID sent
             * to the PTM. Use that instead of PTID if so. */
            if (pt_flags & BCMLTM_PT_OP_FLAGS_PT_SID) {
                ptid = ltm_buffer[param_offset.pt_sid_override];
            } else {
                ptid = cur_mem_args->pt;
            }

            /* Get PTM entry offsets */
            (void)bcmltm_wb_ptm_entry_index_offset_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offset);

            cur_entry = ltm_buffer + entry_offset.data;

            /*
             * Perform the PTM operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            SHR_IF_ERR_EXIT
                (bcmptm_mreq_indexed_lt(unit,
                                        req_flags,
                                        ptid,
                                        &cur_pt_dyn_info,
                                        cur_mem_args->pt_static_info,
                                        NULL, /* misc_info */
                                        pt_op,
                                        cur_entry,
                                        word_size,
                                        ltid,
                                        lt_entry->trans_id, /* cseq_id */
                                        
                                        NULL, /* notify ptr */
                                        NULL, /* notify info (cookie) */
                                        cur_entry,
                                        &rsp_ltid,
                                        &rsp_flags));

            /* @@@ Process the response values - LTID and flags */
            SHR_IF_ERR_EXIT
                (bcmltm_pt_status_mreq_indexed_lt(unit, ltid, ptid,
                                                  &cur_pt_dyn_info));
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*
 * This function examines the index of a simple index LT table for
 * existence, and permits an LT entry insertion if it does not.
 */
int
bcmltm_ee_node_model_index_insert(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, ltm_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    if (SHR_BITGET(inuse_bitmap, cur_index)) {
        /* Entry already exists.  Insert not permitted */
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    /* Else, mark entry as now existing.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITSET(inuse_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

/*
 * This function examines the index of a simple index LT table for
 * existence, and removes the in-use marker if so.
 */
int
bcmltm_ee_node_model_index_delete(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, ltm_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    if (!SHR_BITGET(inuse_bitmap, cur_index)) {
        /* Entry does not exist.  Delete not permitted */
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Else, mark entry as now free.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITCLR(inuse_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

/*
 * This function tests the index of a simple index LT table for
 * existence.
 */
int
bcmltm_ee_node_model_index_exists(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, ltm_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    if (!SHR_BITGET(inuse_bitmap, cur_index)) {
        /* Entry does not exist. */
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_model_index_write(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    return bcmltm_ee_node_model_index_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_WRITE);
}

int
bcmltm_ee_node_model_index_read(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    return bcmltm_ee_node_model_index_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_READ);
}

/*
 * This function examines the index of a shared resource simple index
 * LT table for existence, and permits an LT entry insertion
 * if it does not.
 */
int
bcmltm_ee_node_global_index_insert(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *global_bitmap;
    uint32_t global_ltid;
    bcmltm_lt_state_t *global_lt_state;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    global_ltid = ltm_md->params->global_inuse_ltid;

    /* Global LT state */
    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_get(unit, global_ltid, &global_lt_state));

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, global_lt_state,
                               BCMLTM_STATE_TYPE_GLOBAL_BITMAP,
                               (void *) &global_bitmap));

    if (SHR_BITGET(global_bitmap, cur_index)) {
        /* Entry already exists.  Insert not permitted */
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    /* Else, mark entry as now existing.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITSET(global_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

/*
 * This function examines the index of a shared resource simple index
 * LT table for existence, and removes the in-use marker if so.
 */
int
bcmltm_ee_node_global_index_delete(int unit,
                                  bcmltm_lt_md_t *ltm_md,
                                  bcmltm_lt_state_t *ltm_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t cur_index;
    bcmltm_ee_index_info_t *idx_info;
    SHR_BITDCL *global_bitmap;
    uint32_t global_ltid;
    bcmltm_lt_state_t *global_lt_state;

    SHR_FUNC_ENTER(unit);

    idx_info = BCMLTM_EE_INDEX_INFO(node_cookie);

    if (idx_info == NULL) {
        /* "No-key" LTs do not require an API field to determine the index.
         * They may only have one entry. */
        cur_index = 0;
    } else {
        cur_index = ltm_buffer[idx_info->index_offset];
    }

    global_ltid = ltm_md->params->global_inuse_ltid;

    /* Global LT state */
    SHR_IF_ERR_EXIT
        (bcmltm_state_lt_get(unit, global_ltid, &global_lt_state));

    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, global_lt_state,
                               BCMLTM_STATE_TYPE_GLOBAL_BITMAP,
                               (void *) &global_bitmap));

    if (!SHR_BITGET(global_bitmap, cur_index)) {
        /* Entry does not exist.  Delete not permitted */
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Else, mark entry as now free.
     * Note that this is the working state which is discarded
     * on a subsequent failure any time during the transaction.
     */
    SHR_BITCLR(global_bitmap, cur_index);

 exit:
    SHR_FUNC_EXIT();
}

