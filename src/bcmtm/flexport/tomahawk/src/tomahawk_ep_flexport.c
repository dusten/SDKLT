/*! \file tomahawk_ep_flexport.c
 *
 * Tomahawk egress pipe flex port related apis.
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


/*** START SDK API COMMON CODE ***/
#if defined(BCM_TOMAHAWK_SUPPORT)

/*! @file tomahawk_ep_flexport.c
 *  @brief EP (EDB) FlexPort for Tomahawk.
 *  Details are shown below.
 */

static const soc_th_ep_core_cfg_t
    soc_th_ep_core_cfg_tbl[] = {
    /* speed   ep2mmu  ep2_mmu_max    L2_lat      L3 lat       full_lat
     * LR  3:2 2:1   LR  3:2 2:1   LR  3:2 2:1
     */
    {    -1,   0,   0, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0}},   /* SAF     */
    { 10000,  13,  11, {51, 42, 30}, {60, 48, 27}, {60, 48, 27}},   /* 10GE    */
    { 11000,  13,  11, {51, 42, 30}, {60, 48, 27}, {60, 48, 27}},   /* HG[11]  */
    { 20000,  18,  16, {63, 48, 30}, {54, 42, 21}, {54, 42, 21}},   /* 20GE    */
    { 21000,  18,  16, {63, 48, 30}, {54, 42, 21}, {54, 42, 21}},   /* HG[21]  */
    { 25000,  16,  15, {60, 48, 27}, {51, 36, 18}, {51, 36, 18}},   /* 25GE    */
    { 27000,  16,  15, {60, 48, 27}, {51, 36, 18}, {51, 36, 18}},   /* HG[27]  */
    { 40000,  25,  19, {57, 45, 24}, {42, 30, 12}, {42, 30, 12}},   /* 40GE    */
    { 42000,  25,  19, {57, 45, 24}, {42, 30, 12}, {42, 30, 12}},   /* HG[42]  */
    { 50000,  27,  23, {54, 42, 21}, {33, 27, 12}, {33, 27, 12}},   /* 50GE    */
    { 53000,  27,  23, {54, 42, 21}, {33, 27, 12}, {33, 27, 12}},   /* HG[53]  */
    {100000,  44,  36, {48, 36, 18}, {33, 27, 15}, {33, 27, 15}},   /* 100GE   */
    {106000,  44,  36, {48, 36, 18}, {33, 27, 15}, {33, 27, 15}}    /* HG[106] */
};

