/*! \file bcmfp_lt_group.c
 *
 * APIs for LT group interface
 *
 * This file contains function definitions for group LT template.
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
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_group.h>
#include <bcmfp/bcmfp_lt_policy.h>
#include <bcmfp/map/bcmfp_lt_group_map.h>
#include <bcmfp/map/bcmfp_lt_src_class_mode_map.h>
#include <bcmfp/map/bcmfp_lt_presel_entry_map.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmimm/bcmimm_basic.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*******************************************************************************
  Static functions
 */

static int
bcmfp_lt_group_qual_bitmap_add(int unit,
                               bcmfp_qual_t qual,
                               uint32_t *field_val,
                               bcmfp_lt_group_config_t *group_config)
{
    bcmfp_qual_bitmap_t *node = NULL;
    SHR_FUNC_ENTER(unit);

    BCMFP_ALLOC(node, sizeof(bcmfp_qual_bitmap_t), "qual bitmap");
    node->qual = qual;
    sal_memcpy(node->bitmap, field_val, sizeof(uint32_t) * BCMFP_MAX_WSIZE);
    node->next = NULL;

    /* Add to qual bitmap list */
    if (group_config->qual_bitmap == NULL) {
        group_config->qual_bitmap = node;
    } else {
        node->next =  group_config->qual_bitmap;
        group_config->qual_bitmap = node;
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(node);
    }

    SHR_FUNC_EXIT();
}

static int
bcmfp_lt_group_qual_add(int unit,
                        bcmfp_lt_field_info_t *field_info,
                        bcmfp_lt_group_qual_map_t *group_qual_map,
                        uint32_t *field_val,
                        bcmfp_lt_group_config_t *group_config)
{
    bool is_partial = FALSE;
    uint32_t i = 0;
    uint32_t idx = 0;
    uint32_t width = 0;
    uint32_t mask = 0;
    uint32_t all_ones = 0xFFFFFFFF;
    SHR_FUNC_ENTER(unit);

    /* Add to Qset */
    if (group_qual_map->qual == BCMFP_QUAL_SRC_IP6) {
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_SRC_IP6_UPPER);
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_SRC_IP6_LOWER);
    } else if (group_qual_map->qual == BCMFP_QUAL_DST_IP6) {
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_DST_IP6_UPPER);
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_DST_IP6_LOWER);
    } else if (group_qual_map->qual == BCMFP_QUAL_DST_IP6) {
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_DST_IP6_UPPER);
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_DST_IP6_LOWER);
    } else if (group_qual_map->qual == BCMFP_QUAL_INNER_SRC_IP6) {
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_INNER_SRC_IP6_UPPER);
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_INNER_SRC_IP6_LOWER);
    } else if (group_qual_map->qual == BCMFP_QUAL_INNER_DST_IP6) {
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_INNER_DST_IP6_UPPER);
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_INNER_DST_IP6_LOWER);
    } else if (group_qual_map->qual == BCMFP_QUAL_INNER_DST_IP6) {
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_INNER_DST_IP6_UPPER);
        BCMFP_QSET_ADD(group_config->qset, BCMFP_QUAL_INNER_DST_IP6_LOWER);
    } else  {
        BCMFP_QSET_ADD(group_config->qset, group_qual_map->qual);
    }
     if (field_info->is_symbol) {
         SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Check for partial bitmap */
    width = field_info->width;
    idx = (width - 1) >> 5;
    width = width % 32;

    /* Checking first to second last words.  */
    for (i = 0; i < idx; i++) {
        if ((field_val[i] & all_ones) != all_ones) {
            is_partial = TRUE;
            break;
        }
    }

    /* Checking last word. */
    if (is_partial == FALSE) {
        if (width == 0) {
            mask = all_ones;
        } else {
            mask = ((1 << width) - 1);
        }

        /* Last remaining parts is at index=idx. */
        if ((field_val[idx] & mask) != mask) {
            is_partial = TRUE;
        }
    }

    /* Making Partial bitmap */
    if (is_partial) {
        BCMFP_QSET_ADD(group_config->partial_qset, group_qual_map->qual);
        SHR_IF_ERR_EXIT(bcmfp_lt_group_qual_bitmap_add(unit,
                            group_qual_map->qual, field_val, group_config));
    }

exit:

    SHR_FUNC_EXIT();
}


static int
bcmfp_lt_group_action_add(int unit,
                        bcmfp_lt_group_qual_map_t *group_qual_map,
                        bcmfp_lt_group_config_t *group_config)
{
    SHR_FUNC_ENTER(unit);

    /* Add to Aset */
    BCMFP_ASET_ADD(group_config->aset, group_qual_map->action);
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}


