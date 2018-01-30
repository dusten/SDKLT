/*! \file bcmfp_presel_action_cfg.c
 *
 * Utility APIs to operate with FP preselection actions.
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/* Private Function Declarations. */
static int
presel_action_offset_get(int unit,
                         bcmfp_stage_t *stage,
                         bcmfp_action_t action,
                         bcmfp_action_offset_cfg_t *a_cfg_ptr);
static int
presel_action_value_get(int unit,
                        uint8_t part,
                        uint8_t num_parts,
                        bcmfp_action_t action,
                        bcmfp_ext_sel_t *ext_codes,
                        int *value);
static int
presel_action_init(int unit,
                   uint8_t num_parts,
                   bcmfp_stage_t *stage,
                   bcmfp_action_t action,
                   bcmfp_ext_sel_t *ext_codes,
                   uint32_t **edw);

#define PRESEL_ACTION_OFFSET_GET(_unit_,             \
                                 _stage_,            \
                                 _action_,           \
                                 _a_offset_)         \
    do {                                             \
        SHR_IF_ERR_EXIT(                             \
            presel_action_offset_get((_unit_),       \
                                     (_stage_),      \
                                     (_action_),     \
                                     (_a_offset_))); \
    } while (0)

static bcmfp_action_t presel_actions[] = {
                           BCMFP_ACTION_AUX_TAG_A_SEL,
                           BCMFP_ACTION_AUX_TAG_B_SEL,
                           BCMFP_ACTION_AUX_TAG_C_SEL,
                           BCMFP_ACTION_AUX_TAG_D_SEL,
                           BCMFP_ACTION_SRC_DST_CONTAINER_1_SEL,
                           BCMFP_ACTION_SRC_DST_CONTAINER_0_SEL,
                           BCMFP_ACTION_SRC_CONTAINER_A_SEL,
                           BCMFP_ACTION_SRC_CONTAINER_B_SEL,
                           BCMFP_ACTION_CLASS_ID_CONTAINER_D_SEL,
                           BCMFP_ACTION_CLASS_ID_CONTAINER_C_SEL,
                           BCMFP_ACTION_CLASS_ID_CONTAINER_B_SEL,
                           BCMFP_ACTION_CLASS_ID_CONTAINER_A_SEL,
                           BCMFP_ACTION_TCP_FN_SEL,
                           BCMFP_ACTION_TOS_FN_SEL,
                           BCMFP_ACTION_TTL_FN_SEL,
                           BCMFP_ACTION_NORMALIZE_L2,
                           BCMFP_ACTION_NORMALIZE_L3_L4,
                           BCMFP_ACTION_MULTIWIDE_MODE,
                           BCMFP_ACTION_IPBM_SOURCE,
                           BCMFP_ACTION_IPBM_PRESENT,
                           BCMFP_ACTION_LOGICAL_TABLE_ID,
                           BCMFP_ACTION_KEY_GEN_PROGRAM_PROFILE_INDEX,
                           BCMFP_ACTION_EM_KEY_TYPE,
                           BCMFP_ACTION_DEFAULT_POLICY_ENABLE,
                           BCMFP_ACTION_PRESEL_ENABLE};

