/*! \file bcma_cli_cmd.c
 *
 * Main module of the CLI (Command Line Interpreter).
 *
 * This module contains the main command loop as well as lookup and
 * dispatch functions.
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
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_redir.h>
#include <bcma/cli/bcma_cli_unit.h>
#include <bcma/cli/bcma_cli.h>

#include "cli_internal.h"

/*******************************************************************************
 *
 * Private functions
 */

/*
 * Ctrl-C wrapper function for bcma_cli_cmd_process.
 */
static int
cli_cmd_process_ctrlc(void *data)
{
    bcma_cli_t *cli = (bcma_cli_t *)data;

    return bcma_cli_cmd_process(cli, cli->ibuf);
}

/*
 * Function to read CLI command input with line continuation support.
 */
static char *
cli_cmd_get(bcma_cli_t *cli, const char *prompt, char *buf, int max)
{
    int len;
    char *str = buf;
    const char *p = prompt;

    while (bcma_cli_gets(cli, p, str, max)) {
        len = sal_strlen(str);
        /* Check for command line continuation */
        if (len >= 1 && str[len - 1] == '\\') {
            /*
             * Perform a continuation prompt if command line ends
             * in a backslash for continuation.
             */
            p = "? ";
            str = str + len - 1;
            max = max - len + 1;

            continue;
        }

        /* Add command to history */
        bcma_cli_add_history(cli, buf);

        return buf;
    }

    /* Might return NULL if Ctrl-C is pressed continuously. */
    return NULL;
}

/*******************************************************************************
 *
 * Exported functions
 */

int
bcma_cli_cmd_enabled(bcma_cli_t *cli, int feature)
{
    if (feature == 0) {
        return TRUE;
    }
    return bcma_cli_unit_feature_enabled(cli, cli->cmd_unit, feature);
}

char *
bcma_cli_gets(bcma_cli_t *cli, const char *prompt, char *str, int max)
{
    if (cli && cli->gets) {
        return cli->gets(cli, prompt, str, max);
    }

    return NULL;
}

void
bcma_cli_add_history(bcma_cli_t *cli, char *str)
{
    if (cli && cli->add_history) {
        cli->add_history(str);
    }
}

int
bcma_cli_show_cmds_avail(bcma_cli_t *cli, char *prefix)
{
    bcma_cli_cmd_list_t *cl;
    bcma_cli_command_t *cmd;
    int cmds = 0;

    if (!cli || BAD_CLI(cli)) {
        return FALSE;
    }

    cli_out("Available commands:");
    cl = cli->command_root;
    while (cl) {
        cmd = cl->cmd;
        if (cmd == NULL) {
            /* Should never get here */
            continue;
        }
        if (!bcma_cli_cmd_enabled(cli, cl->feature)) {
            cl = cl->next;
            continue;
        }
        if (prefix && sal_strncmp(cmd->name, prefix, sal_strlen(prefix)) != 0) {
            cl = cl->next;
            continue;
        }
        if (cmds++) {
            cli_out(",");
        }
        cli_out(" %s", cmd->name);
        cl = cl->next;
    }
    cli_out("\n");

    return 0;
}

int
bcma_cli_cmd_lookup(bcma_cli_t *cli, const char *name,
                    bcma_cli_command_t **clicmd)
{
    bcma_cli_cmd_list_t *cl;
    bcma_cli_command_t *cmd;
    size_t maxcmp;
    int rv = BCMA_CLI_CMD_NOT_FOUND;

    if (cli == NULL) {
        return BCMA_CLI_CMD_NOT_FOUND;
    }

    cl = cli->command_root;
    maxcmp = sal_strlen(name);
    *clicmd = NULL;

    while (cl) {
        cmd = cl->cmd;
        if (cmd == NULL) {
            /* Should never get here */
            continue;
        }
        if (!bcma_cli_cmd_enabled(cli, cl->feature)) {
            cl = cl->next;
            continue;
        }
        if (bcma_cli_parse_cmp(cmd->name, name, 0)) {
            if (sal_strlen(cmd->name) == maxcmp) {
                /* Exact match */
                *clicmd = cmd;
                rv = BCMA_CLI_CMD_OK;
                break;
            }
            rv = (*clicmd) ? BCMA_CLI_CMD_AMBIGUOUS : BCMA_CLI_CMD_OK;
            *clicmd = cmd;
        }
        cl = cl->next;
    }
    return rv;
}

