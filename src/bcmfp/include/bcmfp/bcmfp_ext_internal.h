/*! \file bcmfp_ext_internal.h
 *
 * Defines, Enums and Structures to represent FP extractors
 * related information.
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

#ifndef BCMFP_EXT_INTERNAL_H
#define BCMFP_EXT_INTERNAL_H

/*
 * Typedef:
 *    BCMFP_EXT_ATTR_XXX
 * Purpose:
 *    Field extractor sections attribute flags.
 */
#define BCMFP_EXT_ATTR_PASS_THRU                           (1 << 0)

#define BCMFP_EXT_ATTR_NOT_IN_EIGHTY_BIT                   (1 << 1)

#define BCMFP_EXT_ATTR_NOT_WITH_IPBM                       (1 << 2)

#define BCMFP_EXT_ATTR_DUPLICATE                           (1 << 3)

#define BCMFP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID           (1 << 4)

#define BCMFP_EXT_ATTR_NOT_WITH_NAT_NEEDED                 (1 << 5)

#define BCMFP_EXT_ATTR_NOT_WITH_DROP                       (1 << 6)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0             (1 << 7)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1             (1 << 8)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_80          (1 << 9)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_80          (1 << 10)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_MODE_128                (1 << 11)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_B           (1 << 12)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_B           (1 << 13)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_C           (1 << 14)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_C           (1 << 15)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_0     (1 << 16)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_1     (1 << 17)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_0 (1 << 18)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_1 (1 << 19)



/* Egress FP possible keys. */
#define EFP_KEY1          (0x1)  /* IPv4 key.      */
#define EFP_KEY2          (0x2)  /* IPv6 key.      */
#define EFP_KEY3          (0x3)  /* IPv6 dw key.   */
#define EFP_KEY4          (0x4)  /* L2 key.        */
#define EFP_KEY5          (0x5)  /* HiGiG Key.     */
#define EFP_KEY6          (0x6)  /* HiGiG Key.     */
#define EFP_KEY7          (0x7)  /* Loopback Key.  */
#define EFP_KEY8          (0x8)  /* Bytes after
                                    L2 Header key. */

/* Egress FP slice v6 key  modes. */
#define  EGR_MODE_SIP6         (0x0)
#define  EGR_MODE_DIP6         (0x1)
#define  EGR_MODE_SIP_DIP_64   (0x2)

/*
 * Typedef:
 *     bcmfp_ingress_ctrl_sel_t
 * Purpose:
 *     Pre-Mux extractor input ext controls.
 */
typedef enum bcmfp_ext_ctrl_sel_e {
    BCMFP_EXT_CTRL_SEL_DISABLE            = 0, /* Selector disabled.                */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_A          = 1, /* Auxiliary Tag A selector.         */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_B          = 2, /* Auxiliary Tag B selector.         */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_C          = 3, /* Auxiliary Tag C selector.         */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_D          = 4, /* Auxiliary Tag D selector.         */
    BCMFP_EXT_CTRL_SEL_TCP_FN             = 5, /* TCP function selector.            */
    BCMFP_EXT_CTRL_SEL_TOS_FN             = 6, /* ToS function selector.            */
    BCMFP_EXT_CTRL_SEL_TTL_FN             = 7, /* TTL function selector.            */
    BCMFP_EXT_CTRL_SEL_CLASSID_A          = 8, /* Class ID container A selector.    */
    BCMFP_EXT_CTRL_SEL_CLASSID_B          = 9, /* Class ID container B selector.    */
    BCMFP_EXT_CTRL_SEL_CLASSID_C          = 10, /* Class ID container C selector.   */
    BCMFP_EXT_CTRL_SEL_CLASSID_D          = 11, /* Class ID container D selector.   */
    BCMFP_EXT_CTRL_SEL_SRC_A              = 12, /* Source Container A selector.     */
    BCMFP_EXT_CTRL_SEL_SRC_B              = 13, /* Source Container B selector.     */
    BCMFP_EXT_CTRL_SEL_SRC_DST_0          = 14, /* Src/Dst Container 0 selector.    */
    BCMFP_EXT_CTRL_SEL_SRC_DST_1          = 15, /* Src/Dst Container 1 selector.    */
    BCMFP_EXT_CTRL_SEL_IPBM_SOURCE        = 16, /* IPBM Source                       */
    BCMFP_EXT_CTRL_SEL_SRC                = 17, /* Source Port type selector.       */
    BCMFP_EXT_CTRL_SEL_IP_HEADER          = 18, /* IP header type(Inner/Outer) selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASSID   = 19, /* EFP KEY1 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASSID   = 20, /* EFP KEY2 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASSID   = 21, /* EFP KEY3 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASSID   = 22, /* EFP KEY4 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASSID   = 23, /* EFP KEY6 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASSID   = 24, /* EFP KEY7 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASSID   = 25, /* EFP KEY8 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP       = 26, /* EFP KEY4 DVP Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP       = 27, /* EFP KEY8 DVP Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY4_MDL       = 28, /* EFP KEY4 MDL Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR        = 29, /* EFP IP6 Address(Src/Dst) Selector. */
    BCMFP_EXT_CTRL_SEL_COUNT              = 30  /* Always Last. Not a usable value. */
} bcmfp_ext_ctrl_sel_t;


