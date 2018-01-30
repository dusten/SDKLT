/*! \file bcmpc_pmgr_drv_default.c
 *
 * This file implements the default port manager driver.
 *
 * The port manager API will use port manager driver instances to achieve the
 * hardware configuration of PHY and PMAC.
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
#include <shr/shr_util.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_drv.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

#define PMGR_CFG_UNDO_NUM   (10)

/*!
 * \brief Check if the port config is changed.
 *
 * \param [in] _force Forcedly treat as config changed.
 * \param [in] _pcfg1 The first pcfg variable.
 * \param [in] _pcfg2 The second pcfg variable.
 * \param [in] _f field name.
 *
 * retval 1 The port configuration is changed.
 * retval 0 The port configuration is same.
 */
#define PMGR_CFG_CHANGED(_force, _pcfg1, _pcfg2, _f) \
            ((_force) || (!BCMPC_FIELD_VAL_EQUAL(_pcfg1, _pcfg2, _f)))

/*!
 * \brief Error-exit on PHYMOD operation error.
 *
 * Evaluate an expression of PHYMOD and if it evaluates to a PHYMOD error
 * code, then log a error message and go to the function's single
 * point of exit with a SHR error, SHR_E_FAIL.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * \param [in] _expr Expression to evaluate.
 */
#define PHY_IF_ERR_EXIT(_expr) \
    do {   \
        int phymod_err = _expr; \
        if (phymod_err != PHYMOD_E_NONE) { \
            LOG_WARN(BSL_LOG_MODULE, \
                     (BSL_META_U(_func_unit, "PHYMOD failure[%d]\n"), \
                      phymod_err)); \
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL); \
        } \
    } while (0)

/*! Port configuration handler. */
typedef int
(*pmgr_pcfg_attr_op_f)(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                       bcmpc_port_cfg_t *pcfg);

/*! Phy operation handler function. */
typedef int
(*pmgr_phy_op_f)(int unit, phymod_phy_access_t *phy_acc, uint32_t param);

/*! This structure is used to define the operation for the PHY. */
typedef struct pmgr_phy_op_hdl_s {

    /*! Operation name. */
    char *name;

    /*! Operation handler. */
    pmgr_phy_op_f func;

} pmgr_phy_op_hdl_t;


/*! Port control flags. */
#define PMGR_CTRL_F_MAC (1 << 0)
#define PMGR_CTRL_F_PHY (1 << 1)


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the PHYMOD dispatch type.
 *
 * Get the PHYMOD driver for the given phyiscal port.
 *
 * \param [in] unit Unit number.
 * \param [in] pc_drv PC driver.
 * \param [in] pport Physical device port.
 *
 * \return The phymod driver for the port.
 */
static phymod_dispatch_type_t
port_phy_drv_get(int unit, bcmpc_drv_t *pc_drv, bcmpc_pport_t pport)
{
    if (!bcmdrd_feature_is_real_hw(unit)) {
        return phymodDispatchTypeNull;
    }

    return pc_drv->phy_drv_get(unit, pport);
}

/*!
 * \brief Get BCMPC driver information.
 *
 * To get the PHY/PMAC driver information via BCMPC driver object, and fill the
 * information to \ref bcmpc_pmgr_drv_data_t structure.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [out] drv_data Pmgr driver data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL BCMPC driver object is not assigned.
 */
