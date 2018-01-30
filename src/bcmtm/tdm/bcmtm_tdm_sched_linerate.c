/*! \file bcmtm_tdm_sched_linerate.c
 *
 * TDM core functions for linerate scheduler calendar.
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
 * \brief Get linerate calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar length.
 */
int
bcmtm_tdm_lr_cal_len_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal_len;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal_len;
        }
    }

    return 0;
}

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
void
bcmtm_tdm_lr_cal_len_set(tdm_mod_t *tdm, int cal_id, int cal_len, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        cal_len = cal_len <= TDM_MAX_CAL_LEN ? cal_len : TDM_MAX_CAL_LEN;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].lr.cal_len = cal_len;
        } else {
            tdm->prev_user_data.cals[cal_id].lr.cal_len = cal_len;
        }
    }
}

/*!
 * \brief Get pointer of linerate calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pointer of linerate calendar.
 */
int
*bcmtm_tdm_lr_cal_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal;
        }
    }

    return 0;
}

/*!
 * \brief Get linerate calendar enable indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Calendar linerate enable indicator.
 */
int
bcmtm_tdm_lr_cal_en_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal_en;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal_en;
        }
    }
    return 0;
}

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
void
bcmtm_tdm_lr_cal_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].lr.cal_en = val;
        } else {
            tdm->prev_user_data.cals[cal_id].lr.cal_en = val;
        }
    }
}

/*!
 * \brief Get linerate calendar valid indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar valid indicator.
 */
int
bcmtm_tdm_lr_cal_valid_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal_valid;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal_valid;
        }
    }
    return 0;
}

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
void
bcmtm_tdm_lr_cal_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].lr.cal_valid = val;
        } else {
            tdm->prev_user_data.cals[cal_id].lr.cal_valid = val;
        }
    }
}

/*!
 * \brief Reserve ancillary bandwidth in linerate calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Ancillary slot assignment successfully passed.
 * \retval TDM_FAIL Ancillary slot assignment failed.
 */
int
bcmtm_tdm_lr_cal_ancl_assign(tdm_mod_t *tdm)
{
    int i, j, k, k_prev, k_idx, idx_up, idx_dn, result = TDM_PASS;
    int ancl_num, cal_len, token_empty, token_ancl, lr_en;
    int *cal_main;
    int pipe_id;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    ancl_num = bcmtm_tdm_pipe_ancl_limit_get(tdm);
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
    token_empty = bcmtm_tdm_token_empty_get(tdm);
    token_ancl = bcmtm_tdm_token_ancl_get(tdm);
    lr_en = bcmtm_tdm_pipe_lr_en_get(tdm);

    /* allocate ANCL slots */
    cal_main = bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
    if (ancl_num > 0) {
        TDM_PRINT1("Allocate %d ANCL slots\n\n", ancl_num);
        if (lr_en == TDM_TRUE) {
            k = 0;
            for (j = 0; j < ancl_num; j++) {
                idx_up = 0;
                idx_dn = 0;
                k_idx = 0;
                k_prev = k;
                if (j == (ancl_num - 1)) {
                    k = cal_len - 1;
                } else {
                    k = k_prev + ((cal_len - k_prev) / (ancl_num - j)) - 1;
                }
                for (i = k; i > 0 && i < cal_len; i--) {
                    if (cal_main[i] == token_empty) {
                        idx_up = i;
                        break;
                    }
                }
                for (i = k; i < cal_len; i++) {
                    if (cal_main[i] == token_empty) {
                        idx_dn = i;
                        break;
                    }
                }
                if (idx_up > 0 || idx_dn > 0) {
                    if (idx_up > 0 && idx_dn > 0) {
                        k_idx = ((k - idx_up) < (idx_dn - k)) ?
                                idx_up : idx_dn;
                    } else if (idx_up > 0) {
                        k_idx = idx_up;
                    } else if (idx_dn > 0) {
                        k_idx = idx_dn;
                    }
                }
                if (k_idx == 0) {
                    for (i = cal_len-1; i > 0; i--) {
                        if (cal_main[i] == token_empty) {
                            k_idx = i;
                            break;
                        }
                    }
                }
                if (k_idx > 0 && k_idx < cal_len) {
                    cal_main[k_idx] = token_ancl;
                    TDM_PRINT2("Allocate %d-th ANCL slot at index #%03d\n",
                                j+1, k_idx);
                } else {
                    TDM_WARN1("Failed in %d-th ANCL slot allocation\n",j);
                    result = TDM_FAIL;
                }
            }
        } else {
            k_prev = 0;
            for (j = 0; j < ancl_num; j++) {
                k_idx = k_prev + ((cal_len - k_prev) / (ancl_num - j)) - 1;
                cal_main[k_idx] = token_ancl;
                k_prev = k_idx + 1;
                TDM_PRINT1("Allocate ANCL slot at index #%03d\n", k_idx);
            }
        }
        TDM_SML_BAR
    }

    return result;
}
