/*! \file shr_thread.h
 *
 * Common thread framework built on top of SAL API.
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

#ifndef SHR_THREAD_H
#define SHR_THREAD_H

#include <sal/sal_sem.h>
#include <sal/sal_time.h>
#include <sal/sal_thread.h>

#include <shr/shr_pb.h>

/*! Use this value to wait indefinitely in \ref shr_thread_sleep. */
#define SHR_THREAD_FOREVER      SAL_SEM_FOREVER

/* Forward declaration */
struct shr_thread_ctrl_s;

/*!
 * \brief Type for main thread function.
 *
 * This function is provided by the application and will be called
 * from the thread framework once all the associated control
 * structures have been initialized.
 *
 * The application must provide a function of this type, when \ref
 * shr_thread_start is called.
 *
 * \param [in] tc Thread control returned by \ref shr_thread_start.
 * \param [in] arg Context passed in by \ref shr_thread_start.
 *
 * \return Nothing.
 */
typedef void (shr_thread_func_f)(struct shr_thread_ctrl_s *tc, void *arg);

/*!
 * Thread control object.
 */
typedef struct shr_thread_ctrl_s {

    /*! Signature which indicates that object is initialized. */
    uint32_t tc_sig;

    /*! Next thread in global thread list. */
    struct shr_thread_ctrl_s *next;

    /*! Application-provided function to call from thread context. */
    shr_thread_func_f *thread_func;

    /*! Application-provided context for the thread function. */
    void *thread_arg;

    /*! Semaphore to synchronize thread stop/exit flow. */
    sal_sem_t stop_sem;

    /*! Semaphore to allow thread to wait for an event. */
    sal_sem_t sleep_sem;

    /*! Thread name for debug purposes. */
    char name[32];

    /*! Non-zero if thread is active (running or done). */
    int active;

    /*! Non-zero if thread has been requested to stop. */
    int stopping;

    /*! Non-zero if thread is waiting for an event. */
    int waiting;

    /*! Thread function has exited and returned to thread control. */
    int done;

    /*! Non-zero if thread encountered an internal state error. */
    int error;

} shr_thread_ctrl_t;

/*!
 * \brief Start a thread.
 *
 * This function will launch a thread via the SAL thread API and call
 * into the supplied application function.
 *
 * The thread will use the default SAL stack size, and the priority
 * uses the SAL API definitions directly. If \c prio is set to -1,
 * then \c SAL_THREAD_PRIO_DEFAULT will be used.
 *
 * Example:
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     ...
 * }
 *
 * void my_main(void)
 * {
 *     shr_thread_ctrl_t *tc;
 *
 *     tc = shr_thread_start("myThread", -1, my_thread, NULL);
 *     ...
 * }
 * \endcode
 *
 * \param [in] name Thread name.
 * \param [in] prio Thread priority (use -1 for default priority).
 * \param [in] func Application function to call from newly created thread.
 * \param [in] arg Transparent context passed in to application function.
 *
 * \return Thread control object or NULL on error.
 */
extern shr_thread_ctrl_t *
shr_thread_start(char *name, int prio, shr_thread_func_f func, void *arg);

/*!
 * \brief Terminate an active thread.
 *
 * This function is used to terminate a thread, which was started by
 * \ref shr_thread_start. The caller must specify the maximum time to
 * wait for the thread to terminate itself.
 *
 * If the thread has already exited, then this function will simply
 * clean up the resources associated with the thread.
 *
 * If the thread is still running, then a stop request will be issued,
 * and the function will wait for the thread to exit.
 *
 * If a timeout value of zero is specified, the function will return
 * immediately. In this case \ref shr_thread_stop must be called again
 * later with a non-zero timeout value to verify that the thread did
 * indeed stop. This mechanism allows a caller to wait in parallel for
 * multiple threads to terminate.
 *
 * An event-driven thread function will typical look as shown
 * below. When \ref shr_thread_stop is called, a stop request will be
 * issued and the thread will be woken up. The thread must check for a
 * stop request using \ref shr_thread_stopping and exit accordingly.
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     while (1) {
 *         shr_thread_sleep(tc, SHR_THREAD_FOREVER);
 *         if (shr_thread_stopping(tc)) {
 *             break;
 *         }
 *         ...
 *     }
 * }
 *
 * void my_shutdown(void)
 * {
 *     int rv;
 *
 *     rv = shr_thread_stop(tc, 500000);
 *     ...
 * }
 * \endcode
 *
 * By provoding a timeout value, the caller may choose to wait for the
 * thread to terminate.
 *
 * \param [in] tc Thread control returned by \ref shr_thread_start.
 * \param [in] usecs Number of microseconds to wait for the thread to stop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT The thread did not stop within the specified time.
 */
extern int
shr_thread_stop(shr_thread_ctrl_t *tc, sal_usecs_t usecs);

/*!
 * \brief Check for stop request.
 *
 * This function should be called from an event-driven thread to check
 * if it should terminate itself.
 *
 * A typical event-driven thread is shown below.
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     while (1) {
 *         shr_thread_sleep(tc, SHR_THREAD_FOREVER);
 *         if (shr_thread_stopping(tc)) {
 *             break;
 *         }
 *         ...
 *     }
 * }
 * \endcode
 *
 * \param [in] tc Thread control passed in as a function argument.
 *
 * \retval TRUE A stop request was submitted, exit as soon as possible.
 * \retval FALSE No stop request was submitted, continue processing.
 */
extern int
shr_thread_stopping(shr_thread_ctrl_t *tc);

/*!
 * \brief Check if thread is done.
 *
 * This function may be called from a parent thread to check if a
 * child thread is done.
 *
 * If the function returns TRUE, then the parent thread must call \ref
 * shr_thread_stop to clean up the thread completely.
 *
 * \param [in] tc Thread control passed in as a function argument.
 *
 * \retval TRUE Thread is done.
 * \retval FALSE Thread is still running.
 */
extern int
shr_thread_done(shr_thread_ctrl_t *tc);

/*!
 * \brief Wait for event.
 *
 * This function may be called from an event-driven thread when there
 * is no more work to do.
 *
 * Another thread must wake up the sleeping thread by calling \ref
 * shr_thread_wake.  The thread will wait indefinitely if the timeout
 * value SHR_THREAD_FOREVER is used.
 *
 * A typical event-driven thread is shown below.
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     while (1) {
 *         shr_thread_sleep(tc, SHR_THREAD_FOREVER);
 *         if (shr_thread_stopping(tc)) {
 *             break;
 *         }
 *         ...
 *     }
 * }
 * \endcode
 *
 * Note that the timeout value can also be used to simply wake up the
 * thread at fixed intervals.
 *
 * \param [in] tc Thread control passed in as a function argument.
 * \param [in] usecs Maximum number of microseconds to wait for a wake event.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to sleep.
 */
extern int
shr_thread_sleep(shr_thread_ctrl_t *tc, sal_usecs_t usecs);

/*!
 * \brief Wake an event-driven thread.
 *
 * Use this function to wake up an event-driven thread, which is
 * asleep after calling \ref shr_thread_sleep.
 *
 * This function is safe to call from interrupt context.
 *
 * \param [in] tc Thread control for the thread to be woken.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
shr_thread_wake(shr_thread_ctrl_t *tc);

/*!
 * \brief Dump status of all threads to a print buffer.
 *
 * The \c pb parameter may be specified as NULL if only the number of
 * active threads is of interest.
 *
 * \param [in] pb Print buffer object.
 *
 * \return Number of active threads.
 */
extern int
shr_thread_dump(shr_pb_t *pb);

#endif /* SHR_THREAD_H */
