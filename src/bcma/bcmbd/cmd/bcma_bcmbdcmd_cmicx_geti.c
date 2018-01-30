/*! \file bcma_bcmbdcmd_cmicx_geti.c
 *
 * CLI 'geti' command for CMICx
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

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx_geti.h>

/*
 * geti cmic
 * Get a cmic register
 */
static int
geti_cmic(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    uint32_t addr;
    uint32_t data;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_uint32(arg, &addr) < 0) {
        return bcma_bcmbd_parse_error("address", arg);
    }

    addr &= ~3;
    BCMDRD_DEV_READ32(unit, addr, &data);

    cli_out("cmic[0x%"PRIx32"] = 0x%"PRIx32"\n", addr, data);
    return 0;
}

/*
 * Get an iProc register
 */
static int
geti_iproc(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    uint32_t addr;
    uint32_t data;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_uint32(arg, &addr) < 0) {
        return bcma_bcmbd_parse_error("address", arg);
    }

    addr &= ~3;
    bcmdrd_hal_iproc_read(unit, addr, &data);

    cli_out("iproc[0x%"PRIx32"] = 0x%"PRIx32"\n", addr, data);
    return 0;
}

/*
 * geti reg
 * Get a chip register via S-channel.
 */
static int
geti_reg(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    int size;
    bcma_bcmbd_id_t sid;

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_bcmbd_parse_error("address", arg);
    }

    /* Default size is 1 (32 bits) */
    size = 1;

    /* Optional second argument is the size of the register */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && (bcma_bcmbd_parse_int(arg, &size) < 0)) {
        return bcma_bcmbd_parse_error("size", arg);
    }

    /* Only 32 and 64 bit supported for registers */
    if (size != 1 && size != 2) {
        return bcma_bcmbd_parse_error("size", arg);
    }

    sal_sprintf(sid.addr.name, "0x%04"PRIx32"%08"PRIx32"",
                sid.addr.ext32, sid.addr.name32);

    /* Output all matching registers */
    return bcma_bcmbd_cmicx_regops(unit, NULL, -1, &sid, size, NULL, NULL);
}

/*
 * geti mem
 * Get a chip memory via S-channel.
 */
static int
geti_mem(bcma_cli_args_t *args)
{
    int unit = args->unit;
    const char *arg;
    int size;
    bcma_bcmbd_id_t sid;

    /* Crack the identifier */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_bcmbd_parse_id(arg, &sid, 1) < 0) {
        return bcma_bcmbd_parse_error("address", arg);
    }

    /* Default size is 1 (32 bits) */
    size = 1;

    /* Optional second argument is the memory size in words */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && (bcma_bcmbd_parse_int(arg, &size) < 0)) {
        return bcma_bcmbd_parse_error("size", arg);
    }

    /*
     * Memory specifications can come in a couple of formats:
     *
     * MEM
     * MEM[i0,i1]
     * MEM.blockN[i0, i1]
     * MEM.block[b0, b1].[i0,i1]
     */
    return bcma_bcmbd_cmicx_memops(unit, NULL, -1, &sid, size, NULL, NULL);
}


static int
geti_miim(bcma_cli_args_t *args)
{
#if 1
    cli_out ("geti miim is not supported yet.\n");

    return 0;
#else
    int unit = args->unit;
    const char *arg;
    uint32_t regaddr, data;
    bcma_bcmbd_id_t sid;
    int i, devad;

    /* Crack the phy_id and addresses */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || (bcma_bcmbd_parse_id(arg, &sid, 1) < 0) ||
        sid.addr.start >= 0) {
        return bcma_bcmbd_parse_error("miim addr", arg);
    }

    if (sid.port.start < 0 && sid.port.end < 0) {
        sid.port.start = 0;
        sid.port.end = 0x1f;
    } else if (sid.port.end < 0) {
        sid.port.end = sid.port.start;
    }

    /* If present, treat block number as clause 45 devad */
    devad = 0;
    if (sid.block.start >= 0) {
        devad = sid.block.start;
    }

    for (i = sid.port.start; i <= sid.port.end; i++) {
        regaddr = i + (0x10000 * devad);
        if (cdk_xgsd_miim_read(unit, sid.addr.name32, regaddr, &data) < 0) {
            cli_out("%s reading miim(0x%"PRIx32")[0x%x]\n",
                    BCMA_CLI_CONFIG_ERROR_STR, sid.addr.name32, i);
        } else {
            cli_out("miim(0x%"PRIx32")[0x%"PRIx32"] = 0x%"PRIx32"\n",
                    sid.addr.name32, regaddr, data);
        }
    }

    return 0;
#endif
}

static bcma_bcmbd_vect_t geti_vects[] =
{
    { "cmic",   geti_cmic,     },
    { "iproc",  geti_iproc,    },
    { "reg",    geti_reg,      },
    { "mem",    geti_mem,      },
    { "miim",   geti_miim,     },
    { 0, 0 },
};

int
bcma_bcmbdcmd_cmicx_geti(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    if (bcma_bcmbd_parse_vect(args, geti_vects, NULL) < 0) {
        bcma_bcmbd_parse_error("type", BCMA_CLI_ARG_CUR(args));
    }
    return 0;
}
