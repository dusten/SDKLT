/*! \file bcma_clicmd_if.c
 *
 * CLI 'if' command.
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
#include <bcma/cli/bcma_clicmd_if.h>

/*
 * Check the condition result of string and return the boolean result.
 * If string 'c' is numeric, the boolean result is TRUE only if numeric value
 * of 'c' is not 0. Otherwise string 'c' will be took as a command line to
 * execute, and boolean result will be TRUE only if the execution result
 * is successful.
 */
static int
condition_get(bcma_cli_t *cli, const char *c)
{
    int op_not = FALSE;
    int val, cond;

    /* Process for boolean operator NOT */
    while (*c == '!') {
        c++;
        op_not = !op_not;
    }

    if (bcma_cli_parse_int(c, &val) < 0) {
        cond = (bcma_cli_cmd_process(cli, c) == BCMA_CLI_CMD_OK) ? TRUE : FALSE;
    } else  {
        cond = (val != 0) ? TRUE : FALSE;
    }

    return op_not ? !cond : cond;
}

int
bcma_clicmd_if(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = 0, cond = 0;
    int op_and = FALSE, op_or = FALSE;
    const char *c;
    int iferr_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_IFERROR, TRUE);

    /* Check condition after if command. */
    do {
        c = BCMA_CLI_ARG_GET(args);
        if (c == NULL) {
            cli_out("%s: missing test condition\n", BCMA_CLI_ARG_CMD(args));
            return BCMA_CLI_CMD_USAGE;
        }

        /*
         * If 'cond' is FALSE and followed by an AND boolean operator,
         * the condition after AND will be ignored. Similarly, if 'cond' is
         * TRUE and followd by an OR boolean operator, the condition
         * after OR will be ignored. Since these ignored condition won't
         * change the condtion result.
         */
        if ((!op_and && !op_or) || (cond && op_and) || (!cond && op_or)) {
            rv = condition_get(cli, c);
            if (op_and) {
                cond = cond && rv;
            } else if (op_or) {
                cond = cond || rv;
            } else {
                cond = rv;
            }
        }

        c = BCMA_CLI_ARG_GET(args);
        if (c == NULL) {
            return BCMA_CLI_CMD_OK;
        }

        /* Check if boolean operator AND or OR exists. */
        op_and = (sal_strcmp(c, "&&") == 0) ? TRUE : FALSE;
        if (!op_and) {
            op_or = (sal_strcmp(c, "||") == 0) ? TRUE : FALSE;
        } else {
            op_or = FALSE;
        }
    } while (op_and || op_or);

    rv = BCMA_CLI_CMD_OK;

    if (cond) {
        /* Execute commands before 'else'. */
        do {
            /* Running command list until 'else' */
            if (sal_strcasecmp(c, "else") == 0) {
                /* Discard arguments after else when condition is TRUE. */
                BCMA_CLI_ARG_DISCARD(args);
                return rv;
            }

            rv = bcma_cli_cmd_process(cli, c);
            if (rv != BCMA_CLI_CMD_OK && iferr_stop) {
                return rv;
            }
        } while ((c = BCMA_CLI_ARG_GET(args)) != NULL);
    } else {
        /* Find commands after 'else' */
        while (sal_strcasecmp(c, "else") != 0) {
            c = BCMA_CLI_ARG_GET(args);
            if (c == NULL) {
                return rv;
            }
        }

        /* Execute commands after 'else'. */
        while ((c = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = bcma_cli_cmd_process(cli, c);
            if (rv != BCMA_CLI_CMD_OK && iferr_stop) {
                return rv;
            }
        }
    }

    return rv;
}
