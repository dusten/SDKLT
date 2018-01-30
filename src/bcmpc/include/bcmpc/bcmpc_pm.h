/*! \file bcmpc_pm.h
 *
 * BCMPC PortMacro (PM) APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 *
 * The PortMacro library provides the functions to configure the whole PM, the
 * core within a PM or the lane within a PM.
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

#ifndef BCMPC_PM_H
#define BCMPC_PM_H

#include <shr/shr_bitop.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_pmgr.h>


/*******************************************************************************
 * Typedefs
 */

/*!
 * \brief Logical PM port resource.
 *
 * The structure describes the resource of a logical PM port which is derived by
 * lookup topology database.
 */
typedef struct bcmpc_pm_lport_rsrc_s {

    /*! Physical port bitmap of the logical PM port. */
    SHR_BITDCLNAME(ppbmp, BCMPC_NUM_PPORTS_PER_CHIP_MAX);

    /*! Lane bitmap of the logical PM port. */
    SHR_BITDCLNAME(lbmp, BCMPC_NUM_LANES_PER_PM_MAX);

    /*! Physical PM port of the logical PM port. */
    bcmpc_pport_t pm_pport;

    /*! Max speed. */
    uint32_t max_speed;

} bcmpc_pm_lport_rsrc_t;

/*!
 * \brief Port macro core identity.
 *
 * This structure is used to specifiy a port macro (PM) core.
 *
 * The PM core is constructed by the PM ID, the core index within the PM.
 */
typedef struct bcmpc_pm_core_s {

    /*! Port macro ID. */
    int pm_id;

    /*! Core index within port macro. */
    int core_idx;

} bcmpc_pm_core_t;

/*!
 * \brief Port macro core configuration.
 *
 * This structure is used to configure the board-related properties of a
 * port macro core.
 */
typedef struct bcmpc_pm_core_cfg_s {

    /*! Core clock in kHz. 0 means default value. */
    uint32_t core_clk;

    /*! Bits[3:0] is lane 0 mapping, bits[7:4] is lane 1 mapping, etc. */
    uint64_t tx_lane_map;

    /*! Bits[3:0] is lane 0 mapping, bits[7:4] is lane 1 mapping, etc. */
    uint64_t rx_lane_map;

    /*! Bit 0 flips lane 0, bit 1 flips lane 1, etc. */
    uint16_t tx_polarity_flip;

    /*! Bit 0 flips lane 0, bit 1 flips lane 1, etc. */
    uint16_t rx_polarity_flip;

} bcmpc_pm_core_cfg_t;

/*!
 * \brief Tx lane configuration profile.
 *
 * This structure includes the Tx lane configurations e.g. preemphasis and
 * amplitude control.
 */
typedef struct bcmpc_pm_tx_lane_prof_s {

    /*! Tx FIR pre cursor tap value. */
    int pre;

    /*! Tx FIR main cursor tap value. */
    int main;

    /*! Tx FIR post cursor tap value. */
    int post;

    /*! Tx FIR post2 cursor tap value. */
    int post2;

    /*! Tx FIR post3 cursor tap value. */
    int post3;

    /*! Tx amplitude control. */
    int amp;

    /*! Tx driver mode. */
    int drv_mode;

} bcmpc_pm_tx_lane_prof_t;

/*!
 * \brief Rx lane configuration profile.
 *
 * Shareable Rx lane configuration for receiver equalization, etc.
 *
 * These settings are normally controlled by the SerDes firmware, so
 * they must be explicitly set to override mode to take effect.
 */
typedef struct bcmpc_pm_rx_lane_prof_s {

    /*! DFE VGA control. */
    int rx_vga;

    /*! Override enable for DFE VGA control. */
    bool rx_vga_override;

    /*! DFE tap values. */
    int rx_def[BCMPC_NUM_RX_DFE_TAPS_MAX];

    /*! Override enable for DFE tap values. */
    bool rx_def_override[BCMPC_NUM_RX_DFE_TAPS_MAX];

    /*! Peaking filter control. */
    int rx_peak_filter;

    /*! Override enable for peaking filter control. */
    bool rx_peak_filter_override;

    /*! Low-frequency peaking filter control. */
    int rx_lf_peak_filter;

    /*! Override enable for low-frequency peaking filter control. */
    bool rx_lf_peak_filter_override;

    /*! Enable Rx adaptation. */
    bool rx_adap;

} bcmpc_pm_rx_lane_prof_t;

/*!
 * \brief Port macro lane identity.
 *
 * This structure is used to specifiy a port macro (PM) lane.
 *
 * The PM lane is constructed by the PM ID, the core index within the PM and the
 * lane number within the core.
 */
typedef struct bcmpc_pm_lane_s {

    /*! Port macro ID. */
    int pm_id;

    /*! Core index within port macro. */
    int core_idx;

    /*! Lane number within core. */
    int core_lane;

} bcmpc_pm_lane_t;

