/*! \file ser_lt_ser_injection.c
 *
 * Functions used to get or update IMM LT SER_INJECTION
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
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>

#include "ser_lts.h"
#include "ser_lt_ser_injection.h"
#include "ser_lt_ser_injection_status.h"
#include "ser_lt_ser_control_pt.h"
#include "ser_inject_and_test.h"
 /******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER


/******************************************************************************
 * Typedefs
 */
/*!
 * \brief Get data of a field from cache of LT SER_INJECTIONt.
 *
 *
 * \param [in] unit           Logical device id.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or BCMPTM_SER_INVALID_FIELD_VAL
 */
extern uint32_t
bcmptm_ser_injection_field_val_get(int unit, uint32_t fid_lt);

/*! Macros to get value of fields of SER_INJECTIONt */
#define  BCMPTM_SER_INJECTION_PT_ID(_unit) \
            bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_PT_IDf)

#define  BCMPTM_SER_INJECTION_PT_INSTANCE(_unit) \
            bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_PT_INSTANCEf)

#define  BCMPTM_SER_INJECTION_PT_INDEX(_unit) \
            bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_PT_INDEXf)

#define  BCMPTM_SER_INJECTION_XOR_BANK(_unit) \
            bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_XOR_BANKf)

#define  BCMPTM_SER_INJECTION_INJECT_ERR_BIT_NUM(_unit) \
            bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_INJECT_ERR_BIT_NUMf)

#define  BCMPTM_SER_INJECTION_INJECT_VALIDATE(_unit) \
            bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_INJECT_VALIDATEf)

#define  BCMPTM_SER_INJECTION_TIME_TO_WAIT(_unit) \
            bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_TIME_TO_WAITf)

/******************************************************************************
 * Private variables
 */
/* Cache for IMM LT SER_INJECTION */
static bcmptm_ser_lt_field_info_t lt_ser_injection_cache[] = {
    { SER_INJECTIONt_PT_IDf, {0}, INVALID_PT_ID },
    { SER_INJECTIONt_PT_INSTANCEf, {0} },
    { SER_INJECTIONt_PT_INDEXf, {0} },
    { SER_INJECTIONt_XOR_BANKf, {0} },
    { SER_INJECTIONt_INJECT_ERR_BIT_NUMf, {0}, SER_SINGLE_BIT_ERR },
    { SER_INJECTIONt_INJECT_VALIDATEf, {0}, SER_VALIDATION },
    { SER_INJECTIONt_TIME_TO_WAITf, {0}, 100000 }
};
static uint32_t lt_ser_injection_field_num = COUNTOF(lt_ser_injection_cache);

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_INJECTIONt logical table for entry validation.
 */
static int
bcmptm_ser_injection_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key,
                                             const bcmltd_field_t *data,
                                             void *context);
/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_INJECTIONt logical table entry commit stages.
 */
static int
bcmptm_ser_injection_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                        uint32_t trans_id,
                                        bcmimm_entry_event_t event_reason,
                                        const bcmltd_field_t *key,
                                        const bcmltd_field_t *data,
                                        void *context,
                                        bcmltd_fields_t *output_fields);

static bcmimm_lt_cb_t bcmptm_ser_injection_imm_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_injection_imm_validation_callback,

    /*! Staging function. */
    .stage = bcmptm_ser_injection_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 * bcmptm_ser_injection_fid_check
 */
