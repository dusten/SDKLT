/*! \file bcmptm_rm_hash_internal.h
 *
 * APIs, defines for PTRM-Hash interfaces
 *
 * This file contains APIs, defines for PTRM-Hash interfaces
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

#ifndef BCMPTM_RM_HASH_INTERNAL_H
#define BCMPTM_RM_HASH_INTERNAL_H

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
#include <bcmptm/bcmptm_uft_internal.h>


/*******************************************************************************
 * Defines
 */
/*! Max move depth for hash reordering.  */
#define BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH   6

/*! Default move depth for hash reordering.  */
#define BCMPTM_RM_HASH_DEF_HASH_REORDERING_DEPTH   4

/*! Max hash bank count for a physical table. */
#define BCMPTM_RM_HASH_MAX_BANK_COUNT  8

/*! Number of robust hash modules for a hash table */
#define BCMPTM_RM_HASH_RHASH_MODULES_NUM         2

/*! Entry word size for action and remap tables */
#define BCMPTM_RM_HASH_RHASH_ENTRY_WORDS_MAX     4

/*! Bitsize of robust hash added for an entry key */
#define BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH     16

/*! Remap table size */
#define BCMPTM_RM_HASH_RHASH_REMAP_TABLE_SIZE    256

/*! Action table size */
#define BCMPTM_RM_HASH_RHASH_ACTION_TABLE_SIZE   256

/*! Action table width */
#define BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH  64

/*! Action table depth */
#define BCMPTM_RM_HASH_RHASH_ACTION_TABLE_DEPTH  4
/*******************************************************************************
 * Typedefs
 */
/*! \brief All the possible hash algorithms */
typedef enum {
    BCMPTM_RM_HASH_VEC_ZERO,
    BCMPTM_RM_HASH_VEC_CRC32,
    BCMPTM_RM_HASH_VEC_CRC16,
    BCMPTM_RM_HASH_VEC_LSB,
    BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB,
    BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO,
    BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
    BCMPTM_RM_HASH_VEC_CRC32A_LSB_ZERO
} bcmptm_rm_hash_vector_type_t;

/*! \brief All the possible hash modes
  - Dual hash and shared hash have different entry index calculating mechanism
  w.r.t hash bucket. */
typedef enum {
    BCMPTM_RM_HASH_TYPE_DUAL,
    BCMPTM_RM_HASH_TYPE_SHARED
} bcmptm_rm_hash_type_t;

/*! \brief Attributes for hash vector at a per hank basis
  - For some devices, attribute info of each bank can correspond to separate SID.
  - For fixed-func devices, attribute info of all banks corresponds to a single
  SID. */
typedef struct bcmptm_rm_hash_vector_bank_attr_s {

/*! \brief hash vector algorithm used to do hash calculation.
  - An algorithm used for generating the hash vector based on the table entry key
  for a given bank. */
    bcmptm_rm_hash_vector_type_t type;

/*! \brief Mask to the hash vector
  - A mask applied to the hash vector for a given physical bank to get the
    final bucket pointer in that bank.
  - Mostly this is calculated by bucket_count - 1 */
    uint32_t mask;

/*! \brief Offset to the hash vector
  - Is used to apply cyclic shift on hash vector to get the
    specific range of hash vector so that different physical bank will get
    different bucket based on same hash vector. */
    uint16_t offset;

/*! \brief Whether or not robust hash enabled
  - If there is no corresponding robust hash for a given LTID, this is false.
  - If there is corresponding robust hash for a given LTID, but user chooses
    to disable it via complier, this is false, otherwise it should be true. */
    bool en_robust;

/*! \brief SID of corresponding robust remap table.
  - This SID corresponds to the provided logical
    table on a specific physical bank. This will be used to query the ptcache
    to get the remap entry via a index calculated based on the provided key. */
    bcmdrd_sid_t robust_remap_sid;

/*! \brief Width of REMAP_DATAf for robust remap table.
  - REMAP_DATA is a random value used to append or prepend the hash key to add
    entropy of the key. */
    uint16_t remap_dfield_width;

/*! \brief Start bit of REMAP_DATAf for robust remap table.
  - REMAP_DATA is a random value used to append or prepend the hash key to add
    entropy of the key. */
    uint16_t remap_dfield_start_bit;

/*! \brief SID of corresponding robust action table.
  - This SID is corresponding to the provided logical
    table on a specific physical bank. This will be used to query the ptcache
    to get the action entry via a index calculated based on the provided key */
    bcmdrd_sid_t robust_action_sid;

/*! \brief Width of ACTIONf for robust action table.
  - ACTION is used to indicate appending or prepending the hash key with the remap
    data to add entropy of the key. */
    uint16_t action_dfield_width;

/*! \brief Start bit of ACTIONf for robust action table.
  - ACTION is used to indicate appending or prepending the hash key with the remap
    data to add entropy of the key. */
    uint16_t action_dfield_start_bit;

/*! \brief hw logical bank number for the physical bank.
  - This field is only applicable to the devices that support logical bank in HW, else
    the value of it will be ignored. */
    uint8_t logical_bank;
} bcmptm_rm_hash_vector_bank_attr_t;

