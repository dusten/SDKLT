/*! \file tr_mgr_nb.c
 *
 * Transaction manager north bound (NB) API.
 * This module implements the NB API for the transaction manager
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
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>
#include <shr/shr_ha.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmltm/bcmltm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmcfg/comp/bcmcfg_trm_resources.h>
#include <bcmtrm/trm_api.h>
#include <bcmtrm/generated/bcmtrm_ha.h>
#include "trm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMTRM_ENTRY
#define ONE_SEC_WAIT    1000000

/* TRM HA memory sub ID */
#define TRM_HA_SUB  0

/* TRM HA memory sub for interactive table */
#define TRM_INTERACT_HA_SUB 1

#define TRM_HA_SIGNATURE    0x7bd78a34

#define TRM_MIN_INTERACT_FIELD_CNT  128

#define TRM_INVALID_HA_IDX  0xFFFFFFFF

#define MAX_PENDING_TRANSACTIONS 256

/*
 * The default number for maximal entries structures to create per unit. This
 * configurable parameter can be used to restrict the memory consumption used by
 * the entry containers.
 */
#define DEFAULT_MAX_ACTIVE_ENTRIES  512

/*
 * The default number of the maximal transaction structures to create per unit.
 * This configurable parameter can be used to restrict the memory consumption
 * used by the transactin containers.
 */
#define DEFAULT_MAX_ACTIVE_TRANSACTIONS 128
/*
 * Internal data structures.
 */
typedef void (*element_free_f)(void *hdl);

typedef struct trans_instruct_s {
    enum {ENTRY_OP, TRANSACTION_OP, EXIT} type;
    void *element;
} trans_instruct_t;

typedef struct trm_sync_object_s {
    sal_sem_t sem;
    struct trm_sync_object_s *next;
} trm_sync_object_t;


typedef struct {
    sal_msgq_t model_queue;  /* Modeled message Q */
    sal_msgq_t interact_queue; /* Interactive message Q */
    sal_mutex_t mtx;    /* mutex for the unit - see more details below */
    bool initialized;   /* true if the unit had been initialized */
    bool terminate;     /* indicate if the thread should terminate */
    int unit;           /* the unit associated with this resource */
    int thread_stopped; /* counts how many threads had stopped */
    bool stop_api;      /* indicates that the unit is in stop state */
    uint32_t op_id;     /* Operation ID - increment number */
    uint32_t trans_id;  /* Transaction ID */
    trn_info_t *ha_mem; /* HA memory pointer */
    trn_interact_info_t *inter_ha_mem; /* HA memory for interactive tables */
    uint32_t active_sync_trans; /* Counts the active synchronous transactions */
} trm_unit_resources_t;

/*
 * Global variables to the TRM module.
 */
sal_msgq_t bcmtrm_hw_notif_q;

bcmtrm_trans_state_t bcmtrm_trans_ha_state;

shr_lmm_hdl_t bcmtrm_ltm_entry_hdl;

/*
 * Local variables.
 */

/*
 * this variable contains all the resources that are associated with a specific
 * unit. It enables us to control and clean up all the resources associated
 * with a particular unit. Special field is the mtx.
 * (mtx) mutexes used in the case where a specific transaction contains entries
 * for multiple units. In this case a thread that is going to access unit that
 * is not his, will have to grant access to another unit by grabing the mutex
 * associated with the desired unit. Note also that the operation ID will have
 * to be coordinated between the various threads. To do this, the MS byte will
 * be the value of the local unit
 */
static trm_unit_resources_t unit_resources[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Local memory manager handle contains elements of mutual exclusion objects.
 * The objects ate used to protect the transaction
 * internal data structure when changed by multiple threads.
 */
static shr_lmm_hdl_t ltm_prot_obj_hdl;

static shr_lmm_hdl_t ltm_sync_hdl;

static shr_lmm_hdl_t entry_mem_hdl;

static shr_lmm_hdl_t trans_mem_hdl;

static trm_sync_object_t *sync_obj_free_list;
static sal_mutex_t sync_obj_free_list_mtx;

/*
 * This mutex being used to modify an entry state. In particular  async
 * entries state can be changed via multiple threads.
 */
static sal_mutex_t entry_state_mtx;

/* count the number of running notification thread */
static int notify_thread_running = 0;

/*
 * Keep lists of committed entries and transactions. Since it is a global
 * resource needs to define mutexs to protect them.
 */
static sal_mutex_t active_entry_mutex;
static sal_mutex_t active_trans_mutex;
static bcmtrm_entry_t *active_entry_list;
static bcmtrm_trans_t *active_trans_list;

/* Indicates if atomic operation is enabled or disabled. */
static bool atomic_disabled;

static int alloc_lock_obj(bcmtrm_trans_t *trans)
{
    trans->lock_obj = shr_lmm_alloc(ltm_prot_obj_hdl);
    if (!trans->lock_obj) {
        return SHR_E_MEMORY;
    }
    if (!trans->lock_obj->mutex_avail) {
        trans->lock_obj->mutex = sal_mutex_create("");
        if (!trans->lock_obj->mutex) {
            return SHR_E_MEMORY;
        }
        trans->lock_obj->mutex_avail = true;
    }
    return SHR_E_NONE;
}

/*
 * Drain the given queue and free the calling thread that posted the request
 * into the queue.
 */
static void queue_drain(sal_msgq_t q)
{
    trans_instruct_t q_element;

    /* Need to clean up all the elements in the queue */
    while (sal_msgq_recv(q, &q_element, 0) == SHR_E_NONE) {
        if (q_element.type == ENTRY_OP) {
            bcmtrm_appl_entry_inform((bcmtrm_entry_t *)q_element.element,
                                     SHR_E_DISABLED,
                                     BCMLT_NOTIF_OPTION_HW);
        } else if (q_element.type == TRANSACTION_OP) {
            bcmtrn_trans_cb_and_clean((bcmtrm_trans_t *)q_element.element,
                                      BCMLT_NOTIF_OPTION_HW,
                                      SHR_E_DISABLED);
        }
    }
}

sal_mutex_t bcmtrm_unit_mutex_get(int unit)
{
    return unit_resources[unit].mtx;
}

sal_mutex_t bcmtrm_entry_mutex_get(void)
{
    return entry_state_mtx;
}

/*
 * Post an entry into the interactive queue. This function only called from
 * processing a regular (non-PT) transaction
 */
void bcmtrm_q_interactive_entry(bcmtrm_entry_t *entry,
                                bcmlt_priority_level_t priority)
{
    trans_instruct_t q_element;
    sal_msgq_priority_t pri;
    int unit = entry->info.unit;

    if (priority == BCMLT_PRIORITY_NORMAL) {
        pri = SAL_MSGQ_NORMAL_PRIORITY;
    }  else {
        pri = SAL_MSGQ_HIGH_PRIORITY;
    }
    entry->state = E_COMMITTING;
    q_element.element = (void *)entry;
    q_element.type = ENTRY_OP;
    sal_msgq_post(unit_resources[unit].interact_queue,
                  &q_element,
                  pri,
                  SAL_MSGQ_FOREVER);
}

static void interactive_unit_thread(void *arg) {
    trm_unit_resources_t *unit_res = (trm_unit_resources_t * )arg;
    trans_instruct_t q_element;
    int rv;
    uint32_t op_id = 0;
    bool running = true;

    while (running) {
        rv = sal_msgq_recv(unit_res->interact_queue,
                           (void *)&q_element,
                           ONE_SEC_WAIT);
        if (unit_res->terminate) {
            break;      /* the thread has to exit */
        }
        if (rv == SAL_MSGQ_E_TIMEOUT) { /* no action */
            continue;
        }
        if (rv != 0) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit_res->unit,
                         "Failed to receive interactive message rv=%d\n"), rv));
            break;     /* add some log message as this should not happen */
        }
        switch (q_element.type) {
        case ENTRY_OP:
            bcmtrm_process_interactive_entry_req(
                                (bcmtrm_entry_t *)q_element.element,
                                op_id,
                                unit_res->inter_ha_mem);
            bcmtrm_entry_complete((bcmtrm_entry_t *)q_element.element);
            break;
        case TRANSACTION_OP:
            rv = bcmtrm_handle_interactive_transaction(
                                    (bcmtrm_trans_t * )q_element.element,
                                    &op_id,
                                    unit_res->inter_ha_mem);
            if (rv != SHR_E_NONE) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit_res->unit,
                                     "Inter trans failed op_id=%d error=%d\n"),
                          op_id, rv));
            }
            bcmtrm_trans_complete((bcmtrm_trans_t *)q_element.element);
            break;
        case EXIT:
            queue_drain(unit_res->interact_queue);
            /* Fall through */
        default:
            running = false;
            break;
        }
    }
    sal_mutex_take(unit_res->mtx, SAL_MUTEX_FOREVER);
    unit_res->thread_stopped++;
    sal_mutex_give(unit_res->mtx);
}

