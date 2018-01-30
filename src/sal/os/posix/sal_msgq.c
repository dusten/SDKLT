/*! \file sal_msgq.c
 *
 * Message queue API.
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
#include <sal/sal_msgq.h>

#define SAL_MSGQ_SIGNATURE   0x8532b9d0

typedef struct basic_element_s {
    struct basic_element_s *next;
    uint8_t data[];
} basic_element_t;

typedef struct {
    basic_element_t *head;
    basic_element_t *tail;
} pri_list_t;

typedef struct rwlock_ctrl_s {
    uint32_t signature;
    size_t element_size;
    size_t num_of_element;
    size_t max_elements;
    uint8_t *buf;
    pri_list_t pri_lists[SAL_MSGQ_PRIORITIES];
    basic_element_t *free_element_list;
    pthread_mutex_t q_lock_mutex;
    pthread_mutex_t recv_mutex;
    pthread_mutex_t post_mutex;
    pthread_cond_t recv_cond;
    pthread_cond_t post_cond;
    uint32_t    pending_to_post;
    uint32_t    pending_to_recv;
    const char *desc;
} rwlock_ctrl_t;


sal_msgq_t
sal_msgq_create(size_t element_size, size_t max_elements, const char *desc)
{
    rwlock_ctrl_t *mq;
    size_t actual_element_size = element_size + sizeof(void *);

    mq = (rwlock_ctrl_t *)malloc(sizeof(rwlock_ctrl_t));
    if (!mq) {
        return NULL;
    }
    memset(mq, 0, sizeof(*mq));
    do {
        int j;
        basic_element_t *p_element;
        mq->free_element_list = (basic_element_t *)
            malloc(actual_element_size * max_elements);
        if (!mq->free_element_list) {
            break;
        }
        mq->buf = (uint8_t *)mq->free_element_list;  /* Keep pointer to free the memory */
        for (j = 0, p_element = mq->free_element_list;
             j < max_elements-1;
             j++) {
            p_element->next = (basic_element_t *)((uint8_t *)p_element + actual_element_size);
            p_element = p_element->next;
        }
        p_element->next = NULL; /* Terminate the list */

        for (j = 0; j < SAL_MSGQ_PRIORITIES; j++) {
            mq->pri_lists[j].head = NULL;
            mq->pri_lists[j].tail = NULL;
        }
        if ((0 != pthread_mutex_init(&mq->q_lock_mutex, NULL)) ||
            (0 != pthread_mutex_init(&mq->recv_mutex, NULL)) ||
            (0 != pthread_mutex_init(&mq->post_mutex, NULL))) {
            break;
        }
        if (0 != pthread_cond_init(&mq->recv_cond, NULL)) {
            break;
        }
        if (0 != pthread_cond_init(&mq->post_cond, NULL)) {
            break;
        }
        mq->desc = desc;
        mq->element_size = element_size;
        mq->max_elements = max_elements;
        mq->num_of_element = 0;
        mq->pending_to_post = 0;
        mq->pending_to_recv = 0;
        mq->signature = SAL_MSGQ_SIGNATURE;
        return (sal_msgq_t)mq;
    } while (0);

    /* Free all allocated resources and return failure */
    if (mq->buf) {
        free (mq->buf);
    }
    if (*(uint32_t *)(&mq->q_lock_mutex)) {
        pthread_mutex_destroy(&mq->q_lock_mutex);
    }
    if (*(uint32_t *)(&mq->recv_mutex)) {
        pthread_mutex_destroy(&mq->recv_mutex);
    }
    if (*(uint32_t *)(&mq->post_mutex)) {
        pthread_mutex_destroy(&mq->post_mutex);
    }
    if (*(uint32_t *)(&mq->recv_cond)) {
        pthread_cond_destroy(&mq->recv_cond);
    }
    if (*(uint32_t *)(&mq->post_cond)) {
        pthread_cond_destroy(&mq->post_cond);
    }
    free(mq);
    return NULL;
}

void
sal_msgq_destroy(sal_msgq_t msgq_hdl)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE)) {
        return;
    }

    /* Release Q resources */
    pthread_cond_destroy(&mq->recv_cond);
    pthread_cond_destroy(&mq->post_cond);
    pthread_mutex_destroy(&mq->q_lock_mutex);
    pthread_mutex_destroy(&mq->recv_mutex);
    pthread_mutex_destroy(&mq->post_mutex);
    free (mq->buf);
    free (mq);
}
static int
_timed_calc(uint32_t usec, struct timespec *ts)
{
#if defined(CLOCK_REALTIME)
    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        return -1;
    }
