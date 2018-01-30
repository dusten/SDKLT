/*! \file bcmfp_oper_get.c
 *
 * BCMFP operational information get APIs.
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

#include <bsl/bsl.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*
 * \brief Get qualifier offset(s) and width(s) belongs
 *  to a partucular part. Caller will provide chain of HA
 *  elements(blk_id and blk_offset corresponds to first HA
 *  element in the chain) holding qualifiers offset(s) and
 *  width(s) information in all parts.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id of first HA element in the chain.
 * \param [in] blk_offset HA block offset of first HA element in the chain.
 * \param [in] part_id First/Second/Third Slice.
 * \param [out] q_offset Qualifer offset(s) and width(s) in part_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
STATIC int
bcmfp_qual_offset_info_get(int unit,
                           uint8_t blk_id,
                           uint32_t blk_offset,
                           bcmfp_part_id_t part_id,
                           bcmfp_qual_offset_info_t *q_offset)
{
    bool hit_status = FALSE;
    uint16_t offset_id = 0;
    uint16_t max_offset_id = 0;
    bcmfp_part_id_t part = 0;
    bcmfp_ha_element_t *ha_element = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(q_offset, SHR_E_PARAM);

    sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));
    /* Loop through the chain. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        ha_element = NULL;
        SHR_IF_ERR_EXIT(
            bcmfp_ha_element_get(unit, blk_id,
                                 blk_offset, &ha_element));
        /* Ideally all HA elements in the chain should have type
         * "QUAL_OFFSET_WIDTH_PART_ID". But in furture there may
         * be a requirement to keep other type of HA elements in
         * the same chain qualifier offset(s) and width(s) are
         * saved. So ignore if the HA element type is other than
         * "QUAL_OFFSET_WIDTH_PART_ID".
         */
        if (ha_element->type ==
            BCMFP_HA_ELEMENT_TYPE_QUAL_OFFSET_WIDTH_PART_ID) {
            /* HA element with type "QUAL_OFFSET_WIDTH_PART_ID" has
             * following 4 different fields embedded in its value.
             * 1. b0  - b1  --> part id
             * 2. b2  - b11 --> offset
             * 3. b12 - b21 --> width
             * 4. b22 - b31 --> offset id
             */
            part = ha_element->value & 0x3;
            if (part == part_id) {
                hit_status = TRUE;
                offset_id = (ha_element->value >> 22) & 0x3FF;
                if (max_offset_id < offset_id) {
                    max_offset_id = offset_id;
                }
                q_offset->offset[offset_id] =
                     (ha_element->value >> 2) & 0x3FF;
                q_offset->width[offset_id] =
                     (ha_element->value >> 12) & 0x3FF;
                q_offset->num_offsets = max_offset_id + 1;
                if (q_offset->width[offset_id] != 0) {
                    LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                            "offset_id=%d, offset=%d, width=%u\n"),
                            offset_id,
                            q_offset->offset[offset_id],
                            q_offset->width[offset_id]));
                }
            }
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
    if (hit_status == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_info_get(int unit,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_stage_oper_info_t **stage_oper_info)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    *stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_mode_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t *oper_mode)
{
    uint64_t value = 0;
    bcmltd_fid_t field;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper_mode, SHR_E_PARAM);

    if (stage_id == BCMFP_STAGE_ID_INGRESS) {
        field = FP_CONFIGt_FP_ING_OPERMODEf;
    } else if (stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {
        field = FP_CONFIGt_FP_EM_OPERMODE_PIPEUNIQUEf;
    } else if (stage_id == BCMFP_STAGE_ID_EGRESS) {
        field = FP_CONFIGt_FP_EGR_OPERMODE_PIPEUNIQUEf;
    } else if (stage_id == BCMFP_STAGE_ID_LOOKUP) {
        field = FP_CONFIGt_FP_VLAN_OPERMODE_PIPEUNIQUEf;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    *oper_mode = value;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t **oinfo)
{
    void *ha_mem = NULL;
    uint8_t idx = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oinfo, SHR_E_PARAM);

    *oinfo = NULL;
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->group_oper_info_seg;
    group_oper_info =
         (bcmfp_group_oper_info_t *)((uint8_t *)ha_mem + offset);
    if (group_id == 0) {
        *oinfo = group_oper_info;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    for (idx = 0; idx < stage_oper_info->num_groups; idx++)  {
        if (group_oper_info[idx].group_id == 0)  {
            continue;
        }
        if (group_oper_info[idx].group_id == group_id) {
            *oinfo = &(group_oper_info[idx]);
        }
    }
    if (*oinfo == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             uint32_t **keygen_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_prof_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));
    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE && pipe_id != -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    } else if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE &&
               pipe_id == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
         *keygen_prof_ref_count = (uint32_t *)((uint8_t *)ha_mem +
                       stage_oper_info->keygen_prof_ref_count_seg +
                       (sizeof(uint32_t) *
                        stage_oper_info->num_keygen_prof *
                        pipe_id));
    } else {
         *keygen_prof_ref_count = (uint32_t *)((uint8_t *)ha_mem +
                                  stage_oper_info->keygen_prof_ref_count_seg);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             uint32_t **action_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_prof_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));
    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE && pipe_id != -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    } else if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE &&
               pipe_id == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
         *action_prof_ref_count = (uint32_t *)((uint8_t *)ha_mem +
                       stage_oper_info->action_prof_ref_count_seg +
                       (sizeof(uint32_t) *
                        stage_oper_info->num_action_prof *
                        pipe_id));
    } else {
         *action_prof_ref_count = (uint32_t *)((uint8_t *)ha_mem +
                                  stage_oper_info->action_prof_ref_count_seg);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qos_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             uint32_t **qos_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qos_prof_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));
    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE && pipe_id != -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    } else if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE &&
               pipe_id == -1) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
         *qos_prof_ref_count = (uint32_t *)((uint8_t *)ha_mem +
                       stage_oper_info->qos_prof_ref_count_seg +
                       (sizeof(uint32_t) *
                        stage_oper_info->num_qos_prof *
                        pipe_id));
    } else {
         *qos_prof_ref_count = (uint32_t *)((uint8_t *)ha_mem +
                               stage_oper_info->qos_prof_ref_count_seg);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_res_id_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t *action_res_id)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_res_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                                  group_id, &group_oper_info));
    *action_res_id = group_oper_info->action_res_id;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_qual_offset_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_qual_t qual_id,
                        bcmfp_qual_offset_info_t *q_offset)
{
    bool qid_found = FALSE;
    bool hit_status = FALSE;
    bool qid_blk_id_found = FALSE;
    bool qid_blk_offset_found = FALSE;
    uint8_t blk_id = 0;
    uint32_t blk_offset = 0;
    uint8_t qid_blk_id = 0;
    uint32_t qid_blk_offset = 0;
    bcmfp_qual_t ha_qual_id = BCMFP_QUAL_COUNT;
    bcmfp_ha_element_t *ha_element = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(q_offset, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                                  group_id, &group_oper_info));
    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        ha_element = NULL;
        SHR_IF_ERR_EXIT(
            bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
        if (ha_element->type ==
            BCMFP_HA_ELEMENT_TYPE_SHORT_QID_AND_NUM_OFFSETS) {
            ha_qual_id = ha_element->value & 0xFFFF;
            qid_found = TRUE;
        } else if (ha_element->type == BCMFP_HA_ELEMENT_TYPE_HA_BLK_ID) {
            qid_blk_id = ha_element->value;
            qid_blk_id_found = TRUE;
        } else if (ha_element->type == BCMFP_HA_ELEMENT_TYPE_HA_BLK_OFFSET) {
            qid_blk_offset = ha_element->value;
            qid_blk_offset_found = TRUE;
        }
        if (qid_found == TRUE &&
            qid_blk_id_found == TRUE &&
            qid_blk_offset_found == TRUE) {
            if (qual_id == ha_qual_id) {
                hit_status = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                            "---Qualifier(%d) offset(s) and"
                            " width(s) in part(%d)---\n"),
                            qual_id, part_id));
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmfp_qual_offset_info_get(unit, qid_blk_id,
                                               qid_blk_offset,
                                               part_id, q_offset));
                break;
            }
            qid_found = FALSE;
            qid_blk_id_found = FALSE;
            qid_blk_offset_found = FALSE;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
    if (hit_status == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_flags_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      uint32_t *group_flags)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_flags, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                     group_id, &group_oper_info));

    *group_flags = group_oper_info->flags;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_tbl_inst_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      int *tbl_inst)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_inst, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                     group_id, &group_oper_info));

    *tbl_inst = group_oper_info->tbl_inst;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_ext_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_ext_sel_t **ext_codes)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                     group_id, &group_oper_info));
    *ext_codes = &(group_oper_info->ext_codes[part_id]);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_qual_present(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id,
                         bcmfp_qual_t qual_id,
                         bool *qual_present)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_ha_element_t *ha_element = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit, stage_id,
                      group_id, &group_oper_info));

    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;
    *qual_present = FALSE;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmfp_ha_element_get(unit, blk_id,
                      blk_offset, &ha_element));
        if ((ha_element->type ==
            BCMFP_HA_ELEMENT_TYPE_SHORT_QID_AND_NUM_OFFSETS) &&
            (ha_element->value == qual_id)) {
            *qual_present = TRUE;
            break;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
exit:
    SHR_FUNC_EXIT();
}

