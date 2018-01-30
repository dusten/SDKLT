/*! \file rm_hash_pt_register.c
 *
 * Physical table register function for hash resource manager
 *
 * This file contains physical table register function for physical hash tables
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
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
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
static rm_hash_dev_pt_reg_info_t dev_pt_reg_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Get the total number of banks for a hash type LT.
 *
 * This function will be called during system initialization.
 *
 * \param [in] unit Unit number.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t object of a LT.
 * \param [out] bank_cnt Pointer to buffer for recording bank count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_bank_cnt_get(int unit,
                     const bcmptm_rm_hash_more_info_t *more_info,
                     uint8_t *bank_cnt)
{
    uint8_t pbank, num_banks = 0;
    uint16_t pbank_bmp = more_info->valid_pbank_bitmap;

    SHR_FUNC_ENTER(unit);

    /* Get total number of banks */
    for (pbank = 0; pbank < more_info->num_banks; pbank++) {
        if (!((0x01 << pbank) & pbank_bmp)) {
            continue;
        }
        num_banks++;
    }
    *bank_cnt = num_banks;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert a hash memory information structure into memory information list
 * of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] reg_info Pointer to rm_hash_pt_reg_info_t containing
 *             pt registration info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_pt_reg_info_insert(int unit, rm_hash_pt_reg_info_t *reg_info)
{
    rm_hash_pt_reg_info_t *head = dev_pt_reg_info[unit].head;

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "hash logical table info registering...\n")));

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "sid:[%s], ovly_sid:[%s], pt_info:[%p]\n"),
                           bcmdrd_pt_sid_to_name(unit, reg_info->sid),
                           bcmdrd_pt_sid_to_name(unit, reg_info->ovly_sid),
                           (void*)reg_info->pt_info));
    reg_info->next = head;
    dev_pt_reg_info[unit].head = reg_info;
}

/*!
 * \brief Allocate memory for a rm_hash_pt_reg_info_t structure
 *
 * \param [in] unit Unit number.
 * \param [out] reg_info Buffer to point to a rm_hash_pt_reg_info_t
 *              structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_pt_reg_info_alloc(int unit, rm_hash_pt_reg_info_t **reg_info)
{
    rm_hash_pt_reg_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(info, sizeof(rm_hash_pt_reg_info_t), "pt reg info");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(rm_hash_pt_reg_info_t));
    *reg_info = info;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate memory for a rm_hash_pt_info_t structure
 *
 * \param [in] unit Unit number.
 * \param [out] pt_info Buffer to point to a rm_hash_pt_info_t
 *              structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_pt_info_alloc(int unit, rm_hash_pt_info_t **pt_info)
{
    rm_hash_pt_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(info, sizeof(rm_hash_pt_info_t), "pt info");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(rm_hash_pt_info_t));
    *pt_info = info;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free all the memory space managed thru hash memory information list
 * of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_pt_reg_info_free(int unit)
{
    rm_hash_pt_reg_info_t *tmp_info = NULL, *cur_info = NULL;

    cur_info = dev_pt_reg_info[unit].head;
    /*
     * pt info may be shared among multiple overlay sids, pt info
     * will be released in the first round.
     */
    while (cur_info != NULL) {
        if (cur_info->ovly_sid == INVALIDm) {
            SHR_FREE(cur_info->pt_info);
        }
        cur_info = cur_info->next;
    }
    cur_info = dev_pt_reg_info[unit].head;
    /* pt registration info will be released in the 2nd round. */
    while (cur_info != NULL) {
        tmp_info = cur_info;
        cur_info = cur_info->next;
        SHR_FREE(tmp_info);
    }
    dev_pt_reg_info[unit].head = NULL;
    dev_pt_reg_info[unit].num_ltids = 0;
}

