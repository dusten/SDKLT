/*! \file bcmptm_cth_alpm_be_sysm.c
 *
 * Interface functions to the system manager
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
#include <sal/sal_assert.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "bcmptm/bcmptm_internal.h"
#include "bcmptm/bcmptm_chip_internal.h"
#include "bcmptm/bcmptm_cth_alpm_be_internal.h"
#include "bcmptm/bcmptm_cth_alpm_be_sysm.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define IF_BLOCK_RETURN(cat, unit, comp_list, block_comp)                   \
    if (is_blocked(cat, unit, comp_list, COUNTOF(comp_list), block_comp)) { \
        return SHR_SYSM_RV_BLOCKED;                                         \
    }

/*******************************************************************************
 * Private variables
 */
static bcmmgmt_sysm_comp_id_t init_comp_list[] = {
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_DRD_COMP_ID,
        BCMMGMT_LTD_COMP_ID,
        BCMMGMT_IMM_FE_COMP_ID,
        BCMMGMT_EVM_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_FCL_COMP_ID
    };
static bcmmgmt_sysm_comp_id_t cleanup_comp_list[] = {
        BCMMGMT_TRM_COMP_ID,
        BCMMGMT_LTM_COMP_ID
    };

/*******************************************************************************
 * Private Functions
 */
static bool
is_blocked(shr_sysm_categories_t instance_category, int unit,
           bcmmgmt_sysm_comp_id_t *comp_list,
           uint8_t count,
           uint32_t *blocking_component)
{
    int i;
    for (i = 0; i < count; i++) {
        if (!shr_sysm_is_comp_complete(comp_list[i], instance_category, unit)) {
            *blocking_component = comp_list[i];
            return TRUE;
        }
    }
    return FALSE;
}

/* Phase: init
 * Expect Devices will be attached, BD, DRD, LRD will happen here.
 * We can do config-independent init here
 */
static shr_sysm_rv_t
cth_alpm_be_unit_init(shr_sysm_categories_t instance_category,
                      int unit,
                      void *comp_data,
                      bool warm,
                      uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);
    return SHR_SYSM_RV_DONE;
}

/* Phase: comp_config
 */
static shr_sysm_rv_t
cth_alpm_be_unit_comp_config(shr_sysm_categories_t instance_category,
                             int unit,
                             void *comp_data,
                             bool warm,
                             uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);

    if (!warm) {
        int tmp_rv;
        tmp_rv = bcmptm_cth_alpm_be_init(unit, warm);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "bcmptm_cth_alpm_be_init FAILED(rv = %0d)\n"),
                 tmp_rv));
            return SHR_SYSM_RV_ERROR;
        }
    }
    return SHR_SYSM_RV_DONE;
}

/* Phase: pre_config
 * Expect Cfg writes.
 * unit-dependent config will happen with LT writes
 */
static shr_sysm_rv_t
cth_alpm_be_unit_pre_config(shr_sysm_categories_t instance_category,
                            int unit,
                            void *comp_data,
                            bool warm,
                            uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    init_comp_list, blocking_component);
    if (warm) {
        int tmp_rv;
        tmp_rv = bcmptm_cth_alpm_be_init(unit, warm);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "bcmptm_cth_alpm_be_init FAILED(rv = %0d)\n"),
                 tmp_rv));
            return SHR_SYSM_RV_ERROR;
        }
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: run
 * 1. Start all threads as configured.
 */
static void
cth_alpm_be_unit_run(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data)
{
    return;
}

/* Phase: stop
 */
static shr_sysm_rv_t
cth_alpm_be_unit_stop(shr_sysm_categories_t instance_category,
                      int unit,
                      void *comp_data,
                      bool graceful,
                      uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    cleanup_comp_list, blocking_component);

    return SHR_SYSM_RV_DONE;
}

/* Phase: shutdown */
static shr_sysm_rv_t
cth_alpm_be_unit_shutdown(shr_sysm_categories_t instance_category,
                          int unit,
                          void *comp_data,
                          bool warm,
                          uint32_t *blocking_component)
{
    IF_BLOCK_RETURN(instance_category, unit,
                    cleanup_comp_list, blocking_component);
    if (SHR_FAILURE(bcmptm_cth_alpm_be_cleanup(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_cth_alpm_be_start(void)
{
    shr_sysm_cb_func_set_t funcs;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Register unit-independent functions */
    sal_memset(&funcs, 0, sizeof(funcs));
    SHR_IF_ERR_EXIT(
        shr_sysm_register(BCMMGMT_CTH_ALPM_BE_COMP_ID,
                          SHR_SYSM_CAT_GENERIC,
                          &funcs,
                          NULL));

    /* Register unit-specific functions */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = cth_alpm_be_unit_init;
    funcs.comp_config = cth_alpm_be_unit_comp_config;
    funcs.pre_config = cth_alpm_be_unit_pre_config;
    funcs.run = cth_alpm_be_unit_run;
    funcs.stop = cth_alpm_be_unit_stop;
    funcs.shutdown = cth_alpm_be_unit_shutdown;
    SHR_IF_ERR_EXIT(
        shr_sysm_register(BCMMGMT_CTH_ALPM_BE_COMP_ID,
                          SHR_SYSM_CAT_UNIT,
                          &funcs,
                          NULL));
exit:
    SHR_FUNC_EXIT();
}
