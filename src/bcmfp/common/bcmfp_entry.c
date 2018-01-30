/*! \file bcmfp_entry.c
 *
 * APIs to operate on FP entries.
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
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmfp/bcmfp_lt_internal.h>
#include <bcmfp/map/bcmfp_lt_entry_map.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_group.h>
#include <bcmfp/bcmfp_lt_rule.h>
#include <bcmfp/bcmfp_lt_entry.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

typedef struct bcmfp_entry_buffer_s {
     uint32_t entry_key_words[BCMFP_ENTRY_WORDS_MAX];
     uint32_t entry_data_words[BCMFP_ENTRY_WORDS_MAX];
} bcmfp_entry_buffer_t;

static bcmfp_entry_buffer_t req_entry_buffer[BCMDRD_CONFIG_MAX_UNITS]
                                            [BCMFP_ENTRY_PARTS_MAX];
static bcmfp_entry_buffer_t rsp_entry_buffer[BCMDRD_CONFIG_MAX_UNITS]
                                            [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *req_ekw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *req_edw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *rsp_ekw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];

static uint32_t *rsp_edw[BCMDRD_CONFIG_MAX_UNITS]
                        [BCMFP_ENTRY_PARTS_MAX];


STATIC
int bcmfp_entry_hash_same_key_get(int unit,
                                  uint32_t trans_id,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_entry_id_t entry_id,
                                  uint32_t req_flags,
                                  bcmltd_sid_t req_ltid,
                                  void *pt_dyn_info,
                                  bcmptm_keyed_lt_mreq_info_t *req_info,
                                  bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                                  bcmltd_sid_t *rsp_ltid,
                                  uint32_t *rsp_flags,
                                  uint8_t *same_key)

{
    uint32_t out_fields = 0;
    uint32_t **ekw = NULL;
    uint32_t wsize = 0;
    int i = 0;
    uint32_t fid = 0;
    uint32_t group_id = 0;
    uint32_t rule_id = 0;
    uint64_t data_val = 0;
    uint8_t part = 0;
    uint8_t parts_count = 0;
    bool new_key_exists = FALSE;
    bool old_key_exists  = FALSE;
    uint32_t new_key_entry_index = 0;
    uint32_t old_key_entry_index = 0;
    uint32_t rsp_flags2 = 0;
    bcmltd_sid_t  rsp_ltid2 = 0;
    bcmltd_fields_t *in = NULL;
    bcmltd_fields_t input = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_entry_sp_fids_t *fid_list = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_lt_group_config_t group_config = {0};
    bcmfp_lt_rule_config_t rule_config = {0};
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;

    SHR_FUNC_ENTER(unit);

    /* Allocate Memory for entry Key word and data word */
    BCMFP_ALLOC(ekw, sizeof(uint32_t *) * BCMFP_ENTRY_PARTS_MAX, "ekw part");

    for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
        BCMFP_ALLOC(ekw[i],
                sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX, "ekw word");
    }

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get_by_type(unit,
                                              stage_id,
                                              BCMFP_LT_TYPE_ENTRY,
                                              &lt_info));
    fid_list = (bcmfp_lt_entry_sp_fids_t *) lt_info->sp_fids;

    input.count = 0;
    input.field = NULL;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, lt_info->ltid, &input));
    SHR_IF_ERR_EXIT
        (bcmfp_lt_bcmltd_fields_buff_alloc(unit, lt_info->ltid, &imm_fields));
    out_fields = imm_fields.count;

    /*
     Lookup for the key with new ekw
     if found
       Lookup with old ekw/edw
       if not found
          return ERROR
       else
         if the entry_index of old and new keys are same
           continue with same_key=1
         else
           return entry_exist error
     else if not found
       Lookup for key with old ekw
         if not found
           insert with same key=0
         else if found
           delete the entry with the old ekw index
    */

    /*Lookup up for the new EKW/EDW*/
    BCMFP_RET_VAL_ASSIGN
        (bcmptm_ltm_mreq_keyed_lt(unit,  req_flags, req_ltid, pt_dyn_info,
                                  NULL, BCMPTM_OP_LOOKUP, req_info,
                                  trans_id, rsp_info, rsp_ltid, rsp_flags));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        new_key_exists = TRUE;
        new_key_entry_index = rsp_info->rsp_entry_index[0];
    }

    in = &input;
    input.field[0]->id = fid_list->entry_id_fid;
    input.field[0]->data =  entry_id;
    input.count = 1;
    BCMFP_RET_VAL_ASSIGN
        (bcmimm_entry_lookup(unit, req_ltid, in, &imm_fields));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        fid = fid_list->group_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, group_id);
            SHR_IF_ERR_EXIT
                (bcmfp_lt_group_config_get(unit, trans_id, stage_id,
                                           group_id, &group_config));
        }

        fid = fid_list->rule_id_fid;
        BCMFP_RET_VAL_ASSIGN
            (bcmfp_lt_field_data_get(unit, fid, &imm_fields, &data_val));
        if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
            BCMFP_UINT64_TO_UINT32(data_val, rule_id);
            SHR_IF_ERR_EXIT
                (bcmfp_lt_rule_config_get(unit, trans_id, stage_id, rule_id,
                                          entry_id, NULL, &group_config,
                                          &rule_config));
        }

        SHR_IF_ERR_EXIT
            (bcmfp_em_entry_build_ekw(unit, trans_id, req_ltid,
                                          group_id, entry_id, &group_config,
                                          &rule_config, ekw));
        sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));
        sal_memset(&hash_attrs, 0, sizeof(bcmptm_rm_hash_entry_attrs_t));
        SHR_IF_ERR_EXIT
            (bcmfp_group_oper_info_get(unit, stage_id,
                                       group_id, &group_oper_info));

        BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE(group_oper_info->group_mode,
                hash_attrs.entry_mode);
        lt_mreq_info.entry_attrs = &hash_attrs;

        lt_mreq_info.entry_id = entry_id;
        lt_mreq_info.pdd_l_dtyp = NULL;
        lt_mreq_info.same_key = 0;

        SHR_IF_ERR_EXIT(
                bcmfp_group_tcam_size_get(unit, FALSE, stage_id,
                                          group_id, &wsize));
        lt_mreq_info.rsp_entry_key_buf_wsize = wsize;

        wsize = 0;
        SHR_IF_ERR_EXIT(
                bcmfp_group_policy_size_get(unit, FALSE, stage_id,
                                            group_id, &wsize));
        lt_mreq_info.rsp_entry_data_buf_wsize = wsize;
        sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));

            SHR_IF_ERR_EXIT
                    (bcmfp_group_parts_count(unit,  group_oper_info->flags,
                        &parts_count));

            for (part = 0; part < parts_count; part++) {
                req_edw[unit][part] = req_entry_buffer[unit][part].entry_data_words;
                rsp_ekw[unit][part] = rsp_entry_buffer[unit][part].entry_key_words;
                rsp_edw[unit][part] = rsp_entry_buffer[unit][part].entry_data_words;
            }
            lt_mreq_info.entry_key_words = ekw;
            lt_mreq_info.entry_data_words = req_edw[unit];
            lt_mrsp_info.rsp_entry_key_words = rsp_ekw[unit];
            lt_mrsp_info.rsp_entry_data_words = rsp_edw[unit];
            lt_mrsp_info.rsp_pdd_l_dtyp = NULL;

            BCMFP_RET_VAL_ASSIGN
                (bcmptm_ltm_mreq_keyed_lt(unit, req_flags, req_ltid,
                                          pt_dyn_info, NULL, BCMPTM_OP_LOOKUP,
                                          &lt_mreq_info, trans_id, &lt_mrsp_info,
                                          &rsp_ltid2, &rsp_flags2));
            if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
                old_key_exists = TRUE;
                old_key_entry_index = lt_mrsp_info.rsp_entry_index[0];
            }

        }
        switch ((int)old_key_exists)  {
            case FALSE:
                if (new_key_exists) {
                    /* The Key already exists for the entry being inserted*/
                    SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
                } else  {
                    /*The Key doesnt exists for the entry being inserted */
                    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
                }
                break;
            case TRUE:
                if (new_key_exists) {
                    if (new_key_entry_index == old_key_entry_index)  {
                        /* On updating the data portion of the entry. Key
                         * calculated from the new and old ekw is same */
                        *same_key = TRUE;
                    } else  {
                        /* On updating the rule/group portion of the entry.
                         * The new key conflicts with some other entry's key */
                        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
                    }
                } else {
                    /* Here we need to delete the entry from the older index and
                     * insert in new index */
                    SHR_RETURN_VAL_EXIT
                        (bcmptm_ltm_mreq_keyed_lt(unit, req_flags, req_ltid,
                                         pt_dyn_info, NULL, BCMPTM_OP_DELETE,
                                         &lt_mreq_info, trans_id, &lt_mrsp_info,
                                         &rsp_ltid2, &rsp_flags2));
                }
        }

