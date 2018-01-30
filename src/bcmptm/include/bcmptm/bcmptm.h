/*! \file bcmptm.h
 *
 * APIs, defines for top-level PTM interfaces
 *
 * This file contains APIs, defines for top-level PTM interfaces
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

#ifndef BCMPTM_H
#define BCMPTM_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_ltm_types.h>


/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Read a mem or reg internal to SOC - Interactive path.
 * \n By default data is read from PTCache for cacheable PTs..
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_entry_words Array large enough to hold read data
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ireq_read(int unit,
                 uint32_t flags,
                 bcmdrd_sid_t sid,
                 void *pt_dyn_info,
                 void *pt_ovrr_info,
                 size_t rsp_entry_wsize,
                 bcmltd_sid_t req_ltid,

                 uint32_t *rsp_entry_words,
                 bcmltd_sid_t *rsp_ltid,
                 uint32_t *rsp_flags);

/*!
 * \brief Write a mem or reg internal to SOC - Interactive path.
 * - We have provided a debug option to allow Ireq path to modify modeled table.
 * If the entry was already valid in cache and not written by ireq_path, we will
 * preserved the LTID, DFID. Else, we will allow ireq path to over-write LTID,
 * DFID for this entry in cache.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ireq_write(int unit,
                  uint32_t flags,
                  bcmdrd_sid_t sid,
                  void *pt_dyn_info,
                  void *pt_ovrr_info,
                  uint32_t *entry_words,
                  bcmltd_sid_t req_ltid,

                  bcmltd_sid_t *rsp_ltid,
                  uint32_t *rsp_flags);

/*!
 * \brief Verify sentinel words, invalid entries in ptcache
 * \n Can be used as end-check in ALL tests to make sure that sentinel words in
 * PTcache did not corrupted.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_verify(int unit);

/*!
  \brief Perform Lookup, Insert, Delete for hash type PTs - Interactive path.
  - WILL NOT USE SW_CACHE
  - For simple tables, PTM will:
   -# Use (entry_key_words | entry_data_words) as entry_words and forward op to HW.
   -# Copy rsp_entry_words as received from HW into entry_key_words,
   entry_data_words arrays.

  \todo We have separated out key_words, data_words into separate arrays for
  support of L2_ENTRY table on interactive path with HW learning enabled.
  HW allows support of L2_ENTRY table with different entry sizes. Will PTM
  support mapping of LTM data to different hw_entry formats on interactive path?

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_hash_info_t (for XGS devices)
           \n Specifies the bank information.
  \param [in] pt_ovrr_info Ptr to override info (TBD)
  \param [in] entry_key_words Array large enough to hold key, mask fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] entry_data_words Array large enough to hold data fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for hash tables
  \param [in] rsp_entry_buf_wsize Num of words in rsp_entry_key_words,
  rsp_entry_data_words arrays
  \param [in] req_ltid Logical Table enum that is accessing the table

  \param [out] rsp_entry_key_words As received from HW
  \param [out] rsp_entry_data_words As received from HW
  \param [out] rsp_entry_index As received from HW
  \param [out] rsp_ltid Same as req_ltid (TBD)..
  \param [out] rsp_flags More detailed status
  \n For successful insert, BCMPTM_RSP_FLAGS_REPLACED may be asserted.
  \retval SHR_E_NONE Table entry found, deleted, inserted successfully.
  \retval SHR_E_NOT_FOUND Table entry not found (for lookup, delete ops)
  \retval SHR_E_FULL Table full (for Insert op only).
  \retval SHR_E_TIMEOUT S-channel operation timed out.
  \retval SHR_E_INTERNAL Invalid S-channel NACK response type.
  \retval SHR_E_FAIL Other failures.
 */
extern int
bcmptm_ireq_hash_lt(int unit,
                    uint32_t flags,
                    bcmdrd_sid_t sid,
                    void *pt_dyn_info,
                    void *pt_ovrr_info,
                    uint32_t *entry_key_words,
                    uint32_t *entry_data_words,
                    bcmptm_op_type_t req_op,
                    size_t rsp_entry_buf_wsize,
                    bcmltd_sid_t req_ltid,

                    uint32_t *rsp_entry_key_words,
                    uint32_t *rsp_entry_data_words,
                    uint32_t *rsp_entry_index,
                    bcmltd_sid_t *rsp_ltid,
                    uint32_t *rsp_flags);

