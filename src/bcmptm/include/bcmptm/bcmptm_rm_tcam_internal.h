/*! \file bcmptm_rm_tcam_internal.h
 *
 * APIs, defines for PTRM-Tcam interfaces
 * This file contains APIs, defines for PTRM-Tcam interfaces
 * for priority only and priority with entry id based TCAMs
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

#ifndef BCMPTM_RM_TCAM_INTERNAL_H
#define BCMPTM_RM_TCAM_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmlrd_ptrm.h> 
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm.h>

/*******************************************************************************
 * Defines
 */
/*! TCAM has Aggregated view. */
#define BCMPTM_RM_TCAM_F_AGGR_VIEW      (1 << 0)

/*! TCAM hash TCAM only view. */
#define BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW (1 << 1)

/*! TCAM has Data only view. */
#define BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW (1 << 2)

/*! TCAM has auto expansion support */
#define BCMPTM_RM_TCAM_F_AUTO_EXPANSION (1 << 3)

/*! TCAM has default presel entry */
#define BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT (1 << 4)

/*! TCAM has Depth wise expansion support */
#define BCMPTM_RM_TCAM_F_DEPTH_EXPANSION (1 << 5)

/*! TCAM has width wise expansion support */
#define BCMPTM_RM_TCAM_F_WIDTH_EXPANSION (1 << 6)

/*! Maximum number of TCAMs to be paired on system. */
#define BCMPTM_RM_TCAM_MAX_WIDTH_PARTS 4

/*! Identifies global pipe for TCAMs */
#define BCMPTM_RM_TCAM_GLOBAL_PIPE -1

/*! RM TCAM register/ memory operation flags */
/*! Operation selcode install */
#define BCMPTM_RM_TCAM_OPER_SELCODE_INSTALL          (1 << 0)
/*! Operation selcode copy */
#define BCMPTM_RM_TCAM_OPER_SELCODE_COPY             (1 << 1)
/*! Operation selcode reset */
#define BCMPTM_RM_TCAM_OPER_SELCODE_RESET            (1 << 2)
/*! Operation slice enable */
#define BCMPTM_RM_TCAM_OPER_SLICE_DISABLE            (1 << 3)
/*! Operation slice disable */
#define BCMPTM_RM_TCAM_OPER_SLICE_ENABLE             (1 << 4)
/*! Operation action priority update */
#define BCMPTM_RM_TCAM_OPER_ACTION_PRIO_UPDATE       (1 << 5)
/*! Operation partition priority update */
#define BCMPTM_RM_TCAM_OPER_PARTITION_PRIO_UPDATE    (1 << 6)
/*! Operation slice map update */
#define BCMPTM_RM_TCAM_OPER_SLICE_MAP_UPDATE         (1 << 7)

/*******************************************************************************
 * Typedefs
 */
/*! Group Modes supported */
typedef enum  bcmptm_rm_tcam_group_mode_e {
    /*! Group Mode Single */
    BCMPTM_RM_TCAM_GRP_MODE_SINGLE,
    /*! Group Mode Intra Slice Double */
    BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA,
    /*! Group Mode Inter Slice Double */
    BCMPTM_RM_TCAM_GRP_MODE_DBLINTER,
    /*! Group Mode Triple */
    BCMPTM_RM_TCAM_GRP_MODE_TRIPLE,
    /*! Group Mode Quad */
    BCMPTM_RM_TCAM_GRP_MODE_QUAD,
    /*! Group Mode Count */
    BCMPTM_RM_TCAM_GRP_MODE_COUNT,
} bcmptm_rm_tcam_group_mode_t;

/*! Sid Enums for all internal table/register operations */
typedef enum bcmptm_rm_tcam_sid_e {
    /*! IFP_LOGICAL_TABLE_ACTION_PRIORITYm */
    BCMPTM_RM_TCAM_IFP_LOGICAL_TABLE_ACTION_PRIORITY,
    /*! IFP_LOGICAL_TABLE_CONFIGr */
    BCMPTM_RM_TCAM_IFP_LOGICAL_TABLE_CONFIG,
    /*! IFP_CONFIGr */
    BCMPTM_RM_TCAM_IFP_CONFIG,
    /*! EFP_SLICE_MAPr */
    BCMPTM_RM_TCAM_EFP_SLICE_MAP,
    /*! EFP_SLICE_CONTROLr */
    BCMPTM_RM_TCAM_EFP_SLICE_CONTROL,
    /*! EFP_CLASSID_SELECTORr */
    BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR,
    /*! EFP_KEY4_DVP_SELECTORr */
    BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR,
    /*! EFP_KEY8_DVP_SELECTORr */
    BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR,
    /*! EFP_KEY4_MDL_SELECTORr */
    BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR,
    /*! VFP_SLICE_MAPr */
    BCMPTM_RM_TCAM_VFP_SLICE_MAP,
    /*! VFP_SLICE_CONTROLr */
    BCMPTM_RM_TCAM_VFP_SLICE_CONTROL,
    /*! VFP_KEY_CONTROL_1r */
    BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1,
    /*! VFP_KEY_CONTROL_2r */
    BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2,
    /*! Count */
    BCMPTM_RM_TCAM_SID_COUNT

} bcmptm_rm_tcam_sid_t;

