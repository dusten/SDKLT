/*! \file bcmevm_api.c
 *
 * BCM component generated notification API. This file contains the
 * implementation of the component generated notifications API as well
 * as the transaction manager registration for callback.
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
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_hash_str.h>
#include <bcmdrd_config.h>
#include <bcmevm/bcmevm_api.h>
#include "bcmevm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMEVM_EVENT

#define EVENT_TABLE_SIZE    32

typedef struct notify_comp_cb_s {
    bcmevm_event_cb *cb;
    struct notify_comp_cb_s *next;
} notify_comp_cb_t;

typedef struct notify_table_cb_s {
    bcmevm_cb *cb;
    uint32_t table_id;
    struct notify_table_cb_s *next;
} notify_table_cb_t;

static bcmevm_cb *cb_func;

static sal_mutex_t nfy_mutex;

static int nfy_active_units;

static shr_hash_str_hdl event_handler_tbl[BCMDRD_CONFIG_MAX_UNITS];

static notify_table_cb_t *tbl_event_list[BCMDRD_CONFIG_MAX_UNITS];

int bcmevm_unit_init(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (event_handler_tbl[unit]) {
        return SHR_E_RESOURCE;
    }

    if (!nfy_mutex) {
        nfy_mutex = sal_mutex_create("nfy");
    }
    SHR_NULL_CHECK(nfy_mutex, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT(
           shr_hash_str_dict_alloc(EVENT_TABLE_SIZE, &event_handler_tbl[unit]));

    nfy_active_units++;
exit:
    SHR_FUNC_EXIT();
}

int bcmevm_unit_shutdown(int unit)
{
    notify_comp_cb_t *it;
    void *val;
    int rv;
    char *key;

    SHR_FUNC_ENTER(unit);
    if (!event_handler_tbl[unit]) {
        return SHR_E_INIT;
    }

    /*
     * Free all the registered callback structures by traversing the dictionary
     */
    rv = shr_hash_str_get_first(event_handler_tbl[unit], &key, &val);
    while (rv == SHR_E_NONE) {
        /* Coverity
         * The value of 'val' is being refreshed by the function
         * shr_hash_str_get_next() below.
         */
        /* coverity[use_after_free : FALSE] */
        it = (notify_comp_cb_t *)val;
        while (it) {
            val = it;
            it = it->next;
            sal_free(val);
        }
        rv = shr_hash_str_get_next(event_handler_tbl[unit], &key, &val);
    }
    shr_hash_str_dict_free(event_handler_tbl[unit]);
    event_handler_tbl[unit] = NULL;

    if (--nfy_active_units == 0) {
        sal_mutex_destroy(nfy_mutex);
        nfy_mutex = NULL;
    }

    if (tbl_event_list[unit]) {
        notify_table_cb_t *table_it = tbl_event_list[unit];
        notify_table_cb_t *helper;
        while (table_it) {
            helper = table_it->next;
            sal_free(table_it);
            table_it = helper;
        }
        tbl_event_list[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

void bcmevm_register_cb(bcmevm_cb *cb)
{
    cb_func = cb;
}

int bcmevm_table_notify(int unit,
                        bool high_pri,
                        uint32_t table_id,
                        bcmlt_opcode_t opcode,
                        bcmltm_field_list_t *fields)
{
    notify_table_cb_t *it = tbl_event_list[unit];

    /* Start by calling the components callback */
    while (it) {
        if (it->table_id == table_id) {
            it->cb(unit, high_pri, table_id, opcode, fields);
        }
        it = it->next;
    }

    /* Now call upper layers so the APP can be informed. */
    if (!cb_func) {
        return SHR_E_NO_HANDLER;
    }
    return cb_func(unit, high_pri, table_id, opcode, fields);
}

int bcmevm_register_tbl_event(int unit,
                              uint32_t table_id,
                              bcmevm_cb *cb)
{
    notify_table_cb_t *it = tbl_event_list[unit];

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER));

    /* Check if function is already registered */
    while (it) {
        if (it->table_id == table_id && it->cb == cb) {
            SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
        }
        it = it->next;
    }

    /* Allocate new element */
    it = sal_alloc(sizeof(notify_table_cb_t), "bfyTbl");
    SHR_NULL_CHECK(it, SHR_E_MEMORY);
    it->cb = cb;
    it->table_id = table_id;
    it->next = tbl_event_list[unit];
    tbl_event_list[unit] = it;

exit:
    sal_mutex_give(nfy_mutex);
    SHR_FUNC_EXIT();
}

