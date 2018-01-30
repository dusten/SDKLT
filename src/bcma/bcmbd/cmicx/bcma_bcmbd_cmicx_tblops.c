/*! \file bcma_bcmbd_cmicx_tblops.c
 *
 * Symbolic hash table operations for CMICd devices.
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

#include <shr/shr_error.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_cmicx_tbl.h>

#include <bcma/bcmbd/bcma_bcmbd_cmicx.h>

#define CMICX_RESP_WORD_INDEX_GET(_w)  ((_w) & 0xfffff)

/*! Supported CLI BCMBD CMICd table operations. */
typedef enum cmicx_tbl_op_e {
    CMICX_TBL_INSERT,
    CMICX_TBL_DELETE,
    CMICX_TBL_LOOKUP,
} cmicx_tbl_op_t;

/*******************************************************************************
 * Private functions
 */

/*
 * Display table operations information
 */
static void
print_tbl_op(int unit, const char *prefix, bcma_bcmbd_id_t *sid,
             int a, int b, int t, int sect_size,
             uint32_t adext, uint32_t addr,
             const char *errstr, int rc)
{
    char blockname[32];
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);

    if (rc < 0) {
        cli_out("%s%s ", BCMA_CLI_CONFIG_ERROR_STR, errstr);
    }

    cli_out("%s %s", prefix, sid->addr.name);
    if (t >= 0) {
        cli_out("{%d}", t);
    }
    if (a >= 0) {
        cli_out("[%d]", a);
    }
    if (b >= 0) {
        cli_out(".%s", bcma_bcmbd_block_name(unit, b, blockname,
                                             sizeof(blockname)));
    }
    cli_out(" [0x%04"PRIx32"%08"PRIx32"]", adext, addr);

    if (rc < 0) {
        cli_out(" (%d)", rc);
    }
}

/*
 * Perform table insert/delete/lookup operation and
 * display necessary information.
 */
