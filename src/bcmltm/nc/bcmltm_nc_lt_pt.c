/*! \file bcmltm_nc_lt_pt.c
 *
 * Logical Table Manager - Node Cookies.
 *
 * Logical Table mappings with Physical Table groups.
 *
 * This file contains routines to construct LT information
 * for LT with Physical Table groups.  The information is used by
 * the LTM metadata during the FA and EE stages.
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
#include <bcmbd/bcmbd.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmptm/bcmptm.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lt_index_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>

#include "bcmltm_nc_lt_map.h"
#include "bcmltm_nc_lt_pt.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/* PT Key Index and Memory Parameter WBC min and max bit positions  */
#define PT_KEY_MINBIT        0
#define PT_KEY_MAXBIT       31

/*!
 * \brief Keys allocation variants.
 *
 * This information is used for selecting a subset of fields
 * and building the desired PTM field list.
 */
typedef enum keys_alloc_e {
    /*!
     * No allocation involved, use all fields as listed in map.
     */
    KEYS_ALLOC_NONE,

    /*!
     * Select only keys fields that are required during an INSERT
     * operation on Index with Allocation LTs.
     */
    KEYS_ALLOC_REQUIRED,

    /*!
     * Select only keys fields that are optional during an INSERT
     * operation on Index with Allocation LTs.  If keys are not present,
     * they are allocated during the operation.
     */
    KEYS_ALLOC_OPTIONAL
} keys_alloc_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if given offset matches an offset in the list.
 *
 * This routine searchs the given offset in the provided offset list.
 *
 * \param [in] list Offset list.
 * \param [in] count Number of offsets in list.
 * \param [in] offset Offset to match.
 *
 * \retval TRUE Given offset was found in list.
 * \retval FALSE Otherwise.
 */
static bool
offset_match(uint32_t *list, uint32_t count, uint32_t offset)
{
    uint32_t i;

    for (i = 0; i < count; i++) {
        if (offset == list[i]) {
            return TRUE;
        }
    }

    return FALSE;
}


/*!
 * \brief Get the working buffer offsets for the LT index and memory parameter.
 *
 * This routine gets the working buffer offsets used for the LT index and
 * memory parameter on an Index LT.
 *
 * The offset for LT index must correspond to a PT index offset.
 * The offset for the LT memory parameter must correspond to a PT memory
 * parameter offset (port or table instance).
 *
 * LTM uses a track index to manage entries on Simple Index LTs.
 * The track index is calculated from the LT index and the memory parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_list PT list to search.
 * \param [out] index_offset Returning LT index offset.
 * \param [out] mem_param_offset Returning LT memory parameter offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_mem_param_offset_get(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_pt_list_t *pt_list,
                              uint32_t *index_offset,
                              uint32_t *mem_param_offset)
{
    uint32_t num_pt = pt_list->num_pt_view;
    unsigned int size;
    uint32_t *offset_list = NULL;
    uint32_t *index_list = NULL;
    uint32_t *mem_param_list = NULL;
    uint32_t index_count;
    uint32_t mem_param_count;
    bool index_found = FALSE;
    bool mem_param_found = FALSE;
    bool table_inst = FALSE;
    uint32_t pt_base_offset;
    bcmltm_wb_ptm_param_offset_t pt_offset;
    uint32_t pt_view;
    uint32_t pt_op = 0;      /* For now, assume pt op is 0 */

    SHR_FUNC_ENTER(unit);

    *index_offset = BCMLTM_WB_OFFSET_INVALID;
    *mem_param_offset = BCMLTM_WB_OFFSET_INVALID;

    /* Expected to be called on LT with direct PT maps */
    if (num_pt == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    size = sizeof(*offset_list) * num_pt * 2;
    SHR_ALLOC(offset_list, size, "LTM offset");
    SHR_NULL_CHECK(offset_list, SHR_E_MEMORY);
    sal_memset(offset_list, 0, sizeof(*offset_list));

    index_list = offset_list;
    mem_param_list = &offset_list[num_pt];

    /* Get mapped offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_pt_index_offsets_get(unit, sid, pt_list,
                                        num_pt,
                                        index_list, &index_count));
    SHR_IF_ERR_EXIT
        (bcmltm_nc_pt_mem_param_offsets_get(unit, sid, pt_list,
                                            num_pt,
                                            mem_param_list, &mem_param_count));

    /*
     * Find a PT with offsets that match an offset in the index
     * offset list and an offset in the memory parameter list.
     *
     * If a map list count is 0, the offset can be any PT.
     * In this case, the data is always zero because the data
     * should have never been modified.
     */
    if (index_count == 0) {
        index_found = TRUE;
    }
    if (mem_param_count == 0) {
        mem_param_found = TRUE;
    }

    for (pt_view = 0; pt_view < pt_list->num_pt_view; pt_view++) {
        /* Get PTM parameter offsets */
        pt_base_offset = pt_list->memop_lists[pt_view]->param_offsets[pt_op];
        (void)bcmltm_wb_ptm_param_offset_get(pt_base_offset, &pt_offset);

        if (index_count > 0) { /* Index */
            if (offset_match(index_list, index_count, pt_offset.index)) {
                index_found = TRUE;
            } else {
                index_found = FALSE;
                continue;
            }
        } /* else, don't care which PT */

        if (mem_param_count > 0) {  /* Port or Table instance */
            if (offset_match(mem_param_list, mem_param_count,
                             pt_offset.port)) {
                mem_param_found = TRUE;
            } else if (offset_match(mem_param_list, mem_param_count,
                                    pt_offset.table_inst)) {
                mem_param_found = TRUE;
                table_inst = TRUE;
            } else {
                mem_param_found = FALSE;
                continue;
            }
        } /* else, don't care which PT */

        if (index_found && mem_param_found) {
            break;
        }
    }

    if (pt_view >= num_pt) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Unable to find correlation between "
                              "logical table key and PT offset: "
                              "sid=%d\n"),
                   sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    *index_offset = pt_offset.index;
    if (table_inst) {
        *mem_param_offset = pt_offset.table_inst;
    } else {
        *mem_param_offset = pt_offset.port;
    }

 exit:
    SHR_FREE(offset_list);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT param_offsets metadata.
 *
 * Destroy the given PT param_offsets metadata.
 *
 * \param [in] param_offsets Pointer to PT param_offsets metadata to destroy.
 */
static void
pt_param_offsets_destroy(uint32_t *param_offsets)
{
    if (param_offsets == NULL) {
        return;
    }

    SHR_FREE(param_offsets);

    return;
}


/*!
 * \brief Create the PT param_offsets metadata.
 *
 * Create the PT parameter indexes metadata for the given PT view.
 *
 * Assumes arguments are valid (i.e. unit, etc.)
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [out] param_offsets_ptr Returning pointer to PT param_offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_param_offsets_create(int unit,
                        const bcmltm_map_pt_info_t *pt_info,
                        const bcmltm_wb_block_t *wb_block,
                        uint32_t **param_offsets_ptr)
{
    uint32_t *param_offsets = NULL;
    unsigned int size;
    uint32_t op_idx;
    uint32_t num_ops = pt_info->num_ops;

    SHR_FUNC_ENTER(unit);

    /* Allocate for all PT ops needed for this PT view */
    size = sizeof(uint32_t) * num_ops;
    SHR_ALLOC(param_offsets, size, "LTM PT param_offsets");
    SHR_NULL_CHECK(param_offsets, SHR_E_MEMORY);
    sal_memset(param_offsets, 0, size);

    for (op_idx = 0; op_idx < num_ops; op_idx++) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_param_base_offset_get(wb_block,
                                                 op_idx,
                                                 &param_offsets[op_idx]));
    }

    *param_offsets_ptr = param_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        pt_param_offsets_destroy(param_offsets);
        *param_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT buffer offsets metadata.
 *
 * Destroy the given PT buffer offsets metadata.
 *
 * \param [in] buffer_offsets Pointer to PT buffer offsets to destroy
 */
