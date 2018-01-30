/*! \file bcm56960_a0_tm_tdm_chk_internal.c
 *
 * TDM calendar validity checking functions.
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

#ifdef _TDM_STANDALONE
    #include <bcm56960_a0_tm_tdm_top_internal.h>
#else
    #include <bcmtm/chip/bcm56960_a0_tm_tdm_top_internal.h>
#endif


/********************************************************************
 * Internal functions.
 */
/*!
 * \brief Check TDM chip-specific structures.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_struct_chk(tdm_mod_t *tdm)
{
    int freq, port, port_speed = 0, result = TDM_PASS;

    /* Check frequency. */
    freq = bcmtm_tdm_cfg_freq_get(tdm);
    switch (freq) {
        case TH_CLK_950MHZ:
        case TH_CLK_850MHZ:
        case TH_CLK_765MHZ:
        case TH_CLK_672MHZ:
        case TH_CLK_645MHZ:
        case TH_CLK_545MHZ:
            break;
        default:
            TDM_ERROR1("[Struct-Frequency] invalid frequency %d\n", freq);
            result = TDM_FAIL;
            break;
    }

    /* Check speed/state. */
    for (port = 1; port <= TH_NUM_PHY_PORTS; port++) {
        if (port < tdm->user_data.cfg.num_gports) {
                port_speed = tdm->user_data.cfg.port_speeds[port];
        }
        if (port_speed > TDM_SPEED_0 &&
            bcmtm_tdm_fport_chk(tdm, port) == TDM_TRUE) {
            if (bcmtm_tdm_port_lr_chk(tdm, port, 0) == TDM_TRUE ||
                bcmtm_tdm_port_os_chk(tdm, port, 0) == TDM_TRUE ||
                bcmtm_tdm_port_mgmt_chk(tdm, port, 0) == TDM_TRUE) {
                switch (port_speed) {
                    case TDM_SPEED_1G:
                    case TDM_SPEED_2P5G:
                    case TDM_SPEED_10G:
                    case TDM_SPEED_11G:
                    case TDM_SPEED_20G:
                    case TDM_SPEED_21G:
                    case TDM_SPEED_25G:
                    case TDM_SPEED_27G:
                    case TDM_SPEED_40G:
                    case TDM_SPEED_42G:
                    case TDM_SPEED_50G:
                    case TDM_SPEED_53G:
                    case TDM_SPEED_100G:
                    case TDM_SPEED_106G:
                        break;
                    default:
                        result = TDM_FAIL;
                        TDM_ERROR3("%s, port %3d, speed %dG\n",
                                   "[Struct-Speed], invalid speed",
                                   port, (port_speed / 1000));
                        break;
                }
            } else {
                result = TDM_FAIL;
                TDM_ERROR3("%s, port %3d, state %d\n",
                    "[Struct-State], invalid state",
                    port, bcmtm_tdm_cfg_port_state_get(tdm, port, 0));
            }
        }
    }

    return result;
}

