/*! \file bcmptm_wal_internal.h
 *
 * This file contains APIs, defines for WAL sub-block
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

#ifndef BCMPTM_WAL_INTERNAL_H
#define BCMPTM_WAL_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_internal.h>


/*******************************************************************************
  Defines
 */


/*******************************************************************************
  Typedefs
 */


/*******************************************************************************
  Global variables
 */


/*******************************************************************************
  Function prototypes
 */
/*!
  \brief Enable/Disable Ignoring of error for ops sent to HW
  \param [in] unit Logical device id
  \param [in] enable TRUE, FALSE

  \retval SHR_E_NONE Success
  */
extern int
bcmptm_wal_ignore_op_err(int unit, bool enable);

/*!
  \brief Update HW table entry (through WAL)
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (TBD)
           \n TBD
  \param [in] cseq_id Commit Sequence Id
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] misc_info Information needed for SLAM.
  \param [in] entry_words Array large enough to hold write_data
  \param [in] op_type See definition for bcmptm_rm_op_t
  \param [in] slice_mode_change_info Information needed to change slice_mode.

  \param [out] rsp_flags More detailed status (TBD)

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available. Eventually timed out.
  */
extern int
bcmptm_wal_write(int unit,
                 uint32_t flags,
                 uint32_t cseq_id,
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 void *pt_ovrr_info,
                 bcmptm_misc_info_t *misc_info,
                 uint32_t *entry_words,
                 bcmptm_rm_op_t op_type,
                 bcmptm_rm_slice_change_t *slice_mode_change_info,

                 uint32_t *rsp_flags);

/*!
  \brief Read table entry from HW (through WAL)
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
           \n BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM can be asserted to get raw
           data for TCAM type mems.
           \n BCMPTM_REQ_FLAGS_POSTED_HW_READ can be asserted to get control
           back in non-blocking fashion (without waiting for read data from HW)
  \param [in] cseq_id Commit Sequence Id
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] misc_info Information needed for DMA..
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array

  \param [out] rsp_entry_words Array large enough to hold read data
  \param [out] rsp_flags More detailed status (TBD)

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_read(int unit,
                uint32_t flags,
                uint32_t cseq_id,
                bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *pt_dyn_info,
                void *pt_ovrr_info,
                bcmptm_misc_info_t *misc_info,
                size_t rsp_entry_wsize,

                uint32_t *rsp_entry_words,
                uint32_t *rsp_flags);

/*!
  \brief Commit pending(uncommitted) ops
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
              \n Asserted BCMPTM_REQ_FLAGS_HW_SYNC will make WAL return ctl back
              to caller only when all ops have executed in HW (WAL is empty)
  \param [in] mreq_state_in_trans mreq_state
  \param [in] cseq_id Commit Sequence Id
  \param [in] notify_fn_ptr Notify TRM when committed ops are pushed to HW
  \param [in] notify_info Notify TRM when committed ops are pushed to HW

  \param [out] rsp_flags More detailed status (TBD)

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_commit(int unit,
                  uint32_t flags,
                  bool mreq_state_in_trans,
                  uint32_t cseq_id,
                  bcmptm_trans_cb_f notify_fn_ptr,
                  void *notify_info,

                  uint32_t *rsp_flags);

/*!
  \brief Delete pending(uncommitted) ops
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (TBD)
  \param [in] cseq_id Commit Sequence Id

  \param [out] rsp_flags More detailed status (TBD)

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_abort(int unit,
                 uint32_t flags,
                 uint32_t cseq_id,

                 uint32_t *rsp_flags);

/*!
  \brief Search for SERC restore data in WAL
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
           \n TBD. Pass all ZEROs for now.
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
  \param [in] req_slice_num Tcam slice_num for corrupted entry. Must be -1 for non
  Tcam tables and for Tcam tables which don't have slices.
  \param [in] req_slice_mode Current slice_mode for slice specified by
  slice_num. Must be BCMPTM_TCAM_SLICE_INVALID for non Tcam tables and for Tcam
  tables which don't have slices.

  \param [out] rsp_entry_words Array large enough to hold read data

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_serc_data_find(int unit,
                          uint32_t flags,
                          bcmdrd_sid_t sid,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          void *pt_ovrr_info,
                          size_t rsp_entry_wsize,
                          int req_slice_num,
                          bcmptm_rm_slice_mode_t req_slice_mode,

                          uint32_t *rsp_entry_words);

/*!
  \brief Drain all ops in WAL
  - Means: wait for all committed ops in WAL to finish executing in HW.
  - Optionally, stop WAL rdr after the drain is complete.

  \param [in] unit Logical device id
  \param [in] end_check TRUE means expect WAL c_trans_state to be IDLE when WAL
  is empty.

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_drain(int unit, bool end_check);

/*!
  \brief Alloc, init wal data structs
  - Writer side only

  \param [in] unit Logical device id
  \param [in] warm TRUE implies warmboot

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_init(int unit, bool warm);

/*!
  \brief De-alloc wal data structs
  - Writer side only

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_cleanup(int unit);

/*!
  \brief Enable, disable WAL auto-commit mode
  - All -ve numbers mean auto-commit mode
  - Must not provide Commit from LTM when in auto-commit mode
  - 0 means real-commit mode

  \param [in] unit Logical device id
  \param [in] wal_mode 0 => normal. Negative implies auto-commit

  \retval SHR_E_NONE Success
*/
extern int
bcmptm_wal_mode_sel(int unit, int wal_mode);

