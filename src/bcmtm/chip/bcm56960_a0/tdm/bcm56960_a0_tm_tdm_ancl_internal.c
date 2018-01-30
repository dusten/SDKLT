/*! \file bcm56960_a0_tm_tdm_ancl_internal.c
 *
 * Ancillary bandwidth assignment functions.
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
 * \brief Print pipe linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \return Nothing.
 */
static void
bcm56960_a0_tdm_lr_cal_print(tdm_mod_t *tdm, int cal_id)
{
    TDM_PRINT2("%12s %3d\n", "TOKEN_CMIC", TH_CMIC_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_MGM1", TH_MGM1_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_MGM2", TH_MGM2_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_LPB0", TH_LPB0_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_LPB1", TH_LPB1_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_LPB2", TH_LPB2_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_LPB3", TH_LPB3_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_NULL", TH_NULL_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_OPP1", TH_OPP1_TOKEN);
    TDM_PRINT2("%12s %3d\n", "TOKEN_IDLE", TH_IDLE_TOKEN);

    bcmtm_tdm_lr_cal_print(tdm, cal_id);
}

/*!
 * \brief Assign ancillary bandwidth for IDB calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval TDM_PASS Ancillary assignment successfully passed.
 * \retval TDM_FAIL Ancillary assignment failed.
 */
static int
bcm56960_a0_tdm_ancl_idb_parse(tdm_mod_t *tdm, int cal_id)
{
    int j, cnt = 0, token_ancl, cal_len, *cal = NULL;
    int tokens[TH_NUM_ANCL];
    int token_cmic_mgmt, token_lpbk;

    token_ancl = bcmtm_tdm_token_ancl_get(tdm);
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
    cal = bcmtm_tdm_lr_cal_get(tdm, cal_id, 0);

    TDM_MEMSET(tokens, 0, sizeof(int) * TH_NUM_ANCL);

    switch (cal_id) {
        case TH_IDB_PIPE_0_CAL_ID:
            token_cmic_mgmt = TH_CMIC_TOKEN;
            token_lpbk = TH_LPB0_TOKEN;
            break;
        case TH_IDB_PIPE_1_CAL_ID:
            token_cmic_mgmt = TH_MGM1_TOKEN;
            token_lpbk = TH_LPB1_TOKEN;
            break;
        case TH_IDB_PIPE_2_CAL_ID:
            token_cmic_mgmt = TH_MGM2_TOKEN;
            token_lpbk = TH_LPB2_TOKEN;
            break;
        case TH_IDB_PIPE_3_CAL_ID:
            token_cmic_mgmt = TH_OPP1_TOKEN;
            token_lpbk = TH_LPB3_TOKEN;
            break;
        default:
            token_cmic_mgmt = TH_OVSB_TOKEN;
            token_lpbk = TH_OVSB_TOKEN;
            break;
    }
    for (cnt = 0; cnt < TH_NUM_ANCL; cnt++) {
        switch (cnt) {
            /* CMIC or MGMT. */
            case 0:
            case 2:
            case 6:
            case 8:
                tokens[cnt] = token_cmic_mgmt;
                break;
            /* OPPORTUNISTIC. */
            case 3:
            case 9:
                tokens[cnt] = TH_OPP1_TOKEN;
                break;
            /* NULL. */
            case 4:
                tokens[cnt] = TH_NULL_TOKEN;
                break;
            /* SBUS. */
            case 7:
                tokens[cnt] = TH_IDLE_TOKEN;
                break;
            /* LOOPBACK. */
/*             case 1:
            case 5: */
            default:
                tokens[cnt] = token_lpbk;
                break;
        }
    }

    if (cal != NULL && cal_len > 0) {
        cnt = 0;
        for (j = 0; j < cal_len; j++) {
            if (cal[j] == token_ancl) {
                cal[j] = tokens[(++cnt) % TH_NUM_ANCL];
            }
        }
    }

    return TDM_PASS;
}

/*!
 * \brief Assign ancillary bandwidth for MMU calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval TDM_PASS Ancillary assignment successfully passed.
 * \retval TDM_FAIL Ancillary assignment failed.
 */
static int
bcm56960_a0_tdm_ancl_mmu_parse(tdm_mod_t *tdm, int cal_id)
{
    int cnt = 0, j, token_ancl, cal_len, *cal = NULL;
    int tokens[TH_NUM_ANCL];
    int token_cmic_mgmt, token_lpbk;

    token_ancl = bcmtm_tdm_token_ancl_get(tdm);
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
    cal = bcmtm_tdm_lr_cal_get(tdm, cal_id, 0);

    TDM_MEMSET(tokens, 0, sizeof(int) * TH_NUM_ANCL);

    switch (cal_id) {
        case TH_MMU_PIPE_0_CAL_ID:
            token_cmic_mgmt = TH_CMIC_TOKEN;
            token_lpbk = TH_LPB0_TOKEN;
            break;
        case TH_MMU_PIPE_1_CAL_ID:
            token_cmic_mgmt = TH_MGM1_TOKEN;
            token_lpbk = TH_LPB1_TOKEN;
            break;
        case TH_MMU_PIPE_2_CAL_ID:
            token_cmic_mgmt = TH_MGM2_TOKEN;
            token_lpbk = TH_LPB2_TOKEN;
            break;
        case TH_MMU_PIPE_3_CAL_ID:
            token_cmic_mgmt = TH_OPP1_TOKEN;
            token_lpbk = TH_LPB3_TOKEN;
            break;
        default:
            token_cmic_mgmt = TH_OVSB_TOKEN;
            token_lpbk = TH_OVSB_TOKEN;
            break;
    }
    for (cnt = 0; cnt < TH_NUM_ANCL; cnt++) {
        switch (cnt) {
            /* CMIC or MGMT. */
            case 0:
            case 2:
            case 6:
            case 8:
                tokens[cnt] = token_cmic_mgmt;
                break;
            /* NULL. */
            case 3:
            case 7:
            case 9:
                tokens[cnt] = TH_NULL_TOKEN;
                break;
            /* OPPORTUNISTIC. */
            case 4:
                tokens[cnt] = TH_OPP1_TOKEN;
                break;
            /* LOOPBACK. */
/*             case 1:
            case 5: */
            default:
                tokens[cnt] = token_lpbk;
                break;
        }
    }

    if (cal != NULL && cal_len > 0) {
        cnt = 0;
        for (j = 0; j < cal_len; j++) {
            if (cal[j] == token_ancl) {
                cal[j] = tokens[(++cnt) % TH_NUM_ANCL];
            }
        }
    }

    return TDM_PASS;
}