/*!
 * \brief Port macro lane configuration.
 *
 * This structure is used to configure the board-related properties of a
 * port macro lane. The actual lane configuration resides in a separate
 * tx and rx profile.
 *
 * Once a port operating mode has been established (forced or
 * auto-negotiated), the active port operating mode is matched against
 * the PORT_OPMODE array, and if a match is found, then the lane
 * configuration from the corresponding lane profiles is applied.
 *
 * For example, if the port operating mode is PC_PORT_OPMODE_10G_SFI
 * and PORT_OPMODE[3]=PC_PORT_OPMODE_10G_SFI, then the settings from
 * PC_PM_TX_LANE_PROFILE_ID[3] and PC_PM_RX_LANE_PROFILE_ID[3] will be
 * used.
 *
 * If no matching mode is found then the PORT_OPMODE array will be
 * searched for PC_PORT_OPMODE_ANY, and if found, the corresponding
 * lane profiles will be used.
 *
 * If there is no entry for PC_PORT_OPMODE_ANY either, then the
 * current/default lane settings will be used.
 */
typedef struct bcmpc_pm_lane_cfg_s {

    /*! Array of port operating modes. */
    bcmpc_port_opmode_t port_opmode[BCMPC_NUM_OPMODES_PER_UNIT_MAX];

    /*! Array of Tx lane profile IDs. */
    int tx_prof_id[BCMPC_NUM_OPMODES_PER_UNIT_MAX];

    /*! Array of Rx lane profile IDs. */
    int rx_prof_id[BCMPC_NUM_OPMODES_PER_UNIT_MAX];

} bcmpc_pm_lane_cfg_t;

/*!
 * \brief PM lane status.
 *
 * This structure is used to inspect the current lane status. The status will
 * include the Tx driver current settings, the Rx equalizer settings.
 */
typedef struct bcmpc_pm_lane_status_s {

    /*! Tx lane status. */
    bcmpc_pm_tx_lane_prof_t tx;

    /*! Rx lane status. */
    bcmpc_pm_rx_lane_prof_t rx;

} bcmpc_pm_lane_status_t;


/*******************************************************************************
 * Function declarations
 */

/*!
 * \brief Initalize PM mode data structure.
 *
 * \param [in] mode Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_mode_t_init(bcmpc_pm_mode_t *mode);

/*!
 * \brief Add the PM mode to the given PM instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Instance identity to create.
 * \param [in] mode PM mode.
 *
 * \return shr_error_t.
 */
extern int
bcmpc_pm_mode_insert(int unit, int pm_id, bcmpc_pm_mode_t *mode);

/*!
 * \brief Delete the PM mode configuration of the given PM instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Instance identity.
 *
 * \return shr_error_t.
 */
extern int
bcmpc_pm_mode_delete(int unit, int pm_id);

/*!
 * \brief Set the PM mode of the given PM instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Instance identity to set.
 * \param [in] mode PM mode.
 *
 * \return shr_error_t.
 */
extern int
bcmpc_pm_mode_set(int unit, int pm_id, bcmpc_pm_mode_t *mode);

/*!
 * \brief Get the PM mode of the given PM instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Instance identity to get.
 * \param [in] mode PM mode.
 *
 * \return shr_error_t.
 */
extern int
bcmpc_pm_mode_get(int unit, int pm_id, bcmpc_pm_mode_t *mode);

/*!
 * \brief Initalize PM core configuration structure.
 *
 * \param [in] ccfg Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_core_cfg_t_init(bcmpc_pm_core_cfg_t *ccfg);

/*!
 * \brief Initalize PM tx lane profile data structure.
 *
 * \param [in] prof Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_tx_lane_prof_t_init(bcmpc_pm_tx_lane_prof_t *prof);

/*!
 * \brief Initalize PM rx lane profile data structure.
 *
 * \param [in] prof Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_rx_lane_prof_t_init(bcmpc_pm_rx_lane_prof_t *prof);

/*!
 * \brief Initalize PM lane configuration data structure.
 *
 * \param [in] lcfg Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_lane_cfg_t_init(bcmpc_pm_lane_cfg_t *lcfg);

/*!
 * \brief Initalize PM lane status data structure.
 *
 * \param [in] st Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_lane_status_t_init(bcmpc_pm_lane_status_t *st);

/*!
 * \brief Get the PM lane status from Hardware.
 *
 * \param [in] unit Unit number
 * \param [in] pm_lane PM lane identity.
 * \param [out] st PM lane status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pm_lane_status_get(int unit, bcmpc_pm_lane_t *pm_lane,
                         bcmpc_pm_lane_status_t *st);

/*!
 * \brief Initalize logical PM port resource data structure.
 *
 * \param [in] prsrc Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_lport_rsrc_t_init(bcmpc_pm_lport_rsrc_t *prsrc);

/*!
 * \brief Get the logical PM port resource by a physical port.
 *
 * This function would lookup topology database to get the logical PM port
 * resource by the physical port number of a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The physical port number of a logical port.
 * \param [out] prsrc Logical PM port resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Failed to get the logical PM port resource with \c pport.
 */
extern int
bcmpc_pm_lport_rsrc_get(int unit, bcmpc_pport_t pport,
                        bcmpc_pm_lport_rsrc_t *prsrc);

#endif /* BCMPC_PM_H */
