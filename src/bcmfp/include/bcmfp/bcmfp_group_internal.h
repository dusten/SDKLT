/*! \file bcmfp_group_internal.h
 *
 * Defines, Enums and Structures to represent FP groups
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

#ifndef BCMFP_GROUP_INTERNAL_H
#define BCMFP_GROUP_INTERNAL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_ext_internal.h>
#include <bcmfp/bcmfp_pmux_internal.h>
#include <bcmfp/bcmfp_entry_internal.h>
#include <bcmfp/bcmfp_qual_cfg_internal.h>

/*
 * Group status flags.
 */

/* Group resides in a single slice. */
#define BCMFP_GROUP_SPAN_SINGLE_SLICE          (1 << 0)

/* Group resides in a two paired slices. */
#define BCMFP_GROUP_SPAN_DOUBLE_SLICE          (1 << 1)

/* Group resides in three paired slices. */
#define BCMFP_GROUP_SPAN_TRIPLE_SLICE          (1 << 2)

/* Group entries are double wide in each slice. */
#define BCMFP_GROUP_INTRASLICE_DOUBLEWIDE      (1 << 3)

/* Intra-slice occupies only one part in slice. */
#define BCMFP_GROUP_INTRASLICE_SPAN_ONE_PART   (1 << 4)

typedef uint8_t bcmfp_part_id_t;

/* GROUP with default policy */
#define BCMFP_GROUP_WITH_DEFAULT_POLICY        (1 << 5)

/* GROUP with default presel */
#define BCMFP_GROUP_WITH_DEFAULT_PRESEL        (1 << 6)

typedef uint32_t bcmfp_group_id_t;

typedef enum bcmfp_group_mode_e {

    BCMFP_GROUP_MODE_SINGLE = 0,

    BCMFP_GROUP_MODE_SINGLE_ASET_NARROW,

    BCMFP_GROUP_MODE_SINGLE_ASET_WIDE,

    BCMFP_GROUP_MODE_DBLINTRA,

    BCMFP_GROUP_MODE_DBLINTER,

    BCMFP_GROUP_MODE_TRIPLE,

    BCMFP_GROUP_MODE_QUAD,

    BCMFP_GROUP_MODE_AUTO,

    BCMFP_GROUP_MODE_COUNT

} bcmfp_group_mode_t;


typedef struct bcmfp_group_mode_bmp_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_GROUP_MODE_COUNT)];
} bcmfp_group_mode_bmp_t;

typedef enum bcmfp_group_slice_mode_e {

    BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE        = 0,

    BCMFP_GROUP_SLICE_MODE_L3_SINGLE_WIDE        = 1,

    BCMFP_GROUP_SLICE_MODE_L3_DOUBLE_WIDE        = 2,

    BCMFP_GROUP_SLICE_MODE_L3_ANY_SINGLE_WIDE    = 3,

    BCMFP_GROUP_SLICE_MODE_L3_ANY_DOUBLE_WIDE    = 4,

    BCMFP_GROUP_SLICE_MODE_L3_ALT_DOUBLE_WIDE    = 5,

    BCMFP_GROUP_SLICE_MODE_COUNT                 = 6

} bcmfp_group_slice_mode_t;

typedef enum bcmfp_group_type_e {

    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_ANY         = 0,

    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV4        = 1,

    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IPV6        = 2,

    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_IP          = 3,

    BCMFP_GROUP_TYPE_PORT_ANY_PACKET_NONIP       = 4,

    BCMFP_GROUP_TYPE_PORT_HIGIG_PACKET_ANY       = 5,

    BCMFP_GROUP_TYPE_PORT_LOOPBACK_PACKET_ANY    = 6,

    BCMFP_GROUP_TYPE_PORT_FRONT_PACKET_ANY       = 7,

    BCMFP_GROUP_TYPE_COUNT                       = 8

} bcmfp_group_type_t;

/*
 * Typedef:
 *     bcmfp_qual_offset_info_t
 * Purpose:
 *     Qualifier offsets in FP tcam.
 */
