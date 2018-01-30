/*! \file bcmptm_types.h
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

#ifndef BCMPTM_TYPES_H
#define BCMPTM_TYPES_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_error.h>


/*******************************************************************************
 * Defines
 */
/*
 * Flags available to LTM for bcmptm_ireq, mreq.
 */

/*! Default flags */
#define BCMPTM_REQ_FLAGS_NO_FLAGS                      0x00000000

/*!Meaningful only for Read request, Read portion of Read_Modify_Write requests.
 * \n 1 => Read from HW even if PT is cacheable
 * \n 0 => Read from Cache (if PT is cacheable)
 * \n This flag is not looked at for Write. Writes will always update the cache,
 if PT is cacheable,
 * \n Meaningful for both interactive (PTpassthru) path and Modeled path
 */
#define BCMPTM_REQ_FLAGS_DONT_USE_CACHE                0x00000001

/*! Meaningful only for Read requests, Read portion of Read_Modify_Write
  requests.
 * \n 1 => Return data as received from HW as is.
 * \n 0 => Attempt to convert XY format to Key, Mask format.
 * \n This flag is not looked at for Write, Read_Modify_Write requests.
 * \n Expected encoding of Key, Mask bits from LTM:
 * \n Key_bit, Mask_bit, Meaning:
 * \n 0, 1, Compare 0
 * \n 1, 1, Compare 1
 * \n 0, 0, Dont_Compare (key bit is dont_care)
 * \n 1, 0, Dont_Compare (key bit is dont_care)
 * \n Note: XY format can be different for different chips, TCAM types.
 */
#define BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM            0x00000002

/*! 1 => Commit uncommitted WAL operations as a 'commit sequence' for HW
 * \n Return ctl back to LTM (without waiting for committed ops in WAL to
 * finish execution in HW)..
 * \n Meaningful only when req_op = BCMPTM_OP_NOP
 * \n Meaningful only for Modeled path.
 */
#define BCMPTM_REQ_FLAGS_COMMIT                        0x00000004

/*! 1 => Delete all uncommitted WAL operations and revert PTcache, SW state.
 * \n Meaningful only when req_op = BCMPTM_OP_NOP
 * \n Meaningful only for Modeled path.
 * \n notify_fn_ptr will NOT be called.
 * \todo NOT YET IMPLEMENTED
 */
#define BCMPTM_REQ_FLAGS_ABORT                         0x00000020

/*! 1 => Entry insert should be to all pipes individually
 * \n though the device is in global mode.
 * This is to override the behaviour of global and per pipe mode
 * for special handling in TCAMs.
 */
#define BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL      0x00000040

/*! 1 => Interactive request is for passthru (pt) table
 *  0 => Interactive request is for LT
 *  x for Modeled path */
#define BCMPTM_REQ_FLAGS_PASSTHRU                      0x80000000

/*! 1 => entry_words in hw_entry format
 *  0 => entry_words in ctr_cache format
 *  Same value as FLAGS_PASSTHRU on purpose */
#define BCMPTM_REQ_FLAGS_CCI_HW_ENTRY_FORMAT           0x80000000

/*
 * Response flags
 */
/*! Default flags */
#define BCMPTM_RSP_FLAGS_NO_FLAGS                      0x00000000

/*!Meaningful only Insert request
 * \n 1 => Key already existed, so was replaced with new entry (data) */
#define BCMPTM_RSP_FLAGS_REPLACED                      0x00000001

/*! Maximum PT indices that can get updated during single operation */
#define BCMPTM_RSP_MAX_INDEX                           0x00000004
/*******************************************************************************
 * Typedefs
 */
/*!
 * \brief Op types for bcmptm requests (modeled path)
 */
typedef enum {
    BCMPTM_OP_NOP = 1,
    BCMPTM_OP_LOOKUP,
    BCMPTM_OP_INSERT,
    BCMPTM_OP_DELETE,
    BCMPTM_OP_GET_FIRST,
    BCMPTM_OP_GET_NEXT,
    BCMPTM_OP_WRITE,
    BCMPTM_OP_READ
} bcmptm_op_type_t;

/*! \brief Information to be passed with keyed LT request - modeled path
  */
