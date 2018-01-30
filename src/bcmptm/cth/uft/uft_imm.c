/*! \file uft_imm.c
 *
 * Uft interface to in-memory table.
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
#include <sal/sal_assert.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmevm/bcmevm_api.h>

#include "uft_device.h"
#include "uft_internal.h"
#include "uft_util.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief TABLE_EM_CONTROLt input fields parsing.
 *
 * Parse IMM TABLE_EM_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] ctrl Table exact match control data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
table_em_ctrl_fields_parse(int unit,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           uft_tbl_em_ctrl_t *ctrl)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != TABLE_EM_CONTROLt_TABLE_IDf) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    ctrl->lt_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
        case TABLE_EM_CONTROLt_DEVICE_EM_GROUP_IDf:
            ctrl->grp_id = fval;
            ctrl->grp_id_valid = TRUE;
            break;
        case TABLE_EM_CONTROLt_MOVE_DEPTHf:
            ctrl->move_depth = fval;
            ctrl->move_depth_valid = TRUE;
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
 * \brief TABLE_EM_CONTROL logical table validate callback function.
 *
 * Validate the field values of TABLE_EM_CONTROLt logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
table_em_ctrl_imm_validate(int unit,
                           bcmltd_sid_t sid,
                           bcmimm_entry_event_t event,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context)
{
    uft_tbl_em_ctrl_t tbl_ctrl;
    uint32_t num_actual;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    sal_memset(&tbl_ctrl, 0, sizeof(uft_tbl_em_ctrl_t));
    SHR_IF_ERR_EXIT
        (table_em_ctrl_fields_parse(unit, key, data, &tbl_ctrl));
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            /* For insert opeartaion, default value is set if not given. */
            if (!tbl_ctrl.move_depth_valid) {
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmlrd_field_default_get(
                        unit, TABLE_EM_CONTROLt,
                        TABLE_EM_CONTROLt_MOVE_DEPTHf,
                        1, &def_val, &num_actual));
                tbl_ctrl.move_depth = def_val;
                tbl_ctrl.move_depth_valid = TRUE;
            }

            SHR_IF_ERR_EXIT
                (bcmptm_uft_tbl_em_ctrl_validate(unit, &tbl_ctrl));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmptm_uft_tbl_em_ctrl_validate(unit, &tbl_ctrl));
            break;
        case BCMIMM_ENTRY_DELETE:
        case BCMIMM_ENTRY_LOOKUP:
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief TABLE_EM_CONTROL IMM table change callback function for staging.
 *
 * Handle TABLE_EM_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
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
table_em_ctrl_imm_stage(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        bcmimm_entry_event_t event,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    uft_tbl_em_ctrl_t ctrl;
    uint32_t num_actual;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ctrl, 0, sizeof(uft_tbl_em_ctrl_t));
    if (output_fields) {
        output_fields->count = 0;
    }
    SHR_IF_ERR_EXIT
        (table_em_ctrl_fields_parse(unit, key, data, &ctrl));
    SHR_IF_ERR_VERBOSE_EXIT(
    bcmlrd_field_default_get(unit, TABLE_EM_CONTROLt,
                             TABLE_EM_CONTROLt_MOVE_DEPTHf,
                             1, &def_val, &num_actual));
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            if (!ctrl.move_depth_valid) {

                ctrl.move_depth = def_val;
                ctrl.move_depth_valid = TRUE;
            }
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_lt_move_depth_update(unit,
                                                     ctrl.lt_id,
                                                     ctrl.move_depth));
            break;
        case BCMIMM_ENTRY_UPDATE:
            if (!ctrl.move_depth_valid) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_lt_move_depth_update(unit,
                                                         ctrl.lt_id,
                                                         ctrl.move_depth));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_lt_move_depth_update(unit,
                                                     ctrl.lt_id,
                                                     def_val));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
