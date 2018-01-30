/*! \file bcm56960_a0_pc_drv_attach.c
 *
 * BCM56960 device specific PC driver.
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
#include <shr/shr_debug.h>

#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_cmicd_miim.h>

#include <phymod/phymod.h>

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_drv.h>
#include <bcmpc/bcmpc_cmicd_pmac.h>

#include <bcmbd/chip/bcm56960_a0_acc.h>
#include <bcmpmac/chip/bcm56960_a0_pmac_drv_list.h>

#include "bcm56960_a0_pc_internal.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

typedef enum th_pm_type_e {
    TH_PM_TYPE_CPU,
    TH_PM_TYPE_LB,
    TH_PM_TYPE_PM4X10,
    TH_PM_TYPE_PM4X25
} th_pm_type_t;

typedef struct pm_lane_layout_s {
    int lane_map[4];
} pm_lane_layout_t;

typedef struct port_map_s {
    bcmpc_lport_t lport;
    bcmpc_pport_t pport;
} port_map_t;

#define TSC_REG_ADDR_TSCID_SET(_reg_addr, _phyad)    \
            ((_reg_addr) |= ((_phyad) & 0x1f) << 19)


/*******************************************************************************
 * TSC Bus Driver
 */

static bcmpc_pport_t
bcm56960_a0_phy_addr_to_port(int unit, uint32_t phy_addr)
{
    /*
     * Internal phy address:
     * Bus 0: PHYID 1-24 are mapped to physical ports 1-24
     * Bus 1: PHYID 1-16 are mapped to physical ports 25-40
     * Bus 2: PHYID 1-24 are mapped to physical ports 41-64
     * Bus 3: PHYID 1-24 are mapped to physical ports 65-88
     * Bus 4: PHYID 1-16 are mapped to physical ports 89-104
     * Bus 5: PHYID 1-24 are mapped to physical ports 105-128
     * Bus 6: PHYID 1 is mapped to physical port 129 and
     *        PHYID 3 is mapped to physical port 131
     */

    uint32_t bus = BCMBD_CMICD_MIIM_IBUS_NUM(phy_addr);

    if (bus == 6) {
        return (phy_addr & BCMBD_CMICD_MIIM_PHY_ID) + 128;
    } else if (bus == 5) {
        return (phy_addr & BCMBD_CMICD_MIIM_PHY_ID) + 104;
    } else if (bus == 4) {
        return (phy_addr & BCMBD_CMICD_MIIM_PHY_ID) + 88;
    } else if (bus == 3) {
        return (phy_addr & BCMBD_CMICD_MIIM_PHY_ID) + 64;
    } else if (bus == 2) {
        return (phy_addr & BCMBD_CMICD_MIIM_PHY_ID) + 40;
    } else if (bus == 1) {
        return (phy_addr & BCMBD_CMICD_MIIM_PHY_ID) + 24;
    } else {
        return (phy_addr & BCMBD_CMICD_MIIM_PHY_ID);
    }
}

/*!
 * \brief Generic function for register write for PM4X25 and PM4X10.
 *
 * \param reg_access_mem The memory that used for access PHY registers.
 *                       e.g: CLPORT_UCMEM_DATAm, XLPORT_UCMEM_DATA.
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param phy_addr Only 5 bits of PHY id used in case of sbus.
 * \param reg_addr The register addrress
 * \param val The value to write to register. 16 bits value + 16 bits mask.
 */
static int
bcm56960_a0_tsc_bus_write(void *user_acc, uint32_t phy_addr,
                          uint32_t reg_addr, uint32_t val)
{
    int rv = SHR_E_NONE;
    int unit, port, idx;
    uint32_t data[16];
    CLPORT_WC_UCMEM_DATAm_t cl_ucmem_data;
    XLPORT_WC_UCMEM_DATAm_t xl_ucmem_data;
    bcmpc_phy_access_data_t *user_data = (bcmpc_phy_access_data_t *)user_acc;

    unit = user_data->unit;
    port = bcm56960_a0_phy_addr_to_port(unit, phy_addr);
    TSC_REG_ADDR_TSCID_SET(reg_addr, phy_addr);

    if ((val & 0xffff0000) == 0) {
        val |= 0xffff0000;
    }

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[1] = ((val & 0xffff) << 16) |
              ((~val & 0xffff0000) >> 16);
    data[2] = 1; /* for TSC register write */

    if (port == 129) {
        /* TSCE SBUS access */
        for (idx = 0; idx < 4; idx++) {
            XLPORT_WC_UCMEM_DATAm_SET(xl_ucmem_data, idx, data[idx]);
        }
        rv = WRITE_XLPORT_WC_UCMEM_DATAm(unit, 0, xl_ucmem_data, port);
    } else {
        /* TSCF SBUS access */
        for (idx = 0; idx < 4; idx++) {
            CLPORT_WC_UCMEM_DATAm_SET(cl_ucmem_data, idx, data[idx]);
        }
        rv = WRITE_CLPORT_WC_UCMEM_DATAm(unit, 0, cl_ucmem_data, port);
    }

    return rv;
}

