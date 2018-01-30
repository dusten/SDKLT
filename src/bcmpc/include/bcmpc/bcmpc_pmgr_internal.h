/*! \file bcmpc_pmgr_internal.h
 *
 * BCMPC PMGR Internal Functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
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

#ifndef BCMPC_PMGR_INTERNAL_H
#define BCMPC_PMGR_INTERNAL_H

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_drv.h>
#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pm.h>


/*!
 * \brief Port config entry structure.
 *
 * The port configurations are stored within a linked list.
 * This data structure represents the element of the list.
 *
 * The key of the entry, i.e. \c dbe->key, is a physical port number.
 */
typedef struct bcmpc_pmgr_db_entry_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! Port configuration. */
    bcmpc_port_cfg_t pcfg;

} bcmpc_pmgr_db_entry_t;


/*!
 * \brief Port manager driver data for a port
 *
 * This structure is used to store the PMAC and PHY driver information for a
 * port. The information is gotten via the methods of \ref bcmpc_drv_t.
 *
 * In brief, \ref bcmpc_drv_t provides the "methods" to retrive driver
 * information for each port. And \ref bcmpc_pmgr_drv_data_t is the "storage"
 * to save the information.
 */
typedef struct bcmpc_pmgr_drv_data_s {

    /*! Physical device port number. */
    bcmpc_pport_t pport;

    /*! Physical PM port number. */
    bcmpc_pport_t pm_pport;

    /*! Max spped of the port. */
    int max_speed;

    /*! PMAC Driver. */
    bcmpmac_drv_t *pmac_drv;

    /*! PMAC Access. */
    bcmpmac_access_t pmac_acc;

    /*! PMAC Access data. */
    bcmpc_pmac_access_data_t pmac_data;

    /*! PHY Access. */
    phymod_phy_access_t phy_acc;

    /*! PHY Core Access. */
    phymod_core_access_t phy_core_acc;

    /*! PHY Core Lanes. */
    int phy_core_lanes;

    /*! PHY Access Driver. */
    bcmpc_phy_access_data_t phy_data;

} bcmpc_pmgr_drv_data_t;


/*******************************************************************************
 * Port config DB functions
 */

/*!
 * \brief Save the port configuration to DB.
 *
 * \param [in] unit Unit number
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_db_pcfg_save(int unit, bcmpc_pport_t pport,
                        bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Get the port configuration from DB.
 *
 * \param [in] unit Unit number
 * \param [in] pport Physical port number.
 * \param [out] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_db_pcfg_get(int unit, bcmpc_pport_t pport,
                       bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Destroy the port configuration from DB.
 *
 * \param [in] unit Unit number
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_db_entry_destroy(int unit, bcmpc_pport_t pport);


/*******************************************************************************
 * Type converter functions
 */

