/*! \file sysm_local.c
 *
 * Broadcom System manager back-end implementation
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

#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <sal/sal_msgq.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_assert.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_sysm.h>

/* Header file with prototypes for public functions in this file */
#include "sysm_internal.h"

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_SHR_SYSM

#define SYSM_LOG_UNIT(_inst) \
    (((_inst)->type == SHR_SYSM_CAT_UNIT) ? (_inst)->unit : BSL_UNIT_UNKNOWN)

#define MAX_ASSISTANT   0  

static shr_lmm_hdl_t sysm_assist_req_mem_hdl;
static sal_msgq_t sysm_assist_q;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get name of system manager state.
 *
 * For invalid states the special string "?" will be returned.
 *
 * \param [in] state System manager state.
 *
 * \return ASCII string.
 */
static const char *state_name(shr_sysm_states_t state)
{
    switch (state) {
    case SHR_SYSM_INIT:
        return "init";
    case SHR_SYSM_COMP_CONFIG:
        return "comp-config";
    case SHR_SYSM_PRE_CONFIG:
        return "pre-config";
    case SHR_SYSM_RUN:
        return "run";
    case SHR_SYSM_STOP:
        return "stop";
    case SHR_SYSM_SHUTDOWN:
        return "shutdown";
    default:
        break;
    }
    return "?";
}

/*!
 * \brief Move blocked component to active list
 *
 * This function move components from the block array entry (based on
 * component_id) to the active list. The dependent array contains pointers to
 * blocked components. The array arranged according to the blocking component
 * ID, so component that is blocked by component K will be pointed by the
 * K'th entry of the array
 *
 * \param [in] component_id Blocking component ID.
 * \param [in] p_instance System manager instance.
 *
 * \return Nothing.
 */
static void move_blocked_to_active(uint32_t component_id,
                                   sysm_instance_t *p_instance)
{
    sysm_component_instance_t *p_active_component =
                                    p_instance->l_component_active;

    if (!p_active_component) {
        p_instance->l_component_active =
                p_instance->dependencies[component_id];
    } else {
        /* Find the last component in the active list */
        while (p_active_component->next) {
            p_active_component = p_active_component->next;
        }
        /* Add the blocked components to the end of the list */
        p_active_component->next = p_instance->dependencies[component_id];
    }
    /* Check how many in the blocked list */
    p_active_component = p_instance->dependencies[component_id];
    while (p_active_component) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance),
                                 p_active_component->component_db->component_id,
                                 "Component %"PRId32" unblocked "
                                 "from component %"PRId32"\n"),
                     p_active_component->component_db->component_id,
                     component_id));
        p_instance->component_active_count++;
        p_active_component = p_active_component->next;
    }
    /* Clear the dependecy list */
    p_instance->dependencies[component_id] = NULL;
}

/*!
 * \brief Prepare the active list
 *
 * This function scans the complete component list and extract all the
 * components that have action function for the current state of the instance
 * The extracted component moved into the active list. The active list counter
 * and the component proc counts are counting the number of extracted
 * components.
 * Also the complete array being reset
 *
 * \param [in] instance point to the system manager instance
 *
 * \return none
 */
