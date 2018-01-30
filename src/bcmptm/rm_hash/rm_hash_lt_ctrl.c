/*! \file rm_hash_lt_ctrl.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the resource manager for physical hash tables
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "rm_hash_internal.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */

static rm_hash_dev_lt_ctrl_t *dev_lt_ctrl[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief This function allocates required lt_ctrl space for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_alloc(int unit)
{
    uint16_t num_ltids = 0;
    uint32_t req_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_num_ltids_get(unit, &num_ltids));

    req_size = (num_ltids * sizeof(rm_hash_lt_ctrl_t)) + sizeof(rm_hash_dev_lt_ctrl_t);
    dev_lt_ctrl[unit] = sal_alloc(req_size, "hash lt ctrl info");
    if (dev_lt_ctrl[unit] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    sal_memset(dev_lt_ctrl[unit], 0, req_size);
    dev_lt_ctrl[unit]->cnt = num_ltids;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function free allocated lt_ctrl space for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (dev_lt_ctrl[unit] != NULL) {
        sal_free(dev_lt_ctrl[unit]);
        dev_lt_ctrl[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure lt_ctrl for a specific hash table ltid
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to lt control object of the ltid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_set(int unit, rm_hash_lt_ctrl_t *lt_ctrl)
{
    uint16_t curr = 0, idx = 0, k = 0, num_move = 0;

    SHR_FUNC_ENTER(unit);

    curr = dev_lt_ctrl[unit]->curr;
    for (idx = 0; idx < curr; idx++) {
        if (lt_ctrl->ltid < dev_lt_ctrl[unit]->array[idx].ltid) {
            break;
        }
    }
    num_move = curr - idx;
    for (k = curr - 1; num_move > 0; k--, num_move--) {
        dev_lt_ctrl[unit]->array[k + 1] = dev_lt_ctrl[unit]->array[k];
    }
    dev_lt_ctrl[unit]->array[idx] = *lt_ctrl;
    dev_lt_ctrl[unit]->curr++;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if a SID has been registered in lt_ctrl.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table sid.
 * \param [in] lt_ctrl Pointer to lt control object of the ltid.
 *
 * \retval TRUE This SID has been registered.
 * \retval FLASE This SID has not been registered.
 */
static bool
rm_hash_lt_ctrl_grp_sid_registered(int unit,
                                   bcmdrd_sid_t sid,
                                   rm_hash_lt_ctrl_t *lt_ctrl)
{
    uint8_t idx;

    for (idx = 0; idx < lt_ctrl->num_grp; idx++) {
        if (lt_ctrl->grp_info[idx].sid == sid) {
            return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_hash_lt_ctrl_grp_info_get(int unit,
                                    bcmdrd_sid_t sid,
                                    rm_hash_lt_ctrl_t *lt_ctrl,
                                    rm_hash_grp_info_t **grp_info)
{
    uint8_t idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < lt_ctrl->num_grp; idx++) {
        if (lt_ctrl->grp_info[idx].sid == sid) {
            *grp_info = &lt_ctrl->grp_info[idx];
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_get(int unit,
                           bcmltd_sid_t ltid,
                           rm_hash_lt_ctrl_t **plt_ctrl)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t num_ltids = dev_lt_ctrl[unit]->cnt;
    uint16_t start = 0, end = 0, idx = 0, mid = 0;

    SHR_FUNC_ENTER(unit);

    end = num_ltids - 1;
    while (idx < num_ltids) {
        mid = (start + end) / 2;
        lt_ctrl = dev_lt_ctrl[unit]->array + mid;
        if (ltid == lt_ctrl->ltid) {
            *plt_ctrl = lt_ctrl;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else if (ltid < lt_ctrl->ltid) {
            end = mid - 1;
            if ((mid == 0) || (end < start)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
        } else {
            start = mid + 1;
            if ((mid == num_ltids) || (start > end)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
        }
        idx++;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_add(int unit, bcmltd_sid_t ltid)
{
    bcmptm_rm_hash_lt_info_t *lt_info;
    uint16_t data_type_list[RM_HASH_MAX_BANK_COUNT], *data_type = NULL;
    int ent_info_idx;
    uint32_t idx;
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;
    rm_hash_pt_reg_info_t *pt_reg_info = NULL;
    rm_hash_lt_ctrl_t lt_ctrl;
    uint8_t hei_cnt = 0, hei_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {0};
    uint8_t info_idx = 0, hei_idx;
    bcmdrd_sid_t sid, grp_sid, ovly_sid = INVALIDm;
    rm_hash_grp_info_t *grp_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_ctrl, 0, sizeof(lt_ctrl));
    lt_ctrl.ltid = ltid;
    lt_ctrl.move_depth = BCMPTM_RM_HASH_DEF_HASH_REORDERING_DEPTH;
    data_type = &data_type_list[0];
    lt_info = &(lt_ctrl.lt_info);
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_ptrm_info_get(unit,
                                      ltid,
                                      0,
                                      FALSE,
                                      lt_info,
                                      &ent_info_idx,
                                      &data_type));
    if (lt_info->rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    for (idx = 0; idx < lt_info->rm_more_info_count; idx++) {
        more_info = lt_info->rm_more_info + idx;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_given_key_ent_info_get(unit,
                                                   lt_info,
                                                   more_info,
                                                   &hei_cnt,
                                                   hei_list));
        for (info_idx = 0; info_idx < hei_cnt; info_idx++) {
            hei = lt_info->hw_entry_info + hei_list[info_idx];
            sid = hei->sid[0];
            ovly_sid = INVALIDm;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ovly_sid_get(unit, sid, &ovly_sid));
            if (ovly_sid != INVALIDm) {
                grp_sid = ovly_sid;
            } else {
                grp_sid = sid;
            }
            if (rm_hash_lt_ctrl_grp_sid_registered(unit,
                                                   grp_sid,
                                                   &lt_ctrl) == FALSE) {
                uint8_t grp_idx = 0;
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_pt_reg_info_get(unit,
                                                    grp_sid,
                                                    &pt_reg_info));
                grp_idx = lt_ctrl.num_grp;
                lt_ctrl.grp_info[grp_idx].sid = grp_sid;
                lt_ctrl.grp_info[grp_idx].pt_info = pt_reg_info->pt_info;
                sal_memcpy(lt_ctrl.grp_info[grp_idx].vhei_idx, hei_list, hei_cnt);
                lt_ctrl.grp_info[grp_idx].num_vhei = hei_cnt;
                lt_ctrl.num_grp++;
            } else {
                /* Check if entry info needs to be updated. */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_lt_ctrl_grp_info_get(unit,
                                                         grp_sid,
                                                         &lt_ctrl,
                                                         &grp_info));
                for (hei_idx = 0; hei_idx < grp_info->num_vhei; hei_idx++) {
                    if (grp_info->vhei_idx[hei_idx] == hei_list[info_idx]) {
                        break;
                    }
                }
                if (hei_idx == grp_info->num_vhei) {
                    grp_info->vhei_idx[grp_info->num_vhei] = hei_list[info_idx];
                    grp_info->num_vhei++;
                }
            }
        }
    }

    SHR_IF_ERR_EXIT
        (rm_hash_lt_ctrl_set(unit, &lt_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(rm_hash_lt_ctrl_alloc(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(rm_hash_lt_ctrl_free(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_dump(int unit, bcmltd_sid_t ltid)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_pt_info_t *pt_info = NULL;
    uint8_t lbank = 0;
    uint8_t idx = 0, grp_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit,
                                    ltid,
                                    &lt_ctrl));

    for (grp_idx = 0; grp_idx < lt_ctrl->num_grp; grp_idx++) {
        pt_info = lt_ctrl->grp_info[grp_idx].pt_info;
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\n*******************************************\n"
                                "ltid:[%d], \n"
                                "hash type:[%d], \n"
                                "bucket mode flags:[%d], \n"
                                "num_lbanks:[%d], \n"
                                "num_slbs:[%d], \n"
                                "slb_size:[%d], \n"
                                "slb_state_hdl:[%d], \n"
                                "ent_node_hdl:[%d], \n"
                                "slb_state:[%p], \n"
                                "ent_node:[%p], \n"
                                "ent_ctr_offset:[%d]\n"),
                                ltid,
                                pt_info->hash_type,
                                pt_info->bmf,
                                pt_info->num_lbanks,
                                pt_info->num_slbs,
                                pt_info->slb_size,
                                pt_info->b_state_hdl,
                                pt_info->e_node_hdl,
                                (void*)pt_info->b_state,
                                pt_info->e_node,
                                pt_info->e_ctr_offset));

        for (lbank = 0; lbank < pt_info->num_lbanks; lbank++) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "lbank:[%d], "
                                    "lbank_size:[%d], "
                                    "lbank_base:[%d] \n"),
                                    pt_info->lbank_list[lbank],
                                    pt_info->lbank_size[lbank],
                                    pt_info->lbank_base[lbank]));
        }
        for (idx = 0; idx < RM_HASH_MAX_KEY_TYPE_COUNT; idx++) {
            if (pt_info->key_attrib[idx].valid == TRUE) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "key type:[%d], "
                                        "valid:[%d], "
                                        "bkt_mode:[%d]\n"),
                                        idx,
                                        pt_info->key_attrib[idx].valid,
                                        pt_info->key_attrib[idx].bm));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
