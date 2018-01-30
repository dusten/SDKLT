/*! \file bcmfp_lt_entry.c
 *
 * APIs for Ingress FP Entry table handlers
 *
 * This file contains APIs for Ingress FP Entry
 * table handlers.
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
  Includes
 */
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_lt_entry.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmfp/bcmfp_lt_group.h>
#include <bcmfp/bcmfp_lt_rule.h>
#include <bcmfp/bcmfp_lt_policy.h>
#include <bcmfp/bcmfp_lt_presel_entry.h>
#include <bcmfp/map/bcmfp_lt_entry_map.h>
#include <bcmmeter/bcmmeter_fp_ing.h>
#include <bcmmeter/bcmmeter_fp_egr.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmfp/map/bcmfp_lt_group_map.h>
#include <bcmfp/map/bcmfp_lt_rule_map.h>
#include <bcmfp/map/bcmfp_lt_policy_map.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <shr/shr_util.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

static int
bcmfp_lt_group_oper_mode_ltid_update(int unit,
                                     uint32_t trans_id,
                                     bcmfp_lt_entry_config_t *entry_config)
{
    uint32_t mode_fid, ltid_fid, gid;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_group_config_t *group_config = NULL;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmfp_stage_id_t stage_id;
    uint64_t data = 0;
    uint64_t mode_oper_value = 0;
    uint32_t mode_oper = 0;
    bcmltd_sid_t group_ltid = 0;
    bcmltd_fields_t imm_fields = {0};
    uint32_t out_fields = 0;
    uint32_t offset = 0;
    uint16_t add_count = 0;

    SHR_FUNC_ENTER(unit);

    group_config = &(entry_config->group_config);
    stage_id = entry_config->stage_id;
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_GROUP,
                                              &lt_info));
    gid = group_config->group_id;
    group_ltid = lt_info->ltid;
    /* Build the key */
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));
    out_fields = imm_fields.count;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;

    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.count = 1;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_lookup(unit,
                            group_ltid,
                            in,
                            &imm_fields));

    switch (group_config->mode) {
        case BCMFP_GROUP_MODE_SINGLE:
        case BCMFP_GROUP_MODE_SINGLE_ASET_NARROW:
        case BCMFP_GROUP_MODE_SINGLE_ASET_WIDE:
            mode_oper = 1;
            break;
        case BCMFP_GROUP_MODE_DBLINTRA:
            mode_oper = 2;
            break;
        case BCMFP_GROUP_MODE_DBLINTER:
            mode_oper = 3;
            break;
        case BCMFP_GROUP_MODE_TRIPLE:
            mode_oper = 4;
            break;
        case BCMFP_GROUP_MODE_QUAD:
            mode_oper = 5;
            break;
        default:
            mode_oper = 0;
    }
    BCMFP_UINT32_TO_UINT64(mode_oper, mode_oper_value);

    mode_fid = fid_list->mode_oper_fid;
    ltid_fid = fid_list->group_ltid_oper_fid;
    /* if mode_oper field is present then mode_ltd also will exists in IMM */
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, mode_fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        add_count = 1;
    }
    else {
        add_count = 3;
    }
    BCMFP_RET_VAL_ASSIGN(SHR_E_NONE);

    /*Update the in with KEY*/
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.field[1]->id = mode_fid;
    input.field[1]->data = mode_oper_value;
    input.field[2]->id = ltid_fid;
    input.field[2]->data = group_config->group_ltid;
    input.count = imm_fields.count + add_count;
    if (in != NULL) {
        for (offset = 3; offset < input.count; offset++) {
            input.field[offset]->id = imm_fields.field[offset - add_count]->id;
            input.field[offset]->idx = imm_fields.field[offset - add_count]->idx;
            input.field[offset]->data = imm_fields.field[offset - add_count]->data;
        }
    }
    SHR_IF_ERR_EXIT(bcmimm_entry_update(unit,
                                        0,
                                        group_ltid,
                                        in));
 exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    SHR_FUNC_EXIT();
}

static int
bcmfp_lt_group_oper_mode_update(int unit,
                                uint32_t trans_id,
                                bcmfp_lt_entry_config_t *entry_config)
{
    uint32_t fid, gid;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_group_config_t *group_config = NULL;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmfp_stage_id_t stage_id;
    uint64_t data = 0;
    uint64_t mode_oper_value = 0;
    uint32_t mode_oper = 0;
    bcmltd_sid_t group_ltid = 0;
    bcmltd_fields_t imm_fields = {0};
    uint32_t out_fields = 0;
    uint32_t offset = 0;
    uint16_t add_count = 0;

    SHR_FUNC_ENTER(unit);

    group_config = &(entry_config->group_config);
    stage_id = entry_config->stage_id;
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_GROUP,
                                              &lt_info));
    gid = group_config->group_id;
    group_ltid = lt_info->ltid;
    /* Build the key */
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));
    out_fields = imm_fields.count;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;

    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.count = 1;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_lookup(unit,
                            group_ltid,
                            in,
                            &imm_fields));
    
    switch (group_config->mode) {
        case BCMFP_GROUP_MODE_SINGLE:
        case BCMFP_GROUP_MODE_SINGLE_ASET_NARROW:
        case BCMFP_GROUP_MODE_SINGLE_ASET_WIDE:
            mode_oper = 1;
            break;
        case BCMFP_GROUP_MODE_DBLINTRA:
            mode_oper = 2;
            break;
        case BCMFP_GROUP_MODE_DBLINTER:
            mode_oper = 3;
            break;
        case BCMFP_GROUP_MODE_TRIPLE:
            mode_oper = 4;
            break;
         case BCMFP_GROUP_MODE_QUAD:
            mode_oper = 5;
            break;
        default:
            mode_oper = 0;
    }

    BCMFP_UINT32_TO_UINT64(mode_oper, mode_oper_value);

    fid = fid_list->mode_oper_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        add_count = 1;
    }
    else {
        add_count = 2;
    }
    BCMFP_RET_VAL_ASSIGN(SHR_E_NONE);

    /*Update the in with KEY*/
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.field[1]->id = fid;
    input.field[1]->data = mode_oper_value;
    input.count = imm_fields.count + add_count;
    if (in != NULL) {
        for (offset = 2; offset < input.count; offset++) {
            input.field[offset]->id = imm_fields.field[offset - add_count]->id;
            input.field[offset]->idx = imm_fields.field[offset - add_count]->idx;
            input.field[offset]->data = imm_fields.field[offset - add_count]->data;
        }
    }
    SHR_IF_ERR_EXIT(bcmimm_entry_update(unit,
                                        0,
                                        group_ltid,
                                        in));
 exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    SHR_FUNC_EXIT();
}

static int
bcmfp_lt_group_oper_mode_ltid_reset(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    uint32_t gid)
{
    uint64_t mode_oper_value = 0;
    uint64_t group_ltid_oper_value = 0;
    uint32_t offset = 0;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *in = NULL;
    uint32_t out_fields = 0;
    bcmltd_sid_t group_ltid = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_GROUP,
                                              &lt_info));
    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    if (lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP)  {
        /* EFP does not support this*/
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    group_ltid = lt_info->ltid;
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));
    out_fields = imm_fields.count;

    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.count = 1;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_lookup(unit,
                            group_ltid,
                            in,
                            &imm_fields));

    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.field[1]->id = fid_list->mode_oper_fid;
    input.field[1]->data = mode_oper_value;
    input.field[2]->id = fid_list->group_ltid_oper_fid;
    input.field[2]->data = group_ltid_oper_value;
    input.count = imm_fields.count + 1;
    if (in != NULL) {
        for (offset = 3; offset < input.count; offset++) {
            input.field[offset]->id = imm_fields.field[offset - 1]->id;
            input.field[offset]->idx = imm_fields.field[offset - 1]->idx;
            input.field[offset]->data = imm_fields.field[offset - 1]->data;
        }
    }
    SHR_IF_ERR_EXIT(bcmimm_entry_update(unit,
                                        0,
                                        group_ltid,
                                        in));

 exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    SHR_FUNC_EXIT();
}

static int
bcmfp_lt_group_oper_mode_reset(int unit,
                               bcmfp_stage_id_t stage_id,
                               uint32_t gid)
{
    uint64_t mode_oper_value = 0;
    uint32_t offset = 0;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *in = NULL;
    uint32_t out_fields = 0;
    bcmltd_sid_t group_ltid = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_GROUP,
                                              &lt_info));
    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    if (lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP)  {
        /* EFP does not support this*/
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    group_ltid = lt_info->ltid;
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));
    out_fields = imm_fields.count;

    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.count = 1;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_lookup(unit,
                            group_ltid,
                            in,
                            &imm_fields));

    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.field[1]->id = fid_list->mode_oper_fid;
    input.field[1]->data = mode_oper_value;
    input.count = imm_fields.count + 1;
    if (in != NULL) {
        for (offset = 2; offset < input.count; offset++) {
            input.field[offset]->id = imm_fields.field[offset - 1]->id;
            input.field[offset]->idx = imm_fields.field[offset - 1]->idx;
            input.field[offset]->data = imm_fields.field[offset - 1]->data;
        }
    }
    SHR_IF_ERR_EXIT(bcmimm_entry_update(unit,
                                        0,
                                        group_ltid,
                                        in));

 exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    SHR_FUNC_EXIT();
}


