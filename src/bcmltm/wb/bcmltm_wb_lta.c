/*! \file bcmltm_wb_lta.c
 *
 * Logical Table Manager Working Buffer for LTA interfaces.
 *
 * This file contains routines to manage the working buffer blocks
 * used for operations that require to assemble data for the LTA
 * interfaces:
 *     Custom Table Handler
 *     Field Transforms
 *     Field Validations
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
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE       BSL_LS_BCMLTM_METADATA


/* Sizes (in words) for the LTA fields 'param' section */
#define PARAM_WSIZE      (BCMLTM_BYTES2WORDS(sizeof(bcmltd_fields_t)))

/* Sizes (in words) for the LTA fields 'ptrs' section */
#define PTRS_WSIZE(_n)   (BCMLTM_BYTES2WORDS(sizeof(bcmltd_field_t*) * _n))

/* Sizes (in words) for the LTA fields 'field' array section */
#define FIELD_WSIZE(_n)  (BCMLTM_BYTES2WORDS(sizeof(bcmltd_field_t)) * _n)


/* Checks that type is a valid LTA block type */
#define BLOCK_LTA_TYPE_VALID(_type)                        \
    ((_type == BCMLTM_WB_BLOCK_TYPE_LTA_CTH) ||            \
     (_type == BCMLTM_WB_BLOCK_TYPE_LTA_XFRM) ||           \
     (_type == BCMLTM_WB_BLOCK_TYPE_LTA_VAL))


/*
 * Check that block type is valid.  Function exits if invalid.
 */
#define LTA_BLOCK_TYPE_CHECK(_b)                                    \
    do {                                                            \
        if (!BLOCK_LTA_TYPE_VALID(_b->type)) {                      \
            LOG_ERROR(BSL_LOG_MODULE,                               \
                      (BSL_META("LTM invalid LTA working buffer "   \
                                "LTA block type: "                  \
                                "block_type=%d\n"),                 \
                       _b->type));                                  \
            return SHR_E_INTERNAL;                                  \
        }                                                           \
    } while (0)

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get working buffer LTA fields offsets.
 *
 * This function gets the LTA fields offsets for the given number of
 * fields elements (fid, idx) with specified base offset.
 *
 * \param [in] base_offset Base offset of LTA fields section.
 * \param [in] num_fields Number of field elements (fid, idx).
 * \param [out] offset Returning LTA fields offsets.
 *
 * \retval Size, in words, of LTA fields section.
 */
static uint32_t
lta_fields_offset_get(uint32_t base_offset,
                      uint32_t num_fields,
                      bcmltm_wb_lta_fields_offset_t *offset)
{
    uint32_t wsize = 0;
    uint32_t param_wsize;
    uint32_t ptrs_wsize;
    uint32_t field_wsize;

    /*
     * WB LTA fields section layout
     *
     * Memory is allocated for the given number of fields.
     *
     * LTA Fields:
     *     +------------------------------+
     *     | Param:                       |
     *     |     bcmltd_fields_t          |
     *     |                              |
     *     |------------------------------|
     *     | Ptrs:                        |
     *     |     (bcmltd_field_t*) * n    |
     *     |                              |
     *     | Array of pointers to fields  |
     *     |------------------------------|
     *     | Field:                       |
     *     |     bcmltd_field_t * n       |
     *     |                              |
     *     | Array of bcmltm_field_t      |
     *     +------------------------------+
     *
     * Where "n" is the number of field elements (fid, idx).
     */

    sal_memset(offset, 0, sizeof(*offset));

    param_wsize = PARAM_WSIZE;
    ptrs_wsize = PTRS_WSIZE(num_fields);
    field_wsize = FIELD_WSIZE(num_fields);

    offset->param = base_offset;
    if (num_fields > 0) {
        /*
         * If number of fields is zero, nothing is allocated
         * for the 'ptrs' and 'field' array sections.
         */
        offset->ptrs = offset->param + param_wsize;
        offset->field = offset->ptrs + ptrs_wsize;
    }

    wsize = param_wsize + ptrs_wsize + field_wsize;

    return wsize;
}


/*!
 * \brief Get working buffer size for a LTA fields section.
 *
 * This function gets the working buffer size of the LTA fields section
 * for the given number fields.
 *
 * \param [in] num_fields Number of fields.
 *
 * \retval Size, in words, of a LTA fields section.
 */
static int
lta_fields_wsize_get(uint32_t num_fields)
{
    bcmltm_wb_lta_fields_offset_t offset;

    return lta_fields_offset_get(0, num_fields, &offset);
}


