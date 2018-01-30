/*! \file bcma_salcmd_thread.c
 *
 * Test thread API.
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

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_time.h>
#include <sal/sal_sleep.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/sal/bcma_salcmd_thread.h>

static void
dump_threads(void)
{
    shr_pb_t *pb;

    pb = shr_pb_create();
    shr_thread_dump(pb);
    cli_out("Installed threads:\n%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

static void
test_thread(shr_thread_ctrl_t *tc, void *arg)
{
    sal_usecs_t to_usec;

    assert(arg);
    to_usec = *((sal_usecs_t *)arg);

    cli_out("[Test thread]: Start\n");

    if (to_usec) {
        while (1) {
            cli_out("[Test thread]: Sleep\n");
            shr_thread_sleep(tc, to_usec);
            cli_out("[Test thread]: Wake up\n");
            if (shr_thread_stopping(tc)) {
                break;
            }
            cli_out("[Test thread]: Processing\n");
            sal_sleep(1);
        }
    }
    cli_out("[Test thread]: Exit\n");
}

int
bcma_salcmd_thread(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    static shr_thread_ctrl_t *tc;
    sal_usecs_t to_usecs = SHR_THREAD_FOREVER;
    int rv = BCMA_CLI_CMD_FAIL;
    int to_val;
    const char *cmd;
    const char *to_str;

    cmd = BCMA_CLI_ARG_GET(args);
    if (!cmd) {
        return BCMA_CLI_CMD_USAGE;
    }

    to_str = BCMA_CLI_ARG_GET(args);
    if (to_str) {
        if (bcma_cli_parse_int(to_str, &to_val) < 0) {
            cli_out("%sInvalid timeout value: '%s'\n",
                    BCMA_CLI_CONFIG_ERROR_STR, to_str);
            return BCMA_CLI_CMD_FAIL;
        }
        if (to_val < 0) {
            to_usecs = SHR_THREAD_FOREVER;
        } else {
            to_usecs = to_val * SECOND_USEC;
        }
    }

    if (sal_strcmp(cmd, "status") == 0) {
        dump_threads();
    } else if (sal_strcmp(cmd, "start") == 0) {
        if (tc) {
            cli_out("Thread already running\n");
            return BCMA_CLI_CMD_OK;
        }
        tc = shr_thread_start("utshrTestStart", -1, test_thread, &to_usecs);
        if (tc) {
            rv = BCMA_CLI_CMD_OK;
        }
    } else if (sal_strcmp(cmd, "stop") == 0) {
        if (!tc) {
            cli_out("Thread is not running\n");
            return BCMA_CLI_CMD_OK;
        }
        rv = shr_thread_stop(tc, to_usecs);
        if (SHR_SUCCESS(rv)) {
            tc = NULL;
        }
    } else if (sal_strcmp(cmd, "wake") == 0) {
        if (!tc) {
            cli_out("Thread is not running\n");
            return BCMA_CLI_CMD_OK;
        }
        rv = shr_thread_wake(tc);
    } else {
        cli_out("%sunknown thread commands: '%s'\n",
                BCMA_CLI_CONFIG_ERROR_STR, cmd);
    }

    return rv;
}
