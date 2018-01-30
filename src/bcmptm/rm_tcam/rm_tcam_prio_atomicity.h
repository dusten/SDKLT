/*! \file rm_tcam_prio_atomicity.h
 *
 * Utils, defines internal to PRIO TCAM Atomicity changes
 * This file contains utils, defines which are internal to
 * TCAM resource manager(i.e. these are not visible outside RM-TCAM)
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

#ifndef RM_TCAM_PRIO_ATOMICITY_H
#define RM_TCAM_PRIO_ATOMICITY_H

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Update the entry words to a particular TCAM index.
 * \n Updates both HW and SW state
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
    - For Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
    - tbl_inst member of pt_dyn_info may indicate pipe num.
    - Must pass tbl_inst received as part of pt_dyn_info for all cmdproc write,
      read requests.
    - Need pipe num to support unique mode for FP tables (eg: IFP, EFP, VFP)
    - Value of -1 means Global mode.
    - pipe_num for all underlying SIDs (TCAM_ONLY, TDATA_ONLY views)
      must be same for a given LT.
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] found_index TCAM index of the entry_id which needs the update.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_entry_update(int unit,
                                 uint32_t flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 uint32_t found_index);


/*!
 * \brief Update the entry words to a particular TCAM index.
 * \n Updates only the data portion of the TCAM index.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
    - For Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
    - tbl_inst member of pt_dyn_info may indicate pipe num.
    - Must pass tbl_inst received as part of pt_dyn_info for all cmdproc write,
      read requests.
    - Need pipe num to support unique mode for FP tables (eg: IFP, EFP, VFP)
    - Value of -1 means Global mode.
    - pipe_num for all underlying SIDs (TCAM_ONLY, TDATA_ONLY views)
      must be same for a given LT.
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] found_index TCAM index of the entry_id which needs the update.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_entry_update_data_only(int unit,
                               uint32_t flags,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_rm_tcam_req_t *req_info,
                               bcmptm_rm_tcam_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags,
                               uint32_t found_index);

/*!
 * \brief To verify if the requested update is a key change or a data change
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
    - For Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
    - tbl_inst member of pt_dyn_info may indicate pipe num.
    - Must pass tbl_inst received as part of pt_dyn_info for all cmdproc write,
      read requests.
    - Need pipe num to support unique mode for FP tables (eg: IFP, EFP, VFP)
    - Value of -1 means Global mode.
    - pipe_num for all underlying SIDs (TCAM_ONLY, TDATA_ONLY views)
      must be same for a given LT.
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] found_index TCAM index of the entry_id which needs the update.
 * \param [out] update_type Data only or Key only or both change
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_update_type_get(int unit,
                        uint32_t req_flags,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        bcmbd_pt_dyn_info_t *pt_dyn_info,
                        bcmptm_rm_tcam_req_t *req_info,
                        bcmptm_rm_tcam_rsp_t *rsp_info,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags,
                        uint32_t found_index,
                        uint8_t *update_type);

#endif /* RM_TCAM_PRIO_ATOMICITY_H */