/*!
 * \brief Add an LTA block type to the working buffer handle.
 *
 * This function adds an LTA working buffer block required for LTA
 * interfaces such as Custom Table Handlers, Field Transforms and
 * Field Validations.
 *
 * Valid PTM block types are:
 *     BCMLTM_WB_BLOCK_TYPE_LTA_CTH
 *     BCMLTM_WB_BLOCK_TYPE_LTA_XFRM
 *     BCMLTM_WB_BLOCK_TYPE_LTA_VAL
 *
 * \param [in] unit Unit number.
 * \param [in] type Working buffer block type BCMLTM_WB_BLOCK_TYPE_LTA_xxx
 * \param [in] in_num_fields Number of input fields elements (fid, idx).
 * \param [in] out_num_fields Number of output fields elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_wb_block_lta_add(int unit,
                        bcmltm_wb_block_type_t type,
                        uint32_t in_num_fields,
                        uint32_t out_num_fields,
                        bcmltm_wb_handle_t *handle,
                        bcmltm_wb_block_id_t *block_id,
                        bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t in_fields_wsize;
    uint32_t out_fields_wsize;
    uint32_t block_wsize;
    uint32_t in_base_offset;
    uint32_t out_base_offset;

    SHR_FUNC_ENTER(unit);

    /*
     * Working Buffer LTA Block Layout
     *
     *    +----------------------+
     *    | Input Fields         |
     *    |                      |
     *    |----------------------|
     *    | Output Fields        |
     *    |                      |
     *    +----------------------+
     *
     * Each section is large enough to hold the number of fields required
     * for the input and the output fields.
     */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Check for valid block type */
    if (!BLOCK_LTA_TYPE_VALID(type)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM invalid LTA working buffer block type: "
                              "sid=%d block_type=%d\n"),
                   handle->sid, type));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Sections sizes */
    in_fields_wsize = lta_fields_wsize_get(in_num_fields);
    out_fields_wsize = lta_fields_wsize_get(out_num_fields);
    block_wsize = in_fields_wsize + out_fields_wsize;

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize, type, handle);
    if (block == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate LTA IO "
                              "working buffer block: "
                              "sid=%d in_num_fields=%d out_num_fields=%d\n"),
                   handle->sid, in_num_fields, out_num_fields));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Base offsets for sections */
    in_base_offset = block->base_offset;
    out_base_offset = in_base_offset + in_fields_wsize;

    /* Fill block information */
    block->u.lta.in_num_fields = in_num_fields;
    block->u.lta.out_num_fields = out_num_fields;
    (void)lta_fields_offset_get(in_base_offset, in_num_fields,
                                &block->u.lta.offset.in);
    (void)lta_fields_offset_get(out_base_offset, out_num_fields,
                                &block->u.lta.offset.out);

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
int
bcmltm_wb_block_lta_cth_add(int unit,
                            uint32_t num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr)
{
    /*
     * Number of input and output fields are the same
     * for Custom Table Handler.
     */
    return bcmltm_wb_block_lta_add(unit, BCMLTM_WB_BLOCK_TYPE_LTA_CTH,
                                   num_fields, num_fields,
                                   handle, block_id, block_ptr);
}


int
bcmltm_wb_block_lta_xfrm_add(int unit,
                             uint32_t in_num_fields,
                             uint32_t out_num_fields,
                             bcmltm_wb_handle_t *handle,
                             bcmltm_wb_block_id_t *block_id,
                             bcmltm_wb_block_t **block_ptr)
{
    return bcmltm_wb_block_lta_add(unit, BCMLTM_WB_BLOCK_TYPE_LTA_XFRM,
                                   in_num_fields, out_num_fields,
                                   handle, block_id, block_ptr);
}


int
bcmltm_wb_block_lta_val_add(int unit,
                            uint32_t in_num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr)
{
    /* Field validation only requires input fields */
    return bcmltm_wb_block_lta_add(unit, BCMLTM_WB_BLOCK_TYPE_LTA_VAL,
                                   in_num_fields, 0,
                                   handle, block_id, block_ptr);
}


int
bcmltm_wb_lta_fields_offset_get(const bcmltm_wb_block_t *wb_block,
                                int field_type,
                                bcmltm_wb_lta_fields_offset_t *offset)
{
    const bcmltm_wb_lta_fields_offset_t *lta_offset;

    /* Check for valid block type */
    LTA_BLOCK_TYPE_CHECK(wb_block);

    if (field_type == BCMLTM_WB_LTA_INPUT_FIELDS) {
        lta_offset = &wb_block->u.lta.offset.in;
    } else {
        lta_offset = &wb_block->u.lta.offset.out;
    }

    sal_memcpy(offset, lta_offset, sizeof(*lta_offset));

    return SHR_E_NONE;
}


int
bcmltm_wb_lta_field_map_offset_get(const bcmltm_wb_block_t *wb_block,
                                   int field_type,
                                   uint32_t field_idx,
                                   uint32_t *offset)
{
    uint32_t num_fields;
    const bcmltm_wb_lta_fields_offset_t *lta_offset;

    /* Check for valid block type */
    LTA_BLOCK_TYPE_CHECK(wb_block);

    if (field_type == BCMLTM_WB_LTA_INPUT_FIELDS) {
        num_fields = wb_block->u.lta.in_num_fields;
        lta_offset = &wb_block->u.lta.offset.in;
    } else {
        num_fields = wb_block->u.lta.out_num_fields;
        lta_offset = &wb_block->u.lta.offset.out;
    }

    if (field_idx >= num_fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer field index: "
                            "index (%d) exceeds max allocated fields (%d)\n"),
                   field_idx, num_fields));
        return SHR_E_INTERNAL;
    }

    *offset = lta_offset->field + FIELD_WSIZE(field_idx);

    return SHR_E_NONE;
}

