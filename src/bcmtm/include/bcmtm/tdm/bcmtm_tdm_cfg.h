/*! \file bcmtm_tdm_cfg.h
 *
 * Function prototypes for TDM user/chip/pipe configurations.
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

#ifndef BCMTM_TDM_CFG_H
#define BCMTM_TDM_CFG_H


#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_defines.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
#endif


/***********************************************************************
 * Internal functions: user config info
 */
/*!
 * \brief Get user config data on unit.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Unit.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_unit_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on device ID.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Device ID.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_dev_id_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on core clock frequency.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Core clock frequency.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_freq_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on number of general ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Nubmer of general ports.
 *
 * General ports include front panel ports + CPU + Management + Loopback.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_num_gports_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on flex_en.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Flex operation indicator.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_flex_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on chk_en.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Checker enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_chk_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on port state.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port state.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_port_state_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Set user config data on port state.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] state Port state.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_cfg_port_state_set(tdm_mod_t *tdm, int port, int state, int mode);

/*!
 * \brief Get user config data on port speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port speed.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_port_speed_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Set user config data on port speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] speed Port speed.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_cfg_port_speed_set(tdm_mod_t *tdm, int port, int speed, int mode);

/*!
 * \brief Get user config data on port speed in Ethernet speed mode.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port speed.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_port_speed_eth_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Print TDM config info.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_cfg_print(tdm_mod_t *tdm);

/***********************************************************************
 * Internal functions: chip config info
 */
/*!
 * \brief Get the lowest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest front panel port.
 */
TDM_EXTERN int
bcmtm_tdm_fport_lo_get(tdm_mod_t *tdm);

/*!
 * \brief Get the highest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest front panel port.
 */
TDM_EXTERN int
bcmtm_tdm_fport_hi_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of pipes.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of pipes.
 */
TDM_EXTERN int
bcmtm_tdm_num_pipes_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of PMs.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of PMs.
 */
TDM_EXTERN int
bcmtm_tdm_num_pms_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of lanes per PM.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of lanes per PM.
 */
TDM_EXTERN int
bcmtm_tdm_num_pm_lanes_get(tdm_mod_t *tdm);

/*!
 * \brief Get same port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Same port spacing.
 */
TDM_EXTERN int
bcmtm_tdm_min_same_space_get(tdm_mod_t *tdm, int port);

/*!
 * \brief Get sister port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Sister port spacing.
 */
TDM_EXTERN int
bcmtm_tdm_min_sister_space_get(tdm_mod_t *tdm, int port);

/*!
 * \brief Get port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Port PM number.
 */
TDM_EXTERN int
bcmtm_tdm_port_pm_get(tdm_mod_t *tdm, int port);

/*!
 * \brief Set port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_port_pm_set(tdm_mod_t *tdm, int port, int pm);


/*!
 * \brief Get empty port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Empty port token.
 */
TDM_EXTERN int
bcmtm_tdm_token_empty_get(tdm_mod_t *tdm);

/*!
 * \brief Get oversub port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Oversub port token.
 */
TDM_EXTERN int
bcmtm_tdm_token_ovsb_get(tdm_mod_t *tdm);

/*!
 * \brief Get ancilary port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Ancilary port token.
 */
TDM_EXTERN int
bcmtm_tdm_token_ancl_get(tdm_mod_t *tdm);

/*!
 * \brief Get idle-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle1 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_idl1(tdm_mod_t *tdm);

/*!
 * \brief Get idle-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle2 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_idl2(tdm_mod_t *tdm);

/*!
 * \brief Get idle port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_idle(tdm_mod_t *tdm);

/*!
 * \brief Get opportunistic-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-1 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_opp1(tdm_mod_t *tdm);

/*!
 * \brief Get opportunistic-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-2 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_opp2(tdm_mod_t *tdm);

/*!
 * \brief Get null port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval null port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_null(tdm_mod_t *tdm);


/***********************************************************************
 * Internal functions: pipe config info
 */
/*!
 * \brief Get pipe config data on slot granularity.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Slot unit in Mbps.
 *
 * Slot_unit represents the bandwidth per slot of linerate main calendar.
 * Note that the slot unit has the same precision of port speed.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_slot_unit_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on pipe index.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe ID.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_id_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on the lowest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest port number.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_port_lo_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on the highest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest port number.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_port_hi_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on limitiation of linerate slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max linerate slots.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_lr_limit_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on limitiation of ancilary slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max ancilary slots.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_ancl_limit_get(tdm_mod_t *tdm);



/*!
 * \brief Get pipe variable on linerate enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe linerate enable indicator.
 */
TDM_EXTERN char
bcmtm_tdm_pipe_lr_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe variable on oversub enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe oversub enable indicator.
 */
TDM_EXTERN char bcmtm_tdm_pipe_os_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe variable on linerate port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of linerate ports in pipe.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_lr_cnt_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe variable on oversub port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of oversub ports in pipe.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_os_cnt_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of speed types for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe linerate ports.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_lr_spd_types_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of speed types for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe oversub ports.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_os_spd_types_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port buffer for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe linerate port buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_lr_buff_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port count buffer for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe oversub port buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_os_buff_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port count per speed for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based oversub port count buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_lr_spd_prt_cnt_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port count per speed for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based linerate port count buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_os_spd_prt_cnt_get(tdm_mod_t *tdm);


#endif /* BCMTM_TDM_CFG_H */
