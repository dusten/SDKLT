/*! \file bcmltcmd_list.c
 *
 * Sub-command 'list' of table access commands in CLI.
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

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"

#define DEFAULT_GROUP_COUNT_PER_LT  5
#define DESC_PREFIX_LEN_MAX         1024

/*!
 * \brief Arguments for table list callback function.
 *
 * This structure is used to store unit and flag for the
 * callback function used to list the LT table and field.
 * This would be passed as cookie information for the callback function.
 */
typedef struct table_list_args_s {
    int unit;    /*!< Device unit number */
    int cb_flag; /*!< Option to indicate the level of information. */
} table_list_args_t;

/* Format the PT information for each LT field. */
static const char*
field_pt_info(shr_pb_t *pb, const char *pf_str,
                       int unit, const char *tbl_name,
                       bcmlt_field_def_t *df)
{
    const char *prefix = pf_str ? pf_str : "";
    bcmlt_field_map_hdl_t *info, *fld_info;
    uint32_t num_ent = 1, num_map = 1, ent, map;
    int rv;
    uint32_t size;

    if (pb == NULL || df == NULL) {
        return shr_pb_str(pb);
    }

    /* Get PT mapping. */
    while (1) {
        /* Calculate size based on number of entries available per field
         * and the number of PT mapping per entry.
         */
        size = ((sizeof(bcmlt_field_map_hdl_t) +
                 (sizeof(bcmlt_field_map_info_t) * num_map)) *
                num_ent);

        /* List of all handlers. */
        fld_info = sal_alloc(size, "FIELD INFO LIST");
        if (fld_info == NULL) {
            return shr_pb_str(pb);
        }
        sal_memset(fld_info, 0, size);
        fld_info->num_map = num_map;

        rv = bcmlt_field_pt_map_get(unit, tbl_name,
                                    df->name, &num_ent,
                                    fld_info);
        if (rv == SHR_E_FULL) {
            /* Either number of handles allocated is insufficient OR
             * Number of PT field mapping allocated is insufficient. */
            num_map = fld_info->num_map;
            sal_free(fld_info);
            continue;
        }

        /* Received all the information. Form output strings. */
        for (ent = 0; ent < num_ent; ent++) {
            size = sizeof(bcmlt_field_map_hdl_t) +
                (sizeof(bcmlt_field_map_info_t) * num_map);
            info = (bcmlt_field_map_hdl_t *)
                ((char *)fld_info + (size * ent));
            if (sal_strlen(info->map_info[0].fmap) != 0) {
                if (sal_strlen(info->int_fld_type) != 0) {
                    shr_pb_printf(pb, "%sHandler: %s (%s)\n", prefix,
                                  info->handler,
                                  info->int_fld_type);
                } else {
                    shr_pb_printf(pb, "%sHandler: %s\n", prefix,
                                  info->handler);
                }
                for (map = 0; map < num_map; map++) {
                    shr_pb_printf(pb, "  %sMap: %s\n",
                                  prefix, info->map_info[map].fmap);
                    if ((!sal_strcmp(info->handler, "Direct")) ||
                        (!sal_strcmp(info->handler, "Transform"))) {
                        shr_pb_printf(pb, "  %sBit (min, max): %d, %d\n",
                                      prefix,
                                      info->map_info[map].minbit,
                                      info->map_info[map].maxbit);
                    }
                }
            }
        }

        sal_free(fld_info);
        return shr_pb_str(pb);
    }

    return shr_pb_str(pb);
}

static const char*
pt_info_format(shr_pb_t *pb, int unit, const char *tbl_name, uint32_t *count)
{
    uint32_t pt_count = DEFAULT_GROUP_COUNT_PER_LT;
    const char **pt_names = NULL;
    int rv;

    do {
        pt_names = sal_alloc(sizeof(const char *) * pt_count, "BCMLT PT INFO");
        if (pt_names == NULL) {
            return shr_pb_str(pb);
        }
        sal_memset(pt_names, 0, sizeof(const char *) * pt_count);

        rv = bcmlt_table_pt_name_get(unit, tbl_name, &pt_count, pt_names);
        if (rv == SHR_E_FULL) {
            sal_free(pt_names);
            continue;
        }

        /* Process PT information. */
        shr_pb_printf(pb, "  PT Reference:\n");
        if (pt_count) {
            bcma_bcmlt_format_strs_array(pb, "    ", pt_names, pt_count);
        } else {
            shr_pb_printf(pb, "    Custom table handler\n");
        }

        *count = pt_count;
        sal_free(pt_names);
        break;
    } while (1);

    return shr_pb_str(pb);
}

