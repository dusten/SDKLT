/*! \file bcmltm_nc_util.c
 *
 * Logical Table Manager - Node Cookie and Map Utility.
 *
 * This file contains utility routines to be used by the LTM Node Cookie
 * and Map routines.
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

#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Perform a left shift and fill rest with 1's */
#define RANGE_VALUE_LEFT_SHIFT(_val, _bits)         \
    (((_val) << (_bits)) + ((1 << (_bits)) - 1))

/*
 * Flags used in lt_field_count_get() to indicate desired fields to return.
 *
 * KEY         - Key fields
 * VALUE       - Value fields
 * MAPPED_ONLY - Only mapped fields (if not specified, then get
 *               all defined fields, mapped and unmapped)
 */
#define FIELD_F_KEY            0x00000001
#define FIELD_F_VALUE          0x00000002
#define FIELD_F_MAPPED_ONLY    0x00000004


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if logical field is to be processed based on flags.
 *
 * This helper routine checks whether the given logical field is to be
 * processed or skipped based on the specified flags as follows:
 *   FIELD_F_KEY         - Key fields
 *   FIELD_F_VALUE       - Value fields
 *   FIELD_F_MAPPED_ONLY - Only mapped fields (if not specified, then get
 *                         all defined fields, mapped and unmapped)
 *
 * \param [in] fid Logical field ID.
 * \param [in] tbl LRD table representation
 * \param [in] map LRD map.
 * \param [in] flags Indicates desired field type to process.
 *
 * \retval TRUE Field is to be processed based on flags.
 * \retval FALSE Field is to be skipped based on flags.
 */
static bool
lt_field_process(uint32_t fid,
                 const bcmlrd_table_rep_t *tbl, const bcmlrd_map_t *map,
                 uint32_t flags)
{
    bool process = TRUE;

    /*
     * Field: Key or Value
     * Need to check LRD table_rep because the LRD map
     * does not retain the KEY flag if field is unmapped.
     */
    if (tbl->field[fid].flags & BCMLRD_FIELD_F_KEY) {  /* Key field */
        /* Skip key field if not requested */
        if (!(flags & FIELD_F_KEY)) {
            process = FALSE;
        }
    } else {  /* Value field */
        /* Skip value field if not requested */
        if (!(flags & FIELD_F_VALUE)) {
            process = FALSE;
        }
    }

    /* Skip unmapped fields if only mapped fields are requested */
    if ((flags & FIELD_F_MAPPED_ONLY) &&
        bcmlrd_map_field_is_unmapped(map, fid)) {
        process = FALSE;
    }

    return process;
}


/*!
 * \brief Get number of field id and idx count for a given logical table.
 *
 * This core routine returns the count of logical fields IDs and
 * the total number of field elements (fid,idx) for the requested
 * types of field (indicated by 'flags') for the specified logical table.
 *
 * Notes:
 *   num_fid       - This is the number of logical fields
 *                   and does not include the indexes.
 *                   It represents the number of LT field IDs (fid).
 *
 *   num_fid_idx   - This is the number of the unique logical field
 *                   elements for a given table.  A field element
 *                   is identified by the field ID and index (fid,idx).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] flags Specifies requested fields FIELD_F_xxx.
 * \param [out] num_fid Number of field IDs (fid).
 * \param [out] num_fid_idx Number of field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_field_count_get(int unit, bcmlrd_sid_t sid,
                   uint32_t flags,
                   uint32_t *num_fid, uint32_t *num_fid_idx)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmlrd_map_t *map = NULL;
    uint32_t fid_count = 0;
    uint32_t fid_idx_count = 0;
    uint32_t fid;
    uint32_t idx_count;

    SHR_FUNC_ENTER(unit);

    *num_fid = *num_fid_idx = 0;

    /* Get LRD information */
    tbl = bcmlrd_table_get(sid);
    if (tbl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not get LRD table representation: "
                              "sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    for (fid = 0; fid < tbl->fields; fid++) {
        /* Check whether to process or skip field */
        if (!lt_field_process(fid, tbl, map, flags)) {
            continue;
        }

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);

        fid_idx_count += idx_count;
        fid_count++;
    }

    *num_fid = fid_count;
    *num_fid_idx = fid_idx_count;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get list of field IDs and index count for a given logical table.
 *
 * This core routine returns list of field IDs and associated index count
 * for the requested types of field (indicated by 'flags') for
 * the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] flags Specifies requested fields FIELD_F_xxx.
 * \param [in] list_max Size of allocated list.
 * \param [out] list List of field IDs and index count.
 * \param [out] num Number of fields returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_field_list_get(int unit, bcmlrd_sid_t sid, uint32_t flags,
                  uint32_t list_max,
                  bcmltm_fid_idx_t *list, uint32_t *num)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmlrd_map_t *map = NULL;
    uint32_t fid_count = 0;
    uint32_t fid;
    uint32_t idx_count;

    SHR_FUNC_ENTER(unit);

    /* Get LRD information */
    tbl = bcmlrd_table_get(sid);
    if (tbl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not get LRD table representation: "
                              "sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));


    for (fid = 0; fid < tbl->fields; fid++) {
        /* Check whether to process or skip field */
        if (!lt_field_process(fid, tbl, map, flags)) {
            continue;
        }

        if (fid_count >= list_max) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);

        list[fid_count].fid = fid;
        list[fid_count].idx_count = idx_count;

        fid_count++;
    }

    *num = fid_count;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Find given field in transform entry.
 *
 * This routine checks if the given field ID is part of the transform
 * field list.  Dependending on the input argument, it looks into
 * either the source field list (API facing field) or
 * the destination field list (PT).
 *
 * \param [in] unit Unit number.
 * \param [in] entry LRD transform map entry to search.
 * \param [in] src Indicates to look into the source or destination fields.
 * \param [in] fid Field ID to find.
 *
 * \retval TRUE Field ID is found.
 * \retval FALSE Field ID is not found.
 */
