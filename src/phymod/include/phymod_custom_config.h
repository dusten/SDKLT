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
 * 
 * 
 *
 * System interface definitions for SDKLT.
 */

#ifndef PHYMOD_CUSTOM_CONFIG_H
#define PHYMOD_CUSTOM_CONFIG_H

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_alloc.h>

#include <shr/shr_error.h>

#include <phymod_custom_chip_config.h>

/* Define dummies in case PHYMOD drivers are removed from a release */
#ifndef PHYMOD_TSCE_SUPPORT
#define phymodDispatchTypeTsce phymodDispatchTypeCount
#endif
#ifndef PHYMOD_TSCF_SUPPORT
#define phymodDispatchTypeTscf phymodDispatchTypeCount
#endif
#ifndef PHYMOD_TSCBH_SUPPORT
#define phymodDispatchTypeTscbh phymodDispatchTypeCount
#endif

#define PHYMOD_DEBUG_ERROR(stuff_)      LOG_ERROR(BSL_LS_BCMPC_PHYMOD, stuff_)

#define PHYMOD_DEBUG_WARN(stuff_)       LOG_WARN(BSL_LS_BCMPC_PHYMOD, stuff_)

#define PHYMOD_DEBUG_VERBOSE(stuff_)    LOG_VERBOSE(BSL_LS_BCMPC_PHYMOD, stuff_)

#define PHYMOD_DIAG_OUT(stuff_)         cli_out stuff_

#ifndef SAL_NO_STDC
#define PHYMOD_CONFIG_DEFINE_SIZE_T     0
#define PHYMOD_CONFIG_DEFINE_UINT8_T    0
#define PHYMOD_CONFIG_DEFINE_UINT16_T   0
#define PHYMOD_CONFIG_DEFINE_UINT32_T   0
#define PHYMOD_CONFIG_DEFINE_UINT64_T   0
#define PHYMOD_CONFIG_DEFINE_INT8_T     0
#define PHYMOD_CONFIG_DEFINE_INT16_T    0
#define PHYMOD_CONFIG_DEFINE_INT32_T    0
#define PHYMOD_CONFIG_DEFINE_INT64_T    0
#define PHYMOD_CONFIG_DEFINE_PRIu32     0
#define PHYMOD_CONFIG_DEFINE_PRId32     0
#define PHYMOD_CONFIG_DEFINE_PRIx32     0
#define PHYMOD_CONFIG_DEFINE_PRIu64     0
#define PHYMOD_CONFIG_DEFINE_PRId64     0
#define PHYMOD_CONFIG_DEFINE_PRIx64     0
#else
/* Don't need to re-define the types already been defined in SAL  */
#define PHYMOD_CONFIG_DEFINE_SIZE_T     !SAL_CONFIG_DEFINE_SIZE_T
#define PHYMOD_CONFIG_DEFINE_UINT8_T    !SAL_CONFIG_DEFINE_UINT8_T
#define PHYMOD_CONFIG_DEFINE_UINT16_T   !SAL_CONFIG_DEFINE_UINT16_T
#define PHYMOD_CONFIG_DEFINE_UINT32_T   !SAL_CONFIG_DEFINE_UINT32_T
#define PHYMOD_CONFIG_DEFINE_UINT64_T   !SAL_CONFIG_DEFINE_UINT64_T
#define PHYMOD_CONFIG_DEFINE_INT8_T     !SAL_CONFIG_DEFINE_INT8_T
#define PHYMOD_CONFIG_DEFINE_INT16_T    !SAL_CONFIG_DEFINE_INT16_T
#define PHYMOD_CONFIG_DEFINE_INT32_T    !SAL_CONFIG_DEFINE_INT32_T
#define PHYMOD_CONFIG_DEFINE_INT64_T    !SAL_CONFIG_DEFINE_INT64_T
#define PHYMOD_CONFIG_DEFINE_PRIu32     !SAL_CONFIG_DEFINE_PRIu32
#define PHYMOD_CONFIG_DEFINE_PRId32     !SAL_CONFIG_DEFINE_PRId32
#define PHYMOD_CONFIG_DEFINE_PRIx32     !SAL_CONFIG_DEFINE_PRIx32
#define PHYMOD_CONFIG_DEFINE_PRIu64     !SAL_CONFIG_DEFINE_PRIu64
#define PHYMOD_CONFIG_DEFINE_PRId64     !SAL_CONFIG_DEFINE_PRId64
#define PHYMOD_CONFIG_DEFINE_PRIx64     !SAL_CONFIG_DEFINE_PRIx64
#endif

