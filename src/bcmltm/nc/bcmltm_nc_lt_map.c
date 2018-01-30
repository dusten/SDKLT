/*! \file bcmltm_nc_lt_map.c
 *
 * Logical Table Manager - Logical Table Mapping Information.
 *
 * This file contains routines that parse the LRD mapping information.
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

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmltm/bcmltm_lta_intf_internal.h>

#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/chip/bcmlrd_limits.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>
#include "bcmltm_nc_lt_map.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Number of PT ops required for a PT view */
#define LOGICAL_NUM_PT_OPS         1

/* Maximum number of field indexes in a field */
#define MAX_FIELD_IDXS             10


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Cleanup PT groups mapping information.
 *
 * Free allocated memory for PT groups mapping information.
 *
 * \param [in] pt_map Pointer to PT groups mapping.
 */
static void
map_pt_cleanup(bcmltm_map_pt_t *pt_map)
{
    SHR_FREE(pt_map);

    return;
}


/*!
 * \brief Get the count of PT groups for given logical table.
 *
 * Get the count of PT groups for given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] count Number of PT groups.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_pt_count_get(int unit, bcmlrd_sid_t sid, uint32_t *count)
{
    uint32_t num_pt = 0;
    bcmlrd_map_t const *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    *count = 0;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Find out number of PT maps */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Check for physical table destination */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        num_pt++;
    }

    *count = num_pt;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the PT groups mapping for given logical table.
 *
 * Create the table PT mapping for given logical table.
 *
 * Create the PT groups mapping for given logical table from
 * parsing the LRD map group information of kind BCMLRD_MAP_PHYSICAL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [in] attr Table attributes.
 * \param [out] pt_map_ptr Returning pointer to table PT groups mapping.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_pt_parse(int unit,
             bcmlrd_sid_t sid,
             const bcmltm_table_attr_t *attr,
             bcmltm_map_pt_t **pt_map_ptr)
{
    unsigned int size;
    bcmltm_map_pt_t *pt_map = NULL;
    bcmlrd_map_t const *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    bcmdrd_sid_t ptid;
    uint32_t num_ops = LOGICAL_NUM_PT_OPS;
    uint32_t num_pt = 0;
    uint32_t group_idx;
    uint32_t pt_idx;
    uint32_t flags;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_table_type_t lt_type;

    SHR_FUNC_ENTER(unit);

    *pt_map_ptr = NULL;

    /* Get PT group count */
    SHR_IF_ERR_EXIT(map_pt_count_get(unit, sid, &num_pt));
    if (num_pt == 0) {
        SHR_EXIT();
    }

    size = sizeof(*pt_map) + (sizeof(bcmltm_map_pt_info_t) * num_pt);
    SHR_ALLOC(pt_map, size, "LTM Logical Table PT map");
    SHR_NULL_CHECK(pt_map, SHR_E_MEMORY);
    sal_memset(pt_map, 0, size);

    /* Get LT type */
    lt_type = attr->type;

    /* Get Working Buffer handle */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    for (group_idx = 0, pt_idx = 0; group_idx < map->groups; group_idx++) {

        group = &map->group[group_idx];

        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* Sanity check, this should not occur unless above logic is wrong */
        if (pt_idx >= num_pt) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM metadata PT mapping: "
                                  "invalid pt index for sid=%d "
                                  "pt_idx=%d num_pt=%d\n"),
                       sid, (int) pt_idx, (int) num_pt));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        ptid = group->dest.id;
        flags = bcmltm_wb_ptm_op_flags_get(unit, ptid);

        /* Set PT Entry Changed flag if applicable */
        if (bcmltm_wb_ptm_pt_changed_flag_valid(lt_type)) {
            flags |= BCMLTM_PT_OP_FLAGS_PT_CHANGED;
        }

        /*
         * PTM working buffer block
         * - Non-Keyed : One block per PT.
         * - Keyed     : One block per LT regardless of number of PTs.
         *
         * Keyed LTs only have 1 PTM working buffer block because
         * there is only 1 PTM operation.
         */
        if ((pt_idx == 0) || !BCMLTM_TABLE_TYPE_LT_KEYED(lt_type)) {
            /* Add Working Buffer block */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_block_ptm_lt_add(unit, ptid, lt_type, num_ops,
                                            wb_handle, &wb_block_id, NULL));
        }

        pt_map->info[pt_idx].sid = ptid;
        pt_map->info[pt_idx].flags = flags;
        pt_map->info[pt_idx].num_ops = num_ops;
        pt_map->info[pt_idx].wb_block_id = wb_block_id;
        pt_map->num_pt++;
        pt_idx++;
    }

    *pt_map_ptr = pt_map;

 exit:
    if (SHR_FUNC_ERR()) {
        map_pt_cleanup(pt_map);
        *pt_map_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup Custom Table Handler groups mapping information.
 *
 * Free allocated memory for Custom Table Handler groups mapping information.
 *
 * \param [in] map_groups Pointer to Custom Table Handler groups mapping.
 */
static void
map_cth_cleanup(bcmltm_map_cth_t *map_groups)
{
    SHR_FREE(map_groups);

    return;
}


/*!
 * \brief Get the count of Custom Table Handler groups for given logical table.
 *
 * Get the count of Custom Table Handler for given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] count Number of Custom Table Handler groups.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_cth_count_get(int unit, bcmlrd_sid_t sid, uint32_t *count)
{
    uint32_t num = 0;
    bcmlrd_map_t const *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    *count = 0;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Find out number of Custom Table Handler group maps */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Check for CTH map group kind */
        if (group->dest.kind != BCMLRD_MAP_CUSTOM) {
            continue;
        }

        num++;
    }

    /* Only 1 Custom Table Handler is supported */
    if (num > 1) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "LT id=%d has %d Custom Table Handlers defined, "
                             "only 1 Custom Table Handler is supported "
                             "per LT.\n"),
                  sid, num));
        /* Reset to max allowed */
        num = 1;
    }

    *count = num;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the Custom Table Handler groups mapping for given LT.
 *
 * Create the Custom Table Handler groups mapping for given logical table from
 * parsing the LRD map group information of kind BCMLRD_MAP_CUSTOM.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] map_group_ptr Returning pointer to table handler groups map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_cth_parse(int unit, bcmlrd_sid_t sid,
              bcmltm_map_cth_t **map_groups_ptr)
{
    unsigned int size;
    bcmltm_map_cth_t *map_groups = NULL;
    uint32_t num = 0;

    SHR_FUNC_ENTER(unit);

    *map_groups_ptr = NULL;

    /* Get Custom Table Handler group count */
    SHR_IF_ERR_EXIT(map_cth_count_get(unit, sid, &num));
    if (num == 0) {
        SHR_EXIT();
    }

    size = sizeof(*map_groups);
    SHR_ALLOC(map_groups, size, "LTM Logical Table Custom Table Handler map");
    SHR_NULL_CHECK(map_groups, SHR_E_MEMORY);
    sal_memset(map_groups, 0, size);

    map_groups->num = num;

    *map_groups_ptr = map_groups;

 exit:
    if (SHR_FUNC_ERR()) {
        map_cth_cleanup(map_groups);
        *map_groups_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup LTM groups mapping information.
 *
 * Free allocated memory for LTM groups mapping information.
 *
 * \param [in] map_groups Pointer to LTM groups mapping.
 */
static void
map_ltm_cleanup(bcmltm_map_ltm_t *map_groups)
{
    SHR_FREE(map_groups);

    return;
}


/*!
 * \brief Get the count of LTM map groups for given logical table.
 *
 * Get the count of LTM map groups for given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] count Number of LTM map groups.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_ltm_count_get(int unit, bcmlrd_sid_t sid, uint32_t *count)
{
    uint32_t num = 0;
    bcmlrd_map_t const *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    *count = 0;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Find out number of LTM map groups */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Check for LTM map group kind */
        if (group->dest.kind != BCMLRD_MAP_LTM) {
            continue;
        }

        num++;
    }

    /* Only 1 LTM group is supported */
    if (num > 1) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "LT id=%d has %d LTM map group defined, "
                             "only 1 LTM group is supported per LT.\n"),
                  sid, num));
        /* Reset to max allowed */
        num = 1;
    }

    *count = num;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the LTM map group for given LT.
 *
 * Create the LTM map groups for given logical table from
 * parsing the LRD map group information of kind BCMLRD_MAP_LTM.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] map_group_ptr Returning pointer to LTM map groups.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_ltm_parse(int unit, bcmlrd_sid_t sid,
              bcmltm_map_ltm_t **map_groups_ptr)
{
    unsigned int size;
    bcmltm_map_ltm_t *map_groups = NULL;
    uint32_t num = 0;

    SHR_FUNC_ENTER(unit);

    *map_groups_ptr = NULL;

    /* Get LTM group count */
    SHR_IF_ERR_EXIT(map_ltm_count_get(unit, sid, &num));
    if (num == 0) {
        SHR_EXIT();
    }

    size = sizeof(*map_groups);
    SHR_ALLOC(map_groups, size, "LTM Logical LTM map groups");
    SHR_NULL_CHECK(map_groups, SHR_E_MEMORY);
    sal_memset(map_groups, 0, size);

    map_groups->num = num;

    *map_groups_ptr = map_groups;

 exit:
    if (SHR_FUNC_ERR()) {
        map_ltm_cleanup(map_groups);
        *map_groups_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup FV groups mapping information.
 *
 * Free allocated memory for FV groups mapping information.
 *
 * \param [in] fv_map Pointer to FV groups mapping.
 */
static void
map_fv_cleanup(bcmltm_map_fv_t *fv_map)
{
    SHR_FREE(fv_map);

    return;
}


/*!
 * \brief Get the count of FV groups for given logical table.
 *
 * Get the count of FV groups for given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] count Number of FV groups.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_fv_count_get(int unit, bcmlrd_sid_t sid, uint32_t *count)
{
    uint32_t num_fv = 0;
    bcmlrd_map_t const *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    *count = 0;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Find out number of FV maps */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Check for Field Validation */
        if (group->dest.kind != BCMLRD_MAP_VALIDATION) {
            continue;
        }

        num_fv++;
    }

    *count = num_fv;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FV groups mapping for given logical table.
 *
 * Create the table FV mapping for given logical table.
 *
 * Create the FV groups mapping for given logical table from
 * parsing the LRD map group information of kind BCMLRD_MAP_VALIDATION.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] fv_map_fvr Returning pointer to table FV groups mapping.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_fv_parse(int unit,
             bcmlrd_sid_t sid,
             bcmltm_map_fv_t **map_groups_ptr)
{
    unsigned int size;
    bcmltm_map_fv_t *map_groups = NULL;
    uint32_t num = 0;

    SHR_FUNC_ENTER(unit);

    *map_groups_ptr = NULL;

    /* Get Field Validation group count */
    SHR_IF_ERR_EXIT(map_fv_count_get(unit, sid, &num));
    if (num == 0) {
        SHR_EXIT();
    }

    size = sizeof(*map_groups);
    SHR_ALLOC(map_groups, size, "LTM Logical Table Field Validation map");
    SHR_NULL_CHECK(map_groups, SHR_E_MEMORY);
    sal_memset(map_groups, 0, size);

    map_groups->num = num;

    *map_groups_ptr = map_groups;

 exit:
    if (SHR_FUNC_ERR()) {
        map_fv_cleanup(map_groups);
        *map_groups_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup Table Commit groups mapping information.
 *
 * Free allocated memory for Table Commit groups mapping information.
 *
 * \param [in] tc_map Pointer to Table Commit groups mapping.
 */
static void
map_tc_cleanup(bcmltm_map_tc_t *tc_map)
{
    SHR_FREE(tc_map);

    return;
}


/*!
 * \brief Get the count of Table Commit groups for given logical table.
 *
 * Get the count of Table Commit groups for given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] count Number of Table Commit groups.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_tc_count_get(int unit, bcmlrd_sid_t sid, uint32_t *count)
{
    uint32_t num = 0;
    bcmlrd_map_t const *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    *count = 0;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Find out number of Table Commit maps */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Check for Table Commit */
        if (group->dest.kind != BCMLRD_MAP_TABLE_COMMIT) {
            continue;
        }

        num++;
    }

    *count = num;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the Table Commit groups mapping for given logical table.
 *
 * Create the Table Commit groups mapping for given logical table from
 * parsing the LRD map group information of kind BCMLRD_MAP_TABLE_COMMIT.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [out] map_groups_ptr Returning pointer to Table Commit groups mapping.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_tc_parse(int unit,
             bcmlrd_sid_t sid,
             bcmltm_map_tc_t **map_groups_ptr)
{
    unsigned int size;
    bcmltm_map_tc_t *map_groups = NULL;
    uint32_t num = 0;

    SHR_FUNC_ENTER(unit);

    *map_groups_ptr = NULL;

    /* Get Table Commit group count */
    SHR_IF_ERR_EXIT(map_tc_count_get(unit, sid, &num));
    if (num == 0) {
        SHR_EXIT();
    }

    size = sizeof(*map_groups);
    SHR_ALLOC(map_groups, size, "LTM Logical Table Commit map");
    SHR_NULL_CHECK(map_groups, SHR_E_MEMORY);
    sal_memset(map_groups, 0, size);

    map_groups->num = num;

    *map_groups_ptr = map_groups;

 exit:
    if (SHR_FUNC_ERR()) {
        map_tc_cleanup(map_groups);
        *map_groups_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a field map object.
 *
 * Allocate a field map object from the given field map list.
 * This function does not fill field map information.
 *
 * If the field is already present in the list, memory is allocated for
 * the field map and appended to the link list of that existing field.
 * If the field is new, the element in the field map array is used.
 *
 * As a result, the new field map is allocated and placed such that
 * maps with the same field ID and indexes are across the link list of
 * the array element.  This organization is used for creating the
 * corresponding node cookie.
 *
 * Note that other than maps with matching (fid, idx) being placed
 * across the link list of an array element, the elements in the list array
 * are not ordered.
 *
 * \param [in] unit Unit number.
 * \param [in] lfid Logical field ID.
 * \param [in] idx Index of field elements.
 * \param [in] flist Field list to add new field map to.
 * \param [in] field_map_ptr Returns pointer to new field map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_alloc(int unit,
                uint32_t lfid,
                uint32_t idx,
                bcmltm_map_field_mapped_list_t *flist,
                bcmltm_map_field_mapped_t **field_map_ptr)
{
    bcmltm_map_field_mapped_t *field_map = NULL;
    bcmltm_map_field_mapped_t *new_field_map = NULL;
    uint32_t map_idx = 0;
    uint32_t found = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Check if (field_id, index) is new to the list */
    for (i = 0; i < flist->num; i++) {
        field_map = &flist->field_maps[i];
        if ((field_map->src_field_id == lfid) &&
            (field_map->src_field_idx == idx)) {
            map_idx = i;
            found = 1;
            break;
        }
    }

    if (!found) {
        /* New field, return array element */
        *field_map_ptr = &flist->field_maps[flist->num++];
        SHR_EXIT();
    }

    /* Field is not new, add map into corresponding element link list */
    field_map = &flist->field_maps[map_idx];

    /* Allocate */
    SHR_ALLOC(new_field_map, sizeof(*new_field_map),
              "LTM Logical Table map_field_mapped");
    SHR_NULL_CHECK(new_field_map, SHR_E_MEMORY);
    sal_memset(new_field_map, 0, sizeof(*new_field_map));

    /* Insert it first in the link list (order does not matter) */
    new_field_map->next = field_map->next;
    field_map->next = new_field_map;

    *field_map_ptr = new_field_map;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the fixed field metadata for given LRD field entry map.
 *
 * Get the fixed field metadata for given LRD field entry map.
 *
 * \param [in] unit Unit number.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] map_entry LRD field entry map.
 * \param [out] field_map Field structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_field_get(int unit,
                const bcmltm_wb_block_t *ptm_wb_block,
                const bcmlrd_map_entry_t *map_entry,
                bcmltm_fixed_field_t *field_map)
{
    SHR_FUNC_ENTER(unit);

    /* Set working buffer offset */
    /*
     * Fixed fields for Hash/TCAM are always expected to be part of
     * the Key entry.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                           map_entry->desc.field_id,
                                           BCMLTM_FIELD_TYPE_KEY,
                                           BCMLTM_FIELD_DIR_IN,
                                           map_entry->desc.entry_idx,
                                           &(field_map->wbc.buffer_offset)));

    /* Set min and max bit information */
    field_map->wbc.maxbit = map_entry->desc.maxbit;
    field_map->wbc.minbit = map_entry->desc.minbit;

    /* Set fixed value (entry in use) */
    field_map->field_value = map_entry->u.fixed.value;

    /* Set delete value (entry not in use)*/
    field_map->delete_value = map_entry->u.fixed.default_value;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Parse LRD field map entry which requires Field Validation.
 *
 * This routine parses the LRD field entry of types:
 *     BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION
 *     BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION
 *
 * It also gets any other information needed to
 * construct the metadata expected for the validation node cookies.
 *
 * \param [in] unit Unit number.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_mapped_validate_parse(int unit,
                                const bcmlrd_map_entry_t *entry,
                                bcmltm_map_entries_fv_t *lt_map)
{
    bcmltm_map_fv_list_t *fv_list;
    const bcmltd_field_val_handler_t *fv_handler = NULL;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    /* Field Validation Handler */
    fv_handler = bcmltm_lta_intf_val_handler_get(unit, entry->u.handler_id);

    /* Sanity check */
    if ((fv_handler == NULL) ||
        (fv_handler->arg == NULL) ||
        (fv_handler->arg->fields <= 0)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid Field Validation handler\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (entry->entry_type == BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION) {
        fv_list = &lt_map->validate_keys;
    } else if (entry->entry_type == BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION) {
        fv_list = &lt_map->validate_values;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unsupported entry type\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    idx = fv_list->num;
    if (idx >= fv_list->max) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Field Validation index is out of bound\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    sal_memcpy(&fv_list->fv[idx], fv_handler, sizeof(bcmltd_field_val_handler_t));

    fv_list->num = ++idx;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Parse LRD map entry for a direct PT field map.
 *
 * This routine parses the LRD entry of types:
 *     BCMLRD_MAP_ENTRY_MAPPED_KEY
 *     BCMLRD_MAP_ENTRY_MAPPED_VALUE
 *
 * It also gets any other information needed to construct the
 * metadata expected in bcmltm_ptm_field_map_t.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] map LRD map.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_mapped_parse(int unit, bcmlrd_sid_t sid,
                       const bcmltm_table_attr_t *attr,
                       const bcmltm_wb_block_t *ptm_wb_block,
                       const bcmlrd_map_t *map,
                       const bcmlrd_map_entry_t *entry,
                       bcmltm_map_entries_pt_t *lt_map)
{
    bcmltm_map_field_mapped_t *field_map = NULL;
    bcmltm_map_field_mapped_list_t *flist;
    uint32_t lfid, idx_count, idx;
    bcmltm_field_type_t field_type = BCMLRD_FIELD_F_KEY;
    uint16_t fidx_minbit[MAX_FIELD_IDXS];
    uint16_t fidx_maxbit[MAX_FIELD_IDXS];
    uint32_t num_fidx;
    uint64_t *default_value = NULL;
    uint32_t default_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;

    SHR_FUNC_ENTER(unit);

    /* Logical source field ID */
    lfid = entry->u.mapped.src.field_id;

    /* Key or Value Field Map */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_map_entry_mapped_field_type_get(unit, attr->type, lfid,
                                                   map, entry, &field_type));
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        flist = &lt_map->keys;
    } else {
        flist = &lt_map->values;
    }

    /* INSTANCE virtual field indicates table has access per instance */
    if (entry->desc.field_id == BCMLRD_FIELD_INSTANCE) {
        lt_map->table_inst = TRUE;
    }

    /* Get field min and max bits per index */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_field_to_fidx_min_max(unit, entry->desc.field_id,
                                         entry->desc.minbit,
                                         entry->desc.maxbit,
                                         MAX_FIELD_IDXS,
                                         fidx_minbit, fidx_maxbit,
                                         &num_fidx));
    /*
     * An LRD direct map entry is expected only for a mapped field.
     * An unmapped field at this point is considered an error.
     */

    /* Get number of field indices */
    idx_count = bcmlrd_field_idx_count_get(unit, sid, lfid);

    /* All fields has at least 1 index count for a mapped entry */
    if (idx_count < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid field index count of zero "
                              "for mapped field: "
                              "sid=%d fid=%d\n"),
                   sid, lfid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW default value for field */
    SHR_ALLOC(default_value, sizeof(*default_value) * idx_count,
              "LTM field default values");
    SHR_NULL_CHECK(default_value, SHR_E_MEMORY);
    sal_memset(default_value, 0, sizeof(*default_value) * idx_count);
    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit, sid, lfid,
                                  idx_count,
                                  default_value,
                                  &default_count));
    if (idx_count != default_count) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field defaults: "
                              "sid=%d fid=%d "
                              "expected=%d actual=%d\n"),
                   sid, lfid, idx_count, default_count));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW minimum value for field */
    SHR_ALLOC(minimum_value, sizeof(*minimum_value) * idx_count,
              "LTM field minimum values");
    SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
    sal_memset(minimum_value, 0, sizeof(*minimum_value) * idx_count);
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, sid, lfid,
                              idx_count,
                              minimum_value,
                              &minimum_count));
    if (idx_count != minimum_count) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field minimums: "
                              "sid=%d fid=%d "
                              "expected=%d actual=%d\n"),
                   sid, lfid, idx_count, minimum_count));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW maximum value for field */
    SHR_ALLOC(maximum_value, sizeof(*maximum_value) * idx_count,
              "LTM field maximum values");
    SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);
    sal_memset(maximum_value, 0, sizeof(*maximum_value) * idx_count);
    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, sid, lfid,
                              idx_count,
                              maximum_value,
                              &maximum_count));
    if (idx_count != maximum_count) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field maximums: "
                              "sid=%d fid=%d "
                              "expected=%d actual=%d\n"),
                   sid, lfid, idx_count, maximum_count));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < idx_count; idx++) {
        /* Allocate new map object in list */
        SHR_IF_ERR_EXIT
            (field_map_alloc(unit, lfid, idx, flist, &field_map));

        field_map->src_field_idx = idx;
        field_map->src_field_id = lfid;
        field_map->default_value = default_value[idx];
        field_map->minimum_value = minimum_value[idx];
        field_map->maximum_value = maximum_value[idx];
        field_map->dst_field_id = entry->desc.field_id;
        field_map->ptm_wb_block = ptm_wb_block;
        field_map->pt_entry_idx = entry->desc.entry_idx;
        /* Set WBC min and max bit information */
        if (idx < num_fidx) {
            field_map->wbc_minbit = fidx_minbit[idx];
            field_map->wbc_maxbit = fidx_maxbit[idx];
        } else {
            /*
             * Corresponding PTM min, max range doesn't exist.
             * To avoid PTM write, set BCMLTM_FIELD_BIT_POSITION_INVALID
             * for both minbit and maxbit.
             */
            field_map->wbc_minbit = BCMLTM_FIELD_BIT_POSITION_INVALID;
            field_map->wbc_maxbit = BCMLTM_FIELD_BIT_POSITION_INVALID;
        }
    }

 exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);
    SHR_FREE(default_value);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Parse LRD map entry for a fixed field map.
 *
 * This routine parses the LRD entry of types:
 *     BCMLRD_MAP_ENTRY_FIXED_KEY
 *     BCMLRD_MAP_ENTRY_FIXED_VALUE
 *
 * It also gets any other information needed to construct the
 * metadata expected in bcmltm_fixed_field_t.
 *
 * \param [in] unit Unit number.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_fixed_parse(int unit,
                      const bcmltm_wb_block_t *ptm_wb_block,
                      const bcmlrd_map_entry_t *entry,
                      bcmltm_map_entries_pt_t *lt_map)
{
    bcmltm_fixed_field_t *field;

    SHR_FUNC_ENTER(unit);

    /* Check array size */
    if (lt_map->fixed.num >= lt_map->fixed.max) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Fill information */
    field = &lt_map->fixed.field_maps[lt_map->fixed.num];
    SHR_IF_ERR_EXIT
        (fixed_field_get(unit, ptm_wb_block, entry, field));
    lt_map->fixed.num++;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse LRD map entry for field transforms.
 *
 * This routine parses the LRD entry of types:
 *     BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER
 *     BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER
 *     BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER
 *     BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER
 *
 * It also gets any other information needed to construct the
 * metadata expected for the field transform node cookies.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] ptid Physical table ID corresponding to the PT map group.
 * \param [in] ptm_wb_block PTM working buffer block corresponding to the PT.
 * \param [in] entry LRD map entry to parse.
 * \param [out] lt_map LT map structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entry_xfrm_parse(int unit, bcmlrd_sid_t sid,
                     bcmdrd_sid_t ptid,
                     const bcmltm_wb_block_t *ptm_wb_block,
                     const bcmlrd_map_entry_t *entry,
                     bcmltm_map_entries_pt_t *lt_map)
{
    bcmltm_map_field_xfrm_list_t *xfrm_list;
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    bcmltm_xfrm_dst_fields_t *xfrm_dst = NULL;
    uint32_t idx;
    uint32_t num_fields;
    uint32_t findex;
    bcmltm_field_dir_t field_dir;

    SHR_FUNC_ENTER(unit);

    /* Transform Handler */
    xfrm = bcmltm_lta_intf_xfrm_handler_get(unit, entry->u.handler_id);

    /* Sanity check */
    if (xfrm == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Transform Handler is NULL\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) {
        xfrm_list = &lt_map->fwd_key_xfrms;
        field_dir = BCMLTM_FIELD_DIR_IN;
    } else if (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER) {
        xfrm_list = &lt_map->fwd_value_xfrms;
        field_dir = BCMLTM_FIELD_DIR_IN;
    } else if (entry->entry_type == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER) {
        xfrm_list = &lt_map->rev_key_xfrms;
        field_dir = BCMLTM_FIELD_DIR_OUT;
    } else if (entry->entry_type == BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER) {
        xfrm_list = &lt_map->rev_value_xfrms;
        field_dir = BCMLTM_FIELD_DIR_OUT;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unsupported entry type\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    idx = xfrm_list->num;
    if (idx >= xfrm_list->max) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Transform index is out of bound\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    sal_memcpy(&xfrm_list->xfrm[idx], xfrm, sizeof(bcmltd_xfrm_handler_t));

    if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
        (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
        num_fields = xfrm->arg->rfields;
    } else {
        num_fields = xfrm->arg->fields;
    }
    xfrm_dst = &xfrm_list->xfrm_dst[idx];
    xfrm_dst->ptid = ptid;
    xfrm_dst->num = num_fields;

    if (num_fields == 0) {
        xfrm_list->num = ++idx;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_ALLOC(xfrm_dst->field,
              sizeof(bcmltm_xfrm_dst_field_t) * num_fields,
              "LTM Logical Table xfrm physical fields");
    SHR_NULL_CHECK(xfrm_dst->field, SHR_E_MEMORY);
    sal_memset(xfrm_dst->field, 0,
               sizeof(bcmltm_xfrm_dst_field_t) * num_fields);


    /* field list in xfrm */
    for (findex = 0; findex < num_fields ; findex++) {
        bcmltm_xfrm_dst_field_t dst_field;
        uint32_t field_id;  /* PT field ID */
        bcmltm_field_type_t field_type;
        uint16_t pt_entry_idx;

        if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
            (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
            field_id = xfrm->arg->rfield_list->field_array[findex].field_id;
            pt_entry_idx = xfrm->arg->rfield_list->field_array[findex].entry_idx;
        } else {
            field_id = xfrm->arg->field_list->field_array[findex].field_id;
            pt_entry_idx = xfrm->arg->field_list->field_array[findex].entry_idx;
        }

        /* If entry_type is KEY transform handler,
         * destination physical field would be key field. */
        if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
            (entry->entry_type == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER)) {
            field_type = BCMLTM_FIELD_TYPE_KEY;
        } else {
            field_type = BCMLTM_FIELD_TYPE_VALUE;
        }
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                               field_id, field_type,
                                               field_dir,
                                               pt_entry_idx,
                                               &(dst_field.wbc.buffer_offset)));

        if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
            (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
            dst_field.wbc.maxbit = xfrm->arg->rfield_list->field_array[findex].maxbit;
            dst_field.wbc.minbit = xfrm->arg->rfield_list->field_array[findex].minbit;
        } else {
            dst_field.wbc.maxbit = xfrm->arg->field_list->field_array[findex].maxbit;
            dst_field.wbc.minbit = xfrm->arg->field_list->field_array[findex].minbit;
        }
        dst_field.field_id = field_id;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Table %d (%d), field (%d), "
                                "wb offset, min, max = (%d, %d, %d)\n"),
                     sid, ptid, field_id,
                     dst_field.wbc.buffer_offset,
                     dst_field.wbc.minbit,
                     dst_field.wbc.maxbit));

        sal_memcpy(&xfrm_dst->field[findex], &dst_field,
                   sizeof(bcmltm_xfrm_dst_field_t));

        /* INSTANCE virtual field indicates table has access per instance */
        if (field_id == BCMLRD_FIELD_INSTANCE) {
            lt_map->table_inst = TRUE;
        }

        if (xfrm->arg->tables > 0) {
            lt_map->table_xfrm = TRUE;
        } else {
            lt_map->table_xfrm = FALSE;
        }
    }

    xfrm_list->num = ++idx;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize LT field list map information.
 *
 * This routine initializes the LT field list map information.
 *
 * \param [in] unit Unit number.
 * \param [in] flist_count Array size.
 * \param [out] flist LT field list map to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_mapped_init(int unit,
                      uint32_t flist_count,
                      bcmltm_map_field_mapped_list_t *flist)
{
    bcmltm_map_field_mapped_t *field_maps = NULL;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    flist->max = flist_count;
    flist->num = 0;
    flist->field_maps = NULL;

    /* Allocate field map array */
    SHR_ALLOC(field_maps, sizeof(*field_maps) * flist_count,
              "LTM Logical Table field maps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, sizeof(*field_maps) * flist_count);
    flist->field_maps = field_maps;

    /* Initialize field index to indicate all fields are invalid */
    for (i = 0; i < flist_count; i++) {
        field_maps[i].src_field_idx = BCMLTM_FIELD_INDEX_UNDEFINED;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup LT field list map information.
 *
 * Cleanup LT field list map information.
 *
 * \param [out] flist - Field list to cleanup.
 */
static void
map_field_mapped_cleanup(bcmltm_map_field_mapped_list_t *flist)
{
    uint32_t idx;
    bcmltm_map_field_mapped_t *field_map = NULL;
    bcmltm_map_field_mapped_t *next_map = NULL;

    if (flist->field_maps != NULL) {
        /* Free elements in link list */
        for (idx = 0; idx < flist->max; idx++) {
            field_map = flist->field_maps[idx].next;
            while (field_map != NULL) {
                next_map = field_map->next;
                SHR_FREE(field_map);
                field_map = next_map;
            }
        }

        SHR_FREE(flist->field_maps);
    }

    return;
}


/*!
 * \brief Initialize LT fixed field list map information.
 *
 * This routine initializes the LT fixed field list map information.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD table map.
 * \param [out] flist LT field list map to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_fixed_init(int unit,
                     const bcmlrd_map_t *map,
                     bcmltm_map_field_fixed_list_t *flist)
{
    bcmltm_fixed_field_t *field_maps = NULL;
    const bcmlrd_map_group_t *group = NULL;
    uint32_t group_idx;
    size_t num_fields;
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);

    /* Get size for the worse case:  include all fields for all PTs */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Skip groups that are not of Physical table kind */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, group->dest.id,
                                    0, NULL, &num_fields));
        count += num_fields;
    }

    flist->max = count;
    flist->num = 0;

    /* Allocate field map array */
    SHR_ALLOC(field_maps, sizeof(*field_maps) * count,
              "LTM Logical Table fixed field maps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, sizeof(*field_maps) * count);
    flist->field_maps = field_maps;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup LT fixed field list map information.
 *
 * Cleanup LT fixed field list map information.
 *
 * \param [out] flist - Field list to cleanup.
 */
static void
map_field_fixed_cleanup(bcmltm_map_field_fixed_list_t *flist)
{
    SHR_FREE(flist->field_maps);

    return;
}

/*!
 * \brief Count number of Transform entry.
 *
 * Count number of Transform entry.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD map.
 * \param [out] fwd_key_xfrm_count Number of forward Key Transform entry.
 * \param [out] fwd_value_xfrm_count Number of forward Value Transform entry.
 * \param [out] rev_key_xfrm_count Number of reverse Key Transform entry.
 * \param [out] rev_value_xfrm_count Number of reverse Value Transform entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_xfrm_count_get(int unit,
                         const bcmlrd_map_t *map,
                         uint32_t *fwd_key_xfrm_count,
                         uint32_t *fwd_value_xfrm_count,
                         uint32_t *rev_key_xfrm_count,
                         uint32_t *rev_value_xfrm_count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;

    SHR_FUNC_ENTER(unit);

    /* Count number of transform entry */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Process Physical table kind */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }
        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            if (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) {
                (*fwd_key_xfrm_count)++;
            } else if (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER) {
                (*fwd_value_xfrm_count)++;
            } else if (entry->entry_type == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER) {
                (*rev_key_xfrm_count)++;
            } else if (entry->entry_type == BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER) {
                (*rev_value_xfrm_count)++;
            }
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize LT xfrm list map information.
 *
 * This routine initializes the LT xfrm list map information.
 *
 * \param [in] unit Unit number.
 * \param [in] xfrm_count Number of Field Transform.
 * \param [out] xfrm_list LT field list map to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_xfrm_init(int unit,
                    uint32_t xfrm_count,
                    bcmltm_map_field_xfrm_list_t *xfrm_list)
{
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    if (xfrm_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    xfrm_list->max = xfrm_count;
    xfrm_list->num = 0;
    size = sizeof(bcmltd_xfrm_handler_t) * xfrm_count;

    /* Allocate field map array */
    SHR_ALLOC(xfrm_list->xfrm, size,
              "LTM Logical Table Transform Maps");
    SHR_NULL_CHECK(xfrm_list->xfrm, SHR_E_MEMORY);
    sal_memset(xfrm_list->xfrm, 0, size);

    /* Allocate field map array */
    SHR_ALLOC(xfrm_list->xfrm_dst,
              sizeof(bcmltm_xfrm_dst_fields_t) * xfrm_count,
              "LTM Logical Table Transform Target Maps");
    SHR_NULL_CHECK(xfrm_list->xfrm_dst, SHR_E_MEMORY);
    sal_memset(xfrm_list->xfrm_dst, 0,
               sizeof(bcmltm_xfrm_dst_fields_t) * xfrm_count);

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup LT field xfrm list map information.
 *
 * Cleanup LT field xfrm list map information.
 *
 * \param [out] xfrm_list xfrm list map to cleanup.
 */
static void
map_field_xfrm_cleanup(bcmltm_map_field_xfrm_list_t *xfrm_list)
{
    if (xfrm_list == NULL) {
        return;
    }

    if (xfrm_list->xfrm != NULL) {
        SHR_FREE(xfrm_list->xfrm);
    }

    if (xfrm_list->xfrm_dst != NULL) {
        uint32_t num_xfrm = xfrm_list->num;
        uint32_t idx;

        for (idx = 0; idx < num_xfrm ; idx++) {
            if (xfrm_list->xfrm_dst[idx].field != NULL) {
                SHR_FREE(xfrm_list->xfrm_dst[idx].field);
            }
        }
        SHR_FREE(xfrm_list->xfrm_dst);
    }

    return;
}


/*!
 * \brief Count number of Field Validation entries.
 *
 * Count number of Field Validation entries.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD map.
 * \param [out] key_fv_count Number of Key Field Validation entries.
 * \param [out] value_fv_count Number of Value Field Validation entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_fv_count_get(int unit,
                       const bcmlrd_map_t *map,
                       uint32_t *key_fv_count,
                       uint32_t *value_fv_count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;

    SHR_FUNC_ENTER(unit);

    /* Count number of transform entry */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Process Field Validation kind */
        if (group->dest.kind != BCMLRD_MAP_VALIDATION) {
            continue;
        }
        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            if (entry->entry_type == BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION) {
                (*key_fv_count)++;
            } else if (entry->entry_type == BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION) {
                (*value_fv_count)++;
            }
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize LT field list map information for Field Validation.
 *
 * This routine initializes the LT field list map information for Field
 * Validation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fv_count Number of Field Validation entries
 * \param [out] fv_list LT mapping for Field Validation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_field_mapped_validate_init(int unit, bcmlrd_sid_t sid,
                               uint32_t fv_count,
                               bcmltm_map_fv_list_t *fv_list)
{
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    if (fv_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    fv_list->max = fv_count;
    fv_list->num = 0;
    size = sizeof(bcmltd_field_val_handler_t) * fv_count;

    /* Allocate field map array */
    SHR_ALLOC(fv_list->fv, size,
              "LTM Logical Table Field Validation Maps");
    SHR_NULL_CHECK(fv_list->fv, SHR_E_MEMORY);
    sal_memset(fv_list->fv, 0, size);

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup LT field list map information for Field Validation.
 *
 * Cleanup LT field list map information for Field Validation.
 *
 * \param [out] list field list to cleanup.
 */
static void
map_field_mapped_validate_cleanup(bcmltm_map_fv_list_t *list)
{
    if (list == NULL) {
        return;
    }

    if (list->fv != NULL) {
        SHR_FREE(list->fv);
    }

    return;
}


/*!
 * \brief Count number of Table Commit entries.
 *
 * Count number of Table Commit entries.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD map.
 * \param [out] count Number of Table Commit entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_entries_tc_count_get(int unit,
                         const bcmlrd_map_t *map,
                         uint32_t *count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num = 0;

    *count = 0;

    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Check for Table Commit kind */
        if (group->dest.kind != BCMLRD_MAP_TABLE_COMMIT) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            if (entry->entry_type == BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER) {
                num++;
            }
        }
    }

    *count = num;

    return SHR_E_NONE;
}


/*!
 * \brief Get the Logical Table type.
 *
 * This function gets the logical table type as specified in
 * the table definition.
 *
 * The LT map group kind along with its defined table type (which
 * should reflect nature of the key fields) are reflected in
 * the returned table type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] type Table type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_type_get(int unit, bcmlrd_sid_t sid, bcmltm_table_type_t *type)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    uint32_t ltm_count;
    uint32_t cth_count;
    uint32_t pt_count;
    uint32_t groups_count = 0;
    uint32_t lrd_type, tbl_flags;

    SHR_FUNC_ENTER(unit);

    *type = 0;

    /* Get group map counts to identify group kind */
    SHR_IF_ERR_EXIT(map_ltm_count_get(unit, sid, &ltm_count));
    SHR_IF_ERR_EXIT(map_cth_count_get(unit, sid, &cth_count));
    SHR_IF_ERR_EXIT(map_pt_count_get(unit, sid, &pt_count));

    /* Sanity checks */
    /*
     * Rule:
     * - A table can contain groups of only 1 kind.
     * - A table LT can have only 1 LTM group.
     * - A table LT can have only 1 CTH group.
     * - A table can have several PT groups (direct PT maps).
     */
    if (ltm_count > 0) {
        groups_count++;
    }
    if (cth_count > 0) {
        groups_count++;
    }
    if (pt_count > 0) {
        groups_count++;
    }
    if (groups_count > 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LT table cannot have more than 1 kind of "
                              "map group: sid=%d "
                              "ltm_count=%d cth_count=%d pt_count=%d\n"),
                   sid, ltm_count, cth_count, pt_count));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Check that LTM and CTH map groups are no more than 1 */
    if (ltm_count > 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LT table cannot have more than 1 LTM group: "
                              "sid=%d ltm_count=%d\n"),
                   sid, ltm_count));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    if (cth_count > 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LT table cannot have more than 1 CTH group: "
                              "sid=%d cth_count=%d\n"),
                   sid, cth_count));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    tbl = bcmlrd_table_get(sid);
    if (tbl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not get LRD table representation: "
                              "sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /* Set type flags */
    *type = BCMLTM_TABLE_TYPE_FLAG_CATG_LT;
    if (ltm_count > 0) {
        *type |= BCMLTM_TABLE_TYPE_FLAG_MAP_LTM;
    } else if (cth_count > 0) {
        *type |= BCMLTM_TABLE_TYPE_FLAG_MAP_CTH;
    } else if (pt_count > 0) {
        *type |= BCMLTM_TABLE_TYPE_FLAG_MAP_PT;
    }

    if (tbl->flags & BCMLRD_TABLE_F_TYPE_MAPPED) {
        SHR_IF_ERR_EXIT(
            bcmlrd_map_table_attr_get(unit, sid,
                                      BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_TYPE,
                                      &tbl_flags));
    } else {
        tbl_flags = tbl->flags;
    }
    lrd_type = tbl_flags & BCMLTD_TABLE_F_TYPE_MASK;
    if (lrd_type == BCMLTD_TABLE_F_TYPE_INDEX) {
        if (tbl_flags & BCMLTD_TABLE_F_TYPE_INDEX_ALLOC) {
            *type |= BCMLTM_TABLE_TYPE_FLAG_KEY_ALLOC_INDEX;
        } else {
            *type |= BCMLTM_TABLE_TYPE_FLAG_KEY_SIMPLE_INDEX;
        }
    } else if (lrd_type == BCMLTD_TABLE_F_TYPE_TCAM) {
        *type |= BCMLTM_TABLE_TYPE_FLAG_KEY_TCAM;
    } else if (lrd_type == BCMLTD_TABLE_F_TYPE_HASH) {
        *type |= BCMLTM_TABLE_TYPE_FLAG_KEY_HASH;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Undefined LRD table type: "
                              "sid=%d\n lrd_table_flags=0x%x"),
                   sid, tbl->flags));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_nc_lt_map_attr_parse(int unit, bcmlrd_sid_t sid,
                            bcmltm_table_attr_t *attr)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_table_attr_t *tbl_attr = NULL;
    const bcmlrd_field_data_t *field_data = NULL;
    uint32_t attr_idx, lfid, num_attrs;
    uint32_t ro_cnt = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(attr, 0, sizeof(*attr));

    /* Set default attributes */
    attr->type = 0x0;
    attr->mode = BCMLTM_TABLE_MODE_MODELED;
    attr->flag = 0x0;
    attr->index_min = 0;
    attr->index_max = 0;
    attr->mem_param_min = 0;
    attr->mem_param_max = 0;
    attr->global_ltid = BCMLTD_SID_INVALID;

    /* Get table type */
    SHR_IF_ERR_EXIT(lt_type_get(unit, sid, &attr->type));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Set table_min, max for LTM group table */
    if (BCMLTM_TABLE_TYPE_LT_LTM(attr->type)) {
        uint32_t num_table = bcmltm_logical_table_count_get();
        attr->index_min = 0;
        attr->index_max = num_table - 1;
    }

    num_attrs = map->table_attr->attributes;
    for (attr_idx = 0; attr_idx < num_attrs; attr_idx++) {
        tbl_attr = &map->table_attr->attr[attr_idx];
        switch(tbl_attr->key) {
        case BCMLRD_MAP_TABLE_ATTRIBUTE_INTERACTIVE:
            /* Get table operating mode attribute */
            if (tbl_attr->value == TRUE) {
                attr->mode = BCMLTM_TABLE_MODE_INTERACTIVE;
            } else {
                attr->mode = BCMLTM_TABLE_MODE_MODELED;
            }
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MIN_INDEX:
            attr->index_min = tbl_attr->value;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MAX_INDEX:
            attr->index_max = tbl_attr->value;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_SIZE:
            attr->index_max = tbl_attr->value - 1;
            attr->index_min = 0;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MIN_INDEX:
            attr->mem_param_min = tbl_attr->value;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MAX_INDEX:
            attr->mem_param_max = tbl_attr->value;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MIN_INDEX:
            attr->mem_param_min = tbl_attr->value;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MAX_INDEX:
            attr->mem_param_max = tbl_attr->value;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_SHARED_INDEX:
            attr->flag |= BCMLTM_TABLE_FLAG_OVERLAY;
            attr->global_ltid = tbl_attr->value;
            break;
        default:
            break;
        }
    }

    for (lfid = 0; lfid < map->field_data->fields; lfid++) {
        field_data = &map->field_data->field[lfid];
        if (field_data->flags & BCMLRD_FIELD_F_READ_ONLY) {
            ro_cnt++;
        }
    }

    if (ro_cnt == map->field_data->fields) {
        attr->flag |= BCMLTM_TABLE_FLAG_READ_ONLY;
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_map_groups_parse(int unit, bcmlrd_sid_t sid,
                              const bcmltm_table_attr_t *attr,
                              bcmltm_map_groups_t *groups)
{
    SHR_FUNC_ENTER(unit);

    /* PT groups */
    SHR_IF_ERR_EXIT(map_pt_parse(unit, sid, attr, &groups->pt));

    /* FV groups */
    SHR_IF_ERR_EXIT(map_fv_parse(unit, sid, &groups->fv));

    /* Table Commit groups */
    SHR_IF_ERR_EXIT(map_tc_parse(unit, sid, &groups->tc));

    /* Custom Table Handler groups */
    SHR_IF_ERR_EXIT(map_cth_parse(unit, sid, &groups->cth));

    /* LTM groups */
    SHR_IF_ERR_EXIT(map_ltm_parse(unit, sid, &groups->ltm));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lt_map_groups_cleanup(groups);
    }

    SHR_FUNC_EXIT();
}


