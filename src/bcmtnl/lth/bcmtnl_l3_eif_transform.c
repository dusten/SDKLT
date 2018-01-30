/*! \file bcmtnl_l3_eif_transform.c
 *
 * L3_EIF.TNL_ENCAP_ID and L3_EIF.L3_EIF_ID Transform Handler
 * This file contains field transform information for
 * L3_EIF LT fields L3_EIF_ID and TNL_ENCAP_ID.
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
#include <bcmtnl/bcmtnl_l3_intf.h>

#include <bcmtnl/bcmtnl_encap_id.h>
#include <bcmtnl/bcmtnl_l3_eif_id.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTNL_COMMON

/*!
 * L3_EIF_ID - shared data between L3_EIF_ID
 * transform and TNL_ENCAP_ID transform.
 */
uint32_t    bcmtnl_l3_eif_id;

/*******************************************************************************
 * Public functions
 */

/*
 * \brief bcmtnl_l3_eif_id_transform is forward transform
 * for saving L3_EIF_ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Input field values.
 * \param [out] out             Output field values.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_l3_eif_id_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg)
{
    uint32_t    l3_eif_id = 0;
    uint32_t    field_count = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    if (field_count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 1) || (in->field == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /*
     * Get L3_EIF_ID from the input component data argument and save it.
     * This will be used by TNL_ENCAP_ID transform functions to retrieve
     * the L3_EIF_ID associated with the TNL_ENCAP_ID undergoing transform.
     */
    l3_eif_id = in->field[0]->data;
    *((uint32_t *)arg->comp_data->user_data) = l3_eif_id;

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = l3_eif_id;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmtnl_l3_eif_id_rev_transform is reverse transform
 * for saving EGR_L3_INTF.__INDEX.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Input field values.
 * \param [out] out             Output field values.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_l3_eif_id_rev_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    uint32_t    l3_eif_id = 0;
    uint32_t    field_count = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    if (field_count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 1) || (in->field == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    l3_eif_id = in->field[0]->data;
    *((uint32_t *)arg->comp_data->user_data) = l3_eif_id;

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = l3_eif_id;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmtnl_encap_id_transform is forward transform
 * for TNL_ENCAP_ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Input field values.
 * \param [out] out             Output field values.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_encap_id_transform(int unit,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_transform_arg_t *arg)
{
    uint32_t    field_count = 0;
    uint32_t    fid = 0, fval = 0;
    uint32_t    idx = 0;
    uint8_t     tnl_type = 0;
    uint32_t    tnl_id = 0;
    uint32_t    hw_tnl_idx = 0;
    uint32_t    l3_eif_id = 0;
    uint32_t    shift_bits = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    if (field_count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 2) || (in->field == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->value == NULL) ||
        (arg->rfield_list == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < field_count; idx++) {
        if (in->field[idx] == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        fid = in->field[idx]->id;
        fval = in->field[idx]->data;

        switch (fid) {
            case L3_EIFt_TNL_TYPEf:
                tnl_type = fval;
                break;
            case L3_EIFt_TNL_ENCAP_IDf:
                tnl_id = fval;
                break;
            default:
                break;
        }
    }

    l3_eif_id = *((uint32_t *)arg->comp_data->user_data);

    /*
     * Convert TNL_ENCAP_ID to EGR_L3_INTF.MPLS_TUNNEL_INDEX.
     */
    shift_bits = arg->value[0];
    if (tnl_type == BCMTNL_TYPE_IPV4) {
        hw_tnl_idx = (tnl_id << shift_bits);
    } else if (tnl_type == BCMTNL_TYPE_IPV6) {
        hw_tnl_idx = ((tnl_id * 2) << shift_bits);
    } else if (tnl_type == BCMTNL_TYPE_MPLS) {
        hw_tnl_idx = tnl_id;
    } else {
        hw_tnl_idx = 0;
    }

    BCMTNL_L3_EGR_INTF(unit)[l3_eif_id].type = tnl_type;
    BCMTNL_L3_EGR_INTF(unit)[l3_eif_id].tnl_id = tnl_id;

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = hw_tnl_idx;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmtnl_encap_id_rev_transform is reverse transform
 * for TNL_ENCAP_ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Input field values.
 * \param [out] out             Output field values.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmtnl_encap_id_rev_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    uint32_t    field_count = 0;
    uint32_t    idx = 0, fid = 0;
    uint8_t     tnl_type = 0;
    uint32_t    tnl_id = 0;
    uint32_t    l3_eif_id = 0;
    bcmltd_field_t **flist = NULL;
    const bcmltd_field_desc_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;

    if (field_count == 0) {
        SHR_EXIT();
    }

    if ((field_count < 1) || (in->field == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    l3_eif_id = *((uint32_t *)arg->comp_data->user_data);

    tnl_type = BCMTNL_L3_EGR_INTF(unit)[l3_eif_id].type;
    tnl_id   = BCMTNL_L3_EGR_INTF(unit)[l3_eif_id].tnl_id;

    flist = out->field;
    finfo = arg->rfield_list->field_array;
    field_count = arg->rfield_list->field_num;

    out->count = 0;

    for (idx = 0; idx < field_count; idx++) {
        if (flist[idx] == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        fid = finfo[idx].field_id;
        switch (fid) {
        case L3_EIFt_TNL_TYPEf:
            flist[idx]->id = L3_EIFt_TNL_TYPEf;
            flist[idx]->data = tnl_type;
            out->count++;
            break;
        case L3_EIFt_TNL_ENCAP_IDf:
            flist[idx]->id = L3_EIFt_TNL_ENCAP_IDf;
            flist[idx]->data = tnl_id;
            out->count++;
            break;
        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
