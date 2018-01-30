/*! \file bcmdrd_sym_field.c
 *
 * BCMDRD symbol field routines.
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

#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_symbols.h>

static int
field_desc_size(uint32_t fdw)
{
    if (BCMDRD_SYM_FIELD_EXT2(fdw)) {
        return 3;
    }
    if (BCMDRD_SYM_FIELD_EXT(fdw)) {
        return 2;
    }
    return 1;
}

static void
field_info_get(uint32_t *fp, bcmdrd_sym_field_info_t *finfo, int *fd_sz,
               const char **fnames)
{
    int width;
    int num_words;

    /* Get size of field encoding */
    num_words = field_desc_size(*fp);

    if (finfo) {
        sal_memset(finfo, 0, sizeof(*finfo));
        switch (num_words) {
        case 3:
            /* Triple Word */
            finfo->fid = BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp);
            finfo->minbit = BCMDRD_SYM_FIELD_EXT2_MIN_GET(*fp);
            width = BCMDRD_SYM_FIELD_EXT2_WIDTH_GET(*fp);
            finfo->view = BCMDRD_SYM_FIELD_EXT2_VIEW_GET(*fp);
            if (BCMDRD_SYM_FIELD_EXT2_ATTR_IS_COUNTER(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_COUNTER;
            }
            if (BCMDRD_SYM_FIELD_EXT2_ATTR_IS_KEY(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_KEY;
            }
            if (BCMDRD_SYM_FIELD_EXT2_ATTR_IS_MASK(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_MASK;
            }
            break;
        case 2:
            /* Double Word */
            finfo->fid = BCMDRD_SYM_FIELD_EXT_ID_GET(*fp);
            finfo->minbit = BCMDRD_SYM_FIELD_EXT_MIN_GET(*fp);
            width = BCMDRD_SYM_FIELD_EXT_WIDTH_GET(*fp);
            finfo->view = -1;
            if (BCMDRD_SYM_FIELD_EXT_ATTR_IS_COUNTER(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_COUNTER;
            }
            if (BCMDRD_SYM_FIELD_EXT_ATTR_IS_KEY(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_KEY;
            }
            if (BCMDRD_SYM_FIELD_EXT_ATTR_IS_MASK(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_MASK;
            }
            break;
        default:
            /* Assume Single Word */
            finfo->fid = BCMDRD_SYM_FIELD_ID_GET(*fp);
            finfo->minbit = BCMDRD_SYM_FIELD_MIN_GET(*fp);
            width = BCMDRD_SYM_FIELD_WIDTH_GET(*fp);
            finfo->view = -1;
            break;
        }

        finfo->maxbit = finfo->minbit + width - 1;
        finfo->name = NULL;

        if (fnames) {
            finfo->name = fnames[finfo->fid];
        }
    }

    if (fd_sz) {
        *fd_sz = num_words;
    }
}

static uint32_t *
field_desc_get(uint32_t *fp, bcmdrd_fid_t fid)
{
    int num_words;

    while (fp) {
        /* Get size of field encoding */
        num_words = field_desc_size(*fp);

        /* Check field ID */
        switch (num_words) {
        case 3:
            /* Triple Word */
            if (fid == BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp)) {
                return fp;
            }
            break;
        case 2:
            /* Double Word */
            if (fid == BCMDRD_SYM_FIELD_EXT_ID_GET(*fp)) {
                return fp;
            }
            break;
        default:
            /* Assume Single Word */
            if (fid == BCMDRD_SYM_FIELD_ID_GET(*fp)) {
                return fp;
            }
            break;
        }

        if (BCMDRD_SYM_FIELD_LAST(*fp)) {
            break;
        }

        /* Move to next field descriptor */
        fp += num_words;
    }

    return NULL;
}

static uint32_t *
field_desc_get_by_name(uint32_t *fp, const char **fnames, const char *fname)
{
    int num_words;
    bcmdrd_fid_t fid;

    while (fp) {
        /* Get size of field encoding */
        num_words = field_desc_size(*fp);

        /* Check field ID */
        switch (num_words) {
        case 3:
            /* Triple Word */
            fid = BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp);
            if (sal_strcasecmp(fname, fnames[fid]) == 0) {
                return fp;
            }
            break;
        case 2:
            /* Double Word */
            fid = BCMDRD_SYM_FIELD_EXT_ID_GET(*fp);
            if (sal_strcasecmp(fname, fnames[fid]) == 0) {
                return fp;
            }
            break;
        default:
            /* Assume Single Word */
            fid = BCMDRD_SYM_FIELD_ID_GET(*fp);
            if (sal_strcasecmp(fname, fnames[fid]) == 0) {
                return fp;
            }
            break;
        }

        if (BCMDRD_SYM_FIELD_LAST(*fp)) {
            break;
        }

        /* Move to next field descriptor */
        fp += num_words;
    }

    return NULL;
}

int
bcmdrd_sym_fid_list_get(const bcmdrd_symbols_t *symbols,
                        bcmdrd_sid_t sid, size_t list_max,
                        bcmdrd_fid_t *fid_list, size_t *num_fid)
{
    const bcmdrd_symbol_t *symbol;
    size_t idx;
    bcmdrd_fid_t fid;
    uint32_t *fp;
    int num_words;

    if (symbols == NULL) {
        return -1;
    }
    if (sid >= symbols->size) {
        return -1;
    }
    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return -1;
    }

    fp = symbol->fields;
    if (fp == NULL) {
        *num_fid = 0;
        return 0;
    }

    idx = 0;

    while (fp) {
        /* Get size of field encoding */
        num_words = field_desc_size(*fp);

        /* Check field ID */
        switch (num_words) {
        case 3:
            /* Triple Word */
            fid = BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp);
            break;
        case 2:
            /* Double Word */
            fid = BCMDRD_SYM_FIELD_EXT_ID_GET(*fp);
            break;
        default:
            /* Assume Single Word */
            fid = BCMDRD_SYM_FIELD_ID_GET(*fp);
            break;
        }
        if (idx < list_max) {
            if (fid_list == NULL) {
                return -1;
            }
            fid_list[idx] = fid;
        }
        idx++;
        if (BCMDRD_SYM_FIELD_LAST(*fp)) {
            break;
        }
        /* Move to next field descriptor */
        fp += num_words;
    }
    *num_fid = idx;

    return 0;
}

