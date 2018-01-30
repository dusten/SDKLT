/*! \file sal_rwlock.c
 *
 * Read/write lock API.
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
#include <sal/sal_rwlock.h>

#define SAL_RWLOCK_SIGNATURE   0x8482a9d0

/* Local abstract type built on the POSIX rwlock */
typedef struct rwlock_ctrl_s {
    uint32_t signature;
    pthread_rwlock_t rwlock;
    const char *desc;
} rwlock_ctrl_t;

static int
timed_calc(uint32_t usec, struct timespec *ts)
{
#if defined(CLOCK_REALTIME)
    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        return -1;
    }
#else
    /* we use time if realtime clock is not available */
    ts->tv_sec = time(NULL)+1; /* round up fractions of sec */
#endif

    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;
    ts->tv_nsec += (usec % SECOND_USEC) * 1000;
    /* currect overflow - if occured */
    if (ts->tv_nsec > SECOND_NSEC) {
        /* can only have overflow of 1 sec */
        ts->tv_sec++;
        ts->tv_nsec -= SECOND_NSEC;
    }
    return 0;
}

sal_rwlock_t
sal_rwlock_create(const char *desc)
{
    rwlock_ctrl_t *mc;

    if ((mc = malloc(sizeof(rwlock_ctrl_t))) == NULL) {
        return NULL;
    }

    mc->desc = desc;   /* save the name for debug usage */
    mc->signature = SAL_RWLOCK_SIGNATURE;
    /* initialize with the default attributes */
    if (pthread_rwlock_init(&mc->rwlock, NULL) != 0) {
        free(mc);
        return NULL;
    }

    return (sal_rwlock_t)mc;
}

void
sal_rwlock_destroy(sal_rwlock_t rwlock)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return;
    }
    pthread_rwlock_destroy(&mc->rwlock);
    free(mc);
}

int
sal_rwlock_rlock(sal_rwlock_t rwlock, uint32_t usec)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    int rv = -1;
    struct timespec ts;

    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return -1;
    }
    if (usec == SAL_RWLOCK_FOREVER) {
        rv = pthread_rwlock_rdlock(&mc->rwlock);
    } else if (usec == SAL_RWLOCK_NOWAIT) {
        rv = pthread_rwlock_tryrdlock(&mc->rwlock);
    } else {
        if (timed_calc(usec, &ts) == 0) {
            rv = pthread_rwlock_timedrdlock(&mc->rwlock, &ts);
        }
    }
    return (rv != 0 ? -1 : 0);
}

int
sal_rwlock_wlock(sal_rwlock_t rwlock, uint32_t usec)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    int rv = -1;
    struct timespec ts;

    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return -1;
    }
    if (usec == SAL_RWLOCK_FOREVER) {
        rv = pthread_rwlock_wrlock(&mc->rwlock);
    } else if (usec == SAL_RWLOCK_NOWAIT) {
        rv = pthread_rwlock_trywrlock(&mc->rwlock);
    } else {
        if (timed_calc(usec, &ts) == 0) {
            rv = pthread_rwlock_timedwrlock(&mc->rwlock, &ts);
        }
    }
    return (rv != 0 ? -1 : 0);
}

int
sal_rwlock_give(sal_rwlock_t rwlock)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    int rv;

    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return -1;
    }
    rv = pthread_rwlock_unlock(&mc->rwlock);
    return (rv != 0 ? -1 : 0);
}

