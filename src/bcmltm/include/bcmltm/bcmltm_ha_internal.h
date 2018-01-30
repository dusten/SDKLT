/*! \file bcmltm_ha_internal.h
 *
 * Logical Table Manager High Availability Internal Definitions.
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

#ifndef BCMLTM_HA_INTERNAL_H
#define BCMLTM_HA_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>


/*!
 * \brief HA block types.
 *
 * The High Availability persistent memory block categories.
 */
typedef enum bcmltm_ha_block_type_e {
    /*! Invalid block number to simplify bad HA pointer detection. */
    BCMLTM_HA_BLOCK_TYPE_INVALID = 0,

    /*! Same-size bcmltm_lt_state_t structures for all LTs. */
    BCMLTM_HA_BLOCK_TYPE_STATE = 1,

    /*! Various-sized bcmltm_lt_state_data_t structures for all LTs. */
    BCMLTM_HA_BLOCK_TYPE_STATE_DATA = 2,

    /*! LTM Transaction Status for current modeled transaction. */
    BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS = 3,

    /*! Rollback copy of base state for LTs in the current transaction. */
    BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE = 4,

    /*! Rollback copy of state data for LTs in the current transaction.  */
    BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE_DATA = 5,

    /*! Total HA block type count. */
    BCMLTM_HA_BLOCK_TYPE_COUNT = 6
} bcmltm_ha_block_type_t;

/*!
 * HA offset + subblock encoded pointer.
 *
 * Encoding is:
 * ha_ptr[31:24] = subblock type
 * ha_ptr[23:0] = block offset in bytes
 */
typedef uint32_t bcmltm_ha_ptr_t;

/*! HA subblock encoding shift. */
#define BCMLTM_HA_PTR_BLOCK_TYPE_SHIFT    24
/*! HA subblock encoding mask. */
#define BCMLTM_HA_PTR_BLOCK_TYPE_MASK     0xff
/*! HA block offset encoding mask. */
#define BCMLTM_HA_PTR_OFFSET_MASK         0xffffff

/*! Extract HA subblock from encoded pointer. */
#define BCMLTM_HA_PTR_TO_BLOCK_TYPE(_ha_ptr) \
    (((_ha_ptr) >> BCMLTM_HA_PTR_BLOCK_TYPE_SHIFT) & \
     BCMLTM_HA_PTR_BLOCK_TYPE_MASK)

/*! Extract HA block offset from encoded pointer. */
#define BCMLTM_HA_PTR_OFFSET(_ha_ptr) \
    ((_ha_ptr) & BCMLTM_HA_PTR_OFFSET_MASK)

/*! Encode HA block offset and subblock into HA pointer. */
#define BCMLTM_HA_PTR(_bt, _offset)                              \
    ((((uint32_t)((_bt) & BCMLTM_HA_PTR_BLOCK_TYPE_MASK)) <<     \
      BCMLTM_HA_PTR_BLOCK_TYPE_SHIFT) |                          \
     ((_offset) & BCMLTM_HA_PTR_OFFSET_MASK))

/*! An invalid LTM HA pointer value. */
#define BCMLTM_HA_PTR_INVALID 0

/*! Test for invalid LTM HA pointer. */
#define BCMLTM_HA_PTR_IS_INVALID(_ha_ptr) \
    ((_ha_ptr) == BCMLTM_HA_PTR_INVALID)

/*! Set LTM HA pointer to the invalid value. */
#define BCMLTM_HA_PTR_INVALID_SET(_ha_ptr) \
    (_ha_ptr) = BCMLTM_HA_PTR_INVALID

/*! LTM HA block signature base. */
#define BCMLTM_BLOCK_SIGNATURE_BASE   0xd1d54200

/*! LTM HA block signature for a block type. */
#define BCMLTM_BLOCK_SIGNATURE(_bt)   \
    (((uint32_t)((_bt) & BCMLTM_HA_PTR_BLOCK_TYPE_MASK)) | \
     BCMLTM_BLOCK_SIGNATURE_BASE)

/*! LTM HA block signature for a block type. */
#define BCMLTM_BLOCK_SIGNATURE_TYPE(_sig)   \
    ((bcmltm_ha_block_type_t)((_sig) & BCMLTM_HA_PTR_BLOCK_TYPE_MASK))


/*! LTM HA block header. */

typedef struct bcmltm_ha_block_header_s {
    /*!
     * HA recovery block validation marker
     */
    uint32_t signature;

    /*!
     * Total data length of the block
     */
    uint32_t block_size;
} bcmltm_ha_block_header_t;

/*!
 * \brief HA LTM state size values.
 */
