/*! \file bcmdrd_sym_dump_fields.c
 *
 * Dump symbol field information into a string.
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

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_sym_dump.h>

#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
/*!
 * \brief Print bit range string.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] size Size of ou
 * \param [in] minbit First bit in bit range.
 * \param [in] maxbit Last bit in bit range.
 *
 * \retval 0 No errors
 */
static int
show_bit_range(shr_pb_t *pb, int minbit, int maxbit)
{
    if (minbit == maxbit) {
        shr_pb_printf(pb, "<%d>", minbit);
    } else {
        shr_pb_printf(pb, "<%d:%d>", maxbit, minbit);
    }
    return 0;
}

/*!
 * \brief Compare two strings from qsort.
 *
 * Note that the input parameters are not strings, but pointers to
 * strings.
 *
 * \param [in] e1 Pointer to first string.
 * \param [in] e2 Pointer to second string.
 *
 * \return Same as standard strcmp function.
 */
static int
qsort_strcmp(const void *e1, const void *e2)
{
    const char *s1 = *(const char **)e1;
    const char *s2 = *(const char **)e2;

    return sal_strcmp(s1, s2);
}

/*!
 * \brief Sort field string.
 *
 * Create a copy of the field string and split it into separate
 * strings for each field. Sort the string array using qsort and write
 * the sorted list back to the print buffer.
 *
 * The last field in the fields string is expected to be followed by a
 * delimiter as well.
 *
 * In case of errors, the fields string will remain unchanged.
 *
 * \param [in] pb Print buffer containing unsorted fields string.
 * \param [in] start Offset of the fields string in the print buffer.
 * \param [in] fcnt Number of fields in the fields string.
 * \param [in] delimiter Delimiter between each field.
 *
 * \retval 0 No errors.
 */
static int
sort_fields(shr_pb_t *pb, int start, int fcnt, char delimiter)
{
    int idx;
    char *ptr, *dstr;
    const char *fstr;
    const char **str_array;

    if (fcnt <= 0) {
        /* Nothing to sort */
        return 0;
    }

    /* Create a copy of the fields string in the print buffer */
    fstr = shr_pb_str(pb);
    dstr = sal_strdup(&fstr[start]);
    if (dstr) {
        /* Split fields string into separate strings for each field */
        str_array = sal_alloc(fcnt * sizeof(char *), "bcmdrdDumpFld");
        if (str_array) {
            fstr = dstr;
            for (idx = 0; idx < fcnt; idx++) {
                str_array[idx] = fstr;
                ptr = sal_strchr(fstr, delimiter);
                if (ptr) {
                    *ptr = '\0';
                    fstr = ptr + 1;
                }
            }
            /* Sort strng array */
            sal_qsort(str_array, fcnt, sizeof(char *), qsort_strcmp);
            /* Rewind print buffer and write back sprted field strings */
            shr_pb_mark(pb, start);
            shr_pb_reset(pb);
            for (idx = 0; idx < fcnt; idx++) {
                shr_pb_printf(pb, "%s%c", str_array[idx], delimiter);
            }
            sal_free(str_array);
        }
        sal_free(dstr);
    }
    return 0;
}
#endif

int
bcmdrd_sym_dump_fields(shr_pb_t *pb, const bcmdrd_symbol_t *symbol,
                       const char **fnames, uint32_t *data, uint32_t flags,
                       bcmdrd_symbol_filter_cb_f fcb, void *cookie)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 0
    return 0;
#else
    int mval;
    int start;
    char *indent;
    char delimiter;
    bcmdrd_sym_field_info_t finfo;
    uint32_t val[BCMDRD_MAX_PT_WSIZE];
    int idx = 0;
    int fcnt = 0;
    int size;

    /* Save starting point */
    start = shr_pb_mark(pb, -1);

    /* Indentation before each field */
    indent = (flags & BCMDRD_SYM_DUMP_SINGLE_LINE) ? "" : "\t";

    /* Delimiter between each field */
    delimiter = (flags & BCMDRD_SYM_DUMP_SINGLE_LINE) ? ',' : '\n';

    BCMDRD_SYM_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        /* Create a reset point for skipping zeros */
        shr_pb_mark(pb, -1);

        /* Use real field name when available */
        if (finfo.name) {
            if (fcb && finfo.view >= 0) {
                if (fcb(symbol, fnames, fnames[finfo.view], cookie) != 0) {
                    continue;
                }
            }
            shr_pb_printf(pb, "%s%s", indent, finfo.name);
        } else {
            shr_pb_printf(pb, "%sfield%d", indent, idx++);
        }

        /* Create bit range string */
        if (flags & BCMDRD_SYM_DUMP_BIT_RANGE) {
            show_bit_range(pb, finfo.minbit, finfo.maxbit);
        }

        if (data) {
            /* Get normalized field value */
            sal_memset(val, 0, sizeof(val));
            if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
                size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
                bcmdrd_field_be_get(data, BCMDRD_BYTES2WORDS(size),
                                    finfo.minbit, finfo.maxbit, val);
            } else {
                bcmdrd_field_get(data, finfo.minbit, finfo.maxbit, val);
            }

            /* Create field value string */
            mval = COUNTOF(val) - 1;
            while (mval && val[mval] == 0) {
                mval--;
            }

            /* Optionally skip fields with all zeros */
            if (flags & BCMDRD_SYM_DUMP_SKIP_ZEROS) {
                if (mval == 0 && val[mval] == 0) {
                    shr_pb_reset(pb);
                    continue;
                }
            }

            if (mval == 0 && val[mval] < 10) {
                /* Values less thans 10 in decimal only */
                shr_pb_printf(pb, "=%"PRIu32, val[0]);
            } else if (mval == 0 && (flags & BCMDRD_SYM_DUMP_EXT_HEX)) {
                /* Single-word values in both decimal and hexadecimal */
                shr_pb_printf(pb, "=0x%"PRIx32"(%"PRIu32")", val[0], val[0]);
            } else {
                /* Anything else is shown as a hex-string */
                shr_pb_printf(pb, "=0x%"PRIx32"", val[mval]);
                while (mval--) {
                    shr_pb_printf(pb, "_%08"PRIx32"", val[mval]);
                }
            }
        }

        /* Add field delimiter */
        shr_pb_printf(pb, "%c", delimiter);

        fcnt++;

    } BCMDRD_SYM_FIELDS_ITER_END();

    if (flags & BCMDRD_SYM_DUMP_ALPHA_SORT) {
        sort_fields(pb, start, fcnt, delimiter);
    }

    if (flags & BCMDRD_SYM_DUMP_SINGLE_LINE) {
        if (fcnt) {
            /* Remove last delimiter */
            shr_pb_mark(pb, shr_pb_mark(pb, -1) - 1);
            shr_pb_reset(pb);
        }
        shr_pb_printf(pb, "\n");
    }

    return 0;
#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */
}
