/*! \file tomahawk_flexport_top.c
 *
 * Tomahawk flex port top opeartions apis.
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



#if defined(BCM_TOMAHAWK_SUPPORT)
#ifndef SOC_TOMAHAWK_FLEXPORT_C
#define SOC_TOMAHAWK_FLEXPORT_C


/*! @file tomahawk_flexport_top.c
 *   @brief FlexPort functions and structures.
 *   Details are shown below.
 */


/*! @fn int (*flexport_phases[MAX_FLEX_PHASES]) (int unit, soc_port_schedule_state_t
 *                                                *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief Phase control array containing the right sequence of FlexPort phases
 *          implemented as functions.
 */
int (*flexport_phases[MAX_FLEX_PHASES]) (int unit, soc_port_schedule_state_t
                                         *port_schedule_state) = {
    /* Must call for starting the phases. */
    &soc_tomahawk_flex_start,               /* 0 */

    /* Port down sequence. Sect 6.6.1. */
    &soc_tomahawk_flex_top_port_down,       /* 1 */

    /* Reconfigure sequence. */
    &soc_tomahawk_tdm_calculation_flexport, /* 2 */
    &soc_tomahawk_flex_ep_reconfigure_remove, /* 3 */
    &soc_tomahawk_flex_mmu_reconfigure_phase1, /* 4 */
    &soc_tomahawk_flex_mmu_reconfigure_phase2, /* 5 */
    &soc_tomahawk_flex_idb_reconfigure,     /* 6 */
    &soc_tomahawk_flex_ep_reconfigure_add,  /* 7 */

    /* Port Up sequence. Sect 6.6.2. */
    &soc_tomahawk_flex_top_port_up,         /* 8 */

    /* Must call for ending the phases. */
    &soc_tomahawk_flex_end,                 /* 9 */

    NULL,                                    /* 10 */
    NULL,                                    /* 11 */
    NULL,                                    /* 12 */
    NULL,                                    /* 13 */
    NULL,                                    /* 14 */
    NULL,                                    /* 15 */
    NULL,                                    /* 16 */
    NULL,                                    /* 17 */
    NULL,                                    /* 18 */
    NULL,                                    /* 19 */
    NULL,                                    /* 20 */
    NULL,                                    /* 21 */
    NULL,                                    /* 22 */
    NULL,                                    /* 23 */
    NULL                                     /* 24 */
};


/*! @fn void *soc_tomahawk_port_lane_info_alloc(void)
 *   @brief Allocates memory pointed to by soc_port_lane_info_t. Need this
 *          special function because SystemVerilog-C DPI limitations in
 *          synchronizing memory space that is a pointer to a struct type.
 *          Since s/w owns the the pointer to soc_port_lane_info_t struct
 *          in soc_port_resource_t, we cannot change it.
 */
void *
soc_tomahawk_port_lane_info_alloc (
    void
    )
{
    soc_port_lane_info_t *lane_info_mem;

    lane_info_mem =
        (soc_port_lane_info_t *) sal_alloc(sizeof(soc_port_lane_info_t),
                                           "Flexport lane information");
    if (lane_info_mem == NULL) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(9999, "Memory allocation error!\n")));
    }

    return lane_info_mem;
}


/*! @fn int soc_tomahawk_set_port_lane_info(int unit, const void *lane_info_ptr,
 *               int port_idx, int pgw, int xlp)
 *   @param unit Chip unit number.
 *   @param lane_info_ptr Pointer to a soc_port_lane_info_t struct variable.
 *   @param port_idx Index within XLP.
 *   @param pgw PGW instance where lane resides.
 *   @param xlp XLP number within PGW.
 *   @brief Sets all members in a soc_port_lane_info_t struct variable.
 */
