/*! \file bcmfp_qual_internal.h
 *
 * Enumerations for all qualifiers supported on all FP stages of all devices.
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

#ifndef BCMFP_QUAL_INTERNAL_H
#define BCMFP_QUAL_INTERNAL_H

#include <shr/shr_bitop.h>

/* All the different qualifiers supported on the device. */
typedef enum bcmfp_qual_s {
    BCMFP_QUAL_DST_MAC = 0,

    BCMFP_QUAL_SRC_MAC,

    BCMFP_QUAL_DST_IP4,

    BCMFP_QUAL_SRC_IP4,

    BCMFP_QUAL_SRC_IP6,

    BCMFP_QUAL_SRC_IP6_UPPER,

    BCMFP_QUAL_SRC_IP6_LOWER,

    BCMFP_QUAL_DST_IP6,

    BCMFP_QUAL_DST_IP6_UPPER,

    BCMFP_QUAL_DST_IP6_LOWER,

    BCMFP_QUAL_OUTER_VLAN_ID,

    BCMFP_QUAL_OUTER_VLAN_CFI,

    BCMFP_QUAL_OUTER_VLAN_PRIORITY,

    BCMFP_QUAL_INNER_VLAN_ID,

    BCMFP_QUAL_INNER_VLAN_CFI,

    BCMFP_QUAL_INNER_VLAN_PRIORITY,

    BCMFP_QUAL_ETHERTYPE,

    BCMFP_QUAL_L4DST_PORT,

    BCMFP_QUAL_L4SRC_PORT,

    BCMFP_QUAL_MIM_ISID,

    BCMFP_QUAL_NVGRE_VSID,

    BCMFP_QUAL_VXLAN_VNID,

    BCMFP_QUAL_VXLAN_FLAGS,

    BCMFP_QUAL_VXLAN_RSVD_24,

    BCMFP_QUAL_VXLAN_RSVD_8,

    BCMFP_QUAL_ETAG,

    BCMFP_QUAL_VNTAG,

    BCMFP_QUAL_CNTAG,

    BCMFP_QUAL_HG_OPCODE,

    BCMFP_QUAL_MPLS_FWD_LABEL_ID,

    BCMFP_QUAL_MPLS_FWD_LABEL_TTL,

    BCMFP_QUAL_MPLS_FWD_LABEL_EXP,

    BCMFP_QUAL_MPLS_FWD_LABEL_BOS,

    BCMFP_QUAL_MPLS_CTRL_WORD,

    BCMFP_QUAL_IP6_FLOW_LABEL,

    BCMFP_QUAL_TTL,

    BCMFP_QUAL_TOS,

    BCMFP_QUAL_IP_PROTOCOL,

    BCMFP_QUAL_TCP_CONTROL,

    BCMFP_QUAL_EM_GROUP_CLASS_ID_0,

    BCMFP_QUAL_EM_GROUP_CLASS_ID_1,

    BCMFP_QUAL_PRE_LOGICAL_TBL_ID,

    BCMFP_QUAL_IP_FIRST_EH_PROTO,

    BCMFP_QUAL_IP_SECOND_EH_PROTO,

    BCMFP_QUAL_IP_FIRST_EH_SUBCODE,

    BCMFP_QUAL_IP_FLAGS_MF,

    BCMFP_QUAL_IP_FLAGS_DF,

    BCMFP_QUAL_HIGIG_PKT,

    BCMFP_QUAL_DROP_PKT,

    BCMFP_QUAL_VISIBILITY_PKT,

    BCMFP_QUAL_DOSATTACK_PKT,

    BCMFP_QUAL_L4_PKT,

    BCMFP_QUAL_MIRROR_PKT,

    BCMFP_QUAL_IP_FRAG,

    BCMFP_QUAL_NON_FRAGMENT_FIRST_FRAGMENT,

    BCMFP_QUAL_ICMP_ERROR_PKT,

    BCMFP_QUAL_REPLICATION_PKT,

    BCMFP_QUAL_L2SRC_HIT,

    BCMFP_QUAL_L2DST_HIT,

    BCMFP_QUAL_L3SRC_HOST_HIT,

    BCMFP_QUAL_L3DST_HOST_HIT,

    BCMFP_QUAL_L3DST_LPM_HIT,

    BCMFP_QUAL_L3_TUNNEL_HIT,

    BCMFP_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT,

    BCMFP_QUAL_MPLSENTRY_SECONDLOOKUP_HIT,

    BCMFP_QUAL_MYSTATIONTCAM_HIT,

    BCMFP_QUAL_L2SRC_STATIC,

    BCMFP_QUAL_L2STATION_MOVE,

    BCMFP_QUAL_DEST_IP_LOCAL,

    BCMFP_QUAL_L3_ROUTABLE_PKT,

    BCMFP_QUAL_HIGIGLOOKUP_PKT,

    BCMFP_QUAL_VXLT_LOOKUP_HIT,

    BCMFP_QUAL_L2CACHE_HIT,

    BCMFP_QUAL_SYSTEM_PORT_CLASS,

    BCMFP_QUAL_L3_INTF_CLASS,

    BCMFP_QUAL_SRC_VP_CLASS,

    BCMFP_QUAL_VLAN_CLASS,

    BCMFP_QUAL_FP_VLAN_CLASS0,

    BCMFP_QUAL_FP_VLAN_CLASS1,

    BCMFP_QUAL_PORT_FP_VLAN_GROUPID,

    BCMFP_QUAL_PORT_FP_ING_KEYSELECT_CLASS,

    BCMFP_QUAL_UDF_CLASS,

    BCMFP_QUAL_FP_ING_CLASSID_TYPE,

    BCMFP_QUAL_L2SRC_CLASS,

    BCMFP_QUAL_L2DEST_CLASS,

    BCMFP_QUAL_L3SRC_CLASS,

    BCMFP_QUAL_L3DEST_CLASS,

    BCMFP_QUAL_FP_ING_CLASSID,

    BCMFP_QUAL_PRESEL_CLASS,

    BCMFP_QUAL_FWD_VLAN_VALID,

    BCMFP_QUAL_INGRESS_STP_STATE,

    BCMFP_QUAL_SRC_MODULE,

    BCMFP_QUAL_SRC_PORT,

    BCMFP_QUAL_SRC_TRUNK,

    BCMFP_QUAL_SRC_VP,

    BCMFP_QUAL_SRC_VP_VALID,

    BCMFP_QUAL_DST_MODULE,

    BCMFP_QUAL_DST_PORT,

    BCMFP_QUAL_DST_TRUNK,

    BCMFP_QUAL_DST_VP,

    BCMFP_QUAL_DST_VP_TRUNK,

    BCMFP_QUAL_DST_VP_VALID,

    BCMFP_QUAL_DST_NHOP,

    BCMFP_QUAL_DST_OVERLAY_ECMP,

    BCMFP_QUAL_DST_UNDERLAY_ECMP,

    BCMFP_QUAL_DST_L3_MC_GROUP,

    BCMFP_QUAL_DST_L2_MC_GROUP,

    BCMFP_QUAL_FWD_VLAN_ID,

    BCMFP_QUAL_VRF,

    BCMFP_QUAL_VPN,

    BCMFP_QUAL_L3_IIF,

    BCMFP_QUAL_INPORT,

    BCMFP_QUAL_INPORTS,

    BCMFP_QUAL_SYSTEM_PORTS,

    BCMFP_QUAL_DEVICE_PORTS,

    BCMFP_QUAL_INT_PRI,

    BCMFP_QUAL_COLOR,

    BCMFP_QUAL_NAT_SRC_REALM_ID,

    BCMFP_QUAL_NAT_INT_CN,

    BCMFP_QUAL_MACADDR_NORMALIZE,

    BCMFP_QUAL_IPADDR_NORMALIZE,

    BCMFP_QUAL_BYTES_AFTER_L2HEADER,

    BCMFP_QUAL_BFD_DISCRIMINATOR,

    BCMFP_QUAL_PKT_LENGTH,

    BCMFP_QUAL_MPLS_FORWARDING_LABEL_ACTION,

    BCMFP_QUAL_LOOPBACK_TYPE,

    BCMFP_QUAL_TUNNEL_TYPE,

    BCMFP_QUAL_LOOPBACK,

    BCMFP_QUAL_PKT_RES,

    BCMFP_QUAL_EM_FIRST_LOOKUP_CLASSID,

    BCMFP_QUAL_EM_SECOND_LOOKUP_CLASSID,

    BCMFP_QUAL_EM_FIRST_LOOKUP_LTID,

    BCMFP_QUAL_EM_SECOND_LOOKUP_LTID,

    BCMFP_QUAL_L2_FORMAT,

    BCMFP_QUAL_INNER_TPID,

    BCMFP_QUAL_OUTER_TPID,

    BCMFP_QUAL_FWD_TYPE,

    BCMFP_QUAL_MPLS_BOS_TERMINATED,

    BCMFP_QUAL_RAL_PRESENT,

    BCMFP_QUAL_GAL_PRESENT,

    BCMFP_QUAL_MPLS_CW_VALID,

    BCMFP_QUAL_IP_CHECKSUM_VALID,

    BCMFP_QUAL_VLAN_INNER_PRESENT,

    BCMFP_QUAL_VLAN_OUTER_PRESENT,

    BCMFP_QUAL_VLAN_TRANSLATED_INNER_PRESENT,

    BCMFP_QUAL_VLAN_TRANSLATED_OUTER_PRESENT,

    BCMFP_QUAL_CFG_FOUR_OFFSET_ADD,

    BCMFP_QUAL_RANGE_CHECKID_BMP,

    BCMFP_QUAL_L3_TYPE,

    BCMFP_QUAL_C_IP_PROTOCOL,

    BCMFP_QUAL_C_L4SRC_PORT,

    BCMFP_QUAL_C_L4DST_PORT,

    BCMFP_QUAL_C_ETHERTYPE,

    BCMFP_QUAL_C_TTL0,

    BCMFP_QUAL_C_TTL1,

    BCMFP_QUAL_C_TOS0,

    BCMFP_QUAL_C_TOS1,

    BCMFP_QUAL_C_TCP_CONTROL0,

    BCMFP_QUAL_C_TCP_CONTROL1,

    BCMFP_QUAL_IP_TUNNEL_TTL,

    BCMFP_QUAL_IP_TUNNEL_CLASS_0,

    BCMFP_QUAL_C_SRC_IP,

    BCMFP_QUAL_C_DST_IP,

    BCMFP_QUAL_C_SRC_IP6,

    BCMFP_QUAL_C_DST_IP6,

    BCMFP_QUAL_MIXED_SRC_CLASS,

    BCMFP_QUAL_INNER_IP_PROTO_COMMON,

    BCMFP_QUAL_IP_PROTO_COMMON,

    BCMFP_QUAL_TUNNEL_TERMINATED,

    BCMFP_QUAL_OUTER_VLAN_PRI,

    BCMFP_QUAL_INNER_L3_TYPE,

    BCMFP_QUAL_SYSTEM_L4_PKT,

    BCMFP_QUAL_INNER_TTL,

    BCMFP_QUAL_INNER_IP_FLAGS_MF,

    BCMFP_QUAL_INNER_IP_FLAGS_DF,

    BCMFP_QUAL_INNER_TOS,

    BCMFP_QUAL_INNER_L4DST_PORT,

    BCMFP_QUAL_INNER_L4SRC_PORT,

    BCMFP_QUAL_ICMP_TYPE_CODE,

    BCMFP_QUAL_INNER_IP_PROTOCOL,

    BCMFP_QUAL_INNER_DST_IP4,

    BCMFP_QUAL_INNER_SRC_IP4,

    BCMFP_QUAL_INNER_SRC_IP6,

    BCMFP_QUAL_INNER_SRC_IP6_UPPER,

    BCMFP_QUAL_INNER_SRC_IP6_LOWER,

    BCMFP_QUAL_INNER_DST_IP6,

    BCMFP_QUAL_INNER_DST_IP6_UPPER,

    BCMFP_QUAL_INNER_DST_IP6_LOWER,

    BCMFP_QUAL_INNER_SRC_IP6_HIGH,

    BCMFP_QUAL_SRC_IP6_HIGH,

    BCMFP_QUAL_INNER_DST_IP6_HIGH,

    BCMFP_QUAL_DST_IP6_HIGH,

    BCMFP_QUAL_SNAP_HEADER,

    BCMFP_QUAL_LLC_HEADER,

    BCMFP_QUAL_FIBRE_INNER_HEADER_TYPE,

    BCMFP_QUAL_FIBRE_OUTER_HEADER_TYPE,

    BCMFP_QUAL_FCOE_SOF,

    BCMFP_QUAL_FIBRE_DATA_FIELD_CTRL,

    BCMFP_QUAL_FIBRE_CLASS_SPEC_CTRL,

    BCMFP_QUAL_FIBRE_FRAME_CTRL,

    BCMFP_QUAL_FIBRE_TYPE,

    BCMFP_QUAL_FIBRE_DST_ID,

    BCMFP_QUAL_FIBRE_SRC_ID,

    BCMFP_QUAL_FIBRE_ROUTING_CTRL,

    BCMFP_QUAL_FIBRE_VFT_HOP_COUNT,

    BCMFP_QUAL_FIBRE_VFT_VSAN_ID,

    BCMFP_QUAL_FIBRE_VFT_VSAN_PRI,

    BCMFP_QUAL_FIBRE_VFT_VERSION,

    BCMFP_QUAL_NAT_NEEDED,

    BCMFP_QUAL_NAT_DST_REALM_ID,

    BCMFP_QUAL_EM_FIRST_LOOKUP_HIT,

    BCMFP_QUAL_EM_SECOND_LOOKUP_HIT,

    BCMFP_QUAL_UDF1_CHUNK0,

    BCMFP_QUAL_UDF1_CHUNK1,

    BCMFP_QUAL_UDF1_CHUNK2,

    BCMFP_QUAL_UDF1_CHUNK3,

    BCMFP_QUAL_UDF1_CHUNK4,

    BCMFP_QUAL_UDF1_CHUNK5,

    BCMFP_QUAL_UDF1_CHUNK6,

    BCMFP_QUAL_UDF1_CHUNK7,

    BCMFP_QUAL_UDF2_CHUNK0,

    BCMFP_QUAL_UDF2_CHUNK1,

    BCMFP_QUAL_UDF2_CHUNK2,

    BCMFP_QUAL_UDF2_CHUNK3,

    BCMFP_QUAL_UDF2_CHUNK4,

    BCMFP_QUAL_UDF2_CHUNK5,

    BCMFP_QUAL_UDF2_CHUNK6,

    BCMFP_QUAL_UDF2_CHUNK7,

    BCMFP_QUAL_EGR_NHOP_CLASS_ID,

    BCMFP_QUAL_EGR_L3_INTF_CLASS_ID,

    BCMFP_QUAL_EGR_DVP_CLASS_ID,

    BCMFP_QUAL_HG_LOOKUP_PKT,

    BCMFP_QUAL_HG_PKT,

    BCMFP_QUAL_INNER_VLAN_PRI,

    BCMFP_QUAL_MIRR_COPY,

    BCMFP_QUAL_DST_HG,

    BCMFP_QUAL_OUTPORT,

    BCMFP_QUAL_CPU_COS,

    BCMFP_QUAL_INT_CN,

    BCMFP_QUAL_IP_PROTO,

    BCMFP_QUAL_FABRIC_QUE_TAG,

    BCMFP_QUAL_HG_DST_MC,

    BCMFP_QUAL_HG_DST_MC_GROUP_ID,

    BCMFP_QUAL_HG_DST_TRAFFIC_CLASS,

    BCMFP_QUAL_HG_DST_DST_MOD,

    BCMFP_QUAL_HG_SRC_DST_MOD,

    BCMFP_QUAL_HG_LOAD_BALANCE_ID,

    BCMFP_QUAL_HG_COLOR,

    BCMFP_QUAL_HG_EXT_HEADER_TAG,

    BCMFP_QUAL_HG_INT_CN_NOTIFY,

    BCMFP_QUAL_HG_MIRR_SWITCH_PKT,

    BCMFP_QUAL_HG_DST_TRUNK,

    BCMFP_QUAL_HG_DST_TRUNK_ID,

    BCMFP_QUAL_HG_ING_L3_SWITCH_PACKET,

    BCMFP_QUAL_HG_LABEL_TYPE,

    BCMFP_QUAL_HG_MPLS_PKT,

    BCMFP_QUAL_HG_LABEL,

    BCMFP_QUAL_HG_REPLICATION_ID,

    BCMFP_QUAL_HG_VLAN,

    BCMFP_QUAL_HG_FILTERING_MODE,

    BCMFP_QUAL_HG_SRC_TRUNK,

    BCMFP_QUAL_HG_PRESERVE_FLAGS,

    BCMFP_QUAL_HG_ING_CLASS_FLAG,

    BCMFP_QUAL_HG_EGR_MULTICAST,

    BCMFP_QUAL_HG_FWD_TYPE,

    BCMFP_QUAL_HG_VNI,

    BCMFP_QUAL_HG_MC_INDEX,

    BCMFP_QUAL_HG_VP_REPLICATION_ID,

    BCMFP_QUAL_HG_DONOT_FLAGS,

    BCMFP_QUAL_HG_VP_LAG_FAIL_OVER_PKT,

    BCMFP_QUAL_HG_PROTECTION_SW_STATUS,

    BCMFP_QUAL_HG_MIRR_TO_VP,

    BCMFP_QUAL_HG_VP_PRESERVE_FLAGS,

    BCMFP_QUAL_HG_DST_TYPE,

    BCMFP_QUAL_HG_SRC_TYPE,

    BCMFP_QUAL_HG_OFLD_ENG_CLASS_TAG,

    BCMFP_QUAL_HG_OFLD_ENG_DEF_FLG,

    BCMFP_QUAL_HG_OFLD_ENG_VXLT_STATUS,

    BCMFP_QUAL_HG_OFLD_ENG_PKT_PRI_NEW,

    BCMFP_QUAL_HG_OFLD_ENG_DSCP_NEW,

    BCMFP_QUAL_HG_OFLD_ENG_PRESERVE_DSCP,

    BCMFP_QUAL_HG_OFLD_ENG_PRESERVE_DOT1P,

    BCMFP_QUAL_HG_OFLD_ENG_SRC_TYPE,

    BCMFP_QUAL_LOOPBACK_QUEUE,

    BCMFP_QUAL_PKT_IS_VISIBLE,

    BCMFP_QUAL_LOOPBACK_CPU_MSQRD_PKT_PROF,

    BCMFP_QUAL_LOOPBACK_COLOR,

    BCMFP_QUAL_LOOPBACK_TRAFFIC_CLASS,

    BCMFP_QUAL_LOOPBACK_PKT_PROCESSING_PORT,

    BCMFP_QUAL_COUNT

} bcmfp_qual_t;

