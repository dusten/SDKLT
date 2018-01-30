/*! \file bcmlm_drv.c
 *
 * Link Manager driver
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

#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include "bcmlm_chip_internal.h"

/******************************************************************************
* Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLM_DRV

/* Linkscan driver */
static bcmlm_drv_t *bcmlm_drv[BCMDRD_CONFIG_MAX_UNITS];

/*! Device specific attach function type. */
typedef int (*bcmlm_drv_attach_f)(bcmlm_drv_t *drv);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
        { _bc##_lm_drv_attach },
static struct {
        bcmlm_drv_attach_f attach;
} list_dev_attach[] = {
        { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
            { 0 } /* end-of-list */
};

#define LM_CALL(_ld, _lf, _la) \
        ((_ld) == 0 ? SHR_E_INIT: \
              ((_ld)->_lf == 0 ? SHR_E_UNAVAIL: (_ld)->_lf _la))

#define LINKSCAN_HW_CONFIG(_u, _hw) \
        LM_CALL(bcmlm_drv[_u], hw_config, ((_u), (_hw)))

#define LINKSCAN_HW_LINK_GET(_u, _hw_link) \
        LM_CALL(bcmlm_drv[_u], hw_link_get, ((_u), (_hw_link)))

#define LINKSCAN_HW_INTR_CLEAR(_u) \
        LM_CALL(bcmlm_drv[_u], hw_intr_clear, (_u))

#define LINKSCAN_HW_INTR_CB_SET(_u, _intr_func) \
        LM_CALL(bcmlm_drv[_u], hw_intr_cb_set, ((_u), (_intr_func)))

#define LINKSCAN_HW_SCAN_STOP(_u)    \
        LM_CALL(bcmlm_drv[_u], hw_scan_stop, (_u))

#define LINKSCAN_HW_SCAN_START(_u)    \
        LM_CALL(bcmlm_drv[_u], hw_scan_start, (_u))

#define LINKSCAN_PORT_VALIDATE(_u, _port) \
        LM_CALL(bcmlm_drv[_u], port_validate, ((_u), (_port)))

#define LINKSCAN_FAULT_CHK_ENABLED(_u, _enabled) \
        LM_CALL(bcmlm_drv[_u], fault_check_enabled, ((_u), (_enabled)))

/******************************************************************************
* Private functions
 */

/******************************************************************************
* Public functions
 */
int
bcmlm_drv_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(bcmlm_drv[unit] != NULL ? SHR_E_INTERNAL: SHR_E_NONE);

    SHR_ALLOC(bcmlm_drv[unit], sizeof(bcmlm_drv_t), "bcmlm_drv");
    SHR_NULL_CHECK(bcmlm_drv[unit], SHR_E_MEMORY);
    sal_memset(bcmlm_drv[unit], 0, sizeof(bcmlm_drv_t));
    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(bcmlm_drv[unit]));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_drv_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bcmlm_drv[unit], SHR_E_INTERNAL);

    SHR_FREE(bcmlm_drv[unit]);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_dev_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmlm_drv[unit] && bcmlm_drv[unit]->hw_init) {
        SHR_IF_ERR_EXIT
            (bcmlm_drv[unit]->hw_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_config(int unit, bcmdrd_pbmp_t hw)
{
    shr_port_t lport, pport;
    bcmdrd_pbmp_t ppbm;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_CLEAR(ppbm);
    BCMDRD_PBMP_ITER(hw, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport != BCMPC_INVALID_PPORT) {
            BCMDRD_PBMP_PORT_ADD(ppbm, pport);
        }
    }

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_SCAN_STOP(unit));

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_CONFIG(unit, ppbm));

    if (BCMDRD_PBMP_NOT_NULL(ppbm)) {
        SHR_IF_ERR_EXIT
            (LINKSCAN_HW_SCAN_START(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_link_get(int unit, bcmdrd_pbmp_t *hw)
{
    shr_port_t lport, pport;
    bcmdrd_pbmp_t ppbm;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_CLEAR(*hw);
    BCMDRD_PBMP_CLEAR(ppbm);
    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_LINK_GET(unit, &ppbm));

    BCMDRD_PBMP_ITER(ppbm, pport) {
        lport = bcmpc_pport_to_lport(unit, pport);
        if (lport != BCMPC_INVALID_LPORT) {
            BCMDRD_PBMP_PORT_ADD(*hw, lport);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_intr_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_INTR_CLEAR(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_hw_linkscan_intr_cb_set(int unit, bcmbd_intr_f cb)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (LINKSCAN_HW_INTR_CB_SET(unit, cb));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_linkscan_fault_check_enabled(int unit, int *enabled)
{
    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(LINKSCAN_FAULT_CHK_ENABLED(unit, enabled))) {
        SHR_RETURN_VAL_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_linkscan_port_validate(int unit, shr_port_t port)
{
    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(LINKSCAN_PORT_VALIDATE(unit, port))) {
        SHR_RETURN_VAL_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmlm_sw_linkscan_link_get(int unit, shr_port_t port, int *link)
{
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port);
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_link_state_get(unit, pport, link));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_linkscan_fault_get(int unit, shr_port_t port, int *fault)
{
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port);
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_fault_state_get(unit, pport, fault));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_port_update(int unit, shr_port_t port, int link)
{
    shr_port_t pport;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, port);
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_port_link_change(unit, pport, link));

exit:
    SHR_FUNC_EXIT();
}