typedef struct bcmptm_keyed_lt_mreq_info_s {

/*! \brief Array of arrays holding key fields, mask fields of an entry
  - General
   -# Array of arrays is needed for key_width expansion - different key fields of
   an LTID may map to different rows of same phys table (SID). For eg: IPv6 key
   will map to 4 consecutive rows of L3_TUNNEL table.
   -# Value of KEY_TYPEf, BASE_VALIDf fields (as needed by HW) will be written
   by PTRM. 'Written' but NOT 'Inserted' - means LTM will have 'slots' for these
   fields in entry_key_words as needed, so PTM will never need to shift bits
   in this array - even for flex-keygen cases.

  - Hash type LTs
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Only single row of entry_key_words is needed.
   -# entry_key_words is needed for all ops

  - Aggr type Pri Tcam LTs
   -# 'Aggr type' refers to Tcam tables where TCAM+DATA view is defined in
   regsfile.
   -# For Aggr type LTs, L2P mapping L2P mapping must map user fields to
   TCAM+DATA view.
   -# entry_key_words format must match with key, mask portion in TCAM+DATA view
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Array of arrays is not needed.

  - Non_Aggr type Pri Tcam LTs
   -# 'Non_Aggr type' refers to Tcam tables where TCAM+DATA view is NOT
   available in regsfile.
   -# For such tables format of entry_key_words must match with format of
   TCAM_ONLY view in regsfile.
   -# Array of arrays can be used to support key_width expansion.

  - Prefix type Tcam LTs (unmapped)
   -# LTM will provide tcam mask as bit_mask in DM format (not HW specific XY
   format). PTM will convert DM format to HW-specific XY format.
   -# PTRM will compute prefix_length and also do checking of mask bits (for
   holes)
   -# entry_key_words is needed for all ops (need to know prefix_length for
   LOOKUP, DELETE)
   -# Array of arrays is not needed.
   */
    uint32_t **entry_key_words;

/*! \brief Array of arrays holding data fields (dfields) of an entry
  - General
   -# Array of arrays is needed for key_width expansion (different key fields of
   an LTID map to different rows of same phys table (SID). For eg: IPv6 key will
   map to 4 consecutive locations of L3_TUNNEL table.
   -# Value of DATA_TYPEf, BASE_VALIDf fields (as needed by HW) will be written/
   inserted by PTRM.

  - Hash type LTs
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Only single row of entry_data_words is needed..
   -# entry_data_words can be non_zero only for INSERT op
   -# Must be ALL_ZEROS for LOOKUP, DELETE ops (only entry_key_words are
   needed for these ops).

  - Aggr type Pri Tcam LTs
   -# entry_data_words format must match with data portion in TCAM+DATA view for
   simple tables (single fixed dfield format LTs)
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Array of arrays is not needed.
   -# entry_data_words must be ALL_ZEROs for LOOKUP, DELETE ops.
   -# entry_data_words can be non-zero only for INSERT op.

  - Non_Aggr type Pri Tcam LTs
   -# For such tables format of entry_data_words must match with format of
   TCAM_DATA_ONLY view in regsfile.
   -# Array of arrays can be used to support width expansion of associated data
   (eg: fp_policy)
   -# Num of elements in entry_data_words need not be same as same as num
   elements in entry_key_words array. For example, if we have 3*160 bit key and
   2x fp_policy, we will have 3 rows in entry_key_words and 2 rows in
   entry_data_words.
   -# entry_data_words must be ALL_ZEROs for LOOKUP, DELETE ops.
   -# entry_data_words can be non-zero only for INSERT op.

  - Prefix type Tcam LTs (unmapped)
   -# entry_data_words is needed only for INSERT op.
   -# Must be ALL_ZEROs for LOOKUP, DELETE ops.
   -# Array of arrays is not needed.
   */
    uint32_t **entry_data_words;

/*! \brief Array of logical_data_type presented by LTM
  - General
   -# Must be NULL for simple tables which do not have multiple dfield formats.
   -# Needed only if LT supports multiple dfield formats and, or entry widths.
   -# Needed because for same LT, we may want to insert entry with different set
   of dfields. Eg: we may want to INSERT an entry with or without flex_counter.
   -# Meaningful only for ops where entry_data_words are provided by LTM
   -# Array is needed because pdd_l_dtyp could be different for each row of
   entry_data_words
   -# Each row of pdd_l_dtyp provides information for corresponding row of
   entry_data_words.
   -# Because every row of entry_data_words may correspond to separate SID,
   value of 0 must be used if SID does not have multiple dfield formats.
   -# Meaningful only for INSERT op. Should be NULL for other ops.
   */
    uint16_t *pdd_l_dtyp;

/*! \brief entry_id (handle) needed for priority type Tcam LTs with entry_id
  - Dont care for Hash type LTs, Prefix type Tcam LTs
  - Needed for most (but not all) Pri Tcam LTs
  - For Pri Tcam LTs with entry_id:
   -# Search for LOOKUP, DELETE ops is based on entry_id (and not the key) and
   hence entry_key_words will not be needed for these ops.

  \todo Any other comments from IDC team about 'entry_id' on LTM - PTM
  interface?
  */
    uint32_t entry_id;

/*! \brief Needed for priority type TCAM LTs
  - Dont care for Hash type LTs, Prefix type Tcam LTs
  - For Pri Tcam LTs.
   -# Dont_care for LOOKUP, DELETE ops. If user-specified entry is found, it's
   entry_pri is returned as rsp_entry_pri.
   -# Needed only for INSERT op
    - entry_pri for Inserted/Replaced entry will be set/changed to entry_pri

  \todo Any other comments from IDC team about 'entry_pri' on LTM - PTM
  interface? */
    uint32_t entry_pri;

/*! \brief LTM indicates to PTM that this is same key as previous LOOKUP op
  - Description below applies for all LTs on Modeled path.

  - Assumption for modeled path
   -# There is only one thread for modeled path and therefore only one uncommitted
   transaction in WAL per unit. PTM cannot assemble multiple concurrent transactions
   (even for different tables in WAL) for same unit.

  - After LOOKUP op, if LTM comes back with INSERT for same key, it must specify
  'same_key=TRUE'. This saves PTRM from having to redo the search (which may
  involve re-computing hash vector, bucket, index, lookup related state, etc for
  this key).
   -# For Hash type LTs, PTRM must save the hash_vector and bucket related information for LAST
   lookup operation from LTM for last LOOKUP op.
   -# For TCAM type LTs, PTRM can save info related to last LOOKUP op.

  - As per BCMLT op discussions::
   -# API req for INSERT must result in error if
   entry already exists. This requires LTM to first issue LOOKUP and only if
   entry does not exist it will issue INSERT with same_key=TRUE. Because previous
   LOOKUP failed, PTM will attempt to insert new entry when it gets INSERT. In
   this case, LTM must set same_key TRUE for INSERT op.
   -# API req for UPDATE must result in error if entry does not exist. This requires
   LTM to first issue LOOKUP and only if entry exists it will issue INSERT with
   same_key=TRUE. Because previous LOOKUP succeeded, PTM will either: 'replace
   entry' or 'insert new entry and delete old entry'.
   -# In both of above cases, when LOOKUP is followed by INSERT, LTM must
   provide same_key=TRUE with INSERT op to PTM.

  - Observation:
   -# same_key MUST always be FALSE for LOOKUP op
   -# same_key MUST always be FALSE for DELETE op
   -# same_key MUST always be 1 for for INSERT op.
   If LTM can guarantee above behavior we dont need 'same_key' param -
   PTRM can implicitly use the expected value. */
    bool same_key;

/*! \brief Num of words in rsp_entry_key_buf array
  - PTM will assume all rows of rsp_entry_key_words to be of same size */
    size_t rsp_entry_key_buf_wsize;

/*! \brief Num of words in rsp_entry_data_buf array
  - PTM will assume all rows of rsp_entry_data_words to be of same size */
    size_t rsp_entry_data_buf_wsize;

/*! \brief Atrributes of entry needed for special type of TCAMs like FP.
 *   - PTRM will convert to required type and use the information. */
    void *entry_attrs;
} bcmptm_keyed_lt_mreq_info_t;

