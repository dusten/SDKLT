/*! \file ha_shmem.c
 *
 * HA shared memory manager.
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

#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_mutex.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bsl/bsl.h>
#include "ha_internal.h"

#define BSL_LOG_MODULE BSL_LS_SHR_HA

#define MSECT_VECTOR_CHUNK  10

/* Find free block with minimal length */
static ha_mem_blk_hdr_t* find_free_block(ha_private_t *this,
                                         uint32_t min_length);

/* Find the previous block to a block */
static ha_mem_blk_hdr_t *find_prev_block(ha_private_t *this,
                                         ha_mem_blk_hdr_t *start);

/* Resizing the section */
static ha_mem_blk_hdr_t *section_resize(ha_private_t *this, int size);

/* Sanity check of the entire HA memory */
static int sanity_check(ha_private_t *this);

static int ha_free(void *private, void *mem);

/*
 * This function is equivalent in functionality to the C++ vector::push_back.
 */
static void push_back(ha_private_t *this, ha_mem_section_t *sec) {
    if (this->msec_free_idx + 1 >= this->msect_size) {
        ha_mem_section_t *tmp;
        size_t alloc_size = sizeof(ha_mem_section_t) *
            (MSECT_VECTOR_CHUNK + this->msect_size);
        /* need to realloc */
        tmp = sal_alloc(alloc_size, "shrHaMemSect");
        if (!tmp) {
            return;
        }
        sal_memset(tmp, 0, alloc_size);
        sal_memcpy(tmp,
                   this->mem_sect,
                   sizeof(ha_mem_section_t) * this->msect_size);
        this->msect_size += MSECT_VECTOR_CHUNK;
        sal_free(this->mem_sect);
        this->mem_sect = tmp;
    }
    this->mem_sect[this->msec_free_idx++] = *sec;
}

static int ha_mm_create(void *mmap,
                        int size,
                        bool n_file,
                        shr_ha_resize_f resize_f,
                        void *context,
                        void **private)
{
    ha_private_t *this;
    size_t blk_size = sizeof(ha_mem_section_t) * MSECT_VECTOR_CHUNK;

    this = sal_alloc(sizeof(ha_private_t), "shrHaPriv");
    if (!this) {
        return SHR_E_MEMORY;
    }
    sal_memset(this, 0, sizeof(ha_private_t));
    this->mem_sect = sal_alloc(blk_size, "shrHaMemSect");
    if (!this->mem_sect) {
        sal_free(this);
        return SHR_E_MEMORY;
    }
    sal_memset(this->mem_sect,
               0,
               sizeof(ha_mem_section_t) * MSECT_VECTOR_CHUNK);
    this->msect_size = MSECT_VECTOR_CHUNK;
    this->msec_free_idx = 0;
    this->new_file = n_file;
    this->mutex = sal_mutex_create((char *)"ha_mutex");
    assert (this->mutex != NULL);
    this->initialized = false;
    this->blk_len = size;
    this->free_mem = NULL;
    this->resize_cb = resize_f;
    this->init_shr_mem = mmap;
    this->ctx = context;
    *private = (void *)this;
    return SHR_E_NONE;
}

/*!
 * \brief ha_mem_mgr class destructor.
 *
 * Free all resources associated with the class.
 *
 * \retval none
 */
static void ha_mm_delete(void *private, shr_ha_blk_free_f blk_free_cb)
{
    ha_private_t *this = (ha_private_t *)private;
    size_t j;
    ha_mem_section_t *it;
    uint8_t *mem_start;

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    if (blk_free_cb) {
        for (j = 0; j < this->msec_free_idx; j++) {
            it = &this->mem_sect[j];
            if (it->mem_start) {
                mem_start = (uint8_t *)it->mem_start;
                if (j == 0) { /* The start of the first section is skewed */
                    mem_start -= sizeof(ha_mem_comp_struct_ctrl_blk_t);
                }
                blk_free_cb((void *)mem_start, it->sect_len);
            }
        }
    }
    sal_free(this->mem_sect);
    sal_mutex_destroy(this->mutex);
    sal_free(this);
}

