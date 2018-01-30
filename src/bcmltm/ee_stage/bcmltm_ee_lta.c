/*! \file bcmltm_ee_lta.c
 *
 * Logical Table Manager Execution Engine
 *
 * Logical Table Adaptors
 *
 * This file contains the LTM EE stage functions for LTA custom table
 * handlers.  The Logical Table Adaptor interface for custom tables
 * allows the implementation of a LT to be controlled solely through
 * custom logic.  This is intended for cases where the standard LTM
 * processing is too generic for the special case of this LT.
 *
 * The LTA interface provides a standardized method of passing the
 * LT fields and other necessary data to the custom logic.  The custom
 * table implementation is responsible for analyzing the fields,
 * producing the necessary PTM operations, and extracting any
 * return fields.
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
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
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


/*******************************************************************************
 * Public functions
 */

int
bcmltm_ee_node_lta_insert(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->validate(unit,
                                   BCMLT_OPCODE_INSERT,
                                   in_fields,
                                   lta_handler->arg));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lta_handler->insert(unit,
                             lt_entry->trans_id,
                             in_fields,
                             lta_handler->arg));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_alloc_insert(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_table_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->validate(unit,
                                   BCMLT_OPCODE_INSERT,
                                   in_fields,
                                   lta_handler->arg));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lta_handler->insert_alloc(unit,
                                   lt_entry->trans_id,
                                   in_fields,
                                   out_fields,
                                   lta_handler->arg));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_lookup(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_table_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->validate(unit,
                                   BCMLT_OPCODE_LOOKUP,
                                   in_fields,
                                   lta_handler->arg));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lta_handler->lookup(unit,
                             lt_entry->trans_id,
                             in_fields,
                             out_fields,
                             lta_handler->arg));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_delete(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->validate(unit,
                                   BCMLT_OPCODE_DELETE,
                                   in_fields,
                                   lta_handler->arg));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lta_handler->delete(unit,
                             lt_entry->trans_id,
                             in_fields,
                             lta_handler->arg));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lta_update(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    if (lta_handler->validate != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->validate(unit,
                                   BCMLT_OPCODE_UPDATE,
                                   in_fields,
                                   lta_handler->arg));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lta_handler->update(unit,
                             lt_entry->trans_id,
                             in_fields,
                             lta_handler->arg));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}


int
bcmltm_ee_node_lta_traverse(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_lta_table_params_t *lta_table_params;
    const bcmltd_table_handler_t *lta_handler;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_table_params = BCMLTM_LTA_TABLE_PARAMS(node_cookie);
    lta_handler = lta_table_params->handler;
    ptr_math = ltm_buffer + lta_table_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_table_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;

    if ((lt_entry->flags & BCMLTM_ENTRY_FLAG_TRAVERSE_START) == 0) {
        /* Continuing traverse */
        if (lta_handler->validate != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (lta_handler->validate(unit,
                                       BCMLT_OPCODE_TRAVERSE,
                                       in_fields,
                                       lta_handler->arg));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->traverse->next(unit,
                                         lt_entry->trans_id,
                                         in_fields,
                                         out_fields,
                                         lta_handler->arg));
    } else {
        /* Start traverse */
        SHR_IF_ERR_VERBOSE_EXIT
            (lta_handler->traverse->first(unit,
                                          lt_entry->trans_id,
                                          out_fields,
                                          lta_handler->arg));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}
