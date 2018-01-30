/*! \file bcmcfg_read_outer.c
 *
 * The BCMCFG outer reader is responsible for handling the outermost
 * map keys in the configuration file, and dispatching to the next
 * level of the read handlers.
 *
 * The outer reader is dispatched to additional readers to the
 * bcmcfg_read_handler structure.
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

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/bcmcfg_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_READ

/* Index of inner handlers */
#define INNER_H 1

typedef struct bcmcfg_read_outer_data_s {
    int count;
    const bcmcfg_read_handler_t * const * handler;
} bcmcfg_read_outer_data_t;

/* Forward declarations of the outer reader handler functions and data
   structure. */
static int
bcmcfg_read_outer_setup(bool start,
                        void *user_data);
static int
bcmcfg_read_outer_scalar(const char *value,
                         bcmcfg_read_context_t *context,
                         void *user_data);
static int
bcmcfg_read_outer_seq(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data);
static int
bcmcfg_read_outer_map(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data);

static int
bcmcfg_read_outer_doc(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data);

static int
bcmcfg_read_outer_abort(bcmcfg_read_context_t *context,
                        void *user_data);

static const bcmcfg_read_handler_t bcmcfg_read_outer;

/* Read handlers - Add additional handlers here. */
static const bcmcfg_read_handler_t *bcmcfg_read_handler[] = {
    &bcmcfg_read_outer,         /* MUST be first */
    &bcmcfg_read_component,
    &bcmcfg_read_device
};

static bcmcfg_read_outer_data_t bcmcfg_read_outer_data = {
    .count   = COUNTOF(bcmcfg_read_handler),
    .handler = bcmcfg_read_handler
};

/* Outer reader handler. */
static const bcmcfg_read_handler_t bcmcfg_read_outer = {
    .key = "", /* key is ignored for this handler. */
    .setup  = bcmcfg_read_outer_setup,
    .doc    = bcmcfg_read_outer_doc,
    .scalar = bcmcfg_read_outer_scalar,
    .seq    = bcmcfg_read_outer_seq,
    .map    = bcmcfg_read_outer_map,
    .abort  = bcmcfg_read_outer_abort,
    .user_data = &bcmcfg_read_outer_data
};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Outer reader init/cleanup handler.
 *
 * Initialize or cleanup outer reader handler. Currently nothing to
 * do.
 *
 * \param [in]    start         true if init, false if cleanup.
 * \param [in]    user_data     Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_setup(bool start,
                        void *user_data)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(start);
    COMPILER_REFERENCE(user_data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader scalar handler.
 *
 * Only map keys need to be dealt with, so search for an inner reader
 * handler with the given key, starting with read handler 1 (this
 * handler is at zero).
 *
 * \param [in]     value        Scalar value.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_scalar(const char *value,
                         bcmcfg_read_context_t *context,
                         void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    uint32_t i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (i = INNER_H; i < COUNTOF(bcmcfg_read_handler); i++) {
        if (!sal_strcmp(value, bcmcfg_read_handler[i]->key)) {
            break;
        }
    }

    if (i < COUNTOF(bcmcfg_read_handler)) {
        /* transition to next read handler - the handler is set in
           user_data at this level, and the subsequent map handler
           will retrieve it. */
        info->user_data = (void *)(bcmcfg_read_handler[i]);
    } else {
        SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader sequence handler.
 *
 * Currently nothing to do.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_seq(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(start);
    COMPILER_REFERENCE(context);
    COMPILER_REFERENCE(user_data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader map handler.
 *
 * Start a new mapping by picking of the new reader handler from the
 * previous level user_data set by bcmcfg_read_outer_scalar().
 *
 * \param [in]     start        true if init, false if cleanup.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_map(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_read_handler_t *new_handler;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    /* handler for this level is in user_data of previous if it exists
       - otherwise don't touch the handler. */
    if (start) {
        new_handler = (bcmcfg_read_handler_t *)((info-1)->user_data);
        if (new_handler) {
            info->handler = new_handler;
            info->user_data = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader doc handler.
 *
 * Call all the inner doc handlers.
 *
 * \param [in]     start        true if init, false if cleanup.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_doc(bool start,
                      bcmcfg_read_context_t *context,
                      void *user_data)
{
    bcmcfg_read_outer_data_t *data = (bcmcfg_read_outer_data_t *)user_data;
    int i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    for (i = INNER_H; i < data->count; i++) {
        if (data->handler[i]->doc) {
            int rv = (data->handler[i]->doc)(start,
                                             context,
                                             data->handler[i]->user_data);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_CONT(rv);
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Outer reader abort handler.
 *
 * Call all the inner abort handlers.
 *
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_abort(bcmcfg_read_context_t *context,
                        void *user_data)
{
    bcmcfg_read_outer_data_t *data = (bcmcfg_read_outer_data_t *)user_data;
    int i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    for (i = INNER_H; i < data->count; i++) {
        if (data->handler[i]->abort) {
            int rv = (data->handler[i]->abort)(context,
                                               data->handler[i]->user_data);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_CONT(rv);
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Registered reader setup and cleanup.
 *
 * Call all the registered reader setup functions.
 *
 * \param [in]    start         true if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_outer_setup_cleanup(bool start)
{
    size_t i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    for ( i = 0; i < COUNTOF(bcmcfg_read_handler); i++) {
        if (!bcmcfg_read_handler[i]->setup) {
            /* setup not needed */
            continue;
        }
        bcmcfg_read_handler[i]->setup(start,
                                      bcmcfg_read_handler[i]->user_data);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*
 * Initialize the reader context by start out in stream context, using
 * this outer reader.
 */
int
bcmcfg_read_outer_init_context(bcmcfg_read_context_t *context)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (context) {
        context->info[context->level].handler = &bcmcfg_read_outer;
    } else {
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    }
    SHR_FUNC_EXIT();
}

/*
 * Call all the registered reader setup functions.
 */
int
bcmcfg_read_outer_init(void)
{
    return bcmcfg_read_outer_setup_cleanup(true);
}

/*
 * Call all the registered reader cleanup functions.
 */
int
bcmcfg_read_outer_cleanup(void)
{
    return bcmcfg_read_outer_setup_cleanup(false);
}