static int
port_drv_data_get(int unit, bcmpc_pport_t pport,
                  bcmpc_pmgr_drv_data_t *drv_data)
{
    bcmpc_drv_t *pc_drv;
    bcmpmac_access_t *pmac_acc;
    phymod_access_t *pm_acc;
    bcmpc_pm_info_t pm_info;
    bcmpc_pm_lport_rsrc_t prsrc;
    int pm_id, lane;

    SHR_FUNC_ENTER(unit);

    sal_memset(drv_data, 0, sizeof(*drv_data));
    bcmpc_pm_lport_rsrc_t_init(&prsrc);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    SHR_IF_ERR_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_pm_lport_rsrc_get(unit, pport, &prsrc),
         SHR_E_NOT_FOUND);

    /* Save the device and PM physical port number. */
    drv_data->pport = pport;
    drv_data->pm_pport = prsrc.pm_pport;
    drv_data->max_speed = prsrc.max_speed;

    /* Setup pmac_drv and pmac_acc. */
    drv_data->pmac_data.unit = unit;
    drv_data->pmac_data.blk_id = pc_drv->blk_id_get(unit, pport);
    drv_data->pmac_drv = pc_drv->pmac_drv_get(unit, pport);
    SHR_NULL_CHECK(drv_data->pmac_drv, SHR_E_INTERNAL);
    pmac_acc = &(drv_data->pmac_acc);
    PMAC_ACC_USER_ACC(pmac_acc) = &(drv_data->pmac_data);
    PMAC_ACC_BUS(pmac_acc) = pc_drv->pmac_bus_get(unit, pport);

    /* Setup phy_core_acc. */
    drv_data->phy_data.unit = unit;
    drv_data->phy_core_acc.type = port_phy_drv_get(unit, pc_drv, pport);
    drv_data->phy_core_lanes = pm_info.prop.num_lanes;

    pm_acc = &drv_data->phy_core_acc.access;
    PHYMOD_ACC_USER_ACC(pm_acc) = &(drv_data->phy_data);
    PHYMOD_ACC_BUS(pm_acc) = pc_drv->phy_bus_get(unit, pport);
    PHYMOD_ACC_ADDR(pm_acc) = pc_drv->phy_addr_get(unit, pport);

    /* Setup phy_acc. */
    drv_data->phy_acc.type = drv_data->phy_core_acc.type;

    pm_acc = &drv_data->phy_acc.access;
    sal_memcpy(pm_acc, &drv_data->phy_core_acc.access,
               sizeof(phymod_access_t));
    SHR_BIT_ITER(prsrc.lbmp, BCMPC_NUM_LANES_PER_PM_MAX, lane) {
        PHYMOD_ACC_LANE_MASK(pm_acc) |= 1 << lane;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * PHY operation handlers
 */

/*!
 * \brief Set PHY enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] enable 1 to enable, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
phy_enable_set(int unit, phymod_phy_access_t *phy_acc, uint32_t enable)
{
    phymod_phy_tx_lane_control_t tx_ctrl;
    phymod_phy_rx_lane_control_t rx_ctrl;

    SHR_FUNC_ENTER(unit);

    if (enable) {
        tx_ctrl = phymodTxSquelchOff;
        rx_ctrl = phymodRxSquelchOff;
    } else {
        tx_ctrl = phymodTxSquelchOn;
        rx_ctrl = phymodRxSquelchOn;
    }

    PHY_IF_ERR_EXIT
        (phymod_phy_tx_lane_control_set(phy_acc, tx_ctrl));
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_lane_control_set(phy_acc, rx_ctrl));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get PHY enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] enable 1 to enable, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
phy_enable_get(int unit, phymod_phy_access_t *phy_acc, uint32_t *enable)
{
    phymod_phy_tx_lane_control_t tx_ctrl;
    phymod_phy_rx_lane_control_t rx_ctrl;

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_tx_lane_control_get(phy_acc, &tx_ctrl));
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_lane_control_get(phy_acc, &rx_ctrl));

    if (tx_ctrl == phymodTxSquelchOn && rx_ctrl == phymodRxSquelchOn) {
        *enable = 0;
    } else {
        *enable = 1;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Private functions for handling the fields of bcmpc_port_cfg_t
 */

/*!
 * \brief Get the PHY and MAC enable state of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_data Driver information.
 * \param [in] flag The target of the operation, PMGR_CTRL_F_XXX.
 * \param [out] enable 1 for enabled, 0 for disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_enable_get(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                int flag, uint32_t *enable)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    phymod_phy_access_t *phy_acc;
    bcmpmac_access_t *pmac_acc;
    uint32_t mac_reset;

    SHR_FUNC_ENTER(unit);

    phy_acc = &drv_data->phy_acc;
    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    if (!flag) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "flag is not specified\n")));
        *enable = 0;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (flag & PMGR_CTRL_F_PHY) {
        SHR_IF_ERR_EXIT
            (phy_enable_get(unit, phy_acc, enable));

        if (*enable == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    if (flag & PMGR_CTRL_F_MAC) {
        SHR_IF_ERR_EXIT
            (pmacd->mac_reset_get(pmac_acc, pm_pport, &mac_reset));

        if (mac_reset) {
            *enable = 0;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }

        SHR_IF_ERR_EXIT
            (pmacd->rx_enable_get(pmac_acc, pm_pport, enable));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the PHY and MAC enable state of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_data Driver information.
 * \param [in] flag The target of the operation, PMGR_CTRL_F_XXX.
 * \param [in] enable 1 to enable, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_enable_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                int flag, int enable)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    phymod_phy_access_t *phy_acc;
    bcmpmac_access_t *pmac_acc;
    uint32_t failover_en = 0;

    SHR_FUNC_ENTER(unit);

    phy_acc = &drv_data->phy_acc;
    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    if (flag & PMGR_CTRL_F_MAC) {
        /* If the LAG failover is enabled, always keep the MAC enabled. */
        SHR_IF_ERR_EXIT
            (pmacd->failover_enable_get(pmac_acc, pm_pport, &failover_en));
        enable = failover_en ? 1 : enable;

        SHR_IF_ERR_EXIT
            (pmacd->rx_enable_set(pmac_acc, pm_pport, enable == 1 ? 1 : 0));
        SHR_IF_ERR_EXIT
            (pmacd->tx_enable_set(pmac_acc, pm_pport, enable == 1 ? 1 : 0));
        SHR_IF_ERR_EXIT
            (pmacd->mac_reset_set(pmac_acc, pm_pport, enable == 1 ? 0 : 1));
    }

    if (flag & PMGR_CTRL_F_PHY) {
        SHR_IF_ERR_EXIT
            (phy_enable_set(unit, phy_acc, enable));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update administrative enable.
 *
 * A port must be enabled in order to forward packets. If a port is not enabled,
 * the driver will attempt to bring down any active link on the port, usually by
 * powering down the PHY. If no PHY is associated with the port, the MAC will be
 * disabled. An administratively disabled port may be brought partially up in
 * order to allow packet loopback on the port.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_admin_enable_update(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                         bcmpc_port_cfg_t *pcfg)
{
    int flag;
    phymod_phy_access_t *phy_acc;

    SHR_FUNC_ENTER(unit);

    phy_acc = &drv_data->phy_acc;

    /*
     * Port is administrative enabled.
     */
    if (pcfg->enable) {
        /* Enable the PHY of the port or enable the MAC for a PHY-less port. */
        if (PHYMOD_ACC_BUS(&phy_acc->access)) {
            flag = PMGR_CTRL_F_PHY;
        } else {
            flag = PMGR_CTRL_F_MAC;
        }

        SHR_RETURN_VAL_EXIT
            (port_enable_set(unit, drv_data, flag, 1));
    }

    /*
     * Port is administrative disabled.
     */
    /* When the port is configured to MAC loopback, to keep MAC enabled. */
    if (pcfg->lpbk_mode == BCMPC_PORT_LPBK_MAC) {
        SHR_IF_ERR_EXIT
            (port_enable_set(unit, drv_data, PMGR_CTRL_F_PHY, 0));
        SHR_RETURN_VAL_EXIT
            (port_enable_set(unit, drv_data, PMGR_CTRL_F_MAC, 1));
    }

    /* Otherwise, we need to disable both of MAC and PHY. */
    SHR_IF_ERR_EXIT
        (port_enable_set(unit, drv_data,
                         PMGR_CTRL_F_MAC | PMGR_CTRL_F_PHY, 0));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get loopback configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_data Driver information.
 * \param [out] lpbk_mode Loopback mode.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_lpbk_get(int unit, bcmpc_pmgr_drv_data_t *drv_data,
              bcmpc_port_lpbk_t *lpbk_mode)
{
    int pm_pport;
    uint32_t enable;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    phymod_phy_access_t *phy_acc;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;
    phy_acc = &drv_data->phy_acc;

    SHR_IF_ERR_EXIT
        (pmacd->lpbk_get(pmac_acc, pm_pport, &enable));
    if (enable) {
        *lpbk_mode = BCMPC_PORT_LPBK_MAC;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    PHY_IF_ERR_EXIT
        (phymod_phy_loopback_get(phy_acc, phymodLoopbackGlobal, &enable));
    if (enable) {
        *lpbk_mode = BCMPC_PORT_LPBK_PHY;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    PHY_IF_ERR_EXIT
        (phymod_phy_loopback_get(phy_acc, phymodLoopbackRemotePMD, &enable));
    if (enable) {
        *lpbk_mode = BCMPC_PORT_LPBK_REMOTE;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    *lpbk_mode = BCMPC_PORT_LPBK_NONE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set loopback configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_lpbk_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
              bcmpc_port_cfg_t *pcfg)
{
    int pm_pport;
    uint32_t en;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    phymod_phy_access_t *phy_acc;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;
    phy_acc = &drv_data->phy_acc;

    /* Set loopback configuration. */
    en = (pcfg->lpbk_mode == BCMPC_PORT_LPBK_MAC) ? 1 : 0;
    SHR_IF_ERR_EXIT
        (pmacd->lpbk_set(pmac_acc, pm_pport, en));

    en = (pcfg->lpbk_mode == BCMPC_PORT_LPBK_PHY) ? 1 : 0;
    PHY_IF_ERR_EXIT
        (phymod_phy_loopback_set(phy_acc, phymodLoopbackGlobal, en));

    en = (pcfg->lpbk_mode == BCMPC_PORT_LPBK_REMOTE) ? 1 : 0;
    PHY_IF_ERR_EXIT
        (phymod_phy_loopback_set(phy_acc, phymodLoopbackRemotePMD, en));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set autoneg operating mode.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_an_mode_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                 bcmpc_port_cfg_t *pcfg)
{
    phymod_phy_access_t *phy_acc;
    uint32_t lmask, an_completed;
    int num_lanes, is_hg = 0;
    bcmpc_autoneg_prof_t prof;
    phymod_autoneg_ability_t an_adv;
    phymod_autoneg_control_t an_ctrl;

    SHR_FUNC_ENTER(unit);

    phy_acc = &drv_data->phy_acc;
    phymod_autoneg_control_t_init(&an_ctrl);
    phymod_autoneg_ability_t_init(&an_adv);

    lmask = PHYMOD_ACC_LANE_MASK(&phy_acc->access);
    num_lanes = shr_util_popcount(lmask);

    if (pcfg->encap_mode == BCMPC_PORT_ENCAP_HIGIG ||
        pcfg->encap_mode == BCMPC_PORT_ENCAP_HIGIG2) {
        is_hg = 1;
    }

    /* Disable AN for Non-AUTONEG modes. */
    if (pcfg->opmode != BCMPC_PORT_OPMODE_AUTONEG) {
        PHY_IF_ERR_EXIT
            (phymod_phy_autoneg_get(phy_acc, &an_ctrl, &an_completed));

        an_ctrl.enable = 0;
        an_ctrl.num_lane_adv = num_lanes;
        PHY_IF_ERR_EXIT
            (phymod_phy_autoneg_set(phy_acc, &an_ctrl));

        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Get the Autoneg profile. */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_db_imm_entry_lookup(unit,
                                   PC_AUTONEG_PROFILEt,
                                   P(&pcfg->autoneg_profile_id), P(&prof)),
         SHR_E_NOT_FOUND);

    /* Do nothing if no profile is found. */
    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Convert the Autoneg profile to phymod values. */
    SHR_IF_ERR_EXIT
        (bcmpc_autoneg_prof_to_phy_adv(unit, num_lanes, is_hg, &prof, &an_adv));
    SHR_IF_ERR_EXIT
        (bcmpc_autoneg_prof_to_phy_ctrl(unit, &prof, &an_ctrl));

    /* Apply the settings and enable autoneg. */
    PHY_IF_ERR_EXIT
        (phymod_phy_autoneg_ability_set(phy_acc, &an_adv));

    an_ctrl.enable = 1;
    an_ctrl.num_lane_adv = num_lanes;
    PHY_IF_ERR_EXIT
        (phymod_phy_autoneg_set(phy_acc, &an_ctrl));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set forced operating mode.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_forced_mode_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                     bcmpc_port_cfg_t *pcfg)
{
    int pport;
    phymod_phy_access_t *phy_acc;
    phymod_phy_inf_config_t intf_cfg;
    uint32_t lmask;
    int num_lanes, is_hg = 0, ref_clk = 0;

    SHR_FUNC_ENTER(unit);

    pport = drv_data->pport;
    phy_acc = &drv_data->phy_acc;

    lmask = PHYMOD_ACC_LANE_MASK(&phy_acc->access);
    num_lanes = shr_util_popcount(lmask);

    if (pcfg->encap_mode == BCMPC_PORT_ENCAP_HIGIG ||
        pcfg->encap_mode == BCMPC_PORT_ENCAP_HIGIG2) {
        is_hg = 1;
    }

    /*
     * Configure the PHY here. The MAC settings will be sync-ed up when link up.
     */
    phymod_phy_inf_config_t_init(&intf_cfg);
    SHR_IF_ERR_EXIT
        (bcmpc_pm_core_ref_clk_get(unit, pport, &ref_clk));

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_opmode_to_phy(unit, pcfg, num_lanes, is_hg, ref_clk,
                                  &intf_cfg));
    PHY_IF_ERR_EXIT
        (phymod_phy_interface_config_set(phy_acc, 0, &intf_cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set operating mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_opmode_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                bcmpc_port_cfg_t *pcfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_an_mode_set(unit, drv_data, pcfg));

    switch (pcfg->opmode) {
        case BCMPC_PORT_OPMODE_INVALID:
        case BCMPC_PORT_OPMODE_AUTONEG:
            break;
        default:
            SHR_IF_ERR_EXIT
                (port_forced_mode_set(unit, drv_data, pcfg));
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get operating mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] lpbk_mode Loopback mode.
 * \param [out] opmode Port operating mode.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_opmode_get(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                bcmpc_port_lpbk_t lpbk_mode,
                bcmpc_port_opmode_t *opmode)
{
    int pm_pport, max_speed;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    phymod_phy_access_t *phy_acc;
    phymod_phy_inf_config_t intf_cfg;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    max_speed = drv_data->max_speed;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;
    phy_acc = &drv_data->phy_acc;
    phymod_phy_inf_config_t_init(&intf_cfg);

    PHY_IF_ERR_EXIT
        (phymod_phy_interface_config_get(phy_acc, 0, 0, &intf_cfg));

    /*
     * To get the speed from MAC when
     * 1. MAC loopback mode.
     * 2. The PHY interface is phymodInterfaceBypass i.e. NULL PHY.
     */
    if (lpbk_mode == BCMPC_PORT_LPBK_MAC ||
        intf_cfg.interface_type == phymodInterfaceBypass) {
        SHR_IF_ERR_EXIT
            (pmacd->speed_get(pmac_acc, pm_pport, &intf_cfg.data_rate));

        /* Return the maximum port speed when PMAC does not report the speed. */
        if (intf_cfg.data_rate == 0) {
            intf_cfg.data_rate = max_speed;
        }
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_opmode_from_phy(unit, &intf_cfg, opmode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get encap configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [out] encap_mode Encapsulation mode.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_encap_get(int unit, bcmpc_pmgr_drv_data_t *drv_data,
               bcmpc_port_encap_t *encap_mode)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    bcmpmac_encap_mode_t pmac_encap;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->encap_get(pmac_acc, pm_pport, &pmac_encap));
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_encap_mode_from_pmac(pmac_encap, encap_mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set encap configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_encap_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
               bcmpc_port_cfg_t *pcfg)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    bcmpmac_encap_mode_t pmac_encap;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_encap_mode_to_pmac(pcfg->encap_mode, &pmac_encap));
    SHR_IF_ERR_EXIT
        (pmacd->encap_set(pmac_acc, pm_pport, pmac_encap));

    /* Reconfigure other blocks: PGW/MMU/TDM/IP/EP. */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pause configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [out] pause_tx Tx pause ability.
 * \param [out] pause_rx Rx pause ability.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_get(int unit, bcmpc_pmgr_drv_data_t *drv_data,
               uint32_t *pause_tx, uint32_t *pause_rx)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    bcmpmac_pause_ctrl_t pause_ctrl;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->pause_get(pmac_acc, pm_pport, &pause_ctrl));
    *pause_tx = pause_ctrl.tx_enable;
    *pause_rx = pause_ctrl.rx_enable;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pause configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
               bcmpc_port_cfg_t *pcfg)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    bcmpmac_pause_ctrl_t pause_ctrl;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    pause_ctrl.tx_enable = pcfg->pause_tx;
    pause_ctrl.rx_enable = pcfg->pause_rx;

    SHR_IF_ERR_EXIT
        (pmacd->pause_set(pmac_acc, pm_pport, &pause_ctrl));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pause MAC address configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [out] pause_addr Pause MAC address.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_addr_get(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                    bcmpc_mac_t pause_addr)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->pause_addr_get(pmac_acc, pm_pport, pause_addr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pause MAC address configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_addr_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                    bcmpc_port_cfg_t *pcfg)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->pause_addr_set(pmac_acc, pm_pport, pcfg->pause_addr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get max frame size configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [out] max_frame_size Max frame size.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_frame_max_get(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                   uint32_t *max_frame_size)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->frame_max_get(pmac_acc, pm_pport, max_frame_size));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set max frame size configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_frame_max_set(int unit, bcmpc_pmgr_drv_data_t *drv_data,
                   bcmpc_port_cfg_t *pcfg)
{
    int pm_pport;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    pm_pport = drv_data->pm_pport;
    pmacd = drv_data->pmac_drv;
    pmac_acc = &drv_data->pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->frame_max_set(pmac_acc, pm_pport, pcfg->max_frame_size));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Undo port configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] backlog List of attribute config functions to execute.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_cfg_undo(int unit, pmgr_pcfg_attr_op_f *backlog,
              bcmpc_pmgr_drv_data_t *drv_data, bcmpc_port_cfg_t *pcfg)
{
    pmgr_pcfg_attr_op_f *attr_cfg = backlog;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (port_enable_set(unit, drv_data, PMGR_CTRL_F_MAC, 0));

    while (*attr_cfg) {
        SHR_IF_ERR_CONT
            ((*attr_cfg)(unit, drv_data, pcfg));
        attr_cfg++;
    }

    SHR_IF_ERR_CONT
        (port_admin_enable_update(unit, drv_data, pcfg));

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Pmgr driver functions
 */

static int
default_pm_port_enable(int unit, bcmpc_pport_t pport, int enable)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->port_init(pmac_acc, pm_pport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
default_port_cfg_set(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                     bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpc_port_cfg_t old_pcfg;
    int rv, lcnt = 0, force_cfg = 0, do_hardware_update = 0;
    pmgr_pcfg_attr_op_f backlog[PMGR_CFG_UNDO_NUM] = { 0 };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&old_pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        /*
         * SHR_E_NOT_FOUND means there is no previous configuration.
         * To set all attributes of port config.
         */
        force_cfg = 1;
    }

    /*
     * Do haradware configuration only when the port is,
     * 1. Administrative enable state changed.
     * 2. Administrative enabled.
     * 3. Administrative disabled but in MAC loopback.
     */
    if (PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, enable)) {
        do_hardware_update = 1;
        /* Forced sync all of the configuration to hardware. */
        force_cfg = 1;
    } else if (pcfg->enable) {
        do_hardware_update = 1;
    } else if (pcfg->lpbk_mode == BCMPC_PORT_LPBK_MAC) {
        do_hardware_update = 1;
    }

    if (!do_hardware_update) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /*
     * Start to update the hardware.
     */
    SHR_IF_ERR_EXIT
        (port_enable_set(unit, &drv_data, PMGR_CTRL_F_MAC, 0));

    if (force_cfg ||
        !BCMPC_ARRAY_EQUAL(pcfg->pause_addr, old_pcfg.pause_addr)) {
        SHR_IF_ERR_EXIT
            (port_pause_addr_set(unit, &drv_data, pcfg));
        backlog[lcnt++] = port_pause_addr_set;
    }

    if (PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, pause_tx) ||
        PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, pause_rx)) {
        SHR_IF_ERR_EXIT
            (port_pause_set(unit, &drv_data, pcfg));
        backlog[lcnt++] = port_pause_set;
    }

    if (PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, max_frame_size)) {
        SHR_IF_ERR_EXIT
            (port_frame_max_set(unit, &drv_data, pcfg));
        backlog[lcnt++] = port_frame_max_set;
    }

    if (PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, encap_mode)) {
        SHR_IF_ERR_EXIT
            (port_encap_set(unit, &drv_data, pcfg));
        backlog[lcnt++] = port_encap_set;
    }

    if (PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, opmode) ||
        PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, autoneg_profile_id)) {
        SHR_IF_ERR_EXIT
            (port_opmode_set(unit, &drv_data, pcfg));
        backlog[lcnt++] = port_opmode_set;
    }

    if (PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, lpbk_mode)) {
        SHR_IF_ERR_EXIT
            (port_lpbk_set(unit, &drv_data, pcfg));
        backlog[lcnt++] = port_lpbk_set;
    }

    SHR_IF_ERR_EXIT
        (port_admin_enable_update(unit, &drv_data, pcfg));

