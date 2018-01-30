/*! \file bcma_bcmltcmd_pt.c
 *
 * Physical table access command in CLI.
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

#include <shr/shr_error.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmlt/bcma_bcmltcmd_pt.h>

#include "bcmltcmd_internal.h"

/* Default setting of pt command parameters */
static bcmltcmd_params_t pt_params = {
    .unit = -1,
    .async = 0,
    .priority = BCMLT_PRIORITY_NORMAL,
    .commit = BCMLTCMD_COMMIT_BY_ENTRY,
    .notify = BCMLT_NOTIF_OPTION_COMMIT,
    .trans_type = BCMLT_TRANS_TYPE_BATCH,
    .disp_word = BCMA_BCMLT_DISP_WORD_SET(BCMA_BCMLT_DISP_MODE_HEX_DEC,
                                          BCMA_BCMLT_DISP_MODE_HEX_DEC)
};

/* Physical table operation codes */
static bcma_cli_parse_enum_t pt_cmds_enum[] = {
    { "set",    BCMLT_PT_OPCODE_SET       },
    { "get",    BCMLT_PT_OPCODE_GET       },
    { "modify", BCMLT_PT_OPCODE_MODIFY    },
    { "lookup", BCMLT_PT_OPCODE_LOOKUP    },
    { NULL,     BCMLT_PT_OPCODE_NOP       }
};

/*
 * Get physical table operation code from CLI input.
 */
static bcmlt_pt_opcode_t
pt_opcode_parse(const char *str)
{
    bcmlt_pt_opcode_t opc;
    bcma_cli_parse_enum_t *e = &pt_cmds_enum[0];

    if (str == NULL) {
        return BCMLT_PT_OPCODE_NOP;
    }

    while (e->name != NULL) {
        if (sal_strcasecmp(e->name, str) == 0) {
            break;
        }
        e++;
    }

    opc = e->val;

    return opc;
}

/*
 * The function is called after a physical table operation has been committed.
 */
static int
pt_table_op_post_commit(bcma_bcmlt_entry_info_t *ei, bcmlt_pt_opcode_t opc)
{
    int rv;
    bcmlt_entry_info_t bcmlt_ei;
    bcmltcmd_params_t *params = &pt_params;

    /* Check entry status after sync or async entry commit. */
    rv = bcmlt_entry_info_get(ei->eh, &bcmlt_ei);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlt_ei.status;
    }
    if (SHR_FAILURE(rv)) {
        cli_out("%spt %s %s: "BCMA_BCMLT_ERRMSG_FMT".\n",
                BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                bcmltcmd_pt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
        return -1;
    }

    /* Disply entry fields if operation is to get table */
    if (opc == BCMLT_PT_OPCODE_GET || opc == BCMLT_PT_OPCODE_LOOKUP) {
        /* Display entry information */
        bcma_bcmlt_entry_list(ei, "    ", 0, params->disp_word);
    }

    return 0;
}

/*
 * The function is called before an operation is committed.
 */
static void
pt_table_op_pre_commit(bcma_bcmlt_entry_info_t *ei, bcmlt_pt_opcode_t opc)
{
    int idx;
    shr_pb_t *pb = shr_pb_create();
    uint32_t disp_word = BCMA_BCMLT_DISP_WORD_SET(BCMA_BCMLT_DISP_MODE_HEX,
                                                  BCMA_BCMLT_DISP_MODE_HEX);

    /* Set entry attribute */
    if (ei->attr != 0) {
        bcmlt_entry_attrib_set(ei->eh, ei->attr);
    }

    /*
     * Display what we're going to commit.
     * eg. physical table name, op code, field value parsed from CLI.
     */
    shr_pb_printf(pb, "  %s", bcmltcmd_pt_opcode_format(opc));
    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        shr_pb_printf(pb, " ");
        bcma_bcmlt_field_format(pb, field, disp_word);
    }
    shr_pb_printf(pb, "\n");

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);
}

/*
 * Callback function for asyncronous commitment.
 */
