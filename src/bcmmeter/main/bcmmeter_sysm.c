/*! \file bcmmeter_sysm.c
 *
 * Meter component interface to system manager.
 *
 * This file implements the Meter interface to system manager.
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
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmmeter/bcmmeter_sysm.h>
#include <bcmmeter/bcmmeter_fp_egr.h>
#include <bcmmeter/bcmmeter_fp_ing.h>
#include <bcmmeter/bcmmeter_sc.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMMETER_INIT

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Attach METER drivers.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmmeter_attach (int unit)
{
    const bcmlrd_map_t  *map = NULL;
    int                 rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, METER_FP_ING_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_ing_attach(unit));
    }

    rv = bcmlrd_map_get(unit, METER_FP_EGR_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_egr_attach(unit));
    }

    rv = bcmlrd_map_get(unit, METER_STORM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_sc_attach(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach METER drivers.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmmeter_detach (int unit)
{
    const bcmlrd_map_t  *map = NULL;
    int                 rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, METER_FP_ING_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
    SHR_IF_ERR_EXIT(bcmmeter_fp_ing_detach(unit));
    }

    rv = bcmlrd_map_get(unit, METER_FP_EGR_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_egr_detach(unit));
    }

    rv = bcmlrd_map_get(unit, METER_STORM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_sc_detach(unit));
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Initialize METER handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmmeter_init(int unit, bool warm)
{
    const bcmlrd_map_t  *map = NULL;
    int                 rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, METER_FP_ING_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_ing_init(unit, !warm));
    }

    rv = bcmlrd_map_get(unit, METER_FP_ING_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_ing_dev_config(unit));
    }

    rv = bcmlrd_map_get(unit, METER_FP_EGR_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_egr_init(unit, !warm));
    }

    rv = bcmlrd_map_get(unit, METER_FP_EGR_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_egr_dev_config(unit));
    }

    rv = bcmlrd_map_get(unit, METER_STORM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_sc_init(unit, !warm));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup METER handler function software data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmmeter_cleanup(int unit)
{
    const bcmlrd_map_t  *map = NULL;
    int                 rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, METER_FP_ING_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_ing_cleanup(unit));
    }

    rv = bcmlrd_map_get(unit, METER_FP_EGR_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_fp_egr_cleanup(unit));
    }

    rv = bcmlrd_map_get(unit, METER_STORM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmmeter_sc_cleanup(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

static shr_sysm_rv_t
sys_init (shr_sysm_categories_t instance_category,
          int unit,
          void *comp_data,
          bool warm,
          uint32_t *blocking_comp)
{
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

    if (bcmmeter_attach(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Initializes METER feature on this BCM unit.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
sys_config (shr_sysm_categories_t instance_category,
            int unit,
            void *comp_data,
            bool warm,
            uint32_t *blocking_comp)
{
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

    if (bcmmeter_init(unit, warm) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Shutsdown METER feature on this BCM unit.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
sys_shutdown(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool warm,
             uint32_t *blocking_component)
{
    if (bcmmeter_cleanup(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmmeter_detach(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Meter component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmmeter_start(void)
{
    shr_sysm_cb_func_set_t  funcs;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = sys_init;
    funcs.comp_config = sys_config;
    funcs.shutdown = sys_shutdown;
    funcs.stop = NULL;

    SHR_IF_ERR_EXIT(
        shr_sysm_register(BCMMGMT_METER_COMP_ID,
                          SHR_SYSM_CAT_UNIT,
                          &funcs,
                          NULL));
exit:
    SHR_FUNC_EXIT();
}
