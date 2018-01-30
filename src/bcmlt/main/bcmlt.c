/*! \file bcmlt.c
 *
 *  Logical Table basic API to handle initialization shutdown
 *  device attach and detach.
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
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_sysm.h>
#include <shr/shr_debug.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>

#include "bcmlt_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMLT_TABLE

static bool bcmlt_initialized = false;

/* Keeps map of all the attached units, i.e. if the unit is attached or not. */
static bool unit_attached_map[BCMDRD_CONFIG_MAX_UNITS];

static sal_sem_t unit_attached_sem[BCMDRD_CONFIG_MAX_UNITS];

/*!
 *\brief Callback function to create entry.
 *
 * This function being called by the notification mechanism to create a new
 * entry that can be used for user notification. This function being registered
 * with the TRM so it can be called when required.
 *
 * \param [in] unit The unit of this entry.
 * \param [in] tbl_id The logical table ID.
 * \param [in] opcode The opcode associated with this entry.
 * \param [in] fields The list of fields for this entry.
 * \param [out] entry The newly allocated entry.
 *
 * \return SHR_E_NONE on succes.
 * \return SHR_E_PARAM when one of the input parameters is invalid.
 * \return SHR_E_MEMORY when failed to allocate entry resources.
 */
static int entry_assist(int unit,
                        uint32_t tbl_id,
                        bcmlt_opcode_t opcode,
                        shr_fmm_t *fields,
                        bcmtrm_entry_t **entry)
{
    const bcmlrd_table_rep_t *lt_info;
    bcmlt_entry_handle_t entry_hdl;
    shr_fmm_t *fld;

    SHR_FUNC_ENTER(unit);

    lt_info = bcmlrd_table_get(tbl_id);
    if (lt_info == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Invalid table ID %u\n"), tbl_id));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, lt_info->name, &entry_hdl));

    *entry = bcmlt_hdl_data_get(entry_hdl);
    (*entry)->opcode.lt_opcode = opcode;
    (*entry)->l_field = fields;
    fld = fields;
    while (fld) {
        (*entry)->fld_arr[fld->id] = fld;
        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

bool bcmlt_is_initialized(void)
{
    return bcmlt_initialized;
}

int bcmlt_init(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (bcmlt_initialized) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    bcmlt_initialized = true;
    SHR_IF_ERR_EXIT(bcmlt_db_init());

    SHR_IF_ERR_EXIT(bcmlt_hdl_init());

    bcmtrm_register_entry_assist(entry_assist);
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_close(void)
{
    int j;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!bcmlt_initialized) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    /* Verify that all units had been detached */
    for (j = 0; j < BCMDRD_CONFIG_MAX_UNITS; j++) {
        if (unit_attached_map[j]) {
            SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
        }
    }
    bcmlt_initialized = false;

    SHR_IF_ERR_EXIT(bcmlt_db_delete());

    bcmlt_hdl_shutdown();
exit:
    SHR_FUNC_EXIT();
}

static void unit_init_cb(shr_sysm_categories_t instance_cat,
                         int unit,
                         int status)
{
    if (instance_cat == SHR_SYSM_CAT_UNIT) {
        unit_attached_map[unit] = (status == SHR_E_NONE);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "System init returned %d\n"), status));
        sal_sem_give(unit_attached_sem[unit]);
    }
}

int bcmlt_device_attach(int unit, bool warm)
{
    bool valid_unit = true;

    SHR_FUNC_ENTER(unit);
    if (!bcmlt_is_initialized()) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS)) {
        valid_unit = false;
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    if (unit_attached_map[unit]) {
        valid_unit = false;    /* To avoid the cleanup */
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    /* Verify that the device exist */
    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_UNIT;
    }

    if (!unit_attached_sem[unit]) {
        unit_attached_sem[unit] = sal_sem_create("", SAL_SEM_BINARY, 0);
        SHR_NULL_CHECK(unit_attached_sem[unit], SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT(shr_sysm_instance_new(SHR_SYSM_CAT_UNIT,
                                          unit,
                                          warm,
                                          false,
                                          unit_init_cb));

    sal_sem_take(unit_attached_sem[unit], SAL_SEM_FOREVER);
    if (!unit_attached_map[unit]) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Failed unit attach\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Successfully attached unit\n")));
    SHR_IF_ERR_EXIT(bcmlt_tables_db_init(unit));

    /* Inform the TRM about the max number of fields in interactive tables */
    bcmtrm_interactive_table_init(unit,
                                  warm,
                                  bcmlt_db_interact_tbl_max_fields(unit));
exit:
    if (SHR_FUNC_ERR() && valid_unit && unit_attached_sem[unit]) {
        shr_sysm_instance_delete(SHR_SYSM_CAT_UNIT, unit);
        sal_sem_destroy(unit_attached_sem[unit]);
        unit_attached_sem[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

static void unit_stop_cb(shr_sysm_categories_t instance_cat,
                         int unit,
                         int status)
{
    if (instance_cat == SHR_SYSM_CAT_UNIT) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "System stop returned %d\n"), status));
        /* Unit stayed attached is the status is not OK */
        unit_attached_map[unit] = (status != SHR_E_NONE);
        sal_sem_give(unit_attached_sem[unit]);
    }
}

int bcmlt_device_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS) ||
        !unit_attached_map[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(shr_sysm_instance_stop(SHR_SYSM_CAT_UNIT,
                                           unit,
                                           false,
                                           unit_stop_cb));
    sal_sem_take(unit_attached_sem[unit], SAL_SEM_FOREVER);
    /* Return error if the unit is still attached */
    if (unit_attached_map[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    sal_sem_destroy(unit_attached_sem[unit]);
    SHR_IF_ERR_EXIT(bcmlt_tables_db_delete(unit));
    shr_sysm_instance_delete(SHR_SYSM_CAT_UNIT, unit);
    unit_attached_sem[unit] = NULL;
exit:
    SHR_FUNC_EXIT();
}

bool bcmlt_is_unit_attached(int unit)
{
    return ((unit >= 0) && (unit < BCMDRD_CONFIG_MAX_UNITS) &&
            unit_attached_map[unit]);
}

int bcmlt_api_clear(int unit)
{
    SHR_FUNC_ENTER(unit);
    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS) ||
        !unit_attached_map[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    bcmtrm_clean_uncommitted(unit);
exit:
    SHR_FUNC_EXIT();
}
