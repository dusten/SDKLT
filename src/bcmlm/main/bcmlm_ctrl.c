/*! \file bcmlm_ctrl.c
 *
 * Link controller.
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>
#include <sal/sal_mutex.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include "bcmlm_internal.h"

/******************************************************************************
* Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLM_CTRL

/*
 * Link manager control structure.
 */
typedef struct bcmlm_ctrl_s {

    /* Synchronization */
    sal_mutex_t     lock;

    /* Linkscan thread name */
    char            name[THREAD_NAME_LEN_MAX];

    /* Control semaphore */
    sal_sem_t       sem;

    /* Linkscan thread ID */
    sal_thread_t    pid;

    /* system is ready for linkscan to start */
    int             ready;

    /* Linkscan thread running */
    int             linkscan_running;

    /* Time between scans */
    int             interval_us;

    /* Ports enabled with link scan */
    bcmdrd_pbmp_t   pbm_linkscan;

    /* Software link scan ports */
    bcmdrd_pbmp_t   pbm_sw;

    /* Hardware link scan ports */
    bcmdrd_pbmp_t   pbm_hw;

    /* Link override ports */
    bcmdrd_pbmp_t   pbm_ovr;

    /* Link state for ports with override linkscan mode */
    bcmdrd_pbmp_t   pbm_ovr_link;

    /* Force up/down ports */
    bcmdrd_pbmp_t   pbm_force;

    /* Force up/down state */
    bcmdrd_pbmp_t   pbm_force_link;

    /* Ports currently up */
    bcmdrd_pbmp_t   pbm_link_up;

    /* Ports receiving/transmitting fault */
    bcmdrd_pbmp_t   pbm_fault;

    /* Ports that newly enabled with link scan */
    bcmdrd_pbmp_t   pbm_newly_enabled;

    /* Ports that newly disabled with link scan */
    bcmdrd_pbmp_t   pbm_newly_disabled;

    /* Ports that suspended scan */
    bcmdrd_pbmp_t   pbm_suspend;

    /* HW link config has changed */
    int             hw_conf_change;

    /* HW link status has changed */
    int             hw_change;

    /* Override link status has changed */
    int             ovr_change;

} bcmlm_ctrl_t;

/* link control database */
static bcmlm_ctrl_t *bcmlm_ctrl[BCMLM_DEV_NUM_MAX];

#define LC_LOCK(unit) \
    sal_mutex_take(bcmlm_ctrl[unit]->lock, SAL_MUTEX_FOREVER)

#define LC_UNLOCK(unit) \
    sal_mutex_give(bcmlm_ctrl[unit]->lock)

/******************************************************************************
* Private functions
 */
