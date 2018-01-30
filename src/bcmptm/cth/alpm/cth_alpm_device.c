/*! \file cth_alpm_device.c
 *
 * CTH ALPM Device specific
 *
 * This file contains the main function for CTH ALPM device specific info.
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "cth_alpm_device.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHALPM

/*******************************************************************************
 * Private variables
 */

static cth_alpm_device_info_t* cth_alpm_device_info[BCMDRD_CONFIG_MAX_UNITS] = {0};
static cth_alpm_driver_t* cth_alpm_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Populate device info into data structure
 *
 * \param [in] u Device u.
 * \param [out] device Device info data structure
 *
 * \return SHR_E_XXX.
 */
static int
cth_alpm_device_info_init(int u, cth_alpm_device_info_t *device)
{
    int rv = SHR_E_UNAVAIL;

    if (CTH_ALPM_DRV(u) && CTH_ALPM_DRV_EXEC(u, device_info_init)) {
        rv = CTH_ALPM_DRV_EXEC(u, device_info_init) (u, device);
    }
    return rv;
}
/*******************************************************************************
 * Public Functions
 */

const cth_alpm_device_info_t *
bcmptm_cth_alpm_device_info_get(int u)
{
    return CTH_ALPM_DEV(u);
}

int
bcmptm_cth_alpm_device_control_validate(int u,
                                        bcmptm_cth_alpm_control_t *control)
{
    int rv = SHR_E_UNAVAIL;

    if (CTH_ALPM_DRV(u) && CTH_ALPM_DRV_EXEC(u, control_validate)) {
        rv = CTH_ALPM_DRV_EXEC(u, control_validate) (u, control);
    }
    return rv;
}

int
bcmptm_cth_alpm_device_control_encode(int u,
                                      bcmptm_cth_alpm_control_t *control,
                                      cth_alpm_control_pt_t *control_pt)
{
    int rv = SHR_E_UNAVAIL;

    if (CTH_ALPM_DRV(u) && CTH_ALPM_DRV_EXEC(u, control_encode)) {
        rv = CTH_ALPM_DRV_EXEC(u, control_encode) (u, control, control_pt);
    }
    return rv;
}

void
bcmptm_cth_alpm_device_cleanup(int u)
{
    SHR_FREE(CTH_ALPM_DEV(u));
    SHR_FREE(CTH_ALPM_DRV(u));
}

int
bcmptm_cth_alpm_device_init(int u)
{
    int rv;

    SHR_FUNC_ENTER(u);
    SHR_ALLOC(CTH_ALPM_DEV(u), sizeof(cth_alpm_device_info_t),
              "CTH ALPM device info");
    SHR_NULL_CHECK(CTH_ALPM_DEV(u), SHR_E_MEMORY);
    sal_memset(CTH_ALPM_DEV(u), 0, sizeof(cth_alpm_device_info_t));

    SHR_ALLOC(CTH_ALPM_DRV(u), sizeof(cth_alpm_driver_t),
              "CTH ALPM driver");
    SHR_NULL_CHECK(CTH_ALPM_DRV(u), SHR_E_MEMORY);
    sal_memset(CTH_ALPM_DRV(u), 0, sizeof(cth_alpm_driver_t));

    rv = bcmptm_cth_alpm_driver_register(u, CTH_ALPM_DRV(u));
    if (rv == SHR_E_UNAVAIL) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(u, "CTH ALPM driver missing.\n")));
        SHR_RETURN_VAL_EXIT(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(cth_alpm_device_info_init(u, CTH_ALPM_DEV(u)));
exit:
    SHR_FUNC_EXIT();
}

