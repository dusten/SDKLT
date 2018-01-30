/*! \file bcmltm_ee_passthru.c
 *
 * Logical Table Manager Execution Engine
 *
 * Physical table bypass implementation
 *
 * This file contains the LTM EE stage functions for PT passthrou
 * That is, the set of PTM operations which permit access to the
 * underlying physical device resources on which LTs are built.
 *
 * Under normal operations, these tables are not used to reach the
 * device.  These are provided for debug or unanticipated exception cases,
 * and perhaps as building blocks for heirarchical LTs.
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

/*
 * Working buffer layout for PT passthru
 */

/*
 * | uint32_t index |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] |
 */

#include <shr/shr_debug.h>
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_tree.h>
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
 * \brief Convert parameter offset list in WB to PTM dynamic hash info
 *
 * This function extracts the PTM parameters from the Working Buffer
 * based on the parsed parameter offsets and returns the information
 * in the dynamic hash info structure to be used by the PTM interactive
 * hash operation.
 *
 * \param [in] pt_flags BCMLTM_PT_OP_FLAGS_* to parse parameters.
 * \param [in] offset The processed set of LTM buffer offsets that
 *                    indicate the location of each memory paramter.
 * \param [in] ltm_buffer Pointer to the LTM Working Buffer for this op.
 * \param [out] dyn_info The PTM dynamic information structure to be
 *                     filled out with the provided memory parameters.
 *
 * \retval None.
 */
static void
bcmltm_pt_dyn_hash_info_get(uint32_t pt_flags,
                            bcmltm_wb_ptm_param_offset_t *offset,
                            uint32_t *ltm_buffer,
                            bcmbd_pt_dyn_hash_info_t *dyn_hash_info)
{
    if (pt_flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) {
        /*
         * Add the table instance to the PT dynamic info
         * Note, only one of port or table instance should
         * be specified for a given PT operation.
         */
        dyn_hash_info->tbl_inst = ltm_buffer[offset->table_inst];
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_BANK) {
        /*
         * Add the cur_index to the memen_spec.
         */
        dyn_hash_info->bank = ltm_buffer[offset->bank];
    }
}

/*!
 * \brief Common logic for LTM-PTM interface of PT PassThru hash table ops.
 *
 * This function implements the common logic for the LTM-PTM interface
 * to dispatch INSERT, LOOKUP, and DELETE ops for PT PassThru hash
 * physical tables.  The PTM interface for each op is common, so the
 * preparatory logic is very similar.
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
bcmltm_pt_hash_opcode(int unit,
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
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offset_t param_offset;
    bcmltm_wb_ptm_entry_keyed_pthru_offset_t entry_offset;
    bcmbd_pt_dyn_hash_info_t cur_pt_dyn_hash_info;
    uint32_t rsp_ltid;
    uint32_t rsp_flags;
    uint32_t ltid;
    uint32_t *entry_key;
    uint32_t *entry_data;
    uint32_t entry_index;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    pt_count = BCMLTM_PT_LIST_NUM(node_cookie);

    for (ptix = 0; ptix < pt_count; ptix++) {
        cur_mem_args = BCMLTM_PT_LIST_MEM_ARGS(node_cookie, ptix);
        pt_op_list = BCMLTM_PT_LIST_OPS_LIST(node_cookie, ptix);
        pt_sbus_count = pt_op_list->num_pt_ops;
        word_size = pt_op_list->word_size;
        pt_flags = pt_op_list->flags;

        /* Sanity check (since pt_dyn_hash_info is only created on Hashed PT) */
        if (cur_mem_args->pt_dyn_hash_info == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
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
            sal_memcpy(&cur_pt_dyn_hash_info, cur_mem_args->pt_dyn_hash_info,
                       sizeof(bcmbd_pt_dyn_hash_info_t));

            bcmltm_pt_dyn_hash_info_get(pt_flags, &param_offset,
                                        ltm_buffer, &cur_pt_dyn_hash_info);

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

            /* Prepare the entry arrays */
            (void)bcmltm_wb_ptm_entry_keyed_pthru_offset_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offset);
            entry_key = ltm_buffer + entry_offset.key;
            entry_data = ltm_buffer + entry_offset.data;

            /*
             * Perform the PTM write operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_hash_lt(unit,
                                     cur_mem_args->flags,
                                     cur_mem_args->pt,
                                     &cur_pt_dyn_hash_info,
                                     cur_mem_args->pt_static_info,
                                     entry_key,
                                     entry_data,
                                     pt_op,
                                     word_size,
                                     ltid,
                                     entry_key,
                                     entry_data,
                                     &entry_index,
                                     &rsp_ltid,
                                     &rsp_flags));

            if (!(lt_entry->flags & BCMLTM_ENTRY_FLAG_PASSTHRU)) {
                /* @@@ Process the response values - index, LTID, and flags */
                SHR_IF_ERR_EXIT
                    (bcmltm_pt_status_ireq_hash_lt(unit, ltid,
                                                   cur_mem_args->pt,
                                                   entry_index,
                                                   &cur_pt_dyn_hash_info));
            }
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