typedef struct bcmfp_qset_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_QUAL_COUNT)];
} bcmfp_qset_t;

#define BCMFP_QSET_INIT(qset)  \
    sal_memset(&(qset), 0, sizeof(bcmfp_qset_t))

#define BCMFP_QSET_ADD(qset, q)  SHR_BITSET(((qset).w), (q))

#define BCMFP_QSET_REMOVE(qset, q)  SHR_BITCLR(((qset).w), (q))

#define BCMFP_QSET_TEST(qset, q)  SHR_BITGET(((qset).w), (q))

#define BCMFP_QSET_COPY(to_qset, from_qset) \
    sal_memcpy((to_qset).w, (from_qset).w, sizeof (bcmfp_qset_t))

#define BCMFP_QSET_PBMP_TEST(qset) (BCMFP_QSET_TEST(qset, BCMFP_QUAL_INPORTS))

/*
 * Typedef:
 *    BCMFP_QUAL_POST_MUX_XXX
 * Purpose:
 *    Post muxed field qualifiers flags.
 */
#define BCMFP_QUAL_POST_MUX_IPBM                       (1 << 0)

#define BCMFP_QUAL_POST_MUX_NAT_DST_REALM_ID           (1 << 1)

#define BCMFP_QUAL_POST_MUX_NAT_NEEDED                 (1 << 2)