static int
bcmptm_ser_injection_fid_check(int unit, uint32_t fid_lt)
{
    /* new field is added to LT */
    if (fid_lt >= lt_ser_injection_field_num) {
        return SHR_E_INTERNAL;
    }
    /*
    * When field is inserted/deleted to/from SER_INJECTION LT,
    * and lt_ser_injection_cache is not updated accordingly.
    */
    if (lt_ser_injection_cache[fid_lt].lt_fid != fid_lt) {
        return SHR_E_INTERNAL;
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_injection_cache_update: update cache of LT SER_INJECTION by IMM callback
 */
static int
bcmptm_ser_injection_cache_update(int unit, uint32_t fid_lt, uint32_t data)
{
    int rv = SHR_E_NONE;

    rv = bcmptm_ser_injection_fid_check(unit, fid_lt);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    lt_ser_injection_cache[fid_lt].data[unit] = data;
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_injection_imm_validation_callback
 */
static int
bcmptm_ser_injection_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key,
                                             const bcmltd_field_t *data,
                                             void *context)
{
    const bcmltd_field_t *lt_field;
    uint32_t lt_fid, fval;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t pt_id;
    int index = 0, valid = 0, max_index = 0, inject_inst = 0;
    uint32_t xor_bank = 0;
    bcmdrd_pt_ser_info_t ser_info;

    SHR_FUNC_ENTER(unit);

    if (!bcmptm_ser_checking_enable(unit)) {
        return SHR_E_DISABLED;
    }
    if (event_reason != BCMIMM_ENTRY_LOOKUP &&
        event_reason != BCMIMM_ENTRY_UPDATE) {
        return SHR_E_UNAVAIL;
    }
    /* Parse data field */
    lt_field = data;

    while (lt_field) {
        lt_fid = lt_field->id;
        fval = lt_field->data;
        rv = bcmptm_ser_injection_cache_update(unit, lt_fid, fval);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        lt_field = lt_field->next;
    }
    pt_id = BCMPTM_SER_INJECTION_PT_ID(unit);
    inject_inst = (int)BCMPTM_SER_INJECTION_PT_INSTANCE(unit);
    index = (int)BCMPTM_SER_INJECTION_PT_INDEX(unit);
    xor_bank = BCMPTM_SER_INJECTION_XOR_BANK(unit);

    if (bcmdrd_pt_is_valid(unit, pt_id) == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "[%s] is invalid. It has OVERLAY flag!\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
        return SHR_E_CONFIG;
    }

    /* Some slices are configured as narrow mode, and others can configured as wide mode */
    rv = bcmptm_ser_paired_tcam_index_valid(unit, pt_id, index, &valid);
    if (SHR_FAILURE(rv) || (valid == 0)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Injection index [%d] of [%s] is invalid."
                             " Pay attention to slice mode.\n"),
                  index, bcmdrd_pt_sid_to_name(unit, pt_id)));
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }
    valid = bcmptm_ser_sram_index_valid(unit, pt_id, index, &max_index);
    if (valid == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Injection index of [%s] is invalid,"
                             " Min index [0], Max index [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id), max_index));
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }
    if (bcmptm_ser_control_pt_field_val_get(unit, pt_id, SER_CONTROL_PTt_HW_FAULTf)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "There is hard fault in [%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
        rv = SHR_E_FAIL;
        SHR_RETURN_VAL_EXIT(rv);
    }
    if (bcmdrd_pt_attr_is_fifo(unit, pt_id)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "FIFO [%s] cannot be injected SER error\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
        rv = SHR_E_UNAVAIL;
        SHR_RETURN_VAL_EXIT(rv);
    }
    /* no ser info */
    rv = bcmdrd_pt_ser_info_get(unit, pt_id, &ser_info);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "[%s][%d] may not support SER protection\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id), pt_id));
        SHR_RETURN_VAL_EXIT(rv);
    }
    /*
    * If xor_bank is 1, inject SER error to XOR bank.
    * If xor_bank is 0, inject SER error to table bank.
    */
    if (xor_bank == 0) {
        rv = SHR_E_NONE;
    } else if (ser_info.type != BCMDRD_SER_TYPE_DIRECT_XOR &&
               ser_info.type != BCMDRD_SER_TYPE_DIRECT_XOR_LP &&
               ser_info.type != BCMDRD_SER_TYPE_HASH_XOR &&
               ser_info.type != BCMDRD_SER_TYPE_HASH_XOR_LP) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "[%s][%d] may have no XOR bank, its SER type is [%d],"
                             " please assign XOR_BANKf as [0]\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id), pt_id, ser_info.type));
        rv = SHR_E_CONFIG;
        SHR_RETURN_VAL_EXIT(rv);
    }
    SHR_RETURN_VAL_EXIT(rv);

