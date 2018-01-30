/*! \file bcml2_learn_imm.c
 *
 * L2 Learn interfaces to IMM.
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
#include <bcmimm/bcmimm_int_comp.h>

#include <bcml2/bcml2_util.h>
#include <bcml2/common/bcml2_learn_imm.h>
#include <bcml2/common/bcml2_learn_ctrl.h>
/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCML2_LEARN_IMM

/*! Data structure to save the info from L2_LEARN_CONTROL LT entry. */
typedef struct l2_learn_control_info_s {
    /*! Bitmap of valid struct members. */
    uint32_t        member_bmp;
#define             MEMBER_BMP_ENABLE         (1 << 0)

    /*! Enable L2 report. */
    bool            enable;
} l2_learn_control_info_t;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief LT L2_LEARN_CONTROL fields parsing.
 *
 * Parse LT L2_LEARN_CONTROL input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key, LT input key field array.
 * \param [in] data, LT input data field array.
 * \param [out] lt_info Parse result of LT fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
l2_learn_control_fields_parse(int unit,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              l2_learn_control_info_t *lt_info)
{
    const bcmltd_field_t *lt_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(key);

    /* Parse data fields. */
    lt_field = data;
    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;

        switch (fid) {
        case L2_LEARN_CONTROLt_REPORTf:
            lt_info->enable = fval ? TRUE : FALSE;
            lt_info->member_bmp |= MEMBER_BMP_ENABLE;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        lt_field = lt_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LT L2_LEARN_DATA  key fields parsing.
 *
 * Parse LT L2_LEARN_DATA input key fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key, LT input key field array.
 * \param [in] data, LT input data field array.
 * \param [out] lt_info Parse result of LT fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
l2_learn_key_fields_parse(int unit,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          l2_learn_addr_t *l2addr)
{
    const bcmltd_field_t *lt_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t fval32[2] = {0};

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(data);

    /* Parse data fields. */
    lt_field = key;
    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;

        switch (fid) {
        case L2_LEARN_DATAt_VLAN_IDf:
            l2addr->vid = (uint16_t)fval;
            break;
        case L2_LEARN_DATAt_MAC_ADDRf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            SAL_MAC_ADDR_FROM_UINT32(l2addr->mac,fval32);
            break;
        case L2_LEARN_DATAt_DEST_TYPEf:
            l2addr->dest_type = (uint8_t)fval;
            break;
        case L2_LEARN_DATAt_TRUNK_IDf:
            l2addr->trunk_id  = (uint8_t)fval;
            break;
        case L2_LEARN_DATAt_MODIDf:
            l2addr->modid     = (uint8_t)fval;
            break;
        case L2_LEARN_DATAt_MODPORTf:
            l2addr->port      = (uint8_t)fval;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        lt_field = lt_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L2_LEARN_CONTROL IMM table change callback function for staging.
 *
 * Handle L2_LEARN_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
  * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL L2 Learn fails to handle LT change events.
 */
static int
l2_learn_control_cth(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t entry_event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context,
                     bcmltd_fields_t *output_fields)
{
    l2_learn_control_info_t lt_info;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(context);

    if (output_fields) {
        output_fields->count = 0;
    }
    sal_memset(&lt_info, 0, sizeof(l2_learn_control_info_t));
    SHR_IF_ERR_VERBOSE_EXIT(
        l2_learn_control_fields_parse(unit, key, data, &lt_info));

    switch (entry_event) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (lt_info.member_bmp & MEMBER_BMP_ENABLE) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcml2_learn_ctrl_report_enable(unit, lt_info.enable));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcml2_learn_ctrl_report_enable(unit, 0));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L2_LEARN_DATA IMM table opcode validate.
 *
 * Validate L2_LEARN_DATA IMM table opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
  * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE  L2_LEARN_DATA does support the opcode.
 * \retval SHR_E_UNAVAIL L2_LEARN_DATA does not support the opcode.
 */
static int
l2_learn_data_validate(int unit,
                       bcmltd_sid_t sid,
                       bcmimm_entry_event_t entry_event,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(key);
    COMPILER_REFERENCE(data);
    COMPILER_REFERENCE(context);

    switch (entry_event) {
    case BCMIMM_ENTRY_DELETE:
    case BCMIMM_ENTRY_LOOKUP:
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L2_LEARN_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that is conresponding
 * to L2_LEARN_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t l2_learn_control_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = l2_learn_control_cth,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief L2_LEARN_DATA IMM table change callback function for staging.
 *
 * Handle L2_LEARN_DATA IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
  * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL L2 Learn fails to handle LT change events.
 */
static int
l2_learn_data_cth(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t entry_event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    l2_learn_addr_t l2addr;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(context);

    if (output_fields) {
        output_fields->count = 0;
    }
    sal_memset(&l2addr, 0, sizeof(l2_learn_addr_t));

    switch (entry_event) {
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT(
            l2_learn_key_fields_parse(unit, key, data, &l2addr));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcml2_learn_ctrl_cache_entry_del(unit, trans_id, &l2addr));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L2_LEARN_DATA In-memory event callback structure.
 *
 * This structure contains callback functions that is conresponding
 * to L2_LEARN_DATA logical table entry commit stages.
 */
static bcmimm_lt_cb_t l2_learn_data_imm_callback = {

    /*! Validate function. */
    .validate = l2_learn_data_validate,

    /*! Staging function. */
    .stage = l2_learn_data_cth,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};
/*******************************************************************************
 * Public Functions
 */
int
bcml2_learn_imm_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for L2 Learn LTs here.
     */
    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            L2_LEARN_CONTROLt,
                            &l2_learn_control_imm_callback,
                            NULL));
    SHR_IF_ERR_EXIT(
        bcmimm_lt_event_reg(unit,
                            L2_LEARN_DATAt,
                            &l2_learn_data_imm_callback,
                            NULL));
exit:
    SHR_FUNC_EXIT();
}
