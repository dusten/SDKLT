/*! \file bcmltm.h
 *
 * BCMLTM Module Interface.
 *
 * Logical Table Manager.
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the Logical Table Manager module.
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

#ifndef BCMLTM_H
#define BCMLTM_H

#include <bcmltm/bcmltm_types.h>
#include <bcmptm/bcmptm_types.h>

/*!
 *
 * \brief Initialize the metadata for managing the LTs on a specific unit.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_init(int unit, bool warm);

/*!
 * \brief Cleanup LTM metadata on a specific unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_cleanup(int unit);

/*!
 * \brief Entry operation functions.
 *
 * All the entry operation functions receive a pointer to bcmltm entry.
 * The pointer is obtained by the caller to the operation functions and
 * should remain valid until one of the following occur:
 * 1. A call to bcmltm_abort() with match transaction ID occur. Once
 * the bcmltm_abort() returns the entry pointer no longer valid.
 * 2. Once the transaction callback function (notif_fn) has been called.
 * The entry is considered invalid once the callback function has returned.
 *
 * The pointer to the entry will be freed by the caller to the operation
 * functions.
 */

/*!
 *
 * \brief Process one LT entry insert operation.
 *
 * The LTM implementation of a single LT entry insert operation, which
 * creates a new element with the specified key and value fields.
 * Unspecified fields are assigned the software default.
 * Index tables track whether the entry is in-use and reject a second
 * insert operation.
 * Index with Allocation tables track whether the entry is reserved
 * and reserve the index if not.
 * Keyed table entries may be rejected by the PTM if insufficient
 * resources exist to place the entry.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_insert(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry lookup operation.
 *
 * The LTM implementation of a single LT entry lookup operation.
 * The entry with matching key must have been previously supplied
 * with the INSERT command.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_lookup(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry delete operation.
 *
 * The LTM implementation of a single LT entry delete operation.
 *
 * Simple index tables will have the entry matching the supplied key
 * returned to software default field values.  The in-use status is
 * cleared.
 *
 * Keyed table entries will be removed from the device resouce.
 *
 * Index will Allocation tables may supply a range argurment in
 * addition to the index key value.  If the range value is not
 * supplied, it is presumed to mean a range of 1.
 * The index key value is be interpreted as a base index.
 * For each index in the sequence [base_index, base_index + range - 1]
 * these steps are taken:
 *  1) Write corresponding PT entries to the device to set all
 *     fields to software defaults.
 *        -- More complex tables may require profile reference count
 *           management, aggregation list remove, etc.
 *  2) Clear the in-use status.
 *  3) Mark the entry index as unreserved.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_delete(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry update operation.
 *
 * The LTM implementation of a single LT entry update operation, which
 * will change only the specified values fields for the entry which
 * matches the given key field.
 * This operation also permits the removal of individual fields, which
 * are returned to software default and/or removed from (P-)PDD.
 * The LT entry with this key must have been previously supplied by the
 * INSERT commant.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_update(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry traverse start operation.
 *
 * The LTM implementation of a single LT entry traverse start operation,
 * which will return the fields of the first entry in a LT.
 * For index LTs, the first entry is the first index within the in-use
 * bitmap.
 * For Keyed LTs, the PTRM determines an ordering for the entries which
 * it tracks.  This tracking begins for a LTID with this traverse start.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_traverse_first(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry traverse next operation.
 *
 * The LTM implementation of a single LT entry traverse next operation,
 * which will return the fields of the next entry in a LT.  The key
 * fields of the previous entry are supplied to this operation.
 * For index LTs, the next entry is the subsequent index of the in-use
 * bitmap.
 * For Keyed LTs, the PTRM determines an ordering for the entries which
 * it tracks.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_traverse_next(bcmltm_entry_t *entry);

/*
 * PassThru exclusive functions
 */

/*!
 *
 * \brief Process one LT entry PT set operation.
 *
 * The LTM implementation of a single LT entry PT set operation.
 * Constructs a PT entry for the indicated physical resource with
 * each fields set to either a provided API field value or
 * the hardware default field value.
 * This entry is writen to the device resource indicated by the
 * supplied index field value.  Some device resources may accept
 * additional memory parameters to specify the exact entry location.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_set(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT modify operation.
 *
 * The LTM implementation of a single LT entry PT modify operation.
 * The PT entry indicated by the index field value plus other memory
 * parameters is retrieved from the PTM.  The supplied API field values
 * overwrite the current physical field value.  Then the entry is
 * written back to the device resource.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_modify(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT get operation.
 *
 * The LTM implementation of a single LT entry PT get operation.
 * The PT entry indicated by the index field value plus other memory
 * parameters is retrieved from the PTM. The physical field values
 * are copied to the API output field list.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_get(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT clear operation.
 *
 * The LTM implementation of a single LT entry PT clear operation.
 * The PT entry indicated by the index field value plus other memory
 * parameters is written with an entry containing all physical fields
 * set to the hardware default field value.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_clear(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT insert operation.
 *
 * The LTM implementation of a single LT entry PT insert operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_insert(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT delete operation.
 *
 * The LTM implementation of a single LT entry PT delete operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_delete(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT lookup operation.
 *
 * The LTM implementation of a single LT entry PT lookup operation.
 *
 * \param [in] entry pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_lookup(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT FIFO pop operation.
 *
 * The LTM implementation of a single LT entry PT FIFO pop operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_fifo_pop(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT FIFO push operation.
 *
 * The LTM implementation of a single LT entry PT FIFO push operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_fifo_push(bcmltm_entry_t *entry);

/*!
 *
 * \brief Commit all entries with transaction ID.
 *
 * All entries with the specified ID should be committed to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID to commit. All entries belonging
 *              to this transaction will be committed.
 * \param [in] notif_fn Notification callback to application.
 * \param [in] context Notification callback cookie.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_commit(int unit,
              uint32_t trans_id,
              bcmptm_trans_cb_f notif_fn,
              void *context);

/*!
 *
 * \brief Abort all entries with transaction ID.
 *
 * The operation for all entries with the specified ID should
 * be aborted and the state should be restored to the state
 * before the entry(ies) with this ID were installed.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID to abort. All entries belonging
 *              to this transaction will be aborted.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_abort(int unit,
             uint32_t trans_id);


/*!
 * \brief Increment LT_STATS field by LTID.
 *
 * Wrapper for auto-incrementing various LT_STATS values given the LTID
 * of the table to update.  It must verify LT in transaction and
 * retrieve the lastest LT state.  This function is intended for use
 * by modules outside of the LTM implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [in] stat_field FID indicating which stat is to increment.
 */
extern void bcmltm_stats_increment_lt(int unit,
                                      uint32_t ltid,
                                      uint32_t stat_field);


#endif /* BCMLTM_H */
