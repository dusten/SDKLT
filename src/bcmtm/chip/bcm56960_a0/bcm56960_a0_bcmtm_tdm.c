/*! \file bcm56960_a0_bcmtm_tdm.c
 *
 * TDM library functions for bcm56960_a0 device.
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

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>

#include "include/bcm56960_a0/bcm56960_a0_bcmtm_types.h"
#include "include/bcm56960_a0/bcm56960_a0_bcmtm_drv.h"
#include <bcmtm/chip/bcm56960_a0_tm_tdm_methods_internal.h>
#include <bcmtm/chip/bcm56960_a0_tm_tdm_defines_internal.h>

#include "../../flexport/tomahawk/include/tomahawk_flexport_defines.h"

/*******************************************************************************
 * Local definitions
 */
extern int
soc_tomahawk_tdm_init(int unit, soc_port_schedule_state_t *port_schedule_state,
                        bcmtm_tdm_drv_t *th_tdm_drv);

extern int
soc_tomahawk_port_ratio_get(int unit,
                            soc_port_schedule_state_t *port_schedule_state,
                             int clport, int *mode, int prev_or_new);


#define IS_CPU_PORT(u, p)    bcmtm_port_is_cpu(u, p)
#define IS_LB_PORT(u, p)     bcmtm_port_is_lb(u, p)


#define IS_MANAGEMENT_PORT(u, p) 0

#define _TH_MMU_PORT_MNG0  1
#define _TH_MMU_PORT_MNG1  1
#define _TH_DEV_PORT_MNG0  1
#define _TH_DEV_PORT_MNG1 1
#define _TH_IDB_PORT_CPU_MNG 1

/*******************************************************************************
 * Private functions
 */
static int
soc_th_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    _bcmtm_tomahawk_tdm_t *tdm;
    bcmtm_drv_info_t *drv;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, phy_port, is_flexport, group, slot;
    int *port_p2PBLK_inst_mapping;

    bcmtm_drv_info_get(unit, &drv);
    tdm = &(drv->tdm);

    is_flexport = port_schedule_state->is_flexport;

    port_schedule_state->calendar_type = drv->fabric_port_enable ?
                                        _TH_TDM_CALENDAR_UNIVERSAL :
                                        _TH_TDM_CALENDAR_ETHERNET_OPTIMIZED;

    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    for (pipe = 0; pipe <  _TH_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        tdm_ischd->num_slices = _TH_OVS_HPIPE_COUNT_PER_PIPE;
        tdm_eschd->num_slices = _TH_OVS_HPIPE_COUNT_PER_PIPE;

        if (is_flexport) {
            /* TDM Calendar is always in slice 0 */
            sal_memcpy(tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].idb_tdm,
                        sizeof(int)*_TH_TDM_LENGTH);
            sal_memcpy(tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].mmu_tdm,
                        sizeof(int)*_TH_TDM_LENGTH);
        }

        /* OVS */
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* IDB OVERSUB*/
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TH_TDM_LENGTH;
            sched->num_ovs_groups = _TH_OVS_GROUP_COUNT_PER_HPIPE;
            sched->ovs_group_len = _TH_OVS_GROUP_TDM_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _TH_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                    for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                    }
                }
                for (slot = 0; slot < _TH_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }

            /* MMU OVERSUB */
            sched = &tdm_eschd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TH_TDM_LENGTH;
            sched->num_ovs_groups = _TH_OVS_GROUP_COUNT_PER_HPIPE;
            sched->ovs_group_len = _TH_OVS_GROUP_TDM_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _TH_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                    for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                        }
                    }
                for (slot = 0; slot < _TH_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }
        }
    }

    if (is_flexport) {
        port_p2PBLK_inst_mapping =
            port_schedule_state->in_port_map.port_p2PBLK_inst_mapping;
        /* pblk info for phy_port */
        for (phy_port = 1; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
            if (phy_port < TH_NUM_EXT_PORTS) {
                port_p2PBLK_inst_mapping[phy_port] =
                                        tdm->pblk_info[phy_port].pblk_cal_idx;
            } else {
                port_p2PBLK_inst_mapping[phy_port] = -1;
            }
        }
    }
    return SHR_E_NONE;
}