/*! @fn int soc_tomahawk_ep_flexport_get_max_ovs_ratio(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int *old_max_ovs_ratio, int *new_max_ovs_ratio )
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param old_max_ovs_ratio returned pointer to old_max_ovs_ratio.
 *  @param new_max_ovs_ratio returned pointer to new_max_ovs_ratio.
 *  @brief Compute old and new ovs ratio across all Half Pipes
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_flexport_get_max_ovs_ratio(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int *old_max_ovs_ratio, int *new_max_ovs_ratio)
{
    int pipe, hpipe;
    int told_max_ovs_ratio, tnew_max_ovs_ratio;
    soc_tomahawk_flex_scratch_t *cookie;

    /* Find old/new max ovs ratio among the whole half pipes in the device */
    cookie = port_schedule_state->cookie;
    told_max_ovs_ratio = 0;
    tnew_max_ovs_ratio = 0;
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        for (hpipe = 0; hpipe < _TH_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            if (tnew_max_ovs_ratio <
                cookie->new_hpipe_ovs_ratio[pipe][hpipe]) {
                tnew_max_ovs_ratio = cookie->new_hpipe_ovs_ratio[pipe][hpipe];
            }
            if (told_max_ovs_ratio <
                cookie->prev_hpipe_ovs_ratio[pipe][hpipe]) {
                told_max_ovs_ratio = cookie->prev_hpipe_ovs_ratio[pipe][hpipe];
            }
        }
    }

    *old_max_ovs_ratio = told_max_ovs_ratio;
    *new_max_ovs_ratio = tnew_max_ovs_ratio;

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_ep_get_ep2mmu_credit(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Dev port num
 *  @param ovs_ratio ovs_ratio inside {10-LR, 15-OVS 3:2, 20- OVS 2:1}
 *  @brief Helper function to get EP2MMU credit for a port;
 * This API assumes that credits were retrieved for active ports after FlexPort
 */
STATIC int
soc_tomahawk_ep_get_ep2mmu_credit(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    int ct_class = 0;
    int speed;
    int old_max_ovs_ratio, new_max_ovs_ratio;
    soc_tomahawk_flex_scratch_t *cookie;

    soc_tomahawk_ep_flexport_get_max_ovs_ratio(unit,
                                                port_schedule_state,
                                                &old_max_ovs_ratio,
                                                &new_max_ovs_ratio);

    /* Get ct_class */
    cookie = port_schedule_state->cookie;
    speed = cookie->exact_out_log_port_speed[logical_port];
    ct_class = soc_tomahawk_get_ct_class(speed);

    /* At MAX FREQ, the ovs_ratio will always be less than 3:2;
     * Still add that check
     */
    if ((1700 == port_schedule_state->frequency) &&
        (new_max_ovs_ratio < 20) ) {
        return soc_th_ep_core_cfg_tbl[ct_class].egr_mmu_credit_max_freq;
    } else {
        return soc_th_ep_core_cfg_tbl[ct_class].egr_mmu_credit;
    }
}


/*! @fn int soc_tomahawk_ep_get_ct_xmit_start_cnt(int ct_class, int latency_mode,
 *              int ovs_ratio)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param latency_mode Latency mode 0-FULL; 1-L3; 2-L2.
 *  @param ovs_ratio ovs_ratio inside {10-LR, 15-OVS 3:2, 20- OVS 2:1}
 *  @brief Helper function to get XMIT_START_CNT value
 */
/*
 * On BCM56960, the switch latency bypass modes availabe are:
 * 0 - normal operation (FULL)
 * 1 - balanced latency L2 + L3 (L3)
 * 2 - low latency L2  (L2)
 */
STATIC int
soc_tomahawk_ep_get_ct_xmit_start_cnt(int ct_class, int latency_mode,
                                       int ovs_ratio)
{
    int xmit_cnt;

    switch (latency_mode) {
    /* FULL latency */
    case 0:
        if (ovs_ratio <= 10) {     /* LR */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_full_latency.
                line_rate;
        } else if (ovs_ratio <= 15) {     /* OVS 3:2 */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_full_latency.
                oversub_3_2;
        } else {     /* OVS 2:1 */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_full_latency.
                oversub_2_1;
        }
        break;
    case 1:
        if (ovs_ratio <= 10) {     /* LR */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_l3_latency.line_rate;
        } else if (ovs_ratio <= 15) {     /* OVS 3:2 */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_l3_latency.
                oversub_3_2;
        } else {     /* OVS 2:1 */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_l3_latency.
                oversub_2_1;
        }
        break;
    case 2:
        if (ovs_ratio <= 10) {     /* LR */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_l2_latency.line_rate;
        } else if (ovs_ratio <= 15) {     /* OVS 3:2 */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_l2_latency.
                oversub_3_2;
        } else {     /* OVS 2:1 */
            xmit_cnt =
                soc_th_ep_core_cfg_tbl[ct_class].xmit_cnt_l2_latency.
                oversub_2_1;
        }
        break;
    default: xmit_cnt = 0; break;
    }

    return xmit_cnt;
}


/*! @fn int soc_tomahawk_ep_get_xmit_start_count(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int logical_port, int *xmit_start_cnt)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *             variable
 *  @param logical_port Dev port num
 *  @param xmit_start_cnt array of 16 entries 0-SAF; 1-12 CT; 13-15 Unused
 *  @brief Helper function to get XMIT_START_CNT array for a port;
 * This API assumes that credits were retrieved for active ports after FlexPort
 */
STATIC int
soc_tomahawk_ep_get_xmit_start_count(
    int  unit, soc_port_schedule_state_t *port_schedule_state,
    int  logical_port, int *xmit_start_cnt)
{
    int dst_ct_class, src_ct_class;
    int speed;
    int latency_mode;
    int saf_xmit_start_cnt, dst_ct_xmit_start_cnt;
    int old_max_ovs_ratio, new_max_ovs_ratio;
    soc_tomahawk_flex_scratch_t *cookie;

    cookie = port_schedule_state->cookie;
    speed = cookie->exact_out_log_port_speed[logical_port];
    dst_ct_class = soc_tomahawk_get_ct_class(speed);

    soc_tomahawk_ep_flexport_get_max_ovs_ratio(unit,
                                                port_schedule_state,
                                                &old_max_ovs_ratio,
                                                &new_max_ovs_ratio);

    latency_mode = port_schedule_state->cutthru_prop.switch_bypass_mode;

    /* Populate xmit_start_cnt array */
    saf_xmit_start_cnt = soc_tomahawk_ep_get_ct_xmit_start_cnt(
                                      0, latency_mode, new_max_ovs_ratio);                                                            /* 0 means SAF */
    dst_ct_xmit_start_cnt = soc_tomahawk_ep_get_ct_xmit_start_cnt(
        dst_ct_class, latency_mode, new_max_ovs_ratio);
    for (src_ct_class=0; src_ct_class < 13; src_ct_class++) {
        if (src_ct_class < dst_ct_class) {
            xmit_start_cnt[src_ct_class] = saf_xmit_start_cnt;
        } else {
            xmit_start_cnt[src_ct_class] = dst_ct_xmit_start_cnt;
        }
    }
    xmit_start_cnt[13] = 0;
    xmit_start_cnt[14] = 0;
    xmit_start_cnt[15] = 0;

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_ep_flexport_sft_rst_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *         struct variable
 *  @param rst_on rst_on==1-> ASSERT reset; rst_on==0-> DEASSERT reset
 *  @brief Main API that sft resets all flexing ports
 * Return Value:
 *      SOC_E_*
 */
#if 0
int
soc_tomahawk_ep_flexport_sft_rst_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state, int rst_on)
{
    int i;
    int logical_port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    soc_mem_t mem;

    mem = EGR_PER_PORT_BUFFER_SFT_RESETm;

    if (rst_on == 1) {
        memfld = 1;
    } else {
        memfld = 0;
    }

    /* For ports going DOWN  or UP do:
     * Assert(rst_on=1)/De-assert(rst_on=0) per port sft reset
     */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;

        if ( logical_port > 0) {
            sal_memset(entry, 0, sizeof(entry));
            soc_mem_field_set(unit, mem, entry, ENABLEf, &memfld);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                              logical_port, entry));
        }
    }

    return SOC_E_NONE;
}
#endif


