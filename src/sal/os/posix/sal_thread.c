/*! \file sal_thread.c
 *
 * Thread API.
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
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/param.h>
#include <sys/prctl.h>

#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>

static pthread_mutex_t _sal_thread_lock = PTHREAD_MUTEX_INITIALIZER;

#define THREAD_LOCK() pthread_mutex_lock(&_sal_thread_lock)
#define THREAD_UNLOCK() pthread_mutex_unlock(&_sal_thread_lock)

/*
 * PTHREAD_STACK_MIN must be defined to avoid stack corruption.
 * Normally this is accomplished by including sys/param.h.
 */
#ifndef PTHREAD_STACK_MIN
#  ifdef PTHREAD_STACK_SIZE
#    define PTHREAD_STACK_MIN PTHREAD_STACK_SIZE
#  else
#    error PTHREAD_STACK_MIN not defined
#  endif
#endif

#ifndef SAL_THREAD_NAME_MAX
#define SAL_THREAD_NAME_MAX             32
#endif

typedef struct thread_info_s {
    char name[SAL_THREAD_NAME_MAX];
    void (*func)(void *);
    void *arg;
    pthread_t id;
    int stksz;
    sal_sem_t sem;
    struct thread_info_s *next;
} thread_info_t;

#ifndef SAL_THREAD_DATA_MAX
#define SAL_THREAD_DATA_MAX             8
#endif

typedef struct {
    pthread_key_t key;
    int mapped;
} thread_data_ctrl_t;

static thread_info_t *thread_head = NULL;
static thread_data_ctrl_t thread_data[SAL_THREAD_DATA_MAX];

static void *
thread_boot(void *ti_void)
{
    thread_info_t *ti = ti_void;
    void (*func)(void *);
    void *arg;

    /* Ensure that we give up all resources upon exit */
    pthread_detach(pthread_self());

#ifdef PR_SET_NAME
    prctl(PR_SET_NAME, ti->name, 0, 0, 0);
#endif

    func = ti->func;
    arg = ti->arg;

    ti->id = pthread_self();

    /* Notify parent to continue */
    sal_sem_give(ti->sem);

    /* Call thread function */
    (*func)(arg);

    /* Thread function did not call sal_thread_exit() */
    sal_thread_exit(0);

    /* Will never get here */
    return NULL;
}

sal_thread_t
sal_thread_create(char *name, int stksz, int prio,
                  sal_thread_func_f func, void *arg)
{
    pthread_attr_t attribs;
    struct sched_param param;
    thread_info_t *ti;
    pthread_t id;
    sal_sem_t sem;

    if (pthread_attr_init(&attribs)) {
        return SAL_THREAD_ERROR;
    }

    stksz += PTHREAD_STACK_MIN;
    pthread_attr_setstacksize(&attribs, stksz);

    if (prio == SAL_THREAD_PRIO_NO_PREEMPT) {
        pthread_attr_setinheritsched(&attribs, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attribs, SCHED_FIFO);
        param.sched_priority = SAL_THREAD_RT_PRIO_HIGHEST;
        pthread_attr_setschedparam(&attribs, &param);
    }

    if ((ti = malloc(sizeof (*ti))) == NULL) {
	return SAL_THREAD_ERROR;
    }

    if ((sem = sal_sem_create("threadBoot", 1, 0)) == NULL) {
	free(ti);
	return SAL_THREAD_ERROR;
    }
    strncpy(ti->name, name, sizeof(ti->name));
    ti->name[sizeof(ti->name) - 1] = 0;
    ti->func = func;
    ti->arg = arg;
    ti->id = (pthread_t)0;
    ti->stksz = stksz;
    ti->sem = sem;

    THREAD_LOCK();
    ti->next = thread_head;
    thread_head = ti;
    THREAD_UNLOCK();

    if (pthread_create(&id, &attribs, thread_boot, (void *)ti)) {
        THREAD_LOCK();
	thread_head = thread_head->next;
        THREAD_UNLOCK();
	free(ti);
        sal_sem_destroy(sem);
	return(SAL_THREAD_ERROR);
    }

    /*
     * Note that at this point ti can no longer be safely
     * dereferenced, as the thread we just created may have exited
     * already. Instead we wait for the new thread to update
     * thread_info_t and tell us to continue.
     */
    sal_sem_take(sem, SAL_SEM_FOREVER);
    sal_sem_destroy(sem);

    return ((sal_thread_t)id);
}

sal_thread_t
sal_thread_self(void)
{
    return (sal_thread_t)pthread_self();
}

char *
sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size)
{
    thread_info_t *ti;
    char *name;

    name = NULL;

    THREAD_LOCK();
    for (ti = thread_head; ti != NULL; ti = ti->next) {
	if (ti->id == (pthread_t)thread) {
	    strncpy(thread_name, ti->name, thread_name_size);
	    thread_name[thread_name_size - 1] = 0;
	    name = thread_name;
            break;
	}
    }
    THREAD_UNLOCK();

    if (name == NULL) {
        thread_name[0] = 0;
    }

    return name;
}

void
sal_thread_exit(int rc)
{
    thread_info_t *ti, **tp;
    pthread_t id = pthread_self();

    ti = NULL;

    THREAD_LOCK();
    for (tp = &thread_head; (*tp) != NULL; tp = &(*tp)->next) {
	if ((*tp)->id == id) {
	    ti = (*tp);
	    (*tp) = (*tp)->next;
	    break;
	}
    }
    THREAD_UNLOCK();

    if (ti) {
        free(ti);
    }

    pthread_exit((void *)(unsigned long)rc);
}

void
sal_thread_yield(void)
{
    sal_usleep(1);
}

void
sal_thread_console_set(sal_thread_t thread)
{
}

sal_thread_t
sal_thread_console_get(void)
{
    return SAL_THREAD_ERROR;
}


extern sal_thread_data_t *
sal_thread_data_create(sal_thread_data_destroy_f destructor)
{
    int i;

    THREAD_LOCK();
    for (i = 0; i < SAL_THREAD_DATA_MAX; i++) {
        if (!thread_data[i].mapped) {
            thread_data[i].mapped = 1;
            break;
        }
    }
    THREAD_UNLOCK();

    if (i >= SAL_THREAD_DATA_MAX) {
        return NULL;
    }
    if (pthread_key_create(&thread_data[i].key, destructor) != 0) {
    	thread_data[i].mapped = 0;
        return NULL;
    }
    return (void *)&thread_data[i].key;
}

int
sal_thread_data_set(sal_thread_data_t *tdata, void *val)
{
    if (tdata == NULL) {
        return 0;
    }
     return pthread_setspecific(*(pthread_key_t *)tdata, val);
}

void *
sal_thread_data_get(sal_thread_data_t *tdata)
{
    if (tdata == 0) {
        return NULL;
    }
    return pthread_getspecific(*(pthread_key_t *)tdata);
}

int
sal_thread_data_delete(sal_thread_data_t *tdata)
{
    if (tdata == NULL) {
        return 0;
    }
    return pthread_key_delete(*(pthread_key_t *)tdata);
}