#define BCMFP_EXT_CTRL_SEL_DONT_CARE (-1)


/*
 * Typedef:
 *     bcmfp_ingress_ext_section_t
 * Purpose:
 *     Key Generation extractor selector encodings.
 */
typedef enum bcmfp_ext_section_e {
    BCMFP_EXT_SECTION_DISABLE   = 0,  /* Selector disabled.               */
    BCMFP_EXT_SECTION_L1E32     = 1,  /* Level1 32bit extractor.          */
    BCMFP_EXT_SECTION_L1E16     = 2,  /* Level1 16bit extractor.          */
    BCMFP_EXT_SECTION_L1E8      = 3,  /* Level1 8bit extractor.           */
    BCMFP_EXT_SECTION_L1E4      = 4,  /* Level1 4bit extractor.           */
    BCMFP_EXT_SECTION_L1E2      = 5,  /* Level1 2bit extractor.           */
    BCMFP_EXT_SECTION_L2E16     = 6,  /* Level2 16bit extractor.          */
    BCMFP_EXT_SECTION_L2E4      = 7,  /* 104 Passthru bits.               */
    BCMFP_EXT_SECTION_L3E16     = 8,  /* Level3 16bit extractor.          */
    BCMFP_EXT_SECTION_L3E4      = 9,  /* Level3 4bit extractor.           */
    BCMFP_EXT_SECTION_L3E2      = 10, /* Level3 2bit extractor.           */
    BCMFP_EXT_SECTION_L3E1      = 11, /* Level3 16bit extractor.          */
    BCMFP_EXT_SECTION_L4        = 12, /* Level4 output.                   */
    BCMFP_EXT_SECTION_L4A       = 13, /* Level4 Slice A output.           */
    BCMFP_EXT_SECTION_L4B       = 14, /* Level4 Slice B output.           */
    BCMFP_EXT_SECTION_L4C       = 15, /* Level4 Slice C output.           */
    BCMFP_EXT_SECTION_L2AE16    = 16, /* Level2 Slice A 16bit extractor.  */
    BCMFP_EXT_SECTION_L2BE16    = 17, /* Level2 Slice B 16 bit extractor. */
    BCMFP_EXT_SECTION_L2CE16    = 18, /* Level2 Slice C 16 bit extractor. */
    BCMFP_EXT_SECTION_L2AE4     = 19, /* Level2 Slice A 4bit extractor.   */
    BCMFP_EXT_SECTION_L2BE4     = 20, /* Level2 Slice B 4bit extractor.   */
    BCMFP_EXT_SECTION_L2CE4     = 21, /* Level2 Slice C 4bit extractor.   */
    BCMFP_EXT_SECTION_L3AE16    = 22, /* Level3 Slice A 16bit extractor.  */
    BCMFP_EXT_SECTION_L3BE16    = 23, /* Level3 Slice B 16bit extractor.  */
    BCMFP_EXT_SECTION_L3CE16    = 24, /* Level3 Slice C 16bit extractor.  */
    BCMFP_EXT_SECTION_L3AE4     = 25, /* Level3 Slice A 4bit extractor.   */
    BCMFP_EXT_SECTION_L3BE4     = 26, /* Level3 Slice B 4bit extractor.   */
    BCMFP_EXT_SECTION_L3CE4     = 27, /* Level3 Slice C 4bit extractor.   */
    BCMFP_EXT_SECTION_FPF0      = 28, /* FPF0(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF1      = 29, /* FPF1(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF2      = 30, /* FPF2(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF3      = 31, /* FPF3(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF4      = 32, /* FPF4(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_COUNT     = 33  /* Always Last. Not a usable value. */
} bcmfp_ext_section_t;

 /*
 * Typedef:
 *     bcmfp_fwd_entity_sel_t
 * Purpose:
 *     Enumerate Forwarding entity Type selection.
 */