/*!
  \brief Apply wal_cfg
  - Mainly for debug - allows unit test to change wal_cfg vars in middle of
    test according to wal_mode selected using bcmptm_wal_mode_sel
  - There is no re-sizing of WAL buffers in HA space.

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success
*/
extern int
bcmptm_wal_cfg_set(int unit);

/*!
  \brief Check if WAL is in auto-commit mode

  \param [in] unit Logical device id

  \retval TRUE if WAL is in auto-commit mode
*/
extern bool
bcmptm_wal_autocommit_enabled(int unit);

/*! \brief Verify cseq_id for each LTM req

  \param [in] unit Logical device id
  \param [in] cseq_id Commit Sequence Id

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_cseq_id_check(int unit, uint32_t cseq_id);

/*! \brief Transaction cmd: commit, abort, cseq_id_check

  \param [in] unit Logical device id
  \param [in] flags For COMMIT, BCMPTM_REQ_FLAGS_HW_SYNC is meaningful
              \n Usage TBD for other commands
  \param [in] trans_cmd Refer to details of bcmptm_trans_cmd_t
  \param [in] cseq_id Commit Sequence Id
  \param [in] notify_fn_ptr Meaningful only for COMMIT
  \param [in] notify_info Meaningful only for COMMIT

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_trans_cmd(int unit, uint32_t flags,
                     bcmptm_trans_cmd_t trans_cmd,
                     uint32_t cseq_id, bcmptm_trans_cb_f notify_fn_ptr,
                     void* notify_info);

/*! \brief Record mreq_cseq_id, and set mreq_state_in_trans
    - Following must be called for every non-NOP request from LTM
    - It must be called even for READ, LOOKUP, GET_FIRST, GET_NEXT, etc ops
    - It changes mreq_state_in_trans to TRUE.

  \param [in] unit Logical device id
  \param [in] flags Usage TBD
  \param [in] cseq_id Commit Sequence Id

  \retval SHR_E_NONE Success */
int
bcmptm_wal_mreq_state_update(int unit, uint32_t cseq_id, uint32_t flags);

/*!
 * \brief Start WAL reader thread
 * - Includes alloc for msgq
 * - Includes WAL reader thread_create and ensuring that it is run state
 *
 * \param [in] unit Logical device id
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_init(int unit);

/*!
 * \brief Release WAL reader thread and msgq resources
 *
 * \param [in] unit Logical device id
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_cleanup(int unit);

/*!
 * \brief Caller (SERC thread) is requesting scf_lock
 *
 * \param [in] unit Logical device id
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_lock_req_serc(int unit);

/*!
 * \brief Caller (SERC thread) wants to give up walr_lock
 *
 * \param [in] unit Logical device id
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_unlock_req_serc(int unit);

/*!
 * \brief Check wal DMA assist availability.
 *
 * \param [in] unit Logical device id
 * \param [in] read_op TRUE => Read operation. FALSE => Write operation.
 * \param [in] entry_count Number of entries.
 * \param [out] wal_dma_avail TRUE => DMA assist is available for WAL writes,
 * reads.
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern void
bcmptm_wal_dma_avail(int unit, bool read_op, uint32_t entry_count,
                     bool *wal_dma_avail);

#endif /* BCMPTM_WAL_INTERNAL_H */
