/*! \file bcmpc_topo.c
 *
 * BCMPC Topology APIs for PM Instance
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_db_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_TOPO

/*!
 * \brief Port macro type entry structure.
 *
 * The deployed PM types are stored within a linked list.
 * This data structure represents the element of the list.
 *
 * The key of the entry, i.e. \c dbe->key, is a PM type id.
 */
typedef struct bcmpc_topo_entry_type_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! Type info. */
    bcmpc_topo_type_t info;

} bcmpc_topo_entry_type_t;

/*!
 * \brief Port macro instance entry structure.
 *
 * The deployed PM instances are stored within a linked list.
 * This data structure represents the element of the list.
 *
 * The key of the entry, i.e. \c dbe->key, is a PM instance id.
 */
typedef struct bcmpc_topo_entry_inst_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! Instance info. */
    bcmpc_topo_t info;

} bcmpc_topo_entry_inst_t;

/* PM type database */
static bcmpc_db_entry_t *topo_type_db[BCMPC_NUM_UNITS_MAX];

/* PM instance database */
static bcmpc_db_entry_t *topo_inst_db[BCMPC_NUM_UNITS_MAX];


/*******************************************************************************
 * Internal public functions
 */

void
bcmpc_topo_type_t_init(bcmpc_topo_type_t *tinfo)
{
    sal_memset(tinfo, 0, sizeof(bcmpc_topo_type_t));
    tinfo->num_aggr = 1;
}

int
bcmpc_topo_type_create(int unit, int tid)
{
    bcmpc_topo_entry_type_t *new = NULL;
    bcmpc_db_entry_t *dbe;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    /* Check if tid is duplicated */
    dbe = bcmpc_db_entry_search(topo_type_db[unit], 1, &key, NULL);
    if (dbe) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    /* Allocate a new entry */
    SHR_ALLOC(new, sizeof(bcmpc_topo_entry_type_t), "bcmpcTopoType");
    SHR_NULL_CHECK(new, SHR_E_MEMORY);

    /* Init the the value of the entry */
    sal_memset(new, 0, sizeof(bcmpc_topo_entry_type_t));

    /* Type ID is the key of the table */
    new->dbe.keys[0] = tid;

    /* Add the new entry to DB table */
    bcmpc_db_entry_add(&topo_type_db[unit], &new->dbe);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_destroy(int unit, int tid)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_type_t *entry;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_del(&topo_type_db[unit], 1, &key);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
    SHR_FREE(entry);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_set(int unit, int tid, bcmpc_topo_type_t *tinfo)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_type_t *entry;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_type_db[unit], 1, &key, NULL);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
    sal_memcpy(&(entry->info), tinfo, sizeof(bcmpc_topo_type_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_get(int unit, int tid, bcmpc_topo_type_t *tinfo)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_type_t *entry;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    bcmpc_topo_type_t_init(tinfo);

    dbe = bcmpc_db_entry_search(topo_type_db[unit], 1, &key, NULL);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
    sal_memcpy(tinfo, &(entry->info), sizeof(bcmpc_topo_type_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_traverse(int unit, bcmpc_topo_type_traverse_cb_f cb_func,
                         void *cookie)
{
    int rv = SHR_E_NONE;
    bcmpc_db_entry_t *dbe, *next;
    bcmpc_topo_entry_type_t *entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    dbe = bcmpc_db_entry_next(topo_type_db[unit], NULL);
    while (dbe) {
        next = bcmpc_db_entry_next(topo_type_db[unit], dbe);

        entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
        rv = (*cb_func)(unit, entry->dbe.keys[0], &entry->info, cookie);
        if (SHR_FAILURE(rv)) {
            SHR_RETURN_VAL_EXIT(rv);
        }

        dbe = next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_id_get(int unit, bcmpc_pport_t pport, int *pm_id)
{
    bcmpc_db_entry_t *dbe, *next;
    bcmpc_topo_entry_inst_t *entry;
    int phys_pm_port;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_next(topo_inst_db[unit], NULL);

    while (dbe) {
        bcmpc_topo_t *pm_info;
        bcmpc_topo_type_t tinfo;

        next = bcmpc_db_entry_next(topo_inst_db[unit], dbe);

        entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
        pm_info = &entry->info;

        SHR_IF_ERR_EXIT
            (bcmpc_topo_type_get(unit, pm_info->tid, &tinfo));

        if ((pport >= pm_info->base_pport) &&
            (pport < pm_info->base_pport + tinfo.num_ports) ) {
                /* Check if the physical port is invalid. */
                phys_pm_port = pport - pm_info->base_pport;
                if (SHR_BITGET(pm_info->invalid_pbmp, phys_pm_port)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
                }

                *pm_id = dbe->keys[0];
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }

        dbe = next;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_topo_t_init(bcmpc_topo_t *pm_info)
{
    sal_memset(pm_info, 0, sizeof(bcmpc_topo_t));
}

int
bcmpc_topo_create(int unit, int pm_id)
{
    bcmpc_topo_entry_inst_t *new = NULL;
    bcmpc_db_entry_t *dbe;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    /* Check if pm_id is duplicated */
    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    if (dbe) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    /* Allocate a new entry */
    SHR_ALLOC(new, sizeof(bcmpc_topo_entry_inst_t), "bcmpcTopoInst");
    SHR_NULL_CHECK(new, SHR_E_MEMORY);

    /* Init the the value of the entry */
    sal_memset(new, 0, sizeof(bcmpc_topo_entry_inst_t));

    /* Instance ID is the key of the table */
    new->dbe.keys[0] = pm_id;

    /* Add the new entry to DB table */
    bcmpc_db_entry_add(&topo_inst_db[unit], &new->dbe);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_destroy(int unit, int pm_id)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_inst_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_del(&topo_inst_db[unit], 1, &key);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
    SHR_FREE(entry);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_set(int unit, int pm_id, bcmpc_topo_t *pm_info)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_inst_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
    sal_memcpy(&(entry->info), pm_info, sizeof(bcmpc_topo_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_get(int unit, int pm_id, bcmpc_topo_t *pm_info)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_inst_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    SHR_NULL_VERBOSE_CHECK(dbe, SHR_E_NOT_FOUND);

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
    sal_memcpy(pm_info, &(entry->info), sizeof(bcmpc_topo_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_traverse(int unit, bcmpc_topo_traverse_cb_f cb_func,
                    void *cookie)
{
    int rv = SHR_E_NONE;
    bcmpc_db_entry_t *dbe, *next;
    bcmpc_topo_entry_inst_t *entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    dbe = bcmpc_db_entry_next(topo_inst_db[unit], NULL);
    while (dbe) {
        next = bcmpc_db_entry_next(topo_inst_db[unit], dbe);

        entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
        rv = (*cb_func)(unit, entry->dbe.keys[0], &entry->info, cookie);
        if (SHR_FAILURE(rv)) {
            SHR_RETURN_VAL_EXIT(rv);
        }

        dbe = next;
    }

exit:
    SHR_FUNC_EXIT();
}
