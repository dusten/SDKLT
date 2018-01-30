/*! \file bcma_io_ctrlc.c
 *
 * System inteface for handling Ctrl-C traps.
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

#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include <bsl/bsl.h>

#include <bcma/io/bcma_io_ctrlc.h>

/*! Maximum number of nested Ctrl-C trapped function calls. */
#ifndef IO_CTRLC_LVL_MAX
#define IO_CTRLC_LVL_MAX        8
#endif

/*!
 * \brief Ctrl-C trap control.
 */
typedef struct io_ctrlc_s {

    /*! Jump buffers for Ctrl-C exceptions (level 0 is unused). */
    jmp_buf jmp_buf[IO_CTRLC_LVL_MAX + 1];

    /*! Current function nesting level. */
    int cur_lvl;

    /*! Current Ctrl-C enable state. */
    int enable;

    /*! Current Ctrl-C suspend state. */
    int suspend;

} io_ctrlc_t;

static io_ctrlc_t ctrlc_data;

static void
ctrl_c_handler(int sig)
{
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META("Ctrl-C\n")));

    if (ctrlc_data.cur_lvl > 0) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META(
                        "Current Ctrl_C level = %d\n"),
                     ctrlc_data.cur_lvl));
        /* Flush output buffers to prevent garbled output after longjmp */
        fflush(0);
        longjmp(ctrlc_data.jmp_buf[ctrlc_data.cur_lvl], 1);
    }
}

int
bcma_io_ctrlc_enable_set(int enable)
{
    struct sigaction sigact;

    if (enable != ctrlc_data.enable) {
        if (enable) {
            sigemptyset(&sigact.sa_mask);
            sigact.sa_handler = ctrl_c_handler;
            sigact.sa_flags = SA_NODEFER;
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Enable Ctrl-C\n")));
            if (sigaction(SIGINT, &sigact, NULL) < 0) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META("sigaction failed\n")));
                return -1;
            }
        } else {
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Disable Ctrl-C\n")));
            signal(SIGINT, SIG_DFL);
        }
        ctrlc_data.enable = enable;
    }
    return 0;
}

int
bcma_io_ctrlc_suspend_set(int suspend)
{
    sigset_t sigset;

    if (suspend) {
        if (ctrlc_data.suspend == 0) {
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Suspend Ctrl-C\n")));
            sigemptyset(&sigset);
            sigaddset(&sigset, SIGINT);
            sigprocmask(SIG_BLOCK, &sigset, NULL);
        }
        ctrlc_data.suspend++;
    } else if (ctrlc_data.suspend) {
        ctrlc_data.suspend--;
        if (ctrlc_data.suspend == 0) {
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Resume Ctrl-C\n")));
            sigemptyset(&sigset);
            sigaddset(&sigset, SIGINT);
            sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        }
    }
    return 0;
}

/*
 * Please refer to standard C setjmp/longjmp documentation.
 */
int
bcma_io_ctrlc_exec(int (*func)(void *), void *data, int rv_intr)
{
    int rv = rv_intr;

    /* Check nesting level */
    if (++ctrlc_data.cur_lvl >= IO_CTRLC_LVL_MAX) {
        LOG_WARN(BSL_LS_APPL_SHELL,
                 (BSL_META("Maximium Ctrl-C nesting level exceeded\n")));
        ctrlc_data.cur_lvl--;
        return 0;
    }

    if (setjmp(ctrlc_data.jmp_buf[ctrlc_data.cur_lvl]) == 0) {
        /* Normal return from setjmp */
        rv = func(data);
    }

    ctrlc_data.cur_lvl--;

    return rv;
}
