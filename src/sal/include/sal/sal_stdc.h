/*! \file sal_stdc.h
 *
 * Convenience file for mapping SAL C library functions to standard C.
 *
 * This file should never be included by regular source files as this
 * may silently break our SAL portability rules.
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

#ifndef SAL_STDC_H
#define SAL_STDC_H

#if !defined(SAL_CONFIG_H) || defined(SAL_NO_STDC)

/*
 * If we get here it means that some file other than sal_config.h
 * included this file before sal_config.h did.
 */
#error sal_stdc.h file cannot be included by regular source files

#else

/*!
 * \cond SAL_STDC
 */

#include <stddef.h>
#define SAL_CONFIG_DEFINE_SIZE_T        0

#include <inttypes.h>
#define SAL_CONFIG_DEFINE_UINT8_T       0
#define SAL_CONFIG_DEFINE_UINT16_T      0
#define SAL_CONFIG_DEFINE_UINT32_T      0
#define SAL_CONFIG_DEFINE_UINT64_T      0
#define SAL_CONFIG_DEFINE_UINTPTR_T     0
#define SAL_CONFIG_DEFINE_INT8_T        0
#define SAL_CONFIG_DEFINE_INT16_T       0
#define SAL_CONFIG_DEFINE_INT32_T       0
#define SAL_CONFIG_DEFINE_INT64_T       0
#define SAL_CONFIG_DEFINE_PRIu32        0
#define SAL_CONFIG_DEFINE_PRId32        0
#define SAL_CONFIG_DEFINE_PRIx32        0
#define SAL_CONFIG_DEFINE_PRIu64        0
#define SAL_CONFIG_DEFINE_PRId64        0
#define SAL_CONFIG_DEFINE_PRIx64        0

#include <stdbool.h>
#define SAL_CONFIG_DEFINE_BOOL_T        0

#include <string.h>
#ifndef sal_memcmp
#define sal_memcmp memcmp
#endif
#ifndef sal_memcpy
#define sal_memcpy memcpy
#endif
#ifndef sal_memset
#define sal_memset memset
#endif
#ifndef sal_memmove
#define sal_memmove memmove
#endif
#ifndef sal_strcpy
#define sal_strcpy strcpy
#endif
#ifndef sal_strncpy
#define sal_strncpy strncpy
#endif
#ifndef sal_strlen
#define sal_strlen strlen
#endif
#ifndef sal_strcmp
#define sal_strcmp strcmp
#endif
#ifndef sal_strncmp
#define sal_strncmp strncmp
#endif
#ifndef sal_strchr
#define sal_strchr strchr
#endif
#ifndef sal_strrchr
#define sal_strrchr strrchr
#endif
#ifndef sal_strstr
#define sal_strstr strstr
#endif
#ifndef sal_strcat
#define sal_strcat strcat
#endif
#ifndef sal_strncat
#define sal_strncat strncat
#endif

#include <ctype.h>
#ifndef sal_tolower
#define sal_tolower tolower
#endif
#ifndef sal_toupper
#define sal_toupper toupper
#endif
#ifndef sal_isspace
#define sal_isspace isspace
#endif
#ifndef sal_isupper
#define sal_isupper isupper
#endif
#ifndef sal_islower
#define sal_islower islower
#endif
#ifndef sal_isalpha
#define sal_isalpha isalpha
#endif
#ifndef sal_isdigit
#define sal_isdigit isdigit
#endif
#ifndef sal_isalnum
#define sal_isalnum isalnum
#endif
#ifndef sal_isxdigit
#define sal_isxdigit isxdigit
#endif

#include <stdlib.h>
#ifndef sal_exit
#define sal_exit exit
#endif
#ifndef sal_abort
#define sal_abort abort
#endif
#ifndef sal_strtol
#define sal_strtol strtol
#endif
#ifndef sal_strtoul
#define sal_strtoul strtoul
#endif
#ifndef sal_strtoll
#define sal_strtoll strtoll
#endif
#ifndef sal_strtoull
#define sal_strtoull strtoull
#endif
#ifndef sal_atoi
#define sal_atoi atoi
#endif
#ifndef sal_abs
#define sal_abs abs
#endif
#ifndef sal_rand
#define sal_rand rand
#endif
#ifndef sal_srand
#define sal_srand srand
#endif
#ifndef sal_qsort
#define sal_qsort qsort
#endif

#include <stdio.h>
#ifndef sal_vsnprintf
#define sal_vsnprintf vsnprintf
#endif
#ifndef sal_vsprintf
#define sal_vsprintf vsprintf
#endif
#ifndef sal_snprintf
#define sal_snprintf snprintf
#endif
#ifndef sal_sprintf
#define sal_sprintf sprintf
#endif

/*!
 * \endcond
 */

#endif /* SAL_CONFIG_H */

#else

/*
 * If we get here it means that some file other than sal_config.h
 * included this file after sal_config.h already included it once.
 */
#error sal_stdc.h file cannot be included by regular source files

#endif /* SAL_STDC_H */