int
bcmfp_lt_entry_config_free(int unit,
                           bcmfp_lt_entry_config_t *entry_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_lt_policy_config_free(unit, &(entry_config->policy_config)));

    SHR_IF_ERR_EXIT
        (bcmfp_lt_rule_config_free(unit, &(entry_config->rule_config)));

    SHR_IF_ERR_EXIT
        (bcmfp_lt_group_config_free(unit, &(entry_config->group_config)));

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_entry_meter_params_set(int unit,
                             uint32_t trans_id,
                             uint32_t stage_id,
                             uint32_t gid,
                             uint32_t meter_id,
                             uint32_t **edw)
{
    bool global_pool = TRUE;
    bcmmeter_fp_entry_t *meter_entry = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmmeter_pool_global(unit, stage_id, &global_pool));
    /*
     * If Meter Pool is not global, Meter module requires slice
     * information from FP to write to HW meter table.
     * bcmmeter_entry_insert() to be called with the appropriate
     * slice number.
     * If Meter Pool is Global, Meter entry will be already programmed by
     * Meter module with the METER_POOL_ID passed by the user.
     */
    if (meter_id) {
        if (global_pool == FALSE) {
            SHR_IF_ERR_EXIT
                (bcmfp_group_oper_info_get(unit, stage_id,
                                           gid, &group_oper_info));
            SHR_IF_ERR_EXIT
                (bcmmeter_entry_insert(unit,
                                       trans_id,
                                       stage_id,
                                       group_oper_info->slice_id,
                                       meter_id));
        }
        SHR_IF_ERR_EXIT
            (bcmmeter_entry_attach(unit, stage_id, meter_id));
    }
    SHR_IF_ERR_EXIT
        (bcmmeter_entry_lookup(unit, stage_id, meter_id, &meter_entry));
    /*
     * Use the following fields from meter_entry to program IFP_POLICY_TABLE
     *
     * meter_entry->hw_fields.meter_pair_index  => METER_PAIR_INDEXf
     * meter_entry->hw_fields.meter_pair_mode   => METER_PAIR_MODEf
     * meter_entry->hw_fields.mode_modifier     => METER_PAIR_MODE_MODIFIERf
     * meter_entry->hw_fields.meter_test_odd    => METER_TEST_ODDf
     * meter_entry->hw_fields.meter_update_odd  => METER_UPDATE_ODDf
     * meter_entry->hw_fields.meter_test_even   => METER_TEST_EVENf
     * meter_entry->hw_fields.meter_update_even => METER_UPDATE_EVENf
     */

    SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                   BCMFP_ACTION_METER_PAIR_MODE_MODIFIER,
                                   meter_entry->hw_fields.mode_modifier,
                                   edw));
    SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                   BCMFP_ACTION_METER_PAIR_MODE,
                                   meter_entry->hw_fields.meter_pair_mode,
                                   edw));
    if (global_pool == TRUE) {
        SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                       BCMFP_ACTION_METER_PAIR_INDEX,
                                       meter_entry->hw_fields.meter_pair_index,
                                       edw));
    } else {
        SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                       BCMFP_ACTION_METER_INDEX_EVEN,
                                       meter_entry->hw_fields.meter_pair_index,
                                       edw));
        SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                       BCMFP_ACTION_METER_INDEX_ODD,
                                       meter_entry->hw_fields.meter_pair_index,
                                       edw));
    }
    SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                   BCMFP_ACTION_METER_UPDATE_EVEN,
                                   meter_entry->hw_fields.meter_update_even,
                                   edw));
    SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                   BCMFP_ACTION_METER_TEST_EVEN,
                                   meter_entry->hw_fields.meter_test_even,
                                   edw));
    SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                   BCMFP_ACTION_METER_UPDATE_ODD,
                                   meter_entry->hw_fields.meter_update_odd,
                                   edw));
    SHR_IF_ERR_EXIT(bcmfp_action_set(unit, stage_id, gid,
                                   BCMFP_ACTION_METER_TEST_ODD,
                                   meter_entry->hw_fields.meter_test_odd,
                                   edw));
 exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_pdd_policy_set(int unit,
                     uint32_t trans_id,
                     bcmfp_stage_t *stage,
                     bcmltd_sid_t ltid,
                     int pipe_id,
                     uint32_t flag,
                     bcmfp_group_id_t group_id,
                     bcmfp_lt_policy_data_t *policy_data,
                     int *qos_prof_idx,
                     int *em_class_id,
                     uint32_t **edw)
{
    uint32_t *prof_1_data = NULL;
    uint32_t *prof_2_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_INTERNAL);

    BCMFP_ALLOC(prof_1_data,
                sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX, "profile set 1 word");
    BCMFP_ALLOC(prof_2_data,
                sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX, "profile set 2 word");

    while(policy_data != NULL) {
        /* QOS profile actions */
        if (BCMFP_ACTION_IN_PROFILE_SET_1(policy_data->action)) {
            SHR_IF_ERR_EXIT(bcmfp_action_set(unit,
                                     stage->stage_id,
                                     group_id,
                                     policy_data->action,
                                     policy_data->param,
                                     &prof_1_data));
        } else if (BCMFP_ACTION_IN_PROFILE_SET_2(policy_data->action)) {
            SHR_IF_ERR_EXIT(bcmfp_action_set(unit,
                                     stage->stage_id,
                                     group_id,
                                     policy_data->action,
                                     policy_data->param,
                                     &prof_2_data));
        } else if (BCMFP_ACTION_EXACT_MATCH_CLASS_ID ==  policy_data->action) {
            *em_class_id = policy_data->param;
        } else {
            
            if (flag & BCMFP_GROUP_WITH_DEFAULT_POLICY) {
                /* match id 15*/
                *edw[0] = 0x1e49;
            } else {
                
                if (stage->flags & BCMFP_STAGE_TH3) {
                      *edw[0] = 0x4801;
                } else {
                    *edw[0] = 0x49;
                }
            }
        }
        policy_data = policy_data->next;
    }
    SHR_IF_ERR_EXIT(
        bcmfp_entry_qos_profile_install(unit, trans_id,
                                        stage,
                                        ltid,
                                        pipe_id,
                                        flag,
                                        group_id,
                                        prof_1_data,
                                        prof_2_data,
                                        qos_prof_idx));

exit:
    SHR_FREE(prof_1_data);
    SHR_FREE(prof_2_data);
    SHR_FUNC_EXIT();
}


