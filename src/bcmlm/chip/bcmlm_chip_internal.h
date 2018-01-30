/*! \file bcmlm_chip_internal.h
 *
 * Link Manager per-chip driver definitions.
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

#ifndef BCMLM_CHIP_INTERNAL_H
#define BCMLM_CHIP_INTERNAL_H

#include <bcmbd/bcmbd_intr.h>
#include <bcmlm/bcmlm_types.h>

/*******************************************************************************
 * Typedefs
 */

/*!
 * \brief Initialize hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in initializing hardware
 *         linkscan.
 */
typedef int (*ls_hw_init_f)(int unit);

/*!
 * \brief Configure hardware linkscan.
 *
 * \param [in] unit Unit number.
 * \param [in] pbm Bitmap of ports to be enabled with hardware linkscan.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in configuring hardware
 *         linkscan.
 */
typedef int (*ls_hw_config_f)(int unit, bcmdrd_pbmp_t pbm);

/*!
 * \brief Get link state from hardware linkscan.
 *
 * \param [in] unit Unit number.
 * \param [out] pbm Bitmap of ports in link up state.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in getting link state.
 */
typedef int (*ls_hw_link_get_f)(int unit, bcmdrd_pbmp_t *pbm);

/*!
 *  \brief Clear hardware linkscan interrupt.
 *
 *  \param [in] unit Unit number.
 *
 *  \retval SHR_E_NONE No error.
 */
typedef int (*ls_hw_link_intr_clear_f)(int unit);

/*!
 * \brief Register callback for hardware linkscan interrupt.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_func Callback function.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in registering callback.
 */
typedef int (*ls_hw_link_intr_func_set_f)(int unit, bcmbd_intr_f intr_func);

/*!
 * \brief Stop hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT Failed to disable hardware linkscan.
 */
typedef int (*ls_hw_scan_stop_f)(int unit);

/*!
 * \brief Start hardware linkscan.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*ls_hw_scan_start_f)(int unit);

/*!
 * \brief Fault checking in linkscan is enabled or not.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Enabled or disabled.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in getting device info.
 */
typedef int (*ls_sw_fault_check_enabled_f)(int unit, int *enabled);

/*!
 * \brief Check the port is valid and eligible for link scan.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors, otherwise port is invalid.
 */
typedef int (*ls_sw_port_validate_f)(int unit, shr_port_t port);

/*!
 * \brief Link Manager driver object
 *
 * The Link Manager driver object is used to initialize and access hardware
 * linkscan or firmware linkscan for a switch device, and access MAC/PHY via
 * Port Control for software linkscan or port update due to link state change.
 */
typedef struct bcmlm_drv_s {

    /*! Initialize hardware linkscan */
    ls_hw_init_f hw_init;

    /*! Configure hardware linkscan */
    ls_hw_config_f hw_config;

    /*! Get link status from hardware linkscan */
    ls_hw_link_get_f hw_link_get;

    /*! Clear interrupt */
    ls_hw_link_intr_clear_f hw_intr_clear;

    /*! Register interrupt callback */
    ls_hw_link_intr_func_set_f hw_intr_cb_set;

    /*! Stop hardware linkscan */
    ls_hw_scan_stop_f hw_scan_stop;

    /*! Start hardware linkscan */
    ls_hw_scan_stop_f hw_scan_start;

    /*! Fault check is enabled or not */
    ls_sw_fault_check_enabled_f fault_check_enabled;

    /*! Check the port is valid and eligible for link scan */
    ls_sw_port_validate_f port_validate;

} bcmlm_drv_t;


/*******************************************************************************
 * Internal public Functions
 */
/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_lm_drv_attach(bcmlm_drv_t *drv);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

#endif /* BCMLM_CHIP_INTERNAL_H */
