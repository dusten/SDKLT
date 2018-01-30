/*! \file bcmltm.c
 *
 * Logical Table Manager
 *
 * This module contains the control implementation of Logical Table
 * Management functions.
 *
 * LTM northbound APIs
 * Transaction tracking, including commit and abort
 * Invocation of LTM abstract function node trees
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
#include <bsl/bsl.h>

#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_ha_internal.h>
#include <bcmltm/bcmltm_md.h>
#include <bcmltm/bcmltm_stats_internal.h>
#include <bcmltm/bcmltm_md_pthru_internal.h>
#include <bcmltm/bcmltm_md_logical_internal.h>
#include <bcmltm/bcmltm_lta_cth_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_ENTRY

bcmltm_transaction_status_t *bcmltm_trans_status[BCMDRD_CONFIG_MAX_UNITS];

uint32_t *working_buffer_modeled[BCMDRD_CONFIG_MAX_UNITS];
uint32_t *working_buffer_interactive[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Accessor macros
 */
#define LTM_TRANS_STATUS(_u)         bcmltm_trans_status[(_u)]
#define LTM_TRANS_LT_NUM(_u)         (bcmltm_trans_status[(_u)]->lt_num)
#define LTM_TRANS_LT_TOP(_u)         ((bcmltm_trans_status[(_u)]->lt_num) - 1)

/*!
 * \brief Table Commit Handler Interfaces.
 *
 * This enumeration contains the types of interfaces in a
 * Table Commit handler that can be called.
 */
typedef enum table_commit_intf_e {
    /*! Prepare the transaction. */
    TABLE_COMMIT_INTF_PREPARE = 0,

    /*! Commit the transaction. */
    TABLE_COMMIT_INTF_COMMIT = 1,

    /*! Abort the transaction. */
    TABLE_COMMIT_INTF_ABORT = 2,
} table_commit_intf_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Provide a string pointer to the LT opcode name.
 *
 * This function returns a string pointer to the LT opcode name.
 *
 * \param [in] opcode LT opcode.
 *
 * \retval String pointer to the LT opcode name.
 */
static const char *
ltm_lt_opcode_str(bcmlt_opcode_t opcode)
{
    switch (opcode) {
    case BCMLT_OPCODE_NOP:
        return "NOP";
    case BCMLT_OPCODE_INSERT:
        return "INSERT";
    case BCMLT_OPCODE_LOOKUP:
        return "LOOKUP";
    case BCMLT_OPCODE_DELETE:
        return "DELETE";
    case BCMLT_OPCODE_UPDATE:
        return "UPDATE";
    case BCMLT_OPCODE_TRAVERSE:
        return "TRAVERSE";
    default:
        return "UNKNOWN";
    }
}

/*!
 * \brief Provide a string pointer to the PT opcode name.
 *
 * This function returns a string pointer to the PT opcode name.
 *
 * \param [in] opcode PT opcode.
 *
 * \retval String pointer to the PT opcode name.
 */
static const char *
ltm_pt_opcode_str(bcmlt_pt_opcode_t opcode)
{
    switch (opcode) {
    case BCMLT_PT_OPCODE_NOP:
        return "NOP";
    case BCMLT_PT_OPCODE_FIFO_POP:
        return "FIFO_POP";
    case BCMLT_PT_OPCODE_FIFO_PUSH:
        return "FIFO_PUSH";
    case BCMLT_PT_OPCODE_SET:
        return "SET";
    case BCMLT_PT_OPCODE_MODIFY:
        return "MODIFY";
    case BCMLT_PT_OPCODE_GET:
        return "GET";
    case BCMLT_PT_OPCODE_CLEAR:
        return "CLEAR";
    case BCMLT_PT_OPCODE_INSERT:
        return "INSERT";
    case BCMLT_PT_OPCODE_DELETE:
        return "DELETE";
    case BCMLT_PT_OPCODE_LOOKUP:
        return "LOOKUP";
    default:
        return "UNKNOWN";
    }
}

/*!
 * \brief Provide a string pointer to the LT name.
 *
 * This function returns a string pointer to the logical table name
 * for the given LT ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 *
 * \retval String pointer to the LT name.
 */
static const char *
ltm_lt_name_str(int unit, uint32_t sid)
{
    const bcmlrd_table_rep_t *tbl;
    const char *name = NULL;

    tbl = bcmlrd_table_get(sid);
    if (tbl != NULL) {
        name = tbl->name;
    }

    if (name == NULL) {
        name = "UNKNOWN";
    }

    return name;
}

/*!
 * \brief Provide a string pointer to the PT name.
 *
 * This function returns a string pointer to the physical table name
 * for the given PT ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical Table ID.
 *
 * \retval String pointer to the PT name.
 */
static const char *
ltm_pt_name_str(int unit, uint32_t sid)
{
    const char *name = NULL;

    name = bcmdrd_pt_sid_to_name(unit, sid);

    if (name == NULL) {
        name = "UNKNOWN";
    }

    return name;
}

