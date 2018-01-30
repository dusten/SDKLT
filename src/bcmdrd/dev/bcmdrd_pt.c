/*! \file bcmdrd_pt.c
 *
 * BCMDRD PT routines.
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

#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

static const bcmdrd_symbol_t *
symbol_get(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    return bcmdrd_sym_info_get(symbols, sid, NULL);
}

int
bcmdrd_pt_info_get(int unit, bcmdrd_sid_t sid,
                   bcmdrd_sym_info_t *sinfo)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_info_get(symbols, sid, sinfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_info_get_by_name(int unit, const char *sname,
                           bcmdrd_sym_info_t *sinfo)
{
    const bcmdrd_symbols_t *symbols;
    bcmdrd_sid_t sid;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_symbols_find(sname, symbols, &sid) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    if (bcmdrd_sym_info_get(symbols, sid, sinfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_acctype_get(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_chip_info_t *cinfo;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    return bcmdrd_chip_acctype_get(cinfo, sid);
}

int
bcmdrd_pt_blktype_get(int unit, bcmdrd_sid_t sid, int blktype)
{
    const bcmdrd_chip_info_t *cinfo;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    return bcmdrd_chip_blktype_get(cinfo, sid, blktype);
}

int
bcmdrd_pt_index_max(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return -1;
    }

    return BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
}

int
bcmdrd_pt_index_min(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return -1;
    }

    return BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
}

static void
pt_pipe_info_get(int unit, bcmdrd_sym_info_t *sinfo,
                 uint32_t *pmask, int *linst)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = sinfo->offset;
    pi->acctype = bcmdrd_pt_acctype_get(unit, sinfo->sid);
    pi->blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
    if (pmask) {
        *pmask = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_PMASK);
    }
    if (linst) {
        *linst = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);
    }
}

static bool
pt_baseidx_valid(int unit, bcmdrd_sym_info_t *sinfo, int baseidx)
{
    uint32_t idx_invalid;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = sinfo->offset;
    pi->acctype = bcmdrd_pt_acctype_get(unit, sinfo->sid);
    pi->blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
    pi->baseidx = baseidx;
    idx_invalid = bcmdrd_dev_pipe_info(unit, pi,
                                       BCMDRD_PIPE_INFO_TYPE_BASEIDX_INVALID);
    return (idx_invalid == 0);
}

bool
bcmdrd_pt_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    symbol = bcmdrd_sym_info_get(symbols, sid, sinfo);
    if (symbol == NULL) {
        return false;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return false;
    }

    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return false;
    }

    if (tbl_inst >= 0) {
        uint32_t pmask = 0;

        pt_pipe_info_get(unit, sinfo, &pmask, NULL);
        if (pmask != 0) {
            /* Check for holes in per-pipe PTs */
            if ((pmask & (1 << tbl_inst)) == 0) {
                return false;
            }
        } else if (bcmdrd_pt_is_port_reg(unit, sid)) {
            /* Check for holes in port-based PTs */
            if (!pt_baseidx_valid(unit, sinfo, tbl_inst)) {
                return false;
            }
            return bcmdrd_chip_port_reg_valid(cinfo, sid, tbl_inst, index);
        } else if (bcmdrd_pt_is_soft_port_reg(unit, sid)) {
            int blktype;
            const bcmdrd_block_t *blkp;
            bcmdrd_pbmp_t pbmp;
            bcmdrd_port_num_domain_t pnd;

            blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
            blkp = bcmdrd_chip_block(cinfo, blktype, tbl_inst);
            if (blkp == NULL) {
                return false;
            }
            /* Check for holes in soft-port-based PTs */
            BCMDRD_PBMP_ASSIGN(pbmp, blkp->pbmps);
            pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
            BCMDRD_PBMP_AND(pbmp, cinfo->valid_pbmps[pnd]);
            if (BCMDRD_PBMP_IS_NULL(pbmp)) {
                return false;
            }
        }
    }

    return true;
}

