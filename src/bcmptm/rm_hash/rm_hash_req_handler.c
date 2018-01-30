/*! \file rm_hash_req_handler.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement the basic operations of hash
 * resource manager. More specifically, it includes the routines for LOOKUP,
 * INSERT, DELETE operations.
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
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
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
/*! Per-unit hash search bucket info object */
static rm_hash_srch_bkt_info_t dev_last_lkup_bkt_info[BCMDRD_CONFIG_MAX_UNITS];

/*! Per-unit hash search result info object */
static rm_hash_srch_result_t   dev_last_lkup_result[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Get key type for a hash table physical bank.
 *
 * \param [in] unit Unit number.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] sid_idx SID index of the physical bank.
 * \param [out] key_type Pointer to a buffer recording corresponding key type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_key_type_get(int unit,
                     const bcmptm_rm_hash_more_info_t *more_info,
                     uint8_t sid_idx,
                     uint16_t *key_type)
{
    SHR_FUNC_ENTER(unit);

    if (more_info->key_type_val == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    if (more_info->num_bank_sid == 1) {
        /*
         * For some devices, there is only one key type for all
         * the banks.
         */
        *key_type = more_info->key_type_val[0];
    } else {
        /*
         * For some devices, each physical bank has a separate key type
         * It will be the physical bank number for these device.
         */
        *key_type = more_info->key_type_val[sid_idx];
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get data type for an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ent_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] entry Pointer to entry buffer.
 * \param [out] data_type Pointer to buffer for recording data type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_data_type_get(int unit,
                      const bcmptm_rm_hash_hw_entry_info_t *ent_info,
                      uint32_t *entry,
                      uint16_t *data_type)
{
    uint32_t val[BCMPTM_MAX_PT_FIELD_WORDS] = {0};
    int sbit, ebit;

    SHR_FUNC_ENTER(unit);

    sbit = ent_info->hw_dtype_field_start_bit;
    ebit = ent_info->hw_dtype_field_start_bit + ent_info->hw_dtype_field_width - 1;
    bcmdrd_field_get(entry, sbit, ebit, &val[0]);
    *data_type = (uint16_t)val[0];

    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill in response information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] ent_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] ent_index Index of the entry.
 * \param [in] ent_sid SID of the entry.
 * \param [in] dfield_format_id Data format id of the entry.
 * \param [in] entry Entry buffer.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure for recording
 *              response info for a hash operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_rsp_info_set(int unit,
                     const bcmptm_rm_hash_hw_entry_info_t *ent_info,
                     uint32_t ent_index,
                     bcmdrd_sid_t ent_sid,
                     uint16_t dfield_format_id,
                     uint32_t *entry,
                     bcmptm_rm_hash_rsp_t *rsp_info)
{

    SHR_FUNC_ENTER(unit);

    rsp_info->rsp_entry_index = ent_index;
    rsp_info->rsp_entry_sid = ent_sid;
    rsp_info->rsp_dfield_format_id = dfield_format_id;
    if (ent_info->hw_dtype_field_width != 0) {
        SHR_IF_ERR_EXIT
            (rm_hash_data_type_get(unit,
                                   ent_info,
                                   entry,
                                   &rsp_info->rsp_entry_data_type));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "RM hash rsp info: rsp_entry_index = %d, "
                            "rsp_entry_sid = %s, rsp_dfield_format_id = %d.\n"),
                            rsp_info->rsp_entry_index,
                            bcmdrd_pt_sid_to_name(unit, rsp_info->rsp_entry_sid),
                            rsp_info->rsp_dfield_format_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove an old entry from physical hash table and clear the
 * corresponding software state maintained in rm hash.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_dual_ent_remove(int unit,
                        uint32_t req_flags,
                        bcmbd_pt_dyn_info_t *pt_dyn_info,
                        rm_hash_pt_info_t *pt_info,
                        rm_hash_dual_ent_node_t *e_node)
{
    uint32_t rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS, e_size = 0;
    const uint32_t *n_ent = NULL;
    uint32_t ne_words[BCMPTM_MAX_PT_ENTRY_WORDS] = {0};
    uint8_t lbank = 0;
    uint32_t slb = 0;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};

    SHR_FUNC_ENTER(unit);

    /* Get null entry content and its size of the specified sid */
    n_ent = bcmdrd_pt_default_value_get(unit, e_node->sid);
    e_size = bcmdrd_pt_entry_size(unit, e_node->sid);
    if ((n_ent == NULL) || (e_size == 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    sal_memcpy(ne_words, n_ent, e_size);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Remove an existing entry on mem[%s] index[%d].\n"),
                            bcmdrd_pt_sid_to_name(unit, e_node->sid),
                            e_node->e_idx));
    dyn_info = *pt_dyn_info;
    dyn_info.index = e_node->e_idx;
    /* Update HW and PTcache */
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_flags,
                              0,
                              e_node->sid,
                              &dyn_info,
                              NULL,
                              NULL, /* misc_info */
                              ne_words,
                              TRUE,
                              TRUE,
                              TRUE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              0,
                              0,
                              &rsp_flags));
    /* Update SW state maintained in rm hash */
    lbank = e_node->e_loc.lbank;
    slb   = e_node->slb_info[lbank].slb;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_state_clear(unit, pt_info, slb, &e_node->e_loc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove an old entry from physical hash table and clear the
 * corresponding software state maintained in rm hash.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_shr_ent_remove(int unit,
                       uint32_t req_flags,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       rm_hash_pt_info_t *pt_info,
                       rm_hash_shr_ent_node_t *e_node)
{
    uint32_t rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS, e_size = 0;
    const uint32_t *n_ent = NULL;
    uint32_t ne_words[BCMPTM_MAX_PT_ENTRY_WORDS] = {0};
    uint8_t lbank = 0;
    uint32_t slb = 0;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};

    SHR_FUNC_ENTER(unit);

    /* Get null entry content and its size of the specified sid */
    n_ent = bcmdrd_pt_default_value_get(unit, e_node->sid);
    e_size = bcmdrd_pt_entry_size(unit, e_node->sid);
    if ((n_ent == NULL) || (e_size == 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    sal_memcpy(ne_words, n_ent, e_size);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Remove an existing entry on mem[%s] index[%d].\n"),
                            bcmdrd_pt_sid_to_name(unit, e_node->sid),
                            e_node->e_idx));
    /* Update HW and PTcache */
    dyn_info = *pt_dyn_info;
    dyn_info.index = e_node->e_idx;
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_flags,
                              0,
                              e_node->sid,
                              &dyn_info,
                              NULL,
                              NULL, /* misc_info */
                              ne_words,
                              TRUE,
                              TRUE,
                              TRUE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              0,
                              0,
                              &rsp_flags));
    /* Update SW state maintained in rm hash */
    lbank = e_node->e_loc.lbank;
    slb = e_node->slb_info[lbank].slb;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_state_clear(unit, pt_info, slb, &e_node->e_loc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search an entry in the physical hash table and remove it.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] hei_idx Index of hardware entry info.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] sbkt_info Pointer to rm_hash_srch_bkt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_indirect_remove(int unit,
                            uint32_t req_flags,
                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                            bcmptm_rm_hash_req_t *req_info,
                            uint8_t hei_idx,
                            const bcmptm_rm_hash_lt_info_t *lt_info,
                            rm_hash_grp_info_t *grp_info,
                            const bcmptm_rm_hash_more_info_t *more_info,
                            rm_hash_srch_bkt_info_t *sbkt_info)
{
    rm_hash_srch_bank_info_t sbank_info;
    rm_hash_srch_result_t sresult;
    uint8_t lbank;
    rm_hash_pt_info_t *pt_info = grp_info->pt_info;

    SHR_FUNC_ENTER(unit);

    for (lbank = 0; lbank < pt_info->num_lbanks; lbank++) {
        bcmptm_rm_hash_ent_srch_bank_info_init(unit,
                                               &lbank,
                                               1,
                                               &sbank_info);
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_search(unit,
                                       req_flags,
                                       req_info,
                                       pt_dyn_info,
                                       lt_info,
                                       grp_info,
                                       more_info,
                                       sbkt_info,
                                           &sbank_info,
                                       &sresult));
        if ((sresult.e_exist == TRUE) && (sresult.hei_idx == hei_idx)) {
            if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                SHR_IF_ERR_EXIT
                    (rm_hash_dual_ent_remove(unit,
                                             req_flags,
                                             pt_dyn_info,
                                             pt_info,
                                             &sresult.e_node.de_node));
                break;
            } else {
                SHR_IF_ERR_EXIT
                    (rm_hash_shr_ent_remove(unit,
                                            req_flags,
                                            pt_dyn_info,
                                            pt_info,
                                            &sresult.e_node.se_node));
                break;
            }
        }
    }
    if (lbank == pt_info->num_lbanks) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash delete request handler
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_delete(int unit,
               uint32_t req_flags,
               bcmltd_sid_t req_ltid,
               bcmbd_pt_dyn_info_t *pt_dyn_info,
               bcmptm_rm_hash_req_t *req_info,
               size_t rsp_entry_buf_wsize,
               uint32_t *rsp_entry_words,
               bcmptm_rm_hash_rsp_t *rsp_info,
               bcmltd_sid_t *rsp_ltid,
               uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_srch_bkt_info_t sbkt_info;
    rm_hash_srch_bank_info_t sbank_info;
    rm_hash_srch_result_t sresult;
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;
    size_t e_size;
    rm_hash_grp_info_t *grp_info = NULL;
    bcmdrd_sid_t grp_sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

     /* Get corresponding lt control object */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    /* Get rm more info object corresponding to entry_attrs. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_more_info_get(unit,
                                          req_info,
                                          lt_ctrl,
                                          &more_info));
    SHR_NULL_CHECK(more_info, SHR_E_PARAM);

    grp_info = &lt_ctrl->grp_info[0];

    /* Get software logical bucket list and valid base bucket bitmap */
    if (lt_ctrl->num_grp > 1) {
        /* This LT mapps to multiple physical SIDs */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_group_sid_get(unit,
                                          &lt_ctrl->lt_info,
                                          more_info,
                                          &grp_sid));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_grp_info_get(unit,
                                                grp_sid,
                                                lt_ctrl,
                                                &grp_info));
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_srch_slb_info_get(unit,
                                              req_info,
                                              &lt_ctrl->lt_info,
                                              grp_info->pt_info,
                                              more_info,
                                              &sbkt_info));
    /* Setup search bank info for the following entry search operation */
    bcmptm_rm_hash_ent_srch_bank_info_init(unit,
                                           grp_info->pt_info->lbank_list,
                                           grp_info->pt_info->num_lbanks,
                                           &sbank_info);
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_search(unit,
                                   req_flags,
                                   req_info,
                                   pt_dyn_info,
                                   &lt_ctrl->lt_info,
                                   grp_info,
                                   more_info,
                                   &sbkt_info,
                                   &sbank_info,
                                   &sresult));
    if (sresult.e_exist == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    if (grp_info->pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        SHR_IF_ERR_EXIT
            (rm_hash_dual_ent_remove(unit,
                                     req_flags,
                                     pt_dyn_info,
                                     grp_info->pt_info,
                                     &sresult.e_node.de_node));
        hei = lt_ctrl->lt_info.hw_entry_info + sresult.hei_idx;
        SHR_IF_ERR_EXIT
            (rm_hash_rsp_info_set(unit,
                                  hei,
                                  sresult.e_node.de_node.e_idx,
                                  sresult.e_node.de_node.sid,
                                  sresult.dft_id,
                                  sresult.e_words,
                                  rsp_info));
    } else {
        SHR_IF_ERR_EXIT
            (rm_hash_shr_ent_remove(unit,
                                    req_flags,
                                    pt_dyn_info,
                                    grp_info->pt_info,
                                    &sresult.e_node.se_node));
        hei = lt_ctrl->lt_info.hw_entry_info + sresult.hei_idx;
        SHR_IF_ERR_EXIT
            (rm_hash_rsp_info_set(unit,
                                  hei,
                                  sresult.e_node.se_node.e_idx,
                                  sresult.e_node.se_node.sid,
                                  sresult.dft_id,
                                  sresult.e_words,
                                  rsp_info));
    }
    e_size = (hei->entry_bitsize + 31) / 32;
    if (rsp_entry_buf_wsize < e_size) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    sal_memcpy(rsp_entry_words, sresult.e_words, e_size * 4);
    *rsp_ltid = sresult.ltid;
    *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Replace an hash table entry directly with another entry which has the same
 * width.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] result Pointer to rm_hash_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_replace(int unit,
                    uint32_t req_flags,
                    bcmltd_sid_t req_ltid,
                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                    bcmptm_rm_hash_req_t *req_info,
                    const bcmptm_rm_hash_lt_info_t *lt_info,
                    rm_hash_pt_info_t *pt_info,
                    const bcmptm_rm_hash_more_info_t *more_info,
                    rm_hash_srch_result_t *sresult)
{
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    uint16_t key_type = 0;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};
    uint32_t wr_rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    rm_hash_dual_ent_node_t *de_node = NULL;
    rm_hash_shr_ent_node_t  *se_node = NULL;
    uint8_t lbank = 0;

    SHR_FUNC_ENTER(unit);

    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        de_node = &sresult->e_node.de_node;
        lbank = de_node->e_loc.lbank;
        if (more_info->key_type_val != NULL) {
            SHR_IF_ERR_EXIT
                (rm_hash_key_type_get(unit, more_info, lbank, &key_type));
            hei = lt_info->hw_entry_info + sresult->hei_idx;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_key_type_fill(unit,
                                              key_type,
                                              hei,
                                              req_info->entry_words));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "There exists a matched entry with same size for "
                                "ltid[%d] on mem[%s] index[%d]. "
                                "The existing matched entry will be replaced.\n"),
                                req_ltid,
                                bcmdrd_pt_sid_to_name(unit, de_node->sid),
                                de_node->e_idx));
        /* Use the values provided by caller, except index */
        dyn_info = *pt_dyn_info;
        dyn_info.index = de_node->e_idx;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_write(unit,
                                  req_flags,
                                  0,
                                  de_node->sid,
                                  &dyn_info,
                                  NULL,
                                  NULL, /* misc_info */
                                  req_info->entry_words,
                                  TRUE,
                                  TRUE,
                                  TRUE,
                                  BCMPTM_RM_OP_NORMAL, NULL,
                                  req_ltid,
                                  req_info->dfield_format_id,
                                  &wr_rsp_flags));
    } else {
        se_node = &sresult->e_node.se_node;
        lbank = se_node->e_loc.lbank;
        if (more_info->key_type_val != NULL) {
            SHR_IF_ERR_EXIT
                (rm_hash_key_type_get(unit, more_info, lbank, &key_type));
            hei = lt_info->hw_entry_info + sresult->hei_idx;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_key_type_fill(unit,
                                              key_type,
                                              hei,
                                              req_info->entry_words));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "There exists a matched entry with same size for "
                                "ltid[%d] on mem[%s] index[%d]. "
                                "The existing matched entry will be replaced.\n"),
                                req_ltid,
                                bcmdrd_pt_sid_to_name(unit, se_node->sid),
                                se_node->e_idx));
        /* Use the values provided by caller, except index */
        dyn_info = *pt_dyn_info;
        dyn_info.index = se_node->e_idx;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_write(unit,
                                  req_flags,
                                  0,
                                  se_node->sid,
                                  &dyn_info,
                                  NULL,
                                  NULL, /* misc_info */
                                  req_info->entry_words,
                                  TRUE,
                                  TRUE,
                                  TRUE,
                                  BCMPTM_RM_OP_NORMAL, NULL,
                                  req_ltid,
                                  req_info->dfield_format_id,
                                  &wr_rsp_flags));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a new entry into HW and record the entry in SW
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] ent_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] lbkt_list Pointer to the list of logical buckets.
 * \param [in] lbank Logical bank.
 * \param [in] ent_offset Offset of the first base entry.
 * \param [in] ent_bmp Bitmap of the entry.
 * \param [out] info Pointer to the bcmptm_rm_hash_rsp_t for recording response
 *              info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_install(int unit,
                    uint32_t req_flags,
                    bcmltd_sid_t req_ltid,
                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                    bcmptm_rm_hash_req_t *req_info,
                    const bcmptm_rm_hash_hw_entry_info_t *hei,
                    const bcmptm_rm_hash_more_info_t *more_info,
                    rm_hash_pt_info_t *pt_info,
                    rm_hash_ent_slb_info_t *slb_info_list,
                    rm_hash_bm_t e_bm,
                    rm_hash_ent_loc_t *e_loc,
                    bcmptm_rm_hash_rsp_t *info)
{
    bcmdrd_sid_t sid;
    uint16_t key_type = 0;
    uint32_t e_idx = 0, e_sig = 0;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};
    uint32_t wr_rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    uint8_t lbank;

    SHR_FUNC_ENTER(unit);

    lbank = e_loc->lbank;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_sid_get(unit, more_info, hei, lbank, &sid));
    /*
     * Fill the entry words with the correct key type, if there exists key type
     * field for this entry.
     */
    if (more_info->key_type_val != NULL) {
        SHR_IF_ERR_EXIT
            (rm_hash_key_type_get(unit, more_info, lbank, &key_type));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_type_fill(unit,
                                          key_type,
                                          hei,
                                          req_info->entry_words));
    }
    /* Compute the entry index */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      slb_info_list[lbank].slb,
                                      e_loc,
                                      hei->num_base_entries,
                                      &e_idx));
    dyn_info = *pt_dyn_info;
    dyn_info.index = e_idx;
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_flags,
                              0,
                              sid,
                              &dyn_info,
                              NULL,
                              NULL, /* misc_info */
                              req_info->entry_words,
                              TRUE,
                              TRUE,
                              TRUE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              req_ltid,
                              req_info->dfield_format_id,
                              &wr_rsp_flags));

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "The entry for ltid[%d] will be inserted on "
                           "mem[%s] index[%d].\n"),
                           req_ltid,
                           bcmdrd_pt_sid_to_name(unit, sid),
                           e_idx));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_sig_generate(unit,
                                         more_info,
                                         req_info->entry_words,
                                         &e_sig));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_state_set(unit,
                                      pt_info,
                                      e_loc,
                                      slb_info_list,
                                      sid,
                                      e_idx,
                                      e_sig,
                                      e_bm,
                                      hei->num_base_entries));
    info->rsp_entry_index = e_idx;
    info->rsp_entry_sid = sid;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash insert request handler
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_insert(int unit,
               uint32_t req_flags,
               bcmltd_sid_t req_ltid,
               bcmbd_pt_dyn_info_t *pt_dyn_info,
               bcmptm_rm_hash_req_t *req_info,
               size_t rsp_entry_buf_wsize,
               uint32_t *rsp_entry_words,
               bcmptm_rm_hash_rsp_t *rsp_info,
               bcmltd_sid_t *rsp_ltid,
               uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_srch_bkt_info_t sbkt_info, *psbkt_info = NULL;
    rm_hash_srch_bank_info_t sbank_info;
    rm_hash_srch_result_t result, *presult;
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    uint16_t data_type_val[RM_HASH_MAX_BANK_COUNT], *data_type;
    bcmptm_rm_hash_lt_info_t lt_info;
    int hei_idx = 0;
    bool differ_collision = FALSE, e_loc_found = FALSE, table_full = FALSE;
    uint32_t flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    bcmptm_rm_hash_rsp_t info;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;
    rm_hash_bm_t e_bm;
    rm_hash_ent_loc_t e_loc;
    uint8_t lbank, cnt, idx_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {0};
    bcmdrd_sid_t ptsid, grp_sid;
    const lt_mreq_info_t *lrd_info = NULL;
    rm_hash_grp_info_t *grp_info = NULL;

    rm_hash_dual_ent_node_t *de_node = NULL;
    rm_hash_shr_ent_node_t  *se_node = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&info, 0, sizeof (info));
    /*
     * Get lt control object corresponding to the requested ltid.
     * lt control object is defined on per ltid basis.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    /*
     * Get rm more info object corresponding to entry_attrs.
     * For EM based lt, a ltid will have multiple more info.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_more_info_get(unit,
                                          req_info,
                                          lt_ctrl,
                                          &more_info));
    SHR_NULL_CHECK(more_info, SHR_E_PARAM);

    grp_info = &lt_ctrl->grp_info[0];

    /* Get software logical bucket list and valid base bucket bitmap */
    if (lt_ctrl->num_grp > 1) {
        /* This LT mapps to multiple physical SIDs */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_group_sid_get(unit,
                                          &lt_ctrl->lt_info,
                                          more_info,
                                          &grp_sid));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_grp_info_get(unit,
                                                grp_sid,
                                                lt_ctrl,
                                                &grp_info));
    }

    /*
     * For insert operation, the hardware entry info corresponding to data
     * format id is determined and needs be retrieved.
     */
    data_type = &data_type_val[0];
    /* query for LT info */
    SHR_IF_ERR_EXIT
        (bcmlrd_lt_mreq_info_get(unit, req_ltid, &lrd_info));
    SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
    if ((lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
        (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_given_key_ent_info_get(unit,
                                                   &lt_ctrl->lt_info,
                                                   more_info,
                                                   &cnt,
                                                   idx_list));
        hei = lt_ctrl->lt_info.hw_entry_info + idx_list[0];
        SHR_NULL_CHECK(hei, SHR_E_PARAM);
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_ptrm_info_get(unit,
                                          req_ltid,
                                          req_info->dfield_format_id,
                                          TRUE,
                                          &lt_info,
                                          &hei_idx,
                                          &data_type));
        hei = lt_ctrl->lt_info.hw_entry_info + hei_idx;
        SHR_NULL_CHECK(hei, SHR_E_PARAM);
    }

    pt_info = grp_info->pt_info;

    if (req_info->same_key == TRUE) {
        /*
         * An entry lookup operation has been performed before this insert
         * operation, and the entry physical, logical bucket list and search
         * result has been saved, no matter if there exists matched entry.
         * In this case, the search result will be used directly to avoid
         * hash vector re-computing in the following insert operation.
         */
        psbkt_info = &dev_last_lkup_bkt_info[unit];
        presult    = &dev_last_lkup_result[unit];
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "No LOOKUP preceding entry INSERT!!\n")));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_srch_slb_info_get(unit,
                                                  req_info,
                                                  &lt_ctrl->lt_info,
                                                  pt_info,
                                                  more_info,
                                                  &sbkt_info));
        bcmptm_rm_hash_ent_srch_bank_info_init(unit,
                                               pt_info->lbank_list,
                                               pt_info->num_lbanks,
                                               &sbank_info);
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_search(unit,
                                       req_flags,
                                       req_info,
                                       pt_dyn_info,
                                       &lt_ctrl->lt_info,
                                       grp_info,
                                       more_info,
                                       &sbkt_info,
                                       &sbank_info,
                                       &result));
        psbkt_info = &sbkt_info;
        presult = &result;
    }
    if (presult->e_exist == TRUE) {
        /*
         * The entry with the same key exists. Check if the view width of the
         * existing entry is same as the new entry. If it is same, the existing
         * one can be replaced by direct memory write operation. Otherwise, the
         * new entry needs to be inserted first, then the old entry need to be
         * deleted.
         */
        if (presult->hei_idx == hei_idx) {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_replace(unit,
                                     req_flags,
                                     req_ltid,
                                     pt_dyn_info,
                                     req_info,
                                     &lt_ctrl->lt_info,
                                     pt_info,
                                     more_info,
                                     presult));
            if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                de_node = &presult->e_node.de_node;
                SHR_IF_ERR_EXIT
                    (rm_hash_rsp_info_set(unit,
                                          hei,
                                          de_node->e_idx,
                                          de_node->sid,
                                          req_info->dfield_format_id,
                                          req_info->entry_words,
                                          rsp_info));
            } else {
                se_node = &presult->e_node.se_node;
                SHR_IF_ERR_EXIT
                    (rm_hash_rsp_info_set(unit,
                                          hei,
                                          se_node->e_idx,
                                          se_node->sid,
                                          req_info->dfield_format_id,
                                          req_info->entry_words,
                                          rsp_info));
            }
            *rsp_ltid = req_ltid;
            *rsp_flags = BCMPTM_RSP_FLAGS_REPLACED;
            /*
             * The requested entry has been installed into physical table and
             * corresponding software state has been updated. return now.
             */
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_differ_collision_get(unit, &differ_collision));
    if (differ_collision == TRUE) {
        /* Check if there is enough base entries to insert new entry */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_table_fullness_get(unit,
                                               pt_info,
                                               hei->num_base_entries,
                                               &table_full));
        if (table_full == TRUE) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
    }
    /*
     * If there is no matched entry, or the matched entry has different
     * entry info (entry format). The entry requested will be inserted.
     * Check if there is space in the hash table for it firstly.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_bkt_mode_get(unit,
                                         more_info,
                                         pt_info,
                                         &e_bm));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_space_search(unit,
                                              pt_info,
                                              hei,
                                              psbkt_info->slb_info_list,
                                              e_bm,
                                              &e_loc_found,
                                              &e_loc));
    if (e_loc_found == TRUE) {
        SHR_IF_ERR_EXIT
            (rm_hash_ent_install(unit,
                                 req_flags,
                                 req_ltid,
                                 pt_dyn_info,
                                 req_info,
                                 hei,
                                 more_info,
                                 pt_info,
                                 psbkt_info->slb_info_list,
                                 e_bm,
                                 &e_loc,
                                 &info));
        if (presult->e_exist == TRUE) {
            if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                de_node = &presult->e_node.de_node;
                SHR_IF_ERR_EXIT
                    (rm_hash_dual_ent_remove(unit,
                                             req_flags,
                                             pt_dyn_info,
                                             pt_info,
                                             de_node));
            } else {
                se_node = &presult->e_node.se_node;
                SHR_IF_ERR_EXIT
                    (rm_hash_shr_ent_remove(unit,
                                            req_flags,
                                            pt_dyn_info,
                                            pt_info,
                                            se_node));
            }
            flags = BCMPTM_RSP_FLAGS_REPLACED;
        }
        SHR_IF_ERR_EXIT
            (rm_hash_rsp_info_set(unit,
                                  hei,
                                  info.rsp_entry_index,
                                  info.rsp_entry_sid,
                                  req_info->dfield_format_id,
                                  req_info->entry_words,
                                  rsp_info));
        *rsp_ltid = req_ltid;
        *rsp_flags = flags;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /*
     * All the candidate buckets have no enough space to accomadate
     * the new entry. Try to perform entry move and insert.
     */
    for (lbank = 0; lbank < pt_info->num_lbanks; lbank++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_sid_get(unit, more_info, hei, lbank, &ptsid));
        if (bcmptm_rm_hash_insert_with_ent_move(unit,
                                                req_flags,
                                                req_ltid,
                                                ptsid,
                                                pt_info,
                                                lt_ctrl->move_depth,
                                                more_info,
                                                pt_dyn_info,
                                                req_info,
                                                lbank,
                                                psbkt_info->slb_info_list,
                                                hei->num_base_entries,
                                                e_bm,
                                                &info) == SHR_E_NONE) {
            /*
             * The entry requested has been inserted thru moving existing
             * entry in the hash table.
             */
            if (presult->e_exist == TRUE) {
                /*
                 * There exists an entry which has same key but different width.
                 * The old entry will be deleted. The old entry may be moved to
                 * make room for the new entry, so the latest index for the old
                 * entry needs to be get.
                 */
                SHR_IF_ERR_EXIT
                    (rm_hash_ent_indirect_remove(unit,
                                                 req_flags,
                                                 pt_dyn_info,
                                                 req_info,
                                                 presult->hei_idx,
                                                 &lt_ctrl->lt_info,
                                                 grp_info,
                                                 more_info,
                                                 psbkt_info));
                flags = BCMPTM_RSP_FLAGS_REPLACED;
            }
            SHR_IF_ERR_EXIT
                (rm_hash_rsp_info_set(unit,
                                      hei,
                                      info.rsp_entry_index,
                                      info.rsp_entry_sid,
                                      req_info->dfield_format_id,
                                      req_info->entry_words,
                                      rsp_info));
            *rsp_ltid = req_ltid;
            *rsp_flags = flags;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_FULL);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int bcmptm_rm_hash_lookup(int unit,
                          uint32_t req_flags,
                          bcmltd_sid_t req_ltid,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          bcmptm_rm_hash_req_t *req_info,
                          size_t rsp_entry_buf_wsize,
                          uint32_t *rsp_entry_words,
                          bcmptm_rm_hash_rsp_t *rsp_info,
                          bcmltd_sid_t *rsp_ltid,
                          uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_srch_bkt_info_t *sbkt_info = NULL;
    rm_hash_srch_bank_info_t sbank_info;
    rm_hash_srch_result_t *sresult = NULL;
    const bcmptm_rm_hash_hw_entry_info_t *ent_info = NULL;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;
    size_t ent_size;
    rm_hash_shr_ent_node_t *se_node = NULL;
    rm_hash_dual_ent_node_t *de_node = NULL;
    rm_hash_grp_info_t *grp_info = NULL;
    bcmdrd_sid_t grp_sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

    /*
     * Get lt control object corresponding to the requested ltid.
     * lt control object is defined on per ltid basis.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);
    /* Get more info based on req_info */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_more_info_get(unit,
                                          req_info,
                                          lt_ctrl,
                                          &more_info));
    grp_info = &lt_ctrl->grp_info[0];

    /* Get software logical bucket list and valid base bucket bitmap */
    if (lt_ctrl->num_grp > 1) {
        /* This LT mapps to multiple physical SIDs */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_group_sid_get(unit,
                                          &lt_ctrl->lt_info,
                                          more_info,
                                          &grp_sid));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_grp_info_get(unit,
                                                grp_sid,
                                                lt_ctrl,
                                                &grp_info));
    }

    /*
     * For lookup operation, the search bucket list for the entry will be saved
     * no matter if the entry is found or not. This will be used directly for
     * the following insert operation, to avoid hash vector recomputing.
     */
    sbkt_info = &dev_last_lkup_bkt_info[unit];
    sresult   = &dev_last_lkup_result[unit];
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_srch_slb_info_get(unit,
                                              req_info,
                                              &lt_ctrl->lt_info,
                                              grp_info->pt_info,
                                              more_info,
                                              sbkt_info));
    pt_info = grp_info->pt_info;
    bcmptm_rm_hash_ent_srch_bank_info_init(unit,
                                           pt_info->lbank_list,
                                           pt_info->num_lbanks,
                                           &sbank_info);
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_search(unit,
                                   req_flags,
                                   req_info,
                                   pt_dyn_info,
                                   &lt_ctrl->lt_info,
                                   grp_info,
                                   more_info,
                                   sbkt_info,
                                       &sbank_info,
                                   sresult));
    if (sresult->e_exist == TRUE) {
        ent_info = lt_ctrl->lt_info.hw_entry_info + sresult->hei_idx;
        if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
            de_node = &sresult->e_node.de_node;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "RM hash lookup operation for ltid[%d] found "
                                    "a matched entry on mem[%s] index[%d].\n"),
                                    req_ltid,
                                    bcmdrd_pt_sid_to_name(unit, de_node->sid),
                                    de_node->e_idx));
            SHR_IF_ERR_EXIT
                (rm_hash_rsp_info_set(unit,
                                      ent_info,
                                      de_node->e_idx,
                                      de_node->sid,
                                      sresult->dft_id,
                                      sresult->e_words,
                                      rsp_info));
        } else {
            se_node = &sresult->e_node.se_node;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "RM hash lookup operation for ltid[%d] found "
                                    "a matched entry on mem[%s] index[%d].\n"),
                                    req_ltid,
                                    bcmdrd_pt_sid_to_name(unit, se_node->sid),
                                    se_node->e_idx));
        SHR_IF_ERR_EXIT
            (rm_hash_rsp_info_set(unit,
                                  ent_info,
                                  se_node->e_idx,
                                  se_node->sid,
                                  sresult->dft_id,
                                  sresult->e_words,
                                  rsp_info));
        }
        ent_size = (ent_info->entry_bitsize + 31) / 32;
        if (rsp_entry_buf_wsize < ent_size) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        sal_memcpy(rsp_entry_words, sresult->e_words, ent_size * 4);
        *rsp_ltid = sresult->ltid;
        *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmptm_rm_hash_req(int unit,
                       uint32_t req_flags,
                       uint32_t cseq_id,
                       bcmltd_sid_t req_ltid,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       bcmptm_op_type_t req_op,
                       bcmptm_rm_hash_req_t *req_info,
                       size_t rsp_entry_buf_wsize,

                       uint32_t *rsp_entry_words,
                       bcmptm_rm_hash_rsp_t *rsp_info,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags)
{
    int tmp_rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info->entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    switch (req_op) {
    case BCMPTM_OP_LOOKUP:
        tmp_rv = bcmptm_rm_hash_lookup(unit,
                                       req_flags,
                                       req_ltid,
                                       pt_dyn_info,
                                       req_info,
                                       rsp_entry_buf_wsize,
                                       rsp_entry_words,
                                       rsp_info,
                                       rsp_ltid,
                                       rsp_flags);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_INSERT:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_ent_req_cmd(unit, cseq_id));
        tmp_rv = rm_hash_insert(unit,
                                req_flags,
                                req_ltid,
                                pt_dyn_info,
                                req_info,
                                rsp_entry_buf_wsize,
                                rsp_entry_words,
                                rsp_info,
                                rsp_ltid,
                                rsp_flags);
        if (tmp_rv == SHR_E_FULL || tmp_rv == SHR_E_MEMORY) {
            SHR_RETURN_VAL_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_DELETE:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_ent_req_cmd(unit, cseq_id));
        SHR_IF_ERR_EXIT
            (rm_hash_delete(unit,
                            req_flags,
                            req_ltid,
                            pt_dyn_info,
                            req_info,
                            rsp_entry_buf_wsize,
                            rsp_entry_words,
                            rsp_info,
                            rsp_ltid,
                            rsp_flags));
        break;
    case BCMPTM_OP_GET_FIRST:
        tmp_rv = bcmptm_rm_hash_travs_cmd_proc(unit,
                                               req_flags,
                                               cseq_id,
                                               req_ltid,
                                               pt_dyn_info,
                                               BCMPTM_OP_GET_FIRST,
                                               req_info,
                                               rsp_entry_buf_wsize,
                                               rsp_entry_words,
                                               rsp_info,
                                               rsp_ltid,
                                               rsp_flags);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_GET_NEXT:
        tmp_rv = bcmptm_rm_hash_travs_cmd_proc(unit,
                                               req_flags,
                                               cseq_id,
                                               req_ltid,
                                               pt_dyn_info,
                                               BCMPTM_OP_GET_NEXT,
                                               req_info,
                                               rsp_entry_buf_wsize,
                                               rsp_entry_words,
                                               rsp_info,
                                               rsp_ltid,
                                               rsp_flags);
        if ((tmp_rv == SHR_E_NOT_FOUND) || (tmp_rv == SHR_E_UNAVAIL)) {
            SHR_RETURN_VAL_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}
