/*! \file bcma_bcmbd_cmicm_memregs.c
 *
 * This file provides all of the basic register and memory reads and writes
 * for CMICm devices.
 * These functions are shared by both the symbolic and non-symbolic commands.
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
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_cmicm_reg.h>
#include <bcmbd/bcmbd_cmicm_mem.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicm.h>

/*******************************************************************************
 * Private functions
 */

/*
 * Print port information to specified buffer
 */
static int
port_str_get(int unit, int port, char *buf, int size)
{

#if 0
/* #if CDK_CONFIG_INCLUDE_PORT_MAP == 1 */
    /* Show logical port as well if port is mapped */
    if (CDK_DEV(unit)->port_map_size > 0) {
        CDK_SNPRINTF(buf, size, "%2d -> %2d", CDK_PORT_MAP_P2L(unit, port), port);
    } else
#endif
    sal_snprintf(buf, size, "%2d", port);

    return 0;
}

/*
 * Calculate the blocktype and instance for a physical block number
 */
static int
block_pnd(int unit, int block, bcmdrd_port_num_domain_t *pnd)
{
    int idx;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    /* Get block type */
    for (idx = 0; idx < cinfo->nblocks; idx++) {
        const bcmdrd_block_t *blkp = cinfo->blocks + idx;
        if (blkp->blknum == block) {
            if (pnd) {
                *pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);
            }
            return 0;
        }
    }
    return -1;
}

/*
 * Display register/memory operations information
 */
static void
print_memreg_op(int unit, const char *prefix, bcma_bcmbd_id_t *sid,
                int a, int b, int p, uint32_t adext, uint32_t addr,
                const char *errstr, int rc)
{
    char blockname[32];
    char port_str[16];
    bcmdrd_port_num_domain_t pnd;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    if (rc < 0) {
        cli_out("%s%s ", BCMA_CLI_CONFIG_ERROR_STR, errstr);
    }

    cli_out("%s %s", prefix, sid->addr.name);
    if (a >= 0) {
        cli_out("[%d]", a);
    }
    if (b >= 0) {
        cli_out(".%s", bcma_bcmbd_block_name(unit, b, blockname,
                                                 sizeof(blockname)));
    }
    if (p >= 0) {
        int port = bcmdrd_chip_port_number(cinfo, b, p);

        port_str_get(unit, port, port_str, sizeof(port_str));
        /* Check if this block uses logical/MMU port numbers */
        if (b >= 0 && block_pnd(unit, b, &pnd) == 0) {
            if (pnd == BCMDRD_PND_LOGIC) {
                sal_sprintf(port_str, "logic-%d", p);
            } else if (pnd == BCMDRD_PND_MMU) {
                sal_sprintf(port_str, "mmu-%d", p);
            }
        }
        cli_out(".%s%d (BLOCK %d, PORT %s)", sid->port.name, p, b, port_str);
    }
    cli_out(" [0x%03"PRIx32"%08"PRIx32"]", adext, addr);

    if (rc < 0) {
        cli_out(" (%d)", rc);
    }
}

/*
 * Perform register read/write operation and display necessary information.
 */