#define BCMFP_QUAL_POST_MUX_DROP                       (1 << 3)

#define BCMFP_QUAL_POST_MUX_SRC_DST_CONT_0             (1 << 4)

#define BCMFP_QUAL_POST_MUX_SRC_DST_CONT_1             (1 << 5)

#define BCMFP_QUAL_POST_MUX_SRC_DST_CONT_0_SLI_B       (1 << 6)

#define BCMFP_QUAL_POST_MUX_SRC_DST_CONT_1_SLI_B       (1 << 7)

#define BCMFP_QUAL_POST_MUX_SRC_DST_CONT_0_SLI_C       (1 << 8)

#define BCMFP_QUAL_POST_MUX_SRC_DST_CONT_1_SLI_C       (1 << 9)

#define BCMFP_QUAL_POST_MUX_EM_HIT_STATUS_LOOKUP_0     (1 << 10)

#define BCMFP_QUAL_POST_MUX_EM_HIT_STATUS_LOOKUP_1     (1 << 11)

#define BCMFP_QUAL_POST_MUX_EM_ACTION_CLASSID_LOOKUP_0 (1 << 12)

#define BCMFP_QUAL_POST_MUX_EM_ACTION_CLASSID_LOOKUP_1 (1 << 13)

/*
 * Field Qualifier IpProtocolCommon (BCMFP_QUAL_IP_PROTO_COMMON and
 * BCMFP_QUAL_INNER_IP_PROTO_COM)
 */