/*!
 *
 * \brief Allocate Working Buffers for a unit
 *
 * The Working Buffer space for a given unit, both modeled and
 * interactive modes, is allocated during initialization rather than
 * at the start of every operation.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_wb_init(int unit)
{
    uint32_t wb_size_max, pt_wb_size_max;
    SHR_FUNC_ENTER(unit);

    if ((working_buffer_modeled[unit] != NULL) ||
        (working_buffer_interactive[unit] != NULL)) {
        /* This shouldn't happen? */
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Create Modeled path Working Buffer space for this unit. */
    wb_size_max = 0;
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_wb_max_get(unit, BCMLTM_TABLE_MODE_MODELED,
                                      &wb_size_max));
    SHR_ALLOC(working_buffer_modeled[unit], wb_size_max,
              "LTM Modeled WB");
    if (working_buffer_modeled[unit] == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  (BSL_META_U(unit,
                              "Insufficient memory for modeled path"
                              "working buffer\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    /* Create Interactive path Working Buffer space for this unit. */
    wb_size_max = 0;
    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_wb_max_get(unit, BCMLTM_TABLE_MODE_INTERACTIVE,
                                      &wb_size_max));

    pt_wb_size_max = 0;
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_wb_max_get(unit, &pt_wb_size_max));

    if (pt_wb_size_max > wb_size_max) {
        wb_size_max = pt_wb_size_max;
    }

    SHR_ALLOC(working_buffer_interactive[unit], wb_size_max,
               "LTM Interactive WB");
    if (working_buffer_interactive[unit] == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  (BSL_META_U(unit,
                              "Insufficient memory for interactive path"
                              "working buffer\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Cleanup Working Buffers for a unit
 *
 * The Working Buffer space for a given unit, both modeled and
 * interactive modes, is freed during cleaup.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static void
bcmltm_wb_cleanup(int unit)
{
    if (working_buffer_modeled[unit] != NULL) {
        SHR_FREE(working_buffer_modeled[unit]);
        working_buffer_modeled[unit] = NULL;
    }

    if (working_buffer_interactive[unit] != NULL) {
        SHR_FREE(working_buffer_interactive[unit]);
        working_buffer_interactive[unit] = NULL;
   }
}

/*!
 *
 * \brief Initialize transaction management of LTM
 *
 * Create the transaction record memory organization within the
 * LTM HA transaction memory block.  Also prepare the state rollback
 * memory areas.
 *
 * The HA memory regions are not set to any initialized state
 * in this function.  The LTM HA module handles this properly
 * whether Cold Boot or HA recovery.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_transaction_init(int unit)
{
    uint8_t trans_lt_max, lt_num;
    bcmltm_ha_ptr_t trans_status_hap;
    uint32_t trans_status_size, state_size, state_data_size;
    bcmltm_state_mgmt_t *state_mgmt;
    bcmltm_ha_ptr_t state_rollback_hap, data_rollback_hap;
    void *state_rollback, *data_rollback;

    SHR_FUNC_ENTER(unit);

    trans_lt_max = bcmltm_ha_transaction_max(unit);
    state_mgmt = bcmltm_state_mgmt_get(unit);
    SHR_NULL_CHECK(state_mgmt, SHR_E_INIT);

    trans_status_size = sizeof(bcmltm_transaction_status_t) +
        (trans_lt_max * sizeof(uint32_t));

    trans_status_hap =
        bcmltm_ha_alloc(unit,
                        BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS,
                        trans_status_size);
    LTM_TRANS_STATUS(unit) =
        bcmltm_ha_ptr_to_mem_ptr(unit, trans_status_hap);
    SHR_NULL_CHECK(LTM_TRANS_STATUS(unit), SHR_E_MEMORY);
    LTM_TRANS_STATUS(unit)->ltid_list = (uint32_t *)
        (((uint8_t *)LTM_TRANS_STATUS(unit)) +
         sizeof(bcmltm_transaction_status_t));

    SHR_ALLOC(state_mgmt->lt_state_rollback_chunk_hap,
              trans_lt_max * sizeof(bcmltm_ha_ptr_t),
              "LTM State Rollback HA Pointers");
    SHR_NULL_CHECK(state_mgmt->lt_state_rollback_chunk_hap, SHR_E_MEMORY);
    sal_memset(state_mgmt->lt_state_rollback_chunk_hap, 0,
               trans_lt_max * sizeof(bcmltm_ha_ptr_t));

    SHR_ALLOC(state_mgmt->lt_state_data_rollback_chunk_hap,
              trans_lt_max * sizeof(bcmltm_ha_ptr_t),
              "LTM State Data Rollback HA Pointers");
    SHR_NULL_CHECK(state_mgmt->lt_state_data_rollback_chunk_hap,
                   SHR_E_MEMORY);
    sal_memset(state_mgmt->lt_state_data_rollback_chunk_hap, 0,
               trans_lt_max * sizeof(bcmltm_ha_ptr_t));

    state_size = state_mgmt->maximum_state_size;
    state_data_size = state_mgmt->maximum_state_data_size;

    for (lt_num = 0; lt_num < trans_lt_max; lt_num++) {
        state_rollback_hap =
            bcmltm_ha_alloc(unit,
                            BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE,
                            state_size);
        state_rollback = bcmltm_ha_ptr_to_mem_ptr(unit, state_rollback_hap);
        SHR_NULL_CHECK(state_rollback, SHR_E_MEMORY);
        state_mgmt->lt_state_rollback_chunk_hap[lt_num] = state_rollback_hap;

        data_rollback_hap =
            bcmltm_ha_alloc(unit,
                            BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE_DATA,
                            state_data_size);
        data_rollback = bcmltm_ha_ptr_to_mem_ptr(unit, data_rollback_hap);
        SHR_NULL_CHECK(data_rollback, SHR_E_MEMORY);
        state_mgmt->lt_state_data_rollback_chunk_hap[lt_num] =
            data_rollback_hap;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Erase transaction status for a unit
 *
 * Erase the transaction record of a given unit upon termination of
 * the transaction (commit or abort).  Also clear the state rollback
 * memory areas.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static void
bcmltm_transaction_clear(int unit)
{
    LTM_TRANS_LT_NUM(unit) = 0;
    return;
}

/*!
 *
 * \brief Check transaction status of LT entry operation
 *
 * Verify that a LT entry operation is valid within a transaction.
 * Validate the transaction ID is in proper sequence.
 * Note if this LTID has already been referenced by this transaction.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] ltid Logical Table ID.
 * \param [out] new_trans_ltid
 *        TRUE This LTID is not already include in in this transaction.
 *        FALSE This LTID is already in this transaction
 *
 * \retval SHR_E_NONE No errors
 */
static int
ltm_transaction_update(int unit,
                       uint32_t trans_id,
                       uint32_t ltid,
                       bool *new_trans_ltid)
{
    bcmltm_transaction_status_t *trans_status;
    uint32_t ltix, lt_num;

    trans_status = LTM_TRANS_STATUS(unit);
    lt_num = LTM_TRANS_LT_NUM(unit);

    if (lt_num == 0) {
        /* Start of transaction after previous commit */
        trans_status->trans_id = trans_id;
    } else if (trans_status->trans_id != trans_id) {
        /* Mismatched transaction ID during incomplete transaction.
         * This should be caught at TRM layer.
         */
        return SHR_E_INTERNAL;
    } /* Else, new entry in this transaction */

    for (ltix = 0; ltix < lt_num; ltix++) {
        if (trans_status->ltid_list[ltix] == ltid) {
            /* Found this entry's LTID in the transaction list */
            *new_trans_ltid = FALSE;
            return SHR_E_NONE;
        }
    }

    /* Didn't find this LTID in the existing transaction list, so add
     * it if there is room. */
    if (lt_num >= BCMLTM_TRANSACTION_LT_MAX) {
        /* No room to add LTID to transaction list */
        return SHR_E_RESOURCE;
    }

    *new_trans_ltid = TRUE;
    trans_status->ltid_list[lt_num] = ltid;
    LTM_TRANS_LT_NUM(unit)++;

    return SHR_E_NONE;
}

/*!
 * \brief Cleanup LTM transaction and state management information.
 *
 * This function frees any physical memory allocated
 * for the LTM transaction and state management information.
 *
 * HA memory is left as is in case of future Warm Boot.
 *
 * \param [in] unit Unit number.
 */
static void
bcmltm_transaction_cleanup(int unit)
{
    uint8_t trans_lt_max;
    uint32_t trans_status_size;
    bcmltm_transaction_status_t *trans_status;
    bcmltm_state_mgmt_t *state_mgmt;

    bcmltm_transaction_clear(unit);

    /* Cleanup Transaction Status */
    trans_status = LTM_TRANS_STATUS(unit);
    if (trans_status != NULL) {
        trans_lt_max = bcmltm_ha_transaction_max(unit);
        trans_status_size = sizeof(bcmltm_transaction_status_t) +
            (trans_lt_max * sizeof(uint32_t));
        sal_memset(trans_status, 0, trans_status_size);
    }

    state_mgmt = bcmltm_state_mgmt_get(unit);
    if (state_mgmt != NULL) {
        if (state_mgmt->lt_state_rollback_chunk_hap != NULL) {
            SHR_FREE(state_mgmt->lt_state_rollback_chunk_hap);
        }
        if (state_mgmt->lt_state_data_rollback_chunk_hap != NULL) {
            SHR_FREE(state_mgmt->lt_state_data_rollback_chunk_hap);
        }
    }
}

/*!
 *
 * \brief Initialize LTM state data.
 *
 * This routine initializes the state data information
 * for all logical tables.
 *
 * PT Pass Thru tables do no have any state data information.
 *
 * Note: Cleanup is not necessary since the state data
 * cleanup happens during the LT state cleanup (part of the LT metadata
 * destroy).
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltm_state_data_init(int unit, bool warm)
{
    bcmltm_md_t *ltm_md;
    uint32_t i;
    uint32_t sid;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        /* Nothing to do */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_retrieve(unit, &ltm_md));

    for (i = 0; i < ltm_md->lt_max; i++) {
        /* Skip undefined tables */
        if (ltm_md->lt_md[i] == NULL) {
            continue;
        }

        /* Table ID is the index of the LT metadata */
        sid = i;
        SHR_IF_ERR_EXIT
            (bcmltm_md_logical_state_data_reset(unit, sid));
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Call the specified Table Commit handler interface.
 *
 * This routine calls the Table Commit handler for the specified
 * interface, prepare, commit, or abort.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] tc_list Table Commit handler list.
 * \param [in] intf Handler interface type to call TABLE_COMMIT_INTF_.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_commit_process(int unit,
                     uint32_t trans_id,
                     const bcmltm_table_commit_list_t *tc_list,
                     table_commit_intf_t intf)
{
    const bcmltd_table_commit_handler_t *handler;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (tc_list == NULL) {
        SHR_EXIT();
    }

    for (i = 0; i < tc_list->num_handlers; i++) {
        handler = tc_list->handlers[i];
        if (handler == NULL) {
            continue;
        }

        if (intf == TABLE_COMMIT_INTF_PREPARE) {
            SHR_IF_ERR_EXIT
                (handler->prepare(unit, trans_id, handler->arg));
        } else if (intf == TABLE_COMMIT_INTF_COMMIT) {
            SHR_IF_ERR_EXIT
                (handler->commit(unit, trans_id, handler->arg));
        } else if (intf == TABLE_COMMIT_INTF_ABORT) {
            SHR_IF_ERR_EXIT
                (handler->abort(unit, trans_id, handler->arg));
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Process one LT entry operation.
 *
 * The LTM implementation of a single LT entry operation.
 *
 * Receive a single logical table entry update from the LT operaton handler.
 * Perform a sequence of Field Adaptation and Execution Engine operations
 * for the relevant API op requested.
 * Provide appropriate responses via API entry and/or notification
 * callbacks.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors
 */
static int
ltm_entry_operation(bcmltm_entry_t *ltm_entry)
{
    /*
     * These memory entries should be pointers, implmented as a bank of
     * pre-allocated buffers from which they may be drawn.
     * This will be similar to the RX buffer methodology.
     */
    uint32_t unit = ltm_entry->unit;
    uint32_t *working_buffer = NULL;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_state_t *lt_state = NULL;
    bcmltm_ha_ptr_t lt_state_hap;
    bcmltm_lt_op_md_t *op_md;
    uint32_t trix, opix;
    bcmltm_table_catg_t table_catg;
    bcmltm_field_stats_t op_stat, op_err_stat;
    bool new_trans_ltid;
    bool extra_trans_ltid;
    uint32_t extra_ltid;
    bcmltm_lt_md_t *extra_lt_md = NULL;
    bcmltm_ha_ptr_t extra_lt_state_hap;
    bcmltm_field_list_t *api_field_data;
    uint32_t table_id;
    uint32_t *lt_stats;
    const char *table_catg_str = "";
    const char *table_name = "";
    const char *opcode_str = "";

    SHR_FUNC_ENTER(unit);

    /*
     * Determine metadata relevant to the selected table
     */
    table_id = ltm_entry->table_id;
    if (ltm_entry->flags & BCMLTM_ENTRY_FLAG_PASSTHRU) {
        opix = (uint32_t) ltm_entry->opcode.pt_opcode;
        table_catg = BCMLTM_TABLE_CATG_PTHRU;
        table_catg_str = "Physical";
        opcode_str = ltm_pt_opcode_str(opix);
        table_name = ltm_pt_name_str(unit, table_id);
    } else {
        opix = (uint32_t) ltm_entry->opcode.lt_opcode;
        table_catg = BCMLTM_TABLE_CATG_LOGICAL;
        table_catg_str = "Logical";
        opcode_str = ltm_lt_opcode_str(opix);
        table_name = ltm_lt_name_str(unit, table_id);
    }

    LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                (BSL_META_U(unit,
                            "%s Table Entry Operation %s "
                            "%s (sid=%d trans_id=%d)\n"),
                 table_catg_str, opcode_str,
                 table_name, table_id, ltm_entry->trans_id));

    /* Dynamic init metadata */
    SHR_IF_ERR_EXIT(bcmltm_md_lt_retrieve(unit,
                                          table_catg,
                                          table_id, &lt_md));

    if (lt_md == NULL) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "Unrecognized %s Table %s (sid=%d)\n"),
                     table_catg_str, table_name, table_id));
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /* Working LT state */
    lt_state_hap = lt_md->params->lt_state_hap;
    if (BCMLTM_HA_PTR_IS_INVALID(lt_state_hap)) {
        /* This LT has no state, so this is not an error. */
        lt_state = NULL;
    } else {
        /* This LT should have state, so the pointer must be valid. */
        lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
        SHR_NULL_CHECK(lt_state, SHR_E_INTERNAL);
    }

    if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_MODELED) {
        SHR_IF_ERR_EXIT
            (ltm_transaction_update(unit, ltm_entry->trans_id,
                                    table_id,
                                    &new_trans_ltid));

        if (new_trans_ltid) {
            /*
             * First reference to this modeled path LT in this transaction.
             * Make a rollback copy of the LT state in case of abort.
             */
            SHR_IF_ERR_EXIT
                (bcmltm_state_clone(unit, LTM_TRANS_LT_TOP(unit), lt_state_hap,
                                    &(lt_md->params->lt_rollback_state_hap)));

            /* Call Table Commit handler to indicate start of transaction */
            SHR_IF_ERR_EXIT
                (table_commit_process(unit, ltm_entry->trans_id,
                                      lt_md->tc_list,
                                      TABLE_COMMIT_INTF_PREPARE));
        }

        /*
         * Handle the cases where other table's state may be affected
         * 1) TABLE_CONTROL, which can change the state of other LTs.
         * 2) Shared index LTs, where one table manages the global
         *    in-use bitmap.
         */
        extra_trans_ltid = FALSE;
        if ((table_catg == BCMLTM_TABLE_CATG_LOGICAL) &&
            (table_id == TABLE_CONTROLt) &&
            (opix == BCMLT_OPCODE_UPDATE)) {
            /* Need to capture the LT state of the target LT */
            api_field_data =
                bcmltm_api_find_field(ltm_entry->in_fields,
                                      TABLE_CONTROLt_TABLE_IDf,
                                      0);
            if (api_field_data == NULL) {
                LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                            (BSL_META_U(unit,
                             "Missing TABLE_CONTROL.TABLE_ID field\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            extra_ltid = api_field_data->data;
            extra_trans_ltid = TRUE;
        } else if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_OVERLAY) {
            extra_ltid = lt_md->params->global_inuse_ltid;
            if (extra_ltid != lt_md->params->ltid) {
                extra_trans_ltid = TRUE;
            }
        }

        if (extra_trans_ltid) {
            SHR_IF_ERR_EXIT
                (ltm_transaction_update(unit, ltm_entry->trans_id,
                                        extra_ltid,
                                        &new_trans_ltid));

            if (new_trans_ltid) {
                /* First reference to this modeled path LT in this transaction.
                 * Make a rollback copy of the LT state in case of abort.
                 */
                SHR_IF_ERR_EXIT
                    (bcmltm_md_lt_retrieve(unit,
                                           table_catg,
                                           extra_ltid, &extra_lt_md));

                if (extra_lt_md == NULL) {
                    LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                                (BSL_META_U(unit,
                                     "Invalid TABLE_CONTROL.TABLE_ID\n")));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM );
                }

                extra_lt_state_hap = extra_lt_md->params->lt_state_hap;
                if (BCMLTM_HA_PTR_IS_INVALID(extra_lt_state_hap)) {
                    /* No valid LT state! */
                    LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                                (BSL_META_U(unit,
                                            "Missing LT state for %s "
                                            "Table %s (sid=%d)\n"),
                                 table_catg_str, table_name, table_id));
                    return SHR_E_INTERNAL;
                }

                SHR_IF_ERR_EXIT
                    (bcmltm_state_clone(unit, LTM_TRANS_LT_TOP(unit),
                                        extra_lt_state_hap,
                            &(extra_lt_md->params->lt_rollback_state_hap)));
            }
        }

        working_buffer = working_buffer_modeled[unit];
    } else {
        working_buffer = working_buffer_interactive[unit];
    }

    op_md = lt_md->op[opix];
    if (op_md == NULL) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "Operation %s not available on this table "
                                "%s (sid=%d)\n"),
                     opcode_str, table_name, table_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NO_HANDLER);
    }

    /* Clear Working Buffer */
    sal_memset(working_buffer, 0, op_md->working_buffer_size);

    for (trix = 0; trix < op_md->num_roots; trix++) {
        /* Field Adaptation stage */
        if (op_md->fa_node_roots[trix] != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_fa_stage(unit, op_md->fa_node_roots[trix],
                                 lt_md,
                                 lt_state,
                                 ltm_entry,
                                 working_buffer));
        }

        /* Execution Engine stage */
        if (op_md->ee_node_roots[trix] != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_ee_stage(unit, op_md->ee_node_roots[trix],
                                 lt_md,
                                 lt_state,
                                 ltm_entry,
                                 working_buffer));
        }
    }

 exit:
    if (lt_md != NULL) {
        lt_stats = BCMLTM_STATS_ARRAY(lt_md);
        if (table_catg == BCMLTM_TABLE_CATG_LOGICAL) {
            op_stat = op_err_stat = BCMLRD_FIELD_STATS_NUM;
            switch (opix) {
            case BCMLT_OPCODE_INSERT:
                op_stat = BCMLRD_FIELD_LT_INSERT_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_INSERT_ERROR_COUNT;
                if (!SHR_FUNC_ERR() && (lt_state != NULL)) {
                    /* Increase inuse entries on successful INSERT */
                    (lt_state->entry_count)--;
                }
                break;
            case BCMLT_OPCODE_LOOKUP:
                op_stat = BCMLRD_FIELD_LT_LOOKUP_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_LOOKUP_ERROR_COUNT;
                break;
            case BCMLT_OPCODE_DELETE:
                op_stat = BCMLRD_FIELD_LT_DELETE_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_DELETE_ERROR_COUNT;
                if (!SHR_FUNC_ERR() && (lt_state != NULL)) {
                    /* Decrease inuse entries on successful DELETE */
                    (lt_state->entry_count)--;
                }
                break;
            case BCMLT_OPCODE_UPDATE:
                op_stat = BCMLRD_FIELD_LT_UPDATE_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_UPDATE_ERROR_COUNT;
                break;
            case BCMLT_OPCODE_TRAVERSE:
                op_stat = BCMLRD_FIELD_LT_TRAVERSE_COUNT;
                op_err_stat = BCMLRD_FIELD_LT_TRAVERSE_ERROR_COUNT;
                break;
            }
            if (SHR_FUNC_ERR()) {
                bcmltm_stats_increment(lt_stats, op_err_stat);
            }
            bcmltm_stats_increment(lt_stats, op_stat);
        }
        if (SHR_FUNC_ERR()) {
            bcmltm_stats_increment(lt_stats,
                                   BCMLRD_FIELD_LT_ERROR_COUNT);
        }
    }

    /*
     * Log verbose message only on SUCCESS.
     * Failure cases should have already logged corresponding error earlier.
     */
    if (!SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "%s %s successful (sid=%d trans_id=%d)\n"),
                     opcode_str, table_name, table_id, ltm_entry->trans_id));
    }

    SHR_FUNC_EXIT();
}


