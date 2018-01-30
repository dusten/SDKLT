/*! \file bcmpc_lport_internal.h
 *
 * BCMPC LPORT Internal Functions.
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

#ifndef BCMPC_LPORT_INTERNAL_H
#define BCMPC_LPORT_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pm_internal.h>

/*!
 * \brief Bit definition for auto-negotiation protocols.
 *
 * This enum defines the bits of \c an_protocol in \ref bcmpc_autoneg_prof_t.
 */
typedef enum bcmpc_an_protocol_e {

    /*! Parallel detection (no exchange of information). */
    BCMPC_AN_PROT_PARALLEL_DETECT,

    /*! Enable IEEE 802.3 clause 28 auto-negotiation. */
    BCMPC_AN_PROT_CL28,

    /*! Enable IEEE 802.3 clause 37 auto-negotiation. */
    BCMPC_AN_PROT_CL37,

    /*! Enable IEEE 802.3 clause 73 auto-negotiation. */
    BCMPC_AN_PROT_CL73,

    /*! Enable SGMII auto-negotiation. */
    BCMPC_AN_PROT_SGMII,

    /*! Enable Broadcom clause 37 auto-negotiation. */
    BCMPC_AN_PROT_BAM37,

    /*! Enable Broadcom clause 73 auto-negotiation. */
    BCMPC_AN_PROT_BAM73,

    /*! Must be the last. */
    BCMPC_AN_PROT_COUNT

} bcmpc_an_protocol_t;

/*!
 * \brief Bit definition for Auto-negotiation advertised abilities.
 *
 * This enum defines the bits of \c adv_ability in \ref bcmpc_autoneg_prof_t.
 */
typedef enum bcmpc_an_adv_ability_e {

    /*! Advertise 10 Mbps copper mode (10BASE-T). */
    BCMPC_AN_ADV_10M_T,

    /*! Advertise 10 Mbps copper half-duplex mode (10BASE-T HDX). */
    BCMPC_AN_ADV_10M_T_HDX,

    /*! Advertise 100 Mbps copper mode (100BASE-T). */
    BCMPC_AN_ADV_100M_T,

    /*! Advertise 100 Mbps copper half-duplex mode (100BASE-T HDX). */
    BCMPC_AN_ADV_100M_T_HDX,

    /*! Advertise 100 Mbps fiber mode (100BASE-FX). */
    BCMPC_AN_ADV_100M_FX,

    /*! Advertise 1000 Mbps copper mode (1000BASE-T). */
    BCMPC_AN_ADV_1000M_T,

    /*! Advertise 1000 Mbps fiber mode (1000BASE-X). */
    BCMPC_AN_ADV_1000M_X,

    /*! Advertise 2.5 Gbps mode (HiGig only). */
    BCMPC_AN_ADV_2P5G,

    /*! Advertise 10 Gbps XAUI mode. */
    BCMPC_AN_ADV_10G,

    /*! Advertise 10 Gbps CX mode (10GBASE-CX4). */
    BCMPC_AN_ADV_10G_CX,

    /*! Advertise 10 Gbps CR mode (10GBASE-CR2/CR4). */
    BCMPC_AN_ADV_10G_CR,

    /*! Advertise 10 Gbps KR mode (10GBASE-KR/KR2). */
    BCMPC_AN_ADV_10G_KR,

    /*! Advertise 10 Gbps KX mode (10GBASE-KX4). */
    BCMPC_AN_ADV_10G_KX,

    /*! Advertise 12 Gbps mode (HiGig only). */
    BCMPC_AN_ADV_12G,

    /*! Advertise 13 Gbps mode (HiGig only). */
    BCMPC_AN_ADV_13G,

    /*! Advertise 20 Gbps XAUI/RXAUI mode. */
    BCMPC_AN_ADV_20G,

    /*! Advertise 20 Gbps CX mode (20GBASE-CX4). */
    BCMPC_AN_ADV_20G_CX,

    /*! Advertise 20 Gbps CR mode (20GBASE-CR2/CR4). */
    BCMPC_AN_ADV_20G_CR,

    /*! Advertise 20 Gbps KR mode (20GBASE-KR2). */
    BCMPC_AN_ADV_20G_KR,

    /*! Advertise 21 Gbps mode (HiGig only). */
    BCMPC_AN_ADV_21G,

    /*! Advertise 40 Gbps XLAUI mode. */
    BCMPC_AN_ADV_40G,

    /*! Advertise 40 Gbps CR mode (40GBASE-CR4). */
    BCMPC_AN_ADV_40G_CR,

    /*! Advertise 40 Gbps KR mode (40GBASE-KR4). */
    BCMPC_AN_ADV_40G_KR,

    /*! Advertise 42 Gbps mode (HiGig only). */
    BCMPC_AN_ADV_42G,

    /*! Advertise 100 Gbps CAUI mode. */
    BCMPC_AN_ADV_100G,

    /*! Advertise 100 Gbps CR mode (100GBASE-CR4/CR10). */
    BCMPC_AN_ADV_100G_CR,

    /*! Advertise 120 Gbps mode (HiGig only). */
    BCMPC_AN_ADV_120G,

    /*! Advertise 127 Gbps mode (HiGig only). */
    BCMPC_AN_ADV_127G,

    /*! Advertise asymmetric Tx PAUSE (symmetric if ADV_PAUSE_RX=1). */
    BCMPC_AN_ADV_PAUSE_TX,

    /*! Advertise symmetric if ADV_PAUSE_TX=1. */
    BCMPC_AN_ADV_PAUSE_RX,

    /*! Advertise 100 Mbps Energy Efficient Ethernet support. */
    BCMPC_AN_ADV_EEE_100M,

    /*! Advertise 1000 Mbps Energy Efficient Ethernet support. */
    BCMPC_AN_ADV_EEE_1G,

    /*! Must be the last. */
    BCMPC_AN_ADV_COUNT

} bcmpc_an_adv_ability_t;

