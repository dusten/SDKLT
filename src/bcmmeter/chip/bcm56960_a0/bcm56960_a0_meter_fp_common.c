/*! \file bcm56960_a0_meter_fp_common.c
 *
 * This file defines bcm56960_a0 chip specific functions.
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
#include <bcmdrd_config.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmmeter/bcmmeter_fp_egr.h>
#include <bcmmeter/bcmmeter_fp_ing.h>
#include "bcm56960_a0_meter_fp_egr_drv.h"
#include "bcm56960_a0_meter_fp_ing_drv.h"

#include "bcm56960_a0_meter_fp_common.h"

#define BSL_LOG_MODULE          BSL_LS_BCMMETER_FP

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Calculate meter granularity params.
 *
 * Calculate meter granularity, refresh count and
 * bucket size from meter rate and burst size.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_rate Metering rate.
 * \param [in] burst_size Meter burst size.
 * \param [in] byte_mode Meter mode
 * \param [out] gran Meter granularity.
 * \param [out] refresh_count Refresh count.
 * \param [out] bkt_sz Bucket size.
 * \param [out] bkt_cnt Bucket count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Unable to find a granularity level.
 */
static int
bcm56960_a0_meter_fp_compute_gran_params (int unit,
                                          int stage,
                                          uint32_t meter_rate,
                                          uint32_t burst_size,
                                          int byte_mode,
                                          int *gran,
                                          uint32_t *refresh_count,
                                          uint32_t *bkt_sz,
                                          uint32_t *bkt_cnt)
{
    uint32_t    min = 1, max = 0;
    uint32_t    min_burst = 1, max_burst = 0;
    uint32_t    bucket_size = 0, bucket_count = 0;
    uint32_t    i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < BCMMETER_FP_MAX_GRAN(unit, stage); i++) {
        /*
         * Get the max rate supported at each granularity level
         */
        min = BCMMETER_FP_GRAN_INFO(unit, stage, byte_mode, i).min_rate;
        max = BCMMETER_FP_GRAN_INFO(unit, stage, byte_mode, i).max_rate;
        min_burst = BCMMETER_FP_GRAN_INFO(unit, stage, byte_mode, i).min_burst;
        max_burst = (min_burst * BCMMETER_FP_MAX_BUCKETSIZE(unit, stage));

        /*
         * In TH EFP, min and max meter rates at each granularity level is
         * half of IFP.
         */
        if (BCMMETER_FP_REFRESHCOUNT_ADJUST(unit, stage)) {
            max /= 2;
        }

        if ((max >= meter_rate) && (max_burst >= burst_size)) {
            *gran = i;
            break;
        }
    }
    if (i == BCMMETER_FP_MAX_GRAN(unit, stage)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *refresh_count = ((meter_rate + (min - 1)) / min);
    /*
     * Multiply by 2 to adjust the refresh count
     */
    if (BCMMETER_FP_REFRESHCOUNT_ADJUST(unit, stage)) {
        *refresh_count *= 2;
    }

    if (*refresh_count > BCMMETER_FP_MAX_REFRESHCOUNT(unit, stage)) {
        *refresh_count = BCMMETER_FP_MAX_REFRESHCOUNT(unit, stage);
    }

    if (burst_size) {
        bucket_size = ((burst_size + (min_burst - 1)) / min_burst);
        if (bucket_size > BCMMETER_FP_MAX_BUCKETSIZE(unit, stage)) {
            bucket_size = BCMMETER_FP_MAX_BUCKETSIZE(unit, stage);
        }
        if (bucket_size == 0) {
            bucket_size = 1;
        }

        *bkt_sz = bucket_size;

        bucket_count = (bucket_size * BCMMETER_FP_BUCKETSIZE_GRAN(unit, stage));
        *bkt_cnt = bucket_count;
    } else {
        *bkt_sz = *bkt_cnt = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable slice status.
 *
 * Enable/disable the slice for access.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] slice FP slice number.
 * \param [in] status Enable/Disable flag.
 * \param [out] orig_status Slice status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56960_a0_meter_fp_slice_status_set (int unit,
                                       uint32_t trans_id,
                                       int stage,
                                       int slice,
                                       uint32_t status,
                                       uint32_t *orig_status)
{
    uint32_t                entry_sz = 0, flags = 0;
    uint32_t                rsp_flags;
    uint8_t                 *read_buf = NULL;
    uint8_t                 fid_count = 0;
    bcmltd_sid_t            req_ltid, rsp_ltid;
    bcmdrd_sid_t            sid;
    bcmdrd_fid_t            fid;
    bcmbd_pt_dyn_info_t     pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    if (BCMMETER_FP_SLICE_CONTROL(unit, stage) == NULL) {
        SHR_EXIT();
    }

    sid = BCMMETER_FP_SLICE_CONTROL(unit, stage)->sid;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(read_buf, entry_sz, "Slice entry");
    SHR_NULL_CHECK(read_buf, SHR_E_MEMORY);
    sal_memset(read_buf, 0, entry_sz);

    req_ltid = (stage == BCMMETER_FP_STAGE_ID_INGRESS) ? METER_FP_ING_TEMPLATEt :
        METER_FP_EGR_TEMPLATEt;

    fid_count = BCMMETER_FP_SLICE_CONTROL(unit, stage)->fid_count;
    /*
     * Select the field name corresponding to the slice.
     */
    if (slice < fid_count) {
        fid = BCMMETER_FP_SLICE_CONTROL(unit, stage)->fids[slice];
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /*
     * Global register. Index is 0 and instance is -1.
     */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;

    /*
     * Read slice control register to check if slice is enabled.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    0,
                                    entry_sz,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)read_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    bcmdrd_pt_field_get(unit, sid, (uint32_t *)read_buf,
                        fid, orig_status);

    /*
     * If slice is already in desired state, do nothing.
     */
    if (status == *orig_status) {
        SHR_EXIT();
    }

    bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, fid,
                        &status);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)read_buf,
                                    0,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FREE(read_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore slice status.
 *
 * Restore the slice state.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] slice FP slice number.
 * \param [in] status Enable/Disable flag.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56960_a0_meter_fp_slice_status_reset (int unit,
                                         uint32_t trans_id,
                                         int stage,
                                         int slice,
                                         uint32_t status)
{
    uint32_t                entry_sz = 0, flags = 0;
    uint32_t                rsp_flags;
    uint8_t                 *read_buf = NULL;
    uint8_t                 fid_count = 0;
    bcmltd_sid_t            req_ltid, rsp_ltid;
    bcmdrd_sid_t            sid;
    bcmdrd_fid_t            fid;
    bcmbd_pt_dyn_info_t     pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    if (BCMMETER_FP_SLICE_CONTROL(unit, stage) == NULL) {
        SHR_EXIT();
    }

    sid = BCMMETER_FP_SLICE_CONTROL(unit, stage)->sid;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(read_buf, entry_sz, "Slice entry");
    SHR_NULL_CHECK(read_buf, SHR_E_MEMORY);
    sal_memset(read_buf, 0, entry_sz);

    req_ltid = (stage == BCMMETER_FP_STAGE_ID_INGRESS) ? METER_FP_ING_TEMPLATEt :
        METER_FP_EGR_TEMPLATEt;

    fid_count = BCMMETER_FP_SLICE_CONTROL(unit, stage)->fid_count;
    /*
     * Select the field name corresponding to the slice.
     */
    if (slice < fid_count) {
        fid = BCMMETER_FP_SLICE_CONTROL(unit, stage)->fids[slice];
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /*
     * Global register. Index is 0 and instance is -1.
     */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;

    /*
     * Read slice control register to check if slice is enabled.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    0,
                                    entry_sz,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)read_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, fid,
                        &status);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)read_buf,
                                    0,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FREE(read_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable meter refresh for BCM56960_A0.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_meter_fp_refresh_status_set (int unit,
                                         int stage,
                                         uint32_t refresh_en)
{
    uint32_t                entry_sz = 0;
    uint32_t                rsp_flags;
    uint8_t                 *read_buf = NULL;
    bcmltd_sid_t            req_ltid, rsp_ltid;
    bcmdrd_sid_t            sid;
    bcmdrd_fid_t            fid;
    bcmbd_pt_dyn_info_t     pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    sid = BCMMETER_FP_REFRESH_CONFIG_SID(unit, stage);
    fid = BCMMETER_FP_REFRESH_EN_FID(unit, stage);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(read_buf, entry_sz, "Config entry");
    SHR_NULL_CHECK(read_buf, SHR_E_MEMORY);
    sal_memset(read_buf, 0, entry_sz);

    req_ltid = (stage == BCMMETER_FP_STAGE_ID_INGRESS) ? METER_FP_ING_CONFIGt :
        METER_FP_EGR_CONFIGt;

    /*
     * Global register. Index is 0 and instance is -1.
     */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);

    /* Read the refresh config table. */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          0,
                          sid,
                          &pt_dyn_info,
                          NULL,
                          entry_sz,
                          req_ltid,
                          (uint32_t *)read_buf,
                          &rsp_ltid,
                          &rsp_flags));

    bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, fid,
                        &refresh_en);

    /* Write back refresh config table. */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           0,
                           sid,
                           &pt_dyn_info,
                           NULL,
                           (uint32_t *)read_buf,
                           req_ltid,
                           &rsp_ltid,
                           &rsp_flags));