/*! \brief Attributes for hash vector at a per logical table basis */
typedef struct bcmptm_rm_hash_vector_attr_s {

/*! \brief width of KEY_LSBf for a specific LTID.
  - The LSB of a key can be directly used as a hash vector,
    this is mainly for debug purpose. For a given LTID, the LSB is fixed.
  - Same for all banks */
    uint16_t hw_lsb_field_width;

/*! \brief start_bit position of KEY_LSBf
  - MUST be same for all banks */
    uint16_t hw_lsb_field_start_bit;

/*! \brief Key length used to do hash calculation.
  - This is the maximal key length among all the key types supported in by
  hw_entry. It is used to calculate the hash vector. Is used to pad zeros if
  the actual (compare) key length is less, because for a same SID, the hash
  calculation assumes key_length is always the maximal one. */
    uint16_t key_length;

/*! \brief Array of bank attributes for a given LTID.
  - Number of elements = num_banks (as declared above)
  - Separate hash_vector info is needed for each bank (irrespective of whether
  they map to same / different SID values). */
    const bcmptm_rm_hash_vector_bank_attr_t * bank_attr;
} bcmptm_rm_hash_vector_attr_t;


/*! \brief Information specific to hw_entry for this LT
  - hw_entry_info refers to num, position of KEY_TYPEf, DATA_TYPEf, and also
  value of KEY_TYPEf - but not value of DATA_TYPEf. Even though a LT may support
  large num of dfield_formats, each with different value for DATA_TYPEf,
  underlying hw_entry_format choices are limited to few HW views (like 1x, 2x,
  etc)
  - For Single_Data_Format type LTs:
   -# There will be only single  hw_entry_info
  - For Dual_Data_Format type LTs where KEY_TYPE implicitly acts as DATA_TYPE:
   -# There will be only two hw_entry_info
  - For Multiple_Data_Format type LTs
   -# max num of hw_entry_info will depend on 1x, 2x, 4x, etc views
   offered in regsfile (for ALPM we can have 70b, 105b, 140b, 210b views) */
typedef struct bcmptm_rm_hash_hw_entry_info_s {

/*! \brief SID for each physical bank
  - Num elements in array = bcmptm_rm_hash_lt_info_t.num_bank_sid.
  For some devices, each physical bank can have a separate SID */
    const bcmdrd_sid_t *sid;

/*! \brief HW details of KEY_TYPEf in entry
  - MUST be same for all banks
  - NOT an array - but pointer to structure maintained by LRD
  - NULL means underlying PTs don't have KEY_TYPEf */
    const bcmlrd_hw_rfield_info_t *hw_ktype_field_info;

/*! \brief width of DATA_TYPEf
  - MUST be same for all banks
  - 0 means underlying PTs don't have DATA_TYPEf (eg: hash tables which
  support only single data_format) */
    uint16_t hw_dtype_field_width;

/*! \brief start_bit position of DATA_TYPEf
  - MUST be same for all banks
  - DontCare if DATA_TYPEf does not exist */
    uint16_t hw_dtype_field_start_bit;

/*! \brief Size of hw_entry in bits
  - Bitsize of an entry in this table view
  - It is used for validating response entry buffer size. */
    uint16_t entry_bitsize;

/*! \brief Number of base entries for this view.
  - num_base_entries describes how many base entries are occupied for an
  entry in this table view. */
    uint8_t  num_base_entries;
} bcmptm_rm_hash_hw_entry_info_t;

