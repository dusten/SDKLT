/*! \file bcm56960_a0_fp_ing_presel_qual_db.c
 *
 * IFP preselection qualifier configuration DB
 * initialization function for Tomahawk(56960_A0).
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
bcmfp_bcm56960_a0_ingress_presel_qual_cfg_db_init(int unit,
                                                  bcmfp_stage_t *stage)
{
    bcmfp_qual_flags_bmp_t qual_flags;
    BCMFP_PRESEL_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));
    /* EXACT_MATCH_LOGICAL_TABLE_ID_0 - 5 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_EM_FIRST_LOOKUP_LTID,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         32,
                                         0);

    /* EXACT_MATCH_LOGICAL_TABLE_ID_1 - 5 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_EM_SECOND_LOOKUP_LTID,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         37,
                                         0);

    /* HG_LOOKUP - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_HIGIGLOOKUP_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         43,
                                         0);

    /* MIRRO- 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MIRROR_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         45,
                                         0);

    /* HG- 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_HIGIG_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         46,
                                         0);

    /* DROP - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_DROP_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         47,
                                         0);

    /* PKT_RESOLUTION - 6 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_PKT_RES,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         6,
                                         48,
                                         0);

    /* FORWARDING_TYPE - 4 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_FWD_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         4,
                                         54,
                                         0);

    /* LOOKUP_STATUS_VECTOR - 19 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_VXLT_LOOKUP_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         3,
                                         58,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_FWD_VLAN_VALID,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         61,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_INGRESS_STP_STATE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         2,
                                         62,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L2SRC_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         64,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L2SRC_STATIC,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         65,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L2DST_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         66,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L2CACHE_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         67,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L3SRC_HOST_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         68,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L3DST_HOST_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         69,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L3DST_LPM_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         70,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L2STATION_MOVE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         71,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_DOSATTACK_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         72,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L3_TUNNEL_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         73,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         74,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MPLSENTRY_SECONDLOOKUP_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         75,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MPLS_BOS_TERMINATED,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         76,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_SRC_VP_VALID,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         77,
                                         0);

    /* SVP_VALID - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MYSTATIONTCAM_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         78,
                                         0);

    /* TUNNEL_TYPE_LOOPBACK_TYPE - 5 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_LOOPBACK_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         79,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_TUNNEL_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         79,
                                         0);
    /* LOOPBACK - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_LOOPBACK,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         83,
                                         0);

    /* L4_VALID - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L4_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         84,
                                         0);

    /* L3_TYPE - 5 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L3_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         85,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MIXED_SRC_CLASS,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         32,
                                         0,
                                         0);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_em_presel_qual_cfg_db_init(int unit,
                                                  bcmfp_stage_t *stage)
{
    bcmfp_qual_flags_bmp_t qual_flags;
    BCMFP_PRESEL_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    /* HG_LOOKUP - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_HIGIGLOOKUP_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         36,
                                         0);

    /* MIRRO- 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MIRROR_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         33,
                                         0);

    /* HG- 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_HIGIG_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         34,
                                         0);

    /* DROP - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_DROP_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         35,
                                         0);
    /* FORWARDING_TYPE - 4 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_FWD_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         4,
                                         42,
                                         0);

    /* LOOKUP_STATUS_VECTOR - 19 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_VXLT_LOOKUP_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         3,
                                         47,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_FWD_VLAN_VALID,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         49,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_INGRESS_STP_STATE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         2,
                                         50,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_DOSATTACK_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         60,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L3_TUNNEL_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         61,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         62,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MPLSENTRY_SECONDLOOKUP_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         63,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MPLS_BOS_TERMINATED,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         64,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_SRC_VP_VALID,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         65,
                                         0);

    /* SVP_VALID - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MYSTATIONTCAM_HIT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         66,
                                         0);

    /* TUNNEL_TYPE_LOOPBACK_TYPE - 5 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_LOOPBACK_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         67,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_TUNNEL_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         67,
                                         0);
    /* LOOPBACK - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_LOOPBACK,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         71,
                                         0);

    /* L4_VALID - 1 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L4_PKT,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         1,
                                         72,
                                         0);

    /* L3_TYPE - 5 */
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_L3_TYPE,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         5,
                                         73,
                                         0);

    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit,
                                         stage,
                                         BCMFP_QUAL_MIXED_SRC_CLASS,
                                         qual_flags,
                                         BCMFP_EXT_SECTION_DISABLE,
                                         0,
                                         32,
                                         0,
                                         0);

exit:
    SHR_FUNC_EXIT();
}
