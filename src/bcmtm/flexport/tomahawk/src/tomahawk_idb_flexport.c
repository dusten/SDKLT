/*! \file tomahawk_idb_flexport.c
 *
 * Tomahawk IDB flexport operations.
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
#include <bcmtm/bcmtm_internal_socmem_glue.h>
#include <bcmdrd/bcmdrd_feature.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
/*** START SDK API COMMON CODE ***/


/*! @file tomahawk_idb_flexport.c
 *  @brief IDB flex for Tomahawk.
 *  Details are shown below.
 */


/* Bubble MOP */
typedef struct soc_tomahawk_bmop_s {
    uint8 line_rate[SOC_SWITCH_BYPASS_MODE_LOW + 1];   /* Line-rate profile ::
                                                        * 0->FULL, 1->L3, 2->L2 */
    uint8 oversub[SOC_SWITCH_BYPASS_MODE_LOW + 1];     /* Oversub profile ::
                                                        * 0->FULL, 1->L3, 2->L2 */
} soc_tomahawk_bmop_t;

static const struct soc_tomahawk_bmop_cfg_s {
    int speed;
    soc_tomahawk_bmop_t bmop_enable;                  /* Bubble MOP enable */
} soc_tomahawk_bmop_cfg_tbl[] = {
    {    -1, {{0, 0, 0}, {0, 0, 0}}},       /* SAF     */
    { 10000, {{0, 0, 0}, {0, 0, 0}}},       /* 10GE    */
    { 11000, {{0, 0, 0}, {0, 0, 0}}},       /* HG[11]  */
    { 20000, {{1, 1, 0}, {0, 0, 0}}},       /* 20GE    */
    { 21000, {{1, 1, 0}, {0, 0, 0}}},       /* HG[21]  */
    { 25000, {{1, 1, 0}, {1, 0, 0}}},       /* 25GE    */
    { 27000, {{1, 1, 0}, {1, 0, 0}}},       /* HG[27]  */
    { 40000, {{1, 1, 1}, {1, 1, 0}}},       /* 40GE    */
    { 42000, {{1, 1, 1}, {1, 1, 0}}},       /* HG[42]  */
    { 50000, {{1, 1, 1}, {1, 1, 1}}},       /* 50GE    */
    { 53000, {{1, 1, 1}, {1, 1, 1}}},       /* HG[53]  */
    {100000, {{1, 1, 1}, {1, 1, 1}}},       /* 100GE   */
    {106000, {{1, 1, 1}, {1, 1, 1}}}        /* HG[106] */
};

#define _TH_OBM_PARAMS_LOSSLESS_INDEX    0
#define _TH_OBM_PARAMS_LOSSY_INDEX       1

static const struct soc_tomahawk_obm_setting_s {
    int discard_limit;
    int shared_discard_threshold;
    int shared_sop_threshold;
    int lossless_discard;
    int port_xoff;
    int port_xon;
    int lossless_xoff;
    int lossless_xon;
    int lossy_low_pri;
    int lossy_discard;
    int lossy_min;
    int sop_discard_mode;
} soc_tomahawk_obm_settings[2][5] = {
    /* LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 - invalid */
            0,               /* discard_limit */
            0,               /* shared_discard_threshold */
            0,               /* shared_sop_threshold */
            0,               /* lossless_discard */
            129,             /* port_xoff : Default value */
            119,             /* port_xon : Default value*/
            384,             /* lossless_xoff : Default value*/
            288,             /* lossless_xon : Default value*/
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 1 lane */
            253,             /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            1023,            /* lossless_discard */
            559,             /* port_xoff */
            549,             /* port_xon */
            45,             /* lossless_xoff */
            99,              /* lossless_xon */
            100,             /* lossy_low_pri */
            196,             /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            506,            /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            1023,            /* lossless_discard */
            312,            /* port_xoff */
            302,            /* port_xon */
            108,             /* lossless_xoff */
            291,             /* lossless_xon */
            100,             /* lossy_low_pri */
            196,             /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */
            0,               /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            0,               /* lossless_discard */
            129,             /* port_xoff */
            119,             /* port_xon */
            384,             /* lossless_xoff */
            288,             /* lossless_xon */
            100,               /* lossy_low_pri */
            0,               /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 4 lanes */
            1012,            /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            1023,            /* lossless_discard */
            682,            /* port_xoff */
            672,            /* port_xon */
            258,             /* lossless_xoff */
            738,             /* lossless_xon */
            100,             /* lossy_low_pri */
            196,             /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSY Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 - invalid */
            0,               /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            0,               /* lossless_discard */
            129,             /* port_xoff : Default value */
            119,             /* port_xon : Default value*/
            384,             /* lossless_xoff : Default value*/
            288,             /* lossless_xon : Default value*/
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 1 lane */
            1152,            /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            1023,            /* lossless_discard */
            129,            /* port_xoff */
            119,            /* port_xon */
            1023,            /* lossless_xoff */
            1023,            /* lossless_xon */
            100,             /* lossy_low_pri */
            196,             /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            506,            /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            1023,            /* lossless_discard */
            312,            /* port_xoff */
            302,            /* port_xon */
            1023,            /* lossless_xoff */
            1023,            /* lossless_xon */
            100,             /* lossy_low_pri */
            196,             /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid  */
            0,               /* discard_limit */
            1023,             /* shared_discard_threshold */
            1023,             /* shared_sop_threshold */
            0,               /* lossless_discard */
            129,             /* port_xoff */
            119,             /* port_xon */
            384,             /* lossless_xoff */
            288,             /* lossless_xon */
            0,               /* lossy_low_pri */
            0,               /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        },
        { /* 4 lanes */
            1012,            /* discard_limit */
            1023,            /* shared_discard_threshold */
            1023,            /* shared_sop_threshold */
            1023,            /* lossless_discard */
            682,            /* port_xoff */
            672,            /* port_xon */
            1023,            /* lossless_xoff */
            1023,            /* lossless_xon */
            100,             /* lossy_low_pri */
            196,             /* lossy_discard */
            0,               /* lossy_min */
            1                /* sop_discard_mode */
        }
    }
};

static const struct soc_tomahawk_fsaf_dur_timer {
    int frequency;
    int duration_timer;             /* ForceSAF duration timer */
} soc_tomahawk_fsaf_dur_timer_tbl[] = {
    { 1700,  3400 },
    { 1275,  2550 },
    { 1133,  2266 },
    {  850,  1700 },
    {  637,  1274 },      /*_TH_FORCE_SAF_TIMER_ENTRY_CNT=5*/
    {   -1,  12000}
};


static const soc_reg_t soc_tomahawk_obm_ca_ctrl_regs[_TH_PIPES_PER_DEV][
    _TH_PBLKS_PER_PIPE] = {
    {IDB_OBM0_CA_CONTROL_PIPE0r, IDB_OBM1_CA_CONTROL_PIPE0r,
     IDB_OBM2_CA_CONTROL_PIPE0r, IDB_OBM3_CA_CONTROL_PIPE0r,
     IDB_OBM4_CA_CONTROL_PIPE0r, IDB_OBM5_CA_CONTROL_PIPE0r,
     IDB_OBM6_CA_CONTROL_PIPE0r, IDB_OBM7_CA_CONTROL_PIPE0r},
    {IDB_OBM0_CA_CONTROL_PIPE1r, IDB_OBM1_CA_CONTROL_PIPE1r,
     IDB_OBM2_CA_CONTROL_PIPE1r, IDB_OBM3_CA_CONTROL_PIPE1r,
     IDB_OBM4_CA_CONTROL_PIPE1r, IDB_OBM5_CA_CONTROL_PIPE1r,
     IDB_OBM6_CA_CONTROL_PIPE1r, IDB_OBM7_CA_CONTROL_PIPE1r},
    {IDB_OBM0_CA_CONTROL_PIPE2r, IDB_OBM1_CA_CONTROL_PIPE2r,
     IDB_OBM2_CA_CONTROL_PIPE2r, IDB_OBM3_CA_CONTROL_PIPE2r,
     IDB_OBM4_CA_CONTROL_PIPE2r, IDB_OBM5_CA_CONTROL_PIPE2r,
     IDB_OBM6_CA_CONTROL_PIPE2r, IDB_OBM7_CA_CONTROL_PIPE2r},
    {IDB_OBM0_CA_CONTROL_PIPE3r, IDB_OBM1_CA_CONTROL_PIPE3r,
     IDB_OBM2_CA_CONTROL_PIPE3r, IDB_OBM3_CA_CONTROL_PIPE3r,
     IDB_OBM4_CA_CONTROL_PIPE3r, IDB_OBM5_CA_CONTROL_PIPE3r,
     IDB_OBM6_CA_CONTROL_PIPE3r, IDB_OBM7_CA_CONTROL_PIPE3r}
};

static const soc_reg_t obm_ctrl_regs[_TH_PIPES_PER_DEV][_TH_PBLKS_PER_PIPE] =
{
    {IDB_OBM0_CONTROL_PIPE0r, IDB_OBM1_CONTROL_PIPE0r, IDB_OBM2_CONTROL_PIPE0r,
     IDB_OBM3_CONTROL_PIPE0r,
     IDB_OBM4_CONTROL_PIPE0r, IDB_OBM5_CONTROL_PIPE0r, IDB_OBM6_CONTROL_PIPE0r,
     IDB_OBM7_CONTROL_PIPE0r},
    {IDB_OBM0_CONTROL_PIPE1r, IDB_OBM1_CONTROL_PIPE1r, IDB_OBM2_CONTROL_PIPE1r,
     IDB_OBM3_CONTROL_PIPE1r,
     IDB_OBM4_CONTROL_PIPE1r, IDB_OBM5_CONTROL_PIPE1r, IDB_OBM6_CONTROL_PIPE1r,
     IDB_OBM7_CONTROL_PIPE1r},
    {IDB_OBM0_CONTROL_PIPE2r, IDB_OBM1_CONTROL_PIPE2r, IDB_OBM2_CONTROL_PIPE2r,
     IDB_OBM3_CONTROL_PIPE2r,
     IDB_OBM4_CONTROL_PIPE2r, IDB_OBM5_CONTROL_PIPE2r, IDB_OBM6_CONTROL_PIPE2r,
     IDB_OBM7_CONTROL_PIPE2r},
    {IDB_OBM0_CONTROL_PIPE3r, IDB_OBM1_CONTROL_PIPE3r, IDB_OBM2_CONTROL_PIPE3r,
     IDB_OBM3_CONTROL_PIPE3r,
     IDB_OBM4_CONTROL_PIPE3r, IDB_OBM5_CONTROL_PIPE3r, IDB_OBM6_CONTROL_PIPE3r,
     IDB_OBM7_CONTROL_PIPE3r}
};

