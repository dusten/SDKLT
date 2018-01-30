/*
 * $Id: cint_ast.h,v 1.17 2012/03/02 16:21:39 yaronm Exp $
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
 * File:        cint_ast.h
 * Purpose:     CINT AST node interfaces
 */

#ifndef __CINT_AST_H__
#define __CINT_AST_H__

#include "cint_config.h"
#include "cint_operators.h"

typedef enum cint_ast_type_e {

    cintAstNone = -1,

#define CINT_AST_LIST_ENTRY(_entry) cintAst##_entry ,
#include "cint_ast_entry.h"

    cintAstLast

} cint_ast_type_t;

#define CINT_AST_TYPE_VALID(_e) (_e > cintAstNone && _e < cintAstLast)

typedef struct cint_ast_s {
    /* Internal Use Only */
    struct cint_ast_s* inext;


    struct cint_ast_s* next;

    const char* file;
    int line;

    int refcount;
    int noexec;
    cint_ast_type_t ntype;

    /* Node Types */
    union {

        /* Constant Integer */
        struct {
            long i;
        } integer;
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
        /* Constant Long Long */
        struct {
            long long i;
        } _longlong;
#endif

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
        /* Constant Double */
        struct {
            double d;
        } _double;
#endif


        /* Constant String */
        struct {
            const char* s;
        } string;


        /* Type */

#define CINT_AST_TYPE_F_SIGNED          0x1
#define CINT_AST_TYPE_F_UNSIGNED        0x2
#define CINT_AST_TYPE_F_STATIC          0x4
#define CINT_AST_TYPE_F_EXTERN          0x8
#define CINT_AST_TYPE_F_CONST           0x10
#define CINT_AST_TYPE_F_TYPEDEF         0x20
#define CINT_AST_TYPE_F_VOLATILE        0x40

        struct {
            const char* s;
        } type;


        /* Identifier */
        struct {
            const char* s;
        } identifier;


        /* Declaration */
        struct {
            struct cint_ast_s* type;
            int pcount;
            int num_dimension_initializers;
            struct cint_ast_s*
                dimension_initializers[CINT_CONFIG_ARRAY_DIMENSION_LIMIT];
            struct cint_ast_s* identifier;
            struct cint_ast_s* init;
        } declaration;

        struct {
            struct cint_ast_s* initializers;
        } initializer;

        /* Operator */
        struct {
            cint_operator_t op;
            struct cint_ast_s* left;
            struct cint_ast_s* right;
            struct cint_ast_s* extra;
        } operator;


        /* Function Call */
        struct {
            const char* name;
            struct cint_ast_s* parameters;
            struct cint_datatype_s* dtp;
        } function;

        /* Function Definition */
        struct {
            struct cint_ast_s* declaration;
            struct cint_ast_s* parameters;
            struct cint_ast_s* statements;
        } functiondef;

        /* Structure Definition */
        struct {
            struct cint_ast_s* name;
            struct cint_ast_s* members;
        } structuredef;

        /* Expression List */
        struct {
            struct cint_ast_s* list;
        } elist;

        /* While */
        struct {
            struct cint_ast_s* condition;
            struct cint_ast_s* statements;
            int order;
        } _while;


        /* For */
        struct {
            struct cint_ast_s* pre;
            struct cint_ast_s* condition;
            struct cint_ast_s* post;
            struct cint_ast_s* statements;
        } _for;


        /* If */
        struct {
            struct cint_ast_s* condition;
            struct cint_ast_s* statements;
            struct cint_ast_s* _else;
        } _if;

        /* Return */
        struct {
            struct cint_ast_s* expression;
        } _return;

        /* Switch */
        struct {
            struct cint_ast_s* expression;
            struct cint_ast_s* statements;
        } _switch;

        /* Case */
        struct {
            struct cint_ast_s* expression;
            struct cint_ast_s* statements;
        } _case;

        /* Enumerator */
        struct {
            struct cint_ast_s* identifier;
            struct cint_ast_s* value;
        } enumerator;

        /* EnumDef */
        struct {
            struct cint_ast_s* identifier;
            struct cint_ast_s* enumerators;
        } enumdef;

        /* Print */
        struct {
            struct cint_ast_s* expression;
        } print;

        /* Interpreter Commands */
        struct {
            struct cint_ast_s* arguments;
        } cint;

    } utype;


} cint_ast_t;

typedef enum cint_ast_const_e {
    cintAstConstHex,
    cintAstConstOctal,
    cintAstConstDecimal,
    cintAstConstChar,
    cintAstConstFloat
} cint_ast_const_t;

/* Return TRUE if the AST is of the indicated type */
#define CINT_AST(ast, ty) (((ast) != NULL) && ((ast)->ntype == cintAst##ty))

#define CINT_AST_PTR_VOID ((cint_ast_t*)0x1)
#define CINT_AST_PTR_AUTO ((cint_ast_t*)0x2)

