/*! \file bcmltm_ee_config.c
 *
 * Logical Table Manager Execution Engine.
 *
 * Configuration and stats.
 *
 * This file contains the LTM EE stage functions for LT table
 * statistics and configuration parameters. LT statistics are
 * counters of table operations that are not preserved over HA events.
 * LT configurations are parameters which may be modified at runtime
 * (though often only on an empty table) or statistics which _are_
 * preserved over HA events.
 *
 * These values reach into the core of the LTM data structure, and so are
 * not suitable for LTA dispatch.
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
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_EXECUTIONENGINE


typedef enum bcmltm_field_config_e {
    BCMLRD_FIELD_INDEX_MIN_LIMIT = 0,
    BCMLRD_FIELD_INDEX_MAX_LIMIT = 1,
    BCMLRD_FIELD_INDEX_MIN = 2,
    BCMLRD_FIELD_INDEX_MAX = 3,
    BCMLRD_FIELD_ENTRY_COUNT = 4,
    BCMLRD_FIELD_ALLOC_COUNT = 5,
    BCMLRD_FIELD_LT_OP_PT_INFO_ENABLE = 6,
    BCMLRD_FIELD_CONFIG_NUM = 7
} bcmltm_field_config_t;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Validate new LT index range
 *
 * When the TABLE_CONTROL LT is used to change the operating index
 * range, this function checks for any in-use or allocated indexes
 * that would be left inaccessible by the change.  It will reject such
 * a change.  The application should delete/release such indexes
 * before attempting the range modification.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Target Logical Table ID.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] is_max Boolean indicator of the new index boundary.
 *                    TRUE - new index maximum.
 *                    FALSE - new index minimum.
 * \param [in] new_index The new index boundary for this LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Existing entries outside of the new range.
 */
