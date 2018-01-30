/*! \file shr_hash_str.h
 *
 * hash table for strings.
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

#ifndef SHR_HASH_STR_H
#define SHR_HASH_STR_H

#include <shr/shr_types.h>

/*!
 * This is the header file of the hash dictionary API. None of the API
 * function is re-entrant. However, multiple hash tables can be manipulated
 * simultanously. It is only that multiple threads can not operate on the same
 * hash table simultanously. Taking this approach improves performance
 */

/*!
 * Maximal number of hash string elements
 */
#define BCM_SHASH_STR_MAX_ELEMENTS  0x80000     /* 0.5M*/

/*! Dictionary handle type. */
typedef struct shr_hash_str_t *shr_hash_str_hdl;

/*!
 * \brief Initialize hash string module.
 *
 * Call this function once upon initialization. Calling other API
 * functions prior to this will result in error.
 *
 * \return 0 on success otherwise failure to initialize
 */
extern int shr_hash_str_init(void);

/*!
 * \brief delete hash string module.
 *
 * Call this function once upon termination. Calling other functions
 * of this API after cleaning up the module may result in undesired
 * results
 *
 * \return none
 */
extern void shr_hash_str_delete(void);

/*!
 * \brief Allocate hash string dictionary.
 *
 * This API allocates resources for hash dictionary
 *
 * \param [in] num_of_elements dictionary size
 * \param [out] dict_hdl pointer where to place the dictionary handle
 *
 * \return 0 on success otherwise failure in allocating hash dictionary
 */
extern int shr_hash_str_dict_alloc(size_t num_of_elements,
                                   shr_hash_str_hdl *dict_hdl);

/*!
 * \brief create hash string dictionary.
 *
 * This API free all resources associated with the hash dictionary
 *
 * \param [in] dict_hdl the dictionary handle
 *
 * \return 0 on success otherwise failure in freeing hash dictionary
 */
extern int shr_hash_str_dict_free(shr_hash_str_hdl dict_hdl);

/*!
 * \brief Insert element in hash dictionary.
 *
 * This API insert an element into the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [in] key pointer to string used as key. The memory pointed by the
 *              key must remain under the control of the application as long
 *              as the entry is valid.
 * \param [in] value pointer to value.
 *
 * \return 0 success otherwise failure in finding an element
 */
extern int shr_hash_str_dict_insert(shr_hash_str_hdl dict_hdl,
                                    const char *key,
                                    const void *value);

/*!
 * \brief lookup element in hash dictionary.
 *
 * This API lookup for an element in the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [in] key pointer to string used as key. The memory pointed by the
 *              key must remain under the control of the application as long
 *              as the entry is valid.
 * \param [out] value is double pointer where the function will
 *              place the address of value
 *
 * \return 0 success otherwise failure in finding an element
 */
extern int shr_hash_str_dict_lookup(shr_hash_str_hdl dict_hdl,
                             const char *key,
                             void **value);

/*!
 * \brief delete element from hash dictionary.
 *
 * This API deletes an element from the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [in] key pointer to string used as key
 *
 * \return 0 on success, otherwise failure in deleting an element
 */
extern int shr_hash_str_dict_delete(shr_hash_str_hdl dict_hdl,
                 const char *key);


/*!
 * \brief start hash enumeration
 *
 * This API starts to enumerate all the entries in the hash.
 * It returns the first entry. To continue the enumeration
 * the application should call \ref shr_hash_str_get_next.
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [out] key is a double pointer where the function will
 *              place the address of the key
 * \param [out] value is double pointer where the function will
 *              place the address of value
 *
 * \return 0 on success, otherwise failure in finding the first element
 */
extern int shr_hash_str_get_first(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value);

/*!
 * \brief continue hash enumeration
 *
 * This API starts to enumerate all the entries in the hash.
 * It returns the first entry
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [out] key is a double pointer where the function will
 *              place the address of the key
 * \param [out] value is double pointer where the function will
 *              place the address of value
 *
 * \return 0 on success, otherwise failure in finding the next element
 */
extern int shr_hash_str_get_next(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value);
/*!
 * \brief Get statistics for hash dictionary.
 *
 * This API provides some statistics on the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [out] size pointer to the hash table size (can be NULL)
 * \param [out] element_count pointer to the number of elements in the
 *              hash table (can be NULL)
 * \param [out] empty_slots pointer to the number of empty slots in the
 *              hash table (can be NULL)
 *
 * \return 0 on success, otherwise failure in deleting an element
 */
extern int shr_hash_str_dict_info_get(shr_hash_str_hdl dict_hdl,
                 size_t *size,
                 size_t *element_count,
                 size_t *empty_slots);



#endif /* SHR_HASH_STR_H */