exit:
    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        BCMPTM_SER_INJECTION_STATUS_PT_ID_SET(unit, pt_id);
        BCMPTM_SER_INJECTION_STATUS_PT_INSTANCE_SET(unit, inject_inst);
        BCMPTM_SER_INJECTION_STATUS_PT_INDEX_SET(unit, index);
        BCMPTM_SER_INJECTION_STATUS_XOR_BANK_SET(unit, xor_bank);
        BCMPTM_SER_INJECTION_STATUS_SER_ERR_CORRECTED_SET(unit, 0);
        BCMPTM_SER_INJECTION_STATUS_SER_ERR_INJECTED_SET(unit, 0);
        (void)bcmptm_ser_injection_status_imm_update(unit, 0);
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_refresh_regs_disable
 */
static int
bcmptm_ser_refresh_regs_disable(int unit)
{
    uint32_t info_num = 0, i = 0, fval = 0, entry_data = 0;
    bcmptm_ser_ctrl_reg_info_t *mem_refresh_info = NULL;
    bcmptm_ser_fld_info_t      *ctrl_flds_info = NULL;
    bcmdrd_sid_t sid = INVALIDm;
    bcmdrd_fid_t fid = INVALIDf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    (void)BCMPTM_SER_DATA_DRIVER_CB(unit, refresh_regs_info_get_fn)(unit, &mem_refresh_info, &info_num);
    for (i = 0; i < info_num; i++) {
        sid = mem_refresh_info[i].ctrl_reg;
        ctrl_flds_info = mem_refresh_info[i].ctrl_flds_val;
        entry_data = 0;
        rv = bcmptm_ser_pt_read(unit, sid, 0, -1, NULL, &entry_data, 1,
                                BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        fid = ctrl_flds_info->ctrl_fld;
        rv = bcmdrd_pt_field_get(unit, sid, &entry_data, fid, &fval);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* save old value */
        ctrl_flds_info->val[unit] = fval;
        if (fval != 0) {
            /* disable refresh */
            fval = 0;
            rv = bcmdrd_pt_field_set(unit, sid, &entry_data, fid, &fval);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            rv = bcmptm_ser_pt_write(unit, sid, 0, -1, NULL, &entry_data, 0);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to write [%s] to disable refresh function\n"),
                   bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_refresh_regs_restore
 */
static int
bcmptm_ser_refresh_regs_restore(int unit)
{
    uint32_t info_num = 0, i = 0, fval = 0, entry_data = 0;
    bcmptm_ser_ctrl_reg_info_t *mem_refresh_info = NULL;
    bcmptm_ser_fld_info_t      *ctrl_flds_info = NULL;
    bcmdrd_sid_t sid = INVALIDm;
    bcmdrd_fid_t fid = INVALIDf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    (void)BCMPTM_SER_DATA_DRIVER_CB(unit, refresh_regs_info_get_fn)(unit, &mem_refresh_info, &info_num);
    for (i = 0; i < info_num; i++) {
        ctrl_flds_info = mem_refresh_info[i].ctrl_flds_val;
        fval = ctrl_flds_info->val[unit];
        /* already disable refresh function before bcmptm_ser_refresh_regs_disable */
        if (fval == 0) {
            continue;
        } else {
            sid = mem_refresh_info[i].ctrl_reg;
            fid = ctrl_flds_info->ctrl_fld;
            entry_data = 0;
            /* restore refresh function */
            rv = bcmptm_ser_pt_read(unit, sid, 0, -1, NULL, &entry_data, 1,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            rv = bcmdrd_pt_field_set(unit, sid, &entry_data, fid, &fval);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            rv = bcmptm_ser_pt_write(unit, sid, 0, -1, NULL, &entry_data, 0);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to write [%s] to restore refresh function\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
*  bcmptm_ser_data_split_info_update
 */
static void
bcmptm_ser_data_split_info_update(int unit, bcmdrd_sid_t pt_id, int *tbl_inst,
                                  uint32_t *flags, bcmdrd_fid_t *data_split_field)
{
    int rv = SHR_E_NONE;
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    const bcmdrd_symbols_t *symbols;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, pt_id, &acc_type, &acctype_str);
    if (SHR_FAILURE(rv)) {
        return;
    }
    if (!PT_IS_DATA_SPLIT_ACC_TYPE(acctype_str)) {
        return;
    }

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, data_split_mem_test_fid_get_fn)(unit, pt_id, *tbl_inst, data_split_field);

    if (*data_split_field != INVALIDf) {
        symbols = bcmdrd_dev_symbols_get(unit, 0);
        symbol = bcmdrd_sym_field_info_get(symbols, pt_id, *data_split_field, &finfo);
        if (NULL != symbol) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The SER error will be injected to field [%s] which only in instance [%d]\n"),
                      finfo.name, *tbl_inst));
        } else {
            *data_split_field = INVALIDf;
        }
        /* such fields just have 1bit, so inject other fields */
        if (finfo.maxbit == finfo.minbit && (*flags & BCMPTM_SER_INJECT_ERROR_2BIT)) {
            *data_split_field = INVALIDf;
        }
        if (*tbl_inst > 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Forcibly change instance from [%d] to 0, access type of [%s] is data-split.\n"),
                      *tbl_inst, bcmdrd_pt_sid_to_name(unit, pt_id)));
            *tbl_inst = 0;
        }
    }
    if (*data_split_field == INVALIDf) {
        if (*tbl_inst > 0) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Forcibly change instance from [%d] to 0,"
                                 " access type of [%s] is data-split.\n"),
                      *tbl_inst, bcmdrd_pt_sid_to_name(unit, pt_id)));
            *tbl_inst = 0;
        }
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The SER error will be injected to all instances of PT,"
                             " and all instances can be triggered SER event sperately...\n")));
    }
}

