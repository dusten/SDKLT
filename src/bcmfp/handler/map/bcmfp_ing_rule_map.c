/*! \file bcmfp_ing_rule_map.c
 *
 * APIs for Ingress Rule LT fields to internal value mapping
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
bcmfp_lt_rule_sp_fids_t bcmfp_ing_rule_sp_fids = {
    .rule_id_fid = FP_ING_RULE_TEMPLATEt_FP_ING_RULE_TEMPLATE_IDf,
    .qual_inports_fid = FP_ING_RULE_TEMPLATEt_QUAL_INPORTSf,
};

/*******************************************************************************
 * Public functions
 */
int
bcmfp_ing_rule_init(int unit,
                    bcmfp_lt_info_t *lt_info)
{
    SHR_FUNC_ENTER(unit)

    lt_info->flags |= BCMFP_LT_FLAGS_INMEM_SUPPORT;
    lt_info->map_get = bcmfp_ing_rule_qual_map_get;
    lt_info->key_fid = FP_ING_RULE_TEMPLATEt_FP_ING_RULE_TEMPLATE_IDf;
    lt_info->sp_fids = (void *) &bcmfp_ing_rule_sp_fids;

    lt_info->comp_id = BCMMGMT_FP_COMP_ID;
    lt_info->sub_id = BCMFP_LT_IMM_SUBID_ING_RULE_TEMPLATE;
    lt_info->key_sz = BCMFP_LT_IMM_KEY_SZ_4BYTE;
    lt_info->fdata_format = BCMFP_LT_IMM_FDATA_FORMAT_I1D3;
    lt_info->rows = 6144;

    SHR_FUNC_EXIT();
}