void
bcmltm_pt_dyn_info_get(uint32_t pt_flags,
                       bcmltm_wb_ptm_param_offset_t *offset,
                       uint32_t *ltm_buffer,
                       bcmbd_pt_dyn_info_t *dyn_info)
{
    if (pt_flags & BCMLTM_PT_OP_FLAGS_INDEX) {
        /*
         * Add the cur_index to the memen_spec.
         */
        dyn_info->index = ltm_buffer[offset->index];
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_PORT) {
        /*
         * Add the port number to the PT dynamic info.
         */
        dyn_info->tbl_inst = ltm_buffer[offset->port];
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) {
        /*
         * Add the table instance to the PT dynamic info
         * Note, only one of port or table instance should
         * be specified for a given PT operation.
         */
        dyn_info->tbl_inst = ltm_buffer[offset->table_inst];
    }
}


int
bcmltm_ee_node_schan_write(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie)
{
    uint32_t ptix, opix;
    uint32_t pt_flags;
    uint32_t pt_count, pt_sbus_count;
    uint32_t *cur_entry;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offset_t param_offset;
    bcmltm_wb_ptm_entry_index_offset_t entry_offset;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint32_t rsp_flags;
    uint32_t ltid, ptid;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    pt_count = BCMLTM_PT_LIST_NUM(node_cookie);

    for (ptix = 0; ptix < pt_count; ptix++) {
        cur_mem_args = BCMLTM_PT_LIST_MEM_ARGS(node_cookie, ptix);
        pt_op_list = BCMLTM_PT_LIST_OPS_LIST(node_cookie, ptix);
        pt_sbus_count = pt_op_list->num_pt_ops;
        pt_flags = pt_op_list->flags;

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

            /* Get entry section */
            (void)bcmltm_wb_ptm_entry_index_offset_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offset);
            cur_entry = ltm_buffer + entry_offset.data;

            /*
             * Perform the PTM write operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_write(unit,
                                   cur_mem_args->flags,
                                   ptid,
                                   &cur_pt_dyn_info,
                                   cur_mem_args->pt_static_info,
                                   cur_entry,
                                   ltid,
                                   &rsp_ltid,
                                   &rsp_flags));

            if (!(lt_entry->flags & BCMLTM_ENTRY_FLAG_PASSTHRU)) {
                /* @@@ Process the response values - LTID and flags */
                SHR_IF_ERR_EXIT
                    (bcmltm_pt_status_ireq_op(unit, ltid, ptid,
                                              &cur_pt_dyn_info));
            }
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_schan_read(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    uint32_t ptix, opix;
    uint32_t pt_flags;
    uint32_t pt_count, pt_sbus_count, pt_word_size;
    uint32_t *cur_entry;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_pt_op_list_t *pt_op_list;
    bcmltm_wb_ptm_param_offset_t param_offset;
    bcmltm_wb_ptm_entry_index_offset_t entry_offset;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint32_t req_flags, rsp_flags;
    uint32_t ltid, ptid;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    pt_count = BCMLTM_PT_LIST_NUM(node_cookie);

    for (ptix = 0; ptix < pt_count; ptix++) {
        cur_mem_args = BCMLTM_PT_LIST_MEM_ARGS(node_cookie, ptix);
        pt_op_list = BCMLTM_PT_LIST_OPS_LIST(node_cookie, ptix);
        pt_sbus_count = pt_op_list->num_pt_ops;
        pt_word_size = pt_op_list->word_size;
        pt_flags = pt_op_list->flags;

        /* Permit HW forced read */
        req_flags = cur_mem_args->flags;
        if (lt_entry->flags & BCMLTM_ENTRY_FLAG_HW_GET) {
            req_flags |= BCMPTM_REQ_FLAGS_DONT_USE_CACHE;
        }

        for (opix = 0; opix < pt_sbus_count; opix++) {
            /* Fresh copy for any adjustments with PT_OP_FLAGS */
            sal_memcpy(&cur_pt_dyn_info, cur_mem_args->pt_dyn_info,
                       sizeof(bcmbd_pt_dyn_info_t));

            /* Get PTM parameter offsets */
            (void)bcmltm_wb_ptm_param_offset_get(
                pt_op_list->param_offsets[opix],
                &param_offset);

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

            /* Get entry section */
            (void)bcmltm_wb_ptm_entry_index_offset_get(
                pt_op_list->buffer_offsets[opix],
                &entry_offset);
            cur_entry = ltm_buffer + entry_offset.data;

            /*
             * Perform the PTM write operation.
             */
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_read(unit,
                                  req_flags,
                                  ptid,
                                  &cur_pt_dyn_info,
                                  cur_mem_args->pt_static_info,
                                  pt_word_size,
                                  ltid,
                                  cur_entry,
                                  &rsp_ltid,
                                  &rsp_flags));

            if (!(lt_entry->flags & BCMLTM_ENTRY_FLAG_PASSTHRU)) {
                /* @@@ Process the response values - LTID and flags */
                SHR_IF_ERR_EXIT
                    (bcmltm_pt_status_ireq_op(unit, ltid, ptid,
                                              &cur_pt_dyn_info));
            }
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_pt_hash_insert(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    return bcmltm_pt_hash_opcode(unit, lt_md,
                                 lt_state, lt_entry,
                                 ltm_buffer, node_cookie,
                                 BCMPTM_OP_INSERT);
}

int
bcmltm_ee_node_pt_hash_delete(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    return bcmltm_pt_hash_opcode(unit, lt_md,
                                 lt_state, lt_entry,
                                 ltm_buffer, node_cookie,
                                 BCMPTM_OP_DELETE);
}

int
bcmltm_ee_node_pt_hash_lookup(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    int rv;

    rv = bcmltm_pt_hash_opcode(unit, lt_md,
                                lt_state, lt_entry,
                                ltm_buffer, node_cookie,
                                BCMPTM_OP_LOOKUP);

    return rv;
}


int
bcmltm_ee_node_pt_hash_lookup_not_found(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie)
{
    int rv;

    rv = bcmltm_pt_hash_opcode(unit, lt_md,
                                lt_state, lt_entry,
                                ltm_buffer, node_cookie,
                                BCMPTM_OP_LOOKUP);

    if (rv == SHR_E_NONE) {
        /*
         * Finding a previous entry is a failure.
         * INSERT must reference a new entry key.
         */
        LOG_VERBOSE(BSL_LS_BCMLTM_EXECUTIONENGINE,
                    (BSL_META_U(unit,
                                "Unanticipated existing keyed entry\n")));
        rv = SHR_E_EXISTS;
    } else if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

    return rv;
}
