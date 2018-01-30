/*! \file cth_alpm.c
 *
 * Custom Handler for ALPM tables
 *
 * This file contains the main function for CTH ALPM.
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm.h>
#include "cth_alpm_device.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHALPM
#define CTH_ALPM_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)
#define SHR_RETURN      SHR_RETURN_VAL_EXIT
#define LEVEL2 1
#define LEVEL3 2
#define CTH_ALPM_FIELD_COUNT_MAX  20 /* Big enough */

/*******************************************************************************
 * Private variables
 */

static shr_famm_hdl_t cth_alpm_famm_hdl[BCMDRD_CONFIG_MAX_UNITS];

static bool cth_alpm_supported[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Free fields buffer used for imm operaion.
 *
 * \param [in] u Unit number.
 * \param [in] field Fields buffer
 *
 * \retval nothing
 */
static void
fields_free(int u, bcmltd_field_t **field)
{
    if (field) {
        if (field[0]) {
            sal_free(field[0]);
        }
        sal_free(field);
    }
}

/*!
 * \brief Allocate fields buffer used for imm operaion.
 *
 * \param [in] u Unit number.
 * \param [in] count Count of fields
 *
 * \retval Fields buffer
 */
static bcmltd_field_t **
fields_alloc(int u, size_t count)
{
    uint32_t idx = 0;
    bcmltd_field_t **field_ptrs = NULL;

    field_ptrs = sal_alloc(count * sizeof(bcmltd_field_t *), "field ptrs");
    if (field_ptrs == NULL) {
        return NULL;
    }

    field_ptrs[0] = NULL;
    field_ptrs[0] = sal_alloc(count * sizeof(bcmltd_field_t), "field ptr");
    if (field_ptrs[0] == NULL) {
        sal_free(field_ptrs);
        return NULL;
    }
    sal_memset(field_ptrs[0], 0, count * sizeof(bcmltd_field_t));

    for (idx = 1; idx < count; idx++) {
        field_ptrs[idx] = field_ptrs[0] + idx;
    }
    return field_ptrs;
}

/*!
 * \brief Parse uft banks to alpm control
 *
 * \param [in] u Unit number.
 * \param [in] ln ALPM level
 * \param [in] fields Fields buffer
 * \param [out] control ALPM control
 *
 * \retval SHR_E_NONE.
 */
static int
uft_banks_fields_parse(int u,
                       int ln,
                       const bcmltd_fields_t *fields,
                       bcmptm_cth_alpm_control_t *control)
{
    uint8_t idx;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(u);

    /* Parse data field. */
    for (idx = 0; idx < fields->count; idx++) {
        fid = fields->field[idx]->id;
        fval = fields->field[idx]->data;
        switch (fid) {
        case DEVICE_EM_GROUPt_NUM_BANKSf:
        case DEVICE_EM_GROUPt_NUM_FIXED_BANKSf:
            if (ln == LEVEL2) {
                control->num_l2_banks += fval;
            } else {
                control->num_l3_banks += fval;
            }
            break;
        default:
            break;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Use IMM to parse UFT banks for now, but prefer UFT get API.
 *
 * \param [in] u Unit number.
 * \param [out] control ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
uft_banks_get(int u, bcmptm_cth_alpm_control_t *control)
{
    bcmltd_fields_t input, output;
    const cth_alpm_device_info_t *dev_info;
    uint32_t val;
    int rv;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_cth_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    /* Level2 */
    input.count = 1;
    input.field = fields_alloc(u, input.count);
    input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;

    rv = bcmlrd_field_symbol_to_value(u,
                                      DEVICE_EM_GROUPt,
                                      DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf,
                                      "ALPM_LEVEL2_GROUP",
                                      &val);
    if (rv == SHR_E_NOT_FOUND) {
        rv = bcmlrd_field_symbol_to_value(u,
                                          DEVICE_EM_GROUPt,
                                          DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf,
                                          "ALPM_LEVEL2_GROUP",
                                          &val);
    }
    if (SHR_SUCCESS(rv)) {
        input.field[0]->data = val;
        output.count = 10;
        output.field = fields_alloc(u, output.count);
        SHR_IF_ERR_EXIT(
            bcmimm_entry_lookup(u, DEVICE_EM_GROUPt, &input, &output));
        SHR_IF_ERR_EXIT(
            uft_banks_fields_parse(u, LEVEL2, &output, control));
        fields_free(u, input.field);
        fields_free(u, output.field);
    }

    /* Level3 */
    rv = bcmlrd_field_symbol_to_value(u,
                                      DEVICE_EM_GROUPt,
                                      DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf,
                                      "ALPM_LEVEL3_GROUP",
                                      &val);
    if (SHR_SUCCESS(rv)) {
        input.count = 1;
        input.field = fields_alloc(u, input.count);
        input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
        input.field[0]->data = val;
        output.count = 10;
        output.field = fields_alloc(u, output.count);
        SHR_IF_ERR_EXIT(
            bcmimm_entry_lookup(u, DEVICE_EM_GROUPt, &input, &output));
        SHR_IF_ERR_EXIT(
            uft_banks_fields_parse(u, LEVEL3, &output, control));
        fields_free(u, input.field);
        fields_free(u, output.field);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write with mreq
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] trans_id Transaction identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_mreq_write(int u, bcmltd_sid_t lt_id, uint32_t trans_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(u);
    pt_info.index = index;
    pt_info.tbl_inst = -1;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(u, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write with ireq
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_ireq_write(int u, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(u);
    pt_info.index = index;
    pt_info.tbl_inst = -1;
    SHR_IF_ERR_EXIT(
        bcmptm_ireq_write(u, 0, pt_id, &pt_info, NULL,
                          entry_data, lt_id, &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse alpm control structure member from field and field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid  Field ID.
 * \param [in] fval Field value
 * \param [out] ctrl ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_field_parse(int u, uint32_t fid, uint64_t fval,
                             bcmptm_cth_alpm_control_t *ctrl)
{
    switch (fid) {
    case L3_ALPM_CONTROLt_ALPM_MODEf:
        ctrl->alpm_mode = fval;
        break;
    case L3_ALPM_CONTROLt_NUM_DB_0_LEVELSf:
        ctrl->db0_levels = fval;
        break;
    case L3_ALPM_CONTROLt_NUM_DB_1_LEVELSf:
        ctrl->db1_levels = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_0f:
        ctrl->l1_key_input[0] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_1f:
        ctrl->l1_key_input[1] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_2f:
        ctrl->l1_key_input[2] = fval;
        break;
    case L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_3f:
        ctrl->l1_key_input[3] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_0f:
        ctrl->l1_db[0] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_1f:
        ctrl->l1_db[1] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_2f:
        ctrl->l1_db[2] = fval;
        break;
    case L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_3f:
        ctrl->l1_db[3] = fval;
        break;
    case L3_ALPM_CONTROLt_URPFf:
        ctrl->urpf = fval;
        break;
    default:
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*!
 * \brief L3_ALPM_CONTROLt input fields parsing.
 *
 * Parse IMM L3_ALPM_CONTROLt input fields.
 *
 * \param [in] u Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] ctrl ALPM control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to parse.
 */
static int
cth_alpm_control_list_fields_parse(int u,
                                   const bcmltd_field_t *data,
                                   bcmptm_cth_alpm_control_t *ctrl)
{
    const bcmltd_field_t *gen_field = NULL;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(u);
    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        SHR_IF_ERR_EXIT(
            cth_alpm_control_field_parse(u, fid, fval, ctrl));
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROLt input fields parsing.
 *
 * Parse IMM L3_ALPM_CONTROLt input fields.
 *
 * \param [in] u Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] ctrl ALPM control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to parse.
 */
static int
cth_alpm_control_array_fields_parse(int u,
                                    const bcmltd_fields_t *fields,
                                    bcmptm_cth_alpm_control_t *ctrl)
{
    uint32_t fid, idx;
    uint64_t fval;

    SHR_FUNC_ENTER(u);
    /* Parse data field. */
    for (idx = 0; idx < fields->count; idx++) {
        fid = fields->field[idx]->id;
        fval = fields->field[idx]->data;
        SHR_IF_ERR_EXIT(
            cth_alpm_control_field_parse(u, fid, fval, ctrl));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup from imm table and parse to alpm control structure.
 *
 * \param [in] u Unit number.
 * \param [out] ctrl Parsed ALPM control parameters
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
static int
cth_alpm_control_lookup_and_parse(int u, bcmptm_cth_alpm_control_t *ctrl)
{
    bcmltd_fields_t input, output;

    SHR_FUNC_ENTER(u);
    input.count = 0; /* No key */
    input.field = NULL;
    output.count = CTH_ALPM_FIELD_COUNT_MAX;
    output.field = fields_alloc(u, output.count);
    SHR_IF_ERR_EXIT(
        bcmimm_entry_lookup(u, L3_ALPM_CONTROLt, &input, &output));
    SHR_IF_ERR_EXIT(
        cth_alpm_control_array_fields_parse(u, &output, ctrl));
exit:
    fields_free(u, output.field);
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROLt parsed fields Validation.
 *
 * Validate L3_ALPM_CONTROLt parsed fields.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl Parsed ALPM control parameters
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 */
static int
cth_alpm_control_validate(int u, bcmptm_cth_alpm_control_t *ctrl)
{
    uint8_t tcam_blocks;
    uint8_t i;
    const cth_alpm_device_info_t *device_info;

    SHR_FUNC_ENTER(u);
    device_info = bcmptm_cth_alpm_device_info_get(u);
    if (ctrl->urpf) {
        if (!device_info->feature_urpf) {
            SHR_RETURN(SHR_E_PARAM);
        }
        tcam_blocks = device_info->tcam_blocks >> 2; /* Half blocks */
        /* Strict order */
        for (i = 0; i < tcam_blocks - 1; i++) {
            if (ctrl->l1_key_input[i] > ctrl->l1_key_input[i + 1]) {
                SHR_RETURN(SHR_E_PARAM);
            }
        }
        /* Must be symmetrical */
        for (i = 0; i < tcam_blocks; i++) {
            if (ctrl->l1_key_input[i] != ctrl->l1_key_input[i + tcam_blocks]) {
                SHR_RETURN(SHR_E_PARAM);
            }
        }
    } else {
        /* Strict order */
        tcam_blocks = device_info->tcam_blocks >> 1;
        for (i = 0; i < tcam_blocks - 1; i++) {
            if (ctrl->l1_key_input[i] > ctrl->l1_key_input[i + 1]) {
                SHR_RETURN(SHR_E_PARAM);
            }
        }
    }

    /* Verify that levels match with banks */
    switch (ctrl->db0_levels) {
    case 3:     /* DB0 3 Levels */
        if (ctrl->num_l3_banks == 0) {
            SHR_RETURN(SHR_E_PARAM);
        }
    /* fall through */
    case 2:     /* DB0 2 levels */
        if (ctrl->num_l2_banks == 0) {
            SHR_RETURN(SHR_E_PARAM);
        }
        break;
    default:
        break;
    }
    switch (ctrl->db1_levels) {
    case 3:     /* DB1 3 Levels */
        if (ctrl->num_l3_banks == 0) {
            SHR_RETURN(SHR_E_PARAM);
        }
    /* fall through */
    case 2:     /* DB1 2 Levels */
        if (ctrl->num_l2_banks == 0) {
            SHR_RETURN(SHR_E_PARAM);
        }
        break;
    default:
        break;
    }

    /* Do not support DB1 Levels bigger than DB0 Levels */
    if (ctrl->alpm_mode == ALPM_MODE_PARALLEL &&
        ctrl->db1_levels > ctrl->db0_levels) {
        SHR_RETURN(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_device_control_validate(u, ctrl));

    /*
     * Do not verify ALPM mode vs. DB Levels.
     * If Combined mode, simply ignore DB1 Levels.
     * Do not verify ALPM DB.
     */
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write pt of l3 alpm control into hardware.
 *
 * Parse IMM L3_ALPM_CONTROLt input fields.
 *
 * \param [in] u Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] trans_id Transaction identifier.
 * \param [in] ctrl ALPM control
 * \param [in] is_init True if init.
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_write(int u, bcmltd_sid_t lt_id,
                       uint32_t trans_id,
                       bcmptm_cth_alpm_control_t *ctrl,
                       bool is_init)
{
    cth_alpm_control_pt_t pts;

    SHR_FUNC_ENTER(u);
    /* Get entry count */
    sal_memset(&pts, 0, sizeof(pts));
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_device_control_encode(u, ctrl, &pts));

    /* Allocate physical entries and encode, then write */
    if (pts.count) {
        int size, i;

        size = pts.count * sizeof(bcmdrd_sid_t);
        CTH_ALPM_ALLOC(pts.sid, size, "Regs SIDs");
        sal_memset(pts.sid, 0, size);

        size = pts.count * sizeof(alpm_control_entry_t);
        CTH_ALPM_ALLOC(pts.entry, size, "Regs Entries");
        sal_memset(pts.entry, 0, size);

        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_device_control_encode(u, ctrl, &pts));

        /*
         * Notify before making any real changes, such that
         * if such changes are not allowed, we will abort before
         * causing any damage.
         */

        bcmevm_publish_event_notify(u, "bcmptmEvAlpmCtrlUpdate",
                                    (uint64_t) (unsigned long) ctrl);
        SHR_IF_ERR_MSG_EXIT(
            ctrl->rv, (BSL_META_U(u, "L3_ALPM_CONTROL operation aborting.\n")));

        for (i = 0; i < pts.count; i++) {
            if (is_init) {
                SHR_IF_ERR_EXIT(
                    cth_alpm_ireq_write(u, lt_id,
                                        pts.sid[i], 0,
                                        (uint32_t *)&pts.entry[i]));
            } else {
                SHR_IF_ERR_EXIT(
                    cth_alpm_mreq_write(u, lt_id, trans_id,
                                        pts.sid[i], 0,
                                        (uint32_t *)&pts.entry[i]));
            }
        }

        SHR_FREE(pts.sid);
        SHR_FREE(pts.entry);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief L3_ALPM_CONTROL logical table validate callback function.
 *
 * Validate the field values of L3_ALPM_CONTROLt logical table entry.
 *
 * \param [in] u Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid field value.
 */
static int
cth_alpm_control_imm_validate(int u,
                              bcmltd_sid_t sid,
                              bcmimm_entry_event_t event,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              void *context)
{
    bcmptm_cth_alpm_control_t alpm_ctrl;


/*
 * L3_ALPM_CONTROL is a pre-populate LT.
 * It is prepopulated by an internal insert.
 * Users can issue following op codes:
 * INSERT - SHR_E_EXISTS (as entry is always in place)
 * UPDATE - SHR_E_NONE (unless verification error, consider this as SET)
 * LOOKUP - SHR_E_NONE (consider this as GET)
 * DELETE - SHR_E_NONE (not true DELETE, consider this as RESET)
 */

    SHR_FUNC_ENTER(u);
    switch (event) {
    case BCMIMM_ENTRY_INSERT: /* BCMIMM should return SHR_E_EXITS */
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_get(u, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_list_fields_parse(u, data, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_validate(u, &alpm_ctrl));
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_set(u, &alpm_ctrl));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        /*
         * SHR_IF_ERR_EXIT(
         *     bcmptm_cth_alpm_control_reset(u));
         */
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROL IMM table change callback function for staging.
 *
 * Handle L3_ALPM_CONTROL IMM table change events.
 *
 * \param [in] u Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] key Linked list of the key fields identifying the entry.
 * \param [in] data Linked list of the data fields in the modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
cth_alpm_control_imm_stage(int u,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           bcmimm_entry_event_t event,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context,
                           bcmltd_fields_t *output_fields)
{
    bcmptm_cth_alpm_control_t ctrl;

    SHR_FUNC_ENTER(u);
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_get(u, &ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_write(u, sid, trans_id, &ctrl, false));
        break;
    default:
        break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to L3_ALPM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t cth_alpm_control_imm_cb = {

    /*! Validate function. */
    .validate = cth_alpm_control_imm_validate,

    /*! Staging function. */
    .stage = cth_alpm_control_imm_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};



static int
cth_alpm_imm_register(int u)
{
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(u);
    if (SHR_SUCCESS(bcmlrd_map_get(u, L3_ALPM_CONTROLt, &map)) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(u,
                                L3_ALPM_CONTROLt,
                                &cth_alpm_control_imm_cb,
                                NULL));
    }
exit:
    SHR_FUNC_EXIT();
}


static void
handle_uft_event(int u, const char *event, uint64_t ev_data)
{
    bcmltd_sid_t lt_id = 0;
    uint32_t trans_id = 0;
    bcmptm_cth_alpm_control_t alpm_ctrl;

    (void)bcmptm_cth_alpm_control_get(u, &alpm_ctrl);

    
    alpm_ctrl.num_l2_banks = ev_data & 0xFF;
    alpm_ctrl.num_l3_banks = (ev_data >> 8) & 0xFF;
    (void)cth_alpm_control_validate(u, &alpm_ctrl);
    (void)bcmptm_cth_alpm_control_set(u, &alpm_ctrl);
    
    (void)cth_alpm_control_write(u, lt_id, trans_id, &alpm_ctrl, true);
}

static int
cth_alpm_control_default_init(int u)
{
    const cth_alpm_device_info_t *dev_info;
    bcmptm_cth_alpm_control_t default_control;

    SHR_FUNC_ENTER(u);
    dev_info = bcmptm_cth_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);
    default_control = dev_info->default_control;
    SHR_IF_ERR_EXIT(
        uft_banks_get(u, &default_control));
    SHR_IF_ERR_EXIT(
        bcmptm_cth_alpm_control_default_set(u, &default_control));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pre-insert the L3_ALPM_CONTROL LT with default values.
 *
 * \param [in] u Unit number.
 * \param [in] ctrl Default ALPM control
 *
 * \retval SHR_E_XXX.
 */
static int
cth_alpm_control_pre_insert(int u, bcmptm_cth_alpm_control_t *ctrl)
{
    bcmltd_fields_t fields = {0};
    uint32_t f_cnt = 0;

    SHR_FUNC_ENTER(u);
    if (!cth_alpm_famm_hdl[u]) {
        SHR_IF_ERR_EXIT(
            shr_famm_hdl_init(CTH_ALPM_FIELD_COUNT_MAX,
                              &cth_alpm_famm_hdl[u]));
    }
    fields.field = shr_famm_alloc(cth_alpm_famm_hdl[u], CTH_ALPM_FIELD_COUNT_MAX);
    if (fields.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    f_cnt = 0;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_ALPM_MODEf;
    fields.field[f_cnt++]->data = ctrl->alpm_mode;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_NUM_DB_0_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db0_levels;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_NUM_DB_1_LEVELSf;
    fields.field[f_cnt++]->data = ctrl->db1_levels;
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_0f;
    fields.field[f_cnt++]->data = ctrl->l1_key_input[0];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_1f;
    fields.field[f_cnt++]->data = ctrl->l1_key_input[1];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_2f;
    fields.field[f_cnt++]->data = ctrl->l1_key_input[2];
    fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_3f;
    fields.field[f_cnt++]->data = ctrl->l1_key_input[3];
    if (ctrl->l1_db_valid) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_0f;
        fields.field[f_cnt++]->data = ctrl->l1_db[0];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_1f;
        fields.field[f_cnt++]->data = ctrl->l1_db[1];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_2f;
        fields.field[f_cnt++]->data = ctrl->l1_db[2];
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_3f;
        fields.field[f_cnt++]->data = ctrl->l1_db[3];
    }
    if (ctrl->urpf_valid) {
        fields.field[f_cnt  ]->id   = L3_ALPM_CONTROLt_URPFf;
        fields.field[f_cnt++]->data = ctrl->urpf;
    }
    fields.count = f_cnt;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_insert(u, L3_ALPM_CONTROLt, &fields));
exit:
    if (fields.field) {
        shr_famm_free(cth_alpm_famm_hdl[u], fields.field, CTH_ALPM_FIELD_COUNT_MAX);
    }
    if (cth_alpm_famm_hdl[u]) {
        shr_famm_hdl_delete(cth_alpm_famm_hdl[u]);
        cth_alpm_famm_hdl[u] = 0;
    }
    SHR_FUNC_EXIT();
}

static bool
cth_alpm_device_is_supported(int u)
{
    return cth_alpm_supported[u];
}
/*******************************************************************************
 * Public Functions
 */

int
bcmptm_cth_alpm_init(int u, bool warm)
{
    int rv;
    bcmptm_cth_alpm_control_t ctrl;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(u);

    rv = bcmlrd_map_get(u, L3_ALPM_CONTROLt, &map);
    if (!(SHR_SUCCESS(rv) && map)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    rv = bcmptm_cth_alpm_device_init(u);
    if (SHR_SUCCESS(rv)) {
        const cth_alpm_device_info_t *dev_info;
        dev_info = bcmptm_cth_alpm_device_info_get(u);
        SHR_NULL_CHECK(dev_info, SHR_E_INIT);
        cth_alpm_supported[u] = true;
    } else {
        cth_alpm_supported[u] = false;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(
        cth_alpm_control_default_init(u));
    if (warm) {
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_default_get(u, &ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_lookup_and_parse(u, &ctrl));
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_set(u, &ctrl));
    } else {
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_reset(u));
        SHR_IF_ERR_EXIT(
            bcmptm_cth_alpm_control_get(u, &ctrl));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_write(u, L3_ALPM_CONTROLt, 0, &ctrl, true));
        SHR_IF_ERR_EXIT(
            cth_alpm_control_pre_insert(u, &ctrl));
    }
    SHR_IF_ERR_EXIT(
        cth_alpm_imm_register(u));
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(u,
                                        /* name to align with UFT module */
                                        "bcmptmEvUftBanksUpdate",
                                        handle_uft_event));
exit:
    if (cth_alpm_supported[u] == false) {
        bcmptm_cth_alpm_device_cleanup(u);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_stop(int u)
{
    SHR_FUNC_ENTER(u);
    if (!cth_alpm_device_is_supported(u)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        bcmevm_unregister_published_event(u,
                                          /* name to align with UFT module */
                                          "bcmptmEvUftBanksUpdate",
                                          handle_uft_event));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_alpm_cleanup(int u, bool warm)
{
    SHR_FUNC_ENTER(u);
    if (!cth_alpm_device_is_supported(u)) {
        SHR_EXIT();
    }
    bcmptm_cth_alpm_device_cleanup(u);
exit:
    SHR_FUNC_EXIT();
}

