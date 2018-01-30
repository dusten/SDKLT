/*! \file bcmptm_bcm56960_a0_cci.c
 *
 * This file contains the misc utils for bcm56960_a0's CCI
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

#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include "./bcmptm_bcm56960_a0_cci.h"

#define COUNTER_POOL_ENABLE_MASK    (1 << 1)
#define CLR_ON_READ_MASK            (1 << 2)

typedef struct ctr_evict_s {
    bcmdrd_sid_t sid;
    bcmdrd_sid_t ctrl_sid;
    bool clr_on_read;
} ctr_evict_t;

static const ctr_evict_t ctr_eviction[] = {
    {ING_FLEX_CTR_COUNTER_TABLE_0m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_1m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_2m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_3m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_4m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_4r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_5m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_5r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_6m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_6r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_7m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_7r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_8m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_8r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_9m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_9r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_10m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_10r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_11m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_11r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_12m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_12r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_13m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_13r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_14m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_14r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_15m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_15r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_16m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_11r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_17m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_17r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_18m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_18r, 1},
    {ING_FLEX_CTR_COUNTER_TABLE_19m, ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_19r, 1},
    {EGR_FLEX_CTR_COUNTER_TABLE_0m, EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r, 1},
    {EGR_FLEX_CTR_COUNTER_TABLE_1m, EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r, 1},
    {EGR_FLEX_CTR_COUNTER_TABLE_2m, EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r, 1},
    {EGR_FLEX_CTR_COUNTER_TABLE_3m, EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r, 1},
    {EGR_PERQ_XMT_COUNTERSm, 0, 0},
    {EFP_COUNTER_TABLEm, 0, 0}
};

static bool
is_ctr_evict(int unit,
             bcmdrd_sid_t sid)
{
    int i;
    int size = sizeof(ctr_eviction) / sizeof(ctr_evict_t);

    for (i = 0; i < size; i++) {
        if (sid == ctr_eviction[i].sid) {
            return true;
        }
    }

    return false;
}

int
bcm56960_a0_pt_cci_ctrtype_get(int unit,
                               bcmdrd_sid_t sid)
{
    int rv = SHR_E_NONE;
    int blktype;
    bool port;

    rv = bcmdrd_pt_blktype_get(unit, sid, 0);
    if(SHR_FAILURE(rv)) {
        return rv;
    }
    blktype = rv;
    port = bcmdrd_pt_is_port_reg(unit, sid);
    if (port) {
        if ((blktype == BLKTYPE_CLPORT) ||
            (blktype == BLKTYPE_XLPORT) ||
             (blktype == BLKTYPE_EPIPE) ||
             (blktype == BLKTYPE_IPIPE)) {
            return CCI_CTR_TYPE_PORT;
        } else {
            return -1;
        }
    } else if (blktype == BLKTYPE_EPIPE) {
        if (is_ctr_evict(unit, sid)) {
            return CCI_CTR_TYPE_EVICT;
        } else {
            return CCI_CTR_TYPE_EPIPE;
        }
    } else if (blktype == BLKTYPE_IPIPE) {
        if (is_ctr_evict(unit, sid)) {
            return CCI_CTR_TYPE_EVICT;
        } else {
            return CCI_CTR_TYPE_IPIPE;
        }
    } else if ((blktype == BLKTYPE_MMU_GLB) ||
            (blktype == BLKTYPE_MMU_SC) ||
             (blktype == BLKTYPE_MMU_XPE)) {
        return CCI_CTR_TYPE_TM;
    }

    return SHR_E_PARAM;
}

int
bcm56960_a0_pt_cci_ctr_sym_map(int unit,
                               bcmptm_cci_ctr_info_t *ctr_info)
{
    /* No Transform */
    ctr_info->txfm_fld = -1;
    ctr_info->map_id = ctr_info->lt_map_id;

    return SHR_E_NONE;
}

int
bcm56960_a0_pt_cci_frmt_sym_get(int unit, bcmdrd_sid_t *sid)
{
    int rv = SHR_E_NONE;
    bcmdrd_sid_t frmt_sid;

    frmt_sid = *sid;
    *sid = frmt_sid;
    return rv;
}

int
bcm56960_a0_pt_cci_frmt_index_from_port(int unit, bcmdrd_sid_t sid, int port,
                                        int *index_min, int *index_max)
{
    return SHR_E_UNAVAIL;
}

bool
bcm56960_a0_pt_cci_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index)
{
    bool rv;

    rv = bcmdrd_pt_index_valid(unit, sid, tbl_inst, index);
    return rv;
}

int
bcm56960_a0_pt_cci_ctr_evict_enable(int unit, bcmdrd_sid_t sid, bool enable)
{
    int i;
    int rv = SHR_E_FAIL;
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t val;

    int size = COUNTOF(ctr_eviction);

    for (i = 0; i < size; i++) {
        if ((sid == ctr_eviction[i].sid) && (ctr_eviction[i].ctrl_sid != 0)) {
            dyn_info.index = 0;
            dyn_info.tbl_inst = 0;
            rv = bcmbd_pt_read(unit, ctr_eviction[i].ctrl_sid,
                               &dyn_info, NULL, &val, sizeof(val));
            if (SHR_SUCCESS(rv)) {
                if (enable) {
                    val |= COUNTER_POOL_ENABLE_MASK;
                    val |= CLR_ON_READ_MASK;
                } else {
                    val &= ~COUNTER_POOL_ENABLE_MASK;
                }
                rv = bcmbd_pt_write(unit, ctr_eviction[i].ctrl_sid,
                                    &dyn_info, NULL, &val);
            }
            break;
        }
    }
    return rv;
}

bool
bcm56960_a0_pt_cci_ctr_evict_is_enable(int unit, bcmdrd_sid_t sid)
{
    int i;
    bool ret = true;
    int rv;
    bcmbd_pt_dyn_info_t dyn_info;
    int size = COUNTOF(ctr_eviction);
    uint32_t val;

    for (i = 0; i < size; i++) {
        if ((sid == ctr_eviction[i].sid) && (ctr_eviction[i].ctrl_sid != 0)) {
            dyn_info.index = 0;
            dyn_info.tbl_inst = 0;
            rv = bcmbd_pt_read(unit, ctr_eviction[i].ctrl_sid,
                               &dyn_info, NULL, &val, sizeof(val));
            if (SHR_SUCCESS(rv)) {
                if (!(val & COUNTER_POOL_ENABLE_MASK)) {
                    ret = true;
                }
            }
            break;
        }
    }
    return ret;
}

bool
bcm56960_a0_pt_cci_ctr_evict_is_clr_on_read(int unit, bcmdrd_sid_t sid)
{
    int i;
    bool ret = false;
    int size = COUNTOF(ctr_eviction);

    for (i = 0; i < size; i++) {
        if (sid == ctr_eviction[i].sid) {
            ret = ctr_eviction[i].clr_on_read;
            break;
        }
    }
    return ret;
}

bool
bcm56960_a0_pt_cci_ctr_is_bypass(int unit, bcmdrd_sid_t sid)
{
    return false;
}

