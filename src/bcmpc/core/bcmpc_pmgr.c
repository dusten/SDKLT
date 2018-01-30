/*! \file bcmpc_pmgr.c
 *
 * BCMPC Port Manager (PMGR) APIs
 *
 * The Port Manager is responsible for updating the hardware based on the
 * changes to the logical tables. The Port Manager does not write to hardware
 * directly, but relies on the PMGR drivers.
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_drv.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_tm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/*!
 * \brief Get and check the port manager driver is null.
 *
 * Get and check the PMGR driver is null. If the PMGR driver, \c _drv, is NULL,
 * then exit the function.
 *
 * \param [in] _u Unit number.
 * \param [in] _p Device physical port.
 * \param [out] _drv Pmgr driver pointer.
 */
#define PMGR_DRV_GET(_u, _p, _drv) \
    do { \
        _drv = pmgr_drv_get(_u, _p); \
        if (_drv == NULL) { \
            SHR_RETURN_VAL_EXIT(SHR_E_NONE); \
        } \
    } while (0)

/*!
 * \brief Cached link state.
 *
 * This structure is used to detect the link transition and trigger the link
 * up/down procedure when linkscan is disabled or under forced link mode.
 *
 * A port will be in forced link mode when it is configured as MAC/PHY loopback
 * or disabled. When change to the forced link mode, PC is responsible to
 * trigger the link transition no matter linkscan is enabled or disabled.
 *
 * The \c forced_link means the port is in forced link mode or not.
 *
 * When forced_link is set, \c live_link shows the forced link state. e.g.
 * forced_link = 1 and live_link = 0 indicates the port is forced link down.
 *
 * When forced_link is clear, \c live_link means the current link state which
 * is got from the PHY.
 *
 * As for \c latched_link_down, it is used to trigger a link transition to down
 * when we update any port settings.
 */
typedef struct pmgr_link_state_s {

    /*! Forced link. */
    bool forced_enable;

    /*! Live link state. */
    bool live_link;

    /*! Latched link-down state. */
    bool latched_link_down;

} pmgr_link_state_t;


/*! Software chaced link state. */
static pmgr_link_state_t
pmgr_link_state[BCMPC_NUM_UNITS_MAX][BCMPC_NUM_PPORTS_PER_CHIP_MAX];

/*! Utilities for accessing pmgr_link_state array. */
#define PMGR_LINK_STATE_OBJ(_u, _p) pmgr_link_state[_u][_p]
#define PMGR_LINK_FORCED(_u, _p) pmgr_link_state[_u][_p].forced_enable
#define PMGR_LINK_CACHED_STATE(_u, _p) pmgr_link_state[_u][_p].live_link
#define PMGR_LINK_LATCHED_DOWN(_u, _p) pmgr_link_state[_u][_p].latched_link_down


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the port manager driver pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 *
 * \return Pointer of a port manager driver.
 */
static bcmpc_pmgr_drv_t*
pmgr_drv_get(int unit, bcmpc_pport_t pport)
{
    int rv;
    bcmpc_drv_t *pc_drv = NULL;

    /*
     * Suppose that a port is not within a real hardware
     * PortMacro if the PMAC driver is not specified.
     * For such kind of ports, e.g. CMIC port, we do not call
     * PMGR driver for the further hardware configuration.
     */
    rv = bcmpc_drv_get(unit, &pc_drv);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }
    if (pc_drv->pmac_drv_get(unit, pport) == NULL) {
        return NULL;
    }

    /* Otherwise, we use the default PMGR driver. */
    return &bcmpc_pmgr_drv_default;
}

