/*! \file bcmltx_l3uc_ecmp_or_nhop_transform.c
 *
 * NHOP_ID/ECMP_ID Transform Handler
 *
 * This file contains field transform information for
 * ECMP_NHOP field.
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
#include <bcmltx/bcml3/bcmltx_l3uc_ecmp_or_nhop.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L3

/*
 * \brief bcmltx_l3uc_ecmp_or_nhop_transform is forward transform for ECMP or NHOP ID
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              ECMP_NHOP/NHOP_ID/ECMP_ID
 * \param [out] out             L3_ENTRY_IPV4_UNICAST.ECMP/NEXT_HOP_INDEX fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3uc_ecmp_or_nhop_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint64_t ECMP_NHOP_field_val = 0;
    uint64_t NHOP_ID_field_val = 0;
    uint64_t ECMP_ID_field_val = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_l3uc_ecmp_or_nhop_transform\n")));

    /* No input no output */
    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 3) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    ECMP_NHOP_field_val = in->field[0]->data;
    NHOP_ID_field_val   = in->field[1]->data;
    ECMP_ID_field_val   = in->field[2]->data;
    if (ECMP_ID_field_val > ECMP_ID_MAX_VALUE) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* First  reverse argument[0] is always ECMP flag.
     * Second reverse argument[1] is NEXT_HOP_INDEX.
     */
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->data = ECMP_NHOP_field_val;
    out->field[1]->id   = arg->rfield[1];
    if (ECMP_NHOP_field_val) {
        if (NHOP_ID_field_val != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        out->field[1]->data = ECMP_ID_field_val;
    } else {
        if (ECMP_ID_field_val != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        out->field[1]->data = NHOP_ID_field_val;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmltx_l3uc_ecmp_or_nhop_rev_transform is reverse transform for ECMP or NHOP ID
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              L3_ENTRY_IPV4_UNICAST/ECMP/NEXT_HOP_INDEX
 * \param [out] out             ECMP_NHOP/NHOP_ID/ECMP_ID fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3uc_ecmp_or_nhop_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint64_t ECMP_NHOP_field_val = 0;
    uint64_t ECMP_NHOP_ID_field_val = 0;

    SHR_FUNC_ENTER(unit);

    if (in->count != 2) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /* 1 indicates ECMP index, 0 indicates NEXT_HOP index. */
    ECMP_NHOP_field_val = in->field[0]->data;
    ECMP_NHOP_ID_field_val = in->field[1]->data;

    /* First  reverse argument[0] is always ECMP_NHOP.
     * Second reverse argument[1] is alwasy NHOP_ID.
     * Third  reverse argument[2] is alwasy ECMP_ID.
     */
    out->field[0]->id = arg->rfield[0];
    out->field[0]->data = ECMP_NHOP_field_val;
    if (!ECMP_NHOP_field_val) {
        out->field[1]->id = arg->rfield[1];
        out->field[1]->data = ECMP_NHOP_ID_field_val;
        out->field[2]->id = arg->rfield[2];
        out->field[2]->data = 0;
    } else {
        out->field[1]->id = arg->rfield[1];
        out->field[1]->data = 0;
        out->field[2]->id = arg->rfield[2];
        out->field[2]->data = ECMP_NHOP_ID_field_val;
    }

exit:
    SHR_FUNC_EXIT();
}

