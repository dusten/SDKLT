/*! \file bcma_bcmlt.c
 *
 * Functions based on bcmlt library.
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
#include <shr/shr_pb_format.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/io/bcma_io_term.h>
#include <bcma/bcmlt/bcma_bcmlt.h>

#define DEFAULT_WINDOW_COLUMNS     80
#define STRS_ARRAY_SIZE_THRESHOLD  128

/*
 * This function checks if search name matches table name (case insensitive).
 * Return value is 1 if matched, otherwise 0.
 */
static int
table_match(const char *tname, const char *sname,
            bcma_bcmlt_match_mode_t match_mode)
{
    const char *str;

    if (tname == NULL || sname == NULL ||
        tname[0] == '\0' || sname[0] == '\0') {
        return 0;
    }

    if (match_mode == BCMA_BCMLT_MATCH_SUBSTR) {

        /* Match for all */
        if (sal_strcmp(sname, "*") == 0) {
            return 1;
        }

        switch (sname[0]) {
        case '^':
            /* Match string from start. */
            str = sname + 1;
            if (sal_strncasecmp(tname, str, sal_strlen(str)) == 0) {
                return 1;
            }
            break;
        case '*':
            str = sname + 1;
            /* Match string if ever occurred. */
            if (sal_strcasestr(tname, str) != NULL) {
                return 1;
            }
            break;
        case '@':
            str = sname + 1;
            /* Match string exactly */
            if (sal_strcasecmp(tname, str) == 0) {
                return 1;
            }
            break;
        default:
            /* Default match is a substring match */
            if (sal_strcasestr(tname, sname) != NULL) {
                return 1;
            }
            break;
        }
    } else {
        /* Exact match */
        if (sal_strcasecmp(tname, sname) == 0) {
            return 1;
        }
    }

    return 0;
}

/*
 * bcma_bcmlt_tables_search callback function.
 * Allocate entry information and set it to zero.
 * Allocate fields array with number of default fields and set it to zero.
 * Make a local copy of the default fields array information to entry.
 */
static int
entry_calloc(void *cookie, const char *name,
             bcmlt_field_def_t *table_fields, uint32_t num_table_fields)
{
    uint32_t idx;
    bcma_bcmlt_entry_info_t **entry_info = (bcma_bcmlt_entry_info_t **)cookie;
    bcma_bcmlt_entry_info_t *ei = NULL;

    *entry_info = NULL;

    ei = sal_alloc(sizeof(*ei), "bcmaLtEntryInfo");
    if (ei == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Fail to allocate entry for table %s\n"), name));
        return 0;
    }
    sal_memset(ei, 0, sizeof(*ei));
    ei->name = name;
    ei->num_def_fields = num_table_fields;

    /* Get number for key fields. */
    for (idx = 0; idx < num_table_fields; idx++) {
        if (table_fields[idx].key) {
            ei->num_key_fields++;
        }
    }

    if (num_table_fields > 0) {
        int arr_size = sizeof(bcmlt_field_def_t) * num_table_fields;

        ei->def_fields = sal_alloc(arr_size, "bcmaLtDefFields");
        if (ei->def_fields == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Fail to allocate %d default fields "
                                "for table %s\n"),
                       (int)num_table_fields, name));
            sal_free(ei);
            return 0;
        }
        sal_memcpy(ei->def_fields, table_fields, arr_size);

        if (bcma_bcmlt_entry_fields_create(ei, num_table_fields) < 0) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Fail to allocate %d fields "
                                "for table %s\n"),
                       (int)num_table_fields, name));
            sal_free(ei->def_fields);
            sal_free(ei);
            return 0;
        }
    }

    *entry_info = ei;

    return 0;
}

/* Free dynamic allocated array field memory */
static void
field_array_free(bcma_bcmlt_field_t *field)
{
    bcma_bcmlt_field_array_t *arr = field->u.array;

    if (arr == NULL) {
        return;
    }
    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        sal_free(arr->u.strs);
    } else {
        sal_free(arr->u.vals);
    }
    if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
        sal_free(arr->elements);
    }
    sal_free(arr);
    field->u.array = NULL;
}