/*! @fn int soc_tomahawk_ep_enable_disable(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - port disabled; else port enabled.
 *  @brief Enables or disables EDB port; configures EGR_ENABLE table
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_enable_disable(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int                        logical_port,
                                int                        down_or_up)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    soc_mem_t mem;

    if (0 == down_or_up ) { /* port down */
        memfld = 0;
    } else { /* port up */
        memfld = 1;
    }
    mem = EGR_ENABLEm;
    sal_memset(entry, 0, sizeof(uint32) * soc_mem_entry_words(unit, EGR_ENABLEm));
    soc_mem_field_set(unit, mem, entry, PRT_ENABLEf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, logical_port,
                entry));
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_ep_set_egr_mmu_credit(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - clear credits; else configure credits
 *  @brief Sets or resets EDB 2 MMU credit; configures EGR_MMU_CELL_CREDITm table
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_set_egr_mmu_credit(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int logical_port, int down_or_up)
{
    uint32 entry;
    uint32 reg_fld;
    soc_reg_t reg;

    if (0 == down_or_up ) { /* port down */
        reg_fld = 0;
    } else { /* port up */
        reg_fld = soc_tomahawk_ep_get_ep2mmu_credit(unit, port_schedule_state,
                                                    logical_port);
    }

    reg = EGR_MMU_CELL_CREDITr;
    entry = 0;
    soc_reg_field_set(unit, reg, &entry, CREDITf, reg_fld);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, logical_port, 0, entry));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_ep_set_dev_to_phy(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - reset mapping; else configure mapping
 *  @brief Sets or resets EDB 2 MMU credit; configures
 *         EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_set_dev_to_phy(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int                        logical_port,
                                int                        down_or_up)
{
    uint32 rval;
    int physical_port;
    soc_reg_t reg;

    if (0 == down_or_up ) { /* port down */
        physical_port = 0xFF;
    } else { /* port up */
        physical_port =
            port_schedule_state->out_port_map.port_l2p_mapping[logical_port];
    }

    rval = 0;
    reg = EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr;
    soc_reg_field_set(unit, reg, &rval, PHYSICAL_PORT_NUMBERf, physical_port);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, logical_port, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_ep_set_edb_1dbg_b(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - clear credits; else configure credits
 *  @brief Configures EDB_1DBG_Bm
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_set_edb_1dbg_b(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int                        logical_port,
                                int                        down_or_up)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int physical_port;
    soc_mem_t mem;
    int is_ovs;
    soc_port_map_type_t *port_map;

    if (0 == down_or_up ) { /* port down */
        port_map = &port_schedule_state->in_port_map;
        memfld = 0;
    } else { /* port up */
        port_map = &port_schedule_state->out_port_map;
        memfld =
            (15 * 11875 *
             (port_schedule_state->out_port_map.log_port_speed[logical_port] /
         1000)) / 1000;
    }

    /* Configure only if OVS */
    is_ovs = SOC_PBMP_NOT_NULL(port_map->oversub_pbm) ? 1 : 0;
    if (1 == is_ovs) {
        mem = EDB_1DBG_Bm;
        sal_memset(entry, 0, sizeof(uint32) *
                   soc_mem_entry_words(unit, EDB_1DBG_Bm));
        physical_port = port_map->port_l2p_mapping[logical_port];
        soc_mem_field_set(unit, mem, entry, FIELD_Bf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                          physical_port, entry));
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_ep_flexport_xmit_cnt_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @brief Configures EGR_XMIT_START_COUNT_PIPE table; all valid per port 13
 *         entries
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_flexport_xmit_cnt_set(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    soc_mem_t mem;
    int pipe, class, mem_index;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int xmit_start_cnt[16];
    static const soc_mem_t ep_xmit_cnt_mems[_TH_PIPES_PER_DEV] = {
        EGR_XMIT_START_COUNT_PIPE0m, EGR_XMIT_START_COUNT_PIPE1m,
        EGR_XMIT_START_COUNT_PIPE2m, EGR_XMIT_START_COUNT_PIPE3m
    };

    pipe = logical_port / _TH_DEV_PORTS_PER_PIPE;

    /* Set all XMIT START CNT values for a port; 0-SAF; 1-12 CT classes;
     * 13-15 reserved
     */
    mem = ep_xmit_cnt_mems[pipe];
    soc_tomahawk_ep_get_xmit_start_count(unit, port_schedule_state,
                                          logical_port,
                                          xmit_start_cnt);
    for (class = 0; class < 13; class++) {
        sal_memset(entry, 0, sizeof(uint32) *
                   soc_mem_entry_words(unit, EGR_XMIT_START_COUNT_PIPE0m));
        mem_index = (logical_port % _TH_DEV_PORTS_PER_PIPE) * 16 + class;
        memfld = 18;
        soc_mem_field_set(unit, mem, entry, THRESHOLDf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_index,
                                          entry));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_ep_flexport_xmit_cnt_set_all_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief API to configure EGR_XMIT_START_COUNT_PIPE0/1/2/3m for either added
 *         ports or all ports in a pipe (if ovs_ratio changes)
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_flexport_xmit_cnt_set_all_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int old_max_ovs_ratio, new_max_ovs_ratio;
    int i, logical_port, physical_port, is_ovs;


    /* Find old/new max ovs ratio among the whole half pipes in the device */
    soc_tomahawk_ep_flexport_get_max_ovs_ratio(unit,
                                                port_schedule_state,
                                                &old_max_ovs_ratio,
                                                &new_max_ovs_ratio);

    /* ovs_ratio_changed; all ports from device configured */
    if (new_max_ovs_ratio != old_max_ovs_ratio) {
        for (logical_port = 0; logical_port < _TH_DEV_PORTS_PER_DEV;
             logical_port++) {
            is_ovs = SOC_PBMP_MEMBER(
                port_schedule_state->out_port_map.oversub_pbm,
                logical_port) ? 1 : 0;
            if ((port_schedule_state->out_port_map.log_port_speed[logical_port]
                 > 0 ) &&
                (is_ovs == 1)) {
                soc_tomahawk_ep_flexport_xmit_cnt_set(unit,
                                                       port_schedule_state,
                                                       logical_port);
            }
        }
    } else { /* ovs_ratio is the same; only added ports are configured */
        for (i = 0; i < port_schedule_state->nport; i++) {
            logical_port = port_schedule_state->resource[i].logical_port;
            physical_port = port_schedule_state->resource[i].physical_port;
            if (BCMPC_INVALID_PPORT != physical_port) { /* that is, port UP */
                /* Configure XMIT_START_CNT entries for added port :
                 * write EGR_XMIT_START_COUNT_PIPE0/1/2/3m
                 */
                soc_tomahawk_ep_flexport_xmit_cnt_set(unit,
                                                       port_schedule_state,
                                                       logical_port);
            }
        }
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_ep_flexport_ct_class_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @brief Configures EGR_IP_CUT_THRU_CLASS table in all 4 pipes
 * Return Value:
 *      SOC_E_*
 */
STATIC int
soc_tomahawk_ep_flexport_ct_class_set(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    soc_mem_t mem;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int speed;
    soc_tomahawk_flex_scratch_t *cookie;

    /* Set CT_CLASS for this port in all PIPEs */
    mem = EGR_IP_CUT_THRU_CLASSm;
    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, EGR_IP_CUT_THRU_CLASSm));
    cookie = port_schedule_state->cookie;
    speed  = cookie->exact_out_log_port_speed[logical_port];
    memfld = soc_tomahawk_get_ct_class(speed);
    soc_mem_field_set(unit, mem, entry, CUT_THRU_CLASSf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, logical_port,
                                      entry));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_ep_flexport_drain_port(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param cell_counts number of additional cell in prebuffering
 *  @brief Drain a port that goes down
 * Return Value:
 *      SOC_E_*
 */

STATIC int
soc_tomahawk_ep_flexport_drain_port(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    int pipe;
    uint32 rval_save, rval;
    soc_reg_t reg;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 util;
    int physical_port;
    soc_mem_t mem;
    int timeout, iter;

    static const soc_reg_t misc_ctrl_regs[_TH_PIPES_PER_DEV] = {
        EGR_EDB_MISC_CTRL_PIPE0r, EGR_EDB_MISC_CTRL_PIPE1r,
        EGR_EDB_MISC_CTRL_PIPE2r, EGR_EDB_MISC_CTRL_PIPE3r
    };

    pipe = logical_port / _TH_DEV_PORTS_PER_PIPE;

    /* Configure EGR_EDB_MISC_CTRL such that EGR_MAX_USED_ENTRIES returns
     * buffer utilization
     */
    reg = misc_ctrl_regs[pipe];
    rval_save = 0;
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0,
                                              &rval_save));
    rval = rval_save;
    soc_reg_field_set(unit, reg, &rval, SELECT_CURRENT_USED_ENTRIESf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    /* polling port's buffer utilization until is 0 */
    timeout = 10000;
    iter = 0;
    physical_port =
        /* assumes drain before FlexPort */
        port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, EGR_MAX_USED_ENTRIESm));
    mem = EGR_MAX_USED_ENTRIESm;
    while (iter++ < timeout) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                          physical_port, entry));
        /* coverity[callee_ptr_arith : FALSE] */
        soc_mem_field_get(unit, mem, entry, LEVELf, &util);
        if (0 == util) { break; }
    }

    /* Configure EGR_EDB_MISC_CTRL back to previous value */
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0,
                                              rval_save));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_ep_port_down(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to follow EDB port down sequence for flexport operation.
 *         Follows the sequence specified in document "TH_uA_flexport.pdf",
 *         Section 6.6.1.1
 *         Two main parts:
 *            (1) Poll until EDB buffer is empty
 *            (2) Hold EDB port buffer in reset state and disable cell request
 *                generation in EP
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_flex_ep_port_down(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    int i, down_or_up;
    int logical_port, physical_port;

    /*
     * 6. Poll until EDB buffer is empty
     * Set EGR_EDB_MISC_CTRL.SELECT_CURRENT_USED_ENTRIES to 1.
     * Poll until EGR_MAX_USED_ENTRIES[physical_port] is equal to 0.
     */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == BCMPC_INVALID_PPORT) {
            logical_port = port_schedule_state->resource[i].logical_port;
            SOC_IF_ERROR_RETURN(soc_tomahawk_ep_flexport_drain_port(unit,
                                    port_schedule_state, logical_port));
        }
    }

    /*
     * 7. Hold EDB port buffer in reset state and disable cell request generation
     *    in EP2
     * Set EGR_ENABLE[device_port].PRT_ENABLE to 0.
     * Set EGR_PER_PORT_BUFFER_SFT_RESET[device_port] to 1.
     */

    down_or_up = 0; /* that is, port DOWN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        physical_port = port_schedule_state->resource[i].physical_port;
        logical_port = port_schedule_state->resource[i].logical_port;
        if (physical_port == BCMPC_INVALID_PPORT) { /* that is, port DOWN */
            /* Disable port : write EGR_ENABLEm */
            soc_tomahawk_ep_enable_disable(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_ep_reconfigure_remove(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that reconfigures all ports from EGR pipe - remove ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_flex_ep_reconfigure_remove(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i, down_or_up;
    int logical_port, physical_port;

    /* For ports going DOWN do:
     * 1. Delete Dev 2 Phy mapping
     * 2. Reset max Ep2MMU credits to 0
     * 3. Reset EDB_1DBG_Bm to 0
     */
    down_or_up = 0; /* that is, port DOWN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (BCMPC_INVALID_PPORT == physical_port) { /* that is, port DOWN */
            /* Reset Logical to phy mapping : write
             * EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr
             */
            soc_tomahawk_ep_set_dev_to_phy(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);

            /* Reset EP2MMU credits to 0 : write EGR_MMU_CELL_CREDITm */
            soc_tomahawk_ep_set_egr_mmu_credit(unit, port_schedule_state,
                                                logical_port,
                                                down_or_up);

            /* Reset edb_1dbg_b : write EDB_1DBG_Bm */
            soc_tomahawk_ep_set_edb_1dbg_b(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_ep_reconfigure_add(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that reconfigures all ports from EGR pipe - add ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_flex_ep_reconfigure_add(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i, down_or_up;
    int logical_port, physical_port;


    /* For ports going UP do:
     * 1. Map Dev 2 Phy for added port
     * 2. Configure max Ep2MMU credits to proper value
     * 3. Configure EDB_1DBG_Bm for added port
     * 4. Configure CT_class for added port
     */
    down_or_up = 1; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (BCMPC_INVALID_PPORT != physical_port) { /* that is, port UP */
            /* Map Dev 2 Phy : write EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr */
            soc_tomahawk_ep_set_dev_to_phy(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);

            /* Configure max Ep2MMU credits to proper value :
             * write EGR_MMU_CELL_CREDITm
             */
            soc_tomahawk_ep_set_egr_mmu_credit(unit, port_schedule_state,
                                                logical_port,
                                                down_or_up);

            /* Configure EDB_1DBG_Bm : write EDB_1DBG_Bm */
            soc_tomahawk_ep_set_edb_1dbg_b(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
            /* Configure CT_class for added port : write EGR_IP_CUT_THRU_CLASSm */
            soc_tomahawk_ep_flexport_ct_class_set(unit, port_schedule_state,
                                                   logical_port);
        }
    }

    /* 4. Configure XMIT_START_CNT entries for added port OR all ports
     * write EGR_XMIT_START_COUNT_PIPE0/1/2/3m
     */
    SOC_IF_ERROR_RETURN(soc_tomahawk_ep_flexport_xmit_cnt_set_all_ports(unit,
                            port_schedule_state));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk_flex_ep_port_up(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that adds all ports from EGR pipe
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_flex_ep_port_up(int                        unit,
                              soc_port_schedule_state_t *port_schedule_state)
{
    int i, down_or_up;
    int logical_port;


    /*
     * Release EDB port buffer reset and enable cell request generation in EP
     * Set EGR_PER_PORT_BUFFER_SFT_RESET[device_port] to 0
     * Set EGR_ENABLE[device_port].PRT_ENABLE to 1
     */
    /* De-assert PM intf sft_reset */
    /* Enable Ports going up after PM sft_rst is de-asserted */
    /* For ports going UP do:
     * 1. Enable port; write EGR_ENABLEm
     */
    down_or_up = 1; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        if (logical_port > 0) { /* that is, port UP */
            /* Enable port; write EGR_ENABLEm */
            soc_tomahawk_ep_enable_disable(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }

    return SOC_E_NONE;
}


/*** END SDK API COMMON CODE ***/


/*! @fn int soc_tomahawk_tdm_calculate_ovs(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int pipe, int prev_or_new)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param prev_or_new calculate OVS ratios for prev or new OVS tables;
 *         0 - prev; 1- new
 *  @brief Calculate ovs_ratios per PIPE and HPIPE
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk_ep_display_xmit_cnt(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int ct_class, lat, ovs_ratio, port, ep2mmu_cred;
    int xmit_cnt;
    int xmit_cnt_tbl[16];

    /* FULL LATENCY */

    for (ovs_ratio=2; ovs_ratio<=4; ovs_ratio++) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n\nOVS=%d\n"), 5*ovs_ratio));
        for (ct_class=0; ct_class < 13; ct_class++) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                "ct_class=%d\tep2mmu=%d\tep2mmu_max_freq=%d\t"),
                     ct_class, soc_th_ep_core_cfg_tbl[ct_class].egr_mmu_credit,
                     soc_th_ep_core_cfg_tbl[ct_class].egr_mmu_credit_max_freq));
            for (lat=2; lat>=0; lat--) {
                xmit_cnt = soc_tomahawk_ep_get_ct_xmit_start_cnt(ct_class, lat,
                                                                  5*ovs_ratio);
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "%2d\t"), xmit_cnt));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
        }
    }


    for (port = 1; port < 136; port++) {
        if (port_schedule_state->out_port_map.log_port_speed[port] > 0 ) {
            ep2mmu_cred = soc_tomahawk_ep_get_ep2mmu_credit(
                unit, port_schedule_state, port);
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "LOG_PORT=%3d\tSPEED=%3d\tCRED=%2d\t"),
                     port,
                     port_schedule_state->in_port_map.log_port_speed[port]/1000,
                     ep2mmu_cred));
            soc_tomahawk_ep_get_xmit_start_count(unit, port_schedule_state,
                                                  port,
                                                  xmit_cnt_tbl);
            for (ct_class=0; ct_class < 16; ct_class++) {
                LOG_DEBUG(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit, "%2d\t"), xmit_cnt_tbl[ct_class]));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk_ep_flexport_remove_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk_flex_start")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_start(unit, port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk_tdm_calculation_flexport")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_calculation_flexport(unit,
                                                               port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk_flex_ep_port_down")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_ep_port_down(unit,
                                                        port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk_ep_flexport_reconfigure_remove")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_ep_reconfigure_remove(unit,
                            port_schedule_state));


    return SOC_E_NONE;
}