static void sysm_prep_active_list(sysm_instance_t *instance)
{
    sysm_component_instance_t *component_instance =
                instance->l_component_complete;
    uint32_t comp_id;

    /*
     * p_prev_component_instance pointing to the previous component in the
     * component complete list
     */
    sysm_component_instance_t *p_prev_component_instance = component_instance;
    bool first_element = true;

    /* Mark everything as complete */
    sal_memset(instance->completed_array, 1, shr_sysm_get_max_component_id());
    while (component_instance) {
        comp_id = component_instance->component_db->component_id;
        if (instance->state == SHR_SYSM_RUN) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UX(SYSM_LOG_UNIT(instance), comp_id,
                                  "Component %"PRId32" started\n"),
                      comp_id));
        } else if (instance->state == SHR_SYSM_SHUTDOWN) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UX(SYSM_LOG_UNIT(instance), comp_id,
                                  "Component %"PRId32" shut down\n"),
                      comp_id));
        }
        /* if the component has a callback in this state */
        if (component_instance->component_db->cb_vector[instance->state]) {
            /* Disconnect from the component complete list */
            if (first_element) {
                instance->l_component_complete = component_instance->next;
                p_prev_component_instance = component_instance->next;
            } else {
                p_prev_component_instance->next = component_instance->next;
            }
            /* Add to the active list */
            component_instance->next = instance->l_component_active;
            instance->l_component_active = component_instance;
            if (first_element) {
                component_instance = p_prev_component_instance;
            } else {
                component_instance = p_prev_component_instance->next;
            }
            instance->component_proc_count++;
            /* Active component, mark it incomplete */
            instance->completed_array[comp_id] = 0;
        } else {
            first_element = false;
            p_prev_component_instance = component_instance;
            component_instance = p_prev_component_instance->next;
        }
    }
    instance->component_active_count = instance->component_proc_count;
}

/*!
 * \brief Check if any blocked component can be unblocked
 *
 * This function scans the complete component list and check if any blocked
 * components are blocking on the completed component. Typically, this function
 * being called when the active list is empty but more components needs to be
 * processed
 *
 * \param [in] p_instance point to the system manager instance
 *
 * \return true if a component moved into the active list
 */
static bool check_blocked(sysm_instance_t *p_instance)
{
    int component_id;
    sysm_component_instance_t *p_component_instance =
            p_instance->l_component_complete;

    /*
     * Find the first scenario (there shoudl be one) where a completed
     * component is blocking another component. This is possible if one
     * component is blocked by more than one components.
     */
    while (p_component_instance) {
        component_id = p_component_instance->component_db->component_id;
        if (p_instance->dependencies[component_id]) {
            move_blocked_to_active(component_id, p_instance);
            return true;
        }
        p_component_instance = p_component_instance->next;
    }
    return false;
}

/*!
 * \brief Makes the component callback
 *
 * This function makes the call to the component registered function. It
 * validates the return code and blocking component ID (when relevant)
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] component_db is a pointer to the component registered callback
 * functions
 * \param [out] blocking_component is a pointer where the blocking component
 * ID should be placed (when the component is blocked)
 *
 * \return SHR_SYSM_RV_ERROR when error, otherwise success
 */
static shr_sysm_rv_t sysm_invoke_comp_cb(sysm_instance_t *p_instance,
                             sysm_component_t *component_db,
                             uint32_t *blocking_component)
{
    shr_sysm_rv_t rv;
    uint32_t component_id = component_db->component_id;

    if (p_instance->state == SHR_SYSM_RUN) {
        shr_sysm_run_cb cb = (shr_sysm_run_cb)component_db->cb_vector[SHR_SYSM_RUN];
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                 "Component %"PRId32" callback (%s)\n"),
                     component_id, state_name(p_instance->state)));
        cb(p_instance->type, p_instance->unit, component_db->comp_data);
        rv = SHR_SYSM_RV_DONE;
    } else {
        shr_sysm_cb cb = component_db->cb_vector[p_instance->state];
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                 "Component %"PRId32" callback (%s)\n"),
                     component_id, state_name(p_instance->state)));
        rv = cb(p_instance->type,
                p_instance->unit,
                component_db->comp_data,
                p_instance->warm_start,
                blocking_component);
        assert(rv >= SHR_SYSM_RV_BLOCKED);
        assert(rv <= SHR_SYSM_RV_ERROR);
        /* Check for error condition */
        if (rv == SHR_SYSM_RV_BLOCKED) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                     "Component %"PRId32" blocked "
                                     "by component %"PRId32"\n"),
                         component_id, *blocking_component));
            if (*blocking_component > shr_sysm_get_max_component_id()) {
                rv = SHR_SYSM_RV_ERROR;
            }
        }
        if (rv == SHR_SYSM_RV_ERROR) {
            
            LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(SYSM_LOG_UNIT(p_instance),
                                    "Component %"PRId32" returned error "
                                    "at phase %s\n"),
                         component_id, state_name(p_instance->state)));
        }
    }
    return rv;
}

