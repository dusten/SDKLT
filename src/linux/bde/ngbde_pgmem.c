/*! \file ngbde_pgmem.c
 *
 * \brief PGMEM allocator.
 *
 * This module is used to allocate large physically contiguous memory
 * blocks using the Linux kernel page allocator.
 *
 * The Linux page allocator can allocate contiguous memory up until a
 * certain size, which depends on the kernel version and the CPU
 * architecture.
 *
 * If a larger contiguous memory block is requested, then we need to
 * allocate multiple blocks from the Linux page allocator and then
 * check if which ones are contiguous.
 *
 * The smaller memory blocks from which the larger block is assembled
 * are referred to as "chunks".
 *
 * The PGMEM allocator will continue to allocate chunks from the Linux
 * page allocator, until a contiguous memory block of the requested
 * size has been assembled, or until a predefined maximum number of
 * chunks have been allocated. Obviously the process is also stopped
 * if the Linux page allocator returns an error.
 *
 * A physically contiguous memory block assembled from smaller memory
 * chunks are referred to as "cmblocks".
 *
 * The chance of success depends on the requested memory block size as
 * well as the fragmentation level of the system memory, i.e. the
 * sooner after system boot these memory block are requested, the more
 * likely these requests are to succeed.
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

#include <ngbde.h>


/*******************************************************************************
 * Local definitions
 */

/*! Maximum size the kernel can allocate in a single allocation. */
#define MEM_CHUNK_SIZE_MAX      (1 << (MAX_ORDER - 1 + PAGE_SHIFT))

/*! Default block size we wil request from the kernel. */
#define MEM_CHUNK_SIZE_DEFAULT  (512 * ONE_KB)

/*! \cond */
static int pgmem_chunk_size = 0;
module_param(pgmem_chunk_size, int, 0);
MODULE_PARM_DESC(pgmem_chunk_size,
"Memory chunk size in KB used by page allocator (default auto).");
/*! \endcond */

/*! \cond */
static int pgmem_debug = 0;
module_param(pgmem_debug, int, 0);
MODULE_PARM_DESC(pgmem_debug,
"Enable page memory allocator debug output (default 0).");
/*! \endcond */

/*! Helper macro for debug trace output. */
#define PGMEM_TRACE(_s)                         \
    do {                                        \
        if (pgmem_debug) {                      \
            printk(_s);                         \
        }                                       \
    } while (0)

/*!
 * Chunk memory block descriptor.
 */
typedef struct cmblock_desc_s {

    /*! Linked-list handle. */
    struct list_head list;

    /*! Requested cmblock size. */
    unsigned long req_size;

    /*! Memory chunk size. */
    unsigned long chunk_size;

    /*! Memory chunk size in alternate format (2^x). */
    unsigned long chunk_order;

    /*! Current cmblock size. */
    unsigned long cmblk_size;

    /*! Logical address of cmblock. */
    unsigned long cmblk_begin;

    /*! Logical end address of cmblock. */
    unsigned long cmblk_end;

    /*! Array of logical chunk addresses. */
    unsigned long *chunk_ptr;

    /*! Maximum number of chunks to allocate. */
    int chunk_cnt_max;

    /*! Current number of chunks allocated. */
    int chunk_cnt;

} cmblock_desc_t;

static LIST_HEAD(cmblocks_list);


/*!
 * \name Chunk tag mask.
 * \anchor CT_xxx
 *
 * The lower two bits of the chunk address is used to tag the chunk
 * with its current state.
 */
#define CT_MASK         0x3

/*! Chunk is untagged. */
#define CT_UNTAGGED     0

/*! Chunk was discarded. */
#define CT_DISCARDED    1

/*! Chunk is part of largest cmblock. */
#define CT_LARGEST      2

/*! Chunk is part of current cmblock. */
#define CT_CURRENT      3

/*! Set block as untagged. */
#define CTAG_SET(_a, _t)                        \
    do {                                        \
        (_a) &= ~CT_MASK;                       \
        (_a) |= _t;                             \
    } while (0)

/*! Set block as untagged. */
#define CTAG_GET(_a)                            \
    ((_a) & CT_MASK)


/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Find largest contiguous memory block.
 *
 * Find largest contiguous memory block from a pool of memory chunks.
 *
 * Assembly stops if a cmblock of the requested cmblock size has been
 * obtained.
 *
 * The lower two address bits of the memory chunks are encoded as a
 * tag according to \ref CT_xxx.
 *
 * \param [in] cmbd cmblock descriptor.
 *
 * \return Always 0.
 */
