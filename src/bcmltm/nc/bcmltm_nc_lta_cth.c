/*! \file bcmltm_nc_lta_cth.c
 *
 * Logical Table Manager - Node Cookies
 *
 * Logical Table mappings with Custom Table Handler groups.
 *
 * This file contains routines to construct LT information
 * for LT with Custom Table Handler groups.
 *
 * This information is used by the LTM metadata during
 * the FA and EE stages when calling into the Logical Table Adaptor LTA
 * interface.
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
#include <bcmltm/bcmltm_wb_lta_internal.h>

#include "bcmltm_nc_lt_map.h"
#include "bcmltm_nc_lta_cth.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Destroy the given LTA CTH table params metadata.
 *
 * Destroy the given LTA CTH table params metadata.
 *
 * \param [in] params LTA table params to destroy.
 */
static void
table_params_destroy(bcmltm_lta_table_params_t *params)
{
    if (params == NULL) {
        return;
    }

    SHR_FREE(params);

    return;
}


/*!
 * \brief Create the LTA CTH table params metadata for given table.
 *
 * Create the LTA Custom Table Handler table params metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] map Custom table handler mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [out] params_ptr Returning pointer table params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_params_create(int unit,
                    const bcmltm_map_entries_cth_t *map,
                    const bcmltm_wb_block_t *wb_block,
                    bcmltm_lta_table_params_t **params_ptr)
{
    bcmltm_lta_table_params_t *params = NULL;
    bcmltm_wb_lta_fields_offset_t in_offset;
    bcmltm_wb_lta_fields_offset_t out_offset;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(*params),
              "LTM Logical Table lta_table_params_t");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(*params));

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offset_get(wb_block,
                                         BCMLTM_WB_LTA_INPUT_FIELDS,
                                         &in_offset));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offset_get(wb_block,
                                         BCMLTM_WB_LTA_OUTPUT_FIELDS,
                                         &out_offset));

    /* Fill data */
    params->handler = map->handler;
    params->fields_input_offset = in_offset.param;
    params->fields_output_offset = out_offset.param;

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        table_params_destroy(params);
        *params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the list of field maps metadata.
 *
 * Destroy the list of field maps metadata.
 *
 * \param [in] field_maps Field maps metadata to destroy.
 */
static void
field_maps_destroy(bcmltm_lta_field_map_t *field_maps)
{
    SHR_FREE(field_maps);

    return;
}


