/*! \file shr_ha.h
 *
 * SHR HA API header file
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

#ifndef SHR_HA_H
#define SHR_HA_H

#include <sal/sal_types.h>

/*!
 * \brief Shared file resize callback.
 *
 * This function signature used by the HA manager to request the application
 * to resize the shared memory. The reason for resizing the shared memory is
 * due to memory allocation that exceeds the amount of available shared memory.
 * The application must implement this function. Typically, the application
 * should take the following steps:
 * a. Round up the requested memory size \c addition to the nearest system
 *    page boundary. Set \c addition_mem_size to be the rounded up size.
 * b. Resize the shared file using the file handle \c fh to the size of
 *    \c curr_file_size + \c addition_mem_size. The application can use
 *    the posix function ftruncate().
 * c. Remap the extra shared memory. This can be done as follows:
 *   i.  Try to simply extend the current memory map using the \c old_mmap and
 *       the \c old_size parameters. Use the function mremap(). Doing so will
 *       be the most efficient for the HA manager. If the OS can not extend
 *       the current memory map continue to step (ii)
 *   ii. Create a new memory map of size \c addition_mem_size and map it to the
 *       file handle \c fh at an offset of \c curr_file_size.
 * d. Set the value of \c actual_addition to \c addition_mem_map. It is
 *    important that the HA manager would know the actual size of the shared
 *    file and all the memory mapped segments.
 * e. Return the shared memory map from (c)(i) or (c)(ii) above.
 *
 * \param [in] curr_file_size The shared file current size.
 * \param [in] old_mmap The memory map to extend (resize).
 * \param [in] old_size The current memory map size (of \c old_mmap.
 * \param [in] addition The desired size to extend the shared file.
 * \param [out] actual_addition The actual size of which the file was extended.
 * \param [in] ctx Opaque pointer provided in \ref shr_ha_init()
 *
 * \return A pointer to a shared memory if succeeded or NULL otherwise.
 */
typedef void *(*shr_ha_resize_f)(uint32_t curr_file_size,
                                 void *old_mmap,
                                 uint32_t old_size,
                                 uint32_t addition,
                                 uint32_t *actual_addition,
                                 void *ctx);

/*!
 * \brief Shared memory free callback.
 *
 * This function signature is is used by the HA manager to release memory map
 * segment. This function should be implemented by the application.
 * The implementation should simply call the posix function \c munmap().
 *
 * \param [in] mmap The pointer to the memory map segment to free.
 * \param [in] len The length of the memory map segment.
 *
 * \return none.
 */
typedef void (*shr_ha_blk_free_f)(void *mmap, uint32_t len);

/*!
 * \brief Initialize HA memory.
 *
 * This function being called for any newly discovered unit. The function must
 * be called by the application prior of calling \c bcmlt_device_attach().
 * The application must open the shared file associated with the unit and that
 * create memory map into the shared file. The memory map pointer \c mmap is
 * passed as input for this function. When opening the shared file the
 * application need to determine if the file already exist or not. The input
 * parameter new_file should indicate if the file is new or already existed.
 * The main reason to differentiate between existing file to new file is the
 * size of the file and the shared memory. If the file was already exist, the
 * application must memory map the entire space of the file. That is since
 * the HA manager stored valuable information inside the file. Note that file
 * created at a specific size might be extended during run-time using the
 * application provided function \c resize_f (see \ref shr_ha_resize_f()).
 *
 * \param [in] unit The unit number associated with this HA instance.
 * \param [in] mmap A pointer to the memory map that is mapped onto the
 * shared file.
 * \param [in] size The size of the shared memory segment.
 * \param [in] enabled If true enable using HA memory, otherwise implement via
 * \c SAL alloc and free functions.
 * \param [in] new_file Boolean value indicate if the shared file is new or
 * had prior context.
 * \param [in] resize_f Application provided callback function to increase the
 * size of the shared file if needed.
 * \param [in] ctx Opaque pointer to use when calling the \c resize_f function
 *
 * \return SHR_E_NONE success, otherwise failure in adding the field to the
 */
extern int shr_ha_init(int unit,
                       void *mmap,
                       int size,
                       bool enabled,
                       bool new_file,
                       shr_ha_resize_f resize_f,
                       void *ctx);