uint32_t
bcmdrd_pt_entry_size(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return 0;
    }

    return BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
}

uint32_t
bcmdrd_pt_entry_wsize(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;
    uint32_t size;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return 0;
    }

    size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
    return BCMDRD_BYTES2WORDS(size);
}

bcmdrd_port_num_domain_t
bcmdrd_pt_port_num_domain(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_chip_info_t *cinfo;
    int blktype;

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_PARAM;
    }

    return bcmdrd_chip_port_num_domain(cinfo, sid, blktype);
}

int
bcmdrd_pt_name_to_sid(int unit, const char *name, bcmdrd_sid_t *sid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_INIT;
    }
    symbol = bcmdrd_symbols_find(name, symbols, sid);
    if (symbol == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

const char *
bcmdrd_pt_sid_to_name(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return NULL;
    }
    if (sid >= symbols->size) {
        return NULL;
    }
    return symbols->symbols[sid].name;
}

int
bcmdrd_pt_sid_list_get(int unit, size_t list_max,
                       bcmdrd_sid_t *sid_list, size_t *num_sid)
{
    const bcmdrd_symbols_t *symbols;
    size_t idx;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_INIT;
    }
    *num_sid = symbols->size;
    if (list_max == 0) {
        return SHR_E_NONE;
    }
    if (sid_list == NULL) {
        return SHR_E_PARAM;
    }
    for (idx = 0; idx < symbols->size; idx++) {
        if (idx >= list_max) {
            break;
        }
        sid_list[idx] = idx;
    }
    return SHR_E_NONE;
}

const uint32_t *
bcmdrd_pt_default_value_get(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return NULL;
    }
    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return NULL;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        return bcmdrd_sym_reg_resetval_get(symbols, symbol);
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        return bcmdrd_sym_mem_null_ent_get(symbols, symbol);
    }
    return NULL;
}

bool
bcmdrd_pt_default_value_is_nonzero(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(unit, 0);
    const bcmdrd_symbol_t *symbol = bcmdrd_sym_info_get(symbols, sid, NULL);

    return bcmdrd_sym_default_value_is_nonzero(symbols, symbol);
}

bool
bcmdrd_pt_is_valid(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    if ((symbol->flags &
         (BCMDRD_SYMBOL_FLAG_REGISTER | BCMDRD_SYMBOL_FLAG_MEMORY)) == 0) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_OVERLAY) == 0);
}

bool
bcmdrd_pt_is_reg(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) != 0);
}

bool
bcmdrd_pt_is_mem(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) != 0);
}

bool
bcmdrd_pt_is_port_reg(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_PORT) != 0);
}

bool
bcmdrd_pt_is_soft_port_reg(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_SOFT_PORT) != 0);
}

bool
bcmdrd_pt_is_readonly(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_READONLY) != 0);
}

bool
bcmdrd_pt_is_notest(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_NOTEST) != 0);
}

bool
bcmdrd_pt_is_counter(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_COUNTER) != 0);
}

bool
bcmdrd_pt_is_cacheable(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_CACHEABLE) != 0);
}

bool
bcmdrd_pt_attr_is_cam(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_CAM) != 0);
}

bool
bcmdrd_pt_attr_is_hashed(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_HASHED) != 0);
}

bool
bcmdrd_pt_attr_is_fifo(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_FIFO) != 0);
}

