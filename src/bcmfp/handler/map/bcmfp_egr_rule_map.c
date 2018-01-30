/*! \file bcmfp_egr_rule_map.c
 *
 * APIs for Egress Rule LT fields to internal value mapping
 *
 * This file contains function definitions for Ingress Rule LT template
 * fields to qualifier mapping.
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
  Includes
 */
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/map/bcmfp_lt_rule_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*! Ingress FP Rule Field Identifiers that needs individual handling. */
bcmfp_lt_rule_sp_fids_t bcmfp_egr_rule_sp_fids = {
    .rule_id_fid = FP_EGR_RULE_TEMPLATEt_FP_EGR_RULE_TEMPLATE_IDf,
};

/*******************************************************************************
 * Public functions
 */
int
bcmfp_egr_rule_init(int unit,
                    bcmfp_lt_info_t *lt_info)
{
    SHR_FUNC_ENTER(unit)

    lt_info->flags |= BCMFP_LT_FLAGS_INMEM_SUPPORT;
    lt_info->map_get = bcmfp_egr_rule_qual_map_get;
    lt_info->key_fid = FP_EGR_RULE_TEMPLATEt_FP_EGR_RULE_TEMPLATE_IDf;
    lt_info->sp_fids = (void *) &bcmfp_egr_rule_sp_fids;
    lt_info->comp_id = BCMMGMT_FP_COMP_ID;
    lt_info->sub_id = BCMFP_LT_IMM_SUBID_EGR_RULE_TEMPLATE;
    lt_info->key_sz = BCMFP_LT_IMM_KEY_SZ_4BYTE;
    lt_info->fdata_format = BCMFP_LT_IMM_FDATA_FORMAT_I1D3;
    lt_info->rows = 1024;

    SHR_FUNC_EXIT();
}