exit:
    /* Undo the change when something failed. */
    if (SHR_FUNC_ERR() && lcnt > 0) {
        port_cfg_undo(unit, backlog, &drv_data, &old_pcfg);
    }

    SHR_FUNC_EXIT();
}

static int
default_port_status_get(int unit, bcmpc_pport_t pport,
                        bcmpc_pmgr_port_status_t *pst)
{
    bcmpc_pmgr_drv_data_t drv_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    SHR_IF_ERR_EXIT
        (port_lpbk_get(unit, &drv_data, &pst->lpbk_mode));
    SHR_IF_ERR_EXIT
        (port_encap_get(unit, &drv_data, &pst->encap_mode));
    SHR_IF_ERR_EXIT
        (port_pause_get(unit, &drv_data, &pst->pause_tx, &pst->pause_rx));
    SHR_IF_ERR_EXIT
        (port_pause_addr_get(unit, &drv_data, pst->pause_addr));
    SHR_IF_ERR_EXIT
        (port_frame_max_get(unit, &drv_data, &pst->max_frame_size));
    SHR_IF_ERR_EXIT
        (port_opmode_get(unit, &drv_data, pst->lpbk_mode, &pst->opmode));
    SHR_IF_ERR_EXIT
        (port_enable_get(unit, &drv_data, PMGR_CTRL_F_MAC, &pst->mac_enabled));
    SHR_IF_ERR_EXIT
        (port_enable_get(unit, &drv_data, PMGR_CTRL_F_PHY, &pst->phy_enabled));

exit:
    SHR_FUNC_EXIT();
}

