/*
 * $Id: cint_internal.h,v 1.24 2012/03/02 16:21:39 yaronm Exp $
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
 * File:        cint_internal.h
 * Purpose:     CINT internal interfaces
 */

#ifndef __CINT_INTERNAL_H__
#define __CINT_INTERNAL_H__

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_types.h"
#include "cint_error.h"
#include "cint_variables.h"

extern cint_error_t
cint_variable_create(cint_variable_t** rvar,
                     const char* vname,
                     const cint_parameter_desc_t* desc,
                     unsigned vflags,
                     void* addr);


/*
 * Char operations
 */
extern cint_variable_t*
cint_auto_char(signed char c);

extern signed char
cint_char_value(cint_variable_t* v);

extern cint_variable_t*
cint_char_set(cint_variable_t* v, char c);

#define cint_uchar_set(_v, i) cint_char_set(_v, i)
#define cint_uchar_value(_v) ((unsigned char)cint_char_value(_v))

/*
 * Short operations
 */

extern cint_variable_t*
cint_auto_short(short s);

extern short
cint_short_value(cint_variable_t* v);

extern cint_variable_t*
cint_short_set(cint_variable_t* v, short s);


/*
 * Integer operations
 */
extern cint_variable_t*
cint_auto_integer(int i);

extern int
cint_integer_value(cint_variable_t* v);

extern cint_variable_t*
cint_integer_set(cint_variable_t* v, int i);

/* Unsigned treatements */
#define cint_uinteger_set(_v, i) cint_integer_set(_v, i)
#define cint_uinteger_value(_v) ((unsigned int)cint_integer_value(_v))
extern cint_variable_t* cint_auto_uinteger(unsigned int i);

/*
 * Long integer operations
 */
extern cint_variable_t*
cint_auto_long(long i);

extern long
cint_long_value(cint_variable_t* v);

extern cint_variable_t*
cint_long_set(cint_variable_t* v, long i);

/* Unsigned treatments */
#define cint_ulong_value(_v) ((unsigned long)cint_long_value(_v))
#define cint_ulong_set(_v, _i) cint_long_set(_v,_i)
extern cint_variable_t* cint_auto_ulong(unsigned long i);


/*
 * Long Long integer operations
 */
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
#define CINT_LONGLONG long long
#else
#define CINT_LONGLONG long
#endif

extern cint_variable_t*
cint_auto_longlong(CINT_LONGLONG i);

extern CINT_LONGLONG
cint_longlong_value(cint_variable_t* v);

extern cint_variable_t*
cint_longlong_set(cint_variable_t* v, CINT_LONGLONG i);

/* Unsigned treatements */
#define cint_ulonglong_value(_v) ((unsigned CINT_LONGLONG) cint_longlong_value(_v))
#define cint_ulonglong_set(_v, _i) cint_longlong_set(_v, _i);
extern cint_variable_t* cint_auto_ulonglong(unsigned CINT_LONGLONG i);




/*
 * Double operations
 */
#if CINT_CONFIG_INCLUDE_DOUBLES == 1
/* Use real double type */
#define CINT_DOUBLE double
#else
#define CINT_DOUBLE int
#endif

extern cint_variable_t*
cint_auto_double(CINT_DOUBLE d);

extern CINT_DOUBLE
cint_double_value(cint_variable_t* v);

extern cint_variable_t*
cint_double_set(cint_variable_t* v, CINT_DOUBLE d);

extern int
cint_logical_value(cint_variable_t* v);

#define cint_auto_logical cint_auto_integer

extern void*
cint_pointer_value(cint_variable_t* v);

extern char*
cint_cstring_value(const char* s);

extern int
cint_variable_print(cint_variable_t* v, int indent, const char* vname);

extern int
cint_variable_printf(cint_variable_t* v);

extern int
cint_chartoi(const char* str, int* rv);

extern int
cint_ctoi(const char* str, long* rv);

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
extern int
cint_ctolli(const char* str, long long* rv);
extern char *
cint_lltoa(char *buf, int len, unsigned long long num, int sign, int base, int prec);
#endif

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
extern int
cint_ctod(const char* str, double* rv);
#endif

extern int
cint_is_integer(cint_variable_t* v);

extern int
cint_is_enum(cint_variable_t* v);

extern cint_variable_t*
cint_auto_pointer(const char* type, void* value);

extern int
cint_is_pointer(cint_variable_t* v);

extern int
cint_is_vpointer(cint_variable_t* v);

extern unsigned
cint_atomic_flags(cint_variable_t* v);

extern int
cint_constant_find(const char* name, int* c);

extern int
cint_type_compatible(cint_datatype_t* dt1, cint_datatype_t* dt2);

extern int
cint_type_check(cint_datatype_t* dt1, cint_datatype_t* dt2);

int
cint_eval_type_list(cint_ast_t* ast, char* basetype, int len,
                    unsigned* flags, int is_variable_declaration);

extern cint_variable_t*
cint_auto_string(const char* s);

extern cint_variable_t*
cint_variable_clone(cint_variable_t* v);

extern cint_variable_t*
cint_variable_address(cint_variable_t* v);

extern char*
cint_strlcpy(char* dst, const char* src, int size);

extern void*
cint_maddr_dynamic_struct_t(void*p, int mnum, cint_struct_type_t* parent);

extern int
cint_parameter_count(cint_parameter_desc_t* pd);

extern int
cint_parameter_void(cint_parameter_desc_t* pd);

extern void
cint_fparams_print(cint_parameter_desc_t* pd);

extern int
cint_snprintf_ex(char** dst, int* size, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 3, 4)));

#if CINT_CONFIG_INCLUDE_CINT_LOAD == 1
extern void
cint_ast_cint_cmd_unload(void *handle);
#endif

extern void cint_variable_data_copy(cint_variable_t* dst,
                                    cint_variable_t* src);

extern int
cint_array_combine_dimensions(cint_parameter_desc_t * type,
                              const cint_parameter_desc_t * parameter);

#endif /* __CINT_INTERNAL_H__ */
