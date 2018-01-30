/*! \file ser_inject_and_test.c
 *
 * Functions for SER inject and test
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
#include <bcmdrd/bcmdrd_symbols.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_libc.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmltd/chip/bcmltd_id.h>

#include "ser_inject_and_test.h"
#include "ser_lt_ser_injection_status.h"
#include "ser_lt_ser_control_pt.h"
#include "ser_lt_ser_stats.h"
 /******************************************************************************
 * Defines
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Max numer of fields which are injected SER error */
#define  MAX_NUM_SER_INJECT_FIELD     10

/* Max field number of PTs */
#define  MAX_PT_FIELD_NUM             200

/******************************************************************************
 * Typedefs
 */


/******************************************************************************
 * Private variables
 */


/******************************************************************************
 * Private Functions
 */

/******************************************************************************
 *  bcmptm_ser_sram_test_fid_get
 */
static int
bcmptm_ser_sram_test_fid_get(int unit, bcmdrd_sid_t sid,
                             bcmdrd_fid_t *fid, int double_bit)
{
    char *field_name[] = {
                           "ECC",
                           "ECC_0",
                           "EVEN_PARITY",
                           "PARITY",
                           "EVEN_PARITY_0",
                           "EVEN_PARITY_LOWER",
                           "PARITY_0",
                           "ECCP",
                           "DATA",
                           "DATA_0",
                          };
    size_t  i = 0, count = sizeof(field_name) / sizeof(field_name[0]);
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    const bcmdrd_symbols_t *symbols;
    int rv = SHR_E_NONE;
    size_t num_fields = 0;
    bcmdrd_fid_t field_list[MAX_PT_FIELD_NUM];

    SHR_FUNC_ENTER(unit);

    symbols = bcmdrd_dev_symbols_get(unit, 0);

    for (i = 0; i < count; i++) {
        symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid, field_name[i], &finfo);
        if (NULL != symbol) {
            /* needs multi-bit field */
            if (double_bit && ((finfo.maxbit - finfo.minbit) < 1)) {
                continue;
            }
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  " The SER error will be injected to field [%s] of reg [%s]\n"),
                       finfo.name, bcmdrd_pt_sid_to_name(unit, sid)));
            *fid = finfo.fid;
            rv = SHR_E_NONE;
            SHR_RETURN_VAL_EXIT(rv);
        }
    }
    rv = bcmdrd_pt_fid_list_get(unit, sid, MAX_PT_FIELD_NUM, field_list, &num_fields);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* select first multi-bit field or last 1-bit field */
    for (i = 0; i < num_fields; i++) {
        rv = bcmdrd_pt_field_info_get(unit, sid, field_list[i], &finfo);
        SHR_IF_ERR_EXIT(rv);
        if (finfo.maxbit - finfo.minbit >= 1) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  " The SER error will be injected to field [%s] of reg [%s]\n"),
                       finfo.name, bcmdrd_pt_sid_to_name(unit, sid)));
            *fid = field_list[i];
            rv = SHR_E_NONE;
            SHR_RETURN_VAL_EXIT(rv);
        }
    }
    if (double_bit) {
        /* not found two bit field */
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " %s has no field which width is two or more bit\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        rv = SHR_E_NOT_FOUND;
        SHR_RETURN_VAL_EXIT(rv);
    }
    if (i == num_fields) {
        i -= 1;
    }
    *fid = field_list[i];

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to find field of %s to be injected SER error\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
*  bcmptm_ser_tcam_test_fid_get
 */