typedef enum bcmfp_fwd_entity_sel_e {
    BCMFP_FWD_ENTITY_SVPGPORT       = 0, /* MPLS gport selection.            */
    BCMFP_FWD_ENTITY_GLP            = 1, /* (MOD/PORT/TRUNK) selection.      */
    BCMFP_FWD_ENTITY_MODPORTGPORT   = 2, /* MOD/PORT even if port is part of
                                          the trunk.                         */
    BCMFP_FWD_ENTITY_PORTGROUPNUM   = 4, /* Ingress port group and number    */
    BCMFP_FWD_ENTITY_COUNT          = 5  /* Always Last. Not a usable value. */
} bcmfp_fwd_entity_sel_t;

#define BCMFP_EXT_L1_E32_SELECTORS_MAX  4
#define BCMFP_EXT_L1_E16_SELECTORS_MAX  7
#define BCMFP_EXT_L1_E8_SELECTORS_MAX   7
#define BCMFP_EXT_L1_E4_SELECTORS_MAX   8
#define BCMFP_EXT_L1_E2_SELECTORS_MAX   8
#define BCMFP_EXT_L2_E16_SELECTORS_MAX  10
#define BCMFP_EXT_L3_E4_SELECTORS_MAX   21
#define BCMFP_EXT_L3_E2_SELECTORS_MAX   5
#define BCMFP_EXT_L3_E1_SELECTORS_MAX   2
#define BCMFP_EXT_PMUX_SELECTORS_MAX    15


#define BCMFP_EXT_SELCODE_DONT_CARE    (-1)
#define BCMFP_EXT_SELCODE_DONT_USE     (-2)

/*
 * Typedef:
 *     bcmfp_ext_sel_t
 * Purpose:
 *     Logical table key generation program control selectors.
 */
