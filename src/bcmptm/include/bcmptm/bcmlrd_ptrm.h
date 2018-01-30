/*! \file bcmlrd_ptrm.h
 *
 * <description>
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

#ifndef BCMLRD_PTRM_H
#define BCMLRD_PTRM_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
  Defines
 */


/*******************************************************************************
  Typedefs
 */
/*! \brief pt type for an LT.
  - Could be different depending on configuration. Eg: LPM type LTs can be ALPM
  or TCAM */
typedef enum bcmlrd_ptrm_type_s {
    LT_PT_TYPE_HASH = 1, /*!< ??? Includes Exact_Match tables */
    LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID, /*!< Tcam tables with entry_pri, entry_id */
    LT_PT_TYPE_TCAM_PRI_ONLY, /*!< Tcam tables with entry_pri only */
    LT_PT_TYPE_LPM_V4, /*!< Tcam tables which are prefix-managed */
    LT_PT_TYPE_LPM_V6, /*!< Tcam tables which are prefix-managed */
    LT_PT_TYPE_DIRECT, /*!< Direct tables */
    LT_PT_TYPE_FP, /*!< FP tables */
    LT_PT_TYPE_CTR_CFG_ONLY, /*!< LT which maps to config portion of SID which
                                  supports both counter and cfg fields */
    LT_PT_TYPE_FP_EM,  /*! FP exact match table. */
    LT_PT_TYPE_HASH_MPLS_DECAP,  /*! Tunnel mpls decap table. */
} bcmlrd_ptrm_type_t;

/*! \brief Array of uint32_t to hold any BITMAPs */
typedef struct bcmlrd_field_bmp_s {
    /*! \brief Array of uint32_t to hold bitmaps.
      - Size of this array varies based on unique bits required to save.
      - General formula to caliculate the size of this array is
        (numer of unique bits to save) % 32.
      */
    SHR_BITDCL *w;
} bcmlrd_field_bmp_t;

/*! \brief Macros to manipulate bits in bitspams of type
 *   bcmlrd_field_bmp_t.
 */

/*! \brief Macro to set the bit in BMP of type bcmlrd_field_bmp_t. */
#define BCMLRD_FIELD_BMP_ADD(bmp, cl)     SHR_BITSET(((bmp).w), (cl))

/*! \brief Macro to reset the bit in BMP of type bcmlrd_field_bmp_t. */
#define BCMLRD_FIELD_BMP_REMOVE(bmp, cl)  SHR_BITCLR(((bmp).w), (cl))

/*! \brief Macro to test the bit in BMP of type bcmlrd_field_bmp_t. */
#define BCMLRD_FIELD_BMP_TEST(bmp, cl)    SHR_BITGET(((bmp).w), (cl))

/*! \brief Macro to perform binary search on a sorted table.
 *
 * \param [in] tbl Is the table to search in.
 * \param [in] field Is the field of the table data structure that should
 * be compared in the search.
 * \param [in] item Is the item to search for.
 * \param [in] size Is the max index to use in the search. Typically this
 * will be the number of rows in the table - 1 (index starts for 0).
 * \param [out] idx Is the index in the table where the item was found. The
 * caller should define this parameter as type int and should initialize it to
 * -1 before the search. If the idx is still -1 after the search it indicates
 * that the element was not found.
 */
#define BIN_SEARCH(tbl, field, item, size, idx) \
{                                               \
    int start = 0;                              \
    int end = size;                             \
    int middle;                                 \
    int res;                                    \
                                                \
    while (end >= start) {                      \
        middle = start + (end - start) / 2;     \
        res = (int)tbl[middle]field - (int)item; \
        if (res == 0) {                         \
            idx = middle;                       \
            break;                              \
        } else if (res < 0) {                   \
            start = middle + 1;                 \
        } else {                                \
            end = middle - 1;                   \
        }                                       \
    }                                           \
}


/*! \brief Information about repeated fields in hw_entry.
  - Example: KEY_TYPEf */
typedef struct bcmlrd_hw_rfield_info_s {
/*! \brief Number of times the field occurs */
    uint8_t num_reps;

/*! \brief Width of field in bits */
    uint16_t field_width;

/*! \brief Start position of the field (array)
   - Num of elements = num_reps */
    const uint16_t *field_start_bit;
} bcmlrd_hw_rfield_info_t;

/*! \brief Information about repeated fields in hw_entry.
  - Example: KEY_TYPEf */
typedef struct bcmlrd_hw_field_list_s {
/*! \brief Number of fields in the list */
    uint8_t num_fields;

/*! \brief Width of each field in bits (array)
   - Num of elements = num_fields */
    const uint16_t *field_width;

/*! \brief Start position of the field (array)
   - Num of elements = num_fields */
    const uint16_t *field_start_bit;
} bcmlrd_hw_field_list_t;

