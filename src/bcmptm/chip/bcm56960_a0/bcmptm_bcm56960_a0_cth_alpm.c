/*! \file bcmptm_bcm56960_a0_cth_alpm.c
 *
 * Chip specific functions for CTH ALPM
 *
 * This file contains the chip specific functions for CTH ALPM
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>

#include <bcmptm/bcmptm_internal.h>

#include "bcmptm_bcm56960_a0_cth_alpm.h"
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CTHALPM
#define KEY_INPUT_128  0   /* IPv6-128 */
#define KEY_INPUT_64  1   /* IPv6-64 */

#define L1_DB0 0
#define L1_DB1 1    /* Valid only if dual_mode, indicating Global */
#define L1_DBS 2
/*******************************************************************************
 * Private Functions
 */

static int
bcm56960_a0_cth_alpm_control_encode(int u, bcmptm_cth_alpm_control_t *ctrl,
                                    cth_alpm_control_pt_t *pts)
{
    int v6_cam[4] = {0};
    int tcam_sel[8] = {0};
    L3_DEFIP_KEY_SELr_t key_sel;
    L3_DEFIP_RPF_CONTROLr_t rpf_ctrl;
    L3_DEFIP_ALPM_CFGr_t alpm_cfg;

    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(pts, SHR_E_PARAM);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    pts->count = 3;

    /* If entry/sid pointer is null, simply return count. */
    if (!pts->entry || !pts->sid) {
        SHR_EXIT();
    }

    L3_DEFIP_KEY_SELr_CLR(key_sel);
    L3_DEFIP_RPF_CONTROLr_CLR(rpf_ctrl);
    L3_DEFIP_ALPM_CFGr_CLR(alpm_cfg);

    /* $ KEY_SEL setting */
    if (!ctrl->urpf) {
        
        v6_cam[0] = (ctrl->l1_key_input[0] == KEY_INPUT_128) ? 1 : 0;
        v6_cam[1] = (ctrl->l1_key_input[1] == KEY_INPUT_128) ? 1 : 0;
        v6_cam[2] = (ctrl->l1_key_input[2] == KEY_INPUT_128) ? 1 : 0;
        v6_cam[3] = (ctrl->l1_key_input[3] == KEY_INPUT_128) ? 1 : 0;
    } else {
        v6_cam[0] = (ctrl->l1_key_input[0] == KEY_INPUT_128) ? 1 : 0;
        v6_cam[1] = (ctrl->l1_key_input[1] == KEY_INPUT_128) ? 1 : 0;
        v6_cam[2] = (ctrl->l1_key_input[2] == KEY_INPUT_128) ? 1 : 0;
        v6_cam[3] = (ctrl->l1_key_input[3] == KEY_INPUT_128) ? 1 : 0;
        
        assert(v6_cam[0] == v6_cam[2]);
        assert(v6_cam[1] == v6_cam[3]);

        L3_DEFIP_KEY_SELr_URPF_LOOKUP_CAM4f_SET(key_sel, 0x1);
        L3_DEFIP_KEY_SELr_URPF_LOOKUP_CAM5f_SET(key_sel, 0x1);
        L3_DEFIP_KEY_SELr_URPF_LOOKUP_CAM6f_SET(key_sel, 0x1);
        L3_DEFIP_KEY_SELr_URPF_LOOKUP_CAM7f_SET(key_sel, 0x1);
    }

    L3_DEFIP_KEY_SELr_V6_KEY_SEL_CAM0_1f_SET(key_sel, v6_cam[0]);
    L3_DEFIP_KEY_SELr_V6_KEY_SEL_CAM2_3f_SET(key_sel, v6_cam[1]);
    L3_DEFIP_KEY_SELr_V6_KEY_SEL_CAM4_5f_SET(key_sel, v6_cam[2]);
    L3_DEFIP_KEY_SELr_V6_KEY_SEL_CAM6_7f_SET(key_sel, v6_cam[3]);

    /* $ RPF_CONTROL setting */
    switch (ctrl->alpm_mode) {
    case ALPM_MODE_COMBINED:
        if (ctrl->db0_levels == 1) { /* Level-1 only */
            L3_DEFIP_RPF_CONTROLr_LPM_MODEf_SET(rpf_ctrl, 0);
            L3_DEFIP_RPF_CONTROLr_LOOKUP_MODEf_SET(rpf_ctrl, 0);
        } else { /* Legcay ALPM mode */
            L3_DEFIP_RPF_CONTROLr_LPM_MODEf_SET(rpf_ctrl, 1);
            L3_DEFIP_RPF_CONTROLr_LOOKUP_MODEf_SET(rpf_ctrl, 0);
        }
        break;
    case ALPM_MODE_PARALLEL:
    default:
        if (ctrl->db1_levels == 2) { /* Legacy parallel */
            L3_DEFIP_RPF_CONTROLr_LPM_MODEf_SET(rpf_ctrl, 1);
            L3_DEFIP_RPF_CONTROLr_LOOKUP_MODEf_SET(rpf_ctrl, 1);
        } else { /* Legacy TCAM/ALPM mode */
            L3_DEFIP_RPF_CONTROLr_LPM_MODEf_SET(rpf_ctrl, 1);
            L3_DEFIP_RPF_CONTROLr_LOOKUP_MODEf_SET(rpf_ctrl, 2);
        }
        break;
    }
    L3_DEFIP_RPF_CONTROLr_DEFIP_RPF_ENABLEf_SET(rpf_ctrl, ctrl->urpf);

    /* $ ALPM_CFG setting */
    switch (ctrl->alpm_mode) {
    case ALPM_MODE_COMBINED:
        if (ctrl->urpf) {
            tcam_sel[4] = 2;
            tcam_sel[5] = 2;
            tcam_sel[6] = 2;
            tcam_sel[7] = 2;
        }
        break;
    case ALPM_MODE_PARALLEL:
    default:
        if (ctrl->urpf) {
            tcam_sel[2] = 1;
            tcam_sel[3] = 1;
            tcam_sel[4] = 2;
            tcam_sel[5] = 2;
            tcam_sel[6] = 3;
            tcam_sel[7] = 3;
        } else {
            tcam_sel[4] = 1;
            tcam_sel[5] = 1;
            tcam_sel[6] = 1;
            tcam_sel[7] = 1;
        }
        break;
    }
    L3_DEFIP_ALPM_CFGr_TCAM0_SELf_SET(alpm_cfg, tcam_sel[0]);
    L3_DEFIP_ALPM_CFGr_TCAM1_SELf_SET(alpm_cfg, tcam_sel[1]);
    L3_DEFIP_ALPM_CFGr_TCAM2_SELf_SET(alpm_cfg, tcam_sel[2]);
    L3_DEFIP_ALPM_CFGr_TCAM3_SELf_SET(alpm_cfg, tcam_sel[3]);
    L3_DEFIP_ALPM_CFGr_TCAM4_SELf_SET(alpm_cfg, tcam_sel[4]);
    L3_DEFIP_ALPM_CFGr_TCAM5_SELf_SET(alpm_cfg, tcam_sel[5]);
    L3_DEFIP_ALPM_CFGr_TCAM6_SELf_SET(alpm_cfg, tcam_sel[6]);
    L3_DEFIP_ALPM_CFGr_TCAM7_SELf_SET(alpm_cfg, tcam_sel[7]);

    pts->sid[0] = L3_DEFIP_KEY_SELr;
    pts->sid[1] = L3_DEFIP_RPF_CONTROLr;
    pts->sid[2] = L3_DEFIP_ALPM_CFGr;
    sal_memcpy(pts->entry[0], &key_sel, sizeof(key_sel));
    sal_memcpy(pts->entry[1], &rpf_ctrl, sizeof(rpf_ctrl));
    sal_memcpy(pts->entry[2], &alpm_cfg, sizeof(alpm_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_cth_alpm_control_validate(int u, bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);
    switch (ctrl->num_l2_banks) {
    case 0:
    case 2:
    case 4:
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_cth_alpm_device_info_init(int u, cth_alpm_device_info_t *dev)
{
    bcmptm_cth_alpm_control_t *ctrl;

    SHR_FUNC_ENTER(u);

    if (dev) {
        ctrl = &dev->default_control;
        dev->tcam_blocks = 8;
        dev->tcam_depth = 1024;
        ctrl->alpm_mode = ALPM_MODE_COMBINED;
        ctrl->db0_levels = 1;
        ctrl->db1_levels = 1;
        /* All l1 blocks paired by default */
        ctrl->l1_key_input[0] = KEY_INPUT_128;
        ctrl->l1_key_input[1] = KEY_INPUT_128;
        ctrl->l1_key_input[2] = KEY_INPUT_128;
        ctrl->l1_key_input[3] = KEY_INPUT_128;
        ctrl->l1_db[0] = L1_DB0;
        ctrl->l1_db[1] = L1_DB0;
        ctrl->l1_db[2] = L1_DB0;
        ctrl->l1_db[3] = L1_DB0;
        ctrl->urpf = 0;
        ctrl->l1_db_valid = 0;
        ctrl->urpf_valid = 1;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56960_a0_cth_alpm_driver_register(int u, cth_alpm_driver_t *drv)
{
    SHR_FUNC_ENTER(u);

    if (drv) {
        drv->control_encode = bcm56960_a0_cth_alpm_control_encode;
        drv->control_validate = bcm56960_a0_cth_alpm_control_validate;
        drv->device_info_init = bcm56960_a0_cth_alpm_device_info_init;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}