static const soc_reg_t idb_ca_cpu_ctrl[_TH_PIPES_PER_DEV]= {
         IDB_CA_CPU_CONTROL_PIPE0r,
         IDB_CA_CPU_CONTROL_PIPE1r,
         IDB_CA_CPU_CONTROL_PIPE2r,
         IDB_CA_CPU_CONTROL_PIPE3r};

static const soc_reg_t idb_ca_lpbk_ctrl[_TH_PIPES_PER_DEV]= {
         IDB_CA_LPBK_CONTROL_PIPE0r,
         IDB_CA_LPBK_CONTROL_PIPE1r,
         IDB_CA_LPBK_CONTROL_PIPE2r,
         IDB_CA_LPBK_CONTROL_PIPE3r};

/*! @fn int soc_tomahawk_get_pipe_from_phy_pnum(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate pipe number from physical port number.
 *  @returns pipe_number
 */
int
soc_tomahawk_get_pipe_from_phy_pnum(int pnum)
{
    return((pnum==_TH_PHY_PORT_LPBK0)                        ? 0 :
           (pnum==_TH_PHY_PORT_LPBK1)                        ? 1 :
           (pnum==_TH_PHY_PORT_LPBK2)                        ? 2 :
           (pnum==_TH_PHY_PORT_LPBK3)                        ? 3 :
           (pnum==_TH_PHY_PORT_CPU)                          ? 0 :
           (pnum==_TH_PHY_PORT_MNG0)                         ? 1 :
           (pnum==_TH_PHY_PORT_MNG1)                         ? 2 :
           (pnum>(_TH_PBLKS_PER_PIPE*_TH_PORTS_PER_PBLK*3)) ? 3 :
           (pnum>(_TH_PBLKS_PER_PIPE*_TH_PORTS_PER_PBLK*2)) ? 2 :
           (pnum>(_TH_PBLKS_PER_PIPE*_TH_PORTS_PER_PBLK*1)) ? 1 : 0);
}

/*! @fn int soc_tomahawk_get_pm_from_phy_pnum(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate PM number from physical port number.
 *  @returns PM number
 */
int
soc_tomahawk_get_pm_from_phy_pnum(int pnum)
{
    return (((pnum-1) & 0x1f)>>2);
}

/*! @fn int soc_tomahawk_get_subp_from_phy_pnum(int pnum)
 *  @param pnum Physical por number.
 *  @brief Helper function to calculate subport number from physical port number.
 *  @returns subport number
 */
int
soc_tomahawk_get_subp_from_phy_pnum(int pnum)
{
    return ((pnum-1)&0x3);
}

/*! @fn int soc_tomahawk_idb_clear_stats_new_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to clear statistics of the new ports that are comming
 *         up during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_clear_stats_new_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int prt;
    soc_reg_t reg;
    uint64 rval64, fldval64;
    int pnum;
    int pipe_num;
    int pm_num;
    int subp;

    const soc_reg_t obm_usage_regs[_TH_PIPES_PER_DEV][_TH_PBLKS_PER_PIPE][2]
        = {
        {{IDB_OBM0_MAX_USAGE_PIPE0r,
          IDB_OBM0_MAX_USAGE_PIPE0r},
         {IDB_OBM1_MAX_USAGE_PIPE0r, IDB_OBM1_MAX_USAGE_PIPE0r},
         {IDB_OBM2_MAX_USAGE_PIPE0r,
          IDB_OBM2_MAX_USAGE_PIPE0r},
         {IDB_OBM3_MAX_USAGE_PIPE0r, IDB_OBM3_MAX_USAGE_PIPE0r},
         {IDB_OBM4_MAX_USAGE_PIPE0r,
          IDB_OBM4_MAX_USAGE_PIPE0r},
         {IDB_OBM5_MAX_USAGE_PIPE0r, IDB_OBM5_MAX_USAGE_PIPE0r},
         {IDB_OBM6_MAX_USAGE_PIPE0r,
          IDB_OBM6_MAX_USAGE_PIPE0r},
         {IDB_OBM7_MAX_USAGE_PIPE0r, IDB_OBM7_MAX_USAGE_PIPE0r}},

        {{IDB_OBM0_MAX_USAGE_PIPE1r,
          IDB_OBM0_MAX_USAGE_PIPE1r},
         {IDB_OBM1_MAX_USAGE_PIPE1r, IDB_OBM1_MAX_USAGE_PIPE1r},
         {IDB_OBM2_MAX_USAGE_PIPE1r,
          IDB_OBM2_MAX_USAGE_PIPE1r},
         {IDB_OBM3_MAX_USAGE_PIPE1r, IDB_OBM3_MAX_USAGE_PIPE1r},
         {IDB_OBM4_MAX_USAGE_PIPE1r,
          IDB_OBM4_MAX_USAGE_PIPE1r},
         {IDB_OBM5_MAX_USAGE_PIPE1r, IDB_OBM5_MAX_USAGE_PIPE1r},
         {IDB_OBM6_MAX_USAGE_PIPE1r,
          IDB_OBM6_MAX_USAGE_PIPE1r},
         {IDB_OBM7_MAX_USAGE_PIPE1r, IDB_OBM7_MAX_USAGE_PIPE1r}},

        {{IDB_OBM0_MAX_USAGE_PIPE2r,
          IDB_OBM0_MAX_USAGE_PIPE2r},
         {IDB_OBM1_MAX_USAGE_PIPE2r, IDB_OBM1_MAX_USAGE_PIPE2r},
         {IDB_OBM2_MAX_USAGE_PIPE2r,
          IDB_OBM2_MAX_USAGE_PIPE2r},
         {IDB_OBM3_MAX_USAGE_PIPE2r, IDB_OBM3_MAX_USAGE_PIPE2r},
         {IDB_OBM4_MAX_USAGE_PIPE2r,
          IDB_OBM4_MAX_USAGE_PIPE2r},
         {IDB_OBM5_MAX_USAGE_PIPE2r, IDB_OBM5_MAX_USAGE_PIPE2r},
         {IDB_OBM6_MAX_USAGE_PIPE2r,
          IDB_OBM6_MAX_USAGE_PIPE2r},
         {IDB_OBM7_MAX_USAGE_PIPE2r, IDB_OBM7_MAX_USAGE_PIPE2r}},

        {{IDB_OBM0_MAX_USAGE_PIPE3r,
          IDB_OBM0_MAX_USAGE_PIPE3r},
         {IDB_OBM1_MAX_USAGE_PIPE3r, IDB_OBM1_MAX_USAGE_PIPE3r},
         {IDB_OBM2_MAX_USAGE_PIPE3r,
          IDB_OBM2_MAX_USAGE_PIPE3r},
         {IDB_OBM3_MAX_USAGE_PIPE3r, IDB_OBM3_MAX_USAGE_PIPE3r},
         {IDB_OBM4_MAX_USAGE_PIPE3r,
          IDB_OBM4_MAX_USAGE_PIPE3r},
         {IDB_OBM5_MAX_USAGE_PIPE3r, IDB_OBM5_MAX_USAGE_PIPE3r},
         {IDB_OBM6_MAX_USAGE_PIPE3r,
          IDB_OBM6_MAX_USAGE_PIPE3r},
         {IDB_OBM7_MAX_USAGE_PIPE3r, IDB_OBM7_MAX_USAGE_PIPE3r}}
        };

    for (prt = 0; prt < port_schedule_state->nport; prt++) {
        if (port_schedule_state->resource[prt].physical_port != -1) {
            pnum =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[
                        prt].logical_port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(pnum);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(pnum);
            subp = soc_tomahawk_get_subp_from_phy_pnum(pnum);
            for (i = 0; i < 2; i++) {
                COMPILER_64_ZERO(fldval64);
                reg = obm_usage_regs[pipe_num][pm_num][i];
                if (i == 0) {
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg,
                                                            REG_PORT_ANY, 0,
                                                            &rval64));
                    if (subp==0) {
                        soc_reg64_field_set(unit, reg, &rval64,
                                            PORT0_MAX_USAGEf,
                                            fldval64);
                    } else if (subp==1) {
                        soc_reg64_field_set(unit, reg, &rval64,
                                            PORT1_MAX_USAGEf,
                                            fldval64);
                    } else if (subp==2) {
                        soc_reg64_field_set(unit, reg, &rval64,
                                            PORT2_MAX_USAGEf,
                                            fldval64);
                    } else {
                        soc_reg64_field_set(unit, reg, &rval64,
                                            PORT3_MAX_USAGEf,
                                            fldval64);
                    }
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg,
                                                            REG_PORT_ANY, 0,
                                                            rval64));
                }
                else {
                    COMPILER_64_ZERO(rval64);
                    soc_reg64_field_set(unit, reg, &rval64, SHARED_MAX_USAGEf,
                                        fldval64);
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg,
                                                            REG_PORT_ANY, 0,
                                                            rval64));
                }
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_reset_buffer(int unit, int pipe_num, int pm_num,
 *              int subp, int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number within the PM: Ranges from 0 to 3.
 *  @param reset_buffer IDB OBM reset buffer: 1 - Apply reset. 0 - Release reset.
 *  @brief Helper function to apply / or release IDB OBM buffer reset.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_obm_reset_buffer(int unit, int pipe_num, int pm_num, int subp,
                                   int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;

    reg = obm_ctrl_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (subp == 0) {
        soc_reg_field_set(unit, reg, &rval, PORT0_RESETf, reset_buffer);
    } else if (subp == 1) {
        soc_reg_field_set(unit, reg, &rval, PORT1_RESETf, reset_buffer);
    } else if (subp == 2) {
        soc_reg_field_set(unit, reg, &rval, PORT2_RESETf, reset_buffer);
    } else {
        soc_reg_field_set(unit, reg, &rval, PORT3_RESETf, reset_buffer);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_ca_reset_buffer(int unit, int pipe_num, int pm_num,
 *              int subp, int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number within the PM: Ranges from 0 to 3.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_ca_reset_buffer(int unit, int pipe_num, int pm_num, int subp,
                                  int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;

    reg = soc_tomahawk_obm_ca_ctrl_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (subp == 0) {
        soc_reg_field_set(unit, reg, &rval, PORT0_RESETf, reset_buffer);
    } else if (subp == 1) {
        soc_reg_field_set(unit, reg, &rval, PORT1_RESETf, reset_buffer);
    } else if (subp == 2) {
        soc_reg_field_set(unit, reg, &rval, PORT2_RESETf, reset_buffer);
    } else {
        soc_reg_field_set(unit, reg, &rval, PORT3_RESETf, reset_buffer);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_lpbk_ca_reset_buffer(int unit, int pipe_num,
                      int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset
 *                  for loopback port.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_lpbk_ca_reset_buffer(int unit, int pipe_num, int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;
    reg = idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, PORT_RESETf, reset_buffer);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_lpbk_ca_in_reset(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @brief Helper function to check if the IDB loopback CA is in reset.
 *  @returns Reset status.
 */