static int
bcmlm_link_update(int unit, shr_port_t port)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    int cur_link, new_link, cur_fault, new_fault;
    int lc_locked = 0;
    int logical_link, fault_chk;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    LC_LOCK(unit);
    lc_locked = 1;

    cur_link = BCMDRD_PBMP_MEMBER(lmctrl->pbm_link_up, port) ? 1 : 0;
    new_link = FALSE;
    cur_fault = BCMDRD_PBMP_MEMBER(lmctrl->pbm_fault, port) ? 1 : 0;
    new_fault = FALSE;

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_force, port)) {
        new_link = BCMDRD_PBMP_MEMBER(lmctrl->pbm_force_link, port) ? 1 : 0;
    } else if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port)) {
        new_link = BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr_link, port) ? 1 : 0;
    } else if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
        bcmdrd_pbmp_t hw_link;
        SHR_IF_ERR_EXIT
            (bcmlm_hw_linkscan_link_get(unit, &hw_link));
        new_link = BCMDRD_PBMP_MEMBER(hw_link, port) ? 1 : 0;
    } else if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sw, port)) {
        SHR_IF_ERR_EXIT
            (bcmlm_sw_linkscan_link_get(unit, port, &new_link));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    logical_link = new_link;
    if (SHR_FAILURE(bcmlm_linkscan_fault_check_enabled(unit, &fault_chk))) {
        fault_chk = 0;
    }
    if (cur_link && new_link && (fault_chk == 1) &&
        (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sw, port) ||
         BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port))) {
            SHR_IF_ERR_EXIT
                (bcmlm_linkscan_fault_get(unit, port, &new_fault));
            logical_link = (new_link && !new_fault) ? 1 : 0;
    }

    if ((((cur_link) && (new_link) && (cur_fault == new_fault)) ||
        ((!cur_link) && (!new_link))) &&
        !(BCMDRD_PBMP_MEMBER(lmctrl->pbm_newly_enabled, port))) {
        /* No change */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (!new_link) {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_link_up, port);
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_fault, port);
    }

    if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_force, port) &&
        ((cur_link != new_link) || (cur_fault != new_fault))) {
        SHR_IF_ERR_EXIT
            (bcmlm_port_update(unit, port, logical_link));
    }

    if (new_link) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_link_up, port);
        if (new_fault) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_fault, port);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_fault, port);
        }
    }

    if (cur_link != new_link) {
        if (new_link) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: link up\n"),
                      port));
        } else {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: link down\n"),
                      port));
        }
    }

    if (cur_fault != new_fault) {
        if (new_fault) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: fault detected\n"),
                      port));
        } else {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: fault cleared\n"),
                      port));
        }
    }

    lc_locked = 0;
    LC_UNLOCK(unit);

    /* Update link state in IMM */
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_newly_enabled, port)) {
        SHR_IF_ERR_CONT
            (bcmlm_imm_link_state_insert(unit, port, logical_link));
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_newly_enabled, port);
    } else {
        SHR_IF_ERR_CONT
            (bcmlm_imm_link_state_update(unit, port, logical_link));
    }

exit:
    if (lc_locked) {
        LC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

static void
bcmlm_linkscan_hw_interrupt(int unit, uint32_t data)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    if (SHR_FAILURE(bcmlm_hw_linkscan_intr_clear(unit))) {
        return;
    }

    if ((lmctrl != NULL) && (lmctrl->sem != NULL)) {
        lmctrl->hw_change = 1;
        sal_sem_give(lmctrl->sem);
    }
}

