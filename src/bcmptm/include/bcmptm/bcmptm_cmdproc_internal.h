/*! \file bcmptm_cmdproc_internal.h
 *
 * This file contains APIs, defines for cmdproc.
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

#ifndef BCMPTM_CMDPROC_INTERNAL_H
#define BCMPTM_CMDPROC_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_types.h>


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
  \brief Update HW table entry (through WAL) and PTcache
  - Provides independent control for posting of Write request to WAL queue and
  update of cache entry
  - When writing entry in cache,
   -# 'cache_vbit' for this entry can be set to 1 or 0
   -# req_ltid, req_dfield_format_id will also be stored as 'meta-data for this
   cache entry.

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
           \n Pass ZEROs otherwise.
  \param [in] cseq_id Id for current trans
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
           \n Must specify index
           \n For entry specified by LTM op, PTRM must pass tbl_inst as
           provided by LTM
           \n When moving entry from one location to another, PTRM must
           still use tbl_inst as provided by LTM.
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
           \n Pass NULL
           \n Does PTRM ever try to override sbus fields? TBD
  \param [in] misc_info Information needed when writing multiple entries.
  \param [in] entry_words Array large enough to hold write_data
  \param [in] write_hw 1 => Update HW entry (add write op to WAL)
                    \n 0 => HW entry is not updated
  \param [in] write_cache 1 => Update cache entry with entry_words
                       \n 0 => Cache entry is not updated
  \param [in] set_cache_vbit Meaningful ONLY if write_cache = 1
                       \n vbit is stored for every cache_entry and indicates
                       whether cache_entry was written SW or not.
                       \n 1 => Make vbit TRUE for this cache_entry
                       \n 0 => Make vbit FALSE for this cache_entry
                       \n cache_vbit state will NOT change if write_cache = 0
  \param [in] op_type See definition for bcmptm_rm_op_t.
  \param [in] slice_mode_change_info Information needed to change slice_mode.
  \param [in] req_ltid Logical Table enum that is accessing the table
              \n Will get stored in PTcache
  \param [in] req_dfield_format_id dfield_format of the entry
              \n Will get stored in PTcache
  \param [out] rsp_flags More detailed status (TBD)
            \n Return ZEROs for now

  \retval SHR_E_NONE Success
  \retval SHR_E_FAIL Example: if write_cache=1 and PT is not cacheable, etc */
extern int
bcmptm_cmdproc_write(int unit,
                     uint32_t flags,
                     uint32_t cseq_id,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                     void *pt_ovrr_info,
                     bcmptm_misc_info_t *misc_info,
                     uint32_t *entry_words,
                     bool write_hw,
                     bool write_cache,
                     bool set_cache_vbit,
                     bcmptm_rm_op_t op_type,
                     bcmptm_rm_slice_change_t *slice_mode_change_info,
                     bcmltd_sid_t req_ltid,
                     uint16_t req_dfield_format_id,

                     uint32_t *rsp_flags);

/*!
  \brief Read latest table entry (from PTcache and, if requested, from HW)
  - Entry_words from cache will be returned - irrespective of whether
  'cache_vbit' for this entry was 0 or 1. See Return Values for this function.
  - If read from HW is requested, entry_data_words will be from HW -
  irrespective of whether cache_vbit is 0 or 1.
  - ltid, dfield_format_id stored with cache_entry will also be returned. If
  entry was requested from HW and it was not valid in cache, LTID_RSP_HW,
  DFID_RSP_HW will be returned.

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
           \n Asserting of BCMPTM_REQ_FLAGS_DONT_USE_CACHE will result in read
           of entry from ptcache - to fetch LTID, DFID meta-data and then will
           read entry from HW via WAL - to fetch hw_entry_words.
           \n Also, in this case, wal_read() will use BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM.
           \n If entry is not valid in cache and read from HW was requested,
           cmdproc will return LTID_RSP_HW and DFID_RSP_HW as meta-data.
           \n Pass ZEROs for all other bits.
  \param [in] cseq_id Id for current trans
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
           \n Must specify index
           \n For entry specified by LTM op, PTRM must pass tbl_inst as
           provided by LTM
           \n When moving entry from one location to another, PTRM must
           still use tbl_inst as provided by LTM.
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
           \n Pass NULL
           \n Does PTRM ever try to override sbus fields? TBD
  \param [in] misc_info Info needed when reading multiple entries.
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array

  \param [out] rsp_entry_words Array large enough to hold read data
  \param [out] rsp_entry_cache_vbit 'cache_vbit (vbit)' state of entry in cache
  \param [out] rsp_ltid ltid of entry in cache. Meaningful only if
  rsp_entry_cache_vbit=1
  \param [out] rsp_dfield_format_id dfield_format_id of entry in cache.
  Meaningful only if rsp_entry_cache_vbit=1
  \param [out] rsp_flags More detailed status (TBD)
            \n Ignore for now

  \retval SHR_E_NONE Success.
  - rsp_entry_cache_vbit specifies whether the cache entry was written with
  that view or not.
  - Hash_overlay tables:
   - Separate cache_vbit is maintained for each entry of all views.
   - PTcache only caches the 4x view. Writes to 1x, 2x, 4x views change contents
   of 4x view entry. And thus rsp_entry_words are correct even if rsp_cache_vbit=0
   - For example: if 4x view at addr 0 was written with {A,B,C,D} and if we read
   1x view at addr 0, 1, 2, 3 - read will return A, B, C, D respectively - and
   for each of 1x read we will return rsp_cache_vbit=0. Hash_rsrc_mgr should not
   consider this is an error.
  - Tcam_overlay tables:
   - Separate cache_vbit is maintained for each entry of all views.
   - PTcache has separate cache for 1x, 2x view. And thus, rsp_entry_words are
   not correct if rsp_entry_cache_vbit=0
   \todo Sprint3 POR is to not implement overlay views for same underlying TCAMs
   in pt_cache. rsrc_mgr for IFP_TCAM, L3_DEFIP tables must not assume that
   writes to 1x view will update wide_view in cache and vice-versa.
  \retval SHR_E_UNAVAIL Cache is not allocated for this PT. rsp_entry_words are
  not correct. This will happen even if read from HW is requested - because on
  modeled path all tables need to be cacheable.
 */

