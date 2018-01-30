/*! \file bcma_env.h
 *
 * CLI Environment
 *
 * Interface for managing CLI environment variables.
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

#ifndef BCMA_ENV_H
#define BCMA_ENV_H

#include <sal/sal_types.h>

/*!Return value from bcma_env_var_traverse call-back function to stop traversing */
#define BCMA_ENV_VAR_TRAVERSE_STOP   1

/*! Return value from bcma_env_var_traverse call-back function to delete variable */
#define BCMA_ENV_VAR_TRAVERSE_DELETE 2

/*! Opaque environment handle */
typedef void *bcma_env_handle_t;

/*!
 * \brief Allocate environment object.
 *
 * The env module supports hash table to speed up searches for variables.
 * The hash table size is (1 << 'hash_order'). No hashing is applied
 * if 'hash_order' is 0. Users can choose the hash order arbitrarily
 * depending on their applications. The larger the hash table size is,
 * the faster the search result might be retrieved. On the other hand,
 * the larger the hash table size is, the more memory spaces would be
 * consumed for hash table allocation.
 *
 * \param [in] hash_order Order of hash table size
 *
 * \return Opaque handle to environement object or NULL if error.
 */
extern bcma_env_handle_t
bcma_env_create(uint32_t hash_order);

/*!
 * \brief Free previously allocated environment object.
 *
 * This function will also free all resources associated with the
 * environment object.
 *
 * \param [in] eh Environment handle
 *
 * \retval 0 No errors
 * \retval -1 Not dynamically allocated.
 */
extern int
bcma_env_destroy(bcma_env_handle_t eh);

/*!
 * \brief Initialize environment object.
 *
 * \param [in] eh Environment handle
 * \param [in] hash_order Order of hash table size
 *
 * \retval 0 No errors
 * \retval -1 Bad environment or memory allocation error
 */
extern int
bcma_env_init(bcma_env_handle_t eh, uint32_t hash_order);

/*!
 * \brief Free resources of environment object.
 *
 * \param [in] eh Environment handle
 *
 * \retval 0 No errors
 * \retval -1 Bad environment
 */
extern int
bcma_env_cleanup(bcma_env_handle_t eh);

/*!
 * \brief Set an environment variable.
 *
 * Variables set in the local scope are hidden when the scope is
 * pushed, and they will be deleted when the scope is popped. Global
 * variables are visible in all scopes.
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [in] value Value of variable
 * \param [in] local Set variable in local scope (TRUE/FALSE)
 *
 * \retval 0 Variable was set successfully
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_set(bcma_env_handle_t eh, const char *name, char *value, int local);

/*!
 * \brief Set decimal environment variable.
 *
 * Convenience function to store integer as a decimal string. For
 * example, the value 12 will be stored as the string "12".
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [in] value Value of variable (as raw integer)
 * \param [in] local Set variable in local scope (TRUE/FALSE)
 *
 * \retval 0 Variable was set successfully
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_integer_set(bcma_env_handle_t eh, const char *name, int value, int local);

/*!
 * \brief Set hexadecimal environment variable.
 *
 * Convenience function to store integer as a hexadecimal string. For
 * example, the value 12 will be stored as the string "0xc".
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [in] value Value of variable (as raw integer)
 * \param [in] local Set variable in local scope (TRUE/FALSE)
 *
 * \retval 0 Variable was set successfully
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_hex_set(bcma_env_handle_t eh, const char *name, int value, int local);

/*!
 * \brief Unset an environment variable.
 *
 * Variables in the local scope will be searched first, and if not
 * found here, the list of global variables will be searched.
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [in] local Check against variables in local scope (TRUE/FALSE)
 * \param [in] global Check against global variables (TRUE/FALSE)
 *
 * \retval 0 Variable was unset successfully
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_unset(bcma_env_handle_t eh, const char *name, int local, int global);

/*!
 * \brief Get value of an environment variable.
 *
 * Variables in the local scope will be searched first, and if not
 * found here, the list of global variables will be searched.
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 *
 * \return Name of variable or NULL if the variable is not found
 */
