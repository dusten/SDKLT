/*
 * $Id: cint_porting_stdlib.h
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
 * File:        cint_porting_stdlib.h
 * Purpose:     CINT porting stdlib interfaces
 */

#ifndef CINT_PORTING_STDLIB_H
#define CINT_PORTING_STDLIB_H

#include "cint_config.h"

/*
 * The following portability macros must be defined
 * before your can build the interpreter.
 */

/*
 * Standard C Library compatibility
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef CINT_PRINTF
#define CINT_PRINTF printf
#endif

#ifndef CINT_FPRINTF
#define CINT_FPRINTF fprintf
#endif

#ifndef CINT_VPRINTF
#define CINT_VPRINTF vprintf
#endif

#ifndef CINT_MALLOC
#define CINT_MALLOC(sz) malloc(sz)
#endif

#ifndef CINT_REALLOC
#define CINT_REALLOC realloc
#endif

#ifndef CINT_FREE
#define CINT_FREE free
#endif

#ifndef CINT_STRNCPY
#define CINT_STRNCPY strncpy
#endif

#ifndef CINT_STRRCHR
#define CINT_STRRCHR strrchr
#endif

#ifndef CINT_STRCMP
#define CINT_STRCMP strcmp
#endif

#ifndef CINT_STRSTR
#define CINT_STRSTR strstr
#endif

#ifndef CINT_MEMCPY
#define CINT_MEMCPY memcpy
#endif

#ifndef CINT_MEMSET
#define CINT_MEMSET memset
#endif

#ifndef CINT_SPRINTF
#define CINT_SPRINTF sprintf
#endif

#ifndef CINT_SNPRINTF
#define CINT_SNPRINTF snprintf
#endif

#ifndef CINT_VSNPRINTF
#define CINT_VSNPRINTF vsnprintf
#endif

#ifndef CINT_STRCAT
#define CINT_STRCAT strcat
#endif

#ifndef CINT_STRNCAT
#define CINT_STRNCAT strncat
#endif

#ifndef CINT_STRDUP
#define CINT_STRDUP strdup
#endif

#ifndef CINT_STRLEN
#define CINT_STRLEN strlen
#endif

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
#ifndef CINT_READLINE
#include <readline/readline.h>
#define CINT_READLINE readline
#endif
#ifndef CINT_READLINE_FREE
#define CINT_READLINE_FREE free
#endif
#endif

#if CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY == 1
#ifndef CINT_ADD_HISTORY
#include <readline/history.h>
#define CINT_ADD_HISTORY add_history
#endif
#endif

#ifndef CINT_GETC
#define CINT_GETC getc
#endif

#ifndef CINT_FERROR
#define CINT_FERROR ferror
#endif

#if CINT_CONFIG_FILE_IO == 1

#ifndef CINT_FOPEN
#define CINT_FOPEN fopen
#endif

#ifndef CINT_FCLOSE
#define CINT_FCLOSE fclose
#endif

#ifndef CINT_FREAD
#define CINT_FREAD fread
#endif

#endif /* CINT_CONFIG_FILE_IO */

#ifndef CINT_FATAL_ERROR
#define CINT_FATAL_ERROR(msg) do { fputs(msg, stderr); exit(2); } while(0)
#endif

#endif /* CINT_PORTING_STDLIB_H */

