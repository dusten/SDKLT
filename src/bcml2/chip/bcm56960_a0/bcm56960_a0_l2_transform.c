/*! \file bcm56960_a0_l2_transform.c
 *
 * L2 Transform Handler for bcm56960 a0.
 *
 * This file contains field transform information for L2 LTs.
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
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcml2/common/bcml2_transform_types.h>
#include <bcml2/chip/bcml2_transform_chip.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCML2_TRANSFORM
#define MC_MAC_BIT_POS        40
typedef enum lt_dest_type_s {
    LT_PORT      = 0,
    LT_L2_MC_GRP = 1,
    LT_TRUNK     = 2
} lt_dest_type_t;

bcml2_mac_type_t bcml2_mac_type;
/******************************************************************************
* Private functions
 */

/*
 * \brief L2_FDB_VLAN destination forward transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            L2_FDB_VLAN.DEST_TYPE,
 *                            MODID, MODPORT,TRUNK_ID,L2_MC_GRP_ID.
 * \param [out] out           L2X.T/TGID/MODULE_ID/PORT_NUM/L2MC_PTR fields.
 * \param [in]  arg           Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
l2_fdb_vlan_dest_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    int rv = SHR_E_NONE;
    uint32_t dest_type_field_id = L2v_DEST_TYPEf;
    uint32_t tgid_field_id = L2v_TGIDf;
    uint32_t module_id_field_id = L2v_MODULE_IDf;
    uint32_t port_num_field_id = L2v_PORT_NUMf;
    uint32_t l2mc_ptr_field_id = L2v_L2MC_PTRf;
    uint64_t lt_dest_type_field_val = 0;
    uint64_t tgid_field_val = 0;
    uint64_t module_id_field_val = 0;
    uint64_t port_num_field_val = 0;
    uint64_t l2mc_ptr_field_val = 0;
    bcml2_mac_type_t *shared_data;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t l2_fdb_vlan_dest_transform\n")));

    shared_data = arg->comp_data->user_data;
    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_RETURN_VAL_EXIT(rv);
    }

    for (i = 0; i < in->count; i++) {
        switch (in->field[i]->id) {
        case L2_FDB_VLANt_DEST_TYPEf:
            lt_dest_type_field_val = in->field[i]->data;
            break;
        case L2_FDB_VLANt_TRUNK_IDf:
            tgid_field_val = in->field[i]->data;
            break;
        case L2_FDB_VLANt_MODIDf:
            module_id_field_val = in->field[i]->data;
            break;
        case L2_FDB_VLANt_MODPORTf:
            port_num_field_val = in->field[i]->data;
            break;
        case L2_FDB_VLANt_L2_MC_GRP_IDf:
            l2mc_ptr_field_val = in->field[i]->data;
            break;
        default:
            rv = SHR_E_PARAM;
            SHR_RETURN_VAL_EXIT(rv);
            break;
        }
    }

    if (shared_data->is_mc == TRUE) {
        if (lt_dest_type_field_val != LT_L2_MC_GRP) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    } else {
        if (lt_dest_type_field_val == LT_L2_MC_GRP) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    out->count = 0;
    if (lt_dest_type_field_val == LT_PORT) {
        out->field[out->count]->id = dest_type_field_id;
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = module_id_field_id;
        out->field[out->count]->data = module_id_field_val;
        out->count++;
        out->field[out->count]->id = port_num_field_id;
        out->field[out->count]->data = port_num_field_val;
        out->count++;
    } else if (lt_dest_type_field_val == LT_TRUNK) {
        out->field[out->count]->id = dest_type_field_id;
        out->field[out->count]->data = 1;
        out->count++;
        out->field[out->count]->id = tgid_field_id;
        out->field[out->count]->data = tgid_field_val;
        out->count++;
    } else {
        out->field[out->count]->id = dest_type_field_id;
        out->field[out->count]->data = 0;
        out->count++;
        out->field[out->count]->id = l2mc_ptr_field_id;
        out->field[out->count]->data = l2mc_ptr_field_val;
        out->count++;
    }

    SHR_RETURN_VAL_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN destination reverse transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            L2X.T/TGID/MODULE_ID/PORT_NUM/L2MC_PTR fields.
 * \param [out] out           L2_FDB_VLAN.DEST_TYPE,
 *                            MODID, MODPORT,TRUNK_ID,L2_MC_GRP_ID
 * \param [in]  arg           Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
l2_fdb_vlan_dest_rev_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    size_t i;
    int rv = SHR_E_NONE;
    uint32_t dest_type_field_id = L2_FDB_VLANt_DEST_TYPEf;
    uint32_t trunk_id_field_id = L2_FDB_VLANt_TRUNK_IDf;
    uint32_t modid_field_id = L2_FDB_VLANt_MODIDf;
    uint32_t modport_field_id = L2_FDB_VLANt_MODPORTf;
    uint32_t l2_mc_grp_id_field_id = L2_FDB_VLANt_L2_MC_GRP_IDf;
    uint64_t dest_type_field_val = 0;
    uint64_t trunk_id_field_val = 0;
    uint64_t modid_field_val = 0;
    uint64_t modport_field_val = 0;
    uint64_t l2_mc_grp_id_field_val = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\t l2_fdb_vlan_gport_rev_transform\n")));
    for (i = 0; i < in->count; i++) {
        switch (in->field[i]->id) {
        case L2v_DEST_TYPEf:
            dest_type_field_val = in->field[i]->data;
            break;
        case L2v_TGIDf:
            trunk_id_field_val = in->field[i]->data;
            break;
        case L2v_MODULE_IDf:
            modid_field_val = in->field[i]->data;
            break;
        case L2v_PORT_NUMf:
            modport_field_val = in->field[i]->data;
            break;
        case L2v_L2MC_PTRf:
            l2_mc_grp_id_field_val = in->field[i]->data;
            break;
        default:
            rv = SHR_E_PARAM;
            SHR_RETURN_VAL_EXIT(rv);
            break;
        }
    }

    out->count = 0;
    if (dest_type_field_val == 1) {
        out->field[out->count]->id = dest_type_field_id;
        out->field[out->count]->data = LT_TRUNK;
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
        out->field[out->count]->id = l2_mc_grp_id_field_id;
        out->field[out->count]->data = 0;
        out->count++;
    } else if (dest_type_field_val == 0) {
        if (((bcml2_mac_type_t *)arg->comp_data->user_data)->is_mc == TRUE) {
            out->field[out->count]->id = dest_type_field_id;
            out->field[out->count]->data = LT_L2_MC_GRP;
            out->count++;
            out->field[out->count]->id = l2_mc_grp_id_field_id;
            out->field[out->count]->data = l2_mc_grp_id_field_val;
            out->count++;
            out->field[out->count]->id = trunk_id_field_id;
            out->field[out->count]->data = 0;
            out->count++;
            out->field[out->count]->id = modid_field_id;
            out->field[out->count]->data = 0;
            out->count++;
            out->field[out->count]->id = modport_field_id;
            out->field[out->count]->data = 0;
            out->count++;
        } else {
            out->field[out->count]->id = dest_type_field_id;
            out->field[out->count]->data = LT_PORT;
            out->count++;
            out->field[out->count]->id = modid_field_id;
            out->field[out->count]->data = modid_field_val;
            out->count++;
            out->field[out->count]->id = modport_field_id;
            out->field[out->count]->data = modport_field_val;
            out->count++;
            out->field[out->count]->id = trunk_id_field_id;
            out->field[out->count]->data = 0;
            out->count++;
            out->field[out->count]->id = l2_mc_grp_id_field_id;
            out->field[out->count]->data = 0;
            out->count++;
        }
    } else {
        rv = SHR_E_PARAM;
    }

    SHR_RETURN_VAL_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN.MAC_ADDR forward transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            L2_FDB_VLAN.MAC_ADDR.
 * \param [out] out           L2X.L2::MAC_ADDR,
 * \param [in]  arg           Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
l2_fdb_vlan_mac_addr_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{

    SHR_FUNC_ENTER(unit);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\t l2_fdb_vlan_mac_addr_transform\n")));
    if (in == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (in->count == 0) {
        /* First call in LT traverse case. */
        out->count = 0;
        out->field[out->count]->id	 = L2v_MAC_ADDRf;
        out->field[out->count]->data = 0;
        out->count++;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    out->count = 0;
    out->field[out->count]->id   = L2v_MAC_ADDRf;
    out->field[out->count]->data = in->field[0]->data;
    out->count++;

    if ((in->field[0]->data >> MC_MAC_BIT_POS) & 1) {
        ((bcml2_mac_type_t *)arg->comp_data->user_data)->is_mc = TRUE;
    } else {
        ((bcml2_mac_type_t *)arg->comp_data->user_data)->is_mc = FALSE;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN.MAC_ADDR reverse transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            L2_FDB_VLAN.MAC_ADDR.
 * \param [out] out           L2X.L2::MAC_ADDR,
 * \param [in]  arg           Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
l2_fdb_vlan_mac_addr_rev_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\t l2_fdb_vlan_mac_addr_rev_transform\n")));

    if (in == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (in->count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    out->field[out->count]->id   = L2_FDB_VLANt_MAC_ADDRf;
    out->field[out->count]->data = in->field[0]->data;
    out->count++;

    if ((in->field[0]->data >> MC_MAC_BIT_POS) & 1) {
        ((bcml2_mac_type_t *)arg->comp_data->user_data)->is_mc = TRUE;
    } else {
        ((bcml2_mac_type_t *)arg->comp_data->user_data)->is_mc = FALSE;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}
/******************************************************************************
* Public functions
 */
/*!
 * \brief Attach bcm56960 L2 transform interfaces.
 *
 * Attach L2  transform interfaces.
 *
 * \param [in] unit Unit number.
 * \param [in] db   L2 transform interfaces database.
 *
 * \retval SHR_E_NONE No errors.
  * \retval SHR_E_PARAM The input parameter db is NULL.
 * \retval SHR_E_FAIL Fail to attach L2  transform interfaces.
 */
int
bcm56960_a0_l2_transform_attach(int unit, l2_transform_t *db)
{
    SHR_FUNC_ENTER(unit);

    if (db == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    db->l2_fdb_vlan_dest_fwd      = l2_fdb_vlan_dest_transform;
    db->l2_fdb_vlan_dest_rev      = l2_fdb_vlan_dest_rev_transform;
    db->l2_fdb_vlan_mac_addr_fwd  = l2_fdb_vlan_mac_addr_transform;
    db->l2_fdb_vlan_mac_addr_rev  = l2_fdb_vlan_mac_addr_rev_transform;

exit:
    SHR_FUNC_EXIT();
}
