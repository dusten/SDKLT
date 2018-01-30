/*! \file rm_hash_utils.c
 *
 * Util functions for hash resource manager
 *
 * This file contains utility functions for hash resource manager
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
/*
 *! \brief Hardware logical bucket to software logical bucket mapping
 *
 * This data structure describes the information of how a hardware logical
 * bucket maps to the software logical bucket.
 */
typedef struct rm_hash_hlb_slb_map_s {
    /*! \brief Divisior to divide the hlb */
    uint8_t divisor;

    /*! \brief Multiplier to the remainder */
    uint8_t shft_multip;

    /*! \brief Valid base bucket mask */
    uint8_t vbb_mask;
} rm_hash_hlb_slb_map_t;

/*
 *! \brief Base bucket bitmap to base bucket offset mapping
 *
 * This data structure describes the base bucket offset within a software
 * logical bucket.
 */
typedef struct rm_hash_base_bkt_offset_s {
    /*! \brief Valid or not */
    bool valid;

    /*! \brief offset of base bucket within the software logical bucket */
    uint8_t offset;
} rm_hash_base_bkt_offset_t;

/*
 *! \brief Base bucket bitmap to base bucket offset mapping
 *
 * This data structure describes the base bucket offset within a software
 * logical bucket.
 */
typedef struct rm_hash_base_ent_offset_s {
    /*! \brief Valid or not */
    bool valid;

    /*! \brief offset of base entry within the base bucket */
    uint8_t offset;
} rm_hash_base_ent_offset_t;

/*******************************************************************************
 * Private variables
 */
