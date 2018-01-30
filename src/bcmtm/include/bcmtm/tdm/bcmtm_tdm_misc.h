/*! \file bcmtm_tdm_misc.h
 *
 * Miscellaneous functions of TDM core algorithm.
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

#ifndef BCMTM_TDM_MISC_H
#define BCMTM_TDM_MISC_H


#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_defines.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
#endif


/***********************************************************************
 * Internal functions
 */
/*!
 * \brief Initialize TDM core resources.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Initilaization passed.
 * \retval TDM_FAIL  Initilaization failed.
 */
TDM_EXTERN int
bcmtm_tdm_core_init(tdm_mod_t *tdm);

/*!
 * \brief TDM null funtion.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS.
 */
TDM_EXTERN int
bcmtm_tdm_core_null(tdm_mod_t *tdm);

/*!
 * \brief Populate linerate calendar based on vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Lineate calendar generation completed successfully.
 * \retval TDM_FAIL Linerate calendar generation failed.
 */
TDM_EXTERN int
bcmtm_tdm_vmap_drv(tdm_mod_t *tdm);


/*!
 * \brief Get speed class by port token.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Port mode.
 *
 * \retval Speed class.
 */
TDM_EXTERN int
bcmtm_tdm_port_speed_idx_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Get number of slots for a speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] speed Port Ethernet speed.
 *
 * \retval Number of slots.
 */
TDM_EXTERN int
bcmtm_tdm_port_slots_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Get number of slots for a port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Port mode.
 *
 * \retval Number of slots.
 */
TDM_EXTERN int
bcmtm_tdm_speed_slots_get(tdm_mod_t *tdm, int port_speed);


/***********************************************************************
 * Internal functions: checker
 */
/*!
 * \brief Check sister-port-space restriction among all linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
TDM_EXTERN int
bcmtm_tdm_sister_chk(tdm_mod_t *tdm);

/*!
 * \brief Check same-port-space restriction among all linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
TDM_EXTERN int
bcmtm_tdm_same_chk(tdm_mod_t *tdm);

/*!
 * \brief Get number of slots allocated for a given token in a calendar.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of slots allocated for a given token.
 */
TDM_EXTERN int
bcmtm_tdm_cal_token_count(tdm_mod_t *tdm, int cal_id, int token);

/*!
 * \brief Check linerate port bandwidth in a given calendar.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
TDM_EXTERN int
bcmtm_tdm_lr_port_bw_chk(tdm_mod_t *tdm, int cal_id, int port);

/*!
 * \brief Check if given port is a general port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 *
 * General port = front panel + CPU + MGMT + Loopback
 */
TDM_EXTERN int
bcmtm_tdm_gport_chk(tdm_mod_t *tdm, int port);

/*!
 * \brief Check if given port is a front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_fport_chk(tdm_mod_t *tdm, int port);

/*!
 * \brief Check if given port is a linerate port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_lr_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is an oversub port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_os_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is an Ethernet port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_eth_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is a higig port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_hg_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is a management port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_mgmt_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Calculate sister port distance of port at given position.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal Pointer of linerate calendar array.
 * \param [in] cal_len length of linerate calendar array.
 * \param [in] espace Extra space.
 * \param [in] pos Slot position.
 * \param [in] dir Moving direction.
 * \retval Sister port distance if violation detected, otherwise 0.
 */
TDM_EXTERN int
bcmtm_tdm_slot_sister_dist_get(tdm_mod_t *tdm, int *cal, int cal_len,
                               int espace, int pos, int dir);

/*!
 * \brief Calculate same port distance of port at given position.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal Pointer of linerate calendar array.
 * \param [in] cal_len length of linerate calendar array.
 * \param [in] espace Extra space.
 * \param [in] pos Slot position.
 * \param [in] dir Moving direction.
 * \retval Same port distance if violation detected, otherwise 0.
 */
TDM_EXTERN int
bcmtm_tdm_slot_same_dist_get(tdm_mod_t *tdm, int *cal, int cal_len,
                             int espace, int pos, int dir);

/*!
 * \brief Get max and min linerate jitter under given speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] speed Port speed in Mbps.
 * \param [in] cal_len Calendar length.
 * \param [out] jitter_min Pointer of min jitter.
 * \param [out] jitter_max Pointer of max jitter.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_speed_jitter_get(tdm_mod_t *tdm, int speed, int cal_len,
                              int *jitter_min, int *jitter_max);

/*!
 * \brief Print TDM pipe config info.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_pipe_config_print(tdm_mod_t *tdm);

/*!
 * \brief Generate pipe oversub ports into pipe oversub speed groups.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Oversub group generated successfully.
 * \retval TDM_FAIL Oversub group generated failed.
 */
int
bcmtm_tdm_pipe_os_grp_gen(tdm_mod_t *tdm);

/*!
 * \brief Print pipe oversub groups.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_os_grp_print(tdm_mod_t *tdm);

/*!
 * \brief Print pipe linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_cal_print(tdm_mod_t *tdm, int cal_id);

#endif /* BCMTM_TDM_MISC_H */
