/*! \file bcmfp_ha_internal.h
 *
 * Data structures stored in HA area for BCMFP component.
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

#ifndef BCMFP_HA_INTERNAL_H
#define BCMFP_HA_INTERNAL_H

#include <bcmfp/bcmfp_trans_internal.h>

/*! First version number for BCMFP HA blocks. */
#define BCMFP_HA_VERSION_1 0x1

/*! Current version number for BCMFP HA blocks. */
#define BCMFP_HA_STRUCT_VERSION BCMFP_HA_VERSION_1

/*! Signature for BCMFP HA blocks. */
#define BCMFP_HA_STRUCT_SIGN 0x02030D061001

/*! Minimum valid block id for BCMFP HA blocks. */
#define BCMFP_HA_BLK_ID_MIN 0x1

/*! Maximum valid block id for BCMFP HA blocks. */
#define BCMFP_HA_BLK_ID_MAX 0xFE

/*! Minimum valid block id for BCMFP fixed HA blocks.
 *  Fixed HA blocks are created during system initialization.
 */
#define BCMFP_HA_BLK_ID_FIXED_MIN  0x1

/*! Maximum valid block id for BCMFP fixed HA blocks.
 *  Fixed HA blocks are created during system initialization.
 */
#define BCMFP_HA_BLK_ID_FIXED_MAX  0xF

/*! Minimum valid block id for BCMFP dynamic HA blocks.
 *  Dynamic HA blocks are created during run time.
 */
#define BCMFP_HA_BLK_ID_DYNAMIC_MIN 0xBF

/*! Maximum valid block id for BCMFP dynamic HA blocks.
 *  Dynamic HA blocks are created during run time.
 */
#define BCMFP_HA_BLK_ID_DYNAMIC_MAX 0xDE

/*! Minimum valid block id for BCMFP fixed backup HA blocks.
 *  Fixed backup HA blocks are created during system initialization.
 */
#define BCMFP_HA_BACKUP_BLK_ID_FIXED_MIN  0x10

/*! Maximum valid block id for BCMFP fixed backup HA blocks.
 *  Fixed backup HA blocks are created during system initialization.
 */
#define BCMFP_HA_BACKUP_BLK_ID_FIXED_MAX  0x1E

/*! Minimum valid block id for BCMFP dynamic backup HA blocks.
 *  Dynamic backup HA blocks are created during run time.
 */
#define BCMFP_HA_BACKUP_BLK_ID_DYNAMIC_MIN 0xDF

/*! Maximum valid block id for BCMFP dynamic backup HA blocks.
 *  Dynamic backup HA blocks are created during run time.
 */
#define BCMFP_HA_BACKUP_BLK_ID_DYNAMIC_MAX 0xFE

/*! Invalid HA block ID. Used in HA block id validations. */
#define BCMFP_HA_BLK_ID_INVALID    0x0

/*! Since block offset is uint32_t type the max offset is ((2^32) -1).
 *  This macros is used to identify invalid HA block offset.
 */
#define BCMFP_HA_BLK_OFFSET_INVALID 0xFFFFFFFF

/*! Reserved BCMFP HA block Ids. */
typedef enum bcmfp_ha_blk_id_s {
    /*! HA block id for ingress stage(IFP). */
    BCMFP_HA_BLK_ID_INGRESS = 1,
    /*! HA block id for egress stage(EFP). */
    BCMFP_HA_BLK_ID_EGRESS,
    /*! HA block id for vlan stage(VFP). */
    BCMFP_HA_BLK_ID_VLAN,
    /*! HA block id for exact match stage(EM). */
    BCMFP_HA_BLK_ID_EM,
    /*! Always Last. Not a usable value. */
    BCMFP_HA_BLK_ID_COUNT,
} bcmfp_ha_blk_id_t;

/* All BCMFP HA blocks will have this header. */
typedef struct bcmfp_ha_blk_hdr_s {
    /* HA block size */
    uint32_t blk_size;
    /* Back up block Id */
    uint8_t backup_blk_id;
    /* Free HA element in this HA block. This is used to
     * maintain list of free HA elements in this HA block.
     */
    uint32_t free_blk_offset;
    /* Number of free HA elements. */
    uint32_t free_ha_element_count;
    /* Transaction State */
    bcmfp_trans_state_t trans_state;
} bcmfp_ha_blk_hdr_t;

/*! HA blocks created in run time have a fixed number of
 *  HA elements in it in addition to the HA header at the
 *  begining. Each HA element has TV(Type, Value) fields.
 *  Following are the HA elements types in use across
 *  different HA versions.
 */
