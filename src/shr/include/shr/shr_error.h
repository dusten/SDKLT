/*! \file shr_error.h
 *
 * Shared error codes.
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

#ifndef SHR_ERROR_H
#define SHR_ERROR_H

/*!
 * \brief Standard SDK error codes.
 *
 * IMPORTANT: These error codes must match the corresponding text
 * messages in shr_error.c.
 */
typedef enum {

    /*!
     * The operation completed successfully.
     */
    SHR_E_NONE                 =  0,

    /*!
     * This usually indicates that software encountered an internal
     * data inconsistency or an unanticipated hardware state.
     */
    SHR_E_INTERNAL             = -1,

    /*!
     * An operation failed due to insufficient dynamically allocated
     * memory.
     */
    SHR_E_MEMORY               = -2,

    /*!
     * The first argument of many API routines is a unit number. This
     * error occurs if that number refers to a non-existent unit.
     */
    SHR_E_UNIT                 = -3,

    /*!
     * A parameter to an API routine was invalid. A null pointer value
     * may have been passed to the routine, or an integer parameter
     * may be outside of its allowed range.
     */
    SHR_E_PARAM                = -4,

    /*!
     * The operation encountered a pooled resource (e.g. a table or a
     * list) with no valid elements.
     */
    SHR_E_EMPTY                = -5,

    /*!
     * The operation encountered a pooled resource (e.g. a table or a
     * list) with no room for new elements.
     */
    SHR_E_FULL                 = -6,

    /*!
     * The specified entry in a pooled resource (e.g. a table or a
     * list) could not be found.
     */
    SHR_E_NOT_FOUND            = -7,

    /*!
     * The specified entry of a pooled resource (e.g. a table or a
     * list) already exists.
     */
    SHR_E_EXISTS               = -8,

    /*!
     * The operation did not complete within the maximum allowed time frame.
     */
    SHR_E_TIMEOUT              = -9,

    /*!
     * An operation was attempted before the previous operation had
     * completed.
     */
    SHR_E_BUSY                 = -10,

    /*!
     * An operation could not be completed. This may be due to a
     * hardware or configuration problem.
     */
    SHR_E_FAIL                 = -11,

    /*!
     * The operation could not be completed because a required feature
     * was disabled.
     */
    SHR_E_DISABLED             = -12,

    /*!
     * The specified identifier was not valid. Note that this error
     * code will normally take precedence over \ref SHR_E_PARAM.
     */
    SHR_E_BADID                = -13,

    /*!
     * The operation could not be completed due to lack of hardware
     * resources.
     */
    SHR_E_RESOURCE             = -14,

    /*!
     * The operation could not be completed due to incomplete or
     * incorrect configurataion.
     */
    SHR_E_CONFIG               = -15,

    /*!
     * The hardware does not support the requested operation.
     */
    SHR_E_UNAVAIL              = -16,

    /*!
     * An operation was attempted before initialization was complete.
     */
    SHR_E_INIT                 = -17,

    /*!
     * The specified port value was not valid. Note that this error
     * code will normally take precedence over \ref SHR_E_PARAM.
     */
    SHR_E_PORT                 = -18,

    /*!
     * A low-level register or memory access failed.
     */
    SHR_E_IO                   = -19,

    /*!
     * Access method not permitted. Typically returned if attempting
     * to write to a read-only object.
     */
    SHR_E_ACCESS               = -20,

    /*!
     * No handler exists to perform the hardware access associated
     * with a an operation on a software object.
     */
    SHR_E_NO_HANDLER           = -21,

    /*!
     * The operation was only partially completed, and this could
     * potentially leave the system in an unexpected state.
     */
    SHR_E_PARTIAL              = -22,

    SHR_E_LIMIT                = -23           /* Must come last */

} shr_error_t;

/*! Check for successful return value. */
#define SHR_SUCCESS(_expr) ((_expr) >= 0)

/*! Check for error return value. */
#define SHR_FAILURE(_expr) ((_expr) < 0)

/*!
 * \brief Get standard error message
 *
 * Returns a text message corresponding to the error code passed in.
 *
 * \param [in] rv Error code
 *
 * \return Pointer to error message
 */
extern const char *
shr_errmsg(int rv);

#endif /* SHR_ERROR_H */
