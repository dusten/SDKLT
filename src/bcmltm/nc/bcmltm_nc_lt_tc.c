/*! \file bcmltm_nc_lt_tc.c
 *
 * Logical Table Manager - Node Cookies.
 *
 * Logical Table mappings with Table Commit groups.
 *
 * This file contains routines to construct LT information
 * for LT with Table Commit groups.  The information is used by
 * the LTM metadata.
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

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include "bcmltm_nc_lt_map.h"
#include "bcmltm_nc_lt_tc.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA



/*******************************************************************************
 * Private functions
 */


/*!
 * \brief Destroy the Table Commit list metadata.
 *
 * Destroy the Table Commit list metadata.
 *
 * \param [in] tc_list Table Commit list to destroy.
 */
static void
table_commit_list_destroy(bcmltm_table_commit_list_t *tc_list)
{
    if (tc_list == NULL) {
        return;
    }

    SHR_FREE(tc_list->handlers);
    SHR_FREE(tc_list);

    return;
}

/*!
 * \brief Create the Table Commit list metadata for given table.
 *
 * Create the Table Commit list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] map Table Commit mapping information.
 * \param [out] tc_list Returning pointer for Table Commit list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_commit_list_create(int unit,
                         const bcmltm_map_entries_tc_t *map,
                         bcmltm_table_commit_list_t **tc_list_ptr)
{
    bcmltm_table_commit_list_t *tc_list = NULL;
    const bcmltd_table_commit_handler_t **handlers = NULL;
    uint32_t num_handlers;
    unsigned int size;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *tc_list_ptr = NULL;

    num_handlers = map->num_handlers;
    if (num_handlers == 0) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(tc_list, sizeof(*tc_list),
              "LTM Logical Table table_commit_list_t");
    SHR_NULL_CHECK(tc_list, SHR_E_MEMORY);
    sal_memset(tc_list, 0, sizeof(*tc_list));

    size = sizeof(*handlers) * num_handlers;
    SHR_ALLOC(handlers, size,
              "LTM Logical Table bcmltd_table_commit_handler_t list");
    SHR_NULL_CHECK(handlers, SHR_E_MEMORY);
    sal_memset(handlers, 0, size);

    for (i = 0; i < num_handlers; i++) {
        handlers[i] =
            bcmltm_lta_intf_table_commit_handler_get(unit, map->handler_id[i]);
    }

    tc_list->handlers = handlers;
    tc_list->num_handlers = num_handlers;

    *tc_list_ptr = tc_list;

 exit:
    if (SHR_FUNC_ERR()) {
        if ((tc_list == NULL) || (tc_list->handlers == NULL)) {
            SHR_FREE(handlers);
        }
        table_commit_list_destroy(tc_list);
        *tc_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_nc_lt_tc_info_create(int unit, bcmlrd_sid_t sid,
                            const bcmltm_map_tc_t *map_group,
                            bcmltm_nc_lt_tc_info_t **info_ptr)
{
    bcmltm_nc_lt_tc_info_t *info = NULL;
    bcmltm_map_entries_tc_t entries;

    SHR_FUNC_ENTER(unit);

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_map_entries_tc_parse(unit, sid, &entries));

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "LTM Logical Table bcmltm_nc_lt_tc_info_t");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Create LT info for Table Commit handlers */
    SHR_IF_ERR_EXIT
        (table_commit_list_create(unit, &entries, &info->tc_list));
    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lt_tc_info_destroy(info);
        *info_ptr = NULL;
    }

    bcmltm_nc_lt_map_entries_tc_cleanup(&entries);

    SHR_FUNC_EXIT();
}

void
bcmltm_nc_lt_tc_info_destroy(bcmltm_nc_lt_tc_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Table Commit info */
    table_commit_list_destroy(info->tc_list);

    SHR_FREE(info);

    return;
}

