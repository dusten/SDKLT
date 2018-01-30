/*! \file bcmltcmd_complete.c
 *
 * Common completion functions for physical and logical table
 * access commands in CLI.
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

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"
#include "bcmltcmd_complete_internal.h"

/* Structure to manipulate the strings for auto-complete feature. */
typedef struct complete_match_s {
    /* Array of candidate strings for auto-complete. */
    const char **strs;
    /* Maximum array size of candidate string. */
    int size;
    /* The match prefix string among the candidate strings if not NULL. */
    char *substr;
    /* String length of the match prefix string. */
    int substr_len;
    /* Actual number of elements of the candidate strings array. */
    int cnt;
    /* Used when complete for a fixed syntax, e.g. array field completion. */
    char *unique_str;
    /* Append character */
    int append_char;
} complete_match_t;

static complete_match_t *cur_cmatch = NULL;

/* Cleanup funciton for complete_match_t. */
static void
complete_match_cleanup(complete_match_t *cmatch)
{
    if (cmatch) {
        if (cmatch->strs) {
            sal_free(cmatch->strs);
        }
        if (cmatch->substr) {
            sal_free(cmatch->substr);
        }
        if (cmatch->unique_str) {
            sal_free(cmatch->unique_str);
        }
        sal_free(cmatch);
    }
}

/*
 * Initialization function for complete_match_t.
 */
static complete_match_t *
complete_match_init(char *substr, int size)
{
    complete_match_t *cmatch;

    cmatch = sal_alloc(sizeof(complete_match_t), "bcmaBcmltcmdCompleteMatch");
    if (cmatch == NULL) {
        return NULL;
    }
    sal_memset(cmatch, 0, sizeof(complete_match_t));

    if (substr) {
        cmatch->substr = sal_strdup(substr);
        if (cmatch->substr == NULL) {
            complete_match_cleanup(cmatch);
            return NULL;
        }
        cmatch->substr_len = sal_strlen(substr);
    }

    if (size > 0) {
        cmatch->strs = sal_alloc(sizeof(const char *) * size,
                                 "bcmaBcmltcmdCompleteMatchStrs");
        if (cmatch->strs == NULL) {
            complete_match_cleanup(cmatch);
            return NULL;
        }
        cmatch->size = size;
    }

    cmatch->append_char = ' ';

    return cmatch;
}

/* Add a candidate string to complete_match_t. */
static int
complete_match_strs_add(complete_match_t *cmatch, const char *str)
{
    if (cmatch->cnt >= cmatch->size) {
        return -1;
    }

    cmatch->strs[cmatch->cnt++] = str;

    return 0;
}

/*
 * Check if a given string is matched as a candidate string and add
 * the string to complete_match_t if qualified.
 */
static int
complete_match_strs_compare_add(complete_match_t *cmatch, const char *str)
{
    if (cmatch->substr) {
        if (sal_strncasecmp(str, cmatch->substr, cmatch->substr_len) != 0) {
            return -1;
        }
    }
    return complete_match_strs_add(cmatch, str);
}

/* Add a unique string to complete_match_t */
static int
complete_match_unique_str_add(complete_match_t *cmatch, const char *str)
{
    cmatch->unique_str = sal_strdup(str);

    return cmatch->unique_str ? 0 : -1;
}

/* Customize the append character to the single matched string. */
static int
complete_match_append_char_set(complete_match_t *cmatch, char append_char)
{
    cmatch->append_char = append_char;

    return 0;
}

/* Check whehter the complete_match_t structure contains any matched string. */
static bool
complete_match_is_empty(complete_match_t *cmatch)
{
    if (cmatch) {
        if (cmatch->cnt > 0 || cmatch->unique_str) {
            return false;
        }
    }
    return true;
}

/*
 * The rl_compentry_func_t *entry_func for rl_completion_matches()
 * in readline library, which returns the matched strings one by one and
 * ends with NULL.
 */
static char *
complete_match_strs_entry_func(const char *text, int state)
{
    static int idx;

    if (state == 0) {
        idx = 0;
        if (cur_cmatch == NULL) {
            return NULL;
        }
    }

    while (idx < cur_cmatch->cnt) {
        char *str;

        /*
         * The returned string will be freed within the readline library,
         * so we need to ensure that strdup uses the corresponding memory
         * allocation function.
         */
        str = bcma_rl_strdup(cur_cmatch->strs[idx]);
        if (str == NULL) {
            break;
        }
        idx++;
        return str;
    }

    complete_match_cleanup(cur_cmatch);
    cur_cmatch = NULL;

    return NULL;
}