/*
 * This thread is running per unit. It receives requests into its queue and
 * process them. In general the expectation is that every transaction has
 * two stages:
 * 1. Setup - where the entry(ies) operation being perform and committed
 * 2. H/W callback - that happens once the h/w completed its work
 * Unfortunately, we can't rely on the order of things as in some
 * cases the h/w callback occurs synchronously from within this thread.
 * Furthermore, this thread can be delayed and some h/w operation might
 * be very fast. The reason we need things to be in order is that we
 * are calling the application callback from here. We would like to make
 * the application callback in order (i.e. HW option should come after
 * COMMIT). Fortunately, there can be only single outstanding operation
 * that should be synchronized in order. So with careful planing we can
 * reserve a single structure that can be filled only when some pending
 * even has to be queued (to keep the callback order).
 * An object (entry or transaction) will have state. It will be in COMMITTING
 * state until after the first callback occurs (BCMLT_NOTIF_OPTION_COMMIT)
 * If the h/w operation completed while in COMMITTING state it will be queued
 * Otherwise it will make the proper notification to the application. The trick
 * is to avoid scenario where the h/w notification wants to queue the event but
 * the modeled_unit_thread was too fast and didn't find anything in the special
 * queue. One way to resolve it is to make the h/w callback thread to be in
 * higher priority, but this will not work well in Linux. The other option is
 * to minimize the exposure of task switch by making the h/w notification
 * function signaling the modeled_unit_thread() immediately. None of the
 * options is bullet proof so the solution is to have a "queue" of up to two
 * entries served at the highest priority.
*/
static void modeled_unit_thread(void *arg)
{
    trm_unit_resources_t *unit_res = (trm_unit_resources_t *)arg;
    trans_instruct_t q_element;
    int rv;
    bool running = true;

    while (running) {
        rv = sal_msgq_recv(unit_res->model_queue,
                           (void *)&q_element,
                           ONE_SEC_WAIT);
        if (unit_res->terminate) {
            break;      /* the thread has to exit */
        }
        if (rv == SAL_MSGQ_E_TIMEOUT) {
            continue;
        }
        if (rv != 0) {
            LOG_WARN(BSL_LOG_MODULE,
             (BSL_META_U(unit_res->unit,
                         "Failed to receive model message rv=%d\n"),
                         rv));
            break;
        }
        switch (q_element.type) {
        case ENTRY_OP:
            sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
            bcmtrm_process_entry_req((bcmtrm_entry_t *)q_element.element,
                                     &unit_res->op_id,
                                     &unit_res->trans_id);
            sal_mutex_give(unit_res->mtx);
            break;
        case TRANSACTION_OP:
            rv = bcmtrm_handle_transaction((bcmtrm_trans_t *)q_element.element,
                                           &unit_res->op_id,
                                           &unit_res->trans_id,
                                           NULL);
            if (rv != SHR_E_NONE) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit_res->unit,
                                     "Trans failed trans_id=%d error=%d\n"),
                          unit_res->trans_id, rv));
            }
            break;
        case EXIT:
            queue_drain(unit_res->model_queue);
            /* Fall through */
        default:
            running = false;
            break;
        }
    }
    sal_mutex_take(unit_res->mtx, SAL_MUTEX_FOREVER);
    unit_res->thread_stopped++;
    sal_mutex_give(unit_res->mtx);
}

/*
 * This function manages a list of available semaphores. The semaphores
 * are a field in a structure that contains the semaphore and a pointer
 * to the next element. The free list is global for all units. If the
 * free list is not empty the function takes the element at the top of the
 * list and return it. Otherwise, the function allocates a new element and
 * a new semaphore for it. The newly allocated element is returned.
 */
static trm_sync_object_t *sync_obj_alloc(void)
{
    trm_sync_object_t *sync_obj = NULL;

    sal_mutex_take(sync_obj_free_list_mtx, SAL_MUTEX_FOREVER);
    if (sync_obj_free_list) {
        sync_obj = sync_obj_free_list;
        sync_obj_free_list = sync_obj->next;
    }
    sal_mutex_give(sync_obj_free_list_mtx);
    if (sync_obj) {
        return sync_obj;
    }

    sync_obj = shr_lmm_alloc(ltm_sync_hdl);
    if (!sync_obj) {
        return NULL;
    }

    sync_obj->sem = sal_sem_create("TRM", SAL_SEM_BINARY, 0);
    if (!sync_obj->sem) {
        return NULL;
    }

    return sync_obj;
}

/*
 * Free sync element back into the free list. The element will be placed
 * at the begining of the free list. It is assumed that the element was used
 * so the next user will block when attempting to take it.
 */
static void sync_obj_free(trm_sync_object_t *sync_obj)
{
    sal_mutex_take(sync_obj_free_list_mtx, SAL_MUTEX_FOREVER);
    sync_obj->next = sync_obj_free_list;
    sync_obj_free_list = sync_obj;
    sal_mutex_give(sync_obj_free_list_mtx);
}

