/*! \file bcmltm_state.c
 *
 * Logical Table Manager
 *
 * This module contains routines for the modification, retrieval, and
 * manipulation of Logical Table state.
 *
 * Each LT has a basic set of table state, which may include other optional
 * elements for the overall state.
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
#include <bcmltm/bcmltm_state_internal.h>
#include <bsl/bsl.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

static bcmltm_state_mgmt_t lt_state_mgmt[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Accessor macros
 */
#define LT_STATE_MGMT(_u)         lt_state_mgmt[(_u)]
#define LT_STATE_MAX_SIZE(_u)     (LT_STATE_MGMT(_u).maximum_state_size)
#define LT_STATE_DATA_MAX_SIZE(_u)  \
    (LT_STATE_MGMT(_u)->maximum_state_data_size)

#define LT_STATE_CLONE_MAX(_u)    (LT_STATE_MGMT(_u).lt_trans_max)
#define LT_STATE_CLONE_COUNT(_u)  (LT_STATE_MGMT(_u).lt_clone_count)

#define LT_STATE_ROLLBACK_HAP(_u,_c)  \
    (LT_STATE_MGMT(_u).lt_state_rollback_chunk_hap[(_c)])
#define LT_STATE_DATA_ROLLBACK_HAP(_u,_c)  \
    (LT_STATE_MGMT(_u).lt_state_data_rollback_chunk_hap[(_c)])

#define LT_STATE_GET_CB(_u)       (LT_STATE_MGMT(_u).lt_state_get_cb)
#define LT_INFO_GET_CB(_u)        (LT_STATE_MGMT(_u).lt_info_get_cb)

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

bcmltm_state_mgmt_t *
bcmltm_state_mgmt_get(int unit)
{
    return &(LT_STATE_MGMT(unit));
}

int
bcmltm_state_data_get(int unit,
                      bcmltm_lt_state_t *lt_state,
                      bcmltm_state_type_t state_type,
                      void **state_data)
{
    bcmltm_lt_state_data_t *state_iter;
    bcmltm_ha_ptr_t state_iter_hap, state_data_hap;

    state_iter_hap = lt_state->table_state_data_hap;

    while (state_iter_hap != BCMLTM_HA_PTR_INVALID) {
        state_iter = bcmltm_ha_ptr_to_mem_ptr(unit, state_iter_hap);
        if (state_iter == NULL) {
            break;
        }
        if (state_iter->state_data_type == state_type) {
            state_data_hap = state_iter->state_data_hap;
            *state_data = bcmltm_ha_ptr_to_mem_ptr(unit, state_data_hap);
            return SHR_E_NONE;
        }
        state_iter_hap = state_iter->next_state_data_hap;
    }

    return SHR_E_NOT_FOUND;
}

