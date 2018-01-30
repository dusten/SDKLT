/*! \file bcmptm_rm_alpm_internal.h
 *
 * APIs, defines for PTM to interface with ALPM Resource Manager
 * Mainly for V4 and V6 Route LTs
 * This file contains APIs, defines for PTRM-ALPM interfaces
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

#ifndef BCMPTM_RM_ALPM_INTERNAL_H
#define BCMPTM_RM_ALPM_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_internal.h>

/*******************************************************************************
  Typedefs
 */
/*! \brief Information requested by ALPM Resource Manager
 */
typedef struct bcmptm_rm_alpm_req_info_s {

/*! \brief Array of words holding key, mask fields of an entry.
 */

    uint32_t *ekw;

/*! \brief Array of words holding data fields of an entry.
 */
    uint32_t *edw;

/*! \brief Logical ID for dfield_format
 * - \todo For now it will be 0. Instances of non zero cases yet to figure out.
 */
    uint16_t dfield_format_id;

/*! \brief Tells PTRM that this is same key as previous LOOKUP op.
 * - For more information Refer to the definition in bcmptm.h
 */
    bool same_key;

/*! \brief Num of words in rsp_ekw_buf array
 * - Mainly for sanity check
 */
    size_t rsp_ekw_buf_wsize;

/*! \brief Num of words in rsp_edw_buf array
 * - Mainly for sanity check
 */
    size_t rsp_edw_buf_wsize;

/*! \brief Count of array rsp_entry_index & array rsp_entry_sid
 * - Suggest to set to 3, as this is the max levels.
 */
    uint8_t rsp_entry_max_count;
} bcmptm_rm_alpm_req_info_t;

/*! \brief Information responded by ALPM Resource Manager */
typedef struct bcmptm_rm_alpm_rsp_info_s {

/*! \brief  Array of words holding key, mask fields of found entry.
 * - Match formats of L3_DEFIP or L3_DEFIP_PAIR_128 view.
 */
    uint32_t *rsp_ekw;

/*! \brief  Array of words holding data fields of found entry.
 * - Match formats of L3_DEFIP or L3_DEFIP_PAIR_128 view.
 */
    uint32_t *rsp_edw;

/*! \brief Logical ID for dfield_format of found entry (all ops).
 * - If entry was found, PTRM will return logical dfield_format.
 * - If entry was not found, PTRM will set this var to 0.
 * -  \todo PTRM will return 0.
 */
    uint16_t rsp_dfield_format_id;

/*! \brief Index of entry.
 * - Meaningful for LOOKUP, INSERT ops.
 * - For LOOKUP, it's index of found entry.
 * - For INSERT, it's index of new location.
 * - Entries (from multiple levels) are possible.
 */
    uint32_t *rsp_entry_index;


/*! \brief SID of entry.
 * - Meaningful for LOOKUP, INSERT ops.
 * - For LOOKUP, it's index of found entry.
 * - For INSERT, it's index of new location.
 * - Entries (from multiple levels) are possible.
 */
    bcmdrd_sid_t *rsp_entry_sid;

/*! \brief Actual entry count returend
 * - The actual count that array rsp_entry_index & rsp_entry_sid filled
 */
    uint8_t rsp_entry_count;
} bcmptm_rm_alpm_rsp_info_t;

/*******************************************************************************
  Function prototypes
 */
/*!
 * \brief Perform Lookup, Insert, Delete for ROUTE LTs
 *
 * \param [in] u Logical device id
 *
 * \param [in] req_flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 *
 * \param [in] cseq_id Transaction id. Must be passed with every cmdproc_write, read
 *
 * \param [in] req_ltid Logical Table enum for current req_op
 *
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 *
 * \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for tables
 *             with key
 *
 * \param [in] req_info See the definition for bcmptm_rm_alpm_req_info_t
 *
 * \param [out] rsp_info See the definition for bcmptm_rm_alpm_rsp_info_t
 *
 * \param [out] rsp_ltid LTID of found entry.
 * \n 1. If entry was found, for all ops, PTRM will return ltid of found
 *       entry from PTcache.
 * \n 2. If entry was not found, PTRM will return ALL_ZEROs for LOOKUP,
 *       DELETE ops,
 * \n 3. If entry was not found, PTRM will return req_ltid for INSERT op.
 *
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND Entry not found for Lookup, Delete ops
 * \retval SHR_E_FULL Could not perform requested Insert
 * \retval SHR_E_UNAVAIL Requested op is not supported
 */
extern int
bcmptm_rm_alpm_req(int u,
                   uint32_t req_flags,
                   uint32_t cseq_id,
                   bcmltd_sid_t req_ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_alpm_req_info_t *req_info,

                   bcmptm_rm_alpm_rsp_info_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags);


/*!
 * \brief Initialize the Resource manager for ALPMs.
 * \param [in] u Logical device id
 * \param [in] warm Warm boot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_init(int u, bool warm);

/*!
 * \brief Stop the Resource manager for ALPMs.
 * \param [in] u Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_stop(int u);

/*!
 * \brief De-initialize the Resource manager for ALPMs.
 * \param [in] u Logical device id
 * \param [in] warm Warm cleanup
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_cleanup(int u, bool warm);

/*!
 * \brief RM ALPM Commit, Abort, Idle_Check
 *
 * \param [in] u Logical device id
 * \param [in] flags usage TBD
 * \param [in] cseq_id Transaction id
 * \param [in] trans_cmd Commit, Abort, Idle_Check (See bcmptm_trans_cmd_t)
 *
 * \retval SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_trans_cmd(int u,
                         uint32_t flags,
                         uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd);


/*!
 * \brief Registration of LTID with RM ALPM
 *
 * \param [in] u Logical device id
 * \param [in] ltid LTID that needs to be managed by rsrc_mgr
 * \param [in] phase Phase 1/2/3
 * \param [in] lt_state_offset Offset into LT state mem (in rsrc_mgr)
 * \n Note: lt_state_offset will be stored by caller only in phase 2
 * (comp_config)
 *
 * \retval SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_lt_reg(int u,
                      bcmltd_sid_t ltid,
                      bcmptm_sub_phase_t phase,
                      uint32_t *lt_state_offset);


/*!
 * \brief RM ALPM component config phase of init sequence
 *
 * \param [in] u Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] phase 1,2,3
 * \n 1 => 1st scan of all LTs. Refer calls to bcmptm_rm_alpm_lt_reg()
 * \n 2 => 2nd scan of all LTs. Refer calls to bcmptm_rm_alpm_lt_reg()
 * \n 3 => No scan of LTIDs.
 *
 * \retval SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_comp_config(int u, bool warm, bcmptm_sub_phase_t phase);

/*!
 * \brief Initialize global resources for Resource manager for ALPMs.
 *
 * \retval SHR_E_NONE Success, SHR_E_MEMORY Fails.
 */
extern int
bcmptm_rm_alpm_global_init(void);

/*!
 * \brief Cleanup global resources for the Resource manager for ALPMs.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_alpm_global_cleanup(void);

#endif /* BCMPTM_RM_ALPM_INTERNAL_H */

