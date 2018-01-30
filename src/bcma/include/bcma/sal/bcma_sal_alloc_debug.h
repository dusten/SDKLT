/*! \file bcma_sal_alloc_debug.h
 *
 * Debug API for sal_alloc and friends.
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

#ifndef BCMA_SAL_ALLOC_DEBUG_H
#define BCMA_SAL_ALLOC_DEBUG_H

#include <sal/sal_types.h>

/*!
 * \brief Set \c sal_alloc trace prefix list.
 *
 * The \c sal_alloc function optionally prints a message on the
 * console each time sal_alloc is called. The trace prefix list is
 * used to filter these messages.
 *
 * If \c prefix is an empty string (""), then no messages will be
 * printed. If the prefix string is "*", then all messages will be
 * printed. Any other prefix string will be matched against the ID
 * string parameter supplied to sal_alloc.
 *
 * Multiple prefixes may be specified as a comma-separated list,
 * e.g. "sal,shr".
 *
 * \param [in] prefix Prefix string(s) to match against sal_alloc ID string.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_trace_prefix_set(const char *prefix);

/*!
 * \brief Set list of trace prefixes to ignore.
 *
 * If \c prefix is an empty string (""), then no prefixes will be
 * ignored.
 *
 * Multiple prefixes may be specified as a comma-separated list,
 * e.g. "sal,shr".
 *
 * See also \ref bcma_sal_alloc_debug_trace_prefix_set.
 *
 * \param [in] prefix Prefix string(s) to ignore.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_notrace_prefix_set(const char *prefix);

/*!
 * \brief Set \c sal_alloc track prefix.
 *
 * The \c sal_alloc function optionally tracks all allocations, which
 * can then be dumped on demand, for example when an application is
 * terminated.
 *
 * The \c prefix determines which allocation records will be printed
 * when the allocation database is dumped. If \c prefix is an empty
 * string (""), then no records will be printed. If the prefix string
 * is "*", then all records will be printed. Any other prefix string
 * will be matched against the ID string parameter supplied to
 * sal_alloc.
 *
 * \param [in] prefix Prefix string to match against sal_alloc ID string.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_track_prefix_set(const char *prefix);

/*!
 * \brief Enable or disable memory fill.
 *
 * If memory fill is enabled, then newly allocated memory will be
 * filled with a fixed pattern. Likewise, another fixed pattern will
 * be filled into the memory immediately before it is freed.
 *
 * \param [in] fill Set to true to enable memory fill.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_fill_enable_set(bool fill);

/*!
 * \brief Show memory allocation status.
 *
 * Print allocation statistics and controls to console.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_status(void);

/*!
 * \brief Dump tracked allocations.
 *
 * The tracked allocations will be printed according to the current
 * track prefix. See \ref bcma_sal_alloc_debug_track_prefix_set for
 * details.
 *
 * \return Always 0.
 */
extern int
bcma_sal_alloc_debug_dump(void);

/*!
 * \brief Clean up the sal_alloc debug module.
 *
 * Clean up resources allocated by \ref bcma_sal_alloc_debug_init.
 *
 * \return 0 on success, otherwise -1.
 */
extern int
bcma_sal_alloc_debug_cleanup(void);

/*!
 * \brief Initialize the sal_alloc debug module.
 *
 * This function will install debug hooks into the SAL memory
 * allocation API (sal_alloc, etc.).
 *
 * The debug hooks will add overrun markers around each chunk of
 * allocated memory, and these markers will be checked when a chunk is
 * freed.
 *
 * If tracking is enabled, then all memory allocations are saved to a
 * database, which can be dumped on demand using \ref
 * bcma_sal_alloc_debug_dump.
 *
 * \param [in] track_enable Set to true to enable tracking.
 *
 * \return 0 on success, otherwise -1.
 */
extern int
bcma_sal_alloc_debug_init(bool track_enable);

#endif /* BCMA_SAL_ALLOC_DEBUG_H */