/*!
 *
 * \brief Prepare one LT entry operation for common processing
 *
 * Common implementation to ready a Logical Table operation for
 * opcode processing.
 *
 * The TRM-LTM interface uses separate functions for each opcode.
 * The LTM implementation combines them into a single LTM opcode
 * processing step.  This happens in two stages
 * 1) Combine LT (or PT PassThru) opcodes together
 * 2) Merge PT PassThru and LT opcodes processing at the common
 * implementation.
 *
 * \param [in] lt_op The LT opcode determined by the TRM function call.
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_entry_process(bcmlt_opcode_t lt_op,
                     bcmltm_entry_t *entry)
{
    if (entry == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  ("No entry structure provided to %s op\n",
                   ltm_lt_opcode_str(lt_op)));
        return SHR_E_PARAM;
    }

    if ((entry->flags & BCMLTM_ENTRY_FLAG_PASSTHRU) != 0) {
        /* Logical Table only interace */
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(entry->unit,
                                "PT PassThru flagged for %s op on LT %s\n"),
                     ltm_lt_opcode_str(lt_op),
                     ltm_lt_name_str(entry->unit, entry->table_id)));
        return SHR_E_PARAM;
    }

    entry->opcode.lt_opcode = lt_op;
    return ltm_entry_operation(entry);
}

