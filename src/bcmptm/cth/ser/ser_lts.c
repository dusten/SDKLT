/*! \file ser_lts.c
 *
 * SER LTs initialization functions
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

/******************************************************************************
 * Includes
 */
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>

#include "ser_lts.h"
#include "ser_lt_ser_control.h"
#include "ser_lt_ser_control_pt.h"
#include "ser_lt_ser_injection.h"
#include "ser_lt_ser_injection_status.h"
#include "ser_lt_ser_stats.h"
#include "ser_lt_ser_notification.h"

/******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*! Default depth of INTR dispatching message queue */
#define MSG_Q_DEFAULT_DEPTH   8

/******************************************************************************
 * Typedefs
 */

/******************************************************************************
 * Function declaration
 */
static uint32_t
bcmptm_ser_msg_q_depth_get(int unit);

/******************************************************************************
 * Private variables
 */
static bcmptm_ser_cth_cb_t bcmptm_cth_cb = {

    .ser_control_field_val_get = &bcmptm_ser_control_field_val_get,

    .ser_control_pt_field_val_get = &bcmptm_ser_control_pt_field_val_get,

    .ser_control_pt_imm_update = &bcmptm_ser_control_pt_imm_update,

    .ser_control_pt_imm_insert = &bcmptm_ser_control_pt_imm_insert,

    .ser_hw_fault_notify = &bcmptm_ser_hw_fault_notify,

    .ser_stats_cache_update = &bcmptm_ser_stats_cache_update,

    .ser_stats_imm_update = &bcmptm_ser_stats_imm_update,

    .ser_msg_q_depth_get = &bcmptm_ser_msg_q_depth_get,

    .ser_checking_start = &bcmptm_ser_checking_enable
};
/*!\brief Pointer to routines in chip specific code */
const bcmptm_ser_data_driver_t* bcmptm_ser_data_cb[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private Functions
 */
/******************************************************************************
 * bcmptm_ser_config_field_data_get
 */
static int
bcmptm_ser_config_field_data_get(int unit, uint32_t lt_fid, uint32_t *lt_fld_data)
{
    int rv = SHR_E_NONE;
    uint64_t fld_val;

    SHR_FUNC_ENTER(unit);
    rv = bcmcfg_field_get(unit, SER_CONFIGt, lt_fid, &fld_val);
    SHR_IF_ERR_EXIT(rv);

    *lt_fld_data = (uint32_t)fld_val;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_msg_q_depth_get
 */
static uint32_t
bcmptm_ser_msg_q_depth_get(int unit)
{
    uint32_t intr_msg_q_depth = MSG_Q_DEFAULT_DEPTH;

    (void)bcmptm_ser_config_field_data_get(unit, SER_CONFIGt_MESSAGE_Q_DEPTHf,
                                           &intr_msg_q_depth);
    return intr_msg_q_depth;
}

/******************************************************************************
 * bcmptm_ser_lts_imm_create
 */
static int
bcmptm_ser_lts_imm_create(int unit, bool warm)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!warm) {
        rv = bcmptm_ser_control_imm_init(unit);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_stats_imm_update(unit, 1);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_injection_status_imm_update(unit, 1);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_hw_fault_notify(unit, INVALID_PT_ID, 0, 0, 1);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_injection_imm_init(unit);
        SHR_IF_ERR_EXIT(rv);

        /* SER_CONTROL_PT IMM LT is created in routine bcmptm_ser_pt_ser_enable */
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to create SER LTs\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_lts_cache_sync
 */
static int
bcmptm_ser_lts_cache_sync(int unit, int warm)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Not need to sync cache of SER_NOTIFICATIONt/SER_INJECTION_STATUSt/SER_INJECTIONt */
    if (warm) {
        rv = bcmptm_ser_control_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_control_pt_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);
        /* If cold booting, default value of this LT is zero */
        rv = bcmptm_ser_stats_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to sync data of IMM LTs to cache\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_lts_imm_register
 */
static int
bcmptm_ser_lts_imm_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmptm_ser_control_callback_register(unit));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_control_pt_callback_register(unit));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_injection_callback_register(unit));
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/******************************************************************************
 * bcmptm_ser_init
 */
