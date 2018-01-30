/*! \file bcma_bcmbdcmd_cmicx_delete.c
 *
 * CLI 'delete' command for CMICx
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

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx_delete.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/*
 * Private data for symbol iterator.
 */
typedef struct cmicx_iter_s {
    int unit;
    bcma_bcmbd_id_t *sid;
    int ctoks_num;
    bcma_cli_tokens_t *ctoks;
} cmicx_iter_t;

/*
 * Get or Set the data for a symbol -- symbol iterator function
 */
static int
iter_op(const bcmdrd_symbol_t *symbol, bcmdrd_sid_t symbol_id, void *vptr)
{
    uint32_t and_masks[BCMDRD_MAX_PT_WSIZE];
    uint32_t or_masks[BCMDRD_MAX_PT_WSIZE];
    cmicx_iter_t *cmicx_iter = (cmicx_iter_t *)vptr;
    bcma_bcmbd_id_t sid;
    int ctoks_num = cmicx_iter->ctoks_num;
    bcma_cli_tokens_t *ctoks = cmicx_iter->ctoks;
    const char **fnames;
    int unit = cmicx_iter->unit;
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(unit, 0);

    assert(symbols);

    fnames = symbols->field_names;

    sal_memcpy(&sid, cmicx_iter->sid, sizeof(sid));

    /* Copy the address in for this symbol */
    sal_strlcpy(sid.addr.name, symbol->name, sizeof(sid.addr.name));
    sid.addr.name32 = symbol->addr;

    /* These CSTs contain the data and/or field assignments */
    bcma_bcmbd_encode_fields_from_tokens(symbol, fnames, ctoks_num, ctoks,
                                         BCMDRD_MAX_PT_WSIZE,
                                         and_masks, or_masks);

    bcma_bcmbd_cmicx_tbl_delete(unit, symbol, &sid, and_masks, or_masks);

    return 0;
}

static int
cmicx_delete(bcma_cli_t *cli, bcma_cli_args_t *args,
             bcma_cli_tokens_t *ctoks, int ctoks_num)
{
    int idx;
    int data_start = 0;
    int unit;
    const bcmdrd_symbols_t *symbols;
    const char *arg = NULL;
    bcma_bcmbd_id_t sid;
    bcmdrd_symbols_iter_t iter;
    cmicx_iter_t cmicx_iter;
    uint32_t sid_flags = 0;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return BCMA_CLI_CMD_NO_SYM;
    }

    sal_memset(&iter, 0, sizeof(iter));
    sal_memset(&cmicx_iter, 0, sizeof(cmicx_iter));

    /*
     * The format of this command must be one of the following:
     *
     * set <symbol> [flags=] <[data words] or [field assignments]>
     * set <flags=> <[data words] or [field assignments]>
     */

    /* Parse input arguments */
    if ((arg = bcma_bcmbd_parse_args(args, ctoks, ctoks_num)) != NULL) {
        /* Error in argument i */
        return bcma_bcmbd_parse_error("", arg);
    }

    /* Match any symbol by default */
    bcma_bcmbd_parse_id("*", &sid, 0);

    /* Look through our arguments */
    for (idx = 0; idx < ctoks_num && ctoks[idx].argc; idx++) {
        /* Flags specified? */
        if (sal_strcmp("flags", ctoks[idx].argv[0]) == 0 &&
            ctoks[idx].argc > 1) {
            bcma_bcmbd_tok2flags(unit, &ctoks[idx], &iter.pflags, &iter.aflags,
                                 &iter.pinfo, &iter.ainfo);
        }
        else {
            /* Field data value specified */
            if (ctoks[idx].argc > 1) {
                data_start = idx;
                break;
            }
            /* Raw data value specified */
            if (bcma_bcmbd_parse_is_int(ctoks[idx].argv[0])) {
                data_start = idx;
                break;
            }
            /* Crack the identifier */
            if (bcma_bcmbd_parse_id(ctoks[idx].argv[0], &sid, 0) < 0) {
                return bcma_bcmbd_parse_error("symbol", ctoks[idx].argv[0]);
            }
        }
    }

    if (data_start == 0) {
        return bcma_bcmbd_parse_error("data", NULL);
    }

    sid.flags = sid_flags;

    cmicx_iter.unit = unit;
    cmicx_iter.ctoks_num = ctoks_num;
    cmicx_iter.ctoks = &ctoks[data_start];
    cmicx_iter.sid = &sid;

    iter.name = sid.addr.name;
    iter.symbols = symbols;
    iter.function = iter_op;
    iter.vptr = &cmicx_iter;

    /* Iterate */
    if (bcmdrd_symbols_iter(&iter) <= 0) {
        cli_out("no matching symbols\n");
    }

    return 0;
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */

int
bcma_bcmbdcmd_cmicx_delete(bcma_cli_t *cli, bcma_cli_args_t *args)
{
#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
    return BCMA_CLI_CMD_NO_SYM;
#else
    int rv;
    int ctoks_num = BCMA_CLI_CONFIG_ARGS_CNT_MAX;
    bcma_cli_tokens_t *ctoks;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliDeleteCtoks");
    if (ctoks == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = cmicx_delete(cli, args, ctoks, ctoks_num);

    sal_free(ctoks);

    return rv;
#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
}