typedef enum bcmfp_ipprotocolcommon_e {
    BCMFP_IP_PROTO_COMMON_TCP = 0,         /* TCP Packets (6) */
    BCMFP_IP_PROTO_COMMON_UDP = 1,         /* UDP Packets (17) */
    BCMFP_IP_PROTO_COMMON_IGMP = 2,        /* IGMP Packets (2) */
    BCMFP_IP_PROTO_COMMON_ICMP = 3,        /* ICMP Packets (1) */
    BCMFP_IP_PROTO_COMMON_IP6ICMP = 4,     /* IPv6 ICMP Packets (58) */
    BCMFP_IP_PROTO_COMMON_IP6HOPBYHOP = 5, /* IPv6 Hop-by-Hop Packets (0) */
    BCMFP_IP_PROTO_COMMON_IPINIP = 6,      /* IPv4 Payload Packets (4) */
    BCMFP_IP_PROTO_COMMON_TCPUDP = 7,      /* TCP and UDP Packets (6 & 17) */
    BCMFP_IP_PROTO_COMMON_UNKNOWN = 8,     /* Not-standard IP Protocol. */
    BCMFP_IP_PROTO_COMMON_IP6INIP = 9,     /* IPv6 (0x29) */
    BCMFP_IP_PROTO_COMMON_MPLSINIP = 10,   /* MPLS (0x89) */
    BCMFP_IP_PROTO_COMMON_COUNT = 11       /* Always Last. Not a usable value.*/
} bcmfp_ipprotocolcommon_t;

