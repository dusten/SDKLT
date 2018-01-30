/*! \file bcmpmac_drv.h
 *
 * BCM PortMAC Driver.
 *
 * Declaration of the functions which are supported by PortMAC driver.
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

#ifndef BCMPMAC_DRV_H
#define BCMPMAC_DRV_H

#include <bcmpmac/bcmpmac_acc.h>

/******************************************************************************
 * PortMAC driver functions
 */

/*!
 * \brief Initialize a port.
 *
 * This function is used to initialize or clean up a port. It is called when a
 * port is added or deleted.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] init 1 to initialize; 0 to clean up.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_init_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       uint32_t init);

/*!
 * \brief Set Port reset state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] reset 1 to reset; 0 to out of reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_reset_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            uint32_t reset);

/*!
 * \brief Get port reset state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] reset 1 to reset; 0 to out of reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_reset_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            uint32_t *reset);

/*!
 * \brief Set port enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                             uint32_t enable);

/*!
 * \brief Get port enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                             uint32_t *enable);

/*!
 * \brief Set MAC reset state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] reset 1 to reset; 0 to out of reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_mac_reset_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t reset);

/*!
 * \brief Get MAC reset state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] reset 1 to reset; 0 to out of reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_mac_reset_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *reset);

/*!
 * \brief Set Rx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rx_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t enable);

/*!
 * \brief Get Rx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rx_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *enable);

/*!
 * \brief Set Tx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t enable);

/*!
 * \brief Get Tx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *enable);

/*!
 * \brief Set speed.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] speed Speed in Mbps.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_speed_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       uint32_t speed);

/*!
 * \brief Get speed.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] speed Speed in Mbps.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_speed_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       uint32_t *speed);

/*!
 * \brief Set encap mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] encap Encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_encap_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_encap_mode_t encap);

/*!
 * \brief Get encap mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] encap Encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_encap_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_encap_mode_t *encap);

/*!
 * \brief Set MAC loopback.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] en 1 to enable MAC loopback; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_lpbk_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                      uint32_t en);

/*!
 * \brief Get MAC loopback.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] en 1 to enable MAC loopback; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_lpbk_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                      uint32_t *en);

/*!
 * \brief Set pause flow control configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] pause_ctrl Pause flow control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_pause_ctrl_t *pause_ctrl);

/*!
 * \brief Get pause flow control configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] pause_ctrl Pause flow control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_pause_ctrl_t *pause_ctrl);

/*!
 * \brief Set PAUSE frame source address.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] mac MAC address used for PAUSE transmission.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_addr_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            bcmpmac_mac_t mac);

/*!
 * \brief Get PAUSE frame source address.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] mac MAC address used for PAUSE transmission.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_addr_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            bcmpmac_mac_t mac);

/*!
 * \brief Set maximum receive frame size.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] size Maximum frame size in bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_frame_max_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t size);

/*!
 * \brief Get maximum receive frame size.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] size Maximum frame size in bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_frame_max_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *size);


/*!
 * \brief Get fault status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] st Fault status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_fault_status_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                              bcmpmac_fault_status_t *st);

/*!
 * \brief Set PM port mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] mode PM port mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_core_mode_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           bcmpmac_core_port_mode_t *mode);

/*!
 * \brief Execute port operation.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] op Operation string.
 * \param [in] op_param Operation parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_op_exec_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                     const char *op, uint32_t op_param);

/*!
 * \brief Get LAG failover loopback status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] failover_lpbk Set when in lag failover loopback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_loopback_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                   uint32_t *failover_lpbk);

/*!
 * \brief Get LAG failover enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] en 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                                 uint32_t *en);

/*!
 * \brief Enable/Disable LAG failover.
 *
 * If LAG failover mode is enabled, the MAC will route all TX packets to the RX
 * path when the link is down. The state is called failover loopback.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] en 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                                 uint32_t en);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * Change the port back to normal operation no matter the port is in failover
 * loopback state or not.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_loopback_remove_f)(bcmpmac_access_t *pa,
                                      bcmpmac_pport_t pm_pport);

/*!
 * \brief Set PFC configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] pfc_cfg PFC configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pfc_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                     bcmpmac_pfc_t *pfc_cfg);

/*!
 * \brief Get PFC configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] pfc_cfg PFC configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pfc_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                     bcmpmac_pfc_t *pfc_cfg);


/******************************************************************************
 * PortMAC driver object
 */

