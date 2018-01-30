/*! \file bcmltp_start.c
 *
 * BCMLTP System Manager interface.
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

#include <sal/sal_assert.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltp/bcmltp_player.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltp/bcmltp_sysm.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module - use BCMCFG */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/* Units available. */
static bool available_unit[BCMDRD_CONFIG_MAX_UNITS];

/* Units initialized. */
static bool initialized_unit[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Playback BCMCFG data.
 *
 * Playback BCMCFG data once all units are available.
 *
 * \param [in]  unit                    The device unit number.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error.
 */
static shr_sysm_rv_t
bcmltp_sysm_play(int unit)
{
    shr_sysm_rv_t sysm_rv = SHR_SYSM_RV_DONE;
    size_t i;
    bool runnable = true;

    initialized_unit[unit] = true;

    /* See if all available units are initialized. */
    for (i = 0; i < COUNTOF(available_unit); i++) {
        if (available_unit[i] != initialized_unit[i]) {
            runnable = false;
            break;
        }
    }

    if (runnable) {
        int rv;

        /* All units are available, so play back configuration. */
        rv = bcmltp_play();
        if (SHR_FAILURE(rv) && rv != SHR_E_UNAVAIL) {
            sysm_rv = SHR_SYSM_RV_ERROR;
        }
    }

    return sysm_rv;
}

/*!
 * \brief Scan available units.
 *
 * Scan for avalable units, so bcmltp_sysm_play() can determine when
 * all units are available for playback.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error.
 */
static shr_sysm_rv_t
bcmltp_sysm_scan(void)
{
    size_t unit;

    for (unit = 0; unit < COUNTOF(available_unit); unit++) {
        if (bcmdrd_dev_exists(unit)) {
            available_unit[unit] = true;
        }
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLTP initialization function.
 *
 * BCMLTP is requested to initialize.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_init(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    shr_sysm_rv_t sysm_rv = SHR_SYSM_RV_DONE;

    if (instance_category == SHR_SYSM_CAT_GENERIC) {
        sysm_rv = bcmltp_sysm_scan();
    } else if (instance_category == SHR_SYSM_CAT_UNIT) {
        if (!shr_sysm_is_comp_complete(BCMMGMT_TRM_COMP_ID,
                                       instance_category,
                                       unit)) {
            *blocking_component = BCMMGMT_TRM_COMP_ID;
            sysm_rv = SHR_SYSM_RV_BLOCKED;
        }
    }

    return sysm_rv;
}

/*!
 * \brief BCMLTP config function.
 *
 * BCMLTP is requested to configure.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_comp_config(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLTP pre_config function.
 *
 * BCMLTP is requested to pre-configure.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_pre_config(shr_sysm_categories_t instance_category,
                       int unit,
                       void *comp_data,
                       bool warm,
                       uint32_t *blocking_component)
{
    shr_sysm_rv_t sysm_rv;

    if (instance_category == SHR_SYSM_CAT_UNIT) {
        sysm_rv = bcmltp_sysm_play(unit);
    } else {
        sysm_rv = SHR_SYSM_RV_DONE;
    }

    return sysm_rv;
}

/*!
 * \brief BCMLTP run function
 *
 * BCMLTP is requested to run.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 *
 * \return none.
 */
static void
bcmltp_sysm_run(shr_sysm_categories_t instance_category,
                int unit,
                void *comp_data)
{
}

/*!
 * \brief BCMLTP stop function.
 *
 * BCMLTP is requested to stopped.
 *
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  graceful                Indicates abrupt or graceful stop.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_stop(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLTP shutdown function.
 *
 * BCMLTP is requested to being shutdown completely.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_shutdown(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data,
                     bool warm,
                     uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLTP System manager init struct.
 */
static shr_sysm_cb_func_set_t bcmltp_sysm_handler = {
    /*! Initialization. */
    .init        = bcmltp_sysm_init,

    /*! Component configuration. */
    .comp_config = bcmltp_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = bcmltp_sysm_pre_config,

    /*! Run. */
    .run         = bcmltp_sysm_run,

    /*! Stop. */
    .stop        = bcmltp_sysm_stop,

    /*! Shutdown. */
    .shutdown    = bcmltp_sysm_shutdown
};

/*******************************************************************************
 * Public functions
 */

/*
 * Called by system manager to start BCMLTP.
 *
 * Both generic events and unit events are used.
 */
int
bcmltp_start(void)
{
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {

        rv = shr_sysm_register(BCMMGMT_LTP_COMP_ID,
                               SHR_SYSM_CAT_GENERIC,
                               &bcmltp_sysm_handler,
                               NULL);

        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        rv = shr_sysm_register(BCMMGMT_LTP_COMP_ID,
                               SHR_SYSM_CAT_UNIT,
                               &bcmltp_sysm_handler,
                               NULL);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

    } while (0);

    SHR_FUNC_EXIT();
}
