/*! \file bcma_clicmd_loop.c
 *
 * CLI 'loop' command.
 */
/*
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
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_loop.h>

/*! Context for Ctrl-C wrapper function loop_ctrlc. */
typedef struct loop_info_s {

    /*! CLI object */
    bcma_cli_t *cli;

    /*! CLI command arguments */
    bcma_cli_args_t *args;

    /*! Number of loop times to be executed, -1 means forever */
    int num;

    /*! Number of current loop time */
    int cnt;

} loop_info_t;

/*
 * Function to process "loop" command.
 */
static int
do_loop_ctrlc(void *data)
{
    loop_info_t *lpinfo = (loop_info_t *)data;
    bcma_cli_t *cli = lpinfo->cli;
    bcma_cli_args_t *args = lpinfo->args;
    int lpforever = (lpinfo->num == -1) ? TRUE : FALSE;
    int rv = 0;
    int lperr_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_LOOPERROR, TRUE);
    int arg_start;
    const char *c;

    /* Save the start command */
    arg_start = args->arg;

    /* Execute command in loop. */
    for (lpinfo->cnt = 0; lpforever || lpinfo->cnt < lpinfo->num;
         lpinfo->cnt++) {
        /* Restore command aguments to the start command. */
        args->arg = arg_start;

        /* Execute the command list in order. */
        while ((c = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = bcma_cli_cmd_process(cli, c);
            if (rv != BCMA_CLI_CMD_OK && lperr_stop) {
                return rv;
            }
        }
    }

    return rv;
}

int
bcma_clicmd_loop(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = 0;
    int lpnum, lpforever = FALSE;
    const char *a;
    loop_info_t loop_info, *lpinfo = &loop_info;

    if (BCMA_CLI_ARG_CNT(args) < 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    a = BCMA_CLI_ARG_GET(args);
    if (sal_strcmp(a, "*") == 0) {
        lpforever = TRUE;
    } else {
        if (bcma_cli_parse_int(a, &lpnum) < 0 || lpnum < 0) {
            cli_out("%s%s: Invalid loop count: %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args), a);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    lpinfo->cli = cli;
    lpinfo->args = args;
    lpinfo->num = lpforever ? -1 : lpnum;

    rv = bcma_cli_ctrlc_exec(cli, do_loop_ctrlc, lpinfo);

    /* Display warning message if loop is interrupted by ctrl-c or error return. */
    if (rv != BCMA_CLI_CMD_OK) {
        cli_out("%s: Warning: Looping aborted on the %dth loop\n",
                BCMA_CLI_ARG_CMD(args), lpinfo->cnt + 1);

        /* Discard the reset arguments when loop was interrupted. */
        BCMA_CLI_ARG_DISCARD(args);
    }

    return rv;
}
