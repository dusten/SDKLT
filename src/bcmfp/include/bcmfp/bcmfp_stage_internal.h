/*! \file bcmfp_stage_internal.h
 *
 * Defines, Enums and Structures to represent FP stages(IFP/EFP/VFP/EM)
 * related information.
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

#ifndef BCMFP_STAGE_INTERNAL_H
#define BCMFP_STAGE_INTERNAL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_qual_cfg_internal.h>
#include <bcmfp/bcmfp_presel_qual_cfg_internal.h>
#include <bcmfp/bcmfp_action_cfg_internal.h>
#include <bcmfp/bcmfp_presel_action_cfg_internal.h>
#include <bcmfp/bcmfp_ext_cfg_internal.h>
#include <bcmfp/bcmfp_keygen_api.h>

#define BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE (1 << 0)

#define BCMFP_STAGE_KEY_TYPE_PRESEL          (1 << 1)

#define BCMFP_STAGE_KEY_TYPE_SELCODE         (1 << 2)

#define BCMFP_STAGE_KEY_TYPE_FIXED           (1 << 3)

#define BCMFP_STAGE_POLICY_TYPE_PDD          (1 << 4)

#define BCMFP_STAGE_KEYGEN_PROFILE           (1 << 5)

#define BCMFP_STAGE_ACTION_PROFILE           (1 << 6)

#define BCMFP_STAGE_ACTION_QOS_PROFILE       (1 << 7)

#define BCMFP_STAGE_ENTRY_TYPE_HASH          (1 << 8)

#define BCMFP_STAGE_DEFAULT_POLICY           (1 << 9)

#define BCMFP_STAGE_MODE_HALF_NOT_SUPPORTED  (1 << 10)

#define BCMFP_STAGE_INPORTS_SUPPORT          (1 << 11)


#define BCMFP_STAGE_TH3                      (1 << 12)

#define BCMFP_GROUP_MAX_IFP                  (128)

typedef struct bcmfp_stage_s bcmfp_stage_t;

typedef int(*bcmfp_fn_qual_cfg_db_init_t)(int unit,
                                          bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_presel_qual_cfg_db_init_t)(int unit,
                                          bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_action_cfg_db_init_t)(int unit,
                                            bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_action_conflict_check_t)(int unit,
                                             bcmfp_action_t action1,
                                             bcmfp_action_t action2);
typedef int(*bcmfp_fn_presel_action_cfg_db_init_t)(int unit,
                                            bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_ext_cfg_db_init_t)(int unit,
                                         bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_stage_init_t)(int unit,
                                    bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_qualifier_enum_set_t)(int unit,
                                            bcmfp_qual_t qaul,
                                            int value,
                                            uint32_t *data,
                                            uint32_t *mask);
typedef int(*bcmfp_fn_src_class_mode_set_t)(int unit,
                                            uint32_t trans_id,
                                            bcmltd_sid_t sid,
                                            int pipe_id,
                                            uint32_t mode);

typedef int(*bcmfp_fn_qualifier_enum_get_t)(int unit,
                                            bcmfp_qual_t qaul,
                                            uint32_t data,
                                            uint32_t mask,
                                            int *value);
typedef int(*bcmfp_fn_group_install_t)(int unit, uint32_t trans_id,
                                       bcmfp_group_t *fg);
typedef int(*bcmfp_fn_group_uninstall_t)(int unit, uint32_t trans_id,
                                         bcmfp_group_id_t group_id);
typedef int(*bcmfp_fn_udf_qual_id_get_t)(int unit,
                                         uint8_t udf_chunk_id,
                                         bcmfp_qual_t *qual_id);
typedef int(*bcmfp_fn_group_selcode_key_get_t)(int unit,
                                               bcmfp_group_t *fg);
typedef int(*bcmfp_fn_entry_key_match_type_set_t)(int unit,
                                  bcmfp_stage_t *stage,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_group_slice_mode_t slice_mode,
                                  bcmfp_group_type_t  port_pkt_type,
                                  uint32_t **ekw);
typedef int(*bcmfp_fn_group_action_profile_install_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_t *fg);
typedef int(*bcmfp_fn_group_action_profile_uninstall_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t group_id);
typedef int(*bcmfp_fn_entry_qos_profile_install_t)(int unit,
                                       uint32_t trans_id,
                                       uint32_t stage_id,
                                       bcmltd_sid_t ltid,
                                       int pipe_id,
                                       uint32_t flag,
                                       bcmfp_group_id_t group_id,
                                       uint32_t *prof_1_data,
                                       uint32_t *prof_2_data,
                                       int *qos_prof_idx);
typedef int(*bcmfp_fn_entry_qos_profile_uninstall_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t group_id,
                                       int qos_prof_idx);
typedef int(*bcmfp_fn_entry_qos_profile_id_get_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t group_id,
                                       uint32_t **edw,
                                       int *qos_prof_idx);
typedef int(*bcmfp_fn_custom_entry_set_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t gid,
                                       bool only_ekw,
                                       int qos_prof_idx,
                                       int em_class_id,
                                       uint32_t **ekw,
                                       uint32_t **edw);
typedef int(*bcmfp_fn_default_policy_install_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_t *fg,
                                       int qos_prof_idx,
                                       int em_class_id,
                                       uint32_t **edw);
typedef int(*bcmfp_fn_default_policy_uninstall_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t group_id,
                                       int default_policy_idx);

/* Function Pointers */
typedef struct bcmfp_fn_ptrs_s {
    bcmfp_fn_qual_cfg_db_init_t           bcmfp_fn_qual_cfg_db_init;
    bcmfp_fn_presel_qual_cfg_db_init_t    bcmfp_fn_presel_qual_cfg_db_init;
    bcmfp_fn_action_cfg_db_init_t         bcmfp_fn_action_cfg_db_init;
    bcmfp_fn_action_conflict_check_t      bcmfp_fn_action_conflict_check;
    bcmfp_fn_presel_action_cfg_db_init_t  bcmfp_fn_presel_action_cfg_db_init;
    bcmfp_fn_ext_cfg_db_init_t            bcmfp_fn_ext_cfg_db_init;
    bcmfp_fn_stage_init_t                 bcmfp_fn_stage_init;
    bcmfp_fn_qualifier_enum_set_t         bcmfp_fn_qualifier_enum_set;
    bcmfp_fn_src_class_mode_set_t         bcmfp_fn_src_class_mode_set;
    bcmfp_fn_qualifier_enum_get_t         bcmfp_fn_qualifier_enum_get;
    bcmfp_fn_group_install_t              bcmfp_fn_group_install;
    bcmfp_fn_group_uninstall_t            bcmfp_fn_group_uninstall;
    bcmfp_fn_udf_qual_id_get_t            bcmfp_fn_udf_qual_id_get;
    bcmfp_fn_group_selcode_key_get_t      bcmfp_fn_group_selcode_key_get;
    bcmfp_fn_entry_key_match_type_set_t   bcmfp_fn_entry_key_match_type_set;
    bcmfp_fn_group_action_profile_install_t     bcmfp_fn_group_action_profile_install;
    bcmfp_fn_group_action_profile_uninstall_t   bcmfp_fn_group_action_profile_uninstall;
    bcmfp_fn_entry_qos_profile_install_t        bcmfp_fn_entry_qos_profile_install;
    bcmfp_fn_entry_qos_profile_uninstall_t      bcmfp_fn_entry_qos_profile_uninstall;
    bcmfp_fn_entry_qos_profile_id_get_t         bcmfp_fn_entry_qos_profile_id_get;
    bcmfp_fn_custom_entry_set_t                 bcmfp_fn_custom_entry_set;
    bcmfp_fn_default_policy_install_t           bcmfp_fn_default_policy_install;
    bcmfp_fn_default_policy_uninstall_t         bcmfp_fn_default_policy_uninstall;
} bcmfp_fn_ptrs_t;

