/*! \file bcmltm_md_pthru_internal.h
 *
 * Logical Table Manager Physical Table Pass Through (Thru) Metadata.
 *
 * These definitions are internal to the LTM component and is
 * intended, therefore, to be used only by LTM sub-components.
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

#ifndef BCMLTM_MD_PTHRU_INTERNAL_H
#define BCMLTM_MD_PTHRU_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>


/*!
 * \brief Initialize the LTM metadata for PT Pass Thru on a given unit.
 *
 * This function requires caller to synchronize access to this routine
 * by possible multiple threads.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_create(int unit);


/*!
 * \brief Destroy the LTM metadata for PT Pass Thru on a given unit.
 *
 * Cleanup all data associated with the LTM PT Pass Thru for
 * the specified unit.
 *
 * This function requires caller to synchronize access to this routine
 * by possible multiple threads.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_destroy(int unit);


/*!
 * \brief Retrieve the metadata for a given logical table.
 *
 * Get the pointer to the table metadata for a given table id
 * on the specified unit.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Table ID.
 * \param [out] ltm_md_ptr Returns pointer to the table metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_lt_retrieve(int unit,
                            uint32_t ltid,
                            bcmltm_lt_md_t **ltm_md_ptr);

/*!
 * \brief Get the maximum Working Buffer size for PT PassThru.
 *
 * Get the maximum Working Buffer size requrired to support
 * PT PassThru tables.
 *
 * \param [in] unit Unit number.
 * \param [out] wb_max Returns Working Buffer maximum.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_wb_max_get(int unit,
                           uint32_t *wb_max);

/*!
 * \brief Retrieve the key field list for a physical table.
 *
 * Get the pointer to the key field list for a given physical table ID
 * on the specified unit.
 *
 * Assumes unit is valid and !null param pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [out] list Pointer to key field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_key_field_list_retrieve(int unit, bcmdrd_sid_t sid,
                                        const bcmltm_ptm_field_list_t **list);


/*!
 * \brief Retrieve the value field list for a physical table.
 *
 * Get the pointer to the value field list for a given physical table ID
 * on the specified unit.
 *
 * Assumes unit is valid and !null param pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [out] list Pointer to value field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_value_field_list_retrieve(int unit, bcmdrd_sid_t sid,
                                          const bcmltm_ptm_field_list_t **list);


/*!
 * \brief Get field information for register/memory field.
 *
 * Obtain field information (name, start bit, end bit, etc.) for
 * specified symbol ID and field ID.
 *
 * Assumes unit is valid and !null param pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 * \param [out] finfo Field information structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_md_pthru_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                               bcmdrd_sym_field_info_t *finfo);

#endif /* BCMLTM_MD_PTHRU_INTERNAL_H */
