/*! \file ser_lt_ser_control_pt.c
 *
 * Functions used to get or update IMM SER_CONTROL_PT LT
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
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmltd/chip/bcmltd_id.h>

#include "ser_lts.h"
#include "ser_lt_ser_control_pt.h"
 /******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Bit offset of every field of LT SER_CONTROL_PT */
#define  BCMPTM_SER_PT_ID_BIT_OFFSET                  0
#define  BCMPTM_SER_ECC_PARITY_CHECK_BIT_OFFSET       16
#define  BCMPTM_SER_SW_SCANNED_BIT_OFFSET             17
#define  BCMPTM_SER_HW_SCANNED_BIT_OFFSET             18
#define  BCMPTM_SER_ECC_PARITY_CHECK_OPER_BIT_OFFSET  19
/* PT_ID field mask */
#define  BCMPTM_SER_PT_ID_MASK   0xFFFF

/******************************************************************************
 * Typedefs
 */
/* Used to save field id and field offset of SER_CONTROL_PTt */
typedef struct lt_ser_control_pt_field_s {
    uint32_t  lt_fid;
    uint32_t  bit_offset;
    uint32_t  mask;
} bcmptm_ser_control_pt_field_info_t;

/******************************************************************************
 * Private variables
 */
/* cache handler for LT SER_CONTROL_PT */
static uint32_t *lt_ser_control_pt_cache[BCMDRD_CONFIG_MAX_UNITS];

/* PT number */
static uint32_t pt_num[BCMDRD_CONFIG_MAX_UNITS];

/* Use to protect lt_ser_control_pt_cache */
static sal_mutex_t lt_ser_control_pt_mutex[BCMDRD_CONFIG_MAX_UNITS];

/* Used to check whether fid is valid or not */
static const bcmptm_ser_control_pt_field_info_t lt_ser_control_pt_info[] = {
    { SER_CONTROL_PTt_PT_IDf, BCMPTM_SER_PT_ID_BIT_OFFSET, BCMPTM_SER_PT_ID_MASK}, /* key */
    { SER_CONTROL_PTt_ECC_PARITY_CHECKf, BCMPTM_SER_ECC_PARITY_CHECK_BIT_OFFSET, 1},
    { SER_CONTROL_PTt_SW_SCANf, BCMPTM_SER_SW_SCANNED_BIT_OFFSET, 1},
    { SER_CONTROL_PTt_HW_SCANf, BCMPTM_SER_HW_SCANNED_BIT_OFFSET, 1},
    { SER_CONTROL_PTt_HW_FAULTf, BCMPTM_SER_ECC_PARITY_CHECK_OPER_BIT_OFFSET, 1}
};
static uint32_t lt_ser_control_pt_field_num = COUNTOF(lt_ser_control_pt_info);

/*! Macro for lt_ser_control_pt_mutex */
#define SER_CONTROL_PT_MUTEX_CREATE(_unit) \
    do { \
        if (lt_ser_control_pt_mutex[_unit] == NULL) { \
            lt_ser_control_pt_mutex[_unit] = sal_mutex_create("ser_control_pt_cache"); \
            if (lt_ser_control_pt_mutex[_unit] == NULL) { \
                LOG_WARN(BSL_LOG_MODULE, \
                        (BSL_META_U(_unit, \
                                    "\t Fail to create mutex to protect ser_control_pt cache\n"))); \
                return SHR_E_MEMORY; \
            } \
        } \
    } while(0)

#define SER_CONTROL_PT_MUTEX_DESTROY(_unit) \
    do { \
        if (lt_ser_control_pt_mutex[_unit] != NULL) { \
            sal_mutex_destroy(lt_ser_control_pt_mutex[_unit]); \
            lt_ser_control_pt_mutex[_unit] = NULL; \
        } \
    } while(0)

#define SER_CONTROL_PT_MUTEX_TAKE(_unit) \
    do { \
        if (lt_ser_control_pt_mutex[_unit] != NULL) { \
            (void)sal_mutex_take(lt_ser_control_pt_mutex[_unit], SAL_MUTEX_FOREVER); \
        } else { \
            LOG_WARN(BSL_LOG_MODULE, \
                    (BSL_META_U(_unit, \
                                "\t Find lt_ser_control_pt_mutex is NULL when try to take it\n"))); \
            return SHR_E_EMPTY; \
        } \
    } while(0)

