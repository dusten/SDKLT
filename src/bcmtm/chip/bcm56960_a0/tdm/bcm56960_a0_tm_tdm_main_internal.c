/*! \file bcm56960_a0_tm_tdm_main_internal.c
 *
 * TDM chip-specific main sequences.
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
 * \brief Get port range of the given calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 * \param [out] port_lo Lowest physical port number.
 * \param [out] port_hi Highest physical port number.
 *
 * \return Nothing.
 */
void
bcm56960_a0_tdm_port_range_get(tdm_mod_t *tdm, int cal_id,
                               int *port_lo, int *port_hi)
{
    int prt_lo = 0, prt_hi = 0;

    switch (cal_id) {
        case 0:
            prt_lo = TH_CAL_0_PORT_LO;
            prt_hi = TH_CAL_0_PORT_HI;
            break;
        case 1:
            prt_lo = TH_CAL_1_PORT_LO;
            prt_hi = TH_CAL_1_PORT_HI;
            break;
        case 2:
            prt_lo = TH_CAL_2_PORT_LO;
            prt_hi = TH_CAL_2_PORT_HI;
            break;
        case 3:
            prt_lo = TH_CAL_3_PORT_LO;
            prt_hi = TH_CAL_3_PORT_HI;
            break;
        case 4:
            prt_lo = TH_CAL_4_PORT_LO;
            prt_hi = TH_CAL_4_PORT_HI;
            break;
        case 5:
            prt_lo = TH_CAL_5_PORT_LO;
            prt_hi = TH_CAL_5_PORT_HI;
            break;
        case 6:
            prt_lo = TH_CAL_6_PORT_LO;
            prt_hi = TH_CAL_6_PORT_HI;
            break;
        case 7:
            prt_lo = TH_CAL_7_PORT_LO;
            prt_hi = TH_CAL_7_PORT_HI;
            break;
        default:
            break;
    }
    if (port_lo != NULL) {
        *port_lo = prt_lo;
    }
    if (port_hi != NULL) {
        *port_hi = prt_hi;
    }
}

/*!
 * \brief Check if the given calendar is higig.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval TDM_TRUE Calendar is higig.
 * \retval TDM_FALSE Calendar is not higig.
 */
static int
bcm56960_a0_tdm_pipe_higig_chk(tdm_mod_t *tdm, int cal_id)
{
    int port, port_lo, port_hi;
    int result = TDM_FALSE;

    /* check regular ports */
    bcm56960_a0_tdm_port_range_get(tdm, cal_id, &port_lo, &port_hi);
    for (port = port_lo; port <= port_hi; port++) {
        if (bcmtm_tdm_port_hg_chk(tdm, port, 0) == TDM_TRUE)
            return TDM_FALSE;
    }

    return result;
}

/*!
 * \brief Get limited linerate calendar length of the given calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \retval Calendar length.
 *
 * Calendar length depends on frequency and higig property.
 */
static int
bcm56960_a0_tdm_cal_len_limit_get(tdm_mod_t *tdm, int cal_id)
{
    int cal_len = 0, cal_len_limit, hg_en;
    int clk_freq;

    clk_freq = bcmtm_tdm_cfg_freq_get(tdm);

    /* Get calendar length. */
    hg_en = bcm56960_a0_tdm_pipe_higig_chk(tdm, cal_id);
    switch (clk_freq) {
        case TH_CLK_950MHZ:
            cal_len = (hg_en == TDM_TRUE) ? TH_LEN_950MHZ_HG:
                                            TH_LEN_950MHZ_EN;
            break;
        case TH_CLK_850MHZ:
            cal_len = (hg_en == TDM_TRUE) ? TH_LEN_850MHZ_HG:
                                            TH_LEN_850MHZ_EN;
            break;
        case TH_CLK_765MHZ:
            cal_len = (hg_en == TDM_TRUE) ? TH_LEN_765MHZ_HG:
                                            TH_LEN_765MHZ_EN;
            break;
        case TH_CLK_672MHZ:
            cal_len = (hg_en == TDM_TRUE) ? TH_LEN_672MHZ_HG:
                                            TH_LEN_672MHZ_EN;
            break;
        case TH_CLK_645MHZ:
            cal_len = (hg_en == TDM_TRUE) ? TH_LEN_645MHZ_HG:
                                            TH_LEN_645MHZ_EN;
            break;
        case TH_CLK_545MHZ:
            cal_len = (hg_en == TDM_TRUE) ? TH_LEN_545MHZ_HG:
                                            TH_LEN_545MHZ_EN;
            break;
        default:
            break;
    }

    /* Check validity of calendar length. */
    if (cal_len == 0) {
        TDM_ERROR1("Invalid core clock frequency %0d\n", clk_freq);
    } else {
        cal_len_limit = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
        if (cal_len > cal_len_limit && cal_len_limit > 0) {
            TDM_ERROR4("%s, pipe %0d, len %0d, limit %0d (overflow)\n",
                       "Invalid calendar length ",
                       cal_id, cal_len, cal_len_limit);
            cal_len = 0;
        }
    }

    return cal_len;
}