#else
    /* Use time if realtime clock is not available */
    ts->tv_sec = time(NULL)+1; /* round up fractions of sec */
#endif

    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;
    ts->tv_nsec += (usec % SECOND_USEC) * 1000;
    /* Currect overflow - if occured */
    if (ts->tv_nsec > SECOND_NSEC) {
        /* Can only have overflow of 1 sec */
        ts->tv_sec++;
        ts->tv_nsec -= SECOND_NSEC;
    }
    return 0;
}

int
sal_msgq_post(sal_msgq_t msgq_hdl,
              void *element,
              sal_msgq_priority_t pri,
              uint32_t usec)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;
    int rv;
    basic_element_t *p_element;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE) ||
        (pri < SAL_MSGQ_MIN_PRIORITY) || (pri > SAL_MSGQ_MAX_PRIORITY)) {
        return SAL_MSGQ_E_INVALID_PARAM;
    }

    /* Lock the queue */
    if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
        rv = __LINE__;
        return -rv;
    }
    /* Is the queue full or other pending ? */
    if (mq->num_of_element == mq->max_elements) {
        int ret_status;

        /* Need to wait until there is room */
        if (usec == SAL_MSGQ_NOWAIT) { /* Return error if can't wait */
            pthread_mutex_unlock(&mq->q_lock_mutex);
            return SAL_MSGQ_E_TIMEOUT;
        }
        mq->pending_to_post++;

post_retry:
        rv = pthread_mutex_lock(&mq->post_mutex);
        if (rv != 0) {
            mq->pending_to_post--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            rv = __LINE__;
            return -rv;
        }
        pthread_mutex_unlock(&mq->q_lock_mutex);

        /*
         * This can be critical place. If we get stuck here (task switch) and
         * the recv threads depleting the queue then nobody will signal the
         * condition variable ==> deadlock. For this reason, we make sure that
         * the recv thread locks the post mutex before signaling. Since at this
         * point we have the post_mutex, the recv thread will block and we
         * can get into the condition function which will unlock the post_mutex
        */
        ret_status = -2;
        if (usec == SAL_MSGQ_FOREVER) {
            rv = pthread_cond_wait(&mq->post_cond, &mq->post_mutex);
        } else  {
            struct timespec ts;
            int loop_count = 0;
            do {
                _timed_calc(usec, &ts);

                rv = pthread_cond_timedwait(&mq->post_cond,
                                            &mq->post_mutex, &ts);
                assert ((rv == 0) || (rv == ETIMEDOUT));
                if (rv == ETIMEDOUT) {
                    ret_status = SAL_MSGQ_E_TIMEOUT;
                }
            } while (rv == EINVAL && loop_count++ < 1);
        }
        /* Unlock the mutex */
        pthread_mutex_unlock(&mq->post_mutex);

        /* Lock the queue again */
        if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
            rv = __LINE__;
            return -rv;
        }
        if (rv != 0) {  /* If failed condition wait */
            mq->pending_to_post--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            return ret_status;
        }
        /* It is possible, on multiple cores, that multiple threads will
         * wake up from a single condition signal (according to the
         * documentation). So check that everything is okay
         */
        if (mq->num_of_element == mq->max_elements) {
            goto post_retry;
        }
        mq->pending_to_post--;
    }
    /* When we here, the queue is locked and there is room */
    assert (mq->num_of_element < mq->max_elements);
    assert (mq->free_element_list != NULL);

    /* Get an element from the free list */
    p_element = mq->free_element_list;
    mq->free_element_list = p_element->next;
    p_element->next = NULL;

    /* Copy the data into the element */
    memcpy(p_element->data, element, mq->element_size);

    /* Attach the element to the tail of the list (based on its priority) */
    if (mq->pri_lists[pri].head) {    /* If the list is not empty */
        mq->pri_lists[pri].tail->next = p_element;  /* Attach to the tail end*/
    }  else {
        mq->pri_lists[pri].head = p_element;
    }
    mq->pri_lists[pri].tail = p_element;  /* Set the tail to be the new element */

    mq->num_of_element++;

    if (mq->pending_to_recv > 0) {   /* If someone waiting for recv */
        pthread_mutex_lock(&mq->recv_mutex);
        rv = pthread_cond_signal(&mq->recv_cond); /* Wake up pending recv threads */
        assert(rv == 0);
        pthread_mutex_unlock(&mq->recv_mutex);
    }

    /* Unlock the queue and return */
    pthread_mutex_unlock(&mq->q_lock_mutex);
    return 0;
}