/*!
 * \brief Non-overlay ptsid register information init
 *
 * \param [in] unit Unit number.
 * \param [in] sid Hash physical table sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_base_pt_reg_info_init(int unit, bcmdrd_sid_t sid)
{
    rm_hash_pt_reg_info_t *reg_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_pt_reg_info_alloc(unit, &reg_info));

    SHR_IF_ERR_EXIT
        (rm_hash_pt_info_alloc(unit, &pt_info));

    reg_info->sid = sid;
    reg_info->ovly_sid = INVALIDm;
    reg_info->pt_info = pt_info;
    rm_hash_pt_reg_info_insert(unit, reg_info);

exit:
    if (SHR_FUNC_ERR()) {
        if (reg_info != NULL) {
            SHR_FREE(reg_info);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Overlay ptsid register information init.
 *
 * \param [in] unit Unit number.
 * \param [in] sid physical sid.
 * \param [in] ovly_sid Overlay sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ovly_pt_reg_info_init(int unit,
                              bcmdrd_sid_t sid,
                              bcmdrd_sid_t ovly_sid)
{
    rm_hash_pt_reg_info_t *info = NULL, *ovly_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_pt_reg_info_alloc(unit, &info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_reg_info_get(unit, ovly_sid, &ovly_info));
    info->sid = sid;
    info->ovly_sid = ovly_sid;
    info->pt_info = ovly_info->pt_info;
    rm_hash_pt_reg_info_insert(unit, info);

exit:
    if (SHR_FUNC_ERR()) {
        if (info != NULL) {
            SHR_FREE(info);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Bucket mode get
 *
 * \param [in] unit Unit number.
 * \param [in] more_info Pionter to structure bcmptm_rm_hash_more_info_t.
 * \param [out] bm Bucket mode value.
 * \param [out] bmf Bucket mode flag.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_bkt_mode_get(int unit,
                     const bcmptm_rm_hash_more_info_t *more_info,
                     rm_hash_bm_t *bm,
                     uint8_t *bmf)
{
    SHR_FUNC_ENTER(unit);

    switch(more_info->entry_mode) {
    case BCMPTM_RM_HASH_ENTRY_MODE_EM_128:
    case BCMPTM_RM_HASH_ENTRY_MODE_EM_160:
        *bm = RM_HASH_BM_0;
        *bmf = RM_HASH_BM0;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case BCMPTM_RM_HASH_ENTRY_MODE_EM_320:
        if (more_info->en_base_valid == FALSE) {
            *bm = RM_HASH_BM_1;
            *bmf = RM_HASH_BM1;
        } else {
            
            *bm = RM_HASH_BM_0;
            *bmf = RM_HASH_BM0;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_MPLS:
    case BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_L3_TNL:
        *bm = RM_HASH_BM_N;
        *bmf = RM_HASH_BMN;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case BCMPTM_RM_HASH_ENTRY_MODE_NONE:
        if (more_info->en_base_valid == FALSE) {
            *bm = RM_HASH_BM_N;
            *bmf = RM_HASH_BMN;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Key type get
 *
 * \param [in] unit Unit number.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] sid_idx SID index.
 * \param [out] key_type Key type value.
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
         * For fixed-func devices, there is only one key type for all
         * the banks.
         */
        *key_type = more_info->key_type_val[0];
    } else {
        /* For flex-func devices, each bank has separated key type value. */
        *key_type = more_info->key_type_val[sid_idx];
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the widest bucket mode
 *
 * \param [in] unit Unit number.
 * \param [in] bmf Bucket mode flags.
 * \param [out] max_bm Maximum bucket mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_max_bkt_mode_get(int unit, uint8_t bmf, rm_hash_bm_t *max_bm)
{
    SHR_FUNC_ENTER(unit);

    if (bmf & RM_HASH_BM2) {
        *max_bm = RM_HASH_BM_2;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    if (bmf & RM_HASH_BM1) {
        *max_bm = RM_HASH_BM_1;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    if (bmf & RM_HASH_BM0) {
        *max_bm = RM_HASH_BM_0;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    if (bmf & RM_HASH_BMN) {
        *max_bm = RM_HASH_BM_N;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set software logical bucket size based on bucket mode
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] bm Bucket mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_size_set(int unit,
                     rm_hash_pt_info_t *pt_info,
                     const bcmptm_rm_hash_more_info_t *more_info,
                     rm_hash_bm_t bm)
{
    uint8_t hlb_size = more_info->num_base_entries;

    SHR_FUNC_ENTER(unit);

    /*
     * num_base_entries in rm hash more info structure describes
     * how many hardware logical entries are contained in a hardware
     * logical bucket.
     */
    switch(bm) {
    case RM_HASH_BM_0:
        pt_info->slb_size = hlb_size;
        break;
    case RM_HASH_BM_1:
        pt_info->slb_size = hlb_size * 2;
        break;
    case RM_HASH_BM_2:
        pt_info->slb_size = hlb_size * 4;
        break;
    case RM_HASH_BM_N:
        pt_info->slb_size = hlb_size;
        if (more_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
            pt_info->slb_size = hlb_size / 2;
        }
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Non-overlay physical table information init.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_pt_info_init(int unit,
                     rm_hash_pt_info_t *pt_info,
                     const bcmptm_rm_hash_more_info_t *more_info)
{
    uint16_t key_type = 0;
    rm_hash_bm_t bm = RM_HASH_BM_N;
    uint8_t bmf = 0, num_lbanks = 0, lbank = 0, ebank = 0;
    const bcmptm_rm_hash_vector_bank_attr_t *vec_attrib = NULL;
    uint32_t lbank_size = 0, num_slbs = 0;

    SHR_FUNC_ENTER(unit);

    if (pt_info->init == TRUE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    pt_info->hash_type = more_info->hash_type;

    /*
     * For the given key type value corresponding to the specified
     * rm more info, get the bucket mode of it. For EXACT MATCH
     * table on fixed-func devices, this is achieved via entry mode.
     */
    SHR_IF_ERR_EXIT
        (rm_hash_bkt_mode_get(unit, more_info, &bm, &bmf));
    pt_info->bmf |= bmf;
    SHR_IF_ERR_EXIT
        (rm_hash_key_type_get(unit, more_info, 0, &key_type));
    pt_info->key_attrib[key_type].bm = bm;
    pt_info->key_attrib[key_type].valid = TRUE;

    /*
     * Although it is possible that a LT can have multiple more info,
     * corresponding to multiple key type values, the bank count remain
     * the same.
     */
    SHR_IF_ERR_EXIT
        (rm_hash_bank_cnt_get(unit, more_info, &num_lbanks));
    pt_info->num_lbanks = num_lbanks;

    vec_attrib = more_info->hash_vector_attr->bank_attr;
    for (ebank = 0; ebank < more_info->num_banks; ebank++, vec_attrib++) {
        /* Eligible bank to logical bank list, using eligible bank to index */
        lbank = vec_attrib->logical_bank;
        pt_info->lbank_list[ebank] = lbank;
        pt_info->ebank_list[lbank] = ebank;
        /*
         * Extract the hash vector type info, offset and mask,
         * the information may be updated at runtime.
         */
        pt_info->vec_info[lbank].type   = vec_attrib->type;
        pt_info->vec_info[lbank].offset = vec_attrib->offset;
        pt_info->vec_info[lbank].mask   = vec_attrib->mask;
        /*
         * For a specific bank, the bank size is described via the number
         * of hardware base bucket. If there are multiple bucket modes
         * supported, the bank size will be described via the number of
         * the widest logical buckets - same as software logical bucket.
         */
        lbank_size = vec_attrib->mask + 1;
        switch (bm) {
        case RM_HASH_BM_N:
        case RM_HASH_BM_0:
            pt_info->lbank_size[lbank] = lbank_size;
            break;
        case RM_HASH_BM_1:
            pt_info->lbank_size[lbank] = lbank_size / 2;
            break;
        case RM_HASH_BM_2:
            pt_info->lbank_size[lbank] = lbank_size / 4;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }
    /*
     * Initialize the base for each bank using the logical bucket number
     * of the 1st bucket mode encountered.
     */
    for (lbank = 0; lbank < pt_info->num_lbanks; lbank++) {
        pt_info->lbank_base[lbank] = num_slbs;
        num_slbs += pt_info->lbank_size[lbank];
    }
    pt_info->num_slbs = num_slbs;

    /*
     * Bucket size in pt info will be described via the number of base entry.
     * The bucket size in more info refers to the number of logical entry in
     * a logical bucket for a key type.
     */
    SHR_IF_ERR_EXIT
        (rm_hash_slb_size_set(unit, pt_info, more_info, bm));
    pt_info->init = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update pt register info
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_pt_info_update(int unit,
                       rm_hash_pt_info_t *pt_info,
                       const bcmptm_rm_hash_more_info_t *more_info)
{
    rm_hash_bm_t cur_bm = RM_HASH_BM_N, max_bm = RM_HASH_BM_N;
    uint16_t key_type = 0;
    uint8_t bmf = 0, num_lbanks = 0, lbank = 0, ebank = 0;
    const bcmptm_rm_hash_vector_bank_attr_t *bank_attr = NULL;
    uint32_t lbank_size = 0, num_slbs = 0;

    SHR_FUNC_ENTER(unit);

    if (pt_info->hash_type != more_info->hash_type) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (rm_hash_bkt_mode_get(unit, more_info, &cur_bm, &bmf));
    SHR_IF_ERR_EXIT
        (rm_hash_key_type_get(unit, more_info, 0, &key_type));
    pt_info->key_attrib[key_type].bm = cur_bm;
    pt_info->key_attrib[key_type].valid = TRUE;

    /* Check if there exist mismatch among different LTs */
    SHR_IF_ERR_EXIT
        (rm_hash_bank_cnt_get(unit, more_info, &num_lbanks));
    if (pt_info->num_lbanks != num_lbanks) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    SHR_IF_ERR_EXIT
        (rm_hash_max_bkt_mode_get(unit, pt_info->bmf, &max_bm));

    bank_attr = more_info->hash_vector_attr->bank_attr;
    for (ebank = 0; ebank < more_info->num_banks; ebank++, bank_attr++) {
        lbank = bank_attr->logical_bank;
        lbank_size = bank_attr->mask + 1;
        if (cur_bm > max_bm) {
            /*
             * Wider bucket mode is to be supported, modify the software
             * logical bucket accordingly.
             */
            switch (cur_bm) {
            case RM_HASH_BM_N:
            case RM_HASH_BM_0:
                pt_info->lbank_size[lbank] = lbank_size;
                break;
            case RM_HASH_BM_1:
                pt_info->lbank_size[lbank] = lbank_size / 2;
                break;
            case RM_HASH_BM_2:
                pt_info->lbank_size[lbank] = lbank_size / 4;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
    }
    if (cur_bm > max_bm) {
        /*
         * if wider bucket mode is encountered, update the base of each bank
         * using the number of logical buckets in wider bucket mode.
         */
        for (lbank = 0; lbank < pt_info->num_lbanks; lbank++) {
            pt_info->lbank_base[lbank] = num_slbs;
            num_slbs += pt_info->lbank_size[lbank];
        }
        pt_info->num_slbs = num_slbs;
        SHR_IF_ERR_EXIT
            (rm_hash_slb_size_set(unit, pt_info, more_info, cur_bm));
    }
    pt_info->bmf |= bmf;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_pt_reg_info_add(int unit, bcmltd_sid_t ltid)
{
    bcmptm_rm_hash_lt_info_t lt_info;
    uint16_t data_type_list[RM_HASH_MAX_BANK_COUNT], *data_type = NULL;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    uint8_t hei_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {0};
    rm_hash_pt_reg_info_t *reg_info = NULL;
    bcmdrd_sid_t sid, ovly_sid = INVALIDm;
    uint8_t hei_cnt = 0, info_idx;
    int hei_idx;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_info, 0, sizeof(bcmptm_rm_hash_lt_info_t));
    data_type = &data_type_list[0];
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_ptrm_info_get(unit,
                                      ltid,
                                      0,
                                      FALSE,
                                      &lt_info,
                                      &hei_idx,
                                      &data_type));
    /*
     * It is possible that a LT may have several different key type values on
     * SAME physical hash table, for example LT based on exact match hash
     * tables. Each key type will correspond to a rm_more_info structure.
     * Also, it is possible that a LT may map to multiple separated physical
     * tables, for example LT TNL_MPLS_DECAP maps onto MPLS_ENTRY and
     * L3_TUNNEL_SINGLE tables. Each mapping in this case has a separated
     * rm more info structure.
     */
    for (idx = 0; idx < lt_info.rm_more_info_count; idx++) {
        more_info = lt_info.rm_more_info + idx;
        if (more_info->num_bank_sid > 1) {
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }
        /*
         * It is possible that a key type may have several different table
         * views, for example a key type may have single, duble views.
         * Get the informaton about entry info count and entry info list
         * for this more info. i.e. key type value.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_given_key_ent_info_get(unit,
                                                   &lt_info,
                                                   more_info,
                                                   &hei_cnt,
                                                   hei_list));
        for (info_idx = 0; info_idx < hei_cnt; info_idx++) {
            hei = lt_info.hw_entry_info + hei_list[info_idx];
            sid = hei->sid[0];
            ovly_sid = INVALIDm;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ovly_sid_get(unit, sid, &ovly_sid));
            if ((ovly_sid != INVALIDm) &&
                (bcmptm_rm_hash_pt_reg_info_get(unit,
                                                ovly_sid,
                                                &reg_info) == SHR_E_NOT_FOUND)) {
                SHR_IF_ERR_EXIT
                    (rm_hash_base_pt_reg_info_init(unit, ovly_sid));
            }
            if (bcmptm_rm_hash_pt_reg_info_get(unit,
                                               sid,
                                               &reg_info) == SHR_E_NOT_FOUND) {
                if (ovly_sid == INVALIDm) {
                    SHR_IF_ERR_EXIT
                        (rm_hash_base_pt_reg_info_init(unit, sid));
                } else {
                    SHR_IF_ERR_EXIT
                        (rm_hash_ovly_pt_reg_info_init(unit, sid, ovly_sid));
                }
            }
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_pt_reg_info_get(unit, sid, &reg_info));
            if (reg_info->pt_info->init == FALSE) {
                SHR_IF_ERR_EXIT
                    (rm_hash_pt_info_init(unit,
                                          reg_info->pt_info,
                                          more_info));
            } else {
                SHR_IF_ERR_EXIT
                    (rm_hash_pt_info_update(unit,
                                            reg_info->pt_info,
                                            more_info));
            }
        }
    }
    dev_pt_reg_info[unit].num_ltids++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pt_reg_info_get(int unit,
                               bcmdrd_sid_t sid,
                               rm_hash_pt_reg_info_t **reg_info)
{
    rm_hash_pt_reg_info_t *curr = dev_pt_reg_info[unit].head;

    SHR_FUNC_ENTER(unit);

    for ( ; curr != NULL; curr = curr->next) {
        if (curr->sid == sid) {
            *reg_info = curr;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_dev_pt_reg_info_get(int unit,
                                   rm_hash_pt_reg_info_t **reg_info)
{
    SHR_FUNC_ENTER(unit);

    *reg_info = dev_pt_reg_info[unit].head;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_num_ltids_get(int unit, uint16_t *num_ltids)
{
    SHR_FUNC_ENTER(unit);

    *num_ltids = dev_pt_reg_info[unit].num_ltids;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pt_reg_info_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    rm_hash_pt_reg_info_free(unit);

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pt_reg_info_dump(int unit)
{
    rm_hash_pt_reg_info_t *reg_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;
    uint8_t lbank, idx;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                            "Total number of LTID:[%d]\n"),
                            dev_pt_reg_info[unit].num_ltids));
    reg_info = dev_pt_reg_info[unit].head;
    for ( ; reg_info != NULL; reg_info = reg_info->next) {
        pt_info = reg_info->pt_info;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\n*******************************************\n"
                                "reg_info: [%p], \n"
                                "pt_info:[%p], \n"
                                "physical table:[%s], \n"
                                "overlay table:[%s], \n"
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
                                (void*)reg_info,
                                (void*)pt_info,
                                bcmdrd_pt_sid_to_name(unit, reg_info->sid),
                                bcmdrd_pt_sid_to_name(unit, reg_info->ovly_sid),
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

    SHR_FUNC_EXIT();
}

