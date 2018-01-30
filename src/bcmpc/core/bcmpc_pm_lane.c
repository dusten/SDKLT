/*! \file bcmpc_pm_lane.c
 *
 * BCMPC PM Lane APIs
 *
 * This file implements the functions for configuring the lanes within a PM.
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

#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PM

/*! Tx and Rx lane profile IDs. */
typedef struct pm_lane_prof_id_s {

    /*! Tx lane profile ID. */
    int tx;

    /*! Rx lane profile ID. */
    int rx;

} pm_lane_prof_id_t;

/*!
 * \brief Tx lane profile entry.
 *
 * This stucture is the context for the traverse function pm_tx_lane_hw_update
 * which will be invoked by bcmpc_pm_tx_lane_prof_internal_update().
 */
typedef struct pm_tx_lane_prof_entry_s {

    /*! Profile ID. */
    int prof_id;

    /*! Tx lane profile. */
    bcmpc_pm_tx_lane_prof_t *prof;

} pm_tx_lane_prof_entry_t;

/*!
 * \brief Rx lane profile entry.
 *
 * This stucture is the context for the traverse function pm_rx_lane_hw_update
 * which will be invoked by bcmpc_pm_rx_lane_prof_internal_update().
 */
typedef struct pm_rx_lane_prof_entry_s {

    /*! Profile ID. */
    int prof_id;

    /*! Tx lane profile. */
    bcmpc_pm_rx_lane_prof_t *prof;

} pm_rx_lane_prof_entry_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the operating mode of the given PM lane.
 *
 * This function will try to find the operating mode of the port which the given
 * PM lane belongs to.
 *
 * BCMPC_PORT_OPMODE_INVALID will be returned when fail to find the port
 * operating mode from the IMM database.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_lane PM lane.
 *
 * \return The operation mode for the PM lane.
 */
static bcmpc_port_opmode_t
pm_lane_opmode_get(int unit, bcmpc_pm_lane_t *pm_lane)
{
    int rv, pm_port;
    bcmpc_pm_info_t pm_info;
    bcmpc_pm_mode_t pm_mode;
    bcmpc_lport_t pport;
    bcmpc_lport_t lport;
    bcmpc_port_cfg_t pcfg;

    if (SHR_FAILURE(bcmpc_pm_info_get(unit, pm_lane->pm_id, &pm_info))) {
        return BCMPC_PORT_OPMODE_INVALID;
    }

    if (SHR_FAILURE(bcmpc_pm_mode_get(unit, pm_lane->pm_id, &pm_mode))) {
        return BCMPC_PORT_OPMODE_INVALID;
    }

    /* Find the PM port of the lane. */
    for (pm_port = 0; pm_port < pm_info.prop.num_lanes; pm_port++) {
        if (pm_mode.lane_map[pm_port] & (1 << pm_lane->core_lane)) {
            break;
        }
    }

    pport = pm_info.base_pport + pm_port;
    lport = bcmpc_pport_to_lport(unit, pport);
    if (lport == BCMPC_INVALID_LPORT) {
        return BCMPC_PORT_OPMODE_INVALID;
    }

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (SHR_FAILURE(rv)) {
        return BCMPC_PORT_OPMODE_INVALID;
    }

    return pcfg.opmode;
}

/*!
 * \brief Get the Tx and Rx lane profile ID for the given operating mode.
 *
 * This function is used to get the Tx and Rx lane profile IDs in the lane
 * configuration for the given operating mode.
 *
 * \c primary is the profile IDs that is specified for the \c opmode; when no
 * exactly matched profile ID for the given opmode, then the profile ID for
 * BCMPC_PORT_OPMODE_ANY will be assigned to \c secondary.
 *
 * \param [in] unit Unit number.
 * \param [in] lcfg Lane configuration.
 * \param [in] opmode Operating mode.
 * \param [out] priamry Primary Tx and Rx profile IDs.
 * \param [out] secondary Secondary Tx and Rx profile IDs.
 */
