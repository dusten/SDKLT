/*! \file ser_lt_ser_control.c
 *
 * Functions used to get or update cache and IMM of SER_CONTROLt
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
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmltd/chip/bcmltd_id.h>

#include "ser_lts.h"
#include "ser_lt_ser_control.h"
/******************************************************************************
* Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER


/******************************************************************************
 * Typedefs
 */

/******************************************************************************
 * Private variables
 */
/* Cache for IMM LT SER_CONTROL */
/* Default value should be modified according "map SER_CONTROL" */
static bcmptm_ser_lt_field_info_t lt_ser_control_cache[] = {
    { SER_CONTROLt_TCAM_SCANf, {0}, TRUE },
    { SER_CONTROLt_TCAM_SCAN_INTERVALf, {0}, 10000000},
    { SER_CONTROLt_TCAM_SCAN_CHUNK_SIZEf, {0}, 256 },
    { SER_CONTROLt_TCAM_ENTRIES_READ_PER_INTERVALf, {0}, 4096 },
    { SER_CONTROLt_SRAM_SCANf, {0}, TRUE },
    { SER_CONTROLt_SRAM_SCAN_INTERVALf, {0}, 100000000 },
    { SER_CONTROLt_SRAM_SCAN_CHUNK_SIZEf, {0}, 256 },
    { SER_CONTROLt_SRAM_ENTRIES_READ_PER_INTERVALf, {0}, 4096 },
    { SER_CONTROLt_REPORT_SINGLE_BIT_ECCf, {0}, TRUE },
    { SER_CONTROLt_SER_LOGGINGf, {0}, TRUE },
    { SER_CONTROLt_ERRONEOUS_ENTRIES_LOGGINGf, {0}, FALSE },
    { SER_CONTROLt_HW_FAULTf, {0}, FALSE }
};
static uint32_t lt_ser_control_field_num = COUNTOF(lt_ser_control_cache);

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_CONTROLt logical table for entry validation.
 */
static int
bcmptm_ser_control_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                           bcmimm_entry_event_t event_reason,
                                           const bcmltd_field_t *key,
                                           const bcmltd_field_t *data,
                                           void *context);
/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_CONTROLt logical table entry commit stages.
 */
static int
bcmptm_ser_control_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key,
                                      const bcmltd_field_t *data,
                                      void *context,
                                      bcmltd_fields_t *output_fields);

static bcmimm_lt_cb_t bcmptm_ser_control_imm_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_control_imm_validation_callback,

    /*! Staging function. */
    .stage = bcmptm_ser_control_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 * bcmptm_ser_control_fid_check:
 */
static int
bcmptm_ser_control_fid_check(int unit, uint32_t fid_lt)
{
    /* new field is added to LT */
    if (fid_lt >= lt_ser_control_field_num) {
        return SHR_E_INTERNAL;
    }
    /*
    * When field is inserted/deleted to/from SER_CONTROL LT,
    * and lt_ser_control_cache is not updated accordingly.
    */
    if (lt_ser_control_cache[fid_lt].lt_fid != fid_lt) {
        return SHR_E_INTERNAL;
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_control_cache_update: update cache of LT SER_CONTROL by IMM callback
 */
static int
bcmptm_ser_control_cache_update(int unit, uint32_t fid_lt, uint32_t data)
{
    int rv = SHR_E_NONE;

    rv = bcmptm_ser_control_fid_check(unit, fid_lt);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    lt_ser_control_cache[fid_lt].data[unit] = data;
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_control_imm_validation_callback
 */
static int
bcmptm_ser_control_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                           bcmimm_entry_event_t event_reason,
                                           const bcmltd_field_t *key,
                                           const bcmltd_field_t *data,
                                           void *context)
{
    if (!bcmptm_ser_checking_enable(unit)) {
        return SHR_E_DISABLED;
    }
    if (event_reason != BCMIMM_ENTRY_LOOKUP &&
        event_reason != BCMIMM_ENTRY_UPDATE) {
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_control_imm_stage_callback
 */
static int
bcmptm_ser_control_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key,
                                      const bcmltd_field_t *data,
                                      void *context,
                                      bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *lt_field;
    uint32_t lt_fid;
    uint32_t fval;
    int rv = SHR_E_NONE;
    uint32_t old_single_bit, new_single_bit;
    uint32_t tcam_scan_enable_old, tcam_scan_enable_new;
    uint32_t sram_scan_enable_old, sram_scan_enable_new;
    uint32_t pt_id;
    size_t sid_num;

    if (output_fields) {
        output_fields->count = 0;
    }
    old_single_bit = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_REPORT_SINGLE_BIT_ECCf);
    tcam_scan_enable_old = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_TCAM_SCANf);
    sram_scan_enable_old = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_SRAM_SCANf);
    /* Parse data field */
    lt_field = data;

    while (lt_field) {
        lt_fid = lt_field->id;
        fval = lt_field->data;
        rv = bcmptm_ser_control_cache_update(unit, lt_fid, fval);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        lt_field = lt_field->next;
    }
    new_single_bit = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_REPORT_SINGLE_BIT_ECCf);
    tcam_scan_enable_new = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_TCAM_SCANf);
    sram_scan_enable_new = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_SRAM_SCANf);

    /* disable or enable all PTs ECC 1bit report */
    if (new_single_bit != old_single_bit) {
        rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_num);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        for (pt_id = 0; pt_id < sid_num; pt_id++) {
            (void)bcmptm_ser_pt_ser_enable(unit, pt_id, BCMDRD_SER_EN_TYPE_ECC1B,
                                           new_single_bit);
        }
    }
    if (tcam_scan_enable_new != tcam_scan_enable_old) {
        if (tcam_scan_enable_new) {
            rv = bcmptm_ser_tcam_scan_start(unit);
        } else {
            rv = bcmptm_ser_tcam_scan_stop(unit, 0);
        }
    }
    if (sram_scan_enable_new != sram_scan_enable_old) {
        if (sram_scan_enable_new) {
            rv = bcmptm_ser_sram_scan_start(unit, 0);
        } else {
            rv = bcmptm_ser_sram_scan_stop(unit, 0);
        }
    }
    return rv;
}

