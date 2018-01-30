/*! \file sal_thread.h
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

#ifndef SAL_THREAD_H
#define SAL_THREAD_H

#include <sal/sal_types.h>

/*! Special \ref sal_thread_t error handle. */
#define SAL_THREAD_ERROR                ((sal_thread_t) -1)

/*! Default stack size for threads created via \ref sal_thread_create. */
#ifndef SAL_THREAD_STKSZ
#define	SAL_THREAD_STKSZ                16384
#endif

/*! Opaque thread handle. */
#define SAL_THREAD_NAME_MAX_LEN         80

/*! Opaque thread handle. */
typedef struct sal_thread_s *sal_thread_t;

/*!
 * \brief Run thread at highest acceptable priority.
 *
 * The primary use of this priority level is for user threads
 * executing interrupt handlers.
 */
#define SAL_THREAD_PRIO_NO_PREEMPT      -1

/*!
 * \brief Run thread at default priority.
 *
 * Convenience value for thread with no special scheduling needs.
 *
 * The value can also be used to let certain threads run at higher or
 * lower priority relative to the default priority,
 * e.g. (SAL_THREAD_PRIO_DEFAULT + 5).
 */
#ifndef SAL_THREAD_PRIO_DEFAULT
#define SAL_THREAD_PRIO_DEFAULT         50
#endif

/*!
 * \brief Highest acceptable real-time priority.
 *
 * Used to implement the \ref SAL_THREAD_PRIO_NO_PREEMPT priority on
 * POSIX systems. Allow override in case the priority level needs to
 * be coordinated with other real-time application threads.
 */
#ifndef SAL_THREAD_RT_PRIO_HIGHEST
#define SAL_THREAD_RT_PRIO_HIGHEST      90
#endif

/*! Per-thread data type. */
typedef void sal_thread_data_t;

/*!
 * \brief Thread function.
 *
 * A function of this type must be supplied to \ref sal_thread_create,
 * and upon successful creation of a thread, this function will be
 * called from the newly created thread, and upon return the thread
 * will terminate.
 *
 * \param [in] arg Thread context supplied via \ref sal_thread_create.
 *
 * \return Nothing.
 */
typedef void (sal_thread_func_f)(void *arg);

/*!
 * \brief Thread data desctructor function.
 *
 * If a function of this type is supplied to \ref
 * sal_thread_data_create, then it will be called when the thread
 * exits if the associated per-thread data key is non-NULL. The data
 * key will be supplied as the parameter.
 *
 * \param [in] arg Data key supplied via \ref sal_thread_data_set.
 *
 * \return Nothing.
 */
typedef void (sal_thread_data_destroy_f)(void *arg);

/*!
 * \brief Create new thread.
 *
 * \param [in] name Thread name.
 * \param [in] stksz Stack size for this thread.
 * \param [in] prio Thread priority (0-99 or SAL_THREAD_PRIO_xxx).
 * \param [in] func Thread function to be called by new thread.
 * \param [in] arg Context to be passed to thread function.
 *
 * \return Thread handle or SAL_THREAD_ERROR on failure.
 */
extern sal_thread_t
sal_thread_create(char *name, int stksz, int prio,
                  sal_thread_func_f func, void *arg);

/*!
 * \brief Get thread handle of current thread.
 *
 * \return Thread handle or SAL_THREAD_ERROR if not launched by \ref
 *         sal_thread_create.
 */
extern sal_thread_t
sal_thread_self(void);

/*!
 * \brief Get thread name.
 *
 * \param [in] thread Thread handle.
 * \param [out] thread_name Buffer where to  store the thread name.
 * \param [in] thread_name_size Size of \c thread_name buffer.
 *
 * \return Pointer to \c thread_name or NULL if thread was not found.
 */
extern char *
sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size);

/*!
 * \brief Clean up thread.
 *
 * A thread function supplied to \ref sal_thread_create does not need
 * to call this API directly, as it will be called when the thread
 * function returns.
 *
 * \param [in] rc Thread exit code (0 means normal exit).
 *
 * \return Nothing.
 */
extern void
sal_thread_exit(int rc);

/*!
 * \brief Yield execution.
 *
 * Voluntarily yield execution of the current thread to allow other
 * threads of equal scheduling priority to run.
 *
 * \return Nothing.
 */
extern void
sal_thread_yield(void);

/*!
 * \brief Mark a thread as being the interactive console thread.
 *
 * Calling this API for the console thread allows for special
 * treatment of the console thread and the non-console
 * threads. Special cases include log messages and lock management,
 * where for example Ctrl-C can be disabled while the console thread
 * holds a lock.
 *
 * \param [in] thread Thread handle.
 *
 * \return Nothing.
 */
extern void
sal_thread_console_set(sal_thread_t thread);

/*!
 * \brief Get designated console thread.
 *
 * Returns the thread handle supplied via \ref sal_thread_console_set.
 *
 * \return Console thread handle, or NULL if no designated console thread
 *         exists.
 */
extern sal_thread_t
sal_thread_console_get(void);

/*!
 * \brief Create per-thread data object.
 *
 * This function will create a new data key for all threads belonging
 * to the parent process. The initial value of the key will be NULL
 * for all threads.
 *
 * \param [in] destructor Destructor function to be called on thread exit.
 *
 * \return Per-thread data handle.
 */
extern sal_thread_data_t *
sal_thread_data_create(sal_thread_data_destroy_f destructor);

/*!
 * \brief Set per-thread data key for this thread.
 *
 * If non-NULL, this key will be supplied to the \c destructor
 * function upon thread exit.
 *
 * \param [in] tdata Per-thread data handle from \ref sal_thread_data_create.
 * \param [in] val New per-thread data key.
 *
 * \retval 0 No errors
 */
extern int
sal_thread_data_set(sal_thread_data_t *tdata, void *val);

/*!
 * \brief Get per-thread data key for this thread.
 *
 * \param [in] tdata Per-thread data handle from \ref sal_thread_data_create.
 *
 * \return 0 if no errors, otherwise non-zero.
 */
extern void *
sal_thread_data_get(sal_thread_data_t *tdata);

/*!
 * \brief Delete per-thread data object.
 *
 * Calling this function will invoke the destructor function supplied
 * to \ref sal_thread_data_create.
 *
 * \param [in] tdata Per-thread data handle.
 *
 * \retval 0 No errors
 */
extern int
sal_thread_data_delete(sal_thread_data_t *tdata);

#endif /* SAL_THREAD_H */
