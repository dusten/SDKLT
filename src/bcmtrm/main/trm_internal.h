/*! \file trm_internal.h
 *
 * Transaction manager internal header file
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

#ifndef TRM_INTERNAL_H
#define TRM_INTERNAL_H

#include <sal/sal_msgq.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmtrm/generated/bcmtrm_ha.h>

/*!
 *\brief Macros to control entry and transaction ID
 */
#define OP_ID_BITS  23  /*!< number of bits for the operation ID */
#define OP_ID_MASK ((1<<OP_ID_BITS)-1)  /*!< operation ID mask   */

#define INIT_OP_ID(op_id, unit,interactive)         \
    if (interactive) {                              \
        op_id = (unit << OP_ID_BITS) + (1 << 31);   \
    } else {                                        \
        op_id = unit << OP_ID_BITS;                 \
    }

#define INCREMENT_OP_ID(id)  \
    if (((id) & OP_ID_MASK) == OP_ID_MASK) {    \
        (id) = (id) & (0xFFFFFFFF-OP_ID_MASK);  \
    } else {                                    \
        (id)++;                                 \
    }

/*
 * Insert an element (_elem) to double linked list (_list).
 * The element being inserted to the begining of the list.
 */
#define ELEMENT_INSERT(_list, _mtx, _elem)               \
    _elem->debug_prev = NULL;                            \
    sal_mutex_take(_mtx, SAL_MUTEX_FOREVER);             \
    _elem->debug_next = _list;                           \
    if (_list) {                                         \
        _list->debug_prev = _elem;                       \
    }                                                    \
    _list = _elem;                                       \
    sal_mutex_give(_mtx);

/*
 * Extracts element (_elem) from a doubled linked list.
 */
#define EXCLUSIVE_ELEMENT_EXTRACT(_list, _mtx, _elem)           \
    sal_mutex_take(_mtx, SAL_MUTEX_FOREVER);                    \
    if (_elem->debug_prev) {                                    \
        _elem->debug_prev->debug_next = _elem->debug_next;      \
        if (_elem->debug_next) {                                \
            _elem->debug_next->debug_prev = _elem->debug_prev;  \
        }                                                       \
    } else if (_list == _elem) { /* First element? */           \
        _list = _elem->debug_next;                              \
        if (_list) {                                            \
            _list->debug_prev = NULL;                           \
        }                                                       \
    }                                                           \
    sal_mutex_give(_mtx);                                       \
    _elem->debug_next = _elem->debug_prev = NULL;

#define ELEMENT_EXTRACT(_list, _elem)                           \
    if (_elem->debug_prev) {                                    \
        _elem->debug_prev->debug_next = _elem->debug_next;      \
        if (_elem->debug_next) {                                \
            _elem->debug_next->debug_prev = _elem->debug_prev;  \
        }                                                       \
    } else if (_list == _elem) { /* First element? */           \
        _list = _elem->debug_next;                              \
        if (_list) {                                            \
            _list->debug_prev = NULL;                           \
        }                                                       \
    }                                                           \
    _elem->debug_next = _elem->debug_prev = NULL;


typedef struct {
    enum {ENTRY_CB, TRANS_CB, ENTRY_NOTIF, NOTIF_EXIT} type;
    bcmtrm_entry_t *ltm_entry;
    shr_error_t status;
} bcmtrm_hw_notif_struct_t;

extern shr_lmm_hdl_t bcmtrm_ltm_entry_hdl;   /*!< used to obtain ltm entries */

extern sal_msgq_t bcmtrm_hw_notif_q;

extern bool bcmltm_notify_units[];

typedef void (trans_state_set_cb)(int unit,
                                  uint32_t state,
                                  uint32_t trans_id,
                                  uint32_t *hdl);

typedef void (trans_state_update_cb)(int unit,
                                     uint32_t state,
                                     uint32_t trans_id,
                                     uint32_t hdl);

typedef void (trans_state_terminate_cb)(int unit,
                                        uint32_t trans_id,
                                        uint32_t hdl);

typedef struct bcmtrm_trans_state_s {
   trans_state_set_cb *set_f;
   trans_state_update_cb *update_f;
   trans_state_terminate_cb *cancel_f;
   trans_state_terminate_cb *done_f;
} bcmtrm_trans_state_t;

extern bcmtrm_trans_state_t bcmtrm_trans_ha_state;

/*!
 * \brief Process single modeled entry.
 *
 * This function processes a single modeled entry by calling the proper LTM
 * function according to the opcode of the entry. This function calls the
 * ltm action function followed by the ltm commit function. If needed, a
 * callback to the entity issuing the entry operation will be made.
 *
 * \param [in] entry Entry is the table entry to process.
 * \param [in,out] op_id Entry operation ID to use for this request. This
 * number will be incremented by this function after the operation.
 * \param [in,out] trans_id Transaction ID to use for this request. This
 * number will be incremented by this function after the operation.
 *
 * \retval SHR_E_NONE on success and error code for failure.
 */
