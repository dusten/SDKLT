/*! \file bcmtnl_decap_port_profile.c
 *
 * TNL_DECAP_PORT_PROFILE Transform Handler
 *
 * This file contains field/index transform information
 * for TNL_DECAP_PORT_PROFILE.
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
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbol_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltx/bcmtnl/bcmltx_tnl_decap_port_profile.h>

/* BSL Module */
#define BSL_LOG_MODULE  BSL_LS_BCMLTX_TNL

/*!
 * \brief TNL decap port transform
 *
 * This function transforms DECAP_PORT list
 * into portbitmap.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_tnl_decap_port_profile_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    int         idx = 0, port_count = 0;
    int         fld_count = 0, out_idx = 0;
    uint32_t    port = 0, pbmp = 0;
    uint32_t    alloc_sz = 0;
    uint32_t    fld_sz = 0, *data = NULL;

    SHR_FUNC_ENTER(unit);

    if ((arg->values == 0) || (arg->value == NULL) ||
        (arg->comp_data == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    port_count = in->count;
    fld_count  = arg->value[0];
    fld_sz     = arg->value[1];

    alloc_sz = (sizeof(uint32_t) * fld_count);
    SHR_ALLOC(data, alloc_sz, "Output fld array");
    sal_memset(data, 0, alloc_sz);

    for (idx = 0; idx < port_count; idx++) {
        if (in->field[idx]->data == 0) {
            continue;
        } else {
            port = idx;
        }
        pbmp = (1 << (port % fld_sz));
        out_idx = (port / fld_sz);
        data[out_idx] |= pbmp;
    }

    for (idx = 0; idx < fld_count; idx++) {
        out->field[idx]->id = arg->rfield[idx];
        out->field[idx]->data = data[idx];
    }
    out->count = fld_count;

exit:
    SHR_FREE(data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TNL decap port rev transform
 *
 * This function transforms DECAP port pbmp
 * into port list.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_tnl_decap_port_profile_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    int      idx = 0, count = 0;
    bool     member = false;
    uint32_t fid = 0, out_idx = 0;
    uint32_t data = 0, fld_sz = 0, arr_idx = 0;

    SHR_FUNC_ENTER(unit);

    if ((arg->values == 0) || (arg->value == NULL) ||
        (arg->comp_data == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    count = in->count;
    fld_sz = arg->value[1];
    fid = arg->rfield[0];

    for (idx = 0; idx < count; idx++) {
        data = in->field[idx]->data;
        for (out_idx = 0; out_idx < fld_sz; out_idx++) {
            member = (((data >> out_idx) & 0x1) ? true : false);
            arr_idx = ((idx * fld_sz) + out_idx);
            out->field[arr_idx]->id = fid;
            out->field[arr_idx]->idx = arr_idx;
            out->field[arr_idx]->data = member;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
