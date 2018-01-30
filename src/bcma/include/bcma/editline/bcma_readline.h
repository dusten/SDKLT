/*! \file bcma_readline.h
 *
 * Top-level header file for readline API.
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

#ifndef BCMA_READLINE_H
#define BCMA_READLINE_H

/*
 * By default we use the self-contained and compact editline
 * implementation of the major readline APIs.
 *
 * If BCMA_NO_EDITLINE is defined, we will expect an external version
 * of the GNU readline library to be present.
 */
#ifndef BCMA_NO_EDITLINE
#define BCMA_NO_EDITLINE 0
#endif

#if BCMA_NO_EDITLINE == 1

/*******************************************************************************
 * Definitions for GNU readline
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

/*
 * Dummy functions to allow seamless switching between editline and
 * GNU readline, i.e. w/o any need for #ifdefs.
 */
#include <bcma/editline/bcma_editline_io.h>

static inline int
bcma_editline_init(bcma_editline_io_cb_t *io_cb,
                   bcma_editline_term_keys_t *term_keys)
{
    return 0;
}

static inline int
bcma_editline_cleanup(void)
{
    clear_history();
    return 0;
}

/*
 * The readline functions may use a different set of allocation APIs
 * than the rest of the application, so we need special versions to
 * allow the application to free readline-allocated memory and vice
 * versa.
 */

#define bcma_rl_alloc(_sz) \
    malloc(_sz)

#define bcma_rl_free(_ptr) \
    free(_ptr)

#define bcma_rl_strdup(_str) \
    strdup(_str)

#else /* BCMA_NO_EDITLINE == 0 */

/*******************************************************************************
 * Definitions for SDK readline (editline)
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/editline/bcma_editline.h>

#define bcma_rl_alloc(_sz) \
    sal_alloc(_sz, "bcmaCliEditline")

#define bcma_rl_free(_ptr) \
    sal_free(_ptr)

#define bcma_rl_strdup(_str) \
    sal_strdup(_str)

#endif /* BCMA_NO_EDITLINE */

/*
 * Multiple CLI instances are created to implement different shell
 * interfaces, but all the CLI instances share the same terminal input
 * function, which normally is readline.
 *
 * For readline completion support we need to know the current CLI
 * context, because this affects how the CLI command parameters should
 * be completed.
 *
 * The cur_cli variable is used to identify the currently active CLI
 * instance in order to supply the correct CLI context for readline
 * completion support.
 */
void *bcma_readline_context;

static inline void
bcma_readline_context_set(void *context)
{
    bcma_readline_context = context;
}

static inline void*
bcma_readline_context_get(void)
{
    return bcma_readline_context;
}

#endif /* BCMA_READLINE_H */
