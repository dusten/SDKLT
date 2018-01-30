/*! \file bcml2_learn_drv.c
 *
 * L2 Learn drivers.
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcml2/common/bcml2_learn_drv.h>
#include <bcml2/chip/bcml2_learn_drv_chip.h>
/******************************************************************************
* Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCML2_LEARN_DRV

/* L2 Learn driver database. */
static bcml2_learn_drv_t *bcml2_learn_drv[BCML2_DEV_NUM_MAX] = {NULL};

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_l2_learn_drv_attach },
static struct {
    bcml2_learn_drv_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};
/******************************************************************************
* Private functions
 */


/******************************************************************************
* Public functions
 */
/*!
 * \brief Attach L2 Learn drivers.
 *
 * Attach L2 Learn drivers.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to attach L2 Learn drivers.
 */
int
bcml2_learn_drv_attach(int unit)
{
    size_t size;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(
        (bcml2_learn_drv[unit] != NULL) ? SHR_E_INTERNAL : SHR_E_NONE);

    size = sizeof(bcml2_learn_drv_t);
    bcml2_learn_drv[unit] = sal_alloc(size, "bcml2 learn driver database");
    SHR_NULL_CHECK(bcml2_learn_drv[unit], SHR_E_MEMORY);
    sal_memset(bcml2_learn_drv[unit], 0, size);

    SHR_IF_ERR_EXIT(
        list_dev_attach[dev_type].attach(unit, bcml2_learn_drv[unit]));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach L2 Learn drivers.
 *
 * Detach L2 Learn drivers.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to detach L2 Learn drivers.
 */
int
bcml2_learn_drv_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (bcml2_learn_drv[unit] != NULL) {
    sal_free(bcml2_learn_drv[unit]);
    bcml2_learn_drv[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable L2 hardware learning.
 *
 * \param [in] unit Unit number.
 * \param [in] Enable Enable indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Not available.
 */
int
bcml2_learn_drv_hw_enable(int unit, bool enable)
{
    SHR_FUNC_ENTER(unit);

    if (bcml2_learn_drv[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (bcml2_learn_drv[unit]->hw_enable == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(bcml2_learn_drv[unit]->hw_enable(unit, enable));
    }

exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize L2 Learn driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
int
bcml2_learn_drv_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    if (bcml2_learn_drv[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (bcml2_learn_drv[unit]->init == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(bcml2_learn_drv[unit]->init(unit, warm));
    }

exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy L2 Learn driver database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcml2_learn_drv_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcml2_learn_drv[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (bcml2_learn_drv[unit]->cleanup == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(bcml2_learn_drv[unit]->cleanup(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/Disable L2 Learn interrupts.
 *
 * \param [in] unit Unit number.
 * \param [in] Enable Indicator to enable L2 Learn interrupts.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcml2_learn_drv_intr_enable(int unit, int enable)
{
    SHR_FUNC_ENTER(unit);

    if (bcml2_learn_drv[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (bcml2_learn_drv[unit]->intr_enable == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(bcml2_learn_drv[unit]->intr_enable(unit, enable));
    }

    exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse the L2 Learn cache table.
 *
 * Traverse all entries of L2 Learn cache table and invoke the callback.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
int
bcml2_learn_drv_cache_traverse(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcml2_learn_drv[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (bcml2_learn_drv[unit]->cache_traverse == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT(bcml2_learn_drv[unit]->cache_traverse(unit));
    }

    exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the L2 Learn cache entry.
 *
 * Delete the L2 Learn cache entry.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] l2addr L2 Mac address.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to delete the L2 Learn cache entry.
 */
int
bcml2_learn_drv_entry_delete(int unit,
                             uint32_t trans_id,
                             l2_learn_addr_t *l2addr)
{
    SHR_FUNC_ENTER(unit);

    if (bcml2_learn_drv[unit] == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (bcml2_learn_drv[unit]->entry_delete == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        SHR_IF_ERR_EXIT
            (bcml2_learn_drv[unit]->entry_delete(unit, trans_id, l2addr));
    }

exit:
    SHR_FUNC_EXIT();
}
