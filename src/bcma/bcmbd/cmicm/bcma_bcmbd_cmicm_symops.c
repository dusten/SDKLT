/*! \file bcma_bcmbd_cmicm_symops.c
 *
 * Symbolic register/memory operations for CMICm devices.
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

#include <bcmbd/bcmbd_cmicm.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicm.h>

/*******************************************************************************
 * Private functions
 */

/*
 * Get or set a CMIC register.
 */
static int
cmic_symop(int unit, const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
           bcma_bcmbd_id_t *sid, uint32_t *and_masks, uint32_t *or_masks)
{
    uint32_t addr, data, step;
    int idx;

    /* No port/block identifiers on cmic registers */
    if (sid->port.valid || sid->block.valid) {
        return bcma_bcmbd_parse_error("cmic address", sid->id);
    }

    /* Did the user specify indices? */
    if (sid->addr.start == -1) {
        /* No indices specified, insert limits for the memory */
        sid->addr.start = BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
        sid->addr.end = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
    }

    step = BCMDRD_SYMBOL_INDEX_STEP_GET(symbol->index);

    for (idx = sid->addr.start; idx <= sid->addr.end; idx++) {

        /* Index 32 bit addresses */
        addr = symbol->addr + (idx * 4 * step);

        /* Read the data */
        BCMDRD_DEV_READ32(unit, addr, &data);

        /* This is a read-modify-write if masks are specified */
        if (and_masks || or_masks ) {
            if(and_masks) data &= and_masks[0];
            if(or_masks) data |= or_masks[0];

            /* Write the data */
            BCMDRD_DEV_WRITE32(unit, addr, data);

        } else {
            /* If we're here, it was a read operation and we should output the data */
            cli_out("%s", symbol->name);
            if (BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index) > 0) {
                cli_out("[%d]", idx);
            }
            cli_out(".cmic [0x%08"PRIx32"] = 0x%"PRIx32"\n", addr, data);

            /* Output field data if it is available */
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
            if (sid->flags & BCMA_BCMBD_CMD_IDF_RAW) {
                continue;
            }
            if (symbol->fields) {
                int skip_zeros = (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) ? 1 : 0;
                /* coverity[callee_ptr_arith] */
                bcma_bcmbd_show_fields(unit, symbol, &data, skip_zeros);
            }
#endif
        }
    }

    return 0;
}

/*
 * Get or set a chip register via S-channel.
 */
