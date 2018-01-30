/*! \file bcmptm_rm_tcam_fp.h
 *
 *  APIs, defines for PTRM-TCAM interfaces
 *  This file contains APIs, defines for PTRM-TCAM interfaces
 *  for FP based TCAMs
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

#ifndef BCMPTM_RM_TCAM_FP_H
#define BCMPTM_RM_TCAM_FP_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmlrd_ptrm.h> 
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>

/*******************************************************************************
 * Defines
 */
/*! Number of words for Flags Bitmap*/
#define BCMPTM_FP_FLAGS_BMP_WORDS (BCMPTM_FP_FLAGS_COUNT / 32) + 1
/* ! Max number of parts count for a group */
#define BCMPTM_TCAM_FP_MAX_PARTS  (4)

/* FP STAGE FLAGS */
#define BCMPTM_STAGE_KEY_TYPE_PRESEL            (1 << 0)
#define BCMPTM_STAGE_KEY_TYPE_SELCODE           (1 << 1)
#define BCMPTM_STAGE_KEY_TYPE_FIXED             (1 << 2)
#define BCMPTM_STAGE_INTRASLICE_CAPABLE         (1 << 3)
#define BCMPTM_STAGE_AUTOEXPANSION              (1 << 4)
#define BCMPTM_STAGE_INTRASLICE_SPAN_ONE_PART   (1 << 5)
#define BCMPTM_STAGE_ENTRY_INDEX_REMAP          (1 << 6)
#define BCMPTM_STAGE_ENTRY_TYPE_HASH            (1 << 7)
#define BCMPTM_STAGE_MULTI_VIEW_TCAM            (1 << 8)

/*******************************************************************************
 * Typedefs
 */

/*! Types of Flags */
typedef enum bcmptm_fp_flags_s {

    /*! To indicate that the stage supports preselection */
    BCMPTM_FP_FLAGS_PRESEL_STAGE_SUPPORT,

    /*! To indicate its a preselection entry */
    BCMPTM_FP_FLAGS_PRESEL_ENTRY,

    /*! To indicate its a default preselection entry */
    BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT,

    /*! To indicate the last entry in a group */
    BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP,

    /*! Total count of flags */
    BCMPTM_FP_FLAGS_COUNT,

} bcmptm_fp_flags_t;

/*! Flags bitmap structure */
typedef struct bcmptm_fp_flags_bmp_s {
    SHR_BITDCL w[BCMPTM_FP_FLAGS_BMP_WORDS];
} bcmptm_fp_flags_bmp_t;

/*! Stages supported */
typedef enum  bcmptm_rm_tcam_fp_stage_id_e {
    /*! IFP stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_INGRESS,
    /*! EFP stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_EGRESS,
    /*! VFP Stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_LOOKUP,
    /*! EM stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_EXACT_MATCH,
    /*! Stage count */
    BCMPTM_RM_TCAM_FP_STAGE_ID_COUNT,
} bcmptm_rm_tcam_fp_stage_id_t;

/*! To pass selcode from FP to PTM */
typedef struct bcmptm_rm_tcam_fp_group_selcode_s {
    int8_t intraslice;            /* Intraslice double wide selection.    */
    int8_t fpf2;                  /* FPF2 field(s) select.                */
    int8_t fpf3;                  /* FPF3 field(s) select.                */
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
} bcmptm_rm_tcam_fp_group_selcode_t;

/*! This structure is passed from FP to PTM on
 * every INSERT/DELETE/UPDATE operations of
 * entries in FP_INGRESS_ENTRY table. FP will
 * pass this as a void pointer to PTM and FP
 * subcomponent in PTM module will typecast
 * the void pointer to this structure type.
 */