const bcmdrd_symbol_t *
bcmdrd_sym_field_info_get(const bcmdrd_symbols_t *symbols,
                          bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                          bcmdrd_sym_field_info_t *finfo)
{
    uint32_t *fp;
    const bcmdrd_symbol_t *symbol;

    if (symbols == NULL) {
        return NULL;
    }
    if (sid >= symbols->size) {
        return NULL;
    }
    symbol = &symbols->symbols[sid];
    fp = field_desc_get(symbol->fields, fid);
    if (fp == NULL) {
        return NULL;
    }
    field_info_get(fp, finfo, NULL, symbols->field_names);
    return symbol;
}

const bcmdrd_symbol_t *
bcmdrd_sym_field_info_get_by_name(const bcmdrd_symbols_t *symbols,
                                  bcmdrd_sid_t sid, const char *fname,
                                  bcmdrd_sym_field_info_t *finfo)
{
    uint32_t *fp;
    const bcmdrd_symbol_t *symbol;

    if (symbols == NULL) {
        return NULL;
    }
    if (sid >= symbols->size) {
        return NULL;
    }
    symbol = &symbols->symbols[sid];
    fp = field_desc_get_by_name(symbol->fields, symbols->field_names, fname);
    if (fp == NULL) {
        return NULL;
    }
    field_info_get(fp, finfo, NULL, symbols->field_names);
    return symbol;
}


uint32_t *
bcmdrd_sym_field_info_decode(uint32_t *fp, const char **fnames,
                             bcmdrd_sym_field_info_t *finfo)
{
    int fd_sz = 1;

    if (!fp) {
        return NULL;
    }

    field_info_get(fp, finfo, &fd_sz, fnames);

    if (BCMDRD_SYM_FIELD_LAST(*fp)) {
        return NULL;
    }

    /* Return pointer to next field descriptor */
    return fp + fd_sz;
}

uint32_t
bcmdrd_sym_field_info_count(uint32_t *fp)
{
    int count = 0;

    while (fp) {
        fp = bcmdrd_sym_field_info_decode(fp, NULL, NULL);
        count++;
    }
    return count;
}
