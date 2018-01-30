/*! \file bcmltm_table_op_pt_info.h
 *
 * Interface to update in-memory table with PT status.
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

#ifndef BCMLTM_TABLE_OP_PT_INFO_H
#define BCMLTM_TABLE_OP_PT_INFO_H

#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmbd/bcmbd.h>

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for keyed LT.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of keyed LT operation.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for keyed operation.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 * \param [in] mrsp_info Response information of last operation.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_mreq_keyed_lt(int unit, bcmltd_sid_t ltid,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_keyed_lt_mrsp_info_t *mrsp_info);

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for indexed LT.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of indexed LT operation.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for indexed operation.
 * \param [in] ptid Physical table ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_mreq_indexed_lt(int unit, bcmltd_sid_t ltid,
                                 uint32_t ptid,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info);

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for interactive hash path.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of HASH LT operation in interactive path.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] ptid Physical table ID.
 * \param [in] entry_index Index in the physical table.
 * \param [in] pt_dyn_hash_info Pointer to bcmbd_pt_dyn_hash_info_t.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_ireq_hash_lt(int unit, bcmltd_sid_t ltid,
                              uint32_t ptid, uint32_t entry_index,
                              bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info);

/*!
 * \brief TABLE_OP_PT_INFO logical table with PT status for interactive path.
 *
 * This function updates the TABLE_OP_PT_INFO table with
 * PT status of indexed LT operation in interactive path.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logical Table enum for current operation.
 * \param [in] ptid Physical table ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_pt_status_ireq_op(int unit, bcmltd_sid_t ltid,
                         uint32_t ptid,
                         bcmbd_pt_dyn_info_t *pt_dyn_info);

#endif /* BCMLTM_TABLE_OP_PT_INFO_H */
