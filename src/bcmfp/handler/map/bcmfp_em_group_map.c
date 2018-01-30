/*! \file bcmfp_em_group_map.c
 *
 * APIs for Exact Match LT fields to internal value mapping
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
bcmfp_lt_group_sp_fids_t bcmfp_em_group_sp_fids = {
    .group_id_fid = FP_EM_GRP_TEMPLATEt_FP_EM_GRP_TEMPLATE_IDf,
    .policy_id_fid = FP_EM_GRP_TEMPLATEt_FP_EM_POLICY_TEMPLATE_IDf,
    .pipe_fid = FP_EM_GRP_TEMPLATEt_PIPEf,
    .priority_fid = FP_EM_GRP_TEMPLATEt_ENTRY_PRIORITYf,
    .group_ltid_auto_fid= FP_EM_GRP_TEMPLATEt_LTID_AUTOf,
    .group_ltid_fid = FP_EM_GRP_TEMPLATEt_LTIDf,
    .group_ltid_oper_fid = FP_EM_GRP_TEMPLATEt_LTID_OPERf,
    .group_lookup_id_fid = FP_EM_GRP_TEMPLATEt_LOOKUPf,
    .mode_fid = FP_EM_GRP_TEMPLATEt_MODEf,
    .mode_auto_fid = FP_EM_GRP_TEMPLATEt_MODE_AUTOf,
    .mode_oper_fid = FP_EM_GRP_TEMPLATEt_MODE_OPERf,
    .presel_ids_set_count_fid = FP_EM_GRP_TEMPLATEt_FP_EM_PRESEL_ENTRY_COUNTf,
    .presel_ids_set_fid = FP_EM_GRP_TEMPLATEt_FP_EM_PRESEL_ENTRY_TEMPLATE_IDf,
    .presel_lt_sid = FP_EM_PRESEL_ENTRY_TEMPLATEt,
    .presel_lt_id_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_FP_EM_PRESEL_ENTRY_TEMPLATE_IDf,
    .presel_lt_mixed_src_class_mask_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASS_MASKf,
    .src_class_mode_lt_sid = FP_EM_SRC_CLASS_MODEt
};

/*******************************************************************************
 * Public functions
 */
int
bcmfp_em_group_init(int unit,
                     bcmfp_lt_info_t *lt_info)
{
    SHR_FUNC_ENTER(unit)

    lt_info->flags |= BCMFP_LT_FLAGS_INMEM_SUPPORT;
    lt_info->flags |= BCMFP_LT_FLAGS_PRESEL_VALID;
    lt_info->flags |= BCMFP_LT_FLAGS_HASH_KEY_GRP;
    lt_info->map_get = bcmfp_em_group_field_map_get;
    lt_info->key_fid = FP_EM_GRP_TEMPLATEt_FP_EM_GRP_TEMPLATE_IDf;
    lt_info->sp_fids = (void *) &bcmfp_em_group_sp_fids;

    SHR_FUNC_EXIT();
}

