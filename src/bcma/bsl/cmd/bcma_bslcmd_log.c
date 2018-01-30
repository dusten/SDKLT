/*! \file bcma_bslcmd_log.c
 *
 * CLI BSL shell commands
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

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bsl/bcma_bslfile.h>

#include <bcma/bsl/bcma_bslcmd_log.h>

#ifndef BCMA_BSL_CONFIG_DEFAULT_LOG_FILE
#define BCMA_BSL_CONFIG_DEFAULT_LOG_FILE "bcm.log"
#endif

static int
log_on(char *log_file, int append, int quiet)
{
    bool new_file = false;
    char *cur_file = bcma_bslfile_name();

    if (!append || !cur_file || !bcma_bslfile_is_enabled() ||
        sal_strcmp(log_file, cur_file) != 0) {
        if (bcma_bslfile_open(log_file, append) < 0) {
            cli_out("%sCould not start logging\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        new_file = true;
    }

    if (bcma_bslfile_enable(1) < 0) {
        cli_out("%sCould not start logging\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!quiet) {
        cli_out("File logging %s to %s\n",
                new_file ? "started" : "continued",
                log_file);
    }

    return BCMA_CLI_CMD_OK;
}

static int
log_off(char *log_file, int quiet)
{
    if (log_file == NULL || !bcma_bslfile_is_enabled()) {
        cli_out("File logging is not active.\n");
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_bslfile_enable(0) < 0) {
        cli_out("%sCould not stop logging to %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, log_file);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!quiet) {
        cli_out("File logging to %s stopped.\n", log_file);
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bslcmd_log(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    char *file, *cur_file;
    int append = 1;
    int quiet = 0;
    const char *arg;
    bcma_cli_parse_table_t pt;

    if ((file = bcma_bslfile_name()) == NULL) {
        file = BCMA_BSL_CONFIG_DEFAULT_LOG_FILE;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        cli_out("Logging to file %s: %s\n",
                file,
                bcma_bslfile_is_enabled() ? "enabled" : "disabled");
        return BCMA_CLI_CMD_OK;
    }

    cur_file = sal_strdup(file);
    if (cur_file == NULL) {
        cli_out("%sstrdup failed.\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }
    file = cur_file;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "File", "str", &file, NULL);
    bcma_cli_parse_table_add(&pt, "Append", "bool", &append, NULL);
    bcma_cli_parse_table_add(&pt, "Quiet", "bool", &quiet, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        sal_free(file);
        return BCMA_CLI_CMD_USAGE;
    }

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        /* Default is to turn logging on */
        arg = "on";
    }

    if (sal_strcasecmp("on", arg) == 0) {
        rv = log_on(file, append, quiet);
    } else if (sal_strcasecmp("off", arg) == 0) {
        rv = log_off(file, quiet);
    } else {
        rv = BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_done(&pt);

    sal_free(cur_file);

    return rv;
}