/*! Transaction states for RM tcam */
typedef enum {
    /*! Idle state */
    BCMPTM_RM_TCAM_STATE_IDLE = 0,
    /*! Uncommited changes in Active DB */
    BCMPTM_RM_TCAM_STATE_UC_A = 1,
    /*! Commiting changes. i/e. copying Active to Backup */
    BCMPTM_RM_TCAM_STATE_COPY_A_TO_B = 2,
    /*! Aborting changes. i/e. copying Backup to Active */
    BCMPTM_RM_TCAM_STATE_COPY_B_TO_A = 3
} bcmptm_rm_tcam_trans_state_t;

/*! This structure is passed to chip functions
 *  to update the internal tables/registers
 */
typedef struct bcmptm_rm_tcam_sid_info_s {
    /*! Instance of memory or register */
    int tbl_inst;
    /*! Logical Table action priority.      */
    uint8_t *lt_action_pri;
    /*! Logical Table action Pri count */
    uint16_t lt_action_pri_count;
    /*! Logical Group ID */
    uint8_t *group_ltid;
    /*! Logical Table to Physical slice mapping. */
    uint32_t lt_part_map;
    /*! Logical table partition priority.   */
    uint8_t *lt_part_pri;
    /*! Number of slices */
    uint8_t num_slices;
    /*! Intraslice group */
    bool intraslice;
    /*! Pairing enabled */
    bool pairing;
    /*! Count of parts */
    uint8_t parts_count;
    /*! Primary Slice number
     * Used in case of copy from primary slice.
     */
    uint8_t pri_slice_id;
    /*! Slice number */
    uint8_t slice_id;
    /*! Selcode associated with the group */
    void *selcode;
    /*! Stage Flags */
    uint32_t stage_flags;
    /*! Group Mode */
    uint32_t group_mode;
    /*! Operation flags */
    uint32_t oper_flags;
    /*! Slice group id for SER mode */
    uint32_t slice_group_id;

} bcmptm_rm_tcam_sid_info_t;

/*! \brief TCAM related information to know the details of slices allocated to LTID.
  - It gives information if a TCAM is turned on or off.
  - The valid range of indexex that can be used.
 */
typedef struct bcmptm_rm_tcam_slice_info_s {

/*! \brief Slice Number for the TCAM management.
  */
    uint8_t slice_id;

/*! \brief This indicates if the slice_id is enabled or not for the LTID.
  - some TCAMs may be turned off for power saving.
  */
    bool valid;

/*! \brief First valid Index in the Slice.
  */
    uint32_t start_index;

/*! brief Last Valid Index in the Slice.
  */
    uint32_t end_index;

} bcmptm_rm_tcam_slice_info_t;


/*! \brief Place holder for more info as needed by rsrc_mgr from LRD
  - This contains data which is needed for rsrc_mgr apart from generic data.
  - The current details are more specific to FP TCAMs.
 */