#define SER_CONTROL_PT_MUTEX_GIVE(_unit) \
    do { \
        if (lt_ser_control_pt_mutex[_unit] != NULL) { \
            (void)sal_mutex_give(lt_ser_control_pt_mutex[_unit]); \
        } else { \
            LOG_WARN(BSL_LOG_MODULE, \
                    (BSL_META_U(_unit, \
                                "\t Find lt_ser_control_pt_mutex is NULL when try to release it\n"))); \
            return SHR_E_EMPTY; \
        } \
    } while(0)

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_CONTROL_PTt logical table for entry validation.
 */
static int
bcmptm_ser_control_pt_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                              bcmimm_entry_event_t event_reason,
                                              const bcmltd_field_t *key,
                                              const bcmltd_field_t *data,
                                              void *context);

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_CONTROL_PTt logical table entry commit stages.
 */
static int
bcmptm_ser_control_pt_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                         uint32_t trans_id,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key,
                                         const bcmltd_field_t *data,
                                         void *context,
                                         bcmltd_fields_t *output_fields);

static bcmimm_lt_cb_t bcmptm_ser_control_pt_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_control_pt_imm_validation_callback,

    /*! Staging function. */
    .stage = bcmptm_ser_control_pt_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 * bcmptm_ser_control_pt_fid_check
 */