/*!
 * \brief Execute the PHY operation.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] op PHY operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_phy_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_op_exec, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->phy_op_exec(unit, pport, op));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Execute the PortMAC operation.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] op PortMAC operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_pmac_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_port_cfg_t pcfg;
    bcmpc_lport_t lport;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pmac_op_exec, SHR_E_NO_HANDLER);

    if (sal_strcmp(op->name, "speed_set") == 0 &&
        op->param == BCMPC_OP_PARAM_NONE) {
        /* No speed value is specified, to sync MAC speed with PHY. */
        bcmpc_port_cfg_t_init(&pcfg);
        lport = bcmpc_pport_to_lport(unit, pport);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)),
             SHR_E_NOT_FOUND);

        op->param = bcmpc_pmgr_speed_get(unit, pport, pcfg.opmode);
    }

    SHR_IF_ERR_EXIT
        (pmgr_drv->pmac_op_exec(unit, pport, op));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Execute the port config profile.
 *
 * \ref bcmpc_cfg_profile_t.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] prof Config profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_profile_exec(int unit, bcmpc_pport_t pport, bcmpc_cfg_profile_t *prof)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_operation_t *op;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    for (i = 0; i < prof->op_cnt; i++) {
        op = &prof->ops[i];

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Execute operation: drv[%s], op[%s], prm[%d]\n"),
             op->drv, op->name, op->param));

        if (sal_strcmp(op->drv, "phy") == 0) {
            SHR_IF_ERR_EXIT
                (pmgr_phy_op_exec(unit, pport, op));
        } else if (sal_strcmp(op->drv, "pmac") == 0) {
            SHR_IF_ERR_EXIT
                (pmgr_pmac_op_exec(unit, pport, op));
        } else if (sal_strcmp(op->drv, "tm") == 0) {
            SHR_IF_ERR_EXIT
                (bcmpc_tm_op_exec(unit, pport, op));
        } else {
            SHR_IF_ERR_EXIT
                (pc_drv->op_exec(unit, pport, op));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the forced link state according to the port configuration.
 *
 * A port will be forced link up under the following circumstances.
 * - A port is in MAC loopback no matter the port is enabled or disabled.
 * - A port is in PHY loopback and enabled.
 *
 * \param [in] unit Unit number.
 * \param [in] pcfg Port configuration.
 *
 * \return true for forced link up, otherwise false.
 */
static bool
pmgr_forced_link_up_check(int unit, bcmpc_port_cfg_t *pcfg)
{
    /* Return link up for MAC loopback. */
    if (pcfg->lpbk_mode == BCMPC_PORT_LPBK_MAC) {
        return true;
    }

    /* Return link up for PHY local loopback when the port is enabled. */
    if ((pcfg->enable == 1) &&
        (pcfg->lpbk_mode == BCMPC_PORT_LPBK_PHY ||
         pcfg->lpbk_mode == BCMPC_PORT_LPBK_PHY_PCS ||
         pcfg->lpbk_mode == BCMPC_PORT_LPBK_PHY_PMD)) {
        return true;
    }

    return false;
}

/*!
 * \brief Force a port to be link up or down.
 *
 * This function will be called when we set a port entering or exiting the
 * forced link mode.
 *
 * It will trigger the link transition procedure only when we set a port into
 * the forced link mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [in] forced Set for enable forced mode, otherwise disable.
 * \param [in] up Set for forced up, otherwise forced down.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_forced_link_update(int unit, bcmpc_pport_t pport, bool forced, bool up)
{
    pmgr_link_state_t backup;

    SHR_FUNC_ENTER(unit);

    sal_memcpy(&backup, &PMGR_LINK_STATE_OBJ(unit, pport), sizeof(backup));

    PMGR_LINK_FORCED(unit, pport) = forced;

    if (forced) {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_port_link_change(unit, pport, up));
    }

exit:
    if (SHR_FUNC_ERR()) {
        sal_memcpy(&PMGR_LINK_STATE_OBJ(unit, pport), &backup, sizeof(backup));
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the lane settings for a logical port.
 *
 * This function will update the lane settings according to the current port
 * operating mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] opmode Port operating mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_port_lcfg_update(int unit, bcmpc_pport_t pport,
                      bcmpc_port_opmode_t opmode)
{
    int rv, start_lane, num_lanes, li;
    int pm_id, lanes_per_core, lane_idx;
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    bcmpc_pm_tx_lane_prof_t tx_prof;
    bcmpc_pm_rx_lane_prof_t rx_prof;
    bcmpc_pm_lane_t pm_lane;

    bcmpc_pm_lport_rsrc_t prsrc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_lport_rsrc_get(unit, pport, &prsrc));

    lanes_per_core = pm_prop->num_lanes / pm_prop->num_aggr;
    SHR_BIT_ITER(prsrc.lbmp, BCMPC_NUM_LANES_PER_PM_MAX, start_lane) {
        break;
    }
    SHR_BITCOUNT_RANGE(prsrc.lbmp, num_lanes, 0, sizeof(prsrc.lbmp));

    pm_lane.pm_id = pm_id;
    for (li = 0; li < num_lanes; li++) {
        lane_idx = start_lane + li;
        pm_lane.core_idx = lane_idx / lanes_per_core;
        pm_lane.core_lane = lane_idx - (pm_lane.core_idx * lanes_per_core);

        /* Get the Tx lane config and commit to Hardware. */
        rv = bcmpc_pm_tx_lane_prof_get(unit, &pm_lane, opmode, &tx_prof);
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_tx_lane_cfg_set(unit, &pm_lane, &tx_prof));
        }

        /* Get the Rx lane config and commit to Hardware. */
        rv = bcmpc_pm_rx_lane_prof_get(unit, &pm_lane, opmode, &rx_prof);
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_rx_lane_cfg_set(unit, &pm_lane, &rx_prof));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the PFC settings for the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] prof_id PFC profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_port_pfc_update(int unit, bcmpc_pport_t pport, int prof_id)
{
    int rv;
    bcmpc_pfc_prof_t prof;

    SHR_FUNC_ENTER(unit);

    bcmpc_pfc_prof_t_init(&prof);

    if (prof_id != BCMPC_INVALID_PROF_ID) {
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PFC_PROFILEt,
                                       P(&prof_id), P(&prof));
        /* Do nothing if no profile is found. */
        if (rv == SHR_E_NOT_FOUND) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else {
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pfc_set(unit, pport, &prof));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the cached link state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] link link state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static void
pmgr_cached_link_set(int unit, bcmpc_pport_t pport, int link)
{
    PMGR_LINK_LATCHED_DOWN(unit, pport) = 0;
    PMGR_LINK_CACHED_STATE(unit, pport) = link;
}


