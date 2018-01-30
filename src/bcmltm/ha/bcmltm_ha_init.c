/*! \file bcmltm_ha_init.c
 *
 * Logical Table Manager
 *
 * This module contains an implementation sufficient to permit management
 * of a direct-indexed table.  Most operations of the LTM are stubs.
 *
 * Translate LT (field,value) entry components to PT fields in HW entry format
 * Provide Logical resource manager stub
 * Provide LTM state update stub
 * Invoke PTM
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
#include <shr/shr_ha.h>
#include <bsl/bsl.h>

#include <bcmcfg/comp/bcmcfg_ltm_resources.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>

#include <bcmltm/bcmltm_ha_internal.h>
#include <bcmltm/bcmltm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_HIGHAVAILABILITY

bcmltm_ha_block_info_t \
     ha_block_info[BCMDRD_CONFIG_MAX_UNITS][BCMLTM_HA_BLOCK_TYPE_COUNT];

/*
 * Accessor macros
 */
#define LTM_HA_BLOCK(_u, _t)               ha_block_info[(_u)][(_t)]


/*******************************************************************************
 * Private functions
 */


/*******************************************************************************
 * Public functions
 */

uint8_t
bcmltm_ha_transaction_max(int unit)
{
    uint8_t max_tables_per_transaction = BCMLTM_TRANSACTION_LT_MAX;
    const bcmcfg_ltm_resources_config_t *ltm_conf;

    ltm_conf = bcmcfg_ltm_resources_config_get();
    if (ltm_conf != NULL && ltm_conf->max_tables_transaction != 0) {
        max_tables_per_transaction = ltm_conf->max_tables_transaction;
    }

    return max_tables_per_transaction;
}

int
bcmltm_ha_init(int unit,
               bool warm,
               bcmltm_ha_state_sizes_t *ha_state_sizes)
{
    uint32_t block_size;
    uint8_t trans_lt_max;
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;
    bcmltm_state_mgmt_t *state_mgmt;
    bcmltm_ha_block_header_t *block_header;
    SHR_FUNC_ENTER(unit);

    /*
     * Prepare High Availability records of Logical Table state
     * for use by the LT metadata creation system.
     */

    LOG_VERBOSE(BSL_LS_BCMLTM_HIGHAVAILABILITY,
                (BSL_META_U(unit,
                            "Logical Table Manager High Availability initialization\n")));

    trans_lt_max = bcmltm_ha_transaction_max(unit);
    state_mgmt = bcmltm_state_mgmt_get(unit);

    state_mgmt->maximum_state_size = ha_state_sizes->maximum_state_size;
    state_mgmt->maximum_state_data_size =
        ha_state_sizes->maximum_state_data_size;
    state_mgmt->lt_trans_max = trans_lt_max;

    /* Cold Boot */
    for (block_type = BCMLTM_HA_BLOCK_TYPE_STATE;
         block_type < BCMLTM_HA_BLOCK_TYPE_COUNT;
         block_type++) {
        block_info = &(LTM_HA_BLOCK(unit, block_type));

        /* Redundant for now, but this record may be useful later. */
        block_info->block_type = block_type;

        switch (block_type) {
        case BCMLTM_HA_BLOCK_TYPE_STATE:
            block_info->block_size =
                ha_state_sizes->total_state_size;
            break;

        case BCMLTM_HA_BLOCK_TYPE_STATE_DATA:
            block_info->block_size =
                ha_state_sizes->total_state_data_size;
            break;

        case BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS:
            block_info->block_size =
                (trans_lt_max * sizeof(uint32_t)) +
                sizeof(bcmltm_transaction_status_t);
            break;

        case BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE:
            block_info->block_size =
                trans_lt_max *
                ha_state_sizes->maximum_state_size;
            break;

        case BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE_DATA:
            block_info->block_size =
                trans_lt_max *
                ha_state_sizes->maximum_state_data_size;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }

        /* Add the block header for HA recovery identification */
        block_info->block_size += sizeof(bcmltm_ha_block_header_t);
        block_size = block_info->block_size;
        block_info->ha_block_ptr =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_LTM_COMP_ID,
                             block_type,
                             &block_size);
        SHR_NULL_CHECK(block_info->ha_block_ptr, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(block_size < block_info->block_size ?
                        SHR_E_MEMORY : SHR_E_NONE);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "LTM block %d: "
                                "requested bytes = %0u, allocated bytes = %0u \n"),
                     block_type,
                     (uint32_t)block_info->block_size,
                     (uint32_t)block_size));

        /* Block header init */
        block_header = (bcmltm_ha_block_header_t *)(block_info->ha_block_ptr);

        if (!warm ||
            (block_header->signature != BCMLTM_BLOCK_SIGNATURE(block_type))) {
            /* Cold Boot (or corrupt HA) */
            sal_memset(block_info->ha_block_ptr, 0, block_size);
            block_header->block_size = block_size;
            block_header->signature = BCMLTM_BLOCK_SIGNATURE(block_type);
        } else {
            /* Recovery */
            if (block_size < block_header->block_size) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "LTM block %d: "
                                        "expected bytes = %0u, recovered bytes = %0u \n"),
                             block_type,
                             (uint32_t)block_size,
                             (uint32_t)block_header->block_size));
            }
        }

        /* Block header now in use */
        block_info->allocated_size = sizeof(bcmltm_ha_block_header_t);
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_ha_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