static bool
xfrm_field_find(int unit,
                const bcmlrd_map_entry_t *entry, bool src, uint32_t fid)
{
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    const uint32_t *fields;
    uint32_t num_fields;
    uint32_t fidx;

    /* Transform Handler */
    xfrm = bcmltm_lta_intf_xfrm_handler_get(unit, entry->u.handler_id);

    /* Sanity check */
    if (xfrm == NULL) {
        return FALSE;
    }

    /* Forward transform */
    if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
        (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
        if (src) {
            fields = xfrm->arg->field;
            num_fields = xfrm->arg->fields;
        } else {
            fields = xfrm->arg->rfield;
            num_fields = xfrm->arg->rfields;
        }
    } else { /* Reverse transform */
        if (src) {
            fields = xfrm->arg->rfield;
            num_fields = xfrm->arg->rfields;
        } else {
            fields = xfrm->arg->field;
            num_fields = xfrm->arg->fields;
        }
    }

    /* Look for field ID */
    for (fidx = 0; fidx < num_fields; fidx++) {
        if (fields[fidx] == fid) {
            return TRUE;
        }
    }

    return FALSE;
}


/*!
 * \brief Get the working buffer offset for the given PT param field ID.
 *
 * This routine gets the PTM working buffer offset for the specified
 * field ID (parameter section).
 *
 * \param [in] unit Unit number.
 * \param [in] pt_list PT list.
 * \param [in] ptid PT ID of field.
 * \param [in] fid Field ID to get offset for.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_param_offset_get(int unit,
                    const bcmltm_pt_list_t *pt_list, bcmdrd_sid_t ptid,
                    uint32_t fid, uint32_t *offset)
{
    uint32_t pt_view;
    uint32_t pt_op = 0;      /* For now, assume pt op is 0 */
    const bcmltm_pt_op_list_t *pt_op_list = NULL;
    bcmltm_wb_ptm_param_offset_t param;

    SHR_FUNC_ENTER(unit);

    /* Find PT */
    for (pt_view = 0; pt_view < pt_list->num_pt_view; pt_view++) {
        if (pt_list->mem_args[pt_view]->pt == ptid) {
            pt_op_list = pt_list->memop_lists[pt_view];
            break;
        }
    }

    if (pt_op_list == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get PTM parameter offsets */
    (void)bcmltm_wb_ptm_param_offset_get(pt_op_list->param_offsets[pt_op],
                                         &param);

    if (fid == BCMLRD_FIELD_INDEX) {
        *offset = param.index;
    } else if (fid == BCMLRD_FIELD_PORT) {
        *offset = param.port;
    } else if (fid == BCMLRD_FIELD_INSTANCE) {
        *offset = param.table_inst;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_lt_field_count_get(int unit, bcmlrd_sid_t sid,
                          bool mapped_only,
                          uint32_t *num_fid, uint32_t *num_fid_idx)
{
    uint32_t flags = FIELD_F_KEY | FIELD_F_VALUE;

    if (mapped_only) {
        flags |= FIELD_F_MAPPED_ONLY;
    }

    return lt_field_count_get(unit, sid, flags, num_fid, num_fid_idx);
}


int
bcmltm_lt_field_key_count_get(int unit, bcmlrd_sid_t sid,
                              bool mapped_only,
                              uint32_t *num_fid, uint32_t *num_fid_idx)
{
    uint32_t flags = FIELD_F_KEY;

    if (mapped_only) {
        flags |= FIELD_F_MAPPED_ONLY;
    }

    return lt_field_count_get(unit, sid, flags, num_fid, num_fid_idx);
}


int
bcmltm_lt_field_list_get(int unit, bcmlrd_sid_t sid,
                         bool mapped_only,
                         uint32_t list_max,
                         bcmltm_fid_idx_t *list, uint32_t *num)
{
    uint32_t flags = FIELD_F_KEY | FIELD_F_VALUE;

    if (mapped_only) {
        flags |= FIELD_F_MAPPED_ONLY;
    }

    return lt_field_list_get(unit, sid, flags, list_max, list, num);
}

int
bcmltm_lt_field_key_list_get(int unit, bcmlrd_sid_t sid,
                             bool mapped_only,
                             uint32_t list_max,
                             bcmltm_fid_idx_t *list, uint32_t *num)
{
    uint32_t flags = FIELD_F_KEY;

    if (mapped_only) {
        flags |= FIELD_F_MAPPED_ONLY;
    }

    return lt_field_list_get(unit, sid, flags, list_max, list, num);
}


int
bcmltm_nc_field_to_fidx_min_max(int unit, uint32_t fid,
                                uint16_t minbit, uint16_t maxbit,
                                uint32_t list_max,
                                uint16_t *fidx_minbit, uint16_t *fidx_maxbit,
                                uint32_t *num_fidx)
{
    uint32_t width;
    uint32_t idx;
    uint32_t idx_count;
    uint32_t last_idx;

    width = maxbit - minbit + 1;
    idx_count = bcmltd_field_idx_count_get(width);

    if (list_max < idx_count) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM field to field index array: "
                              "array count too small: "
                              "fid=%d max=%d actual=%d\n"),
                   fid, list_max, idx_count));
        return SHR_E_INTERNAL;
    }

    last_idx = idx_count - 1;
    for (idx = 0; idx < idx_count; idx++) {
        /* Minbit on field index element */
        fidx_minbit[idx] = minbit + (BCMLTM_FIELD_SIZE_BITS * idx);

        /* Maxbit on last field index element takes field maxbit */
        if (idx == last_idx) {
            fidx_maxbit[idx] = maxbit;
        } else {
            fidx_maxbit[idx] = fidx_minbit[idx] + (BCMLTM_FIELD_SIZE_BITS - 1);
        }
    }

    *num_fidx = idx_count;

    return SHR_E_NONE;
}