/*!
 * \brief PortMAC driver
 *
 * PortMAC driver function vector.
 *
 * There are two common parameters cross the functions of the driver.
 *
 * The first is \c pa (PAMC access info) which provides the necessary
 * infomation for hardware access. And the second is \c pm_pport (Port macro
 * physical port number) which indicates the target port of the operation.
 *
 * The physical port macro port is the physical port within a port macro. It is
 * sometimes referred to as the "block-port number" or the "port index".
 */
typedef struct bcmpmac_drv_s {

    /*! Driver name. */
    const char *name;

    /*! Port init function. */
    bcmpmac_port_init_f port_init;

    /*! Port reset set function. */
    bcmpmac_port_reset_set_f port_reset_set;

    /*! Port reset get function. */
    bcmpmac_port_reset_get_f port_reset_get;

    /*! Port enable set function. */
    bcmpmac_port_enable_set_f port_enable_set;

    /*! Port enable get function. */
    bcmpmac_port_enable_get_f port_enable_get;

    /*! MAC reset set function. */
    bcmpmac_mac_reset_set_f mac_reset_set;

    /*! MAC reset get function. */
    bcmpmac_mac_reset_get_f mac_reset_get;

    /*! Rx enable set function. */
    bcmpmac_rx_enable_set_f rx_enable_set;

    /*! Rx enable get function. */
    bcmpmac_rx_enable_get_f rx_enable_get;

    /*! Tx enable set function. */
    bcmpmac_tx_enable_set_f tx_enable_set;

    /*! Tx enable get function. */
    bcmpmac_tx_enable_get_f tx_enable_get;

    /*! Speed set function. */
    bcmpmac_speed_set_f speed_set;

    /*! Speed get function. */
    bcmpmac_speed_get_f speed_get;

    /*! Encap mode set function. */
    bcmpmac_encap_set_f encap_set;

    /*! Encap mode get function. */
    bcmpmac_encap_get_f encap_get;

    /*! MAC loopback set function. */
    bcmpmac_lpbk_set_f lpbk_set;

    /*! MAC loopback get function. */
    bcmpmac_lpbk_get_f lpbk_get;

    /*! Pause set function. */
    bcmpmac_pause_set_f pause_set;

    /*! Pause get function. */
    bcmpmac_pause_get_f pause_get;

    /*! PAUSE frame source address set function. */
    bcmpmac_pause_addr_set_f pause_addr_set;

    /*! PAUSE frame source address get function. */
    bcmpmac_pause_addr_get_f pause_addr_get;

    /*! Max receive frame size set function. */
    bcmpmac_frame_max_set_f frame_max_set;

    /*! Max receive frame size get function. */
    bcmpmac_frame_max_get_f frame_max_get;

    /*! Max receive frame size get function. */
    bcmpmac_fault_status_get_f fault_status_get;

    /*! Core port mode set function. */
    bcmpmac_core_mode_set_f core_mode_set;

    /*! Excute operation. */
    bcmpmac_op_exec_f op_exec;

    /*! Failover set function. */
    bcmpmac_failover_enable_set_f failover_enable_set;

    /*! Failover get function. */
    bcmpmac_failover_enable_get_f failover_enable_get;

    /*! Failover loopback status get function. */
    bcmpmac_failover_loopback_get_f failover_loopback_get;

    /*! Failover loopback bring out function. */
    bcmpmac_failover_loopback_remove_f failover_loopback_remove;

    /*! PFC set function. */
    bcmpmac_pfc_set_f pfc_set;

    /*! PFC get function. */
    bcmpmac_pfc_get_f pfc_get;

} bcmpmac_drv_t;

#endif /* BCMPMAC_DRV_H */