static int
bcmptm_ser_control_pt_fid_check(int unit, uint32_t pt_id, uint32_t fid_lt)
{
    uint32_t pt_id_cache = INVALIDm;
    uint32_t bit_offset, mask;

    if (lt_ser_control_pt_cache[unit] == NULL) {
        return SHR_E_INIT;
    }
    /* new field is added to LT */
    if (fid_lt >= lt_ser_control_pt_field_num) {
        return SHR_E_INTERNAL;
    }
    /*
    * When field is inserted/deleted to/from SER_INJECTION LT,
    * and lt_ser_injection_cache is not updated accordingly.
    */
    if (lt_ser_control_pt_info[fid_lt].lt_fid != fid_lt) {
        return SHR_E_INTERNAL;
    }
    if (pt_num[unit] < pt_id) {
        return SHR_E_INTERNAL;
    }
    if (SER_CONTROL_PTt_PT_IDf == fid_lt) {
        bit_offset = lt_ser_control_pt_info[fid_lt].bit_offset;
        mask = lt_ser_control_pt_info[fid_lt].mask;
        pt_id_cache = (lt_ser_control_pt_cache[unit][pt_id] >> bit_offset) & mask;
        if (pt_id_cache != 0 && pt_id_cache != pt_id) {
            return SHR_E_INTERNAL;
        }
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_control_pt_cache_update: update cache of LT SER_CONTROL_PT
 * by IMM callback
 */
static int
bcmptm_ser_control_pt_cache_update(int unit, uint32_t pt_id,
                                   uint32_t fid_lt, uint32_t data)
{
    uint32_t bit_offset = 0, mask = 0;
    int rv = SHR_E_NONE;

    rv = bcmptm_ser_control_pt_fid_check(unit, pt_id, fid_lt);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    bit_offset = lt_ser_control_pt_info[fid_lt].bit_offset;
    mask = lt_ser_control_pt_info[fid_lt].mask;

    if (data == 0) {
        lt_ser_control_pt_cache[unit][pt_id] &= ~(mask << bit_offset);
    } else {
        lt_ser_control_pt_cache[unit][pt_id] |= ((data & mask) << bit_offset);
    }
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_control_pt_imm_validation_callback
 */
static int
bcmptm_ser_control_pt_imm_validation_callback(int unit, bcmltd_sid_t ltid,
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
 * bcmptm_ser_control_pt_imm_stage_callback
 */
static int
bcmptm_ser_control_pt_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                         uint32_t trans_id,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key,
                                         const bcmltd_field_t *data,
                                         void *context,
                                         bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *lt_field;
    uint32_t lt_fid, pt_id = INVALIDm, pt_id_mapped = INVALIDm;
    uint32_t fval;
    int rv = SHR_E_NONE, inject_err_to_key_fld;
    uint32_t ser_enable;

    if (output_fields) {
        output_fields->count = 0;
    }
    lt_field = key;
    /* Find PT ID */
    while (lt_field) {
        lt_fid = lt_field->id;
        if (lt_fid == SER_CONTROL_PTt_PT_IDf) {
            pt_id = lt_field->data;
            break;
        }
        lt_field = lt_field->next;
    }
    if (pt_id == INVALIDm) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Please input PT ID\n")));
        return SHR_E_PARAM;
    }

    SER_CONTROL_PT_MUTEX_TAKE(unit);
    /* Parse data field */
    lt_field = data;
    while (lt_field) {
        lt_fid = lt_field->id;
        fval = lt_field->data;
        rv = bcmptm_ser_control_pt_cache_update(unit, pt_id, lt_fid, fval);
        if (SHR_FAILURE(rv)) {
            SER_CONTROL_PT_MUTEX_GIVE(unit);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "\t Fail to update ser_control_pt cache of [%s]\n"),
                       bcmdrd_pt_sid_to_name(unit, pt_id)));
            return rv;
        }
        lt_field = lt_field->next;
    }
    SER_CONTROL_PT_MUTEX_GIVE(unit);
    /* Get latest value */
    ser_enable = bcmptm_ser_control_pt_field_val_get(unit, pt_id, SER_CONTROL_PTt_ECC_PARITY_CHECKf);

    (void)BCMPTM_SER_DATA_DRIVER_CB(unit, tcam_only_remap_fn)(unit, pt_id, &pt_id_mapped, &inject_err_to_key_fld);
    if (pt_id != pt_id_mapped) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " [%s] and [%s] share the same physical table.\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id_mapped),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Parity or ecc checking has been not enabled or disabled in H/W, so far.\n")));
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " If you want to enable or disable SER protection,"
                             " please try to execute command"
                             " \'lt SER_CONTROL_PT update PT_ID=%s \'.\n"),
                             bcmdrd_pt_sid_to_name(unit, pt_id_mapped)));
    } else {
        /* H/W write, disable or re-enable SER checking for one PT */
        if (!bcmdrd_pt_attr_is_cam(unit,  pt_id)) {
            rv = bcmptm_ser_pt_ser_enable(unit, pt_id,
                                          BCMDRD_SER_EN_TYPE_EC, ser_enable);
        } else {
            /* map L3_TUNNEm to L3_TUNNEL_ONLYm */
            (void)BCMPTM_SER_DATA_DRIVER_CB(unit, ip_ep_sram_remap_fn)(unit, pt_id, &pt_id);
            /* if pt_id == L3_TUNNEm, the following routine will return failure. */
            rv = bcmptm_ser_tcam_pt_ser_enable(unit, pt_id, ser_enable);
        }
    }
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return rv;
}

/******************************************************************************
 * bcmptm_ser_control_pt_insert: insert entries to IMM LT SER_CONTROL_PT by SER component
 */
static int
bcmptm_ser_control_pt_insert(int unit,
                             bcmptm_ser_lt_flds_update_t *fld_info_array,
                             uint32_t num)
{
    int rv = SHR_E_NONE;
    uint32_t pt_id = INVALIDm, i = 0;
    bcmltd_fields_t lt_flds;

    if (num > lt_ser_control_pt_field_num) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Field number is wrong\t\n")));
        return SHR_E_PARAM;
    }
    /* Find PT ID */
    for (i = 0; i < num; i++) {
        if (fld_info_array[i].lt_fid == SER_CONTROL_PTt_PT_IDf) {
            pt_id = fld_info_array[i].data;
            break;
        }
    }
    /* PT_ID is not found */
    if (pt_id == INVALIDm || pt_id == INVALIDr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t PT ID is invalid\n")));
        return SHR_E_PARAM;
    }
    lt_flds.count = num;
    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, num);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }
    SER_CONTROL_PT_MUTEX_TAKE(unit);
    for (i = 0; i < num; i++) {
        bcmptm_ser_imm_fld_set(lt_flds, i, fld_info_array[i].lt_fid, 0,
                               fld_info_array[i].data);

        rv = bcmptm_ser_control_pt_cache_update(unit, pt_id,
                                                fld_info_array[i].lt_fid,
                                                fld_info_array[i].data);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "\t Fail to update cache ser_control_pt\n")));
            break;
        }
    }
    SER_CONTROL_PT_MUTEX_GIVE(unit);

    if (SHR_FAILURE(rv)) {
        bcmptm_ser_imm_flds_free(unit, lt_flds.field, num);
        return rv;
    }
    rv = bcmimm_entry_insert(unit, SER_CONTROL_PTt, &lt_flds);

    if (rv == SHR_E_EXISTS) {
        rv = bcmimm_entry_update(unit, true, SER_CONTROL_PTt, &lt_flds);
    }
    bcmptm_ser_imm_flds_free(unit, lt_flds.field, num);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to update IMM ser_control_pt\n")));
    }
    return rv;
}

