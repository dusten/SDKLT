/*! \file uft.c
 *
 * Unified format table management.
 *
 * This file contains the main function for UFT LT.
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "uft_internal.h"
#include "uft_device.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_UFT

/*******************************************************************************
 * Private variables
 */
/*! Fields array handler. */
static shr_famm_hdl_t rm_hash_fld_arr_hdl[BCMDRD_CONFIG_MAX_UNITS];

static int
uft_dev_em_bank_lt_init(int unit, uft_dev_info_t *dev)
{
    bcmltd_fields_t in_flds;
    int f_cnt, bank_id;

    SHR_FUNC_ENTER(unit);

    in_flds.field = shr_famm_alloc(rm_hash_fld_arr_hdl[unit], 20);
    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    for (bank_id = 1; bank_id < dev->max_banks; bank_id++) {
        f_cnt = 0;
        in_flds.field[f_cnt]->id = DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf;
        in_flds.field[f_cnt++]->data = bank_id;
        in_flds.field[f_cnt]->id = DEVICE_EM_BANKt_OFFSETf;
        in_flds.field[f_cnt++]->data = dev->bank_offset[bank_id];
        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_BANKt, &in_flds));
    }

exit:
    shr_famm_free(rm_hash_fld_arr_hdl[unit], in_flds.field, 20);
    SHR_FUNC_EXIT();
}

static int
uft_dev_em_bank_info_lt_init(int unit, uft_dev_info_t *dev)
{
    bcmltd_fields_t in_flds;
    uint32_t f_cnt = 0;
    int idx, grp_idx;
    uft_bank_info_t *bank_info = NULL;

    SHR_FUNC_ENTER(unit);

    in_flds.field = shr_famm_alloc(rm_hash_fld_arr_hdl[unit], 20);
    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    for (idx = 1; idx < dev->max_banks; idx++) {
        f_cnt = 0;
        bank_info = &(dev->bank_info[idx]);
        in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf;
        in_flds.field[f_cnt++]->data = bank_info->bank_id;
        in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_GROUP_CNTf;
        in_flds.field[f_cnt++]->data = bank_info->grp_cnt;
        for (grp_idx = 0; grp_idx < bank_info->grp_cnt; grp_idx++) {
            in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_DEVICE_EM_GROUP_IDf;
            in_flds.field[f_cnt]->data = bank_info->grps[grp_idx];
            in_flds.field[f_cnt++]->idx = grp_idx;
        }
        in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_BASE_ENTRY_WIDTHf;
        in_flds.field[f_cnt++]->data = bank_info->base_entry_width;
        in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_BASE_BUCKET_WIDTHf;
        in_flds.field[f_cnt++]->data = bank_info->base_buket_width;
        in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_NUM_BASE_BUCKETSf;
        in_flds.field[f_cnt++]->data = bank_info->num_base_buckets;
        in_flds.field[f_cnt]->id = DEVICE_EM_BANK_INFOt_NUM_BASE_ENTRIESf;
        in_flds.field[f_cnt++]->data = bank_info->num_base_entries;
        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_BANK_INFOt, &in_flds));
    }

exit:
    shr_famm_free(rm_hash_fld_arr_hdl[unit], in_flds.field, 20);
    SHR_FUNC_EXIT();
}


static int
uft_dev_em_grp_lt_init(int unit, uft_dev_info_t *dev)
{
    bcmltd_fields_t in_flds;
    uint32_t f_cnt = 0;
    int idx, bank_idx;
    uft_grp_info_t *grp = NULL;


    SHR_FUNC_ENTER(unit);

    in_flds.field = shr_famm_alloc(rm_hash_fld_arr_hdl[unit], 20);
    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    for (idx = 1; idx < dev->max_grps; idx++) {
        f_cnt = 0;
        grp = &(dev->grp_info[idx]);
        in_flds.field[f_cnt]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
        in_flds.field[f_cnt++]->data = grp->grp_id;
        in_flds.field[f_cnt]->id = DEVICE_EM_GROUPt_VECTOR_TYPEf;
        in_flds.field[f_cnt++]->data = grp->vector_type;
        in_flds.field[f_cnt]->id = DEVICE_EM_GROUPt_NUM_FIXED_BANKSf;
        in_flds.field[f_cnt++]->data = grp->fixed_cnt;
        for (bank_idx = 0; bank_idx < grp->fixed_cnt; bank_idx++) {
            in_flds.field[f_cnt]->id = DEVICE_EM_GROUPt_FIXED_DEVICE_EM_BANK_IDf;
            in_flds.field[f_cnt]->data = grp->fixed_banks[bank_idx];
            in_flds.field[f_cnt++]->idx = bank_idx;
        }
        in_flds.count = f_cnt;
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, DEVICE_EM_GROUPt, &in_flds));
    }

exit:
    shr_famm_free(rm_hash_fld_arr_hdl[unit], in_flds.field, 20);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize UFT info
 *
 * \param [in] unit Device unit.
 *
 * \return SHR_E_XXX
 */
static int
uft_info_init(int unit)
{
    uft_dev_info_t *dev_info;
    int idx;
    bcmptm_cth_uft_be_grp_info_t *grps = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmptm_uft_device_init(unit));
    dev_info = bcmptm_uft_device_info_get(unit);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    SHR_IF_ERR_EXIT(bcmptm_uft_imm_register_init(unit));
    SHR_IF_ERR_EXIT(uft_dev_em_bank_info_lt_init(unit, dev_info));
    SHR_IF_ERR_EXIT(uft_dev_em_bank_lt_init(unit, dev_info));
    SHR_IF_ERR_EXIT(uft_dev_em_grp_lt_init(unit, dev_info));

    SHR_IF_ERR_EXIT(bcmptm_cth_uft_be_init(unit, dev_info->max_grps, FALSE));
    SHR_IF_ERR_EXIT(bcmptm_cth_uft_grp_info_get(unit, &grps));
    for (idx = 0; idx < dev_info->max_grps; idx++) {
        grps->ptsid[idx] = dev_info->grp_info[idx].ovly_ptsid;
    }

exit:
    SHR_FUNC_EXIT();

}

static void
uft_info_cleanup(int unit)
{
    bcmptm_cth_uft_be_cleanup(unit, FALSE);
    bcmptm_uft_device_cleanup(unit);
}


/*******************************************************************************
 * Public Functions
 */


int
bcmptm_uft_init(int unit)
{
    int rv = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, DEVICE_EM_GROUPt, &map);
    if (!(SHR_SUCCESS(rv) && map)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (!rm_hash_fld_arr_hdl[unit]) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(UFT_FIELD_COUNT_MAX,
                               &rm_hash_fld_arr_hdl[unit]));
    }

    SHR_IF_ERR_EXIT(uft_info_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_uft_regs_init(unit));

exit:
    if (rm_hash_fld_arr_hdl[unit]) {
        shr_famm_hdl_delete(rm_hash_fld_arr_hdl[unit]);
        rm_hash_fld_arr_hdl[unit] = 0;
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_cleanup(int unit)
{
    int rv = SHR_E_NONE;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, DEVICE_EM_GROUPt, &map);
    if (!(SHR_SUCCESS(rv) && map)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    uft_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