/******************************************************************************
*  bcmptm_ser_pt_inst_check
 */
static int
bcmptm_ser_pt_inst_check(int unit, bcmdrd_sid_t pt_id,
                         int *tbl_inst, uint32_t *index)
{
    int rv = SHR_E_NONE, tbl_inst_num = 0, pipe_num = 0;
    uint32_t pt_depth_per_inst = 0, pt_depth_inst_start = 0, pt_depth_inst_end = 0;
    uint32_t max_index = 0, min_index = 0;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, pt_id, &acc_type, &acctype_str);
    if(SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcmptm_ser_tbl_inst_num_get(unit, pt_id, &tbl_inst_num);
    if(SHR_FAILURE(rv)) {
        return rv;
    }
    /*
    * Memory EGR_1588_SAm just have one instance, its access type is ADDR_SPLIT_SPLIT.
    * Register IBCAST_64r etc. have 140 instance, its access type also is ADDR_SPLIT_SPLIT.
    */
    if ((PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str) ||
         PT_IS_ADDR_SPLIT_SPLIT_ACC_TYPE(acctype_str))) {
        if (*tbl_inst < tbl_inst_num) {
            return SHR_E_NONE;
        } else {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Injection instance [%d] of [%s] should not be larger than [%d]\n"),
                      *tbl_inst, bcmdrd_pt_sid_to_name(unit, pt_id), tbl_inst_num));
        }
        max_index = bcmdrd_pt_index_max(unit, pt_id);
        min_index = bcmdrd_pt_index_min(unit, pt_id);
        /* Registers which access type is ADDR_SPLIT_SPLIT */
        if (max_index == min_index) {
            return SHR_E_CONFIG;
        }
        pipe_num = bcmdrd_pt_num_pipe_inst(unit, pt_id);
        if (pipe_num <= *tbl_inst) {
            return SHR_E_CONFIG;
        }
        pt_depth_per_inst = (max_index - min_index + 1) / pipe_num;
        pt_depth_inst_start = (*tbl_inst) * pt_depth_per_inst;
        pt_depth_inst_end = pt_depth_inst_start + pt_depth_per_inst - 1;

        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Access-type of [%s] is [%s], BD does not support inst [%d].\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id),
                  PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str) ? "Addr-split" : "Addr-split-split",
                  *tbl_inst));
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Indexes from [%d] to [%d] are in inst [%d].\n"),
                             pt_depth_inst_start, pt_depth_inst_end, *tbl_inst));
        return SHR_E_CONFIG;
    } else if (*tbl_inst >= tbl_inst_num) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Injection inst [%d] of [%s] should not be larger than [%d]\n"),
                  *tbl_inst, bcmdrd_pt_sid_to_name(unit, pt_id), tbl_inst_num));
        return SHR_E_CONFIG;
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_injection_imm_stage_callback
 */
