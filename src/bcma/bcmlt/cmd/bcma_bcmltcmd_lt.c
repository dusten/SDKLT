/*! \file bcma_bcmltcmd_lt.c
 *
 * Logical table access command in CLI.
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

#include <bcma/bcmlt/bcma_bcmltcmd_lt.h>

#include "bcmltcmd_internal.h"

/*
 * Cookie for the callback function traversed_entry_commit()
 * from bcma_bcmlt_table_traverse().
 */
typedef struct trvs_entry_commit_info_s {
    bcma_bcmlt_entry_info_t *ei;
    int opc;
    int rv;
} trvs_entry_commit_info_t;

static int
lt_table_op_commit(bcma_bcmlt_entry_info_t *ei, int opcode);

/* Default setting of pt command parameters */
static bcmltcmd_params_t lt_params = {
    .unit = -1,
    .async = 0,
    .priority = BCMLT_PRIORITY_NORMAL,
    .commit = BCMLTCMD_COMMIT_BY_ENTRY,
    .notify = BCMLT_NOTIF_OPTION_COMMIT,
    .trans_type = BCMLT_TRANS_TYPE_ATOMIC,
    .disp_word = BCMA_BCMLT_DISP_WORD_SET(BCMA_BCMLT_DISP_MODE_HEX_DEC,
                                          BCMA_BCMLT_DISP_MODE_HEX_DEC)
};

/* Logical table operation codes */
static bcma_cli_parse_enum_t lt_cmds_enum[] = {
    { "delete",   BCMLT_OPCODE_DELETE   },
    { "insert",   BCMLT_OPCODE_INSERT   },
    { "update",   BCMLT_OPCODE_UPDATE   },
    { "lookup",   BCMLT_OPCODE_LOOKUP   },
    { "traverse", BCMLT_OPCODE_TRAVERSE },
    { NULL,       BCMLT_OPCODE_NOP      }
};

/*
 * Get logical table operation code from CLI input.
 */