static int
default_phy_link_state_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_link_status_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    phymod_phy_access_t *phy_acc;
    uint32_t linked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    phy_acc = &drv_data.phy_acc;

    PHY_IF_ERR_EXIT
        (phymod_phy_link_status_get(phy_acc, &linked));

    if (linked) {
        BCMPC_PMGR_LINK_ST_SET(*st, BCMPC_PMGR_LINK_EVENT_LINK);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
default_fault_state_get(int unit, bcmpc_pport_t pport,
                        bcmpc_pmgr_link_status_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    int pm_pport;
    bcmpmac_fault_status_t fault_st = { 0 };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    /* Get MAC fault state. */
    SHR_IF_ERR_EXIT
        (pmacd->fault_status_get(pmac_acc, pm_pport, &fault_st));

    if (fault_st.local_fault) {
        BCMPC_PMGR_LINK_ST_SET(*st, BCMPC_PMGR_LINK_EVENT_LOCAL_FAULT);
    }

    if (fault_st.remote_fault) {
        BCMPC_PMGR_LINK_ST_SET(*st, BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
default_phy_link_info_get(int unit, bcmpc_pport_t pport,
                          bcmpc_pmgr_link_status_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    phymod_phy_access_t *phy_acc;
    uint32_t pmd_locked = 1, signal_detected = 1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    phy_acc = &drv_data.phy_acc;

    PHY_IF_ERR_EXIT
        (phymod_phy_rx_pmd_locked_get(phy_acc, &pmd_locked));

    
    /*
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_signal_detect_get(phy_acc, &signal_detected),
         PHYMOD_E_UNAVAIL);
    */

    if (!pmd_locked) {
        BCMPC_PMGR_LINK_ST_SET(*st, BCMPC_PMGR_LINK_EVENT_PMD_LINK_ERR);
    }

    if (!signal_detected) {
        BCMPC_PMGR_LINK_ST_SET(*st, BCMPC_PMGR_LINK_EVENT_NO_SIGNAL);
    }

    

exit:
    SHR_FUNC_EXIT();
}

static int
default_core_mode_set(int unit, bcmpc_pport_t pport,
                      bcmpc_pm_mode_t *mode)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    bcmpmac_core_port_mode_t pmac_mode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pm_mode_to_pmac(mode, &pmac_mode));

    SHR_IF_ERR_EXIT
        (pmacd->core_mode_set(pmac_acc, pm_pport, &pmac_mode));

exit:
    SHR_FUNC_EXIT();
}

static int
default_phy_init(int unit, bcmpc_pport_t pport,
                 bcmpc_pm_core_cfg_t *ccfg, int do_core_init)
{
    int core_lanes, phy_lanes, ref_clk = 0, max_speed, ln;
    uint32_t phy_lmask;
    bcmpc_pmgr_drv_data_t drv_data;
    phymod_phy_access_t *phy_acc;
    phymod_core_access_t *core_acc;
    phymod_core_init_config_t phymod_ccfg;
    phymod_core_status_t phymod_cst;
    phymod_phy_init_config_t phymod_phycfg;
    phymod_phy_inf_config_t intf_cfg;
    phymod_interface_t intf_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    core_acc = &drv_data.phy_core_acc;
    phy_acc = &drv_data.phy_acc;
    core_lanes = drv_data.phy_core_lanes;
    phy_lmask = PHYMOD_ACC_LANE_MASK(&phy_acc->access);
    phy_lanes = shr_util_popcount(phy_lmask);
    max_speed = drv_data.max_speed;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pm_core_cfg_to_phy(unit, ccfg, core_lanes, &phymod_ccfg));

    if (do_core_init == 1) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&phymod_ccfg);
        PHY_IF_ERR_EXIT
            (phymod_core_init(core_acc, &phymod_ccfg, &phymod_cst));
    }

    phymod_phy_init_config_t_init(&phymod_phycfg);
    for (ln = 0; ln < phy_lanes; ln++) {
        PHY_IF_ERR_EXIT
            (phymod_phy_media_type_tx_get(phy_acc, phymodMediaTypeChipToChip,
                                          &phymod_phycfg.tx[ln]));
    }
    sal_memcpy(&phymod_phycfg.polarity, &phymod_ccfg.polarity_map,
               sizeof(phymod_polarity_t));

    PHY_IF_ERR_EXIT
        (phymod_phy_init(phy_acc, &phymod_phycfg));

    /*
     * Initialize the PHY with the max port speed.
     *
     * The PHY settings will be updated later by port_opmode_set(), while we
     * need do phymod_phy_interface_config_set() once before we are trying to
     * enable the autoneg mode, otherwise we will fail to get linked under
     * autoneg mode.
 */
    phymod_phy_inf_config_t_init(&intf_cfg);
    SHR_IF_ERR_EXIT
        (bcmpc_pm_core_ref_clk_get(unit, pport, &ref_clk));
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_default_phymod_intf_get(max_speed, phy_lanes, 0, 0,
                                            &intf_type));

    intf_cfg.data_rate = max_speed;
    intf_cfg.interface_type = intf_type;
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_ref_clk_to_phy(ref_clk, &intf_cfg.ref_clock));

    PHY_IF_ERR_EXIT
        (phymod_phy_interface_config_set(phy_acc, 0, &intf_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
default_lane_status_get(int unit, bcmpc_pm_lane_t *pm_lane,
                        bcmpc_pm_lane_status_t *lane_st)
{
    bcmpc_drv_t *pc_drv;
    int pport, lanes_per_core, lane_idx;
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    phymod_phy_access_t phy_acc;
    phymod_access_t *acc;
    bcmpc_phy_access_data_t acc_data;
    phymod_tx_t phymod_tx;
    phymod_rx_t phymod_rx;
    bcmpc_pm_tx_lane_prof_t *lst_tx = &lane_st->tx;
    bcmpc_pm_rx_lane_prof_t *lst_rx = &lane_st->rx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_info_get(unit, pm_lane->pm_id, &pm_info));
    pport = pm_info.base_pport;
    lanes_per_core = pm_prop->num_lanes / pm_prop->num_aggr;
    lane_idx = pm_lane->core_idx * lanes_per_core + pm_lane->core_lane;

    /* Setup phymod_access_t. */
    phymod_phy_access_t_init(&phy_acc);
    phy_acc.type = port_phy_drv_get(unit, pc_drv, pport);
    acc_data.unit = unit;
    acc = &phy_acc.access;
    PHYMOD_ACC_USER_ACC(acc) = &acc_data;
    PHYMOD_ACC_BUS(acc) = pc_drv->phy_bus_get(unit, pport);
    PHYMOD_ACC_ADDR(acc) = pc_drv->phy_addr_get(unit, pport);
    PHYMOD_ACC_LANE_MASK(acc) = 1 << lane_idx;

    PHY_IF_ERR_EXIT
        (phymod_phy_tx_get(&phy_acc, &phymod_tx));
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_get(&phy_acc, &phymod_rx));

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_tx_lane_status_from_phy(unit, &phymod_tx, lst_tx));
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_rx_lane_status_from_phy(unit, &phymod_rx, lst_rx));

