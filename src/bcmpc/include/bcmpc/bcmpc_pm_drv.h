/*! \file bcmpc_pm_drv.h
 *
 * BCMPC PM Driver Object.
 *
 * The PM driver is responsible for handling the PM-specific abilities and
 * limitions.
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

#ifndef BCMPC_PM_DRV_H
#define BCMPC_PM_DRV_H

#include <bcmpc/bcmpc_lport.h>


/*******************************************************************************
 * PM driver functions
 */

/*!
 * \name PM driver functions
 *
 * The function prototypes for \ref bcmpc_pm_drv_t.
 */

/*! \{ */

/*!
 * \brief PM operating mode validation function pointer type.
 *
 * This function is used to validate the PM-specific operating mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_mode PM mode.
 *
 * \retval SHR_E_NONE The \c pm_mode is valid.
 * \retval SHR_E_FAIL The \c pm_mode is invalid.
 */
typedef int (*bcmpc_pm_mode_validate_f)(int unit,
                                        bcmpc_pm_mode_t *pm_mode);

/*!
 * \brief Port mode validation function pointer type.
 *
 * This function is used to validate the PM-specific port speed value based on
 * the number of lanes which are used by the port.
 *
 * \param [in] unit Unit number.
 * \param [in] num_lane Number of lanes.
 * \param [in] speed Port speed value in Mbps.
 *
 * \retval SHR_E_NONE The \c speed is valid.
 * \retval SHR_E_FAIL The \c speed is invalid.
 */
typedef int (*bcmpc_pm_port_speed_validate_f)(int unit, uint32_t num_lanes,
                                              uint32_t speed);

/*!
 * \brief Get the VCO value from the given port speed.
 *
 * \param [in] unit Unit number.
 * \param [in] speed Speed value in Mbps.
 * \param [in] num_lanes Number of lanes.
 * \param [in] fec FEC type, \ref bcmpc_fec_t.
 * \param [in] speed Speed value in Mbps.
 * \param [out] vco VCO value in MHz.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_speed_to_vco_get_f)(int unit, uint32_t num_lanes, uint32_t speed,
                               bcmpc_fec_t fec, uint32_t *vco);

/*!
 * \brief Get the PLL divider value from the given VCO and reference clock.
 *
 * \param [in] unit Unit number.
 * \param [in] ref_clk Reference clock in KHz.
 * \param [in] vco VCO value in MHz.
 * \param [out] pll PLL divider value multiplys 1000. E.g. 72000 means 72.000,
 *                  and 73600 means 73.600.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_vco_to_pll_get_f)(int unit, uint32_t ref_clk, uint32_t vco,
                             uint32_t *pll);


/*! \} */


/*******************************************************************************
 * PM driver object
 */

/*!
 * \brief Port manager driver.
 *
 * The PortMacro (PM) driver is used to sustain the PM-specific abilities e.g.
 * the restriction of the speed mode, speed combination, speed-to-VCO mapping
 * and so on.
 *
 * Comparing to PMAC and PHYMOD driver, the PM driver does not provide any
 * hardware access operations but just use to get the PM-specific abilities and
 * limitations.
 */
typedef struct bcmpc_pm_drv_s {

    /*! PM operating mode validate. */
    bcmpc_pm_mode_validate_f pm_mode_validate;

    /*! Port speed validate. */
    bcmpc_pm_port_speed_validate_f speed_validate;

    /*! FIXME: SUPPORT
     *
     * speed_to_vco_get() and vco_to_pll_get() are for demonstration purposes;
     * they are not really invoked in the code yet.
 */
    /*! Speed to VCO get. */
    bcmpc_pm_speed_to_vco_get_f speed_to_vco_get;

    /*! VCO to PLL divider get. */
    bcmpc_pm_vco_to_pll_get_f vco_to_pll_get;

} bcmpc_pm_drv_t;


/*******************************************************************************
 * PM driver instance
 */

/*! PM4x10 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm4x10;

/*! PM4x25 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm4x25;

/*! PM8X50 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm8x50;

#endif /* BCMPC_PM_DRV_H */