int
bcmfp_lt_group_validate(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_entry_event_t action,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data)
{
    bool has_mode_auto = FALSE;
    bool mode_auto = FALSE;
    bool has_mode = FALSE;
    uint32_t presel_id = 0;
    uint32_t mode = 0;
    uint32_t i = 0;
    uint32_t group_id = 0;
    uint64_t key_val = 0;
    uint64_t data_val = 0;
    uint32_t fid = 0;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_lt_presel_entry_sp_fids_t *presel_fid_list = NULL;
    bcmltd_fields_t grp_imm_fields = {0};
    bcmltd_fields_t presel_imm_fields = {0};
    bcmltd_fields_t input = {0};
    uint32_t grp_out_fields = 0;
    uint32_t presel_out_fields = 0;
    bcmltd_fields_t *in = NULL;
    bcmltd_sid_t group_ltid = 0;
    bcmltd_field_t presel_set[BCMFP_GROUP_MAX_PRESEL];
    bcmltd_field_t *presel_fields[BCMFP_GROUP_MAX_PRESEL];
    bcmltd_fields_t multi_fields;
    bcmltd_field_t inmem_presel_set[BCMFP_GROUP_MAX_PRESEL];
    bcmltd_field_t *inmem_presel_fields[BCMFP_GROUP_MAX_PRESEL];
    bcmltd_fields_t inmem_multi_fields = {0};

    SHR_FUNC_ENTER(unit);

    if (action == BCMIMM_ENTRY_LOOKUP) {
        /* Ignore other fields */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;

    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, sid, &grp_imm_fields));

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit,
                                                      sid,
                                                      &input));
    grp_out_fields = input.count;

    if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID) {
        SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit,
                                                          fid_list->presel_lt_sid,
                                                          &presel_imm_fields));
        presel_out_fields = presel_imm_fields.count;
    }
    /*Get the key value from the key array */
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);
    in = &input;

    /* Automode not valid for EFP */
    if (lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP)  {
        BCMFP_RET_VAL_ASSIGN(bcmfp_fixed_key_group_validate(unit,
                                                            action,
                                                            lt_info,
                                                            key,
                                                            data));
        if (SHR_FUNC_VAL_IS(SHR_E_CONFIG)) {
            SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
        }
    }  else {
        fid = fid_list->mode_auto_fid;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                     fid,
                                                                     data,
                                                                     &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            has_mode_auto = TRUE;
            BCMFP_UINT64_TO_BOOL(data_val, mode_auto);
        }
        fid = fid_list->mode_fid;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                     fid,
                                                                     data,
                                                                     &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, mode);
            has_mode = TRUE;
        }
    }

    if (action == BCMIMM_ENTRY_INSERT) {
        /* if auto_mode is not present or FALSE, Mode Must be present */
        if (!(lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP)) {
            if ((!(has_mode_auto && mode_auto)) &&
                ((has_mode == FALSE) || (mode == 0))) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
    } else { /* action == BCMIMM_ENTRY_UPDATE */
        /* Validate Group Mode */
        if (!(lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP)) {
            if ((has_mode_auto) || (has_mode)) {
                if ((!(has_mode_auto && mode_auto)) &&
                        ((has_mode == FALSE) || (mode == 0))) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
            }
        }

    }
    /*To restrict two VFP groups having same priority*/
    if (lt_info->flags & BCMFP_LT_FLAGS_UNIQUE_GRP_PRI) {
        BCMFP_RET_VAL_ASSIGN(bcmfp_group_priority_validate(unit,
                                                           action,
                                                           lt_info,
                                                           key,
                                                           data));
        if (SHR_FUNC_VAL_IS(SHR_E_CONFIG)) {
            SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
        }
    }


    if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID) {
        /* Verifying Presels. */
        fid = fid_list->presel_ids_set_fid;
        /* Get group info from in memory to check for duplicates */
        if (action == BCMIMM_ENTRY_UPDATE) {
            group_ltid = sid;

            input.field[0]->id = fid_list->group_id_fid;
            input.field[0]->data = group_id;
            input.count = 1;

            SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                                group_ltid,
                                                in,
                                                &grp_imm_fields));
            for (i = 0; i < BCMFP_GROUP_MAX_PRESEL; i++) {
                inmem_presel_fields[i] = &inmem_presel_set[i];
            }
            inmem_multi_fields.count = BCMFP_GROUP_MAX_PRESEL;
            inmem_multi_fields.field = inmem_presel_fields;
            BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_multi_data_get(unit,
                        sid, fid, &grp_imm_fields, &inmem_multi_fields));

            /* In case Presel was not set earlier */
            if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                inmem_multi_fields.count = 0;
            }
        }

        for (i = 0; i < BCMFP_GROUP_MAX_PRESEL; i++) {
            presel_fields[i] = &presel_set[i];
        }
        multi_fields.count = BCMFP_GROUP_MAX_PRESEL;
        multi_fields.field = presel_fields;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_multi_data_get_from_data_array(unit,
                    fid, data, &multi_fields));

        SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, fid_list->presel_lt_sid, &lt_info));
        presel_fid_list = (bcmfp_lt_presel_entry_sp_fids_t*) lt_info->sp_fids;

        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            /* Fail if duplicate Presel Entry in List is present */
            
            SHR_IF_ERR_EXIT
                (bcmfp_lt_field_duplicate_check(unit, &multi_fields,
                                                &inmem_multi_fields));

            for (i = 0; i < multi_fields.count; i++) {
                if (presel_set[i].data != 0)  {
                    BCMFP_UINT64_TO_UINT32(presel_set[i].data, presel_id);

                    input.field[0]->id = presel_fid_list->presel_id_fid;
                    input.field[0]->data = presel_id;
                    input.count = 1;
                   /* Reset the out fields count */
                    presel_imm_fields.count = presel_out_fields;
                    BCMFP_RET_VAL_ASSIGN(bcmimm_entry_lookup(unit,
                                                        fid_list->presel_lt_sid,
                                                        in,
                                                        &presel_imm_fields));
                    if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                    }
                }
            }
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, grp_out_fields,
                                            &grp_imm_fields);
    if (lt_info)  {
        if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID) {
            (void) bcmfp_lt_bcmltd_fields_buff_free(unit, presel_out_fields,
                                                    &presel_imm_fields);
        }
    }
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, grp_out_fields, &input);

    SHR_FUNC_EXIT();
}