/*
 * The evolution of a state is first allocating it via the state_set function
 * followed by state_update and last by state_done. The state_cancel only
 * being used for error handling.
 * It is important to notice few rules:
 * 1. The state_set always being followed by state_cancel or state_update for
 * the same trans_id.
 * 2. state_done can be called at any time. However, it is being called in the
 * order it was created (using the state_set). It means that transaction order
 * is maintained from the creation (using the set) to completion (using the
 * done).
 */

/*!
 * \brief Allocate new transaction state entry and set it to the given state.
 *
 * This function saves the transaction ID and state in HA memory dedicated for
 * this.
 *
 * \param [in] unit Is the unit associated with the transaction.
 * \param [in] state Is the state of the transaction.
 * \param [in] trans_id Is the transaction ID.
 * \param [out] hdl Is the handle of the entry that should be used for future
 * reference to this transaction.
 *
 * \retval None.
 */
static void trans_state_set(int unit,
                            uint32_t state,
                            uint32_t trans_id,
                            uint32_t *hdl)
{
    trm_unit_resources_t *unit_res = &unit_resources[unit];
    trn_info_t *ha_mem = unit_res->ha_mem;
    uint32_t idx = ha_mem->last_idx;
    int delay_count = 0;
    int array_cnt;

    /*
     * Restrict the number of active transactions to make sure there are
     * sufficient resources to track them. Waiting in this loop should be
     * rare event. The entry count will be decremented by the WAL reader
     * thread upon completion of a transaction.
     */
    while (ha_mem->entry_cnt == BCMTRM_MAX_COMMITTED_TRANS &&
           delay_count++ < 1000) {
        sal_usleep(100);
    }
    if (ha_mem->entry_cnt == BCMTRM_MAX_COMMITTED_TRANS) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                    "TRM states are full. HA functionality might fail\n")));
        *hdl = TRM_INVALID_HA_IDX;
        return;
    }

    sal_mutex_take(ha_mem->sync_obj, SAL_MUTEX_FOREVER);
    array_cnt = 0;
    do {
        if (ha_mem->trn_state[idx].state == BCMTRM_STATE_NONE) {
            break;
        }
        idx = (idx + 1) % BCMTRM_MAX_COMMITTED_TRANS;
    } while (array_cnt++ < BCMTRM_MAX_COMMITTED_TRANS);

    assert(array_cnt < BCMTRM_MAX_COMMITTED_TRANS);

    ha_mem->trn_state[idx].state = state;
    ha_mem->trn_state[idx].trn_id = trans_id;
    ha_mem->last_idx = (idx + 1) % BCMTRM_MAX_COMMITTED_TRANS;
    ha_mem->entry_cnt++;
    *hdl = idx;
    sal_mutex_give(ha_mem->sync_obj);
}

/*!
 * \brief Free the transaction state.
 *
 * This function being called when the staging or commit operation had failed.
 * The function returns the entry state, that was allocated by the function
 * \ref bcmtrm_trans_state_set(), to the free list. This function is the inverse
 * function of \ref bcmtrm_trans_state_set().
 *
 * \param [in] unit Is the unit associated with the transaction.
 * \param [in] trans_id Indicate the transactin ID to cancel.
 * \param [in] hdl Is the handle of the entry as it was provided by the
 * function \ref bcmtrm_trans_state_set.
 *
 * \retval none.
 */
static void trans_state_cancel(int unit, uint32_t trans_id, uint32_t hdl)
{
    trm_unit_resources_t *unit_res = &unit_resources[unit];
    trn_info_t *ha_mem = unit_res->ha_mem;

    if (hdl >= BCMTRM_MAX_COMMITTED_TRANS) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Invalid handle %u trans=%u\n"),
                  hdl, trans_id));
        return;
    }

    sal_mutex_take(ha_mem->sync_obj, SAL_MUTEX_FOREVER);

    if (ha_mem->trn_state[hdl].trn_id == trans_id && ha_mem->entry_cnt > 0) {
        ha_mem->entry_cnt--;
        ha_mem->trn_state[hdl].state = BCMTRM_STATE_NONE;
        ha_mem->trn_state[hdl].trn_id = 0;
        ha_mem->last_idx = hdl;
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Failed to find TRM state trans_id=%u\n"),
                             trans_id));
    }

    sal_mutex_give(ha_mem->sync_obj);
}

/*!
 * \brief Update transaction state.
 *
 * This function updates the state of a transaction that was initially set
 * using the function \ref bcmtrm_trans_state_set(). Typically, this function
 * is being called just prior to the commit phase.
 *
 * \param [in] unit Is the unit associated with the transaction.
 * \param [in] state Is the new state of the transaction.
 * \param [in] trans_id Is the transaction ID to update its state.
 * \param [in] hdl Is the handle of the entry as it was provided by the
 * function \ref bcmtrm_trans_state_set.
 *
 * \retval None.
 */
static void trans_state_update(int unit,
                               uint32_t state,
                               uint32_t trans_id,
                               uint32_t hdl)
{
    trm_unit_resources_t *unit_res = &unit_resources[unit];
    trn_info_t *ha_mem = unit_res->ha_mem;

    if (hdl >= BCMTRM_MAX_COMMITTED_TRANS) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Invalid handle %u trans=%u\n"),
                  hdl, trans_id));
        return;
    }

    sal_mutex_take(ha_mem->sync_obj, SAL_MUTEX_FOREVER);
    if (ha_mem->trn_state[hdl].trn_id == trans_id && ha_mem->entry_cnt > 0) {
        ha_mem->trn_state[hdl].state = state;
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Failed to find TRM state trans_id=%u\n"),
                             trans_id));
    }
    sal_mutex_give(ha_mem->sync_obj);
}

/*!
 * \brief Release the transaction information from HA memory.
 *
 * This function being called once the transaction had been to completion.
 * The transaction is then freed from being tracked by HA memory.
 *
 * \param [in] unit Is the unit associated with the transaction.
 * \param [in] trans_id Is the transaction ID of the transaction.
 * \param [in] hdl Is the handle of the entry as it was provided by the
 * function \ref bcmtrm_trans_state_set.
 *
 * \retval none.
 */
static void trans_state_done(int unit, uint32_t trans_id, uint32_t hdl)
{
    trm_unit_resources_t *unit_res = &unit_resources[unit];
    trn_info_t *ha_mem = unit_res->ha_mem;

    if (hdl >= BCMTRM_MAX_COMMITTED_TRANS) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Invalid handle %u trans=%u\n"),
                  hdl, trans_id));
        return;
    }

    sal_mutex_take(ha_mem->sync_obj, SAL_MUTEX_FOREVER);

    if (ha_mem->trn_state[hdl].trn_id == trans_id && ha_mem->entry_cnt > 0) {
        ha_mem->trn_state[hdl].state = BCMTRM_STATE_NONE;
        ha_mem->trn_state[hdl].trn_id = 0;
        ha_mem->entry_cnt--;
        ha_mem->start_idx = (hdl + 1) % BCMTRM_MAX_COMMITTED_TRANS;
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Failed to find TRM state trans_id=%u\n"),
                             trans_id));
    }

    sal_mutex_give(ha_mem->sync_obj);
}

