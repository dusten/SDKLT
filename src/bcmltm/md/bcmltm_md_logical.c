/*! \file bcmltm_md_logical.c
 *
 * Logical Table Manager for Logical Table Metadata.
 *
 * The tables in this category are those defined through
 * various methods to produce a mapping from the API fields to
 * either the physical resources on the device or an internal
 * database.
 *
 * The Logical Table metadata uses the symbol IDs from the
 * LRD component, Logical Resource Database.  These symbol IDs
 * are the identification for all the defined logical tables.
 *
 * The LRD symbol IDs are defined in a compact numbering
 * space [0.. n-1], where 'n' is the total number of LRD symbols.
 *
 * The design of this component is based on the LRD symbol ID attribute
 * define above.   The symbol ID is used as the array index into
 * several of the table metadata array data structure.
 *
 * The LRD symbol ID value is used as the LTM table ID.
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

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmbd/bcmbd.h>

#include <bcmptm/bcmptm.h>

#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/chip/bcmlrd_limits.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_ha_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include <bcmltm/bcmltm_md_logical_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>

#include "bcmltm_md_lt_drv.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*
 * Accessor macros
 */
#define LOGICAL_LTM_MD(_u)               logical_ltm_md[_u]
#define LOGICAL_SID_MAX_COUNT(_u)        LOGICAL_LTM_MD(_u)->lt_max
#define LOGICAL_HA_SIZES(_u)             ha_state_sizes[_u]


/*
 * Helper macros
 */
/* Check that LTM has been initialized */
#define LOGICAL_INIT(_u)                        \
    (LOGICAL_LTM_MD(_u) != NULL)

/* Check that symbold id is valid */
#define LOGICAL_SID_VALID(_u, _sid)             \
    ((_sid) < LOGICAL_SID_MAX_COUNT(_u))


/* LTM LT Metadata */
static bcmltm_md_t    *logical_ltm_md[BCMLTM_MAX_UNITS];

/* LTM HA state sizes */
static bcmltm_ha_state_sizes_t ha_state_sizes[BCMLTM_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the size for the key boundary information.
 *
 * This routine gets the size (bytes) required to store
 * the Index LT key field value boundary information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval Size in bytes required to store the key boundary information.
 */
static uint32_t
logical_key_bounds_size_get(int unit, bcmlrd_sid_t sid)
{
    int rv;
    uint32_t num_fid;
    uint32_t num_fid_idx;
    uint32_t key_bounds_size = 0;

    rv = bcmltm_lt_field_key_count_get(unit, sid, TRUE,
                                       &num_fid, &num_fid_idx);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    key_bounds_size = sizeof(bcmltm_index_key_bounds_t) +
        (sizeof(bcmltm_key_bound_info_t) * num_fid_idx);

    return key_bounds_size;
}


/*!
 * \brief Initialize the key boundary information for a logical table.
 *
 * This routine initializes the key boundary information with
 * the key fields minimum and maximum limits as specified
 * in table map file for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] key_bounds Key boundary information to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_key_bounds_init(int unit, bcmlrd_sid_t sid,
                        bcmltm_index_key_bounds_t *key_bounds)
{
    bcmltm_key_bound_info_t *key_info;
    uint32_t num_fid;
    uint32_t num_fid_idx;
    uint32_t fid;
    uint32_t fid_count;
    uint32_t idx_count;
    unsigned int size;
    bcmltm_fid_idx_t *fid_idx = NULL;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;
    uint32_t i;
    uint32_t idx;
    uint32_t key_idx;

    SHR_FUNC_ENTER(unit);

    key_bounds->num_keys = 0;

    /* Get count of key field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_lt_field_key_count_get(unit, sid, TRUE,
                                       &num_fid, &num_fid_idx));
    if (num_fid_idx == 0) {
        SHR_EXIT();
    }

    /* Allocate field id/idx array */
    size = sizeof(*fid_idx) * num_fid;
    SHR_ALLOC(fid_idx, size, "LTM Key Field id_idx list");
    SHR_NULL_CHECK(fid_idx, SHR_E_MEMORY);
    sal_memset(fid_idx, 0, size);

    /* Get list of key field IDs and index count */
    SHR_IF_ERR_EXIT
        (bcmltm_lt_field_key_list_get(unit, sid, TRUE,
                                      num_fid, fid_idx, &fid_count));

    /* Populate key bounds information */
    key_info = key_bounds->key_bounds;
    for (i = key_idx = 0; i < fid_count; i++) {
        fid = fid_idx[i].fid;
        idx_count = fid_idx[i].idx_count;

        size = sizeof(*minimum_value) * idx_count;
        SHR_ALLOC(minimum_value, size, "LTM field minimum values");
        SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
        sal_memset(minimum_value, 0, size);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_min_get(unit, sid, fid,
                                  idx_count, minimum_value, &minimum_count));
        if (idx_count != minimum_count) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid index count for "
                                  "field minimums "
                                  "sid=%d fid=%d "
                                  "expected=%d actual=%d\n"),
                       sid, fid, idx_count, minimum_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW maximum value for field */
        size = sizeof(*maximum_value) * idx_count;
        SHR_ALLOC(maximum_value, size, "LTM field maximum values");
        SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);
        sal_memset(maximum_value, 0, size);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_max_get(unit, sid, fid,
                                  idx_count, maximum_value, &maximum_count));
        if (idx_count != maximum_count) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid index count for "
                                  "field maximums "
                                  "sid=%d fid=%d "
                                  "expected=%d actual=%d\n"),
                       sid, fid, idx_count, maximum_count));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        for (idx = 0; idx < idx_count; idx++) {
            if (key_idx >= num_fid_idx) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM Custom Table Handler: current "
                                      "field count exceeds max count sid=%d "
                                      "count=%d max_count=%d\n"),
                           sid, (i+1), num_fid_idx));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }

            key_info[i].api_field_id = fid;
            key_info[i].field_idx = idx;
            key_info[i].current_key_min = minimum_value[idx];
            key_info[i].current_key_max = maximum_value[idx];

            key_idx++;
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
    }

    /* Set number of key elements populated */
    key_bounds->num_keys = key_idx;

 exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);
    SHR_FREE(fid_idx);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Reset the state data for given logical table.
 *
 * This routine resets the various state data types for
 * the specified logical table to the initial state during cold boot.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Table ID.
 * \param [in] lt_state LT state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_data_reset(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_state_t *lt_state)
{
    int rv;
    bcmltm_index_key_bounds_t *key_bounds = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Initialize the various state data types.
     * The order of initialization of the different state data
     * is important since there are dependencies among some of them.
     */

    /*
     * State Data: BCMLTM_STATE_TYPE_INUSE_BITMAP
     *             BCMLTM_STATE_TYPE_GLOBAL_BITMAP
     *
     * The initial state should be zero during a cold boot.
     * Since the state data is reset to zero when this is added,
     * there is no need to take any actions.
     */

    /*
     * State Data: BCMLTM_STATE_TYPE_KEY_BOUNDS
     */
    rv = bcmltm_state_data_get(unit, lt_state,
                               BCMLTM_STATE_TYPE_KEY_BOUNDS,
                               (void *)&key_bounds);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT
            (logical_key_bounds_init(unit, sid, key_bounds));
    } else {
        if (rv != SHR_E_NOT_FOUND) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM initialization: "
                                  "could not get Key Bounds state data"
                                  "sid=%d rv=%d\n"),
                       sid, rv));
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Erase the given LT state metadata.
 *
 * Erase the given LT state metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_state_hap HA Pointer to LT state data metadata to destroy.
 */
