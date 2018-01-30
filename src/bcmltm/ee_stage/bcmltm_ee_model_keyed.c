/*! \file bcmltm_ee_model_keyed.c
 *
 * Logical Table Manager Execution Engine
 *
 * Logical table keyed implementations
 *
 * This file contains the LTM EE stage functions for LT operations
 * on modeled keyed tables.
 *
 * ALLOC  - not supported for modeled keyed LTs
 * INSERT
 * LOOKUP
 * DELETE
 * UPDATE
 *
 * To support these basic operations on modeled keyed LTs, the following
 * capabilities are required:
 *
 * PT insert
 * PT delete
 * PT lookup
 *
 * Note that the PTM interface uses the same functions for
 * hash and TCAM keyed tables.  The implementation of the LTM-PTM
 * interface here is common for the two table types.
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
 * \brief Fill in a PT keyed memory request information structure.
 *
 * Prepare the memory request information structure for a keyed PT op
 * from the parameters and metadata of the LTM.
 *
 * This function will need expansion for TCAMs and PDD support.
 *
 * \param [in,out] mreq_info Pointer to the keyed memory request structure
 * \param [in] entry_key_list Pointer to the start of the array of arrays
 *                            holding the PT key entries.
 * \param [in] entry_data_list Pointer to the start of the array of arrays
 *                             holding the PT data entries.
 * \param [in] same_key Boolean value of whether this PT key has already
 *                      been used as the immediate prior keyed PT op
 *                      during this LTM operation.
 * \param [in] key_word_size The count of uint32_t values in each key entry
 *                            array element.
 * \param [in] data_word_size The count of uint32_t values in each data entry
 *                            array element.
 * \param [in] priority The TCAM priority specified by the application.
 */
static void
keyed_mreq_info_init(bcmptm_keyed_lt_mreq_info_t *mreq_info,
                     uint32_t **entry_key_list,
                     uint32_t **entry_data_list,
                     bool same_key,
                     uint8_t key_word_size,
                     uint8_t data_word_size,
                     uint32_t priority)
{
    mreq_info->entry_key_words = entry_key_list;
    mreq_info->entry_data_words = entry_data_list;
    mreq_info->pdd_l_dtyp = NULL; /* For PDD */
        
    mreq_info->entry_id = 0; /* For TCAMs */
    mreq_info->entry_pri = priority; /* For TCAMs */
    mreq_info->same_key = same_key;
    mreq_info->rsp_entry_key_buf_wsize = key_word_size;
    mreq_info->rsp_entry_data_buf_wsize = data_word_size;
}

/*!
 * \brief Fill in a PT keyed memory response information structure.
 *
 * Prepare the memory response information structure for a keyed PT op
 * from the parameters and metadata of the LTM.
 *
 * This function will need expansion for TCAMs and PDD support.
 *
 * \param [in,out] mrsp_info Pointer to the keyed memory response structure
 * \param [in] rsp_entry_key_list Pointer to the start of the array of
 *                                arrays holding the PT key entries.
 * \param [in] rsp_entry_data_list Pointer to the start of the array of
 *                                 arrays holding the PT data entries.
 */
static void
keyed_mrsp_info_init(bcmptm_keyed_lt_mrsp_info_t *mrsp_info,
                     uint32_t **rsp_entry_key_list,
                     uint32_t **rsp_entry_data_list)
{
    /* These values must be initialized to provide memory for the returned
     * output values */
    mrsp_info->rsp_entry_key_words = rsp_entry_key_list;
    mrsp_info->rsp_entry_data_words = rsp_entry_data_list;
    mrsp_info->rsp_pdd_l_dtyp = NULL; /* For PDD */
                                  

    /* These are clearing the structure in preparation to receive output */
    /* For all Keyed */
    mrsp_info->rsp_entry_pipe = 0;
    /* For TCAMs */
    mrsp_info->rsp_entry_pri = 0;
    mrsp_info->rsp_entry_sid_cnt = 0;
    mrsp_info->rsp_entry_index_cnt = 0;
    mrsp_info->rsp_entry_sid_data_cnt = 0;
    mrsp_info->rsp_entry_index_data_cnt = 0;
    sal_memset(mrsp_info->rsp_entry_sid, 0,
               sizeof(mrsp_info->rsp_entry_sid));
    sal_memset(mrsp_info->rsp_entry_index, 0,
               sizeof(mrsp_info->rsp_entry_index));
    sal_memset(mrsp_info->rsp_entry_sid_data, 0,
               sizeof(mrsp_info->rsp_entry_sid_data));
    sal_memset(mrsp_info->rsp_entry_index_data, 0,
               sizeof(mrsp_info->rsp_entry_index_data));
}