exit:
    SHR_FREE(read_buf);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Configure FP meters for BCM56960_A0.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56960_a0_meter_fp_hw_config (int unit,
                                int stage,
                                uint32_t refresh_en)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_fp_refresh_status_set(unit,
                                                 stage,
                                                 refresh_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear FP meter entry
 *
 * Clear FP meter table entry in hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_entry Pointer to the meter entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
int
bcm56960_a0_meter_fp_entry_clear (int unit,
                                  uint32_t trans_id,
                                  int stage,
                                  bcmmeter_fp_entry_t *meter_entry)
{
    int         pipe_id = 0, i = 0, pool = 0;
    bool        meter_pairing = false;
    uint32_t    flags = 0, rsp_flags = 0;
    uint32_t    entry_sz = 0, meter_index = 0;
    uint32_t    orig_status = 0;
    uint8_t     *meter_buf = NULL;
    uint8_t     oper_mode = 0;
    bcmbd_pt_dyn_info_t     pt_dyn_info;
    bcmltd_sid_t            rsp_ltid, req_ltid;
    bcmdrd_sid_t            sid;

    SHR_FUNC_ENTER(unit);

    pipe_id = meter_entry->fields.pipe;
    meter_index = meter_entry->hw_fields.meter_hw_index;
    pool = meter_entry->hw_fields.pool;

    if (BCMMETER_FP_POOL_GLOBAL(unit, stage) == false) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_fp_slice_status_set(unit,
                                                   trans_id,
                                                   stage,
                                                   pool,
                                                   TRUE,
                                                   &orig_status));
    }

    req_ltid = (stage == BCMMETER_FP_STAGE_ID_INGRESS) ? METER_FP_ING_TEMPLATEt :
        METER_FP_EGR_TEMPLATEt;
    sid = BCMMETER_FP_METER_SID(unit, stage);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(meter_buf, entry_sz, "ifp meter entry");
    SHR_NULL_CHECK(meter_buf, SHR_E_MEMORY);

    BCMMETER_FP_PAIRING_CHECK_ASSIGN(meter_entry->fields.meter_mode,
                                     meter_pairing);

    SHR_IF_ERR_EXIT(bcmmeter_fp_oper_mode_get(unit, stage, &oper_mode));

    for (i = 0; i < BCMMETER_FP_PAIR_NUM; i++) {
        /*
         * For non-paired meter modes, like DEFAULT and FLOW,
         * hw fields are stored at odd index, "1". Skip index "0".
         */
        if ((meter_pairing == false) && (i == BCMMETER_FP_INDEX_EVEN)) {
            continue;
        }

        sal_memset(meter_buf, 0, entry_sz);

        pt_dyn_info.index = meter_index;
        pt_dyn_info.tbl_inst =
            (oper_mode == BCMMETER_FP_OPER_MODE_GLOBAL) ? -1 : pipe_id;

        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        flags,
                                        sid,
                                        &pt_dyn_info,
                                        NULL,
                                        NULL, /* misc_info */
                                        BCMPTM_OP_WRITE,
                                        (uint32_t *)meter_buf,
                                        0,
                                        req_ltid,
                                        trans_id,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &rsp_ltid,
                                        &rsp_flags));

        meter_index++;
    }

    if (BCMMETER_FP_POOL_GLOBAL(unit, stage) == false) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_fp_slice_status_reset(unit,
                                                     trans_id,
                                                     stage,
                                                     pool,
                                                     orig_status));
    }

