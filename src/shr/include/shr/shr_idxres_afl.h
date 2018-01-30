/*! \file shr_idxres_afl.h
 *
 * Module: Aligned Indexed resource management, using banked lists.
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

#ifndef SHR_IDXRES_AFL_H
#define SHR_IDXRES_AFL_H

#include <sal/sal_types.h>

/*! Convert bytes to bits. */
#define BYTES2BITS(x) ((x) * 8)

/*! Element managed by _aidxres_list_t. */
typedef uint32_t shr_aidxres_element_t;

struct shr_aidxres_list_s;

/*! Handle to a _aidxres_list_t. */
typedef struct shr_aidxres_list_s *shr_aidxres_list_handle_t;

/*!
 * \brief Create an aligned/contiguous_blocked banked free list.
 *
 * The validLow and validHigh values are used to specify the valid range
 * of entries for querying 'free/used' status of an entry; any value not
 * in this range is considered an invalid argument, but values that are
 * not between first and last will be permanently 'used' and not allowed
 * by the free operation nor ever provided by the allocate operation.
 * The blocking factor is the actual power of two that is to be used when
 * computing maximum block size.  Blocks will be able to be manipulated up
 * to 2^blocking_factor, but note that blocking_factor must be less than
 * or equal to the number of bits used for bank index (so in 8b mode, this
 * must be 7 or less, in 16b mode it must be 15 or less, and in 32b mode
 * it must be 31 or less).
 *
 * \param  [out] list Place to put list handle.
 * \param  [in] first Number of first entry to manage.
 * \param  [in] last Number of last entry to manage.
 * \param  [in] valid_low Low valid entry value.
 * \param  [in] valid_high High valid entry value.
 * \param  [in] block_factor Max block power of two.
 * \param  [in] name Name for the list (used for sal_alloc).
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY Unable to allocate memory.
 * \retval SHR_E_RESOURCE Unable to create lock.
 * \retval SHR_E_INTERNAL Unable obtain lock.
 */
extern int
shr_aidxres_list_create(shr_aidxres_list_handle_t *list,
                        shr_aidxres_element_t first,
                        shr_aidxres_element_t last,
                        shr_aidxres_element_t valid_low,
                        shr_aidxres_element_t valid_high,
                        shr_aidxres_element_t block_factor,
                        char *name);

/*!
 * \brief Destroy a list.
 *
 * This destroys the list, but does not claim the semaphore first, so the
 * caller must take care not to destroy the list while it's being used.
 * It is possible that some OSes will not permit the destruction of a lock
 * that is in use, so maybe that at least helps.  It is also willing to
 * destroy the list even if there are still allocated entries.
 *
 * \param  [in] list The list handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 */
extern int
shr_aidxres_list_destroy(shr_aidxres_list_handle_t list);

/*!
 * \brief Allocate the next available single element from a list.
 *
 *  As for the idxres list alloc call, this returns *one* element.
 *
 * \param [in] list List from which to allocate.
 * \param [out] element Where to put alloced elem num.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 */
extern int
shr_aidxres_list_alloc(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t *element);

/*!
 * \brief Allocate a set of the next available single elements from a list.
 *
 * This uses the same function as shr_idxres_list_alloc, except that it
 * verifies that there are enough elements free to fulfill the request
 * before it tries to allocate any of them.  It is still possible that an
 * error prevents completion, however, so if the result is not success,
 * the done value must be verified (and any elements that were done that
 * can not be used must be freed).
 * The set is NOT guaranteed to be contiguous.
 * The set consists of count *one* element blocks.
 *
 * \param [in] list List from which to allocate.
 * \param [in] count Number of elements to allocate.
 * \param [out] *elements Ptr to array for alloced elems.
 * \param [out] *done Ptr to number of successful allocs.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_RESOURCE Not enough free elements.
 */
extern int
shr_aidxres_list_alloc_set(shr_aidxres_list_handle_t list,
                           shr_aidxres_element_t count,
                           shr_aidxres_element_t *elements,
                           shr_aidxres_element_t *done);

/*!
 * \brief Allocate a block (of specified count) of elements from a list
 * The block is guaranteed to be aligned to the next power of two into
 * which it fits, and will be contiguous.
 *
 * \param [in] list List from which to allocate.
 * \param [in] count Size of block, in elements.
 * \param [out] element Where to put alloced elem num.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_RESOURCE No free blocks large enough.
 */
extern int
shr_aidxres_list_alloc_block(shr_aidxres_list_handle_t list,
                             shr_aidxres_element_t count,
                             shr_aidxres_element_t *element);

/*!
 * \brief Free an element or block of elements back to a list.
 *
 * Freeing an entry already in the list is checked, as well as freeing an
 * entry outside of the list-managed range.  Elements can be freed using
 * either free call, no matter which alloc call was used to obtain them.
 *
 * \param [in] list List from which elem was alloced.
 * \param [in] element Number to free.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_RESOURCE Tried to free invalid elements.
 */
extern int
shr_aidxres_list_free(shr_aidxres_list_handle_t list,
                      shr_aidxres_element_t element);

/*!
 * \brief Free a set of elements back to a list.
 *
 * This uses the same function as shr_idxres_list_free.  It is possible
 * that an error prevents completion, so if the result is not success, the
 * done value must be verified (and any elements that were not done that
 * can not be reused must still be freed).  Elements can be freed using
 * either free call, no matter which alloc method was used to obtain them.
 *
 * \param [in] list List to which to free.
 * \param [in] count Number of elements to free.
 * \param [in] elements Ptr to array for elems to free.
 * \param [out] done Ptr to number of successful frees.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_RESOURCE Tried to free invalid elements.
 */
