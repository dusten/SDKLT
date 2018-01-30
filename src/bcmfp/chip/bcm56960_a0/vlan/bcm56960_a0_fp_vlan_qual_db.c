/*! \file bcm56960_a0_fp_vlan_qual_db.c
 *
 * VFP qualifier configuration DB initialization function
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
bcmfp_bcm56960_a0_vlan_qual_cfg_db_init(int unit,
                                        bcmfp_stage_t *stage)
{
    uint32_t f1_offset = 164, f2_offset = 36, f3_offset = 0;
    bcmfp_qual_flags_bmp_t qual_flags;

    BCMFP_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));
/* VFP_F1*/
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_PROTO_COMMON,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            3, f1_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_PROTO_COMMON,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            3, f1_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_TUNNEL_TERMINATED,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 3, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_IP_CHECKSUM_VALID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 4, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_INNER_TPID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            2, f1_offset + 5, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_OUTER_TPID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            2, f1_offset + 7, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_L2_FORMAT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            2, f1_offset + 9, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_HIGIGLOOKUP_PKT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 11, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_HIGIG_PKT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 12, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_OUTER_VLAN_ID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            12, f1_offset + 13, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_OUTER_VLAN_CFI,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 25, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_OUTER_VLAN_PRI,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            3, f1_offset + 26, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_VLAN_INNER_PRESENT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 29, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_VLAN_OUTER_PRESENT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 30, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_L3_TYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            5, f1_offset + 31, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L3_TYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            5,  f1_offset + 31, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_TWO_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_TRUNK,
                            qual_flags,
                            BCMFP_EXT_SECTION_FPF1, 0,
                            16, f1_offset + 36, 0,/* trunk id field of SGLP */
                            0, 0,
                            1, f1_offset + 52, 0,/* trunk bit of SGLP */
                            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_GLP);

    BCMFP_QUAL_CFG_TWO_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_VP,
                       qual_flags,
                       BCMFP_EXT_SECTION_FPF1, 0,
                       17, f1_offset + 36, 0,  /* S_FIELD */
                       0, 0,
                       1, f1_offset + 56, 0,/* SVP_VALID */
                       BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_SVPGPORT);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 36, 0,/* Port field of unresolved SGLP */
            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_MODPORTGPORT);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_MODULE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 36 + 8, 0,/* mod  field of unresolved SGLP */
            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_MODPORTGPORT);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INPORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 36, 0,/* ingress port field */
            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_PORTGROUPNUM);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SYSTEM_PORT_CLASS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 36 + 8, 0,/* ingress port group field */
            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_PORTGROUPNUM);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_L4_PKT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 56, 0);

/* VFP_F2_0*/
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TTL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TTL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TCP_CONTROL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            6, f2_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_FLAGS_MF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 14, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_FLAGS_DF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 15, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FLAGS_MF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 14, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FLAGS_DF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 15, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_L4DST_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L4DST_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_L4SRC_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 40, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L4SRC_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 40, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_ICMP_TYPE_CODE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 40, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 56, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f2_offset + 56, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 96, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 96, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_F2_1 */
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP6_UPPER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP6_LOWER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP6_UPPER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP6_LOWER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

 /* VFP_F2_2 */
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_DST_IP6_UPPER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_DST_IP6_LOWER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_DST_IP6_UPPER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_DST_IP6_LOWER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_F2_3 */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_ETHERTYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 3,
            16, f2_offset + 16, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_SRC_MAC,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 3,
            48, f2_offset + 32, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_DST_MAC,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 3,
            48, f2_offset + 80, 0);

 /* VFP_F2_4 */
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset + 48, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset + 48, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_SRC_MAC,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            48, f2_offset + 80, 0);

/* VFP_F2_5 */
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset + 48, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset + 48, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_DST_MAC,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            48, f2_offset + 80, 0);

/* VFP_F2_6 */
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP6_HIGH,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP6_HIGH,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_DST_IP6_HIGH,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_DST_IP6_HIGH,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_F2_7 */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_VNTAG,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 7,
            32, f2_offset + 0, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_SNAP_HEADER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 7,
            40, f2_offset + 64, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_LLC_HEADER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 7,
            24, f2_offset + 104, 0);

