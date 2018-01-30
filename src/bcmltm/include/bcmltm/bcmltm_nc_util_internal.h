/*! \file bcmltm_nc_util_internal.h
 *
 * Logical Table Manager - Node Cookie and Map Utility.
 *
 * This file contains utility routines used for constructing
 * the LTM metadata node cookies.
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

#ifndef BCMLTM_NC_UTIL_INTERNAL_H
#define BCMLTM_NC_UTIL_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_map_types_internal.h>

/*!
 * \brief Field ID and Index count.
 */
typedef struct bcmltm_fid_idx_s {
    /*! Field ID. */
    uint32_t fid;

    /*! Field Index Count. */
    uint32_t idx_count;
} bcmltm_fid_idx_t;


/*!
 * \brief Get number of fields for a logical table.
 *
 * This routine returns the number of field IDs and
 * field elements (id,idx) for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] mapped_only Count only mapped fields.
 * \param [out] num_fid Number of field IDs (fid).
 * \param [out] num_fid_idx Number of field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_lt_field_count_get(int unit, bcmlrd_sid_t sid,
                          bool mapped_only,
                          uint32_t *num_fid, uint32_t *num_fid_idx);

/*!
 * \brief Get number of Key fields for a logical table.
 *
 * This routine returns the number Key field IDs and
 * field elements (id,idx) for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] mapped_only Count only mapped fields.
 * \param [out] num_fid Number of Key field IDs (fid).
 * \param [out] num_fid_idx Number of Key field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_lt_field_key_count_get(int unit, bcmlrd_sid_t sid,
                              bool mapped_only,
                              uint32_t *num_fid, uint32_t *num_fid_idx);


/*!
 * \brief Get list of field IDs and index count for a logical table.
 *
 * This routine returns the list of field IDs and field index count
 * for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] mapped_only Return only mapped fields.
 * \param [in] list_max Size of allocated list.
 * \param [out] list List of field IDs and index count.
 * \param [out] num Number of fields returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_lt_field_list_get(int unit, bcmlrd_sid_t sid,
                         bool mapped_only,
                         uint32_t list_max,
                         bcmltm_fid_idx_t *list, uint32_t *num);

/*!
 * \brief Get list of Key field IDs and index count for a logical table.
 *
 * This routine returns the list of Key field IDs and field index count
 * for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] mapped_only Return only mapped fields.
 * \param [in] list_max Size of allocated list.
 * \param [out] list List of Key field IDs and index count.
 * \param [out] num Number of Key fields returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_lt_field_key_list_get(int unit, bcmlrd_sid_t sid,
                             bool mapped_only,
                             uint32_t list_max,
                             bcmltm_fid_idx_t *list, uint32_t *num);

/*!
 * \brief Convert a field into field index min/max bit positions.
 *
 * Convert a field into required number of field elements
 * necessary to hold the entire field width.   The resulting data
 * is a set of arrays for the min and max bit positions
 * for each of the field indexes.
 *
 * This function is normally called on physical fields where the caller
 * needs to know the min and max bit positions for each field element
 * on a wide field.
 *
 * Field:  field[0]:  [min_0 .. max_0]
 *         field[1]:  [min_1 .. max_1]
 *         field[n]:  [min_n .. max_n]
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] minbit Minimim bit position of field.
 * \param [in] maxbit Maximum bit position of field.
 * \param [in] list_max Array size.
 * \param [out] fidx_minbit Retuning minimum bit positions for field indexes.
 * \param [out] fidx_maxbit Retuning maximum bit positions for field indexes.
 * \param [out] num_fidx Returning actual number of field indexes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_field_to_fidx_min_max(int unit, uint32_t fid,
                                uint16_t minbit, uint16_t maxbit,
                                uint32_t list_max,
                                uint16_t *fidx_minbit,
                                uint16_t *fidx_maxbit,
                                uint32_t *num_fidx);

/*!
 * \brief Indicate if logical field corresponds to an LT index.
 *
 * This routine returns TRUE if the given logical field is a key
 * and maps to a PT index.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] fid Logical Field ID.
 *
 * \retval TRUE, if given LT field maps to a key PT field.
 *         FALSE, otherwise.
 */
extern bool
bcmltm_nc_field_is_lt_index(int unit, bcmlrd_sid_t sid, uint32_t fid);


/*!
 * \brief Get the PT index offsets.
 *
 * This function gets the PTM working buffer offsets for all the PT
 * indexes.
 *
 * Note that some fields may be mapped to both, a PT index
 * and also a PT port or table instance memory parameter.  In this case,
 * the field is treated as a 'index' (from the index tracking perspective)
 * and would be included in this output.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_list PT list.
 * \param [in] offset_max Array size of offset.
 * \param [out] offset Returning offsets.
 * \param [out] num_offset Actual number of offsets returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_pt_index_offsets_get(int unit, bcmlrd_sid_t sid,
                               const bcmltm_pt_list_t *pt_list,
                               uint32_t offset_max,
                               uint32_t *offset, uint32_t *num_offset);

/*!
 * \brief Get the PT memory parameter offsets.
 *
 * This function gets the PTM working buffer offsets for all the PT
 * memory parameters.
 *
 * Note that some fields may be mapped to both, a PT index
 * and also a PT port or table instance memory parameter.  In this case,
 * the field is treated as a 'index' (from the index tracking perspective)
 * and is not included in this output.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_list PT list.
 * \param [in] offset_max Array size of offset.
 * \param [out] offset Returning offsets.
 * \param [out] num_offset Actual number of offsets returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_pt_mem_param_offsets_get(int unit, bcmlrd_sid_t sid,
                                   const bcmltm_pt_list_t *pt_list,
                                   uint32_t offset_max,
                                   uint32_t *offset, uint32_t *num_offset);

/*!
 * \brief Find the PTM working buffer block for given Physical Table ID.
 *
 * This function finds and returns the PTM working buffer block
 * for the given Physical Table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] lt_type LT table type.
 * \param [in] pt_map PT map groups.
 * \param [in] ptid Physical table ID to look for.
 *
 * \retval Pointer to PTM working buffer block.
 *         NULL if not found.
 */