int
bcmfp_lt_entry_hw_insert(int unit,
                         uint32_t trans_id,
                         bcmfp_lt_entry_config_t *entry_config)
{
    uint32_t i = 0;
    uint32_t pipe_id = 0;
    uint32_t gid = 0;
    uint32_t pipe_map;
    uint32_t pipe_min = 0;
    uint32_t pipe_max = 0;
    uint32_t flags = 0;
    uint32_t oper_state = 0;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t *qual_bitmap = NULL;
    uint16_t group_ref_count = 0;
    int qos_prof_idx = -1;
    int em_class_id = -1;
    bool reinsert_valid = FALSE;
    bcmdrd_pbmp_t pbmp_data;
    bcmdrd_pbmp_t pbmp_mask;
    bcmdrd_pbmp_t valid_pbmp;
    bcmfp_stage_id_t stage_id;
    bcmfp_group_t fg = {0};
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_oper_mode_t oper_mode;
    bcmfp_lt_group_config_t *group_config = NULL;
    bcmfp_lt_rule_config_t *rule_config = NULL;
    bcmfp_lt_policy_config_t *policy_config = NULL;
    bcmfp_lt_policy_config_t *default_policy_config = NULL;
    bcmfp_lt_rule_data_t *rule_data = NULL;
    bcmfp_lt_policy_data_t *policy_data = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_config, SHR_E_INTERNAL);

    stage_id = entry_config->stage_id;
    group_config = &(entry_config->group_config);
    rule_config = &(entry_config->rule_config);
    policy_config = &(entry_config->policy_config);
    /* Allocate Memory for entry Key word and data word */
    BCMFP_ALLOC(ekw, sizeof(uint32_t *) * BCMFP_ENTRY_PARTS_MAX, "ekw part");
    BCMFP_ALLOC(edw, sizeof(uint32_t *) * BCMFP_ENTRY_PARTS_MAX, "edw part");

    for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
        BCMFP_ALLOC(ekw[i],
                sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX, "ekw word");
        BCMFP_ALLOC(edw[i],
                sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX, "edw word");
    }

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    gid = group_config->group_id;
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmfp_group_oper_info_get(unit, stage_id, gid,
                                  &group_oper_info),
                                  SHR_E_NOT_FOUND);
    /* EFP & VFP-group_oper_info will not be NULL*/
    if ((group_oper_info == NULL) ||
        (group_oper_info->valid == FALSE))
    {
        /* Group is not created yet */
        fg.flags = 0;
        fg.ltid = entry_config->ltid;
        fg.group_id = gid;
        fg.group_prio = group_config->priority;
        fg.action_res_id = group_config->virtual_slice_grp;
        fg.num_presel_ids = group_config->num_presel_ids;
        fg.pipe_id = group_config->pipe_id;
        fg.lookup_id = group_config->lookup_id;
        fg.stage_id = stage_id;
        BCMFP_QSET_COPY(fg.qset, group_config->qset);
        fg.qual_bitmap = group_config->qual_bitmap;
        fg.group_mode = group_config->mode;

        if (group_config->policy_valid) {
            default_policy_config = &(group_config->policy_config);
        }
        /* Initialize the action profile index's to -1 */
        fg.ext_codes[0].action_profile_idx = -1;
        fg.ext_codes[0].default_action_prof_idx = -1;
        fg.ext_codes[0].default_qos_prof_idx = -1;
        fg.ext_codes[0].default_policy_enable = 0;

        if (stage->flags & BCMFP_STAGE_KEY_TYPE_FIXED) {
            fg.group_slice_mode = group_config->slice_mode;
            fg.group_port_pkt_type = group_config->port_pkt_type;
        }
        if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
            fg.lookup_id = group_config->lookup_id;
        }

        SHR_IF_ERR_EXIT(bcmfp_group_create(unit, trans_id, &fg));

        /* Update group mode */
        group_config->mode = fg.group_mode;
        /* update group_config oper ltid */
        group_config->group_ltid = fg.action_res_id;

        if (group_config->policy_valid) {
            policy_data = default_policy_config->data;
            /* call EM funtion to update qos profile data and
             * action profile data and return qos_prof_idx and
             * edw of action profile data
             */
            SHR_IF_ERR_EXIT(bcmfp_pdd_policy_set(unit,
                                          trans_id,
                                          stage,
                                          entry_config->ltid,
                                          group_config->pipe_id,
                                          group_config->flag,
                                          group_config->group_id,
                                          policy_data,
                                          &qos_prof_idx,
                                          &em_class_id,
                                          edw));
            /* Action profile idx for default entry */
            SHR_IF_ERR_EXIT(
                bcmfp_group_action_profile_install(unit,
                                                   trans_id,
                                                   stage, &fg));
            /* Default policy install */
            SHR_IF_ERR_EXIT(bcmfp_default_policy_install(unit,
                                          trans_id,
                                          stage,
                                          &fg,
                                          qos_prof_idx,
                                          em_class_id,
                                          edw));
            fg.flags &= ~BCMFP_GROUP_WITH_DEFAULT_POLICY;
            qos_prof_idx = -1;
            em_class_id = -1;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_group_oper_info_get(unit, stage_id, gid, &group_oper_info));
    }
    group_ref_count = group_oper_info->ref_count;
    /* Get stage operational mode */
    SHR_IF_ERR_EXIT(bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));

    if (oper_mode == BCMFP_STAGE_OPER_MODE_GLOBAL_PIPE_AWARE) {
        pipe_min = 0;
        SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
        pipe_max = shr_util_popcount(pipe_map) - 1;
    } else if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE){
        pipe_min = pipe_max = group_config->pipe_id;
    }

    if (rule_config->valid) {
        if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
            BCMDRD_PBMP_CLEAR(valid_pbmp);
            if (oper_mode == BCMFP_STAGE_OPER_MODE_GLOBAL_PIPE_AWARE) {
                for (pipe_id = pipe_min; pipe_id <= pipe_max; pipe_id++) {
                    BCMDRD_PBMP_CLEAR(pbmp_data);
                    SHR_IF_ERR_EXIT(
                        bcmdrd_dev_pipe_phys_pbmp(unit, pipe_id, &pbmp_data));
                    BCMDRD_PBMP_OR(valid_pbmp, pbmp_data);
                }
            } else if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE){
                SHR_IF_ERR_EXIT(
                    bcmdrd_dev_pipe_phys_pbmp(unit, pipe_min, &valid_pbmp));
            }
        }

        rule_data = rule_config->data;
        while(rule_data != NULL) {
            qual_bitmap = NULL;
            if (BCMFP_QSET_TEST(group_config->partial_qset,
                rule_data->qual)) {
                qual_bitmap = rule_data->bitmap;
            }

            SHR_IF_ERR_EXIT(bcmfp_qual_set(unit,
                                           stage_id,
                                           gid,
                                           BCMFP_ENTRY_TYPE_ALL,
                                           rule_data->qual,
                                           qual_bitmap,
                                           rule_data->data,
                                           rule_data->mask,
                                           ekw));
            rule_data = rule_data->next;
        }
        if (stage->flags & BCMFP_STAGE_KEY_TYPE_FIXED) {
            SHR_IF_ERR_EXIT(bcmfp_entry_key_match_type_set(unit,
                                            stage,
                                            gid,
                                            group_config->slice_mode,
                                            group_config->port_pkt_type,
                                            ekw));
        }
    }

    policy_data = policy_config->data;
    if (!(stage->flags & BCMFP_STAGE_POLICY_TYPE_PDD)) {
        if (policy_config->valid) {
            while(policy_data != NULL) {
                SHR_IF_ERR_EXIT(bcmfp_action_set(unit,
                                                 stage_id,
                                                 gid,
                                                 policy_data->action,
                                                 policy_data->param,
                                                 edw));
                policy_data = policy_data->next;
            }
        }
    } else {
        /* call EM funtion to update qos profile data and
         * action profile data and return qos_prof_idx and
         * edw of action profile data
         */
        SHR_IF_ERR_EXIT(bcmfp_pdd_policy_set(unit,
                                             trans_id,
                                             stage,
                                             entry_config->ltid,
                                             group_config->pipe_id,
                                             group_config->flag,
                                             group_config->group_id,
                                             policy_data,
                                             &qos_prof_idx,
                                             &em_class_id,
                                             edw));
        SHR_IF_ERR_EXIT(bcmfp_custom_entry_set(unit,
                                               trans_id,
                                               stage,
                                               gid,
                                               FALSE,
                                               qos_prof_idx,
                                               em_class_id,
                                               ekw,
                                               edw));
    }

    if (entry_config->meter_valid) {
        if (group_ref_count ||
            !(stage->flags & BCMFP_STAGE_KEY_TYPE_FIXED)) {
            SHR_IF_ERR_EXIT(bcmfp_entry_meter_params_set(unit,
                                                    trans_id,
                                                    stage_id,
                                                    gid,
                                                    entry_config->meter_id,
                                                    edw));
        } else {
            /* Meter is valid for first entry for Fixed key group
             * Need to re-insert same entry with meter */
            reinsert_valid = TRUE;
        }
    }

     /* If the operating mode is  either GLOBAL_PIPE_AWARE or PIPE_UNIQUE, entry
      * insert has to performed for each pipe */
    
    if ((stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) &&
        (stage->flags & BCMFP_STAGE_INPORTS_SUPPORT) &&
        (oper_mode != BCMFP_STAGE_OPER_MODE_GLOBAL))  {

        if(oper_mode == BCMFP_STAGE_OPER_MODE_GLOBAL_PIPE_AWARE) {
            flags = BCMFP_ENTRY_WRITE_PER_PIPE_IN_GLOBAL;
        } else {
            flags = 0;
        }

        /* Validate Inport Data */
        BCMDRD_PBMP_NEGATE(pbmp_data, valid_pbmp);
        BCMDRD_PBMP_AND(pbmp_data, rule_config->inports_pbmp_data);
        if (BCMDRD_PBMP_NOT_NULL(pbmp_data)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        /* Validate Inport mask */
        BCMDRD_PBMP_NEGATE(pbmp_mask, valid_pbmp);
        BCMDRD_PBMP_AND(pbmp_mask, rule_config->inports_pbmp_mask);
        if (BCMDRD_PBMP_NOT_NULL(pbmp_mask)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        for (pipe_id = pipe_min; pipe_id <= pipe_max; pipe_id++) {
            if (pipe_id != pipe_min) {
                flags |= BCMFP_ENTRY_SAME_KEY;
            }
            /* Calculate ekw/edw for each pipe if the INPORTS qualifier is
             * present in the rule. */
            if ((rule_config->valid) &&
                (rule_config->is_qual_inports == TRUE)) {
                BCMDRD_PBMP_CLEAR(pbmp_data);
                BCMDRD_PBMP_CLEAR(pbmp_mask);
                SHR_IF_ERR_EXIT(bcmfp_device_ipbm_to_pipe_ipbmp(unit,
                            rule_config->inports_pbmp_data, pipe_id, &pbmp_data));

                SHR_IF_ERR_EXIT(bcmfp_device_ipbm_to_pipe_ipbmp(unit,
                            rule_config->inports_pbmp_mask, pipe_id, &pbmp_mask));

                SHR_IF_ERR_EXIT(bcmfp_qual_set(unit,
                                               stage_id,
                                               gid,
                                               BCMFP_ENTRY_TYPE_ALL,
                                               BCMFP_QUAL_INPORTS,
                                               NULL,
                                               pbmp_data.w,
                                               pbmp_mask.w,
                                               ekw));
            }

            SHR_IF_ERR_EXIT
                (bcmfp_entry_insert(unit,
                                    flags,
                                    trans_id,
                                    pipe_id,
                                    stage_id,
                                    entry_config->ltid,
                                    gid,
                                    entry_config->eid,
                                    entry_config->priority,
                                    ekw,
                                    edw));
        }
    } else {
        flags = 0;
        if (group_ref_count == 0) {
            flags = BCMFP_ENTRY_FIRST_IN_GROUP;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_insert(unit,
                                flags,
                                trans_id,
                                group_config->pipe_id,
                                stage_id,
                                entry_config->ltid,
                                gid,
                                entry_config->eid,
                                entry_config->priority,
                                ekw,
                                edw));

        if (reinsert_valid == TRUE) {
            SHR_IF_ERR_EXIT(bcmfp_entry_meter_params_set(unit,
                                                         trans_id,
                                                         stage_id,
                                                         gid,
                                                         entry_config->meter_id,
                                                         edw));
            SHR_IF_ERR_EXIT
                (bcmfp_entry_insert(unit,
                                    0,
                                    trans_id,
                                    group_config->pipe_id,
                                    stage_id,
                                    entry_config->ltid,
                                    gid,
                                    entry_config->eid,
                                    entry_config->priority,
                                    ekw,
                                    edw));
        }
    }

    for (i = 0; i < BCMFP_ENTRY_PARTS_MAX;i++) {
        sal_memset(ekw[i], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
        sal_memset(edw[i], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    }

    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL
            && group_ref_count == 0) {
        SHR_IF_ERR_EXIT
            (bcmfp_lt_presel_entry_config(unit, trans_id, stage_id,
            entry_config->eid, group_config, ekw, edw));
    }

    group_ref_count++;
    group_oper_info->ref_count = group_ref_count;

    /* update the oper status with SUCCESS*/
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_entry_oprtnl_state_get(unit,
                                                         stage_id,
                                                         entry_config->eid,
                                                         &oper_state));
    if(SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND))  {
        SHR_IF_ERR_EXIT(bcmfp_lt_entry_oprtnl_state_update(unit,
                                                           trans_id,
                                                           stage_id,
                                                           entry_config->eid,
                                                           BCMFP_ENTRY_SUCCESS));
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    if (ekw != NULL) {
        for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
            SHR_FREE(ekw[i]);
        }
        SHR_FREE(ekw);
    }
    if (edw != NULL) {
        for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
            SHR_FREE(edw[i]);
        }
        SHR_FREE(edw);
    }

   SHR_FUNC_EXIT();
}

