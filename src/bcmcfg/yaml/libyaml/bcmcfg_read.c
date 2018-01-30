/*! \file bcmcfg_read.c
 *
 * YAML file reader
 *
 * The BCMCFG YAML reader is divided into two parts. The main reader
 * based on LibYAML is here, and calls out to specialized readers
 * based on a map key.
 *
 * See http://pyyaml.org/wiki/LibYAML to understand the LibYAML
 * interface. The design of this reader is based on the valid YAML
 * event grammar described at the link above and reproduced here:
 *
 *    stream ::= STREAM-START document* STREAM-END
 *    document ::= DOCUMENT-START node DOCUMENT-END
 *    node ::= ALIAS | SCALAR | sequence | mapping
 *    sequence ::= SEQUENCE-START node* SEQUENCE-END
 *    mapping ::= MAPPING-START (node node)* MAPPING-END
 *
 * This interface uses LibYAML events to drive a reader state machine,
 * and the reader state machine uses a "context stack" to keep track
 * of the reader state. LibYAML events are read in a loop until an
 * event indicates that event processing is complete or some error was
 * detected. All common event handling, such as managing context,
 * separating map keys and values, and sequence counting are handled
 * in this reader. Almost all events push and pop elements on the
 * context stack, and events are dispatched via the context stack
 * to the read handlers.
 *
 * The config file itself consists of a single stream and zero or more
 * documents in that stream. Each document contains a map, where the
 * keys of the map indicate which specialized reader to call for
 * subsequent events. The specific interpretation of the YAML values
 * is handled by the specialized readers themselves.
 *
 * Each specialized reader is associated with a reader handler. The
 * reader handler indicates which outer key it handles, has a set of
 * driver functions for each YAML event, and a pointer to private user
 * data.
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
#include <bcmcfg/bcmcfg.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>

/* External OSS dependency */
#include <yaml.h>

/* YAML library dependencies for File IO */
#include <stdio.h>
#include <errno.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_READ

#if !defined(BCMCFG_USE_YAML_FILE)
#define BCMCFG_USE_YAML_FILE 1
#endif

/* Keep track of these events for diagnostic purposes. */
static int bcmcfg_stream_count;
static int bcmcfg_no_event_count;

/* Reader context stack. */
static bcmcfg_read_context_t bcmcfg_context;

