/*! \file ha_internal.h
 *
 * HA internal class definition
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

#ifndef HA_INTERNAL_H
#define HA_INTERNAL_H

#include <sal/sal_mutex.h>
#include <shr/shr_ha.h>

#define RESIZE_MEMORY_SIZE  0x1000000   /* 16MB */

/*!
 * \brief HA block signature.
 * Each HA block starts with a control block section
 * (see \ref ha_mem_blk_hdr_t). The first word of the control block uses this
 * value.
 */
#define HA_MEM_SIGNATURE    0xa75c9b3e

/*!
 * \brief Invalid block section.
 * Each HA control block (see \ref ha_mem_blk_hdr_t) specifies the section it
 * belong to.
 * HA blocks that used for internal ISSU purpose are linked together where the
 * pointer of such a block contains the section number and the offset of the
 * block within this section. To specify 'NULL' pointer, we use this
 * invalid block section as the section number.
 */
#define INVALID_BLOCK_SECTION    0xFFFFFFFF

/*!
 * \brief HA block attribute.
 * This value of the HA block attribute indicates that the block is used for
 * internal consumption and not by some component. The block ID of such block
 * type is invalid.
 */
#define BLOCK_INTERNAL_USE  0x1

/*!
 * \brief HA Block state.
 * Each HA block is either free or allocated. The HA block control section
 * (see \ref ha_mem_blk_hdr_t) contains the state field that indicate the state
 * of each particular block.
 */
typedef enum {
    HA_BLK_FREE = 0,
    HA_BLK_ALLOCATED = 1
} ha_mem_block_state_t;

/*!
 * \brief Memory block header type.
 *
 * Every memory block contains the following header structure.
 * The structure contains information required for the memory manager
 * as well as for version upgrade/downgrade
 */
typedef struct {
    /*! Every block starts with signature */
    uint32_t signature;
    /*! Unique block ID */
    uint16_t blk_id;
    /*! Block state (allocated/free) */
    uint8_t state;
    /*! Block attributes (see \ref BLOCK_INTERNAL_USE) */
    uint8_t attrib;
    /*! Block length including the header */
    uint32_t length;
    /*!
     * The offset to the begining of the previous block. In reality it is also
     * the length of the previous block.
     */
    uint32_t prev_offset;
    /*! The section containing the block */
    uint32_t section;
} ha_mem_blk_hdr_t;

/*!
 * \brief Section header.
 *
 * Every section of shared memory contains a control block that
 * helps to manage the section. The control block being allocated within
 * regular memory.
 */
typedef struct {
    /*! Pointer to the begining of the HA memory block for this section */
    ha_mem_blk_hdr_t *mem_start;
    /*! Out of bounds block */
    ha_mem_blk_hdr_t *mem_end;
    /*! Pointer to the free memory block */
    ha_mem_blk_hdr_t *free_mem;
    /*! The section length in bytes */
    uint32_t sect_len;
} ha_mem_section_t;

/*!
 * \brief Control block for blocks that manages the components structure info.
 */
typedef struct {
    /*! Every block starts with signature */
    uint32_t signature;
    /*!
     * Blocks might be chained together (linked list) in case a single block
     * size was insufficient to maintain all the structures information. See
     * \ref INVALID_BLOCK_SECTION. A pointer (HA pointer) conposes of block
     * section and offset within the section of the block.
     */
    uint32_t next_blk_offset;
    uint32_t next_blk_section;
    /*! Number of elements already filled in this block. */
    uint32_t num_of_elements;
    /*! The number of elements this block may contain. */
    uint32_t block_size;
} ha_mem_comp_struct_ctrl_blk_t;

/*!
 * \brief Control structure per unit.
 *
 * This is the main data structure that manages all the HA memory associated
 * with a single unit.
 */
typedef struct ha_private_s {
    /*! The size (in elements) of the mem_sect array */
    size_t msect_size;
    /*!
     * The index of the first free element in the array. Everything above
     * this index is also empty.
     */
    size_t msec_free_idx;
    /*! Vector contains all the HA memory sections */
    ha_mem_section_t *mem_sect;
    /*! Pointer to the begining of the free block */
    ha_mem_blk_hdr_t *free_mem;
    /*! Total allocated HA memory (used + unused) */
    uint32_t blk_len;
    /*! Indicates if created new file or opened existing file */
    bool new_file;
    /*! Protect shared HA memory resource */
    sal_mutex_t mutex;
    /*! Indicates if mem_init() method was called */
    bool initialized;
    /*! Shared mem resize callback */
    shr_ha_resize_f resize_cb;
    /*! Initial shared memory pointer */
    void *init_shr_mem;
    /*! Opaque pointer to be use during block resize */
    void *ctx;
    /*! Pointer to the component structure control blocks. */
    ha_mem_comp_struct_ctrl_blk_t *issu_blk;
} ha_private_t;