/*!
 * \brief Convert PMGR encap mode to PMAC.
 *
 * \param [in] pmgr_mode PMGR encap mode.
 * \param [out] pmac_mode PMAC encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_encap_mode_to_pmac(bcmpc_port_encap_t pmgr_mode,
                              bcmpmac_encap_mode_t *pmac_mode);

/*!
 * \brief Convert PMGR encap mode from PMAC.
 *
 * \param [in] pmac_mode PMAC encap mode.
 * \param [out] pmgr_mode PMGR encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_encap_mode_from_pmac(bcmpmac_encap_mode_t pmac_mode,
                                bcmpc_port_encap_t *pmgr_mode);

/*!
 * \brief Convert PM operating mode to PMAC.
 *
 * \param [in] pm_mode PM operating mode.
 * \param [out] pmac_mode PMAC core port mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_mode_to_pmac(bcmpc_pm_mode_t *pm_mode,
                           bcmpmac_core_port_mode_t *pmac_mode);

/*!
 * \brief Get the default PHYMOD interface.
 *
 * The default interface type is derived from port speed, number of lanes and
 * encapsulation mode.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] num_lanes Number of lanes.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_default_phymod_intf_get(int speed, int num_lanes, int is_hg,
                                   int fiber_pref, phymod_interface_t *intf);

/*!
 * \brief Convert core clock value to PHYMOD reference clock type.
 *
 * \param [in] ref_clk Core reference clock in KHz.
 * \param [out] phymod_clk PHYMOD reference clock value.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_ref_clk_to_phy(int ref_clk, phymod_ref_clk_t *phymod_clk);

/*!
 * \brief Convert PMGR operating mode to PHYMOD interface config.
 *
 * \param [in] unit Unit number
 * \param [in] pcfg Port configuration.
 * \param [in] num_lanes The number of lanes.
 * \param [in] is_hg Set when using Higig/Higig2 encap.
 * \param [in] ref_clk Core refernece clock in KHz.
 * \param [out] intf_cfg PHYMOD interface config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_opmode_to_phy(int unit, bcmpc_port_cfg_t *pcfg,
                         int num_lanes, int is_hg, int ref_clk,
                         phymod_phy_inf_config_t *intf_cfg);

/*!
 * \brief Convert PMGR operating mode from PHYMOD interface config.
 *
 * \param [in] unit Unit number
 * \param [in] intf_cfg PHYMOD interface config.
 * \param [out] opmode Port operating mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_opmode_from_phy(int unit, phymod_phy_inf_config_t *intf_cfg,
                           bcmpc_port_opmode_t *opmode);

/*!
 * \brief Convert core configuration to PHYMOD.
 *
 * \param [in] unit Unit number
 * \param [in] ccfg PHYMOD interface config.
 * \param [in] num_lanes Number of lanes for the core.
 * \param [out] phymod_ccfg Phymod core configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_core_cfg_to_phy(int unit, bcmpc_pm_core_cfg_t *ccfg,
                              int num_lanes,
                              phymod_core_init_config_t *phymod_ccfg);

/*!
 * \brief Convert Tx lane profile to PHYMOD Tx config.
 *
 * \param [in] unit Unit number
 * \param [in] prof_tx Tx lane profile.
 * \param [out] phymod_tx PHYMOD Tx config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_tx_lane_prof_to_phy(int unit, bcmpc_pm_tx_lane_prof_t *prof_tx,
                               phymod_tx_t *phymod_tx);

/*!
 * \brief Convert Rx lane profile to PHYMOD Rx config.
 *
 * \param [in] unit Unit number
 * \param [in] prof_rx Rx lane profile.
 * \param [in] dfe_taps The number of DFE TAPs.
 * \param [out] phymod_rx PHYMOD Rx config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_rx_lane_prof_to_phy(int unit, bcmpc_pm_rx_lane_prof_t *prof_rx,
                               int dfe_taps, phymod_rx_t *phymod_rx);

/*!
 * \brief Convert Tx lane status from PHYMOD Tx config.
 *
 * \param [in] unit Unit number
 * \param [in] phymod_tx PHYMOD Tx config.
 * \param [out] st_tx Tx lane status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_tx_lane_status_from_phy(int unit, phymod_tx_t *phymod_tx,
                                   bcmpc_pm_tx_lane_prof_t *st_tx);

/*!
 * \brief Convert Rx lane status from PHYMOD Rx config.
 *
 * \param [in] unit Unit number
 * \param [in] phymod_rx PHYMOD Rx config.
 * \param [out] st_rx Rx lane status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_rx_lane_status_from_phy(int unit, phymod_rx_t *phymod_rx,
                                   bcmpc_pm_rx_lane_prof_t *st_rx);

/*!
 * \brief Convert autoneg profile to phymod_autoneg_control_t.
 *
 * \param [in] unit Unit number.
 * \param [in] prof Autoneg profile.
 * \param [out] phy_an_ctrl PHYMOD autoneg control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_autoneg_prof_to_phy_ctrl(int unit, bcmpc_autoneg_prof_t *prof,
                               phymod_autoneg_control_t *phy_an_ctrl);

/*!
 * \brief Convert autoneg profile to phymod_autoneg_ability_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_lanes Number of lanes.
 * \param [in] is_hg Is Higig/Higig2 mode.
 * \param [in] prof Autoneg profile.
 * \param [out] phy_an_ability PHYMOD autoneg ability.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_autoneg_prof_to_phy_adv(int unit, int num_lanes, int is_hg,
                              bcmpc_autoneg_prof_t *prof,
                              phymod_autoneg_ability_t *phy_an_ability);


/*******************************************************************************
 * Hardware configuration functions for other PC components.
 */

