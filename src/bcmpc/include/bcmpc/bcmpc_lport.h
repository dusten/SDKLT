/*! \file bcmpc_lport.h
 *
 * BCMPC Logical PORT (LPORT) APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 *
 * The logical port library provides the functions to create switch logical
 * ports and associate the logical port number to a physical device port number.
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

#ifndef BCMPC_LPORT_H
#define BCMPC_LPORT_H

#include <bcmpc/bcmpc_types.h>


/*!
 * \brief Globally available port operating modes.
 *
 * Any given logical port will only support a subset of these modes.
 *
 * 1000/100/10 Mbps copper ports will use MII if the internal PHY is
 * not a real copper PHY.
 *
 * Note that this enum should be same as PC_PORT_OPMODE_T.
 */
typedef enum bcmpc_port_opmode_e {

    /*! Ensure that zero is not interpreted as a valid mode. */
    BCMPC_PORT_OPMODE_INVALID = 0,

    /*! Special mode for selecting default lane configuration. */
    BCMPC_PORT_OPMODE_ANY,

    /*! Auto-negotiation mode. */
    BCMPC_PORT_OPMODE_AUTONEG,

    /*! Custom port mode, where parameters are read from a profile. */
    BCMPC_PORT_OPMODE_CUSTOM,

    /*! 10 Mbps copper mode (10BASE-T). */
    BCMPC_PORT_OPMODE_10M_T,

    /*! 10 Mbps copper half-duplex mode (10BASE-T HDX). */
    BCMPC_PORT_OPMODE_10M_T_HDX,

    /*! 100 Mbps copper mode (100BASE-T). */
    BCMPC_PORT_OPMODE_100M_T,

    /*! 100 Mbps copper half-duplex mode (100BASE-T HDX). */
    BCMPC_PORT_OPMODE_100M_T_HDX,

    /*! 100 Mbps fiber mode (100BASE-FX). */
    BCMPC_PORT_OPMODE_100M_FX,

    /*! 1000 Mbps copper mode (1000BASE-T). */
    BCMPC_PORT_OPMODE_1000M_T,

    /*! 1000 Mbps fiber mode (1000BASE-X). */
    BCMPC_PORT_OPMODE_1000M_X,

    /*! 2.5 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_2P5G,

    /*! Default 10 Gbps mode for this port type (typically XAUI). */
    BCMPC_PORT_OPMODE_10G,

    /*! 10 Gbps SFI mode. */
    BCMPC_PORT_OPMODE_10G_SFI,

    /*! 10 Gbps XFI mode. */
    BCMPC_PORT_OPMODE_10G_XFI,

    /*! 10 Gbps CX mode. */
    BCMPC_PORT_OPMODE_10G_CX,

    /*! 10 Gbps CR mode. */
    BCMPC_PORT_OPMODE_10G_CR,

    /*! 10 Gbps KR mode. */
    BCMPC_PORT_OPMODE_10G_KR,

    /*! 10 Gbps KX mode. */
    BCMPC_PORT_OPMODE_10G_KX,

    /*! 12 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_12G,

    /*! 13 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_13G,

    /*! Default 20 Gbps mode for this port type (typically RXAUI). */
    BCMPC_PORT_OPMODE_20G,

    /*! 20 Gbps KR mode (20GBASE-KR2). */
    BCMPC_PORT_OPMODE_20G_KR,

    /*! 21 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_21G,

    /*! 25 Gbps XFI mode. */
    BCMPC_PORT_OPMODE_25G_XFI,

    /*! 27 Gbps XFI mode (HiGig only). */
    BCMPC_PORT_OPMODE_27G_XFI,

    /*! Default 40 Gbps mode for this port type (typically (XLAUI). */
    BCMPC_PORT_OPMODE_40G,

    /*! 40 Gbps CR mode (40GBASE-CR4). */
    BCMPC_PORT_OPMODE_40G_CR,

    /*! 40 Gbps KR mode (40GBASE-KR4). */
    BCMPC_PORT_OPMODE_40G_KR,

    /*! 42 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_42G,

    /*! 50 Gbps KR mode. */
    BCMPC_PORT_OPMODE_50G_KR,

    /*! 53 Gbps KR mode (HiGig only). */
    BCMPC_PORT_OPMODE_53G_KR,

    /*! Default 100 Gbps mode for this port type (typically CAUI). */
    BCMPC_PORT_OPMODE_100G,

    /*! 100 Gbps CR mode. */
    BCMPC_PORT_OPMODE_100G_CR,

    /*! 106 Gbps KR mode (HiGig only). */
    BCMPC_PORT_OPMODE_106G_KR,

    /*! 120 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_120G,

    /*! 127 Gbps mode (HiGig only). */
    BCMPC_PORT_OPMODE_127G,

    /*! 200 Gbps mode. */
    BCMPC_PORT_OPMODE_200G,

    /*! 400 Gbps mode. */
    BCMPC_PORT_OPMODE_400G,

    /*! Must be the last. */
    BCMPC_PORT_OPMODE_COUNT

} bcmpc_port_opmode_t;

/*!
 * \brief Globally available packet encapsulations.
 *
 * Note that this enum should be same as PC_ENCAP_T.
 */
