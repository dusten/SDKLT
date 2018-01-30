/*
 * $Id: phymod_tsc_iblk.c,v 1.1.2.5 2013/06/11 15:49:17 mlarsen Exp $
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
 * 
 * 
 *
 * XGS internal PHY read function with AER support.
 *
 * This function accepts a 32-bit PHY register address and will
 * properly configure clause 45 DEVAD and XAUI lane access.
 * Please see phymod_reg.h for additional information.
 */

#include <phymod/acc/phymod_tsc_iblk.h>
#include <phymod/phymod_debug.h>

/*
 * Some bus drivers can handle both clause 22 and clause 45 access, so
 * we use this bit to request clause 45 access.
 */
#define FORCE_CL45      0x20
#define LANE_IS_MEMBER(_lnmap, _ln) (((_lnmap) & (0x1 << (_ln))) ? 1 : 0)
#define LANE_MAX 16 /* QSGMII has multi-core, support more than 4 lanes */

/* TSCBH has 8 lanes, lower 4 lanes belong to MPP0 and upper 4 lanes belong to MPP1 */
#define TSCBH_LANE_IS_MPP1(_ln) (_ln > 3 ? 1: 0)

int
phymod_tsc_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t lane_map, lane;
    uint32_t aer, add;
    phymod_bus_t* bus;
    uint8_t pll_index = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x pa=null\n", add));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    /* Do not attempt to read write-only registers */
    if (addr & PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY) {
        *data = 0;
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x WO=1\n", add));
        return 0;
    }

    /* Determine which lane to read from */
    lane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        lane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
    } else {
        /* Use first lane in lane map by default */
        lane_map = PHYMOD_ACC_LANE_MASK(pa);
        if (lane_map & 0x1) {
            lane = 0;
        } else if (lane_map & 0x2) {
            lane = 1;
        } else if (lane_map & 0x4) {
            lane = 2;
        } else if (lane_map & 0x8) {
            lane = 3;
        } else if (lane_map & 0xfff0) {
            lane = -1;
            while (lane_map) {
                lane++;
                lane_map >>= 1;
            }
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Encode address extension */
    aer = lane | (devad << 11) | (pll_index << 8);

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_READ(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_READ(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Read register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_READ(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl22 add=%x aer=%x blk=%x adr=%x reg=%x lm=%0d rtn=%0d d=%x\n",
                            add, aer, blkaddr, addr, regaddr, pa->lane_mask, ioerr, *data));
    return ioerr;
}



int
phymod_tscbh_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t lane_map, lane;
    uint32_t aer, add;
    phymod_bus_t* bus;
    uint8_t pll_index = 0, mpp_sel = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x pa=null\n", add));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    /* Do not attempt to read write-only registers */
    if (addr & PHYMOD_REG_ACC_TSC_IBLK_WR_ONLY) {
        *data = 0;
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd add=%x WO=1\n", add));
        return 0;
    }

    /* Determine which lane to read from */
    lane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        lane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
    } else {
        /* Use first lane in lane map by default */
        lane_map = PHYMOD_ACC_LANE_MASK(pa);
        if (lane_map & 0x1) {
            lane = 0;
        } else if (lane_map & 0x2) {
            lane = 1;
        } else if (lane_map & 0x4) {
            lane = 2;
        } else if (lane_map & 0x8) {
            lane = 3;
        } else if (lane_map & 0x10) {
            lane = 4;
        } else if (lane_map & 0x20) {
            lane = 5;
        } else if (lane_map & 0x40) {
            lane = 6;
        } else if (lane_map & 0x80) {
            lane = 7;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Encode address extension based on if pcs or PMD */
    if (devad) {
    /* PMD device read */
        aer = lane | (devad << 11) | (pll_index << 8);
    } else {
    /* PCS device */
        if (TSCBH_LANE_IS_MPP1(lane)) {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_1;
        } else {
            mpp_sel = PHYMOD_TSCBH_IBLK_MPP_0;
        }
        aer = (lane % 4) | (mpp_sel << PHYMOD_TSCBH_IBLK_MPP_SHIFT);
    }

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_READ(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_READ(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, *data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Read register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_READ(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_rd cl22 add=%x aer=%x blk=%x adr=%x reg=%x lm=%0d rtn=%0d d=%x\n",
                            add, aer, blkaddr, addr, regaddr, pa->lane_mask, ioerr, *data));
    return ioerr;
}


static int
_tsc_iblk_write_lane(
    const phymod_access_t *pa,
    uint32_t lane,
    uint32_t addr,
    uint32_t data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t aer, add;
    uint32_t wr_mask, rdata;
    phymod_bus_t* bus;
    uint32_t is_write_disabled;
    uint8_t pll_index = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr add=%x pa=null\n", addr));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    if (PHYMOD_IS_WRITE_DISABLED(pa, &is_write_disabled) ==  0) {
        if (is_write_disabled) {
            return ioerr;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Check if write mask is specified */
    wr_mask = (data >> 16);
    if (wr_mask) {
        /* Read register if bus driver does not support write mask */
        if (PHYMOD_BUS_CAP_WR_MODIFY_GET(bus) == 0) {
            ioerr += phymod_tsc_iblk_read(pa, addr, &rdata);
            data = (rdata & ~wr_mask) | (data & wr_mask);
            data &= 0xffff;
        }
    }

    /* Encode address extension */
    aer = lane | (devad << 11) | (pll_index << 8);

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        addr &= 0xffff;
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Write register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_WRITE(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl22 add=%x aer=%x blk=%x reg=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                            addr, aer, blkaddr, regaddr, addr, pa->lane_mask, ioerr, data));
    return ioerr;
}

static int
_tscbh_iblk_write_lane(
    const phymod_access_t *pa,
    uint32_t lane,
    uint32_t addr,
    uint32_t data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t blkaddr, regaddr;
    uint32_t aer, add;
    uint32_t wr_mask, rdata;
    phymod_bus_t* bus;
    uint32_t is_write_disabled;
    uint8_t pll_index = 0;

    add = addr ;

    /* Work around for ARM compiler error [-Werror=unused-but-set-variable] */
    if ( add == 0 ) {
        add = addr;
    }

    if (pa == NULL) {
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr add=%x pa=null\n", addr));
        return -1;
    }

    bus = PHYMOD_ACC_BUS(pa);
    pll_index = PHYMOD_ACC_PLLIDX(pa) & 0x3;

    if (PHYMOD_IS_WRITE_DISABLED(pa, &is_write_disabled) ==  0) {
        if (is_write_disabled) {
            return ioerr;
        }
    }

    /* Use default DEVAD if none specified */
    if(((pa->devad & PHYMOD_ACC_DEVAD_0_OVERRIDE_MASK) != 0) && (devad == 0)) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    /* Force DEVAD if want special address */
    } else if(pa->devad & PHYMOD_ACC_DEVAD_FORCE_MASK) {
        devad = (pa->devad & PHYMOD_ACC_DEVAD_MASK);
    }

    /* Check if write mask is specified */
    wr_mask = (data >> 16);
    if (wr_mask) {
        /* Read register if bus driver does not support write mask */
        if (PHYMOD_BUS_CAP_WR_MODIFY_GET(bus) == 0) {
            ioerr += phymod_tscbh_iblk_read(pa, addr, &rdata);
            data = (rdata & ~wr_mask) | (data & wr_mask);
            data &= 0xffff;
        }
    }

    /* Encode address extension based on if pcs or PMD */
    if (devad) { /* PMD */
        aer = lane | (devad << 11) | (pll_index << 8);
    } else {
        aer = lane | (devad << 11);
    }

    /* Mask raw register value */
    addr &= 0xffff;

    /* If bus driver supports lane control, then we are done */
    if (PHYMOD_BUS_CAP_LANE_CTRL_GET(bus)) {
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (aer << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr sbus add=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Use clause 45 access if supported */
    if (PHYMOD_ACC_F_CLAUSE45_GET(pa)) {
        addr &= 0xffff;
        devad |= FORCE_CL45;
        ioerr += PHYMOD_BUS_WRITE(pa, 0xffde | (devad << 16), aer);
        ioerr += PHYMOD_BUS_WRITE(pa, addr | (devad << 16), data);
        PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl45 add=%x dev=%x aer=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                                add, devad, aer, addr, pa->lane_mask, ioerr, data));
        return ioerr;
    }

    /* Write address extension register */
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, 0xffd0);
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1e, aer);

    /* Select block */
    blkaddr = addr & 0xfff0;
    ioerr += PHYMOD_BUS_WRITE(pa, 0x1f, blkaddr);

    /* Write register value */
    regaddr = addr & 0xf;
    if (addr & 0x8000) {
        regaddr |= 0x10;
    }
    ioerr += PHYMOD_BUS_WRITE(pa, regaddr, data);
    PHYMOD_VDBG(DBG_ACC,pa,("iblk_wr cl22 add=%x aer=%x blk=%x reg=%x adr=%x lm=%0x rtn=%0d d=%x\n",
                            addr, aer, blkaddr, regaddr, addr, pa->lane_mask, ioerr, data));
    return ioerr;
}