static char *
complete_match_unique_str_entry_func(const char *text, int state)
{
    if (state == 0) {
        if (cur_cmatch == NULL) {
            return NULL;
        }
        if (cur_cmatch->unique_str) {
            char *str;

            /*
             * The returned string will be freed within the readline library,
             * so we need to ensure that strdup uses the corresponding memory
             * allocation function.
             */
            str = bcma_rl_strdup(cur_cmatch->unique_str);
            if (str) {
                return str;
            }
        }
    }

    complete_match_cleanup(cur_cmatch);
    cur_cmatch = NULL;

    return NULL;
}

/*
 * Callback function of bcma_bcmlt_tables_search() to add the matched
 * table names to the candidate strings.
 */
static int
table_complete_match(void *cookie, const char *tbl_name,
                     bcmlt_field_def_t *tbl_fields, uint32_t num_tbl_fields)
{
    complete_match_t *cmatch = (complete_match_t *)cookie;

    return complete_match_strs_add(cmatch, tbl_name);
}

/*
 * Callback function of bcma_bcmlt_tables_search() to get the matched
 * table name.
 */
static int
table_match_name_get(void *cookie, const char *tbl_name,
                     bcmlt_field_def_t *tbl_fields, uint32_t num_tbl_fields)
{
    const char **name = (const char **)cookie;

    *name = tbl_name;

    return 0;
}

/* Get the number of fields in a specified table. */
static int
num_table_fields_get(int unit, uint32_t search_scope, const char *table_name)
{
    int rv;
    uint32_t num_fields;

    rv = bcmlt_table_field_defs_get(unit, table_name, 0, NULL, &num_fields);
    if (SHR_FAILURE(rv)) {
        return -1;
    }
    return num_fields;
}

/* Get the number of logical tables or physical tables or both. */
static int
num_tables_get(int unit, uint32_t search_scope)
{
    int rv;
    int num_tables = 0;
    uint32_t flags = 0;
    char *table_name;

    if (search_scope & BCMA_BCMLT_SCOPE_F_LTBL) {
        flags |= BCMLT_TABLE_SEARCH_LT;
    }
    if (search_scope & BCMA_BCMLT_SCOPE_F_PTBL) {
        flags |= BCMLT_TABLE_SEARCH_PT;
    }
    if (flags == 0) {
        /* Search both logical and physical tables if no flags are specified. */
        flags = BCMLT_TABLE_SEARCH_LT | BCMLT_TABLE_SEARCH_PT;
    }

    rv = bcmlt_table_names_get_first(unit, flags, &table_name);
    while (SHR_SUCCESS(rv)) {
        num_tables++;
        rv = bcmlt_table_names_get_next(unit, flags, &table_name);
    }

    return num_tables;
}

/*
 * This function will return the default fields information of
 * a specified table. The caller is responsible for freeing the
 * returned array by sal_free().
 */
static bcmlt_field_def_t *
default_fields_info_get(int unit, uint32_t scope, const char *tbl_name,
                        int *fnums)
{
    int rv;
    uint32_t num_fields;
    bcmlt_field_def_t *tbl_fields;

    rv = num_table_fields_get(unit, scope, tbl_name);
    if (rv <= 0) {
        return NULL;
    }
    num_fields = rv;

    tbl_fields = sal_alloc(sizeof(bcmlt_field_def_t) * num_fields,
                           "bcmaBcmltcmdDefFields");
    if (tbl_fields == NULL) {
        return NULL;
    }

    /* Get default fields information. */
    rv = bcmlt_table_field_defs_get(unit, tbl_name, num_fields,
                                    tbl_fields, &num_fields);
    if (SHR_FAILURE(rv)) {
        sal_free(tbl_fields);
        return NULL;
    }

    if (fnums) {
        *fnums = num_fields;
    }
    return tbl_fields;
}