static int
cmicm_cmd_regop(int unit, const bcmdrd_symbol_t *symbol,
                bcma_bcmbd_id_t *sid,
                int aindex, int block, int port, uint32_t size,
                uint32_t *and_masks, uint32_t *or_masks)
{
    int rc = 0;
    uint32_t idx, adext, addr;
    uint32_t data[2];

    if (size > sizeof(data) / sizeof(uint32_t)) {
        cli_out("%sentity size (%"PRIx32" words) too big\n",
                BCMA_CLI_CONFIG_ERROR_STR, size);
        return -1;
    }

    /* Default address */
    addr = sid->addr.name32;
    adext = sid->addr.ext32;

    /* Calculate the absolute address for this access */
    if (symbol && block > 0) {
        int lane = port;
        uint32_t step = BCMDRD_SYMBOL_INDEX_STEP_GET(symbol->index);
        if (lane < 0) {
            lane = 0;
        }
        /* Update address extension with specified block */
        BCMBD_CMICM_ADEXT_BLOCK_SET(adext, block);
        addr = bcmbd_block_port_addr(unit, block, lane, addr, aindex * step);
    }

    /* Read the data */
    if (size == 1) {
        rc = bcmbd_cmicm_reg_read(unit, adext, addr, data, 1);
    } else {
        rc = bcmbd_cmicm_reg_read(unit, adext, addr, data, 2);
    }

    /* Print an error message if the read failed */
    if (rc < 0) {
        print_memreg_op(unit, "register", sid, aindex, block, port,
                        adext, addr, "reading", rc);
        cli_out("\n");
        return rc;
    }

    /* If masks are specific, this is a read-modify-write operation */
    if (and_masks || or_masks) {
        for (idx = 0; idx < size; idx++) {
            if (and_masks) {
                data[idx] &= and_masks[idx];
            }
            if (or_masks) {
                data[idx] |= or_masks[idx];
            }
        }

        /* Write the data */
        if (size == 1) {
            rc = bcmbd_cmicm_reg_write(unit, adext, addr, data, 1);
        } else {
            rc = bcmbd_cmicm_reg_write(unit, adext, addr, data, 2);
        }

        /* Print en error message if the write failed */
        if (rc < 0) {
            print_memreg_op(unit, "register", sid, aindex, block, port,
                            adext, addr, "writing", rc);
            cli_out("\n");
            return rc;
        }

        /* No more to be done if this was a write rather than a read */
        return 0;
    }

    if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        for (idx = 0; idx < size; idx++) {
            if (data[idx]) {
                break;
            }
        }
        if (idx >= size) {
            return 0;
        }
    }

    /* If we are here, this was a read. Output the register data */
    print_memreg_op(unit, "register", sid, aindex, block, port,
                    adext, addr, "reading", rc);
    if (size == 1) {
        cli_out(" = 0x%08"PRIx32"\n", data[0]);
    }
    if (size == 2) {
        cli_out(" = 0x%08"PRIx32":0x%08"PRIx32"\n", data[0], data[1]);
    }

    /* Decode the individual fields if they are available */
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    if (sid->flags & BCMA_BCMBD_CMD_IDF_RAW) {
        return 0;
    }
    if (symbol && symbol->fields) {
        int skip_zeros = (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) ? 1 : 0;
        /* Decode the result */
        bcma_bcmbd_show_fields(unit, symbol, data, skip_zeros);
    }
#endif
    return 0;
}

/*
 * Perform memory read/write operation and display necessary information.
 */
static int
cmicm_cmd_memop(int unit, const bcmdrd_symbol_t *symbol,
                bcma_bcmbd_id_t *sid,
                int block, int mindex, uint32_t size,
                uint32_t *and_masks, uint32_t *or_masks)
{
    int rc;
    int idx;
    uint32_t adext, i, addr;
    uint32_t data[BCMDRD_MAX_PT_WSIZE];

    if (size > COUNTOF(data)) {
        cli_out("%sentity size (%"PRIx32" words) too big\n",
                BCMA_CLI_CONFIG_ERROR_STR, size);
        return -1;
    }

    /* For raw access, memory index may be unspecified */
    idx = mindex;
    if (idx < 0) {
        idx = 0;
    }

    /* Default address */
    addr = sid->addr.name32;
    adext = sid->addr.ext32;

    /* Calculate the absolute address for this memory read */
    if (block > 0) {
        /* Update address extension with specified block */
        BCMBD_CMICM_ADEXT_BLOCK_SET(adext, block);
        addr = bcmbd_block_port_addr(unit, block, -1, addr, -1);
    }

    /* Read the data */
    /* coverity[overrun-call] */
    rc = bcmbd_cmicm_mem_read(unit, adext, addr + idx, data, size);

    /* If the read failed, output an error */
    if (rc < 0) {
        print_memreg_op(unit, "memory", sid, mindex, block, -1,
                        adext, addr + idx, "reading", rc);
        cli_out("\n");
        return rc;
    }

    /* If masks are specified, this is a read-modify-write */
    if (and_masks || or_masks) {
        for (i = 0; i < size; i++) {
            if(and_masks) data[i] &= and_masks[i];
            if(or_masks) data[i] |= or_masks[i];
        }

        /* Write the data */
        rc = bcmbd_cmicm_mem_write(unit, adext, addr + idx, data, size);

        /* If the write failed, output an error */
        if (rc < 0) {
            print_memreg_op(unit, "memory", sid, mindex, block, -1,
                            adext, addr + idx, "writing", rc);
            cli_out("\n");
            return rc;
        }

        /* Nothing more to be done for writes */
        return rc;
    }

    if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        for (i = 0; i < size; i++) {
            if (data[i]) break;
        }
        if (i >= size) return 0;
    }

    /* If we got here, this was a read. Print out the memory data */
    print_memreg_op(unit, "memory ", sid, mindex, block, -1,
                    adext, addr + idx, "reading", rc);
    cli_out(" = ");
    for (i = 0; i < size; i++) {
        cli_out("0x%08"PRIx32" ", data[i]);
    }
    cli_out("\n");

    /* Decode the individual fields if they are available */
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    if (sid->flags & BCMA_BCMBD_CMD_IDF_RAW) {
        return 0;
    }
    if (symbol && symbol->fields) {
        int skip_zeros = (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) ? 1 : 0;
        /* Decode the result */
        bcma_bcmbd_show_fields(unit, symbol, data, skip_zeros);
    }