static void
pt_buffer_offsets_destroy(uint32_t *buffer_offsets)
{
    if (buffer_offsets == NULL) {
        return;
    }

    SHR_FREE(buffer_offsets);

    return;
}


/*!
 * \brief Create the PT buffer offsets metadata.
 *
 * Create the PT buffer offsets metadata for the given PT view.
 *
 * Assumes arguments are valid (i.e. unit, etc.)
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Table PT mapping information.
 * \param [in] wb_block Working buffer block.
 * \param [out] buffer_offsets_ptr Returning pointer to PT buffer offsets md.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_buffer_offsets_create(int unit,
                         const bcmltm_map_pt_info_t *pt_info,
                         const bcmltm_wb_block_t *wb_block,
                         uint32_t **buffer_offsets_ptr)
{
    uint32_t *buffer_offsets = NULL;
    unsigned int size;
    uint32_t op_idx;
    uint32_t num_ops = pt_info->num_ops;

    SHR_FUNC_ENTER(unit);

    /* Allocate for all PT (ops) needed for this PT view */
    size = sizeof(uint32_t) * num_ops;
    SHR_ALLOC(buffer_offsets, size, "LTM PT buffer_offsets");
    SHR_NULL_CHECK(buffer_offsets, SHR_E_MEMORY);
    sal_memset(buffer_offsets, 0, size);

    for (op_idx = 0; op_idx < num_ops; op_idx++) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_ptm_entry_base_offset_get(wb_block,
                                                 op_idx,
                                                 &buffer_offsets[op_idx]));
    }

    *buffer_offsets_ptr = buffer_offsets;

exit:
    if (SHR_FUNC_ERR()) {
        pt_buffer_offsets_destroy(buffer_offsets);
        *buffer_offsets_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT op list metadata.
 *
 * Destroy the given PT op list metadata.
 *
 * \param [in] op_list Pointer to PT op list metadata to destroy.
 */
static void
pt_op_list_destroy(bcmltm_pt_op_list_t *op_list)
{
    if (op_list == NULL) {
        return;
    }

    pt_buffer_offsets_destroy(op_list->buffer_offsets);
    pt_param_offsets_destroy(op_list->param_offsets);

    SHR_FREE(op_list);
    return;
}


/*!
 * \brief Create the PT op list metadata.
 *
 * Create the PT op list metadata for the given PT view.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, pt_info are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_info Table PT mapping information.
 * \param [in] lt_map LT mapping information.
 * \param [out] op_list_ptr Returning pointer to PT op list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_op_list_create(int unit,
                  bcmlrd_sid_t sid,
                  const bcmltm_map_pt_info_t *pt_info,
                  const bcmltm_map_entries_pt_t *lt_map,
                  bcmltm_pt_op_list_t **op_list_ptr)
{
    bcmltm_pt_op_list_t *op_list = NULL;
    bcmdrd_sid_t ptid;
    const bcmltm_wb_block_t *wb_block = NULL;

    SHR_FUNC_ENTER(unit);

    ptid = pt_info->sid;

    /* Get working buffer block for PT */
    wb_block = bcmltm_wb_lt_block_get(unit, sid, pt_info->wb_block_id);
    if (wb_block == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not find working buffer block: "
                              "sid=%d ptid=%d wb_block_id=%d\n"),
                   sid, ptid, pt_info->wb_block_id));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    SHR_ALLOC(op_list, sizeof(*op_list),
              "LTM Logical Table pt_op_list");
    SHR_NULL_CHECK(op_list, SHR_E_MEMORY);
    sal_memset(op_list, 0, sizeof(*op_list));

    op_list->flags = pt_info->flags;
    op_list->num_pt_ops = pt_info->num_ops;
    op_list->word_size = bcmdrd_pt_entry_wsize(unit, ptid);

    /*
     * Override the table instance flag since this depends on the
     * LT map specification rather than the HW symbol properties.
     */
    if (lt_map->table_inst) {
        op_list->flags |= BCMLTM_PT_OP_FLAGS_TABLE_INST;
    } else {
        op_list->flags &= ~BCMLTM_PT_OP_FLAGS_TABLE_INST;
    }
    if (lt_map->table_xfrm) {
        op_list->flags |= BCMLTM_PT_OP_FLAGS_PT_SID;
    } else {
        op_list->flags &= ~BCMLTM_PT_OP_FLAGS_PT_SID;
    }

    SHR_IF_ERR_EXIT
        (pt_param_offsets_create(unit, pt_info, wb_block,
                                 &op_list->param_offsets));
    SHR_IF_ERR_EXIT
        (pt_buffer_offsets_create(unit, pt_info, wb_block,
                                  &op_list->buffer_offsets));

    *op_list_ptr = op_list;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_op_list_destroy(op_list);
        *op_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given list of pointers to PT op list metadata.
 *
 * Destroy the given list of pointers to PT op list metadata.
 *
 * \param [in] num_pt_views Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT op list.
 */
static void
pt_op_list_ptrs_destroy(uint32_t num_pt_views,
                        bcmltm_pt_op_list_t **op_list_ptrs)
{
    uint32_t pt_view;

    if (op_list_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (op_list_ptrs[pt_view] == NULL) {
            continue;
        }
        pt_op_list_destroy(op_list_ptrs[pt_view]);
    }

    SHR_FREE(op_list_ptrs);
    return;
}


/*!
 * \brief Create the list of pointers to PT op list metadata.
 *
 * Create the list of pointers to PT op list metadata for the given
 * physical mapping information.
 *
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit and list of table IDs are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] lt_map LT mapping information.
 * \param [out] op_list_ptrs_ptr Returning pointer to list of PT op list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_op_list_ptrs_create(int unit,
                       bcmlrd_sid_t sid,
                       const bcmltm_map_pt_t *pt_map,
                       const bcmltm_map_entries_pt_t *lt_map,
                       bcmltm_pt_op_list_t ***op_list_ptrs_ptr)
{
    unsigned int size;
    uint32_t pt_view;
    uint32_t num_pt_views = pt_map->num_pt;
    bcmltm_pt_op_list_t **op_list_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    size = sizeof(bcmltm_pt_op_list_t *) * num_pt_views;
    SHR_ALLOC(op_list_ptrs, size,
              "LTM Logical Table op_list ptrs");
    SHR_NULL_CHECK(op_list_ptrs, SHR_E_MEMORY);
    sal_memset(op_list_ptrs, 0, size);

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        pt_op_list_create(unit, sid, &pt_map->info[pt_view], lt_map,
                          &op_list_ptrs[pt_view]);
    }

    *op_list_ptrs_ptr = op_list_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_op_list_ptrs_destroy(num_pt_views, op_list_ptrs);
        *op_list_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT dynamic info metadata.
 *
 * Destroy the given PT dynamic info metadata.
 *
 * \param [in] pt_dyn_info Pointer to PT dynamic info to destroy.
 */
