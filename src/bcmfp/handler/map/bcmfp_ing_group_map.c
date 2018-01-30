/*! \file bcmfp_ing_group_map.c
 *
 * APIs for Ingress Group LT fields to internal value mapping
 *
 * This file contains function definitions for group LT template
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/map/bcmfp_lt_group_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

/*! Ingress FP Group Field Identifiers that needs individual handling. */
bcmfp_lt_group_sp_fids_t bcmfp_ing_group_sp_fids = {
    .group_id_fid = FP_ING_GRP_TEMPLATEt_FP_ING_GRP_TEMPLATE_IDf,
    .pipe_fid = FP_ING_GRP_TEMPLATEt_PIPEf,
    .priority_fid = FP_ING_GRP_TEMPLATEt_ENTRY_PRIORITYf,
    .mode_fid = FP_ING_GRP_TEMPLATEt_MODEf,
    .mode_auto_fid = FP_ING_GRP_TEMPLATEt_MODE_AUTOf,
    .mode_oper_fid = FP_ING_GRP_TEMPLATEt_MODE_OPERf,
    .presel_ids_set_count_fid = FP_ING_GRP_TEMPLATEt_FP_ING_PRESEL_ENTRY_COUNTf,
    .presel_ids_set_fid = FP_ING_GRP_TEMPLATEt_FP_ING_PRESEL_ENTRY_TEMPLATE_IDf,
    .presel_lt_sid = FP_ING_PRESEL_ENTRY_TEMPLATEt,
    .presel_lt_id_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_FP_ING_PRESEL_ENTRY_TEMPLATE_IDf,
    .presel_lt_mixed_src_class_mask_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASS_MASKf,
    .src_class_mode_lt_sid = FP_ING_SRC_CLASS_MODEt,
};

/*! Array of fid of read-only fields in VFP group */
uint32_t bcmfp_ing_group_add_field_fids[] = {
    FP_ING_GRP_TEMPLATEt_MODE_OPERf
};

/*******************************************************************************
 * Public functions
 */
int
bcmfp_ing_group_init(int unit,
                     bcmfp_lt_info_t *lt_info)
{
    SHR_FUNC_ENTER(unit)

    lt_info->flags |= BCMFP_LT_FLAGS_INMEM_SUPPORT;
    lt_info->flags |= BCMFP_LT_FLAGS_PRESEL_VALID;
    lt_info->map_get = bcmfp_ing_group_qual_map_get;
    lt_info->key_fid = FP_ING_GRP_TEMPLATEt_FP_ING_GRP_TEMPLATE_IDf;
    lt_info->sp_fids = (void *) &bcmfp_ing_group_sp_fids;
    lt_info->add_fields_fid = bcmfp_ing_group_add_field_fids;
    lt_info->add_fields_count = COUNTOF(bcmfp_ing_group_add_field_fids);;
    lt_info->comp_id = BCMMGMT_FP_COMP_ID;
    lt_info->sub_id = BCMFP_LT_IMM_SUBID_ING_GRP_TEMPLATE;
    lt_info->key_sz = BCMFP_LT_IMM_KEY_SZ_4BYTE;
    lt_info->fdata_format = BCMFP_LT_IMM_FDATA_FORMAT_I1D3;
    lt_info->rows = 32;

    SHR_FUNC_EXIT();
}