static void
logical_state_destroy(int unit, bcmltm_ha_ptr_t lt_state_hap)
{
    bcmltm_lt_state_t *state;
    bcmltm_ha_ptr_t cur_state_data_hap, next_state_data_hap, data_hap;
    bcmltm_lt_state_data_t *cur_state_data;
    void *data;

    state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    if (state == NULL) {
        return;
    }

    cur_state_data_hap = state->table_state_data_hap;

    while (!BCMLTM_HA_PTR_IS_INVALID(cur_state_data_hap)) {
        cur_state_data = bcmltm_ha_ptr_to_mem_ptr(unit, cur_state_data_hap);
        if (cur_state_data == NULL) {
            /* Inconsistent state */
            return;
        }

        next_state_data_hap = cur_state_data->next_state_data_hap;
        data_hap = cur_state_data->state_data_hap;

        data = bcmltm_ha_ptr_to_mem_ptr(unit, data_hap);
        if (data != NULL) {
            sal_memset(data, 0, cur_state_data->state_data_size);
        }

        sal_memset(cur_state_data, 0, sizeof(bcmltm_lt_state_data_t));
        cur_state_data_hap = next_state_data_hap;
    }

    sal_memset(state, 0, sizeof(*state));
    return;
}


/*!
 * \brief Add LT additional state data.
 *
 * Allocates and add specified additional state data block
 * to the given table state.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_state_hap HA pointer of LT state to add state data to.
 * \param [in] data_size Size in bytes of state data to add.
 * \param [in] data_type Format of the state data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_data_add(int unit,
                       bcmltm_ha_ptr_t lt_state_hap,
                       unsigned int data_size,
                       bcmltm_state_type_t data_type)
{
    bcmltm_lt_state_t *lt_state;
    bcmltm_ha_ptr_t tsd_hap, data_hap;
    bcmltm_lt_state_data_t *tsd = NULL;
    void *data = NULL;

    SHR_FUNC_ENTER(unit);

    lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    SHR_NULL_CHECK(lt_state, SHR_E_INTERNAL);

    /* State data structure */
    tsd_hap = bcmltm_ha_alloc(unit,
                              BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                              sizeof(bcmltm_lt_state_data_t));
    tsd = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_hap);
    SHR_NULL_CHECK(tsd, SHR_E_MEMORY);

    /*
     * Now fill out state_data structure in case of error in null check,
     * so destroy function operates properly.
     */
    tsd->state_data_type = data_type;
    BCMLTM_HA_PTR_INVALID_SET(tsd->state_data_hap);
    BCMLTM_HA_PTR_INVALID_SET(tsd->next_state_data_hap);

    /* Append to list */
    if (BCMLTM_HA_PTR_IS_INVALID(lt_state->table_state_data_hap)) {
        lt_state->table_state_data_hap = tsd_hap;
    } else {
        bcmltm_ha_ptr_t last_tsd_hap = lt_state->table_state_data_hap;
        bcmltm_lt_state_data_t *last_tsd;

        /*
         * We already checked above that last_tsd_hap is valid, so this
         * loop must execute at least once to give a valid last_tsd.
         */
        while (!BCMLTM_HA_PTR_IS_INVALID(last_tsd_hap)) {
            last_tsd = bcmltm_ha_ptr_to_mem_ptr(unit, last_tsd_hap);
            SHR_NULL_CHECK(last_tsd, SHR_E_INTERNAL);

            last_tsd_hap = last_tsd->next_state_data_hap;
        }

        last_tsd->next_state_data_hap = tsd_hap;
    }

    data_hap = bcmltm_ha_alloc(unit,
                               BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                               data_size);
    data = bcmltm_ha_ptr_to_mem_ptr(unit, data_hap);
    SHR_NULL_CHECK(data, SHR_E_MEMORY);
    sal_memset(data, 0, data_size);

    /* Update state data */
    tsd->state_data_size = data_size;
    tsd->state_data_hap = data_hap;

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the LT state.
 *
 * Create the LT state for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] table_attr Table attribute.
 * \param [out] lt_state_hap_ptr Returning pointer to HA pointer LT state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_state_create(int unit, bcmlrd_sid_t sid,
                     const bcmltm_table_attr_t *table_attr,
                     bcmltm_ha_ptr_t *lt_state_hap_ptr)
{
    bcmltm_ha_ptr_t lt_state_hap;
    bcmltm_lt_state_t *lt_state = NULL;
    unsigned int bitmap_size;
    uint32_t total_table_size;  /* Table size, (lt_index * mem_param) */
    bcmltm_table_type_t table_type;

    SHR_FUNC_ENTER(unit);

    table_type = table_attr->type;

    /* Allocate */
    lt_state_hap = bcmltm_ha_alloc(unit, BCMLTM_HA_BLOCK_TYPE_STATE,
                                sizeof(*lt_state));
    lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    SHR_NULL_CHECK(lt_state, SHR_E_MEMORY);
    if (lt_state->initialized) {
        /* HA recovery */
        if (lt_state->ltid != sid) {
            /* Mismatched LT state */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM state HA recovery: "
                                  "mismatched LTIDs - state sid=%d "
                                  "expected sid=%d\n"),
                       lt_state->ltid, sid));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        } else {
            /* Nothing to do beyond connect LT to recovered state below. */
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    sal_memset(lt_state, 0, sizeof(*lt_state));

    /* Fill data */
    lt_state->ltid = sid;
    lt_state->current_index_min = table_attr->index_min;
    lt_state->current_index_max = table_attr->index_max;
    lt_state->current_param_min = table_attr->mem_param_min;
    lt_state->current_param_max = table_attr->mem_param_max;
    lt_state->entry_count = 0;
    BCMLTM_HA_PTR_INVALID_SET(lt_state->table_state_data_hap);

    /*
     * Entry bitmap
     *
     * The bitmap needs to track all the unique logical table entries.
     * As such, the size needs to account for any memory parameter
     * defined for the table, in addition to the lt index.
     */
    total_table_size = (table_attr->index_max + 1) *
        (table_attr->mem_param_max + 1);

    bitmap_size = SHR_BITALLOCSIZE(total_table_size);
    SHR_IF_ERR_EXIT
        (logical_state_data_add(unit, lt_state_hap,
                                bitmap_size,
                                BCMLTM_STATE_TYPE_INUSE_BITMAP));

    /*
     * Global Entry Bitmap
     *
     * This global bitmap keeps track of the shared entries
     * among overlay tables.  This global bitmap is required
     * only for the table that is selected as the 'master' overlay table.
     */
    if ((table_attr->flag & BCMLTM_TABLE_FLAG_OVERLAY) &&
        (sid == table_attr->global_ltid)) {
        SHR_IF_ERR_EXIT
            (logical_state_data_add(unit, lt_state_hap,
                                    bitmap_size,
                                    BCMLTM_STATE_TYPE_GLOBAL_BITMAP));
    }

    /*
     * Index LT Key Field boundary information
     *
     * This contains the current Index LT key field boundaries.
     * Initial key field boundaries for Index LTs can be modified
     * through the TABLE_FIELD_CONTROL LT.
     */
    if (BCMLTM_TABLE_TYPE_LT_INDEX(table_type)) {
        uint32_t key_bounds_size;

        key_bounds_size = logical_key_bounds_size_get(unit, sid);
        SHR_IF_ERR_EXIT
            (logical_state_data_add(unit, lt_state_hap,
                                    key_bounds_size,
                                    BCMLTM_STATE_TYPE_KEY_BOUNDS));
    }

    /* Now LT state is prepared */
    lt_state->initialized = TRUE;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_state_destroy(unit, lt_state_hap);
        *lt_state_hap_ptr = BCMLTM_HA_PTR_INVALID;
    } else {
        *lt_state_hap_ptr = lt_state_hap;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine an LTs state size requirements.
 *
 * Evaluate the LT state metadata size required for the given table.
 * This routine returns the required memory for the LT state and
 * the state data.
 * This function is used to determine the HA memory requirements
 * for the LTM.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] state_size_ptr Returning pointer to LT state size.
 * \param [out] state_data_size_ptr Returning pointer to LT state data size.
 *
 * \retval None.
 */