/*!
 * \brief Auto-negotiation configuration profile.
 *
 * This structure is used to configure the auto-negotiation settings.
 * There are two fields in the structure; the first one describes the supported
 * auto-negotition methods, the second one describes the advertised abilities.
 *
 * The settings are in bit arrays. A bit is set when the corresponding protocol
 * or ability is enabled.
 * The bit definitions could be found by \ref bcmpc_an_protocol_t and
 * \ref bcmpc_an_adv_ability_t.
 */
typedef struct bcmpc_autoneg_prof_s {

    /*! Supported auto-negotiation protpcols. */
    SHR_BITDCLNAME(an_protocol, BCMPC_AN_PROT_COUNT);

    /*! Advertised abilities. */
    SHR_BITDCLNAME(adv_ability, BCMPC_AN_ADV_COUNT);

} bcmpc_autoneg_prof_t;

/*!
 * \name Autoneg profile access.
 *
 * Macro for configuring bcmpc_autoneg_prof_t fields.
 */

/*! \{ */

/*!
 * \brief Macro to set the supported protocol to the autoneg profile.
 *
 * \param [in] _prof Autoneg profile, \ref bcmpc_autoneg_prof_t.
 * \param [in] _prot Autoneg protocol, \ref bcmpc_an_protocol_t.
 */
#define BCMPC_AN_PROT_SET(_prof, _prot) SHR_BITSET((_prof)->an_protocol, _prot)

/*!
 * \brief Macro to clear the supported protocol to the autoneg profile.
 *
 * \param [in] _prof Autoneg profile, \ref bcmpc_autoneg_prof_t.
 * \param [in] _prot Autoneg protocol, \ref bcmpc_an_protocol_t.
 */
#define BCMPC_AN_PROT_CLR(_prof, _prot) SHR_BITCLR((_prof)->an_protocol, _prot)

/*!
 * \brief Macro to get the supported protocol to the autoneg profile.
 *
 * \param [in] _prof Autoneg profile, \ref bcmpc_autoneg_prof_t.
 * \param [in] _prot Autoneg protocol, \ref bcmpc_an_protocol_t.
 */
#define BCMPC_AN_PROT_GET(_prof, _prot) SHR_BITGET((_prof)->an_protocol, _prot)

/*!
 * \brief Macro to set the advertised ability to the autoneg profile.
 *
 * \param [in] _prof Autoneg profile, \ref bcmpc_autoneg_prof_t.
 * \param [in] _adv Advertised ability, \ref bcmpc_an_adv_ability_t.
 */
#define BCMPC_AN_ADV_SET(_prof, _adv) SHR_BITSET((_prof)->adv_ability, _adv)

/*!
 * \brief Macro to clear the advertised ability to the autoneg profile.
 *
 * \param [in] _prof Autoneg profile, \ref bcmpc_autoneg_prof_t.
 * \param [in] _adv Advertised ability, \ref bcmpc_an_adv_ability_t.
 */
#define BCMPC_AN_ADV_CLR(_prof, _adv) SHR_BITCLR((_prof)->adv_ability, _adv)

/*!
 * \brief Macro to get the advertised ability to the autoneg profile.
 *
 * \param [in] _prof Autoneg profile, \ref bcmpc_autoneg_prof_t.
 * \param [in] _adv Advertised ability, \ref bcmpc_an_adv_ability_t.
 */
#define BCMPC_AN_ADV_GET(_prof, _adv) SHR_BITGET((_prof)->adv_ability, _adv)

/*! \} */