/*!
 * \brief Process result from component callback
 *
 * This function takes the proper action as a result of component callback
 * function processing. If a component is blocked the function places the
 * component in the blocking array at that index that matches the blocking
 * component ID. If the component has done it will be placed in the completed
 * list. For done components the function is examining the blocking array
 * for blocked components. If blocked components were found the function
 * moves them into the active list
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] p_component_instance is a pointer to the component instance
 * \param [in] res is the returned value from the component callback
 * \param [in] blocking_id is the blocking component ID in case the res
 * is SHR_SYSM_RV_BLOCKED
 *
 * \return 0 on success and -1 otherwise.
 */
static int sysm_process_result(
                        sysm_instance_t *p_instance,
                        sysm_component_instance_t *p_component_instance,
                        shr_sysm_rv_t res,
                        uint32_t blocking_id)
{
    int component_id;

    if (res == SHR_SYSM_RV_ERROR) {
        return -1;
    }
    if (res == SHR_SYSM_RV_BLOCKED) {
        /*
         * Add the component to the list of dependencies for this blocking
         * component ID. This is unsorted linked list.
         */
        p_component_instance->next =
            p_instance->dependencies[blocking_id];
        p_instance->dependencies[blocking_id] = p_component_instance;
        if (p_instance->l_component_active == NULL) {
            /* Check if any of the completed components can free a blocked component */
            check_blocked(p_instance);
        }
    } else {  /* Component done */
        assert(res == SHR_SYSM_RV_DONE); /* Sanity */
        component_id = p_component_instance->component_db->component_id;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                 "Component %"PRId32" done state %s\n"),
                     component_id, state_name(p_instance->state)));
        /* Add component to the complete list */
        p_component_instance->next = p_instance->l_component_complete;
        p_instance->l_component_complete = p_component_instance;
        /* Mark the component complete array */
        p_instance->completed_array[component_id] = 1;
        /* Check if any component was blocked by this component */
        if (p_instance->dependencies[component_id]) {
            move_blocked_to_active(component_id, p_instance);
            assert(p_instance->l_component_active != NULL);
        }
        /* Check if need to refresh state */
        if (--p_instance->component_proc_count == 0) {
            p_component_instance = shr_sysm_prep_for_next_state(p_instance, true);
        }
    }
    return 0;
}

/*!
 * \brief Handles assistance thread completion
 *
 * This function being called by the assistence thread when it completes
 * the processing of a given job. The information encapsulated in the
 * input parameter. It is placed in the assist list of the instance to
 * be processed later by the dedicated thread
 *
 * \param [in] assisted_req is a structure point to the request to processed
 * by the assisting thread.
 *
 * \return none
 */
static void assist_complete(sysm_assist_job_t *assisted_req)
{
    sal_mutex_take(assisted_req->p_instance->assisted_mutex,
                   SAL_MUTEX_FOREVER);
    assisted_req->next = assisted_req->p_instance->l_assisted;
    assisted_req->p_instance->l_assisted = assisted_req;
    sal_mutex_give(assisted_req->p_instance->assisted_mutex);
}

/*!
 * \brief Process results generated by the assistence threads
 *
 * This function being called by the dedicated instance thread to process
 * the results posted by the assistence thread(s). The result list from
 * the assisting thread (\c l_assisted_res) may contain more than a single
 * result. It is important for the caller to determine how many results
 * were in this list as it needs to know the current load of the assisting
 * thread.
 *
 * \param [in] l_assisted_res is the assistence result list
 * \param [out] processed Returned the number of components that were
 * in the result list.
 *
 * \return 0 If no error. Error code otherwise.
 */