static int
bcmptm_ser_tcam_test_fid_get(int unit, bcmdrd_sid_t sid, int phy_idx, int tbl_inst,
                             uint32_t flags, bcmdrd_fid_t *fid, int *test_fld_num)
{
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    char *key_name[] = { "KEY",
                         "KEY0",
                         "KEY1",
                         "KEY0_LWR"
                       };
    char *mask_name[] = { "MASK",
                          "MASK0",
                          "MASK1",
                          "MASK0_LWR"
                        };
    char *valid_name[] = { "VALID",
                           "VALID0",
                           "VALID0_LWR"
                         };
    int i = 0, key_count = COUNTOF(key_name), rv = SHR_E_NONE;
    int valid_count = COUNTOF(valid_name);
    uint32_t mem_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t mask_fbuf[BCMDRD_MAX_PT_WSIZE];
    const bcmdrd_symbols_t *symbols = NULL;

    SHR_FUNC_ENTER(unit);

    *test_fld_num = 0;
    symbols = bcmdrd_dev_symbols_get(unit, 0);

    if ((BCMPTM_SER_INJECT_ERROR_KEY_MASK & flags) == 0) {
        for (i = 0; i < valid_count; i++) {
            symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid,
                                                       valid_name[i], &finfo);
            if (NULL != symbol) {
                fid[0] = finfo.fid;
                *test_fld_num = 1;
                rv = SHR_E_NONE;
                SHR_RETURN_VAL_EXIT(rv);
            } else {
                continue;
            }
        }
    } else  {
        /* for BCMPTM_SER_INJECT_ERROR_KEY_MASK */
        for (i = 0; i < key_count; i++) {
            symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid,
                                                       key_name[i], &finfo);
            if (NULL == symbol) {
                continue;
            }
            fid[0] = finfo.fid;
            symbol = bcmdrd_sym_field_info_get_by_name(symbols, sid,
                                                       mask_name[i], &finfo);
            if (NULL == symbol) {
                /* it is abnormal when tcams have key field, but have not mask field. */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            sal_memset(mem_data, 0, sizeof(mem_data));
            sal_memset(mask_fbuf, 0, sizeof(mask_fbuf));

            rv = bcmptm_ser_pt_read(unit, sid, phy_idx, tbl_inst, NULL,
                                    mem_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            rv = bcmdrd_pt_field_get(unit, sid, mem_data, finfo.fid, mask_fbuf);
            SHR_IF_ERR_VERBOSE_EXIT(rv);

            /* if first bit of mask field is 1, the bit needs not to flip */
            if (mask_fbuf[0] == 1) {
                *test_fld_num = 1;
            } else {
                fid[1] = finfo.fid;
                *test_fld_num = 2;
            }
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        /* not found */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
*  bcmptm_ser_error_data_generate
 */
static void
bcmptm_ser_error_data_generate(uint32_t *fbuf, int double_bit)
{
    if((fbuf[0] & 0x1) == 1) {
        fbuf[0] &= 0xFFFFFFFE;
    } else {
        fbuf[0] |= 0x1;
    }
    if (0 == double_bit) {
        return;
    }
    if((fbuf[0] & 2) == 2) {
        fbuf[0] &= 0xFFFFFFFD;
    } else {
        fbuf[0] |= 0x00000002;
    }
}

/******************************************************************************
*  bcmptm_ser_injection_hint_msg_print
 */
static int
bcmptm_ser_injection_hint_msg_print(int unit, bcmdrd_sid_t sid, int double_bit, int ecc_checking)
{
    if (double_bit) {
       LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            " Inject 2bit ser error to SRAM[%s]\n"),
                 bcmdrd_pt_sid_to_name(unit, sid)));
       if (ecc_checking == 0) {
           LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                " SRAM[%s] only supports 1bit parity protection\n"),
                     bcmdrd_pt_sid_to_name(unit, sid)));
           return SHR_E_CONFIG;
       }
   } else {
       LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            " Inject 1bit ser error to SRAM[%s]\n"),
                 bcmdrd_pt_sid_to_name(unit, sid)));
       if (ecc_checking == 1) {
           LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                " SRAM[%s] supports 2bit ECC protection\n"),
                     bcmdrd_pt_sid_to_name(unit, sid)));
       }
   }
   return SHR_E_NONE;
}

/******************************************************************************
*  bcmptm_ser_validation_hint_msg_print
 */
