/*! \file bcmfp_oper_set.c
 *
 * BCMFP operational information set APIs.
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*
 * \brief During FP component initialization time, every stage
 *  gets HA space to save group operational information of max
 *  number of groups that can be created in taht stage. When
 *  some group is created successfully on run time, one of the
 *  free group operational information blocks will acquired and
 *  assigned to that group id. Note that these consecutive group
 *  operational information blocks in stages HA space are not
 *  indexed by group id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFp stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] oinfo Allocated group operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
STATIC int
bcmfp_group_oper_info_acquire(int unit,
                      bcmfp_stage_id_t stage_id,
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
    for (idx = 0; idx < stage_oper_info->num_groups; idx++)  {
        if (group_oper_info[idx].group_id == 0)  {
            break;
        }
    }
    if (idx == stage_oper_info->num_groups) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    *oinfo = &(group_oper_info[idx]);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Upon successfull creation of group, offset(s) and
 *  width(s) information of all qualifiers in all parts of
 *  the group will be saved in HA area. A single qualifier
 *  may be part of different slices of the group(if the group
 *  is multi wide). So this function may be called multiple
 *  times for a single qualifier. offset(s) and width(s) of a
 *  single qualifier in all parts are saved in single HA
 *  elements chain.
 *
 *  Groups Qualifiers offset(s) and width(s) information is
 *  stored in two types of HA element chains.
 *
 *  Type-1: Each Qualifier has 3 consecutive HA elements in this
 *  chain. First HA element element holds qualifier id. Second HA
 *  element holds head HA block id of another HA element chain
 *  holding offset(s) and width(s) of the qualifier in all parts.
 *  Third HA element holds head HA block offset of HA block id
 *  stored in second HA element.Only one HA element chain of this
 *  type is required per group. Head HA block id and offset comes
 *  from the group operational information.Number of HA elements
 *  in this chain is equal to (3 * number of qualifiers present in
 *  the group.
 *
 *  Type-2:Each qualifier has one HA element chain of this type.
 *  Head HA blok id and offsets to this chain comes from Type-1
 *  HA element chain. Number of HA elements in this chain depends
 *  on the number of unique offsets needs to be saved per qualifier.
 *  Number of HA element chains is equal to number of qualifiers
 *  present in the group.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the group.
 * \param [in] qual_id Qualifier identifier.
 * \param [out] q_offset Qualifiers offset(s) and width(s).
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
STATIC int
bcmfp_group_qual_offset_info_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_qual_t qual_id,
                        bcmfp_qual_offset_info_t *q_offset)
{
    uint16_t idx = 0;
    uint8_t blk_id = 0;
    uint8_t qid_blk_id = 0;
    uint8_t qid_offset_blk_id = 0;
    uint8_t first_qid_offset_blk_id = 0;
    uint32_t blk_offset = 0;
    uint32_t qid_blk_offset = 0;
    uint32_t qid_offset_blk_offset = 0;
    uint32_t first_qid_offset_blk_offset = 0;
    uint32_t value = 0;
    uint16_t num_ha_elements = 0;
    bcmfp_ha_element_t *ha_element = NULL;
    bcmfp_ha_element_t *qid_ha_element = NULL;
    bcmfp_ha_element_t *qid_offset_ha_element = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(q_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                                     group_id, &group_oper_info));
    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;

    /* Check if offset(s) and width(s) of qualifier is already
     * present in group operational information. This function
     * being called multiple times if group is multi wide and
     * qualifier is present in multiple slices.
     */
    while ((blk_id != BCMFP_HA_BLK_ID_INVALID) &&
           (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_element_get(unit, blk_id,
                     blk_offset, &ha_element));
        if (ha_element->type ==
            BCMFP_HA_ELEMENT_TYPE_SHORT_QID_AND_NUM_OFFSETS) {
             if (ha_element->value == qual_id) {
                 qid_ha_element = ha_element;
                 break;
             }
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }

    /* Caliculate the number of HA elements required to save
     * qualifiers offset(s) and width(s) information.
     */
    num_ha_elements = 0;
    for (idx = 0; idx < q_offset->num_offsets; idx++) {
        if (q_offset->width[idx] == 0) {
            continue;
        }
        num_ha_elements++;
    }

    /* Allocate required number of HA elements from HA blocks
     * created in run time.
     */
    SHR_IF_ERR_EXIT(
        bcmfp_ha_elements_acquire(unit,
                                  num_ha_elements,
                                  &qid_offset_ha_element,
                                  &qid_offset_blk_id,
                                  &qid_offset_blk_offset));

    /* First time offset info of this qualifier is pushed to
     * group oper info.
     */
    if (qid_ha_element == NULL) {
        SHR_IF_ERR_EXIT(
            bcmfp_ha_elements_acquire(unit, 3,
                                      &qid_ha_element,
                                      &qid_blk_id,
                                      &qid_blk_offset));
        /* Add (qual_id, block_id, block_offset) to groups Type-1
         * HA element chain.
         */
        ha_element = qid_ha_element;
        ha_element->type = BCMFP_HA_ELEMENT_TYPE_SHORT_QID_AND_NUM_OFFSETS;
        ha_element->value = qual_id;
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
        ha_element->type = BCMFP_HA_ELEMENT_TYPE_HA_BLK_ID;
        ha_element->value = qid_offset_blk_id;
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
        ha_element->type = BCMFP_HA_ELEMENT_TYPE_HA_BLK_OFFSET;
        ha_element->value = qid_offset_blk_offset;
        /* Add this 3 HA element chain to Type-1 HA element chain of group. */
        ha_element->next_blk_id = group_oper_info->qual_offset_info.blk_id;
        ha_element->next_blk_offset =
                              group_oper_info->qual_offset_info.blk_offset;
        group_oper_info->qual_offset_info.blk_id = qid_blk_id;
        group_oper_info->qual_offset_info.blk_offset = qid_blk_offset;
    } else {
        /* Add the new offset(s) and width(s) chain to the end of
         * Type-2 HA element chain of the qualifier.
         */
        blk_id = qid_ha_element->next_blk_id;
        blk_offset = qid_ha_element->next_blk_offset;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
        first_qid_offset_blk_id = ha_element->value;
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
        first_qid_offset_blk_offset = ha_element->value;
        blk_id = first_qid_offset_blk_id;
        blk_offset = first_qid_offset_blk_offset;
        ha_element = NULL;
        while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
               blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_ha_element_get(unit, blk_id,
                         blk_offset, &ha_element));
            blk_id = ha_element->next_blk_id;
            blk_offset = ha_element->next_blk_offset;
        }
        if (ha_element != NULL) {
            ha_element->next_blk_id = qid_offset_blk_id;
            ha_element->next_blk_offset = qid_offset_blk_offset;
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    /* All the HA element chains are linked properly. Now update the
     * Type-2 HA element values.
     */
    blk_id = qid_offset_blk_id;
    blk_offset = qid_offset_blk_offset;
    for (idx = 0; idx < q_offset->num_offsets; idx++) {
        if (q_offset->width[idx] == 0) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_element_get(unit, blk_id, blk_offset,
                                 &qid_offset_ha_element));
        qid_offset_ha_element->type =
            BCMFP_HA_ELEMENT_TYPE_QUAL_OFFSET_WIDTH_PART_ID;
        value = part_id;
        value |= (q_offset->offset[idx] << 2);
        value |= (q_offset->width[idx] << 12);
        value |= (idx << 22);
        qid_offset_ha_element->value = value;
        blk_id = qid_offset_ha_element->next_blk_id;
        blk_offset = qid_offset_ha_element->next_blk_offset;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief When group is deleted, offset(s) and width(s) information
 *  of all its qualifiers needs to be deleted. This involves releasing
 *  the Type-1 and Type-2 HA element chains of Group. To know more
 *  about Type-1 and Type-2 HA element chains refer the function header
 *  of bcmfp_group_qual_offset_info_set.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
STATIC int
bcmfp_group_qual_offset_info_reset(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_group_id_t group_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t q_offset_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t q_offset_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_ha_element_t *ha_element = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                                  group_id, &group_oper_info));
    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;
    /* First release Type-2 HA element chains of the group. */
    while ((blk_id != BCMFP_HA_BLK_ID_INVALID) &&
           (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_element_get(unit, blk_id,
                     blk_offset, &ha_element));
        if (ha_element->type ==
            BCMFP_HA_ELEMENT_TYPE_HA_BLK_ID) {
            q_offset_blk_id = ha_element->value;
        } else if (ha_element->type ==
                   BCMFP_HA_ELEMENT_TYPE_HA_BLK_OFFSET) {
            q_offset_blk_offset = ha_element->value;
        }
        if ((q_offset_blk_id !=
             BCMFP_HA_BLK_ID_INVALID) &&
            (q_offset_blk_offset !=
             BCMFP_HA_BLK_OFFSET_INVALID)) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_ha_elements_release(unit, q_offset_blk_id,
                                          q_offset_blk_offset));
            q_offset_blk_id = BCMFP_HA_BLK_ID_INVALID;
            q_offset_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
    /* Then release Type-1 HA element chain of the group. */
    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_ha_elements_release(unit, blk_id, blk_offset));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_mode_set(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t oper_mode)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));
    stage_oper_info->oper_mode = oper_mode;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_res_id_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t action_res_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                              group_id, &group_oper_info));
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));
    group_oper_info->action_res_id = action_res_id;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_set(int unit,
                          bcmfp_group_t *fg)
{
    uint8_t idx = 0;
    uint8_t parts_count = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_qual_t qual_id;
    bcmfp_part_id_t part_id = 0;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_qual_offset_info_t  *q_offset = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(fg->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmfp_group_oper_info_get(unit, fg->stage_id,
            fg->group_id, &group_oper_info), SHR_E_NOT_FOUND);
    if (group_oper_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_group_oper_info_acquire(unit,
                                              fg->stage_id,
                                              &group_oper_info));
    }
    group_oper_info->valid = TRUE;
    group_oper_info->group_id = fg->group_id;
    group_oper_info->group_prio = fg->group_prio;
    group_oper_info->tbl_inst = fg->pipe_id;
    group_oper_info->slice_id= fg->lookup_id;
    group_oper_info->group_mode = fg->group_mode;
    group_oper_info->group_slice_mode = fg->group_slice_mode;
    group_oper_info->flags = fg->flags;
    group_oper_info->action_res_id = fg->action_res_id;
    group_oper_info->num_presel_ids = fg->num_presel_ids;

    group_oper_info->ref_count = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_parts_count(unit, fg->flags, &parts_count));
    for (part_id = 0; part_id < parts_count; part_id++) {
        for (idx = 0; idx < fg->qual_arr[0][part_id].size; idx++) {
            qual_id = fg->qual_arr[0][part_id].qid_arr[idx];
            q_offset = &(fg->qual_arr[0][part_id].offset_arr[idx]);
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_group_qual_offset_info_set(unit,
                                                 fg->stage_id,
                                                 fg->group_id,
                                                 part_id,
                                                 qual_id,
                                                 q_offset));
        }
        sal_memcpy(&(group_oper_info->ext_codes[part_id]),
                   &(fg->ext_codes[part_id]),
                   sizeof(bcmfp_ext_sel_t));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_qual_offset_info_reset(unit, stage_id, group_id));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                                  group_id, &group_oper_info));
    group_oper_info->valid = FALSE;
    group_oper_info->qual_offset_info.blk_id =
                                     BCMFP_HA_BLK_ID_INVALID;
    group_oper_info->qual_offset_info.blk_offset =
                                 BCMFP_HA_BLK_OFFSET_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_update(int unit,
                             bcmfp_group_t *fg)
{
    uint8_t idx = 0;
    uint8_t parts_count = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_qual_t qual_id;
    bcmfp_part_id_t part_id = 0;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_qual_offset_info_t  *q_offset = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(fg->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, fg->stage_id,
                     fg->group_id, &group_oper_info));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_group_parts_count(unit, fg->flags, &parts_count));
    for (part_id = 0; part_id < parts_count; part_id++) {
        for (idx = 0; idx < fg->qual_arr[0][part_id].size; idx++) {
            qual_id = fg->qual_arr[0][part_id].qid_arr[idx];
            q_offset = &(fg->qual_arr[0][part_id].offset_arr[idx]);
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_group_qual_offset_info_set(unit,
                                                 fg->stage_id,
                                                 fg->group_id,
                                                 part_id,
                                                 qual_id,
                                                 q_offset));
        }
        sal_memcpy(&(group_oper_info->ext_codes[part_id]),
                   &(fg->ext_codes[part_id]),
                   sizeof(bcmfp_ext_sel_t));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_params_set(int unit,
                       bcmfp_stage_id_t stage_id,
                       int pipe_id,
                       bcmfp_group_id_t group_id,
                       uint32_t group_prio,
                       bcmfp_group_slice_mode_t group_slice_mode,
                       uint8_t port_pkt_type)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmfp_group_oper_info_get(unit, stage_id,
            group_id, &group_oper_info), SHR_E_NOT_FOUND);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));

    if (group_oper_info == NULL) {
        /*If the given group Id is not found */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_group_oper_info_acquire(unit,
                                              stage_id,
                                              &group_oper_info));
    }
    group_oper_info->group_id = group_id;
    group_oper_info->tbl_inst = pipe_id;
    group_oper_info->group_prio = group_prio;
    group_oper_info->group_slice_mode = group_slice_mode;
    group_oper_info->port_pkt_type = port_pkt_type;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_params_reset(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmfp_group_oper_info_get(unit, stage_id,
            group_id, &group_oper_info), SHR_E_NOT_FOUND);

    if (group_oper_info == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));

    group_oper_info->group_id = 0;
    group_oper_info->group_prio = 0;
    group_oper_info->tbl_inst = 0;
    group_oper_info->group_slice_mode = 0;
    group_oper_info->port_pkt_type = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_info_init(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t blk_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = 0;
    uint8_t backup_blk_id = 0;
    uint16_t idx = 0;
    uint32_t size = 0;
    uint32_t *ref_counts = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    /* Initialize the block header. */
    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
    blk_hdr->blk_size = blk_size;
    blk_hdr->free_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    blk_hdr->free_ha_element_count = 0;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    blk_hdr->backup_blk_id = backup_blk_id;

    /* Initialize different fields in stage operational information. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));
    stage_oper_info->trans_state = BCMFP_TRANS_STATE_IDLE;
    stage_oper_info->num_groups = stage->misc_info->num_groups;
    stage_oper_info->num_keygen_prof = stage->misc_info->num_keygen_prof;
    stage_oper_info->num_action_prof = stage->misc_info->num_action_prof;
    stage_oper_info->num_qos_prof = stage->misc_info->num_qos_prof;

    /* Initialize the offset to group opertaional information of the
     * stage in the fixed HA block created for the stage.
     */
    stage_oper_info->group_oper_info_seg = sizeof(bcmfp_stage_oper_info_t) +
                                           sizeof(bcmfp_ha_blk_hdr_t);
    group_oper_info = (bcmfp_group_oper_info_t *)((uint8_t *)ha_mem +
                                     stage_oper_info->group_oper_info_seg);
    size = stage_oper_info->num_groups * sizeof(bcmfp_group_oper_info_t);
    /* Initialize the fields in group operational information for maximum
     * possible groups in the stage.
     */
    sal_memset(group_oper_info, 0, size);
    for (idx = 0; idx < stage_oper_info->num_groups; idx++) {
         group_oper_info[idx].valid = FALSE;
         group_oper_info[idx].qual_offset_info.blk_id =
                                     BCMFP_HA_BLK_ID_INVALID;
         group_oper_info[idx].qual_offset_info.blk_offset =
                                   BCMFP_HA_BLK_OFFSET_INVALID;
    }

    /* Initialize the offset to keygen profile table entries reference
     * counts of the stage in the fixed HA block created for the stage.
     */
    if (stage_oper_info->num_keygen_prof != 0) {
        size = sizeof(bcmfp_ha_blk_hdr_t) +
               sizeof(bcmfp_stage_oper_info_t) +
               (stage_oper_info->num_groups *
                sizeof(bcmfp_group_oper_info_t));
        stage_oper_info->keygen_prof_ref_count_seg = size;
        ref_counts = (uint32_t * )((uint8_t *)ha_mem +
                     stage_oper_info->keygen_prof_ref_count_seg);
        size = stage_oper_info->num_keygen_prof * sizeof(uint32_t);
        sal_memset(ref_counts, 0, size);
    }

    /* Initialize the offset to action profile table entries reference
     * counts of the stage in the fixed HA block created for the stage.
     */
    if (stage_oper_info->num_action_prof != 0) {
        size = sizeof(bcmfp_ha_blk_hdr_t) +
               sizeof(bcmfp_stage_oper_info_t) +
               (stage_oper_info->num_groups *
                sizeof(bcmfp_group_oper_info_t)) +
               (stage_oper_info->num_keygen_prof *
               sizeof(uint32_t));
        stage_oper_info->action_prof_ref_count_seg = size;
        ref_counts = (uint32_t * )((uint8_t *)ha_mem +
                     stage_oper_info->action_prof_ref_count_seg);
        size = stage_oper_info->num_action_prof * sizeof(uint32_t);
        sal_memset(ref_counts, 0, size);
    }

    /* Initialize the offset to qos profile table entries reference
     * counts of the stage in the fixed HA block created for the stage.
     */
    if (stage_oper_info->num_qos_prof != 0) {
        size = sizeof(bcmfp_ha_blk_hdr_t) +
               sizeof(bcmfp_stage_oper_info_t) +
               (stage_oper_info->num_groups *
                sizeof(bcmfp_group_oper_info_t)) +
               (stage_oper_info->num_keygen_prof *
                sizeof(uint32_t)) +
               (stage_oper_info->num_action_prof *
                sizeof(uint32_t));
        stage_oper_info->qos_prof_ref_count_seg = size;
        ref_counts = (uint32_t * )((uint8_t *)ha_mem +
                     stage_oper_info->qos_prof_ref_count_seg);
        size = stage_oper_info->num_qos_prof * sizeof(uint32_t);
        sal_memset(ref_counts, 0, size);
    }

exit:
    SHR_FUNC_EXIT();
}