/* Mapping of LT field to Qualifier */
int
bcmfp_em_group_field_map_get(int unit,
                             uint32_t fid,
                             uint32_t idx,
                             void *map)
{
    bcmfp_qual_t qual = -1;
    bcmfp_action_t action = -1;
    bcmfp_lt_em_group_field_type_t field_type;
    bcmfp_lt_group_qual_map_t *group_map = NULL;
    SHR_FUNC_ENTER(unit);

    group_map = (bcmfp_lt_group_qual_map_t*) map;

    switch(fid) {
        case FP_EM_GRP_TEMPLATEt_QUAL_OUTER_VLAN_ID_BITMAPf:
            qual = BCMFP_QUAL_OUTER_VLAN_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRI_BITMAPf:
            qual = BCMFP_QUAL_OUTER_VLAN_PRIORITY;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFI_BITMAPf:
            qual = BCMFP_QUAL_OUTER_VLAN_CFI;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_INNER_VLAN_ID_BITMAPf:
            qual = BCMFP_QUAL_INNER_VLAN_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_INNER_VLAN_PRI_BITMAPf:
            qual = BCMFP_QUAL_INNER_VLAN_PRIORITY;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_INNER_VLAN_CFI_BITMAPf:
            qual = BCMFP_QUAL_INNER_VLAN_CFI;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_MAC_BITMAPf:
            qual = BCMFP_QUAL_SRC_MAC;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DST_MAC_BITMAPf:
            qual = BCMFP_QUAL_DST_MAC;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_IP4_BITMAPf:
            qual = BCMFP_QUAL_SRC_IP4;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DST_IP4_BITMAPf:
            qual = BCMFP_QUAL_DST_IP4;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_UPPERf:
            qual = BCMFP_QUAL_SRC_IP6_UPPER;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_LOWERf:
            qual = BCMFP_QUAL_SRC_IP6_LOWER;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_UPPERf:
            qual = BCMFP_QUAL_DST_IP6_UPPER;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_LOWERf:
            qual = BCMFP_QUAL_DST_IP6_LOWER;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_ETHERTYPE_BITMAPf:
            qual = BCMFP_QUAL_ETHERTYPE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L4SRC_PORT_BITMAPf:
            qual = BCMFP_QUAL_L4SRC_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L4DST_PORT_BITMAPf:
            qual = BCMFP_QUAL_L4DST_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MIM_ISID_BITMAPf:
            qual = BCMFP_QUAL_MIM_ISID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_NVGRE_VSID_BITMAPf:
            qual = BCMFP_QUAL_NVGRE_VSID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_VNID_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_VNID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_FLAGS_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_FLAGS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_24_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_RSVD_24;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_8_BITMAPf:
            qual = BCMFP_QUAL_VXLAN_RSVD_8;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_ETAG_BITMAPf:
            qual = BCMFP_QUAL_ETAG;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VNTAG_BITMAPf:
            qual = BCMFP_QUAL_VNTAG;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_CNTAG_BITMAPf:
            qual = BCMFP_QUAL_CNTAG;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ID_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTL_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_TTL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXP_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_EXP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOS_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FWD_LABEL_BOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLS_CTRL_WORD_BITMAPf:
            qual = BCMFP_QUAL_MPLS_CTRL_WORD;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP6_FLOW_LABEL_BITMAPf:
            qual = BCMFP_QUAL_IP6_FLOW_LABEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_TTL_BITMAPf:
            qual = BCMFP_QUAL_TTL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_TOS_BITMAPf:
            qual = BCMFP_QUAL_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_PROTOCOL_BITMAPf:
            qual = BCMFP_QUAL_IP_PROTOCOL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_TCP_CONTROL_FLAGS_BITMAPf:
            qual = BCMFP_QUAL_TCP_CONTROL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_BITMAPf:
            qual = BCMFP_QUAL_IP_FIRST_EH_PROTO;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_SECOND_EH_PROTO_BITMAPf:
            qual = BCMFP_QUAL_IP_SECOND_EH_PROTO;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_BITMAPf:
            qual = BCMFP_QUAL_IP_FIRST_EH_SUBCODE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_FLAGS_MF_BITMAPf:
            qual = BCMFP_QUAL_IP_FLAGS_MF;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_FLAGS_DF_BITMAPf:
            qual = BCMFP_QUAL_IP_FLAGS_DF;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_FRAG_BITMAPf:
            qual = BCMFP_QUAL_IP_FRAG;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_HIGIG_PKT_BITMAPf:
            qual = BCMFP_QUAL_HIGIG_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_HIGIGLOOKUP_PKT_BITMAPf:
            qual = BCMFP_QUAL_HIGIGLOOKUP_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DROP_PKT_BITMAPf:
            qual = BCMFP_QUAL_DROP_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VISIBILITY_PKT_BITMAPf:
            qual = BCMFP_QUAL_VISIBILITY_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DOSATTACK_PKT_BITMAPf:
            qual = BCMFP_QUAL_DOSATTACK_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L4_PKT_BITMAPf:
            qual = BCMFP_QUAL_L4_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MIRROR_PKT_BITMAPf:
            qual = BCMFP_QUAL_MIRROR_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENT_BITMAPf:
            qual = BCMFP_QUAL_NON_FRAGMENT_FIRST_FRAGMENT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_ICMP_ERROR_PKT_BITMAPf:
            qual = BCMFP_QUAL_ICMP_ERROR_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_REPLICATION_PKT_BITMAPf:
            qual = BCMFP_QUAL_REPLICATION_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L3_TNL_HIT_BITMAPf:
            qual = BCMFP_QUAL_L3_TUNNEL_HIT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VXLT_LOOKUP_HIT_BITMAPf:
            qual = BCMFP_QUAL_VXLT_LOOKUP_HIT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_BITMAPf:
            qual = BCMFP_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_BITMAPf:
            qual = BCMFP_QUAL_MPLSENTRY_SECONDLOOKUP_HIT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_BITMAPf:
            qual = BCMFP_QUAL_MYSTATIONTCAM_HIT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DST_MPLS_BOS_TERMINATED_BITMAPf:
            qual = BCMFP_QUAL_MPLS_BOS_TERMINATED;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DST_IP_LOCAL_BITMAPf:
            qual = BCMFP_QUAL_DEST_IP_LOCAL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_BITMAPf:
            qual = BCMFP_QUAL_L3_ROUTABLE_PKT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SYSTEM_PORT_CLASS_BITMAPf:
            qual = BCMFP_QUAL_SYSTEM_PORT_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L3_INTF_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L3_INTF_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_VP_CLASS_BITMAPf:
            qual = BCMFP_QUAL_SRC_VP_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VLAN_CLASS_BITMAPf:
            qual = BCMFP_QUAL_VLAN_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS0_BITMAPf:
            qual = BCMFP_QUAL_FP_VLAN_CLASS0;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS1_BITMAPf:
            qual = BCMFP_QUAL_FP_VLAN_CLASS1;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_PORT_FP_VLAN_GROUPID_BITMAPf:
            qual = BCMFP_QUAL_PORT_FP_VLAN_GROUPID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_PORT_FP_ING_KEYSELECT_CLASS_BITMAPf:
            qual = BCMFP_QUAL_PORT_FP_ING_KEYSELECT_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_UDF_CLASS_BITMAPf:
            qual = BCMFP_QUAL_UDF_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L2SRC_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L2SRC_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L2DST_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L2DEST_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L3SRC_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L3SRC_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L3DST_CLASS_BITMAPf:
            qual = BCMFP_QUAL_L3DEST_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_PRESEL_CLASS_BITMAPf:
            qual = BCMFP_QUAL_PRESEL_CLASS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_FWD_VLAN_VALID_BITMAPf:
            qual = BCMFP_QUAL_FWD_VLAN_VALID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_VP_VALID_BITMAPf:
            qual = BCMFP_QUAL_SRC_VP_VALID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_DST_VP_VALID_BITMAPf:
            qual = BCMFP_QUAL_DST_VP_VALID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_PORT_BITMAPf:
            qual = BCMFP_QUAL_SRC_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_TRUNK_BITMAPf:
            qual = BCMFP_QUAL_SRC_TRUNK;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_SRC_VP_BITMAPf:
            qual = BCMFP_QUAL_SRC_VP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_FWD_VLAN_ID_BITMAPf:
            qual = BCMFP_QUAL_FWD_VLAN_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VRF_BITMAPf:
            qual = BCMFP_QUAL_VRF;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VPN_BITMAPf:
            qual = BCMFP_QUAL_VPN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L3_IIF_BITMAPf:
            qual = BCMFP_QUAL_L3_IIF;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_INPORT_BITMAPf:
            qual = BCMFP_QUAL_INPORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_RAL_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_RAL_PRESENT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_GAL_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_GAL_PRESENT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLS_CW_VALID_BITMAPf:
            qual = BCMFP_QUAL_MPLS_CW_VALID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_NAT_SRC_REALM_ID_BITMAPf:
            qual = BCMFP_QUAL_NAT_SRC_REALM_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_INT_CN_BITMAPf:
            qual = BCMFP_QUAL_NAT_INT_CN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MACADDR_NORMALIZE_BITMAPf:
            qual = BCMFP_QUAL_MACADDR_NORMALIZE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IPADDR_NORMALIZE_BITMAPf:
            qual = BCMFP_QUAL_IPADDR_NORMALIZE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VLAN_INNER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_INNER_PRESENT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_OUTER_PRESENT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_TRANSLATED_VLAN_INNER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_INNER_PRESENT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_TRANSLATED_VLAN_OUTER_PRESENT_BITMAPf:
            qual = BCMFP_QUAL_VLAN_TRANSLATED_OUTER_PRESENT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_BYTES_AFTER_L2HEADER_BITMAPf:
            qual = BCMFP_QUAL_BYTES_AFTER_L2HEADER;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_IP_CHECKSUM_VALID_BITMAPf:
            qual = BCMFP_QUAL_IP_CHECKSUM_VALID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_BFD_DISCRIMINATOR_BITMAPf:
            qual = BCMFP_QUAL_BFD_DISCRIMINATOR;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_PKT_LENGTH_BITMAPf:
            qual = BCMFP_QUAL_PKT_LENGTH;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_RANGE_CHECKID_BMP_BITMAPf:
            qual = BCMFP_QUAL_RANGE_CHECKID_BMP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FORWARDING_LABEL_ACTION_BITMAPf:
            qual = BCMFP_QUAL_MPLS_FORWARDING_LABEL_ACTION;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPE_BITMAPf:
            qual = BCMFP_QUAL_LOOPBACK_TYPE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_TNL_TYPE_BITMAPf:
            qual = BCMFP_QUAL_TUNNEL_TYPE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_LOOPBACK_BITMAPf:
            qual = BCMFP_QUAL_LOOPBACK;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L3_TYPE_BITMAPf:
            qual = BCMFP_QUAL_L3_TYPE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_L2_FORMAT_BITMAPf:
            qual = BCMFP_QUAL_L2_FORMAT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_INNER_TPID_BITMAPf:
            qual = BCMFP_QUAL_INNER_TPID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_OUTER_TPID_BITMAPf:
            qual = BCMFP_QUAL_OUTER_TPID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_FORWARDING_TYPE_BITMAPf:
            qual = BCMFP_QUAL_FWD_TYPE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_ING_STP_STATE_BITMAPf:
            qual = BCMFP_QUAL_INGRESS_STP_STATE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_L4SRC_PORT_BITMAPf:
            qual = BCMFP_QUAL_C_L4SRC_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_L4DST_PORT_BITMAPf:
            qual = BCMFP_QUAL_C_L4DST_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_ETHERTYPE_BITMAPf:
            qual = BCMFP_QUAL_C_ETHERTYPE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_TTL0_BITMAPf:
            qual = BCMFP_QUAL_C_TTL0;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_TTL1_BITMAPf:
            qual = BCMFP_QUAL_C_TTL1;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_TOS0_BITMAPf:
            qual = BCMFP_QUAL_C_TOS0;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_TOS1_BITMAPf:
            qual = BCMFP_QUAL_C_TOS1;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS0_BITMAPf:
            qual = BCMFP_QUAL_C_TCP_CONTROL0;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS1_BITMAPf:
            qual = BCMFP_QUAL_C_TCP_CONTROL1;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_DST_IP_BITMAPf:
            qual = BCMFP_QUAL_C_DST_IP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_C_SRC_IP_BITMAPf:
            qual = BCMFP_QUAL_C_SRC_IP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_QUAL_UDF_CHUNKS_BITMAPf:
            SHR_IF_ERR_EXIT(
                bcmfp_udf_qual_id_get(unit,
                                      BCMFP_STAGE_ID_EXACT_MATCH,
                                      (uint8_t)idx,
                                      &qual));
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_DROPf:
            action = BCMFP_ACTION_R_DROP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_DROPf:
            action = BCMFP_ACTION_Y_DROP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_DROPf:
            action = BCMFP_ACTION_G_DROP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_UNTAG_PKT_PRIORITYf:
            action = BCMFP_ACTION_NEW_UNTAG_PKT_PRIORITY;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_COLORf:
            action = BCMFP_ACTION_NEW_R_COLOR;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_COLORf:
            action = BCMFP_ACTION_NEW_Y_COLOR;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_COLORf:
            action = BCMFP_ACTION_NEW_G_COLOR;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_COS_MAPf:
            action = BCMFP_ACTION_NEW_R_COS_MAP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_INTPRI_TO_INNER_DOT1Pf:
            action = BCMFP_ACTION_R_INTPRI_TO_INNER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_INTPRIf:
            action = BCMFP_ACTION_NEW_R_INTPRI;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_INTPRI_TO_TOSf:
            action = BCMFP_ACTION_R_INTPRI_TO_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_INTPRI_UPDATES_CANCELf:
            action = BCMFP_ACTION_R_INTPRI_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_UC_COSf:
            action = BCMFP_ACTION_NEW_R_UC_COS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_MC_COSf:
            action = BCMFP_ACTION_NEW_R_MC_COS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_COS_MAPf:
            action = BCMFP_ACTION_NEW_Y_COS_MAP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_INTPRI_TO_INNER_DOT1Pf:
            action = BCMFP_ACTION_Y_INTPRI_TO_INNER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_INTPRIf:
            action = BCMFP_ACTION_NEW_Y_INTPRI;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_INTPRI_TO_TOSf:
            action = BCMFP_ACTION_Y_INTPRI_TO_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_INTPRI_UPDATES_CANCELf:
            action = BCMFP_ACTION_Y_INTPRI_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_UC_COSf:
            action = BCMFP_ACTION_NEW_Y_UC_COS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_MC_COSf:
            action = BCMFP_ACTION_NEW_Y_MC_COS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_COS_MAPf:
            action = BCMFP_ACTION_NEW_G_COS_MAP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_INTPRI_TO_INNER_DOT1Pf:
            action = BCMFP_ACTION_G_INTPRI_TO_INNER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_INTPRIf:
            action = BCMFP_ACTION_NEW_G_INTPRI;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_INTPRI_TO_TOSf:
            action = BCMFP_ACTION_G_INTPRI_TO_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_INTPRI_UPDATES_CANCELf:
            action = BCMFP_ACTION_G_INTPRI_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_UC_COSf:
            action = BCMFP_ACTION_NEW_G_UC_COS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_MC_COSf:
            action = BCMFP_ACTION_NEW_G_MC_COS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_HGT_SPRAY_HASH_CANCELf:
            action = BCMFP_ACTION_HGT_SPRAY_HASH_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_TRUNK_SPARY_HASH_CANCELf:
            action = BCMFP_ACTION_TRUNK_SPARY_HASH_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_ECMP_SPRAY_HASH_CANCELf:
            action = BCMFP_ACTION_ECMP_SPRAY_HASH_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_DO_NOT_NATf:
            action = BCMFP_ACTION_DO_NOT_NAT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_MATCH_IDf:
            action = BCMFP_ACTION_MATCH_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_COPY_TO_CPUf:
            action = BCMFP_ACTION_R_COPY_TO_CPU;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_COPY_TO_CPU_CANCELf:
            action = BCMFP_ACTION_R_COPY_TO_CPU_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_CANCELf:
            action = BCMFP_ACTION_R_SWITCH_TO_CPU_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_REINSATEf:
            action = BCMFP_ACTION_R_SWITCH_TO_CPU_REINSATE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_COPY_TO_CPU_WITH_TIMESTAMPf:
            action = BCMFP_ACTION_R_COPY_TO_CPU_WITH_TIMESTAMP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_COPY_TO_CPUf:
            action = BCMFP_ACTION_Y_COPY_TO_CPU;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_COPY_TO_CPU_CANCELf:
            action = BCMFP_ACTION_Y_COPY_TO_CPU_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_CANCELf:
            action = BCMFP_ACTION_Y_SWITCH_TO_CPU_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_REINSATEf:
            action = BCMFP_ACTION_Y_SWITCH_TO_CPU_REINSATE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMPf:
            action = BCMFP_ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_COPY_TO_CPUf:
            action = BCMFP_ACTION_G_COPY_TO_CPU;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_COPY_TO_CPU_CANCELf:
            action = BCMFP_ACTION_G_COPY_TO_CPU_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_CANCELf:
            action = BCMFP_ACTION_G_SWITCH_TO_CPU_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_REINSATEf:
            action = BCMFP_ACTION_G_SWITCH_TO_CPU_REINSATE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_COPY_TO_CPU_WITH_TIMESTAMPf:
            action = BCMFP_ACTION_G_COPY_TO_CPU_WITH_TIMESTAMP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_DO_NOT_CUT_THROUGHf:
            action = BCMFP_ACTION_DO_NOT_CUT_THROUGH;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_DO_NOT_URPFf:
            action = BCMFP_ACTION_DO_NOT_URPF;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_DO_NOT_CHANGE_TTLf:
            action = BCMFP_ACTION_DO_NOT_CHANGE_TTL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_ECNf:
            action = BCMFP_ACTION_NEW_R_ECN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_PRESERVE_DOT1Pf:
            action = BCMFP_ACTION_R_PRESERVE_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_OUTER_DOT1P_TO_INNER_DOT1Pf:
            action = BCMFP_ACTION_R_OUTER_DOT1P_TO_INNER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_OUTER_DOT1Pf:
            action = BCMFP_ACTION_NEW_R_OUTER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_OUTER_DOT1P_TO_TOSf:
            action = BCMFP_ACTION_R_OUTER_DOT1P_TO_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_DOT1P_UPDATES_CANCELf:
            action = BCMFP_ACTION_R_DOT1P_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_ECNf:
            action = BCMFP_ACTION_NEW_Y_ECN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_PRESERVE_DOT1Pf:
            action = BCMFP_ACTION_Y_PRESERVE_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_OUTER_DOT1P_TO_INNER_DOT1Pf:
            action = BCMFP_ACTION_Y_OUTER_DOT1P_TO_INNER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_OUTER_DOT1Pf:
            action = BCMFP_ACTION_NEW_Y_OUTER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_OUTER_DOT1P_TO_TOSf:
            action = BCMFP_ACTION_Y_OUTER_DOT1P_TO_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_DOT1P_UPDATES_CANCELf:
            action = BCMFP_ACTION_Y_DOT1P_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_ECNf:
            action = BCMFP_ACTION_NEW_G_ECN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;

        case FP_EM_GRP_TEMPLATEt_ACTION_G_PRESERVE_DOT1Pf:
            action = BCMFP_ACTION_G_PRESERVE_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_OUTER_DOT1P_TO_INNER_DOT1Pf:
            action = BCMFP_ACTION_G_OUTER_DOT1P_TO_INNER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_OUTER_DOT1Pf:
            action = BCMFP_ACTION_NEW_G_OUTER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_OUTER_DOT1P_TO_TOSf:
            action = BCMFP_ACTION_G_OUTER_DOT1P_TO_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_DOT1P_UPDATES_CANCELf:
            action = BCMFP_ACTION_G_DOT1P_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_DSCPf:
            action = BCMFP_ACTION_NEW_R_DSCP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_DSCP_UPDATES_CANCELf:
            action = BCMFP_ACTION_R_DSCP_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_PRESERVE_DSCPf:
            action = BCMFP_ACTION_R_PRESERVE_DSCP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_DSCPf:
            action = BCMFP_ACTION_NEW_Y_DSCP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_DSCP_UPDATES_CANCELf:
            action = BCMFP_ACTION_Y_DSCP_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_PRESERVE_DSCPf:
            action = BCMFP_ACTION_Y_PRESERVE_DSCP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_DSCPf:
            action = BCMFP_ACTION_NEW_G_DSCP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_DSCP_UPDATES_CANCELf:
            action = BCMFP_ACTION_G_DSCP_UPDATES_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_PRESERVE_DSCPf:
            action = BCMFP_ACTION_G_PRESERVE_DSCP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_TOSf:
            action = BCMFP_ACTION_NEW_G_TOS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_TOS_TO_PRE_FP_ING_OUTER_DOT1Pf:
            action = BCMFP_ACTION_G_TOS_TO_PRE_FP_ING_OUTER_DOT1P;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDSf:
            action = BCMFP_ACTION_CHANGE_PKT_L2_FIELDS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDS_CANCELf:
            action = BCMFP_ACTION_CHANGE_PKT_L2_FIELDS_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_CLASSIFICATION_TAGf:
            action = BCMFP_ACTION_NEW_CLASSIFICATION_TAG;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_SWITCH_TO_L3UCf:
            action = BCMFP_ACTION_SWITCH_TO_L3UC;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_SWITCH_TO_ECMPf:
            action = BCMFP_ACTION_SWITCH_TO_ECMP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_L3_SWITCH_CANCELf:
            action = BCMFP_ACTION_L3_SWITCH_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_I2E_CLASSID_SELECTf:
            action = BCMFP_ACTION_I2E_CLASSID_SELECT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_HIGIG_CLASSID_SELECTf:
            action = BCMFP_ACTION_HIGIG_CLASSID_SYSTEM_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_I2E_CLASSIDf:
            action = BCMFP_ACTION_NEW_I2E_CLASSID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_BFD_SEESSION_IDXf:
            action = BCMFP_ACTION_NEW_BFD_SEESSION_IDX;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_CPU_COSf:
            action = BCMFP_ACTION_NEW_CPU_COS;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_SERVICE_POOL_IDf:
            action = BCMFP_ACTION_NEW_SERVICE_POOL_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_R_DROP_CANCELf:
            action = BCMFP_ACTION_R_DROP_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_Y_DROP_CANCELf:
            action = BCMFP_ACTION_Y_DROP_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_G_DROP_CANCELf:
            action = BCMFP_ACTION_G_DROP_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_G_INTCNf:
            action = BCMFP_ACTION_NEW_G_INTCN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_Y_INTCNf:
            action = BCMFP_ACTION_NEW_Y_INTCN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_R_INTCNf:
            action = BCMFP_ACTION_NEW_R_INTCN;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_MIRROR_OVERRIDEf:
            action = BCMFP_ACTION_MIRROR_OVERRIDE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_SFLOW_ENABLEf:
            action = BCMFP_ACTION_SFLOW_ENABLE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_REDIRECT_TO_MODULEf:
            action = BCMFP_ACTION_REDIRECT_TO_MODULE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_REDIRECT_TO_PORTf:
            action = BCMFP_ACTION_REDIRECT_TO_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_REDIRECT_TO_DVPf:
            action = BCMFP_ACTION_REDIRECT_TO_DVP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_REDIRECT_TO_TRUNKf:
            action = BCMFP_ACTION_REDIRECT_TO_TRUNK;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_MODULEf:
            action = BCMFP_ACTION_UNMODIFIED_REDIRECT_TO_MODULE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_TRUNKf:
            action = BCMFP_ACTION_UNMODIFIED_REDIRECT_TO_TRUNK;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_PORTf:
            action = BCMFP_ACTION_UNMODIFIED_REDIRECT_TO_PORT;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_REDIRECT_TO_NHOPf:
            action = BCMFP_ACTION_REDIRECT_TO_NHOP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_REDIRECT_TO_ECMPf:
            action = BCMFP_ACTION_REDIRECT_TO_ECMP;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_REDIRECT_UC_CANCELf:
            action = BCMFP_ACTION_REDIRECT_UC_CANCEL;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NEW_HIGIG_EHf:
            action = BCMFP_ACTION_NEW_HIGIG_EH;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_HIGIG_EH_MASK_PROFILE_IDf:
            action = BCMFP_ACTION_HIGIG_EH_MASK_PROFILE_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_NAT_EGR_OVERRIDEf:
            action = BCMFP_ACTION_NAT_EGRESS_OVERRIDE;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
        case FP_EM_GRP_TEMPLATEt_ACTION_EXACT_MATCH_CLASS_IDf:
            action = BCMFP_ACTION_EXACT_MATCH_CLASS_ID;
            field_type = BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION;
            break;
    default:
            /* Fid is Not Qualifier. */
            SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            break;
    }

    group_map->fid = fid;
    group_map->qual = qual;
    group_map->action = action;
    group_map->field_type  = field_type;

exit:

    SHR_FUNC_EXIT();
}