/******************************************************************************
 * Public Functions
 */

/******************************************************************************
 * bcmptm_ser_control_pt_imm_update: update IMM LT SER_CONTROL_PT by SER component
 */
int
bcmptm_ser_control_pt_imm_update(int unit, bcmdrd_sid_t pt_id, uint32_t fid_lt, uint32_t data)
{
    bcmltd_fields_t lt_flds;
    int rv = SHR_E_NONE;

    if (fid_lt == SER_CONTROL_PTt_PT_IDf) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Can't to update field SER_CONTROL_PTt_PT_IDf\n")));
        return SHR_E_PARAM;
    }
    lt_flds.count = 2;
    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, 2);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }
    /* key */
    bcmptm_ser_imm_fld_set(lt_flds, 0, SER_CONTROL_PTt_PT_IDf, 0, pt_id);
    /* data */
    bcmptm_ser_imm_fld_set(lt_flds, 1, fid_lt, 0, data);

    rv = bcmimm_entry_update(unit, true, SER_CONTROL_PTt, &lt_flds);

    bcmptm_ser_imm_flds_free(unit, lt_flds.field, 2);

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to update data of SER_CONTROL_PTt\n")));
        return rv;
    }

    SER_CONTROL_PT_MUTEX_TAKE(unit);
    rv = bcmptm_ser_control_pt_cache_update(unit, pt_id, fid_lt, data);
    SER_CONTROL_PT_MUTEX_GIVE(unit);

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to update data to cache of SER_CONTROL_PTt\n")));
    }
    return rv;
}

/******************************************************************************
* bcmptm_ser_control_pt_imm_insert used to insert new entry to IMM LT SER_CONTROL_PTt
 */
int
bcmptm_ser_control_pt_imm_insert(int unit, bcmdrd_sid_t sid, int enable)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_lt_flds_update_t fld_info_enable[] = {
            {SER_CONTROL_PTt_PT_IDf},
            {SER_CONTROL_PTt_ECC_PARITY_CHECKf, 1},
            {SER_CONTROL_PTt_SW_SCANf, 1}
        };
    bcmptm_ser_lt_flds_update_t fld_info_disable[] = {
            {SER_CONTROL_PTt_PT_IDf},
            {SER_CONTROL_PTt_ECC_PARITY_CHECKf, 0},
            {SER_CONTROL_PTt_SW_SCANf, 0}
        };
    bcmptm_ser_lt_flds_update_t *fld_info_ptr = enable ? fld_info_enable: fld_info_disable;
    uint32_t num = enable ? COUNTOF(fld_info_enable) : COUNTOF(fld_info_disable);

    SHR_FUNC_ENTER(unit);

    fld_info_ptr[0].data = sid;

    rv = bcmptm_ser_control_pt_insert(unit, fld_info_ptr, num);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to record control information for PT[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_control_pt_field_val_get: get field data from cache of SER_CONTROL_PTt
 */
uint32_t
bcmptm_ser_control_pt_field_val_get(int unit, bcmdrd_sid_t pt_id, uint32_t fid_lt)
{
    uint32_t bit_offset = 0, mask = 0, data = 0;
    int rv = bcmptm_ser_control_pt_fid_check(unit, pt_id, fid_lt);

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t The field id of LT SER_CONTROL_PTt is invalid\n")));
        return BCMPTM_SER_INVALID_FIELD_VAL;
    }
    bit_offset = lt_ser_control_pt_info[fid_lt].bit_offset;
    mask = lt_ser_control_pt_info[fid_lt].mask;

    SER_CONTROL_PT_MUTEX_TAKE(unit);
    data = ((lt_ser_control_pt_cache[unit][pt_id] >> bit_offset) & mask);
    SER_CONTROL_PT_MUTEX_GIVE(unit);

    return data;
}