int
bcmfp_em_entry_build_ekw(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_group_id_t group_id,
                         bcmfp_entry_id_t entry_id,
                         bcmfp_lt_group_config_t *group_config,
                         bcmfp_lt_rule_config_t *rule_config,
                         uint32_t **ekw)
{
    uint32_t *qual_bitmap = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    bcmfp_stage_t *stage = NULL;
    bcmfp_lt_rule_data_t *rule_data = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rule_config, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (rule_config->valid) {

        rule_data = rule_config->data;
        while(rule_data != NULL) {
            qual_bitmap = NULL;
            if (BCMFP_QSET_TEST(group_config->partial_qset,
                rule_data->qual)) {
                qual_bitmap = rule_data->bitmap;
            }

            SHR_IF_ERR_EXIT(bcmfp_qual_set(unit,
                                           stage_id,
                                           group_id,
                                           BCMFP_ENTRY_TYPE_ALL,
                                           rule_data->qual,
                                           qual_bitmap,
                                           rule_data->data,
                                           rule_data->mask,
                                           ekw));
            rule_data = rule_data->next;
        }
    }
    SHR_IF_ERR_EXIT(bcmfp_custom_entry_set(unit,
                                trans_id,
                                stage,
                                group_id,
                                TRUE,
                                -1,
                                -1,
                                ekw,
                                NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_hw_delete(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         bcmdrd_sid_t entry_ltid,
                         uint32_t eid,
                         uint32_t gid)
{
    uint32_t i = 0;
    uint32_t flags = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_lt_group_config_t group_config = {0};
    bcmfp_lt_rule_config_t rule_config = {0};
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmltd_sid_t presel_sid = FP_ING_PRESEL_ENTRY_TEMPLATEt;
    uint32_t rule_id = 0;
    uint32_t **ekw = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
        presel_sid = FP_EM_PRESEL_ENTRY_TEMPLATEt;
    }

    SHR_IF_ERR_EXIT
        (bcmfp_lt_group_config_get(unit, trans_id, stage_id, gid, &group_config));

    if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
        /* Allocate Memory for entry Key word and data word */
        BCMFP_ALLOC(ekw, sizeof(uint32_t *) * BCMFP_ENTRY_PARTS_MAX, "ekw part");

	    for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
            BCMFP_ALLOC(ekw[i],
                sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX, "ekw word");
        }
        SHR_IF_ERR_EXIT
            (bcmfp_lt_rule_id_get(unit, trans_id, stage_id,
                                  entry_ltid, eid, &rule_id));
        SHR_IF_ERR_EXIT
            (bcmfp_lt_rule_config_get(unit,
                                      trans_id,
                                      stage_id,
                                      rule_id,
                                      eid,
                                      NULL,
                                      &group_config,
                                      &rule_config));
        SHR_IF_ERR_EXIT
            (bcmfp_em_entry_build_ekw(unit, trans_id, entry_ltid,
                                      gid, eid, &group_config,
                                      &rule_config, ekw));
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id, gid,
                                  &group_oper_info));

    if (group_oper_info->ref_count != 1) {
        /* More Entries are created in this group */
            SHR_IF_ERR_EXIT(bcmfp_entry_delete(unit, 0,
                                               trans_id,
                                               stage_id,
                                               entry_ltid,
                                               gid, eid,
                                               ekw));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    /* Last Entry in this Group */
    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
        if (group_config.num_presel_ids == 0) {
            flags = BCMFP_ENTRY_PRESEL | BCMFP_ENTRY_PRESEL_DEFAULT;
            if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
                flags |= BCMFP_ENTRY_LAST_IN_GROUP;
            }
            SHR_IF_ERR_EXIT
                (bcmfp_entry_delete(unit, flags, trans_id, stage_id,
                    presel_sid, gid, gid, ekw));
        } else {
            for (i = 0; i < group_config.num_presel_ids; i++) {
                SHR_IF_ERR_EXIT(bcmfp_entry_delete(unit, BCMFP_ENTRY_PRESEL,
                                    trans_id, stage_id, presel_sid,
                                    gid, group_config.presel_ids[i], ekw));
            }
            if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
                flags = BCMFP_ENTRY_PRESEL | BCMFP_ENTRY_LAST_IN_GROUP;
            } else {
                flags = BCMFP_ENTRY_PRESEL;
            }
            SHR_IF_ERR_EXIT
                (bcmfp_entry_delete(unit, flags, trans_id, stage_id,
                    presel_sid, gid, group_config.presel_ids[i], ekw));
        }
    }
    SHR_IF_ERR_EXIT
        (bcmfp_entry_delete(unit, BCMFP_ENTRY_LAST_IN_GROUP, trans_id,
                            stage_id, entry_ltid, gid, eid, ekw));
    SHR_IF_ERR_EXIT(bcmfp_group_delete(unit, trans_id, stage_id, gid));

