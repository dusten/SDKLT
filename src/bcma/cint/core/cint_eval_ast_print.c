/*
 * $Id: cint_eval_ast_print.c,v 1.8 2010/09/22 13:57:50 dkelley Exp $
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
 * File:        cint_eval_ast_print.c
 * Purpose:     CINT print function
 */

#include "cint_eval_ast_print.h"
#include "cint_porting.h"
#include "cint_variables.h"
#include "cint_internal.h"
#include "cint_error.h"
#include "cint_debug.h"
#include "cint_eval_asts.h"


cint_variable_t*
cint_eval_ast_Print(cint_ast_t* ast)
{
    cint_variable_t* v = NULL;

    if(ast) {

        switch(ast->utype.print.expression->ntype)
            {
                /*
                 * Make the output nicer for immediate strings and integers.
                 */
            case cintAstString:
                {
                    /* print immediate string value */
                    char *tmp, *nl;

                    tmp = cint_cstring_value
                        (ast->utype.print.expression->utype.string.s);
                    CINT_PRINTF("%s", tmp);

                    /* Check if the string terminates in a newline. If
                       not, then issue one. */
                    nl=CINT_STRRCHR(tmp, '\n');
                    if (nl == NULL || nl[1] != 0) {
                        /*
                          nl is NULL if there were no newlines in the
                          string at all. nl[0] is non-zero if the
                          last newline in the string is not before the
                          terminating null.
                         */
                        CINT_PRINTF("\n");
                    }
                    CINT_FREE(tmp);
                    break;
                }
            case cintAstInteger:
                {
                    /* print immediate integer value */
                    CINT_PRINTF("%ld\n", ast->utype.print.expression->utype.integer.i);
                    break;
                }
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
            case cintAstLongLong:
                {
                    long long i =
                        ast->utype.print.expression->utype._longlong.i;
                    char buf1[50], *s1;
                    s1 = cint_lltoa(buf1, sizeof(buf1), i, 1, 10, 0);
                    /* print immediate integer value */
                    CINT_PRINTF("%s\n", s1);
                    break;
                }
#endif
            case cintAstType:
                {
                    /* Lookup the type and print information about it */
                    cint_datatype_t dt;
                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    if(cint_datatype_find(ast->utype.print.expression->utype.type.s, &dt) == CINT_E_NONE) {
                        switch((dt.flags & CINT_DATATYPE_FLAGS_TYPE))
                            {
                            case CINT_DATATYPE_F_ATOMIC:
                                {
                                    CINT_PRINTF("%s: atomic datatype, size %d bytes\n", dt.basetype.ap->name, dt.basetype.ap->size);
                                    break;
                                }
                            case CINT_DATATYPE_F_STRUCT:
                                {
                                    const cint_parameter_desc_t* sm;
                                    CINT_PRINTF("struct %s {\n", dt.basetype.sp->name);
                                    for(sm = dt.basetype.sp->struct_members; sm->basetype; sm++) {
                                        cint_datatype_t dt1;
                                        dt1.desc = *sm;
                                        CINT_PRINTF("    %s %s;\n", cint_datatype_format(&dt1, 0), sm->name);
                                    }
                                    CINT_PRINTF("}\n");
                                    CINT_PRINTF("size is %d bytes\n", dt.basetype.sp->size);
                                    break;
                                }
                            case CINT_DATATYPE_F_ENUM:
                                {
                                    const cint_enum_map_t* ep;
                                    CINT_PRINTF("enum %s {\n", dt.basetype.ep->name);
                                    for(ep = dt.basetype.ep->enum_map; ep->name; ep++) {
                                        CINT_PRINTF("    %s = %d\n", ep->name, ep->value);
                                    }
                                    CINT_PRINTF("}\n");
                                    break;
                                }
                            case CINT_DATATYPE_F_FUNC_POINTER:
                                {
                                    cint_parameter_desc_t* p = dt.basetype.fpp->params;
                                    CINT_PRINTF("function pointer: '%s (*%s)", cint_datatype_format_pd(p, 0), dt.basetype.fpp->name);
                                    cint_fparams_print(p+1);
                                    CINT_PRINTF("'\n");
                                    break;
                                }
                            case CINT_DATATYPE_F_TYPEDEF:
                                {
                                    /* Nothing yet */
                                    break;
                                }
                            }
                    }
                else {
                        /* Should never get here */
                    }
                    break;
                }
            default:
                {
                    v = cint_eval_ast(ast->utype.print.expression);

                    if(v) {
                        cint_variable_print(v, 0, v->name);
                    }
                    else {
                        /* No error necessary -- an error would have been printed during the evaluation */
                    }
                    break;
                }
            }
    }

    return v;
}