static int
bcmltm_index_range_change_check(int unit,
                                uint32_t ltid,
                                bcmltm_lt_state_t *lt_state,
                                bool is_max,
                                uint32_t new_index)
{
    uint32_t range_low, range_hi;
    uint64_t range_limit_low, range_limit_hi;
    uint32_t mem_param_min, mem_param_max, cur_mp;
    uint32_t index_multiplier, index_offset;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_info_lt_get(unit, ltid,
                            TABLE_INFOt_INDEX_MAX_LIMITf, 0,
                            &range_limit_hi));
    SHR_IF_ERR_EXIT
        (bcmltm_info_lt_get(unit, ltid,
                            TABLE_INFOt_INDEX_MIN_LIMITf, 0,
                            &range_limit_low));

    if (is_max) {
        if (new_index > range_limit_hi) {
            /* Beyond valid range of maximum index */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        } else {
            if (new_index < lt_state->current_index_max) {
                /* Decreasing max index, sanity checks */
                if (new_index < lt_state->current_index_min) {
                    /* Max less than min, reject */
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                } else {
                    /* Check for orphaned entries */
                    range_low = new_index + 1;
                    range_hi = lt_state->current_index_max;
                }
            } else {
                /* Increasing maximum index */
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    } else {
        if (new_index < range_limit_low) {
            /* Beyond valid range of minimum index */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        } else {
            if (new_index > lt_state->current_index_min) {
                /* Increasing min index, sanity checks */
                if (new_index >
                    lt_state->current_index_max) {
                    /* Max less than min, reject */
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                } else {
                    /* Check for orphaned entries */
                    range_low = lt_state->current_index_min;
                    range_hi = new_index - 1;
                }
            } else {
                /* Decreasing minimum index */
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    }

    mem_param_min = lt_state->current_param_min;
    mem_param_max = lt_state->current_param_max;
    index_multiplier = range_limit_hi + 1;

    /* In-use bitmap check for changing range */
    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    for (cur_mp = mem_param_min; cur_mp <= mem_param_max; cur_mp++) {
        index_offset = cur_mp * index_multiplier;
        if (!SHR_BITNULL_RANGE(inuse_bitmap, range_low + index_offset,
                               (range_hi - range_low + 1))) {
            /* Index limit change will orphan existing entry. */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate new LT param range
 *
 * When the TABLE_CONTROL LT is used to change the operating param
 * range, this function checks for any in-use or allocated parames
 * that would be left inaccessible by the change.  It will reject such
 * a change.  The application should delete/release such parames
 * before attempting the range modification.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Target Logical Table ID.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] is_max Boolean indicator of the new param boundary.
 *                    TRUE - new param maximum.
 *                    FALSE - new param minimum.
 * \param [in] new_param The new param boundary for this LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Existing entries outside of the new range.
 */
static int
bcmltm_param_range_change_check(int unit,
                                uint32_t ltid,
                                bcmltm_lt_state_t *lt_state,
                                bool is_max,
                                uint32_t new_param)
{
    uint64_t range_limit_low, range_limit_hi;
    uint64_t range_mp_low, range_mp_hi;
    uint32_t mp_low, mp_hi, cur_mp;
    uint32_t index_multiplier, index_offset;
    SHR_BITDCL *inuse_bitmap;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_info_lt_get(unit, ltid,
                            TABLE_INFOt_INDEX_MAX_LIMITf, 0,
                            &range_limit_hi));
    SHR_IF_ERR_EXIT
        (bcmltm_info_lt_get(unit, ltid,
                            TABLE_INFOt_INDEX_MIN_LIMITf, 0,
                            &range_limit_low));
    SHR_IF_ERR_EXIT
        (bcmltm_info_lt_get(unit, ltid,
                            TABLE_INFOt_PARAM_MAX_LIMITf, 0,
                            &range_mp_hi));
    SHR_IF_ERR_EXIT
        (bcmltm_info_lt_get(unit, ltid,
                            TABLE_INFOt_PARAM_MIN_LIMITf, 0,
                            &range_mp_low));

    if (is_max) {
        if (new_param > range_mp_hi) {
            /* Beyond valid range of maximum param */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        } else {
            if (new_param < lt_state->current_param_max) {
                /* Decreasing max param, sanity checks */
                if (new_param < lt_state->current_param_min) {
                    /* Max less than min, reject */
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                } else {
                    /* Check for orphaned entries */
                    mp_low = new_param + 1;
                    mp_hi = lt_state->current_param_max;
                }
            } else {
                /* Increasing maximum param */
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    } else {
        if (new_param < range_mp_low) {
            /* Beyond valid range of minimum param */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        } else {
            if (new_param > lt_state->current_param_min) {
                /* Increasing min param, sanity checks */
                if (new_param >
                    lt_state->current_param_max) {
                    /* Max less than min, reject */
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                } else {
                    /* Check for orphaned entries */
                    mp_low = lt_state->current_param_min;
                    mp_hi = new_param - 1;
                }
            } else {
                /* Decreasing minimum param */
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    }

    index_multiplier = range_limit_hi + 1;

    /* In-use bitmap check for changing range */
    SHR_IF_ERR_EXIT
        (bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_INUSE_BITMAP,
                               (void *) &inuse_bitmap));

    /* Check each of the newly unavailable MP values to verify no
     * orphaned entries in the complete range. */
    for (cur_mp = mp_low; cur_mp <= mp_hi; cur_mp++) {
        index_offset = cur_mp * index_multiplier;
        if (!SHR_BITNULL_RANGE(inuse_bitmap, range_limit_low + index_offset,
                               (range_limit_hi - range_limit_low + 1))) {
            /* Index limit change will orphan existing entry. */
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve the value of a LT_STATS field.
 *
 * Internal function to recover a per-LT statistic value.
 *
 * \param [in] lt_stats Pointer to the statistics array for this LT.
 * \param [in] field_id The ID number for this LT_CONFIG field.
 * \param [in] field_idx The array index for this LT_CONFIG field.
 * \param [in] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_stats_read(uint32_t *lt_stats,
                  uint32_t field_id,
                  uint32_t field_idx,
                  uint64_t *data_value)
{
    if (field_id < BCMLRD_FIELD_STATS_NUM) {
        *data_value = lt_stats[field_id];
    } else {
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Assign the value of a LT_STATS field.
 *
 * Internal function to set a per-LT statistic value.
 *
 * \param [in] lt_stats Pointer to the statistics array for this LT.
 * \param [in] field_id The ID number for this LT_CONFIG field.
 * \param [in] field_idx The array index for this LT_CONFIG field.
 * \param [in] data_value The uint64_t pointer to provide the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_stats_write(uint32_t *lt_stats,
                   uint32_t field_id,
                   uint32_t field_idx,
                   uint64_t data_value)
{
    if (field_id < BCMLRD_FIELD_STATS_NUM) {
        lt_stats[field_id]= data_value & 0xffffffff;
    } else {
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Set the value of a TABLE_CONTROL field.
 *
 * Internal function to set the value of a per-LT
 * configuration in the HA-protected storage location.  Also
 * implements the configuration change.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Target Logical Table ID.
 * \param [in] lt_state Pointer to the state for the target LT.
 * \param [in] field_id The ID number for this TABLE_CONTROL field.
 * \param [in] field_idx The array index for this TABLE_CONTROL field.
 * \param [in] data_value The uint64_t value for this control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_control_write(int unit,
                     uint32_t ltid,
                     bcmltm_lt_state_t *lt_state,
                     uint32_t field_id,
                     uint32_t field_idx,
                     uint64_t data_value)
{
    SHR_FUNC_ENTER(unit);

    switch (field_id) {
    case TABLE_CONTROLt_INDEX_MINf:
        SHR_IF_ERR_EXIT
            (bcmltm_index_range_change_check(unit, ltid, lt_state,
                                             FALSE, data_value));
        lt_state->current_index_min = data_value;
        break;
    case TABLE_CONTROLt_INDEX_MAXf:
        SHR_IF_ERR_EXIT
            (bcmltm_index_range_change_check(unit, ltid, lt_state,
                                             TRUE, data_value));
        lt_state->current_index_max = data_value;
        break;
    case TABLE_CONTROLt_PARAM_MINf:
        SHR_IF_ERR_EXIT
            (bcmltm_param_range_change_check(unit, ltid, lt_state,
                                             FALSE, data_value));
        lt_state->current_param_min = data_value;
        break;
    case TABLE_CONTROLt_PARAM_MAXf:
        SHR_IF_ERR_EXIT
            (bcmltm_param_range_change_check(unit, ltid, lt_state,
                                             TRUE, data_value));
        lt_state->current_param_max = data_value;
        break;
    case TABLE_CONTROLt_TABLE_OP_PT_INFOf:
        lt_state->table_op_pt_info_enable =
            (data_value != 0) ? TRUE : FALSE;
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the value of a TABLE_CONTROL field.
 *
 * Internal function to recover the current value of a per-LT
 * control value for the HA-protected storage location.
 *
 * \param [in] lt_state Pointer to the metadata for the target LT.
 * \param [in] field_id The ID number for this TABLE_CONTROL field.
 * \param [in] field_idx The array index for this TABLE_CONTROL field.
 * \param [in] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_control_read(bcmltm_lt_state_t *lt_state,
                    uint32_t field_id,
                    uint32_t field_idx,
                    uint64_t *data_value)
{
    switch (field_id) {
    case TABLE_CONTROLt_INDEX_MINf:
        *data_value = lt_state->current_index_min;
        break;
    case TABLE_CONTROLt_INDEX_MAXf:
        *data_value = lt_state->current_index_max;
        break;
    case TABLE_CONTROLt_PARAM_MINf:
        *data_value = lt_state->current_param_min;
        break;
    case TABLE_CONTROLt_PARAM_MAXf:
        *data_value = lt_state->current_param_max;
        break;
    case TABLE_CONTROLt_TABLE_OP_PT_INFOf:
        *data_value = lt_state->table_op_pt_info_enable;
        break;
    default:
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_ee_node_lt_table_write(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint32_t fs_ix;
    bcmltm_field_list_t *api_field_data;
    bcmltm_field_spec_list_t *fs_list;
    bcmltm_field_spec_t *cur_fs;
    uint32_t target_ltid;
    bcmltm_lt_state_t *target_lt_state = NULL;
    uint32_t *target_lt_stats = NULL;

    SHR_FUNC_ENTER(unit);

    fs_list = BCMLTM_EE_FIELD_SPEC_LIST(node_cookie);
    target_ltid = ltm_buffer[fs_list->ltid_offset];

    if (lt_entry->table_id == TABLE_STATSt) {
        /* LT stats */
        SHR_IF_ERR_EXIT
            (bcmltm_stats_lt_get(unit, target_ltid, &target_lt_stats));
    } else {
        /* Dynamic LT state */
        SHR_IF_ERR_EXIT
            (bcmltm_state_lt_get(unit, target_ltid, &target_lt_state));
    }

    for (fs_ix = 0; fs_ix < fs_list->num_field_specs; fs_ix++) {
        cur_fs = &(fs_list->field_specs[fs_ix]);

        api_field_data = bcmltm_api_find_field(lt_entry->in_fields,
                                               cur_fs->field_id,
                                               cur_fs->field_idx);
        if (api_field_data == NULL) {  /* Value fields are optional */
            continue;  /* No configuration change required */
        }

        if (lt_entry->table_id == TABLE_CONTROLt) {
            rv = bcmltm_control_write(unit,
                                      target_ltid,
                                      target_lt_state,
                                      cur_fs->field_id,
                                      cur_fs->field_idx,
                                      api_field_data->data);
        } else if (lt_entry->table_id == TABLE_STATSt) {
            rv = bcmltm_stats_write(target_lt_stats,
                                    cur_fs->field_id,
                                    cur_fs->field_idx,
                                    api_field_data->data);
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
        if (rv == SHR_E_UNAVAIL) {
            /* This configuration is not implemeneted for this LT. Skip. */
            continue;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_lt_table_read(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie)
{
    int rv = SHR_E_NONE;
    uint64_t val64 = 0;
    uint32_t fs_ix;
    bcmltm_field_list_t *api_field_data, *last_out_field;
    bcmltm_field_spec_list_t *fs_list;
    bcmltm_field_spec_t *cur_fs;
    uint32_t target_ltid;
    bcmltm_lt_state_t *target_lt_state = NULL;
    uint32_t *target_lt_stats = NULL;

    SHR_FUNC_ENTER(unit);

    fs_list = BCMLTM_EE_FIELD_SPEC_LIST(node_cookie);
    target_ltid = ltm_buffer[fs_list->ltid_offset];

    if (lt_entry->table_id == TABLE_STATSt) {
        /* LT stats */
        SHR_IF_ERR_EXIT
            (bcmltm_stats_lt_get(unit, target_ltid, &target_lt_stats));
    } else {
        /* Dynamic LT state */
        SHR_IF_ERR_EXIT
            (bcmltm_state_lt_get(unit, target_ltid, &target_lt_state));
    }

    last_out_field = NULL;

    for (fs_ix = 0; fs_ix < fs_list->num_field_specs; fs_ix++) {
        cur_fs = &(fs_list->field_specs[fs_ix]);

        if (lt_entry->table_id == TABLE_INFOt) {
            rv = bcmltm_info_lt_get(unit, target_ltid,
                                    cur_fs->field_id,
                                    cur_fs->field_idx, &val64);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
        } else if (lt_entry->table_id == TABLE_CONTROLt) {
            rv = bcmltm_control_read(target_lt_state,
                                     cur_fs->field_id,
                                     cur_fs->field_idx, &val64);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
        } else if (lt_entry->table_id == TABLE_STATSt) {
            rv = bcmltm_stats_read(target_lt_stats, cur_fs->field_id,
                                   cur_fs->field_idx, &val64);
        } else {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        if (rv == SHR_E_UNAVAIL) {
            /* This configuration is not implemeneted for this LT. Skip. */
            continue;
        }

        api_field_data = (lt_entry->field_alloc_cb)();
        if (api_field_data == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }

        /* Add retrieved field value to entry */
        api_field_data->id = cur_fs->field_id;
        api_field_data->idx = cur_fs->field_idx;
        api_field_data->data = val64;
        api_field_data->flags = 0;
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