exit:
    SHR_FUNC_EXIT();
}

static int
default_tx_lane_cfg_set(int unit, bcmpc_pm_lane_t *pm_lane,
                        bcmpc_pm_tx_lane_prof_t *tx_prof)
{
    bcmpc_drv_t *pc_drv;
    int pport, lanes_per_core, lane_idx;
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    phymod_phy_access_t phy_acc;
    phymod_access_t *acc;
    bcmpc_phy_access_data_t acc_data;
    phymod_tx_t phymod_tx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_info_get(unit, pm_lane->pm_id, &pm_info));
    pport = pm_info.base_pport;
    lanes_per_core = pm_prop->num_lanes / pm_prop->num_aggr;
    lane_idx = pm_lane->core_idx * lanes_per_core + pm_lane->core_lane;

    /* Setup phymod_access_t. */
    phymod_phy_access_t_init(&phy_acc);
    phy_acc.type = port_phy_drv_get(unit, pc_drv, pport);
    acc_data.unit = unit;
    acc = &phy_acc.access;
    PHYMOD_ACC_USER_ACC(acc) = &acc_data;
    PHYMOD_ACC_BUS(acc) = pc_drv->phy_bus_get(unit, pport);
    PHYMOD_ACC_ADDR(acc) = pc_drv->phy_addr_get(unit, pport);
    PHYMOD_ACC_LANE_MASK(acc) = 1 << lane_idx;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_tx_lane_prof_to_phy(unit, tx_prof, &phymod_tx));

    PHY_IF_ERR_EXIT
        (phymod_phy_tx_set(&phy_acc, &phymod_tx));