exit:
    if (ekw != NULL) {
        for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
            SHR_FREE(ekw[i]);
        }
        SHR_FREE(ekw);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_field_update(int unit,
                            bcmltd_sid_t ltid,
                            const bcmltd_field_t * in_field,
                            uint32_t out_fields,
                            bcmltd_fields_t *imm_fields,
                            uint8_t *flags)
{
    uint32_t fid = 0;
    uint64_t data = 0;
    uint64_t in_data = 0;
    SHR_FUNC_ENTER(unit);

    fid = in_field->id;
    in_data = in_field->data;

    /* Search for Fid in imm_fields */
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_data_get(unit, fid, imm_fields, &data));
    if (SHR_FUNC_ERR()) {
        /* Need to add field */
        *flags |= BCMFP_LT_FIELD_NEW;
    } else if (data != in_data) {
        *flags |= BCMFP_LT_FIELD_REMOVED;
        *flags |= BCMFP_LT_FIELD_NEW;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_insert(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t sid,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data)
{
    uint32_t gid = 0;
    uint64_t data_val = 0;
    bcmfp_stage_id_t stage_id;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_entry_config_t entry_config = {0};
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    uint32_t policy_id = 0, rule_id= 0;
    uint32_t fid = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_lt_entry_sp_fids_t *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));

    stage_id = lt_info->stage_id;
    fid_list = (bcmfp_lt_entry_sp_fids_t *) lt_info->sp_fids;

    /* Populate Entry configuration */
    SHR_IF_ERR_EXIT(bcmfp_lt_entry_config_get(unit,
                                              trans_id,
                                              stage_id,
                                              lt_info,
                                              key,
                                              data,
                                              0,
                                              NULL,
                                              &entry_config));
    gid = entry_config.group_config.group_id;
    /* Call to H/w for Configuration */
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_entry_hw_insert(unit, trans_id, &entry_config));
    if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_IF_ERR_EXIT(bcmfp_lt_entry_oprtnl_state_delete(unit,
                                                           trans_id,
                                                           stage_id,
                                                           entry_config.eid));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id, gid, &group_oper_info));

    /*EFP does not support MODE_OPER */
    if (!(lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP) &&
        !(lt_info->flags & BCMFP_LT_FLAGS_HASH_KEY_GRP))  {
        if (group_oper_info->ref_count == 1) {
            SHR_IF_ERR_EXIT(bcmfp_lt_group_oper_mode_update(unit,
                                                            trans_id,
                                                            &entry_config));
        }
    }

    if (lt_info->flags & BCMFP_LT_FLAGS_HASH_KEY_GRP) {
        if (group_oper_info->ref_count == 1) {
            SHR_IF_ERR_EXIT(bcmfp_lt_group_oper_mode_ltid_update(unit,
                                                                 trans_id,
                                                                 &entry_config));
        }
    }

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    /*Insert the entry in the Rule template info list*/
    fid = fid_list->rule_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data_val) {
        BCMFP_UINT64_TO_UINT32(data_val, rule_id);
        SHR_IF_ERR_EXIT(bcmfp_lt_rule_template_info_insert(unit,
                                                           trans_id,
                                                           stage,
                                                           rule_id,
                                                           entry_config.eid));
    }

    /*Insert the entry in the  Policy template info list*/
    fid = fid_list->policy_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data_val) {
        BCMFP_UINT64_TO_UINT32(data_val, policy_id);
        SHR_IF_ERR_EXIT(bcmfp_lt_policy_template_info_insert(unit,
                                                             trans_id,
                                                             stage,
                                                             policy_id,
                                                             entry_config.eid));
    }
    SHR_IF_ERR_EXIT(bcmfp_lt_lock_templates_id(unit,
                                               stage_id,
                                               &entry_config));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    (void) bcmfp_lt_entry_config_free(unit, &entry_config);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_update(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t sid,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data)
{
    uint32_t gid = 0;
    bool changed = FALSE;
    bool global_pool = TRUE;
    uint32_t fid = 0;
    uint32_t eid = 0;
    uint32_t meter_id = 0;
    uint32_t old_meter_id = 0;
    uint32_t out_fields = 0;
    uint64_t data_val = 0;
    uint64_t key_val = 0;
    uint8_t flags = 0;
    uint32_t policy_id_imm = 0, rule_id_imm = 0;
    uint32_t policy_id_in = 0, rule_id_in= 0;
    bcmltd_field_t *data_iter = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmfp_stage_id_t stage_id;
    bcmltd_sid_t ltid;
    bcmltd_fields_t imm_fields = {0};
    bcmfp_lt_entry_config_t entry_config = {0};
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_entry_sp_fids_t *fid_list = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));

    ltid = sid;
    stage_id = lt_info->stage_id;
    fid_list = (bcmfp_lt_entry_sp_fids_t *) lt_info->sp_fids;

    fid = fid_list->entry_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, eid);

    if(!(lt_info->flags & BCMFP_LT_FLAGS_NO_METER_SUPPORT)) {
        fid = fid_list->meter_id_fid;
        BCMFP_RET_VAL_ASSIGN(
            bcmfp_lt_field_data_get_from_data_array(unit, fid,
                                             data, &data_val));

        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, meter_id);
        }
    }

    /* Populate imm_fields */
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, ltid, &imm_fields));
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, ltid, &input));
    out_fields = input.count;

    in = &input;
    input.field[0]->id = fid_list->entry_id_fid;
    input.field[0]->data = eid;
    input.count = 1;
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                        sid,
                                        in,
                                        &imm_fields));

    if(!(lt_info->flags & BCMFP_LT_FLAGS_NO_METER_SUPPORT)) {
        fid = fid_list->meter_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, old_meter_id);
        }
    }
    data_iter = (bcmltd_field_t*) data;
    do {
        fid = data_iter->id;
        SHR_IF_ERR_EXIT(bcmfp_lt_entry_field_update(unit,
                ltid, data_iter, out_fields, &imm_fields, &flags));

        changed |= (flags != 0);
        data_iter = data_iter->next;
    } while(data_iter);

    if (!changed) {
        /* No Change in Entry Configuration */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Populate Entry configuration */
    SHR_IF_ERR_EXIT(bcmfp_lt_entry_config_get(unit,
                                              trans_id,
                                              stage_id,
                                              lt_info,
                                              key,
                                              data,
                                              0,
                                              NULL,
                                              &entry_config));
    /* If old and new meter id's are same.
     * It will be attached during bcmfp_entry_meter_params_set
     */
    if (old_meter_id) {
        SHR_IF_ERR_EXIT
            (bcmmeter_entry_detach(unit, stage_id, old_meter_id));
    }

    if (meter_id != old_meter_id) {
        SHR_IF_ERR_EXIT
            (bcmmeter_pool_global(unit, stage_id, &global_pool));
        if (old_meter_id) {
            if (global_pool == FALSE) {
                SHR_IF_ERR_EXIT
                    (bcmmeter_entry_delete(unit,
                                           trans_id,
                                           stage_id,
                                           old_meter_id));
            }
        }
    }

    /* Call to H/w for Configuration */
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_entry_hw_insert(unit, trans_id, &entry_config));
    if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_IF_ERR_EXIT(bcmfp_lt_entry_oprtnl_state_delete(unit,
                                                           trans_id,
                                                           stage_id,
                                                           eid));
        SHR_EXIT();
    }

    gid = entry_config.group_config.group_id;
    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id, gid,
                                  &group_oper_info));
    /* Update should not increase the ref_count */
    group_oper_info->ref_count--;

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    fid = fid_list->rule_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) ) {
        if (data_val) {
            BCMFP_UINT64_TO_UINT32(data_val, rule_id_in);
        }
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, rule_id_imm);
        }
        if (rule_id_in != rule_id_imm) {
            /*Delete the entry from the of rule_id_imm list if not 0*/
            if (rule_id_imm)  {
                SHR_IF_ERR_EXIT(
                        bcmfp_lt_rule_template_info_delete(unit,
                                                           trans_id,
                                                           stage,
                                                           rule_id_imm,
                                                           entry_config.eid));
            }
            /*Insert the entry in the of rule_id_in list if not 0*/
            if(rule_id_in) {
                SHR_IF_ERR_EXIT(
                       bcmfp_lt_rule_template_info_insert(unit,
                                                          trans_id,
                                                          stage,
                                                          rule_id_in,
                                                          entry_config.eid));
            }
        }
    }

    fid = fid_list->policy_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        if (data_val) {
            BCMFP_UINT64_TO_UINT32(data_val, policy_id_in);
        }
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, policy_id_imm);
        }
        if (policy_id_in != policy_id_imm) {
            /*Delete the entry from the of policy_id_imm list if not 0*/
            if (policy_id_imm)  {
                SHR_IF_ERR_EXIT(
                        bcmfp_lt_policy_template_info_delete(unit,
                                                             trans_id,
                                                             stage,
                                                             policy_id_imm,
                                                             entry_config.eid));
            }
            /*Insert the entry in the of policy_id_in list if not 0*/
            if(policy_id_in) {
                SHR_IF_ERR_EXIT(
                        bcmfp_lt_policy_template_info_insert(unit,
                                                             trans_id,
                                                             stage,
                                                             policy_id_in,
                                                             entry_config.eid));
            }
        }
    }
    
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_delete(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t sid,
                      const bcmltd_field_t *key)
{
    uint32_t eid = 0;
    uint32_t gid = 0;
    uint32_t fid = 0;
    uint32_t meter_id = 0;
    uint32_t rule_id = 0;
    uint32_t policy_id = 0;
    uint32_t out_fields = 0;
    uint64_t key_val = 0;
    uint64_t data = 0;
    bool     global_pool = TRUE;
    bcmfp_stage_id_t stage_id;
    bcmltd_sid_t ltid;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmltd_fields_t imm_fields = {0};
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_info_t *grp_lt_info = NULL;
    bcmfp_lt_info_t *rule_lt_info = NULL;
    bcmfp_lt_info_t *policy_lt_info = NULL;
    bcmfp_lt_entry_sp_fids_t *fid_list = NULL;
    bcmfp_lt_group_sp_fids_t *grp_fid_list = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));

    ltid = sid;
    stage_id = lt_info->stage_id;
    fid_list = (bcmfp_lt_entry_sp_fids_t *) lt_info->sp_fids;

    /* Get Entry configuration from In-mem DB */
    fid = fid_list->entry_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, eid);

    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, ltid, &imm_fields));
    out_fields = imm_fields.count;

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, ltid, &input));
    out_fields = input.count;

    in = &input;
    input.field[0]->id = fid_list->entry_id_fid;
    input.field[0]->data = eid;
    input.count = 1;
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                        sid,
                                        in,
                                        &imm_fields));

    /*Get the group template ID*/
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    BCMFP_UINT64_TO_UINT32(data, gid);
    SHR_IF_ERR_EXIT(bcmfp_lt_entry_hw_delete(unit,
                                             trans_id,
                                             stage_id,
                                             ltid,
                                             eid,
                                             gid));
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmfp_group_oper_info_get(unit, stage_id, gid,
                        &group_oper_info), SHR_E_NOT_FOUND);
    if (group_oper_info != NULL) {
        group_oper_info->ref_count--;
    }
    /* LT for stage */
    /*Unlock Group Id*/
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_GROUP,
                                              &grp_lt_info));
    grp_fid_list = (bcmfp_lt_group_sp_fids_t*) grp_lt_info->sp_fids;

    in = &input;
    input.field[0]->id = grp_fid_list->group_id_fid;
    input.field[0]->data = gid;
    SHR_IF_ERR_EXIT
        (bcmimm_entry_unlock(unit, grp_lt_info->ltid, in));

    if (!(grp_lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP) &&
        !(grp_lt_info->flags & BCMFP_LT_FLAGS_HASH_KEY_GRP))  {
        if ((group_oper_info == NULL) ||
            (group_oper_info->valid == FALSE)) {
            SHR_IF_ERR_EXIT(bcmfp_lt_group_oper_mode_reset(unit,
                                                           stage_id,
                                                           gid));
        }
    }

    if (grp_lt_info->flags & BCMFP_LT_FLAGS_HASH_KEY_GRP) {
        if ((group_oper_info == NULL) ||
            (group_oper_info->valid == FALSE)) {
            SHR_IF_ERR_EXIT(bcmfp_lt_group_oper_mode_ltid_reset(unit,
                                                                stage_id,
                                                                gid));
        }
    }

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_RULE,
                                              &rule_lt_info));

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_POLICY,
                                              &policy_lt_info));
    
    if(!(lt_info->flags & BCMFP_LT_FLAGS_NO_METER_SUPPORT)) {
        fid = fid_list->meter_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data) {
            BCMFP_UINT64_TO_UINT32(data, meter_id);
            SHR_IF_ERR_EXIT(bcmmeter_entry_detach(unit, stage_id, meter_id));

            SHR_IF_ERR_EXIT(bcmmeter_pool_global(unit, stage_id, &global_pool));

            /*
             * If Meter Pool is not global, Meter entry will be programmed
             * in H/W when FP entry is installed. Similarly, Meter entry
             * should be deleted when the FP entry is uninstalled.
             * If Meter Pool is Global, Meter entry will be programmed and
             * deleted solely by Meter module.
             */
            if (global_pool == FALSE) {
                SHR_IF_ERR_EXIT(bcmmeter_entry_delete(unit,
                                                      trans_id,
                                                      stage_id,
                                                      meter_id));
            }
        }
    }
    SHR_IF_ERR_EXIT(bcmfp_lt_entry_oprtnl_state_delete(unit,
                                                       trans_id,
                                                       stage_id,
                                                       eid));

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    fid = fid_list->rule_id_fid;
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data) {
        BCMFP_UINT64_TO_UINT32(data, rule_id);
        SHR_IF_ERR_EXIT(bcmfp_lt_rule_template_info_delete(unit,
                                                        trans_id,
                                                        stage,
                                                        rule_id,
                                                        eid));
    }
    fid = fid_list->policy_id_fid;
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data) {
        BCMFP_UINT64_TO_UINT32(data, policy_id);
        SHR_IF_ERR_EXIT(bcmfp_lt_policy_template_info_delete(unit,
                                                         trans_id,
                                                         stage,
                                                         policy_id,
                                                         eid));
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_config_get(int unit, uint32_t trans_id,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_lt_info_t *lt_info,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          uint32_t template_fid,
                          const bcmltd_field_t *template_data,
                          bcmfp_lt_entry_config_t *entry_config)
{
    uint32_t fid = 0, gid = 0;
    uint32_t policy_id = 0, rule_id = 0;
    uint64_t data_val = 0;
    uint64_t key_val = 0;
    bcmfp_lt_entry_sp_fids_t * fid_list = NULL;
    bcmfp_lt_group_config_t *group_config = NULL;
    bcmfp_lt_rule_config_t *rule_config = NULL;
    bcmfp_lt_policy_config_t *policy_config = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t imm_fields = {0};
    uint32_t out_fields = 0;
    bcmltd_fields_t *in = NULL;
    const bcmltd_field_t *rule_template_data = NULL;
    const bcmltd_field_t *policy_template_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_config, SHR_E_INTERNAL);

    fid_list = (bcmfp_lt_entry_sp_fids_t *) lt_info->sp_fids;
    group_config = &(entry_config->group_config);
    rule_config = &(entry_config->rule_config);
    policy_config = &(entry_config->policy_config);

    entry_config->ltid = lt_info->ltid;
    entry_config->stage_id = lt_info->stage_id;

    /*Get the key value from the key array */
    fid = fid_list->entry_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, entry_config->eid);

    /* Populate imm_fields */
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit,
                                           entry_config->ltid,
                                           &imm_fields));
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit,
                                                      entry_config->ltid,
                                                      &input));
    out_fields = input.count;

    in = &input;
    input.field[0]->id = fid_list->entry_id_fid;
    input.field[0]->data =  entry_config->eid;
    input.count = 1;
    BCMFP_RET_VAL_ASSIGN(bcmimm_entry_lookup(unit,
                                             entry_config->ltid,
                                             in,
                                             &imm_fields));

    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        /* Reading from the in-memory during update operation */
        fid = fid_list->priority_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, entry_config->priority);
        }

        fid = fid_list->group_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, gid);
        }

        fid = fid_list->rule_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, rule_id);
        }

        fid = fid_list->policy_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, policy_id);
        }
        fid = fid_list->meter_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, entry_config->meter_id);
        }
        fid = fid_list->ctr_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, entry_config->ctr_id);
        }
    }

    fid = fid_list->priority_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, entry_config->priority);
    }

    fid = fid_list->group_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) && (data_val)) {
        BCMFP_UINT64_TO_UINT32(data_val, gid);
    }
    if (gid) {
        SHR_IF_ERR_EXIT(bcmfp_lt_group_config_get(unit,
                                                  trans_id,
                                                  stage_id,
                                                  gid,
                                                  group_config));
    }
    fid = fid_list->rule_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        if (data_val) {
            BCMFP_UINT64_TO_UINT32(data_val, rule_id);
        } else  {
            /* Dettach the rule template if the ID == 0 */
            rule_id = 0;
            rule_config->valid = FALSE;
        }
    }

    if (rule_id)  {
        /*Check if the template data is meant for this template*/
        if (template_fid == fid_list->rule_id_fid)  {
            rule_template_data = template_data;
        }
        SHR_IF_ERR_EXIT(bcmfp_lt_rule_config_get(unit,
                                                 trans_id,
                                                 stage_id,
                                                 rule_id,
                                                 entry_config->eid,
                                                 rule_template_data,
                                                 group_config,
                                                 rule_config));
    }

    fid = fid_list->policy_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        if (data_val) {
            BCMFP_UINT64_TO_UINT32(data_val, policy_id);
        } else {
            /* Dettach the policy template if the ID == 0 */
            policy_id = 0;
            policy_config->valid = FALSE;
        }
    }

    if (policy_id)  {
        /*Check if the template data is meant for this template*/
        if (template_fid == fid_list->policy_id_fid)  {
            policy_template_data = template_data;
        }
        SHR_IF_ERR_EXIT(bcmfp_lt_policy_config_get(unit,
                                                   trans_id,
                                                   stage_id,
                                                   policy_id,
                                                   entry_config->eid,
                                                   policy_template_data,
                                                   policy_config));
    }

    if(!(lt_info->flags & BCMFP_LT_FLAGS_NO_METER_SUPPORT)) {
        fid = fid_list->meter_id_fid;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                     fid,
                                                                     data,
                                                                     &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            if (data_val) {
                BCMFP_UINT64_TO_UINT32(data_val, entry_config->meter_id);
            } else  {
                entry_config->meter_id = 0;
            }
        }
        if (entry_config->meter_id == 0){
            entry_config->meter_valid = FALSE;
        } else  {
            entry_config->meter_valid = TRUE;
        }
    }

    fid = fid_list->ctr_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        if (data_val) {
            BCMFP_UINT64_TO_UINT32(data_val, entry_config->ctr_id);
        } else  {
            /* Dettach the counter of the ID == 0 */
            entry_config->ctr_id = 0;
        }
    }
    if (entry_config->ctr_id == 0){
        entry_config->ctr_valid = FALSE;
    } else {
        entry_config->ctr_valid = TRUE;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    if (SHR_FUNC_ERR()) {
        (void) bcmfp_lt_entry_config_free(unit, entry_config);
    }

    SHR_FUNC_EXIT();
}