int
bcmptm_ser_init(int unit, bool warm)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    bcmptm_ser_cth_cb_register(&bcmptm_cth_cb);

    /* space for slice mode information */
    bcmptm_ser_slice_mode_info_init(unit, warm);

    if (!bcmptm_ser_checking_enable(unit)) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }
    bcmptm_ser_data_cb[unit] = bcmptm_ser_data_driver_register(unit);
    if (bcmptm_ser_data_cb[unit] == NULL) {
        rv = SHR_E_UNAVAIL;
        SHR_IF_ERR_EXIT(rv);
    }
    rv = bcmptm_ser_control_pt_cache_init(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmptm_ser_lts_imm_create(unit, warm);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmptm_ser_lts_cache_sync(unit, warm);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmptm_ser_lts_imm_register(unit);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to initialize SER component\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_cleanup
 */
int
bcmptm_ser_cleanup(int unit, bool warm)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* space for slice mode information */
    bcmptm_ser_slice_mode_info_deinit(unit, warm);

    if (!bcmptm_ser_checking_enable(unit)) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }

    bcmptm_ser_control_pt_cache_deinit(unit);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to cleanup SER component\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_imm_fld_set
 */
void
bcmptm_ser_imm_fld_set(bcmltd_fields_t lt_flds, int index, uint32_t fid_lt,
                       int array_fld_idx, uint32_t data)
{
    lt_flds.field[index]->id = fid_lt;
    lt_flds.field[index]->data = data;
    lt_flds.field[index]->idx = array_fld_idx;
}

/******************************************************************************
 * bcmptm_ser_imm_flds_free
 */
void
bcmptm_ser_imm_flds_free(int unit, shr_fmm_t** fld_array, uint32_t fld_num)
{
    int i = (int)fld_num;

    for (i -= 1; i >= 0; i--) {
        sal_free(fld_array[i]);
        fld_array[i] = NULL;
    }
    sal_free(fld_array);
    fld_array = NULL;

}

/******************************************************************************
 * bcmptm_ser_imm_flds_allocate
 */
shr_fmm_t**
bcmptm_ser_imm_flds_allocate(int unit, uint32_t fld_num)
{
    shr_fmm_t** fld_array = NULL;
    int i = 0, num = (int)fld_num;

    fld_array = sal_alloc(sizeof(shr_fmm_t *) * num, "shr_fmm_t pointer");
    if (fld_array == NULL) {
        return NULL;
    }
    for (i = 0; i < num; i++) {
        fld_array[i] = sal_alloc(sizeof(shr_fmm_t), "shr_fmm_t entity");
        sal_memset(fld_array[i], 0, sizeof(shr_fmm_t));
        if (fld_array[i] == NULL) {
            break;
        }
    }
    if (i == num) {
        return fld_array;
    } else {
        bcmptm_ser_imm_flds_free(unit, fld_array, i);
        return NULL;
    }
}

/******************************************************************************
 * bcmptm_ser_checking_enable
 */
bool
bcmptm_ser_checking_enable(int unit)
{
    uint32_t  ser_checking = SER_CHECKING_DISABLE;
    bool      psim = FALSE, bcmsim = FALSE;
    bool      enable = FALSE;

    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bcmsim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    if (psim || bcmsim) {
        ser_checking = SER_CHECKING_DISABLE;
    } else {
        (void)bcmptm_ser_config_field_data_get(unit, SER_CONFIGt_SER_ENABLEf,
                                               &ser_checking);
    }
    enable = ser_checking ? SER_CHECKING_ENABLE : SER_CHECKING_DISABLE;
    return enable;
}