static void
pt_dyn_info_destroy(bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    SHR_FREE(pt_dyn_info);
    return;
}


/*!
 * \brief Create the PT dynamic info metadata.
 *
 * Create the PT op list metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_dyn_info_ptr Returning pointer to PT dynamic info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_dyn_info_create(int unit, bcmbd_pt_dyn_info_t **pt_dyn_info_ptr)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt_dyn_info, sizeof(*pt_dyn_info),
              "LTM Logical Table pt_dyn_info");
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_MEMORY);
    sal_memset(pt_dyn_info, 0, sizeof(*pt_dyn_info));

    /* Fill data */
    pt_dyn_info->index = 0;
    pt_dyn_info->tbl_inst = -1;

    *pt_dyn_info_ptr = pt_dyn_info;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_dyn_info_destroy(pt_dyn_info);
        *pt_dyn_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT mem args metadata.
 *
 * Destroy the given PT mem args metadata.
 *
 * \param [in] mem_args Pointer to PT mem args metadata to destroy.
 */
static void
pt_mem_args_destroy(bcmltm_pt_mem_args_t *mem_args)
{
    if (mem_args == NULL) {
        return;
    }

    pt_dyn_info_destroy(mem_args->pt_dyn_info);
    SHR_FREE(mem_args);

    return;
}


/*!
 * \brief Create the PT mem args metadata.
 *
 * Create the PT mem args metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] ptid Physical table ID.
 * \param [out] mem_args_ptr Returning pointer to PT mem args metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_mem_args_create(int unit, bcmdrd_sid_t ptid,
                   bcmltm_pt_mem_args_t **mem_args_ptr)
{
    bcmltm_pt_mem_args_t *mem_args = NULL;
    uint32_t flags = 0;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(mem_args, sizeof(*mem_args),
              "LTM Logical Table pt_mem_args");
    SHR_NULL_CHECK(mem_args, SHR_E_MEMORY);
    sal_memset(mem_args, 0, sizeof(*mem_args));

    /* Fill data */
    mem_args->flags = flags;
    mem_args->pt = ptid;
    mem_args->pt_static_info = NULL;
    mem_args->pt_dyn_hash_info = NULL;  /* Not used */
    SHR_IF_ERR_EXIT
        (pt_dyn_info_create(unit, &mem_args->pt_dyn_info));

    *mem_args_ptr = mem_args;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_mem_args_destroy(mem_args);
        *mem_args_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given list of pointers to PT mem args metadata.
 *
 * Destroy the given list of pointers to PT mem args metadata.
 *
 * \param [in] num_pt_views Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT mem args list.
 */
static void
pt_mem_args_ptrs_destroy(uint32_t num_pt_views,
                         bcmltm_pt_mem_args_t **mem_args_ptrs)
{
    uint32_t pt_view;

    if (mem_args_ptrs == NULL) {
        return;
    }

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        if (mem_args_ptrs[pt_view] == NULL) {
            continue;
        }
        pt_mem_args_destroy(mem_args_ptrs[pt_view]);
    }

    SHR_FREE(mem_args_ptrs);
    return;
}


/*!
 * \brief Create the list of pointers to PT mem args metadata.
 *
 * Create the list of pointers to PT mem args metadata for the given
 * physical mapping information.
 *
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit and list of table IDs are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_map Table PT mapping information.
 * \param [out] mem_args_ptrs_ptr Returning pointer to list of PT mem args list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_mem_args_ptrs_create(int unit,
                        const bcmltm_map_pt_t *pt_map,
                        bcmltm_pt_mem_args_t ***mem_args_ptrs_ptr)
{
    unsigned int size;
    uint32_t pt_view;
    uint32_t num_pt_views = pt_map->num_pt;
    bcmltm_pt_mem_args_t **mem_args_ptrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    size = sizeof(bcmltm_pt_mem_args_t *) * num_pt_views;
    SHR_ALLOC(mem_args_ptrs, size,
              "LTM Logical Table mem_args ptrs");
    SHR_NULL_CHECK(mem_args_ptrs, SHR_E_MEMORY);
    sal_memset(mem_args_ptrs, 0, size);

    for (pt_view = 0; pt_view < num_pt_views; pt_view++) {
        pt_mem_args_create(unit, pt_map->info[pt_view].sid,
                           &mem_args_ptrs[pt_view]);
    }

    *mem_args_ptrs_ptr = mem_args_ptrs;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_mem_args_ptrs_destroy(num_pt_views, mem_args_ptrs);
        *mem_args_ptrs_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT list metadata.
 *
 * Destroy the given PT list metadata.
 *
 * \param [in] pt_list List of PT metadata.
 */
static void
pt_list_destroy(bcmltm_pt_list_t *pt_list)
{
    if (pt_list == NULL) {
        return;
    }

    pt_op_list_ptrs_destroy(pt_list->num_pt_view, pt_list->memop_lists);
    pt_mem_args_ptrs_destroy(pt_list->num_pt_view, pt_list->mem_args);

    SHR_FREE(pt_list);

    return;
}


/*!
 * \brief Create the PT list metadata.
 *
 * Create the PT list metadata for the given physical mapping information.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] lt_map LT mapping information.
 * \param [out] pt_list_ptr Returning pointer PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_list_create(int unit, bcmlrd_sid_t sid,
               const bcmltm_map_pt_t *pt_map,
               const bcmltm_map_entries_pt_t *lt_map,
               bcmltm_pt_list_t **pt_list_ptr)
{
    bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt_list, sizeof(*pt_list),
              "LTM Logical Table pt_list");
    SHR_NULL_CHECK(pt_list, SHR_E_MEMORY);
    sal_memset(pt_list, 0, sizeof(*pt_list));

    /* Fill data */
    pt_list->num_pt_view = pt_map->num_pt;
    SHR_IF_ERR_EXIT
        (pt_mem_args_ptrs_create(unit, pt_map,
                                 &pt_list->mem_args));
    SHR_IF_ERR_EXIT
        (pt_op_list_ptrs_create(unit, sid, pt_map, lt_map,
                                &pt_list->memop_lists));

    *pt_list_ptr = pt_list;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_list_destroy(pt_list);
        *pt_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given PT keyed operations information metadata.
 *
 * Destroy the given PT keyed operations information metadata.
 *
 * \param [in] pt_list List of PT metadata.
 */
static void
pt_keyed_op_info_destroy(bcmltm_pt_keyed_op_info_t *info)
{
    if (info == NULL) {
        return;
    }

    pt_mem_args_destroy(info->mem_args);

    SHR_FREE(info);

    return;
}