/*******************************************************************************
 * Public functions
 */

void
bcmpc_pmgr_port_status_t_init(bcmpc_pmgr_port_status_t *st)
{
    sal_memset(st, 0, sizeof(*st));
}

int
bcmpc_pmgr_pm_port_enable(int unit, bcmpc_pport_t pport, int enable)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pm_port_enable, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->pm_port_enable(unit, pport, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_cfg_set(int unit, bcmpc_pport_t pport,
                        bcmpc_lport_t lport, bcmpc_port_cfg_t *cfg)
{
    int rv;
    bool is_new = false, is_forced_up = false;
    uint64_t lport_u64 = lport;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_port_cfg_t cfg_old;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_cfg_set, SHR_E_NO_HANDLER);

    /* We should not touch the hardware if the opmode is not specified. */
    if (cfg->opmode == BCMPC_PORT_OPMODE_INVALID) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Do nothing when no config changed. */
    bcmpc_port_cfg_t_init(&cfg_old);
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&cfg_old));
    is_new = SHR_FAILURE(rv);
    if (!is_new && sal_memcmp(cfg, &cfg_old, sizeof(*cfg)) == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Update the configuration of the lanes which are used by the port. */
    if (cfg->opmode != cfg_old.opmode) {
        SHR_IF_ERR_EXIT
            (pmgr_port_lcfg_update(unit, pport, cfg->opmode));
    }

    is_forced_up = pmgr_forced_link_up_check(unit, cfg);

    /* Any port configuration change will cause a link down. */
    if (is_forced_up) {
        /*
         * For forced link mode, PC is responsible for triggering the link
         * transition procedure.
         *
         * For forecd link-up case, we force the link DOWN first, update the
         * port configuration to hardware, and then force the link be UP.
         */
        SHR_IF_ERR_EXIT
            (pmgr_forced_link_update(unit, pport, 1, 0));
    } else {
        /*
         * For normal mode, PC needs to set the latched-down flag, in order to
         * make bcmpc_pmgr_link_state_get() reflect the latched-down state
         * when it is called.
         */
        PMGR_LINK_LATCHED_DOWN(unit, pport) = 1;
    }

    /*
     * Update the port configuration to hardware.
     */
    if (cfg->pfc_profile_id != cfg_old.pfc_profile_id) {
        SHR_IF_ERR_EXIT
            (pmgr_port_pfc_update(unit, pport, cfg->pfc_profile_id));
    }
    SHR_IF_ERR_EXIT
        (pmgr_drv->port_cfg_set(unit, pport, lport, cfg));

    if (is_forced_up) {
        /* Force the link up after the port config is updated. */
        SHR_IF_ERR_EXIT
            (pmgr_forced_link_update(unit, pport, 1, 1));
    }

    /*
     * Send out port change events if required.
     *
     * bcmpcEvPortAdd: Add a port.
     * bcmpcEvLinkForceUp: Put a port into loopback mode.
     * bcmpcEvLinkForceDown: Disable a port.
     * bcmpcEvLinkForceNone: Enable a port or bring it out of loopback mode.
     */
    if (is_new) {
        bcmevm_publish_event_notify(unit, "bcmpcEvPortAdd", lport_u64);
    } else if (cfg->enable == cfg_old.enable &&
               cfg->lpbk_mode == cfg_old.lpbk_mode) {
        /* No event-related config changed. */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (is_forced_up) {
        bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceUp", lport_u64);
    } else if (cfg->enable == 0) {
        SHR_IF_ERR_EXIT
            (pmgr_forced_link_update(unit, pport, 1, 0));
        bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceDown", lport_u64);
    } else {
        SHR_IF_ERR_EXIT
            (pmgr_forced_link_update(unit, pport, 0, 0));
        bcmevm_publish_event_notify(unit, "bcmpcEvLinkForceNone", lport_u64);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_port_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_port_status_t *st)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->port_status_get, SHR_E_NO_HANDLER);

    sal_memset(st, 0, sizeof(*st));

    /* Get the status from hardware. */
    SHR_IF_ERR_EXIT
        (pmgr_drv->port_status_get(unit, pport, st));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_link_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_link_status_t *st)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_pmgr_drv_t *pmgr_drv;
    int cur_link, cached_link = 0;
    bcmpc_lport_t lport;
    bcmpc_ls_mode_t linkscan_mode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_link_state_get, SHR_E_NO_HANDLER);
    SHR_NULL_CHECK
        (pmgr_drv->phy_link_info_get, SHR_E_NO_HANDLER);
    SHR_NULL_CHECK
        (pmgr_drv->fault_state_get, SHR_E_NO_HANDLER);

    sal_memset(st, 0, sizeof(*st));
    cached_link = PMGR_LINK_CACHED_STATE(unit, pport);

    lport = bcmpc_pport_to_lport(unit, pport);
    linkscan_mode = bcmpc_linkscan_mode_get(unit, lport);

    SHR_IF_ERR_EXIT
        (pmgr_drv->fault_state_get(unit, pport, st));
    SHR_IF_ERR_EXIT
        (pmgr_drv->phy_link_info_get(unit, pport, st));

    /*
     * Note that:
     *
     * There must be only one component to read the PHY link state, because
     * normally the standard link_get() function in the PHY driver gets the
     * latched status and it is usually clear-on-read. If more than one entity
     * tries to read the PHY link state, we may miss a temporary link-down
     * event.
     */

    /*
     * When linkscan is enabled or the port is in forced link mode, returns
     * the cached state directly.
     *
     * - For the linkscan mode enabled case, LM will read the hardware state and
     *   update to PC.
     * - For the forced link mode case, the cached state will equal to the
     *   forced state.
     */
    if (linkscan_mode != BCMPC_LS_MODE_DISABLED ||
        PMGR_LINK_FORCED(unit, pport)) {
        if (cached_link) {
            BCMPC_PMGR_LINK_ST_SET(*st, BCMPC_PMGR_LINK_EVENT_LINK);
        }

        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /*
     * If linkscan is disabled, PC will get the link state from hardware and
     * triggers link up/down procedure when the link state is changed.
     */
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_link_state_get(unit, pport, &cur_link));
    if (cur_link) {
        BCMPC_PMGR_LINK_ST_SET(*st, BCMPC_PMGR_LINK_EVENT_LINK);
    }
    if (cur_link != cached_link) {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_port_link_change(unit, pport, cur_link));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_link_state_get(int unit, bcmpc_pport_t pport, int *link)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pmgr_link_status_t st = 0;

    SHR_FUNC_ENTER(unit);

    *link = 0;

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_link_state_get, SHR_E_NO_HANDLER);
    SHR_NULL_CHECK
        (pmgr_drv->fault_state_get, SHR_E_NO_HANDLER);

    /*
     * When the latched link-down is set, return link DOWN and then clear
     * the latched state.
     */
    if (PMGR_LINK_LATCHED_DOWN(unit, pport) == 1) {
        PMGR_LINK_LATCHED_DOWN(unit, pport) = 0;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Get the link status from hardware. */
    SHR_IF_ERR_EXIT
        (pmgr_drv->phy_link_state_get(unit, pport, &st));
    SHR_IF_ERR_EXIT
        (pmgr_drv->fault_state_get(unit, pport, &st));

    /* Consider PHY link state and MAC fault state. */
    if (BCMPC_PMGR_LINK_ST_GET(st, BCMPC_PMGR_LINK_EVENT_LINK) &&
        !BCMPC_PMGR_LINK_ST_GET(st, BCMPC_PMGR_LINK_EVENT_LOCAL_FAULT) &&
        !BCMPC_PMGR_LINK_ST_GET(st, BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT)) {
        *link = 1;
    }

exit:
    if (!SHR_FUNC_ERR() && !PMGR_LINK_FORCED(unit, pport)) {
        /* Update the link state cache. */
        PMGR_LINK_CACHED_STATE(unit, pport) = *link;
    }

    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_fault_state_get(int unit, bcmpc_pport_t pport,
                           int *rmt_fault)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_pmgr_link_status_t st = 0;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->fault_state_get, SHR_E_NO_HANDLER);

    /* Get the state from hardware. */
    SHR_IF_ERR_EXIT
        (pmgr_drv->fault_state_get(unit, pport, &st));
    *rmt_fault = BCMPC_PMGR_LINK_ST_GET(st, BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_core_mode_set(int unit, bcmpc_pport_t pport,
                         bcmpc_pm_mode_t *mode)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->core_mode_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->core_mode_set(unit, pport, mode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_phy_init(int unit, bcmpc_pport_t pport,
                    bcmpc_pm_core_cfg_t *ccfg, int do_core_init)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->phy_init, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->phy_init(unit, pport, ccfg, do_core_init));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_lane_status_get(int unit, bcmpc_pm_lane_t *pm_lane,
                           bcmpc_pm_lane_status_t *st)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_get(unit, pm_lane->pm_id, &pm_info));

    PMGR_DRV_GET(unit, pm_info.base_pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->lane_status_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->lane_status_get(unit, pm_lane, st));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_tx_lane_cfg_set(int unit, bcmpc_pm_lane_t *pm_lane,
                           bcmpc_pm_tx_lane_prof_t *tx_prof)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_get(unit, pm_lane->pm_id, &pm_info));

    PMGR_DRV_GET(unit, pm_info.base_pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->tx_lane_cfg_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->tx_lane_cfg_set(unit, pm_lane, tx_prof));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_rx_lane_cfg_set(int unit, bcmpc_pm_lane_t *pm_lane,
                           bcmpc_pm_rx_lane_prof_t *rx_prof)
{
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_get(unit, pm_lane->pm_id, &pm_info));

    PMGR_DRV_GET(unit, pm_info.base_pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->rx_lane_cfg_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->rx_lane_cfg_set(unit, pm_lane, rx_prof));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_speed_get(int unit, bcmpc_pport_t pport,
                     bcmpc_port_opmode_t opmode_cfg)
{
    int rv = SHR_E_NONE;
    uint32_t speed_cur = 0;
    bcmpc_pmgr_port_status_t pst;
    bcmpc_port_opmode_t opmode_cur = opmode_cfg;

    if (!PMGR_LINK_CACHED_STATE(unit, pport)) {
        return 0;
    }

    if (bcmdrd_feature_is_real_hw(unit)) {
        rv = bcmpc_pmgr_port_status_get(unit, pport, &pst);
        if (SHR_SUCCESS(rv)) {
            opmode_cur = pst.opmode;
        }
    }

    rv = bcmpc_port_opmode_to_speed(unit, opmode_cur, &speed_cur);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    return speed_cur;
}