int
bcmdrd_pt_num_tbl_inst(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    const bcmdrd_chip_info_t *cinfo;
    int tbl_num = 0;
    int linst = 0;
    uint32_t pmask = 0;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return SHR_E_PARAM;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_PARAM;
    }

    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return 0;
    }

    /* If access type is UNIQUE, tbl_num is the number of unique pipes */
    pt_pipe_info_get(unit, sinfo, &pmask, &linst);
    while (pmask) {
        tbl_num++;
        pmask >>= 1;
    }

    /*
     * Check for port-based registers if access type
     * is not UNIQUE (tbl_num == 0)
     */
    if (tbl_num == 0 && bcmdrd_pt_is_port_reg(unit, sid)) {
        int port;
        bcmdrd_pbmp_t pbmps;

        if (bcmdrd_chip_port_reg_pbmp(cinfo, sid, &pbmps) == 0) {
            /* Get maximum port number from pbmp */
            if (!BCMDRD_PBMP_IS_NULL(pbmps)) {
                for (port = BCMDRD_CONFIG_MAX_PORTS - 1; port >= 0; port--) {
                    if (BCMDRD_PBMP_MEMBER(pbmps, port)) {
                        tbl_num = port + 1;
                        break;
                    }
                }
            }
        }
    }

    /* Check for per-block symbols if tbl_num == 0 */
    if (tbl_num == 0 && bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        int blktype, idx;

        /* Soft-port PTs should have only one block type */
        if ((blktype = bcmdrd_pt_blktype_get(unit, sid, 0)) >= 0) {
            bcmdrd_pbmp_t blktype_pbmps;

            /* Get the valid port bitmap of this block type. */
            bcmdrd_chip_blktype_pbmp(cinfo, blktype, &blktype_pbmps);

            /* Start from the back */
            for (idx = cinfo->nblocks - 1; idx >= 0; idx--) {
                const bcmdrd_block_t *blkp = &cinfo->blocks[idx];

                if (blkp->type != blktype) {
                    continue;
                }

                /* Skip trailing block instances with empty port bitmap */
                if (tbl_num == 0) {
                    bcmdrd_pbmp_t pbmps;

                    /* Check the block port bitmap with the valid port bitmap */
                    BCMDRD_PBMP_ASSIGN(pbmps, blktype_pbmps);
                    BCMDRD_PBMP_AND(pbmps, blkp->pbmps);
                    if (BCMDRD_PBMP_IS_NULL(pbmps)) {
                        continue;
                    }
                }
                tbl_num++;
            }
        }
    }

    /* Check for basetype instances if tbl_num == 0 */
    if (tbl_num == 0 && linst > 1) {
        tbl_num = linst;
    }

    return ((tbl_num == 0) ? 1 : tbl_num);
}

int
bcmdrd_pt_tbl_inst_from_port(int unit, bcmdrd_sid_t sid, int port)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return -1;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return -1;
    }

    /* Check if the port is valid for IPORT/EPORT base types */
    if (!pt_baseidx_valid(unit, sinfo, port)) {
        return -1;
    }

    if (bcmdrd_pt_is_port_reg(unit, sid)) {
        bcmdrd_pbmp_t pbmps;

        if (bcmdrd_chip_port_reg_pbmp(cinfo, sid, &pbmps) == 0) {
            if (BCMDRD_PBMP_MEMBER(pbmps, port)) {
                return port;
            }
        }
    } else if (bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        bcmdrd_pblk_t drd_pblk, *pblk = &drd_pblk;
        int blktype;

        sal_memset(pblk, 0, sizeof(*pblk));

        if ((blktype = bcmdrd_pt_blktype_get(unit, sid, 0)) >= 0) {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, pblk) == 0) {
                return pblk->inst;
            }
        }
    }

    return -1;
}

int
bcmdrd_pt_port_block_lane_get(int unit, bcmdrd_sid_t sid, int port,
                              int *blknum, int *lane)
{
    int btype = -1;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    bcmdrd_pblk_t drd_pblk, *pblk = &drd_pblk;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return SHR_E_NOT_FOUND;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_UNIT;
    }

    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return SHR_E_PARAM;
    }

    /* Check if the port is valid for IPORT/EPORT base types */
    if (!pt_baseidx_valid(unit, sinfo, port)) {
        return SHR_E_PORT;
    }

    sal_memset(pblk, 0, sizeof(*pblk));

    while ((btype = bcmdrd_pt_blktype_get(unit, sid, btype + 1)) >= 0) {
        if (bcmdrd_chip_port_block(cinfo, port, btype, pblk) == 0) {
            break;
        }
    }
    if (btype < 0) {
        return SHR_E_PORT;
    }

    if (blknum != NULL) {
        *blknum = bcmdrd_chip_block_number(cinfo, btype, pblk->inst);
    }
    if (lane != NULL) {
        *lane = pblk->lane;
    }

    return SHR_E_NONE;
}

