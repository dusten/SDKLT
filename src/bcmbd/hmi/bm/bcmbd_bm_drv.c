/*! \file bcmbd_bm_drv.c
 *
 * Top-level APIs for BM access.
 *
 * The primary PT API maps directly to the BM API, so the port/block
 * access functions are only to allow us to work with the built-in
 * simulator.
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

#include <sal/sal_assert.h>
#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_simhook.h>
#include <bcmbd/bcmbd_bm.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt.h>

/* BM IPC */
#include "bm_rpc_intf.h"

static char *bm_srv_port;
static char *bm_srv_host;
static struct bm_rpc_intf_s *bm_srv_handle;

/* Log source for this component */
#define BSL_LOG_MODULE                  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * BM PT interface for symbol read/write.
 */

static int
bcmbd_bm_reg_read(int unit, uint32_t adext, uint32_t addr,
                  uint32_t *data, size_t wsize)
{
    /* Simulator hooks */
    if (bcmbd_simhook_read) {
        return bcmbd_simhook_read(unit, adext | BCMBD_SIM_SOC_REG, addr,
                                  data, BCMDRD_WORDS2BYTES(wsize));
    }

    return SHR_E_UNAVAIL;
}

static int
bcmbd_bm_reg_write(int unit, uint32_t adext, uint32_t addr,
                   uint32_t *data, size_t wsize)
{
    /* Simulator hooks */
    if (bcmbd_simhook_write) {
        return bcmbd_simhook_write(unit, adext | BCMBD_SIM_SOC_REG, addr,
                                   data, BCMDRD_WORDS2BYTES(wsize));
    }

    return SHR_E_UNAVAIL;
}

static int
bcmbd_bm_mem_read(int unit, uint32_t adext, uint32_t addr,
                  uint32_t *data, size_t wsize)
{
    /* Simulator hooks */
    if (bcmbd_simhook_read) {
        return bcmbd_simhook_read(unit, adext | BCMBD_SIM_SOC_MEM, addr,
                                  data, BCMDRD_WORDS2BYTES(wsize));
    }

    return SHR_E_UNAVAIL;
}

static int
bcmbd_bm_mem_write(int unit, uint32_t adext, uint32_t addr,
                   uint32_t *data, size_t wsize)
{
    /* Simulator hooks */
    if (bcmbd_simhook_write) {
        return bcmbd_simhook_write(unit, adext | BCMBD_SIM_SOC_MEM, addr,
                                   data, BCMDRD_WORDS2BYTES(wsize));
    }

    return SHR_E_UNAVAIL;
}

static uint32_t
bcmbd_bm_block_port_addr(int block, int lane,
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
bcmbd_bm_block_addr(int unit, bcmdrd_sym_info_t *sinfo,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    uint32_t *addr)
{
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_pblk_t pblk;
    int blktype, blkinst, blknum, port;

    assert(addr);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

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
        *addr = bcmbd_block_port_addr(unit, blknum, pblk.lane,
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
        *addr = bcmbd_block_port_addr(unit, blknum, -1,
                                      sinfo->offset, dyn_info->index);
    }
    return 0;
}

static int
bcmbd_bm_reg_block_read(int unit, bcmdrd_sym_info_t *sinfo,
                        bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                        uint32_t *data, size_t wsize)
{
    uint32_t addr, adext;

    assert(sinfo->entry_wsize <= wsize);
    wsize = sinfo->entry_wsize;

    if (bcmbd_bm_block_addr(unit, sinfo, dyn_info, &addr) < 0) {
        return -1;
    }
    adext = 0;

    return bcmbd_bm_reg_read(unit, adext, addr, data, wsize);
}

static int
bcmbd_bm_reg_block_write(int unit, bcmdrd_sym_info_t *sinfo,
                         bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                         uint32_t *data)
{
    uint32_t wsize;
    uint32_t addr, adext;

    wsize = sinfo->entry_wsize;

    if (bcmbd_bm_block_addr(unit, sinfo, dyn_info, &addr) < 0) {
        return -1;
    }
    adext = 0;

    return bcmbd_bm_reg_write(unit, adext, addr, data, wsize);
}

static int
bcmbd_bm_mem_block_read(int unit, bcmdrd_sym_info_t *sinfo,
                        bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                        uint32_t *data, size_t wsize)
{
    uint32_t addr, adext;

    assert(sinfo->entry_wsize <= wsize);
    wsize = sinfo->entry_wsize;

    if (bcmbd_bm_block_addr(unit, sinfo, dyn_info, &addr) < 0) {
        return -1;
    }
    adext = 0;

    return bcmbd_bm_mem_read(unit, adext, addr, data, wsize);
}

static int
bcmbd_bm_mem_block_write(int unit, bcmdrd_sym_info_t *sinfo,
                         bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                         uint32_t *data)
{
    uint32_t wsize;
    uint32_t addr, adext;

    wsize = sinfo->entry_wsize;

    if (bcmbd_bm_block_addr(unit, sinfo, dyn_info, &addr) < 0) {
        return -1;
    }
    adext = 0;

    return bcmbd_bm_mem_write(unit, adext, addr, data, wsize);
}

static int
bcmbd_bm_sim_read(int unit, bcmdrd_sid_t sid,
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
        return bcmbd_bm_reg_block_read(unit, sinfo, dyn_info, ovrr_info,
                                       data, wsize);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        return bcmbd_bm_mem_block_read(unit, sinfo, dyn_info, ovrr_info,
                                       data, wsize);
    }
    return SHR_E_UNAVAIL;
}