static bcmlt_opcode_t
lt_opcode_parse(const char *str)
{
    bcmlt_opcode_t opc;
    bcma_cli_parse_enum_t *e = &lt_cmds_enum[0];

    if (str == NULL) {
        return BCMLT_OPCODE_NOP;
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
 * This function is used to check whether the specified option
 * is valid for the given opcode. The valid option will be
 * stored in the entry information for further processing.
 */
static bool
lt_opcode_option_check(bcma_bcmlt_entry_info_t *ei,
                       bcmlt_opcode_t opc, char opt)
{
    if (opt == 'i') {
        /*
         * OPT: INSERT after entry not found UPDATE.
         * Support in update opcode.
         */
        if (opc == BCMLT_OPCODE_UPDATE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_INS_UPDT;
            return true;
        }
    } else if (opt == 'l') {
        /*
         * OPT: Perform lookup operation on each traversed entry.
         * Support in traverse opcode.
         */
        if (opc == BCMLT_OPCODE_TRAVERSE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_TRVS_LKUP;
            return true;
        }
    } else if (opt == 'u') {
        /*
         * OPT: Perform update operation on each traversed entry.
         * Support in traverse opcode.
         */
        if (opc == BCMLT_OPCODE_TRAVERSE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_TRVS_UPD;
            return true;
        }
    } else if (opt == 'd') {
        /*
         * OPT: Perform delete operation on each traversed entry.
         * Support in traverse opcode.
         */
        if (opc == BCMLT_OPCODE_TRAVERSE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_TRVS_DEL;
            return true;
        }
    } else if (opt == 'a') {
        /*
         * OPT: Auto-fill unspecified key fields by default values.
         * Support in insert/lookup/update/delete opcodes.
         */
        if (opc == BCMLT_OPCODE_INSERT || opc == BCMLT_OPCODE_LOOKUP ||
            opc == BCMLT_OPCODE_UPDATE || opc == BCMLT_OPCODE_DELETE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_AUTO_KFS;
            return true;
        }
    }

    return false;
}

/*
 * The function is called after a logical table operation has been committed.
 */
static int
lt_table_op_post_commit(bcma_bcmlt_entry_info_t *ei, bcmlt_opcode_t opc)
{
    int rv;
    bcmlt_entry_info_t bcmlt_ei;
    bcmltcmd_params_t *params = &lt_params;

    /* Check entry status after sync or async entry commit. */
    rv = bcmlt_entry_info_get(ei->eh, &bcmlt_ei);
    if (SHR_SUCCESS(rv)) {
        rv = bcmlt_ei.status;
        /*
         * If an UPDATE operation fails with NOT_FOUND, then we retry
         * the operation as an INSERT instead of forcing the user to
         * do this manually.
         */
        if (opc == BCMLT_OPCODE_UPDATE && rv == SHR_E_NOT_FOUND &&
            ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_INS_UPDT) {
            cli_out("    "BCMA_BCMLT_ERRMSG_FMT"\n", BCMA_BCMLT_ERRMSG(rv));
            return lt_table_op_commit(ei, BCMLT_OPCODE_INSERT);
        }
    }
    if (SHR_FAILURE(rv)) {
        cli_out("%slt %s %s: "BCMA_BCMLT_ERRMSG_FMT".\n",
                BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                bcmltcmd_lt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
        return -1;
    }

    if (opc == BCMLT_OPCODE_LOOKUP) {
        /* Display entry information */
        bcma_bcmlt_entry_list(ei, "    ", 0, params->disp_word);
    } else if (opc == BCMLT_OPCODE_INSERT) {
        /* Check for index with allocate on insert operation */
        if (ei->num_commit_fields >= 0) {
            uint32_t fcnt = 0;

            rv = bcmlt_entry_field_count(ei->eh, &fcnt);
            if (SHR_SUCCESS(rv)) {
                if ((int)fcnt > ei->num_commit_fields) {
                    /* Presumably to be index with allocation */
                    bcma_bcmlt_entry_list(ei, "    ", 1, params->disp_word);
                }
            }
        }
    }

    return 0;
}

/*
 * The function is called before an operation is committed.
 */
static void
lt_table_op_pre_commit(bcma_bcmlt_entry_info_t *ei, bcmlt_opcode_t opc)
{
    int idx;
    uint32_t fcnt = 0;
    shr_pb_t *pb = shr_pb_create();
    uint32_t disp_word = BCMA_BCMLT_DISP_WORD_SET(BCMA_BCMLT_DISP_MODE_HEX,
                                                  BCMA_BCMLT_DISP_MODE_HEX);

    /* Set entry attribute */
    if (ei->attr != 0) {
        bcmlt_entry_attrib_set(ei->eh, ei->attr);
    }

    /*
     * Display what we're going to commit.
     * eg. logical table name, op code, field value parsed from CLI.
     */
    shr_pb_printf(pb, "  %s", bcmltcmd_lt_opcode_format(opc));
    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        shr_pb_printf(pb, " ");
        bcma_bcmlt_field_format(pb, field, disp_word);
    }
    shr_pb_printf(pb, "\n");

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    /* Get number of fields to be committed. */
    ei->num_commit_fields = -1;
    if (SHR_SUCCESS(bcmlt_entry_field_count(ei->eh, &fcnt))) {
        ei->num_commit_fields = fcnt;
    }
}

/*
 * Callback function for asyncronous commitment.
 */
static void
lt_commit_cb(bcmlt_notif_option_t event, bcmlt_opcode_t opcode,
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
            bcmltcmd_lt_opcode_format(opcode), entry_info_p->notif_opt, event);

    if (entry_info_p->notif_opt == BCMLT_NOTIF_OPTION_ALL) {
        /*
         * If notify option is NOTIFY_ALL, multiple notifications will be sent.
         * Only call post_commit function once.
         */
        if (event == BCMLT_NOTIF_OPTION_COMMIT) {
            lt_table_op_post_commit(ei, opcode);
        }
    } else {
        lt_table_op_post_commit(ei, opcode);
    }

    /* Ready to clean up resource once BCMLT_NOTIF_OPTION_COMMIT is sent. */
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
 * Callback function for bcma_bcmlt_table_traverse().
 * Process for commit logical table operation for the traversed entry.
 */
static int
traversed_entry_commit(void *cookie, bcma_bcmlt_entry_info_t *tei, int entry_no)
{
    trvs_entry_commit_info_t *teci = (trvs_entry_commit_info_t *)cookie;
    bcma_bcmlt_entry_info_t *ei = teci->ei;
    int opc = teci->opc;
    bcmltcmd_params_t *params = &lt_params;
    int rv, idx;

    /* Skip others if errors occurred previously. */
    if (SHR_FAILURE(teci->rv)) {
        return -1;
    }

    /* Add fields to the traversed entry (if any). */
    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        /* Return failure if key field is specified. */
        if (field->flags & BCMA_BCMLT_FIELD_F_KEY) {
            cli_out("%sSet key field %s is not allowed in traversing "
                    "table %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    ei->fields[idx].name, tei->name);
            teci->rv = SHR_E_FAIL;
            return -1;
        }

        if (bcma_bcmlt_entry_field_add(tei, &ei->fields[idx]) < 0) {
            cli_out("%sFail to add field %s to traversed entry_no %d "
                    "in table %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    ei->fields[idx].name, entry_no, tei->name);
            teci->rv = SHR_E_FAIL;
            return -1;
        }
    }

    /* Commit operation for the traversed entry. */
    if (opc != BCMLT_OPCODE_NOP) {
        rv = bcmlt_entry_commit(tei->eh, opc, params->priority);
        if (SHR_SUCCESS(rv)) {
            bcmlt_entry_info_t bcmlt_ei;

            rv = bcmlt_entry_info_get(tei->eh, &bcmlt_ei);
            if (SHR_SUCCESS(rv)) {
                rv = bcmlt_ei.status;
            }
        }
        if (SHR_FAILURE(rv)) {
            cli_out("%s%s operation failed while traversing table %s: "
                    BCMA_BCMLT_ERRMSG_FMT"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, bcmltcmd_lt_opcode_format(opc),
                    tei->name, BCMA_BCMLT_ERRMSG(rv));
            teci->rv = rv;
            return -1;
        }
    }

    /* Display traversed entry information. */
    if (entry_no != 0) {
        cli_out("\n");
    }
    return bcma_bcmlt_entry_list(tei, "    ",
                                 opc == BCMLT_OPCODE_LOOKUP ? 0 : 1,
                                 params->disp_word);
}

/*
 * LT traverse operation.
 */
static int
op_commit_traverse(bcma_bcmlt_entry_info_t *ei)
{
    int cnt;
    trvs_entry_commit_info_t teci;
    bcmlt_opcode_t opcode = BCMLT_OPCODE_NOP;

    sal_memset(&teci, 0, sizeof(teci));
    teci.ei = ei;
    if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_TRVS_LKUP) {
        opcode = BCMLT_OPCODE_LOOKUP;
    } else if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_TRVS_UPD) {
        opcode = BCMLT_OPCODE_UPDATE;
    } else if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_TRVS_DEL) {
        opcode = BCMLT_OPCODE_DELETE;
    }
    teci.opc = opcode;

    cnt = bcma_bcmlt_table_traverse(ei->unit, ei->name,
                                    ei->def_fields, ei->num_def_fields,
                                    traversed_entry_commit, &teci);

    if (SHR_FAILURE(teci.rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (cnt > 0) {
        cli_out("  %d entr%s traversed", cnt, (cnt > 1) ? "ies are" : "y is");
        if (opcode == BCMLT_OPCODE_NOP) {
            cli_out(".\n");
        } else {
            cli_out(" for %s operation.\n", bcmltcmd_lt_opcode_format(opcode));
        }
        return BCMA_CLI_CMD_OK;
    }

    if (cnt < 0) {
        cli_out("    "BCMA_BCMLT_ERRMSG_FMT"\n", BCMA_BCMLT_ERRMSG(cnt));
    }
    return BCMA_CLI_CMD_FAIL;
}

/*
 * Process for commit logical table operation.
 */
static int
lt_table_op_commit(bcma_bcmlt_entry_info_t *ei, int opcode)
{
    int rv;
    bcmltcmd_params_t *params = &lt_params;
    bcmlt_opcode_t opc = opcode;

    lt_table_op_pre_commit(ei, opc);

    if (opc == BCMLT_OPCODE_TRAVERSE) {
        return op_commit_traverse(ei);
    }

    if (params->async) {
        /* Commit logical table in asynchrnous mode. */
        rv = bcmlt_entry_commit_async(ei->eh, opc, (void *)ei, params->notify,
                                      lt_commit_cb, params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%slt %s %s: bcmlt_entry_commit_async failed: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_lt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
        }

        /*
         * Need to cleanup resourece after some time if no notify event
         * in asynchronous mode.
         */
        if (params->notify == BCMLT_NOTIF_OPTION_NO_NOTIF) {
            bcmltcmd_schedule_entry_info_cleanup(ei);
        }
    } else {
        /* Commit logical table in synchrnous mode. */
        rv = bcmlt_entry_commit(ei->eh, opc, params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%slt %s %s: bcmlt_entry_commit failed: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_lt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
            return -1;
        }

        /* Perform post-op only if entry is committed successfully. */
        if (lt_table_op_post_commit(ei, opc) != 0) {
            return -1;
        }
    }

    return 0;
}

/*
 * Parse logical table operation in syntax: <op> [<field>=<val> ...]
 */
static int
lt_table_op_parse(bcma_cli_args_t *args, bcma_bcmlt_entry_info_t *ei, int *opc)
{
    char *arg;
    char buf[16];
    char *opts;
    char opt;

    /* Get logical table operation code */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    *opc = lt_opcode_parse(arg);
    if (*opc == BCMLT_OPCODE_NOP) {
        /* Not a valid op code, restore the argument */
        BCMA_CLI_ARG_PREV(args);
        return BCMA_CLI_CMD_USAGE;
    }

    /* Check for operation options. */
    ei->opts = 0;
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        while ((opt = *opts++) != '\0') {
            if (!lt_opcode_option_check(ei, *opc, opt)) {
                cli_out("%sOption '%c' is not supported in %s "
                        "operation.\n", BCMA_CLI_CONFIG_ERROR_STR,
                        opt, bcmltcmd_lt_opcode_format(*opc));
                return BCMA_CLI_CMD_FAIL;
            }
        }
    }

    if (bcmltcmd_parse_fields(args, ei) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Do logical table opeations input from CLI for entry based commitment.
 */
static int
do_lt_by_entry(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcmltcmd_params_t *params = &lt_params;
    bcmltcmd_cb_t cb;

    cb.table_op_parse = lt_table_op_parse;
    cb.table_op_commit = lt_table_op_commit;

    return bcmltcmd_commit_by_entry(cli, args, params, &cb, TRUE);
}

/* lt sub-command handler for table operation */
static int
lt_command_table_op(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    /* Curretnly support entry commit. */
    return do_lt_by_entry(cli, args);
}

/* lt sub-command "traverse" handler */
static int
lt_command_traverse(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcmltcmd_params_t *params = &lt_params;

    if (bcmltcmd_tables_traverse(cli, args, TRUE, params->disp_word) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/* lt sub-command "list" handler */
static int
lt_command_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    if (bcmltcmd_tables_list(cli, args, TRUE) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

char *
bcmltcmd_lt_opcode_format(bcmlt_opcode_t opc)
{
    bcma_cli_parse_enum_t *e = &lt_cmds_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_opcode_t)e->val == opc) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid OP>" : e->name;
}

const bcma_cli_parse_enum_t *
bcmltcmd_lt_opcodes_get(void)
{
    return lt_cmds_enum;
}

int
bcma_bcmltcmd_lt(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *arg;
    bcmltcmd_params_t *params = &lt_params;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Set lt command unit number */
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

    if ((arg = BCMA_CLI_ARG_CUR(args)) != NULL) {
        if (sal_strcmp(arg, "list") == 0) {
            /* lt command for tables list */
            BCMA_CLI_ARG_NEXT(args);
            return lt_command_list(cli, args);
        } else if (sal_strcmp(arg, "traverse") == 0) {
            /* lt command for tables traverse */
            BCMA_CLI_ARG_NEXT(args);
            return lt_command_traverse(cli, args);
        }
    }

    if (BCMA_CLI_ARG_CNT(args) > 0) {
        /* lt command for tables operation */
        rv = lt_command_table_op(cli, args);
    }

    return rv;

}
