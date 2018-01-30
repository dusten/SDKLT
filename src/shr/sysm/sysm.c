/*! \file sysm.c
 *
 * <description>
 * Broadcom System manager API
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

/* header files required to build this file */
#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <sal/sal_msgq.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_sysm.h>

/* header file with prototypes for public functions in this file */
#include "sysm_internal.h"

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_SHR_SYSM

#define VALIDATE_INSTANCE_INPUT_PARAMS(_category,_unit) \
    if ((_category >= SHR_SYSM_CAT_COUNT) || (_category < 0)){\
        LOG_WARN(BSL_LOG_MODULE,                            \
                (BSL_META_U(_unit,                          \
                         "Invalid category\n")));           \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                   \
    }                                                       \
    if ((_unit >= (int)sysm_instance_db[_category].array_size) ||\
        (_unit < 0)) {                                      \
        LOG_WARN(BSL_LOG_MODULE,                            \
                (BSL_META_U(_unit,                          \
                         "Invalid unit\n")));               \
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);                    \
    }

#define VALIDATE_INSTANCE(_instance_cat, _unit, _p_instance)  \
    if (!sysm_instance_db[_instance_cat].instance_array) {    \
        LOG_WARN(BSL_LOG_MODULE,                              \
                (BSL_META_U(_unit,                            \
                         "The system was not initialized properly\n"))); \
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);                  \
    }                                                         \
    _p_instance = sysm_instance_db[_instance_cat].instance_array[_unit]; \
    if (!_p_instance) {                                       \
        LOG_WARN(BSL_LOG_MODULE,                              \
                (BSL_META_U(_unit,                            \
                         "instance does not exist\n")));      \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                     \
    }

typedef struct {
    sysm_component_t *l_comp;
    uint32_t comp_count;
} sysm_category_db_t;
static sysm_category_db_t sysm_db[SHR_SYSM_CAT_COUNT];

typedef struct {
    sysm_instance_t **instance_array;
    size_t array_size;
} sysm_instances_db_t;

static sysm_instances_db_t sysm_instance_db[SHR_SYSM_CAT_COUNT];

static enum {INTERNAL, EXTERNAL} sysm_thread_mode;
static uint32_t sysm_max_num_of_comp;
static sal_mutex_t sysm_mutex;
static bool sysm_running;
static uint32_t sysm_instance_count;

/*******************************************************************************
 * Private functions
 */