/* Allocate memory for array field in array_size */
static int
field_array_alloc(bcma_bcmlt_field_t *field, int array_size)
{
    bcma_bcmlt_field_array_t *arr;

    arr = sal_alloc(sizeof(bcma_bcmlt_field_array_t), "bcmaLtFieldArray");
    if (arr == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Fail to allocate array field %s\n"),
                   field->name));
        return -1;
    }
    sal_memset(arr, 0, sizeof(*arr));

    if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
        /* Information for scalar array or symbol strings array field */
        arr->elements = sal_alloc(sizeof(bool) * array_size, "bcmaLtArrElem");
        if (arr->elements == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Fail to allocate array field %s info\n"),
                       field->name));
            sal_free(arr);
            return -1;
        }
        sal_memset(arr->elements, 0, sizeof(bool) * array_size);
    }

    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        /* Symbol strings array field */
        arr->u.strs = sal_alloc(sizeof(const char *) * array_size,
                                "bcmaLtArrStrs");
        if (arr->u.strs == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Fail to allocate symbol array field %s\n"),
                       field->name));
            sal_free(arr->elements);
            sal_free(arr);
            return -1;
        }
        sal_memset(arr->u.strs, 0, sizeof(const char *) * array_size);
    } else {
        /* Scalar array field or wide field */
        arr->u.vals = sal_alloc(sizeof(uint64_t) * array_size, "bcmaLtArrVals");
        if (arr->u.vals == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Fail to allocate value array field %s\n"),
                       field->name));
            sal_free(arr->elements);
            sal_free(arr);
            return -1;
        }
        sal_memset(arr->u.vals, 0, sizeof(uint64_t) * array_size);
    }

    arr->size = array_size;

    field->u.array = arr;

    return 0;
}

/*
 * Return formatted string of field access type.
 */
static char *
field_access_str(bcmlt_field_acc_t acc)
{
    switch (acc) {
    case BCMLT_FIELD_ACCESS_READ_WRITE:
        return "R/W";
    case BCMLT_FIELD_ACCESS_READ_ONLY:
        return "R/O";
    default:
        return "N/A";
    }
}

/*
 * Format the strings in array strs in row x column matrix.
 * The columns are calculated dynamically according to the current
 * window width and the maximum string length in the array. If the
 * array size is too large, the calculation will be skipped and the
 * strings will be listed in one column per line.
 */
void
bcma_bcmlt_format_strs_array(shr_pb_t *pb, const char * prefix,
                             const char **strs, int cnt)
{
    int len, maxlen = 0;
    const char *pfix = prefix ? prefix : "";
    int idx, row, rows, col, cols, cw = 0;
    int win_cols, win_rows;

    if (cnt > STRS_ARRAY_SIZE_THRESHOLD) {
        /*
         * Optimization for large array size to avoid the column width
         * calculation. Just print each string per line.
         */
        cols = 1;
    } else {
        /* Determine column count from the maximum string length. */
        for (idx = 0; idx < cnt; idx++) {
            len = sal_strlen(strs[idx]);
            if (len > maxlen) {
                maxlen = len;
            }
        }
        cw = maxlen + 3;
        if (bcma_io_term_winsize_get(&win_cols, &win_rows) < 0) {
            win_cols = DEFAULT_WINDOW_COLUMNS;
        }
        cols = (win_cols - sal_strlen(pfix)) / cw;
        if (cols <= 0) {
            cols = 1;
        }
    }
    rows = (cnt + cols - 1) / cols;

    for (row = 0; row < rows; row++) {
        shr_pb_printf(pb, "%s", pfix);
        for (col = 0; col < cols; col++) {
            idx = row + col * rows;
            if (idx >= cnt) {
                break;
            }
            if (cols == 1) {
                shr_pb_printf(pb, "%s", strs[idx]);
            } else {
                shr_pb_printf(pb, "%-*s", cw, strs[idx]);
            }
        }
        shr_pb_printf(pb, "\n");
    }
}