typedef enum bcmpc_port_encap_e {

    /*! Standard Ethernet encapsulation. */
    BCMPC_PORT_ENCAP_IEEE = 0,

    /*! Broadcom HiGig+ encapsulation. */
    BCMPC_PORT_ENCAP_HIGIG,

    /*! Broadcom HiGig2 encapsulation. */
    BCMPC_PORT_ENCAP_HIGIG2,

    /*! Broadcom HiGig-Lite encapsulation. */
    BCMPC_PORT_ENCAP_HIGIG_LITE,

    /*! Broadcom Encapsulated HiGig (E-HG) L2 tunnel. */
    BCMPC_PORT_ENCAP_EHG_L2,

    /*! Broadcom Encapsulated HiGig (E-HG) L3 (IP GRE) tunnel. */
    BCMPC_PORT_ENCAP_EHG_L3,

    /*! Broadcom HiGig Over Ethernet (HGoE) encapsulation. */
    BCMPC_PORT_ENCAP_HGOE,

    /*! Must be the last. */
    BCMPC_PORT_ENCAP_COUNT

} bcmpc_port_encap_t;

/*!
 * \brief Port Control loopback types.
 *
 * PHY loopback modes must squelch the unused Tx and Rx paths if
 * possible.
 *
 * Outermost is closer to the line side of the PHY.
 *
 * Innermost is closer to the system (MAC) side of the PHY.
 *
 * Note that this enum should be same as PC_LOOPBACK_T.
 */
typedef enum bcmpc_port_lpbk_e {

    /*! No loopback. */
    BCMPC_PORT_LPBK_NONE = 0,

    /*! MAC loopback. */
    BCMPC_PORT_LPBK_MAC,

    /*! PCS loopback. */
    BCMPC_PORT_LPBK_PHY_PCS,

    /*! PMD loopback. */
    BCMPC_PORT_LPBK_PHY_PMD,

    /*! Outermost PHY loopback path. */
    BCMPC_PORT_LPBK_PHY,

    /*! PCS remote loopback. */
    BCMPC_PORT_LPBK_REMOTE_PCS,

    /*! PMD remote loopback. */
    BCMPC_PORT_LPBK_REMOTE_PMD,

    /*! Outermost remote loopback. */
    BCMPC_PORT_LPBK_REMOTE,

    /*! Must be the last. */
    BCMPC_PORT_LPBK_COUNT

} bcmpc_port_lpbk_t;

/*!
 * \brief Port Control FEC types.
 *
 * Note that this enum should be same as PC_FEC_T.
 */
typedef enum bcmpc_fec_e {

    /*! Disable FEC. */
    BCMPC_FEC_NONE,

    /*! Clause 74 / BASE-R FEC. */
    BCMPC_FEC_CL74,

    /*! Clause 91 / RS 528 FEC */
    BCMPC_FEC_CL91,

    /*! RS 544 FEC. */
    BCMPC_FEC_RS544,

    /*! RS 272 FEC. */
    BCMPC_FEC_RS272,

    /*! Must be the last. */
    BCMPC_FEC_COUNT

} bcmpc_fec_t;

/*!
 * \brief Port config structure
 *
 * This data structure defines the attributes for a logical port configuration.
 */
typedef struct bcmpc_port_cfg_s {

    /*! Physical device port number. */
    bcmpc_pport_t pport;

    /*! Port operating mode. */
    bcmpc_port_opmode_t opmode;

    /*! Administrative enable. */
    int enable;

    /*! Autoneg profile ID. */
    uint32_t autoneg_profile_id;

    /*! Custom port operating mode profile ID. */
    uint32_t custom_opmode_id;

    /*! PFC profile ID. */
    uint32_t pfc_profile_id;

    /*! Encapsulation mode. */
    bcmpc_port_encap_t encap_mode;

    /*! Loopback mode. */
    bcmpc_port_lpbk_t lpbk_mode;

    /*! Pause MAC address. */
    bcmpc_mac_t pause_addr;

    /*! Tx pause ability. */
    int pause_tx;

    /*! Rx pause ability. */
    int pause_rx;

    /*! Max frame size. */
    uint32_t max_frame_size;

} bcmpc_port_cfg_t;

/*!
 * \brief Initalize port configuration data structure.
 *
 * \param [in] cfg Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_port_cfg_t_init(bcmpc_port_cfg_t *cfg);

/*!
 * \brief Add a logical port.
 *
 * Add a logical port with the given physical port and port configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_port_insert(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Delete a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_port_delete(int unit, bcmpc_lport_t lport);

/*!
 * \brief Set logical port configuration.
 *
 * Set the associated physical port and the port configuration to the given
 * logical port.
 *
 * If the \c pcfg->pport=BCMPC_INVALID_PPORT, to free the associated
 * physical port, and the pport is become available for other logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_port_set(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Get the physical port number for a logical port.
 *
 * The function will return the physical port number for the given logical
 * port \c lport when the logical port has been inserted with valid settings
 * according to the current PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical device port.
 *
 * \return The physical port number, or BCMPC_INVALID_PPORT when errors.
 */
extern bcmpc_pport_t
bcmpc_lport_to_pport(int unit, bcmpc_lport_t lport);

/*!
 * \brief Get the logical port number from the given physical port.
 *
 * The function will return the logical port number for the given physical
 * port \c pport when the logical port has been inserted with the valid
 * settings accroding to the current PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 *
 * \return The physical port number, or BCMPC_INVALID_LPORT when errors.
 */
extern bcmpc_lport_t
bcmpc_pport_to_lport(int unit, bcmpc_pport_t pport);

/*!
 * \brief Enable/Disable LAG failover.
 *
 * If LAG failover mode is enabled, the MAC will route all TX packets to the RX
 * path when the link is down. The state is called failover loopback.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical device port.
 * \param [in] enable 1 to enable, otherwise disable.
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmpc_port_failover_enable_set(int unit, bcmpc_lport_t lport,
                               bool enable);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * Change the port back to normal operation no matter the port is in failover
 * loopback state or not. \ref bcmpc_port_failover_enable_set.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical device port.
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmpc_port_failover_loopback_remove(int unit, bcmpc_lport_t lport);

#endif /* BCMPC_LPORT_H */
