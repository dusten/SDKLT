/*! \file bcmfp_ha.c
 *
 * BCMFP HA memory management APIs.
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
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmlrd/bcmlrd_map.h>
#include <shr/shr_util.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_HA

void *ha_ptrs[BCMDRD_CONFIG_MAX_UNITS][BCMFP_HA_BLK_ID_MAX];

#define BCMFP_HA_PTR(unit, sub_comp) ha_ptrs[unit][sub_comp]

STATIC int
bcmfp_ha_stage_oper_size_get(int unit,
                 bcmfp_stage_id_t stage_id,
                 uint32_t *size)
{
    bcmfp_stage_t *stage = NULL;
    /* global mode */
    uint32_t pipe_count = 1;
    uint32_t pipe_map;
    bcmfp_stage_oper_mode_t oper_mode;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    *size = 0;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_stage_get(unit, stage_id, &stage));
    if (stage->misc_info == NULL) {
       SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Default configuration is GLOBAL*/
    BCMFP_RET_VAL_ASSIGN(bcmlrd_map_get(unit, FP_CONFIGt, &map));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_IF_ERR_EXIT
            (bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));

        if(oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
            /* Get the number of pipes */
            SHR_IF_ERR_EXIT(bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
            pipe_count = shr_util_popcount(pipe_map);
        }
    }
    *size += sizeof(bcmfp_ha_blk_hdr_t);
    *size += sizeof(bcmfp_stage_oper_info_t);
    *size += stage->misc_info->num_groups *
                sizeof(bcmfp_group_oper_info_t);
    if (stage->misc_info->num_keygen_prof != 0) {
        *size += pipe_count *
                 stage->misc_info->num_keygen_prof *
                 sizeof(uint32_t);
    }
    if (stage->misc_info->num_action_prof != 0) {
        *size += pipe_count *
                 stage->misc_info->num_action_prof *
                 sizeof(uint32_t);
    }
    if (stage->misc_info->num_qos_prof != 0) {
        *size += pipe_count *
                 stage->misc_info->num_qos_prof *
                 sizeof(uint32_t);
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ha_blk_create(int unit,
                    uint8_t blk_id,
                    uint32_t blk_size)
{
    void *ha_ptr = NULL;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t idx = 0;
    uint32_t num_ha_elements = 0;
    uint32_t size = blk_size;
    uint32_t free_blk_offset = 0;
    bcmfp_ha_element_t *ha_element = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_id > BCMFP_HA_BLK_ID_MAX) ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_id <= BCMFP_HA_BLK_ID_INVALID) ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_VERBOSE_EXIT(
        (blk_size % sizeof(bcmfp_ha_element_t) != 0) ?
                              SHR_E_PARAM : SHR_E_NONE);
    size += sizeof(bcmfp_ha_blk_hdr_t);
    BCMFP_HA_PTR(unit, blk_id) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 &size);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
         "is created with size %0d\n"), blk_id, blk_size));
    /* Inialize the HA elements which starts after block header. */
    ha_ptr = BCMFP_HA_PTR(unit, blk_id);
    ha_element = (bcmfp_ha_element_t *)((uint8_t *)ha_ptr +
                                  sizeof(bcmfp_ha_blk_hdr_t));
    free_blk_offset = sizeof(bcmfp_ha_blk_hdr_t);
    num_ha_elements = blk_size / sizeof(bcmfp_ha_element_t);
     if (!num_ha_elements)  {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
                            "is created with size %0d\n"), blk_id, blk_size));
    }
    SHR_IF_ERR_EXIT(
        (num_ha_elements == 0) ? SHR_E_INTERNAL : SHR_E_NONE);
    /* Initialize the TV(type and value) in HA elements to
     * default values. And link all HA elements to its next
     * HA element.
     */
    for (idx = 0; idx < (num_ha_elements -1); idx++) {
         free_blk_offset += sizeof(bcmfp_ha_element_t);
         ha_element[idx].type = BCMFP_HA_ELEMENT_TYPE_NONE;
         ha_element[idx].value = 0;
         ha_element[idx].next_blk_id = blk_id;
         ha_element[idx].next_blk_offset = free_blk_offset;
    }
    /* Last HA element in the block initialized seperately as
     * next_blk_id and next_blk_offset are set to INVALID values.
     */
    ha_element[idx].type = BCMFP_HA_ELEMENT_TYPE_NONE;
    ha_element[idx].value = 0;
    ha_element[idx].next_blk_id = BCMFP_HA_BLK_ID_INVALID;
    ha_element[idx].next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    /* Initialize the block header. */
    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
    blk_hdr->free_blk_offset = sizeof(bcmfp_ha_blk_hdr_t);
    blk_hdr->free_ha_element_count = num_ha_elements;
    blk_hdr->blk_size = size;
    blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    /* Create the back up block Id. */
    SHR_IF_ERR_EXIT(
        bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    blk_hdr->backup_blk_id = backup_blk_id;
    BCMFP_HA_PTR(unit, backup_blk_id) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_FP_COMP_ID,
                                 backup_blk_id,
                                 &size);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMFP:HA memory with block ID:%0d "
         "is created with size %0d\n"), backup_blk_id, blk_size));
    /* Copy the Active block to backup block. */
    sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
               BCMFP_HA_PTR(unit, blk_id), size);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_mem_get(int unit,
                 uint8_t sub_id,
                 void **ha_mem)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ha_mem, SHR_E_PARAM);

    BCMFP_HA_BLK_ID_VALIDATE(unit, sub_id);
    *ha_mem = BCMFP_HA_PTR(unit, sub_id);
    if (*ha_mem == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_mem_set(int unit,
                 uint8_t sub_id,
                 void *ha_mem)
{
    SHR_FUNC_ENTER(unit);
    BCMFP_HA_BLK_ID_VALIDATE(unit, sub_id);
    BCMFP_HA_PTR(unit, sub_id) = ha_mem;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_blk_hdr_get(int unit, uint8_t sub_id,
                     bcmfp_ha_blk_hdr_t **blk_hdr)
{
    void *ha_mem = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(blk_hdr, SHR_E_PARAM);

    BCMFP_HA_BLK_ID_VALIDATE(unit, sub_id);
    ha_mem = BCMFP_HA_PTR(unit, sub_id);
    if (ha_mem == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    *blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_backup_blk_id_get(int unit,
                           uint8_t blk_id,
                           uint8_t *backup_blk_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(backup_blk_id, SHR_E_PARAM);

    BCMFP_HA_BLK_ID_VALIDATE(unit, blk_id);

    if (blk_id >= BCMFP_HA_BLK_ID_FIXED_MIN &&
        blk_id <= BCMFP_HA_BLK_ID_FIXED_MAX) {
        *backup_blk_id = blk_id + BCMFP_HA_BLK_ID_FIXED_MAX;
    } else if (blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN &&
               blk_id <= BCMFP_HA_BLK_ID_DYNAMIC_MAX) {
        *backup_blk_id = blk_id + BCMFP_HA_BLK_ID_DYNAMIC_MAX;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP: HA back up block ID:%0d \n"), blk_id));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_element_get(int unit,
                     uint8_t blk_id,
                     uint32_t blk_offset,
                     bcmfp_ha_element_t **ha_element)
{
    void *ha_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ha_element, SHR_E_PARAM);

    *ha_element = NULL;

    BCMFP_HA_BLK_ID_VALIDATE(unit, blk_id);

    BCMFP_HA_BLK_OFFSET_VALIDATE(unit, blk_id, blk_offset);

    ha_ptr = BCMFP_HA_PTR(unit, blk_id);

    *ha_element = (bcmfp_ha_element_t *)((uint8_t *)ha_ptr + blk_offset);
exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_ha_elements_release(int unit, uint8_t blk_id, uint32_t blk_offset)
{
    void *ha_ptr = NULL;
    uint8_t curr_blk_id = 0;
    uint8_t next_blk_id = 0;
    uint32_t curr_blk_offset = 0;
    uint32_t next_blk_offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_element_t *curr_ha_element = NULL;

    SHR_FUNC_ENTER(unit);

    curr_blk_id = blk_id;
    curr_blk_offset = blk_offset;
    while ((curr_blk_id != BCMFP_HA_BLK_ID_INVALID) &&
         (curr_blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_EXIT
            (bcmfp_trans_state_set(unit, curr_blk_id,
                            BCMFP_TRANS_STATE_UC_A));
        SHR_IF_ERR_EXIT(
            bcmfp_ha_element_get(unit, curr_blk_id,
                 curr_blk_offset, &curr_ha_element));
        next_blk_id = curr_ha_element->next_blk_id;
        next_blk_offset = curr_ha_element->next_blk_offset;
        ha_ptr = BCMFP_HA_PTR(unit, curr_blk_id);
        if (ha_ptr == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(_func_unit, "%s\n"),
                       shr_errmsg(SHR_E_INTERNAL)));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
        curr_ha_element->type = BCMFP_HA_ELEMENT_TYPE_NONE;
        curr_ha_element->value = 0;
        curr_ha_element->next_blk_id = curr_blk_id;
        curr_ha_element->next_blk_offset = blk_hdr->free_blk_offset;
        blk_hdr->free_blk_offset = curr_blk_offset;
        blk_hdr->free_ha_element_count++;
        curr_blk_id = next_blk_id;
        curr_blk_offset = next_blk_offset;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_elements_acquire(int unit,
                    uint16_t req_ha_elements,
                    bcmfp_ha_element_t **ha_elements,
                    uint8_t *first_ha_element_blk_id,
                    uint32_t *first_ha_element_blk_offset)
{
    void *ha_ptr = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t first_free_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint16_t pooled_ha_elements = 0;
    uint32_t blk_size = 0;
    uint32_t blk_offset = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_element_t *free_ha_element = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ha_elements, SHR_E_PARAM);
    SHR_NULL_CHECK(first_ha_element_blk_id, SHR_E_PARAM);
    SHR_NULL_CHECK(first_ha_element_blk_offset, SHR_E_PARAM);

    /* Count the total number of free HA elements in all
     * the HA blocks(created in run time) together.
     */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = BCMFP_HA_PTR(unit, blk_id);
         if (ha_ptr == NULL) {
            if (first_free_blk_id == BCMFP_HA_BLK_ID_INVALID) {
                first_free_blk_id = blk_id;
            }
            continue;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
             continue;
         }
         pooled_ha_elements += blk_hdr->free_ha_element_count;
         if (pooled_ha_elements >= req_ha_elements) {
             break;
         }
    }

    /* There is no enough room and no block ids left over to use. */
    if ((pooled_ha_elements < req_ha_elements) &&
        (first_free_blk_id == BCMFP_HA_BLK_ID_INVALID)) {
        
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    /* There is no enough room and free block ids are there then
     * create a new HA block.
     */
    if ((pooled_ha_elements < req_ha_elements) &&
        (first_free_blk_id != BCMFP_HA_BLK_ID_INVALID)) {
        blk_size = 4 * 1024 * sizeof(bcmfp_ha_element_t);
        SHR_IF_ERR_EXIT(
            bcmfp_ha_blk_create(unit, first_free_blk_id, blk_size));
    }

    pooled_ha_elements = 0;
    /* Acquire the required number of HA elements. */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = BCMFP_HA_PTR(unit, blk_id);
         /* Skip un allocated HA blocks */
         if (ha_ptr == NULL) {
             continue;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
         /* Skip if HA block doesnot have free HA elements. */
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }
         blk_offset = blk_hdr->free_blk_offset;
         if (free_ha_element != NULL) {
             free_ha_element->next_blk_id = blk_id;
             free_ha_element->next_blk_offset = blk_offset;
         }
         /* Acquired free HA elements from the current block id. */
         while (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
             SHR_IF_ERR_EXIT(
                 bcmfp_ha_element_get(unit, blk_id,
                          blk_offset, &free_ha_element));
             /* If this is the first free HA element record its
              *  block id and block offset.
              */
             if (*ha_elements == NULL) {
                 *ha_elements = free_ha_element;
                 *first_ha_element_blk_id = blk_id;
                 *first_ha_element_blk_offset = blk_offset;
             }
             pooled_ha_elements++;
             /* Change the block transaction state to Uncommitted Active as
              * some free HA elements in this block are going to be used.
              */
             SHR_IF_ERR_EXIT
                 (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));

             /* Remove the HA element from free HA elements linked list
              * of the HA block and update the number of free HA elements.
              */
             blk_hdr->free_blk_offset = free_ha_element->next_blk_offset;
             blk_hdr->free_ha_element_count--;
             /* Stop if required number of HA elements are acquired. */
             if (pooled_ha_elements == req_ha_elements) {
                 break;
             }
             /* Move to the next free HA element in the HA block. */
             blk_offset = free_ha_element->next_blk_offset;
         }
         /* Stop if required number of HA elements are acquired. */
         if (pooled_ha_elements == req_ha_elements) {
             break;
         }
    }
    /* End of the acquired HA elements linked list. */
    if (free_ha_element != NULL)  {
        free_ha_element->next_blk_id = BCMFP_HA_BLK_ID_INVALID;
        free_ha_element->next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    }

exit:
    /* Release the acquired HA elements if there is an error. */
    if (SHR_FUNC_ERR() && *ha_elements != NULL) {
        SHR_IF_ERR_CONT(
            bcmfp_ha_elements_release(unit,
                       *first_ha_element_blk_id,
                       *first_ha_element_blk_offset));
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_cleanup(int unit)
{
    uint8_t blk_id = 0;
    uint8_t backup_blk_id = 0;

    SHR_FUNC_ENTER(unit);

    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_INGRESS) != NULL) {
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_INGRESS));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA memory for "
                       "INGRESS stage is freed\n")));
        blk_id = BCMFP_HA_BLK_ID_INGRESS;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, backup_blk_id));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA back up memory for "
                       "INGRESS stage is freed\n")));

    }
    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EGRESS) != NULL) {
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EGRESS));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA memory for "
                       "EGRESS stage is freed\n")));
        blk_id = BCMFP_HA_BLK_ID_EGRESS;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, backup_blk_id));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA back up memory for "
                       "EGRESS stage is freed\n")));
    }
    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_VLAN) != NULL) {
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_VLAN));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA memory for "
                       "VLAN stage is freed\n")));
        blk_id = BCMFP_HA_BLK_ID_VLAN;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, backup_blk_id));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA back up memory for "
                       "VLAN stage is freed\n")));
    }
    if (BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EM) != NULL) {
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EM));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA memory for "
                       "Exact Match stage is freed\n")));
        blk_id = BCMFP_HA_BLK_ID_EM;
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, backup_blk_id));
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "BCMFP:HA back up memory for "
                       "Exact Match stage is freed\n")));
    }

    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
        if (BCMFP_HA_PTR(unit, blk_id) == NULL) {
            continue;
        }
        shr_ha_mem_free(unit, BCMFP_HA_PTR(unit, blk_id));
        backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        shr_ha_mem_free(unit,
            BCMFP_HA_PTR(unit, backup_blk_id));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ha_init(int unit)
{
    uint8_t backup_blk_id = 0;
    uint32_t sw_state_size = 0;
    bcmfp_control_t *fc = NULL;
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_control_get(unit, &fc));

    if (fc->ifp != NULL) {
        stage_id = BCMFP_STAGE_ID_INGRESS;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_stage_oper_size_get(unit, stage_id,
                                         &sw_state_size));
        if (sw_state_size != 0) {
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_INGRESS) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     BCMFP_HA_BLK_ID_INGRESS,
                                     &sw_state_size);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP:HA memory for INGRESS stage "
             "is created with size %0d\n"), sw_state_size));
            SHR_IF_ERR_EXIT(
                bcmfp_stage_oper_info_init(unit,
                                       BCMFP_STAGE_ID_INGRESS,
                                       sw_state_size));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_ha_backup_blk_id_get(unit,
                     BCMFP_HA_BLK_ID_INGRESS, &backup_blk_id));
            BCMFP_HA_PTR(unit, backup_blk_id) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     backup_blk_id,
                                     &sw_state_size);
            sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
                       BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_INGRESS),
                       sw_state_size);
        }
    }

    if (fc->efp != NULL) {
        stage_id = BCMFP_STAGE_ID_EGRESS;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_stage_oper_size_get(unit, stage_id,
                                         &sw_state_size));
        if (sw_state_size != 0) {
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EGRESS) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     BCMFP_HA_BLK_ID_EGRESS,
                                     &sw_state_size);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP:HA memory for EGRESS stage "
                "is created with size %0d\n"), sw_state_size));
            SHR_IF_ERR_EXIT(
                bcmfp_stage_oper_info_init(unit,
                                           BCMFP_STAGE_ID_EGRESS,
                                           sw_state_size));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_ha_backup_blk_id_get(unit,
                         BCMFP_HA_BLK_ID_EGRESS, &backup_blk_id));
            BCMFP_HA_PTR(unit, backup_blk_id) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     backup_blk_id,
                                     &sw_state_size);
            sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
                       BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EGRESS),
                       sw_state_size);
        }
    }

    if (fc->vfp != NULL) {
        stage_id = BCMFP_STAGE_ID_LOOKUP;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_stage_oper_size_get(unit, stage_id,
                                         &sw_state_size));
        if (sw_state_size != 0) {
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_VLAN) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     BCMFP_HA_BLK_ID_VLAN,
                                     &sw_state_size);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP:HA memory for VLAN stage "
                "is created with size %0d\n"), sw_state_size));
            SHR_IF_ERR_EXIT(
                bcmfp_stage_oper_info_init(unit,
                                       BCMFP_STAGE_ID_LOOKUP,
                                       sw_state_size));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_ha_backup_blk_id_get(unit,
                      BCMFP_HA_BLK_ID_VLAN, &backup_blk_id));
            BCMFP_HA_PTR(unit, backup_blk_id) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     backup_blk_id,
                                     &sw_state_size);
            sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
                       BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_VLAN),
                       sw_state_size);
        }
    }

    if (fc->emfp != NULL) {
        stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_stage_oper_size_get(unit, stage_id,
                                         &sw_state_size));
        if (sw_state_size != 0) {
            BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EM) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     BCMFP_HA_BLK_ID_EM,
                                     &sw_state_size);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "BCMFP:HA memory for Exact Match stage "
                 "is created with size %0d\n"), sw_state_size));
            SHR_IF_ERR_EXIT(
                bcmfp_stage_oper_info_init(unit,
                                       BCMFP_STAGE_ID_EXACT_MATCH,
                                       sw_state_size));
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_ha_backup_blk_id_get(unit,
                    BCMFP_HA_BLK_ID_EM, &backup_blk_id));
            BCMFP_HA_PTR(unit, backup_blk_id) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_FP_COMP_ID,
                                     backup_blk_id,
                                     &sw_state_size);
            sal_memcpy(BCMFP_HA_PTR(unit, backup_blk_id),
                       BCMFP_HA_PTR(unit, BCMFP_HA_BLK_ID_EM),
                       sw_state_size);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_ha_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

