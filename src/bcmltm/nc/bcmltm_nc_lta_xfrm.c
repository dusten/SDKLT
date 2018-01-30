/*! \file bcmltm_nc_lta_xfrm.c
 *
 * Logical Table Manager - Node Cookies.
 *
 * Logical Table mappings with Field Transform groups.
 *
 * This file contains routines to construct LT information
 * for LT with Field Transform group.
 *
 * This information is used by the LTM metadata during
 * the FA and EE stages when calling into the Logical Table Adaptor LTA
 * interface.
 *
 * Transform fields are declared in the LRD map file along with its
 * type, whether SOURCE or DESTINATION:
 * - SOURCE fields:
 *   These fields are those declared as "transform_src" in the LT map file.
 *   For the most part, these are the logical table fields.
 *
 * - DESTINATION fields:
 *   These fields are those declared as "transform_dst" in the LT map file.
 *   For the most part, these are the physical table fields.
 *
 * Each LT transform consists of a set of transforms:
 * - FORWARD Transform:
 *   This takes information from a set of LT logical fields
 *   and, for the most part, produces the Physical fields and
 *   corresponding information to be written to PTM.
 * - REVERSE Transform:
 *   This takes a set of Physical fields and corresponding information
 *   obtained from PTM and produces a set of LT logical fields.
 *
 * The source and destination fields are used as input or output depending
 * on the transform function:
 * - FORWARD Transform:  INPUT  = SOURCE fields
 *                       OUTPUT = DESTINATION fields
 * - REVERSE Transform:  INPUT  = DESTINATION fields
 *                       OUTPUT = SOURCE fields
 *
 *   +---------+----------------------+----------------------+
 *   |         | Forward              | Reverse              |
 *   +---------+----------------------+----------------------+
 *   | INPUT   | arg->field(Logical)  | arg->field(Physical) |
 *   +---------+----------------------+----------------------+
 *   | OUTPUT  | arg->rfield(Physical)| arg->rfield(Logical) |
 *   +---------+----------------------+----------------------+
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
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>
#include "bcmltm_nc_lt_map.h"
#include "bcmltm_nc_lta_xfrm.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Maximum number of field indexes in a field */
#define MAX_FIELD_IDXS           50


/*******************************************************************************
 * Private functions
 */

#define XFRM_SRC_FCOUNT(ptr, type) \
    ((type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) || \
     (type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM)) ? ptr->fields : ptr->rfields
#define XFRM_DST_FCOUNT(ptr, type) \
    ((type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) || \
     (type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM)) ? ptr->rfields : ptr->fields

#define XFRM_SRC_FID(ptr, type, idx) \
    ((type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) || \
     (type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM)) ? ptr->field[idx] : ptr->rfield[idx]

