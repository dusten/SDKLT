/*! \file shr_sysm.h
 *
 * System manager API
 *
 * Interface to the system manager
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

#ifndef SHR_SYSM_H
#define SHR_SYSM_H

#include <sal/sal_types.h>

/*!
 * \brief System manager init.
 *
 * This function initializes the system manager by providing some operational
 * parameters.
 *
 * \param [in] max_component_id Indicates the maximal value of a component ID.
 * \param [in] internal_thread_mode Indicates the threading mode. If not
 * internal, the system manager will not create any thread and will rely on
 * an application thread to be the main engine that runs the system manager.
 * If internal mode, the system manager will create thread for every system
 * manager instance.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int shr_sysm_init(uint32_t max_component_id, bool internal_thread_mode);

/*!
 * \brief System manager delete.
 *
 * This function deletes the system manager by cleaning up all its resources.
 * Call this function only after all the instances had been deleted
 * (see \ref shr_sysm_instance_delete).
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int shr_sysm_delete(void);

/*!
 * \brief System manager instance categories.
 *
 * These are the system manager instance categories. A component will typically
 * register itself to one type of category, however, some will be registered for
 * multiple categories. If a component registers itself into multiple categories
 * it will (typically) use different callback functions as the activity in each
 * category is different.
 */
typedef enum {
    SHR_SYSM_CAT_UNIT = 0, /*!< type unit instance, instance for every unit*/
    SHR_SYSM_CAT_GENERIC,  /*!< type is generic, not multi instances       */
    SHR_SYSM_CAT_COUNT     /*!< Determine the maximal types number         */
} shr_sysm_categories_t;

/*!
 * \brief System manager define category.
 *
 * This function defines a system manager category parameters. IN particular it
 * specifies what are the maximal instance number that should be part of this
 * category.
 *
 * \param [in] cat Is the category to define.
 * \param [in] max_num_of_instances Indicates the maximal instance number
 * under this category.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int shr_sysm_category_define(shr_sysm_categories_t cat,
                                    uint32_t max_num_of_instances);

/*!
 * \brief System manager returned value.
 *
 * These are the values that a system manager callback function may return.
 */
typedef enum {
    SHR_SYSM_RV_BLOCKED,/*!< Can't finish due to dependency on other component*/
    SHR_SYSM_RV_DONE,   /*!< Operation completed                               */
    SHR_SYSM_RV_ERROR   /*!< Operation failed                                  */
} shr_sysm_rv_t;


/*!
 * \brief System manager callback function.
 *
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 * \param [in] instance_cat Is an indicator of the system manager
 *              instance. For SHR_SYSM_CAT_GENERIC the unit parameter
 *              should be ignored.
 * \param [in] unit The device unit number. This identified the instance
 *              of the system manager.
 * \param [in] comp_data Is a pointer to the component context provided
 *              during the component registration.
 * \param [in] warm Is a boolean value indicate warm or cold start.
 * \param [out] blocking_comp Set this parameter if the component is blocked
 *              by other component. In this case the function returns
 *              SHR_SYSM_RV_BLOCKED and sets the blocking_cmponent to
 *              point to the name of the blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, BCM_SYS_MGR_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
typedef shr_sysm_rv_t (*shr_sysm_cb)(shr_sysm_categories_t instance_cat,
                                     int unit,
                                     void *comp_data,
                                     bool warm,
                                     uint32_t *blocking_comp);

/*!
 * \brief System manager stop callback function.
 *
 * This function signature is used by the system manager to invoke a component
 * stop callback event. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 * \param [in] instance_cat Is an indicator of the system manager
 *              instance. For SHR_SYSM_CAT_GENERIC the unit parameter
 *              should be ignored.
 * \param [in] unit The device unit number. This identified the instance
 *              of the system manager.
 * \param [in] comp_data Is a pointer to the component context provided
 *              during the component registration.
 * \param [in] graceful Is a boolean value indicating if the stop should
 * be performed gracefully or not (i.e. A.S.A.P).
 * \param [out] blocking_comp Set this parameter if the component is blocked
 *              by other component. In this case the function returns
 *              SHR_SYSM_RV_BLOCKED and sets the blocking_cmponent to
 *              point to the name of the blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, BCM_SYS_MGR_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
typedef shr_sysm_rv_t (*shr_sysm_stop_cb)(shr_sysm_categories_t instance_cat,
                                          int unit,
                                          void *comp_data,
                                          bool graceful,
                                          uint32_t *blocking_comp);

/*!
 * \brief System manager run callback function
 *
 * This function signature is used for the run function. A
 * component should provide this callback function if it chooses to be
 * called for the run event.
 *
 * \param [in] instance_cat Is an indicator of the system manager
 *              instance. For SHR_SYSM_CAT_GENERIC the unit parameter
 *              should be ignored.
 * \param [in] unit The device unit number. This identified the instance
 *              of the system manager.
 * \param [in] comp_data Is an opaque pointer to the component context provided
 *              during the component registration.
 *
 * \return none.
 */
typedef void (*shr_sysm_run_cb)(shr_sysm_categories_t instance_cat,
                                int unit,
                                void *comp_data);

/*!
 * \brief Component callback function set.
 */