table_em_ctrl_imm_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    bcmevm_publish_event_notify(unit, "bcmptmEvUftLtMoveDepthChg", 0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief TABLE_EM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to TABLE_EM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t table_em_ctrl_imm_callback = {

    /*! Validate function. */
    .validate = table_em_ctrl_imm_validate,

    /*! Staging function. */
    .stage    = table_em_ctrl_imm_stage,

    /*! Commit function. */
    .commit   = table_em_ctrl_imm_commit,

    /*! Abort function. */
    .abort    = NULL
};

/*!
 * \brief DEVICE_EM_GROUPt input fields parsing.
 *
 * Parse IMM DEVICE_EM_GROUPt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] grp Device exact match group data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
dev_em_grp_fields_parse(int unit,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        uft_dev_em_grp_t *grp)
{
    const bcmltd_field_t *gen_field = NULL;
    uint32_t fid, fidx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    grp->grp_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        fidx = gen_field->idx;
        switch (fid) {
        case DEVICE_EM_GROUPt_ROBUSTf:
            grp->robust = fval;
            grp->robust_valid = TRUE;
            break;
        case DEVICE_EM_GROUPt_VECTOR_TYPEf:
            grp->vector_type = fval;
            grp->vector_type_valid = TRUE;
            break;
        case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
            if (fidx >= UFT_VAL_BANKS_MAX) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
            grp->bank[fidx] = fval;
            grp->bank_valid[fidx] = TRUE;
            break;
        case DEVICE_EM_GROUPt_NUM_BANKSf:
            grp->bank_cnt = fval;
            grp->bank_cnt_valid = TRUE;
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
 * \brief DEVICE_EM_GROUP logical table validate callback function.
 *
 * Validate the field values of DEVICE_EM_GROUPt logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
dev_em_grp_imm_validate(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_entry_event_t event,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context)
{
    uft_dev_em_grp_t grp, old_grp;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            break;
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&grp, 0, sizeof(uft_dev_em_grp_t));
            sal_memset(&old_grp, 0, sizeof(uft_dev_em_grp_t));
            SHR_IF_ERR_EXIT(dev_em_grp_fields_parse(unit, key, data, &grp));
            old_grp.grp_id = grp.grp_id;
            SHR_IF_ERR_EXIT(uft_lt_grp_get_from_imm(unit, &old_grp));
            if (!grp.vector_type_valid) {
                grp.vector_type = old_grp.vector_type;
            }
            if (!grp.bank_cnt_valid) {
                grp.bank_cnt = old_grp.bank_cnt;
            }
            for (idx = 0; idx < grp.bank_cnt; idx++) {
                if(!grp.bank_valid[idx]) {
                    grp.bank[idx] = old_grp.bank[idx];
                }
            }

            SHR_IF_ERR_EXIT
                (bcmptm_uft_dev_em_grp_validate(unit, &grp));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_GROUP IMM table change callback function for staging.
 *
 * Handle DEVICE_EM_GROUP IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
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
dev_em_grp_imm_stage(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context,
                     bcmltd_fields_t *output_fields)
{
    uft_dev_em_grp_t grp, old_grp;
    uint32_t idx;
    bcmptm_rm_hash_pt_chg_t pt_chg;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            break;
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&grp, 0, sizeof(uft_dev_em_grp_t));
            sal_memset(&old_grp, 0, sizeof(uft_dev_em_grp_t));
            sal_memset(&pt_chg, 0, sizeof(pt_chg));

            SHR_IF_ERR_EXIT(dev_em_grp_fields_parse(unit, key, data, &grp));
            old_grp.grp_id = grp.grp_id;
            SHR_IF_ERR_EXIT(uft_lt_grp_get_from_imm(unit, &old_grp));
            if (!grp.vector_type_valid) {
                grp.vector_type = old_grp.vector_type;
            }
            if (!grp.bank_cnt_valid) {
                grp.bank_cnt = old_grp.bank_cnt;
            }
            for (idx = 0; idx < grp.bank_cnt; idx++) {
                if(!grp.bank_valid[idx]) {
                    grp.bank[idx] = old_grp.bank[idx];
                }
            }

            SHR_IF_ERR_EXIT
                (bcmptm_uft_dev_em_grp_set(unit, sid, trans_id,
                                           &grp, &pt_chg));
           SHR_IF_ERR_EXIT
               (bcmptm_rm_hash_pt_info_update(unit, &pt_chg));
            break;
         case BCMIMM_ENTRY_LOOKUP:
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
         default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_grp_imm_commit(int unit, bcmltd_sid_t sid,
                      uint32_t trans_id, void *context)
{
    SHR_FUNC_ENTER(unit);

    bcmevm_publish_event_notify(unit, "bcmptmEvUftBankChg", 0);

    SHR_FUNC_EXIT();
}

static void
dev_em_grp_imm_abort(int unit, bcmltd_sid_t sid,
                     uint32_t trans_id, void *context)
{

}

/*!
 * \brief DEVICE_EM_GROUP In-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to DEVICE_EM_GROUP logical table entry commit stages.
 */
static bcmimm_lt_cb_t dev_em_grp_imm_callback = {

    /*! Validate function. */
    .validate = dev_em_grp_imm_validate,

    /*! Staging function. */
    .stage    = dev_em_grp_imm_stage,

    /*! Commit function. */
    .commit   = dev_em_grp_imm_commit,

    /*! Abort function. */
    .abort    = dev_em_grp_imm_abort
};

/*!
 * \brief DEVICE_EM_BANKt input fields parsing.
 *
 * Parse IMM DEVICE_EM_BANKt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] grp Device exact match group data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
dev_em_bank_fields_parse(int unit,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         uft_dev_em_bank_t *bank)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    bank->bank_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
            case DEVICE_EM_BANKt_OFFSETf:
                bank->offset = fval;
                bank->offset_valid = TRUE;
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
 * \brief DEVICE_EM_BANK logical table validate callback function.
 *
 * Validate the field values of DEVICE_EM_BANKt logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
dev_em_bank_imm_validate(int unit,
                         bcmltd_sid_t sid,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context)
{
    uft_dev_em_bank_t bank;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            break;
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&bank, 0, sizeof(uft_dev_em_bank_t));
            SHR_IF_ERR_EXIT
                (dev_em_bank_fields_parse(unit, key, data, &bank));
            SHR_IF_ERR_EXIT
                (bcmptm_uft_dev_em_bank_validate(unit, &bank));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            break;
         default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_BANK IMM table change callback function for staging.
 *
 * Handle DEVICE_EM_BANK IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
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
dev_em_bank_imm_stage(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      bcmimm_entry_event_t event,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data,
                      void *context,
                      bcmltd_fields_t *output_fields)
{
    uft_dev_em_bank_t bank;
    bcmptm_rm_hash_pt_chg_t pt_chg;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            break;
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&pt_chg, 0, sizeof(pt_chg));
            sal_memset(&bank, 0, sizeof(uft_dev_em_bank_t));
            SHR_IF_ERR_EXIT
                (dev_em_bank_fields_parse(unit, key, data, &bank));
            SHR_IF_ERR_EXIT
                (bcmptm_uft_dev_em_bank_set(unit, sid, trans_id,
                                            &bank, &pt_chg));
            SHR_IF_ERR_EXIT
                 (bcmptm_rm_hash_pt_info_update(unit, &pt_chg));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_bank_imm_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    bcmevm_publish_event_notify(unit, "bcmptmEvUftBankChg", 0);

    SHR_FUNC_EXIT();
}

static void
dev_em_bank_imm_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{

}

/*!
 * \brief DEVICE_EM_BANK in-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to DEVICE_EM_BANK logical table entry commit stages.
 */
static bcmimm_lt_cb_t dev_em_bank_imm_callback = {

    /*! Validate function. */
    .validate = dev_em_bank_imm_validate,

    /*! Staging function. */
    .stage    = dev_em_bank_imm_stage,

    /*! Commit function. */
    .commit   = dev_em_bank_imm_commit,

    /*! Abort function. */
    .abort    = dev_em_bank_imm_abort
};


/*******************************************************************************
 * Public Functions
 */

int
bcmptm_uft_imm_register_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for TABLE_EM_CONTROL LT here.
     */
    rv = bcmlrd_map_get(unit, TABLE_EM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 TABLE_EM_CONTROLt,
                                 &table_em_ctrl_imm_callback,
                                 NULL));
    }

    /*
     * To register callback for DEVICE_EM_GROUP LT here.
     */
    rv = bcmlrd_map_get(unit, DEVICE_EM_GROUPt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DEVICE_EM_GROUPt,
                                 &dev_em_grp_imm_callback,
                                 NULL));
    }

    /*
     * To register callback for DEVICE_EM_BANK LT here.
     */
    rv = bcmlrd_map_get(unit, DEVICE_EM_BANKt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DEVICE_EM_BANKt,
                                 &dev_em_bank_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