/*! \brief Information to be returned for keyed LT request - modeled path
  */
typedef struct bcmptm_keyed_lt_mrsp_info_s {

/*! \brief Key, mask fields of entry found (all ops)
  - General
   -# PTM will not touch this array if entry was not found (all ops).
   -# It is assumed that INSERT op will always be preceded by LOOKUP op, so
   'old_entry' was already returned to LTM and thus, there is no need for PTM to
   return this information again for subsequent INSERT-found case (for which
   PTRM may replace data or delete old entry).

  \todo LTM team must confirm above assumption.

  - Need to separate out rsp_entry_key_words, rsp_entry_data_words to support
  returning of data in following cases:
   -# data could have multiple dfield_formats
   -# data could be encoded in PDD
   -# data from Non_aggr type Tcam LTs

  - Hash type LTs, Aggr type Pri Tcam LTs, Prefix type Tcam LTs (unmapped)
   -# Only single row of entry_key_words is returned.
   -# For entry found case, rsp_entry_key_words may also contain data_fields.
   LTM must only pick 'key' fields (and ignore non-key fields).

  - Non_Aggr type Pri Tcam LTs
   -# Array of arrays can be used to return key, mask fields for Non_Aggr type
   Pri LTs with key expansion.
   -# rsp_entry_key_words will contain TCAM_ONLY fields
  */
    uint32_t **rsp_entry_key_words;

/*! \brief Data fields of entry found (all ops)
  - General
   -# PTM will not touch this array if entry was not found (all ops).
   -# It is assumed that INSERT op will always be preceded by LOOKUP op, so
   'old_entry' was already returned to LTM and thus, there is no need for PTM to
   return this information again for subsequent INSERT-found case (for which
   PTRM may replace data or delete old entry).

  \todo LTM team must confirm above assumption.

  - Hash type LTs, Aggr type Pri Tcam LTs, Prefix type Tcam LTs (unmapped)
   -# Only single row of entry_data_words is returned.
   -# For entry found case, rsp_entry_data_words may also contain non-data fields.
   LTM must only pick 'data' fields (and ignore non-data fields).

  - Non_Aggr type Pri Tcam LTs
   -# Array of arrays can be used to return key, mask fields for Non_Aggr type
   Pri LTs with data expansion.
   -# rsp_entry_key_words will contain TCAM_DATA_ONLY fields
  */
    uint32_t **rsp_entry_data_words;

/*! \brief Logical_data_type for entry found
  - General
   -# Meaningful only for cases where entry_data_words is returned by PTM
   -# Will be NULL for entry not found case.
   -# Will be NULL for INSERT op case (because INSERT was preceded by LOOKUP and
   so old_entry was already returned).
   -# Each row corresponds to row of rsp_entry_data_words
   -# Different data fields could be present/absent in every row of
   rsp_entry_data_words and hence we need corresponding row of pdd_l_dtyp
   -# Will be NULL for simple tables where data fields have only one format.
   -# Because every row of rsp_entry_data_words may correspond to separate SID,
   value of 0 must be used if SID does not have multiple data field formats.
   */
    uint16_t *rsp_pdd_l_dtyp;

/*! \brief Returned for priority type LTs
  - Hash type LTs
   -# Same value received with request (should be dont_care in LTM)

  - Prefix type LTs
   -# Same value received with request (should be dont_care in LTM)

  - Pri type LTs
  \todo Must ask IDC to fill description on rsp_entry_pri
  */
    uint32_t rsp_entry_pri;

/*! \brief count of valid entries in rsp_entry_index array */
    uint32_t rsp_entry_index_cnt;

/*! \brief Index at which entry was found (all ops)
  - For all ops, if entry was found, PTRM will return index of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT op, PTRM will return index of new entry if INSERT succeeded.  */
    uint32_t rsp_entry_index[BCMPTM_RSP_MAX_INDEX];

/*! \brief count of valid entries in rsp_entry_sid array */
    uint32_t rsp_entry_sid_cnt;

/*! \brief SID of PT where entry was found (all ops)
  - For all ops, if entry was found, PTRM will return SID of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT, PTRM will return SID of new entry if INSERT succeeded. */
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RSP_MAX_INDEX];