int
bcmfp_lt_group_config_free(int unit,
                           bcmfp_lt_group_config_t *group_config)
{
    bcmfp_qual_bitmap_t *temp_ptr = NULL;
    SHR_FUNC_ENTER(unit);

    if (group_config->qual_bitmap != NULL)  {
        temp_ptr = group_config->qual_bitmap;

        while(temp_ptr != NULL) {
            temp_ptr = temp_ptr->next;
            SHR_FREE(group_config->qual_bitmap);
            group_config->qual_bitmap = temp_ptr;
        }
    }
    group_config->qual_bitmap = NULL;

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_ingress_grp_config_insert(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t sid,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data)
{
    uint32_t i = 0;
    uint64_t data_val = 0;
    uint32_t src_class_mask = 0;
    uint32_t fid = 0;
    uint32_t presel_id = 0;
    uint32_t in_pipe_id = 0;
    bool increment_flag = FALSE;
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t input = {0};
    uint32_t out_fields = 0;
    bcmltd_fields_t *in = NULL;
    bcmfp_stage_id_t stage_id;
    bcmfp_stage_oper_mode_t oper_mode;
    bcmltd_field_t presel_set[BCMFP_GROUP_MAX_PRESEL];
    bcmltd_field_t *presel_fields[BCMFP_GROUP_MAX_PRESEL];
    bcmltd_fields_t multi_fields;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_lt_presel_entry_sp_fids_t *presel_fid_list = NULL;
    bcmfp_lt_src_class_mode_sp_fids_t *src_class_mode_fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    stage_id = lt_info->stage_id;

    input.count = 0;
    input.field = NULL;
    in = &input;
    BCMFP_ALLOC(input.field, sizeof(bcmltd_field_t *),
            "bcmltd field array");
    BCMFP_ALLOC(input.field[0], sizeof (bcmltd_field_t), "field");

    if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID) {
        SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                                  stage_id,
                                                  BCMFP_LT_TYPE_PRESEL_ENTRY,
                                                  &lt_info));
        imm_fields.count = 0;
        imm_fields.field = NULL;
        SHR_IF_ERR_EXIT
            (bcmfp_lt_bcmltd_fields_buff_alloc(unit, lt_info->ltid, &imm_fields));
        out_fields = imm_fields.count;
        fid = fid_list->presel_ids_set_fid;
        for (i = 0; i < BCMFP_GROUP_MAX_PRESEL; i++) {
            presel_fields[i] = &presel_set[i];
        }
        multi_fields.count = BCMFP_GROUP_MAX_PRESEL;
        multi_fields.field = presel_fields;
        BCMFP_RET_VAL_ASSIGN
                  (bcmfp_lt_field_multi_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &multi_fields));

        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            for (i = 0; i < multi_fields.count; i++) {
                if (presel_set[i].data == 0)  {
                    continue;
                }
                BCMFP_UINT64_TO_UINT32(presel_set[i].data, presel_id);

                /* Lock the presel  template*/
                presel_fid_list = (bcmfp_lt_presel_entry_sp_fids_t*) lt_info->sp_fids;
                input.field[0]->id = presel_fid_list->presel_id_fid;
                input.field[0]->data = presel_id;
                input.count = 1;

                SHR_IF_ERR_EXIT
                    (bcmimm_entry_lock(unit, lt_info->ltid, in));

                /* Check if the Mixed Src Class Mask field is present in the
                 * presel entry and is non zero*/

                input.field[0]->id = presel_fid_list->presel_id_fid;
                input.field[0]->data = presel_id;
                input.count = 1;
                imm_fields.count = out_fields;
                SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                                    lt_info->ltid,
                                                    in,
                                                    &imm_fields));
                fid = fid_list->presel_lt_mixed_src_class_mask_fid;
                BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
                if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                    BCMFP_UINT64_TO_UINT32(data_val, src_class_mask);
                }
                if((src_class_mask != 0) &&
                   (increment_flag == FALSE))  {
                    SHR_IF_ERR_EXIT
                        (bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));
                    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                                              stage_id,
                                                              BCMFP_LT_SRC_CLASS_MODE,
                                                              &lt_info));
                    src_class_mode_fid_list = (bcmfp_lt_src_class_mode_sp_fids_t*) lt_info->sp_fids;
                    input.field[0]->id = src_class_mode_fid_list->pipe_id_fid;
                    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
                        fid = fid_list->pipe_fid;
                        SHR_IF_ERR_EXIT
                            (bcmfp_lt_field_data_get_from_data_array(unit,
                                                                     fid,
                                                                     data,
                                                                     &data_val));
                        BCMFP_UINT64_TO_UINT32(data_val, in_pipe_id);

                        /*Lock the src_class LT entry with index pipe id*/
                        input.field[0]->data = in_pipe_id;
                        input.count = 1;
                        SHR_IF_ERR_EXIT
                            (bcmimm_entry_lock(unit, lt_info->ltid, in));
                    } else {
                        /*Lock the src_class LT entry with index 0 in global mode*/
                        
                        input.field[0]->data = 0;
                        input.count = 1;
                        SHR_IF_ERR_EXIT
                            (bcmimm_entry_lock(unit, lt_info->ltid, in));
                    }
                    increment_flag = TRUE;
                }
            }
        }

    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    if (in) {
        SHR_FREE(in->field[0]);
        SHR_FREE(in->field);
    }
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    SHR_FUNC_EXIT();
}


