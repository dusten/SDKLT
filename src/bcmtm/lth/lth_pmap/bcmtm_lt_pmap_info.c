/*! \file bcmtm_lt_pmap_info.c
 *
 * Logical Table Custom Handlers for TM_PORT_MAP_INFO.
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
 INCLUDES
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/lth_pmap/bcmtm_lt_pmap_info.h>

/*******************************************************************************
* Public functions
 */

int
bcmtm_lt_pmap_info_validate(int unit,
                             bcmlt_opcode_t opcode,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    int lport, pport;
    bcmlrd_sid_t sid = arg->sid;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_lt_field_validate(unit, opcode, sid, in));

    lport = in->field[TM_PORT_MAP_INFOt_PORT_IDf]->data;

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
            /* Check if the logical port exists. */
            if (SHR_FAILURE(bcmtm_port_phys_port_get(unit, lport, &pport))) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMLT_OPCODE_TRAVERSE:
            break;
        default:
            /* Insert, update, delete are not supported because of read-only. */
            SHR_RETURN_VAL_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_insert(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_lookup(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int lport, pport, mmu_port, mmu_chip_port, idb_port, pipe, uc_q_base,
             num_uc_q, mc_q_base, num_mc_q;
    size_t num_fields = 0, fid = 0;
    const bcmlrd_field_data_t *field;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    lport = in->field[TM_PORT_MAP_INFOt_PORT_IDf]->data;
    num_fields = TM_PORT_MAP_INFOt_FIELD_COUNT;

    out->count = 0;
    /* populating out buffer */
    while (fid <= num_fields) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, arg->sid, fid, &field)) {
            fid++;
            continue;
        }
        switch(fid) {
            case TM_PORT_MAP_INFOt_PORT_IDf:
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, lport);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_PC_PHYS_PORT_IDf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_phys_port_get(unit, lport, &pport));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, pport);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_PIPEf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_pipe_get(unit, lport, &pipe));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, pipe);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_TM_LOCAL_PORTf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_idb_port_get(unit, lport, &idb_port));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, idb_port);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_TM_SPARSE_GLOBAL_PORTf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_mmu_port_get(unit, lport, &mmu_port));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, mmu_port);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_TM_COMPACT_GLOBAL_PORTf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_mmu_chip_port_get(unit, lport, &mmu_chip_port));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, mmu_chip_port);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_BASE_UC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_uc_cosq_base_get(unit, lport, &uc_q_base));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, uc_q_base);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_NUM_UC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_num_uc_cosq_get(unit, lport, &num_uc_q));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, num_uc_q);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_BASE_MC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_mc_cosq_base_get(unit, lport, &mc_q_base));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, mc_q_base);
                out->count++;
                break;
            case TM_PORT_MAP_INFOt_NUM_MC_Qf:
                SHR_IF_ERR_EXIT
                    (bcmtm_port_num_mc_cosq_get(unit, lport, &num_mc_q));
                BCMTM_POPULATE_OUT_FIELD(fid, out->count, num_mc_q);
                out->count++;
                break;
            default:
                break;
        }
        fid++;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_delete(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_update(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pmap_info_first(int unit,
                            uint32_t trans_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    uint64_t min = 0, max = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PORT_MAP_INFOt,
                                         TM_PORT_MAP_INFOt_PORT_IDf,
                                         &min, &max));

    /* prepare the in buffer and perform lookup */
    out->field[TM_PORT_MAP_INFOt_PORT_IDf]->data = min;
    SHR_IF_ERR_EXIT(
            bcmtm_lt_pmap_info_lookup(unit, trans_id, out, out, arg));
exit:
    SHR_FUNC_EXIT();


}

int
bcmtm_lt_pmap_info_next(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t min, max;
    uint32_t lport;
    int pport;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PORT_MAP_INFOt,
                                         TM_PORT_MAP_INFOt_PORT_IDf,
                                         &min, &max));

    if (in) {
        lport = ++(in->field[TM_PORT_MAP_INFOt_PORT_IDf]->data);
    } else {
        lport = min;
    }

    /* Stop traverse when exceeding the max logical port number. */
    while (lport <= max) {
        /* Exit the current iteration once we get a valid entry. */
        if (SHR_SUCCESS(bcmtm_port_phys_port_get(unit, lport, &pport))) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TM_PORT_MAP_INFOt_PORT_IDf;
            flds.field[0]->data = lport;
            flds.count = 1;
            bcmtm_lt_pmap_info_lookup(unit, trans_id, &flds, out, arg);
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        } else {
            lport++;
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}
