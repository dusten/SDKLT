/*! \file sal_types.h
 *
 * Basic types and convenience macros.
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

#ifndef SAL_TYPES_H
#define SAL_TYPES_H

#include <sal_config.h>

/*!
 * \cond SAL_TYPES
 */

#if SAL_CONFIG_DEFINE_UINT8_T == 1
typedef SAL_CONFIG_TYPE_UINT8_T uint8_t;
#endif

#if SAL_CONFIG_DEFINE_UINT16_T == 1
typedef SAL_CONFIG_TYPE_UINT16_T uint16_t;
#endif

#if SAL_CONFIG_DEFINE_UINT32_T == 1
typedef SAL_CONFIG_TYPE_UINT32_T uint32_t;
#endif

#if SAL_CONFIG_DEFINE_UINT64_T == 1
typedef SAL_CONFIG_TYPE_UINT64_T uint64_t;
#endif

#if SAL_CONFIG_DEFINE_UINTPTR_T == 1
typedef SAL_CONFIG_TYPE_UINTPTR_T uintptr_t;
#endif

#if SAL_CONFIG_DEFINE_INT8_T == 1
typedef SAL_CONFIG_TYPE_INT8_T int8_t;
#endif

#if SAL_CONFIG_DEFINE_INT16_T == 1
typedef SAL_CONFIG_TYPE_INT16_T int16_t;
#endif

#if SAL_CONFIG_DEFINE_INT32_T == 1
typedef SAL_CONFIG_TYPE_INT32_T int32_t;
#endif

#if SAL_CONFIG_DEFINE_INT64_T == 1
typedef SAL_CONFIG_TYPE_INT64_T int64_t;
#endif

#if SAL_CONFIG_DEFINE_SIZE_T == 1
typedef SAL_CONFIG_TYPE_SIZE_T size_t;
#endif

#if SAL_CONFIG_DEFINE_BOOL_T == 1
typedef SAL_CONFIG_TYPE_BOOL_T bool;
#endif

#if SAL_CONFIG_DEFINE_DMA_ADDR_T == 1
typedef SAL_CONFIG_TYPE_DMA_ADDR_T dma_addr_t;
#endif

#if SAL_CONFIG_DEFINE_PRIu32 == 1
#define PRIu32 SAL_CONFIG_MACRO_PRIu32
#endif

#if SAL_CONFIG_DEFINE_PRId32 == 1
#define PRId32 SAL_CONFIG_MACRO_PRId32
#endif

#if SAL_CONFIG_DEFINE_PRIx32 == 1
#define PRIx32 SAL_CONFIG_MACRO_PRIx32
#endif

#if SAL_CONFIG_DEFINE_PRIu64 == 1
#define PRIu64 SAL_CONFIG_MACRO_PRIu64
#endif

#if SAL_CONFIG_DEFINE_PRId64 == 1
#define PRId64 SAL_CONFIG_MACRO_PRId64
#endif

#if SAL_CONFIG_DEFINE_PRIx64 == 1
#define PRIx64 SAL_CONFIG_MACRO_PRIx64
#endif

#ifndef offsetof
#define offsetof(_s, _m) ((unsigned long)&(((_s *)0)->_m))
#endif

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef VOLATILE
#define VOLATILE volatile
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef COUNTOF
#define COUNTOF(ary) ((int) (sizeof(ary) / sizeof((ary)[0])))
#endif

#ifndef COMPILER_REFERENCE
#define COMPILER_REFERENCE(_a) ((void)(_a))
#endif

/*!
 * Compiler attribute keyword.
 *
 * We use this only for enhanced code validation, so it does not need
 * to be fully portable.
 */
#ifndef SAL_ATTR
#  if defined(__GNUC__) && !defined(__PEDANTIC__)
#    define SAL_ATTR(_a) __attribute__(_a)
#  else
#    define SAL_ATTR(_a)
#  endif
#endif

/*!
 * \endcond
 */

#endif /* SAL_TYPES_H */