/*!
 * \brief Initializes the shared memory.
 *
 * This function initialized the shared memory. The memory is being iniitializes
 * in the following way:
 * 1. The header of the component structure information is being placed at the
 * top of the memory block. All the regular blocks will be allocated
 * after this section.
 * 2. If it is a new file (cold start) the entire memory is marked as a free
 * block with the length of the entire memory excluding the control block from
 * (1) above.
 * 3. If it is an existing file (warm boot) the function checks the validity of
 * all the existing blocks. It also adjusts their sections and offset since
 * in warm boot there is only single section.
 *
 * \param [in] private Is the handle to the HA memory control structure.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_INIT The memory was already initialized
 * \retval SHR_E_INTERNAL The shared memory structure is not coherent. This
 * may occur if a memory overwrite took place in the previous run. Typically for
 * this scenario, memory overwrite may occur due to a component writing outside
 * its allocated block size and spilling over to the next block. Since each HA
 * memory block starts with 32 bit signature this can be detected. When detected
 * the HA module will write an error message indicating the last good block
 * owner - typically the suspected module.
 */
static int mem_init(void *private)
{
    ha_mem_blk_hdr_t *first_hdr;
    ha_mem_section_t first_sect;
    int rv = 0;
    ha_private_t *this = (ha_private_t *)private;
    uint8_t *block_alloc_sect;
    ha_mem_comp_struct_ctrl_blk_t *ctrl_blk;

    if (this->initialized) {
        return SHR_E_INIT;
    }
    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);

    /*
     * We place a control block at the begining of the HA memory. Therefore
     * we skew the first section mem_start by the size of the control block.
     * For the purpose of block allocation the mem_start is fine. The only
     * time we need the actual mem_start is when freeing or resizing the
     * HA block since it is done by the application.
     */
    block_alloc_sect = (uint8_t *)this->init_shr_mem +
                        sizeof(ha_mem_comp_struct_ctrl_blk_t);
    ctrl_blk = (ha_mem_comp_struct_ctrl_blk_t *)this->init_shr_mem;
    first_sect.mem_start = (ha_mem_blk_hdr_t *)block_alloc_sect;
    first_sect.mem_end =
        (ha_mem_blk_hdr_t *)((uint8_t *)this->init_shr_mem + this->blk_len);
    first_sect.free_mem = first_sect.mem_start;
    first_sect.sect_len = this->blk_len;
    push_back(this, &first_sect);

    /* The free memory starts at the first section mem start */
    this->free_mem = first_hdr = first_sect.mem_start;

    if (this->new_file) {
        first_hdr->signature = HA_MEM_SIGNATURE;
        first_hdr->length = this->blk_len - sizeof(ha_mem_comp_struct_ctrl_blk_t);
        first_hdr->state = (uint8_t)HA_BLK_FREE;
        first_hdr->section = 0;
        first_hdr->prev_offset = this->blk_len;
        sal_memset(ctrl_blk,  0,  sizeof(*ctrl_blk));
        ctrl_blk->signature = HA_MEM_SIGNATURE;
        ctrl_blk->next_blk_offset = 0;
        ctrl_blk->next_blk_section = INVALID_BLOCK_SECTION;
    } else {
        rv = sanity_check(this);
        if (rv == SHR_E_NONE) {
            shr_ha_issu_struct_init(this);
        }
    }
    BSL_LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("start=%p, end=%p, len=%d, free=%p\n"),
                            (void *)this->mem_sect[0].mem_start,
                            (void *)this->mem_sect[0].mem_end,
                            this->blk_len,
                            (void *)this->mem_sect[0].free_mem));
    sal_mutex_give(this->mutex);
    this->initialized = (rv == 0);
    return rv;
}