int
phymod_tsc_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    int ioerr = 0;
    uint32_t lane_map, hwlane, is_hwsupport = 1;

    hwlane = 0;
    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        hwlane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
        return _tsc_iblk_write_lane(pa, hwlane, addr, data);
    }
    lane_map = PHYMOD_ACC_LANE_MASK(pa) & 0xffff;
    switch (lane_map) {
        case 0x0:
            hwlane = 0x0;
            break;
        case 0x1:
            hwlane = 0x0;
            break;
        case 0x2:
            hwlane = 0x1;
            break;
        case 0x4:
            hwlane = 0x2;
            break;
        case 0x8:
            hwlane = 0x3;
            break;
        case 0xf:
            hwlane = PHYMOD_TSC_IBLK_BCAST;
            break;
        case 0x3:
            hwlane = PHYMOD_TSC_IBLK_MCAST01;
            break;
        case 0xc:
            hwlane = PHYMOD_TSC_IBLK_MCAST23;
            break;
        default:
            is_hwsupport = 0;
            hwlane = 0x0;
    }
     /*
     * If the lane mask is supported by HW, i.e. a single lane or a
     * supported multicast combination (0x3, 0xC or 0xF), then program
     * directly by HW.
     */
    if (is_hwsupport) {
        return _tsc_iblk_write_lane(pa, hwlane, addr, data);
    }
     /*
     * If the lane mask is not supported directly by HW, e.g. 0x9, then
     * program one lane at a time.
     */
    for (hwlane = 0; hwlane < LANE_MAX; hwlane++) {
        if (LANE_IS_MEMBER(lane_map, hwlane)) {
            ioerr = _tsc_iblk_write_lane(pa, hwlane, addr, data);
            if (ioerr != 0) {
                return ioerr;
            }
        }
    }
    return ioerr;
}