/*! Group Modes supported */
typedef enum  bcmptm_rm_hash_entry_mode_e {
    /*! Entry Mode none. */
    BCMPTM_RM_HASH_ENTRY_MODE_NONE = 0,

    /*! Exact Match Mode 128. */
    BCMPTM_RM_HASH_ENTRY_MODE_EM_128,

    /*! Exact Match Mode 160. */
    BCMPTM_RM_HASH_ENTRY_MODE_EM_160,

    /*! Exact Match Mode 320. */
    BCMPTM_RM_HASH_ENTRY_MODE_EM_320,

    /*! Using MPLS_ENTRY table. */
    BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_MPLS,

    /*! Using L3_TUNNEL_SINGLE table */
    BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_L3_TNL,

    /*! Entry Mode Count. */
    BCMPTM_RM_HASH_ENTRY_MODE_COUNT,
} bcmptm_rm_hash_entry_mode_t;

/*! This structure is passed from FP to PTM on
 * every INSERT/DELETE/UPDATE operations of
 * entries in FP_EM_ENTRY table. FP will
 * pass this as a void pointer to PTM and FP
 * subcomponent in PTM module will typecast
 * the void pointer to this structure type.
 */
typedef struct bcmptm_rm_hash_entry_attrs_s {

       /*! Mode(EM_128/EM_160/EM_320) of this etnry.
        * This information is required for every operation.
        */
       bcmptm_rm_hash_entry_mode_t entry_mode;

} bcmptm_rm_hash_entry_attrs_t;


/*! \brief Additional info for HASH based LTs accessed on modeled path.
  */