extern const bcmltm_wb_block_t *
bcmltm_ptm_wb_block_find(int unit, bcmlrd_sid_t sid,
                         bcmltm_table_type_t lt_type,
                         const bcmltm_map_pt_t *pt_map, bcmdrd_sid_t ptid);

/*!
 * \brief Destroy the given LTA fields list metadata.
 *
 * Destroy the given LTA field list metadata.
 *
 * \param [in] flist LTA field list to destroy.
 */
extern void
bcmltm_nc_field_list_destroy(bcmltm_lta_field_list_t *flist);

/*!
 * \brief Create the LTA copy field list metadata for given table.
 *
 * Create the LTA Copy field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] from_list Origin LTA field list from which to copy.
 * \param [in] to_list Target LTA field list to which to copy.
 * \param [out] copy_list_ptr Returning pointer for LTA field list
 *              copy metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_copy_lta_field_list_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lta_field_list_t *from_list,
                                     bcmltm_lta_field_list_t *to_list,
                                     bcmltm_lta_field_list_t **copy_list_ptr);

/*!
 * \brief Get the field map type for given LRD direct field map entry.
 *
 * This routine gets the field map type, Key or Value,
 * based on the LRD entry information.
 *
 * This is only applicable for a direct PT field map entry.
 *
 * A logical field can be mapped to more than one PT field.  It is
 * the mapped PT field that ultimately determines whether a field map
 * type is a Key or Value.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_type Table type.
 * \param [in] fid LT field ID.
 * \param [in] map LRD map.
 * \param [in] entry LRD map entry.
 * \param [out] field_type Returning field map type BCMLTM_FIELD_TYPE_xxx.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_map_entry_mapped_field_type_get(int unit,
                                          bcmltm_table_type_t lt_type,
                                          uint32_t fid,
                                          const bcmlrd_map_t *map,
                                          const bcmlrd_map_entry_t *entry,
                                          bcmltm_field_type_t *field_type);

/*!
 * \brief Check if given PT is mapped to Key or Value fields.
 *
 * This routine checks if the given PT ID is mapped to Key fiels.
 * This is only applicable for Keyed LTs.
 *
 * Although the function can be used for any Keyed LT, it is primarily
 * used for TCAMs with Non-Aggregate views (where the LT maps to 2 PTs) to
 * determine which PT contains maps to Key fields.  This type of table
 * should contain all Key fields mapped to one PT and all Value fields
 * mapped to the second PT.  It is assumed that this rule is enforced
 * during the LT definition.
 *
 * Note that LT Hash and TCAM with Aggregate views only have 1 PT mapped.
 * The single PT contains maps to both Key and Value fields.  In this case,
 * 'is_key' will always return TRUE.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] ptid PT ID to check.
 * \param [out] is_key Return TRUE if PT maps to Key fields,
 *                     FALSE otherwise.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_map_pt_key_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_table_attr_t *attr,
                         bcmdrd_sid_t ptid,
                         bool *is_key);

/*!
 * \brief Destroy the list of field maps metadata.
 *
 * Destroy the list of field maps metadata.
 *
 * \param [in] num_fields Number of fields in field map list.
 * \param [in] field_map_list Field maps metadata to destroy.
 */
extern void
bcmltm_nc_ptm_field_maps_destroy(uint32_t num_fields,
                                 bcmltm_ptm_field_map_t *field_map_list);

/*!
 * \brief Destroy the field list metadata.
 *
 * Destroy the field list metadata.
 *
 * \param [in] field_list Field list metadata to destroy.
 */
extern void
bcmltm_nc_ptm_field_list_destroy(bcmltm_ptm_field_list_t *field_list);

/*!
 * \brief Destroy the field mapping metadata.
 *
 * Destroy the field mapping metadata.
 *
 * \param [in] ptm_mapping Field mapping metadata to destroy.
 */
extern void
bcmltm_nc_ptm_mapping_destroy(bcmltm_to_ptm_mapping_t *ptm_mapping);

/*!
 * \brief Destroy the given FA LT Index metadata.
 *
 * Destroy the given FA LT Index metadata.
 *
 * \param [in] index LT Index.
 */
extern void
bcmltm_nc_fa_lt_index_destroy(bcmltm_fa_lt_index_t *index);

/*!
 * \brief Get the total number of LRD table symbol IDs.
 *
 * Get the total number of LRD table IDs.  This count represents
 * the total number of logical tables defined for all devices.
 * As such, some sids may not be valid or applicable on a given device.
 *
 * For instance:
 *   Device-A: valid SIDs are 0, 1, 2, 3
 *   Device-B: valid SIDs are 0, 3
 *   Device-C: valid SIDs are 4, 5
 *
 * For above example, the count should return 6.
 *
 * \retval Total number of LRD table symbol IDs.
 */
extern uint32_t
bcmltm_logical_table_count_get(void);

#endif /* BCMLTM_NC_UTIL_INTERNAL_H */
