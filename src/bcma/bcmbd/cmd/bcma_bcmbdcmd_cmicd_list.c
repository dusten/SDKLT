/*! \file bcma_bcmbdcmd_cmicd_list.c
 *
 * CLI 'list' command for CMICd
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

#include <shr/shr_pb.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_sym_dump.h>
#include <bcmdrd/bcmdrd_pt_ser.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_list.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

typedef struct sym_info_s {
    int unit;
    uint32_t flags;
    char *view;
} sym_info_t;


#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1

static int
view_filter(const bcmdrd_symbol_t *symbol, const char **fnames,
            const char *encoding, void *cookie)
{
    char *view = (char *)cookie;
    char *ptr;

    /* Do not filter if no (or unknown) encoding */
    if (encoding == NULL || *encoding != '{' || view == NULL) {
        return 0;
    }

    /* Do not filter if encoding cannot be parsed */
    if ((ptr = sal_strchr(encoding, '}')) == NULL) {
        return 0;
    }

    /* Do not filter if view (partially) matches */
    ptr++;
    if (sal_strcmp(view, "*") == 0 ||
        sal_strncasecmp(ptr, view, sal_strlen(view)) == 0) {
        return 0;
    }

    /* No match - filter this field */
    return 1;
}

#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */

/*
 * Prints the information for a single symbol
 */
static int
print_sym(const bcmdrd_symbol_t *s, bcmdrd_sid_t symbol_id, void *vptr)
{
    sym_info_t *si = (sym_info_t *)vptr;
    uint32_t mask, flags, acc_type, blk_info;
    uint32_t size, min, max, step;
    int enum_val, blktype, idx;
    const char *flagstr;
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(si->unit, 0);
    bcmdrd_pt_ser_info_t ser_info, *seri = &ser_info;
    int rv;

    if (si->flags & BCMA_BCMBD_CMD_IDF_RAW) {
        cli_out("%s\n", s->name);
        return 0;
    }

    size = BCMDRD_SYMBOL_INDEX_SIZE_GET(s->index);
    min = BCMDRD_SYMBOL_INDEX_MIN_GET(s->index);
    max = BCMDRD_SYMBOL_INDEX_MAX_GET(s->index);
    step = BCMDRD_SYMBOL_INDEX_STEP_GET(s->index);

    /* Extract dual-pipe access type from flags */
    flags = s->flags;
    acc_type = BCMDRD_SYM_INFO_ACCTYPE(s->info);
    blk_info = BCMDRD_SYM_INFO_BLKBMP(s->info);

    /* Max index may depend on current chip configuration */
    if (flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        enum_val = symbol_id;
        max = bcmdrd_chip_mem_maxidx(bcmdrd_dev_chip_info_get(si->unit), enum_val, max);
    }

    cli_out("%-13s %s", "Name:", s->name);
#if  BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES == 1
    if (s->ufname) {
        cli_out(", %s", s->ufname);
    }
    if (s->alias) {
        cli_out(", %s", s->alias);
    }
#endif
    cli_out("\n");
    cli_out("%-13s 0x%08"PRIx32"\n", "Offset:", s->addr);
    if (acc_type) {
        cli_out("%-13s %"PRIu32"\n", "Access:", acc_type);
    }
    if (flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        cli_out("%-13s %"PRIu32" bytes (%"PRIu32" words)\n",
                "Size:", size, BCMDRD_BYTES2WORDS(size));
    } else {
        const uint32_t *resetval;

        cli_out("%-13s %d-bit\n", "Size:", BCMDRD_BYTES2BITS(size));

        resetval = bcmdrd_sym_reg_resetval_get(symbols, s);
        if (resetval) {
            int wsize;

            wsize = BCMDRD_BYTES2WORDS(size);
            cli_out("%-13s 0x%08"PRIx32, "Reset value:", resetval[0]);
            for (idx = 1; idx < wsize; idx++) {
                cli_out(", 0x%08"PRIx32, resetval[idx]);
            }
            cli_out("\n");

            cli_out("%-13s 0x%08"PRIx32, "Reset mask:", resetval[wsize]);
            for (idx = 1; idx < wsize; idx++) {
                cli_out(", 0x%08"PRIx32, resetval[wsize + idx]);
            }
            cli_out("\n");
        }
    }

    cli_out("%-13s ", "Flags:");

    for (mask = 1; mask; mask <<= 1) {
        if (flags & mask) {
            flagstr = bcma_bcmbd_symflag_type2name(mask);
            if (flagstr != NULL) {
                cli_out("%s,", flagstr);
            }
        }
    }

    cli_out("(0x%"PRIx32")\n", flags);

    cli_out("%-13s ", "Blocks:");

    for (idx=0; idx < BCMA_BCMBD_MAX_BLKTYPES_PER_SYM; idx++) {
        blktype = bcma_bcmbd_info_block_type_get(si->unit, idx, blk_info);
        if (blktype < 0) {
            break;
        }
        flagstr = bcma_bcmbd_block_type2name(si->unit, blktype);
        if (flagstr != NULL) {
            cli_out("%s,", flagstr);
        }
    }

    cli_out("(0x%"PRIx32")\n", blk_info);

    if ((flags & BCMDRD_SYMBOL_FLAG_MEMORY) || (max - min) > 0) {
        cli_out("%-13s %"PRIu32":%"PRIu32"", "Index:", min, max);
        if (step > 1) {
            cli_out(" (step 0x%"PRIx32")", step);
        }
        cli_out("\n");
    }

    /* SER information */
    cli_out("%-13s ", "ECC/SER:");
    rv = bcmdrd_pt_ser_info_get(si->unit, symbol_id, seri);
    if (rv == SHR_E_NONE) {
        cli_out("yes\n");
        cli_out("\t%-15s %s\n",
                "Memory type:", bcma_bcmbd_ser_type2name(seri->type));
        cli_out("\t%-15s %d\n", "XOR banks:", seri->num_xor_banks);
        cli_out("\t%-15s %s\n",
                "Drop packet:", seri->drop_pkt ? "true" : "false");
        cli_out("\t%-15s %s\n",
                "Response:", bcma_bcmbd_ser_resp_type2name(seri->resp));
    } else {
        cli_out("n/a\n");
    }
    /* SER control register/field information */
    for (idx = 0; idx < BCMDRD_SER_EN_TYPE_MAX; idx++) {
        bcmdrd_pt_ser_en_ctrl_t ser_en_ctrl, *ser_ec = &ser_en_ctrl;
        bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
        bcmdrd_sym_field_info_t field_info, *finfo = &field_info;

        rv = bcmdrd_pt_ser_en_ctrl_get(si->unit, idx, symbol_id, ser_ec);
        if (rv != SHR_E_NONE) {
            continue;
        }
        bcmdrd_sym_info_get(symbols, ser_ec->sid, sinfo);
        bcmdrd_sym_field_info_get(symbols, ser_ec->sid, ser_ec->fid, finfo);
        cli_out("\t%-15s ", bcma_bcmbd_ser_en_type2name(idx));
        cli_out("%s.%s", sinfo->name, finfo->name);
        if (ser_ec->fmask != 0) {
            cli_out("(mask=0x%"PRIx32")", ser_ec->fmask);
        }
        cli_out("\n");
    }

#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    if (s->fields && (si->flags & BCMA_BCMBD_CMD_IDF_NONZERO) == 0) {
        shr_pb_t *pb;
        bcmdrd_symbol_filter_cb_f filter_cb;

        assert(symbols);

        filter_cb = (si->view) ? view_filter : bcmdrd_symbol_field_filter;
        cli_out("%-13s %"PRIu32"\n",
                "Fields:", bcmdrd_sym_field_info_count(s->fields));
        pb = shr_pb_create();
        bcmdrd_sym_dump_fields(pb, s, symbols->field_names,
                               NULL, BCMDRD_SYM_DUMP_BIT_RANGE,
                               filter_cb, si->view);
        cli_out("%s", shr_pb_str(pb));
        shr_pb_destroy(pb);
    }
