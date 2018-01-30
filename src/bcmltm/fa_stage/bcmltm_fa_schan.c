/*! \file bcmltm_fa_schan.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Simple API field to/from HW SCHAN entry conversion
 *
 * This file contains the LTM FA stage functions for SCHAN entry
 * formation and decode.  These are the most common field conversion
 * methods.
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

/*
 * Working buffer layout for PT passthru
 */

/*
 * | uint32_t index |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] |
 */

#include <shr/shr_debug.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_FIELDADAPTATION


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Copy API fields into the Working Buffer.
 *
 * The internal implementation of FA nodes which copy API fields into
 * the Working Buffer.  May permit missing fields or not based on
 * the all_required parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 * \param [in] fields_required Indicates the mapped fields existence
 *             requirement choice.  One of BCMLTM_API_FIELDS_REQUIRED_*.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_api_fields_to_wb(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie,
                                bcmltm_fields_required_t fields_required)
{
    bool field_range_valid = TRUE;
    bool logical;
    uint32_t fix, field_count, found_count, ltid;
    bcmltm_ptm_field_list_t *field_list;
    bcmltm_ptm_field_map_t *field_maps_list, *cur_field_map = NULL;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data;
    bcmltm_field_stats_t field_err_stat;
    const bcmlrd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    /* Initialize in case of error */
    field_val[0] = field_val[1] = 0;

    ltid = lt_md->params->ltid;
    logical =
        (lt_entry->flags & BCMLTM_ENTRY_FLAG_PASSTHRU) ? FALSE : TRUE ;

    field_list = BCMLTM_FA_A2S_FIELD_LIST(node_cookie);
    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    found_count = 0;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_field_map->api_field_id,
                                               cur_field_map->field_idx);
        if (api_field_data == NULL) {  /* Value fields are optional */
            if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL ) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            } else {
                /* The WB schan entries should be set to default */
                continue;
            }
        }

        /* Check field range for Logical Tables, no PT PassThru. */
        if (logical &&
            ((api_field_data->data < cur_field_map->minimum_value) ||
             (api_field_data->data > cur_field_map->maximum_value))) {
            /* Out of bounds field value */
            field_range_valid = FALSE;
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        sal_memset(field_val, 0, sizeof(field_val));
        if ((fields_required == BCMLTM_FIELDS_REQUIRED_UNSET) &&
            ((api_field_data->flags & SHR_FMM_FIELD_DEL) != 0)) {
            bcmltm_uint64_to_uint32_array(cur_field_map->default_value,
                                          field_val);
        } else {
            bcmltm_uint64_to_uint32_array(api_field_data->data, field_val);
        }

        while (cur_field_map != NULL) {
            
            /* Field validation check */
            field_range_valid = FALSE;
            field_range_valid = bcmltm_field_fit_check(&(cur_field_map->wbc),
                                                 field_val);

            if (field_range_valid != TRUE) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            bcmltm_field_value_into_buffer(cur_field_map,
                                           field_val, ltm_buffer);

            /* Update PT entry changed flag if appropriate. */
            if (cur_field_map->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
                ltm_buffer[cur_field_map->pt_changed_offset] =
                    BCMLTM_PT_ENTRY_CHANGED_VALUE;
            }

            /* Next link in map list for this API field */
            cur_field_map = cur_field_map->next_map;
        }

        found_count++;
    }

    if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE) {
        if (found_count == 0) {
            /* Record absence of fields in Working Buffer. */
            ltm_buffer[field_list->index_absent_offset] =
                BCMLTM_INDEX_ABSENT;
        } else if (found_count != field_count) {
            tbl = bcmlrd_table_get(ltid);
            if (tbl != NULL) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "LTM %s: Incorrect number of key fields\n"),
                             tbl->name));
            }
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        /* Else, standard all key fields present */
    }

 exit:
    if (SHR_FUNC_ERR()) {
        tbl = bcmlrd_table_get(ltid);
        if ((tbl != NULL) && (cur_field_map != NULL) &&
            (cur_field_map->api_field_id < tbl->fields )) {
            /* Field validation failure */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "LTM %s: %s field 0x%08x%08x"
                                    " for field %s[%d]\n"),
                         tbl->name,
                         field_range_valid ? "Missing" : "Out of bounds",
                         field_val[1], field_val[0],
                         tbl->field[cur_field_map->api_field_id].name,
                         cur_field_map->field_idx));
        }
        field_err_stat = field_range_valid ?
            BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT :
            BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT;

        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               field_err_stat);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_api_key_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    bcmltm_fields_required_t keys_required;

    keys_required =
        (lt_entry->flags & BCMLTM_ENTRY_FLAG_TRAVERSE_START) ?
        BCMLTM_FIELDS_REQUIRED_NONE :
        BCMLTM_FIELDS_REQUIRED_ALL;

    return bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                           lt_entry, ltm_buffer,
                                           node_cookie,
                                           keys_required);
}