/*!
 * \brief sanity_check verifies the memory validity.
 *
 * This function runs after reopening an existing file. It goes through
 * the entire memory to verify that every block has valid signature. It also
 * "fixes" some of the prev_offset fields. These needed to be fixed when
 * the original allocation was made using multiple sections. When the file
 * being reopen it will have a single section containing all the sections
 * that were there in the original memory allocation.
 *
 * \retval true no errors
 * \retval false the memory is corrupted
 */
static int sanity_check(ha_private_t *this)
{
    uint32_t section;
    uint32_t prev_blk_len = 0;
    ha_mem_blk_hdr_t *blk_hdr = this->mem_sect[0].mem_start;
    uint16_t prev_blk_id = 0xFFFF;

    section = blk_hdr->section;
    do {
        if (blk_hdr->signature != HA_MEM_SIGNATURE) {
            LOG_ERROR(BSL_LOG_MODULE,
                   (BSL_META("HA memory is being corrupt, last comp=%d sub=%d"),
                    prev_blk_id>>8, prev_blk_id & 0xFF));
            return SHR_E_INTERNAL;
        }
        /* Search for section transition */
        if (section != blk_hdr->section) {
            blk_hdr->prev_offset = prev_blk_len;
            section = blk_hdr->section;
        }
        /* Upon initialization everything belongs to the only section */
        blk_hdr->section = 0;
        prev_blk_len = blk_hdr->length;
        prev_blk_id = blk_hdr->blk_id;
        blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
    } while (blk_hdr < this->mem_sect[0].mem_end);

    return SHR_E_NONE;
}

/*!
 * \brief find a block with matched block ID.
 *
 * Search the entire HA memory for an accoupied block with matching blk_id
 *
 * \param [in] blk_id is the block ID to search for
 *
 * \retval pointer to the block if was found
 * \retval NULL if the block was not found
 */
ha_mem_blk_hdr_t *shr_ha_block_find(ha_private_t *this, uint16_t blk_id)
{
    ha_mem_blk_hdr_t *blk_hdr;
    ha_mem_section_t *it;
    size_t j;

    /* iterate through all the sections */
    for (j = 0; j < this->msec_free_idx; j++) {
        it = &this->mem_sect[j];
        blk_hdr = it->mem_start;
        /* seach each block in this section */
        do {
            /* validate signature */
            if (blk_hdr->signature != HA_MEM_SIGNATURE) {
                BSL_LOG_ERROR(BSL_LOG_MODULE, (BSL_META("invalid Ha block signature\n")));
                assert (0);
                return NULL;
            }
            /* if the block is allocated and the block ID matches then we found it */
            if ((blk_hdr->state == (uint8_t)HA_BLK_ALLOCATED) &&
                !(blk_hdr->attrib & BLOCK_INTERNAL_USE) &&
                (blk_hdr->blk_id == blk_id)) {
                return blk_hdr;
            }
            /* increment the block poiter to the next block */
            blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
        } while (blk_hdr < it->mem_end); /* bailed out if block pointer points beyond the end */
    }
    return NULL;
}

/*!
 * \brief Find free block of size larger or equal to min_length.
 *
 * Start searching at the section containing the free_mem block. Also start
 * searching from that free_mem block.
 * In the future this module can be optimized by maintaining a linked list of
 * all the free blocks.
 *
 * \param [in] this is the HA block context.
 * \param [in] min_length is the minimal length of the free block to search for.
 *
 * \retval pointer to the block if was found
 * \retval NULL if the block was not found
 */