typedef struct bcmltm_ha_state_sizes_s {
    /*!
     * Total size of all LTs bcmltm_lt_state_t structures.
     * This should be the size of the STATE HA block.
     */
    uint32_t total_state_size;

    /*!
     * Total size of all LTs state data.
     * This should be the size of the STATE_DATA HA block.
     */
    uint32_t total_state_data_size;

    /*!
     * Maximum size of a single LT bcmltm_lt_state_t structure.
     * This is included in calculating the size of the
     * ROLLBACK_STATE HA block.
     */
    uint32_t maximum_state_size;

    /*!
     * Maximum size of a single LT's combined state data.
     * This is included in calculating the size of the
     * ROLLBACK_STATE_DATA HA block.
     */
    uint32_t maximum_state_data_size;
} bcmltm_ha_state_sizes_t;

/*!
 * \brief LTM HA block information.
 */
typedef struct bcmltm_ha_block_info_s {
    /*!
     * LTM bcmltm_ha_block_type_t identifying the purpose of this block.
     */
    bcmltm_ha_block_type_t block_type;

    /*!
     * Total size of this HA block.
     */
    uint32_t block_size;

    /*!
     * Size of memory from this block already committed.
     */
    uint32_t allocated_size;

    /*!
     * Pointer to the HA memory space of this block.
     */
    void *ha_block_ptr;
} bcmltm_ha_block_info_t;


/*!
 * \brief Retrieve the maximum number of LTs per transaction.
 *
 * The number of Logical Table state records maintained for rollback
 * during an Atomic Transaction may be configured during initialization.
 * This function returns that number to the LTM elements which
 * depend upon this value.
 *
 * \param [in] unit Logical device id.
 *
 * \retval Number of LTs permitted per transaction.
 */
extern uint8_t
bcmltm_ha_transaction_max(int unit);

/*!
 * \brief Initialize LTM High Availability blocks.
 *
 * When the LTM is intialized, the HA block must be created with the
 * necessary sizes to handle the information to be stored in each block.
 * First, sufficient examination of the LTs configured in this run
 * must be completed to determine the size of each block.
 * Next, the HA regions are either initialized or recovered.
 *
 * \param [in] unit Logical device id.
 * \param [in] warm indicates if this is cold or warm boot.
 * \param [in] ha_state_sizes Pointer to HA state sizes record.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ha_init(int unit,
               bool warm,
               bcmltm_ha_state_sizes_t *ha_state_sizes);

/*!
 * \brief Cleanup LTM High Availability blocks.
 *
 * Erase and release the LTM High Availability blocks.
 *
 * \param [in] unit Logical device id
 *
 * \retval None
 */
extern void
bcmltm_ha_cleanup(int unit);

/*!
 * \brief Allocate memory from High Availability blocks.
 *
 * The LTM's High Availability blocks are used for storing LT state,
 * transaction status, and LT rollback state.  During metadata
 * initialization, the core LT state structures are allocated from
 * some of these blocks.
 *
 * This function verifies that sufficient memory to provide the
 * requested byte size remains in the HA block, reserves that amount
 * after word-alignment, and updates the record of space allocated.
 *
 * \param [in] unit Logical device id.
 * \param [in] block_type The HA block type from which to allocate memory.
 * \param [in] size Size in bytes of desired memory segment.
 *
 * \retval Encoded HA ptr to HA memory location of specified size.
 * \retval BCMLTM_HA_PTR_INVALID on failure due to insufficient memory.
 */
extern bcmltm_ha_ptr_t
bcmltm_ha_alloc(int unit,
                bcmltm_ha_block_type_t block_type,
                uint32_t size);

/*!
 * \brief Clear High Availability records.
 *
 * This function erases all of the information within the Logical
 * Table Manager High Availability blocks.
 *
 * \param [in] unit Logical device id.
 *
 * \retval None
 */
extern void
bcmltm_ha_reset(int unit);

/*!
 * \brief Convert HA encoded pointer to physical memory pointer.
 *
 * This function converts the HA encoded (subblock, offset) format
 * of a LTM HA pointer into the physical memory required for further
 * processing.  If the HA pointer is invalid, it returns NULL.
 *
 * \param [in] unit Logical device id.
 * \param [in] ha_ptr LTM HA encoded pointer.
 *
 * \retval Physical memory pointer if HA pointer is valid.
 *         NULL if HA pointer is invalid.
 */
extern void *
bcmltm_ha_ptr_to_mem_ptr(int unit,
                         bcmltm_ha_ptr_t ha_ptr);

/*!
 * \brief Increment HA encoded pointer by given amount
 *
 * This function adds the provided byte increment value,
 * with word-alignment, to the byte offset element of the
 * HA encoded (subblock, offset) format pointer.
 * If insufficient space remains for this block, then the returned
 * HA pointer value is BCMLTM_HA_PTR_INVALID.
 *
 * \param [in] unit Logical device id.
 * \param [in] ha_ptr LTM HA encoded pointer.
 * \param [in] increment Byte count to increase LTM HA encoded pointer.
 *
 * \retval New HA pointer.
 */
extern bcmltm_ha_ptr_t
bcmltm_ha_ptr_increment(int unit,
                        bcmltm_ha_ptr_t ha_ptr,
                        uint32_t increment);

#endif /* BCMLTM_HA_INTERNAL_H */
