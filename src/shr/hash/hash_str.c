/*! \file hash_str.c
 *
 * <description> This file is the implementation of a hash table that handle
 * string keys.
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
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_hash_str.h>

#define ALLOCATE_ELEMENT_CHUNK 128
#define HASH_MODULE_SIGNATURE   0xe73ca69b
#define MIN_TABLE_SIZE      5

typedef struct _element_s {
    struct _element_s *next;
    const char  *key;
    const void  *val;
} _element_t;

typedef struct shr_hash_str_s {
    uint32_t signature;
    size_t size;
    size_t element_count;
    size_t free_slots;
    _element_t **table;
    uint32_t last_search_slot;
    _element_t *last_search_element;
} shr_hash_str_t;

static shr_lmm_hdl_t local_element_hdl;

static bool local_hash_str_init = false;

size_t shr_hash_str_calc_tbl_size(size_t num_of_elements);

int shr_hash_str_init()
{
    int rv;
    if (local_hash_str_init) {
        return 0;
    }
    LMEM_MGR_INIT(_element_t,
                  local_element_hdl,
                  ALLOCATE_ELEMENT_CHUNK,
                  true,
                  rv);
    if (0 != rv) {
        return -1;
    }
    local_hash_str_init = true;
    return 0;
}

void shr_hash_str_delete()
{
    if (!local_hash_str_init) {
        return;
    }
    shr_lmm_delete(local_element_hdl);
    local_hash_str_init = false;
}

int shr_hash_str_dict_alloc(size_t num_of_elements,
                                   shr_hash_str_hdl *dict_hdl)
{
    shr_hash_str_t *dict;
    size_t alloc_size;

    if (num_of_elements > BCM_SHASH_STR_MAX_ELEMENTS) {
        return SHR_E_PARAM;
    }
    if (!dict_hdl || !local_hash_str_init) {
        return SHR_E_PARAM;
    }
    num_of_elements = shr_hash_str_calc_tbl_size(num_of_elements);

    dict = (shr_hash_str_t *)sal_alloc(sizeof(shr_hash_str_t), "shrHashDict");
    if (!dict) {
        return SHR_E_MEMORY;
    }
    alloc_size = sizeof(_element_t *) * num_of_elements;
    dict->table = (_element_t **)sal_alloc(alloc_size, "shrHashTbl");
    if (!dict->table) {
        sal_free ((void *)dict);
        return SHR_E_MEMORY;
    }
    /* reset the table to 0 */
    sal_memset(dict->table, 0, alloc_size);
    dict->element_count = 0;
    dict->size = num_of_elements;
    dict->free_slots = num_of_elements;
    dict->signature = HASH_MODULE_SIGNATURE;
    *dict_hdl = (shr_hash_str_hdl)dict;
    return SHR_E_NONE;
}


int shr_hash_str_dict_free(shr_hash_str_hdl dict_hdl)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    size_t j;
    _element_t *p_element;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    /* free all the dictionary elements */
    for (j = 0; (j < dict->size) && (dict->element_count > 0); j++) {
        if (dict->table[j]) {
            do {
                p_element = dict->table[j];
                dict->table[j] = p_element->next;
                shr_lmm_free(local_element_hdl, (void *)p_element);
                dict->element_count--;
            } while (dict->table[j]);
        }
    }

    sal_free(dict->table);
    sal_free ((void *)dict);
    return SHR_E_NONE;
}

static size_t hash_idx (const char *key, size_t modulo)
{
#define MULTIPLIER (101)
    const unsigned char *p_key = (unsigned char *)key;
    uint64_t hash = 0;

    while (*p_key != 0) {
        hash = hash * MULTIPLIER + (*p_key);
        p_key++;
    }
    return hash % modulo;
}

int shr_hash_str_dict_insert(shr_hash_str_hdl dict_hdl,
                             const char *key,
                             const void *value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    _element_t *p_element;
    size_t index;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    index = hash_idx(key, dict->size);
    /*
    * Verify that the element is not already there
    */
    p_element = dict->table[index];
    while (p_element) {
        if (sal_strcmp(p_element->key, key) == 0) {
            return SHR_E_EXISTS;
        }
        p_element = p_element->next;
    }

    p_element = (_element_t *)shr_lmm_alloc(local_element_hdl);
    if (!p_element) {
        return SHR_E_MEMORY;
    }
    p_element->key = key;
    p_element->val = value;
    p_element->next = dict->table[index];
    if (!dict->table[index]) {
        dict->free_slots--;
    }
    dict->table[index] = p_element;
    dict->element_count++;
    return SHR_E_NONE;
}