/*!
 * \brief Common logic for LTM-PTM interface of keyed PT table ops.
 *
 * This function implements the common logic for the LTM-PTM interface
 * to dispatch INSERT, LOOKUP, and DELETE ops for keyed (hash or TCAM)
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
bcmltm_ee_node_model_keyed_opcode(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie,
                                  bcmptm_op_type_t pt_op)
{
    uint32_t pt_flags;
    uint8_t key_word_size;
    uint8_t data_word_size;
    uint32_t entix;
    bcmltm_pt_keyed_op_info_t *keyed_op_info;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_wb_ptm_param_offset_t param_offset;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint32_t rsp_flags;
    uint32_t req_flags;
    uint32_t ltid;
    bcmptm_keyed_lt_mreq_info_t mreq_info;
    bcmptm_keyed_lt_mrsp_info_t mrsp_info;
    uint32_t *input_entry_key_list[BCMLTM_PT_ENTRY_LIST_MAX];
    uint32_t *input_entry_data_list[BCMLTM_PT_ENTRY_LIST_MAX];
    uint32_t *output_entry_key_list[BCMLTM_PT_ENTRY_LIST_MAX];
    uint32_t *output_entry_data_list[BCMLTM_PT_ENTRY_LIST_MAX];
    int rv;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    keyed_op_info = BCMLTM_KEYED_OP_INFO(node_cookie);

    cur_mem_args = keyed_op_info->mem_args;
    key_word_size = keyed_op_info->key_word_size;
    data_word_size = keyed_op_info->data_word_size;
    pt_flags = keyed_op_info->flags;

    /* Permit HW forced (bypass PT cache) */
    req_flags = cur_mem_args->flags;
    if (lt_entry->flags & BCMLTM_ENTRY_FLAG_HW_GET) {
        req_flags |= BCMPTM_REQ_FLAGS_DONT_USE_CACHE;
    }

    /* Get PTM parameter offsets */
    (void)bcmltm_wb_ptm_param_offset_get(keyed_op_info->param_offset,
                                         &param_offset);

    /*
     * Check PT entry changed flag.
     *
     * Note that the current Keyed table implementation allows only
     * one PT association.  As such this flag is currently never set.
     * Nevertheless, this PT entry changed logic is present to allow
     * for easy future adaptations in case Keyed table implementations
     * changes.
     */
    if (pt_flags & BCMLTM_PT_OP_FLAGS_PT_CHANGED) {
        if (ltm_buffer[param_offset.pt_changed] !=
            BCMLTM_PT_ENTRY_CHANGED_VALUE) {
            SHR_EXIT();
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

    /* Prepare the entry arrays */
    for (entix = 0; entix < BCMLTM_PT_ENTRY_LIST_MAX; entix++) {
        input_entry_key_list[entix] = ltm_buffer +
            keyed_op_info->input_key_buffer_offset[entix];

        input_entry_data_list[entix] = ltm_buffer +
            keyed_op_info->input_data_buffer_offset[entix];

        output_entry_key_list[entix] = ltm_buffer +
            keyed_op_info->output_key_buffer_offset[entix];

        output_entry_data_list[entix] = ltm_buffer +
            keyed_op_info->output_data_buffer_offset[entix];
    }

    /* Set up mreq info */
    keyed_mreq_info_init(&mreq_info,
                         input_entry_key_list,
                         input_entry_data_list,
                         (pt_op == BCMPTM_OP_INSERT),
                         key_word_size,
                         data_word_size,
                         ltm_buffer[param_offset.priority]);

    /* Set up mrsq info */
    keyed_mrsp_info_init(&mrsp_info,
                         output_entry_key_list,
                         output_entry_data_list);

    /*
     * Perform the PTM write operation.
     */
    bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                           BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
    rv = bcmptm_mreq_keyed_lt(unit,
                              req_flags,
                              ltid,
                              &cur_pt_dyn_info,
                              cur_mem_args->pt_static_info,
                              pt_op,
                              &mreq_info,
                              lt_entry->trans_id, /* cseq_id */
                              &mrsp_info,
                              &rsp_ltid,
                              &rsp_flags);

    if (rv == SHR_E_NOT_FOUND) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    } else if (rv == SHR_E_PARAM) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    /* @@@ Process the response values - LTID and flags */
    SHR_IF_ERR_EXIT
        (bcmltm_pt_status_mreq_keyed_lt(unit, ltid,
                                        &cur_pt_dyn_info,
                                        &mrsp_info));

    if (pt_flags & BCMLTM_PT_OP_FLAGS_PRIORITY) {
        /* Copy response TCAM priority to WB */
        ltm_buffer[param_offset.priority] =
            mrsp_info.rsp_entry_pri;
    }

    if ((pt_flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) &&
        ((pt_op == BCMPTM_OP_GET_NEXT) ||
         (pt_op == BCMPTM_OP_GET_FIRST))) {
        /* Copy response pipe to WB */
        ltm_buffer[param_offset.table_inst] =
            mrsp_info.rsp_entry_pipe;
    }

exit:
    if (SHR_FUNC_ERR() &&
        !(SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND) ||
          SHR_FUNC_VAL_IS(SHR_E_EXISTS))) {  /* Unexpected errors */
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_ee_node_model_keyed_insert(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                           lt_state, lt_entry,
                                           ltm_buffer, node_cookie,
                                           BCMPTM_OP_INSERT));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_model_keyed_delete(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                           lt_state, lt_entry,
                                           ltm_buffer, node_cookie,
                                           BCMPTM_OP_DELETE));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_model_keyed_lookup(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    int rv;

    rv = bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_LOOKUP);
    if (rv == SHR_E_NOT_FOUND) {
        /*
         * An existing keyed entry matching this lookup
         * is required for this function node.
         */
        LOG_VERBOSE(BSL_LS_BCMLTM_EXECUTIONENGINE,
                    (BSL_META_U(unit,
                              "Required existing keyed entry not found\n")));
    }

    return rv;
}

int
bcmltm_ee_node_model_keyed_lookup_not_found(int unit,
                                            bcmltm_lt_md_t *lt_md,
                                            bcmltm_lt_state_t *lt_state,
                                            bcmltm_entry_t *lt_entry,
                                            uint32_t *ltm_buffer,
                                            void *node_cookie)
{
    int rv;

    rv = bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_LOOKUP);

    if (rv == SHR_E_NONE) {
        /*
         *Finding a previous entry is a failure.
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

int
bcmltm_ee_node_model_keyed_traverse(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    bcmptm_op_type_t pt_op;

    pt_op =
        ((lt_entry->flags & BCMLTM_ENTRY_FLAG_TRAVERSE_START) == 0) ?
        BCMPTM_OP_GET_NEXT : BCMPTM_OP_GET_FIRST;

    return bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             pt_op);
}