/*!
 * \brief Initialize TDM chip-specific parameters.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM chip-specific init successfully completed.
 * \retval TDM_FAIL TDM chip-specific init failed.
 */
static int
bcm56960_a0_tdm_main_init(tdm_mod_t *tdm)
{
    int cal_id, port, pm;

    /* Initialize chip variables. */
    tdm->chip_data.cfg.fp_port_lo = 1;
    tdm->chip_data.cfg.fp_port_hi = TH_NUM_PHY_PORTS;
    tdm->chip_data.cfg.num_pipes = TH_NUM_PIPE;
    tdm->chip_data.cfg.num_pms = TH_NUM_PHY_PM;
    tdm->chip_data.cfg.num_pm_lanes = TH_NUM_PM_LNS;
    tdm->chip_data.cfg.min_space_same = TH_MIN_SPACING_SAME_PORT;
    tdm->chip_data.cfg.min_space_sister = TH_MIN_SPACING_SISTER_PORT;
    tdm->chip_data.token.token_ancl = TH_ANCL_TOKEN;
    tdm->chip_data.token.token_empty = TH_NUM_EXT_PORTS;
    tdm->chip_data.token.token_ovsb = TH_OVSB_TOKEN;
    tdm->chip_data.token.token_cmic = TH_CMIC_TOKEN;
    tdm->chip_data.token.token_idl1 = TH_IDL1_TOKEN;
    tdm->chip_data.token.token_idl2 = TH_IDL2_TOKEN;
    tdm->chip_data.token.token_null = TH_NULL_TOKEN;
    for (port = 0; port <= TH_NUM_EXT_PORTS; port++) {
        pm = (port >= 1 && port <= TH_NUM_PHY_PORTS) ?
             ((port - 1) / TH_NUM_PM_LNS) : TH_NUM_PHY_PM;
        bcmtm_tdm_port_pm_set(tdm, port, pm);
    }

    for (cal_id = 0; cal_id < TDM_MAX_NUM_CALS; cal_id++) {
        tdm->user_data.cals[cal_id].cal_en = 1;
        tdm->user_data.cals[cal_id].lr.cal_en = 1;
        tdm->user_data.cals[cal_id].lr.cal_len =
                bcm56960_a0_tdm_cal_len_limit_get(tdm, cal_id);
        tdm->user_data.cals[cal_id].ovsb.cal_en = 1;
        tdm->user_data.cals[cal_id].ovsb.grp_num = TH_OS_VBS_GRP_NUM;
        tdm->user_data.cals[cal_id].ovsb.grp_len = TH_OS_VBS_GRP_LEN;
    }

/* for backward compatibility */
{
    int grp, pos, token_empty;
    token_empty = tdm->chip_data.token.token_empty;
    for (cal_id = 0; cal_id < TDM_MAX_NUM_CALS; cal_id++) {
        for (pos = 0; pos < TDM_MAX_CAL_LEN; pos++) {
            tdm->user_data.cals[cal_id].lr.cal[pos] = token_empty;
        }
        for (grp = 0; grp < TDM_MAX_GRP_WID; grp++) {
            for (pos = 0; pos < TDM_MAX_GRP_LEN; pos++) {
                tdm->user_data.cals[cal_id].ovsb.cal[grp][pos] = token_empty;
            }
        }
    }
}

    return TDM_PASS;
}

/*!
 * \brief Request TDM core execution.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM core execution successfully completed.
 * \retval TDM_FAIL TDM core execution failed.
 */
static int
bcm56960_a0_tdm_main_corereq(tdm_mod_t *tdm)
{
    return bcmtm_tdm_core_init(tdm);
}

/*!
 * \brief Generate TDM calendars for the given pipe.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] pipe_id, Pipe ID.
 *
 * \retval TDM_PASS TDM calendar generation successfully completed.
 * \retval TDM_FAIL TDM calendar generation failed.
 *
 * All ingress, egress and mmu calendars belonging to the same pipe
 * will be generated.
 */
static int
bcm56960_a0_tdm_main_pipe_cal_gen(tdm_mod_t *tdm, int pipe_id)
{
    int port_lo = 0, port_hi = 0;
    int cal_len, lr_limit, ancl_limit;

    bcmtm_tdm_pipe_config_print(tdm);

    ancl_limit = TH_NUM_ANCL;
    cal_len = bcm56960_a0_tdm_cal_len_limit_get(tdm, pipe_id);
    lr_limit = cal_len - ancl_limit;
    bcm56960_a0_tdm_port_range_get(tdm, pipe_id, &port_lo, &port_hi);

    /* Set pipe parameters */
    tdm->core_data.cfg.slot_unit = TDM_SLOT_UNIT_2P5G;
    tdm->core_data.cfg.pipe_id = pipe_id;
    tdm->core_data.cfg.pipe_port_lo = port_lo;
    tdm->core_data.cfg.pipe_port_hi = port_hi;
    tdm->core_data.cfg.num_lr_limit = lr_limit;
    tdm->core_data.cfg.num_ancl_limit= ancl_limit;

    return bcm56960_a0_tdm_main_corereq(tdm);
}