/*! \brief Info needed for every dfield format. */
typedef struct lt_hash_dfield_format_s {

/*! \brief Index into hw_entry_info array
  - Every data_format maps to one of the 1x, 2x, 4x etc  hw_entries */
    uint8_t index_hw_entry_info;
} lt_hash_dfield_format_t;

/*! \brief Info common for TCAM, HASH based LTs accessed on modelled path.
  - LT may have several possible dfield formats - but a unique combination of
  key fields. */
typedef struct lt_mreq_info_s {

/*! \brief num of entry_key_words rows that LTM is providing.
  - Array of arrays needed to support key_width expansion.

  - Hash type LTs
   -# Must be 1

  - Aggr_Tcam type LTs
   -# Must be 1 - we cannot support key_width expansion for these.

  - Non_Aggr_Tcam type LTs
   -# Can be greater than 1 to support key_width expansion
   -# Each row of ekw can map to different SID (each fp slice inst in
   device can be separate SID) */
    uint8_t num_ekw;

/*! \brief num of entry_data_words rows that LTM is providing.
  - Array of arrays needed to support data_width expansion.

  - Hash type LTs
   -# Must be 1

  - Aggr_Tcam type LTs
   -# Must be 1 - we cannot support data_width expansion for these.

  - Non_Aggr_Tcam type LTs
   -# Can be greater than 1 to support data_width expansion
   -# Each row of edw can map to diff SID (each fp_policy inst in device
   can have separate SID) */
    uint8_t num_edw;

/*! \brief Distinguish Non_Aggr, Aggr type LTs
  - 0 means Non_Aggr type LT
   -# Non_Aggr type LT refers to LT that is mapped to individual TCAM_ONLY,
   DATA_ONLY views - because HW does not offer combined TCAM+DATA view.
  - Non_zero implies Hash type LTs and Aggr_Tcam type LTs
   -# Non_zero number specifies number of words to OR in ekw, edw */
    uint16_t aggr_ekw_edw;

/*! \brief Identify the rsrc_mgr for this LT.
  - See defn of bcmlrd_ptrm_type_t */
    bcmlrd_ptrm_type_t pt_type;

/*! \brief Ptr to array which holds dfield_format info for this LT
  - Needed to allow PTM to 'pack' LTM specified fields into HW specific format
  and 'unpack' fields from HW specific format to LTM format.
  - When dfield_format_count is 1, dfield_format specified by element0 is the HW
 format of the fields. No remapping of dfields is needed.
  - When dfield_format_count is > 1, dfield_format specified by element0
  represents 'virtual normalized superset' format which is used to exchange data
  fields between LTM, PTM */
    const void *dfield_format;

/*! \brief num of data field formats for this LT.
  - Num of elements in dfield_format array */
    uint16_t dfield_format_count;

/*! \brief Ptr to array which holds hw_entry_info for this LT
  - hw_entry_info refers to number, position of KEY_TYPEf, DATA_TYPEf, and also
  value of KEY_TYPEf - but not value of DATA_TYPEf. Even though a LT may support
  large number of dfield_formats, each with different value for DATA_TYPEf,
  underlying hw_entry_format choices are limited to HW views (like 1x, 2x, etc)
  - For Single_Data_Format type LTs:
   -# There will be only single  hw_entry_info
  - For Dual_Data_Format type LTs where KEY_TYPE implicitly acts as DATA_TYPE:
   -# There will be only two hw_entry_info
  - For Multiple_Data_Format type LTs
   -# max num of hw_entry_info will corresponding to 1x, 2x, 4x, etc views
   offered in regsfile (for ALPM we can have 70b, 105b, 140b, 210b views) */
    const void *hw_entry_info;

/*! \brief Num of hw_entry_info for this LT.
  - Num of elements in hw_entry_info array */
    uint16_t hw_entry_info_count;

/*! \brief Length of array rm_more_info */
    uint32_t rm_more_info_count;

/*! \brief Additional rsrc_mgr specific info */
    const void *rm_more_info;
} lt_mreq_info_t;

/*******************************************************************************
  Function prototypes
 */
/*!
  \brief Initializes copy of PTRM table

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success
  \retval SHR_E_MEMORY otherwise.
 */
extern int
bcmlrd_lt_mreq_info_init(int unit);

/*!
  \brief Cleanup copy of PTRM table

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success
 */
extern int
bcmlrd_lt_mreq_info_cleanup(int unit);
/*!
  \brief Fetch lt_mreq_info for hash, tcam, alpm tables.

  \param [in] unit Logical device id
  \param [in] req_ltid Logical Table enum that is accessing the table

  \param [out] lt_mreq_info Information about LT for req on modelled path

  \retval SHR_E_NONE Success
  \retval SHR_E_UNAVAIL lt_info info is not avail for this LTID
 */
extern int
bcmlrd_lt_mreq_info_get(int unit,
                        bcmltd_sid_t req_ltid,
                        const lt_mreq_info_t **lt_mreq_info);

#endif /* BCMLRD_PTRM_H */
