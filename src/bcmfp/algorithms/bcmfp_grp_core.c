/*! \file bcmfp_grp_core.c
 *
 * Group Create and Delete APIs.
 *
 * Core Group create and Delete APIs common to all architecture
 * types(KEY_TYPE_PRESEL/KEY_TYPE_SELCODE/KEY_TYPE_FIXED).
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_keygen_api.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include "bcmfp_keygen_md.h"
#include "bcmfp_grp_selcode.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

STATIC int
bcmfp_grp_keygen_qual_info_alloc(int unit,
                              bcmfp_group_t *grp_cfg,
                              bcmfp_keygen_cfg_t *keygen_cfg)
{
    int qual_id_count = 0;
    uint16_t idx = 0;
    size_t array_size = 0;
    bcmfp_qual_t qual_id;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    SHR_BITCOUNT_RANGE(grp_cfg->qset.w,
                       qual_id_count,
                       0, BCMFP_QUAL_COUNT);
    SHR_IF_ERR_EXIT((qual_id_count == 0) ? SHR_E_CONFIG : SHR_E_NONE);

    array_size = qual_id_count * sizeof(bcmfp_keygen_qual_info_t);
    BCMFP_ALLOC(keygen_cfg->qual_info_arr,
                array_size, "Array of qual_ids in group");
    keygen_cfg->qual_info_count = qual_id_count;
    idx = 0;
    SHR_BIT_ITER(grp_cfg->qset.w, BCMFP_QUAL_COUNT, qual_id) {
        if (keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id] == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        keygen_cfg->qual_info_arr[idx].qual_id = qual_id;
        qual_bitmap = grp_cfg->qual_bitmap;
        while (qual_bitmap != NULL) {
            if (qual_bitmap->qual == qual_id) {
                sal_memcpy(keygen_cfg->qual_info_arr[idx].bitmap,
                         qual_bitmap->bitmap,
                         sizeof(uint32_t) * BCMFP_KEYGEN_QUAL_BITMAP_MAX_WORDS);
                keygen_cfg->qual_info_arr[idx].partial = TRUE;
                break;
            }
            qual_bitmap = qual_bitmap->next;
        }
        idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_grp_oper_get(int unit,
                   bcmfp_keygen_oper_t *keygen_oper,
                   bcmfp_group_t *grp_cfg)
{
    uint8_t part = 0;
    uint8_t gran = 0;
    uint8_t level = 0;
    uint8_t ext_num = 0;
    uint8_t ctrl_sel_val = 0;
    uint8_t parts_count = 0;
    uint16_t ext_code_idx = 0;
    uint16_t qual_idx = 0;
    uint16_t offset = 0;
    uint16_t num_offsets = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_ext_sel_t *grp_ext_code = NULL;
    bcmfp_group_qual_offset_info_t *group_qual = NULL;
    bcmfp_keygen_qual_offset_info_t *qual_offset_info = NULL;
    bcmfp_keygen_ext_ctrl_sel_info_t *keygen_ext_code = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_oper, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit, grp_cfg->flags, &parts_count));

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, grp_cfg->stage_id, &stage));

    for (part = 0; part < parts_count; part++) {
        group_qual = &(grp_cfg->qual_arr[0][part]);
        qual_offset_info = &(keygen_oper->qual_offset_info[part]);
        group_qual->size = qual_offset_info->size;
        BCMFP_ALLOC(group_qual->qid_arr,
                    qual_offset_info->size * sizeof(bcmfp_qual_t),
                    "Group Qualifer Information");
        BCMFP_ALLOC(group_qual->offset_arr,
                    qual_offset_info->size * sizeof(bcmfp_qual_offset_info_t),
                    "Group Qualifer Offset Information");
        for (qual_idx = 0; qual_idx < qual_offset_info->size; qual_idx++) {
            group_qual->qid_arr[qual_idx] =
                qual_offset_info->qid_arr[qual_idx];
            num_offsets = 0;
            for (offset = 0; offset < BCMFP_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
                 if (!qual_offset_info->offset_arr[qual_idx].width[offset]) {
                     continue;
                 }

                 group_qual->offset_arr[qual_idx].offset[offset] =
                     qual_offset_info->offset_arr[qual_idx].offset[offset];
                 group_qual->offset_arr[qual_idx].width[offset] =
                     qual_offset_info->offset_arr[qual_idx].width[offset];
                 if (grp_cfg->group_mode == BCMFP_GROUP_MODE_SINGLE &&
                     !(stage->flags & BCMFP_STAGE_MODE_HALF_NOT_SUPPORTED)) {
                     group_qual->offset_arr[qual_idx].offset[offset] -= 80;
                 }
                 num_offsets++;
                 if (num_offsets ==
                     qual_offset_info->offset_arr[qual_idx].num_offsets) {
                     break;
                 }
            }
            group_qual->offset_arr[qual_idx].num_offsets = (offset + 1);
        }
    }

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        bcmfp_ext_codes_init(unit, &(grp_cfg->ext_codes[part]));
    }

    for (ext_code_idx = 0;
        ext_code_idx < keygen_oper->ext_ctrl_sel_info_count;
        ext_code_idx++) {
        keygen_ext_code = &(keygen_oper->ext_ctrl_sel_info[ext_code_idx]);
        part = keygen_ext_code->part;
        gran = keygen_ext_code->gran;
        level = keygen_ext_code->level;
        ext_num = keygen_ext_code->ext_num;
        ctrl_sel_val = keygen_ext_code->ctrl_sel_val;
        grp_ext_code = &(grp_cfg->ext_codes[part]);
        if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER) {
            if (level == 1) {
                if (gran == 32) {
                    grp_ext_code->l1_e32_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 16) {
                    grp_ext_code->l1_e16_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 8) {
                    grp_ext_code->l1_e8_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 4) {
                    grp_ext_code->l1_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l1_e2_sel[ext_num] = ctrl_sel_val;
                } else {
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
            } else if (level == 2) {
                if (gran == 16) {
                    grp_ext_code->l2_e16_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 4) {
                    grp_ext_code->l2_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l2_e2_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 1) {
                    grp_ext_code->l2_e1_sel[ext_num] = ctrl_sel_val;
                } else {
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
            } else if (level == 3) {
                if (gran == 4) {
                    grp_ext_code->l3_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l3_e2_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 1) {
                    grp_ext_code->l3_e1_sel[ext_num] = ctrl_sel_val;
                } else {
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A) {
            grp_ext_code->aux_tag_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B) {
            grp_ext_code->aux_tag_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C) {
            grp_ext_code->aux_tag_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D) {
            grp_ext_code->aux_tag_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN) {
            grp_ext_code->tcp_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN) {
            grp_ext_code->tos_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN) {
            grp_ext_code->ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_A) {
            grp_ext_code->class_id_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_B) {
            grp_ext_code->class_id_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_C) {
            grp_ext_code->class_id_cont_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_D) {
            grp_ext_code->class_id_cont_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A) {
            grp_ext_code->src_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B) {
            grp_ext_code->src_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0) {
            grp_ext_code->src_dest_cont_0_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1) {
            grp_ext_code->src_dest_cont_1_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            grp_ext_code->ipbm_present = 1;
            grp_ext_code->ipbm_source = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM) {
            grp_ext_code->normalize_l3_l4_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM) {
            grp_ext_code->normalize_mac_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_UDF) {
            grp_ext_code->udf_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_PMUX) {
            grp_ext_code->pmux_sel[ctrl_sel_val] = 1;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN) {
            grp_ext_code->alt_ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (part = 0; part < parts_count; part++) {
            group_qual = &(grp_cfg->qual_arr[0][part]);
            if (group_qual->qid_arr != NULL) {
                sal_free(group_qual->qid_arr);
            }
            if (group_qual->offset_arr != NULL) {
                sal_free(group_qual->offset_arr);
            }
        }
    }
    (void)bcmfp_keygen_oper_free(unit, parts_count, keygen_oper);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *   bcmfp_em_group_validate_view
 * Purpose:
 *   Validate Exact Match View.
 * Parameters:
 *   unit          - (IN) BCM device number.
 *   stage         - (IN) stage
 *   group_mode    - (IN) group mode
 *   qset_key_size - (IN) Qualifier Key Size.
 *   aset_key_size - (IN) Actions Key Size.
 * Returns:
 *   SHR_E_XXX
 */
