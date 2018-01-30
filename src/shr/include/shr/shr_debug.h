/*! \file shr_debug.h
 *
 * Generic macros for tracing function call trees.
 *
 * The main principle is to establish a single point of exit for each
 * function, and then combine this with a standard method of logging
 * error conditions.
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

#ifndef SHR_DEBUG_H
#define SHR_DEBUG_H

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>

/*!
 * \brief Function entry declarations and code.
 *
 * This macro must appear in each function right after the local
 * variable declarations.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 * }
 * \endcode
 *
 * \param [in] _unit Switch unit number.
 */
#define SHR_FUNC_ENTER(_unit)                           \
    shr_error_t _func_unit = _unit;                     \
    shr_error_t _func_rv = SHR_E_NONE;                  \
    LOG_DEBUG(BSL_LOG_MODULE,                           \
              (BSL_META_U(_func_unit, "enter\n")));

/*!
 * \brief Single point of exit code.
 *
 * This macro must appear at the very bottom of each function, and it
 * must be preceded an 'exit' label and optionally some resource
 * clean-up code.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 */
#define SHR_FUNC_EXIT()                                 \
    LOG_DEBUG(BSL_LOG_MODULE,                           \
              (BSL_META_U(_func_unit, "exit\n")));      \
    return _func_rv;

/*!
 * \brief Goto single point of exit.
 *
 * Go to label 'exit', which must be defined explicitly in each
 * function.
 *
 * This macro is normally not called directly, but it is used as part
 * of other error handling macros.
 */
#define SHR_EXIT() goto exit

/*!
 * \brief Check for function error state.
 *
 * This macro is a Boolean expression, which evaluates to TRUE, if the
 * macro-based function return value (declared by \ref SHR_FUNC_ENTER)
 * is set to error.
 *
 * It can be used to clean up allocated resources in case of failure,
 * for example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 *
 *   exit:
 *     if (SHR_FUNC_ERROR()) {
 *         ...
 *     }
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 */
#define SHR_FUNC_ERR()                          \
    SHR_FAILURE(_func_rv)

/*!
 * \brief Assign function return value from expression and exit.
 *
 * The expression can be a function call or a fixed error code.
 *
 * See \ref SHR_FUNC_VAL_IS for an example.
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_RETURN_VAL_EXIT(_expr)              \
    do {                                        \
        _func_rv = _expr;                       \
        SHR_EXIT();                             \
    } while (0)

/*!
 * \brief Warn on expression error.
 *
 * Evaluate an expression and log a warning message if the expression
 * evaluates to a standard error code. The result is assigned to the
 * macro-based function return value (declared by \ref
 * SHR_FUNC_ENTER), but code execution continues immediately below the
 * statement (i.e. no jump to the single point of exit).
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_CONT(some_other_func(unit));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_IF_ERR_CONT(_expr)                          \
    do {                                                \
        shr_error_t _rv = _expr;                        \
        if (SHR_FAILURE(_rv)) {                         \
            LOG_WARN(BSL_LOG_MODULE,                    \
                     (BSL_META_U(_func_unit, "%s\n"),   \
                      shr_errmsg(_rv)));                \
            _func_rv = _rv;                             \
        }                                               \
    } while(0)

/*!
 * \brief Error-exit on expression error.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log a error message and go to the function's single
 * point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_EXIT
 *         (some_other_func(unit));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_IF_ERR_EXIT(_expr)                          \
    do {                                                \
        shr_error_t _rv = _expr;                        \
        if (SHR_FAILURE(_rv)) {                         \
            LOG_ERROR(BSL_LOG_MODULE,                   \
                      (BSL_META_U(_func_unit, "%s\n"),  \
                       shr_errmsg(_rv)));               \
            _func_rv = _rv;                             \
            SHR_EXIT();                                 \
        }                                               \
    } while(0)

/*!
 * \brief Verbose-exit on expression error.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log a verbose message and go to the function's single
 * point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_VERBOSE_EXIT
 *         (some_other_func(unit));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_IF_ERR_VERBOSE_EXIT(_expr)                   \
    do {                                                 \
        shr_error_t _rv = _expr;                         \
        if (SHR_FAILURE(_rv)) {                          \
            LOG_VERBOSE(BSL_LOG_MODULE,                  \
                        (BSL_META_U(_func_unit, "%s\n"), \
                         shr_errmsg(_rv)));              \
            _func_rv = _rv;                              \
            SHR_EXIT();                                  \
        }                                                \
    } while(0)

/*!
 * \brief Exit on expression error with custom message.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log a custom error message and go to the function's
 * single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * The error message must be specified as a single expression using
 * one of the BSM_META_xxx macros.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_MSG_EXIT
 *         (some_other_func(unit),
 *          (BSL_META_U(unit,
 *                      "Operation failed for instance %d\n"),
 *           local_idx));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 * \param [in] _stuff Custom print statement (see description).
 */
#define SHR_IF_ERR_MSG_EXIT(_expr, _stuff)               \
    do {                                                 \
        shr_error_t _rv = _expr;                         \
        if (SHR_FAILURE(_rv)) {                          \
            LOG_ERROR(BSL_LOG_MODULE, _stuff);           \
            _func_rv = _rv;                              \
            SHR_EXIT();                                  \
        }                                                \
    } while(0)

/*!
 * \brief Verbose-exit on expression error with custom message.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log a custom verbose message and go to the function's
 * single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * The error message must be specified as a single expression using
 * one of the BSM_META_xxx macros.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_VERBOSE_MSG_EXIT
 *         (some_other_func(unit),
 *          (BSL_META_U(unit,
 *                      "Operation failed for instance %d\n"),
 *           local_idx));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 * \param [in] _stuff Custom print statement (see description).
 */