static int
bcmptm_ser_validation_hint_msg_print(int unit, bcmdrd_sid_t sid, int phy_idx,
                                     int double_bit, int ecc_checking, int rv)
{
    uint32_t ser_checking_enable = 0;
    /*
    * sometime SERC finds SER error of one PT reported again and again,
    * then SERC close parity checking.
    */
    ser_checking_enable =
        bcmptm_ser_control_pt_field_val_get(unit, sid,
                                            SER_CONTROL_PTt_ECC_PARITY_CHECKf);
    if (ser_checking_enable == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Parity checking of [%s] is disabled by SERC thread!\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " So this PT should be read successfully!\n")));
            return SHR_E_FAIL;
        } else {
            return SHR_E_NONE;
        }
    }

    /* (0 == 0): parity protect or (1 == 1):ecc protect */
    if (double_bit == ecc_checking) {
        if (!SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " %s[%d] should not be read successfully\n"),
                      bcmdrd_pt_sid_to_name(unit, sid), phy_idx));
            return SHR_E_FAIL;
        }
    } else {
        if (double_bit == 1) {
            /* two bit error in memory which owns parity checking */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " There is a 2bit error in %s[%d] owning parity checking.\n"),
                      bcmdrd_pt_sid_to_name(unit, sid), phy_idx));
        } else {
            /* one bit error in memory which owns ECC checking */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " There is a 1bit error in %s[%d] owning ECC checking.\n"),
                      bcmdrd_pt_sid_to_name(unit, sid), phy_idx));
        }
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " Such SER error is invalid\n")));
            return rv;
        }
    }
    return SHR_E_NONE;
}

/******************************************************************************
*  bcmptm_ser_error_inject
 */
static int
bcmptm_ser_error_inject(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                        int tbl_inst, int phy_idx, int double_bit)
{
    uint32_t mem_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t bad_data;
    int  rv = SHR_E_NONE;
    uint32_t fbuf[BCMDRD_MAX_PT_WSIZE];
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    sal_memset(mem_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
    sal_memset(fbuf, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);

    /* Cannot read data from PTcache for some PTs, such as: EGR_VLAN_XLATE_ECCm */
    rv = bcmptm_ser_pt_read(unit, sid, phy_idx, tbl_inst, NULL,
                            mem_data, BCMDRD_MAX_PT_WSIZE,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, sid, mem_data, fid, fbuf);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* bit flip */
    bcmptm_ser_error_data_generate(fbuf, double_bit);

    bad_data = fbuf[0];

    rv = bcmdrd_pt_field_set(unit, sid, mem_data, fid, fbuf);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_write(unit, sid, phy_idx, tbl_inst, NULL, mem_data, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_read(unit, sid, phy_idx, tbl_inst, NULL,
                            mem_data, BCMDRD_MAX_PT_WSIZE,
                            BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmdrd_pt_field_get(unit, sid, mem_data, fid, fbuf);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (bad_data != fbuf[0]) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Read data [0x%x] is not equal to written err-data[0x%x] "
                             "of memory[%s] index [%d]\n"),
                  fbuf[0], bad_data, bcmdrd_pt_sid_to_name(unit, sid), phy_idx));

        rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
        SHR_IF_ERR_EXIT(rv);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to inject SER error to field [%s]\n"),
                  finfo.name));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to inject SER error to memory %s\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_xor_write_enable used to enable or disable DEBUG_DISABLE_XOR_WRITEf,
 * This field will be set as "1", and then inject SER error to memory table.
 */