typedef struct shr_sysm_cb_func_set_s {
    /*! The init function callback (or NULL if N/A) */
    shr_sysm_cb init;
    /*! The component configuration function callback (or NULL if N/A) */
    shr_sysm_cb comp_config;
    /*! The component pre system configuration function callback
     * (or NULL if N/A)
     */
    shr_sysm_cb pre_config;
    /*! The run function callback (or NULL if N/A). This
     * function will be called when moving into the run state
     */
    shr_sysm_run_cb run;
    /*! The stop function callback (or NULL if N/A). The
     * stop function stops the processing of new component events that are
     * associated with this instance. By stopping the process the system
     * manager enables the next step of shut down where it knows that there
     * are no more requests in transition. The stop callback function is being
     * called as a result of calling ref shr_sysm_instance_stop() function.
     */
    shr_sysm_stop_cb stop;
    /*! The shutdown function callback (or NULL if N/A). This function
     * should free all the component resources associated with this type of
     * instance.
     */
    shr_sysm_cb shutdown;
}shr_sysm_cb_func_set_t;

/*!
 * \brief Component registeration with system manager.
 *
 * This function enables a component to register callback functions with
 * a particular instance type of the system manager. As a result, the
 * system manager will call the registered components functions whenever
 * a new instance of this type will be created or destroyed.
 *
 * \param [in] comp_id is component ID.
 * \param [in] instance_cat is an indicator of the system manager
 *              instance.
 * \param [in] cb_set pointer to the set of the component callback functions.
 * \param [in] comp_data is a pointer to the component context.
 *
 * \return none.
 */
extern int shr_sysm_register(uint32_t comp_id,
                             shr_sysm_categories_t instance_cat,
                             shr_sysm_cb_func_set_t *cb_set,
                             void *comp_data);


/*!
 * \brief System manager component complete query
 *
 * This function returns true if the component of a particular system manager
 * instance has completed.
 *
 * \param [in] comp_id is the component ID.
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 *
 * \return true is the component completed its operation for this phase and
 *         false otherwise.
 */
extern bool shr_sysm_is_comp_complete(uint32_t comp_id,
                                          shr_sysm_categories_t instance_cat,
                                          int unit);

/*!
 * \brief State change callback function.
 *
 * This function prototype is used when calling system manager state
 * change function for particular instance. Since state change
 * operation might take some time, this function used to
 * asynchronously inform the caller about the completion of the state
 * change operation.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] status Is the status of the completion. SHR_E_NONE if successful
 * and error code otherwise.
 *
 * \return none.
 */
typedef void (*sysm_instance_state_chng_cb)(shr_sysm_categories_t instance_cat,
                                            int unit,
                                            int status);
/*!
 * \brief System manager instance new.
 *
 * This function creates a new instance of the system manager based on the
 * instance category and unit supplied. Every component that was previously
 * registered with the system manager using this instance category will
 * automatically be associated with this system manager instance. See
 * \ref shr_sysm_register().
 *
 * Typically, when new unit being attached this function will be called with
 * SHR_SYSM_CAT_UNIT and the new unit number as input parameters.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] warm is a boolean value indicate cold or warm start.
 * \param [in] single_step is a boolean value indicate if the instance
 * should be executed in single step mode.
 * \param [in] func is user provided callback function to inform the
 * caller that the state transition had been completed.
 *
 * \return SHR_E_NONE on success and error code otherwise
 */
extern int shr_sysm_instance_new(shr_sysm_categories_t instance_cat,
                                int unit,
                                bool warm,
                                bool single_step,
                                sysm_instance_state_chng_cb func);

/*!
 * \brief System manager instance delete.
 *
 * This function deletes an instance of the system manager based on the
 * instance type and unit. This function will free all resources associated with
 * the instance. Note that the instance must be stopped prior to delete by
 * calling the function \ref shr_sysm_instance_stop().
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_sysm_instance_delete(shr_sysm_categories_t instance_cat,
                                   int unit);

/*!
 * \brief System manager instance stop.
 *
 * This function stops an instance of the system manager based on the
 * instance category and unit provided. All components associated with this
 * instance will have their stop call-back function invoked. This function
 * is non-blocking and the caller can determine when the operation have
 * completed by providing a call-back function. The provided call-back function
 * will be called only when the instance had shut down.
 * The shutdown process can be done gracefully. In this case all the operations
 * that had been requested will complete their execution. None-graceful
 * shutdown will stop any further operation and will shutdown as soon as
 * possible. One scenario using non-graceful shutdown is when extracting line
 * card from a chassis.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] graceful indicates if the stop is graceful or not.
 * \param [in] func is user provided callback function to inform the
 * caller that the state transition had been completed.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_sysm_instance_stop(shr_sysm_categories_t instance_cat,
                                 int unit,
                                 bool graceful,
                                 sysm_instance_state_chng_cb func);

/*!
 * \brief System manager instance step.
 *
 * This function execute one or more steps on an instance of the system
 * manager based on the instance category and unit. For this function
 * to be effective, the system manager instance must be created in
 * a single step mode. This function is useful only for debug
 * purposes.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] number_of_steps indicates how many single steps to.
 * execute by system manager. Single step involved single call to
 * single component. Set this parameter to 0xFFFFFFFF to run in normal mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_sysm_instance_step(shr_sysm_categories_t instance_cat,
                                 int unit,
                                 uint32_t number_of_steps);


/*!
 * \brief System manager instance process.
 *
 * This function processing an instance. The purpose of this function is to
 * be used in an external threading model. The function will block for the
 * duration of processing the system manager state machine.
 * This function should be used in two scenarios:
 * 1) After calling \ref shr_sysm_instance_new()
 * 2) After calling \ref shr_sysm_instance_stop()
 * In both cases the functions will only prepare the system manager instance
 * to be processed but will not block for the actual states processing. It is
 * only \ref shr_sysm_instance_process() that will actually perform the state
 * transitions of the particular system manager instance.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_sysm_instance_process(shr_sysm_categories_t instance_cat,
                                    int unit);

#endif
