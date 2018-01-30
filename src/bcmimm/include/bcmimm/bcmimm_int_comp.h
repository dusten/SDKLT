/*! \file bcmimm_int_comp.h
 *
 * In-memory Inter component API. This API shoudl use by other components
 * (typically custom handlers) to access the in memory data.
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

#ifndef BCMIMM_INT_COMP_H
#define BCMIMM_INT_COMP_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Lookup for an entry.
 *
 * This function performs a lookup for an entry using the key fields included
 * in \c in structure. Once found, the function will construct the data fields
 * into the output array allocated at \c out.
 * The output list contains only fields that were inserted/updated.
 * Fields, belong to the table, that were never set will not be part of the
 * output.
 * Note - It is important that the caller will free every field in the output
 * list using the function shr_fmm_free().
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry
 * to be looked up. Data fields may be present.
 * \param [out] out This is an output structure. It should contain array of
 * field pointers that is sufficiently large to place all the fields of the
 * entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_lookup(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out);

/*!
 * \brief Update an entry.
 *
 * This function updates an entry by providing the key fields along with the
 * data fields. Only the specified data fields will be modified. Existing
 * fields that are not part of the input will be unchanged.
 *
 * \param [in] unit This is device unit number.
 * \param [in] hi_pri Indicates if the operation should have high priority.
 * This parameter is mostly applicable to the notification that will be
 * generated in response to the change.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing all the fields of the entry
 * to be updated. Both key and data fields should be present.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_update(int unit,
                               bool hi_pri,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Delete an entry.
 *
 * This function deletes an entry using the entry key fields.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry.
 * Other fields might be present as well but will have no impact on the
 * operation.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_delete(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Insert a new entry.
 *
 * This function inserts a new entry into in-memory LT by providing the key
 * and data fields. All the key fields must be provided. Data fields might
 * be partially provide.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing all the fields of the entry
 * to be inserted. Both key and data fields should be present.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_insert(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Lock an entry of the table.
 *
 * This function locks an entry from the logical table specified via \c sid.
 * Locked entry can not be modified (or deleted). This functionality is
 * useful if the entry being referred by another field in a different logical
 * table. By default every entry is unlocked unless explicitly locked.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry
 * to be locked.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_lock(int unit,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in);

/*!
 * \brief Unlock an entry of the table.
 *
 * This function unlocks an entry that was previously locked. Once unlocked,
 * the entry can be modified or deleted.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry
 * to be unlocked.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_unlock(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Table traverse get first table entry.
 *
 * This function starts to traverse the table from its first entry.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [out] out This is a structure to place the entry content in. Note that
 * only fields that were set will be present in the output structure.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_get_first(int unit,
                                  bcmltd_sid_t sid,
                                  bcmltd_fields_t *out);

/*!
 * \brief Table traverse get next table entry.
 *
 * This function continues to traverse the table from its previously traversed
 * entry. The function \ref bcmimm_entry_get_first() should be called prior
 * of calling this function to initiate the traverse output. The output from
 * \ref bcmimm_entry_get_first() should be used at the first call to this
 * function. Otherwise only part of the table might be traversed as this
 * function will start where the previous call to this function was left off.
 * The order of the entries is based on internal oerder maintained by the
 * back-end module hence the caller should not assume anything about the order
 * of entries.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in Is the input fields that must include the key fields. It is
 * initially being obtain via \ref bcmimm_entry_get_first() and later via
 * previous call to bcmimm_entry_get_next(). The input parameter for this
 * function is critical to find the next entry, that is the entry follows the
 * current entry of which its key being provided in this input parameter.
 * \param [out] out This is a structure to place the entry content in. Note that
 * only fields that were set will be present in the output structure.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_get_next(int unit,
                                 bcmltd_sid_t sid,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out);

/*!
 * \brief In-memory notification reason
 *
 * This enumerates the various reasons for notification.
 */
typedef enum bcmimm_entry_event_e {
    BCMIMM_ENTRY_INSERT,    /*!< Insert event. */
    BCMIMM_ENTRY_UPDATE,    /*!< Update event. */
    BCMIMM_ENTRY_DELETE,    /*!< Delete event. */
    BCMIMM_ENTRY_LOOKUP     /*!< Lookup event only used for verification */
} bcmimm_entry_event_t;

/*!
 * \brief In-memory logical table staging callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive table staging events for specific
 * logical table. The callback contains the field values of the entry that
 * was changed along with the key value identify the entry.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_stage_cb)(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key_fields,
                                 const bcmltd_field_t *data_fields,
                                 void *context,
                                 bcmltd_fields_t *output_fields);

/*!
 * \brief In-memory transaction complete callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive transaction commit event
 * for specific logical table.
 * The callback parameter include the table ID and associated transaction ID to
 * commit.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_complete_cb)(int unit,
                                    bcmltd_sid_t sid,
                                    uint32_t trans_id,
                                    void *context);

/*!
 * \brief In-memory transaction abort callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive transaction abort event
 * for specific logical table. This callback mechanism can
 * be used to abort a staged transaction.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \return None.
 */
typedef void (bcmimm_lt_abort_cb)(int unit,
                                    bcmltd_sid_t sid,
                                    uint32_t trans_id,
                                    void *context);

/*!
 * \brief In-memory entry validate callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to participate in the validation phase for specific
 * logical table. The callback contains the field values of the entry that
 * needed validation along with the key value identify the entry.
 * If the entry is valid this function should return no error (SHR_E_NONE),
 * otherwise it should return error code.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_validate_cb)(int unit,
                                    bcmltd_sid_t sid,
                                    bcmimm_entry_event_t action,
                                    const bcmltd_field_t *key_fields,
                                    const bcmltd_field_t *data_fields,
                                    void *context);

/*!
 * \brief In-memory event callback structure.
 *
 * This structure contains all possible callback functions
 * that will be conresponding to various entry commit stages.
 */
typedef struct bcmimm_lt_cb_s {
    bcmimm_lt_validate_cb *validate;  /*!< Validate function */
    bcmimm_lt_stage_cb *stage;        /*!< Staging function */
    bcmimm_lt_complete_cb *commit;       /*!< Commit function */
    bcmimm_lt_abort_cb *abort;        /*!< Abort function */
} bcmimm_lt_cb_t;
/*!
 * \brief Register to receive logical table change event.
 *
 * This function registers a callback function to be called whenever the
 * logical table identified by \c sid has changed. The caller can register a
 * functions for validation, staging, post commit and transaction abort.
 *
 *
 * \param [in] unit Is the device unit number.
 * \param [in] sid Is the logical table ID.
 * \param [in] cb Is the structure that contains the callback functions
 * required for this table.
 * \param [in] context Is a pointer that will be provided with the callback
 * functions. The caller can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_lt_event_reg(int unit,
                               bcmltd_sid_t sid,
                               bcmimm_lt_cb_t *cb,
                               void *context);

#endif /* BCMIMM_INT_COMP_H */