int
bcmfp_lt_ingress_grp_config_update(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t sid,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data)
{
    uint32_t key_fid = 0;
    uint32_t offset = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t fid = 0;
    uint32_t gid = 0;
    uint32_t idx = 0;
    uint32_t src_class_mask = 0;
    uint32_t last_index = 0;
    uint64_t data_val = 0;
    uint64_t key_val = 0;
    uint32_t presel_id = 0;
    uint32_t in_pipe_id = 0;
    bool increment_flag = FALSE;
    bcmltd_sid_t group_ltid;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_field_info_t *field_info = NULL;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_lt_presel_entry_sp_fids_t *presel_fid_list = NULL;
    bcmfp_lt_src_class_mode_sp_fids_t *src_class_mode_fid_list = NULL;
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t input = {0};
    uint32_t out_fields = 0;
    uint16_t add_count = 0;
    bcmltd_fields_t *in = NULL;
    bcmfp_stage_id_t stage_id;
    bcmfp_stage_oper_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    stage_id = lt_info->stage_id;

    key_fid = fid_list->group_id_fid;
    group_ltid = sid;

    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          key_fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, gid);

    imm_fields.count = 0;
    imm_fields.field = NULL;

    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));

    out_fields = imm_fields.count;
    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));

    in = &input;
    input.field[0]->id = key_fid;
    input.field[0]->data = gid;
    input.count = 1;
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                        sid,
                                        in,
                                        &imm_fields));
    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_PRESEL_ENTRY,
                                              &lt_info));
    presel_fid_list = (bcmfp_lt_presel_entry_sp_fids_t*) lt_info->sp_fids;
    input.field[0]->id = presel_fid_list->presel_id_fid;
    input.count = 1;
    do {
        fid = data->id;
        idx = data->idx;

        for (j = 0; j < imm_fields.count; j++) {
            if ((imm_fields.field[j]->id == fid) &&
                (imm_fields.field[j]->idx == idx)) {
                if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID  &&
                        fid == fid_list->presel_ids_set_fid) {

                    /* Decrement ref count for old id */
                    if (imm_fields.field[j]->data)  {
                        BCMFP_UINT64_TO_UINT32(imm_fields.field[j]->data,
                                               presel_id);
                        input.field[0]->data = presel_id;

                        SHR_IF_ERR_EXIT
                            (bcmimm_entry_unlock(unit, lt_info->ltid, in));
                    }

                    /* Increment ref count for new id */
                    if (in->field[i]->data)  {
                        BCMFP_UINT64_TO_UINT32(in->field[i]->data, presel_id);
                        input.field[0]->data = presel_id;
                        SHR_IF_ERR_EXIT
                            (bcmimm_entry_lock(unit, lt_info->ltid, in));
                    }
                    
                }
                imm_fields.field[j]->data = in->field[i]->data;
                break;
            }
        }

        if (j == imm_fields.count) {
            /* Field not found in imm_fields. Add it */
            last_index = imm_fields.count;
            SHR_IF_ERR_EXIT
                (bcmfp_lt_field_add(unit, group_ltid, fid, out_fields, &imm_fields));

            SHR_IF_ERR_EXIT
                (bcmfp_lt_field_info_get(unit, fid, lt_info, &field_info));

            /* Set Data to newly added field */
            if (field_info->is_array) {
                imm_fields.field[last_index]->idx = in->field[i]->idx;
                imm_fields.field[last_index]->data = in->field[i]->data;
            } else {
                imm_fields.field[last_index + idx]->data = in->field[i]->data;
            }
            if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID  &&
                    fid == fid_list->presel_ids_set_fid) {
                if (in->field[i]->data == 0) {
                    continue;
                }
                /* Increment ref count for new id */
                BCMFP_UINT64_TO_UINT32(in->field[i]->data, presel_id);
                input.field[0]->data = presel_id;

                SHR_IF_ERR_EXIT
                    (bcmimm_entry_lock(unit, lt_info->ltid, in));
                /* Check if the presel entry has Mixed Src Class Qualifier. */
                fid = fid_list->presel_lt_mixed_src_class_mask_fid;
                BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
                if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                    BCMFP_UINT64_TO_UINT32(data_val, src_class_mask);
                }
                if((src_class_mask != 0) &&
                   (increment_flag == FALSE))  {
                    /* Increment the SRC_CLASS_MODE REF_COUNT by one*/
                    SHR_IF_ERR_EXIT
                        (bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));
                    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                                              stage_id,
                                                              BCMFP_LT_SRC_CLASS_MODE,
                                                              &lt_info));
                    src_class_mode_fid_list = (bcmfp_lt_src_class_mode_sp_fids_t*) lt_info->sp_fids;
                    input.field[0]->id = src_class_mode_fid_list->pipe_id_fid;
                    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
                        fid = fid_list->pipe_fid;
                        SHR_IF_ERR_EXIT(bcmfp_lt_field_data_get(unit,
                                                                fid,
                                                                in,
                                                                &data_val));
                        BCMFP_UINT64_TO_UINT32(data_val, in_pipe_id);
                        input.field[0]->data = in_pipe_id;
                        input.count = 1;
                        SHR_IF_ERR_EXIT
                            (bcmimm_entry_lock(unit, lt_info->ltid, in));
                    } else {
                        /* Update REF_COUNT for pipe=0 in global mode */
                        input.field[0]->data = 0;
                        input.count = 1;
                        SHR_IF_ERR_EXIT
                            (bcmimm_entry_lock(unit, lt_info->ltid, in));
                    }
                    increment_flag = TRUE;
                }

            }
        }
        data = data->next;
    } while (data != NULL);

    /* Reset the Group mode_oper to NONE on update*/
    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.count = 1;
    imm_fields.count = out_fields;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_lookup(unit,
                            group_ltid,
                            in,
                            &imm_fields));
    
    fid = fid_list->mode_oper_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        add_count = 1;
    }
    else {
        add_count = 2;
    }

    /*Update the in with KEY*/
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = gid;
    input.field[1]->id = fid;
    /* Reset the mode_oper to 0*/
    input.field[1]->data = 0;
    input.count = imm_fields.count + add_count;
    if (in != NULL) {
        for (offset = 2; offset < input.count; offset++) {
            input.field[offset]->id = imm_fields.field[offset - add_count]->id;
            input.field[offset]->idx = imm_fields.field[offset - add_count]->idx;
            input.field[offset]->data = imm_fields.field[offset - add_count]->data;
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_ingress_grp_config_delete(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t sid,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data)
{
    uint32_t fid = 0;
    uint32_t i = 0;
    uint32_t presel_id = 0;
    uint32_t group_id = 0;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_lt_presel_entry_sp_fids_t *presel_fid_list = NULL;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    uint32_t out_fields = 0;
    uint64_t key_val = 0;
    bcmfp_stage_id_t stage_id = 0;

    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t multi_fields;
    bcmltd_field_t presel_set[BCMFP_GROUP_MAX_PRESEL];
    bcmltd_field_t *presel_fields[BCMFP_GROUP_MAX_PRESEL];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    stage_id = lt_info->stage_id;

    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, sid, &imm_fields));
    out_fields = imm_fields.count;

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, sid, &input));

    /*Get the key value from the key array */
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);
    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = group_id;
    input.count = 1;
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                        sid,
                                        in,
                                        &imm_fields));

    if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID) {
        fid = fid_list->presel_ids_set_fid;
        /* Increment for Presel Entry */
        for (i = 0; i < BCMFP_GROUP_MAX_PRESEL; i++) {
            presel_fields[i] = &presel_set[i];
        }
        multi_fields.count = BCMFP_GROUP_MAX_PRESEL;
        multi_fields.field = presel_fields;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_multi_data_get(unit,
                                                           sid,
                                                           fid,
                                                           &imm_fields,
                                                           &multi_fields));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            for (i = 0; i < multi_fields.count; i++) {
                BCMFP_UINT64_TO_UINT32(presel_set[i].data, presel_id);
                SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                                          stage_id,
                                                          BCMFP_LT_TYPE_PRESEL_ENTRY,
                                                          &lt_info));
                presel_fid_list = (bcmfp_lt_presel_entry_sp_fids_t*) lt_info->sp_fids;
                input.field[0]->id = presel_fid_list->presel_id_fid;
                input.count = 1;
                input.field[0]->data = presel_id;
                SHR_IF_ERR_EXIT
                        (bcmimm_entry_unlock(unit, lt_info->ltid, in));
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
bcmfp_fixed_key_group_validate(int unit,
                               bcmimm_entry_event_t action,
                               bcmfp_lt_info_t *lt_info,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data)
{
    uint32_t grp_idx = 1;
    uint32_t fid = 0;
    uint64_t key_val = 0;
    uint32_t grp_id = 0;
    uint32_t group_id = 0;
    uint64_t data_val = 0;
    uint32_t out_fields = 0;
    uint32_t oper_mode = 0;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmltd_fields_t imm_fields = {0};
    bcmltd_sid_t group_ltid = 0;
    uint32_t grp_pri = 0, in_grp_pri = 0;
    uint32_t pipe_id = 0, in_pipe_id = 0;
    uint32_t mode = 0, in_mode = 0;
    uint32_t port_pkt_type = 0, in_port_pkt_type = 0;
    bool same = TRUE;
    bcmfp_stage_id_t stage_id;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    stage_id = lt_info->stage_id;
    group_ltid = lt_info->ltid;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));
    out_fields = imm_fields.count;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;

    /* Update with the inmemory value if the values in the user-input is default
     * and this is needed in case of UPDATE operation. */
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));

    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = group_id;
    input.count = 1;
    if (action == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                            group_ltid,
                                            in,
                                            &imm_fields));
    }
    SHR_IF_ERR_EXIT(bcmfp_stage_oper_mode_get(unit,
                                              stage_id,
                                              &oper_mode));
    if(oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        /* Update the pipe_id */
        fid = fid_list->pipe_fid;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                     fid,
                                                                     data,
                                                                     &data_val));

        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, in_pipe_id);
        } else if (action == BCMIMM_ENTRY_UPDATE) {
            /*Update with the in-mem value if the input does not have the field value */
            BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit,
                                                         fid,
                                                         &imm_fields,
                                                         &data_val));
            if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                BCMFP_UINT64_TO_UINT32(data_val, in_pipe_id);
            }
        }
    }

    /* Update the group mode */
    fid = fid_list->mode_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, in_mode);
    } else if (action == BCMIMM_ENTRY_UPDATE) {
        /*Update with the in-mem value if the input does not have the field value */
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, in_mode);
        }
    }

    /* Update the group priority */
    fid = fid_list->priority_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, in_grp_pri);
    } else if (action == BCMIMM_ENTRY_UPDATE) {
        /*Update with the in-mem value if the input does not have the field value */
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, in_grp_pri);
        }
    }


    /* Update the group port_pkt_type */
    fid = fid_list->port_pkt_type_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, in_port_pkt_type);
    } else if (action == BCMIMM_ENTRY_UPDATE) {
        /*Update with the in-mem value if the input does not have the field value */
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, in_port_pkt_type);
        }
    }

    
    while(grp_idx <= BCMFP_GROUP_MAX_EGRESS) {
       /* Skip validation if the group_id in imm is same as in user input */
        group_oper_info = NULL;
        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmfp_group_oper_info_get(unit, stage_id, grp_idx,
                                      &group_oper_info),
                                      SHR_E_NOT_FOUND);
        if ((group_oper_info != NULL) && (group_oper_info->group_id))  {
            grp_id = group_oper_info->group_id;
            if (grp_id != group_id)  {
                /* Check if pipe_id is same */
                if(oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
                    pipe_id = group_oper_info->tbl_inst;
                    if (in_pipe_id == pipe_id)  {
                        same = TRUE;
                    } else  {
                        same = FALSE;
                    }
                }

                if (same == TRUE) {
                    /* Check if group priority is same */
                    grp_pri = group_oper_info->group_prio;
                    if (in_grp_pri == grp_pri)  {
                        same = TRUE;
                    } else  {
                        same = FALSE;
                    }
                }

                if (same == TRUE) {
                    /* Check if group mode is same */
                    mode = group_oper_info->group_slice_mode;
                    if (in_mode == mode)  {
                        same = TRUE;
                    } else {
                        same = FALSE;
                    }
                }
                if (same == TRUE) {
                    /* Check if port_pkt_type is same */
                    port_pkt_type = group_oper_info->port_pkt_type;
                    if (in_port_pkt_type == port_pkt_type)  {
                        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
                    } else  {
                        same = FALSE;
                    }
                }
            }
        }
        grp_idx++;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}