static void
logical_state_size(int unit, bcmlrd_sid_t sid,
                     uint32_t *state_size_ptr,
                     uint32_t *state_data_size_ptr)
{
    bcmltm_table_attr_t table_attr;
    uint32_t total_table_size;  /* Table size, (lt_index * mem_param) */
    uint32_t state_size = 0, state_data_size = 0;
    bcmltm_table_type_t table_type;

    if (SHR_FAILURE(bcmltm_nc_lt_info_table_attr_get(unit, sid,
                                                     &table_attr))) {
        *state_size_ptr = 0;
        *state_data_size_ptr = 0;
        return;
    }

    table_type = table_attr.type;

    /* Size of bcmltm_lt_state_t structure. */
    state_size +=
        BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_t));

    /* Size of bcmltm_lt_state_data_t structure. */
    state_data_size +=
        BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_data_t));

    /* In-use bitmap, where applicable */
    total_table_size = (table_attr.index_max + 1) *
        (table_attr.mem_param_max + 1);

    state_data_size +=
        BCMLTM_WORDSIZEBYTES(SHR_BITALLOCSIZE(total_table_size));

    /* Add state data calculation for global bitmaps. */
    if ((table_attr.flag & BCMLTM_TABLE_FLAG_OVERLAY) &&
        (sid == table_attr.global_ltid)) {
        /* Size of bcmltm_lt_state_data_t structure. */
        state_data_size +=
            BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_data_t));

        /* Global bitmap */
        state_data_size +=
            BCMLTM_WORDSIZEBYTES(SHR_BITALLOCSIZE(total_table_size));
    }

    /* Add state data for Index LT key field boundary information */
    if (BCMLTM_TABLE_TYPE_LT_INDEX(table_type)) {
        uint32_t key_bounds_size;

        key_bounds_size = logical_key_bounds_size_get(unit, sid);
        state_data_size +=
            BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_data_t));
        state_data_size +=
            BCMLTM_WORDSIZEBYTES(key_bounds_size);
    }

    *state_size_ptr = state_size;
    *state_data_size_ptr = state_data_size;
}

