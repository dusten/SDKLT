/*! \file bcmtrunk_common.c
 *
 * <description>
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
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtrunk/bcmtrunk_chips.h>
#include "./bcmtrunk_common.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMTRUNK_SYSM


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static bcmtrunk_drv_t *trunk_drv[BCMDRD_CONFIG_MAX_UNITS];

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_trunk_drv_attach }, /* 8th param is _bc (SW Base Cfg) */
static struct {
    bcmtrunk_drv_attach_f attach;
} list_dev_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};


/*******************************************************************************
 * Private Functions
 */
static int
bcmtrunk_drv_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific fnptr_attach */
    SHR_IF_ERR_EXIT
        (list_dev_attach[dev_type].attach(trunk_drv[unit]));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcmtrunk_drv_init(int unit)
{
    size_t size;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(trunk_drv[unit] != NULL? SHR_E_INTERNAL : SHR_E_NONE);

    /* Allocate space for func ptrs for this unit */
    size = sizeof(bcmtrunk_drv_t);
    trunk_drv[unit] = sal_alloc(size, "bcmtrunk_function_pointers");
    SHR_NULL_CHECK(trunk_drv[unit], SHR_E_MEMORY);
    sal_memset(trunk_drv[unit], 0, size); /* clean slate */
    SHR_IF_ERR_EXIT(bcmtrunk_drv_attach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_drv_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(trunk_drv[unit], SHR_E_INTERNAL);

    /* Release space allocated for func ptrs */
    sal_free(trunk_drv[unit]);
    trunk_drv[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_trunk_mode_set(int unit, bool fast)
{
    fn_trunk_mode_set_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_trunk_mode_set) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, fast);
    }
}

int
bcmtrunk_grp_add(int unit,
                 uint32_t trans_id,
                 bcmltd_sid_t lt_id,
                 bcmtrunk_lth_grp_bk_t *grp_bk,
                 bcmtrunk_group_param_t *param)
{
    fn_grp_add_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_grp_add) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, grp_bk, param);
    }
}

int
bcmtrunk_grp_update(int unit,
                    uint32_t trans_id,
                    bcmltd_sid_t lt_id,
                    bcmtrunk_lth_grp_bk_t *grp_bk,
                    bcmtrunk_group_param_t *param)
{
    fn_grp_update_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_grp_update) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, grp_bk, param);
    }
}

int
bcmtrunk_grp_del(int unit,
                 uint32_t trans_id,
                 bcmltd_sid_t lt_id,
                 bcmtrunk_lth_grp_bk_t *grp_bk,
                 bcmtrunk_group_param_t *param)
{
    fn_grp_del_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_grp_del) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, grp_bk, param);
    }
}

int
bcmtrunk_grp_init(int unit, bcmtrunk_lth_grp_bk_t *grp_bk)
{
    fn_grp_init_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_grp_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, grp_bk);
    }
}

int
bcmtrunk_fast_grp_add(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t lt_id,
                      bcmtrunk_fast_group_param_t *param)
{
    fn_fast_grp_add_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_fast_grp_add) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, param);
    }
}

int
bcmtrunk_fast_grp_update(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t lt_id,
                         bcmtrunk_fast_group_param_t *param,
                         bcmtrunk_fast_group_t *grp)
{
    fn_fast_grp_update_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_fast_grp_update) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, param, grp);
    }
}

int
bcmtrunk_fast_grp_del(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t lt_id,
                      bcmtrunk_fast_group_param_t *param,
                      bcmtrunk_fast_group_t *grp)
{
    fn_fast_grp_del_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_fast_grp_del) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, param, grp);
    }
}

int
bcmtrunk_fast_grp_init(int unit, bcmtrunk_lth_fgrp_bk_t *fgrp_bk)
{
    fn_fast_grp_init_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_fast_grp_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, fgrp_bk);
    }
}

int
bcmtrunk_failover_add(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t lt_id,
                      bcmtrunk_failover_param_t *param)
{
    fn_failover_add_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_failover_add) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, param);
    }
}

int
bcmtrunk_failover_update(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t lt_id,
                         bcmtrunk_failover_param_t *param,
                         bcmtrunk_failover_t *entry)
{
    fn_failover_update_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_failover_update) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, param, entry);
    }
}

int
bcmtrunk_failover_del(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t lt_id,
                      bcmtrunk_failover_param_t *param)
{
    fn_failover_del_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_failover_del) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, trans_id, lt_id, param);
    }
}

int
bcmtrunk_failover_init(int unit, bcmtrunk_failover_bk_t *failover_bk)
{
    fn_failover_init_t my_fn;

    if ((my_fn = trunk_drv[unit]->fn_failover_init) == NULL) {
        return SHR_E_UNAVAIL;
    } else {
        return my_fn(unit, failover_bk);
    }
}
