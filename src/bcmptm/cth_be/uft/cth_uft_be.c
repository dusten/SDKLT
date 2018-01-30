/*! \file cth_uft_be.c
 *
 * Backend part of CTH_UFT
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>

/*******************************************************************************
 * Defines
 */
#define CTH_UFT_BE_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)
#define SHR_RETURN  SHR_RETURN_VAL_EXIT

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHUFT

/*******************************************************************************
 * Private variables
 */
static bcmptm_cth_uft_be_grp_info_t* cth_uft_grp[BCMDRD_CONFIG_MAX_UNITS] = {0};


/*******************************************************************************
 * Private Functions
 */
static int
cth_uft_ptsid_to_grpid(int unit, bcmdrd_sid_t ptsid, int *grp_id)
{
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(grp_id, SHR_E_MEMORY);

    for (idx = 0; idx < cth_uft_grp[unit]->grps_cnt; idx++) {
        if (ptsid == cth_uft_grp[unit]->ptsid[idx]) {
            *grp_id = idx;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static int
cth_uft_bank_offset_get(int unit, int bank_id, uint8_t * offset)
{
    uint32_t fid;
    uint64_t fval;
    bcmltd_fields_t input, output;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    input.field  = NULL;
    output.count = UFT_FIELD_COUNT_MAX;
    output.field = NULL;
    SHR_IF_ERR_EXIT(bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_EXIT(bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf;
    input.field[0]->data = bank_id;
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_BANKt, &input, &output));
    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fval = output.field[idx]->data;
        switch (fid) {
            case DEVICE_EM_BANKt_OFFSETf:
                *offset = fval;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                break;
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_uft_lt_fields_buff_alloc(int unit, bcmltd_fields_t *in)
{
    uint32_t idx = 0;
    bcmltd_field_t *f_ptr = NULL;
    size_t cnt;

    SHR_FUNC_ENTER(unit);

    if (!in || (in->count < 1)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    cnt = in->count;
    in->field = sal_alloc(cnt * sizeof(bcmltd_field_t *),
                          "bcmltd field pointer array");
    if (!(in->field)) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    /* Allocate all fields struct at one time. */
    f_ptr = sal_alloc(cnt * sizeof(bcmltd_field_t), "bcmltd field");
    if (!f_ptr) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    sal_memset(f_ptr, 0, cnt * sizeof(bcmltd_field_t));

    for (idx = 0; idx < cnt; idx++) {
        in->field[idx] = f_ptr + idx;
    }

exit:
    if(SHR_FUNC_ERR()) {
        (void) bcmptm_uft_lt_fields_buff_free(unit, in);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_lt_fields_buff_free(int unit, bcmltd_fields_t *in)
{
    SHR_FUNC_ENTER(unit);

    if(!in) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    if (in->field) {
        SHR_FREE(in->field[0]);
        SHR_FREE(in->field);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_lt_move_depth_get(int unit, bcmltd_sid_t ltid,
                                 uint8_t *move_depth)
{
    int rv;
    uint32_t fid;
    uint64_t fval;
    bcmltd_fields_t input, output;
    size_t idx;
    uint32_t num_actual = 0;
    uint64_t def_val = 0;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    input.field  = NULL;
    output.count = UFT_FIELD_COUNT_MAX;
    output.field = NULL;
    SHR_IF_ERR_EXIT(bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_EXIT(bcmptm_uft_lt_fields_buff_alloc(unit, &output));
    input.field[0]->id = TABLE_EM_CONTROLt_TABLE_IDf;
    input.field[0]->data = ltid;
    rv = bcmimm_entry_lookup(unit, TABLE_EM_CONTROLt, &input, &output);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, TABLE_EM_CONTROLt,
                                      TABLE_EM_CONTROLt_MOVE_DEPTHf,
                                      1, &def_val, &num_actual));
        *move_depth = def_val;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fval = output.field[idx]->data;
        switch (fid) {
            case TABLE_EM_CONTROLt_DEVICE_EM_GROUP_IDf:
                break;
            case TABLE_EM_CONTROLt_MOVE_DEPTHf:
                *move_depth = fval;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                break;
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_bank_info_get(int unit, bcmdrd_sid_t ptsid,
                             bcmptm_pt_banks_info_t *banks_info)
{
    uint32_t fid, fidx;
    uint64_t fval;
    bcmltd_fields_t input, output;
    size_t idx;
    int i;
    int grp_id = 0;
    int v_pbank[BCMPTM_UFT_MAX_BANK_COUNT] = {0};
    int f_pbank[BCMPTM_UFT_MAX_BANK_COUNT] = {0};
    int f_bank_cnt = 0, v_bank_cnt = 0;
    uint8_t *offset_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(banks_info, 0, sizeof(bcmptm_pt_banks_info_t));
    input.count  = 1;
    input.field  = NULL;
    output.count = UFT_FIELD_COUNT_MAX;
    output.field = NULL;
    SHR_IF_ERR_EXIT(cth_uft_ptsid_to_grpid(unit, ptsid, &grp_id));
    SHR_IF_ERR_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));
    input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
    input.field[0]->data = grp_id;
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt, &input, &output));

    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fval = output.field[idx]->data;
        fidx = output.field[idx]->idx;
        switch (fid) {
            case DEVICE_EM_GROUPt_ROBUSTf:
                banks_info->robust_en = fval;
                break;
            case DEVICE_EM_GROUPt_VECTOR_TYPEf:
                banks_info->vector_type = fval;
                break;
            case DEVICE_EM_GROUPt_FIXED_DEVICE_EM_BANK_IDf:
                f_pbank[fidx] = fval;
                break;
            case DEVICE_EM_GROUPt_NUM_FIXED_BANKSf:
                f_bank_cnt = fval;
                break;
            case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
                v_pbank[fidx] = fval;
                break;
            case DEVICE_EM_GROUPt_NUM_BANKSf:
                v_bank_cnt = fval;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                break;
        }
    }

    offset_ptr = banks_info->hash_offset;
    for (i = 0; i < f_bank_cnt; i++) {
        SHR_IF_ERR_EXIT
            (cth_uft_bank_offset_get(unit, f_pbank[i], offset_ptr++));
    }
    for (i = 0; i < v_bank_cnt; i++) {
        SHR_IF_ERR_EXIT
            (cth_uft_bank_offset_get(unit, v_pbank[i], offset_ptr++));
    }

    banks_info->bank_cnt = f_bank_cnt + v_bank_cnt;

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_grp_info_get(int unit, bcmptm_cth_uft_be_grp_info_t **grps)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(grps, SHR_E_INTERNAL);
    (*grps) = cth_uft_grp[unit];
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_be_init(int unit, int grps_cnt, bool warm)
{
    int size;
    SHR_FUNC_ENTER(unit);

    size = sizeof(bcmptm_cth_uft_be_grp_info_t);
    CTH_UFT_BE_ALLOC(cth_uft_grp[unit], size, "uft grps info");
    sal_memset(cth_uft_grp[unit], 0, size);
    if (grps_cnt > 0) {
        size = grps_cnt * sizeof(bcmdrd_sid_t);
        CTH_UFT_BE_ALLOC(cth_uft_grp[unit]->ptsid, size, "uft ptsid");
        sal_memset(cth_uft_grp[unit]->ptsid, 0, size);
        cth_uft_grp[unit]->grps_cnt = grps_cnt;
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(cth_uft_grp[unit]);
    }

    SHR_FUNC_EXIT();
}

void
bcmptm_cth_uft_be_cleanup(int unit, bool warm)
{
    if (cth_uft_grp[unit]) {
        SHR_FREE(cth_uft_grp[unit]->ptsid);
    }
    SHR_FREE(cth_uft_grp[unit]);
}