static void
pt_commit_cb(bcmlt_notif_option_t event, bcmlt_pt_opcode_t opcode,
             bcmlt_entry_info_t  *entry_info_p,
             void *usr_data)
{
    bcma_bcmlt_entry_info_t *ei;

    if (!entry_info_p) {
        return;
    }

    ei = (bcma_bcmlt_entry_info_t *)usr_data;
    if (!ei) {
        return;
    }

    cli_out("\n%s %s: async notify(%d) %d\n", ei->name,
            bcmltcmd_pt_opcode_format(opcode), entry_info_p->notif_opt, event);

    if (event != entry_info_p->notif_opt) {
        /* Notify option is NOTIFY_ALL, only call post_commit function once. */
        if (event == BCMLT_NOTIF_OPTION_COMMIT) {
            pt_table_op_post_commit(ei, opcode);
        }
    } else {
        pt_table_op_post_commit(ei, opcode);
    }

    if (event == BCMLT_NOTIF_OPTION_COMMIT) {
        bcma_bcmlt_entry_info_destroy(ei);
        return;
    }

    if (entry_info_p->notif_opt == BCMLT_NOTIF_OPTION_HW) {
        /*
         * No BCMLT_NOTIF_OPTION_COMMIT event will be sent.
         * Need to cleanup resourece after some time.
         */
        bcmltcmd_schedule_entry_info_cleanup(ei);
    }
}

/*
 * Physical table operation commit funciton.
 */
static int
pt_table_op_commit(bcma_bcmlt_entry_info_t *ei, int opcode)
{
    int rv;
    bcmltcmd_params_t *params = &pt_params;
    bcmlt_pt_opcode_t opc = opcode;

    pt_table_op_pre_commit(ei, opc);

    if (params->async) {
        /* Commit physical table in asynchrnous mode. */
        rv = bcmlt_pt_entry_commit_async(ei->eh, opc, ei, params->notify,
                                         pt_commit_cb, params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%spt %s %s: bcmlt_pt_entry_commit_async failed: "
                    BCMA_BCMLT_ERRMSG_FMT"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_pt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
        }

        /*
         * Need to cleanup resourece after some time if no notify event
         * in asynchronous mode.
         */
        if (params->notify == BCMLT_NOTIF_OPTION_NO_NOTIF) {
            bcmltcmd_schedule_entry_info_cleanup(ei);
        }
    } else {
        /* Commit physical table in synchrnous mode. */
        rv = bcmlt_pt_entry_commit(ei->eh, opc, params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%spt %s %s: bcmlt_pt_entry_commit failed: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_pt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
            return -1;
        }

        /* Perform post-op only if entry is committed successfully. */
        if (pt_table_op_post_commit(ei, opc) != 0) {
            return -1;
        }
    }

    return rv;
}

/*
 * Parse physical table operation in syntax: <op> [<field>=<val> ...]
 */
static int
pt_table_op_parse(bcma_cli_args_t *args, bcma_bcmlt_entry_info_t *ei, int *opc)
{
    char *arg;

    /* Get physical table operation code */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    *opc = pt_opcode_parse(arg);
    if (*opc == BCMLT_PT_OPCODE_NOP) {
        /* Not a valid op code, restore the argument */
        BCMA_CLI_ARG_PREV(args);
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcmltcmd_parse_fields(args, ei) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Do physical table opeations input from CLI for entry based commitment.
 */
static int
do_pt_by_entry(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcmltcmd_params_t *params = &pt_params;
    bcmltcmd_cb_t cb;

    cb.table_op_parse = pt_table_op_parse;
    cb.table_op_commit = pt_table_op_commit;

    return bcmltcmd_commit_by_entry(cli, args, params, &cb, FALSE);
}

/* pt sub-command handler for table operation */
static int
pt_command_table_op(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    /* Curretnly support entry commit. */
    return do_pt_by_entry(cli, args);
}

/* pt sub-command "list" handler */
static int
pt_command_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    if (bcmltcmd_tables_list(cli, args, FALSE) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

char *
bcmltcmd_pt_opcode_format(bcmlt_pt_opcode_t opc)
{
    bcma_cli_parse_enum_t *e = &pt_cmds_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_pt_opcode_t)e->val == opc) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid OP>" : e->name;
}

const bcma_cli_parse_enum_t *
bcmltcmd_pt_opcodes_get(void)
{
    return pt_cmds_enum;
}

int
bcma_bcmltcmd_pt(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *arg;
    bcmltcmd_params_t *params = &pt_params;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Set pt command unit number */
    params->unit = cli->cmd_unit;

    /* Check input arguments by CLI parse table */
    if (bcmltcmd_parse_parameters(cli, args, params) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (params->unit < 0) {
        cli_out("%s: Error: Invalid unit %d\n",
                BCMA_CLI_ARG_CMD(args), params->unit);
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_CUR(args);
    if (arg && sal_strcmp(arg, "list") == 0) {
        /* pt command for tables list */
        BCMA_CLI_ARG_NEXT(args);
        return pt_command_list(cli, args);
    }

    if (BCMA_CLI_ARG_CNT(args) > 0) {
        /* pt command for tables operation */
        rv = pt_command_table_op(cli, args);
    }

    return rv;
}