#if MAX_ASSISTANT
static int process_result_list(sysm_assist_job_t *l_assisted_res,
                               int *processed)
{
    void *tmp;
    int rv = 0;

    *processed = 0;
    while (l_assisted_res) {
        if (rv == 0) {
            rv = sysm_process_result(
                            l_assisted_res->p_instance,
                            l_assisted_res->p_component_instance,
                            l_assisted_res->rv,
                            l_assisted_res->blocking_id);
        }
        tmp = (void *)l_assisted_res;
        l_assisted_res = l_assisted_res->next;
        shr_lmm_free(sysm_assist_req_mem_hdl, tmp);
        (*processed)++;
    }

    return rv;
}
#endif
/*!
 * \brief Send request to the assisting thread(s)
 *
 * This function post a job to the assisting thread
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] p_component_instance points to the component that should be
 * processed by the assisting thread
 *
 * \return number of posted jobs
 */
#if MAX_ASSISTANT
static int send_assist_req(sysm_instance_t *p_instance,
                           sysm_component_instance_t *p_component_instance)
{
    sysm_assist_job_t *l_assisted_req;
    int rv;

    l_assisted_req = shr_lmm_alloc(sysm_assist_req_mem_hdl);
    if (!l_assisted_req) {
        return 0;
    }
    l_assisted_req->p_component_instance = p_component_instance;
    l_assisted_req->p_instance = p_instance;
    p_instance->component_active_count--;
    rv = sal_msgq_post(sysm_assist_q,
                       (void *)&l_assisted_req,
                       SAL_MSGQ_NORMAL_PRIORITY,
                       SAL_MSGQ_FOREVER);
    if (rv != 0) {
        shr_lmm_free(sysm_assist_req_mem_hdl, l_assisted_req);
        return 0;
    }
    return 1;
}
#endif
static void sysm_generate_error_report(sysm_instance_t *p_instance)
{
    uint32_t j;
    sysm_component_instance_t *p_comp;

    LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(SYSM_LOG_UNIT(p_instance),
                              "Component cyclic dependency\n")));
    for (j = 0; j < shr_sysm_get_max_component_id(); j++) {
        if (p_instance->dependencies[j]) {
            p_comp = p_instance->dependencies[j];
            while (p_comp) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Component %"PRId32" blocked on "
                                    "component %"PRId32"\n"),
                           p_comp->component_db->component_id, j));
                p_comp = p_comp->next;
            }
        }
    }
}

/*!
 * \brief System manager process component
 *
 * This function process a single component in an instance
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] p_component_instance points to the component that should be
 * processed by the assisting thread
 *
 * \return none
 */
static int sysm_process_component(sysm_instance_t *p_instance,
                             sysm_component_instance_t *p_component_instance)
{
    uint32_t blocking_component;
    shr_sysm_rv_t rv;

    rv = sysm_invoke_comp_cb(p_instance,
                           p_component_instance->component_db,
                           &blocking_component);
    p_instance->component_active_count--;
    rv = sysm_process_result(p_instance,
                             p_component_instance,
                             rv,
                             blocking_component);
    return rv;
}

/*******************************************************************************
 * Public functions
 */
int shr_sysm_process_instance(sysm_instance_t *p_instance)
{
    sysm_component_instance_t *p_component_instance;
    int rv = SHR_E_NONE;

    sal_mutex_take(p_instance->instance_mutex, SAL_MUTEX_FOREVER);
    while ((p_instance->component_proc_count > 0) &&
           (p_instance->steps_left > 0)) {
        /* Make sure that there is an active component waiting */
        if (!p_instance->l_component_active) {
            /* if not, check if any blocked component can be unblocked */
            if (!check_blocked(p_instance)) {
                /*
                 * there is nothing to do while there are still component
                 * to process. Therefore this is a component dependency deadlock.
                 * report that and stop.
                 */
                sysm_generate_error_report(p_instance);
                /* stop this instance from executing */
                p_instance->steps_left = 0;
                rv = SHR_E_RESOURCE;
                break;
            }
        }
        /* First extract the component out from the active list */
        p_component_instance = p_instance->l_component_active;
        p_instance->l_component_active = p_component_instance->next;
        p_instance->steps_left--;
        sysm_process_component(p_instance, p_component_instance);
    }
    sal_mutex_give(p_instance->instance_mutex);
    return rv;
}