bool
bcmltm_nc_field_is_lt_index(int unit, bcmlrd_sid_t sid, uint32_t fid)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;

    /* Get LRD mapping */
    if (SHR_FAILURE(bcmlrd_map_get(unit, sid, &map))) {
        return FALSE;
    }

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* For each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            /* Check direct mapped destination field */
            if (entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY) {
                if ((entry->u.mapped.src.field_id == fid) &&
                    (entry->desc.field_id == BCMLRD_FIELD_INDEX)) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}


int
bcmltm_nc_pt_index_offsets_get(int unit, bcmlrd_sid_t sid,
                               const bcmltm_pt_list_t *pt_list,
                               uint32_t offset_max,
                               uint32_t *offset, uint32_t *num_offset)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid;
    uint32_t fid = 0;
    uint32_t offset_count = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find the key field(s) mapped to the PT index.
     * Usually, the LT index corresponds to the PT index:
     *   __INDEX
     *
     * There are cases where a field is mapped to both __INDEX and also
     * to a memory parameter (i.e. __PORT, __INSTANCE).  In this case,
     * the offset is part of the 'index' offset (for index copy and
     * tracking purposes).
     *
     * Example of logical key field is mapped to both an index and
     * memory parameter:
     *           F1: PT1.__INDEX
     *           F1: PT2.__PORT
     */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;
        found = FALSE;

        /*
         * Check map entry
         *
         * It is only necessary to find one entry per group
         * since a PT only has one index.
         */
        for (entry_idx = 0; (entry_idx < group->entries) && !found;
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY: /* Direct mapped field */
                fid = entry->desc.field_id;
                if (fid == BCMLRD_FIELD_INDEX) {
                    found = TRUE;
                } else if ((fid == BCMLRD_FIELD_PORT) ||
                           (fid == BCMLRD_FIELD_INSTANCE)) {
                    /* Check if source is also mapped to __INDEX */
                    if (bcmltm_nc_field_is_lt_index(unit, sid,
                                             entry->u.mapped.src.field_id)) {
                        found = TRUE;
                    }
                }
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER: /* Transform */
                if (xfrm_field_find(unit, entry, 0, BCMLRD_FIELD_INDEX)) {
                    found = TRUE;
                    fid = BCMLRD_FIELD_INDEX;
                }
                break;

            default:
                break;
            }

        }

        /* Get offset */
        if (found) {
            if (offset_count >= offset_max) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }

            SHR_IF_ERR_EXIT
                (pt_param_offset_get(unit, pt_list, ptid, fid,
                                     &offset[offset_count]));
            offset_count++;
        }
    }

    *num_offset = offset_count;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_pt_mem_param_offsets_get(int unit, bcmlrd_sid_t sid,
                                   const bcmltm_pt_list_t *pt_list,
                                   uint32_t offset_max,
                                   uint32_t *offset, uint32_t *num_offset)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid;
    uint32_t fid = 0;
    uint32_t offset_count = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find the key field(s) mapped to the PT memory parameter.
     * The memory parameters are normally mapped to these virtual
     * destination fields:
     *   __PORT
     *   __INSTANCE
     *
     * If the source field also maps to the PT Index (__INDEX),
     * this should be part of the index offset and not
     * the memomy parameter (for index copy and
     * tracking purposes).
     */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;
        found = FALSE;

        /*
         * Check map entry
         *
         * It is only necessary to find one entry per group
         * since a PT only has one index.
         */
        for (entry_idx = 0; (entry_idx < group->entries) && !found;
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY: /* Direct mapped field */
                fid = entry->desc.field_id;
                if ((fid == BCMLRD_FIELD_PORT) ||
                    (fid == BCMLRD_FIELD_INSTANCE)) {
                    /* Check if source is also mapped to __INDEX */
                    if (!bcmltm_nc_field_is_lt_index(unit, sid,
                                              entry->u.mapped.src.field_id)) {
                        found = TRUE;
                    }
                }
                break;

            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER: /* Transform */
                if (xfrm_field_find(unit, entry, 0, BCMLRD_FIELD_PORT)) {
                    found = TRUE;
                    fid = BCMLRD_FIELD_PORT;
                } else if (xfrm_field_find(unit, entry, 0,
                                           BCMLRD_FIELD_INSTANCE)) {
                    found = TRUE;
                    fid = BCMLRD_FIELD_INSTANCE;
                }
                break;

            default:
                break;
            }

        }

        /* Get offset */
        if (found) {
            if (offset_count >= offset_max) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }

            SHR_IF_ERR_EXIT
                (pt_param_offset_get(unit, pt_list, ptid, fid,
                                     &offset[offset_count]));
            offset_count++;
        }
    }

    *num_offset = offset_count;

 exit:
    SHR_FUNC_EXIT();
}

