/*
 * $Id: cint_parser.h,v 1.10 2013/05/10 21:02:45 dkelley Exp $
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
 * File:        cint_parser.h
 * Purpose:     CINT parser interfaces
 */

/*******************************************************************************
 *
 * CINT C Parser Interface
 *
 * This is the object-oriented interface to the lexical scanner
 * and generated parser state machine.
 */

#ifndef __CINT_CPARSER_H__
#define __CINT_CPARSER_H__

#include "cint_config.h"
#include "cint_ast.h"

typedef struct cint_cparser_s {

    /* Private Implementation Members */
    cint_ast_t* result;
    void* scanner;
    void* parser;
    int error;

} cint_cparser_t;


/*
 * Create a C Parser Instance
 */
extern cint_cparser_t* cint_cparser_create(void);


/*
 * Set the input file handle
 */
int cint_cparser_start_handle(cint_cparser_t* cp, void* handle);

/*
 * Set the input string
 */
int cint_cparser_start_string(cint_cparser_t* cp, const char* string);

/*
 * Parse one C translation unit from the current input handle.
 */
cint_ast_t* cint_cparser_parse(cint_cparser_t* cp);

/*
 * Parse one C translation unit from the given input string.
 */
cint_ast_t* cint_cparser_parse_string(const char* string);

/*
 * Retrieve the error code from the last request for parsing
 */
int cint_cparser_error(cint_cparser_t* cp);

/*
 * Destroy a C Parser Instance
 */
int cint_cparser_destroy(cint_cparser_t* cp);

/*
 * Handle a parser fatal error
 */
extern void cint_cparser_fatal_error(char *msg);

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
extern int cint_cparser_input_readline(void *in, char* buf,
                                       int* result, int max_size, int prompt);
#else

/*
 * Default character input
 */
extern int cint_cparser_input_default(void *in, char* buf,
                                      int* result, int max_size, int prompt);

/*
 * Default character input with echo
 */
extern int cint_cparser_input_default_echo(void *in,
                                           char* buf,
                                           int* result, int max_size,
                                           int prompt, int echo);

#endif /* CINT_CONFIG_INCLUDE_PARSER_READLINE */

/*
   Interfaces to library functions.  Declaring here avoids painful
   interactions with other header files and helps keep track of
   portability interfaces.
*/

extern void *cint_cparser_alloc(unsigned int size);
extern void *cint_cparser_realloc(void *ptr,  unsigned int size);
extern void cint_cparser_free(void *ptr);
extern void *cint_cparser_memcpy(void *dst, const void *src, int len);
extern void *cint_cparser_memset(void *dst, int c, int len);
extern void cint_cparser_message(const char *msg, int len);
extern int cint_cparser_interactive(void);
extern int cint_cparser_include(int level);
extern const char *cint_cparser_set_prompt(const char *prompt);

#endif /* __CINT_CPARSER_H__ */