extern cint_ast_t* cint_ast_integer(int i);
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
extern cint_ast_t* cint_ast_long_long(long long i);
#endif
#if CINT_CONFIG_INCLUDE_DOUBLES == 1
extern cint_ast_t* cint_ast_double(double d);
#endif
extern cint_ast_t* cint_ast_constant(const char* s, cint_ast_const_t ctype);
extern cint_ast_t* cint_ast_string(const char* s);
extern cint_ast_t* cint_ast_var(const char* s);
extern cint_ast_t* cint_ast_operator(cint_operator_t operator, cint_ast_t* left, cint_ast_t* right);
extern cint_ast_t* cint_ast_statement(cint_ast_t* s);
extern cint_ast_t* cint_ast_identifier(const char* s);
extern cint_ast_t* cint_ast_initializer(cint_ast_t* inits);
extern cint_ast_t* cint_ast_type(const char* s);
extern cint_ast_t* cint_ast_declaration(void);
extern cint_ast_t* cint_ast_function(cint_ast_t* expr, cint_ast_t* args);
extern cint_ast_t* cint_ast_function_def(void);
extern cint_ast_t* cint_ast_structure_def(cint_ast_t* name, cint_ast_t* members);
extern cint_ast_t* cint_ast_elist(cint_ast_t* first);
extern cint_ast_t* cint_ast_while(cint_ast_t* expr, cint_ast_t* statements, int order);
extern cint_ast_t* cint_ast_for(cint_ast_t* pre, cint_ast_t* cond, cint_ast_t* post, cint_ast_t* statements);
extern cint_ast_t* cint_ast_if(cint_ast_t* expr, cint_ast_t* statements, cint_ast_t* _else);
extern cint_ast_t* cint_ast_case(cint_ast_t* expr, cint_ast_t* statements);
extern cint_ast_t* cint_ast_switch(cint_ast_t* expr, cint_ast_t* statements);
extern cint_ast_t* cint_ast_print(cint_ast_t* expr);
extern cint_ast_t* cint_ast_cint(cint_ast_t* args);
extern cint_ast_t* cint_ast_continue(void);
extern cint_ast_t* cint_ast_break(void);
extern cint_ast_t* cint_ast_return(cint_ast_t* expr);
extern cint_ast_t* cint_ast_enumerator(cint_ast_t* identifier, cint_ast_t* value);
extern cint_ast_t* cint_ast_enumdef(cint_ast_t* identifier, cint_ast_t* enumerators);
extern cint_ast_t* cint_ast_empty(void);
extern cint_ast_t* cint_ast_statement_with_no_effect(cint_ast_t* args);

extern const char* (*cint_ast_get_file_f)(void);
extern int (*cint_ast_get_line_f)(void);

extern cint_ast_t* cint_ast_last(cint_ast_t* root);
extern int cint_ast_append(cint_ast_t* root, cint_ast_t* tail);
extern int cint_ast_count(cint_ast_t* root);
extern void cint_ast_dump(cint_ast_t* root, int indent);
extern void cint_ast_dump_single(cint_ast_t* root, int indent);
extern void cint_ast_free_all(void);
extern void cint_ast_free(cint_ast_t* ast);
extern void cint_ast_free_single (cint_ast_t* a);

extern int cint_ast_int(cint_ast_t* a);
extern const char* cint_ast_str(cint_ast_t* a);
extern cint_ast_t* cint_ast_ternary(cint_ast_t* expression,
                                    cint_ast_t* t, cint_ast_t* f);
extern cint_ast_t* cint_ast_comma(cint_ast_t* left, cint_ast_t* right);
extern cint_ast_t* cint_ast_declaration_init(cint_ast_t* spec,
                                             cint_ast_t* init);
extern cint_ast_t* cint_ast_declarator_init(cint_ast_t* decl,
                                            cint_ast_t* init);
extern cint_ast_t* cint_ast_struct_declaration(cint_ast_t* qual,
                                               cint_ast_t* decl);
extern cint_ast_t* cint_ast_pointer_declarator(cint_ast_t* ptr,
                                               cint_ast_t* decl);
extern cint_ast_t* cint_ast_identifier_declarator(cint_ast_t* ident);
extern cint_ast_t* cint_ast_array_declarator(cint_ast_t* decl,
                                             cint_ast_t* len);
extern cint_ast_t* cint_ast_function_declarator(cint_ast_t* decl,
                                                cint_ast_t* param);
extern cint_ast_t* cint_ast_pointer_indirect(cint_ast_t* val);
extern cint_ast_t* cint_ast_parameter_declaration_append(cint_ast_t* spec,
                                                         cint_ast_t* decl);
extern cint_ast_t* cint_ast_parameter_declaration(cint_ast_t* decl);
extern cint_ast_t* cint_ast_compound_statement(cint_ast_t* stmt);
extern cint_ast_t* cint_ast_function_definition(cint_ast_t* ty,
                                                cint_ast_t* decl,
                                                cint_ast_t* stmt);
extern void cint_ast_touch(cint_ast_t* ast, int delta);

#endif /* __CINT_AST_H__ */