int
bcma_cli_cmd_dispatch(bcma_cli_t *cli, const char *str, const char **c_next)
{
    bcma_cli_args_t *args;
    bcma_cli_command_t *clicmd;
    char *cmd;
    int cmd_unit;
    int rv = BCMA_CLI_CMD_USAGE;

    args = sal_alloc(sizeof(*args), "bcmaCliCmdArgs");
    if (args == NULL) {
        cli_out("%sFailed to allocate command argument buffer\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }
    /* Parse command line */
    sal_memset(args, 0, sizeof(*args));

    if (bcma_cli_parse_args(cli, str, c_next, args) < 0) {
        sal_free(args);
        return rv;
    }

    /* Empty command */
    if (BCMA_CLI_ARG_CNT(args) == 0) {
        /* Ignore empty line */
        sal_free(args);
        return BCMA_CLI_CMD_OK;
    }

    cmd_unit = cli->cur_unit;

#if BCMA_CLI_CONFIG_UNIT_PREFIX == 1
    if ((cmd = sal_strchr(args->argv[0], ':')) != NULL) {
        *cmd++ = 0;
        if (bcma_cli_parse_int(args->argv[0], &cmd_unit) < 0) {
            cli_out("%sBad unit specification\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            sal_free(args);
            return rv;
        }
        if (!bcma_cli_unit_valid(cli, cmd_unit)) {
            /* Unit is invalid */
            cli_out("%sUnit %d is not valid\n",
                    BCMA_CLI_CONFIG_ERROR_STR, cmd_unit);
            sal_free(args);
            return rv;
        }
        cli->cmd_unit = cmd_unit;
        cli->cur_unit = cmd_unit;
        if (*cmd == 0) {
            /* Just switch default unit */
            sal_free(args);
            return BCMA_CLI_CMD_OK;
        }
    } else {
        cmd = args->argv[0];
    }
#else
    cmd = args->argv[0];
#endif

    /* Lookup command and execute if found */
    switch (bcma_cli_cmd_lookup(cli, cmd, &clicmd)) {
    case BCMA_CLI_CMD_OK:
        args->argv[0] = clicmd->name;
        /* Skip command name when passing arguments to command handlers */
        BCMA_CLI_ARG_NEXT(args);
        rv = clicmd->func(cli, args);
        if (rv == BCMA_CLI_CMD_OK && BCMA_CLI_ARG_CUR(args) != NULL) {
            /*
             * Warn about unconsumed arguments.  If a command doesn't
             * need all its arguments, it should use BCMA_CLI_ARG_DISCARD to
             * eat them.
             */
            cli_out("%s: WARNING: excess arguments ignored "
                    "beginning with '%s'\n", clicmd->name, BCMA_CLI_ARG_CUR(args));
        }
        /* Indicate command complete to redirection handler */
        bcma_cli_redir_cmd_done(cli);
        break;
    case BCMA_CLI_CMD_AMBIGUOUS:
        cli_out("%sAmbiguous command\n", BCMA_CLI_CONFIG_ERROR_STR);
        bcma_cli_show_cmds_avail(cli, args->argv[0]);
        sal_free(args);
        return BCMA_CLI_CMD_AMBIGUOUS;
    default:
        if (cli->cmd_except) {
            rv = cli->cmd_except(cli, args);
            if (rv != BCMA_CLI_CMD_NOT_FOUND) {
                break;
            }
        }
        cli_out("%sInvalid command\n", BCMA_CLI_CONFIG_ERROR_STR);
        bcma_cli_show_cmds_avail(cli, NULL);
        sal_free(args);
        return BCMA_CLI_CMD_NOT_FOUND;
    }

    /* Print out common error messages */
    switch (rv) {
    case BCMA_CLI_CMD_NO_SYM:
        cli_out("%sChip symbol information not available\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        break;
    case BCMA_CLI_CMD_BAD_ARG:
        cli_out("%sBad argument or wrong number of arguments\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        break;
    case BCMA_CLI_CMD_USAGE:
        if (clicmd != NULL) {
            bcma_cli_cmd_usage_show(cli, clicmd);
        }
        break;
    default:
        break;
    }

    sal_free(args);
    return rv;
}

int
bcma_cli_cmd_process(bcma_cli_t *cli, const char *str)
{
    const char *c_next;
    int rv = BCMA_CLI_CMD_OK;

    c_next = str;
    while (c_next && *c_next &&
           (rv != BCMA_CLI_CMD_USAGE && rv != BCMA_CLI_CMD_INTR &&
            rv != BCMA_CLI_CMD_EXIT)) {
        rv = bcma_cli_cmd_dispatch(cli, c_next, &c_next);
        bcma_cli_var_result_set(cli, rv);
    }

    return rv;
}

int
bcma_cli_add_command(bcma_cli_t *cli, bcma_cli_command_t *clicmd, int feature)
{
    bcma_cli_cmd_list_t *cl;

    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    if (clicmd == NULL) {
        bcma_cli_cmd_list_clear(cli);
        return 0;
    }

    /* Do not reinstall the same command structure */
    for (cl = cli->command_root; cl; cl = cl->next) {
        if (cl->cmd == clicmd) {
            /* Already installed */
            return 0;
        }
    }

    if (clicmd->synop == NULL) {
        clicmd->synop = "";
    }

    cl = bcma_cli_cmd_entry_create();
    if (cl == NULL) {
        return -1;
    }
    cl->cmd = clicmd;
    cl->feature = feature;
    cl->next = cli->command_root;
    cli->command_root = cl;

    return 0;
}

int
bcma_cli_cmd_exception_set(bcma_cli_t *cli, bcma_cli_cmd_except cmd_except)
{
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->cmd_except = cmd_except;

    return 0;
}

int
bcma_cli_cmd_loop(bcma_cli_t *cli)
{
    char prompt[BCMA_CLI_CONFIG_IO_MAX_PROMPT];

    if (cli->gets == NULL) {
        return -1;
    }

    bcma_cli_ctrlc_enable_set(cli, 1);
    bcma_cli_var_scope_push(cli);
    bcma_cli_var_result_set(cli, 0);

    while (TRUE) {
        int unit;

        /* Generate the prompt with unit suffix */
        unit = cli->cur_unit;
        if (unit < 0) {
            sal_snprintf(prompt, sizeof(prompt)-1,
                         "%s> ", cli->prompt);
        } else {
            sal_snprintf(prompt, sizeof(prompt)-1,
                         "%s.%d> ", cli->prompt, unit);
        }

        /* Read a command */
        if (cli_cmd_get(cli, prompt, cli->ibuf, sizeof(cli->ibuf)) == NULL) {
            cli_out("\n");
            continue;
        }

        /* Execute the command */
        if (bcma_cli_ctrlc_exec(cli, cli_cmd_process_ctrlc, cli) ==
            BCMA_CLI_CMD_EXIT) {
            break;
        }
    }

    bcma_cli_var_scope_pop(cli);
    bcma_cli_ctrlc_enable_set(cli, 0);

    return 0;
}
