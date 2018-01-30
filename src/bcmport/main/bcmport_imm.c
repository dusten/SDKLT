/*! \file bcmport_imm.c
 *
 * BCMPORT interface to in-memory table.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include "bcmport_internal.h"
#include <bcmport/bcmport_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPORT_IMM

/*! PORT fields array lmm handler. */
static shr_famm_hdl_t port_fld_arr_hdl;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief imm PORT_ING_MIRRORt notification input fields parsing.
 *
 * Parse imm PORT_ING_MIRRORt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port ingress mirror data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_imirror_lt_fields_parse(int unit,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             port_imirror_t *imirror)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(imirror, 0, sizeof(*imirror));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_ING_MIRRORt_PORT_IDf:
            imirror->port = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        case PORT_ING_MIRRORt_MIRROR_INSTANCE_IDf:
            imirror->instance_id = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_ING_MIRRORt_MIRROR_ENABLEf:
            imirror->enable = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_ING_MIRROR IMM table change callback function for staging.
 *
 * Handle PORT_ING_MIRROR IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_ing_mirror_imm_stage_callback(int unit,
                                   bcmltd_sid_t sid,
                                   uint32_t trans_id,
                                   bcmimm_entry_event_t event_reason,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context,
                                   bcmltd_fields_t *output_fields)
{
    port_imirror_t cfg;
    bool m_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_imirror_lt_fields_parse(unit, key, data, &cfg));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!SHR_BITGET(cfg.fbmp, PORT_ING_MIRRORt_MIRROR_ENABLEf)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        m_en = cfg.enable;
        break;
    case BCMIMM_ENTRY_DELETE:
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmport_ingress_mirror_set(unit,
                                    sid,
                                    trans_id,
                                    cfg.port,
                                    cfg.instance_id,
                                    m_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_ING_MIRROR In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to PORT_ING_MIRROR logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_ing_mirror_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = port_ing_mirror_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_ENG_MIRRORt notification input fields parsing.
 *
 * Parse imm PORT_EGR_MIRRORt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] emirror Port egress mirror info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_emirror_lt_fields_parse(int unit,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             port_emirror_t *emirror)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(emirror, 0, sizeof(*emirror));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_EGR_MIRRORt_PORT_IDf:
            emirror->port = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        case PORT_EGR_MIRRORt_EGR_PORT_IDf:
            emirror->egr_port = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        case PORT_EGR_MIRRORt_MIRROR_INSTANCE_IDf:
            emirror->instance_id = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_EGR_MIRRORt_MIRROR_ENABLEf:
            emirror->enable = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_EGR_MIRROR IMM table change callback function for staging.
 *
 * Handle PORT_EGR_MIRROR IMM logical table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_egr_mirror_imm_stage_callback(int unit,
                                   bcmltd_sid_t sid,
                                   uint32_t trans_id,
                                   bcmimm_entry_event_t event_reason,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context,
                                   bcmltd_fields_t *output_fields)
{
    port_emirror_t cfg;
    bool m_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_emirror_lt_fields_parse(unit, key, data, &cfg));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!SHR_BITGET(cfg.fbmp, PORT_EGR_MIRRORt_MIRROR_ENABLEf)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        m_en = cfg.enable;
        break;
    case BCMIMM_ENTRY_DELETE:
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmport_egress_mirror_set(unit,
                                    sid,
                                    trans_id,
                                    cfg.port,
                                    cfg.egr_port,
                                    cfg.instance_id,
                                    m_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_EGR_MIRROR In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to PORT_PORT_ING_MIRROR logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_egr_mirror_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = port_egr_mirror_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_BRIDGEt notification input fields parsing.
 *
 * Parse imm PORT_BRIDGEt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port ingress mirror data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
port_bridge_lt_fields_parse(int unit,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            port_bridge_info_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(port_bridge_info_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_BRIDGEt_PORT_IDf:
            lt_info->member_bmp |= MEMBER_BMP_PORT_ID;
            lt_info->port        = fval;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_BRIDGEt_BRIDGEf:
            lt_info->member_bmp |= MEMBER_BMP_BRIDGE;
            lt_info->bridge      = fval;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_BRIDGE IMM table change callback function for staging.
 *
 * Handle PORT_BRIDGE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_bridge_imm_callback_stage(int unit,
                               bcmltd_sid_t sid,
                               uint32_t trans_id,
                               bcmimm_entry_event_t event_reason,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    port_bridge_info_t lt_info;
    bool bridge = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_bridge_lt_fields_parse(unit, key, data, &lt_info));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!(lt_info.member_bmp & MEMBER_BMP_BRIDGE)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        bridge = lt_info.bridge;
        break;
    case BCMIMM_ENTRY_DELETE:
        bridge = 0;
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmport_port_bridge_set(unit,
                                trans_id,
                                sid,
                                lt_info.port,
                                bridge));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_BRIDGE In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to PORT_BRIDGE logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_bridge_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = port_bridge_imm_callback_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_MEMBERSHIP_POLICYt notification input fields parsing.
 *
 * Parse imm PORT_MEMBERSHIP_POLICYt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port ingress mirror data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
port_membership_policy_lt_fields_parse(int unit,
                                       const bcmltd_field_t *key,
                                       const bcmltd_field_t *data,
                                       port_membership_policy_info_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(port_membership_policy_info_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case PORT_MEMBERSHIP_POLICYt_PORT_IDf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_PORT_ID);
                lt_info->port_id = (bcmport_id_t) fval;
                break;

        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case PORT_MEMBERSHIP_POLICYt_ING_VLAN_MEMBERSHIP_CHECKf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_ING_VLAN_MEMBERSHIP_CHECK);
                lt_info->ing_vlan_membership_check = (bool) fval;
                break;
            case PORT_MEMBERSHIP_POLICYt_EGR_VLAN_MEMBERSHIP_CHECKf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_EGR_VLAN_MEMBERSHIP_CHECK);
                lt_info->egr_vlan_membership_check = (bool) fval;
                break;
            case PORT_MEMBERSHIP_POLICYt_SKIP_VLAN_CHECKf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_SKIP_VLAN_CHECK);
                lt_info->skip_vlan_check = (bool) fval;
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PORT_POLICY LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
port_membership_policy_info_default_values_init(int unit,
                                                port_membership_policy_info_t *lt_info)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (!BCMPORT_LT_FIELD_GET(
             lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_ING_VLAN_MEMBERSHIP_CHECK)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     PORT_MEMBERSHIP_POLICYt,
                                     PORT_MEMBERSHIP_POLICYt_ING_VLAN_MEMBERSHIP_CHECKf,
                                     1, &def_val, &num));
        lt_info->ing_vlan_membership_check = def_val ? true : false;
        BCMPORT_LT_FIELD_SET(lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_ING_VLAN_MEMBERSHIP_CHECK);
    }

    if (!BCMPORT_LT_FIELD_GET(
             lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_EGR_VLAN_MEMBERSHIP_CHECK)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     PORT_MEMBERSHIP_POLICYt,
                                     PORT_MEMBERSHIP_POLICYt_EGR_VLAN_MEMBERSHIP_CHECKf,
                                     1, &def_val, &num));
        lt_info->egr_vlan_membership_check = def_val ? true : false;
        BCMPORT_LT_FIELD_SET(lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_EGR_VLAN_MEMBERSHIP_CHECK);
    }

    if (!BCMPORT_LT_FIELD_GET(
                     lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_SKIP_VLAN_CHECK)) {
        int rv = SHR_E_NONE;
        const bcmlrd_field_data_t *field_info;

        rv = bcmlrd_field_get(unit, PORT_MEMBERSHIP_POLICYt,
                              PORT_MEMBERSHIP_POLICYt_SKIP_VLAN_CHECKf, &field_info);
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_EXIT(
                bcmlrd_field_default_get(unit,
                                         PORT_MEMBERSHIP_POLICYt,
                                         PORT_MEMBERSHIP_POLICYt_SKIP_VLAN_CHECKf,
                                         1, &def_val, &num));
            lt_info->skip_vlan_check = def_val ? true : false;
            BCMPORT_LT_FIELD_SET(lt_info->fbmp, BCMPORT_MEMBERSHIP_POLICY_LT_FIELD_SKIP_VLAN_CHECK);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief PORT_POLICY IMM table change callback function for staging.
 *
 * Handle PORT_POLICY IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_membership_policy_imm_callback_stage(int unit,
                                          bcmltd_sid_t sid,
                                          uint32_t trans_id,
                                          bcmimm_entry_event_t event_reason,
                                          const bcmltd_field_t *key,
                                          const bcmltd_field_t *data,
                                          void *context,
                                          bcmltd_fields_t *output_fields)
{
    port_membership_policy_info_t lt_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_membership_policy_lt_fields_parse(unit, key, data, &lt_info));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            break;
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(
                port_membership_policy_info_default_values_init(unit, &lt_info));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmport_port_membership_policy_set(unit,
                                           trans_id,
                                           sid, lt_info.port_id,
                                           &lt_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_BRIDGE In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to PORT_BRIDGE logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_membership_policy_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = port_membership_policy_imm_callback_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


/*******************************************************************************
 * Public Functions
 */

int
bcmport_imm_db_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!port_fld_arr_hdl) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(PORT_FIELD_COUNT_MAX, &port_fld_arr_hdl));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmport_imm_db_cleanup(int unit)
{
    if (port_fld_arr_hdl) {
        shr_famm_hdl_delete(port_fld_arr_hdl);
        port_fld_arr_hdl = 0;
    }
}

int
bcmport_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for PORT LTs here.
     */
    rv = bcmlrd_map_get(unit, PORT_ING_MIRRORt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_ING_MIRRORt,
                                 &port_ing_mirror_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, PORT_EGR_MIRRORt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_EGR_MIRRORt,
                                 &port_egr_mirror_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, PORT_BRIDGEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(unit,
                                PORT_BRIDGEt,
                                &port_bridge_imm_callback,
                                NULL));
    }

    rv = bcmlrd_map_get(unit, PORT_MEMBERSHIP_POLICYt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(unit,
                                PORT_MEMBERSHIP_POLICYt,
                                &port_membership_policy_imm_callback,
                                NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