/*
 * Field Qualifier FibreChannel (for BCMFP_QUAL_FIBRE_OUTER_HEADER_TYPE and
 * BCMFP_QUAL_FIBRE_INNER_HEADER_TYPE).
 */
typedef enum bcmfp_fibrechan_e {
    BCMFP_FIBRECHANANY = 0,           /* Don't care. */
    BCMFP_FIBRECHAN = 1,              /* Standard Fibre Channel header. */
    BCMFP_FIBRECHANENCAP = 2,         /* Encapsulation (ENC) header. */
    BCMFP_FIBRECHANVIRTUAL = 3,       /* Virtual fabric tag (VFT) header. */
    BCMFP_FIBRECHANROUTED = 4,        /* Interfabric routing (IFR) header. */
    BCMFP_FIBRECHANCOUNT = 5          /* Always Last. Not a usable value. */
} bcmfp_fibrechan_t;

/* Field Qualifier IpType (BCMFP_QUAL_L3_TYPE). */
typedef enum bcmfp_iptype_e {
    BCMFP_IPTYPE_ANY = 0,              /* Don't care. */
    BCMFP_IPTYPE_NONIP = 1,            /* Non-Ip packet. */
    BCMFP_IPTYPE_IPV4NOOPTS = 2,       /* IPv4 without options. */
    BCMFP_IPTYPE_IPV4WITHOPTS = 3,     /* IPv4 with options. */
    BCMFP_IPTYPE_IPV4ANY = 4,          /* Any IPv4 packet. */
    BCMFP_IPTYPE_IPV6NOEXTHDR = 5,     /* IPv6 packet without any extension
                                           header. */
    BCMFP_IPTYPE_IPV6ONEEXTHDR = 6,    /* IPv6 packet with one extension
                                           header. */
    BCMFP_IPTYPE_IPV6TWOEXTHDR = 7,    /* IPv6 packet with two or more
                                           extension headers. */
    BCMFP_IPTYPE_IPV6_ANY = 8,         /* Any IPv6 packet. */
    BCMFP_IPTYPE_IP = 9,               /* IPv4 and IPv6 packets. */
    BCMFP_IPTYPE_ARP = 10,             /* ARP/RARP. */
    BCMFP_IPTYPE_ARPREQUEST = 11,      /* ARP Request. */
    BCMFP_IPTYPE_ARPREPLY = 12,        /* ARP Reply. */
    BCMFP_IPTYPE_MPLSUNICAST = 13,     /* Mpls unicast frame (ethertype =
                                           0x8847). */
    BCMFP_IPTYPE_MPLSMULTICAST = 14,   /* Mpls mcast frame   (ethertype =
                                           0x8848). */
    BCMFP_IPTYPE_TRILL = 15,           /* Trill packet. */
    BCMFP_IPTYPE_MIM = 16,             /* Mac-in-Mac frame. */
    BCMFP_IPTYPE_FCOE = 17,            /* Fiber Channel Packets (0x8906). */
    BCMFP_IPTYPE_COUNT = 18            /* Always Last. Not a usable value. */
} bcmfp_iptype_t;