typedef struct bcmptm_rm_hash_more_info_s {

/*! \brief Hash bank mode for a given LTID
  - The addressing schemes for dual hash and shared hash are different.
  - bank_type is used to determine how to get the entry index based on
  bucket value. */
    bcmptm_rm_hash_type_t hash_type;

/*! \brief mode for a entry.
  - It is just used for FP_EM_ENTRY LT. */
    bcmptm_rm_hash_entry_mode_t entry_mode;

/*! \brief Array to hold values of KEY_TYPEf to be used for each bank
  - Array is needed for case where PT in each block may be assigned
  a different KEY_TYPE value.
  - Num elements in array = num_bank_sid
  - NULL means underlying PTs (banks) don't have KEY_TYPEf
  - Except for LTs where KEY_TYPE implicitly acts as DATA_TYPE, contents of this
 array will be same for all hw_entry_info (1x, 2x, ...) of an LT and thus all
 can point to same array of key_type_val */
    const uint16_t *key_type_val;

/*! \brief List of key fields in entry.
  - Used for hash_vector extraction, so ORDER of the fields MUST match the order
  in which HW uses these fields for hash_vector extraction.
  - Also used to compare key fields and check for presence of key in hw_entry
  - Same for all underlying banks
  - Must include KEY_TYPEf (only ONCE (first occurrence) - because we want to
  use these for hash_vector generation)
  - Must NOT include BASE_VALIDf in key portion - to allow these fields to be
  used for hash_vector generation and for compare */
    const bcmlrd_hw_field_list_t *hw_key_fields;

/*! \brief Num of banks with separate SIDs (for same dfield_format)
  - For case where 'PTs in different block', 'banks in same block'
  have different regsfile view, there will be multiple SIDs (banks) for same
  dfield_format.
  - For fixed-func devices there is single regsfile view for all banks, and
  also no blocks, so there will be only one SID (bank) per dfield_format.
  - In other words, num_bank_sid represents num of banks of hash table each of
  which has separate SID
  - Will be same for all bcmptm_rm_hash_hw_entry_info_t
  - Specifies num of elements in sid, key_type_val, data_type_val arrays. */
    uint8_t num_bank_sid;

/*! \brief Specifies whether HW table uses BASE_VALIDf
  - TRUE means HW uses BASE_VALIDf
  - Start bit position, width, value of BASE_VALIDf are chip-wide constants */
    bool en_base_valid;

/*! \brief Hash type LT uses KEY_TYPEf also as DATA_TYPEf
  - For older devices - two fixed dfield formats for same table
  are implemented using two KEY_TYPE values which differ in LSB.
  Basically, LSB of KEY_TYPE value acts like DATA_TYPE and thus there is no
  explicit DATA_TYPEf needed. Here is how it works:
   - KEY_TYPE LSB is set to 0 for hash_vector computation and locate the bucket.
   - KEY_TYPEf LSB is ignored for key comparison (when searching for entry)
   - KEY_TYPEf LSB is returned for found entry.
   - KEY_TYPEf LSB is stored for insert
   - There could be multiple KEY_TYPEf in entry - but all with same value.
   - dfield_format_count, hw_entry_info_count WILL be 2 when hash_ktype_as_dtype
   is TRUE. */
    bool lt_hash_ktype_as_dtype;

/*! \brief Attributes needed to compute hash_vector
  - See defn for bcmptm_rm_hash_vector_attr_t */
    const bcmptm_rm_hash_vector_attr_t *hash_vector_attr;

/*! \brief Bitmap to indicate possible hw_entry_info for this LT
  - Needed for SEARCH, DELETE ops where we dont know whether we are looking for
  1x 2x or 4x type entry. This bitmap identifies the possible hw_entry formats
  for given LT.
  - Bit 0,1,2 corresponds to hw_entry_index=0,1,2 respectively */
    uint8_t hw_entry_valid_bitmap;

/*! \brief Number of banks for a LTID
  - For some devices, num_banks can be same as num_bank_sid.
  - For fixed-func devices (where all banks appear as one SID)
  - 'num_banks' must include sum of dedicated banks and shared UFT banks
  associated with this LT.
  - For example, if two UFT banks are used as L2_ENTRY table, and if num
  of dedicated banks for L2_ENTRY is 3, then 'num_banks' will be 2+3 = 5. But,
  because all banks map to same SID, bcmptm_rm_hash_lt_info_t.num_bank_sid will
  be 1 */
    uint8_t num_banks;

/*! \brief Number of base entries in a hash bucket
  - num_base_entries describes how many base entries are contained in a hash
  bucket. For dual hash table, usually there are 8 base entries in a bucket.
  For shared hash table, there are 4 base entries
  in a bucket. This information is required for calculating entry index based
  on hash bucket. */
    uint8_t num_base_entries;

/*! \brief Bitmap to indicate valid physical banks for this LT
  - Each bit in the bitmap will correspond to a physical hash bank. If a bit
  is set, corresponding physical bank is valid for this LT. Else, the physical
  bank is invalid for the LT. */
    uint16_t valid_pbank_bitmap;

/*! \brief flag to indicate if hw support logical bank for this LT
  - For fixed-func devices, HW supports logical bank concept, and there exist
  registers to configure the mapping between logical table and physical
  table. This flag will be TRUE.
  - For some devices, there may be no logical bank concept. The flag will
  be FALSE */
    bool hw_logical_bank_mapping;
} bcmptm_rm_hash_more_info_t;


/*! \brief Information specific to LT needed by PTRM
  - PTRM can get this info by calling bcmptm_cmdproc_ptrm_info_get()
  - Info is defined to support LOOKUP, DELETE, INSERT for:
   -# Simple Tables (one LTID => one SID, one dfield_format)
   -# For some older devices, (SID, KEY_TYPE) are used as DATA_TYPE
   -# For newer devices, single KEY_TYPE may correspond to several
   DATA_TYPEs- Also, we need SW (device independent) version of 'key_attr_table
  entry' - so SW does not have to read cached version of key_type_attr table
  entry */
typedef struct bcmptm_rm_hash_lt_info_s {

/*! \brief Num of hw_entry formats for this LT.
  - LT may map to one of the 1x, 2x, 4x, etc hw views.
  - Num of elements in bcmptm_rm_hash_hw_entry_info_t array. */
    uint16_t hw_entry_info_count;

/*! \brief Ptr to array which holds hw_entry_info for this LT
  - LT may map to one of the 1x, 2x, 4x, etc hw views. PTRM needs information
  about KEY_TYPEf, DATA_TYPEf, KEY fields, etc for each of these views. */
    const bcmptm_rm_hash_hw_entry_info_t *hw_entry_info;

/*! \brief Length of array rm_more_info */
    uint32_t rm_more_info_count;

/*! \brief Additional info needed by rsrc_mgr
  - See defn for bcmptm_rm_hash_more_info_t */
    const bcmptm_rm_hash_more_info_t *rm_more_info;
} bcmptm_rm_hash_lt_info_t;

