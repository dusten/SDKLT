/*! \file bcmfp_egress_imm.c
 *
 * In-memory call backs for Egress Field Processor stage.
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_egress_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmfp/bcmfp_lt_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmfp/map/bcmfp_lt_group_map.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_entry.h>
#include <bcmlrd/bcmlrd_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON


/*!
 * \brief FP_EGR_GRP_TEMPLATE table change callback function for staging.
 *
 * Handle FP_EGR_GRP_TEMPLATE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_egress_grp_config_stage(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              bcmimm_entry_event_t event_reason,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              void *context,
                              bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            if (data != NULL) {
                SHR_IF_ERR_EXIT(bcmfp_lt_egress_grp_config_update(unit,
                                                                  trans_id,
                                                                  sid,
                                                                  key,
                                                                  data));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT(bcmfp_lt_egress_grp_config_delete(unit,
                                                              trans_id,
                                                              sid,
                                                              key,
                                                              data));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP_EGR_GRP_TEMPLATE IMM logical table validate callback function.
 *
 * Validate the field values of the FP_EGR_GRP_TEMPLATE logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] event_reason Call back reason.
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
bcmfp_egress_grp_config_validate(int unit,
                                 bcmltd_sid_t sid,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 void *context) {
    SHR_FUNC_ENTER(unit);

    if (event_reason == BCMIMM_ENTRY_INSERT)  {
        SHR_NULL_CHECK(data, SHR_E_PARAM);
    }
    /*Skip validation for LOOKUP and DELETE operation */
    if ((event_reason != BCMIMM_ENTRY_LOOKUP) &&
        (event_reason != BCMIMM_ENTRY_DELETE))  {
        if (data != NULL) {
            SHR_RETURN_VAL_EXIT(bcmfp_lt_group_validate(unit,
                                                        sid,
                                                        event_reason,
                                                        key,
                                                        data));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP_EGR_RULE_TEMPLATE IMM table change callback function for staging.
 *
 * Handle FP_EGR_RULE_TEMPLATE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_egress_rule_config_stage(int unit,
                               bcmltd_sid_t sid,
                               uint32_t trans_id,
                               bcmimm_entry_event_t event_reason,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    /* Skip Custom handlers */
    if ((event_reason != BCMIMM_ENTRY_DELETE) &&
            (data == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_lt_update_entries(unit, trans_id, BCMFP_LT_TYPE_RULE,
                                         sid, key, data, event_reason));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP_EGR_POLICY_TEMPLATE IMM table change callback function for staging.
 *
 * Handle FP_EGR_POLICY_TEMPLATE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_egress_policy_config_stage(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 void *context,
                                 bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    /* Skip Custom handlers
     * No need to update as there is not change in the fields */
    if ((event_reason != BCMIMM_ENTRY_DELETE) &&
            (data == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_lt_update_entries(unit, trans_id, BCMFP_LT_TYPE_POLICY,
                                         sid, key, data, event_reason));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP_EGR_POLICY_TEMPLATE IMM logical table validate callback function.
 *
 * Validate the field values of the FP_EGR_POLICY_TEMPLATE logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] event_reason Call back reason.
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
bcmfp_egress_policy_config_validate(int unit,
                                    bcmltd_sid_t sid,
                                    bcmimm_entry_event_t event_reason,
                                    const bcmltd_field_t *key,
                                    const bcmltd_field_t *data,
                                    void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Skip Custom handlers */
    if (data == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /*Skip validation for LOOKUP and DELETE operation */
    if ((event_reason != BCMIMM_ENTRY_LOOKUP) &&
            (event_reason != BCMIMM_ENTRY_DELETE))  {
        SHR_RETURN_VAL_EXIT(bcmfp_lt_policy_validate(unit,
                                                     sid,
                                                     event_reason,
                                                     key,
                                                     data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP_EGR_ENTRY_TEMPLATE IMM table change callback function for staging.
 *
 * Handle FP_EGR_ENTRY_TEMPLATE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmfp_egress_entry_config_stage(int unit,
                                bcmltd_sid_t sid,
                                uint32_t trans_id,
                                bcmimm_entry_event_t event_reason,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                void *context,
                                bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    /* Skip Custom handlers */
    if ((event_reason != BCMIMM_ENTRY_DELETE) &&
            (data == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT(bcmfp_lt_entry_insert(unit, trans_id, sid, key, data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT(bcmfp_lt_entry_update(unit, trans_id, sid, key, data));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT(bcmfp_lt_entry_delete(unit, trans_id, sid, key));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP_EGR_ENTRY_TEMPLATE IMM logical table validate callback function.
 *
 * Validate the field values of the FP_EGR_ENTRY_TEMPLATE logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] event_reason Call back reason.
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
bcmfp_egress_entry_config_validate(int unit,
                                   bcmltd_sid_t sid,
                                   bcmimm_entry_event_t event_reason,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Group template is mandatory*/
    if (event_reason == BCMIMM_ENTRY_INSERT)  {
        SHR_NULL_CHECK(data, SHR_E_PARAM);
    }
    /*Skip validation for LOOKUP and DELETE operation */
    if ((event_reason != BCMIMM_ENTRY_LOOKUP) &&
            (event_reason != BCMIMM_ENTRY_DELETE))  {
        if (data != NULL) {
            SHR_RETURN_VAL_EXIT(bcmfp_lt_entry_validate(unit,
                                                        sid,
                                                        event_reason,
                                                        key,
                                                        data));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief FP_EGR_GRP_CONFIG In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to FP_EGR_GRP_CONFIG logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_egress_grp_config_imm_callback = {

  /*! Validate function. */
  .validate = bcmfp_egress_grp_config_validate,

  /*! Staging function. */
  .stage = bcmfp_egress_grp_config_stage,

  /*! Commit function. */
  .commit = bcmfp_egr_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_egr_trans_abort
};

/*!
 * \brief FP_EGR_RULE_CONFIG In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to FP_RULE_GRP_CONFIG logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_egress_rule_config_imm_callback = {

  /*! Validate function. */
  .validate = NULL,

  /*! Staging function. */
  .stage = bcmfp_egress_rule_config_stage,

  /*! Commit function. */
  .commit = bcmfp_egr_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_egr_trans_abort
};

/*!
 * \brief FP_EGR_POLICY_CONFIG In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to FP_RULE_GRP_CONFIG logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_egress_policy_config_imm_callback = {

  /*! Validate function. */
  .validate = bcmfp_egress_policy_config_validate,

  /*! Staging function. */
  .stage = bcmfp_egress_policy_config_stage,

  /*! Commit function. */
  .commit = bcmfp_egr_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_egr_trans_abort
};

/*!
 * \brief FP_EGR_ENTRY In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to FP_EGR_ENTRYt logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmfp_egress_entry_config_imm_callback = {

  /*! Validate function. */
  .validate = bcmfp_egress_entry_config_validate,

  /*! Staging function. */
  .stage = bcmfp_egress_entry_config_stage,

  /*! Commit function. */
  .commit = bcmfp_egr_trans_commit,

  /*! Abort function. */
  .abort = bcmfp_egr_trans_abort
};


int
bcmfp_egress_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    /*
     * To register callbacks for EFP group, rule and entry.
     */
    BCMFP_RET_VAL_ASSIGN(bcmlrd_map_get(unit, FP_EGR_GRP_TEMPLATEt, &map));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 FP_EGR_GRP_TEMPLATEt,
                                 &bcmfp_egress_grp_config_imm_callback,
                                 NULL));
    }
    BCMFP_RET_VAL_ASSIGN(bcmlrd_map_get(unit, FP_EGR_RULE_TEMPLATEt, &map));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 FP_EGR_RULE_TEMPLATEt,
                                 &bcmfp_egress_rule_config_imm_callback,
                                 NULL));
    }
    BCMFP_RET_VAL_ASSIGN(bcmlrd_map_get(unit, FP_EGR_POLICY_TEMPLATEt, &map));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 FP_EGR_POLICY_TEMPLATEt,
                                 &bcmfp_egress_policy_config_imm_callback,
                                 NULL));
    }
    BCMFP_RET_VAL_ASSIGN(bcmlrd_map_get(unit, FP_EGR_ENTRYt, &map));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 FP_EGR_ENTRYt,
                                 &bcmfp_egress_entry_config_imm_callback,
                                 NULL));
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}