/* Check if the string contains comma for array syntax. */
static complete_match_t *
array_field_check(bcmlt_field_def_t *df, char *match_substr, const char *text)
{
    char *ptr;
    complete_match_t *cmatch;

    if (df->depth == 0 || match_substr == NULL) {
        return NULL;
    }

    if (match_substr[sal_strlen(match_substr) - 1] == ']') {
        cmatch = complete_match_init(NULL, 0);
        if (cmatch) {
            complete_match_unique_str_add(cmatch, text);
            complete_match_append_char_set(cmatch, '=');
        }
        return cmatch;
    }

    ptr = sal_strrchr(match_substr, ',');
    if (ptr++) {
        int len = sal_strlen(ptr);

        if (*ptr == '\0' || sal_strncasecmp(df->name, ptr, len) == 0) {
            /* Auto-complete for field names in field array syntax. */
            cmatch = complete_match_init(NULL, 0);
            if (cmatch) {
                complete_match_unique_str_add(cmatch, df->name);
                complete_match_append_char_set(cmatch, '[');
            }
            return cmatch;
        }
    }
    return NULL;
}

/* Auto-complete function for field value. */
static complete_match_t *
symbol_field_check(int unit, const char *tbl_name, bcmlt_field_def_t *df,
                   char *match_substr, const char *text)
{
    const char **sym_strs;
    uint32_t num_sym_strs, idx;
    complete_match_t *cmatch;

    if (!df->symbol) {
        return NULL;
    }

    sym_strs = bcma_bcmlt_entry_field_symbol_strs_get(unit, tbl_name, df->name,
                                                      &num_sym_strs);
    if (sym_strs == NULL) {
        return NULL;
    }

    cmatch = complete_match_init(match_substr, num_sym_strs);
    if (cmatch) {
        for (idx = 0; idx < num_sym_strs; idx++) {
            complete_match_strs_compare_add(cmatch, sym_strs[idx]);
        }
        /* Do not ended by space if the field is an array. */
        if (df->depth > 0) {
            complete_match_append_char_set(cmatch, '\0');
        }
    }
    sal_free(sym_strs);

    if (complete_match_is_empty(cmatch)) {
        complete_match_cleanup(cmatch);
        return NULL;
    }

    return cmatch;
}

/* Auto-complete function for field value. */
static complete_match_t *
field_values_complete(int unit, uint32_t scope, const char *tbl_name,
                      char *field_name, char *match_substr, const char *text)
{
    int idx, num_fields;
    complete_match_t *cmatch = NULL;
    bcmlt_field_def_t *tbl_fields, *df = NULL;

    /* Get the field of the table. */
    tbl_fields = default_fields_info_get(unit, scope, tbl_name, &num_fields);
    if (tbl_fields == NULL) {
        return NULL;
    }

    /* Find the matching field default information. */
    for (idx = 0; idx < num_fields; idx++) {
        df = &tbl_fields[idx];
        if (sal_strcasecmp(field_name, df->name) == 0) {
            break;
        }
    }
    /* No matching field. */
    if (idx == num_fields) {
        sal_free(tbl_fields);
        return NULL;
    }

    cmatch = array_field_check(df, match_substr, text);
    if (cmatch) {
        sal_free(tbl_fields);
        return cmatch;
    }

    cmatch = symbol_field_check(unit, tbl_name, df, match_substr, text);
    if (cmatch) {
        sal_free(tbl_fields);
        return cmatch;
    }

    /* Display field default information if ends in '=', e.g. <field_name>= */
    if (text[0] == '\0') {
        shr_pb_t *pb = shr_pb_create();

        shr_pb_printf(pb, "%s\n", df->name);
        bcma_bcmlt_entry_field_default_info_format(pb, "    ",
                                                   unit, tbl_name, df);
        bcma_clirlc_info_show(shr_pb_str(pb));
        shr_pb_destroy(pb);
    }

    sal_free(tbl_fields);
    return NULL;
}