const char *
bcma_bcmlt_entry_field_default_info_format(shr_pb_t *pb, const char *pf_str,
                                           int unit, const char *tbl_name,
                                           bcmlt_field_def_t *df)
{
    const char *prefix = pf_str ? pf_str : "";

    if (pb == NULL || df == NULL) {
        return "<N/A>\n";
    }

    /* Field width. */
    shr_pb_printf(pb, "%sWidth: %d bit%s\n",
                  prefix, df->width, df->width > 1 ? "s" : "");

    /* Display field default, minimum and maximum values for scalar fields. */
    if (!df->symbol) {
        shr_pb_printf(pb, "%sValue (default, min, max): ", prefix);
        shr_pb_format_uint64(pb, "", &df->def, 1, 0);
        shr_pb_format_uint64(pb, ", ", &df->min, 1, 0);
        shr_pb_format_uint64(pb, ", ", &df->max, 1, 0);
        shr_pb_printf(pb, "\n");
    }

    /* Field attribute: key, symbol, array, wide-field. */
    shr_pb_printf(pb, "%sAttribute: ", prefix);
    shr_pb_printf(pb, "%s", field_access_str(df->access));
    if (df->key) {
        shr_pb_printf(pb, ", key");
    }
    if (df->symbol) {
        shr_pb_printf(pb, ", symbol");
    }
    if (df->depth > 0) {
        shr_pb_printf(pb, ", array[%d]", df->depth);
    }
    if (df->width > 64) {
        shr_pb_printf(pb, ", wide-field[%d]", df->elements);
    }
    shr_pb_printf(pb, "\n");

    /* Get symbol strings of a symbol field. */
    if (df->symbol) {
        const char **sym_strs;
        uint32_t elems;

        shr_pb_printf(pb, "%sSymbol: ", prefix);
        sym_strs = bcma_bcmlt_entry_field_symbol_strs_get(unit, tbl_name,
                                                          df->name, &elems);
        if (sym_strs) {
            shr_pb_printf(pb, "%u\n", elems);
            bcma_bcmlt_format_strs_array(pb, "          ", sym_strs, elems);
            sal_free(sym_strs);
        } else {
            shr_pb_printf(pb, "N/A\n");
        }

        /* The symbol field default value. */
        shr_pb_printf(pb, "%sDefault value: %s\n", prefix, df->sym_def);
    }

    return shr_pb_str(pb);
}

const char **
bcma_bcmlt_entry_field_symbol_strs_get(int unit, const char *tbl_name,
                                       const char *field_name,
                                       uint32_t *elems)
{
    int rv;
    bcmlt_entry_handle_t eh;
    const char **sstrs;
    uint32_t cnt;

    if (tbl_name == NULL || field_name == NULL) {
        return NULL;
    }

    rv = bcmlt_entry_allocate(unit, tbl_name, &eh);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit, "%s: "BCMA_BCMLT_ERRMSG_FMT"\n"),
                   tbl_name, BCMA_BCMLT_ERRMSG(rv)));
        return NULL;
    }
    /* Get symbol strings array size. */
    rv = bcmlt_entry_field_symbol_info_get(eh, field_name, 0, NULL, &cnt);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit, "%s-%s: "BCMA_BCMLT_ERRMSG_FMT"\n"),
                   tbl_name, field_name, BCMA_BCMLT_ERRMSG(rv)));
        bcmlt_entry_free(eh);
        return NULL;
    }
    sstrs = sal_alloc(sizeof(const char *) * cnt, "bcmaBcmltFieldSymStrs");
    if (sstrs == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit, "Fail to allocate symbol strings\n")));
        bcmlt_entry_free(eh);
        return NULL;
    }
    /* Get symbol strings array. */
    rv = bcmlt_entry_field_symbol_info_get(eh, field_name, cnt, sstrs, &cnt);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit, "%s-%s: "BCMA_BCMLT_ERRMSG_FMT"\n"),
                   tbl_name, field_name, BCMA_BCMLT_ERRMSG(rv)));
        sal_free(sstrs);
        bcmlt_entry_free(eh);
        return NULL;
    }
    bcmlt_entry_free(eh);
    *elems = cnt;
    return sstrs;
}

