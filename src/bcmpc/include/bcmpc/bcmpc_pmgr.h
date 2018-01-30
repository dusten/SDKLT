/*! \file bcmpc_pmgr.h
 *
 * BCMPC Port Manager (PMGR) APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
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

#ifndef BCMPC_PMGR_H
#define BCMPC_PMGR_H

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_lport.h>

/*!
 * \brief Enumeration of PHY line side interface.
 */
typedef enum bcmpc_phy_intf_e {

    /*! No PHY */
    BCMPC_PHY_INTF_NULL,

    /*! SerDes Interface. */
    BCMPC_PHY_INTF_SERDES,

    /*! Media-Independent Interface. */
    BCMPC_PHY_INTF_MII,

    /*! Gigabit MII. */
    BCMPC_PHY_INTF_GMII,

    /*! Serial Gigabit MII. */
    BCMPC_PHY_INTF_SGMII,

    /*! Ten Bit Interface. */
    BCMPC_PHY_INTF_TBI,

    /*! 10-Gigabit MII. */
    BCMPC_PHY_INTF_XGMII,

    /*! Reduced Gigabit MII. */
    BCMPC_PHY_INTF_RGMII,

    /*! Reverse MII. */
    BCMPC_PHY_INTF_RVMII,

    /*! 10-gigabit Attachment Unit Interface. */
    BCMPC_PHY_INTF_XAUI,

    /*! Reduced XAUI. */
    BCMPC_PHY_INTF_RXAUI,

    /*! 40-gigabit AUI. */
    BCMPC_PHY_INTF_XLAUI,

    /*! 100-gigabit AUI. */
    BCMPC_PHY_INTF_CAUI,

    /*! XFP Interface. */
    BCMPC_PHY_INTF_XFI,

    /*! Serdes Framer Interface. */
    BCMPC_PHY_INTF_SFI,

    /*! Coper Interface. */
    BCMPC_PHY_INTF_CX,

    /*! Coper Interface. */
    BCMPC_PHY_INTF_CR,

    /*! Backplane Interface with 64B/66B PCS. */
    BCMPC_PHY_INTF_KR,

    /*! Backplane Interface with 8B/10B PCS. */
    BCMPC_PHY_INTF_KX,

    /*! Short Reach Interface. */
    BCMPC_PHY_INTF_SR,

    /*! Long Reach Interface. */
    BCMPC_PHY_INTF_LR,

    /*! Must be the last. */
    BCMPC_PHY_INTF_COUNT

} bcmpc_phy_intf_t;

/*!
 * \brief Port operating mode configuration
 *
 * This structure store the attribute values of each port operating mode
 * in \ref bcmpc_port_opmode_t.
 */
typedef struct bcmpc_port_opmode_config_s {

    /*! PHY interface. */
    bcmpc_phy_intf_t phy_intf;

    /*! Speed in Mbps. */
    uint32_t speed;

    /*! Half duplex mode. */
    uint8_t hd;

} bcmpc_port_opmode_config_t;

/*!
 * \brief Port status structure
 *
 * This data structure defines the attributes for a logical port status.
 */
typedef struct bcmpc_pmgr_port_status_s {

    /*! Port operating mode. */
    bcmpc_port_opmode_t opmode;

    /*! MAC enabled. */
    uint32_t mac_enabled;

    /*! PHY enabled. */
    uint32_t phy_enabled;

    /*! Encapsulation mode. */
    bcmpc_port_encap_t encap_mode;

    /*! Loopback mode. */
    bcmpc_port_lpbk_t lpbk_mode;

    /*! Pause MAC address. */
    bcmpc_mac_t pause_addr;

    /*! Tx pause ability. */
    uint32_t pause_tx;

    /*! Rx pause ability. */
    uint32_t pause_rx;

    /*! Max frame size. */
    uint32_t max_frame_size;

} bcmpc_pmgr_port_status_t;

/*!
 * \brief Link status type.
 *
 * The link status is not a singal bit for up or down, but also including some
 * link related information, such as remote/local faults, etc.
 *
 * This enum defines the events for the port link status.
 */
typedef uint32_t bcmpc_pmgr_link_status_t;