/* Auto-complete function for fields. */
static complete_match_t *
fields_complete(int unit, uint32_t scope, const char *tbl_name,
                char *match_substr, const char *text)
{
    int rv, idx, num_fields, last_match_idx = -1;
    uint32_t act_num_fields;
    complete_match_t *cmatch;
    bcmlt_field_def_t *tbl_fields, *df = NULL;
    bool array_syntax = false;

    tbl_fields = default_fields_info_get(unit, scope, tbl_name, &num_fields);
    if (tbl_fields == NULL) {
        return NULL;
    }

    /* Get default fields information. */
    rv = bcmlt_table_field_defs_get(unit, tbl_name, num_fields,
                                    tbl_fields, &act_num_fields);
    if (SHR_FAILURE(rv)) {
        sal_free(tbl_fields);
        return NULL;
    }

    /* Check for array field syntax to get the exact match field name */
    if (match_substr) {
        char *ptr = sal_strchr(match_substr, '[');

        if (ptr && ptr != match_substr) {
            *ptr = '\0';
            array_syntax = true;
        }
    }
    cmatch = complete_match_init(match_substr, num_fields);
    if (cmatch == NULL) {
        sal_free(tbl_fields);
        return NULL;
    }


    for (idx = 0; idx < num_fields; idx++) {
        df = &tbl_fields[idx];
        if (array_syntax) {
            /* match_substr should be the exact field name. */
            if (sal_strcasecmp(df->name, match_substr) == 0) {
                if (complete_match_unique_str_add(cmatch, text) == 0) {
                    last_match_idx = idx;
                    break;
                }
            }
        } else {
            if (complete_match_strs_compare_add(cmatch, df->name) == 0) {
                last_match_idx = idx;
            }
        }
    }

    if ((cmatch->unique_str || cmatch->cnt == 1) && last_match_idx >= 0) {
        char append_char = '=';

        if (tbl_fields[last_match_idx].depth > 0) {
            /* Array field. */
            char last_ch = text[sal_strlen(text) - 1];

            if (array_syntax) {
                if (last_ch == ']') {
                    append_char = '=';
                } else {
                    /* Do nothing. */
                    complete_match_cleanup(cmatch);
                    sal_free(tbl_fields);
                    return NULL;
                }
            } else {
                append_char = '[';
            }
        }
        complete_match_append_char_set(cmatch, append_char);
    }

    sal_free(tbl_fields);

    if (complete_match_is_empty(cmatch)) {
        complete_match_cleanup(cmatch);
        return NULL;
    }

    return cmatch;
}

/* Auto-complete function for tables. */
static complete_match_t *
tables_complete(int unit, uint32_t scope, char *match_substr)
{
    int num_tables;
    complete_match_t *cmatch;
    shr_pb_t *pb;

    num_tables = num_tables_get(unit, scope);
    if (num_tables <= 0) {
        return NULL;
    }

    cmatch = complete_match_init(match_substr, num_tables);
    if (cmatch == NULL) {
        return NULL;
    }

    pb = shr_pb_create();
    if (pb == NULL) {
        complete_match_cleanup(cmatch);
        return NULL;
    }

    if (match_substr == NULL) {
        /* Match for all tables. */
        shr_pb_printf(pb, "*");
    } else {
        /* Match from the beginning for completion. */
        char *ptr = match_substr;

        if (*ptr == '@' || *ptr == '^') {
            ptr++;
        }
        shr_pb_printf(pb, "^%s", ptr);
    }

    bcma_bcmlt_tables_search(unit, shr_pb_str(pb), BCMA_BCMLT_MATCH_SUBSTR,
                             scope, table_complete_match, cmatch);

    shr_pb_destroy(pb);

    if (complete_match_is_empty(cmatch)) {
        complete_match_cleanup(cmatch);
        return NULL;
    }

    return cmatch;
}

/* Auto-complete function for opcodes. */
static complete_match_t *
opcodes_complete(const bcma_cli_parse_enum_t *opcodes, char *match_substr)
{
    complete_match_t *cmatch;
    int idx, num_opcodes = 0;

    while (opcodes[num_opcodes].name != NULL) {
        num_opcodes++;
    }

    cmatch = complete_match_init(match_substr, num_opcodes);
    if (cmatch == NULL) {
        return NULL;
    }

    for (idx = 0; idx < num_opcodes; idx++) {
        complete_match_strs_compare_add(cmatch, opcodes[idx].name);
    }

    if (complete_match_is_empty(cmatch)) {
        complete_match_cleanup(cmatch);
        return NULL;
    }

    return cmatch;
}

/* Auto-complete function for attributes. */
static complete_match_t *
attrbutes_complete(const bcma_cli_parse_enum_t *attrs, char *match_str)
{
    char *attr_str = "attr=";
    int attr_len = sal_strlen(attr_str);
    int len;
    complete_match_t *cmatch = NULL;

    if (match_str == NULL) {
        return NULL;
    }

    len = sal_strlen(match_str);
    if (len < 2) {
        return NULL;
    }

    if (sal_strncasecmp(match_str, attr_str,
                        attr_len > len ? len : attr_len) != 0) {
        return NULL;
    }

    if (len < attr_len) {
        /* Match for "attr=" */
        cmatch = complete_match_init(NULL, 0);
        if (cmatch) {
            complete_match_unique_str_add(cmatch, attr_str);
            complete_match_append_char_set(cmatch, '\0');
        }
    } else {
        /* Match for the supported attribute strings */
        int num_attrs = 0, idx;
        char *mstr;

        while (attrs[num_attrs].name != NULL) {
            num_attrs++;
        }

        /*
         * Attribute format:
         *     attr=ATTR1,ATTR2,ATTR3...
         * Find the <ATTR> string to be matched
         */
        mstr = sal_strrchr(match_str, ',');
        if (mstr) {
            mstr++;
        } else {
            mstr = match_str + attr_len;
        }

        cmatch = complete_match_init(mstr, num_attrs);
        if (cmatch == NULL) {
            return NULL;
        }

        for (idx = 0; idx < num_attrs; idx++) {
            complete_match_strs_compare_add(cmatch, attrs[idx].name);
        }
        if (num_attrs > 1) {
            complete_match_append_char_set(cmatch, '\0');
        }

        if (complete_match_is_empty(cmatch)) {
            complete_match_cleanup(cmatch);
            return NULL;
        }
    }

    return cmatch;
}