/*
 * The following are dummy functions to be used when atomic operation and HA
 * are disabled.
 */
static void dummy_trans_state_set(int unit,
                                  uint32_t state,
                                  uint32_t trans_id,
                                  uint32_t *hdl)
{
    return;
}

static void dummy_trans_state_cancel(int unit, uint32_t trans_id, uint32_t hdl)
{
    return;
}

static void dummy_trans_state_update(int unit,
                                     uint32_t state,
                                     uint32_t trans_id,
                                     uint32_t hdl)
{
    return;
}

static void dummy_trans_state_done(int unit, uint32_t trans_id, uint32_t hdl)
{
    return;
}

/*================================================================*
 *     C O M P O N E N T    I N T E R N A L   F U N C T I O N S   *
 *================================================================*/
bcmltm_field_list_t *bcmtrm_local_field_alloc(void) {
    return (bcmltm_field_list_t *)shr_fmm_alloc();
}

void bcmtrm_set_trans_entries_state(bcmtrm_trans_t *trans, int state)
{
    bcmtrm_entry_t *entry_it;
    for (entry_it = trans->l_entries;
          entry_it;
          entry_it = entry_it->next) {
        entry_it->state = state;
    }
}

int bcmtrm_stop(int unit, bool graceful)
{
    trans_instruct_t q_element;
    int rc;
    int j;
    sal_msgq_priority_t pri;
    trm_unit_resources_t *unit_res = &unit_resources[unit];
    int timeout;

    unit_res->thread_stopped = 0;
    unit_res->stop_api = true;

    if (graceful) {
        pri = SAL_MSGQ_NORMAL_PRIORITY;
        timeout = 2000;
    } else {
        bcmltm_notify_units[unit] = false;
        unit_res->terminate = true;
        pri = SAL_MSGQ_HIGH_PRIORITY;
        timeout = 1000;
    }

    q_element.type = EXIT;
    /* Post exit events for both threads */
    rc = sal_msgq_post(unit_res->model_queue,
                       &q_element,
                       pri,
                       SAL_MSGQ_FOREVER);
    if (0 != rc) {
        return SHR_E_INTERNAL;
    }
    rc = sal_msgq_post(unit_res->interact_queue,
                       &q_element,
                       pri,
                       SAL_MSGQ_FOREVER);
    if (0 != rc) {
        return SHR_E_INTERNAL;
    }

    /* Wait for the threads to exit until timeout */
    for (j = 0; j < timeout; j++) {
        if (unit_res->thread_stopped == 2) {
            break;
        }
        sal_usleep(1000);
    }
    if (j >= timeout) {
        return SHR_E_TIMEOUT;
    }
    /* Need to check that all synchronous operations had been completed */
    j = 0;
    timeout = 1000;
    do {
        if (unit_res->active_sync_trans == 0) {
            break;
        }
        sal_usleep(1000);
    } while (j++ < timeout);
    if (j >= timeout) {
        return SHR_E_TIMEOUT;
    }

    return SHR_E_NONE;
}

int bcmtrm_recover(int unit)
{
    int rv = SHR_E_NONE;
    trm_unit_resources_t *unit_res = &unit_resources[unit];
    trn_info_t *ha_mem = unit_res->ha_mem;
    uint32_t idx = ha_mem->start_idx;
    uint32_t inter_blk_size = sizeof(trn_interact_field_t) *
                                TRM_MIN_INTERACT_FIELD_CNT;

    if (atomic_disabled) {
        return SHR_E_NONE;
    }

    while (idx != ha_mem->last_idx && rv == SHR_E_NONE) {
        rv = bcmtrm_recover_trans(unit, &ha_mem->trn_state[idx]);
        sal_memset(&ha_mem->trn_state[idx], 0, sizeof(ha_mem->trn_state[0]));
        idx  = (idx + 1) % BCMTRM_MAX_COMMITTED_TRANS;
        ha_mem->start_idx = idx;
        ha_mem->entry_cnt--;
    }

    /* Make sure that the entry count is 0 */
    ha_mem->entry_cnt = 0;

    /*
     * Now recover interactive operations. These operations must be repeated.
     * Start by allocating the HA block. No need for exact size.
     */
    unit_res->inter_ha_mem = shr_ha_mem_alloc(unit, BCMMGMT_TRM_COMP_ID,
                                              TRM_INTERACT_HA_SUB,
                                              &inter_blk_size);
    if (!unit_res->inter_ha_mem) {
        return SHR_E_MEMORY;
    }
    if (unit_res->inter_ha_mem->signature == TRM_HA_SIGNATURE) {
        bcmtrm_recover_interact(unit, unit_res->inter_ha_mem);
    }

    return rv;
}

/*================================================================*
 *     C O M P O N E N T    E X T E R N A L   F U N C T I O N S   *
 *================================================================*/
