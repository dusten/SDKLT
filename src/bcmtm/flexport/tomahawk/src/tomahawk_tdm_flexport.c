/*! \file tomahawk_tdm_flexport.c
 *
 * TDM calendar apis for flexport operations.
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

#include "../../main/flexport_top.h"
#include <bcmtm/chip/bcm56960_a0_tm_tdm_top_internal.h>


#if defined(BCM_TOMAHAWK_SUPPORT)


/*** START SDK API COMMON CODE ***/


/*! @file tomahawk_tdm_flexport.c
 *  @brief Scheduler TDM init.
 *  Details are shown below.
 */


/*! @fn int soc_tomahawk_set_tdm_soc_pkg(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              tdm_soc_t *soc_pkg, int new_or_prev)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param *soc_pkg a pointer to variable of type tdm_soc_t
 *  @param new_or_prev if set function uses out_port_map; else function uses
 *         in_port_map
 *  @brief Helper function to allocate & populate state and speed in a tdm_soc_t
 *         structure
 * Description:
 *      Helper function to allocate & populate state and speed in a tdm_soc_t
 *      structure
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 *      *soc_pkg - pointer to tdm_soc_t structure
 *      new_or_prev - specifies if its the previous or the new soc_pkg
 *                    new_or_prev==1 means new, else means previous;
 *                    that is, if new_or_prev==1 use out_port_map, else
 *                    use in_port_map
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_set_tdm_soc_pkg(int                        unit,
                              soc_port_schedule_state_t *port_schedule_state,
                              tdm_cfg_t                  *tdm_cfg,
                              int                        new_or_prev)
{
    int port, phy_port, lane, idx;
    uint32 port_speeds[_TH_PHY_PORTS_PER_DEV] = {0};
    uint32 port_states[_TH_PHY_PORTS_PER_DEV] = {0}; /* tdm_port_state_t */
    soc_port_map_type_t *port_map;

    if (new_or_prev == 1) {
        port_map = &port_schedule_state->out_port_map;
    } else {
        port_map = &port_schedule_state->in_port_map;
    }

    for (port = 0; port < _TH_DEV_PORTS_PER_DEV; port++) {
        if (port_map->log_port_speed[port] > 0 ) {
            phy_port = port_map->port_l2p_mapping[port];
            if (phy_port == -1) {
                continue;
            }
            if (SOC_PBMP_MEMBER(port_map->hg2_pbm,port)) {
                port_states[phy_port] =
                    SOC_PBMP_MEMBER(port_map->oversub_pbm, port) ?
                    TDM_STATE_OVERSUB_HG : TDM_STATE_LINERATE_HG;
                for (lane = 1; lane < port_map->port_num_lanes[port]; lane++) {
                    port_states[phy_port + lane] = TDM_STATE_COMBINE_HG;
                }
            } else {
                port_states[phy_port] =
                    SOC_PBMP_MEMBER(port_map->oversub_pbm, port) ?
                    TDM_STATE_OVERSUB : TDM_STATE_LINERATE;
                for (lane = 1; lane < port_map->port_num_lanes[port]; lane++) {
                    port_states[phy_port + lane] = TDM_STATE_COMBINE;
                }
            }
            port_speeds[phy_port] = port_map->log_port_speed[port];
        }
    }


    /* Init TDM configure parameters. */
    TDM_MEMSET(tdm_cfg, 0, sizeof(tdm_cfg_t));
    tdm_cfg->unit = unit;
    tdm_cfg->dev_id = 0;
    tdm_cfg->clk_freq_core = port_schedule_state->frequency;
    tdm_cfg->num_gports = TH_NUM_EXT_PORTS;
    tdm_cfg->flex_en = port_schedule_state->is_flexport;
    tdm_cfg->chk_en = 0;
    tdm_cfg->chip_vars = NULL;

    for (idx = 0;
         idx < _TH_PHY_PORTS_PER_DEV && idx < TDM_MAX_NUM_GPORTS; idx++) {
        tdm_cfg->port_speeds[idx] = port_speeds[idx];
        tdm_cfg->port_states[idx] = port_states[idx];
    }

    /* Shift state to left one position; required by C_TDM */
    for (idx = 1; idx < TDM_MAX_NUM_GPORTS; idx++) {
        tdm_cfg->port_states[idx - 1] = tdm_cfg->port_states[idx];
    }


    /* Just printings */
    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "frequency: %dMHz\n"),
                  port_schedule_state->frequency));
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "port speed & state map:")));
        for (idx = 0; idx < TH_NUM_EXT_PORTS; idx++) {
            if (idx % 8 == 0) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "phy_port=%d state=%1d speed=%3dG;    "),
                      idx, port_states[idx], port_speeds[idx]/1000));
        }
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_copy_prev_tables(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              tdm_mod_t *_tdm_pkg)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param *_tdm_pkg a pointer to variable of type tdm_mod_t
 *  @brief Helper function to allocate & populate state and speed in a tdm_soc_t
 *         structure
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 *      *soc_pkg - pointer to tdm_soc_t structure
 *      new_or_prev - specifies if its the previous or the new soc_pkg
 *                    new_or_prev==1 means new, else means previous;
 *                    that is, if new_or_prev==1 use out_port_map, else
 *                    use in_port_map
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_tdm_copy_prev_tables(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    tdm_mod_t *_tdm_pkg)
{
    int pipe;
    soc_tomahawk_flex_scratch_t *cookie;

    /* Also, copy in cookie the prev OVS tables & pkt scheduler */
    cookie = port_schedule_state->cookie;
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        sal_memcpy(&cookie->prev_tdm_ingress_schedule_pipe[pipe],
                   &port_schedule_state->tdm_ingress_schedule_pipe[pipe],
                   sizeof(soc_tdm_schedule_pipe_t));
        sal_memcpy(&cookie->prev_tdm_egress_schedule_pipe[pipe],
                   &port_schedule_state->tdm_egress_schedule_pipe[pipe],
                   sizeof(soc_tdm_schedule_pipe_t));
        sal_memcpy(&cookie->curr_tdm_ingress_schedule_pipe[pipe],
                   &port_schedule_state->tdm_ingress_schedule_pipe[pipe],
                   sizeof(soc_tdm_schedule_pipe_t));
        sal_memcpy(&cookie->curr_tdm_egress_schedule_pipe[pipe],
                   &port_schedule_state->tdm_egress_schedule_pipe[pipe],
                   sizeof(soc_tdm_schedule_pipe_t));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_set_out_map(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to calculate port_schedule_state->out_port_map from
 *         port_schedule_state->out_port_map and port_schedule_state->resource[]
 * Description:
 *      Helper function
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_tdm_set_out_map(int                        unit,
                              soc_port_schedule_state_t *port_schedule_state)
{
    int i, logical_port, physical_port;
    int prev_physical_port, prev_mmu_port;
    soc_tomahawk_flex_scratch_t *cookie;

    /* Copy in_port_map in out_port_map */
    for (i= 0; i< SOC_MAX_NUM_PORTS; i++) {
        port_schedule_state->out_port_map.log_port_speed[i]   =
            port_schedule_state->in_port_map.log_port_speed[i];
        port_schedule_state->out_port_map.port_num_lanes[i]   =
            port_schedule_state->in_port_map.port_num_lanes[i];
    }

    sal_memcpy(port_schedule_state->out_port_map.port_p2l_mapping,
               port_schedule_state->in_port_map.port_p2l_mapping,
               sizeof(int)*_TH_PHY_PORTS_PER_DEV);
    sal_memcpy(port_schedule_state->out_port_map.port_l2p_mapping,
               port_schedule_state->in_port_map.port_l2p_mapping,
               sizeof(int)*_TH_DEV_PORTS_PER_DEV);
    sal_memcpy(port_schedule_state->out_port_map.port_p2m_mapping,
               port_schedule_state->in_port_map.port_p2m_mapping,
               sizeof(int)*_TH_PHY_PORTS_PER_DEV);
    sal_memcpy(port_schedule_state->out_port_map.port_m2p_mapping,
               port_schedule_state->in_port_map.port_m2p_mapping,
               sizeof(int)*_TH_MMU_PORTS_PER_DEV);
    sal_memcpy(port_schedule_state->out_port_map.port_l2i_mapping,
               port_schedule_state->in_port_map.port_l2i_mapping,
               sizeof(int)*_TH_DEV_PORTS_PER_DEV);

    sal_memcpy(&port_schedule_state->out_port_map.physical_pbm,
               &port_schedule_state->in_port_map.physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&port_schedule_state->out_port_map.hg2_pbm,
               &port_schedule_state->in_port_map.hg2_pbm, sizeof(pbmp_t));
    sal_memcpy(&port_schedule_state->out_port_map.management_pbm,
               &port_schedule_state->in_port_map.management_pbm, sizeof(pbmp_t));
    sal_memcpy(&port_schedule_state->out_port_map.oversub_pbm,
               &port_schedule_state->in_port_map.oversub_pbm, sizeof(pbmp_t));


    if (1 == port_schedule_state->is_flexport) {
        /* Update for ports that are down
         * Remove all the mappings and bitmaps
         */
        for (i = 0; i < port_schedule_state->nport; i++) {
            logical_port = port_schedule_state->resource[i].logical_port;
            physical_port = port_schedule_state->resource[i].physical_port;
            if (-1 == physical_port) { /* that is, port down */
                port_schedule_state->out_port_map.log_port_speed[logical_port]
                    = 0;
                port_schedule_state->out_port_map.port_num_lanes[logical_port]
                    = 0;
                /* Remove previous log_port to phy_port mapping */
                port_schedule_state->out_port_map.port_l2p_mapping[logical_port
                ] = -1;
                /* Remove previous log_port to idb_port mapping */
                port_schedule_state->out_port_map.port_l2i_mapping[logical_port
                ] = -1;
                /* Remove previous phy_port to log_port mapping */
                prev_physical_port =
                    port_schedule_state->in_port_map.port_l2p_mapping[
                        logical_port];
                port_schedule_state->out_port_map.port_p2l_mapping[
                    prev_physical_port] = -1;
                /* Remove previous phy_port to mmu_port mapping */
                port_schedule_state->out_port_map.port_p2m_mapping[
                    prev_physical_port] = -1;
                /* Remove previous mmu_port to phy_port mapping */
                prev_mmu_port =
                    port_schedule_state->in_port_map.port_p2m_mapping[
                        prev_physical_port];
                port_schedule_state->out_port_map.port_m2p_mapping[
                    prev_mmu_port] = -1;
                /* Remove corresponding bitmaps */
                SOC_PBMP_PORT_REMOVE(
                    port_schedule_state->out_port_map.physical_pbm,
                    prev_physical_port);
                SOC_PBMP_PORT_REMOVE(port_schedule_state->out_port_map.hg2_pbm,
                                     logical_port);
                SOC_PBMP_PORT_REMOVE(
                    port_schedule_state->out_port_map.oversub_pbm, logical_port);
                SOC_PBMP_PORT_REMOVE(
                    port_schedule_state->out_port_map.management_pbm,
                    logical_port);                                                                    /* That's optional */
            }
        }

        /* Update for ports that are brought up */
        for (i = 0; i < port_schedule_state->nport; i++) {
            logical_port = port_schedule_state->resource[i].logical_port;
            physical_port = port_schedule_state->resource[i].physical_port;
            if (-1 != physical_port) { /* that is, port up */
                port_schedule_state->out_port_map.log_port_speed[logical_port]
                    =
                        port_schedule_state->resource[i].speed;
                port_schedule_state->out_port_map.port_num_lanes[logical_port]
                    =
                        port_schedule_state->resource[i].num_lanes;
                /* Add log_port to phy_port mapping */
                port_schedule_state->out_port_map.port_l2p_mapping[logical_port
                ] = physical_port;
                /* Add log_port to idb_port mapping */
                port_schedule_state->out_port_map.port_l2i_mapping[logical_port
                ] =
                    port_schedule_state->resource[i].idb_port;
                /* Add phy_port to log_port mapping */
                port_schedule_state->out_port_map.port_p2l_mapping[
                    physical_port] = logical_port;
                /* Add phy_port to mmu_port mapping */
                port_schedule_state->out_port_map.port_p2m_mapping[
                    physical_port] =
                    port_schedule_state->resource[i].mmu_port;
                /* Add mmu_port to phy_port mapping */
                port_schedule_state->out_port_map.port_m2p_mapping[
                    port_schedule_state->resource[i].mmu_port] = physical_port;
                /* Add corresponding bitbams */
                SOC_PBMP_PORT_ADD(
                    port_schedule_state->out_port_map.physical_pbm,
                    physical_port);
                if (_SHR_PORT_ENCAP_HIGIG2 ==
                    port_schedule_state->resource[i].encap) {
                    SOC_PBMP_PORT_ADD(port_schedule_state->out_port_map.hg2_pbm,
                                      logical_port);
                }
                if (1 == port_schedule_state->resource[i].oversub) {
                    SOC_PBMP_PORT_ADD(
                        port_schedule_state->out_port_map.oversub_pbm,
                        logical_port);
                }
                if ((_TH_PHY_PORT_MNG0 == physical_port) ||
                    (_TH_PHY_PORT_MNG1 == physical_port) ) {
                    SOC_PBMP_PORT_ADD(
                        port_schedule_state->out_port_map.management_pbm,
                        logical_port);                                                                 /* That's optional */
                }
            }
        }
    }

    /* Copy exact out_port_map.log_port_speed[port] to cookie->exact_out_log_port_speed[] */
    cookie = port_schedule_state->cookie;
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        cookie->exact_out_log_port_speed[i] =
            port_schedule_state->out_port_map.log_port_speed[i];
    }

    /* Remap input/output & resource speeds to ETH bitrates */
    soc_tomahawk_port_schedule_speed_remap(unit, port_schedule_state);

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_flexport_ovs_add_rem_port(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port,
 *              int rem_add_cmd, int set_idb, int set_mmu )
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param logical_port Logical port
 *  @param rem_add_cmd OPCODE for operation performed
 *  @param set_idb acts only in IDB block
 *  @param set_mmu acts only in MMU block
 *  @brief This low granularity API adds or removes a single port
 * Description:
 *      This low granularity API adds or removes a single port
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 *      int logical_port - logical port to be removed/added
 *      int rem_add_cmd is coded as:
 *           0 : port is DOWN and needs to be removed from OVS tables, pblk,
 *               pkt scheduler
 *           1 : port is UP and needs to be added to OVS tables, pblk, pkt
 *               scheduler
 *           2 : port is part of a consolidation move (remove only from OVS
 *               tables; not PBLK or PKT scheduler)
 *           3 : port is part of a consolidation move (add only in OVS tables;
 *               not PBLK or PKT scheduler)
 *      int set_idb - act only in IDB block
 *      int set_mmu - act only in MMU block
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_tdm_flexport_ovs_add_rem_port(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int logical_port,
    int rem_add_cmd,
    int set_idb,
    int set_mmu )
{
    soc_reg_t reg;
    int i, pipe, hpipe=0, group=0, slot=0, id = 0, mode, hpipe_i, group_i,
        slot_i;
    int phy_port, idb_port, mmu_port, inst;
    uint32 rval;
    int pm_num, pblk_cal_idx;
    int pblk_valid=0;
    int ovs_class, ovs_sister_spacing;
    int idb_same_spacing=0, mmu_same_spacing=0, is_hsp=0;

    bcmbd_pt_dyn_info_t pt_dyn_info;
    soc_port_map_type_t *port_map;
    soc_tdm_schedule_pipe_t  *prev_tdm_schedule_pipe, *new_tdm_schedule_pipe;
    soc_tdm_schedule_pipe_t  *tdm_schedule_pipe, *curr_tdm_schedule_pipe;
    int subport;
    int cfg_en_PBLK_CALENDAR=0;
    int cfg_en_OVR_SUB_GRP_CFG=0;
    int in_or_out_port_map;
    int found;
    soc_tomahawk_flex_scratch_t *cookie;

    static const soc_reg_t idb_grp_cfg_regs[_TH_PIPES_PER_DEV][2] = {
        {IS_OVR_SUB_GRP_CFG_PIPE0r, IS_OVR_SUB_GRP_CFG_PIPE0r},
        {IS_OVR_SUB_GRP_CFG_PIPE1r, IS_OVR_SUB_GRP_CFG_PIPE1r},
        {IS_OVR_SUB_GRP_CFG_PIPE2r, IS_OVR_SUB_GRP_CFG_PIPE2r},
        {IS_OVR_SUB_GRP_CFG_PIPE3r, IS_OVR_SUB_GRP_CFG_PIPE3r},
    };

    static const soc_reg_t mmu_grp_cfg_regs[2] = {
        OVR_SUB_GRP_CFGr, OVR_SUB_GRP_CFGr
    };

    static const soc_reg_t idb_grp_ovs_regs[_TH_PIPES_PER_DEV][2][6] = {
        {
            {IS_OVR_SUB_GRP0_TBL_PIPE0r,
             IS_OVR_SUB_GRP1_TBL_PIPE0r,
             IS_OVR_SUB_GRP2_TBL_PIPE0r,
             IS_OVR_SUB_GRP3_TBL_PIPE0r,
             IS_OVR_SUB_GRP4_TBL_PIPE0r,
             IS_OVR_SUB_GRP5_TBL_PIPE0r },
            {IS_OVR_SUB_GRP0_TBL_PIPE0r,
             IS_OVR_SUB_GRP1_TBL_PIPE0r,
             IS_OVR_SUB_GRP2_TBL_PIPE0r,
             IS_OVR_SUB_GRP3_TBL_PIPE0r,
             IS_OVR_SUB_GRP4_TBL_PIPE0r,
             IS_OVR_SUB_GRP5_TBL_PIPE0r }
        },
        {
            {IS_OVR_SUB_GRP0_TBL_PIPE1r,
             IS_OVR_SUB_GRP1_TBL_PIPE1r,
             IS_OVR_SUB_GRP2_TBL_PIPE1r,
             IS_OVR_SUB_GRP3_TBL_PIPE1r,
             IS_OVR_SUB_GRP4_TBL_PIPE1r,
             IS_OVR_SUB_GRP5_TBL_PIPE1r },
            {IS_OVR_SUB_GRP0_TBL_PIPE1r,
             IS_OVR_SUB_GRP1_TBL_PIPE1r,
             IS_OVR_SUB_GRP2_TBL_PIPE1r,
             IS_OVR_SUB_GRP3_TBL_PIPE1r,
             IS_OVR_SUB_GRP4_TBL_PIPE1r,
             IS_OVR_SUB_GRP5_TBL_PIPE1r }
        },
        {
            {IS_OVR_SUB_GRP0_TBL_PIPE2r,
             IS_OVR_SUB_GRP1_TBL_PIPE2r,
             IS_OVR_SUB_GRP2_TBL_PIPE2r,
             IS_OVR_SUB_GRP3_TBL_PIPE2r,
             IS_OVR_SUB_GRP4_TBL_PIPE2r,
             IS_OVR_SUB_GRP5_TBL_PIPE2r },
            {IS_OVR_SUB_GRP0_TBL_PIPE2r,
             IS_OVR_SUB_GRP1_TBL_PIPE2r,
             IS_OVR_SUB_GRP2_TBL_PIPE2r,
             IS_OVR_SUB_GRP3_TBL_PIPE2r,
             IS_OVR_SUB_GRP4_TBL_PIPE2r,
             IS_OVR_SUB_GRP5_TBL_PIPE2r }
        },
        {
            {IS_OVR_SUB_GRP0_TBL_PIPE3r,
             IS_OVR_SUB_GRP1_TBL_PIPE3r,
             IS_OVR_SUB_GRP2_TBL_PIPE3r,
             IS_OVR_SUB_GRP3_TBL_PIPE3r,
             IS_OVR_SUB_GRP4_TBL_PIPE3r,
             IS_OVR_SUB_GRP5_TBL_PIPE3r },
            {IS_OVR_SUB_GRP0_TBL_PIPE3r,
             IS_OVR_SUB_GRP1_TBL_PIPE3r,
             IS_OVR_SUB_GRP2_TBL_PIPE3r,
             IS_OVR_SUB_GRP3_TBL_PIPE3r,
             IS_OVR_SUB_GRP4_TBL_PIPE3r,
             IS_OVR_SUB_GRP5_TBL_PIPE3r },
        }
    };

    static const soc_reg_t mmu_grp_ovs_regs[2][6] = {
        {OVR_SUB_GRP0_TBLr, OVR_SUB_GRP1_TBLr,
         OVR_SUB_GRP2_TBLr, OVR_SUB_GRP3_TBLr,
         OVR_SUB_GRP4_TBLr, OVR_SUB_GRP5_TBLr },
        {OVR_SUB_GRP0_TBLr, OVR_SUB_GRP1_TBLr,
         OVR_SUB_GRP2_TBLr, OVR_SUB_GRP3_TBLr,
         OVR_SUB_GRP4_TBLr, OVR_SUB_GRP5_TBLr }
    };

    static const soc_reg_t idb_pblk_cal_regs[_TH_PIPES_PER_DEV][2][8] = {
        {
            {IS_PBLK0_CALENDAR_PIPE0r, IS_PBLK1_CALENDAR_PIPE0r,
             IS_PBLK2_CALENDAR_PIPE0r, IS_PBLK3_CALENDAR_PIPE0r,
             IS_PBLK4_CALENDAR_PIPE0r, IS_PBLK5_CALENDAR_PIPE0r,
             IS_PBLK6_CALENDAR_PIPE0r, IS_PBLK7_CALENDAR_PIPE0r },
            {IS_PBLK0_CALENDAR_PIPE0r, IS_PBLK1_CALENDAR_PIPE0r,
             IS_PBLK2_CALENDAR_PIPE0r, IS_PBLK3_CALENDAR_PIPE0r,
             IS_PBLK4_CALENDAR_PIPE0r, IS_PBLK5_CALENDAR_PIPE0r,
             IS_PBLK6_CALENDAR_PIPE0r, IS_PBLK7_CALENDAR_PIPE0r }
        },
        {
            {IS_PBLK0_CALENDAR_PIPE1r, IS_PBLK1_CALENDAR_PIPE1r,
             IS_PBLK2_CALENDAR_PIPE1r, IS_PBLK3_CALENDAR_PIPE1r,
             IS_PBLK4_CALENDAR_PIPE1r, IS_PBLK5_CALENDAR_PIPE1r,
             IS_PBLK6_CALENDAR_PIPE1r, IS_PBLK7_CALENDAR_PIPE1r },
            {IS_PBLK0_CALENDAR_PIPE1r, IS_PBLK1_CALENDAR_PIPE1r,
             IS_PBLK2_CALENDAR_PIPE1r, IS_PBLK3_CALENDAR_PIPE1r,
             IS_PBLK4_CALENDAR_PIPE1r, IS_PBLK5_CALENDAR_PIPE1r,
             IS_PBLK6_CALENDAR_PIPE1r, IS_PBLK7_CALENDAR_PIPE1r }
        },
        {
            {IS_PBLK0_CALENDAR_PIPE2r, IS_PBLK1_CALENDAR_PIPE2r,
             IS_PBLK2_CALENDAR_PIPE2r, IS_PBLK3_CALENDAR_PIPE2r,
             IS_PBLK4_CALENDAR_PIPE2r, IS_PBLK5_CALENDAR_PIPE2r,
             IS_PBLK6_CALENDAR_PIPE2r, IS_PBLK7_CALENDAR_PIPE2r },
            {IS_PBLK0_CALENDAR_PIPE2r, IS_PBLK1_CALENDAR_PIPE2r,
             IS_PBLK2_CALENDAR_PIPE2r, IS_PBLK3_CALENDAR_PIPE2r,
             IS_PBLK4_CALENDAR_PIPE2r, IS_PBLK5_CALENDAR_PIPE2r,
             IS_PBLK6_CALENDAR_PIPE2r, IS_PBLK7_CALENDAR_PIPE2r }
        },
        {
            {IS_PBLK0_CALENDAR_PIPE3r, IS_PBLK1_CALENDAR_PIPE3r,
             IS_PBLK2_CALENDAR_PIPE3r, IS_PBLK3_CALENDAR_PIPE3r,
             IS_PBLK4_CALENDAR_PIPE3r, IS_PBLK5_CALENDAR_PIPE3r,
             IS_PBLK6_CALENDAR_PIPE3r, IS_PBLK7_CALENDAR_PIPE3r },
            {IS_PBLK0_CALENDAR_PIPE3r, IS_PBLK1_CALENDAR_PIPE3r,
             IS_PBLK2_CALENDAR_PIPE3r, IS_PBLK3_CALENDAR_PIPE3r,
             IS_PBLK4_CALENDAR_PIPE3r, IS_PBLK5_CALENDAR_PIPE3r,
             IS_PBLK6_CALENDAR_PIPE3r, IS_PBLK7_CALENDAR_PIPE3r }
        }
    };


    static const soc_reg_t mmu_pblk_cal_regs[2][8] = {
        {PBLK0_CALENDARr, PBLK1_CALENDARr,
         PBLK2_CALENDARr, PBLK3_CALENDARr,
         PBLK4_CALENDARr, PBLK5_CALENDARr,
         PBLK6_CALENDARr, PBLK7_CALENDARr },
        {PBLK0_CALENDARr, PBLK1_CALENDARr,
         PBLK2_CALENDARr, PBLK3_CALENDARr,
         PBLK4_CALENDARr, PBLK5_CALENDARr,
         PBLK6_CALENDARr, PBLK7_CALENDARr }
    };


    static int pblk_slots[SOC_TH_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TH_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TH_PORT_RATIO_QUAD */
    };

    cookie = port_schedule_state->cookie;
    pipe = logical_port / _TH_DEV_PORTS_PER_PIPE;


    prev_tdm_schedule_pipe = &cookie->prev_tdm_ingress_schedule_pipe[pipe];
    new_tdm_schedule_pipe  =
        &port_schedule_state->tdm_ingress_schedule_pipe[pipe];

    /* if both IDB and MMU in one shot use either ingress or egress;
     * else is the assigned one*/
    if ((set_idb == 1) && (set_mmu == 0)) {
        curr_tdm_schedule_pipe = &cookie->curr_tdm_ingress_schedule_pipe[pipe];
    } else if ((set_idb == 0) && (set_mmu == 1)) {
        curr_tdm_schedule_pipe = &cookie->curr_tdm_egress_schedule_pipe[pipe];
    } else {
        curr_tdm_schedule_pipe = &cookie->curr_tdm_ingress_schedule_pipe[pipe];
    }


    switch(rem_add_cmd) {
    /* 0 : port is DOWN and needs to be removed from prev OVS tables, pblk,
     * pkt scheduler */
    case 0: port_map = &port_schedule_state->in_port_map;
        tdm_schedule_pipe = prev_tdm_schedule_pipe;         /* use previous */
        phy_port = port_map->port_l2p_mapping[logical_port];
        idb_port = 0x3f;
        mmu_port = 0x3f;
        id = 0x7;
        in_or_out_port_map = 1;
        cfg_en_PBLK_CALENDAR = 1;
        pblk_valid = 0;
        idb_same_spacing = 0; mmu_same_spacing = 0;
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "PORT DOWN   pipe=%1d"), pipe));
        break;
    /* 1 : port is UP   and needs to be added   to   new OVS tables, pblk,
     * pkt scheduler */
    case 1: port_map = &port_schedule_state->out_port_map;
        tdm_schedule_pipe = new_tdm_schedule_pipe;         /* use new */
        phy_port = port_map->port_l2p_mapping[logical_port];
        idb_port = port_map->port_l2i_mapping[logical_port] & 0x3f;
        mmu_port = port_map->port_p2m_mapping[phy_port] & 0x3f;
        id = port_map->port_p2PBLK_inst_mapping[phy_port];
        in_or_out_port_map = 0;
        cfg_en_PBLK_CALENDAR = 1;
        pblk_valid = 1;
        soc_tomahawk_get_hsp_info(unit,
                                   port_map->log_port_speed[logical_port],
                                   1, 0, &is_hsp, &idb_same_spacing);
        soc_tomahawk_get_hsp_info(unit,
                                   port_map->log_port_speed[logical_port],
                                   0, 1, &is_hsp, &mmu_same_spacing);
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "PORT   UP   pipe=%1d"), pipe));
        break;
    /* 2 : port is part of a consolidation move (remove only from current
     * OVS tables; not PBLK or PKT scheduler) */
    case 2: port_map = &port_schedule_state->out_port_map;
        tdm_schedule_pipe = prev_tdm_schedule_pipe;         /* use new */
        phy_port = port_map->port_l2p_mapping[logical_port];
        idb_port = 0x3f;
        mmu_port = 0x3f;
        id = 0x7;
        cfg_en_PBLK_CALENDAR = 0;
        in_or_out_port_map = 0;
        idb_same_spacing = 0; mmu_same_spacing = 0;
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "MOVE-REMOVE pipe=%1d"), pipe));
        break;
    /* 3 : port is part of a consolidation move (add only in new OVS tables;
     * not PBLK or PKT scheduler) */
    case 3: port_map = &port_schedule_state->out_port_map;
        tdm_schedule_pipe = new_tdm_schedule_pipe;         /* use new */
        phy_port = port_map->port_l2p_mapping[logical_port];
        idb_port = port_map->port_l2i_mapping[logical_port] & 0x3f;
        mmu_port = port_map->port_p2m_mapping[phy_port] & 0x3f;
        id = port_map->port_p2PBLK_inst_mapping[phy_port];
        cfg_en_PBLK_CALENDAR = 0;
        in_or_out_port_map = 0;
        soc_tomahawk_get_hsp_info(unit,
                                   port_map->log_port_speed[logical_port],
                                   1, 0, &is_hsp, &idb_same_spacing);
        soc_tomahawk_get_hsp_info(unit,
                                   port_map->log_port_speed[logical_port],
                                   0, 1, &is_hsp, &mmu_same_spacing);
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "MOVE-   ADD pipe=%1d"), pipe));
        break;
    default: LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                       "Wrong rem_add_cmd command=%d for logical_port %d"),
                       rem_add_cmd, logical_port));
        port_map = &port_schedule_state->in_port_map;
        tdm_schedule_pipe = new_tdm_schedule_pipe;
        phy_port = 0;
        idb_port = 0x3f;
        mmu_port = 0x3f;
        in_or_out_port_map = 0;
        break;
    }

    /* If port to be removed; find phy_port in previous OVS tables (ingress);
     * if to be added then find phy_port in current OVS tables (egress)
     */
    found = 0;
    /* Find the location of the port to be removed/ added in OVS tables */
    for (hpipe_i = 0; hpipe_i < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe_i++) {
        for (group_i = 0; group_i < _TH_OVS_GROUP_COUNT_PER_HPIPE;
             group_i++) {
            for (slot_i = 0; slot_i < _TH_OVS_GROUP_TDM_LENGTH; slot_i++) {
                if (phy_port ==
                    tdm_schedule_pipe->tdm_schedule_slice[hpipe_i].
                    oversub_schedule[group_i][slot_i]) {
                    found = 1;
                    hpipe = hpipe_i; group = group_i; slot = slot_i;
                    break;
                }
            }
            if (found == 1) {break; }
        }
        if (found == 1) {break; }
    }

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              " hpipe=%1d log_port=%3d phy_port=%3d group=%1d slot=%1d \n"),
              hpipe, logical_port, phy_port, group, slot));


    /* Actual removal / add from OVR_SUB_GRP0_TBL
     * IDB OverSub Groups */
    if (set_idb == 1) {
        rval = 0;
        reg = idb_grp_ovs_regs[pipe][hpipe][group];
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, slot, rval));
    }
    /* MMU OverSub Groups */
    inst = pipe;
    if (set_mmu == 1) {
        rval = 0;
        reg = mmu_grp_ovs_regs[hpipe][group];
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port);
        BCMTM_PT_DYN_INFO(pt_dyn_info, slot, inst);

        SOC_IF_ERROR_RETURN
            (bcmtm_pt_indexed_write(unit, reg, -1, &(pt_dyn_info), &rval));
    }

    /* Actual update in OVR_SUB_GRP_CFG
     * Here, tdm_egress_schedule_pipe works as a temp table that gets updated
     * when add/remove/move port
     * The initial state is previous OVS tables and the final state will be
     * new OVS tables
     */
    ovs_class = 0;
    switch(rem_add_cmd) {
    /*  Remove port; check if group becomes empty */
    case 0:
    case 2:
        /* Remove from temp table */
        curr_tdm_schedule_pipe->tdm_schedule_slice[hpipe].oversub_schedule[
            group][slot] = TH_NUM_EXT_PORTS;
        /* If group gets empty due to remove then configure ovs_class to 0;
         * otherwise skip
         */
        cfg_en_OVR_SUB_GRP_CFG = 1;
        for (i = 0; i < _TH_OVS_GROUP_TDM_LENGTH; i++) {
            if (TH_NUM_EXT_PORTS >
                curr_tdm_schedule_pipe->tdm_schedule_slice[hpipe].
                oversub_schedule[
                    group][i]) {
                cfg_en_OVR_SUB_GRP_CFG = 0;
                break;
            }
        }
        if (cfg_en_OVR_SUB_GRP_CFG == 1)  {ovs_class = 0; }
        break;
    /*  Check if group previously empty; Add port */
    case 1:
    case 3:
        /* If group is empty before then configure ovs_class to the right one;
         * otherwise skip
         */
        cfg_en_OVR_SUB_GRP_CFG = 1;
        for (i = 0; i < _TH_OVS_GROUP_TDM_LENGTH; i++) {
            if (TH_NUM_EXT_PORTS >
                curr_tdm_schedule_pipe->tdm_schedule_slice[hpipe].
                oversub_schedule[
                    group][i]) {
                cfg_en_OVR_SUB_GRP_CFG = 0;
                break;
            }
        }
        if (cfg_en_OVR_SUB_GRP_CFG == 1)  {
            _soc_tomahawk_speed_to_ovs_class_mapping(
                unit, port_map->log_port_speed[logical_port],
                &ovs_class);
        }
        /* Add to temp table */
        curr_tdm_schedule_pipe->tdm_schedule_slice[hpipe].oversub_schedule[
            group][slot] = phy_port;
        break;
    }

    /* Actual update in OVR_SUB_GRP_CFG */
    /* Update only if:
     * - port removed & group gets empty;
     * - group previously empty & port is added;
     */
    if (cfg_en_OVR_SUB_GRP_CFG == 1) {
        ovs_sister_spacing = 4;
        if (set_idb == 1) {
            rval = 0;
            reg = idb_grp_cfg_regs[pipe][hpipe];
            /* on the ingress, all ports have HSP same spacing */
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf,
                              idb_same_spacing);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf,
                              ovs_sister_spacing);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, group, rval));
        }
        if (set_mmu == 1) {
            rval = 0;
            reg = mmu_grp_cfg_regs[hpipe];
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf, mmu_same_spacing);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf,
                              ovs_sister_spacing);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_rawport_set(unit, reg, inst, group, rval));
        }
    }

    /* Actual update in PBLK_CALENDAR
     * Remove/Add PBLK only entries associated with this port */
    if (cfg_en_PBLK_CALENDAR == 1) {
        pm_num  = (phy_port - 1) / _TH_PORTS_PER_PBLK;
        subport = (phy_port - 1) % _TH_PORTS_PER_PBLK;
        pblk_cal_idx = port_map->port_p2PBLK_inst_mapping[phy_port];
        soc_tomahawk_port_ratio_get(unit, port_schedule_state, pm_num, &mode,
                                     in_or_out_port_map);
        for (i = 0; i < 6; i++) {
            /* Remove or Add on <i> pblk position */
            if ((pblk_cal_idx >= 0) &&(pblk_slots[mode][i] == subport)) {
                if (set_idb == 1) {
                    reg  = idb_pblk_cal_regs[pipe][hpipe][pblk_cal_idx];
                    soc_reg_field_set(unit, reg,  &rval,  VALIDf, pblk_valid);
                    /* on ingress, all active ports have the same same_spacing */
                    soc_reg_field_set(unit, reg,  &rval,  SPACINGf,
                                      idb_same_spacing);
                    soc_reg_field_set(unit, reg,  &rval,  PORT_NUMf, idb_port);
                    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                                                              REG_PORT_ANY, i,
                                                              rval));
                }
                if (set_mmu == 1) {
                    reg  = mmu_pblk_cal_regs[hpipe][pblk_cal_idx];
                    soc_reg_field_set(unit, reg,  &rval,  VALIDf, pblk_valid);
                    /* on ingress, all active ports have the same same_spacing */
                    soc_reg_field_set(unit, reg,  &rval,  SPACINGf,
                                      mmu_same_spacing);
                    soc_reg_field_set(unit, reg,  &rval,  PORT_NUMf, mmu_port);
                    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst,
                                                              i, rval));
                }
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_flexport_remove_ports_sel(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int set_idb, int set_mmu)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param set_idb acts only in IDB block
 *  @param set_mmu acts only in MMU block
 *  @brief This task will remove all ports from TDM scheduler that are going DOWN
 *         at FlexPort
 * Description:
 *      This task will remove all ports from TDM scheduler that are going DOWN
 *      at FlexPort
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 *      int set_idb - act only in IDB block
 *      int set_mmu - act only in MMU block
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_tdm_flexport_remove_ports_sel(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int set_idb, int set_mmu )
{
    int i, logical_port, physical_port;
    int rem_add_cmd;
    int is_port_oversub;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "START soc_tomahawk_tdm_flexport_remove_ports\n")));

    /* Update for ports that are down
     * Remove ports that are going down from TDM scheduler
     */
    /* 0 : port is DOWN and needs to be removed from OVS tables, pblk,
     * pkt scheduler
     */
    rem_add_cmd = 0;
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        is_port_oversub =
            SOC_PBMP_MEMBER(port_schedule_state->in_port_map.oversub_pbm,
                            logical_port) ?
            1 : 0;
        if (( BCMPC_INVALID_PPORT == physical_port) && (1 == is_port_oversub)) {
            /* that is, OVS port going DOWN */
            soc_tomahawk_tdm_flexport_ovs_add_rem_port(unit,
                                                        port_schedule_state,
                                                        logical_port,
                                                        rem_add_cmd,
                                                        set_idb,
                                                        set_mmu );
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_flexport_add_ports_sel(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int set_idb, int set_mmu)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param set_idb acts only in IDB block
 *  @param set_mmu acts only in MMU block
 *  @brief This task will add all ports into TDM scheduler that are brought UP
 *         at FlexPort
 * Description:
 *      This task will add all ports into TDM scheduler that are brought UP at
 *      FlexPort
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 *      int set_idb - act only in IDB block
 *      int set_mmu - act only in MMU block
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_tdm_flexport_add_ports_sel(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int set_idb,
    int set_mmu )
{
    int i, logical_port, physical_port;
    int rem_add_cmd;
    int is_port_oversub;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "START soc_tomahawk_tdm_flexport_add_ports\n")));

    /* Update for ports that are down
     * Add ports that are going down from TDM scheduler
     */
    /* 0 : port is DOWN and needs to be removed from OVS tables, pblk, pkt
     * scheduler
     */
    rem_add_cmd = 1;
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        is_port_oversub =
            SOC_PBMP_MEMBER(port_schedule_state->out_port_map.oversub_pbm,
                            logical_port) ?
            1 : 0;
        if (( BCMPC_INVALID_PPORT != physical_port) && (1 == is_port_oversub)) {
            /* that is, OVS port UP */
            soc_tomahawk_tdm_flexport_ovs_add_rem_port(unit,
                                                        port_schedule_state,
                                                        logical_port,
                                                        rem_add_cmd,
                                                        set_idb,
                                                        set_mmu );
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_flexport_ovs_consolidate_per_speed(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int pipe, int hpipe, int ovs_class, int set_idb, int set_mmu)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param pipe pipe number
 *  @param hpipe half pipe number
 *  @param ovs_class ovs_class
 *  @param set_idb acts only in IDB block
 *  @param set_mmu acts only in MMU block
 *  @brief This task will consolidate all OVS groups in pipe, hpipe that belong
 *         to ovs_class speed
 * Return Value:
 *      SOC_E_*
 */

/**
 * Within each Half Pipe, first consolidate the speeds that require less or
 * equal number of groups compared to previous OVS tables.
 * Then consolidate the speeds that require higher number of groups as
 * compared with previous OVS tables.
 *
 * FOREACH Half Pipe hp_num in current PIPE DO {
 *     FOREACH speed inside {10G, 20G, 25G, 40G, 50G, 100G} DO {
 *         IF the number of groups within hp_num with speed from new_ovs_tables[][]
 *         is equal or less than the number of groups with speed from
 *         prev_ovs_tables[][] then DO {
 *             STEP 2.1 Consolidate groups with speed in Half Pipe hp_num;
 *         }
 *     }
 *     FOREACH speed inside {10G, 20G, 25G, 40G, 50G, 100G} DO {
 *         IF the number of groups within hp_num with speed from new_ovs_tables[][]
 *         is higher than the number of groups with speed from prev_ovs_tables[][]
 *         then DO {
 *             STEP 2.1 Consolidate groups with speed in Half Pipe hp_num;
 *         }
 *     }
 * }
 *
 * STEP 2.1 This step consolidates all groups with class speed in Half Pipe
 * hp_num. It will take a few iterations in do-while loop until all ports are
 * moved to the new locations (usually 2-3 iterations).
 * NOTE: do-while loop is required; some of the ports cannot be moved in a
 * single iteration because the new position is occupied by a port which
 * eventually will be moved too.
 *
 * IF Half Pipe hp_num doesn't have any active ports with speed exit STEP 2.1;
 * DO {
 *     port_moved=FAlSE;
 *     FOREACH active phy_port in groups from curr_ovs_tables[][] with speed
 *     within Half Pipe hp_num DO {
 *         IF phy_port in curr_ovs_tables[][] has the same position as in
 *         new_ovs_tables[][] then skip this port;
 *         ELSE {
 *             find the new position in the new_ovs_tables[][] (new_group_idx
 *             & new_pos);
 *             IF there is a port already active in curr_ovs_tables[new_group_idx]
 *             [new_pos] skip for now this phy_port;
 *             ELSE {
 *                 Move phy_port from current position (prev_group_idx & prev_pos)
 *                 to new position (new_group_idx & new_pos) - call a task for
 *                 that (follow Section 6.6.3.2.4.3 from TH FlexPort document);
 *                 IF group new_group_idx is empty then update
 *                 HPIPE<hp_num>_OVR_SUB_GRP_CFG::SPEED field with class speed;
 *                 update accordingly curr_ovs_tables[][];
 *                 port_moved=TRUE;
 *             }
 *         }
 *     }
 * } WHILE (port_moved);
 */
STATIC int
soc_tomahawk_tdm_flexport_ovs_consolidate_per_speed(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int pipe, int hpipe, int ovs_class, int set_idb, int set_mmu )
{
    int i, j, group, slot, physical_port, logical_port;
    soc_tomahawk_flex_scratch_t *cookie;
    soc_tdm_schedule_t *curr_tdm_schedule_hpipe, *new_tdm_schedule_hpipe;
    int ovs_class_i;
    int grp_bmp[_TH_OVS_GROUP_COUNT_PER_HPIPE];
    int num_of_ovs_grps;

    int port_moved;
    int dst_group=0, dst_slot=0;
    int rem_add_cmd, found;

    cookie = port_schedule_state->cookie;

    /* if both IDB and MMU in one shot use either ingress or egress; else, use
     * the right one
     */
    if ((set_idb == 1) && (set_mmu == 0)) {
        curr_tdm_schedule_hpipe =
            &cookie->curr_tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[hpipe];
    } else if ((set_idb == 0) && (set_mmu == 1)) {
        curr_tdm_schedule_hpipe =
            &cookie->curr_tdm_egress_schedule_pipe[pipe].tdm_schedule_slice[hpipe];
    } else {
        curr_tdm_schedule_hpipe =
            &cookie->curr_tdm_ingress_schedule_pipe[pipe].tdm_schedule_slice[hpipe];
    }

    new_tdm_schedule_hpipe =
        &port_schedule_state->tdm_ingress_schedule_pipe[pipe].
        tdm_schedule_slice[
            hpipe];

    /* Find how many active groups are in this ovs_class*/
    num_of_ovs_grps = 0;
    sal_memset(grp_bmp, 0, sizeof(grp_bmp));
    for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
        for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
            physical_port =
                curr_tdm_schedule_hpipe->oversub_schedule[group][slot];
            if (physical_port < TH_NUM_EXT_PORTS) {
                logical_port =
                    port_schedule_state->out_port_map.port_p2l_mapping[
                        physical_port
                    ];
                _soc_tomahawk_speed_to_ovs_class_mapping(
                    unit,
                    port_schedule_state->
                    out_port_map.log_port_speed[logical_port],
                    &ovs_class_i);
                if (ovs_class == ovs_class_i) {
                    num_of_ovs_grps++;
                    grp_bmp[group] = 1;
                }
                break;
            }
        }
    }

    if (num_of_ovs_grps == 0) {
        return SOC_E_NONE; /* nothing to do */
    }

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "START soc_tomahawk_tdm_flexport_ovs_consolidate_per_speed "
              "pipe=%d, hpipe=%d, ovs_class=%d\n"),
              pipe, hpipe, ovs_class));

    do {
        port_moved = 0;

        for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
            for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
                physical_port =
                    curr_tdm_schedule_hpipe->oversub_schedule[group][slot];
                if (physical_port < TH_NUM_EXT_PORTS) {
                    /* if this port is in the same place in the new OVS table
                     * then skip */
                    if (physical_port ==
                        new_tdm_schedule_hpipe->oversub_schedule[group][slot]) {
                        continue;
                    } else {
                        /* find the new position of the port in the new OVS
                         * tables */
                        found = 0;
                        for (i = 0; i < _TH_OVS_GROUP_COUNT_PER_HPIPE; i++) {
                            for (j = 0; j < _TH_OVS_GROUP_TDM_LENGTH; j++) {
                                if (physical_port ==
                                    new_tdm_schedule_hpipe->oversub_schedule[i]
                                    [j]) {
                                    found = 1;
                                    dst_group = i; dst_slot = j;
                                    break;
                                }
                            }
                            if (found == 1) {
                                break;
                            }
                        }
                        if (found == 0) {
                            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                      "___ERROR could not find group and slot "
                                      "in new OVS for phy_port=%d"),
                                      physical_port));
                        }
                        /* Now, check if a valid port exists in curr OVS tables
                         * at position [dst_group][dst_slot] */
                        if (curr_tdm_schedule_hpipe->oversub_schedule[dst_group]
                            [dst_slot] < TH_NUM_EXT_PORTS) {
                            continue;
                        } else {
                            logical_port =
                                port_schedule_state->out_port_map.
                                port_p2l_mapping[
                                    physical_port];
                            /* move port from [group][slot] to [dst_group]
                             * [dst_slot] */
                            rem_add_cmd = 3; /* Add first*/
                            soc_tomahawk_tdm_flexport_ovs_add_rem_port(
                                unit,
                                port_schedule_state,
                                logical_port,
                                rem_add_cmd,
                                set_idb,
                                set_mmu );
                            rem_add_cmd = 2; /* Remove */
                            soc_tomahawk_tdm_flexport_ovs_add_rem_port(
                                unit,
                                port_schedule_state,
                                logical_port,
                                rem_add_cmd,
                                set_idb,
                                set_mmu );
                            port_moved = 1;
                        }
                    }
                }
            }
        }
    } while (port_moved == 1);


    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_flexport_ovs_consolidate_sel(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int set_idb, int set_mmu )
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param set_idb acts only in IDB block
 *  @param set_mmu acts only in MMU block
 *  @brief This task will consolidate all OVS groups in the device
 * Return Value:
 *      SOC_E_*
 */


