/*! \file bcm56960_a0_fp_ing_qual_enum.c
 *
 * Some qualifiers in IFP are exposed as enumerations to end user.
 * User provides one of the many enumerated values supported for
 * those qualifers and internally the SDK converts that enumerated
 * value  to data and mask to be configured in h/w. This file contains
 * data and mask set/get functions for all such qualifiers for
 * Tomahawk(56960_A0).
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

/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_stpstate_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_INGRESS_STP_STATE from
 *     the given enum value.
 *
 * Parameters:
 *     unit     - (IN) BCM unit number
 *     stp_state- (IN) STP state
 *     *data    - (OUT) Qualifier data part
 *     *mask    - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_stpstate_set(int unit,
                                                int stp_state,
                                                uint32_t *data,
                                                uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch(stp_state) {
        case BCMFP_STG_STP_DISABLE:
            *data = 0x00;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_BLOCK:
            *data = 0x01;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_LEARN:
            *data = 0x02;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_FORWARD:
            *data = 0x03;
            *mask = 0x03;
            break;
        case BCMFP_STG_STP_LEARN_FORWARD:
            *data = 0x02;
            *mask = 0x02;
            break;
        case BCMFP_STG_STP_LEARN_DISABLE:
            *data = 0x00;
            *mask = 0x01;
            break;
        case BCMFP_STG_STP_FORWARD_BLOCK:
            *data = 0x01;
            *mask = 0x01;
            break;
        case BCMFP_STG_STP_DISABLE_BLOCK:
            *data = 0x00;
            *mask = 0x02;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_stpstate_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_STP_STATE from the
 *    given data and mask.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     data      - (IN) Qualifier data part
 *     mask      - (IN) Qualifier mask part
 *     *stp_state- (OUT) STP state
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_stpstate_get(int unit,
                                                uint32_t data,
                                                uint32_t mask,
                                                int *stp_state)

{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stp_state, SHR_E_PARAM);

    if ((data == 0x0) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_DISABLE;
    } else  if ((data == 0x1) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_BLOCK;
    } else  if ((data == 0x2) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_LEARN;
    } else  if ((data == 0x3) && (mask == 0x3)) {
        *stp_state = BCMFP_STG_STP_FORWARD;
    } else  if ((data == 0x2) && (mask == 0x2)) {
        *stp_state = BCMFP_STG_STP_LEARN_FORWARD;
    } else  if ((data == 0x0) && (mask == 0x1)) {
        *stp_state = BCMFP_STG_STP_LEARN_DISABLE;
    } else  if ((data == 0x1) && (mask == 0x1)) {
        *stp_state = BCMFP_STG_STP_FORWARD_BLOCK;
    } else  if ((data == 0x0) && (mask == 0x2)) {
        *stp_state = BCMFP_STG_STP_DISABLE_BLOCK;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_vxlt_lookup_hit_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_VXLT_LOOKUP_HIT from
 *     the given enum value.
 *
 * Parameters:
 *     unit     - (IN) BCM unit number
 *     hit      - (IN) lookup hit
 *     *data    - (OUT) Qualifier data part
 *     *mask    - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_vxlt_lookup_hit_set(int unit,
                                                       int hit,
                                                       uint32_t *data,
                                                       uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch(hit) {
        case BCMFP_VXLT_NO_HIT:
            *data = 0x00;
            *mask = 0x00;
            break;
        case BCMFP_VXLT_FIRST_LOOKUP_HIT:
            *data = 0x01;
            *mask = 0x01;
            break;
        case BCMFP_VXLT_SECOND_LOOKUP_HIT:
            *data = 0x02;
            *mask = 0x02;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_vxlt_lookup_hit_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_VXLT_LOOKUP_HIT from the
 *    given data and mask.
 *
 * Parameters:
 *     unit - (IN) BCM unit number
 *     data - (IN) Qualifier data part
 *     mask - (IN) Qualifier mask part
 *     *hit - (OUT) lookup hit
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_vxlt_lookup_hit_get(int unit,
                                                       uint32_t data,
                                                       uint32_t mask,
                                                       int *hit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hit, SHR_E_PARAM);

    if ((data == 0x0) && (mask == 0x0)) {
        *hit = BCMFP_VXLT_NO_HIT;
    } else  if ((data == 0x1) && (mask == 0x1)) {
        *hit = BCMFP_VXLT_FIRST_LOOKUP_HIT;
    } else  if ((data == 0x2) && (mask == 0x2)) {
        *hit = BCMFP_VXLT_SECOND_LOOKUP_HIT;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *Function: bcmfp_bcm56960_a0_ingress_qual_tunnel_type_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_TUNNEL_TYPE from
 *     the given enum value.
 *
 * Parameters:
 *     unit  - (IN) BCM unit number
 *     type  - (IN) tunnel type
 *     *data - (OUT) Qualifier data part
 *     *mask - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_tunnel_type_set(int unit,
                                                   int type,
                                                   uint32_t *data,
                                                   uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch(type) {
        case BCMFP_TNL_TYPE_ANY:
            *data = 0x0;
            *mask = 0x0;
            break;
        case BCMFP_TNL_TYPE_NONE:
            *data = 0x0;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_IP:
            *data = 0x1;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_MPLS:
            *data = 0x2;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_MIM:
            *data = 0x3;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_AUTO_IP4_MC:
            *data = 0x6;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_TRILL:
            *data = 0x7;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_L2GRE:
            *data = 0x8;
            *mask = 0x1f;
            break;
        case BCMFP_TNL_TYPE_VXLAN:
            *data = 0x9;
            *mask = 0x1f;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_tunnel_type_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_TUNNEL_TYPE from the
 *    given data and mask.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     data      - (IN) Qualifier data part
 *     mask      - (IN) Qualifier mask part
 *     *type     - (OUT) tunnel type
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_tunnel_type_get(int unit,
                                                   uint32_t data,
                                                   uint32_t mask,
                                                   int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    switch (data & mask) {
        case 0x1:
            *type = BCMFP_TNL_TYPE_IP;
            break;
        case 0x2:
            *type = BCMFP_TNL_TYPE_MPLS;
            break;
        case 0x3:
            *type = BCMFP_TNL_TYPE_MIM;
            break;
        case 0x6:
            *type = BCMFP_TNL_TYPE_AUTO_IP4_MC;
            break;
        case 0x7:
            *type = BCMFP_TNL_TYPE_TRILL;
            break;
        case 0x8:
            *type = BCMFP_TNL_TYPE_L2GRE;
            break;
        case 0x9:
            *type = BCMFP_TNL_TYPE_VXLAN;
            break;
        case 0x0:
            if(mask) {
                *type = BCMFP_TNL_TYPE_NONE;
                break;
            }
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *Function: bcmfp_bcm56960_a0_ingress_qual_loopback_type_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_LOOPBACK_TYPE from
 *     the given enum value.
 *
 * Parameters:
 *     unit  - (IN) BCM unit number
 *     type  - (IN) loopback type
 *     *data - (OUT) Qualifier data part
 *     *mask - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_loopback_type_set(int unit,
                                                     int type,
                                                     uint32_t *data,
                                                     uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch (type) {
        case BCMFP_LB_TYPE_ANY:
            *data = 0x10;
            *mask = 0x10;
            break;
        case BCMFP_LB_TYPE_MIM:
            *data = 0x10;
            *mask = 0x1f;
            break;
        case BCMFP_LB_TYPE_TRILL_NETWORK:
            *data = 0x11;
            *mask = 0x1f;
            break;
        case BCMFP_LB_TYPE_TRILL_ACCESS:
            *data = 0x12;
            *mask = 0x1f;
            break;
        case BCMFP_LB_TYPE_VXLAN:
            *data = 0x1b;
            *mask = 0x1f;
            break;
        case BCMFP_LB_TYPE_L2GRE:
            *data = 0x1e;
            *mask = 0x1f;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_loopback_type_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_LOOPBACK_TYPE from the
 *    given data and mask.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     data      - (IN) Qualifier data part
 *     mask      - (IN) Qualifier mask part
 *     *type     - (OUT) loopback type
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_loopback_type_get(int unit,
                                                     uint32_t data,
                                                     uint32_t mask,
                                                     int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    switch (data & mask) {
        case 0x10:
            if (mask == 0x10) {
                *type = BCMFP_LB_TYPE_ANY;
            } else {
                *type = BCMFP_LB_TYPE_MIM;
            }
            break;
        case 0x11:
            *type = BCMFP_LB_TYPE_TRILL_NETWORK;
            break;
        case 0x12:
            *type = BCMFP_LB_TYPE_TRILL_ACCESS;
            break;
        case 0x1b:
            *type = BCMFP_LB_TYPE_VXLAN;
            break;
        case 0x1e:
            *type = BCMFP_LB_TYPE_L2GRE;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 *Function: bcmfp_bcm56960_a0_ingress_qual_pkt_res_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_PKT_RES from
 *     the given enum value.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     pkt_res   - (IN) pkt resolution
 *     *data     - (OUT) Qualifier data part
 *     *mask     - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_pkt_res_set(int unit,
                                               int pkt_res,
                                               uint32_t *data,
                                               uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch(pkt_res) {
        case BCMFP_UNKNOWN_PKT:
            *data = 0x00;
            *mask = 0x00;
            break;
        case BCMFP_CONTROL_PKT:
            *data = 0x01;
            *mask = 0x01;
            break;
        case BCMFP_BPDU_PKT:
            *data = 0x04;
            *mask = 0x04;
            break;
        case BCMFP_L2BC_PKT:
            *data = 0x0c;
            *mask = 0x0c;
            break;
        case BCMFP_L2UC_PKT:
            *data = 0x08;
            *mask = 0x08;
            break;
        case BCMFP_UNKNOWN_L2_PKT:
            *data = 0x09;
            *mask = 0x09;
            break;
        case BCMFP_UNKNOWN_L3MC_PKT:
            *data = 0x13;
            *mask = 0x13;
            break;
        case BCMFP_KNOWN_L3MC_PKT:
            *data = 0x12;
            *mask = 0x12;
            break;
        case BCMFP_KNOWN_L2MC_PKT:
            *data = 0x0a;
            *mask = 0x0a;
            break;
        case BCMFP_UNKNOWN_L2MC_PKT:
            *data = 0x0b;
            *mask = 0x0b;
            break;
        case BCMFP_KNOWN_L3UC_PKT:
            *data = 0x10;
            *mask = 0x10;
            break;
        case BCMFP_UNKNOWN_L3UC_PKT:
            *data = 0x11;
            *mask = 0x11;
            break;
        case BCMFP_KNOWN_MPLS_PKT:
            *data = 0x1c;
            *mask = 0x1c;
            break;
        case BCMFP_KNOWN_MPLS_L3_PKT:
            *data = 0x1a;
            *mask = 0x1a;
            break;
        case BCMFP_KNOWN_MPLS_L2_PKT:
            *data = 0x18;
            *mask = 0x18;
            break;
        case BCMFP_UNKNOWN_MPLS_PKT:
            *data = 0x19;
            *mask = 0x19;
            break;
        case BCMFP_KNOWN_MIM_PKT:
            *data = 0x20;
            *mask = 0x20;
            break;
        case BCMFP_UNKNOWN_MIM_PKT:
            *data = 0x21;
            *mask = 0x21;
            break;
        case BCMFP_KNOWN_TRILL_PKT:
            *data = 0x28;
            *mask = 0x28;
            break;
        case BCMFP_UNKNOWN_TRILL_PKT:
            *data = 0x29;
            *mask = 0x29;
            break;
        case BCMFP_KNOWN_NIV_PKT:
            *data = 0x30;
            *mask = 0x30;
            break;
        case BCMFP_UNKNOWN_NIV_PKT:
            *data = 0x31;
            *mask = 0x31;
            break;
        case BCMFP_OAM_PKT:
            *data = 0x02;
            *mask = 0x02;
            break;
        case BCMFP_BFD_PKT:
            *data = 0x03;
            *mask = 0x03;
            break;
        case BCMFP_IEEE1588_PKT:
            *data = 0x06;
            *mask = 0x06;
            break;
        case BCMFP_KNOWN_L2GRE_PKT:
            *data = 0x32;
            *mask = 0x32;
            break;
        case BCMFP_KNOWN_VXLAN_PKT:
            *data = 0x33;
            *mask = 0x33;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_pkt_res_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_PKT_RES from the
 *    given data and mask.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     data      - (IN) Qualifier data part
 *     mask      - (IN) Qualifier mask part
 *     *pkt_res  - (OUT) resolution
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_pkt_res_get(int unit,
                                               uint32_t data,
                                               uint32_t mask,
                                               int *pkt_res)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pkt_res, SHR_E_PARAM);

    switch(data & mask) {
        case 0x00:
            *pkt_res = BCMFP_UNKNOWN_PKT;
            break;
        case 0x01:
            *pkt_res = BCMFP_CONTROL_PKT;
            break;
        case 0x04:
            *pkt_res = BCMFP_BPDU_PKT;
            break;
        case 0x0c:
            *pkt_res = BCMFP_L2BC_PKT;
            break;
        case 0x08:
            *pkt_res = BCMFP_L2UC_PKT;
            break;
        case 0x09:
            *pkt_res = BCMFP_UNKNOWN_L2_PKT;
            break;
        case 0x13:
            *pkt_res = BCMFP_UNKNOWN_L3MC_PKT;
            break;
        case 0x12:
            *pkt_res = BCMFP_KNOWN_L3MC_PKT;
            break;
        case 0x0a:
            *pkt_res = BCMFP_KNOWN_L2MC_PKT;
            break;
        case 0x0b:
            *pkt_res = BCMFP_UNKNOWN_L2MC_PKT;
            break;
        case 0x10:
            *pkt_res = BCMFP_KNOWN_L3UC_PKT;
            break;
        case 0x11:
            *pkt_res = BCMFP_UNKNOWN_L3UC_PKT;
            break;
        case 0x1c:
            *pkt_res = BCMFP_KNOWN_MPLS_PKT;
            break;
        case 0x1a:
            *pkt_res = BCMFP_KNOWN_MPLS_L3_PKT;
            break;
        case 0x18:
            *pkt_res = BCMFP_KNOWN_MPLS_L2_PKT;
            break;
        case 0x19:
            *pkt_res = BCMFP_UNKNOWN_MPLS_PKT;
            break;
        case 0x20:
            *pkt_res = BCMFP_KNOWN_MIM_PKT;
            break;
        case 0x21:
            *pkt_res = BCMFP_UNKNOWN_MIM_PKT;
            break;
        case 0x28:
            *pkt_res = BCMFP_KNOWN_TRILL_PKT;
            break;
        case 0x29:
            *pkt_res = BCMFP_UNKNOWN_TRILL_PKT;
            break;
        case 0x30:
            *pkt_res = BCMFP_KNOWN_NIV_PKT;
            break;
        case 0x31:
            *pkt_res = BCMFP_UNKNOWN_NIV_PKT;
            break;
        case 0x02:
            *pkt_res = BCMFP_OAM_PKT;
            break;
        case 0x03:
            *pkt_res = BCMFP_BFD_PKT;
            break;
        case 0x06:
            *pkt_res = BCMFP_IEEE1588_PKT;
            break;
        case 0x32:
            *pkt_res = BCMFP_KNOWN_L2GRE_PKT;
            break;
        case 0x33:
            *pkt_res = BCMFP_KNOWN_VXLAN_PKT;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 *Function: bcmfp_bcm56960_a0_ingress_qual_mpls_fwd_lbl_action_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_MPLS_FWD_LABEL_ACTION from
 *     the given enum value.
 *
 * Parameters:
 *     unit    - (IN) BCM unit number
 *     action  - (IN) mpls forward label action
 *     *data   - (OUT) Qualifier data part
 *     *mask   - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_mpls_fwd_lbl_action_set(int unit,
                                                           int action,
                                                           uint32_t *data,
                                                           uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    *mask = 0x7;
    switch(action)  {
        case BCMFP_MPLS_ACT_PHP:
            *data = 0x2;
            break;
        case BCMFP_MPLS_ACT_SWAP:
            *data = 0x3;
            break;
        case BCMFP_MPLS_ACT_POP:
            *data = 0x4;
            break;
        case BCMFP_MPLS_ACT_POP_USE_L2_VPN:
            *data = 0x5;
            break;
        case BCMFP_MPLS_ACT_POP_USE_L3_VPN:
            *data = 0x6;
            break;
        case BCMFP_MPLS_ACT_POP_USE_ENTROPY:
            *data = 0x7;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_ingress_qual_mpls_fwd_lbl_action_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_MPLS_FWD_LABEL_ACTION from the
 *    given data and mask.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     data      - (IN) Qualifier data part
 *     mask      - (IN) Qualifier mask part
 *     *action   - (OUT) mpls forward label action
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_ingress_qual_mpls_fwd_lbl_action_get(int unit,
                                                           uint32_t data,
                                                           uint32_t mask,
                                                           int *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    switch (data ) {
        case 0x2:
            *action = BCMFP_MPLS_ACT_PHP;
            break;
        case 0x3:
            *action = BCMFP_MPLS_ACT_SWAP;
            break;
        case 0x4:
            *action = BCMFP_MPLS_ACT_POP;
            break;
        case 0x5:
            *action = BCMFP_MPLS_ACT_POP_USE_L2_VPN;
            break;
        case 0x6:
            *action = BCMFP_MPLS_ACT_POP_USE_L3_VPN;
            break;
        case 0x7:
            *action = BCMFP_MPLS_ACT_POP_USE_ENTROPY;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:  bcmfp_bcm56960_a0_ingress_qual_enum_set
 *
 * Purpose:
 *     Get the data and mask for the given qualifier from its
 *     enum set.
 *
 * Parameters:
 *     unit  - (IN) BCM unit number
 *     qual  - (IN) Qualifier name
 *     value - (IN) Qualifier value
 *     *data - (OUT) Qualifier data part
 *     *mask - (OUT) Qualifier mask part
 * Returns:
 *     SHR_E_XXX
 */
