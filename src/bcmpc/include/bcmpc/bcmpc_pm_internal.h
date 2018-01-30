/*! \file bcmpc_pm_internal.h
 *
 * BCMPC PM Internal Functions.
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

#ifndef BCMPC_PM_INTERNAL_H
#define BCMPC_PM_INTERNAL_H

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm.h>


/*******************************************************************************
 * Typedefs
 */

/*!
 * \name Lane Configuration Encoding
 *
 * Some lane configurations are encoded together for a core, such as tx/rx
 * swap and polarity swap setting.
 */

/*! \{ */

/*!
 * \brief Get the configuration value of the given lane.
 *
 * This macro is used to get the specific lane configuration from the whole core
 * setting.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 * \param [in] _bits The bits of data per lane.
 *
 * \return The lane configuration value for \c _lane.
 */
#define BCMPC_LANE_CFG_GET(_core, _lane, _bits) \
            ((_core >> (_lane * _bits)) & ((_bits << 1) - 1))

/*!
 * \brief Set the configuration value of the given lane.
 *
 * This macro is used to set the specific lane configuration to the whole core
 * setting.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 * \param [in] _bits The bits of data per lane.
 * \param [in] _val The value of the lane configuration to set.
 *
 * \return The lane configuration value for \c _lane.
 */
#define BCMPC_LANE_CFG_SET(_core, _lane, _bits, _val) \
            (_core |= ((_val & ((_bits << 1) - 1)) << (_lane * _bits)))

/*! \} */

/*!
 * \name Lane Map Encoding
 *
 * Macro for lane map encoding/decoding.
 */

/*! \{ */

/*!
 * \brief The bits count for eahc lane in tx/rx lane map,
 * \ref bcmpc_pm_core_cfg_t.
 */
#define BCMPC_LANE_MAP_BITS_PER_LANE 4

/*!
 * \brief Get the lane map value of the given lane.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 *
 * \return The lane map value for \c _lane.
 */
#define BCMPC_LANE_MAP_GET(_core, _lane) \
            BCMPC_LANE_CFG_GET(_core, _lane, BCMPC_LANE_MAP_BITS_PER_LANE)

/*!
 * \brief Set the lane map value of the given lane.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 * \param [in] _val The value of the lane map to set.
 */
#define BCMPC_LANE_MAP_SET(_core, _lane, _val) \
            BCMPC_LANE_CFG_SET(_core, _lane, BCMPC_LANE_MAP_BITS_PER_LANE, _val)

/*! \} */


/*******************************************************************************
 * Function declarations
 */

/*!
 * \brief Get the Tx lane profile for the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_lane PM lane identity.
 * \param [in] opmode Port operating mode.
 * \param [out] tx_prof Tx lane profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Tx lane profile is not found.
 */
extern int
bcmpc_pm_tx_lane_prof_get(int unit, bcmpc_pm_lane_t *pm_lane,
                          bcmpc_port_opmode_t opmode,
                          bcmpc_pm_tx_lane_prof_t *tx_prof);

/*!
 * \brief Get the Rx lane profile for the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_lane PM lane identity.
 * \param [in] opmode Port operating mode.
 * \param [out] rx_prof Rx lane profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Rx lane profile is not found.
 */
extern int
bcmpc_pm_rx_lane_prof_get(int unit, bcmpc_pm_lane_t *pm_lane,
                          bcmpc_port_opmode_t opmode,
                          bcmpc_pm_rx_lane_prof_t *rx_prof);

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
 * \param [in] do_core_init 1 to do core initialization, otherwise 0.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pm_phy_init(int unit, bcmpc_pport_t pport, int do_core_init);

/*!
 * \brief Get the core resource by a physical port.
 *
 * The core resource is derived from topology database and the current PM mode.
 *
 * For non-aggregated PM or run an aggregated PM in aggregation mode, the output
 * \c core_info is same as the output of \ref bcmpc_pm_info_get().
 *
 * When run an aggregated PM as several independent sub-PMs, this function will
 * return the specific sub-PM info, \c core_info, where the given pport belongs
 * to.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The physical port number of a logical port.
 * \param [out] core_info PM core resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Failed to get the logical PM port resource with \c pport.
 */
extern int
bcmpc_pm_core_rsrc_get(int unit, bcmpc_pport_t pport,
                       bcmpc_pm_info_t *core_info);

/*!
 * \brief Set the PM mode configuration.
 *
 * Set the PM mode configuration to the given PM.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [in] mode PM mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_pm_mode_set(int unit, int pm_id, bcmpc_pm_mode_t *mode);

/*!
 * \brief Delete the PM mode configuration.
 *
 * Delete the PM mode configuration of the given PM.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_pm_mode_delete(int unit, int pm_id);

/*!
 * \brief Update the lane configuration.
 *
 * This function is called when we INSERT or UPDATE a lane configuration.
 *
 * When we change the active profile ID for a lane, it will lead to a hardware
 * update.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_lane PM lane.
 * \param [in] lcfg Lane configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_lane_internal_update(int unit, bcmpc_pm_lane_t *pm_lane,
                              bcmpc_pm_lane_cfg_t *lcfg);

/*!
 * \brief Update the Tx lane profile.
 *
 * This function is called when we INSERT or UPDATE a Tx lane profile.
 *
 * When a profile is updated, we have to update the hardware for all lanes
 * that are actively using the profile.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] prof_id Tx lane profile ID.
 * \param [in] prof Tx lane profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_tx_lane_prof_internal_update(int unit, int prof_id,
                                      bcmpc_pm_tx_lane_prof_t *prof);

/*!
 * \brief Update the Rx lane profile.
 *
 * This function is called when we INSERT or UPDATE a Rx lane profile.
 *
 * When a profile is updated, we have to update the hardware for all lanes
 * that are actively using the profile.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] prof_id Rx lane profile ID.
 * \param [in] prof Rx lane profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_rx_lane_prof_internal_update(int unit, int prof_id,
                                      bcmpc_pm_rx_lane_prof_t *prof);

/*!
 * \brief Staging the PM mode configuration.
 *
 * Stage the new PM configurations to make the PC internal sub-components
 * know we are currently updating the PM mode and be able to get the
 * the port attribues e.g. the number of lanes and max speed accroding to
 * the new PM mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [in] pm_mode PM mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_mode_stage_set(int unit, int pm_id, bcmpc_pm_mode_t *pm_mode);

/*!
 * \brief Get the staged PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [out] pm_mode PM mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_mode_stage_get(int unit, int pm_id, bcmpc_pm_mode_t **pm_mode);

/*!
 * \brief Delete the staged PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_mode_stage_delete(int unit, int pm_id);

#endif /* BCMPC_PM_INTERNAL_H */