/*!
 * \brief Set the port mode of a PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The first device physical port of the PM.
 * \param [in] mode PM operating mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_core_mode_set(int unit, bcmpc_pport_t pport,
                         bcmpc_pm_mode_t *mode);

/*!
 * \brief Initialize the PHY of the given physical port.
 *
 * This function is used to initialize the PHY of the given physical port.
 *
 * When \c do_core_init is set, this function will initialize the PHY of the
 * port, and also the core where the port belong to.
 *
 * We might need do core initialization when the first port of a core is
 * created.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ccfg Core configuration.
 * \param [in] do_core_init 1 to do core initialization, otherwise 0.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_phy_init(int unit, bcmpc_pport_t pport,
                    bcmpc_pm_core_cfg_t *ccfg, int do_core_init);

/*!
 * \brief Get the PM lane status.
 *
 * \param [in] unit Unit number
 * \param [in] pm_lane PM lane.
 * \param [out] st PM lane status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_lane_status_get(int unit, bcmpc_pm_lane_t *pm_lane,
                           bcmpc_pm_lane_status_t *st);

/*!
 * \brief Get LAG failover status.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] failover_loopback 1 when in lag failover loopback state,
 *                                otherwise 0.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_loopback_get(int unit, bcmpc_pport_t pport,
                                 uint32_t *failover_loopback);

/*!
 * \brief Get LAG failover enable state.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] enable 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_enable_get(int unit, bcmpc_pport_t pport,
                               uint32_t *enable);

/*!
 * \brief Enable/Disable LAG failover mode.
 *
 * If LAG failover mode is enabled, the MAC will route all TX packets to the RX
 * path when the link is down. The state is called failover loopback.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] enable 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_enable_set(int unit, bcmpc_pport_t pport,
                               uint32_t enable);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * Change the port back to normal operation no matter the port is in failover
 * loopback state or not.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_loopback_remove(int unit, bcmpc_pport_t pport);

/*!
 * \brief Set the Tx lane config.
 *
 * \param [in] unit Unit number
 * \param [in] pm_lane PM lane.
 * \param [in] tx_prof Tx lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_tx_lane_cfg_set(int unit, bcmpc_pm_lane_t *pm_lane,
                           bcmpc_pm_tx_lane_prof_t *tx_prof);

/*!
 * \brief Set the Rx lane config.
 *
 * \param [in] unit Unit number
 * \param [in] pm_lane PM lane.
 * \param [in] rx_prof Rx lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_rx_lane_cfg_set(int unit, bcmpc_pm_lane_t *pm_lane,
                           bcmpc_pm_rx_lane_prof_t *rx_prof);

/*!
 * \brief Get the current speed of a given port.
 *
 * When link is up, this function will return the hardware port speed value.
 * If it is not able to get the speed from hardware, e.g. on a simulator, the
 * configured port speed will be returened according to \c opmode_cfg.
 *
 * When link is down, this function will return 0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] opmode_cfg The configured operating mode.
 *
 * \returns The current port speed, or 0 if link down.
 */
extern int
bcmpc_pmgr_speed_get(int unit, bcmpc_pport_t pport,
                     bcmpc_port_opmode_t opmode_cfg);

/*!
 * \brief Set the PFC config.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] pfc_prof PFC lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pfc_set(int unit, bcmpc_pport_t pport,
                   bcmpc_pfc_prof_t *pfc_prof);

#endif /* BCMPC_PMGR_INTERNAL_H */