int
bcmfp_group_priority_validate (int unit,
                               bcmimm_entry_event_t action,
                               bcmfp_lt_info_t *lt_info,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data)
{

    uint32_t grp_idx = 1;
    uint32_t fid = 0;
    uint64_t key_val = 0;
    uint32_t grp_id = 0;
    uint32_t group_id = 0;
    uint32_t oper_mode = 0;
    uint64_t data_val = 0;
    uint32_t out_fields = 0;
    bcmfp_stage_id_t stage_id = 0;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmltd_fields_t imm_fields = {0};
    bcmltd_sid_t group_ltid = 0;
    uint32_t grp_pri = 0, in_grp_pri = 0;
    uint32_t pipe_id = 0, in_pipe_id = 0;
    bool same = TRUE;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    stage_id = lt_info->stage_id;
    group_ltid = lt_info->ltid;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));
    out_fields = imm_fields.count;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;

    /* Update with the inmemory value if the values in the user-input is default
     * and this is needed in case of UPDATE operation. */
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);


    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));
    out_fields = input.count;
    in = &input;
    input.field[0]->id = fid_list->group_id_fid;
    input.field[0]->data = group_id;
    input.count = 1;
    if (action == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                            group_ltid,
                                            in,
                                            &imm_fields));
    }
    SHR_IF_ERR_EXIT(bcmfp_stage_oper_mode_get(unit,
                                              stage_id,
                                              &oper_mode));

    if(oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        /* Update the pipe_id */
        fid = fid_list->pipe_fid;
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                     fid,
                                                                     data,
                                                                     &data_val));

        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, in_pipe_id);
        } else if (action == BCMIMM_ENTRY_UPDATE) {
            /*Update with the in-mem value if the input does not have the field value */
            BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit,
                                                         fid,
                                                         &imm_fields,
                                                         &data_val));
            if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                BCMFP_UINT64_TO_UINT32(data_val, in_pipe_id);
            }
        }
    }

    /* Update the group priority */
    fid = fid_list->priority_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, in_grp_pri);
    } else if (action == BCMIMM_ENTRY_UPDATE) {
        /*Update with the in-mem value if the input does not have the field value */
        BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, in_grp_pri);
        }
    }

    
    while(grp_idx <= BCMFP_GROUP_MAX_VFP) {
        /* Skip validation if the group_id in imm is same as in user input */
        group_oper_info = NULL;
        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmfp_group_oper_info_get(unit, stage_id, grp_idx,
                                      &group_oper_info),
                                      SHR_E_NOT_FOUND);
        if (group_oper_info != NULL)   {
            grp_id = group_oper_info->group_id;
            if (grp_id != group_id)  {
                /* Check if pipe_id is same */
                if(oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
                    pipe_id = group_oper_info->tbl_inst;
                    if (in_pipe_id == pipe_id)  {
                        same = TRUE;
                    } else  {
                        same = FALSE;
                    }
                }
                if (same == TRUE) {
                    /* Check if group priority is same */
                    grp_pri = group_oper_info->group_prio;
                    if (in_grp_pri == grp_pri)  {
                        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
                    } else  {
                        same = FALSE;
                    }
                }
            }
        }
        grp_idx++;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

    SHR_FUNC_EXIT();
}