int shr_hash_str_dict_lookup(shr_hash_str_hdl dict_hdl,
                 const char *key,
                 void **value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    _element_t *p_element;
    size_t index;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    index = hash_idx(key, dict->size);
    p_element = dict->table[index];
    while (p_element) {
        if (sal_strcmp(p_element->key, key) == 0) {
            break;
        }
        p_element = p_element->next;
    }
    if (p_element) {
        *value = (void *)p_element->val;
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

int shr_hash_str_dict_delete(shr_hash_str_hdl dict_hdl,
                 const char *key)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    _element_t *p_element;
    _element_t *p_tmp_element = NULL;
    size_t index;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    index = hash_idx(key, dict->size);
    p_element = dict->table[index];
    if (!p_element) {
        return SHR_E_NOT_FOUND;
    }
    if (sal_strcmp(p_element->key, key) == 0) {
        dict->table[index] = p_element->next;
        p_tmp_element = p_element;
    }
    else {
        while (p_element->next) {
            if (sal_strcmp(p_element->next->key, key) == 0) {
                p_tmp_element = p_element->next;
                p_element->next = p_tmp_element->next;
            }
        }
    }
    if (!p_tmp_element) {
        return SHR_E_NOT_FOUND;
    }
    shr_lmm_free(local_element_hdl, (void *)p_tmp_element);

    /* update statistics */
    if (!dict->table[index]) {
        dict->free_slots++;
    }
    dict->element_count--;
    return SHR_E_NONE;
}

int shr_hash_str_get_first(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return -1;
    }
    dict->last_search_element = NULL;
    dict->last_search_slot = 0;
    while (dict->last_search_slot < dict->size) {
        if (dict->table[dict->last_search_slot]) {
            dict->last_search_element = dict->table[dict->last_search_slot];
            break;
        }
        dict->last_search_slot++;
    }
    if (dict->last_search_element) {
        if (key) {
            *key = (char *)dict->last_search_element->key;
        }
        if (value) {
            *value = (void *)dict->last_search_element->val;
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

int shr_hash_str_get_next(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    bool need_increment = false;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return -1;
    }
    if (dict->last_search_element) {
        dict->last_search_element = dict->last_search_element->next;
        need_increment = true;
    }
    if (!dict->last_search_element) {
        if (need_increment) {
            dict->last_search_slot++;
        }
        while (dict->last_search_slot < dict->size) {
            if (dict->table[dict->last_search_slot]) {
                dict->last_search_element = dict->table[dict->last_search_slot];
                break;
            }
            dict->last_search_slot++;
        }
    }

    if (dict->last_search_element) {
        if (key) {
            *key = (char *)dict->last_search_element->key;
        }
        if (value) {
            *value = (void *)dict->last_search_element->val;
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}


int shr_hash_str_dict_info_get(shr_hash_str_hdl dict_hdl,
                 size_t *size,
                 size_t *element_count,
                 size_t *empty_slots)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    if (size) {
        *size = dict->size;
    }
    if (element_count) {
        *element_count = dict->element_count;
    }
    if (empty_slots) {
        *empty_slots = dict->free_slots;
    }
    return SHR_E_NONE;
}


/*
 * Utility function to do modular exponentiation.
 * It returns (x^y) % p
 */
static uint32_t power(uint32_t x, uint32_t y, uint32_t p)
{
    uint32_t res = 1;      /* Initialize result */
    x = x % p;  /* Update x if it is more than or equal to p */
    while (y > 0)
    {
        /* If y is odd, multiply x with result */
        if (y & 1) {
            res = (res*x) % p;
        }

        /* y must be even now */
        y = y>>1; /* y = y/2 */
        x = (x*x) % p;
    }
    return res;
}

/*
 * This function is called for all k trials. It returns
 * false if n is composite and returns true if n is
 * probably prime.
 * d is an odd number such that  d*2^r = n-1
 * for some r >= 1
 */
static bool miiller_test(uint32_t d, uint32_t n)
{
    /*
     * Pick a random number in [2..n-2]
     * Corner cases make sure that n > 4
     */
    uint32_t a = 2 + sal_rand() % (n - 4);

    /* Compute a^d % n */
    uint32_t x = power(a, d, n);

    if ((x == 1)  || (x == n-1)) {
       return true;
    }

    /*
     * Keep squaring x while one of the following doesn't happen
     * (i)   d does not reach n-1
     * (ii)  (x^2) % n is not 1
     * (iii) (x^2) % n is not n-1
     */
    while (d != n-1)
    {
        x = (x * x) % n;
        d *= 2;

        if (x == 1) {
            return false;
        }
        if (x == n-1) {
            return true;
        }
    }

    /* Return composite */
    return false;
}

/*
 * It returns false if n is composite and returns true if n
 * is probably prime.  k is an input parameter that determines
 * accuracy level. Higher value of k indicates more accuracy.
 */
static bool is_prime(uint32_t n, uint32_t k)
{
    uint32_t i;
    /* Find r such that n = 2^d * r + 1 for some r >= 1 */
    uint32_t d = n - 1;
    while (!(d & 0x1)) {
        d = d >> 1;
    }

    /* Iterate given number of 'k' times */
    for (i = 0; i < k; i++) {
         if (miiller_test(d, n) == false) {
              return false;
         }
    }

    return true;
}

#define NUM_OF_ITERATIONS 4

size_t shr_hash_str_calc_tbl_size(size_t num_of_elements)
{
    size_t rv;
    if (num_of_elements <= MIN_TABLE_SIZE) {
        return MIN_TABLE_SIZE;
    }

    for (rv = num_of_elements; rv < 2 * num_of_elements; rv++) {
        if (is_prime((uint32_t)rv, NUM_OF_ITERATIONS)) {
            return rv;
        }
    }
    return num_of_elements;
}
