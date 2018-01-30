/*! \file bcm56960_a0_fp_ing_action_db.c
 *
 * IFP action configuration DB initialization function for Tomahawk(56960_A0).
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
bcmfp_bcm56960_a0_ingress_action_cfg_db_init(int unit,
                                             bcmfp_stage_t *stage)
{
    uint32_t offset = 0;                  /* General variable to carry offset. */
    uint32_t profile_one_offset = 0;      /* Offset of PROFILE_ONE_SET. */
    uint32_t profile_two_offset = 0;      /* Offset of PROFILE_TWO_SET. */
    uint32_t redirect_offset = 0;         /* Offset of REDIRECT_SET. */
    uint32_t l3swl2change_offset = 0;     /* Offset of L3SW_CHANGE_L2_SET. */
    uint32_t nat_offset = 0;              /* Offset of NAT_SET. */
    uint32_t mirror_offset = 0;           /* Offset of MIRROR_SET. */
    uint32_t lb_offset = 0;               /* Offset of LB_CONTROL_SET. */
    uint32_t nat_override_offset = 0;     /* Offset of NAT_OVERRIDE_SET. */
    uint32_t copytocpu_offset = 0;        /* Offset of COPY_TO_CPU_SET. */
    uint32_t cutthrough_offset = 0;       /* Offset of CUT_THRU_OVERRIDE_SET. */
    uint32_t urpf_offset = 0;             /* Offset of URPF_OVERRIDE_SET. */
    uint32_t ttl_offset = 0;              /* Offset of TTL_SET. */
    uint32_t cpucos_offset = 0;           /* Offset of CHANGE_CPU_COS_SET. */
    uint32_t drop_offset = 0;             /* Offset of DROP_SET. */
    uint32_t mirror_override_offset = 0;  /* Offset of MIRROR_OVERRIDE_SET. */
    uint32_t sflow_offset = 0;            /* Offset of SFLOW_SET. */
    uint32_t debug_offset = 0;            /* Offset of INSTRUMENTATION_TRIGGERS_ENABLE. */
    uint32_t meter_offset = 0;            /* Offset of IFP_METER_SET. */

    BCMFP_ACTION_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);


    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
        if (stage->stage_id == BCMFP_STAGE_ID_INGRESS) {
            SHR_IF_ERR_EXIT(
                bcmfp_bcm56960_a0_ingress_presel_action_cfg_db_init(unit, stage));
        } else if (stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {
            SHR_IF_ERR_EXIT(
                bcmfp_bcm56960_a0_em_presel_action_cfg_db_init(unit, stage));
        }
    }
    if (stage->stage_id == BCMFP_STAGE_ID_INGRESS) {
        profile_one_offset     = 0;  /* Start of the IFP_POLICY_TABLE */
        mirror_offset          = 56; /* IFP_PROFILE_SET_1(56b) */
        lb_offset              = 68; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) */
        nat_override_offset    = 71; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      *  LB_CONTROLS_SET(3b)
                                      */
        copytocpu_offset       = 72; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      *  LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b)
                                      */
        cutthrough_offset      = 89; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      *  COPY_TO_CPU_SET(17b)
                                      */
        urpf_offset            = 90; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b)
                                      */
        ttl_offset             = 91; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b)
                                      */
        profile_two_offset    = 101; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b)
                                      */
        l3swl2change_offset   = 153; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b) +
                                      * PROFILE_SET_2(52b)
                                      */
        cpucos_offset         = 176; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b) +
                                      * PROFILE_SET_2(52b) + L3SW_CHANGE_L2_SET(33b)
                                      */

        drop_offset           = 184; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b) +
                                      * PROFILE_SET_2(52b) + L3SW_CHANGE_L2_SET(33b)
                                      * + CHANGE_CPU_COS_SET(8b)
                                      */
        mirror_override_offset = 190; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       */
        sflow_offset           = 192; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b)
                                       */
        debug_offset           = 193; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b) + SFLOW_SET(1b)
                                       */
        redirect_offset        = 202; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b) + SFLOW_SET(1b) +
                                       * INSTRUMENTATION_TRIGGERS_ENABLE(1b) +
                                       * ECCP_1(8b)
                                       */
        nat_offset             = 283; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b) + SFLOW_SET(1b) +
                                       * INSTRUMENTATION_TRIGGERS_ENABLE(1b) +
                                       * ECCP_1(8b) + REDIRECT_SET(38b) +
                                       * COUNTER_SET(25b) + METER_SET(18b)
                                       */
        meter_offset           = 265;

    }

    /* IFP_PROFILE_SET_1(56b) */
    offset = profile_one_offset;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_UNTAG_PKT_PRIORITY,
            0, BCMFP_ACTION_SET_INPUT_PRIORITY,
            offset + 52, 4, -1, offset + 51, 1, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_NEW_R_COLOR,
            0, BCMFP_ACTION_SET_DROP_PRECEDENCE,
            offset + 0, 2, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_NEW_Y_COLOR,
            0, BCMFP_ACTION_SET_DROP_PRECEDENCE,
            offset + 2, 2, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_NEW_G_COLOR,
            0, BCMFP_ACTION_SET_DROP_PRECEDENCE,
            offset + 4, 2, -1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_COS_MAP,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 6, 2, -1, 30, 4, 2);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_COSQ,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 6, 8, -1, 30, 4, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_COS_MAP,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 6, 2, -1, 30, 4, 2);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_COSQ,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 6, 8, -1, 30, 4, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_COS_MAP,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 6, 2, -1, 30, 4, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_INTPRI_TO_INNER_DOT1P,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 30, 4, 4);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_INTPRI,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 6, 8, -1, 30, 4, 5);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_INTPRI_TO_TOS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 30, 4, 6);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_INTPRI_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 30, 4, 7);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_UC_COS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 6, 4, -1, 30, 4, 8);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_MC_COS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 10, 4, -1, 30, 4, 9);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_COSQ,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 14, 8, -1, 34, 4, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_COS_MAP,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 14, 2, -1, 34, 4, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_INTPRI_TO_INNER_DOT1P,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 34, 4, 4);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_INTPRI,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 14, 8, -1, 34, 4, 5);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_INTPRI_TO_TOS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 34, 4, 6);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_INTPRI_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 34, 4, 7);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_UC_COS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 14, 4, -1, 34, 4, 8);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_MC_COS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 18, 4, -1, 34, 4, 9);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_COSQ,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 22, 8, -1, 38, 4, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_COS_MAP,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 22, 2, -1, 38, 4, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_INTPRI_TO_INNER_DOT1P,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 38, 4, 4);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_INTPRI,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 22, 8, -1, 38, 4, 5);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_INTPRI_TO_TOS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 38, 4, 6);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_INTPRI_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 38, 4, 7);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_UC_COS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 22, 4, -1, 38, 4, 8);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_MC_COS,
            0, BCMFP_ACTION_SET_COS_OR_INT_PRI,
            offset + 26, 4, -1, 38, 4, 9);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
            BCMFP_ACTION_NEW_G_INTCN,
            0, BCMFP_ACTION_SET_INT_CN,
            offset + 46, 2, -1, 50, 1, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
            BCMFP_ACTION_NEW_Y_INTCN,
            0, BCMFP_ACTION_SET_INT_CN,
            offset + 44, 2, -1, 49, 1, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage,
            BCMFP_ACTION_NEW_R_INTCN,
            0, BCMFP_ACTION_SET_INT_CN,
            offset + 42, 2, -1, 48, 1, 1);


    /* IFP_MIRROR_SET(12b) */
    offset = mirror_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_MIRROR_INST_0,
            0, BCMFP_ACTION_SET_MIRROR, offset + 0, 2, -1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_MIRROR_INST_1,
            0, BCMFP_ACTION_SET_MIRROR, offset + 2, 2, -1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_MIRROR_INST_2,
            0, BCMFP_ACTION_SET_MIRROR, offset + 4, 2, -1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_MIRROR_INST_3,
            0, BCMFP_ACTION_SET_MIRROR, offset + 6, 2, -1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_MIRROR_SET,
            0, BCMFP_ACTION_SET_MIRROR, offset + 8, 4, -1);

    /* IFP_LB_CONTROLS_SET(3b) */
    offset = lb_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
            BCMFP_ACTION_HGT_SPRAY_HASH_CANCEL,
            0, BCMFP_ACTION_SET_LB_CONTROL,
            offset + 0, 1, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
            BCMFP_ACTION_TRUNK_SPARY_HASH_CANCEL,
            0, BCMFP_ACTION_SET_LB_CONTROL,
            offset + 1, 1, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage,
            BCMFP_ACTION_ECMP_SPRAY_HASH_CANCEL,
            0, BCMFP_ACTION_SET_LB_CONTROL,
            offset + 2, 1, 1);

    /* IFP_NAT_OVERRIDE_SET(1b) */
    offset = nat_override_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_DO_NOT_NAT,
            0, BCMFP_ACTION_SET_NAT_OVERRIDE,
            offset + 0, 1, 1);
    /* IFP_COPY_TO_CPU_SET(17b) */
    offset = copytocpu_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_COPY_TO_CPU,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_MATCH_ID,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 9, 8, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_COPY_TO_CPU_CANCEL,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 0, 3, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_SWITCH_TO_CPU_CANCEL,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 0, 3, 3);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_SWITCH_TO_CPU_REINSATE,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 0, 3, 4);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_COPY_TO_CPU_WITH_TIMESTAMP,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 0, 3, 5);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_COPY_TO_CPU,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 3, 3, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_COPY_TO_CPU_CANCEL,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 3, 3, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_SWITCH_TO_CPU_CANCEL,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 3, 3, 3);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_SWITCH_TO_CPU_REINSATE,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 3, 3, 4);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMP,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 3, 3, 5);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_COPY_TO_CPU,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 6, 3, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_COPY_TO_CPU_CANCEL,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 6, 3, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_SWITCH_TO_CPU_CANCEL,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 6, 3, 3);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_SWITCH_TO_CPU_REINSATE,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 6, 3, 4);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_COPY_TO_CPU_WITH_TIMESTAMP,
            0, BCMFP_ACTION_SET_COPY_TO_CPU,
            offset + 6, 3, 5);

    /* IFP_CUT_THRU_OVERRIDE_SET(1b) */
    offset = cutthrough_offset;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_DO_NOT_CUT_THROUGH,
            0, BCMFP_ACTION_SET_CUT_THRU_OVERRIDE,
            offset + 0, 1, 1);

    /* IFP_URPF_OVERRIDE_SET(1b) */
    offset = urpf_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_DO_NOT_URPF,
            0, BCMFP_ACTION_SET_URPF_OVERRIDE,
            offset + 0, 1, 1);

    /* IFP_TTL_SET(1b) */
    offset = ttl_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_DO_NOT_CHANGE_TTL,
            0, BCMFP_ACTION_SET_TTL_OVERRIDE,
            offset + 0, 1, 1);

    /* IFP_PROFILE_SET_2(52b) */
    offset = profile_two_offset;

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_ECN,
            BCMFP_ACTION_NO_EM_SUPPORT, BCMFP_ACTION_SET_CHANGE_ECN,
            offset + 0, 2, -1, offset + 6, 1, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_ECN,
            BCMFP_ACTION_NO_EM_SUPPORT, BCMFP_ACTION_SET_CHANGE_ECN,
            offset + 2, 2, -1, offset + 7, 1, 1);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_ECN,
            BCMFP_ACTION_NO_EM_SUPPORT, BCMFP_ACTION_SET_CHANGE_ECN,
            offset + 4, 2, -1, offset + 8, 1, 1);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_PRESERVE_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 18, 3, 3);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_OUTER_DOT1P_TO_INNER_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 18, 3, 4);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_OUTER_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 9, 3, -1, offset + 18, 3, 5);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_OUTER_DOT1P_TO_TOS,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 18, 3, 6);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_DOT1P_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 18, 3, 7);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_PRESERVE_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 21, 3, 3);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_OUTER_DOT1P_TO_INNER_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 21, 3, 4);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_OUTER_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 12, 3, -1, offset + 21, 3, 5);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_OUTER_DOT1P_TO_TOS,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 21, 3, 6);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_DOT1P_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 21, 3, 7);

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_PRESERVE_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 24, 3, 3);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_OUTER_DOT1P_TO_INNER_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 24, 3, 4);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_OUTER_DOT1P,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 15, 3, -1, offset + 24, 3, 5);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_OUTER_DOT1P_TO_TOS,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 24, 3, 6);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_DOT1P_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_CHANGE_PKT_PRI,
            offset + 24, 3, 7);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_R_DSCP,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 40, 6, -1, offset + 27, 2, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_DSCP_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 27, 2, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_PRESERVE_DSCP,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 27, 2, 3);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_Y_DSCP,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 34, 6, -1, offset + 29, 2, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_DSCP_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 29, 2, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_PRESERVE_DSCP,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 29, 2, 3);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_DSCP,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 46, 6, -1, offset + 31, 3, 3);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_DSCP_UPDATES_CANCEL,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 31, 3, 4);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_PRESERVE_DSCP,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 31, 3, 5);

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_G_TOS,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 46, 6, -1, offset + 31, 3, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_TOS_TO_PRE_FP_ING_OUTER_DOT1P,
            0, BCMFP_ACTION_SET_DSCP_TOS,
            offset + 31, 3, 2);

    /* IFP_L3SW_CHANGE_L2_SET(33b) */
    offset = l3swl2change_offset;

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_CHANGE_PKT_L2_FIELDS,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 0, 15, -1, offset + 19, 4, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_CHANGE_PKT_L2_FIELDS_CANCEL,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 19, 4, 2);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_CLASSIFICATION_TAG,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 0, 16, -1, offset + 19, 4, 4);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_SWITCH_TO_L3UC,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 0, 15, -1,
            offset + 17, 1, 0, offset + 19, 4, 6);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_SWITCH_TO_ECMP,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 0, 11, -1,
            offset + 17, 1, 1, offset + 19, 4, 6);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_ECMP_HASH,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 11, 3, -1, offset + 19, 4, 6);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_L3_SWITCH_CANCEL,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 19, 4, 7);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_I2E_CLASSID_SELECT,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 0, 4, -1, offset + 19, 4, 8);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_HIGIG_CLASSID_SYSTEM_PORT,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 4, 3, -1, offset + 19, 4, 8);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_I2E_CLASSID,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 7, 9, -1, offset + 19, 4, 8);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_BFD_SEESSION_IDX,
            0, BCMFP_ACTION_SET_L3_SW_CHANGE_L2,
            offset + 0, 12, -1, offset + 19, 4, 9);
    /* IFP_CHANGE_CPU_COS_SET(8b) */
    offset = cpucos_offset;

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_CPU_COS,
            0, BCMFP_ACTION_SET_CPU_COS,
            offset + 0, 6, -1, offset + 6, 2, 1);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_NEW_SERVICE_POOL_ID, 0,
            BCMFP_ACTION_SET_CPU_COS,
            offset + 2, 2, -1,
            offset + 4, 2, 2,
            offset + 6, 2, 2);
    /* IFP_DROP_SET(6b) */
    offset = drop_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_DROP,
            0, BCMFP_ACTION_SET_DROP, offset + 0, 2, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_R_DROP_CANCEL,
            0, BCMFP_ACTION_SET_DROP, offset + 0, 2, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_DROP,
            0, BCMFP_ACTION_SET_DROP, offset + 2, 2, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_Y_DROP_CANCEL,
            0, BCMFP_ACTION_SET_DROP, offset + 2, 2, 2);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_DROP,
            0, BCMFP_ACTION_SET_DROP, offset + 4, 2, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_G_DROP_CANCEL,
            0, BCMFP_ACTION_SET_DROP, offset + 4, 2, 2);

    /* IFP_MIRROR_OVERRIDE_SET(1b) */
    offset = mirror_override_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_MIRROR_OVERRIDE,
            0, BCMFP_ACTION_SET_MIRROR_OVERRIDE, offset + 0, 1, 1);

    /* IFP_SFLOW_SET(1b) */
    offset = sflow_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_SFLOW_ENABLE,
            0, BCMFP_ACTION_SET_SFLOW, offset + 0, 1, 1);

    /* IFP_INSTRUMENTATION_TRIGGERS_ENABLE_SET(1b)*/
    offset = debug_offset;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_ENABLE_PKT_VISIBILITY,
            BCMFP_ACTION_NO_EM_SUPPORT, BCMFP_ACTION_SET_INSTRUMENTATION,
            offset + 0, 1, 1);

    /* IFP_REDIRECT_SET(38b) */
    offset = redirect_offset;

    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage, BCMFP_ACTION_REDIRECT_TO_MODULE,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 14, 8, -1, offset + 22 , 1, 0,
            offset + 3, 3, 0, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage, BCMFP_ACTION_REDIRECT_TO_PORT,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 8, -1, offset + 22 , 1, 0,
            offset + 3, 3, 6, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_REDIRECT_TO_DVP,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 14, -1,
            offset + 3, 3, 6, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage, BCMFP_ACTION_REDIRECT_TO_TRUNK,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 10, -1, offset + 22, 1, 1,
            offset + 3, 3, 0, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage,
            BCMFP_ACTION_UNMODIFIED_REDIRECT_TO_MODULE,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 14, 8, -1, offset + 22, 1, 0,
            offset + 3, 3, 1, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage,
            BCMFP_ACTION_UNMODIFIED_REDIRECT_TO_TRUNK,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 10, -1, offset + 22, 1, 1,
            offset + 3, 3, 1, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage,
            BCMFP_ACTION_UNMODIFIED_REDIRECT_TO_PORT,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 8, -1, offset + 22, 1, 0,
            offset + 3, 3, 1, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_REDIRECT_TO_NHOP,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 16, -1, offset + 3, 3, 2, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_REDIRECT_TO_ECMP,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 16, -1, offset + 3, 3, 3, offset + 0, 3, 1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_REDIRECT_UC_CANCEL,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 0, 3, 2);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_REDIRECT_TO_FP_ING_PBM,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 10, -1, offset + 23, 2, 0, offset + 0, 3, 3);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_REDIRECT_TO_INGRESS_VLAN_BMP,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 17, 1, 0, offset + 23, 2, 1, offset + 0, 3, 3);
    BCMFP_ACTION_CFG_ADD_FOUR(unit, stage, BCMFP_ACTION_REDIRECT_TO_INGRESS_VLAN_FP_ING_PBM,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 10, -1, offset + 17, 1, 1,
            offset + 23, 2, 1, offset + 0, 3, 3);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_REDIRECT_TO_L2MC_GROUP,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 14, -1, offset + 23, 2, 2, offset + 0, 3, 3);
    BCMFP_ACTION_CFG_ADD_THREE(unit, stage, BCMFP_ACTION_REDIRECT_TO_L3MC_GROUP,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 14, -1, offset + 23, 2, 3, offset + 0, 3, 3);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_MASK_EGRESS_PBMP,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 10, -1, offset + 0, 3, 4);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_EGRESS_PORTS_ADD,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 6, 10, -1, offset + 0, 3, 5);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NEW_HIGIG_EH,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 3, 32, -1, offset + 0, 3, 6);
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_HIGIG_EH_MASK_PROFILE_ID,
            0, BCMFP_ACTION_SET_REDIRECT,
            offset + 35, 3, -1, offset + 0, 3, 6);

    /* IFP_NAT_SET(12b) */
    offset = nat_offset;

    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, BCMFP_ACTION_NAT_EGRESS_OVERRIDE,
            0, BCMFP_ACTION_SET_NAT,
            offset + 0, 10, -1, offset + 10, 1, 1);

    /* IFP_METER_SET */
    offset = meter_offset;

    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_TEST_ODD,
            0, BCMFP_ACTION_SET_METER,
            offset + 17, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_UPDATE_ODD,
            0, BCMFP_ACTION_SET_METER,
            offset + 16, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_TEST_EVEN,
            0, BCMFP_ACTION_SET_METER,
            offset + 15, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_UPDATE_EVEN,
            0, BCMFP_ACTION_SET_METER,
            offset + 14, 1, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_PAIR_INDEX,
            0, BCMFP_ACTION_SET_METER,
            offset + 4, 10, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_PAIR_MODE,
            0, BCMFP_ACTION_SET_METER,
            offset + 1, 3, -1);
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_METER_PAIR_MODE_MODIFIER,
            0, BCMFP_ACTION_SET_METER,
            offset + 0, 1, -1);

    /* Exact match class id */
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, BCMFP_ACTION_EXACT_MATCH_CLASS_ID,
            BCMFP_ACTION_NO_IFP_SUPPORT, 0, 0, 12, 0);

exit:
    SHR_FUNC_EXIT();
}