extern int bcmtrm_process_entry_req(bcmtrm_entry_t *entry,
                                     uint32_t *op_id,
                                     uint32_t *trans_id);

/*!
 * \brief Process single interactive entry.
 *
 * This function processes a single interactive entry by calling the proper
 * LTM function according to the opcode of the entry. Calling the LTM with
 * interactive entry is a blocking call that will blocks until the hardware
 * had been updated. Therefore, there is no reason to call the ltm commit
 * function for this entry.
 *
 * \param [in] entry Entry is the table entry to process.
 * \param [in] op_id Entry operation ID to use for this request.
 * \param [in] ha_mem Is the HA memory to store the operation in case
 * of HA event.
 *
 * \retval SHR_E_NONE on success and error code for failure. Error codes
 * will reflect the return value from the LTM layer.
 */
extern int bcmtrm_process_interactive_entry_req(bcmtrm_entry_t *entry,
                                                uint32_t op_id,
                                                trn_interact_info_t *ha_mem);

/*!
 * \brief Memory allocator for entry field.
 *
 * This function allocates an entry field data structure. The function is
 * used by the LTM for lookup operations. The object is actually allocated
 * from the bcmlt pool via a function provided to the bcmtrm_init()
 * function.
 *
 * \retval pointer to entry field memory on success and NULL on failure.
 */
extern bcmltm_field_list_t* bcmtrm_local_field_alloc(void);


/*!
 * \brief Calls proper LTM staging function.
 *
 * This function allocates an ltm entry structure and call the proper ltm
 * action function based on the entry opcode.
 *
 * \param [in] entry Entry is the table entry to process.
 * \param [in] op_id Entry operation ID to use for this request.
 * \param [in] trans_id Transaction ID to use for this request.
 *
 * \retval The value as it returned from the ltm action function.
 */
int bcmtrm_stage_entry(bcmtrm_entry_t *entry,
                       uint32_t op_id,
                       uint32_t trans_id);

/*!
 * \brief Process a transaction.
 *
 * This function processes a transaction by processing the entries one by
 * one. Atomic transactions will be comitted once after all the entry
 * operations were successfully completed. When processing batch
 * transaction the function treat each entry individually.
 *
 * \param [in] trans Transaction to process by this function.
 * \param [in,out] op_id Entry operation ID to use for this request. During
 * batch processing every entry triggers the function to increment this number.
 * \param [in,out] trans_id Transaction ID to use for this request. During
 * batch processing every entry triggers the function to increment this number.
 * \param [in] user_data Is a synchronous object that synchronizes the caller
 * with the execution of the transaction.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_handle_transaction(bcmtrm_trans_t *trans,
                                     uint32_t *op_id,
                                     uint32_t *trans_id,
                                     void *user_data);

/*!
 * \brief Process interactive transaction.
 *
 * This function processes a transaction that is composed of interactive
 * (or PT) entries only. This transaction must be a batch transaction.
 *
 * \param [in] trans points to the transaction to process.
 * \param [in,out] op_id pointer to the initial entry operation ID.
 * Every entry increments this number.
 * \param [in] ha_mem Is the HA memory to store the operation in case
 * of HA event.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_handle_interactive_transaction(
                                    bcmtrm_trans_t *trans,
                                    uint32_t *op_id,
                                    trn_interact_info_t *ha_mem);

/*!
 * \brief Inform the caller about entry status change.
 *
 * This function informs the caller of the entry operation about a change
 * in the status of the operation. Informing the caller will be done if
 * the notification option matches the option requested by the caller,
 * unless the status indicates error which will always result in
 * informing the caller.
 *
 * \param [in] entry points to the entry to process.
 * \param [in] status is the current entry status.
 * \param [in] notify_opt is the reason for the notification.
 *
 * \retval none.
 */
extern void bcmtrm_appl_entry_inform(bcmtrm_entry_t *entry,
                        shr_error_t status,
                        bcmlt_notif_option_t notif_opt);

/*!
 * \brief Inform the caller about transaction status change.
 *
 * This function informs the caller of the entry operation about a change
 * in the status of the operation. Informing the caller will be done if
 * the notification option matches the option requested by the caller,
 * unless the status indicates error which will always result in
 * informing the caller.
 *
 * \param [in] trans points to the transaction to process.
 * \param [in] status is the current transaction status.
 * \param [in] notify_opt is the reason for the notification.
 *
 * \retval none.
 */
extern void bcmtrm_appl_trans_inform(bcmtrm_trans_t *trans,
                                  shr_error_t status,
                                  bcmlt_notif_option_t notif_opt);

