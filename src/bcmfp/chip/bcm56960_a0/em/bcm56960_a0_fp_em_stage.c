/*! \file bcm56960_a0_fp_em_stage.c
 *
 * API to initialize stage attributes for Tomahawk(56960_A0) device.
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
#include <bcmfp/chip/bcm56960_a0_fp.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_bcm56960_a0_exactmatch_stage_init(int unit,
                                     bcmfp_stage_t *stage)
{
    bcmfp_group_mode_t mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmfp_qset_aset_size_info_t *key_data_sz_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->flags |= (BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE |
                     BCMFP_STAGE_KEY_TYPE_PRESEL |
                     BCMFP_STAGE_ENTRY_TYPE_HASH |
                     BCMFP_STAGE_POLICY_TYPE_PDD |
                     BCMFP_STAGE_KEYGEN_PROFILE |
                     BCMFP_STAGE_ACTION_PROFILE |
                     BCMFP_STAGE_DEFAULT_POLICY |
                     BCMFP_STAGE_ACTION_QOS_PROFILE);

    for (mode = 0; mode < BCMFP_GROUP_MODE_COUNT; mode++) {
        stage->hw_entry_info[mode] = NULL;
    }
    /* max key gen profiles available for EM are only 16 */
    stage->num_group_action_res_ids = 16;

    /* Aset and Qset size stucture */
    BCMFP_ALLOC(key_data_sz_info,
                sizeof(bcmfp_qset_aset_size_info_t),
                "data and key size info for group modes in em");

    /* Aset Narrow Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "Hardware entry info for aset narrow group in em");
    hw_entry_info->sid = EXACT_MATCH_2m;
    hw_entry_info->sid_data_only = EXACT_MATCH_2m;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->key_fid = VALIDf;
    hw_entry_info->mask_fid = VALIDf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = 32768;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE_ASET_NARROW);
    hw_entry_info = NULL;
    key_data_sz_info->key_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = 160;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = 18;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][0] = 101;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][1] = 59;
    key_data_sz_info->policy_data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_ACTION_DATAf;
    key_data_sz_info->data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_DATAf;
    key_data_sz_info->action_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_ACTION_PROFILE_IDf;
    key_data_sz_info->qos_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_QOS_PROFILE_IDf;
    key_data_sz_info->em_class_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_EXACT_MATCH_CLASS_IDf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][0] =
                                         MODE160v_KEY_0_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][1] =
                                         MODE160v_KEY_1_ONLYf;
    key_data_sz_info->em_mode[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = 1;

    /* Aset Wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "Hardware entry info for aset wide group in em");
    hw_entry_info->sid = EXACT_MATCH_2m;
    hw_entry_info->sid_data_only = EXACT_MATCH_2m;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->key_fid = VALIDf;
    hw_entry_info->mask_fid = VALIDf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_entries = 32768;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE_ASET_WIDE);
    hw_entry_info = NULL;
    key_data_sz_info->key_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = 128;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = 50;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][0] = 101;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][1] = 27;
    key_data_sz_info->policy_data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_ACTION_DATAf;
    key_data_sz_info->data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_DATAf;
    key_data_sz_info->action_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_ACTION_PROFILE_IDf;
    key_data_sz_info->qos_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_QOS_PROFILE_IDf;
    key_data_sz_info->em_class_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_EXACT_MATCH_CLASS_IDf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][0] =
                                         MODE128v_KEY_0_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][1] =
                                         MODE128v_KEY_1_ONLYf;
    key_data_sz_info->em_mode[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = 0;

    /* Inter Slice Double wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "Hardware entry info for dblinter wide group in ingress");
    hw_entry_info->sid = EXACT_MATCH_4m;
    hw_entry_info->sid_data_only = EXACT_MATCH_4m;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->key_fid = VALIDf;
    hw_entry_info->mask_fid = VALIDf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_entries = 32768;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTER] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_DBLINTER);
    hw_entry_info = NULL;
    key_data_sz_info->key_size[BCMFP_GROUP_MODE_DBLINTER] = 320;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_DBLINTER] = 60;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][0] = 101;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][1] = 101;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][2] = 101;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][3] = 17;
    key_data_sz_info->policy_data_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_ACTION_DATAf;
    key_data_sz_info->data_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_DATAf;
    key_data_sz_info->action_prof_id_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_ACTION_PROFILE_IDf;
    key_data_sz_info->qos_prof_id_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_QOS_PROFILE_IDf;
    key_data_sz_info->em_class_id_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_EXACT_MATCH_CLASS_IDf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_DBLINTER][0] =
                                         MODE320v_KEY_0_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_DBLINTER][1] =
                                         MODE320v_KEY_1_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_DBLINTER][2] =
                                         MODE320v_KEY_2_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_DBLINTER][3] =
                                         MODE320v_KEY_3_ONLYf;
    key_data_sz_info->em_mode[BCMFP_GROUP_MODE_DBLINTER] = 2;

    stage->key_data_size_info = key_data_sz_info;
    key_data_sz_info = NULL;

    /* Initialize the stage miscellaneous information */
    BCMFP_ALLOC(stage->misc_info,
                sizeof(bcmfp_stage_misc_info_t),
                "EM Stage miscellaneous information");
    stage->misc_info->num_group_action_res_ids = 32;
    stage->misc_info->num_groups = 16;
    stage->misc_info->num_keygen_prof = 16;
    stage->misc_info->num_action_prof = 32;
    stage->misc_info->num_qos_prof = 128;

exit:
    SHR_FREE(key_data_sz_info);
    SHR_FUNC_EXIT();
}