typedef struct bcmptm_rm_tcam_more_info_s {

/*! \brief Flags to hold the details of the TCAM mgmt.
  - This includes various feature supports and entry types.
 */
    uint32_t flags;

/*! \brief Stage Name for the current TCAM.
  - In order to set the right properties, we need to know the stage name.
  - Parameters like entry count varies based on mode per stage.
 */
    uint8_t stage_id;

/*! \brief Number of pipes in TCAM.
  - This information is typically coming from DRD.
  - This is a placeholder for current use.
  */
    uint8_t pipe_count;

/*! \brief HW atomicity support in TCAM.
  - This information is used during entry update.
  */
    uint8_t hw_atomicity_support;

/*! \brief Array to hold the number of slices in the TCAM
  - Number of rows = pipe_count.
  */
    const uint8_t *slice_count;

/*! \brief Array to hold number of logical groups in TCAM.
  - This is currently used only by FP TCAM.
  - Number of rows = pipe_count.
  */
    const uint32_t *group_count;

/*! \brief Number of TCAM physical entries in every slice of TCAM.
  - This an array of arrays.
  - number of rows = pipe_count
  - number of columns = slice_count
  - Typically, entry count is same for all slices, but not always.
  */
    const uint32_t **entry_count;

/*! \brief Number of presel entries in every slice of TCAM.
  - This an array of arrays.
  - number of rows = pipe_count
  - number of columns = slice_count
  - Typically, entry count is same for all slices, but not always.
  */
    const uint32_t **presel_count;

/*! \brief Number of bundles for the TCAM grouping in H/W.
  - In tomahawk, TCAMs are grouped into bundles in IFP.
  - This field holds the bundle size for the TCAM type.
 */
    uint16_t bundle_size;

/*! \brief Number of action_resolution ids possible
     for virtual slice groups.
  - This is a feature specific to FP TCAMs.
  */
   uint16_t num_action_res_ids;

/*! /brief Array to hold the slice information
  - This gives details of slice numbers in the TCAM.
  - It also gives the valid indexes in the TCAM.
  */
    const bcmptm_rm_tcam_slice_info_t* slice_info;

/*! \brief Number of LT's sharing same TCAM in H/W.
 */
    uint8_t shared_lt_count;

/*! /brief Array to hold the share LT's information
  - This gives list of LT's sharing same TCAM.
  */
    const bcmdrd_sid_t* shared_lt_info;

/*! /brief Array to hold the main LT's information
  - This gives main LT for this same TCAM.
  */
    const bcmdrd_sid_t* main_lt_info;

}bcmptm_rm_tcam_more_info_t;

/*! \brief PT-specific info (SID, KEY_TYPE, DATA_TYPE, etc) for Tcam type LT
  - For some devices, when we have flex keygen, we can have KEY_TYPE fields.
  - To be generic, I have taken the approach that KEY_TYPE field will also be
    written, inserted by PTRM (and not LTM) and hence its value and position is
    part of this struct.
  */