int
bcmfp_bcm56960_a0_ingress_qual_enum_set(int unit,
                                        bcmfp_qual_t qual,
                                        int value,
                                        uint32_t *data,
                                        uint32_t *mask)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch (qual) {
        case BCMFP_QUAL_L3_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_ip_type_set(unit,
                                                              value,
                                                              data,
                                                              mask));
        case BCMFP_QUAL_COLOR:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_color_set(unit,
                                                            value,
                                                            data,
                                                            mask));
        case BCMFP_QUAL_L2_FORMAT:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_l2format_set(unit,
                                                               value,
                                                               data,
                                                               mask));
        case BCMFP_QUAL_INNER_TPID:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_innertpid_set(unit,
                                                                value,
                                                                data,
                                                                mask));
        case BCMFP_QUAL_OUTER_TPID:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_outertpid_set(unit,
                                                                value,
                                                                data,
                                                                mask));
        case BCMFP_QUAL_FWD_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_forwardingtype_set(unit,
                                                                     value,
                                                                     data,
                                                                     mask));
        case BCMFP_QUAL_IP_FRAG:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_ipfrag_set(unit,
                                                             value,
                                                             data,
                                                             mask));
        case BCMFP_QUAL_INGRESS_STP_STATE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_stpstate_set(unit,
                                                                value,
                                                                data,
                                                                mask));
        case BCMFP_QUAL_VXLT_LOOKUP_HIT:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_vxlt_lookup_hit_set(unit,
                                                                       value,
                                                                       data,
                                                                       mask));
        case BCMFP_QUAL_TUNNEL_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_tunnel_type_set(unit,
                                                                   value,
                                                                   data,
                                                                   mask));
        case BCMFP_QUAL_LOOPBACK_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_loopback_type_set(unit,
                                                                    value,
                                                                    data,
                                                                    mask));
        case BCMFP_QUAL_PKT_RES:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_pkt_res_set(unit,
                                                               value,
                                                               data,
                                                               mask));
        case BCMFP_QUAL_MPLS_FORWARDING_LABEL_ACTION:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_mpls_fwd_lbl_action_set(unit,
                                                                           value,
                                                                           data,
                                                                           mask));
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:  bcmfp_bcm56960_a0_ingress_qual_enum_get
 *
 * Purpose:
 *     Get the enum value for the given qualifier from its
 *     data and mask.
 *
 * Parameters:
 *     unit   - (IN) BCM unit number
 *     qual   - (IN) Qualifier name
 *     data   - (IN) Qualifier data part
 *     mask   - (IN) Qualifier mask part
 *     *value - (OUT) Qualifier value
 * Returns:
 *     SHR_E_XXX
 */