/* Mapping of LT field to Qualifier */
int
bcmfp_ing_rule_qual_map_get(int unit,
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
        case FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_IDf:
            qual = BCMFP_QUAL_OUTER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_ID_MASKf:
            qual = BCMFP_QUAL_OUTER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRIf:
            qual = BCMFP_QUAL_OUTER_VLAN_PRIORITY;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRI_MASKf:
            qual = BCMFP_QUAL_OUTER_VLAN_PRIORITY;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFIf:
            qual = BCMFP_QUAL_OUTER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFI_MASKf:
            qual = BCMFP_QUAL_OUTER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INNER_VLAN_IDf:
            qual = BCMFP_QUAL_INNER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INNER_VLAN_ID_MASKf:
            qual = BCMFP_QUAL_INNER_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INNER_VLAN_PRIf:
            qual = BCMFP_QUAL_INNER_VLAN_PRIORITY;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INNER_VLAN_PRI_MASKf:
            qual = BCMFP_QUAL_INNER_VLAN_PRIORITY;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INNER_VLAN_CFIf:
            qual = BCMFP_QUAL_INNER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INNER_VLAN_CFI_MASKf:
            qual = BCMFP_QUAL_INNER_VLAN_CFI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_MACf:
            qual = BCMFP_QUAL_SRC_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_MAC_MASKf:
            qual = BCMFP_QUAL_SRC_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_MACf:
            qual = BCMFP_QUAL_DST_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_MAC_MASKf:
            qual = BCMFP_QUAL_DST_MAC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP4f:
            qual = BCMFP_QUAL_SRC_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP4_MASKf:
            qual = BCMFP_QUAL_SRC_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP4f:
            qual = BCMFP_QUAL_DST_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP4_MASKf:
            qual = BCMFP_QUAL_DST_IP4;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6u_UPPERf:
            qual = BCMFP_QUAL_SRC_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6u_LOWERf:
            qual = BCMFP_QUAL_SRC_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_UPPERf:
            qual = BCMFP_QUAL_SRC_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_LOWERf:
            qual = BCMFP_QUAL_SRC_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6u_UPPERf:
            qual = BCMFP_QUAL_DST_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6u_LOWERf:
            qual = BCMFP_QUAL_DST_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_UPPERf:
            qual = BCMFP_QUAL_DST_IP6_UPPER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_LOWERf:
            qual = BCMFP_QUAL_DST_IP6_LOWER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_ETHERTYPEf:
            qual = BCMFP_QUAL_ETHERTYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_ETHERTYPE_MASKf:
            qual = BCMFP_QUAL_ETHERTYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L4SRC_PORTf:
            qual = BCMFP_QUAL_L4SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L4SRC_PORT_MASKf:
            qual = BCMFP_QUAL_L4SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L4DST_PORTf:
            qual = BCMFP_QUAL_L4DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L4DST_PORT_MASKf:
            qual = BCMFP_QUAL_L4DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MIM_ISIDf:
            qual = BCMFP_QUAL_MIM_ISID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MIM_ISID_MASKf:
            qual = BCMFP_QUAL_MIM_ISID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NVGRE_VSIDf:
            qual = BCMFP_QUAL_NVGRE_VSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NVGRE_VSID_MASKf:
            qual = BCMFP_QUAL_NVGRE_VSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_VNIDf:
            qual = BCMFP_QUAL_VXLAN_VNID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_VNID_MASKf:
            qual = BCMFP_QUAL_VXLAN_VNID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_FLAGSf:
            qual = BCMFP_QUAL_VXLAN_FLAGS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_FLAGS_MASKf:
            qual = BCMFP_QUAL_VXLAN_FLAGS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_24f:
            qual = BCMFP_QUAL_VXLAN_RSVD_24;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_24_MASKf:
            qual = BCMFP_QUAL_VXLAN_RSVD_24;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_8f:
            qual = BCMFP_QUAL_VXLAN_RSVD_8;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_8_MASKf:
            qual = BCMFP_QUAL_VXLAN_RSVD_8;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_ETAGf:
            qual = BCMFP_QUAL_ETAG;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_ETAG_MASKf:
            qual = BCMFP_QUAL_ETAG;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VNTAGf:
            qual = BCMFP_QUAL_VNTAG;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VNTAG_MASKf:
            qual = BCMFP_QUAL_VNTAG;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_CNTAGf:
            qual = BCMFP_QUAL_CNTAG;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_CNTAG_MASKf:
            qual = BCMFP_QUAL_CNTAG;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_IDf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ID_MASKf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTLf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_TTL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTL_MASKf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_TTL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_EXP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXP_MASKf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_EXP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOSf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_BOS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOS_MASKf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_BOS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_CTRL_WORDf:
            qual = BCMFP_QUAL_MPLS_CTRL_WORD;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_CTRL_WORD_MASKf:
            qual = BCMFP_QUAL_MPLS_CTRL_WORD;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP6_FLOW_LABELf:
            qual = BCMFP_QUAL_IP6_FLOW_LABEL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP6_FLOW_LABEL_MASKf:
            qual = BCMFP_QUAL_IP6_FLOW_LABEL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TTLf:
            qual = BCMFP_QUAL_TTL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TTL_MASKf:
            qual = BCMFP_QUAL_TTL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TOSf:
            qual = BCMFP_QUAL_TOS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TOS_MASKf:
            qual = BCMFP_QUAL_TOS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf:
            qual = BCMFP_QUAL_IP_PROTOCOL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOL_MASKf:
            qual = BCMFP_QUAL_IP_PROTOCOL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TCP_CONTROL_FLAGSf:
            qual = BCMFP_QUAL_TCP_CONTROL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TCP_CONTROL_FLAGS_MASKf:
            qual = BCMFP_QUAL_TCP_CONTROL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf:
            qual = BCMFP_QUAL_IP_FIRST_EH_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_MASKf:
            qual = BCMFP_QUAL_IP_FIRST_EH_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_SECOND_EH_PROTOf:
            qual = BCMFP_QUAL_IP_SECOND_EH_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_SECOND_EH_PROTO_MASKf:
            qual = BCMFP_QUAL_IP_SECOND_EH_PROTO;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf:
            qual = BCMFP_QUAL_IP_FIRST_EH_SUBCODE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_MASKf:
            qual = BCMFP_QUAL_IP_FIRST_EH_SUBCODE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_MFf:
            qual = BCMFP_QUAL_IP_FLAGS_MF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_MF_MASKf:
            qual = BCMFP_QUAL_IP_FLAGS_MF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_DFf:
            qual = BCMFP_QUAL_IP_FLAGS_DF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_DF_MASKf:
            qual = BCMFP_QUAL_IP_FLAGS_DF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_FRAGf:
            qual = BCMFP_QUAL_IP_FRAG;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_HIGIG_PKTf:
            qual = BCMFP_QUAL_HIGIG_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_HIGIG_PKT_MASKf:
            qual = BCMFP_QUAL_HIGIG_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_HIGIGLOOKUP_PKTf:
            qual = BCMFP_QUAL_HIGIGLOOKUP_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_HIGIGLOOKUP_PKT_MASKf:
            qual = BCMFP_QUAL_HIGIGLOOKUP_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DROP_PKTf:
            qual = BCMFP_QUAL_DROP_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DROP_PKT_MASKf:
            qual = BCMFP_QUAL_DROP_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VISIBILITY_PKTf:
            qual = BCMFP_QUAL_VISIBILITY_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VISIBILITY_PKT_MASKf:
            qual = BCMFP_QUAL_VISIBILITY_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DOSATTACK_PKTf:
            qual = BCMFP_QUAL_DOSATTACK_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DOSATTACK_PKT_MASKf:
            qual = BCMFP_QUAL_DOSATTACK_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L4_PKTf:
            qual = BCMFP_QUAL_L4_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L4_PKT_MASKf:
            qual = BCMFP_QUAL_L4_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MIRROR_PKTf:
            qual = BCMFP_QUAL_MIRROR_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MIRROR_PKT_MASKf:
            qual = BCMFP_QUAL_MIRROR_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENTf:
            qual = BCMFP_QUAL_NON_FRAGMENT_FIRST_FRAGMENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_ICMP_ERROR_PKTf:
            qual = BCMFP_QUAL_ICMP_ERROR_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_ICMP_ERROR_PKT_MASKf:
            qual = BCMFP_QUAL_ICMP_ERROR_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_REPLICATION_PKTf:
            qual = BCMFP_QUAL_REPLICATION_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_REPLICATION_PKT_MASKf:
            qual = BCMFP_QUAL_REPLICATION_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2SRC_HITf:
            qual = BCMFP_QUAL_L2SRC_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2SRC_HIT_MASKf:
            qual = BCMFP_QUAL_L2SRC_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2DST_HITf:
            qual = BCMFP_QUAL_L2DST_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2DST_HIT_MASKf:
            qual = BCMFP_QUAL_L2DST_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3SRC_HOST_HITf:
            qual = BCMFP_QUAL_L3SRC_HOST_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3SRC_HOST_HIT_MASKf:
            qual = BCMFP_QUAL_L3SRC_HOST_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3DST_HOST_HITf:
            qual = BCMFP_QUAL_L3DST_HOST_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3DST_HOST_HIT_MASKf:
            qual = BCMFP_QUAL_L3DST_HOST_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3DST_LPM_HITf:
            qual = BCMFP_QUAL_L3DST_LPM_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3DST_LPM_HIT_MASKf:
            qual = BCMFP_QUAL_L3DST_LPM_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_TNL_HITf:
            qual = BCMFP_QUAL_L3_TUNNEL_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_TNL_HIT_MASKf:
            qual = BCMFP_QUAL_L3_TUNNEL_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VXLT_LOOKUP_HITf:
            qual = BCMFP_QUAL_VXLT_LOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2CACHE_HITf:
            qual = BCMFP_QUAL_L2CACHE_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2CACHE_HIT_MASKf:
            qual = BCMFP_QUAL_L2CACHE_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf:
            qual = BCMFP_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_MASKf:
            qual = BCMFP_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf:
            qual = BCMFP_QUAL_MPLSENTRY_SECONDLOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_MASKf:
            qual = BCMFP_QUAL_MPLSENTRY_SECONDLOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf:
            qual = BCMFP_QUAL_MYSTATIONTCAM_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_MASKf:
            qual = BCMFP_QUAL_MYSTATIONTCAM_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2SRC_STATICf:
            qual = BCMFP_QUAL_L2SRC_STATIC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2SRC_STATIC_MASKf:
            qual = BCMFP_QUAL_L2SRC_STATIC;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2STATION_MOVEf:
            qual = BCMFP_QUAL_L2STATION_MOVE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2STATION_MOVE_MASKf:
            qual = BCMFP_QUAL_L2STATION_MOVE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_BOS_TERMINATEDf:
            qual = BCMFP_QUAL_MPLS_BOS_TERMINATED;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_BOS_TERMINATED_MASKf:
            qual = BCMFP_QUAL_MPLS_BOS_TERMINATED;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP_LOCALf:
            qual = BCMFP_QUAL_DEST_IP_LOCAL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_IP_LOCAL_MASKf:
            qual = BCMFP_QUAL_DEST_IP_LOCAL;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKTf:
            qual = BCMFP_QUAL_L3_ROUTABLE_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_MASKf:
            qual = BCMFP_QUAL_L3_ROUTABLE_PKT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORT_CLASSf:
            qual = BCMFP_QUAL_SYSTEM_PORT_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORT_CLASS_MASKf:
            qual = BCMFP_QUAL_SYSTEM_PORT_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_INTF_CLASSf:
            qual = BCMFP_QUAL_L3_INTF_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_INTF_CLASS_MASKf:
            qual = BCMFP_QUAL_L3_INTF_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_VP_CLASSf:
            qual = BCMFP_QUAL_SRC_VP_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_VP_CLASS_MASKf:
            qual = BCMFP_QUAL_SRC_VP_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VLAN_CLASSf:
            qual = BCMFP_QUAL_VLAN_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VLAN_CLASS_MASKf:
            qual = BCMFP_QUAL_VLAN_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS0f:
            qual = BCMFP_QUAL_FP_VLAN_CLASS0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS0_MASKf:
            qual = BCMFP_QUAL_FP_VLAN_CLASS0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS1f:
            qual = BCMFP_QUAL_FP_VLAN_CLASS1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS1_MASKf:
            qual = BCMFP_QUAL_FP_VLAN_CLASS1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PORT_FP_VLAN_GROUPIDf:
            qual = BCMFP_QUAL_PORT_FP_VLAN_GROUPID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PORT_FP_VLAN_GROUPID_MASKf:
            qual = BCMFP_QUAL_PORT_FP_VLAN_GROUPID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PORT_FP_ING_KEYSELECT_CLASSf:
            qual = BCMFP_QUAL_PORT_FP_ING_KEYSELECT_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PORT_FP_ING_KEYSELECT_CLASS_MASKf:
            qual = BCMFP_QUAL_PORT_FP_ING_KEYSELECT_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_UDF_CLASSf:
            qual = BCMFP_QUAL_UDF_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_UDF_CLASS_MASKf:
            qual = BCMFP_QUAL_UDF_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2SRC_CLASSf:
            qual = BCMFP_QUAL_L2SRC_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2SRC_CLASS_MASKf:
            qual = BCMFP_QUAL_L2SRC_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2DST_CLASSf:
            qual = BCMFP_QUAL_L2DEST_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2DST_CLASS_MASKf:
            qual = BCMFP_QUAL_L2DEST_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3SRC_CLASSf:
            qual = BCMFP_QUAL_L3SRC_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3SRC_CLASS_MASKf:
            qual = BCMFP_QUAL_L3SRC_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3DST_CLASSf:
            qual = BCMFP_QUAL_L3DEST_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3DST_CLASS_MASKf:
            qual = BCMFP_QUAL_L3DEST_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PRESEL_CLASSf:
            qual = BCMFP_QUAL_PRESEL_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PRESEL_CLASS_MASKf:
            qual = BCMFP_QUAL_PRESEL_CLASS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_0f:
            qual = BCMFP_QUAL_EM_GROUP_CLASS_ID_0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_0_MASKf:
            qual = BCMFP_QUAL_EM_GROUP_CLASS_ID_0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_1f:
            qual = BCMFP_QUAL_EM_GROUP_CLASS_ID_1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_1_MASKf:
            qual = BCMFP_QUAL_EM_GROUP_CLASS_ID_1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_VALIDf:
            qual = BCMFP_QUAL_FWD_VLAN_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_VALID_MASKf:
            qual = BCMFP_QUAL_FWD_VLAN_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_VP_VALIDf:
            qual = BCMFP_QUAL_SRC_VP_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_VP_VALID_MASKf:
            qual = BCMFP_QUAL_SRC_VP_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_VP_VALIDf:
            qual = BCMFP_QUAL_DST_VP_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_VP_VALID_MASKf:
            qual = BCMFP_QUAL_DST_VP_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_MODULEf:
            qual = BCMFP_QUAL_SRC_MODULE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_MODULE_MASKf:
            qual = BCMFP_QUAL_SRC_MODULE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_PORTf:
            qual = BCMFP_QUAL_SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_PORT_MASKf:
            qual = BCMFP_QUAL_SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_TRUNKf:
            qual = BCMFP_QUAL_SRC_TRUNK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_TRUNK_MASKf:
            qual = BCMFP_QUAL_SRC_TRUNK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_VPf:
            qual = BCMFP_QUAL_SRC_VP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SRC_VP_MASKf:
            qual = BCMFP_QUAL_SRC_VP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_MODULEf:
            qual = BCMFP_QUAL_DST_MODULE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_MODULE_MASKf:
            qual = BCMFP_QUAL_DST_MODULE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_PORTf:
            qual = BCMFP_QUAL_DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_PORT_MASKf:
            qual = BCMFP_QUAL_DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_TRUNKf:
            qual = BCMFP_QUAL_DST_TRUNK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_TRUNK_MASKf:
            qual = BCMFP_QUAL_DST_TRUNK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_VP_TRUNKf:
            qual = BCMFP_QUAL_DST_VP_TRUNK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_VP_TRUNK_MASKf:
            qual = BCMFP_QUAL_DST_VP_TRUNK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_VPf:
            qual = BCMFP_QUAL_DST_VP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DST_VP_MASKf:
            qual = BCMFP_QUAL_DST_VP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NHOPf:
            qual = BCMFP_QUAL_DST_NHOP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NHOP_MASKf:
            qual = BCMFP_QUAL_DST_NHOP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OVERLAY_ECMPf:
            qual = BCMFP_QUAL_DST_OVERLAY_ECMP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OVERLAY_ECMP_MASKf:
            qual = BCMFP_QUAL_DST_OVERLAY_ECMP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_UNDERLAY_ECMPf:
            qual = BCMFP_QUAL_DST_UNDERLAY_ECMP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_UNDERLAY_ECMP_MASKf:
            qual = BCMFP_QUAL_DST_UNDERLAY_ECMP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_MC_GROUPf:
            qual = BCMFP_QUAL_DST_L3_MC_GROUP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_MC_GROUP_MASKf:
            qual = BCMFP_QUAL_DST_L3_MC_GROUP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2_MC_GROUPf:
            qual = BCMFP_QUAL_DST_L2_MC_GROUP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2_MC_GROUP_MASKf:
            qual = BCMFP_QUAL_DST_L2_MC_GROUP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_IDf:
            qual = BCMFP_QUAL_FWD_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_ID_MASKf:
            qual = BCMFP_QUAL_FWD_VLAN_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VRFf:
            qual = BCMFP_QUAL_VRF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VRF_MASKf:
            qual = BCMFP_QUAL_VRF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VPNf:
            qual = BCMFP_QUAL_VPN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VPN_MASKf:
            qual = BCMFP_QUAL_VPN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_IIFf:
            qual = BCMFP_QUAL_L3_IIF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_IIF_MASKf:
            qual = BCMFP_QUAL_L3_IIF;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INPORTf:
            qual = BCMFP_QUAL_INPORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INPORT_MASKf:
            qual = BCMFP_QUAL_INPORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INPORTSf:
            qual = BCMFP_QUAL_INPORTS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INPORTS_MASKf:
            qual = BCMFP_QUAL_INPORTS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORTSf:
            qual = BCMFP_QUAL_SYSTEM_PORTS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORTS_MASKf:
            qual = BCMFP_QUAL_SYSTEM_PORTS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DEVICE_PORTSf:
            qual = BCMFP_QUAL_DEVICE_PORTS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_DEVICE_PORTS_MASKf:
            qual = BCMFP_QUAL_DEVICE_PORTS;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INT_PRIf:
            qual = BCMFP_QUAL_INT_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INT_PRI_MASKf:
            qual = BCMFP_QUAL_INT_PRI;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_COLORf:
            qual = BCMFP_QUAL_COLOR;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_RAL_PRESENTf:
            qual = BCMFP_QUAL_RAL_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_RAL_PRESENT_MASKf:
            qual = BCMFP_QUAL_RAL_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_GAL_PRESENTf:
            qual = BCMFP_QUAL_GAL_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_GAL_PRESENT_MASKf:
            qual = BCMFP_QUAL_GAL_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_CW_VALIDf:
            qual = BCMFP_QUAL_MPLS_CW_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_CW_VALID_MASKf:
            qual = BCMFP_QUAL_MPLS_CW_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NAT_NEEDEDf:
            qual = BCMFP_QUAL_NAT_NEEDED;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NAT_NEEDED_MASKf:
            qual = BCMFP_QUAL_NAT_NEEDED;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NAT_SRC_REALM_IDf:
            qual = BCMFP_QUAL_NAT_SRC_REALM_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NAT_SRC_REALM_ID_MASKf:
            qual = BCMFP_QUAL_NAT_SRC_REALM_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NAT_DST_REALM_IDf:
            qual = BCMFP_QUAL_NAT_DST_REALM_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_NAT_DST_REALM_ID_MASKf:
            qual = BCMFP_QUAL_NAT_DST_REALM_ID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INT_CNf:
            qual = BCMFP_QUAL_NAT_INT_CN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INT_CN_MASKf:
            qual = BCMFP_QUAL_NAT_INT_CN;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MACADDR_NORMALIZEf:
            qual = BCMFP_QUAL_MACADDR_NORMALIZE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MACADDR_NORMALIZE_MASKf:
            qual = BCMFP_QUAL_MACADDR_NORMALIZE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IPADDR_NORMALIZEf:
            qual = BCMFP_QUAL_IPADDR_NORMALIZE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IPADDR_NORMALIZE_MASKf:
            qual = BCMFP_QUAL_IPADDR_NORMALIZE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VLAN_INNER_PRESENTf:
            qual = BCMFP_QUAL_VLAN_INNER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VLAN_INNER_PRESENT_MASKf:
            qual = BCMFP_QUAL_VLAN_INNER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf:
            qual = BCMFP_QUAL_VLAN_OUTER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_MASKf:
            qual = BCMFP_QUAL_VLAN_OUTER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TRANSLATED_VLAN_OUTER_PRESENTf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_OUTER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TRANSLATED_VLAN_OUTER_PRESENT_MASKf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_OUTER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TRANSLATED_VLAN_INNER_PRESENTf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_INNER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TRANSLATED_VLAN_INNER_PRESENT_MASKf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_INNER_PRESENT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_BYTES_AFTER_L2HEADERf:
            qual = BCMFP_QUAL_BYTES_AFTER_L2HEADER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_BYTES_AFTER_L2HEADER_MASKf:
            qual = BCMFP_QUAL_BYTES_AFTER_L2HEADER;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_CHECKSUM_VALIDf:
            qual = BCMFP_QUAL_IP_CHECKSUM_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_IP_CHECKSUM_VALID_MASKf:
            qual = BCMFP_QUAL_IP_CHECKSUM_VALID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_BFD_DISCRIMINATORf:
            qual = BCMFP_QUAL_BFD_DISCRIMINATOR;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_BFD_DISCRIMINATOR_MASKf:
            qual = BCMFP_QUAL_BFD_DISCRIMINATOR;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PKT_LENGTHf:
            qual = BCMFP_QUAL_PKT_LENGTH;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PKT_LENGTH_MASKf:
            qual = BCMFP_QUAL_PKT_LENGTH;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_RANGE_CHECKID_BMPf:
            qual = BCMFP_QUAL_RANGE_CHECKID_BMP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_RANGE_CHECKID_BMP_MASKf:
            qual = BCMFP_QUAL_RANGE_CHECKID_BMP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FORWARDING_LABEL_ACTIONf:
            qual = BCMFP_QUAL_MPLS_FORWARDING_LABEL_ACTION;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_LOOPBACK_TYPEf:
            qual = BCMFP_QUAL_LOOPBACK_TYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_TNL_TYPEf:
            qual = BCMFP_QUAL_TUNNEL_TYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_LOOPBACKf:
            qual = BCMFP_QUAL_LOOPBACK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_LOOPBACK_MASKf:
            qual = BCMFP_QUAL_LOOPBACK;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_PKT_RESOLUTIONf:
            qual = BCMFP_QUAL_PKT_RES;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_HITf:
            qual = BCMFP_QUAL_EM_FIRST_LOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_HIT_MASKf:
            qual = BCMFP_QUAL_EM_FIRST_LOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_SECOND_LOOKUP_HITf:
            qual = BCMFP_QUAL_EM_SECOND_LOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_SECOND_LOOKUP_HIT_MASKf:
            qual = BCMFP_QUAL_EM_SECOND_LOOKUP_HIT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_CLASSIDf:
            qual = BCMFP_QUAL_EM_FIRST_LOOKUP_CLASSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_CLASSID_MASKf:
            qual = BCMFP_QUAL_EM_FIRST_LOOKUP_CLASSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_SECOND_LOOKUP_CLASSIDf:
            qual = BCMFP_QUAL_EM_SECOND_LOOKUP_CLASSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_EM_SECOND_LOOKUP_CLASSID_MASKf:
            qual = BCMFP_QUAL_EM_SECOND_LOOKUP_CLASSID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L3_TYPEf:
            qual = BCMFP_QUAL_L3_TYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_L2_FORMATf:
            qual = BCMFP_QUAL_L2_FORMAT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_INNER_TPIDf:
            qual = BCMFP_QUAL_INNER_TPID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_OUTER_TPIDf:
            qual = BCMFP_QUAL_OUTER_TPID;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_FORWARDING_TYPEf:
            qual = BCMFP_QUAL_FWD_TYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_ING_STP_STATEf:
            qual = BCMFP_QUAL_INGRESS_STP_STATE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_ENUM;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_L4SRC_PORTf:
            qual = BCMFP_QUAL_C_L4SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_L4SRC_PORT_MASKf:
            qual = BCMFP_QUAL_C_L4SRC_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_L4DST_PORTf:
            qual = BCMFP_QUAL_C_L4DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_L4DST_PORT_MASKf:
            qual = BCMFP_QUAL_C_L4DST_PORT;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_SRC_IPf:
            qual = BCMFP_QUAL_C_SRC_IP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_SRC_IP_MASKf:
            qual = BCMFP_QUAL_C_SRC_IP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_DST_IPf:
            qual = BCMFP_QUAL_C_DST_IP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_DST_IP_MASKf:
            qual = BCMFP_QUAL_C_DST_IP;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_ETHERTYPEf:
            qual = BCMFP_QUAL_C_ETHERTYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_ETHERTYPE_MASKf:
            qual = BCMFP_QUAL_C_ETHERTYPE;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TTL0f:
            qual = BCMFP_QUAL_C_TTL0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TTL0_MASKf:
            qual = BCMFP_QUAL_C_TTL0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TTL1f:
            qual = BCMFP_QUAL_C_TTL1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TTL1_MASKf:
            qual = BCMFP_QUAL_C_TTL1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TOS0f:
            qual = BCMFP_QUAL_C_TOS0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TOS0_MASKf:
            qual = BCMFP_QUAL_C_TOS0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TOS1f:
            qual = BCMFP_QUAL_C_TOS1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TOS1_MASKf:
            qual = BCMFP_QUAL_C_TOS1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS0f:
            qual = BCMFP_QUAL_C_TCP_CONTROL0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS0_MASKf:
            qual = BCMFP_QUAL_C_TCP_CONTROL0;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS1f:
            qual = BCMFP_QUAL_C_TCP_CONTROL1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS1_MASKf:
            qual = BCMFP_QUAL_C_TCP_CONTROL1;
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_UDF_CHUNKSf:
            field_type = BCMFP_LT_RULE_FIELD_TYPE_DATA;
            SHR_IF_ERR_EXIT(
                bcmfp_udf_qual_id_get(unit,
                                      BCMFP_STAGE_ID_INGRESS,
                                      (uint8_t)idx,
                                      &qual));
            break;
        case FP_ING_RULE_TEMPLATEt_QUAL_UDF_CHUNKS_MASKf:
            field_type = BCMFP_LT_RULE_FIELD_TYPE_MASK;
            SHR_IF_ERR_EXIT(
                bcmfp_udf_qual_id_get(unit,
                                      BCMFP_STAGE_ID_INGRESS,
                                      (uint8_t)idx,
                                      &qual));
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
