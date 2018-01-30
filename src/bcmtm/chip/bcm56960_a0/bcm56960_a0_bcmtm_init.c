/*! \file bcm56960_a0_bcmtm_init.c
 *
 * Traffic manager init library
 *
 * The library provides functions to setup traffic manager in bcm56960_a0
 * family of devices. The configurations depend on the number of ports in use
 * and other port-specific attrbutes.
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

#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/chip/bcm56960_a0_acc.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>

#include <bcm56960_a0/bcm56960_a0_bcmtm_types.h>
#include <bcm56960_a0/bcm56960_a0_bcmtm_drv.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmtm/bcmtm_internal_tdm.h>

#include <bcmtm/bcmtm_device_info.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>

/*******************************************************************************
 * Local definitions
 */
#define NUM_PHYS_PORTS                  BCM56960_A0_NUM_PORTS
#define NUM_LOGIC_PORTS                 BCM56960_A0_NUM_PORTS

#define TH_MGMT_PORT_0                  129
#define TH_MGMT_PORT_1                  131

/* Device port number of loopback ports */
#define TH_DEV_LB_PORT_0                33
#define TH_DEV_LB_PORT_1                67
#define TH_DEV_LB_PORT_2                101
#define TH_DEV_LB_PORT_3                135


#define TH_PORTS_PER_PBLK               4
#define TH_PBLKS_PER_PIPE               8

#define TH_NUM_PIPES                    BCM56960_A0_NUM_PIPES
#define TH_NUM_XPES                     BCM56960_A0_NUM_XPES
#define TH_NUM_LAYERS                   BCM56960_A0_NUM_LAYERS

#define TH_PORTS_PER_PIPE               (TH_PORTS_PER_PBLK * TH_PBLKS_PER_PIPE)

static bcmtm_int_port_info_t cpu_ports[BCMTM_NUM_CPU_PORTS] = {
    { 0, 0, 0, 32, 32 }
};

static bcmtm_int_port_info_t lb_ports[BCMTM_NUM_LB_PORTS] = {
    { 33,  132, 0, 33, 33   },    /* loopback port 0 */
    { 67,  133, 1, 33, 97   },    /* loopback port 1 */
    { 101, 134, 2, 33, 161  },    /* loopback port 2 */
    { 135, 135, 3, 33, 225  }     /* loopback port 3 */
};

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief bcm56960 chip specific port pipe informations.
 *
 * Gets the pipe information for a given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * returns pipe in which port lies.
 */
static int
bcm56960_a0_tm_port_pipe_get(int unit, int port)
{
    int pp, pipe;

    if ((port < 0) || (port >= NUM_PHYS_PORTS)) {
        return -1;
    }
    /* Fixed mappings */
    if (port == cpu_ports[0].phy_port) {
        return cpu_ports[0].pipe;
    }
    for (pp = 0; pp < COUNTOF(lb_ports); pp++) {
        if (port == lb_ports[pp].phy_port) {
            return lb_ports[pp].pipe;
        }
    }
    if (port == TH_MGMT_PORT_0) {
        pipe = 1;
    } else if (port == TH_MGMT_PORT_1) {
        pipe = 2;
    } else {
        pipe = (port - 1) / TH_PORTS_PER_PIPE;
    }

    return pipe;
}

/*!
 * \brief bcm56960 chip specific idb port informations.
 *
 * Gets the idb port information for a given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * returns idb port number for the physical port.
 */
static int
bcm56960_a0_tm_port_idbport_get(int unit, int port)
{
    int pp, idb_port;

    if ((port < 0) || (port >= NUM_PHYS_PORTS)) {
        return -1;
    }

    /* Fixed mappings */
    if (port == cpu_ports[0].phy_port) {
        return cpu_ports[0].idb_port;
    }
    for (pp = 0; pp < COUNTOF(lb_ports); pp++) {
        if (port == lb_ports[pp].phy_port) {
            return lb_ports[pp].idb_port;
        }
    }

    if (port == TH_MGMT_PORT_0 || port == TH_MGMT_PORT_1) {
        idb_port = 32;
    } else {
        idb_port = (port - 1) % TH_PORTS_PER_PIPE;
    }

    return idb_port;
}