int
bcmdrd_pt_num_pipe_inst(int unit, bcmdrd_sid_t sid)
{
    int pipe_inst;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    if (!bcmdrd_dev_exists(unit)) {
        return -1;
    }

    sal_memset(pi, 0, sizeof(*pi));
    pi->blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    pipe_inst = bcmdrd_dev_pipe_info(unit, pi,
                                     BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);

    return (pipe_inst == 0) ? 1 : pipe_inst;
}

int
bcmdrd_pt_fid_list_get(int unit, bcmdrd_sid_t sid, size_t list_max,
                       bcmdrd_fid_t *fid_list, size_t *num_fid)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_fid_list_get(symbols, sid, list_max, fid_list, num_fid) < 0) {
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                         bcmdrd_sym_field_info_t *finfo)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_field_info_get(symbols, sid, fid, finfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_field_info_get_by_name(int unit, bcmdrd_sid_t sid, const char *fname,
                                 bcmdrd_sym_field_info_t *finfo)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_field_info_get_by_name(symbols, sid, fname, finfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

bool
bcmdrd_pt_field_is_readonly(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    
    return false;
}

int
bcmdrd_pt_field_name_to_fid(int unit, bcmdrd_sid_t sid,
                            const char *fname, bcmdrd_fid_t *fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get_by_name(unit, sid, fname, &finfo);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    *fid = finfo.fid;
    return SHR_E_NONE;
}

int
bcmdrd_pt_field_maxbit(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return -1;
    }
    return finfo.maxbit;
}

int
bcmdrd_pt_field_minbit(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return -1;
    }
    return finfo.minbit;
}

bool
bcmdrd_pt_field_is_counter(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return false;
    }
    return ((finfo.flags & BCMDRD_SYMBOL_FIELD_FLAG_COUNTER) != 0);
}

bool
bcmdrd_pt_field_is_key(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return false;
    }
    return ((finfo.flags & BCMDRD_SYMBOL_FIELD_FLAG_KEY) != 0);
}

bool
bcmdrd_pt_field_is_mask(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return false;
    }
    return ((finfo.flags & BCMDRD_SYMBOL_FIELD_FLAG_MASK) != 0);
}

int
bcmdrd_pt_field_get(int unit, bcmdrd_sid_t sid, uint32_t *sbuf,
                    bcmdrd_fid_t fid, uint32_t *fbuf)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    uint32_t size;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    symbol = bcmdrd_sym_field_info_get(symbols, sid, fid, &finfo);
    if (symbol == NULL) {
        return -1;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
        bcmdrd_field_be_get(sbuf, BCMDRD_BYTES2WORDS(size),
                            finfo.minbit, finfo.maxbit, fbuf);
    } else {
        bcmdrd_field_get(sbuf, finfo.minbit, finfo.maxbit, fbuf);
    }
    return 0;
}

int
bcmdrd_pt_field_set(int unit, bcmdrd_sid_t sid, uint32_t *sbuf,
                    bcmdrd_fid_t fid, uint32_t *fbuf)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    uint32_t size;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    symbol = bcmdrd_sym_field_info_get(symbols, sid, fid, &finfo);
    if (symbol == NULL) {
        return -1;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
        bcmdrd_field_be_set(sbuf, BCMDRD_BYTES2WORDS(size),
                            finfo.minbit, finfo.maxbit, fbuf);
    } else {
        bcmdrd_field_set(sbuf, finfo.minbit, finfo.maxbit, fbuf);
    }
    return 0;
}

#endif
