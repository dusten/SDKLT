/*! \file bcmlt_ltable_field_array.c
 *
 * This file implements all the functionality associated with array fields.
 * Array fields can be scalar or symbol. The main functionality is the field
 * add and delete. Note that for arrays, the implementation keeps the array
 * fields sorted based on their index.
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
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Assign u64 value to a field or from field to u64 value.
 *
 * This function is used to assign a u64 value from the u64
 * array pointed by val_arry to the input field or vice versa. The direction
 * of assignment dictated by the value of to_field. After assigning the value
 * the function increment the pointer to point the next element in the array.
 *
 * \param [in] fld points to the field at assign the value to it.
 * \param [in,out] val_arry points to a pointer pointing to an array of boolean
 * values. This pointer is also incremented to point to the next element of the
 * array.
 * \param [in] to_field is true if the copy is from the array into the field
 * and false otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int assign_u64(bcmtrm_entry_t *entry,
                       shr_fmm_t *fld,
                       void **val_arry,
                       bool to_field)
{
    uint64_t **val = (uint64_t **)val_arry;
    if (to_field) {
        fld->data = **val;
    } else {
        **val = fld->data;
    }
    (*val)++;
    *val_arry = *val;
    return SHR_E_NONE;
}

/*!
 * \brief Assign value to or from symbol.
 *
 * This function assigned symbol value from enumeration value or enumeration
 * value from symbol. The operation is based on the value of the parameter
 * \c to_field.
 *
 * \param [in] entry Is a pointer to the entry structure.
 * \param [in,out] fld Is the field structure to assign the data to/from it.
 * \param [in] val_arry Is an array of values that should be used as source or
 * destination of the assignment. The function increments this pointer to
 * point to the next data element.
 * \param [in] to_field Indicates if the value should be assigned to or from
 * the field structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int assign_symbol(bcmtrm_entry_t *entry,
                          shr_fmm_t *fld,
                          void **val_arry,
                          bool to_field)
{
    /*
     * For assign_u64 the value is uint64_t, however, here the value is
     * a pointer. So we are at three pointers deep.
     */
    const char ***val = (const char ***)val_arry;
    int rv;

    if (to_field) {
        uint32_t enum_val = 0;
        rv = bcmlrd_field_symbol_to_value(entry->info.unit,
                                 entry->table_id,
                                 fld->id,
                                 **val,
                                 &enum_val);
        fld->data = enum_val;
    } else {
        rv = bcmlrd_field_value_to_symbol(entry->info.unit,
                                      entry->table_id,
                                      fld->id,
                                      (uint32_t)(fld->data & 0xFFFFFFFF),
                                      *val);
    }
    if (rv != SHR_E_NONE) {
        return rv;
    }
    (*val)++;
    *val_arry = (void *)*val;
    return rv;
}

/*!
 * \brief Verify symbol array values.
 *
 * This function verifies that all the symbol values, provided as input
 * to an array symbol add function are valid.
 *
 * \param [in] entry is a pointer to the entry structure.
 * \param [in] field_name is the field identifier associated with the symbol
 * values.
 * \param [in] data Is an array of symbol values.
 * \param [in] num_of_elem Indicates how many symbols present in the array
 * \c data
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int verify_sym(bcmtrm_entry_t *entry,
                      const char *field_name,
                      const char **data,
                      uint32_t num_of_elem)
{
    uint32_t f_id;
    bcmlt_field_def_t *f_def;
    uint32_t j;
    uint32_t enum_val = 0;

    SHR_FUNC_ENTER(entry->info.unit);

    if (num_of_elem == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                                            field_name,
                                            entry->db_hdl,
                                            &f_def,
                                            &f_id));

    if (!f_def->symbol) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    for (j = 0; j < num_of_elem; j++) {
        if (bcmlrd_field_symbol_to_value(entry->info.unit,
                                         entry->table_id,
                                         f_id,
                                         data[j],
                                         &enum_val) != SHR_E_NONE) {
            SHR_RETURN_VAL_EXIT(SHR_E_BADID);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search for field in an entry.
 *
 * This function is used to search for a field in an entry based on the field
 * ID. If the field was found the function also set the value of the previous
 * field to the one that was found.
 *
 * \param [in] entry is a pointer to the entry structure.
 * \param [in] field_id is the id to search for
 * \param [out] prev points to the previous field to the searched field (if
 * the searched field was found) or to NULL if the field was not found in the
 * entry. If the input value of this parameter is NULL this parameter will be
 * ignored
 *
 * \return pointer to the field if was found and NULL otherwise.
 */
