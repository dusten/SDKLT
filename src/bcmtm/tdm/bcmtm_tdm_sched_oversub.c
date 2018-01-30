/*! \file bcmtm_tdm_sched_oversub.c
 *
 * TDM core functions for oversub scheduler calendar.
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
    #include <bcmtm_tdm_top.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_top.h>
#endif


/***********************************************************************
 * Internal functions
 */

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
int *
bcmtm_tdm_os_grp_get(tdm_mod_t *tdm, int cal_id, int grp_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS &&
        grp_id < TDM_MAX_GRP_WID) {
        if (mode == TDM_CAL_CURR) {
            return ((int *) &(tdm->user_data.cals[cal_id].ovsb.cal[grp_id]));
        } else {
            return ((int *) &(tdm->prev_user_data.cals[cal_id].ovsb.
                              cal[grp_id]));
        }
    }

    return 0;
}

/*!
 * \brief Get number of oversub groups by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Number of oversub groups.
 */
int
bcmtm_tdm_os_grp_num_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.grp_num;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.grp_num;
        }
    }

    return 0;
}

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
void
bcmtm_tdm_os_grp_num_set(tdm_mod_t *tdm, int cal_id, int grp_num, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        grp_num = (grp_num <= TDM_MAX_GRP_WID) ? grp_num : TDM_MAX_GRP_WID;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.grp_num = grp_num;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.grp_num = grp_num;
        }
    }
}

/*!
 * \brief Get length of oversub group by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Length of oversub group.
 */
int
bcmtm_tdm_os_grp_len_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.grp_len;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.grp_len;
        }
    }

    return 0;
}

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
void
bcmtm_tdm_os_grp_len_set(tdm_mod_t *tdm, int cal_id, int grp_len, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        grp_len = (grp_len <= TDM_MAX_GRP_LEN) ? grp_len : TDM_MAX_GRP_LEN;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.grp_len = grp_len;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.grp_len = grp_len;
        }
    }
}

/*!
 * \brief Get enable indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar enable indicator.
 */
int
bcmtm_tdm_os_grp_en_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.cal_en;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.cal_en;
        }
    }
    return 0;
}

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
void
bcmtm_tdm_os_grp_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.cal_en = val;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.cal_en = val;
        }
    }
}

/*!
 * \brief Get valid indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar valid indicator.
 */
int
bcmtm_tdm_os_grp_valid_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.cal_valid;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.cal_valid;
        }
    }
    return 0;
}

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
void
bcmtm_tdm_os_grp_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.cal_valid = val;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.cal_valid = val;
        }
    }
}