/*! \brief Information to be passed to PTRM for each LTM op */
typedef struct bcmptm_rm_hash_req_s {

/*! \brief Ptr to array holding entry words
  - Format of fields in this array will match regsfile key, data fields format
  of underlying PT
  - Value of KEY_TYPEf, DATA_TYPEf fields (as needed by HW) must be written by PTRM
  - For LOOKUP, DELETE ops
   - Only key portion of the entry will be provided
   - KEY_TYPE, BASE_VALID values must be filled by PTRM
  - For INSERT op
   - key, data portion of the entry will be provided
   - KEY_TYPE, DATA_TYPE, BASE_VALID values must be filled by PTRM */
    uint32_t *entry_words;

/*! \brief Tells PTRM that this is same key as previous op
  - PTRM must save the hash_vector and bucket related information for LAST
   lookup operation from LTM. If LTM comes back with Insert for same key, it must
   specify 'same_key=TRUE'. This saves PTM from having to re-compute the hash_vector,
   bucket, index, lookup related state, etc for this key.
   -# same_key MUST always be FALSE for LOOKUP op
   -# same_key MUST always be FALSE for DELETE op
   -# same_key MUST always be TRUE for for INSERT op. We don't need it if LTM can
   guarantee that INSERT to PTM will always be preceded by LOOKUP for same key.
   In short, if LTM can guarantee above behavior we dont need 'same_key' param -
   PTRM can implicitly use the expected value.
   -# As per BCMLT op discussions, API req for INSERT must result in error if
   entry already exists. This requires LTM to first issue LOOKUP and (only if
   entry does not exist) then issue INSERT with same_key=TRUE. Because previous
   LOOKUP failed, PTM will attempt to insert new entry when it gets INSERT. Thus
   LTM must always set same_key TRUE for INSERT op.
   -# As per BCMLT op discussions, API req for UPDATE must result in error if
   entry does not exist. This requires LTM to first issue LOOKUP and (only if
   entry exists) then issue INSERT with same_key=TRUE. Because previous LOOKUP
   succeeded, PTM will either: 'replace entry' or 'insert new entry and delete
   old entry'.
   -# ASSUME: We have only one thread for modelled path and one uncommitted
   transaction in WAL. We cannot assemble multiple concurrent transactions (even
   for different tables in WAL) */
    bool same_key;

/*! \brief id for logical dfield_format
  - PTM (LTM) will provide this info ONLY for INSERT ops.
   -# PTM does not have this info for LOOKUP, DELETE commands.

  - When writing the entry (INSERT)
   -# PTRM must provide dfield_format_id with Writes to CmdProc.

  - When searching for entry (for any op)
   -# PTRM must ignore this value
   -# Will be returned by CmdProc during reads of found entry. PTRM must return
   this as part of rsp.
 */
    uint16_t dfield_format_id;

/*! \brief Atrributes of entry needed for special type of HASHs like FP_EM.
  - PTRM will convert to required type and use the information. */
    void *entry_attrs;

} bcmptm_rm_hash_req_t;


/*! \brief Info returned for hash table request - modelled path */
typedef struct bcmptm_rm_hash_rsp_s {

/*! \brief Index at which entry was found (all ops)
  - For all ops, if entry was found, PTRM will return index of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT op, PTRM will return index of new entry if INSERT succeeded. */
    uint32_t rsp_entry_index;

/*! \brief SID of PT where entry was found (all ops)
  - For all ops, if entry was found, PTRM will return SID of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT, PTRM will return SID of new entry if INSERT succeeded. */
    bcmdrd_sid_t rsp_entry_sid;


/*! \brief Value of DATA_TYPEf of found entry (all ops)
  - Is needed by PTM to 'unpack' entry from hw_format to ltm format
  - If entry was found and has DATA_TYPEf, PTRM will return value of DATA_TYPEf.
  - If entry was found and uses KEY_TYPEf as DATA_TYPE, then PTRM must return
  value of KEY_TYPEf LSB as rsp_entry_data_type.
  - If entry was not found, PTRM will not set this var */
    uint16_t rsp_entry_data_type;

/*! \brief Logical ID for dfield_format of found entry (all ops).
  - If entry was found, PTRM will return logical dfield_format.
  - If entry was not found, PTRM will set this var to 0 */
    uint16_t rsp_dfield_format_id;
} bcmptm_rm_hash_rsp_t;

