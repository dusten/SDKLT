/*! \file bcma_bcmlt_field_parse.c
 *
 * Field parse and format functions.
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

#include <shr/shr_bitop.h>
#include <shr/shr_pb_format.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_token.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/*
 * Remove a specified character from the given string.
 */
static void
str_rm_chr(char *src, char c)
{
    if (src) {
        char *dst = src;

        do {
            if (*src != c) {
                *dst++ = *src;
            }
        } while (*src++ != '\0');
    }
}

/*
 * Convert the input field symbol string to upper case except for
 * field PT_ID which is ended in lower case 'm' or 'r'.
 */
static void
field_symbol_string_check(const char *field, char **field_val)
{
    char *str = *field_val;

    /* Take the string as-is the symbol string which is case ignored. */
    sal_strupr(str);

    /*
     * Exceptions:
     *   Field PT_ID is ended in 'm' or 'r'.
     *   Field PT_ID_DATA is "INVALID_PT" or ended in 'm' or 'r' as PT_ID.
     */
    if (sal_strcmp(field, "PT_ID") == 0 ||
        sal_strcmp(field, "PT_ID_DATA") == 0) {
        int idx = sal_strlen(str) - 1;
        char c = str[idx];

        if (c == 'M' || c == 'R') {
            str[idx] = sal_tolower(c);
        }
    }
}

/*
 * Parse the input string (if specified in str[a] or str[a-b]) index,
 * and return in (str, a, b)
 */
static int
array_index_parse(char *str, int *sidx, int *eidx)
{
    char *ptr, *cptr;
    char ch;
    int idx = -1, start = -1, end = -1;

    *sidx = -1;
    *eidx = -1;

    /* Parse the string index specified in [a] or [a-b] */
    ptr = sal_strchr(str, '[');
    if (ptr == NULL) {
        return -1;
    }
    cptr = ptr + 1;
    do {
        ch = *cptr++;
        if (ch >= '0' && ch <= '9') {
            if (idx == -1) {
                idx = 0;
            }
            idx = (idx * 10) + (ch - '0');
        } else if (ch == '-') {
            if (idx == -1) {
                return -1;
            }
            start = idx;
            idx = -1;
        } else if (ch == ']') {
            if (idx == -1) {
                return -1;
            }
            if (start == -1) {
                start = idx;
            }
            if (end != -1) {
                return -1;
            }
            end = idx;
        } else if (ch != 0) {
            return -1;
        }
    } while (ch != 0);

    if (idx == -1 || start == -1 || end == -1) {
        return -1;
    }

    *ptr = '\0';
    *sidx = start;
    *eidx = end;

    return 0;
}

/*
 * Field array assignment string field_arg should be in format of:
 * <field>[<r1>]=<v2>{,<field>[<r2>]=<v2>,...}, where ri = <i> | <i1>-<i2>
 */
static int
field_array_parse(char *field_arg, const char *name,
                  bcma_bcmlt_field_array_t *arr, int is_str)
{
    int rv = 0;
    int len;
    int idx = -1, start = -1, end = -1;
    char *tok, *s = NULL;

    if (arr == NULL) {
        return -1;
    }
    if ((is_str && arr->u.strs == NULL) || (!is_str && arr->u.vals == NULL)) {
        return -1;
    }

    len = sal_strlen(name);

    /*
     * Tokenize <field>[<r1>]=<v2>,<field>[<r2>]=<v2>,... to
     * <field>[r]=<v>
     */
    tok = sal_strtok_r(field_arg, ",", &s);
    while (tok) {
        /* Parse <field>[r]=<v> */
        char *s1, *s2;
        char *p = sal_strchr(tok, '=');
        char ch;

        if (p == NULL) {
            rv = -1;
            break;
        }
        s1 = tok;
        s2 = p + 1;
        *p = '\0';
        if (*s1 == '\0' || *s2 == '\0') {
            rv = -1;
            break;
        }
        /* Check the field name for each token */
        if (sal_strncasecmp(s1, name, len) != 0) {
            rv = -1;
            break;
        }
        ch = s1[len];
        if (ch != '[') {
            rv = -1;
            break;
        }
        /* Parse field array index range in <field>[r] */
        if (array_index_parse(s1, &start, &end) < 0) {
            rv = -1;
            break;
        }
        /* Check the parse result of the range of array indices */
        if (start >= arr->size || end >= arr->size || start > end) {
            rv = -1;
            break;
        }
        /* Set parsed result to elements of array field */
        for (idx = start; idx <= end; idx++) {
            arr->elements[idx] = true;
            if (is_str) {
                field_symbol_string_check(name, &s2);
                arr->u.strs[idx] = s2;
            } else {
                /* Remove underscore in s2 if any. */
                str_rm_chr(s2, '_');
                if (bcma_cli_parse_uint64(s2, &arr->u.vals[idx]) < 0) {
                    rv = -1;
                    break;
                }
            }
        }

        tok = sal_strtok_r(NULL, ",", &s);
    }

    return rv;
}

