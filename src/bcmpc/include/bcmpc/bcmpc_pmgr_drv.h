/*! \file bcmpc_pmgr_drv.h
 *
 * BCMPC PMGR Driver Object.
 *
 * Declare the PMGR driver structures and publish the driver instances.
 *
 * The Port Manager driver would handle the PMAC and PHY programming sequence.
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

#ifndef BCMPC_PMGR_DRV_H
#define BCMPC_PMGR_DRV_H

#include <bcmpc/bcmpc_drv.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_lport_internal.h>

/*******************************************************************************
 * PMGR driver functions
 */

/*!
 * \name Port Manager driver functions
 *
 * The function prototypes for \ref bcmpc_pmgr_drv_t.
 */

/*! \{ */

/*!
 * \brief Enable the PM port of the given physical port.
 *
 * See \ref bcmpc_pmgr_pm_port_enable.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] enable 1 to enalbe, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_port_enable_f)(int unit, bcmpc_pport_t pport,
                               int enable);

/*!
 * \brief Set the port configuration of the given physical port.
 *
 * See \ref bcmpc_pmgr_port_cfg_set.
 *
 * Note that the implementation of this function should only update the hardware
 * if the configuration has changed.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] lport The associated logical port.
 * \param [in] cfg Port configuration to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_port_cfg_set_f)(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                        bcmpc_port_cfg_t *cfg);

/*!
 * \brief Get the port status of the given physical port.
 *
 * See \ref bcmpc_pmgr_port_status_get.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Port status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_port_status_get_f)(int unit, bcmpc_pport_t pport,
                                bcmpc_pmgr_port_status_t *st);

/*!
 * \brief Get the PHY link info of the given physical port.
 *
 * This function is used to get PHY link information e.g. PCS/PMD link error.
 *
 * It provides the detail information for understanding why link state is down.
 * When PHY link errors are detected, the corresponding bits (\ref
 * bcmpc_pmgr_link_event_t) in \c st will be set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_link_info_get_f)(int unit, bcmpc_pport_t pport,
                                  bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Get the PHY link state of the given physical port.
 *
 * This function only reports the PHY link state. When link up,
 * the bit \ref BCMPC_PMGR_LINK_EVENT_LINK in \c st will be set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_link_state_get_f)(int unit, bcmpc_pport_t pport,
                                   bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Get the fault state of the given physical port.
 *
 * This function reports the MAC local and remote fault state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_fault_state_get_f)(int unit, bcmpc_pport_t pport,
                                bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Set the port mode of a PM.
 *
 * See \ref bcmpc_pmgr_core_mode_set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The first device physical port of the PM.
 * \param [in] mode PM mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_core_mode_set_f)(int unit, bcmpc_pport_t pport,
                              bcmpc_pm_mode_t *mode);

/*!
 * \brief Initialize the PHY of the given physical port.
 *
 * See \ref bcmpc_pmgr_phy_init.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ccfg Core configuration.
 * \param [in] do_core_init 1 to do core initialization, otherwise 0.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_init_f)(int unit, bcmpc_pport_t pport,
                         bcmpc_pm_core_cfg_t *ccfg, int do_core_init);

/*!
 * \brief Get the PM lane status.
 *
 * See \ref bcmpc_pmgr_lane_status_get.
 *
 * \param [in] unit Unit number
 * \param [in] pm_lane PM lane.
 * \param [out] st PM lane status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_lane_status_get_f)(int unit, bcmpc_pm_lane_t *pm_lane,
                                bcmpc_pm_lane_status_t *st);

/*!
 * \brief Set the Tx lane config.
 *
 * See \ref bcmpc_pmgr_tx_lane_cfg_set.
 *
 * \param [in] unit Unit number
 * \param [in] pm_lane PM lane.
 * \param [in] tx_prof Tx lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_tx_lane_cfg_set_f)(int unit, bcmpc_pm_lane_t *pm_lane,
                                bcmpc_pm_tx_lane_prof_t *tx_prof);

/*!
 * \brief Set the Rx lane config.
 *
 * See \ref bcmpc_pmgr_rx_lane_cfg_set.
 *
 * \param [in] unit Unit number
 * \param [in] pm_lane PM lane.
 * \param [in] rx_prof Rx lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_rx_lane_cfg_set_f)(int unit, bcmpc_pm_lane_t *pm_lane,
                                bcmpc_pm_rx_lane_prof_t *rx_prof);

/*!
 * \brief Get LAG failover status.
 *
 * See \ref bcmpc_pmgr_failover_loopback_get.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] failover_loopback 1 when in lag failover loopback state,
 *                                otherwise 0.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_loopback_get_f)(int unit, bcmpc_pport_t pport,
                                      uint32_t *failover_loopback);

/*!
 * \brief Get LAG failover enable state.
 *
 * See \ref bcmpc_pmgr_failover_enable_get.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] en 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_enable_get_f)(int unit, bcmpc_pport_t pport,
                                    uint32_t *en);

/*!
 * \brief Set LAG failover enable state.
 *
 * See \ref bcmpc_pmgr_failover_enable_set.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] en 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_enable_set_f)(int unit, bcmpc_pport_t pport,
                                    int en);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * See \ref bcmpc_pmgr_failover_loopback_remove.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_loopback_remove_f)(int unit, bcmpc_pport_t pport);


/*!
 * \brief Set PFC configuration.
 *
 * See \ref bcmpc_pmgr_pfc_set.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] pfc_prof PFC configuration.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_pfc_set_f)(int unit, bcmpc_pport_t pport,
                        bcmpc_pfc_prof_t *pfc_prof);

/*! \} */


