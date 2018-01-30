/*
 * $Id: cint_types.h,v 1.21 2012/03/02 16:21:40 yaronm Exp $
 *
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
 * File:        cint_types.h
 * Purpose:     CINT type system interface
 */

#ifndef __CINT_TYPES_H__
#define __CINT_TYPES_H__

#include "cint_config.h"

/*
 * Atomic data types structure
 */

#define CINT_ATOMIC_TYPE_F_CHAR           0x1
#define CINT_ATOMIC_TYPE_F_SHORT          0x2
#define CINT_ATOMIC_TYPE_F_INT            0x4
#define CINT_ATOMIC_TYPE_F_LONG           0x8
#define CINT_ATOMIC_TYPE_F_LONGLONG       0x10
#define CINT_ATOMIC_TYPE_F_DOUBLE         0x20
#define CINT_ATOMIC_TYPE_FLAGS_MASK       0xFF  /* First Byte reserved for type flags */


#define _AF(_type) CINT_ATOMIC_TYPE_F_##_type
#define CINT_ATOMIC_TYPE_FLAGS_CSI      ( _AF(CHAR) | _AF(SHORT) | _AF(INT) )
#define CINT_ATOMIC_TYPE_FLAGS_CSIL     ( _AF(CHAR) | _AF(SHORT) | _AF(INT) | _AF(LONG) )
#define CINT_ATOMIC_TYPE_FLAGS_INTEGRAL ( CINT_ATOMIC_TYPE_FLAGS_CSI | _AF(LONG) | _AF(LONGLONG) )
#define CINT_ATOMIC_TYPE_FLAGS_ARITH    ( CINT_ATOMIC_TYPE_FLAGS_INTEGRAL | _AF(DOUBLE) )

#define CINT_ATOMIC_TYPE_F_SIGNED         0x100
#define CINT_ATOMIC_TYPE_F_UNSIGNED       0x200
#define CINT_ATOMIC_TYPE_F_ABSTRACT       0x400
#define CINT_ATOMIC_TYPE_F_CAP_ONLY       0x800

typedef enum cint_atomic_format_e {
    cintAtomicFormatCintPrintVariable,
    cintAtomicFormatCintPrintf,
    cintAtomicFormatCintLast
} cint_atomic_format_t;

typedef struct cint_atomic_type_s {
    const char* name;
    int size;
    unsigned int flags;
    int (*format)(void* p, char* dst, int size, cint_atomic_format_t format);
    int (*assign)(void* p, const char* expr);
} cint_atomic_type_t;



/******************************************************************************
 *
 * Autogen Description Structures
 */

#define CINT_PARAM_IN           0x100 /* pointer is input */
#define CINT_PARAM_OUT          0x200 /* pointer is output */
#define CINT_PARAM_INOUT        (CINT_PARAM_IN|CINT_PARAM_OUT)
#define CINT_PARAM_VP           0x400 /* param is array base */
#define CINT_PARAM_VL           0x800 /* param is array length */
#define CINT_PARAM_IDX          0x0ff /* VP/VL assoc. param idx */

/*
 * Description of a named parameter or structure member.
 */
typedef struct cint_parameter_desc_s {
    const char* basetype;
    const char* name;
    int pcount;
    /*
     * Specifies a single dimension array.  When the type is registered
     * with the interpreter it is converted to num_dimensions and
     * dimensions.
     */
    int array;
    unsigned int flags;
    int num_dimensions;
    int dimensions[CINT_CONFIG_ARRAY_DIMENSION_LIMIT];
} cint_parameter_desc_t;

typedef struct cint_fparams_s {
    void* rv;
    void* args[CINT_CONFIG_MAX_FPARAMS];
} cint_fparams_t;

/*
 * Function description structure
 */

struct cint_ast_s;

typedef void (*cint_fpointer_t)(void);

typedef int (*cint_wrapper_f)(cint_fparams_t* params);

