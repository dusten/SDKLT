/*! \file uft_device.c
 *
 * UFT device specific info
 *
 * This file contains device specific info which are internal to
 * UFT Resource Manager
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
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "uft_device.h"
#include "uft_internal.h"
#include "uft_util.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_UFT

/*******************************************************************************
 * Private variables
 */
static uft_dev_info_t* uft_dev_info[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uft_driver_t* uft_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};


/*******************************************************************************
 * Private functions
 */
static int
uft_reg_info_update(int unit, bcmltd_sid_t lt_id, uint32_t trans_id,
                    uft_regs_info_t *regs)
{
    uft_hw_map_t *reg_map = NULL;
    uint32_t entry[UFT_REGS_ENTRY_WORDS_MAX];
    int idx, f_idx;

    SHR_FUNC_ENTER(unit);
    sal_memset(entry, 0, sizeof(entry));

    for (idx = 0; idx < regs->cnt; idx++) {
        reg_map = &regs->reg_map[idx];
        SHR_IF_ERR_EXIT
            (bcmptm_uft_lt_hw_read(unit, lt_id, trans_id,
                                   reg_map->sid, 0, entry));
        for (f_idx = 0; f_idx < reg_map->f_cnt; f_idx++) {
            bcmdrd_pt_field_set(unit, reg_map->sid, entry,
                                reg_map->f_ids[f_idx],
                                &reg_map->f_data[f_idx]);
        }
        SHR_IF_ERR_EXIT
            (bcmptm_uft_lt_hw_write(unit, lt_id, trans_id,
                                    reg_map->sid, 0, entry));
    }
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

uft_dev_info_t *
bcmptm_uft_device_info_get(int unit)
{
    return UFT_DEV(unit);
}

uft_driver_t *
bcmptm_uft_driver_get(int unit)
{
    return UFT_DRV(unit);
}

int
bcmptm_uft_regs_init(int unit)
{
    uft_hw_map_t *reg_map = NULL;
    uint32_t entry[UFT_REGS_ENTRY_WORDS_MAX];
    int idx, f_idx;
    uft_regs_info_t regs;

    SHR_FUNC_ENTER(unit);

    sal_memset(&regs, 0, sizeof(regs));

    SHR_IF_ERR_EXIT
        (UFT_DRV_EXEC(unit, regs_init)(unit, &regs));

    for (idx = 0; idx < regs.cnt; idx++) {
        reg_map = &regs.reg_map[idx];
        SHR_IF_ERR_EXIT
            (bcmptm_uft_lt_ireq_read(unit, DEVICE_EM_BANK_INFOt,
                                     reg_map->sid, 0, entry));
        for (f_idx = 0; f_idx < reg_map->f_cnt; f_idx++) {
            bcmdrd_pt_field_set(unit, reg_map->sid, entry,
                                reg_map->f_ids[f_idx],
                                &reg_map->f_data[f_idx]);
        }
        SHR_IF_ERR_EXIT
            (bcmptm_uft_lt_ireq_write(unit, DEVICE_EM_BANK_INFOt,
                                      reg_map->sid, reg_map->index, entry));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_device_info_init(int unit, uft_dev_info_t *dev)
{
    return UFT_DRV_EXEC(unit, device_info_init)(unit, dev);
}

int
bcmptm_uft_tbl_em_ctrl_validate(int unit, uft_tbl_em_ctrl_t *ctrl)
{
    return UFT_DRV_EXEC(unit, tbl_em_ctrl_validate)(unit, ctrl);
}

int
bcmptm_uft_dev_em_grp_validate(int unit, uft_dev_em_grp_t *grp)
{
    uft_dev_info_t *drv = UFT_DEV(unit);
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(UFT_DRV_EXEC(unit, dev_em_grp_validate)(unit, grp, drv));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_grp_set(int unit, bcmltd_sid_t lt_id, uint32_t trans_id,
                          uft_dev_em_grp_t *grp,
                          bcmptm_rm_hash_pt_chg_t *pts_info)
{
    uft_dev_info_t *drv = UFT_DEV(unit);
    uft_regs_info_t regs;
    int idx;
    uft_bank_bmp_t req_bmp, temp_bmp;

    SHR_FUNC_ENTER(unit);
    sal_memset(req_bmp.bk_bmp, 0, sizeof(req_bmp));
    sal_memset(temp_bmp.bk_bmp, 0, sizeof(temp_bmp));
    sal_memset(&regs, 0, sizeof(regs));

    for (idx = 0; idx < grp->bank_cnt; idx++) {
        SHR_BITSET(req_bmp.bk_bmp, grp->bank[idx]);
    }

    if (SHR_BITEQ_RANGE(req_bmp.bk_bmp,
                        drv->grp_c_bmp_arr[grp->grp_id].bk_bmp,
                        0, UFT_PHY_BANKS_MAX)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_BITOR_RANGE(drv->grp_c_bmp_arr[grp->grp_id].bk_bmp,
                    drv->free_bk_bmp,
                    0, UFT_PHY_BANKS_MAX, temp_bmp.bk_bmp);
    SHR_BITAND_RANGE(temp_bmp.bk_bmp, req_bmp.bk_bmp,
                     0, UFT_PHY_BANKS_MAX, temp_bmp.bk_bmp);
    if (!SHR_BITEQ_RANGE(req_bmp.bk_bmp, temp_bmp.bk_bmp,
                         0, UFT_PHY_BANKS_MAX)) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }
    SHR_IF_ERR_EXIT
        (UFT_DRV_EXEC(unit, dev_em_grp_set)(unit, grp, UFT_DEV(unit),
                                            &regs, pts_info));

    SHR_IF_ERR_EXIT
        (uft_reg_info_update(unit, lt_id, trans_id, &regs));

    SHR_BITOR_RANGE(drv->free_bk_bmp,
                    drv->grp_c_bmp_arr[grp->grp_id].bk_bmp,
                    0, UFT_PHY_BANKS_MAX,
                    drv->free_bk_bmp);
    SHR_BITREMOVE_RANGE(drv->free_bk_bmp,
                        req_bmp.bk_bmp, 0, UFT_PHY_BANKS_MAX,
                        drv->free_bk_bmp);
    SHR_BITCOPY_RANGE(drv->grp_c_bmp_arr[grp->grp_id].bk_bmp, 0,
                      req_bmp.bk_bmp, 0, UFT_PHY_BANKS_MAX);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_dev_em_bank_validate(int unit, uft_dev_em_bank_t *bank)
{
    return UFT_DRV_EXEC(unit, dev_em_bank_validate)(unit, bank);
}

int
bcmptm_uft_dev_em_bank_set(int unit, bcmltd_sid_t lt_id, uint32_t trans_id,
                           uft_dev_em_bank_t *bank,
                           bcmptm_rm_hash_pt_chg_t *pt_chg)
{
    uft_regs_info_t regs;
    uft_dev_info_t *dev;
    uft_grp_info_t *grp_info;
    int idx, id;
    uint32_t pbank;
    uft_dev_em_grp_t grp;

    SHR_FUNC_ENTER(unit);

    sal_memset(&regs, 0, sizeof(regs));
    dev = UFT_DEV(unit);
    SHR_IF_ERR_EXIT
        (UFT_DRV_EXEC(unit, dev_em_bank_set)(unit, bank, UFT_DEV(unit), &regs));

    SHR_IF_ERR_EXIT
        (uft_reg_info_update(unit, lt_id, trans_id, &regs));

    for (idx = 0; idx < dev->max_grps; idx++) {
        if (SHR_BITGET(dev->grp_c_bmp_arr[idx].bk_bmp, bank->bank_id)) {
            grp_info = dev->grp_info + idx;
            sal_memset(&grp, 0, sizeof(grp));
            grp.grp_id = grp_info->grp_id;
            SHR_IF_ERR_EXIT(uft_lt_grp_get_from_imm(unit, &grp));
            pt_chg->ptsid = grp_info->ovly_ptsid;
            pt_chg->num_lbanks = grp.bank_cnt;
            pt_chg->vector_type = grp.vector_type;
            for (id = 0; id < grp_info->fixed_cnt; id++) {
                pbank = grp_info->fixed_banks[id];
                pt_chg->bank_offset[id] = dev->bank_offset[pbank];
            }
            for (id = 0; id < grp.bank_cnt; id++) {
                pbank = grp.bank[id];
                pt_chg->bank_offset[id + grp_info->fixed_cnt] = dev->bank_offset[pbank];
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_device_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    UFT_ALLOC(UFT_DEV(unit), sizeof(uft_dev_info_t), "UFT dev info");
    sal_memset(UFT_DEV(unit), 0, sizeof(uft_dev_info_t));

    UFT_ALLOC(UFT_DRV(unit), sizeof(uft_driver_t), "UFT driver");
    sal_memset(UFT_DRV(unit), 0, sizeof(uft_driver_t));

    SHR_IF_ERR_EXIT(bcmptm_uft_driver_register(unit, UFT_DRV(unit)));
    SHR_IF_ERR_EXIT(bcmptm_uft_device_info_init(unit, UFT_DEV(unit)));

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_uft_device_cleanup(int unit)
{
    if (UFT_DEV(unit)) {
        SHR_FREE(UFT_DEV(unit)->grp_a_bmp_arr);
        SHR_FREE(UFT_DEV(unit)->grp_c_bmp_arr);

        SHR_FREE(UFT_DEV(unit));
    }
    SHR_FREE(UFT_DRV(unit));
}