/* Mapping of LT field to Qualifier */
int
bcmfp_egr_rule_qual_map_get(int unit,
                            uint32_t fid,
                            uint32_t idx,
                            void *map)
{
    bcmfp_qual_t qual;
    bcmfp_lt_rule_field_type_t field_type;
    bcmfp_lt_rule_qual_map_t *rule_map = NULL;
    SHR_FUNC_ENTER(unit);

    rule_map = (bcmfp_lt_rule_qual_map_t *) map;

    switch(fid) {
        case FP_EGR_RULE_TEMPLATEt_QUAL_EGR_NHOP_CLASS_IDf:
            qual = BCMFP_QUAL_EGR_NHOP_CLASS_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_EGR_NHOP_CLASS_ID_MASKf:
            qual = BCMFP_QUAL_EGR_NHOP_CLASS_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_EGR_L3_INTF_CLASS_IDf:
            qual = BCMFP_QUAL_EGR_L3_INTF_CLASS_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_EGR_L3_INTF_CLASS_ID_MASKf:
            qual = BCMFP_QUAL_EGR_L3_INTF_CLASS_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_EGR_DVP_CLASS_IDf:
            qual = BCMFP_QUAL_EGR_DVP_CLASS_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_EGR_DVP_CLASS_ID_MASKf:
            qual = BCMFP_QUAL_EGR_DVP_CLASS_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L4_PKTf:
            qual = BCMFP_QUAL_L4_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L4_PKT_MASKf:
            qual = BCMFP_QUAL_L4_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_VP_VALIDf:
            qual = BCMFP_QUAL_DST_VP_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_VP_VALID_MASKf:
            qual = BCMFP_QUAL_DST_VP_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_VPf:
            qual = BCMFP_QUAL_DST_VP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_VP_MASKf:
            qual = BCMFP_QUAL_DST_VP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INT_PRIf:
            qual = BCMFP_QUAL_INT_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INT_PRI_MASKf:
            qual = BCMFP_QUAL_INT_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_COLORf:
            qual = BCMFP_QUAL_COLOR;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L2_FORMATf:
            qual = BCMFP_QUAL_L2_FORMAT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_ETHERTYPEf:
            qual = BCMFP_QUAL_ETHERTYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_ETHERTYPE_MASKf:
            qual = BCMFP_QUAL_ETHERTYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_MACf:
            qual = BCMFP_QUAL_SRC_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_MAC_MASKf:
            qual = BCMFP_QUAL_SRC_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_MACf:
            qual = BCMFP_QUAL_DST_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_MAC_MASKf:
            qual = BCMFP_QUAL_DST_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VXLT_LOOKUP_HITf:
            qual = BCMFP_QUAL_VXLT_LOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VXLT_LOOKUP_HIT_MASKf:
            qual = BCMFP_QUAL_VXLT_LOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INNER_VLAN_CFIf:
            qual = BCMFP_QUAL_INNER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INNER_VLAN_CFI_MASKf:
            qual = BCMFP_QUAL_INNER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INNER_VLAN_PRIf:
            qual = BCMFP_QUAL_INNER_VLAN_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INNER_VLAN_PRI_MASKf:
            qual = BCMFP_QUAL_INNER_VLAN_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INNER_VLAN_IDf:
            qual = BCMFP_QUAL_INNER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INNER_VLAN_ID_MASKf:
            qual = BCMFP_QUAL_INNER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INPORTf:
            qual = BCMFP_QUAL_INPORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INPORT_MASKf:
            qual = BCMFP_QUAL_INPORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKTf:
            qual = BCMFP_QUAL_L3_ROUTABLE_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_MASKf:
            qual = BCMFP_QUAL_L3_ROUTABLE_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_IDf:
            qual = BCMFP_QUAL_OUTER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_ID_MASKf:
            qual = BCMFP_QUAL_OUTER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFIf:
            qual = BCMFP_QUAL_OUTER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFI_MASKf:
            qual = BCMFP_QUAL_OUTER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRIf:
            qual = BCMFP_QUAL_OUTER_VLAN_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRI_MASKf:
            qual = BCMFP_QUAL_OUTER_VLAN_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VLAN_INNER_PRESENTf:
            qual = BCMFP_QUAL_VLAN_INNER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VLAN_INNER_PRESENT_MASKf:
            qual = BCMFP_QUAL_VLAN_INNER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf:
            qual = BCMFP_QUAL_VLAN_OUTER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_MASKf:
            qual = BCMFP_QUAL_VLAN_OUTER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SYSTEM_PORT_CLASSf:
            qual = BCMFP_QUAL_SYSTEM_PORT_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SYSTEM_PORT_CLASS_MASKf:
            qual = BCMFP_QUAL_SYSTEM_PORT_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_FP_ING_CLASSID_TYPEf:
            qual = BCMFP_QUAL_FP_ING_CLASSID_TYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_FP_ING_CLASSIDf:
            qual = BCMFP_QUAL_FP_ING_CLASSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_FP_ING_CLASSID_MASKf:
            qual = BCMFP_QUAL_FP_ING_CLASSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTPORTf:
            qual = BCMFP_QUAL_OUTPORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_OUTPORT_MASKf:
            qual = BCMFP_QUAL_OUTPORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_CPU_COSf:
            qual = BCMFP_QUAL_CPU_COS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_CPU_COS_MASKf:
            qual = BCMFP_QUAL_CPU_COS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L3_TYPEf:
            qual = BCMFP_QUAL_L3_TYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_FWD_VLAN_IDf:
            qual = BCMFP_QUAL_FWD_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_FWD_VLAN_ID_MASKf:
            qual = BCMFP_QUAL_FWD_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VRFf:
            qual = BCMFP_QUAL_VRF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VRF_MASKf:
            qual = BCMFP_QUAL_VRF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VPNf:
            qual = BCMFP_QUAL_VPN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_VPN_MASKf:
            qual = BCMFP_QUAL_VPN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_FORWARDING_TYPEf:
            qual = BCMFP_QUAL_FWD_TYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INT_CNf:
            qual = BCMFP_QUAL_INT_CN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_INT_CN_MASKf:
            qual = BCMFP_QUAL_INT_CN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DROP_PKTf:
            qual = BCMFP_QUAL_DROP_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DROP_PKT_MASKf:
            qual = BCMFP_QUAL_DROP_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_FLAGS_MFf:
            qual = BCMFP_QUAL_IP_FLAGS_MF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_FLAGS_MF_MASKf:
            qual = BCMFP_QUAL_IP_FLAGS_MF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_MIRR_COPYf:
            qual = BCMFP_QUAL_MIRR_COPY;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_MIRR_COPY_MASKf:
            qual = BCMFP_QUAL_MIRR_COPY;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_TCP_CONTROL_FLAGSf:
            qual = BCMFP_QUAL_TCP_CONTROL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_TCP_CONTROL_FLAGS_MASKf:
            qual = BCMFP_QUAL_TCP_CONTROL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L4DST_PORTf:
            qual = BCMFP_QUAL_L4DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L4DST_PORT_MASKf:
            qual = BCMFP_QUAL_L4DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L4SRC_PORTf:
            qual = BCMFP_QUAL_L4SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_L4SRC_PORT_MASKf:
            qual = BCMFP_QUAL_L4SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_ICMP_TYPE_CODEf:
            qual = BCMFP_QUAL_ICMP_TYPE_CODE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_ICMP_TYPE_CODE_MASKf:
            qual = BCMFP_QUAL_ICMP_TYPE_CODE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_TTLf:
            qual = BCMFP_QUAL_TTL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_TTL_MASKf:
            qual = BCMFP_QUAL_TTL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf:
            qual = BCMFP_QUAL_IP_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_PROTOCOL_MASKf:
            qual = BCMFP_QUAL_IP_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP4f:
            qual = BCMFP_QUAL_DST_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP4_MASKf:
            qual = BCMFP_QUAL_DST_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP4f:
            qual = BCMFP_QUAL_SRC_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP4_MASKf:
            qual = BCMFP_QUAL_SRC_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_TOSf:
            qual = BCMFP_QUAL_TOS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_TOS_MASKf:
            qual = BCMFP_QUAL_TOS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6u_UPPERf:
            qual = BCMFP_QUAL_SRC_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6u_LOWERf:
            qual = BCMFP_QUAL_SRC_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_UPPERf:
            qual = BCMFP_QUAL_SRC_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_LOWERf:
            qual = BCMFP_QUAL_SRC_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6u_UPPERf:
            qual = BCMFP_QUAL_DST_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6u_LOWERf:
            qual = BCMFP_QUAL_DST_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_UPPERf:
            qual = BCMFP_QUAL_DST_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_LOWERf:
            qual = BCMFP_QUAL_DST_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_HIGHf:
            qual = BCMFP_QUAL_SRC_IP6_HIGH;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_HIGH_MASKf:
            qual = BCMFP_QUAL_SRC_IP6_HIGH;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_HIGHf:
            qual = BCMFP_QUAL_DST_IP6_HIGH;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_HIGH_MASKf:
            qual = BCMFP_QUAL_DST_IP6_HIGH;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf:
            qual = BCMFP_QUAL_IP_FIRST_EH_SUBCODE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_MASKf:
            qual = BCMFP_QUAL_IP_FIRST_EH_SUBCODE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf:
            qual = BCMFP_QUAL_IP_FIRST_EH_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_MASKf:
            qual = BCMFP_QUAL_IP_FIRST_EH_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_EXP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXP_MASKf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_EXP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_PKT_IS_VISIBLEf:
            qual = BCMFP_QUAL_PKT_IS_VISIBLE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_PKT_IS_VISIBLE_MASKf:
            qual = BCMFP_QUAL_PKT_IS_VISIBLE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_AFTER_L2HEADERf:
            qual = BCMFP_QUAL_BYTES_AFTER_L2HEADER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_AFTER_L2HEADER_MASKf:
            qual = BCMFP_QUAL_BYTES_AFTER_L2HEADER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    rule_map->fid = fid;
    rule_map->qual = qual;
    rule_map->field_type = field_type;

exit:

    SHR_FUNC_EXIT();
}
