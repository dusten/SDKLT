/*! \file bcma_clicmd_for.c
 *
 * CLI 'for' command.
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
#include <bcma/cli/bcma_clicmd_for.h>

#define VAR_DEFAULT_FMT "%d"

/*
 * Cookie for Ctrl-C wrapper function for_ctrlc.
 */
typedef struct for_info_s {

    /*! CLI object */
    bcma_cli_t *cli;

    /*! CLI command arguments */
    bcma_cli_args_t *args;

    /*! Variable name in for-loop */
    const char *var_name;

    /*! Variable value format */
    const char *var_fmt;

    /*! Current variable value in for-loop */
    int cur_idx;

} for_info_t;

/*
 * Function to prcoess "for" command.
 */
static int
do_for_ctrlc(void *data)
{
    for_info_t *finfo = (for_info_t *)data;
    bcma_cli_t *cli = finfo->cli;
    bcma_cli_args_t *args = finfo->args;
    const char *c;
    char *s2 = NULL, *var_name;
    char *s_start, *s_stop, *s_step;
    char *fmt;
    int start, stop, step;
    int arg_start;
    char val[80];
    int rv = 0;
    int lperr_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_LOOPERROR, TRUE);

    c = BCMA_CLI_ARG_GET(args);

    var_name = sal_strtok_r((char *)c, "=", &s2);
    s_start = sal_strtok_r(NULL, ",", &s2);
    s_stop = sal_strtok_r(NULL, ",", &s2);
    s_step = sal_strtok_r(NULL, ",", &s2);
    fmt = sal_strtok_r(NULL, "\0", &s2);

    /* Check for necessary arguments */
    if (var_name == NULL || s_start == NULL || s_stop == NULL) {
        cli_out("%s%s: Invalid loop format\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
        return BCMA_CLI_CMD_FAIL;
    }

    /* Check arguments format */
    if (bcma_cli_parse_int(s_start, &start) < 0 ||
        bcma_cli_parse_int(s_stop, &stop) < 0) {
        cli_out("%s%s: Invalid loop format\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
        return BCMA_CLI_CMD_FAIL;
    }

    if (s_step) {
        if (bcma_cli_parse_int(s_step, &step) < 0 || step == 0) {
            cli_out("%s%s: Invalid loop format\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
            return BCMA_CLI_CMD_FAIL;
        }
    } else {
        /* Default step value */
        step = 1;
    }

    finfo->var_name = var_name;
    finfo->var_fmt = fmt;

    if (fmt == NULL) {
        /* Default format */
        fmt = VAR_DEFAULT_FMT;
    }

    /* Save the start command */
    arg_start = args->arg;

    /* Execute commands in for loop. */
    for (finfo->cur_idx = start;
         (step > 0 && finfo->cur_idx <= stop) ||
         (step < 0 && finfo->cur_idx >= stop);
         finfo->cur_idx += step) {
        sal_snprintf(val, sizeof(val), fmt, finfo->cur_idx, "", "", "");
        bcma_cli_var_set(cli, var_name, val, TRUE);

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
bcma_clicmd_for(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = 0;
    for_info_t for_info, *finfo = &for_info;

    if (BCMA_CLI_ARG_CNT(args) < 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    finfo->cli = cli;
    finfo->args = args;
    finfo->var_name = NULL;

    rv = bcma_cli_ctrlc_exec(cli, do_for_ctrlc, finfo);

    if (finfo->var_name == NULL) {
        return rv;
    }

    /* Display warning message if loop is interrupted by ctrl-c or error return. */
    if (rv != BCMA_CLI_CMD_OK) {
        if (finfo->var_fmt == NULL) {
            finfo->var_fmt = VAR_DEFAULT_FMT;
        }
        cli_out("%s: Warning: Looping aborted on %s=",
                BCMA_CLI_ARG_CMD(args), finfo->var_name);
        cli_out(finfo->var_fmt, finfo->cur_idx, "", "", "");
        cli_out("\n");

        /* Discard arguments interrupted in loop. */
        BCMA_CLI_ARG_DISCARD(args);
    }

    bcma_cli_var_set(cli, finfo->var_name, NULL, TRUE);

    return rv;
}