exit:
    SHR_FUNC_EXIT();
}

static int
default_rx_lane_cfg_set(int unit, bcmpc_pm_lane_t *pm_lane,
                        bcmpc_pm_rx_lane_prof_t *rx_prof)
{
    bcmpc_drv_t *pc_drv;
    int pport, lanes_per_core, lane_idx, num_of_dfe_taps;
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    phymod_phy_access_t phy_acc;
    phymod_access_t *acc;
    bcmpc_phy_access_data_t acc_data;
    phymod_rx_t phymod_rx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_info_get(unit, pm_lane->pm_id, &pm_info));
    pport = pm_info.base_pport;
    lanes_per_core = pm_prop->num_lanes / pm_prop->num_aggr;
    lane_idx = pm_lane->core_idx * lanes_per_core + pm_lane->core_lane;

    /* Setup phymod_access_t. */
    phymod_phy_access_t_init(&phy_acc);
    phy_acc.type = port_phy_drv_get(unit, pc_drv, pport);
    acc_data.unit = unit;
    acc = &phy_acc.access;
    PHYMOD_ACC_USER_ACC(acc) = &acc_data;
    PHYMOD_ACC_BUS(acc) = pc_drv->phy_bus_get(unit, pport);
    PHYMOD_ACC_ADDR(acc) = pc_drv->phy_addr_get(unit, pport);
    PHYMOD_ACC_LANE_MASK(acc) = 1 << lane_idx;

    /* Get the number of DFE TAPs. */
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_get(&phy_acc, &phymod_rx));
    num_of_dfe_taps = phymod_rx.num_of_dfe_taps;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_rx_lane_prof_to_phy(unit, rx_prof, num_of_dfe_taps,
                                        &phymod_rx));
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_set(&phy_acc, &phymod_rx));

