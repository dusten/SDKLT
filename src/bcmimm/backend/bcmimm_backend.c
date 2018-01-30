/*! \file bcmimm_backend.c
 *
 * In-Memory back-end implementation.
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
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_alloc.h>
#include <sal/sal_rwlock.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <shr/shr_ha.h>
#include <shr/shr_hash_alg.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/back_e_ha.h>
#include "be_internals.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_BACKEND

/* The first sub component to use. Do not use 0 since this concidered NULL */
#define MIN_SUB_COMP        1

#define ROUND_TO_WORD(x) (((x) + 3) / 4) * 4

#define VALIDATE_INPUT(t_info, hdl) \
    if ((hdl >> 16) >= BCMDRD_CONFIG_MAX_UNITS) {                   \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                           \
    }                                                               \
    if (((hdl & 0xFFFF) >= active_tables[hdl >> 16].array_size) || \
        !t_info || !t_info->tbl ||                                  \
        (t_info->tbl->signature != BCMIMM_BLK_SIGNATURE)) {         \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                           \
    }

#define DATA_TABLE_CTRL_CHUNK   10

/*
 * per unit we keep all the tables in an array that points to them. The table
 * handle contains the index in this array. This allows us to quickly find
 * the relevant table.
*/
typedef struct {
    uint32_t array_size;
    uint32_t active_tbls;
    bcmimm_tbl_info_t *tbls;
} tables_array_t;

uint8_t bcmimm_be_comp_id;

static tables_array_t active_tables[BCMDRD_CONFIG_MAX_UNITS];

uint8_t bcmimm_available_subs[BCMDRD_CONFIG_MAX_UNITS];

