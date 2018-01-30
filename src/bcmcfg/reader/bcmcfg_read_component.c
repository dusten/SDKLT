/*! \file bcmcfg_read_component.c
 *
 * Component configuration handler. This uses the software component
 * configuration schema to interpret the YAML event stream and
 * populate the software configuration data structures.
 *
 * The software configuration schema is designed so that the C
 * software configuration structures are addressable as an array of
 * uint32_t.
 *
 * Similar to other readers, this reader maintains a stack of data
 * corresponding to each reader level. To keep thinks simple, the
 * maximum depth of this stack is the same and the other readers -
 * this means that the lowest elements are never used.
 *
 * Software components define a configuration schema appropriate to
 * their component. The configuration schema is compiled into a
 * "component scanner", which is a set of records that describes
 * the software component schema programatically. The scanner
 * guides the interpretation of the YAML events and storage of the
 * configuration data.
 *
 * There are two readers involved: the "component" reader that handles the
 * outer "component:" keys, and the "comp node" reader, that handles the
 * software component schema.
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
#include <bcmcfg/comp/bcmcfg_comp_scanner_conf.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_COMP

/*!
 * \brief Component node data.
 *
 * Component node information that is tracked per-level.
 */
typedef struct bcmcfg_comp_node_u_s {
    /*! Magic number to help detect data structure confusion. */
    uint32_t magic;

    /*! Component Scanner*/
    const bcmcfg_comp_scanner_t *scanner;

    /*! Start index in the scanner when handling a map. */
    uint32_t start_idx;

    /*! Current node index in the scanner. */
    uint32_t node_idx;

    /*! Storage offset when handling sequences. */
    uint32_t offset;
} bcmcfg_comp_node_u_t;

/* Component node stack, indexed by reader level. */
static bcmcfg_comp_node_u_t bcmcfg_comp_node[BCMCFG_MAX_LEVEL];


/*******************************************************************************
 * Private functions
 */

/*
 * Inner node reader
 */

