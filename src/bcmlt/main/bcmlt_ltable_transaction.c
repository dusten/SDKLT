/*! \file bcmlt_ltable_transaction.c
 *
 *  Handles logical tables transactions.
 *  This file handles all the functionality of the API that is transaction
 *  based.
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

#include <sal/sal_alloc.h>
#include <bcmlt/bcmlt.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_TRANSACTION

/* Sanity check macro */
#define TRANS_VALIDATE(trans) \
    if (!trans || (trans->state != T_ACTIVE)) {                             \
        if (!trans) {                                                       \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Invalid transaction handle\n")));        \
        } else {                                                            \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Transaction was committed\n")));         \
        }                                                                   \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                                   \
    }

int bcmlt_transaction_allocate(bcmlt_trans_type_t type,
                               bcmlt_transaction_hdl_t *trans_hdl)
{
    bcmtrm_trans_t *trans;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!bcmlt_is_initialized()) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    if (!trans_hdl || (type < BCMLT_TRANS_TYPE_ATOMIC) ||
        (type >= BCMLT_TRANS_TYPE_NUM)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    trans = bcmtrm_trans_alloc(type);
    SHR_NULL_CHECK(trans, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT(bcmlt_hdl_alloc(trans, trans_hdl));
    trans->info.trans_hdl = *trans_hdl;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_free(bcmlt_transaction_hdl_t trans_hdl)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    if (!trans || (trans->state == T_IDLE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmtrm_trans_free(trans));

    SHR_IF_ERR_EXIT(bcmlt_hdl_free(trans_hdl));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Entry should be linked according to the order they were inserted.
 * This will enable the function bcmlt_transaction_entry_num_get() to make
 * sense.
*/
int bcmlt_transaction_entry_add(bcmlt_transaction_hdl_t trans_hdl,
                                bcmlt_opcode_t opcode,
                                bcmlt_entry_handle_t entry_hdl)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    ENTRY_VALIDATE(entry);
    LT_OPCODE_VALIDATE(opcode, entry->info.unit);
    if (entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions for PT entry\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry already belong to a transaction\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Don't allow interactive entry in atomic transaction */
    if ((trans->info.type == BCMLT_TRANS_TYPE_ATOMIC) &&
        (entry->interactive)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Can't use atomic transaction for "\
                                "interactive tables\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (trans->l_entries) {  /* Not the first entry */
        if (trans->pt_trans) { /* Don't allow mixed transactions */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                    "Can't mix LT and PT entries in the "\
                                    "same transaction\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        /* For now do not allow transaction which spans multiple units */
        if (trans->unit != entry->info.unit) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                      (BSL_META_U(entry->info.unit,
                                   "No support for multi-unit transaction\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }
    } else {   /* First entry */
        trans->pt_trans = false;
        trans->unit = entry->info.unit;
    }

    /* Clear get from hw attribute if opcode is not lookup */
    if (opcode == BCMLT_OPCODE_LOOKUP) {
        /* Clean out all fields other than key fields. */
        SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        /* Clean out all fields other than key fields. */
        if (entry->l_field) {
            SHR_IF_ERR_EXIT(bcmlt_entry_clean_data_fields(entry));
            if (!entry->l_field) {   /* If no key fields don't search again */
                LOG_VERBOSE(BSL_LOG_MODULE,
                          (BSL_META_U(entry->info.unit,
                                      "Only the first traverse operation can" \
                                      "have no key fields\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
    } else {
        entry->attrib &= ~TRM_E_ATTR_HW_GET;
    }

    entry->opcode.lt_opcode = opcode;
    /* Add entry to the end of the list */
    entry->next = NULL;
    if (!trans->l_entries) {
        trans->l_entries = entry;
    } else {
        trans->last_entry->next = entry;
    }
    trans->last_entry = entry;
    entry->p_trans = trans;
    trans->info.num_entries++;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_pt_entry_add(bcmlt_transaction_hdl_t trans_hdl,
                                   bcmlt_pt_opcode_t opcode,
                                   bcmlt_entry_handle_t entry_hdl)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    ENTRY_VALIDATE(entry);
    PT_OPCODE_VALIDATE(opcode, entry->info.unit);
    if (!entry->pt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Use the PT functions only for PT entry\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (entry->p_trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Entry already belongs to a transaction\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (trans->info.type == BCMLT_TRANS_TYPE_ATOMIC) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                              "Can't use atomic transaction for PT tables\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (trans->l_entries) {
        if (!trans->pt_trans) { /* Don't allow mixed transactions */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                    "Can't mix LT and PT entries in the "\
                                    "same transaction\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        /* For now do not allow transaction which spans multiple units */
        if (trans->unit != entry->info.unit) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                   "No support for multi-unit transaction\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        trans->pt_trans = true;
        trans->unit = entry->info.unit;
    }
    entry->opcode.pt_opcode = opcode;

    /* Add entry to the end of the list */
    entry->next = NULL;
    if (!trans->l_entries) {
        trans->l_entries = entry;
    } else {
        trans->last_entry->next = entry;
    }
    trans->last_entry = entry;
    entry->p_trans = trans;
    trans->info.num_entries++;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_entry_delete(bcmlt_transaction_hdl_t  trans_hdl,
                                   bcmlt_entry_handle_t  entry_hdl)
{
    bcmtrm_entry_t *entry;
    bcmtrm_entry_t *entry_prev = NULL;
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    /* Can only delete from non-committed transactions */
    TRANS_VALIDATE(trans);
    if ((trans->info.num_entries == 0) || !entry_hdl) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    for (entry = trans->l_entries; entry; entry = entry->next) {
        if (entry->info.entry_hdl == entry_hdl) {
            if (entry_prev) {
                entry_prev->next = entry->next;
            } else {
                trans->l_entries = entry->next;
            }
            if (entry == trans->last_entry) {
                trans->last_entry = entry_prev;
            }
            break;
        }
        entry_prev = entry;
    }
    SHR_NULL_CHECK(entry, SHR_E_PARAM); /* Didn't find the entry */

    trans->info.num_entries--;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_info_get(bcmlt_transaction_hdl_t trans_hdl,
                               bcmlt_transaction_info_t  *trans_info)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    if (!trans || (trans->state == T_IDLE) || !trans_info) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *trans_info = trans->info;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_commit_async(bcmlt_transaction_hdl_t trans_hdl,
        bcmlt_notif_option_t notif_opt,
        void *user_data,
        bcmlt_trans_cb notif_fn,
        bcmlt_priority_level_t priority)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    PRIORITY_VALIDATE(priority, trans->unit);
    NOTIFICATION_VALIDATE(notif_opt, notif_fn, trans->unit);
    if (trans->info.num_entries == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    trans->cb = notif_fn;
    trans->info.notif_opt = notif_opt;
    trans->usr_data = user_data;
    trans->priority = priority;

    bcmlt_replay_trans_record(trans); /* Record the trans op */
    SHR_IF_ERR_EXIT(bcmtrm_asynch_trans_req(trans));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_commit(bcmlt_transaction_hdl_t trans_hdl,
                             bcmlt_priority_level_t priority)
{
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    TRANS_VALIDATE(trans);
    PRIORITY_VALIDATE(priority, trans->unit);

    if (trans->info.num_entries == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    trans->priority = BCMLT_PRIORITY_NORMAL;
    trans->info.notif_opt = BCMLT_NOTIF_OPTION_NO_NOTIF;

    bcmlt_replay_trans_record(trans); /* Record the trans op */
    SHR_IF_ERR_EXIT(bcmtrm_trans_req(trans));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_transaction_entry_num_get(bcmlt_transaction_hdl_t trans_hdl,
                                    uint32_t entry_num,
                                    bcmlt_entry_info_t  *entry_info)
{
    bcmtrm_entry_t *entry;
    uint32_t j;
    bcmtrm_trans_t *trans = bcmlt_hdl_data_get(trans_hdl);

    SHR_FUNC_ENTER(trans ? trans->unit : BSL_UNIT_UNKNOWN);
    if (!trans || !entry_info ||
        (trans->state == T_IDLE) ||
        (entry_num >= trans->info.num_entries)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    for (j = 0, entry = trans->l_entries;
          j < entry_num;
          j++, entry = entry->next);

    *entry_info = entry->info;
exit:
    SHR_FUNC_EXIT();
}

