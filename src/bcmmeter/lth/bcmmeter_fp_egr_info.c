/*! \file bcmmeter_fp_egr_info.c
 *
 * This file contains EFP Meter device info handler functions.
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
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmmeter/bcmmeter_fp_egr.h>

#include <bcmmeter/bcmmeter_fp_egr_info.h>

#define BSL_LOG_MODULE          BSL_LS_BCMMETER_FP

/*******************************************************************************
 * Private functions
 */
static int
bcmmeter_fp_egr_device_info (int unit,
                             uint32_t trans_id,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    size_t      num_fid = 0, count = 0;
    uint32_t    idx = 0, fid = 0;
    uint32_t    table_sz = 0;
    bcmlrd_fid_t   *fid_list = NULL;
    bcmltd_field_t *foutlist = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, METER_FP_EGR_DEVICE_INFOt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               METER_FP_EGR_DEVICE_INFOt,
                               num_fid,
                               fid_list,
                               &count));

    foutlist = out->field[0];

    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        foutlist[idx].id = fid;

        switch (fid) {
        case METER_FP_EGR_DEVICE_INFOt_NUM_METER_POOLSf:
            foutlist[idx].data = BCMMETER_FP_EGR_NUM_POOLS(unit);
            break;
        case METER_FP_EGR_DEVICE_INFOt_NUM_METERS_PER_POOLf:
            foutlist[idx].data = BCMMETER_FP_EGR_METERS_PER_POOL(unit);
            break;
        case METER_FP_EGR_DEVICE_INFOt_NUM_METERS_PER_PIPEf:
            foutlist[idx].data = BCMMETER_FP_EGR_METERS_PER_PIPE(unit);
            break;
        case METER_FP_EGR_DEVICE_INFOt_NUM_METERSf:
            foutlist[idx].data = BCMMETER_FP_EGR_METERS(unit);
            break;
        case METER_FP_EGR_DEVICE_INFOt_NUM_GRANULARITYf:
            foutlist[idx].data = BCMMETER_FP_EGR_MAX_GRAN(unit);
            break;
        default:
            break;
        }
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

