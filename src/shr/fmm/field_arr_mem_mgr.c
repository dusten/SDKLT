/*! \file field_arr_mem_mgr.c
 *
 * Field array memory manager API implementation
 *
 * Interface to the field array memory manager
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
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>

#define FIELD_ARR_ALLOC_CHUNKS  2

typedef struct famm_hdl_s {
    uint32_t size;
    uint32_t ref_count;
    shr_lmm_hdl_t hdl;
    struct famm_hdl_s *next;
} famm_hdl_t;

static famm_hdl_t *famm_hdl_list;
static sal_mutex_t famm_mutex;

#define FAMM_LOCK() sal_mutex_take(famm_mutex, SAL_MUTEX_FOREVER)
#define FAMM_UNLOCK() sal_mutex_give(famm_mutex)

int shr_famm_init(void)
{
    shr_famm_cleanup();

    famm_mutex = sal_mutex_create("fld_arr_mem_mgr");
    if (!famm_mutex) {
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

void shr_famm_cleanup(void)
{
    famm_hdl_t *fh;

    if (famm_mutex) {
        sal_mutex_destroy(famm_mutex);
        famm_mutex = 0;
    }

    while (famm_hdl_list) {
        fh = famm_hdl_list;
        famm_hdl_list = famm_hdl_list->next;
        shr_lmm_delete(fh->hdl);
        sal_free(fh);
    }
}

int shr_famm_hdl_init(uint32_t num_of_fld, shr_famm_hdl_t *hdl)
{
    int rv;
    uint32_t element_size, pointer_offset;
    famm_hdl_t *fh;

    FAMM_LOCK();
    fh = famm_hdl_list;
    while (fh) {
        if (fh->size == num_of_fld) {
            fh->ref_count++;
            break;
        }
        fh = fh->next;
    }

    if (!fh) {
        fh = sal_alloc(sizeof(*fh), "fld_arr_hdl");
        if (!fh) {
            FAMM_UNLOCK();
            return SHR_E_MEMORY;
        }
        fh->size = num_of_fld;
        fh->ref_count = 1;

        /*
         * Initialize local memory instance for field array.
         * The basic element is composed of:
         *  1. Array of fmm pointers (array_size is specified by num_of_fld)
         *  2. Pointer to the next element in the list
         */
        element_size = (num_of_fld + 1) * sizeof(shr_fmm_t *);
        pointer_offset = num_of_fld * sizeof(shr_fmm_t *);

        rv = shr_lmm_init(FIELD_ARR_ALLOC_CHUNKS,
                          element_size,
                          pointer_offset,
                          true,
                          &fh->hdl);

        if (0 != rv) {
            sal_free(fh);
            FAMM_UNLOCK();
            return SHR_E_MEMORY;
        }

        fh->next = famm_hdl_list;
        famm_hdl_list = fh;
    }
    FAMM_UNLOCK();

    *hdl = (shr_famm_hdl_t)fh;

    return SHR_E_NONE;
}

void shr_famm_hdl_delete(shr_famm_hdl_t hdl)
{
    famm_hdl_t *fh = (famm_hdl_t *)hdl;

    if (!fh) {
        return;
    }

    FAMM_LOCK();
    if (--fh->ref_count == 0) {
        famm_hdl_t *tmp;
        shr_lmm_delete(fh->hdl);
        /* delete the element from the linked list */
        if (fh == famm_hdl_list) {
            famm_hdl_list = fh->next;
        } else {
            tmp = famm_hdl_list;
            while (tmp->next) {
                if (tmp->next == fh) {
                    tmp->next = fh->next;
                    break;
                }
                tmp = tmp->next;
            }
        }
        sal_free(fh);
    }
    FAMM_UNLOCK();
}

shr_fmm_t** shr_famm_alloc(shr_famm_hdl_t hdl, uint32_t num_of_fld)
{
    famm_hdl_t *fh = (famm_hdl_t *)hdl;
    shr_fmm_t **field_arr;
    uint32_t i;

    if (!fh || fh->size < num_of_fld)  {
        return NULL;
    }

    field_arr = shr_lmm_alloc(fh->hdl);
    if (!field_arr) {
        return NULL;
    }

    for (i = 0; i < num_of_fld; ) {
        field_arr[i] = shr_fmm_alloc();
        if (!field_arr[i]) {
            shr_famm_free(hdl, field_arr, i);
            return NULL;
        }
        sal_memset(field_arr[i], 0, sizeof(shr_fmm_t));
        i++;
    }

    return field_arr;
}

void shr_famm_free(shr_famm_hdl_t hdl,
                   shr_fmm_t ** field_arr,
                   uint32_t num_of_fld)
{
    famm_hdl_t *fh = (famm_hdl_t *)hdl;
    uint32_t i;

    if (fh->size < num_of_fld)  {
        num_of_fld = fh->size;
    }

    for (i = 0; i < num_of_fld; i++) {
        shr_fmm_free(field_arr[i]);
    }

    shr_lmm_free(fh->hdl, (void *)field_arr);
}