/* Packet colors. */
#define BCMFP_COLOR_PRESERVE    0
#define BCMFP_COLOR_GREEN       1
#define BCMFP_COLOR_YELLOW      2
#define BCMFP_COLOR_RED         3
#define BCMFP_COLOR_BLACK       4


/*
 * Field Qualifier L2Format (for BCMFP_QUAL_L2_FORMAT).
 * Used to qualify packets based on L2 header format.
 */
typedef enum bcmfp_l2format_e {
    BCMFP_L2_FORMAT_ANY = 0,            /* Do not qualify on L2 format. */
    BCMFP_L2_FORMAT_ETHII = 1,          /* Ethernet 2 (802.2). */
    BCMFP_L2_FORMAT_SNAP = 2,           /* Sub-Network Access Protocol (SNAP). */
    BCMFP_L2_FORMAT_LLC = 3,            /* Logical Link Control. */
    BCMFP_L2_FORMAT_802DOT3 = 4,        /* 802.3 frame format. */
    BCMFP_L2_FORMAT_SNAPPRIVATE = 5,    /* Sub-Network Access Protocol (SNAP).
                                           Vendor specific protocol. */
    BCMFP_L2_FORMAT_MIM = 6,            /* MAC-In-MAC. */
    BCMFP_L2_FORMAT_PPPOE = 7,          /* PPPoE frame. */
    BCMFP_L2_FORMAT_COUNT = 8           /* Always Last. Not a usable value. */
} bcmfp_l2format_t;


/*
 * Field Qualifier Forwarding Type (for BCMFP_QUAL_FWD_TYPE)
 */
typedef enum bcmfp_forwardingtype_e {
    BCMFP_FORWARDING_TYPE_ANY = 0,                  /* Don't care. */
    BCMFP_FORWARDING_TYPE_L2 = 1,                   /* L2 switching forwarding. */
    BCMFP_FORWARDING_TYPE_L3 = 2,                   /* L3 routing forwarding. */
    BCMFP_FORWARDING_TYPE_L2SHARED = 3,             /* Shared vlan L2 forwarding. */
    BCMFP_FORWARDING_TYPE_L2INDEPENDENT = 4,        /* Vlan based L2 forwarding. */
    BCMFP_FORWARDING_TYPE_L2VPN = 5,                /* VFI based L2 forwarding. */
    BCMFP_FORWARDING_TYPE_L2VPNDIRECT = 6,          /* L2 point to point vpn forwarding. */
    BCMFP_FORWARDING_TYPE_L3DIRECT = 7,             /* L3 predefined next hop forwarding. */
    BCMFP_FORWARDING_TYPE_IP4UCAST = 8,             /* IPv4 Unicast Routing forwarding. */
    BCMFP_FORWARDING_TYPE_IP4MCAST = 9,             /* IPv4 Multicast Routing forwarding. */
    BCMFP_FORWARDING_TYPE_IP6UCAST = 10,            /* IPv6 Unicast Routing forwarding. */
    BCMFP_FORWARDING_TYPE_IP6MCAST = 11,            /* IPv6 Multicast Routing forwarding. */
    BCMFP_FORWARDING_TYPE_MPLS = 12,                /* MPLS Switching forwarding. */
    BCMFP_FORWARDING_TYPE_TRILL = 13,               /* Trill forwarding. */
    BCMFP_FORWARDING_TYPE_RXREASON = 14,            /* Forwarding according to a RxReason. */
    BCMFP_FORWARDING_TYPE_TRAFFICMANAGEMENT = 15,   /* Traffic Management forwarding, when
                                                     an external Packet Processor sets the
                                                     forwarding decision. */
    BCMFP_FORWARDING_TYPE_SNOOP = 16,               /* Snooped packet. */
    BCMFP_FORWARDING_TYPE_MPLSLABEL1 = 17,          /* MPLS Switching forwarding according
                                                       to the 1st label */
    BCMFP_FORWARDING_TYPE_MPLSLABEL2 = 18,          /* MPLS Switching forwarding according
                                                       to the 2nd label */
    BCMFP_FORWARDING_TYPE_MPLSLABEL3 = 19,          /* MPLS Switching forwarding according
                                                       to the 3rd label */
    BCMFP_FORWARDING_TYPE_FCOE = 20,                /* Fiber Channel over Ethernet
                                                    forwarding. */
    BCMFP_FORWARDING_TYPE_PORTEXTENDER = 21,        /* VNTAG/ETAG based forwarding. */
    BCMFP_FORWARDING_TYPE_COUNT = 22                /* Always Last. Not a usable value. */
} bcmfp_forwardingtype_t;


