/*! \file bcma_bcmbdcmd_cmicm_seti.c
 *
 * CLI 'seti' command for CMICm
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

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicm.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicm_seti.h>

static int
parse_multiword(const char *str, bcma_cli_tokens_t *ctok,
                uint32_t *words, int max_words)
{
    int idx;

    /* Parse string into word tokens */
    if (bcma_cli_tokenize(str, ctok, ":") < 0) {
        return -1;
    }

    /* Check array size */
    if (ctok->argc > max_words) {
        return -1;
    }

    /* Convert all tokens to integers */
    for (idx = 0; idx < ctok->argc; idx++) {
        /* This argument must be an integer */
        if (bcma_bcmbd_parse_uint32(ctok->argv[idx], &words[idx]) < 0) {
            return -1;
        }
    }

    return 0;
}

/*
 * seti cmic
 * Set a cmic register
 */
static int
seti_cmic(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    uint32_t addr;
    uint32_t data;

    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_bcmbd_parse_uint32(arg, &addr) < 0) {
        return bcma_bcmbd_parse_error("address", arg);
    }
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_bcmbd_parse_uint32(arg, &data) < 0) {
        return bcma_bcmbd_parse_error("data", arg);
    }

    addr &= ~3;
    BCMDRD_DEV_WRITE32(unit, addr, data);

    return 0;
}

/*
 * seti reg
 * Get a chip register via S-channel.
 */
static int
do_seti_reg(bcma_cli_args_t *args, bcma_cli_tokens_t *ctok)
{
    int unit = args->unit;
    const char *arg;
    uint32_t and_masks[2];
    uint32_t or_masks[2];
    bcma_bcmbd_id_t sid;
    int size;

    /* Register will be cleared */
    and_masks[0] = and_masks[1] = 0;

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_bcmbd_parse_error("address", arg);
    }

    /* 32 or 64 bit multiword value */
    arg = BCMA_CLI_ARG_GET(args);
    or_masks[0] = or_masks[1] = 0;
    if (arg == NULL ||
        parse_multiword(arg, ctok, or_masks, COUNTOF(or_masks)) < 0) {
        return bcma_bcmbd_parse_error("data", arg);
    }

    /* Default size is the number of words specified */
    size = ctok->argc;

    /* Optional third argument is the size of the register */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && bcma_bcmbd_parse_int(arg, &size) < 0) {
        return bcma_bcmbd_parse_error("size", arg);
    }

    return bcma_bcmbd_cmicm_regops(unit, NULL, -1, &sid, size, and_masks, or_masks);
}

/*
 * seti reg
 * Get a chip register via S-channel.
 */
static int
seti_reg(bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_tokens_t *ctok;

    ctok = sal_alloc(sizeof(bcma_cli_tokens_t), "bcmaCliSetiCtoks");
    if (ctok == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = do_seti_reg(args, ctok);

    sal_free(ctok);

    return rv;
}

/*
 * seti mem
 * Get a chip memory via S-channel.
 */
static int
do_seti_mem(bcma_cli_args_t *args, bcma_cli_tokens_t *ctok)
{
    int unit = args->unit;
    const char *arg;
    int size;
    bcma_bcmbd_id_t sid;
    uint32_t and_masks[8];
    uint32_t or_masks[8];

    /* Memory will be cleared */
    sal_memset(and_masks, 0, sizeof(and_masks));

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_bcmbd_parse_error("address", arg);
    }

    /* Second argument is the data */
    arg = BCMA_CLI_ARG_GET(args);
    sal_memset(or_masks, 0, sizeof(or_masks));
    if (arg == NULL ||
        parse_multiword(arg, ctok, or_masks, COUNTOF(or_masks)) < 0) {
        return bcma_bcmbd_parse_error("data", arg);
    }

    /* Default size is the number of words specified */
    size = ctok->argc;

    /*
     * Optional third argument is the the memory size in words
     * Any words NOT specified in the data will be written as
     * zero (up to the size.)
     */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && (bcma_bcmbd_parse_int(arg, &size) < 0)) {
        return bcma_bcmbd_parse_error("size", arg);
    }

    return bcma_bcmbd_cmicm_memops(unit, NULL, -1, &sid, size, and_masks, or_masks);
}

/*
 * seti mem
 * Get a chip memory via S-channel.
 */
static int
seti_mem(bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_tokens_t *ctok;

    ctok = sal_alloc(sizeof(bcma_cli_tokens_t), "bcmaCliSetiCtoks");
    if (ctok == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = do_seti_mem(args, ctok);

    sal_free(ctok);

    return rv;
}


static int
seti_miim(bcma_cli_args_t *args)
{
#if 1
    cli_out ("seti miim is not supported yet.\n");

    return 0;
#else
    int unit = args->unit;
    const char *arg;
    uint32_t data;
    bcma_bcmbd_id_t sid;
    int i;

    /* Crack the phy_id and addresses */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL ||
        bcma_bcmbd_parse_id(arg, &sid, 1) < 0 || sid.addr.start >= 0) {
        return bcma_bcmbd_parse_error("miim addr", arg);
    }

    if (sid.port.start < 0 && sid.port.end < 0) {
        sid.port.start = 0;
        sid.port.end = 0x1f;
    } else if (sid.port.end < 0) {
        sid.port.end = sid.port.start;
    }

    /* Get the data */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_uint32(arg, &data) < 0) {
        return bcma_bcmbd_parse_error("miim data", arg);
    }

    for (i = sid.port.start; i <= sid.port.end; i++) {
        if (cdk_xgsm_miim_write(unit, sid.addr.name32, i, data) < 0) {
            cli_out("%s writing miim(0x%"PRIx32")[0x%x]\n",
                    BCMA_CLI_CONFIG_ERROR_STR, sid.addr.name32, i);
        }
    }

    return 0;
#endif
}

static bcma_bcmbd_vect_t seti_vects[] =
{
    { "cmic",   seti_cmic,     },
    { "reg",    seti_reg,      },
    { "mem",    seti_mem,      },
    { "miim",   seti_miim,     },
    { 0, 0 },
};

int
bcma_bcmbdcmd_cmicm_seti(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    if (bcma_bcmbd_parse_vect(args, seti_vects, NULL) < 0) {
        bcma_bcmbd_parse_error("type", BCMA_CLI_ARG_CUR(args));
    }
    return 0;
}