int bcmtrm_init(void)
{
#define ENTRY_ALLOC_CHUNKS  64
#define ASYNC_ELEMENT_CHUNK_SIZE   5
#define LTM_ENTRY_CHUNK_SIZE       64
#define TRANS_ALLOC_CHUNKS  32
    int rv;
    const bcmcfg_feature_ctl_config_t *feature_conf;
    const bcmcfg_trm_resources_config_t *trm_conf;
    uint32_t use_val;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!ltm_sync_hdl) {
        /* allocate chunks of 5 element at a time */
        LMEM_MGR_INIT(trm_sync_object_t,
                      ltm_sync_hdl,
                      ASYNC_ELEMENT_CHUNK_SIZE,
                      true,
                      rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
    }

    trm_conf = bcmcfg_trm_resources_config_get();
    if (!bcmtrm_ltm_entry_hdl) {
        LMEM_MGR_INIT(bcmltm_entry_t,
                           bcmtrm_ltm_entry_hdl,
                           LTM_ENTRY_CHUNK_SIZE,
                           true,
                           rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
    }
    if (!trans_mem_hdl) {
        if (trm_conf == NULL || trm_conf->max_transactions == 0) {
            use_val = DEFAULT_MAX_ACTIVE_TRANSACTIONS;
        } else {
            use_val = trm_conf->max_transactions;
        }

        LMEM_MGR_REST_INIT(bcmtrm_trans_t,
                           trans_mem_hdl,
                           TRANS_ALLOC_CHUNKS,
                           true,
                           use_val,
                           rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
    }

    if (!entry_mem_hdl) {
        if (trm_conf == NULL || trm_conf->max_entries == 0) {
            use_val = DEFAULT_MAX_ACTIVE_ENTRIES;
        } else {
            use_val = trm_conf->max_entries;
        }

        LMEM_MGR_REST_INIT(bcmtrm_entry_t,
                           entry_mem_hdl,
                           ENTRY_ALLOC_CHUNKS,
                           true,
                           use_val,
                           rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
    }

    if (!ltm_prot_obj_hdl) {
        LMEM_MGR_INIT(bcmtrm_data_protect_object_t,
                      ltm_prot_obj_hdl,
                      ASYNC_ELEMENT_CHUNK_SIZE,
                      true,
                      rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
    }
    active_entry_mutex = sal_mutex_create("trnActEnt");
    active_trans_mutex = sal_mutex_create("trnActTrans");
    sync_obj_free_list_mtx = sal_mutex_create("trnSyncObjList");
    entry_state_mtx = sal_mutex_create("trnEntryStateMtx");

    active_entry_list = NULL;
    active_trans_list = NULL;

    bcmevm_register_cb(bcmtrm_sb_table_notify);
    if (1 /*internal_thread_mode*/) {
        sal_thread_t t_hdl;
        bcmtrm_hw_notif_q = sal_msgq_create(sizeof(bcmtrm_hw_notif_struct_t),
                                    MAX_PENDING_TRANSACTIONS,
                                    "");

        t_hdl = sal_thread_create("",
                                  2 * SAL_THREAD_STKSZ,
                                  50,
                                  bcmtrm_notify_thread,
                                  &notify_thread_running);
        if (!t_hdl) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }
    }

    feature_conf = bcmcfg_feature_ctl_config_get();
    if (feature_conf == NULL || feature_conf->dis_stomic_trans == 0) {
        bcmtrm_trans_ha_state.set_f = trans_state_set;
        bcmtrm_trans_ha_state.cancel_f = trans_state_cancel;
        bcmtrm_trans_ha_state.update_f = trans_state_update;
        bcmtrm_trans_ha_state.done_f = trans_state_done;
        atomic_disabled = false;
    } else {  /* Dummy functions */
        bcmtrm_trans_ha_state.set_f = dummy_trans_state_set;
        bcmtrm_trans_ha_state.cancel_f = dummy_trans_state_cancel;
        bcmtrm_trans_ha_state.update_f = dummy_trans_state_update;
        bcmtrm_trans_ha_state.done_f = dummy_trans_state_done;
        atomic_disabled = true;
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmtrm_delete()
{
    bcmtrm_hw_notif_struct_t hw_notif;
    int rc;
    int j;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    /* Wait until pending activity is completed */
    for (j = 0; j < 1000; j++) {
        if (active_entry_list || active_trans_list) {
            sal_usleep(100);
        }
    }
    /* Reject if committed entries exist */
    if (active_entry_list || active_trans_list) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }

    hw_notif.type = NOTIF_EXIT;
    rc = sal_msgq_post(bcmtrm_hw_notif_q,
                       &hw_notif,
                       SAL_MSGQ_HIGH_PRIORITY,
                       SAL_MSGQ_FOREVER);
    if (0 != rc) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    /* Wait for the threads to exit */
    for (j = 0; j < 1000; j++) {
        if (!notify_thread_running) {
            break;
        }
        sal_usleep(1000);
    }
    if (j >= 1000) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Notify thread failed to exit\n")));
    } else {
        sal_msgq_destroy(bcmtrm_hw_notif_q);
    }

    if (ltm_sync_hdl) {
        /* Free all the semaphores from the free list */
        while (sync_obj_free_list) {
            sal_sem_destroy(sync_obj_free_list->sem);
            sync_obj_free_list = sync_obj_free_list->next;
        }
        shr_lmm_delete(ltm_sync_hdl);
        ltm_sync_hdl = NULL;
        sal_mutex_destroy(sync_obj_free_list_mtx);
    }
    if (entry_mem_hdl) {
        shr_lmm_delete(entry_mem_hdl);
        entry_mem_hdl = NULL;
    }
    if (trans_mem_hdl) {
        shr_lmm_delete(trans_mem_hdl);
        trans_mem_hdl = NULL;
    }
    if (bcmtrm_ltm_entry_hdl) {
        shr_lmm_delete(bcmtrm_ltm_entry_hdl);
        bcmtrm_ltm_entry_hdl = NULL;
    }
    if (ltm_prot_obj_hdl) {
        bcmtrm_data_protect_object_t *p_prot_obj;
        p_prot_obj = shr_lmm_alloc(ltm_prot_obj_hdl);
        while (p_prot_obj && p_prot_obj->mutex_avail) {
            sal_mutex_destroy(p_prot_obj->mutex);
            p_prot_obj = shr_lmm_alloc(ltm_prot_obj_hdl);
        }
        shr_lmm_delete(ltm_prot_obj_hdl);
        ltm_prot_obj_hdl = NULL;
    }
    if (active_entry_mutex) {
        sal_mutex_destroy(active_entry_mutex);
        active_entry_mutex = NULL;
    }
    if (active_trans_mutex) {
        sal_mutex_destroy(active_trans_mutex);
        active_trans_mutex = NULL;
    }
    if (entry_state_mtx) {
        sal_mutex_destroy(entry_state_mtx);
        entry_state_mtx = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmtrm_unit_init(int unit, bool warm)
{
    sal_thread_t t_hdl;
    trm_unit_resources_t *unit_res = NULL;
    trn_info_t *ha_mem = NULL;
    uint32_t ha_blk_len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (unit >= BCMDRD_CONFIG_MAX_UNITS) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    bcmltm_notify_units[unit] = true;

    unit_res = &unit_resources[unit];
    sal_memset(unit_res, 0, sizeof(*unit_res));
    unit_res->model_queue =
        sal_msgq_create(sizeof(trans_instruct_t),
                        MAX_PENDING_TRANSACTIONS,
                        "");
    SHR_NULL_CHECK(unit_res->model_queue, SHR_E_MEMORY);

    unit_res->interact_queue =
        sal_msgq_create(sizeof(trans_instruct_t),
                         MAX_PENDING_TRANSACTIONS,
                         "");
    SHR_NULL_CHECK(unit_res->interact_queue, SHR_E_MEMORY);

    ha_blk_len = sizeof(trn_info_t);
    ha_mem = shr_ha_mem_alloc(unit, BCMMGMT_TRM_COMP_ID, TRM_HA_SUB,
                              &ha_blk_len);
    SHR_NULL_CHECK(ha_mem, SHR_E_MEMORY);

    if (!warm || ha_mem->signature != TRM_HA_SIGNATURE) {
        sal_memset(ha_mem, 0, sizeof(trn_info_t));
        ha_mem->signature = TRM_HA_SIGNATURE;
    }

    ha_mem->sync_obj = sal_mutex_create("trmHa");
    SHR_NULL_CHECK(ha_mem->sync_obj, SHR_E_MEMORY);
    unit_res->ha_mem = ha_mem;

    unit_res->mtx = sal_mutex_create("trmUnit");
    SHR_NULL_CHECK(unit_res->mtx, SHR_E_MEMORY);

    unit_res->unit = unit;
    unit_res->initialized = true;
    unit_res->terminate = false;
    unit_res->stop_api = false;
    INIT_OP_ID(unit_res->trans_id, unit, false);

    t_hdl = sal_thread_create("",
                              2 * SAL_THREAD_STKSZ,
                              50,
                              modeled_unit_thread,
                              (void *)unit_res);
    SHR_NULL_CHECK(t_hdl, SHR_E_MEMORY);

    t_hdl = sal_thread_create("",
                                2*SAL_THREAD_STKSZ,
                                50,
                                interactive_unit_thread,
                                (void *)unit_res);
    SHR_NULL_CHECK(t_hdl, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT(bcmtrm_table_event_alloc(unit));

exit:
    if (SHR_FUNC_ERR() && unit_res) {
        unit_res->initialized = true;
        if (ha_mem) {
            unit_res->ha_mem = ha_mem;
        }
        bcmtrm_unit_delete(unit);
    }
    SHR_FUNC_EXIT();
}

int bcmtrm_unit_delete(int unit)
{
    trm_unit_resources_t *unit_res;

    SHR_FUNC_ENTER(unit);

    if (unit >= BCMDRD_CONFIG_MAX_UNITS) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    unit_res = &unit_resources[unit];

    if (!unit_res->initialized) {
        return SHR_E_INIT;
    }

    /* Clean existing API resources */
    bcmtrm_clean_uncommitted(unit);

    /* Clean up resources */
    if (unit_res->model_queue) {
        sal_msgq_destroy(unit_res->model_queue);
    }
    if (unit_res->interact_queue) {
        sal_msgq_destroy(unit_res->interact_queue);
    }
    if (unit_res->mtx) {
        sal_mutex_destroy(unit_res->mtx);
    }

    if (unit_res->ha_mem && unit_res->ha_mem->sync_obj) {
        sal_mutex_destroy(unit_res->ha_mem->sync_obj);
        unit_resources[unit].ha_mem->sync_obj = NULL;
    }

    sal_memset(unit_res, 0, sizeof(*unit_res));

    bcmtrm_table_event_free(unit);

exit:
    SHR_FUNC_EXIT();
}

void bcmtrm_clean_uncommitted(int unit)
{
    bcmtrm_trans_t *trans;
    bcmtrm_trans_t *trans_it;
    bcmtrm_entry_t *entry;
    bcmtrm_entry_t *entry_it;

    /*
     * Start clearing the transactions as the transaction will also clear some
     * of the entries. We don't want to clear the entries from underneath the
     * transaction.
     * Transactions are in ACTIVE state before and after commit (i.e. after
     * commit was processed). The idea is to clear out the transactions
     * regardless if they are before or after the commit (as long as the commit
     * operation completed).
     */
    /* Take the lock to protect the transaction list */
    sal_mutex_take(active_trans_mutex, SAL_MUTEX_FOREVER);
    trans_it = active_trans_list;
    while (trans_it) {
        if (trans_it->unit == unit && trans_it->state == T_ACTIVE) {
            trans = trans_it;
            trans_it = trans_it->debug_next;
            bcmtrm_trans_done(trans, false);
        } else {
            trans_it = trans_it->debug_next;
        }
    }
    sal_mutex_give(active_trans_mutex);

    /* Take the lock to protect the entry list */
    sal_mutex_take(active_entry_mutex, SAL_MUTEX_FOREVER);
    entry_it = active_entry_list;
    while (entry_it) {
        if (entry_it->info.unit == unit && entry_it->state == E_ACTIVE) {
            entry = entry_it;
            entry_it = entry_it->debug_next;
            bcmtrm_entry_done(entry, false);
        } else {
            entry_it = entry_it->debug_next;
        }
    }
    sal_mutex_give(active_entry_mutex);
}

int bcmtrm_interactive_table_init(int unit, bool warm, uint32_t max_fields)
{
    trm_unit_resources_t *unit_res;
    trn_interact_info_t *ha_mem;
    uint32_t ha_blk_len = sizeof(trn_interact_info_t);

    SHR_FUNC_ENTER(unit);

    if (unit >= BCMDRD_CONFIG_MAX_UNITS) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    unit_res = &unit_resources[unit];

    /* Skip the HA allocation if atomic operation is disabled */
    if (atomic_disabled) {
        unit_res->inter_ha_mem = NULL;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Check if not already initialized */
    if (warm && unit_res->inter_ha_mem) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (max_fields < TRM_MIN_INTERACT_FIELD_CNT) {
        max_fields = TRM_MIN_INTERACT_FIELD_CNT;
    }
    ha_blk_len += sizeof(trn_interact_field_t) * max_fields;

    ha_mem = shr_ha_mem_alloc(unit, BCMMGMT_TRM_COMP_ID,
                              TRM_INTERACT_HA_SUB,
                              &ha_blk_len);
    SHR_NULL_CHECK(ha_mem, SHR_E_MEMORY);

    if (!warm || ha_mem->signature != TRM_HA_SIGNATURE) {
        /* Just clean up the block header */
        sal_memset(ha_mem, 0, sizeof(trn_interact_info_t));
        ha_mem->signature = TRM_HA_SIGNATURE;
        ha_mem->max_fields = max_fields;
    }

    unit_res->inter_ha_mem = ha_mem;

exit:
    SHR_FUNC_EXIT();
}

static void sync_trans_cb(bcmlt_notif_option_t event,
                          bcmlt_transaction_info_t *trans_info,
                          void *usr_data)
{
    sal_sem_give((sal_sem_t)usr_data);
}

int bcmtrm_asynch_trans_req(bcmtrm_trans_t *trans)
{
    int rc;
    trans_instruct_t q_element;
    sal_msgq_priority_t pri;

    if (unit_resources[trans->unit].stop_api) {
        return SHR_E_UNAVAIL;
    }
    bcmtrm_set_trans_entries_state(trans, E_COMMITTING);
    if (trans->priority == BCMLT_PRIORITY_NORMAL) {
        pri = SAL_MSGQ_NORMAL_PRIORITY;
    }  else {
        pri = SAL_MSGQ_HIGH_PRIORITY;
    }
    trans->syncronous = false;
    trans->state = T_COMMITTING;
    q_element.element = (void *)trans;
    q_element.type = TRANSACTION_OP;
    rc = alloc_lock_obj(trans);
    if (rc != SHR_E_NONE) {
        return rc;
    }

    /* Prepare the transaction internals */
    trans->info.status = SHR_E_NONE;
    trans->committed_entries = 0;
    trans->processed_entries = 0;
    trans->commit_success = 0;

    if (trans->pt_trans) {
        rc = sal_msgq_post(unit_resources[trans->unit].interact_queue,
                           &q_element,
                           pri,
                           SAL_MSGQ_FOREVER);
    } else {
        rc = sal_msgq_post(unit_resources[trans->unit].model_queue,
                           &q_element,
                           pri,
                           SAL_MSGQ_FOREVER);
    }
    return rc;
}

int bcmtrm_trans_req(bcmtrm_trans_t *trans)
{
    int rc;
    trm_sync_object_t *sync_obj;
    trans_instruct_t q_element;
    sal_msgq_priority_t pri;
    trm_unit_resources_t *unit_res = &unit_resources[trans->unit];

    if (unit_res->stop_api) {
        return SHR_E_UNAVAIL;
    }
    /* get a synch object */
    sync_obj = sync_obj_alloc();
    if (!sync_obj) {
        return SHR_E_MEMORY;
    }

    if (trans->priority == BCMLT_PRIORITY_NORMAL) {
        pri = SAL_MSGQ_NORMAL_PRIORITY;
    }  else {
        pri = SAL_MSGQ_HIGH_PRIORITY;
    }
    trans->info.notif_opt = BCMLT_NOTIF_OPTION_HW;
    trans->cb = sync_trans_cb;
    trans->usr_data = (void *)sync_obj->sem;
    trans->syncronous = true;
    trans->state = T_COMMITTING;
    trans->processed_entries = 0;
    rc = alloc_lock_obj(trans);
    if (rc != SHR_E_NONE) {
        return rc;
    }

    /* Increment the synchronous counter */
    sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
    unit_res->active_sync_trans++;
    sal_mutex_give(unit_res->mtx);

    /* Prepare the transaction internals */
    trans->info.status = SHR_E_NONE;
    trans->committed_entries = 0;
    trans->processed_entries = 0;
    trans->commit_success = 0;
    bcmtrm_set_trans_entries_state(trans, E_COMMITTING);

    if (trans->pt_trans) {
        /*
         * post the transaction into the transaction queue but first find the
         * relevan unit
        */
        q_element.element = (void *)trans;
        q_element.type = TRANSACTION_OP;
        rc = sal_msgq_post(unit_res->interact_queue,
                           &q_element,
                           pri,
                           SAL_MSGQ_FOREVER);
    } else {
        rc = bcmtrm_handle_transaction(trans,
                                       &unit_res->op_id,
                                       &unit_res->trans_id,
                                       sync_obj->sem);
    }
    /* Block until the operation is completed/failed */
    if (rc == SHR_E_NONE) {
        rc = sal_sem_take(sync_obj->sem, SAL_SEM_FOREVER);
    }

    sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
    unit_res->active_sync_trans--;
    sal_mutex_give(unit_res->mtx);

    /* Clean the user data field */
    trans->usr_data = NULL;

    sync_obj_free(sync_obj);

    if (rc != SHR_E_NONE) {
        return rc;
    }

    if (!trans->pt_trans) {
        bcmtrm_entry_t *entry = trans->l_entries;
        /* Process all the completed entries */
        while (entry) {
            if (entry->ltm_entry) {
                bcmtrm_proc_entry_results(entry, entry->ltm_entry);
            }

            /*
             * Inform the applicaiton about changed tables only for successful
             * operations.
             */
            if ((trans->info.type == BCMLT_TRANS_TYPE_BATCH ||
                 trans->info.status == SHR_E_NONE) &&
                 entry->info.status == SHR_E_NONE) {
                bcmtrm_tbl_chg_event(entry);
            }

            entry->state = E_ACTIVE;
            entry = entry->next;
        }
        trans->state = T_ACTIVE;
    }
    return SHR_E_NONE;
}

bcmtrm_entry_t *bcmtrm_entry_alloc(int unit,
                                   uint32_t tbl_id,
                                   bool interact,
                                   bool pt,
                                   shr_lmm_hdl_t fld_array_hdl,
                                   const char *tbl_name)
{
    bcmtrm_entry_t *entry;

    if (unit_resources[unit].stop_api) {
        return NULL;
    }
    entry = shr_lmm_alloc(entry_mem_hdl);
    if (!entry) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Maximum entries were allocated\n")));
        return NULL;
    }
    sal_memset(entry, 0, sizeof(*entry));
    entry->table_id = tbl_id;
    entry->interactive = interact;
    entry->pt = pt;
    entry->state = E_ACTIVE;
    entry->info.unit = unit;
    entry->info.status = SHR_E_NONE;
    entry->asynch = false;
    entry->info.table_name = tbl_name;
    entry->delete = false;

    if (fld_array_hdl) {
        entry->fld_arr = shr_lmm_alloc(fld_array_hdl);
        if (!entry->fld_arr) {
            shr_lmm_free(entry_mem_hdl, entry);
            return NULL;
        }
    }
    entry->fld_arr_mem_hdl = fld_array_hdl;

    ELEMENT_INSERT(active_entry_list,
                   active_entry_mutex,
                   entry);
    return entry;
}

int bcmtrm_entry_free(bcmtrm_entry_t *entry)
{
    int rv;

    if (!entry->asynch) {
        if (entry->state != E_ACTIVE) {
            return SHR_E_BUSY;
        } else {
            bcmtrm_entry_done(entry, true);
            return SHR_E_NONE;
        }
    }
    rv = sal_mutex_take(entry_state_mtx, SAL_MUTEX_FOREVER);
    if (rv != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }
    /* if the entry is done release it now, otherwise mark it to be deleted */
    if (entry->state == E_ACTIVE) {
        bcmtrm_entry_done(entry, true);
    } else {
        entry->delete = true;
    }
    sal_mutex_give(entry_state_mtx);
    return rv;
}

void bcmtrm_entry_done(bcmtrm_entry_t *entry, bool exclusive)
{
    shr_fmm_t *field;
    if (exclusive) {
        EXCLUSIVE_ELEMENT_EXTRACT(active_entry_list, active_entry_mutex, entry);
    } else {
        ELEMENT_EXTRACT(active_entry_list, entry);
    }
    /* Free all the fields */
    while (entry->l_field) {
        field = entry->l_field;
        entry->l_field = field->next;
        shr_fmm_free(field);
    }

    /* Free the field array */
    if (entry->fld_arr) {
        shr_lmm_free(entry->fld_arr_mem_hdl, entry->fld_arr);
    }

    entry->state = E_IDLE;
    shr_lmm_free(entry_mem_hdl, entry);
}

int bcmtrm_asynch_entry_req(bcmtrm_entry_t *entry)
{
    int rc;
    trans_instruct_t q_element;
    sal_msgq_priority_t pri;

    if (unit_resources[entry->info.unit].stop_api) {
        return SHR_E_UNAVAIL;
    }
    if (entry->priority == BCMLT_PRIORITY_NORMAL) {
        pri = SAL_MSGQ_NORMAL_PRIORITY;
    }  else {
        pri = SAL_MSGQ_HIGH_PRIORITY;
    }
    entry->asynch = true;
    rc = sal_mutex_take(entry_state_mtx, SAL_MUTEX_FOREVER);
    if (rc != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }
    if (entry->delete) {
        sal_mutex_give(entry_state_mtx);
        bcmtrm_entry_done(entry, true);
        return SHR_E_NO_HANDLER;
    }
    entry->state = E_COMMITTING;
    sal_mutex_give(entry_state_mtx);
    q_element.element = (void *)entry;
    q_element.type = ENTRY_OP;
    if (entry->interactive || entry->pt) {
        if (entry->interactive) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                    "Interactive async processing entry "\
                                        "for table %s operation %s\n"),
                         entry->info.table_name,
                         bcmtrm_ltopcode_to_str(entry->opcode.lt_opcode)));
        }
        rc = sal_msgq_post(unit_resources[entry->info.unit].interact_queue,
                           &q_element,
                           pri,
                           SAL_MSGQ_FOREVER);
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                          "async processing entry for table %s operation %s\n"),
                     entry->info.table_name,
                     bcmtrm_ltopcode_to_str(entry->opcode.lt_opcode)));
        rc = sal_msgq_post(unit_resources[entry->info.unit].model_queue,
                           &q_element,
                           pri,
                           SAL_MSGQ_FOREVER);
    }
    return rc;
}

