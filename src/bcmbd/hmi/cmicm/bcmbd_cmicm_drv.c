/*! \file bcmbd_cmicm_drv.c
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

#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_cmicm.h>
#include <bcmbd/bcmbd_cmicm_reg.h>
#include <bcmbd/bcmbd_cmicm_mem.h>
#include <bcmbd/bcmbd_cmicm_tbl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

static uint32_t
bcmbd_cmicm_block_port_addr(int block, int lane,
                            uint32_t offset, uint32_t idx)
{
    uint32_t addr;

    if (lane < 0) {
        /* Default block/port calculation for memories */
        addr = offset;
        addr += idx;
    } else {
        /* Default block/port calculation for registers */
        addr = (offset | lane);
        addr += (idx * 0x100);
    }
    return addr;
}

static int
bcmbd_cmicm_reg_block_read(int unit, bcmdrd_sym_info_t *sinfo,
                           bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                           uint32_t *data, size_t wsize)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;
    int blktype, blkinst, blknum, port;
    uint32_t addr, adext;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    assert(sinfo->entry_wsize <= wsize);
    wsize = sinfo->entry_wsize;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_PORT) {
        /* Iterate over blocks until matching port is found */
        port = dyn_info->tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk) == 0) {
                break;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, blktype + 1);
        } while (blktype >= 0);
        if (blktype < 0) {
            return -1;
        }
        blknum = pblk.blknum;
        addr = bcmbd_block_port_addr(unit, blknum, pblk.lane,
                                     sinfo->offset, dyn_info->index);
    } else {
        blkinst = dyn_info->tbl_inst;
        if (blkinst < 0) {
            blkinst = 0;
        }
        blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (blknum < 0) {
            return -1;
        }
        addr = bcmbd_block_port_addr(unit, blknum, -1,
                                     sinfo->offset, dyn_info->index);
    }
    adext = BCMBD_CMICM_BLKACC2ADEXT(sinfo->blktypes);
    BCMBD_CMICM_ADEXT_BLOCK_SET(adext, blknum);
    return bcmbd_cmicm_reg_read(unit, adext, addr, data, wsize);
}

static int
bcmbd_cmicm_reg_block_write(int unit, bcmdrd_sym_info_t *sinfo,
                            bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                            uint32_t *data)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;
    int blktype, blkinst, blknum, port;
    uint32_t wsize;
    uint32_t addr, adext;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    wsize = sinfo->entry_wsize;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_PORT) {
        /* Iterate over blocks until matching port is found */
        port = dyn_info->tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk) == 0) {
                break;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, blktype + 1);
        } while (blktype >= 0);
        if (blktype < 0) {
            return -1;
        }
        blknum = pblk.blknum;
        addr = bcmbd_block_port_addr(unit, blknum, pblk.lane,
                                     sinfo->offset, dyn_info->index);
    } else {
        blkinst = dyn_info->tbl_inst;
        if (blkinst < 0) {
            blkinst = 0;
        }
        blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (blknum < 0) {
            return -1;
        }
        addr = bcmbd_block_port_addr(unit, blknum, -1,
                                     sinfo->offset, dyn_info->index);
    }
    adext = BCMBD_CMICM_BLKACC2ADEXT(sinfo->blktypes);
    BCMBD_CMICM_ADEXT_BLOCK_SET(adext, blknum);
    return bcmbd_cmicm_reg_write(unit, adext, addr, data, wsize);
}

static int
bcmbd_cmicm_mem_block_read(int unit, bcmdrd_sym_info_t *sinfo,
                           bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                           uint32_t *data, size_t wsize)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;
    int blktype, blkinst, blknum, port;
    uint32_t addr, adext;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    assert(sinfo->entry_wsize <= wsize);
    wsize = sinfo->entry_wsize;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_PORT) {
        /* Iterate over blocks until matching port is found */
        port = dyn_info->tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk) == 0) {
                break;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, blktype + 1);
        } while (blktype >= 0);
        if (blktype < 0) {
            return -1;
        }
        blknum = pblk.blknum;
        addr = bcmbd_block_port_addr(unit, blknum, pblk.lane,
                                     sinfo->offset, dyn_info->index);
    } else {
        blkinst = dyn_info->tbl_inst;
        if (blkinst < 0) {
            blkinst = 0;
        }
        blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (blknum < 0) {
            return -1;
        }
        addr = bcmbd_block_port_addr(unit, blknum, -1,
                                     sinfo->offset, dyn_info->index);
    }
    adext = BCMBD_CMICM_BLKACC2ADEXT(sinfo->blktypes);
    BCMBD_CMICM_ADEXT_BLOCK_SET(adext, blknum);
    return bcmbd_cmicm_mem_read(unit, adext, addr, data, wsize);
}

static int
bcmbd_cmicm_mem_block_write(int unit, bcmdrd_sym_info_t *sinfo,
                            bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                            uint32_t *data)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;
    int blktype, blkinst, blknum, port;
    uint32_t wsize;
    uint32_t addr, adext;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    wsize = sinfo->entry_wsize;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_PORT) {
        /* Iterate over blocks until matching port is found */
        port = dyn_info->tbl_inst;
        do {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, &pblk) == 0) {
                break;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, blktype + 1);
        } while (blktype >= 0);
        if (blktype < 0) {
            return -1;
        }
        blknum = pblk.blknum;
        addr = bcmbd_block_port_addr(unit, blknum, pblk.lane,
                                     sinfo->offset, dyn_info->index);
    } else {
        blkinst = dyn_info->tbl_inst;
        if (blkinst < 0) {
            blkinst = 0;
        }
        blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
        if (blknum < 0) {
            return -1;
        }
        addr = bcmbd_block_port_addr(unit, blknum, -1,
                                     sinfo->offset, dyn_info->index);
    }
    adext = BCMBD_CMICM_BLKACC2ADEXT(sinfo->blktypes);
    BCMBD_CMICM_ADEXT_BLOCK_SET(adext, blknum);
    return bcmbd_cmicm_mem_write(unit, adext, addr, data, wsize);
}