typedef enum bcmfp_ha_element_type_s {
    /*! HA element has no type. By default all
     *  HA elements type is set to this value.
     */
    BCMFP_HA_ELEMENT_TYPE_NONE = 0,
    /*! HA element holds Qualifer ID of type bcmfp_qual_t. */
    BCMFP_HA_ELEMENT_TYPE_SHORT_QID_AND_NUM_OFFSETS,
    /*! HA element holds (offset, width, part, offset_id)
     *  of a qualifier.
     */
    BCMFP_HA_ELEMENT_TYPE_QUAL_OFFSET_WIDTH_PART_ID,
    /*! HA element holds HA block ID. */
    BCMFP_HA_ELEMENT_TYPE_HA_BLK_ID,
    /*! HA element holds offset in a HA block. This type of
     *  HA elemets are follwed by HA element of type
     *  BCMFP_HA_ELEMENT_TYPE_HA_BLK_ID.
     */
    BCMFP_HA_ELEMENT_TYPE_HA_BLK_OFFSET,
    /*! Always Last. Not a usable value. */
    BCMFP_OPER_INFO_TYPE_COUNT,
} bcmfp_ha_element_type_t;

/*! HA blocks created in run time have a fixed number of
 *  HA elements in it, in addition to the HA header at the
 *  begining.
 */
typedef struct bcmfp_ha_element_s {
    /*! Type of the value stored in the value. */
    bcmfp_ha_element_type_t type;
    /*! Actual value saved in this element. */
    uint32_t value;
    /*! Next HA block ID. If this is 0, that means
     *  the end of chain.
     */
    uint16_t next_blk_id;
    /*! Offset in the next HA block ID */
    uint32_t next_blk_offset;
} bcmfp_ha_element_t;

/*! BCMFP HA block information. */
typedef struct bcmfp_ha_blk_info_s {
    /* Unique Id assigned to HA block created by BCMFP.
     * This will be same as sub component id used while
     * creating the HA block using bcmfp_ha_mem_alloc API.
     */
    uint16_t blk_id;
    /* block info will be derived for a certain s/w state
     * during run time. This block offset is the offset in
     * the HA block of "blk_id". This is multiples of size
     * of bcmfp_ha_element_t.
     */
    uint32_t blk_offset;
} bcmfp_ha_blk_info_t;

/*! Macro to check if the HA block id is fixed(created during
 *  FP component initialization time) or not.
 */
#define BCMFP_HA_BLK_ID_IS_FIXED(blk_id)       \
     ((blk_id >= BCMFP_HA_BLK_ID_FIXED_MIN) && \
      (blk_id <= BCMFP_HA_BLK_ID_FIXED_MAX))

/*! Macro to check if the HA block id is dynamic(created during
 *  FP component run time) or not.
 */
#define BCMFP_HA_BLK_ID_IS_DYNAMIC(blk_id)       \
     ((blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN) && \
      (blk_id <= BCMFP_HA_BLK_ID_DYNAMIC_MAX))

/*! Macro to get the HA block id reserved for a given stage_id. */
#define BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id)         \
    do {                                                      \
        if (stage_id == BCMFP_STAGE_ID_INGRESS) {             \
           blk_id = BCMFP_HA_BLK_ID_INGRESS;                  \
        } else if (stage_id == BCMFP_STAGE_ID_EGRESS) {       \
           blk_id = BCMFP_HA_BLK_ID_EGRESS;                   \
        } else if (stage_id == BCMFP_STAGE_ID_LOOKUP) {       \
           blk_id = BCMFP_HA_BLK_ID_VLAN;                     \
        } else if (stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {  \
           blk_id = BCMFP_HA_BLK_ID_EM;                       \
        } else {                                              \
           SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);               \
        }                                                     \
    } while(0)

/*! Macro to validate the HA block Id. This will check whether
 *  HA block id is with in the min, max limits of valid block Ids.
 */
#define BCMFP_HA_BLK_ID_VALIDATE(unit, blk_id)    \
    do {                                          \
        SHR_IF_ERR_EXIT(                          \
            (blk_id == BCMFP_HA_BLK_ID_INVALID) ? \
                   SHR_E_INTERNAL : SHR_E_NONE);  \
        SHR_IF_ERR_EXIT(                          \
            (blk_id > BCMFP_HA_BLK_ID_MAX) ?      \
                  SHR_E_INTERNAL : SHR_E_NONE);   \
        SHR_IF_ERR_EXIT(                          \
            (blk_id < BCMFP_HA_BLK_ID_MIN) ?      \
                  SHR_E_INTERNAL : SHR_E_NONE);   \
    } while(0)

/*! Macros to validate the HA block offset. It does following checks
 *- 1. blk_offset is not same as BCMFP_HA_BLK_OFFSET_INVALID.
 *- 2. blk_offset is not more than block size.
 *- 3. blk_offset is not multiple of size of bcmfp_ha_element_t.
 *- This macro doesnot validate blk_id. So before using this macro
 *  BCMFP_HA_BLK_ID_VALIDATE should be preceeded for validating the
 *  block id.
 */
