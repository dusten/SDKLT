/*! \file trm_trans.c
 *
 * This module implements the transaction handling backend
 * of the transaction manager (TRM)
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

#include <sal/sal_assert.h>
#include <sal/sal_types.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmltm/bcmltm.h>
#include <bcmtrm/trm_api.h>
#include <bcmtrm/generated/bcmtrm_ha.h>
#include "trm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMTRM_TRANSACTION

/*
 * This function being called by the PTM WAL thread upon completion
 * of the commit operation into H/W. This function being called for
 * asynchroneous transactions only. The transaction can be batch or
 * atomic.
 */
static void trm_async_trans_cb(uint32_t trans_id,
                               shr_error_t status,
                               void *user_data)
{
    bcmtrm_entry_t *entry = (bcmtrm_entry_t *)user_data;
    bcmtrm_hw_notif_struct_t hw_notif;
    bcmtrm_trans_t *trans;

    if (!entry) {
        return;
    }
    trans = entry->p_trans;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(trans->unit, "trans_id=%d status=%d\n"),
                 trans_id, status));

    if (trans->state == T_ACTIVE || trans->state == T_IDLE) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(trans->unit,
                             "callback for non active trans trans_id=%u\n"),
                  trans_id));
        return;
    }
    bcmtrm_trans_ha_state.done_f(entry->info.unit, trans_id, entry->ha_trn_hdl);

    hw_notif.type = TRANS_CB;
    hw_notif.ltm_entry = entry;
    hw_notif.status = status;
    /* Post the event to the notification thread */
    sal_msgq_post(bcmtrm_hw_notif_q, &hw_notif, SAL_MSGQ_NORMAL_PRIORITY,
                  SAL_MSGQ_FOREVER);
}

/*
 * This function called by the PTM WAL thread on completion of an atomic
 * transaction that was synchronous transaction.
 */
static void sync_atomic_trans_cb(uint32_t trans_id,
                                 shr_error_t status,
                                 void *user_data)
{
    bcmtrm_trans_t *trans = (bcmtrm_trans_t *)user_data;

    bcmtrm_trans_ha_state.done_f(trans->unit, trans_id,
                                 trans->l_entries->ha_trn_hdl);
    trans->info.status = status;
    if (status != SHR_E_NONE) {
        bcmtrm_entry_t *entry_it = trans->l_entries;
        /* Update the state of all entries */
        while (entry_it) {
            entry_it->info.status = status;
            entry_it = entry_it->next;
        }
    }

    /* Call the transaction callback function */
    bcmtrn_trans_cb_and_clean(trans, BCMLT_NOTIF_OPTION_HW, status);
}

/*
 * This function processes atomic transaction.
 */