bcmtrm_trans_t *bcmtrm_trans_alloc(bcmlt_trans_type_t type)
{
    bcmtrm_trans_t *trans;

    if (atomic_disabled && type == BCMLT_TRANS_TYPE_ATOMIC) {
        return NULL;
    }

    trans = shr_lmm_alloc(trans_mem_hdl);
    if (!trans) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Maximum transactions were allocated\n")));
        return NULL;
    }
    sal_memset(trans, 0, sizeof(*trans));
    trans->state = T_ACTIVE;
    trans->info.type = type;
    trans->info.status = SHR_E_NONE;
    trans->syncronous = true;   /* Required for bcmtrm_trans_free() */
    ELEMENT_INSERT(active_trans_list,
                   active_trans_mutex,
                   trans);
    return trans;
}

void bcmtrm_trans_done(bcmtrm_trans_t *trans, bool exclusive)
{
    bcmtrm_entry_t *entry = trans->l_entries;
    bcmtrm_entry_t *entry_tmp;

    if (exclusive) {
        EXCLUSIVE_ELEMENT_EXTRACT(active_trans_list, active_trans_mutex, trans);
    } else {
        ELEMENT_EXTRACT(active_trans_list, trans);
    }
    while (entry) {
        if (entry->ltm_entry) {
            shr_lmm_free(bcmtrm_ltm_entry_hdl, (void *)entry->ltm_entry);
            entry->ltm_entry = NULL;
        }
        entry_tmp = entry;
        entry = entry->next;
        bcmtrm_entry_done(entry_tmp, true);
    }
    if (trans->lock_obj) {
        shr_lmm_free(ltm_prot_obj_hdl, trans->lock_obj);
        trans->lock_obj = NULL;
    }
    trans->state = T_IDLE;
    shr_lmm_free(trans_mem_hdl, trans);
}