/*!
 * \brief Create the PT keyed operations information metadata.
 *
 * Create the PT keyed operations information metadata for
 * the given logical table and PTM operation type.
 *
 * This routine populates the required metadata information.
 * This metadata is used for Keyed LTs of type Hash and TCAMs.
 *
 * The PT maps for Keyed LTs should be defined as follows:
 * - Hash                   : 1 PT (which maps to Key and Value fields).
 * - TCAM Aggregate View    : 1 PT (which maps to Key and Value fields).
 * - TCAM Non-Aggregate View: 2 PTs (one maps to Key fields and the second
 *                            maps to Value fields).
 *
 * It is assumed that the FLTG will check to verify that the tables
 * are defined correctly to meet these PT map rules.
 *
 * The PTM interface for Keyed tables expects the Key and Value fields to
 * to be passed in the same PTM operation.  The Key fields are placed in
 * the PT key entry section whereas the Value fields are placed in
 * the PT data entry section.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] attr Table attributes.
 * \param [in] ptm_op PTM operation.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] lt_map LT mapping information.
 * \param [out] info_ptr Returning pointer for PT keyed op info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_keyed_op_info_create(int unit, bcmlrd_sid_t sid,
                        const bcmltm_table_attr_t *attr,
                        bcmptm_op_type_t ptm_op,
                        const bcmltm_map_pt_t *pt_map,
                        const bcmltm_map_entries_pt_t *lt_map,
                        bcmltm_pt_keyed_op_info_t **info_ptr)
{
    bcmltm_pt_keyed_op_info_t *info = NULL;
    bcmltm_wb_block_id_t wb_block_id;
    const bcmltm_wb_block_t *wb_block = NULL;
    uint32_t pt_view;
    bcmdrd_sid_t ptid;
    uint32_t pt_wsize;
    uint32_t entry_base_offset;
    bcmltm_wb_ptm_entry_keyed_lt_offset_t entry_offset;
    uint32_t pt_idx;
    uint32_t set_ki_idx, set_ko_idx, set_di_idx, set_do_idx;

    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    if (pt_map->num_pt < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Keyed LT should contain at least 1 PT: "
                              "sid=%d num_pt=%d\n"),
                   sid, pt_map->num_pt));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Get working buffer block
     * Keyed LTs only have 1 PTM working buffer block.
     * Use block ID of any view (all views should have same block ID).
     */
    wb_block_id = pt_map->info[0].wb_block_id;
    wb_block = bcmltm_wb_lt_block_get(unit, sid, wb_block_id);
    if (wb_block == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not find working buffer block: "
                              "sid=%d wb_block_id=%d\n"),
                   sid, wb_block_id));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info),
              "LTM Logical Table pt_keyed_op_info");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Fill data */

    /* Assign PT key and data entry sizes to allocated PT entry size in WB */
    pt_wsize = bcmltm_wb_ptm_pt_entry_wsize();
    info->key_word_size  = pt_wsize;
    info->data_word_size = pt_wsize;

    /* Combine flags of all PTs */
    for (pt_view = 0; pt_view < pt_map->num_pt; pt_view++) {
        info->flags |=  pt_map->info[pt_view].flags;
    }
    /*
     * Override the table instance flag since this depends on the
     * LT map specification rather than the HW symbol properties.
     */
    if (lt_map->table_inst) {
        info->flags |= BCMLTM_PT_OP_FLAGS_TABLE_INST;
    } else {
        info->flags &= ~BCMLTM_PT_OP_FLAGS_TABLE_INST;
    }
    if (lt_map->table_xfrm) {
        info->flags |= BCMLTM_PT_OP_FLAGS_PT_SID;
    } else {
        info->flags &= ~BCMLTM_PT_OP_FLAGS_PT_SID;
    }

    /* Use first PT ID for mem_args (this is a don't care, PTM uses LTID) */
    ptid = pt_map->info[0].sid;
    SHR_IF_ERR_EXIT
        (pt_mem_args_create(unit, ptid, &info->mem_args));

    /* Get PTM param working buffer offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_param_base_offset_get(wb_block, 0,
                                             &info->param_offset));

    /* Get PTM entry key/data working buffer offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_base_offset_get(wb_block, 0,
                                             &entry_base_offset));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_lt_offset_get(entry_base_offset,
                                                 &entry_offset));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_KEY,
                                           BCMLTM_FIELD_DIR_IN,
                                           &set_ki_idx));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_VALUE,
                                           BCMLTM_FIELD_DIR_IN,
                                           &set_di_idx));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_KEY,
                                           BCMLTM_FIELD_DIR_OUT,
                                           &set_ko_idx));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_entry_keyed_idx_get(ptm_op,
                                           BCMLTM_FIELD_TYPE_VALUE,
                                           BCMLTM_FIELD_DIR_OUT,
                                           &set_do_idx));
    for (pt_idx = 0; pt_idx < BCMLTM_PT_ENTRY_LIST_MAX; pt_idx++) {
        info->input_key_buffer_offset[pt_idx] =
            entry_offset.key[set_ki_idx][pt_idx];
        info->input_data_buffer_offset[pt_idx] =
            entry_offset.data[set_di_idx][pt_idx];
        info->output_key_buffer_offset[pt_idx] =
            entry_offset.key[set_ko_idx][pt_idx];
        info->output_data_buffer_offset[pt_idx] =
            entry_offset.data[set_do_idx][pt_idx];
    }

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        pt_keyed_op_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given list of pointers to PT keyed info metadata.
 *
 * Destroy the given list of pointers to PT keyed info metadata.
 *
 * \param [in] num Number of entries in list.
 * \param [in] op_list_ptrs List of pointers to PT mem args list.
 */
static void
pt_keyed_list_destroy(bcmltm_pt_keyed_op_info_t **info_list)
{
    uint32_t idx;

    if (info_list == NULL) {
        return;
    }

    for (idx = 0; idx < BCMLTM_NC_PT_KEYED_NUM; idx++) {
        pt_keyed_op_info_destroy(info_list[idx]);
    }

    return;
}