static int
bcmbd_bm_sim_write(int unit, bcmdrd_sid_t sid,
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
        return bcmbd_bm_reg_block_write(unit, sinfo, dyn_info, ovrr_info,
                                        data);
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        return bcmbd_bm_mem_block_write(unit, sinfo, dyn_info, ovrr_info,
                                        data);
    }
    return SHR_E_UNAVAIL;
}

static int
bcmbd_bm_model_read(int unit, bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                    uint32_t *data, size_t wsize)
{
    bm_rpc_data_t c_data;
    int rv = SHR_E_FAIL;

    sal_memset(&c_data, 0, sizeof(c_data));
    c_data.unit = unit;
    c_data.sid = sid;
    c_data.index = dyn_info->index;
    /* dyn_info->tbl_inst ignored */
    c_data.buf_len = BCMDRD_WORDS2BYTES(wsize);
    if (bm_srv_handle != NULL) {
        rv = bm_rpc_intf_l2p_get(bm_srv_handle, &c_data);
    }
    if (rv == 0 && ((c_data.buf_len & 3) == 0) &&
        (((size_t)(c_data.buf_len/sizeof(uint32_t))) <= wsize)) {
        sal_memcpy(data, c_data.buf, c_data.buf_len);
    } else {
        rv = SHR_E_FAIL;
    }

    return rv;
}

static int
bcmbd_bm_model_write(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                     uint32_t *data)
{
    bm_rpc_data_t c_data;
    int rv = SHR_E_FAIL;

    sal_memset(&c_data, 0, sizeof(c_data));
    c_data.unit = unit;
    c_data.sid = sid;
    c_data.index = dyn_info->index;
    /* dyn_info->tbl_inst ignored */
    c_data.buf_len = BCMDRD_WORDS2BYTES(BCMDRD_MAX_PT_WSIZE);
    sal_memcpy(c_data.buf, data, c_data.buf_len);
    if (bm_srv_handle != NULL) {
        rv = bm_rpc_intf_p2l_set(bm_srv_handle, &c_data);
    }
    if (rv != 0) {
        rv = SHR_E_FAIL;
    }

    return rv;
}


int
bcmbd_bm_read(int unit, bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                    uint32_t *data, size_t wsize)
{
    return bm_srv_port == NULL ?
        bcmbd_bm_sim_read(unit, sid, dyn_info, ovrr_info, data, wsize) :
        bcmbd_bm_model_read(unit, sid, dyn_info, ovrr_info, data, wsize);
}

int
bcmbd_bm_write(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                     uint32_t *data)
{
    return bm_srv_port == NULL ?
        bcmbd_bm_sim_write(unit, sid, dyn_info, ovrr_info, data) :
        bcmbd_bm_model_write(unit, sid, dyn_info, ovrr_info, data);
}


static int
bcmbd_bm_sdk_msg_init(int unit)
{
    int rv = -1;

    bm_srv_port = getenv("NGSDK_BM_SOCK");
    if (bm_srv_port == NULL) {
        return SHR_E_NONE;
    }

    bm_srv_host = getenv("NGSDK_BM_HOST");
    if (bm_srv_host == NULL) {
        bm_srv_host = "localhost";
    }

    bm_srv_handle = NULL;

    rv = bm_rpc_intf_open(&bm_srv_handle, bm_srv_host, sal_atoi(bm_srv_port));
    if (rv < 0) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

static int
bcmbd_bm_sdk_msg_cleanup(int unit)
{

    if (bm_srv_handle != NULL) {
        (void)bm_rpc_intf_close(bm_srv_handle);
        bm_srv_handle = NULL;
    }

    bm_srv_host = NULL;
    bm_srv_port = NULL;

    return SHR_E_NONE;
}

/*******************************************************************************
 * BM driver init and cleanup
 */

int
bcmbd_bm_drv_init(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_FAIL;
    }

    /* Look for device-specific address calculation, otherwise use default */
    bd->block_port_addr = bcmdrd_dev_block_port_addr_func(unit);
    if (bd->block_port_addr == NULL) {
        bd->block_port_addr = bcmbd_bm_block_port_addr;
    }

    bd->pt_read = bcmbd_bm_read;
    bd->pt_write = bcmbd_bm_write;

    return bcmbd_bm_sdk_msg_init(unit);
}

int
bcmbd_bm_drv_cleanup(int unit)
{
    return bcmbd_bm_sdk_msg_cleanup(unit);
}
