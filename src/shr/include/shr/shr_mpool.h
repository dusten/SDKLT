/*! \file shr_mpool.h
 *
 * Simple memory manager.
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

#ifndef SHR_MPOOL_H
#define SHR_MPOOL_H

#include <sal/sal_types.h>

/*! Opaque memory pool handle. */
struct shr_mpool_mem_s;

/*! Handle to be used for all operations on a memory pool. */
typedef struct shr_mpool_mem_s *shr_mpool_handle_t;

/*!
 * \brief Create and initialize mpool control structures.
 *
 * The mpool module is a very simple memory allocator, which can be
 * used to manage a fixed block of memory.
 *
 * The allocated memory will be aligned to a minimum chunk size, in
 * order to minimize fragmentation. No other measures are taken to
 * prevent fragmentation.
 *
 * Although memory can be allocated an freed dynamically, it is
 * recommended that a driver allocates as much memory as it needs
 * up-front. Such a strategy will ensure that any memory constraints
 * are detected during initialization instead of after hours/days of
 * operation.
 *
 * \param [in] base_address Pointer to mpool memory block.
 * \param [in] size Total size of mpool memory block.
 * \param [in] chunk_size Minimum allocation size (currently unsupported).
 *
 * \return mpool handle or NULL on error.
 */
extern shr_mpool_handle_t
shr_mpool_create(void* base_address, size_t size, size_t chunk_size);

/*!
 * \brief Free mpool control structures.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 *
 * \retval 0 No errors.
 */
extern int
shr_mpool_destroy(shr_mpool_handle_t pool);

/*!
 * \brief Allocate block of memory.
 *
 * The allocated memory is uninitialized.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 * \param [in] size Minimum size of memory to allocate.
 *
 * \return Pointer to allocated memory or NULL on error.
 */
extern void *
shr_mpool_alloc(shr_mpool_handle_t pool, size_t size);

/*!
 * \brief Free block of memory.
 *
 * Free memory block allocated with \ref shr_mpool_alloc.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 * \param [in] ptr Pointer to memory to free.
 */
extern void
shr_mpool_free(shr_mpool_handle_t pool, void* ptr);

/*!
 * \brief Return amount of memory currently allocated.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 *
 * \return Total amount of allocated memory.
 */
extern int
shr_mpool_usage(shr_mpool_handle_t pool);

/*!
 * \brief Return allocation unit size.
 *
 * This function is mainly for debug and test purposes.
 *
 * Each allocation will be made from chunks of the allocation unit
 * size, e.g. requesting just a single byte of memory will still
 * consume a full allocation unit.
 *
 * \param [in] pool mpool handle (from \ref shr_mpool_create).
 *
 * \return Allocation unit size.
 */
extern size_t
shr_mpool_chunk_size(shr_mpool_handle_t pool);

#endif  /* SHR_MPOOL_H */