int
sal_msgq_recv(sal_msgq_t msgq_hdl, void *element, uint32_t usec)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;
    int rv;
    basic_element_t *p_element;
    int j;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE)) {
        return SAL_MSGQ_E_INVALID_PARAM;
    }

    /* Lock the queue */
    if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
        return -1;
    }
    /* Is the queue empty or others are waiting to receive ? */
    if (mq->num_of_element == 0) {
        int ret_status;

        /* Need to wait, does the caller willing too? */
        if (usec == SAL_MSGQ_NOWAIT) { /* Return error if can't wait */
            pthread_mutex_unlock(&mq->q_lock_mutex);
            rv = __LINE__;
            return -rv;
        }
        mq->pending_to_recv++;   /* Increment the number of waiting */

recv_retry:
        rv = pthread_mutex_lock(&mq->recv_mutex);
        if (0 != rv) {
            mq->pending_to_recv--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            rv = __LINE__;
            return -rv;
        }
        pthread_mutex_unlock(&mq->q_lock_mutex);

        ret_status = -2;
        if (usec == SAL_MSGQ_FOREVER) {
            rv = pthread_cond_wait(&mq->recv_cond, &mq->recv_mutex);
        } else  {
            struct timespec ts;
            int loop_count = 0;
            do {
                _timed_calc(usec, &ts);

                rv = pthread_cond_timedwait(&mq->recv_cond,
                                            &mq->recv_mutex,
                                            &ts);
                if (rv == ETIMEDOUT) {
                    ret_status = SAL_MSGQ_E_TIMEOUT;
                }
            } while (rv == EINVAL && loop_count++ < 1);
        }
        /* Unlock the mutex */
        pthread_mutex_unlock(&mq->recv_mutex);

        /* Lock the queue again */
        if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
            rv = __LINE__;
            return -rv;
        }
        if (rv != 0) {  /* If failed condition wait */
            mq->pending_to_recv--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            return ret_status;
        }

        /* It is possible, on multiple cores, that multiple threads will
         * wake up from a single condition signal (according to the
         * documentation). So verify that if the Q is empty that try again.
         */
        if (mq->num_of_element == 0) {
            goto recv_retry;
        }

        mq->pending_to_recv--;  /* Decrement the number of receive pending */
    } /* End of if Q is empty or pending receivers */

    /* If we are here have have the Q lock and there are elements in the Q */
    assert (mq->num_of_element > 0);
    for (j = SAL_MSGQ_MAX_PRIORITY; j >= SAL_MSGQ_MIN_PRIORITY; j--) {
        if (mq->pri_lists[j].head) {
            break;
        }
    }
    assert(j >= SAL_MSGQ_MIN_PRIORITY);
    p_element = mq->pri_lists[j].head;
    mq->pri_lists[j].head = p_element->next;
    if (mq->pri_lists[j].tail == p_element) {
        mq->pri_lists[j].tail = NULL;  /* was the last element */
    }

    /* Copy the data */
    memcpy(element, p_element->data, mq->element_size);

    /* Free the element to the free list */
    p_element->next = mq->free_element_list;
    mq->free_element_list = p_element;

    mq->num_of_element--;
    if (mq->pending_to_post > 0) {  /* If someone is waiting to post */
        pthread_mutex_lock(&mq->post_mutex);
        rv = pthread_cond_signal(&mq->post_cond); /* Wake up pending post threads */
        assert(rv == 0);
        pthread_mutex_unlock(&mq->post_mutex);
    }

    pthread_mutex_unlock(&mq->q_lock_mutex);
    return 0;
}

size_t
sal_msgq_count_get(sal_msgq_t msgq_hdl)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE)) {
        return 0;
    }
    /* No need to lock the queue as this value being updated atomically */
    return mq->num_of_element;
}