int
bcmltm_fa_node_api_alloc_key_fields_to_wb(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                           lt_entry, ltm_buffer,
                                           node_cookie,
                              BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE);
}

int
bcmltm_fa_node_api_value_fields_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                           lt_entry, ltm_buffer,
                                           node_cookie,
                                           BCMLTM_FIELDS_REQUIRED_NONE);
}

int
bcmltm_fa_node_api_fields_unset_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_wb(unit, lt_md, lt_state,
                                           lt_entry, ltm_buffer,
                                           node_cookie,
                                           BCMLTM_FIELDS_REQUIRED_UNSET);
}

int
bcmltm_fa_node_wb_to_api_fields(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_ptm_field_list_t *field_list;
    bcmltm_ptm_field_map_t *field_maps_list, *cur_field_map;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint64_t field_val64;

    SHR_FUNC_ENTER(unit);

    field_list = BCMLTM_FA_A2S_FIELD_LIST(node_cookie);
    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    last_out_field = lt_entry->out_fields;
    if (last_out_field != NULL) {
        while (last_out_field->next != NULL) {
            last_out_field = last_out_field->next;
        }
        /* Traverse out fields to end of list for returning data
         * While the entry structure provided to the LTM
         * should have no output fields in the list, this permits
         * multiple data return nodes in a read op tree.
         */
    }

    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        sal_memset(field_val, 0, sizeof(field_val));
        bcmltm_field_value_from_buffer(cur_field_map,
                                       field_val, ltm_buffer);

        /* @@@ Field validation check here @@@ */
        bcmltm_uint32_array_to_uint64(field_val, &field_val64);

        if (lt_entry->flags & BCMLTM_ENTRY_FLAG_EXC_DEF) {
            if (field_val64 == cur_field_map->default_value) {
                /* Do not add default value fields to output list */
                continue;
            }
        }

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add retrieve field value to entry */
        api_field_data->data = field_val64;
        api_field_data->id = cur_field_map->api_field_id;
        api_field_data->idx = cur_field_map->field_idx;
        api_field_data->flags = 0; /* Flags only set by APP layer */
        api_field_data->next = NULL;
        if (last_out_field == NULL) {
            lt_entry->out_fields = api_field_data;
        } else {
            last_out_field->next = api_field_data;
        }
        last_out_field = api_field_data;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_wb_to_api_alloc_fields(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_ptm_field_list_t *field_list;
    bcmltm_ptm_field_map_t *field_maps_list, *cur_field_map;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint64_t field_val64;

    SHR_FUNC_ENTER(unit);

    field_list = BCMLTM_FA_A2S_FIELD_LIST(node_cookie);
    if (ltm_buffer[field_list->index_absent_offset] !=
        BCMLTM_INDEX_ABSENT) {
        /*
         * No allocation took place, so no need to return
         * key fields mapped to index.
         */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    last_out_field = lt_entry->out_fields;
    if (last_out_field != NULL) {
        while (last_out_field->next != NULL) {
            last_out_field = last_out_field->next;
        }
        /* Traverse out fields to end of list for returning data
         * While the entry structure provided to the LTM
         * should have no output fields in the list, this permits
         * multiple data return nodes in a read op tree.
         */
    }

    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        sal_memset(field_val, 0, sizeof(field_val));
        bcmltm_field_value_from_buffer(cur_field_map,
                                       field_val, ltm_buffer);

        /* @@@ Field validation check here @@@ */
        bcmltm_uint32_array_to_uint64(field_val, &field_val64);

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /* Add retrieve field value to entry */
        api_field_data->data = field_val64;
        api_field_data->id = cur_field_map->api_field_id;
        api_field_data->idx = cur_field_map->field_idx;
        api_field_data->flags = 0; /* Flags only set by APP layer */
        api_field_data->next = NULL;
        if (last_out_field == NULL) {
            lt_entry->out_fields = api_field_data;
        } else {
            last_out_field->next = api_field_data;
        }
        last_out_field = api_field_data;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_fixed_fields_to_wb(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_fixed_field_t *cur_fixed, *field_list;
    bcmltm_fixed_field_list_t *fixed_field_data;

    fixed_field_data = BCMLTM_FA_FIXED_FIELD_LIST(node_cookie);
    field_count = fixed_field_data->num_fixed_fields;
    field_list = fixed_field_data->fixed_fields;

    for (fix = 0; fix < field_count; fix++) {
        cur_fixed = &(field_list[fix]);

        /* Should we add an assert that (maxbit - minbit +1) <= 32? */
        bcmltm_field_value_into_wbc(&(cur_fixed->wbc),
                                    &(cur_fixed->field_value),
                                    ltm_buffer);
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_delete_fixed_fields_to_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_fixed_field_t *cur_fixed, *field_list;
    bcmltm_fixed_field_list_t *fixed_field_data;

    fixed_field_data = BCMLTM_FA_FIXED_FIELD_LIST(node_cookie);
    field_count = fixed_field_data->num_fixed_fields;
    field_list = fixed_field_data->fixed_fields;

    for (fix = 0; fix < field_count; fix++) {
        cur_fixed = &(field_list[fix]);

        /* Should we add an assert that (maxbit - minbit +1) <= 32? */
        bcmltm_field_value_into_wbc(&(cur_fixed->wbc),
                                    &(cur_fixed->delete_value),
                                    ltm_buffer);
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_default_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_ptm_field_list_t *field_list;
    bcmltm_ptm_field_map_t *field_maps_list, *cur_field_map;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];

    field_list = BCMLTM_FA_A2S_FIELD_LIST(node_cookie);
    field_count = field_list->num_fields;
    field_maps_list = field_list->field_maps;
    for (fix = 0; fix < field_count; fix++) {
        cur_field_map = &(field_maps_list[fix]);

        /*
         * Default value must be contained in the first field map of
         * of the field map chain.
         */
        field_val[0] = cur_field_map->default_value & 0xffffffff;
        field_val[1] = (cur_field_map->default_value >> 32) & 0xffffffff;

        while (cur_field_map != NULL) {
            bcmltm_field_value_into_buffer(cur_field_map,
                                           field_val, ltm_buffer);

            /* Update PT entry changed flag if appropriate. */
            if (cur_field_map->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
                ltm_buffer[cur_field_map->pt_changed_offset] =
                    BCMLTM_PT_ENTRY_CHANGED_VALUE;
            }

            /* Next link in map list for this API field */
            cur_field_map = cur_field_map->next_map;
        }
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_wbc_to_wbc(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie)
{
    uint32_t wix, wbc_count;
    bcmltm_wb_copy_list_t *wbc_list;
    bcmltm_wb_copy_t *wb_copy_list, *cur_wb_copy;
    uint32_t wbc_val[BCMDRD_MAX_PT_WSIZE];

    wbc_list = BCMLTM_FA_WB_COPY_LIST(node_cookie);
    wbc_count = wbc_list->num_wb_copy;
    wb_copy_list = wbc_list->wb_copy;
    for (wix = 0; wix < wbc_count; wix++) {
        cur_wb_copy = &(wb_copy_list[wix]);

        /* Retrieve value from source WBC */
        bcmltm_field_value_from_wbc(&(cur_wb_copy->wbc_source),
                                    wbc_val, ltm_buffer);

        /* Set value at destination WBC */
        bcmltm_field_value_into_wbc(&(cur_wb_copy->wbc_destination),
                                    wbc_val, ltm_buffer);
    }

    return SHR_E_NONE;
}

int
bcmltm_fa_node_alloc_wbc_to_wbc(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    bcmltm_wb_copy_list_t *wbc_list;

    wbc_list = BCMLTM_FA_WB_COPY_LIST(node_cookie);
    if (ltm_buffer[wbc_list->index_absent_offset] != BCMLTM_INDEX_ABSENT) {
        /* Nothing to do */
        return SHR_E_NONE;
    }

    /* Otherwise, copy allocatable key fields to necessary destinations. */
    return bcmltm_fa_node_wbc_to_wbc(unit, lt_md, lt_state,
                                     lt_entry, ltm_buffer, node_cookie);
}