/*!
 * \brief Create the list of field maps metadata.
 *
 * Create the list of field maps metadata for either the
 * LTA input field list or the LTA output field list.
 *
 * \param [in] unit Unit number.
 * \param [in] map Custom table handler mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicate input or output field list.
 * \param [in] is_key Indicates field list should include key fields only.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_maps_create(int unit,
                  const bcmltm_map_entries_cth_t *map,
                  const bcmltm_wb_block_t *wb_block,
                  int field_type,
                  int is_key,
                  bcmltm_lta_field_map_t **field_maps_ptr,
                  uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *field_maps = NULL;
    bcmltm_map_field_t *field = NULL;
    unsigned int size;
    uint32_t i;
    uint32_t field_max_count;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of fields */
    if (is_key) {
        field_max_count = map->num_key_fields;
        field = map->key_field;
    } else {
        field_max_count = map->num_fields;
        field = map->field;
    }
    if (field_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * field_max_count;
    SHR_ALLOC(field_maps, size,
              "LTM Logical Table lta_field_map_t");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /* Fill data */
    for (i = 0; i < field_max_count ; i++) {
        /* Get field offset */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lta_field_map_offset_get(wb_block, field_type,
                                                i, &offset));

        field_maps[i].api_field_id = field[i].fid;
        field_maps[i].field_idx = field[i].idx;
        field_maps[i].minimum_value = field[i].minimum_value;
        field_maps[i].maximum_value = field[i].maximum_value;
        field_maps[i].field_offset = offset;

        /*
         * The following fields are not used by the FA node cookie
         * for LTA Custom Table Handler:
         *   field_maps[i].wbc;
         */
    }

    *num_fields = field_max_count;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        field_maps_destroy(field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given LTA CTH fields list metadata.
 *
 * Destroy the given LTA CTH field list metadata.
 *
 * \param [in] flist LTA field list to destroy.
 */
static void
field_list_destroy(bcmltm_lta_field_list_t *flist)
{
    if (flist == NULL) {
        return;
    }

    field_maps_destroy(flist->field_maps);

    SHR_FREE(flist);

    return;
}


/*!
 * \brief Create the LTA CTH field list metadata for given table.
 *
 * Create the LTA Custom Table Handler field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] map Custom table handler mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [in] is_key Indicates field list should include key fields only.
 * \param [out] flist Returning pointer for field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_list_create(int unit,
                  const bcmltm_map_entries_cth_t *map,
                  const bcmltm_wb_block_t *wb_block,
                  int field_type,
                  int is_key,
                  bcmltm_lta_field_list_t **flist_ptr)
{
    bcmltm_lta_field_list_t *flist = NULL;
    bcmltm_wb_lta_fields_offset_t offset;
    uint32_t num_fields;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(flist, sizeof(*flist),
              "LTM Logical Table lta_field_list_t");
    SHR_NULL_CHECK(flist, SHR_E_MEMORY);
    sal_memset(flist, 0, sizeof(*flist));

    /* Get fields offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offset_get(wb_block, field_type, &offset));

    /* Fill data */
    flist->fields_parameter_offset = offset.param;
    flist->fields_ptrs_offset = offset.ptrs;

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (field_maps_create(unit, map, wb_block, field_type, is_key,
                           &flist->field_maps,
                           &num_fields));

    /* All LTA values are in a single map list for CTH */
    flist->num_maps = num_fields;
    flist->max_fields = num_fields;

    /* Not applicable */
    flist->target_fields_parameter_offset = BCMLTM_WB_OFFSET_INVALID;
    flist->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *flist_ptr = flist;

 exit:
    if (SHR_FUNC_ERR()) {
        field_list_destroy(flist);
        *flist_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_nc_lta_cth_info_create(int unit, bcmlrd_sid_t sid,
                              const bcmltm_map_cth_t *map_group,
                              bcmltm_nc_lta_cth_info_t **info_ptr)
{
    bcmltm_nc_lta_cth_info_t *info = NULL;
    bcmltm_map_entries_cth_t entries;
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;

    SHR_FUNC_ENTER(unit);

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_map_entries_cth_parse(unit, sid, &entries));

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "LTM LT bcmltm_nc_lta_cth_info_t");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Add Working Buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_lta_cth_add(unit, entries.num_fields,
                                     wb_handle, &wb_block_id, &wb_block));
    info->wb_block_id = wb_block_id;

    /* Custom Table Handler interface */
    info->handler = entries.handler;

    /* Create Custom Table Handler table params (used as LTA EE node cookie) */
    SHR_IF_ERR_EXIT
        (table_params_create(unit, &entries, wb_block,
                             &info->table_params));

    /*
     * Create Custom Table Handler Field List Input/Output
     * (used as LTA FA node cookies)
     */
    SHR_IF_ERR_EXIT
        (field_list_create(unit, &entries, wb_block,
                           BCMLTM_WB_LTA_INPUT_FIELDS, 0 /* All fields */,
                           &info->fin));
    SHR_IF_ERR_EXIT
        (field_list_create(unit, &entries, wb_block,
                           BCMLTM_WB_LTA_INPUT_FIELDS, 1 /* Key fields only */,
                           &info->fin_keys));
    SHR_IF_ERR_EXIT
        (field_list_create(unit, &entries, wb_block,
                           BCMLTM_WB_LTA_OUTPUT_FIELDS, 0 /* All fields */,
                           &info->fout));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lta_cth_info_destroy(info);
        *info_ptr = NULL;
    }

    bcmltm_nc_lt_map_entries_cth_cleanup(&entries);

    SHR_FUNC_EXIT();
}


void
bcmltm_nc_lta_cth_info_destroy(bcmltm_nc_lta_cth_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy CTH Field lists */
    field_list_destroy(info->fout);
    field_list_destroy(info->fin_keys);
    field_list_destroy(info->fin);

    /* Destroy CTH table params */
    table_params_destroy(info->table_params);

    SHR_FREE(info);

    return;
}

