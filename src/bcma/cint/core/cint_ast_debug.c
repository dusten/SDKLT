/*
 * $Id: cint_ast_debug.c,v 1.16 2012/03/02 16:21:39 yaronm Exp $
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
 * File:        cint_ast_debug.c
 * Purpose:     CINT debug functions
 */

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_internal.h"
#include "cint_ast.h"

static void
__indent(int count)
{
    while(count--) CINT_PRINTF(" ");
}

static int
__print(int indent, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 2, 3)));

static int
__print(int indent, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __indent(indent);
    CINT_VPRINTF(fmt, args);
    va_end(args);
    return 0;
}

static int
__start_member(int indent, const char* str)
{
    return __print(indent, "{ %s\n", str);
}

static int
__end_member(int indent)
{
    return __print(indent, "}\n");
}

static int
__member(int indent, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 2, 3)));

static int
__member(int indent, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __indent(indent);
    CINT_PRINTF("{ ");
    CINT_VPRINTF(fmt, args);
    CINT_PRINTF(" }\n");
    va_end(args);
    return 0;
}

#define START(_str) __start_member(indent, _str)
#define END() __end_member(indent)
#define MEMBER(expr) __member expr


static void
__cint_ast_dump_Empty(cint_ast_t* ast, int indent)
{
}

static void
__cint_ast_dump_Integer(cint_ast_t* ast, int indent)
{
    int i = ast->utype.integer.i;
    __print(indent, "0x%x - %d - %u\n", i, i, i);
}

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
static void
__cint_ast_dump_LongLong(cint_ast_t* ast, int indent)
{
    long long i = ast->utype._longlong.i;
    char buf1[50], *s1;
    char buf2[50], *s2;
    char buf3[50], *s3;

    s1 = cint_lltoa(buf1, sizeof(buf1), i, 0, 16, 16);
    s2 = cint_lltoa(buf2, sizeof(buf2), i, 0, 10, 0);
    s3 = cint_lltoa(buf3, sizeof(buf3), i, 0, 10, 0);
    __print(indent, "0x%s - %s - %s\n", s1, s2, s3);
}
#endif

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
static void
__cint_ast_dump_Double(cint_ast_t* ast, int indent)
{
    double d = ast->utype._double.d;
    __print(indent, "%f\n", d);
}
#endif

static void
__cint_ast_dump_String(cint_ast_t* ast, int indent)
{
    __print(indent, "'%s'\n",
            ast->utype.string.s);
}


static void
__cint_ast_dump_Type(cint_ast_t* ast, int indent)
{
    __print(indent, "'%s'\n",
            ast->utype.type.s);
}


static void
__cint_ast_dump_Identifier(cint_ast_t* ast, int indent)
{
    __print(indent, "'%s'\n",
            ast->utype.identifier.s);
}

static void
__cint_ast_dump_Initializer(cint_ast_t* ast, int indent)
{
    cint_ast_dump(ast->utype.initializer.initializers, indent+4);
}

static void
__cint_ast_dump_Declaration(cint_ast_t* ast, int indent)
{
    int dimension = 0;

    START("TYPE");
    cint_ast_dump(ast->utype.declaration.type, indent+4);
    END();

    MEMBER((indent, "PCOUNT %d", ast->utype.declaration.pcount));

    START("DIMENSIONS");
    for(dimension = 0;
        dimension < CINT_CONFIG_ARRAY_DIMENSION_LIMIT;
        ++dimension) {
        cint_ast_dump(
            ast->utype.declaration.dimension_initializers[dimension],
            indent+4);
    }
    END();

    START("IDENT");
    cint_ast_dump(ast->utype.declaration.identifier, indent+4);
    END();

    START("INIT");
    cint_ast_dump(ast->utype.declaration.init, indent+4);
    END();

    MEMBER(
        (indent,
        "ARRAYDIMENSIONS %d",
        ast->utype.declaration.num_dimension_initializers));
}


static void
__cint_ast_dump_Operator(cint_ast_t* ast, int indent)
{
    MEMBER((indent, "OP = '%s'", cint_operator_name(ast->utype.operator.op)));

    START("LEFT");
    cint_ast_dump(ast->utype.operator.left, indent+4);
    END();

    START("RIGHT");
    cint_ast_dump(ast->utype.operator.right, indent+4);
    END();

    START("EXTRA");
    cint_ast_dump(ast->utype.operator.extra, indent+4);
    END();
}

static void
__cint_ast_dump_Function(cint_ast_t* ast, int indent)
{
    MEMBER((indent, "NAME = '%s'", ast->utype.function.name));

    START("PARAMS");
    cint_ast_dump(ast->utype.function.parameters, indent+4);
    END();
}