int bcmfp_lt_lock_templates_id(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_lt_entry_config_t *entry_config)

{
    bcmfp_lt_group_sp_fids_t *group_fid_list = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_group_config_t *group_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_config, SHR_E_INTERNAL);
    group_config = &(entry_config->group_config);


    input.count = 0;
    input.field = NULL;
    in = &input;
    BCMFP_ALLOC(input.field, sizeof(bcmltd_field_t *),
            "bcmltd field array");
    BCMFP_ALLOC(input.field[0], sizeof (bcmltd_field_t), "field");

    /* Locke the group template*/
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                stage_id,
                BCMFP_LT_TYPE_GROUP,
                &lt_info));
    group_fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    input.field[0]->id = group_fid_list->group_id_fid;
    input.field[0]->data = group_config->group_id;
    input.count = 1;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lock(unit, lt_info->ltid, in));

exit:
    if (in) {
        SHR_FREE(in->field[0]);
        SHR_FREE(in->field);
    }
    SHR_FUNC_EXIT();
}


int
bcmfp_lt_rule_id_get(int unit,
                     uint32_t trans_id,
                     bcmfp_stage_id_t stage_id,
                     bcmltd_sid_t entry_ltid,
                     bcmfp_entry_id_t entry_id,
                     uint32_t *rule_id)
{
    uint32_t fid = 0;
    uint32_t out_fields = 0;
    uint64_t data = 0;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmltd_fields_t imm_fields = {0};
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_entry_sp_fids_t *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, entry_ltid, &lt_info));

    fid_list = (bcmfp_lt_entry_sp_fids_t *) lt_info->sp_fids;

    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, entry_ltid, &imm_fields));
    out_fields = imm_fields.count;

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, entry_ltid, &input));
    out_fields = input.count;

    in = &input;
    input.field[0]->id = fid_list->entry_id_fid;
    input.field[0]->data = entry_id;
    input.count = 1;
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                        entry_ltid,
                                        in,
                                        &imm_fields));

    fid = fid_list->rule_id_fid;
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data) {
        BCMFP_UINT64_TO_UINT32(data, *rule_id);
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_update_entries(int unit,
                        uint32_t trans_id,
                        int lt_type,
                        bcmltd_sid_t sid,
                        const bcmltd_field_t *template_key,
                        const bcmltd_field_t *template_data,
                        bcmimm_entry_event_t event_reason)
{
/*
1. Find the stage from the sid
2. Scan through the entries in the stage
3. Check if key is linked with the entry
4. If yes call the hardware update function
*/
    uint32_t gid;
    uint32_t entry_id = 0;
    uint64_t key_val = 0;
    bcmfp_stage_id_t stage_id;
    bcmltd_sid_t ltid;
    bcmfp_lt_info_t *entry_lt_info = NULL;
    bcmfp_lt_info_t *template_lt_info = NULL;
    bcmltd_field_t data_field = {0};
    const bcmltd_field_t *entry_data = &data_field;
    bcmltd_field_t key_field = {0};
    const bcmltd_field_t *entry_key = &key_field;
    bcmfp_lt_entry_config_t entry_config = {0};
    uint32_t fid = 0;
    uint32_t template_id_fid = 0;
    bcmfp_lt_policy_sp_fids_t *policy_fid_list = NULL;
    bcmfp_lt_rule_sp_fids_t *rule_fid_list = NULL;
    bcmfp_lt_entry_sp_fids_t *entry_fid_list = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;
    void *temp_ptr = NULL;

    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &template_lt_info));

    stage_id = template_lt_info->stage_id;
    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    /* Get the Entry LTD from the stage_Id */
    switch (stage_id) {
        case BCMFP_STAGE_ID_LOOKUP:
            ltid = FP_VLAN_ENTRYt;
            break;
        case BCMFP_STAGE_ID_INGRESS:
            ltid = FP_ING_ENTRYt;
            break;
        case BCMFP_STAGE_ID_EGRESS:
            ltid = FP_EGR_ENTRYt;
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            ltid = FP_EM_ENTRYt;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, ltid, &entry_lt_info));
    entry_fid_list = (bcmfp_lt_entry_sp_fids_t *)entry_lt_info->sp_fids;

    switch(lt_type) {
        case BCMFP_LT_TYPE_POLICY:
            /*Return if no entry is linked to the policy template list*/
            if(stage->policy_template_info_list[template_key->data - 1] == NULL)  {
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
            rule_fid_list = (bcmfp_lt_rule_sp_fids_t *)template_lt_info->sp_fids;
            policy_fid_list = (bcmfp_lt_policy_sp_fids_t *)template_lt_info->sp_fids;
            fid = policy_fid_list->policy_id_fid;
            template_id_fid = entry_fid_list->policy_id_fid;
            break;
        case BCMFP_LT_TYPE_RULE:
            /*Return if no entry is linked to the rule template list*/
            if(stage->rule_template_info_list[template_key->data - 1] == NULL)  {
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
            rule_fid_list = (bcmfp_lt_rule_sp_fids_t *)template_lt_info->sp_fids;
            fid = rule_fid_list->rule_id_fid;
            template_id_fid = entry_fid_list->rule_id_fid;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          template_key,
                                                          &key_val));

    if (lt_type == BCMFP_LT_TYPE_RULE)  {
        temp_ptr = stage->rule_template_info_list[template_key->data - 1];
    } else  {
        temp_ptr = stage->policy_template_info_list[template_key->data - 1];
    }
    /*Scan through entries in the rule/policy temp list */
    while (temp_ptr) {
        /* Entry Id */
        key_field.id = entry_fid_list->entry_id_fid;
        if (lt_type == BCMFP_LT_TYPE_RULE)  {
            key_field.data = ((bcmfp_rule_template_info_t*)temp_ptr)->eid;
        } else  {
            key_field.data = ((bcmfp_policy_template_info_t*)temp_ptr)->eid;
        }
        /* Template Id */
        data_field.id = template_id_fid;
        data_field.data = key_val;
        /* Templates updated data value*/
        SHR_IF_ERR_EXIT(bcmfp_lt_entry_config_get(unit,
                                                  trans_id,
                                                  stage_id,
                                                  entry_lt_info,
                                                  entry_key,
                                                  entry_data,
                                                  template_id_fid,
                                                  template_data,
                                                  &entry_config));
        entry_id = key_field.data;
        if (event_reason == BCMIMM_ENTRY_DELETE)  {
            switch(lt_type) {
                case BCMFP_LT_TYPE_POLICY:
                    entry_config.policy_config.valid = FALSE;
                    SHR_IF_ERR_EXIT
                        (bcmfp_lt_entry_oprtnl_state_update(unit,
                                                    trans_id,
                                                    stage_id,
                                                    entry_id,
                                                    BCMFP_ENTRY_NO_POLICY_TEMP));
                    break;
                case BCMFP_LT_TYPE_RULE:
                    entry_config.rule_config.valid = FALSE;
                    SHR_IF_ERR_EXIT
                        (bcmfp_lt_entry_oprtnl_state_update(unit,
                                                      trans_id,
                                                      stage_id,
                                                      entry_id,
                                                      BCMFP_ENTRY_NO_RULE_TEMP));
                    break;
            }
        }

        gid = entry_config.group_config.group_id;
        SHR_IF_ERR_EXIT(bcmfp_group_oper_info_get(unit,
                                                  stage_id, gid,
                                                  &group_oper_info));

        /* Call to H/w for Configuration */
        if (entry_config.group_config.valid == TRUE)  {
            BCMFP_RET_VAL_ASSIGN
                (bcmfp_lt_entry_hw_insert(unit, trans_id, &entry_config));
            if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                SHR_IF_ERR_EXIT(bcmfp_lt_entry_oprtnl_state_delete(unit,
                                                                   trans_id,
                                                                   stage_id,
                                                                   entry_id));
                SHR_EXIT();
            }
            /* Update should not increase the ref_count */
            group_oper_info->ref_count--;

        } else {
            /*Currently the below part will not hit as the group config
             * is always true */
#if 0
            entry_id =  in.field[0]->data;
            BCMFP_RET_VAL_ASSIGN
                (bcmfp_lt_entry_oprtnl_state_get(unit,
                                                 stage_id,
                                                 entry_id,
                                                 &oper_state));
            if ((SHR_FUNC_VAL_IS(SHR_E_NONE) &&
                        oper_state == BCMFP_ENTRY_SUCCESS )) {
                SHR_IF_ERR_EXIT(bcmfp_lt_entry_hw_delete(unit,
                            trans_id,
                            stage_id,
                            ltid,
                            entry_id,
                            gid));
            }
#endif
        }
        if (lt_type == BCMFP_LT_TYPE_RULE)  {
            temp_ptr = ((bcmfp_rule_template_info_t*)temp_ptr)->next;
        } else  {
            temp_ptr = ((bcmfp_policy_template_info_t*)temp_ptr)->next;
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}




int
bcmfp_lt_entry_oprtnl_state_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t eid,
                                  uint32_t *oper_state)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_staged_entr_oper_state_t *entr_oper_state = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    entr_oper_state = stage->staged_entr_state;

    while (entr_oper_state) {
        if (entr_oper_state->eid == eid) {
            *oper_state = entr_oper_state->oper_state;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        entr_oper_state = entr_oper_state->next;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_oprtnl_state_update(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_id_t stage_id,
                                   uint32_t eid,
                                   uint32_t oper_state)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_staged_entr_oper_state_t *entr_oper_state = NULL;
    bcmfp_staged_entr_oper_state_t *new_entr_oper_state = NULL;

    SHR_FUNC_ENTER(unit);
    if (eid == 0) {
        SHR_IF_ERR_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    entr_oper_state = stage->staged_entr_state;

    /* Check if the oper_state of the entry exists*/
    while (entr_oper_state) {
        if (entr_oper_state->eid == eid) {
            entr_oper_state->oper_state = oper_state;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        entr_oper_state = entr_oper_state->next;
    }

    /*If the entry does not exists, add new */
    BCMFP_ALLOC(new_entr_oper_state,
                sizeof(bcmfp_staged_entr_oper_state_t),
                "BCMFP: staged entry oper_state");
    new_entr_oper_state->eid = eid;
    new_entr_oper_state->oper_state = oper_state;
    if (stage->staged_entr_state == NULL)  {
        stage->staged_entr_state = new_entr_oper_state;
    } else  {
        new_entr_oper_state->next = stage->staged_entr_state;
        stage->staged_entr_state = new_entr_oper_state;
    }


 exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_oprtnl_state_delete(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_id_t stage_id,
                                   uint32_t eid)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_staged_entr_oper_state_t *entr_oper_state = NULL;
    bcmfp_staged_entr_oper_state_t *prev_entr_oper_state = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->staged_entr_state == NULL)  {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /*Check if the HEAD has the entry oper_state*/
    if (stage->staged_entr_state->eid == eid) {
        entr_oper_state = stage->staged_entr_state;
        stage->staged_entr_state =  entr_oper_state->next;
        SHR_FREE(entr_oper_state);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    entr_oper_state = stage->staged_entr_state;
    /*Check the other nodes */
    prev_entr_oper_state = entr_oper_state;
    while (entr_oper_state) {
        if (entr_oper_state->eid == eid) {
            prev_entr_oper_state->next = entr_oper_state->next;
            SHR_FREE(entr_oper_state);
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        prev_entr_oper_state = entr_oper_state;
        entr_oper_state = entr_oper_state->next;
    }


 exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_oprtnl_state_clear(int unit,
                                  bcmfp_stage_id_t stage_id)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_staged_entr_oper_state_t *entr_oper_state = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->staged_entr_state == NULL)  {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    while (stage->staged_entr_state)  {
        entr_oper_state = stage->staged_entr_state;
        stage->staged_entr_state =  entr_oper_state->next;
        SHR_FREE(entr_oper_state);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_entry_oprtnl_state_insert(int unit,
                                   bcmfp_stage_id_t stage_id)
{
    bcmfp_stage_t *stage = NULL;
    uint32_t fid = 0;
    uint32_t offset = 0;
    uint32_t oper_state = 0;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    uint64_t data = 0;
    bcmltd_sid_t entry_ltid = 0;
    bcmltd_fields_t out = {0};
    uint32_t out_fields = 0;
    uint16_t add_count = 0;
    bcmfp_lt_info_t *entry_lt_info = NULL;
    bcmfp_lt_entry_sp_fids_t *entry_fid_list = NULL;
    bcmfp_staged_entr_oper_state_t *entr_oper_state = NULL;

    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case BCMFP_STAGE_ID_LOOKUP:
            entry_ltid = FP_VLAN_ENTRYt;
            break;
        case BCMFP_STAGE_ID_INGRESS:
            entry_ltid = FP_ING_ENTRYt;
            break;
        case BCMFP_STAGE_ID_EGRESS:
            entry_ltid = FP_EGR_ENTRYt;
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            entry_ltid = FP_EM_ENTRYt;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    in = &input;
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, entry_ltid, &input));
    out_fields = input.count;

    out.count = 0;
    out.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, entry_ltid, &out));
    out_fields = out.count;

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, entry_ltid, &entry_lt_info));
    entry_fid_list = (bcmfp_lt_entry_sp_fids_t *)entry_lt_info->sp_fids;

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    entr_oper_state = stage->staged_entr_state;

    /* Check if the oper_state of the entry exists*/
    while (entr_oper_state) {
        in = &input;
        input.field[0]->id = entry_fid_list->entry_id_fid;
        input.field[0]->data = entr_oper_state->eid;
        input.count = 1;
        /* Should look for the next entry in the SLL if the entry is not found
         * in the IMM */
        BCMFP_RET_VAL_ASSIGN
            (bcmimm_entry_lookup(unit,
                                 entry_ltid,
                                 in,
                                 &out));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            fid = entry_fid_list->oprtnl_state_fid;
            BCMFP_RET_VAL_ASSIGN
                (bcmfp_lt_field_data_get(unit, fid, &out, &data));
            if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                add_count = 1;
            }
            else {
                add_count = 2;
            }
            /* Get the staged operation status from the stage structure*/
            SHR_IF_ERR_EXIT
                (bcmfp_lt_entry_oprtnl_state_get(unit,
                                                 stage_id,
                                                 entr_oper_state->eid,
                                                 &oper_state));
            /*Update the in with KEY*/
            input.field[1]->id = fid;
            input.field[1]->data = oper_state;
            input.count = out.count + add_count;
            if (in != NULL) {
                for (offset = 2; offset < input.count; offset++) {
                    input.field[offset]->id = out.field[offset - add_count]->id;
                    input.field[offset]->idx = out.field[offset - add_count]->idx;
                    input.field[offset]->data = out.field[offset - add_count]->data;
                }
            }
            SHR_IF_ERR_EXIT(bcmimm_entry_update(unit,
                                                0,
                                                entry_ltid,
                                                in));
        }
        entr_oper_state = entr_oper_state->next;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

 exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &out);
    SHR_FUNC_EXIT();
}