/*!
 * \brief Get the number of source fields for the given LT transform.
 *
 * For given logical table transform, it returns the number of
 * source fields count as follows:
 *   fid_count      - This is the number of logical fields IDs
 *                    and does not include the indexes.
 *   num_fields     - This is the number of the unique field
 *                    elements.  Each element is identified by
 *                    its field ID and field index (fid,idx).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm_arg Transform handler argument.
 * \param [out] fid_count Returning number of src field IDs (fid).
 * \param [out] num_fields Returning number of src field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
src_fields_count_get(int unit, bcmlrd_sid_t sid, int xfrm_type,
                     const bcmltd_transform_arg_t *xfrm_arg,
                     uint32_t *fid_count, uint32_t *num_fields)
{
    uint32_t i;
    uint32_t idx_count;
    int fid;

    *fid_count = *num_fields = 0;
    *fid_count = XFRM_SRC_FCOUNT(xfrm_arg, xfrm_type);

    for (i = 0; i < *fid_count; i++) {
        fid = XFRM_SRC_FID(xfrm_arg, xfrm_type, i);
        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        *num_fields += idx_count;
    }

    return SHR_E_NONE;
}


/*!
 * \brief Get the number of destination fields for the given LT transform.
 *
 * For given logical table transform, it returns the number of
 * destination fields count as follows:
 *   fid_count      - This is the number of physical fields IDs
 *                    and does not include the indexes.
 *   num_fields     - This is the number of the unique field
 *                    elements.  Each element is identified by
 *                    its field ID and field index (fid,idx).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm_arg Transform handler argument.
 * \param [in] xfrm_dst Transform destination fields.
 * \param [out] fid_count Returning number of dst field IDs (fid).
 * \param [out] num_fields Returning number of dst field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dst_fields_count_get(int unit, bcmlrd_sid_t sid, int xfrm_type,
                     const bcmltd_transform_arg_t *xfrm_arg,
                     const bcmltm_xfrm_dst_fields_t *xfrm_dst,
                     uint32_t *fid_count, uint32_t *num_fields)
{
    uint32_t i;
    uint32_t idx_count;
    uint32_t width;

    SHR_FUNC_ENTER(unit);

    *fid_count = *num_fields = 0;
    *fid_count = XFRM_DST_FCOUNT(xfrm_arg, xfrm_type);

    /* Sanity check */
    if (*fid_count != xfrm_dst->num) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM Transform destination fields: "
                              "field count mistmach "
                              "lrd_xfrm=%d ltm_xfrm=%d\n"),
                   *fid_count, xfrm_dst->num));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < *fid_count; i++) {
        width = xfrm_dst->field[i].wbc.maxbit -
                xfrm_dst->field[i].wbc.minbit + 1;
        idx_count = bcmltd_field_idx_count_get(width);
        *num_fields += idx_count;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given LTA XFRM table params metadata.
 *
 * Destroy the given LTA XFRM table params metadata.
 *
 * \param [in] params LTA table params to destroy.
 */
static void
transform_params_destroy(bcmltm_lta_transform_params_t *params)
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
 * \brief Create the LTA XFRM table params metadata for given table.
 *
 * Create the LTA Field Table Handler table params metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] xfrm Transform handler
 * \param [in] xfrm_type Transform or Reverse Transform
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [out] params_ptr Returning pointer table params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
transform_params_create(int unit,
                        bcmltd_xfrm_handler_t *xfrm,
                        int xfrm_type,
                        const bcmltm_wb_block_t *lta_wb_block,
                        const bcmltm_wb_block_t *ptm_wb_block,
                        bcmltm_lta_transform_params_t **params_ptr)
{
    bcmltm_lta_transform_params_t *params = NULL;
    bcmltm_wb_lta_fields_offset_t in_offset;
    bcmltm_wb_lta_fields_offset_t out_offset;
    uint32_t pt_sid_override_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t pt_sid_selector_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(params, sizeof(bcmltm_lta_transform_params_t),
              "LTM Logical Table lta_transform_params_t");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(bcmltm_lta_transform_params_t));

    
    SHR_ALLOC(params->lta_args, sizeof(bcmltd_transform_arg_t),
              "LTM Logical Table bcmltd_transform_arg_t");
    SHR_NULL_CHECK(params->lta_args, SHR_E_MEMORY);
    sal_memset(params->lta_args, 0, sizeof(bcmltd_transform_arg_t));

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offset_get(lta_wb_block,
                                         BCMLTM_WB_LTA_INPUT_FIELDS,
                                         &in_offset));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lta_fields_offset_get(lta_wb_block,
                                         BCMLTM_WB_LTA_OUTPUT_FIELDS,
                                         &out_offset));

    /* Fill data */
    params->lta_transform = xfrm->transform;
    params->fields_input_offset = in_offset.param;
    params->fields_output_offset = out_offset.param;
    /* Get offset from PTM WB */
    if (xfrm->arg->tables != 0) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_pt_sid_override_offset_get(ptm_wb_block,
                                                      &pt_sid_override_offset));
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                               BCMLRD_FIELD_TABLE_SEL,
                                               BCMLTM_FIELD_TYPE_KEY,
                                               BCMLTM_FIELD_DIR_IN,
                                               0,
                                               &pt_sid_selector_offset));
    }
    params->pt_sid_selector_offset = pt_sid_selector_offset;
    params->pt_sid_override_offset = pt_sid_override_offset;
    sal_memcpy(params->lta_args, xfrm->arg, sizeof(*params->lta_args));

    
    params->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        transform_params_destroy(params);
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
 * \brief Create the list of transform source field maps metadata.
 *
 * Create the list of field maps metadata for the LTA Transform
 * SOURCE fields for given LT and transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform type.
 * \param [in] field_type LTA field type.
 * \param [in] xfrm Transform handler.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_src_field_maps_create(int unit,
                           bcmlrd_sid_t sid, int xfrm_type,
                           int field_type,
                           const bcmltd_xfrm_handler_t *xfrm,
                           const bcmltm_wb_block_t *lta_wb_block,
                           bcmltm_lta_field_map_t **field_maps_ptr,
                           uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *field_maps = NULL;
    unsigned int size;
    uint32_t map_idx;
    uint32_t xfrm_idx;
    uint32_t fid;           /* Source (logical) field ID */
    uint32_t fid_count;     /* Number of source field IDs */
    uint32_t total_fields;  /* Total source fields elements (fid, idx) */
    const bcmltd_transform_arg_t *xfrm_arg = xfrm->arg;
    uint32_t idx;
    uint32_t idx_count;
    uint32_t offset;
    uint64_t *default_value = NULL;
    uint32_t default_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    SHR_IF_ERR_EXIT
        (src_fields_count_get(unit, sid, xfrm_type, xfrm_arg,
                              &fid_count, &total_fields));

    /* Allocate field_map */
    size = sizeof(*field_maps) * total_fields;
    SHR_ALLOC(field_maps, size,
              "LTM Logical Table Source lta_field_map_t");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /* Fill data */
    for (map_idx = 0, xfrm_idx = 0; xfrm_idx < fid_count; xfrm_idx++) {
        fid = XFRM_SRC_FID(xfrm_arg, xfrm_type, xfrm_idx);

        /* Get number of field indices */
        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        if (idx_count == 0) {
            continue;    /* Skip unmapped fields */
        }

        /* Get SW default value for field */
        SHR_ALLOC(default_value, sizeof(*default_value) * idx_count,
                  "LTM field default values");
        SHR_NULL_CHECK(default_value, SHR_E_MEMORY);
        sal_memset(default_value, 0, sizeof(*default_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, sid, fid,
                                      idx_count,
                                      default_value,
                                      &default_count));
        if (idx_count != default_count) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field defaults: "
                                  "sid=%d fid=%d "
                                  "expected=%d actual=%d\n"),
                       sid, fid, idx_count, default_count));
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
            if (map_idx >= total_fields) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Transform Field Map: current "
                                      "src field count exceeds max count "
                                      "sid=%d "
                                      "count=%d max_count=%d\n"),
                           sid, (map_idx+1), total_fields));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }

            /* Get field offset */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(lta_wb_block, field_type,
                                                    map_idx, &offset));

            field_maps[map_idx].api_field_id = fid;
            field_maps[map_idx].field_idx = idx;
            field_maps[map_idx].field_offset = offset;
            field_maps[map_idx].default_value = default_value[idx];
            field_maps[map_idx].minimum_value = minimum_value[idx];
            field_maps[map_idx].maximum_value = maximum_value[idx];

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "LTM Transform Source Field Map: "
                                    "sid=%d fid=%d idx=%d offset=%d\n"),
                         sid,
                         field_maps[map_idx].api_field_id,
                         field_maps[map_idx].field_idx,
                         field_maps[map_idx].field_offset));
            map_idx++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
        SHR_FREE(default_value);
    }

    *num_fields = total_fields;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        field_maps_destroy(field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);
    SHR_FREE(default_value);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the list of transform destination field maps metadata.
 *
 * Create the list of field maps metadata for the LTA Transform
 * DESTINATION fields for given LT and transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] xfrm_type Transform type.
 * \param [in] field_type LTA field type.
 * \param [in] xfrm Transform handler.
 * \param [in] xfrm_dst Transform destination fields.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_dst_field_maps_create(int unit,
                           bcmlrd_sid_t sid,
                           const bcmltm_table_attr_t *attr,
                           int xfrm_type,
                           int field_type,
                           const bcmltd_xfrm_handler_t *xfrm,
                           const bcmltm_xfrm_dst_fields_t *xfrm_dst,
                           const bcmltm_wb_block_t *lta_wb_block,
                           const bcmltm_wb_block_t *ptm_wb_block,
                           bcmltm_lta_field_map_t **field_maps_ptr,
                           uint32_t *num_fields)
{
    bcmltm_lta_field_map_t *field_maps = NULL;
    unsigned int size;
    bcmdrd_sid_t ptid;
    bcmltm_xfrm_dst_field_t *dst_field;
    uint32_t map_idx;
    uint32_t dst_idx;
    uint32_t fid;           /* Destination (physical) field ID */
    uint32_t fid_count;     /* Number of dst field IDs */
    uint32_t total_fields;  /* Total dst fields elements (fid, idx) */
    const bcmltd_transform_arg_t *xfrm_arg = xfrm->arg;
    const bcmlrd_map_t *map = NULL;
    uint32_t idx;
    uint32_t idx_count;
    uint16_t fidx_minbit[MAX_FIELD_IDXS];
    uint16_t fidx_maxbit[MAX_FIELD_IDXS];
    uint32_t offset;
    uint32_t pt_changed_offset;
    bcmltm_field_type_t lt_field_type;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Determine LT field type: Key or Value */
    if ((xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
        (xfrm_type == BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM)) {
        lt_field_type = BCMLTM_FIELD_TYPE_KEY;
    } else {
        lt_field_type = BCMLTM_FIELD_TYPE_VALUE;
    }

    SHR_IF_ERR_EXIT
        (dst_fields_count_get(unit, sid, xfrm_type, xfrm_arg, xfrm_dst,
                              &fid_count, &total_fields));

    /* Allocate field_map */
    size = sizeof(*field_maps) * total_fields;
    SHR_ALLOC(field_maps, size,
              "LTM Logical Table Destination lta_field_map_t");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Get PT changed offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_pt_changed_offset_get(attr->type,
                                             lt_field_type,
                                             ptm_wb_block,
                                             &pt_changed_offset));

    /* Fill data */
    for (map_idx = 0, dst_idx = 0; dst_idx < xfrm_dst->num; dst_idx++) {
        ptid = xfrm_dst->ptid;
        dst_field = &xfrm_dst->field[dst_idx];
        fid = dst_field->field_id;

        SHR_IF_ERR_EXIT
            (bcmltm_nc_field_to_fidx_min_max(unit, fid,
                                             dst_field->wbc.minbit,
                                             dst_field->wbc.maxbit,
                                             MAX_FIELD_IDXS,
                                             fidx_minbit, fidx_maxbit,
                                             &idx_count));

        /* All fields has at least 1 index count */
        if (idx_count < 1) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM Transform Destination Field Map: "
                                  "invalid field index count (should be > 1) "
                                  "sid=%d ptid=%d fid=%d count=%d\n"),
                       sid, ptid, fid, idx_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Fill data for each field index element */
        for (idx = 0; idx < idx_count; idx++) {
            if (map_idx >= total_fields) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Transform Destination Field Map: "
                                      "current field count exceeds max count "
                                      "sid=%d "
                                      "count=%d max_count=%d\n"),
                           sid, (map_idx+1), total_fields));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }

            /* Get field offset */
            SHR_IF_ERR_EXIT
                (bcmltm_wb_lta_field_map_offset_get(lta_wb_block,
                                                    field_type,
                                                    map_idx, &offset));

            field_maps[map_idx].api_field_id = fid;
            field_maps[map_idx].field_idx = idx;
            field_maps[map_idx].field_offset = offset;

            field_maps[map_idx].pt_changed_offset = pt_changed_offset;

            field_maps[map_idx].wbc.buffer_offset =
                dst_field->wbc.buffer_offset;
            field_maps[map_idx].wbc.maxbit = fidx_maxbit[idx];
            field_maps[map_idx].wbc.minbit = fidx_minbit[idx];

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "LTM Transform Destination Field Map: "
                                    "sid=%d ptid=%d fid=%d idx=%d offset=%d "
                                    "wbc(buf, max, min)=(%d, %d, %d)\n"),
                         sid, ptid,
                         field_maps[map_idx].api_field_id,
                         field_maps[map_idx].field_idx,
                         field_maps[map_idx].field_offset,
                         field_maps[map_idx].wbc.buffer_offset,
                         field_maps[map_idx].wbc.maxbit,
                         field_maps[map_idx].wbc.minbit));

            map_idx++;
        }
    }

    *num_fields = total_fields;
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
 * \brief Create the list of field maps metadata.
 *
 * Create the list of field maps metadata for either the
 * LTA Source or Destination field list for given LT and transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm Transform handler.
 * \param [in] xfrm_dst Transform destination fields.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps (fid,idx) returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_field_maps_create(int unit,
                       bcmlrd_sid_t sid,
                       const bcmltm_table_attr_t *attr,
                       int xfrm_type,
                       const bcmltd_xfrm_handler_t *xfrm,
                       const bcmltm_xfrm_dst_fields_t *xfrm_dst,
                       const bcmltm_wb_block_t *lta_wb_block,
                       const bcmltm_wb_block_t *ptm_wb_block,
                       int field_type,
                       bcmltm_lta_field_map_t **field_maps_ptr,
                       uint32_t *num_fields)
{
#define IS_FTYPE_IN(ftype)  (ftype == BCMLTM_WB_LTA_INPUT_FIELDS)
#define IS_XTYPE_FWD(xtype) ((xtype == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) || \
                             (xtype == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM))

    if ((IS_FTYPE_IN(field_type) && IS_XTYPE_FWD(xfrm_type)) ||
        (!IS_FTYPE_IN(field_type) && !IS_XTYPE_FWD(xfrm_type))) {
        return xfrm_src_field_maps_create(unit, sid, xfrm_type, field_type, xfrm,
                                          lta_wb_block, field_maps_ptr,
                                          num_fields);
    } else {
        return xfrm_dst_field_maps_create(unit, sid, attr,
                                          xfrm_type, field_type, xfrm, xfrm_dst,
                                          lta_wb_block, ptm_wb_block,
                                          field_maps_ptr,
                                          num_fields);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Create the LTA XFRM field list metadata for given table.
 *
 * Create the LTA Custom Table Handler field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm Transform handler.
 * \param [in] xfrm_dst Transform destination fields.
 * \param [in] lta_wb_block LTA transform working buffer block.
 * \param [in] ptm_wb_block PTM working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [out] flist Returning pointer for field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_field_list_create(int unit,
                       bcmlrd_sid_t sid,
                       const bcmltm_table_attr_t *attr,
                       int xfrm_type,
                       const bcmltd_xfrm_handler_t *xfrm,
                       const bcmltm_xfrm_dst_fields_t *xfrm_dst,
                       const bcmltm_wb_block_t *lta_wb_block,
                       const bcmltm_wb_block_t *ptm_wb_block,
                       int field_type,
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
        (bcmltm_wb_lta_fields_offset_get(lta_wb_block, field_type, &offset));

    /* Fill data */
    flist->fields_parameter_offset = offset.param;
    flist->fields_ptrs_offset = offset.ptrs;

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (xfrm_field_maps_create(unit, sid, attr, xfrm_type, xfrm, xfrm_dst,
                                lta_wb_block, ptm_wb_block, field_type,
                                &flist->field_maps, &num_fields));
    flist->max_fields = num_fields;
    flist->num_maps = num_fields;

    
    flist->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *flist_ptr = flist;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_field_list_destroy(flist);
        *flist_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_nc_lta_xfrm_info_create(int unit, bcmlrd_sid_t sid,
                               const bcmltm_table_attr_t *attr,
                               int xfrm_type,
                               bcmltm_map_entries_pt_t *entries,
                               const bcmltm_map_pt_t *pt_map_group,
                               bcmltm_nc_lta_xfrm_info_list_t **list_ptr)
{
    bcmltm_nc_lta_xfrm_info_list_t *list = NULL;
    bcmltm_map_field_xfrm_list_t *xfrm_list;
    uint32_t num_xfrm = 0;
    uint32_t size = 0;
    bcmltm_nc_lta_xfrm_info_t info;
    bcmltd_xfrm_handler_t *xfrm = NULL;
    bcmltm_xfrm_dst_fields_t *xfrm_dst = NULL;
    uint32_t idx = 0;
    uint32_t src_fid_count;
    uint32_t dst_fid_count;
    uint32_t src_total_fields;  /* Total fields elements (fid, idx) */
    uint32_t dst_total_fields;  /* Total fields elements (fid, idx) */
    bcmltm_wb_block_id_t wb_block_id;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    uint32_t in_total_fields;  /* Total fields elements (fid, idx) */
    uint32_t out_total_fields;  /* Total fields elements (fid, idx) */
    bcmdrd_sid_t ptid;
    const bcmltm_wb_block_t *ptm_wb_block;

    SHR_FUNC_ENTER(unit);

    if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        xfrm_list = &entries->fwd_key_xfrms;
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        xfrm_list = &entries->fwd_value_xfrms;
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM) {
        xfrm_list = &entries->rev_key_xfrms;
    } else {
        xfrm_list = &entries->rev_value_xfrms;
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

    num_xfrm = xfrm_list->num;
    /* Allocate */
    SHR_ALLOC(list , sizeof(*list), "LTM LT bcmltm_nc_lta_xfrm_info_list_t");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));
    if (num_xfrm == 0) {
        list->num = num_xfrm;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    size = sizeof(bcmltm_nc_lta_xfrm_info_t) * num_xfrm;
    SHR_ALLOC(list->xfrm_info , size, "LTM LT bcmltm_nc_lta_xfrm_info_list_t");
    SHR_NULL_CHECK(list->xfrm_info, SHR_E_MEMORY);
    sal_memset(list->xfrm_info, 0, size);

    /* Allocate */
    for (idx = 0; idx < num_xfrm; idx++) {
        /* Obtain tranform arg pointer */
        if (&xfrm_list->xfrm[idx] == NULL) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "LTM metadata : "
                      "xfrm handler is not defined for sid=%d idx=%d\n"),
                       sid, idx));
        }
        xfrm = &xfrm_list->xfrm[idx];
        xfrm_dst = &xfrm_list->xfrm_dst[idx];

        sal_memset(&info, 0, sizeof(bcmltm_nc_lta_xfrm_info_t));

        SHR_IF_ERR_EXIT
            (src_fields_count_get(unit, sid, xfrm_type, xfrm->arg,
                                  &src_fid_count, &src_total_fields));

        SHR_IF_ERR_EXIT
            (dst_fields_count_get(unit, sid, xfrm_type, xfrm->arg, xfrm_dst,
                                  &dst_fid_count, &dst_total_fields));

        /*
         * Forward Transform:  Input are Source fields
         *                     Output are Destination fields
         *
         * Reverse Transform:  Input are Destination fields
         *                     Output are Source fields
         */
        if ((xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
            (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
            in_total_fields = src_total_fields;
            out_total_fields = dst_total_fields;
        } else {
            in_total_fields = dst_total_fields;
            out_total_fields = src_total_fields;
        }
        /* Add Working Buffer block */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_block_lta_xfrm_add(unit,
                                          in_total_fields, out_total_fields,
                                          wb_handle, &wb_block_id, &wb_block));
        info.wb_block_id = wb_block_id;

        ptid = xfrm_dst->ptid;

        ptm_wb_block = bcmltm_ptm_wb_block_find(unit, sid, attr->type,
                                                pt_map_group, ptid);

        /*
         * Create Field Transform Handler table params
         * (used as LTA FA node cookies for calling transform).
         */
        SHR_IF_ERR_EXIT
            (transform_params_create(unit, xfrm,
                                     xfrm_type, wb_block, ptm_wb_block,
                                     &info.xfrm_params));

        /*
         * Create Field Transform Handler Field List Input/Output
         * (used as LTA FA node cookies).
         */
        SHR_IF_ERR_EXIT
            (xfrm_field_list_create(unit, sid, attr, xfrm_type, xfrm, xfrm_dst,
                                    wb_block, ptm_wb_block,
                                    BCMLTM_WB_LTA_INPUT_FIELDS, &info.fin));
        SHR_IF_ERR_EXIT
            (xfrm_field_list_create(unit, sid, attr, xfrm_type, xfrm, xfrm_dst,
                                    wb_block, ptm_wb_block,
                                    BCMLTM_WB_LTA_OUTPUT_FIELDS, &info.fout));

        sal_memcpy(&list->xfrm_info[idx], &info,
                   sizeof(bcmltm_nc_lta_xfrm_info_t));
    }

    list->num = num_xfrm;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lta_xfrm_info_destroy(list);
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_xfrm_copy_info_create(int unit, bcmlrd_sid_t sid,
                                    bcmltm_nc_lta_xfrm_info_list_t *from_list,
                                    bcmltm_nc_lta_xfrm_info_list_t *to_list,
                                    bcmltm_nc_lta_xfrm_info_list_t **list_ptr)
{
    bcmltm_nc_lta_xfrm_info_list_t *list = NULL;
    uint32_t to_num_xfrm = 0;
    uint32_t from_num_xfrm = 0;
    uint32_t from_xfrm_idx;
    uint32_t to_xfrm_idx;
    uint32_t size = 0;
    bcmltm_nc_lta_xfrm_info_t info;

    SHR_FUNC_ENTER(unit);

    to_num_xfrm = to_list->num;
    from_num_xfrm = from_list->num;

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "LTM LT bcmltm_nc_lta_xfrm_info_list_t");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));
    if ((to_num_xfrm * from_num_xfrm) == 0) {
        list->num = 0;
        *list_ptr = list;
        SHR_FUNC_EXIT();
    }

    size = sizeof(bcmltm_nc_lta_xfrm_info_t) * to_num_xfrm * from_num_xfrm;
    SHR_ALLOC(list->xfrm_info , size, "LTM LT bcmltm_nc_lta_xfrm_info_list_t");
    SHR_NULL_CHECK(list->xfrm_info, SHR_E_MEMORY);
    sal_memset(list->xfrm_info, 0, size);

    sal_memset(&info, 0, sizeof(info));

    /* Allocate */
    for (to_xfrm_idx = 0; to_xfrm_idx < to_num_xfrm; to_xfrm_idx++) {
        for (from_xfrm_idx = 0; from_xfrm_idx < from_num_xfrm;
             from_xfrm_idx++) {
            SHR_IF_ERR_EXIT
                (bcmltm_nc_copy_lta_field_list_create(unit, sid,
                           from_list->xfrm_info[from_xfrm_idx].fout,
                           to_list->xfrm_info[to_xfrm_idx].fin,
                                                      &(info.fin)));

            sal_memcpy(&(list->xfrm_info[(to_xfrm_idx * from_num_xfrm) +
                                         from_xfrm_idx]),
                       &info, sizeof(info));
        }
    }

    list->num = to_num_xfrm * from_num_xfrm;
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lta_xfrm_info_destroy(list);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_nc_lta_xfrm_info_destroy(bcmltm_nc_lta_xfrm_info_list_t *list)
{
    bcmltm_nc_lta_xfrm_info_t *info = NULL;
    uint32_t idx;

    if (list == NULL) {
        return;
    }

    for (idx = 0; idx < list->num; ++idx) {
        info = &list->xfrm_info[idx];

        /* Destroy XFRM Field lists */
        bcmltm_nc_field_list_destroy(info->fout);
        bcmltm_nc_field_list_destroy(info->fin);

        /* Destroy XFRM table params */
        transform_params_destroy(info->xfrm_params);
    }

    SHR_FREE(list->xfrm_info);

    SHR_FREE(list);
    list = NULL;

    return;
}

