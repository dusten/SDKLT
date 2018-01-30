/*
 * $Id: bcma_cint_cmd.c
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
 * bcma_cint_cmd.c
 */
#include <sal_config.h>
#include <bsl/bsl.h>

#include "bcma_cint_sdk_atomics.h"
#include "../core/cint_interpreter.h"
#include <bcma/cint/bcma_cint_cmd.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include "../core/cint_porting.h"
#include <bcma/io/bcma_io_file.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_bshell.h>

/* bcma_cint_bcmlt_data.c */
extern cint_data_t bcma_cint_data;

/* bcm_cint_stdclib_data.c */
extern cint_data_t stdclib_cint_data;

/* Custom functions */

static int
bshell(int unit, char *cmd)
{
    return bcma_cli_bshell(unit, cmd);
}
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                  bshell,
                  int,int,unit,0,0,
                  char*,char,cmd,1,0);

static cint_function_t __cint_cmd_functions[] =
    {
        CINT_FWRAPPER_ENTRY(bshell),
        CINT_ENTRY_LAST
    };

static cint_constants_t bcma_cint_cmd_constants[] =
{
    { "TRUE", 1 },
    { "FALSE", 0 },
    { "true", 1 },
    { "false", 0 },
    { NULL }
};

cint_data_t cmd_cint_data =
{
    NULL,
    __cint_cmd_functions,
    NULL,
    NULL,
    NULL,
    bcma_cint_cmd_constants,
    NULL
};


static int
bcma_cint_sdk_data_init(void)
{
    cint_interpreter_add_data(&bcma_cint_sdk_data, NULL);
    cint_interpreter_add_data(&stdclib_cint_data, NULL);
    cint_interpreter_add_data(&bcma_cint_data, NULL);
    cint_interpreter_add_data(&cmd_cint_data, NULL);
    return 0;
}

static int
bcma_cint_event_handler(void* cookie, cint_interpreter_event_t event)
{
    switch (event)
    {
        case cintEventReset:
            /*
             * Interpreter has reset. Re-register our data
             */
            bcma_cint_sdk_data_init();
            break;
        default:
            /* Nothing */
            break;
    }
    return 0;
}

#if (defined(VXWORKS) || defined(__KERNEL__))
static void
bcma_cint_init_path(void)
{
    char *path = NULL;
#if defined(VXWORKS)
    path = var_get("CINT_INCLUDE_PATH");
#elif defined(__KERNEL__)
    path = getenv("CINT_INCLUDE_PATH");
#endif

    if (path) {
        cint_interpreter_include_set(path);
    }
}
#endif

/* fatal error interface to lexer - abort to diag shell */
void cmd_cint_fatal_error(char *msg)
{
    CINT_PRINTF("%s",msg);
}

int
cmd_cint_initialize(void)
{
    static int init = 0;
    if (init == 0) {
        cint_interpreter_init();
        cint_interpreter_event_register(bcma_cint_event_handler, NULL);
#if (defined(VXWORKS) || defined(__KERNEL__))
        bcma_cint_init_path();
#endif
        bcma_cint_sdk_data_init();
#ifdef INCLUDE_LIB_C_UNIT
        c_unit_initialize(c_unit_context_set);
#endif
        init = 1;
    }
    return 0;
}

/* Data for local Ctrl-C trap */
typedef struct do_cint_parse_s {
    void *handle;
    const char *prompt;
    int argc;
    char **argv;
} do_cint_parse_t;

static int
do_cint_parse_ctrlc(void *data)
{
    do_cint_parse_t *cparse = (do_cint_parse_t *)data;

    cint_interpreter_parse(cparse->handle, cparse->prompt,
                           cparse->argc, cparse->argv);

    return 0;
}

static int
do_cint_parse(bcma_cli_t *cli, void *handle, const char *prompt,
              int argc, char **argv)
{
    int rv;
    do_cint_parse_t cint_parse, *cparse = &cint_parse;

    cparse->handle = handle;
    cparse->prompt = prompt;
    cparse->argc = argc;
    cparse->argv = argv;
    /*
     * Use local Ctrl-C trap to ensure that the cint shell still runs
     * when Ctrl-C is pressed.
     */
    do {
        rv = bcma_cli_ctrlc_exec(cli, do_cint_parse_ctrlc, cparse);
    } while (rv == BCMA_CLI_CMD_INTR);

    return 0;
}

static int
bcma_cmd_cint(bcma_cli_t *cli, bcma_cli_args_t* a)
{
    static bool cint_active; 
    char* s;
    int cmp;
    int argc = 0;
    char* argv[16];

    if (cint_active) {
        CINT_PRINTF("Error: cint cannot be called recursively.\n");
        return 0;
    }
    cint_active = true;

    cmd_cint_initialize();

    s = BCMA_CLI_ARG_CUR(a);
    if (s) {
        cmp = sal_strcmp(s, "allow_file_info");
        if(!cmp) {
            argv[argc] = BCMA_CLI_ARG_GET(a);
            argc++;
        }
    }

    if (BCMA_CLI_ARG_CUR(a)) {
        /* Load file */
#if CINT_CONFIG_INCLUDE_STDLIB == 1
        FILE* fp;
#else
        bcma_io_file_handle_t fp;
#endif
        s = BCMA_CLI_ARG_GET(a);

        if ((fp = CINT_FOPEN(s, "r")) != NULL) {

            sal_memset(&argv[argc], 0,
                    (sizeof(argv)-(argc*sizeof(char *))));
            while ((s = BCMA_CLI_ARG_GET(a))) {
                if (argc < 16) {
                    argv[argc] = s;
                }
                argc++;
            }

            if ( argc <= 16 ) {
                do_cint_parse(cli, fp, NULL, argc, argv);
            }
            else {
                cli_out("Error: cannot pass more than 16 "
                        "arguments, excluding the filename.\n");
            }
            CINT_FCLOSE(fp);
        }
        else {
            cli_out("Error: could not open file '%s'\n", s);
        }
    }
    else {
        cli_out("Entering C Interpreter. Type 'exit;' to quit.\n\n");
        do_cint_parse(cli, NULL, "cint> ", argc, argv);
    }

    cint_active = false;

    return 0;
}


#define BCMA_CINTCMD_HELP_DESC \
    "Starts CINT prompt, used to execute BCM LT API's"

static bcma_cli_command_t cmd_cint = {
    "cint",
    bcma_cmd_cint,
    BCMA_CINTCMD_HELP_DESC,
};

int
bcma_cintcmd_add_cint_cmd(bcma_cli_t *cli)
{
    return bcma_cli_add_command(cli, &cmd_cint, 0);
}