static int
soc_th_soc_tdm_update(int unit,
                      soc_port_schedule_state_t *port_schedule_state)
{
    bcmtm_drv_info_t *drv;
    _bcmtm_tomahawk_tdm_t *tdm;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
#if 1
    int pipe, hpipe, port, phy_port, clport, slot;
#else
    int pipe, hpipe, port, phy_port, slot;
#endif
    int length, ovs_core_slot_count, ovs_io_slot_count, port_slot_count;
    int index, group;

    bcmtm_drv_info_get(unit, &drv);
    tdm = &(drv->tdm);

    /* Copy info from soc_port_schedule_state_t to _bcmtm_tomahawk_tdm_t */
    for (pipe = 0; pipe <  _TH_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        /* TDM Calendar is always in slice 0 */
        sal_memcpy(tdm->tdm_pipe[pipe].idb_tdm,
                    tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_TH_TDM_LENGTH);
        sal_memcpy(tdm->tdm_pipe[pipe].mmu_tdm,
                    tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_TH_TDM_LENGTH);

        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (slot = 0; slot < _TH_OVS_GROUP_TDM_LENGTH; slot++) {
                    tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot] =
                        sched->oversub_schedule[group][slot];
                }
            }
#if 0
            for (slot = 0; slot < _TH_PKT_SCH_LENGTH; slot++) {
                tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot] =
                    sched->pkt_sch_or_ovs_space[0][slot];

            }
#endif
        }
    }

    /* pblk info init  */
    for (phy_port = 1; phy_port < TH_NUM_EXT_PORTS; phy_port++) {
        tdm->pblk_info[phy_port].pblk_cal_idx = -1;
        tdm->pblk_info[phy_port].pblk_hpipe_num = -1;
    }

    /* pblk info for phy_port */
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (index = 0; index < _TH_OVS_GROUP_TDM_LENGTH; index++) {
                    phy_port =
                        tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port < TH_NUM_EXT_PORTS) {
                        tdm->pblk_info[phy_port].pblk_hpipe_num = hpipe;
                        tdm->pblk_info[phy_port].pblk_cal_idx =
                            port_schedule_state->out_port_map.port_p2PBLK_inst_mapping[phy_port];
                    }
                }
            }
        }
    }

    /* CLPORT port ratio */
    for (clport = 0; clport < _TH_PBLKS_PER_DEV; clport++) {
        soc_tomahawk_port_ratio_get(unit, port_schedule_state, clport,
            &tdm->port_ratio[clport], 1);
    }

    /* Calculate the oversub ratio */
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        /* At MAX Frequency OVS is always 3:2 */
        if (drv->frequency == 1700) {
            for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
                tdm->ovs_ratio_x1000[pipe][hpipe] = 1500;
            }
            continue;
        }

        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        /* Count number of OVSB_TOKEN assigned by the TDM code */
        for (length = _TH_TDM_LENGTH; length > 0; length--) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[length - 1]
                    != TH_NUM_EXT_PORTS) {
                break;
            }
        }
        ovs_core_slot_count = 0;
        for (index = 0; index < length; index++) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[index]
                    == TH_OVSB_TOKEN) {
                ovs_core_slot_count++;
            }
        }

        /* At line-rate with less than max frequency, OVS picks the 2:1 */
        if (ovs_core_slot_count == 0) {
            for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
                tdm->ovs_ratio_x1000[pipe][hpipe] = 2000;
            }
        }

        /* Count number of slot needed for half-pipe's oversub I/O bandwidth */
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            ovs_io_slot_count = 0;
            for (group = 0; group < _TH_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (index = 0; index < _TH_OVS_GROUP_TDM_LENGTH; index++) {
                    phy_port = tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port == TH_NUM_EXT_PORTS) {
                        continue;
                    }
                    port = drv->port_p2l_mapping[phy_port];
                    if ((port == -1) || IS_CPU_PORT(unit, port) ||
                        IS_LB_PORT(unit, port) ||
                        IS_MANAGEMENT_PORT(unit, port)) {
#if 0
                        LOG_ERROR(BSL_LS_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "Flexport: Invalid physical "
                                              "port %d in OverSub table.\n"),
                                   phy_port));