/* Allow floating point except for Linux kernel builds */
#ifndef __KERNEL__
#define PHYMOD_CONFIG_INCLUDE_FLOATING_POINT    1
#endif

/* Include register reset values in PHY symbol tables */
#define PHYMOD_CONFIG_INCLUDE_RESET_VALUES      1

/* Match PHYMOD error code with shared error codes */
#define PHYMOD_CONFIG_DEFINE_ERROR_CODES        0

typedef enum {
    PHYMOD_E_NONE       = SHR_E_NONE,
    PHYMOD_E_INTERNAL   = SHR_E_INTERNAL,
    PHYMOD_E_MEMORY     = SHR_E_MEMORY,
    PHYMOD_E_IO         = SHR_E_INTERNAL,
    PHYMOD_E_PARAM      = SHR_E_PARAM,
    PHYMOD_E_CORE       = SHR_E_PORT,
    PHYMOD_E_PHY        = SHR_E_PORT,
    PHYMOD_E_BUSY       = SHR_E_BUSY,
    PHYMOD_E_FAIL       = SHR_E_FAIL,
    PHYMOD_E_TIMEOUT    = SHR_E_TIMEOUT,
    PHYMOD_E_RESOURCE   = SHR_E_RESOURCE,
    PHYMOD_E_CONFIG     = SHR_E_CONFIG,
    PHYMOD_E_UNAVAIL    = SHR_E_UNAVAIL,
    PHYMOD_E_INIT       = SHR_E_INIT,
    PHYMOD_E_LIMIT      = SHR_E_LIMIT           /* Must come last */
} phymod_error_t;

/* Define the PHYMOD_XXX functions */
#define PHYMOD_USLEEP       sal_usleep
#define PHYMOD_SLEEP        sal_sleep
#define PHYMOD_MALLOC       sal_alloc
#define PHYMOD_FREE         sal_free
#define PHYMOD_MEMSET       sal_memset
#define PHYMOD_MEMCPY       sal_memcpy
#define PHYMOD_STRCMP       sal_strcmp
#define PHYMOD_STRNCMP      sal_strncmp
#define PHYMOD_STRNCPY      sal_strncpy
#define PHYMOD_STRCHR       sal_strchr
#define PHYMOD_STRSTR       sal_strstr
#define PHYMOD_STRLEN       sal_strlen
#define PHYMOD_STRCPY       sal_strcpy
#define PHYMOD_STRCAT       sal_strcat
#define PHYMOD_STRNCAT      sal_strncat
#define PHYMOD_STRTOUL      sal_strtoul
#define PHYMOD_SPRINTF      sal_sprintf
#define PHYMOD_SNPRINTF     sal_snprintf
#define PHYMOD_TIME_USECS   sal_time_usecs
#define PHYMOD_SRAND        sal_srand
#define PHYMOD_RAND         sal_rand

/* Define macros used in PHYMOD library */
#ifndef COMPILER_64_SET
#define COMPILER_64_SET(dst, src_hi, src_lo) \
    ((dst) = (((uint64_t)((uint32_t)(src_hi))) << 32) | \
              ((uint64_t)((uint32_t)(src_lo))))
#endif
#ifndef COMPILER_64_COPY
#define COMPILER_64_COPY(dst, src)    ((dst) = (src))
#endif
#ifndef COMPILER_64_LO
#define COMPILER_64_LO(src)           ((uint32_t)(src))
#endif
#ifndef COMPILER_64_ADD_64
#define COMPILER_64_ADD_64(dst, src)  ((dst) += (src))
#endif
#ifndef COMPILER_64_UMUL_32
#define COMPILER_64_UMUL_32(dst, src) ((dst) *= (src))
#endif
#ifndef COMPILER_64_UDIV_64
#define COMPILER_64_UDIV_64(dst, src) ((dst) /= (src))
#endif
#ifndef COMPILER_64_SHL
#define COMPILER_64_SHL(dst, bits)    ((dst) <<= (bits))
#endif
#ifndef COMPILER_64_LT
#define COMPILER_64_LT(src1, src2)    ((src1) < (src2))
#endif
#ifndef COMPILER_64_GE
#define COMPILER_64_GE(src1, src2)    ((src1) >= (src2))
#endif

#endif /* PHYMOD_CUSTOM_CONFIG_H */