bcmimm_free_list_t *bcmimm_free_elem[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocate active table control structure.
 *
 * This function allocates (or extends) the array size that points to the
 * currently active tables.
 *
 * \param [in] unit This is the unit associated with this table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int active_table_alloc(int unit)
{
    tables_array_t *act_tbl = &active_tables[unit];
    uint32_t new_size;
    uint32_t curr_size = act_tbl->array_size * sizeof(bcmimm_tbl_info_t);

    if (act_tbl->active_tbls == act_tbl->array_size) {
        new_size = curr_size +
            DATA_TABLE_CTRL_CHUNK * sizeof(bcmimm_tbl_info_t);

        act_tbl->tbls = shr_util_realloc(act_tbl->tbls,
                                         curr_size,
                                         new_size,
                                         "bcmimmBeTblInf");
        if (act_tbl->tbls == NULL) {
            return SHR_E_MEMORY;
        }
        sal_memset(((uint8_t *)act_tbl->tbls) + curr_size,
                   0,
                   new_size - curr_size);
        act_tbl->array_size += DATA_TABLE_CTRL_CHUNK;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Indicator if a specific table already exist in the active tables.
 *
 * This function searches the active tables of a given unit for a table that
 * matches the module and sub-module IDs.
 *
 * \param [in] unit This is the unit associated with this table.
 * \param [in] comp_id Is the component ID to match.
 * \param [in] sub_id Is the sub-component ID to match.
 *
 * \return True if the table exist and false otherwise.
 */
static bool table_exist(int unit, uint8_t comp_id, uint8_t sub_id)
{
    tables_array_t *act_tbl = &active_tables[unit];
    uint32_t j;
    tbl_header_t *tbl;

    for (j = 0; j < act_tbl->active_tbls; j++) {
        tbl = act_tbl->tbls[j].tbl;
        if (tbl && (tbl->comp_id == comp_id) && (tbl->sub_id == sub_id)){
            return true;
        }
    }
    return false;
}

/*******************************************************************************
 * Public functions
 */
int bcmimm_be_init(int unit, uint8_t comp_id, bool warm)
{

    SHR_FUNC_ENTER(unit);

    bcmimm_available_subs[unit] = MIN_SUB_COMP;
    bcmimm_be_comp_id = comp_id;
    bcmimm_free_lists_retrieve(unit, MIN_SUB_COMP);

    SHR_FUNC_EXIT();
}

int bcmimm_be_cleanup(int unit)
{
    tables_array_t *act_tbl = &active_tables[unit];
    bcmimm_free_list_t *free_list;
    bcmimm_free_list_t *tmp;
    uint32_t j;

    SHR_FUNC_ENTER(unit);

    /* Find if all the tables has been destroyed */
    for (j = 0; j < act_tbl->active_tbls; j++) {
        if (act_tbl->tbls[j].tbl) {
            SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
        }
    }

    if (act_tbl->tbls) {
        SHR_FREE(act_tbl->tbls);
        act_tbl->active_tbls = 0;
        act_tbl->array_size = 0;
    }

    bcmimm_available_subs[unit] = MIN_SUB_COMP;  /* Reset the sub component */
    free_list = bcmimm_free_elem[unit]; /* Clear the free field list */
    bcmimm_free_elem[unit] = NULL;

    while (free_list) {
        sal_mutex_destroy(free_list->lock);
        tmp = free_list;
        free_list = free_list->next;
        sal_free(tmp);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_create(int unit,
                           uint8_t comp_id,
                           uint8_t sub_id,
                           size_t key_size,
                           size_t data_size,
                           size_t rows,
                           bool warm,
                           bcmimm_be_tbl_hdl_t *hdl)
{
    uint32_t entry_len;
    uint32_t len;
    tbl_header_t *tbl;
    tables_array_t *act_tbl = &active_tables[unit];
    bcmimm_tbl_info_t *t_info;
    uint32_t idx;
    uint32_t field_len;

    SHR_FUNC_ENTER(unit);
    /* Verify that the same table was not already been created */
    if (table_exist(unit, comp_id, sub_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }
    entry_len = ROUND_TO_WORD(key_size +
                              (BCMIMM_ELEM_PTR_SIZE * 2) +
                              BCMIMM_ELEM_REF_CNT_SIZE);
    len = sizeof(tbl_header_t) + entry_len * rows;

    SHR_IF_ERR_EXIT(active_table_alloc(unit));

    for (idx = 0; idx < act_tbl->active_tbls; idx++) {
        if (!act_tbl->tbls[idx].tbl) {
            break;
        }
    }
    if (idx == act_tbl->active_tbls) {
        idx = act_tbl->active_tbls++;
    }
    t_info = &act_tbl->tbls[idx];

    /* Set the entry free list */
    t_info->entry_free_list = bcmimm_free_list_find(unit,
                                             entry_len,
                                             &bcmimm_free_elem[unit]);
    SHR_NULL_CHECK(t_info->entry_free_list, SHR_E_MEMORY);

    /* Field length contains the data size + field ID + element pointer (HA) */
    field_len = ROUND_TO_WORD(data_size +
                              BCMIMM_ELEM_PTR_SIZE +
                              BCMIMM_BE_FID_SIZE);

    /* Set the field free list */
    t_info->field_free_list = bcmimm_free_list_find(unit,
                                             field_len,
                                             &bcmimm_free_elem[unit]);
    SHR_NULL_CHECK(t_info->field_free_list, SHR_E_MEMORY);

    t_info->unit = unit;
    t_info->lock = sal_rwlock_create("IMM_BE");
    SHR_NULL_CHECK(t_info->lock, SHR_E_MEMORY);
    t_info->sorted_fid = NULL;
    t_info->fid_array_size = 0;

    tbl = shr_ha_mem_alloc(unit,
                           comp_id,
                           sub_id,
                           &len);
    SHR_NULL_CHECK(tbl, SHR_E_MEMORY);
    t_info->tbl = tbl;

    /*
     * It is possible that we are coming to this function after this table
     * was already created. When we shut down we keep the HA information
     * in tact so we can recover for warm boot. However, if it is cold boot
     * and the block signature matches (i.e. valid block) then we need to
     * first clean up the old dynamic data and entry elements and return them
     * back to their respective free lists.
     */
    if (!warm && tbl->signature == BCMIMM_BLK_SIGNATURE) {
        bcmimm_table_content_clear(t_info);
    }
    if (!warm || tbl->signature != BCMIMM_BLK_SIGNATURE) {
        sal_memset(tbl, 0, len);
        tbl->comp_id = comp_id;
        tbl->sub_id = sub_id;
        tbl->data_len = data_size;
        tbl->key_len = key_size;
        tbl->num_of_rows = rows;
        tbl->num_of_ent = 0;
        tbl->ent_len = (uint8_t)entry_len;
        tbl->signature = BCMIMM_BLK_SIGNATURE;
    } else if ((tbl->data_len != data_size) || (tbl->key_len != key_size)) {
        
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *hdl = (bcmimm_be_tbl_hdl_t)((((uint16_t)(unit & 0xFFFF)) << 16) + idx);
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_release(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);

    /* Free resources that are not HA resources */
    sal_rwlock_destroy(t_info->lock);
    if (t_info->sorted_fid) {
        sal_free(t_info->sorted_fid);
    }
    /* Release the table info struct */
    sal_memset(t_info, 0, sizeof(*t_info));

    /* Decrease the number of active tables for this unit */
    active_tables[hdl >> 16].active_tbls--;
exit:
    SHR_FUNC_EXIT();
}

size_t bcmimm_be_table_size_get(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    if ((hdl >> 16) >= BCMDRD_CONFIG_MAX_UNITS) {
        return 0;
    }
    if (((hdl & 0xFFFF) >= active_tables[hdl >> 16].active_tbls) ||
        !t_info || !t_info->tbl ||
        (t_info->tbl->signature != BCMIMM_BLK_SIGNATURE)) {
        return 0;
    }
    return t_info->tbl->num_of_ent;
}

int bcmimm_be_table_ctx_get_first(bcmimm_be_tbl_hdl_t hdl,
                                  void *key,
                                  bcmimm_be_fields_t *out)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint32_t *ptr;
    uint8_t *ent;
    uint32_t start_idx = 0;
    uint32_t start_ptr  = 0;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);
    if ((key == NULL) || (out == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    ent = bcmimm_next_entry_find(t_info,
                                 true,
                                 start_idx,
                                 &start_ptr);
    if (!ent) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    sal_memcpy(key, ent, t_info->tbl->key_len);

    ptr = BCMIMM_ENTRY_PTR_GET(ent, t_info->tbl->ent_len);
    ptr--;
    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_data_field_get(t_info, *ptr, out));

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_ctx_get_next(bcmimm_be_tbl_hdl_t hdl,
                                 void *in_key,
                                 void *out_key,
                                 bcmimm_be_fields_t *out)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint32_t *ptr;
    uint8_t *ent;
    uint32_t start_idx;
    uint32_t start_ptr  = 0;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);
    if ((in_key == NULL) || (out_key == NULL) || (out == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    /* Find the previous entry and pointer */
    start_idx = shr_elf_hash(in_key, t_info->tbl->key_len) %
                    t_info->tbl->num_of_rows;
    ent = bcmimm_entry_find(t_info, in_key, &ptr , NULL);
    if (ent) {
        start_ptr = *ptr;
    }

    /* Search from the next element */
    ent = bcmimm_next_entry_find(t_info,
                                 false,
                                 start_idx,
                                 &start_ptr );
    if (!ent) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    sal_memcpy(out_key, ent, t_info->tbl->key_len);

    ptr = BCMIMM_ENTRY_PTR_GET(ent, t_info->tbl->ent_len);
    ptr--;
    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_data_field_get(t_info, *ptr, out));

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_clear(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER));
    bcmimm_table_content_clear(t_info);

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_insert(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           const bcmimm_be_fields_t *in)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint32_t idx;
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);
    idx = shr_elf_hash(key, t_info->tbl->key_len) % t_info->tbl->num_of_rows;
    entry = (uint8_t *)t_info->tbl + sizeof(tbl_header_t);
    entry += idx * t_info->tbl->ent_len;
    ent_ptr = BCMIMM_ENTRY_PTR_GET(entry,t_info->tbl->ent_len);
    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER));

    if (BCMIMM_IS_OCCUPIED(*ent_ptr)) {
        /* Verify that the key is not already there */
        if (sal_memcmp(key, entry, t_info->tbl->key_len) == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
        }

        /*
         * The bin is occupied, go throuh the link list until the last
         * element.
         */
        while (!BCMIMM_IS_NULL_PTR(*ent_ptr)) {
            entry = bcmimm_entry_get (t_info->entry_free_list, *ent_ptr);
            if (sal_memcmp(key, entry, t_info->tbl->key_len) == 0) {
                SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
            }
            ent_ptr = BCMIMM_ENTRY_PTR_GET(entry, t_info->tbl->ent_len);
        }

        /*
         * This function allocates new entry from the free list and assign the
         * pointer value (ent_ptr) to point to this newly allocated entry.
         * Note the ent_ptr is a pointer to the next element pointer area
         * of the last entry in the list. So once this function returns
         * the last entry will point to the newly allocated entry.
        */
        entry = bcmimm_free_list_elem_get(t_info->unit,
                                   t_info->entry_free_list,
                                   ent_ptr);
        SHR_NULL_CHECK(entry, SHR_E_MEMORY);
        /* Assigned to the last entry pointer */
        ent_ptr = BCMIMM_ENTRY_PTR_GET(entry, t_info->tbl->ent_len);
    }
    sal_memcpy(entry, key, t_info->tbl->key_len);
    *ent_ptr = 0;   /* 'NULL' termination */
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, t_info->tbl->ent_len);
    *entry_lock_cnt = 0;
    BCMIMM_OCCUPIED_SET(*ent_ptr);
    ent_ptr--;
    SHR_IF_ERR_EXIT(bcmimm_data_field_insert(t_info, ent_ptr, in));
    t_info->tbl->num_of_ent++;
exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_delete(bcmimm_be_tbl_hdl_t hdl, const void *key)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint8_t *prev_entry;
    uint32_t *ent_ptr;
    uint32_t *prev_ent_ptr;
    uint32_t del_ent_ptr; /* The pointer value of the entry to delete */
    size_t ptr_offset = t_info->tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER));
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, &prev_entry);

    if (entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Don't delete locked entries */
    if (BCMIMM_IS_LOCKED(*ent_ptr)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /* Free the data fields */
    bcmimm_free_list_elem_put(t_info->field_free_list, *(ent_ptr-1));
    t_info->tbl->num_of_ent--;

    if (prev_entry == NULL) {
        /*
         * The entry was the first entry. In this case if there is a following
         * entry in the same bin then copy the following entry on to the first
         * one. After, free the next entry. Otherwise (only entry in the bin),
         * simply clear the occupied bit.
         */
        if (BCMIMM_IS_NULL_PTR(*ent_ptr)) {
            /* No other entry in this bin */
            BCMIMM_OCCUPIED_CLEAR(*ent_ptr);
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else {
            prev_entry = entry;
            del_ent_ptr = *ent_ptr;
            entry = bcmimm_entry_get (t_info->entry_free_list, *ent_ptr);
            /*
             * Copy the next entry into the first one. This makes the first
             * entry to point to the next->next. so we short circuit the
             * second entry in the bin. It also means that the field list
             * is pointed by the first entry.
             */
            sal_memcpy(prev_entry, entry, t_info->tbl->ent_len);
            /* Last need to set the entry to delete pointer to NULL */
            ent_ptr = (uint32_t *)(entry + ptr_offset);
            *ent_ptr = 0;
        }
    } else {
        prev_ent_ptr = (uint32_t *)(prev_entry + ptr_offset);
        del_ent_ptr = *prev_ent_ptr;
        *prev_ent_ptr = *ent_ptr; /* Point to the next->next */
        *ent_ptr = 0;   /* Null terminate the entry */
    }
    bcmimm_free_list_elem_put(t_info->entry_free_list, del_ent_ptr);

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_lookup(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           bcmimm_be_fields_t *out)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (!entry) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    ent_ptr--;
    SHR_IF_ERR_EXIT(bcmimm_data_field_get(t_info, *ent_ptr, out));

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_update(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           const bcmimm_be_fields_upd_t *in)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    SHR_NULL_CHECK(entry, SHR_E_NOT_FOUND);

    /* Don't update locked entries */
    if (BCMIMM_IS_LOCKED(*ent_ptr)) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

    ent_ptr--;
    SHR_IF_ERR_EXIT(bcmimm_data_field_update(t_info, ent_ptr, in));

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_lock(bcmimm_be_tbl_hdl_t hdl, const void *key)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER));
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    SHR_NULL_CHECK(entry, SHR_E_NOT_FOUND);
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, t_info->tbl->ent_len);
    if (*entry_lock_cnt == 0xFFFF) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    if ((*entry_lock_cnt)++ == 0) {
        BCMIMM_ENTRY_LOCK(*ent_ptr);
    }
exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_unlock(bcmimm_be_tbl_hdl_t hdl, const void *key)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER));
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    SHR_NULL_CHECK(entry, SHR_E_NOT_FOUND);
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, t_info->tbl->ent_len);
    if (*entry_lock_cnt > 0) {
        if (--(*entry_lock_cnt) == 0) {
            BCMIMM_ENTRY_UNLOCK(*ent_ptr);
        }
    }

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_get_lock_cnt(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           uint32_t *lock_cnt)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    if (!lock_cnt) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER));

    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, t_info->tbl->ent_len);

    *lock_cnt = *entry_lock_cnt;
exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_data_resize(bcmimm_be_tbl_hdl_t hdl, size_t d_size)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}
