/*! \file bcmtnl_l3_intf.c
 *
 * This file contains L3 egress tunnel interface
 * software state related intialization and
 * cleanup routines.
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
#include <shr/shr_ha.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmtnl/bcmtnl_internal.h>

#include <bcmtnl/bcmtnl_l3_intf_table_commit.h>
#include <bcmtnl/bcmtnl_l3_intf.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTNL_COMMON

/*******************************************************************************
 * Local definitions
 */
bcmtnl_l3_intf_state_t  bcmtnl_l3_egr_intf_state[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief L3 sw resources init.
 *
 * Initialize L3 SW data structures for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmtnl_l3_intf_sw_state_init(int unit,
                             bool warm)
{
    uint32_t    tbl_sz = 0;
    uint32_t    ha_alloc_sz = 0, ha_req_sz = 0;
    bcmlrd_field_def_t  field_def;

    SHR_FUNC_ENTER(unit);

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    L3_EIFt,
                                    L3_EIFt_L3_EIF_IDf,
                                    &field_def));

    tbl_sz = (BCMTNL_FIELD_MAX(field_def) + 1);
    if (tbl_sz == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Allocate backup HA memory for L3 tnl interface sw state. */
    ha_req_sz = (tbl_sz * sizeof(bcmtnl_l3_intf_t));
    ha_alloc_sz = ha_req_sz;
    BCMTNL_L3_EGR_INTF_HA(unit) =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMTNL_L3_INTF_SUB_COMP_ID,
                         &ha_alloc_sz);

    SHR_NULL_CHECK(BCMTNL_L3_EGR_INTF_HA(unit), SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_sz < ha_req_sz) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(BCMTNL_L3_EGR_INTF_HA(unit), 0, ha_alloc_sz);
    }

    /* Allocate memory for L3 tnl interface sw state. */
    SHR_ALLOC(BCMTNL_L3_EGR_INTF(unit),
              ha_alloc_sz,
              "l3 eif tunnel interface");
    SHR_NULL_CHECK(BCMTNL_L3_EGR_INTF(unit), SHR_E_MEMORY);
    sal_memset(BCMTNL_L3_EGR_INTF(unit), 0, ha_alloc_sz);


    BCMTNL_L3_EGR_INTF_SIZE(unit) = ha_alloc_sz;

    /* Copy backup sw state to active sw state. */
    sal_memcpy(BCMTNL_L3_EGR_INTF(unit),
               BCMTNL_L3_EGR_INTF_HA(unit),
               ha_alloc_sz);

exit:
    if (SHR_FUNC_ERR()) {
        if (BCMTNL_L3_EGR_INTF(unit) != NULL) {
            SHR_FREE(BCMTNL_L3_EGR_INTF(unit));
        }
        if (BCMTNL_L3_EGR_INTF_HA(unit) != NULL) {
            shr_ha_mem_free(unit, BCMTNL_L3_EGR_INTF_HA(unit));
        }

        BCMTNL_L3_EGR_INTF_SIZE(unit) = 0;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 sw resources cleanup.
 *
 * Cleanup L3 SW data structures allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmtnl_l3_intf_sw_state_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (BCMTNL_L3_EGR_INTF(unit) != NULL) {
        SHR_FREE(BCMTNL_L3_EGR_INTF(unit));
    }

    BCMTNL_L3_EGR_INTF_SIZE(unit) = 0;

    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 tunnel encap id prepare
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmtnl_l3_intf_table_prepare(int unit,
                             uint32_t trans_id,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 tunnel encap id commit
 *
 * Discard the backup state and copy current state
 * to backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmtnl_l3_intf_table_commit(int unit,
                            uint32_t trans_id,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t                alloc_sz = 0;
    bcmtnl_l3_intf_t        *bkp_ptr = NULL;
    bcmtnl_l3_intf_t        *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    alloc_sz = BCMTNL_L3_EGR_INTF_SIZE(unit);
    bkp_ptr = BCMTNL_L3_EGR_INTF_HA(unit);
    active_ptr = BCMTNL_L3_EGR_INTF(unit);
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Copy active sw state to backup sw state. */
    sal_memcpy(bkp_ptr, active_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 tunnel encap id abort
 *
 * Rollback the current state to backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmtnl_l3_intf_table_abort(int unit,
                           uint32_t trans_id,
                           const bcmltd_generic_arg_t *arg)
{
    uint32_t                alloc_sz = 0;
    bcmtnl_l3_intf_t        *bkp_ptr = NULL;
    bcmtnl_l3_intf_t        *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    alloc_sz = BCMTNL_L3_EGR_INTF_SIZE(unit);
    bkp_ptr = BCMTNL_L3_EGR_INTF_HA(unit);
    active_ptr = BCMTNL_L3_EGR_INTF(unit);
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Copy backup sw state to active sw state. */
    sal_memcpy(active_ptr, bkp_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}