sysm_component_instance_t *shr_sysm_prep_for_next_state(
                                    sysm_instance_t *p_instance,
                                    bool auto_mode)
{
    bool next_state;

    p_instance->l_component_active = NULL;
    do {
        next_state = true;
        switch (p_instance->state) {
        case SHR_SYSM_INIT:
            p_instance->state = SHR_SYSM_COMP_CONFIG;
            sysm_prep_active_list(p_instance);
            break;
        case SHR_SYSM_COMP_CONFIG:
            p_instance->state = SHR_SYSM_PRE_CONFIG;
            sysm_prep_active_list(p_instance);
            break;
        case SHR_SYSM_PRE_CONFIG:
            p_instance->state = SHR_SYSM_RUN;
            sysm_prep_active_list(p_instance);
            break;
        case SHR_SYSM_RUN:
            if (auto_mode) {
                if (p_instance->state_chng_func) {
                    p_instance->state_chng_func(p_instance->type,
                                                p_instance->unit,
                                                SHR_E_NONE);
                    p_instance->state_chng_func = NULL;
                }
                next_state = false;
            } else {
                p_instance->state = SHR_SYSM_STOP;
                sysm_prep_active_list(p_instance);
            }
            break;
        case SHR_SYSM_STOP:
            p_instance->state = SHR_SYSM_SHUTDOWN;
            sysm_prep_active_list(p_instance);
            break;
        case SHR_SYSM_SHUTDOWN:
            if (p_instance->state_chng_func) {
                p_instance->state_chng_func(p_instance->type,
                                            p_instance->unit,
                                            SHR_E_NONE);
                p_instance->state_chng_func = NULL;
            }
            next_state = false;
            break;
        default:
            assert(0);
        }
    /*
     *  Automatically continue to the next state if the active component list
     *  is empty
     */
    } while (next_state && !p_instance->l_component_active);

    return p_instance->l_component_active;
}

static void report_error_chg(sysm_instance_t *instance)
{
    if (instance->state_chng_func) {
        instance->state_chng_func(instance->type,
                                  instance->unit,
                                  SHR_E_FAIL);
        instance->state_chng_func = NULL;
    }
}

/*
 * Process all the pending results performed by assistance threads.
 * The assistant thread places processed jobs on the assisted
 * list of the instance (instance->l_assisted). This function is being
 * called by the main (dedicated) thread for the unit.
*/
#if MAX_ASSISTANT
static int proc_pending_res(sysm_instance_t *instance, int *processed)
{
    sysm_assist_job_t *l_assisted_res;

    sal_mutex_take(instance->assisted_mutex, SAL_MUTEX_FOREVER);
    l_assisted_res = instance->l_assisted;
    instance->l_assisted = NULL;
    sal_mutex_give(instance->assisted_mutex);
    /* now process the assisted response list */
    return process_result_list(l_assisted_res, processed);
}
#endif
void shr_sysm_instance_thread(void *arg)
{
    sysm_instance_t *instance = (sysm_instance_t *)arg;
    sysm_component_instance_t *p_component_instance;
    int rv;
#if MAX_ASSISTANT
    int assist_count = 0;
    int processed;
#endif
    while (instance->running) {
        sal_mutex_take(instance->instance_mutex, SAL_MUTEX_FOREVER);
        while ((instance->component_proc_count > 0) &&
               (instance->steps_left > 0)) {
#if MAX_ASSISTANT
            /* Check if there are any pending results */
            if (assist_count && instance->l_assisted) {
                rv = proc_pending_res(instance, &processed);
                assist_count -= processed;
                if (rv < 0) {  /* If something failed */
                    /* Stop this instance from executing */
                    instance->steps_left = 0;
                    report_error_chg(instance);
                    break;    /* Break out of the instance processing */
                }
                continue;
            }
#endif
            /*
             * Make sure that the active list is not empty. First check that
             * nothing became unblocked. If the list is still empty it might
             * be due to pending assistent thread request. In this case wait
             * for it to complete
             */
            if (!instance->l_component_active) {
#if MAX_ASSISTANT
                if (!check_blocked(instance) && assist_count) {
                    sal_thread_yield();
                    continue;
                }
#else
                check_blocked(instance);
#endif
            }
            if (!instance->l_component_active) {
                /*
                 * There is nothing to do while there are still
                 * components to process, and therefore this is a
                 * component dependency deadlock. Report that and
                 * stop.
                 */
                sysm_generate_error_report(instance);
                /* Stop this instance from executing */
                instance->steps_left = 0;
                report_error_chg(instance);
                break;
            }
            assert(instance->l_component_active != NULL);
            /* Extract the component out from the active list */
            p_component_instance = instance->l_component_active;
            instance->l_component_active = p_component_instance->next;
            instance->steps_left--;

            rv = 0;
#if MAX_ASSISTANT
            /* Check if can use the assistant thread */
            if (assist_count < MAX_ASSISTANT &&
                instance->component_active_count > 2) {
                if (send_assist_req(instance, p_component_instance) == 0) {
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(SYSM_LOG_UNIT(instance),
                                         "Out of thread resources\n")));
                    /* Process this normally */
                    rv = sysm_process_component(instance, p_component_instance);
                } else {
                    assist_count++;
                }
            } else {  /* If not process from this thread */
                rv = sysm_process_component(instance, p_component_instance);
            }
#else
            rv = sysm_process_component(instance, p_component_instance);
#endif
            /* Check if the block above had critical failure */
            if (rv < 0) {
                report_error_chg(instance);
                instance->steps_left = 0;
#if MAX_ASSISTANT
                assist_count = 0;
#endif
                break;
            }
        }