/*!
 * \brief Generic function for register read for PM4X25 and PM4X10.
 *
 * \param reg_access_mem The memory that used for access PHY registers.
 *                       e.g: CLPORT_UCMEM_DATAm, XLPORT_UCMEM_DATA.
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param phy_addr Only 5 bits of PHY id used in case of sbus.
 * \param reg_addr The register addrress
 * \param val The value read from the register.
 */
static int
bcm56960_a0_tsc_bus_read(void *user_acc, uint32_t phy_addr,
                          uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    int unit, port, idx;
    uint32_t data[16];
    CLPORT_WC_UCMEM_DATAm_t cl_ucmem_data;
    XLPORT_WC_UCMEM_DATAm_t xl_ucmem_data;
    bcmpc_phy_access_data_t *user_data = (bcmpc_phy_access_data_t *)user_acc;

    unit = user_data->unit;
    port = bcm56960_a0_phy_addr_to_port(unit, phy_addr);

    TSC_REG_ADDR_TSCID_SET(reg_addr, phy_addr);

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[2] = 0; /* for TSC register READ */

    if (port == 129) {
        /* TSCE SBUS access */
        for (idx = 0; idx < 4; idx++) {
            XLPORT_WC_UCMEM_DATAm_SET(xl_ucmem_data, idx, data[idx]);
        }
        rv = WRITE_XLPORT_WC_UCMEM_DATAm(unit, 0, xl_ucmem_data, port);

        if (SHR_SUCCESS(rv)) {
            rv = READ_XLPORT_WC_UCMEM_DATAm(unit, 0, &xl_ucmem_data, port);
        }
        *val = XLPORT_WC_UCMEM_DATAm_GET(xl_ucmem_data, 1);
    } else {
        /* TSCF SBUS access */
        for (idx = 0; idx < 4; idx++) {
            CLPORT_WC_UCMEM_DATAm_SET(cl_ucmem_data, idx, data[idx]);
        }
        rv = WRITE_CLPORT_WC_UCMEM_DATAm(unit, 0, cl_ucmem_data, port);

        if (SHR_SUCCESS(rv)) {
            rv = READ_CLPORT_WC_UCMEM_DATAm(unit, 0, &cl_ucmem_data, port);
        }
        *val = CLPORT_WC_UCMEM_DATAm_GET(cl_ucmem_data, 1);
    }

    return rv;
}

static phymod_bus_t bcm56960_a0_tsc_bus = {
    .bus_name = "TSC Bus",
    .read = bcm56960_a0_tsc_bus_read,
    .write = bcm56960_a0_tsc_bus_write,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};


/*******************************************************************************
 * Port Macro mode validation function
 */

static int
bcm56960_a0_pm4x10_mode_validate(int unit, bcmpc_pm_mode_t *mode)
{
    bcmpc_pm_opmode_t opmode = mode->opmode[0];
    pm_lane_layout_t valid_layout = { { 0x1, 0x2, 0x4, 0x8 } };

    if (opmode != BCMPC_PM_OPMODE_DEFAULT) {
        return SHR_E_FAIL;
    }

    if (BCMPC_ARRAY_EQUAL(valid_layout.lane_map, mode->lane_map)) {
        return SHR_E_NONE;
    }

    return SHR_E_FAIL;
}


/*******************************************************************************
 * Port operating mode validation function
 */

