/*! \file bcmecmp_sysm.c
 *
 * ECMP interface functions to system manager.
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
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmecmp/bcmecmp_dev.h>
#include <bcmecmp/bcmecmp_sysm.h>
#include <bcmecmp/bcmecmp_db_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Local definitions
 */
bcmecmp_control_t *ecmp_ctrl[BCMECMP_NUM_UNITS_MAX] = {0};

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Allocates ECMP control structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_resources_alloc(int unit)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_resources_alloc.\n")));

    if (!BCMECMP_CTRL(unit)) {
        BCMECMP_ALLOC(
            BCMECMP_CTRL(unit),
            sizeof(bcmecmp_control_t),
            "bcmecmpSysEcmpCtrl");
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Free ECMP control structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_resources_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_resources_free.\n")));

    if (BCMECMP_CTRL(unit)) {
        BCMECMP_FREE(BCMECMP_CTRL(unit));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ECMP handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_cleanup(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_cleanup.\n")));

    if (!BCMECMP_CTRL(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_dev_cleanup(unit, warm));
    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_detach(unit));
    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_resources_free(unit));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ECMP handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmecmp_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_init.\n")));

    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_resources_alloc(unit));
    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_attach(unit));
    SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_dev_init(unit, warm));

    BCMECMP_CTRL(unit)->init = TRUE;

    exit:
        if (SHR_FUNC_ERR()) {
            /*
             * Perform cleanup and return the actual error value for all other
             * error conditions.
             */
            bcmecmp_cleanup(unit, warm);

            /*
             * Unavailable error code returned by bcmlrd_map_get() function
             * for ECMP LTs, when ECMP support is excluded during compilation.
             * Do not treat this condition as an error and set return value to
             * SHR_E_NONE.
             */
            if (SHR_FUNC_VAL_IS(SHR_E_UNAVAIL)) {
                _func_rv = SHR_E_NONE;
            }
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes ECMP component on this BCM unit.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start status.
 * \param [out] blocking_comp Pointer to the name of the blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 * \retval SHR_SYSM_RV_BLOCKED On being blocked by another component.
 */
static shr_sysm_rv_t
bcmecmp_unit_init(shr_sysm_categories_t instance_category,
                  int unit,
                  void *comp_data,
                  bool warm,
                  uint32_t *blocking_comp)
{
    shr_error_t rv; /* Function return value. */

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_unit_init.\n")));

    /* ECMP depends on following components initialization to be completed. */
    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_DRD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_DRD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_LTD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_LTD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_BD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_BD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_PTM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    /* Initialization ECMP component. */
    rv = bcmecmp_init(unit, warm);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "bcmecmp_init() failed rv=%s.\n"),
                  shr_errmsg(rv)));
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Configures ECMP resolution mode for this unit.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start status.
 * \param [out] blocking_comp Pointer to the name of the blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 * \retval SHR_SYSM_RV_BLOCKED On being blocked by another component.
 */
static shr_sysm_rv_t
bcmecmp_unit_config(shr_sysm_categories_t instance_category,
                    int unit,
                    void *comp_data,
                    bool warm,
                    uint32_t *blocking_comp)
{
    shr_error_t rv = SHR_E_INTERNAL; /* Function return value. */

    /* ECMP depends on following components initialization completion. */
    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_DRD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_DRD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_LTD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_LTD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_BD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_BD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_PTM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_TRUNK_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_TRUNK_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    /*
     * Program device ECMP resolution mode in hardware register, when booting
     * up in cold boot mode. Skip this step when booting up in warm boot mode,
     * as it's expected that the device was already programmed to desired
     * ECMP_MODE during previous cold boot initialization sequence.
     */
    if (BCMECMP_FNCALL_CHECK(unit, mode_config) && !warm) {
        rv = BCMECMP_FNCALL_EXEC(unit, mode_config)(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ECMP mode config failed=%s.\n"),
                      shr_errmsg(rv)));
            bcmecmp_cleanup(unit, warm);
            return SHR_SYSM_RV_ERROR;
        }
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Shutsdown ECMP component on this BCM unit.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start status.
 * \param [out] blocking_comp Pointer to the name of the blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 * \retval SHR_SYSM_RV_BLOCKED On being blocked by another component.
 */
static shr_sysm_rv_t
bcmecmp_unit_shutdown(shr_sysm_categories_t instance_category,
                      int unit,
                      void *comp_data,
                      bool warm,
                      uint32_t *blocking_comp)
{
    shr_error_t rv; /* Function return value. */

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_unit_shutdown.\n")));

    /*
     * Following dependencies ensure that TRM, LTM are no longer sending
     * commands to ECMP when we attempt to do ECMP component stop.
     */
    if (!shr_sysm_is_comp_complete(BCMMGMT_TRM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_TRM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_LTM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_comp = BCMMGMT_LTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    /* Cleanup resources allcoated for ECMP component. */
    rv = bcmecmp_cleanup(unit, warm);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "bcmecmp_cleanup() failed=%s.\n"),
                  shr_errmsg(rv)));
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief ECMP component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmecmp_start(void)
{
    shr_sysm_cb_func_set_t  funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    sal_memset(&funcs, 0, sizeof(funcs));

    /* Register ECMP component initialization function. */
    funcs.init = bcmecmp_unit_init;

    /* Register ECMP resolution mode hardware configuration function. */
    funcs.comp_config = bcmecmp_unit_config;

    /* Register ECMP component shutdown/cleanup function. */
    funcs.shutdown = bcmecmp_unit_shutdown;

    /* Register ECMP component with System Manager as per-UNIT instance type. */
    SHR_IF_ERR_EXIT(
        shr_sysm_register(BCMMGMT_ECMP_COMP_ID,
                          SHR_SYSM_CAT_UNIT,
                          &funcs,
                          NULL));
    exit:
        SHR_FUNC_EXIT();
}
