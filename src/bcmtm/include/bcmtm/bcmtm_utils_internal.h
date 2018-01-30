/*! \file bcmtm_utils_internal.h
 *
 * Internal  utility functions.
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

#ifndef BCMTM_UTILS_INTERNAL_H
#define BCMTM_UTILS_INTERNAL_H

#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>

#define BCMTM_POPULATE_OUT_FIELD(fid, count, fval)\
    out->field[count]->id = fid; \
    out->field[count]->data = fval;


typedef
struct bcmtm_lt_pt_mapping {
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    int  pt_offset;
    int granularity;
} bcmtm_lt_pt_mapping_t;

/*!
 * \brief validates the logical table field data and key information.
 *
 * Return if the fields and data corresponding to the fields and keys for
 * logical table is right or not.
 *
 * \param [in] unit         Unit number.
 * \param [in] opcode       Opcode for the logical table (insert/delete/update/lookup).
 * \param [in] sid          Logical table sid.
 * \param [in] in           In field list.
 *
 * \retval 1      in field list is correct.
 * \retval 0      in field list is incorrect.
 */
int
bcmtm_lt_field_validate (int unit,
                        bcmlt_opcode_t opcode,
                        bcmlrd_sid_t sid,
                        const bcmltd_fields_t *in);

/*!
 * \brief validates the logical table operation to be performed.
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in] unit         Unit number.
 * \param [in] opcode       Opcode for the logical table (insert/delete/update/lookup).
 * \param [in] bmap_ret     tracks information about the operation
 *
 * \retval SHR_E_EXISTS     entry exists.
 * \retval SHR_E_NOT_FOUND  entry not found.
 */
int
bcmtm_lt_opcode_validate(int unit,
                         bcmlt_opcode_t opcode,
                         int bmap_ret);

/*!
 * \brief parses in in list and prepares a data list
 *
 * prepare data list with the in parameters and default parameters
 *
 * \param [in] unit         Unit number.
 * \param [in] in           in field list .
 * \param [in] sid          Logical table sid.
 * \param [in] num_fid      max number of fields for the given logical table
 * \param [out] in_data     data list for all the fields
 */
int
bcmtm_in_field_parser (int unit,
                       const bcmltd_fields_t *in,
                       bcmlrd_sid_t sid,
                       size_t num_fid,
                       uint32_t *in_data);

/*!
 * \brief allocate memory to make the field list
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in] unit         Unit number.
 * \param [in] num_fields   number of fields.
 * \param [out] flist       allocated memory for field list
 */
int
bcmtm_field_list_alloc(int unit,
                       size_t num_fields,
                       bcmltd_fields_t *flist);

/*!
 * \brief frees the allocated field list
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in]          flist allocated field list .
 */
void
bcmtm_field_list_free(bcmltd_fields_t *flist);

/*!
 * \brief performs range check
 *
 * verifies the fval is within min max ranges (inclusive of min and max)
 *
 * \param [in] fval         field value
 * \param [in] min          min value
 * \param [in] max          max value
 *
 * \retval 1      in field list is correct.
 * \retval 0      in field list is incorrect.
 */
int
bcmtm_fval_range_check(uint64_t fval,
                       const bcmlrd_field_data_t *fdata,
                       uint32_t width);

/*!
 * \brief returns field value range
 *
 * returns the min and max values supported for a field
 *
 * \param [in] unit         Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Table Field.
 * \param [out] min          min value
 * \param [out] max          max value
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
int
bcmtm_field_value_range_get(int unit, bcmlrd_sid_t sid, uint32_t fid,
                                uint64_t *min, uint64_t *max);


/*!
 * \brief Populated logical table field.
 *
 * \param [in] unit Unit number.
 * \param [in] flist Linked list of fields.
 * \param [in] fid Table Field.
 * \param [in] fval Field values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
extern void
bcmtm_field_populate(int unit,
        bcmltd_fields_t *flist,
        bcmltd_fid_t fid,
        uint32_t idx,
        uint64_t fval);

/*!
 * \brief Default values for the field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Table Field.
 * \param [out] def Default field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
extern int
bcmtm_field_default_value_get(int unit,
        bcmlrd_sid_t sid,
        uint32_t fid,
        uint64_t *def);

/*!
 * \brief Get the field value from field array for Logical table field ID.
 *
 * \param [in] unit  Unit number.
 * \param [in] fid   Logical table field ID.
 * \param [in] field_array Field array having field id and field_value.
 * \param [out] fval field value for the logical table field ID.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_NOT_FOUND field ID not found in field array.
 */
extern int
bcmtm_fval_get_from_field_array(int unit,
        bcmlrd_fid_t fid,
        const bcmltd_field_t *field_array,
        uint64_t *fval);

#endif /* BCMTM_UTILS_INTERNAL_H */
