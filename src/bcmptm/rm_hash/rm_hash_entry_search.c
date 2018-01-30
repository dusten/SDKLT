/*! \file rm_hash_entry_search.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement entry search for a given entry
 * key. In hash resource manager, searching and determining if there exists a
 * matched entry for a given key is a fundamental operation upon which the basic
 * hash table operation LOOKUP, INSERT and DELETE will rely.
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


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Determine whether two entries have same key values.
 *
 * This function extracts the key fields values from the entry buffer, and
 * compare whether they are identical.
 *
 * \param [in] entry_a Entry a to be compared.
 * \param [in] entry_b Entry b to be compared.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure
 *             corresponding to a LT.
 *
 * \retval TRUE The two entries have same key values.
 * \retval FALSE The two entries have different key values.
 */
static bool
rm_hash_key_is_identical(const uint32_t *entry_a,
                         const uint32_t *entry_b,
                         const bcmptm_rm_hash_more_info_t *more_info)
{
    uint8_t idx;
    int sbit, ebit;
    uint32_t key_field_a[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t key_field_b[BCMPTM_MAX_PT_FIELD_WORDS];
    const bcmlrd_hw_field_list_t *key_fields = more_info->hw_key_fields;

    for (idx = 0; idx < key_fields->num_fields; idx++) {
        uint16_t num_words, word;
        sal_memset(key_field_a, 0, sizeof(key_field_a));
        sal_memset(key_field_b, 0, sizeof(key_field_b));
        sbit = key_fields->field_start_bit[idx];
        ebit = key_fields->field_start_bit[idx] + key_fields->field_width[idx] - 1;
        bcmdrd_field_get(entry_a, sbit, ebit, key_field_a);
        bcmdrd_field_get(entry_b, sbit, ebit, key_field_b);
        num_words = (key_fields->field_width[idx] + 31) / 32;
        for (word = 0; word < num_words; word++) {
            if (key_field_a[word] != key_field_b[word]) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/*!
 * \brief Get the hardware logical bucket for an entry
 *
 * This function computes hardware logical bucket for an entry
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] more_info Pointer to rm_hash_lt_ctrl_t structure.
 * \param [out] hlb_list Pointer to a buffer to save hardware logical bucket.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_hlb_get(int unit,
                bcmptm_rm_hash_req_t *req_info,
                const bcmptm_rm_hash_lt_info_t *lt_info,
                rm_hash_pt_info_t *pt_info,
                const bcmptm_rm_hash_more_info_t *more_info,
                uint32_t *hlb_list)
{
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    uint8_t hei_cnt = 0, hei_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {0};
    uint8_t key_a[BCMPTM_MAX_PT_ENTRY_WORDS * 4] = {0}, num_sids = 0;
    uint8_t key_b[BCMPTM_MAX_PT_ENTRY_WORDS * 4] = {0};
    bcmptm_rm_hash_rhash_cfg_t *rhcfg = NULL;
    int key_size = 0;

    SHR_FUNC_ENTER(unit);

    num_sids = more_info->num_bank_sid;
    if (num_sids > 1) {
        /* Unsupported */
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /* Get valid hardware entry info for the given rm more info */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_given_key_ent_info_get(unit,
                                               lt_info,
                                               more_info,
                                               &hei_cnt,
                                               hei_list));
    /*
     * The 1st valid hardware entry info is good enough for key extraction
     * and vector computing.
     */
    hei = lt_info->hw_entry_info + hei_list[0];
    if (more_info->key_type_val != NULL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_type_fill(unit,
                                          more_info->key_type_val[0],
                                          hei,
                                          req_info->entry_words));
    }
    rhcfg = pt_info->rh_cfg;
    bcmptm_rm_hash_key_extract(unit,
                               req_info->entry_words,
                               more_info,
                               hei,
                               rhcfg,
                               0,
                               0,
                               key_a,
                               &key_size);
    /*
     * Only the physical hash table supports robust hash and this feature
     * has been enabled, the key_b extraction and transform is required.
     */
    if ((rhcfg != NULL) && (rhcfg->enable == TRUE)) {
        bcmptm_rm_hash_key_extract(unit,
                                   req_info->entry_words,
                                   more_info,
                                   hei,
                                   rhcfg,
                                   1,
                                   0,
                                   key_b,
                                   &key_size);
    } else {
        sal_memcpy(key_b, key_a, sizeof(key_a));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_vector_compute(unit,
                                       req_info->entry_words,
                                       key_a,
                                       key_b,
                                       key_size,
                                       pt_info->lbank_list,
                                       pt_info->num_lbanks,
                                       more_info,
                                       pt_info,
                                       hlb_list));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_ent_srch_slb_info_get(int unit,
                                     bcmptm_rm_hash_req_t *req_info,
                                     const bcmptm_rm_hash_lt_info_t *lt_info,
                                     rm_hash_pt_info_t *pt_info,
                                     const bcmptm_rm_hash_more_info_t *more_info,
                                     rm_hash_srch_bkt_info_t *sbkt_info)
{
    uint8_t num_sids;
    rm_hash_bm_t e_bm;

    SHR_FUNC_ENTER(unit);

    num_sids = more_info->num_bank_sid;
    if (num_sids > 1) {
        /* Unsupported */
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /* Compute hardware logical bucket list */
    SHR_IF_ERR_EXIT
        (rm_hash_hlb_get(unit,
                         req_info,
                         lt_info,
                         pt_info,
                         more_info,
                         sbkt_info->hlb_list));
    /* Get bucket mode of the requested entry */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_bkt_mode_get(unit,
                                         more_info,
                                         pt_info,
                                         &e_bm));
    /*
     * For this ltid, hardware logical bucket list have been computed.
     * Compute software logical bucket and corresponding valid base
     * bucket bitmap within them.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_hlb_to_slb_get(unit,
                                       pt_info,
                                       e_bm,
                                       sbkt_info->hlb_list,
                                       sbkt_info->slb_info_list));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Specify the bank list on which the entry search will be performed.
 *
 * \param [in] unit Unit number.
 * \param [in] lbank_list Search bank list.
 * \param [in] num_lbanks Search bank number.
 * \param [out] sbank_info Pointer to rm_hash_srch_bank_info_t structure.
 */
void
bcmptm_rm_hash_ent_srch_bank_info_init(int unit,
                                       uint8_t *lbank_list,
                                       uint8_t num_lbanks,
                                       rm_hash_srch_bank_info_t *sbank_info)
{
    uint8_t idx;

    for (idx = 0; idx < num_lbanks; idx++) {
        sbank_info->lbank_list[idx] = lbank_list[idx];
    }
    sbank_info->num_lbanks = num_lbanks;
}

/*!
 * \brief Search an entry from the PT cache
 *
 * For a requested entry, its corresponding software logical bucket list
 * has been calculated. The requested entry will be searched in the software
 * logical bucket list to determine if it exists.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Request flags.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] sbkt_info Pointer to rm_hash_srch_bkt_info_t structure.
 * \param [in] sbank_info Pointer to rm_hash_srch_bank_info_t structure.
 * \param [out] sresult Pointer to rm_hash_srch_result_t structure.
 */
int
bcmptm_rm_hash_ent_search(int unit,
                          uint32_t req_flags,
                          bcmptm_rm_hash_req_t *req_info,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          const bcmptm_rm_hash_lt_info_t *lt_info,
                          rm_hash_grp_info_t *grp_info,
                          const bcmptm_rm_hash_more_info_t *more_info,
                          rm_hash_srch_bkt_info_t *sbkt_info,
                          rm_hash_srch_bank_info_t *sbank_info,
                          rm_hash_srch_result_t *sresult)
{
    rm_hash_pt_info_t *pt_info = NULL;
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_shr_ent_node_t  *se_node = NULL;
    rm_hash_dual_ent_node_t *de_node = NULL;
    uint8_t bank_idx, idx, hei_idx, lbank;
    bool eligible = FALSE;
    uint32_t e_sig = 0;
    rm_hash_bm_t e_bm;

    SHR_FUNC_ENTER(unit);

    pt_info = grp_info->pt_info;
    /*
     * There is a valid cache entry at current index for this
     * table view. Compare requested entry word with cache value.
     */
    if ((more_info->num_bank_sid > 1) &&
        (more_info->key_type_val != NULL)) {
        /*
         * If each physical bank has separated key type value,
         * the key type value needs to be filled before entry
         * comparing.
         */
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /* Get bucket mode of the requested entry */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_bkt_mode_get(unit,
                                         more_info,
                                         pt_info,
                                         &e_bm));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_sig_generate(unit,
                                         more_info,
                                         req_info->entry_words,
                                         &e_sig));
    for (bank_idx = 0; bank_idx < sbank_info->num_lbanks; bank_idx++) {
        lbank = sbank_info->lbank_list[bank_idx];
        b_state = pt_info->b_state + sbkt_info->slb_info_list[lbank].slb;
        if (b_state->ve_cnt > 0) {
            if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_dual_ent_node_get(unit,
                                                      pt_info,
                                                      b_state->e_node_offset,
                                                      &de_node));
                for (idx = 0; idx < b_state->ve_cnt; idx++, de_node++) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_eligible_for_dual_ent_compare(unit,
                                                                      lt_info,
                                                                      grp_info,
                                                                      de_node,
                                                                      e_bm,
                                                                      e_sig,
                                                                      &eligible,
                                                                      &hei_idx));
                    if (eligible == TRUE) {
                        bcmbd_pt_dyn_info_t dyn_info = *pt_dyn_info;
                        bool cache_valid;
                        bcmltd_sid_t ltid;
                        uint16_t dft_id;
                        uint32_t rd_req_flags, rd_rsp_flags;
                        dyn_info.index = de_node->e_idx;
                        rd_req_flags = req_flags & ~BCMPTM_REQ_FLAGS_DONT_USE_CACHE;
                        SHR_IF_ERR_EXIT
                            (bcmptm_cmdproc_read(unit,
                                                 rd_req_flags,
                                                 0,
                                                 de_node->sid,
                                                 &dyn_info,
                                                 NULL,
                                                 NULL, /* misc_info */
                                                 BCMPTM_MAX_PT_ENTRY_WORDS,
                                                 sresult->e_words,
                                                 &cache_valid,
                                                 &ltid,
                                                 &dft_id,
                                                 &rd_rsp_flags));
                        if (cache_valid == FALSE) {
                            continue;
                        }
                        if (rm_hash_key_is_identical(req_info->entry_words,
                                                     sresult->e_words,
                                                     more_info) == TRUE) {
                            sresult->e_exist = TRUE;
                            sresult->e_node.de_node = *de_node;
                            sresult->ltid = ltid;
                            sresult->hei_idx = hei_idx;
                            sresult->dft_id = dft_id;
                            if (req_flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) {
                                SHR_IF_ERR_EXIT
                                    (bcmptm_cmdproc_read(unit,
                                                         req_flags,
                                                         0,
                                                         de_node->sid,
                                                         &dyn_info,
                                                         NULL,
                                                         NULL, /* misc_info */
                                                         BCMPTM_MAX_PT_ENTRY_WORDS,
                                                         sresult->e_words,
                                                         &cache_valid,
                                                         &ltid,
                                                         &dft_id,
                                                         &rd_rsp_flags));
                            }
                            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
                        }
                    }
                }
            } else {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_shr_ent_node_get(unit,
                                                     pt_info,
                                                     b_state->e_node_offset,
                                                     &se_node));
                for (idx = 0; idx < b_state->ve_cnt; idx++, se_node++) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_eligible_for_shr_ent_compare(unit,
                                                                     lt_info,
                                                                     grp_info,
                                                                     se_node,
                                                                     e_bm,
                                                                     e_sig,
                                                                     &eligible,
                                                                     &hei_idx));
                    if (eligible == TRUE) {
                        bcmbd_pt_dyn_info_t dyn_info = *pt_dyn_info;
                        bool cache_valid;
                        bcmltd_sid_t ltid;
                        uint16_t dft_id;
                        uint32_t rd_req_flags, rd_rsp_flags;
                        dyn_info.index = se_node->e_idx;
                        rd_req_flags = req_flags & ~BCMPTM_REQ_FLAGS_DONT_USE_CACHE;
                        SHR_IF_ERR_EXIT
                            (bcmptm_cmdproc_read(unit,
                                                 rd_req_flags,
                                                 0,
                                                 se_node->sid,
                                                 &dyn_info,
                                                 NULL,
                                                 NULL, /* misc_info */
                                                 BCMPTM_MAX_PT_ENTRY_WORDS,
                                                 sresult->e_words,
                                                 &cache_valid,
                                                 &ltid,
                                                 &dft_id,
                                                 &rd_rsp_flags));
                        if (cache_valid == FALSE) {
                            continue;
                        }
                        if (rm_hash_key_is_identical(req_info->entry_words,
                                                     sresult->e_words,
                                                     more_info) == TRUE) {
                            sresult->e_exist = TRUE;
                            sresult->e_node.se_node = *se_node;
                            sresult->ltid = ltid;
                            sresult->hei_idx = hei_idx;
                            sresult->dft_id = dft_id;
                            if (req_flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) {
                                SHR_IF_ERR_EXIT
                                    (bcmptm_cmdproc_read(unit,
                                                         req_flags,
                                                         0,
                                                         se_node->sid,
                                                         &dyn_info,
                                                         NULL,
                                                         NULL, /* misc_info */
                                                         BCMPTM_MAX_PT_ENTRY_WORDS,
                                                         sresult->e_words,
                                                         &cache_valid,
                                                         &ltid,
                                                         &dft_id,
                                                         &rd_rsp_flags));
                            }
                            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
                        }
                    }
                }
            }
        }
    }
    sresult->e_exist = FALSE;

exit:
    SHR_FUNC_EXIT();
}