static int
cmicx_cmd_tblop(int unit, const bcmdrd_symbol_t *symbol,
                bcma_bcmbd_id_t *sid, int block, int pipe,
                uint32_t size, uint32_t *and_masks, uint32_t *or_masks,
                cmicx_tbl_op_t tbl_op)
{
    int rc;
    int index;
    uint32_t adext, i, addr;
    uint32_t  bank_ignore_mask = 0;
    uint32_t data[BCMDRD_MAX_PT_WSIZE] = {0}, resp_data[BCMDRD_MAX_PT_WSIZE];
    bcma_bcmbd_cmicx_tblop_f tblop_func;
    bcmbd_tbl_resp_info_t resp_info;
    char *err_str, *ok_str;

    if (size > COUNTOF(data)) {
        cli_out("%sentity size (%"PRIx32" words) too big\n",
                BCMA_CLI_CONFIG_ERROR_STR, size);
        return -1;
    }

    /* If the table operation failed, output an error */
    if (tbl_op == CMICX_TBL_INSERT) {
        err_str = "table insert failure:";
        ok_str = "entry inserted";
        tblop_func = bcmbd_cmicx_tbl_insert;
    } else if (tbl_op == CMICX_TBL_DELETE) {
        err_str = "table delete failure:";
        ok_str = "entry deleted";
        tblop_func = bcmbd_cmicx_tbl_delete;
    } else if (tbl_op == CMICX_TBL_LOOKUP) {
        err_str = "table lookup failure:";
        ok_str = "entry found";
        tblop_func = bcmbd_cmicx_tbl_lookup;
    } else {
        cli_out("%sunknown table operation\n", BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    /* Default address */
    addr = sid->addr.name32;
    adext = sid->addr.ext32;

    if (pipe >= 0) {
        BCMBD_CMICX_ADEXT_ACCTYPE_SET(adext, pipe);
    }

    /* Calculate the absolute address for this table operation */
    if (block > 0) {
        /* Update address extension with specified block */
        BCMBD_CMICX_ADEXT_BLOCK_SET(adext, block);
        addr = bcmbd_block_port_addr(unit, block, -1, addr, -1);
    }

    /* Set table entry value */
    if (and_masks || or_masks) {
        for (i = 0; i < size; i++) {
            if(and_masks) data[i] &= and_masks[i];
            if(or_masks) data[i] |= or_masks[i];
        }

        /* Perform table operation */
        rc = tblop_func(unit, adext, addr, data, size, bank_ignore_mask,
                        &resp_info, resp_data);

        /* If the table operation failed, output an error */
        if (rc != SHR_E_NONE) {
            print_tbl_op(unit, "entry", sid, 0, block,
                         pipe, 0, adext, addr, err_str, rc);
            cli_out("\n");
            return rc;
        }

        /* Get memory index if no failure */
        index = CMICX_RESP_WORD_INDEX_GET(resp_info.resp_word);

        /* Display returned entry index. */
        cli_out("%s in memroy index: %d\n", ok_str, index);

        if (tbl_op != CMICX_TBL_LOOKUP) {
            return rc;
        }
    } else {
        cli_out("%sHash table key is not specificed.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }

    if (sid->flags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        for (i = 0; i < size; i++) {
            if (resp_data[i]) break;
        }
        if (i >= size) return 0;
    }

    /*
     * If we got here, this was a successful lookup operation.
     * Print out the memory data.
     */
    print_tbl_op(unit, " ", sid, 0, block,
                 pipe, 0, adext, addr, "", rc);
    cli_out(" = ");
    for (i = 0; i < size; i++) {
        cli_out("0x%08"PRIx32" ", resp_data[i]);
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

/*
 * This function iterates over all specified memory blocks in a
 * given sid, performs the request table operation, and displays necessary info.
 */
static int
cmicx_tblops(int unit, const bcmdrd_symbol_t *symbol, bcma_bcmbd_id_t *sid,
             uint32_t size, uint32_t *and_masks, uint32_t *or_masks,
             cmicx_tbl_op_t tbl_op)
{
    int b, bt, t;
    uint32_t pipe_info = 0, pmask = 0;
    int acc_type = -1;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(cinfo);
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

            if (symbol) {
                acc_type = BCMBD_CMICX_BLKACC2ACCTYPE(symbol->info);
                pipe_info = bcmbd_cmicx_pipe_info(unit, sid->addr.name32,
                                                  acc_type, bt, -1);
            }
            pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);
        }

        for (t = sid->pipe.start; t <= sid->pipe.end; t++) {
            /* Loop for base type (if applicable) */
            if (t != -1) {
                /* If this is not a valid pipe value, skip it */
                if (!((1 << t) & pmask)) {
                    continue;
                }
            }

            cmicx_cmd_tblop(unit, symbol, sid, b, t, size, and_masks, or_masks,
                            tbl_op);
        }
    }
    return 0;
}

/*
 * Hash table operation insert/delete/lookup via S-channel.
 */
static int
tbl_symop(int unit, const bcmdrd_symbol_t *symbol, bcma_bcmbd_id_t *sid,
          uint32_t *and_masks, uint32_t *or_masks, cmicx_tbl_op_t tbl_op)
{
    int b, wsize;
    bcma_bcmbd_id_t sid2;
    const bcmdrd_block_t *blkp;
    int blktype, acc_type;
    uint32_t pipe_info;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    assert(symbol);
    assert(cinfo);

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_HASHED)) {
        cli_out("%ssymbol '%s' is not a hashed table\n",
                BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
        return -1;
    }

    /* The indices are always 0 for table operations. */
    sid->addr.start = 0;
    sid->addr.end = 0;

    sid->block.start = 0;
    sid->block.end = 0;

    sid->port.start = 0;
    sid->port.end = 0;

    /*
    * Iterate through all blocks of which this memory is a member
    */
    for (blktype = 0; blktype < cinfo->nblktypes; blktype++) {

        if (!bcma_bcmbd_info_block_type_valid(unit, blktype, symbol->info)) {
            continue;
        }

        /* Need a copy of this SID for this block iteration */
        sal_memcpy(&sid2, sid, sizeof(sid2));

        /*
        * Set block type filter in case identical block types are
        * not contiguous.
        */
        sid2.block.ext32 = (1 << blktype);

        acc_type = BCMBD_CMICX_BLKACC2ACCTYPE(symbol->info);
        pipe_info = bcmbd_cmicx_pipe_info(unit, sid->addr.name32,
                                          acc_type, blktype, -1);

        for (b = 0; b < cinfo->nblocks; b++) {

            /* Get the block pointer for this block */
            blkp = cinfo->blocks + b;
            assert(blkp);

            if (blkp->type != blktype){
                continue;
            }

            /* Setup pipe info */
            if (!sid2.pipe.valid) {
                if (pipe_info > 0) {
                    int num_pipe = 0;
                    int pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);

                    if (pmask > 0) {
                        while (pmask) {
                            num_pipe++;
                            pmask >>= 1;
                        };
                        sid2.pipe.start = 0;
                        sid2.pipe.end = num_pipe - 1;
                    }
                }
            } else {
                if (pipe_info == 0) {
                    sid2.pipe.start = -1;
                    sid2.pipe.end = -1;
                }
            }

            /* specific blocks were indicated. */
            /* Need to convert these to physical blocks */
            sid2.block.start = bcmdrd_chip_block_number(cinfo, blktype,
                                                        sid->block.start);
            sid2.block.end = bcmdrd_chip_block_number(cinfo, blktype,
                                                      sid->block.end);
        }

        /* Skip if we don't have a valid block range by now */
        if (sid2.block.start == -1) {
            continue;
        }

        /* Get dual-pipe access type from symbol flags */
        sid2.addr.ext32 = BCMBD_CMICX_BLKACC2ADEXT(symbol->info);

        wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
        cmicx_tblops(unit, symbol, &sid2, wsize, and_masks, or_masks, tbl_op);
    }

    return 0;
}

/*******************************************************************************
 * Exported functions
 */

int
bcma_bcmbd_cmicx_tbl_insert(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks)
{
    return tbl_symop(unit, symbol, sid, and_masks, or_masks, CMICX_TBL_INSERT);
}

int
bcma_bcmbd_cmicx_tbl_delete(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks)
{
    return tbl_symop(unit, symbol, sid, and_masks, or_masks, CMICX_TBL_DELETE);
}

int
bcma_bcmbd_cmicx_tbl_lookup(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks)
{
    return tbl_symop(unit, symbol, sid, and_masks, or_masks, CMICX_TBL_LOOKUP);
}