int
bcmbd_cmicm_read(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data, size_t wsize)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return SHR_E_PARAM;
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMMAPPED) {
        uint32_t addr = sinfo->offset + (dyn_info->index * sizeof(uint32_t));
        return BCMDRD_DEV_READ32(unit, addr, data);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        return bcmbd_cmicm_reg_block_read(unit, sinfo, dyn_info, ovrr_info,
                                          data, wsize);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        return bcmbd_cmicm_mem_block_read(unit, sinfo, dyn_info, ovrr_info,
                                          data, wsize);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_cmicm_write(int unit, bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                  uint32_t *data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t offset;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return SHR_E_PARAM;
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMMAPPED) {
        offset = sinfo->offset + (dyn_info->index * sizeof(uint32_t));
        return BCMDRD_DEV_WRITE32(unit, offset, *data);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        return bcmbd_cmicm_reg_block_write(unit, sinfo, dyn_info, ovrr_info,
                                           data);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        return bcmbd_cmicm_mem_block_write(unit, sinfo, dyn_info, ovrr_info,
                                           data);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_cmicm_insert(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    const bcmdrd_chip_info_t *cinfo;
    int blktype, blkinst, blknum;
    uint32_t addr, adext;
    uint32_t bank_ignore_mask = 0;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return SHR_E_PARAM;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    assert(sinfo->entry_wsize <= wsize);
    wsize = sinfo->entry_wsize;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    blkinst = dyn_hash_info->tbl_inst;
    if (blkinst < 0) {
        blkinst = 0;
    }
    blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
    if (blknum < 0) {
        return -1;
    }

    addr = bcmbd_block_port_addr(unit, blknum, -1, sinfo->offset, 0);
    

    adext = BCMBD_CMICM_BLKACC2ADEXT(sinfo->blktypes);
    BCMBD_CMICM_ADEXT_BLOCK_SET(adext, blknum);

    return bcmbd_cmicm_tbl_insert(unit, adext, addr, data, wsize,
                                  bank_ignore_mask, resp_info, resp_data);
}

int
bcmbd_cmicm_delete(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    const bcmdrd_chip_info_t *cinfo;
    int blktype, blkinst, blknum;
    uint32_t addr, adext;
    uint32_t bank_ignore_mask = 0;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return SHR_E_PARAM;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    assert(sinfo->entry_wsize <= wsize);
    wsize = sinfo->entry_wsize;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    blkinst = dyn_hash_info->tbl_inst;
    if (blkinst < 0) {
        blkinst = 0;
    }
    blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
    if (blknum < 0) {
        return -1;
    }

    addr = bcmbd_block_port_addr(unit, blknum, -1, sinfo->offset, 0);
    

    adext = BCMBD_CMICM_BLKACC2ADEXT(sinfo->blktypes);
    BCMBD_CMICM_ADEXT_BLOCK_SET(adext, blknum);

    return bcmbd_cmicm_tbl_delete(unit, adext, addr, data, wsize,
                                  bank_ignore_mask, resp_info, resp_data);
}

int
bcmbd_cmicm_lookup(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    const bcmdrd_chip_info_t *cinfo;
    int blktype, blkinst, blknum;
    uint32_t addr, adext;
    uint32_t bank_ignore_mask = 0;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, sinfo))) {
        return SHR_E_PARAM;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    assert(sinfo->entry_wsize <= wsize);
    wsize = sinfo->entry_wsize;

    blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);

    blkinst = dyn_hash_info->tbl_inst;
    if (blkinst < 0) {
        blkinst = 0;
    }
    blknum = bcmdrd_chip_block_number(cinfo, blktype, blkinst);
    if (blknum < 0) {
        return -1;
    }

    addr = bcmbd_block_port_addr(unit, blknum, -1, sinfo->offset, 0);
    

    adext = BCMBD_CMICM_BLKACC2ADEXT(sinfo->blktypes);
    BCMBD_CMICM_ADEXT_BLOCK_SET(adext, blknum);

    return bcmbd_cmicm_tbl_lookup(unit, adext, addr, data, wsize,
                                  bank_ignore_mask, resp_info, resp_data);
}

int
bcmbd_cmicm_drv_init(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_FAIL;
    }

    /* Look for device-specific address calculation, otherwise use default */
    bd->block_port_addr = bcmdrd_dev_block_port_addr_func(unit);
    if (bd->block_port_addr == NULL) {
        bd->block_port_addr = bcmbd_cmicm_block_port_addr;
    }

    bd->pt_read = bcmbd_cmicm_read;
    bd->pt_write = bcmbd_cmicm_write;
    bd->pt_insert = bcmbd_cmicm_insert;
    bd->pt_delete = bcmbd_cmicm_delete;
    bd->pt_lookup = bcmbd_cmicm_lookup;

    return 0;
}