/*!
 * \brief Write a scalar value.
 *
 * Scalar value is written to the appropriate offset in the software
 * component configuration data structure.
 *
 * \param [in]    u             Component node data.
 * \param [in]    seq_idx       Sequence index.
 * \param [in]    value         Scalar value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_compnode_scalar_value(const bcmcfg_comp_node_u_t *u,
                                  uint32_t seq_idx,
                                  const char *value)
{
    const bcmcfg_comp_scanner_t *scanner = u->scanner;
    const bcmcfg_comp_node_t *node = scanner->schema +  u->node_idx;
    char *end;
    uint32_t u_value;
    uint32_t *data = *scanner->data;
    uint32_t w_offset = (node->offset + u->offset) / sizeof(uint32_t);

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    w_offset += seq_idx;
    u_value = sal_strtoul(value, &end, 0);
    if (*value && !*end) {
        /* Valid conversion */
        data[w_offset] = u_value;
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unable to convert %s\n"), value));
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Search for a map key.
 *
 * Search for a map key in a component mapping node starting at the
 * node start_idx. If the key is found, set node_idx to the scanner
 * index of the found key. If the key is not found, return
 * SHR_E_NOT_FOUND.
 *
 * \param [in,out] u            Component node data.
 * \param [in]     key          Map key.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_compnode_search(bcmcfg_comp_node_u_t *u,
                            const char *key)
{
    const bcmcfg_comp_node_t *schema = u->scanner->schema;
    uint32_t idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    for (idx = u->start_idx; idx != BCMCFG_NO_IDX; idx = schema[idx].next) {
        if (!sal_strcmp(key, schema[idx].key)) {
            u->node_idx = idx;
            break;
        }
    }
    if (idx == BCMCFG_NO_IDX) {
        SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Scalar in sequence context.
 *
 * Set a scalar value when in a sequence.
 *
 * \param [in]     u            Component node data.
 * \param [in]     context      Reader context.
 * \param [in]     value        Scalar value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_compnode_scalar_seq(const bcmcfg_comp_node_u_t *u,
                                const bcmcfg_read_context_t *context,
                                const char *value)
{
    int seq_container = context->info[context->level].seq_container;
    /* If there is no seq_container, seq_idx == 0 */
    int seq_idx = context->info[seq_container].seq_idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT(bcmcfg_read_compnode_scalar_value(u,
                                                      seq_idx,
                                                      value));
    SHR_FUNC_EXIT();
}

/*!
 * \brief Software component scalar handler.
 *
 * Scalar events for software component configuration occur for map
 * keys, map values, and sequences. Any other context is an error.
 *
 * Map keys:    Search for the key.
 * Map values:  Set the value.
 * Sequences:   Set the value.
 *
 * \param [in]     value        Scalar value.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_compnode_scalar(const char *value,
                            bcmcfg_read_context_t *context,
                            void *user_data)
{
    const bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_comp_node_u_t *u = (bcmcfg_comp_node_u_t *)info->user_data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (info->state) {
    case BCMCFG_READ_STATE_MAP:
        SHR_IF_ERR_CONT(bcmcfg_read_compnode_search(u, value));
        break;
    case BCMCFG_READ_STATE_SEQ:
        SHR_IF_ERR_CONT(bcmcfg_read_compnode_scalar_seq(u,
                                                        context,
                                                        value));
        break;
    case BCMCFG_READ_STATE_MAP_V:
        SHR_IF_ERR_CONT(bcmcfg_read_compnode_scalar_value(u, 0, value));
        break;
    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        break;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy component node data from previous level.
 *
 * If 'start' is true, copy previous level node data to the current
 * level.
 *
 * \param [in]     start        True if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_compnode_copy(bool start,
                          bcmcfg_read_context_t *context,
                          void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_comp_node_u_t *u;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(user_data);

    if (start) {
        /* copy previous user data */
        u = bcmcfg_comp_node + context->level;
        sal_memcpy(u, (info-1)->user_data, sizeof(bcmcfg_comp_node_u_t));
        info->user_data = u;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Handle YAML map event.
 *
 * When starting a map, set the sequence container and start_idx, and
 * calculate storage offset if within a sequence.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_compnode_map(bool start,
                          bcmcfg_read_context_t *context,
                          void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(user_data);

    SHR_IF_ERR_CONT(bcmcfg_read_compnode_copy(start, context, user_data));
    if (!SHR_FUNC_ERR() && start) {
        bcmcfg_comp_node_u_t *u = (bcmcfg_comp_node_u_t *)info->user_data;
        int seq_container = context->info[context->level].seq_container;

        /* keys for the matched map start in the next slot */
        u->start_idx = u->node_idx+1;

        /* if within a sequence, handle sequence indexing */
        if (seq_container > 0) {
            bcmcfg_read_level_info_t *seq =  context->info + seq_container;
            const bcmcfg_comp_node_t *schema =
                u->scanner->schema + u->node_idx;

            /* Update offset based on sequence index.*/

            if (seq->seq_idx < schema->array) {
                u->offset += seq->seq_idx * schema->size;
            } else {
                /* Either this schema node isn't an array, or the requested
                   array index is beyond the schema limit. */
                SHR_IF_ERR_CONT(SHR_E_FAIL);
            }
        }
    }
    SHR_FUNC_EXIT();
}

/* Software configuration reader handler. */
static bcmcfg_read_handler_t bcmcfg_read_compnode = {
    .key = "compnode",
    .scalar = bcmcfg_read_compnode_scalar,
    .map = bcmcfg_read_compnode_map,
    .seq = bcmcfg_read_compnode_copy
};


/*!
 * \brief Setup component reader.
 *
 * Component reader setup/cleanup. Called once with start=true, and
 * once with start=false.
 *
 * \param [in]    start         true if start, false if stop.
 * \param [in]    user_data     Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_component_setup(bool start,
                            void *user_data)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (start) {
        bcmcfg_read_component_alloc(bcmcfg_component_conf);
    } else {
        bcmcfg_read_component_free(bcmcfg_component_conf);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Transition to software component reader.
 *
 * Transition to the software component read handler based on a
 * successful match of a component key. Set up the sw comp user data
 * and pass the setup data via a user_data pointer.
 *
 * \param [in]     context      Reader context.
 * \param [in,out] info         Read level information.
 * \param [in]     scanner      Component scanner.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_component_transition(const bcmcfg_read_context_t *context,
                                 bcmcfg_read_level_info_t *info,
                                 const bcmcfg_comp_scanner_t *scanner)
{
    bcmcfg_comp_node_u_t *u;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    u = bcmcfg_comp_node + context->level;

    /* clear out */
    sal_memset(u, 0, sizeof(*u));
    /* set fields */
    u->magic = 0x37735995;
    /* transition to comp node read handler */
    u->scanner = scanner;
    /* initial node to search */
    u->start_idx = u->node_idx = 1;
    /* offset */
    u->offset = 0;
    /* pass comp_node_u data to following map.start */
    info->user_data = u;
    SHR_FUNC_EXIT();
}

/*!
 * \brief Component reader scalar handler.
 *
 * Only map keys need to be dealt with, so search for an software
 * component scanner with the given key, starting with scanner 0.
 *
 * \param [in]     value        Scalar value.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_component_scalar(const char *value,
                             bcmcfg_read_context_t *context,
                             void *user_data)
{
    const bcmcfg_comp_scanner_conf_t *conf =
        *(const bcmcfg_comp_scanner_conf_t **)user_data;

    bcmcfg_read_level_info_t *info = context->info + context->level;
    uint32_t i;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    for (i = 0; i < conf->count; i++) {
        /* eventually bsearch */
        if (!sal_strcmp(value, conf->scanner[i]->schema[0].key)) {
            break;
        }
    }

    if (i < conf->count) {
        SHR_IF_ERR_CONT(bcmcfg_read_component_transition(context,
                                                         info,
                                                         conf->scanner[i]));
    } else {
        SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Component reader map handler.
 *
 * Start a component configuration mapping by picking up the new
 * reader handler data from the previous level user_data set by
 * bcmcfg_read_component_transition().
 *
 * \param [in]     start        true if init, false if cleanup.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_component_map(bool start,
                          bcmcfg_read_context_t *context,
                          void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (start) {
        /* transition to the comp_read handler; user data is in the
         previous level user_data. */
        info->handler = &bcmcfg_read_compnode;
        info->user_data = (info-1)->user_data;
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/* Pointer to all the component scanners. This may be overridden for
   test purposes. */
const bcmcfg_comp_scanner_conf_t *bcmcfg_component_conf =
    &bcmcfg_comp_scanner_conf;


/*
 * For internal and test purposes ONLY. This function is not static,
 * and should only be called externally by unit tests.
 */
int
bcmcfg_read_component_alloc(const bcmcfg_comp_scanner_conf_t *conf)
{
    size_t i;
    size_t needed = 0;
    uint32_t *buf;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* See how much is needed */
    for (i = 0; i < conf->count; i++) {
        /* data_size should always be a multiple of uint32_t. If it
         isn't then something is seriously broken in the cfg code
         generator. */
        if ((conf->scanner[i]->data_size % sizeof(*buf)) != 0) {
            needed = 0;
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }
        needed += conf->scanner[i]->data_size;
        if (*conf->scanner[i]->data != NULL) {
            /* If the data pointer is not NULL, then the state of this structure
             is undefined - return an error. */
            needed = 0;
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }
    }

    if (needed) {
        /* Allocate buffer */
        buf = sal_alloc(needed, "bcmcfg_read_component_alloc");
        if (buf) {
            sal_memset(buf, 0, needed);
            for (i = 0; i < conf->count; i++) {
                *conf->scanner[i]->data = buf;
                buf += conf->scanner[i]->data_size / sizeof(*buf);
            }
        } else {
            SHR_IF_ERR_CONT(SHR_E_MEMORY);
        }
    }
    SHR_FUNC_EXIT();
}

/*
 * For internal and test purposes ONLY. This function is not static,
 * and should only be called externally by unit tests.
 */
int
bcmcfg_read_component_free(const bcmcfg_comp_scanner_conf_t *conf)
{
    size_t i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (conf && *conf->scanner[0]->data) {
        sal_free(*conf->scanner[0]->data);
        /* Get rid of dangling pointers. */
        for (i = 0; i < conf->count; i++) {
            *conf->scanner[i]->data = NULL;
        }
    } else {
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
    }
    SHR_FUNC_EXIT();
}

/* Read component handler */
const bcmcfg_read_handler_t bcmcfg_read_component = {
    .key       = "component",
    .setup     = bcmcfg_read_component_setup,
    .scalar    = bcmcfg_read_component_scalar,
    .map       = bcmcfg_read_component_map,
    .user_data = (void *)&bcmcfg_component_conf
};