int
bcmfp_lt_group_config_get(int unit,
                          uint32_t trans_id,
                          bcmfp_stage_id_t stage_id,
                          uint32_t group_id,
                          bcmfp_lt_group_config_t *group_config)
{
  bool auto_mode = FALSE;
  uint8_t active_presel_count = 0;
  uint32_t i = 0;
  uint16_t group_ref_count = 0;
  uint32_t fid = 0;
  uint32_t idx = 0;
  uint32_t incr = 0;
  uint32_t out_fields = 0;
  /* Default value of GRP_MODE is SINGLE=1 */
  uint32_t mode = 1;
  uint32_t mode_oper = 0;
  uint32_t group_em_ltid = 0;
  uint32_t group_em_auto_ltid= 0;
  uint32_t group_em_oper_ltid = 0;
  uint32_t field_val[BCMFP_MAX_WSIZE];
  uint64_t data = 0;
  uint64_t data_def = 0;
  bcmltd_sid_t group_ltid = 0;
  bcmfp_lt_group_qual_map_t group_qual_map;
  bcmltd_fields_t imm_fields;
  bcmltd_fields_t input;
  bcmltd_fields_t *in = NULL;
  bcmltd_field_t presel_set[BCMFP_GROUP_MAX_PRESEL];
  bcmltd_field_t *presel_fields[BCMFP_GROUP_MAX_PRESEL];
  bcmltd_fields_t multi_fields;
  bcmfp_lt_info_t *lt_info = NULL;
  bcmfp_lt_group_sp_fids_t *fid_list = NULL;
  bcmfp_lt_field_info_t *field_info = NULL;
  bcmfp_stage_oper_mode_t stage_oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
  bcmfp_group_oper_info_t *group_oper_info = NULL;
  bcmfp_stage_t *stage = NULL;
  bcmfp_lt_policy_config_t *policy_config = NULL;

  SHR_FUNC_ENTER(unit);

  SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
  /* LT for stage */
  SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                            stage_id,
                                            BCMFP_LT_TYPE_GROUP,
                                            &lt_info));
  SHR_IF_ERR_EXIT_EXCEPT_IF(
      bcmfp_group_oper_info_get(unit, stage_id,
                    group_id, &group_oper_info), SHR_E_NOT_FOUND);
  if (group_oper_info != NULL) {
      group_ref_count = group_oper_info->ref_count;
  }

  group_ltid = lt_info->ltid;
  fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;

  imm_fields.count = 0;
  imm_fields.field = NULL;
  SHR_IF_ERR_EXIT
    (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &imm_fields));

  input.count = 0;
  input.field = NULL;
  SHR_IF_ERR_EXIT
    (bcmfp_lt_bcmltd_fields_buff_alloc(unit, group_ltid, &input));
  out_fields = input.count;
  in = &input;
  input.field[0]->id = fid_list->group_id_fid;
  input.field[0]->data = group_id;
  input.count = 1;
  SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                        group_ltid,
                                        in,
                                        &imm_fields));
  i = 0;
  while (i < imm_fields.count) {
    fid = imm_fields.field[i]->id;
    idx = imm_fields.field[i]->idx;

    
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
      (lt_info->map_get(unit, fid, idx, (void *) &group_qual_map));
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
      SHR_IF_ERR_EXIT
        (bcmfp_lt_field_data_to_uint32_array(unit,
                                             imm_fields.field[i + idx],
                                             field_info,
                                             field_val));
    }

    if ((stage->flags & BCMFP_STAGE_POLICY_TYPE_PDD) &&
        (group_qual_map.field_type == BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION)) {
        SHR_IF_ERR_EXIT(bcmfp_lt_group_action_add(unit,
                                                  &group_qual_map,
                                                  group_config));
    } else {
        SHR_IF_ERR_EXIT(bcmfp_lt_group_qual_add(unit,
                                                field_info,
                                                &group_qual_map,
                                                field_val,
                                                group_config));
    }
    /* Increment i. */
    i += incr;
  }

  group_config->group_id = group_id;

  SHR_IF_ERR_EXIT(
      bcmfp_stage_oper_mode_get(unit, stage_id, &stage_oper_mode));

  if (stage_oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
    group_config->flag |= 0x1;
    group_config->pipe_id = -1;
  } else {
    /* fill Pipe info */
    fid = fid_list->pipe_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      group_config->flag |= 0x1;
      BCMFP_UINT64_TO_UINT32(data, group_config->pipe_id);
    }
  }

  /* fill Priority info */
  group_config->priority = 0;
  fid = fid_list->priority_fid;
  BCMFP_RET_VAL_ASSIGN
    (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
  if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
    BCMFP_UINT64_TO_UINT32(data, group_config->priority);
  }

  group_config->lookup_id = 0;
  fid = fid_list->group_lookup_id_fid;
  BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
  if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      BCMFP_UINT64_TO_UINT32(data, group_config->lookup_id);
  }

  if (lt_info->flags & BCMFP_LT_FLAGS_FIXED_KEY_GRP) {
    fid = fid_list->mode_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (!SHR_FUNC_ERR()) {
      BCMFP_UINT64_TO_UINT32(data, group_config->slice_mode);
    }

    fid = fid_list->port_pkt_type_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (!SHR_FUNC_ERR()) {
      BCMFP_UINT64_TO_UINT32(data, group_config->port_pkt_type);
    }
  }
  else {
    /* fill auto mode */
    fid = fid_list->mode_auto_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      BCMFP_UINT64_TO_BOOL(data, auto_mode);
    }

    fid = fid_list->mode_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      BCMFP_UINT64_TO_UINT32(data, mode);
    }

    fid = fid_list->mode_oper_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      BCMFP_UINT64_TO_UINT32(data, mode_oper);
    }


    
    
    if (group_ref_count != 0) {
      /* Group is already configured in H/W, Use Mode Oper */
      group_config->mode = mode_oper;
    } else if (auto_mode == FALSE) {
      
        switch (mode) {
            case 1:
                group_config->mode = BCMFP_GROUP_MODE_SINGLE;
                break;
            case 2:
                group_config->mode = BCMFP_GROUP_MODE_DBLINTRA;
                break;
            case 3:
                group_config->mode = BCMFP_GROUP_MODE_DBLINTER;
                break;
            case 4:
                group_config->mode = BCMFP_GROUP_MODE_TRIPLE;
                break;
            case 5:
                group_config->mode = BCMFP_GROUP_MODE_QUAD;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }
    } else {
      group_config->mode = BCMFP_GROUP_MODE_AUTO;
    }
  }
  if (lt_info->flags & BCMFP_LT_FLAGS_VIRTUAL_SLICE_GRP) {
    fid = fid_list->virtual_slice_grp_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (!SHR_FUNC_ERR()) {
      BCMFP_UINT64_TO_UINT8(data, group_config->virtual_slice_grp);
    }
  }

  if (lt_info->flags & BCMFP_LT_FLAGS_HASH_KEY_GRP) {

    fid = fid_list->policy_id_fid;
    policy_config = &(group_config->policy_config);
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        if (data) {
            group_config->policy_valid = TRUE;
            group_config->flag |= BCMFP_GROUP_WITH_DEFAULT_POLICY;
            policy_config->valid = TRUE;
        } else {
            /* Dettach the policy template if the ID == 0 */
            group_config->policy_valid = FALSE;
            policy_config->valid = FALSE;
        }
    }
    if (group_config->policy_valid)  {
        SHR_IF_ERR_EXIT(bcmfp_lt_policy_config_get(unit,
                                                   trans_id,
                                                   stage_id,
                                                   data,
                                                   0,
                                                   NULL,
                                                   policy_config));
    }
    fid = fid_list->group_ltid_auto_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      BCMFP_UINT64_TO_BOOL(data, group_em_auto_ltid);
    }

    fid = fid_list->group_ltid_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data, group_em_ltid);
    }

    fid = fid_list->group_ltid_oper_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      BCMFP_UINT64_TO_UINT32(data, group_em_oper_ltid);
    }

    if (group_ref_count != 0) {
      /* Group is already configured in H/W, Use Mode Oper */
        group_config->group_ltid = group_em_oper_ltid;
    } else if (group_em_auto_ltid == FALSE) {
        group_config->group_ltid = group_em_ltid;
    } else {
      group_config->group_ltid = BCMFP_GROUP_AUTO_LTID;
    }
  }

  if (lt_info->flags & BCMFP_LT_FLAGS_PRESEL_VALID) {
    for (i = 0; i < BCMFP_GROUP_MAX_PRESEL; i++) {
      presel_fields[i] = &presel_set[i];
    }

    fid = fid_list->presel_ids_set_count_fid;
    BCMFP_RET_VAL_ASSIGN
      (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      BCMFP_UINT64_TO_UINT32(data, active_presel_count);
    }

    multi_fields.count = BCMFP_GROUP_MAX_PRESEL;
    multi_fields.field = presel_fields;

    fid = fid_list->presel_ids_set_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_multi_data_get(unit,
                                                       group_ltid,
                                                       fid,
                                                       &imm_fields,
                                                       &multi_fields));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
      group_config->num_presel_ids = (active_presel_count < multi_fields.count)?
          active_presel_count : multi_fields.count;
      for (i = 0; i < group_config->num_presel_ids; i++) {
        BCMFP_UINT64_TO_UINT32(presel_set[i].data, group_config->presel_ids[i]);
      }
    }
  }

  group_config->valid = TRUE;

  SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

  if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
    bcmfp_lt_group_config_free(unit, group_config);
  }

  (void)bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);
  (void)bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);

  SHR_FUNC_EXIT();
}