/**
 * Within each Half Pipe, first consolidate the speeds that require less or
 * equal number of groups compared to previous OVS tables.
 * Then consolidate the speeds that require higher number of groups as compared
 * with previous OVS tables.
 *
 * FOREACH Half Pipe hp_num in current PIPE DO {
 *     FOREACH speed inside {10G, 20G, 25G, 40G, 50G, 100G} DO {
 *         IF the number of groups within hp_num with speed from new_ovs_tables[][]
 *         is equal or less than the number of groups with speed from
 *         prev_ovs_tables[][] then DO {
 *             STEP 2.1 Consolidate groups with speed in Half Pipe hp_num;
 *         }
 *     }
 *     FOREACH speed inside {10G, 20G, 25G, 40G, 50G, 100G} DO {
 *         IF the number of groups within hp_num with speed from new_ovs_tables[][]
 *         is higher than the number of groups with speed from prev_ovs_tables[][]
 *         then DO {
 *             STEP 2.1 Consolidate groups with speed in Half Pipe hp_num;
 *         }
 *     }
 * }
 */
STATIC int
soc_tomahawk_tdm_flexport_ovs_consolidate_sel(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int set_idb, int set_mmu )
{
    int i, logical_port, physical_port;
    int pipe, hpipe, group, slot;
    /* these arrays keep the number of groups corresponding to an ovs_class
     * ovs_class is in range 0:6 and coding is
     * 0 :   0G
     * 1 :  10G
     * 2 :  20G
     * 3 :  25G
     * 4 :  40G
     * 5 :  50G
     * 5 : 100G
     */
    int prev_num_grp[_TH_PIPES_PER_DEV][_TH_OVS_HPIPE_COUNT_PER_PIPE][7];
    int new_num_grp[_TH_PIPES_PER_DEV][_TH_OVS_HPIPE_COUNT_PER_PIPE][7];
    uint32 flexport_ovs_pipe_map; /* indicates if any OVS flex in a pipe */
    int is_port_oversub;
    int ovs_class;
    soc_tomahawk_flex_scratch_t *cookie;

    cookie = port_schedule_state->cookie;

    /* Set flexpor & ovs pipe_map */
    flexport_ovs_pipe_map = 0;
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port  = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        is_port_oversub = 0;
        if (BCMPC_INVALID_PPORT == physical_port) {
            /* that is, port DOWN; look in prev OVS port map */
            is_port_oversub =
                SOC_PBMP_MEMBER(port_schedule_state->in_port_map.oversub_pbm,
                                logical_port) ?
                1 : 0;
        } else {
            /* that is, port UP; look in new OVS port map */
            is_port_oversub =
                SOC_PBMP_MEMBER(port_schedule_state->out_port_map.oversub_pbm,
                                logical_port) ?
                1 : 0;
        }
        if (1 == is_port_oversub) {
            pipe = logical_port / _TH_DEV_PORTS_PER_PIPE;
            flexport_ovs_pipe_map |= 1 << pipe;
        }
    }


    /* Set prev_num_grp & new_num_grp */
    sal_memset(prev_num_grp, 0, sizeof(prev_num_grp));
    sal_memset(new_num_grp,  0, sizeof(prev_num_grp));
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(flexport_ovs_pipe_map & (1 << pipe))) {
            continue;
        }
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                /* set prev_num_grp */
                for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
                    physical_port =
                        cookie->prev_tdm_ingress_schedule_pipe[pipe].
                        tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    if (physical_port < TH_NUM_EXT_PORTS) {
                        logical_port =
                            port_schedule_state->in_port_map.port_p2l_mapping[
                                physical_port];
                        _soc_tomahawk_speed_to_ovs_class_mapping(
                            unit,
                            port_schedule_state
                            ->in_port_map.log_port_speed[logical_port],
                            &ovs_class);
                        prev_num_grp[pipe][hpipe][ovs_class]++;
                        break;
                    }
                }
                /* set new_num_grp */
                for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
                    physical_port =
                        port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                        tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    if (physical_port < TH_NUM_EXT_PORTS) {
                        logical_port =
                            port_schedule_state->out_port_map.port_p2l_mapping[
                                physical_port];
                        _soc_tomahawk_speed_to_ovs_class_mapping(
                            unit,
                            port_schedule_state
                            ->out_port_map.log_port_speed[logical_port],
                            &ovs_class);
                        new_num_grp[pipe][hpipe][ovs_class]++;
                        break;
                    }
                }
            }
        }
    }

    /* Consolidate OVS tables in the whole device */
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(flexport_ovs_pipe_map & (1 << pipe))) {
            continue;
        }
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* FIRST, consolidate speeds that are reducing the number of groups */
            for (ovs_class = 1; ovs_class < 7; ovs_class++) {
                if (new_num_grp[pipe][hpipe][ovs_class] <=
                    prev_num_grp[pipe][hpipe][ovs_class]) {
                    soc_tomahawk_tdm_flexport_ovs_consolidate_per_speed(
                        unit, port_schedule_state,
                        pipe,
                        hpipe,
                        ovs_class,
                        set_idb,
                        set_mmu);
                }
            }
            /* SECONDLY, consolidate speeds that are incresing the number of
             * groups
             */
            for (ovs_class = 1; ovs_class < 7; ovs_class++) {
                if (new_num_grp[pipe][hpipe][ovs_class] >
                    prev_num_grp[pipe][hpipe][ovs_class]) {
                    soc_tomahawk_tdm_flexport_ovs_consolidate_per_speed(
                        unit, port_schedule_state,
                        pipe,
                        hpipe,
                        ovs_class,
                        set_idb,
                        set_mmu);
                }
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_calculate_ovs_per_pipe(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int pipe, int prev_or_new)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param pipe pipe_number
 *  @param prev_or_new calculate OVS ratios for prev or new OVS tables;
 *         0 - prev; 1 - new
 *  @brief Calculate ovs_ratios per PIPE and HPIPE
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_tdm_calculate_ovs_per_pipe(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int pipe, int prev_or_new)
{
    int cal_len, length;
    int lr_slots;
    int pipe_ovs_slots;
    int hpipe_ovs_slots[_TH_OVS_HPIPE_COUNT_PER_PIPE];
    soc_port_map_type_t *port_map;
    int pipe_ovs_ratio; /* per PIPE ovs_ratio */
    int hpipe_ovs_ratio[_TH_OVS_HPIPE_COUNT_PER_PIPE]; /* per Half PIPE ovs_ratio */

    int hpipe, group, slot, port_slots, physical_port, logical_port;
    soc_tomahawk_flex_scratch_t *cookie;

    if (prev_or_new == 0) { /* thats OVS tables before FlexPort */
        port_map = &port_schedule_state->in_port_map;
    } else {
        port_map = &port_schedule_state->out_port_map;
    }

    /* Get calendar length */
    cal_len =
        port_schedule_state->tdm_egress_schedule_pipe[pipe].tdm_schedule_slice[
            0].
        cal_len;
    for (length = cal_len; length > 0; length--) {
        if (port_schedule_state->tdm_egress_schedule_pipe[pipe].
            tdm_schedule_slice[0].linerate_schedule[length - 1] !=
            TH_NUM_EXT_PORTS) {
            break;
        }
    }
    lr_slots = length - TH_NUM_ANCL; /* slots assigned to OVS scheduling */

    pipe_ovs_slots = 0;
    for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
        hpipe_ovs_slots[hpipe] = 0;
        for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
            for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
                physical_port =
                    port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                    tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                if (physical_port != TH_NUM_EXT_PORTS) {
                    logical_port = port_map->port_p2l_mapping[physical_port];
                    port_slots = port_map->log_port_speed[logical_port] / 2500;
                    hpipe_ovs_slots[hpipe] += port_slots;
                }
            }
        }
        pipe_ovs_slots += hpipe_ovs_slots[hpipe];
    }

    /* Calculate ovs_ratios */
    if (pipe_ovs_slots == 0) { /* Line Rate */
        pipe_ovs_ratio = 10;
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            hpipe_ovs_ratio[hpipe] = 10;
        }
    } else { /* OVS case */
        int t_ovs_ratio;
        /* this will give a small error */
        t_ovs_ratio = (pipe_ovs_slots * 1000) / lr_slots;
        if ( (port_schedule_state->frequency == 1700) ||
             (t_ovs_ratio <= 1500) ) {
            /* At MAX Frequency OVS is always 1.5
             */
            pipe_ovs_ratio = 15;
        } else {
            pipe_ovs_ratio = 20;
        }
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* this will give a small div error */
            t_ovs_ratio = (hpipe_ovs_slots[hpipe] * 2000) / lr_slots;
            if ( (port_schedule_state->frequency == 1700) ||
                 (t_ovs_ratio <= 1500) ) {
                hpipe_ovs_ratio[hpipe] = 15;
            } else {
                hpipe_ovs_ratio[hpipe] = 20;
            }
        }
    }

    /* Assign in cookie members */
    cookie = port_schedule_state->cookie;
    if (prev_or_new == 0) { /* thats OVS tables before FlexPort */
        cookie->prev_pipe_ovs_ratio[pipe] = pipe_ovs_ratio;
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            cookie->prev_hpipe_ovs_ratio[pipe][hpipe] = hpipe_ovs_ratio[hpipe];
        }
    } else {
        cookie->new_pipe_ovs_ratio[pipe] = pipe_ovs_ratio;
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            cookie->new_hpipe_ovs_ratio[pipe][hpipe] = hpipe_ovs_ratio[hpipe];
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_calculate_ovs(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int pipe, int prev_or_new)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @param prev_or_new calculate OVS ratios for prev or new OVS tables;
 *         0 - prev; 1- new
 *  @brief Calculate ovs_ratios per PIPE and HPIPE
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_tdm_calculate_ovs(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int                        prev_or_new)
{
    int pipe;
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        soc_tomahawk_tdm_calculate_ovs_per_pipe(unit, port_schedule_state,
                                                 pipe,
                                                 prev_or_new);
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_tdm_calculation_flexport(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief API to calculate TDM tables for FlexPort
 * Description:
 *     API to calculate TDM tables for FlexPort
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_tdm_calculation_flexport(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int pipe, hpipe;
    int group, idx, cal_id, cal_grp;

    bcmtm_drv_t *tm_drv;
    tdm_mod_t *_tdm_pkg = NULL;
    tdm_cfg_t tdm_cfg;
    bcmtm_tdm_drv_t th_tdm_drv;
    int result = TDM_PASS;
    int mmu_cal_id, idb_cal_id;


    /*
     *  if (is_flexport)
     *  then { out_port_map = (in_port_map + soc_port_resource_t::inst) }
     *  else { out_port_map = in_port_map }
     */
    /* If init then just copy in_port_map to out_port_map*/
    soc_tomahawk_tdm_set_out_map(unit, port_schedule_state);
    soc_print_port_schedule_state(unit, port_schedule_state);

    /* Compute OVS ratios before FlexPort*/
    soc_tomahawk_tdm_calculate_ovs(unit, port_schedule_state, 0);

    /*
     *  This step is the same for init or for FlexPort
     *  Create _tdm_pkg->_chip_data.soc_pkg*
     */
    /* soc_pkg is used for both init and FlexPort */
    soc_tomahawk_set_tdm_soc_pkg(unit, port_schedule_state, &tdm_cfg, 1);

    /* Init TDM driver functions. */
    TDM_MEMSET(&th_tdm_drv, 0, sizeof(bcmtm_tdm_drv_t));

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->tdm_drv(0, &th_tdm_drv)) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "Unable to attach TDM driver functions.\n")));
        return SOC_E_FAIL;
    }

    /* Run TDM algorithm to generate TDM calendars. */
    if (th_tdm_drv.tdm_init(&_tdm_pkg) != TDM_PASS) {
        TDM_ERROR0("TDM internal init FAILED.\n");
        result = TDM_FAIL;
    }

    /* Allocate and populate previous state, speed arrays, etc. in
     * _tdm_pkg->_prev_chip_data.soc_pkg
     */
    if (1 == port_schedule_state->is_flexport) {
        soc_tomahawk_set_tdm_soc_pkg(unit, port_schedule_state,
                                      &(_tdm_pkg->prev_user_data.cfg),
                                      0);
        /* Allocate memeory for previous TDM tables in _tdm_pkg */
        /* tdm_th_alloc_prev_chip_data(_tdm_pkg); */
        soc_tomahawk_tdm_copy_prev_tables(unit, port_schedule_state, _tdm_pkg);
    }


    if (result == TDM_PASS) {
        if (th_tdm_drv.tdm_cfg(_tdm_pkg, &tdm_cfg) != TDM_PASS) {
            TDM_ERROR0("TDM internal cfg FAILED.\n");
            result = TDM_FAIL;
        }
    }
    if (result == TDM_PASS) {
        if (th_tdm_drv.tdm_run(_tdm_pkg) != TDM_PASS) {
            TDM_ERROR0("TDM internal calendar generation FAILED.\n");
            result = TDM_FAIL;
        }
    }

    if (result == TDM_PASS) {
        /* IDB calendar (0-3), MMU calendar (4-7) */
        for (cal_id = 0; cal_id < _TH_PIPES_PER_DEV; cal_id++) {
            switch (cal_id) {
            case 0:
                idb_cal_id = 0;
                mmu_cal_id = 4;
                break;
            case 1:
                idb_cal_id = 1;
                mmu_cal_id = 5;
                break;
            case 2:
                idb_cal_id = 2;
                mmu_cal_id = 6;
                break;
            case 3:
                idb_cal_id = 3;
                mmu_cal_id = 7;
                break;
#ifndef __COVERITY__
            default:
                return SOC_E_FAIL;
#endif
            }

            /* IDB TDM main calendar */
            sal_memcpy(
                port_schedule_state->tdm_ingress_schedule_pipe[cal_id].
                tdm_schedule_slice[0].linerate_schedule,
                _tdm_pkg->user_data.cals[idb_cal_id].lr.cal,
                sizeof(int) * _TH_TDM_LENGTH);

            /* MMU TDM main calendar */
            sal_memcpy(
                port_schedule_state->tdm_egress_schedule_pipe[cal_id].
                tdm_schedule_slice[0].linerate_schedule,
                _tdm_pkg->user_data.cals[mmu_cal_id].lr.cal,
                sizeof(int) * _TH_TDM_LENGTH);

            /* IDB and MMU OverSub Groups */
            for (idx = 0; idx < _TH_OVS_GROUP_COUNT_PER_PIPE; idx++) {
                /* Group 0:5 -> hpipe 0 */
                hpipe = idx / _TH_OVS_GROUP_COUNT_PER_HPIPE;
                cal_grp = idx % _TH_OVS_GROUP_COUNT_PER_HPIPE;
                sal_memcpy(
                    port_schedule_state->tdm_ingress_schedule_pipe[
                        cal_id].tdm_schedule_slice[hpipe].oversub_schedule[
                        cal_grp],
                    _tdm_pkg->user_data.cals[idb_cal_id].ovsb.cal[idx],
                    sizeof(int) * _TH_OVS_GROUP_TDM_LENGTH);
                sal_memcpy(
                    port_schedule_state->tdm_egress_schedule_pipe[cal_id].
                    tdm_schedule_slice[hpipe].oversub_schedule[cal_grp],
                    _tdm_pkg->user_data.cals[mmu_cal_id].ovsb.cal[idx],
                    sizeof(int) * _TH_OVS_GROUP_TDM_LENGTH);
            }
        }
    }

    /* Free TDM resource. */
    th_tdm_drv.tdm_free(_tdm_pkg);


    /* Compute OVS ratios after FlexPort*/
    soc_tomahawk_tdm_calculate_ovs(unit, port_schedule_state, 1);


    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Pipe %d idb_tdm:"), pipe));
            for (idx = 0; idx < _TH_TDM_LENGTH; idx++) {
                if (idx % 16 == 0) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
                }
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " %3d"),
                          port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                          tdm_schedule_slice[0].linerate_schedule[idx] ));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
            for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
                for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE;
                     group++) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                              "Pipe %d hpipe %d group %d ovs_tdm"),
                              pipe, hpipe, group));
                    for (idx = 0; idx < _TH_OVS_GROUP_TDM_LENGTH;
                         idx++) {
                        if (idx == 0) {
                            LOG_DEBUG(BSL_LS_SOC_PORT,
                                      (BSL_META_U(unit, "\n    ")));
                        }
                        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " %3d"),
                                  port_schedule_state->tdm_ingress_schedule_pipe
                                  [pipe].tdm_schedule_slice[hpipe].
                                  oversub_schedule[group]
                                  [idx]));
                    }
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
                }
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "Pipe %d hpipe %d IDB PKT SCHEDULER\n"),
                          pipe, hpipe));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Pipe %d mmu_tdm:"), pipe));
            for (idx = 0; idx < _TH_TDM_LENGTH; idx++) {
                if (idx % 16 == 0) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
                }
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " %3d"),
                          port_schedule_state->tdm_egress_schedule_pipe[pipe].
                          tdm_schedule_slice[0].linerate_schedule[idx] ));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_tdm_flexport_remove_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief This task will remove all ports from TDM scheduler that are going
 *         DOWN at FlexPort
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_tdm_flexport_remove_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    return (soc_tomahawk_tdm_flexport_remove_ports_sel(unit,
                                                        port_schedule_state, 1,
                                                        1));
}


