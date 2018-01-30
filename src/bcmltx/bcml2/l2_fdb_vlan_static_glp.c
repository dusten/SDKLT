/*! \file l2_fdb_vlan_static_glp.c
 *
 * L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODID/MODPORT Transform Handler
 *
 * This file contains field transform information for
 * L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODID/MODPORT.
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
#include <bcmltx/bcml2/bcmltx_l2_fdb_vlan_static_glp.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L2

/*
 * \brief L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODID/MODPORT forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODID/MODPORT.
 * \param [out] out             L2_USER_ENTRY.T/TGID/MODULE_ID/PORT_NUM fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_static_glp_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    int rv = SHR_E_NONE;
    uint32_t t_field_id = Tf;
    uint32_t tgid_field_id = TGIDf;
    uint32_t module_id_field_id = MODULE_IDf;
    uint32_t port_num_field_id = PORT_NUMf;
    uint64_t t_field_val = 0;
    uint64_t tgid_field_val = 0;
    uint64_t module_id_field_val = 0;
    uint64_t port_num_field_val = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcml2_l2_fdb_vlan_static_glp_transform\n")));

    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }

    for (i = 0; i < in->count; i++) {
        switch (in->field[i]->id) {
        case L2_FDB_VLAN_STATICt_IS_TRUNKf:
            t_field_val = in->field[i]->data;
            break;
        case L2_FDB_VLAN_STATICt_TRUNK_IDf:
            tgid_field_val = in->field[i]->data;
            break;
        case L2_FDB_VLAN_STATICt_MODIDf:
            module_id_field_val = in->field[i]->data;
            break;
        case L2_FDB_VLAN_STATICt_MODPORTf:
            port_num_field_val = in->field[i]->data;
            break;
        default:
            rv = SHR_E_PARAM;
            SHR_RETURN_VAL_EXIT(rv);
            break;
        }
    }

    out->count = 0;
    if (t_field_val == 0) {
        out->field[out->count]->id = t_field_id;
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = module_id_field_id;
        out->field[out->count]->data = module_id_field_val;
        out->count++;
        out->field[out->count]->id = port_num_field_id;
        out->field[out->count]->data = port_num_field_val;
        out->count++;
    } else {
        out->field[out->count]->id = t_field_id;
        out->field[out->count]->data = 1;
        out->count++;
        out->field[out->count]->id = tgid_field_id;
        out->field[out->count]->data = tgid_field_val;
        out->count++;
   }

    SHR_RETURN_VAL_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODID/MODPORT forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              L2_USER_ENTRY.T/TGID/MODULE_ID/PORT_NUM fields.
 * \param [out] out             L2_FDB_VLAN_STATIC.IS_TRUNK/TRUNK_ID/MODID/MODPORT.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_static_glp_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    size_t i;
    int rv = SHR_E_NONE;
    uint32_t is_trunk_field_id = L2_FDB_VLAN_STATICt_IS_TRUNKf;
    uint32_t trunk_id_field_id = L2_FDB_VLAN_STATICt_TRUNK_IDf;
    uint32_t modid_field_id = L2_FDB_VLAN_STATICt_MODIDf;
    uint32_t modport_field_id = L2_FDB_VLAN_STATICt_MODPORTf;
    uint64_t is_trunk_field_val = 0;
    uint64_t trunk_id_field_val = 0;
    uint64_t modid_field_val = 0;
    uint64_t modport_field_val = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcml2_l2_fdb_vlan_static_glp_rev_transform\n")));
    for (i = 0; i < in->count; i++) {
        switch (in->field[i]->id) {
        case Tf:
            is_trunk_field_val = in->field[i]->data;
            break;
        case TGIDf:
            trunk_id_field_val = in->field[i]->data;
            break;
        case MODULE_IDf:
            modid_field_val = in->field[i]->data;
            break;
        case PORT_NUMf:
            modport_field_val = in->field[i]->data;
            break;
        default:
            rv = SHR_E_PARAM;
            SHR_RETURN_VAL_EXIT(rv);
            break;
        }
    }

    out->count = 0;
    if (is_trunk_field_val == 0) {
        out->field[out->count]->id = is_trunk_field_id;
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = trunk_id_field_id;
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = modid_field_id;
        out->field[out->count]->data = modid_field_val;
        out->count++;
        out->field[out->count]->id = modport_field_id;
        out->field[out->count]->data = modport_field_val;
        out->count++;
    } else {
        out->field[out->count]->id = is_trunk_field_id;
        out->field[out->count]->data = 1;
        out->count++;
        out->field[out->count]->id = trunk_id_field_id;
        out->field[out->count]->data = trunk_id_field_val;
        out->count++;
        out->field[out->count]->id = modid_field_id;
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = modport_field_id;
        out->field[out->count]->data = 0;
        out->count++;
    }

    SHR_RETURN_VAL_EXIT(rv);
 exit:
    SHR_FUNC_EXIT()
}