/******************************************************************************
 * bcmptm_ser_control_pt_cache_init
 */
int
bcmptm_ser_control_pt_cache_init(int unit)
{
    size_t sid_num = 0;
    uint32_t *ptr = NULL, cache_size = 0;
    int rv = SHR_E_NONE;

    if (lt_ser_control_pt_cache[unit] == NULL) {
        rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_num);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        if (sid_num >= BCMPTM_SER_PT_ID_MASK) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                               "\t Number of PTs is overflow\n")));
            return SHR_E_PARAM;
        }
        pt_num[unit] = sid_num;
        cache_size = sizeof(uint32_t) * sid_num;
        ptr = sal_alloc(cache_size, "ser_control_pt_cache");
        if (ptr == NULL) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                               "\t Fail to allocate memory space for ser_control_pt cache\n")));
            return SHR_E_MEMORY;
        }
        sal_memset(ptr, 0, cache_size);
        lt_ser_control_pt_cache[unit] = ptr;
    }
    /* Create Mutex to protect SER_CONTROL_PT cache */
    SER_CONTROL_PT_MUTEX_CREATE(unit);
    return SHR_E_NONE;
}

/******************************************************************************
 * bcmptm_ser_control_pt_cache_deinit
 */
void
bcmptm_ser_control_pt_cache_deinit(int unit)
{

    if (lt_ser_control_pt_cache[unit] != NULL) {
        sal_free(lt_ser_control_pt_cache[unit]);
        lt_ser_control_pt_cache[unit] = NULL;
    }
    SER_CONTROL_PT_MUTEX_DESTROY(unit);
}

/******************************************************************************
 * bcmptm_ser_control_pt_sync
 */
int
bcmptm_ser_control_pt_cache_sync(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    size_t i = 0;
    bcmltd_fields_t lt_flds_in, lt_flds_out;
    uint32_t pt_id = 0, num;

    SHR_FUNC_ENTER(unit);

    lt_flds_in.field = NULL;
    lt_flds_out.field = NULL;

    fld_array = bcmptm_ser_imm_flds_allocate(unit, 1);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds_in.count = 1;
    lt_flds_in.field = fld_array;

    fld_array = bcmptm_ser_imm_flds_allocate(unit, lt_ser_control_pt_field_num);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds_out.count = lt_ser_control_pt_field_num;
    lt_flds_out.field = fld_array;

    num = pt_num[unit];
    for (pt_id = 0; pt_id < num; pt_id++) {
        /* key */
        bcmptm_ser_imm_fld_set(lt_flds_in, 0, lt_ser_control_pt_info[0].lt_fid, 0, pt_id);

        rv = bcmimm_entry_lookup(unit, SER_CONTROL_PTt, &lt_flds_in, &lt_flds_out);
        SHR_IF_ERR_EXIT(rv);

        for (i = 0; i < lt_flds_out.count; i++) {
            rv = bcmptm_ser_control_pt_cache_update(unit, pt_id, lt_flds_out.field[i]->id,
                                                    lt_flds_out.field[i]->data);
            SHR_IF_ERR_EXIT(rv);
        }
    }
exit:
    if (lt_flds_in.field) {
        bcmptm_ser_imm_flds_free(unit, lt_flds_in.field, lt_flds_in.count);
    }
    if (lt_flds_out.field != NULL) {
        /* lt_flds_out.count may be modified by bcmimm_entry_lookup */
        bcmptm_ser_imm_flds_free(unit, lt_flds_out.field, lt_ser_control_pt_field_num);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "\t Fail to sync IMM SER_CONTROL_PTt data to cache\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_control_pt_callback_register
 */
int
bcmptm_ser_control_pt_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_CONTROL_PTt,
                             &bcmptm_ser_control_pt_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}


