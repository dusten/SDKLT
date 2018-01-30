/*! \file bcmpc_util_internal.h
 *
 *  BCMPC internal utilites.
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

#ifndef BCMPC_UTIL_INTERNAL_H
#define BCMPC_UTIL_INTERNAL_H

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_drv.h>


/*!
 * \brief Compare the data of two same-type array variables.
 *
 * \c _var1 and \c _var2 are two array variables with the same data type.
 *
 * \param [in] _var1 The first variable.
 * \param [in] _var2 The second variable.
 *
 * \retval 1 The two arrays are identical.
 * \retval 0 The two arrays are different.
 */
#define BCMPC_ARRAY_EQUAL(_var1, _var2) \
    (sal_memcmp(_var1, _var2, sizeof(_var1)) == 0)

/*!
 * \brief Compare the field value in two same-type variables.
 *
 * \c _var1 and \c _var2 are two variables with the same data type.
 * \c _f is a field name in the data structure.
 *
 * \param [in] _var1 The first variable.
 * \param [in] _var2 The second variable.
 * \param [in] _f field name.
 *
 * \retval 1 The value of field \c _f is same in \c _var1 and \c _var2.
 * \retval 0 The value of \c _f is different.
 */
#define BCMPC_FIELD_VAL_EQUAL(_var1, _var2, _f) \
    ((_var1)->_f == (_var2)->_f)

/*!
 * \brief PM information.
 *
 * The structure describes the resource of a PM which is derived by
 * lookup topology database.
 */
typedef struct bcmpc_pm_info_s {

    /*! Base physical port number. */
    bcmpc_pport_t base_pport;

    /*! Type properities. */
    bcmpc_topo_type_t prop;

} bcmpc_pm_info_t;

/*!
 * \brief Set the PC driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv PC driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_drv_set(int unit, bcmpc_drv_t *drv);

/*!
 * \brief Get the PC driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv PC driver to get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_drv_get(int unit, bcmpc_drv_t **drv);

/*!
 * \brief Initalize PM resource data structure.
 *
 * \param [in] pm_info Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_info_t_init(bcmpc_pm_info_t *pm_info);

/*!
 * \brief Get the PM resource by a PM ID.
 *
 * This function would lookup topology database to get the PM
 * resource by the PM ID.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [out] pm_info PM resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Failed to get the logical PM port resource by \c pm_id.
 */
extern int
bcmpc_pm_info_get(int unit, int pm_id, bcmpc_pm_info_t *pm_info);

/*!
 * \brief Get the core reference clock for the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] ref_clk Core reference clock in KHz.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the core clock.
 */
extern int
bcmpc_pm_core_ref_clk_get(int unit, bcmpc_pport_t pport, int *ref_clk);


/*!
 * \brief Get the linkscan mode for the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \return Linkscan mode of the given port.
 */
extern bcmpc_ls_mode_t
bcmpc_linkscan_mode_get(int unit, bcmpc_lport_t lport);

#endif /* BCMPC_UTIL_INTERNAL_H */
