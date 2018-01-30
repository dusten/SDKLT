/*! \file bcmmgmt.h
 *
 * Top-level starting and stopping the SDK.
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

#ifndef BCMMGMT_H
#define BCMMGMT_H

#include <bcmmgmt/bcmmgmt_sysm.h>

/*!
 * \brief Perform default SDK start-up sequence.
 *
 * This function will start all SDK components and attach all devices.
 *
 * Prior to calling this function, devices must be created in the DRD
 * and any SDK configuration files must be loaded via \ref
 * bcmmgmt_core_config_load.
 *
 * No error is reported if \c conf_file is NULL or the file is not
 * found.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 * \param [in] conf_file Name of SDK configuration file.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components failed to start.
 * \retval SHR_E_PARTIAL One or more devices did failed to start.
 */
extern int
bcmmgmt_init(bool warm, const char *conf_file);

/*!
 * \brief Shut down the SDK System Manager.
 *
 * This function will detach all running devices and stop all
 * registered SDK components.
 *
 * \param [in] graceful Indicates if the shutdown should complete all
 *                      pending LT operations or simply shut down as
 *                      fast as possible.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmmgmt_shutdown(bool graceful);

/*!
 * \brief Initialize core utilities.
 *
 * This function initializes and instantiates various core utilities
 * of the SDK.
 *
 * This function must be called before the System Manager is started.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core utilities failed to initialize.
 */
extern int
bcmmgmt_core_init(void);

/*!
 * \brief Clean up core utilities.
 *
 * This function cleans up all initialization done by \ref
 * bcmmgmt_core_init.
 *
 * \return Always SHR_E_NONE.
 */
extern int
bcmmgmt_core_cleanup(void);

/*!
 * \brief Load and parse SDK configuration file.
 *
 * This file may be called multiple times with different file names.
 *
 * The SDK core utilities must be initialized via \ref
 * bcmmgmt_core_init before this function is called.
 *
 * \param [in] conf_file Name of SDK configuration file.
 *
 * \retval SHR_E_NONE Configuration file successfully parsed.
 * \retval SHR_E_INIT Core utilities not initialized.
 * \retval SHR_E_PARAM Invalid file name.
 * \retval SHR_E_NOT_FOUND Configuration file not found.
 * \retval SHR_E_CONFIG Error parsing configuration file.
 */
extern int
bcmmgmt_core_config_load(const char *conf_file);

/*!
 * \brief Start core components.
 *
 * This function will register all the specified SDK components with
 * the System Manager and start the device-independent part of the
 * components.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 * \param [in] comp_list List of component start functions.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components failed to start.
 */
extern int
bcmmgmt_core_start(bool warm, const bcmmgmt_comp_list_t *comp_list);

/*!
 * \brief Shut down all core components of the SDK.
 *
 * \param [in] graceful Indicates if the shutdown should complete all
 *                      pending LT operations or simply shut down as
 *                      fast as possible.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components did not start correctly.
 */
extern int
bcmmgmt_core_stop(bool graceful);

/*!
 * \brief Attach device.
 *
 * This function will register a device with the BCMLT API and the
 * System Manager, after which the System Manager will start the
 * device-specific parts of the installed SDK components.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL The device failed to attach.
 */
extern int
bcmmgmt_dev_attach(int unit, bool warm);

/*!
 * \brief Check if a device is attached.
 *
 * Check if a device has been attached using \ref bcmmgmt_dev_attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Device is attached.
 * \retval false Device is not attached.
 */
extern bool
bcmmgmt_dev_attached(int unit);

/*!
 * \brief Attach all devices.
 *
 * This function will call \ref bcmmgmt_dev_attach for all devices
 * that exist in the DRD.
 *
 * The System Manager must be started via \ref bcmmgmt_core_start
 * before calling this API.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARTIAL One or more devices did failed to attach.
 */
extern int
bcmmgmt_dev_attach_all(bool warm);

/*!
 * \brief Detach a device.
 *
 * Thsi function will stop the device-specific parts of the components
 * registered with the System Manager and unregister the device from
 * the BCMLT API.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL The device failed to detach.
 */
extern int
bcmmgmt_dev_detach(int unit);

/*!
 * \brief Detach all devices.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more devices failed to detach.
 */
extern int
bcmmgmt_dev_detach_all(void);

#endif /* BCMMGMT_H */