int
bcmfp_lt_egress_grp_config_update(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t sid,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data)
{
    uint32_t fid = 0;
    bcmfp_group_slice_mode_t mode = BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE;
    uint64_t key_val = 0;
    uint64_t data_val = 0;
    uint32_t group_id = 0;
    uint32_t pipe_id = 0;
    uint32_t port_pkt_type = 0;
    uint32_t priority = 0;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    stage_id = lt_info->stage_id;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);

    fid = fid_list->pipe_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));

    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, pipe_id);
    }

    fid = fid_list->mode_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));

    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, mode);
    }


    fid = fid_list->port_pkt_type_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, port_pkt_type);
    }

    fid = fid_list->priority_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));

    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, priority);
    }
    SHR_IF_ERR_EXIT(bcmfp_group_params_set(unit,
                                           stage_id,
                                           pipe_id,
                                           group_id,
                                           priority,
                                           mode,
                                           port_pkt_type));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_egress_grp_config_delete(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t sid,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data)
{
    uint32_t fid = 0;
    uint32_t group_id = 0;
    uint64_t key_val = 0;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    stage_id = lt_info->stage_id;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);

    SHR_IF_ERR_EXIT(bcmfp_group_params_reset(unit,
                                             stage_id,
                                             group_id));

exit:

    SHR_FUNC_EXIT();
}