/*!
 * \brief Generate TDM calendars for all pipes.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM calendar generation successfully completed.
 * \retval TDM_FAIL TDM calendar generation failed.
 *
 * TDM config validity checking (if needed) will be done here.
 */
static int
bcm56960_a0_tdm_main_cal_gen(tdm_mod_t *tdm)
{
    int pipe_id, result = TDM_PASS;
    int param_num_pipes;

    param_num_pipes = bcmtm_tdm_num_pipes_get(tdm);
    for (pipe_id = 0; pipe_id < param_num_pipes; pipe_id++) {
        if (bcm56960_a0_tdm_main_pipe_cal_gen(tdm, pipe_id) != TDM_PASS) {
            result = TDM_FAIL;
            TDM_ERROR1("Failed to generate calendars for pipe %0d\n",
                       pipe_id);
        }
    }

    return result;
}

/*!
 * \brief Transcribe chip-specific port speeds and states.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM transcription passed.
 * \retval TDM_FAIL TDM transcription failed.
 *
 * TDM config validity checking (if needed) will be done here.
 */
static int
bcm56960_a0_tdm_main_port_transcribe(tdm_mod_t *tdm)
{
    /* Print port config after transcription. */
    bcmtm_tdm_cfg_print(tdm);

    return TDM_PASS;
}

/*!
 * \brief TDM chip-specific entry function.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM calendar generated correctly.
 * \retval TDM_FAIL TDM calendar generated incorrectly or incompletely.
 */
static int
bcm56960_a0_tdm_main_start(tdm_mod_t *tdm)
{
    /* Initialize chip/core parameters. */
    if (bcm56960_a0_tdm_main_init(tdm) != TDM_PASS) {
        return TDM_FAIL;
    }

    /* Transcribe ports. */
    if (bcm56960_a0_tdm_main_port_transcribe(tdm) != TDM_PASS) {
        return TDM_FAIL;
    }

    /* Generate calendars. */
    if (bcm56960_a0_tdm_main_cal_gen(tdm) != TDM_PASS) {
        return TDM_FAIL;
    }

    /* Check calendar validity. */
    if (bcmtm_tdm_cfg_chk_en_get(tdm) == TDM_TRUE) {
        if (bcm56960_a0_tdm_chk(tdm) != TDM_PASS) {
            return TDM_FAIL;
        }
    }

    return TDM_PASS;
}

/*!
 * \brief Initialize TDM core execution functions.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM driver init sequence completed successfully.
 * \retval TDM_FAIL TDM driver init sequence failed.
 */
static int
bcm56960_a0_tdm_drv_init(tdm_mod_t **tdm)
{
    int drv_idx;
    tdm_mod_t *tdmp = NULL;

    if (tdm == NULL) {
        return TDM_FAIL;
    }

    tdmp = TDM_ALLOC(sizeof(tdm_mod_t), "TDM constructor");
    if (!tdmp) {
        TDM_ERROR0("Failed to allocate tdm resource.\n");
        return TDM_FAIL;
    } else {
        TDM_MEMSET(tdmp, 0, sizeof(tdm_mod_t));
        for (drv_idx = 0; drv_idx < TDM_DRV_SIZE; drv_idx++) {
            switch (drv_idx) {
                case TDM_DRV_RUN:
                    tdmp->core_drv[drv_idx] = &bcm56960_a0_tdm_main_start;
                    break;
                case TDM_CORE_ALLOC:
                    tdmp->core_drv[drv_idx] = &bcm56960_a0_tdm_lr_cal_gen;
                    break;
                default:
                    tdmp->core_drv[drv_idx] = &bcmtm_tdm_core_null;
                    break;
            }
        }
        (*tdm) = tdmp;
    }

    return TDM_PASS;
}

/********************************************************************
 * Internal driver functions.
 */
/*!
 * \brief Attach TDM chip-specific driver functions.
 *
 * \param [in] tdm Pointer of TDM driver structure.
 *
 * \retval TDM_PASS TDM driver attachment completed successfully.
 * \retval TDM_FAIL TDM driver attachment failed.
 */
int
bcm56960_a0_tdm_drv_attach(int unit, bcmtm_tdm_drv_t *drv)
{
    if (drv != NULL) {
        drv->tdm_init = &bcm56960_a0_tdm_drv_init;
        drv->tdm_cfg = &bcmtm_tdm_drv_cfg;
        drv->tdm_run = &bcm56960_a0_tdm_main_start;
        drv->tdm_free = &bcmtm_tdm_drv_free;

        return TDM_PASS;
    }

    return -1;
}
