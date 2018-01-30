/*! \file bcmltm_md.h
 *
 * Logical Table Manager Metadata.
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

#ifndef BCMLTM_MD_H
#define BCMLTM_MD_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_pt.h>

/*!
 * \brief Get a count of all field IDs for given physical table.
 *
 * Get a count of all distinct fields for a given physical table ID in the
 * specified unit.
 *
 * The count includes only unique field IDs regardless of the field width.
 * For example, if a field occupies 128 bits (i.e. 2 field elements),
 * the returned count for this is 1.
 *
 * Unlike the corresponding DRD function, the list count includes
 * the special key fields, such as the index key field (for indexed memory
 * tables), or the port key field (for port-based register).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [out] num_fid Number of field IDs in table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pt_field_count_get(int unit, bcmdrd_sid_t sid, size_t *num_fid);


/*!
 * \brief  Get list of fields IDs for given physical table ID.
 *
 * Get the list of distinct fields IDs for a given physical table ID in the
 * specified unit.
 *
 * The list includes only unique field IDs regardless of the field width.
 * For example, if a field occupies 128 bits (i.e. 2 field elements),
 * the returned list contains 1 field.
 *
 * Unlike the corresponding DRD function, the list includes
 * the special key fields, such as the index key field (for indexed memory
 * tables), or the port key field (for port-based register).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] list_max Number of entries allocated in field ID list.
 * \param [out] fid_list Field ID list.
 * \param [out] num_fid Total number of field IDs returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pt_field_list_get(int unit, bcmdrd_sid_t sid, size_t list_max,
                            bcmdrd_fid_t *fid_list, size_t *num_fid);


/*!
 * \brief Get field information for register/memory field.
 *
 * Obtain field information (name, start bit, end bit, etc.) for
 * specified symbol ID and field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 * \param [out] finfo Field information structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pt_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                            bcmdrd_sym_field_info_t *finfo);

/*!
 * \brief Find given field in transform entry.
 *
 * This routine checks if the given field ID is part of the transform
 * field list.  Dependending on the input argument, it looks into
 * either the source field list (API facing field) or
 * the destination field list (PT).
 *
 * \param [in] unit Unit number.
 * \param [in] entry LRD transform map entry.
 * \param [in] src Indicates to look into the source or destination fields.
 * \param [out] num_fields Number of fields.
 * \param [out] flist Array of fields xfrm information.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_PARAM on NULL entry.
 * \retval SHR_E_INIT on sanity check failure.
 */
extern int
bcmltm_md_xfrm_field_get(int unit, const bcmlrd_map_entry_t *entry,
                         bool src, uint32_t *num_fields,
                         const bcmltd_field_list_t **flist);

#endif /* BCMLTM_MD_H */