static int
bcm56960_a0_pm4x10_port_mode_validate(int unit, bcmpc_pport_t pport,
                                      bcmpc_port_opmode_t port_mode)
{
    uint32_t i, speed;
    uint32_t valid_speeds_one_lane[] = { 10000 };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_port_opmode_to_speed(unit, port_mode, &speed));

    if (speed == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (i = 0; i < COUNTOF(valid_speeds_one_lane); i++) {
        if (speed == valid_speeds_one_lane[i]) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_IF_ERR_VERBOSE_MSG_EXIT
        (SHR_E_FAIL,
         (BSL_META_U(unit,
                     "The port operating mode is not supported.\n")));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Device init and cleanup functions
 */

static int
bcm56960_a0_pm_type_create(int unit)
{
    bcmpc_topo_type_t tinfo;

    SHR_FUNC_ENTER(unit);

    /* Create PM4X10 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH_PM_TYPE_PM4X10));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_PM4X10";
    tinfo.num_lanes = 4;
    tinfo.num_ports = 3;
    tinfo.pm_mode_validate = bcm56960_a0_pm4x10_mode_validate;
    tinfo.port_mode_validate = bcm56960_a0_pm4x10_port_mode_validate;
    tinfo.pm_drv = &bcmpc_pm_drv_pm4x10;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH_PM_TYPE_PM4X10, &tinfo));

    /* Create PM4X25 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH_PM_TYPE_PM4X25));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_PM4X25";
    tinfo.num_lanes = 4;
    tinfo.num_ports = 4;
    tinfo.pm_drv = &bcmpc_pm_drv_pm4x25;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH_PM_TYPE_PM4X25, &tinfo));

    /* Create PM type for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH_PM_TYPE_CPU));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_CPU";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH_PM_TYPE_CPU, &tinfo));

    /* Create PM type for Loopback port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH_PM_TYPE_LB));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_LOOPBACK";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH_PM_TYPE_LB, &tinfo));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_first_port_get(bcmdrd_pbmp_t pbmp) {
    int port;

    for (port = 0; port < BCMDRD_CONFIG_MAX_PORTS; port++) {
        if (BCMDRD_PBMP_MEMBER(pbmp, port)) {
            return port;
        }
    }

    return 0;
}

static int
bcm56960_a0_pm_instance_create(int unit)
{
    int pm_id = 0;
    bcmpc_topo_t pm_info;
    const bcmdrd_chip_info_t *ci;
    const bcmdrd_block_t *blkp;

    SHR_FUNC_ENTER(unit);

    ci = bcmdrd_dev_chip_info_get(unit);
    if (ci == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    for (blkp = ci->blocks; blkp < ci->blocks + ci->nblocks; blkp++) {
        if ((blkp->type != BLKTYPE_CLPORT) &&
            (blkp->type != BLKTYPE_XLPORT) &&
            (blkp->type != BLKTYPE_LBPORT)) {
            continue;
        }

        if (BCMDRD_PBMP_IS_NULL(blkp->pbmps)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmpc_topo_create(unit, pm_id));

        bcmpc_topo_t_init(&pm_info);

        if (blkp->type == BLKTYPE_CLPORT) {
            pm_info.tid = TH_PM_TYPE_PM4X25;
        } else if (blkp->type == BLKTYPE_XLPORT) {
            pm_info.tid = TH_PM_TYPE_PM4X10;
            /*
             * The PM4x10 in Tomahawk occupied 3 physical device ports,
             * 129 ~ 131, and the physical PM port 1 (physical device port
             * 130) is invalid.
             */
            SHR_BITSET(pm_info.invalid_pbmp, 1);
        } else {
            pm_info.tid = TH_PM_TYPE_LB;
        }

        pm_info.base_pport = bcm56960_a0_first_port_get(blkp->pbmps);

        SHR_IF_ERR_EXIT
            (bcmpc_topo_set(unit, pm_id, &pm_info));

        pm_id++;
    }

    /* Create instance for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_create(unit, pm_id));

    bcmpc_topo_t_init(&pm_info);

    pm_info.tid = TH_PM_TYPE_CPU;
    pm_info.base_pport = CMIC_PORT;

    SHR_IF_ERR_EXIT
        (bcmpc_topo_set(unit, pm_id, &pm_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_type_destroy(int unit, int tid, bcmpc_topo_type_t *tinfo,
                         void *cookie)
{
    return bcmpc_topo_type_destroy(unit, tid);
}

static int
bcm56960_a0_inst_destroy(int unit, int pm_id, bcmpc_topo_t *pm_info,
                         void *cookie)
{
    return bcmpc_topo_destroy(unit, pm_id);
}


/*******************************************************************************
 * Port Control Driver
 */

static int
bcm56960_a0_pc_blk_id_get(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id, blktype;
    bcmpc_topo_t pm_info;
    const bcmdrd_chip_info_t *ci;
    bcmdrd_pblk_t pblk;

    ci = bcmdrd_dev_chip_info_get(unit);
    if (ci == NULL) {
        return -1;
    }

    if (pport == CMIC_PORT) {
        return ci->cmic_block;
    }

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    switch (pm_info.tid) {
        case TH_PM_TYPE_PM4X10:
            blktype = BLKTYPE_XLPORT;
            break;
        case TH_PM_TYPE_PM4X25:
            blktype = BLKTYPE_CLPORT;
            break;
        case TH_PM_TYPE_LB:
            blktype = BLKTYPE_LBPORT;
            break;
        default:
            return -1;
    }

    if (bcmdrd_chip_port_block(ci, pport, blktype, &pblk)) {
        return -1;
    }

    return pblk.blknum;
}

static int
bcm56960_a0_pc_phy_addr_get(int unit, bcmpc_pport_t pport)
{
    /*
     * Internal phy address:
     * Bus 0: PHYID 1-24 are mapped to physical ports 1-24
     * Bus 1: PHYID 1-16 are mapped to physical ports 25-40
     * Bus 2: PHYID 1-24 are mapped to physical ports 41-64
     * Bus 3: PHYID 1-24 are mapped to physical ports 65-88
     * Bus 4: PHYID 1-16 are mapped to physical ports 89-104
     * Bus 5: PHYID 1-24 are mapped to physical ports 105-128
     * Bus 6: PHYID 1 is mapped to physical port 129 and
     *        PHYID 3 is mapped to physical port 131
     */
    if (pport == 129 || pport == 131) {
        return ((pport - 129) & ~0x3) + 1 + BCMBD_CMICD_MIIM_IBUS(6);
    }
    if (pport > 104) {
        return ((pport - 105) & ~0x3) + 1 + BCMBD_CMICD_MIIM_IBUS(5);
    }
    if (pport > 88) {
        return ((pport - 89) & ~0x3) + 1 + BCMBD_CMICD_MIIM_IBUS(4);
    }
    if (pport > 64) {
        return ((pport - 65) & ~0x3) + 1 + BCMBD_CMICD_MIIM_IBUS(3);
    }
    if (pport > 40) {
        return ((pport - 41) & ~0x3) + 1 + BCMBD_CMICD_MIIM_IBUS(2);
    }
    if (pport > 24) {
        return ((pport - 25) & ~0x3) + 1 + BCMBD_CMICD_MIIM_IBUS(1);
    }
    return ((pport - 1) & ~0x3) + 1 + BCMBD_CMICD_MIIM_IBUS(0);
}

static phymod_bus_t *
bcm56960_a0_pc_phy_bus_get(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    if (pm_info.tid == TH_PM_TYPE_PM4X10) {
        return &bcm56960_a0_tsc_bus;
    } else if (pm_info.tid == TH_PM_TYPE_PM4X25) {
        return &bcm56960_a0_tsc_bus;
    }

    return NULL;
}

static phymod_dispatch_type_t
bcm56960_a0_pc_phy_drv_get(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    if (pm_info.tid == TH_PM_TYPE_PM4X10) {
        return phymodDispatchTypeTsce;
    } else if (pm_info.tid == TH_PM_TYPE_PM4X25) {
        return phymodDispatchTypeTscf;
    }

    return -1;
}

static bcmpmac_drv_t *
bcm56960_a0_pc_pmac_drv_get(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    if (pm_info.tid == TH_PM_TYPE_PM4X10) {
        return &bcm56960_a0_pmac_xlport;
    } else if (pm_info.tid == TH_PM_TYPE_PM4X25) {
        return &bcm56960_a0_pmac_clport;
    }

    return NULL;
}

static bcmpc_cfg_profile_t *
bcm56960_a0_pc_port_up_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_up_prof;
    /* Enabling from egress side to ingress side. */
    static bcmpc_operation_t port_up_ops[] = {
        
        { "debug", "ep_credit", BCMPC_OP_PARAM_NONE },
        { "tm", "egr_up", 1 },
        { "tm", "mmu_up", 1 },
        { "tm", "ing_up", 1 },
        { "pmac", "speed_set", BCMPC_OP_PARAM_NONE },
        { "pmac", "rx_enable", 1 },
        { "pmac", "tx_enable", 1 },
        { "pmac", "mac_reset", 0 },
        
        { "debug", "fwd_enable", 1 },
        { "pmac", "failover_toggle", BCMPC_OP_PARAM_NONE },
    };

    port_up_prof.ops = port_up_ops;
    port_up_prof.op_cnt = COUNTOF(port_up_ops);

    return &port_up_prof;
}