static int handle_atomic(bcmtrm_trans_t *trans,
                         uint32_t *op_id,
                         uint32_t *trans_id,
                         void *user_data)
{
    int rv = SHR_E_PARAM;
    bcmtrm_entry_t *entry;
    bcmptm_trans_cb_f  cb;
    void *context;
    uint32_t ha_trn_hdl;
    sal_mutex_t mtx = bcmtrm_unit_mutex_get(trans->unit);

    /* Take exclusive access to the unit for modeled path */
    sal_mutex_take(mtx, SAL_MUTEX_FOREVER);

    bcmtrm_trans_ha_state.set_f(trans->unit,
                                BCMTRM_STATE_STAGING,
                                *trans_id,
                                &ha_trn_hdl);
    for (entry = trans->l_entries; entry; entry = entry->next) {
        entry->p_trans = trans;
        rv = bcmtrm_stage_entry(entry, *op_id, *trans_id);
        entry->info.status = rv;

        if (rv != SHR_E_NONE) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(entry->info.unit,
                             "Atomic transaction operation failed trans_id=%u"),
                      *trans_id));
            /* Cancel the transaction */
            bcmltm_abort(trans->unit, *trans_id);
            bcmtrm_trans_ha_state.cancel_f(trans->unit, *trans_id, ha_trn_hdl);

            trans->processed_entries = trans->info.num_entries;
            break;
        }
    }

    if (rv != SHR_E_NONE) {
        INCREMENT_OP_ID(*trans_id);
        /*
         * Done with this transaction. Open the device to other modeled
         * operations.
         */
        sal_mutex_give(mtx);

        trans->info.status = SHR_E_FAIL;
        trans->state = T_ACTIVE;
        bcmtrn_trans_cb_and_clean(trans,
                                  BCMLT_NOTIF_OPTION_COMMIT,
                                  rv);
        return SHR_E_NONE;
    }

    trans->state = T_COMMITTED;
    bcmtrm_set_trans_entries_state(trans, E_COMMITTED);
    if (!trans->syncronous) {
        /* The transaction is now committed, inform the app. */
        bcmtrm_appl_trans_inform(trans,
                              SHR_E_NONE,
                              BCMLT_NOTIF_OPTION_COMMIT);
        cb = trm_async_trans_cb;
        context = trans->l_entries;
    } else {
        cb = sync_atomic_trans_cb;
        context = trans;
    }

    trans->l_entries->ha_trn_hdl = ha_trn_hdl;
    bcmtrm_trans_ha_state.update_f(trans->unit,
                                   BCMTRM_STATE_COMMITTED,
                                   *trans_id,
                                   ha_trn_hdl);
    /*
     * Commit the transaction. Do not change anything regarding this
     * transaction past this call to commit as the callback may occur prior to
     * the return of the commit.
     * The only activity allowed post the call to commit is error handling from
     * the commit itself.
     * This is under the assumption that the WAL callback will not happen if
     * there was an error during commit (remember that WAL calback indicates
     * error or completion of the operation.
     */
    rv = bcmltm_commit(trans->unit, *trans_id, cb, context);

    if (rv != SHR_E_NONE) {
        bcmtrm_trans_ha_state.cancel_f(trans->unit, *trans_id, ha_trn_hdl);
    }

    INCREMENT_OP_ID(*trans_id);
    /* Allow other threads to access the unit's modeled path */
    sal_mutex_give(mtx);

    if (rv != SHR_E_NONE) {
        if (!trans->syncronous) {
            sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
            trans->state = T_ACTIVE;
            trans->info.status = SHR_E_FAIL;
            sal_mutex_give(trans->lock_obj->mutex);
        }
        bcmtrn_trans_cb_and_clean(trans,
                                  BCMLT_NOTIF_OPTION_HW,
                                  rv);
    }

    return rv;
}

/*
 * Important to remember that once the last entry was staged than the
 * application must be notified before the commit. The reason is that as a
 * result of commit, it is possible that the h/w commit callback will happen
 * before we inform the application about the transaction status of committed.
 * The result might be that the application will be notified about h/w
 * completion before commitment. Informing the application about transaction
 * committed after the last entry was staged solves this issue.
 */