/*!
 * \brief Create the list of PT keyed operations information metadata.
 *
 * Create the list of PT keyed operations information metadata for
 * all the PTM operations on the given logical table.
 * Some PTM operations share the same information, so only necessary
 * distint node cookie are created.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_map Table PT mapping information.
 * \param [in] lt_map LT mapping information.
 * \param [out] info_list Returning list of PT keyed op info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_keyed_list_create(int unit,
                     bcmlrd_sid_t sid,
                     const bcmltm_table_attr_t *attr,
                     const bcmltm_map_pt_t *pt_map,
                     const bcmltm_map_entries_pt_t *lt_map,
                     bcmltm_pt_keyed_op_info_t **info_list)
{
    SHR_FUNC_ENTER(unit);

    /* Create information for INSERT/DELETE (share same information) */
    SHR_IF_ERR_EXIT
        (pt_keyed_op_info_create(unit, sid, attr,
                                 BCMPTM_OP_INSERT,
                                 pt_map, lt_map,
                                 &info_list[BCMLTM_NC_PT_KEYED_INS_DEL]));

    /* Create information for LOOKUP/TRAVERSE (share same information) */
    SHR_IF_ERR_EXIT
        (pt_keyed_op_info_create(unit, sid, attr,
                                 BCMPTM_OP_LOOKUP,
                                 pt_map, lt_map,
                                 &info_list[BCMLTM_NC_PT_KEYED_LOOK_TRAV]));

 exit:
    if (SHR_FUNC_ERR()) {
        pt_keyed_list_destroy(info_list);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given FA LT Index metadata.
 *
 * Destroy the given FA LT Index metadata.
 *
 * \param [in] index LT Index.
 */
static void
fa_lt_index_destroy(bcmltm_fa_lt_index_t *index)
{
    bcmltm_nc_fa_lt_index_destroy(index);

    return;
}


/*!
 * \brief Create the FA LT Index metadata.
 *
 * Create the FA LT Index metadata for the given table.
 *
 * This routine populates the required metadata information.
 *
 * LTs with no index or memory parameter will still reference
 * a valid WB offset location.  In this case, the content of the
 * location will always be zero (working buffer is always reset
 * to zero during its allocation for each operation).
 * In addition, the corresponding 'min' and 'max' metadata for
 * 'lt_index' and 'mem_param' are also set to zero.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] lt_index_wb_offset LT Index working buffer block offset.
 * \param [in] pt_list PT list.
 * \param [out] index_ptr Returning pointer FA LT Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fa_lt_index_create(int unit, bcmlrd_sid_t sid,
                   const bcmltm_table_attr_t *attr,
                   const bcmltm_wb_lt_index_offset_t *lt_index_wb_offset,
                   const bcmltm_pt_list_t *pt_list,
                   bcmltm_fa_lt_index_t **index_ptr)
{
    bcmltm_fa_lt_index_t *index = NULL;
    uint32_t lt_index_offset;
    uint32_t mem_param_offset;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(index, sizeof(*index),
              "LTM Logical Table fa_lt_index_t");
    SHR_NULL_CHECK(index, SHR_E_MEMORY);
    sal_memset(index, 0, sizeof(*index));

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (lt_index_mem_param_offset_get(unit, sid, pt_list,
                                       &lt_index_offset, &mem_param_offset));

    /* Fill data */
    index->lt_index_offset = lt_index_offset;
    index->track_index_offset = lt_index_wb_offset->track;
    index->mem_param_offset = mem_param_offset;
    index->index_multiplier = attr->index_max + 1;

    /* Valid for Index with Allocation LT */
    if (BCMLTM_TABLE_TYPE_LT_ALLOC_INDEX(attr->type)) {
        index->index_absent_offset = lt_index_wb_offset->index_absent;
    } else {
        index->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;
    }

    *index_ptr = index;

 exit:
    if (SHR_FUNC_ERR()) {
        fa_lt_index_destroy(index);
        *index_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given EE Index Info metadata.
 *
 * Destroy the given EE Index Info metadata.
 *
 * \param [in] index_info Index info.
 */
static void
ee_lt_index_destroy(bcmltm_ee_index_info_t *index_info)
{
    if (index_info == NULL) {
        return;
    }

    SHR_FREE(index_info);

    return;
}


/*!
 * \brief Create the EE LT Index Info metadata.
 *
 * Create the EE LT Index Info metadata for the given table.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_index_wb_offset LT Index working buffer block offset.
 * \param [out] index_info_ptr Returning pointer EE LT Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ee_lt_index_create(int unit, bcmlrd_sid_t sid,
                   const bcmltm_wb_lt_index_offset_t *lt_index_wb_offset,
                   bcmltm_ee_index_info_t **index_info_ptr)
{
    bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(index_info, sizeof(*index_info),
              "LTM Logical Table ee_index_info_t");
    SHR_NULL_CHECK(index_info, SHR_E_MEMORY);
    sal_memset(index_info, 0, sizeof(*index_info));

    /* Fill data */
    index_info->index_offset = lt_index_wb_offset->track;

    *index_info_ptr = index_info;

 exit:
    if (SHR_FUNC_ERR()) {
        ee_lt_index_destroy(index_info);
        *index_info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the given LT to PT index WB copy list metadata.
 *
 * Destroy the given LT to PT index WB copy list metadata.
 *
 * \param [in] index LT Index.
 */
static void
lt_to_pt_index_destroy(bcmltm_wb_copy_list_t *list)
{
    if (list == NULL) {
        return;
    }

    if (list->wb_copy != NULL) {
        SHR_FREE(list->wb_copy);
    }
    SHR_FREE(list);

    return;
}


/*!
 * \brief Create the LT to PT index WB copy list metadata.
 *
 * Create the LT to PT index WB copy list metadata for the given table.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] pt_list PT list.
 * \param [in] lt_index FA LT Index.
 * \param [out] list_ptr Returning pointer LT to PT index copy list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_to_pt_index_create(int unit, bcmlrd_sid_t sid,
                      const bcmltm_pt_list_t *pt_list,
                      const bcmltm_fa_lt_index_t *lt_index,
                      bcmltm_wb_copy_list_t **list_ptr)
{
    bcmltm_wb_copy_list_t *list = NULL;
    int num_pt = pt_list->num_pt_view;
    uint32_t *offset = NULL;
    uint32_t *index_offset = NULL;
    uint32_t *mem_param_offset = NULL;
    uint32_t num_index_offset;
    uint32_t num_mem_param_offset;
    uint32_t num_wb_copy;
    bcmltm_wb_copy_t *wb_copy = NULL;
    bcmltm_wb_copy_t *wb_ptr;
    unsigned int size;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    size = sizeof(*offset) * num_pt * 2;
    SHR_ALLOC(offset, size, "LTM offset");
    SHR_NULL_CHECK(offset, SHR_E_MEMORY);
    sal_memset(offset, 0, sizeof(*offset));

    index_offset = offset;
    mem_param_offset = &offset[num_pt];

    /* Get offsets */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_pt_index_offsets_get(unit, sid, pt_list,
                                        num_pt,
                                        index_offset, &num_index_offset));
    SHR_IF_ERR_EXIT
        (bcmltm_nc_pt_mem_param_offsets_get(unit, sid, pt_list,
                                            num_pt,
                                            mem_param_offset,
                                            &num_mem_param_offset));

    num_wb_copy = num_index_offset + num_mem_param_offset;

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list),
              "LTM Logical Table LT to PT wb_copy_list");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    if (num_wb_copy > 0) {
        size = sizeof(*wb_copy) * num_wb_copy;
        SHR_ALLOC(wb_copy, size,
                  "LTM Logical Table LT to PT wb_copy");
        SHR_NULL_CHECK(wb_copy, SHR_E_MEMORY);
        sal_memset(wb_copy, 0, sizeof(*wb_copy));
    }

    list->num_wb_copy = num_wb_copy;
    list->wb_copy = wb_copy;

    if (wb_copy != NULL) {
        wb_ptr = wb_copy;

        /* Fill WBC Index Offsets */
        for (i = 0; i < num_index_offset; i++, wb_ptr++) {
            wb_ptr->wbc_source.buffer_offset = lt_index->lt_index_offset;
            wb_ptr->wbc_source.minbit = PT_KEY_MINBIT;
            wb_ptr->wbc_source.maxbit = PT_KEY_MAXBIT;
            wb_ptr->wbc_destination.buffer_offset = index_offset[i];
            wb_ptr->wbc_destination.minbit = PT_KEY_MINBIT;
            wb_ptr->wbc_destination.maxbit = PT_KEY_MAXBIT;
        }

        /* Fill WBC Memory Parameter Offsets */
        for (i = 0; i < num_mem_param_offset; i++, wb_ptr++) {
            wb_ptr->wbc_source.buffer_offset = lt_index->mem_param_offset;
            wb_ptr->wbc_source.minbit = PT_KEY_MINBIT;
            wb_ptr->wbc_source.maxbit = PT_KEY_MAXBIT;
            wb_ptr->wbc_destination.buffer_offset = mem_param_offset[i];
            wb_ptr->wbc_destination.minbit = PT_KEY_MINBIT;
            wb_ptr->wbc_destination.maxbit = PT_KEY_MAXBIT;
        }
    }

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        lt_to_pt_index_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FREE(offset);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Check if logical key field is required on IwA.
 *
 * This routine checks if given logical key field is required
 * on an INSERT for Index with Allocation LTs.
 *
 * On Index with Allocation LTs, some keys can be absent during INSERT,
 * and if so, they are allocated by LTM.
 *
 * - Required keys:  those that correspond to the LT memory parameters,
 *                   i.e. PT port, PT instance.
 * - Optional keys:  those that correspond to the LT index,
 *                   i.e. PT index.
 *
 * Assumptions:
 * - Field ID is a valid key field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 *
 * \retval TRUE, if field is required for INSERT on IwA.
 * \retval FALSE, if field is optional for INSERT on IwA.
 */