/*!
  \brief Perform Write, Read for Index based LTs (without Key) - Modeled path.
  - This API allows LTM to manage direct type LTs (eg: profile tables) that
  may map to several physical tables (SIDs). LTM decides the order in which
  phys tables are accessed. PTM does not do LTID to SID mapping for ops
  presented by this API. There is no rsrc_mgr or SW state for these in PTM.
  - Only WRITE, READ, NOP type opcodes supported
  - Hash type LTs, TCAM type LTs should not be accessed with
  bcmptm_mreq_indexed_lt()
  - NOPs allow LTM to issue ABORT, COMMIT without any table access.
  Only unit, flags, req_op, cseq_id, notify_fn_ptr, notify_info,
  rsp_flags, ret_val are meaningful for NOP request.
  - READs allow LTM to perform read_modify_write.
  - WRITEs:
   - req_ltid is stored in PTcache along with entry
  - READs from HW:
   - LTM can force PTM to read from HW by setting
   BCMPTM_REQ_FLAGS_DONT_USE_CACHE along with READ op.
  - READs from PTcache:
   - rsp_ltid is provided from PTcache along with entry
   - All physical tables in modeled path are expected to be Cacheable
    -# If table is not cacheable, READ req will return SHR_E_UNAVAIL
    -# If entry is not valid in cache (was never written before by SW) ???
    \n For MEMORY based tables, cache contains 0s and this will match HW tables
    that were cleared during init. ??? Can we have cacheable table with non-zero
    init value?. Current behavior: return SHR_E_NONE and value from PTcache
    \n For REGISTER based tables - cache contains 0s but may not match default
    HW value.
    \todo Must fill ptcache with null entry values - and return SHR_E_NONE even
    if entry is not valid in cache

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Write, Read requests, index member of pt_dyn_info is necessary
  \n for Write, Read requests, tbl_inst member of pt_dyn_info may indicate pipe
  num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] misc_info Additional information that can be specified to request
  use of DMA. Refer to definition of bcmptm_misc_info_t for details.
  \param [in] req_op Only WRITE, READ, NOP ops are valid
  \param [in] entry_words Array large enough to hold write data
  \n Dont care for READ, NOP requests.
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
  \n Dont care for WRITE, NOP requests.
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] cseq_id Commit Sequence ID.
  \n For every mreq, LTM must also provide a cseq_id - even for mreq for which
  COMMIT is not asserted.
  \n Commit sequence contains multiple mreq. Each mreq submitted to PTM could
  result in N physical table ops where N could > size of schan_fifo. Having
  cseq_id for each mreq allows PTM to track each 'bunch of pt ops' for
  every mreq.
  \n For committed sequences which requested notification, this ID will be returned
  along with notification msg (TBD).
  \n PTM cmdproc will maintain overall state 'IDLE', so we do not need to
  reserve any value (like 0, 0xFFFF) as 'reserved'.
  \n This ID will also help with crash recovery (details TBD).

  \param [in] notify_fn_ptr Will be called when committed transaction is
  pushed to HW.
  \n Meaningful ONLY for COMMIT (and not for ABORT)
  \n NULL means notification is not needed.

  \param [in] notify_info Will be returned as param with notify_fn_ptr call
  \n Meaningful ONLY when notify_fn_ptr is not NULL

  \param [out] rsp_entry_words Array large enough to hold read data
  \n Dont care for WRITE, NOP requests.
  \param [out] rsp_ltid For READs from PTcache, rsp_ltid is ltid of entry in
  PTcache. For all other cases, rsp_ltid = req_ltid. (TBD)
  \param [out] rsp_flags More detailed status (TBD)

  \retval SHR_E_NONE Success
  \retval SHR_E_UNAVAIL Table is not cacheable.
 */
extern int
bcmptm_mreq_indexed_lt(int unit,
                       uint32_t flags,
                       bcmdrd_sid_t sid,
                       void *pt_dyn_info,
                       void *pt_ovrr_info,
                       bcmptm_misc_info_t *misc_info,
                       bcmptm_op_type_t req_op,
                       uint32_t *entry_words,
                       size_t rsp_entry_wsize,
                       bcmltd_sid_t req_ltid,
                       uint32_t cseq_id,
                       bcmptm_trans_cb_f notify_fn_ptr,
                       void *notify_info,

                       uint32_t *rsp_entry_words,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags);