int
soc_tomahawk_set_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int         port_idx,
    int         pgw,
    int         xlp
    )
{
    soc_port_lane_info_t *lane_ptr = (soc_port_lane_info_t *) lane_info_ptr;

    lane_ptr->port_index = port_idx;
    lane_ptr->pgw = pgw;
    lane_ptr->xlp = xlp;

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_get_port_lane_info(int unit, const void *lane_info_ptr,
 *               int *port_idx, int *pgw, int *xlp)
 *   @param unit Chip unit number.
 *   @param lane_info_ptr Pointer to a soc_port_lane_info_t struct variable.
 *   @param port_idx Index within XLP.
 *   @param pgw PGW instance where lane resides.
 *   @param xlp XLP number within PGW.
 *   @brief Gets all members in a soc_port_lane_info_t struct variable.
 */
int
soc_tomahawk_get_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int *       port_idx,
    int *       pgw,
    int *       xlp
    )
{
    soc_port_lane_info_t *lane_ptr = (soc_port_lane_info_t *) lane_info_ptr;

    *port_idx = lane_ptr->port_index;
    *pgw = lane_ptr->pgw;
    *xlp = lane_ptr->xlp;

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_reconfigure_ports(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief Top-level call carrying out all FlexPort functionalities.
 */
int
soc_tomahawk_reconfigure_ports (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
    int i, j, rv;

    if (LOG_CHECK(BSL_LS_SOC_PORT)) {
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "FlexPort top level function entered.\n")));

        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "nport = %d \n"), port_schedule_state->nport));
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "resource[0].num_lanes = %d \n"),
                 port_schedule_state->resource[0].num_lanes));
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Frequency = %d MHz\n"),
                 port_schedule_state->frequency));
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "is_flexport = %d \n"),
                 port_schedule_state->is_flexport));
        for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "port_p2l_mapping[%d] = %d \n"),
                      i, port_schedule_state->in_port_map.port_p2l_mapping[i]));
        }
        for (i = 0; i < SOC_MAX_NUM_MMU_PORTS; i++) {
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "port_m2p_mapping[%d] = %d \n"),
                      i, port_schedule_state->in_port_map.port_m2p_mapping[i]));
        }
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "Number of OVS groups = %d \n"),
                  port_schedule_state->tdm_egress_schedule_pipe[0].
                  tdm_schedule_slice[0].num_ovs_groups));
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "OVS group length = %d \n"),
                  port_schedule_state->tdm_egress_schedule_pipe[0].
                  tdm_schedule_slice[0].ovs_group_len));
        for (i = 0;
             i <
             port_schedule_state->tdm_egress_schedule_pipe[0].
             tdm_schedule_slice[0].
             num_ovs_groups; i++) {
             LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                       "Pipe 0 egress oversyb group %d entries = \n"), i));
            for (j = 0;
                 j < port_schedule_state->tdm_egress_schedule_pipe[0].
                 tdm_schedule_slice[0].ovs_group_len; j++) {
              LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "%d \n"),
                        port_schedule_state->tdm_egress_schedule_pipe[0].
                        tdm_schedule_slice[0].oversub_schedule[i][j]));
            }
        }
    }

    /* FlexPort phase calls. */
    for (i = 0; i < MAX_FLEX_PHASES; i++) {
        if (flexport_phases[i] != NULL) {
            rv = flexport_phases[i](unit, port_schedule_state);
            if (rv != SOC_E_NONE) {
                sal_free(port_schedule_state->cookie);
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "Error encountered. Cookie space deallocated.\n")));
                return rv;
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_start(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief The start of FlexPort series of operations along with allocations of
 *          required program memory.
 */
int
soc_tomahawk_flex_start (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
    soc_tomahawk_flex_scratch_t *cookie;

    cookie =
        (soc_tomahawk_flex_scratch_t *) sal_alloc(sizeof(
                                                       soc_tomahawk_flex_scratch_t),
                                                   "Flexport cookie");
    if (cookie == NULL) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit, "Memory allocation error!\n")));
        return SOC_E_MEMORY;
    }
    port_schedule_state->cookie = cookie;
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Cookie space allocated.\n")));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_end(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief The end of FlexPort series of operations along with deallocations of
 *          program memory.
 */
int
soc_tomahawk_flex_end (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
    int i, j, sz;

    sz = sizeof(soc_port_schedule_state_t);
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "Size of cookie = %d Bytes.\n"), sz));
    sal_free(port_schedule_state->cookie);
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "Cookie space deallocated.\n")));

    /* Iterate through all port resource items and reclaim the allocated
     * lane information spaces.                                          */
    for (i = 0; i < port_schedule_state->nport; i++) {
        for (j = 0; j < port_schedule_state->resource[i].num_lanes; j++) {
            if (port_schedule_state->resource[i].lane_info[j] != NULL) {
                sal_free(port_schedule_state->resource[i].lane_info[j]);
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "Lane information space deallocated.\n")));
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_top_port_down(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief For legacy reasons, software wants to separate this part under a
 *          different function call from the rest of the FlexPort code.
 */
int
soc_tomahawk_flex_top_port_down (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
#if 0
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_dis_forwarding_traffic(unit,
                                                                port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_mac_rx_port_down(unit,
                                                            port_schedule_state));
#endif
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_idb_port_down(unit,
                                                         port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_mmu_port_down(unit,
                                                         port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_ep_port_down(unit,
                                                        port_schedule_state));

#if 0
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_mac_tx_port_down(unit,
                                                            port_schedule_state));
#endif
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_top_port_up(int unit, soc_port_schedule_state_t
 *               *port_schedule_state)
 *   @param unit Chip unit number.
 *   @param port_schedule_state Pointer to data holding all required information
 *          for FlexPort opeations.
 *   @brief For legacy reasons, software wants to separate this part under a
 *          different function call from the rest of the FlexPort code.
 */
int
soc_tomahawk_flex_top_port_up (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    )
{
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_mmu_port_up(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_ep_port_up(unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_idb_port_up(unit,
                                                       port_schedule_state));

#if 0
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_mac_port_up(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_en_forwarding_traffic(unit,
                                                                port_schedule_state));
#endif

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_get_ct_class(int speed)
 *  @param speed Speed
 *  @brief Helper function to get CT class
 */
int
soc_tomahawk_get_ct_class(int speed)
{
    int ct_class = 0;
    switch (speed) {
    case 10000: ct_class = 1; break;
    case 11000: ct_class = 2; break;
    case 20000: ct_class = 3; break;
    case 21000: ct_class = 4; break;
    case 25000: ct_class = 5; break;
    case 27000: ct_class = 6; break;
    case 40000: ct_class = 7; break;
    case 42000: ct_class = 8; break;
    case 50000: ct_class = 9; break;
    case 53000: ct_class = 10; break;
    case 100000: ct_class = 11; break;
    case 106000: ct_class = 12; break;
    default: ct_class = 0; break;
    }
    return ct_class;
}

#endif /* SOC_TOMAHAWK_FLEXPORT_C */


#endif /* BCM_TOMAHAWK_SUPPORT */