static int
bcmmeter_fp_egr_gran_info (int unit,
                           uint32_t trans_id,
                           uint32_t gran,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    size_t                  num_fid = 0, count = 0;
    uint32_t                i = 0, fid = 0;
    uint32_t                table_sz = 0;
    uint32_t                max_burst = 0;
    bcmlrd_fid_t            *fid_list = NULL;
    bcmltd_field_t          *foutlist = NULL;
    bcmmeter_fp_gran_info_t *gran_info_bytes = NULL;
    bcmmeter_fp_gran_info_t *gran_info_pkts = NULL;

    SHR_FUNC_ENTER(unit);

    gran_info_bytes = BCMMETER_FP_EGR_GRAN_INFO_BYTES(unit);
    gran_info_pkts = BCMMETER_FP_EGR_GRAN_INFO_PKTS(unit);
    if ((gran_info_bytes == NULL) || (gran_info_pkts == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit,
                                METER_FP_EGR_GRANULARITY_INFOt,
                                &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               METER_FP_EGR_GRANULARITY_INFOt,
                               num_fid,
                               fid_list,
                               &count));

    foutlist = out->field[0];
    for (i = 0; i < count; i++) {
        fid = fid_list[i];
        foutlist[i].id = fid;
        switch (i) {
        case METER_FP_EGR_GRANULARITY_INFOt_METER_FP_EGR_GRANULARITY_INFO_IDf:
            foutlist[i].data = gran;
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MIN_RATE_KBPSf:
            foutlist[i].data = gran_info_bytes[gran].min_rate;
            if (BCMMETER_FP_EGR_REFRESHCOUNT_ADJUST(unit)) {
                foutlist[i].data /= 2;
            }
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MAX_RATE_KBPSf:
            foutlist[i].data = gran_info_bytes[gran].max_rate;
            if (BCMMETER_FP_EGR_REFRESHCOUNT_ADJUST(unit)) {
                foutlist[i].data /= 2;
            }
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MIN_BURST_KBITSf:
            foutlist[i].data = (gran_info_bytes[gran].min_burst / 1000);
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MAX_BURST_KBITSf:
            max_burst = ((gran_info_bytes[gran].min_burst *
                          BCMMETER_FP_EGR_MAX_BUCKETSIZE(unit)) / 1000);
            foutlist[i].data = max_burst;
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MIN_RATE_PPSf:
            foutlist[i].data = gran_info_pkts[gran].min_rate;
            if (BCMMETER_FP_EGR_REFRESHCOUNT_ADJUST(unit)) {
                foutlist[i].data /= 2;
            }
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MAX_RATE_PPSf:
            foutlist[i].data = gran_info_pkts[gran].max_rate;
            if (BCMMETER_FP_EGR_REFRESHCOUNT_ADJUST(unit)) {
                foutlist[i].data /= 2;
            }
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MIN_BURST_PKTSf:
            foutlist[i].data = (gran_info_pkts[gran].min_burst / 1000);
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_FP_EGR_GRANULARITY_INFOt_MAX_BURST_PKTSf:
            max_burst = ((gran_info_pkts[gran].min_burst *
                          BCMMETER_FP_EGR_MAX_BUCKETSIZE(unit)) / 1000);
            foutlist[i].data = max_burst;
        default:
            break;
        }
    }
exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief EFP meter gran info validation
 *
 * Validate EFP Meter gran info parameters.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_info_validate (int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if ((opcode != BCMLT_OPCODE_LOOKUP) &&
        (opcode != BCMLT_OPCODE_TRAVERSE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter gran info insert.
 *
 * Insert into EFP meter gran info LT. Invalid operation
 * on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmmeter_fp_egr_info_insert (int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter gran info lookup.
 *
 * Lookup EFP Meter gran info LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_info_lookup (int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int             field_count = 0;
    uint32_t        sid = 0, fid = 0, gran = 0;
    bcmltd_field_t  **flist = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    sid = arg->sid;

    switch (sid) {
    case METER_FP_EGR_DEVICE_INFOt:
        SHR_IF_ERR_EXIT
            (bcmmeter_fp_egr_device_info(unit, trans_id, out, arg));
        break;
    case METER_FP_EGR_GRANULARITY_INFOt:
        flist = in->field;
        field_count = in->count;

        fid = METER_FP_EGR_GRANULARITY_INFOt_METER_FP_EGR_GRANULARITY_INFO_IDf;
        SHR_IF_ERR_EXIT
            (bcmmeter_entry_field_value_get(unit,
                                            field_count,
                                            flist,
                                            fid,
                                            &gran));
        if (gran >= BCMMETER_FP_EGR_MAX_GRAN(unit)) {
            SHR_RETURN_VAL_EXIT(SHR_E_BADID);
        }

        SHR_IF_ERR_EXIT
            (bcmmeter_fp_egr_gran_info(unit, trans_id, gran, out, arg));
        break;

    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter gran info delete.
 *
 * Delete EFP Meter gran info. Invalid operation
 * on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmmeter_fp_egr_info_delete (int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter gran info update.
 *
 * Update EFP meter gran info. Invalid operaton
 * on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmmeter_fp_egr_info_update (int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter info traverse get first.
 *
 * Get the first EFP meter info LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_info_first(int unit,
                           uint32_t trans_id,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    uint32_t            sid = 0, fid = 0, gran = 0;
    bcmlrd_field_def_t  field_def;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    sid = arg->sid;

    switch (sid) {
    case METER_FP_EGR_DEVICE_INFOt:
        SHR_IF_ERR_EXIT
            (bcmmeter_fp_egr_device_info(unit, trans_id, out, arg));
        break;
    case METER_FP_EGR_GRANULARITY_INFOt:
        sid = METER_FP_EGR_GRANULARITY_INFOt;
        fid = METER_FP_EGR_GRANULARITY_INFOt_METER_FP_EGR_GRANULARITY_INFO_IDf;
        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid, &field_def));
        gran = BCMMETER_FP_FIELD_MIN(field_def.dtag);
        SHR_IF_ERR_EXIT
            (bcmmeter_fp_egr_gran_info(unit, trans_id, gran, out, arg));
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter info traverse get next.
 *
 * Get the next EFP meter info LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_info_next(int unit,
                          uint32_t trans_id,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    int             field_count = 0;
    uint32_t        sid = 0, fid = 0, gran = 0;
    bcmltd_field_t  **flist = NULL;


    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    sid = arg->sid;

    switch (sid) {
    case METER_FP_EGR_DEVICE_INFOt:
        SHR_IF_ERR_EXIT
            (bcmmeter_fp_egr_device_info(unit, trans_id, out, arg));
        break;
    case METER_FP_EGR_GRANULARITY_INFOt:
        flist = in->field;
        field_count = in->count;

        fid = METER_FP_EGR_GRANULARITY_INFOt_METER_FP_EGR_GRANULARITY_INFO_IDf;
        SHR_IF_ERR_EXIT
            (bcmmeter_entry_field_value_get(unit,
                                            field_count,
                                            flist,
                                            fid,
                                            &gran));
        if (gran >= BCMMETER_FP_EGR_MAX_GRAN(unit) ||
            ((gran + 1) >= BCMMETER_FP_EGR_MAX_GRAN(unit))) {
            SHR_RETURN_VAL_EXIT(SHR_E_BADID);
        }

        gran = (gran + 1);

        SHR_IF_ERR_EXIT
            (bcmmeter_fp_egr_gran_info(unit, trans_id, gran, out, arg));
        break;

    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();

}