int
bcmfp_lt_entry_validate(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_entry_event_t action,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data)
{
    bool group_valid = FALSE;
    uint32_t fid = 0;
    uint32_t eid = 0;
    uint32_t gid = 0;
    uint32_t old_gid = 0;
    uint32_t group_id =  0;
    bcmltd_sid_t group_ltid =  0;
    uint64_t data_val = 0;
    uint64_t key_val = 0;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *in = NULL;
    bcmltd_sid_t entry_ltid = 0;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_entry_sp_fids_t *fid_list = NULL;
    bcmfp_lt_group_sp_fids_t *group_fid_list = NULL;
    uint32_t out_fields = 0;
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    entry_ltid = sid;
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, entry_ltid, &lt_info));

    stage_id = lt_info->stage_id;
    fid_list = (bcmfp_lt_entry_sp_fids_t *) lt_info->sp_fids;
    fid = fid_list->entry_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, eid);
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, entry_ltid, &imm_fields));
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, entry_ltid, &input));
    out_fields = input.count;

    /* Restrict updating the group Id during update */
    if (action == BCMIMM_ENTRY_UPDATE)   {
        in = &input;
        input.field[0]->id = fid_list->entry_id_fid;
        input.field[0]->data = eid;
        input.count = 1;
        
        BCMFP_RET_VAL_ASSIGN(bcmimm_entry_lookup(unit,
                                                 entry_ltid,
                                                 in,
                                                 &imm_fields));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            fid = fid_list->group_id_fid;
            BCMFP_RET_VAL_ASSIGN
                    (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
            if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                group_valid = TRUE;
                BCMFP_UINT64_TO_UINT32(data_val, old_gid);
            }
        }
        if (group_valid == TRUE)  {
            fid = fid_list->group_id_fid;
            BCMFP_RET_VAL_ASSIGN
                    (bcmfp_lt_field_data_get_from_data_array(unit,
                                                             fid,
                                                             data,
                                                             &data_val));
            if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                BCMFP_UINT64_TO_UINT32(data_val, gid);
                if (gid != old_gid) {
                    SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
                }
            }
        }
    }
    /*Check if group exists */
    fid = fid_list->group_id_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, group_id);
        SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                                  stage_id,
                                                  BCMFP_LT_TYPE_GROUP,
                                                  &lt_info));
        group_ltid = lt_info->ltid;
        group_fid_list = (bcmfp_lt_group_sp_fids_t*) lt_info->sp_fids;
        
        input.field[0]->id = group_fid_list->group_id_fid;
        input.field[0]->data = group_id;
        input.count = 1;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_inmem_entry_exists(unit,
                                                         group_ltid,
                                                         &input));
        if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
    }
    
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}