static bool
key_field_alloc_required(int unit, bcmlrd_sid_t sid, uint32_t fid)
{
    /* Field is not required if it corresponds to an index */
    if (bcmltm_nc_field_is_lt_index(unit, sid, fid)) {
        return FALSE;
    }

    return TRUE;
}


/*!
 * \brief Allocate a field map object and insert to link list.
 *
 * This function allocates a field map object and inserts it
 * to the field map link list.
 *
 * This function does not fill field map information.
 *
 * \param [in] field_list Field list to add new field map to.
 * \param [in] field_map_ptr Returns pointer to new field map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_map_alloc(bcmltm_ptm_field_map_t *field_list,
                bcmltm_ptm_field_map_t **field_map_ptr)
{
    bcmltm_ptm_field_map_t *new_field_map = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Allocate */
    SHR_ALLOC(new_field_map, sizeof(*new_field_map),
              "LTM Logical Table map_field_mapped");
    SHR_NULL_CHECK(new_field_map, SHR_E_MEMORY);
    sal_memset(new_field_map, 0, sizeof(*new_field_map));

    /* Insert it first in the link list (order does not matter) */
    new_field_map->next_map = field_list->next_map;
    field_list->next_map = new_field_map;

    *field_map_ptr = new_field_map;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the field map metadata for given LRD field entry map.
 *
 * Get the field map metadata for given LRD field entry map.
 *
 * \param [in] unit Unit number.
 * \param [in] attr Table attributes.
 * \param [in] map_info Field map information from LRD map entry.
 * \param [in] field_type Indicates key or value field map type.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [out] field_map PTM field map to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ptm_field_map_get(int unit,
                  const bcmltm_table_attr_t *attr,
                  const bcmltm_map_field_mapped_t *map_info,
                  bcmltm_field_type_t field_type,
                  bcmltm_field_dir_t field_dir,
                  bcmltm_ptm_field_map_t *field_map)
{
    SHR_FUNC_ENTER(unit);

    field_map->api_field_id = map_info->src_field_id;
    field_map->field_idx = map_info->src_field_idx;

    /* Set working buffer offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_lt_offset_get(map_info->ptm_wb_block,
                                           map_info->dst_field_id,
                                           field_type,
                                           field_dir,
                                           map_info->pt_entry_idx,
                                           &(field_map->wbc.buffer_offset)));
    field_map->wbc.maxbit = map_info->wbc_maxbit;
    field_map->wbc.minbit = map_info->wbc_minbit;

    /* Set PT changed offset */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_pt_changed_offset_get(attr->type,
                                             field_type,
                                             map_info->ptm_wb_block,
                                             &field_map->pt_changed_offset));

    /* Default value */
    field_map->default_value = map_info->default_value;
    /* Minimum value */
    field_map->minimum_value = map_info->minimum_value;
    /* Maximum value */
    field_map->maximum_value = map_info->maximum_value;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the list of field maps metadata.
 *
 * Create the list of field maps metadata.
 *
 * This routine copies the maps for valid fields from the source list which
 * was perhaps larger than needed and formats it to what is expected
 * for its final use (FA cookie).
 *
 * NOTE:  The node functions that use this list expects a certain
 * placement of the field maps in order to function properly.
 * The following pattern must be met for the returned field map list:
 * - Maps that correspond to the same API logical field ID and index (fid, idx)
 *   are placed across the link list of the array element.
 * - Logical fields with distinct (fid, idx) are placed along the array.
 *   (there is no other additional ordering, i.e. elements in list
 *    array are not ordered).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] map_info_list Source field mapping information list.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [in] keys_alloc Indicates what keys to include in returning list
 *                        (relevant only for key fields on IwA LTs).
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ptm_field_maps_create(int unit,
                      bcmlrd_sid_t sid,
                      const bcmltm_table_attr_t *attr,
                      const bcmltm_map_field_mapped_list_t *map_info_list,
                      bcmltm_field_type_t field_type,
                      bcmltm_field_dir_t field_dir,
                      keys_alloc_t keys_alloc,
                      bcmltm_ptm_field_map_t **field_maps_ptr,
                      uint32_t *num_fields)
{
    bcmltm_ptm_field_map_t *field_maps = NULL;
    bcmltm_ptm_field_map_t *field_map = NULL;
    bcmltm_ptm_field_map_t *new_field_map = NULL;
    const bcmltm_map_field_mapped_t *map_info;
    unsigned int size;
    uint32_t field_max_count;
    uint32_t info_idx;
    uint32_t field_idx;
    uint32_t lt_fid;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of fields maps */
    field_max_count = map_info_list->num;
    if (field_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * field_max_count;
    SHR_ALLOC(field_maps, size,
              "LTM Logical Table ptm_field_map");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /*
     * Process field map information list
     *
     * The map information list is already organized in the
     * the way the FA node cookie field map is expected:
     * - All maps with the same field ID and index (fid, idx)
     *   are placed across the link list of the array element.
     * - Fields with distinct (fid, idx) are placed along the array.
     * - Note that there is no other additional ordering
     *   (i.e. elements in list array are not ordered).
     *
     * As as result, the PTM field map can be constructed following
     * the same map information list organization.
     */
    for (info_idx = 0, field_idx = 0;
         info_idx < map_info_list->max; info_idx++) {

        map_info = &map_info_list->field_maps[info_idx];

        /* Skip invalid field */
        if (map_info->src_field_idx == BCMLTM_FIELD_INDEX_UNDEFINED) {
            continue;
        }

        /*
         * Skip key field if:
         *   - Request is alloc required AND key field is optional
         *   - Request is alloc optional AND key field is required
         */
        lt_fid = map_info->src_field_id;
        if (field_type == BCMLTM_FIELD_TYPE_KEY) {
            if (((keys_alloc == KEYS_ALLOC_REQUIRED) &&
                 !key_field_alloc_required(unit, sid, lt_fid)) ||
                ((keys_alloc == KEYS_ALLOC_OPTIONAL) &&
                 key_field_alloc_required(unit, sid, lt_fid))) {
                continue;
            }
        }

        /* Process field map */
        field_map = &field_maps[field_idx];

        /* Get field map information */
        SHR_IF_ERR_EXIT
            (ptm_field_map_get(unit, attr, map_info, field_type, field_dir,
                               field_map));

        /* Process maps across link list */
        map_info = map_info->next;
        while (map_info != NULL) {
            /* Allocate map and insert to list */
            SHR_IF_ERR_EXIT
                (field_map_alloc(field_map, &new_field_map));

            /* Get field map information */
            SHR_IF_ERR_EXIT
                (ptm_field_map_get(unit, attr, map_info, field_type, field_dir,
                                   new_field_map));

            map_info = map_info->next;
        }

        field_idx++;
    }

    /* Sanity check */
    if (keys_alloc == KEYS_ALLOC_NONE) {
        if (field_idx != map_info_list->num) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM Number of fields mismatch: "
                                  "sid=%d expected=%d actual=%d\n"),
                       sid, map_info_list->num, field_idx));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
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
 * \brief Create the field list metadata.
 *
 * Create the field list metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] lt_index_wb_offset LT Index working buffer block offset
 *                                (relevant only on IwA LTs).
 * \param [in] lt_map LT mapping information.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [in] keys_alloc Indicates what keys to include in returning list
 *                        (relevant only for key fields on IwA LTs).
 * \param [out] field_list_ptr Returning pointer to field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ptm_field_list_create(int unit,
                      bcmlrd_sid_t sid,
                      const bcmltm_table_attr_t *attr,
                      const bcmltm_wb_lt_index_offset_t *lt_index_wb_offset,
                      const bcmltm_map_entries_pt_t *lt_map,
                      bcmltm_field_type_t field_type,
                      bcmltm_field_dir_t field_dir,
                      keys_alloc_t keys_alloc,
                      bcmltm_ptm_field_list_t **field_list_ptr)
{
    const bcmltm_map_field_mapped_list_t *map_info_list;
    bcmltm_ptm_field_list_t *field_list = NULL;
    uint32_t num_fields;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list),
              "LTM Logical Table ptm_field_list");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /* Key or Value */
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        map_info_list = &lt_map->keys;
    } else {
        map_info_list = &lt_map->values;
    }

    /* Get field map */
    SHR_IF_ERR_EXIT
        (ptm_field_maps_create(unit, sid, attr, map_info_list,
                               field_type, field_dir, keys_alloc,
                               &field_list->field_maps,
                               &num_fields));
    field_list->num_fields = num_fields;

    /* Valid for Index with Allocation LT and on optional key fields */
    if (BCMLTM_TABLE_TYPE_LT_ALLOC_INDEX(attr->type) &&
        (field_type == BCMLTM_FIELD_TYPE_KEY) &&
        (keys_alloc == KEYS_ALLOC_OPTIONAL)) {
        /* Sanity check */
        if (lt_index_wb_offset == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Index with Allocation LT must have "
                                  "a LT Index working buffer block: "
                                  "sid=%d\n"),
                       sid));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        field_list->index_absent_offset = lt_index_wb_offset->index_absent;
    } else {
        field_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;
    }

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_ptm_field_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the field mapping metadata.
 *
 * Create the field mapping metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] lt_index_wb_offset LT Index working buffer block offset,
 *                                (relevant only on IwA LTs).
 * \param [in] lt_map LT mapping information.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed LTs on Key fields).
 * \param [in] keys_alloc Indicates what keys to include in returning list
 *                        (relevant only for key fields on IwA LTs).
 * \param [out] ptm_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ptm_mapping_create(int unit,
                   bcmlrd_sid_t sid,
                   const bcmltm_table_attr_t *attr,
                   const bcmltm_wb_lt_index_offset_t *lt_index_wb_offset,
                   const bcmltm_map_entries_pt_t *lt_map,
                   bcmltm_field_type_t field_type,
                   bcmltm_field_dir_t field_dir,
                   keys_alloc_t keys_alloc,
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
        (ptm_field_list_create(unit, sid, attr, lt_index_wb_offset,
                               lt_map, field_type, field_dir, keys_alloc,
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
 * \brief Destroy the fixed field list metadata.
 *
 * Destroy the fixed field list metadata.
 *
 * \param [in] list Fixed field list metadata to destroy.
 */
static void
fixed_fields_destroy(bcmltm_fixed_field_list_t *list)
{
    if (list == NULL) {
        return;
    }

    SHR_FREE(list->fixed_fields);
    SHR_FREE(list);

    return;
}


/*!
 * \brief Create the fixed field list metadata.
 *
 * Create the fixed field list metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_map LT mapping information.
 * \param [out] list_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fixed_fields_create(int unit,
                    bcmlrd_sid_t sid,
                    const bcmltm_map_entries_pt_t *lt_map,
                    bcmltm_fixed_field_list_t **list_ptr)
{
    bcmltm_fixed_field_list_t *list = NULL;
    bcmltm_fixed_field_t *fields = NULL;
    uint32_t count;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list),
              "LTM Logical Table fixed_field_list");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    /* Allocate for the fields array and copy */
    count = lt_map->fixed.num;
    if (count > 0) {
        SHR_ALLOC(fields, sizeof(*fields) * count,
              "LTM Logical Table fixed field maps");
        SHR_NULL_CHECK(fields, SHR_E_MEMORY);
        sal_memcpy(fields, lt_map->fixed.field_maps,
                   sizeof(*fields) * count);
    }

    /* Fill data */
    list->num_fixed_fields = count;
    list->fixed_fields = fields;

    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        fixed_fields_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_nc_lt_pt_info_create(int unit, bcmlrd_sid_t sid,
                            const bcmltm_table_attr_t *attr,
                            const bcmltm_map_pt_t *pt_map,
                            bcmltm_nc_lt_pt_info_t **pt_ptr)
{
    bcmltm_nc_lt_pt_info_t *pt = NULL;
    bcmltm_map_entries_pt_t entries;
    bcmltm_table_type_t lt_type;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_block_t *lt_index_wb_block = NULL;
    bcmltm_wb_lt_index_offset_t lt_index_wb_offset;
    bcmltm_wb_lt_index_offset_t *lt_index_wb_offset_ref = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(pt, sizeof(*pt), "LTM Logical Table bcmltm_nc_lt_pt_info_t");
    SHR_NULL_CHECK(pt, SHR_E_MEMORY);
    sal_memset(pt, 0, sizeof(*pt));

    /* Get LT type */
    lt_type = attr->type;

    /* Add LT Index working buffer block (only for index table type) */
    pt->lt_index_wb_block_id = BCMLTM_WB_BLOCK_ID_INVALID;
    if (BCMLTM_TABLE_TYPE_LT_INDEX(lt_type)) {
        wb_handle = bcmltm_wb_lt_handle_get(unit, sid);

        SHR_IF_ERR_EXIT
            (bcmltm_wb_block_lt_index_add(unit, wb_handle,
                                          &pt->lt_index_wb_block_id,
                                          &lt_index_wb_block));
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_index_offset_get(lt_index_wb_block,
                                           &lt_index_wb_offset));
        lt_index_wb_offset_ref = &lt_index_wb_offset;
    }

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_map_entries_pt_parse(unit, sid, attr, pt_map, &entries));

    /* Create PT Information list (used as EE node cookie for PTM operations) */
    if (BCMLTM_TABLE_TYPE_LT_INDEX(lt_type)) {
        SHR_IF_ERR_EXIT
            (pt_list_create(unit, sid, pt_map, &entries, &pt->pt_list));
    } else if (BCMLTM_TABLE_TYPE_LT_KEYED(lt_type)) {
        SHR_IF_ERR_EXIT
            (pt_keyed_list_create(unit, sid, attr, pt_map, &entries,
                                  pt->pt_keyed_list));
    }

    /* Create Input Key PTM Field Mappings (FA node cookie) */
    SHR_IF_ERR_EXIT
        (ptm_mapping_create(unit, sid, attr, lt_index_wb_offset_ref,
                            &entries,
                            BCMLTM_FIELD_TYPE_KEY,
                            BCMLTM_FIELD_DIR_IN,
                            KEYS_ALLOC_NONE,
                            &pt->ptm_mapping_keys_in));
    /*
     * Create Output Key PTM Field Mappings (FA node cookie).
     * Input and output field mappings are different on Keyed LTs,
     * but they are the same on Index LTs.
     */
    if (BCMLTM_TABLE_TYPE_LT_KEYED(lt_type)) {
        (ptm_mapping_create(unit, sid, attr, lt_index_wb_offset_ref,
                            &entries,
                            BCMLTM_FIELD_TYPE_KEY,
                            BCMLTM_FIELD_DIR_OUT,
                            KEYS_ALLOC_NONE,
                            &pt->ptm_mapping_keys_out));
    } else {
        pt->ptm_mapping_keys_out = pt->ptm_mapping_keys_in;
    }

    /* Create Required and Optional Key PTM Field Mappings for IwA LTs */
    if (BCMLTM_TABLE_TYPE_LT_ALLOC_INDEX(lt_type)) {
        SHR_IF_ERR_EXIT
            (ptm_mapping_create(unit, sid, attr, lt_index_wb_offset_ref,
                                &entries,
                                BCMLTM_FIELD_TYPE_KEY,
                                BCMLTM_FIELD_DIR_IN,
                                KEYS_ALLOC_REQUIRED,
                                &pt->ptm_mapping_keys_alloc_required));
        SHR_IF_ERR_EXIT
            (ptm_mapping_create(unit, sid, attr, lt_index_wb_offset_ref,
                                &entries,
                                BCMLTM_FIELD_TYPE_KEY,
                                BCMLTM_FIELD_DIR_IN,
                                KEYS_ALLOC_OPTIONAL,
                                &pt->ptm_mapping_keys_alloc_optional));
    }

    /*
     * Create Value PTM field mappings (FA node cookie).
     * Input and output field mappings are the same  for all LTs.
     */
    SHR_IF_ERR_EXIT
        (ptm_mapping_create(unit, sid, attr, lt_index_wb_offset_ref,
                            &entries,
                            BCMLTM_FIELD_TYPE_VALUE,
                            BCMLTM_FIELD_DIR_IN,
                            KEYS_ALLOC_NONE,
                            &pt->ptm_mapping_values));

    /* Create Fixed Field Mappings (used as FA node cookies) */
    SHR_IF_ERR_EXIT
        (fixed_fields_create(unit, sid, &entries,
                             &pt->fixed_fields));

    /* Create and adjust LT Index related node cookies */
    if (BCMLTM_TABLE_TYPE_LT_INDEX(lt_type)) {
        /* FA LT Index (used as FA node cookie on LT Index stage) */
        SHR_IF_ERR_EXIT
            (fa_lt_index_create(unit, sid, attr,
                                &lt_index_wb_offset, pt->pt_list,
                                &pt->fa_lt_index));

        /* EE LT Index (used as EE node cookie on LT Index operations) */
        SHR_IF_ERR_EXIT
            (ee_lt_index_create(unit, sid, &lt_index_wb_offset,
                                &pt->ee_lt_index));

        /* LT to PT Index WBC copy (FA node cookie on LT Index traverse) */
        SHR_IF_ERR_EXIT
            (lt_to_pt_index_create(unit, sid,
                                   pt->pt_list, pt->fa_lt_index,
                                   &pt->lt_to_pt_index));
    }

    /* Create Forward Key Field Transform Information (FA node cookie) */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_info_create(unit, sid, attr,
                                        BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM,
                                        &entries, pt_map,
                                        &pt->fwd_key_xfrm_info_list));

    /* Create Forward Value Field Transform Information (FA node cookie) */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_info_create(unit, sid, attr,
                                        BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM,
                                        &entries, pt_map,
                                        &pt->fwd_value_xfrm_info_list));

    /* Create Reverse Key Field Transform Information (FA node cookie) */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_info_create(unit, sid, attr,
                                        BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM,
                                        &entries, pt_map,
                                        &pt->rev_key_xfrm_info_list));

    /* Create Reverse Value Field Transform Information (FA node cookie) */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_info_create(unit, sid, attr,
                                        BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM,
                                        &entries, pt_map,
                                        &pt->rev_value_xfrm_info_list));

    /* Create LTA Copy Field Information (FA node cookie) */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_copy_info_create(unit, sid,
                                             pt->rev_value_xfrm_info_list,
                                             pt->fwd_value_xfrm_info_list,
                                             &pt->copy_xfrm_info_list));

    *pt_ptr = pt;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lt_pt_info_destroy(pt);
        *pt_ptr = NULL;
    }

    bcmltm_nc_lt_map_entries_pt_cleanup(&entries);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy LT cookies info for PT map groups.
 *
 * Destroy LT cookies info for PT map groups.
 *
 * \param [in] pt Pointer to LT info for PT map groups to destroy.
 */