void
bcmltm_nc_lt_map_groups_cleanup(bcmltm_map_groups_t *groups)
{
    /* PT groups */
    map_pt_cleanup(groups->pt);
    groups->pt = NULL;

    /* FV groups */
    map_fv_cleanup(groups->fv);
    groups->fv = NULL;

    /* Table Commit groups */
    map_tc_cleanup(groups->tc);
    groups->tc = NULL;

    /* Custom Table Handler groups */
    map_cth_cleanup(groups->cth);
    groups->cth = NULL;

    /* LTM groups */
    map_ltm_cleanup(groups->ltm);
    groups->ltm = NULL;

    return;
}


int
bcmltm_nc_lt_map_entries_pt_parse(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_table_attr_t *attr,
                                  const bcmltm_map_pt_t *pt_map_group,
                                  bcmltm_map_entries_pt_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num_fid;  /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;  /* Total fields elements (fid, idx) */
    uint32_t num_fwd_key_xfrm = 0, num_fwd_value_xfrm = 0;
    uint32_t num_rev_key_xfrm = 0, num_rev_value_xfrm = 0;
    bcmdrd_sid_t ptid;
    const bcmltm_wb_block_t *ptm_wb_block = NULL;
    bcmltm_table_type_t lt_type;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Get number of mapped LT field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_lt_field_count_get(unit, sid, TRUE,
                                   &num_fid, &num_fid_idx));

    /* Init Keys field maps */
    SHR_IF_ERR_EXIT
        (map_field_mapped_init(unit, num_fid_idx, &lt_map->keys));

    /* Init Values field maps */
    SHR_IF_ERR_EXIT
        (map_field_mapped_init(unit, num_fid_idx, &lt_map->values));

    /* Init Fixed field maps */
    SHR_IF_ERR_EXIT
        (map_field_fixed_init(unit, map, &lt_map->fixed));

    /* Count number of forward/reverse Field Transform */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_count_get(unit, map,
                                  &num_fwd_key_xfrm, &num_fwd_value_xfrm,
                                  &num_rev_key_xfrm, &num_rev_value_xfrm));

    /* Init Forward Key Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_fwd_key_xfrm,
                             &lt_map->fwd_key_xfrms));

    /* Init Forward Value Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_fwd_value_xfrm,
                             &lt_map->fwd_value_xfrms));

    /* Init Reverse Key Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_rev_key_xfrm,
                             &lt_map->rev_key_xfrms));

    /* Init Reverse Value Transform field maps */
    SHR_IF_ERR_EXIT
        (map_field_xfrm_init(unit, num_rev_value_xfrm,
                             &lt_map->rev_value_xfrms));

    /* Get lt type */
    lt_type = attr->type;

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Physical table kind */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* Find PTM working buffer block */
        ptid = group->dest.id;
        ptm_wb_block = bcmltm_ptm_wb_block_find(unit, sid, lt_type,
                                                pt_map_group, ptid);
        if (ptm_wb_block == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM PTM working buffer block "
                                  "not found: ltid=%d ptid=%d\n"),
                       sid, ptid));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY:
            case BCMLRD_MAP_ENTRY_MAPPED_VALUE:
                SHR_IF_ERR_EXIT
                    (map_entry_mapped_parse(unit, sid, attr, ptm_wb_block,
                                            map, entry,
                                            lt_map));
                break;

            case BCMLRD_MAP_ENTRY_FIXED_KEY:
            case BCMLRD_MAP_ENTRY_FIXED_VALUE:
                SHR_IF_ERR_EXIT
                    (map_entry_fixed_parse(unit, ptm_wb_block, entry,
                                           lt_map));
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
            case BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER:
                SHR_IF_ERR_EXIT
                    (map_entry_xfrm_parse(unit, sid, ptid,
                                          ptm_wb_block, entry,
                                          lt_map));
                break;

            default:
                break;
            }
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


