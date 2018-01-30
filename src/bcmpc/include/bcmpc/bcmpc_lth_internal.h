/*! \file bcmpc_lth_internal.h
 *
 * BCMPC Logical Table Handler (LTH) Utilities.
 *
 * Declaration of the LTH utilities which are used by BCMPC internally.
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

#ifndef BCMPC_LTH_INTERNAL_H
#define BCMPC_LTH_INTERNAL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Add a field to the given field array.
 *
 * \param [in] _flds Field array, refer to bcmltd_fields_t.
 * \param [in] _fid Field ID, refer to bcmltd_field_t.
 * \param [in] _fidx Field Index, refer to bcmltd_field_t.
 * \param [in] _fdata Field data, refer to bcmltd_field_t.
 * \param [in,out] _fcnt Fields count. The \c _fcnt will increase 1 after the
 *                       field is added.
 */
#define BCMPC_LTH_FIELD_ADD(_flds, _fid, _fidx, _fdata, _fcnt) \
            do { \
                _flds->field[_fcnt]->id = _fid; \
                _flds->field[_fcnt]->idx = _fidx; \
                _flds->field[_fcnt]->data = _fdata; \
                _fcnt++; \
            } while (0);

/*!
 * \brief Get the value of a field from input array.
 *
 * \param [in] unit Unit number.
 * \param [in] in LTA input field array.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
extern int
bcmpc_lth_field_get(int unit, const bcmltd_fields_t *in, uint32_t fid,
                    uint64_t *fval);

/*!
 * \brief Get the minimum and maximum value of the given field.
 *
 * This function will get the minimum and maximum value of a LT field
 * from the LRD field definition.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Table Field.
 * \param [out] min Minimum field value.
 * \param [out] max Maximum field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
int
bcmpc_lth_field_value_range_get(int unit, uint32_t sid, uint32_t fid,
                                uint64_t *min, uint64_t *max);

/*!
 * \brief Validate the value of the given field.
 *
 * This function uses the LRD field definition to validate the input field
 * value.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field Table Field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Some or all field values are invalid.
 */
extern int
bcmpc_lth_field_validate(int unit, uint32_t sid, const bcmltd_field_t *field);

/*!
 * \brief Validate the values of the input field array.
 *
 * This function uses the LRD field definition to validate the input field
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] in LTA input field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Some or all field values are invalid.
 */
extern int
bcmpc_lth_fields_validate(int unit, uint32_t sid, const bcmltd_fields_t *in);

#endif /* BCMPC_LTH_INTERNAL_H */
