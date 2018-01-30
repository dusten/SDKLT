/*! \file bcmfp_group.c
 *
 * FP group related APIs.
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
#include <bcmfp/bcmfp_lt_util.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_group_parts_count(int unit,
                        uint32_t group_flags,
                        uint8_t *part_count)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(part_count, SHR_E_PARAM);

    /* Check group flags settings and determine number of TCAM entry parts. */
    if (group_flags & BCMFP_GROUP_SPAN_SINGLE_SLICE) {
        if ((group_flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE) &&
            !(group_flags & BCMFP_GROUP_INTRASLICE_SPAN_ONE_PART)) {
            *part_count = 2;
        } else {
            *part_count = 1;
        }
    } else if (group_flags & BCMFP_GROUP_SPAN_DOUBLE_SLICE) {
        if ((group_flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE) &&
            !(group_flags & BCMFP_GROUP_INTRASLICE_SPAN_ONE_PART)) {
            *part_count = 4;
        } else {
            *part_count = 2;
        }
    } else if (group_flags & BCMFP_GROUP_SPAN_TRIPLE_SLICE) {
        *part_count = 3;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_qual_bitmap_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_qual_t qual,
                            uint32_t *qual_bitmap)
{
    uint32_t qual_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_bitmap, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_qual_data_size_get(unit, stage_id, qual, &qual_size));

    SHR_BITSET_RANGE(qual_bitmap, 0, qual_size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_action_res_id_allocate(int unit,
                                   int tbl_inst,
                                   bcmfp_stage_id_t stage_id,
                                   uint8_t *action_res_id)
{
    uint8_t id = 0;
    uint8_t num_action_res_ids = 0;
    bcmfp_stage_t *ing_stage = NULL;
    bcmfp_stage_t *em_stage = NULL;
    bcmfp_group_oper_info_t *ing_group_oper_info = NULL;
    bcmfp_group_oper_info_t *em_group_oper_info = NULL;
    uint16_t idx = 0;
    bcmfp_group_action_res_id_bmp_t action_res_id_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_res_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit,
                                    BCMFP_STAGE_ID_INGRESS,
                                    &ing_stage));
    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  BCMFP_STAGE_ID_INGRESS,
                                  0, &ing_group_oper_info));

    *action_res_id = 0;
    num_action_res_ids = 0;
    sal_memset(&action_res_id_bmp, 0,
                     sizeof(bcmfp_group_action_res_id_bmp_t));
    for (idx = 0; idx < ing_stage->misc_info->num_groups; idx++)  {
        if (ing_group_oper_info[idx].valid == FALSE) {
            continue;
        }

        /* For global mode(tbl_inst=-1), the same action_res_id has to be free
         * for all the pipes, to be used in group creation. So in global mode
         * set the  pbmp with all the action_res_ids in use, irrespective of the
         * instances. This pbmp contains the action_res_id in use for both EM
         * and IFP */
        if ((tbl_inst == -1) ||
            (ing_group_oper_info[idx].tbl_inst == -1) ||
            (ing_group_oper_info[idx].tbl_inst == tbl_inst)) {
            if (!SHR_BITGET(action_res_id_bmp.w,
                    ing_group_oper_info[idx].action_res_id)) {
                SHR_BITSET(action_res_id_bmp.w,
                        ing_group_oper_info[idx].action_res_id);
                num_action_res_ids++;
            }
        }
    }
    
    BCMFP_RET_VAL_ASSIGN(bcmfp_stage_get(unit,
                                         BCMFP_STAGE_ID_EXACT_MATCH,
                                         &em_stage));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) && (em_stage)) {
        SHR_IF_ERR_EXIT(
                bcmfp_group_oper_info_get(unit,
                                          BCMFP_STAGE_ID_EXACT_MATCH,
                                          0, &em_group_oper_info));
        for (idx = 0; idx < em_stage->misc_info->num_groups; idx++)  {
            if (em_group_oper_info[idx].valid == FALSE) {
                continue;
            }
            if ((tbl_inst == -1) ||
                (em_group_oper_info[idx].tbl_inst == -1) ||
                (em_group_oper_info[idx].tbl_inst == tbl_inst)) {
            if (!SHR_BITGET(action_res_id_bmp.w,
                    em_group_oper_info[idx].action_res_id)) {
                    SHR_BITSET(action_res_id_bmp.w,
                            em_group_oper_info[idx].action_res_id);
                    num_action_res_ids++;
                }
            }
        }
    }

    if (num_action_res_ids == ing_stage->num_group_action_res_ids) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

    for (id = 0; id < ing_stage->num_group_action_res_ids; id++) {
        if (!SHR_BITGET(action_res_id_bmp.w, id)) {
            *action_res_id = id;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_delete(int unit, uint32_t trans_id,
                   bcmfp_stage_id_t stage_id,
                   bcmfp_group_id_t group_id)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
        SHR_IF_ERR_EXIT(bcmfp_group_uninstall(unit, trans_id, stage, group_id));
        if (stage->flags & BCMFP_STAGE_ACTION_PROFILE) {
            SHR_IF_ERR_EXIT(
                bcmfp_group_action_profile_uninstall(unit, trans_id,
                                                     stage, group_id));

            SHR_IF_ERR_EXIT(
                bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));
            if (0 != group_oper_info->ext_codes[0].default_policy_enable) {
                SHR_IF_ERR_EXIT(bcmfp_default_policy_uninstall(unit,
                                  trans_id,
                                  stage,
                                  group_id,
                                  group_oper_info->ext_codes[0].keygen_index));
                group_oper_info->flags |= BCMFP_GROUP_WITH_DEFAULT_POLICY;
                SHR_IF_ERR_EXIT(
                    bcmfp_group_action_profile_uninstall(unit,
                                                         trans_id,
                                                         stage,
                                                         group_id));
                group_oper_info->flags &= ~BCMFP_GROUP_WITH_DEFAULT_POLICY;
                SHR_IF_ERR_EXIT(
                    bcmfp_entry_qos_profile_uninstall(unit,
                                  trans_id,
                                  stage,
                                  group_id,
                                  group_oper_info->ext_codes[0].default_qos_prof_idx));
            }
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_oper_info_delete(unit, stage_id, group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_oper_info_t *group_oper_info,
                        bcmptm_rm_tcam_fp_group_selcode_t *selcode)
{
    uint8_t  part = 0;
    uint8_t  parts_count = 0;
    bcmfp_ext_sel_t  *ext_ptr;
    bcmptm_rm_tcam_fp_group_selcode_t  *sel_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_oper_info, SHR_E_PARAM);
    SHR_NULL_CHECK(selcode, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit,
                                group_oper_info->flags,
                                &parts_count));

    /* Fill in selcodes to be pass for entry insertion */
    sal_memset(selcode, -1,
       (sizeof(bcmptm_rm_tcam_fp_group_selcode_t) * BCMPTM_TCAM_FP_MAX_PARTS));

    for (part = 0; part < parts_count; part++) {
        sel_ptr = &selcode[part];
        ext_ptr = &group_oper_info->ext_codes[part];
        sel_ptr->intraslice = ext_ptr->intraslice;
        sel_ptr->fpf2       = ext_ptr->fpf2;
        sel_ptr->fpf3       = ext_ptr->fpf3;
        if (stage->flags & BCMFP_STAGE_KEY_TYPE_FIXED) {
            sel_ptr->egr_key1_classid_sel = ext_ptr->egr_key1_classid_sel;
            sel_ptr->egr_key2_classid_sel = ext_ptr->egr_key2_classid_sel;
            sel_ptr->egr_key3_classid_sel = ext_ptr->egr_key3_classid_sel;
            sel_ptr->egr_key4_classid_sel = ext_ptr->egr_key4_classid_sel;
            sel_ptr->egr_key6_classid_sel = ext_ptr->egr_key6_classid_sel;
            sel_ptr->egr_key7_classid_sel = ext_ptr->egr_key7_classid_sel;
            sel_ptr->egr_key8_classid_sel = ext_ptr->egr_key8_classid_sel;
            sel_ptr->egr_key4_dvp_sel     = ext_ptr->egr_key4_dvp_sel;
            sel_ptr->egr_key8_dvp_sel     = ext_ptr->egr_key8_dvp_sel;
            sel_ptr->egr_key4_mdl_sel     = ext_ptr->egr_key4_mdl_sel;
            sel_ptr->egr_ip_addr_sel      = ext_ptr->egr_ip_addr_sel;
        } else {
            sel_ptr->src_sel       = ext_ptr->src_sel;
            sel_ptr->ip_header_sel = ext_ptr->ip_header_sel;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_combined_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_t *fg)
{
    uint8_t idx = 0;
    uint8_t  part = 0;
    uint8_t  parts_count = 0;
    bcmfp_ext_sel_t  *ext_ptr;
    bcmfp_ext_sel_t  *sel_ptr;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    if (!(stage->flags & BCMFP_STAGE_KEY_TYPE_FIXED)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage->stage_id,
                                  0, &group_oper_info));

    parts_count = (fg->group_mode == BCMFP_GROUP_MODE_SINGLE) ? 1 : 2;

    for (idx = 0; idx < stage->misc_info->num_groups; idx++) {
        if (group_oper_info[idx].valid == FALSE) {
            continue;
        }
        if ((group_oper_info[idx].group_id == fg->group_id) ||
                (group_oper_info[idx].tbl_inst != fg->pipe_id) ||
                (group_oper_info[idx].group_mode != fg->group_mode) ||
                (group_oper_info[idx].group_prio != fg->group_prio)) {
            continue;
        }

        /* Fill in selcodes to be pass for entry insertion */
        for (part = 0; part < parts_count; part++) {
            sel_ptr = &fg->ext_codes[part];
            ext_ptr = &(group_oper_info[idx].ext_codes[part]);

            if (sel_ptr->egr_key1_classid_sel != -1 &&
                    ext_ptr->egr_key1_classid_sel != -1 &&
                    sel_ptr->egr_key1_classid_sel !=
                    ext_ptr->egr_key1_classid_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key1_classid_sel != -1) {
                sel_ptr->egr_key1_classid_sel = ext_ptr->egr_key1_classid_sel;
            }

            if (sel_ptr->egr_key2_classid_sel != -1 &&
                    ext_ptr->egr_key2_classid_sel != -1 &&
                    sel_ptr->egr_key2_classid_sel !=
                    ext_ptr->egr_key2_classid_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key2_classid_sel != -1) {
                sel_ptr->egr_key2_classid_sel = ext_ptr->egr_key2_classid_sel;
            }

            if (sel_ptr->egr_key3_classid_sel != -1 &&
                    ext_ptr->egr_key3_classid_sel != -1 &&
                    sel_ptr->egr_key3_classid_sel !=
                    ext_ptr->egr_key3_classid_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key3_classid_sel != -1) {
                sel_ptr->egr_key3_classid_sel = ext_ptr->egr_key3_classid_sel;
            }

            if (sel_ptr->egr_key4_classid_sel != -1 &&
                    ext_ptr->egr_key4_classid_sel != -1 &&
                    sel_ptr->egr_key4_classid_sel !=
                    ext_ptr->egr_key4_classid_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key4_classid_sel != -1) {
                sel_ptr->egr_key4_classid_sel = ext_ptr->egr_key4_classid_sel;
            }

            if (sel_ptr->egr_key6_classid_sel != -1 &&
                    ext_ptr->egr_key6_classid_sel != -1 &&
                    sel_ptr->egr_key6_classid_sel !=
                    ext_ptr->egr_key6_classid_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key6_classid_sel != -1) {
                sel_ptr->egr_key6_classid_sel = ext_ptr->egr_key6_classid_sel;
            }

            if (sel_ptr->egr_key7_classid_sel != -1 &&
                    ext_ptr->egr_key7_classid_sel != -1 &&
                    sel_ptr->egr_key7_classid_sel !=
                    ext_ptr->egr_key7_classid_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key7_classid_sel != -1) {
                sel_ptr->egr_key7_classid_sel = ext_ptr->egr_key7_classid_sel;
            }

            if (sel_ptr->egr_key8_classid_sel != -1 &&
                    ext_ptr->egr_key8_classid_sel != -1 &&
                    sel_ptr->egr_key8_classid_sel !=
                    ext_ptr->egr_key8_classid_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key8_classid_sel != -1) {
                sel_ptr->egr_key8_classid_sel = ext_ptr->egr_key8_classid_sel;
            }

            if (sel_ptr->egr_key4_dvp_sel != -1 &&
                    ext_ptr->egr_key4_dvp_sel != -1 &&
                    sel_ptr->egr_key4_dvp_sel !=
                    ext_ptr->egr_key4_dvp_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key4_dvp_sel != -1) {
                sel_ptr->egr_key4_dvp_sel     = ext_ptr->egr_key4_dvp_sel;
            }

            if (sel_ptr->egr_key8_dvp_sel != -1 &&
                    ext_ptr->egr_key8_dvp_sel != -1 &&
                    sel_ptr->egr_key8_dvp_sel !=
                    ext_ptr->egr_key8_dvp_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key8_dvp_sel != -1) {
                sel_ptr->egr_key8_dvp_sel     = ext_ptr->egr_key8_dvp_sel;
            }

            if (sel_ptr->egr_key4_mdl_sel != -1 &&
                    ext_ptr->egr_key4_mdl_sel != -1 &&
                    sel_ptr->egr_key4_mdl_sel !=
                    ext_ptr->egr_key4_mdl_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key4_mdl_sel != -1) {
                sel_ptr->egr_key4_mdl_sel     = ext_ptr->egr_key4_mdl_sel;
            }

            if (sel_ptr->egr_ip_addr_sel != -1 &&
                    ext_ptr->egr_ip_addr_sel != -1 &&
                    sel_ptr->egr_ip_addr_sel !=
                    ext_ptr->egr_ip_addr_sel) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_ip_addr_sel != -1) {
                sel_ptr->egr_ip_addr_sel      = ext_ptr->egr_ip_addr_sel;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/* Below function can be enhanced to do all presel related checks
 * for all relevant stages
 */
int bcmfp_group_presel_entry_validate(int unit,
                                      int tbl_inst,
                                      bcmfp_stage_id_t stage_id,
                                      uint8_t lookup_id,
                                      uint8_t num_presel_ids)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit,
                                    stage_id,
                                    &stage));

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  0, &group_oper_info));

    for (idx = 0; idx < stage->misc_info->num_groups; idx++)  {
        if (group_oper_info[idx].valid == FALSE) {
            continue;
        }
        /* Two groups with default presel cannot have same lookup_id/slice_id */
        if (group_oper_info[idx].tbl_inst == tbl_inst) {
            if ((group_oper_info[idx].slice_id == lookup_id)
                && (group_oper_info[idx].flags & BCMFP_GROUP_WITH_DEFAULT_PRESEL)
                && (0 == num_presel_ids)) {
                SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