/* Field Qualifier IpFrag */
typedef enum bcmfp_ipfrag_e {
    BCMFP_IPFRAG_NON = 0,          /* Non-fragmented packet. */
    BCMFP_IPFRAG_FIRST = 1,        /* First fragment of fragmented packet. */
    BCMFP_IPFRAG_NONORFIRST = 2,   /* Non-fragmented or first fragment. */
    BCMFP_IPFRAG_NOTFIRST = 3,     /* Not the first fragment. */
    BCMFP_IPFRAG_ANY = 4,          /* Any fragment of fragmented packet. */
    BCMFP_IPFRAG_COUNT = 5         /* Always last. Not a usable value. */
} bcmfp_ipfrag_t;

/*Field Qualifier for TPID*/
typedef enum bcmfp_tpid_e {
    BCMFP_TPID_8100 = 0,     /* TPID is 0x8100. */
    BCMFP_TPID_9100 = 1,     /* TPID is 0x9100. */
    BCMFP_TPID_88A8 = 2,     /* TPID is 0x88a8. */
    BCMFP_TPID_OTHER = 3      /* TPID is different than Above Options. */
} bcmfp_tpid_t;

/* Field qualifier for STP state (for BCMFP_QUAL_INGRESS_STP_STATE )*/
typedef enum bcmfp_stg_stp_e {
    BCMFP_STG_STP_DISABLE = 0,
    BCMFP_STG_STP_BLOCK = 1,
    BCMFP_STG_STP_LISTEN = 2,
    BCMFP_STG_STP_LEARN = 3,
    BCMFP_STG_STP_FORWARD = 4,
    BCMFP_STG_STP_LEARN_FORWARD = 5,
    BCMFP_STG_STP_LEARN_DISABLE = 6,
    BCMFP_STG_STP_FORWARD_BLOCK = 7,
    BCMFP_STG_STP_DISABLE_BLOCK = 8,
    BCMFP_STG_STP_COUNT = 9
} bcmfp_stg_stp_t;

/* Field qualifier for VLAN translation look up hit (for
 * BCMFP_QUAL_VXLT_LOOKUP_HIT )*/
typedef enum bcmfp_vxlt_lookup_hit_e {
    BCMFP_VXLT_NO_HIT              = 0, /* VXLT Lookup Not Hit. */
    BCMFP_VXLT_FIRST_LOOKUP_HIT    = 1, /* First VXLT Lookup Hit. */
    BCMFP_VXLT_SECOND_LOOKUP_HIT   = 2, /* Second VXLT Lookup Hit. */
    BCMFP_VXLT_ANY_LOOKUP_HIT      = 3  /* First or Second VXLT Lookup Hit. */
} bcmfp_vxlt_lookup_hit_t;

/* Field qualified for mpls forwarding label action
 * (for BCMFP_QUAL_MPLS_FORWARDING_LABEL_ACTION )*/
typedef enum bcmfp_mpls_fwd_label_action_e {
    BCMFP_MPLS_ACT_INVALID             = 0, /* MPLS Forward Label Action is Invalid. */
    BCMFP_MPLS_ACT_PHP                 = 1, /* MPLS Forward Label Action is PHP. */
    BCMFP_MPLS_ACT_SWAP                = 2, /* MPLS Forward Label Action is Swap. */
    BCMFP_MPLS_ACT_POP                 = 3, /*POP the MPLS label */
    BCMFP_MPLS_ACT_POP_USE_L2_VPN      = 4, /* POP the MPLS label and use L2 VPN. */
    BCMFP_MPLS_ACT_POP_USE_L3_VPN      = 5, /* POP the MPLS label and use L3 VPN. */
    BCMFP_MPLS_ACT_POP_USE_ENTROPY     = 6  /* POP the MPLS label and use Entropy. */
} bcmfp_mpls_fwd_label_action_t;

/* Field qualifier for Loopback type (for BCMFP_QUAL_LOOPBACK_TYPE) */
typedef enum bcmfp_loopback_type_e {
    BCMFP_LB_TYPE_ANY                 = 0,  /* Don't care. */
    BCMFP_LB_TYPE_MIM                 = 1,  /* Mac in Mac frames. */
    BCMFP_LB_TYPE_TRILL_NETWORK       = 2,  /* Trill network port loopback. */
    BCMFP_LB_TYPE_TRILL_ACCESS        = 3,  /* Trill access port loopback. */
    BCMFP_LB_TYPE_L2GRE               = 4,  /* L2 GRE loopback packet. */
    BCMFP_LB_TYPE_VXLAN               = 5   /* Vxlan loopback packet. */
} bcmfp_loopback_type_t;

/* Field qualifier for Tunnel type (for BCMFP_QUAL_LOOPBACK_TYPE) */
typedef enum bcmfp_tunnel_type_e {
    BCMFP_TNL_TYPE_ANY                 = 0,  /* Don't care. */
    BCMFP_TNL_TYPE_IP                  = 1,  /* IP in IP, Istap, GRE. */
    BCMFP_TNL_TYPE_MPLS                = 2,  /* MPLS. */
    BCMFP_TNL_TYPE_MIM                 = 3,  /* Mac in Mac. */
    BCMFP_TNL_TYPE_AUTO_IP4_MC         = 4,  /* IPV4 Automatic multicast. */
    BCMFP_TNL_TYPE_TRILL               = 5,  /* Trill. */
    BCMFP_TNL_TYPE_L2GRE               = 6,  /* L2 GRE. */
    BCMFP_TNL_TYPE_VXLAN               = 7,  /* Vxlan Tunnel Packet. */
    BCMFP_TNL_TYPE_NONE                = 8   /* L2 termination. */
} bcmfp_tunnel_type_t;

