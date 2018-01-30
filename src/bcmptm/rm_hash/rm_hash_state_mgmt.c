/*! \file rm_hash_state_mgmt.c
 *
 * Software State of The Resource Manager for Hash Table
 *
 * This file defines software objects that record software state of resource
 * manager of hash tables, and defines corresponding functions that get and set
 * these software objects.
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
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmdrd_new.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "rm_hash_internal.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

#define RM_HASH_INIT_UNDO_LIST_LENGTH           1024
#define RM_HASH_BASE_BKT_SIZE                   4

/*******************************************************************************
 * Typedefs
 */
/*
 *! \brief Function used to search a free slot
 *
 * This data structure describes the signature of the function used to search
 * a free slot.
 */
typedef int (*fn_ent_free_slot_srch_t)(int unit,
                                       rm_hash_pt_info_t *pt_info,
                                       const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                       rm_hash_ent_slb_info_t *slb_info_list,
                                       bool *e_loc_found,
                                       rm_hash_ent_loc_t *e_loc);

/*
 *! \brief Free base entry info for a base bucket
 *
 * This data structure describes free base entry info for a base bucket.
 */
typedef struct rm_hash_base_bkt_free_ent_s {
    /*! \brief Free base entry is available or not */
    bool avail;

    /*! \brief Free base entry count */
    uint8_t fbe_cnt;

    /*! \brief Free base entry bitmap */
    uint8_t bmp;

    /*! \brief Free base entry offset */
    uint8_t offset;
} rm_hash_base_bkt_free_ent_t;

/*
 *! \brief Base entry to logical entry mapping
 *
 * This data structure describes base entry to logical entry mapping
 */
typedef struct rm_hash_be_to_le_map_s {
    /*! \brief Base entry bitmap is valid or not */
    bool valid;

    /*! \brief Logical entry bitmap for the given base entry bmp */
    uint8_t le_bmp;
} rm_hash_be_to_le_map_t;

/*******************************************************************************
 * Private variables
 */
