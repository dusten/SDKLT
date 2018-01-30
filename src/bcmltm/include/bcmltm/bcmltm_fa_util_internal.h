/*! \file bcmltm_fa_util_internal.h
 *
 * Logical Table Manager Binary Trees.
 * Field Adaptation Nodes.
 *
 * A set of functions and definitions forming the nodes of
 * LTM FA binary trees.
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

#ifndef BCMLTM_FA_UTIL_INTERNAL_H
#define BCMLTM_FA_UTIL_INTERNAL_H

#include <bcmltm/bcmltm_internal.h>

/*!
 * \brief Validate the API-supplied field value for a specific field ID
 *
 * Check that the set bits of the API-supplied field value fit within
 * the valid bit size of the target Working Buffer coordinate.
 *
 * \param [in] wbc Working Buffer Coordinate
 * \param [in] field_val A uint32_t array holding the field value.
 *
 * \retval bool TRUE if field value fits within the WBC location.
 *              FALSE if the field value exceeds the WBC size.
 */
extern bool
bcmltm_field_fit_check(bcmltm_wb_coordinate_t *wbc, uint32_t *field_val);

/*!
 * \brief Retrieve the API-supplied field value for a specific field ID
 *
 * Search the API sourced field value list for the field value matching
 * the supplied field ID.  Return a pointer to the field list structure
 * for that field value if found.  Otherwise return NULL.
 *
 * \param [in] in_fields List of API-provided field value structures.
 * \param [in] api_field_id The field ID to seek within the in_fields
 *             field value list.
 * \param [in] field_idx The field array index for this field ID to
 *             match in the field value list.
 *
 * \retval non-NULL Pointer to API field list structure matching field ID
 * \retval NULL Matching field value not found for provided field ID.
 */
extern bcmltm_field_list_t *
bcmltm_api_find_field(bcmltm_field_list_t *in_fields,
                      uint32_t api_field_id,
                      uint32_t field_idx);

/*!
 * \brief Copy a uint32_t array field value into the Working Buffer.
 *
 * Copy the specified uint32_t field value array into the Working Buffer
 * at the location and field size specified.
 *
 * \param [in] wbc Working Buffer coordinate - offset, minbit, maxbit
 * \param [in] field_val uint32_t array containing the field value.
 * \param [out] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_into_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer);

/*!
 * \brief Retrieve a uint32_t array field value from the Working Buffer.
 *
 * Copy the specified field value from the Working Buffer into
 * the uint32_t field value array provided.
 *
 * \param [in] wbc Working Buffer coordinate - offset, minbit, maxbit
 * \param [out] field_val uint32_t array containing the field value.
 * \param [in] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_from_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer);

/*!
 * \brief Copy a uint32_t array field value per the PTM field map.
 *
 * Copy the specified uint32_t field value array into the Working Buffer
 * at the location and field size specified.
 *
 * \param [in] field_map PTM field structure pointer including WBC.
 * \param [in] field_val uint32_t array containing the field value.
 * \param [out] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_into_buffer(bcmltm_ptm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer);

/*!
 * \brief Retrieve a uint32_t array field value per the PTM field map.
 *
 * Copy the specified field value from the Working Buffer into
 * the uint32_t field value array provided.
 *
 * \param [in] field_map PTM field structure pointer including WBC.
 * \param [out] field_val uint32_t array containing the field value.
 * \param [in] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_from_buffer(bcmltm_ptm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer);




/*!
 * \brief Convert uint64 values into internal uint32 array.
 *
 *
 * \param [in] field_val64 uint64_t data value.
 * \param [out] field_val uint32_t array large enough to hold the data value.
 *
 * \retval None.
 */
extern void
bcmltm_uint64_to_uint32_array(uint64_t field_val64,
                              uint32_t *field_val);

/*!
 * \brief Convert an internal uint32_t array into a uint64_t value.
 *
 * \param [in] field_val uint32_t array large enough to hold the data value.
 * \param [out] field_val64 uint64_t data value.
 *
 * \retval None.
 */
extern void
bcmltm_uint32_array_to_uint64(uint32_t *field_val,
                              uint64_t *field_val64);

/*!
 * \brief Search a Logical Table Adapter I/O structure.
 *
 * Traverse a LTA I/O structure to locate the field
 * matching the given API field ID and index.  Return the field
 * value structure pointer if found, NULL if not.
 *
 * \param [in] lta_fields LTA fields input/output structure pointer
 * \param [in] field_id Field identifier value.
 * \param [in] field_idx The field array index for this field ID to
 *             match in the field value list.
 *
 * \retval Not-NULL Matching field value structure.
 * \retval NULL No matching field value found.
 */
extern bcmltm_field_list_t *
bcmltm_lta_find_field(const bcmltd_fields_t *lta_fields,
                      uint32_t field_id,
                      uint32_t field_idx);

#endif /* BCMLTM_FA_UTIL_INTERNAL_H */