typedef struct bcmptm_rm_tcam_hw_entry_info_s {

/*! \brief num of ekw rows that LTM is providing for key_width expansion.
  - This value is per hw_entry_info format supported for one LTID.

  - Aggr_Tcam type LTs
    -# Must be 1 - we cannot support key_width expansion for these.

  - Non_Aggr_Tcam type LTs
    -# Can be greater than 1 to support key_width expansion
    -# Each row of ekw can map to diff SID (each IFP slice can have
       separate SID)
  */
    uint8_t num_key_rows;

/*! \brief num of edw rows that LTM is providing for data_width expansion.
  - This value is per hw_entry_info format supported for one LTID.

  - Aggr_Tcam type LTs
    -# Must be 1 - we cannot support data_width expansion for these.

  - Non_Aggr_Tcam type LTs
    -# Can be greater than 1 to support data_width expansion
    -# Each row of edw can map to diff SID (each fp_policy inst can
       have separate SID)
  */
    uint8_t num_data_rows;

/*! \brief Distinguish Non_Aggr, Aggr type LTs
  - 1 means Non_Aggr type view
  - 0 means Aggr type view
  */
    bool non_aggr;

/*! \brief Mode of the group
  - FP and LPM TCAMS will have various modes as
    per bcmptm_rm_tcam_group_mode_t.
  - Other TCAMS will have mode as BCMPTM_RM_TCAM_GRP_MODE_SINGLE which
    is like the default state.
  */
    uint16_t group_mode;

/*! \brief Number of rows in **sid
  - See details for **sid */
    uint8_t num_depth_inst;

/*! \brief entry boundary in **sid
  - See details for **sid */
    uint8_t entry_boundary;

/*! \brief SID for TCAM+DATA views or TCAM_ONLY views.
  - Array of arrays needed to support width, depth expansion of key
  - num_ekw specifies num columns in each row - for width_expansion
  - num_depth_inst specifies num of rows (r) - for depth expansion

  - For Aggr_Tcam type LTs
    -# each sid corresponds to TCAM+DATA view
    -# there can be no width_expansion - number of columns is 'num_ekw=1'
    -# there can be depth_expansion - number of rows is 'num_depth_inst'

  - For Non_Aggr_Tcam type LTs
    -# each sid corresponds to TCAM_ONLY (key) view
    -# there can be key_width expansion - number of columns is 'num_ekw'
    -# there can be key_depth_expansion - number of rows is 'num_depth_inst'
    -# For key_width_only_expansion case, ekw[N] corresponds sid(0, N)
    -# For key_depth_only_expansion case, ekw[0] corresponds sid(r, 0) where r is
       the depth instance num
  */
    const bcmdrd_sid_t **sid;

/*! \brief SID for TDATA_ONLY views.
  - Meaningful only for Aggr_Tcam type LTs
  - 'non_aggr' flag is part of bcmptm_rm_tcam_req_t
  - Array of arrays needed to support width, depth expansion of policy_data
  - num_edw specifies num columns in each row - for width_expansion
  - num_depth_inst specifies num of rows (r) - for depth expansion

  - For Aggr_Tcam type LTs
    -# **sid_data_only is list of TDATA_ONLY views.
    -# there can be no width_expansion - number of columns is 'num_ekw=1'
    -# there can be depth_expansion - number of rows is 'num_depth_inst'
    -# This can be used by rsrc_mgr to do 'Insert-replace' - because for some
       chips, HW has atomicity issue and so write to aggr view for existing entry
       may result in entry to disappear momentarily - so it is better to write to
       DATA_ONLY view of found entry and do 'Insert-replace'.

  - For Non_Aggr_Tcam type LTs
    -# each sid_data_only corresponds to TDATA_ONLY view
    -# there can be data_width expansion - number of columns is 'num_edw'
    -# there can be data_depth_expansion - number of rows is 'num_depth_inst'
       (because for every TCAM rule we have SRAM row and thus num_sid_data_only
        = num_depth_inst)
    -# For data_width_only_expansion case, edw[N] corresponds sid_data_only(0, N)
    -# For data_depth_only_expansion case, edw[0] corresponds sid_data_only(r, 0)
       where r is the depth instance num
  */
    const bcmdrd_sid_t **sid_data_only;

/*! \brief Array to hold values of KEY_TYPEf to be used for each SID
  - Array is needed for devices where PT in each block may be assigned a diff
    value.
  - Array of array is needed to support key_width, key_depth expansion
  - For Aggr_Tcam type LTs
    -# Each element corresponds to element in **sid
  - For Non_Aggr_Tcam type LTs
    -# Each element corresponds to element in **sid
  */
    const uint16_t **key_type_val;

/*! \brief HW details of KEY_TYPEf
  - For Aggr_Tcam type LTs
    -# Same for all underlying TCAM+DATA views

  - For Non_Aggr_Tcam type LTs
    -# Same for all underlying TCAM_ONLY views

  - NULL means underlying PTs don't have KEY_TYPEf. */
    const bcmlrd_hw_rfield_info_t *hw_ktype_field_info;

/*! \brief width of DATA_TYPEf
  - For Aggr_Tcam type LTs
    -# NOT AN ARRAY - Same for all underlying TCAM+DATA views

  - For Non_Aggr_Tcam type LTs
    -# NOT AN ARRAY - Same for all underlying TDATA_ONLY views

  - 0 means underlying PTs don't have DATA_TYPEf */
    uint16_t hw_dtype_field_width;

/*! \brief start_bit position of DATA_TYPEf
  - For Aggr_Tcam type LTs
    -# NOT AN ARRAY - Same for all underlying TCAM+DATA views

  - For Non_Aggr_Tcam type LTs
    -# NOT AN ARRAY - Same for all underlying TDATA_ONLY views

  - DontCare if DATA_TYPEf does not exist */
    uint16_t hw_dtype_field_start_bit;

/*! \brief Start bit for 'data portion' in Tcam view.
  - Meaningful for all Tcam type LTs.
  - Used by PTRM to be able to create data for TDATA_ONLY view from TCAM+DATA
    view (for 'Insert-Replace' case) */
    uint16_t data_start_bit;

/*! \brief End bit for 'data portion' in Tcam view.
  - Meaningful for all Tcam type LTs.
  - Used by PTRM to be able to create data for TDATA_ONLY view from TCAM+DATA
    view (for 'Insert-Replace' case) */
    const uint16_t *data_end_bit;

/*! \brief Size in bits of 'Key portion' in TCAM view.
  - Meaningful for all Tcam type LTs.
  - Used by resource manager to know the size of  TCAM_ONLY portion in bits.
  - Pointer is not required because all width expanded TCAM_ONLY views will have
    same key width. */
    uint16_t key_size;

} bcmptm_rm_tcam_hw_entry_info_t;

