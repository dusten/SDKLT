/*! \file bcm56960_a0_fp_egr_action_db.c
 *
 * EFP action configuration DB initialization function for Tomahawk(56960_A0).
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56960_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_bcm56960_a0_egress_action_cfg_db_init(int unit,
                                            bcmfp_stage_t *stage)
{

    BCMFP_ACTION_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* MEMORY EFP_POLICY_TABLE */
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_DROP,
                             0, 0, 61, 2, 1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_DROP_CANCEL,
                             0, 0, 61, 2, 2);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_DSCP,
                             0, 0, 56, 1, 1, 50, 6, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_ECN,
                             0, 0, 134, 1, 1, 128, 2, -1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_DROP,
                             0, 0, 59, 2, 1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_DROP_CANCEL,
                             0, 0, 59, 2, 2);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_DSCP,
                             0, 0, 49, 1, 1, 43, 6, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_ECN,
                             0, 0, 134, 1, 1, 128, 2, -1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_DROP,
                             0, 0, 57, 2, 1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_DROP_CANCEL,
                             0, 0, 57, 2, 2);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_DSCP,
                             0, 0, 42, 1, 1, 36, 6, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_ECN,
                             0, 0, 134, 1, 1, 128, 2, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_INNER_VLANID,
                             0, 0, 91, 1, 1, 90, 12, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_OUTER_VLANID,
                             0, 0, 78, 1, 1, 66, 12, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_OUTER_TPID,
                             0, 0, 65, 1, 1, 63, 2, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_OUTER_DOT1P,
                             0, 0, 23, 1, 1, 20, 3, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_OUTER_DOT1P,
                             0, 0, 19, 1, 1, 16, 3, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_OUTER_DOT1P,
                             0, 0, 15, 1, 1, 12, 3, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_INNER_PRI,
                             0, 0, 35, 1, 1, 32, 3, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_INNER_PRI,
                             0, 0, 31, 1, 1, 28, 3, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_INNER_PRI,
                             0, 0, 27, 1, 1, 24, 3, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_OUTER_CFI,
                             0, 0, 5, 1, 1, 4, 1, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_OUTER_CFI,
                             0, 0, 3, 1, 1, 2, 1, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_OUTER_CFI,
                             0, 0, 1, 1, 1, 0, 1, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_INNER_CFI,
                             0, 0, 11, 1, 1, 10, 1, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_INNER_CFI,
                             0, 0, 9, 1, 1, 8, 1, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_INNER_CFI,
                             0, 0, 7, 1, 1, 6, 1, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_HG_CONGESTION_CLASS,
                             0, 0, 141, 1, 1, 139, 2, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_HG_CONGESTION_CLASS,
                             0, 0, 144, 1, 1, 142, 2, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_HG_CONGESTION_CLASS,
                             0, 0, 147, 1, 1, 145, 2, -1);
    
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_HG_DST_MODULE,
                               0, 0, 138, 1, 1, 189, 1, 1, 181, 8, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_HG_DST_PORT,
                               0, 0, 138, 1, 1, 180, 1, 1, 172, 8, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_G_HG_COLOR,
                               0, 0, 138, 1, 1, 171, 1, 1, 169, 2, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_Y_HG_COLOR,
                               0, 0, 138, 1, 1, 168, 1, 1, 166, 2, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_R_HG_COLOR,
                               0, 0, 138, 1, 1, 165, 1, 1, 163, 2, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_G_HG_INTPRI,
                               0, 0, 138, 1, 1, 162, 1, 1, 158, 4, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_Y_HG_INTPRI,
                               0, 0, 138, 1, 1, 157, 1, 1, 153, 4, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_R_HG_INTPRI,
                               0, 0, 138, 1, 1, 152, 1, 1, 148, 4, -1);

    BCMFP_ACTION_CFG_ADD_FIVE(unit, stage, BCMFP_ACTION_NEW_LB_SRC_MODULE,
                              0, 0, 138, 1, 0, 177, 1, 1, 176, 1, 1, 175, 1, 1,
                              167, 8, -1);

    BCMFP_ACTION_CFG_ADD_FIVE(unit, stage, BCMFP_ACTION_NEW_LB_SRC_PORT,
                              0, 0, 138, 1, 0, 177, 1, 1, 176, 1, 1, 166, 1, 1,
                              158, 8, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_LB_PKT_PROFILE,
                               0, 0, 138, 1, 0, 157, 1, 1, 154, 3, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_LB_PP_PORT,
                               0, 0, 138, 1, 0, 153, 1, 1, 145, 8, -1);

    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_LB_TYPE,
                               0, 0, 138, 1, 0, 144, 1, 1, 139, 5, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_SET_VXLAN_HEADER_8_31,
                             0, 0, 137, 1, 1, 64, 24, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_SET_VXLAN_HEADER_56_63,
                             0, 0, 137, 1, 1, 88, 8, -1);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_SET_VXLAN_FLAGS,
                             0, 0, 137, 1, 1, 96, 8, -1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_PAIR_MODE,
                             0, 0, 106, 3, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_TEST_EVEN,
                             0, 0, 109, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_TEST_ODD,
                             0, 0, 110, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_UPDATE_EVEN,
                             0, 0, 111, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_UPDATE_ODD,
                             0, 0, 112, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_INDEX_EVEN,
                             0, 0, 113, 7, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_INDEX_ODD,
                             0, 0, 120, 7, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_PAIR_MODE_MODIFIER,
                             0, 0, 127, 1, -1);

exit:
    SHR_FUNC_EXIT();
}
