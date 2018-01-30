/*! \file bcmltcmd.c
 *
 * Common functions for physical and logical table access commands in CLI.
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
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_token.h>

#include <bcmlt/bcmlt.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"

/* Commit priority */
static bcma_cli_parse_enum_t commit_pri_enum[] = {
    { "NORMAL", BCMLT_PRIORITY_NORMAL },
    { "HIGH",   BCMLT_PRIORITY_HIGH   },
    { NULL,     0                     }
};

/* Commit notify option */
static bcma_cli_parse_enum_t notify_opt_enum[] = {
    { "COMMIT",    BCMLT_NOTIF_OPTION_COMMIT   },
    { "HW",        BCMLT_NOTIF_OPTION_HW       },
    { "ALL",       BCMLT_NOTIF_OPTION_ALL      },
    { "NO_NOTIFY", BCMLT_NOTIF_OPTION_NO_NOTIF },
    { NULL,        0                           }
};

#if 0
/* Commit mode enumeration */
static bcma_cli_parse_enum_t commit_mode_enum[] = {
    { "ENTRY", BCMLTCMD_COMMIT_BY_ENTRY },
    { "TRANS", BCMLTCMD_COMMIT_BY_TRANS },
    { NULL,    0                        }
};
#endif

/* Field value display option */
static bcma_cli_parse_enum_t disp_mode_enum[] = {
    { "HEXadecimal",    BCMA_BCMLT_DISP_MODE_HEX     },
    { "DECimal",        BCMA_BCMLT_DISP_MODE_DEC     },
    { "HexDec",         BCMA_BCMLT_DISP_MODE_HEX_DEC },
    { NULL,             0                            }
};

/* BCMLT entry attributes */
static bcma_cli_parse_enum_t ent_attr_enum[] = {
    { "CACHE_BYPASS",   BCMLT_ENT_ATTR_GET_FROM_HW },
    { NULL,             0                          }
};

/*
 * Parse and do entry operation in format:
 * <table_name> <table_op> [<table_op> ...]
 */
