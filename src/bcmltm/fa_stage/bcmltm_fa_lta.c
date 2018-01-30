/*! \file bcmltm_fa_lta.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Logical Table Adaptors
 *
 * This file contains the LTM FA stage functions for LTA custom table
 * handlers.  The Logical Table Adaptor interface for custom tables
 * allows the implementation of a LT to be controlled solely through
 * custom logic.  This is intended for cases where the standard LTM
 * processing is too generic for the special case of this LT.
 *
 * The LTA interface provides a standardized method of passing the
 * LT fields and other necessary data to the custom logic.  The custom
 * table implementation is responsible for analyzing the fields,
 * producing the necessary PTM operations, and extracting any
 * return fields.
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
 * \brief Initialize a LTA field arrays within the Working Buffer.
 *
 * This function prepares the space assigned withing the Working Buffer
 * to manage LTA input or output field lists.
 * When the output argument is true, the field structures are cleared.
 * Input arguments expect the field structures to be initialized
 * by some other operation.
 *
 * \param [in] unit Unit number.
 * \param [in] output When TRUE, clear the field structures for a LTA
 *             output lists.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_lta_field_list_init(int unit,
                                   bool output,
                                   uint32_t *ltm_buffer,
                                   void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint32_t fix, field_count, max_fields;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_field_list->fields_ptrs_offset;
    lta_fields_ptrs = (bcmltd_field_t **)ptr_math;
    lta_field_maps = lta_field_list->field_maps;

    max_fields = lta_field_list->max_fields;
    lta_fields_parameter->count = 0;
    lta_fields_parameter->field = lta_fields_ptrs;

    if (output) {
        /* Initialize field structures for output field values. */
        field_count = 0;
        for (fix = 0; fix < max_fields; fix++) {
            cur_lta_map = &(lta_field_maps[fix]);
            ptr_math = ltm_buffer + cur_lta_map->field_offset;
            cur_lta_field = (bcmltd_field_t *)ptr_math;

            /*
             * Initialize the field structure
             */
            sal_memset(cur_lta_field, 0,
                       sizeof(bcmltd_field_t));

            lta_fields_ptrs[field_count] = cur_lta_field;
            field_count++;
        }
        lta_fields_parameter->count = field_count;
    }

    return rv;
}

