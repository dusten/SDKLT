/*! \file trm_notify.c
 *
 * This module implements the notification functionality of the
 * transaction manager. The notification being made to the application
 * in case where a transaction or entry operation changes state or in
 * the case that a table had changed and required notification.
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

#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <sal/sal_msgq.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_assert.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>
#include <shr/shr_error.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltm/bcmltm.h>
#include <bcmtrm/trm_api.h>
#include "trm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMTRM_ENTRY

typedef struct {
    bcmlt_table_sub_cb cb;
    void *user_data;
} table_event_info;

static table_event_info *trm_unit_tbl_event[BCMDRD_CONFIG_MAX_UNITS];

bool bcmltm_notify_units[BCMDRD_CONFIG_MAX_UNITS];
static size_t max_tbl_id[BCMDRD_CONFIG_MAX_UNITS];

void bcmtrm_tbl_chg_event(bcmtrm_entry_t *p_entry)
{
    bcmlt_opcode_t opcode = p_entry->opcode.lt_opcode;
    if (p_entry->pt) {    /* no change events for PT tables */
        return;
    }
    if (((opcode == BCMLT_OPCODE_INSERT) ||
         (opcode == BCMLT_OPCODE_UPDATE) ||
         (opcode == BCMLT_OPCODE_DELETE)) &&
        (max_tbl_id[p_entry->info.unit] > (size_t)p_entry->table_id) &&
        trm_unit_tbl_event[p_entry->info.unit][p_entry->table_id].cb) {
        table_event_info *p_table_event =
            &trm_unit_tbl_event[p_entry->info.unit][p_entry->table_id];
        bcmlt_table_notif_info_t table_info;
        table_info.unit = p_entry->info.unit;
        table_info.entry_hdl = p_entry->info.entry_hdl;
        table_info.table_name = p_entry->info.table_name;
        p_table_event->cb(&table_info, p_table_event->user_data);
    }
}



void bcmtrm_notify_thread(void *arg)
{
#define ONE_SEC_WAIT    1000000
    bool running = true;
    bcmtrm_hw_notif_struct_t hw_notif;
    int rv;
    int *notify_thread_running = (int *)arg;

    (*notify_thread_running)++;
    while (running) {
        rv = sal_msgq_recv(bcmtrm_hw_notif_q, (void *)&hw_notif, ONE_SEC_WAIT);
        if (rv == SAL_MSGQ_E_TIMEOUT) {
            continue;
        }
        if (rv != 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("Failed to receive hw notify message rv=%d\n"),
                      rv));
            break;
        }
        switch (hw_notif.type) {
        case NOTIF_EXIT:
            running = false;
            break;
        case ENTRY_CB:
            {
                bcmtrm_entry_t *entry = hw_notif.ltm_entry;
                bool entry_complete;
                /* Don't process invalid units */
                if (!bcmltm_notify_units[entry->info.unit]) {
                    break;
                }
                if (hw_notif.status == SHR_E_NONE) {
                    bcmtrm_tbl_chg_event(entry);
                }
                /*
                 * Set the status. The function
                 * bcmtrm_proc_entry_results() may override the status.
                 */
                entry->info.status = hw_notif.status;
                bcmtrm_proc_entry_results(entry, entry->ltm_entry);
                if (entry->asynch) {
                    entry_complete = true;
                } else {
                    entry_complete = false;
                    entry->state = E_ACTIVE;   /* The entry is done */
                }
                /* Inform the application if needed */
                bcmtrm_appl_entry_inform(entry,
                                         hw_notif.status,
                                         BCMLT_NOTIF_OPTION_HW);
                if (entry_complete) {
                    bcmtrm_entry_complete(entry);
                }
                break;
            }
        case TRANS_CB:
            {
                bcmtrm_entry_t *entry = hw_notif.ltm_entry;
                bcmtrm_trans_t *trans = entry->p_trans;
                bool trans_complete = false;
                /* Don't process invalid units */
                if (!bcmltm_notify_units[entry->info.unit]) {
                    break;
                }
                if (trans->info.type == BCMLT_TRANS_TYPE_ATOMIC) {
                    /*
                     * This is the only notification. Update the status of each
                     * entry with this status - maybe not necessary
                     */
                    bcmtrm_entry_t *entry_it;
                    for (entry_it = trans->l_entries;
                          entry_it;
                          entry_it = entry_it->next) {
                        /*
                         * Set the status. The function
                         * bcmtrm_proc_entry_results() may override the status.
                         */
                        entry_it->info.status = hw_notif.status;
                        if (entry_it->ltm_entry) {
                            bcmtrm_proc_entry_results(entry_it,
                                                      entry_it->ltm_entry);
                        }
                        if (hw_notif.status == SHR_E_NONE) {
                            bcmtrm_tbl_chg_event(entry_it);
                        }
                    }
                    trans->info.status = hw_notif.status;
                    if (trans->syncronous) {
                        trans->state = T_ACTIVE;
                        trans_complete = false;
                    } else {
                        trans_complete = true;
                    }
                    bcmtrn_trans_cb_and_clean(trans,
                                              BCMLT_NOTIF_OPTION_HW,
                                              hw_notif.status);
                } else {
                    uint32_t processed_entries;
                    /*
                     * Set the status. The function
                     * bcmtrm_proc_entry_results() may override the status.
                     */
                    entry->info.status = hw_notif.status;
                    if (entry->ltm_entry) {
                        bcmtrm_proc_entry_results(entry,
                                                  entry->ltm_entry);
                    }
                    if (hw_notif.status == SHR_E_NONE) {
                        bcmtrm_tbl_chg_event(entry);
                    }
                    /* Lock transaction db */
                    sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
                    processed_entries = ++trans->processed_entries;
                    sal_mutex_give(trans->lock_obj->mutex);
                    if (processed_entries == trans->info.num_entries) {
                        if (trans->syncronous) {
                            /*
                             * For synchronous we can set it all here. We know
                             * that nothing can change the status or
                             * delete flag values. We have to be sure that the
                             * callback is the last thing since after that
                             * the entry can be deleted under our feet (since
                             * the state changes to T_ACTIVE (required for
                             * synchronous operations before calling the
                             * callback)).
                             */
                            trans->state = T_ACTIVE;
                            trans_complete = false;
                        } else {
                            /*
                             * In asynchronous mode we simply set the
                             * transaction complete, so it can be cleaned up
                             * at the end. Here the state is not necessarily
                             * has to be T_ACTIVE to free the transaction.
                            */
                            trans_complete = true;
                        }
                        if (trans->commit_success == processed_entries) {
                            trans->info.status = SHR_E_NONE;
                        } else if (trans->commit_success == 0) {
                            trans->info.status = SHR_E_FAIL;
                        } else {
                            trans->info.status = SHR_E_PARTIAL;
                        }
                        bcmtrn_trans_cb_and_clean(trans,
                                                  BCMLT_NOTIF_OPTION_HW,
                                                  hw_notif.status);
                    }
                }
                if (trans_complete) {
                    bcmtrm_trans_complete(trans);
                }

                break;
            }
        case ENTRY_NOTIF:
            {
                bcmtrm_entry_t *entry = hw_notif.ltm_entry;  /* Entry can't be NULL */
                const bcmlrd_table_rep_t *lrd_tbl = bcmlrd_table_get(entry->table_id);
                if (lrd_tbl) {
                    entry->info.table_name = lrd_tbl->name;
                    bcmtrm_tbl_chg_event(entry);
                }
                bcmtrm_entry_done(entry, true);
                break;
            }
        default:
            assert(0);
        }
    }
    (*notify_thread_running)--;
}