/*!
 * \brief Allocate memory block.
 *
 * This function being used to allocate a block of HA memory and associate it
 * with a particular unit. The component and sub-component IDs (\c mod_id,
 * \c sub_id respectively) are in essence non volatile pointers. When the
 * application starts warm, the component and sub-component IDs can be used
 * to retrieve the same memory content that was held prior to the boot.
 * To control the numbers, each software component will have pre-allocated
 * component ID (in some header file). The sub-component IDs will b managed
 * internally by the component.
 * In case of software upgrade, the HA will used the auto generated structure
 * signature (\c struct_sig), that describes the data structure of the HA
 * memory, to perform automatic upgrade to the data structure.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] comp_id Pre-allocated component ID.
 * \param [in] sub_id Internal ID managed by the component.
 * \param [out] length This parameter used as input and output. For input it
 * specifies the desired memory size. For output it is the actual memory block
 * that was allocated. The output value
 * has meaning during warm boot where the requested size might be different
 * from the actual size of the block that was allocated in previous run.
 *
 * \return If success the function returns pointer to the desired memory.
 * Otherwise it returns NULL.
 */
extern void *shr_ha_mem_alloc(int unit,
                              uint8_t comp_id,
                              uint8_t sub_id,
                              uint32_t *length);

/*!
 * \brief Reallocate existing memory block.
 *
 * This function extends already allocated memory block to a desired length.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] mem The current block to extend
 * \param [in] length The desired length of memory to allocate.
 *
 * \return If success the function returns pointer to the extended memory
 * block. Otherwise it returns NULL.
 */
extern void *shr_ha_mem_realloc(int unit, void *mem, uint32_t length);

/*!
 * \brief Free previously allocated memory block.
 *
 * This function returns the memory block into the free pool. Once the block
 * had been returned into the free pool it can be reused by the HA manager for
 * future requests of the same or smaller size. This can potentially cause
 * memory fragmentation. It is therefore recommended to avoid continuous
 * allocation and free of HA memory during run-time.
 * Since the application is allocating the HA memory, there is no good reason to
 * free the HA memory upon unit shutdown. If necessary, the application will
 * free this memory. Otherwise, the HA memory may be used for following warm
 * start.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] mem The current block to to free
 *
 * \return SHR_E_NONE success, otherwise failure in adding the field to the
 */
extern int shr_ha_mem_free(int unit, void *mem);

/*!
 * \brief Inform the HA about the location of a data structure in the HA block.
 *
 * Through this function a component can inform the HA manager about the
 * location of a known HA data structure within an HA memory block. This
 * information is being used during the ISSU process where the ISSU manager
 * determines if a particular data structure has changed and therefore needs
 * to be updated to the new structure while maintaining the previous values.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] mem The HA block where the data structure is being placed.
 * \param [in] offset The number of bytes from the beginning of the HA block
 * where the structure is being located.
 * \param [in] id An automatically generated unique ID for the data structure.
 *
 * \return SHR_E_NONE success, otherwise SHR_E_PARAM due to wrong parameters.
 */
extern int shr_ha_struct_report(int unit,
                                void *mem,
                                uint32_t offset,
                                uint64_t id);

/*!
 * \brief Retrieve the location of an HA data structure.
 *
 * The purpose of this function is to retrieve the location of an HA data
 * structure that was previously reported to the HA manager via
 * \ref shr_ha_struct_report() function.
 * This function should only be called during warm boot where the component
 * needs to find out the location of its own structures within HA memory. This
 * functionality is particularly important after software upgrade where the
 * locations of the structures can shift within the block due to the upgrade
 * migration process.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] id Unique generated structure ID.
 * \param [out] mem The HA memory block that is containing the data structure.
 * \param [out] offset The offset from the beginning of the block were the
 *        data structure is located.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_INTERNAL Internal data structure mismatch.
 * \retval SHR_E_NOT_FOUND The structure ID was not found.
 */
extern int shr_ha_struct_info_get(int unit,
                                  uint64_t id,
                                  void **mem,
                                  uint32_t *offset);

/*!
 * \brief Close previously initiated HA memory.
 *
 * This function is the inverse of \ref shr_ha_init() function. It is expected
 * to be called by the application after the call to \c bcmlt_device_detach().
 * The application should also provide a callback function that will be called
 * by the HA manager to free the memory map segments associated with this unit
 * (see \ref shr_ha_blk_free_f() for more details).
 * Note that the application is expected to close the shared file associated
 * with this unit. The file can be kept (for future warm boot) or can be
 * removed from its storage (using the function \c shm_unlink()).
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] blk_free_cb An application provided function that can free
 * the memory mapped segments.
 *
 * \return SHR_E_NONE success, otherwise error code
 */
extern int shr_ha_close(int unit, shr_ha_blk_free_f blk_free_cb);

/*!
 * \brief Macros to define and extract HA pointers
 *
 * HA pointers are 64 bit numbers that contains two parts.
 * 1. An eight bit block identifier - the sub component. This is the block from
 *    bit 56 to 63
 * 2. A two bit for component proprietary usage. These two bits are opaque to
 *    the HA component. The two bits will be located in bits 54, 55.
 * 3. The offset from the beginning of the block where the pointer points. These
 *    32 bits located from bit 0 to bit 31.
 *
 * The above scheme implies that pointers can not be exchanged between
 * components as each component manages its own sub component id value.
 */