/* Private Function Definitions */
static int
presel_action_offset_get(int unit,
                         bcmfp_stage_t *stage,
                         bcmfp_action_t action,
                         bcmfp_action_offset_cfg_t *a_cfg_ptr)
{
    bcmfp_action_offset_cfg_t *a_cfg_ptr2 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(a_cfg_ptr, SHR_E_PARAM);

    SHR_IF_ERR_EXIT((action < 0) ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_EXIT((action >= BCMFP_ACTION_COUNT) ? SHR_E_PARAM : SHR_E_NONE);

    a_cfg_ptr2 = &(stage->presel_action_cfg_db->action_cfg[action]->offset_cfg);
    sal_memset(a_cfg_ptr, 0, sizeof(bcmfp_action_offset_cfg_t));
    sal_memcpy(a_cfg_ptr, a_cfg_ptr2, sizeof(bcmfp_action_offset_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

static int
presel_action_value_get(int unit,
                        uint8_t part,
                        uint8_t num_parts,
                        bcmfp_action_t action,
                        bcmfp_ext_sel_t *ext_codes,
                        int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    *value = -1;
    switch (action) {
        case BCMFP_ACTION_AUX_TAG_A_SEL:
            *value = ext_codes[part].aux_tag_a_sel;
            break;
        case BCMFP_ACTION_AUX_TAG_B_SEL:
            *value = ext_codes[part].aux_tag_b_sel;
            break;
        case BCMFP_ACTION_AUX_TAG_C_SEL:
            *value = ext_codes[part].aux_tag_c_sel;
            break;
        case BCMFP_ACTION_AUX_TAG_D_SEL:
            *value = ext_codes[part].aux_tag_d_sel;
            break;
        case BCMFP_ACTION_SRC_DST_CONTAINER_0_SEL:
            *value = ext_codes[part].src_dest_cont_0_sel;
            break;
        case BCMFP_ACTION_SRC_DST_CONTAINER_1_SEL:
            *value = ext_codes[part].src_dest_cont_1_sel;
            break;
        case BCMFP_ACTION_SRC_CONTAINER_A_SEL:
            *value = ext_codes[part].src_cont_a_sel;
            break;
        case BCMFP_ACTION_SRC_CONTAINER_B_SEL:
            *value = ext_codes[part].src_cont_b_sel;
            break;
        case BCMFP_ACTION_CLASS_ID_CONTAINER_A_SEL:
            *value = ext_codes[part].class_id_cont_a_sel;
            break;
        case BCMFP_ACTION_CLASS_ID_CONTAINER_B_SEL:
            *value = ext_codes[part].class_id_cont_b_sel;
            break;
        case BCMFP_ACTION_CLASS_ID_CONTAINER_C_SEL:
            *value = ext_codes[part].class_id_cont_c_sel;
            break;
        case BCMFP_ACTION_CLASS_ID_CONTAINER_D_SEL:
            *value = ext_codes[part].class_id_cont_d_sel;
            break;
        case BCMFP_ACTION_TCP_FN_SEL:
            *value = ext_codes[part].tcp_fn_sel;
            break;
        case BCMFP_ACTION_TOS_FN_SEL:
            *value = ext_codes[part].tos_fn_sel;
            break;
        case BCMFP_ACTION_TTL_FN_SEL:
            *value = ext_codes[part].ttl_fn_sel;
            break;
        case BCMFP_ACTION_NORMALIZE_L2:
            *value = ext_codes[part].normalize_mac_sel;
            break;
        case BCMFP_ACTION_NORMALIZE_L3_L4:
            *value = ext_codes[part].normalize_l3_l4_sel;
            break;
        case BCMFP_ACTION_IPBM_PRESENT:
            *value = ext_codes[part].ipbm_present;
            break;
        case BCMFP_ACTION_IPBM_SOURCE:
            *value = ext_codes[part].ipbm_source;
            break;
        case BCMFP_ACTION_KEY_GEN_PROGRAM_PROFILE_INDEX:
            *value = ext_codes[part].keygen_index;
            break;
        case BCMFP_ACTION_PRESEL_ENABLE:
            *value = 1;
            break;
        case BCMFP_ACTION_LOGICAL_TABLE_ID:
            *value = ext_codes[part].ltid;
            break;
        case BCMFP_ACTION_EM_KEY_TYPE:
            *value = ext_codes[0].key_type;
            break;
        case BCMFP_ACTION_MULTIWIDE_MODE:
            if (num_parts == 1) {
                *value = 0;
            } else if (num_parts == 2) {
                *value = (part == 0) ? 1 : 2;
            } else if (num_parts == 3) {
                *value = (part == 0) ? 1 : ((part == 1) ? 3 : 4);
            } else {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            break;
        case BCMFP_ACTION_DEFAULT_POLICY_ENABLE:
            *value = ext_codes[0].default_policy_enable;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
presel_action_init(int unit,
                   uint8_t num_parts,
                   bcmfp_stage_t *stage,
                   bcmfp_action_t action,
                   bcmfp_ext_sel_t *ext_codes,
                   uint32_t **edw)
{
    int value = -1;
    uint8_t part = 0;
    bcmfp_action_offset_cfg_t a_offset;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    for (part = 0; part < num_parts; part++) {

        SHR_IF_ERR_EXIT(
            presel_action_value_get(unit,
                                    part,
                                    num_parts,
                                    action,
                                    ext_codes,
                                    &value));

        if (value  == -1) {
            continue;
        }

        sal_memset(&a_offset, 0, sizeof(bcmfp_action_offset_cfg_t));
        PRESEL_ACTION_OFFSET_GET(unit, stage, action, &a_offset);
        a_offset.value[0] = value;
        BCMFP_ACTION_SET(unit, edw[part], &a_offset);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_cfg_t_init(int unit,
                               bcmfp_action_cfg_t *presel_action_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(presel_action_cfg, SHR_E_PARAM);

    sal_memset(presel_action_cfg, 0, sizeof(bcmfp_action_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *new_action_cfg)
{
    bcmfp_action_cfg_t *presel_action_cfg = NULL;
    bcmfp_action_cfg_db_t *presel_action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_action_cfg, SHR_E_PARAM);

    presel_action_cfg_db = stage->presel_action_cfg_db;

    if (presel_action_cfg_db == NULL) {
        BCMFP_ALLOC(presel_action_cfg_db,
                    sizeof(bcmfp_action_cfg_db_t),
                    "Action Config DB");
        stage->presel_action_cfg_db = presel_action_cfg_db;
    }

    BCMFP_ALLOC(presel_action_cfg,
                sizeof(bcmfp_action_cfg_t),
                "BCMFP:Action config");

    sal_memcpy(presel_action_cfg, new_action_cfg, sizeof(bcmfp_action_cfg_t));

    if (presel_action_cfg_db->action_cfg[action] == NULL) {
        presel_action_cfg_db->num_actions++;
        presel_action_cfg_db->action_cfg[action] = presel_action_cfg;
    } else {
        presel_action_cfg->next = presel_action_cfg_db->action_cfg[action];
        presel_action_cfg_db->action_cfg[action] = presel_action_cfg;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_cfg_db_cleanup(int unit,
                            bcmfp_stage_t *stage)
{
    bcmfp_action_t action = 0;
    bcmfp_action_cfg_t *presel_action_cfg = NULL;
    bcmfp_action_cfg_db_t *presel_action_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    presel_action_cfg_db = stage->presel_action_cfg_db;

    if (presel_action_cfg_db != NULL) {

        for (action = 0; action < BCMFP_ACTION_COUNT; action++) {

            while (presel_action_cfg_db->action_cfg[action] != NULL) {
                presel_action_cfg = presel_action_cfg_db->action_cfg[action];
                presel_action_cfg_db->action_cfg[action] = presel_action_cfg->next;
                SHR_FREE(presel_action_cfg);
            }

            presel_action_cfg_db->action_cfg[action] = NULL;
        }

        SHR_FREE(presel_action_cfg_db);
    }

    stage->presel_action_cfg_db = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t group_id,
                 uint32_t action,
                 uint32_t param,
                 uint32_t **edw)
{
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    PRESEL_ACTION_OFFSET_GET(unit, stage, action, &a_offset);
    if(-1 == a_offset.value[0])
    {
        a_offset.value[0] = param;
    }
    BCMFP_ACTION_SET(unit, edw[0], &a_offset);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_presel_init(int unit,
                         uint32_t stage_id,
                         uint32_t group_id,
                         uint32_t entry_id,
                         uint32_t **edw,
                         uint8_t  flag)
{
    uint8_t idx = 0;
    uint8_t num_parts = 0;
    uint8_t num_actions = 0;
    bcmfp_action_t action;
    bcmfp_stage_t *stage = NULL;
    bcmfp_ext_sel_t *ext_codes = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));

    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit,
                                group_oper_info->flags,
                                &num_parts));

    ext_codes = group_oper_info->ext_codes;

    num_actions = sizeof(presel_actions) / sizeof(bcmfp_action_t);

    if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
        if (group_oper_info->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {
            ext_codes[0].key_type = 1;
        } else if (group_oper_info->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
            ext_codes[0].key_type = 0;
        } else if (group_oper_info->group_mode == BCMFP_GROUP_MODE_DBLINTER) {
            ext_codes[0].key_type = 2;
        }
    }

    for (idx = 0; idx < num_actions; idx++) {
        action = presel_actions[idx];
        if (stage->presel_action_cfg_db->action_cfg[action] != NULL) {
            /*
             * For non-default presel entries, don't set default
             * action value for L2 & L3_L4 normalize.
             */
            if (!flag &&
                    ((action == BCMFP_ACTION_NORMALIZE_L2) ||
                     (action == BCMFP_ACTION_NORMALIZE_L3_L4))) {
                continue;
            }
            /* for default presel entry dont set default policy enable
             * even if default policy is created.
             */
            if (flag &&
                   (action == BCMFP_ACTION_DEFAULT_POLICY_ENABLE)) {
                continue;
            }
            SHR_IF_ERR_EXIT(
                presel_action_init(unit,
                                   num_parts,
                                   stage,
                                   action,
                                   ext_codes,
                                   edw));
        }
    }

exit:
    SHR_FUNC_EXIT();
}
