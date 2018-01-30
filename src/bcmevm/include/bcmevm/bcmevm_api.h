/*! \file bcmevm_api.h
 *
 *  BCM component generated notification API
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

#ifndef BCMEVM_API_H
#define BCMEVM_API_H

#include <bcmltm/bcmltm_types.h>
#include <bcmltd/bcmltd_lt_types.h>

/*!
 * \brief North bound table change notification.
 *
 * This function is a north bound function registered with the component
 * generation table change notification component. This callback function
 * records the request and post it with one of the transaction manager
 * notification thread. Therefore this function will return fast.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Logical table ID for the table
 *                      associated with the change.
 * \param [in] opcode Operation code that was perform. Note that the
 *                    application needs to know if it was insert/update
 *                    or delete operation.
 * \param [in] fields List of entry fields.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
typedef int (bcmevm_cb)(int unit,
                        bool high_pri,
                        uint32_t table_id,
                        bcmlt_opcode_t opcode,
                        bcmltm_field_list_t *fields);

/*!
 * \brief North bound registration for callback function.
 *
 * This function registers north bound callback function that will
 * be call whenever the notification function \ref bcmevm_table_notify()
 * being called.
 *
 * \param [in] cb Is a pointer to callback function.
 *
 * \return None.
 */
extern void bcmevm_register_cb(bcmevm_cb *cb);

/*!
 * \brief South bound table change notification.
 *
 * This function is being called by a component that can update an LT
 * entry, for example a port state change. The function provides the
 * SDK components with the ability to inform the application about
 * such asynchronous events. The application is required to register a
 * callback function with a particular LT to receive table changes.
 *
 * If an entry was changed the component should only provide the
 * modified fields along with the key fields. Fields that were
 * unchanged should not be required to be present. If the application
 * would like to obtain the complete entry it can do this at any time
 * using a lookup operation. For this reason it is important to
 * provide the key fields with the entry.
 *
 * This function only works for LOGICAL tables as the application can
 * not register itself for changes in physical tables.
 *
 * \param [in] unit Unit number.
 * \param [in] high_pri Indicates if the event is high priority event.
 * \param [in] table_id Logical table ID for the table associated with
 *                      the change.
 * \param [in] opcode Operation code that was performed. Note that the
 *                    application needs to know if it was insert/update
 *                    or delete operation.
 *
 * \param [in] fields List of entry fields. The fields will be used by
 *                    the TRM asynchronously and therefore the
 *                    application should not free the fields. The
 *                    fields list should be allocated using the shared
 *                    field memory manager (shr_fmm) utility.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmevm_table_notify(int unit,
                               bool high_pri,
                               uint32_t table_id,
                               bcmlt_opcode_t opcode,
                               bcmltm_field_list_t *fields);


/*!
 * \brief Register to receive table change events.
 *
 * This function being called by a component that is interested to receive
 * notification for LT changes that are made by a component (i.e. south bound
 * changes). Changes made to LT by the application (north bound changes) will
 * not be notified to the component via this mechanism.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Identifier of the table of interest.
 * \param [in] unit cb Caller provided callback function.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmevm_register_tbl_event(int unit,
                                     uint32_t table_id,
                                     bcmevm_cb *cb);

/*!
 * \brief Event callback function prototype.
 *
 * This function prototype used to carry event notifications for components
 * that were registered to receive such notifications.
 *
 * The callback function being called from the context of the event poster
 * thread. Therefore the action taken by the component should be minimal.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event for which the function had been called for.
 * \param [in] ev_data Data associated with the event.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
typedef void (bcmevm_event_cb)(int unit,
                               const char *event,
                               uint64_t ev_data);

/*!
 * \brief Register callback function to receive published event.
 *
 * This function can be used by components that wishes to receive notifications
 * of the specified event.
 *
 * It is expected that the call to this function will happen only after
 * the init state of the system manager.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] cb Callback function to call when the event being posted.
 *
 * \return SHR_E_NONE on success and error code on failure. Possible error codes
 * can be:
 * SHR_E_UNIT - for invalid unit.
 * SHR_E_PARAM - for invalid event or callback function.
 */
extern int bcmevm_register_published_event(int unit,
                                           const char *event,
                                           bcmevm_event_cb *cb);

/*!
 * \brief Unegister callback function from receiving published event.
 *
 * This function can be used by components that wishes to stop receiving
 * notifications of the specified event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] cb Registered callback function that was registered to
 * the receive the given event
 *
 * \return SHR_E_NONE on success and error code on failure. Possible error codes
 * can be:
 * SHR_E_UNIT - for invalid unit.
 * SHR_E_PARAM - for invalid event or callback function.
 */
extern int bcmevm_unregister_published_event(int unit,
                                            const char *event,
                                            bcmevm_event_cb *cb);

/*!
 * \brief Notify other components of an event.
 *
 * This function informs all the components that were interested in receiving
 * the posted event about the occurance of this event. The event can be
 * associated with 64 bit data field. This data will be past to every component
 * callback function.
 * The caller must understand that this function will block until all registered
 * components had been informed.
 * The caller should not be aware if any other component is registered to
 * receive this event or not.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] ev_data Data associated with the event.
 *
 * \return None.
 */
extern void bcmevm_publish_event_notify(int unit,
                                        const char *event,
                                        uint64_t ev_data);

#endif /* BCMEVM_API_H */
