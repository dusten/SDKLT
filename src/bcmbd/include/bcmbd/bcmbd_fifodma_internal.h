/*! \file bcmbd_fifodma_internal.h
 *
 * FIFODMA APIs and data types used only internally.
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

#ifndef BCMBD_FIFODMA_INTERNAL_H
#define BCMBD_FIFODMA_INTERNAL_H

#include <sal/sal_sem.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_thread.h>
#include <bcmbd/bcmbd_fifodma.h>

/*! Maximum number of FIFODMA channels */
#define FIFODMA_CHAN_NUM_MAX    12

/*! FIFODMA thread priority */
#define FIFODMA_THREAD_PRI      SAL_THREAD_PRIO_DEFAULT

/*!
 * \brief FIFODMA channel.
 */
typedef struct fifodma_chan_s {
    /*! Host buffer physical address */
    uint64_t buf_paddr;

    /*! Host buffer memory address */
    void *buf_addr;

    /*! Cache memory address */
    void *buf_cache;

    /*! Number of host buffer entries */
    uint32_t num_entries;

    /*! Entry width in words */
    uint32_t data_width;

    /*! Memory size in bytes */
    uint32_t buf_size;

    /*! Ready entry pointer */
    uint32_t ptr;

    /*! Interrupt spinlock */
    sal_spinlock_t lock;

    /*! Channel is started */
    int started;

    /*! Interrupt is enabled */
    int intr_enabled;

    /*! Interrupt is active */
    int intr_active;
} fifodma_chan_t;

struct fifodma_ctrl_s;

/*! Handle FIFODMA */
typedef void (*fifodma_handle_t)(struct fifodma_ctrl_s *ctrl, int chan);
/*! Start FIFODMA */
typedef int (*fifodma_start_t)(struct fifodma_ctrl_s *ctrl, int chan);
/*! Stop FIFODMA */
typedef int (*fifodma_stop_t)(struct fifodma_ctrl_s *ctrl, int chan);
/*! Pop FIFODMA */
typedef int (*fifodma_pop_t)(struct fifodma_ctrl_s *ctrl, int chan,
                             uint32_t *ptr, uint32_t *num);

/*!
 * \brief Exported operation functions.
 */
typedef struct fifodma_ops_s {
    /*! Handle FIFODMA */
    fifodma_handle_t    fifodma_handle;

    /*! Start FIFODMA */
    fifodma_start_t     fifodma_start;

    /*! Stop FIFODMA */
    fifodma_stop_t      fifodma_stop;

    /*! Pop FIFODMA */
    fifodma_pop_t       fifodma_pop;
} fifodma_ops_t;

/*! Notify FIFODMA interrupt */
typedef void (*fifodma_intr_notify_t)(int unit, int chan);

/*!
 * \brief FIFODMA control.
 */
typedef struct fifodma_ctrl_s {
    /*! Device number */
    int unit;

    /*! Channel number */
    int chan_num;

    /*! FIFOs */
    fifodma_chan_t fifo[FIFODMA_CHAN_NUM_MAX];

    /*! FIFODMA work */
    bcmbd_fifodma_work_t work[FIFODMA_CHAN_NUM_MAX];

    /*! Interrupt handler thread ID */
    sal_thread_t pid;

    /*! Interrupt synchronous semaphore */
    sal_sem_t intr;

    /*! Pointer to the exported funtions structure */
    fifodma_ops_t *ops;

    /*! Notify interrupt */
    fifodma_intr_notify_t intr_notify;

    /*! Device active indicator */
    int active;
} fifodma_ctrl_t;

#endif /* BCMBD_FIFODMA_INTERNAL_H */