exit:
    SHR_FREE(meter_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write FP meter entry
 *
 * Write FP meter table entry in hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_entry Pointer to the meter entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
int
bcm56960_a0_meter_fp_entry_write (int unit,
                                  uint32_t trans_id,
                                  int stage,
                                  bcmmeter_fp_entry_t *meter_entry)
{
    int         pipe_id = 0, i = 0, pool = 0;
    bool        meter_pairing = false;
    uint32_t    flags = 0, entry_sz = 0, meter_index = 0;
    uint32_t    rsp_flags = 0, refresh_mode = 0, refresh_count = 0;
    uint32_t    bucket_size = 0, gran = 0, byte_mode = 0, bucket_count = 0;
    uint32_t    orig_status = 0;
    uint8_t     *meter_buf = NULL;
    uint8_t     oper_mode = 0;
    bcmbd_pt_dyn_info_t     pt_dyn_info;
    bcmltd_sid_t            rsp_ltid, req_ltid;
    bcmdrd_sid_t            sid;
    bcmdrd_fid_t            fid;

    SHR_FUNC_ENTER(unit);

    pipe_id = meter_entry->fields.pipe;
    meter_index = meter_entry->hw_fields.meter_hw_index;
    pool = meter_entry->hw_fields.pool;

    if (BCMMETER_FP_POOL_GLOBAL(unit, stage) == false) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_fp_slice_status_set(unit,
                                                   trans_id,
                                                   stage,
                                                   pool,
                                                   TRUE,
                                                   &orig_status));
    }


    req_ltid = (stage == BCMMETER_FP_STAGE_ID_INGRESS) ? METER_FP_ING_TEMPLATEt :
        METER_FP_EGR_TEMPLATEt;
    sid = BCMMETER_FP_METER_SID(unit, stage);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(meter_buf, entry_sz, "ifp meter entry");
    SHR_NULL_CHECK(meter_buf, SHR_E_MEMORY);

    BCMMETER_FP_PAIRING_CHECK_ASSIGN(meter_entry->fields.meter_mode,
                                     meter_pairing);

    SHR_IF_ERR_EXIT(bcmmeter_fp_oper_mode_get(unit, stage, &oper_mode));

    for (i = 0; i < BCMMETER_FP_PAIR_NUM; i++) {
        /*
         * For non-paired meter modes, like DEFAULT and FLOW,
         * hw fields are stored at odd index, "1". Skip index "0".
         */
        if ((meter_pairing == false) && (i == BCMMETER_FP_INDEX_EVEN)) {
            continue;
        }

        byte_mode = meter_entry->hw_fields.byte_mode;
        refresh_mode = meter_entry->hw_fields.refresh_mode;
        refresh_count = meter_entry->hw_fields.refresh_count[i];
        bucket_size = meter_entry->hw_fields.bucket_size[i];
        bucket_count = meter_entry->hw_fields.bucket_count[i];
        gran = meter_entry->hw_fields.gran[i];

        sal_memset(meter_buf, 0, entry_sz);
        fid = BCMMETER_FP_BUCKETSIZE_FID(unit, stage);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                            &bucket_size);

        fid = BCMMETER_FP_BUCKETCOUNT_FID(unit, stage);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                            &bucket_count);

        fid = BCMMETER_FP_REFRESHCOUNT_FID(unit, stage);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                            &refresh_count);

        fid = BCMMETER_FP_GRAN_FID(unit, stage);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                            &gran);

        fid = BCMMETER_FP_REFRESHMODE_FID(unit, stage);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                            &refresh_mode);

        fid = BCMMETER_FP_BYTEMODE_FID(unit, stage);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                            &byte_mode);

        pt_dyn_info.index = meter_index;
        pt_dyn_info.tbl_inst =
            (oper_mode == BCMMETER_FP_OPER_MODE_GLOBAL) ? -1 : pipe_id;


        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        flags,
                                        sid,
                                        &pt_dyn_info,
                                        NULL,
                                        NULL, /* misc_info */
                                        BCMPTM_OP_WRITE,
                                        (uint32_t *)meter_buf,
                                        0,
                                        req_ltid,
                                        trans_id,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &rsp_ltid,
                                        &rsp_flags));

        meter_index++;
    }

    if (BCMMETER_FP_POOL_GLOBAL(unit, stage) == false) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_fp_slice_status_reset(unit,
                                                     trans_id,
                                                     stage,
                                                     pool,
                                                     orig_status));
    }

