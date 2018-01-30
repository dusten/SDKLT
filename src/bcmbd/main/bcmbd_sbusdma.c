/*! \file bcmbd_sbusdma.c
 *
 * SBUSDMA driver APIs
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_thread.h>
#include <sal/sal_time.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_devlist.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_SBUSDMA

static sbusdma_ctrl_t *sbusdma_ctrl[SBUSDMA_DEV_NUM_MAX];

inline static int
sbusdma_workqueue_init(sbusdma_workqueue_t *queue)
{
    queue->head = queue->rear = NULL;
    queue->count = 0;
    queue->lock = sal_spinlock_create("sbusdma_workqueue_lock");
    if (!queue->lock) {
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

inline static int
sbusdma_workqueue_cleanup(sbusdma_workqueue_t *queue)
{
    sal_spinlock_lock(queue->lock);
    queue->head = queue->rear = NULL;
    queue->count = 0;
    sal_spinlock_unlock(queue->lock);

    sal_spinlock_destroy(queue->lock);
    queue->lock = NULL;

    return SHR_E_NONE;
}

inline static int
sbusdma_workqueue_is_empty(sbusdma_workqueue_t *queue)
{
    return queue->head == NULL;
}

inline static int
sbusdma_work_enqueue(sbusdma_workqueue_t *queue, bcmbd_sbusdma_work_t *work)
{
    sal_spinlock_lock(queue->lock);
    if (!queue->head) {
        queue->head = work;
    } else {
        ((bcmbd_sbusdma_work_t *)queue->rear)->link = work;
    }
    queue->rear = work;
    queue->count++;
    sal_spinlock_unlock(queue->lock);

    return SHR_E_NONE;
}

inline static int
sbusdma_work_dequeue(sbusdma_workqueue_t *queue, bcmbd_sbusdma_work_t **work)
{
    sal_spinlock_lock(queue->lock);
    *work = (bcmbd_sbusdma_work_t *)queue->head;
    if (*work) {
        queue->head = (*work)->link;
        (*work)->link = NULL;
        queue->count--;
    }
    sal_spinlock_unlock(queue->lock);

    return SHR_E_NONE;
}

static void
sbusdma_work_process(void *vp)
{
    sbusdma_ctrl_t *ctrl = vp;
    bcmbd_sbusdma_work_t *work = NULL;

    do {
        sal_sem_take(ctrl->sem, SAL_SEM_FOREVER);
        while (!sbusdma_workqueue_is_empty(&ctrl->queue)) {
            sbusdma_work_dequeue(&ctrl->queue, &work);
            if (!work) {
                break;
            }
            work->state = SBUSDMA_WORK_SCHED;
            if (work->pc) {
                work->pc(ctrl->unit, work->pc_data);
            }
            ctrl->ops->work_execute(ctrl, work);
            if (work->cb) {
                work->cb(ctrl->unit, work->cb_data);
            }
        }
    } while (ctrl->active);

    ctrl->pid = SAL_THREAD_ERROR;

    sal_thread_exit(0);
}

int
bcmbd_sbusdma_attach(int unit)
{
    sbusdma_ctrl_t *ctrl = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    ctrl = bcmbd_sbusdma_ctrl_get(unit);
    if (!ctrl) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(ctrl, 0, sizeof(*ctrl));
    ctrl->unit = unit;

    sbusdma_workqueue_init(&ctrl->queue);

    ctrl->sem = sal_sem_create("sbusdma_ctrl_sem", SAL_SEM_BINARY, 0);
    if (!ctrl->sem) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    for (i = 0; i < DEV_QUEUE_NUM_MAX; i++) {
        ctrl->intr[i] = sal_sem_create("sbusdma_intr_sem", SAL_SEM_BINARY, 0);
        if (!ctrl->intr[i]) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    ctrl->pid = sal_thread_create("sbusdma_daemon", SAL_THREAD_STKSZ,
                                  SBUSDMA_THREAD_PRI,
                                  sbusdma_work_process, (void *)ctrl);
    if (ctrl->pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Could not start sbusdma work thread.\n")));
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    ctrl->active = 1;
    sbusdma_ctrl[unit] = ctrl;

exit:
    if (SHR_FUNC_ERR()) {
        if (ctrl) {
            for (i = 0; i < DEV_QUEUE_NUM_MAX; i++) {
                if (ctrl->intr[i]) {
                    sal_sem_destroy(ctrl->intr[i]);
                    ctrl->intr[i] = NULL;
                }
            }

            if (ctrl->sem) {
                sal_sem_destroy(ctrl->sem);
                ctrl->sem = NULL;
            }

            sbusdma_workqueue_cleanup(&ctrl->queue);
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmbd_sbusdma_detach(int unit)
{
    sbusdma_ctrl_t *ctrl = sbusdma_ctrl[unit];
    bcmbd_sbusdma_work_t *work = NULL;
    int retry = 1000;
    int i;
    int rv = SHR_E_NONE;

    if (!ctrl) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->active) {
        return SHR_E_NONE;
    }

    ctrl->active = 0;

    if (ctrl->pid != SAL_THREAD_ERROR) {
        sal_sem_give(ctrl->sem);

        while (ctrl->pid != SAL_THREAD_ERROR && retry--) {
            sal_usleep(1000);
        }
        if (ctrl->pid != SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Sbusdma thread will not exit.\n")));
            rv = SHR_E_INTERNAL;
        }
    }

    for (i = 0; i < DEV_QUEUE_NUM_MAX; i++) {
        if (ctrl->intr[i]) {
            sal_sem_destroy(ctrl->intr[i]);
            ctrl->intr[i] = NULL;
        }
    }

    if (ctrl->sem) {
        sal_sem_destroy(ctrl->sem);
        ctrl->sem = NULL;
    }

    while (1) {
        sbusdma_work_dequeue(&ctrl->queue, &work);
        if (!work) {
            break;
        } else {
            work->state = SBUSDMA_WORK_CREATED;
            ctrl->ops->batch_work_delete(ctrl, work);
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Should not have pended work at this moment.\n")));
        }
    }

    sbusdma_workqueue_cleanup(&ctrl->queue);

    return rv;
}

int
bcmbd_sbusdma_light_work_init(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work) {
        return SHR_E_PARAM;
    }

    if (work->state != SBUSDMA_WORK_NULL) {
        return SHR_E_UNAVAIL;
    }

    return ctrl->ops->light_work_init(ctrl, work);
}

int
bcmbd_sbusdma_batch_work_create(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work || !work->data) {
        return SHR_E_PARAM;
    }

    if (work->state != SBUSDMA_WORK_NULL &&
        work->state != SBUSDMA_WORK_DESTROYED) {
        return SHR_E_UNAVAIL;
    }

    return ctrl->ops->batch_work_create(ctrl, work);
}

int
bcmbd_sbusdma_batch_work_delete(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work) {
        return SHR_E_PARAM;
    }

    if (work->state == SBUSDMA_WORK_NULL ||
        work->state == SBUSDMA_WORK_DESTROYED) {
        return SHR_E_NONE;
    }

    return ctrl->ops->batch_work_delete(ctrl, work);
}

int
bcmbd_sbusdma_work_execute(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work || !work->data) {
        return SHR_E_PARAM;
    }

    if (work->state != SBUSDMA_WORK_INITED &&
        work->state != SBUSDMA_WORK_CREATED &&
        work->state < SBUSDMA_WORK_DONE) {
        return SHR_E_UNAVAIL;
    }

    return ctrl->ops->work_execute(ctrl, work);
}

int
bcmbd_sbusdma_work_add(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work || !work->data) {
        return SHR_E_PARAM;
    }

    if (work->state != SBUSDMA_WORK_INITED &&
        work->state != SBUSDMA_WORK_CREATED &&
        work->state < SBUSDMA_WORK_DONE) {
        return SHR_E_UNAVAIL;
    }

    work->state = SBUSDMA_WORK_QUEUED;

    sbusdma_work_enqueue(&ctrl->queue, work);
    sal_sem_give(ctrl->sem);

    return SHR_E_NONE;
}