void
bcmltm_nc_lt_map_entries_pt_cleanup(bcmltm_map_entries_pt_t *lt_map)
{
    map_field_mapped_cleanup(&lt_map->keys);
    map_field_mapped_cleanup(&lt_map->values);
    map_field_fixed_cleanup(&lt_map->fixed);
    map_field_xfrm_cleanup(&lt_map->fwd_key_xfrms);
    map_field_xfrm_cleanup(&lt_map->fwd_value_xfrms);
    map_field_xfrm_cleanup(&lt_map->rev_key_xfrms);
    map_field_xfrm_cleanup(&lt_map->rev_value_xfrms);

    return;
}

int
bcmltm_nc_lt_map_entries_fv_parse(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_map_fv_t *map_group,
                                  bcmltm_map_entries_fv_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num_key_fv = 0;
    uint32_t num_value_fv = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Count number of Field Validation entries */
    SHR_IF_ERR_EXIT
        (map_field_fv_count_get(unit, map,
                                &num_key_fv, &num_value_fv));
    /* Init Validate Keys field maps */
    SHR_IF_ERR_EXIT
        (map_field_mapped_validate_init(unit, sid, num_key_fv,
                                        &lt_map->validate_keys));

    /* Init Validate Values field maps */
    SHR_IF_ERR_EXIT
        (map_field_mapped_validate_init(unit, sid, num_value_fv,
                                        &lt_map->validate_values));

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Field Validation kind */
        if (group->dest.kind != BCMLRD_MAP_VALIDATION) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION:
            case BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION:

                SHR_IF_ERR_EXIT
                    (map_entry_mapped_validate_parse(unit, entry, lt_map));
                break;

            default:
                break;
            }
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lt_map_entries_fv_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_nc_lt_map_entries_fv_cleanup(bcmltm_map_entries_fv_t *lt_map)
{
    map_field_mapped_validate_cleanup(&lt_map->validate_keys);
    map_field_mapped_validate_cleanup(&lt_map->validate_values);

    return;
}

