/*! \file bcmptm_bcm56960_a0_cci_evict.c
 *
 * This file contains the bcm56960_a0's CCI Eviction driver
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_cci_evict_internal.h>
#include "./bcmptm_bcm56960_a0_cci.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CCI


static const bcmdrd_sid_t evict_control_reg[] = {
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_0r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_1r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_2r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_3r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_4r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_5r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_6r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_7r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_8r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_9r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_10r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_11r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_12r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_13r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_14r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_15r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_16r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_17r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_18r,
    ING_FLEX_CTR_EVICTION_CONTROL_POOL_19r,
    EGR_FLEX_CTR_EVICTION_CONTROL_POOL_0r,
    EGR_FLEX_CTR_EVICTION_CONTROL_POOL_1r,
    EGR_FLEX_CTR_EVICTION_CONTROL_POOL_2r,
    EGR_FLEX_CTR_EVICTION_CONTROL_POOL_3r,
    EGR_PERQ_EVICTION_CONTROLr,
    EGR_EFP_EVICTION_CONTROLr
};

static const bcmdrd_sid_t evict_thd_reg[] = {
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_0r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_1r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_2r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_3r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_4r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_5r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_6r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_7r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_8r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_9r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_10r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_11r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_12r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_13r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_14r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_15r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_16r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_17r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_18r,
    ING_FLEX_CTR_EVICTION_THRESHOLD_POOL_19r,
    EGR_FLEX_CTR_EVICTION_THRESHOLD_POOL_0r,
    EGR_FLEX_CTR_EVICTION_THRESHOLD_POOL_1r,
    EGR_FLEX_CTR_EVICTION_THRESHOLD_POOL_2r,
    EGR_FLEX_CTR_EVICTION_THRESHOLD_POOL_3r,
    EGR_PERQ_EVICTION_THRESHOLDr,
    EGR_EFP_EVICTION_THRESHOLDr
};

static const bcmdrd_sid_t evict_seed_reg[] = {
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_0r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_1r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_2r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_3r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_4r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_5r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_6r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_7r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_8r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_9r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_10r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_11r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_12r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_13r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_14r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_15r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_16r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_17r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_18r,
    ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_19r,
    EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_0r,
    EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_1r,
    EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_2r,
    EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_3r,
    EGR_PERQ_EVICTION_SEEDr,
    EGR_EFP_EVICTION_SEEDr
};

static const bcmdrd_sid_t evict_update_reg[] = {
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_4r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_5r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_6r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_7r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_8r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_9r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_10r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_11r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_12r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_13r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_14r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_15r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_16r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_17r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_18r,
    ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_19r,
    EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r,
    EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r,
    EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r,
    EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r,
    EGR_PERQ_CNTR_UPDATE_CONTROLr,
    EGR_EFP_CNTR_UPDATE_CONTROLr
};

static ctr_evict_mode_t bcm56960_a0_evict_mode[COUNTOF(evict_control_reg)] = {0};

static int
evict_mem_idx_get(int unit, ctr_pool_t pool, uint16_t pool_id, int *mem_idx)
{
    int offset = 0;

    SHR_FUNC_ENTER(unit);
    switch(pool){
        case CTR_POOL_ING_FLEX:
            offset = 0;
            break;
        case CTR_POOL_EGR_FLEX:
            offset = 20;
            break;
        case CTR_POOL_EGR_PERQ:
            offset = 24;
            break;
        case CTR_POOL_EGR_FP:
            offset = 25;
            break;
        default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    *mem_idx = offset + pool_id;

exit:
    SHR_FUNC_EXIT();
}

static int
evict_data_modify(int unit,
                  bcmltd_sid_t ltid,
                  uint32_t trans_id,
                  bcmdrd_sid_t sid,
                  bcmdrd_fid_t fid,
                  uint32_t *value)
{
    int pipe, tbl_inst_num;

    SHR_FUNC_ENTER(unit);
    tbl_inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    for (pipe = 0; pipe < tbl_inst_num; pipe++) {
        SHR_IF_ERR_EXIT(
            bcmptm_cci_evict_lt_data_modify(
                unit, ltid, trans_id, sid, pipe, fid, value));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_evict_entry_update(int unit,
                               bcmltd_sid_t ltid,
                               uint32_t trans_id,
                               ctr_control_entry_t *entry)
{
    int             mem_idx = 0;
    bcmdrd_sid_t    sid;
    bcmdrd_fid_t    fid;
    uint32_t        value[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        evict_mem_idx_get(unit, entry->pool, entry->pool_id, &mem_idx));

    if (entry->update_flags & CTR_UPDATE_F_E_MODE) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_control_reg[mem_idx];
        value[0] = entry->evict_mode;
        fid = MODEf;
        SHR_IF_ERR_EXIT(
            evict_data_modify(unit, ltid, trans_id, sid, fid, value));
        bcm56960_a0_evict_mode[mem_idx] = entry->evict_mode;

        sal_memset(value, 0, sizeof(value));
        sid = evict_update_reg[mem_idx];
        fid = CLR_ON_READf;
        if (entry->evict_mode) {
            value[0] = 1;
        } else {
            value[0] = 0;
        }
        SHR_IF_ERR_EXIT(
            evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_THD_BYTES) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_thd_reg[mem_idx];
        value[0] = entry->evict_thd_bytes & 0xFFFFFFFF;
        value[1] = (entry->evict_thd_bytes >> 32) & 0xFFFFFFFF;
        fid = THRESHOLD_BYTESf;
        SHR_IF_ERR_EXIT(
            evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_THD_PKTS) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_thd_reg[mem_idx];
        value[0] = entry->evict_thd_pkts;
        fid = THRESHOLD_PKTSf;
        SHR_IF_ERR_EXIT(
            evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_SEED) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_seed_reg[mem_idx];
        value[0] = entry->evict_seed & 0xFFFFFFFF;
        value[1] = (entry->evict_seed >> 32) & 0xFFFFFFFF;
        fid = THRESHOLD_PKTSf;
        SHR_IF_ERR_EXIT(
            evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_evict_init(int unit)
{
    bcmdrd_sid_t        sid;
    uint32_t            *entry_buf = NULL;
    uint32_t            entry_sz;
    uint32_t            mem_id, pipe;
    int                 i, tbl_inst;
    int                 tbl_inst_num;

    SHR_FUNC_ENTER(unit);
    entry_sz = bcmdrd_pt_entry_wsize(unit, evict_control_reg[0]);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "ctr evict control");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    for (i = 0; i < COUNTOF(evict_control_reg); i++) {
        sid = evict_control_reg[i];
        mem_id = i + 1;
        tbl_inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
        pipe = 0;
        for (tbl_inst = 0; tbl_inst < tbl_inst_num; tbl_inst++) {
            SHR_IF_ERR_EXIT(
                bcmptm_cci_evict_pt_iread(unit, sid, tbl_inst, entry_buf));
            bcmdrd_pt_field_set(unit, sid, entry_buf, MEMORY_IDf, &mem_id);
            bcmdrd_pt_field_set(unit, sid, entry_buf, PIPE_IDf, &pipe);
            SHR_IF_ERR_EXIT(
                bcmptm_cci_evict_pt_iwrite(unit, sid, tbl_inst, entry_buf));
            pipe++;
        }
        bcm56960_a0_evict_mode[i] = 0;
    }

    sid = CENTRAL_CTR_EVICTION_CONTROLr;
    SHR_IF_ERR_EXIT(
        bcmptm_cci_evict_pt_iread(unit, sid, 0, entry_buf));
    bcmdrd_pt_field_set(unit, sid, entry_buf, NUM_CE_PER_PIPEf, &mem_id);
    SHR_IF_ERR_EXIT(
        bcmptm_cci_evict_pt_iwrite(unit, sid, 0, entry_buf));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_evict_fifo_enable(int unit, bool enable)
{
    bcmdrd_sid_t        sid;
    uint32_t            *entry_buf = NULL;
    uint32_t            entry_sz, value;
    bcmdrd_fid_t        fid;

    SHR_FUNC_ENTER(unit);
    sid = CENTRAL_CTR_EVICTION_CONTROLr;
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "ctr evict fifo");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    fid = FIFO_ENABLEf;
    SHR_IF_ERR_EXIT(
        bcmptm_cci_evict_pt_iread(unit, sid, 0, entry_buf));
    value = (enable == TRUE) ? 1 : 0;
    bcmdrd_pt_field_set(unit, sid, entry_buf, fid, &value);
    SHR_IF_ERR_EXIT(
        bcmptm_cci_evict_pt_iwrite(unit, sid, 0, entry_buf));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_evict_mode_get(int unit,
                           bcmdrd_sid_t sid,
                           ctr_evict_mode_t *mode)
{
    int i;

    SHR_FUNC_ENTER(unit);
    for (i = 0; i < COUNTOF(evict_control_reg); i++) {
        if (evict_control_reg[i] == sid) {
            break;
        }
    }
    if (i == COUNTOF(evict_control_reg)) {
        SHR_EXIT();
    }
    *mode = bcm56960_a0_evict_mode[i];

exit:
    SHR_FUNC_EXIT();
}

int
bcm56960_a0_pt_cci_evict_driver_register(int unit, ctr_evict_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);
    if (drv) {
        drv->evict_init = bcm56960_a0_evict_init;
        drv->evict_fifo_enable = bcm56960_a0_evict_fifo_enable;
        drv->evict_entry_update = bcm56960_a0_evict_entry_update;
        drv->evict_mode_get = bcm56960_a0_evict_mode_get;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
exit:
    SHR_FUNC_EXIT();
}