int bcmtrm_table_event_subscribe(int unit,
                                 uint32_t table_id,
                                 bcmlt_table_sub_cb event_func,
                                 void *user_data)
{
    table_event_info *p_event = &trm_unit_tbl_event[unit][table_id];

    p_event->cb = event_func;
    p_event->user_data = user_data;
    return SHR_E_NONE;
}

int bcmtrm_table_event_alloc(int unit)
{
    size_t num_of_tables;
    int rv = SHR_E_NONE;

    num_of_tables = BCMLRD_TABLE_COUNT;
    do {
        if (num_of_tables > 0) {
            trm_unit_tbl_event[unit] =
                sal_alloc(num_of_tables * sizeof(table_event_info),
                "TRM event info");
            if (!trm_unit_tbl_event[unit]) {
                rv = SHR_E_MEMORY;
                break;
            }
            sal_memset(trm_unit_tbl_event[unit],
                       0,
                       num_of_tables * sizeof(table_event_info));
            max_tbl_id[unit] = num_of_tables;
        }
    } while (0);
    return rv;
}

void bcmtrm_table_event_free(int unit)
{
    if (trm_unit_tbl_event[unit] != NULL) {
        sal_free(trm_unit_tbl_event[unit]);
        trm_unit_tbl_event[unit] = NULL;
        max_tbl_id[unit] = 0;
    }
}

static bcmtrm_entry_assist_f e_assist;
void bcmtrm_register_entry_assist(bcmtrm_entry_assist_f cb)
{
    e_assist = cb;
}

int bcmtrm_sb_table_notify(int unit,
                           bool high_pri,
                           uint32_t table_id,
                           bcmlt_opcode_t opcode,
                           bcmltm_field_list_t *fields)
{
    bcmtrm_entry_t *entry;
    bcmtrm_hw_notif_struct_t hw_notif;

    if (!fields) {
        return SHR_E_PARAM;
    }
    if (!e_assist) {
        return SHR_E_INIT;
    }

    if (!trm_unit_tbl_event[unit] || !trm_unit_tbl_event[unit][table_id].cb) {
        /* No callback */
        bcmltm_field_list_t *tmp_f;
        bcmltm_field_list_t *working_fld = fields;

        while (working_fld) {
            tmp_f = working_fld;
            working_fld = working_fld->next;
            shr_fmm_free(tmp_f);
        }
        return SHR_E_NONE;
    }
    if (e_assist(unit, table_id, opcode, fields, &entry) != SHR_E_NONE) {
        return SHR_E_PARAM;
    }

    entry->info.status = SHR_E_NONE;

    hw_notif.type = ENTRY_NOTIF;
    hw_notif.ltm_entry = entry;
    hw_notif.status = SHR_E_NONE;
    /* post the event to the notification thread */
    sal_msgq_post(
        bcmtrm_hw_notif_q,
        &hw_notif,
        (high_pri ? SAL_MSGQ_HIGH_PRIORITY : SAL_MSGQ_NORMAL_PRIORITY),
        SAL_MSGQ_FOREVER);

    return SHR_E_NONE;
}

