/*! \file bcma_sys_conf_cli.c
 *
 * Basic initialization of SDK core libraries.
 *
 * The intention is to avoid code duplication across multiple
 * SDK-based applications, but the code may also serve as a template
 * for a customer-equivalent.
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
#include <sal/sal_assert.h>
#include <sal/sal_sysrq.h>

#include <bcma/io/bcma_io_term.h>
#include <bcma/io/bcma_io_sig.h>
#include <bcma/io/bcma_io_ctrlc.h>
#include <bcma/io/bcma_io_file.h>
#include <bcma/env/bcma_env.h>
#include <bcma/env/bcma_envcmd.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_redir.h>
#include <bcma/cli/bcma_cli_bshell.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_rcload.h>
#include <bcma/cli/bcma_clicmd_rccache.h>
#include <bcma/editline/bcma_readline.h>

#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 *
 * For use by SDK-version of readline
 */

/*
 * Console hooks required by SDK-version of readline
 */
static bcma_editline_io_cb_t el_io_cb = {
    bcma_io_term_read,
    bcma_io_term_write,
    bcma_io_term_mode_set,
    bcma_io_term_winsize_get,
    bcma_io_send_sigint,
    bcma_io_send_sigquit
};

/*******************************************************************************
 *
 * CLI input function
 */

/*
 * Readline-based terminal input
 */
static char *
io_gets(bcma_cli_t *cli, const char *prompt, char *buf, int max)
{
    char *str;
    int len;

    bcma_readline_context_set(cli);

    str = readline(prompt);
    if (str == NULL) {
        return NULL;
    }

    len = sal_strlen(str) + 1;
    if (len <= max) {
        sal_memcpy(buf, str, len);
    } else {
        char msg[80];

        sal_snprintf(msg, sizeof(msg), "echo 'Commandline too long (%d)'", len);
        sal_strlcpy(buf, msg, max);
    }
    bcma_rl_free(str);
    return buf;
}

/*
 * Add string to input history
 */
static void
io_add_history(char *str)
{
    char *p = str;

    /* Do not add string consisting of all white spaces to history */
    while (p && !sal_isspace(*p++)) {
        add_history(str);
        return;
    }
}

/*******************************************************************************
 *
 * CLI environment variables
 */

static const char *
var_get(void *cookie, const char *name)
{
    return bcma_env_var_get((bcma_env_handle_t)cookie, name);
}

static int
var_set(void *cookie, const char *name, char *value, int local)
{
    if (value == NULL) {
        return bcma_env_var_unset((bcma_env_handle_t)cookie, name, local, !local);
    }

    return bcma_env_var_set((bcma_env_handle_t)cookie, name, value, local);
}

static int
var_scope_push(void *cookie)
{
    return bcma_env_scope_push((bcma_env_handle_t)cookie);
}

static int
var_scope_pop(void *cookie)
{
    return bcma_env_scope_pop((bcma_env_handle_t)cookie);
}

static bcma_cli_var_cb_t var_cb = {
    var_get,
    var_set,
    var_scope_push,
    var_scope_pop
};

/*******************************************************************************
 *
 * CLI Ctrl-C handling
 */

static int
ctrlc_enable_set(void *cookie, int mode)
{
    return bcma_io_ctrlc_enable_set(mode);
}

static int
ctrlc_exec(void *cookie, int (*func)(void *), void *data, int rv_intr)
{
    return bcma_io_ctrlc_exec(func, data, rv_intr);
}

static bcma_cli_ctrlc_cb_t bsh_ctrlc_cb = {
    ctrlc_enable_set,
    ctrlc_exec
};

static bcma_cli_ctrlc_cb_t dsh_ctrlc_cb = {
    /* Support Ctrl-C enable/disable in main shell only. */
    NULL,
    ctrlc_exec
};

/*******************************************************************************
 *
 * CLI I/O redirection handling
 */

#define REDIR_INPUT_NAME        "/tmp/bcmcli.input.txt"
#define REDIR_OUTPUT_NAME       "/tmp/bcmcli.output.txt"

#define REDIR_LINE_SIZE_MAX     1024

/* Data for local Ctrl-C trap */
typedef struct redir_flush_s {
    bcma_io_file_handle_t ofile;
    size_t bufsz;
    char *buf;
} redir_flush_t;

static int redir_enable;

static int
redir_enable_set(void *cookie, int enable)
{
    /* Update status */
    redir_enable = enable;

    return 0;
}

