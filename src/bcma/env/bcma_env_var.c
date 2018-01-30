/*! \file bcma_env_var.c
 *
 * CLI Environment
 *
 * Manage environment variables.
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

#include <bcma/env/bcma_env.h>

#include "env_internal.h"

/*
 * Speed up searches by first checking if a hash matches.
 * It encodes the first to last character, as well as string length.
 */
static inline uint32_t
var_hash(const char *name, uint32_t hash_order)
{
    uint32_t hash, idx;

    /* No hashing */
    if (hash_order == 0) {
        return 0;
    }

    idx = 0;
    hash = 5381;

    while (name[idx] != 0) {
        hash = ((hash << 5) + hash) + name[idx];
        idx++;
    }
    hash = ((hash << 5) + hash) + idx;

    return (hash & ((1 << hash_order) - 1));
}

/*
 * Find variable in linked list.
 */
static env_var_t *
var_find_list(env_var_list_t *vl, const char *name)
{
    env_var_t *ev;
    uint32_t hash = var_hash(name, vl->hash_order);
    env_var_t *htab = &vl->htab[hash];

    for (ev = htab->next; ev != htab; ev = ev->next) {
        if (sal_strcmp(ev->name, name) == 0) {
            return ev;
        }
    }

    return NULL;
}

/*
 * Find local/global variable in current scope.
 */
static env_var_t *
var_find(env_t *env, const char *name, int local, int global)
{
    env_var_t *ev = NULL;

    if (local && env->var_scope_list) {
        ev = var_find_list(&env->var_scope_list->local, name);
    }
    if (global && ev == NULL) {
        ev = var_find_list(&env->var_global_list, name);
    }
    return ev;
}

/*
 * Remove variable from linked list and free associate memory.
 */
static void
var_delete(env_t *env, env_var_t *ev)
{
    ev->prev->next = ev->next;
    ev->next->prev = ev->prev;

    sal_free(ev->name);
    sal_free(ev->value);
    sal_free(ev);
}

/*
 * Allocate memory for and insert variable in linked list.
 */
static int
var_set_list(env_t *env, env_var_list_t *vl, const char *name, char *val)
{
    env_var_t *ev;
    char *v_value = sal_strdup(val);

    if (v_value == NULL) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    if ((ev = var_find_list(vl, name)) != NULL) {
        /* If already exists on list, replace value */
        sal_free(ev->value);
        ev->value = v_value;
        sal_spinlock_unlock(env->lock);
    } else {
        uint32_t hash;
        env_var_t *htab;

        sal_spinlock_unlock(env->lock);

        /* Otherwise, create a new entry and link it on the list */
        ev = (env_var_t *)sal_alloc(sizeof(*ev), "bcmaEnvDiagVar");
        if (ev == NULL) {
            sal_free(v_value);
            return -1;
        }
        sal_memset(ev, 0, sizeof(*ev));
        ev->name = sal_strdup(name);
        if (ev->name == NULL) {
            sal_free(ev);
            sal_free(v_value);
            return -1;
        }
        ev->value = v_value;

        hash = var_hash(name, vl->hash_order);
        htab = &vl->htab[hash];
        sal_spinlock_lock(env->lock);
        ev->prev  = htab->prev;
        ev->next  = htab;
        ev->prev->next = ev;
        ev->next->prev = ev;
        sal_spinlock_unlock(env->lock);
    }
    return 0;
}

int
bcma_env_var_set(bcma_env_handle_t eh, const char *name, char *value, int local)
{
    env_t *env = (env_t *)eh;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    if (local && env->var_scope_list) {
        return var_set_list(env, &env->var_scope_list->local, name, value);
    }
    return var_set_list(env, &env->var_global_list, name, value);
}

int
bcma_env_var_integer_set(bcma_env_handle_t eh, const char *name, int value, int local)
{
    char v_str[12];

    sal_snprintf(v_str, sizeof(v_str), "%d", value);
    return bcma_env_var_set(eh, name, v_str, local);
}

int
bcma_env_var_hex_set(bcma_env_handle_t eh, const char *name, int value, int local)
{
    char v_str[12];

    sal_snprintf(v_str, sizeof(v_str), "0x%04x", value);
    return bcma_env_var_set(eh, name, v_str, local);
}

int
bcma_env_var_unset(bcma_env_handle_t eh, const char *name, int local, int global)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, local, global);
    if (ev == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    var_delete(env, ev);

    sal_spinlock_unlock(env->lock);

    return 0;
}

const char *
bcma_env_var_get(bcma_env_handle_t eh, const char *name)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;
    const char * val = NULL;

    if (env == NULL || BAD_ENV(env)) {
        return NULL;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, TRUE, TRUE);
    if (ev) {
        val = ev->value;
    }

    sal_spinlock_unlock(env->lock);

    return val;
}

int
bcma_env_var_exists(bcma_env_handle_t eh, const char *name, int local, int global)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return FALSE;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, local, global);

    sal_spinlock_unlock(env->lock);

    return (ev) ? TRUE : FALSE;
}

int
bcma_env_var_traverse(bcma_env_handle_t eh, int scope,
                      int (*cb)(void *cookie, const char *name, const char *value,
                                uint32_t *tag, int hash_idx),
                      void *cookie)
{
    env_t *env = (env_t *)eh;
    env_var_list_t *vl = NULL;
    env_var_t *ev, *pev;
    env_var_scope_t *vs;
    int idx, htab_size, rv = 0;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    if (scope == 0) {
        vl = &env->var_global_list;
    } else {
        /* Find matching local scope */
        for (vs = env->var_scope_list, idx = 1; vs; vs = vs->next, idx++) {
            if (scope == idx) {
                vl = &vs->local;
                break;
            }
        }
    }

    if (vl == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    htab_size = 1 << vl->hash_order;

    /* Invoke call-back function for all variables information in selected scope */
    for (idx = 0; idx < htab_size; idx++) {
        env_var_t *htab = &vl->htab[idx];
        for (ev = htab->next; ev != htab; ev = ev->next) {
            rv = cb(cookie, ev->name, ev->value, &ev->tag,
                    (vl->hash_order == 0) ? -1 : idx);
            if (rv < 0 || rv == BCMA_ENV_VAR_TRAVERSE_STOP) {
                /* Stop traversing on error or stop control. */
                break;
            } else if (rv == BCMA_ENV_VAR_TRAVERSE_DELETE) {
                /* Delete variable */
                pev = ev->prev;
                var_delete(env, ev);
                ev = pev;
            }
        }
        if (rv < 0 || rv == BCMA_ENV_VAR_TRAVERSE_STOP) {
            break;
        }
    }

    sal_spinlock_unlock(env->lock);

    return rv;
}

int
bcma_env_var_tag_set(bcma_env_handle_t eh, const char *name, uint32_t tag, int local)
{
    env_t *env = (env_t *)eh;
    env_var_list_t *vl;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    if (local && env->var_scope_list) {
        vl = &env->var_scope_list->local;
    } else {
        vl = &env->var_global_list;
    }

    ev = var_find_list(vl, name);

    if (ev == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    ev->tag = tag;

    sal_spinlock_unlock(env->lock);

    return 0;
}

int
bcma_env_var_tag_get(bcma_env_handle_t eh, const char *name, uint32_t *tag)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, TRUE, TRUE);

    if (ev == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    if (tag) {
        *tag = ev->tag;
    }

    sal_spinlock_unlock(env->lock);

    return 0;
}