/* Format table/field desciption.
 * Append prefix at beginning of each line in the description.
 */
static int
format_description(const char *src, char *dst, char *prefix, int prefix_len_max)
{
    char *line;
    char *desc, *desc_orig;
    bool first_line = TRUE;
    char *null_prefix = "";
    int prefix_len = 0;
    char *dst_ptr = dst;

    desc_orig = desc = sal_strdup(src);

    /* Split the description into lines and add prefix to eahc line. */
    while ((line = sal_strtok_r(desc, "\n", &desc))) {
        char *line_prefix = prefix;

        /* Prefix not needed for the first line. */
        if (first_line == TRUE) {
            line_prefix = null_prefix;
            first_line = FALSE;
        }
        prefix_len += sal_strlen(line_prefix);
        if (prefix_len > prefix_len_max) {
            /* Insufficient memory. */
            sal_free(desc_orig);
            return SHR_E_MEMORY;
        }

        /* Add prefix at the start of the line. */
        sal_memcpy(dst_ptr, line_prefix, sal_strlen(line_prefix));
        dst_ptr += sal_strlen(line_prefix);

        /* Copy the contents to dst. Include "\n" at the end of the line. */
        sal_memcpy(dst_ptr, line, sal_strlen(line) + 1);
        dst_ptr += sal_strlen(line);

        sal_strcat(dst_ptr, "\n");
        dst_ptr += 1;
    }

    sal_free(desc_orig);
    return SHR_E_NONE;
}

static const char *
display_description(shr_pb_t *pb, int unit,
                    const char *tbl_name,
                    const char *fld_name,
                    const char *prefix)
{
    const char *desc = NULL;
    char *desc_formatted = NULL;
    int prefix_len_max = DESC_PREFIX_LEN_MAX;
    char *line_prefix;
    int rv;

    if (fld_name == NULL) {
        rv = bcmlt_table_desc_get(unit, tbl_name, &desc);
    } else {
        rv = bcmlt_field_desc_get(unit, tbl_name, fld_name, &desc);
    }

    if (rv != SHR_E_NONE) {
        return shr_pb_str(pb);
    }

    /* Handle NULL description. */
    if (sal_strlen(desc) == 0) {
        shr_pb_printf(pb, "%sDescription: <none>\n", prefix);
        return shr_pb_str(pb);
    }
    line_prefix = sal_alloc(sal_strlen(prefix) +
                            sal_strlen("Description: ") + 1,
                            "Description Prefix");
    if (line_prefix == NULL) {
        return shr_pb_str(pb);
    }
    sal_strncpy(line_prefix, prefix, sal_strlen(prefix));
    sal_strcpy(line_prefix + sal_strlen(prefix), "            ");

    desc_formatted = sal_alloc(sal_strlen(desc) +
                               prefix_len_max,
                               "Table/Field description display");
    if (desc_formatted == NULL) {
        sal_free(line_prefix);
        return shr_pb_str(pb);
    }
    sal_memset(desc_formatted, 0, sal_strlen(desc) + prefix_len_max);

    /* Format description by appending prefix for each line. */
    while (format_description(desc, desc_formatted,
                              line_prefix,
                              prefix_len_max) == SHR_E_MEMORY) {
        prefix_len_max = prefix_len_max + DESC_PREFIX_LEN_MAX;
        sal_free(desc_formatted);
        desc_formatted = sal_alloc(sal_strlen(desc) +
                                   prefix_len_max,
                                   "Table/Field description display");
        if (desc_formatted == NULL) {
            sal_free(line_prefix);
            return shr_pb_str(pb);
        }
        sal_memset(desc_formatted, 0, sal_strlen(desc) + prefix_len_max);
    }

    /* Display description. */
    shr_pb_printf(pb, "%sDescription: %s", prefix, desc_formatted);

    sal_free(line_prefix);
    sal_free(desc_formatted);
    return shr_pb_str(pb);
}

/*
 * Format the specified table and its fields to the print buffer.
 */