/*!
 * \brief Process ltm results for modeled entry.
 *
 * This function process the ltm structure after the entry was processed by the
 * ltm. If there are output fields (in case of lookup) it moves them to the
 * entry fields list. It then release the ltm entry.
 *
 * \param [in] entry points to the entry to process.
 * \param [in] ltm_entry points to the ltm entry to process.
 *
 * \retval none.
 */
extern void bcmtrm_proc_entry_results(bcmtrm_entry_t *entry,
                             bcmltm_entry_t *ltm_entry);

/*!
 * \brief Converts LT opcodes to string.
 *
 * This function used for debug purposes only. It returns a string that is
 * corresponding to the opcode provided as input parameter.
 *
 * \param [in] opcode The opcode to convert.
 *
 * \retval The string associated with the given opcode.
 */
extern const char* bcmtrm_ltopcode_to_str(bcmlt_opcode_t opcode);

/*!
 * \brief Set the states of all entries in a transaction.
 *
 * This function sets the state of all the entries in the transaction to the
 * desired state.
 *
 * \param [in] trans point to the transaction to process.
 * \param [in] state is the desired state that the entries should be set with.
 *
 * \retval none.
 */
extern void bcmtrm_set_trans_entries_state(bcmtrm_trans_t *trans, int state);

/*!
 * \brief Queue interactive entry for processing the the interactive thread.
 *
 * This function being called when an interactive entry being processed in a
 * batch transaction. Since this action is blocking, its being transfered to be
 * processed by the interactive thread so the modeled thread can continue
 * without being blocked.
 *
 * \param [in] entry points to the entry to process.
 * \param [in] priority indicates the priority level of which this entry should
 * be processed. It is taken from the transaction priority.
 *
 * \retval none.
 */
extern void bcmtrm_q_interactive_entry(bcmtrm_entry_t *entry,
                                    bcmlt_priority_level_t priority);

/*!
 * \brief Process transaction operation complete.
 *
 * This function being called when the last entry of a transaction had been
 * processed. It makes sure that all the ltm entries had been released and than
 * it makes sure that the proper completion callback will be made. This
 * function is called only when the h/w operation was completed or if an error
 * had occur and the processing of the transaction had stopped.
 *
 * \param [in] trans points to the transaction to process.
 * \param [in] notify_opt is the reason for the notification.
 * \param [in] status is the current transaction status.
 *
 * \retval none.
 */
extern void bcmtrn_trans_cb_and_clean(bcmtrm_trans_t *trans,
                                    bcmlt_notif_option_t notif_opt,
                                    shr_error_t status);

/*!
 * \brief Process stop event.
 *
 * This function being called to handle stop event. When the function completes
 * its action the threads associated with the unit should been exited.
 * Stopping the threads can be done gracefully or not based on the graceful
 * input parameter.
 *
 * \param [in] unit is the unit nuber.
 * \param [in] graceful indicates the method to stop the threads. Graceful
 * methods will allow the threads to process everything they currently have
 * inside their queue while in non-graceful mode the thread only finishes
 * its current on-going operation.
 *
 * \retval SHR_E_NONE on success and SHR_E_TIMEOUT on failure.
 */
extern int bcmtrm_stop(int unit, bool graceful);

/*!
 * \brief Free all transaciton resources.
 *
 * This function being called to free all the transaction resources once
 * the processing of the transaction had completed.
 *
 * \param [in] trans points to the transaction to free.
 * \param [in] exclusive Indicates if the function has exclusive control over
 * the active transaction list or not.
 *
 * \retval none.
 */
extern void bcmtrm_trans_done(bcmtrm_trans_t *trans, bool exclusive);

/*!
 * \brief Notification thread.
 *
 * This threads handles the application notification for all units.
 * Notification can be made for asynchronous operation as well as table
 * change notifications.
 *
 * \param [in] arg Arg is used to communicate between the thread and the
 * thread creator. It is used as a counter to signal that the thread is
 * running or existing.
 *
 * \retval none.
 */
extern void bcmtrm_notify_thread(void *arg);

/*!
 * \brief Allocates table event vector.
 *
 * This function allocates table event vector to store function callback
 * associated with the tables. This function being called during unit
 * initialization.
 *
 * \param [in] unit Unit associated with the table events.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_table_event_alloc(int unit);

/*!
 * \brief Free table event vector.
 *
 * This function frees table event vector, typically when a unit being
 * detached.
 *
 * \param [in] unit Unit associated with the table events.
 *
 * \retval none.
 */
extern void bcmtrm_table_event_free(int unit);

/*!
 * \brief Retrieve mutext associated with a unit.
 *
 * This function retrieves a mutex associated with a particular unit. The
 * purpose of the mutex is to prevent multiple staging operations on the
 * same unit simultaneously.
 *
 * \param [in] unit Unit to retrieve the mutex for.
 *
 * \retval Mutex handle. This function should not fail unless TRM init fails.
 */