/*******************************************************************************
 * PMGR driver object
 */

/*!
 * \brief Port manager driver.
 *
 * A port manager is a driver which controls complex operations on a logical
 * port. The port manager is the component, which knows how a port, MAC and
 * PHY work together. Several logical port operations require that PHY APIs
 * and PMAC APIs are called in a particular order.
 */
typedef struct bcmpc_pmgr_drv_s {

    /*! PM port enable set. */
    bcmpc_pmgr_pm_port_enable_f pm_port_enable;

    /*! Port configuration set. */
    bcmpc_port_cfg_set_f port_cfg_set;

    /*! Port status get. */
    bcmpc_pmgr_port_status_get_f port_status_get;

    /*! PHY link information get. */
    bcmpc_pmgr_phy_link_info_get_f phy_link_info_get;

    /*! PHY link state get. */
    bcmpc_pmgr_phy_link_state_get_f phy_link_state_get;

    /*! Fault state get. */
    bcmpc_pmgr_fault_state_get_f fault_state_get;

    /*! PM mode set. */
    bcmpc_pmgr_core_mode_set_f core_mode_set;

    /*! PHY initialization. */
    bcmpc_pmgr_phy_init_f phy_init;

    /*! PM lane status get. */
    bcmpc_pmgr_lane_status_get_f lane_status_get;

    /*! Tx lane configuration set. */
    bcmpc_pmgr_tx_lane_cfg_set_f tx_lane_cfg_set;

    /*! Rx lane configuration set. */
    bcmpc_pmgr_rx_lane_cfg_set_f rx_lane_cfg_set;

    /*! PHY operation execute. */
    bcmpc_op_exec_f phy_op_exec;

    /*! PMAC operation execute. */
    bcmpc_op_exec_f pmac_op_exec;

    /*! Failover status get function. */
    bcmpc_pmgr_failover_loopback_get_f failover_loopback_get;

    /*! Failover set function. */
    bcmpc_pmgr_failover_enable_set_f failover_enable_set;

    /*! Failover get function. */
    bcmpc_pmgr_failover_enable_get_f failover_enable_get;

    /*! Failover loopback bring out function. */
    bcmpc_pmgr_failover_loopback_remove_f failover_loopback_remove;

    /*! PFC configuration set. */
    bcmpc_pmgr_pfc_set_f pfc_set;

} bcmpc_pmgr_drv_t;


/*******************************************************************************
 * PMGR driver instance
 */

/*! The default port manager driver. */
extern bcmpc_pmgr_drv_t bcmpc_pmgr_drv_default;

#endif /* BCMPC_PMGR_DRV_H */