exit:
    SHR_FUNC_EXIT();
}

static int
default_pmac_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->op_exec(pmac_acc, pm_pport, op->name, op->param));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * PHY Operations
 */

static pmgr_phy_op_hdl_t pmgr_phy_ops[] = {
    { "lane_enable", phy_enable_set },
};

static int
default_phy_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_pmgr_drv_data_t drv_data;
    phymod_phy_access_t *phy_acc;
    pmgr_phy_op_hdl_t *op_hdl;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    phy_acc = &drv_data.phy_acc;

    for (i = 0; i < COUNTOF(pmgr_phy_ops); i++) {
        op_hdl = &pmgr_phy_ops[i];

        if (sal_strcmp(op->name, op_hdl->name) != 0) {
            continue;
        }

        SHR_RETURN_VAL_EXIT
            (op_hdl->func(unit, phy_acc, op->param));
    }

    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Operation not supported[%s]\n"), op->name));

    SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
default_failover_loopback_get(int unit, bcmpc_pport_t pport,
                              uint32_t *failover_loopback)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->failover_loopback_get(pmac_acc, pm_pport, failover_loopback));

exit:
    SHR_FUNC_EXIT();
}

static int
default_failover_enable_get(int unit, bcmpc_pport_t pport,
                            uint32_t *enable)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->failover_enable_get(pmac_acc, pm_pport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
default_failover_enable_set(int unit, bcmpc_pport_t pport,
                            int enable)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->failover_enable_set(pmac_acc, pm_pport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
default_failover_loopback_remove(int unit, bcmpc_pport_t pport)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_EXIT
        (pmacd->failover_loopback_remove(pmac_acc, pm_pport));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set PFC configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
default_pfc_set(int unit, bcmpc_pport_t pport, bcmpc_pfc_prof_t *prof)
{
    int pm_pport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    bcmpmac_pfc_t pfc_cfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_drv_data_get(unit, pport, &drv_data));

    pm_pport = drv_data.pm_pport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    pfc_cfg.tx_enable = prof->tx_enable;
    pfc_cfg.rx_enable = prof->rx_enable;
    pfc_cfg.stats_enable = prof->stats_enable;
    pfc_cfg.refresh_timer = prof->refresh_timer;
    sal_memcpy(pfc_cfg.da, prof->da, sizeof(pfc_cfg.da));
    pfc_cfg.eth_type = prof->eth_type;
    pfc_cfg.opcode = prof->opcode;
    pfc_cfg.xoff_timer = prof->xoff_timer;

    SHR_IF_ERR_EXIT
        (pmacd->pfc_set(pmac_acc, pm_pport, &pfc_cfg));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

bcmpc_pmgr_drv_t bcmpc_pmgr_drv_default = {
    .pm_port_enable = default_pm_port_enable,
    .port_cfg_set = default_port_cfg_set,
    .port_status_get = default_port_status_get,
    .phy_link_info_get = default_phy_link_info_get,
    .phy_link_state_get = default_phy_link_state_get,
    .fault_state_get = default_fault_state_get,
    .core_mode_set = default_core_mode_set,
    .phy_init = default_phy_init,
    .lane_status_get = default_lane_status_get,
    .tx_lane_cfg_set = default_tx_lane_cfg_set,
    .rx_lane_cfg_set = default_rx_lane_cfg_set,
    .pmac_op_exec = default_pmac_op_exec,
    .phy_op_exec = default_phy_op_exec,
    .failover_enable_set = default_failover_enable_set,
    .failover_enable_get = default_failover_enable_get,
    .failover_loopback_get = default_failover_loopback_get,
    .failover_loopback_remove = default_failover_loopback_remove,
    .pfc_set = default_pfc_set,
};
