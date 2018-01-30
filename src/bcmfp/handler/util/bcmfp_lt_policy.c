/*! \file bcmfp_lt_policy.c
 *
 * APIs for LT policy template
 *
 * This file contains function definitions for policy LT template
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
#include <bcmfp/bcmfp_lt_policy.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmfp/map/bcmfp_lt_policy_map.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmfp/bcmfp_lt_entry.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*******************************************************************************
  Includes
 */
static int
bcmfp_lt_policy_action_add(int unit,
                           bcmfp_lt_field_info_t *field_info,
                           bcmfp_lt_policy_action_map_t *policy_action_map,
                           uint32_t *field_val,
                           bcmfp_lt_policy_config_t *policy_config)
{
    bcmfp_lt_policy_data_t *policy_data = NULL;
    SHR_FUNC_ENTER(unit);

    /* Allocate New Node */
    BCMFP_ALLOC(policy_data, sizeof(bcmfp_lt_policy_data_t), "policy node");
    if (policy_config->data == NULL) {
        policy_config->data = policy_data;
    } else {
        /* Adding as first node */
        policy_data->next = policy_config->data;
        policy_config->data = policy_data;
    }

    policy_data->action = policy_action_map->action;
    policy_data->param = *field_val;

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_policy_validate(int unit,
                         bcmltd_sid_t sid,
                         bcmimm_entry_event_t action,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data)
{
    uint64_t key_val = 0;
    uint64_t data_val = 0;
    uint32_t out_fields = 0;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_policy_sp_fids_t *fid_list = NULL;
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_t action1;
    bcmfp_action_t action2;
    int32_t max_policy_count = 0;
    int32_t policy_fid = 0;
    uint32_t start_policy_fid  = 0;
    uint32_t fid = 0;
    uint32_t idx = 0;
    uint32_t policy_id = 0;
    bcmfp_aset_t actions_bit_list;
    bcmfp_lt_policy_action_map_t policy_map;

    SHR_FUNC_ENTER(unit);

    sal_memset(&actions_bit_list, 0, sizeof(bcmfp_aset_t));
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));

    fid_list = (bcmfp_lt_policy_sp_fids_t *) lt_info->sp_fids;


    /*Get the key value from the key array */
    fid = fid_list->policy_id_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_lt_field_key_get_from_key_array(unit, fid, key, &key_val));

    BCMFP_UINT64_TO_UINT32(key_val, policy_id);

    if ((action == BCMIMM_ENTRY_DELETE) ||
        (action == BCMIMM_ENTRY_UPDATE)) {

        fid_list = (bcmfp_lt_policy_sp_fids_t *) lt_info->sp_fids;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_lt_bcmltd_fields_buff_alloc(unit, sid, &imm_fields));

        out_fields = imm_fields.count;
        input.count = 0;
        input.field = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_lt_bcmltd_fields_buff_alloc(unit, sid, &input));
        out_fields = input.count;

        BCMFP_UINT64_TO_UINT32(key_val, policy_id);
        in = &input;
        input.field[0]->id = fid;
        input.field[0]->data = policy_id;
        input.count = 1;

        BCMFP_RET_VAL_ASSIGN
            (bcmimm_entry_lookup(unit, sid, in, &imm_fields));
    }

    start_policy_fid = fid_list->policy_id_fid + 1;
    max_policy_count = fid_list->policy_fid_count;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, lt_info->stage_id, &stage));

    if ((action == BCMIMM_ENTRY_INSERT)||
        (action == BCMIMM_ENTRY_UPDATE))  {
        /* Make a list of actions from input as well as in-memory*/
        for (policy_fid = start_policy_fid;
            policy_fid < max_policy_count;
            policy_fid++) {
            BCMFP_RET_VAL_ASSIGN
                (bcmfp_lt_field_data_get_from_data_array(unit,
                                 policy_fid, data, &data_val));

            
            if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data_val) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (lt_info->map_get(unit, policy_fid,
                                      idx, (void *) &policy_map));
                SHR_BITSET(actions_bit_list.w, policy_map.action);
            } else if (!SHR_FUNC_VAL_IS(SHR_E_NONE) &&
                       (action == BCMIMM_ENTRY_UPDATE))  {
                BCMFP_RET_VAL_ASSIGN
                    (bcmfp_lt_field_data_get(unit, policy_fid,
                                     &imm_fields, &data_val));
                if (SHR_FUNC_VAL_IS(SHR_E_NONE) && data_val) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (lt_info->map_get(unit, policy_fid,
                                          idx, (void *) &policy_map));
                    SHR_BITSET(actions_bit_list.w, policy_map.action);
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }
    }

    /* Check for conflicting actions in the list */
    for (action1 = 0; action1 < BCMFP_ACTION_COUNT - 1; action1++) {
        if (SHR_BITGET(actions_bit_list.w, action1)) {
            for (action2 = action1 + 1;
                 action2 < BCMFP_ACTION_COUNT; action2++)  {
                if (SHR_BITGET(actions_bit_list.w, action2)) {
                    BCMFP_RET_VAL_ASSIGN
                        (bcmfp_action_conflict_check(unit, stage,
                                               action1, action2));
                    if (SHR_FUNC_VAL_IS(SHR_E_CONFIG)) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "BCMFP:%s action is"
                             "conflicting with %s action in the"
                             "policy template id %d\n"),
                            bcmfp_action_string(action1),
                            bcmfp_action_string(action2),
                            policy_id));
                        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
                    }
                    if (SHR_FUNC_ERR()) {
                        BCMFP_RET_VAL_EXIT();
                    }
                }
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
bcmfp_lt_policy_config_free(int unit,
                            bcmfp_lt_policy_config_t *policy_config)
{
    bcmfp_lt_policy_data_t *temp_ptr = NULL;
    SHR_FUNC_ENTER(unit);

    temp_ptr = policy_config->data;

    while(temp_ptr != NULL) {
        temp_ptr = temp_ptr->next;
        SHR_FREE(policy_config->data);
        policy_config->data = temp_ptr;
    }

    policy_config->valid = FALSE;

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_policy_config_get(int unit,
                           uint32_t trans_id,
                           bcmfp_stage_id_t stage_id,
                           uint32_t policy_id,
                           uint32_t eid,
                           const bcmltd_field_t *template_data,
                           bcmfp_lt_policy_config_t *policy_config)
{
    uint32_t i = 0;
    uint32_t fid = 0;
    uint32_t idx = 0;
    uint32_t incr = 0;
    uint64_t data_def = 0;
    uint32_t out_fields = 0;
    uint32_t field_val[BCMFP_MAX_WSIZE];
    bcmltd_sid_t policy_ltid = 0;
    bcmltd_fields_t imm_fields;
    bcmltd_fields_t input;
    bcmltd_fields_t *in;
    bcmfp_lt_field_info_t *field_info = NULL;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_policy_action_map_t policy_act_map;
    bcmfp_lt_policy_sp_fids_t *fid_list = NULL;
    const bcmltd_field_t *data = template_data;
    bcmfp_aset_t action_fid_list;

    SHR_FUNC_ENTER(unit);

    sal_memset(&action_fid_list, 0, sizeof(bcmfp_aset_t));
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_POLICY,
                                              &lt_info));

    policy_ltid = lt_info->ltid;
    fid_list = (bcmfp_lt_policy_sp_fids_t *) lt_info->sp_fids;


    imm_fields.count = 0;
    imm_fields.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, policy_ltid, &imm_fields));

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, policy_ltid, &input));
    out_fields = input.count;


    policy_config->ltid = policy_ltid;
    policy_config->policy_id = policy_id;

    while(data) {
        fid = data->id;
        idx = data->idx;
        SHR_BITSET(action_fid_list.w, fid);

        SHR_IF_ERR_EXIT
            (bcmfp_lt_field_info_get(unit, fid, lt_info, &field_info));

        
        if (field_info->dtag != BCMLT_FIELD_DATA_TAG_RAW) {
            data_def = field_info->def.u64;
        } else {
            data_def = 0;
        }
        if (data->data == data_def)
        {
            data = data->next;
            continue;
        }
        BCMFP_RET_VAL_ASSIGN
            (lt_info->map_get(unit, fid, idx, (void *) &policy_act_map));
        if (SHR_FUNC_ERR()) {
            data = data->next;
            continue;
        }

        sal_memset(&field_val[0], 0, sizeof(field_val));
        if (field_info->is_array == TRUE) {
            /* Each array Index has different action. */
            incr = 1;
        } else {
            incr = field_info->elements;
        }
        while(incr--)  {
            SHR_IF_ERR_EXIT(bcmfp_lt_field_data_to_uint32_array(unit,
                                                                (bcmltd_field_t*)data,
                                                                field_info,
                                                                field_val));
            if (incr) {
                data = data->next;
            }
        }
        SHR_IF_ERR_EXIT(bcmfp_lt_policy_action_add(unit,
                                                   field_info,
                                                   &policy_act_map,
                                                   field_val,
                                                   policy_config));
        if(data)  {
            data = data->next;
        }
        policy_config->valid = TRUE;
    }

    in = &input;
    input.field[0]->id =fid_list->policy_id_fid;
    input.field[0]->data = policy_id;
    input.count = 1;
    BCMFP_RET_VAL_ASSIGN(bcmimm_entry_lookup(unit,
                                             policy_ltid,
                                             in,
                                             &imm_fields));

    if (SHR_FUNC_ERR() && (template_data == NULL)) {
        SHR_IF_ERR_EXIT(bcmfp_lt_entry_oprtnl_state_update(unit,
                                                           trans_id,
                                                           stage_id,
                                                           eid,
                                                           BCMFP_ENTRY_NO_POLICY_TEMP));
        policy_config->valid = FALSE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    i = 0;
    while (i < imm_fields.count) {
        fid = imm_fields.field[i]->id;
        idx = imm_fields.field[i]->idx;
        if (SHR_BITGET(action_fid_list.w, fid)) {
            i++;
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmfp_lt_field_info_get(unit, fid, lt_info, &field_info));

        
        if (field_info->dtag != BCMLT_FIELD_DATA_TAG_RAW) {
            data_def = field_info->def.u64;
        } else {
            data_def = 0;
        }

        if (imm_fields.field[i]->data == data_def)
        {
            i++;
            continue;
        }

        BCMFP_RET_VAL_ASSIGN
            (lt_info->map_get(unit, fid, idx, (void *) &policy_act_map));
        if (SHR_FUNC_ERR()) {
            i++;
            continue;
        }

        sal_memset(&field_val[0], 0, sizeof(field_val));
        if (field_info->is_array == TRUE) {
            /* Each array Index has different action. */
            incr = 1;
        } else {
            incr = field_info->elements;
        }
        for (idx = 0; idx < incr; idx++) {
            SHR_IF_ERR_EXIT(bcmfp_lt_field_data_to_uint32_array(unit,
                                                                imm_fields.field[i + idx],
                                                                field_info,
                                                                field_val));
        }
        SHR_IF_ERR_EXIT(bcmfp_lt_policy_action_add(unit,
                                                   field_info,
                                                   &policy_act_map,
                                                   field_val,
                                                   policy_config));
        /* Increment i. */
        i += incr;
    }

    policy_config->valid = TRUE;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        bcmfp_lt_policy_config_free(unit, policy_config);
    }

    bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_policy_template_info_delete(int unit,
                                     uint32_t trans_id,
                                     bcmfp_stage_t *stage,
                                     uint32_t policy_id,
                                     bcmfp_entry_id_t eid)
{
    bcmfp_policy_template_info_t *policy_entry_ptr = NULL;
    bcmfp_policy_template_info_t *prev_policy_entry_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /*Reset the policy Id in policy bit list */
    if(stage->policy_template_info_list[policy_id - 1] != NULL)  {
        policy_entry_ptr = stage->policy_template_info_list[policy_id - 1];
        /*If the head has the entry*/
        if (stage->policy_template_info_list[policy_id - 1]->eid == eid)  {
            stage->policy_template_info_list[policy_id - 1] = stage->policy_template_info_list[policy_id - 1]->next;
            SHR_FREE(policy_entry_ptr);
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else  {
            prev_policy_entry_ptr = policy_entry_ptr;
            while(policy_entry_ptr)  {
                if (policy_entry_ptr->eid == eid)  {
                    prev_policy_entry_ptr->next = policy_entry_ptr->next;
                    SHR_FREE(policy_entry_ptr);
                    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
                }
                prev_policy_entry_ptr = policy_entry_ptr;
                policy_entry_ptr = policy_entry_ptr->next;
            }
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_policy_template_info_insert(int unit,
                                     uint32_t trans_id,
                                     bcmfp_stage_t *stage,
                                     uint32_t policy_id,
                                     bcmfp_entry_id_t eid)
{
    bcmfp_policy_template_info_t *policy_entry_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /*Add NODE with entry Id to the entry list*/
    BCMFP_ALLOC(policy_entry_ptr,
            sizeof(bcmfp_policy_template_info_t),
            "BCMFP: policy template entry");
    policy_entry_ptr->eid = eid;
    policy_entry_ptr->next = NULL;

    if (stage->policy_template_info_list[policy_id - 1] == NULL)  {
        stage->policy_template_info_list[policy_id - 1] = policy_entry_ptr;
    } else  {
        policy_entry_ptr->next = stage->policy_template_info_list[policy_id - 1];
        stage->policy_template_info_list[policy_id - 1] = policy_entry_ptr;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}