static void
pm_lane_prof_id_get(int unit, bcmpc_pm_lane_cfg_t *lcfg,
                    bcmpc_port_opmode_t opmode,
                    pm_lane_prof_id_t *primary, pm_lane_prof_id_t *secondary)
{
    int idx;

    primary->tx = BCMPC_INVALID_PROF_ID;
    primary->rx = BCMPC_INVALID_PROF_ID;
    secondary->tx = BCMPC_INVALID_PROF_ID;
    secondary->rx = BCMPC_INVALID_PROF_ID;

    for (idx = 0; idx < COUNTOF(lcfg->port_opmode); idx++) {
        if (lcfg->port_opmode[idx] == opmode) {
            primary->tx = lcfg->tx_prof_id[idx];
            primary->rx = lcfg->rx_prof_id[idx];
        } else if (lcfg->port_opmode[idx] == BCMPC_PORT_OPMODE_ANY) {
            secondary->tx = lcfg->tx_prof_id[idx];
            secondary->rx = lcfg->rx_prof_id[idx];
        }

        if (primary->tx != BCMPC_INVALID_PROF_ID &&
            primary->rx != BCMPC_INVALID_PROF_ID) {
            break;
        }
    }
}

/*!
 * \brief Update the Tx lane configuration to hardware.
 *
 * This is the traverse callback function which is used in
 * \ref bcmpc_pm_tx_lane_prof_internal_update.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_pm_lane_t).
 * \param [in] key The lane index, bcmpc_pm_lane_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_pm_lane_cfg_t).
 * \param [in] data The lane configuration, bcmpc_pm_lane_cfg_t.
 * \param [in] cookie Tx lane profile entry, pm_tx_lane_prof_entry_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pm_tx_lane_hw_update(int unit, size_t key_size, void *key,
                     size_t data_size, void *data,
                     void *cookie)
{
    bcmpc_pm_lane_t *pm_lane = key;
    bcmpc_pm_lane_cfg_t *lcfg = data;
    pm_tx_lane_prof_entry_t *tx_prof = cookie;
    bcmpc_port_opmode_t opmode;
    pm_lane_prof_id_t prof_id, candidate_id;

    SHR_FUNC_ENTER(unit);

    opmode = pm_lane_opmode_get(unit, pm_lane);
    pm_lane_prof_id_get(unit, lcfg, opmode, &prof_id, &candidate_id);

    /*
     * Update the hardware if the Tx lane profile is currently used.
     */
    if (prof_id.tx != BCMPC_INVALID_PROF_ID) {
        if (tx_prof->prof_id == prof_id.tx) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_tx_lane_cfg_set(unit, pm_lane, tx_prof->prof));
        }
    } else {
        if (tx_prof->prof_id == candidate_id.tx) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_tx_lane_cfg_set(unit, pm_lane, tx_prof->prof));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the Rx lane configuration to hardware.
 *
 * This is the traverse callback function which is used in
 * \ref bcmpc_pm_rx_lane_prof_internal_update.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_pm_lane_t).
 * \param [in] key The lane index, bcmpc_pm_lane_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_pm_lane_cfg_t).
 * \param [in] data The lane configuration, bcmpc_pm_lane_cfg_t.
 * \param [in] cookie Rx lane profile entry, pm_rx_lane_prof_entry_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pm_rx_lane_hw_update(int unit, size_t key_size, void *key,
                     size_t data_size, void *data,
                     void *cookie)
{
    bcmpc_pm_lane_t *pm_lane = key;
    bcmpc_pm_lane_cfg_t *lcfg = data;
    pm_rx_lane_prof_entry_t *rx_prof = cookie;
    bcmpc_port_opmode_t opmode;
    pm_lane_prof_id_t prof_id, candidate_id;

    SHR_FUNC_ENTER(unit);

    opmode = pm_lane_opmode_get(unit, pm_lane);
    pm_lane_prof_id_get(unit, lcfg, opmode, &prof_id, &candidate_id);

    /*
     * Update the hardware if the Rx lane profile is currently used.
     */
    if (prof_id.rx != BCMPC_INVALID_PROF_ID) {
        if (rx_prof->prof_id == prof_id.rx) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_rx_lane_cfg_set(unit, pm_lane, rx_prof->prof));
        }
    } else {
        if (rx_prof->prof_id == candidate_id.rx) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_rx_lane_cfg_set(unit, pm_lane, rx_prof->prof));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Internal Public functions
 */