static ha_mem_blk_hdr_t *find_free_block(ha_private_t *this,
                                         uint32_t min_length)
{
    ha_mem_blk_hdr_t *blk_hdr;
    int section_idx;
    unsigned j;
    unsigned mem_free_section;

    /*
    ** search in all the memory sections
    */
    if (this->free_mem) {
        mem_free_section = this->free_mem->section;
    } else {
        mem_free_section = 0;
    }

    for (j = 0; j < this->msec_free_idx; j++) {
        section_idx = (j + mem_free_section) % this->msec_free_idx;
        /* start the search at the mem_free block */
        blk_hdr = this->mem_sect[section_idx].free_mem;
        do {
            if (blk_hdr->signature != HA_MEM_SIGNATURE) {
                BSL_LOG_ERROR(BSL_LOG_MODULE, (BSL_META("invalid Ha block signature\n")));
                assert (0);
                return NULL;
            }

            if ((blk_hdr->state == (uint8_t)HA_BLK_FREE) &&
                (blk_hdr->length >= min_length)) {
                blk_hdr->section = section_idx;
                return blk_hdr;
            }
            /* Increament to the next block */
            blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
            if (blk_hdr >= this->mem_sect[section_idx].mem_end) {
                blk_hdr = this->mem_sect[section_idx].mem_start;    /* wrap around */
            }
        } while (blk_hdr != this->mem_sect[section_idx].free_mem); /* as long as we are not were we started */
    }
    return NULL;
}

/*!
 * \brief Find the previous block in the chain.
 *
 * If the input doesn't point to the start of the section then use
 * the prev_offset to move the block pointer accordingly
 *
 * \param [in] start is a reference to the block where the function
 *             is looking for its predecesor.
 *
 * \retval pointer to the block if was found
 * \retval NULL if the block was not found
 */
static ha_mem_blk_hdr_t *find_prev_block(ha_private_t *this,
                                         ha_mem_blk_hdr_t *start)
{
    ha_mem_blk_hdr_t *prev_blk_hdr;
    /* Check if start points to the begining of the section */
    if (start == this->mem_sect[start->section].mem_start) {
        return NULL;    /* no previous to the start */
    }
    prev_blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)start - start->prev_offset);
    /* Sanity check */
    if ((prev_blk_hdr < this->mem_sect[start->section].mem_start) ||
        (prev_blk_hdr > this->mem_sect[start->section].mem_end) ||
        (prev_blk_hdr->signature != HA_MEM_SIGNATURE)) {
        BSL_LOG_ERROR(BSL_LOG_MODULE, (BSL_META("invalid previous block\n")));
        assert(0);
    }

    prev_blk_hdr->section = start->section;
    return prev_blk_hdr;
}

/*!
 * \brief Resize shared memory section
 *
 * The resize of the shared memory involves two steps:
 * 1. resize the shared memory file
 * 2. Resize the memory map
 * The second step is a bit more complicated. The first attempt is to resize
 * the memory map. If that works everything is fine. The memory map resize may
 * fail due to the ability to extend the current shared memory block. The HA
 * can't allow shared memory relocation which may restrict the the possibility
 * for shared memory remapping.
 * Therefore, if the memory remap fails the HA creates a new memory map that is
 * mapped to the extension is size of the shared file. Internally, we call this
 * memory map a "section". Each section managed by section header that contains
 * information about the header length and free memory.
 *
 * \param [in] size is the extension size
 *
 * \retval pointer to the extended block
 * \retval NULL if something failed
 */