int
bcma_bcmlt_entry_field_get(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field)
{
    int rv;

    if (!ei || !ei->eh || !field) {
        return -1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;
        uint32_t idx, sz = 0;

        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            rv = bcmlt_entry_field_array_symbol_get(ei->eh, field->name, 0,
                                                    arr->u.strs, arr->size,
                                                    &sz);
        } else {
            rv = bcmlt_entry_field_array_get(ei->eh, field->name, 0,
                                             arr->u.vals, arr->size, &sz);
        }
        /*
         * Mark all array elements as valid for scalar array and
         * symbol strings array.
         */
        if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            for (idx = 0; idx < sz; idx++) {
                arr->elements[idx] = true;
            }
        }
    } else {
        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            rv = bcmlt_entry_field_symbol_get(ei->eh, field->name,
                                              &field->u.str);
        } else {
            rv = bcmlt_entry_field_get(ei->eh, field->name, &field->u.val);
        }
    }

    if (SHR_FAILURE(rv)) {
        cli_out("%sFail to get field %s value: "BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, field->name, BCMA_BCMLT_ERRMSG(rv));
        return -1;
    }

    return 0;
}

int
bcma_bcmlt_entry_field_add(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field)
{
    int rv = 0;

    if (!ei || !ei->eh || !field) {
        return -1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;

        /* Sanity check */
        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE &&
            field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Scalar array and wide field can not be "
                                "suppported in a single field.\n")));
            return -1;
        }

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            /* Wide field */
            rv = bcmlt_entry_field_array_add(ei->eh, field->name, 0,
                                             arr->u.vals, arr->size);
        } else {
            /* Scalar array or symbol strings array */
            int idx, sidx, cnt;

            /* Add all valid elements of array field to entry. */
            for (idx = 0; idx < arr->size; idx++) {
                if (!arr->elements[idx]) {
                    continue;
                }

                /*
                 * Get a continuous range set of valid elements
                 * in the array.
                 */
                sidx = idx;
                do {
                    idx++;
                } while (idx < arr->size && arr->elements[idx]);
                cnt = idx - sidx;

                /* Set the range of field array data to entry */
                if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
                    rv = bcmlt_entry_field_array_symbol_add(ei->eh,
                                                            field->name, sidx,
                                                            &arr->u.strs[sidx],
                                                            cnt);
                } else {
                    rv = bcmlt_entry_field_array_add(ei->eh,
                                                     field->name, sidx,
                                                     &arr->u.vals[sidx],
                                                     cnt);
                }

                if (SHR_FAILURE(rv)) {
                    cli_out("%sFail to add array field %s value: "
                            BCMA_BCMLT_ERRMSG_FMT"\n",
                            BCMA_CLI_CONFIG_ERROR_STR, field->name,
                            BCMA_BCMLT_ERRMSG(rv));
                    return -1;
                }
            }
        }
    } else {
        /* Non-array field */
        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            rv = bcmlt_entry_field_symbol_add(ei->eh, field->name,
                                              field->u.str);
        } else {
            rv = bcmlt_entry_field_add(ei->eh, field->name, field->u.val);
        }
    }

    if (SHR_FAILURE(rv)) {
        cli_out("%sFail to add field %s value: "BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, field->name, BCMA_BCMLT_ERRMSG(rv));
        return -1;
    }

    return 0;

}

int
bcma_bcmlt_entry_field_default_val_set(bcma_bcmlt_entry_info_t *ei,
                                       bcmlt_field_def_t *df,
                                       bcma_bcmlt_field_t *field)
{
    if (!ei || !field || !df) {
        return -1;
    }

    if (bcma_bcmlt_entry_field_init(field, df) != 0) {
        return -1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;
        int idx;

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            arr->u.vals[0] = df->def;
        } else if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            /* Set default value to each element in the scalar array. */
            for (idx = 0; idx < arr->size; idx++) {
                arr->u.strs[idx] = df->sym_def;
                arr->elements[idx] = true;
            }
        } else {
            /* Set default value to each element in the scalar array. */
            for (idx = 0; idx < arr->size; idx++) {
                arr->u.vals[idx] = df->def;
                arr->elements[idx] = true;
            }
        }
    } else if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        field->u.str = df->sym_def;
    } else {
        field->u.val = df->def;
    }

    return 0;
}