int
bcmltm_nc_lt_map_entries_tc_parse(int unit, bcmlrd_sid_t sid,
                                  bcmltm_map_entries_tc_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t total_num_tc = 0;
    uint32_t *handler_id = NULL;
    uint32_t handler_idx = 0;
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Count number of Table Commit entries */
    SHR_IF_ERR_EXIT
        (map_entries_tc_count_get(unit, map, &total_num_tc));

    /* Allocate handler ids array */
    size = sizeof(*handler_id) * total_num_tc;
    SHR_ALLOC(handler_id, size, "LTM Table Commit handler list");
    SHR_NULL_CHECK(handler_id, SHR_E_MEMORY);
    sal_memset(handler_id, 0, size);

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Table Commit kind */
        if (group->dest.kind != BCMLRD_MAP_TABLE_COMMIT) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER:
                if (handler_idx >= total_num_tc) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "LTM Table Commit Handler: "
                                          "current table commit entry count "
                                          "exceeds max count sid=%d "
                                          "count=%d max_count=%d\n"),
                               sid, (handler_idx+1), total_num_tc));
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }

                handler_id[handler_idx] = entry->u.handler_id;
                handler_idx++;
                break;
            default:
                break;
            }
        }
    }

    lt_map->num_handlers = handler_idx;
    lt_map->handler_id = handler_id;

 exit:
    if (SHR_FUNC_ERR()) {
        if (lt_map->handler_id == NULL) {
            SHR_FREE(handler_id);
        }
        bcmltm_nc_lt_map_entries_tc_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_nc_lt_map_entries_tc_cleanup(bcmltm_map_entries_tc_t *lt_map)
{
    SHR_FREE(lt_map->handler_id);
    lt_map->handler_id = NULL;

    return;
}

int
bcmltm_nc_lt_map_entries_cth_parse(int unit, bcmlrd_sid_t sid,
                                   bcmltm_map_entries_cth_t *lt_map)
{
    int rv;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num_fid;  /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;    /* Total fields elements (fid, idx) */
    bcmltm_map_field_t *field = NULL;     /* Total field list */
    uint32_t key_field_idx = 0 ;
    bcmltm_map_field_t *key_field = NULL; /* Total Key field list */
    unsigned int size;
    uint32_t i;
    uint32_t fid;
    uint32_t idx;
    uint32_t idx_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;
    bcmlrd_field_def_t field_def;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Get number of unique API facing LT fields and total fields(fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_lt_field_count_get(unit, sid, FALSE,
                                   &num_fid, &num_fid_idx));

    /* Allocate field IDs array */
    size = sizeof(*field) * num_fid_idx;
    SHR_ALLOC(field, size, "LTM CTH field list");
    SHR_NULL_CHECK(field, SHR_E_MEMORY);
    sal_memset(field, 0, size);

    /* Allocate field IDs array for Key field list */
    size = sizeof(*key_field) * num_fid_idx;
    SHR_ALLOC(key_field, size, "LTM key field list");
    SHR_NULL_CHECK(key_field, SHR_E_MEMORY);
    sal_memset(key_field, 0, size);

    /*
     * Assign Fields (ID,Index)
     *
     * Field IDs for Custom Table Handlers are not provided by the LRD
     * because API field IDs are always from [0..num_fields-1] (by design).
     */
    for (i = 0, fid = 0; fid < num_fid; fid++) {

        rv = bcmlrd_table_field_def_get(unit, sid, fid, &field_def);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_UNAVAIL) {
                continue;  /* Skip unmapped field */
            } else {
                SHR_RETURN_VAL_EXIT(rv);
            }
        }

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        /* All fields has at least 1 index count for mapped fields */
        if (idx_count < 1) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM group: invalid "
                                  "field index count (should be > 1) "
                                  "sid=%d fid=%d count=%d\n"),
                       sid, fid, idx_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW minimum value for field */
        SHR_ALLOC(minimum_value, sizeof(*minimum_value) * idx_count,
                  "LTM field minimum values");
        SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
        sal_memset(minimum_value, 0, sizeof(*minimum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_min_get(unit, sid, fid,
                                  idx_count,
                                  minimum_value,
                                  &minimum_count));
        if (idx_count != minimum_count) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field minimums: "
                                  "sid=%d fid=%d "
                                  "expected=%d actual=%d\n"),
                       sid, fid, idx_count, minimum_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW maximum value for field */
        SHR_ALLOC(maximum_value, sizeof(*maximum_value) * idx_count,
                  "LTM field maximum values");
        SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);
        sal_memset(maximum_value, 0, sizeof(*maximum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_max_get(unit, sid, fid,
                                  idx_count,
                                  maximum_value,
                                  &maximum_count));
        if (idx_count != maximum_count) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field maximums: "
                                  "sid=%d fid=%d "
                                  "expected=%d actual=%d\n"),
                       sid, fid, idx_count, maximum_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        for (idx = 0; idx < idx_count; idx++) {
            if (i >= num_fid_idx) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Custom Table Handler: current "
                                      "field count exceeds max count sid=%d "
                                      "count=%d max_count=%d\n"),
                           sid, (i+1), num_fid_idx));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            if (field_def.key) {
                key_field[key_field_idx].fid = fid;
                key_field[key_field_idx].idx = idx;
                key_field[key_field_idx].minimum_value =
                    minimum_value[idx];
                key_field[key_field_idx].maximum_value =
                    maximum_value[idx];
                key_field_idx++;
            }
            field[i].fid = fid;
            field[i].idx = idx;
            field[i].minimum_value = minimum_value[idx];
            field[i].maximum_value = maximum_value[idx];
            i++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
    }

    lt_map->key_field      = key_field;
    lt_map->num_key_fields = key_field_idx;
    lt_map->field          = field;
    lt_map->num_fields     = i;

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Custom Table Handler group kind */
        if (group->dest.kind != BCMLRD_MAP_CUSTOM) {
            continue;
        }

        /* Process map entries until table handler entry is found */
        for (entry_idx = 0;
             (entry_idx < group->entries) && (lt_map->handler == NULL);
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_TABLE_HANDLER:
                lt_map->handler =
                    bcmltm_lta_intf_table_handler_get(unit, entry->u.handler_id);
                break;

            default:
                break;
            }
        }

        break;  /* Support 1 custom table handler entry per LT */
    }

 exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);

    if (SHR_FUNC_ERR()) {
        SHR_FREE(key_field);
        lt_map->key_field = NULL;

        SHR_FREE(field);
        lt_map->field = NULL;

        bcmltm_nc_lt_map_entries_cth_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}