/* Mapping of LT field to Qualifier */
int
bcmfp_ing_group_qual_map_get(int unit,
                             uint32_t fid,
                             uint32_t idx,
                             void *map)
{
    bcmfp_qual_t qual;
    bcmfp_lt_group_qual_map_t *group_map = NULL;
    SHR_FUNC_ENTER(unit);

    group_map = (bcmfp_lt_group_qual_map_t *) map;

    switch(fid) {
        case FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_ID_BITMAPf:
            qual = BCMFP_QUAL_OUTER_VLAN_ID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRI_BITMAPf:
            qual = BCMFP_QUAL_OUTER_VLAN_PRIORITY;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFI_BITMAPf:
            qual = BCMFP_QUAL_OUTER_VLAN_CFI;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INNER_VLAN_ID_BITMAPf:
            qual = BCMFP_QUAL_INNER_VLAN_ID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INNER_VLAN_PRI_BITMAPf:
            qual = BCMFP_QUAL_INNER_VLAN_PRIORITY;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INNER_VLAN_CFI_BITMAPf:
            qual = BCMFP_QUAL_INNER_VLAN_CFI;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_MAC_BITMAPf:
            qual = BCMFP_QUAL_SRC_MAC;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_MAC_BITMAPf:
            qual = BCMFP_QUAL_DST_MAC;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP4_BITMAPf:
            qual = BCMFP_QUAL_SRC_IP4;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_IP4_BITMAPf:
            qual = BCMFP_QUAL_DST_IP4;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_UPPERf:
            qual = BCMFP_QUAL_SRC_IP6_UPPER;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_LOWERf:
            qual = BCMFP_QUAL_SRC_IP6_LOWER;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_UPPERf:
            qual = BCMFP_QUAL_DST_IP6_UPPER;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_LOWERf:
            qual = BCMFP_QUAL_DST_IP6_LOWER;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_ETHERTYPE_BITMAPf:
            qual = BCMFP_QUAL_ETHERTYPE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L4SRC_PORT_BITMAPf:
            qual = BCMFP_QUAL_L4SRC_PORT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L4DST_PORT_BITMAPf:
            qual = BCMFP_QUAL_L4DST_PORT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MIM_ISID_BITMAPf:
            qual = BCMFP_QUAL_MIM_ISID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_NVGRE_VSID_BITMAPf:
            qual = BCMFP_QUAL_NVGRE_VSID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_VNID_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_VNID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_FLAGS_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_FLAGS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_24_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_RSVD_24;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_8_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_RSVD_8;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_ETAG_BITMAPf:
            qual = BCMFP_QUAL_ETAG;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VNTAG_BITMAPf:
            qual = BCMFP_QUAL_VNTAG;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_CNTAG_BITMAPf:
            qual = BCMFP_QUAL_CNTAG;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ID_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_ID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTL_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_TTL;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXP_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_EXP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOS_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_BOS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_CTRL_WORD_BITMAPf:
            qual = BCMFP_QUAL_MPLS_CTRL_WORD;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP6_FLOW_LABEL_BITMAPf:
            qual = BCMFP_QUAL_IP6_FLOW_LABEL;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TTL_BITMAPf:
            qual = BCMFP_QUAL_TTL;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TOS_BITMAPf:
            qual = BCMFP_QUAL_TOS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_PROTOCOL_BITMAPf:
            qual = BCMFP_QUAL_IP_PROTOCOL;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TCP_CONTROL_FLAGS_BITMAPf:
            qual = BCMFP_QUAL_TCP_CONTROL;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_BITMAPf:
            qual = BCMFP_QUAL_IP_FIRST_EH_PROTO;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_SECOND_EH_PROTO_BITMAPf:
            qual = BCMFP_QUAL_IP_SECOND_EH_PROTO;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_BITMAPf:
            qual = BCMFP_QUAL_IP_FIRST_EH_SUBCODE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_FLAGS_MF_BITMAPf:
            qual = BCMFP_QUAL_IP_FLAGS_MF;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_FLAGS_DF_BITMAPf:
            qual = BCMFP_QUAL_IP_FLAGS_DF;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_FRAG_BITMAPf:
            qual = BCMFP_QUAL_IP_FRAG;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_HIGIG_PKT_BITMAPf:
            qual = BCMFP_QUAL_HIGIG_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_HIGIGLOOKUP_PKT_BITMAPf:
            qual = BCMFP_QUAL_HIGIGLOOKUP_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DROP_PKT_BITMAPf:
            qual = BCMFP_QUAL_DROP_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VISIBILITY_PKT_BITMAPf:
            qual = BCMFP_QUAL_VISIBILITY_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DOSATTACK_PKT_BITMAPf:
            qual = BCMFP_QUAL_DOSATTACK_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L4_PKT_BITMAPf:
            qual = BCMFP_QUAL_L4_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MIRROR_PKT_BITMAPf:
            qual = BCMFP_QUAL_MIRROR_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENT_BITMAPf:
            qual = BCMFP_QUAL_NON_FRAGMENT_FIRST_FRAGMENT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_ICMP_ERROR_PKT_BITMAPf:
            qual = BCMFP_QUAL_ICMP_ERROR_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_REPLICATION_PKT_BITMAPf:
            qual = BCMFP_QUAL_REPLICATION_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2SRC_HIT_BITMAPf:
            qual = BCMFP_QUAL_L2SRC_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2DST_HIT_BITMAPf:
            qual = BCMFP_QUAL_L2DST_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3SRC_HOST_HIT_BITMAPf:
            qual = BCMFP_QUAL_L3SRC_HOST_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3DST_HOST_HIT_BITMAPf:
            qual = BCMFP_QUAL_L3DST_HOST_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3DST_LPM_HIT_BITMAPf:
            qual = BCMFP_QUAL_L3DST_LPM_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3_TNL_HIT_BITMAPf:
            qual = BCMFP_QUAL_L3_TUNNEL_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VXLT_LOOKUP_HIT_BITMAPf:
            qual = BCMFP_QUAL_VXLT_LOOKUP_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2CACHE_HIT_BITMAPf:
            qual = BCMFP_QUAL_L2CACHE_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_BITMAPf:
            qual = BCMFP_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_BITMAPf:
            qual = BCMFP_QUAL_MPLSENTRY_SECONDLOOKUP_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_BITMAPf:
            qual = BCMFP_QUAL_MYSTATIONTCAM_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2SRC_STATIC_BITMAPf:
            qual = BCMFP_QUAL_L2SRC_STATIC;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2STATION_MOVE_BITMAPf:
            qual = BCMFP_QUAL_L2STATION_MOVE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_MPLS_BOS_TERMINATED_BITMAPf:
            qual = BCMFP_QUAL_MPLS_BOS_TERMINATED;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_IP_LOCAL_BITMAPf:
            qual = BCMFP_QUAL_DEST_IP_LOCAL;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_BITMAPf:
            qual = BCMFP_QUAL_L3_ROUTABLE_PKT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SYSTEM_PORT_CLASS_BITMAPf:
            qual = BCMFP_QUAL_SYSTEM_PORT_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3_INTF_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L3_INTF_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_VP_CLASS_BITMAPf:
            qual = BCMFP_QUAL_SRC_VP_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VLAN_CLASS_BITMAPf:
            qual = BCMFP_QUAL_VLAN_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS0_BITMAPf:
            qual = BCMFP_QUAL_FP_VLAN_CLASS0;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS1_BITMAPf:
            qual = BCMFP_QUAL_FP_VLAN_CLASS1;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_PORT_FP_VLAN_GROUPID_BITMAPf:
            qual = BCMFP_QUAL_PORT_FP_VLAN_GROUPID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_PORT_FP_ING_KEYSELECT_CLASS_BITMAPf:
            qual = BCMFP_QUAL_PORT_FP_ING_KEYSELECT_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_UDF_CLASS_BITMAPf:
            qual = BCMFP_QUAL_UDF_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2SRC_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L2SRC_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2DST_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L2DEST_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3SRC_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L3SRC_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3DST_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L3DEST_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_PRESEL_CLASS_BITMAPf:
            qual = BCMFP_QUAL_PRESEL_CLASS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_0_BITMAPf:
            qual = BCMFP_QUAL_EM_GROUP_CLASS_ID_0;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_1_BITMAPf:
            qual = BCMFP_QUAL_EM_GROUP_CLASS_ID_1;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_FWD_VLAN_VALID_BITMAPf:
            qual = BCMFP_QUAL_FWD_VLAN_VALID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_VP_VALID_BITMAPf:
            qual = BCMFP_QUAL_SRC_VP_VALID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_VP_VALID_BITMAPf:
            qual = BCMFP_QUAL_DST_VP_VALID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_MODULE_BITMAPf:
            qual = BCMFP_QUAL_SRC_MODULE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_PORT_BITMAPf:
            qual = BCMFP_QUAL_SRC_PORT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_TRUNK_BITMAPf:
            qual = BCMFP_QUAL_SRC_TRUNK;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SRC_VP_BITMAPf:
            qual = BCMFP_QUAL_SRC_VP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_MODULE_BITMAPf:
            qual = BCMFP_QUAL_DST_MODULE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_PORT_BITMAPf:
            qual = BCMFP_QUAL_DST_PORT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_TRUNK_BITMAPf:
            qual = BCMFP_QUAL_DST_TRUNK;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_VP_TRUNK_BITMAPf:
            qual = BCMFP_QUAL_DST_VP_TRUNK;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DST_VP_BITMAPf:
            qual = BCMFP_QUAL_DST_VP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_NHOP_BITMAPf:
            qual = BCMFP_QUAL_DST_NHOP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_OVERLAY_ECMP_BITMAPf:
            qual = BCMFP_QUAL_DST_OVERLAY_ECMP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_UNDERLAY_ECMP_BITMAPf:
            qual = BCMFP_QUAL_DST_UNDERLAY_ECMP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3_MC_GROUP_BITMAPf:
            qual = BCMFP_QUAL_DST_L3_MC_GROUP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2_MC_GROUP_BITMAPf:
            qual = BCMFP_QUAL_DST_L2_MC_GROUP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_FWD_VLAN_ID_BITMAPf:
            qual = BCMFP_QUAL_FWD_VLAN_ID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VRF_BITMAPf:
            qual = BCMFP_QUAL_VRF;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VPN_BITMAPf:
            qual = BCMFP_QUAL_VPN;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3_IIF_BITMAPf:
            qual = BCMFP_QUAL_L3_IIF;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INPORT_BITMAPf:
            qual = BCMFP_QUAL_INPORT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INPORTS_BITMAPf:
            qual = BCMFP_QUAL_INPORTS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_SYSTEM_PORTS_BITMAPf:
            qual = BCMFP_QUAL_SYSTEM_PORTS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_DEVICE_PORTS_BITMAPf:
            qual = BCMFP_QUAL_DEVICE_PORTS;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INT_PRI_BITMAPf:
            qual = BCMFP_QUAL_INT_PRI;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_COLOR_BITMAPf:
            qual = BCMFP_QUAL_COLOR;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_RAL_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_RAL_PRESENT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_GAL_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_GAL_PRESENT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_CW_VALID_BITMAPf:
            qual = BCMFP_QUAL_MPLS_CW_VALID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_NAT_NEEDED_BITMAPf:
            qual = BCMFP_QUAL_NAT_NEEDED;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_NAT_SRC_REALM_ID_BITMAPf:
            qual = BCMFP_QUAL_NAT_SRC_REALM_ID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_NAT_DST_REALM_ID_BITMAPf:
            qual = BCMFP_QUAL_NAT_DST_REALM_ID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INT_CN_BITMAPf:
            qual = BCMFP_QUAL_NAT_INT_CN;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MACADDR_NORMALIZE_BITMAPf:
            qual = BCMFP_QUAL_MACADDR_NORMALIZE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IPADDR_NORMALIZE_BITMAPf:
            qual = BCMFP_QUAL_IPADDR_NORMALIZE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VLAN_INNER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_INNER_PRESENT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_OUTER_PRESENT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TRANSLATED_VLAN_INNER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_INNER_PRESENT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TRANSLATED_VLAN_OUTER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_OUTER_PRESENT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_BYTES_AFTER_L2HEADER_BITMAPf:
            qual = BCMFP_QUAL_BYTES_AFTER_L2HEADER;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_IP_CHECKSUM_VALID_BITMAPf:
            qual = BCMFP_QUAL_IP_CHECKSUM_VALID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_BFD_DISCRIMINATOR_BITMAPf:
            qual = BCMFP_QUAL_BFD_DISCRIMINATOR;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_PKT_LENGTH_BITMAPf:
            qual = BCMFP_QUAL_PKT_LENGTH;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_RANGE_CHECKID_BMP_BITMAPf:
            qual = BCMFP_QUAL_RANGE_CHECKID_BMP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FORWARDING_LABEL_ACTION_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FORWARDING_LABEL_ACTION;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPE_BITMAPf:
            qual = BCMFP_QUAL_LOOPBACK_TYPE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_TNL_TYPE_BITMAPf:
            qual = BCMFP_QUAL_TUNNEL_TYPE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_LOOPBACK_BITMAPf:
            qual = BCMFP_QUAL_LOOPBACK;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_PKT_RESOLUTION_BITMAPf:
            qual = BCMFP_QUAL_PKT_RES;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_HITf:
            qual = BCMFP_QUAL_EM_FIRST_LOOKUP_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_EM_SECOND_LOOKUP_HITf:
            qual = BCMFP_QUAL_EM_SECOND_LOOKUP_HIT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_CLASSID_BITMAPf:
            qual = BCMFP_QUAL_EM_FIRST_LOOKUP_CLASSID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_EM_SECOND_LOOKUP_CLASSID_BITMAPf:
            qual = BCMFP_QUAL_EM_SECOND_LOOKUP_CLASSID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L3_TYPE_BITMAPf:
            qual = BCMFP_QUAL_L3_TYPE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_L2_FORMAT_BITMAPf:
            qual = BCMFP_QUAL_L2_FORMAT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_INNER_TPID_BITMAPf:
            qual = BCMFP_QUAL_INNER_TPID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_OUTER_TPID_BITMAPf:
            qual = BCMFP_QUAL_OUTER_TPID;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_FORWARDING_TYPE_BITMAPf:
            qual = BCMFP_QUAL_FWD_TYPE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_ING_STP_STATE_BITMAPf:
            qual = BCMFP_QUAL_INGRESS_STP_STATE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_L4SRC_PORT_BITMAPf:
            qual = BCMFP_QUAL_C_L4SRC_PORT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_L4DST_PORT_BITMAPf:
            qual = BCMFP_QUAL_C_L4DST_PORT;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_ETHERTYPE_BITMAPf:
            qual = BCMFP_QUAL_C_ETHERTYPE;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_TTL0_BITMAPf:
            qual = BCMFP_QUAL_C_TTL0;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_TTL1_BITMAPf:
            qual = BCMFP_QUAL_C_TTL1;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_TOS0_BITMAPf:
            qual = BCMFP_QUAL_C_TOS0;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_TOS1_BITMAPf:
            qual = BCMFP_QUAL_C_TOS1;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS0_BITMAPf:
            qual = BCMFP_QUAL_C_TCP_CONTROL0;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS1_BITMAPf:
            qual = BCMFP_QUAL_C_TCP_CONTROL1;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_SRC_IP_BITMAPf:
            qual = BCMFP_QUAL_C_SRC_IP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_C_DST_IP_BITMAPf:
            qual = BCMFP_QUAL_C_DST_IP;
            break;
        case FP_ING_GRP_TEMPLATEt_QUAL_UDF_CHUNKS_BITMAPf:
            SHR_IF_ERR_EXIT(
                bcmfp_udf_qual_id_get(unit,
                                      BCMFP_STAGE_ID_INGRESS,
                                      (uint8_t)idx,
                                      &qual));
            break;
        default:
            /* Fid is Not Qualifier. */
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            break;
    }

    group_map->fid = fid;
    group_map->qual = qual;

exit:

    SHR_FUNC_EXIT();
}