typedef struct bcmptm_rm_tcam_entry_attrs_s {


       /*! BCMPTM_FP_XXX flags */
       bcmptm_fp_flags_bmp_t  flags;

       /*! Group Id to which the entry belongs to. */
       uint32_t group_id;

       /*! priority of the group_id.This information
        * is required only when first entry in the group
        * is created.
        */
       uint32_t group_prio;


       /*! ltid  of the group. This information
        * is required to resolve the action priority when
        * conflicting actions are processed when multiple
        * rules are to hit from different groups
        */
       uint8_t group_ltid;

       /*! Mode(SINGLE/DBLINTRA/DBLINTER/TRIPLE/QUAD) of
        * the group.This information is
        *  required only when first entry in the group is created.
        */
       bcmptm_rm_tcam_group_mode_t group_mode;


       /*! Number of preset Ids associated with group_id.
       * This information is required only when first
       * entry in the group is created.
       */
       uint32_t num_presel_ids;

       /*! Stage Id(INGRESS/EGRESS/VLAN) to which the group belongs to. */
       bcmptm_rm_tcam_fp_stage_id_t stage_id;

       /*! Pipe Id(Global pipe (or) pipe 0 (or) Pipe 1 etc.)
        * to which the group belongs to.
        */
       int pipe_id;

       /* ! Selcode associated with the group_id.
        */
       bcmptm_rm_tcam_fp_group_selcode_t selcode[BCMPTM_TCAM_FP_MAX_PARTS];

       /* ! Slice id of the group id.
        */
       uint8_t slice_id;

} bcmptm_rm_tcam_entry_attrs_t;


/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Get number of groups per pipe supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe id of type int8_t.
 * \param [in] grp_mode of type bcmptm_rm_tcam_group_mode_t.
 * \param [out] num_groups_per_pipe Number of groups per pipe.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_max_groups_per_mode_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  bcmptm_rm_tcam_group_mode_t grp_mode,
                                  uint32_t *num_groups_per_mode);
/*!
 * \brief Get number of groups per pipe supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe id of type int8_t.
 * \param [out] num_groups_per_pipe Number of groups per pipe.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint32_t *num_groups_per_pipe);

/*!
 * \brief Get number of presel entries per slice supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe id of type int8_t.
 * \param [in] slice_id Slice Number
 * \param [out] num_presel_entries_per_slice Number of presel entries per slice.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_presel_entries_per_slice_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint8_t slice_id,
                                  uint32_t *num_presel_entries_per_slice);
/*!
 * \brief Get number of entries per slice supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] slice_id Slice Number
 * \param [in] pipe_id Pipe id of type int8_t.
 * \param [in] grp_mode of type bcmptm_rm_tcam_group_mode_t.
 * \param [out] num_entries_per_slice Number of entries per slice.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_entries_per_slice_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t slice_id,
                                  int pipe_id,
                                  bcmptm_rm_tcam_group_mode_t grp_mode,
                                  uint32_t *num_entries_per_slice);
/*!
 * \brief Get start and end physical index for a given slice.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] slice_id Slice Number
 * \param [in] pipe_id Pipe id of type int8_t.
 * \param [in] grp_mode of type bcmptm_rm_tcam_group_mode_t.
 * \param [out] start_index Start physical index of a slice.
 * \param [out] end_index End physical index of a slice.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_slice_start_end_index_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t slice_id,
                                  int pipe_id,
                                  uint32_t *start_index,
                                  uint32_t *end_index);
/*!
 * \brief Get number of slices supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe id of type int8_t.
 * \param [out] num_slices Number of slices.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_slices_count_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint8_t *num_slices);
/*!
 * \brief Get number of pipes supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] num_pipes Number of pipes.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_pipes_count_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *num_pipes);
/*!
 * \brief To get the max action resolution IDs supported by given stage
 *
 * \param [in] unit BCM device number.
 * \param [in] ltid Logical Table Enum
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] pipe_id pipe number.
 * \param [out] num_action_res_ids number of action resolution IDs
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_max_action_resolution_ids_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  int pipe_id,
                                  uint32_t *num_action_res_ids);
/*!
 * \brief To get the max entries that can be created for the given group
 * considering auto expansion if auto expansion flag is true
 *
 * \param [in] unit BCM device number.
 * \param [in] ltid Logical Table Enum
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] pipe_id  pipe number.
 * \param [in] group_id group ID.
 * \param [in] auto_exp 1 -- Auto expansion considered
 *                       0 -- Auto expansion not considered
 * \param [out]  max_entries_per_group number of entries that can be created
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_max_entries_per_group_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  bcmptm_rm_tcam_entry_attrs_t *entry_attrs,
                                  bool auto_expansion,
                                  uint32_t *max_entries_per_group);

#endif /* BCMPTM_RM_TCAM_FP_H */