void
bcmltm_nc_lt_map_entries_cth_cleanup(bcmltm_map_entries_cth_t *lt_map)
{
    SHR_FREE(lt_map->key_field);
    SHR_FREE(lt_map->field);
    lt_map->key_field = NULL;
    lt_map->field = NULL;

    return;
}

int
bcmltm_nc_lt_map_entries_ltm_parse(int unit, bcmlrd_sid_t sid,
                                   bcmltm_map_entries_ltm_t *lt_map)
{
    int rv;
    const bcmlrd_map_t *map = NULL;
    uint32_t num_fid;  /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;    /* Total fields elements (fid, idx) */
    bcmltm_map_field_t *key_field = NULL;
    uint32_t key_field_idx = 0 ;
    bcmltm_map_field_t*value_field = NULL;
    uint32_t value_field_idx = 0 ;
    unsigned int size;
    uint32_t i;
    uint32_t fid;
    uint32_t idx;
    uint32_t idx_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;
    bcmlrd_field_def_t field_def;


    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Get number of unique API facing LT fields and total fields(fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_lt_field_count_get(unit, sid, FALSE,
                                   &num_fid, &num_fid_idx));

    /* Allocate field IDs array */
    size = sizeof(*key_field) * num_fid_idx;
    SHR_ALLOC(key_field, size, "LTM key field list");
    SHR_NULL_CHECK(key_field, SHR_E_MEMORY);
    sal_memset(key_field, 0, size);

    size = sizeof(*value_field) * num_fid_idx;
    SHR_ALLOC(value_field, size, "LTM value field list");
    SHR_NULL_CHECK(value_field, SHR_E_MEMORY);
    sal_memset(value_field, 0, size);

    /*
     * Assign Fields (ID,Index)
     *
     * API field IDs for LTM group are always from [0..num_fields-1]
     * (by design).
     */
    for (i = 0, fid = 0; fid < num_fid; fid++) {

        rv = bcmlrd_table_field_def_get(unit, sid, fid, &field_def);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_UNAVAIL) {
                continue;  /* Skip unmapped field */
            } else {
                SHR_RETURN_VAL_EXIT(rv);
            }
        }

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);

        /* All fields has at least 1 index count for mapped fields */
        if (idx_count < 1) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM group: invalid "
                                  "field index count (should be > 1) "
                                  "sid=%d fid=%d count=%d\n"),
                       sid, fid, idx_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW minimum value for field */
        SHR_ALLOC(minimum_value, sizeof(*minimum_value) * idx_count,
                  "LTM field minimum values");
        SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
        sal_memset(minimum_value, 0, sizeof(*minimum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_min_get(unit, sid, fid,
                                  idx_count,
                                  minimum_value,
                                  &minimum_count));
        if (idx_count != minimum_count) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field minimums: "
                                  "sid=%d fid=%d "
                                  "expected=%d actual=%d\n"),
                       sid, fid, idx_count, minimum_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW maximum value for field */
        SHR_ALLOC(maximum_value, sizeof(*maximum_value) * idx_count,
                  "LTM field maximum values");
        SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);
        sal_memset(maximum_value, 0, sizeof(*maximum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_max_get(unit, sid, fid,
                                  idx_count,
                                  maximum_value,
                                  &maximum_count));
        if (idx_count != maximum_count) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field maximums: "
                                  "sid=%d fid=%d "
                                  "expected=%d actual=%d\n"),
                       sid, fid, idx_count, maximum_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        for (idx = 0; idx < idx_count; idx++) {
            if (i >= num_fid_idx) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM group: current "
                                      "field count exceeds max count sid=%d "
                                      "count=%d max_count=%d\n"),
                           sid, (i+1), num_fid_idx));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }

            if (field_def.key) {
                key_field[key_field_idx].fid = fid;
                key_field[key_field_idx].idx = idx;
                key_field[key_field_idx].minimum_value =
                    minimum_value[idx];
                key_field[key_field_idx].maximum_value =
                    maximum_value[idx];
                key_field_idx++;
            } else {
                value_field[value_field_idx].fid = fid;
                value_field[value_field_idx].idx = idx;
                value_field[value_field_idx].minimum_value =
                    minimum_value[idx];
                value_field[value_field_idx].maximum_value =
                    maximum_value[idx];
                value_field_idx++;
            }
            i++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
    }

    lt_map->key_field        = key_field;
    lt_map->num_key_fields   = key_field_idx;
    lt_map->value_field      = value_field;
    lt_map->num_value_fields = value_field_idx;

 exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);

    if (SHR_FUNC_ERR()) {
        SHR_FREE(key_field);
        lt_map->key_field = NULL;

        SHR_FREE(value_field);
        lt_map->value_field = NULL;

        bcmltm_nc_lt_map_entries_ltm_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}


void
bcmltm_nc_lt_map_entries_ltm_cleanup(bcmltm_map_entries_ltm_t *lt_map)
{
    SHR_FREE(lt_map->key_field);
    SHR_FREE(lt_map->value_field);
    lt_map->key_field = NULL;
    lt_map->value_field = NULL;

    return;
}