static rm_hash_state_elems_amount_t dev_state_elems_amount[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_dev_ent_ctr_desc_t   *dev_ent_ctr_desc[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_trans_record_t       *dev_trans_rec[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_trans_undo_list_t    *dev_trans_undo_list[BCMDRD_CONFIG_MAX_UNITS];

static const rm_hash_base_bkt_free_ent_t sw_ent_free_info [16] = {
    {TRUE,  0x04, 0x01, 0x00}, /* 0000 */
    {TRUE,  0x03, 0x02, 0x01}, /* 0001 */
    {TRUE,  0x03, 0x01, 0x00}, /* 0010 */
    {TRUE,  0x02, 0x04, 0x02}, /* 0011 */
    {TRUE,  0x03, 0x01, 0x00}, /* 0100 */
    {TRUE,  0x02, 0x02, 0x01}, /* 0101 */
    {TRUE,  0x02, 0x01, 0x00}, /* 0110 */
    {TRUE,  0x01, 0x08, 0x03}, /* 0111 */
    {TRUE,  0x03, 0x01, 0x00}, /* 1000 */
    {TRUE,  0x02, 0x02, 0x01}, /* 1001 */
    {TRUE,  0x02, 0x01, 0x00}, /* 1010 */
    {TRUE,  0x01, 0x04, 0x02}, /* 1011 */
    {TRUE,  0x02, 0x01, 0x00}, /* 1100 */
    {TRUE,  0x01, 0x02, 0x01}, /* 1101 */
    {TRUE,  0x01, 0x01, 0x00}, /* 1110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 1111 */
};

static const rm_hash_base_bkt_free_ent_t dw_ent_free_info [16] = {
    {TRUE,  0x04, 0x03, 0x00}, /* 0000 */
    {TRUE,  0x03, 0x0C, 0x02}, /* 0001 */
    {TRUE,  0x03, 0x0C, 0x02}, /* 0010 */
    {TRUE,  0x02, 0x0C, 0x02}, /* 0011 */
    {TRUE,  0x03, 0x03, 0x00}, /* 0100 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0101 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0110 */
    {FALSE, 0x01, 0x08, 0x03}, /* 0111 */
    {TRUE,  0x03, 0x03, 0x00}, /* 1000 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1001 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1010 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1011 */
    {TRUE,  0x02, 0x03, 0x00}, /* 1100 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1101 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 1111 */
};

static const rm_hash_base_bkt_free_ent_t qw_ent_free_info [16] = {
    {TRUE,  0x04, 0x0F, 0x00}, /* 0000 */
    {FALSE, 0x03, 0x00, 0x00}, /* 0001 */
    {FALSE, 0x03, 0x00, 0x00}, /* 0010 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0011 */
    {FALSE, 0x03, 0x00, 0x00}, /* 0100 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0101 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0110 */
    {FALSE, 0x01, 0x00, 0x00}, /* 0111 */
    {FALSE, 0x03, 0x00, 0x00}, /* 1000 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1001 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1010 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1011 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1100 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1101 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 1111 */
};

static const rm_hash_be_to_le_map_t bm1e_be_to_le_map [16] = {
    {FALSE, 0x00}, /* 0000 */
    {TRUE,  0x03}, /* 0001 */
    {TRUE,  0x03}, /* 0010 */
    {TRUE,  0x03}, /* 0011 */
    {TRUE,  0x0C}, /* 0100 */
    {FALSE, 0x00}, /* 0101 */
    {FALSE, 0x00}, /* 0110 */
    {FALSE, 0x00}, /* 0111 */
    {TRUE,  0x0C}, /* 1000 */
    {FALSE, 0x00}, /* 1001 */
    {FALSE, 0x00}, /* 1010 */
    {FALSE, 0x00}, /* 1011 */
    {TRUE,  0x0C}, /* 1100 */
    {FALSE, 0x00}, /* 1101 */
    {FALSE, 0x00}, /* 1110 */
    {TRUE,  0x0F}, /* 1111 */
};

static const rm_hash_ent_size_map_t e_size_map [16] = {
    {FALSE, 0x00}, /* 0000 */
    {TRUE,  0x01}, /* 0001 */
    {TRUE,  0x01}, /* 0010 */
    {TRUE,  0x02}, /* 0011 */
    {TRUE,  0x01}, /* 0100 */
    {FALSE, 0x00}, /* 0101 */
    {FALSE, 0x00}, /* 0110 */
    {FALSE, 0x00}, /* 0111 */
    {TRUE,  0x01}, /* 1000 */
    {FALSE, 0x00}, /* 1001 */
    {FALSE, 0x00}, /* 1010 */
    {FALSE, 0x00}, /* 1011 */
    {TRUE,  0x02}, /* 1100 */
    {FALSE, 0x00}, /* 1101 */
    {FALSE, 0x00}, /* 1110 */
    {TRUE,  0x04}, /* 1111 */
};

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Iterate all the lt register information object and calculate the amount
 * of various software state elements involved in hash resource manager.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_state_elems_amount_accum(int unit)
{
    rm_hash_pt_reg_info_t *reg_info = NULL;
    rm_hash_state_elems_amount_t *elems_amount = &dev_state_elems_amount[unit];

    SHR_FUNC_ENTER(unit);

    sal_memset(elems_amount, 0, sizeof(rm_hash_state_elems_amount_t));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_dev_pt_reg_info_get(unit, &reg_info));
    for ( ; reg_info != NULL; reg_info = reg_info->next) {
        if (reg_info->ovly_sid == INVALIDm) {
            elems_amount->slb_cnt += reg_info->pt_info->num_slbs;
            if (reg_info->pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                elems_amount->dual_e_node_cnt += reg_info->pt_info->num_slbs *
                                                 reg_info->pt_info->slb_size;
            } else {
                elems_amount->shr_e_node_cnt += reg_info->pt_info->num_slbs *
                                                reg_info->pt_info->slb_size;
            }
            elems_amount->uent_ctr_cnt++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free HA memory space allocated for various resource manager software
 * state elements.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_state_elems_free(int unit)
{
    rm_hash_pt_reg_info_t *reg_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_dev_pt_reg_info_get(unit, &reg_info));
    for ( ; reg_info != NULL; reg_info = reg_info->next) {
        if (reg_info->ovly_sid != INVALIDm) {
            continue;
        }
        pt_info = reg_info->pt_info;
        if (pt_info->b_state != NULL) {
            if (shr_ha_mem_free(unit, pt_info->b_state) != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Could not free bucket state !!\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else {
                pt_info->b_state = NULL;
            }
        }
        if (pt_info->e_node != NULL) {
            if (shr_ha_mem_free(unit, pt_info->e_node) != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Could not free entry node !!\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else {
                pt_info->e_node = NULL;
            }
        }
    }

    if (dev_ent_ctr_desc[unit] != NULL) {
        if (shr_ha_mem_free(unit, dev_ent_ctr_desc[unit]) != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Could not free entry counter !!\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else {
            dev_ent_ctr_desc[unit] = NULL;
        }
    }
    if (dev_trans_rec[unit] != NULL) {
        if (shr_ha_mem_free(unit, dev_trans_rec[unit]) != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Could not free transaction record !!\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else {
            dev_trans_rec[unit] = NULL;
        }
    }
    if (dev_trans_undo_list[unit] != NULL) {
        if (shr_ha_mem_free(unit, dev_trans_undo_list[unit]) != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Could not free transaction undo list !!\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else {
            dev_trans_undo_list[unit] = NULL;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate HA memory space for resource manager sw state.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if this is warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_state_elems_alloc(int unit, bool warm)
{
    uint32_t req_size, alloc_size, elem_cnt;
    rm_hash_pt_reg_info_t *reg_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;
    uint16_t sub_id;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Warmboot: not yet supported !!\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (rm_hash_state_elems_amount_accum(unit));
    /* Initial HA sub-component id for rm hash */
    sub_id = BCMPTM_HA_SUBID_MAX;

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_dev_pt_reg_info_get(unit, &reg_info));
    for ( ; reg_info != NULL; reg_info = reg_info->next) {
        if (reg_info->ovly_sid != INVALIDm) {
            continue;
        }
        if (sub_id >= 0x100) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
        pt_info = reg_info->pt_info;
        /* Allocate software logical bucket state array */
        pt_info->b_state_hdl = sub_id;
        elem_cnt = pt_info->num_slbs;
        req_size = elem_cnt * sizeof(rm_hash_slb_state_t);
        alloc_size = req_size;
        if (req_size > 0) {
            pt_info->b_state = shr_ha_mem_alloc(unit,
                                                BCMMGMT_PTM_COMP_ID,
                                                (uint8_t)sub_id,
                                                &alloc_size);
            SHR_NULL_CHECK(pt_info->b_state, SHR_E_MEMORY);
            SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
            sal_memset(pt_info->b_state, 0, alloc_size);
        }
        sub_id++;
        if (sub_id >= 0x100) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
        /* Allocate entry node array */
        pt_info->e_node_hdl = sub_id;
        elem_cnt = pt_info->num_slbs * pt_info->slb_size;
        if (elem_cnt > 0) {
            if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                req_size = elem_cnt * sizeof(rm_hash_dual_ent_node_t);
                alloc_size = req_size;
                pt_info->e_node = shr_ha_mem_alloc(unit,
                                                   BCMMGMT_PTM_COMP_ID,
                                                   (uint8_t)sub_id,
                                                   &alloc_size);
            } else {
                req_size = elem_cnt * sizeof(rm_hash_shr_ent_node_t);
                alloc_size = req_size;
                pt_info->e_node = shr_ha_mem_alloc(unit,
                                                   BCMMGMT_PTM_COMP_ID,
                                                   (uint8_t)sub_id,
                                                   &alloc_size);
            }
            SHR_NULL_CHECK(pt_info->e_node, SHR_E_MEMORY);
            SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
            sal_memset(pt_info->e_node, 0, alloc_size);
        }
        sub_id++;
    }

    /* Allocate used base entries counter */
    elem_cnt = dev_state_elems_amount[unit].uent_ctr_cnt;
    req_size = (elem_cnt * sizeof(uint32_t)) + sizeof(rm_hash_dev_ent_ctr_desc_t);
    alloc_size = req_size;
    dev_ent_ctr_desc[unit] = shr_ha_mem_alloc(unit,
                                              BCMMGMT_PTM_COMP_ID,
                                              BCMPTM_HA_SUBID_RMHASH_USD_ENT_CTR,
                                              &alloc_size);
    SHR_NULL_CHECK(dev_ent_ctr_desc[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    sal_memset(dev_ent_ctr_desc[unit], 0, alloc_size);
    dev_ent_ctr_desc[unit]->cnt = elem_cnt;

    /* Allocate transaction record */
    req_size = sizeof(rm_hash_trans_record_t);
    alloc_size = req_size;
    dev_trans_rec[unit] = shr_ha_mem_alloc(unit,
                                           BCMMGMT_PTM_COMP_ID,
                                           BCMPTM_HA_SUBID_RMHASH_TRANS_REC,
                                           &alloc_size);
    SHR_NULL_CHECK(dev_trans_rec[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    sal_memset(dev_trans_rec[unit], 0, alloc_size);

    /* Allocate transaction undo list */
    req_size = (RM_HASH_INIT_UNDO_LIST_LENGTH * sizeof(rm_hash_trans_undo_instr_t))
               + sizeof(rm_hash_trans_undo_list_t);
    alloc_size = req_size;
    dev_trans_undo_list[unit] = shr_ha_mem_alloc(unit,
                                                 BCMMGMT_PTM_COMP_ID,
                                                 BCMPTM_HA_SUBID_RMHASH_UNDO_LIST,
                                                 &alloc_size);
    SHR_NULL_CHECK(dev_trans_undo_list[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    sal_memset(dev_trans_undo_list[unit], 0, alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        rm_hash_state_elems_free(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure HA memory space for software state during cold boot
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if this is warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_state_elems_config(int unit, bool warm)
{
    rm_hash_pt_reg_info_t *reg_info = NULL;
    uint32_t e_node_offset = 0, e_ctr_offset = 0;
    rm_hash_slb_state_t *b_state = NULL;
    uint32_t slb_idx;
    rm_hash_trans_record_t *trans_rec = NULL;
    rm_hash_trans_undo_list_t *trans_ulist = NULL;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Warmboot: not yet supported !!\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_dev_pt_reg_info_get(unit, &reg_info));

    for ( ; reg_info != NULL; reg_info = reg_info->next) {
        if (reg_info->ovly_sid == INVALIDm) {
            pt_info = reg_info->pt_info;
            /* Configure bucket state elements belonging to this table */
            b_state = pt_info->b_state;
            e_node_offset = 0;
            for (slb_idx = 0; slb_idx < pt_info->num_slbs; slb_idx++) {
                /* Configure entry_node_offset for each bucket state. */
                b_state->e_node_offset = e_node_offset;
                e_node_offset += pt_info->slb_size;
                b_state++;
            }
            pt_info->e_ctr_offset = e_ctr_offset;
            e_ctr_offset++;
        }
    }
    /* Clear transaction record in cold boot */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_trans_record_get(unit, &trans_rec));
    trans_rec->cseq_id = 0;
    trans_rec->trans_state = BCMPTM_RM_HASH_STATE_IDLE;
    /* Set the length of undo list to the default value in cold boot */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_trans_undo_list_get(unit, &trans_ulist));
    trans_ulist->cnt = RM_HASH_INIT_UNDO_LIST_LENGTH;

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_reg_info_dump(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Swap two entry node.
 *
 * \param [in] ent_node_a Entry node A.
 * \param [in] ent_node_b Entry node B.
 */
static void
rm_hash_shr_ent_node_swap(rm_hash_shr_ent_node_t *e_node_a,
                          rm_hash_shr_ent_node_t *e_node_b)
{
    rm_hash_shr_ent_node_t e_node = *e_node_a;
    *e_node_a = *e_node_b;
    *e_node_b = e_node;
}

/*!
 * \brief Swap two entry node.
 *
 * \param [in] ent_node_a Entry node A.
 * \param [in] ent_node_b Entry node B.
 */
static void
rm_hash_dual_ent_node_swap(rm_hash_dual_ent_node_t *e_node_a,
                           rm_hash_dual_ent_node_t *e_node_b)
{
    rm_hash_dual_ent_node_t e_node = *e_node_a;
    *e_node_a = *e_node_b;
    *e_node_b = e_node;
}

/*!
 * \brief Get the bitmap of used base entries in a base bucket.
 *
 * For a given software logical bucket and the specified base bucket in it,
 * this function will get the used base entry bitmap in the the specified
 * base bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] bb_bmp Base bucket bitmap.
 * \param [out] be_bmp Pointer to a buffer to contain the calculated bitmap.
 */
static int
rm_hash_bb_view_be_bmp_get(int unit,
                           uint16_t slb_be_bmp,
                           uint8_t bb_bmp,
                           uint8_t *be_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    switch (bb_bmp) {
    case 0x0001:
        tmp = slb_be_bmp & 0x000F;
        *be_bmp = tmp;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0002:
        tmp = slb_be_bmp & 0x00F0;
        *be_bmp = tmp >> 4;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0004:
        tmp = slb_be_bmp & 0x0F00;
        *be_bmp = tmp >> 8;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0008:
        tmp = slb_be_bmp & 0xF000;
        *be_bmp = tmp >> 12;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the bitmap of logical entry in a base bucket.
 *
 * For a given software logical bucket and the specified base bucket in it,
 * this function will get the logical entry bitmap in the the specified
 * base bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] bm Bucket mode.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] bb_bmp Base bucket bitmap.
 * \param [out] le_bmp Pointer to a buffer to contain the calculated bitmap.
 */
static int
rm_hash_bb_view_le_bmp_get(int unit,
                           uint16_t slb_le_bmp,
                           uint8_t bb_bmp,
                           uint8_t *le_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    switch (bb_bmp) {
    case 0x0001:
        tmp = slb_le_bmp & 0x000F;
        *le_bmp = tmp;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0002:
        tmp = slb_le_bmp & 0x00F0;
        *le_bmp = tmp >> 4;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0004:
        tmp = slb_le_bmp & 0x0F00;
        *le_bmp = tmp >> 8;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0008:
        tmp = slb_le_bmp & 0xF000;
        *le_bmp = tmp >> 12;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the bitmap of logical entry and actual used based entry
 *
 * For a given software logical bucket and the specified base bucket in it,
 * this function will get the used base entry and logical entry bitmap
 * in the the specified base bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] bm Bucket mode.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] bb_bmp Base bucket bitmap.
 * \param [out] ue_bmp Pointer to a buffer to contain the calculated bitmap.
 */
static int
rm_hash_bb_view_ue_bmp_get(int unit,
                           rm_hash_slb_ent_bmp_t *slb_e_bmp,
                           uint8_t bb_bmp,
                           rm_hash_bm_t bm,
                           uint8_t *ue_bmp)
{
    uint8_t be_bmp = 0, le_bmp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_bb_view_be_bmp_get(unit,
                                    slb_e_bmp->be_bmp,
                                    bb_bmp,
                                    &be_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_bb_view_le_bmp_get(unit,
                                    slb_e_bmp->le_bmp[bm],
                                    bb_bmp,
                                    &le_bmp));
    *ue_bmp = be_bmp | le_bmp;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get free base entry info
 *
 * Get free base entry info for the given bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] be_bmp Used base entry bitmap.
 * \param [in] e_size Entry size.
 * \param [out] fe_info Pointer to an rm_hash_base_bkt_free_ent_t structure.
 */
static int
rm_hash_bb_free_space_info_get(int unit,
                               uint8_t be_bmp,
                               uint8_t e_size,
                               const rm_hash_base_bkt_free_ent_t **fe_info)
{

    SHR_FUNC_ENTER(unit);

    switch (e_size) {
    case 0x01:
        *fe_info = &sw_ent_free_info[be_bmp];
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x02:
        *fe_info = &dw_ent_free_info[be_bmp];
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x04:
        *fe_info = &qw_ent_free_info[be_bmp];
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BMN in a BMN bucket.
 *
 * Try to find free slot for an entry BMN in a BMN bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] he_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [out] e_loc_found Flag indicating if free slot has been found.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 */
static int
rm_hash_bmn_ent_free_slot_srch_in_bmn_slb(int unit,
                                          rm_hash_pt_info_t *pt_info,
                                          const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                          rm_hash_ent_slb_info_t *slb_info_list,
                                          bool *e_loc_found,
                                          rm_hash_ent_loc_t *e_loc)
{
    uint8_t lbank = 0, fbe_cnt = 0, max_fbe_num = 0, idx = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_base_bkt_free_ent_t *fe = NULL, *qfe = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        b_state = pt_info->b_state + slb_info_list[idx].slb;
        SHR_IF_ERR_EXIT
            (rm_hash_bb_free_space_info_get(unit,
                                            (uint8_t)b_state->e_bmp.be_bmp,
                                            he_info->num_base_entries,
                                            &fe));
        if (fe->avail == FALSE) {
            continue;
        }
        fbe_cnt = fe->fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            lbank = idx;
            qfe = fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        e_loc->lbank = lbank;
        e_loc->bb_bmp = 0x01;
        e_loc->be_bmp = qfe->bmp;
        e_loc->le_bmp = qfe->bmp;
        *e_loc_found = TRUE;
    } else {
        *e_loc_found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM0 in a BM0 bucket.
 *
 * Try to find free slot for an entry BM0 in a BM0 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] he_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [out] e_loc_found Flag indicating if free slot has been found.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 */
static int
rm_hash_bm0_ent_free_slot_srch_in_bm0_slb(int unit,
                                          rm_hash_pt_info_t *pt_info,
                                          const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                          rm_hash_ent_slb_info_t *slb_info_list,
                                          bool *e_loc_found,
                                          rm_hash_ent_loc_t *e_loc)
{
    uint8_t lbank = 0, fbe_cnt = 0, max_fbe_num = 0, idx = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_base_bkt_free_ent_t *fe = NULL, *qfe = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        b_state = pt_info->b_state + slb_info_list[idx].slb;
        SHR_IF_ERR_EXIT
            (rm_hash_bb_free_space_info_get(unit,
                                            (uint8_t)b_state->e_bmp.be_bmp,
                                            he_info->num_base_entries,
                                            &fe));
        if (fe->avail == FALSE) {
            continue;
        }
        fbe_cnt = fe->fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            lbank = idx;
            qfe = fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        e_loc->lbank = lbank;
        e_loc->bb_bmp = 0x01;
        e_loc->be_bmp = qfe->bmp;
        e_loc->le_bmp = qfe->bmp;
        *e_loc_found = TRUE;
    } else {
        *e_loc_found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM0 in a BM1 bucket.
 *
 * Try to find free slot for an entry BM0 in a BM1 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] he_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [out] e_loc_found Flag indicating if free slot has been found.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 */
static int
rm_hash_bm0_ent_free_slot_srch_in_bm1_slb(int unit,
                                          rm_hash_pt_info_t *pt_info,
                                          const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                          rm_hash_ent_slb_info_t *slb_info_list,
                                          bool *e_loc_found,
                                          rm_hash_ent_loc_t *e_loc)
{
    uint8_t lbank = 0, fbe_cnt = 0, max_fbe_num = 0, idx = 0, vbb_bmp = 0;
    uint8_t bb_idx = 0, bb_bmp = 0, qbb_bmp = 0;
    uint8_t ue_bmp = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_base_bkt_free_ent_t *fe = NULL, *qfe = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        b_state = pt_info->b_state + slb_info_list[idx].slb;
        vbb_bmp = slb_info_list[idx].vbb_bmp;
        for (bb_idx = 0; bb_idx < RM_HASH_BM1_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            if (bb_bmp & vbb_bmp) {
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_view_ue_bmp_get(unit,
                                                &b_state->e_bmp,
                                                bb_bmp,
                                                RM_HASH_BM_0,
                                                &ue_bmp));
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_free_space_info_get(unit,
                                                    ue_bmp,
                                                    he_info->num_base_entries,
                                                    &fe));
                break;
            }
        }
        if (fe->avail == FALSE) {
            continue;
        }
        fbe_cnt = fe->fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            lbank = idx;
            qbb_bmp = bb_bmp;
            qfe = fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        e_loc->lbank = lbank;
        e_loc->bb_bmp = qbb_bmp;
        e_loc->be_bmp = qfe->bmp;
        e_loc->le_bmp = qfe->bmp;
        *e_loc_found = TRUE;
    } else {
        *e_loc_found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM1 in a BM1 bucket.
 *
 * Try to find free slot for an entry BM1 in a BM1 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] he_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [out] e_loc_found Flag indicating if free slot has been found.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 */
static int
rm_hash_bm1_ent_free_slot_srch_in_bm1_slb(int unit,
                                          rm_hash_pt_info_t *pt_info,
                                          const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                          rm_hash_ent_slb_info_t *slb_info_list,
                                          bool *e_loc_found,
                                          rm_hash_ent_loc_t *e_loc)
{
    uint8_t qual_lbank = 0, fbe_cnt = 0, max_fbe_num = 0;
    uint8_t hlb_fbe_cnt = 0, hlb_bb_bmp = 0, idx = 0;
    uint8_t bb_idx = 0, bb_bmp = 0, qual_bb_bmp = 0;
    uint8_t ue_bmp = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_base_bkt_free_ent_t *fe = NULL, *qfe = NULL, *hlb_qfe = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        b_state = pt_info->b_state + slb_info_list[idx].slb;
        hlb_fbe_cnt = 0;
        hlb_bb_bmp = 0;
        hlb_qfe = NULL;

        for (bb_idx = 0; bb_idx < RM_HASH_BM1_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            SHR_IF_ERR_EXIT
                (rm_hash_bb_view_ue_bmp_get(unit,
                                            &b_state->e_bmp,
                                            bb_bmp,
                                            RM_HASH_BM_1,
                                            &ue_bmp));
            SHR_IF_ERR_EXIT
                (rm_hash_bb_free_space_info_get(unit,
                                                ue_bmp,
                                                he_info->num_base_entries,
                                                &fe));
            if ((fe->avail == TRUE) && (hlb_qfe == NULL)) {
                /*
                 * A free slot has been found in one of the base buckets
                 * of the hardware logical bucket. The corresponding
                 * base bucket bitmap and free entry info in this base
                 * bucket will be recorded.
                 */
                hlb_bb_bmp = bb_bmp;
                hlb_qfe = fe;
            }
            hlb_fbe_cnt += fe->fbe_cnt;
        }

        if (hlb_qfe == NULL) {
            /*
             * There is no free space in the hardware logical bucket
             * in this bank.
             */
            continue;
        }

        fbe_cnt = hlb_fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qual_lbank = idx;
            qual_bb_bmp = hlb_bb_bmp;
            qfe = hlb_qfe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE * 2) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        e_loc->lbank  = qual_lbank;
        e_loc->bb_bmp = qual_bb_bmp;
        e_loc->be_bmp = qfe->bmp;
        e_loc->le_bmp = bm1e_be_to_le_map[qfe->bmp].le_bmp;
        *e_loc_found = TRUE;
    } else {
        *e_loc_found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM0 in a BM2 bucket.
 *
 * Try to find free slot for an entry BM0 in a BM2 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] he_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [out] e_loc_found Flag indicating if free slot has been found.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 */
static int
rm_hash_bm0_ent_free_slot_srch_in_bm2_slb(int unit,
                                          rm_hash_pt_info_t *pt_info,
                                          const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                          rm_hash_ent_slb_info_t *slb_info_list,
                                          bool *e_loc_found,
                                          rm_hash_ent_loc_t *e_loc)
{
    uint8_t qual_lbank = 0, fbe_cnt = 0, max_fbe_num = 0;
    uint8_t idx = 0;
    uint8_t bb_idx = 0, vbb_bmp = 0, bb_bmp = 0, qual_bb_bmp = 0;
    uint8_t ue_bmp = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_base_bkt_free_ent_t *fe = NULL, *qual_fe = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        b_state = pt_info->b_state + slb_info_list[idx].slb;
        vbb_bmp = slb_info_list[idx].vbb_bmp;

        for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            if (bb_bmp & vbb_bmp) {
                /* The only one valid base bucket has been identified */
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_view_ue_bmp_get(unit,
                                                &b_state->e_bmp,
                                                bb_bmp,
                                                RM_HASH_BM_0,
                                                &ue_bmp));
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_free_space_info_get(unit,
                                                    ue_bmp,
                                                    he_info->num_base_entries,
                                                    &fe));
                /*
                 * No matter if free space has been found,
                 * terminate the iteration over base bucket.
                 */
                break;
            }
        }
        if (fe->avail == FALSE) {
            /*
             * There is no free space in the hardware logical bucket
             * in this bank.
             */
            continue;
        }
        fbe_cnt = fe->fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qual_lbank = idx;
            qual_bb_bmp = bb_bmp;
            qual_fe = fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE) {
                break;
            }
        }
    }

    if (qual_fe != NULL) {
        e_loc->lbank  = qual_lbank;
        e_loc->bb_bmp = qual_bb_bmp;
        e_loc->be_bmp = qual_fe->bmp;
        e_loc->le_bmp = qual_fe->bmp;
        *e_loc_found = TRUE;
    } else {
        *e_loc_found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM1 in a BM2 bucket.
 *
 * Try to find free slot for an entry BM1 in a BM2 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] he_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [out] e_loc_found Flag indicating if free slot has been found.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 */
static int
rm_hash_bm1_ent_free_slot_srch_in_bm2_slb(int unit,
                                          rm_hash_pt_info_t *pt_info,
                                          const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                          rm_hash_ent_slb_info_t *slb_info_list,
                                          bool *e_loc_found,
                                          rm_hash_ent_loc_t *e_loc)
{
    uint8_t qual_lbank = 0, fbe_cnt = 0, max_fbe_num = 0;
    uint8_t idx = 0, hlb_fbe_cnt = 0, hlb_bb_bmp = 0;
    uint8_t bb_idx = 0, vbb_bmp = 0, bb_bmp = 0, qual_bb_bmp = 0;
    uint8_t ue_bmp = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_base_bkt_free_ent_t *fe = NULL, *qual_fe = NULL, *hlb_qual_fe = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        b_state = pt_info->b_state + slb_info_list[idx].slb;
        vbb_bmp = slb_info_list[idx].vbb_bmp;

        hlb_fbe_cnt = 0;
        hlb_bb_bmp = 0;
        hlb_qual_fe = NULL;

        for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            if (bb_bmp & vbb_bmp) {
                /* One of valid base buckets has been determined */
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_view_ue_bmp_get(unit,
                                                &b_state->e_bmp,
                                                bb_bmp,
                                                RM_HASH_BM_1,
                                                &ue_bmp));
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_free_space_info_get(unit,
                                                    ue_bmp,
                                                    he_info->num_base_entries,
                                                    &fe));
                if ((fe->avail == TRUE) && (hlb_qual_fe == NULL)) {
                    /*
                     * A free slot has been found in one of the base buckets
                     * of this hardware logical buckets. The corresponding
                     * base bucket bitmap and free entry info in this base
                     * bucket will be recorded.
                     */
                    hlb_bb_bmp = bb_bmp;
                    hlb_qual_fe = fe;
                }
                hlb_fbe_cnt += fe->fbe_cnt;
            }
        }

        if (hlb_qual_fe == NULL) {
            /*
             * There is no free space in the hardware logical bucket
             * in this bank.
             */
            continue;
        }

        fbe_cnt = hlb_fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qual_lbank = idx;
            qual_bb_bmp = hlb_bb_bmp;
            qual_fe = hlb_qual_fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE * 2) {
                break;
            }
        }
    }

    if (qual_fe != NULL) {
        e_loc->lbank  = qual_lbank;
        e_loc->bb_bmp = qual_bb_bmp;
        e_loc->be_bmp = qual_fe->bmp;
        e_loc->le_bmp = bm1e_be_to_le_map[e_loc->be_bmp].le_bmp;
        *e_loc_found = TRUE;
    } else {
        *e_loc_found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *
 * Examine if there is enough space for a given size entry in a given bucket.
 */
static int
rm_hash_bm2_ent_free_slot_srch_in_bm2_slb(int unit,
                                          rm_hash_pt_info_t *pt_info,
                                          const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                          rm_hash_ent_slb_info_t *slb_info_list,
                                          bool *e_loc_found,
                                          rm_hash_ent_loc_t *e_loc)
{
    uint8_t qual_lbank = 0, fbe_cnt = 0, max_fbe_num = 0;
    uint8_t idx = 0, hlb_fbe_cnt = 0, hlb_bb_bmp = 0;
    uint8_t bb_idx = 0, vbb_bmp = 0, bb_bmp = 0, qual_bb_bmp = 0;
    uint8_t ue_bmp = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_base_bkt_free_ent_t *fe = NULL, *qual_fe = NULL, *hlb_qual_fe = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        b_state = pt_info->b_state + slb_info_list[idx].slb;
        vbb_bmp = slb_info_list[idx].vbb_bmp;

        hlb_fbe_cnt = 0;
        hlb_bb_bmp = 0;
        hlb_qual_fe = NULL;

        for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            if (bb_bmp & vbb_bmp) {
                /* One of valid base buckets has been determined */
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_view_ue_bmp_get(unit,
                                                &b_state->e_bmp,
                                                bb_bmp,
                                                RM_HASH_BM_2,
                                                &ue_bmp));
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_free_space_info_get(unit,
                                                    ue_bmp,
                                                    he_info->num_base_entries,
                                                    &fe));
                if ((fe->avail == TRUE) && (hlb_qual_fe == NULL)) {
                    /*
                     * A free slot has been found in one of the base buckets
                     * of this hardware logical buckets. The corresponding
                     * base bucket bitmap and free entry info in this base
                     * bucket will be recorded.
                     */
                    hlb_bb_bmp = bb_bmp;
                    hlb_qual_fe = fe;
                }
                hlb_fbe_cnt += fe->fbe_cnt;
            }
        }

        if (hlb_qual_fe == NULL) {
            /*
             * There is no free space in the hardware logical bucket
             * in this bank.
             */
            continue;
        }

        fbe_cnt = hlb_fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qual_lbank = idx;
            qual_bb_bmp = hlb_bb_bmp;
            qual_fe = hlb_qual_fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE * 4) {
                break;
            }
        }
    }

    if (qual_fe != NULL) {
        e_loc->lbank  = qual_lbank;
        e_loc->bb_bmp = qual_bb_bmp;
        e_loc->be_bmp = qual_fe->bmp;
        e_loc->le_bmp = 0x0F;
        *e_loc_found = TRUE;
    } else {
        *e_loc_found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_ent_slb_view_be_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_be_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    tmp = 0x000F & e_loc->be_bmp;
    switch (e_loc->bb_bmp) {
    case 0x0001:
        *slb_be_bmp = tmp;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0002:
        *slb_be_bmp = tmp << 4;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0004:
        *slb_be_bmp = tmp << 8;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0008:
        *slb_be_bmp = tmp << 12;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_slb_view_le_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_le_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    tmp = 0x000F & e_loc->le_bmp;
    switch (e_loc->bb_bmp) {
    case 0x0001:
        *slb_le_bmp = tmp;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0002:
        *slb_le_bmp = tmp << 4;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0004:
        *slb_le_bmp = tmp << 8;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    case 0x0008:
        *slb_le_bmp = tmp << 12;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_shr_ent_node_allocate(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     rm_hash_slb_state_t *b_state,
                                     rm_hash_shr_ent_node_t **pe_node)
{
    rm_hash_shr_ent_node_t *e_node = pt_info->e_node;
    uint32_t offset = b_state->e_node_offset;

    SHR_FUNC_ENTER(unit);

    *pe_node = e_node + offset + b_state->ve_cnt;
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_dual_ent_node_allocate(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      rm_hash_slb_state_t *b_state,
                                      rm_hash_dual_ent_node_t **pe_node)
{
    rm_hash_dual_ent_node_t *e_node = pt_info->e_node;
    uint32_t offset = b_state->e_node_offset;

    SHR_FUNC_ENTER(unit);

    *pe_node = e_node + offset + b_state->ve_cnt;
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_dual_ent_node_insert(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    rm_hash_slb_state_t *b_state,
                                    rm_hash_dual_ent_node_t *de_node)
{
    uint16_t be_bmp, le_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                &de_node->e_loc,
                                                &be_bmp));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                &de_node->e_loc,
                                                &le_bmp));

    b_state->e_bmp.be_bmp |= be_bmp;
    b_state->e_bmp.le_bmp[de_node->bm] |= le_bmp;
    b_state->ve_cnt++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_shr_ent_node_insert(int unit,
                                   rm_hash_pt_info_t *pt_info,
                                   rm_hash_slb_state_t *b_state,
                                   rm_hash_shr_ent_node_t *se_node)
{
    uint16_t be_bmp, le_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                &se_node->e_loc,
                                                &be_bmp));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                &se_node->e_loc,
                                                &le_bmp));

    b_state->e_bmp.be_bmp |= be_bmp;
    b_state->e_bmp.le_bmp[se_node->bm] |= le_bmp;
    b_state->ve_cnt++;

exit:
    SHR_FUNC_EXIT();
}

