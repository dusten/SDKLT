/*! \file bcmlt_ltable_field.c
 *
 * Handles regular fields.
 * This module contains functions that handles field functionality. A field
 * can be symbol or scalar. The field can not be array field. These been
 * handled by a different module.
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

#include <sal/sal_alloc.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include <bcmltm/bcmltm_md.h>
#include <bcmltd/bcmltd_table.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/* Adds a field into an entry */
#define FIELD_ADD(entry, fname, sym, field)  \
    if (!fname) {                                                \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                        \
    }                                                            \
    { int _rv;                                                   \
        if ((_rv = alloc_new_field(entry, fname, sym, field))    \
            != SHR_E_NONE) {                                     \
            SHR_RETURN_VAL_EXIT(_rv);                            \
        }                                                        \
    }

/* Obtain a field from an entry */
#define FIELD_GET(entry, fname, field) \
        if (!entry || (entry->state == E_IDLE) ||                    \
            !fname) {                                                \
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                        \
        }                                                            \
        { int _rv;                                                   \
            if ((_rv = find_field_by_name(entry, fname, field)) \
                 != SHR_E_NONE) {                                    \
                 SHR_RETURN_VAL_EXIT(_rv);                           \
            }                                                        \
        }

#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffffL
#endif

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Allocates new field and adds it to the entry.
 *
 * This function adds a new field onto an entry or update the value of
 * the field if already in the entry.
 *
 * \param [in] entry is the entry to add the field into.
 * \param [in] field_name is the name of the field to add.
 * \param [in] sym is true if the field is of symbol type.
 * \param [out] field is a pointer to the newly created/existed field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int alloc_new_field(bcmtrm_entry_t *entry,
                           const char *field_name,
                           bool sym,
                           shr_fmm_t **field)
{
    shr_fmm_t *loc_field;
    uint32_t field_id;
    bcmlt_field_def_t *attr;

    SHR_FUNC_ENTER(entry->info.unit);

    /* Do not allow to add field for notification */
    if (!entry->db_hdl) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    /* Get field ID from DB */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlt_db_field_info_get(entry->info.unit,
                                field_name,
                                entry->db_hdl,
                                &attr,
                                &field_id));

    if ((attr->symbol != sym) ||
        (attr->depth > 0) || (attr->elements > 1)/*(attr->array)*/){
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit, "Field %s is not a scalar\n"),
                     field_name));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (entry->fld_arr) {
        loc_field = entry->fld_arr[field_id];
    } else {
        loc_field = bcmlt_find_field_in_entry(entry, field_id, NULL);
    }
    if (!loc_field) {
        loc_field = shr_fmm_alloc();
        SHR_NULL_CHECK(loc_field, SHR_E_MEMORY);
        loc_field->id = field_id;
        loc_field->idx = 0;
        loc_field->flags = 0;
        loc_field->next = entry->l_field;
        entry->l_field = loc_field;
        if (entry->fld_arr) {
            entry->fld_arr[field_id] = loc_field;
        }
    }
    *field = loc_field;
exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Search for a field in an entry based on field name.
 *
 * This function searches the given entry for a specific field that matches
 * the field name. If found the function set the output value of \c field.
 * Note that the entry only contains internal field ID, therefore the function
 * searches the internal DB for the internal field ID.
 *
 * \param [in] entry is the entry to add the field into.
 * \param [in] field_name is the name of the field to search.
 * \param [out] field is a pointer to the newly created/existed field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int find_field_by_name(bcmtrm_entry_t *entry,
                               const char *field_name,
                               shr_fmm_t **field)
{
    uint32_t field_id;

    SHR_FUNC_ENTER(entry->info.unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlt_db_field_info_get(entry->info.unit,
                                field_name,
                                entry->db_hdl,
                                NULL,
                                &field_id));

    if (entry->fld_arr) {
        *field = entry->fld_arr[field_id];
    } else {
        *field = bcmlt_find_field_in_entry(entry, field_id, NULL);
    }
    SHR_NULL_VERBOSE_CHECK(*field, SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

static bool entry_field_mapped(bcmlrd_map_entry_t *entry)
{
    int entry_type = entry->entry_type;

    if ((entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY) ||
        (entry_type == BCMLRD_MAP_ENTRY_MAPPED_VALUE)) {
        return TRUE;
    }

    return FALSE;
}

static bool entry_field_fixed(bcmlrd_map_entry_t *entry)
{
    int entry_type = entry->entry_type;

    if ((entry_type == BCMLRD_MAP_ENTRY_FIXED_KEY) ||
        (entry_type == BCMLRD_MAP_ENTRY_FIXED_VALUE)) {
        return TRUE;
    }

    return FALSE;
}

static bool field_handler_internal(uint32_t field_id) {
    switch (field_id) {
        case BCMLRD_FIELD_INDEX :
        case BCMLRD_FIELD_PORT :
        case BCMLRD_FIELD_ENTRY_PRIORITY :
        case BCMLRD_FIELD_INSTANCE :
        case BCMLRD_FIELD_TABLE_SEL :
            return TRUE;
        default:
            return FALSE;
    }
}

static const char *internal_field_id_to_name(uint32_t field_id) {
    switch (field_id) {
        case BCMLRD_FIELD_INDEX :
            return "__INDEX";
        case BCMLRD_FIELD_PORT :
            return "__PORT";
        case BCMLRD_FIELD_ENTRY_PRIORITY :
            return "__ENTRY_PRIORITY";
        case BCMLRD_FIELD_INSTANCE :
            return "__INSTANCE";
        case BCMLRD_FIELD_TABLE_SEL :
            return "__TABLE_SEL";
        default:
            return NULL;
    }
}

static const char *xfrm_dir(bcmlrd_map_entry_t *entry)
{
    switch (entry->entry_type) {
        case BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER:
        case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
               return "Forward";
        case BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER:
        case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
               return "Reverse";
        default:
               return NULL;
    }
}

static bool xfrm_field_find(int unit, const bcmlrd_map_entry_t *entry,
                            uint32_t fid)
{
    uint32_t idx;
    bool src = TRUE;
    uint32_t num_fields;
    const bcmltd_field_list_t *flist;

    if (bcmltm_md_xfrm_field_get(unit, entry, src,
                                 &num_fields, &flist) != 0) {
        return FALSE;
    }

    for (idx = 0; idx < num_fields; idx++) {
        if (flist->field_array[idx].field_id == fid) {
            return TRUE;
        }
    }

    return FALSE;
}

static int
update_pt_info(int unit, bcmlt_field_map_hdl_t *fld_info,
               uint32_t pt_id, uint32_t map,
               bool pt_field_valid, uint32_t pt_field_id,
               uint32_t minbit, uint32_t maxbit)
{
    int rv = SHR_E_NONE;
    uint32_t map_len, field_len;
    const char *pt_name = NULL;
    bcmlt_field_map_info_t *map_info;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    map_len = 0;
    map_info = &fld_info->map_info[map];

    /* Update Map information. */
    if (pt_id != UINT32_MAX) {

        pt_name = bcmdrd_pt_sid_to_name(unit, pt_id);

        map_len = sal_strlen(pt_name);
        if (map_len < BCMLT_MAPPING_STR_LEN_MAX) {
            sal_memcpy(map_info->fmap, pt_name, map_len + 1);
        } else {
            sal_strcpy(map_info->fmap, "<Table name too long>.");
            map_len = BCMLT_MAPPING_STR_LEN_MAX;
        }
    }

    if (pt_field_valid && !field_handler_internal(pt_field_id)) {
        /* Concatenate the PT field to the PT table name.
         * map_len is set to the length of table name. */
        uint32_t len = map_len;

        /* Update PT field bit positions. */
        map_info->minbit = minbit;
        map_info->maxbit = maxbit;

        /* Return if name buffer is not sufficient. */
        if (map_len >= BCMLT_MAPPING_STR_LEN_MAX) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }

        rv = bcmdrd_pt_field_info_get(unit, pt_id, pt_field_id, &finfo);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Error in reading PT field info.\n")));
            SHR_RETURN_VAL_EXIT(rv);
        }

        /* Form the mapping string.
         * Add 3 bytes.
         * 1 for NULL termination,
         * 1 for period between table & field,
         * last byte for appending "f" at the end of field name. */
        field_len = sal_strlen(finfo.name);
        map_len += field_len + 3;
        if (map_len <= BCMLT_MAPPING_STR_LEN_MAX) {
            map_info->fmap[len++] = '.';
            /* coverity[overrun-local] */
            sal_memcpy(&map_info->fmap[len], finfo.name, field_len);
            len += field_len;
            map_info->fmap[len++] = 'f';
            map_info->fmap[len] = '\0';
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int bcmlt_entry_field_add(bcmlt_entry_handle_t entry_hdl,
        const char *field_name,
        uint64_t data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    FIELD_ADD(entry, field_name, false, &field);
    field->data = data;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 const char *data)
{
    shr_fmm_t *field = NULL;
    int rv;
    uint32_t val;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    FIELD_ADD(entry, field_name, true, &field);
    if (!data) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    rv = bcmlrd_field_symbol_to_value(entry->info.unit,
                                 entry->table_id,
                                 field->id,
                                 data,
                                 &val);
    if (rv != SHR_E_NONE) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }
    field->data = val;
exit:
    /*
     * Failure can be due to invalid symbol or invalid field name. If the
     * later then field is NULL. Don't delete it!
     */
    if (SHR_FUNC_ERR() && field) {
        shr_fmm_t *prev_field;
        /* Delete the field */
        if (entry->fld_arr) {
            entry->fld_arr[field->id] = NULL;
        }
        field = bcmlt_find_field_in_entry(entry, field->id, &prev_field);
        if (!prev_field) {
            entry->l_field = field->next;
        } else {
            prev_field->next = field->next;
        }
        shr_fmm_free(field);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_get(bcmlt_entry_handle_t entry_hdl,
        const char *field_name,
        uint64_t *data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    FIELD_GET(entry, field_name, &field);
    *data = field->data;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 const char **data)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    FIELD_GET(entry, field_name, &field);
    if (!data) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(
        bcmlrd_field_value_to_symbol(entry->info.unit,
                                      entry->table_id,
                                      field->id,
                                      (uint32_t)(field->data & 0xFFFFFFFF),
                                      data));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_unset(bcmlt_entry_handle_t entry_hdl,
                            const char *field_name)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);

    ENTRY_VALIDATE(entry);
    FIELD_GET(entry, field_name, &field);
    field->flags |= SHR_FMM_FIELD_DEL;

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_remove(bcmlt_entry_handle_t entry_hdl,
                             const char *field_name)
{
    shr_fmm_t *p_field;
    shr_fmm_t *p_field_tmp;
    int rv;
    uint32_t field_id;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!entry->l_field) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    rv = bcmlt_db_field_info_get(entry->info.unit,
                          field_name,
                          entry->db_hdl,
                          NULL,
                          &field_id);
    if (rv != 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (entry->fld_arr) {
        entry->fld_arr[field_id] = NULL;    /* Clean up the field array */
    }

    p_field = entry->l_field;
    /* If the field is the first in the list */
    if (p_field->id == field_id) {
        entry->l_field = p_field->next;
        shr_fmm_free(p_field);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Search the rest of the field list */
    p_field_tmp = NULL;
    for (; p_field->next && (p_field->next->id != field_id);
         p_field = p_field->next);
    if (!p_field->next) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    p_field_tmp = p_field->next;
    p_field->next = p_field_tmp->next; /* Remove the field from the list */
    shr_fmm_free(p_field_tmp);

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_symbol_info_get(bcmlt_entry_handle_t entry_hdl,
                                      const char *field_name,
                                      uint32_t sym_names_size,
                                      const char *sym_names[],
                                      uint32_t *actual_size)
{
    uint32_t field_id;
    bcmlt_field_def_t *attr;
    shr_enum_map_t *names = NULL;
    size_t sym_count;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);

    SHR_IF_ERR_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                          field_name,
                          entry->db_hdl,
                          &attr,
                          &field_id));
    if (!attr->symbol) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Field %s is not of symbol type\n"),
                     field_name));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlrd_field_symbols_count_get(entry->info.unit,
                                        entry->table_id,
                                        field_id,
                                        &sym_count));
    if (actual_size) {
        *actual_size = (uint32_t)sym_count;
    }
    if (sym_names) {
        size_t j;
        size_t act;
        if (sym_names_size < sym_count) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        names = sal_alloc(sizeof(shr_enum_map_t) * sym_count, "BCMLT");
        SHR_NULL_CHECK(names, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(bcmlrd_field_symbols_get(entry->info.unit,
                                                 entry->table_id,
                                                 field_id,
                                                 sym_count,
                                                 names,
                                                 &act));
        if (act != sym_count) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        for (j = 0; j < sym_count; j++) {
            sym_names[j] = names[j].name;
        }
    }
exit:
    if (names) {
        sal_free(names);
    }
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_traverse(bcmlt_entry_handle_t entry_hdl,
                               const char **field_name,
                               bcmlt_field_data_info_t *data,
                               uint32_t **search_hdl)
{
    shr_fmm_t *field;
    bcmlrd_field_def_t field_info;
    bcmdrd_sym_field_info_t f_info;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!search_hdl || !data || !field_name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(entry->l_field, SHR_E_NOT_FOUND);

    /* Find the next field */
    if (*search_hdl == NULL) {
        field = entry->l_field;
    } else {
        field = (shr_fmm_t *)(*search_hdl);
        if (!field->next) {
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
        } else {
            field = field->next;
        }
    }

    /* Obtain the field information */
    if (entry->pt) {
        SHR_IF_ERR_EXIT(bcmltm_md_pt_field_info_get(entry->info.unit,
                                                    entry->table_id,
                                                    field->id,
                                                    &f_info));
        *field_name = f_info.name;
        data->symbol = false;
        data->array = true;
    } else {
        SHR_IF_ERR_EXIT(bcmlrd_table_field_def_get(entry->info.unit,
                                                   entry->table_id,
                                                   field->id,
                                                   &field_info));

        *field_name = field_info.name;
        data->symbol = field_info.symbol;
        data->array = field_info.array;
    }
    data->idx = field->idx;
    if (data->symbol) {
        SHR_IF_ERR_EXIT(
               bcmlrd_field_value_to_symbol(entry->info.unit,
                                            entry->table_id,
                                            field->id,
                                            field->data,
                                            &data->data.sym_val));
    } else {
        data->data.val = field->data;
    }
    *search_hdl = (uint32_t *)field;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_count(bcmlt_entry_handle_t entry_hdl,
                            uint32_t *field_count)
{
    shr_fmm_t *field;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    uint32_t count = 0;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!field_count) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    field = entry->l_field;
    while (field) {
        field = field->next;
        count++;
    }

    *field_count = count;
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_field_pt_map_get(int unit, const char *tbl_name,
                           const char *fld_name, uint32_t *instances,
                           bcmlt_field_map_hdl_t *fld_info)
{
    void *hdl;
    int rv;
    uint32_t pt_id, lt_field_id, pt_field_id, field_id;
    bool direct, internal, xfrm;
    bool pt_field_valid, entry_found;
    uint32_t inst = 0, index, size;
    size_t grp, fld, ent;
    bool src = FALSE;
    uint32_t num_fields;
    const bcmltd_field_list_t *flist;
    bcmlt_table_attrib_t *table_attr;
    const bcmlrd_map_t *map = NULL;
    bcmlt_field_def_t *f_attr;
    bcmlrd_map_entry_t *entry = NULL;
    shr_lmm_hdl_t fld_array_hdl;
    bcmlt_field_map_hdl_t *info;

    SHR_FUNC_ENTER(unit);
    if (!bcmlt_is_initialized()) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    if (!bcmlt_is_unit_attached(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    if ((!tbl_name) || (!fld_name)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    pt_id = lt_field_id = pt_field_id = UINT32_MAX;

    /* Find the table ID associated with the name */
    SHR_IF_ERR_EXIT
        (bcmlt_db_table_info_get(unit, tbl_name, &table_attr,
                                 &fld_array_hdl, &hdl));

    /* Get Map details. */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, table_attr->table_id, &map));

    /* Get logical field ID to compare against the map information. */
    SHR_IF_ERR_EXIT(
        bcmlt_db_field_info_get(unit, fld_name, hdl,
                                &f_attr, &lt_field_id));

    /* For each group, check for field desc information. */
    for (grp = 0; grp < map->groups; grp++) {
        if (map->group[grp].dest.kind == BCMLRD_MAP_CUSTOM) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        if (map->group[grp].dest.kind == BCMLRD_MAP_VALIDATION) {
            continue;
        }
        for (ent = 0; ent < map->group[grp].entries; ent++) {
            direct = internal = xfrm = FALSE;
            entry_found = pt_field_valid = FALSE;
            entry = (bcmlrd_map_entry_t *)&map->group[grp].entry[ent];
            info = fld_info;

            /* For fixed value fields, do nothing. */
            if (entry_field_fixed(entry)) {
                continue;
            }

            if (entry_field_mapped(entry)) {
                field_id = entry->u.mapped.src.field_id;

                /* For direct mapping, get PT and PT fields ID. */
                if (field_id == lt_field_id) {
                    /* Handler internal fields (__PORT etc). */
                    if (field_handler_internal(entry->desc.field_id)) {
                        internal = TRUE;
                    } else {
                        direct = TRUE;
                        pt_field_valid = TRUE;
                        pt_field_id = entry->desc.field_id;
                    }
                    pt_id = map->group[grp].dest.id;
                    inst++;
                    entry_found = TRUE;
                }
            } else if (xfrm_field_find(unit, entry, lt_field_id)) {
                /* For transform fields, get info on all the field mappings. */
                pt_id = map->group[grp].dest.id;
                xfrm = TRUE;
                inst++;
                SHR_IF_ERR_EXIT(
                    bcmltm_md_xfrm_field_get(unit, entry, src,
                                             &num_fields, &flist));
                if (num_fields > info->num_map) {
                    /* IF allocated mapping is insufficent, return error. */
                    info->num_map = num_fields;
                    if (inst > *instances) {
                        *instances = inst;
                    }
                    SHR_RETURN_VAL_EXIT(SHR_E_FULL);
                }
                entry_found = TRUE;
            }

            if ((inst > *instances) || (!entry_found)) {
                /* More than allocated instance found. Skip fld_info update. */
                continue;
            }

            index = inst - 1;
            size = sizeof(bcmlt_field_map_hdl_t) +
                (sizeof(bcmlt_field_map_info_t) * info->num_map);
            info = (bcmlt_field_map_hdl_t *)
                ((char *) fld_info + (size * index));
            if (!xfrm) {
                rv = update_pt_info(unit, info, pt_id, 0,
                                    pt_field_valid, pt_field_id,
                                    entry->desc.minbit,
                                    entry->desc.maxbit);
                if (rv != SHR_E_NONE) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("Error in updating PT info.\n")));
                    SHR_RETURN_VAL_EXIT(rv);
                }

                if (direct) {
                    sal_strcpy(info->handler, "Direct");
                } else if (internal) {
                    int len;
                    const char *int_type_name =
                        internal_field_id_to_name(entry->desc.field_id);
                    sal_strcpy(info->handler, "Internal");
                    if (int_type_name != NULL) {
                        len = sal_strlen(int_type_name);
                        if (len < BCMLT_TYPE_STR_LEN_MAX) {
                            sal_memcpy(info->int_fld_type,
                                       int_type_name, len + 1);
                        } else {
                            /* Ideally the execution flow should not
                             * hit this block. */
                            sal_strcpy(info->int_fld_type,
                                       "<Internal>");
                        }
                    }
                }
            } else {
                /* For transform, return PT map info for all fields. */
                const char *xfrm_dir_name = xfrm_dir(entry);
                uint32_t len;
                sal_strcpy(info->handler, "Transform");
                if (xfrm_dir_name != NULL) {
                    len = sal_strlen(xfrm_dir_name);
                    if (len >= BCMLT_TYPE_STR_LEN_MAX) {
                        /* Truncate string to match the max length. */
                        len = BCMLT_TYPE_STR_LEN_MAX - 1;
                    }
                    sal_memcpy(info->int_fld_type,
                               xfrm_dir_name, len + 1);
                }

                /* Update PT table name for each mapping in xfrm. */
                for (fld = 0; fld < num_fields; fld++) {
                    pt_field_id = flist->field_array[fld].field_id;
                    rv = update_pt_info(unit, info, pt_id,
                                        fld, TRUE, pt_field_id,
                                        flist->field_array[fld].minbit,
                                        flist->field_array[fld].maxbit);
                    if (rv != SHR_E_NONE) {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META("Error in updating PT info.\n")));
                        SHR_RETURN_VAL_EXIT(rv);
                    }
                } /* for fld loop. */
            } /* xfrm check. */
        } /* for entries loop. */
    } /* for group loop. */

    /* Retrun error if number of PT map instances is more than expected. */
    if (inst > *instances) {
        *instances = inst;
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_field_desc_get(int unit,
                         const char *tbl_name,
                         const char *fld_name,
                         const char **fld_desc)
{
    int tbl_id;
    uint32_t fld;
    int field_found = FALSE;
    const bcmltd_table_rep_t *lt_info;
    const bcmltd_field_rep_t *fld_info;

    SHR_FUNC_ENTER(unit);

    /* Sanity checks. */
    if (!bcmlt_is_initialized()) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
    if (!bcmlt_is_unit_attached(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    if (!tbl_name) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    tbl_id = bcmltd_table_name_to_idx(tbl_name);

    /* Return if table is not found. */
    if (tbl_id == SHR_E_NOT_FOUND) {
        SHR_RETURN_VAL_EXIT(tbl_id);
    }

    lt_info = bcmltd_table_get((bcmltd_sid_t)tbl_id);
    if (lt_info == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /* Loop through fields in LT to find the requested field. */
    for (fld = 0; fld < lt_info->fields; fld++) {
        fld_info = &lt_info->field[fld];
        if (!sal_strcmp(fld_name, fld_info->name)) {
            *fld_desc = fld_info->desc;
            field_found = TRUE;
            break;
        }
    }

    if (field_found == FALSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}
