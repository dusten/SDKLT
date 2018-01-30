/*! \file bcmdrd_chip.c
 *
 * <description>
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
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_chip.h>

/*
 * Get port index within a block from the top level port index.
 */
static int
port_block_lane_get(const bcmdrd_block_t *blkp,
                    bcmdrd_port_num_domain_t pnd, int port)
{
    int p, lane = 0;

    if (pnd != BCMDRD_PND_PHYS) {
        /* Assign port index to lane for non-physical ports */
        if (BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            return port;
        }
        return -1;
    }

    /*
     * To get the correct lane index, the block port bitmap for physical ports
     * should contain no holes.
     */
    BCMDRD_PBMP_ITER(blkp->pbmps, p) {
        if (p == port) {
            return lane;
        }
        lane++;
    }
    return -1;
}

/*
 * Convert a block type mask (with a single bit set) to an enunmerated
 * block type value. Can also be expressed as 'val = log2(mask)'.
 */
static int
blktype_from_mask(uint32_t blktypes, int blktype)
{
    do {
        if ((1 << blktype) & blktypes) {
            return blktype;
        }
    } while (++blktype < 32);

    return -1;
}

int
bcmdrd_chip_acctype_get(const bcmdrd_chip_info_t *cinfo, int enum_val)
{
    const bcmdrd_symbol_t *symbol;

    if (cinfo == NULL) {
        return -1;
    }

    symbol = bcmdrd_sym_info_get(cinfo->symbols, enum_val, NULL);
    if (symbol == NULL) {
        return -1;
    }

    return BCMDRD_SYM_INFO_ACCTYPE(symbol->info);
}

int
bcmdrd_chip_blktype_get(const bcmdrd_chip_info_t *cinfo, int enum_val,
                        int blktype)
{
    const bcmdrd_symbol_t *symbol;
    uint32_t blkbmp;

    if (cinfo == NULL) {
        return -1;
    }

    symbol = bcmdrd_sym_info_get(cinfo->symbols, enum_val, NULL);
    if (symbol == NULL) {
        return -1;
    }

    blkbmp = BCMDRD_SYM_INFO_BLKBMP(symbol->info);
    return blktype_from_mask(blkbmp, blktype);
}

int
bcmdrd_chip_port_block(const bcmdrd_chip_info_t *cinfo,
                       int port, int blktype, bcmdrd_pblk_t *pblk)
{
    /* Get the block association for a port number */
    int idx;
    int lane, inst = 0;
    bcmdrd_port_num_domain_t pnd;

    pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return -1;
    }

    /* Foreach block in the chip */
    for (idx = 0; idx < cinfo->nblocks; idx++) {
        const bcmdrd_block_t *blkp = &cinfo->blocks[idx];

        if (blkp->type != blktype) {
            continue;
        }

        if (BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            lane = port_block_lane_get(blkp, pnd, port);
            if (lane >= 0) {
                pblk->type = blkp->type;
                pblk->inst = inst;
                pblk->blknum = blkp->blknum;
                pblk->lane = lane;
                return 0;
            }
        }
        inst++;
    }
    return -1;
}

int
bcmdrd_chip_port_number(const bcmdrd_chip_info_t *cinfo, int blknum, int lane)
{
    int idx, port;
    bcmdrd_port_num_domain_t pnd;

    if (cinfo == NULL || lane < 0) {
        return -1;
    }

    /* Foreach block in the chip */
    for (idx = 0; idx < cinfo->nblocks; idx++) {
        const bcmdrd_block_t *blkp = &cinfo->blocks[idx];

        if (blkp->blknum != blknum) {
            continue;
        }

        pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);

        BCMDRD_PBMP_ITER(blkp->pbmps, port) {
            if (port_block_lane_get(blkp, pnd, port) == lane) {
                if (BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
                    return port;
                }
                return -1;
            }
        }
    }
    return -1;
}