extern sal_mutex_t bcmtrm_unit_mutex_get(int unit);

/*!
 * \brief Retrieve mutext protecting entry state change.
 *
 * This function retrieves the mutex that allows the state of the entry to
 * be updated. This function returns a mutext that is global as oppose
 * to per unit.
 *
 * \retval Mutex handle. This function should not fail unless TRM init fails.
 */
extern sal_mutex_t bcmtrm_entry_mutex_get(void);

/*!
 * \brief Free the entry and its resources.
 *
 * Calling to this function when all the operations associated with the entry
 * had been completed. This function frees (recycle) all the resources
 * associated with the entry.
 *
 * \param [in] entry Entry to recycle back to the free list.
 * \param [in] exclusive Indicate if the function has exclusive control over
 * the active entry list or not.
 *
 * \retval none.
 */
extern void bcmtrm_entry_done(bcmtrm_entry_t *entry, bool exclusive);

/*!
 * \brief The last thing to do in the processing of a transaction
 *
 * This function is only significant for asynch transactions. These types of
 * transactions can be freed asynchronously. This function therefore check
 * if the delete flag was set for this transaction. If it did it will free
 * the transaction. In any case, it will only mark the transaction state to
 * T_ACTIVE. Being in this state enables the bcmtrm_trans_free() to free the
 * transaction.
 *
 * \param [in] trans Transaction to handle for completion.
 *
 * \retval none.
 */
extern void bcmtrm_trans_complete(bcmtrm_trans_t *trans);

/*!
 * \brief The last thing to do in the processing of an entry.
 *
 * This function is only significant for asynch entries. These types of
 * entries can be freed asynchronously. This function therefore check
 * if the delete flag was set for this entry. If it did it will free
 * the entry. In any case, it will only mark the entry state to E_ACTIVE.
 * Being in this state enables the bcmtrm_entry_free() to free the entry.
 *
 * \param [in] entry Entry to handle for completion.
 *
 * \retval none.
 */
extern void bcmtrm_entry_complete(bcmtrm_entry_t *entry);

/*!
 * \brief Recover all open transaction during warm boot.
 *
 * This function go through all the open transactions and decides, based on
 * their state, to abort or commit them. The function recovers the transactions
 * until nothing to verify or until no more
 *
 * \param [in] unit Is the recovered unit number.
 *
 * \retval SHR_E_NONE on success and error code when a particular transaction
 * failed to be recovered
 */
extern int bcmtrm_recover(int unit);

/*!
 * \brief Recovers specific transaction.
 *
 * This function recovered specific transaction of the given unit. This
 * function calls, based on the transaction state, to abort or commit the
 * transaction.
 *
 * \param [in] unit Is the unit where the recovery operation being performed.
 * \param [in] trn_state Is the transaction state structure. It contains the
 * transaction ID and transaction state.
 *
 * \retval SHR_E_NONE on success and SHR_E_BUSY when entry execution stalled.
 */
extern int bcmtrm_recover_trans(int unit, trn_state_t *trn_state);

/*!
 * \brief Recovers interactive operation
 *
 * This function releats the last interactive operation by issuing the
 * complete entry operation.
 *
 * \param [in] unit Is the unit where the recovery operation being performed.
 * \param [in] trn_state Is the transaction state structure. It contains the
 * transaction ID and transaction state.
 *
 * \retval none.
 */
extern void bcmtrm_recover_interact(int unit, trn_interact_info_t *ha_mem);


/*!
 * \brief South bound table change notification.
 *
 * This function receives an LTM entry change event and post it into the
 * notify thread.
 * This function only operates on logical tables.
 *
 * \param [in] unit is the unit number.
 * \param [in] high_pri Indicates if the event is high priority event.
 * \param [in] table_id is the logical table ID for the table
 * associated with the change.
 * \param [in] opcode is the operation code that was perform. Note that
 * the application needs to know if it was insert/update or delete operation.
 * \param [in] fields is a list of the entry fields. The fields will
 * be used by the TRM asynchronously and therefore the application
 * should not free the fields. The fields list should be allocated using
 * the shared field memory manager (shr_fmm) utility.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmtrm_sb_table_notify(int unit,
                                  bool high_pri,
                                  uint32_t table_id,
                                  bcmlt_opcode_t opcode,
                                  bcmltm_field_list_t *fields);


/*!
 * \brief Send table changed event to registered application.
 *
 * This function checks if an application callback function was registered
 * to receive change notifications. If a function was found, it will be
 * called with the modified entry.
 *
 * \param [in] entry Is the changed entry in the table.
 *
 * \return None.
 */
extern void bcmtrm_tbl_chg_event(bcmtrm_entry_t *entry);

#endif /* TRM_INTERNAL_H */