#endif

    return 0;
}

/*******************************************************************************
 * Exported functions
 */

int
bcma_bcmbd_cmicm_regops(int unit,
                        const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                        bcma_bcmbd_id_t *sid, uint32_t size,
                        uint32_t *and_masks, uint32_t *or_masks)
{
    int a, b, bt = -1, p;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    /* Foreach address index */
    for (a = sid->addr.start; a <= sid->addr.end; a++) {
        /* Foreach block number */
        for (b = sid->block.start; b <= sid->block.end; b++) {

            if (b != -1) {
                /* If this is not a valid block number for the chip, skip it */
                if (bcmdrd_chip_block_type(cinfo, b, &bt, NULL) < 0) {
                    continue;
                }

                /* Check that block type is valid for this symbol */
                if (symbol &&
                    !bcma_bcmbd_info_block_type_valid(unit, bt, symbol->info)) {
                    continue;
                }

                /* Check that block type is valid for this operation */
                if (sid->block.ext32 && (sid->block.ext32 & (1 << bt)) == 0) {
                    continue;
                }

                /* Skip this block if its port bitmap is empty.
                 * This is because not all blocks of the same type that exist
                 * on a chip may have ports in them, specially true of GPORTs.
                 */
                if (bcma_bcmbd_symbol_block_valid_ports_get(unit, enum_val, b, &pbmp) < 0) {
                    continue;
                }
                if (BCMDRD_PBMP_IS_NULL(pbmp)) {
                    continue;
                }
            }

            /* Foreach port */
            for (p = sid->port.start; p <= sid->port.end; p++) {

                /* If this is not a valid port for this block number, skip it */
                if (symbol && b != -1 && p != -1 && bt != -1) {
                    int port;
                    bcmdrd_port_num_domain_t pnd;

                    pnd = bcmdrd_chip_port_num_domain(cinfo, enum_val, bt);
                    if (pnd == bcmdrd_chip_port_num_domain(cinfo, -1, bt)) {
                        port = bcmdrd_chip_port_number(cinfo, b, p);
                        if (port == -1) {
                            /* This combination is not valid for the chip */
                            continue;
                        }
                    } else if (pnd != BCMDRD_PND_PHYS) {
                        port = p;
                    } else {
                        continue;
                    }
                    if (!bcmdrd_chip_port_reg_valid(cinfo, enum_val, port, a)) {
                        continue;
                    }
                }
                /* Perform the operation on this specific register */
                cmicm_cmd_regop(unit, symbol, sid, a, b, p, size,
                                and_masks, or_masks);
            }
        }
    }
    return 0;
}

int
bcma_bcmbd_cmicm_memops(int unit,
                        const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                        bcma_bcmbd_id_t *sid, uint32_t size,
                        uint32_t *and_masks, uint32_t *or_masks)
{
    int b, idx;

    for (b = sid->block.start; b <= sid->block.end; b++) {
        for (idx = sid->addr.start; idx <= sid->addr.end; idx++) {
            cmicm_cmd_memop(unit, symbol, sid, b, idx, size,
                            and_masks, or_masks);
        }
    }
    return 0;
}
