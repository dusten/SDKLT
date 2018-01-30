/*
 * $Id: cint_operators.h,v 1.12 2010/08/09 18:59:59 dkelley Exp $
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
 * File:        cint_operators.h
 * Purpose:     CINT operator interfaces
 */

#ifndef __CINT_OPERATORS_H__
#define __CINT_OPERATORS_H__

#include "cint_config.h"
#include "cint_types.h"

/*
 * Operator Flags
 */
#define CINT_OPERATOR_F_ACCEPT_INTEGRAL         0x1
#define CINT_OPERATOR_F_ACCEPT_DOUBLE           0x2
#define CINT_OPERATOR_F_ACCEPT_POINTER          0x4
#define CINT_OPERATOR_F_ACCEPT_ALL              0x8

#define CINT_OPERATOR_F_LEFT                    0x100
#define CINT_OPERATOR_F_RIGHT                   0x200
#define CINT_OPERATOR_F_TYPE_CHECK              0x400
#define CINT_OPERATOR_F_OPTIONAL                0x800
#define CINT_OPERATOR_F_LOGICAL                 0x1000


#define CINT_OPERATOR_FLAGS_UNARY CINT_OPERATOR_F_RIGHT
#define CINT_OPERATOR_FLAGS_IUNARY CINT_OPERATOR_FLAGS_UNARY | CINT_OPERATOR_F_ACCEPT_INTEGRAL
#define CINT_OPERATOR_FLAGS_AUNARY CINT_OPERATOR_FLAGS_IUNARY | CINT_OPERATOR_F_ACCEPT_DOUBLE
#define CINT_OPERATOR_FLAGS_PUNARY CINT_OPERATOR_FLAGS_AUNARY | CINT_OPERATOR_F_ACCEPT_POINTER

#define CINT_OPERATOR_FLAGS_BINARY CINT_OPERATOR_F_LEFT | CINT_OPERATOR_F_RIGHT | CINT_OPERATOR_F_TYPE_CHECK
#define CINT_OPERATOR_FLAGS_IBINARY CINT_OPERATOR_FLAGS_BINARY | CINT_OPERATOR_F_ACCEPT_INTEGRAL
#define CINT_OPERATOR_FLAGS_ABINARY CINT_OPERATOR_FLAGS_IBINARY | CINT_OPERATOR_F_ACCEPT_DOUBLE
#define CINT_OPERATOR_FLAGS_PBINARY CINT_OPERATOR_FLAGS_ABINARY | CINT_OPERATOR_F_ACCEPT_POINTER


typedef enum cint_operator_e {

#define CINT_OPERATOR_LIST_ENTRY(name, _entry, f) cintOp##_entry ,

#include "cint_op_entry.h"

    cintOpLast

} cint_operator_t;


/*
 * Operator operand types
 */
typedef enum cint_operand_type_e {

    cintOperandInt,
    cintOperandUInt,
    cintOperandLong,
    cintOperandULong,
    cintOperandLongLong,
    cintOperandULongLong,
    cintOperandDouble,
    cintOperandPointer

} cint_operand_type_t;

/*
 * Evaluate an operator AST.
 */
struct cint_ast_s;
struct cint_variable_s;

extern struct cint_variable_s*
cint_eval_operator(struct cint_ast_s* opnode);

extern cint_operator_t
cint_operator_type(struct cint_ast_s* ast);

struct cint_variable_s*
cint_eval_operator_internal(struct cint_ast_s* ast, cint_operator_t op,
                            struct cint_variable_s* left,
                            struct cint_variable_s* right);

extern const char*
cint_operator_name(cint_operator_t op);

#endif /* __CINT_OPERATORS_H__ */