/*! \brief Decribe the info of physical table.
  - This structure is provied for UFT sub-module. */
typedef struct bcmptm_rm_hash_uft_pt_info_s {
    /*! \brief Physical table sid. */
    bcmdrd_sid_t ptsid;

    /*! \brief vector_type. */
    uint8_t vector_type;

    /*! \brief Number of valid logical banks. */
    uint8_t num_lbanks;

    /*! \brief Bank offset. */
    uint8_t bank_offset[BCMPTM_RM_HASH_MAX_BANK_COUNT];
} bcmptm_rm_hash_pt_chg_t;

/*!
 * \brief Robust hash config structure.
 */
typedef struct bcmptm_rm_hash_rhash_cfg_s {
    /*! If Robust hash is enabled */
    bool enable;

    /*! Widest hash table view for which the robust hash maps */
    bcmdrd_sid_t wsid;

    /*! If word boundary alignment is required */
    bool alignment;

    /*! Remap table memories */
    bcmdrd_sid_t remap_tab[BCMPTM_RM_HASH_RHASH_MODULES_NUM];

    /*! Action table memories */
    bcmdrd_sid_t action_tab[BCMPTM_RM_HASH_RHASH_MODULES_NUM];

    /*! Remap data field id. */
    bcmdrd_fid_t remp_data_f;

    /*! Action field id. */
    bcmdrd_fid_t action_f;

    /*! Remap Table bank 0 SW cache */
    uint32_t remap_data_a[BCMPTM_RM_HASH_RHASH_REMAP_TABLE_SIZE];

    /*! Remap Table bank 1 SW cache */
    uint32_t remap_data_b[BCMPTM_RM_HASH_RHASH_REMAP_TABLE_SIZE];

    /*! Action Table bank 0 SW cache */
    uint64_t action_data_a[BCMPTM_RM_HASH_RHASH_ACTION_TABLE_DEPTH];

    /*! Action Table bank 1 SW cache */
    uint64_t action_data_b[BCMPTM_RM_HASH_RHASH_ACTION_TABLE_DEPTH];
} bcmptm_rm_hash_rhash_cfg_t;

/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
  \brief Perform Lookup, Insert, Delete for Hash Table based LT
  \param [in] unit Logical device id
  \param [in] req_flags TBD. Ignore for now.
           \n (??? eg: final read from HW instead of cache?)
  \param [in] cseq_id Transaction id. Must be passed with every cmdproc_write, read
  \param [in] req_ltid Logical Table enum for current req_op
           \n Must be provided with Writes to CmdProc
           \n Will be provided by CmdProc during reads.
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  - For Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
  - tbl_inst member of pt_dyn_info may indicate pipe num.
  - Must pass tbl_inst received as part of pt_dyn_info for all cmdproc write,
  read requests.
  - Need pipe num to support unique mode for EXACT_MATCH tables.
  - Value of -1 means Global mode.
  - pipe_num for all underlying SIDs will be same for a given LT
  \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for hash tables
  \param [in] req_info See defn for bcmptm_rm_hash_req_t
  \param [in] rsp_entry_buf_wsize Num of words in rsp_entry_words array

  \param [out] rsp_entry_words Contents of found entry (all ops)
        \n PTRM will not touch this array if entry was not found (all ops)
        \n PTM will separate out key, data portions for LTM (to support
        returning of data for pdd type LTs)
        \n Is dont_care for INSERT
  \param [out] rsp_info See defn for bcmptm_rm_hash_rsp_t
  \param [out] rsp_ltid LTID of found entry
            \n If entry was found, for all ops, PTRM will return ltid of found
               entry from PTcache (from cmdproc read).
            \n If entry was not found, PTRM will return ALL_ZEROs for LOOKUP,
               DELETE ops.
            \n If entry was not found, PTRM will return req_ltid for INSERT op.
  \param [out] rsp_flags Return ZEROs for LOOKUP, DELETE ops
                      \n For Insert operation, PTRM must provide additional info
                      as below:
                       - OLD_ENTRY_DELETED
                       - REPLACED
                       - ??? Anything else?

  \retval SHR_E_NONE Success
  \retval SHR_E_NOT_FOUND Entry not found for Lookup, Delete ops
  \retval SHR_E_FULL Could not perform requested Insert due to Hash Collisions
  \retval SHR_E_MEMORY Could not perform requested Insert due to Table Full
  \retval SHR_E_UNAVAIL Requested op is not supported
 */