const bcmltm_wb_block_t *
bcmltm_ptm_wb_block_find(int unit, bcmlrd_sid_t sid,
                         bcmltm_table_type_t lt_type,
                         const bcmltm_map_pt_t *pt_map, bcmdrd_sid_t ptid)
{
    uint32_t i;
    const bcmltm_wb_block_t *wb_block = NULL;

    /* Find PT ID */
    for (i =  0; i < pt_map->num_pt; i++) {
        if (pt_map->info[i].sid == ptid) {
            wb_block = bcmltm_wb_lt_block_get(unit, sid,
                                              pt_map->info[i].wb_block_id);
            break;
        }
    }

    return wb_block;
}

/*!
 * \brief Destroy the given LTA XFRM fields list metadata.
 *
 * Destroy the given LTA XFRM field list metadata.
 *
 * \param [in] flist LTA field list to destroy.
 */
void
bcmltm_nc_field_list_destroy(bcmltm_lta_field_list_t *flist)
{
    if (flist == NULL) {
        return;
    }

    SHR_FREE(flist->field_maps);
    SHR_FREE(flist);

    return;
}

int
bcmltm_nc_copy_lta_field_list_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lta_field_list_t *from_list,
                                     bcmltm_lta_field_list_t *to_list,
                                     bcmltm_lta_field_list_t **copy_list_ptr)
{
    bcmltm_lta_field_list_t *copy_list = NULL;
    uint32_t copy_fields;
    unsigned int size;
    uint32_t from_ix, to_ix;
    bcmltm_lta_field_map_t *field_maps = NULL;
    bcmltm_lta_field_map_t *from_map, *to_map;
    bcmlrd_map_t const *lrd_map;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &lrd_map));

    /* Allocate */
    SHR_ALLOC(copy_list, sizeof(*copy_list),
              "LTM Logical Table lta_field_list_t");
    SHR_NULL_CHECK(copy_list, SHR_E_MEMORY);
    sal_memset(copy_list, 0, sizeof(*copy_list));

    /* Copying from one LTA field list to a different LTA field list. */

    /* Not applicable */
    copy_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    /* Fill data */
    copy_list->fields_parameter_offset =
        from_list->fields_parameter_offset;
    copy_list->target_fields_parameter_offset =
        to_list->fields_parameter_offset;
    copy_list->max_fields = to_list->max_fields;

    /* Allocate field_map for all possible to fields */
    size = sizeof(*field_maps) * copy_list->max_fields;
    SHR_ALLOC(field_maps, size,
              "LTM Logical Table Copy lta_field_map_t");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    copy_fields = 0;

    for (from_ix = 0; from_ix < from_list->num_maps; from_ix++) {
        from_map = &(from_list->field_maps[from_ix]);
        for (to_ix = 0; to_ix < to_list->num_maps; to_ix++) {
            to_map = &(to_list->field_maps[to_ix]);
            if ((from_map->api_field_id == to_map->api_field_id)
                && (from_map->field_idx == to_map->field_idx)) {
                /* Check if room for the new field in copy list. */
                if (copy_fields == copy_list->max_fields) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "LTM LTA Copy Map: current "
                                          "field count exceeds max count "
                                          "sid=%d "
                                          "count=%d max_count=%d\n"),
                               sid, copy_fields + 1, copy_list->max_fields));
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }

                if (lrd_map->field_data->field[to_map->api_field_id].flags &
                    BCMLRD_FIELD_F_READ_ONLY) {
                    /* Do not copy Read Only fields. */
                    continue;
                }

                /* Matching field value, add to copy list */
                field_maps[copy_fields].api_field_id = to_map->api_field_id;
                field_maps[copy_fields].field_idx = to_map->field_idx;
                /* These are the only two structure members used by
                 * LTA to LTA field copy.
                 */

                copy_fields++;
                /* A given field should only appear once in the list. */
                break;
           }
        }
    }

    if (copy_fields > 0) {
        copy_list->field_maps = field_maps;
        copy_list->num_maps = copy_fields;

        *copy_list_ptr = copy_list;
    } else {
        SHR_FREE(field_maps);
        bcmltm_nc_field_list_destroy(copy_list);
        *copy_list_ptr = NULL;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(field_maps);
        bcmltm_nc_field_list_destroy(copy_list);
        *copy_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


int
bcmltm_nc_map_entry_mapped_field_type_get(int unit,
                                          bcmltm_table_type_t lt_type,
                                          uint32_t fid,
                                          const bcmlrd_map_t *map,
                                          const bcmlrd_map_entry_t *entry,
                                          bcmltm_field_type_t *field_type)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check for map entry type */
    if ((entry->entry_type != BCMLRD_MAP_ENTRY_MAPPED_KEY) &&
        (entry->entry_type != BCMLRD_MAP_ENTRY_MAPPED_VALUE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Incorrect map entry type for "
                              "direct PT field map: entry_type=%d\n"),
                   entry->entry_type));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    

    /* Get field map type from map entry type */
    if (BCMLTM_TABLE_TYPE_LT_INDEX(lt_type)) {
        if (entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY) {
            *field_type = BCMLTM_FIELD_TYPE_KEY;
        } else {
            *field_type = BCMLTM_FIELD_TYPE_VALUE;
        }

    } else {
        const bcmlrd_field_data_t *field_data = NULL;

        /* Get field map type from LT field definition */
        field_data = &map->field_data->field[fid];

        /*
         * Check for special virtual fields that are not flagged
         * in the LRD as Keys but are treated as Keys by the PTM interface
         * and therefore needs to be placed in the LTM Keys field list.
         *
         * Key fields for PTM/LTM are those fields that are necessary to
         * identify a resource in an operation.
         *
         * There are different reasons why a PT Key field may not be declared
         * as Key in the LT map file.  One of the reasons, for example,
         * is that for FLTG produces 2 LT fields per Key field
         * defined in a TCAM table, which does not apply to a virtual field
         * (i.e. INSTANCE).
         */
        if (entry->desc.field_id == BCMLRD_FIELD_INSTANCE) {
            *field_type = BCMLTM_FIELD_TYPE_KEY;
        } else {
            if (field_data->flags & BCMLRD_FIELD_F_KEY) {
                *field_type = BCMLTM_FIELD_TYPE_KEY;
            } else {
                *field_type = BCMLTM_FIELD_TYPE_VALUE;
            }
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_map_pt_key_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_table_attr_t *attr,
                         bcmdrd_sid_t ptid,
                         bool *is_key)
{
    bool key_found = FALSE;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    *is_key = FALSE;

    /* Only applicable to Keyed LTs */
    if (!BCMLTM_TABLE_TYPE_LT_KEYED(attr->type)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Find PT group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        if ((group->dest.kind == BCMLRD_MAP_PHYSICAL) &&
            (group->dest.id == ptid)) {
            break;
        }
    }
    if (group_idx >= map->groups) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Unable to find PT map group "
                              "sid=%d ptid=%d\n"),
                   sid, ptid));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Find first direct field map on a Key field.
     *
     * On TCAMs with Non-Aggregate views (where the LT maps to 2 PTs),
     * a PT is declared as Key if any of its fields is a Key type.
     *
     * On TCAMs with Aggregate views and Hash (where LT maps to 1 PT),
     * the PT will likely be declared as Key because the single PT
     * contains maps to both Key and Value fields.
     */
    for (entry_idx = 0;
         (entry_idx < group->entries) && !key_found;
         entry_idx++) {
        entry = &group->entry[entry_idx];

        switch(entry->entry_type) {
        case BCMLRD_MAP_ENTRY_MAPPED_KEY:
        case BCMLRD_MAP_ENTRY_FIXED_KEY:
        case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
        case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
            key_found = TRUE;
            break;

        case BCMLRD_MAP_ENTRY_MAPPED_VALUE:
            
            SHR_IF_ERR_EXIT
                (bcmltm_nc_map_entry_mapped_field_type_get(unit,
                                                 attr->type,
                                                 entry->u.mapped.src.field_id,
                                                 map,
                                                 entry,
                                                 &field_type));
            if (field_type == BCMLTM_FIELD_TYPE_KEY) {
                key_found = TRUE;
            }
            break;

        default:
            break;
        }
    }

    if (key_found) {
        *is_key = TRUE;
    }

 exit:
    SHR_FUNC_EXIT();
}