static const char *
field_value_format(shr_pb_t *pb, uint64_t val,
                   bcma_bcmlt_display_mode_t disp_mode)
{
    if (disp_mode == BCMA_BCMLT_DISP_MODE_DEC) {
        if (val <= 0xffffffff) {
            /* Display decimal for single-word scalar */
            shr_pb_printf(pb, "%"PRIu64, val);
        } else {
            shr_pb_format_uint64(pb, NULL, &val, 1, 0);
        }
    } else {
        shr_pb_format_uint64(pb, NULL, &val, 1, 0);
        if (disp_mode == BCMA_BCMLT_DISP_MODE_HEX_DEC) {
            if (val >= 10 && val <= 0xffffffff) {
                /* Display both decimal and hexadecimal for single-word scalar */
                shr_pb_printf(pb, "(%"PRIu64")", val);
            }
        }
    }
    return shr_pb_str(pb);
}

/*
 * Format strings in field array structure to make the indices with the same
 * symbol string or scalar value in the same group to the print buffer.
 * An example of the format is field[a,b,c-d]=<SYM_STR> or field[a-b,c,d]=<val>
 */
static const char *
field_array_format(shr_pb_t *pb, const char *name,
                   bcma_bcmlt_field_array_t *arr, int is_str,
                   bcma_bcmlt_display_mode_t disp_mode)
{
    int idx, sidx = -1;

    /*
     * Format the valid elements in an array field to be:
     * field[r1]=v1,field[r2]=v2,...,where r1 = <idx> | <idx1>-<idx2>
     */
    for (idx = 0; idx < arr->size; idx++) {
        if (!arr->elements[idx]) {
            continue;
        }
        if (sidx != -1) {
            shr_pb_printf(pb, ",");
        }
        sidx = idx;
        /* Get a continuous range of array elements with the same value. */
        while (++idx < arr->size && arr->elements[idx]) {
            if (is_str) {
                if (arr->u.strs[sidx] != arr->u.strs[idx]) {
                    break;
                }
            } else {
                if (arr->u.vals[sidx] != arr->u.vals[idx]) {
                    break;
                }
            }
        }
        idx--;
        /* Format field name and indices range */
        if (sidx == idx) {
            shr_pb_printf(pb, "%s[%d]=", name, sidx);
        } else {
            shr_pb_printf(pb, "%s[%d-%d]=", name, sidx, idx);
        }
        /* Format field value */
        if (is_str) {
            shr_pb_printf(pb, "%s", arr->u.strs[sidx]);
        } else {
            field_value_format(pb, arr->u.vals[sidx], disp_mode);
        }
    }
    return shr_pb_str(pb);
}

/*
 * Parse a hex format string starting with 0x into uint64 array
 */