exit:
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &input);
    (void) bcmfp_lt_bcmltd_fields_buff_free(unit, out_fields, &imm_fields);

    if (ekw != NULL) {
        for (i = 0; i < BCMFP_ENTRY_PARTS_MAX; i++) {
            SHR_FREE(ekw[i]);
        }
        SHR_FREE(ekw);
    }

    SHR_FUNC_EXIT();
}

int
bcmfp_entry_insert(int unit,
                   uint32_t flags,
                   uint32_t trans_id,
                   int tbl_inst,
                   bcmfp_stage_id_t stage_id,
                   bcmltd_sid_t req_ltid,
                   bcmfp_group_id_t group_id,
                   bcmfp_entry_id_t entry_id,
                   uint32_t entry_prio,
                   uint32_t **ekw,
                   uint32_t **edw)
{
    bool is_presel = 0;
    uint8_t part = 0;
    uint8_t same_key = FALSE;
    uint8_t parts_count = 0;
    uint32_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t wsize = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmltd_sid_t rsp_ltid;
    bcmfp_stage_t *stage = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_tcam_entry_attrs_t attrs;
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    rv = bcmfp_group_oper_info_get(unit, stage_id, group_id, &group_oper_info);
    SHR_IF_ERR_EXIT(rv);

    sal_memset(&attrs, 0, sizeof(bcmptm_rm_tcam_entry_attrs_t));
    sal_memset(&hash_attrs, 0, sizeof(bcmptm_rm_hash_entry_attrs_t));

    sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    /* for EM hash entry, we need to use hash entry attrs */
    if (!(stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH)
        || (flags & BCMFP_ENTRY_PRESEL)) {
        if (flags & BCMFP_ENTRY_PRESEL) {
            is_presel = 1;
            SHR_BITSET(attrs.flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
            if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
                SHR_BITSET(attrs.flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
            }
        }
        /* In BCMFP group ids ranges from 1-<MaxId>  and in RM_TCAM, this range
         * is mapped to 0-<MaxId-1>  */
        attrs.group_id = group_oper_info->group_id - 1;

        attrs.group_prio = group_oper_info->group_prio;

        attrs.group_ltid = group_oper_info->action_res_id;

        BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE(group_oper_info->group_mode,
                                              attrs.group_mode);

        attrs.num_presel_ids = group_oper_info->num_presel_ids;

        BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs.stage_id);

        if (stage->flags & BCMFP_STAGE_KEY_TYPE_SELCODE &&
            flags & BCMFP_ENTRY_FIRST_IN_GROUP) {
            SHR_IF_ERR_EXIT(
                bcmfp_group_selcode_get(unit,
                                        stage,
                                        group_oper_info,
                                        attrs.selcode));
        }

        if (flags & BCMFP_ENTRY_WRITE_PER_PIPE_IN_GLOBAL) {
            attrs.pipe_id = -1;
        } else {
            attrs.pipe_id = tbl_inst;
        }
        /* for exact match presel entry update the slice id in attrs */
        if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH) {
            attrs.slice_id = group_oper_info->slice_id;
        }
        lt_mreq_info.entry_attrs = &attrs;

    } else {
        BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE(group_oper_info->group_mode,
                                               hash_attrs.entry_mode);
        lt_mreq_info.entry_attrs = &hash_attrs;
    }

    lt_mreq_info.entry_id = entry_id;
    lt_mreq_info.entry_pri = entry_prio;
    lt_mreq_info.entry_key_words = ekw;
    lt_mreq_info.entry_data_words = edw;
    lt_mreq_info.pdd_l_dtyp = NULL;
    lt_mreq_info.same_key = 0;

    if (flags & BCMFP_ENTRY_SAME_KEY) {
        lt_mreq_info.same_key = 1;
    }
    if (flags & BCMFP_ENTRY_WRITE_PER_PIPE_IN_GLOBAL) {
        req_flags = BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL;
    }

    SHR_IF_ERR_EXIT(
        bcmfp_entry_valid_bit_set(unit,
                                  is_presel,
                                  stage_id,
                                  group_id,
                                  ekw));

    SHR_IF_ERR_EXIT(
        bcmfp_group_tcam_size_get(unit,
                                  is_presel,
                                  stage_id,
                                  group_id,
                                  &wsize));
    lt_mreq_info.rsp_entry_key_buf_wsize = wsize;

    wsize = 0;
    SHR_IF_ERR_EXIT(
        bcmfp_group_policy_size_get(unit,
                                  is_presel,
                                  stage_id,
                                  group_id,
                                  &wsize));
    lt_mreq_info.rsp_entry_data_buf_wsize = wsize;

    sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));

    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit,
                                group_oper_info->flags,
                                &parts_count));

    for (part = 0; part < parts_count; part++) {
        rsp_ekw[unit][part] = rsp_entry_buffer[unit][part].entry_key_words;
        rsp_edw[unit][part] = rsp_entry_buffer[unit][part].entry_data_words;
    }
    lt_mrsp_info.rsp_entry_key_words = rsp_ekw[unit];
    lt_mrsp_info.rsp_entry_data_words = rsp_edw[unit];
    lt_mrsp_info.rsp_pdd_l_dtyp = NULL;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = tbl_inst;

    if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH &&
        !(flags & BCMFP_ENTRY_PRESEL)) {
        /*Return with entry exists error if key is conflciting with another
         * entry key. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_hash_same_key_get(unit, trans_id, stage_id, entry_id,
                                           req_flags, req_ltid,
                                           (void *)&pt_dyn_info,  &lt_mreq_info,
                                           &lt_mrsp_info,  &rsp_ltid,
                                           &rsp_flags,  &same_key));
        if (same_key == TRUE) {
            /* If the updated entry key conflicts with same entry key, continue with
             * same_key set to 1*/
            lt_mreq_info.same_key = 1;
        }
    }
    SHR_IF_ERR_EXIT(bcmptm_ltm_mreq_keyed_lt(unit,
                                             req_flags,
                                             req_ltid,
                                             (void *)&pt_dyn_info,
                                             NULL,
                                             BCMPTM_OP_INSERT,
                                             &lt_mreq_info,
                                             trans_id,
                                             &lt_mrsp_info,
                                             &rsp_ltid,
                                             &rsp_flags));
    /* Update slice id in group oper info */
    if (stage->flags & BCMFP_STAGE_KEY_TYPE_FIXED &&
        flags & BCMFP_ENTRY_FIRST_IN_GROUP) {
        group_oper_info->slice_id = attrs.slice_id;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_delete(int unit,
                   uint32_t flags,
                   uint32_t trans_id,
                   bcmfp_stage_id_t stage_id,
                   bcmltd_sid_t req_ltid,
                   bcmfp_group_id_t group_id,
                   bcmfp_entry_id_t entry_id,
                   uint32_t **ekw)
{
    bool is_presel = 0;
    uint8_t part = 0;
    uint8_t parts_count = 0;
    uint32_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t wsize = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_tcam_entry_attrs_t attrs;
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;
    int qos_prof_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    rv = bcmfp_group_oper_info_get(unit, stage_id, group_id, &group_oper_info);
    SHR_IF_ERR_EXIT(rv);

    sal_memset(&attrs, 0, sizeof(bcmptm_rm_tcam_entry_attrs_t));
    sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));

    /* for EM hash entry, we need to use hash entry attrs */
    if (!(stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH
        && !(flags & BCMFP_ENTRY_PRESEL))) {
        if (flags & BCMFP_ENTRY_PRESEL) {
            SHR_BITSET(attrs.flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
            if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
                SHR_BITSET(attrs.flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
            }
        }

        if (flags & BCMFP_ENTRY_LAST_IN_GROUP) {
            SHR_BITSET(attrs.flags.w, BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP);
        }

        /* In BCMFP group ids ranges from 1-<MaxId>  and in RM_TCAM, this range
         * is mapped to 0-<MaxId-1>  */
        attrs.group_id = group_oper_info->group_id - 1;

        attrs.group_prio = group_oper_info->group_prio;

        BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE(group_oper_info->group_mode,
                                              attrs.group_mode);

        attrs.num_presel_ids = group_oper_info->num_presel_ids;

        BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs.stage_id);

        attrs.pipe_id = group_oper_info->tbl_inst;
        lt_mreq_info.entry_attrs = &attrs;

        if (flags & BCMFP_ENTRY_PRESEL) {
            is_presel = 1;
        }
    } else {
        BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE(group_oper_info->group_mode,
                                               hash_attrs.entry_mode);
        lt_mreq_info.entry_attrs = &hash_attrs;
    }

    lt_mreq_info.entry_id = entry_id;
    lt_mreq_info.pdd_l_dtyp = NULL;
    lt_mreq_info.same_key = 0;

    SHR_IF_ERR_EXIT(
        bcmfp_group_tcam_size_get(unit,
                                  is_presel,
                                  stage_id,
                                  group_id,
                                  &wsize));
    lt_mreq_info.rsp_entry_key_buf_wsize = wsize;

    wsize = 0;
    SHR_IF_ERR_EXIT(
        bcmfp_group_policy_size_get(unit,
                                  is_presel,
                                  stage_id,
                                  group_id,
                                  &wsize));
    lt_mreq_info.rsp_entry_data_buf_wsize = wsize;

    sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));

    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit,
                                group_oper_info->flags,
                                &parts_count));

    for (part = 0; part < parts_count; part++) {
        req_ekw[unit][part] = req_entry_buffer[unit][part].entry_key_words;
        req_edw[unit][part] = req_entry_buffer[unit][part].entry_data_words;
        rsp_ekw[unit][part] = rsp_entry_buffer[unit][part].entry_key_words;
        rsp_edw[unit][part] = rsp_entry_buffer[unit][part].entry_data_words;
    }

    if (stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH
        && !(flags & BCMFP_ENTRY_PRESEL)) {
        lt_mreq_info.entry_key_words = ekw;
    } else {
        lt_mreq_info.entry_key_words = req_ekw[unit];
    }
    lt_mreq_info.entry_data_words = req_edw[unit];
    lt_mrsp_info.rsp_entry_key_words = rsp_ekw[unit];
    lt_mrsp_info.rsp_entry_data_words = rsp_edw[unit];
    lt_mrsp_info.rsp_pdd_l_dtyp = NULL;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = group_oper_info->tbl_inst;
    SHR_IF_ERR_EXIT(bcmptm_ltm_mreq_keyed_lt(unit,
                                             req_flags,
                                             req_ltid,
                                             (void *)&pt_dyn_info,
                                             NULL,
                                             BCMPTM_OP_DELETE,
                                             &lt_mreq_info,
                                             trans_id,
                                             &lt_mrsp_info,
                                             &rsp_ltid,
                                             &rsp_flags));

    if (stage->flags & BCMFP_STAGE_ACTION_QOS_PROFILE
        && !(flags & BCMFP_ENTRY_PRESEL)) {
        SHR_IF_ERR_EXIT(bcmfp_entry_qos_profile_id_get(unit, trans_id,
                                                       stage,
                                                       group_id,
                                                       rsp_edw[unit],
                                                       &qos_prof_idx));
        SHR_IF_ERR_EXIT(
             bcmfp_entry_qos_profile_uninstall(unit, trans_id,
                                                  stage, group_id,
                                                  qos_prof_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

