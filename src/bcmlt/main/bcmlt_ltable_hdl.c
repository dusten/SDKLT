/*! \file bcmlt_ltable_hdl.c
 *
 *  Logical table handle manager.
 *  This file convert handle to pointer and pointer to handle.
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
#include <sal/sal_mutex.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmlt/bcmlt.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/*
 * Determine the number of handles that will be initially allocate
 */
#define HANDLE_ARRAY_CHUNK_INIT  300

/*
 * Determine the number of handles that will be allocate with every extension.
 */
#define HANDLE_ARRAY_CHUNK  100

/*
 * Rather that starting from 0 start for this number so random handle
 * numbers will be invalid.
 */
#define HANDLE_BASE     1000

/*!
 * \brief Resource manager structure for handles
 */
typedef struct {
    uint8_t *array;   /*!< Pointer to data array. */
    uint8_t *free_list;  /*!< Point to the begining of the free list. */
    uint8_t *last_elem;  /*!< Point to the last element of the free list. */
    sal_mutex_t mutex;   /*!< Protect multi thread resource allocation. */
    uint32_t array_size;  /*!< Maintains the array size. */
} hdl_resource_t;

static hdl_resource_t hdl_resource;

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Initializes pointer array free list.
 *
 * This function Initializes the input array to be a link list of elements.
 * It then sets the field \c last_elem of res (resource) to point to the
 * last element.
 *
 * \param [in] array Is the begining of the array of pointers.
 * \param [in] res Is the resource control structure. It maintains the
 * \c array as well as the pointers to the first and last free elements.
 * \param [in] size Is the number of pointers available in the array.
 *
 * \return None.
 */
static void ptr_array_init(void *array, hdl_resource_t *res, uint32_t size)
{
    uint32_t j;
    uint8_t **ptr = (uint8_t **)array;

    for (j = 0; j < size-1; j++) {
        *ptr = (uint8_t *)(ptr + 1);
        ptr++;
    }
    *ptr = NULL;
    res->last_elem = (uint8_t *)ptr;
}

/*!
 *\brief Extend pointer array free list.
 *
 * This function extends the pointers array by extending the original array
 * and initializing the new extention.
 *
 * \param [in] res Is the resource control structure. It maintains the
 * \c array as well as the pointers to the first and last free elements.
 * \param [in] size Is the number of pointers needed to be added.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int ptr_array_extend(hdl_resource_t *res, uint32_t size)
{
    void *ptr = res->array;

    /* Resize the array */
    ptr = shr_util_realloc(ptr,
                           res->array_size * sizeof(uint8_t *),
                           size * sizeof(uint8_t *),
                           "bcmltPtrArray");
    if (!ptr) {
        return SHR_E_MEMORY;
    }
    res->array = ptr;

    /* Set ptr to point to the uninitialized section of the array */
    ptr = (res->array + res->array_size * sizeof(uint8_t *));
    ptr_array_init(ptr, res, size - res->array_size);

    res->array_size = size;
    res->free_list = ptr;
    return SHR_E_NONE;
}

/*******************************************************************************
 * External functions local to this component
 */
int bcmlt_hdl_init(void)
{
    hdl_resource_t *res = &hdl_resource;
    uint32_t array_len = sizeof(uint8_t *) * HANDLE_ARRAY_CHUNK_INIT;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_ALLOC(res->array, array_len, "BCMLT_HDL");
    SHR_NULL_CHECK(res->array, SHR_E_MEMORY);
    res->array_size = HANDLE_ARRAY_CHUNK_INIT;

    /* Initialize the free list */
    res->free_list = res->array;
    ptr_array_init(res->free_list, res, HANDLE_ARRAY_CHUNK_INIT);

    res->mutex = sal_mutex_create("BCMLT_HDL");
    SHR_NULL_CHECK(res->mutex, SHR_E_MEMORY);

exit:
    if (SHR_FUNC_ERR()) {
        if (res->array) {
            sal_free(res->array);
            res->array = NULL;
            res->array_size = 0;
        }
    }
    SHR_FUNC_EXIT();
}

