/*! \file shr_lmem_mgr.h
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

#ifndef SHR_LMEM_MGR_H
#define SHR_LMEM_MGR_H

#include <sal/sal_libc.h>

/*!
 * \brief Forward declaration of local memory handle
 */
typedef struct shr_lmm_hdl_s *shr_lmm_hdl_t;

/*!
 * \brief Initializes local memory instance.
 *
 * This macro initializes an instance of the local memory manager. It is
 * assume that each managed element data type contains a pointer field
 * named 'next'. The pointer field is of type (T *).
 *
 * \param [in] T is the type of the memory that should be managed
 * \param [out] hdl is the application handler that represent
 *              this instance of the local memory manager
 * \param [in] chunk_size is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represent the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in]  multi_thread is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise
 * \param [out] rv is the return value. 0 for success, otherwise failure
 *
 * \return void None.
 */
#define LMEM_MGR_INIT(T,hdl,chunk_size,multi_thread,rv)  \
{                                       \
    rv = shr_lmm_init(chunk_size,       \
            sizeof(T),                  \
            (uintptr_t) ((char *)&(((T *)(0))->next)), \
            multi_thread,               \
            &hdl);             \
}

/*!
 * \brief Initializes local memory instance with memory restriction.
 *
 * This macro initializes an instance of the local memory manager similar to
 * \ref LMEM_MGR_INIT() with an additional option that enables the caller to
 * place a limit on the number of elements that can be allocated.
 *
 * \param [in] T is the type of the memory that should be managed
 * \param [out] hdl is the application handler that represent
 *              this instance of the local memory manager
 * \param [in] chunk_size is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represent the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in]  multi_thread is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise
 * \param [in] max Is the maximal number of elements that can be allocated. This
 *              parameter determine that the maximal memory size that will be
 *              allocated for this element is the size of \c T * max.
 * \param [out] rv is the return value. 0 for success, otherwise failure
 *
 * \return void None.
 */
#define LMEM_MGR_REST_INIT(T,hdl,chunk_size,multi_thread,max,rv)  \
{                                         \
    T x##T;                               \
    rv = shr_lmm_restrict_init(chunk_size,\
            sizeof(T),                    \
            (uint32_t)((uint8_t *)&(x##T.next)-(uint8_t *)&x##T),\
            multi_thread,                 \
            max,                          \
            &hdl);                        \
}

/*!
 * \brief Initializes local memory instance.
 *
 * The application can call this function directly instead of using the
 * initialization macro. Use this function if the element data type doesn't
 * contains a pointer field named 'next'. Note, however, that the element
 * type must have a different field that is a pointer to the type of the
 * element. This is mandatory to enable link list of this elements.
 *
 * \param [in] chunk_size Is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represnt the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in] element_size Is the size of the basic element (in bytes).
 *              The application may use sizeof() to obtain this value.
 * \param [in] pointer_offset Is the offset (in bytes) of the link list
 *              pointer from the begining of the structure.
 * \param [in]  multi_thread Is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise.
 * \param [out] hdl Is the application handler that represent this instance
 *              of the local memory manager. The caller must use this handle
 *              with every call to allocate or free a memory element.
 *
 * \return 0 for success, -1 for failure.
 */
extern int
shr_lmm_init(size_t chunk_size,
             size_t element_size,
             uint32_t pointer_offset,
             bool multi_thread,
             shr_lmm_hdl_t *hdl);

/*!
 * \brief Initializes local memory instance with restricted number of elements.
 *
 * The application can call this function directly instead of using the
 * initialization macro. Use this function if the element data type doesn't
 * contains a pointer field named 'next'. Note, however, that the element
 * type must have a different field that is a pointer to the type of the
 * element. This is mandatory to enable link list of this elements.
 * This function different from the function \ref shr_lmm_init() as it allows
 * the caller to specify the maximal number of elements that can be allocated.
 * This enables the caller to control the amount of memory it is willing to
 * dedicate for this particular element.
 *
 * \param [in]  chunk_size Is the desired chunk size allocation. For
 *              applications that manages many elements this number
 *              should be larger. In reality, this number represnt the
 *              ratio between allocating individual elements or chunk of
 *              elements.
 * \param [in]  element_size Is the size of the basic element (in bytes)
 *              The application may use sizeof() to obtain this value.
 * \param [in]  pointer_offset Is the offset (in bytes) of the link list
 *              pointer from the begining of the structure.
 * \param [in]  multi_thread Is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise.
 * \param [in]  max_elements Indicates the maxmimal number of memory elements
 *              that can be allocated for this specific instance.
 * \param [out] hdl Is the application handler that represent this instance
 *              of the local memory manager. The caller must use this handle
 *              with every call to allocate or free a memory element.
 *
 * \return 0 for success, -1 for failure.
 */
extern int
shr_lmm_restrict_init(size_t chunk_size,
                      size_t element_size,
                      uint32_t pointer_offset,
                      bool multi_thread,
                      uint32_t max_elements,
                      shr_lmm_hdl_t *hdl);


/*!
 * \brief Deletes pre-allocated memory manager
 *
 * This function deletes an instance of the local memory manager.
 * As a result, all the resources allocated for this instance will
 * be freed.
 *
 * \param [in] hdl is the local memory handle
 *
 * \return none
 */
extern void
shr_lmm_delete(shr_lmm_hdl_t hdl);

/*!
 * \brief Allocate an element
 *
 * This function allocates a single element from the local memory pool.
 * If the memory pool is empty the function will allocate a new chunk
 * of elements (based on chunk_size provided by \ref shr_lmm_init()).
 *
 * \param [in] hdl is the local memory handle
 *
 * \return pointer to an element on success or NULL on failure.
 */
extern void
*shr_lmm_alloc(shr_lmm_hdl_t hdl);

/*!
 * \brief Free an element back to the free pool
 *
 * This function frees a single element to the local memory pool.
 *
 * \param [in] hdl is the local memory handle
 * \param [in] element is a pointer to the element to free
 *
 * \return pointer to an element on success or NULL on failure.
 */
extern void
shr_lmm_free(shr_lmm_hdl_t hdl, void *element);

#endif /* SHR_LMEM_MGR_H */
