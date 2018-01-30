/*! \file bcmlt_ltable_replay.c
 *
 * This file implements the logical table replay feature.
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
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <shr/shr_error.h>
#include <shr/shr_fmm.h>
#include <bcmdrd_config.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define ENTRY_TYPE    1
#define TRANS_TYPE    2

static bcmlt_capture_cb_t replay_fh[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Allocates new field and adds it to the entry.
 *
 * This function saves an entry using an application callback into the IO
 * provided via the parameter \c fd. The entry saved as the basic entry
 * structure followed by saving every field of the entry.
 *
 * \param [in] fd is the IO handle to be used by the application callback.
 * \param [in] entry is the entry to save.
 * \param [in] app_cb is the application supplied callback structure that
 * contain read and write functions that actually perform the saving of
 * the data into non-volatile memory that can be replay later.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int record_entry(void *fd,
                        bcmtrm_entry_t *entry,
                        bcmlt_capture_cb_t *app_cb)
{
    shr_fmm_t *field;
    int rv;
    uint8_t tbl_name_len = sal_strlen(entry->info.table_name) + 1;

    if ((app_cb->write(fd, &tbl_name_len, 1) != 1)  ||
        (app_cb->write(fd, (void *)entry->info.table_name, tbl_name_len) !=
             tbl_name_len)) {
        return SHR_E_INTERNAL;
    }
    if (app_cb->write(fd, entry, sizeof(*entry)) != sizeof(*entry)) {
        return SHR_E_INTERNAL;
    }

    /* save all the fields */
    for (field = entry->l_field; field; field = field->next) {
        rv = app_cb->write(fd, field, sizeof(*field));
        if (rv != sizeof(*field)) {
            return SHR_E_INTERNAL;
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Delays the execution to replay at the same timing as original.
 *
 * This function sleeps the duration of the difference between the two input
 * parameters. However, the function limited the sleep time to 1 second. At
 * the end the function sets the input parameter \c current_ts to the time
 * slot representing the operation recording time.
 *
 * \param [in] ts is the timeslot indicating the time that the operation
 * was recorded.
 * \param [in,out] current_ts is the time slot of the previous operation
 * (or 0 if this is the first operation).
 *
 * \return None.
 */
static void wait_till_ready(sal_usecs_t ts,
                            sal_usecs_t *current_ts)
{
    sal_usecs_t diff;

    if (*current_ts == 0) {
        *current_ts = ts;
        return;
    }
    diff = ts - *current_ts;
    if (diff > 1000000) {   /* sleep maximum 1 second */
        diff = 1000000;
    }
    sal_usleep(diff);
    *current_ts = ts;
}

/*!
 *\brief Create new entry from the replay log.
 *
 * This function reads the replay log and construct a new entry. The entry is
 * fully built including the entry fields.
 *
 * \param [in] app_cb is the application provided IO structure to perform
 * read and write operations from/to the replay log.
 *
 * \return On success a pointer to the newly created entry is created. NULL
 * on failure.
 */
static bcmtrm_entry_t* create_entry(bcmlt_replay_cb_t *app_cb)
{
    bcmtrm_entry_t recovered_entry;
    bcmtrm_entry_t *entry;
    shr_fmm_t *field;
    bool end_field_list;
    void *fd = app_cb->fd;
    uint8_t tbl_name_len;
    char *tbl_name;
    bcmlt_table_attrib_t *table_attr;
    void *hdl;
    int rv;

    if (app_cb->read(fd, &tbl_name_len, 1) != 1) {
        return NULL;
    }
    tbl_name = sal_alloc(tbl_name_len,  "replay");
    if (!tbl_name) {
        return NULL;
    }
    if (app_cb->read(fd, tbl_name, tbl_name_len) != tbl_name_len) {
        return NULL;
    }

    if (app_cb->read(fd, &recovered_entry, sizeof(*entry)) != sizeof(*entry)) {
        return NULL;
    }

    /* Table ID may change so retrieve it again */
    rv = bcmlt_db_table_info_get(recovered_entry.info.unit,
                                 tbl_name,
                                 &table_attr,
                                 NULL,
                                 &hdl);
    if (rv != SHR_E_NONE) {
        return NULL;
    }

    entry = bcmtrm_entry_alloc(recovered_entry.info.unit,
                               table_attr->table_id,
                               table_attr->interactive,
                               table_attr->pt,
                               NULL,
                               table_attr->name);
    if (!entry) {
        return NULL;
    }
    entry->db_hdl = hdl;
    entry->asynch = recovered_entry.asynch;
    entry->attrib = recovered_entry.attrib;
    entry->info.unit = recovered_entry.info.unit;
    entry->info.notif_opt = recovered_entry.info.notif_opt;
    entry->opcode = recovered_entry.opcode;
    entry->priority = recovered_entry.priority;
    entry->max_fid = 0;
    entry->fld_arr = NULL;

    rv = bcmlt_hdl_alloc(entry, &entry->info.entry_hdl);
    if (rv != SHR_E_NONE) {
        return NULL;
    }

    /* Now start recovering the fields */
    end_field_list = (recovered_entry.l_field == NULL);
    while (!end_field_list) {
        field = shr_fmm_alloc();
        if (!field) {
            return NULL;
        }
        if (app_cb->read(fd, field, sizeof(*field)) != sizeof(*field)) {
            return NULL;
        }
        end_field_list = field->next == NULL;
        field->next = entry->l_field;
        entry->l_field = field;
    }

    sal_free(tbl_name);
    return entry;
}

/*!
 *\brief Replay an entry operation.
 *
 * This function replay one entry operation. It first ceates the entry
 * by reading its content from the IO and then posting it to the transaction
 * manager for processing.
 *
 * \param [in] app_cb is the application provided IO structure to perform
 * read and write operations from/to the replay log.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int replay_entry(bcmlt_replay_cb_t *app_cb,
                        replay_action_f *cb,
                        bool view_only)
{
    bcmtrm_entry_t *entry;
    int rv = SHR_E_NONE;
    bcmlt_object_hdl_t obj;

    entry = create_entry(app_cb);
    if (!entry) {
        return SHR_E_INTERNAL;
    }
    entry->asynch = false;
    if (!view_only) {
        rv = bcmtrm_entry_req(entry);
    }
    if (cb) {
        obj.entry = true;
        obj.hdl.entry = entry->info.entry_hdl;
        cb(entry->info.unit, &obj);
    }
    bcmtrm_entry_free(entry);
    return rv;
}

/*!
 *\brief Adds an entry to a transaction.
 *
 * This function creates a new entry and adds it to a given transaction.
 *
 * \param [in] trans is the transaction to add the entry to.
 * \param [in] app_cb is the application provided IO structure to perform
 * read and write operations from/to the replay log.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int add_entry(bcmtrm_trans_t *trans, bcmlt_replay_cb_t *app_cb)
{
    bcmtrm_entry_t *entry;

    entry = create_entry(app_cb);
    if (!entry) {
        return SHR_E_MEMORY;
    }
    /* Entry should be kept in the same order so add to the end */
    entry->next = NULL;
    if (!trans->l_entries) {
        trans->l_entries = entry;
    } else {
        trans->last_entry->next = entry;
    }
    trans->last_entry = entry;
    entry->p_trans = trans;
    return SHR_E_NONE;
}

/*!
 *\brief Replay transaction.
 *
 * This function creates a new transaction and all its associated
 * entries. It than posts the transaction to the transaction manager.
 *
 * \param [in] app_cb is the application provided IO structure to perform
 * read and write operations from/to the replay log.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int replay_trans(bcmlt_replay_cb_t *app_cb,
                        replay_action_f *cb,
                        bool view_only)
{
    bcmtrm_trans_t *trans;
    bcmtrm_trans_t recovered_trans;
    uint32_t j;
    int rv;
    void *fd = app_cb->fd;
    bcmlt_object_hdl_t obj;

    if (app_cb->read(fd, &recovered_trans, sizeof(*trans)) != sizeof(*trans)) {
        return SHR_E_INTERNAL;
    }
    trans = bcmtrm_trans_alloc(recovered_trans.info.type);
    if (!trans) {
        return SHR_E_MEMORY;
    }
    rv = bcmlt_hdl_alloc(trans, &trans->info.trans_hdl);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    trans->info.num_entries = recovered_trans.info.num_entries;
    trans->syncronous = true;
    trans->pt_trans = recovered_trans.pt_trans;
    trans->priority = recovered_trans.priority;
    trans->l_entries = NULL;
    for (j = 0; j < trans->info.num_entries; j++) {
        if ((rv = add_entry(trans, app_cb)) != SHR_E_NONE) {
            return rv;
        }
    }
    if (!view_only) {
        rv = bcmtrm_trans_req(trans);
    }
    if (cb) {
        obj.entry = false;
        obj.hdl.trans = trans->info.trans_hdl;
        cb(trans->unit, &obj);
    }
    bcmtrm_trans_free(trans);
    return rv;
}


/*******************************************************************************
 * Public functions
 */
int bcmlt_capture_start(int unit, bcmlt_capture_cb_t *app_cb)
{
    if ((unit < 0) || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return SHR_E_UNIT;
    }
    if (replay_fh[unit].write) {
        return SHR_E_BUSY;
    }
    replay_fh[unit] = *app_cb;
    return SHR_E_NONE;
}

int bcmlt_capture_stop(int unit)
{
    if ((unit < 0) || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return SHR_E_UNIT;
    }
    replay_fh[unit].write = 0;
    return SHR_E_NONE;
}

int bcmlt_replay_entry_record(bcmtrm_entry_t *entry)
{
    void *fd;
    sal_usecs_t *ts;
    sal_usecs_t cur_time;
    uint8_t buffer[sizeof(sal_usecs_t) + 1];
    bcmlt_capture_cb_t *app_cb = &replay_fh[entry->info.unit];

    if (!app_cb->write) {
        return SHR_E_INIT;
    }
    fd = app_cb->fd;
    ts = (sal_usecs_t *)buffer;
    cur_time = sal_time_usecs();
    *ts = cur_time;
    buffer[sizeof(sal_usecs_t)] = ENTRY_TYPE;
    if (app_cb->write(fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
        return SHR_E_INTERNAL;
    }
    return record_entry(fd, entry, app_cb);
}

int bcmlt_replay_trans_record(bcmtrm_trans_t *trans)
{
    void *fd;
    sal_usecs_t *ts;
    sal_usecs_t cur_time;
    uint8_t buffer[sizeof(sal_usecs_t) + 1];
    bcmtrm_entry_t *entry;
    int rv;
    bcmlt_capture_cb_t *app_cb = &replay_fh[trans->unit];

    if (!app_cb->write) {
        return SHR_E_INIT;
    }

    fd = app_cb->fd;
    ts = (sal_usecs_t *)buffer;
    cur_time = sal_time_usecs();
    *ts = cur_time;
    buffer[sizeof(sal_usecs_t)] = TRANS_TYPE;
    rv = app_cb->write(fd, buffer, sizeof(buffer));
    if (rv != sizeof(buffer)) {
        return SHR_E_INTERNAL;
    }
    rv = app_cb->write(fd, trans, sizeof(*trans));
    if (rv != sizeof(*trans)) {
        return SHR_E_INTERNAL;
    }
    for (entry = trans->l_entries; entry; entry = entry->next) {
        rv = record_entry(fd, entry, app_cb);
        if (rv != SHR_E_NONE) {
            return rv;
        }
    }
    return SHR_E_NONE;
}

int bcmlt_capture_replay(bool timing,
                         replay_action_f *cb,
                         bool view_only,
                         bcmlt_replay_cb_t *app_cb)
{
    int rv;
    void *fd;
    sal_usecs_t *ts;
    sal_usecs_t current_ts = 0;
    uint8_t buffer[sizeof(sal_usecs_t) + 1];

    if (!app_cb || !app_cb->read || !app_cb->fd) {
        return SHR_E_PARAM;
    }
    fd = app_cb->fd;
    ts = (sal_usecs_t *)buffer;
    while (app_cb->read(fd, buffer, sizeof(buffer)) == sizeof(buffer)) {
        if (timing) {
            wait_till_ready(*ts, &current_ts);
        }
        if (buffer[sizeof(sal_usecs_t)] == TRANS_TYPE) {
            rv = replay_trans(app_cb, cb, view_only);
        } else if (buffer[sizeof(sal_usecs_t)] == ENTRY_TYPE) {
            rv = replay_entry(app_cb, cb, view_only);
        } else {
            rv = SHR_E_INTERNAL;
        }
        if (rv != SHR_E_NONE) {
            return rv;
        }
    }
    return SHR_E_NONE;
}
