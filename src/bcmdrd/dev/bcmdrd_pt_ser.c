/*! \file bcmdrd_pt_ser.c
 *
 * BCMDRD PT SER routines.
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

#include <sal/sal_libc.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt_ser.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

static void
ser_en_ctrl_get(const uint32_t *en_pf, bcmdrd_pt_ser_en_ctrl_t *en_ctrl)
{
    sal_memset(en_ctrl, 0, sizeof(*en_ctrl));

    en_ctrl->sid = BCMDRD_SER_ENCTRL_REG_GET(*en_pf);
    en_ctrl->fid = BCMDRD_SER_ENCTRL_FIELD_GET(*en_pf);
    if (BCMDRD_SER_ENCTRL_EXT(*en_pf)) {
        en_ctrl->fmask = 1 << BCMDRD_SER_ENCTRL_EXT_BITPOS_GET(*en_pf);
    }
}

static void
ser_info_get(const uint32_t *info_pf, bcmdrd_pt_ser_info_t *info)
{
    sal_memset(info, 0, sizeof(*info));

    info->type = BCMDRD_SER_INFO_TYPE_GET(*info_pf);
    info->num_xor_banks = BCMDRD_SER_INFO_NUM_XOR_BANKS_GET(*info_pf);
    info->drop_pkt = BCMDRD_SER_INFO_DROP_PKT_GET(*info_pf);
    info->resp = BCMDRD_SER_INFO_RESP_GET(*info_pf);
}

int
bcmdrd_pt_ser_en_ctrl_get(int unit,
                          bcmdrd_ser_en_type_t type,
                          bcmdrd_sid_t sid,
                          bcmdrd_pt_ser_en_ctrl_t *en_ctrl)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_ser_data_t *ser_data;
    int sympf_offset;
    const uint32_t *en_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }

    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    if (en_ctrl == NULL) {
        return SHR_E_PARAM;
    }

    ser_data = bcmdrd_sym_ser_data_get(symbols);
    sympf_offset = bcmdrd_sym_ser_profile_offset_get(symbol);

    en_pf = bcmdrd_sym_ser_en_ctrl_profile_get(ser_data, sympf_offset, type);
    if (en_pf == NULL) {
        return SHR_E_FAIL;
    }

    ser_en_ctrl_get(en_pf, en_ctrl);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_info_get(int unit, bcmdrd_sid_t sid,
                       bcmdrd_pt_ser_info_t *info)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_ser_data_t *ser_data;
    int sympf_offset;
    const uint32_t *info_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }

    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    if (info == NULL) {
        return SHR_E_PARAM;
    }

    ser_data = bcmdrd_sym_ser_data_get(symbols);
    sympf_offset = bcmdrd_sym_ser_profile_offset_get(symbol);

    info_pf = bcmdrd_sym_ser_info_profile_get(ser_data, sympf_offset);
    if (info_pf == NULL) {
        return SHR_E_FAIL;
    }

    ser_info_get(info_pf, info);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_rmr_id_list_get(int unit, size_t list_max,
                              bcmdrd_ser_rmr_id_t *rid_list, size_t *num_rid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    uint32_t idx;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_FAIL;
    }
    if (num_rid == NULL) {
        return SHR_E_PARAM;
    }
    *num_rid = ser_data->rmrs_size;

    if (list_max == 0) {
        return SHR_E_NONE;
    }
    if (rid_list == NULL) {
        return SHR_E_PARAM;
    }

    for (idx = 0; idx < ser_data->rmrs_size; idx++) {
        if (idx >= list_max) {
            break;
        }
        rid_list[idx] = idx;
    }

    return SHR_E_NONE;
}

extern int
bcmdrd_pt_ser_rmr_en_ctrl_get(int unit,
                              bcmdrd_ser_en_type_t type,
                              bcmdrd_ser_rmr_id_t rid,
                              bcmdrd_pt_ser_en_ctrl_t *en_ctrl)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    const bcmdrd_ser_rmr_t *rmr;
    int sympf_offset;
    const uint32_t *en_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_FAIL;
    }
    rmr = bcmdrd_sym_ser_rmr_get(ser_data, rid);
    if (rmr == NULL) {
        return SHR_E_FAIL;
    }

    if (en_ctrl == NULL) {
        return SHR_E_PARAM;
    }

    sympf_offset = bcmdrd_ser_rmr_profile_offset_get(rmr);

    en_pf = bcmdrd_sym_ser_en_ctrl_profile_get(ser_data, sympf_offset, type);
    if (en_pf == NULL) {
        return SHR_E_FAIL;
    }

    ser_en_ctrl_get(en_pf, en_ctrl);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_rmr_info_get(int unit, bcmdrd_ser_rmr_id_t rid,
                           bcmdrd_pt_ser_info_t *info)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    const bcmdrd_ser_rmr_t *rmr;
    int sympf_offset;
    const uint32_t *info_pf;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_FAIL;
    }
    rmr = bcmdrd_sym_ser_rmr_get(ser_data, rid);
    if (rmr == NULL) {
        return SHR_E_FAIL;
    }

    if (info == NULL) {
        return SHR_E_PARAM;
    }

    sympf_offset = bcmdrd_ser_rmr_profile_offset_get(rmr);

    info_pf = bcmdrd_sym_ser_info_profile_get(ser_data, sympf_offset);
    if (info_pf == NULL) {
        return SHR_E_FAIL;
    }

    ser_info_get(info_pf, info);

    return SHR_E_NONE;
}

int
bcmdrd_pt_ser_name_to_rmr(int unit, const char *name,
                          bcmdrd_ser_rmr_id_t *rid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;
    const bcmdrd_ser_rmr_t *rmr;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_UNIT;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return SHR_E_FAIL;
    }

    rmr = bcmdrd_ser_rmr_find(name, ser_data, rid);
    if (rmr == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

const char *
bcmdrd_pt_ser_rmr_to_name(int unit, bcmdrd_ser_rmr_id_t rid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_ser_data_t *ser_data;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return NULL;
    }
    ser_data = bcmdrd_sym_ser_data_get(symbols);
    if (ser_data == NULL) {
        return NULL;
    }
    if (rid >= ser_data->rmrs_size) {
        return NULL;
    }
    return ser_data->rmrs[rid].name;
}

#endif