/******************************************************************************
 * Public Functions
 */

/******************************************************************************
 * bcmptm_ser_control_field_val_get: get field data from cache of SER_CONTROLt
 */
uint32_t
bcmptm_ser_control_field_val_get(int unit, uint32_t fid_lt)
{
    int rv = SHR_E_NONE;

    rv = bcmptm_ser_control_fid_check(unit, fid_lt);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t The field id of LT SER_CONTROLt is invalid\n")));
        return BCMPTM_SER_INVALID_FIELD_VAL;
    }
    return lt_ser_control_cache[fid_lt].data[unit];
}

/******************************************************************************
 * bcmptm_ser_control_callback_register
 */
int
bcmptm_ser_control_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_CONTROLt,
                             &bcmptm_ser_control_imm_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_control_imm_init: insert the first entry to
 * to IMM LT SER_CONTROLt by SER component
 */
int
bcmptm_ser_control_imm_init(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    bcmltd_fields_t lt_flds;
    uint32_t fld_num = lt_ser_control_field_num, i = 0;

    SHR_FUNC_ENTER(unit);

    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    if (fld_array == NULL) {
        return SHR_E_MEMORY;
    }
    lt_flds.count = fld_num;
    lt_flds.field = fld_array;

    for (i = 0; i < fld_num; i++) {
        /* Set field value as default value */
        lt_ser_control_cache[i].data[unit] = lt_ser_control_cache[i].default_val;
        bcmptm_ser_imm_fld_set(lt_flds, i, lt_ser_control_cache[i].lt_fid,
                               0, lt_ser_control_cache[i].data[unit]);
    }
    rv = bcmimm_entry_insert(unit, SER_CONTROLt, &lt_flds);
    SHR_IF_ERR_EXIT(rv);

exit:
    bcmptm_ser_imm_flds_free(unit, fld_array, fld_num);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to commit data to SER_INJECTIONt\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_control_cache_sync
 */
int
bcmptm_ser_control_cache_sync(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    size_t i = 0;
    bcmltd_fields_t lt_flds_in, lt_flds_out;

    SHR_FUNC_ENTER(unit);

    lt_flds_out.field = NULL;

    lt_flds_in.count = 0;
    lt_flds_in.field = NULL;

    fld_array = bcmptm_ser_imm_flds_allocate(unit, lt_ser_control_field_num);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds_out.field = fld_array;
    lt_flds_out.count = lt_ser_control_field_num;

    rv = bcmimm_entry_lookup(unit, SER_CONTROLt, &lt_flds_in, &lt_flds_out);
    SHR_IF_ERR_EXIT(rv);

    for (i = 0; i < lt_flds_out.count; i++) {
        rv = bcmptm_ser_control_cache_update(unit, lt_flds_out.field[i]->id,
                                             lt_flds_out.field[i]->data);
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    if (lt_flds_out.field != NULL) {
        /* lt_flds_out.count may be modified by bcmimm_entry_lookup */
        bcmptm_ser_imm_flds_free(unit, lt_flds_out.field, lt_ser_control_field_num);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to sync IMM SER_CONTROLt data to cache\n")));
    }
    SHR_FUNC_EXIT();
}