/*
 * Use local Ctrl-C trap to ensure that temporary buffer is freed and
 * redirection files are deleted even if Ctrl-C is pressed while the
 * output file is being dumped.
 */
static int
redir_flush_ctrlc(void *data)
{
    redir_flush_t *rf = (redir_flush_t *)data;

    /* Dump output file line by line */
    while (bcma_io_file_gets(rf->ofile, rf->buf, rf->bufsz - 1) != NULL) {
        cli_out("%s", rf->buf);
    }

    return 0;
}

static int
redir_cmd_done(void *cookie)
{
    bcma_cli_t *cli = (bcma_cli_t *)cookie;
    int rv = 0;
    redir_flush_t redir_flush, *rf = &redir_flush;

    if (redir_enable) {
        /* Repurpose any existing output as input */
        if (bcma_io_file_move(REDIR_OUTPUT_NAME, REDIR_INPUT_NAME) != 0) {
            rv = -1;
        }
    } else {
        /* Dump output and remove temporary files */
        rf->ofile = bcma_io_file_open(REDIR_OUTPUT_NAME, "r");
        if (rf->ofile) {
            /* Allocate temporary message buffer */
            rf->bufsz = REDIR_LINE_SIZE_MAX;
            rf->buf = sal_alloc(rf->bufsz, "bcmaSysCliRedir");
            if (rf->buf) {
                sal_memset(rf->buf, 0, rf->bufsz);
                /* Dump most recent CLI command output */
                (void)bcma_cli_ctrlc_exec(cli, redir_flush_ctrlc, rf);
                sal_free(rf->buf);
            }
            bcma_io_file_close(rf->ofile);
        }
        if (bcma_io_file_delete(REDIR_INPUT_NAME) != 0) {
            rv = -1;
        }
        if (bcma_io_file_delete(REDIR_OUTPUT_NAME) != 0) {
            rv = -1;
        }
    }

    return rv;
}

static const char *
redir_input_name_get(void *cookie)
{
    return REDIR_INPUT_NAME;
}

static const char *
redir_output_name_get(void *cookie)
{
    return REDIR_OUTPUT_NAME;
}

static bcma_cli_redir_cb_t redir_cb = {
    redir_enable_set,
    redir_cmd_done,
    redir_input_name_get,
    redir_output_name_get
};

static int
redir_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    bcma_io_file_handle_t ofile;

    /* We only redirect output from the CLI */
    if (redir_enable == 0 || meta->source != BSL_SRC_SHELL) {
        return -1;
    }

    /* Append log message to output file */
    ofile = bcma_io_file_open(REDIR_OUTPUT_NAME, "a");
    if (ofile) {
        bcma_io_file_vprintf(ofile, format, args);
        bcma_io_file_close(ofile);
    }

    return 0;
}

/*******************************************************************************
 *
 * CLI command exception handling
 */

static int
cmd_exception(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *filename = BCMA_CLI_ARG_GET(args);

    return bcma_clicmd_rcload_file(cli, args, filename);
}

/*******************************************************************************
 *
 * Debug shell command
 */

/* Local copy of debug shell handle to launch debug shell. */
static bcma_cli_t *dsh;

static int
debugshell_run_ctrlc(void *data)
{
    bcma_cli_t *cli = (bcma_cli_t *)data;

    bcma_cli_cmd_loop(cli);

    return 0;
}

static int
bcma_clicmd_debugshell(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *opt;

    /* Sanity check */
    if (!dsh || !cli) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Sync current unit and command unit for debug shell */
    dsh->cur_unit = cli->cur_unit;
    dsh->cmd_unit = cli->cmd_unit;

    opt = BCMA_CLI_ARG_GET(args);
    if (opt && sal_strcmp(opt, "-c") == 0) {
        /* Execute debug command */
        bcma_cli_cmd_process(dsh, BCMA_CLI_ARG_GET(args));
    } else {
        /* Run debug shell */
        int rv;

        /*
         * Use local Ctrl-C trap to ensure that the debug shell still runs
         * when Ctrl-C is pressed.
         */
        do {
            rv = bcma_cli_ctrlc_exec(dsh, debugshell_run_ctrlc, dsh);
        } while (rv == BCMA_CLI_CMD_INTR);
    }

    return BCMA_CLI_CMD_OK;
}