void
bcmltm_nc_lt_pt_info_destroy(bcmltm_nc_lt_pt_info_t *pt)
{
    if (pt == NULL) {
        return;
    }

    /* Destroy XFRM INFO List */
    bcmltm_nc_lta_xfrm_info_destroy(pt->rev_key_xfrm_info_list);
    bcmltm_nc_lta_xfrm_info_destroy(pt->rev_value_xfrm_info_list);
    bcmltm_nc_lta_xfrm_info_destroy(pt->fwd_key_xfrm_info_list);
    bcmltm_nc_lta_xfrm_info_destroy(pt->fwd_value_xfrm_info_list);
    bcmltm_nc_lta_xfrm_info_destroy(pt->copy_xfrm_info_list);

    /* Destroy LT Index */
    lt_to_pt_index_destroy(pt->lt_to_pt_index);
    ee_lt_index_destroy(pt->ee_lt_index);
    fa_lt_index_destroy(pt->fa_lt_index);

    /* Destroy Fixed Field List Mappings */
    fixed_fields_destroy(pt->fixed_fields);

    /* Destroy Field Mappings for Keys and Values */
    bcmltm_nc_ptm_mapping_destroy(pt->ptm_mapping_values);
    bcmltm_nc_ptm_mapping_destroy(pt->ptm_mapping_keys_alloc_optional);
    bcmltm_nc_ptm_mapping_destroy(pt->ptm_mapping_keys_alloc_required);
    if (pt->ptm_mapping_keys_out != pt->ptm_mapping_keys_in) {
        bcmltm_nc_ptm_mapping_destroy(pt->ptm_mapping_keys_out);
    }
    bcmltm_nc_ptm_mapping_destroy(pt->ptm_mapping_keys_in);

    /* Destroy PT Keyed list */
    pt_keyed_list_destroy(pt->pt_keyed_list);

    /* Destroy PT list */
    pt_list_destroy(pt->pt_list);

    SHR_FREE(pt);

    return;
}