int bcmtrm_trans_free(bcmtrm_trans_t *trans)
{
    int rv;

    if (trans->syncronous) {
        if (trans->state != T_ACTIVE) {
            return SHR_E_BUSY;
        } else {
            bcmtrm_trans_done(trans, true);
            return SHR_E_NONE;
        }
    }
    /* Now deal with asynch transactions */
    rv = sal_mutex_take(trans->lock_obj->mutex, SAL_MUTEX_FOREVER);
    if (rv != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }
    if (trans->state == T_ACTIVE) {
        sal_mutex_give(trans->lock_obj->mutex);
        bcmtrm_trans_done(trans, true);
    } else {
        trans->delete = true;
        sal_mutex_give(trans->lock_obj->mutex);
    }
    return rv;
}

static void entry_sync_pt_comp(bcmlt_notif_option_t event,
                               bcmlt_pt_opcode_t    opcode,
                               bcmlt_entry_info_t  *entry_info,
                               void *usr_data)
{
    sal_sem_give((sal_sem_t)usr_data);
}

static void entry_sync_lt_comp(bcmlt_notif_option_t event,
                               bcmlt_opcode_t    opcode,
                               bcmlt_entry_info_t  *entry_info,
                               void *usr_data)
{
    bcmtrm_entry_t *entry = (bcmtrm_entry_t *)usr_data;

    sal_sem_give((sal_sem_t)entry->usr_data);
}