static void
bcmlm_linkscan_thread(void *arg)
{
    int unit = (unsigned long)(arg);
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    shr_port_t port;
    bcmdrd_pbmp_t pbm_link, pbm_update, empty, new, pbm_hw, pbm_sw, pbm_ovr;
    int link, interval;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Linkscan starting\n")));

    while (!lmctrl->ready) {
        sal_thread_yield();
    }

    /* Insert link state entries to IMM. */
    BCMDRD_PBMP_ITER(lmctrl->pbm_linkscan, port) {
        if (bcmpc_lport_to_pport(unit, port) == BCMPC_INVALID_PPORT) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_suspend, port);
            continue;
        }
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port)) {
            link = BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr_link, port) ? 1 : 0;
        }
        if (!SHR_SUCCESS(bcmlm_sw_linkscan_link_get(unit, port, &link))) {
            link = 0;
        }
        if (link) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_link_up, port);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_link_up, port);
        }
        if (SHR_SUCCESS(bcmlm_imm_link_state_insert(unit, port, link))) {
            BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_newly_enabled, port);
        }
    }

    BCMDRD_PBMP_CLEAR(lmctrl->pbm_newly_disabled);

    /* Register for hardware linkscan interrupt. */
    if (SHR_FAILURE
            (bcmlm_hw_linkscan_intr_cb_set(unit,
                                           bcmlm_linkscan_hw_interrupt))) {
        SHR_EXIT();
    }

    /* Config HW Linkscan */
    if (SHR_FAILURE
            (bcmlm_hw_linkscan_config(unit, lmctrl->pbm_hw))) {
        SHR_EXIT();
    }

    while (lmctrl->linkscan_running) {
        interval = lmctrl->interval_us;

        if (BCMDRD_PBMP_NOT_NULL(lmctrl->pbm_newly_disabled)) {
            BCMDRD_PBMP_ITER(lmctrl->pbm_newly_disabled, port) {
                if (SHR_SUCCESS(bcmlm_imm_link_state_delete(unit, port))) {
                    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_newly_disabled, port);
                }
            }
        }

        if (lmctrl->hw_conf_change) {
            lmctrl->hw_conf_change = 0;
            lmctrl->hw_change = 1;
            BCMDRD_PBMP_ASSIGN(pbm_hw, lmctrl->pbm_hw);
            BCMDRD_PBMP_XOR(pbm_hw, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_hw, lmctrl->pbm_hw);

            /* Update hardware linkscan port configure */
            if (SHR_FAILURE(bcmlm_hw_linkscan_config(unit, pbm_hw))) {
                /* Try to update hw linkscan config again in next round */
                lmctrl->hw_conf_change = 1;
            }
        }

        if (lmctrl->hw_change) {
            lmctrl->hw_change = 0;
            bcmlm_hw_linkscan_link_get(unit, &pbm_link);
            BCMDRD_PBMP_ASSIGN(pbm_hw, lmctrl->pbm_hw);
            BCMDRD_PBMP_XOR(pbm_hw, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_hw, lmctrl->pbm_hw);
            BCMDRD_PBMP_AND(pbm_link, pbm_hw);
            BCMDRD_PBMP_ASSIGN(pbm_update, pbm_link);
            BCMDRD_PBMP_XOR(pbm_update, lmctrl->pbm_link_up);
            BCMDRD_PBMP_AND(pbm_update, pbm_hw);
            BCMDRD_PBMP_ASSIGN(new, lmctrl->pbm_newly_enabled);
            BCMDRD_PBMP_AND(new, pbm_hw);
            BCMDRD_PBMP_OR(pbm_update, new);
            BCMDRD_PBMP_ITER(pbm_update, port) {
                bcmlm_link_update(unit, port);
            }
        }
        if (lmctrl->ovr_change) {
            lmctrl->ovr_change = 0;
            BCMDRD_PBMP_ASSIGN(pbm_ovr, lmctrl->pbm_ovr);
            BCMDRD_PBMP_XOR(pbm_ovr, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_ovr, lmctrl->pbm_ovr);
            BCMDRD_PBMP_ASSIGN(pbm_link, lmctrl->pbm_ovr_link);
            BCMDRD_PBMP_AND(pbm_link, pbm_ovr);
            BCMDRD_PBMP_ASSIGN(pbm_update, pbm_link);
            BCMDRD_PBMP_XOR(pbm_update, lmctrl->pbm_link_up);
            BCMDRD_PBMP_AND(pbm_update, pbm_ovr);
            BCMDRD_PBMP_ASSIGN(new, lmctrl->pbm_newly_enabled);
            BCMDRD_PBMP_AND(new, pbm_ovr);
            BCMDRD_PBMP_OR(pbm_update, new);
            BCMDRD_PBMP_ITER(pbm_update, port) {
                bcmlm_link_update(unit, port);
            }
        }
        if (!BCMDRD_PBMP_IS_NULL(lmctrl->pbm_sw)) {
            BCMDRD_PBMP_ASSIGN(pbm_sw, lmctrl->pbm_sw);
            BCMDRD_PBMP_XOR(pbm_sw, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_sw, lmctrl->pbm_sw);
            BCMDRD_PBMP_ITER(pbm_sw, port) {
                bcmlm_link_update(unit, port);
            }
        } else {
            interval = SAL_SEM_FOREVER;
        }
        sal_sem_take(lmctrl->sem, interval);
    }

    /* To prevent getting HW linkscan interrupt after linkscan is disabled,
     * HW linkscan must be disabled.
     */
    BCMDRD_PBMP_CLEAR(empty);
    if (SHR_FAILURE((bcmlm_hw_linkscan_config(unit, empty)))) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Failed to disable hardware linkscan.\n")));
    }

    /* Unregister for hardware linkscan interrupt. */
    bcmlm_hw_linkscan_intr_cb_set(unit, NULL);

    /* Clear the link state table in IMM */
    if (SHR_FAILURE(bcmlm_imm_link_state_clear(unit))) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Failed to clear link state table.\n")));
    }