/*! \brief Information specific to lt needed by PTRM
  - PTRM can get this info by calling bcmptm_cmdproc_ptrm_info_get()
  */
typedef struct bcmptm_rm_tcam_lt_info_s {

/*! \brief num of ekw rows that LTM is providing for key_width expansion.
  - Aggr_Tcam type LTs
    -# Must be 1 - we cannot support key_width expansion for these.

  - Non_Aggr_Tcam type LTs
    -# Can be greater than 1 to support key_width expansion
    -# Each row of ekw can map to diff SID (each slice in can have
       separate SID)
  */
    uint8_t num_ekw;

/*! \brief num of edw rows that LTM is providing for data_width expansion.
  - Aggr_Tcam type LTs
    -# Must be 1 - we cannot support data_width expansion for these.

  - Non_Aggr_Tcam type LTs
    -# Can be greater than 1 to support data_width expansion
    -# Each row of edw can map to diff SID (each fp_policy inst in can
       have separate SID)
  */
    uint8_t num_edw;

/*! \brief Flag to distinguish Non_Aggr, Aggr Tcam types.
  - For Non_Aggr_Tcam type LTs
    -# req_ekw, req_edw correspond to TCAM_ONLY, TDATA_ONLY views respectively.

  - For Aggr_Tcam type LTs
    -# req_ekw corresponds to TCAM+DATA view and will thus contain key, mask,
       data fields
    -# req_edw is DONT_CARE */
    bool non_aggr;

/*! \brief Identify prefix-based, pri-based TCAM with and without entry_id LTs
  - See defn of bcmlrd_ptrm_type_t */
    bcmlrd_ptrm_type_t pt_type;

/*! \brief Ptr to struct that holds hw info as needed by rsrc_mgr */
    const bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info;

/*! \brief Num of hw_entry_info for this LT.
  - For rm_tcam, this is expected to be 1, because hw_entry_info already
    contains two-dimensional SID information. So, just a place-holder for now.
  */
    uint16_t hw_entry_info_count;

/*! \brief Place holder for more info as needed by rsrc_mgr from LRD
  - TBD - if needed
  */
    const bcmptm_rm_tcam_more_info_t *rm_more_info;

/*! \brief Length of array rm_more_info */
    uint32_t rm_more_info_count;

} bcmptm_rm_tcam_lt_info_t;