/*!
 * \brief Free previously allocated HA block
 *
 * This function being called to free a previously allocated memory block.
 *
 * \param [in] private Handle of the HA control block.
 * \param [in] mem Pointer to the component HA memory block.
 * block.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_PARAM when one of the input parameters was invalid.
 */
typedef int (*shr_ha_free_f)(void *private, void *mem);

/*!
 * \brief Reallocate existing memory
 *
 * This function allocates a block that has the requested length and copies
 * the content of the old block onto the new one. It then frees the original
 * block back into the free pool.
 *
 * \param [in] private Handle of the HA control block.
 * \param [in] mem Current memoy handle.
 * \param [in/out] length On input it is the desired new block length. On output
 *        it is the actual length of the allocated block.
 *
 * \retval pointer to the block if was reallocated
 * \retval NULL otherwise
 */
typedef void* (*shr_ha_realloc_f)(void *private,
                            void *mem,
                            uint32_t length);

/*!
 * \brief Allocate HA memory block.
 *
 * This function allocates a new HA block with block length that is at least
 * the requested length. The function returns a virtual pointer to the HA
 * block which enables the application to use the block as regular memory.
 *
 * \param [in] private Handle of the HA control block.
 * \param [in] mod_id Module (or component) ID.
 * \param [in] sub_id sub module (or component) ID.
 * \param [in/out] length On input it is the desired new block length. On output
 *        it is the actual length of the allocated block.
 *        The actual length should be larger or equal to the requested length.
 *
 * \retval pointer to the allocated block
 * \retval NULL if the block was not allocated
 */
typedef void *(*shr_ha_alloc_f)(void *private,
                          uint8_t mod_id,
                          uint8_t sub_id,
                          uint32_t *length);

/*!
 * \brief Initializes the HA memory.
 *
 * This function being called after the constructor \ref shr_ha_mm_create_f.
 * It initializes the HA block according to the \c new_file field of the private
 * structure.
 *
 * \param [in] private Handle of the HA control block.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_INTERNAL Indicated corrupted HA block (in warm boot).
 */
typedef int (*shr_ha_init_f)(void *private);

/*!
 * \brief Free the HA memory
 *
 * This function being called to free the memory. Note that it is the
 * application responsibility to remove the shared memory file. The free
 * process happens by calling the application supply callback function to
 * free a single section of the HA memory. Once all the sections were freed
 * the function returns.
 *
 * \param [in] private Handle of the HA control block.
 * \param [in] blk_free_cb Application callback function to free a single memory
 *        block.
 *
 * \retval None.
 */
typedef void (*shr_ha_mm_delete_f)(void *private,
                                  shr_ha_blk_free_f blk_free_cb);

/*!
 * \brief The constructor of the shared memory
 *
 * This function construct the shared memory by allocating some resources and
 * setting default values as well as the input values.
 *
 * \param [in] mmap Is Pointer to the HA memory block. The block is being
 *        allocated by the caller.
 * \param [in] size The size of the HA memory block
 * \param [in] n_file Indication if the shared memory was already used before
 *        or if it is a newly allocated memory.
 * \param [in] resize_f Application callback function that can be use to
 *        increase the HA block size.
 * \param [out] private Handle associated with this shared memory.
 *
 * \retval SHR_E_NONE on success
 * \retval SHR_E_MEMORY failed to allocate resources.
 */
typedef int (*shr_ha_mm_create_f)(void *mmap,
                                  int size,
                                  bool n_file,
                                  shr_ha_resize_f resize_f,
                                  void *context,
                                  void **private);

/*!
 * \brief Report the HA about the location of an HA data structure.
 *
 * The purpose of this function is to capture the exact location of all HA
 * data structures that can be upgraded during ISSU. Each data structure being
 * assigned a unique structure ID by the YML parser that is generating the
 * header file which contains the data structure decleration.
 *
 * \param [in] private Handle to the HA memory control block.
 * \param [in] mem The HA memory block that is containing the data structure.
 * \param [in] offset The offset from the begining of the block were the
 *        data structure is located.
 * \param [in] id Unique generated structure ID.
 *
 * \retval SHR_E_NONE on success
 * \retval SHR_E_MEMORY when the function failed to allocate resources.
 * \retval SHR_E_PARAM when one of the input parameters was invalid.
 * \retval SHR_E_INTERBAL when an internal contrl block is corrupted.
 */