#endif

    cli_out("\n");
    return 0;
}

static int
cmicd_list(bcma_cli_t *cli, bcma_cli_args_t *args,
           bcma_cli_tokens_t *ctoks, int ctoks_num)
{
    int idx;
    int unit;
    const char *name = NULL;
    const char *arg = NULL;
    const bcmdrd_symbols_t *symbols;
    bcmdrd_symbols_iter_t iter;
    sym_info_t sym_info;
    uint32_t flags = 0;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return BCMA_CLI_CMD_NO_SYM;
    }

    /* Parse all of our input arguments for options */
    if (BCMA_CLI_ARG_CNT(args) == 0 ||
        (arg = bcma_bcmbd_parse_args(args, ctoks, ctoks_num)) != NULL) {
        /* Error in argument i */
        return bcma_bcmbd_parse_error("symbol", arg);
    }

    sal_memset(&iter, 0, sizeof(iter));
    sal_memset(&sym_info, 0, sizeof(sym_info));

    /* Look through our arguments */
    for (idx = 0; idx < ctoks_num && ctoks[idx].argc; idx++) {
        /* Flags Specified? */
        if (sal_strcmp("nz", ctoks[idx].argv[0]) == 0) {
            flags |= BCMA_BCMBD_CMD_IDF_NONZERO;
        }
        else if (sal_strcmp("raw", ctoks[idx].argv[0]) == 0) {
            flags |= BCMA_BCMBD_CMD_IDF_RAW;
        }
        else if (sal_strcmp("flags", ctoks[idx].argv[0]) == 0 &&
            ctoks[idx].argc > 1) {
            bcma_bcmbd_tok2flags(unit, &ctoks[idx], &iter.pflags, &iter.aflags,
                                 &iter.pinfo, &iter.ainfo);
        }
        else if (name == NULL) {
            /* Symbol expression */
            name = ctoks[idx].argv[0];
        }
        else {
            /* If multi-view memory */
            sym_info.view = ctoks[idx].argv[0];
        }
    }

    if (name == NULL) {
        return bcma_bcmbd_parse_error("symbol", NULL);
    }

    /*
     * By default we list all symbols with the input name as a substring
     */
    iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_STRSTR;

    /*
     * The user can specify explicitly the type of matching with the
     * first character.
     */
    if (sal_strcmp(name, "*") != 0) {
        switch (name[0]) {
        case '^':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_START;
            name++;
            break;
        case '*':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_STRSTR;
            name++;
            break;
        case '@':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_EXACT;
            name++;
            break;
        default:
            break;
        }
    }

    sym_info.unit = unit;
    sym_info.flags = flags;

    /* Interate over all matching symbols */
    iter.name = name;
    iter.symbols = symbols;
    iter.function = print_sym;
    iter.vptr = &sym_info;

    if (bcmdrd_symbols_iter(&iter) <= 0) {
        cli_out("no matching symbols\n");
    }

    return 0;
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */

int
bcma_bcmbdcmd_cmicd_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
    return BCMA_CLI_CMD_NO_SYM;
#else
    int rv;
    int ctoks_num = 3;
    bcma_cli_tokens_t *ctoks;

    ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                      "bcmaCliListCtoks");
    if (ctoks == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = cmicd_list(cli, args, ctoks, ctoks_num);

    sal_free(ctoks);

    return rv;
#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
}