#endif
                        continue;
                    }
                    port_slot_count = drv->port_init_speed[port] / 2500;
                    ovs_io_slot_count += port_slot_count;
                }
            }
            if (ovs_core_slot_count != 0) {
                tdm->ovs_ratio_x1000[pipe][hpipe] =
                    ovs_io_slot_count * 1000 / (ovs_core_slot_count / 2);
            }
        }
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */
int
bcm56960_a0_bcmtm_tdm_init(int unit)
{
    bcmtm_pport_t port, log_port;
    bcmtm_drv_info_t *drv_info;
    soc_port_schedule_state_t *port_schedule_state;
    soc_port_map_type_t *in_portmap;
    int rv;
    int num_hg = 0;
    bcmtm_tdm_drv_t th_tdm_drv;

    bcmtm_drv_info_get(unit, &drv_info);

    port_schedule_state = &(drv_info->port_schedule_state);

    /* Core clock frequency */
    port_schedule_state->frequency = drv_info->frequency;

    /* Construct in_port_map */
    in_portmap = &port_schedule_state->in_port_map;


    /* initialize arrays in init_map struct */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        in_portmap->port_p2l_mapping[port] = -1;
        in_portmap->port_l2p_mapping[port] = -1;
        in_portmap->port_p2m_mapping[port] = -1;
        in_portmap->port_m2p_mapping[port] = -1;
        in_portmap->port_l2i_mapping[port] = -1;
        in_portmap->port_p2PBLK_inst_mapping[port] = -1;
        in_portmap->log_port_speed[port] = -1;
        in_portmap->port_num_lanes[port] = -1;
    }

    for (port = 0; port < _TH_PHY_PORTS_PER_DEV; port++) {
        log_port = drv_info->port_p2l_mapping[port];

        if (log_port == BCMPC_INVALID_LPORT) {
            continue;
        }

        in_portmap->port_p2l_mapping[port] =log_port;
        in_portmap->port_l2p_mapping[log_port] = port;

        in_portmap->port_p2m_mapping[port] = drv_info->port_p2m_mapping[port];
        in_portmap->port_m2p_mapping[drv_info->port_p2m_mapping[port]] = port;

        in_portmap->log_port_speed[log_port] = drv_info->port_speed_max[log_port];
        in_portmap->port_num_lanes[log_port] = drv_info->port_num_lanes[log_port];
        in_portmap->port_p2PBLK_inst_mapping[port] = (((port - 1) / 4) % _TH_PBLKS_PER_PIPE);

        in_portmap->port_l2i_mapping[log_port] = drv_info->port_l2i_mapping[log_port];

        BCMDRD_PBMP_PORT_ADD(in_portmap->physical_pbm, port);
        if (drv_info->port_flags[log_port] & BCMTM_PORT_IS_HG2) {
            BCMDRD_PBMP_PORT_ADD(in_portmap->hg2_pbm, log_port);
            num_hg += 1;
        }
        if (drv_info->port_flags[log_port] & BCMTM_PORT_IS_MGMT) {
            BCMDRD_PBMP_PORT_ADD(in_portmap->management_pbm, log_port);
        }
        if (drv_info->port_flags[log_port] & BCMTM_PORT_IS_OVSB) {
            BCMDRD_PBMP_PORT_ADD(in_portmap->oversub_pbm, log_port);
        }

    }


    /* Non-zero hg ports */
    drv_info->fabric_port_enable = !(!num_hg);
    rv = soc_th_port_schedule_tdm_init(unit, port_schedule_state);
    sal_memset(&th_tdm_drv, 0, sizeof(bcmtm_tdm_drv_t));

    bcm56960_a0_tdm_drv_attach(unit, &th_tdm_drv);

    rv = soc_tomahawk_tdm_init(unit, port_schedule_state, &th_tdm_drv);
    if (rv != SHR_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
            "Unable to configure TDM, please contact your "
            "Field Applications Engineer or Sales Manager for "
            "additional assistance.\n")));
        return rv;
    }

    /* Update SOC TDM info */
    rv = soc_th_soc_tdm_update(unit, port_schedule_state);

    /* Set is_flexport = 1 for future tdm reconfig purposes */
    port_schedule_state->is_flexport = 1;

    return rv;
}