/*!
  \brief Perform Lookup, Insert, Delete for LTs with Key - Modeled path.
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
  \n tbl_inst member of pt_dyn_info may indicate pipe num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] req_op LOOKUP, DELETE, INSERT ops are valid for tables
  with key.
  \n LTM can force PTM to read final entry from HW by setting
   BCMPTM_REQ_FLAGS_DONT_USE_CACHE along with LOOKUP op. This can be used to
   read Hit bits.
  \param [in] req_info See the definition for bcmptm_keyed_lt_mreq_info_t
  \param [in] cseq_id Commit Sequence ID.
  \n See earlier description for details.

  \param [out] rsp_info See the definition for bcmptm_keyed_lt_mrsp_info_t
  \param [out] rsp_ltid Previous owner of the entry
  \param [out] rsp_flags More detailed status (TBD)
  \n For Insert operation, PTM can provide additional information as below:
   - REPLACED
   - OLD_ENTRY_DELETED

  \retval SHR_E_NONE Success
  \retval SHR_E_UNAVAIL Requested op is not supported
  \retval SHR_E_NOT_FOUND Entry not found for Lookup, Delete ops
  \retval SHR_E_FULL Could not perform requested Insert for Tcam Tables
  \retval SHR_E_FULL Could not perform requested Insert due to Hash Collisions
  (Hash Tables)
  \retval SHR_E_MEMORY Could not perform requested Insert due to Table Full
  (Hash Tables).
  \todo SHR_E_MEMORY is not easy to implement in PTRM-HASH.
 */
extern int
bcmptm_mreq_keyed_lt(int unit,
                     uint32_t flags,
                     bcmltd_sid_t req_ltid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     bcmptm_op_type_t req_op,
                     bcmptm_keyed_lt_mreq_info_t *req_info,
                     uint32_t cseq_id,

                     bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags);

/* LTM callbacks for LT statistics and PT status tracking */

/*!
 * \brief Register a callback for incrementing LTM statistics.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT ops related statistic values.  This callback permits the PTM
 * implementation to update these statistics.
 *
 * \param [in] unit Logical device id
 * \param [in] lt_stat_cb Callback function to increment LT statistics
 *             values for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_lt_stat_increment_register(int unit,
                                  bcmptm_lt_stats_increment_cb lt_stat_cb);

/* LTM callbacks for PT status recording. */

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for modeled keyed operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers modeled keyed PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_mreq_keyed_lt_register(int unit,
                      bcmptm_pt_status_mreq_keyed_lt_cb pt_status_cb);

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for modeled index operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers modeled index PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_mreq_indexed_lt_register(int unit,
                      bcmptm_pt_status_mreq_indexed_lt_cb pt_status_cb);

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for interactive hash operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers interactive hash PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_ireq_hash_lt_register(int unit,
                           bcmptm_pt_status_ireq_hash_lt_cb pt_status_cb);

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for interactive index operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers interactive index PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_ireq_op_register(int unit,
                                  bcmptm_pt_status_ireq_op_cb pt_status_cb);

/* LTM wrappers for PTM interfaces */

/*!
 * \brief LTM-callbacks wrapper for bcmptm_ireq_read
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_entry_words Array large enough to hold read data
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ltm_ireq_read(int unit,
                     uint32_t flags,
                     bcmdrd_sid_t sid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,
                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags);

/*!
 * \brief LTM-callbacks wrapper for bcmptm_ireq_write
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ltm_ireq_write(int unit,
                      uint32_t flags,
                      bcmdrd_sid_t sid,
                      void *pt_dyn_info,
                      void *pt_ovrr_info,
                      uint32_t *entry_words,
                      bcmltd_sid_t req_ltid,
                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags);

/*!
  \brief  LTM-callbacks wrapper for bcmptm_ireq_hash_lt

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_hash_info_t (for XGS devices)
           \n Specifies the bank information.
  \param [in] pt_ovrr_info Ptr to override info (TBD)
  \param [in] entry_key_words Array large enough to hold key, mask fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] entry_data_words Array large enough to hold data fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for hash tables
  \param [in] rsp_entry_buf_wsize Num of words in rsp_entry_key_words,
  rsp_entry_data_words arrays
  \param [in] req_ltid Logical Table enum that is accessing the table

  \param [out] rsp_entry_key_words As received from HW
  \param [out] rsp_entry_data_words As received from HW
  \param [out] rsp_entry_index As received from HW
  \param [out] rsp_ltid Same as req_ltid (TBD)..
  \param [out] rsp_flags More detailed status
  \n For successful insert, BCMPTM_RSP_FLAGS_REPLACED may be asserted.
 */
