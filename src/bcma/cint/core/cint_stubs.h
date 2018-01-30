/*
 * $Id:$
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
 * File:        cint_stubs.h
 * Purpose:     stubs for CINT portability interfaces
 */

#ifndef   _CINT_STUBS_H_
#define   _CINT_STUBS_H_

#ifndef CINT_CONFIG_INCLUDE_STUBS
#define CINT_CONFIG_INCLUDE_STUBS 0
#endif

#if CINT_CONFIG_INCLUDE_STUBS == 1
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <stdarg.h>

extern int cint_stub_printf(const char *format, ...);
extern int cint_stub_vprintf(const char *format, va_list ap);
extern void *cint_stub_malloc(size_t size);
extern void *cint_stub_realloc(void *ptr, size_t size);
extern void cint_stub_free(void *ptr);
extern char *cint_stub_strrchr(const char *s, int c);
extern int cint_stub_strcmp(const char *s1, const char *s2);
extern char *cint_stub_strstr(const char *s1, const char *s2);
extern void *cint_stub_memcpy(void *dest, const void *src, size_t n);
extern void *cint_stub_memset(void *s, int c, size_t n);
extern int cint_stub_sprintf(char *str, const char *format, ...);
extern int cint_stub_snprintf(char *str, size_t size, const char *format, ...);
extern int cint_stub_vsnprintf(char *str, size_t size, const char *format, va_list ap);
extern char *cint_stub_strcat(char *dest, const char *src);
extern char *cint_stub_strdup(const char *s);
extern size_t cint_stub_strlen(const char *s);
extern char *cint_stub_strncpy(char *dest, const char *src, size_t n);
extern void cint_stub_fatal_error(const char *msg);
extern size_t cint_stub_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern int cint_stub_ferror(FILE *stream);
extern int cint_stub_getc(FILE *stream);
extern int cint_stub_ferror(FILE *stream);
extern FILE *cint_stub_fopen(const char *path, const char *mode);
extern int cint_stub_fclose(FILE *stream);


#define CINT_PRINTF cint_stub_printf
#define CINT_VPRINTF cint_stub_vprintf
#define CINT_MALLOC(sz) cint_stub_malloc(sz)
#define CINT_FREE cint_stub_free
#define CINT_STRRCHR cint_stub_strrchr
#define CINT_STRCMP cint_stub_strcmp
#define CINT_STRSTR cint_stub_strstr
#define CINT_MEMCPY cint_stub_memcpy
#define CINT_MEMSET cint_stub_memset
#define CINT_SPRINTF cint_stub_sprintf
#define CINT_SNPRINTF cint_stub_snprintf
#define CINT_VSNPRINTF cint_stub_vsnprintf
#define CINT_STRCAT cint_stub_strcat
#define CINT_STRDUP cint_stub_strdup
#define CINT_STRLEN cint_stub_strlen

#define CINT_STRNCPY cint_stub_strncpy
#define CINT_FATAL_ERROR(msg) cint_stub_fatal_error(msg);

#define CINT_FERROR cint_stub_ferror
#define CINT_GETC cint_stub_getc

#ifndef CINT_CONFIG_FILE_IO
#define CINT_CONFIG_FILE_IO 0
#endif

#ifndef CINT_CONFIG_INCLUDE_PARSER_READLINE
#define CINT_CONFIG_INCLUDE_PARSER_READLINE 0
#endif

#ifndef CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY
#define CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY 0
#endif

/* File IO interfaces */
#if CINT_CONFIG_FILE_IO == 1
#define CINT_FREAD cint_stub_fread

#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
#define CINT_FOPEN cint_stub_fopen
#define CINT_FCLOSE cint_stub_fclose
#endif /* CINT_CONFIG_INCLUDE_XINCLUDE */

#endif /* CINT_CONFIG_FILE_IO */

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
#ifndef CINT_READLINE
extern char* readline(const char* p);
#define CINT_READLINE readline
#endif
#endif

#if CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY == 1
extern void add_history(char* p);
#ifndef CINT_ADD_HISTORY
#define CINT_ADD_HISTORY add_history
#endif
#endif

#endif /* CINT_CONFIG_INCLUDE_STUBS */
#endif /* _CINT_STUBS_H_ */
