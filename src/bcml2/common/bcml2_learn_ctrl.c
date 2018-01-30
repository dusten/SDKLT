/*! \file bcml2_learn_ctrl.c
 *
 * L2 Learn Control sublayer.
 * This file implements the L2 Learn control interfaces
 * to the other sublayers in L2 Learn.
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
#include <sal/sal_msgq.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcml2/common/bcml2_learn_ctrl.h>
#include <bcml2/common/bcml2_learn_drv.h>
#include <bcml2/common/bcml2_learn_imm.h>
#include <bcml2/common/bcml2_transform.h>

/******************************************************************************
* Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCML2_LEARN_CTRL
#define L2_LEARN_THREAD_EXIT_TIMEOUT     10000000
#define L2_LEARN_POLLING_INTERVAL        200000
/*
 * L2 Learn control structure.
 */
typedef struct l2_lrn_msg_s {
    enum {
        LEARN_INTR, /* Messgage to notify L2 learn event interrupt. */
        KILL_THREAD /* Message to kill L2 Learn thread from applications. */
    } msg_type;
    void *value;
} l2_lrn_msg_t;

typedef struct bcml2_learn_ctrl_s {
    /* L2 Learn thread name. */
    char            name[THREAD_NAME_LEN_MAX];

    /* L2 learn thread ID. */
    sal_thread_t    pid;

    /* L2 learn thread running indicator. */
    bool            running;

    /* L2 learn thread runs in polling mode. */
    bool            polling;

    /* L2 learn thread semphore used for polling mode. */
    sal_sem_t       notify;

    /*
     * Message queue for
     * 1. Users applications to kill L2 Learn thread.
     * 2. L2 Learn event interrupt from HW.
     */
    sal_msgq_t   l2_lrn_queue;
} bcml2_learn_ctrl_t;

/* L2 learn control database. */
static bcml2_learn_ctrl_t *bcml2_learn_ctrl[BCML2_DEV_NUM_MAX] = {NULL};

/******************************************************************************
* Private functions
 */
/*!
  * \brief L2 Learn thread.
  *          It reports MAC addresses learnt in HW to L2_LEARN_DATA LT.
  * \param [in] arg Unit number.
  *
  * \retval None.
  */
static void
bcml2_learn_thread(void *arg)
{
    int unit = (unsigned long)(arg);
    bcml2_learn_ctrl_t *learn_ctrl = bcml2_learn_ctrl[unit];
    l2_lrn_msg_t msg;
    int rv;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "L2 learn thread starts.\n")));
    learn_ctrl->running = TRUE;

    if (learn_ctrl->polling) {
        sal_memset(&msg, 0, sizeof(l2_lrn_msg_t));
        msg.msg_type = LEARN_INTR;
        rv = sal_msgq_post(learn_ctrl->l2_lrn_queue, (void *)(&msg),
                           SAL_MSGQ_NORMAL_PRIORITY, SAL_MSGQ_NOWAIT);
        if (rv != SHR_E_NONE) {
            goto exit;
        }
    }

    while (learn_ctrl->running) {
        sal_memset(&msg, 0, sizeof(l2_lrn_msg_t));
        rv = sal_msgq_recv(learn_ctrl->l2_lrn_queue, &msg, SAL_MSGQ_FOREVER);
        if (rv != SHR_E_NONE) {
            continue;
        }

        if (msg.msg_type == KILL_THREAD) {
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "L2 Learn thread receives KILL msg.\n")));
            (void)bcml2_learn_drv_cache_traverse(unit);
            break;
        }

        if (learn_ctrl->polling) {
            if (learn_ctrl->notify == NULL) {
                continue;
            }
            (void)bcml2_learn_drv_cache_traverse(unit);
            sal_sem_take(learn_ctrl->notify, L2_LEARN_POLLING_INTERVAL);

            sal_memset(&msg, 0, sizeof(l2_lrn_msg_t));
            msg.msg_type = LEARN_INTR;
            rv = sal_msgq_post(learn_ctrl->l2_lrn_queue, (void *)(&msg),
                                SAL_MSGQ_NORMAL_PRIORITY, SAL_MSGQ_NOWAIT);
            if (rv != SHR_E_NONE) {
                goto exit;
            }
            continue;
        } else {
            /* Interrupt mode. */
            if (msg.msg_type == LEARN_INTR) {
                (void)bcml2_learn_drv_cache_traverse(unit);
            } else {
                LOG_WARN(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                        "L2 Learn thread receives error msg.\n")));
                continue;
            }
        }
    }