typedef enum bcmfp_stage_id_e {

    BCMFP_STAGE_ID_INGRESS = 0,

    BCMFP_STAGE_ID_EGRESS,

    BCMFP_STAGE_ID_LOOKUP,

    BCMFP_STAGE_ID_EXACT_MATCH,

    BCMFP_STAGE_ID_COUNT

} bcmfp_stage_id_t;

typedef enum bcmfp_stage_oper_mode_e {

    BCMFP_STAGE_OPER_MODE_GLOBAL = 0,

    BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE = 1,

    BCMFP_STAGE_OPER_MODE_GLOBAL_PIPE_AWARE = 2,

    BCMFP_STAGE_OPER_MODE_COUNT

} bcmfp_stage_oper_mode_t;

typedef struct bcmfp_qset_aset_size_info_s {

  uint32_t data_size[BCMFP_GROUP_MODE_COUNT];

  uint32_t key_size[BCMFP_GROUP_MODE_COUNT];

  uint32_t key_part_size[BCMFP_GROUP_MODE_COUNT][4];

  uint32_t em_mode[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t policy_data_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t action_prof_id_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t qos_prof_id_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t em_class_id_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t data_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t key_parts_fid[BCMFP_GROUP_MODE_COUNT][4];

} bcmfp_qset_aset_size_info_t;

typedef struct bcmfp_stage_hw_entry_info_s {

    bool aggr_view;

    bool presel_aggr_view;

    bcmdrd_sid_t sid;

    bcmdrd_sid_t sid_data_only;

    bcmdrd_sid_t presel_sid;

    bcmdrd_sid_t presel_sid_data_only;

    bcmdrd_fid_t key_fid;

    bcmdrd_fid_t mask_fid;

    bcmdrd_fid_t valid_fid;

    uint32_t max_entries;

} bcmfp_stage_hw_entry_info_t;

typedef struct bcmfp_stage_misc_info_s {

    uint8_t num_group_action_res_ids;

    uint16_t num_groups;

    uint16_t num_keygen_prof;

    uint16_t num_action_prof;

    uint16_t num_qos_prof;

    bcmfp_group_mode_bmp_t group_mode_bmp;

} bcmfp_stage_misc_info_t;

typedef struct bcmfp_staged_entr_oper_state_s {
    bcmfp_entry_id_t eid;

    uint32_t  oper_state;

    struct bcmfp_staged_entr_oper_state_s *next;

} bcmfp_staged_entr_oper_state_t;

typedef struct bcmfp_rule_template_info_s{
    bcmfp_entry_id_t eid;

    struct bcmfp_rule_template_info_s *next;

} bcmfp_rule_template_info_t;

typedef struct bcmfp_policy_template_info_s {
    bcmfp_entry_id_t eid;

    struct bcmfp_policy_template_info_s *next;

} bcmfp_policy_template_info_t;

struct bcmfp_stage_s {

    uint32_t flags;

    bcmfp_stage_id_t stage_id;

    uint8_t num_group_action_res_ids;

    bcmfp_group_mode_bmp_t group_mode_bmp;

    bcmfp_keygen_qual_cfg_info_db_t *kgn_qual_cfg_info_db;

    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db;

    bcmfp_qual_cfg_info_db_t *presel_qual_cfg_info_db;

    bcmfp_action_cfg_db_t *action_cfg_db;

    bcmfp_action_cfg_db_t *presel_action_cfg_db;

    bcmfp_ext_cfg_db_t *ext_cfg_db_arr[BCMFP_EXT_CFG_MODE_COUNT];

    bcmfp_fn_ptrs_t fn_ptrs;

    bcmfp_stage_hw_entry_info_t *hw_entry_info[BCMFP_GROUP_MODE_COUNT];

    bcmfp_keygen_ext_cfg_db_t *kgn_ext_cfg_db_arr[BCMFP_GROUP_MODE_COUNT];

    bcmfp_qset_t pmux_qset;

    bcmfp_qset_aset_size_info_t *key_data_size_info;

    bcmfp_stage_misc_info_t *misc_info;

    bcmfp_staged_entr_oper_state_t *staged_entr_state;

    bcmfp_rule_template_info_t **rule_template_info_list;

    bcmfp_policy_template_info_t **policy_template_info_list;
};

#define BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(bcmfp_stage_id,             \
                                          bcmptm_stage_id)            \
    do {                                                              \
        if (bcmfp_stage_id == BCMFP_STAGE_ID_INGRESS) {               \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_INGRESS;     \
        } else if(bcmfp_stage_id == BCMFP_STAGE_ID_EGRESS) {          \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_EGRESS;     \
        } else if(bcmfp_stage_id == BCMFP_STAGE_ID_LOOKUP) {          \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_LOOKUP;     \
        } else if(bcmfp_stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {     \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_EXACT_MATCH; \
        }                                                             \
    } while(0)

#endif /* BCMFP_STAGE_INTERNAL_H */