static int allocate_component_list(sysm_instance_t *p_instance,
                                   shr_sysm_categories_t category)
{
    uint32_t j;
    sysm_component_instance_t *p_component_instance = NULL;
    sysm_component_t *p_component;
    size_t alloc_size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    /* create the list of components */
    alloc_size = sizeof(sysm_component_instance_t) *
                sysm_db[category].comp_count;
    SHR_ALLOC(p_component_instance,
              alloc_size,
              "");
    SHR_NULL_CHECK(p_component_instance, SHR_E_MEMORY);
    sal_memset(p_component_instance, 0, alloc_size);
    /* save the pointer so we can free later */
    p_instance->allocated_component_mem = p_component_instance;

    p_instance->completed_array = NULL;
    SHR_ALLOC(p_instance->completed_array, sysm_max_num_of_comp, "");
    SHR_NULL_CHECK(p_instance->completed_array, SHR_E_MEMORY);
    /*
     * Everything marked as complete so we will not get stuck on components
     * that are not there.
     */
    sal_memset(p_instance->completed_array, 1, sysm_max_num_of_comp);

    for (j = 0, p_component = sysm_db[category].l_comp;
          j < sysm_db[category].comp_count;
          p_component = p_component->next, j++, p_component_instance++) {
        p_component_instance->component_db = p_component;
        /* count the number of components to process in init state */
        if (p_component->cb_vector[SHR_SYSM_INIT]) {
            p_instance->component_proc_count++;
            /* add this to the active list */
            p_component_instance->next = p_instance->l_component_active;
            p_instance->l_component_active = p_component_instance;
            p_instance->completed_array[p_component->component_id] = 0;
        } else {
            p_component_instance->next = p_instance->l_component_complete;
            p_instance->l_component_complete = p_component_instance;
        }
    }
    p_instance->component_active_count = p_instance->component_proc_count;
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int shr_sysm_category_define(shr_sysm_categories_t category,
                            uint32_t max_num_of_instances)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if ((category >= SHR_SYSM_CAT_COUNT) || (category < 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    sysm_instance_db[category].array_size = max_num_of_instances;
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_init(uint32_t max_component_id, bool internal_thread_mode)
{
    int j;
    bool allocated = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (internal_thread_mode) {
        sysm_thread_mode = INTERNAL;
    } else {
        sysm_thread_mode = EXTERNAL;
    }
    sysm_max_num_of_comp = max_component_id;
    sysm_mutex = sal_mutex_create("sys_mgr");
    SHR_NULL_CHECK(sysm_mutex, SHR_E_MEMORY);

    for (j = 0; j < SHR_SYSM_CAT_COUNT; j++) {
        size_t alloc_size;
        sysm_instances_db_t *idb = &sysm_instance_db[j];
        if (idb->array_size == 0) {
            continue;
        }
        allocated = true;
        alloc_size = sizeof(sysm_instance_t *) * idb->array_size;
        SHR_ALLOC(idb->instance_array, alloc_size, "");
        SHR_NULL_CHECK(idb->instance_array, SHR_E_MEMORY);
        sal_memset(idb->instance_array, 0, alloc_size);
        sysm_instance_count += idb->array_size;
    }
    if (!allocated) {
        SHR_RETURN_VAL_EXIT(SHR_E_DISABLED);
    }
    /* Create assistance threads in internal threading mode */
    if (internal_thread_mode) {
        sysm_running = true;
        /*
         * Disable for now to avoid multi threads within interactive path
        sal_thread_create("SHR_SYSM_ASSIST",
                       SAL_THREAD_STKSZ * 2,
                       SAL_THREAD_PRIO_DEFAULT,
                       shr_sysm_assist_thread,
                       &sysm_running);
       */
    }
exit:
    if (SHR_FUNC_ERR()) {
        for (j = 0; j < SHR_SYSM_CAT_COUNT; j++) {
            SHR_FREE(sysm_instance_db[j].instance_array);
        }
        if (sysm_mutex) {
            sal_mutex_destroy(sysm_mutex);
            sysm_mutex = NULL;
        }
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META("System manager initialized\n")));
    }

    SHR_FUNC_EXIT();
}

int shr_sysm_delete(void)
{
    int j;
    sysm_component_t *p_component;
    sysm_component_t *tmp;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* First stop the assist threads */
    /*
     * Disable for now to avoid multi threads within interactive path
    if (sysm_thread_mode == INTERNAL) {
        shr_sysm_assist_thread_terminate(&sysm_running);
    }
    */

/* Clean the component DB */
    for (j = 0; j < SHR_SYSM_CAT_COUNT; j++) {
        p_component = sysm_db[j].l_comp;
        while (p_component) {
            tmp = p_component;
            p_component = p_component->next;
            SHR_FREE(tmp);
        }
        sal_memset(&sysm_db[j], 0, sizeof(sysm_db[0]));
        SHR_FREE(sysm_instance_db[j].instance_array);
    }

    /* No need to protect the component DB anymore. Delete the guard */
    sal_mutex_destroy(sysm_mutex);
    sysm_mutex = NULL;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("System manager cleaned up\n")));

    SHR_FUNC_EXIT();
}

int shr_sysm_register(uint32_t comp_id,
                      shr_sysm_categories_t instance_cat,
                      shr_sysm_cb_func_set_t *cb_set,
                      void *comp_data)
{
    sysm_component_t *p_component;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_UX(BSL_UNIT_UNKNOWN, comp_id,
                             "Register component %"PRId32"\n"), comp_id));

    if ((comp_id > sysm_max_num_of_comp) ||
        (instance_cat >= SHR_SYSM_CAT_COUNT) ||
        (instance_cat < 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(sysm_mutex, SAL_MUTEX_FOREVER));

    do {
        /* Make sure component is not registered already */
        p_component = sysm_db[instance_cat].l_comp;
        while (p_component) {
            if (p_component->component_id == comp_id) {
                rv = SHR_E_EXISTS;
                break;
            }
            p_component = p_component->next;
        }
        if (rv != SHR_E_NONE) {
            SHR_RETURN_VAL_EXIT(rv);
        }

        /* allocate new component structure */
        SHR_ALLOC(p_component, sizeof(sysm_component_t), "");
        SHR_NULL_CHECK(p_component, SHR_E_MEMORY);
        p_component->component_id = comp_id;
        p_component->cb_vector[SHR_SYSM_INIT] = cb_set->init;
        p_component->cb_vector[SHR_SYSM_COMP_CONFIG] = cb_set->comp_config;
        p_component->cb_vector[SHR_SYSM_PRE_CONFIG] = cb_set->pre_config;
        p_component->cb_vector[SHR_SYSM_RUN] = (shr_sysm_cb)cb_set->run;
        p_component->cb_vector[SHR_SYSM_STOP] = cb_set->stop;
        p_component->cb_vector[SHR_SYSM_SHUTDOWN] = cb_set->shutdown;
        p_component->comp_data = comp_data;
        /* Add the component into the component list */
        p_component->next = sysm_db[instance_cat].l_comp;
        sysm_db[instance_cat].l_comp = p_component;
        sysm_db[instance_cat].comp_count++;
    } while (0);

    sal_mutex_give(sysm_mutex);

exit:
    SHR_FUNC_EXIT();
}