int
soc_tomahawk_idb_lpbk_ca_in_reset(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;
    int buffer_in_reset;
    reg = idb_ca_lpbk_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    buffer_in_reset = soc_reg_field_get(unit, reg, rval, PORT_RESETf);
    return buffer_in_reset;
}

/*! @fn int soc_tomahawk_idb_cpu_ca_reset_buffer(int unit, int pipe_num,
                      int reset_buffer)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param reset_buffer IDB Cell Assembly reset buffer: 1 - Apply reset.
 *         0 - Release reset.
 *  @brief Helper function to apply / or release IDB Cell Assembly buffer reset
 *                  for CPU port.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_cpu_ca_reset_buffer(int unit, int pipe_num, int reset_buffer)
{
    soc_reg_t reg;
    uint32 rval;
    reg = idb_ca_cpu_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, PORT_RESETf, reset_buffer);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_cpu_ca_in_reset(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @brief Helper function to check if the IDB CPU CA is in reset.
 *  @returns Reset status;
 */
int
soc_tomahawk_idb_cpu_ca_in_reset(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval;
    int buffer_in_reset;
    reg = idb_ca_cpu_ctrl[pipe_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0, &rval));
    buffer_in_reset = soc_reg_field_get(unit, reg, rval, PORT_RESETf);
    return buffer_in_reset;
}

