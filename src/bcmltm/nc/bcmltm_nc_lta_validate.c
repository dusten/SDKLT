/*! \file bcmltm_nc_lta_validate.c
 *
 * Logical Table Manager - Node Cookies.
 *
 * This file contains routines to construct LT information
 * for LT with Field Validation.
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

#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>
#include "bcmltm_nc_lt_map.h"
#include "bcmltm_nc_lta_validate.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Maximum number of field indexes in a field */
#define MAX_FIELD_IDXS            10


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Destroy the given LTA Field Validation params metadata.
 *
 * Destroy the given LTA Field Validation params metadata.
 *
 * \param [in] params LTA Field Validation params to destroy.
 */
static void
field_validate_params_destroy(bcmltm_lta_validate_params_t *params)
{
    if (params == NULL) {
        return;
    }

    if (params->lta_args != NULL) {
        SHR_FREE(params->lta_args);
    }

    SHR_FREE(params);

    return;
}

/*!
 * \brief Create the LTA Field Validation params metadata for given table.
 *
 * Create the LTA Field Validation params metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fv Field validation handler.
 * \param [in] wb_block Working buffer block for LTA field validation.
 * \param [out] params_ptr Returning pointer of Field params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_validate_params_create(int unit, bcmlrd_sid_t sid,
                             bcmltd_field_val_handler_t *fv,
                             const bcmltm_wb_block_t *wb_block,
                             bcmltm_lta_validate_params_t **params_ptr)
{
    bcmltm_lta_validate_params_t *params = NULL;
    uint32_t size;
    bcmltm_wb_lta_fields_offset_t offset;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(bcmltm_lta_validate_params_t),
              "LTM Logical Table lta_validate_params_t");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(bcmltm_lta_validate_params_t));

    size = sizeof(bcmltd_field_val_arg_t);
    SHR_ALLOC(params->lta_args, size, "LTM Logical Table lta_args");
    SHR_NULL_CHECK(params->lta_args, SHR_E_MEMORY);
    sal_memset(params->lta_args, 0, size);

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offset_get(wb_block,
                                         BCMLTM_WB_LTA_INPUT_FIELDS,
                                         &offset));

    /* Fill data */
    params->lta_validate = fv->validate;
    params->fields_input_offset = offset.param;
    sal_memcpy(params->lta_args, fv->arg, sizeof(*params->lta_args));

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        field_validate_params_destroy(params);
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
 * \brief Create the list of Field Validation source field maps metadata.
 *
 * Create the list of field maps metadata for the LTA Field Validation
 * SOURCE fields for given LT.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] total_indices Total indices of Field with Field Valiation.
 * \param [in] fv Field validation handler.
 * \param [in] wb_block Working buffer block for LTA field validation.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_validate_maps_create(int unit,
                           bcmlrd_sid_t sid,
                           uint32_t total_indices,
                           const bcmltd_field_val_handler_t *fv,
                           const bcmltm_wb_block_t *wb_block,
                           bcmltm_lta_field_map_t **field_maps_ptr,
                           uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *lta_field_maps = NULL;
    uint32_t lfid, idx, fidx, lidx, idx_count;
    unsigned int size;
    uint32_t offset;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Allocate field_map */
    size = sizeof(*lta_field_maps) * total_indices;
    SHR_ALLOC(lta_field_maps, size,
              "LTM Logical Table Source lta_field_maps_t");
    SHR_NULL_CHECK(lta_field_maps, SHR_E_MEMORY);
    sal_memset(lta_field_maps, 0, size);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                "LTM Field Validation Field Map: sid=%d, count=%d\n"),
                sid, total_indices));

    for (lidx = 0, fidx = 0; fidx < fv->arg->fields; fidx++) {
        lfid = fv->arg->field[fidx];

        idx_count = bcmlrd_field_idx_count_get(unit, sid, lfid);
        if (idx_count == 0) {
            continue;    /* Skip unmapped fields */
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
            /* Get field offset */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(wb_block,
                                                    BCMLTM_WB_LTA_INPUT_FIELDS,
                                                    lidx, &offset));
            lta_field_maps[lidx].api_field_id = lfid;
            lta_field_maps[lidx].field_idx = idx;
            lta_field_maps[lidx].field_offset = offset;
            lta_field_maps[lidx].minimum_value = minimum_value[idx];
            lta_field_maps[lidx].maximum_value = maximum_value[idx];
            lidx++;
            (*num_fields)++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
    }

    *field_maps_ptr = lta_field_maps;

exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);

    if (SHR_FUNC_ERR()) {
        field_maps_destroy(lta_field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given LTA Field list metadata.
 *
 * Destroy the given LTA Field list metadata.
 *
 * \param [in] flist LTA Field list to destroy.
 */
static void
field_validate_list_destroy(bcmltm_lta_field_list_t *flist)
{
    if (flist == NULL) {
        return;
    }

    field_maps_destroy(flist->field_maps);

    SHR_FREE(flist);

    return;
}

/*!
 * \brief Create the LTA Field Validate list metadata for given table.
 *
 * Create the LTA Validate Field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] total_indices Total number of field elements.
 * \param [in] fv Field validation handler.
 * \param [in] wb_block Working buffer block for LTA field validation.
 * \param [out] flist_ptr Returning pointer to list of field maps.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_validate_list_create(int unit,
                           bcmlrd_sid_t sid,
                           uint32_t total_indices,
                           bcmltd_field_val_handler_t *fv,
                           const bcmltm_wb_block_t *wb_block,
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
        (bcmltm_wb_lta_fields_offset_get(wb_block,
                                         BCMLTM_WB_LTA_INPUT_FIELDS,
                                         &offset));

    /* Fill data */
    flist->fields_parameter_offset = offset.param;
    flist->fields_ptrs_offset = offset.ptrs;

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (field_validate_maps_create(unit, sid, total_indices,
                                    fv, wb_block,
                                    &flist->field_maps, &num_fields));
    flist->max_fields = num_fields;
    flist->num_maps = num_fields;

    /* Not applicable */
    flist->target_fields_parameter_offset = BCMLTM_WB_OFFSET_INVALID;
    flist->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *flist_ptr = flist;

 exit:
    if (SHR_FUNC_ERR()) {
        field_validate_list_destroy(flist);
        *flist_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmltm_nc_lta_validate_info_create(int unit, bcmlrd_sid_t sid,
                                   bcmltm_map_entries_fv_t *entries,
                                   bcmltm_field_type_t field_type,
                                   bcmltm_nc_lta_validate_info_list_t
                                   **list_ptr)
{
    bcmltm_nc_lta_validate_info_list_t  *list = NULL;
    bcmltm_map_fv_list_t *fv_list = NULL;
    bcmltd_field_val_handler_t *fv = NULL;
    bcmltm_nc_lta_validate_info_t info;
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    uint32_t num_validate = 0;
    uint32_t size = 0;
    uint32_t idx = 0;
    uint32_t fidx = 0;

    SHR_FUNC_ENTER(unit);

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        fv_list = &entries->validate_keys;
    } else {
        fv_list = &entries->validate_values;
    }

    /* Get Working Buffer handle */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    if (wb_handle == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM null working buffer handle: "
                              "sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    num_validate = fv_list->num;

    /* Allocate */
    SHR_ALLOC(list , sizeof(*list),
              "LTM LT bcmltm_nc_lta_validate_info_list_t");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    if (num_validate == 0) {
        list->num = 0;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    size = sizeof(bcmltm_nc_lta_validate_info_t) * num_validate;
    SHR_ALLOC(list->validate_info , size,
              "LTM LT bcmltm_nc_lta_validate_info_t");
    SHR_NULL_CHECK(list->validate_info, SHR_E_MEMORY);
    sal_memset(list->validate_info, 0, size);

    for (idx = 0; idx < num_validate; idx++) {
        uint32_t lfid = 0;
        uint32_t count = 0;
        uint32_t num_fields = 0;

        sal_memset(&info, 0, sizeof(bcmltm_nc_lta_validate_info_t));

        fv = &fv_list->fv[idx];
        for (fidx = 0; fidx < fv->arg->fields; fidx++) {
            lfid = fv->arg->field[fidx];
            count = bcmlrd_field_idx_count_get(unit, sid, lfid);
            num_fields += count;
        }
        info.total_indices = num_fields;

        /* Add Working Buffer block */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_block_lta_val_add(unit,
                                         info.total_indices,
                                         wb_handle, &wb_block_id, &wb_block));
        info.wb_block_id = wb_block_id;

        /* Create Field Validation params */
        SHR_IF_ERR_EXIT
            (field_validate_params_create(unit, sid, fv, wb_block,
                                          &info.validate_params));

        /* Create Field Validation List Source */
        SHR_IF_ERR_EXIT
            (field_validate_list_create(unit, sid, info.total_indices,
                                        fv, wb_block, &info.fsrc));

        sal_memcpy(&list->validate_info[idx], &info,
                   sizeof(bcmltm_nc_lta_validate_info_t));
    }

    list->num = num_validate;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lta_validate_info_destroy(list);
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_validate_copy_info_create(int unit, bcmlrd_sid_t sid,
                            bcmltm_nc_lta_xfrm_info_list_t *from_list,
                            bcmltm_nc_lta_validate_info_list_t *to_list,
                            bcmltm_nc_lta_validate_info_list_t **list_ptr)
{
    bcmltm_nc_lta_validate_info_list_t *list = NULL;
    uint32_t to_num_validate = 0;
    uint32_t from_num_xfrm = 0;
    uint32_t from_xfrm_idx;
    uint32_t to_validate_idx;
    uint32_t size = 0;
    bcmltm_nc_lta_validate_info_t info;

    SHR_FUNC_ENTER(unit);

    to_num_validate = to_list->num;
    from_num_xfrm = from_list->num;

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list),
              "LTM LT bcmltm_nc_lta_validate_info_list_t");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));
    if ((to_num_validate * from_num_xfrm) == 0) {
        list->num = 0;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    size = sizeof(bcmltm_nc_lta_validate_info_t) *
        to_num_validate * from_num_xfrm;
    SHR_ALLOC(list->validate_info , size,
              "LTM LT bcmltm_nc_lta_validate_info_t");
    SHR_NULL_CHECK(list->validate_info, SHR_E_MEMORY);
    sal_memset(list->validate_info, 0, size);

    sal_memset(&info, 0, sizeof(info));

    /* Allocate */
    for (to_validate_idx = 0;
         to_validate_idx < to_num_validate;
         to_validate_idx++) {
        for (from_xfrm_idx = 0; from_xfrm_idx < from_num_xfrm;
             from_xfrm_idx++) {
            SHR_IF_ERR_EXIT
                (bcmltm_nc_copy_lta_field_list_create(unit, sid,
                           from_list->xfrm_info[from_xfrm_idx].fout,
                           to_list->validate_info[to_validate_idx].fsrc,
                                                      &(info.fsrc)));

            sal_memcpy(&(list->validate_info[(to_validate_idx *
                                              from_num_xfrm) +
                                             from_xfrm_idx]),
                       &info, sizeof(info));
        }
    }

    list->num = to_num_validate * from_num_xfrm;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lta_validate_info_destroy(list);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_nc_lta_validate_info_destroy(bcmltm_nc_lta_validate_info_list_t *list)
{
    bcmltm_nc_lta_validate_info_t *info = NULL;
    uint32_t idx;

    if (list == NULL) {
        return;
    }

    if (list->validate_info != NULL) {
        for (idx = 0; idx < list->num; ++idx) {
            info = &list->validate_info[idx];

            /* Destroy Field Validation lists */
            field_validate_list_destroy(info->fsrc);

            /* Destroy Field Validation params */
            field_validate_params_destroy(info->validate_params);
        }

        SHR_FREE(list->validate_info);
    }

    SHR_FREE(list);
    list = NULL;

    return;
}