int
bcmltm_state_copy(int unit,
                  bcmltm_ha_ptr_t lt_state_src_hap,
                  bcmltm_ha_ptr_t lt_state_dst_hap)
{
    bcmltm_lt_state_t *lt_state_src, *lt_state_dst;
    bcmltm_ha_ptr_t tsd_src_hap, tsd_dst_hap;
    bcmltm_lt_state_data_t *tsd_src, *tsd_dst;
    void *data_src, *data_dst;

    lt_state_src = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_src_hap);
    lt_state_dst = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_dst_hap);

    if ((lt_state_src == NULL) || (lt_state_dst == NULL)) {
        return SHR_E_INTERNAL;
    }

    /* Note the link before copying over it */
    tsd_dst_hap = lt_state_dst->table_state_data_hap;

    sal_memcpy(lt_state_dst, lt_state_src, sizeof(bcmltm_lt_state_t));

    /* Restore link */
    lt_state_dst->table_state_data_hap = tsd_dst_hap;

    /* Follow chain of source LT state */
    tsd_src_hap = lt_state_src->table_state_data_hap;

    while (!BCMLTM_HA_PTR_IS_INVALID(tsd_src_hap)) {
        tsd_src = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_src_hap);
        tsd_dst = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_dst_hap);
        if ((tsd_src == NULL) || (tsd_dst == NULL)) {
            return SHR_E_INTERNAL;
        }

        if ((tsd_dst->state_data_size != tsd_src->state_data_size) ||
            (tsd_dst->state_data_type != tsd_src->state_data_type)) {
            /* Mismatched src and destination characteristics */
            return SHR_E_INTERNAL;
        }

        data_src = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_src->state_data_hap);
        data_dst = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_dst->state_data_hap);
        if ((data_src == NULL) || (data_dst == NULL)) {
            return SHR_E_INTERNAL;
        }

        /* Copy state data */
        sal_memcpy(data_dst, data_src, tsd_src->state_data_size);

        tsd_src_hap = tsd_src->next_state_data_hap;
        tsd_dst_hap = tsd_dst->next_state_data_hap;
    }

    if (!BCMLTM_HA_PTR_IS_INVALID(tsd_dst_hap)) {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

int
bcmltm_state_clone(int unit,
                   uint8_t lt_idx,
                   bcmltm_ha_ptr_t lt_state_src_hap,
                   bcmltm_ha_ptr_t *lt_state_dst_hap_p)
{
    bcmltm_lt_state_t *lt_state_src, *lt_state_dst;
    bcmltm_ha_ptr_t tsd_src_hap, tsd_dst_hap, *last_tsd_dst_hap;
    bcmltm_lt_state_data_t *tsd_src, *tsd_dst;
    bcmltm_ha_ptr_t state_data_dst_hap;

    /* Fetch the rollback state pointer for this lt_idx */
    *lt_state_dst_hap_p = LT_STATE_ROLLBACK_HAP(unit, lt_idx);

    lt_state_src = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_src_hap);
    lt_state_dst = bcmltm_ha_ptr_to_mem_ptr(unit, *lt_state_dst_hap_p);

    if ((lt_state_src == NULL) || (lt_state_dst == NULL)) {
        return SHR_E_INTERNAL;
    }

    /* Do not copy LT state here */

    /* Follow chain of source LT state */
    tsd_src_hap = lt_state_src->table_state_data_hap;

    /* Fetch the rollback state data pointer for this lt_idx */
    tsd_dst_hap = LT_STATE_DATA_ROLLBACK_HAP(unit, lt_idx);
    last_tsd_dst_hap = &(lt_state_dst->table_state_data_hap);

    while (!BCMLTM_HA_PTR_IS_INVALID(tsd_src_hap)) {
        /* Link last structure to this one */
        *last_tsd_dst_hap = tsd_dst_hap;

        tsd_src = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_src_hap);
        tsd_dst = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_dst_hap);
        if ((tsd_src == NULL) || (tsd_dst == NULL)) {
            return SHR_E_INTERNAL;
        }

        /* Establish structure links for state data record */
        sal_memcpy(tsd_dst, tsd_src, sizeof(bcmltm_lt_state_data_t));

        state_data_dst_hap =
            bcmltm_ha_ptr_increment(unit, tsd_dst_hap,
                                    sizeof(bcmltm_lt_state_data_t));

        if (BCMLTM_HA_PTR_IS_INVALID(state_data_dst_hap)) {
            /* Out of reserved HA space? */
            return SHR_E_INTERNAL;
        }

        /* Do not copy state data yet */
        tsd_dst->state_data_hap = state_data_dst_hap;

        last_tsd_dst_hap = &(tsd_dst->next_state_data_hap);
        tsd_dst_hap =
            bcmltm_ha_ptr_increment(unit, state_data_dst_hap,
                                    tsd_dst->state_data_size);

        if (BCMLTM_HA_PTR_IS_INVALID(tsd_dst_hap)) {
            /* Out of reserved HA space? */
            return SHR_E_INTERNAL;
        }
        /* Advance to next glob */
        tsd_src_hap = tsd_src->next_state_data_hap;
    }

    *last_tsd_dst_hap = BCMLTM_HA_PTR_INVALID;

    return bcmltm_state_copy(unit, lt_state_src_hap, *lt_state_dst_hap_p);
}

void
bcmltm_state_lt_get_register(int unit,
                             bcmltm_state_lt_get_f state_get_cb)
{
    LT_STATE_GET_CB(unit) = state_get_cb;
}

void
bcmltm_info_lt_get_register(int unit,
                            bcmltm_info_lt_get_f info_get_cb)
{
    LT_INFO_GET_CB(unit) = info_get_cb;
}

int
bcmltm_state_lt_get(int unit,
                    uint32_t ltid,
                    bcmltm_lt_state_t **lt_state_p)
{
    if (LT_STATE_GET_CB(unit) == NULL) {
        if (lt_state_p != NULL) {
            *lt_state_p = NULL;
        }
        return SHR_E_UNAVAIL;
    }

    return LT_STATE_GET_CB(unit)(unit, ltid, lt_state_p);
}

int
bcmltm_info_lt_get(int unit,
                   uint32_t table_id,
                   uint32_t field_id,
                   uint32_t field_idx,
                   uint64_t *data_value)
{
    if (LT_INFO_GET_CB(unit) == NULL) {
        return SHR_E_UNAVAIL;
    }

    return LT_INFO_GET_CB(unit)(unit, table_id, field_id,
                                field_idx, data_value);
}