shr_fmm_t *
bcmlt_find_field_in_entry(bcmtrm_entry_t *entry,
                    uint32_t field_id,
                    shr_fmm_t **prev)
{
    shr_fmm_t *field = entry->l_field;

    if (prev) {
        *prev = NULL;
    }
    while (field) {
        if (field->id == field_id) {
            return field;
        }
        if (prev) {
            *prev = field;
        }
        field = field->next;
    }
    return NULL;
}
#if 0
/*! \brief Find the location to place the new array.
 *
 *  This function find the location where a new array of element can be
 *  inserted into the current field array. The field array linked list
 *  is a sorted list based on the field idx parameter.
 *  This function search the field array until it find the last element
 *  that has index that is smaller than the start index. The function also
 *  verify that the entire array can fit into what already been set without
 *  any index overlap. If no overlap was found the field that is previous
 *  to the location where the new array should be inserted is assigned into
 *  \ref p_fld. Note that if this is the first field in the entry then the
 *  value of p_fld will be NULL.
 *
 * \param [in,out] p_fld is the begining of the field array. Its output
 * value will point to the field proceeding the location of the new field
 * list. In other words, the output value will point to the head of the new
 * list.
 * \param [in] prev_fld pointer to the previous field of p_fld.
 * \param [in] start_idx is the first index to add new elements.
 * \param [in] last_idx is the highest index to add new element.
 *
 * \return SHR_E_NONE on success and SHR_E_PARAM otherwise.
 */
static int find_array_location(shr_fmm_t **p_fld,
                               shr_fmm_t *prev_fld,
                               uint32_t *start_idx,
                               uint32_t *last_idx)
{
    uint32_t f_id= (*p_fld)->id;

    /*
     * Desired index is lower than the first index. Loccation is before
     * existing location.
     */
    if ((*p_fld)->idx == *start_idx) {
        (*start_idx)++;
    }
    if ((*p_fld)->idx > *start_idx) {
        if ((*p_fld)->idx <= *last_idx) {
            *last_idx = (*p_fld)->idx - 1;  /* Overlapping indexes */
        }
        *p_fld = prev_fld;
        return SHR_E_NONE;
    } else { /* Location is not at the begining of the array. */
        /* Search for the first element that the array can go into */
        while ((*start_idx <= *last_idx) &&
               (*p_fld)->next && (*p_fld)->next->id == f_id) {
            if ((*p_fld)->next->idx == *start_idx) {
                (*start_idx)++;
            }
            else if ((*p_fld)->next->idx > *start_idx) {
                if ((*p_fld)->next->idx <= *last_idx) {
                    *last_idx = (*p_fld)->next->idx - 1;   /* Overlapping idx */
                }
                return SHR_E_NONE;
            }
            *p_fld = (*p_fld)->next;
        }
        return SHR_E_NONE;
    }
}
#endif

/*! \brief Allocates field array.
 *
 *  This function allocates all the field elements of a field array.
 *  It returns a pointer to the start of the field array list.
 *
 * \param [in] start_idx is the first array index to add new elements.
 * \param [in] num_of_elem indicates the array size.
 * \param [in] field_id is the field array ID.
 * \param [in] dtag is the data tag of this field array.
 *
 * \return A pointer to the start of the list on success and NULL otherwise.
 */
