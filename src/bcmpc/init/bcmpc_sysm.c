/*! \file bcmpc_sysm.c
 *
 * BCMPC interface to system manager.
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
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmpc/bcmpc_drv.h>
#include <bcmpc/bcmpc_sysm.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmpc/bcmpc_tm_internal.h>

/*******************************************************************************
 * Defines
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_INIT


/*******************************************************************************
 * SYSM callback functions
 */

static shr_sysm_rv_t
pc_sysm_init(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool warm,
             uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
pc_sysm_comp_config(shr_sysm_categories_t instance_category,
                    int unit,
                    void *comp_data,
                    bool warm,
                    uint32_t *blocking_component)
{
    bcmpc_drv_t *pc_drv = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Component Dependencies.
     */
    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_BLOCKED);
    }

    /*
     * PC initialization.
     */
    if (SHR_FAILURE(bcmpc_db_imm_init(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    if (SHR_FAILURE(bcmpc_imm_init(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    if (SHR_FAILURE(bcmpc_attach(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    if (pc_drv && pc_drv->topology_init) {
        if (SHR_FAILURE(pc_drv->topology_init(unit))) {
            SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
        }
    }

    if (SHR_FAILURE(bcmpc_p2l_map_init(unit, warm))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    if (pc_drv && pc_drv->dev_init) {
        if (SHR_FAILURE(pc_drv->dev_init(unit, warm))) {
            SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_DONE);

exit:
    SHR_FUNC_EXIT();
}

static shr_sysm_rv_t
pc_sysm_pre_config(shr_sysm_categories_t instance_category,
                   int unit,
                   void *comp_data,
                   bool warm,
                   uint32_t *blocking_component)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Component Dependencies.
     */
    if (!shr_sysm_is_comp_complete(BCMMGMT_LTP_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_LTP_COMP_ID;
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_BLOCKED);
    }

    /*
     * BCMLTP playback happens during pre-configure. At this point, the PC LT
     * configurations from the config file are completed, hence we are able
     * to inform the other components e.g. MMU to do some bulk update.
     */
    if (SHR_FAILURE(bcmpc_tm_update_now(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    /* Enable TM update for each port addition/deletion. */
    if (SHR_FAILURE(bcmpc_tm_port_based_update_set(unit, true))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_DONE);

exit:
    SHR_FUNC_EXIT();
}

static void
pc_sysm_run(shr_sysm_categories_t instance_category,
            int unit,
            void *comp_data)
{
    return;
}

static shr_sysm_rv_t
pc_sysm_stop(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool graceful,
             uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
pc_sysm_shutdown(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmpc_detach(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    if (SHR_FAILURE(bcmpc_db_imm_cleanup(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    /* Disable TM update for each port addition/deletion. */
    if (SHR_FAILURE(bcmpc_tm_port_based_update_set(unit, false))) {
        SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_ERROR);
    }

    SHR_RETURN_VAL_EXIT(SHR_SYSM_RV_DONE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMPC System manager init struct.
 */
static shr_sysm_cb_func_set_t pc_sysm_handler = {
    /*! Initialization. */
    .init        = pc_sysm_init,

    /*! Component configuration. */
    .comp_config = pc_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = pc_sysm_pre_config,

    /*! Run. */
    .run         = pc_sysm_run,

    /*! Stop. */
    .stop        = pc_sysm_stop,

    /*! Shutdown. */
    .shutdown    = pc_sysm_shutdown
};


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_PC_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &pc_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}