/*! \brief Information to be passed to PTRM for each LTM op */
typedef struct bcmptm_rm_tcam_req_s {

/*! \brief Ptr to array holding 'key, mask' fields of an entry

  - Array of array is needed to accommodate key-width expansion cases (where
    LT key width is multiple of underlying PT key width).
    -# 'num_ekw' in bcmptm_rm_tcam_lt_info_t specifies num of rows in the array.

  - Aggr_Tcam type LTs
    -# Format of key, mask, data fields in ekw will match regsfile format of
       underlying TCAM+DATA PT.
    -# num_ekw will be 1.
    -# PTRM must write KEY_TYPEf, DATA_TYPEf values.

  - Non_Aggr_Tcam type LTs
    -# Format of key, mask fields in ekw will match regsfile format of
       underlying TCAM_ONLY PT.
    -# num_ekw can be greater than 1.

  - Priority based LTs with entry_id
    -# ekw is needed ONLY for INSERT op - because searching of entry for such
       tables is based on 'entry_id' and requires rsrc_mgr to keep track of
       entry_id and its location when moving entries.

  - Priority based LTs without entry_id
    -# ekw will be needed for all(INSERT/LOOKUP/DELETE) ops - because searching
       of entry for such tables is based on KEY portion of the entry.

  */
    uint32_t **ekw;

/*! \brief Ptr to array holding 'data' fields of an entry

  - Array of array is needed to allow width expansion of assoc_data (policy)
    -# Num of rows in this array can be > 1 ONLY if LT maps to separate TCAM_ONLY,
       TCAM_DATA_ONLY views.
    -# Num of rows in edw need not be same as same as num rows in ekw array. For
       example, if we have 3*160 bit key and 2x fp_policy, we will have 3 ekw rows
       and 2 edw rows.
    -# 'num_edw' in bcmptm_rm_tcam_lt_info_t specifies num of rows in the array.

  - Aggr_Tcam type LTs
    -# Meaningful only for non aggr LTs. DontCare for aggr LTs.

  - Non_Aggr_Tcam type LTs
    -# Format of fields in the array must match regsfile format of underlying
       TCAM_DATA_ONLY view.

  - Priority based LTs with and without entry_id
    -# edw is needed only for INSERT op.
    -# Must be ALL_ZEROS for LOOKUP, DELETE ops (search is based on entry_id).

  - Misc
    -# LT tooling MUST always use aggr_TCAM+DATA view when avail in regsfile for
       LT to PT mapping.
    -# PTRM must add DATA_TYPEf values.
  */
    uint32_t **edw;

/*! \brief Entry ID - SW object Id to manage the entries in TCAM

   - Needed for priority based LTs with entry_id and not needed for priority
     based LTs without entry_id.

   - For LOOKUP, DELETE ops
     -# rsrc_mgr will search for entry with user-specified entry_id,
        - if entry is found, then rsrc_mgr will read and/or clear the entry.
        - if entry is not found, rsrc_mgr will return SHR_E_NOT_FOUND.

   - For INSERT op with same_key = 1,
     -# rsrc_mgr will update the entry without doing lookup again.

   - For INSERT op with same_key = 0,
     -# if resources are available, rsrc_mgr will allocate a free slot
        to the new entry.Otherwise rsrc_mgr will return SHR_E_FULL.
   */
    int entry_id;

/*! \brief Entry Priority - Value to decide the physical index in the TCAM.

  - Needed for all priority based LTs.

  - Priority based LTs with and without entry_id.
    -# Treated as dont care for LOOKUP, DELETE ops. If entry with user-specified
       entry_id is found, its entry_pri is returned as rsp_entry_pri.
    -# Needed only for INSERT op.

  - entry_pri for Inserted/Replaced entry will be Set/Changed to entry_pri.

  */
    int entry_pri;

/*! \brief Tells PTRM that this is same key as previous LOOKUP op.
  - For more information Refer to the definition in bcmptm.h
  */
    bool same_key;

/*! \brief Logical ID for dfield_format.

  - Each element corresponds to one row in edw.

  - Num of elements = num_edw.

  - For data-width expansion case, we may have separate dfield_Format_id for
    each row of edw.

  - LTM will provide this info ONLY for INSERT ops.
    -# LTM does not have this info for LOOKUP, DELETE commands.
    -# When writing the entry (INSERT), PTRM must provide dfield_format_id with
       Writes to CmdProc.

  - When searching for entry (for any op)
    -# PTRM must ignore value provided by LTM.
    -# Will be returned by CmdProc during reads of found entry.

  - For Aggr_Tcam LTs, there is one underlying TCAM+DATA PT and we store
    dfield_format_id in PTcache for each entry.

  - For Non_Aggr_Tcam LTs, this value must be stored only in cache for
    TCAM_DATA_ONLY view.

  \todo How does PTcache know to reserve dfield_format_id space only for certain
        SIDs? We need to store it for TCAM_DATA_ONLY views only if aggr TCAM
        view is not defined.
  */
    uint16_t *dfield_format_id;

/*! \brief Num of words in rsp_ekw_buf array.
  - PTRM will assume all rows of rsp_ekw to be of same size.
  */
    size_t rsp_ekw_buf_wsize;

/*! \brief Num of words in rsp_edw_buf array.
  - PTRM will assume all rows of rsp_edw to be of same size.
  */
    size_t rsp_edw_buf_wsize;

/*! \brief Atrributes of entry needed for special type of TCAMs like FP.
  - PTRM will convert to required type and use the information. */
    void *entry_attrs;

} bcmptm_rm_tcam_req_t;