extern int
bcmptm_cmdproc_read(int unit,
                    uint32_t flags,
                    uint32_t cseq_id,
                    bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                    void *pt_ovrr_info,
                    bcmptm_misc_info_t *misc_info,
                    size_t rsp_entry_wsize,

                    uint32_t *rsp_entry_words,
                    bool *rsp_entry_cache_vbit,
                    bcmltd_sid_t *rsp_ltid,
                    uint16_t *rsp_dfield_format_id,
                    uint32_t *rsp_flags);

/*!
  \brief Get ptrm_info for an LT
  - PTRM will use this API to access info needed on per LT basis
  - For INSERT op, PTRM will get dfield_format_id from PTM and thus can get
  rsp_index_hw_entry_info, rsp_hw_data_type_val for this entry.
  - For LOOKUP, DELETE op, PTRM will not get dfield_format_id from PTM, but from
  the Cmdproc (when requesting read for found entry from PTcache). PTRM is
  expected to walk through all elements of hw_entry_info array and find the
  entry with matching key.
  - For entry being moved from one location to another, dfield_format_id comes
  from PTcache. PTRM can then query ptrm_info_get with dfield_format_id_valid=1
  to find rsp_hw_data_type_val - necessary when moving from one mem instance to
  another. For any LT, hw_data_type_val depends on dfield_format_id and also on
  mem instance - hence hw_data_type_val is not part of hw_entry_info struct..

  \param [in] unit Logical device id
  \param [in] req_ltid enum for Logical Table
  \param [in] dfield_format_id enum for dfield_format_id
  \param [in] dfield_format_id_valid Indicates if dfield_format_id is valid
  \n For LOOKUP, DELETE ops, PTRM does not have dfield_format_id
  \n For INSERT op, PTRM gets dfield_format_id from PTM
  \n For entries in cache, PTRM gets dfield_format_id as return value from
  bcmptm_cmdproc_read()

  \param [out] rsp_ptrm_info Ptr to info needed by PTRM.
  \n PTM will fill members of struct allocated by PTRM.
  \n For TCAM type LT, this should point to bcmptm_rm_tcam_lt_info_t
  \n For TCAM_um type LT, this should point to bcmptm_rm_tcam_um_lt_info_t
  \n For Hash type LT, this should point to bcmptm_rm_hash_lt_info_t
  \n Will be valid even if dfield_format_id_valid=0

  \param [out] rsp_index_hw_entry_info Index into hw_entry_info array
  \n Not valid if dfield_format_id_valid was 0

  \param [out] rsp_hw_data_type_val Ptr to data_type values
  \n Not valid if dfield_format_id_valid was 0
  \n Ptr to Ptr is used as it is expected that LRD will hold the info and PTM
  is only passing the ptr to that info.
  \n PTRM-HASH
  - Is needed when moving entry from one bucket (bank) to another bucket (bank)
  each of which could be separate SID with diff data_type_val.
  \n PTRM-TCAM
  - For each dfield_format_id, LRD will have an array of data_type_values.
  - Each row in data_type_val array corresponds to depth_instance. Each
  depth_instance could in separate tile.
  - Is needed for PTRM-TCAM when we move an entry from one depth instance to
  another (each instance is in separate Tile with separate data_type_val)

  \retval SHR_E_NONE Success
  \retval SHR_E_UNAVAIL if rsp_ptrm_info is not avail for this LT */
extern int
bcmptm_cmdproc_ptrm_info_get(int unit,
                             bcmltd_sid_t req_ltid,
                             uint16_t dfield_format_id,
                             bool dfield_format_id_valid,

                             void *rsp_ptrm_info,
                             int *rsp_index_hw_entry_info,
                             uint16_t **rsp_hw_data_type_val);

#endif /* BCMPTM_CMDPROC_INTERNAL_H */
