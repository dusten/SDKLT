/*! \file sysm_internal.h
 *
 *  System manager internal declarations.
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

#ifndef SYSM_INTERNAL_H
#define SYSM_INTERNAL_H

#include <shr/shr_sysm.h>


/*!
 * \brief System manager states
 */
typedef enum shr_sysm_states_e {
    SHR_SYSM_INIT = 0,
    SHR_SYSM_COMP_CONFIG,
    SHR_SYSM_PRE_CONFIG,
    SHR_SYSM_RUN,
    SHR_SYSM_STOP,
    SHR_SYSM_SHUTDOWN,
    SHR_SYSM_STATE_LAST = SHR_SYSM_SHUTDOWN
} shr_sysm_states_t;

/*!
 * \brief Component registration information
 */
typedef struct sysm_component_s {
    uint32_t component_id;        /*!< Component ID                           */
    /*!< vector containing component callback functions                       */
    shr_sysm_cb cb_vector[SHR_SYSM_STATE_LAST + 1];
    void *comp_data;              /*!< component opeque data                  */
    struct sysm_component_s *next;/*!< pointer to the next element in the list*/
} sysm_component_t;

/*!
 * \brief Component instance
 * This data structure being created for every component and every instance
 * of the system manager
 */
typedef struct sysm_component_instance_s {
    sysm_component_t *component_db; /*!< pointer to the component information */
    struct sysm_component_instance_s *next;  /*!< point to the next element   */
} sysm_component_instance_t;

/* forward decleration */
struct sysm_instance_s;

/*!
 * \brief Assist job structure
 * This structure contain the information that being passes to and from the
 * assisting thread into the dedicated thread instance. Since this structure
 * needed to be allocated and free many time during runtime, it uses
 * the shared local memory manager to manage this structure pool
 */
typedef struct sysm_assist_job_s {
    /*!< the instance of the system manager - input */
    struct sysm_instance_s *p_instance;
    /*!< the component or process - input */
    sysm_component_instance_t *p_component_instance;
    /*!< the blocking component ID - output */
    uint32_t blocking_id;
    /*!< the return value from the component callback - output */
    shr_sysm_rv_t rv;
    /*!< Required by the local memory manager */
    struct sysm_assist_job_s *next;
} sysm_assist_job_t;


/*!
 * \brief System manager instance data structure
 */
typedef struct sysm_instance_s {
    /*! the instance state  */
    shr_sysm_states_t state;
    /*! list of active components */
    sysm_component_instance_t *l_component_active;
    /*! list of completed components */
    sysm_component_instance_t *l_component_complete;
    /*! array of lists of blocked components indexed by the blocker ID */
    sysm_component_instance_t **dependencies;
    /*! count how many components left for processing */
    uint32_t component_proc_count;
    /*!
     * count how many components are in the active list always
     *  component_active_count <= component_proc_count
     */
    uint32_t component_active_count;
    /*! how many steps before stopping */
    uint32_t steps_left;
    /*! warm start ? */
    bool warm_start;
    /*! The associated thread handle */
    sal_thread_t thread_hdl;
    /*! is the thread running? */
    bool running;
    /*! The instance category */
    shr_sysm_categories_t type;
    /*! the unit associated with the instace */
    int unit;
    /*! use for freeing the component instances */
    sysm_component_instance_t *allocated_component_mem;
    /*! pointer to application state change function */
    sysm_instance_state_chng_cb state_chng_func;
    /*! array of completed components */
    uint8_t *completed_array;
    /*!
     * synchronization object between the assisting thread and the
     * instance dedicated thread
     */
    sal_mutex_t assisted_mutex;
    /*! list of assisted completed jobs */
    sysm_assist_job_t *l_assisted;
    /*! event semaphore to wake up the dedicated thread */
    sal_sem_t event_sem;
    /*!
     * instance content mutex used to protect access from external
     * functions and the instance dedicated thread
     */
    sal_mutex_t instance_mutex;
} sysm_instance_t;


/*!
 * \brief Move the instance into the next state
 *
 * This function moves the state machine of an instance into the next state
 * and update the active list with the components that should be processed
 * for the next state. If the active list for the new state is empty the
 * function automatically jumps into the next state. The RUN state is special
 * state that the function will not progress unless the auto_mode is off.
 * Also the last state is SHUTDOWN so no progress can be made from there.
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] auto_mode indicates if while in RUN state the function
 * should move to the next state (auto_mode is false) or stay in RUN
 * state (auto_mode is true)
 *
 * \return A pointer to the active list after state change
 */
extern sysm_component_instance_t* shr_sysm_prep_for_next_state(
                                        sysm_instance_t *p_instance,
                                        bool auto_mode);

/*!
 * \brief process instance through its state machine
 *
 * This function process a single instance of the system manager. It assumed
 * to be called for external threading model so there is no assistant thread.
 * The processing will continue until there is nothing to process. This can
 * be a result of a component dependency loop or when the state machine arrives
 * into a state that the system manager should stay in.
 *
 * \param [in] p_instance point to the system manager instance
 *
 * \return SHR_E_NONE on success and SHR_E_RESOURCE on failure due to component
 * circular dependency.
 */
extern int shr_sysm_process_instance(sysm_instance_t *p_instance);

/*!
 * \brief Obtain the maximal component ID
 *
 * This function returns the system maximal valid component ID
 *
 * \return the maximal component ID
 */
extern uint32_t shr_sysm_get_max_component_id(void);

/*!
 * \brief Obtain the total number of instances
 *
 * This function returns the total number of system manager instances
 * (from all categories) in the system
 *
 * \return the total number of instances
 */
extern uint32_t shr_sysm_get_instance_count(void);

/*!
 * \brief Terminate the assisting thread(s)
 *
 * This function terminates the assisting thread(s) and waits until
 * the tread(s) had exited its main loop. The function makes sure
 * that the thread loop condition will fail by modifying the value of
 * running to false. It than also make sure that the thread is not blocked
 * on its message queue by posting an empty job.
 * The thread itself cleans up all its queue and other resources
 *
 * \param [in] running is a pointer that shared with the assisting thread(s)
 *
 * \return none
 */
extern void shr_sysm_assist_thread_terminate(bool *running);

/*!
 * \brief System manager instance thread
 *
 * This is the instance main thread function.
 *
 * \param [in] arg is a cast pointer to the instance associated with this
 * thread
 *
 * \return none
 */
extern void shr_sysm_instance_thread(void *arg);

/*!
 * \brief System manager assisting thread
 *
 * This function process a single component in an instance
 *
 * \param [in] arg is a pointer cast to the thread running condition. This
 * enables external signaling for thread termination.
 *
 * \return none
 */
extern void shr_sysm_assist_thread(void *arg);

#endif /* SYSM_INTERNAL_H */