static shr_fmm_t *field_array_alloc(uint32_t start_idx,
                                    uint32_t num_of_elem,
                                    uint32_t field_id,
                                    shr_fmm_t **last_field)
{
    shr_fmm_t *start;
    shr_fmm_t *current;
    uint32_t j;

    start = shr_fmm_alloc();
    if (!start) {
        return NULL;
    }
    start->id = field_id;
    start->idx = start_idx++;
    start->next = NULL;
    current = start;
    for (j = 1; j < num_of_elem; j++) {
        current->next = shr_fmm_alloc();
        if (!current->next) {
            while (start) {  /* Free whatever was allocated so far */
                current = start;
                start = start->next;
                shr_fmm_free(current);
            }
            return NULL;
        }
        current = current->next;
        current->id = field_id;
        current->idx = start_idx++;
        current->next = NULL;
    }
    *last_field = current;
    return start;
}

/*!
 * \brief Validate array field.
 *
 * This function validates that a given field is actually an array and that
 * other parameters associated with the array (such as start index) do not
 * conflict with the field attributes.
 *
 * \param [in] entry is the entry to add the field array to.
 * \param [in] field_name is the name of the field array.
 * \param [in] symbol indicates if the field is a symbol
 * \param [in] s_idx is the index where the current entries should start from
 * (within the array).
 * \param [in] n_of_elem indicates the number of elements to add to the array.
 * \param [out] f_id is the internal field ID of the field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_validate(bcmtrm_entry_t *entry,
                                const char *field_name,
                                bool symbol,
                                uint32_t s_idx,
                                uint32_t n_of_elem,
                                uint32_t *f_id)
{
    bcmlt_field_def_t *f_def;

    SHR_FUNC_ENTER(entry->info.unit);
    if (!field_name || (n_of_elem == 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                                            field_name,
                                            entry->db_hdl,
                                            &f_def,
                                            f_id));

    if (!(((f_def->depth > 0) && (f_def->depth >= s_idx+n_of_elem)) ||/*Array*/
        ((f_def->depth == 0) &&
         (f_def->elements >= s_idx+n_of_elem)) /* Scalar > 64b */ )) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                              "Field %s is not array or operation " \
                              "exceeding array boundary\n"),
                     field_name));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if ((!f_def->array) || (symbol != f_def->symbol)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Creates and insert field array in an entry.
 *
 * This function performs the actual validation, creation and insertion of
 * a field array into an entry. The function is data type agnostic and it uses
 * its inputs to set and assign the proper data type.
 *
 * \param [in] entry is the entry to add the field array to.
 * \param [in] field_name is the name of the field array.
 * \param [in] symbol indicates if the field is a symbol.
 * \param [in] s_idx is the index where the current entries should start from
 * (within the array).
 * \param [in] n_of_elem indicates the number of elements to add.
 * \param [in] data points to array of values.
 * \param [in] assign_f is a function to transfer values between field
 * presentation and user presentation.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_create(bcmtrm_entry_t *entry,
                              const char *field_name,
                              bool symbol,
                              uint32_t s_idx,
                              uint32_t n_of_elem,
                              void *data,
                              int (*assign_f)(bcmtrm_entry_t *entry,
                                               shr_fmm_t *fld,
                                               void **val_arry,
                                               bool to_field))
{
    int rv;
    uint32_t f_id;
    shr_fmm_t *prev_fld;
    shr_fmm_t *field;
    shr_fmm_t *s_list;   /* Start of the list */
    shr_fmm_t *elem;
    void *local_data = data;
    uint32_t actual_start_idx;
    uint32_t actual_end_idx;
    shr_fmm_t *last_field;
    bool end_of_list;

    rv = field_array_validate(entry, field_name, symbol,
                              s_idx, n_of_elem, &f_id);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    actual_start_idx = s_idx;
    actual_end_idx = s_idx + n_of_elem - 1;

    /* Search for the field with the same ID */
    if (entry->fld_arr) {
        field = entry->fld_arr[f_id];
    } else {
        field = bcmlt_find_field_in_entry(entry, f_id, &prev_fld);
    }
    prev_fld = NULL;
    if (field) {
        if (field->idx > actual_start_idx) {  /* If need to find the previous */
            bcmlt_find_field_in_entry(entry, f_id, &prev_fld);
        }

        /* This is the domain to focus the work on */
        end_of_list = false;
        while (actual_start_idx <= actual_end_idx) {
            /* Check if need to add the entry before the first one */
            if (field->idx > actual_start_idx) {
                elem = shr_fmm_alloc();
                if (!elem) {
                    return SHR_E_MEMORY;
                }
                elem->id = f_id;
                elem->idx = actual_start_idx++;
                rv = assign_f(entry, elem, &local_data, true);
                if (rv != SHR_E_NONE) {
                    return SHR_E_PARAM;
                }
                elem->next = field;
                if (prev_fld) {
                    prev_fld->next = elem;
                } else {
                    entry->l_field = elem;
                }
                prev_fld = elem;
                /* Update the array if needed */
                if (entry->fld_arr &&
                    entry->fld_arr[f_id]->idx >= actual_start_idx) {
                    entry->fld_arr[f_id] = elem;
                }
                continue;
            }
            if (field->idx == actual_start_idx) {
                rv = assign_f(entry, field, &local_data, true);
                if (rv != SHR_E_NONE) {
                    return SHR_E_PARAM;
                }
                actual_start_idx++;
                prev_fld = field;

                if (field->next && field->next->id == f_id) {
                    field = field->next;
                } else {
                    end_of_list = true;
                }
                continue;
            }
            if (field->idx < actual_start_idx) {
                if (end_of_list) {
                    elem = shr_fmm_alloc();
                    if (!elem) {
                        return SHR_E_MEMORY;
                    }
                    elem->id = f_id;
                    elem->idx = actual_start_idx++;
                    rv = assign_f(entry, elem, &local_data, true);
                    if (rv != SHR_E_NONE) {
                        return SHR_E_PARAM;
                    }
                    if (prev_fld) {
                        elem->next = prev_fld->next;
                        prev_fld->next = elem;
                    } else {
                        elem->next = NULL;
                        entry->l_field = elem;
                    }
                    prev_fld = elem;
                    continue;
                } else {
                    prev_fld = field;
                    if (field->next && field->next->id == f_id) {
                        field = field->next;
                    } else {
                        end_of_list = true;
                    }
                }
            }
        }
    } else {  /* Didn't find a field with matching ID. */
        /* Add the entire list into the array */
        s_list = field_array_alloc(actual_start_idx,
                                   actual_end_idx - actual_start_idx + 1,
                                   f_id,
                                   &last_field);
        if (!s_list) {
            return SHR_E_MEMORY;
        }

        /* Assign the data */
        for (field = s_list; field; field = field->next) {
            rv = assign_f(entry, field, &local_data, true);
            if (rv != SHR_E_NONE) {
                return SHR_E_PARAM;
            }
        }

        /* Add the array to the begining of the list */
        last_field->next = entry->l_field;
        entry->l_field = s_list;
        if (entry->fld_arr) {
            entry->fld_arr[f_id] = s_list;
        }
    }


    return SHR_E_NONE;
}

/*!
 * \brief Get values from field array.
 *
 * This function reads the values from a field array and copy them
 * into supplied array
 *
 * \param [in] entry is the entry to add the field array to.
 * \param [in] field_name is the name of the field array.
 * \param [in] symbol indicates if the field is a symbol.
 * \param [in] s_idx is the index in the field array where the entries
 * should be read from.
 * \param [in] n_of_elem indicates the number of elements to get.
 * \param [in] data points to empty array where the values should be copied to.
 * \param [out] r_elem_cnt contains the number of elements that were written
 * into the data array.
 * \param [in] assign_f is a function to transfer values between field
 * presentation and user presentation.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_get(bcmtrm_entry_t *entry,
                           const char *field_name,
                           bool symbol,
                           uint32_t s_idx,
                           uint32_t n_of_elem,
                           void *data,
                           uint32_t *r_elem_cnt,
                           int (*assign_f)(bcmtrm_entry_t *entry,
                                            shr_fmm_t *fld,
                                            void **val_arry,
                                            bool to_field))
{
    uint32_t f_id;
    bcmlt_field_def_t *f_def;
    shr_fmm_t *field;
    void *local_data = data;
    shr_fmm_t empty_field;

    SHR_FUNC_ENTER(entry->info.unit);
    if (!field_name || (n_of_elem == 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                                 field_name,
                                 entry->db_hdl,
                                 &f_def,
                                 &f_id));

    if ((!f_def->array) ||
        (symbol != f_def->symbol) ||
        ((f_def->depth == 0) && (f_def->elements == 1))) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Array or symbol mismatch\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Find the field array */
    if (entry->fld_arr) {
        field = entry->fld_arr[f_id];
    } else {
        field = bcmlt_find_field_in_entry(entry, f_id, NULL);
    }
    if (!field) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Field was not found in the entry\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    /* Find the start index in the array */
    while (field && (field->id == f_id) && (field->idx < s_idx)) {
        field = field->next;
    }

    /* Verify that indeed there is a good field */
    if (!field || (field->id != f_id) || (field->idx < s_idx) ||
        (field->idx >= s_idx + n_of_elem)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Field index was not found in the entry\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    /* Copy the data */
    *r_elem_cnt = 0;
    empty_field.data = f_def->def;
    while (field && (field->id == f_id) && (field->idx < s_idx + n_of_elem)) {
        /* If the index present in the fields copy the real data */
        if (*r_elem_cnt + s_idx == field->idx) {
            SHR_IF_ERR_EXIT(assign_f(entry, field, &local_data, false));
            field = field->next;
        } else {   /* Hole in the array, copy the default data */
            empty_field.id = field->id;
            SHR_IF_ERR_EXIT(assign_f(entry, &empty_field, &local_data, false));
        }
        (*r_elem_cnt)++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int bcmlt_entry_field_array_add(bcmlt_entry_handle_t entry_hdl,
                                    const char *field_name,
                                    uint32_t start_idx,
                                    uint64_t *data,
                                    uint32_t num_of_elem)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    SHR_IF_ERR_EXIT(field_array_create(entry, field_name, false,
                                       start_idx, num_of_elem,
                                       data, assign_u64));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       const char **data,
                                       uint32_t num_of_elem)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    /* Verify the symbol input */
    SHR_IF_ERR_VERBOSE_EXIT(verify_sym(entry, field_name, data, num_of_elem));

    SHR_IF_ERR_EXIT(field_array_create(entry, field_name, true,
                                       start_idx, num_of_elem,
                                       (void *)data, assign_symbol));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_get(bcmlt_entry_handle_t entry_hdl,
                                   const char *field_name,
                                   uint32_t start_idx,
                                   uint64_t *data,
                                   uint32_t num_of_elem,
                                   uint32_t *r_elem_cnt)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    SHR_IF_ERR_EXIT(field_array_get(entry, field_name, false,
                                    start_idx, num_of_elem, data,
                                    r_elem_cnt, assign_u64));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       const char **data,
                                       uint32_t num_of_elem,
                                       uint32_t *r_elem_cnt)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    SHR_IF_ERR_EXIT(field_array_get(entry, field_name, true,
                                    start_idx, num_of_elem, data,
                                    r_elem_cnt, assign_symbol));
exit:
    SHR_FUNC_EXIT();
}