extern int
bcmptm_ltm_ireq_hash_lt(int unit,
                        uint32_t flags,
                        bcmdrd_sid_t sid,
                        void *pt_dyn_info,
                        void *pt_ovrr_info,
                        uint32_t *entry_key_words,
                        uint32_t *entry_data_words,
                        bcmptm_op_type_t req_op,
                        size_t rsp_entry_buf_wsize,
                        bcmltd_sid_t req_ltid,
                        uint32_t *rsp_entry_key_words,
                        uint32_t *rsp_entry_data_words,
                        uint32_t *rsp_entry_index,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags);

/*!
  \brief LTM-callbacks wrapper for bcmptm_mreq_indexed_lt

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Write, Read requests, index member of pt_dyn_info is necessary
  \n for Write, Read requests, tbl_inst member of pt_dyn_info may indicate pipe
  num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] misc_info Additional information that can be specified to request
  use of DMA. Refer to definition of bcmptm_misc_info_t for details.
  \param [in] req_op Only WRITE, READ, NOP ops are valid
  \param [in] entry_words Array large enough to hold write data
  \n Dont care for READ, NOP requests.
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
  \n Dont care for WRITE, NOP requests.
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] cseq_id Commit Sequence ID.
  \n For every mreq, LTM must also provide a cseq_id - even for mreq for which
  COMMIT is not asserted.
  \n Commit sequence contains multiple mreq. Each mreq submitted to PTM could
  result in N physical table ops where N could > size of schan_fifo. Having
  cseq_id for each mreq allows PTM to track each 'bunch of pt ops' for
  every mreq.
  \n For committed sequences which requested notification, this ID will be returned
  along with notification msg (TBD).
  \n PTM cmdproc will maintain overall state 'IDLE', so we do not need to
  reserve any value (like 0, 0xFFFF) as 'reserved'.
  \n This ID will also help with crash recovery (details TBD).

  \param [in] notify_fn_ptr Will be called when committed transaction is
  pushed to HW.
  \n Meaningful ONLY for COMMIT (and not for ABORT)
  \n NULL means notification is not needed.

  \param [in] notify_info Will be returned as param with notify_fn_ptr call
  \n Meaningful ONLY when notify_fn_ptr is not NULL

  \param [out] rsp_entry_words Array large enough to hold read data
  \n Dont care for WRITE, NOP requests.
  \param [out] rsp_ltid For READs from PTcache, rsp_ltid is ltid of entry in
  PTcache. For all other cases, rsp_ltid = req_ltid. (TBD)
  \param [out] rsp_flags More detailed status (TBD)
 */
extern int
bcmptm_ltm_mreq_indexed_lt(int unit,
                           uint32_t flags,
                           bcmdrd_sid_t sid,
                           void *pt_dyn_info,
                           void *pt_ovrr_info,
                           bcmptm_misc_info_t *misc_info,
                           bcmptm_op_type_t req_op,
                           uint32_t *entry_words,
                           size_t rsp_entry_wsize,
                           bcmltd_sid_t req_ltid,
                           uint32_t cseq_id,
                           bcmptm_trans_cb_f notify_fn_ptr,
                           void *notify_info,
                           uint32_t *rsp_entry_words,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags);

/*!
  \brief LTM-callbacks wrapper for bcmptm_mreq_keyed_lt
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
  \n tbl_inst member of pt_dyn_info may indicate pipe num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] req_op LOOKUP, DELETE, INSERT ops are valid for tables
  with key.
  \n LTM can force PTM to read final entry from HW by setting
   BCMPTM_REQ_FLAGS_DONT_USE_CACHE along with LOOKUP op. This can be used to
   read Hit bits.
  \param [in] req_info See the definition for bcmptm_keyed_lt_mreq_info_t
  \param [in] cseq_id Commit Sequence ID.
  \n See earlier description for details.

  \param [out] rsp_info See the definition for bcmptm_keyed_lt_mrsp_info_t
  \param [out] rsp_ltid Previous owner of the entry
  \param [out] rsp_flags More detailed status (TBD)
  \n For Insert operation, PTM can provide additional information as below:
   - REPLACED
   - OLD_ENTRY_DELETED
 */
extern int
bcmptm_ltm_mreq_keyed_lt(int unit,
                         uint32_t flags,
                         bcmltd_sid_t req_ltid,
                         void *pt_dyn_info,
                         void *pt_ovrr_info,
                         bcmptm_op_type_t req_op,
                         bcmptm_keyed_lt_mreq_info_t *req_info,
                         uint32_t cseq_id,
                         bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                         bcmltd_sid_t *rsp_ltid,
                         uint32_t *rsp_flags);


#endif /* BCMPTM_H */
