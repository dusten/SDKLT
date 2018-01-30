/*! \file cci_evict.c
 *
 * CCI Counter Eviction drivers
 * This file contains the common driver to support BCMCTR CONTROL LTs
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_cci_evict_internal.h>

/*******************************************************************************
 * Local definitions
 */
/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CCI

/*******************************************************************************
 * Private variables
 */
static ctr_evict_driver_t *evict_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};

#define EVICT_DRV(unit)             (evict_driver[unit])
#define EVICT_DRV_EXEC(unit, fn)    (EVICT_DRV(unit)->fn)

/*******************************************************************************
 * Public Utility Functions
 */
int
bcmptm_cci_evict_lt_data_modify(int unit,
                                bcmltd_sid_t ltid,
                                uint32_t trans_id,
                                bcmdrd_sid_t sid,
                                int tbl_inst,
                                bcmdrd_fid_t fid,
                                uint32_t *value)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint32_t            entry_sz;
    uint32_t            *entry_buf = NULL;

    SHR_FUNC_ENTER(unit);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "ctr evict control");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = tbl_inst;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit,
                                   0,
                                   sid,
                                   &pt_dyn_info,
                                   NULL,
                                   NULL, /* misc_info */
                                   BCMPTM_OP_READ,
                                   NULL,
                                   entry_sz/4,
                                   ltid,
                                   trans_id,
                                   NULL,
                                   NULL,
                                   entry_buf,
                                   &rsp_ltid,
                                   &rsp_flags));

    bcmdrd_pt_field_set(unit, sid, entry_buf, fid, value);

    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit,
                                   0,
                                   sid,
                                   &pt_dyn_info,
                                   NULL, /* misc_info */
                                   NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_buf,
                                   0,
                                   ltid,
                                   trans_id,
                                   NULL,
                                   NULL,
                                   NULL,
                                   &rsp_ltid,
                                   &rsp_flags));
exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_evict_pt_iwrite(int unit,
                           bcmdrd_sid_t sid,
                           int tbl_inst,
                           uint32_t *data)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t        lt_id = -1;
    bcmltd_sid_t        rsp_ltid;
    uint32_t            rsp_flags;

    SHR_FUNC_ENTER(unit);
    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = tbl_inst;

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_write(unit, BCMPTM_REQ_FLAGS_PASSTHRU, sid, &pt_dyn_info,
                          NULL, data, lt_id, &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_evict_pt_iread(int unit,
                          bcmdrd_sid_t sid,
                          int tbl_inst,
                          uint32_t *data)
{
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t        lt_id = -1;
    bcmltd_sid_t        rsp_ltid;
    uint32_t            rsp_flags;
    uint32_t            wsize;

    SHR_FUNC_ENTER(unit);
    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = tbl_inst;

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_read(unit, BCMPTM_REQ_FLAGS_PASSTHRU, sid, &pt_dyn_info,
                         NULL, wsize, lt_id, data, &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Eviction Functions
 */
int
bcmptm_cci_evict_entry_update(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t trans_id,
                              ctr_control_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);
    if (EVICT_DRV_EXEC(unit, evict_entry_update)) {
        SHR_IF_ERR_EXIT(
            EVICT_DRV_EXEC(unit, evict_entry_update)(unit, ltid, trans_id, entry));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_evict_control_init(int unit)
{
    /* Chain the mem_id for eviction eligibale tables */
    SHR_FUNC_ENTER(unit);
    if (EVICT_DRV_EXEC(unit, evict_init)) {
        SHR_IF_ERR_EXIT(
            EVICT_DRV_EXEC(unit, evict_init)(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_evict_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_ALLOC(EVICT_DRV(unit), sizeof(ctr_evict_driver_t), "evict drv");
    SHR_NULL_CHECK(EVICT_DRV(unit), SHR_E_MEMORY);
    sal_memset(EVICT_DRV(unit), 0, sizeof(ctr_evict_driver_t));

    SHR_IF_ERR_EXIT(bcmptm_pt_cci_evict_driver_register(unit, EVICT_DRV(unit)));
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_cci_evict_drv_detach(int unit)
{
    if (EVICT_DRV(unit)) {
        SHR_FREE(EVICT_DRV(unit));
    }
    EVICT_DRV(unit) = NULL;
}