static bool bcmcfg_initialized;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Push reader state.
 *
 * Push the reader state on the context stack. By default, the
 * handler and sequence container is the same as the previous
 * element on the stack. Will return an error of the context
 * stack overflows.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     state        Context state.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_push_state(bcmcfg_read_context_t *context,
                  bcmcfg_read_state_t state)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (context->level < (BCMCFG_MAX_LEVEL - 2)) {
        context->level++;
        info = context->info + context->level;
        info->state = state;
        info->handler = (info-1)->handler;
        info->seq_container = (info-1)->seq_container;
        info->locus = (info-1)->locus;
        info->line = (info-1)->line;
        info->column = (info-1)->column;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META("Context level overflow\n")));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pop reader state.
 *
 * Reset items on the top element of reader context stack, and then
 * pop it off the stack. Will return an error if the context stack
 * underflows.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_pop_state(bcmcfg_read_context_t *context)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (context->level > 0) {
        info->state = BCMCFG_READ_STATE_NONE;
        info->handler = NULL;
        info->user_data = NULL;
        context->level--;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META("Context level underflow\n")));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML alias events.
 *
 * Not implemented yet, so returns SHR_E_INTERNAL.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     event        LibYAML event.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_alias(bcmcfg_read_context_t *context,
                    const yaml_event_t *event)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT(SHR_E_INTERNAL);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML scalar events.
 *
 * Dispatch to the scalar event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     event        LibYAML event.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_scalar(const bcmcfg_read_handler_t *handler,
                     bcmcfg_read_context_t *context,
                     const yaml_event_t *event)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (handler->scalar) {
        SHR_IF_ERR_CONT(handler->scalar((char *)event->data.scalar.value,
                                        context,
                                        handler->user_data));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML sequence events.
 *
 * Initialize the sequence container, sequence index, and then
 * dispatch to the sequence event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_sequence(const bcmcfg_read_handler_t *handler,
                       bcmcfg_read_context_t *context,
                       bool start)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* indicate current sequence container */
    context->info[context->level].seq_container = context->level;
    /* start seq_idx at -1 - immediately following level will increment
       seq_idx to 0, next to 1 etc. */
    context->info[context->level].seq_idx = ((uint32_t)-1);

    if (handler->seq) {
        SHR_IF_ERR_CONT(handler->seq(start,
                                     context,
                                     handler->user_data));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML map events.
 *
 * Dispatch to the map event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_map(const bcmcfg_read_handler_t *handler,
                  bcmcfg_read_context_t *context,
                  bool start)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (handler->map) {
        SHR_IF_ERR_CONT(handler->map(start,
                                     context,
                                     handler->user_data));
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML stream events.
 *
 * Track the stream count, and set done if the stream has ended.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 * \param [out]    done         Done flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_stream(bcmcfg_read_context_t *context,
                     bool start,
                     bool *done)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (start) {
        if (bcmcfg_stream_count == 0) {
            bcmcfg_stream_count++;
        } else {
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        }
    } else {
        if (bcmcfg_stream_count == 1) {
            *done = true;
        } else {
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML doc events.
 *
 * Dispatch to the doc event handler if there is one.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     start        True if starting, false if ending.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_doc(const bcmcfg_read_handler_t *handler,
                  bcmcfg_read_context_t *context,
                  bool start)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (handler->doc) {
        SHR_IF_ERR_CONT(handler->doc(start,
                                     context,
                                     handler->user_data));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML no-event events.
 *
 * Increment the no-event counter.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_no_event(bcmcfg_read_context_t *context)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    bcmcfg_no_event_count++;
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML map node state transitions.
 *
 * If the current state is MAP, then transition to MAP_V
 * If the current state is MAP_V, then transition to MAP
 * Otherwise make no change to the state.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_map_state(bcmcfg_read_context_t *context)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (info->state == BCMCFG_READ_STATE_MAP) {
        info->state = BCMCFG_READ_STATE_MAP_V;
    } else if (info->state == BCMCFG_READ_STATE_MAP_V) {
        info->state = BCMCFG_READ_STATE_MAP;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Increment sequence index.
 *
 * Increment the sequence index if we are at the same level as the
 * sequence container.
 *
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_incr_seq_idx(bcmcfg_read_context_t *context)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (info->seq_container == context->level) {
        context->info[info->seq_container].seq_idx++;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief LibYAML event type to string.
 *
 * Convert a LibYAML event type enum to a string.
 *
 * \param [in]     event_type   Event type.
 *
 * \retval string
 */
static char *
y_event_str(yaml_event_type_t event_type)
{
    char *s = "UKNOWN";

    switch (event_type) {
    case YAML_NO_EVENT:
        s = "NO_EVENT";
        break;

    case YAML_STREAM_START_EVENT:
        s = "STREAM_START";
        break;

    case YAML_STREAM_END_EVENT:
        s = "STREAM_END";
        break;

    case YAML_DOCUMENT_START_EVENT:
        s = "DOCUMENT_START";
        break;

    case YAML_DOCUMENT_END_EVENT:
        s = "DOCUMENT_END";
        break;

    case YAML_ALIAS_EVENT:
        s = "ALIAS";
        break;

    case YAML_SCALAR_EVENT:
        s = "SCALAR";
        break;

    case YAML_SEQUENCE_START_EVENT:
        s = "SEQUENCE_START";
        break;

    case YAML_SEQUENCE_END_EVENT:
        s = "SEQUENCE_END";
        break;

    case YAML_MAPPING_START_EVENT:
        s = "MAPPING_START";
        break;

    case YAML_MAPPING_END_EVENT:
        s = "MAPPING_END";
        break;
    }

    return s;
}

/*!
 * \brief Handle LibYAML events.
 *
 * Manage event context and dispatch to event handlers.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 * \param [in]     event        LibYAML event.
 * \param [out]    done         Done flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_handle_event(const bcmcfg_read_handler_t *handler,
                    bcmcfg_read_context_t *context,
                    const yaml_event_t *event,
                    bool *done)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (event->type == YAML_SCALAR_EVENT) {
        LOG_DEBUG
            (BSL_LOG_MODULE,
             (BSL_META("%02d %s '%s'\n"),
              context->level,
              y_event_str(event->type),
              event->data.scalar.value));
    } else {
        LOG_DEBUG
            (BSL_LOG_MODULE,
             (BSL_META("%02d %s\n"),
              context->level, y_event_str(event->type)));
    }
    switch (event->type) {
    case YAML_NO_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_no_event(context));
        break;

    case YAML_STREAM_START_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_handle_stream(context, true, done));
        break;

    case YAML_STREAM_END_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_handle_stream(context, false, done));
        break;

    case YAML_DOCUMENT_START_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_push_state(context, BCMCFG_READ_STATE_DOC));
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_handle_doc(handler, context, true));
        }
        break;

    case YAML_DOCUMENT_END_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_handle_doc(handler, context, false));
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_pop_state(context));
        }
        break;

    case YAML_ALIAS_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_handle_alias(context, event));
        break;

    case YAML_SCALAR_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_incr_seq_idx(context));
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_handle_scalar(handler, context, event));
        }
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_handle_map_state(context));
        }
        break;

    case YAML_SEQUENCE_START_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_incr_seq_idx(context));
        SHR_IF_ERR_CONT(bcmcfg_push_state(context, BCMCFG_READ_STATE_SEQ));
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_handle_sequence(handler, context, true));
        }
        break;

    case YAML_SEQUENCE_END_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_handle_sequence(handler, context, false));
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_pop_state(context));
        }
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_handle_map_state(context));
        }
        break;

    case YAML_MAPPING_START_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_incr_seq_idx(context));
        SHR_IF_ERR_CONT(bcmcfg_push_state(context, BCMCFG_READ_STATE_MAP));
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_handle_map(handler, context, true));
        }
        break;

    case YAML_MAPPING_END_EVENT:
        SHR_IF_ERR_CONT(bcmcfg_handle_map(handler, context, false));
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_pop_state(context));
        }
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_handle_map_state(context));
        }
        break;

    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        break;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief LibYAML parse loop.
 *
 * Parse and handle LibYAML events until done or an error occurs.
 * If there is an error from a handler, display an error message.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     locus        Error locus.
 * \param [in,out] parser       LibYAML parser.
 * \param [out]    error        Error flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */

/*!
 * \brief Dispatch abort notification.
 *
 * Dispatch abort notification to event handlers.
 *
 * \param [in]     handler      Event handler.
 * \param [in,out] context      Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_abort(const bcmcfg_read_handler_t *handler,
             bcmcfg_read_context_t *context)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (handler->abort) {
        SHR_IF_ERR_CONT(handler->abort(context,
                                       handler->user_data));
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief LibYAML parse loop.
 *
 * Parse and handle LibYAML events until done or an error occurs.
 * If there is an error from a handler, display an error message.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     locus        Error locus.
 * \param [in,out] parser       LibYAML parser.
 * \param [out]    error        Error flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse_loop(bcmcfg_read_context_t *context,
                  const char *locus,
                  yaml_parser_t *parser,
                  bool *error)
{
    yaml_event_t event;
    int yrv = 0;
    int nrv = 0;
    bool done = false;
    bcmcfg_read_level_info_t *info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        yrv = yaml_parser_parse(parser, &event);
        if (!yrv) {
            *error=true;
            break;
        }

        /* Get the current handler. */
        info = context->info + context->level;
        info->locus = locus;
        info->line = (int)event.start_mark.line+1;
        info->column = (int)event.start_mark.column+1;
        if (info->handler) {
            nrv = bcmcfg_handle_event(info->handler,
                                      context,
                                      &event,
                                      &done);

            if (SHR_FAILURE(nrv)) {
                LOG_ERROR
                    (BSL_LOG_MODULE,
                     (BSL_META("%s:%d:%d: syntax error.\n"),
                      locus,
                      info->line,
                      info->column));
                if (info->handler) {
                    (void)bcmcfg_abort(info->handler, context);
                }
            }
            SHR_IF_ERR_CONT(nrv);
        } else {
            /* handler is NULL? */
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        }

        yaml_event_delete(&event);

    } while (!done && !SHR_FUNC_ERR());

    SHR_FUNC_EXIT();
}

/*!
 * \brief LibYAML parser.
 *
 * Initialize and run LibYAML parser.
 *
 * \param [in,out] context      Reader context.
 * \param [in]     locus        Error locus.
 * \param [in,out] parser       LibYAML parser.
 * \param [out]    error        Error flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse(bcmcfg_read_context_t *context,
             const char *locus,
             yaml_parser_t *parser,
             bool *error)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {

        /* Initialize if needed. */
        SHR_IF_ERR_CONT(bcmcfg_read_init());
        if (SHR_FUNC_ERR()) {
            break;
        }

        /* Initialize stream count. */
        bcmcfg_stream_count = 0;
        /* Initialize context. */
        context->level = 0;
        context->info[context->level].state = BCMCFG_READ_STATE_STREAM;

        /* seq_container will always be > 0 because, by definition, the
           first two levels are stream and doc, so 0 is suitable to
           indicate there is no sequence container. */
        context->info[context->level].seq_container = 0;
        context->info[context->level].seq_idx = 0;

        /* Initialize outer reader context. */
        SHR_IF_ERR_CONT(bcmcfg_read_outer_init_context(context));
        if (SHR_FUNC_ERR()) {
            break;
        }

        /* Run the parse loop. */
        SHR_IF_ERR_CONT(bcmcfg_parse_loop(context, locus, parser, error));

    } while (0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Issue parser error.
 *
 * Error message shows error locus (filename or "<string>"), error
 * location, and any additional context information if available.
 *
 * \param [in]    locus         Error locus such as file name.
 * \param [in]    parser        LibYAML parser.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse_error(const char *locus, const yaml_parser_t *parser)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (parser->context) {
        LOG_ERROR
            (BSL_LOG_MODULE,
             (BSL_META("%s:%d:%d: %s %s\n"),
              locus,
              (int)parser->problem_mark.line+1,
              (int)parser->problem_mark.column+1,
              parser->problem,
              parser->context));
    } else {
        LOG_ERROR
            (BSL_LOG_MODULE,
             (BSL_META("%s:%d:%d: %s\n"),
              locus,
              (int)parser->problem_mark.line+1,
              (int)parser->problem_mark.column+1,
              parser->problem));
    }
    SHR_IF_ERR_CONT(SHR_E_FAIL);

    SHR_FUNC_EXIT();
}

#if BCMCFG_USE_YAML_FILE
/*!
 * \brief Parse a BCMCFG format YAML file.
 *
 * Initialize parser, file open/close, and so on. Note that this
 * deliberately does not use SAL interfaces, as this is a primary
 * LibYAML interface, and LibYAML doesn't use SAL either. This libary
 * may be configured not to use a file interface, in which case, this
 * function is compiled out.
 *
 * \param [in]  file            File to parse.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_file_parse_handle(const char *file)
{
    yaml_parser_t parser;
    bool error = false;
    FILE *f;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (yaml_parser_initialize(&parser)) {
        if ((f = fopen(file, "r")) != NULL) {
            yaml_parser_set_input_file(&parser, f);
            SHR_IF_ERR_CONT(bcmcfg_parse(&bcmcfg_context,
                                         file, &parser, &error));
            if (error) {
                SHR_IF_ERR_CONT(bcmcfg_parse_error(file, &parser));
            }
            fclose(f);
        } else if (errno == ENOENT) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_IF_ERR_CONT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
    }

 exit:
    yaml_parser_delete(&parser);
    SHR_FUNC_EXIT();
}
#endif

/*******************************************************************************
 * Public functions
 */

/*
 * Parse YAML file.
 */
int
bcmcfg_file_parse(const char *file)
{
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (file != NULL) {
#if BCMCFG_USE_YAML_FILE
        rv = bcmcfg_file_parse_handle(file);
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            SHR_IF_ERR_CONT(rv);
        }
#else
        SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
#endif
    } else {
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*
 * Parse YAML string.
 */
int
bcmcfg_string_parse(const char *str)
{
    yaml_parser_t parser;
    bool error = false;
    const char *locus = "<string>";
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (str != NULL) {
        if (yaml_parser_initialize(&parser)) {
            yaml_parser_set_input_string(&parser,
                                         (const unsigned char *)str,
                                         strlen(str));

            SHR_IF_ERR_CONT(bcmcfg_parse(&bcmcfg_context,
                                         locus, &parser, &error));

        }

        if (error) {
            SHR_IF_ERR_CONT(bcmcfg_parse_error(locus, &parser));
        }
        yaml_parser_delete(&parser);
    } else {
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    }

    SHR_FUNC_EXIT();
}

/*
 * Initialize the outer reader, which then initializes all other readers.
 */
int
bcmcfg_read_init(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!bcmcfg_initialized) {
        SHR_IF_ERR_CONT(bcmcfg_read_outer_init());
        bcmcfg_initialized = true;
    }
    SHR_FUNC_EXIT();
}

/*
 * Clean up the outer reader, which then cleans up all other readers.
 */
int
bcmcfg_read_cleanup(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (bcmcfg_initialized) {
        SHR_IF_ERR_CONT(bcmcfg_read_outer_cleanup());
        bcmcfg_initialized = false;
    }
    SHR_FUNC_EXIT();
}