static int
bcmptm_ser_injection_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                        uint32_t trans_id,
                                        bcmimm_entry_event_t event_reason,
                                        const bcmltd_field_t *key,
                                        const bcmltd_field_t *data,
                                        void *context,
                                        bcmltd_fields_t *output_fields)
{
    bcmdrd_sid_t pt_id, aggr_pt_id;
    uint32_t tbl_inst = 0, fv_index = 0, flags = 0, xor_bank = 0;
    uint32_t single_bit_err = SER_SINGLE_BIT_ERR, validate = SER_VALIDATION;
    int rv = SHR_E_NONE, is_tcam = 0, inject_err_to_key_fld = 0;
    sal_time_t usleep = 0;
    bcmdrd_fid_t data_split_field = INVALIDf;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    pt_id = BCMPTM_SER_INJECTION_PT_ID(unit);
    tbl_inst = BCMPTM_SER_INJECTION_PT_INSTANCE(unit);
    fv_index = BCMPTM_SER_INJECTION_PT_INDEX(unit);
    xor_bank = BCMPTM_SER_INJECTION_XOR_BANK(unit);
    single_bit_err = BCMPTM_SER_INJECTION_INJECT_ERR_BIT_NUM(unit);
    flags |= (single_bit_err == SER_SINGLE_BIT_ERR) ? 0 : BCMPTM_SER_INJECT_ERROR_2BIT;
    flags |= (xor_bank) ? BCMPTM_SER_INJECT_ERROR_XOR_BANK : 0;

    rv = bcmptm_ser_refresh_regs_disable(unit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* map L3_TUNNELm to L3_TUNNEL_DATA_ONLYm*/
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, ip_ep_sram_remap_fn)(unit, pt_id, &pt_id);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    is_tcam = bcmdrd_pt_attr_is_cam(unit, pt_id);
    if (is_tcam) {
        /* pt_id can be equal to aggr_pt_id */
        (void)BCMPTM_SER_DATA_DRIVER_CB(unit, tcam_only_remap_fn)(unit, pt_id, &aggr_pt_id, &inject_err_to_key_fld);
        /* TCAM_ONLY view, inject SER error to key and mask field */
        if (inject_err_to_key_fld) {
            flags |= BCMPTM_SER_INJECT_ERROR_KEY_MASK;
        }
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, pt_id, &acc_type, &acctype_str);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        /* access type of ING_SNATm is data_split */
        if (PT_IS_DATA_SPLIT_ACC_TYPE(acctype_str) ||
            PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str)) {
            tbl_inst = 0;
        }
        /* inject SER error to Tcam_only instead of AGGR view */
        rv = bcmptm_ser_tcam_ser_inject(unit, aggr_pt_id, pt_id,
                                        tbl_inst, fv_index, flags);
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, pt_acctype_get_fn)(unit, aggr_pt_id, &acc_type, &acctype_str);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* Use AGGR view instead of Tcam_only view to validate SER error */
        /* access type of L3_DEFIP_PAIR_128 is data_split; L3_DEFIP_PAIR_128_ONLY is duplicate */
        if (PT_IS_DATA_SPLIT_ACC_TYPE(acctype_str) ||
            PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str)) {
            tbl_inst = 0;
        }
    } else {
        data_split_field = INVALIDf;
        /* parameter 'tbl_inst', 'flags' may be changed */
        bcmptm_ser_data_split_info_update(unit, pt_id, (int *)&tbl_inst,
                                          &flags, &data_split_field);
        /* parameter 'tbl_inst', 'fv_index' may be changed */
        rv = bcmptm_ser_pt_inst_check(unit, pt_id, (int *)&tbl_inst, &fv_index);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (xor_bank) {
            rv = bcmptm_ser_xor_bank_ser_inject(unit, pt_id, data_split_field,
                                                tbl_inst, fv_index, flags);
        } else {
            rv = bcmptm_ser_sram_ser_inject(unit, pt_id, data_split_field,
                                            tbl_inst, fv_index, flags);
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* update PT information to cache of SER_INJECTION_STATUS LT */
    BCMPTM_SER_INJECTION_STATUS_SER_ERR_INJECTED_SET(unit, 1);

    validate = BCMPTM_SER_INJECTION_INJECT_VALIDATE(unit);
    if (validate == SER_VALIDATION) {
        usleep = (sal_time_t)BCMPTM_SER_INJECTION_TIME_TO_WAIT(unit);
        /* For TCAM, use AGGR view to validation */
        if (is_tcam) {
            pt_id = aggr_pt_id;
        }
        rv = bcmptm_ser_err_injection_validate(unit, pt_id, tbl_inst,
                                               fv_index, usleep, flags);
        /* update PT information to cache of SER_INJECTION_STATUS LT */
        BCMPTM_SER_INJECTION_STATUS_SER_ERR_CORRECTED_SET(unit, SHR_FAILURE(rv) ? 0 : 1);
    }
    SHR_RETURN_VAL_EXIT(rv);

exit:
    (void)bcmptm_ser_refresh_regs_restore(unit);

    BCMPTM_SER_INJECTION_STATUS_XOR_BANK_SET(unit, xor_bank);
    /* update PT information to IMM of SER_INJECTION_STATUS LT */
    (void)bcmptm_ser_injection_status_imm_update(unit, 0);
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/******************************************************************************
 * bcmptm_ser_injection_field_val_get: get field data from cache of SER_INJECTIONt
 */
uint32_t
bcmptm_ser_injection_field_val_get(int unit, uint32_t fid_lt)
{
    int rv = bcmptm_ser_injection_fid_check(unit, fid_lt);

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "The field id of LT SER_INJECTIONt is invalid\n")));
        return BCMPTM_SER_INVALID_FIELD_VAL;
    }
    return lt_ser_injection_cache[fid_lt].data[unit];
}