void bcmlt_hdl_shutdown(void)
{
    if (hdl_resource.array) {
        sal_free(hdl_resource.array);
        hdl_resource.array = NULL;
    }
    hdl_resource.array_size = 0;
    hdl_resource.free_list = NULL;
    if (hdl_resource.mutex) {
        sal_mutex_destroy(hdl_resource.mutex);
        hdl_resource.mutex = NULL;
    }
}

int bcmlt_hdl_alloc(void *data, uint32_t *hdl)
{
    hdl_resource_t *res = &hdl_resource;
    int rv;
    uint8_t **hdl_slot;

    if (sal_mutex_take(res->mutex, SAL_MUTEX_FOREVER) != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }
    if (res->free_list == NULL) {
        rv = ptr_array_extend(res, res->array_size + HANDLE_ARRAY_CHUNK);
        if (rv != SHR_E_NONE) {
            sal_mutex_give(res->mutex);
            return rv;
        }
    }
    hdl_slot = (uint8_t **)res->free_list;
    res->free_list = *hdl_slot;

    /* If that is the last element then update the last element */
    if (!res->free_list) {
        res->last_elem = NULL;
    }

    *hdl_slot = data;
    *hdl = (hdl_slot - ((uint8_t **)res->array)) + HANDLE_BASE;

    if (sal_mutex_give(res->mutex) != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }
    return SHR_E_NONE;
}

int bcmlt_hdl_free(uint32_t hdl)
{
    uint8_t *hdl_slot;
    hdl_resource_t *res = &hdl_resource;
    uint8_t *ptr_val;

    hdl -= HANDLE_BASE;
    if (hdl > res->array_size) {
        return SHR_E_PARAM;
    }
    /*
     * Since the entire array can be reallocated onto a new address we need
     * to make sure it doesn't happen while we obtaining a pointer to the
     * elements of this array.
     */
    if (sal_mutex_take(res->mutex, SAL_MUTEX_FOREVER) != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }

    hdl_slot = (res->array + (hdl * sizeof(uint8_t *)));

    /*
     * Verify that the current pointer points outside the array to avoid
     * double free and verify that the handler was indeed allocated.
     */
    ptr_val = *(uint8_t **)hdl_slot;
    if (((ptr_val >= res->array) &&
        (ptr_val < res->array + (sizeof(uint8_t *)  * res->array_size))) ||
        (hdl_slot == res->last_elem)) {
        sal_mutex_give(res->mutex);
        return SHR_E_PARAM;
    }

    /* Add the handle to the end of the list */
    if (res->last_elem) {
        *(void **)res->last_elem = hdl_slot;
    } else if (!res->free_list) {
        res->free_list = hdl_slot;
    }
    *(void **)hdl_slot = NULL;   /* Terminate the list */
    res->last_elem = hdl_slot;   /* Update the last element pointer */

    if (sal_mutex_give(res->mutex) != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }
    return SHR_E_NONE;
}

void *bcmlt_hdl_data_get(uint32_t hdl)
{
    uint8_t *hdl_slot;
    hdl_resource_t *res = &hdl_resource;
    uint8_t *ptr_val;

    hdl -= HANDLE_BASE;
    if (hdl > res->array_size) {
        return NULL;
    }

    /*
     * Since the entire array can be reallocated onto a new address we need
     * to make sure it doesn't happen while we obtaining a pointer to the
     * elements of this array.
     */
    if (sal_mutex_take(res->mutex, SAL_MUTEX_FOREVER) != SHR_E_NONE) {
        return NULL;
    }

    hdl_slot = (res->array + (hdl * sizeof(uint8_t *)));

    /*
     * Verify that the current pointer points outside the array to avoid
     * returning value that is invalid (from a slot that was freed). This
     * check is only done to protect the caller from getting invalid values.
     */
    ptr_val = *(uint8_t **)hdl_slot;
    if ((ptr_val >= res->array) &&
        (ptr_val < res->array + (sizeof(uint8_t *)  * res->array_size))) {
        sal_mutex_give(res->mutex);
        return NULL;
    }

    sal_mutex_give(res->mutex);
    return *(void **)hdl_slot;
}
