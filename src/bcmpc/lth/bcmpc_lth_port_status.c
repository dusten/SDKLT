/*! \file bcmpc_lth_port_status.c
 *
 * Logical Table Custom Handlers for PC_PORT_STATUS.
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
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_lth_port_status.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PORT_STATUSt


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_port_status_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    size_t idx;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < in->count; idx++) {
        fid = in->field[idx]->id;

        if (fid == PC_PORT_STATUSt_PORT_IDf) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_lth_field_validate(unit, LTH_SID, in->field[idx]));
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_insert(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_lookup(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    uint64_t fval;
    int rv, fcnt = 0;
    uint32_t failover = 0;
    bcmpc_lport_t lport;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;
    bcmpc_pmgr_port_status_t pst;
    bcmpc_pmgr_link_status_t link_st = 0;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    bcmpc_pmgr_port_status_t_init(&pst);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_STATUSt_PORT_IDf, &fval));
    lport = fval;

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "Invalid PORT_ID.\n")));
    }

    pport = bcmpc_lport_to_pport(unit, lport);

    /* Get the status from hardware when the port is valid. */
    if (pport != BCMPC_INVALID_PPORT &&
        pcfg.opmode != BCMPC_PORT_OPMODE_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_status_get(unit, pport, &pst));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_link_status_get(unit, pport, &link_st));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_failover_loopback_get(unit, pport, &failover));
    }

    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PORT_IDf, 0, lport, fcnt);
    fval = pst.opmode;
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_OPMODEf, 0, fval, fcnt);
    fval = (pst.mac_enabled == 0) ? 1 : 0;
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_MAC_DISABLEDf, 0, fval, fcnt);
    fval = (pst.phy_enabled == 0) ? 1 : 0;
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PHY_DISABLEDf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_LINK);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_LINK_UPf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_LOCAL_FAULT);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_LOCAL_FAULTf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_REMOTE_FAULTf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_PLL_LOCK_ERR);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PLL_LOCK_ERRf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_PCS_LINK_ERR);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PCS_LINK_ERRf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_PMD_LINK_ERR);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_PMD_LINK_ERRf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_NO_SIGNAL);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_NO_SIGNALf, 0, fval, fcnt);
    fval = BCMPC_PMGR_LINK_ST_GET(link_st, BCMPC_PMGR_LINK_EVENT_LANE_SYNC_ERR);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_LANE_SYNC_ERRf, 0, fval, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PORT_STATUSt_FAILOVER_LOOPBACKf, 0,
                        failover, fcnt);

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_delete(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_update(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_first(int unit,
                            uint32_t trans_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t min, max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PORT_STATUSt_PORT_IDf,
                                         &min, &max));
    flist = &fld;
    flds.field = &flist;
    flds.field[0]->id = PC_PORT_STATUSt_PORT_IDf;
    flds.field[0]->data = min;
    flds.count = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_port_status_lookup(unit, trans_id, &flds, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_port_status_next(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t lport, min, max;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PM_PROPt_PC_PM_IDf,
                                         &min, &max));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PORT_STATUSt_PORT_IDf, &lport));

    lport++;

    /* Stop traverse when exceeding the max PM ID. */
    while (lport <= max) {
        /* Exit the current iteration once we get a valid entry. */
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = PC_PORT_STATUSt_PORT_IDf;
        flds.field[0]->data = lport;
        flds.count = 1;
        rv = bcmpc_lth_port_status_lookup(unit, trans_id, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        lport++;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}