int
phymod_tscbh_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
    uint32_t lane_map, hwlane = 0, is_hwsupport = 1;
    uint32_t mpp_sel = 0, lane = 0;

    if (addr & PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE) {
        /* Forcing lane overrides default behavior */
        hwlane = (addr >> PHYMOD_REG_ACCESS_FLAGS_SHIFT) & 0x7;
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    }
    lane_map = PHYMOD_ACC_LANE_MASK(pa) & 0xffff;

    /* need to use different lane for pcs or PMD device */
    if (devad) {
    /* for PMD device */
        switch (lane_map) {
            case 0x0:
                hwlane = 0x0;
                break;
            case 0x1:
                hwlane = 0x0;
                break;
            case 0x2:
                hwlane = 0x1;
                break;
            case 0x4:
                hwlane = 0x2;
                break;
            case 0x8:
                hwlane = 0x3;
                break;
            case 0x10:
                hwlane = 0x4;
                break;
            case 0x20:
                hwlane = 0x5;
                break;
            case 0x40:
                hwlane = 0x6;
                break;
            case 0x80:
                hwlane = 0x7;
                break;
            case 0xf:
                hwlane = PHYMOD_BH_IBLK_MCAST0123;
                break;
            case 0xf0:
                hwlane = PHYMOD_BH_IBLK_MCAST4567;
                break;
            case 0x3:
                hwlane = PHYMOD_BH_IBLK_MCAST01;
                break;
            case 0xc:
                hwlane = PHYMOD_BH_IBLK_MCAST23;
                break;
            case 0x30:
                hwlane = PHYMOD_BH_IBLK_MCAST45;
                break;
            case 0xc0:
                hwlane = PHYMOD_BH_IBLK_MCAST67;
                break;
            case 0xff:
                hwlane = PHYMOD_BH_IBLK_BCAST;
                break;
            default:
                is_hwsupport = 0;
                hwlane = 0x0;
        }
    } else {
        /* pcs device */
        switch (lane_map) {
            case 0x0:
                hwlane = 0x100;
                break;
            case 0x1:
                hwlane = 0x100;
                break;
            case 0x2:
                hwlane = 0x101;
                break;
            case 0x4:
                hwlane = 0x102;
                break;
            case 0x8:
                hwlane = 0x103;
                break;
            case 0x10:
                hwlane = 0x200;
                break;
            case 0x20:
                hwlane = 0x201;
                break;
            case 0x40:
                hwlane = 0x202;
                break;
            case 0x80:
                hwlane = 0x203;
                break;
            case 0xf:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST0123;
                break;
            case 0xff:
                hwlane = PHYMOD_TSCBH_IBLK_BCAST;
                break;
            case 0x3:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST01;
                break;
            case 0xc:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST23;
                break;
            case 0x30:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST45;
                break;
            case 0xc0:
                hwlane = PHYMOD_TSCBH_IBLK_MCAST67;
                break;
            default:
                is_hwsupport = 0;
                hwlane = 0x0;
        }
     }
     /*
     * If the lane mask is supported by HW, i.e. a single lane or a
     * supported multicast combination (0x3, 0xC or 0xF), then program
     * directly by HW.
     */
    if (is_hwsupport) {
        return _tscbh_iblk_write_lane(pa, hwlane, addr, data);
    }
     /*
     * If the lane mask is not supported directly by HW, e.g. 0x9, then
     * program one lane at a time.
     */
    for (lane = 0; lane < LANE_MAX; lane++) {
        if (LANE_IS_MEMBER(lane_map, lane)) {
            /* if pcs device, then nbeed to form aer lane */
            if (!devad) {
                if (TSCBH_LANE_IS_MPP1(lane)) {
                    mpp_sel = PHYMOD_TSCBH_IBLK_MPP_1;
                } else {
                    mpp_sel = PHYMOD_TSCBH_IBLK_MPP_0;
                }
                hwlane = (lane % 4) | (mpp_sel << PHYMOD_TSCBH_IBLK_MPP_SHIFT);
            }
            ioerr = _tscbh_iblk_write_lane(pa, hwlane, addr, data);
            if (ioerr != 0) {
                return ioerr;
            }
        }
    }
    return ioerr;
}