void
bcmltm_ha_cleanup(int unit)
{
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;

    for (block_type = BCMLTM_HA_BLOCK_TYPE_STATE;
         block_type < BCMLTM_HA_BLOCK_TYPE_COUNT;
         block_type++) {
        block_info = &(LTM_HA_BLOCK(unit, block_type));
        block_info->ha_block_ptr = NULL;
        /* We do not free HA memory.
         * It is either discarded by the application or used
         * for recovery.
         */
    }

    return;
}


bcmltm_ha_ptr_t
bcmltm_ha_alloc(int unit,
                bcmltm_ha_block_type_t block_type,
                uint32_t size)
{
    uint32_t aligned_size;
    bcmltm_ha_ptr_t segment_ptr;
    bcmltm_ha_block_info_t *block_info;

    BCMLTM_HA_PTR_INVALID_SET(segment_ptr);
    aligned_size = BCMLTM_WORDSIZEBYTES(size);

    block_info = &(LTM_HA_BLOCK(unit, block_type));

    if ((block_info->block_size - block_info->allocated_size) <
        aligned_size) {
        LOG_ERROR(BSL_LS_BCMLTM_HIGHAVAILABILITY,
                  (BSL_META_U(unit,
                              "Logical Table High Availability -\n"
                       "\tinsufficient space for LT state in block %d\n"),
                              block_type));
    } else {
        /* Find start of free space. */
        segment_ptr = BCMLTM_HA_PTR(block_type, block_info->allocated_size);
        /* Update allocated size. */
        block_info->allocated_size += aligned_size;
    }

    return segment_ptr;
}

void
bcmltm_ha_reset(int unit)
{
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;
    void *clear_ptr;
    uint32_t clear_size;

    for (block_type = BCMLTM_HA_BLOCK_TYPE_STATE;
         block_type < BCMLTM_HA_BLOCK_TYPE_COUNT;
         block_type++) {
        block_info = &(LTM_HA_BLOCK(unit, block_type));

        block_info->allocated_size = sizeof(bcmltm_ha_block_header_t);
        clear_ptr = ((char *)block_info->ha_block_ptr) +
            block_info->allocated_size;
        clear_size = block_info->block_size - block_info->allocated_size;
        sal_memset(clear_ptr, 0, clear_size);
    }
}

void *
bcmltm_ha_ptr_to_mem_ptr(int unit,
                         bcmltm_ha_ptr_t ha_ptr)
{
    uint8_t *ptr_math;
    bcmltm_ha_block_info_t *block_info;

    if (BCMLTM_HA_PTR_IS_INVALID(ha_ptr)) {
        return NULL;
    }

    block_info =
        &(LTM_HA_BLOCK(unit, BCMLTM_HA_PTR_TO_BLOCK_TYPE(ha_ptr)));
    ptr_math = (uint8_t *)block_info->ha_block_ptr +
        BCMLTM_HA_PTR_OFFSET(ha_ptr);
    return ptr_math;
}

bcmltm_ha_ptr_t
bcmltm_ha_ptr_increment(int unit,
                        bcmltm_ha_ptr_t ha_ptr,
                        uint32_t increment)
{
    bcmltm_ha_ptr_t new_ha_ptr;
    bcmltm_ha_block_type_t block_type;
    bcmltm_ha_block_info_t *block_info;
    uint32_t offset;

    if (BCMLTM_HA_PTR_IS_INVALID(ha_ptr)) {
        return BCMLTM_HA_PTR_INVALID;
    }

    block_type = BCMLTM_HA_PTR_TO_BLOCK_TYPE(ha_ptr);
    offset = BCMLTM_HA_PTR_OFFSET(ha_ptr);
    block_info = &(LTM_HA_BLOCK(unit, block_type));

    /* Increment, word-aligned. */
    offset += BCMLTM_WORDSIZEBYTES(increment);
    if (offset >= block_info->block_size) {
        return BCMLTM_HA_PTR_INVALID;
    }

    new_ha_ptr = BCMLTM_HA_PTR(block_type, offset);
    return new_ha_ptr;
}