const bcmdrd_block_t *
bcmdrd_chip_block(const bcmdrd_chip_info_t *cinfo, int blktype, int blkinst)
{
    /* Calculate the physical block number for a given blocktype instance */
    int idx;

    if (cinfo == NULL) {
        return NULL;
    }

    for (idx = 0; idx < cinfo->nblocks; idx++) {
        if (cinfo->blocks[idx].type == blktype) {
            if (blkinst == 0) {
                return &cinfo->blocks[idx];
            }
            blkinst--;
        }
    }
    /* The requested block instance is not valid */
    return NULL;
}

int
bcmdrd_chip_block_number(const bcmdrd_chip_info_t *cinfo,
                         int blktype, int blkinst)
{
    const bcmdrd_block_t *blkp;

    blkp = bcmdrd_chip_block(cinfo, blktype, blkinst);
    if (blkp == NULL) {
        return -1;
    }

    return blkp->blknum;
}

int
bcmdrd_chip_blktype_pbmp(const bcmdrd_chip_info_t *cinfo, int blktype,
                         bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_block_t *blkp;
    bcmdrd_port_num_domain_t pnd;
    int idx;

    if (cinfo == NULL || pbmp == NULL) {
        return -1;
    }

    blkp = cinfo->blocks;

    assert(blkp);

    BCMDRD_PBMP_CLEAR(*pbmp);

    /* Iterate over all physical blocks of this type */
    for (idx = 0; idx < cinfo->nblocks; idx++, blkp++) {
        if (blkp->type == blktype) {
            BCMDRD_PBMP_OR(*pbmp, blkp->pbmps);
        }
    }

    pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
    BCMDRD_PBMP_AND(*pbmp, cinfo->valid_pbmps[pnd]);

    return 0;
}

/*
 * Calculate the blocktype and instance for a physical block number
 */
int
bcmdrd_chip_block_type(const bcmdrd_chip_info_t *cinfo, int blknum,
                       int *blktype, int *blkinst)
{
    int idx;

    if (cinfo == NULL) {
        return -1;
    }

    /* Get block type */
    for (idx = 0; idx < cinfo->nblocks; idx++) {
        const bcmdrd_block_t *blkp = cinfo->blocks + idx;

        if (blkp->blknum == blknum) {
            if (blktype) {
                *blktype = blkp->type;
            }
            if (blkinst) {
                /* Get instance if requested */
                *blkinst = 0;
                while (idx > 0) {
                    idx--;
                    blkp--;
                    if (blkp->type == *blktype) {
                        (*blkinst)++;
                    }
                }
            }
            return 0;
        }
    }
    return -1;
}

bcmdrd_port_num_domain_t
bcmdrd_chip_port_num_domain(const bcmdrd_chip_info_t *cinfo, int enum_val,
                            int blktype)
{
    if (cinfo && cinfo->port_num_domain) {
        return cinfo->port_num_domain(enum_val, blktype);
    }

    return BCMDRD_PND_PHYS;
}

int
bcmdrd_chip_valid_pbmp(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp)
{
    if (cinfo == NULL || pbmp == NULL) {
        return -1;
    }

    if (pnd >= BCMDRD_PND_COUNT) {
        return -1;
    }

    BCMDRD_PBMP_ASSIGN(*pbmp, cinfo->valid_pbmps[pnd]);

    return 0;
}

int
bcmdrd_chip_pipe_pbmp(const bcmdrd_chip_info_t *cinfo, int pipe_no,
                      bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp)
{
    int port, pipe;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    if (cinfo == NULL || pbmp == NULL) {
        return -1;
    }

    if (pnd >= BCMDRD_PND_COUNT) {
        return -1;
    }

    BCMDRD_PBMP_CLEAR(*pbmp);

    sal_memset(pi, 0, sizeof(*pi));
    pi->pnd = pnd;

    BCMDRD_PBMP_ITER(cinfo->valid_pbmps[pnd], port) {
        pi->port = port;
        pipe = bcmdrd_chip_pipe_info(cinfo, pi,
                                     BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT);
        if (pipe == pipe_no) {
            BCMDRD_PBMP_PORT_ADD(*pbmp, port);
        }
    }

    return 0;
}

bool
bcmdrd_chip_port_valid(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, int port)
{
    if (cinfo == NULL) {
        return false;
    }

    if (pnd >= BCMDRD_PND_COUNT) {
        return false;
    }

    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return false;
    }

    return true;
}