typedef struct cint_function_s {

    /* Function Name */
    const char* name;

    /* Real function address */
    cint_fpointer_t addr;

    union {
        cint_wrapper_f wrapper;             /* Static Function Wrappers */
        struct cint_ast_s* statements;      /* Interpreter Functions */
    } body;

    /* Function return value and parameter descriptions */
    cint_parameter_desc_t* params;

} cint_function_t;


/*
 * Function pointer description structure
 */

typedef struct cint_function_pointer_s {

    /* Function pointer type name */
    const char* name;
    /* Compiled function to use when referencing this type of function/callback */
    cint_fpointer_t cb;

    cint_parameter_desc_t* params;

    void* data;

} cint_function_pointer_t;


/*
 * Structure description structure
 */
typedef struct cint_struct_type_s {
    const char* name;
    int size;
    cint_parameter_desc_t* struct_members;
    void* (*maddr)(void* p, int idx, struct cint_struct_type_s* parent);
} cint_struct_type_t;


/*
 * Enum value mapping structure
 */
typedef struct cint_enum_map_s {
    const char* name;
    int value;
} cint_enum_map_t;

/*
 * Constants -- described as enum maps
 */
typedef cint_enum_map_t cint_constants_t;

/*
 * Enum description structure
 */
typedef struct cint_enum_type_s {
    const char* name;
    cint_enum_map_t* enum_map;
} cint_enum_type_t;

/*
 * Functional Language Extensions
 */
typedef struct cint_custom_iterator_s {
    const char* name;
    struct cint_ast_s* (*handler)(const char* name, struct cint_ast_s* arguments, struct cint_ast_s* statements);
} cint_custom_iterator_t;

typedef struct cint_custom_macro_s {
    const char* name;
    struct cint_ast_s* (*handler)(const char* name, struct cint_ast_s* arguments);
} cint_custom_macro_t;

/*
 * Data
 */
typedef struct cint_data_s {

    cint_atomic_type_t* atomics;
    cint_function_t* functions;
    cint_struct_type_t* structures;
    cint_enum_type_t* enums;
    cint_parameter_desc_t* typedefs;
    cint_constants_t* constants;
    cint_function_pointer_t* fpointers;
    cint_custom_iterator_t* iterators;
    cint_custom_macro_t* macros;

} cint_data_t;


/*
 * Use when declaring data in a shared library
 */
#define CINT_LOAD_DATA_SYMBOL __cint_data_load
#define CINT_LOAD_DATA(cint_data_struct) cint_data_t* CINT_LOAD_DATA_SYMBOL = & cint_data_struct

extern int
cint_interpreter_callback(cint_function_pointer_t* cb, int nargs, int nreturn, ...);



/***********************************************************************
 *
 * The following macros can be used to declare and initialize
 * CINT datatypes, structures, and wrappers.
 */


/*
 * FUNCTION Wrapper Macros
 */

/* Start a function wrapper */
#define CINT_FWRAPPER_START    int __arg_index = 0

/* Extract a function argument */
#define CINT_FARG(_type, _name) _type _name = *(_type *)fparams->args[__arg_index++]

/* Set the return value */
#define CINT_FRET(_type) *(_type*)fparams->rv

/* VOID Parameter */
#define CINT_PARAM_VOID(name)   { "void", #name, 0, 0 }
#define CINT_ENTRY_LAST         { NULL }


#define CINT_FWRAPPER_END       /* Nothing currently */

#define CINT_FWRAPPER_ENTRY(_fname) { #_fname, (cint_fpointer_t) _fname, { __cint_fwrapper__##_fname }, __cint_parameters__##_fname }
#define CINT_FWRAPPER_NENTRY(_name, _fname) { _name, (cint_fpointer_t) _fname, { __cint_fwrapper__##_fname }, __cint_parameters__##_fname }
#define CINT_FWRAPPER_VARARG_ENTRY(_name, _fname) { _name, (cint_fpointer_t) _fname, { NULL }, NULL }

#include "cint_wrappers.h"

#endif /* __CINT_TYPES_H__ */