exit:
    LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "L2 Learn thread exits.\n")));
    learn_ctrl->pid = SAL_THREAD_ERROR;
    learn_ctrl->running = FALSE;
    sal_thread_exit(0);
    return;
}

/*!
  * \brief Get L2 Learn related LT map status.
  *
  * \param [in] arg Unit number.
  * \param [in] mapped Map status
  *
  * \retval None.
  */
static int
l2_learn_map_status_get(int unit, bool *mapped)
{
    const bcmlrd_map_t *map = NULL;
    int  rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    *mapped = FALSE;

    rv = bcmlrd_map_get(unit, L2_LEARN_DATAt, &map);
    if (!SHR_SUCCESS(rv)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    rv = bcmlrd_map_get(unit, L2_LEARN_CONTROLt, &map);
    if (!SHR_SUCCESS(rv)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    *mapped = TRUE;

exit:
    SHR_FUNC_EXIT();
}
/******************************************************************************
 * Public Functions
 */
/*!
  * \brief API to enable L2 Learn.
  *
  * \param [in] unit Unit number.
  * \param [in] enable Enable indicator.
  *
  * \retval SHR_E_NONE Success.
  * \retval !SHR_E_NONE Failure.
  */
int
bcml2_learn_ctrl_report_enable(int unit, bool enable)
{
    bcml2_learn_ctrl_t *learn_ctrl = bcml2_learn_ctrl[unit];
    l2_lrn_msg_t msg;
    bool mapped = FALSE;
    int   rv;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = L2_LEARN_THREAD_EXIT_TIMEOUT;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(l2_learn_map_status_get(unit, &mapped));
    if (!mapped) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }


    SHR_NULL_CHECK(learn_ctrl, SHR_E_INIT);

    if (enable) {
        /* L2 learn thread is already running. */
        if (learn_ctrl->running || (learn_ctrl->pid != SAL_THREAD_ERROR)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }

        learn_ctrl->pid = sal_thread_create(learn_ctrl->name,
                                            SAL_THREAD_STKSZ,
                                            L2_LEARN_THREAD_PRI,
                                            bcml2_learn_thread,
                                            (void *)(unsigned long)(unit));

        if (learn_ctrl->pid == SAL_THREAD_ERROR) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }

        SHR_IF_ERR_EXIT(bcml2_learn_drv_intr_enable(unit, 1));
        SHR_IF_ERR_EXIT(bcml2_learn_drv_hw_enable(unit, 1));
    } else if (learn_ctrl->pid != SAL_THREAD_ERROR) {
        SHR_IF_ERR_EXIT(bcml2_learn_drv_hw_enable(unit, 0));
        SHR_IF_ERR_EXIT(bcml2_learn_drv_intr_enable(unit, 0));

        sal_memset(&msg, 0, sizeof(l2_lrn_msg_t));
        msg.msg_type = KILL_THREAD;

        rv = sal_msgq_post(learn_ctrl->l2_lrn_queue, (void *)(&msg),
                           SAL_MSGQ_NORMAL_PRIORITY, SAL_MSGQ_NOWAIT);
        if (rv != SHR_E_NONE) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (learn_ctrl->polling) {
            SHR_IF_ERR_EXIT(sal_sem_give(learn_ctrl->notify));
        }

        shr_timeout_init(&to, timeout_usec, 0);
        while (learn_ctrl->running) {
            if (shr_timeout_check(&to)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "L2 Learn thread does not exit\n")));
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy L2 Learn control database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */int
bcml2_learn_ctrl_cleanup(int unit)
{
    bcml2_learn_ctrl_t *learn_ctrl = bcml2_learn_ctrl[unit];
    bool mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(l2_learn_map_status_get(unit, &mapped));
    if (!mapped) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_NULL_CHECK(learn_ctrl, SHR_E_NONE);

    if (learn_ctrl) {
        if(learn_ctrl->running) {
            shr_timeout_t to;
            sal_usecs_t timeout_usec = L2_LEARN_THREAD_EXIT_TIMEOUT;

            SHR_IF_ERR_EXIT(bcml2_learn_ctrl_report_enable(unit, 0));

            shr_timeout_init(&to, timeout_usec, 0);
            while (learn_ctrl->running) {
                if (shr_timeout_check(&to)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "L2 Learn thread does not exit\n")));
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
            }
        }

        if (learn_ctrl->l2_lrn_queue) {
            sal_msgq_destroy(learn_ctrl->l2_lrn_queue);
        }
        if (learn_ctrl->notify != NULL) {
            sal_sem_destroy(learn_ctrl->notify);
            learn_ctrl->notify = NULL;
        }
        SHR_FREE(bcml2_learn_ctrl[unit]);
    }

    SHR_IF_ERR_EXIT(bcml2_learn_drv_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize L2 Learn control database.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
int
bcml2_learn_ctrl_init(int unit, bool warm)
{
    bcml2_learn_ctrl_t *learn_ctrl = NULL;
    bool mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    if (bcml2_learn_ctrl[unit] != NULL) {
        SHR_IF_ERR_EXIT(bcml2_learn_ctrl_cleanup(unit));
    }

    SHR_IF_ERR_EXIT(l2_learn_map_status_get(unit, &mapped));
    if (!mapped) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_ALLOC(learn_ctrl, sizeof(bcml2_learn_ctrl_t), "bcml2_learn_ctrl");
    SHR_NULL_CHECK(learn_ctrl, SHR_E_MEMORY);

    /* Init bcml2 control info */
    sal_memset(learn_ctrl, 0, sizeof(bcml2_learn_ctrl_t));
    learn_ctrl->pid = SAL_THREAD_ERROR;

    learn_ctrl->l2_lrn_queue = NULL;
    learn_ctrl->l2_lrn_queue = sal_msgq_create(sizeof(l2_lrn_msg_t),
                                               MAX_PENDING_LEARN_MSG,
                                               "L2 Learn msg queue");
    SHR_NULL_CHECK(learn_ctrl->l2_lrn_queue, SHR_E_MEMORY);

    sal_snprintf(learn_ctrl->name, THREAD_NAME_LEN_MAX, "%s", "L2_LEARN");

    
    learn_ctrl->polling = TRUE;

    if (learn_ctrl->polling) {
        learn_ctrl->notify = sal_sem_create("L2_notify", SAL_SEM_BINARY, 0);
        SHR_NULL_CHECK(learn_ctrl->notify, SHR_E_MEMORY);
    }

    bcml2_learn_ctrl[unit] = learn_ctrl;
    SHR_IF_ERR_EXIT(
        bcml2_learn_imm_register(unit));

    SHR_IF_ERR_EXIT(bcml2_learn_drv_attach(unit));
    SHR_IF_ERR_EXIT(bcml2_learn_drv_init(unit, warm));

exit:
    if (SHR_FUNC_ERR()) {
        if (learn_ctrl && learn_ctrl->l2_lrn_queue) {
            sal_msgq_destroy(learn_ctrl->l2_lrn_queue);
        }

        if (learn_ctrl) {
            SHR_FREE(learn_ctrl);
        }
        bcml2_learn_ctrl[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete HW cache entry where MAC is learnt from.
 *
 * Delete HW cache entry where MAC is learnt from.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] hw_cache HW cache info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L2 Learn control is not initialized.
 * \retval SHR_E_INTERNAL Fail to delete the hw cache entry.
 */
int
bcml2_learn_ctrl_cache_entry_del(int unit,
                                 uint32_t trans_id,
                                 l2_learn_addr_t * l2addr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcml2_learn_drv_entry_delete(unit, trans_id, l2addr));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Notify HW learn to L2 Learn thread .
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Fail to notify interrupts.
 */
int
bcml2_learn_ctrl_intr_notify(int unit)
{
    l2_lrn_msg_t msg;
    bcml2_learn_ctrl_t *learn_ctrl = bcml2_learn_ctrl[unit];
    int rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(l2_lrn_msg_t));
    msg.msg_type = LEARN_INTR;

    rv = sal_msgq_post(learn_ctrl->l2_lrn_queue, (void *)(&msg),
                       SAL_MSGQ_NORMAL_PRIORITY, SAL_MSGQ_NOWAIT);
    if (rv != SHR_E_NONE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}