void
bcmltm_nc_ptm_field_maps_destroy(uint32_t num_fields,
                                 bcmltm_ptm_field_map_t *field_map_list)
{
    uint32_t idx;
    bcmltm_ptm_field_map_t *field_map = field_map_list;
    bcmltm_ptm_field_map_t *next_map = NULL;

    if (field_map_list == NULL) {
        return;
    }

    for (idx = 0; idx < num_fields; idx++) {
        field_map = field_map_list[idx].next_map;
        while (field_map != NULL) {
            next_map = field_map->next_map;
            SHR_FREE(field_map);
            field_map = next_map;
        }
    }

    SHR_FREE(field_map_list);

    return;
}


void
bcmltm_nc_ptm_field_list_destroy(bcmltm_ptm_field_list_t *field_list)
{
    if (field_list == NULL) {
        return;
    }

    bcmltm_nc_ptm_field_maps_destroy(field_list->num_fields,
                                     field_list->field_maps);

    SHR_FREE(field_list);

    return;
}


void
bcmltm_nc_ptm_mapping_destroy(bcmltm_to_ptm_mapping_t *ptm_mapping)
{
    if (ptm_mapping == NULL) {
        return;
    }

    bcmltm_nc_ptm_field_list_destroy(ptm_mapping->field_list);

    SHR_FREE(ptm_mapping);

    return;
}


void
bcmltm_nc_fa_lt_index_destroy(bcmltm_fa_lt_index_t *index)
{
    if (index == NULL) {
        return;
    }

    SHR_FREE(index);

    return;
}

uint32_t
bcmltm_logical_table_count_get(void)
{
    return BCMLRD_TABLE_COUNT;
}