/*!
 * \brief HA pointer type definition.
 */
typedef uint64_t shr_ha_ptr_t;

/*!
 * \brief HA pointer offset mask definition.
 */
#define SHR_HA_PTR_OFFSET_MASK  0xFFFFFFFF

/*!
 * \brief HA pointer sub module shift definition.
 * Indicates the location of the sub module in the HA pointer.
 */
#define SHR_HA_PTR_SUB_SHIFT    56

/*!
 * \brief HA pointer component private shift definition.
 * Indicates the location of the component private bits in the HA pointer.
 */
#define SHR_HA_PTR_PRIV_SHIFT   54

/*!
 * \brief HA pointer sub module size definition.
 */
#define SHR_HA_PTR_SUB_MASK     0xFF

/*!
 * \brief HA pointer component private bits size definition.
 */
#define SHR_HA_PTR_PRIV_MASK     0x3

/*!
 * \brief HA pointer component private bits mask definition.
 * Specifies the actual bits that are occupied by the component private bits.
 */
#define SHR_HA_PTR_PRIV_BITS    0x60000000000000

/*!
 * \brief HA pointer constructor.
 *
 * \param [in] _sub The sub module of the HA block that this pointer points to.
 * \param [in] _offset The offset (in bytes) from the beginning of the block
 * where the pointer will point to.
 *
 * \return HA pointer.
 */
#define SHR_HA_PTR_CONSTRUCT(_sub, _offset) \
    (((_sub) & SHR_HA_PTR_SUB_MASK) << SHR_HA_PTR_SUB_SHIFT | \
    ((_offset) & SHR_HA_PTR_OFFSET_MASK))

/*!
 * \brief HA pointer constructor with component private bits.
 *
 * \param [in] _sub The sub module of the HA block that this pointer points to.
 * \param [in] _offset The offset (in bytes) from the beginning of the block
 * where the pointer will point to.
 * \param [in] _priv is private value of up to two bits. This value is opaque
 * to the HA module but can be associated with the pointer and retrieved by
 * the component.
 *
 * \return HA pointer.
 */
#define SHR_HA_PTR_EXT_CONSTRUCT(_sub, _offset, _priv) \
    (((_sub) & SHR_HA_PTR_SUB_MASK) << SHR_HA_PTR_SUB_SHIFT |   \
    ((_priv) & SHR_HA_PTR_PRIV_MASK) << SHR_HA_PTR_PRIV_SHIFT | \
    ((_offset) & SHR_HA_PTR_OFFSET_MASK))

/*!
 * \brief Extract HA block offset from encoded pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return The offset (in bytes) of the pointer from the beginning of the HA
 * block.
 */
#define SHR_HA_PTR_OFFSET(_ha_ptr) \
    (_ha_ptr) & SHR_HA_PTR_OFFSET_MASK

/*!
 * \brief Extract HA block sub module from encoded pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return The sub module value.
 */
#define SHR_HA_PTR_SUB(_ha_ptr) \
    (((_ha_ptr) >> SHR_HA_PTR_SUB_SHIFT) & SHR_HA_PTR_SUB_MASK)

/*!
 * \brief Extract HA block private value from encoded pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return The component private value associated with the pointer.
 */
#define SHR_HA_PTR_PRIV(_ha_ptr) \
    (((_ha_ptr) >> SHR_HA_PTR_PRIV_SHIFT) & SHR_HA_PTR_PRIV_MASK)

/*!
 * \brief Set component private value into HA pointer.
 *
 * \param [in] _ha_ptr The HA pointer.
 * \param [in] _priv The component private value.
 *
 * \return None.
 */
#define SHR_HA_PTR_PRIV_SET(_ha_ptr, _priv) \
    ((_ha_ptr) & ~SHR_HA_PTR_PRIV_BITS) |   \
    ((_priv) & SHR_HA_PTR_PRIV_MASK) << SHR_HA_PTR_PRIV_SHIFT

/*!
 * \brief Set HA pointer to HA 'NULL' value.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return None.
 */
#define SHR_HA_PTR_NULL_SET(_ha_ptr) \
    (_ha_ptr) = 0

/*!
 * \brief Check if HA pointer is HA 'NULL'.
 *
 * \param [in] _ha_ptr The HA pointer.
 *
 * \return True if the pointer is HA 'NULL' and false otherwise.
 */
#define SHR_HA_PTR_IS_NULL(_ha_ptr)  \
    ((_ha_ptr) == 0)

#endif /* SHR_HA_H */