/*!
 *
 * \brief Prepare one PT entry operation for common processing
 *
 * Common implementation to ready a PT PassThru operation for
 * opcode processing.
 *
 * The TRM-LTM interface uses separate functions for each opcode.
 * The LTM implementation combines them into a single LTM opcode
 * processing step.  This happens in two stages
 * 1) Combine PT PassThru (or LT) opcodes together
 * 2) Merge PT PassThru and LT opcodes processing at the common
 * implementation.
 *
 * \param [in] ptpt_op The PT PassThru opcode determined by the
 *                     TRM function call.
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors
 */
static int
bcmltm_entry_pt_process(bcmlt_pt_opcode_t ptpt_op,
                        bcmltm_entry_t *entry)
{
    if (entry == NULL) {
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                  ("No entry structure provided to %s op\n",
                   ltm_pt_opcode_str(ptpt_op)));
        return SHR_E_PARAM;
    }

    if ((entry->flags & BCMLTM_ENTRY_FLAG_PASSTHRU) == 0) {
        /* Only PassThru permitted in this function */
        LOG_ERROR(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(entry->unit,
                                "LT flagged for PT %s op on PT %s\n"),
                     ltm_pt_opcode_str(ptpt_op),
                     ltm_pt_name_str(entry->unit, entry->table_id)));
        return SHR_E_PARAM;
    }
    entry->opcode.pt_opcode = ptpt_op;

    return ltm_entry_operation(entry);
}

