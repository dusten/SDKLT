/*! \file env_internal.h
 *
 * CLI Environment
 *
 * Environment definitions intended for internal use only.
 *
 * Application code should normally not need to include this file.
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

#ifndef ENV_INTERNAL_H
#define ENV_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_internal.h>
#include <sal/sal_spinlock.h>

/*! Signature for sanity checks. */
#define ENV_SIGNATURE   SAL_SIG_DEF('E', 'N', 'V', '>')

/*! Check that this is a valid environment object. */
#define BAD_ENV(_env)   ((_env)->env_sig != ENV_SIGNATURE)

/*! Check that this environment object was dynamically allocated. */
#define BAD_OBJ(_env)   ((_env)->dyn_sig != ENV_SIGNATURE)

/*!
 * \brief Environment variable.
 *
 * Simple double-linked list of variables consisting of a name and
 * value pair, as well as tag information.
 */
typedef struct env_var_s {

    /*! Next variable in linked list. */
    struct env_var_s *next;

    /*! Previous variable in linked list. */
    struct env_var_s *prev;

    /*! Variable name. */
    char *name;

    /*! Variable value. */
    char *value;

    /*! Variable tag (application-specific). */
    uint32_t tag;

} env_var_t;

/*!
 * \brief Environment variable list.
 *
 * Hash table is introduced to speed up the search function
 * in variable list.
 */
typedef struct env_var_list_s {

    /*! Hash table for variable list. */
    env_var_t *htab;

    /*! Hash table size order. */
    uint32_t hash_order;

} env_var_list_t;

/*!
 * \brief Environment scope.
 *
 * The environment support multiple local variable scopes.  Local
 * variables are only visible within the scope where they are defined,
 * whereas global variables are visibile everywhere.
 *
 * Typically a new local variable scope is created whenever a new
 * shell script is loaded, as this allows each script to use the same
 * local variable names without side-effects, when one script launches
 * another script.
 */
typedef struct env_var_scope_s {

    /*! Next scope in list. */
    struct env_var_scope_s *next;

    /*! Local variable list for this scope. */
    env_var_list_t local;

} env_var_scope_t;

/*!
 * \brief Environment object.
 *
 * All data for the environment is containe within this object.
 *
 * The signatures in the structure are used for sanity checks.
 */
typedef struct env_s {

    /*! Signature which indicates that object is initialized. */
    unsigned int env_sig;

    /*! Signature for dynamically allocated objects. */
    unsigned int dyn_sig;

    /*! Root of local variable scopes. */
    env_var_scope_t *var_scope_list;

    /*! Lock for list operations. */
    sal_spinlock_t lock;

    /*! Global variable list. */
    env_var_list_t var_global_list;

    /*! Hash table size order. */
    uint32_t hash_order;

} env_t;

/*!
 * \brief Initialize variable list.
 *
 * For internal use only.
 *
 * \param[in] vl Variable list for initialization
 * \param[in] hash_order Hash table size order for initialization
 *
 * \retval 0 No errors
 * \retval -1 resource allocation error
 */
extern int
bcma_env_var_list_init(env_var_list_t *vl, uint32_t hash_order);

/*!
 * \brief Free resources of variable list.
 *
 * For internal use only.
 *
 * \param[in] vl Variable list to free
 *
 * \retval 0 No errors
 * \retval -1 No resource to be freed in varaible list
 */
extern int
bcma_env_var_list_cleanup(env_var_list_t *vl);

#endif /* ENV_INTERNAL_H */