static int
field_parse_uint64_array(const char *str, uint64_t *fval, int size)
{
    int cnt = 0;
    char *p, *s;

    /* Ignore the leading 0x */
    s = sal_strdup(str + 2);
    if (s == NULL) {
        return 0;
    }

    p = s + (sal_strlen(s) - 16);
    if (s > p) {
        p = s;
    }
    while (p >= s) {
        fval[cnt++] = (uint64_t)sal_strtoull(p, NULL, 16);
        if (p == s || cnt == size) {
            break;
        }
        *p = '\0';
        p = (p < s + 16) ? s : p - 16;
    }

    /* Check if the remaining digits are all zero's */
    while (p > s) {
        if (*--p != '0') {
            /*
             * Return failure for the field value width
             * is greater than 64-bit * array size.
             */
            cnt = 0;
            break;
        }
    }

    sal_free(s);

    return cnt;
}

/*
 * Evalute the field_value width in bits. Only support in HEX format.
 */
static int
field_value_width_get(char *field_val)
{
    if (field_val[0] =='0' &&
        (field_val[1] == 'x' || field_val[1] == 'X')) {
        char *ptr = field_val + 2;

        while (*ptr == '0') {
            ptr++;
        }
        if (*ptr) {
            return 4 * sal_strlen(ptr);
        }
        return 0;
    }

    return -1;
}

int
bcma_bcmlt_field_parse(char *field_arg, bcma_bcmlt_field_t *field)
{
    char *field_val = field_arg;

    if (!field_arg || !field) {
        return -1;
    }

    if ((field->flags & BCMA_BCMLT_FIELD_F_ARRAY) == 0) {
        /* field_arg is in format of <field_name>=<field_val> */
        field_val = field_arg + sal_strlen(field->name) + 1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {

            /* Remove underscore in field_val if any. */
            str_rm_chr(field_val, '_');

            /* Parse wide field */
            if (!bcma_cli_parse_is_int(field_val)) {
                return -1;
            }
            /* Check for long hex string ( > 64-bit) for wide field */
            if (field_value_width_get(field_val) > 64) {
                int nums;

                nums = field_parse_uint64_array(field_val,
                                                arr->u.vals, arr->size);
                return (nums == 0) ? -1 : 0;
            }
            /* Use uint64 parser by default */
            if (bcma_cli_parse_uint64(field_val, &arr->u.vals[0]) < 0) {
                return -1;
            }
            return 0;
        } else {
            /* Parse scalar array or symbol strings array */
            int is_str = (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) ? 1 : 0;
            return field_array_parse(field_val, field->name, arr, is_str);
        }
    }

    /* Non-array field */

    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        field_symbol_string_check(field->name, &field_val);
        field->u.str = field_val;
        return 0;
    }

    /* Remove underscore in field_val if any. */
    str_rm_chr(field_val, '_');

    /* Return error if the value for uint64 parser is greater than 64-bit. */
    if (field_value_width_get(field_val) > 64) {
        return -1;
    }

    /* Use uint64 parser by default */
    if (bcma_cli_parse_uint64(field_val, &field->u.val) < 0) {
        return -1;
    }

    return 0;
}

const char *
bcma_bcmlt_field_format(shr_pb_t *pb, bcma_bcmlt_field_t *field,
                        uint32_t disp_word)
{
    bcma_bcmlt_display_mode_t disp_mode;

    if (!field) {
        return "N/A";
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;

        disp_mode = BCMA_BCMLT_ARRAY_FIELD_DISP_MODE_GET(disp_word);

        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            /* Symbol array field */
            return field_array_format(pb, field->name, arr, 1, disp_mode);
        } else if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            /* Scalar array field */
            return field_array_format(pb, field->name, arr, 0, disp_mode);
        } else {
            /* Wide field */
            shr_pb_printf(pb, "%s=", field->name);
            return shr_pb_format_uint64(pb, NULL, arr->u.vals, arr->size,
                                        field->width);
        }
    }

    /* Non-array field */
    shr_pb_printf(pb, "%s=", field->name);
    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        shr_pb_printf(pb, "%s", field->u.str);
        return shr_pb_str(pb);
    }
    disp_mode = BCMA_BCMLT_FIELD_DISP_MODE_GET(disp_word);
    return field_value_format(pb, field->u.val, disp_mode);
}
