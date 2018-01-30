/*
 * $Id: cint_datatypes.h,v 1.20 2012/03/02 16:21:39 yaronm Exp $
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
 * File:        cint_datatypes.h
 * Purpose:     CINT datatype interfaces
 */

#ifndef __CINT_DATATYPES_H__
#define __CINT_DATATYPES_H__

#include "cint_ast.h"
#include "cint_types.h"
#include "cint_error.h"

/*
 * Generic datatype description structure
 */

#define CINT_DATATYPE_F_ATOMIC          0x1
#define CINT_DATATYPE_F_STRUCT          0x2
#define CINT_DATATYPE_F_ENUM            0x4
#define CINT_DATATYPE_F_FUNC            0x8
#define CINT_DATATYPE_F_CONSTANT        0x10
#define CINT_DATATYPE_F_FUNC_DYNAMIC    0x20
#define CINT_DATATYPE_F_FUNC_POINTER    0x40
#define CINT_DATATYPE_F_TYPEDEF         0x80
#define CINT_DATATYPE_F_ITERATOR        0x100
#define CINT_DATATYPE_F_MACRO           0x200
#define CINT_DATATYPE_F_FUNC_VARARG     0x400
#define CINT_DATATYPE_FLAGS_FUNC        (CINT_DATATYPE_F_FUNC | CINT_DATATYPE_F_FUNC_DYNAMIC)
#define CINT_DATATYPE_FLAGS_TYPE        (CINT_DATATYPE_F_ATOMIC | CINT_DATATYPE_F_STRUCT | CINT_DATATYPE_F_ENUM | CINT_DATATYPE_F_FUNC_POINTER | CINT_DATATYPE_F_TYPEDEF)

typedef struct cint_datatype_s {

    /* Flags for this datatype */
    unsigned int flags;

    /*
     * The description of this datatype
     */
    cint_parameter_desc_t desc;

    /*
     * Pointer to the description of the basetype
     */
    union {
        cint_atomic_type_t* ap;
        cint_struct_type_t* sp;
        cint_enum_type_t* ep;
        cint_constants_t* cp;
        cint_function_t* fp;
        cint_function_pointer_t* fpp;
        cint_custom_iterator_t* ip;
        cint_custom_macro_t* mp;
        void* p;
    } basetype;

    /*
     * Original type name. May be different from basetype name based on aliases or typedefs.
     * Also used for temporary storage.
     */
    char type[CINT_CONFIG_MAX_VARIABLE_NAME];

    /*
     * Custom print and assignment vectors which can be used with this datatype.
     * These are different from the atomic type definition as it this type
     * may still be an aggregate and may be treated or referenced as such in
     * addition to the custom assignment and print options.
     */
    cint_atomic_type_t* cap;

    /*
     * Used when identifying when custom operations should be used on a type
     * defined as an array of another type.
     *
     * For example, consider an atomic type defined to be a char[6] with custom
     * input and output functions.  For a variable of this type we could always
     * use the custom functions.  However, if we represented an array of this
     * type we would have to store the custom functions and only use them after
     * we dereferenced the entity sufficiently.  Storing the dimension count
     * (one in this case) of the base type allows us to do this.
 */
    int type_num_dimensions;
} cint_datatype_t;


/*
 * Use for structures created by hand
 */
#define CINT_STRUCT_TYPE_DEFINE(_struct) \
 { \
     #_struct, \
     sizeof(_struct), \
     __cint_struct_members__##_struct, \
     __cint_maddr__##_struct \
 }


extern int
cint_datatype_size(const cint_datatype_t* dt);

extern int
cint_datatype_find(const char* basetype, cint_datatype_t* dt);

extern int
cint_datatype_enum_find(const char* enumid, cint_datatype_t* dt, int* value);

typedef int (*cint_datatype_traverse_t)(void* cookie, const cint_datatype_t* dt);

extern int
cint_datatype_traverse(int flags, cint_datatype_traverse_t cb, void* cookie);

extern char*
cint_datatype_format(const cint_datatype_t* dt, int alloc);

extern char*
cint_datatype_format_pd(const cint_parameter_desc_t* pd, int alloc);

extern void cint_datatype_clear(void);
extern int cint_datatype_add_atomics(cint_atomic_type_t* types);
extern int cint_datatype_add_data(cint_data_t* data, void *dlhandle);
extern int cint_datatype_add_function(cint_function_t* f);
extern int cint_datatype_add_structure(cint_struct_type_t* s);
extern int cint_datatype_add_enumeration(cint_enum_type_t* cet);

extern int cint_datatype_constant_find(const char* name, int* c);
extern int cint_datatype_checkall(int print);
extern void cint_datatype_clear_structure(cint_struct_type_t* structure);
extern void cint_datatype_clear_function(cint_function_t* function);
extern void cint_datatype_clear_enumeration(cint_enum_type_t* enumeration);
extern void cint_datatype_delete_function(const char* fname);
extern void cint_datatype_delete_type_cache(const char* fname);

#endif /* __CINT_DATATYPES_H__ */
