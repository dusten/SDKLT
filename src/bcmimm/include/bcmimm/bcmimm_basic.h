/*! \file bcmimm_basic.h
 *
 * In-memory basic table handler. This file contains the declerations of
 * all the in-memory basic custom handler functions. In general, the API
 * and its behavior is being documented by the BCMLTA design document.
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
#ifndef BCMIMM_BASIC_H
#define  BCMIMM_BASIC_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Validates entry operation on specific logical table.
 *
 * This function validates if a given operation (given as \c opcode) is
 * viable with the specific entry and logical table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] opcode Is the operation to validate.
 * \param [in] in Contains the entry information.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_validate(int unit,
                                 bcmlt_opcode_t opcode,
                                 const bcmltd_fields_t *in,
                                 const bcmltd_generic_arg_t *arg);

/*!
 * \brief Inserts an entry to a specific logical table.
 *
 * This function inserts an entry into a logical table. Note that this
 * operation had already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] in Contains the entry information.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_insert(int unit,
                               uint32_t trans_id,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Lookup for an entry in a specific logical table.
 *
 * This function lookup for an entry in a logical table. Note that this
 * operation had already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] in Contains the entry information.
 * \param [out] out The output information from the lookup will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_lookup(int unit,
                               uint32_t trans_id,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Deletes an entry from a specific logical table.
 *
 * This function deletes an entry from a logical table. Note that this
 * operation had already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] in Contains the entry information.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_delete(int unit,
                               uint32_t trans_id,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Deletes an entry from a specific logical table.
 *
 * This function deletes an entry from a logical table. Note that this
 * operation had already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] in Contains the entry information.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_update(int unit,
                               uint32_t trans_id,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Commits a transaction.
 *
 * This function commits a transaction that was previously defined by
 * the various insert/delete/update and lookup operations. A transaction
 * may contains multiple operations that were staged before.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_commit(int unit,
                               uint32_t trans_id,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Aborts a transaction.
 *
 * This function aborts a transaction that was previously staged via
 * the various insert/delete/update and lookup operations. As a result, all
 * the staging will be cleaned up and not actual change will take place.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_abort(int unit,
                              uint32_t trans_id,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Table enumeration get first.
 *
 * This function starts the enumeration operation by providing the first
 * entry in the table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * This operation being executed during the stage operation.
 * \param [out] out The content of the first entry will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_first(int unit,
                              uint32_t trans_id,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Table enumeration get next.
 *
 * This function continues the enumeration operation by providing the next
 * entry in the table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * This operation being executed during the stage operation.
 * \param [in] in Is the entry content that was returned in previous call
 * to \ref bcmimm_basic_first or \ref bcmimm_basic_next.
 * \param [out] out The content of the next entry will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_next(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg);

/*!
 * \brief Initialize the custom handler and its associated logical tables.
 *
 * This function finds the list of all the in-memory LT in the \c arg
 * parameter. For each of these LT, this function prepares the associated
 * field information and buffers required to perform the desired operations.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] arg Contains logical table information and parameters. The
 * init function prepare data structures for each of the LT listed in this
 * parameter.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_init(int unit,
                             const bcmltd_handler_lifecycle_arg_t *arg);

/*!
 * \brief Clean-up the custom handler and its resources.
 *
 * This function cleans up all resources associated with this custom handler.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] arg Contains logical table information and parameters.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_cleanup(int unit,
                                const bcmltd_handler_lifecycle_arg_t *arg);

#endif