static bcmpc_cfg_profile_t *
bcm56960_a0_pc_port_down_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_down_prof;
    /* Disabling from ingress side to egress side. */
    static bcmpc_operation_t port_down_ops[] = {
        
        { "debug", "fwd_enable", 0 },
        { "pmac", "rx_enable", 0 },
        { "pmac", "tx_flush", 1 },
        { "pmac", "tx_flush", 0 },
        { "pmac", "mac_reset", 1 },
        { "tm", "ing_up", 0 },
        { "tm", "mmu_up", 0 },
        { "tm", "egr_up", 0 },
    };

    port_down_prof.ops = port_down_ops;
    port_down_prof.op_cnt = COUNTOF(port_down_ops);

    return &port_down_prof;
}

static int
bcm56960_a0_pc_topo_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_pm_type_create(unit));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_pm_instance_create(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_pc_dev_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /*
     * FIXME: DESIGN
     *
     * We might do firmware download earilier.
 */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_phy_firmware_load(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_pc_dev_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_traverse(unit, bcm56960_a0_inst_destroy, NULL));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_traverse(unit, bcm56960_a0_type_destroy, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_pc_oversub_pbmp_get(int unit, bcmpc_pbmp_t *pbmp_buf)
{
    uint32_t total_bw = 0;
    bcmdrd_pbmp_t *ovsb_pbmp = pbmp_buf;
    bcmdrd_pbmp_t xl_pbmp;
    int port;
    bcmpc_pm_lport_rsrc_t prsrc;

    SHR_FUNC_ENTER(unit);

    if (bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_CLPORT, ovsb_pbmp) < 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    BCMDRD_PBMP_ITER(*ovsb_pbmp, port) {
        if (SHR_SUCCESS(bcmpc_pm_lport_rsrc_get(unit, port, &prsrc))) {
            total_bw += prsrc.max_speed;
        }
    }
    if (total_bw > 532500 * 4) {
        if (bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_XLPORT, &xl_pbmp) < 0) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        BCMDRD_PBMP_OR(*ovsb_pbmp, xl_pbmp);
    } else {
        BCMDRD_PBMP_CLEAR(*ovsb_pbmp);
    }

exit:
    SHR_FUNC_EXIT();
}

static bcmpc_drv_t bcm56960_a0_pc_drv = {
    .topology_init = bcm56960_a0_pc_topo_init,
    .dev_init = bcm56960_a0_pc_dev_init,
    .dev_cleanup = bcm56960_a0_pc_dev_cleanup,
    .phy_addr_get = bcm56960_a0_pc_phy_addr_get,
    .blk_id_get = bcm56960_a0_pc_blk_id_get,
    .phy_bus_get = bcm56960_a0_pc_phy_bus_get,
    .pmac_bus_get = bcmpc_cmicd_pmac_bus_get,
    .phy_drv_get = bcm56960_a0_pc_phy_drv_get,
    .pmac_drv_get = bcm56960_a0_pc_pmac_drv_get,
    .port_up_prof_get = bcm56960_a0_pc_port_up_prof_get,
    .port_down_prof_get = bcm56960_a0_pc_port_down_prof_get,
    .op_exec = bcm56960_a0_pc_op_exec,
    .oversub_pbmp_get = bcm56960_a0_pc_oversub_pbmp_get,
};


/*******************************************************************************
 * Public functions
 */

int
bcm56960_a0_pc_drv_attach(int unit)
{
    return bcmpc_drv_set(unit, &bcm56960_a0_pc_drv);
}