static int
reg_symop(int unit, const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
          bcma_bcmbd_id_t *sid, uint32_t *and_masks, uint32_t *or_masks)
{
    int port;
    int maxidx;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(symbol);
    assert(symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER);
    assert(cinfo);

    /* Did the user specify indices? */
    if (sid->addr.start == -1) {
        /* No indices specified, insert limits if any */
        maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
        if (maxidx) {
            sid->addr.start = 0;
            sid->addr.end = maxidx;
        }
    }

    /*
     * If the user specified a port number, but not block, then the ports as
     * specified are not block-based. Since such top-level ports can span
     * different blocks, we will iterate over all of the top-level ports
     * specified, and handle each specific port within the loop below.
     */
    for (port = sid->port.start; port <= sid->port.end; port++) {

        int b, match_block, wsize;
        bcma_bcmbd_id_t sid2;
        int blktype;
        const bcmdrd_block_t *blkp;

        /*
         * Iterate through all blocks of this symbol
         */
        for (blktype = 0; blktype < cinfo->nblktypes; blktype++) {

            if (!bcma_bcmbd_info_block_type_valid(unit, blktype, symbol->info)) {
                continue;
            }

            /* Need a copy of the SID for this block iteration */
            sal_memcpy(&sid2, sid, sizeof(sid2));

            /*
             * Set block type filter in case identical block types are
             * not contiguous.
             */
            sid2.block.ext32 = (1 << blktype);

            match_block = 0;

            /*
             * Iterate through all blocks in the chip
             */
            for (b = 0; b < cinfo->nblocks; b++) {

                /* Get the block pointer for this block */
                blkp = cinfo->blocks + b;
                assert(blkp);

                if (blkp->type != blktype) {
                    continue;
                }

                /* Does the ID contain a block specifier? */
                if (!sid2.block.valid) {
                    /* User didn't specify the block, so we'll insert this one */
                    sal_strlcpy(sid2.block.name,
                                bcma_bcmbd_block_type2name(unit, blktype),
                                sizeof(sid2.block.name));

                    /*
                     * If the user DID specify a top-level port number (port != -1)
                     * we need to see if that port is actually a part of this block.
                     * If not, we will just punt.
                     */
                    if (port != -1) {
                        bcmdrd_pblk_t pb;
                        bcmdrd_port_num_domain_t pnd;

                        pnd = bcmdrd_chip_port_num_domain(cinfo, enum_val, blkp->type);
                        if (!bcmdrd_chip_port_valid(cinfo, pnd, port)) {
                            continue;
                        }
                        if (pnd == bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type)) {
                            /* Look for this port within this blocktype */
                            if (bcmdrd_chip_port_block(cinfo, port, blkp->type, &pb) == 0 &&
                                pb.blknum == blkp->blknum) {
                                /* This top-level port is a member of this block */
                                sid2.block.start = sid2.block.end = pb.blknum;
                                sid2.port.start = sid2.port.end = pb.lane;
                            }
                        } else if (pnd != BCMDRD_PND_PHYS) {
                            /* Skip default port checks if not physical ports. */
                            sid2.block.start = sid2.block.end = blkp->blknum;
                            sid2.port.start = sid2.port.end = port;
                        } else {
                            /* This top-level port is not a member of this block */
                            continue;
                        }
                    }
                    else {
                        /* No block and no ports, insert all ports in this block */
                        /* Add all blocks of this type */
                        if (sid2.block.start == -1 || sid2.block.end < blkp->blknum) {
                            sid2.block.end = blkp->blknum;
                        }
                        if (sid2.block.start == -1 || sid2.block.start > blkp->blknum) {
                            sid2.block.start = blkp->blknum;
                        }
                    }
                }
                else {
                    /* User specified a block identifier */
                    /* does the block match this one? */
                    if (sal_strcmp(sid2.block.name,
                        bcma_bcmbd_block_type2name(unit, blktype)) == 0) {
                        /* Block specifier matches */
                        match_block = 1;
                        /* If start and stop were omitted, then we need to put them in */
                        if (sid->block.start == -1) {
                            /* Add all blocks of this type */
                            if (sid2.block.start == -1 || sid2.block.end < blkp->blknum) {
                                sid2.block.end = blkp->blknum;
                            }
                            if (sid2.block.start == -1 || sid2.block.start > blkp->blknum) {
                                sid2.block.start = blkp->blknum;
                            }
                        }
                        else {
                            /* specific blocks were indicated. */
                            /* Need to convert these to physical blocks */
                            sid2.block.start = bcmdrd_chip_block_number(cinfo, blktype,
                                                                        sid->block.start);
                            sid2.block.end = bcmdrd_chip_block_number(cinfo, blktype,
                                                                      sid->block.end);
                        }
                    }
                    else {
                        /* Block specified does not match this one. */
                        /* I guess we're done */
                        continue;
                    }
                }

                /* Blocks are all setup. Now we need to check ports */
                if (symbol->flags & BCMDRD_SYMBOL_FLAG_PORT) {
                    /* This is a port-based register */
                    /* Were specific ports specified? */
                    if (!sid2.port.valid) {
                        sid2.port.start = 0;
                        sid2.port.end = BCMDRD_CONFIG_MAX_PORTS - 1;
                    }
                } else {
                    /* Ignore port specification if not a port-based register */
                    sid2.port.start = -1;
                    sid2.port.end = -1;
                }
            }

            if (sid2.block.valid && !match_block) {
                continue;
            }

            /* Skip if we don't have a valid block range by now */
            if (sid2.block.start == -1) {
                continue;
            }

            /* Get dual-pipe access type from symbol flags */
            sid2.addr.ext32 = BCMBD_CMICM_BLKACC2ADEXT(symbol->info);

            /* Lets get it on */
            wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
            bcma_bcmbd_cmicm_regops(unit, symbol, enum_val, &sid2, wsize,
                                    and_masks, or_masks);
        }

        /*
         * If a block was specified, the ports we happen to be iterating over
         * are block-based physical ports, which we already processed.
         * Lets bail out of the outermost for loop.
         */
        if (sid->block.valid) {
            break;
        }
    }

    return 0;
}

/*
 * Get or Set a chip memory via S-channel.
 */
