/*! \file bcm56960_a0_fp_egr_qual_db.c
 *
 * EFP qualifier configuration DB initialization function
 * for Tomahawk(56960_A0).
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
bcmfp_bcm56960_a0_egress_qual_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage)
{
    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t offset3 = 0;
    uint32_t offset4 = 0;
    uint32_t offset6 = 0;
    uint32_t offset7 = 0;
    uint32_t offset8 = 0;
    bcmfp_qual_flags_bmp_t qual_flags;

    BCMFP_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    /* EFP_KEY4(L2 Key) Qualifiers */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 0, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4_PKT, qual_flags);

    /* EG_NEXT_HOP_CLASS_ID */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 2, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_NHOP_CLASS_ID, qual_flags);

    /* EG_L3_INTF_CLASS_ID */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 2, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_L3_INTF_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 2, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_DVP_CLASS_ID, qual_flags);

    /* DVP_VALID + DVP + SPARE_DVP */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 2, 16);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_VP, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 1, 1);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_VP_VALID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 18, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 22, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_COLOR, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 24, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L2_FORMAT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 26, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_ETHERTYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 42, 48);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SRC_MAC, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 90, 48);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_MAC, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 138, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VXLT_LOOKUP_HIT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 139, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LOOKUP_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 140, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 141, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INNER_VLAN_CFI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 142, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INNER_VLAN_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 145, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INNER_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 157, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 165, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L3_ROUTABLE_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 166, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_MIRR_COPY, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 167, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 179, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_CFI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 180, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 183, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VLAN_INNER_PRESENT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 184, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VLAN_OUTER_PRESENT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 185, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_HG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 186, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SYSTEM_PORT_CLASS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 203, 4);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 194, 9);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 194, 8);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 194, 6);
    BCMFP_QUAL_CFG_OFFSET_ADD(1, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 0, 0);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASSID, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_CPU_COS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 207, 5);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L3_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 212, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 212, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VRF, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 212, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VPN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 226, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 228, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_CN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY4, offset4 + 230, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DROP_PKT, qual_flags);

    /* EFP_KEY1(IPv4 Key) Qualifiers */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 0, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 1, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 5, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_COLOR, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 7, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_IP_FLAGS_MF, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 9, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_TCP_CONTROL, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 17, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4DST_PORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 33, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4SRC_PORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 33, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_ICMP_TYPE_CODE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 49, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_TTL, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 57, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_IP_PROTO, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 65, 32);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_IP4, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 97, 32);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SRC_IP4, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 129, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_TOS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 137, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LOOKUP_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 138, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 139, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INNER_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 151, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 159, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L3_ROUTABLE_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 160, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_MIRR_COPY, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 161, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 173, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_CFI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 174, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 177, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VLAN_INNER_PRESENT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 178, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VLAN_OUTER_PRESENT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 179, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_HG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset4 + 180, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SYSTEM_PORT_CLASS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 188, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 196, 5);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 210, 4);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 201, 9);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 201, 12)
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_NHOP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 201, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_L3_INTF_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 201, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_DVP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 214, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 214, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VRF, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 214, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VPN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 228, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 230, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_CN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY1, offset1 + 232, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DROP_PKT, qual_flags);

    /* EFP_KEY2 (IPv6 Singlewide Key) Qualifiers */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 0, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 1, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_IP_PROTO, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 9 + 64, 64);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR, EGR_MODE_SIP6);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SRC_IP6_UPPER, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 9, 64);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR, EGR_MODE_SIP6);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SRC_IP6_LOWER, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 9 + 64, 64);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR, EGR_MODE_DIP6);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_IP6_UPPER, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 9, 64);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR, EGR_MODE_DIP6);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_IP6_LOWER, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 9, 64);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR, EGR_MODE_SIP_DIP_64);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SRC_IP6_HIGH, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 73, 64);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR, EGR_MODE_SIP_DIP_64);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_IP6_HIGH, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 137, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_TOS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 145, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LOOKUP_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 146, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 147, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INNER_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 159, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 167, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L3_ROUTABLE_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 168, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_MIRR_COPY, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 169, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 181, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_CFI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 182, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTER_VLAN_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 185, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VLAN_INNER_PRESENT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 186, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VLAN_OUTER_PRESENT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 187, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_HG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset4 + 188, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SYSTEM_PORT_CLASS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 205, 4);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 196, 9);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 196, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_DVP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 196, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_NHOP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 196, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_L3_INTF_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 196, 8);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 209, 5);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L3_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 214, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 214, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VRF, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 214, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VPN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 228, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 230, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_CN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY2, offset2 + 232, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DROP_PKT, qual_flags);

    /* EFP_KEY3 (IPv6 Double Wide Key) Qualifiers */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 0, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 1, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 5, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_COLOR, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 7, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_IP_FLAGS_MF, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 9, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_TCP_CONTROL, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 17, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4DST_PORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 33, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L4SRC_PORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 33, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_ICMP_TYPE_CODE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 49, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_TTL, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 57 + 64, 64);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_IP6_UPPER, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 57, 64);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_IP6_LOWER, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 185, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_IP_FIRST_EH_SUBCODE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 193, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_IP_FIRST_EH_PROTO, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 201, 5);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_L3_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 206, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_NHOP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 206, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_L3_INTF_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 206, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASSID, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_DVP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 218, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_CN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY3, offset3 + 220, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DROP_PKT, qual_flags);

    /*EFP_KEY6 (HiGiG Key)  Qualifiers*/
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 0, 32);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FABRIC_QUE_TAG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 148, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DST_MC, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 128, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DST_MC_GROUP_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 144, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DST_TRAFFIC_CLASS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 136, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DST_DST_MOD, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 120, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_SRC_DST_MOD, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 104, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LOAD_BALANCE_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 102, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_COLOR, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 101, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_EXT_HEADER_TAG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 99, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_INT_CN_NOTIFY, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 88, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_MIRR_SWITCH_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 91, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_MIRR_SWITCH_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 95, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DST_TRUNK, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 92, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DST_TRUNK_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 85, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_ING_L3_SWITCH_PACKET, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 86, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LABEL_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 84, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_MPLS_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 64, 20);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LABEL, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 64, 19);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_REPLICATION_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 48, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_VLAN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 46, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_FILTERING_MODE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 45, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_SRC_TRUNK, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 44, 1);
    BCMFP_QUAL_CFG_OFFSET_ADD(1, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 43, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PRESERVE_FLAGS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 80, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_ING_CLASS_FLAG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 95, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_EGR_MULTICAST, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 90, 5);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_FWD_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 80, 10);
    BCMFP_QUAL_CFG_OFFSET_ADD(1, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 36, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_VNI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 64, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_MC_INDEX, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 64, 16);
    BCMFP_QUAL_CFG_OFFSET_ADD(1, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 40, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_VP_REPLICATION_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 46, 1);
    BCMFP_QUAL_CFG_OFFSET_ADD(1, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 45, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DONOT_FLAGS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 44, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_VP_LAG_FAIL_OVER_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 43, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PROTECTION_SW_STATUS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 47, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_MIRR_TO_VP, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 35, 1);
    BCMFP_QUAL_CFG_OFFSET_ADD(1, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 34, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_VP_PRESERVE_FLAGS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 33, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_DST_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 32, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_SRC_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 80, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_CLASS_TAG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 79, 1);
    BCMFP_QUAL_CFG_OFFSET_ADD(1, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 76, 1);
    BCMFP_QUAL_CFG_OFFSET_ADD(2, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 72, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_DEF_FLG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 77, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_VXLT_STATUS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 73, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_PKT_PRI_NEW, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 66, 6);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_DSCP_NEW, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 47, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_PRESERVE_DSCP, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 46, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_PRESERVE_DOT1P, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 43, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_SRC_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 40, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OPCODE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 160, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LOOKUP_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 161, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 162, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 170, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SYSTEM_PORT_CLASS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 178, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 195, 4);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 186, 9);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 186, 12)
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_NHOP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 186, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_L3_INTF_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 186, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_DVP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 199, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_CN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY6, offset6 + 201, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DROP_PKT, qual_flags);

    /* EFP_KEY7(Loopback Key) qualifiers */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 116, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_LOOPBACK_QUEUE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 111, 5);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_LOOPBACK_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 93, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_PKT_IS_VISIBLE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 90, 3);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_LOOPBACK_CPU_MSQRD_PKT_PROF, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 4, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_LOOPBACK_COLOR, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 0, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_LOOPBACK_TRAFFIC_CLASS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 0, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_LOOPBACK_PKT_PROCESSING_PORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 128, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LOOKUP_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 129, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 130, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 138, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SYSTEM_PORT_CLASS, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 146, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 154, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 158, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_COLOR, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 169, 4);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 160, 9);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 160, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_NHOP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 160, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_L3_INTF_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 160, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_DVP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 173, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_CN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY7, offset7 + 175, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DROP_PKT, qual_flags);

    /* EFP_KEY8(Bytes After L2 Key) Qualifiers */
    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 159, 144);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_DISABLE, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_BYTES_AFTER_L2HEADER, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 1, 16);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_VP, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset4 + 0, 1);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_VP_VALID, qual_flags);
    BCMFP_QUAL_CFG_INIT(unit);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 1, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_NHOP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 1, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_L3_INTF_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 1, 12);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_EGR_DVP_CLASS_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 17, 4);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_PRI, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 21, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_COLOR, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 23, 16);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_ETHERTYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 39, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_LOOKUP_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 40, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_PKT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 41, 8);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 49, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_MIRR_COPY, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 50, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DST_HG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 51, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_SYSTEM_PORT_CLASS, qual_flags);
    BCMFP_QUAL_CFG_INIT(unit);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 203, 8);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_OUTPORT, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 203, 6);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_HG_OFLD_ENG_DEF_FLG, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 212, 4);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 203, 9);
    BCMFP_QUAL_CFG_CTRL_SEL_ADD(BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID, 0);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FP_ING_CLASSID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 216, 12);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_VLAN_ID, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 216, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VRF, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 216, 14);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_VPN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 230, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_FWD_TYPE, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 232, 2);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_INT_CN, qual_flags);

    BCMFP_QUAL_CFG_INIT(unit);
    BCMFP_QUAL_CFG_OFFSET_ADD(0, BCMFP_EXT_SECTION_FPF3, EFP_KEY8, offset8 + 234, 1);
    BCMFP_QUAL_CFG_INSERT(unit, stage, BCMFP_QUAL_DROP_PKT, qual_flags);

exit:
    SHR_FUNC_EXIT();
}