/*******************************************************************************
 * Public functions
 */

/*
 * bcmltm_init
 *
 * Initialize the metadata for managing the LTs on a specific unit
 */
int
bcmltm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Incorporate the regsfile products, PEMLA/CANCUN output, and
     * application configuration to construct the metadata
     * for managing LTs and mapping them to the PTs on which they
     * are implemented.
     */

    LOG_VERBOSE(BSL_LS_BCMLTM_LOGICALRESOURCEMANAGER,
                (BSL_META_U(unit,
                            "Logical Table Resource Manager initialization\n")));

    /* Initialize LTM metadata */
    SHR_IF_ERR_EXIT(bcmltm_md_init(unit, warm));

    /* Initialize LTM custom table handlers for this unit */
    SHR_IF_ERR_EXIT(bcmltm_lta_cth_init(unit));

    /* Initialize LTM Working Buffers */
    SHR_IF_ERR_EXIT(bcmltm_wb_init(unit));

    /* Initialize transaction management and state rollback buffers */
    SHR_IF_ERR_EXIT(bcmltm_transaction_init(unit));

    /* Register LTM internal callbacks. */
    bcmltm_stats_lt_get_register(unit, bcmltm_internal_stats_lt_get);
    bcmltm_state_lt_get_register(unit, bcmltm_internal_state_lt_get);
    bcmltm_info_lt_get_register(unit, bcmltm_internal_info_lt_get);

    /* Initialize state data */
    SHR_IF_ERR_EXIT(ltm_state_data_init(unit, warm));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Cleanup of state data is not needed */

    /* Cleanup transaction management and state rollback buffers */
    bcmltm_transaction_cleanup(unit);

    /* Cleanup LTM Working Buffers */
    bcmltm_wb_cleanup(unit);

    /* Cleanup LTM custom table handlers for this unit */
    SHR_IF_ERR_EXIT(bcmltm_lta_cth_cleanup(unit));

    /* Cleanup LTM metadata */
    SHR_IF_ERR_EXIT(bcmltm_md_cleanup(unit));

 exit:
    SHR_FUNC_EXIT();
}