bool shr_sysm_is_comp_complete(uint32_t comp_id,
                                   shr_sysm_categories_t instance_cat,
                                   int unit)
{
    sysm_instance_t *p_instance;

    do {
        /* Validation section, return false for invalid parameters */
        if ((instance_cat >= SHR_SYSM_CAT_COUNT) ||
            (instance_cat < 0)) {
             LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("component looking for invalid category %d\n"),
                      instance_cat));
            break;
        }
        if ((unit >= (int)sysm_instance_db[instance_cat].array_size) ||
            (unit < 0)) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("component looking for invalid unit %d\n"),
                     unit));
            break;
        }
        p_instance = sysm_instance_db[instance_cat].instance_array[unit];
        if (!p_instance) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("component looking for wrong category %d unit "
                               "%d combination\n"),
                     instance_cat, unit));
            break;
        }
        if (comp_id >= sysm_max_num_of_comp) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("component checking for completion of invalid "
                              "component %d\n"),
                     comp_id));
            break;
        }
        /* body section */
        return p_instance->completed_array[comp_id] == 1;
    } while (0);
    /*
     * If here it is because sanity had failed. Still don't want
     * the component to get stuck, so return true
     */
    return true;
}

int shr_sysm_instance_new(shr_sysm_categories_t instance_cat,
                         int unit,
                         bool warm,
                         bool single_step,
                         sysm_instance_state_chng_cb func)
{
    sysm_instance_t *p_instance = NULL;
    sysm_component_t *p_component;

    SHR_FUNC_ENTER(unit);

    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    p_component = sysm_db[instance_cat].l_comp;
    if (!p_component) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                             "No components associated with the category\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    /* allocate the instance */
    p_instance = NULL;
    SHR_ALLOC(p_instance, sizeof(sysm_instance_t), "SHR_SYSM_INSTANCE");
    SHR_NULL_CHECK(p_instance, SHR_E_MEMORY);
    sal_memset(p_instance, 0, sizeof(sysm_instance_t));

    SHR_IF_ERR_EXIT(allocate_component_list(p_instance, instance_cat));

    /* allocate array for blocked components */
    SHR_ALLOC(p_instance->dependencies,
              sizeof(sysm_component_instance_t *) * sysm_max_num_of_comp,
              "");
    SHR_NULL_CHECK(p_instance->dependencies, SHR_E_MEMORY);
    sal_memset(p_instance->dependencies,
               0,
               sizeof(sysm_component_instance_t *) * sysm_max_num_of_comp);

    /* set other instance variables */
    p_instance->warm_start = warm;
    if (single_step) {
        p_instance->steps_left = 0;
    } else {
        p_instance->steps_left = 0xFFFFFFFF;    /* maximum steps */
    }
    p_instance->state = SHR_SYSM_INIT;
    p_instance->running = true;
    p_instance->l_component_complete = NULL;
    p_instance->type = instance_cat;
    p_instance->unit = unit;
    p_instance->state_chng_func = func;
    p_instance->instance_mutex = sal_mutex_create("SHR_SYSM_INST");
    SHR_NULL_CHECK(p_instance->instance_mutex, SHR_E_MEMORY);

    sysm_instance_db[instance_cat].instance_array[unit] = p_instance;
    /* based on threading mode starts the local thread */
    if (sysm_thread_mode == EXTERNAL) {
        p_instance->thread_hdl = NULL;
        p_instance->event_sem = NULL;
    } else {
        p_instance->assisted_mutex = sal_mutex_create("SHR_SYSM_INST_ASSIST");
        SHR_NULL_CHECK(p_instance->assisted_mutex, SHR_E_MEMORY);
        p_instance->event_sem = sal_sem_create("SHR_SYSM_INST", SAL_SEM_BINARY, 0);
        SHR_NULL_CHECK(p_instance->event_sem, SHR_E_MEMORY);
        /*
         * thread create must be the last operation since once it been created
         * thread switch might occur.
         */
        p_instance->thread_hdl = sal_thread_create("SHR_SYSM_INST",
                                                   SAL_THREAD_STKSZ * 2,
                                                   SAL_THREAD_PRIO_DEFAULT,
                                                   shr_sysm_instance_thread,
                                                   (void *)p_instance);
        SHR_NULL_CHECK(p_instance->thread_hdl, SHR_E_MEMORY);
    }
exit:
    if (SHR_FUNC_ERR() && p_instance) {
        if (p_instance->event_sem) {
            sal_sem_destroy(p_instance->event_sem);
        }
        if (p_instance->assisted_mutex) {
            sal_mutex_destroy(p_instance->assisted_mutex);
        }
        if (p_instance->instance_mutex) {
            sal_mutex_destroy(p_instance->instance_mutex);
        }
        SHR_FREE(p_instance->dependencies);
        SHR_FREE(p_instance->allocated_component_mem);
        SHR_FREE(p_instance);
    }
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_delete(shr_sysm_categories_t instance_cat,
                            int unit)
{
    sysm_instance_t *p_instance;
    int attempts = 0;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, p_instance);

    p_instance->steps_left = 0; /* stop the instance thread */
    /* remove the instance from the instances DB */
    sysm_instance_db[instance_cat].instance_array[unit] = NULL;

    /* Now start tearing everything down */
    /* Start with shutting down the thread instance */
    if (sysm_thread_mode == INTERNAL) {
        /*
         * The protocol is that the thread continues to run until its
         * instance is not in running anymore. The thread exits its
         * thread loop when p_instance->running is false. Before the thread
         * function exits, it sets p_instance->running to true to indicate
         * that it is on its way out.
         * Note that the thread instance will not exit its  internal loop as
         * long as there is pending work for the assistant thread on this
         * instance.
         */
        p_instance->running = false;
        sal_sem_give(p_instance->event_sem);
        while ((!p_instance->running) && (attempts++ < 1000)) {
            sal_usleep(1000);
        }
        if (!p_instance->running) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "instance thread didn't complete\n")));
        }
        sal_sem_destroy(p_instance->event_sem);
    }

    /* Wait for all active components to complete */
    attempts = 0;
    while (p_instance->component_active_count && (attempts++ < 1000)) {
        sal_usleep(100);
    }
    if (p_instance->component_active_count > 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }

    sal_mutex_destroy(p_instance->instance_mutex);
    sal_mutex_destroy(p_instance->assisted_mutex);
    SHR_FREE(p_instance->dependencies);
    SHR_FREE(p_instance->completed_array);
    SHR_FREE(p_instance->allocated_component_mem);
    SHR_FREE(p_instance);
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_stop(shr_sysm_categories_t instance_cat,
                          int unit,
                          bool graceful,
                          sysm_instance_state_chng_cb func)
{
    sysm_instance_t *p_instance;
    sysm_component_instance_t *active_comp;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, p_instance);

    if ((p_instance->state != SHR_SYSM_RUN) ||
        (p_instance->component_proc_count > 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }
    /* Sanity passed, start the operation */
    SHR_IF_ERR_EXIT(sal_mutex_take(p_instance->instance_mutex,
                                   SAL_MUTEX_FOREVER));
    p_instance->state_chng_func = func;
    active_comp = shr_sysm_prep_for_next_state(p_instance, false);
    sal_mutex_give(p_instance->instance_mutex);
    /* If there is nothing to do in stop */
    if (active_comp == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    if (sysm_thread_mode == INTERNAL) {
        /* let the thread process the instance */
        sal_sem_give(p_instance->event_sem);
    }
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_step(shr_sysm_categories_t instance_cat,
                          int unit,
                          uint32_t number_of_steps)
{
    sysm_instance_t *p_instance;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, p_instance);

    if (p_instance->steps_left > 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }
    p_instance->steps_left = number_of_steps;
    if (number_of_steps == 0xFFFFFFFF) {    /* back to normal execution? */
        if (sysm_thread_mode == INTERNAL) {   /* kick start the internal thread */
            sal_sem_give(p_instance->event_sem);
        }
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_EXIT(shr_sysm_process_instance(p_instance));
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_process(shr_sysm_categories_t instance_cat,
                             int unit)
{
    sysm_instance_t *p_instance;

    SHR_FUNC_ENTER(unit);
    if (sysm_thread_mode == INTERNAL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, p_instance);

    SHR_IF_ERR_EXIT(shr_sysm_process_instance(p_instance));
exit:
    SHR_FUNC_EXIT();
}

/* none exported functions */
uint32_t shr_sysm_get_max_component_id (void)
{
    return sysm_max_num_of_comp;
}

uint32_t shr_sysm_get_instance_count(void)
{
    return sysm_instance_count;
}