exit:
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Linkscan exiting\n")));

    lmctrl->pid = NULL;
    sal_thread_exit(0);
}

static void
bcmlm_port_add_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port)) {
        LC_LOCK(unit);
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_suspend, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_enabled, port);
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
            lmctrl->hw_conf_change = 1;
        }
        LC_UNLOCK(unit);

        if (lmctrl->linkscan_running && lmctrl->sem) {
            sal_sem_give(lmctrl->sem);
        }
    }
}

static void
bcmlm_port_del_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) &&
        !BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        LC_LOCK(unit);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_suspend, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_disabled, port);
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_link_up, port);
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
            lmctrl->hw_conf_change = 1;
        }
        LC_UNLOCK(unit);

        if (lmctrl->linkscan_running && lmctrl->sem) {
            sal_sem_give(lmctrl->sem);
        }
    }
}

static void
bcmlm_port_force_up_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_force, port);
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_force_link, port);
    LC_UNLOCK(unit);

    if (lmctrl->linkscan_running && lmctrl->sem) {
        sal_sem_give(lmctrl->sem);
    }
}

static void
bcmlm_port_force_down_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_force, port);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_force_link, port);
    LC_UNLOCK(unit);

    if (lmctrl->linkscan_running && lmctrl->sem) {
        sal_sem_give(lmctrl->sem);
    }
}

static void
bcmlm_port_no_force_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_force, port);
    LC_UNLOCK(unit);

    if (lmctrl->linkscan_running && lmctrl->sem) {
        sal_sem_give(lmctrl->sem);
    }
}

/******************************************************************************
 * Public Functions
 */
