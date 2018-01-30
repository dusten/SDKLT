/*! \file bcmtm_tdm_sched.h
 *
 * Function prototypes for TDM schedulers.
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

#ifndef BCMTM_TDM_SCHED_H
#define BCMTM_TDM_SCHED_H


#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_defines.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
#endif


/***********************************************************************
 * Internal functions
 */
/*!
 * \brief Get linerate calendar enable indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Calendar linerate enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_lr_cal_en_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set linerate calendar enable indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Config value for calendar enable. 0-disable, 1->enable.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_lr_cal_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get linerate calendar valid indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar valid indicator.
 */
TDM_EXTERN int
bcmtm_tdm_lr_cal_valid_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set linerate calendar valid indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Config value for calendar valid. 0-invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_lr_cal_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get linerate calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar length.
 */
TDM_EXTERN int
bcmtm_tdm_lr_cal_len_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set linerate calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] cal_len Calendar length.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_lr_cal_len_set(tdm_mod_t *tdm, int cal_id, int cal_len, int mode);

/*!
 * \brief Get pointer of linerate calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pointer of linerate calendar.
 */
TDM_EXTERN int *
bcmtm_tdm_lr_cal_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Reserve ancillary bandwidth in linerate calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Ancillary slot assignment successfully passed.
 * \retval TDM_FAIL Ancillary slot assignment failed.
 */
int
bcmtm_tdm_lr_cal_ancl_assign(tdm_mod_t *tdm);

/*!
 * \brief Get enable indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_en_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set enable indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of enable indicator. 0->invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_os_grp_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get valid indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar valid indicator.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_valid_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set valid indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of valid indicator. 0->invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_os_grp_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get number of oversub groups by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Number of oversub groups.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_num_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set number of oversub groups by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_num Number of oversub groups.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_os_grp_num_set(tdm_mod_t *tdm, int cal_id, int grp_num, int mode);

/*!
 * \brief Get length of oversub group by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Length of oversub group.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_len_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set length of oversub group by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_len Length of oversub groups.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_os_grp_len_set(tdm_mod_t *tdm, int cal_id, int grp_len, int mode);

/*!
 * \brief Get oversub group calendar by calendar ID and group ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_id Group ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pointer of one oversub group.
 */
TDM_EXTERN int *
bcmtm_tdm_os_grp_get(tdm_mod_t *tdm, int cal_id, int grp_id, int mode);



/*!
 * \brief Get packet shaper calendar width by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper calendar width.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_wid_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set packet shaper calendar width by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_wid Calendar width.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_shp_cal_wid_set(tdm_mod_t *tdm, int cal_id, int cal_wid, int mode);

/*!
 * \brief Get packet shaper calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper calendar length.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_len_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set packet shaper calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_len Calendar length.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_shp_cal_len_set(tdm_mod_t *tdm, int cal_id, int cal_len, int mode);

/*!
 * \brief Get packet shaper calendar by calendar ID and subpipe ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] sp_id Subpipe ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pionter of packet shaper calendar.
 */
TDM_EXTERN int *
bcmtm_tdm_shp_cal_get(tdm_mod_t *tdm, int cal_id, int sp_id, int mode);

/*!
 * \brief Get enable indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_en_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set enable indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of enable indicator.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_shp_cal_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get valid indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper valid indicator.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_valid_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Set valid indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of valid indicator. 0->invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void bcmtm_tdm_shp_cal_valid_set(tdm_mod_t *tdm, int cal_id,
                                            int val, int mode);

/*!
 * \brief Get pm2pblk_map length by calendar ID and subpipe ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] sp_id Subpipe ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_shp_pm2pblk_map_len_get(tdm_mod_t *tdm, int cal_id, int sp_id,
                                  int mode);

/*!
 * \brief Set pm2pblk_map length by calendar ID and subpipe ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] sp_id Subpipe ID.
 * \param [in] cal_len Calendar length.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_shp_pm2pblk_map_len_set(tdm_mod_t *tdm, int cal_id, int sp_id,
                                  int cal_len, int mode);

/*!
 * \brief Set valid indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of valid indicator. 0->invalid, 1->valid.
 * \param [in] len Length of pm2pblk_map.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN int *
bcmtm_tdm_shp_pm2pblk_map_get(tdm_mod_t *tdm, int cal_id, int sp_id, int mode);

#endif /* BCMTM_TDM_SCHED_H */