extern int
shr_aidxres_list_free_set(shr_aidxres_list_handle_t list,
                          shr_aidxres_element_t count,
                          shr_aidxres_element_t *elements,
                          shr_aidxres_element_t *done);

/*!
 * \brief Get status of the list itself.
 *
 * If you don't want to fetch a specific attribute of the list, pass
 * NULL for the pointer to that attribute's location.
 * There is no set function for these items; most are set at creation of
 * list and the others are current state of list.
 * Largest free is the largest number of elements that a block can contain
 * and still have the alloc request fulfilled on this list.
 *
 * \param [in] list List to check.
 * \param [out] first Buffer for first value.
 * \param [out] last Buffer for last value.
 * \param [out] valid_low Buffer for valid_low value.
 * \param [out] valid_high Buffer for valid_high value.
 * \param [out] free_count Buffer for free_count value.
 * \param [out] alloc_count Buffer for alloc_count value.
 * \param [out] largest_free Buff for largest free value.
 * \param [out] block_factor Buffer for block factor val.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_RESOURCE Tried to free invalid elements.
 */
extern int
shr_aidxres_list_state(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t *first,
                       shr_aidxres_element_t *last,
                       shr_aidxres_element_t *valid_low,
                       shr_aidxres_element_t *valid_high,
                       shr_aidxres_element_t *free_count,
                       shr_aidxres_element_t *alloc_count,
                       shr_aidxres_element_t *largest_free,
                       shr_aidxres_element_t *block_factor);

/*!
 * \brief See if an element is currently in use.
 *
 * This function ALWAYS returns an error (never SHR_E_NONE).
 *
 * \param [in] list List to check.
 * \param [in] element Element number to check.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_EXISTS Element is in use.
 * \retval SHR_E_NOT_FOUND Element is not in use.
 */
extern int
shr_aidxres_list_elem_state(shr_aidxres_list_handle_t list,
                            shr_aidxres_element_t element);

/*!
 * \brief See if an element is currently in use.
 *
 * This function ALWAYS returns an error (never SHR_E_NONE).
 *
 * \param [in] list List to check.
 * \param [in] element Number to check.
 * \param [in] size Elements in expected block.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_EMPTY none of the elements are in use.
 * \retval SHR_E_FULL all of the elements are in use.
 * \retval SHR_E_CONFIG elements are in use but block(s) do not match.
 * \retval SHR_E_EXISTS some of the elements are in use but not all of them.
 * \retval SHR_E_CONFIG Block size error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_PARAM Invalid parameters.
 */
int
shr_aidxres_list_block_state(shr_aidxres_list_handle_t list,
                             shr_aidxres_element_t element,
                             shr_aidxres_element_t size);

/*!
 * \brief Reserve a range of elements in a list.
 *
 * This is truly an inefficient way to manage top and bottom reservations
 * unless they are not known at list creation time, as this does not do
 * anything to adjust the physical size of the list's workspace; it merely
 * takes the requested range out of the available elements.
 * Elements reserved in this manner can be returned using free; they are
 * allocated as elements instead of blocks.
 *
 * \param [in] list List handle.
 * \param [in] first First entry to reserve.
 * \param [in] last Last entry to reserve.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_RESOURCE Range is not allocatable.
 */
extern int
shr_aidxres_list_reserve(shr_aidxres_list_handle_t list,
                         shr_aidxres_element_t first,
                         shr_aidxres_element_t last);

/*!
 * \brief Reserve a block in a list.
 *
 * This is truly an inefficient way to manage top and bottom reservations
 * unless they are not known at list creation time, as this does not do
 * anything to adjust the physical size of the list's workspace; it merely
 * takes the requested range out of the available elements.
 * Elements reserved in this manner can be returned using free; they are
 * allocated a a single block.
 * The block to be reserved must satisfy all allocation rules for blocks
 * (for example, alignment and size) that apply to the list.
 *
 * \param [in] list List handle.
 * \param [in] first First element in block to reserve.
 * \param [in] count Number of elements in block.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Parameter validation error.
 * \retval SHR_E_INTERNAL Unable obtain or release lock.
 * \retval SHR_E_RESOURCE Range is not allocatable.
 */
extern int
shr_aidxres_list_reserve_block(shr_aidxres_list_handle_t list,
                               shr_aidxres_element_t first,
                               shr_aidxres_element_t count);

/*! This variable controls sanity checking behaviour, assuming the feature is
 * enabled in the idxres_afl.c file.
 * Note that CREATE only checks on RETURN; there is no check to perform for
 * CREATE on ENTRY.  There is no point in doing checks before DESTROY.
 */
extern uint32_t _aidxres_sanity_settings;

/*! Performs sanity checks on entry. */
#define AIDXRES_SANITY_POINT_ENTRY  0x00000001

/*! Performs sanity checks on return. */
#define AIDXRES_SANITY_POINT_RETURN 0x00000002

/*! Performs sanity checks on alloc. */
#define AIDXRES_SANITY_FUNC_ALLOC   0x00000010

/*! Performs sanity checks on free. */
#define AIDXRES_SANITY_FUNC_FREE    0x00000020

/*! Performs sanity checks on create. */
#define AIDXRES_SANITY_FUNC_CREATE  0x00000040

/*! Performs sanity checks on faults. */
#define AIDXRES_SANITY_DUMP_FAULTS  0x00010000

#endif /* SHR_IDXRES_AFL_H */