/*!
 * \brief Priority-based flow control (PFC) configuration profile.
 *
 * This structure is used to configure the PFC settings.
 */
typedef struct bcmpc_pfc_prof_s {

    /*
     * PFC behavior control.
     */

    /*! Enable PFC transmit. */
    bool tx_enable;

    /*! Enable PFC receive. */
    bool rx_enable;

    /*! Enable PFC counters. */
    bool stats_enable;

    /*!
     * Threshold for pause timer to cause XOFF to be resent. Set to 0 to
     * disable this feature.
     */
    uint32_t refresh_timer;

    /*
     * PFC frame configure.
     */

    /*! Destination MAC address to use for the PFC frame. */
    bcmpmac_mac_t da;

    /*! EtherType to use for PFC frame. */
    uint32_t eth_type;

    /*! Opcode to use for PFC frame. */
    uint32_t opcode;

    /*! Time value sent in the Timer Field for classes in XOFF state. */
    uint32_t xoff_timer;

} bcmpc_pfc_prof_t;


/*!
 * \brief Initialize the physical to logical port map.
 *
 * The physical to logical port mapping could be got by traversing PC_PORTt.
 * In order to reduce the usage of traverse for getting the higher performance,
 * we cache the P2L map in the memory.
 *
 * When we need to get the physical port number for a given logical port, we
 * could easily do it by lookup the P2L map.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set for warm boot, otherwise for cold boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_p2l_map_init(int unit, bool warm);

/*!
 * \brief Get logical port number from the the cached P2L map.
 *
 * PC_PORTt could get the logical to physical port information.
 * This function is used to get the inverse information, i.e. physical to
 * logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The logical port number in the cached P2L map.
 */
extern bcmpc_lport_t
bcmpc_p2l_lport_get(int unit, bcmpc_pport_t pport);

/*!
 * \brief Get valid state of the P2L entry from the the cached P2L map.
 *
 * This function is used to get vaild state of a  physical to
 * logical port entry in the cached P2L map.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The valid state of the P2L entry in the cached P2L map.
 */
extern bool
bcmpc_p2l_valid_get(int unit, bcmpc_pport_t pport);

/*!
 * \brief Add a logical port.
 *
 * Add a logical port with the given physical port and port configuration.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_port_insert(int unit, bcmpc_lport_t lport,
                           bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Delete a logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_delete(int unit, bcmpc_lport_t lport);

/*!
 * \brief Set logical port configuration.
 *
 * Set the associated physical port and the port configuration to the given
 * logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_set(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Check the physical ports are free.
 *
 * Check the continuous \c pcnt physical ports from \c pport are free, i.e.
 * those physical ports are not occupied by any logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The base physical port number.
 * \param [out] pcnt The count of physical ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Some or all of the ports are in use.
 */
extern int
bcmpc_pport_free_check(int unit, bcmpc_pport_t pport, size_t pcnt);

/*!
 * \brief Deactivate a range of ports.
 *
 * Deactivate the logical ports which are using the physical ports in the given
 * range.
 *
 * \param [in] unit Unit number.
 * \param [in] pstart Starting physical port.
 * \param [in] pcnt Number of the physical ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_ports_deactivate(int unit, bcmpc_pport_t pstart, int pcnt);

/*!
 * \brief Activate a range of ports.
 *
 * Activate the logical ports which are using the physical ports in the given
 * range when the port configurations are vaild based on the current PM mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pstart Starting physical port.
 * \param [in] pcnt Number of the physical ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_ports_activate(int unit, bcmpc_pport_t pstart, int pcnt);

/*!
 * \brief Initalize auto-negotiation profile data structure.
 *
 * \param [in] prof Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_autoneg_prof_t_init(bcmpc_autoneg_prof_t *prof);

/*!
 * \brief Update the auto-negotiation profile.
 *
 * This function is called when we INSERT or UPDATE a auto-negotiation profile.
 *
 * When a profile is updated, we have to update the hardware for all ports
 * that are actively using the profile.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] prof_id auto-negotiation profile ID.
 * \param [in] prof auto-negotiation profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_autoneg_prof_internal_update(int unit, int prof_id,
                                   bcmpc_autoneg_prof_t *prof);

/*!
 * \brief Initalize PFC profile data structure.
 *
 * \param [in] prof Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pfc_prof_t_init(bcmpc_pfc_prof_t *prof);

/*!
 * \brief Update the PFC profile.
 *
 * This function is called when we INSERT or UPDATE a PFC profile.
 *
 * When a profile is updated, we have to update the hardware for all ports
 * that are actively using the profile.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] prof_id PFC profile ID.
 * \param [in] prof PFC profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pfc_prof_internal_update(int unit, int prof_id,
                               bcmpc_pfc_prof_t *prof);

#endif /* BCMPC_LPORT_INTERNAL_H */