int
bcmfp_lt_vlan_grp_config_update(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t sid,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data)
{
    uint32_t fid = 0;
    uint64_t key_val = 0;
    uint64_t data_val = 0;
    uint32_t group_id = 0;
    uint32_t pipe_id = 0;
    uint32_t priority = 0;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    stage_id = lt_info->stage_id;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);

    fid = fid_list->pipe_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));

    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, pipe_id);
    }

    fid = fid_list->priority_fid;
    BCMFP_RET_VAL_ASSIGN(bcmfp_lt_field_data_get_from_data_array(unit,
                                                                 fid,
                                                                 data,
                                                                 &data_val));

    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data_val, priority);
    }
    SHR_IF_ERR_EXIT(bcmfp_group_params_set(unit,
                                           stage_id,
                                           pipe_id,
                                           group_id,
                                           priority,
                                           0,
                                           0));

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_vlan_grp_config_delete(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t sid,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data)
{
    uint32_t fid = 0;
    uint32_t group_id = 0;
    uint64_t key_val = 0;
    bcmfp_lt_group_sp_fids_t *fid_list = NULL;
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    stage_id = lt_info->stage_id;

    fid_list = (bcmfp_lt_group_sp_fids_t *) lt_info->sp_fids;
    fid = fid_list->group_id_fid;
    SHR_IF_ERR_EXIT(bcmfp_lt_field_key_get_from_key_array(unit,
                                                          fid,
                                                          key,
                                                          &key_val));
    BCMFP_UINT64_TO_UINT32(key_val, group_id);

    SHR_IF_ERR_EXIT(bcmfp_group_params_reset(unit,
                                             stage_id,
                                             group_id));

exit:

    SHR_FUNC_EXIT();
}


