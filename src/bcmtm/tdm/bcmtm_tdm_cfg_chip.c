/*! \file bcmtm_tdm_cfg_chip.c
 *
 * TDM core functions for chip configuration access.
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
 * Internal functions.
 */
/*!
 * \brief Get the lowest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest front panel port.
 */
int
bcmtm_tdm_fport_lo_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.fp_port_lo;
}

/*!
 * \brief Get the highest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest front panel port.
 */
int
bcmtm_tdm_fport_hi_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.fp_port_hi;
}

/*!
 * \brief Get number of pipes.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of pipes.
 */
int
bcmtm_tdm_num_pipes_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.num_pipes;
}

/*!
 * \brief Get number of PMs.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of PMs.
 */
int
bcmtm_tdm_num_pms_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.num_pms;
}

/*!
 * \brief Get number of lanes per PM.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of lanes per PM.
 */
int
bcmtm_tdm_num_pm_lanes_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.num_pm_lanes;
}

/*!
 * \brief Get same port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Same port spacing.
 */
int
bcmtm_tdm_min_same_space_get(tdm_mod_t *tdm, int port)
{
    /* For device with diff sister port spaces, retrieve space limit
     * which is pre-determined by chip side.
     */
    return tdm->chip_data.cfg.min_space_same;
}

/*!
 * \brief Get sister port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Sister port spacing.
 */
int
bcmtm_tdm_min_sister_space_get(tdm_mod_t *tdm, int port)
{
    /* For device with diff sister port spaces, retrieve space limit
     * which is pre-determined by chip side.
     */
    return tdm->chip_data.cfg.min_space_sister;
}

/*!
 * \brief Get port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Port PM number.
 */
int
bcmtm_tdm_port_pm_get(tdm_mod_t *tdm, int port)
{
    return tdm->chip_data.cfg.port2pm_map[port % TDM_MAX_NUM_GPORTS];
}

/*!
 * \brief Set port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_port_pm_set(tdm_mod_t *tdm, int port, int pm)
{
    if (port < TDM_MAX_NUM_GPORTS) {
        if (pm < TDM_MAX_NUM_PMS) {
            tdm->chip_data.cfg.port2pm_map[port] = pm;
        } else {
            TDM_ERROR1("[port_pm_set] Invalid PM number %0d\n", pm);
        }
    } else {
        TDM_ERROR1("[port_pm_set] Invalid port number %0d\n", port);
    }
}



/*!
 * \brief Get empty port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Empty port token.
 */
int
bcmtm_tdm_token_empty_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_empty;
}

/*!
 * \brief Get oversub port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Oversub port token.
 */
int
bcmtm_tdm_token_ovsb_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_ovsb;
}

/*!
 * \brief Get ancilary port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Ancilary port token.
 */
int
bcmtm_tdm_token_ancl_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_ancl;
}

/*!
 * \brief Get idle-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle-1 port token.
 */
int
tdm_cmn_get_token_idl1(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_idl1;
}

/*!
 * \brief Get idle-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle2 port token.
 */
int
tdm_cmn_get_token_idl2(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_idl2;
}

/*!
 * \brief Get idle port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle port token.
 */
int
tdm_cmn_get_token_idle(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_idle;
}

/*!
 * \brief Get opportunistic-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-1 port token.
 */
int
tdm_cmn_get_token_opp1(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_opp1;
}

/*!
 * \brief Get opportunistic-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-2 port token.
 */
int
tdm_cmn_get_token_opp2(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_opp2;
}

/*!
 * \brief Get null port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval null port token.
 */
int
tdm_cmn_get_token_null(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_null;
}