/*! \brief Information returned for tcam table request - modeled path */
typedef struct bcmptm_rm_tcam_rsp_s {

/*! \brief Key, mask fields of entry found (all ops).

  - PTRM will not touch this array if entry was not found (all ops).

  - Array of arrays is needed to support expansion of key width.

  - For INSERT op, this is dont care because all insert operations preceded by
    LOOKUP operation and LOOKUP operation will get the old entry information.

  - For Non_Aggr_Tcam type LTs
    -# rsp_ekw correspond to TCAM_ONLY view.
  - For Aggr_Tcam type LTs
    -# rsp_ekw correspond to TCAM+DATA view and will thus contain key, mask,
       data fields.
  */
    uint32_t **rsp_ekw;

/*! \brief Data fields of entry found (all ops).

  - PTRM will not touch this array if entry was not found (all ops).

  - Array of arrays is needed to support expansion of data width.

  - For INSERT op, this is dont care because all insert operations preceded by
    LOOKUP operation and LOOKUP operation will get the old entry information.

  - For Non_Aggr_Tcam type LTs
    -# rsp_edw correspond to TDATA_ONLY view.

  - For Aggr_Tcam type LTs
    -# rsp_edw is DONT_CARE.
  */
    uint32_t **rsp_edw;

/*! \brief Entry_Priority returned for priority based LTs.

  - Priority type LTs with and without entry_id
    -# For LOOKUP, DELETE ops
       - PTRM will return the saved entry priority if entry was found.
         otherwise PTRM will not update this value.
    -# For INSERT op
       - if insert succeeded, PTM will return same value received with request.
       - if insert failed, PTM will not update this value.
  */
    int rsp_entry_pri;

/*! \brief Index at which entry was found (all ops).

  - For all ops, if entry was found, PTRM will return index of found entry.

  - If entry was not found:
    -# For LOOKUP, DELETE ops, PTRM will not set this variable.
    -# For INSERT op, PTRM will return index of new entry if INSERT succeeded.
  */
    int rsp_entry_index[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];

/*! \brief SID of PT where entry was found (all ops).

  - For all ops, if entry was found, PTRM will return SID of found entry.

  - If entry was not found:
    -# For LOOKUP, DELETE ops, PTRM will not set this variable.
    -# For INSERT, PTRM will return SID of new entry if INSERT succeeded.

  \todo which SID to return for non-aggr LT case?
  */
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];

/*! \brief number of key parts.
  */
    int rsp_entry_index_count;

/*! \brief SID of PT where entry data was found (all ops).

  - For all ops, if entry was found, PTRM will return SID of found entry.

  - If entry was not found:
    -# For LOOKUP, DELETE ops, PTRM will not set this variable.
    -# For INSERT, PTRM will return SID of new entry if INSERT succeeded.

  \todo which SID to return for non-aggr LT case?
  */
    bcmdrd_sid_t rsp_entry_data_sid[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];

/*! \brief Index at which entry data was found (all ops).

  - For all ops, if entry was found, PTRM will return index of found entry.

  - If entry was not found:
    -# For LOOKUP, DELETE ops, PTRM will not set this variable.
    -# For INSERT op, PTRM will return index of new entry if INSERT succeeded.
  */
    int rsp_entry_data_index[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];

/*! \brief number of data parts.
  */
    int rsp_entry_data_index_count;

/*! \brief Pipe number of PT where entry was found.
  - For LTs in global mode, RM will return 0.
  - For LTs in pipe-unique mode, RM will return pipe_num where entry was found
    or successfully inserted.
 */
    int rsp_entry_pipe;

/*! \brief Logical ID for dfield_format of found entry (all ops).

  - One element for each row of rsp_edw.

  - If entry was found, PTRM will return dfield_format (returned from PTcache).

  - If entry was not found, PTRM will return 0s for every element corresponding
  to rsp_edw.
  */
    uint16_t *rsp_dfield_format_id;
} bcmptm_rm_tcam_rsp_t;

/*! \brief Place holder for transaction info as needed by rsrc_mgr from LRD
  - This contains data which is needed for rsrc_mgr apart from generic data.
  - This data is needed to handle transactions in rsrc_mgr
 */
typedef struct bcmptm_rm_tcam_trans_info_s {

/*! \brief To hold the details of the Transaction state.
  - This contains data of transaction state
 */
    uint8_t trans_state;

/*! \brief To hold the size required by the LTID to save its software structures
  - This contains size occupied by LT
 */
    uint32_t ltid_size;

/*! \brief To hold signature word.
  - During warmboot, signature word should match.
 */
    uint32_t sign_word;
} bcmptm_rm_tcam_trans_info_t;

/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
  \brief Perform Lookup, Insert, Delete for Tcam Table based LT - Modelled path.

  \param [in] unit Logical device id
  \param [in] req_flags Just a placed holder for now. TBD.
  \param [in] cseq_id Transaction id. Must be passed with every cmdproc_write, read
  \param [in] req_ltid Logical Table enum for current req_op
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  - For Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
  - tbl_inst member of pt_dyn_info may indicate pipe num.
  - Must pass tbl_inst received as part of pt_dyn_info for all cmdproc write,
    read requests.
  - Need pipe num to support unique mode for FP tables (eg: IFP, EFP, VFP)
  - Value of -1 means Global mode.
  - pipe_num for all underlying SIDs (TCAM_ONLY, TDATA_ONLY views) must be same
    for a given LT
  \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for tcam tables
  \param [in] req_info See the definition for bcmptm_rm_tcam_req_t

  \param [out] rsp_info See the definition for bcmptm_rm_tcam_rsp_t
  \param [out] rsp_ltid LTID of found entry.
            \n If entry was found, for all ops, PTRM will return ltid of found
               entry from PTcache.
            \n If entry was not found, PTRM will return ALL_ZEROs for LOOKUP,
	       DELETE ops,
            \n If entry was not found, PTRM will return req_ltid for INSERT op.
  \param [out] rsp_flags Just a place holder for now.
            \n \todo flags will be defined on need basis.

  \retval SHR_E_NONE Success
  \retval SHR_E_NOT_FOUND Entry not found for Lookup, Delete ops
  \retval SHR_E_FULL Could not perform requested Insert
  \retval SHR_E_UNAVAIL Requested op is not supported
 */