static int
bcmptm_ser_xor_write_disable(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid, int disable)
{
    uint32_t fld_new_val = 0, fld_bit_num = 0;
    int rv = SHR_E_NONE;
    bcmdrd_fid_t fld_array[2] = { INVALIDf, INVALIDf };
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    fld_array[0] = fid;
    fld_array[1] = INVALIDf; /* must */

    if (disable) {
        rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
        SHR_IF_ERR_EXIT(rv);
        fld_bit_num = finfo.maxbit - finfo.minbit + 1;
        fld_new_val = (1 << fld_bit_num) - 1;
    } else {
        fld_new_val = 0;
    }
    rv = bcmptm_ser_reg_multi_fields_modify(unit, sid, fld_array, fld_new_val,
                                            NULL, NULL);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_force_xor_gen_enable used to enable or disable FORCE_XOR_GENERATIONf,
 * If XOR bank has SER error, this field will be set as "1", and then read table bank to trigger SER error.
 */
static int
bcmptm_ser_force_xor_gen_enable(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid, int enable)
{
    uint32_t fld_new_val = 0, fld_bit_num = 0;
    int rv = SHR_E_NONE;
    bcmdrd_fid_t fld_array[2] = { INVALIDf, INVALIDf };
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    fld_array[0] = fid;
    fld_array[1] = INVALIDf; /* must */

    if (enable) {
        rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
        SHR_IF_ERR_EXIT(rv);
        fld_bit_num = finfo.maxbit - finfo.minbit + 1;
        fld_new_val = (1 << fld_bit_num) - 1;
    } else {
        fld_new_val = 0;
    }
    rv = bcmptm_ser_reg_multi_fields_modify(unit, sid, fld_array, fld_new_val,
                                            NULL, NULL);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_injected_sid_and_field_get Get ECC view ID and field ID if "inject_fid" is equal to INVALIDf
 */
static int
bcmptm_ser_injected_sid_and_field_get(int unit, bcmdrd_sid_t fv_sid,
                                      int tbl_inst, int fv_idx, int double_bit,
                                      bcmdrd_sid_t *remap_sid, int *remap_idx,
                                      bcmdrd_sid_t *inject_fid)
{
    int ecc_checking = 0, rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    *remap_sid = fv_sid;
    *remap_idx = fv_idx;

    /* get ecc view SID and index */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, mem_ecc_info_get_fn)(unit, fv_sid, fv_idx, remap_sid, remap_idx, &ecc_checking);
    if (rv == SHR_E_UNAVAIL) {
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* Functional mem -> ECC mem */
    BCMPTM_SER_INJECTION_STATUS_PT_ID_SET(unit, *remap_sid);
    BCMPTM_SER_INJECTION_STATUS_PT_INSTANCE_SET(unit, tbl_inst);
    /* Functional index -> ECC index */
    BCMPTM_SER_INJECTION_STATUS_PT_INDEX_SET(unit, *remap_idx);

    rv = bcmptm_ser_injection_hint_msg_print(unit, fv_sid, double_bit, ecc_checking);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (*inject_fid == INVALIDf) {
        /* get field used to inject ser error */
        rv = bcmptm_ser_sram_test_fid_get(unit, *remap_sid, inject_fid, double_bit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}


/******************************************************************************
* Public Functions
 */

/******************************************************************************
 * bcmptm_ser_inject_tcam used to inject SER error to one TCAM
 * use physical index,
 * The TCAMs are protected by CMIC SER engine.
 */
int
bcmptm_ser_tcam_ser_inject(int unit, bcmdrd_sid_t aggr_sid,
                           bcmdrd_sid_t tcam_only_sid,
                           int tbl_inst, int phy_idx, uint32_t flags)
{
    int rv = SHR_E_NONE, i = 0, rv_enable = SHR_E_EMPTY;
    int test_fld_num = 0;
    bcmdrd_fid_t fids[MAX_NUM_SER_INJECT_FIELD] = { INVALIDf };

    SHR_FUNC_ENTER(unit);

    if (bcmptm_ser_control_pt_field_val_get(unit, aggr_sid, SER_CONTROL_PTt_ECC_PARITY_CHECKf) == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Please enable parity or ecc checking for [%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, aggr_sid)));
        rv = SHR_E_DISABLED;
        SHR_IF_ERR_EXIT(rv);
    }
    /* Disable parity checking */
    rv_enable = bcmptm_ser_tcam_pt_ser_enable(unit, aggr_sid, 0);
    rv = rv_enable;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* find ser injection fid */
    rv = bcmptm_ser_tcam_test_fid_get(unit, tcam_only_sid, phy_idx, tbl_inst, flags, fids,
                                      &test_fld_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* only support 1bit ser error injection */
    /* first modify mask field, then modify key field */
    for (i = test_fld_num - 1; i >= 0 && i < MAX_NUM_SER_INJECT_FIELD; i--) {
        rv = bcmptm_ser_error_inject(unit, tcam_only_sid, fids[i], tbl_inst, phy_idx, 0);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    /* re-enable parity checking */
    if (rv_enable == SHR_E_NONE) {
        (void)bcmptm_ser_tcam_pt_ser_enable(unit, aggr_sid, 1);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to inject ser error to TCAM[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, tcam_only_sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_xor_bank_ser_inject used to inject SER error to XOR bank
 */
int
bcmptm_ser_xor_bank_ser_inject(int unit, bcmdrd_sid_t fv_sid, bcmdrd_fid_t inject_fid,
                               int tbl_inst, int fv_idx, uint32_t flags)
{
    int rv = SHR_E_NONE, rv_enable = SHR_E_EMPTY;
    bcmptm_ser_control_reg_info_t ctrl_reg_info;
    int double_bit = (flags & BCMPTM_SER_INJECT_ERROR_2BIT) ? 1 : 0;
    bcmdrd_sid_t remap_sid = fv_sid;
    int remap_idx = fv_idx;

    SHR_FUNC_ENTER(unit);

    ctrl_reg_info.xor_wr_reg = INVALIDr;

    if (bcmptm_ser_control_pt_field_val_get(unit, fv_sid, SER_CONTROL_PTt_ECC_PARITY_CHECKf) == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Please make sure [%s] supports parity or ecc checking,"
                             "and the checking is enabled\n"),
                  bcmdrd_pt_sid_to_name(unit, fv_sid)));
        rv = SHR_E_DISABLED;
        SHR_IF_ERR_EXIT(rv);
    }

    rv = bcmptm_ser_injected_sid_and_field_get(unit, fv_sid, tbl_inst, fv_idx, double_bit,
                                               &remap_sid, &remap_idx, &inject_fid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv_enable = bcmptm_ser_pt_ser_enable(unit, fv_sid, BCMDRD_SER_EN_TYPE_EC, 0);
    rv = rv_enable;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, sram_ctrl_reg_get_fn)(unit, fv_sid, fv_idx, &ctrl_reg_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (ctrl_reg_info.xor_wr_reg == INVALIDr) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " There is no XOR bank of SRAM[%s] index [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, fv_sid), fv_idx));
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* write field DEBUG_DISABLE_XOR_WRITEf to 0, need to update XOR bank */
    rv = bcmptm_ser_xor_write_disable(unit, ctrl_reg_info.xor_wr_reg,
                                      ctrl_reg_info.xor_wr_field, 0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* inject ser error to appointed sid and fid */
    rv = bcmptm_ser_error_inject(unit, remap_sid, inject_fid, tbl_inst, remap_idx, double_bit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* write field DEBUG_DISABLE_XOR_WRITEf to 1, need not to update XOR bank */
    rv = bcmptm_ser_xor_write_disable(unit, ctrl_reg_info.xor_wr_reg,
                                      ctrl_reg_info.xor_wr_field, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* revert ser error injected to appointed sid and fid, there is no SER error in table bank */
    rv = bcmptm_ser_error_inject(unit, remap_sid, inject_fid, tbl_inst, remap_idx, double_bit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    /* write field DEBUG_DISABLE_XOR_WRITEf to 0 (default value), there is SER error in XOR bank */
    if (ctrl_reg_info.xor_wr_reg != INVALIDr) {
        (void)bcmptm_ser_xor_write_disable(unit, ctrl_reg_info.xor_wr_reg,
                                           ctrl_reg_info.xor_wr_field, 0);
    }
    /* re-enable parity or ecc checking */
    if (rv_enable == SHR_E_NONE) {
        (void)bcmptm_ser_pt_ser_enable(unit, fv_sid, BCMDRD_SER_EN_TYPE_EC, 1);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to inject ser error to XOR bank of SRAM[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, fv_sid)));
    }
    SHR_FUNC_EXIT();

}

/******************************************************************************
 * bcmptm_ser_sram_ser_inject used to inject SER error to one SRAM
 * use functional index
 */
int
bcmptm_ser_sram_ser_inject(int unit, bcmdrd_sid_t fv_sid, bcmdrd_fid_t inject_fid,
                           int tbl_inst, int fv_idx, uint32_t flags)
{
    int rv = SHR_E_NONE, rv_enable = SHR_E_EMPTY;
    bcmdrd_sid_t remap_sid = fv_sid;
    int remap_idx = fv_idx;
    int double_bit = (flags & BCMPTM_SER_INJECT_ERROR_2BIT) ? 1 : 0;
    bcmptm_ser_control_reg_info_t ctrl_reg_info;

    SHR_FUNC_ENTER(unit);

    ctrl_reg_info.xor_wr_reg = INVALIDr;

    if (bcmptm_ser_control_pt_field_val_get(unit, fv_sid, SER_CONTROL_PTt_ECC_PARITY_CHECKf) == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Please make sure [%s] supports parity or ecc checking,"
                             " and the checking is enabled\n"),
                  bcmdrd_pt_sid_to_name(unit, fv_sid)));
        rv = SHR_E_DISABLED;
        SHR_IF_ERR_EXIT(rv);
    }

    rv = bcmptm_ser_injected_sid_and_field_get(unit, fv_sid, tbl_inst, fv_idx, double_bit,
                                               &remap_sid, &remap_idx, &inject_fid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv_enable = bcmptm_ser_pt_ser_enable(unit, fv_sid, BCMDRD_SER_EN_TYPE_EC, 0);
    rv = rv_enable;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    (void)BCMPTM_SER_DATA_DRIVER_CB(unit, sram_ctrl_reg_get_fn)(unit, fv_sid, fv_idx, &ctrl_reg_info);
    /* write field DEBUG_DISABLE_XOR_WRITEf to 1 */
    if (ctrl_reg_info.xor_wr_reg != INVALIDr) {
        rv = bcmptm_ser_xor_write_disable(unit, ctrl_reg_info.xor_wr_reg,
                                          ctrl_reg_info.xor_wr_field, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* inject ser error to appointed sid and fid */
    rv = bcmptm_ser_error_inject(unit, remap_sid, inject_fid, tbl_inst, remap_idx, double_bit);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    /* re-enable parity or ecc checking */
    if (rv_enable == SHR_E_NONE) {
        (void)bcmptm_ser_pt_ser_enable(unit, fv_sid, BCMDRD_SER_EN_TYPE_EC, 1);
    }
    if (ctrl_reg_info.xor_wr_reg != INVALIDr) {
        (void)bcmptm_ser_xor_write_disable(unit, ctrl_reg_info.xor_wr_reg,
                                           ctrl_reg_info.xor_wr_field, 0);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to inject ser error to SRAM[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, fv_sid)));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_err_injection_validate used to validate whether injected SER error
 * can be corrected or not
 */
int
bcmptm_ser_err_injection_validate(int unit, bcmdrd_sid_t fv_sid,
                                  int tbl_inst, int fv_idx,
                                  sal_time_t usleep, uint32_t flags)
{
    uint32_t mem_data[BCMDRD_MAX_PT_WSIZE];
    int rv = SHR_E_NONE, pt_read_rv = SHR_E_NONE;
    bcmdrd_sid_t remap_sid = fv_sid, reported_sid;
    int remap_idx = fv_idx;
    int ecc_checking = 0;
    int double_bit = (flags & BCMPTM_SER_INJECT_ERROR_2BIT) ? 1 : 0;
    int err_in_xor_bank = (BCMPTM_SER_INJECT_ERROR_XOR_BANK & flags) ? 1 : 0;
    bcmdrd_pt_ser_info_t ser_info;
    bcmptm_ser_control_reg_info_t ctrl_reg_info = { INVALIDr, INVALIDf };
    uint32_t ser_checking_enable = 0;

    SHR_FUNC_ENTER(unit);

    /* Get ecc view SID and index */
    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, mem_ecc_info_get_fn)(unit, fv_sid, fv_idx, &remap_sid, &remap_idx, &ecc_checking);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    ctrl_reg_info.force_reg = INVALIDr;

    if (err_in_xor_bank) {
        rv = BCMPTM_SER_DATA_DRIVER_CB(unit, sram_ctrl_reg_get_fn)(unit, fv_sid, fv_idx, &ctrl_reg_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        if (ctrl_reg_info.force_reg == INVALIDr) {
            rv = SHR_E_INTERNAL;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        rv = bcmptm_ser_force_xor_gen_enable(unit, ctrl_reg_info.force_reg,
                                             ctrl_reg_info.force_field, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* Trigger ser event */
    pt_read_rv = bcmptm_ser_pt_read(unit, fv_sid, fv_idx, tbl_inst, NULL,
                                    mem_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    rv = bcmptm_ser_validation_hint_msg_print(unit, fv_sid, fv_idx, double_bit, ecc_checking, pt_read_rv);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* Wait SERC thread to correct SER error */
    sal_usleep(usleep);

    reported_sid = BCMPTM_SER_STATS_PT_ID(unit);

    if (remap_sid != reported_sid) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " SER reported PT name is [%s] but [%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, reported_sid),
                  bcmdrd_pt_sid_to_name(unit, remap_sid)));
    }
    /* Test whether ser error is corrected */
    pt_read_rv = bcmptm_ser_pt_read(unit, fv_sid, fv_idx, tbl_inst, NULL,
                                    mem_data, BCMDRD_MAX_PT_WSIZE,
                                    BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
    /* For SER recovery type is NONE */
    (void)bcmdrd_pt_ser_info_get(unit, fv_sid, &ser_info);

    if (ser_info.resp == BCMDRD_SER_RESP_NONE) {
        ser_checking_enable =
            bcmptm_ser_control_pt_field_val_get(unit, fv_sid,
                                                SER_CONTROL_PTt_ECC_PARITY_CHECKf);
        /* Should be failure */
        if (SHR_FAILURE(pt_read_rv) && ser_checking_enable) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " Because SER recovery type of [%s] is [ser_no_operation].\n"),
                      bcmdrd_pt_sid_to_name(unit, fv_sid)));
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " So SER event should be reported again\n")));
            rv = SHR_E_NONE;
        } else {
            if (ser_checking_enable) {
                /* PT can not be read successfully */
                rv = SHR_E_FAIL;
            } else {
                /*
                * sometime SERC finds SER error of one PT reported again and again,
                * then SERC close parity checking.
                */
                rv = SHR_E_NONE;
            }
        }
        /* Exit */
        SHR_RETURN_VAL_EXIT(rv);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(pt_read_rv);
        if (fv_sid != remap_sid) {
            rv = bcmptm_ser_pt_read(unit, remap_sid, remap_idx, tbl_inst, NULL,
                                     mem_data, BCMDRD_MAX_PT_WSIZE, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

 exit:
    if (err_in_xor_bank && (ctrl_reg_info.force_reg != INVALIDr)) {
        (void)bcmptm_ser_force_xor_gen_enable(unit, ctrl_reg_info.force_reg,
                                              ctrl_reg_info.force_field, 0);
    }
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to validate ser error of %s[%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, fv_sid), fv_idx));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * bcmptm_ser_ing_egr_pipe_select
 */
int
bcmptm_ser_ing_egr_pipe_select(int unit, int pipe)
{
    int rv = SHR_E_NONE, i;
    uint32_t pipe_valid = ((pipe >= 0) && (pipe <= 3))? pipe : 0;
    uint32_t entry_data[1] = {0};
    bcmdrd_sid_t sid[2] = {INVALIDm, INVALIDm};
    bcmdrd_sid_t fid = INVALIDf;

    SHR_FUNC_ENTER(unit);

    rv = BCMPTM_SER_DATA_DRIVER_CB(unit, ing_egr_sbs_control_get_fn)(unit, &sid[0], &sid[1], &fid);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (i = 0; i < 2; i++) {
        /* does not read from ptcache */
        rv = bcmptm_ser_pt_read(unit, sid[i], 0, -1, NULL,
                                entry_data, 1, BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmdrd_pt_field_set(unit, sid[i], entry_data, fid, &pipe_valid);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_pt_write(unit, sid[i], 0, -1, NULL,
                                 entry_data, BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