static void
__cint_ast_dump_FunctionDef(cint_ast_t* ast, int indent)
{
    START("DECLARATION");
    cint_ast_dump(ast->utype.functiondef.declaration, indent+4);
    END();

    START("PARAMETERS");
    cint_ast_dump(ast->utype.functiondef.parameters, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype.functiondef.statements, indent+4);
    END();
}

static void
__cint_ast_dump_StructureDef(cint_ast_t* ast, int indent)
{
    START("NAME");
    cint_ast_dump(ast->utype.structuredef.name, indent+4);
    END();

    START("MEMBERS");
    cint_ast_dump(ast->utype.structuredef.members, indent+4);
    END();
}

static void
__cint_ast_dump_Elist(cint_ast_t* ast, int indent)
{
    cint_ast_t* p;
    for(p = ast->utype.elist.list; p; p = p->next) {
        cint_ast_dump(p, indent);
    }
}

static void
__cint_ast_dump_Enumerator(cint_ast_t* ast, int indent)
{
    START("IDENTIFIER");
    cint_ast_dump(ast->utype.enumerator.identifier, indent+4);
    END();

    START("VALUE");
    cint_ast_dump(ast->utype.enumerator.value, indent+4);
    END();
}

static void
__cint_ast_dump_EnumDef(cint_ast_t* ast, int indent)
{
    START("IDENTIFIER");
    cint_ast_dump(ast->utype.enumdef.identifier, indent+4);
    END();

    START("ENUMERATORS");
    cint_ast_dump(ast->utype.enumdef.enumerators, indent+4);
    END();
}


static void
__cint_ast_dump_While(cint_ast_t* ast, int indent)
{
    MEMBER((indent, "ORDER = %d", ast->utype._while.order));

    START("CONDITION");
    cint_ast_dump(ast->utype._while.condition, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype._while.statements, indent+4);
    END();
}


static void
__cint_ast_dump_For(cint_ast_t* ast, int indent)
{
    START("PRE");
    cint_ast_dump(ast->utype._for.pre, indent+4);
    END();

    START("CONDITION");
    cint_ast_dump(ast->utype._for.condition, indent+4);
    END();

    START("POST");
    cint_ast_dump(ast->utype._for.post, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype._for.statements, indent+4);
    END();
}


static void
__cint_ast_dump_If(cint_ast_t* ast, int indent)
{
    START("CONDITION");
    cint_ast_dump(ast->utype._if.condition, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype._if.statements, indent+4);
    END();

    START("ELSE");
    cint_ast_dump(ast->utype._if._else, indent+4);
    END();
}

static void
__cint_ast_dump_Return(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype._return.expression, indent+4);
    END();
}

static void
__cint_ast_dump_Continue(cint_ast_t* ast, int indent)
{
    /* Nothing */
}

static void
__cint_ast_dump_Break(cint_ast_t* ast, int indent)
{
    /* Nothing */
}

static void
__cint_ast_dump_Print(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype.print.expression, indent+4);
    END();
}

static void
__cint_ast_dump_Cint(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype.cint.arguments, indent+4);
    END();
}

static void
__cint_ast_dump_Switch(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype._switch.expression, indent+4);
    END();
    START("STATEMENTS");
    cint_ast_dump(ast->utype._switch.statements, indent+4);
    END();
}

static void
__cint_ast_dump_Case(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype._case.expression, indent+4);
    END();
    START("STATEMENTS");
    cint_ast_dump(ast->utype._case.statements, indent+4);
    END();
}



struct {
    const char* name;
    void (*dump)(cint_ast_t* node, int indent);
} __ast_table[] = {

#define CINT_AST_LIST_ENTRY(_entry) { #_entry, __cint_ast_dump_##_entry },
#include "cint_ast_entry.h"
    { NULL }
};

void
cint_ast_dump_single(cint_ast_t* ast, int indent)
{
    cint_ast_type_t t;

    if(ast == NULL) {
        __print(indent, "NULL\n");
    } else if (ast == CINT_AST_PTR_VOID) {
        __print(indent, "VOID *\n");
    } else if (ast == CINT_AST_PTR_AUTO) {
        __print(indent, "AUTO\n");
    } else {

        t = ast->ntype;

        if(!CINT_AST_TYPE_VALID(t)) {
            __print(indent, "INVALID AST TYPE %d", t);
        }
        else {
            __print(indent, "{ %s\n", __ast_table[t].name);
            __ast_table[t].dump(ast, indent+4);
            __print(indent, "}\n");
        }
    }
}

void
cint_ast_dump(cint_ast_t* ast, int indent)
{
    cint_ast_dump_single(ast, indent);
    if(ast &&
       ast != CINT_AST_PTR_VOID &&
       ast != CINT_AST_PTR_AUTO && ast->next) {
        cint_ast_dump(ast->next, indent);
    }
}
