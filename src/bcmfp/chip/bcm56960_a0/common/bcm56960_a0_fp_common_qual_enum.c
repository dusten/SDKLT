/*! \file bcm56960_a0_fp_common_qual_enum.c
 *
 * Some qualifiers in FP are exposed as enumerations to end user.
 * User provides one of the many enumerated values supported for
 * those qualifers and internally the SDK converts that enumerated
 * value  to data and mask to be configured in h/w. This file contains
 * data and mask set/get functions for all such qualifiers for common to
 * all the three stages in Tomahawk(56960_A0).
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
 * Function: bcmfp_bcm56960_a0_common_qual_ip_type_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_L3_TYPE from the given enum value.
 *
 * Parameters:
 *     unit      - BCM unit number
 *     type      - (IN) Ip type
 *     *data     - (OUT) Qualifier data part
 *     *mask     - (OUT) Qualifier mask part
 * Returns:
 *     SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_ip_type_set(int unit,
                                              int type,
                                              uint32_t *data,
                                              uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch (type) {
        case BCMFP_IPTYPE_ANY:
            *data = 0x0;
            *mask = 0x0;
            break;
        case BCMFP_IPTYPE_NONIP:
            *data = 0x1f;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_IP:
            *data = 0x0;
            *mask = 0x8;
            break;
        case BCMFP_IPTYPE_IPV4NOOPTS:
            *data = 0x0;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_IPV4WITHOPTS:
            *data = 0x1;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_IPV4ANY:
            *data = 0x0;
            *mask = 0xe;
            break;
        case BCMFP_IPTYPE_IPV6NOEXTHDR:
            *data = 0x4;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_IPV6ONEEXTHDR:
            *data = 0x5;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_IPV6TWOEXTHDR:
            *data = 0x6;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_IPV6_ANY:
            *data = 0x4;
            *mask = 0xc;
            break;
        case BCMFP_IPTYPE_ARP:
            *data = 0x8;
            *mask = 0xe;
            break;
        case BCMFP_IPTYPE_ARPREQUEST:
            *data = 0x8;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_ARPREPLY:
            *data = 0x9;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_TRILL:
            *data = 0xa;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_FCOE:
            *data = 0xb;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_MPLSUNICAST:
            *data = 0xc;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_MPLSMULTICAST:
            *data = 0xd;
            *mask = 0x1f;
            break;
        case BCMFP_IPTYPE_MIM:
            *data = 0xe;
            *mask = 0x1f;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmfp_bcm56960_a0_common_qual_ip_type_get
 * Purpose:
 *     To get the enum value for BCMFP_QUAL_L3_TYPE from the
 *     given data and mask.
 *
 * Parameters:
 *     unit  - (IN) BCM device number
 *     data  - (IN) Qualifier data part
 *     mask  - (IN) Qualifier mask part
 *     *type  - (OUT) Ip Type
 * Returns:
 *     SHR_E_XXX
 * Notes:
 */