/*!
 * \brief An FA tree node to convert API fields to LTA field arrays,
 *        with an option to append default field values.
 *
 * An FA tree node which converts the API fields to an array of
 * Logical Table Adapter fields.  Expected fields are required when
 * corresponding to key fields.  Otherwise, when the fields_default
 * parameter is true and the API field is not present in the input list,
 * the default value is added to the LTA list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 * \param [in] fields_required Indicates the mapped fields existence
 *             requirement choice.  One of BCMLTM_FIELDS_REQUIRED_*.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_fa_node_api_fields_to_lta_core(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie,
                                      bcmltm_fields_required_t fields_required)
{
    uint32_t fix, field_count, found_count, map_count, ltid;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data;
    const bcmlrd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;
    lta_field_maps = lta_field_list->field_maps;

    map_count = lta_field_list->num_maps;
    /* sal_assert(map_count <= lta_field_list->max_fields); */

    field_count = lta_fields_parameter->count;
    found_count = 0;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);
        ptr_math = ltm_buffer + cur_lta_map->field_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_lta_map->api_field_id,
                                               cur_lta_map->field_idx);
        if (api_field_data == NULL) {
            if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL) {
                /* LTA fields are required */
                tbl = bcmlrd_table_get(ltid);
                if ((tbl != NULL) &&
                    (cur_lta_map->api_field_id < tbl->fields )) {
                    /* Field validation failure */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                    "LTM %s: Missing field %s[%d]\n"),
                                 tbl->name,
                                 tbl->field[cur_lta_map->api_field_id].name,
                                 cur_lta_map->field_idx));
                }
                bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                       BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            } else if (fields_required == BCMLTM_FIELDS_REQUIRED_DEFAULT) {
                /* Missing LTA fields are supplied */
                sal_memset(cur_lta_field, 0, sizeof(bcmltd_field_t));
                cur_lta_field->id = cur_lta_map->api_field_id;
                cur_lta_field->idx = cur_lta_map->field_idx;
                cur_lta_field->data = cur_lta_map->default_value;
            } else {
                /* LTA fields are optional */
                continue;
            }
        } else {
            /* First, check that the supplied field value is legal. */
            if ((api_field_data->data < cur_lta_map->minimum_value) ||
                (api_field_data->data > cur_lta_map->maximum_value)) {
                /* Out of bounds field value */
                bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                       BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }

            /*
             * LTA field format is the same as the LTM field format.
             * We just need to copy from the WB location to the allocated
             * output field list.
             */
            sal_memcpy(cur_lta_field, api_field_data,
                       sizeof(bcmltd_field_t));
        }

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
        found_count++;
    }

    if (fields_required == BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE) {
        if (found_count == 0) {
            /* Record absence of fields in Working Buffer. */
            ltm_buffer[lta_field_list->index_absent_offset] =
                BCMLTM_INDEX_ABSENT;
        } else if (found_count != map_count) {
            tbl = bcmlrd_table_get(ltid);
            if (tbl != NULL) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "LTM %s: Incorrect number of key fields\n"),
                             tbl->name));
            }
            bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                                   BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT);
            return SHR_E_PARAM;
        }
        /* Else, standard all key fields present */
    }
    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_default_fields_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint32_t fix, field_count, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_field_list->fields_ptrs_offset;
    lta_fields_ptrs = (bcmltd_field_t **)ptr_math;
    lta_field_maps = lta_field_list->field_maps;

    map_count = lta_field_list->num_maps;
    /* sal_assert(map_count <= lta_field_list->max_fields); */
    field_count = lta_fields_parameter->count;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);
        ptr_math = ltm_buffer + cur_lta_map->field_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;

        /* LTA fields defaults */
        sal_memset(cur_lta_field, 0, sizeof(bcmltd_field_t));
        cur_lta_field->id = cur_lta_map->api_field_id;
        cur_lta_field->idx = cur_lta_map->field_idx;
        cur_lta_field->data = cur_lta_map->default_value;

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;
    lta_fields_parameter->field = lta_fields_ptrs;

    return rv;
}


extern int
bcmltm_fa_node_api_key_fields_to_lta(int unit,
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

    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                                 keys_required);
}

extern int
bcmltm_fa_node_api_alloc_key_fields_to_lta(int unit,
                                           bcmltm_lt_md_t *lt_md,
                                           bcmltm_lt_state_t *lt_state,
                                           bcmltm_entry_t *lt_entry,
                                           uint32_t *ltm_buffer,
                                           void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                      BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE);
}

extern int
bcmltm_fa_node_api_value_fields_to_lta(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                                 BCMLTM_FIELDS_REQUIRED_NONE);
}

extern int
bcmltm_fa_node_api_fields_default_to_lta(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie)
{
    return bcmltm_fa_node_api_fields_to_lta_core(unit,
                                                 lt_md,
                                                 lt_state,
                                                 lt_entry,
                                                 ltm_buffer,
                                                 node_cookie,
                                      BCMLTM_FIELDS_REQUIRED_DEFAULT);
}