int
bcmdrd_chip_port_reg_pbmp(const bcmdrd_chip_info_t *cinfo,
                          int enum_val, bcmdrd_pbmp_t *pbmp)
{
    bcmdrd_port_num_domain_t pnd;
    int bt;

    if (cinfo == NULL) {
        return -1;
    }

    bt = bcmdrd_chip_blktype_get(cinfo, enum_val, 0);
    if (bt < 0) {
        return -1;
    }

    pnd = bcmdrd_chip_port_num_domain(cinfo, enum_val, bt);
    if (pnd == bcmdrd_chip_port_num_domain(cinfo, -1, bt)) {
        bcmdrd_chip_blktype_pbmp(cinfo, bt, pbmp);
        while ((bt = bcmdrd_chip_blktype_get(cinfo, enum_val, bt + 1)) >= 0) {
            bcmdrd_pbmp_t blkpbmp;

            assert(pnd == bcmdrd_chip_port_num_domain(cinfo, -1, bt));
            bcmdrd_chip_blktype_pbmp(cinfo, bt, &blkpbmp);
            BCMDRD_PBMP_OR(*pbmp, blkpbmp);
        }
    } else {
        bcmdrd_chip_valid_pbmp(cinfo, pnd, pbmp);
    }

    return 0;
}

bool
bcmdrd_chip_port_reg_valid(const bcmdrd_chip_info_t *cinfo,
                           int enum_val, int port, int regidx)
{
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_symbol_t *symbol;
    int encoding, maxidx;

    if (cinfo == NULL) {
        return false;
    }

    symbol = bcmdrd_sym_info_get(cinfo->symbols, enum_val, NULL);
    if (symbol == NULL) {
        return false;
    }

    if (bcmdrd_chip_port_reg_pbmp(cinfo, enum_val, &pbmp) < 0) {
        return false;
    }
    if (!BCMDRD_PBMP_MEMBER(pbmp, port)) {
        return false;
    }

    encoding = BCMDRD_SYMBOL_INDEX_ENC_GET(symbol->index);
    maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
    if (encoding || maxidx) {
        if (bcmdrd_chip_reg_index_valid(cinfo, port, regidx < 0 ? 0 : regidx,
                                        encoding, maxidx) == 0) {
            return false;
        }
    }

    return true;
}

uint32_t
bcmdrd_chip_mem_maxidx(const bcmdrd_chip_info_t *cinfo, int enum_val,
                       uint32_t maxidx)
{
    if (cinfo && cinfo->mem_maxidx) {
        maxidx = cinfo->mem_maxidx(enum_val, maxidx);
    }

    return maxidx;
}

int
bcmdrd_chip_reg_index_valid(const bcmdrd_chip_info_t *cinfo, int port,
                            int regidx, int encoding, int regidx_max)
{
    bcmdrd_numel_info_t *numel_info = cinfo->numel_info;
    bcmdrd_numel_range_t *numel_range;
    int idx;
    int *range_id;

    if (encoding > 0 && numel_info != NULL) {
        /* The first range ID of encoding 0 is the number of valid encodings */
        if (encoding < numel_info->encodings[0].range_id[0]) {
            range_id = numel_info->encodings[encoding].range_id;
            idx = 0;
            while (range_id[idx] >= 0) {
                numel_range = &numel_info->chip_ranges[range_id[idx]];
                if (regidx >= numel_range->min && regidx <= numel_range->max
                    && BCMDRD_PBMP_MEMBER(numel_range->pbmp, port)) {
                    return 1;
                }
                idx++;
            }
        }
    } else if (regidx <= regidx_max || regidx_max < 0) {
        return 1;
    }
    return 0;
}

uint32_t
bcmdrd_chip_pipe_info(const bcmdrd_chip_info_t *cinfo,
                      bcmdrd_pipe_info_t *pi, bcmdrd_pipe_info_type_t pi_type)
{
    if (cinfo && cinfo->pipe_info) {
        return cinfo->pipe_info(pi, pi_type);
    }

    return 0;
}