/*****
 *
 * Standard LT TRM interface functions
 */

int
bcmltm_entry_insert(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_INSERT,
                                entry);
}

int
bcmltm_entry_lookup(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_LOOKUP,
                                entry);
}

int
bcmltm_entry_delete(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_DELETE,
                                entry);
}

int
bcmltm_entry_update(bcmltm_entry_t *entry)
{
    return bcmltm_entry_process(BCMLT_OPCODE_UPDATE,
                                entry);
}

int
bcmltm_entry_traverse_first(bcmltm_entry_t *entry)
{
    entry->flags |= BCMLTM_ENTRY_FLAG_TRAVERSE_START;

    return bcmltm_entry_process(BCMLT_OPCODE_TRAVERSE,
                                entry);
}

int
bcmltm_entry_traverse_next(bcmltm_entry_t *entry)
{
    entry->flags &= ~(BCMLTM_ENTRY_FLAG_TRAVERSE_START);

    return bcmltm_entry_process(BCMLT_OPCODE_TRAVERSE,
                                entry);
}

/*****
 *
 * PT PassThru TRM interface functions
 */

int
bcmltm_entry_pt_fifo_pop(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_FIFO_POP,
                                   entry);
}

int
bcmltm_entry_pt_fifo_push(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_FIFO_PUSH,
                                   entry);
}