static ha_mem_blk_hdr_t *section_resize(ha_private_t *this, int size)
{
    uint8_t *mmap_ptr;
    ha_mem_section_t new_sect;
    int last_section = this->msec_free_idx-1;
    uint32_t resize_memory_blocks = size + RESIZE_MEMORY_SIZE;
    uint32_t addition_mem_size = 0;
    ha_mem_blk_hdr_t *blk_hdr;
    void *new_memptr;

    mmap_ptr = (uint8_t *)this->mem_sect[last_section].mem_start;
    if (last_section == 0) { /* The start of the first section is skewed */
        mmap_ptr -= sizeof(ha_mem_comp_struct_ctrl_blk_t);
    }
    new_memptr = this->resize_cb(this->blk_len,
                                 mmap_ptr,
                                 this->mem_sect[last_section].sect_len,
                                 resize_memory_blocks,
                                 &addition_mem_size,
                                 this->ctx);
    if ((new_memptr == NULL) || (addition_mem_size < resize_memory_blocks)) {
        return NULL;
    } else if (new_memptr != (void *)mmap_ptr) {
            /*
             * Was able to create new memory map section. Set the new section
             * header and add it to the memory section vector
             */
            new_sect.mem_start = (ha_mem_blk_hdr_t *)new_memptr;
            new_sect.mem_end = (ha_mem_blk_hdr_t *)
                        ((uint8_t *)new_sect.mem_start + addition_mem_size);
            new_sect.free_mem = new_sect.mem_start;
            new_sect.sect_len = addition_mem_size;
            push_back(this, &new_sect);
            last_section++; /* Added a new section  */
            blk_hdr = new_sect.mem_start;
    } else {  /* Remapping the memory worked so we have the same ptr */
        ha_mem_section_t *section = &this->mem_sect[last_section];
        blk_hdr = section->mem_end;
        /*
         * The first block keeps the length of the last block so when
         * we add a new section at the end it is easy to determine where
         * the previously last block starts
         */
        blk_hdr->prev_offset = section->mem_start->prev_offset;
        section->mem_start->prev_offset = addition_mem_size;

        section->free_mem = blk_hdr;
        section->mem_end = (ha_mem_blk_hdr_t *)
            ((uint8_t *)section->mem_end + addition_mem_size);
        section->sect_len += addition_mem_size;
    }
    /* Now add the free block at the end of the previous block */
    blk_hdr->signature = HA_MEM_SIGNATURE;
    blk_hdr->length = addition_mem_size;
    blk_hdr->state = (uint8_t)HA_BLK_FREE;
    blk_hdr->section = last_section;
    this->blk_len += addition_mem_size;    /* Update the total block length */
    this->free_mem = blk_hdr;  /* Update the global free_mem to the new block */
    BSL_LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("start=%p, end=%p, len=%d, free=%p\n"),
                            (void *)this->mem_sect[0].mem_start,
                            (void *)this->mem_sect[0].mem_end,
                            this->blk_len,
                            (void *)this->mem_sect[0].free_mem));
    return blk_hdr;
}


/*!
 * \brief Allocate HA memory block for the application
 *
 * This function searches for a block with matches mod and sub ID.
 * If such a block was not found the function allocates a new memory block
 * of the proper length and returns a pointer to the memory.
 * The version is used only in case that a matched block was found. In
 * this case if the versions are different the function will call
 * a method that will upgrade/downgrade the structure to the required
 * version. The structure signature identifies the schema that was used
 * to generate the data structure contained in this block
 *
 * \param [in] private Is the handle of the HA control block.
 * \param [in] mod_id is the module ID.
 * \param [in] sub_id is the sub module ID.
 * \param [in,out] length is a pointer contains the desired length The
 *        length also contains the actual allocated memory length.
 *
 * \retval pointer to the allocated block
 * \retval NULL if the block was not allocated
 */
static void *ha_alloc(void *private,
                      uint8_t mod_id,
                      uint8_t sub_id,
                      uint32_t *length)
{
    /* blk_hdr points to the first allocated block */
    ha_mem_blk_hdr_t *blk_hdr;
    uint16_t blk_id = (mod_id << 8) | sub_id;
    ha_private_t *this = (ha_private_t *)private;

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    /* Search for a block with match mod and sub ID */
    blk_hdr = shr_ha_block_find(this, blk_id);
    if (!blk_hdr) {   /* Buffer was not allocated before */
        blk_hdr = shr_ha_alloc_internal_block(this, *length);
        if (!blk_hdr) {
            return NULL;
        }
        blk_hdr->blk_id = blk_id;
        blk_hdr->attrib = 0;
    } else {
        /* Block was allocated before. Since we don't free we can reuse. */
        *length = blk_hdr->length - sizeof(ha_mem_blk_hdr_t);
    }

    sal_mutex_give(this->mutex);
    return (void *)((uint8_t *)blk_hdr + sizeof(ha_mem_blk_hdr_t));
}

