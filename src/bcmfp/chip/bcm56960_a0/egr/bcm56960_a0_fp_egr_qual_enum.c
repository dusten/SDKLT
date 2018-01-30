/*! \file bcm56960_a0_fp_egr_qual_enum.c
 *
 * Some qualifiers in EFP are exposed as enumerations to end user.
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
 * Function: bcmfp_bcm56960_a0_egress_qual_ifp_class_id_type_get
 *
 * Purpose:
 *    To get the enum value for BCMFP_QUAL_FP_ING_CLASSID_TYPE from the given data
 *    and mask.
 *
 * Parameters:
 *     unit           - (IN) BCM unit number
 *     data           - (IN) Qualifier data part
 *     mask           - (IN) Qualifier mask part
 *     *class_id_type - (OUT)
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_egress_qual_ifp_class_id_type_get(int unit,
                                                        uint32_t data,
                                                        uint32_t mask,
                                                        int *class_id_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(class_id_type, SHR_E_PARAM);

    if ((data == 0x1) && (mask == 0xF)) {
        *class_id_type = BCMFP_ING_INTF_PORT_CLASSID;
    } else  if ((data == 0x2) && (mask == 0xF)) {
        *class_id_type = BCMFP_ING_INTF_VPORT_CLASSID;
    } else  if ((data == 0x3) && (mask == 0xF)) {
        *class_id_type = BCMFP_L3_INTF_CLASS;
    } else  if ((data == 0x4) && (mask == 0xF)) {
        *class_id_type = BCMFP_FP_VLAN_CLASS1;
    } else  if ((data == 0x5) && (mask == 0xF)) {
        *class_id_type = BCMFP_FP_VLAN_CLASS0;
    } else  if ((data == 0x6) && (mask == 0xF)) {
        *class_id_type = BCMFP_L2SRC_CLASS;
    } else  if ((data == 0x7) && (mask == 0xF)) {
        *class_id_type = BCMFP_L2DEST_CLASS;
    } else  if ((data == 0x8) && (mask == 0xF)) {
        *class_id_type = BCMFP_L3SRC_CLASS;
    } else  if ((data == 0x9) && (mask == 0xF)) {
        *class_id_type = BCMFP_L3DEST_CLASS;
    } else  if ((data == 0xA) && (mask == 0xF)) {
        *class_id_type = BCMFP_VLAN_CLASS;
    } else  if ((data == 0xF) && (mask == 0xF)) {
        *class_id_type = BCMFP_ING_CLASSID;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmfp_bcm56960_a0_egress_qual_ifp_class_id_type_set
 *
 * Purpose:
 *    To get the data/mask value for BCMFP_QUAL_FP_ING_CLASSID_TYPE from the given
 *    enum.
 * Parameters:
 *     unit              - (IN) BCM unit number
 *     class_id_type     - (IN) lookup hit
 *     *data             - (OUT) Qualifier data part
 *     *mask             - (OUT) Qualifier mask part
 *
 * Returns:
 *     SHR_E_XXX
 */
STATIC
int bcmfp_bcm56960_a0_egress_qual_ifp_class_id_type_set(int unit,
                                                    int class_id_type,
                                                    uint32_t *data,
                                                    uint32_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mask, SHR_E_INTERNAL);

    switch(class_id_type) {
        case BCMFP_ING_INTF_PORT_CLASSID:
            *data = 0x1;
            *mask = 0xF;
            break;
        case BCMFP_ING_INTF_VPORT_CLASSID:
            *data = 0x2;
            *mask = 0xF;
            break;
        case BCMFP_L3_INTF_CLASS:
            *data = 0x3;
            *mask = 0xF;
            break;
        case BCMFP_FP_VLAN_CLASS1:
            *data = 0x4;
            *mask = 0xF;
            break;
        case BCMFP_FP_VLAN_CLASS0:
            *data = 0x5;
            *mask = 0xF;
            break;
        case BCMFP_L2SRC_CLASS:
            *data = 0x6;
            *mask = 0xF;
            break;
        case BCMFP_L2DEST_CLASS:
            *data = 0x7;
            *mask = 0xF;
            break;
        case BCMFP_L3SRC_CLASS:
            *data = 0x08;
            *mask = 0xF;
            break;
        case BCMFP_L3DEST_CLASS:
            *data = 0x09;
            *mask = 0xF;
            break;
        case BCMFP_VLAN_CLASS:
            *data = 0xA;
            *mask = 0xF;
            break;
        case BCMFP_ING_CLASSID:
            *data = 0xF;
            *mask = 0xF;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
/*
 * Function:  bcmfp_bcm56960_a0_egress_qual_enum_set
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
bcmfp_bcm56960_a0_egress_qual_enum_set(int unit,
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
        case BCMFP_QUAL_FWD_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_forwardingtype_set(unit,
                                                                     value,
                                                                     data,
                                                                     mask));
        case BCMFP_QUAL_FP_ING_CLASSID_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_egress_qual_ifp_class_id_type_set(unit,
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
 * Function:  bcmfp_bcm56960_a0_egress_qual_enum_get
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
bcmfp_bcm56960_a0_egress_qual_enum_get(int unit,
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
        case BCMFP_QUAL_FWD_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_common_qual_forwardingtype_get(unit,
                                                                     data,
                                                                     mask,
                                                                     value));
        case BCMFP_QUAL_FP_ING_CLASSID_TYPE:
            SHR_RETURN_VAL_EXIT(
                    bcmfp_bcm56960_a0_egress_qual_ifp_class_id_type_get(unit,
                                                                       data,
                                                                       mask,
                                                                       value));
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
