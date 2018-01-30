/*! \file bcm56960_a0_fp_em_selcodes.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy the IFP group for
 * Tomahawk device(56960_A0).
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

static uint32_t kgp_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t kgp2_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t ap_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

static bcmdrd_sid_t em_profile_sids[] = {
                                    EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm,
                                    EXACT_MATCH_KEY_GEN_MASKm
                                 };

static bcmdrd_sid_t em_action_profile_sid[] = { EXACT_MATCH_ACTION_PROFILEm };
static bcmdrd_sid_t em_qos_profile_sid[] = { EXACT_MATCH_QOS_ACTIONS_PROFILEm };
static bcmdrd_sid_t em_entry_sids[] = {
                                         EXACT_MATCH_2m,
                                         EXACT_MATCH_4m
                                      };
static bcmdrd_sid_t em_default_policy_sid[] = {
                                                 EXACT_MATCH_DEFAULT_POLICYm
                                              };

STATIC int
bcmfp_bcm56960_a0_key_gen_mask_init(int unit,
                                 bcmfp_group_t *fg,
                                 uint8_t part,
                                 bcmdrd_sid_t kgp_mask_sid,
                                 uint32_t *kgp_mask_entry)
{
    uint32_t offset = 0;
    uint32_t *prof_entry = NULL;
    bcmfp_group_qual_offset_info_t *group_qual = NULL;
    uint16_t qual_idx = 0;
    uint32_t entry_buffer[5] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);


    prof_entry = kgp_mask_entry;
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    group_qual = &(fg->qual_arr[0][part]);
    for (qual_idx = 0; qual_idx < group_qual->size; qual_idx++) {
         for (offset = 0; offset < BCMFP_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
             if (group_qual->offset_arr[qual_idx].width[offset] > 0) {
                  SHR_BITSET_RANGE(entry_buffer,
                       group_qual->offset_arr[qual_idx].offset[offset],
                       group_qual->offset_arr[qual_idx].width[offset]);

             }
         }
    }
    bcmdrd_pt_field_set(unit, kgp_mask_sid, prof_entry,
                            SEARCH_MASKf, entry_buffer);
    exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_em_group_install(int unit,
                                   uint32_t trans_id,
                                   bcmfp_group_t *fg)
{
    int index = 0;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint32_t *prof_entry = NULL;
    uint32_t *kgp_ref_count = NULL;
    bcmdrd_sid_t kgp_sid = em_profile_sids[0];
    bcmdrd_sid_t kgp2_sid = em_profile_sids[1];
    uint32_t *profile_data[] = { kgp_entry[unit], kgp2_entry[unit] };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_group_parts_count(unit, fg->flags, &num_parts));

    for (part = 0; part < num_parts; part++) {
        prof_entry = kgp_entry[unit];
        bcmfp_bcm56960_a0_ext_codes_init(unit, fg, part, kgp_sid, prof_entry);

        prof_entry = kgp2_entry[unit];

        sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

        bcmfp_bcm56960_a0_key_gen_mask_init(unit, fg,
                                            part,
                                            kgp2_sid,
                                            prof_entry);

        index = -1;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_prof_ref_count_get(unit,
                                  fg->stage_id, fg->pipe_id, &kgp_ref_count));
        SHR_IF_ERR_EXIT(
            bcmfp_profile_add(unit,
                              trans_id,
                              fg->pipe_id,
                              fg->ltid,
                              2,
                              em_profile_sids,
                              profile_data,
                              kgp_ref_count,
                              &index));
        fg->ext_codes[part].keygen_index = (uint32_t)index;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_em_group_uninstall(int unit, uint32_t trans_id,
                                          bcmfp_group_id_t group_id)
{
    int index = -1;
    int tbl_inst = -1;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint32_t *kgp_ref_count = NULL;
    bcmltd_sid_t req_ltid = FP_EM_ENTRYt;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));

    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit,
                                group_oper_info->flags,
                                &num_parts));

    tbl_inst = group_oper_info->tbl_inst;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_prof_ref_count_get(unit,
                             stage_id, tbl_inst, &kgp_ref_count));
    for (part = 0; part < num_parts; part++) {
        index = (int)group_oper_info->ext_codes[part].keygen_index;
        SHR_IF_ERR_EXIT(
            bcmfp_profile_delete(unit,
                                 trans_id,
                                 tbl_inst,
                                 req_ltid,
                                 2,
                                 em_profile_sids,
                                 kgp_ref_count,
                                 index));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_em_action_profile_install(int unit,
                                            uint32_t trans_id,
                                            bcmfp_group_t *fg)
{
    int index = 0;
    uint32_t value = 0;
    uint32_t *prof_entry = NULL;
    uint32_t *action_prof_ref_count = NULL;
    bcmdrd_fid_t fid;
    uint32_t *profile_data[] = { ap_entry[unit] };
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    prof_entry = ap_entry[unit];
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    value = 1;
    fid = COPY_TO_CPU_SET_ENABLEf;
    bcmdrd_pt_field_set(unit, em_action_profile_sid[0], prof_entry, fid, &value);

    index = -1;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_action_prof_ref_count_get(unit,
                                        fg->stage_id,
                                        fg->pipe_id,
                                        &action_prof_ref_count));
    SHR_IF_ERR_EXIT(
        bcmfp_profile_add(unit,
                          trans_id,
                          fg->pipe_id,
                          fg->ltid,
                          1,
                          em_action_profile_sid,
                          profile_data,
                          action_prof_ref_count,
                          &index));

    if (fg->flags & BCMFP_GROUP_WITH_DEFAULT_POLICY) {
        SHR_IF_ERR_EXIT(
            bcmfp_group_oper_info_get(unit,
                                  fg->stage_id,
                                  fg->group_id,
                                  &group_oper_info));
        group_oper_info->ext_codes[0].default_action_prof_idx = (uint32_t)index;
    } else {
        fg->ext_codes[0].action_profile_idx = (uint32_t)index;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_em_action_profile_uninstall(int unit, uint32_t trans_id,
                                          bcmfp_group_id_t group_id)
{
    int index = -1;
    uint32_t *action_prof_ref_count = NULL;
    bcmltd_sid_t req_ltid = FP_EM_ENTRYt;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));

    if (group_oper_info->flags & BCMFP_GROUP_WITH_DEFAULT_POLICY) {
        index = (int)group_oper_info->ext_codes[0].default_action_prof_idx;
    } else {
        index = (int)group_oper_info->ext_codes[0].action_profile_idx;
    }

    if (-1 != index) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_action_prof_ref_count_get(unit,
                                            stage_id,
                                            group_oper_info->tbl_inst,
                                            &action_prof_ref_count));
        SHR_IF_ERR_EXIT(
            bcmfp_profile_delete(unit,
                             trans_id,
                             group_oper_info->tbl_inst,
                             req_ltid,
                             1,
                             em_action_profile_sid,
                             action_prof_ref_count,
                             index));
    }
exit:
    SHR_FUNC_EXIT();
}
int
bcmfp_bcm56960_a0_entry_qos_profile_install(int unit,
                                            uint32_t trans_id,
                                            uint32_t stage_id,
                                            bcmltd_sid_t ltid,
                                            int pipe_id,
                                            uint32_t flag,
                                            bcmfp_group_id_t group_id,
                                            uint32_t *prof_1_data,
                                            uint32_t *prof_2_data,
                                            int *qos_prof_idx)
{
    uint32_t *prof_entry = NULL;
    uint32_t *qos_prof_ref_count = NULL;
    bcmdrd_fid_t fid;
    uint32_t *profile_data[] = { ap_entry[unit] };
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    prof_entry = ap_entry[unit];
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    fid = PROFILE_SET_1f;
    bcmdrd_pt_field_set(unit, em_action_profile_sid[0],
                        prof_entry, fid, prof_1_data);

    fid = PROFILE_SET_2f;
    bcmdrd_pt_field_set(unit, em_action_profile_sid[0],
                        prof_entry, fid, prof_2_data);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_qos_prof_ref_count_get(unit, stage_id, pipe_id,
                                     &qos_prof_ref_count));
    SHR_IF_ERR_EXIT(
        bcmfp_profile_add(unit,
                          trans_id,
                          pipe_id,
                          ltid,
                          1,
                          em_qos_profile_sid,
                          profile_data,
                          qos_prof_ref_count,
                          qos_prof_idx));
    if (flag & BCMFP_GROUP_WITH_DEFAULT_POLICY) {
        SHR_IF_ERR_EXIT(
            bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));
        group_oper_info->ext_codes[0].default_qos_prof_idx =
                                      (uint32_t)*qos_prof_idx;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_entry_qos_profile_uninstall(int unit, uint32_t trans_id,
                                              bcmfp_group_id_t group_id,
                                              int qos_prof_idx)
{
    uint32_t *qos_prof_ref_count = NULL;
    bcmltd_sid_t req_ltid = FP_EM_ENTRYt;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_qos_prof_ref_count_get(unit, stage_id, group_oper_info->tbl_inst,
                                   &qos_prof_ref_count));
    SHR_IF_ERR_EXIT(
        bcmfp_profile_delete(unit,
                             trans_id,
                             group_oper_info->tbl_inst,
                             req_ltid,
                             1,
                             em_qos_profile_sid,
                             qos_prof_ref_count,
                             qos_prof_idx));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_entry_qos_profile_id_get(int unit, uint32_t trans_id,
                                              bcmfp_group_id_t group_id,
                                              uint32_t **edw,
                                              int *qos_prof_idx)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    uint32_t *prof_entry = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmdrd_sid_t em_mem_sid;
    uint32_t value = 0;
    bcmfp_group_oper_info_t *fg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &fg));
    prof_entry = ap_entry[unit];
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    /* update prof_entry */
    sal_memcpy(prof_entry, edw[0], BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, BCMFP_STAGE_ID_EXACT_MATCH, &stage));

    em_mem_sid = em_entry_sids[0];

    if (fg->group_mode == BCMFP_GROUP_MODE_DBLINTER) {
        /* set the em mem as wide mem */
        em_mem_sid = em_entry_sids[1];
    }
    bcmdrd_pt_field_get(unit, em_mem_sid, prof_entry,
                    stage->key_data_size_info->qos_prof_id_fid[fg->group_mode],
                    &value);
    *qos_prof_idx = value;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_custom_entry_set(int unit,
                               uint32_t trans_id,
                               bcmfp_group_id_t group_id,
                               bool only_ekw,
                               int qos_prof_idx,
                               int em_class_id,
                               uint32_t **ekw,
                               uint32_t **edw)
{
    uint32_t *prof_entry = NULL;
    uint8_t num_parts = 0;
    bcmfp_stage_t *stage = NULL;
    bcmdrd_sid_t em_mem_sid;
    uint32_t entry_buffer[BCMFP_ENTRY_WORDS_MAX];
    uint32_t bit, start_bit;
    uint32_t value = 0;
    bcmfp_group_oper_info_t *fg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  BCMFP_STAGE_ID_EXACT_MATCH,
                                  group_id,
                                  &fg));
    prof_entry = ap_entry[unit];
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(entry_buffer, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, BCMFP_STAGE_ID_EXACT_MATCH, &stage));
    SHR_IF_ERR_EXIT(bcmfp_group_parts_count(unit, fg->flags, &num_parts));
    em_mem_sid = em_entry_sids[0];
    value = 1;
    if (fg->group_mode == BCMFP_GROUP_MODE_DBLINTER) {
        /* set the em mem as wide mem */
        em_mem_sid = em_entry_sids[1];
        /* set the valid and key type fields */
        if (only_ekw == FALSE) {
            bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry, VALID_2f, &value);
            bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry, VALID_3f, &value);
        }
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
          KEY_TYPE_2f, &stage->key_data_size_info->em_mode[fg->group_mode]);
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
          KEY_TYPE_3f, &stage->key_data_size_info->em_mode[fg->group_mode]);
    }

    /* set the valid and key type fields */
    if (only_ekw == FALSE) {
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry, VALID_0f, &value);
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry, VALID_1f, &value);
    }
    bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
          KEY_TYPE_0f, &stage->key_data_size_info->em_mode[fg->group_mode]);
    bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
          KEY_TYPE_1f, &stage->key_data_size_info->em_mode[fg->group_mode]);
    /* Build ekw */
    /* update key field */
    if (fg->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
        /* 128 bit mode */
        for(start_bit = 0,
            bit = 0;
            bit < 48;
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }
        for(start_bit = 80;
            bit < stage->key_data_size_info->key_part_size[fg->group_mode][0];
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }

        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
                    stage->key_data_size_info->key_parts_fid[fg->group_mode][0],
                    entry_buffer);
        sal_memset(entry_buffer, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
        for(bit = 0;
            bit < stage->key_data_size_info->key_part_size[fg->group_mode][1];
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
                    stage->key_data_size_info->key_parts_fid[fg->group_mode][1],
                    entry_buffer);
    } else if (fg->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {
        /* 160 bit mode */
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
            stage->key_data_size_info->key_parts_fid[fg->group_mode][0],
            ekw[0]);
        for(start_bit = stage->key_data_size_info->key_part_size[fg->group_mode][0],
            bit = 0;
            bit < stage->key_data_size_info->key_part_size[fg->group_mode][1];
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
            stage->key_data_size_info->key_parts_fid[fg->group_mode][1],
            entry_buffer);
    } else {
        /* 320 bit mode */
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
            stage->key_data_size_info->key_parts_fid[fg->group_mode][0],
            ekw[0]);
        for(start_bit = stage->key_data_size_info->key_part_size[fg->group_mode][0],
            bit = 0; start_bit < 160; start_bit++,bit++) {
             if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
             }
        }
        for(start_bit = 0;
            bit < stage->key_data_size_info->key_part_size[fg->group_mode][1];
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[1], start_bit)) {
                SHR_BITSET(entry_buffer, bit);
            }
        }
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
            stage->key_data_size_info->key_parts_fid[fg->group_mode][1],
            entry_buffer);
        sal_memset(entry_buffer, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
        for(bit = 0;
            bit < stage->key_data_size_info->key_part_size[fg->group_mode][2];
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[1], start_bit)) {
                SHR_BITSET(entry_buffer, bit);
            }
        }
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
            stage->key_data_size_info->key_parts_fid[fg->group_mode][2],
            entry_buffer);
        sal_memset(entry_buffer, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
        for(bit = 0;
            bit < stage->key_data_size_info->key_part_size[fg->group_mode][3];
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[1], start_bit)) {
                SHR_BITSET(entry_buffer, bit);
            }
        }
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
            stage->key_data_size_info->key_parts_fid[fg->group_mode][3],
            entry_buffer);
    }
    /* clear EKW */
    sal_memset(ekw[0], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    sal_memset(ekw[1], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    /* Update EKW */
    sal_memcpy(ekw[0], prof_entry, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));

    if (only_ekw == FALSE) {
        /* Build EDW */
        sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
        bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
              stage->key_data_size_info->policy_data_fid[fg->group_mode],
              edw[0]);
        /* Action profile ID */
        if (-1 != fg->ext_codes[0].action_profile_idx) {
            value = fg->ext_codes[0].action_profile_idx;
            bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
                  stage->key_data_size_info->action_prof_id_fid[fg->group_mode],
                  &value);
        }
        /* QOS profile ID */
        if (-1 != qos_prof_idx) {
            value = qos_prof_idx;
            bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
               stage->key_data_size_info->qos_prof_id_fid[fg->group_mode],
               &value);
        }
        /* EM class id */
        if (-1 != em_class_id) {
            value = em_class_id;
            bcmdrd_pt_field_set(unit, em_mem_sid, prof_entry,
              stage->key_data_size_info->em_class_id_fid[fg->group_mode],
              &value);
        }
        /* Clear EDW */
        sal_memset(edw[0], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
        /* update EDW */
        sal_memcpy(edw[0], prof_entry, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
   }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_default_policy_install(int unit,
                               uint32_t trans_id,
                               bcmfp_group_t *fg,
                               int qos_prof_idx,
                               int em_class_id,
                               uint32_t **edw)
{
    uint32_t value = 0;
    uint32_t *prof_entry = NULL;
    bcmdrd_fid_t fid;
    bcmfp_stage_t *stage = NULL;
    uint32_t *profile_data[] = { ap_entry[unit] };
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit,
                                    fg->stage_id,
                                    &stage));

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  fg->stage_id,
                                  fg->group_id,
                                  &group_oper_info));
    prof_entry = ap_entry[unit];
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    fid = ACTION_DATAf;
    bcmdrd_pt_field_set(unit, em_default_policy_sid[0], prof_entry, fid, edw[0]);

    value = group_oper_info->ext_codes[0].default_action_prof_idx;
    fid = ACTION_PROFILE_IDf;
    bcmdrd_pt_field_set(unit, em_default_policy_sid[0], prof_entry, fid, &value);

    value = qos_prof_idx;
    fid = QOS_PROFILE_IDf;
    bcmdrd_pt_field_set(unit, em_default_policy_sid[0], prof_entry, fid, &value);

    /* EM class id */
    if (-1 != em_class_id) {
        value = em_class_id;
        fid = EXACT_MATCH_CLASS_IDf;
        bcmdrd_pt_field_set(unit, em_default_policy_sid[0], prof_entry, fid, &value);
    }

    SHR_IF_ERR_EXIT(
         bcmfp_profile_entry_insert(unit,
                                    trans_id,
                                    fg->pipe_id,
                                    fg->ltid,
                                    em_default_policy_sid[0],
                                    profile_data[0],
                                    fg->ext_codes[0].keygen_index));

    group_oper_info->ext_codes[0].default_policy_enable = 1;
    /* Clear EDW */
    sal_memset(edw[0], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_default_policy_uninstall(int unit, uint32_t trans_id,
                                              bcmfp_group_id_t group_id,
                                              int default_policy_idx)
{
    bcmltd_sid_t req_ltid = FP_EM_ENTRYt;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_stage_oper_info_get(unit,
                                  stage_id,
                                  &stage_oper_info));
    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));
    SHR_IF_ERR_EXIT(
            bcmfp_profile_entry_delete(unit,
                                       trans_id,
                                       group_oper_info->tbl_inst,
                                       req_ltid,
                                       em_default_policy_sid[0],
                                       default_policy_idx));
exit:
    SHR_FUNC_EXIT();
}
