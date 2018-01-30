/*! \file bcmltcmd_traverse.c
 *
 * Sub-command 'traverse' of table access commands in CLI.
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

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"

/*
 * Cookie for bcma_bcmlt_tables_search() call-back function
 * table_traverse()
 */
typedef struct tbl_search_trvs_data_s {
    int unit;
    bool verbose;
    bool all_fields;
    uint32_t disp_word;
} tbl_search_trvs_data_t;

/*
 * Callback function of bcma_bcmlt_table_traverse().
 * List fields values of the traversed entry.
 */
static int
traversed_entry_list(void *cookie, bcma_bcmlt_entry_info_t *ei, int entry_no)
{
    tbl_search_trvs_data_t *td = (tbl_search_trvs_data_t *)cookie;
    bool verbose = td->verbose;
    bool all_fields = td->all_fields;
    uint32_t disp_word = td->disp_word;

    if (entry_no == 0) {
        if (!verbose) {
            cli_out("Traverse table [%s]:\n", ei->name);
        }
    } else {
        cli_out("\n");
    }
    bcma_bcmlt_entry_list(ei, "    ", all_fields ? 0 : 1, disp_word);

    return 0;
}

/*
 * Callback frunction of bcma_bcmlt_tables_search().
 * Traverse the matched table and list fields values of the traversed entry.
 */
static int
table_traverse(void *cookie, const char *name,
               bcmlt_field_def_t *fields, uint32_t num_fields)
{
    tbl_search_trvs_data_t *td = (tbl_search_trvs_data_t *)cookie;
    int unit = td->unit;
    bool verbose = td->verbose;
    int cnt;

    if (verbose) {
        cli_out("Traverse table [%s]:\n", name);
    }

    cnt = bcma_bcmlt_table_traverse(unit, name, fields, num_fields,
                                    traversed_entry_list, td);

    if (cnt <= 0) {
        if (verbose) {
            /* No entries are found. */
            cli_out("  "BCMA_BCMLT_ERRMSG_FMT"\n", BCMA_BCMLT_ERRMSG(cnt));
        }
    } else {
        cli_out("  %d entr%s traversed.\n",
                cnt, (cnt > 1) ? "ies are" : "y is");
    }

    return 0;
}

int
bcmltcmd_tables_traverse(bcma_cli_t *cli, bcma_cli_args_t *args, int logical,
                         uint32_t disp_word)
{
    int rv = 0;
    int unit = cli->cmd_unit;
    char *arg, *arg_start, *opts;
    char buf[16], opt;
    bool verbose = false;
    bool all_fields = false;
    tbl_search_trvs_data_t td;
    uint32_t search_scope = logical ? BCMA_BCMLT_SCOPE_F_LTBL :
                                      BCMA_BCMLT_SCOPE_F_PTBL;

    /* Check for list options. */
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        while ((opt = *opts++) != '\0') {
            if (opt == 'v') {
                /* Verbose mode. */
                verbose = true;
            } else if (opt == 'l') {
                /* List all fields instead of key fields only. */
                all_fields = true;
            } else {
                cli_out("%sOption '%c' is not supported.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, opt);
                return -1;
            }
        }
    }

    sal_memset(&td, 0, sizeof(td));
    td.unit = unit;
    td.verbose = verbose;
    td.all_fields = all_fields;
    td.disp_word = disp_word;

    /* Traverse all tables. */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        rv += bcma_bcmlt_tables_search(unit, "*", BCMA_BCMLT_MATCH_SUBSTR,
                                       search_scope, table_traverse, &td);
        if (rv <= 0) {
            cli_out("No available %s tables.\n",
                    logical ? "logical" : "physical");
        }
        return 0;
    }

    /* Traverse the specified table(s) */
    arg_start = arg;
    do {
        rv += bcma_bcmlt_tables_search(unit, arg, BCMA_BCMLT_MATCH_SUBSTR,
                                       search_scope, table_traverse, &td);
    } while ((arg = BCMA_CLI_ARG_GET(args)) != NULL);

    if (rv <= 0) {
        BCMA_CLI_ARG_PREV(args);
        if (arg_start[0] == '@' && arg_start == BCMA_CLI_ARG_GET(args)) {
            /* Can not find the exact match table. */
            cli_out("%sUnsupported %s table: %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    logical ? "logical" : "physical", arg_start + 1);
        } else {
            cli_out("No matching %s tables.\n", logical ? "logical" : "physical");
        }
        BCMA_CLI_ARG_NEXT(args);
    }

    return 0;
}