int bcmfp_bcm56960_a0_common_qual_ip_type_get(int unit,
                                              uint32_t data,
                                              uint32_t mask,
                                              int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if ((0 == data) && (0 == mask)) {
        *type = BCMFP_IPTYPE_ANY;
    } else if ((0x1f == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_NONIP;
    } else if ((0x0 == data) && (0x8 == mask)) {
        *type = BCMFP_IPTYPE_IP;
    } else if ((0x0 == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_IPV4NOOPTS;
    } else if ((0x1 == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_IPV4WITHOPTS;
    } else if ((0x0 == data) && (0xe == mask)) {
        *type = BCMFP_IPTYPE_IPV4ANY;
    } else if ((0x4 == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_IPV6NOEXTHDR;
    } else if ((0x5 == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_IPV6ONEEXTHDR;
    } else if ((0x6 == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_IPV6TWOEXTHDR;
    } else if ((0x4 == data) && (0xc == mask)) {
        *type = BCMFP_IPTYPE_IPV6_ANY;
    } else if ((0x8 == data) && (0xe == mask)) {
        *type = BCMFP_IPTYPE_ARP;
    } else if ((0x8 == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_ARPREQUEST;
    } else if ((0x9 == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_ARPREPLY;
    } else if ((0xa == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_TRILL;
    } else if ((0xb == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_FCOE;
    } else if ((0xc == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_MPLSUNICAST;
    } else if ((0xd == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_MPLSMULTICAST;
    } else if ((0xe == data) && (0x1f == mask)) {
        *type = BCMFP_IPTYPE_MIM;
    } else
    {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *Function: bcmfp_bcm56960_a0_common_qual_l2format_set
 *
 *Purpose:
 *     To get the data/mask for BCMFP_QUAL_L2_FORMAT from the given enum value.
 *
 *Parameters:
 *    unit  - (IN) BCM unit number
 *    type  - (IN) Ethernet frame type
 *    *data - (OUT) Qualifier data part
 *    *mask - (OUT) Qualifier mask part
 *
 *Returns:
 *    SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_l2format_set(int unit,
                                               int type,
                                               uint32_t *data,
                                               uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    if (type >= BCMFP_L2_FORMAT_COUNT) {
        return (SHR_E_PARAM);
    }

    switch (type) {
        case BCMFP_L2_FORMAT_ANY:
            *data = 0x0;
            *mask = 0x0;
            break;
        case BCMFP_L2_FORMAT_ETHII:
            *data = 0x0;
            *mask = 0x3;
            break;
        case BCMFP_L2_FORMAT_SNAP:
            *data = 0x1;
            *mask = 0x3;
            break;
        case BCMFP_L2_FORMAT_LLC:
        case BCMFP_L2_FORMAT_802DOT3:
            *data = 0x2;
            *mask = 0x7;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *Function: bcmfp_bcm56960_a0_common_qual_l2format_get
 *
 *Purpose:
 *    To get the enum value for BCMFP_QUAL_L2_FORMAT from the
 *    given data and mask.
 *
 *Parameters:
 *    unit - (IN) BCM unit number
 *    data - (IN) Qualifier data part
 *    mask - (IN) Qualifier mask part
 *    type - (OUT) Ethernet frame type
 *
 *Returns:
 *    SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_l2format_get(int unit,
                                               uint32_t data,
                                               uint32_t mask,
                                               int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if ((0 == data) && (mask == 0)) {
        *type = BCMFP_L2_FORMAT_ANY;
    } else if ((0 == data) && (mask == 3)) {
        *type = BCMFP_L2_FORMAT_ETHII;
    } else if ((1 == data) && (mask == 3)) {
        *type = BCMFP_L2_FORMAT_SNAP;
    } else if ((2 == data) && (mask == 7)) {
        *type = BCMFP_L2_FORMAT_LLC;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 *Function: bcmfp_bcm56960_a0_common_qual_innertpid_set
 *
 *Purpose:
 *    To get the enum value for BCMFP_QUAL_INNER_TPID from the
 *    given data and mask.
 *
 *Parameters:
 *    unit  - (IN) BCM unit number
 *    tpid  - (IN) Inner TPID
 *    *data - (OUT) Qualifier data part
 *    *mask - (OUT) Qualifier mask part
 *
 *Returns:
 *    SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_innertpid_set(int unit,
                                                int tpid,
                                                uint32_t *data,
                                                uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    *mask = 0x03;
    switch (tpid) {
        case BCMFP_TPID_8100:
            *data = 0x00;
            break;
        case BCMFP_TPID_9100:
            *data = 0x01;
            break;
        case BCMFP_TPID_88A8:
            *data = 0x02;
            break;
        default:
            *data = 0x03;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*
 *Function: bcmfp_bcm56960_a0_common_qual_innertpid_get
 *
 *Purpose:
 *    To get the enum value for BCMFP_QUAL_INNER_TPID from the
 *    given data and mask.
 *
 *Parameters:
 *    unit - (IN) BCM unit number
 *    data - (IN) Qualifier data part
 *    mask - (IN) Qualifier mask part
 *    tpid - (OUT) Inner TPID
 *
 *Returns:
 *    SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_innertpid_get(int unit,
                                                uint32_t data,
                                                uint32_t mask,
                                                int *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    switch (data) {
      case 0x0:
          *tpid = BCMFP_TPID_8100;
          break;
      case 0x1:
          *tpid = BCMFP_TPID_9100;
          break;
      case 0x2:
          *tpid = BCMFP_TPID_88A8;
          break;
      default:
          SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 *Function: bcmfp_bcm56960_a0_common_qual_outertpid_set
 *
 *Purpose:
 *     To get the data/mask for BCMFP_QUAL_OUTER_TPID from the given enum value.
 *
 *Parameters:
 *    unit  - (IN) BCM unit number
 *    tpid  - (IN) Outer TPID
 *    *data - (OUT) Qualifier data part
 *    *mask - (OUT) Qualifier mask part
 *
 *Returns:
 *    SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_outertpid_set(int unit,
                                                int tpid,
                                                uint32_t *data,
                                                uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    *mask = 0x03;
    switch (tpid) {
        case BCMFP_TPID_8100:
            *data = 0x00;
            break;
        case BCMFP_TPID_9100:
            *data = 0x01;
            break;
        case BCMFP_TPID_88A8:
            *data = 0x02;
            break;
        default:
            *data = 0x03;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*
 *Function: bcmfp_bcm56960_a0_common_qual_outertpid_get
 *
 *Purpose:
 *    To get the enum value for FP_QUAL_OUTER_TPID from the
 *    given data and mask.
 *
 *Parameters:
 *    unit - (IN) BCM unit number
 *    data - (IN) Qualifier data part
 *    mask - (IN) Qualifier mask part
 *    *tpid - (OUT) Outer PID
 *
 *Returns:
 *    SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_outertpid_get(int unit,
                                                uint32_t data,
                                                uint32_t mask,
                                                int *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    switch (data) {
      case 0x0:
          *tpid = BCMFP_TPID_8100;
          break;
      case 0x1:
          *tpid = BCMFP_TPID_9100;
          break;
      case 0x2:
          *tpid = BCMFP_TPID_88A8;
          break;
      default:
          SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_common_qual_ipfrag_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_IP_FRAG from
 *     the given enum value.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     frag_info - (IN)IP Fragment Info
 *     *data - (OUT) Qualifier data part
 *     *mask - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_ipfrag_set(int unit,
                                             int frag_info,
                                             uint32_t *data,
                                             uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch(frag_info) {
        case BCMFP_IPFRAG_ANY:
            /* Any fragment of fragmented packet */
            *data = 0x00;
            *mask = 0x02;
            break;
        case BCMFP_IPFRAG_NON:
            /* Non-fragmented packet */
            *data = 0x02;
            *mask = 0x02;
            break;
        case BCMFP_IPFRAG_FIRST:
            /* First fragment of fragmented packet*/
            *data = 0x01;
            *mask = 0x03;
            break;
        case BCMFP_IPFRAG_NONORFIRST:
            /* Non-fragmented or first fragment */
            *data = 0x01;
            *mask = 0x01;
            break;
        case BCMFP_IPFRAG_NOTFIRST:
            /* Not the first fragment */
            *data = 0x00;
            *mask = 0x03;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Function: bcmfp_bcm56960_a0_common_qual_ipfrag_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_IP_FRAG from the
 *    given data and mask.
 *
 * Parameters:
 *     unit      - (IN) BCM unit number
 *     data      - (IN) Qualifier data part
 *     mask      - (IN) Qualifier mask part
 *     *frag_info - (OUT)IP Fragment Info
 *
 * Returns:
 *     SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_ipfrag_get(int unit,
                                             uint32_t data,
                                             uint32_t mask,
                                             int *frag_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(frag_info, SHR_E_PARAM);

    if ((data == 0x2) && (mask == 0x2)) {
        *frag_info = BCMFP_IPFRAG_NON;
    } else  if ((data == 0x1) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_FIRST;
    } else  if ((data == 0x1) && (mask == 0x1)) {
        *frag_info = BCMFP_IPFRAG_NONORFIRST;
    } else  if ((data == 0x0) && (mask == 0x3)) {
        *frag_info = BCMFP_IPFRAG_NOTFIRST;
    } else  if ((data == 0x0) && (mask == 0x2)) {
        *frag_info = BCMFP_IPFRAG_ANY;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *       bcmfp_bcm56960_a0_commom_qual_color_set
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_COLOR from the given enum value.
 *
 * Parameters:
 *      unit  - (IN) Unit number.
 *      color - (IN) Qualifier match color
 *      *data - (OUT) Qualifier data part
 *      *mask - (OUT) Qualifier mask part
 * Returns:
 *      SHR_E_XXX
 * Notes:
 */
int bcmfp_bcm56960_a0_common_qual_color_set(int unit,
                                            int color,
                                            uint32_t *data,
                                            uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    *mask = 0x3;
    switch (color) {
        case BCMFP_COLOR_GREEN:
            *data = 0;
            break;
        case BCMFP_COLOR_RED:
            *data = 1;
            break;
        case BCMFP_COLOR_YELLOW:
            *data = 3;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *       bcmfp_bcm56960_a0_common_qual_color_get
 * Purpose:
 *     To get the enum value for BCMFP_QUAL_COLOR from the
 *     given data and mask.
 *
 * Parameters:
 *      unit - (IN) Unit number
 *      data - (IN) Qualifier data part
 *      mask - (IN) Qualifier mask part
 *      *color - (OUT) Qualifier match color
 * Returns:
 *      SHR_E_XXX
 * Notes:
 */
int bcmfp_bcm56960_a0_common_qual_color_get(int unit,
                                            uint32_t data,
                                            uint32_t mask,
                                            int *color)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(color, SHR_E_PARAM);

    switch (data) {
      case 0:
          *color = BCMFP_COLOR_GREEN;
          break;
      case 1:
          *color = BCMFP_COLOR_RED;
          break;
      case 3:
          *color = BCMFP_COLOR_YELLOW;
          break;
      default:
          SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_common_qual_forwardingtype_set
 *
 * Purpose:
 *     To get the data/mask for BCMFP_QUAL_FWD_TYPE from
 *     the given enum value.
 *
 * Parameters:
 *     unit  - (IN) BCM unit number
 *     type  - (IN) Forwarding type
 *     *data - (OUT) Qualifier data part
 *     *mask - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_forwardingtype_set(int unit,
                                                     int type,
                                                     uint32_t *data,
                                                     uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    if (type >= BCMFP_FORWARDING_TYPE_COUNT) {
        return (SHR_E_PARAM);
    }

    switch (type) {
        case BCMFP_FORWARDING_TYPE_ANY:
            *data = 0;
            *mask = 0;
            break;
        case BCMFP_FORWARDING_TYPE_L2:
            *data = 0;
            *mask = 0xE;
            break;
        case BCMFP_FORWARDING_TYPE_L2INDEPENDENT:
            *data = 0;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_L2SHARED:
            *data = 1;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_L2VPN:
            *data = 2;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_L2VPNDIRECT:
            *data = 3;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_L3DIRECT:
            *data = 4;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_L3:
            *data = 5;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_PORTEXTENDER:
            *data = 6;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_MPLS:
            *data = 7;
            *mask = 0xF;
            break;
        case BCMFP_FORWARDING_TYPE_TRILL:
            *data = 8;
            *mask = 0xF;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_common_qual_forwardingtype_get
 *
 * Purpose:
 *    To get the enum value for FP_QUAL_FORWARDING_TYPE from the
 *    given data and mask.
 *
 * Parameters:
 *     unit - (IN) BCM unit number
 *     data - (IN) Qualifier data part
 *     mask - (IN) Qualifier mask part
 *     *type - (OUT) Forwarding type
 *
 * Returns:
 *     SHR_E_XXX
 */
int bcmfp_bcm56960_a0_common_qual_forwardingtype_get(int unit,
                                                     uint32_t data,
                                                     uint32_t mask,
                                                     int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    if ((0 == data) && (mask == 0)) {
        *type = BCMFP_FORWARDING_TYPE_ANY;
    } else if ((0 == data) && (mask == 0xE)) {
        *type = BCMFP_FORWARDING_TYPE_L2;
    } else if ((0 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_L2INDEPENDENT;
    } else if ((1 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_L2SHARED;
    } else if ((2 == data) && (mask == 0xF)) {
        *type =  BCMFP_FORWARDING_TYPE_L2VPN;
    } else if ((3 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_L2VPNDIRECT;
    } else if ((4 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_L3DIRECT;
    } else if ((5 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_L3;
    } else if ((6 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_PORTEXTENDER;
    } else if ((7 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_MPLS;
    } else if ((8 == data) && (mask == 0xF)) {
        *type = BCMFP_FORWARDING_TYPE_TRILL;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
