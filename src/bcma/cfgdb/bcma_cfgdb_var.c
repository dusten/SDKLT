/*! \file bcma_cfgdb_var.c
 *
 * Configuration data manager for variables operation.
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

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cfgdb/bcma_cfgdb.h>

#include "cfgdb_internal.h"

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_wildcard_delete.
 */
struct trvs_wc_del {

    /*! Wildcard string to be compared with variable name */
    const char *wildcard_str;

    /*! Number of variables deleted */
    int del_num;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to delete variables
 * if variable name matches wildcard string.
 */
static int
var_wildcard_delete(void *cookie, const char *name, const char *value,
                    uint32_t *tag, int hash_idx)
{
    struct trvs_wc_del *wc_del = (struct trvs_wc_del *)cookie;
    const char *wc_str = wc_del->wildcard_str;

    COMPILER_REFERENCE(value);
    COMPILER_REFERENCE(tag);
    COMPILER_REFERENCE(hash_idx);

    if (bcma_cfgdb_wildcard_match(name, wc_str)) {
        wc_del->del_num++;
        return BCMA_CFGDB_VAR_TRAVERSE_DELETE;
    }

    return 0;
}

int
bcma_cfgdb_wildcard_match(const char *str, const char *wc_str)
{
    int match = TRUE;
    const char *ptr = NULL;
    char s, w;

    while (1) {
        s = *str;
        w = *wc_str;

        if (s == '\0') {
            /*
             * When target string reaches end, match is true only if wildcard
             * string reaches end too or meets '*'.
             */
            if ( w == '\0') {
                break;
            } else if (w == '*') {
                wc_str++;
                continue;
            }
            match = FALSE;
            break;
        } else {
            if (s != w) {
                if (w == '*') {
                    /* If '*' is met, locate ptr to the position after '*'. */
                    ptr = ++wc_str;
                    continue;
                } else if (ptr) {
                    /*
                     * Wildcard string is always compared starting from
                     * character after '*' if character is not matching.
                     */
                    wc_str = ptr;
                    w = *wc_str;

                    if (w == '\0') {
                        break;
                    } else if (s == w) {
                        wc_str++;
                    }
                    str++;
                    continue;
                } else {
                    /*
                     * If characters don't match and wildcard '*' is not
                     * appearing yet, it should be no matching.
                     */
                    match = FALSE;
                    break;
                }
            }
        }

        /* Move to next if strings match for non-wildcard characters */
        str++;
        wc_str++;
    }

    return match;
}

int
bcma_cfgdb_var_tag_flags_clear(void *cookie, const char *name,
                               const char *value,
                               uint32_t *tag, int hash_idx)
{
    struct trvs_tag_flag_op *tfop = (struct trvs_tag_flag_op *)cookie;
    const char *target_name = tfop->name;
    int flags = tfop->flags;

    COMPILER_REFERENCE(value);
    COMPILER_REFERENCE(hash_idx);

    /* Clear flags for all variables if target name is NULL. */
    if (target_name == NULL) {
        *tag &= ~(flags & BCMA_CFGDB_TAG_FLAG_MASK);
        return 0;
    }

    /* Stop traversing if the target variable flag has been cleared. */
    if (tfop->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(name, target_name) == 0) {
        *tag &= ~(flags & BCMA_CFGDB_TAG_FLAG_MASK);
        tfop->done = TRUE;
    }

    return 0;
}

int
bcma_cfgdb_var_tag_flags_set(void *cookie, const char *name, const char *value,
                             uint32_t *tag, int hash_idx)
{
    struct trvs_tag_flag_op *tfop = (struct trvs_tag_flag_op *)cookie;
    const char *target_name = tfop->name;
    int flags = tfop->flags;

    COMPILER_REFERENCE(value);
    COMPILER_REFERENCE(hash_idx);

    /* Set flags for all variables if target name is NULL. */
    if (target_name == NULL) {
        *tag |= (flags & BCMA_CFGDB_TAG_FLAG_MASK);
        return 0;
    }

    /* Stop traversing if the target variable flag has been set. */
    if (tfop->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(target_name, name) == 0) {
        *tag |= (flags & BCMA_CFGDB_TAG_FLAG_MASK);
        tfop->done = TRUE;
    }

    return 0;
}

int
bcma_cfgdb_var_set(const char *name, char *value)
{
    int rv = 0;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    if (value == NULL) {
        /* Delete variable if value is NULL. */
        rv = bcma_env_var_unset(cfgdb->env, name, FALSE, TRUE);

        /*
         * If no matching on delete variable, check if name contains
         * wildcard '*' and try deleting variables by wildcard comparison.
         */
        if (rv != 0) {
            if (sal_strchr(name, '*')) {
                struct trvs_wc_del wc_del;

                wc_del.wildcard_str = name;
                wc_del.del_num = 0;

                bcma_cfgdb_var_traverse(var_wildcard_delete, &wc_del);

                rv = wc_del.del_num ? 0 : -1;
            }
        }
    } else {
        /* Set variable is value is not NULL. */
        rv = bcma_env_var_set(cfgdb->env, name, value, FALSE);
    }

    return rv;
}

const char *
bcma_cfgdb_var_get(const char *name)
{
    const char * value;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return NULL;
    }

    value = bcma_env_var_get(cfgdb->env, name);

    if (value != NULL) {
        struct trvs_tag_flag_op tfop;

        tfop.name = name;
        tfop.flags = BCMA_CFGDB_TAG_FLAG_HIT;
        tfop.done = FALSE;

        /* Set HIT flag to the variable. */
        bcma_cfgdb_var_traverse(bcma_cfgdb_var_tag_flags_set, &tfop);
    }

    return value;
}

int
bcma_cfgdb_var_traverse(int (*cb)(void *cookie,
                                  const char *name, const char *value,
                                  uint32_t *tag, int hash_idx),
                        void *cookie)
{
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    return bcma_env_var_traverse(cfgdb->env, 0, cb, cookie);
}