/*!
 * \brief Reallocate existing memory.
 *
 * If the new desired size is smaller/equal to the current block length
 * do nothing. Otherwise, allocate new memory block using the length
 * parameter, copy the old memory onto the new block and free the old block
 *
 * \param [in] private Is the handle of the HA control block.
 * \param [in] mem is the old memoy handle.
 * \param [in,out] length is a pointer contains the desired length The
 *        length also contains the actual allocated memory length.
 *
 * \retval pointer to the block if was reallocated
 * \retval NULL otherwise
 */
static void *ha_realloc(void *private,
                        void *mem,
                        uint32_t length)
{
    ha_mem_blk_hdr_t *blk_hdr;
    void *new_blk_data;
    uint8_t mod_id;
    uint8_t sub_id;

    if (!mem) {
        return NULL;
    }
    blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)mem - sizeof(ha_mem_blk_hdr_t));
    /* sanity check */
    if ((blk_hdr->signature != HA_MEM_SIGNATURE) ||
        (blk_hdr->state != (uint8_t)HA_BLK_ALLOCATED)) {
        return NULL;
    }

    /* is the new length shorter? */
    if (blk_hdr->length - sizeof(ha_mem_blk_hdr_t) >= length) {
        return mem;
    }
    /* change the blk ID so we can allocate a new one with the same ID */
    /* first save the ID */
    mod_id = (uint8_t)(blk_hdr->blk_id >> 8);
    sub_id = (uint8_t)(blk_hdr->blk_id & 0xFF);
    if (blk_hdr->blk_id == 0) {
        blk_hdr->blk_id = 1;
    } else {
        blk_hdr->blk_id = 0;
    }
    new_blk_data = ha_alloc(private,
                           mod_id,
                           sub_id,
                           &length);
    if (!new_blk_data) {
        return NULL;
    }
    sal_memcpy (new_blk_data,
            mem,
            blk_hdr->length - sizeof(ha_mem_blk_hdr_t));

    /* free the old block */
    ha_free(private, mem);

    return new_blk_data;
}

/*!
 * \brief free memory block back into the pool
 *
 * Free memory tries to find adjacent free memory blocks to concatinate them
 * into a larger memory block
 *
 * \param [in] private Is the handle of the HA control block.
 * \param [in] mem is the old memoy handle
 *
 * \retval SHR_E_NONE on success
 * \retval SHR_E_PARAM when one of the input parameters was invalid.
 */
static int ha_free(void *private, void *mem)
{
    ha_mem_blk_hdr_t *blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)mem - sizeof(ha_mem_blk_hdr_t));
    ha_mem_blk_hdr_t *adj_blk_hdr;
    ha_private_t *this = (ha_private_t *)private;

    if (!mem) {
        return SHR_E_PARAM;
    }
    /* Sanity check that the block is valid */
    if ((blk_hdr->signature != HA_MEM_SIGNATURE) ||
        (blk_hdr->state != (uint8_t)HA_BLK_ALLOCATED)) {
        return SHR_E_PARAM;
    }

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    /* Free the block */
    blk_hdr->state = (uint8_t)HA_BLK_FREE;
    /* If the next block is free merge them together. */
    adj_blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
    if (adj_blk_hdr < this->mem_sect[blk_hdr->section].mem_end) {
        if (adj_blk_hdr->signature != HA_MEM_SIGNATURE) {
            BSL_LOG_ERROR(BSL_LOG_MODULE, (BSL_META("invalid HA block signature\n")));
            assert (0);
        }
        if (adj_blk_hdr->state == (uint8_t)HA_BLK_FREE) {
            /* Concatinate the next block */
            blk_hdr->length += adj_blk_hdr->length;
            /* Adjust the previous offset of the next block */
            adj_blk_hdr = (ha_mem_blk_hdr_t *)
                ((uint8_t *)blk_hdr + blk_hdr->length);
            if (adj_blk_hdr < this->mem_sect[blk_hdr->section].mem_end) {
                adj_blk_hdr->prev_offset = blk_hdr->length;
            }
        }
    }

    /* Now see if this block can be concatinated with the previous block */
    adj_blk_hdr = find_prev_block(this, blk_hdr);
    if (adj_blk_hdr && (adj_blk_hdr->state == (uint8_t)HA_BLK_FREE)) {
        /* Add blk_hdr to the previous block */
        adj_blk_hdr->length += blk_hdr->length;
        /* Adjust the previous offset of the next block */
        blk_hdr = (ha_mem_blk_hdr_t *)
            ((uint8_t *)adj_blk_hdr + adj_blk_hdr->length);
        if (blk_hdr < this->mem_sect[adj_blk_hdr->section].mem_end) {
            blk_hdr->prev_offset = adj_blk_hdr->length;
        }
    }
    sal_mutex_give(this->mutex);
    return SHR_E_NONE;
}