static rm_hash_hlb_slb_map_t bmn_ent_bmn_bkt_map = {0x01, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm0_ent_bm0_bkt_map = {0x01, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm0_ent_bm1_bkt_map = {0x02, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm0_ent_bm2_bkt_map = {0x04, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm1_ent_bm1_bkt_map = {0x01, 0x02, 0x0003};

static rm_hash_hlb_slb_map_t bm1_ent_bm2_bkt_map = {0x02, 0x02, 0x0003};

static rm_hash_hlb_slb_map_t bm2_ent_bm2_bkt_map = {0x01, 0x04, 0x000F};

static rm_hash_hlb_slb_map_t *hlb_to_slb_map_info[RM_HASH_BM_CNT][RM_HASH_BM_CNT] = {
{&bmn_ent_bmn_bkt_map, NULL, NULL, NULL},
{NULL, &bm0_ent_bm0_bkt_map, NULL, NULL},
{NULL, &bm0_ent_bm1_bkt_map, &bm1_ent_bm1_bkt_map, NULL},
{NULL, &bm0_ent_bm2_bkt_map, &bm1_ent_bm2_bkt_map, &bm2_ent_bm2_bkt_map}
};

static rm_hash_base_bkt_offset_t bb_bmp_to_offset_map[] = {
{FALSE, 0}, /* 0000 */
{TRUE,  0}, /* 0001 */
{TRUE,  1}, /* 0010 */
{FALSE, 0}, /* 0011 */
{TRUE,  2}, /* 0100 */
{FALSE, 0}, /* 0101 */
{FALSE, 0}, /* 0110 */
{FALSE, 0}, /* 0111 */
{TRUE,  3}, /* 1000 */
};

static rm_hash_base_ent_offset_t be_bmp_to_offset_map[] = {
{FALSE,  0}, /* 0000 */
{TRUE,   0}, /* 0001 */
{TRUE,   1}, /* 0010 */
{TRUE,   0}, /* 0011 */
{TRUE,   2}, /* 0100 */
{FALSE,  0}, /* 0101 */
{FALSE,  0}, /* 0110 */
{FALSE,  0}, /* 0111 */
{TRUE,   3}, /* 1000 */
{FALSE,  0}, /* 1001 */
{FALSE,  0}, /* 1010 */
{FALSE,  0}, /* 1011 */
{TRUE,   2}, /* 1100 */
{FALSE,  0}, /* 1101 */
{FALSE,  0}, /* 1110 */
{TRUE,   0}, /* 1111 */
};

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_valid_hw_entry_info_get(int unit,
                                       const bcmptm_rm_hash_lt_info_t *lt_info,
                                       uint8_t *cnt,
                                       uint8_t *idx_list)
{
    uint8_t idx, pos = 0, val = 1;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);
    SHR_NULL_CHECK(cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(idx_list, SHR_E_PARAM);

    for (i = 0; i < lt_info->rm_more_info_count; i++) {
        val = 1;
        for (idx = 0; idx < lt_info->hw_entry_info_count; idx++) {
            if (val & lt_info->rm_more_info[i].hw_entry_valid_bitmap) {
                idx_list[pos] = idx;
                pos++;
            }
            val <<= 1;
        }
    }

    *cnt = pos;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_sid_get(int unit,
                       const bcmptm_rm_hash_more_info_t *rm_more_info,
                       const bcmptm_rm_hash_hw_entry_info_t *entry_info,
                       uint8_t pbank,
                       bcmdrd_sid_t *sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rm_more_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_info, SHR_E_PARAM);
    SHR_NULL_CHECK(sid, SHR_E_PARAM);

    if (pbank >= rm_more_info->num_banks) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (rm_more_info->num_bank_sid == 1) {
        /*
         * For some devices, there is only one SID for all
         * the banks.
         */
        *sid = entry_info->sid[0];
    } else {
        /*
         * For some devices, it will be assumed that each bank has
         * a separate SID.
         */
        *sid = entry_info->sid[pbank];
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_key_type_fill(int unit,
                             uint16_t key_type,
                             const bcmptm_rm_hash_hw_entry_info_t *entry_info,
                             uint32_t *entry)
{
    uint8_t idx;
    uint32_t field[BCMPTM_MAX_PT_FIELD_WORDS] = {0};
    const bcmlrd_hw_rfield_info_t *ktype_info = entry_info->hw_ktype_field_info;

    SHR_FUNC_ENTER(unit);

    field[0] = key_type;
    /*
     * There may be one or multiple KEY_TYPE fields in entry buffer. PTRM
     * must retrieve the corresponding KEY_TYPE value from LRD and fill
     * KEY_TYPE fields in the buffer.
     */
    for (idx = 0; idx < ktype_info->num_reps; idx++) {
        int sbit, ebit;
        sbit = ktype_info->field_start_bit[idx];
        ebit = sbit + ktype_info->field_width - 1;
        bcmdrd_field_set(entry, sbit, ebit, field);
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_sig_generate(int unit,
                                const bcmptm_rm_hash_more_info_t *more_info,
                                uint32_t *entry,
                                uint32_t *ent_sig)
{
    int sbit, ebit;
    uint32_t key_val[BCMPTM_MAX_PT_FIELD_WORDS] = {0};
    const bcmlrd_hw_field_list_t *key_fields = NULL;

    SHR_FUNC_ENTER(unit);

    key_fields = more_info->hw_key_fields;
    sbit = key_fields->field_start_bit[0];
    ebit = key_fields->field_start_bit[0] + key_fields->field_width[0] - 1;
    bcmdrd_field_get(entry, sbit, ebit, key_val);
    *ent_sig = key_val[0];

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_more_info_get(int unit,
                                 bcmptm_rm_hash_req_t *req_info,
                                 rm_hash_lt_ctrl_t *lt_ctrl,
                                 const bcmptm_rm_hash_more_info_t **more_info)
{
    bcmptm_rm_hash_entry_attrs_t *e_attrs;
    bcmptm_rm_hash_lt_info_t *lt_info = NULL;
    uint8_t idx;

    SHR_FUNC_ENTER(unit);

    e_attrs = (bcmptm_rm_hash_entry_attrs_t *)(req_info->entry_attrs);
    lt_info = &lt_ctrl->lt_info;

    if (e_attrs != NULL) {
        for (idx = 0; idx < lt_info->rm_more_info_count; idx++) {
            if (e_attrs->entry_mode == lt_info->rm_more_info[idx].entry_mode) {
                *more_info = &lt_info->rm_more_info[idx];
                break;
            }
        }
        if (idx == (lt_info->rm_more_info_count)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    } else {
        *more_info = lt_info->rm_more_info;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_given_key_ent_info_get(int unit,
                                      const bcmptm_rm_hash_lt_info_t *lt_info,
                                      const bcmptm_rm_hash_more_info_t *rm_more_info,
                                      uint8_t *cnt,
                                      uint8_t *idx_list)
{
    uint8_t idx, pos = 0, val = 0x01;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rm_more_info, SHR_E_PARAM);
    SHR_NULL_CHECK(cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(idx_list, SHR_E_PARAM);

    for (idx = 0; idx < lt_info->hw_entry_info_count; idx++) {
        if (val & rm_more_info->hw_entry_valid_bitmap) {
            idx_list[pos] = idx;
            pos++;
        }
        val <<= 1;
    }

    *cnt = pos;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_hei_get(int unit,
                       bcmptm_rm_hash_req_t *req_info,
                       bcmltd_sid_t ltid,
                       const bcmptm_rm_hash_lt_info_t *lt_info,
                       uint8_t *cnt,
                       uint8_t *idx_list)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    const bcmptm_rm_hash_more_info_t *more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_more_info_get(unit,
                                          req_info,
                                          lt_ctrl,
                                          &more_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_given_key_ent_info_get(unit,
                                               lt_info,
                                               more_info,
                                               cnt,
                                               idx_list));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_max_bm_get(int unit, uint8_t bmf, rm_hash_bm_t *max_bm)
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

int
bcmptm_rm_hash_hlb_to_slb_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              rm_hash_bm_t e_bm,
                              uint32_t *hlb_list,
                              rm_hash_ent_slb_info_t *slb_info_list)
{
    rm_hash_bm_t t_bm = RM_HASH_BM_N;
    uint8_t lbank = 0, vbb_bmp = 0;
    rm_hash_hlb_slb_map_t *map = NULL;
    uint32_t hlb = 0, slb_offset = 0;

    SHR_FUNC_ENTER(unit);

    /* Get the maximum bucket mode the physical hash table will support */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_max_bm_get(unit, pt_info->bmf, &t_bm));

    map = hlb_to_slb_map_info[t_bm][e_bm];
    if (map == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    for (lbank = 0; lbank < pt_info->num_lbanks; lbank++) {
        hlb = hlb_list[lbank];
        slb_offset = hlb / map->divisor;
        vbb_bmp = map->vbb_mask << (map->shft_multip * (hlb % map->divisor));
        slb_info_list[lbank].slb = pt_info->lbank_base[lbank] + slb_offset;
        slb_info_list[lbank].vbb_bmp = vbb_bmp;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                              "software logical bucket on bank[%d]:[%d], "
                               "valid base bucket bitmap:[%d]\n"),
                               lbank,
                               slb_info_list[lbank].slb,
                               slb_info_list[lbank].vbb_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_slb_to_lbank_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint32_t slb,
                                uint8_t *lbank)
{
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_lbanks; idx++) {
        if (slb < pt_info->lbank_size[idx]) {
            *lbank = idx;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else {
            slb -= pt_info->lbank_size[idx];
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_index_get(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint32_t slb,
                             rm_hash_ent_loc_t *e_loc,
                             uint8_t e_size,
                             uint32_t *e_index)
{
    uint32_t be_idx;
    uint32_t lbank_size, pbkt;
    uint8_t lbank, bb_size, bb_bmp, bb_offset, be_bmp, be_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_lbank_get(unit, pt_info, slb, &lbank));

    switch (pt_info->hash_type) {
    case BCMPTM_RM_HASH_TYPE_SHARED:
        bb_bmp = e_loc->bb_bmp;
        if ((bb_bmp >= COUNTOF(bb_bmp_to_offset_map)) ||
            (bb_bmp_to_offset_map[bb_bmp].valid == FALSE)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        bb_offset = bb_bmp_to_offset_map[bb_bmp].offset;
        be_bmp = e_loc->be_bmp;
        if ((be_bmp >= COUNTOF(be_bmp_to_offset_map)) ||
            (be_bmp_to_offset_map[be_bmp].valid == FALSE)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        be_offset = be_bmp_to_offset_map[be_bmp].offset;
        /*
         * For all the physical non-dual hash tables so far, the size of a base
         * bucket (number of base entries) are always 4.
         */
        bb_size = 4;
        be_idx = pt_info->slb_size * slb;
        be_idx = be_idx + bb_offset * bb_size + be_offset;
        *e_index = be_idx / e_size;
        break;
    case BCMPTM_RM_HASH_TYPE_DUAL:
        /*
         * It is assumed that dual hash table has identical bank
         * size for its two banks. And for dual hash tables, only legacy BM0 mode
         * is supported.
         */
        bb_size = 8;
        lbank_size = pt_info->lbank_size[0];
        pbkt = slb % lbank_size;
        be_bmp = e_loc->be_bmp;
        if ((be_bmp >= COUNTOF(be_bmp_to_offset_map)) ||
            (be_bmp_to_offset_map[be_bmp].valid == FALSE)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        be_offset = be_bmp_to_offset_map[be_bmp].offset;
        *e_index = pbkt * bb_size + lbank * (bb_size / 2) + be_offset;
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_bkt_mode_get(int unit,
                                const bcmptm_rm_hash_more_info_t *more_info,
                                rm_hash_pt_info_t *pt_info,
                                rm_hash_bm_t *e_bm)
{
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    if (more_info->num_bank_sid > 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /*
     * Bucket mode is defined on per key type basis, if there is no explicit
     * key type definition for a hash table, the default index 0 will be used.
     */
    if (more_info->key_type_val != NULL) {
        idx = more_info->key_type_val[0];
    }
    if ((idx >= RM_HASH_MAX_KEY_TYPE_COUNT) ||
        (pt_info->key_attrib[idx].valid == FALSE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    *e_bm = pt_info->key_attrib[idx].bm;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ovly_sid_get(int unit,
                            bcmdrd_sid_t sid,
                            bcmdrd_sid_t *ovly_sid)
{
    const bcmptm_overlay_info_t *ovly_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_pt_overlay_info_get(unit, sid, &ovly_info));
    if ((ovly_info->mode != BCMPTM_OINFO_MODE_NONE)) {
        SHR_IF_ERR_EXIT
            (ovly_info->mode != BCMPTM_OINFO_MODE_HASH ?
                                SHR_E_INTERNAL : SHR_E_NONE);
        *ovly_sid = ovly_info->w_sid;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_group_sid_get(int unit,
                             const bcmptm_rm_hash_lt_info_t *lt_info,
                             const bcmptm_rm_hash_more_info_t *more_info,
                             bcmdrd_sid_t *grp_sid)
{
    uint8_t hei_cnt = 0, hei_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {0};
    const bcmptm_rm_hash_hw_entry_info_t *hei = NULL;
    bcmdrd_sid_t sid, ovly_sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_given_key_ent_info_get(unit,
                                               lt_info,
                                               more_info,
                                               &hei_cnt,
                                               hei_list));
    hei = lt_info->hw_entry_info + hei_list[0];
    sid = hei->sid[0];
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ovly_sid_get(unit, sid, &ovly_sid));

    if (ovly_sid != INVALIDm) {
        *grp_sid = ovly_sid;
    } else {
        *grp_sid = sid;
    }

exit:
    SHR_FUNC_EXIT();
}