#define SHR_IF_ERR_VERBOSE_MSG_EXIT(_expr, _stuff)       \
    do {                                                 \
        shr_error_t _rv = _expr;                         \
        if (SHR_FAILURE(_rv)) {                          \
            LOG_VERBOSE(BSL_LOG_MODULE, _stuff);         \
            _func_rv = _rv;                              \
            SHR_EXIT();                                  \
        }                                                \
    } while(0)

/*!
 * \brief Error-exit on expression error with exception
 *
 * Evaluate an expression and if it evaluates to a standard error code
 * which differs from the exception value, then log a error message
 * and go to the function's single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * Typical exception error codes are SHR_E_NOT_FOUND and
 * SHR_E_UNAVAIL, because these error codes may be a simple reflection
 * of the device state or capabilties, which are used to control the
 * logical flow of the function.
 *
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int rv;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     rv = some_other_func(unit);
 *     SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 * \param [in] _expr Expression to evaluate.
 * \param [in] _rv_except Error code to ignore.
 */
#define SHR_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except)    \
    do {                                                \
        int _rv = _expr;                                \
        if (SHR_FAILURE(_rv) && _rv != _rv_except) {    \
            LOG_ERROR(BSL_LOG_MODULE,                   \
                      (BSL_META_U(_func_unit, "%s\n"),  \
                       shr_errmsg(_rv)));               \
            _func_rv = _rv;                             \
            SHR_EXIT();                                 \
        }                                               \
    } while(0)

/*!
 * \brief Check for specific function error value.
 *
 * This macro is a Boolean expression, which evaluates to TRUE, if the
 * macro-based function return value (declared by \ref SHR_FUNC_ENTER)
 * matches the specified value.
 *
 * It can be used to take special actions for specific error codes.
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 *
 *     if (SHR_FUNC_VAL_IS(SHR_E_UNAVAIL)) {
 *         SHR_RETURN_VAL_EXIT(SHR_E_NONE);
 *     }
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _rv Return value to check against.
 */
#define SHR_FUNC_VAL_IS(_rv)                    \
    (_func_rv == (_rv))


/*!
 * \brief Check for null-pointer.
 *
 * Check if a pointer is NULL, and if so, log an error and exit.
 *
 * The macro is intended for both input parameter checks and memory
 * allocation errors.
 *
 * \param [in] _ptr Pointer to check.
 * \param [in] _rv Function return value to use if pointer is NULL.
 */
#define SHR_NULL_CHECK(_ptr, _rv)               \
    do {                                        \
        if ((_ptr) == NULL) {                   \
            LOG_ERROR(BSL_LOG_MODULE,           \
                      (BSL_META_U(_func_unit,   \
                                  "null\n")));  \
            _func_rv = _rv;                     \
            SHR_EXIT();                         \
        }                                       \
    } while (0)

/*!
 * \brief Verbose-check for null-pointer.
 *
 * Check if a pointer is NULL, and if so, then log a verbose message and go to
 * the function's single point of exit.
 *
 * The macro is intended for both input parameter checks and memory
 * allocation errors.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * \param [in] _ptr Pointer to check.
 * \param [in] _rv Function return value to use if pointer is NULL.
 */
#define SHR_NULL_VERBOSE_CHECK(_ptr, _rv)       \
    do {                                        \
        if ((_ptr) == NULL) {                   \
            LOG_VERBOSE(BSL_LOG_MODULE,         \
                      (BSL_META_U(_func_unit,   \
                                  "null\n")));  \
            _func_rv = _rv;                     \
            SHR_EXIT();                         \
        }                                       \
    } while (0)

/*!
 * \brief Memory allocation with leak check.
 *
 * This macro is intended to prevent memory leaks from premature
 * pointer recycling. This type of leak happens if a pointer to an
 * allocated block of memory is used for another memory allocation
 * before the first block is freed.
 *
 * If the above error is detected, the macro will log and error and
 * exit the current function, otherwise a normall memory allocation
 * will be done using sal_alloc.
 *
 * The macro depends on \ref SHR_FUNC_ENTER and may be used in
 * conjunction with \ref SHR_FREE.
 *
 * \param [in] _ptr Pointer variable to use for memory allocation.
 * \param [in] _sz Amount of memory to allocate (in bytes).
 * \param [in] _str Debug string to be passed to sal_alloc.
 */
#define SHR_ALLOC(_ptr, _sz, _str)                      \
    do {                                                \
        if ((_ptr) == NULL) {                           \
            (_ptr) = sal_alloc(_sz, _str);              \
        } else {                                        \
            LOG_ERROR(BSL_LOG_MODULE,                   \
                      (BSL_META_U(_func_unit,           \
                                  "not null (%s)\n"),   \
                       _str));                          \
            SHR_EXIT();                                 \
        }                                               \
    } while (0)

/*!
 * \brief Free memory and reset pointer.
 *
 * This macro will free allocated memory and reset the associated
 * pointer to NULL. The memory will be freed using sal_free.
 *
 * The macro may be used in conjunction with \ref SHR_ALLOC.
 *
 * \param [in] _ptr Pointer to memory to be freed.
 */
#define SHR_FREE(_ptr)                          \
    do {                                        \
        if ((_ptr) != NULL) {                   \
            sal_free((void *)(_ptr));           \
            (_ptr) = NULL;                      \
        }                                       \
    } while (0)

#endif  /* SHR_DEBUG_H */