int
bcma_bcmlt_entry_field_parse(bcma_bcmlt_entry_info_t *ei,
                             bcma_bcmlt_field_t *field, char *field_arg)
{
    int idx;

    if (!ei || !field || !field_arg) {
        return -1;
    }

    /* Validate field name */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcmlt_field_def_t *df = &ei->def_fields[idx];
        int len = sal_strlen(df->name);

        if (sal_strncasecmp(field_arg, df->name, len) == 0) {
            /*
             * Might be one of the valid field assignment:
             * field=val
             * field[r]=v,...
             */
            if (field_arg[len] == '=' ||
                (df->depth > 0 && field_arg[len] == '[')) {
                if (bcma_bcmlt_entry_field_init(field, df) != 0) {
                    return -1;
                }
                break;
            }
        }
    }

    /* Return error if can not find a matching default field name */
    if (idx == ei->num_def_fields) {
        char *ptr = sal_strchr(field_arg, '=');

        if (ptr) {
            *ptr = '\0';
        }
        cli_out("%sField %s is not valid for table %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, field_arg, ei->name);
        return -1;
    }

    /* Parse and validate field data */
    if (bcma_bcmlt_field_parse(field_arg, field) < 0) {
        cli_out("%sFail to parse field %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, field_arg);
        return -1;
    }

    return 0;
}

int
bcma_bcmlt_entry_list(bcma_bcmlt_entry_info_t *ei, const char *prefix,
                      bool key_only, uint32_t disp_word)
{
    int idx, rv = 0;
    int fcnt = 0;
    shr_pb_t *pb;
    char *key_prefix = "BCMLTM_PT_";
    int len = sal_strlen(key_prefix);

    if (!ei) {
        return -1;
    }

    /* Get all fields values of a table entry. */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcmlt_field_def_t *def_field = &ei->def_fields[idx];
        bcma_bcmlt_field_t *field = &ei->fields[fcnt];

        /* Skip PT key fields that start with "BCMLTM_PT_" */
        if (!ei->logical && def_field->key &&
            sal_strncmp(def_field->name, key_prefix, len) == 0) {
            continue;
        }

        /* List for key fields only. */
        if (key_only && !def_field->key) {
            continue;
        }

        if (bcma_bcmlt_entry_field_init(field, def_field) != 0) {
            return -1;
        }
        if (SHR_FAILURE(bcma_bcmlt_entry_field_get(ei, field))) {
            rv = -1;
        }
        fcnt++;
    }
    ei->num_fields = fcnt;

    if (rv != 0) {
        return rv;
    }

    /*
     * List all fields values in a table.
     * if all fields values are retrieved successfully.
     */
    pb = shr_pb_create();
    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        /* List for key fields only. */
        if (key_only && !(field->flags & BCMA_BCMLT_FIELD_F_KEY)) {
            continue;
        }
        if (prefix != NULL) {
            shr_pb_printf(pb, "%s", prefix);
        }
        bcma_bcmlt_field_format(pb, field, disp_word);
        shr_pb_printf(pb, "\n");
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return 0;
}

int
bcma_bcmlt_entry_field_init(bcma_bcmlt_field_t *field,
                            bcmlt_field_def_t *def_field)
{
    /* Free previously used field data (if any) */
    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        field_array_free(field);
    }

    sal_memset(field, 0, sizeof(*field));

    field->name = def_field->name;
    field->width = def_field->width;

    /* Set field flags by default field attributes */
    if (def_field->key) {
        field->flags |= BCMA_BCMLT_FIELD_F_KEY;
    }
    if (def_field->symbol) {
        field->flags |= BCMA_BCMLT_FIELD_F_SYMBOL;
    }
    if (def_field->depth > 0) {
        field->flags |= BCMA_BCMLT_FIELD_F_ARRAY;
    }
    if (def_field->width > 64) {
        field->flags |= BCMA_BCMLT_FIELD_F_WIDE;
    }

    /* Allocate memory for array field */
    if (def_field->elements > 1) {
        /* Sanity check */
        if (!BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("Invalid attribute "
                                "(w:%"PRIu32" d:%"PRIu32" e:%"PRIu32")"
                                " of field %s\n"),
                       def_field->width, def_field->depth, def_field->elements,
                       field->name));
            return -1;
        }
        return field_array_alloc(field, def_field->elements);
    }

    return 0;
}