static int
find_largest_cmblock(cmblock_desc_t *cmbd)
{
    int i, j, chunks, found;
    unsigned long b, e, a;
    unsigned long *cptr;

    /* Convenience variable */
    chunks = cmbd->chunk_cnt;
    cptr = cmbd->chunk_ptr;

    /* Clear all chunk tags */
    for (i = 0; i < chunks; i++) {
        CTAG_SET(cptr[i], CT_UNTAGGED);
    }
    for (i = 0; i < chunks && cmbd->cmblk_size < cmbd->req_size; i++) {
        /* First chunk must be an untagged chunk */
        if (CTAG_GET(cptr[i]) == CT_UNTAGGED) {
            /* Initial cmblock size is the chunk size */
            b = cptr[i];
            e = b + cmbd->chunk_size;
            CTAG_SET(cptr[i], CT_CURRENT);
            /* Loop looking for adjacent chunks */
            do {
                found = 0;
                for (j = i + 1; j < chunks && (e - b) < cmbd->req_size; j++) {
                    a = cptr[j];
                    /* Check untagged chunks only */
                    if (CTAG_GET(a) == CT_UNTAGGED) {
                        if (a == (b - cmbd->chunk_size)) {
                            /* Found adjacent chunk below current cmblock */
                            CTAG_SET(cptr[j], CT_CURRENT);
                            b = a;
                            found = 1;
                        } else if (a == e) {
                            /* Found adjacent chunk above current cmblock */
                            CTAG_SET(cptr[j], CT_CURRENT);
                            e += cmbd->chunk_size;
                            found = 1;
                        }
                    }
                }
            } while (found);
            /* Now check the size of the assembled memory block */
            if ((e - b) > cmbd->cmblk_size) {
                /* The current block is largest so far */
                cmbd->cmblk_begin = b;
                cmbd->cmblk_end = e;
                cmbd->cmblk_size = e - b;
                /* Re-tag current and previous largest cmblock */
                for (j = 0; j < chunks; j++) {
                    if (CTAG_GET(cptr[j]) == CT_CURRENT) {
                        /* Tag current cmblock as the largest */
                        CTAG_SET(cptr[j], CT_LARGEST);
                    } else if (CTAG_GET(cptr[j]) == CT_LARGEST) {
                        /* Discard previous largest cmblock */
                        CTAG_SET(cptr[j], CT_DISCARDED);
                    }
                }
            } else {
                /* Discard all chunks in current cmblock */
                for (j = 0; j < chunks; j++) {
                    if (CTAG_GET(cptr[j]) == CT_CURRENT) {
                        CTAG_SET(cptr[j], CT_DISCARDED);
                    }
                }
            }
        }
    }
    return 0;
}

/*!
 * \brief Allocate memory chunks and add them to the pool.
 *
 * Memory chunks are allocated using the kernel page allocator.
 *
 * \param [in] cmbd - cmblock descriptor.
 * \param [in] chunks - Number of memory chunks to allocate.
 *
 * \return 0 if no errors, otherwise -1.
 */
static int
alloc_mem_chunks(cmblock_desc_t *cmbd, int chunks)
{
    int i, start;
    unsigned long addr;

    if (cmbd->chunk_cnt + chunks > cmbd->chunk_cnt_max) {
        printk("PGMEM: No more memory chunks\n");
        return -1;
    }
    start = cmbd->chunk_cnt;
    cmbd->chunk_cnt += chunks;
    for (i = start; i < cmbd->chunk_cnt; i++) {
        /* Get chunk from kernel allocator */
        addr = __get_free_pages(GFP_KERNEL | GFP_DMA32, cmbd->chunk_order);
        PGMEM_TRACE(".");
        if (addr) {
            cmbd->chunk_ptr[i] = addr;
        } else {
            printk("PGMEM: Page memory allocation failed\n");
            return -1;
        }
    }
    return 0;
}

/*!
 * \brief Allocate large physically contiguous memory block.
 *
 * If we cannot allocate a sufficiently large block of contiguous
 * memory from the kernel, then we simply keep allocating smaller
 * chunks until we can assemble a contiguous block of the desired
 * size.
 *
 * When maximum amount of system memory has been allocated without the
 * successful assembly of a contiguous memory block, the allocation
 * function will return the largest contiguous block found so far. It
 * is then up to the calling function to decide whether this amount is
 * sufficient to proceed.
 *
 * \param [in] size Requested memory block size.
 * \param [in] chunk_size Assemble cmblock from chunks of this size.
 *
 * \return Pointer to cmblock descriptor, or NULL if error.
 */