STATIC int
bcmfp_em_group_validate_view(int unit,
                             bcmfp_stage_t *stage,
                             bcmfp_group_mode_t group_mode,
                             uint16_t qset_key_size,
                             uint16_t aset_key_size)
{
    shr_error_t rv = SHR_E_NONE;
    bcmfp_qset_aset_size_info_t *key_data_sz = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input Parameter Check. */
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(stage->key_data_size_info, SHR_E_PARAM);

    key_data_sz = stage->key_data_size_info;
    /* Determine view of group
     * based on key and action size
     * View A(210b) = 160b key + 42b policy + 8b overhead
     *                policy (12b classid + 5b action profile id +
     *                        7b qos profile id + 18 bit action data
     * View B(210b) = 128b key + 74b policy + 8b overhead
     *                policy (12b classid + 5b action profile id +
     *                        7b qos profile id + 50 bit action data
     * View C(420b) = 320b key + 84b policy + 16b overhead
     *                policy (12b classid + 5b action profile id +
     *                        7b qos profile id + 60 bit action data
     */
    if (qset_key_size <= key_data_sz->key_size[group_mode] &&
        aset_key_size <= key_data_sz->data_size[group_mode]) {
            rv = SHR_E_NONE;
    } else {
        rv = SHR_E_CONFIG;
    }
    SHR_RETURN_VAL_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmfp_group_calc_qset_keysize
 * Purpose:
 *     Calcualte the real key size of the user supplied QSET based on width of
 *     the qualifiers.
 * Parameters:
 *     unit         - (IN)     BCM unit number.
 *     keygen_cfg   - (IN)     Keygen configuration
 *     rkey_size    - (IN/OUT) Real key size of the supplied QSET.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmfp_group_calc_qset_keysize(int unit,
                              bcmfp_keygen_cfg_t *keygen_cfg,
                              uint16_t *rkey_size)
{
    uint16_t idx = 0;
    uint16_t bit = 0;
    uint16_t qual_size = 0;
    uint16_t qual_cfg_max_size = 0;
    bcmfp_qual_t qual_id;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info1 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    qual_cfg_info1 = keygen_cfg->qual_cfg_info_db->qual_cfg_info;

    SHR_IF_ERR_EXIT((qual_cfg_info1 == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {

        qual_id =  keygen_cfg->qual_info_arr[idx].qual_id;

        if (qual_cfg_info1[qual_id] == NULL) {
            continue;
        }
        /*Get the max qualifier size */
        SHR_IF_ERR_EXIT(
                bcmfp_keygen_qual_cfg_max_size_get(unit,
                                        keygen_cfg,
                                        qual_id,
                                        &qual_cfg_max_size));

        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            /* calculate the bitmap size and use it */
            qual_size = 0;
            for (bit = 0; bit < qual_cfg_max_size; bit++) {
                if (SHR_BITGET(keygen_cfg->qual_info_arr[idx].bitmap, bit)) {
                    qual_size++;
                }
            }
        } else {
            qual_size = qual_cfg_max_size;
        }

        (*rkey_size) += qual_size;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_create(int unit, uint32_t trans_id,
                   bcmfp_group_t *fg)
{
    int group_qual_id_count = 0;
    bool reset_flag = TRUE;
    uint8_t part = 0;
    uint8_t action_res_id = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_mode_t group_mode;
    bcmfp_group_mode_t original_group_mode = 0;
    bcmfp_keygen_cfg_t keygen_cfg;
    bcmfp_keygen_oper_t keygen_oper;
    bcmfp_keygen_mode_t keygen_mode;
    bcmfp_keygen_mode_t keygen_ext_mode;
    uint16_t qset_size = 0;
    uint16_t aset_size = 0;


    SHR_FUNC_ENTER(unit);

    sal_memset(&keygen_cfg, 0, sizeof(bcmfp_keygen_cfg_t));
    sal_memset(&keygen_oper, 0, sizeof(bcmfp_keygen_oper_t));

    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    SHR_BITCOUNT_RANGE(fg->qset.w,
                       group_qual_id_count,
                       0, BCMFP_QUAL_COUNT);

    if (group_qual_id_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, fg->stage_id, &stage));

    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
        keygen_cfg.qual_cfg_info_db = stage->kgn_qual_cfg_info_db;
        SHR_IF_ERR_EXIT(
            bcmfp_grp_keygen_qual_info_alloc(unit, fg, &keygen_cfg));
        if (keygen_cfg.qual_cfg_info_db == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        if (stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {
            SHR_IF_ERR_EXIT(bcmfp_group_calc_qset_keysize(unit,
                                      &keygen_cfg,
                                      &qset_size));

            

            aset_size = 17;
        }
    }

    if (stage->flags & BCMFP_STAGE_KEY_TYPE_FIXED) {
        reset_flag = FALSE;
        SHR_IF_ERR_EXIT(bcmfp_group_slice_mode_to_group_mode(unit,
                                                   fg->group_slice_mode,
                                                   &fg->group_mode));

        for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
            bcmfp_ext_codes_init(unit, &(fg->ext_codes[part]));
        }

        SHR_IF_ERR_EXIT(bcmfp_group_combined_selcode_get(unit, stage, fg));

        SHR_IF_ERR_EXIT(bcmfp_group_selcode_key_get(unit, stage, fg));
    }

    for (group_mode = 0; group_mode < BCMFP_GROUP_MODE_COUNT; group_mode++) {

        if (group_mode == BCMFP_GROUP_MODE_AUTO) {
            continue;
        }

        if (!SHR_BITGET(stage->group_mode_bmp.w, group_mode)) {
            continue;
        }

        if (fg->group_mode != BCMFP_GROUP_MODE_AUTO &&
            (stage->stage_id != BCMFP_STAGE_ID_EXACT_MATCH &&
             fg->group_mode != group_mode))  {
            continue;
        }

        if (stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH &&
            fg->group_mode != group_mode) {
             if (!((fg->group_mode == BCMFP_GROUP_MODE_SINGLE ||
                    fg->group_mode == BCMFP_GROUP_MODE_AUTO) &&
                   (group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW ||
                    group_mode == BCMFP_GROUP_MODE_DBLINTER ||
                    group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE))) {
                 continue;
             }
        }

        if (group_mode == BCMFP_GROUP_MODE_SINGLE
            || group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW
            || group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
            fg->flags = BCMFP_GROUP_SPAN_SINGLE_SLICE;
        } else if (group_mode == BCMFP_GROUP_MODE_DBLINTRA) {
            fg->flags = (BCMFP_GROUP_INTRASLICE_DOUBLEWIDE |
                         BCMFP_GROUP_SPAN_SINGLE_SLICE);
            if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
                fg->flags |= BCMFP_GROUP_INTRASLICE_SPAN_ONE_PART;
            }
        } else if (group_mode == BCMFP_GROUP_MODE_DBLINTER) {
            fg->flags = BCMFP_GROUP_SPAN_DOUBLE_SLICE;
        } else if (group_mode == BCMFP_GROUP_MODE_TRIPLE) {
            fg->flags = BCMFP_GROUP_SPAN_TRIPLE_SLICE;
        } else if (group_mode == BCMFP_GROUP_MODE_QUAD) {
            fg->flags = (BCMFP_GROUP_SPAN_DOUBLE_SLICE |
                         BCMFP_GROUP_INTRASLICE_DOUBLEWIDE);
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        original_group_mode = fg->group_mode;
        fg->group_mode = group_mode;

        if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
            SHR_IF_ERR_EXIT(
                bcmfp_group_parts_count(unit, fg->flags,
                                        &(keygen_cfg.num_parts)));
            if (fg->group_mode == BCMFP_GROUP_MODE_SINGLE) {
                keygen_mode = BCMFP_KEYGEN_MODE_SINGLE;
                keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
            } else if (fg->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
                keygen_mode = BCMFP_KEYGEN_MODE_SINGLE_ASET_WIDE;
                keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
            } else if (fg->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {
                keygen_mode = BCMFP_KEYGEN_MODE_SINGLE_ASET_NARROW;
                keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
            } else if (fg->group_mode == BCMFP_GROUP_MODE_DBLINTRA) {
                keygen_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
                keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
            } else if (fg->group_mode == BCMFP_GROUP_MODE_DBLINTER) {
                keygen_mode = BCMFP_KEYGEN_MODE_DBLINTER;
                keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTER;
            } else if (fg->group_mode == BCMFP_GROUP_MODE_TRIPLE) {
                keygen_mode = BCMFP_KEYGEN_MODE_TRIPLE;
                keygen_ext_mode = BCMFP_KEYGEN_MODE_TRIPLE;
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            keygen_cfg.mode = keygen_mode;
            keygen_cfg.ext_cfg_db = stage->kgn_ext_cfg_db_arr[keygen_ext_mode];

            if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
                rv = bcmfp_em_group_validate_view(unit,
                                                  stage,
                                                  fg->group_mode,
                                                  qset_size,
                                                  aset_size);
                if (SHR_FAILURE(rv)) {
                    fg->group_mode = original_group_mode;
                    continue;
                }
            }
            rv = bcmfp_keygen_cfg_process(unit, &keygen_cfg, &keygen_oper);
        } else {
            rv = bcmfp_group_selcode_assign(unit,
                                            fg,
                                            stage,
                                            group_qual_id_count,
                                            reset_flag);
        }

        if (rv == SHR_E_CONFIG || rv == SHR_E_RESOURCE) {
            fg->group_mode = original_group_mode;
            continue;
        } else if (SHR_SUCCESS(rv)) {
            fg->group_mode = group_mode;
            break;
        } else {
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    SHR_IF_ERR_EXIT(rv);

    sal_memset(fg->entry_type_bmp.w, 0, sizeof(fg->entry_type_bmp.w));

    SHR_BITSET(fg->entry_type_bmp.w, BCMFP_ENTRY_TYPE_ALL);

    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
        /* update group default presel flag if group has 0 valid presel id's */
        if (fg->num_presel_ids == 0) {
            fg->flags |= BCMFP_GROUP_WITH_DEFAULT_PRESEL;
        }

        SHR_IF_ERR_EXIT(bcmfp_grp_oper_get(unit, &keygen_oper, fg));

        SHR_IF_ERR_EXIT(
            bcmfp_group_action_res_id_allocate(unit,
                                               fg->pipe_id,
                                               fg->stage_id,
                                               &action_res_id));
        fg->action_res_id = action_res_id;

        for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
            fg->ext_codes[part].ltid = action_res_id;
        }

        if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
            /* check if there exists a group with same lookup id
             * and default presel entry. 2 groups with same lookup_id
             * and having default presel entry is not valid
             */
            SHR_IF_ERR_EXIT(
               bcmfp_group_presel_entry_validate(unit,
                                                 fg->pipe_id,
                                                 fg->stage_id,
                                                 fg->lookup_id,
                                                 fg->num_presel_ids));
        }

        SHR_IF_ERR_EXIT(bcmfp_group_install(unit, trans_id, stage, fg));
        if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
            SHR_IF_ERR_EXIT(bcmfp_group_action_profile_install(unit,
                                                               trans_id,
                                                               stage, fg));
        }
    }

    SHR_IF_ERR_EXIT(bcmfp_group_oper_info_set(unit, fg));

exit:
    if (SHR_FAILURE(rv)) {
        fg->group_mode = original_group_mode;
    }
    /*coverity fix. Resetting the value as it is no more used */
    original_group_mode = 0;
    SHR_FREE(keygen_cfg.qual_info_arr);
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_group_ext_ctrls_count_get(int unit,
                                bcmfp_group_t *grp_cfg,
                                uint16_t *ext_ctrl_count)
{
    uint8_t part_id = 0;
    uint8_t num_parts = 0;
    uint16_t idx = 0;
    uint16_t count = 0;
    bcmfp_ext_sel_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_ctrl_count, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_group_parts_count(unit, grp_cfg->flags, &num_parts));

    for (part_id = 0; part_id < num_parts; part_id++) {
        ext_codes = NULL;
        SHR_IF_ERR_EXIT
            (bcmfp_group_ext_info_get(unit, grp_cfg->stage_id,
                      grp_cfg->group_id, part_id, &ext_codes));
        for (idx = 0; idx < BCMFP_EXT_PMUX_SELECTORS_MAX; idx++) {
            if (ext_codes->pmux_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L1_E32_SELECTORS_MAX; idx++) {
            if (ext_codes->l1_e32_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L1_E16_SELECTORS_MAX; idx++) {
            if (ext_codes->l1_e16_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L1_E8_SELECTORS_MAX; idx++) {
            if (ext_codes->l1_e8_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L1_E4_SELECTORS_MAX; idx++) {
            if (ext_codes->l1_e4_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L1_E2_SELECTORS_MAX; idx++) {
            if (ext_codes->l1_e2_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L2_E16_SELECTORS_MAX; idx++) {
            if (ext_codes->l2_e16_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L3_E1_SELECTORS_MAX; idx++) {
            if (ext_codes->l3_e1_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L3_E2_SELECTORS_MAX; idx++) {
            if (ext_codes->l3_e2_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        for (idx = 0; idx < BCMFP_EXT_L3_E4_SELECTORS_MAX; idx++) {
            if (ext_codes->l3_e4_sel[idx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            }
        }
        if (ext_codes->aux_tag_a_sel != -1) {
            count++;
        }
        if (ext_codes->aux_tag_b_sel != -1) {
            count++;
        }
        if (ext_codes->aux_tag_c_sel != -1) {
            count++;
        }
        if (ext_codes->aux_tag_d_sel != -1) {
            count++;
        }
        if (ext_codes->tcp_fn_sel != -1) {
            count++;
        }
        if (ext_codes->ttl_fn_sel != -1) {
            count++;
        }
        if (ext_codes->tos_fn_sel != -1) {
            count++;
        }
        if (ext_codes->class_id_cont_a_sel != -1) {
            count++;
        }
        if (ext_codes->class_id_cont_b_sel != -1) {
            count++;
        }
        if (ext_codes->class_id_cont_c_sel != -1) {
            count++;
        }
        if (ext_codes->class_id_cont_d_sel != -1) {
            count++;
        }
        if (ext_codes->src_cont_a_sel != -1) {
            count++;
        }
        if (ext_codes->src_cont_b_sel != -1) {
            count++;
        }
        if (ext_codes->src_dest_cont_0_sel != -1) {
            count++;
        }
        if (ext_codes->src_dest_cont_1_sel != -1) {
            count++;
        }
        if (ext_codes->ipbm_present != -1) {
            count++;
        }
        if (ext_codes->normalize_l3_l4_sel != -1) {
            count++;
        }
        if (ext_codes->normalize_mac_sel != -1) {
            count++;
        }
    }

    *ext_ctrl_count = count;

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_group_ext_ctrls_init(int unit,
                            bcmfp_group_t *grp_cfg,
                            bcmfp_keygen_cfg_t *keygen_cfg)
{
    uint8_t part_id = 0;
    uint16_t idx = 0;
    uint16_t eidx = 0;
    uint16_t ext_ctrls_count = 0;
    bcmfp_ext_sel_t *ext_codes = NULL;
    bcmfp_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (part_id = 0; part_id < BCMFP_ENTRY_PARTS_MAX; part_id++) {
         bcmfp_ext_codes_init(unit, &(grp_cfg->ext_codes[part_id]));
    }
    SHR_IF_ERR_EXIT
        (bcmfp_group_ext_ctrls_count_get(unit,
                   grp_cfg, &ext_ctrls_count));
    if (ext_ctrls_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    BCMFP_ALLOC(ext_ctrl_info,
                ext_ctrls_count * sizeof(bcmfp_keygen_ext_ctrl_sel_info_t),
                "Extractor control information of existing group");
    keygen_cfg->ext_ctrl_sel_info = ext_ctrl_info;
    keygen_cfg->ext_ctrl_sel_info_count = ext_ctrls_count;
    for (part_id = 0; part_id < keygen_cfg->num_parts; part_id++) {
        ext_codes = NULL;
        SHR_IF_ERR_EXIT
            (bcmfp_group_ext_info_get(unit, grp_cfg->stage_id,
                      grp_cfg->group_id, part_id, &ext_codes));
        sal_memcpy(&(grp_cfg->ext_codes[part_id]),
                   ext_codes, sizeof(bcmfp_ext_sel_t));
        for (eidx = 0; eidx < BCMFP_EXT_PMUX_SELECTORS_MAX; eidx++) {
            if (ext_codes->pmux_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_PMUX;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->pmux_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L1_E32_SELECTORS_MAX; eidx++) {
            if (ext_codes->l1_e32_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l1_e32_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 32;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L1_E16_SELECTORS_MAX; eidx++) {
            if (ext_codes->l1_e16_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l1_e16_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 16;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L1_E8_SELECTORS_MAX; eidx++) {
            if (ext_codes->l1_e8_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l1_e8_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 8;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L1_E4_SELECTORS_MAX; eidx++) {
            if (ext_codes->l1_e4_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l1_e4_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 4;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L1_E2_SELECTORS_MAX; eidx++) {
            if (ext_codes->l1_e2_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l1_e2_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 2;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L2_E16_SELECTORS_MAX; eidx++) {
            if (ext_codes->l2_e16_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l2_e16_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 2;
                ext_ctrl_info[idx].gran = 16;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L3_E1_SELECTORS_MAX; eidx++) {
            if (ext_codes->l3_e1_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l3_e1_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 3;
                ext_ctrl_info[idx].gran = 1;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L3_E2_SELECTORS_MAX; eidx++) {
            if (ext_codes->l3_e2_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l3_e2_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 3;
                ext_ctrl_info[idx].gran = 2;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        for (eidx = 0; eidx < BCMFP_EXT_L3_E4_SELECTORS_MAX; eidx++) {
            if (ext_codes->l3_e4_sel[eidx] != BCMFP_EXT_SELCODE_DONT_CARE) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val = ext_codes->l3_e4_sel[eidx];
                ext_ctrl_info[idx].part = part_id;
                ext_ctrl_info[idx].level = 3;
                ext_ctrl_info[idx].gran = 4;
                ext_ctrl_info[idx].ext_num = eidx;
                idx++;
            }
        }
        if (ext_codes->aux_tag_a_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_a_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->aux_tag_b_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_b_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->aux_tag_c_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_c_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->aux_tag_d_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_d_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->tcp_fn_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tcp_fn_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->ttl_fn_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ttl_fn_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->tos_fn_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tos_fn_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->class_id_cont_a_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_a_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->class_id_cont_b_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_b_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->class_id_cont_c_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_c_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->class_id_cont_d_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_d_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->src_cont_a_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_a_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->src_cont_b_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_b_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->src_dest_cont_0_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_0_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->src_dest_cont_1_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_1_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->ipbm_present != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ipbm_source;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->normalize_l3_l4_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_l3_l4_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
        if (ext_codes->normalize_mac_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_mac_sel;
            ext_ctrl_info[idx].part = part_id;
            idx++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_update(int unit,
                   uint32_t trans_id,
                   bcmfp_group_t *grp_cfg)
{
    int qual_id_count = 0;
    uint16_t new_qualifiers_count = 0;
    bcmfp_qual_t qual_id;
    bcmfp_qset_t old_qset;
    bcmfp_qset_t new_qset;
    bool is_present = FALSE;
    bool copy_old_qset = FALSE;
    bcmfp_keygen_mode_t keygen_mode;
    bcmfp_keygen_mode_t keygen_ext_mode;
    bcmfp_keygen_cfg_t keygen_cfg;
    bcmfp_keygen_oper_t keygen_oper;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&keygen_cfg, 0, sizeof(bcmfp_keygen_cfg_t));
    sal_memset(&keygen_oper, 0, sizeof(bcmfp_keygen_oper_t));

    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);

    SHR_BITCOUNT_RANGE(grp_cfg->qset.w,
                       qual_id_count,
                       0, BCMFP_QUAL_COUNT);
    SHR_IF_ERR_EXIT
        ((qual_id_count == 0) ? SHR_E_CONFIG : SHR_E_NONE);

    sal_memset(&new_qset, 0, sizeof(bcmfp_qset_t));
    SHR_BIT_ITER(grp_cfg->qset.w, BCMFP_QUAL_COUNT, qual_id) {
        is_present = FALSE;
        SHR_IF_ERR_EXIT
            (bcmfp_group_qual_present(unit, grp_cfg->stage_id,
                    grp_cfg->group_id, qual_id, &is_present));
        if (is_present == TRUE) {
            continue;
        }
        /* Post mux qualifiers are not allowed to
         * add when group is updated.
         */
        SHR_BITSET(new_qset.w, qual_id);
        new_qualifiers_count++;
    }

    sal_memcpy(&old_qset, &(grp_cfg->qset), sizeof(bcmfp_qset_t));
    copy_old_qset = TRUE;
    sal_memcpy(&(grp_cfg->qset), &new_qset, sizeof(bcmfp_qset_t));
    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, grp_cfg->stage_id, &stage));
    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL &&
        new_qualifiers_count) {
        keygen_cfg.qual_cfg_info_db = stage->kgn_qual_cfg_info_db;
        SHR_IF_ERR_EXIT
            (bcmfp_grp_keygen_qual_info_alloc(unit,
                             grp_cfg, &keygen_cfg));
        SHR_IF_ERR_EXIT
            (bcmfp_group_flags_get(unit, grp_cfg->stage_id,
                        grp_cfg->group_id, &(grp_cfg->flags)));
        SHR_IF_ERR_EXIT
            (bcmfp_group_parts_count(unit, grp_cfg->flags,
                                &(keygen_cfg.num_parts)));
        SHR_IF_ERR_EXIT
            (bcmfp_group_ext_ctrls_init(unit, grp_cfg, &keygen_cfg));

        if (grp_cfg->group_mode == BCMFP_GROUP_MODE_SINGLE) {
            keygen_mode = BCMFP_KEYGEN_MODE_SINGLE;
            keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
        } else if (grp_cfg->group_mode ==
                   BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
            keygen_mode = BCMFP_KEYGEN_MODE_SINGLE_ASET_WIDE;
            keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
        } else if (grp_cfg->group_mode ==
                   BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {
            keygen_mode = BCMFP_KEYGEN_MODE_SINGLE_ASET_NARROW;
            keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
        } else if (grp_cfg->group_mode == BCMFP_GROUP_MODE_DBLINTRA) {
            keygen_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
            keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
        } else if (grp_cfg->group_mode == BCMFP_GROUP_MODE_DBLINTER) {
            keygen_mode = BCMFP_KEYGEN_MODE_DBLINTER;
            keygen_ext_mode = BCMFP_KEYGEN_MODE_DBLINTER;
        } else if (grp_cfg->group_mode == BCMFP_GROUP_MODE_TRIPLE) {
            keygen_mode = BCMFP_KEYGEN_MODE_TRIPLE;
            keygen_ext_mode = BCMFP_KEYGEN_MODE_TRIPLE;
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        keygen_cfg.mode = keygen_mode;
        keygen_cfg.ext_cfg_db = stage->kgn_ext_cfg_db_arr[keygen_ext_mode];
        keygen_cfg.flags |= BCMFP_KEYGEN_CFG_FLAGS_QSET_UPDATE;
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_cfg_process(unit, &keygen_cfg, &keygen_oper));
        SHR_IF_ERR_EXIT(bcmfp_group_oper_info_update(unit, grp_cfg));
    }

exit:
    if (copy_old_qset == TRUE) {
        sal_memcpy(&(grp_cfg->qset), &old_qset, sizeof(bcmfp_qset_t));
    }
    SHR_FREE(keygen_cfg.qual_info_arr);
    SHR_FREE(keygen_cfg.ext_ctrl_sel_info);
    SHR_FUNC_EXIT();
}
