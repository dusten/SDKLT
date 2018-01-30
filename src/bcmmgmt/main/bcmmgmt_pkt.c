/*! \file bcmmgmt_pkt.c
 *
 * Start and stop HMI packet drivers.
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

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_unet.h>
#include <bcmpkt/bcmpkt_buf.h>

#include <bcmmgmt/bcmmgmt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/*******************************************************************************
 * Local definitions
 */

#define SHARED_BUF_SIZE         (9 * 1024)
#define SHARED_BUF_COUNT        (1024)

static bool bpool = false;
static bool ppool = false;

/*******************************************************************************
 * Public functions
 */

int
bcmmgmt_pkt_core_init(void)
{
    int unit;
    int psim;
    bool sim_enabled = false;
    int ndev = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    sim_enabled = false;
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
            if (psim) {
                sim_enabled = true;
            }
            ndev++;
        }
    }

    if (!sim_enabled && ndev > 0) {
        SHR_IF_ERR_EXIT
            (bcmpkt_bpool_create(BCMPKT_BPOOL_SHARED_ID,
                                 SHARED_BUF_SIZE, SHARED_BUF_COUNT));
        bpool = true;
        SHR_IF_ERR_EXIT
            (bcmpkt_ppool_create(BCMPKT_PPOOL_COUNT_DEF));
        ppool = true;
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmmgmt_pkt_core_cleanup();
    }

    SHR_FUNC_EXIT();
}

int
bcmmgmt_pkt_core_cleanup(void)
{
    int rv;
    int actual_rv = SHR_E_NONE;

    if (ppool) {
        rv = bcmpkt_ppool_destroy();
        if (rv != SHR_E_NONE && actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
        if (SHR_SUCCESS(rv)) {
            ppool = false;
        }
    }
    if (bpool) {
        rv = bcmpkt_bpool_cleanup();
        if (rv != SHR_E_NONE && actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
        if (SHR_SUCCESS(rv)) {
            bpool = false;
        }
    }

    return actual_rv;
}

int
bcmmgmt_pkt_dev_init(int unit)
{
    bcmpkt_dev_drv_types_t drv_type;
    int psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);

    SHR_FUNC_ENTER(unit);

    if (!psim) {
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_attach(unit, BCMPKT_DEV_DRV_T_AUTO));
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_type_get(unit, &drv_type));
        if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
            SHR_IF_ERR_EXIT
                (bcmpkt_unet_drv_attach(unit, BCMPKT_UNET_DRV_T_AUTO));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmmgmt_pkt_dev_cleanup(int unit)
{
    int rv;
    int actual_rv = SHR_E_NONE;
    int psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);

    if (!psim) {
        rv = bcmpkt_unet_drv_detach(unit);
        if (rv != SHR_E_NONE && actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
        rv = bcmpkt_dev_drv_detach(unit);
        if (rv != SHR_E_NONE && actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
    }

    return actual_rv;
}
