/*! \file bcm56960_a0_tm_imm_device_pkt_rx_q.c
 *
 * TM DEVICE RX queue imm function handlers.
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

#include <shr/shr_debug.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include "bcm56960_a0_tm_imm_internal.h"
#include <bcmtm/bcmtm_internal_socmem_glue.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmltd/chip/bcmltd_device_constants.h>

/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief
 *
 * Function implementing PT write for DEVICE RX queue.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
 * \param [in] channel RX channel
 * \param [in] cos_bmp COS bitmap
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM fails to handle LT change events.
 */
static int
bcm56960_a0_tm_device_rx_q_pt_set(int unit,
                             bcmltd_sid_t ltid,
                             uint32_t channel,
                             uint64_t cos_bmp)
{
    bcmdrd_sid_t device_rx_q_pt0[] = {
                CMIC_CMC0_CH0_COS_CTRL_RX_0r,
                CMIC_CMC0_CH1_COS_CTRL_RX_0r,
                CMIC_CMC0_CH2_COS_CTRL_RX_0r,
                CMIC_CMC0_CH3_COS_CTRL_RX_0r,
                CMIC_CMC1_CH0_COS_CTRL_RX_0r,
                CMIC_CMC1_CH1_COS_CTRL_RX_0r,
                CMIC_CMC1_CH2_COS_CTRL_RX_0r,
                CMIC_CMC1_CH3_COS_CTRL_RX_0r,
                CMIC_CMC2_CH0_COS_CTRL_RX_0r,
                CMIC_CMC2_CH1_COS_CTRL_RX_0r,
                CMIC_CMC2_CH2_COS_CTRL_RX_0r,
                CMIC_CMC2_CH3_COS_CTRL_RX_0r,
                };
    bcmdrd_sid_t device_rx_q_pt1[] = {
                CMIC_CMC0_CH0_COS_CTRL_RX_1r,
                CMIC_CMC0_CH1_COS_CTRL_RX_1r,
                CMIC_CMC0_CH2_COS_CTRL_RX_1r,
                CMIC_CMC0_CH3_COS_CTRL_RX_1r,
                CMIC_CMC1_CH0_COS_CTRL_RX_1r,
                CMIC_CMC1_CH1_COS_CTRL_RX_1r,
                CMIC_CMC1_CH2_COS_CTRL_RX_1r,
                CMIC_CMC1_CH3_COS_CTRL_RX_1r,
                CMIC_CMC2_CH0_COS_CTRL_RX_1r,
                CMIC_CMC2_CH1_COS_CTRL_RX_1r,
                CMIC_CMC2_CH2_COS_CTRL_RX_1r,
                CMIC_CMC2_CH3_COS_CTRL_RX_1r,
                };
    uint32_t ltm_buf[3] = {0};
    uint32_t fval;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    sid = device_rx_q_pt0[channel];
    fid = COS_BMPf;
    fval = COMPILER_64_LO(cos_bmp);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                    (void *)&pt_dyn_info, ltm_buf));
    sid = device_rx_q_pt1[channel];
    fid = COS_BMPf;
    fval = COMPILER_64_HI(cos_bmp);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                    (void *)&pt_dyn_info, ltm_buf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE RX queue IMM table change callback function for staging.
 *
 * Handle DEVICE RX queue IMM table change.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcm56960_a0_bcmtm_device_rx_q_imm_stage_callback(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             bcmimm_entry_event_t event_reason,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             void *context,
                             bcmltd_fields_t *output_fields)
{
    uint32_t channel, i;
    BCMLTD_BITDCL cos_bmap = 0;

    SHR_FUNC_ENTER(unit);
    if (output_fields) {
        output_fields->count = 0;
    }
    if (key->id != DEVICE_PKT_RX_Qt_RX_Qf) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    channel = COMPILER_64_LO(key->data);
    if (channel > 11) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    for (i = 0; (i < MAX_CPU_COS) && data; i++) {
        if (data->id != DEVICE_PKT_RX_Qt_COSf) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        if (data->data == 1) {
            BCMLTD_BIT_SET(&cos_bmap, data->idx);
        } else {
            BCMLTD_BIT_CLR(&cos_bmap, data->idx);
        }
        data = data->next;
    }
    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_EXIT
            (bcm56960_a0_tm_device_rx_q_pt_set(unit, sid, channel, cos_bmap));
        break;
    case BCMIMM_ENTRY_DELETE:
        cos_bmap = 0;
        SHR_IF_ERR_EXIT
            (bcm56960_a0_tm_device_rx_q_pt_set(unit, sid, channel, cos_bmap));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE RX queue register routine.
 *
 * \param [in] unit Unit number.
 * \param [in] ITM number.
 * \param [in] Event class type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
static int
bcm56960_a0_tm_device_rx_q_imm(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    /*
     * This structure contains callback functions that corresponds
     * to the DEVICE_PKT_RX_Q logical table entry commit stages.
 */
    bcmimm_lt_cb_t cpu_cos_imm_callback = {

        /*! Validate function. */
        .validate = NULL,

        /*! Staging function. */
        .stage = bcm56960_a0_bcmtm_device_rx_q_imm_stage_callback,

        /*! Commit function. */
        .commit = NULL,

        /*! Abort function. */
        .abort = NULL
    };

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for DEVICE_PKT_RX_Q LT.
     */
    rv = bcmlrd_map_get(unit, DEVICE_PKT_RX_Qt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DEVICE_PKT_RX_Qt,
                                 &cpu_cos_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56960_a0_tm_device_imm_rx_q_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Device RX queue IMM callback */
    SHR_IF_ERR_EXIT(bcm56960_a0_tm_device_rx_q_imm(unit));

exit:
    SHR_FUNC_EXIT();
}