int bcmptm_rm_hash_shr_ent_node_delete(int unit,
                                       rm_hash_pt_info_t *pt_info,
                                       rm_hash_slb_state_t *b_state,
                                       uint8_t bb_bmp,
                                       uint8_t be_bmp,
                                       rm_hash_shr_ent_node_t **pe_node)
{
    rm_hash_shr_ent_node_t *e_node = pt_info->e_node, *last_e_node = NULL;
    uint32_t offset = b_state->e_node_offset;
    uint8_t idx;
    uint16_t slb_be_bmp = 0, slb_le_bmp = 0;

    SHR_FUNC_ENTER(unit);

    if (b_state->ve_cnt == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    e_node = e_node + offset;
    last_e_node = e_node + (b_state->ve_cnt - 1);

    for (idx = 0; idx < b_state->ve_cnt; idx++) {
        if (((e_node + idx)->e_loc.bb_bmp == bb_bmp) &&
            ((e_node + idx)->e_loc.be_bmp == be_bmp)) {
            if (idx != (b_state->ve_cnt - 1)) {
                rm_hash_shr_ent_node_swap(e_node + idx, last_e_node);
            }
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &last_e_node->e_loc,
                                                        &slb_be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &last_e_node->e_loc,
                                                        &slb_le_bmp));
            b_state->e_bmp.be_bmp &= ~slb_be_bmp;
            b_state->e_bmp.le_bmp[last_e_node->bm] &= ~slb_le_bmp;
            b_state->ve_cnt--;
            if (pe_node != NULL) {
                *pe_node = last_e_node;
            }
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_dual_ent_node_delete(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    rm_hash_slb_state_t *b_state,
                                    uint8_t bb_bmp,
                                    uint16_t be_bmp,
                                    rm_hash_dual_ent_node_t **pe_node)
{
    rm_hash_dual_ent_node_t *e_node = pt_info->e_node, *last_e_node = NULL;
    uint32_t offset = b_state->e_node_offset;
    uint8_t idx;
    uint16_t slb_be_bmp = 0, slb_le_bmp = 0;

    SHR_FUNC_ENTER(unit);

    if (b_state->ve_cnt == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    e_node = e_node + offset;
    last_e_node = e_node + (b_state->ve_cnt - 1);

    for (idx = 0; idx < b_state->ve_cnt; idx++) {
        if (((e_node + idx)->e_loc.bb_bmp == bb_bmp) &&
            ((e_node + idx)->e_loc.be_bmp == be_bmp)) {
            if (idx != (b_state->ve_cnt - 1)) {
                rm_hash_dual_ent_node_swap(e_node + idx, last_e_node);
            }
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &last_e_node->e_loc,
                                                        &slb_be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &last_e_node->e_loc,
                                                        &slb_le_bmp));
            b_state->e_bmp.be_bmp &= ~slb_be_bmp;
            b_state->e_bmp.le_bmp[last_e_node->bm] &= ~slb_le_bmp;
            b_state->ve_cnt--;
            if (pe_node != NULL) {
                *pe_node = last_e_node;
            }
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_shr_ent_node_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint32_t e_node_offset,
                                rm_hash_shr_ent_node_t **pe_node)
{
    rm_hash_shr_ent_node_t *e_node = pt_info->e_node;

    SHR_FUNC_ENTER(unit);

    e_node = e_node + e_node_offset;
    *pe_node = e_node;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_dual_ent_node_get(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint32_t e_node_offset,
                                 rm_hash_dual_ent_node_t **pe_node)
{
    rm_hash_dual_ent_node_t *e_node = pt_info->e_node;

    SHR_FUNC_ENTER(unit);

    e_node = e_node + e_node_offset;
    *pe_node = e_node;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_bkt_free_ent_space_get(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      rm_hash_slb_ent_bmp_t *slb_e_bmp,
                                      rm_hash_req_ent_info_t *e_info,
                                      bool *accom,
                                      rm_hash_ent_loc_t *e_loc)
{
    uint8_t bb_bmp = 0, bb_idx = 0;
    uint8_t ue_bmp = 0;
    const rm_hash_base_bkt_free_ent_t *fe = NULL;

    SHR_FUNC_ENTER(unit);

    *accom = FALSE;
    for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
        bb_bmp = 0x01 << bb_idx;
        if (bb_bmp & e_info->vbb_bmp) {
            SHR_IF_ERR_EXIT
                (rm_hash_bb_view_ue_bmp_get(unit,
                                            slb_e_bmp,
                                            bb_bmp,
                                            e_info->e_bm,
                                            &ue_bmp));
            SHR_IF_ERR_EXIT
                (rm_hash_bb_free_space_info_get(unit,
                                                ue_bmp,
                                                e_info->e_size,
                                                &fe));
            if (fe->avail == TRUE) {
                e_loc->bb_bmp = bb_bmp;
                e_loc->be_bmp = fe->bmp;
                switch (e_info->e_bm) {
                case RM_HASH_BM_N:
                case RM_HASH_BM_0:
                    e_loc->le_bmp = e_loc->be_bmp;
                    break;
                case RM_HASH_BM_1:
                    e_loc->le_bmp = bm1e_be_to_le_map[e_loc->be_bmp].le_bmp;
                    break;
                case RM_HASH_BM_2:
                    e_loc->le_bmp = 0x0F;
                    break;
                case RM_HASH_BM_CNT:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                *accom = TRUE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_free_ent_space_search(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                     rm_hash_ent_slb_info_t *slb_info_list,
                                     rm_hash_bm_t e_bm,
                                     bool *e_loc_found,
                                     rm_hash_ent_loc_t *e_loc)
{
    fn_ent_free_slot_srch_t func = NULL;
    rm_hash_bm_t t_bm = RM_HASH_BM_N;
    static fn_ent_free_slot_srch_t srch_func[RM_HASH_BM_CNT][RM_HASH_BM_CNT] = {
        {
         rm_hash_bmn_ent_free_slot_srch_in_bmn_slb,
         NULL,
         NULL,
         NULL
        },
        {
         NULL,
         rm_hash_bm0_ent_free_slot_srch_in_bm0_slb,
         NULL,
         NULL
        },
        {
         NULL,
         rm_hash_bm0_ent_free_slot_srch_in_bm1_slb,
         rm_hash_bm1_ent_free_slot_srch_in_bm1_slb,
         NULL
        },
        {
         NULL,
         rm_hash_bm0_ent_free_slot_srch_in_bm2_slb,
         rm_hash_bm1_ent_free_slot_srch_in_bm2_slb,
         rm_hash_bm2_ent_free_slot_srch_in_bm2_slb
        }
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_max_bm_get(unit, pt_info->bmf, &t_bm));
    func = srch_func[t_bm][e_bm];
    if (func != NULL) {
        SHR_RETURN_VAL_EXIT
            (func(unit, pt_info, he_info, slb_info_list, e_loc_found, e_loc));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_used_bent_ctr_get(int unit,
                                 uint16_t ctr_offset,
                                 uint32_t **pctr)
{
    rm_hash_dev_ent_ctr_desc_t *ent_ctr_desc = dev_ent_ctr_desc[unit];
    uint32_t *ctr = NULL;

    SHR_FUNC_ENTER(unit);

    ctr = ent_ctr_desc->array + ctr_offset;
    *pctr = ctr;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_differ_collision_get(int unit,
                                    bool *differ_collision)
{
    rm_hash_dev_ent_ctr_desc_t *ent_ctr_desc = dev_ent_ctr_desc[unit];

    SHR_FUNC_ENTER(unit);

    *differ_collision = ent_ctr_desc->differ_collision;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_differ_collision_set(int unit,
                                    bool differ_collision)
{
    rm_hash_dev_ent_ctr_desc_t *ent_ctr_desc = dev_ent_ctr_desc[unit];

    SHR_FUNC_ENTER(unit);

    ent_ctr_desc->differ_collision = differ_collision;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_table_fullness_get(int unit,
                                  rm_hash_pt_info_t *pt_info,
                                  uint8_t e_size,
                                  bool *full)
{
    uint32_t *uent_ctr = NULL;

    SHR_FUNC_ENTER(unit);

    *full = FALSE;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_used_bent_ctr_get(unit,
                                          pt_info->e_ctr_offset,
                                          &uent_ctr));
    if ((*uent_ctr + e_size) > (pt_info->num_slbs * pt_info->slb_size)) {
        *full = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_bkt_state_set(int unit,
                             rm_hash_pt_info_t *pt_info,
                             rm_hash_ent_loc_t *e_loc,
                             rm_hash_ent_slb_info_t *slb_info,
                             bcmdrd_sid_t sid,
                             uint32_t e_idx,
                             uint32_t e_sig,
                             rm_hash_bm_t bm,
                             uint8_t e_size)
{
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_shr_ent_node_t  *se_node = NULL;
    rm_hash_dual_ent_node_t *de_node = NULL;
    uint32_t offset = 0, *ue_ctr = NULL;
    uint8_t idx;

    SHR_FUNC_ENTER(unit);

    offset = slb_info[e_loc->lbank].slb;
    b_state = pt_info->b_state + offset;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "New entry node inserting...\n")));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "physical mem:[%s], "
                            "lbank:[%d], "
                            "base bucket bitmap:[%d], "
                            "base entry bitmap:[%d], "
                            "logical entry bitmap:[%d] \n"),
                            bcmdrd_pt_sid_to_name(unit, sid),
                            e_loc->lbank,
                            e_loc->bb_bmp,
                            e_loc->be_bmp,
                            e_loc->le_bmp));
    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_allocate(unit,
                                                   pt_info,
                                                   b_state,
                                                   &de_node));
        de_node->sid = sid;
        de_node->e_idx = e_idx;
        de_node->sig = e_sig;
        de_node->bm = bm;
        de_node->e_loc = *e_loc;
        for (idx = 0; idx < pt_info->num_lbanks; idx++) {
            de_node->slb_info[idx] = slb_info[idx];
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "software logical bucket on bank[%d]:[%d], "
                                    "valid base bucket bitmap:[%d]\n"),
                                    idx,
                                    de_node->slb_info[idx].slb,
                                    de_node->slb_info[idx].vbb_bmp));
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_insert(unit,
                                                 pt_info,
                                                 b_state,
                                                 de_node));
        /* pt_info can record the counter directly, instead of offset */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_used_bent_ctr_get(unit,
                                              pt_info->e_ctr_offset,
                                              &ue_ctr));
        *ue_ctr += e_size;
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_ulist_dual_ent_insert(unit,
                                                        offset,
                                                        pt_info->e_ctr_offset,
                                                        de_node,
                                                        BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_allocate(unit,
                                                  pt_info,
                                                  b_state,
                                                  &se_node));
        se_node->sid = sid;
        se_node->e_idx = e_idx;
        se_node->sig = e_sig;
        se_node->bm = bm;
        se_node->e_loc = *e_loc;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "entry node: %p, "
                               "physical mem:[%s], "
                               "sig:[%d], "
                               "bucket mode:[%d], "
                               "entry index:[%d], "),
                               (void*)se_node,
                               bcmdrd_pt_sid_to_name(unit, se_node->sid),
                               se_node->sig,
                               se_node->bm,
                               se_node->e_idx));
        for (idx = 0; idx < pt_info->num_lbanks; idx++) {
            se_node->slb_info[idx] = slb_info[idx];
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "software logical bucket on bank[%d]:[%d], "
                                    "valid base bucket bitmap:[%d]\n"),
                                    idx,
                                    se_node->slb_info[idx].slb,
                                    se_node->slb_info[idx].vbb_bmp));
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_insert(unit,
                                                pt_info,
                                                b_state,
                                                se_node));
        /* pt_info can record the counter directly, instead of offset */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_used_bent_ctr_get(unit,
                                              pt_info->e_ctr_offset,
                                              &ue_ctr));
        *ue_ctr += e_size;
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_ulist_shr_ent_insert(unit,
                                                       offset,
                                                       pt_info->e_ctr_offset,
                                                       se_node,
                                                       BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_bkt_state_clear(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint32_t slb,
                               rm_hash_ent_loc_t *e_loc)
{
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_shr_ent_node_t *se_node = NULL;
    rm_hash_dual_ent_node_t *de_node = NULL;
    uint32_t *ue_ctr = NULL;

    SHR_FUNC_ENTER(unit);

    b_state = pt_info->b_state + slb;
    if (pt_info->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_dual_ent_node_delete(unit,
                                                 pt_info,
                                                 b_state,
                                                 e_loc->bb_bmp,
                                                 e_loc->be_bmp,
                                                 &de_node));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_used_bent_ctr_get(unit,
                                              pt_info->e_ctr_offset,
                                              &ue_ctr));
        *ue_ctr -= e_size_map[e_loc->be_bmp].size;
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
         SHR_IF_ERR_EXIT
             (bcmptm_rm_hash_trans_ulist_dual_ent_insert(unit,
                                                         slb,
                                                         pt_info->e_ctr_offset,
                                                         de_node,
                                                         BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_shr_ent_node_delete(unit,
                                                pt_info,
                                                b_state,
                                                e_loc->bb_bmp,
                                                e_loc->be_bmp,
                                                &se_node));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_used_bent_ctr_get(unit,
                                              pt_info->e_ctr_offset,
                                              &ue_ctr));
        *ue_ctr -= e_size_map[e_loc->be_bmp].size;
        /*
         * Bucket state and entry node has been updated, corresponding
         * undo operation will be recorded in the undo list.
         */
         SHR_IF_ERR_EXIT
             (bcmptm_rm_hash_trans_ulist_shr_ent_insert(unit,
                                                        slb,
                                                        pt_info->e_ctr_offset,
                                                        se_node,
                                                        BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_eligible_for_shr_ent_compare(int unit,
                                            const bcmptm_rm_hash_lt_info_t *lt_info,
                                            rm_hash_grp_info_t *grp_info,
                                            rm_hash_shr_ent_node_t *e_node,
                                            rm_hash_bm_t e_bm,
                                            uint32_t e_sig,
                                            bool *eligible,
                                            uint8_t *vhei_idx)
{
    uint8_t idx;
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    bcmdrd_sid_t ve_sid;
    uint8_t ve_size;

    SHR_FUNC_ENTER(unit);

    *eligible = FALSE;
    if (lt_info->rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /*
     * For a given entry node, only all the following conditions are satisfied,
     * the entry node is eligible for entry compare operation.
     * - Its sid is one of valid sids belonging to the ltid.
     * - Its size is one of valid size of the ltid.
     * - Its bucket mode equals to the requested one.
     * - Its signature equals to the requested one.
     */
    for (idx = 0; idx < grp_info->num_vhei; idx++) {
        hei = lt_info->hw_entry_info + grp_info->vhei_idx[idx];
        ve_sid = hei->sid[0];
        ve_size = hei->num_base_entries;
        if ((e_node->sid == ve_sid) && (e_size_map[e_node->e_loc.be_bmp].size == ve_size) &&
            (e_node->bm == e_bm) && (e_node->sig == e_sig)) {
            *eligible = TRUE;
            *vhei_idx = grp_info->vhei_idx[idx];
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_eligible_for_dual_ent_compare(int unit,
                                             const bcmptm_rm_hash_lt_info_t *lt_info,
                                             rm_hash_grp_info_t *grp_info,
                                             rm_hash_dual_ent_node_t *e_node,
                                             rm_hash_bm_t e_bm,
                                             uint32_t e_sig,
                                             bool *eligible,
                                             uint8_t *vhei_idx)
{
    uint8_t idx;
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    bcmdrd_sid_t ve_sid;
    uint8_t ve_size;

    SHR_FUNC_ENTER(unit);

    *eligible = FALSE;
    if (lt_info->rm_more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /*
     * For a given entry node, only all the following conditions are satisfied,
     * the entry node is eligible for entry compare operation.
     * - Its sid is one of valid sids belonging to the ltid.
     * - Its size is one of valid size of the ltid.
     * - Its bucket mode equals to the requested one.
     * - Its signature equals to the requested one.
     */
    for (idx = 0; idx < grp_info->num_vhei; idx++) {
        hei = lt_info->hw_entry_info + grp_info->vhei_idx[idx];
        ve_sid = hei->sid[0];
        ve_size = hei->num_base_entries;
        if ((e_node->sid == ve_sid) && (e_size_map[e_node->e_loc.be_bmp].size == ve_size) &&
            (e_node->bm == e_bm) && (e_node->sig == e_sig)) {
            *eligible = TRUE;
            *vhei_idx = grp_info->vhei_idx[idx];
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_record_get(int unit,
                                rm_hash_trans_record_t **ptrans_rec)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ptrans_rec, SHR_E_PARAM);

    *ptrans_rec = dev_trans_rec[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_undo_list_get(int unit,
                                   rm_hash_trans_undo_list_t **ptrans_ulist)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ptrans_ulist, SHR_E_PARAM);

    *ptrans_ulist = dev_trans_undo_list[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_undo_list_invalidate(int unit,
                                          rm_hash_trans_undo_list_t *trans_ulist)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(trans_ulist, SHR_E_PARAM);

    sal_memset(&trans_ulist->array[0],
               0,
               trans_ulist->curr * sizeof(rm_hash_trans_undo_instr_t));
    trans_ulist->curr = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_ulist_dual_ent_insert(int unit,
                                           uint32_t b_state_offset,
                                           uint16_t ue_ctr_offset,
                                           rm_hash_dual_ent_node_t *de_node,
                                           rm_hash_trans_undo_opcode_t opcode)
{
    rm_hash_trans_undo_list_t *trans_ulist = NULL;
    uint32_t curr = 0, exst_ulsize = 0, incr_ulsize = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_trans_undo_list_get(unit, &trans_ulist));
    if (trans_ulist->curr >= trans_ulist->cnt) {
        /* There is no space in the undo list, additional space is required */
        exst_ulsize = (sizeof(rm_hash_trans_undo_instr_t) * trans_ulist->cnt) +
                       sizeof(rm_hash_trans_undo_list_t);
        incr_ulsize =  sizeof(rm_hash_trans_undo_instr_t) *
                       RM_HASH_INIT_UNDO_LIST_LENGTH;
        /*
         * Realloc HA space for accomandating new undo instruction.
         * The contents in the old buffer should have already been copied into
         * new buffer.
         */
        trans_ulist = shr_ha_mem_realloc(unit,
                                         trans_ulist,
                                         exst_ulsize + incr_ulsize);
        if (trans_ulist == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
        dev_trans_undo_list[unit] = trans_ulist;
        trans_ulist->cnt += RM_HASH_INIT_UNDO_LIST_LENGTH;
    }
    curr = trans_ulist->curr;
    trans_ulist->array[curr].opcode = opcode;
    trans_ulist->array[curr].b_state_offset = b_state_offset;
    trans_ulist->array[curr].ube_ctr_offset = ue_ctr_offset;
    trans_ulist->array[curr].hash_type = BCMPTM_RM_HASH_TYPE_DUAL;
    trans_ulist->array[curr].e_node.de_node = *de_node;

    trans_ulist->curr++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_ulist_shr_ent_insert(int unit,
                                          uint32_t b_state_offset,
                                          uint16_t ue_ctr_offset,
                                          rm_hash_shr_ent_node_t *se_node,
                                          rm_hash_trans_undo_opcode_t opcode)
{
    rm_hash_trans_undo_list_t *trans_ulist = NULL;
    uint32_t curr = 0, exst_ulsize = 0, incr_ulsize = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_trans_undo_list_get(unit, &trans_ulist));
    if (trans_ulist->curr >= trans_ulist->cnt) {
        /* There is no space in the undo list, additional space is required */
        exst_ulsize = (sizeof(rm_hash_trans_undo_instr_t) * trans_ulist->cnt) +
                       sizeof(rm_hash_trans_undo_list_t);
        incr_ulsize =  sizeof(rm_hash_trans_undo_instr_t) * RM_HASH_INIT_UNDO_LIST_LENGTH;
        /*
         * Realloc HA space for accomandating new undo instruction.
         * The contents in the old buffer should have already been copied into
         * new buffer.
         */
        trans_ulist = shr_ha_mem_realloc(unit,
                                         trans_ulist,
                                         exst_ulsize + incr_ulsize);
        if (trans_ulist == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
        dev_trans_undo_list[unit] = trans_ulist;
        trans_ulist->cnt += RM_HASH_INIT_UNDO_LIST_LENGTH;
    }
    curr = trans_ulist->curr;
    trans_ulist->array[curr].opcode = opcode;
    trans_ulist->array[curr].b_state_offset = b_state_offset;
    trans_ulist->array[curr].ube_ctr_offset = ue_ctr_offset;
    trans_ulist->array[curr].hash_type = BCMPTM_RM_HASH_TYPE_SHARED;
    trans_ulist->array[curr].e_node.se_node = *se_node;

    trans_ulist->curr++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_undo_list_restore(int unit,
                                       rm_hash_trans_undo_list_t *trans_ulist)
{
    int idx = 0;
    rm_hash_trans_undo_instr_t *trans_uinstr = NULL;
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_shr_ent_node_t  *se_node = NULL;
    rm_hash_dual_ent_node_t  *de_node = NULL;
    uint32_t *ue_ctr = NULL;
    bcmdrd_sid_t sid;
    rm_hash_pt_reg_info_t *reg_info = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = trans_ulist->curr - 1; idx >= 0; idx--) {
        trans_uinstr = &trans_ulist->array[idx];

        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_used_bent_ctr_get(unit,
                                              trans_uinstr->ube_ctr_offset,
                                              &ue_ctr));
        if (trans_uinstr->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
            sid = trans_uinstr->e_node.de_node.sid;
        } else {
            sid = trans_uinstr->e_node.se_node.sid;
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_reg_info_get(unit, sid, &reg_info));
        b_state = reg_info->pt_info->b_state + trans_uinstr->b_state_offset;
        if (trans_uinstr->opcode == BCMPTM_RM_HASH_UNDO_ENT_INSERT) {
            if (trans_uinstr->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_dual_ent_node_allocate(unit,
                                                           reg_info->pt_info,
                                                           b_state,
                                                           &de_node));
                *de_node = trans_uinstr->e_node.de_node;
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_dual_ent_node_insert(unit,
                                                         reg_info->pt_info,
                                                         b_state,
                                                         de_node));
                *ue_ctr += e_size_map[de_node->e_loc.be_bmp].size;
            } else {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_shr_ent_node_allocate(unit,
                                                          reg_info->pt_info,
                                                          b_state,
                                                          &se_node));
                *se_node = trans_uinstr->e_node.se_node;
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_shr_ent_node_insert(unit,
                                                        reg_info->pt_info,
                                                        b_state,
                                                        se_node));
                *ue_ctr += e_size_map[se_node->e_loc.be_bmp].size;
            }
        } else if (trans_uinstr->opcode == BCMPTM_RM_HASH_UNDO_ENT_DELETE) {
            if (trans_uinstr->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_dual_ent_node_delete(unit,
                                                         reg_info->pt_info,
                                                         b_state,
                                                         trans_uinstr->e_node.de_node.e_loc.bb_bmp,
                                                         trans_uinstr->e_node.de_node.e_loc.be_bmp,
                                                         &de_node));
                *ue_ctr -= e_size_map[de_node->e_loc.be_bmp].size;
            } else {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_shr_ent_node_delete(unit,
                                                        reg_info->pt_info,
                                                        b_state,
                                                        trans_uinstr->e_node.se_node.e_loc.bb_bmp,
                                                        trans_uinstr->e_node.se_node.e_loc.be_bmp,
                                                        &se_node));
                *ue_ctr -= e_size_map[se_node->e_loc.be_bmp].size;
            }
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
        trans_ulist->curr--;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_ent_req_cmd(int unit,
                                 uint32_t cseq_id)
{
    rm_hash_trans_record_t *trans_rec = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_trans_record_get(unit, &trans_rec));
    switch (trans_rec->trans_state) {
    case BCMPTM_RM_HASH_STATE_IDLE:
        trans_rec->trans_state = BCMPTM_RM_HASH_STATE_UC_A;
        trans_rec->cseq_id = cseq_id;
        break;
    case BCMPTM_RM_HASH_STATE_UC_A:
        if (trans_rec->cseq_id != cseq_id) {
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase)
{
    SHR_FUNC_ENTER(unit);

    if (warm) {
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Warmboot: not yet supported !!\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    switch (phase) {
    case BCMPTM_SUB_PHASE_1:
        break;
    case BCMPTM_SUB_PHASE_2:
        SHR_IF_ERR_EXIT
            (rm_hash_state_elems_alloc(unit, warm));
        SHR_IF_ERR_EXIT
            (rm_hash_state_elems_config(unit, warm));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_init(unit));
        break;
    case BCMPTM_SUB_PHASE_3:
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_reg(int unit,
                      bcmltd_sid_t ltid,
                      bcmptm_sub_phase_t phase,
                      uint32_t *lt_state_offset)
{
    SHR_FUNC_ENTER(unit);

    switch (phase) {
    case BCMPTM_SUB_PHASE_1:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_reg_info_add(unit, ltid));
        break;
    case BCMPTM_SUB_PHASE_2:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_add(unit, ltid));
        break;
    case BCMPTM_SUB_PHASE_3:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_dump(unit, ltid));
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_state_elems_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(rm_hash_state_elems_free(unit));

exit:
    SHR_FUNC_EXIT();
}