/*! @fn int soc_tomahawk_idb_port_mode_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to program IDB port mode for the new ports that are
 *         comming up during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_port_mode_set(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    int i;
    uint32 rval;
    int pnum;
    int pipe_num;
    int pm_num;
    int mode;
    int clport;
    static const int soc_tomahawk_hw_mode_values[SOC_TH_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2
    };


    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            pnum =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i
                    ].logical_port];
            clport = ((pnum-1)>>2);
            soc_tomahawk_port_ratio_get(unit, port_schedule_state, clport,
                                         &mode,
                                         0);
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(pnum);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(pnum);
            reg = soc_tomahawk_obm_ca_ctrl_regs[pipe_num][pm_num];
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      0, &rval));
            soc_reg_field_set(unit, reg, &rval, PORT_MODEf,
                              soc_tomahawk_hw_mode_values[mode]);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                      0, rval));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_wr_obm_shared_config(int unit, int pipe_num,
 *              int pm_num, int subp, int num_lanes)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port number.
 *  @param num_lanes Number of lanes in the port
 *  @param lossless Chip mode - 1: Lossless; 0: Lossy.
 *  @brief Helper function to config OBM shared buffer during flexing
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_wr_obm_shared_config(int unit, int pipe_num, int pm_num,
                                       int subp,
                                       int num_lanes, int lossless)
{
    soc_reg_t reg;
    int shared_discard_threshold;
    int shared_sop_threshold;
    int sop_discard_mode;
    uint32 rval;

    const soc_reg_t obm_shared_cfg[_TH_PIPES_PER_DEV][_TH_PBLKS_PER_PIPE] = {
        {IDB_OBM0_SHARED_CONFIG_PIPE0r, IDB_OBM1_SHARED_CONFIG_PIPE0r,
         IDB_OBM2_SHARED_CONFIG_PIPE0r, IDB_OBM3_SHARED_CONFIG_PIPE0r,
         IDB_OBM4_SHARED_CONFIG_PIPE0r, IDB_OBM5_SHARED_CONFIG_PIPE0r,
         IDB_OBM6_SHARED_CONFIG_PIPE0r, IDB_OBM7_SHARED_CONFIG_PIPE0r},
        {IDB_OBM0_SHARED_CONFIG_PIPE1r, IDB_OBM1_SHARED_CONFIG_PIPE1r,
         IDB_OBM2_SHARED_CONFIG_PIPE1r, IDB_OBM3_SHARED_CONFIG_PIPE1r,
         IDB_OBM4_SHARED_CONFIG_PIPE1r, IDB_OBM5_SHARED_CONFIG_PIPE1r,
         IDB_OBM6_SHARED_CONFIG_PIPE1r, IDB_OBM7_SHARED_CONFIG_PIPE1r},
        {IDB_OBM0_SHARED_CONFIG_PIPE2r, IDB_OBM1_SHARED_CONFIG_PIPE2r,
         IDB_OBM2_SHARED_CONFIG_PIPE2r, IDB_OBM3_SHARED_CONFIG_PIPE2r,
         IDB_OBM4_SHARED_CONFIG_PIPE2r, IDB_OBM5_SHARED_CONFIG_PIPE2r,
         IDB_OBM6_SHARED_CONFIG_PIPE2r, IDB_OBM7_SHARED_CONFIG_PIPE2r},
        {IDB_OBM0_SHARED_CONFIG_PIPE3r, IDB_OBM1_SHARED_CONFIG_PIPE3r,
         IDB_OBM2_SHARED_CONFIG_PIPE3r, IDB_OBM3_SHARED_CONFIG_PIPE3r,
         IDB_OBM4_SHARED_CONFIG_PIPE3r, IDB_OBM5_SHARED_CONFIG_PIPE3r,
         IDB_OBM6_SHARED_CONFIG_PIPE3r, IDB_OBM7_SHARED_CONFIG_PIPE3r}
    };

    reg = obm_shared_cfg[pipe_num][pm_num];
    if (lossless) {
        shared_discard_threshold =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].shared_discard_threshold;
        shared_sop_threshold =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].shared_sop_threshold;
        sop_discard_mode =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].sop_discard_mode;
    } else {
        shared_discard_threshold =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].shared_discard_threshold;
        shared_sop_threshold =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].shared_sop_threshold;
        sop_discard_mode =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].sop_discard_mode;
    }

    rval = 0;

    soc_reg_field_set(unit, reg, &rval, SOP_DISCARD_MODEf, sop_discard_mode);
    soc_reg_field_set(unit, reg, &rval, SOP_THRESHOLDf, shared_sop_threshold);
    soc_reg_field_set(unit, reg, &rval, DISCARD_THRESHOLDf,
                      shared_discard_threshold);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_wr_obm_flow_ctrl_cfg(int unit, int pipe_num,
 *              int pm_num, int subp, int num_lanes)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port number.
 *  @brief Helper function to config OBM shared buffer during flexing
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_wr_obm_flow_ctrl_cfg(int unit, int pipe_num, int pm_num,
                                       int subp, int lossless)
{
    soc_reg_t reg;
    uint64 rval64, fldval64;

    const soc_reg_t obm_flow_ctrl_cfg[_TH_PIPES_PER_DEV][_TH_PBLKS_PER_PIPE]
        = {
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r,
         IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE1r,
         IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE1r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE2r,
         IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE2r},
        {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE3r,
         IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE3r}
        };

    reg = obm_flow_ctrl_cfg[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, subp,
                                            &rval64));
    if (lossless) {
        COMPILER_64_SET(fldval64, 0, 256);
        soc_reg64_field_set(unit, reg, &rval64, XOFF_REFRESH_TIMERf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, FC_TYPEf, fldval64);
        COMPILER_64_SET(fldval64, 0, 1);
        soc_reg64_field_set(unit, reg, &rval64, PORT_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, fldval64);
        COMPILER_64_SET(fldval64, 0, 0xffff);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_PRIORITY_PROFILEf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_PRIORITY_PROFILEf, fldval64);
    } else {
        COMPILER_64_SET(fldval64, 0, 256);
        soc_reg64_field_set(unit, reg, &rval64, XOFF_REFRESH_TIMERf, fldval64);
        COMPILER_64_ZERO(fldval64);
        soc_reg64_field_set(unit, reg, &rval64, FC_TYPEf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, PORT_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_PRIORITY_PROFILEf, fldval64);
        soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_PRIORITY_PROFILEf, fldval64);
    }
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY, subp,
                                            rval64));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_wr_obm_ovs_en(int unit, int pipe_num, int pm_num,
 *              int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port numbber.
 *  @brief Helper function to config OBM CONTROL register during flexing
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_wr_obm_ovs_en(int unit, int pipe_num, int pm_num, int subp)
{
    soc_reg_t reg;
    uint32 rval;

    reg = obm_ctrl_regs[pipe_num][pm_num];
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                                    reg, REG_PORT_ANY, 0, &rval));
    if (subp == 0) {
        soc_reg_field_set(unit, reg, &rval, PORT0_OVERSUB_ENABLEf, 1);
    } else if (subp == 1) {
        soc_reg_field_set(unit, reg, &rval, PORT1_OVERSUB_ENABLEf, 1);
    } else if (subp == 2) {
        soc_reg_field_set(unit, reg, &rval, PORT2_OVERSUB_ENABLEf, 1);
    } else {
        soc_reg_field_set(unit, reg, &rval, PORT3_OVERSUB_ENABLEf, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_wr_obm_thresh(int unit, int pipe_num, int pm_num,
 *              int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port numbber.
 *  @param num_lanes Number of serdes lanes used in the port.
 *  @param lossless Chip mode - 1: Lossless; 0: Lossy.
 *  @brief Helper function to config OBM thresholds during flexing
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_wr_obm_thresh(int unit, int pipe_num, int pm_num, int subp,
                                int num_lanes, int lossless)
{
    soc_reg_t reg;
    int i;
    uint32 lossy_min, lossless0_discard, lossy_discard, lossy_low_pri,
           discard_limit, lossless1_discard;
    uint64 rval64, fldval64;

    const soc_reg_t obm_thresh_pp[_TH_PIPES_PER_DEV][_TH_PBLKS_PER_PIPE][2] =
    {
        {{IDB_OBM0_THRESHOLD_PIPE0r,
          IDB_OBM0_THRESHOLD_PIPE0r},
         {IDB_OBM1_THRESHOLD_PIPE0r, IDB_OBM1_THRESHOLD_PIPE0r},
         {IDB_OBM2_THRESHOLD_PIPE0r,
          IDB_OBM2_THRESHOLD_PIPE0r},
         {IDB_OBM3_THRESHOLD_PIPE0r, IDB_OBM3_THRESHOLD_PIPE0r},
         {IDB_OBM4_THRESHOLD_PIPE0r,
          IDB_OBM4_THRESHOLD_PIPE0r},
         {IDB_OBM5_THRESHOLD_PIPE0r, IDB_OBM5_THRESHOLD_PIPE0r},
         {IDB_OBM6_THRESHOLD_PIPE0r,
          IDB_OBM6_THRESHOLD_PIPE0r},
         {IDB_OBM7_THRESHOLD_PIPE0r, IDB_OBM7_THRESHOLD_PIPE0r}},

        {{IDB_OBM0_THRESHOLD_PIPE1r,
          IDB_OBM0_THRESHOLD_PIPE1r},
         {IDB_OBM1_THRESHOLD_PIPE1r, IDB_OBM1_THRESHOLD_PIPE1r},
         {IDB_OBM2_THRESHOLD_PIPE1r,
          IDB_OBM2_THRESHOLD_PIPE1r},
         {IDB_OBM3_THRESHOLD_PIPE1r, IDB_OBM3_THRESHOLD_PIPE1r},
         {IDB_OBM4_THRESHOLD_PIPE1r,
          IDB_OBM4_THRESHOLD_PIPE1r},
         {IDB_OBM5_THRESHOLD_PIPE1r, IDB_OBM5_THRESHOLD_PIPE1r},
         {IDB_OBM6_THRESHOLD_PIPE1r,
          IDB_OBM6_THRESHOLD_PIPE1r},
         {IDB_OBM7_THRESHOLD_PIPE1r, IDB_OBM7_THRESHOLD_PIPE1r}},

        {{IDB_OBM0_THRESHOLD_PIPE2r,
          IDB_OBM0_THRESHOLD_PIPE2r},
         {IDB_OBM1_THRESHOLD_PIPE2r, IDB_OBM1_THRESHOLD_PIPE2r},
         {IDB_OBM2_THRESHOLD_PIPE2r,
          IDB_OBM2_THRESHOLD_PIPE2r},
         {IDB_OBM3_THRESHOLD_PIPE2r, IDB_OBM3_THRESHOLD_PIPE2r},
         {IDB_OBM4_THRESHOLD_PIPE2r,
          IDB_OBM4_THRESHOLD_PIPE2r},
         {IDB_OBM5_THRESHOLD_PIPE2r, IDB_OBM5_THRESHOLD_PIPE2r},
         {IDB_OBM6_THRESHOLD_PIPE2r,
          IDB_OBM6_THRESHOLD_PIPE2r},
         {IDB_OBM7_THRESHOLD_PIPE2r, IDB_OBM7_THRESHOLD_PIPE2r}},

        {{IDB_OBM0_THRESHOLD_PIPE3r,
          IDB_OBM0_THRESHOLD_PIPE3r},
         {IDB_OBM1_THRESHOLD_PIPE3r, IDB_OBM1_THRESHOLD_PIPE3r},
         {IDB_OBM2_THRESHOLD_PIPE3r,
          IDB_OBM2_THRESHOLD_PIPE3r},
         {IDB_OBM3_THRESHOLD_PIPE3r, IDB_OBM3_THRESHOLD_PIPE3r},
         {IDB_OBM4_THRESHOLD_PIPE3r,
          IDB_OBM4_THRESHOLD_PIPE3r},
         {IDB_OBM5_THRESHOLD_PIPE3r, IDB_OBM5_THRESHOLD_PIPE3r},
         {IDB_OBM6_THRESHOLD_PIPE3r,
          IDB_OBM6_THRESHOLD_PIPE3r},
         {IDB_OBM7_THRESHOLD_PIPE3r, IDB_OBM7_THRESHOLD_PIPE3r}}
    };

    if (lossless) {
        discard_limit =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].discard_limit;
        lossy_min =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossy_min;
        lossless1_discard =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossless_discard;
        lossless0_discard =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossless_discard;
        lossy_discard =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossy_discard;
        lossy_low_pri =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossy_low_pri;
    } else {
        discard_limit =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].discard_limit;
        lossy_min =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossy_min;
        lossless1_discard =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossless_discard;
        lossless0_discard =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossless_discard;
        lossy_discard =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossy_discard;
        lossy_low_pri =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossy_low_pri;
    }
    for (i = 0; i < 2; i++) {
        reg = obm_thresh_pp[pipe_num][pm_num][i];
        if (i == 0) {
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                                    subp, &rval64));
            COMPILER_64_SET(fldval64, 0, lossy_min);
            soc_reg64_field_set(unit, reg, &rval64, LOSSY_MINf, fldval64);
            COMPILER_64_SET(fldval64, 0, lossless0_discard);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossy_discard);
            soc_reg64_field_set(unit, reg, &rval64, LOSSY_DISCARDf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossy_low_pri);
            soc_reg64_field_set(unit, reg, &rval64, LOSSY_LOW_PRIf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
        else {
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                                    subp, &rval64));
            COMPILER_64_SET(fldval64, 0, discard_limit);
            soc_reg64_field_set(unit, reg, &rval64, DISCARD_LIMITf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless1_discard);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_wr_obm_fc_threshold(int unit, int pipe_num, int pm_num, *              int subp, int num_lanes)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num Port Macro number.
 *  @param subp Sub-port number.
 *  @param num_lanes Number of serdeslanes used in the port.
 *  @param lossless Chip mode - 1: Lossless; 0: Lossy.
 *  @brief Helper function to config OBM flow-control thresholds during flexing
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_wr_obm_fc_threshold(int unit, int pipe_num, int pm_num,
                                      int subp,
                                      int num_lanes, int lossless)
{
    soc_reg_t reg;
    int i;
    uint32 lossless0_xon, lossless0_xoff, lossless1_xon, lossless1_xoff,
           port_xon, port_xoff;
    uint64 rval64, fldval64;
    const soc_reg_t obm_fc_thresh_pp[_TH_PIPES_PER_DEV][_TH_PBLKS_PER_PIPE][2
    ] = {
        {{IDB_OBM0_FC_THRESHOLD_PIPE0r,
          IDB_OBM0_FC_THRESHOLD_PIPE0r},
         {IDB_OBM1_FC_THRESHOLD_PIPE0r, IDB_OBM1_FC_THRESHOLD_PIPE0r},
         {IDB_OBM2_FC_THRESHOLD_PIPE0r,
          IDB_OBM2_FC_THRESHOLD_PIPE0r},
         {IDB_OBM3_FC_THRESHOLD_PIPE0r, IDB_OBM3_FC_THRESHOLD_PIPE0r},
         {IDB_OBM4_FC_THRESHOLD_PIPE0r,
          IDB_OBM4_FC_THRESHOLD_PIPE0r},
         {IDB_OBM5_FC_THRESHOLD_PIPE0r, IDB_OBM5_FC_THRESHOLD_PIPE0r},
         {IDB_OBM6_FC_THRESHOLD_PIPE0r,
          IDB_OBM6_FC_THRESHOLD_PIPE0r},
         {IDB_OBM7_FC_THRESHOLD_PIPE0r, IDB_OBM7_FC_THRESHOLD_PIPE0r}},

        {{IDB_OBM0_FC_THRESHOLD_PIPE1r,
          IDB_OBM0_FC_THRESHOLD_PIPE1r},
         {IDB_OBM1_FC_THRESHOLD_PIPE1r, IDB_OBM1_FC_THRESHOLD_PIPE1r},
         {IDB_OBM2_FC_THRESHOLD_PIPE1r,
          IDB_OBM2_FC_THRESHOLD_PIPE1r},
         {IDB_OBM3_FC_THRESHOLD_PIPE1r, IDB_OBM3_FC_THRESHOLD_PIPE1r},
         {IDB_OBM4_FC_THRESHOLD_PIPE1r,
          IDB_OBM4_FC_THRESHOLD_PIPE1r},
         {IDB_OBM5_FC_THRESHOLD_PIPE1r, IDB_OBM5_FC_THRESHOLD_PIPE1r},
         {IDB_OBM6_FC_THRESHOLD_PIPE1r,
          IDB_OBM6_FC_THRESHOLD_PIPE1r},
         {IDB_OBM7_FC_THRESHOLD_PIPE1r, IDB_OBM7_FC_THRESHOLD_PIPE1r}},

        {{IDB_OBM0_FC_THRESHOLD_PIPE2r,
          IDB_OBM0_FC_THRESHOLD_PIPE2r},
         {IDB_OBM1_FC_THRESHOLD_PIPE2r, IDB_OBM1_FC_THRESHOLD_PIPE2r},
         {IDB_OBM2_FC_THRESHOLD_PIPE2r,
          IDB_OBM2_FC_THRESHOLD_PIPE2r},
         {IDB_OBM3_FC_THRESHOLD_PIPE2r, IDB_OBM3_FC_THRESHOLD_PIPE2r},
         {IDB_OBM4_FC_THRESHOLD_PIPE2r,
          IDB_OBM4_FC_THRESHOLD_PIPE2r},
         {IDB_OBM5_FC_THRESHOLD_PIPE2r, IDB_OBM5_FC_THRESHOLD_PIPE2r},
         {IDB_OBM6_FC_THRESHOLD_PIPE2r,
          IDB_OBM6_FC_THRESHOLD_PIPE2r},
         {IDB_OBM7_FC_THRESHOLD_PIPE2r, IDB_OBM7_FC_THRESHOLD_PIPE2r}},

        {{IDB_OBM0_FC_THRESHOLD_PIPE3r,
          IDB_OBM0_FC_THRESHOLD_PIPE3r},
         {IDB_OBM1_FC_THRESHOLD_PIPE3r, IDB_OBM1_FC_THRESHOLD_PIPE3r},
         {IDB_OBM2_FC_THRESHOLD_PIPE3r,
          IDB_OBM2_FC_THRESHOLD_PIPE3r},
         {IDB_OBM3_FC_THRESHOLD_PIPE3r, IDB_OBM3_FC_THRESHOLD_PIPE3r},
         {IDB_OBM4_FC_THRESHOLD_PIPE3r,
          IDB_OBM4_FC_THRESHOLD_PIPE3r},
         {IDB_OBM5_FC_THRESHOLD_PIPE3r, IDB_OBM5_FC_THRESHOLD_PIPE3r},
         {IDB_OBM6_FC_THRESHOLD_PIPE3r,
          IDB_OBM6_FC_THRESHOLD_PIPE3r},
         {IDB_OBM7_FC_THRESHOLD_PIPE3r, IDB_OBM7_FC_THRESHOLD_PIPE3r}}
    };

    if (lossless) {
        lossless0_xon =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossless_xon;
        lossless1_xon =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossless_xon;
        lossless0_xoff =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossless_xoff;
        lossless1_xoff =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].lossless_xoff;
        port_xon =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].port_xon;
        port_xoff =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSLESS_INDEX]
            [num_lanes].port_xoff;
    } else {
        lossless0_xon =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossless_xon;
        lossless1_xon =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossless_xon;
        lossless0_xoff =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossless_xoff;
        lossless1_xoff =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].lossless_xoff;
        port_xon =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].port_xon;
        port_xoff =
            soc_tomahawk_obm_settings[_TH_OBM_PARAMS_LOSSY_INDEX]
            [num_lanes].port_xoff;
    }

    COMPILER_64_ZERO(rval64);

    for (i = 0; i < 2; i++) {
        reg = obm_fc_thresh_pp[pipe_num][pm_num][i];
        if (i == 0) {
            COMPILER_64_SET(fldval64, 0, lossless0_xon);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless0_xoff);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless1_xon);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_XONf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, lossless1_xoff);
            soc_reg64_field_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
        else {
            COMPILER_64_SET(fldval64, 0, port_xon);
            soc_reg64_field_set(unit, reg, &rval64, PORT_XONf, fldval64);
            COMPILER_64_SET(fldval64, 0, port_xoff);
            soc_reg64_field_set(unit, reg, &rval64, PORT_XOFFf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                                    subp, rval64));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_obm_poll_buffer_empty(int unit, int pipe_num,
 *              int pm_num, int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number.
 *  @brief Helper function to poll for IDB OBM buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_obm_poll_buffer_empty(int unit, int pipe_num, int pm_num,
                                        int subp)
{
    soc_reg_t reg;
    uint64 rval64, temp64;
    uint64 total_count;
    bool sim;

    const soc_reg_t obm_usage_regs[_TH_PIPES_PER_DEV][_TH_PBLKS_PER_PIPE] = {
        {IDB_OBM0_USAGE_PIPE0r, IDB_OBM1_USAGE_PIPE0r, IDB_OBM2_USAGE_PIPE0r,
         IDB_OBM3_USAGE_PIPE0r,
         IDB_OBM4_USAGE_PIPE0r, IDB_OBM5_USAGE_PIPE0r, IDB_OBM6_USAGE_PIPE0r,
         IDB_OBM7_USAGE_PIPE0r},
        {IDB_OBM0_USAGE_PIPE1r, IDB_OBM1_USAGE_PIPE1r, IDB_OBM2_USAGE_PIPE1r,
         IDB_OBM3_USAGE_PIPE1r,
         IDB_OBM4_USAGE_PIPE1r, IDB_OBM5_USAGE_PIPE1r, IDB_OBM6_USAGE_PIPE1r,
         IDB_OBM7_USAGE_PIPE1r},
        {IDB_OBM0_USAGE_PIPE2r, IDB_OBM1_USAGE_PIPE2r, IDB_OBM2_USAGE_PIPE2r,
         IDB_OBM3_USAGE_PIPE2r,
         IDB_OBM4_USAGE_PIPE2r, IDB_OBM5_USAGE_PIPE2r, IDB_OBM6_USAGE_PIPE2r,
         IDB_OBM7_USAGE_PIPE2r},
        {IDB_OBM0_USAGE_PIPE3r, IDB_OBM1_USAGE_PIPE3r, IDB_OBM2_USAGE_PIPE3r,
         IDB_OBM3_USAGE_PIPE3r,
         IDB_OBM4_USAGE_PIPE3r, IDB_OBM5_USAGE_PIPE3r, IDB_OBM6_USAGE_PIPE3r,
         IDB_OBM7_USAGE_PIPE3r}
    };

    sim = bcmdrd_feature_is_sim(unit);
    if (sim) {
        return SOC_E_NONE;
    }

    COMPILER_64_ZERO(temp64);
    reg = obm_usage_regs[pipe_num][pm_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, subp,
                                                &rval64));
        total_count = soc_reg64_field_get(unit, reg, rval64, TOTAL_COUNTf);
    } while (COMPILER_64_GT(total_count, temp64));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_ca_poll_buffer_empty(int unit, int pipe_num,
 *              int pm_num, int subp)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @param pm_num PM number within the pipe: Ranges from 0 to 15.
 *  @param subp Subport number.
 *  @brief Helper function to poll for IDB Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_ca_poll_buffer_empty(int unit, int pipe_num, int pm_num,
                                       int subp)
{
    soc_reg_t reg;
    uint64 rval64;
    uint64 ca_fifo_empty;
    bool sim;

    static const soc_reg_t ca_status_regs[_TH_PIPES_PER_DEV][
        _TH_PBLKS_PER_PIPE] = {
        {IDB_OBM0_CA_HW_STATUS_PIPE0r, IDB_OBM1_CA_HW_STATUS_PIPE0r,
         IDB_OBM2_CA_HW_STATUS_PIPE0r, IDB_OBM3_CA_HW_STATUS_PIPE0r,
         IDB_OBM4_CA_HW_STATUS_PIPE0r, IDB_OBM5_CA_HW_STATUS_PIPE0r,
         IDB_OBM6_CA_HW_STATUS_PIPE0r, IDB_OBM7_CA_HW_STATUS_PIPE0r},
        {IDB_OBM0_CA_HW_STATUS_PIPE1r, IDB_OBM1_CA_HW_STATUS_PIPE1r,
         IDB_OBM2_CA_HW_STATUS_PIPE1r, IDB_OBM3_CA_HW_STATUS_PIPE1r,
         IDB_OBM4_CA_HW_STATUS_PIPE1r, IDB_OBM5_CA_HW_STATUS_PIPE1r,
         IDB_OBM6_CA_HW_STATUS_PIPE1r, IDB_OBM7_CA_HW_STATUS_PIPE1r},
        {IDB_OBM0_CA_HW_STATUS_PIPE2r, IDB_OBM1_CA_HW_STATUS_PIPE2r,
         IDB_OBM2_CA_HW_STATUS_PIPE2r, IDB_OBM3_CA_HW_STATUS_PIPE2r,
         IDB_OBM4_CA_HW_STATUS_PIPE2r, IDB_OBM5_CA_HW_STATUS_PIPE2r,
         IDB_OBM6_CA_HW_STATUS_PIPE2r, IDB_OBM7_CA_HW_STATUS_PIPE2r},
        {IDB_OBM0_CA_HW_STATUS_PIPE3r, IDB_OBM1_CA_HW_STATUS_PIPE3r,
         IDB_OBM2_CA_HW_STATUS_PIPE3r, IDB_OBM3_CA_HW_STATUS_PIPE3r,
         IDB_OBM4_CA_HW_STATUS_PIPE3r, IDB_OBM5_CA_HW_STATUS_PIPE3r,
         IDB_OBM6_CA_HW_STATUS_PIPE3r, IDB_OBM7_CA_HW_STATUS_PIPE3r}
    };

    sim = bcmdrd_feature_is_sim(unit);
    if (sim) {
        return SOC_E_NONE;
    }

    reg = ca_status_regs[pipe_num][pm_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, reg, REG_PORT_ANY, 0,
                                                &rval64));
        switch (subp) {
        case 0:
            ca_fifo_empty = soc_reg64_field_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT0f);
            break;
        case 1:
            ca_fifo_empty = soc_reg64_field_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT1f);
            break;
        case 2:
            ca_fifo_empty = soc_reg64_field_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT2f);
            break;
        case 3:
            ca_fifo_empty = soc_reg64_field_get(unit, reg, rval64,
                                                FIFO_EMPTY_PORT3f);
            break;
        default:
            COMPILER_64_SET(ca_fifo_empty, 0, 1);
            break;
        }
    } while (COMPILER_64_IS_ZERO(ca_fifo_empty));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_ca_cpu_poll_buffer_empty(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @brief Helper function to poll for IDB CPU Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_ca_cpu_poll_buffer_empty(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 ca_fifo_empty;
    const soc_reg_t idb_ca_cpu_hw_status[_TH_PIPES_PER_DEV]= {
         IDB_CA_CPU_HW_STATUS_PIPE0r,
         IDB_CA_CPU_HW_STATUS_PIPE1r,
         IDB_CA_CPU_HW_STATUS_PIPE2r,
         IDB_CA_CPU_HW_STATUS_PIPE3r};
    bool sim;
    sim = bcmdrd_feature_is_sim(unit);
    if (sim) {
        return SOC_E_NONE;
    }
    reg = idb_ca_cpu_hw_status[pipe_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                   reg, REG_PORT_ANY, 0, &rval32));
        ca_fifo_empty = soc_reg_field_get(unit, reg, rval32,
                                                FIFO_EMPTYf);
    } while (!ca_fifo_empty);
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_ca_lpbk_poll_buffer_empty(int unit, int pipe_num)
 *  @param unit Chip unit number.
 *  @param pipe_num Pipe number.
 *  @brief Helper function to poll for IDB LOOPBACK port
           Cell Assembly buffer empty.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_ca_lpbk_poll_buffer_empty(int unit, int pipe_num)
{
    soc_reg_t reg;
    uint32 rval32;
    uint32 ca_fifo_empty;
    const soc_reg_t idb_ca_lpbk_hw_status[_TH_PIPES_PER_DEV]= {
         IDB_CA_LPBK_HW_STATUS_PIPE0r,
         IDB_CA_LPBK_HW_STATUS_PIPE1r,
         IDB_CA_LPBK_HW_STATUS_PIPE2r,
         IDB_CA_LPBK_HW_STATUS_PIPE3r};
    bool sim;
    sim = bcmdrd_feature_is_sim(unit);
    if (sim) {
        return SOC_E_NONE;
    }
    reg = idb_ca_lpbk_hw_status[pipe_num];
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                   reg, REG_PORT_ANY, 0, &rval32));
        ca_fifo_empty = soc_reg_field_get(unit, reg, rval32,
                                                FIFO_EMPTYf);
    } while (!ca_fifo_empty);
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_speed_to_bmop_class_map(int speed)
 *  @param speed Port speed.
 *  @brief Helper function to get bubble MOP class as below::
 *         The function look at the port speed & returns the encoding as below::
 *         0 - SAF,
 *         1 - 10GE,
 *         2 - HG[11],
 *         3 - 20GE,
 *         4 - 21[HG},
 *         5 - 25GE,
 *         6 - HG[27],
 *         7 - 40GE,
 *         8 - HG[42],
 *         9 - 50GE,
 *         10 - HG[53],
 *         11 - 100GE,
 *         12 - HG[106],
 *  @returns int
 */
