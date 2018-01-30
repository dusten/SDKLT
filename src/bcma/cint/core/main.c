/*
 * $Id: main.c,v 1.17 2012/03/02 16:21:40 yaronm Exp $
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
 * File:        main.c
 * Purpose:     Standalone CINT application
 */

#include "cint_config.h"

#if CINT_CONFIG_INCLUDE_MAIN == 1

#include "cint_interpreter.h"
#include "cint_ast.h"
#include "cint_parser.h"
#include "cint_variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>

#if CINT_CONFIG_INCLUDE_TEST_DATA == 1
extern int cint_test_data_init(void);
extern int cint_test_data_deinit(void);
#endif

/*
 * Program Options
 */

static int _opt_ast = 0;
static int _opt_str = 0;
static int _opt_prompt = 1;

#ifdef YYDEBUG
extern int cint_c_debug;
#endif

static struct option long_options[] =
    {
        { "atrace", no_argument, &interp.debug.atrace, 1 },
        { "ftrace", no_argument, &interp.debug.ftrace, 1 },
        { "dtrace", no_argument, &interp.debug.dtrace, 1 },
        { "print",  no_argument, &interp.print_expr, 1 },
        { "ast", no_argument, &_opt_ast, 1 },
        { "str", no_argument, &_opt_str, 1 },
        { "noprompt", no_argument, &_opt_prompt, 0 },
#ifdef YYDEBUG
        { "yydebug", no_argument, &cint_c_debug, 1 },
#endif
        { 0, 0, 0, 0 },
    };


static char *
santize_filename(char *s)
{
    return s ? ((CINT_STRLEN(s) > PATH_MAX) ? NULL : s) : NULL;
}

#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
static void
init_include_path(void)
{
    char *path;

    path = getenv("CINT_INCLUDE_PATH");
    if (path) {
        cint_interpreter_include_set(path);
    }
}
#endif

int
main(int argc, char* argv[])
{
    FILE* fp = NULL;
    char *file_arg;

    while(1) {

        int opt_index;
        int c;

        c = getopt_long(argc, argv, "", long_options, &opt_index);

        if(c == -1) {
            break;
        }
    }
    file_arg = santize_filename(argv[optind]);

    /*
     * What actions?
     */
    if(_opt_ast) {

        /* Do not interpret. Just parse and dump the ASTs */
        cint_cparser_t* cp = cint_cparser_create();
        cint_ast_t* ast;

        if(cp == NULL) {
            CINT_PRINTF("** error creating parser object\n");
            return -1;
        }

        if(file_arg) {
#if CINT_CONFIG_FILE_IO == 1
            /* Assume file argument */
            fp = CINT_FOPEN(file_arg, "r");

            if(fp) {
                cint_cparser_start_handle(cp, fp);
            }
            else {
                /* Assume immediate string argument */
                cint_cparser_start_string(cp, file_arg);
            }
#else
            CINT_PRINTF("File IO not supported\n");
            return 1;
#endif
        }
        else {
            /* Assume stdin */
            cint_cparser_start_handle(cp, NULL);
        }

        /*
         * Parse translation units until EOF or error
         */
        while((ast = cint_cparser_parse(cp))) {
            cint_ast_dump(ast, 0);
        }

        cint_cparser_destroy(cp);

#if CINT_CONFIG_FILE_IO == 1
        if(fp) {
            CINT_FCLOSE(fp);
        }
#endif

        return 0;
    } else if (_opt_str) {
        int rv = -1;
        cint_ast_t* ast;

        /* interpret using string interface */
        if (optind < argc) {
            cint_interpreter_init();
            ast = cint_interpreter_parse_string(argv[optind]);
            if (ast) {
                rv = cint_interpreter_evaluate(ast);
            }
        }
        return (rv != 0);
    }

#if CINT_CONFIG_INCLUDE_TEST_DATA == 1
    if (cint_test_data_init() != CINT_E_NONE) {
        return (1);
    }
#endif
    cint_interpreter_init();
#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
    init_include_path();
#endif
#if CINT_CONFIG_FILE_IO == 1
    if(file_arg) {
        fp = CINT_FOPEN(file_arg, "r");
    }
#endif

    cint_interpreter_parse(fp, (_opt_prompt) ? "cint> " : NULL, 0, NULL);

#if CINT_CONFIG_FILE_IO == 1
    if(fp) {
        CINT_FCLOSE(fp);
    }
#endif

    cint_datatype_clear();
    cint_variable_clear();
#if CINT_CONFIG_INCLUDE_TEST_DATA == 1
    if (cint_test_data_deinit() != CINT_E_NONE) {
        return (1);
    }
#endif

    return 0;
}

#else /* CINT_CONFIG_INCLUDE_MAIN */
int cint_main_c_not_empty;
#endif