/*!
 * \brief Check TDM linerate bandwidth per calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_pipe_lr_bw_chk(tdm_mod_t *tdm, int cal_id)
{
    int slot_req, slot_cnt = 0, result = TDM_PASS;
    int port, port_lo, port_hi, speed = 0;

    bcm56960_a0_tdm_port_range_get(tdm, cal_id, &port_lo, &port_hi);
    for (port = port_lo; (port >=0 && port <= port_hi); port++) {
        if (bcmtm_tdm_port_lr_chk(tdm, port, 0) == TDM_TRUE &&
            bcmtm_tdm_fport_chk(tdm, port) == TDM_TRUE) {
            slot_req = bcmtm_tdm_port_slots_get(tdm, port, 0);
            slot_cnt = bcmtm_tdm_cal_token_count(tdm, cal_id, port);
            if (slot_cnt < slot_req) {
                result = TDM_FAIL;
                if (port < tdm->user_data.cfg.num_gports) {
                    speed = tdm->user_data.cfg.port_speeds[port];
                }
                TDM_ERROR6("%s, cal_id %d, port %d, speed %dG, slots %d/%d\n",
                   "[Linerate Bandwidth] insufficient port bandwidth",
                   cal_id, port, (speed / 1000), slot_cnt, slot_req);
            }
        }
    }

    return result;
}

/*!
 * \brief Check TDM oversub bandwidth per calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_pipe_os_bw_chk(tdm_mod_t *tdm, int cal_id)
{
    int i, j, port_speed = 0, port, port_lo, port_hi, grp_speed,
        port_is_found = TDM_FALSE, result = TDM_PASS;
    int cal_grp_num, cal_grp_len, *cal_grp = NULL;

    cal_grp_num = bcmtm_tdm_os_grp_num_get(tdm, cal_id, 0);
    cal_grp_len = bcmtm_tdm_os_grp_len_get(tdm, cal_id, 0);
    if (cal_grp_num == 0 || cal_grp_len == 0) {
        return TDM_PASS;
    }

    bcm56960_a0_tdm_port_range_get(tdm, cal_id, &port_lo, &port_hi);

    /* Check port subscription. */
    for (port = port_lo; (port >= 0 && port <= port_hi); port++) {
        if (bcmtm_tdm_port_os_chk(tdm, port, 0) == TDM_TRUE &&
            bcmtm_tdm_fport_chk(tdm, port) == TDM_TRUE) {
            if (port < tdm->user_data.cfg.num_gports) {
                port_speed = tdm->user_data.cfg.port_speeds[port];
            }
            port_is_found = TDM_FALSE;
            for (i = 0; i < cal_grp_num; i++) {
                cal_grp = bcmtm_tdm_os_grp_get(tdm, cal_id, i, 0);
                for (j = 0; j < cal_grp_len; j++) {
                    if (port == cal_grp[j]) {
                        port_is_found = TDM_TRUE;
                        break;
                    }
                }
                if (port_is_found == TDM_TRUE) {
                    break;
                }
            }
            if (port_is_found == TDM_FALSE) {
                result = TDM_FAIL;
                TDM_ERROR4("%s, cal_id %d, port %d, speed %dG\n",
                           "[Oversub Bandwidth] Unfounded Oversub port",
                           cal_id, port, (port_speed / 1000));
            }
        }
    }
    /* Check if all ports within the same ovsb group have the same speed. */
    for (i = 0; i < cal_grp_num; i++) {
        cal_grp = bcmtm_tdm_os_grp_get(tdm, cal_id, i, 0);
        grp_speed = 0;
        for (j = 1; j < cal_grp_len; j++) {
            port = cal_grp[j];
            if (bcmtm_tdm_fport_chk(tdm, port) == TDM_TRUE) {
                port_speed = bcmtm_tdm_cfg_port_speed_eth_get(tdm, port, 0);
                if (grp_speed == TDM_SPEED_0) {
                    grp_speed = port_speed;
                    continue;
                }
                if (grp_speed != port_speed) {
                    result = TDM_FAIL;
                    TDM_ERROR5("%s, grp %d, grp_speed %dG, %dG port %0d\n",
                               "[Oversub Bandwidth] invalid OVSB group",
                               i, (grp_speed / 1000), (port_speed / 1000),
                               port);
                }
            }
        }
    }

    return result;
}

/*!
 * \brief Check TDM linerate bandwidth for all calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_lr_bw_chk(tdm_mod_t *tdm)
{
    int cal_id, result = TDM_PASS;

    for (cal_id = 0; cal_id < TDM_NUM_CALENDARS; cal_id++) {
        if (bcm56960_a0_tdm_pipe_lr_bw_chk(tdm, cal_id) != TDM_PASS) {
            result = TDM_FAIL;
        }
    }

    return result;
}

/*!
 * \brief Check TDM oversub bandwidth for all calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_os_bw_chk(tdm_mod_t *tdm)
{
    int cal_id, result = TDM_PASS;

    for (cal_id = TH_IDB_PIPE_0_CAL_ID;
         cal_id < TH_IDB_PIPE_3_CAL_ID; cal_id++) {
        if (bcm56960_a0_tdm_pipe_os_bw_chk(tdm, cal_id) != TDM_PASS) {
            result = TDM_FAIL;
        }
    }

    return result;
}

/*!
 * \brief Check TDM linerate and oversub bandwidth for all calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_bw_chk(tdm_mod_t *tdm)
{
    int result = TDM_PASS;

    /* Linerate ports. */
    if (bcm56960_a0_tdm_lr_bw_chk(tdm) != TDM_PASS) {
        result = TDM_FAIL;
    }
    /* Oversub ports. */
    if (bcm56960_a0_tdm_os_bw_chk(tdm) != TDM_PASS) {
        result = TDM_FAIL;
    }

    return result;
}