exit:
    SHR_FREE(meter_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free meter index
 *
 * Free the hardware meter index.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] index Hardware meter index.
 * \param [in] meter_entry Meter entry pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56960_a0_meter_fp_hw_index_free (int unit,
                                    int stage,
                                    uint32_t index,
                                    bcmmeter_fp_entry_t *meter_entry)
{
    int      pipe_id = 0;
    bool     meter_pairing = false;
    uint32_t meter_mode = 0;

    SHR_FUNC_ENTER(unit);

    meter_mode = meter_entry->fields.meter_mode;
    pipe_id = meter_entry->fields.pipe;

    BCMMETER_FP_PAIRING_CHECK_ASSIGN(meter_mode, meter_pairing);

    BCMMETER_FP_HW_INDEX_USED_CLR(unit, stage, pipe_id, index);

    /*
     * If meter is paired, then free the next index also
     */
    if (meter_pairing == true) {
        BCMMETER_FP_HW_INDEX_USED_CLR(unit, stage, pipe_id, (index + 1));
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate meter index
 *
 * Find a free hardware meter index in hardware meter table.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] pool Meter pool id.
 * \param [in] meter_entry Meter entry pointer.
 * \param [out] index Hardware meter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FULL Hardware meter table is full
 */
int
bcm56960_a0_meter_fp_hw_index_alloc (int unit,
                                     int stage,
                                     int pool,
                                     bcmmeter_fp_entry_t *meter_entry,
                                     uint32_t *index)
{
    int         i = 0;
    int         max = 0, min = 0;
    int         pipe_id = 0;
    bool        meter_pairing = false;
    uint32_t    meter_mode = 0;

    SHR_FUNC_ENTER(unit);

    meter_mode = meter_entry->fields.meter_mode;
    pipe_id = meter_entry->fields.pipe;

    BCMMETER_FP_PAIRING_CHECK_ASSIGN(meter_mode, meter_pairing);

    /*
     * Get the min and max index of the particular pool
     */
    min = (BCMMETER_FP_METERS_PER_POOL(unit, stage) * pool);
    max = (min + BCMMETER_FP_METERS_PER_POOL(unit, stage));

    /*
     * Iterate over the pool of the particular pipe and find a
     * free index
     */
    for (i = min; i < max; i++) {
        if (!BCMMETER_FP_HW_INDEX_USED_GET(unit, stage, pipe_id, i)) {
            if (meter_pairing == true) {
                /*
                 * Get even index for meter pair
                 */
                if (!(i & 0x1) &&
                    !BCMMETER_FP_HW_INDEX_USED_GET(unit,
                                                   stage,
                                                   pipe_id,
                                                   (i + 1))) {
                    *index = i;
                    break;
                }
            } else {
                *index = i;
                break;
            }
        }
    }

    if (i == max) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get meter operating values
 *
 * Calculate the operational meter rate and burst size of a meter
 * from hardware entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_entry Pointer to meter entry.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56960_a0_meter_fp_get_oper_value (int unit,
                                     int stage,
                                     bcmmeter_fp_entry_t *meter_entry)
{
    bool meter_pairing = false;
    int  meter_gran = 0;
    uint32_t burst_size = 0, bucket_size = 0;
    uint32_t meter_rate = 0, refresh_count = 0;
    bcmmeter_sw_params_t *sw_fields;
    bcmmeter_hw_params_t  *hw_fields;

    SHR_FUNC_ENTER(unit);

    sw_fields = &meter_entry->fields;
    hw_fields = &meter_entry->hw_fields;

    BCMMETER_FP_PAIRING_CHECK_ASSIGN(sw_fields->meter_mode, meter_pairing);

    meter_gran    = hw_fields->gran[BCMMETER_FP_INDEX_ODD];
    bucket_size   = hw_fields->bucket_size[BCMMETER_FP_INDEX_ODD];
    refresh_count = hw_fields->refresh_count[BCMMETER_FP_INDEX_ODD];

    /*
     * Determine meter rate and burst size (operational values) from
     * refresh count and bucket size
     */
    meter_rate = BCMMETER_COMPUTE_METER_RATE(unit,
                                             stage,
                                             sw_fields->byte_mode,
                                             meter_gran,
                                             refresh_count);
    burst_size = BCMMETER_COMPUTE_BURST_SIZE(unit,
                                             stage,
                                             sw_fields->byte_mode,
                                             meter_gran,
                                             bucket_size);
    /*
     * In TH EFP, only  REFRESHCOUNT tokens are added every refresh cycle
     * unlike IFP which adds (2 * REFRESHCOUNT) tokens.
     * So REFRESHCOUNT is doubled by s/w to achieve the desired metering rate
     * Divide by two while retrieving metering rate.
     */
    if (BCMMETER_FP_REFRESHCOUNT_ADJUST(unit, stage)) {
        meter_rate /= 2;
    }

    sw_fields->min_rate_oper  = meter_rate;
    sw_fields->min_burst_oper = burst_size;

    /*
     * If meter is in pairing mode, get the max rates
     */
    if (meter_pairing == false) {
        SHR_EXIT();
    }

    /*
     * Determine meter rate and burst size (operational values) from
     * refresh count and bucket size
     */
    meter_gran    = hw_fields->gran[BCMMETER_FP_INDEX_EVEN];
    bucket_size   = hw_fields->bucket_size[BCMMETER_FP_INDEX_EVEN];
    refresh_count = hw_fields->refresh_count[BCMMETER_FP_INDEX_EVEN];

    meter_rate = BCMMETER_COMPUTE_METER_RATE(unit,
                                             stage,
                                             sw_fields->byte_mode,
                                             meter_gran,
                                             refresh_count);
    burst_size = BCMMETER_COMPUTE_BURST_SIZE(unit,
                                             stage,
                                             sw_fields->byte_mode,
                                             meter_gran,
                                             bucket_size);

    if (BCMMETER_FP_REFRESHCOUNT_ADJUST(unit, stage)) {
        meter_rate /= 2;
    }

    sw_fields->max_rate_oper  = meter_rate;
    sw_fields->max_burst_oper = burst_size;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get meter hardware parameters.
 *
 * Get hardware parameters of this meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_entry Pointer to meter entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcm56960_a0_meter_fp_convert_to_hw_params (int unit,
                                           int stage,
                                           bcmmeter_fp_entry_t *meter_entry)
{
    int         gran = 0;
    bool        meter_pairing = false;
    uint32_t    refresh_count = 0, burst_size = 0;
    uint32_t    bucket_size = 0, bucket_count = 0;
    uint32_t    min_rate, max_rate;
    uint32_t    min_burst, max_burst;
    bcmmeter_sw_params_t *sw_fields;
    bcmmeter_hw_params_t *hw_fields;

    SHR_FUNC_ENTER(unit);

    sw_fields = &meter_entry->fields;
    hw_fields = &meter_entry->hw_fields;

    if (sw_fields->byte_mode == BCMMETER_FP_MODE_PKTS) {
        min_rate = sw_fields->min_rate_pps;
        max_rate = sw_fields->max_rate_pps;
        min_burst = sw_fields->min_burst_pkts;
        max_burst = sw_fields->max_burst_pkts;
    } else {
        min_rate = sw_fields->min_rate_kbps;
        max_rate = sw_fields->max_rate_kbps;
        min_burst = sw_fields->min_burst_kbits;
        max_burst = sw_fields->max_burst_kbits;
    }

    hw_fields->refresh_mode = 0;
    switch (sw_fields->meter_mode) {
    case BCMMETER_FP_MODE_DEFAULT:
        hw_fields->meter_pair_mode = 0;
        if (sw_fields->color_mode == BCMMETER_FP_COLOR_BLIND) {
            hw_fields->mode_modifier = 0;
        } else {
            hw_fields->mode_modifier = 1;
        }
        break;
    case BCMMETER_FP_MODE_FLOW:
        hw_fields->meter_pair_mode = 1;
        if (hw_fields->meter_hw_index & 0x1) {
            hw_fields->meter_update_odd = 1;
            hw_fields->meter_test_odd = 1;
            hw_fields->meter_update_even = 0;
            hw_fields->meter_test_even = 0;
        } else {
            hw_fields->meter_update_odd = 0;
            hw_fields->meter_test_odd = 0;
            hw_fields->meter_update_even = 1;
            hw_fields->meter_test_even = 1;
        }
        break;
    case BCMMETER_FP_MODE_SRTCM:
        hw_fields->refresh_mode = 1;
        hw_fields->mode_modifier = 0;
        if (sw_fields->color_mode == BCMMETER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 6;
        } else {
            hw_fields->meter_pair_mode = 7;
        }
        break;
    case BCMMETER_FP_MODE_MODSRTCM:
        hw_fields->refresh_mode = 1;
        hw_fields->mode_modifier = 1;
        if (sw_fields->color_mode == BCMMETER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 6;
        } else {
            hw_fields->meter_pair_mode = 7;
        }
        break;
    case BCMMETER_FP_MODE_TRTCM:
        hw_fields->mode_modifier = 0;
        if (sw_fields->color_mode == BCMMETER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 2;
        } else {
            hw_fields->meter_pair_mode = 3;
        }
        break;
    case BCMMETER_FP_MODE_MODTRTCM:
        hw_fields->mode_modifier = 0;
        if (sw_fields->color_mode == BCMMETER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 4;
        } else {
            hw_fields->meter_pair_mode = 5;
        }
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        break;
    }

    hw_fields->byte_mode = sw_fields->byte_mode;

    /*
     * Calculate granularity, refresh count,
     * bucket size for min rate, max rate
     */
    if (min_rate != 0) {
        burst_size = (min_burst * 1000);
        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_fp_compute_gran_params(unit,
                                                      stage,
                                                      min_rate,
                                                      burst_size,
                                                      hw_fields->byte_mode,
                                                      &gran,
                                                      &refresh_count,
                                                      &bucket_size,
                                                      &bucket_count));

        hw_fields->gran[BCMMETER_FP_INDEX_ODD] = gran;
        hw_fields->refresh_count[BCMMETER_FP_INDEX_ODD] = refresh_count;
        hw_fields->bucket_size[BCMMETER_FP_INDEX_ODD] = bucket_size;
        hw_fields->bucket_count[BCMMETER_FP_INDEX_ODD] = bucket_count;
    } else {
        hw_fields->gran[BCMMETER_FP_INDEX_ODD] = 0;
        hw_fields->refresh_count[BCMMETER_FP_INDEX_ODD] = 0;
        hw_fields->bucket_size[BCMMETER_FP_INDEX_ODD] = 0;
        hw_fields->bucket_count[BCMMETER_FP_INDEX_ODD] = 0;
    }

    BCMMETER_FP_PAIRING_CHECK_ASSIGN(sw_fields->meter_mode, meter_pairing);
    if (meter_pairing == true) {
        burst_size = (max_burst * 1000);
        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_fp_compute_gran_params(unit,
                                                      stage,
                                                      max_rate,
                                                      burst_size,
                                                      hw_fields->byte_mode,
                                                      &gran,
                                                      &refresh_count,
                                                      &bucket_size,
                                                      &bucket_count));

        hw_fields->gran[BCMMETER_FP_INDEX_EVEN] = gran;
        hw_fields->refresh_count[BCMMETER_FP_INDEX_EVEN] = refresh_count;
        hw_fields->bucket_size[BCMMETER_FP_INDEX_EVEN] = bucket_size;
        hw_fields->bucket_count[BCMMETER_FP_INDEX_EVEN] = bucket_count;
    } else {
        hw_fields->gran[BCMMETER_FP_INDEX_EVEN] = 0;
        hw_fields->refresh_count[BCMMETER_FP_INDEX_EVEN] = 0;
        hw_fields->bucket_size[BCMMETER_FP_INDEX_EVEN] = 0;
        hw_fields->bucket_count[BCMMETER_FP_INDEX_EVEN] = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