int
bcma_bcmlt_entry_fields_create(bcma_bcmlt_entry_info_t *ei,
                               uint32_t num_fields)
{
    bcma_bcmlt_field_t *fields = NULL;
    int szfields = num_fields;

    if (ei == NULL) {
        return -1;
    }
    /* Always create the array size at least the default fields count. */
    if (szfields < ei->num_def_fields) {
        return -1;
    }

    /*
     * Key fields might be added internally for auto-fill key fields option.
     * Enlarge the array size with key fields number for this feature.
     */
    szfields += ei->num_key_fields;

    fields = sal_alloc(sizeof(bcma_bcmlt_field_t) * szfields, "bcmaLtFields");
    if (fields == NULL) {
        return -1;
    }
    sal_memset(fields, 0, sizeof(bcma_bcmlt_field_t) * szfields);

    if (ei->fields) {
        bcma_bcmlt_entry_fields_destroy(ei);
    }
    ei->fields = fields;
    ei->szfields = szfields;
    /* Clear the in-use fields count. */
    ei->num_fields = 0;

    return 0;
}

int
bcma_bcmlt_entry_fields_destroy(bcma_bcmlt_entry_info_t *ei)
{
    int idx;

    if (ei == NULL || ei->fields == NULL) {
        return -1;
    }

    for (idx = 0; idx < ei->szfields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
            field_array_free(field);
        }
    }
    sal_free(ei->fields);
    ei->fields = NULL;
    ei->szfields = 0;
    ei->num_fields = 0;

    return 0;
}

