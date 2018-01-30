/*! \file bcmltm_wb_lta_internal.h
 *
 * Logical Table Manager Working Buffer Definitions for LTA interfaces.
 *
 * These definitions are used to support Logical Tables that
 * requires LTA interfaces used for the following scenarious:
 *     Custom Table Handler
 *     Field Transforms
 *     Field Validations
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

#ifndef BCMLTM_WB_LTA_INTERNAL_H
#define BCMLTM_WB_LTA_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_wb_types_internal.h>

/*! LTA Input field list. */
#define BCMLTM_WB_LTA_INPUT_FIELDS    0

/*! LTA Output field list. */
#define BCMLTM_WB_LTA_OUTPUT_FIELDS   1


/*!
 * \brief Add a LTA Custom Table Handler working buffer block.
 *
 * This function adds a working buffer block required for a
 * Custom Table Handler for the given number of logical fields
 * elements (fid, idx).
 *
 * \param [in] unit Unit number.
 * \param [in] num_fields Number of LT field elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_lta_cth_add(int unit,
                            uint32_t num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr);


/*!
 * \brief Add a LTA Field Transform working buffer block.
 *
 * This function adds a working buffer block required for a
 * LTA field transform for the given number of input and output
 * fields elements (fid, idx).
 *
 * \param [in] unit Unit number.
 * \param [in] in_num_fields Number of input fields elements (fid, idx).
 * \param [in] out_num_fields Number of output fields elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_lta_xfrm_add(int unit,
                             uint32_t in_num_fields,
                             uint32_t out_num_fields,
                             bcmltm_wb_handle_t *handle,
                             bcmltm_wb_block_id_t *block_id,
                             bcmltm_wb_block_t **block_ptr);


/*!
 * \brief Add a LTA Field Validation working buffer block.
 *
 * This function adds a working buffer block required for a
 * LTA field validation for the given number of input
 * fields elements (fid, idx).
 *
 * \param [in] unit Unit number.
 * \param [in] in_num_fields Number of input fields elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_lta_val_add(int unit,
                            uint32_t in_num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr);


/*!
 * \brief Get LTA fields working buffer offsets.
 *
 * This function gets the LTA fields offsets for the given field
 * type in the specified LTA working buffer block.
 *
 * The field_type is either:
 *   BCMLTM_WB_LTA_INPUT_FIELDS
 *   BCMLTM_WB_LTA_OUTPUT_FIELDS
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [out] offset Returning LTA fields offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lta_fields_offset_get(const bcmltm_wb_block_t *wb_block,
                                int field_type,
                                bcmltm_wb_lta_fields_offset_t *offset);


/*!
 * \brief Get working buffer offset for a LTA field.
 *
 * This function gets the working buffer offset for a field structure
 * in the specified LTA working buffer block.
 *
 * The field index indicates the index into the array of field
 * structures allocated in the working buffer space.
 * Index range should be [0 .. num_fields-1].
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [in] field_idx Index of field array to return offset for.
 * \param [out] offset Returning field offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lta_field_map_offset_get(const bcmltm_wb_block_t *wb_block,
                                   int field_type,
                                   uint32_t field_idx,
                                   uint32_t *offset);

#endif /* BCMLTM_WB_LTA_INTERNAL_H */
