/*! \file bcmfp_keygen_pmux.c
 *
 * Post Mux Qualifier allocation APIs.
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
bcmfp_keygen_pmux_alloc(int unit,
                       bcmfp_keygen_cfg_t *keygen_cfg,
                       bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint8_t found_part = FALSE;
    uint8_t num_qual_cfg = 0;
    uint8_t qual_cfg_id = 0;
    uint8_t pmux_idx = 0;
    bcmfp_qual_t pmux_qual;
    bcmfp_keygen_ext_sel_t *pmux_sel = NULL;
    bcmfp_keygen_qual_flags_bmp_t *qual_flags = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    num_parts = keygen_cfg->num_parts;
    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        pmux_qual = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg = qual_cfg_info[pmux_qual]->qual_cfg_arr;
        qual_flags = &(qual_cfg->qual_flags);
        if (!SHR_BITGET(qual_flags->w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        if (qual_cfg->pri_ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE ||
            qual_cfg->pri_ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            keygen_md->pmux_info[pmux_idx].pmux_qual = pmux_qual;
            keygen_md->pmux_info[pmux_idx].pmux_part = 0;
            keygen_md->pmux_info[pmux_idx].pmux_qual_cfg = qual_cfg;
            pmux_idx++;
            continue;
        }
        if (pmux_sel == NULL) {
            BCMFP_ALLOC(pmux_sel,
                sizeof(bcmfp_keygen_ext_sel_t) * num_parts,
                "Post mux qualifiers secondary selectors");
            for (part = 0; part < num_parts; part++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_keygen_ext_codes_init(unit,
                                  &(pmux_sel[part])));
            }
        }

        found_part = FALSE;
        num_qual_cfg = qual_cfg_info[pmux_qual]->num_qual_cfg;
        for (qual_cfg_id = 0;
             qual_cfg_id < num_qual_cfg;
             qual_cfg_id++) {
             if (SHR_BITGET(qual_cfg[qual_cfg_id].qual_flags.w,
                 BCMFP_KEYGEN_QUAL_FLAGS_NOT_IN_SINGLE) &&
                 keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE) {
                 continue;
            }
            switch (qual_cfg[qual_cfg_id].pri_ctrl_sel) {
                case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].dst_cont_a_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].dst_cont_a_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].dst_cont_a_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].dst_cont_b_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].dst_cont_b_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].dst_cont_b_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].src_dest_cont_0_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].src_dest_cont_0_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].src_dest_cont_0_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].src_dest_cont_1_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].src_dest_cont_1_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].src_dest_cont_1_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_C:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].class_id_cont_c_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].class_id_cont_c_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].class_id_cont_c_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASSID_D:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].class_id_cont_d_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].class_id_cont_d_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].class_id_cont_d_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            if (found_part == TRUE) {
                break;
            }
        }
        if (found_part == FALSE) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        } else {
            keygen_md->pmux_info[pmux_idx].pmux_qual = pmux_qual;
            keygen_md->pmux_info[pmux_idx].pmux_part = part;
            keygen_md->pmux_info[pmux_idx].pmux_qual_cfg =
                                   &(qual_cfg[qual_cfg_id]);
           pmux_idx++;
       }
    }
exit:
    SHR_FREE(pmux_sel);
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_pmux_control_set(int unit,
                             bcmfp_qual_t qual,
                             bcmfp_keygen_md_t *keygen_md,
                             bcmfp_keygen_cfg_t *keygen_cfg)
{
    uint8_t idx = 0;
    uint8_t pmux_num = 0;
    uint8_t pmux_part = 0;
    bcmfp_keygen_ext_sel_t *ext_codes = NULL;
    bcmfp_keygen_pmux_info_t *pmux_info = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_pmux_info_get(unit,
                            qual,
                            keygen_md->num_pmux_qual,
                            keygen_md->pmux_info,
                            &pmux_info));

    pmux_part = pmux_info->pmux_part;
    qual_cfg = pmux_info->pmux_qual_cfg;
    ext_codes = &(keygen_md->ext_codes[pmux_part]);
    for (idx = 0; idx < qual_cfg->num_chunks; idx++) {
        pmux_num = qual_cfg->e_params[idx].sec_val;
        switch (qual_cfg->e_params[idx].section) {
            case BCMFP_KEYGEN_EXT_SECTION_IPBM:
                ext_codes->ipbm_source = qual_cfg->pri_ctrl_sel_val;
                ext_codes->ipbm_present = 1;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_DROP:
            case BCMFP_KEYGEN_EXT_SECTION_NAT_NEEDED:
            case BCMFP_KEYGEN_EXT_SECTION_NAT_DST_REALMID:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_0_3:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_4_7:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_8_11:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_0_3:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_4_7:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_8_11:
            case BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3:
            case BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7:
            case BCMFP_KEYGEN_EXT_SECTION_PKT_RESOLUTION:
            case BCMFP_KEYGEN_EXT_SECTION_INT_PRI:
            case BCMFP_KEYGEN_EXT_SECTION_IFP_DROP_VECTOR:
            case BCMFP_KEYGEN_EXT_SECTION_MH_OPCODE:
            case BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB:
                keygen_md->ext_codes[pmux_part].pmux_sel[pmux_num] = 1;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->src_dest_cont_0_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->src_dest_cont_1_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_A:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->dst_cont_a_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_B:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->dst_cont_b_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_C:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->class_id_cont_c_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_D:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->class_id_cont_d_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_pmux_info_get(int unit,
                    bcmfp_qual_t qual,
                    uint8_t num_pmux_qual,
                    bcmfp_keygen_pmux_info_t *pmux_info_arr,
                    bcmfp_keygen_pmux_info_t **pmux_info)
{
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pmux_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pmux_info_arr, SHR_E_PARAM);

    *pmux_info = NULL;
    for (idx = 0; idx < num_pmux_qual; idx++) {
        if (pmux_info_arr[idx].pmux_qual == qual) {
            *pmux_info = &(pmux_info_arr[idx]);
        }
    }

    if (pmux_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_pmux_qual_offset_get(int unit,
                                 bcmfp_qual_t qual_id,
                                 bcmfp_keygen_md_t *keygen_md,
                                 bcmfp_keygen_cfg_t *keygen_cfg,
                                 bcmfp_keygen_qual_offset_t *qual_offset)
{
     uint8_t idx;
     bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
     bcmfp_keygen_pmux_info_t *pmux_info = NULL;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
     SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
     SHR_NULL_CHECK(qual_offset, SHR_E_PARAM);

     sal_memset(qual_offset, 0, sizeof(bcmfp_keygen_qual_offset_t));

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_keygen_pmux_info_get(unit,
                             qual_id,
                             keygen_md->num_pmux_qual,
                             keygen_md->pmux_info,
                             &pmux_info));
     SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_keygen_pmux_control_set(unit, qual_id,
                             keygen_md, keygen_cfg));
     qual_cfg = pmux_info->pmux_qual_cfg;
     for (idx = 0; idx < qual_cfg->num_chunks; idx++) {
         qual_offset->offset[idx] = qual_cfg->e_params[idx].offset;
         qual_offset->width[idx] = qual_cfg->e_params[idx].width;
         qual_offset->num_offsets++;
     }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_pmux_process(int unit,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t num_pmux_qual = 0;
    bcmfp_qual_t pmux_qual;
    bcmfp_keygen_qual_flags_bmp_t *qual_flags = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        pmux_qual = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg = qual_cfg_info[pmux_qual]->qual_cfg_arr;
        qual_flags = &(qual_cfg->qual_flags);
        if (!SHR_BITGET(qual_flags->w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        num_pmux_qual++;
    }

    if (num_pmux_qual == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    BCMFP_ALLOC(keygen_md->pmux_info,
                num_pmux_qual * sizeof(bcmfp_keygen_pmux_info_t),
                "Groups Post Mux Qualifiers Information");
    keygen_md->num_pmux_qual = num_pmux_qual;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_pmux_alloc(unit, keygen_cfg, keygen_md));
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(keygen_md->pmux_info);
    }
    SHR_FUNC_EXIT();
}
