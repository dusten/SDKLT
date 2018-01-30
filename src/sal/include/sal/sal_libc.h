/*! \file sal_libc.h
 *
 * STandard C functions.
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

#ifndef SAL_LIBC_H
#define SAL_LIBC_H

#include <sal/sal_types.h>

/* System MUST supply stdarg.h */
#include <stdarg.h>

/*!
 * \cond SAL_STDC
 *
 * Standard C library functions.
 */

#ifndef sal_memcmp
extern int sal_memcmp(const void *s1, const void *s2, size_t cnt);
#endif

#ifndef sal_memcpy
extern void *sal_memcpy(void *dest, const void *src, size_t cnt);
#endif

#ifndef sal_memset
extern void *sal_memset(void *dest, int c, size_t cnt);
#endif

#ifndef sal_memmove
extern void *sal_memmove(void *dest, const void *src, size_t cnt);
#endif

#ifndef sal_strcpy
extern char *sal_strcpy(char *dest, const char *src);
#endif

#ifndef sal_strncpy
extern char *sal_strncpy(char *dest, const char *src, size_t cnt);
#endif

#ifndef sal_strlen
extern size_t sal_strlen(const char *str);
#endif

#ifndef sal_strcmp
extern int sal_strcmp(const char *dest, const char *src);
#endif

#ifndef sal_strncmp
extern int sal_strncmp(const char *dest, const char *src, size_t cnt);
#endif

#ifndef sal_strchr
extern char *sal_strchr(const char *dest, int c);
#endif

#ifndef sal_strrchr
extern char *sal_strrchr(const char *dest, int c);
#endif

#ifndef sal_strstr
extern char *sal_strstr(const char *dest, const char *src);
#endif

#ifndef sal_strcat
extern char *sal_strcat(char *dest, const char *src);
#endif

#ifndef sal_strncat
extern char *sal_strncat(char *dest, const char *src, size_t cnt);
#endif

/* ANSI/ISO ctype.h */

#ifndef sal_tolower
extern char sal_tolower(char c);
#endif

#ifndef sal_toupper
extern char sal_toupper(char c);
#endif

#ifndef sal_isspace
extern int sal_isspace(int c);
#endif

#ifndef sal_isupper
extern int sal_isupper(int c);
#endif

#ifndef sal_islower
extern int sal_islower(int c);
#endif

#ifndef sal_isalpha
extern int sal_isalpha(int c);
#endif

#ifndef sal_isdigit
extern int sal_isdigit(int c);
#endif

#ifndef sal_isalnum
extern int sal_isalnum(int c);
#endif

#ifndef sal_isxdigit
extern int sal_isxdigit(int c);
#endif

/* ANSI/ISO stdlib.h */

#ifndef sal_strtol
extern long sal_strtol(const char *s, char **end, int base);
#endif

#ifndef sal_strtoul
extern unsigned long sal_strtoul(const char *s, char **end, int base);
#endif

#ifndef sal_strtoll
extern long long sal_strtoll(const char *s, char **end, int base);
#endif

#ifndef sal_strtoull
extern unsigned long long sal_strtoull(const char *s, char **end, int base);
#endif

#ifndef sal_atoi
extern int sal_atoi(const char *s);
#endif

#ifndef sal_abs
extern int sal_abs(int j);
#endif

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif

#ifndef sal_rand
extern int sal_rand(void);
#endif

#ifndef sal_srand
extern void sal_srand(unsigned seed);
#endif

#ifndef sal_qsort
extern void sal_qsort(void *arr, size_t numel,
                      size_t elsz, int (*cmpfn)(const void *, const void *));
#endif

/* ANSI/ISO stdio.h */

#ifndef sal_vsnprintf
extern int sal_vsnprintf(char *buf, size_t bufsz, const char *fmt, va_list ap);
#endif

#ifndef sal_vsprintf
extern int sal_vsprintf(char *buf, const char *fmt, va_list ap);
#endif

#ifndef sal_snprintf
extern int sal_snprintf(char *buf, size_t bufsz, const char *fmt, ...);
#endif

#ifndef sal_sprintf
extern int sal_sprintf(char *buf, const char *fmt, ...);
#endif

#ifndef sal_strdup
extern char *sal_strdup(const char *str);
#else
/*
 * Always use our version of strdup, which uses sal_alloc() instead of
 * malloc(), and can be freed by sal_free()
 */
#error Do not override sal_strdup
#endif


/* Non-standard ANSI/ISO functions */

#ifndef sal_strcasecmp
extern int sal_strcasecmp(const char *dest, const char *src);
#endif

#ifndef sal_strncasecmp
extern int sal_strncasecmp(const char *dest, const char *src, size_t cnt);
#endif

#ifndef sal_strlcpy
extern size_t sal_strlcpy(char *dest, const char *src, size_t cnt);
#endif

#ifndef sal_strupr
extern void sal_strupr(char *s);
#endif

#ifndef sal_strnchr
extern char *sal_strnchr(const char *dest, int c, size_t cnt);
#endif

#ifndef sal_strtok_r
extern char *sal_strtok_r(char *s1, const char *delim, char **s2);
#endif

#ifndef sal_strcasestr
extern char *sal_strcasestr(const char *dest, const char *src);
#endif

/*!
 * End of standard C library functions.
 *
 * \endcond
 */

/* Special SAL library functions */

#ifndef sal_ctoi
/*!
 * \brief Convert a string to an int type.
 *
 * Similar to atoi, but in addition to 0x it also reconizes prefix 0b
 * for binary numbers (e.g. 0b10010) and 0 for octal numbers
 * (e.g. 0347).
 *
 * If not NULL, the \c end pointer will be updated with the address of
 * the first invalid character in the string. The functionality of \c
 * end is similar to that of standard C \c strtol.
 *
 * \param [in] s Input string,
 * \param [out] end Pointer to first invalid character in \c s.
 *
 * \return Parsed integer value of input string.
 */
extern int sal_ctoi(const char *s, char **end);
#endif


/*! Internal marker used by sal_vsnprintf. */
#ifndef SAL_VSNPRINTF_X_INF
#define SAL_VSNPRINTF_X_INF     0x7ff0
#endif

#endif /* SAL_LIBC_H */