/*! \brief count of valid entries in rsp_entry_sid_data array */
    uint32_t rsp_entry_sid_data_cnt;

/*! \brief SID of PT where entry data was found (all ops)
  - For all ops, if entry data was found, PTRM will return SID of found entry data.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT, PTRM will return SID of new entry if INSERT succeeded. */
    bcmdrd_sid_t rsp_entry_sid_data[BCMPTM_RSP_MAX_INDEX];

/*! \brief count of valid entries in rsp_entry_index_data array */
    uint32_t rsp_entry_index_data_cnt;

/*! \brief Index at which entry data was found (all ops)
  - For all ops, if entry was found, PTRM will return index of found entry data.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT op, PTRM will return index of new entry if INSERT succeeded. */
    uint32_t rsp_entry_index_data[BCMPTM_RSP_MAX_INDEX];

/*! \brief Pipe number of PT where entry was found, successfully inserted.
  - In case of LOOKUP, DELETE ops, if entry was found, PTRM will return
  pipe_number of PT where entry was found. If entry was not found, PTRM will
  not set this var or set it to 0.
  - For successful INSERT op, PTRM will return pipe_number of PT where entry
  was inserted. Else, it will not set the var or set it to 0.
  - For LTs in global mode, RM will return 0.
  - For LTs in pipe-unique mode, RM will return pipe_num where entry was found
  or successfully inserted. */
    uint32_t rsp_entry_pipe;

/*! Aggregate view status. */
    bool aggregate_view;
} bcmptm_keyed_lt_mrsp_info_t;

/*!
 * \brief Define transaction operation status and callback.
 *
 * All logical table transaction operations result in a status
 * to be returned to the caller. This callback function is invoked
 * upon completion of a transaction.  The LTM module passes this
 * callback onward to the PTM so that module may notify when all portions
 * of a modelled transaction are complete.
 *
 * \param [in] trans_id Transaction ID of this notification.
 * \param [in] status Operation status indication.
 * \param [in] context Cookie supplied when callback registered.
 *
 * \return void None.
 */
typedef void (*bcmptm_trans_cb_f)(uint32_t trans_id,
                                  shr_error_t status,
                                  void *context);

/*! \brief Information to be passed with LT request.
  */
typedef struct bcmptm_misc_info_s {

/*! Enable DMA.
 * - Currently supported only for Indexed tables.
 */
    bool dma_enable;

/*! Number of entries for DMA.
 * - Meaningful only when dma_enable is TRUE.
 */
    uint32_t dma_entry_count;
} bcmptm_misc_info_t;

#endif /* BCMPTM_TYPES_H */