static int
entry_op(bcma_cli_t *cli, bcma_cli_args_t *args, bcma_bcmlt_entry_info_t **ei,
         bcmltcmd_params_t *params, bcmltcmd_cb_t *cb, bool logical)
{
    int rv;
    const char *table_name;
    int opc;
    uint32_t attr;

    /* Get entry table name */
    if ((table_name = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Check entry attribute */
    attr = bcmltcmd_parse_attr(args);

    *ei = bcma_bcmlt_entry_info_create(params->unit, table_name, logical, attr,
                                       NULL, 0);
    if (*ei == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = cb->table_op_parse(args, *ei, &opc);
    if (rv != BCMA_CLI_CMD_OK) {
        /*
         * At least one valid table operation should be there
         * for a table entry.
         */
       bcma_bcmlt_entry_info_destroy(*ei);
       return rv;
    }

    cli_out("Table %s:\n", (*ei)->name);

    rv = cb->table_op_commit(*ei, opc);
    if (rv != 0) {
        bcma_bcmlt_entry_info_destroy(*ei);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Try if any other table operations for the entry */
    while (BCMA_CLI_ARG_CNT(args) > 0) {
        if (params->async) {
            /*
             * Each table operation needs a new entry for asynchrnous mode.
             * Pre-allocated a new entry for asynchronous mode to parse
             * table operation.
             */
            *ei = bcma_bcmlt_entry_info_create(params->unit, table_name,
                                               logical, attr, NULL, 0);
            if (*ei == NULL) {
                return BCMA_CLI_CMD_FAIL;
            }
        }

        rv = cb->table_op_parse(args, *ei, &opc);
        if (rv != BCMA_CLI_CMD_OK) {
            /* End of table operation(s) of entry */
            if (params->async) {
                /* Free the pre-allocated entry for asynchronous mode. */
                bcma_bcmlt_entry_info_destroy(*ei);
            }
            break;
        }

        if (params->async) {
            cli_out("\nTable %s:\n", (*ei)->name);
        }

        rv = cb->table_op_commit(*ei, opc);
        if (rv != 0) {
            bcma_bcmlt_entry_info_destroy(*ei);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    /*
     * Clean up entry info directly for synchronous mode.
     * The entry info for asynchronous mode will be cleaned up in
     * commit callback function or scheduled cleanup.
     */
    if (!params->async) {
        bcma_bcmlt_entry_info_destroy(*ei);
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Entry cleanup thread for asynchronous commit
 */
static void
entry_info_cleanup_thread(void *arg)
{
    bcma_bcmlt_entry_info_t *ei = (bcma_bcmlt_entry_info_t *)arg;

    sal_sleep(BCMLTCMD_SCHEDULE_ENTRY_CLEANUP_DELAY_TIME);

    bcma_bcmlt_entry_info_destroy(ei);
}

static int
entry_key_fields_default_val_set(bcma_bcmlt_entry_info_t *ei)
{
    int idx, df_idx;

    for (df_idx = 0; df_idx < ei->num_def_fields; df_idx++) {
        bcmlt_field_def_t *df = &ei->def_fields[df_idx];

        if (!df->key) {
            continue;
        }

        for (idx = 0; idx < ei->num_fields; idx++) {
            if (df->name == ei->fields[idx].name) {
                break;
            }
        }

        /* Set the unspecified key field with its default value. */
        if (idx == ei->num_fields) {
            bcma_bcmlt_field_t *field = &ei->fields[ei->num_fields];

            if (bcma_bcmlt_entry_field_default_val_set(ei, df, field) < 0) {
                return -1;
            }
            ei->num_fields++;
        }
    }

    return 0;
}

int
bcmltcmd_schedule_entry_info_cleanup(bcma_bcmlt_entry_info_t *ei)
{
    sal_thread_t tid;

    tid = sal_thread_create("ei_cleanup", SAL_THREAD_STKSZ, 50,
                            entry_info_cleanup_thread, ei);
    if (tid == SAL_THREAD_ERROR) {
        cli_out("%sbcmltcmd_schedule_entry_info_cleanup: "
                "Fail to create thread.\n", BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    return 0;
}

int
bcmltcmd_commit_by_entry(bcma_cli_t *cli, bcma_cli_args_t *args,
                         bcmltcmd_params_t *params, bcmltcmd_cb_t *cb,
                         bool logical)
{
    int rv;

    if (!params || !cb || !cb->table_op_parse || !cb->table_op_commit) {
        return BCMA_CLI_CMD_FAIL;
    }

    while (BCMA_CLI_ARG_CNT(args) > 0) {
        bcma_bcmlt_entry_info_t *ei;

        rv = entry_op(cli, args, &ei, params, cb, logical);
        if (rv != BCMA_CLI_CMD_OK) {
            return rv;
        }
    }

    return BCMA_CLI_CMD_OK;
}

int
bcmltcmd_parse_fields(bcma_cli_args_t *args, bcma_bcmlt_entry_info_t *ei)
{
    char *arg;
    char **fields_arg = NULL;
    int idx, rv = 0;
    int num_args;

    if (!ei) {
        return -1;
    }

    num_args = BCMA_CLI_ARG_CNT(args);
    if (num_args > ei->num_def_fields) {
        /*
         * Enlarge the fields array since the number of arguments
         * is greater than the default fields array size.
         */
        if (bcma_bcmlt_entry_fields_create(ei, num_args) < 0) {
            return -1;
        }
    }
    if (num_args > 0) {
        fields_arg = sal_alloc(num_args * sizeof(char *),
                               "bcmaCliLtFieldsArg");
        if (fields_arg == NULL) {
            cli_out("Fail to allocate array of field arguments.\n");
            return -1;
        }
    }

    /* Clear field numbers */
    ei->num_fields = 0;

    /*
     * Get Field setting if any, in format:
     * <field_name>=<val> <field_name>=<val> ...
     */
    while ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        char *ptr;

        ptr = sal_strchr(arg, '=');
        if (ptr) {
            /* Simple sanity check for =<val> or <field>= */
            if (ptr == arg || *(ptr + 1) == '\0') {
                cli_out("%sInvalid field assignment: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, arg);
                sal_free(fields_arg);
                return -1;
            }
        } else {
            /* Not a field assignment format, restore the argument */
            BCMA_CLI_ARG_PREV(args);
            break;
        }

        fields_arg[ei->num_fields] = arg;
        ei->num_fields++;
    }

    /* Parse fields value */
    for (idx = 0; idx < ei->num_fields; idx++) {
        if (bcma_bcmlt_entry_field_parse(ei, &ei->fields[idx],
                                         fields_arg[idx]) < 0) {
            rv = -1;
        }
    }

    if (fields_arg) {
        sal_free(fields_arg);
    }

    if (rv < 0) {
        return -1;
    }

    /* Check for auto-fill option for key fields. */
    if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_AUTO_KFS) {
        if (entry_key_fields_default_val_set(ei) < 0) {
            return -1;
        }
    }

    /* Add fields value to entry */
    for (idx = 0; idx < ei->num_fields; idx++) {
        if (bcma_bcmlt_entry_field_add(ei, &ei->fields[idx]) < 0) {
            rv = -1;
        }
    }

    return (rv == 0) ? 0 : -1;
}

uint32_t
bcmltcmd_parse_attr(bcma_cli_args_t *args)
{
    uint32_t attr = 0;
    char *arg;
    char *cmp_str = "attr=";
    int cmp_strlen = sal_strlen(cmp_str);

    arg = BCMA_CLI_ARG_CUR(args);
    if (arg == NULL) {
        return 0;
    }

    if (sal_strncmp(arg, cmp_str, cmp_strlen) == 0) {
        const char *str;
        bcma_cli_tokens_t *tok;
        int idx;

        BCMA_CLI_ARG_NEXT(args);

        str = arg + cmp_strlen;
        if (*str == '\0') {
            return 0;
        }

        tok = sal_alloc(sizeof(*tok), "bcmaBcmltcmdTok");
        if (tok == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Failed to allocate parsing buffer\n")));
            return 0;
        }
        if (bcma_cli_tokenize(str, tok, ",") < 0) {
            sal_free(tok);
            return 0;
        }

        /* Comopare every attribute token to supported attributes */
        for (idx = 0; idx < tok->argc; idx++) {
            bcma_cli_parse_enum_t *e = ent_attr_enum;
            int match = 0;

            while (e->name != NULL) {
                if (sal_strcasecmp(e->name, tok->argv[idx]) == 0) {
                    attr |= e->val;
                    match = 1;
                    break;
                }
                e++;
            }

            /* Disply error for unsupported attributes. */
            if (!match) {
                cli_out("%sInvalid table attribute: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, tok->argv[idx]);
            }
        }
        sal_free(tok);
    }

    return attr;
}

char *
bcmltcmd_parse_options(bcma_cli_args_t *args, int buf_size, char *buf)
{
    char *arg;
    char opt;
    int cnt = 0;

    if (buf == NULL || buf_size <= 0) {
        return NULL;
    }

    sal_memset(buf, 0, buf_size);

    while ((arg = BCMA_CLI_ARG_CUR(args)) != NULL) {
        if (*arg != '-') {
            break;
        }
        BCMA_CLI_ARG_NEXT(args);
        arg++;
        while ((opt = *arg++) != '\0') {
            if (cnt >= buf_size) {
                break;
            }
            buf[cnt++] = opt;
        }
    }

    return buf;
}

int
bcmltcmd_parse_parameters(bcma_cli_t *cli, bcma_cli_args_t *args,
                          bcmltcmd_params_t *cmd_params)
{
    int rv = 0;
    bcma_cli_parse_table_t prstbl;
    bcma_bcmlt_display_mode_t field_disp;
    bcma_bcmlt_display_mode_t arrfield_disp;

    if (!cmd_params) {
        return -1;
    }

    field_disp = BCMA_BCMLT_FIELD_DISP_MODE_GET(cmd_params->disp_word);
    arrfield_disp = BCMA_BCMLT_ARRAY_FIELD_DISP_MODE_GET(cmd_params->disp_word);

    /* Add entry/transaction variables to parse table for user modification */
    bcma_cli_parse_table_init(cli, &prstbl);
    bcma_cli_parse_table_add(&prstbl, "async", "int", &cmd_params->async, NULL);
    bcma_cli_parse_table_add(&prstbl, "priority", "enum",
                             &cmd_params->priority, commit_pri_enum);
    bcma_cli_parse_table_add(&prstbl, "notify", "enum",
                             &cmd_params->notify, notify_opt_enum);
    bcma_cli_parse_table_add(&prstbl, "ScalarFieldDisplayMode", "enum",
                             &field_disp, disp_mode_enum);
    bcma_cli_parse_table_add(&prstbl, "ArrayFieldDisplayMode", "enum",
                             &arrfield_disp, disp_mode_enum);

    if (bcma_cli_parse_table_do_args(&prstbl, args) < 0) {
        cli_out("%s: Error: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        rv = -1;
    }
    cmd_params->disp_word = BCMA_BCMLT_DISP_WORD_SET(field_disp, arrfield_disp);
    bcma_cli_parse_table_done(&prstbl);

    return rv;
}

const bcma_cli_parse_enum_t *
bcmltcmd_ent_attr_enum_get(void)
{
    return ent_attr_enum;
}
