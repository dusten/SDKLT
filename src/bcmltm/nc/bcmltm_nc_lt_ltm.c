/*! \file bcmltm_nc_lt_ltm.c
 *
 * Logical Table Manager - Node Cookies
 *
 * Logical Table mappings with LTM group.
 *
 * This file contains routines to construct LT information
 * for LT with LTM group.
 *
 * This information is used by the LTM metadata during
 * the FA and EE stages.
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

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lt_index_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>

#include "bcmltm_nc_lt_map.h"
#include "bcmltm_nc_lt_ltm.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* LTM Key fiels Min and Max bits (this should the Table ID) */
#define LTID_INDEX_MINBIT        0
#define LTID_INDEX_MAXBIT       31


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the FA LT Index metadata.
 *
 * Create the FA LT Index metadata for the given table.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] offset LT Index working buffer offset.
 * \param [out] index_ptr Returning pointer FA LT Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fa_lt_index_create(int unit,
                   uint32_t offset,
                   bcmltm_fa_lt_index_t **index_ptr)
{
    bcmltm_fa_lt_index_t *index = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(index, sizeof(*index),
              "LTM Logical Table fa_lt_index_t");
    SHR_NULL_CHECK(index, SHR_E_MEMORY);
    sal_memset(index, 0, sizeof(*index));

    /* Fill data */
    index->lt_index_offset = offset;

    /* These are not used */
    index->track_index_offset = BCMLTM_WB_OFFSET_INVALID;
    index->mem_param_offset = BCMLTM_WB_OFFSET_INVALID;
    index->index_multiplier = 0;
    index->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *index_ptr = index;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_fa_lt_index_destroy(index);
        *index_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the field map metadata for given map information.
 *
 * Get the field map metadata for given map information.
 *
 * \param [in] unit Unit number.
 * \param [in] map_info Field map information from LRD.
 * \param [in] offset LT Index working buffer offset.
 * \param [out] field_map Field map to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_field_map_get(int unit,
                  const bcmltm_map_field_t *map_info,
                  uint32_t offset,
                  bcmltm_ptm_field_map_t *field_map)
{
    field_map->api_field_id = map_info->fid;
    field_map->field_idx = map_info->idx;
    field_map->minimum_value = map_info->minimum_value;
    field_map->maximum_value = map_info->maximum_value;
    field_map->wbc.buffer_offset = offset;
    field_map->wbc.maxbit = LTID_INDEX_MAXBIT;
    field_map->wbc.minbit = LTID_INDEX_MINBIT;

    /* These members are not applicable for LTM based LTs */
    field_map->pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
    field_map->default_value = 0;
    field_map->next_map = NULL;

    return SHR_E_NONE;
}


/*!
 * \brief Create the list of key field maps metadata.
 *
 * Create the list of key field maps metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] map_info LRD Field mapping information list.
 * \param [in] offset LT Index working buffer offset.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_field_maps_create(int unit,
                      const bcmltm_map_entries_ltm_t *map_info,
                      uint32_t offset,
                      bcmltm_ptm_field_map_t **field_maps_ptr,
                      uint32_t *num_fields)
{
    bcmltm_ptm_field_map_t *field_maps = NULL;
    bcmltm_ptm_field_map_t *field_map = NULL;
    unsigned int size;
    uint32_t field_idx;
    uint32_t field_max_count;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of key fields */
    field_max_count = map_info->num_key_fields;

    /* Sanity check: LTM based table should only have 1 key */
    if (field_max_count != 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * field_max_count;
    SHR_ALLOC(field_maps, size,
              "LTM Logical Table ptm_field_map");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /*
     * Process field map information list
     */
    for (field_idx = 0; field_idx < field_max_count; field_idx++) {
        field_map = &field_maps[field_idx];
        SHR_IF_ERR_EXIT
            (key_field_map_get(unit,
                               &map_info->key_field[field_idx],
                               offset,
                               field_map));
    }

    *num_fields = field_idx;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_ptm_field_maps_destroy(field_max_count, field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the key field list metadata.
 *
 * Create the key field list metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] map_info LRD Field mapping information list.
 * \param [in] offset LT Index working buffer offset.
 * \param [out] field_list_ptr Returning pointer to field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_field_list_create(int unit,
                      const bcmltm_map_entries_ltm_t *map_info,
                      uint32_t offset,
                      bcmltm_ptm_field_list_t **field_list_ptr)
{
    bcmltm_ptm_field_list_t *field_list = NULL;
    uint32_t num_fields;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list),
              "LTM Logical Table ptm_field_list");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (key_field_maps_create(unit,
                               map_info, offset,
                               &field_list->field_maps,
                               &num_fields));
    field_list->num_fields = num_fields;

    /* Not applicable */
    field_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_ptm_field_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the key field mapping metadata.
 *
 * This routine creates the key fields mapping metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] map_info LRD Field mapping information list.
 * \param [in] offset LT Index working buffer offset.
 * \param [out] ptm_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_mapping_create(int unit,
                   const bcmltm_map_entries_ltm_t *map_info,
                   uint32_t offset,
                   bcmltm_to_ptm_mapping_t **ptm_mapping_ptr)
{
    bcmltm_to_ptm_mapping_t *ptm_mapping = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(ptm_mapping, sizeof(*ptm_mapping),
              "LTM Logical Table ptm_mapping");
    SHR_NULL_CHECK(ptm_mapping, SHR_E_MEMORY);
    sal_memset(ptm_mapping, 0, sizeof(*ptm_mapping));

    /* Fill data */
    SHR_IF_ERR_EXIT
        (key_field_list_create(unit,
                               map_info, offset,
                               &ptm_mapping->field_list));

    *ptm_mapping_ptr = ptm_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_ptm_mapping_destroy(ptm_mapping);
        *ptm_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the list of field spec metadata.
 *
 * Destroy the list of field spec metadata.
 *
 * \param [in] field_spec Field spec metadata to destroy.
 */
static void
field_spec_destroy(bcmltm_field_spec_t *field_spec)
{
    SHR_FREE(field_spec);

    return;
}


/*!
 * \brief Create the list of field spec metadata.
 *
 * Create the list of field spec metadata
 *
 * \param [in] unit Unit number.
 * \param [in] map LTM Group mapping information.
 * \param [out] field_spec_ptr Returning pointer to list of field specs.
 * \param [out] num_fields Number of field specs returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_spec_create(int unit,
                  const bcmltm_map_entries_ltm_t *map,
                  bcmltm_field_spec_t **field_spec_ptr,
                  uint32_t *num_fields)
{
    bcmltm_field_spec_t *field_spec = NULL;
    unsigned int size;
    uint32_t i;
    uint32_t field_max_count;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_spec_ptr = NULL;

    /* Get total number of fields */
    field_max_count = map->num_value_fields;
    if (field_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_spec) * field_max_count;
    SHR_ALLOC(field_spec, size,
              "LTM Logical Table field_spec_t");
    SHR_NULL_CHECK(field_spec, SHR_E_MEMORY);
    sal_memset(field_spec, 0, size);

    /* Fill data */
    for (i = 0; i < field_max_count ; i++) {
        field_spec[i].field_id  = map->value_field[i].fid;
        field_spec[i].field_idx = map->value_field[i].idx;
    }

    *num_fields = field_max_count;
    *field_spec_ptr = field_spec;

 exit:
    if (SHR_FUNC_ERR()) {
        field_spec_destroy(field_spec);
        *num_fields = 0;
        *field_spec_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given field specifier list metadata.
 *
 * Destroy the given field specifier list metadata.
 *
 * \param [in] flist Field specifier list to destroy.
 */
static void
field_spec_list_destroy(bcmltm_field_spec_list_t *flist)
{
    if (flist == NULL) {
        return;
    }

    field_spec_destroy(flist->field_specs);

    SHR_FREE(flist);

    return;
}


/*!
 * \brief Create the field specifier list metadata for given table.
 *
 * Create the field specifier list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] map LTM Group mapping information.
 * \param [in] offset LT Index working buffer offset.
 * \param [out] flist Returning pointer for field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_spec_list_create(int unit,
                       const bcmltm_map_entries_ltm_t *map,
                       uint32_t offset,
                       bcmltm_field_spec_list_t **flist_ptr)
{
    bcmltm_field_spec_list_t *flist = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(flist, sizeof(*flist),
              "LTM Logical Table field_spec_list_t");
    SHR_NULL_CHECK(flist, SHR_E_MEMORY);
    sal_memset(flist, 0, sizeof(*flist));

    /* Fill data */
    flist->ltid_offset = offset;

    /* Get field specifier list */
    SHR_IF_ERR_EXIT
        (field_spec_create(unit, map,
                           &flist->field_specs,
                           &flist->num_field_specs));

    *flist_ptr = flist;

 exit:
    if (SHR_FUNC_ERR()) {
        field_spec_list_destroy(flist);
        *flist_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_nc_lt_ltm_info_create(int unit, bcmlrd_sid_t sid,
                             const bcmltm_map_ltm_t *map_group,
                             bcmltm_nc_lt_ltm_info_t **info_ptr)
{
    bcmltm_nc_lt_ltm_info_t *info = NULL;
    bcmltm_map_entries_ltm_t entries;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_lt_index_offset_t lt_index_offset;
    uint32_t ltid_offset;

    SHR_FUNC_ENTER(unit);

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_map_entries_ltm_parse(unit, sid, &entries));

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "LTM LT bcmltm_nc_lt_ltm_info_t");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Add LT Index Working Buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_lt_index_add(unit, wb_handle,
                                      &info->wb_block_id,
                                      &wb_block));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_index_offset_get(wb_block,
                                       &lt_index_offset));
    ltid_offset = lt_index_offset.track;

    /* Create LT Index */
    SHR_IF_ERR_EXIT
        (fa_lt_index_create(unit, ltid_offset, &info->fa_lt_index));

    /* Create Key Field Map list */
    SHR_IF_ERR_EXIT
        (key_mapping_create(unit, &entries, ltid_offset, &info->ltm_key));

    /*
     * Create LTM group Field Spec List
     */
    SHR_IF_ERR_EXIT
        (field_spec_list_create(unit, &entries, ltid_offset, &info->fs_list));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lt_ltm_info_destroy(info);
        *info_ptr = NULL;
    }

    bcmltm_nc_lt_map_entries_ltm_cleanup(&entries);

    SHR_FUNC_EXIT();
}


void
bcmltm_nc_lt_ltm_info_destroy(bcmltm_nc_lt_ltm_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Field field spec list */
    field_spec_list_destroy(info->fs_list);

    /* Destroy Key Field Map list */
    bcmltm_nc_ptm_mapping_destroy(info->ltm_key);

    /* Destroy LT Index */
    bcmltm_nc_fa_lt_index_destroy(info->fa_lt_index);

    SHR_FREE(info);

    return;
}