static int handle_batch(bcmtrm_trans_t *trans,
                        uint32_t *op_id,
                        uint32_t *trans_id,
                        void *user_data)
{
    int rv;
    bcmtrm_entry_t *entry;
    uint32_t committed_count = 0;
    uint32_t ha_trn_hdl;
    sal_mutex_t mtx = bcmtrm_unit_mutex_get(trans->unit);

    for (entry = trans->l_entries;
         entry;
         entry = entry->next) {
        entry->p_trans = trans;
        /* Send interactive entries into the interactive queue */
        if ((entry->interactive) || (entry->pt)) {
            bcmtrm_q_interactive_entry(entry, trans->priority);
            continue;
        }

        /* Each entry should be execute exclusively (per unit) */
        sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
        bcmtrm_trans_ha_state.set_f(entry->info.unit,
                                    BCMTRM_STATE_STAGING,
                                    *trans_id,
                                    &ha_trn_hdl);

        rv = bcmtrm_stage_entry(entry,
                               *op_id,
                               *trans_id);
        entry->info.status = rv;
        if (rv != SHR_E_NONE) {
            bcmltm_abort(trans->unit, *trans_id);
            bcmtrm_trans_ha_state.cancel_f(trans->unit, *trans_id, ha_trn_hdl);

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(entry->info.unit,
                              "Batch transaction operation failed trans_id=%u"),
                      *trans_id));
            entry->state = E_ACTIVE;
            trans->info.status = SHR_E_PARTIAL;
            sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
            committed_count = ++trans->processed_entries;
            trans->committed_entries++;
            sal_mutex_give(trans->lock_obj->mutex);
        } else {   /* Staging was successful */
            entry->state = E_COMMITTED;
            sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
            trans->committed_entries++;
            trans->commit_success++;
            sal_mutex_give(trans->lock_obj->mutex);
            /* If this is the last entry check if need to inform the app */
            if (entry->next == NULL) {
                if (trans->commit_success == trans->committed_entries) {
                    trans->info.status = SHR_E_NONE;
                } else {
                    trans->info.status = SHR_E_PARTIAL;
                }

                if (!trans->syncronous) {
                    bcmtrm_appl_trans_inform(trans,
                                          trans->info.status,
                                          BCMLT_NOTIF_OPTION_COMMIT);
                }
            }

            if (trans->syncronous) {
                trans->usr_data = user_data;
            }

            entry->ha_trn_hdl = ha_trn_hdl;
            bcmtrm_trans_ha_state.update_f(trans->unit,
                                           BCMTRM_STATE_COMMITTED,
                                           *trans_id,
                                           ha_trn_hdl);
            if (trans->state == T_COMMITTING) {
                trans->state = T_COMMITTED;
            }
            rv = bcmltm_commit(trans->unit, *trans_id,
                               trm_async_trans_cb, entry);
            if (rv != 0) {
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(trans->unit,
                                "Batch transaction entry failed commit "\
                                "trans_id=%d error=%s"),
                          *trans_id, shr_errmsg(rv)));
                bcmtrm_trans_ha_state.cancel_f(trans->unit,
                                               *trans_id,
                                               ha_trn_hdl);
                entry->info.status = SHR_E_FAIL;
                entry->state = E_ACTIVE;
                sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
                committed_count = ++trans->processed_entries;
                sal_mutex_give(trans->lock_obj->mutex);
                trans->info.status = SHR_E_PARTIAL;
            }
        } /* End of else (handling successful staging) */

        INCREMENT_OP_ID(*trans_id);
        sal_mutex_give(mtx);   /* Allow other transactions */
    }  /* End of for loop */

    /*
     * If the last entry failed then the notification thread will not be able to
     * detect this. So we make the callback here.
     */
    if (committed_count == trans->info.num_entries) {
        if (trans->commit_success == 0) {
            trans->info.status = SHR_E_FAIL;
        }
        trans->state = T_ACTIVE;
        bcmtrn_trans_cb_and_clean(trans,
                                  BCMLT_NOTIF_OPTION_COMMIT,
                                  SHR_E_RESOURCE);
    }

    return SHR_E_NONE;
}

void bcmtrm_appl_trans_inform(bcmtrm_trans_t *trans,
                              shr_error_t status,
                              bcmlt_notif_option_t notif_opt)
{
    bcmlt_notif_option_t notif_actual;

    /*
     * No callback for transactions which are:
     * a. Have no callback or
     * b. Asked not to be notified or
     * c. Marked for deletion
     */
    if ((!trans->cb) ||
        (trans->info.notif_opt == BCMLT_NOTIF_OPTION_NO_NOTIF) ||
        trans->delete) {
        return;
    }

    if (status != SHR_E_NONE) {
        if ((trans->info.notif_opt == BCMLT_NOTIF_OPTION_ALL) ||
            (trans->info.notif_opt == notif_opt)) {
            notif_actual = notif_opt;
        } else {
            notif_actual = trans->info.notif_opt;
        }
    } else if ((trans->info.notif_opt != BCMLT_NOTIF_OPTION_ALL) &&
               (trans->info.notif_opt != notif_opt)) {
        return;
    } else {
        notif_actual = notif_opt;
    }
    trans->cb(notif_actual, &trans->info, trans->usr_data);
}