/*!
 * \brief Destroy the given params metadata.
 *
 * Destroy the given params metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] params Pointer to params metadata to destroy.+
 */
static void
logical_params_destroy(int unit,
                       bcmltm_lt_params_t *params)
{
    if (params == NULL) {
        return;
    }

    logical_state_destroy(unit, params->lt_state_hap);

    SHR_FREE(params);

    return;
}


/*!
 * \brief Create the LT params metadata.
 *
 * Create the LT params metadata for the given table.
 * This routine populates the required metadata information.
 *
 * Assumes:
 *   - unit, sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] params_ptr Returning pointer to LT params metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_params_create(int unit, bcmlrd_sid_t sid,
                      bcmltm_lt_params_t **params_ptr)
{
    bcmltm_lt_params_t *params = NULL;
    bcmltm_table_attr_t table_attr;
    uint32_t index_min;
    uint32_t index_max;
    uint32_t mem_param_min;
    uint32_t mem_param_max;

    SHR_FUNC_ENTER(unit);

    

    /* Allocate */
    SHR_ALLOC(params, sizeof(*params), "LTM params");
    SHR_NULL_CHECK(params, SHR_E_MEMORY);
    sal_memset(params, 0, sizeof(*params));

    /* Retrieve table attributes to determine interactive/modeled mode */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_attr_get(unit, sid, &table_attr));

    /*
     * Get lt index minimum and maximum value.
     * If table does not have such key, min and max are set to zeros.
     *
     * The lt index represents a group of entries in a table.
     * In addition, a table may have a memory parameter which identifies
     * a given group.  This memory parameter is accounted for
     * when creating the bitmap used for tracking entries in
     * an Index LT (lt_state).
     */
    index_min = table_attr.index_min;
    index_max = table_attr.index_max;

    /* Sanity check */
    if (index_min > index_max) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM metadata params create: "
                              "invalid sid=%d "
                              "index_min=%d index_max=%d\n"),
                   sid, index_min, index_max));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    mem_param_min = table_attr.mem_param_min;
    mem_param_max = table_attr.mem_param_max;

    /* Sanity check */
    if (mem_param_min > mem_param_max) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM metadata params create: "
                              "invalid sid=%d "
                              "mem_param_min=%d mem_param_max=%d\n"),
                   sid, mem_param_min, mem_param_max));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /* Fill data */
    params->ltid = BCMLTM_SID_TO_LTID(sid);
    if (table_attr.mode != BCMLTM_TABLE_MODE_INTERACTIVE) {
        params->lt_flags |= BCMLTM_LT_FLAGS_MODELED;
    }
    params->index_min = index_min;
    params->index_max = index_max;
    params->mem_param_min = mem_param_min;
    params->mem_param_max = mem_param_max;
    params->global_inuse_ltid = table_attr.global_ltid;
    SHR_IF_ERR_EXIT(logical_state_create(unit, sid, &table_attr,
                                         &params->lt_state_hap));

    *params_ptr = params;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_params_destroy(unit, params);
        *params_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine the HA state size requirements.
 *
 * Evaluate all LT state metadata sizes required to support the
 * configured set of Logical Tables.  Record the total and maximum
 * state sizes for use in calculating the HA block sizes.
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of tables IDs.
 * \param [out] ha_state_size_ptr Returning pointer to HA state size record.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_md_ha_sizes_get(int unit,
                       uint32_t sid_max_count,
                       bcmltm_ha_state_sizes_t *ha_state_size_ptr)
{
    uint32_t sid;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    unsigned int size;
    uint32_t state_size, state_data_size;
    int rv;
    SHR_FUNC_ENTER(unit);

    sal_memset(ha_state_size_ptr, 0, sizeof(*ha_state_size_ptr));

    /* Allocate sid list array */
    sid_max_count = bcmltm_logical_table_count_get();
    size = sizeof(*sid_list) * sid_max_count;
    SHR_ALLOC(sid_list, size, "LTM Logical Table sid_list");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    rv = bcmlrd_table_list_get(unit, sid_max_count, sid_list, &num_sid);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            /* No tables are defined */
            num_sid = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Could not get table ID list rv=%d\n"),
                       rv));
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];

        state_size = 0;
        state_data_size = 0;

        
        logical_state_size(unit, sid, &state_size, &state_data_size);

        if (state_size > ha_state_size_ptr->maximum_state_size) {
            ha_state_size_ptr->maximum_state_size = state_size;
        }

        if (state_data_size > ha_state_size_ptr->maximum_state_data_size) {
            ha_state_size_ptr->maximum_state_data_size = state_data_size;
        }

        ha_state_size_ptr->total_state_size += state_size;
        ha_state_size_ptr->total_state_data_size += state_data_size;
    }

 exit:
    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the LTM HA state in preparation for metadata creation.
 *
 * During metadata creation, the LT state is connected to the
 * metadata structures.  This connection may take the following forms:
 *
 * Cold Boot   - The state record for each LT is created within
 *               the HA space and initialized.
 * Warm Boot   - The state record of each LT is recovered from
 *               the HA space.
 * HA recovery - The state record of each LT is recovered from
 *               the HA space, and sanity checks performed.
 * ISSU/ISSD   - The state record of each LT is recovered from
 *               the HA space.  The supported LT set may change,
 *               so the HA space may require reconfiguration to
 *               match the correct ordering of the new LT set.
 *               The LT state format may have changed, so the
 *               prior state information must be transcribed
 *               into the new format if so.
 *               Each LT may require individual further adjustment
 *               to its state information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [in] sid_max_count Maximum number of tables IDs.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_md_ha_init(int unit,
                  bool warm,
                  uint32_t sid_max_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_md_ha_sizes_get(unit, sid_max_count,
                                &(LOGICAL_HA_SIZES(unit))));

    SHR_IF_ERR_EXIT
        (bcmltm_ha_init(unit, warm, &(LOGICAL_HA_SIZES(unit))));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create operation metadata for the given opcode.
 *
 * Create the operation metadata for given table ID to use for the
 * specified opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_drv LT driver (assumes not null).
 * \param [in] opcode Opcode.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_op_md_create(int unit, bcmlrd_sid_t sid,
                     const bcmltm_md_lt_drv_t *lt_drv,
                     bcmlt_opcode_t opcode,
                     bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_table_attr_t table_attr;

    SHR_FUNC_ENTER(unit);

    if (lt_drv == NULL) {
        SHR_EXIT();
    }

    *op_md_ptr = NULL;

    /*
     * Skip metadata creation if table is read only and
     * opcode is not a LOOKUP and is not a TRAVERSE.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_attr_get(unit, sid, &table_attr));
    if ((table_attr.flag & BCMLTM_TABLE_FLAG_READ_ONLY) &&
        ((opcode != BCMLT_OPCODE_LOOKUP) &&
         (opcode != BCMLT_OPCODE_TRAVERSE))) {
        SHR_EXIT();
    }

    switch (opcode) {
    case BCMLT_OPCODE_NOP:
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_INSERT:   /* Insert an entry */
        if (lt_drv->op_insert_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_insert_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_LOOKUP:   /* Retrieve the contents of an entry */
        if (lt_drv->op_lookup_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_lookup_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_DELETE:  /* Delete an entry */
        if (lt_drv->op_delete_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_delete_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_UPDATE:  /* Modify the specified fields of an entry */
        if (lt_drv->op_update_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_update_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    case BCMLT_OPCODE_TRAVERSE:  /* Retrieve next entry */
        if (lt_drv->op_traverse_create != NULL) {
            SHR_IF_ERR_EXIT
                (lt_drv->op_traverse_create(unit, sid, op_md_ptr));
        }
        SHR_EXIT();
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        break;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        lt_drv->op_destroy(*op_md_ptr);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get LT driver based on table type.
 *
 * Get the Logical Table driver based on the table type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid  Table ID.
 * \param [in] drv Returning pointer to LT driver.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_lt_drv_get(int unit,
                   bcmlrd_sid_t sid,
                   const bcmltm_md_lt_drv_t **drv)
{
    bcmltm_table_type_t type;

    SHR_FUNC_ENTER(unit);

    *drv = NULL;

    /* Get table type */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_type_get(unit, sid, &type));

    /* Check for LT map type */
    if (BCMLTM_TABLE_TYPE_LT_LTM(type)) {
        /*
         * LT with LTM map group.
         * E.g. TABLE_CONTROL, TABLE_INFO, TABLE_STATS.
         * Key types (i.e index, hash, etc.) are not relevant for this type.
         */
        *drv = bcmltm_md_lt_ltm_drv_get();
    } else if (BCMLTM_TABLE_TYPE_LT_CTH(type)) {
        /*
         * LT with CTH map.
         * Key types (i.e index, hash, etc.) are not relevant for this type.
         */
        *drv = bcmltm_md_lta_cth_drv_get();
    } else if (BCMLTM_TABLE_TYPE_LT_PT(type)) {
        /*
         * LT with direct PT maps.
         * Check for specific key types.
         */
        if (BCMLTM_TABLE_TYPE_LT_SIMPLE_INDEX(type)) {
            *drv = bcmltm_md_lt_simple_index_drv_get();
        } else if (BCMLTM_TABLE_TYPE_LT_ALLOC_INDEX(type)) {
            *drv = bcmltm_md_lt_alloc_index_drv_get();
        } else if (BCMLTM_TABLE_TYPE_LT_KEYED(type)) {
            *drv = bcmltm_md_lt_keyed_drv_get();
        }
    }

    if (*drv == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SID %d: "
                              "Table type 0x%x not supported\n"),
                   sid, type));
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the LT metadata.
 *
 * Destroy the LT metadata for given table.
 *
 * Assumes:
 *   - unit and sid are valid
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] lt_md LT metadata to destroy.
 */
static void
logical_lt_destroy(int unit, bcmlrd_sid_t sid,
                   bcmltm_lt_md_t *lt_md)
{
    bcmlt_opcode_t opcode;
    const bcmltm_md_lt_drv_t *lt_drv = NULL;

    if (lt_md == NULL) {
        return;
    }

    /* Get table driver */
    (void) logical_lt_drv_get(unit, sid, &lt_drv);

    /* Destroy ops */
    if ((lt_drv != NULL) && (lt_drv->op_destroy != NULL)) {
        for (opcode = 0; opcode < BCMLT_OPCODE_NUM; opcode++) {
            lt_drv->op_destroy(lt_md->op[opcode]);
        }
    }

    /* Destroy params */
    logical_params_destroy(unit, lt_md->params);

    SHR_FREE(lt_md);

    return;
}


/*!
 * \brief Create the LT metadata.
 *
 * Create the LT metadata for given table.
 *
 * Assumes:
 *   - unit and sid are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_md_ptr Retuning pointer to LT metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_lt_create(int unit,
                  bcmlrd_sid_t sid,
                  bcmltm_lt_md_t **lt_md_ptr)
{
    bcmltm_lt_md_t *lt_md = NULL;
    bcmltm_lt_params_t *params = NULL;
    bcmltm_lt_op_md_t *op_md = NULL;
    bcmlt_opcode_t opcode;
    const bcmltm_md_lt_drv_t *lt_drv = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(lt_md, sizeof(*lt_md), "LTM LT metadata");
    SHR_NULL_CHECK(lt_md, SHR_E_MEMORY);
    sal_memset(lt_md, 0, sizeof(*lt_md));

    /* Get table driver */
    SHR_IF_ERR_EXIT
        (logical_lt_drv_get(unit, sid, &lt_drv));

    /* Create params */
    SHR_IF_ERR_EXIT(logical_params_create(unit, sid, &params));
    lt_md->params = params;

    /* Create ops */
    for (opcode = 0; opcode < BCMLT_OPCODE_NUM; opcode++) {
        SHR_IF_ERR_EXIT
            (logical_op_md_create(unit, sid, lt_drv, opcode, &op_md));
        lt_md->op[opcode] = op_md;
    }

    /* Set Table Commit handler list */
    
    SHR_IF_ERR_EXIT(bcmltm_nc_table_commit_list_retrieve(unit, sid,
                                                         &lt_md->tc_list));

    /* Set CTH handler */
    SHR_IF_ERR_EXIT(bcmltm_nc_lta_cth_handler_retrieve(unit, sid,
                                                       &lt_md->cth));

    *lt_md_ptr = lt_md;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_lt_destroy(unit, sid, lt_md);
        *lt_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy the LTM metadata.
 *
 * Destroy the LTM metadata for given unit.
 *
 * This implementation is based upon the symbols IDs
 * being in a compact numbering space from [0 .. (sid_max_count-1)]
 *
 * Assumes:
 *   - unit is valid
 *
 * \param [in] unit Unit number.
 * \param [in] ltm_md LTM metadata to destroy.
 */
static void
logical_ltm_destroy(int unit, bcmltm_md_t *ltm_md)
{
    bcmlrd_sid_t sid;
    bcmlrd_sid_t sid_max;

    if (ltm_md == NULL) {
        return;
    }

    sid_max = ltm_md->lt_max - 1;

    /* Destroy LT metadata for each table */
    for (sid = 0; sid <= sid_max; sid++) {
        logical_lt_destroy(unit, sid, ltm_md->lt_md[sid]);
        ltm_md->lt_md[sid] = NULL;
    }

    /* Free LTM metadata */
    SHR_FREE(ltm_md);

    /* Clear the HA memory for LT state */
    bcmltm_ha_reset(unit);

    return;
}


/*!
 * \brief Create the LTM metadata for given unit.
 *
 * Create the LTM metadata for given unit.
 *
 * This implementation is based upon the symbols IDs
 * being in a compact numbering space from [0 .. (sid_max_count-1)]
 *
 * Assumes:
 *   - unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of tables IDs.
 * \param [out] ltm_md_ptr Retuning pointer to LT metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
logical_ltm_create(int unit,
                   uint32_t sid_max_count,
                   bcmltm_md_t **ltm_md_ptr)
{
    unsigned int size;
    bcmlrd_sid_t sid;
    bcmlrd_sid_t sid_max;
    bcmltm_md_t *ltm_md = NULL;
    bcmltm_lt_md_t *lt_md = NULL;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    size_t opx;
    uint32_t *wb_max_size_p;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* SID max value */
    sid_max = sid_max_count - 1;

    /* Allocate sid list array */
    size = sizeof(*sid_list) * sid_max_count;
    SHR_ALLOC(sid_list, size, "LTM Logical Table sid_list");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    rv = bcmlrd_table_list_get(unit, sid_max_count, sid_list, &num_sid);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            /* No tables are defined */
            num_sid = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Could not get table ID list rv=%d\n"),
                       rv));
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    /* Verify that SID value fits design criteria [0 .. (max_count-1)] */
    for (idx = 0; idx < num_sid; idx++) {
        if (sid_list[idx] > sid_max) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid SID %d, "
                                  "SID should not be larger than %d\n"),
                       sid_list[idx], sid_max));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Allocate LTM metadata structure */
    size = sizeof(bcmltm_md_t) +
        (sizeof(bcmltm_lt_md_t *) * sid_max_count);
    SHR_ALLOC(ltm_md, size, "LTM Metadata");
    SHR_NULL_CHECK(ltm_md, SHR_E_MEMORY);
    sal_memset(ltm_md, 0, size);

    /* Initialize LTM metadata */
    ltm_md->lt_num = 0;
    ltm_md->lt_max = sid_max_count;
    ltm_md->wb_max_modeled = 0;
    ltm_md->wb_max_interactive = 0;

    /* Create LT metadata for each table */
    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (logical_lt_create(unit, sid, &lt_md), SHR_E_UNAVAIL);
        /* Some Logical tables may not be applicable to a device */
        if (lt_md != NULL) {
            ltm_md->lt_md[sid] = lt_md;
            ltm_md->lt_num++;

            /* Check for maximum Working Buffer size */
            if (lt_md->params->lt_flags & BCMLTM_LT_FLAGS_MODELED) {
                wb_max_size_p = &(ltm_md->wb_max_modeled);
            } else {
                wb_max_size_p = &(ltm_md->wb_max_interactive);
            }
            for (opx = BCMLT_OPCODE_NOP; opx < BCMLT_OPCODE_NUM; opx++) {
                if (lt_md->op[opx] != NULL) {
                    if (lt_md->op[opx]->working_buffer_size >
                        *wb_max_size_p) {
                        *wb_max_size_p =
                            lt_md->op[opx]->working_buffer_size;
                    }
                }
            }
        }
    }

    *ltm_md_ptr = ltm_md;

 exit:
    if (SHR_FUNC_ERR()) {
        logical_ltm_destroy(unit, ltm_md);
        *ltm_md_ptr = NULL;
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
int
bcmltm_md_logical_create(int unit, bool warm)
{
    uint32_t sid_max_count;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM Logical Table metadata create\n")));

    /* Check if metadata has been initialized */
    if (LOGICAL_INIT(unit)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "LTM Logical Table metadata is already "
                             "created\n")));
        SHR_EXIT();
    }

    /* Get total table symbol IDs count */
    sid_max_count = bcmltm_logical_table_count_get();
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total Logical tables is %d\n"),
                 (int)sid_max_count));

    /* Initialize Working Buffer information */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_info_init(unit, sid_max_count));

    /* Initialize & create LT information */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_init(unit, sid_max_count));

    /* Initialize HA information */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ha_init(unit, warm, sid_max_count));

    /* Create LTM metadata */
    rv = logical_ltm_create(unit, sid_max_count, &LOGICAL_LTM_MD(unit));
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            SHR_EXIT();
        } else {
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

 exit:
    if (SHR_FUNC_ERR() || (rv == SHR_E_UNAVAIL)) {
        bcmltm_md_logical_destroy(unit);
    }

    SHR_FUNC_EXIT();
}