/*!
 * \brief Check TDM linerate jitter for given calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_pipe_lr_jitter_chk(tdm_mod_t *tdm, int cal_id)
{
    int i, k, port, port_speed = 0, port_space,
        space_max, space_min, pos_prev, pos_next,
        result = TDM_PASS;
    char port_exist[TH_NUM_PHY_PORTS + 1];
    int cal_len, *cal_main = NULL;

    /* Initialize variables. */
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
    cal_main = bcmtm_tdm_lr_cal_get(tdm, cal_id, 0);
    TDM_MEMSET(port_exist, 0, sizeof(char) * TH_NUM_PHY_PORTS);

    /* Check jitter for each linerate port in calendar. */
    for (i = 0; i < cal_len; i++) {
        port = cal_main[i];
        if (port >= 0 && port < tdm->user_data.cfg.num_gports) {
            port_speed = tdm->user_data.cfg.port_speeds[port];
        }
        if (bcmtm_tdm_fport_chk(tdm, port) == TDM_FALSE ||
            port_exist[port % (TH_NUM_PHY_PORTS + 1)] == TDM_TRUE ||
            port_speed == 0) {
            continue;
        }

        bcmtm_tdm_lr_speed_jitter_get(tdm, port_speed, cal_len,
                                      &space_min, &space_max);
        pos_prev = i;
        for (k = 1; k < cal_len; k++) {
            pos_next = (i + k) % cal_len;
            if (cal_main[pos_next] != port) {
                continue;
            }
            port_space = (pos_next + cal_len - pos_prev) % cal_len;
            if (port_space > space_max) {
                result = TDM_FAIL;
                TDM_ERROR8("%s %d prt %d spd %dG slot[%03d,%03d] %d > %d\n",
                           "[Linerate Jitter (MAX)] cal_id",
                          cal_id, port, (port_speed / 1000),
                          pos_prev, pos_next, port_space, space_max);
            } else if (port_space < space_min) {
                TDM_ERROR8("%s %d prt %d spd %dG slot[%03d,%03d] %d < %d\n",
                           "TDM: [Linerate Jitter (MIN)] cal_id",
                           cal_id, port, (port_speed / 1000),
                           pos_prev, pos_next, port_space, space_min);
            }
            pos_prev = pos_next;
        }
        port_exist[port % (TH_NUM_PHY_PORTS + 1)] = TDM_TRUE;
    }

    return result;
}

/*!
 * \brief Check TDM linerate jitter for all calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Checking passed.
 * \retval TDM_FAIL Checking failed.
 */
static int
bcm56960_a0_tdm_lr_jitter_chk(tdm_mod_t *tdm)
{
    int cal_id, result = TDM_PASS;

    for (cal_id = 0; cal_id < TDM_NUM_CALENDARS; cal_id++) {
        if (bcm56960_a0_tdm_pipe_lr_jitter_chk(tdm, cal_id) != TDM_PASS) {
            result = TDM_FAIL;
        }
    }

    return result;
}

/*!
 * \brief Check TDM calendar validity.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM checking successfully passed.
 * \retval TDM_FAIL TDM checking failed.
 */
int
bcm56960_a0_tdm_chk(tdm_mod_t *tdm)
{
    int result = TDM_PASS;

    TDM_SML_BAR
    TDM_PRINT0("\nTDM checker starting ... \n\n");
    TDM_SML_BAR

    /* Check structure. */
    TDM_PRINT0("TDM: Check Structure (speed, state, frequency)\n\n");
    if (bcm56960_a0_tdm_struct_chk(tdm) != TDM_PASS) {
        result = TDM_FAIL;
    }
    TDM_SML_BAR

    /* Check sister port spacing. */
    TDM_PRINT0("TDM: Check Sister-Port Spacing\n\n");
    if (bcmtm_tdm_sister_chk(tdm) != TDM_PASS) {
        result = TDM_FAIL;
    }
    TDM_SML_BAR

    /* Check same port spacing. */
    TDM_PRINT0("TDM: Check Same-Port Spacing\n\n");
    if (bcmtm_tdm_same_chk(tdm) != TDM_PASS) {
        result = TDM_FAIL;
    }
    TDM_SML_BAR

    /* Check port subscription. */
    TDM_PRINT0("TDM: Check Port Subscription\n\n");
    if (bcm56960_a0_tdm_bw_chk(tdm) != TDM_PASS) {
         result = TDM_FAIL;
    }
    TDM_SML_BAR

    /* Check linerate jitter. */
    TDM_PRINT0("TDM: Check Linerate Jitter\n\n");
    if (bcm56960_a0_tdm_lr_jitter_chk(tdm) != TDM_PASS) {
        result = TDM_FAIL;
    }
    TDM_SML_BAR

    /* Summarize check results. */
    TDM_PRINT0("\n");
    TDM_SML_BAR
    if (result == TDM_FAIL) {
        TDM_PRINT0("TDM: *** FAILED ***\n");
    } else {
        TDM_PRINT0("TDM: *** PASSED ***\n");
    }
    TDM_SML_BAR
    TDM_PRINT0("TDM: TDM checker is completed.\n");
    TDM_BIG_BAR

    return result;
}