int
soc_tomahawk_speed_to_bmop_class_map(int speed)
{
    int bmop_class;
    int i;

    bmop_class=0;
    for (i = 0; i <= 12; i++) {
        if (soc_tomahawk_bmop_cfg_tbl[i].speed == speed) {
            bmop_class = i;
            break;
        }
    }

    return bmop_class;
}

/*! @fn int soc_tomahawk_idb_obm_bubble_mop_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to re-configure BUBBLE MOP settings during flexport
 *         operation. The function looks at the port speed & the latency modes
 *         to determine what the bubble MOP settings should be.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_obm_bubble_mop_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval32;
    int pipe_num;
    int pm_num;
    int subp;
    int bubble_mop_disable;
    int phy_port;
    int port;
    int oversub;
    int latency;
    int i;
    int speed_encoding;

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            oversub = port_schedule_state->resource[i].oversub;
            latency = port_schedule_state->cutthru_prop.switch_bypass_mode;
            speed_encoding =
                (port_schedule_state->cutthru_prop.asf_modes[port]==
                 _SOC_ASF_MODE_SAF) ? 0 :
                soc_tomahawk_speed_to_bmop_class_map(
                    port_schedule_state->resource[i].speed);
            bubble_mop_disable =
                oversub ? !(soc_tomahawk_bmop_cfg_tbl[speed_encoding].
                            bmop_enable.
                            oversub[latency]) :
                !(soc_tomahawk_bmop_cfg_tbl[speed_encoding].
                  bmop_enable.line_rate[latency]);
            reg = obm_ctrl_regs[pipe_num][pm_num];
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY,
                                                      0, &rval32));
            if (subp==0) {
                soc_reg_field_set(unit, reg, &rval32, PORT0_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            } else if (subp ==1) {
                soc_reg_field_set(unit, reg, &rval32, PORT1_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            } else if (subp ==2) {
                soc_reg_field_set(unit, reg, &rval32, PORT2_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            } else {
                soc_reg_field_set(unit, reg, &rval32, PORT3_BUBBLE_MOP_DISABLEf,
                                  bubble_mop_disable);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                      0, rval32));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_obm_port_config_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to re-configure IDB OBM port config register default
 *         priority if lossless class.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_obm_port_config_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval32;
    int pipe_num;
    int pm_num;
    int subp;
    int phy_port;
    int port;
    int i;
    static const soc_reg_t obm_port_config_regs[_TH_PIPES_PER_DEV][
        _TH_PBLKS_PER_PIPE] = {
        {IDB_OBM0_PORT_CONFIG_PIPE0r, IDB_OBM1_PORT_CONFIG_PIPE0r,
         IDB_OBM2_PORT_CONFIG_PIPE0r, IDB_OBM3_PORT_CONFIG_PIPE0r,
         IDB_OBM4_PORT_CONFIG_PIPE0r, IDB_OBM5_PORT_CONFIG_PIPE0r,
         IDB_OBM6_PORT_CONFIG_PIPE0r, IDB_OBM7_PORT_CONFIG_PIPE0r},
        {IDB_OBM0_PORT_CONFIG_PIPE1r, IDB_OBM1_PORT_CONFIG_PIPE1r,
         IDB_OBM2_PORT_CONFIG_PIPE1r, IDB_OBM3_PORT_CONFIG_PIPE1r,
         IDB_OBM4_PORT_CONFIG_PIPE1r, IDB_OBM5_PORT_CONFIG_PIPE1r,
         IDB_OBM6_PORT_CONFIG_PIPE1r, IDB_OBM7_PORT_CONFIG_PIPE1r},
        {IDB_OBM0_PORT_CONFIG_PIPE2r, IDB_OBM1_PORT_CONFIG_PIPE2r,
         IDB_OBM2_PORT_CONFIG_PIPE2r, IDB_OBM3_PORT_CONFIG_PIPE2r,
         IDB_OBM4_PORT_CONFIG_PIPE2r, IDB_OBM5_PORT_CONFIG_PIPE2r,
         IDB_OBM6_PORT_CONFIG_PIPE2r, IDB_OBM7_PORT_CONFIG_PIPE2r},
        {IDB_OBM0_PORT_CONFIG_PIPE3r, IDB_OBM1_PORT_CONFIG_PIPE3r,
         IDB_OBM2_PORT_CONFIG_PIPE3r, IDB_OBM3_PORT_CONFIG_PIPE3r,
         IDB_OBM4_PORT_CONFIG_PIPE3r, IDB_OBM5_PORT_CONFIG_PIPE3r,
         IDB_OBM6_PORT_CONFIG_PIPE3r, IDB_OBM7_PORT_CONFIG_PIPE3r}
    };

    if (port_schedule_state->lossless) {
        for (i = 0; i < port_schedule_state->nport; i++) {
            if (port_schedule_state->resource[i].physical_port != -1) {
                port = port_schedule_state->resource[i].logical_port;
                phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[port];
                pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
                pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
                subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
                rval32=0;
                reg = obm_port_config_regs[pipe_num][pm_num];
                soc_reg_field_set(unit, reg, &rval32, PORT_PRIf, 2);
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg,
                                                          REG_PORT_ANY, subp,
                                                          rval32));
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_obm_dscp_map_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to re-configure IDB_OBM0_DSCP_MAP table as per the lossy/lossless configuration.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_obm_dscp_map_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe_num;
    int pm_num;
    int subp;
    int phy_port;
    int port;
    int i;
    int j;
    int count;
    int lossless;
    uint32 priority;
    static const soc_mem_t obm_pri_map_mem[][_TH_PIPES_PER_DEV][_TH_PORTS_PER_PBLK] = {
        {{IDB_OBM0_PRI_MAP_PORT0_PIPE0m, IDB_OBM0_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE0m, IDB_OBM0_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE1m, IDB_OBM0_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE1m, IDB_OBM0_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE2m, IDB_OBM0_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE2m, IDB_OBM0_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM0_PRI_MAP_PORT0_PIPE3m, IDB_OBM0_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM0_PRI_MAP_PORT2_PIPE3m, IDB_OBM0_PRI_MAP_PORT3_PIPE3m}},
        {{IDB_OBM1_PRI_MAP_PORT0_PIPE0m, IDB_OBM1_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE0m, IDB_OBM1_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE1m, IDB_OBM1_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE1m, IDB_OBM1_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE2m, IDB_OBM1_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE2m, IDB_OBM1_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM1_PRI_MAP_PORT0_PIPE3m, IDB_OBM1_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM1_PRI_MAP_PORT2_PIPE3m, IDB_OBM1_PRI_MAP_PORT3_PIPE3m}},
        {{IDB_OBM2_PRI_MAP_PORT0_PIPE0m, IDB_OBM2_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE0m, IDB_OBM2_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE1m, IDB_OBM2_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE1m, IDB_OBM2_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE2m, IDB_OBM2_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE2m, IDB_OBM2_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM2_PRI_MAP_PORT0_PIPE3m, IDB_OBM2_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM2_PRI_MAP_PORT2_PIPE3m, IDB_OBM2_PRI_MAP_PORT3_PIPE3m}},
        {{IDB_OBM3_PRI_MAP_PORT0_PIPE0m, IDB_OBM3_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE0m, IDB_OBM3_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE1m, IDB_OBM3_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE1m, IDB_OBM3_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE2m, IDB_OBM3_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE2m, IDB_OBM3_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM3_PRI_MAP_PORT0_PIPE3m, IDB_OBM3_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM3_PRI_MAP_PORT2_PIPE3m, IDB_OBM3_PRI_MAP_PORT3_PIPE3m}},
        {{IDB_OBM4_PRI_MAP_PORT0_PIPE0m, IDB_OBM4_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM4_PRI_MAP_PORT2_PIPE0m, IDB_OBM4_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM4_PRI_MAP_PORT0_PIPE1m, IDB_OBM4_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM4_PRI_MAP_PORT2_PIPE1m, IDB_OBM4_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM4_PRI_MAP_PORT0_PIPE2m, IDB_OBM4_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM4_PRI_MAP_PORT2_PIPE2m, IDB_OBM4_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM4_PRI_MAP_PORT0_PIPE3m, IDB_OBM4_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM4_PRI_MAP_PORT2_PIPE3m, IDB_OBM4_PRI_MAP_PORT3_PIPE3m}},
        {{IDB_OBM5_PRI_MAP_PORT0_PIPE0m, IDB_OBM5_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM5_PRI_MAP_PORT2_PIPE0m, IDB_OBM5_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM5_PRI_MAP_PORT0_PIPE1m, IDB_OBM5_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM5_PRI_MAP_PORT2_PIPE1m, IDB_OBM5_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM5_PRI_MAP_PORT0_PIPE2m, IDB_OBM5_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM5_PRI_MAP_PORT2_PIPE2m, IDB_OBM5_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM5_PRI_MAP_PORT0_PIPE3m, IDB_OBM5_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM5_PRI_MAP_PORT2_PIPE3m, IDB_OBM5_PRI_MAP_PORT3_PIPE3m}},
        {{IDB_OBM6_PRI_MAP_PORT0_PIPE0m, IDB_OBM6_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM6_PRI_MAP_PORT2_PIPE0m, IDB_OBM6_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM6_PRI_MAP_PORT0_PIPE1m, IDB_OBM6_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM6_PRI_MAP_PORT2_PIPE1m, IDB_OBM6_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM6_PRI_MAP_PORT0_PIPE2m, IDB_OBM6_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM6_PRI_MAP_PORT2_PIPE2m, IDB_OBM6_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM6_PRI_MAP_PORT0_PIPE3m, IDB_OBM6_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM6_PRI_MAP_PORT2_PIPE3m, IDB_OBM6_PRI_MAP_PORT3_PIPE3m}},
        {{IDB_OBM7_PRI_MAP_PORT0_PIPE0m, IDB_OBM7_PRI_MAP_PORT1_PIPE0m,
          IDB_OBM7_PRI_MAP_PORT2_PIPE0m, IDB_OBM7_PRI_MAP_PORT3_PIPE0m},
         {IDB_OBM7_PRI_MAP_PORT0_PIPE1m, IDB_OBM7_PRI_MAP_PORT1_PIPE1m,
          IDB_OBM7_PRI_MAP_PORT2_PIPE1m, IDB_OBM7_PRI_MAP_PORT3_PIPE1m},
         {IDB_OBM7_PRI_MAP_PORT0_PIPE2m, IDB_OBM7_PRI_MAP_PORT1_PIPE2m,
          IDB_OBM7_PRI_MAP_PORT2_PIPE2m, IDB_OBM7_PRI_MAP_PORT3_PIPE2m},
         {IDB_OBM7_PRI_MAP_PORT0_PIPE3m, IDB_OBM7_PRI_MAP_PORT1_PIPE3m,
          IDB_OBM7_PRI_MAP_PORT2_PIPE3m, IDB_OBM7_PRI_MAP_PORT3_PIPE3m}}
    };
    static const soc_field_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };

    sal_memset(entry, 0, sizeof(entry));

    lossless = port_schedule_state->lossless;
    priority = lossless ? _TH_OBM_PRIORITY_LOSSLESS0 : _TH_OBM_PRIORITY_LOSSY_LO;
    count = COUNTOF(obm_ob_pri_fields);

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            mem = obm_pri_map_mem[pm_num][pipe_num][subp];
            for (j = 0; j < count; j++) {
                soc_mem_field_set(unit, mem, entry, obm_ob_pri_fields[j], &priority);
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_obm_force_saf_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to set the force_SAF setting for OBM.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_idb_obm_force_saf_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval32;
    int pipe_num;
    int pm_num;
    int subp;
    int phy_port;
    int port;
    int i;
    int unsatisfied_threshold;
    int send_enable;
    int receive_enable;
    int duration_timer;
    uint32 ovs_prt_up;
    static const soc_reg_t obm_force_saf_config_regs[_TH_PIPES_PER_DEV][
        _TH_PBLKS_PER_PIPE] = {
        {IDB_OBM0_DBG_A_PIPE0r, IDB_OBM1_DBG_A_PIPE0r, IDB_OBM2_DBG_A_PIPE0r,
         IDB_OBM3_DBG_A_PIPE0r,
         IDB_OBM4_DBG_A_PIPE0r, IDB_OBM5_DBG_A_PIPE0r, IDB_OBM6_DBG_A_PIPE0r,
         IDB_OBM7_DBG_A_PIPE0r},
        {IDB_OBM0_DBG_A_PIPE1r, IDB_OBM1_DBG_A_PIPE1r, IDB_OBM2_DBG_A_PIPE1r,
         IDB_OBM3_DBG_A_PIPE1r,
         IDB_OBM4_DBG_A_PIPE1r, IDB_OBM5_DBG_A_PIPE1r, IDB_OBM6_DBG_A_PIPE1r,
         IDB_OBM7_DBG_A_PIPE1r},
        {IDB_OBM0_DBG_A_PIPE2r, IDB_OBM1_DBG_A_PIPE2r, IDB_OBM2_DBG_A_PIPE2r,
         IDB_OBM3_DBG_A_PIPE2r,
         IDB_OBM4_DBG_A_PIPE2r, IDB_OBM5_DBG_A_PIPE2r, IDB_OBM6_DBG_A_PIPE2r,
         IDB_OBM7_DBG_A_PIPE2r},
        {IDB_OBM0_DBG_A_PIPE3r, IDB_OBM1_DBG_A_PIPE3r, IDB_OBM2_DBG_A_PIPE3r,
         IDB_OBM3_DBG_A_PIPE3r,
         IDB_OBM4_DBG_A_PIPE3r, IDB_OBM5_DBG_A_PIPE3r, IDB_OBM6_DBG_A_PIPE3r,
         IDB_OBM7_DBG_A_PIPE3r}
    };
    static const soc_reg_t idb_force_saf_config_regs[_TH_PIPES_PER_DEV] =
    {IDB_DBG_B_PIPE0r, IDB_DBG_B_PIPE1r, IDB_DBG_B_PIPE2r, IDB_DBG_B_PIPE3r};

    ovs_prt_up=0;
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            if (port_schedule_state->resource[i].oversub) {
                send_enable=1;
                receive_enable=1;
                ovs_prt_up |= (1<<pipe_num);
            }else{
                send_enable=0;
                receive_enable=0;
            }
            unsatisfied_threshold =
                ((port_schedule_state->resource[i].speed == 10000)||
                 (port_schedule_state->resource[i].speed ==
                  11000)||
                 (port_schedule_state->resource[i].speed ==
                  20000)||
                 (port_schedule_state->resource[i].speed ==
                  21000)||
                 (port_schedule_state->resource[i].speed ==
                  40000)||
                 (port_schedule_state->resource[i].speed ==
                  42000)) ? 12 : 18;
            rval32=0;
            reg = obm_force_saf_config_regs[pipe_num][pm_num];
            soc_reg_field_set(unit, reg, &rval32, FIELD_Bf, send_enable);
            soc_reg_field_set(unit, reg, &rval32, FIELD_Af, receive_enable);
            soc_reg_field_set(unit, reg, &rval32, FIELD_Cf,
                              unsatisfied_threshold);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                      subp, rval32));
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "soc_tomahawk_idb_obm_force_saf_set ovs_prt_up:: %x \n"),
              ovs_prt_up));
    duration_timer=
        soc_tomahawk_fsaf_dur_timer_tbl[_TH_FORCE_SAF_TIMER_ENTRY_CNT].
        duration_timer;
    for (i=0; i<_TH_FORCE_SAF_TIMER_ENTRY_CNT; i++) {
        if (port_schedule_state->frequency ==
            soc_tomahawk_fsaf_dur_timer_tbl[i].frequency) {
            duration_timer=soc_tomahawk_fsaf_dur_timer_tbl[i].duration_timer;
        }
    }
    for (i = 0; i < _TH_PIPES_PER_DEV; i++) {
        pipe_num=i;
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk_idb_obm_force_saf_set ovs_prt_up:: %x "
                  "pipe_num %d\n"), ovs_prt_up,pipe_num));
        if (ovs_prt_up>>pipe_num) {
            reg = idb_force_saf_config_regs[pipe_num];
            rval32=0;
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "soc_tomahawk_idb_obm_force_saf_set Write to "
                      "IDB_FORCE_SAF_CONFIG pipe_num %d\n"),
                      pipe_num));
            soc_reg_field_set(unit, reg, &rval32, FIELD_Af, duration_timer);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY,
                                                      0, rval32));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_flex_dis_forwarding_traffic(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to Write EPC_LINK_BMAP table & wait. Follows the sequence
 *         specified in document "TH_uA_flexport.pdf", Section 6.6.1
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_flex_dis_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint32 epc_link_bmap_tbl[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    uint32 mask;
    int port;
    int wait_us;

    /* EPC_LINK_BMAP read, field modify and write. */
    sal_memset(epc_link_bmap_tbl, 0, sizeof(epc_link_bmap_tbl));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                     epc_link_bmap_tbl));
    soc_mem_field_get(unit, EPC_LINK_BMAPm, epc_link_bmap_tbl, PORT_BITMAPf,
                      memfld);

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Disable EPC_LINK_BITMAP READ:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));
    mask = 0xffffffff;
    /* Make EPC_LINK_BMAP=0 for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;
            memfld[(port>>5)] &= (mask^(1<<(port&0x1f)));
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Disable EPC_LINK_BITMAP write:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));

    soc_mem_field_set(unit, EPC_LINK_BMAPm, epc_link_bmap_tbl, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                      epc_link_bmap_tbl));

    /* Wait is 8ms if 10/100 Kbps, else 80 us. Please refer Michael Lau's doc:
     * "TH_uA_flexport.pdf", Section 6.6.1 */
    if ((port_schedule_state->in_port_map.log_port_speed[_TH_LOG_PORT_MNG0] <=
         100) ||
        (port_schedule_state->in_port_map.log_port_speed[_TH_LOG_PORT_MNG1] <=
         100)) {
        wait_us = 8000;
    } else {
        wait_us = 80;
    }
    #ifdef DV_ONLY
    wait_us = 1; /* ONly for DV*/
    #endif
    sal_usleep(wait_us + (SAL_BOOT_QUICKTURN ? 1: 0) * EMULATION_FACTOR);

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_flex_en_forwarding_traffic(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to Write EPC_LINK_BMAP table to enable forwarding traffic
 *         to port. Follows the sequence specified in document "TH_uA_flexport.pdf",
 *         Section 6.6.2, bullet number 5.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_flex_en_forwarding_traffic(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint32 epc_link_bmap_tbl[SOC_MAX_MEM_WORDS];
    uint32 memfld[5];
    int port;

    /* EPC_LINK_BMAP read, field modify and write. */
    sal_memset(epc_link_bmap_tbl, 0, sizeof(epc_link_bmap_tbl));
    sal_memset(memfld, 0, sizeof(memfld));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                     epc_link_bmap_tbl));
    soc_mem_field_get(unit, EPC_LINK_BMAPm, epc_link_bmap_tbl, PORT_BITMAPf,
                      memfld);

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Enable EPC_LINK_BITMAP READ:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));
    /* Make EPC_LINK_BMAP=1 for all the ports going up */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            port = port_schedule_state->resource[i].logical_port;
            memfld[(port>>5)] |= (0x1<<(port&0x1f));
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "Enable EPC_LINK_BITMAP write:: %x %x %x %x %x \n"),
                          memfld[0],memfld[1], memfld[2],memfld[3],memfld[4]));

    soc_mem_field_set(unit, EPC_LINK_BMAPm, epc_link_bmap_tbl, PORT_BITMAPf,
                      memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EPC_LINK_BMAPm, MEM_BLOCK_ALL, 0,
                                      epc_link_bmap_tbl));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_idb_port_down(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to follow IDB port down sequence for flexport operation.
 *         Follows the sequence specified in document "TH_uA_flexport.pdf", Section
 *         6.6.1.1.
 *         Two main parts:
 *            (1) Poll untill IDB buffers are empty.
 *            (2) Hold IDB buffers in reset state.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_flex_idb_port_down(int                        unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int reset_buffer;
    int phy_port;
    int subp;

    /* Poll until IDB buffers are empty for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == BCMPC_INVALID_PPORT) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_obm_poll_buffer_empty(
                                    unit, pipe_num, pm_num, subp));
            }
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_ca_poll_buffer_empty(
                                    unit, pipe_num, pm_num, subp));
            }else if((phy_port==_TH_PHY_PORT_LPBK0)||
                     (phy_port==_TH_PHY_PORT_LPBK1)||
                     (phy_port==_TH_PHY_PORT_LPBK2)||
                     (phy_port==_TH_PHY_PORT_LPBK3)){
                if (!soc_tomahawk_idb_lpbk_ca_in_reset(unit,pipe_num)){
                    SOC_IF_ERROR_RETURN(
                        soc_tomahawk_idb_ca_lpbk_poll_buffer_empty(
                        unit, pipe_num));
                }
            }else if((phy_port==_TH_PHY_PORT_CPU)||
                     (phy_port==_TH_PHY_PORT_MNG0)||
                     (phy_port==_TH_PHY_PORT_MNG1)){
                if (!soc_tomahawk_idb_cpu_ca_in_reset(unit,pipe_num)){
                    SOC_IF_ERROR_RETURN(
                        soc_tomahawk_idb_ca_cpu_poll_buffer_empty(
                        unit, pipe_num));
                }
            }
        }
    }

    /* Hold IDB buffers in reset state for all the ports going down */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == BCMPC_INVALID_PPORT) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            reset_buffer = 1;
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_obm_reset_buffer(unit,
                                                                pipe_num,
                                                                pm_num, subp,
                                                                reset_buffer));
            }
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_ca_reset_buffer(unit,
                                                                pipe_num,
                                                                pm_num, subp,
                                                                reset_buffer));
            }else if((phy_port==_TH_PHY_PORT_LPBK0)||
                     (phy_port==_TH_PHY_PORT_LPBK1)||
                     (phy_port==_TH_PHY_PORT_LPBK2)||
                     (phy_port==_TH_PHY_PORT_LPBK3)){
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_lpbk_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }else if((phy_port==_TH_PHY_PORT_CPU)||
                     (phy_port==_TH_PHY_PORT_MNG0)||
                     (phy_port==_TH_PHY_PORT_MNG1)){
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_cpu_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_flex_idb_reconfigure(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to reconfigure IDB during flexport operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_flex_idb_reconfigure(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int phy_port;
    int subp;
    int num_lanes;
    int lossless;

    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_flexport_idb(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_idb_hsp_set(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_idb_calendar_set(unit,
                                                            port_schedule_state));

    /* Reconfigure OBM thresholds for ports going up */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i
                    ].logical_port];
            num_lanes =
                port_schedule_state->out_port_map.port_num_lanes[
                port_schedule_state->resource[i].logical_port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            lossless = port_schedule_state->lossless;
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Reconfigure IDB: Write OBM CONTROL i %1d "
                      "phy_port %1d pipe=%1d pm=%1d subp=%1d "
                      "\n"),
                      i,phy_port, pipe_num, pm_num, subp));
            SOC_IF_ERROR_RETURN(soc_tomahawk_idb_wr_obm_ovs_en(unit, pipe_num,
                                                                pm_num,subp));
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Reconfigure IDB: Write OBM thresh i %1d "
                      "phy_port %1d pipe=%1d pm=%1d subp=%1d "
                      "num_lanes=%1d\n"),
                      i,phy_port, pipe_num, pm_num, subp, num_lanes));
            SOC_IF_ERROR_RETURN(soc_tomahawk_idb_wr_obm_thresh(unit, pipe_num,
                                                                pm_num,subp,
                                                                num_lanes,
                                                                lossless));
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Write Srared config i %1d pipe=%1d pm=%1d "
                      "subp=%1d num_lanes=%1d\n"),
                      i, pipe_num, pm_num, subp, num_lanes));
            SOC_IF_ERROR_RETURN(soc_tomahawk_idb_wr_obm_shared_config(unit,
                                                                       pipe_num,
                                                                       pm_num,
                                                                       subp,
                                                                       num_lanes,
                                                                       lossless));
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Write Flow_ctrl_cfg i %1d pipe=%1d pm=%1d "
                      "subp=%1d num_lanes=%1d\n"),
                      i, pipe_num, pm_num, subp, num_lanes));
            SOC_IF_ERROR_RETURN(soc_tomahawk_idb_wr_obm_flow_ctrl_cfg(unit,
                                                                       pipe_num,
                                                                       pm_num,
                                                                       subp,
                                                                       lossless));
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Write fc_thresh i %1d pipe=%1d pm=%1d subp=%1d "
                      "num_lanes=%1d\n"),
                      i, pipe_num, pm_num, subp, num_lanes));
            SOC_IF_ERROR_RETURN(soc_tomahawk_idb_wr_obm_fc_threshold(unit,
                                                                      pipe_num,
                                                                      pm_num,
                                                                      subp,
                                                                      num_lanes,
                                                                      lossless));
        }
    }

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_tomahawk_idb_port_mode_set \n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_idb_port_mode_set(unit,
                                                     port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_tomahawk_idb_obm_force_saf_set \n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_idb_obm_force_saf_set(unit,
                                                     port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_tomahawk_idb_obm_port_config_set \n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_idb_obm_port_config_set(unit,
                                                     port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_tomahawk_idb_obm_dscp_map_set \n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_idb_obm_dscp_map_set(unit,
                                                     port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_tomahawk_idb_clear_stats_new_ports \n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk_idb_clear_stats_new_ports(unit,
                                                     port_schedule_state));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk_flex_idb_port_up(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to bringup IDB port during flex operation.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk_flex_idb_port_up(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int pipe_num;
    int pm_num;
    int reset_buffer;
    int phy_port;
    int subp;

    /* Release IDB buffers from reset state for all the ports going up */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i
                    ].logical_port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            reset_buffer = 1;
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_obm_reset_buffer(unit,
                                                                pipe_num,
                                                                pm_num, subp,
                                                                reset_buffer));
            }
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               pm_num, subp,
                                                               reset_buffer));
            }else if((phy_port==_TH_PHY_PORT_LPBK0)||
                     (phy_port==_TH_PHY_PORT_LPBK1)||
                     (phy_port==_TH_PHY_PORT_LPBK2)||
                     (phy_port==_TH_PHY_PORT_LPBK3)){
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_lpbk_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }else if((phy_port==_TH_PHY_PORT_CPU)||
                     (phy_port==_TH_PHY_PORT_MNG0)||
                     (phy_port==_TH_PHY_PORT_MNG1)){
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_cpu_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }
        }
    }
    sal_usleep(5 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != BCMPC_INVALID_PPORT) {
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i
                    ].logical_port];
            pipe_num = soc_tomahawk_get_pipe_from_phy_pnum(phy_port);
            pm_num = soc_tomahawk_get_pm_from_phy_pnum(phy_port);
            subp = soc_tomahawk_get_subp_from_phy_pnum(phy_port);
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                             "IDB port Up i %1d phy_port %1d pipe=%1d pm=%1d "
                             "subp=%1d \n"),
                     i,
                     phy_port, pipe_num, pm_num, subp));
            reset_buffer = 0;
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_obm_reset_buffer(unit,
                                                              pipe_num,
                                                              pm_num, subp,
                                                              reset_buffer));
            }
            if (_TH_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_ca_reset_buffer(unit,
                                                              pipe_num,
                                                              pm_num, subp,
                                                              reset_buffer));
            }else if((phy_port==_TH_PHY_PORT_LPBK0)||
                     (phy_port==_TH_PHY_PORT_LPBK1)||
                     (phy_port==_TH_PHY_PORT_LPBK2)||
                     (phy_port==_TH_PHY_PORT_LPBK3)){
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_lpbk_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }else if((phy_port==_TH_PHY_PORT_CPU)||
                     (phy_port==_TH_PHY_PORT_MNG0)||
                     (phy_port==_TH_PHY_PORT_MNG1)){
                SOC_IF_ERROR_RETURN(soc_tomahawk_idb_cpu_ca_reset_buffer(unit,
                                                               pipe_num,
                                                               reset_buffer));
            }
        }
    }
    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

int
soc_tomahawk_idb_flexport(int                        unit,
                           soc_port_schedule_state_t *port_schedule_state)
{

    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_start(unit, port_schedule_state));

    /* IDB Ports down. */
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_idb_port_down(unit,
                                                    port_schedule_state));

    /* startTDM flex related codes: Florin's routines.*/
    SOC_IF_ERROR_RETURN(soc_tomahawk_tdm_calculation_flexport(unit,
                                                    port_schedule_state));

    /* Reconfigure IDB. */
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_idb_reconfigure(unit,
                                                    port_schedule_state));

    /* IDB Ports up*/
    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_idb_port_up(unit,
                                                    port_schedule_state));

    SOC_IF_ERROR_RETURN(soc_tomahawk_flex_end(unit, port_schedule_state));

    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK_SUPPORT */