/* Check if a token is attribute. */
static bool
str_is_attribute(const char *str)
{
    return (sal_strncasecmp(str, "attr=", 5) == 0);
}

/* Check if a token is opcode. */
static bool
str_is_opcode(const char *str, const bcma_cli_parse_enum_t *opcodes)
{
    const bcma_cli_parse_enum_t *e = &opcodes[0];

    while (e->name != NULL) {
        if (sal_strcmp(str, e->name) == 0) {
            return true;
        }
        e++;
    }

    return false;
}

/* Check if a token is an option. */
static bool
str_is_option(const char *str)
{
    if (str && str[0] == '-') {
        return true;
    }

    return false;
}

/* Check if a token is likely to be field set. */
static bool
str_is_field_set(char *str, char **name, char **val)
{
    char *ptr = sal_strrchr(str, '=');

    if (ptr == NULL || ptr == str) {
        return false;
    }
    /* Check for contiguous equal signs. */
    if ((ptr - 1) >= str && *(ptr - 1) == '=') {
        return false;
    }

    if (val) {
        /* Check field value from the end. */
        *val = (*(ptr + 1) == '\0') ? NULL : ptr + 1;
    }
    if (name) {
        *ptr = '\0';
        /* Find field name from the beginning. */
        *name = str;
        /* Check for array field syntax. */
        if ((ptr = sal_strchr(str, '[')) != NULL) {
            *ptr = '\0';
        } else if ((ptr = sal_strchr(str, '=')) != NULL) {
            *ptr = '\0';
        }
    }
    return true;
}

/* Check if a token is a valid table name. */
static bool
str_is_table_name(int unit, uint32_t scope, const char *str, const char **name)
{
    int rv = bcma_bcmlt_tables_search(unit, str, BCMA_BCMLT_MATCH_EXACT,
                                      scope, table_match_name_get, name);
    return (rv == 1);
}

/*
 * Find a valid table name token from the argument tokens in a reverse order.
 */
static const char *
table_name_find(int unit, uint32_t scope, const bcma_cli_parse_enum_t *opcodes,
                bcma_cli_tokens_t *ctoks, int last_tok, int *tbl_idx)
{
    int idx;
    const char *name = NULL;

    for (idx = last_tok; idx >=1; idx--) {
        /* Skip option. */
        if (ctoks->argv[idx][0] == '-') {
            continue;
        }
        /* Skip opcode. */
        if (str_is_opcode(ctoks->argv[idx], opcodes)) {
            continue;
        }
        /* Skip field set. */
        if (str_is_field_set(ctoks->argv[idx], NULL, NULL)) {
            continue;
        }
        if (str_is_table_name(unit, scope, ctoks->argv[idx], &name)) {
            if (tbl_idx) {
                *tbl_idx = idx;
            }
            return name;
        }
    }
    return NULL;
}