extern const char *
bcma_env_var_get(bcma_env_handle_t eh, const char *name);

/*!
 * \brief Check if an environment variable exists.
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [in] local Check against variables in local scope (TRUE/FALSE)
 * \param [in] global Check against global variables (TRUE/FALSE)
 *
 * \retval TRUE Variable exists
 * \retval FALSE Variable does not exist
 */
extern int
bcma_env_var_exists(bcma_env_handle_t eh, const char *name, int local, int global);

/*!
 * \brief Traverse all variables in var list.
 *
 * The function traverses all variables in var list indicated by 'scope'.
 * Value 0 of 'scope' means to traverse global var list.
 * Value 1 of 'scope' means to traverse current local scope var list.
 * Next local scope var list would be indicated by 'scope'+1 starting from 1.
 *
 * The function would call call-back function 'cb' for all variables.
 * The traverse function would pass variable name, value, tag pointer and
 * hash index to call-back function 'cb'. If no hashing is applied, the
 * 'hash_idx' would be passed by -1.
 *
 * The call-back function 'cb' can control the traverse function by return value.
 * If \ref BCMA_ENV_VAR_TRAVERSE_STOP is returned from 'cb',
 * bcma_env_var_traverse function will be stopped immediately.
 * If \ref BCMA_ENV_VAR_TRAVERSE_DELETE is returned
 * from 'cb', current traversing variable will be deleted then.
 * Negtive value should be returned from call-back functioin 'cb' on error
 * and this would also stop traversing immediately. Otherwise 0 should be
 * returned from call-back function 'cb'.
 *
 * \param [in] eh Environment handle
 * \param [in] scope Scope value to indicate the traverse var list
 * \param [in] cb Traverse call-back function
 * \param [in] cookie Information to be passed to call-back function
 *
 * \retval Last return value from call-back function 'cb'
 */
extern int
bcma_env_var_traverse(bcma_env_handle_t eh, int scope,
                      int (*cb)(void *cookie,
                                const char *name, const char *value,
                                uint32_t *tag, int hash_idx),
                      void *cookie);

/*!
 * \brief Set an environment variable tag.
 *
 * Variables set in the local scope are hidden when the scope is
 * pushed, and they will be deleted when the scope is popped. Global
 * variables are visible in all scopes.
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [in] tag Tag of variable
 * \param [in] local Set variable in local scope (TRUE/FALSE)
 *
 * \retval 0 Variable tag was set successfully
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_tag_set(bcma_env_handle_t eh, const char *name, uint32_t tag, int local);

/*!
 * \brief Get tag of an environment variable.
 *
 * Variables in the local scope will be searched first, and if not
 * found here, the list of global variables will be searched.
 *
 * Variable tag will be updated in content of pointer 'tag'.
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [in,out] tag Pointer of tag
 *
 * \retval 0 Variable tag was get successfully
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_tag_get(bcma_env_handle_t eh, const char *name, uint32_t *tag);

/*!
 * \brief Push scope for local environment variables.
 *
 * Global environment variables are unaffected by a change of scope.
 * Current local variables are hidden when the scope is pushed.
 *
 * \param [in] eh Environment handle
 *
 * \retval 0 No errors
 * \retval -1 Bad environment or memory allocation error
 */
extern int
bcma_env_scope_push(bcma_env_handle_t eh);

/*!
 * \brief Pop scope for local environment variables.
 *
 * Global environment variables are unaffected by a change of scope.
 * Current local variables are deleted when the scope is popped.
 *
 * \param [in] eh Environment handle
 *
 * \retval 0 No errors
 * \retval -1 Bad environment or no more scopes
 */
extern int
bcma_env_scope_pop(bcma_env_handle_t eh);

/*!
 * \brief Number of pushed scopes.
 *
 * \param [in] eh Environment handle
 *
 * \retval Number of pushed scopes
 * \retval -1 Bad environment
 */
extern int
bcma_env_num_scopes_get(bcma_env_handle_t eh);

#endif /* BCMA_ENV_H */