static int interactive_entry_req(bcmtrm_entry_t *entry,
                                 trm_sync_object_t *sync_obj,
                                 trm_unit_resources_t *unit_res)
{
    trans_instruct_t q_element;
    sal_msgq_priority_t pri;
    int rc;

    if (entry->priority == BCMLT_PRIORITY_NORMAL) {
        pri = SAL_MSGQ_NORMAL_PRIORITY;
    }  else {
        pri = SAL_MSGQ_HIGH_PRIORITY;
    }

    entry->cb.entry_pt_cb = entry_sync_pt_comp;
    entry->info.notif_opt = BCMLT_NOTIF_OPTION_HW;
    entry->usr_data = (void *)sync_obj->sem;

    q_element.element = (void *)entry;
    q_element.type = ENTRY_OP;
    rc = sal_msgq_post(unit_res->interact_queue,
                       &q_element,
                       pri,
                       SAL_MSGQ_FOREVER);
    return (rc == 0 ? SHR_E_NONE : SHR_E_INTERNAL);
}

int bcmtrm_entry_req(bcmtrm_entry_t *entry)
{
    int rv;
    trm_sync_object_t *sync_obj;
    trm_unit_resources_t *unit_res = &unit_resources[entry->info.unit];

    if (unit_res->stop_api) {
        return SHR_E_UNAVAIL;
    }

    entry->asynch = false;
    entry->state = E_COMMITTING;

    sync_obj = sync_obj_alloc();
    if (!sync_obj) {
        return SHR_E_MEMORY;
    }

    /* Increment the synchronous counter */
    sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
    unit_res->active_sync_trans++;
    sal_mutex_give(unit_res->mtx);

    if (entry->interactive || entry->pt) {
        if (entry->interactive) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(entry->info.unit,
                                    "Interactive processing entry for table " \
                                    "%s operation %s\n"),
                         entry->info.table_name,
                         bcmtrm_ltopcode_to_str(entry->opcode.lt_opcode)));
        }
        rv = interactive_entry_req(entry, sync_obj, unit_res);
    } else {
        entry->cb.entry_cb = entry_sync_lt_comp;
        entry->usr_data = sync_obj->sem;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Processing entry for table %s operation %s\n"),
                     entry->info.table_name,
                     bcmtrm_ltopcode_to_str(entry->opcode.lt_opcode)));

        sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
        rv = bcmtrm_process_entry_req(entry,
                                      &unit_res->op_id,
                                      &unit_res->trans_id);
        sal_mutex_give(unit_res->mtx);
    }

    if (rv == SHR_E_NONE) {
        sal_sem_take(sync_obj->sem, SAL_SEM_FOREVER);
    }

    sal_mutex_take(unit_res->mtx, SAL_SEM_FOREVER);
    unit_res->active_sync_trans--;
    sal_mutex_give(unit_res->mtx);

    /* Clean up */
    sync_obj_free(sync_obj);
    entry->cb.entry_cb = NULL;
    entry->usr_data = NULL;

    /*
     * Interactive entries will be processed via the interactive and
     * notification threads.
     */
    if (!entry->interactive && !entry->pt) {
        if (entry->ltm_entry) {
            bcmtrm_proc_entry_results(entry, entry->ltm_entry);
        }

        /* Inform other application entities of the change */
        if (entry->info.status == SHR_E_NONE) {
            bcmtrm_tbl_chg_event(entry);
        }
    }

    entry->state = E_ACTIVE;
    return SHR_E_NONE;
}