static complete_match_t *
cmd_complete(bcma_cli_t *cli, bool logical, bcma_cli_tokens_t *ctoks,
             const char *text, int start, int end)
{
    bool space_end;
    uint32_t scope;
    char *ltok = ctoks->argv[ctoks->argc - 1];
    int ltok_len = sal_strlen(ltok);
    int unit = cli->cmd_unit;

    scope = logical ? BCMA_BCMLT_SCOPE_F_LTBL : BCMA_BCMLT_SCOPE_F_PTBL;

    /*
     * If the input text ends in a space character, we will try to guess
     * the next input token. If the input text does not end in a space
     * character, we will try to complete last token.
     *
     * Examples:
     * 1. lt VLAN<tab>
     *    lt VLAN attr=<tab>
     * 2. lt VLAN <tab>
     * In example 1, <tab> is pressed directly after VLAN or attr=.
     * The completion will focus on the current input token.
     * In example 2, <tab> is pressed after VLAN following a space character.
     * The completion will focus on the next input token.
     * To differenciate these completion cases, we need to check whether
     * the input text ends in a space character.
     *
     * Note that '=' and ',' are both word separators in the readline library.
     * The start/end from the completion library will have the same
     * value if the input text is ended in a word separator.
     */
    space_end = (start == end && !sal_strchr("=,", ltok[ltok_len - 1])) ? 1 : 0;

    /* Do nothing for non space-ended option. */
    if (ltok[0] == '-' && !space_end) {
        return NULL;
    }

    if (sal_strcmp(ctoks->argv[1], "list") == 0 ||
        sal_strcmp(ctoks->argv[1], "traverse") == 0) {
        /* Check if we need to suggest the last token for table names. */
        if (ctoks->argc > 2 && !space_end) {
            return tables_complete(unit, scope, ltok);
        }
    } else {
        /*
         * Suggest for entry operation.
         */
        int tbl_idx;
        const char *table_name;
        char *match_substr = space_end ? NULL : ltok;
        int last_idx = space_end ? ctoks->argc - 1 : ctoks->argc - 2;
        bool is_opcode, is_opt, is_field_set;
        const bcma_cli_parse_enum_t *opcodes;

        if (ctoks->argc == 2 && !space_end) {
            return tables_complete(unit, scope, ltok);
        }

        if (logical) {
            opcodes = bcmltcmd_lt_opcodes_get();
        } else {
            opcodes = bcmltcmd_pt_opcodes_get();
        }

        table_name = table_name_find(unit, scope, opcodes,
                                     ctoks, last_idx, &tbl_idx);
        if (table_name == NULL) {
            return NULL;
        }

        if (tbl_idx == last_idx) {
            const bcma_cli_parse_enum_t *attrs = bcmltcmd_ent_attr_enum_get();
            complete_match_t *cmatch = attrbutes_complete(attrs, match_substr);

            if (cmatch == NULL) {
                cmatch = opcodes_complete(opcodes, match_substr);
            }
            return cmatch;
        } else if (tbl_idx == (last_idx - 1) &&
                   str_is_attribute(ctoks->argv[last_idx])) {
            return opcodes_complete(opcodes, match_substr);
        }

        is_opcode = str_is_opcode(ctoks->argv[last_idx], opcodes);
        is_opt = str_is_option(ctoks->argv[last_idx]);
        is_field_set = str_is_field_set(ctoks->argv[last_idx], NULL, NULL);

        if (is_opcode || is_opt || is_field_set) {
            char *fname, *fval;
            complete_match_t *cmatch;

            if (!space_end && str_is_field_set(ltok, &fname, &fval)) {
                return field_values_complete(unit, scope, table_name, fname,
                                             fval, text);
            }
            /* Try to suggest for fields first. */
            cmatch = fields_complete(unit, scope, table_name, match_substr,
                                     text);
            if (cmatch) {
                return cmatch;
            }

            if (is_field_set) {
                /*
                 * Suggest for opcodes as the next entry operation
                 * if no fields are matched.
                 */
                cmatch = opcodes_complete(opcodes, match_substr);
                if (cmatch) {
                    return cmatch;
                }
                /*
                 * Suggestion for tables as the next table operation
                 * if no opcodes are matched.
                 */
                return tables_complete(unit, scope, match_substr);
            }
        }
    }

    return NULL;
}

int
bcmltcmd_complete(bcma_cli_t *cli, bcma_cli_tokens_t *ctoks, bool logical,
                  const char *text, int start, int end,
                  bcma_clirlc_t *complete)
{
    if (cur_cmatch) {
        complete_match_cleanup(cur_cmatch);
        cur_cmatch = NULL;
    }

    /* Do command usage if only command name is there. */
    if (ctoks->argc <= 1) {
        return BCMA_CLI_CMD_USAGE;
    }

    cur_cmatch = cmd_complete(cli, logical, ctoks, text, start, end);
    if (cur_cmatch == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (cur_cmatch->unique_str) {
        complete->entry_func = complete_match_unique_str_entry_func;
    } else {
        complete->entry_func = complete_match_strs_entry_func;
    }
    complete->append_char = cur_cmatch->append_char;

    return BCMA_CLI_CMD_OK;
}