int
bcmpc_pmgr_port_link_change(int unit, bcmpc_pport_t pport, int link)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_pmgr_drv_t *pmgr_drv;
    bcmpc_cfg_profile_t *prof;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Link is changed to %s\n"),
         link ? "UP" : "DOWN"));

    /* Always trust the link state from the caller and cache it. */
    pmgr_cached_link_set(unit, pport, link);

    

    if (link == 0) {
        prof = pc_drv->port_down_prof_get(unit, pport);
    } else {
        prof = pc_drv->port_up_prof_get(unit, pport);
    }

    if (!prof) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    
    SHR_IF_ERR_EXIT
        (bcmpc_tm_port_link_update(unit, pport, link));

    SHR_IF_ERR_EXIT
        (pmgr_profile_exec(unit, pport, prof));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_loopback_get(int unit, bcmpc_pport_t pport,
                                 uint32_t *failover_loopback)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_loopback_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->failover_loopback_get(unit, pport, failover_loopback));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_enable_set(int unit, bcmpc_pport_t pport,
                               uint32_t enable)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_enable_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->failover_enable_set(unit, pport, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_enable_get(int unit, bcmpc_pport_t pport,
                               uint32_t *enable)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_enable_get, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->failover_enable_get(unit, pport, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_failover_loopback_remove(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->failover_loopback_remove, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->failover_loopback_remove(unit, pport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pfc_set(int unit, bcmpc_pport_t pport,
                   bcmpc_pfc_prof_t *pfc_prof)
{
    bcmpc_pmgr_drv_t *pmgr_drv;

    SHR_FUNC_ENTER(unit);

    PMGR_DRV_GET(unit, pport, pmgr_drv);

    SHR_NULL_CHECK
        (pmgr_drv->pfc_set, SHR_E_NO_HANDLER);

    SHR_IF_ERR_EXIT
        (pmgr_drv->pfc_set(unit, pport, pfc_prof));

exit:
    SHR_FUNC_EXIT();
}