int
bcmfp_bcm56960_a0_ingress_qual_enum_get(int unit,
                                        bcmfp_qual_t qual,
                                        uint32_t data,
                                        uint32_t mask,
                                        int *value)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_INTERNAL);

    switch (qual) {
        case BCMFP_QUAL_L3_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_ip_type_get(unit,
                                                              data,
                                                              mask,
                                                              value));
        case BCMFP_QUAL_COLOR:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_color_get(unit,
                                                            data,
                                                            mask,
                                                            value));
        case BCMFP_QUAL_L2_FORMAT:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_l2format_get(unit,
                                                               data,
                                                               mask,
                                                               value));
        case BCMFP_QUAL_INNER_TPID:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_innertpid_get(unit,
                                                                data,
                                                                mask,
                                                                value));
        case BCMFP_QUAL_OUTER_TPID:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_outertpid_get(unit,
                                                                data,
                                                                mask,
                                                                value));
        case BCMFP_QUAL_FWD_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_forwardingtype_get(unit,
                                                                     data,
                                                                     mask,
                                                                     value));
        case BCMFP_QUAL_IP_FRAG:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_ipfrag_get(unit,
                                                             data,
                                                             mask,
                                                             value));
        case BCMFP_QUAL_INGRESS_STP_STATE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_stpstate_get(unit,
                                                                data,
                                                                mask,
                                                                value));
        case BCMFP_QUAL_VXLT_LOOKUP_HIT:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_vxlt_lookup_hit_get(unit,
                                                                       data,
                                                                       mask,
                                                                       value));
        case BCMFP_QUAL_TUNNEL_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_tunnel_type_get(unit,
                                                                   data,
                                                                   mask,
                                                                   value));
        case BCMFP_QUAL_LOOPBACK_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_loopback_type_get(unit,
                                                                    data,
                                                                    mask,
                                                                    value));
        case BCMFP_QUAL_PKT_RES:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_pkt_res_get(unit,
                                                               data,
                                                               mask,
                                                               value));

        case BCMFP_QUAL_MPLS_FORWARDING_LABEL_ACTION:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_ingress_qual_mpls_fwd_lbl_action_get(unit,
                                                                           data,
                                                                           mask,
                                                                           value));
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
