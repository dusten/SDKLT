/*! \file sal_msgq.h
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

#ifndef SAL_MSGQ_H
#define SAL_MSGQ_H

#include <sal/sal_types.h>

/*!
 * Use this value to disable timeout for \ref sal_msgq_recv and \ref
 * sal_msgq_post.
 */
#define SAL_MSGQ_FOREVER        (uint32_t)(-1)

/*!
 * Use this timeout value to return immediately from \ref
 * sal_msgq_post if the send queue is full or from \ref sal_msgq_recv
 * if no message is ready.
 */
#define SAL_MSGQ_NOWAIT         0

/*! Special return value to indicate that the operation timed out. */
#define SAL_MSGQ_E_TIMEOUT      -3

/*! Invalid parameter error code */
#define SAL_MSGQ_E_INVALID_PARAM    -4

/*! Supported message priorities. */
typedef enum {
    SAL_MSGQ_MIN_PRIORITY = 0,
    SAL_MSGQ_NORMAL_PRIORITY = SAL_MSGQ_MIN_PRIORITY,
    SAL_MSGQ_HIGH_PRIORITY = 1,
    SAL_MSGQ_MAX_PRIORITY = SAL_MSGQ_HIGH_PRIORITY
} sal_msgq_priority_t;

/*! Number of supported message priorities. */
#define SAL_MSGQ_PRIORITIES     (SAL_MSGQ_MAX_PRIORITY + 1)

/*! Opaque message queue handle. */
typedef struct sal_msgq_s *sal_msgq_t;

/*!
 * \brief Create message queue
 *
 * \param [in] element_size The size of elements in the queue.
 *              All elements must have the same size
 * \param [in] max_elements The maximum number of elements that can be
 *              queued.
 * \param [in] desc String to identify an object during debug.
 *
 * \return Message queue handle on success or NULL on failure.
 */
extern sal_msgq_t
sal_msgq_create(size_t element_size, size_t max_elements, const char *desc);

/*!
 * \brief Destroy a message queue
 *
 * \param [in] msgq_hdl Message queue handle.
 *
 * \return Nothing.
 */
extern void
sal_msgq_destroy(sal_msgq_t msgq_hdl);

/*!
 * \brief Post a message to the queue
 *
 * \param [in] msgq_hdl Message queue handle
 * \param [in] element Element to push onto the message queue.
 * \param [in] pri Message priority.
 * \param [in] usec Send timeout value. If the queue is full, the function
 *              will block for up to \c usec microseconds.
 *
 * \return 0 on success, SAL_MSGQ_E_TIMEOUT on timeout, non-zero value
 *              if other failure.
 */
extern int
sal_msgq_post(sal_msgq_t msgq_hdl,
              void *element,
              sal_msgq_priority_t pri,
              uint32_t usec);

/*!
 * \brief Receive a message from the queue
 *
 * \param [in] msgq_hdl Message queue handle
 * \param [in] element Buffer where the content of the element will be
 *              copied into. Note that the memory size must be larger
 *              or equal to the element size used in \ref sal_msgq_create.
 * \param [in] usec Receive timeout value. If the queue is empty the
 *              function will wait for up to \c usec microseconds.
 *
 * \return 0 on success, SAL_MSGQ_E_TIMEOUT on timeout, non-zero value
 *              if other failure.
 */
extern int
sal_msgq_recv(sal_msgq_t msgq_hdl, void *element, uint32_t usec);

/*!
 * \brief Reports the number of pending elements in the queue
 *
 * \param [in] msgq_hdl Message queue handle
 *
 * \return The number of elements that are currently pending in the queue.
 * Value of 0 indicates either empty queue or invalid queue handle.
 */
extern size_t
sal_msgq_count_get(sal_msgq_t msgq_hdl);

#endif /* SAL_MSGQ_H */
