/*! \file bcmltm_nc_lta_validate.h
 *
 * Logical Table Manager - Node Cookies.
 *
 * Logical Table mappings with Field Validation.
 *
 * This file contains routines to construct LT information
 * required by the LTM metadata for LT with LTA Field Validation.
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

#ifndef BCMLTM_NC_LTA_VALIDATE_H
#define BCMLTM_NC_LTA_VALIDATE_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include "bcmltm_nc_lta_xfrm.h"


/*!
 * \brief Logical Table cookies information for Field Validation.
 *
 * This structure contains information derived from the LRD
 * for Field map entry.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_nc_lta_validate_info_s {
    /*! Working buffer block ID for this LTA Field Validation. */
    bcmltm_wb_block_id_t wb_block_id;

    /*!
     * LTA Field Validation parameters used as the LTA FA node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_validate_params_t *validate_params;

    /*!
     * LTA src field list used as the LTA FA node cookie.
     * (LTM to/from LTA).
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_field_list_t *fsrc;

    /*!
     * Number of total indices of fields which requires field validation.
     */
    uint32_t total_indices;

} bcmltm_nc_lta_validate_info_t;

/*!
 * \brief Logical Table cookies information for Field Validation.
 *
 * This structure contains information derived from the LRD
 * for Field map entry.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_nc_lta_validate_info_list_s {
    /*!
     * Number of Field validation node cookies.
     */
    uint32_t num;

    /*!
     * Array of field validate node cookie information.
     */
    bcmltm_nc_lta_validate_info_t *validate_info;

} bcmltm_nc_lta_validate_info_list_t;

/*!
 * \brief Create LT cookies info for Field Validation for given table.
 *
 * Create the LT cookies information needed by the LTM metadata
 * for LTA Field Validation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] entries LT map information.
 * \param [in] field_type Indicates key or value field.
 * \param [out] info_ptr Returning pointer to Validate Field info list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_validate_info_create(int unit, bcmlrd_sid_t sid,
                              bcmltm_map_entries_fv_t *entries,
                              bcmltm_field_type_t field_type,
                              bcmltm_nc_lta_validate_info_list_t **info_ptr);


/*!
 * \brief Create LTA-LTA copy LT cookies info for Field Validate calls
 *        for given table.
 *
 * Create the LTA-LTA copy LT cookies information needed by the
 * LTM metadata for LTA Field Validate functions.
 * These are the reverse transform output to validate input
 * LTA field copy nodes used by the UPDATE opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] from_list Node cookie list of reverse transforms from
 *                       which to copy field values in output list.
 * \param [in] to_list Node cookie list of validate functions to
 *                     which to copy field values into input list.
 * \param [out] list_ptr Returning pointer to LT info for validate copy.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_nc_lta_validate_copy_info_create(int unit, bcmlrd_sid_t sid,
                            bcmltm_nc_lta_xfrm_info_list_t *from_list,
                            bcmltm_nc_lta_validate_info_list_t *to_list,
                            bcmltm_nc_lta_validate_info_list_t **list_ptr);

/*!
 * \brief Destroy LT cookies info for Field Validation.
 *
 * Destroy LT cookies info for LTA Field Validation.
 *
 * \param [in] info Pointer to LT Field info to destroy.
 */
extern void
bcmltm_nc_lta_validate_info_destroy(bcmltm_nc_lta_validate_info_list_t *list);

#endif /* BCMLTM_NC_LTA_VALIDATE_H */