#define BCMFP_HA_BLK_OFFSET_VALIDATE(unit, blk_id, blk_offset) \
    do {                                                       \
        void *_ha_ptr = NULL;                                  \
        uint32_t _size = 0;                                    \
        bcmfp_ha_blk_hdr_t *_blk_hdr = NULL;                   \
        SHR_IF_ERR_EXIT(                                       \
            (blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) ?      \
                                SHR_E_INTERNAL: SHR_E_NONE);   \
        if (((blk_offset - sizeof(bcmfp_ha_blk_hdr_t)) %       \
             sizeof(bcmfp_ha_element_t)) != 0) {               \
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);               \
        }                                                      \
        _ha_ptr = BCMFP_HA_PTR(unit, blk_id);                  \
        if (_ha_ptr == NULL) {                                 \
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);              \
        }                                                      \
        _blk_hdr = (bcmfp_ha_blk_hdr_t *)_ha_ptr;              \
        SHR_IF_ERR_EXIT(                                       \
            (blk_offset > _blk_hdr->blk_size) ?                \
                     SHR_E_INTERNAL : SHR_E_NONE);             \
        _size = blk_offset + sizeof(bcmfp_ha_element_t);       \
        SHR_IF_ERR_EXIT(                                       \
            (_size > _blk_hdr->blk_size) ?                     \
               SHR_E_INTERNAL : SHR_E_NONE);                   \
    } while(0)

/*!
 * \brief Allocate and Initialize the HA memory(HA blocks)
 *  required for BCMFP during system initialization time.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int bcmfp_ha_init(int unit);

/*!
 * \brief De allocate the HA memory(HA blocks) allocated for
 *  BCMFP during system initialization time and run time.
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int bcmfp_ha_cleanup(int unit);

/*!
 * \brief Get the pointer to the HA memory for the given
 *  HA block ID.
 *
 * \param [in] unit Logical device id
 * \param [in] sub_id HA block id.
 * \param [out] ha_mem Pointer to HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_mem_get(int unit,
                 uint8_t sub_id,
                 void **ha_mem);
/*!
 * \brief Set the pointer to the HA memory for the given
 *  HA block ID.
 *
 * \param [in] unit Logical device id
 * \param [in] sub_id HA block id.
 * \param [in] ha_mem Pointer to HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_mem_set(int unit,
                 uint8_t sub_id,
                 void *ha_mem);
/*!
 * \brief Get the pointer to the HA element in the given
 *  HA block id at the specified offset. This function can
 *  be used only if HA block is created run time.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id.
 * \param [in] blk_offset HA block offset.
 * \param [in] ha_element Pointer to HA element.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_element_get(int unit,
               uint8_t blk_id,
               uint32_t blk_offset,
               bcmfp_ha_element_t **ha_element);

/*!
 * \brief Acquire the required number of ha elements from the
 *  free pool. Each HA block(run time created) maintains linked
 *  list of free HA elements in it. While acquiring the free HA
 *  elements, following sequence of steps will be executed.
 *  - Go through all existing HA blocks(run time created) until
 *    required number of HA elements are acquired.
 *  - If free HA elements in all existing HA blocks(run time created),
 *    Then create a new HA block and repeat above step.
 *  - While creating a new HA block in the above step, if no block ids
 *    are left free(Each HA block will be assigned one unique blocj id),
 *    then fail.
 *
 * \param [in] unit Logical device id
 * \param [in] req_ha_elements Required number of HA elements.
 * \param [out] ha_elements Pointer to linked list of acquired HA elements.
 * \param [out] first_ha_element_blk_id HA block id of first HA element
 *              in the linked list.
 * \param [out] first_ha_element_blk_offset HA block offset of first
 *              HA element in the linked list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_elements_acquire(int unit,
          uint16_t req_ha_elements,
          bcmfp_ha_element_t **ha_elements,
          uint8_t *first_ha_element_blk_id,
          uint32_t *first_ha_element_blk_offset);

/*!
 * \brief Release the linked list of HA elements(belongs to different
 *  block ids) back to free HA elements of its parent HA block id.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id of first HA element in linked list.
 * \param [in] blk_offset HA block offset first HA element in linked list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_elements_release(int unit,
                          uint8_t blk_id,
                          uint32_t blk_offset);
/*!
 * \brief Get the header of an HA block.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id.
 * \param [out] blk_hdr HA block header.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_blk_hdr_get(int unit,
             uint8_t sub_id,
             bcmfp_ha_blk_hdr_t **blk_hdr);

/*!
 * \brief Get the backup HA block ID for the given Ha block id.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id.
 * \param [out] _backup_blk_id Backup HA block id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ha_backup_blk_id_get(int unit,
                           uint8_t blk_id,
                           uint8_t *backup_blk_id);

#endif /* BCMFP_HA_INTERNAL_H */
