/*! \file bcmdrd_sym_profile.c
 *
 * BCMDRD routines related to register and memory symbol profile.
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

#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_symbols.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/* Common NULL entry array of memory symbols. */
const uint32_t bcmdrd_sym_mem_entry_null_zeroes[BCMDRD_MAX_PT_WSIZE] = {0};

int
bcmdrd_sym_mem_cmp_undef(void *ent_a, void *ent_b)
{
    assert(0 && "Cannot compare entries of this type");

    return 0;
}

int
bcmdrd_sym_mem_cmp_by_mask(void *ent_a, void *ent_b, void *imask, int size)
{
    int idx, bytes;

    bytes = (size & 3);

    for (idx = 1; idx <= bytes; idx++) {
        uint8_t val_a, val_b, mask;
        uint8_t *a = (uint8_t *)ent_a;
        uint8_t *b = (uint8_t *)ent_b;
        uint8_t *im = (uint8_t *)imask;

        mask = ~im[size - idx];
        val_a = a[size - idx] & mask;
        val_b = b[size - idx] & mask;

        BCMDRD_SYM_MEM_VAL_CMP_RETURN(val_a, val_b);
    }

    for (idx = (size >> 2) - 1; idx >= 0; idx--) {
        uint32_t val_a, val_b, mask;
        uint32_t *a = (uint32_t *)ent_a;
        uint32_t *b = (uint32_t *)ent_b;
        uint32_t *im = (uint32_t *)imask;

        mask = ~im[idx];
        val_a = a[idx] & mask;
        val_b = b[idx] & mask;

        BCMDRD_SYM_MEM_VAL_CMP_RETURN(val_a, val_b);
    }

    return 0;
}

bcmdrd_sym_mem_cmp_f
bcmdrd_sym_mem_cmp_fun_get(const bcmdrd_symbols_t *symbols,
                           const bcmdrd_symbol_t *symbol)
{
    const bcmdrd_sym_mem_profile_t *prfs, *spf;
    int offset;

    if (symbols == NULL || symbol == NULL) {
        return NULL;
    }

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        return NULL;
    }

    /* Get the memory profile array for all the symbols. */
    prfs = symbols->mem_profiles;
    if (prfs == NULL) {
        return NULL;
    }

    offset = BCMDRD_SYM_MP_OFFSET_GET(symbol->pfinfo);
    spf = prfs + offset;
    return spf->cmp_fn;
}

const void *
bcmdrd_sym_mem_null_ent_get(const bcmdrd_symbols_t *symbols,
                            const bcmdrd_symbol_t *symbol)
{
    const bcmdrd_sym_mem_profile_t *prfs, *spf;
    int offset;

    if (symbols == NULL || symbol == NULL) {
        return NULL;
    }

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        return NULL;
    }

    /* Get the memory profile array for all the symbols. */
    prfs = symbols->mem_profiles;
    if (prfs == NULL) {
        return NULL;
    }

    offset = BCMDRD_SYM_MP_OFFSET_GET(symbol->pfinfo);
    spf =  prfs + offset;
    return spf->null_ent;
}

const uint32_t *
bcmdrd_sym_reg_resetval_get(const bcmdrd_symbols_t *symbols,
                            const bcmdrd_symbol_t *symbol)
{
    const uint32_t *prfs;
    int offset;

    if (symbols == NULL || symbol == NULL) {
        return NULL;
    }

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER)) {
        return NULL;
    }

    /* Get the register resetvals array of the whole symbols. */
    prfs = symbols->reg_profiles;
    if (prfs == NULL) {
        return NULL;
    }

    offset = BCMDRD_SYM_RP_OFFSET_GET(symbol->pfinfo);

    return prfs + offset;
}

bool
bcmdrd_sym_default_value_is_nonzero(const bcmdrd_symbols_t *symbols,
                                    const bcmdrd_symbol_t *symbol)
{
    const uint32_t *resetval;

    if (symbol == NULL) {
        return false;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        resetval = bcmdrd_sym_mem_null_ent_get(symbols, symbol);
        return (resetval != bcmdrd_sym_mem_entry_null_zeroes);
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        int wsize, idx;

        resetval = bcmdrd_sym_reg_resetval_get(symbols, symbol);
        wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
        for (idx = 0; idx < wsize; idx++) {
            if (resetval[idx] != 0) {
                return true;
            }
        }
        return false;
    }
    return false;
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
