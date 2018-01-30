/*! \file bcmltm_wb_lt_index.c
 *
 * Logical Table Manager Working Buffer for LT Index.
 *
 * This file contains routines to manage the working buffer block
 * used for operations that require to assemble data for the LT index.
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

#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_lt_index_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE       BSL_LS_BCMLTM_METADATA

/* Size (in words) of one information element (uint32_t) */
#define  INDEX_WSIZE         1


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get working buffer LT index offsets.
 *
 * This function gets the LT index offsets with specified base offset.
 *
 * \param [in] base_offset Base offset of LT index.
 * \param [out] offset Returning LT index offsets.
 *
 * \retval Size, in words, of LT index section.
 */
static uint32_t
lt_index_offset_get(uint32_t base_offset,
                    bcmltm_wb_lt_index_offset_t *offset)
{
    uint32_t wsize = 0;

    /*
     * WB LT index section layout
     *
     *     +----------------------+
     *     | Track Index          |
     *     |                      |
     *     |----------------------|
     *     | Index Absent         |
     *     |                      |
     *     +----------------------+
     */

    sal_memset(offset, 0, sizeof(*offset));

    offset->track = base_offset;
    offset->index_absent = offset->track + INDEX_WSIZE;

    wsize = INDEX_WSIZE * 2;

    return wsize;
}


/*!
 * \brief Get working buffer size for a LT index section.
 *
 * This function gets the working buffer size of the LT index section.
 *
 * \retval Size, in words, of a LT index section.
 */
static int
lt_index_wsize_get(void)
{
    bcmltm_wb_lt_index_offset_t offset;

    return lt_index_offset_get(0, &offset);
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_block_lt_index_add(int unit,
                             bcmltm_wb_handle_t *handle,
                             bcmltm_wb_block_id_t *block_id,
                             bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t block_wsize;

    SHR_FUNC_ENTER(unit);

    /*
     * Working Buffer LT Index Layout
     *
     *    +----------------------+
     *    | Index                |
     *    |                      |
     *    +----------------------+
 */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Sections sizes */
    block_wsize = lt_index_wsize_get();

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize,
                                BCMLTM_WB_BLOCK_TYPE_LT_INDEX, handle);
    if (block == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate LT Index "
                              "working buffer block: "
                              "sid=%d\n"),
                   handle->sid));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }


    /* Fill block information */
    (void)lt_index_offset_get(block->base_offset,
                              &block->u.lt_index.offset.index);

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_wb_lt_index_offset_get(const bcmltm_wb_block_t *wb_block,
                              bcmltm_wb_lt_index_offset_t *offset)
{
    /* Check for valid block type */
    if (wb_block->type != BCMLTM_WB_BLOCK_TYPE_LT_INDEX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer block type: "
                            "expected=%d actual=%d\n"),
                   BCMLTM_WB_BLOCK_TYPE_LT_INDEX, wb_block->type));
        return SHR_E_INTERNAL;
    }

    sal_memcpy(offset, &wb_block->u.lt_index.offset.index, sizeof(*offset));

    return SHR_E_NONE;
}