void bcmtrn_trans_cb_and_clean(bcmtrm_trans_t *trans,
                               bcmlt_notif_option_t notif_opt,
                               shr_error_t status)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(trans->unit, "notif_opt=%d\n"), notif_opt));
    /* Set the state of all entries to ACTIVE */
    bcmtrm_set_trans_entries_state(trans, E_ACTIVE);

    /* Inform the application */
    bcmtrm_appl_trans_inform(trans, status, notif_opt);
}

void bcmtrm_trans_complete(bcmtrm_trans_t *trans)
{
    bool del_trans;

    if (!trans->syncronous) {
        sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
        trans->state = T_ACTIVE;
        del_trans = trans->delete;
        sal_mutex_give(trans->lock_obj->mutex);

        if (del_trans) {
            bcmtrm_trans_done(trans, true);
        }
    }
}

int bcmtrm_handle_transaction(bcmtrm_trans_t *trans,
                              uint32_t *op_id,
                              uint32_t *trans_id,
                              void *user_data)
{
    int rv;

    trans->processed_entries = 0;
    switch (trans->info.type) {
        case BCMLT_TRANS_TYPE_ATOMIC:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(trans->unit,
                                    "Processing atomic transaction id=%d\n"),
                         *trans_id));
            rv = handle_atomic(trans, op_id, trans_id, user_data);
            break;
        case BCMLT_TRANS_TYPE_BATCH:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(trans->unit,
                                    "Processing batch transaction id=%d\n"),
                         *trans_id));
            rv = handle_batch(trans, op_id, trans_id, user_data);
            break;
        default:
            rv = SHR_E_INTERNAL;
    }

    return rv;
}

/*
 * Interactive transaction contains only interactive entries. In this case
 * the implementation simply goes through each entry of the transaction
 * until it completes. The entries results will be processed by the
 * bcmtrm_process_interactive_entry_req() functions and by the notification
 * thread which eventually will call the transaction callback.
 */
int bcmtrm_handle_interactive_transaction(bcmtrm_trans_t *trans,
                                          uint32_t *op_id,
                                          trn_interact_info_t *ha_mem)
{
    bcmtrm_entry_t *entry;

    trans->commit_success = 0;
    trans->committed_entries = 0;

    for (entry = trans->l_entries;
         entry;
         entry = entry->next) {
        entry->p_trans = trans;
        bcmtrm_process_interactive_entry_req(entry, *op_id, ha_mem);
        INCREMENT_OP_ID(*op_id);
    }

    return SHR_E_NONE;
}

static void recovery_cb(uint32_t trans_id,
                        shr_error_t status,
                        void *context)
{
    trn_state_t *trn_state = (trn_state_t *)context;

    assert(trn_state != NULL);
    assert(trans_id == trn_state->trn_id);
    trn_state->state = BCMTRM_STATE_NONE;
}

int bcmtrm_recover_trans (int unit, trn_state_t *trn_state)
{
    int rv;
    int wait_count = 0;

    if (trn_state->state == BCMTRM_STATE_NONE) {
        return SHR_E_NONE;
    } else if (trn_state->state == BCMTRM_STATE_STAGING) {
        rv = bcmltm_abort(unit, trn_state->trn_id);
    } else {    /* Commit again */
        rv = bcmltm_commit(unit, trn_state->trn_id, recovery_cb, trn_state);
        if (rv == SHR_E_NONE) {
            while (trn_state->state != BCMTRM_STATE_NONE &&
                   wait_count < 100000) {
                sal_usleep(100);
                wait_count++;
            }
            if (wait_count >= 100000) {
                rv = SHR_E_BUSY;
            }
        }
    }
    return rv;
}