/* VFP_F2_8  */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK7,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK6,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 16, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK5,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 32, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 48, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK3,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 64, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK2,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 80, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK1,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 96, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF1_CHUNK0,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 112, 0);

/* VFP_F2_9 */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK7,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK6,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 16, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK5,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 32, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 48, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK3,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 64, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK2,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 80, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK1,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 96, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK0,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 112, 0);

/* VFP_F2_10 */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_INNER_HEADER_TYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            3, f2_offset + 0, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_OUTER_HEADER_TYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            3, f2_offset + 3, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FCOE_SOF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            8, f2_offset + 6, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_DATA_FIELD_CTRL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            8, f2_offset + 14, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_CLASS_SPEC_CTRL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            8, f2_offset + 22, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_FRAME_CTRL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            24, f2_offset + 30, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_TYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            8, f2_offset + 54, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_DST_ID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            24, f2_offset + 62, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_SRC_ID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            24, f2_offset + 86, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_ROUTING_CTRL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 10,
            8, f2_offset + 110, 0);

/* VFP_F3_0 */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_INNER_VLAN_ID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            12, f3_offset + 0, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_INNER_VLAN_CFI,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            1, f3_offset + 12, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_INNER_VLAN_PRIORITY,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            3, f3_offset + 13, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_ETHERTYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            16, f3_offset + 16, 0);

/* VFP_F3_1 */
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_L4DST_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L4DST_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_L4SRC_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L4SRC_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_ICMP_TYPE_CODE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_F3_2 */
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_IP_FIRST_EH_SUBCODE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 16, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_IP_FIRST_EH_PROTO,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 24, 0);

/* VFP_F3_3 */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_L3_INTF_CLASS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 3,
            8, f3_offset + 0, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_INNER_VLAN_ID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 3,
            12, f3_offset + 8, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_INNER_VLAN_CFI,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 3,
            1, f3_offset + 20, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_INNER_VLAN_PRIORITY,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 3,
            3, f3_offset + 21, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_SRC_MODULE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 3,
            8, f3_offset + 24, 0);

/* VFP_F3_4 */

/* VFP_F3_5 */
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_VFT_HOP_COUNT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 5,
            8, f3_offset, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_VFT_VSAN_ID,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 5,
            12, f3_offset + 8, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_VFT_VSAN_PRI,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 5,
            3, f3_offset + 20, 0);

    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, BCMFP_QUAL_FIBRE_VFT_VERSION,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 5,
            2, f3_offset + 23, 0);

/* VFP_DOUBLE_WIDE_F3 */
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TTL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TTL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TCP_CONTROL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            6, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_FLAGS_MF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            1, f3_offset + 22, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_FLAGS_DF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            1, f3_offset + 23, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FLAGS_MF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            1, f3_offset + 22, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FLAGS_DF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            1, f3_offset + 23, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_DOUBLE_WIDE_F2_0 */
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TTL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TTL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TCP_CONTROL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            6, f2_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_FLAGS_MF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 14, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_FLAGS_DF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 15, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FLAGS_MF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 14, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FLAGS_DF,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            1, f2_offset + 15, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_TOS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_L4DST_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L4DST_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_L4SRC_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 40, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L4SRC_PORT,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 40, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_ICMP_TYPE_CODE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 40, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 56, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_PROTOCOL,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 56, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_DST_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 96, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 96, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_DOUBLE_WIDE_F2_1 */
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP6_UPPER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_INNER_SRC_IP6_LOWER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP6_UPPER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_SRC_IP6_LOWER,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_DOUBLE_WIDE_F2_2 */
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK0,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK1,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 16, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK2,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 32, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK3,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 48, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK4,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 64, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK5,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 80, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK6,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 96, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_UDF2_CHUNK7,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 112, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

/* VFP_DOUBLE_WIDE_F1 */
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FIRST_EH_SUBCODE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_IP_FIRST_EH_PROTO,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 8, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_L3_INTF_CLASS,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, BCMFP_QUAL_ETHERTYPE,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            16, f1_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

exit:
    SHR_FUNC_EXIT();
}