/* Comment out these functions from soc_tomahawk_ep_flexport_add_ports function
 * 1. soc_tomahawk_ep_flexport_ct_class_set
 * 2. soc_tomahawk_ep_flexport_xmit_cnt_set_all_ports
 * 3. soc_tomahawk_ep_enable_disable
 */
int
soc_tomahawk_ep_flexport_add_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk_ep_flexport_add_ports_shim(): "
                        "calling soc_tomahawk_ep_flexport_reconfigure_add")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_ep_reconfigure_add(unit,
                                                              port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk_ep_flexport_add_ports_shim(): "
                        "calling soc_tomahawk_ep_flexport_add_ports")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_ep_port_up(unit, port_schedule_state));

    return SOC_E_NONE;
}


int
soc_tomahawk_ep_flexport_enable_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int down_or_up, i, logical_port, physical_port;


    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk_ep_flexport_add_ports_shim(): "
                        "calling soc_tomahawk_ep_flexport_add_ports")));
    /* Enable Ports going up after PM sft_rst is de-asserted */
    /* For ports going UP do:
     * 6. Enable port; write EGR_ENABLEm
     */
    down_or_up = 1; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 != physical_port) { /* that is, port UP */
            /* Enable port; write EGR_ENABLEm */
            soc_tomahawk_ep_enable_disable(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }
    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK_SUPPORT */