/* Field qualifier for packet resolution (for BCMFP_QUAL_PKT_RES ) */
typedef enum bcmfp_pkt_res_e {
    BCMFP_UNKNOWN_PKT             = 0,      /* Unknown packet. */
    BCMFP_CONTROL_PKT             = 1,      /* Ethernet Control (8808) packets. */
    BCMFP_BPDU_PKT                = 2,      /* L2 user entry table BPDU bit. */
    BCMFP_L2BC_PKT                = 3,      /* L2 broadcast packet. */
    BCMFP_L2UC_PKT                = 4,      /* L2 Unicast Packet. */
    BCMFP_UNKNOWN_L2_PKT          = 5,      /* UnKnown L2 packet. */
    BCMFP_UNKNOWN_L3MC_PKT        = 6,      /* Unknown L3 Multicast Packet. */
    BCMFP_KNOWN_L3MC_PKT          = 7,      /* Known L3 Multicast Packet. */
    BCMFP_KNOWN_L2MC_PKT          = 8,      /* Known L2 Multicast Packet. */
    BCMFP_UNKNOWN_L2MC_PKT        = 9,      /* Unknown L2 Multicast Packet. */
    BCMFP_KNOWN_L3UC_PKT          = 10,     /* Known L3 Unicast Packet. */
    BCMFP_UNKNOWN_L3UC_PKT        = 11,     /* Unknown L3 Unicast Packet. */
    BCMFP_KNOWN_MPLS_PKT          = 12,     /* Known Mpls Packet. */
    BCMFP_KNOWN_MPLS_MC_PKT       = 13,     /* Known Mpls Multicast Packet. */
    BCMFP_KNOWN_MPLS_L3_PKT       = 14,     /* Known Mpls L3 Packet. */
    BCMFP_KNOWN_MPLS_L2_PKT       = 15,     /* Known Mpls L2 Packet. */
    BCMFP_UNKNOWN_MPLS_PKT        = 16,     /* Unknown Mpls Packet. */
    BCMFP_KNOWN_MIM_PKT           = 17,     /* Point to point MIM terminated packet. */
    BCMFP_UNKNOWN_MIM_PKT         = 18,     /* MIM tunnel error packet. */
    BCMFP_KNOWN_TRILL_PKT         = 19,     /* Known egress rbridge TRILL transit packet. */
    BCMFP_UNKNOWN_TRILL_PKT       = 20,     /* Unknown egress rbridge TRILL packet. */
    BCMFP_KNOWN_NIV_PKT           = 21,     /* Known destination vif downstream NIV packet. */
    BCMFP_UNKNOWN_NIV_PKT         = 22,     /* Unknown destination vif downstream NIV packet. */
    BCMFP_OAM_PKT                 = 23,     /* OAM packet that needs to be terminated locally. */
    BCMFP_BFD_PKT                 = 24,     /* BFD packet that needs to be terminated locally. */
    BCMFP_ICNM_PKT                = 25,     /* ICNM Packet. */
    BCMFP_IEEE1588_PKT            = 26,     /* 1588 message. */
    BCMFP_KNOWN_L2GRE_PKT         = 27,     /* Known L2GRE packet. */
    BCMFP_KNOWN_VXLAN_PKT         = 28,     /* Known VXLAN packet */
    BCMFP_KNOWN_FCOE_PKT          = 29,     /* Know FCOE Packet. */
    BCMFP_UNKNOWN_FCOE_PKT        = 30,     /* Unknown FCOE packet. */
    BCMFP_UNKNOWN_PIM_BIDIR_PKT   = 31     /* Unknown PIM Bidirectional Packet. */
} bcmfp_pkt_res_t;


/* Field qualifier for IFP Class Id type (for BCMFP_QUAL_FP_ING_CLASSID_TYPE) */
typedef enum bcmfp_ifp_class_id_type_e {
    BCMFP_ING_INTF_PORT_CLASSID   = 1, /* Source port based Class Id. */
    BCMFP_ING_INTF_VPORT_CLASSID  = 2, /* Source virtual port based Class Id. */
    BCMFP_L3_INTF_CLASS           = 3, /* L3_IIF based Class Id. */
    BCMFP_FP_VLAN_CLASS1          = 4, /* VFP upper Class Id. */
    BCMFP_FP_VLAN_CLASS0          = 5, /* VFP lower Class Id. */
    BCMFP_L2SRC_CLASS             = 6, /* L2 source based Class Id. */
    BCMFP_L2DEST_CLASS            = 7, /* L2 destination based Class Id. */
    BCMFP_L3SRC_CLASS             = 8, /* L3 source based Class Id. */
    BCMFP_L3DEST_CLASS            = 9, /* L3 destination based Class Id. */
    BCMFP_VLAN_CLASS              = 10, /* VLAN based Class Id. */
    BCMFP_ING_CLASSID             = 15 /* IFP based Class Id. */
} bcmfp_ifp_class_id_type_e;

#endif /* BCMFP_QUAL_INTERNAL_H */
