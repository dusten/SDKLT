/*! \file bcmfp_lt_presel_entry.c
 *
 * APIs for LT presel entry template
 *
 * This file contains function definitions for presel entry LT template
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
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmfp/bcmfp_lt_presel_entry.h>
#include <bcmfp/map/bcmfp_lt_presel_entry_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*******************************************************************************
  Static functions
 */
STATIC int
bcmfp_lt_presel_entry_add(int unit,
                          bcmfp_lt_field_info_t *field_info,
                          bcmfp_lt_presel_entry_map_t *presel_map,
                          uint32_t *field_val,
                          bcmfp_lt_presel_config_t *presel_config)
{
    bcmfp_lt_presel_qual_info_t *qual_info = NULL;
    bcmfp_lt_presel_action_info_t *action_info = NULL;
    SHR_FUNC_ENTER(unit);

    if (presel_map->field_type == BCMFP_LT_PRESEL_FIELD_ACTION) {
        /* Allocate New Node */
        BCMFP_ALLOC(action_info, sizeof(bcmfp_lt_presel_action_info_t),
                    "presel entry action node");
        if (presel_config->action_info == NULL) {
            presel_config->action_info = action_info;
        } else {
            /* Adding as first node */
            action_info->next = presel_config->action_info;
            presel_config->action_info = action_info;
        }
        action_info->action = presel_map->t.action;
        action_info->param = *field_val;

        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /*
     * Field is qualifier
     * Search for node in qual_info
     * if found, add there
     * else, create new node
     */
    qual_info = presel_config->qual_info;
    while (qual_info != NULL) {
        if (qual_info->qual == presel_map->t.qual) {
            break;
        }
        qual_info = qual_info->next;
    }

    /* Not found Create New Node */
    if (qual_info == NULL) {
        BCMFP_ALLOC(qual_info, sizeof(bcmfp_lt_presel_qual_info_t),
                    "presel entry qual node");
        if (presel_config->qual_info == NULL) {
            presel_config->qual_info = qual_info;
        } else {
            /* Adding as first node */
            qual_info->next = presel_config->qual_info;
            presel_config->qual_info = qual_info;
        }
    }

    qual_info->qual = presel_map->t.qual;

    if (presel_map->field_type == BCMFP_LT_PRESEL_FIELD_QUAL_ENUM) {
        SHR_IF_ERR_EXIT
            (bcmfp_qualifier_enum_set(unit,
                                      presel_config->stage_id,
                                      qual_info->qual,
                                      *field_val,
                                      &(qual_info->data),
                                      &(qual_info->mask)));
    } else if (presel_map->field_type == BCMFP_LT_PRESEL_FIELD_QUAL_DATA) {
        qual_info->data = *field_val;
    } else {  /* field_type == BCMFP_LT_PRESEL_FIELD_QUAL_MASK */
        qual_info->mask = *field_val;
    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_presel_config_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t presel_id,
                           bcmfp_lt_presel_config_t *presel_config)
{
    uint32_t i = 0;
    uint32_t fid = 0;
    uint32_t idx = 0;
    uint32_t incr = 0;
    uint32_t out_fields = 0;
    uint32_t field_val[BCMFP_MAX_WSIZE];
    uint64_t data = 0;
    bcmltd_sid_t presel_ltid = 0;
    bcmfp_lt_presel_entry_map_t presel_map;
    bcmltd_fields_t imm_fields;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t *in = NULL;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_field_info_t *field_info = NULL;
    bcmfp_lt_presel_entry_sp_fids_t *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_PRESEL_ENTRY,
                                              &lt_info));

    presel_ltid = lt_info->ltid;
    fid_list = (bcmfp_lt_presel_entry_sp_fids_t *) lt_info->sp_fids;

    imm_fields.count = 0;
    imm_fields.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, presel_ltid, &imm_fields));
    out_fields = imm_fields.count;

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT(bcmfp_lt_bcmltd_fields_buff_alloc(unit, presel_ltid, &input));
    out_fields = input.count;
    /* inmem_fields.count is changed in below function */
    in = &input;
    input.field[0]->id = fid_list->presel_id_fid;
    input.field[0]->data = presel_id;
    input.count = 1;

    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit,
                                        presel_ltid,
                                        in,
                                        &imm_fields));

    presel_config->presel_id = presel_id;
    presel_config->ltid = presel_ltid;
    presel_config->stage_id = stage_id;

    i = 0;
    while (i < imm_fields.count) {
        fid = imm_fields.field[i]->id;
        idx = imm_fields.field[i]->idx;

        BCMFP_RET_VAL_ASSIGN
            (lt_info->map_get(unit, fid, idx, (void *) &presel_map));
        if (SHR_FUNC_ERR()) {
            i++;
            continue;
        }

        /* fid has Mapping */
        SHR_IF_ERR_EXIT
            (bcmfp_lt_field_info_get(unit, fid, lt_info, &field_info));

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
                        imm_fields.field[i + idx], field_info, field_val));
        }
        SHR_IF_ERR_EXIT(bcmfp_lt_presel_entry_add(unit, field_info,
                    &presel_map, field_val, presel_config));
        /* Increment i. */
        i += incr;
    }

    /* fill Priority info */
    presel_config->priority = 0;
    fid = fid_list->priority_fid;
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data, presel_config->priority);
    }

    fid = fid_list->ref_count_fid;
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(data, presel_config->ref_count);
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    if (!SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        bcmfp_lt_presel_config_free(unit, presel_config);
    }

    bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_presel_config_free(int unit,
                            bcmfp_lt_presel_config_t *presel_config)
{
    bcmfp_lt_presel_qual_info_t *temp_qual_ptr = NULL;
    bcmfp_lt_presel_action_info_t *temp_action_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK (presel_config, SHR_E_NONE);

    if (presel_config->qual_info != NULL) {
        temp_qual_ptr = presel_config->qual_info;

        while(temp_qual_ptr != NULL) {
            temp_qual_ptr = temp_qual_ptr->next;
            SHR_FREE(presel_config->qual_info);
            presel_config->qual_info = temp_qual_ptr;
        }
    }
    if (presel_config->action_info != NULL) {
        temp_action_ptr = presel_config->action_info;

        while(temp_action_ptr != NULL) {
            temp_action_ptr = temp_action_ptr->next;
            SHR_FREE(presel_config->action_info);
            presel_config->action_info = temp_action_ptr;
        }
    }
    presel_config->qual_info = NULL;
    presel_config->action_info = NULL;

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_presel_entry_config(int unit,
                             uint32_t trans_id,
                             bcmfp_stage_id_t stage_id,
                             uint32_t eid,
                             bcmfp_lt_group_config_t *group_config,
                             uint32_t **ekw,
                             uint32_t **edw)
{
    uint32_t i = 0;
    uint32_t idx = 0;
    uint32_t gid = 0;
    uint32_t entry_flags = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_lt_presel_config_t presel_config = {0};
    bcmfp_lt_presel_qual_info_t *qual_info = NULL;
    bcmfp_lt_presel_action_info_t *action_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(group_config, SHR_E_FAIL);

    gid = group_config->group_id;

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (group_config->num_presel_ids == 0) {
        SHR_IF_ERR_EXIT
            (bcmfp_action_presel_init(unit, stage_id, gid, eid, ekw, TRUE));
        entry_flags = BCMFP_ENTRY_PRESEL | BCMFP_ENTRY_PRESEL_DEFAULT;
        if (!(stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH)) {
            SHR_IF_ERR_EXIT
                (bcmfp_entry_insert(unit,
                                    entry_flags,
                                    trans_id,
                                    group_config->pipe_id,
                                    stage_id,
                                    FP_ING_PRESEL_ENTRY_TEMPLATEt,
                                    gid, gid, 0, ekw, edw));

            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else {
            SHR_IF_ERR_EXIT
                (bcmfp_entry_insert(unit,
                                    entry_flags,
                                    trans_id,
                                    group_config->pipe_id,
                                    stage_id,
                                    FP_EM_PRESEL_ENTRY_TEMPLATEt,
                                    gid, gid, 0, ekw, edw));

            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    for (i = 0; i < group_config->num_presel_ids; i++) {
        /* Reset ekw and edw */
        for (idx = 0; (idx < BCMFP_ENTRY_PARTS_MAX)&& (i != 0) ; idx++) {
            sal_memset(ekw[idx], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
            sal_memset(edw[idx], 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
        }
        sal_memset(&presel_config, 0, sizeof (presel_config));

        SHR_IF_ERR_EXIT(bcmfp_lt_presel_config_get(unit, stage_id,
                                group_config->presel_ids[i], &presel_config));

        SHR_IF_ERR_EXIT(bcmfp_action_presel_init(unit,
                      stage_id, gid, group_config->presel_ids[i], ekw, FALSE));

        qual_info = presel_config.qual_info;
        while(qual_info != NULL) {
            SHR_IF_ERR_EXIT(bcmfp_qual_presel_set(unit,
                                                  stage_id,
                                                  gid,
                                                  qual_info->qual,
                                                  &(qual_info->data),
                                                  &(qual_info->mask),
                                                  ekw));
            qual_info = qual_info->next;
        }

        action_info = presel_config.action_info;
        while (action_info != NULL) {
            SHR_IF_ERR_EXIT(bcmfp_presel_action_set(unit,
                                                    stage_id,
                                                    gid,
                                                    action_info->action,
                                                    action_info->param,
                                                    ekw));
            action_info = action_info->next;
        }

        SHR_IF_ERR_EXIT(bcmfp_entry_insert(unit,
                                           BCMFP_ENTRY_PRESEL,
                                           trans_id,
                                           group_config->pipe_id,
                                           stage_id,
                                           presel_config.ltid,
                                           gid,
                                           presel_config.presel_id,
                                           presel_config.priority,
                                           ekw,
                                           edw));

        (void) bcmfp_lt_presel_config_free(unit, &presel_config);
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    if (SHR_FUNC_ERR()) {
        (void) bcmfp_lt_presel_config_free(unit, &presel_config);
    }

    SHR_FUNC_EXIT();
}