const shr_ha_drv_t shr_ha_drv = {
    .create = ha_mm_create,
    .delete = ha_mm_delete,
    .init = mem_init,
    .alloc = ha_alloc,
    .realloc = ha_realloc,
    .free = ha_free,
    .struct_report = shr_ha_struct_report_ha,
    .struct_info = shr_ha_struct_info_ha
};

/*================================================================*
 *     C O M P O N E N T    I N T E R N A L   F U N C T I O N S   *
 *================================================================*/
ha_mem_blk_hdr_t* shr_ha_alloc_internal_block(ha_private_t *this,
                                              uint32_t length)
{
    uint32_t space_to_allocate;
    ha_mem_blk_hdr_t *blk_hdr;
    ha_mem_blk_hdr_t *new_blk_hdr;

    BSL_LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("allocating new buffer free=%p\n"),
                     (void *)this->free_mem));
    /* Make sure that the length is multiplication of 8 */
    space_to_allocate = ((length + sizeof(ha_mem_blk_hdr_t) + 7) >> 3) << 3;
    /* Now we assume that we will find a free block large enough. */
    blk_hdr = find_free_block(this, space_to_allocate);
    if (!blk_hdr) {
        BSL_LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("calling section_resize\n")));
        /*
         * If we can't find free block that is large enough we need to
         * extend the shared memory
         */
        blk_hdr = section_resize(this, space_to_allocate);
        if (!blk_hdr) {  /* Shared memory extension fails? */
            sal_mutex_give(this->mutex);
            return NULL;
        }
    }

    new_blk_hdr = NULL;
    /* Set the new free block if some space had left */
    if (blk_hdr->length > space_to_allocate + sizeof(ha_mem_blk_hdr_t)) {
        new_blk_hdr = (ha_mem_blk_hdr_t *)
            ((uint8_t *)blk_hdr + space_to_allocate);
        new_blk_hdr->length = blk_hdr->length - space_to_allocate;
        new_blk_hdr->signature = HA_MEM_SIGNATURE;
        new_blk_hdr->state = (uint8_t)HA_BLK_FREE;
        new_blk_hdr->section = blk_hdr->section;
        new_blk_hdr->prev_offset = space_to_allocate;
        /*
         * If this is the last block need to adjust the prev offset of the
         * first block
         */
        if ((ha_mem_blk_hdr_t *)
            ((uint8_t *)new_blk_hdr + new_blk_hdr->length) ==
                this->mem_sect[blk_hdr->section].mem_end) {
            this->mem_sect[blk_hdr->section].mem_start->prev_offset =
                new_blk_hdr->length;
        }
    }

    blk_hdr->length = space_to_allocate;
    blk_hdr->state = (uint8_t)HA_BLK_ALLOCATED;
    if (blk_hdr == this->free_mem) {
        /* Find new value for the free mem  */
        if (new_blk_hdr) {
            this->free_mem = new_blk_hdr;
        } else {  /* Need to find free block */
            this->free_mem =
                find_free_block(this, (uint32_t)(sizeof(ha_mem_blk_hdr_t) + 1));
            if (!this->free_mem) { /* Make sure that free_mem is not null */
                this->free_mem = this->mem_sect[0].mem_start;
                this->free_mem->section = 0;
            }
        }
    }

    return blk_hdr;
}

