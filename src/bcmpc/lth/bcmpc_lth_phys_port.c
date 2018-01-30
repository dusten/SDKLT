/*! \file bcmpc_lth_phys_port.c
 *
 * Logical Table Custom Handlers for PC_PHYS_PORT.
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

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_lth_phys_port.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PHYS_PORTt


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_phys_port_validate(int unit,
                             bcmlt_opcode_t opcode,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_fields_validate(unit, LTH_SID, in));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phys_port_insert(int unit,
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
bcmpc_lth_phys_port_lookup(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    uint64_t fval;
    int rv, pm_id;
    bcmpc_pport_t pport;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PHYS_PORTt_PC_PHYS_PORT_IDf, &fval));
    pport = fval;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (rv == SHR_E_NOT_FOUND) {
        /*
         * If the given physical port number is not found in the topology
         * database, treat it as an invalid value.
         */
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Invalid PC_PHYS_PORT_ID value.\n")));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    out->field[0]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
    out->field[0]->data = pport;
    out->field[1]->id = PC_PHYS_PORTt_PC_PM_IDf;
    out->field[1]->data = pm_id;
    out->field[2]->id = PC_PHYS_PORTt_PM_PHYS_PORTf;
    out->field[2]->data = pport - pm_info.base_pport;
    out->count = 3;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phys_port_delete(int unit,
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
bcmpc_lth_phys_port_update(int unit,
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
bcmpc_lth_phys_port_first(int unit,
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
                                         PC_PHYS_PORTt_PC_PHYS_PORT_IDf,
                                         &min, &max));
    flist = &fld;
    flds.field = &flist;
    flds.field[0]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
    flds.field[0]->data = min;
    flds.count = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_phys_port_lookup(unit, trans_id, &flds, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_phys_port_next(int unit,
                         uint32_t trans_id,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t pport, min, max;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PHYS_PORTt_PC_PHYS_PORT_IDf,
                                         &min, &max));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PHYS_PORTt_PC_PHYS_PORT_IDf, &pport));

    pport++;

    /* Stop traverse when exceeding the max physical port number. */
    while (pport <= max) {
        /* Exit the current iteration once we get a valid entry. */
        flist = &fld;
        flds.field = &flist;
        flds.field[0]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
        flds.field[0]->data = pport;
        flds.count = 1;
        rv = bcmpc_lth_phys_port_lookup(unit, trans_id, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        pport++;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