int
bcmltm_md_logical_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (!LOGICAL_INIT(unit)) {
        SHR_EXIT();
    }

    /* Destroy LTM metadata */
    logical_ltm_destroy(unit, LOGICAL_LTM_MD(unit));
    LOGICAL_LTM_MD(unit) = NULL;

    /* Release HA information */
    bcmltm_ha_cleanup(unit);

    /* Cleanup Logical Table info data structure */
    bcmltm_nc_lt_info_cleanup(unit);

    /* Cleanup Working Buffer information */
    bcmltm_wb_lt_info_cleanup(unit);

  exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_logical_lt_retrieve(int unit,
                              uint32_t ltid,
                              bcmltm_lt_md_t **ltm_md_ptr)
{
    bcmlrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!LOGICAL_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    sid = BCMLTM_LTID_TO_SID(ltid);

    /* Check that unit has metadata */
    if (LOGICAL_LTM_MD(unit) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    if (!LOGICAL_SID_VALID(unit, sid)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid table id %d (sid=%d)\n"),
                   ltid, sid));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /*
     * Check that table is valid on the given unit.
     *
     * Logical table IDs is the super of all valid tables in all devices.
     * As such, there may be tables that are not defined for a given
     * device.  In this case, the pointer of the table metadata is NULL.
     */
    if (LOGICAL_LTM_MD(unit)->lt_md[sid] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    *ltm_md_ptr = LOGICAL_LTM_MD(unit)->lt_md[sid];

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_logical_retrieve(int unit,
                           bcmltm_md_t **ltm_md_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!LOGICAL_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    /* Check that unit has metadata */
    if (LOGICAL_LTM_MD(unit) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    *ltm_md_ptr = LOGICAL_LTM_MD(unit);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_logical_wb_max_get(int unit,
                             bcmltm_table_mode_t mode,
                             uint32_t *wb_max)
{
    SHR_FUNC_ENTER(unit);

    /* Check if metadata has been initialized */
    if (!LOGICAL_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

    /* Check that unit has metadata */
    if (LOGICAL_LTM_MD(unit) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    switch(mode) {
    case BCMLTM_TABLE_MODE_MODELED:
        *wb_max = LOGICAL_LTM_MD(unit)->wb_max_modeled;
        break;

    case BCMLTM_TABLE_MODE_INTERACTIVE:
        *wb_max = LOGICAL_LTM_MD(unit)->wb_max_interactive;
        break;

    default:
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_logical_state_data_reset(int unit, uint32_t ltid)
{
    bcmltm_lt_md_t *lt_md;
    bcmltm_ha_ptr_t lt_state_hap;
    bcmltm_lt_state_t *lt_state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_md_logical_lt_retrieve(unit, ltid, &lt_md));

    /* Skip tables with no state data information */
    if (lt_md->params == NULL) {
        SHR_EXIT();
    }

    lt_state_hap = lt_md->params->lt_state_hap;
    if (lt_state_hap == BCMLTM_HA_PTR_INVALID) {
        SHR_EXIT();
    }

    lt_state = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_hap);
    if (lt_state == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (logical_state_data_reset(unit, ltid, lt_state));

 exit:
    SHR_FUNC_EXIT();
}