void
bcmpc_pm_tx_lane_prof_t_init(bcmpc_pm_tx_lane_prof_t *prof)
{
    sal_memset(prof, 0, sizeof(*prof));
}

void
bcmpc_pm_rx_lane_prof_t_init(bcmpc_pm_rx_lane_prof_t *prof)
{
    sal_memset(prof, 0, sizeof(*prof));
}

void
bcmpc_pm_lane_cfg_t_init(bcmpc_pm_lane_cfg_t *lcfg)
{
    int i;

    for (i = 0; i < COUNTOF(lcfg->port_opmode); i++) {
        lcfg->port_opmode[i] = BCMPC_PORT_OPMODE_INVALID;
        lcfg->tx_prof_id[i] = BCMPC_INVALID_PROF_ID;
        lcfg->rx_prof_id[i] = BCMPC_INVALID_PROF_ID;
    }
}

void
bcmpc_pm_lane_status_t_init(bcmpc_pm_lane_status_t *st)
{
    bcmpc_pm_tx_lane_prof_t_init(&st->tx);
    bcmpc_pm_rx_lane_prof_t_init(&st->rx);
}

int
bcmpc_pm_lane_status_get(int unit, bcmpc_pm_lane_t *pm_lane,
                         bcmpc_pm_lane_status_t *st)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_lane_status_get(unit, pm_lane, st));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_tx_lane_prof_get(int unit, bcmpc_pm_lane_t *pm_lane,
                          bcmpc_port_opmode_t opmode,
                          bcmpc_pm_tx_lane_prof_t *tx)
{
    int tx_prof_id;
    pm_lane_prof_id_t prof_id, candidate_id;
    bcmpc_pm_lane_cfg_t lcfg;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_lane_cfg_t_init(&lcfg);
    bcmpc_pm_tx_lane_prof_t_init(tx);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PM_LANEt, P(pm_lane), P(&lcfg)));

    pm_lane_prof_id_get(unit, &lcfg, opmode, &prof_id, &candidate_id);

    if (prof_id.tx != BCMPC_INVALID_PROF_ID) {
        tx_prof_id = prof_id.tx;
    } else if (candidate_id.tx != BCMPC_INVALID_PROF_ID) {
        tx_prof_id = candidate_id.tx;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PM_TX_LANE_PROFILEt,
                                   P(&tx_prof_id), P(tx)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_rx_lane_prof_get(int unit, bcmpc_pm_lane_t *pm_lane,
                          bcmpc_port_opmode_t opmode,
                          bcmpc_pm_rx_lane_prof_t *rx)
{
    int rx_prof_id;
    pm_lane_prof_id_t prof_id, candidate_id;
    bcmpc_pm_lane_cfg_t lcfg;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_lane_cfg_t_init(&lcfg);
    bcmpc_pm_rx_lane_prof_t_init(rx);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PM_LANEt, P(pm_lane), P(&lcfg)));

    pm_lane_prof_id_get(unit, &lcfg, opmode, &prof_id, &candidate_id);

    if (prof_id.rx != BCMPC_INVALID_PROF_ID) {
        rx_prof_id = prof_id.rx;
    } else if (candidate_id.rx != BCMPC_INVALID_PROF_ID) {
        rx_prof_id = candidate_id.rx;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PM_RX_LANE_PROFILEt,
                                   P(&rx_prof_id), P(rx)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_lane_internal_update(int unit, bcmpc_pm_lane_t *pm_lane,
                              bcmpc_pm_lane_cfg_t *lcfg)
{
    int rv;
    pm_lane_prof_id_t selected_id;
    pm_lane_prof_id_t prof_id, candidate_id;
    pm_lane_prof_id_t prof_id_cur, candidate_id_cur;
    bcmpc_port_opmode_t opmode;
    bcmpc_pm_tx_lane_prof_t tx_prof;
    bcmpc_pm_rx_lane_prof_t rx_prof;
    bcmpc_pm_lane_cfg_t lcfg_cur;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_lane_cfg_t_init(&lcfg_cur);
    bcmpc_db_imm_entry_lookup(unit, PC_PM_LANEt, P(pm_lane), P(&lcfg_cur));
    opmode = pm_lane_opmode_get(unit, pm_lane);
    selected_id.tx = BCMPC_INVALID_PROF_ID;
    selected_id.rx = BCMPC_INVALID_PROF_ID;

    /* Get the active profile ID in the current and the new lane config. */
    pm_lane_prof_id_get(unit, &lcfg_cur, opmode, &prof_id_cur,
                        &candidate_id_cur);
    pm_lane_prof_id_get(unit, lcfg, opmode, &prof_id, &candidate_id);

    /* Check if the active Tx profile is changed. */
    if (prof_id.tx != BCMPC_INVALID_PROF_ID) {
        if (prof_id.tx != prof_id_cur.tx) {
            selected_id.tx = prof_id.tx;
        }
    } else {
        if (candidate_id.tx != candidate_id_cur.tx) {
            selected_id.tx = candidate_id.tx;
        }
    }

    /* Check if the active Rx profile is changed. */
    if (prof_id.rx != BCMPC_INVALID_PROF_ID) {
        if (prof_id.rx != prof_id_cur.rx) {
            selected_id.rx = prof_id.rx;
        }
    } else {
        if (candidate_id.rx != candidate_id_cur.rx) {
            selected_id.rx = candidate_id.rx;
        }
    }

    /*
     * Update the hardware if we change the profile ID that we are currently
     * using.
     */
    if (selected_id.tx != BCMPC_INVALID_PROF_ID) {
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PM_TX_LANE_PROFILEt,
                                       P(&selected_id.tx), P(&tx_prof));
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_tx_lane_cfg_set(unit, pm_lane, &tx_prof));
        }
    }

    if (selected_id.rx != BCMPC_INVALID_PROF_ID) {
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PM_RX_LANE_PROFILEt,
                                       P(&selected_id.rx), P(&rx_prof));
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_rx_lane_cfg_set(unit, pm_lane, &rx_prof));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_tx_lane_prof_internal_update(int unit, int prof_id,
                                      bcmpc_pm_tx_lane_prof_t *prof)
{
    pm_tx_lane_prof_entry_t tx_prof;

    SHR_FUNC_ENTER(unit);

    tx_prof.prof_id = prof_id;
    tx_prof.prof = prof;
    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_traverse(unit,
                                     PC_PM_LANEt,
                                     sizeof(bcmpc_pm_lane_t),
                                     sizeof(bcmpc_pm_lane_cfg_t),
                                     pm_tx_lane_hw_update,
                                     &tx_prof));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_rx_lane_prof_internal_update(int unit, int prof_id,
                                      bcmpc_pm_rx_lane_prof_t *prof)
{
    pm_rx_lane_prof_entry_t rx_prof;

    SHR_FUNC_ENTER(unit);

    rx_prof.prof_id = prof_id;
    rx_prof.prof = prof;
    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_traverse(unit,
                                     PC_PM_LANEt,
                                     sizeof(bcmpc_pm_lane_t),
                                     sizeof(bcmpc_pm_lane_cfg_t),
                                     pm_rx_lane_hw_update,
                                     &rx_prof));

exit:
    SHR_FUNC_EXIT();
}
