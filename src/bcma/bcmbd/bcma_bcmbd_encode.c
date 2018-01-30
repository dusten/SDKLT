/*! \file bcma_bcmbd_encode.c
 *
 * Encode field values for register/memory modification.
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

#include <bcma/bcmbd/bcma_bcmbd.h>

int
bcma_bcmbd_encode_field(const bcmdrd_symbol_t *symbol, const char **fnames,
                        const char *field, const char *value,
                        uint32_t *and_masks, uint32_t *or_masks)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    int v, len, wsize;
    bcmdrd_sym_field_info_t finfo;
    char vstr[8 * BCMDRD_MAX_PT_WSIZE + 32];
    uint32_t val[BCMDRD_MAX_PT_WSIZE];


    BCMDRD_SYM_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        if (sal_strcasecmp(finfo.name, field)) {
            continue;
        }

        if (!bcma_bcmbd_parse_is_int(value)) {
            return -1;
        }

        sal_strlcpy(vstr, value, sizeof(vstr));

        sal_memset(val, 0, sizeof(val));
        if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
            wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
            bcmdrd_field_be_set(and_masks, wsize, finfo.minbit, finfo.maxbit, val);
        } else {
            bcmdrd_field_set(and_masks, finfo.minbit, finfo.maxbit, val);
        }

        /*
         * If the field value starts with 0x the accept values
         * spanning multiple words, e.g. 0x112233445566.
         */
        v = 0;
        if (vstr[0] == '0' && (vstr[1] == 'x' || vstr[1] == 'X')) {
            while ((len = sal_strlen(vstr)) > 10) {
                len -= 8;
                val[v++] = sal_strtoul(&vstr[len], NULL, 16);
                vstr[len] = 0;
            }
        }
        if (bcma_bcmbd_parse_uint32(vstr, &val[v]) < 0) {
            return -1;
        }
        if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
            wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
            bcmdrd_field_be_set(or_masks, wsize, finfo.minbit, finfo.maxbit, val);
        } else {
            bcmdrd_field_set(or_masks, finfo.minbit, finfo.maxbit, val);
        }

        return 0;
    } BCMDRD_SYM_FIELDS_ITER_END();

#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */

    return -1;
}

int
bcma_bcmbd_encode_fields_from_tokens(const bcmdrd_symbol_t *symbol,
                                     const char **fnames,
                                     int ctoks_num,
                                     const bcma_cli_tokens_t *ctoks,
                                     int max,
                                     uint32_t *and_masks,
                                     uint32_t *or_masks)
{
    const bcma_cli_tokens_t *ctok = ctoks;
    uint32_t data;
    int idx;

    /* Initialize masks */
    sal_memset(and_masks, ~0, max * sizeof(*and_masks));
    sal_memset(or_masks, 0, max * sizeof(*or_masks));

    if (ctok->argc == 2 && sal_strcmp(ctok->argv[0], "all") == 0) {
        /* All 32-bit data words are assigned the same value */
        if (bcma_bcmbd_parse_uint32(ctok->argv[1], &data) < 0) {
            return bcma_bcmbd_parse_error("field", ctok->str);
        }
        sal_memset(and_masks, 0, max * sizeof(*and_masks));
        for (idx = 0; idx < max; idx++) {
            or_masks[idx] = data;
        }
    }
    else if (ctok->argc > 0 && bcma_bcmbd_parse_is_int(ctok->argv[0])) {
        /* All tokens are treated as 32-bit data words */
        for (idx = 0; ctok->argc; idx++, ctok++) {
            if (bcma_bcmbd_parse_uint32(ctok->argv[0], &or_masks[idx]) < 0) {
                return bcma_bcmbd_parse_error("field", ctok->str);
            }
            and_masks[idx] = 0;
        }
    }
    else {
        /* All tokens are treated as field=value */
        for (idx = 0; ctok->argc && idx < ctoks_num; idx++, ctok++) {
            if (bcma_bcmbd_encode_field(symbol, fnames,
                                        ctok->argv[0], ctok->argv[1],
                                        and_masks, or_masks)) {
                return bcma_bcmbd_parse_error("field", ctok->str);
            }
        }
    }

    return 0;
}
