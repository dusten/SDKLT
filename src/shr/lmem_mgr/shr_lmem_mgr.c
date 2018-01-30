/*! \file shr_lmem_mgr.c
 *
 * Local memory manager.
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

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_lmem_mgr.h>

#define LMEM_MGR_SIGNATURE   0x75f2a5dc
#define ROBUST_ENABLE       1

#if ROBUST_ENABLE
typedef enum {ELEMENT_FREE, ELEMENT_ALLOC
} element_state_t;
#endif

typedef struct local_mem_chunks_s {
    void *mem;
    struct local_mem_chunks_s *next;
} local_mem_chunks_t;

typedef struct local_mem_mgr_s {
    uint32_t signature;
    size_t element_size;
    size_t chunk_size;
    uint32_t pointer_offset;
    bool mt;
    uint32_t max_count;
    uint32_t allocated;
    sal_mutex_t mtx;
    uint8_t *free_list;
    local_mem_chunks_t *chunk_list;
} local_mem_mgr_t;

static void arrange_new_chunk(local_mem_mgr_t *lm)
{
    uint8_t *p_element = lm->free_list;
    uint8_t **next;
    size_t j;
#if ROBUST_ENABLE
    uint32_t *state;
#endif

    for (j = 0; j < lm->chunk_size-1; j++) {
        next = (uint8_t **)(p_element + lm->pointer_offset);
#if ROBUST_ENABLE
        state = (uint32_t *)(p_element + lm->element_size - sizeof(uint32_t));
        *state = ELEMENT_FREE;
#endif
        p_element += lm->element_size;
        *next = p_element;
    }
    /* last element */
    next = (uint8_t **)(p_element + lm->pointer_offset);
    *next = NULL;
}

int
shr_lmm_init(size_t chunk_size,
             size_t element_size,
             uint32_t pointer_offset,
             bool multi_thread,
             shr_lmm_hdl_t *hdl)
{
    return shr_lmm_restrict_init(chunk_size, element_size, pointer_offset,
                                 multi_thread, 0xFFFFFFFF, hdl);
}

int
shr_lmm_restrict_init(size_t chunk_size,
                      size_t element_size,
                      uint32_t pointer_offset,
                      bool multi_thread,
                      uint32_t max_elements,
                      shr_lmm_hdl_t *hdl)
{
    local_mem_mgr_t *lm;
    lm = sal_alloc(sizeof(local_mem_mgr_t), "shrLmmMgr");
    if (!lm) {
        return -1;
    }
    lm->chunk_size = chunk_size;
#if ROBUST_ENABLE
    element_size += sizeof(uint32_t);
#endif
    lm->element_size = element_size;
    lm->pointer_offset = pointer_offset;
    lm->signature = LMEM_MGR_SIGNATURE;
    lm->mt = multi_thread;
    lm->max_count = max_elements;
    lm->allocated = 0;
    lm->free_list = sal_alloc(chunk_size * element_size, "shrLmmFree");
    if (!lm->free_list) {
        sal_free  (lm);
        return -1;
    }
    sal_memset(lm->free_list, 0, chunk_size * element_size);
    lm->chunk_list = sal_alloc(sizeof(local_mem_chunks_t), "shrLmmChunk");
    if (!lm->chunk_list) {
        sal_free (lm->free_list);
        sal_free (lm);
        return -1;
    }
    lm->chunk_list->mem = lm->free_list;
    lm->chunk_list->next = NULL;
    arrange_new_chunk(lm);
    if (multi_thread) {
        lm->mtx = sal_mutex_create("");
        if (!lm->mtx) {
            lm->mt = false;
            shr_lmm_delete((shr_lmm_hdl_t)lm);
            return -1;
        }
    }
    *hdl = (shr_lmm_hdl_t)lm;
    return 0;
}


void *shr_lmm_alloc(shr_lmm_hdl_t hdl)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    void *rv;
    uint8_t **next;
    local_mem_chunks_t *p_chunk;
    size_t alloc_size;
    bool success = false;
#if ROBUST_ENABLE
    uint32_t *state;
#endif

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE)) {
        return NULL;
    }
    if (lm->mt) {
        sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
    }
    if (lm->allocated >= lm->max_count) {
        if (lm->mt) {
            sal_mutex_give(lm->mtx);
        }
        return NULL;
    }
    if (lm->free_list == NULL) {
        do {
            alloc_size = lm->chunk_size * lm->element_size;
            lm->free_list = sal_alloc(alloc_size, "shrLmmFree");
            if (!lm->free_list) {
                break;
            }
            sal_memset(lm->free_list, 0, alloc_size);
            p_chunk = sal_alloc(sizeof(local_mem_chunks_t), "shrLmmChunk");
            if (!p_chunk) {
                break;
            }
            success = true;
        } while (0);
        if (!success) {
            if (lm->free_list != NULL) {
                sal_free(lm->free_list);
                lm->free_list = NULL;
            }
            if (lm->mt) {
                sal_mutex_give(lm->mtx);
            }
            return NULL;
        }
        p_chunk->mem = lm->free_list;
        p_chunk->next = lm->chunk_list;
        lm->chunk_list = p_chunk;
        arrange_new_chunk(lm);
    }
    rv = (void *)lm->free_list;
#if ROBUST_ENABLE
        state = (uint32_t *)(lm->free_list + lm->element_size - sizeof(uint32_t));
        *state = ELEMENT_ALLOC;
#endif

    next = (uint8_t **)(lm->free_list + lm->pointer_offset);
    lm->free_list = *next;
    lm->allocated++;
    if (lm->mt) {
        sal_mutex_give(lm->mtx);
    }
    return rv;
}

void shr_lmm_free(shr_lmm_hdl_t hdl, void *element)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    uint8_t **next;
#if ROBUST_ENABLE
    uint32_t *state;
#endif

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE) || !element) {
        return;
    }
#if ROBUST_ENABLE
        state = (uint32_t *)(((uint8_t *)element) +
                             lm->element_size - sizeof(uint32_t));
        if (*state == ELEMENT_FREE) {
            return;
        }
        *state = ELEMENT_FREE;
#endif

    next = (uint8_t **)((uint8_t *)element + lm->pointer_offset);
    if (lm->mt) {
        sal_mutex_take(lm->mtx, SAL_MUTEX_FOREVER);
    }
    *next = lm->free_list;
    lm->free_list = element;
    lm->allocated--;
    if (lm->mt) {
        sal_mutex_give(lm->mtx);
    }
}

void shr_lmm_delete(shr_lmm_hdl_t hdl)
{
    local_mem_mgr_t *lm = (local_mem_mgr_t *)hdl;
    local_mem_chunks_t *p_chunk;

    if (!lm || (lm->signature != LMEM_MGR_SIGNATURE)) {
        return;
    }
    if (lm->mt) {
        sal_mutex_destroy(lm->mtx);
    }
    lm->signature = 0;  /* so consequtive call will fail */
    /* sal_free  all the chunks */
    while (lm->chunk_list) {
        p_chunk = lm->chunk_list;
        lm->chunk_list = p_chunk->next;
        sal_free  (p_chunk->mem);
        sal_free  (p_chunk);
    }
    sal_free  (lm);
}