/*!
 * \brief bcm56960 chip specific mmu port informations.
 *
 * Gets the mmu port information for a given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * returns mmu port number for the physical port.
 */
static int
bcm56960_a0_tm_port_mmuport_get(int unit, int port)
{
    int pipe, idb_port;

    idb_port = bcm56960_a0_tm_port_idbport_get(unit, port);
    pipe = bcm56960_a0_tm_port_pipe_get(unit, port);

    if (port ==  TH_MGMT_PORT_0) {
        return 96;
    }
    else if (port == TH_MGMT_PORT_1) {
        return 160;
    }
    else {
        return ((pipe << 6) | ((idb_port & 0x1) << 4) | (idb_port >> 1));
    }
}

/*!
 * \brief bcm56960 chip mmu port mappings.
 *
 * Configures mmu port mappings for the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL Port mapping information unavailable.
 */
static int
bcm56960_a0_tm_mmu_port_map(int unit)
{
    bcmtm_drv_info_t *drv_info;
    int pport, mmu_port, dev_port, idb_port;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t sid ;
    bcmdrd_sid_t sid_list[] = { ING_IDB_TO_DEV_MAPm,
                                SYS_PORTMAPm,
                                EGR_DEV_TO_PHYS_MAPr,
                                MMU_TO_DEV_MAPr,
                                MMU_TO_PHYS_MAPr,
                                MMU_TO_SYS_MAPr };
    int pipe;
    bcmdrd_fid_t fid ;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    int index, sid_ind, inst;
    uint32_t fval ;
    uint32_t ltmbuf = 0;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    for (pport = 0; pport < NUM_PHYS_PORTS; pport++) {
        if ((drv_info->port_p2l_mapping[pport] == -1) ||
            (drv_info->port_p2l_mapping[pport] == 65535)) {
            continue;
        }

        dev_port = drv_info->port_p2l_mapping[pport];
        mmu_port = drv_info->port_p2m_mapping[pport];
        if (pport == 130) {
            pipe = 3;
            idb_port = 32;
        } else {
            pipe = drv_info->port_pipe_mapping[dev_port];
            idb_port = bcm56960_a0_tm_port_idbport_get(unit, pport);
        }

        /*
         * ING_IDB_TO_DEV_MAPm - Ingress physical to device port.
         * SYS_PORTMAPm - Ingress GPP port to device port.
         * EGR_DEV_TO_PHYS_MAPr - Egress device port to physical port.
         * MMU_TO_DEV_MAPr - MMU port to device port.
         * MMU_TO_PHYS_MAPr - MMU port to physical port.
         * MMU_TO_SYS_MAPr - MMU port to system port.
         */
        for (sid_ind = 0; sid_ind < COUNTOF(sid_list); sid_ind++) {
            sid = sid_list[sid_ind];
            ltmbuf = 0;
            switch(sid) {
                case ING_IDB_TO_DEV_MAPm:
                    fid = DEVICE_PORT_NUMBERf;
                    inst = pipe;
                    index = idb_port;
                    fval = dev_port;
                    break;
                case SYS_PORTMAPm:
                    fid = DEVICE_PORT_NUMBERf;
                    inst = 0;
                    index = dev_port;
                    fval = dev_port;
                    break;
                case EGR_DEV_TO_PHYS_MAPr:
                    fid = PHYSICAL_PORT_NUMBERf;
                    inst = dev_port;
                    index = 0;
                    fval = pport;
                    break;
                case MMU_TO_DEV_MAPr:
                    fid = DEVICE_PORTf;
                    inst = mmu_port;
                    index = 0;
                    fval = dev_port;
                    break;
                case MMU_TO_PHYS_MAPr:
                    fid = PHY_PORTf;
                    inst = mmu_port;
                    index = 0;
                    fval = pport;
                    break;
                case MMU_TO_SYS_MAPr:
                    fid = SYSTEM_PORTf;
                    inst = mmu_port;
                    index = 0;
                    fval = dev_port;
                    break;
               default:
                    fid = -1;
                    inst = -1;
                    index = -1;
                    fval = -1;
                    break;
            }

            BCMTM_PT_DYN_INFO(pt_dyn_info, index, inst);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid, ltid,
                                       (void*)&pt_dyn_info, &ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid,
                                        (void*)&pt_dyn_info, &ltmbuf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm56960 chip mmu port thresholds.
 * Enables the mmu port threshold configurations for the ports.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL Port mapping information unavailable.
 */
static int
bcm56960_a0_tm_mmu_thd_en(int unit)
{
    int ioerr = 0;
    bcmdrd_pbmp_t pbmp;
    int port, pipe, idx, mport, lport;
    uint32_t pmap[_TH_PIPES_PER_DEV][2] = {{0}};

    THDU_OUTPUT_PORT_RX_ENABLE_64r_t outp_rx_en;
    MMU_THDM_DB_PORT_RX_ENABLE_64r_t dbp_rx_en;
    MMU_THDM_MCQE_PORT_RX_ENABLE_64r_t mcqep_rx_en;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_get(unit, &drv_info);
    BCMDRD_PBMP_ASSIGN(pbmp,
            ((drv_info->port_schedule_state).in_port_map.physical_pbm));

    BCMDRD_PBMP_ITER(pbmp, port) {
        lport = drv_info->port_p2l_mapping[port];
        pipe = drv_info->port_pipe_mapping[lport];
        mport = drv_info->port_p2m_mapping[port];
        if (pipe < 0 || mport < 0) {
            continue;
        }
        pmap[pipe][(mport & 0x20) >> 5] |= (1 << (mport & 0x1f));
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe ++) {
        for (idx = 0; idx < 2; idx++) {
            THDU_OUTPUT_PORT_RX_ENABLE_64r_SET(outp_rx_en, idx,
                                               pmap[pipe][idx]);
            MMU_THDM_DB_PORT_RX_ENABLE_64r_SET(dbp_rx_en, idx, pmap[pipe][idx]);
            MMU_THDM_MCQE_PORT_RX_ENABLE_64r_SET(mcqep_rx_en, idx,
                                                 pmap[pipe][idx]);
        }
        ioerr += WRITE_THDU_OUTPUT_PORT_RX_ENABLE_64r_ALL(unit, pipe,
                                                          outp_rx_en);
        ioerr += WRITE_MMU_THDM_DB_PORT_RX_ENABLE_64r(unit, pipe, dbp_rx_en);
        ioerr += WRITE_MMU_THDM_MCQE_PORT_RX_ENABLE_64r(unit, pipe,
                                                        mcqep_rx_en);
    }
    SHR_IF_ERR_EXIT(ioerr);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm56960 chip specific port schedule state database.
 *
 * Populates the port schedule database based on the port information from
 * port module.
 *
 * \param [in] unit Unit number.
 * \param [in] delete_update Delete call.
 * \param [in] num_ports Number of ports.
 * \param [in] old_cfg Old port mapping configuration(currently active).
 * \param [in] new_cfg New port mapping configuration (to be updated).
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error return error code.
 */
static int
bcm56960_a0_tm_port_sched_state_resource_fill(int unit,
                               int delete_update,
                               size_t num_ports,
                               const bcmpc_mmu_port_cfg_t *old_cfg,
                               const bcmpc_mmu_port_cfg_t *new_cfg)
{
    int pport;
    int int_port, port_is_int, lport;
    int *num_res;
    soc_port_resource_t  *res;

    bcmtm_drv_info_t *drv_info;
    soc_port_schedule_state_t *port_schedule_state;

    bcmpc_mmu_port_cfg_t port_cfg_data;
    bcmpc_mmu_port_cfg_t *port_cfg;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    port_schedule_state = &(drv_info->port_schedule_state);

    num_res = &(port_schedule_state->nport);
    *num_res = 0;

    for (pport = 0; pport < (int)num_ports; pport++) {
        port_is_int = 0;

        for (int_port = 0; int_port < BCMTM_NUM_CPU_PORTS; int_port++) {
            if (pport == (drv_info->cpu_ports[int_port]).phy_port) {
                port_is_int = 1;
            }
        }
        for (int_port = 0; int_port < BCMTM_NUM_LB_PORTS; int_port++) {
            if (pport == (drv_info->lb_ports[int_port]).phy_port) {
                port_is_int = 1;
            }
        }

        if (port_is_int == 1) {
            continue;
        }


        port_cfg_data = new_cfg[pport];
        port_cfg = &port_cfg_data;

        lport = port_cfg->lport;
        drv_info->port_flags[lport] = 0;
        drv_info->port_p2l_mapping[pport] = lport;

        if (port_cfg->lport != BCMPC_INVALID_LPORT) {

            drv_info->port_l2p_mapping[lport] = pport;
            drv_info->port_l2i_mapping[lport] = (pport - 1) % 32;
            drv_info->port_num_lanes[lport] = port_cfg->num_lanes;
            drv_info->port_speed_max[lport] = port_cfg->speed_max;
            drv_info->port_speed_cur[lport] = port_cfg->speed_cur;
            drv_info->port_p2m_mapping[pport] =
                bcm56960_a0_tm_port_mmuport_get(unit, pport);
            drv_info->port_p2m_local_mapping[pport] =
                drv_info->port_p2m_mapping[pport] & 0x3f;
            drv_info->port_pipe_mapping[lport] =
                bcm56960_a0_tm_port_pipe_get(unit, pport);
            drv_info->num_mc_cosq[lport] = 10;
            drv_info->num_uc_cosq[lport] = 10;
            drv_info->uc_cosq_base[lport] =
                (drv_info->port_p2m_mapping[pport] % 64) * 10;
            drv_info->mc_cosq_base[lport] =
                (drv_info->port_p2m_mapping[pport] % 64) * 10;
            if (port_cfg->flags & BCMPC_MMU_F_HG2) {
                drv_info->port_flags[lport] |= BCMTM_PORT_IS_HG2;
            }
            if (port_cfg->flags & BCMPC_MMU_F_MGMT) {
                drv_info->port_flags[lport] |= BCMTM_PORT_IS_MGMT;
            }
            if (port_cfg->flags & BCMPC_MMU_F_OVSUB) {
                drv_info->port_flags[lport] |= BCMTM_PORT_IS_OVSB;
            }
        }

        /* Populate resource structure if the ports changed */
        if (sal_memcmp(&(old_cfg[pport]),
                       &(new_cfg[pport]), sizeof(bcmpc_mmu_port_cfg_t))) {

            res = &(port_schedule_state->resource[*num_res]);

            /* Adding new port */
            if ((old_cfg[pport].lport == BCMPC_INVALID_LPORT) &&
                (new_cfg[pport].lport != BCMPC_INVALID_LPORT)) {

                res->logical_port = new_cfg[pport].lport;
                res->physical_port = pport;
                res->speed = new_cfg[pport].speed_max;
                res->num_lanes = new_cfg[pport].num_lanes;
                res->encap = (new_cfg[pport].flags & BCMPC_MMU_F_HG2) ?
                                 BCMTM_PORTENCAP_HIGIG2 : BCMTM_PORTENCAP_IEEE;
                res->pipe = bcm56960_a0_tm_port_pipe_get(unit, pport);
                res->idb_port = (pport - 1) % 32;
                res->mmu_port = bcm56960_a0_tm_port_mmuport_get(unit, pport);
                *num_res += 1;
            }

            /* Update drv_info for deleted ports only when
             * resource_execute is completed
 */
            if (delete_update) {
                /* Delete ports */
                if ((new_cfg[pport].lport == BCMPC_INVALID_LPORT) &&
                    (old_cfg[pport].lport != BCMPC_INVALID_LPORT)) {
                    res->logical_port = old_cfg[pport].lport;
                    res->physical_port = BCMPC_INVALID_PPORT;
                    *num_res += 1;
                }
            }

            /* Modify port config */
            if ((new_cfg[pport].lport != BCMPC_INVALID_LPORT) &&
                (old_cfg[pport].lport != BCMPC_INVALID_LPORT)) {

                res->logical_port = old_cfg[pport].lport;
                res->idb_port = (pport - 1 )%32;
                res->mmu_port = bcm56960_a0_tm_port_mmuport_get(unit, pport);
                res->physical_port = BCMPC_INVALID_PPORT;

                *num_res += 1;
                res = &(port_schedule_state->resource[*num_res]);

                res->logical_port = new_cfg[pport].lport;
                res->physical_port = pport;
                res->speed = new_cfg[pport].speed_max;
                res->num_lanes = new_cfg[pport].num_lanes;

                res->encap = (new_cfg[pport].flags & BCMPC_MMU_F_HG2) ?
                                 BCMTM_PORTENCAP_HIGIG2 : BCMTM_PORTENCAP_IEEE;
                res->pipe = bcm56960_a0_tm_port_pipe_get(unit, pport);
                res->idb_port = (pport - 1) % 32;
                res->mmu_port = bcm56960_a0_tm_port_mmuport_get(unit, pport);
                *num_res += 1;
            }
        }
    }

    SHR_IF_ERR_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_a0_tm_shaper_config_init(int unit)
{
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = TM_SHAPER_CONFIGt;
    uint64_t field[TM_SHAPER_CONFIGt_FIELD_COUNT] = {0} ;
    size_t actual;
    uint32_t fval = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info = {0 , 0};
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /* get the values from config table. */

    rv = bcmcfg_table_get(unit, ltid, TM_SHAPER_CONFIGt_FIELD_COUNT,
                    field, &actual);
    if (rv == SHR_E_UNAVAIL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* write to hardware */
    sid = MTRO_REFRESH_CONFIGr;
    fval = (uint32_t)field[TM_SHAPER_CONFIGt_REFRESH_TIMEf];
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,(void *)&pt_dyn_info, &fval));

    sid = MMU_SCFG_MISCCONFIGr;
    fval = (uint32_t)field[TM_SHAPER_CONFIGt_ITU_MODEf];
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,(void *)&pt_dyn_info, &fval));
exit:
    SHR_FUNC_EXIT();
}

static void
bcm56960_a0_bcmtm_dev_info_set(int unit)
{
    struct bcmtm_device_info bcm56960_a0_tm_dev_info = {
        .max_pkt_sz = 9416,
        .pkt_hdr_sz = 64,
        .jumbo_pkt_sz = 9216,
        .default_mtu = 1536,
        .cell_sz = 208,
        .num_cells = 20165,
        .num_port_pg = 8,
        .num_sp = 4,
        .mcq_entry = 8192 };
    bcmtm_device_info[unit] = bcm56960_a0_tm_dev_info;
}


/*******************************************************************************
 * Public functions
 */
int
bcm56960_a0_bcmtm_tdm_reconfigure(int unit)
{
    bcmtm_drv_info_t *drv_info;
    soc_port_schedule_state_t *port_schedule_state;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    port_schedule_state = &(drv_info->port_schedule_state);

    /* No new ports added - return early */
    if (port_schedule_state->nport == 0) {
        SHR_FUNC_EXIT();
    }

    SHR_IF_ERR_EXIT(soc_tomahawk_reconfigure_ports(unit, port_schedule_state));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56960_a0_bcmtm_pc_configure(int unit,
                               size_t num_ports,
                               const bcmpc_mmu_port_cfg_t *old_cfg,
                               const bcmpc_mmu_port_cfg_t *new_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_tm_port_sched_state_resource_fill(unit, FALSE, num_ports,
                                                   old_cfg, new_cfg));

    SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_tdm_init(unit));

    SHR_IF_ERR_EXIT(bcm56960_a0_tm_mmu_thd_en(unit));

    SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_tdm_reconfigure(unit));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_tm_port_sched_state_resource_fill(unit, TRUE, num_ports,
                                                   old_cfg, new_cfg));
    SHR_IF_ERR_EXIT(bcm56960_a0_bcmtm_mmu_config_init(unit));

    SHR_IF_ERR_EXIT
        (bcm56960_a0_tm_mmu_port_map(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56960_a0_bcmtm_chip_tm_init(int unit)
{
    int i;
    int phy_port, log_port;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_drv_info_alloc(unit));
    bcm56960_a0_bcmtm_dev_info_set(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    drv_info->frequency = 850;
    drv_info->oobfc_clock_ns = 8;

    for (i = 0; i < BCMTM_NUM_PPORTS_PER_CHIP_MAX; i++) {
        drv_info->port_num_lanes[i] = -1;
        drv_info->port_p2l_mapping[i] = -1;
        drv_info->port_l2p_mapping[i] = -1;

        drv_info->port_l2i_mapping[i] = -1;


        drv_info->port_p2m_mapping[i] = -1;
        drv_info->port_p2m_local_mapping[i] = -1;
        drv_info->port_pipe_mapping[i] = -1;

        drv_info->num_mc_cosq[i] = -1;
        drv_info->num_uc_cosq[i] = -1;
        drv_info->uc_cosq_base[i] = -1;
        drv_info->mc_cosq_base[i] = -1;

        drv_info->port_speed_max[i] = -1;
        drv_info->port_speed_cur[i] = -1;
    }

    for (i = 0; i < BCMTM_NUM_CPU_PORTS; i++) {
        (drv_info->cpu_ports[i]) = cpu_ports[i];
        phy_port = cpu_ports[i].phy_port;
        log_port = cpu_ports[i].log_port;


        drv_info->port_num_lanes[log_port] = 1;
        drv_info->port_p2l_mapping[phy_port] = cpu_ports[i].log_port;
        drv_info->port_l2p_mapping[log_port] = cpu_ports[i].phy_port;

        drv_info->port_l2i_mapping[log_port] = cpu_ports[i].idb_port;


        drv_info->port_p2m_mapping[phy_port] = cpu_ports[i].mmu_port;
        drv_info->port_p2m_local_mapping[phy_port] = cpu_ports[i].mmu_port & 0x3f;
        drv_info->port_pipe_mapping[log_port] = cpu_ports[i].pipe;

        drv_info->num_mc_cosq[log_port] = 48;
        drv_info->num_uc_cosq[log_port] = 0;
        drv_info->uc_cosq_base[log_port] = 0;
        drv_info->mc_cosq_base[log_port] = 330;

        drv_info->port_speed_max[log_port] = TDM_SPEED_10G;
        drv_info->port_speed_cur[log_port] = TDM_SPEED_10G;
    }

    for (i = 0; i < BCMTM_NUM_LB_PORTS; i++) {
        (drv_info->lb_ports[i]) = lb_ports[i];
        phy_port = lb_ports[i].phy_port;
        log_port = lb_ports[i].log_port;

        drv_info->port_num_lanes[log_port] = 1;
        drv_info->port_p2l_mapping[phy_port] = lb_ports[i].log_port;
        drv_info->port_l2p_mapping[log_port] = lb_ports[i].phy_port;

        drv_info->port_l2i_mapping[log_port] = lb_ports[i].idb_port;

        drv_info->port_p2m_mapping[phy_port] = lb_ports[i].mmu_port;
        drv_info->port_p2m_local_mapping[phy_port] = lb_ports[i].mmu_port & 0x3f;
        drv_info->port_pipe_mapping[log_port] = lb_ports[i].pipe;

        drv_info->num_mc_cosq[log_port] = 10;
        drv_info->num_uc_cosq[log_port] = 0;
        drv_info->uc_cosq_base[log_port] = 0;
        drv_info->mc_cosq_base[log_port] = 320;

        drv_info->port_speed_max[log_port] = TDM_SPEED_10G;
        drv_info->port_speed_cur[log_port] = TDM_SPEED_10G;
    }
    /* Initialize Shaper config. */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_tm_shaper_config_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56960_a0_bcmtm_chip_tm_cleanup(int unit)
{

    SHR_FUNC_ENTER(unit);
    bcmtm_drv_info_free(unit);
    SHR_FUNC_EXIT();
}