typedef int (*shr_ha_struct_report_f)(void *private,
                                      void *mem,
                                      uint32_t offset,
                                      uint64_t id);

/*!
 * \brief Retrieve the location of an HA data structure.
 *
 * The purpose of this function is to retrieve the location of an HA data
 * structure that was previously reported to the HA manager via
 * \ref shr_ha_struct_report_f() function.
 * This function should only be called during warm boot where the component
 * needs to find out the location of its own structures within HA memory. This
 * functionality is particularly important after software upgrade where the
 * locations of the structures can shift within the block due to the upgrade
 * migration process.
 *
 * \param [in] private Handle to the HA memory control block.
 * \param [in] id Unique generated structure ID.
 * \param [out] mem The HA memory block that is containing the data structure.
 * \param [out] offset The offset from the begining of the block were the
 *        data structure is located.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_INTERNAL Internal data structure mismatch.
 * \retval SHR_E_NOT_FOUND The structure ID was not found.
 */
typedef int (*shr_ha_struct_info_f)(void *private,
                                    uint64_t id,
                                    void **mem,
                                    uint32_t *offset);

/*!
 * \brief HA memory implementation.
 *
 * The HA memory can be implemented using memory map allocated by the
 * application (typically the memory mapped to shared memory) as well as
 * regular stdlib memory. The type of implementation being determind during
 * initialization.
 * The implementation may contain the following functions. This structure
 * contains the memory map implementation.
 */
typedef struct shr_ha_drv_s {
    shr_ha_mm_create_f create;
    shr_ha_mm_delete_f delete;
    shr_ha_init_f init;
    shr_ha_alloc_f alloc;
    shr_ha_realloc_f realloc;
    shr_ha_free_f free;
    shr_ha_struct_report_f struct_report;
    shr_ha_struct_info_f struct_info;
} shr_ha_drv_t;

extern const shr_ha_drv_t shr_ha_drv;

/*!
 * \brief Allocate internal block with minimal length.
 *
 * Allocate an HA block that has specific length. The allocation is for
 * internal use.
 *
 * \param [in] this the HA control block.
 * \param [in] length Minimal length of the allocated block.
 *
 * \retval pointer to the block if was found
 * \retval NULL if the block was not found
 */
extern ha_mem_blk_hdr_t* shr_ha_alloc_internal_block(ha_private_t *this,
                                                     uint32_t length);

/*!
 * \brief Find a block with matched block ID.
 *
 * Search the entire HA memory for an occupied block with matching blk_id
 *
 * \param [in] blk_id is the block ID to search for
 *
 * \retval pointer to the block if was found
 * \retval NULL if the block was not found
 */
extern ha_mem_blk_hdr_t* shr_ha_block_find(ha_private_t *this, uint16_t blk_id);

/*=================================================================
   ISSU Definitions
=================================================================*/
/*!
 * \brief Implementation of struct report function.
 *
 * A function of the type shr_ha_struct_report_f (see \ref shr_ha_drv_t).
*/
extern int shr_ha_struct_report_ha(void *private,
                                   void *mem,
                                   uint32_t offset,
                                   uint64_t id);

/*!
 * \brief Implementation of struct info function.
 *
 * A function of the type shr_ha_struct_info_f (see \ref shr_ha_drv_t).
*/
extern int shr_ha_struct_info_ha(void *private,
                                  uint64_t id,
                                  void **mem,
                                  uint32_t *offset);

/*!
 * \brief Initializes the ISSU structure blocks.
 *
 * This functions initializes the block chain of the ISSU structure blocks. It
 * is only being cold for warm boot where the HA file was reallocated.
 * The reason this function is needed is due to the fact that in warm boot all
 * the sections collapses into a single section. Since the ISSU struct blocks
 * are a linked list of blocks where the poiner is the section number and the
 * offset from the begining of the section the pointers must be adjusted
 * to refer only to section 0 (the only section in warm boot).
 * This function go through all the HA blocks and find those that has internal
 * attributes. This is an ISSU structure block and the function chains it to
 * the linked list.
 *
 * \param [in] this is the HA block context.
 *
 * \retval None.
 */
extern void shr_ha_issu_struct_init(ha_private_t *this);

#endif /* HA_INTERNAL_H */