int
bcmltm_entry_pt_set(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_SET,
                                   entry);
}

int
bcmltm_entry_pt_modify(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_MODIFY,
                                   entry);
}

int
bcmltm_entry_pt_get(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_GET,
                                   entry);
}

int
bcmltm_entry_pt_clear(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_CLEAR,
                                   entry);
}

int
bcmltm_entry_pt_insert(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_INSERT,
                                   entry);
}

int
bcmltm_entry_pt_delete(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_DELETE,
                                   entry);
}

int
bcmltm_entry_pt_lookup(bcmltm_entry_t *entry)
{
    return bcmltm_entry_pt_process(BCMLT_PT_OPCODE_LOOKUP,
                                   entry);
}

int
bcmltm_commit(int unit,
              uint32_t trans_id,
              bcmptm_trans_cb_f notif_fn,
              void *context)
{
    bcmltm_transaction_status_t *trans_status;
    bcmltm_lt_md_t *lt_md = NULL;
    uint32_t ltid, ltix;
    uint32_t rsp_flags;
    const bcmltd_table_handler_t *cth_handler = NULL;

    SHR_FUNC_ENTER(unit);

    if (trans_id == LTM_TRANS_STATUS(unit)->trans_id) {
        trans_status = LTM_TRANS_STATUS(unit);
        /* In process transaction matches transaction ID, clean up state */

        for (ltix = 0; ltix < trans_status->lt_num; ltix++) {
            ltid = trans_status->ltid_list[ltix];

            /* Fetch metadata - only Logical Tables will commit */
            SHR_IF_ERR_EXIT(bcmltm_md_lt_retrieve(unit,
                                                  BCMLTM_TABLE_CATG_LOGICAL,
                                                  ltid, &lt_md));

            if (lt_md == NULL) {
                const char *table_name = ltm_lt_name_str(unit, ltid);

                LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                            (BSL_META_U(unit,
                                        "Unrecognized Logical Table "
                                        "%s (sid=%d trans_id=%d)\n"),
                             table_name, ltid, trans_id));
                SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            }

            /* Call Table Commit handler for commit */
            SHR_IF_ERR_EXIT
                (table_commit_process(unit, trans_id,
                                      lt_md->tc_list,
                                      TABLE_COMMIT_INTF_COMMIT));

            /* If LT is CTH, call handler commit */
            cth_handler = lt_md->cth;
            if (cth_handler != NULL) {
                if (cth_handler->ha != NULL) {
                    SHR_IF_ERR_EXIT
                        (cth_handler->ha->commit(unit, trans_id,
                                                 cth_handler->arg));
                }
            }

            /* Discard rollback LT state */
            BCMLTM_HA_PTR_INVALID_SET(lt_md->params->lt_rollback_state_hap);
        }

        /* Clear transaction record */
        bcmltm_transaction_clear(unit);
    }

    /* Whether we found the record or not, we must pass this
     * notification on to the PTM. */
    SHR_IF_ERR_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                BCMPTM_REQ_FLAGS_COMMIT,
                                0,
                                NULL,
                                NULL,
                                NULL,      /* misc_info */
                                BCMPTM_OP_NOP,
                                NULL,
                                0,
                                0,
                                trans_id,  /* cseq_id */
                                notif_fn,  /* notify_fn_ptr */
                                context,   /* notify info (cookie) */
                                NULL,
                                NULL,
                                &rsp_flags));

