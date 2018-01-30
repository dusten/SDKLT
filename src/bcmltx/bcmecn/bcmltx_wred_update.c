/*! \file bcmltx_wred_update.c
 *
 * ECN_WRED_UPDATE Transform Handler
 *
 * This file contains field/index transform information for ECN_PROTOCOL
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
#include <shr/shr_bitop.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmecn/bcmltx_wred_index.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMECN_WRED_UPDATE


/*!
 * \brief ecn index transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 *
 *
 * This transform is used to derive index to update internal
 * congestion mapping from packet color, traffic managers congestion
 * status and incoming internal congestion notification.
 */
int
bcmltx_wred_index_transform(int unit,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_transform_arg_t *arg)

{
    uint32_t tm_cng = 0, cng = 0;
    uint32_t index_shift = 0;
    uint32_t int_cn = 0;
    uint32_t idx = 0;
    uint32_t i = 0, field_count = 0;
    bcmltd_field_t **flist = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmecn_wred_index_transform\n")));

    field_count = in->count;
    flist = in->field;

    /* No input, no output */
    if (field_count == 0) {
        SHR_EXIT();
    }

    /* Input Data validation. */
    if (field_count != 3) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The argument count should be 3.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Retrieve input info from buffers */
    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  " The input buffers can not be NULL.\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        switch (flist[i]->id) {
        case ECN_WRED_UPDATEt_INT_ECN_CNGf:
            int_cn = flist[i]->data;
            break;
        case ECN_WRED_UPDATEt_TM_CONGESTIONf:
            tm_cng = flist[i]->data;
            break;
        case ECN_WRED_UPDATEt_PACKET_CNGf:
            cng = flist[i]->data;
            break;
        default:
            break;
        }
    }

    /*
     * Set up index transform.
     * Below is the index key in hardware
     *  [INT_CN, CCBE__CE_GREEN, CCBE__CE_YELLOW, CCBE__CE_RED, MPB__CNG]
 */
    if (cng == BCM_XGS_COLOR_RED) {
        index_shift = 2;
    } else if (cng == BCM_XGS_COLOR_YELLOW ) {
        index_shift = 3;
    } else if (cng == BCM_XGS_COLOR_GREEN ) {
        index_shift = 4;
    }

    /* Now set up the index */
    idx = ((int_cn << 5) | (tm_cng << index_shift) | cng);

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Index generated for hw table is %d.\n"), idx));


    /* Add information back to output buffers. */
    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = idx;
    out->count++;

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ecn index reverse transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 *
 * This reverse transform is used to convert the hardware index to internal
 * congestion mapping table back to packet color, traffic managers congestion
 * status and incoming internal congestion notification.
 */
int
bcmltx_wred_index_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t tm_cng = 0, cng = 0;
    uint32_t index_shift = 0;
    uint32_t int_cn = 0;
    uint32_t idx = 0;
    uint32_t i = 0;
    uint32_t fid = 0;
    uint32_t field_count = 0;
    bcmltd_field_t **flist = NULL;
    const bcmltd_field_desc_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;

    /* No input, no output */
    if (field_count == 0) {
        SHR_EXIT();
    }

    /* Input Data validation. */
    if (field_count != 1) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The argument count should be 1.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The input buffers can not be NULL.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The output buffers can not be NULL.\n")));

        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* Retrieve input info (index) from buffers */
    idx = in->field[0]->data;

    /* Convert index into the key fields. */
    cng = (idx & 0x3);
    int_cn = ((idx >> 5) & 0x3);

    if (cng == BCM_XGS_COLOR_RED) {
        index_shift = 2;
    } else if (cng == BCM_XGS_COLOR_YELLOW ) {
        index_shift = 3;
    } else if (cng == BCM_XGS_COLOR_GREEN ) {
        index_shift = 4;
    }
    tm_cng = ((idx >> index_shift) & 0x1);

    flist = out->field;
    finfo = arg->rfield_list->field_array;
    field_count = arg->rfield_list->field_num;

    out->count = 0;
    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, " The output buffers can not be NULL.\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        fid = finfo[i].field_id;
        switch (fid) {
        case ECN_WRED_UPDATEt_INT_ECN_CNGf:
            flist[i]->id = ECN_WRED_UPDATEt_INT_ECN_CNGf;
            flist[i]->data = int_cn;
            out->count++;
            break;
        case ECN_WRED_UPDATEt_PACKET_CNGf:
            flist[i]->id = ECN_WRED_UPDATEt_PACKET_CNGf;
            flist[i]->data = cng;
            out->count++;
            break;
        case ECN_WRED_UPDATEt_TM_CONGESTIONf:
            flist[i]->id = ECN_WRED_UPDATEt_TM_CONGESTIONf;
            flist[i]->data = tm_cng;
            out->count++;
            break;
        default:
            break;
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();

}