int bcmevm_register_published_event(int unit,
                                    const char *event,
                                    bcmevm_event_cb *cb)
{
    int rv;
    notify_comp_cb_t *comp_cb;
    notify_comp_cb_t *it = NULL;
    void *val;
    bool new_event;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER));
    if (!event_handler_tbl[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    rv = shr_hash_str_dict_lookup(event_handler_tbl[unit], event, &val);
    if (rv != SHR_E_NONE) {
        new_event = true;
    } else {
        comp_cb = (notify_comp_cb_t *)val;
        /*
         * It is possible that two events will have the same hash value.
         * Therefore need to check matching event string as well.
         */
        if (comp_cb->cb == cb) {
            SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
        }

        new_event = false;
        it = comp_cb;
        while (it->next && it->next->cb != cb) {
            it = it->next;
        }
        if (it->next) {
            SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
        }
    }
    comp_cb = sal_alloc(sizeof(notify_comp_cb_t), "evNotify");
    SHR_NULL_CHECK(comp_cb, SHR_E_MEMORY);

    /* Need to create the entry */
    comp_cb->cb = cb;
    comp_cb->next = NULL;
    if (new_event) {
        SHR_IF_ERR_EXIT(
               shr_hash_str_dict_insert(event_handler_tbl[unit], event, comp_cb));
    } else {
        it->next = comp_cb;
    }

exit:
    sal_mutex_give(nfy_mutex);
    SHR_FUNC_EXIT();
}

int bcmevm_unregister_published_event(int unit,
                                      const char *event,
                                      bcmevm_event_cb *cb)
{
    notify_comp_cb_t *comp_cb;
    notify_comp_cb_t *it;
    void *val;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER));

    if (!event_handler_tbl[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    SHR_IF_ERR_EXIT(
        shr_hash_str_dict_lookup(event_handler_tbl[unit], event, &val));

    comp_cb = (notify_comp_cb_t *)val;

    if (comp_cb->cb == cb) {
        /* If the first element matches then need to delete the hash entry */
        shr_hash_str_dict_delete(event_handler_tbl[unit], event);
        /*
         * If there are more callbacks for this hash slot insert the next
         * element into the hash dtring
         */
        if (comp_cb->next) {
            SHR_IF_ERR_EXIT(shr_hash_str_dict_insert(event_handler_tbl[unit],
                                                     event,
                                                     comp_cb->next));
        }
    } else {
        it = comp_cb;
        while (it->next && it->next->cb != cb) {
            it = it->next;
        }
        if (it->next) {
            comp_cb = it->next;
            it->next = comp_cb->next;
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
        }
    }
    sal_free(comp_cb);
exit:
    sal_mutex_give(nfy_mutex);
    SHR_FUNC_EXIT();
}

void bcmevm_publish_event_notify(int unit,
                                 const char *event,
                                 uint64_t ev_data)
{
    notify_comp_cb_t *it;
    void *val;
    int rv;

    if (!event_handler_tbl[unit]) {
        return;
    }
    sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER);
    do {
        rv = shr_hash_str_dict_lookup(event_handler_tbl[unit], event, &val);
        if (rv != SHR_E_NONE) { /* Nobody registered for this event. */
            break;
        }
        it = (notify_comp_cb_t *)val;
        while (it) {
            it->cb(unit, event, ev_data);
            it = it->next;
        }
    } while (0);
    sal_mutex_give(nfy_mutex);
}