static const char *
default_fields_format(shr_pb_t *pb, uint32_t flag,
                      int unit, const char *tbl_name,
                      bcmlt_field_def_t *fields, uint32_t num_fields)
{
    uint32_t idx, num_key_fields;
    bcmlt_field_def_t *df;
    uint32_t pt_count = DEFAULT_GROUP_COUNT_PER_LT;

    shr_pb_printf(pb, "%s\n", tbl_name);

    /* Display description if requested. */
    if (flag & BCMA_BCMLT_TBL_LIST_OPT_DESC) {
        const char *prefix = "  ";
        display_description(pb, unit, tbl_name, NULL, prefix);
    }

    /* For non-CTH tables, display PT info. */
    if (flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO) {
        pt_info_format(pb, unit, tbl_name, &pt_count);

        /* Skip fields display if PT count is 0 and only PT info is needed. */
        if ((flag == BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO) &&
            (pt_count == 0)) {
            return shr_pb_str(pb);
        }
    }

    /* Get the number key fields */
    num_key_fields = 0;
    for (idx = 0; idx < num_fields; idx++) {
        df = &fields[idx];
        if (df->key) {
            num_key_fields++;
        }
    }

    shr_pb_printf(pb, "  %d field%s (%d key-type field%s):\n",
                  (int)num_fields, num_fields > 1 ? "s" : "",
                  (int)num_key_fields, num_key_fields > 1 ? "s" : "");

    for (idx = 0; idx < num_fields; idx++) {
        df = &fields[idx];
        shr_pb_printf(pb, "    %s\n", df->name);
        /* Display description if requested. */
        if (flag & BCMA_BCMLT_TBL_LIST_OPT_DESC) {
            const char *prefix = "        ";
            display_description(pb, unit, tbl_name, df->name, prefix);
        }

        /* For non-CTH tables, display PT info. */
        if (flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO) {
            bcma_bcmlt_entry_field_default_info_format(pb, "        ",
                                                       unit, tbl_name, df);
        }
        if ((flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO) && (pt_count != 0)) {
            field_pt_info(pb, "        ", unit, tbl_name, df);
        }
    }

    return shr_pb_str(pb);
}

/*
 * bcma_bcmlt_tables_search callback function.
 * List table name, fields names and fields information.
 */
static int
table_list(void *cookie, const char *tbl_name,
           bcmlt_field_def_t *fields, uint32_t num_fields)
{
    table_list_args_t *args;
    int unit, flag;
    shr_pb_t *pb = shr_pb_create();

    args = (table_list_args_t *)cookie;
    unit = args->unit;
    flag = args->cb_flag;

    default_fields_format(pb, flag, unit,
                          tbl_name, fields, num_fields);
    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return 0;
}

/*
 * bcma_bcmlt_tables_search callback function.
 * List table name in raw.
 */
static int
table_list_raw(void *cookie, const char *tbl_name,
               bcmlt_field_def_t *fields, uint32_t num_fields)
{
    cli_out("%s\n", tbl_name);

    return 0;
}

int
bcmltcmd_tables_list(bcma_cli_t *cli, bcma_cli_args_t *args, bool logical)
{
    int rv = 0;
    int unit = cli->cmd_unit;
    char *arg, *arg_start, *opts;
    char buf[16], opt;
    bcma_bcmlt_table_info_f cb = table_list;
    table_list_args_t list_args;
    uint32_t search_scope = logical ? BCMA_BCMLT_SCOPE_F_LTBL :
                                      BCMA_BCMLT_SCOPE_F_PTBL;

    /* Initialize arguments for table list callback function. */
    list_args.unit = unit;
    list_args.cb_flag = 0;

    /* Check for list options. */
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        bool tbl_name_only = false;
        while ((opt = *opts++) != '\0') {
            if (opt == 'b') {
                /* List table names only. */
                tbl_name_only = true;
            } else if (opt == 'l') {
                list_args.cb_flag |= BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO;
            } else if (opt == 'm') {
                list_args.cb_flag |= BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO;
            } else if (opt == 'd') {
                list_args.cb_flag |= BCMA_BCMLT_TBL_LIST_OPT_DESC;
            } else {
                cli_out("%sOption '%c' is not supported.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, opt);
                return -1;
            }
        }
        if (list_args.cb_flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO ||
            list_args.cb_flag & BCMA_BCMLT_TBL_LIST_OPT_DESC ||
            list_args.cb_flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO) {
            if (tbl_name_only) {
                cli_out("Warning : Ignoring option 'b'\n");
            }
        } else if (tbl_name_only) {
            cb = table_list_raw;
        }
    }

    /* List all tables. */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        rv += bcma_bcmlt_tables_search(unit, "*", BCMA_BCMLT_MATCH_SUBSTR,
                                       search_scope, cb, &list_args);
        if (rv <= 0) {
            cli_out("No available %s tables.\n",
                    logical ? "logical" : "physical");
        }
        return 0;
    }

    /* List the specified table(s) */
    arg_start = arg;
    do {
        rv += bcma_bcmlt_tables_search(unit, arg, BCMA_BCMLT_MATCH_SUBSTR,
                                       search_scope, cb, &list_args);
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