#if MAX_ASSISTANT
        /* Wait until all assisted work had been done */
        while (assist_count > 0) {
            if (instance->l_assisted) {
                if ((rv = proc_pending_res(instance, &processed)) < 0) {
                    /* Stop this instance from executing */
                    instance->steps_left = 0;
                    report_error_chg(instance);
                }
                assist_count -= processed;
            }
            /* Give the assist thread chance to finish */
            if (assist_count > 0) {
                sal_usleep(100);
            }
        }
#endif
        sal_mutex_give(instance->instance_mutex);
        sal_sem_take(instance->event_sem, SAL_SEM_FOREVER);
    }
    instance->running = true;
}

void shr_sysm_assist_thread_terminate(bool *running)
{
    sysm_assist_job_t exit_msg;
    int j = 0;

    /* Send a message to the assist thread to exit */
    exit_msg.p_component_instance = NULL;
    exit_msg.p_instance = NULL;
    *running = false;
    sal_msgq_post(sysm_assist_q,
                  &exit_msg,
                  SAL_MSGQ_HIGH_PRIORITY,
                  SAL_MSGQ_FOREVER);
    /* Wait for the assist thread to exit (pooling) */
    while (!(*running) && (j++ < 1000)) {
        sal_usleep(1000);
    }
}

void shr_sysm_assist_thread(void *arg)
{
    bool *sysm_running = (bool *)arg;
    int rv;
    sysm_assist_job_t *q_element;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("Create assist thread\n")));

    sysm_assist_q = sal_msgq_create(sizeof(sysm_assist_job_t *),
                                    shr_sysm_get_instance_count() * 2,
                                    "");
    if (!sysm_assist_q) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("message Q create failed\n")));
        return;
    }
    LMEM_MGR_INIT(sysm_assist_job_t,
                  sysm_assist_req_mem_hdl,
                  16,
                  true,
                  rv);
    if (rv != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Memory allocation failed\n")));
        return;
    }
    while (*sysm_running) {
        rv = sal_msgq_recv(sysm_assist_q, (void *)&q_element, SAL_MSGQ_FOREVER);
        if (!(*sysm_running)) {
            break;
        }
        if (rv != 0) {
            
            break;
        }
        q_element->rv = sysm_invoke_comp_cb(
            q_element->p_instance,
            q_element->p_component_instance->component_db,
            &q_element->blocking_id);
        assist_complete(q_element);
    }
    /* Free the queue and assist memory pool */
    sal_msgq_destroy(sysm_assist_q);
    shr_lmm_delete(sysm_assist_req_mem_hdl);

    *sysm_running = true;    /* this indicates exiting */
}