int
bcmlm_ctrl_stop(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_NONE);

    if(lmctrl->linkscan_running) {
        shr_timeout_t to;
        sal_usecs_t timeout_usec = 10000000;

        SHR_IF_ERR_EXIT
            (bcmlm_ctrl_linkscan_enable(unit, 0));

        shr_timeout_init(&to, timeout_usec, 0);
        while (lmctrl->pid) {
            if (shr_timeout_check(&to)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Linkscan thread did not exit\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_cleanup(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_NONE);

    if (lmctrl) {
        if (lmctrl->lock) {
            sal_mutex_destroy(lmctrl->lock);
        }
        if(lmctrl->sem) {
            sal_sem_destroy(lmctrl->sem);
        }
        sal_free(lmctrl);
        bcmlm_ctrl[unit] = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_init(int unit)
{
    bcmlm_ctrl_t *lmctrl = NULL;

    SHR_FUNC_ENTER(unit);

    if (bcmlm_ctrl[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (bcmlm_ctrl_stop(unit));
        SHR_IF_ERR_EXIT
            (bcmlm_ctrl_cleanup(unit));
    }

    lmctrl = sal_alloc(sizeof(bcmlm_ctrl_t), "bcmlm_ctrl");
    if (!lmctrl) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    /* Init bcmlm control info */
    sal_memset(lmctrl, 0, sizeof(bcmlm_ctrl_t));

    lmctrl->sem = sal_sem_create("bcmlm_ctrl_sem", SAL_SEM_BINARY, 0);
    if (!lmctrl->sem) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    lmctrl->lock = sal_mutex_create("bcmlm_ctrl_lock");
    if (!lmctrl->lock) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    sal_snprintf(lmctrl->name, sizeof(lmctrl->name), "LM_THREAD.%d", unit);
    lmctrl->interval_us = BCMLM_LINKSCAN_INTERVAL_DEFAULT;
    bcmlm_ctrl[unit] = lmctrl;

exit:
    if (SHR_FUNC_ERR()) {
        if (lmctrl && lmctrl->lock) {
            sal_mutex_destroy(lmctrl->lock);
        }

        if (lmctrl && lmctrl->sem) {
            sal_sem_destroy(lmctrl->sem);
        }

        if (lmctrl) {
            sal_free(lmctrl);
        }
        bcmlm_ctrl[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_ready(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    lmctrl->ready = 1;

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_interval_update(int unit, uint32_t interval)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    lmctrl->interval_us = interval;

    if (lmctrl->linkscan_running && lmctrl->sem) {
        sal_sem_give(lmctrl->sem);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_linkscan_enable(int unit, bool enable)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    if (enable) {
        /* Linkscan is already running */
        if (lmctrl->linkscan_running || (lmctrl->pid != NULL)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Linkscan is already running.\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }

        lmctrl->linkscan_running = 1;

        lmctrl->pid = sal_thread_create(lmctrl->name,
                                        SAL_THREAD_STKSZ,
                                        LINKSCAN_THREAD_PRI,
                                        bcmlm_linkscan_thread,
                                        ((void *)(unsigned long)(unit)));

        if (lmctrl->pid == SAL_THREAD_ERROR) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }

    } else if (lmctrl->pid != NULL) {

        lmctrl->linkscan_running = 0;

        sal_sem_give(lmctrl->sem);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_linkscan_mode_update(int unit,
                                shr_port_t port,
                                bcmlm_linkscan_mode_t mode)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    if ((BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port) &&
         (mode == BCMLM_LINKSCAN_MODE_HW)) ||
        (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sw, port) &&
         (mode == BCMLM_LINKSCAN_MODE_SW)) ||
        (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port) &&
         (mode == BCMLM_LINKSCAN_MODE_OVERRIDE)) ||
        (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) &&
         (mode == BCMLM_LINKSCAN_MODE_NONE))) {
        /* No linkscan mode change */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    LC_LOCK(unit);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port) ||
        mode == BCMLM_LINKSCAN_MODE_HW) {
        lmctrl->hw_conf_change = 1;
    }
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port) ||
        mode == BCMLM_LINKSCAN_MODE_OVERRIDE) {
        lmctrl->ovr_change = 1;
    }
    if(!BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) &&
       mode != BCMLM_LINKSCAN_MODE_NONE) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_enabled, port);
    }
    if(BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) &&
       mode == BCMLM_LINKSCAN_MODE_NONE) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_disabled, port);
    }
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_hw, port);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_sw, port);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ovr, port);

    switch (mode) {
    case BCMLM_LINKSCAN_MODE_NONE:
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_linkscan, port);
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_fault, port);
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_link_up, port);
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_suspend, port);
        break;
    case BCMLM_LINKSCAN_MODE_SW:
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_sw, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        break;
    case BCMLM_LINKSCAN_MODE_HW:
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_hw, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        break;
    case BCMLM_LINKSCAN_MODE_OVERRIDE:
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ovr, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        break;
    default:
        LC_UNLOCK(unit);
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    LC_UNLOCK(unit);

    /* Register change now */
    if (lmctrl->linkscan_running && lmctrl->sem) {
        sal_sem_give(lmctrl->sem);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_override_link_state_update(int unit,
                                      shr_port_t port,
                                      bool link)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    LC_LOCK(unit);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr_link, port) ^ link) {
        lmctrl->ovr_change = 1;
    }

    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ovr_link, port);

    if (link) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ovr_link, port);
    }
    LC_UNLOCK(unit);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port) &&
        lmctrl->linkscan_running && lmctrl->sem) {
        sal_sem_give(lmctrl->sem);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_port_event_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvPortAdd",
                                         bcmlm_port_add_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvPortDelete",
                                         bcmlm_port_del_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvLinkForceUp",
                                         bcmlm_port_force_up_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvLinkForceDown",
                                         bcmlm_port_force_down_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvLinkForceNone",
                                         bcmlm_port_no_force_event_hdl));
exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_port_event_unregister(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvPortAdd",
                                           bcmlm_port_add_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvPortDelete",
                                           bcmlm_port_del_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvLinkForceUp",
                                           bcmlm_port_force_up_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvLinkForceDown",
                                           bcmlm_port_force_down_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvLinkForceNone",
                                           bcmlm_port_no_force_event_hdl));
exit:
    SHR_FUNC_EXIT();
}
