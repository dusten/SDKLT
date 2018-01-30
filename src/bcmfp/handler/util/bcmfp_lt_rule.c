/*! \file bcmfp_lt_rule.c
 *
 * APIs for LT rule template
 *
 * This file contains function definitions for rule LT template
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
#include <bcmfp/bcmfp_lt_rule.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmfp/map/bcmfp_lt_rule_map.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmfp/bcmfp_lt_entry.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*******************************************************************************
  Static Functions
 */
static int
bcmfp_lt_group_qual_inports_add(int unit,
                                uint32_t *field_val,
                                bcmfp_lt_rule_qual_map_t *rule_qual_map,
                                bcmfp_lt_rule_config_t *rule_config)
{
    uint32_t i = 0;
    SHR_FUNC_ENTER(unit);

    SHR_BIT_ITER(field_val, SHR_BITWID * BCMFP_MAX_WSIZE, i) {
        if (rule_qual_map->field_type == BCMFP_LT_RULE_FIELD_TYPE_DATA) {
            BCMDRD_PBMP_PORT_ADD(rule_config->inports_pbmp_data, i);
        } else {
            BCMDRD_PBMP_PORT_ADD(rule_config->inports_pbmp_mask, i);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_lt_rule_qual_add(int unit,
                       uint32_t trans_id,
                       uint32_t eid,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_lt_field_info_t *field_info,
                       bcmfp_lt_group_config_t *group_config,
                       bcmfp_lt_rule_qual_map_t *rule_qual_map,
                       uint32_t *field_val,
                       bcmfp_lt_rule_config_t *rule_config)
{
    uint32_t i = 0;
    uint32_t idx = 0;
    uint32_t width = 0;
    uint32_t data = 0;
    uint32_t mask = 0;
    bcmfp_lt_rule_data_t *rule_ptr = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    if (rule_qual_map->qual == BCMFP_QUAL_INPORTS) {
        SHR_IF_ERR_EXIT(bcmfp_lt_group_qual_inports_add(unit,
                            field_val, rule_qual_map, rule_config));
    }

    if (!BCMFP_QSET_TEST(group_config->qset, rule_qual_map->qual)) {
        /* This qual is not part of group qset, fail */
        SHR_IF_ERR_EXIT
            (bcmfp_lt_entry_oprtnl_state_update(
                                  unit, trans_id, stage_id, eid,
                                  BCMFP_ENTRY_RULE_QSET_NOT_IN_GRP));

        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    width = field_info->width;
    idx = (width - 1) >> 5;

    /* search for Rule data */
    rule_ptr = rule_config->data;
    while (rule_ptr != NULL) {
        if (rule_ptr->qual == rule_qual_map->qual) {
            break;
        }
        rule_ptr = rule_ptr->next;
    }

    /* Not found Create New Node */
    if (rule_ptr == NULL) {
        BCMFP_ALLOC(rule_ptr, sizeof(bcmfp_lt_rule_data_t), "rule node");
        if (rule_config->data == NULL) {
            rule_config->data = rule_ptr;
        } else {
            /* Adding as first node */
            rule_ptr->next = rule_config->data;
            rule_config->data = rule_ptr;
        }
    }

    rule_ptr->qual = rule_qual_map->qual;

    if (field_info->is_symbol) {
        SHR_IF_ERR_EXIT
            (bcmfp_qualifier_enum_set(unit,
                                      stage_id,
                                      rule_ptr->qual,
                                      field_val[0],
                                      &data,
                                      &mask));
        rule_ptr->data[0] = data;
        rule_ptr->mask[0] = mask;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (BCMFP_QSET_TEST(group_config->partial_qset, rule_ptr->qual)) {
        /* Get Partial bitmap node in group_config */
        qual_bitmap = group_config->qual_bitmap;
        while (qual_bitmap != NULL) {
            if (qual_bitmap->qual == rule_ptr->qual) {
                break;
            }
            qual_bitmap = qual_bitmap->next;
        }

        /* This should not happen */
        if (qual_bitmap == NULL) {
            return SHR_E_FAIL;
        }
        for (i = 0; i <= idx; i++) {
            /* Qualifier mask in FP_XXX_RULE_TEMPLATE LT must not contain bits
             * set which are unset in the qualifier bitmap in
             * FP_XXX_GRP_TEMPLATE LT.
             */
            if (rule_qual_map->field_type == BCMFP_LT_RULE_FIELD_TYPE_MASK) {
                if ((field_val[i] | qual_bitmap->bitmap[i])
                            != qual_bitmap->bitmap[i]) {
                    SHR_IF_ERR_EXIT
                        (bcmfp_lt_entry_oprtnl_state_update(
                                    unit, trans_id, stage_id, eid,
                                    BCMFP_ENTRY_RULE_QSET_WIDTH_EXCEEDS));

                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
                }
            }
        }
        sal_memcpy(rule_ptr->bitmap,
                   qual_bitmap->bitmap,
                   sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    }

    if (rule_qual_map->field_type == BCMFP_LT_RULE_FIELD_TYPE_DATA) {
        sal_memcpy(rule_ptr->data, field_val, BCMFP_MAX_WSIZE * sizeof(uint32_t));
    } else {
        sal_memcpy(rule_ptr->mask, field_val, BCMFP_MAX_WSIZE * sizeof(uint32_t));
    }

exit:

    SHR_FUNC_EXIT();
}


int
bcmfp_lt_rule_config_free(int unit,
                          bcmfp_lt_rule_config_t *rule_config)
{
    bcmfp_lt_rule_data_t *temp_ptr = NULL;
    SHR_FUNC_ENTER(unit);

    temp_ptr = rule_config->data;

    while(temp_ptr != NULL) {
        temp_ptr = temp_ptr->next;
        SHR_FREE(rule_config->data);
        rule_config->data = temp_ptr;
    }

    rule_config->valid = FALSE;

    SHR_FUNC_EXIT();
}


int
bcmfp_lt_rule_config_get(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         uint32_t rule_id,
                         uint32_t eid,
                         const bcmltd_field_t *template_data,
                         bcmfp_lt_group_config_t *group_config,
                         bcmfp_lt_rule_config_t *rule_config)
{
    uint32_t i = 0;
    uint32_t fid = 0;
    uint32_t idx = 0;
    uint32_t incr = 0;
    uint32_t out_fields = 0;
    uint32_t field_val[BCMFP_MAX_WSIZE];
    uint64_t data_val = 0;
    uint64_t data_def = 0;
    bcmltd_sid_t rule_ltid = 0;
    bcmltd_fields_t imm_fields;
    bcmltd_fields_t input;
    bcmltd_fields_t *in;
    bcmfp_lt_field_info_t *field_info = NULL;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_rule_sp_fids_t *fid_list = NULL;
    bcmfp_lt_rule_qual_map_t rule_qual_map;
    const bcmltd_field_t *data = template_data;
    bcmfp_qset_t qual_fid_list;

    SHR_FUNC_ENTER(unit);

    sal_memset(&qual_fid_list, 0, sizeof(bcmfp_qset_t));
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_RULE,
                                              &lt_info));

    rule_ltid = lt_info->ltid;
    fid_list = (bcmfp_lt_rule_sp_fids_t *) lt_info->sp_fids;

    imm_fields.count = 0;
    imm_fields.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, rule_ltid, &imm_fields));
    out_fields = imm_fields.count;

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, rule_ltid, &input));
    out_fields = input.count;

    in = &input;
    input.field[0]->id = fid_list->rule_id_fid;
    input.field[0]->data = rule_id;
    input.count = 1;

    while(data) {
        fid = data->id;
        idx = data->idx;
        SHR_BITSET(qual_fid_list.w, fid);
        SHR_IF_ERR_EXIT(bcmfp_lt_field_info_get(unit, fid, lt_info, &field_info));

        /* Continue if the value of field is default */
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

        BCMFP_RET_VAL_ASSIGN(lt_info->map_get(unit, fid, idx, (void *) &rule_qual_map));
        if (SHR_FUNC_ERR()) {
            data = data->next;
            continue;
        }

        sal_memset(&field_val[0], 0, sizeof(field_val));
        if (field_info->is_array == TRUE) {
            /* Each array Index has different qualifier. */
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
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_rule_qual_add(unit,
                                                    trans_id,
                                                    eid,
                                                    stage_id,
                                                    field_info,
                                                    group_config,
                                                    &rule_qual_map,
                                                    field_val,
                                                    rule_config));
        if (SHR_FUNC_ERR()) {
            sal_memset(rule_config, 0, sizeof(bcmfp_lt_rule_config_t));
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        if (data) {
            data = data->next;
        }
        rule_config->valid = TRUE;
    }

    BCMFP_RET_VAL_ASSIGN(bcmimm_entry_lookup(unit,
                                             rule_ltid,
                                             in,
                                             &imm_fields));
    if (SHR_FUNC_ERR() && (template_data == NULL)) {
        SHR_IF_ERR_EXIT(bcmfp_lt_entry_oprtnl_state_update(unit,
                                                           trans_id,
                                                           stage_id,
                                                           eid,
                                                           BCMFP_ENTRY_NO_RULE_TEMP));
        rule_config->valid = FALSE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    rule_config->ltid = rule_ltid;
    rule_config->rule_id = rule_id;
    rule_config->stage_id = stage_id;

    i = 0;
    while (i < imm_fields.count) {
        fid = imm_fields.field[i]->id;
        idx = imm_fields.field[i]->idx;
        if (SHR_BITGET(qual_fid_list.w, fid)) {
            i++;
            continue;
        }
        SHR_IF_ERR_EXIT(bcmfp_lt_field_info_get(unit, fid, lt_info, &field_info));
        
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
        BCMFP_RET_VAL_ASSIGN(lt_info->map_get(unit, fid, idx, (void *) &rule_qual_map));
        if (SHR_FUNC_ERR()) {
            i++;
            continue;
        }

        sal_memset(&field_val[0], 0, sizeof(field_val));
        if (field_info->is_array == TRUE) {
            /* Each array Index has different qualifier. */
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
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_rule_qual_add(unit,
                                                    trans_id,
                                                    eid,
                                                    stage_id,
                                                    field_info,
                                                    group_config,
                                                    &rule_qual_map,
                                                    field_val,
                                                    rule_config));
        if (SHR_FUNC_ERR()) {
            sal_memset(rule_config, 0, sizeof(bcmfp_lt_rule_config_t));
            rule_config->valid = FALSE;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        /* Increment i. */
        i += incr;
    }

    fid = fid_list->qual_inports_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        if (data_val != 0)  {
            rule_config->is_qual_inports = TRUE;
        } else {
            rule_config->is_qual_inports = FALSE;
        }
    }

    rule_config->valid = TRUE;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        bcmfp_lt_rule_config_free(unit, rule_config);
    }

    (void)bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    (void)bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_rule_template_info_delete(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage,
                                   uint32_t rule_id,
                                   bcmfp_entry_id_t eid)
{
    bcmfp_rule_template_info_t *rule_entry_ptr = NULL;
    bcmfp_rule_template_info_t *prev_rule_entry_ptr = NULL;
    SHR_FUNC_ENTER(unit);

    /*Reset the rule Id in rule bit list */
    if(stage->rule_template_info_list[rule_id - 1] != NULL)  {
        rule_entry_ptr = stage->rule_template_info_list[rule_id - 1];
        /*If the head has the entry*/
        if (stage->rule_template_info_list[rule_id - 1]->eid == eid)  {
            stage->rule_template_info_list[rule_id - 1] = stage->rule_template_info_list[rule_id - 1]->next;
            SHR_FREE(rule_entry_ptr);
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else  {
            prev_rule_entry_ptr = rule_entry_ptr;
            while(rule_entry_ptr)  {
                if (rule_entry_ptr->eid == eid)  {
                    prev_rule_entry_ptr->next = rule_entry_ptr->next;
                    SHR_FREE(rule_entry_ptr);
                    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
                }
                prev_rule_entry_ptr = rule_entry_ptr;
                rule_entry_ptr = rule_entry_ptr->next;
            }
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_lt_rule_template_info_insert(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage,
                                   uint32_t rule_id,
                                   bcmfp_entry_id_t eid)
{
    bcmfp_rule_template_info_t *rule_entry_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /*Add NODE with entry Id to the entry list*/
    BCMFP_ALLOC(rule_entry_ptr,
            sizeof(bcmfp_rule_template_info_t),
            "BCMFP: rule template entry");
    rule_entry_ptr->eid = eid;
    rule_entry_ptr->next = NULL;

    if(stage->rule_template_info_list[rule_id - 1] == NULL)  {
        stage->rule_template_info_list[rule_id - 1] = rule_entry_ptr;
    } else  {
        rule_entry_ptr->next = stage->rule_template_info_list[rule_id - 1];
        stage->rule_template_info_list[rule_id - 1] = rule_entry_ptr;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