static cmblock_desc_t *
cmblock_alloc(size_t size, size_t chunk_size)
{
    cmblock_desc_t *cmbd;
    int i, chunk_ptr_size;
    unsigned long page_addr;
    struct sysinfo si;

    /* Sanity check */
    if (size == 0 || chunk_size == 0) {
        return NULL;
    }

    /* Allocate an initialize memory cmblock descriptor */
    if ((cmbd = kmalloc(sizeof(cmblock_desc_t), GFP_KERNEL)) == NULL) {
        return NULL;
    }
    memset(cmbd, 0, sizeof(*cmbd));
    cmbd->req_size = size;
    cmbd->chunk_size = PAGE_ALIGN(chunk_size);
    while ((PAGE_SIZE << cmbd->chunk_order) < cmbd->chunk_size) {
        cmbd->chunk_order++;
    }

    /* Determine the maximum possible number of memory chunks */
    si_meminfo(&si);
    cmbd->chunk_cnt_max = (si.totalram << PAGE_SHIFT) / cmbd->chunk_size;
    chunk_ptr_size = cmbd->chunk_cnt_max * sizeof(unsigned long);

    /* Allocate an initialize memory chunk pool */
    cmbd->chunk_ptr = kmalloc(chunk_ptr_size, GFP_KERNEL);
    if (cmbd->chunk_ptr == NULL) {
        kfree(cmbd);
        return NULL;
    }
    memset(cmbd->chunk_ptr, 0, chunk_ptr_size);

    /* Allocate minimum number of memory chunks */
    (void)alloc_mem_chunks(cmbd, cmbd->req_size / cmbd->chunk_size);

    /* Allocate more chunks until we have a complete cmblock */
    do {
        find_largest_cmblock(cmbd);
        PGMEM_TRACE("o");
        if (cmbd->cmblk_size >= cmbd->req_size) {
            break;
        }
    } while (alloc_mem_chunks(cmbd, 8) == 0);

    /* Reserve all pages in the cmblock and free unused chunks */
    for (i = 0; i < cmbd->chunk_cnt; i++) {
        if (CTAG_GET(cmbd->chunk_ptr[i]) == CT_LARGEST) {
            CTAG_SET(cmbd->chunk_ptr[i], CT_UNTAGGED);
            for (page_addr = cmbd->chunk_ptr[i];
                 page_addr < cmbd->chunk_ptr[i] + cmbd->chunk_size;
                 page_addr += PAGE_SIZE) {
                SetPageReserved(virt_to_page((void *)page_addr));
            }
        } else if (cmbd->chunk_ptr[i]) {
            CTAG_SET(cmbd->chunk_ptr[i], CT_UNTAGGED);
            free_pages(cmbd->chunk_ptr[i], cmbd->chunk_order);
            PGMEM_TRACE("x");
            cmbd->chunk_ptr[i] = 0;
        }
    }
    PGMEM_TRACE("O\n");
    return cmbd;
}

/*!
 * \brief Free cmblock and associated resources.
 *
 * Free all memory chunks and other associated resources associated
 * with a contiguous memory block.
 *
 * See alse \ref cmblock_alloc.
 *
 * \param [in] cmbd Command block descriptor to free.
 *
 * \return Nothing.
 */
static void
cmblock_free(cmblock_desc_t *cmbd)
{
    int i;
    unsigned long page_addr;

    if (cmbd->chunk_ptr) {
        for (i = 0; i < cmbd->chunk_cnt; i++) {
            if (cmbd->chunk_ptr[i]) {
                for (page_addr = cmbd->chunk_ptr[i];
                     page_addr < cmbd->chunk_ptr[i] + cmbd->chunk_size;
                     page_addr += PAGE_SIZE) {
                    ClearPageReserved(virt_to_page((void *)page_addr));
                }
                free_pages(cmbd->chunk_ptr[i], cmbd->chunk_order);
                PGMEM_TRACE("X");
            }
        }
        kfree(cmbd->chunk_ptr);
        kfree(cmbd);
    }
}


/*******************************************************************************
 * Public Functions
 */

void *
ngbde_pgmem_alloc(size_t size, gfp_t flags)
{
    cmblock_desc_t *cmbd;
    size_t chunk_size;

    chunk_size = size;

    if (pgmem_chunk_size > 0) {
        chunk_size = pgmem_chunk_size * ONE_KB;
    }

    if (chunk_size > MEM_CHUNK_SIZE_MAX) {
        chunk_size = MEM_CHUNK_SIZE_DEFAULT;
    }

    if (pgmem_debug) {
        printk("PGMEM: Allocate %d MB in %d KB chunks\n",
               (int)(size / ONE_MB), (int)(chunk_size / ONE_KB));
    }

    if ((cmbd = cmblock_alloc(size, chunk_size)) == NULL) {
        return NULL;
    }
    if (cmbd->cmblk_size < size) {
        /* If we didn't get the full size then forget it */
        cmblock_free(cmbd);
        return NULL;
    }
    list_add(&cmbd->list, &cmblocks_list);
    return (void *)cmbd->cmblk_begin;
}

int
ngbde_pgmem_free(void *ptr)
{
    struct list_head *pos;

    list_for_each(pos, &cmblocks_list) {
        cmblock_desc_t *cmbd = list_entry(pos, cmblock_desc_t, list);
        if (ptr == (void *)cmbd->cmblk_begin) {
            list_del(&cmbd->list);
            cmblock_free(cmbd);
            return 0;
        }
    }
    return -1;
}

void
ngbde_pgmem_free_all(void)
{
    struct list_head *pos, *tmp;

    list_for_each_safe(pos, tmp, &cmblocks_list) {
        cmblock_desc_t *cmbd = list_entry(pos, cmblock_desc_t, list);
        list_del(&cmbd->list);
        cmblock_free(cmbd);
    }
}