extern int
bcmptm_rm_tcam_req(int unit,
                   uint32_t req_flags,
                   uint32_t cseq_id,
                   bcmltd_sid_t req_ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_tcam_req_t *req_info,
                   bcmptm_rm_tcam_rsp_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags);

/*!
 * \brief Initialize the Resource manager for TCAMs. This involves calculating,
 * \n allocating, seggregating and initializing the memory for all TCAMs
 * \n supported by RM TCAM module. Updates centralized global
   \n data structure holding memory offset for each TCAM ltid.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_init(int unit);

/*!
 * \brief Deallocate the memory created for TCAM resource manager and update
 * \n the centralized global data structure holding memory offset for TCAM ltid
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_cleanup(int unit);

/*!
 *  \brief Get the total number of entries in a TCAM.
 *
 *  \param [in] unit Logical device id
 *  \param [in] ltid Logical Table enum that is accessing the table.
 *  \param [in] ltid_info LRD information for the ltid.
 *  \param [out] num_entries Pointer to hold Total number of entries in a TCAM
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_depth_get(int unit, bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         uint32_t *num_entries);

/*!
 *  \brief Get the attributes of an entry in a TCAM.
 *
 *  \param [in] unit Logical device id
 *  \param [in] ltid Logical Table enum that is accessing the table.
 *  \param [in] ltid_info LRD information for the ltid.
 *  \param [in] entry_id Entry IdA
 *  \param [in] entry_attrs Attributes of entry.
 *  \param [out] entry_pri Priority of Entry
 *  \param [out] entry_index of entry in TCAM
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */

extern int
bcmptm_rm_tcam_prio_entry_attr_get(int unit, bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint16_t entry_id,
                                   void *entry_attrs,
                                   uint16_t *entry_pri, uint32_t *entry_index);

/*!
 * \brief Component config phase of init sequence
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] phase 1,2,3
 * \n 1 => 1st scan of all LTIDs. Should see calls to bcmptm_rm_tcam_lt_reg
 * \n 2 => 2nd scan of all LTIDs. Should see calls to bcmptm_rm_tcam_lt_reg
 * \n 3 => No scan of LTIDs. No calls to bcmptm_rm_tcam_lt_reg
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase);

/*!
 * \brief Registration of LTID with rm_tcam
 *
 * \param [in] unit Logical device id
 * \param [in] ltid LTID that needs to be managed by rsrc_mgr
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] phase 1,2,3
 * \param [in] lt_state_offset Offset into LT state mem (in rsrc_mgr)
 * \n Note: lt_state_offset will be stored by caller only in phase 2
 * (comp_config)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_lt_reg(int unit, bcmltd_sid_t ltid, bool warm,
                      bcmptm_sub_phase_t phase,
                      uint32_t *lt_state_offset);

/*!
 * \brief Commit, Abort, Idle_Check
 *
 * \param [in] unit Logical device id
 * \param [in] flags usage TBD
 * \param [in] cseq_id Transaction id
 * \param [in] trans_cmd Commit, Abort, Idle_Check (See bcmptm_trans_cmd_t)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_trans_cmd(int unit, uint32_t flags, uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd);

/*!
 * \brief Chip specific function to update registers or memories.
 *
 * \param [in] unit BCM device number.
 * \param [in] ltid Logical Table Enum
 * \param [in] sid_enum To identify sid of type bcmptm_rm_tcam_sid_t
 * \param [in] sid_info Properties of the sid.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int
bcmptm_rm_tcam_memreg_update(int unit,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_sid_t sid_enum,
                        bcmptm_rm_tcam_sid_info_t *sid_info);

#endif /* BCMPTM_RM_TCAM_INTERNAL_H */