/* CLI system command set */
static bcma_cli_command_t cmd_debugshell = {
    "DebugSHell",
    bcma_clicmd_debugshell,
    "Debug shell",
    "[-c <command>]",
    { "Without any options, this command will launch the debug shell.\n" \
      "With the -c option, the debug shell will execute the specified\n" \
      "command and return.\n\n" \
      "Examples:\n\n" \
      "dsh\n" \
      "dsh -c \"get CMIC_DEV_REV_IDr\"" }
};

/*******************************************************************************
 *
 * Assert handler
 */

static void
sdk_assert_handler(const char *expr, const char *file, int line)
{
    char buf[16];

    cli_out("ERROR: Assertion failed: (%s) at %s:%d\n", expr, file, line);
    sal_sysrq(SAL_SYSRQ_BACKTRACE, 0);

    cli_out("ERROR: Continue or quit [C/q]: ");
    if (bcma_io_term_read(buf, sizeof(buf)) > 0) {
        if (sal_tolower(buf[0]) == 'c' || buf[0] == '\n') {
            cli_out("WARNING: Correct behavior no longer guaranteed.\n");
            return;
        }
    }

    sal_sysrq(SAL_SYSRQ_ABORT, 0);
}

static int
bcma_cli_add_assert_handler(void)
{
    sal_assert_set(sdk_assert_handler);
    return 0;
}

/*******************************************************************************
 *
 * Common CLI initialization
 */

static int
basic_cli_init(bcma_cli_t *cli, bcma_cli_ctrlc_cb_t *ctrlc_cb)
{
    /* Add basic CLI commands */
    bcma_clicmd_add_basic_cmds(cli);
    bcma_clicmd_add_script_cmds(cli);
    bcma_clicmd_add_rc_cmds(cli);
    bcma_clicmd_add_dir_cmds(cli);
    bcma_clicmd_add_system_cmds(cli);

    /* Add support for Ctrl-C exceptions in CLI */
    bcma_cli_ctrlc_cb_set(cli, ctrlc_cb, NULL);

    /* Add support for redirection in CLI */
    bcma_cli_redir_cb_set(cli, &redir_cb, cli);

    /* Add support for command exceptions in CLI */
    bcma_cli_cmd_exception_set(cli, cmd_exception);

    return 0;
}

/*******************************************************************************
 *
 * Public functions
 */

int
bcma_sys_conf_cli_init(bcma_sys_conf_t *sc)
{
    /* Initialize SDK-version of readline */
    bcma_editline_init(&el_io_cb, NULL);

    /* Create basic CLI */
    sc->cli = bcma_cli_create();
    bcma_cli_input_cb_set(sc->cli, "BCMLT", io_gets, io_add_history);
    basic_cli_init(sc->cli, &bsh_ctrlc_cb);

    /* Add support for CLI environment variables */
    sc->eh = bcma_env_create(0);
    bcma_cli_var_cb_set(sc->cli, &var_cb, sc->eh);
    bcma_envcmd_add_cmds(sc->cli);

    /* Create debug shell */
    sc->dsh = bcma_cli_create();
    dsh = sc->dsh;
    bcma_cli_input_cb_set(sc->dsh, "sdklt", io_gets, io_add_history);
    basic_cli_init(sc->dsh, &dsh_ctrlc_cb);

    /* Debug shell uses same environment as the main CLI */
    bcma_cli_var_cb_set(sc->dsh, &var_cb, sc->eh);
    bcma_envcmd_add_cmds(sc->dsh);

    bcma_cli_add_command(sc->cli, &cmd_debugshell, 0);

    /* Create assert handler to skip core dump */
    bcma_cli_add_assert_handler();

    bcma_cli_bshell_init(sc->cli);

    return 0;
}

int
bcma_sys_conf_cli_cleanup(bcma_sys_conf_t *sc)
{
    /* Clean up SDK-version of readline */
    bcma_editline_cleanup();

    /* Clean up resources allocated by CLI command rccache */
    bcma_clicmd_rccache_clear(sc->cli);

    /* Clean up environment */
    bcma_env_destroy(sc->eh);

    /* Free remaining CLI resources */
    bcma_cli_destroy(sc->dsh);
    bcma_cli_destroy(sc->cli);

    return 0;
}

int
bcma_sys_conf_cli_redir_bsl(bsl_meta_t *meta, const char *format, va_list args)
{
    return redir_out_hook(meta, format, args);
}
