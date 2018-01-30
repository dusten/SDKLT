/*
 * $Id: phymod_system.h,v 1.1.2.4 2013/07/11 08:38:24 dayad Exp $
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

#ifndef __PHYMOD_SYSTEM_H__
#define __PHYMOD_SYSTEM_H__

#include <phymod/phymod_types.h>

/*
 * This file defines the system interface for a given platform.
 */

#if PHYMOD_CONFIG_INCLUDE_ERROR_PRINT
#ifndef PHYMOD_DEBUG_ERROR
#error PHYMOD_DEBUG_ERROR must be defined when PHYMOD_CONFIG_INCLUDE_ERROR_PRINT is enabled
#endif
#else
#ifdef PHYMOD_DEBUG_ERROR
#undef PHYMOD_DEBUG_ERROR
#endif
#define PHYMOD_DEBUG_ERROR(STAFF_)
#endif

#if PHYMOD_CONFIG_INCLUDE_DEBUG_PRINT
#ifndef PHYMOD_DEBUG_VERBOSE
#error PHYMOD_DEBUG_VERBOSE must be defined when PHYMOD_CONFIG_INCLUDE_DEBUG_PRINT is enabled
#endif
#else
#ifdef PHYMOD_DEBUG_VERBOSE
#undef PHYMOD_DEBUG_VERBOSE
#endif
#define PHYMOD_DEBUG_VERBOSE(STAFF_)
#endif

#if PHYMOD_CONFIG_INCLUDE_DIAG_PRINT
#ifndef PHYMOD_DIAG_OUT
#error PHYMOD_DIAG_OUT must be defined when PHYMOD_CONFIG_INCLUDE_DIAG_PRINT is enabled
#endif
#else
#ifdef PHYMOD_DIAG_OUT
#undef PHYMOD_DIAG_OUT
#endif
#define PHYMOD_DIAG_OUT(STAFF_)
#endif

#ifndef PHYMOD_USLEEP
#error PHYMOD_USLEEP must be defined for the target system like this:
#error extern void PHYMOD_USLEEP(uint32_t usecs);
#endif

#ifndef PHYMOD_SLEEP
#error PHYMOD_SLEEP must be defined for the target system like this:
#error extern void PHYMOD_SLEEP(int secs);
#endif

#ifndef PHYMOD_MALLOC
#error PHYMOD_MALLOC must be defined for the target system like this:
#error extern void *PHYMOD_MALLOC(size_t size, char *descr);
#endif

#ifndef PHYMOD_FREE
#error PHYMOD_FREE must be defined for the target system like this:
#error extern void PHYMOD_FREE(void *ptr);
#endif

#ifndef PHYMOD_MEMSET
#error PHYMOD_MEMSET must be defined for the target system like this:
#error extern void *PHYMOD_MEMSET(void *dst_void, int val, size_t len);
#endif

#ifndef PHYMOD_MEMCPY
#error PHYMOD_MEMCPY must be defined for the target system like this:
#error extern void *PHYMOD_MEMCPY(void *dst, const void *src, size_t n);
#endif

#ifndef PHYMOD_STRCMP
#error PHYMOD_STRCMP must be defined for the target system like this:
#error extern int PHYMOD_STRCMP(const char *str1, const char *str2);
#endif

#ifndef PHYMOD_ASSERT
#define PHYMOD_ASSERT(expr_)
#endif

#endif /* __PHYMOD_SYSTEM_H__ */