/*!
 * \brief Link status bit definition.
 *
 * This enum defines the events for the port link status,
 * \ref bcmpc_pmgr_link_status_t.
 * The bit will be set when the corresponding event occurs.
 */
typedef enum bcmpc_pmgr_link_event_e {

    /*! Port is fully operational. */
    BCMPC_PMGR_LINK_EVENT_LINK = 0,

    /*! Local fault. */
    BCMPC_PMGR_LINK_EVENT_LOCAL_FAULT,

    /*! Remote fault. */
    BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT,

    /*! PLL lock error. */
    BCMPC_PMGR_LINK_EVENT_PLL_LOCK_ERR,

    /*! PCS link error. */
    BCMPC_PMGR_LINK_EVENT_PCS_LINK_ERR,

    /*! PMD link error. */
    BCMPC_PMGR_LINK_EVENT_PMD_LINK_ERR,

    /*! No signal detected. */
    BCMPC_PMGR_LINK_EVENT_NO_SIGNAL,

    /*! Lane synchronization error. */
    BCMPC_PMGR_LINK_EVENT_LANE_SYNC_ERR

} bcmpc_pmgr_link_event_t;


/*!
 * \name Link Status Encoding
 *
 * Macro for link status encoding/decoding.
 */

/*! \{ */

/*!
 * \brief Macro to set the bit mask of link status.
 *
 * \param _st Link status bit mask, i.e. bcmpc_link_status_t.
 * \param _e Link event, i.e. bcmpc_pmgr_link_event_t
 */
#define BCMPC_PMGR_LINK_ST_SET(_st, _e) do { _st |= 1 << _e; } while (0);

/*!
 * \brief Macro to get a link event bit from link status.
 *
 * \param _st Link status bit mask, i.e. bcmpc_link_status_t.
 * \param _e Link event, i.e. bcmpc_pmgr_link_event_t
 */
#define BCMPC_PMGR_LINK_ST_GET(_st, _e) ( (_st >> _e) & 1 )

/*! \} */


/*******************************************************************************
 * PMGR APIs
 */

/*!
 * \brief Initalize port status data structure.
 *
 * \param [in] st Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_port_status_t_init(bcmpc_pmgr_port_status_t *st);

/*!
 * \brief Enable the PM port of the given physical port.
 *
 * This function is used to enable/disable a port in port-block level.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] enable 1 to enalbe, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pm_port_enable(int unit, bcmpc_pport_t pport,
                          int enable);

/*!
 * \brief Set the port configuration of the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] lport The associated logical port.
 * \param [in] cfg Port configuration to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_cfg_set(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                        bcmpc_port_cfg_t *cfg);

/*!
 * \brief Get the port status of the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Port status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_port_status_t *st);

/*!
 * \brief Get the link status of the given physical port.
 *
 * This API is intended for use by the PC_PORT_STATUS LT.
 *
 * It will always return the complete set of link parameters e.g. link
 * status, fault state, PLL lock errors, etc.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_link_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Get the link state of the given physical port.
 *
 * This API is intended for use by software linkscan.
 *
 * It will return a completed link state which consider both of PHY link state
 * and MAC fault state.
 *
 * When a port is set to MAC/PHY local loopback mode, the link state will be
 * focred "up".
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] link Link state. 1 for link up and 0 for down.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_link_state_get(int unit, bcmpc_pport_t pport,
                          int *link);

/*!
 * \brief Get the link fault state of the given physical port.
 *
 * This API is intended for use by software linkscan.
 *
 * It only returns the remote fault state for now, but it might be revised to
 * return both of local and remote fault state if required.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] rmt_fault Remote fault state.
 *                        1 when remote fault is detected, otherwise 0.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_fault_state_get(int unit, bcmpc_pport_t pport,
                           int *rmt_fault);

/*!
 * \brief Execute the link change process per the link state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_link_change(int unit, bcmpc_pport_t pport,
                            int link);

/*!
 * \brief Get the speed value from the given port opmode.
 *
 * \param [in] unit Unit number.
 * \param [in] opmode Port operating mode.
 * \param [speed] speed Speed value.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_port_opmode_to_speed(int unit, bcmpc_port_opmode_t opmode,
                           uint32_t *speed);

#endif /* BCMPC_PMGR_H */