static int
mem_symop(int unit, const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
          bcma_bcmbd_id_t *sid, uint32_t *and_masks, uint32_t *or_masks)
{
    int b, match_block, wsize;
    int maxidx;
    bcma_bcmbd_id_t sid2;
    const bcmdrd_block_t *blkp;
    int blktype;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(symbol);
    assert(symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY);
    assert(cinfo);

    /* Silently ignore if ports were specified */
    if (sid->port.valid) {
        return 0;
    }

    /* Did the user specify indices? */
    if (sid->addr.start == -1) {
        /* No indices specified, insert limits for the memory */
        sid->addr.start = BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
        maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
        sid->addr.end = bcmdrd_chip_mem_maxidx(cinfo, enum_val, maxidx);
    }

    /*
     * Iterate through all blocks of which this memory is a member
     */
    for (blktype = 0; blktype < cinfo->nblktypes; blktype++) {

        if (!bcma_bcmbd_info_block_type_valid(unit, blktype, symbol->info)) {
            continue;
        }

        /* Need a copy of this SID for this block iteration */
        sal_memcpy(&sid2, sid, sizeof(sid2));

        match_block = 0;

        for (b = 0; b < cinfo->nblocks; b++) {

            /* Get the block pointer for this block */
            blkp = cinfo->blocks + b;
            assert(blkp);

            if (blkp->type != blktype){
                continue;
            }

            /* Does the SID contain a block specifier? */
            if (!sid2.block.valid) {
                /* If no specific blocks were specified, add all blocks of this type */
                if (sid2.block.start == -1 || sid2.block.end < blkp->blknum) {
                    sid2.block.end = blkp->blknum;
                }
                if (sid2.block.start == -1 || sid2.block.start > blkp->blknum) {
                    sid2.block.start = blkp->blknum;
                }
            }
            else {
                /* User specified a block identifier */
                /* does the block match this one? */
                if (sal_strcmp(sid2.block.name,
                    bcma_bcmbd_block_type2name(unit, blktype)) == 0) {
                    /* Block specifier matches */
                    match_block = 1;
                    /* If start and stop were omitted, then we need to put them in */
                    if (sid->block.start == -1) {
                        /* Add all blocks of this type */
                        if (sid2.block.start == -1 || sid2.block.end < blkp->blknum) {
                            sid2.block.end = blkp->blknum;
                        }
                        if (sid2.block.start == -1 || sid2.block.start > blkp->blknum) {
                            sid2.block.start = blkp->blknum;
                        }
                    }
                    else {
                        /* specific blocks were indicated. */
                        /* Need to convert these to physical blocks */
                        sid2.block.start = bcmdrd_chip_block_number(cinfo, blktype,
                                                                    sid->block.start);
                        sid2.block.end = bcmdrd_chip_block_number(cinfo, blktype,
                                                                  sid->block.end);
                    }
                }
                else {
                    /* Block specified does not match this one. */
                    /* I guess we're done */
                    continue;
                }
            }
        }

        /* Does the specified block match? */
        if (sid2.block.valid && !match_block) {
            continue;
        }

        /* We don't handle port numbers on memories */
        assert((symbol->flags & BCMDRD_SYMBOL_FLAG_PORT) == 0);

        /* Skip if we don't have a valid block range by now */
        if (sid2.block.start == -1) {
            continue;
        }

        /* Get dual-pipe access type from symbol flags */
        sid2.addr.ext32 = BCMBD_CMICM_BLKACC2ADEXT(symbol->info);

        wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
        bcma_bcmbd_cmicm_memops(unit, symbol, enum_val, &sid2, wsize,
                                and_masks, or_masks);
    }

    return 0;
}

/*******************************************************************************
 * Exported functions
 */

int
bcma_bcmbd_cmicm_symop(int unit,
                       const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                       bcma_bcmbd_id_t *sid,
                       uint32_t *and_masks, uint32_t *or_masks)
{
    int cmic_blktype = bcma_bcmbd_block_name2type(unit, "cmic");

    /* Dispatch according to symbol type */
    if (bcma_bcmbd_info_block_type_valid(unit, cmic_blktype, symbol->info)) {
        cmic_symop(unit, symbol, enum_val, sid, and_masks, or_masks);
    }
    else if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        reg_symop(unit, symbol, enum_val, sid, and_masks, or_masks);
    }
    else if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        mem_symop(unit, symbol, enum_val, sid, and_masks, or_masks);
    }
    else {
        /* Should never get here */
        cli_out("%ssymbol '%s' was not generated correctly\n",
                BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
    }
    return 0;
}