bcma_bcmlt_entry_info_t *
bcma_bcmlt_entry_info_create(int unit, const char *table_name,
                             bool logical, uint32_t attr,
                             bcmlt_field_def_t *fields, uint32_t num_fields)
{
    int rv;
    bcma_bcmlt_entry_info_t *ei = NULL;
    uint32_t search_scope = logical ? BCMA_BCMLT_SCOPE_F_LTBL:
                                      BCMA_BCMLT_SCOPE_F_PTBL;

    if (table_name == NULL) {
        return NULL;
    }

    if (fields == NULL) {
        rv = bcma_bcmlt_tables_search(unit, table_name, BCMA_BCMLT_MATCH_EXACT,
                                      search_scope, entry_calloc, &ei);
        if (rv <= 0) {
            cli_out("%sUnsupported %s table: %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    logical ? "logical" : "physical", table_name);
            return NULL;
        }
    } else {
        entry_calloc(&ei, table_name, fields, num_fields);
    }

    if (!ei) {
        return NULL;
    }

    rv = bcmlt_entry_allocate(unit, ei->name, &ei->eh);
    if (SHR_FAILURE(rv)) {
        bcma_bcmlt_entry_info_destroy(ei);
        return NULL;
    }

    ei->unit = unit;
    ei->logical = logical;
    ei->attr = attr;
    ei->num_commit_fields = -1;

    return ei;
}

int
bcma_bcmlt_entry_info_destroy(bcma_bcmlt_entry_info_t *ei)
{
    if (!ei) {
        return -1;
    }

    if (ei->eh) {
        bcmlt_entry_free(ei->eh);
    }

    if (ei->fields) {
        bcma_bcmlt_entry_fields_destroy(ei);
    }

    if (ei->def_fields) {
        sal_free(ei->def_fields);
    }

    sal_free(ei);

    return 0;
}

int
bcma_bcmlt_table_traverse(int unit, const char *lt_name,
                          bcmlt_field_def_t *fields, uint32_t num_fields,
                          bcma_bcmlt_traverse_f cb, void *cookie)
{
    int rv;
    int cnt = 0;
    uint32_t fcnt = 0;
    bcma_bcmlt_entry_info_t *ei;
    bcmlt_entry_info_t e_info;

    /*
     * Use a new entry to make sure no fields are added
     * to the entry for traverse.
     */
    ei = bcma_bcmlt_entry_info_create(unit, lt_name, 1, 0, fields, num_fields);
    if (ei == NULL) {
        cli_out("%sFail to init table %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, lt_name);
        return SHR_E_FAIL;
    }

    while ((rv = bcmlt_entry_commit(ei->eh,
                                    BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        /* Verify that the transaction was successful. */
        rv = bcmlt_entry_info_get(ei->eh, &e_info);
        if (SHR_SUCCESS(rv)) {
            rv = e_info.status;
        }
        if (SHR_FAILURE(rv)) {
            break;
        }
        if (cb) {
            if (cb(cookie, ei, cnt) < 0) {
                /* Stop traversing on error. */
                break;
            }
        }
        cnt++;

        /*
         * Calling to traverse with an empty entry (entry contains no fields)
         * implies to traverse from get_first. Need to ensure the entry is
         * not empty when calling to traverse get_next.
         */
        rv = bcmlt_entry_field_count(ei->eh, &fcnt);
        if (SHR_SUCCESS(rv) && fcnt == 0) {
            cli_out("%sEntry contains no fields while traversing to "
                    "get_next.\n", BCMA_CLI_CONFIG_ERROR_STR);
            break;
        }
    }

    bcma_bcmlt_entry_info_destroy(ei);

    return (cnt > 0) ? cnt : rv;
}

int
bcma_bcmlt_tables_search(int unit, const char *search_name,
                         bcma_bcmlt_match_mode_t match_mode,
                         uint32_t search_scope,
                         bcma_bcmlt_table_info_f cb, void *cookie)
{
    int rv;
    char *table_name;
    bcmlt_field_def_t *fields_def = NULL;
    uint32_t chunk_size = 64, max_size = 256;
    uint32_t actual_num_of_fields = 0;
    int match = 0;
    uint32_t flags = 0;

    fields_def = sal_alloc(max_size * sizeof(bcmlt_field_def_t),
                           "bcmaLtDefFields");
    if (fields_def == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit, "Fail to allocate default fields\n")));
        return 0;
    }

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
        if (table_match(table_name, search_name, match_mode)) {

            if (match > 0 && match_mode == BCMA_BCMLT_MATCH_EXACT) {
                /* Optimize for exact match */
                continue;
            }

            match++;

            /* Get default fields information of the matched table. */
            rv = bcmlt_table_field_defs_get(unit, table_name,
                                            max_size, fields_def,
                                            &actual_num_of_fields);
            if (SHR_FAILURE(rv)) {
                continue;
            }

            if (actual_num_of_fields > max_size) {
                bcmlt_field_def_t *nfd;
                size_t ns = max_size;

                do {
                    ns += chunk_size;
                } while(actual_num_of_fields > ns);
                nfd = sal_alloc(ns * sizeof(bcmlt_field_def_t),
                                "bcmaLtDefFields");
                if (nfd == NULL) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "Fail to allocate %d default fields"
                                          "information of table %s.\n"),
                               (int)actual_num_of_fields, table_name));
                    continue;
                }

                /* Update default fields pointer */
                sal_free(fields_def);
                fields_def = nfd;
                max_size = ns;


                /* Try again with the expended default fields space. */
                rv = bcmlt_table_field_defs_get(unit, table_name,
                                                max_size, fields_def,
                                                &actual_num_of_fields);
                if (SHR_FAILURE(rv)) {
                    continue;
                }
            }

            if (cb) {
                cb(cookie, table_name, fields_def, actual_num_of_fields);
            }
        }
        rv = bcmlt_table_names_get_next(unit, flags, &table_name);
    }

    sal_free(fields_def);

    return match;
}
