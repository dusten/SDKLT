/*! \file bcmbd_fifodma.c
 *
 * FIFODMA driver APIs
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
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmbd/bcmbd_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_FIFODMA

static fifodma_ctrl_t *fifodma_ctrl[FIFODMA_DEV_NUM_MAX];

static void
fifodma_intr_handler(void *vp)
{
    fifodma_ctrl_t *ctrl = vp;
    fifodma_chan_t *fifo = NULL;
    bcmbd_fifodma_work_t *work = NULL;
    int chan;

    do {
        sal_sem_take(ctrl->intr, SAL_SEM_FOREVER);
        if (!ctrl->active) {
            break;
        }
        for (chan = 0; chan < ctrl->chan_num; chan++) {
            fifo = &ctrl->fifo[chan];
            sal_spinlock_lock(fifo->lock);
            if (fifo->intr_active && fifo->intr_enabled) {
                fifo->intr_active = 0;
                sal_spinlock_unlock(fifo->lock);
                ctrl->ops->fifodma_handle(ctrl, chan);
            } else {
                sal_spinlock_unlock(fifo->lock);
                continue;
            }
            work = &ctrl->work[chan];
            if (work->cb) {
                work->cb(ctrl->unit, work->cb_data);
            }
        }
    } while (ctrl->active);

    ctrl->pid = SAL_THREAD_ERROR;

    sal_thread_exit(0);
}

static void
fifodma_intr_notify(int unit, int chan)
{
    fifodma_ctrl_t *ctrl = fifodma_ctrl[unit];
    fifodma_chan_t *fifo = &ctrl->fifo[chan];

    sal_spinlock_lock(fifo->lock);
    fifo->intr_active = 1;
    sal_spinlock_unlock(fifo->lock);

    sal_sem_give(ctrl->intr);
}

int
bcmbd_fifodma_attach(int unit)
{
    fifodma_ctrl_t *ctrl = NULL;
    fifodma_chan_t *fifo = NULL;
    int chan;

    SHR_FUNC_ENTER(unit);

    ctrl = bcmbd_fifodma_ctrl_get(unit);
    if (!ctrl) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(ctrl, 0, sizeof(*ctrl));
    ctrl->unit = unit;
    ctrl->chan_num = FIFODMA_CHAN_NUM_MAX;

    for (chan = 0; chan < ctrl->chan_num; chan++) {
        fifo = &ctrl->fifo[chan];
        fifo->lock = sal_spinlock_create("fifodma_intr_lock");
        if (!fifo->lock) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    ctrl->intr = sal_sem_create("fifodma_intr_sem", SAL_SEM_BINARY, 0);
    if (!ctrl->intr) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    ctrl->pid = sal_thread_create("fifodma_intr_handler", SAL_THREAD_STKSZ,
                                  FIFODMA_THREAD_PRI,
                                  fifodma_intr_handler, (void *)ctrl);
    if (ctrl->pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Could not start fifodma handler thread.\n")));
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    ctrl->intr_notify = fifodma_intr_notify;

    ctrl->active = 1;
    fifodma_ctrl[unit] = ctrl;

exit:
    if (SHR_FUNC_ERR()) {
        if (ctrl) {
            if (ctrl->intr) {
                sal_sem_destroy(ctrl->intr);
                ctrl->intr = NULL;
            }

            for (chan = 0; chan < ctrl->chan_num; chan++) {
                fifo = &ctrl->fifo[chan];
                if (fifo->lock) {
                    sal_spinlock_destroy(fifo->lock);
                    fifo->lock = NULL;
                }
            }
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmbd_fifodma_detach(int unit)
{
    fifodma_ctrl_t *ctrl = fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;
    int retry = 1000;
    int chan;
    int rv = SHR_E_NONE;

    if (!ctrl) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->active) {
        return SHR_E_NONE;
    }

    ctrl->active = 0;

    if (ctrl->pid != SAL_THREAD_ERROR) {
        sal_sem_give(ctrl->intr);

        while (ctrl->pid != SAL_THREAD_ERROR && retry--) {
            sal_usleep(1000);
        }
        if (ctrl->pid != SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Fifodma thread will not exit.\n")));
            rv = SHR_E_INTERNAL;
        }
    }

    if (ctrl->intr) {
        sal_sem_destroy(ctrl->intr);
        ctrl->intr = NULL;
    }

    for (chan = 0; chan < ctrl->chan_num; chan++) {
        fifo = &ctrl->fifo[chan];
        if (fifo->lock) {
            sal_spinlock_destroy(fifo->lock);
            fifo->lock = NULL;
        }
    }

    return rv;
}

int
bcmbd_fifodma_start(int unit, int chan, bcmbd_fifodma_work_t *work)
{
    fifodma_ctrl_t *ctrl = fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;
    int rv;

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->chan_num || !work) {
        return SHR_E_PARAM;
    }

    if (work->flags & FIFODMA_WF_INT_MODE && !work->cb) {
        return SHR_E_PARAM;
    }

    fifo = &ctrl->fifo[chan];
    if (fifo->started) {
        return SHR_E_RESOURCE;
    }

    fifo->num_entries = work->data->num_entries;
    fifo->data_width = work->data->data_width;
    fifo->buf_size = fifo->num_entries * fifo->data_width * sizeof(uint32_t);
    fifo->buf_cache = sal_alloc(fifo->buf_size, "bcmbdFifoDmaCache");
    if (!fifo->buf_cache) {
        return SHR_E_MEMORY;
    }
    fifo->buf_addr = bcmdrd_hal_dma_alloc(unit, fifo->buf_size, "bcmbdFifoDmaBuffer",
                                          &fifo->buf_paddr);
    if (!fifo->buf_addr) {
        sal_free(fifo->buf_cache);
        return SHR_E_MEMORY;
    }
    sal_memcpy(&ctrl->work[chan], work, sizeof(bcmbd_fifodma_work_t));

    rv = ctrl->ops->fifodma_start(ctrl, chan);
    if (SHR_FAILURE(rv)) {
        bcmdrd_hal_dma_free(unit, fifo->buf_size, fifo->buf_addr, fifo->buf_paddr);
        fifo->buf_addr = NULL;
        sal_free(fifo->buf_cache);
        fifo->buf_cache = NULL;
        return rv;
    }

    work->buf_cache = fifo->buf_cache;

    fifo->started = 1;

    return SHR_E_NONE;
}

int
bcmbd_fifodma_stop(int unit, int chan)
{
    fifodma_ctrl_t *ctrl = fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;
    int rv;

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->chan_num) {
        return SHR_E_PARAM;
    }

    fifo = &ctrl->fifo[chan];
    if (!fifo->started) {
        return SHR_E_NONE;
    }

    rv = ctrl->ops->fifodma_stop(ctrl, chan);

    if (fifo->buf_addr) {
        bcmdrd_hal_dma_free(unit, fifo->buf_size, fifo->buf_addr, fifo->buf_paddr);
        fifo->buf_addr = NULL;
    }
    if (fifo->buf_cache) {
        sal_free(fifo->buf_cache);
        fifo->buf_cache = NULL;
    }

    fifo->started = 0;

    return rv;
}

int
bcmbd_fifodma_pop(int unit, int chan, uint32_t timeout, uint32_t *ptr, uint32_t *num)
{
    fifodma_ctrl_t *ctrl = fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->chan_num || !ptr || !num) {
        return SHR_E_PARAM;
    }

    fifo = &ctrl->fifo[chan];
    if (!fifo->started) {
        return SHR_E_UNAVAIL;
    }

    if (timeout) {
        sal_usleep(timeout);
    }

    return ctrl->ops->fifodma_pop(ctrl, chan, ptr, num);
}

