/*! \file sal_mutex.c
 *
 * Mutex API.
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

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <sal/sal_time.h>
#include <sal/sal_sleep.h>
#include <sal/sal_mutex.h>

/* Default configuration */
#ifndef SAL_MUTEX_USE_POSIX_TIMEDLOCK
#  if defined(_POSIX_TIMERS) && (_POSIX_TIMERS >= 200112L)
#    if defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS >= 200112L)
#      if defined(CLOCK_REALTIME)
#        define SAL_MUTEX_USE_POSIX_TIMEDLOCK   1
#      endif
#    endif
#  endif
#endif
#ifndef SAL_MUTEX_USE_POSIX_TIMEDLOCK
#  define SAL_MUTEX_USE_POSIX_TIMEDLOCK         0
#endif

/* Optionally dump configuration */
#include <sal/sal_internal.h>
#if SAL_INT_DUMP_CONFIG
#pragma message(SAL_INT_VAR_VALUE(SAL_MUTEX_USE_POSIX_TIMEDLOCK))
#endif


/* Local abstract type built on the POSIX mutex */
typedef struct mutex_ctrl_s {
    pthread_mutex_t mutex;
    char *desc;
} mutex_ctrl_t;


#if (SAL_MUTEX_USE_POSIX_TIMEDLOCK)
static int
_timed_mutex_take(mutex_ctrl_t *mc, int usec)
{
    struct timespec timespec, *ts = &timespec;
    time_t sec;
    long nsec;

    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        return -1;
    }

    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;

    /* Compute new nsec */
    nsec = ts->tv_nsec + (usec % SECOND_USEC) * 1000;

    /* Detect and handle rollover */
    if (nsec < 0) {
        ts->tv_sec += 1;
        nsec -= SECOND_NSEC;
    }
    ts->tv_nsec = nsec;

    /* Normalize if needed */
    sec = ts->tv_nsec / SECOND_NSEC;
    if (sec) {
        ts->tv_sec += sec;
        ts->tv_nsec = ts->tv_nsec % SECOND_NSEC;
    }

    /* Treat EAGAIN as a fatal error */
    return pthread_mutex_timedlock(&mc->mutex, ts);
}
#else
static int
_timed_mutex_take(mutex_ctrl_t *mc, int usec)
{
    int time_wait = 1;
    int rv = 0;

    /* Retry algorithm with exponential backoff */
    for (;;) {
        rv = pthread_mutex_trylock(&mc->mutex);

        if (rv != EBUSY) {
            /* Done (or error other than EBUSY) */
            break;
        }

        if (time_wait > usec) {
            time_wait = usec;
        }

        sal_usleep(time_wait);

        usec -= time_wait;

        if (usec == 0) {
            rv = ETIMEDOUT;
            break;
        }

        if ((time_wait *= 2) > 100000) {
            time_wait = 100000;
        }
    }

    return (rv != 0) ? -1 : 0;
}
#endif

sal_mutex_t
sal_mutex_create(char *desc)
{
    mutex_ctrl_t *mc;
    pthread_mutexattr_t attr;

    if ((mc = malloc(sizeof (mutex_ctrl_t))) == NULL) {
	return NULL;
    }

    mc->desc = desc;
    pthread_mutexattr_init(&attr);

    pthread_mutex_init(&mc->mutex, &attr);

    return (sal_mutex_t)mc;
}

void
sal_mutex_destroy(sal_mutex_t mtx)
{
    mutex_ctrl_t *mc = (mutex_ctrl_t *)mtx;

    assert(mc);

    pthread_mutex_destroy(&mc->mutex);

    free(mc);
}

int
sal_mutex_take(sal_mutex_t mtx, int usec)
{
    mutex_ctrl_t *mc = (mutex_ctrl_t *)mtx;
    int rv = 0;

    assert(mc);

    if (usec == SAL_MUTEX_FOREVER) {
        do {
            rv = pthread_mutex_lock(&mc->mutex);
        } while (rv != 0 && errno == EINTR);
    } else {
        rv = _timed_mutex_take(mc, usec);
    }

    return (rv != 0) ? -1 : 0;
}

int
sal_mutex_give(sal_mutex_t mtx)
{
    mutex_ctrl_t *mc = (mutex_ctrl_t *)mtx;
    int	rv;

    assert(mc);

    rv = pthread_mutex_unlock(&mc->mutex);

    return (rv != 0) ? -1 : 0;
}