exit:

    /*
     * Log verbose message only on SUCCESS.
     * Failure cases should have already logged corresponding error earlier.
     */
    if (!SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "LTM commit successful trans_id=%d\n"),
                     trans_id));
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_abort(int unit,
             uint32_t trans_id)
{
    bcmltm_transaction_status_t *trans_status;
    bcmltm_lt_md_t *lt_md = NULL;
    uint32_t ltid, ltix;
    uint32_t rsp_flags;
    const bcmltd_table_handler_t *cth_handler = NULL;

    SHR_FUNC_ENTER(unit);

    if (trans_id == LTM_TRANS_STATUS(unit)->trans_id) {
        trans_status = LTM_TRANS_STATUS(unit);
        /* In process transaction matches transaction ID, clean up state */

        for (ltix = 0; ltix < trans_status->lt_num; ltix++) {
            ltid = trans_status->ltid_list[ltix];

            /* Fetch metadata - only Logical Tables will commit */
            SHR_IF_ERR_EXIT(bcmltm_md_lt_retrieve(unit,
                                                  BCMLTM_TABLE_CATG_LOGICAL,
                                                  ltid, &lt_md));

            if (lt_md == NULL) {
                const char *table_name = ltm_lt_name_str(unit, ltid);

                LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                            (BSL_META_U(unit,
                                        "Unrecognized Logical Table "
                                        "%s (sid=%d trans_id=%d)\n"),
                             table_name, ltid, trans_id));
                SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
            }

            /* Call Table Commit handler for abort */
            SHR_IF_ERR_EXIT
                (table_commit_process(unit, trans_id,
                                      lt_md->tc_list,
                                      TABLE_COMMIT_INTF_ABORT));

            /* If LT is CTH, call handler abort */
            cth_handler = lt_md->cth;
            if (cth_handler != NULL) {
                if (cth_handler->ha != NULL) {
                    SHR_IF_ERR_EXIT
                        (cth_handler->ha->abort(unit, trans_id,
                                                cth_handler->arg));
                }
            }

            if (!BCMLTM_HA_PTR_IS_INVALID(lt_md->params->lt_rollback_state_hap)) {
                /* Copy rollback LT state to working copy */
                SHR_IF_ERR_EXIT
                    (bcmltm_state_copy(unit,
                                       lt_md->params->lt_rollback_state_hap,
                                       lt_md->params->lt_state_hap));

                BCMLTM_HA_PTR_INVALID_SET(lt_md->params->lt_rollback_state_hap);
            }
        }

        /* Clear transaction record */
        bcmltm_transaction_clear(unit);
    }

    /* Whether we found the record or not, we must pass this
     * notification on to the PTM. */
    SHR_IF_ERR_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                BCMPTM_REQ_FLAGS_ABORT,
                                0,
                                NULL,
                                NULL,
                                NULL, /* misc_info */
                                BCMPTM_OP_NOP,
                                NULL,
                                0,
                                0,
                                trans_id, /* cseq_id */
                                NULL, /* notify ptr */
                                NULL, /* notify info (cookie) */
                                NULL,
                                NULL,
                                &rsp_flags));

exit:
    /*
     * Log verbose message only on SUCCESS.
     * Failure cases should have already logged corresponding error earlier.
     */
    if (!SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LS_BCMLTM_ENTRY,
                    (BSL_META_U(unit,
                                "LTM abort successful trans_id=%d\n"),
                     trans_id));
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_stats_increment_lt(int unit,
                          uint32_t ltid,
                          uint32_t stat_field)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    /* This is a CTH interface, so it should only be LTs,
     * not PT PassThru. */
    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               ltid, &lt_md);

    if (SHR_FAILURE(rv) || (lt_md == NULL)) {
        /* No way to update stats */
        return;
    }

    bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md), stat_field);
}

int
bcmltm_internal_stats_lt_get(int unit,
                             uint32_t ltid,
                             uint32_t **lt_stats_p)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               ltid, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    *lt_stats_p = lt_md->lt_stats;

    return SHR_E_NONE;
}

int
bcmltm_internal_state_lt_get(int unit,
                             uint32_t ltid,
                             bcmltm_lt_state_t **lt_state_p)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_ha_ptr_t lt_state_hap;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               ltid, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    lt_state_hap = lt_md->params->lt_state_hap;
    *lt_state_p = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);

    if (*lt_state_p == NULL) {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

int
bcmltm_internal_info_lt_get(int unit,
                            uint32_t table_id,
                            uint32_t field_id,
                            uint32_t field_idx,
                            uint64_t *data_value)
{
    int rv;
    bcmltm_lt_md_t *lt_md = NULL;

    rv = bcmltm_md_lt_retrieve(unit,
                               BCMLTM_TABLE_CATG_LOGICAL,
                               table_id, &lt_md);

    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (lt_md == NULL) {
        return SHR_E_INTERNAL;
    }

    switch (field_id) {
    case TABLE_INFOt_INDEX_MIN_LIMITf:
        *data_value = lt_md->params->index_min;
        break;
    case TABLE_INFOt_INDEX_MAX_LIMITf:
        *data_value = lt_md->params->index_max;
        break;
    case TABLE_INFOt_PARAM_MIN_LIMITf:
        *data_value = lt_md->params->mem_param_min;
        break;
    case TABLE_INFOt_PARAM_MAX_LIMITf:
        *data_value = lt_md->params->mem_param_max;
        break;
    default:
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}
