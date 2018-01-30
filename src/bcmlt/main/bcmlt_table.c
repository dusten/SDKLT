/*! \file bcmlt_table.c
 *
 * Handle table functionality
 *
 * This file contains the implementation of all the table level functionality.
 * In particular functions like subscribe to table events and table entries
 * enumeration.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlt/bcmlt.h>
#include "bcmlt_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMLT_TABLE

int bcmlt_table_names_get_first (int unit,
                                 uint32_t flags,
                                 char **name)
{
    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_RETURN_VAL_EXIT(bcmlt_db_table_name_get_first(unit, flags, name));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_names_get_next (int unit,
                                uint32_t flags,
                                char **name)
{
    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_RETURN_VAL_EXIT(bcmlt_db_table_name_get_next(unit, flags, name));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_field_defs_get(int unit,
                               const char *table_name,
                               uint32_t    fields_array_size,
                               bcmlt_field_def_t *field_defs_array,
                               uint32_t *require_array_size)
{
    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!table_name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlt_db_table_field_defs_get(unit,
                                       table_name,
                                       fields_array_size,
                                       field_defs_array,
                                       require_array_size));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_subscribe(int unit,
                          const char *table_name,
                          bcmlt_table_sub_cb callback,
                          void *user_data)
{
    bcmlt_table_attrib_t *tbl_attr;
    void *tbl_hdl;

    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!table_name || !callback) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    /* Obtain the table ID */
    SHR_IF_ERR_EXIT(bcmlt_db_table_info_get(unit,
                                            table_name,
                                            &tbl_attr,
                                            NULL,
                                            &tbl_hdl));

    if (tbl_attr->pt) { /* Not events for PT tables */
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmtrm_table_event_subscribe(unit,
                                        tbl_attr->table_id,
                                        callback,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_unsubscribe(int unit, const char *table_name)
{
    bcmlt_table_attrib_t *tbl_attr;
    void *tbl_hdl;

    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!table_name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    /* Obtain the table ID */
    SHR_IF_ERR_EXIT(bcmlt_db_table_info_get(unit,
                                            table_name,
                                            &tbl_attr,
                                            NULL,
                                            &tbl_hdl));

    if (tbl_attr->pt) { /* Not events for PT tables */
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Register NULL callback */
    SHR_IF_ERR_EXIT(bcmtrm_table_event_subscribe(unit,
                                        tbl_attr->table_id,
                                        NULL,
                                        0));
exit:
    SHR_FUNC_EXIT();
}

int
bcmlt_table_pt_name_get(int unit, const char *tbl_name,
                        uint32_t *pt_count_ptr, const char *pt_names[])
{
    bcmlt_table_attrib_t *table_attr;
    void *hdl;
    shr_lmm_hdl_t fld_array_hdl;
    int idx, group_count, pt_mapped = 0;
    const bcmlrd_map_t *map = NULL;
    const char **pt_name_list = pt_names;
    uint32_t pt_count = 0;

    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!tbl_name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Find the table ID associated with the name */
    SHR_IF_ERR_EXIT
        (bcmlt_db_table_info_get(unit, tbl_name, &table_attr,
                                 &fld_array_hdl, &hdl));

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, table_attr->table_id, &map));

    group_count = map->groups;
    for (idx = 0; idx < group_count; idx++) {
        if (map->group[idx].dest.kind == BCMLRD_MAP_PHYSICAL) {
            pt_mapped = 1;
            break;
        }
    }
    if (!pt_mapped) {
        /* Table does not have mapped fields. */
        *pt_count_ptr = pt_count;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < group_count; idx++) {
        if (map->group[idx].dest.kind == BCMLRD_MAP_CUSTOM) {
            break;
        }
        if (map->group[idx].dest.kind == BCMLRD_MAP_PHYSICAL) {
            /* If the allocated memory is not sufficient,
             * return the pt count for re-allocation. */
            if (pt_count >= *pt_count_ptr) {
                pt_count++;
                continue;
            }
            pt_name_list[pt_count] =
                (char *)bcmdrd_pt_sid_to_name(unit, map->group[idx].dest.id);
            pt_count++;
        }
    }
    if ( pt_count > *pt_count_ptr) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    }
    *pt_count_ptr = pt_count;

exit:
    if (SHR_FUNC_ERR()) {
        *pt_count_ptr = pt_count;
    }
    SHR_FUNC_EXIT();
}

int bcmlt_table_desc_get(int unit,
                         const char *tbl_name,
                         const char **tbl_desc)
{
    int tbl_id;
    const bcmltd_table_rep_t *lt_info;

    SHR_FUNC_ENTER(unit);

    /* Sanity checks. */
    UNIT_VALIDATION(unit);
    if (!tbl_name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    tbl_id = bcmltd_table_name_to_idx(tbl_name);

    /* Return if table is not found. */
    if (tbl_id == SHR_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Table %s was not found\n"),
                     tbl_name));
        SHR_RETURN_VAL_EXIT(tbl_id);
    }

    lt_info = bcmltd_table_get((bcmltd_sid_t)tbl_id);
    if (lt_info == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    *tbl_desc = lt_info->desc;

exit:
    SHR_FUNC_EXIT();
}
