/*! \file bcmltx_field_demux.c
 *
 * Field demux handler
 *
 * Built-in Transform implementation to split or recombine API fields
 * when they do not map directly to PT fields.
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

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmltx_field_demux.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_COMMON

int
bcmltx_field_demux_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t field_ix, field_num, rfield_ix, rfield_num;
    uint64_t src_lta_data = 0;
    bcmltd_field_t *src_lta_field;
    const bcmltd_field_list_t *src_list;
    const bcmltd_field_list_t *dst_list;
    const bcmltd_field_desc_t *src_field;
    const bcmltd_field_desc_t *cur_subfield;
    bcmltd_field_t      *cur_lta_field;
    uint32_t src_field_id, src_field_idx, src_minbit, src_maxbit;
    uint32_t bitwidth;
    uint64_t bitmask;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_demux_transform \n")));

    if (arg->fields != 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* src(logical)  : in (field)  */
    /* dst(physical) : out(rfield) */
    src_list = arg->field_list;
    dst_list = arg->rfield_list;

    src_field_id = arg->field[0];
    src_field_idx = 0;

    field_num = in->count;
    if (field_num == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Search LTA input list for matching field */
    for (field_ix = 0; field_ix < field_num; field_ix++) {
        src_lta_field = in->field[field_ix];
        if ((src_lta_field->id == src_field_id) &&
            (src_lta_field->idx == src_field_idx)) {
            break;
        }
    }

    if (field_ix == field_num) {
        /* Field not found in LTA input list */
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    rfield_ix = 0;
    rfield_num = out->count;
    for (field_ix = 0; field_ix < src_list->field_num; field_ix++) {

        src_field = &(src_list->field_array[field_ix]);
        src_field_id  = src_field->field_id;
        src_field_idx = src_field->field_idx;
        src_minbit = src_field->minbit;
        src_maxbit = src_field->maxbit;
        bitwidth = src_maxbit - src_minbit + 1;
        if ((bitwidth <= 0) || (bitwidth > 64)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else if (bitwidth == 64) {
            bitmask = ~0ULL;
        } else {
            bitmask = (1ULL << bitwidth) - 1;
        }
        src_lta_data = (src_lta_field->data >> src_minbit) & bitmask;

        if (rfield_ix >= rfield_num) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        cur_subfield = &(dst_list->field_array[field_ix]);

        cur_lta_field = out->field[rfield_ix];
        cur_lta_field->id = cur_subfield->field_id;
        cur_lta_field->idx = cur_subfield->field_idx;
        cur_lta_field->data = src_lta_data;

        rfield_ix++;
    }

    out->count = rfield_ix;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_field_mux_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg)
{
    uint32_t field_ix, field_num, ifield_ix, ifield_num;
    uint64_t src_value = 0;
    bcmltd_field_t *src_lta_field;
    const bcmltd_field_list_t *src_list;
    const bcmltd_field_list_t *dst_list;
    const bcmltd_field_desc_t *src_field;
    const bcmltd_field_desc_t *cur_subfield;
    bcmltd_field_t *cur_lta_field;
    uint32_t src_minbit, src_maxbit;
    uint64_t dst_data;
    uint32_t bitwidth;
    uint64_t bitmask;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_mux_transform \n")));

    /* src(logical)  : out(rfield) */
    /* dst(physical) : in (field)  */
    src_list = arg->rfield_list;
    dst_list = arg->field_list;

    if (out->count < 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    ifield_num = in->count;
    field_num = dst_list->field_num;

    for (ifield_ix = 0; ifield_ix < ifield_num; ifield_ix++) {

        cur_lta_field = in->field[ifield_ix];

        /* Search subfield list for matching field */
        for (field_ix = 0; field_ix < field_num; field_ix++) {
            cur_subfield = &(dst_list->field_array[field_ix]);
            if ((cur_lta_field->id == cur_subfield->field_id) &&
                (cur_lta_field->idx == cur_subfield->field_idx)) {
                break;
            }
        }

        if (field_ix == field_num) {
            /* Subfield not found in LTA input list */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        dst_data = cur_lta_field->data;

        src_field = &(src_list->field_array[field_ix]);
        src_minbit = src_field->minbit;
        src_maxbit = src_field->maxbit;
        bitwidth = src_maxbit - src_minbit + 1;
        if ((bitwidth <= 0) || (bitwidth > 64)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else if (bitwidth == 64) {
            bitmask = ~0ULL;
        } else {
            bitmask = (1ULL << bitwidth) - 1;
        }
        dst_data = ((dst_data & bitmask) << src_minbit);
        src_value |= dst_data;
    }

    /* Copy value to LTA output field list */
    src_lta_field = out->field[0];
    src_lta_field->data = src_value;

    if (arg->rfields != 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    src_lta_field->id = arg->rfield[0];
    src_lta_field->idx = 0;

    out->count = 1;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
 exit:
    SHR_FUNC_EXIT();
}

