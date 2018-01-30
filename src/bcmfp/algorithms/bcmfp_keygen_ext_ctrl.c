/*! \file bcmfp_keygen_ext_ctrl.c
 *
 * Utility APIs to generate secondary selectors.
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

#include "bcmfp_keygen_md.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

STATIC int
bcmfp_keygen_ext_code_part_get(int unit,
                              bcmfp_keygen_md_t *keygen_md,
                              bcmfp_keygen_ext_ctrl_sel_t ctrl_sel,
                              uint8_t ctrl_sel_val,
                              int8_t *part)
{
    uint8_t idx = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(part, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    *part = -1;
    for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
       finfo = &(keygen_md->finfo[idx]);
       if (finfo->section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (finfo->part == -1) {
           continue;
       }
       if (finfo->pri_ctrl_sel == ctrl_sel &&
           finfo->pri_ctrl_sel_val == ctrl_sel_val) {
           *part = finfo->part;
           break;
       }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_codes_init(int unit, bcmfp_keygen_ext_sel_t *ext_codes)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    sal_memset(ext_codes, BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE, sizeof(bcmfp_keygen_ext_sel_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_codes_create(int unit,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_md_t *keygen_md,
                             bcmfp_keygen_oper_t *keygen_oper)
{
    uint8_t part = 0;
    uint8_t level = 0;
    uint16_t idx = 0;
    uint16_t pmux_idx = 0;
    uint16_t ext_idx = 0;
    uint16_t num_ext_codes = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_sel_t *ext_codes = NULL;
    bcmfp_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_oper, SHR_E_PARAM);

    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMFP_KEYGEN_EXT_ATTR_PASS_THRU)) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                continue;
            }
            if (ext_cfg->in_use == TRUE) {
                num_ext_codes++;
            }
        }
    }

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        ext_codes = &(keygen_md->ext_codes[part]);
        if (ext_codes->aux_tag_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->ttl_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->tcp_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->tos_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_dest_cont_0_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_dest_cont_1_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->dst_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->dst_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->ipbm_present != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->normalize_l3_l4_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->normalize_mac_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->udf_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        for (pmux_idx = 0; pmux_idx < 15; pmux_idx++) {
            if (ext_codes->pmux_sel[pmux_idx] == 1) {
                num_ext_codes++;
            }
        }
    }

    SHR_IF_ERR_EXIT((num_ext_codes == 0) ? SHR_E_INTERNAL : SHR_E_NONE);

    keygen_oper->ext_ctrl_sel_info_count = num_ext_codes;
    BCMFP_ALLOC(keygen_oper->ext_ctrl_sel_info,
                      sizeof(bcmfp_keygen_ext_ctrl_sel_info_t) * num_ext_codes,
                      "Keygen Extractor codes");
    ext_ctrl_info = keygen_oper->ext_ctrl_sel_info;
    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMFP_KEYGEN_EXT_ATTR_PASS_THRU)) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                continue;
            }
            if (ext_cfg->in_use == TRUE) {
                ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                finfo = ext_cfg->finfo;
                ext_ctrl_info[idx].ctrl_sel_val = finfo->sec_val;
                ext_ctrl_info[idx].ext_num =
                    BCMFP_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].part =
                    BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].level =
                    BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].gran =
                    BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                idx++;
            }
        }
    }
    for (part = 0; part < keygen_cfg->num_parts; part++) {
        ext_codes = &(keygen_md->ext_codes[part]);
        if (ext_codes->aux_tag_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->ttl_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ttl_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->tcp_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tcp_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->tos_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tos_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_dest_cont_0_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_0_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_dest_cont_1_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_1_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->dst_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->dst_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->dst_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->dst_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->ipbm_present != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ipbm_source;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->normalize_l3_l4_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_l3_l4_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->normalize_mac_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_mac_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->udf_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_UDF;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->udf_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        for (pmux_idx = 0; pmux_idx < 15; pmux_idx++) {
            if (ext_codes->pmux_sel[pmux_idx] == 1) {
                ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_PMUX;
                ext_ctrl_info[idx].ctrl_sel_val = pmux_idx;
                ext_ctrl_info[idx].part = part;
                idx++;
            }
        }
    }

    SHR_IF_ERR_EXIT((idx != num_ext_codes) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_codes_validate(int unit,
                               uint8_t part,
                               bcmfp_keygen_cfg_t *keygen_cfg,
                               bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_ctrl_sel_info_t *keygen_ext_code = NULL;
    bcmfp_keygen_ext_sel_t *grp_ext_code = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_ext_codes_init(unit, &(keygen_md->ext_codes[part])));
    for (idx = 0; idx < keygen_cfg->ext_ctrl_sel_info_count; idx++) {
        keygen_ext_code = &(keygen_cfg->ext_ctrl_sel_info[idx]);
        grp_ext_code = &(keygen_md->ext_codes[keygen_ext_code->part]);
        if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A) {
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
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A) {
            grp_ext_code->dst_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B) {
            grp_ext_code->dst_cont_b_sel = keygen_ext_code->ctrl_sel_val;
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
        } else {
            continue;
        }
    }
    for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
       finfo = &(keygen_md->finfo[idx]);
       if (finfo->section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (finfo->part != part) {
           continue;
       }
       switch (finfo->pri_ctrl_sel) {
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A:
               if (keygen_md->ext_codes[part].aux_tag_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B:
               if (keygen_md->ext_codes[part].aux_tag_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C:
               if (keygen_md->ext_codes[part].aux_tag_c_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_c_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_c_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D:
               if (keygen_md->ext_codes[part].aux_tag_d_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_d_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_d_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_A:
               if (keygen_md->ext_codes[part].class_id_cont_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_B:
               if (keygen_md->ext_codes[part].class_id_cont_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_C:
               if (keygen_md->ext_codes[part].class_id_cont_c_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_c_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_c_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_D:
               if (keygen_md->ext_codes[part].class_id_cont_d_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_d_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_d_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN:
               if (keygen_md->ext_codes[part].tcp_fn_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].tcp_fn_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].tcp_fn_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN:
               if (keygen_md->ext_codes[part].ttl_fn_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].ttl_fn_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].ttl_fn_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN:
               if (keygen_md->ext_codes[part].tos_fn_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].tos_fn_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].tos_fn_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A:
               if (keygen_md->ext_codes[part].src_cont_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_cont_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_cont_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B:
               if (keygen_md->ext_codes[part].src_cont_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_cont_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_cont_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A:
               if (keygen_md->ext_codes[part].dst_cont_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].dst_cont_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].dst_cont_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B:
               if (keygen_md->ext_codes[part].dst_cont_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].dst_cont_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].dst_cont_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0:
               if (keygen_md->ext_codes[part].src_dest_cont_0_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_dest_cont_0_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_dest_cont_0_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1:
               if (keygen_md->ext_codes[part].src_dest_cont_1_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_dest_cont_1_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_dest_cont_1_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT:
               if (keygen_md->ext_codes[0].ipbm_source ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[0].ipbm_source =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[0].ipbm_source !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM:
               if (keygen_md->ext_codes[part].normalize_l3_l4_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].normalize_l3_l4_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].normalize_l3_l4_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM:
               if (keygen_md->ext_codes[part].normalize_mac_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].normalize_mac_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].normalize_mac_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_UDF:
               if (keygen_md->ext_codes[part].udf_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].udf_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].udf_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE:
               break;
           default:
               SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
       }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_part_get(int unit,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md,
                         bcmfp_keygen_ext_field_info_t *finfo)
{
    int8_t part = -1;
    uint8_t idx = 0;
    uint8_t found_untried_part = FALSE;
    uint16_t bits_extracted = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    if (finfo->pri_ctrl_sel != BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_code_part_get(unit, keygen_md, finfo->pri_ctrl_sel,
                                          finfo->pri_ctrl_sel_val, &part));
    }

    if (part != -1 && !(keygen_md->tried_part_bmp[0] & (1 << part))) {
        keygen_md->tried_part_bmp[0] |= (1 << part);
        finfo->part = part;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < keygen_cfg->num_parts; idx++) {
        if (keygen_md->tried_part_bmp[0] & (1 << idx)) {
            continue;
        }
        if (!found_untried_part) {
            found_untried_part = TRUE;
            part = idx;
            bits_extracted = keygen_md->bits_extracted[idx][0];
            continue;
        }
        if (bits_extracted > keygen_md->bits_extracted[idx][0]) {
            part = idx;
            bits_extracted = keygen_md->bits_extracted[idx][0];
        }
    }

    if (part != -1) {
        keygen_md->tried_part_bmp[0] |= (1 << part);
        finfo->part = part;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_part_incr(int unit,
                          bcmfp_keygen_cfg_t *keygen_cfg,
                          bcmfp_keygen_md_t *keygen_md,
                          bcmfp_keygen_ext_section_t section)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    if (keygen_md->next_part_id[0] == (keygen_cfg->num_parts - 1)) {
        keygen_md->next_part_id[0] = 0;
    } else {
        if (keygen_md->tried_part_bmp[0] &
           (1 << keygen_md->next_part_id[0])) {
            keygen_md->next_part_id[0]++;
        }
    }

exit:
    SHR_FUNC_EXIT();

}