/******************************************************************************
 * bcmptm_ser_injection_imm_init: insert an entry to
 * to IMM LT SER_INJECTIONt by SER component
 */
int
bcmptm_ser_injection_imm_init(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    bcmltd_fields_t lt_flds;
    uint32_t fld_num = lt_ser_injection_field_num, i = 0;
    bcmptm_ser_lt_field_info_t *cache_ptr = lt_ser_injection_cache;

    SHR_FUNC_ENTER(unit);

    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    if (fld_array == NULL) {
        return SHR_E_MEMORY;
    }
    lt_flds.count = fld_num;
    lt_flds.field = fld_array;
    for (i = 0; i < fld_num; i++) {
        cache_ptr[i].data[unit] = cache_ptr[i].default_val;
        bcmptm_ser_imm_fld_set(lt_flds, i, cache_ptr[i].lt_fid,
                               0, cache_ptr[i].data[unit]);
    }
    rv = bcmimm_entry_insert(unit, SER_INJECTIONt, &lt_flds);
    SHR_IF_ERR_EXIT(rv);

exit:
    bcmptm_ser_imm_flds_free(unit, fld_array, fld_num);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to commit data to SER_INJECTIONt\n")));
    }
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * bcmptm_ser_injection_callback_register
 */
int
bcmptm_ser_injection_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_INJECTIONt,
                             &bcmptm_ser_injection_imm_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}