/*! @fn int soc_tomahawk_tdm_flexport_ovs_consolidate(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief This task will consolidate all OVS groups in the device
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_tdm_flexport_ovs_consolidate(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    return (soc_tomahawk_tdm_flexport_ovs_consolidate_sel(unit,
                                                           port_schedule_state,
                                                           1, 1));
}

/*! @fn int soc_tomahawk_tdm_flexport_add_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief This task will add all ports into TDM scheduler that are brought UP
 *         at FlexPort
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_tdm_flexport_add_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    return (soc_tomahawk_tdm_flexport_add_ports_sel(unit, port_schedule_state,
                                                     1, 1));
}


/*! @fn int soc_tomahawk_tdm_flexport_idb(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief This task will do TDM FlexPort for IDB - OVS ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_tdm_flexport_idb(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_flexport_remove_ports_sel(unit,
                                                   port_schedule_state, 1, 0));
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_flexport_ovs_consolidate_sel(unit,
                                                   port_schedule_state, 1, 0));
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_flexport_add_ports_sel(unit,
                                                   port_schedule_state, 1, 0));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_tdm_flexport_mmu(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief This task will do TDM FlexPort for MMU - OVS ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_tdm_flexport_mmu(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_flexport_remove_ports_sel(unit,
                                                   port_schedule_state, 0, 1));
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_flexport_ovs_consolidate_sel(unit,
                                                   port_schedule_state, 0, 1));
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_flexport_add_ports_sel(unit,
                                                   port_schedule_state, 0, 1));
    return SOC_E_NONE;
}


/*** END SDK API COMMON CODE ***/


#endif /* BCM_TOMAHAWK_SUPPORT */
