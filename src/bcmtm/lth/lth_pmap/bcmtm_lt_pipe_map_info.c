/*! \file bcmtm_lt_pipe_map_info.c
 *
 * Logical Table Custom Handlers for TM_PIPE_MAP_INFO.
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
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/lth_pmap/bcmtm_lt_pipe_map_info.h>

/*******************************************************************************
* Public functions
 */

int
bcmtm_lt_pipe_map_info_validate(int unit,
                             bcmlt_opcode_t opcode,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    bcmlrd_sid_t sid = arg->sid;

    SHR_FUNC_ENTER(unit);

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
        case BCMLT_OPCODE_TRAVERSE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_lt_field_validate(unit, opcode, sid, in));
            break;
        default:
            /* Insert, update, delete are not supported because of read-only. */
            SHR_RETURN_VAL_EXIT(SHR_E_ACCESS);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_insert(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate the list of port in a pipe.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe
 * \param [in] max_count maximum entries
 * \param [out] lport_list Logical port array
 * \param [out] count Actual number of entries
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Port calculation error.
 */
static int
bcmtm_lt_pipe_port_list_get(int unit,
                            int pipe, int max_count,
                            int *lport_list,
                            int *count)
{
    int i, j, pipe_local, rv;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lport_list, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);
    j = 0;
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        rv = bcmtm_port_pipe_get(unit, i, &pipe_local);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        if (pipe != pipe_local) {
            continue;
        }
        lport_list[j] = i;
        j++;
        if (j >= max_count) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    *count = j;

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_lookup(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
#define MAX_PORTS_PER_PIPE 32
    int i, pipe, lport_list[MAX_PORTS_PER_PIPE], port_count = 0;
    uint32_t num_fields;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    pipe = in->field[TM_PIPE_MAP_INFOt_PIPEf]->data;
    SHR_IF_ERR_EXIT
        (bcmtm_lt_pipe_port_list_get(unit, pipe, MAX_PORTS_PER_PIPE,
                                     lport_list, &port_count));
    num_fields = out->count;
    out->count = 0;

    fid = TM_PIPE_MAP_INFOt_PIPEf;
    bcmtm_field_populate(unit, out, fid, 0, pipe);

    fid = TM_PIPE_MAP_INFOt_PORT_CNTf;
    bcmtm_field_populate(unit, out, fid, 0, port_count);

    fid = TM_PIPE_MAP_INFOt_PORT_IDf;
    for (i = 0; i < port_count; i++) {
        bcmtm_field_populate(unit, out, fid, i, lport_list[i]);
        if (out->count > num_fields) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_delete(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_update(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcmtm_lt_pipe_map_info_first(int unit,
                            uint32_t trans_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    uint64_t min = 0, max = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PIPE_MAP_INFOt,
                                         TM_PIPE_MAP_INFOt_PIPEf,
                                         &min, &max));

    /* prepare the in buffer and perform lookup */
    out->field[TM_PIPE_MAP_INFOt_PIPEf]->data = min;
    SHR_IF_ERR_EXIT(
            bcmtm_lt_pipe_map_info_lookup(unit, trans_id, out, out, arg));
exit:
    SHR_FUNC_EXIT();


}

int
bcmtm_lt_pipe_map_info_next(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    uint64_t min, max;
    uint32_t pipe;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_value_range_get(unit, TM_PIPE_MAP_INFOt,
                                         TM_PIPE_MAP_INFOt_PIPEf,
                                         &min, &max));

    if (in) {
        pipe = ++(in->field[TM_PIPE_MAP_INFOt_PIPEf]->data);
    }

    if (pipe > max) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    in->field[TM_PIPE_MAP_INFOt_PIPEf]->data = pipe;

    bcmtm_lt_pipe_map_info_lookup(unit, trans_id, in, out, arg);
exit:
    SHR_FUNC_EXIT();
}