typedef struct bcmfp_ext_sel_s {
    int    keygen_index;          /* LT keygen program profile index.     */
    int    ipbm_source;           /* Ingress Port Bitmap Source.          */
    int    ltid;                  /* LTID assigned to the group.          */
    int    key_type;              /* EM key mode                          */
    int8_t default_policy_enable; /* EM Default Policy Enable             */
    int8_t l1_e32_sel[4];         /* Level 1 32b extractors.              */
    int8_t l1_e16_sel[10];        /* Level 1 16b extractors.              */
    int8_t l1_e8_sel[7];          /* Level 1 8b extractors.               */
    int8_t l1_e4_sel[10];         /* Level 1 4b extractors.               */
    int8_t l1_e2_sel[8];          /* Level 1 2b extractors.               */
    int8_t l2_e1_sel[3];          /* Level 2 1b extractors.               */
    int8_t l2_e2_sel[7];          /* Level 2 2b extractors.               */
    int8_t l2_e4_sel[16];         /* Level 2 4b extractors.               */
    int8_t l2_e16_sel[10];        /* Level 2 16b extractors.              */
    int8_t l3_e1_sel[2];          /* Level 3 1b extractors.               */
    int8_t l3_e2_sel[5];          /* Level 3 2b extractors.               */
    int8_t l3_e4_sel[21];         /* Level 3 4b extractors.               */
    int8_t pmux_sel[15];          /* Post muxing extractors.              */
    int8_t intraslice;            /* Intraslice double wide selection.    */
    int8_t secondary;             /* Secondary selector.                  */
    int8_t ipbm_present;          /* Post mux IPBM in final key.          */
    int8_t normalize_l3_l4_sel;   /* Normalize L3 and L4 address.         */
    int8_t normalize_mac_sel;     /* Normalize L2 SA & DA.                */
    int8_t aux_tag_a_sel;         /* Auxiliary Tag A Selector.            */
    int8_t aux_tag_b_sel;         /* Auxiliary Tag B Selector.            */
    int8_t aux_tag_c_sel;         /* Auxiliary Tag C Selector.            */
    int8_t aux_tag_d_sel;         /* Auxiliary Tag d Selector.            */
    int8_t tcp_fn_sel;            /* TCP function selector.               */
    int8_t tos_fn_sel;            /* TOS function selector.               */
    int8_t ttl_fn_sel;            /* TTL function selector.               */
    int8_t class_id_cont_a_sel;   /* Class ID container A selector.       */
    int8_t class_id_cont_b_sel;   /* Class ID container B selector.       */
    int8_t class_id_cont_c_sel;   /* Class ID container C selector.       */
    int8_t class_id_cont_d_sel;   /* Class ID container D selector.       */
    int8_t src_cont_a_sel;        /* SRC container A selector.            */
    int8_t src_cont_b_sel;        /* SRC container B selector.            */
    int8_t src_dest_cont_0_sel;   /* SRC or DST container 0 selector.     */
    int8_t src_dest_cont_1_sel;   /* SRC or DST container 1 selector.     */
    int8_t fpf0;                  /* FPF0 field(s) select.                */
    int8_t fpf1;                  /* FPF1 field(s) select.                */
    int8_t fpf2;                  /* FPF2 field(s) select.                */
    int8_t fpf3;                  /* FPF3 field(s) select.                */
    int8_t fpf4;                  /* FPF4 field(s) select.                */
    int8_t src_sel;               /* Src port/trunk entity selection.     */
    int8_t ip_header_sel;         /* Inner/Outer Ip header selection.     */
    int8_t egr_key1_classid_sel;  /* EFP KEY1 Class ID Selector           */
    int8_t egr_key2_classid_sel;  /* EFP KEY2 Class ID Selector           */
    int8_t egr_key3_classid_sel;  /* EFP KEY3 Class ID Selector           */
    int8_t egr_key4_classid_sel;  /* EFP KEY4 Class ID Selector           */
    int8_t egr_key6_classid_sel;  /* EFP KEY6 Class ID Selector           */
    int8_t egr_key7_classid_sel;  /* EFP KEY7 Class ID Selector           */
    int8_t egr_key8_classid_sel;  /* EFP KEY8 Class ID Selector           */
    int8_t egr_key4_dvp_sel;      /* EFP KEY4 DVP Secondary selector      */
    int8_t egr_key8_dvp_sel;      /* EFP KEY8 DVP Secondary selector      */
    int8_t egr_key4_mdl_sel;      /* EFP KEY4 MDL Secondary selector      */
    int8_t egr_ip_addr_sel;       /* Ip6 address format selection.        */
    int8_t action_profile_idx;    /* Index to action profile table.       */
    int8_t default_action_prof_idx;/* Index to action profile table.      */
    int8_t default_qos_prof_idx;  /* Index to action profile table.       */
    int8_t udf_sel;               /* UDF selector.                        */
    int8_t alt_ttl_fn_sel;        /* ALT TTL function selector            */
} bcmfp_ext_sel_t;

extern char *
bcmfp_ext_section_name(bcmfp_ext_section_t section);

#define IS_EXT_OVERLAID_POST_MUXERS(acl_flags, ext_flags) \
          ((SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_NAT_DST_REALM_ID) &&  \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_NAT_NEEDED) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_NAT_NEEDED)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_FIRST_LOOKUP_HIT) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_0)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_SECOND_LOOKUP_HIT) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_1)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_FIRST_LOOKUP_CLASSID) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_0)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_SECOND_LOOKUP_CLASSID) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_1)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_DROP) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_DROP)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_1_SLICE_B) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_B)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_0_SLICE_B) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_B)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_1_SLICE_C) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_C)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_0_SLICE_C) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_C)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_1) &&  \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_0) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0)))

#define IS_EXT_OVERLAID_IPBM(acl_flags, ext_flags) \
                   (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_IPBM) && \
                    (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_IPBM))

#endif /* BCMFP_EXT_INTERNAL_H */