extern int
bcmltm_fa_node_api_fields_update_to_lta(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie)
{
    uint32_t fix, field_count, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;
    bcmltm_field_list_t *api_field_data;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;
    lta_field_maps = lta_field_list->field_maps;

    map_count = lta_field_list->num_maps;
    /* sal_assert(map_count <= lta_field_list->max_fields); */

    field_count = lta_fields_parameter->count;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_lta_map->api_field_id,
                                               cur_lta_map->field_idx);
        if (api_field_data == NULL) {
            /* LTA fields are optional */
            continue;
        }

        cur_lta_field = bcmltm_lta_find_field(lta_fields_parameter,
                                              cur_lta_map->api_field_id,
                                              cur_lta_map->field_idx);
        if (cur_lta_field == NULL) { /* Missing expected field */
            
            ptr_math = ltm_buffer + cur_lta_map->field_offset;
            cur_lta_field = (bcmltd_field_t *)ptr_math;

            lta_fields_ptrs[field_count] = cur_lta_field;
            field_count++;
        }

        /* First, check that the supplied field value is legal. */
        if ((api_field_data->data < cur_lta_map->minimum_value) ||
            (api_field_data->data > cur_lta_map->maximum_value)) {
            /* Out of bounds field value */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        /*
         * Field in API input list, and in the LTA list produced
         * as output of the read during the first step of UPDATE.
         * Modify values per API request.
         */
        if ((api_field_data->flags & SHR_FMM_FIELD_DEL) == 0) {
            /*
             * LTA field format is the same as the LTM field format.
             * We just need to copy from the API input to the allocated
             * input field list.
             */
            sal_memcpy(cur_lta_field, api_field_data,
                       sizeof(bcmltd_field_t));
        } else {
            /* Unset request: revert LTA field to field default value. */
            cur_lta_field->data = cur_lta_map->default_value;
        }

        /* No update to the field pointers in the LTA list because
         * this field was already present in the list when found above. */
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_lta_output_init(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie)
{
    return bcmltm_fa_node_lta_field_list_init(unit, TRUE,
                                              ltm_buffer, node_cookie);
}

extern int
bcmltm_fa_node_lta_input_init(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie)
{
    return bcmltm_fa_node_lta_field_list_init(unit, FALSE,
                                              ltm_buffer, node_cookie);
}

extern int
bcmltm_fa_node_lta_to_api_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, field_count, mix, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint32_t *ptr_math;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    lta_fields_ptrs = lta_fields_parameter->field;
    field_count = lta_fields_parameter->count;

    /* Scan for existing output fields */
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
        if (lt_entry->flags & BCMLTM_ENTRY_FLAG_EXC_DEF) {
            cur_lta_map = NULL;

            
            for (mix = 0; mix < map_count; mix++) {
                if ((lta_field_maps[mix].api_field_id ==
                    lta_fields_ptrs[fix]->id) &&
                    (lta_field_maps[mix].field_idx ==
                     lta_fields_ptrs[fix]->idx)) {
                    cur_lta_map = &(lta_field_maps[mix]);
                    break;
                }
            }

            if (cur_lta_map == NULL) {
                /* Unknown field. */
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            if (lta_fields_ptrs[fix]->data ==
                cur_lta_map->default_value) {
                /* Do not add default value fields to output list */
                continue;
            }
        }

        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /*
         * LTA field format is the same as the LTM field format.
         * We just need to copy from the WB location to the allocated
         * output field list.
         */
        sal_memcpy(api_field_data, lta_fields_ptrs[fix],
                   sizeof(bcmltd_field_t));

        /* Add retrieve field value to entry */
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

extern int
bcmltm_fa_node_lta_to_api_alloc_fields(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie)
{
    uint32_t fix, field_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *api_field_data, *last_out_field;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    if (ltm_buffer[lta_field_list->index_absent_offset] !=
        BCMLTM_INDEX_ABSENT) {
        /*
         * No allocation took place, so no need to return
         * key fields mapped to index.
         */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_fields_ptrs = lta_fields_parameter->field;
    field_count = lta_fields_parameter->count;

    /* Scan for existing output fields */
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
        api_field_data = (lt_entry->field_alloc_cb)();
        SHR_NULL_CHECK(api_field_data, SHR_E_MEMORY);

        /*
         * LTA field format is the same as the LTM field format.
         * We just need to copy from the WB location to the allocated
         * output field list.
         */
        sal_memcpy(api_field_data, lta_fields_ptrs[fix],
                   sizeof(bcmltd_field_t));

        /* Add retrieve field value to entry */
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

extern int
bcmltm_fa_node_lta_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, field_count, map_count, max_fields;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs;
    bcmltd_fields_t *src_lta_fields_parameter, *lta_fields_parameter;
    bcmltm_field_list_t *lta_field;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer + lta_field_list->fields_parameter_offset;
    src_lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    ptr_math = ltm_buffer +
        lta_field_list->target_fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;
    lta_fields_ptrs = lta_fields_parameter->field;

    lta_field_maps = lta_field_list->field_maps;

    field_count = lta_fields_parameter->count;
    max_fields = lta_field_list->max_fields;

    map_count = lta_field_list->num_maps;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);

        lta_field = bcmltm_lta_find_field(src_lta_fields_parameter,
                                          cur_lta_map->api_field_id,
                                          cur_lta_map->field_idx);
        if (lta_field == NULL) { /* Is this optional? */
            continue;
        }

        if (field_count >= max_fields) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        lta_fields_ptrs[field_count] = lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_lta_fields_to_wbc(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t ltid, fix, map_count;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_field_list_t *lta_field;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map = NULL;
    uint32_t *ptr_math;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    const bcmlrd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer +
        lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);

        lta_field = bcmltm_lta_find_field(lta_fields_parameter,
                                          cur_lta_map->api_field_id,
                                          cur_lta_map->field_idx);
        if (lta_field == NULL) { /* Is this optional? */
            continue;
        }

        sal_memset(field_val, 0, sizeof(field_val));
        bcmltm_uint64_to_uint32_array(lta_field->data, field_val);

        /* Field validation check */
        if (!bcmltm_field_fit_check(&(cur_lta_map->wbc), field_val)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        bcmltm_field_value_into_wbc(&(cur_lta_map->wbc),
                                    field_val, ltm_buffer);

        /* Update PT entry changed flag if appropriate. */
        if (cur_lta_map->pt_changed_offset != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[cur_lta_map->pt_changed_offset] =
                BCMLTM_PT_ENTRY_CHANGED_VALUE;
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        ltid = lt_md->params->ltid;
        tbl = bcmlrd_table_get(ltid);
        if ((tbl != NULL) && (cur_lta_map != NULL) &&
            (cur_lta_map->api_field_id < tbl->fields )) {
            /* Field validation failure */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "LTM %s: Invalid field value 0x%08x%08x"
                                    " for field %s[%d]\n"),
                         tbl->name,
                         field_val[1], field_val[0],
                         tbl->field[cur_lta_map->api_field_id].name,
                         cur_lta_map->field_idx));
        }
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

extern int
bcmltm_fa_node_wbc_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie)
{
    uint32_t fix, field_count, map_count, max_fields;
    bcmltm_lta_field_list_t *lta_field_list;
    bcmltd_field_t **lta_fields_ptrs, *cur_lta_field;
    bcmltd_fields_t *lta_fields_parameter;
    bcmltm_lta_field_map_t *lta_field_maps, *cur_lta_map;
    uint32_t *ptr_math;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    lta_field_list = BCMLTM_FA_LTA_FIELD_LIST(node_cookie);

    ptr_math = ltm_buffer +
        lta_field_list->fields_parameter_offset;
    lta_fields_parameter = (bcmltd_fields_t *)ptr_math;

    lta_field_maps = lta_field_list->field_maps;
    map_count = lta_field_list->num_maps;

    field_count = lta_fields_parameter->count;
    max_fields = lta_field_list->max_fields;
    lta_fields_ptrs = lta_fields_parameter->field;

    for (fix = 0; fix < map_count; fix++) {
        cur_lta_map = &(lta_field_maps[fix]);
        ptr_math = ltm_buffer + cur_lta_map->field_offset;
        cur_lta_field = (bcmltd_field_t *)ptr_math;
        sal_memset(cur_lta_field, 0, sizeof(bcmltd_field_t));

        /* Is this memset required? */
        sal_memset(field_val, 0, sizeof(field_val));
        bcmltm_field_value_from_wbc(&(cur_lta_map->wbc),
                                    field_val, ltm_buffer);

        bcmltm_uint32_array_to_uint64(field_val, &(cur_lta_field->data));

        if (field_count >= max_fields) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        cur_lta_field->id = cur_lta_map->api_field_id;
        cur_lta_field->idx = cur_lta_map->field_idx;

        lta_fields_ptrs[field_count] = cur_lta_field;
        field_count++;
    }

    lta_fields_parameter->count = field_count;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_transform(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie)
{
    bcmltm_lta_transform_params_t *lta_transform_params;
    bcmltd_handler_transform_op_f lta_transform;
    bcmltd_transform_arg_t *lta_args;
    bcmltd_fields_t *in_fields, *out_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_transform_params = BCMLTM_LTA_TRANSFORM_PARAMS(node_cookie);
    if ((lta_transform_params->index_absent_offset !=
         BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[lta_transform_params->index_absent_offset] ==
         BCMLTM_INDEX_ABSENT)) {
        /* No index input fields provided, skip transform. */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    lta_transform = lta_transform_params->lta_transform;
    lta_args = lta_transform_params->lta_args;
    ptr_math = ltm_buffer + lta_transform_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;
    ptr_math = ltm_buffer + lta_transform_params->fields_output_offset;
    out_fields = (bcmltd_fields_t *)ptr_math;
    SHR_IF_ERR_VERBOSE_EXIT
        ((lta_transform)(unit,
                         in_fields,
                         out_fields,
                         lta_args));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_TRANSFORM_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_traverse_transform(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie)
{
    if ((lt_entry->flags & BCMLTM_ENTRY_FLAG_TRAVERSE_START) == 0) {
        return bcmltm_fa_node_lta_transform(unit,
                                            lt_md,
                                            lt_state,
                                            lt_entry,
                                            ltm_buffer,
                                            node_cookie);
    }
    /* Else, do not perform key fields forward transform on traverse start
     * because there are no key fields supplied.
     */

    return SHR_E_NONE;
}

int
bcmltm_fa_node_lta_pt_sid_map(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    bcmltm_lta_transform_params_t *lta_transform_params;
    bcmltd_transform_arg_t *lta_args;
    bcmltd_sid_t pt_select;

    SHR_FUNC_ENTER(unit);

    lta_transform_params = BCMLTM_LTA_TRANSFORM_PARAMS(node_cookie);
    lta_args = lta_transform_params->lta_args;

    if (lta_transform_params->pt_sid_selector_offset != BCMLTM_WB_OFFSET_INVALID) {
        /* Selector */
        pt_select = ltm_buffer[lta_transform_params->pt_sid_selector_offset];

        if (pt_select >= lta_args->tables) {
            /* Out of range */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        if (lta_transform_params->pt_sid_override_offset != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[lta_transform_params->pt_sid_override_offset] =
                lta_args->table[pt_select];
        } else {
            /* If a selector is provided, there should be space for
             * the PT SID value. */
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_fa_node_lta_validate(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie)
{
    bcmltm_lta_validate_params_t *lta_validate_params;
    bcmltd_handler_field_val_op_f lta_validate;
    bcmltd_field_val_arg_t *lta_args;
    bcmltd_fields_t *in_fields;
    uint32_t *ptr_math;

    SHR_FUNC_ENTER(unit);

    lta_validate_params = BCMLTM_LTA_VALIDATE_PARAMS(node_cookie);
    lta_validate = lta_validate_params->lta_validate;
    lta_args = lta_validate_params->lta_args;
    ptr_math = ltm_buffer + lta_validate_params->fields_input_offset;
    in_fields = (bcmltd_fields_t *)ptr_math;

    SHR_IF_ERR_VERBOSE_EXIT
        ((lta_validate)(unit,
                        lt_entry->opcode.lt_opcode,
                        in_fields,
                        lta_args));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_VALIDATE_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}