extern int
bcmptm_rm_hash_req(int unit,
                   uint32_t req_flags,
                   uint32_t cseq_id,
                   bcmltd_sid_t req_ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_hash_req_t *req_info,
                   size_t rsp_entry_buf_wsize,

                   uint32_t *rsp_entry_words,
                   bcmptm_rm_hash_rsp_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags);

/*!
 * \brief Initialize PTRM hash (alloc bucket state)
 * \n Must be called every time a new unit is attached and hash table sizes
 * have been determined, configured.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_init(int unit);

/*!
 * \brief PTRM hash cleanup (de-alloc bucket state, etc)
 * \n Will release SW data-structures associated with this
 * unit.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_cleanup(int unit);

/*!
 * \brief Get valid hardware entry info for the given lt info.
 *
 * \param [in] unit Logical device id
 * \param [in] lt_info Logical table info for a ltid
 * \param [out] count The count of valid hardware entry info for the ltid
 * \param [out] idx_list The list of all the valid hardware entry info index
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_valid_hw_entry_info_get(int unit,
                                       const bcmptm_rm_hash_lt_info_t *lt_info,

                                       uint8_t *count,
                                       uint8_t *idx_list);

/*!
 * \brief Get physical table sid for the given bank index
 *
 * \param [in] unit Logical device id
 * \param [in] rm_more_info Additional info needed by rsrc_mgr
 * \param [in] entry_info Hardware entry info
 * \param [in] bank_idx Bank index
 * \param [out] sid Physical table sid if success.
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_sid_get(int unit,
                       const bcmptm_rm_hash_more_info_t *rm_more_info,
                       const bcmptm_rm_hash_hw_entry_info_t *entry_info,
                       uint8_t bank_idx,

                       bcmdrd_sid_t *sid);

/*!
 * \brief Component config phase of init sequence
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] phase 1,2,3
 * \n 1 => 1st scan of all LTIDs. Should see calls to bcmptm_rm_hash_lt_reg
 * \n 2 => 2nd scan of all LTIDs. Should see calls to bcmptm_rm_hash_lt_reg
 * \n 3 => No scan of LTIDs. No calls to bcmptm_rm_hash_lt_reg
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase);

/*!
 * \brief Registration of LTID with rm_hash
 *
 * \param [in] unit Logical device id
 * \param [in] ltid LTID that needs to be managed by rsrc_mgr
 * \param [in] phase 1,2,3
 * \param [in] lt_state_offset Offset into LT state mem (in rsrc_mgr)
 * \n Note: lt_state_offset will be stored by caller only in phase 2
 * (comp_config)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_lt_reg(int unit, bcmltd_sid_t ltid,
                      bcmptm_sub_phase_t phase, uint32_t *lt_state_offset);

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
bcmptm_rm_hash_trans_cmd(int unit, uint32_t flags, uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd);

/*!
 * \brief Update hash move_depth for a hash typed LT.
 * \n Interface provided for configuration of TABLE_EM_CONTROL LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_id Logical talbe id.
 * \param [in] move_depth New move_depth to be set.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_lt_move_depth_update(int unit, bcmdrd_sid_t lt_id,
                                    int move_depth);

/*!
 * \brief Notity RM HASH the change of hash bank attribute.
 * \n Interface provided for configuration of DEVICE_EM_BANK LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] ptsid Physical table id.
 * \param [in] lbank Corresponding logical bankd id.
 * \param [in] bank_attr Attribute pointer of bank.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_bank_attr_assign(int unit, bcmdrd_sid_t ptsid, int lbank,
                                bcmptm_rm_hash_vector_bank_attr_t* bank_attr);

/*!
 * \brief Notity RM HASH the size change, bank map change of phyical tables.
 * \n Interface provided for configuration of DEVICE_EM_GROUP LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] pt_chg Changed info of physical tables.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_pt_info_update(int unit,
                              bcmptm_rm_hash_pt_chg_t *pt_chg);

#endif /* BCMPTM_RM_HASH_INTERNAL_H */