typedef struct bcmfp_qual_offset_info_s {

    uint8_t       num_offsets;                    /* Number of Offsets.      */

    uint16_t      offset[BCMFP_QUAL_OFFSETS_MAX]; /* Qualifier offset.       */

    uint8_t       width[BCMFP_QUAL_OFFSETS_MAX];  /* Qualifier offset width. */

} bcmfp_qual_offset_info_t;

/*
 * Typedef:
 *     bcmfp_group_qual_offset_info_s
 * Purpose:
 *     Field entry part qualifiers information.
 */
typedef struct bcmfp_group_qual_offset_info_s {

    bcmfp_qual_t             *qid_arr;    /* Qualifier id.           */

    bcmfp_qual_offset_info_t *offset_arr; /* Qualifier tcam offsets. */

    uint16_t                 size;        /* Qualifier array size.   */

} bcmfp_group_qual_offset_info_t;

/*! */
#define BCMFP_GROUP_ACTION_RES_IDS_MAX 32

#define BCMFP_GROUP_AUTO_LTID (BCMFP_GROUP_ACTION_RES_IDS_MAX + 1)

typedef struct bcmfp_group_action_res_id_bmp_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_GROUP_ACTION_RES_IDS_MAX)];
} bcmfp_group_action_res_id_bmp_t;

typedef struct bcmfp_group_s {

    uint32_t flags;

    bcmltd_sid_t ltid;

    uint32_t stage_id;

    bcmfp_group_id_t group_id;

    bcmfp_group_mode_t group_mode;

    bcmfp_entry_type_bmp_t entry_type_bmp;

    bcmfp_group_slice_mode_t group_slice_mode;

    bcmfp_group_type_t  group_port_pkt_type;

    uint32_t group_prio;

    uint8_t action_res_id;

    uint8_t num_presel_ids;

    int pipe_id;

    uint8_t lookup_id;

    bcmfp_qset_t qset;

    bcmfp_qual_bitmap_t *qual_bitmap;

    bcmfp_ext_sel_t ext_codes[BCMFP_ENTRY_PARTS_MAX];

    bcmfp_group_qual_offset_info_t qual_arr[BCMFP_ENTRY_TYPE_COUNT]
                                           [BCMFP_ENTRY_PARTS_MAX];

    bcmfp_pmux_info_t *pmux_info;

    uint8_t num_pmux_qual;

} bcmfp_group_t;

#define BCMFP_GROUP_MODE_TO_BCMPTM_GROUP_MODE(bcmfp_group_mode,       \
                                              bcmptm_group_mode)      \
     do {                                                             \
         if (bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE ||           \
          bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW ||  \
          bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {    \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;      \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_DBLINTRA) {   \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA;    \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_DBLINTER) {   \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_DBLINTER;    \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_TRIPLE) {     \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_TRIPLE;      \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_QUAD) {       \
             bcmptm_group_mode = BCMPTM_RM_TCAM_GRP_MODE_QUAD;        \
         } else {                                                     \
             SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);                \
         }                                                            \
     } while (0)

#define BCMFP_EM_GROUP_MODE_TO_BCMPTM_EM_GROUP_MODE(bcmfp_group_mode,       \
                                              bcmptm_group_mode)            \
     do {                                                                   \
         if (bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {     \
             bcmptm_group_mode = BCMPTM_RM_HASH_ENTRY_MODE_EM_160;          \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) { \
             bcmptm_group_mode = BCMPTM_RM_HASH_ENTRY_MODE_EM_128;          \
         } else if(bcmfp_group_mode == BCMFP_GROUP_MODE_DBLINTER) {         \
             bcmptm_group_mode = BCMPTM_RM_HASH_ENTRY_MODE_EM_320;          \
         } else {                                                           \
             SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);                      \
         }                                                                  \
     } while (0)

#endif /* BCMFP_GROUP_INTERNAL_H */