/*!
 * \brief Assign ancillary bandwidth for given calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval TDM_PASS Ancillary assignment successfully passed.
 * \retval TDM_FAIL Ancillary assignment failed.
 */
static int
bcm56960_a0_tdm_ancl_cal_parse(tdm_mod_t *tdm, int cal_id)
{
    int idb_en = 0, mmu_en = 0, result = TDM_PASS;

    switch (cal_id) {
        case TH_MMU_PIPE_0_CAL_ID:
        case TH_MMU_PIPE_1_CAL_ID:
        case TH_MMU_PIPE_2_CAL_ID:
        case TH_MMU_PIPE_3_CAL_ID:
            mmu_en = 1;
            break;
        case TH_IDB_PIPE_0_CAL_ID:
        case TH_IDB_PIPE_1_CAL_ID:
        case TH_IDB_PIPE_2_CAL_ID:
        case TH_IDB_PIPE_3_CAL_ID:
            idb_en = 1;
            break;
        default:
            TDM_ERROR1("Invalid calendar ID - %0d\n", cal_id);
            return TDM_FAIL;
            break;
    }

    /* Parse ancillary slots : CPU, MGMT, LOOPBACK, OPP1, NULL, IDLE. */
    if (idb_en == 1) {
        if (bcm56960_a0_tdm_ancl_idb_parse(tdm, cal_id) != TDM_PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = TDM_FAIL;
        }
    } else if (mmu_en == 1) {
        if (bcm56960_a0_tdm_ancl_mmu_parse(tdm, cal_id) != TDM_PASS) {
            TDM_ERROR1("Failed in ANCL allocation, cal_id %0d\n", cal_id);
            result = TDM_FAIL;
        }
    }

    return result;
}

/*!
 * \brief Assign ancillary bandwidth for pipe.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Ancillary assignment successfully passed.
 * \retval TDM_FAIL Ancillary assignment failed.
 */
int
bcm56960_a0_tdm_ancl_parse(tdm_mod_t *tdm)
{
    int j, pipe_id;
    int *idb_cal_main, idb_cal_len, idb_cal_id;
    int *mmu_cal_main, mmu_cal_len, mmu_cal_id;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    switch (pipe_id) {
        case 0:
            idb_cal_id = TH_IDB_PIPE_0_CAL_ID;
            mmu_cal_id = TH_MMU_PIPE_0_CAL_ID;
            break;
        case 1:
            idb_cal_id = TH_IDB_PIPE_1_CAL_ID;
            mmu_cal_id = TH_MMU_PIPE_1_CAL_ID;
            break;
        case 2:
            idb_cal_id = TH_IDB_PIPE_2_CAL_ID;
            mmu_cal_id = TH_MMU_PIPE_2_CAL_ID;
            break;
        case 3:
            idb_cal_id = TH_IDB_PIPE_3_CAL_ID;
            mmu_cal_id = TH_MMU_PIPE_3_CAL_ID;
            break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n", pipe_id);
            return TDM_FAIL;
    }

    idb_cal_len = bcmtm_tdm_lr_cal_len_get(tdm, idb_cal_id, 0);
    idb_cal_main = bcmtm_tdm_lr_cal_get(tdm, idb_cal_id, 0);
    mmu_cal_len = bcmtm_tdm_lr_cal_len_get(tdm, mmu_cal_id, 0);
    mmu_cal_main = bcmtm_tdm_lr_cal_get(tdm, mmu_cal_id, 0);
    if (idb_cal_len > 0 && idb_cal_main != NULL &&
        mmu_cal_len > 0 && mmu_cal_main != NULL) {
        /* Generate MMU calendar by copying from IDB calendar. */
        for (j = 0; j < idb_cal_len && j < mmu_cal_len; j++) {
            mmu_cal_main[j] = idb_cal_main[j];
        }

        /* Parse ancillary slots in IDB TDM calendar. */
        if (bcm56960_a0_tdm_ancl_cal_parse(tdm, idb_cal_id) != TDM_PASS) {
            return TDM_FAIL;
        }
        bcm56960_a0_tdm_lr_cal_print(tdm, idb_cal_id);

        /* Parse ancillary slots in MMU TDM calendar. */
        if (bcm56960_a0_tdm_ancl_cal_parse(tdm, mmu_cal_id) != TDM_PASS) {
            return TDM_FAIL;
        }
        bcm56960_a0_tdm_lr_cal_print(tdm, mmu_cal_id);
    }

    return bcm56960_a0_tdm_os_grp_gen(tdm);
}
