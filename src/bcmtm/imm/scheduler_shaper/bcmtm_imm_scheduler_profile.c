/*! \file bcmtm_imm_scheduler_profile.c
 *
 * In-memory call back function for schedulers for scheduling profiles of
 * non cpu ports.
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

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
extern bcmtm_scheduler_hdlr_t bcmtm_scheduler_hdlr[BCMDRD_DEV_T_COUNT];

/*******************************************************************************
 * Private functions
 */
static int
bcmtm_scheduler_profile_op_status(int unit,
                                  uint8_t profile_id,
                                  bcmtm_scheduler_profile_t *profile)
{
    int idx, rv;
    bcmltd_fields_t out, in;
    size_t num_fid, count;
    bcmltd_fid_t fid;
    bcmltd_sid_t ltid = TM_SCHEDULER_PROFILEt;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
    bcmtm_field_populate(unit, &in, fid, 0, profile_id);

    for (idx = 0; idx < MAX_TM_SCHEDULER_NODE_QUEUE; idx++) {
        in.count = 1;
        fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf;
        bcmtm_field_populate(unit, &in, fid, 0, idx);

        out.count = num_fid;
        for (count = 0; count < num_fid; count++) {
            out.field[count]->data = 0;
            out.field[count]->id = 0;
            out.field[count]->idx = 0;
        }
        rv = bcmimm_entry_lookup(unit, ltid, &in, &out);
        if (rv == SHR_E_NONE) {
            if (profile) {
                fval = profile[idx].opcode;
            } else {
                fval = 2;
            }
            out.count = 0;
            fid = TM_SCHEDULER_PROFILEt_OPERATIONAL_STATEf;
            bcmtm_field_populate(unit, &out, fid, 0, fval);
            fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
            bcmtm_field_populate(unit, &out, fid, 0, profile_id);
            fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf;
            bcmtm_field_populate(unit, &out, fid, 0, idx);
            SHR_IF_ERR_EXIT(bcmimm_entry_update(unit, 0, ltid, &out));
        }
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Populates scheduler configuration using user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] sched_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmtm_scheduler_profile_fill(int unit,
        const bcmltd_field_t *data,
        bcmtm_scheduler_profile_t *scheduler_profile)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;

    while(cur_data) {
        switch(cur_data->id) {
            case TM_SCHEDULER_PROFILEt_NUM_UC_Qf:
                scheduler_profile->num_ucq = cur_data->data;
                break;
            case TM_SCHEDULER_PROFILEt_NUM_MC_Qf:
                scheduler_profile->num_mcq = cur_data->data;
                break;
            case TM_SCHEDULER_PROFILEt_FLOW_CTRL_UCf:
                scheduler_profile->fc_uc = cur_data->data;
                break;
            case TM_SCHEDULER_PROFILEt_OPERATIONAL_STATEf:
                scheduler_profile->opcode = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  Scheduler profile physical table configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] sched_profile_id Scheduler profile ID.
 * \param [in] ltid Logical table ID.
 * \param [in] sched_cfg Shaper configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_profile_pt_set(int unit,
                               bcmltd_sid_t ltid,
                               uint8_t profile_id,
                               bcmtm_scheduler_profile_t *profile)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* status physical table. */
    if (bcmtm_scheduler_hdlr[dev_type].profile_set) {
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_hdlr[dev_type].profile_set(unit, ltid, profile_id,
                                                 profile));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_profile_reset(int unit,
                              bcmltd_sid_t ltid,
                              uint8_t profile_id,
                              uint8_t node_id)
{
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE_QUEUE];

    SHR_FUNC_ENTER(unit);
    sal_memset(profile, 0,
            (sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE_QUEUE));
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_get(unit, profile_id, profile));

    /* Physical table status default profile. */
    if (profile[node_id].opcode == 0) {
        
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_profile_pt_set(unit, ltid, profile_id, profile));
        /* status the operational state for all the node id for profile. */
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_profile_op_status(unit, profile_id, NULL));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] output_fields This is linked list of extra added data fields
 * (mostly read only fields.)
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_profile_set(int unit,
                            bcmltd_sid_t ltid,
                            uint8_t profile_id,
                            uint8_t node_id,
                            const bcmltd_field_t *data,
                            bcmltd_fields_t *output_fields)
{
    bcmltd_fid_t fid;
    uint8_t opcode = 6, status = 0;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE_QUEUE];

    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0,
            (sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE_QUEUE));
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_get(unit, profile_id, profile));

    /* status the newly added node in profile. */
    bcmtm_scheduler_profile_fill(unit, data, &profile[node_id]);

    /* profile validation. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_validate(unit, (int)node_id, profile, &status));

    opcode = profile[node_id].opcode;
    /* PT status only if opcode returned is success. */
    if (status) {
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_profile_pt_set(unit, ltid, profile_id, profile));
        /* opcode success */
        opcode = 0;
    } else {
        goto op_stage;
    }
op_stage:
    fid = TM_SCHEDULER_PROFILEt_OPERATIONAL_STATEf;
    bcmtm_field_populate(unit, output_fields, fid, 0, opcode);
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_op_status(unit, profile_id, profile));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_SCHEDULER_PROFILE logical table callback function for staging.
 *
 * Handle TM_SCHEDULER_PROFILE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_scheduler_profile_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    uint8_t profile_id, node_id;
    uint64_t fval;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf */
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, key, &fval));
    profile_id = (uint8_t)fval;

    /* TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, key, &fval));
    node_id = (uint8_t)fval;

    switch(event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_set(unit, ltid, profile_id,
                                       node_id, data, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_reset(unit, ltid, profile_id, node_id));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_scheduler_profile_validate(int unit,
                                 int node_id,
                                 bcmtm_scheduler_profile_t *profile,
                                 uint8_t *status)
{
    bcmdrd_dev_type_t dev_type;
    int idx;

    SHR_FUNC_ENTER(unit);
    dev_type = bcmdrd_dev_type(unit);
    if(dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    if((node_id != -1) &&
       (bcmtm_scheduler_hdlr[dev_type].node_profile_validate)) {
        SHR_IF_ERR_EXIT(
            bcmtm_scheduler_hdlr[dev_type].node_profile_validate(unit, node_id,
                                                            &profile[node_id]));
    }
    if (bcmtm_scheduler_hdlr[dev_type].profile_validate) {
        SHR_IF_ERR_EXIT(
            bcmtm_scheduler_hdlr[dev_type].profile_validate(unit, profile));
    }

    *status = 1;
    for (idx = 0; idx < MAX_TM_SCHEDULER_NODE_QUEUE; idx++) {
        if (profile[idx].opcode != 0) {
            *status = 0;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_get(int unit,
                            uint8_t profile_id,
                            bcmtm_scheduler_profile_t *profile)
{
    int idx, rv;
    bcmltd_fields_t out, in;
    size_t num_fid, count;
    bcmltd_fid_t fid;
    bcmltd_sid_t ltid = TM_SCHEDULER_PROFILEt;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
    bcmtm_field_populate(unit, &in, fid, 0, profile_id);

    for (idx = 0; idx < MAX_TM_SCHEDULER_NODE_QUEUE; idx++) {
        in.count = 1; /* as profile id is already populated. */
        fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf;
        bcmtm_field_populate(unit, &in, fid, 0, idx);

        out.count = num_fid;
        for (count = 0; count < num_fid; count++) {
            out.field[count]->data = 0;
            out.field[count]->id = 0;
            out.field[count]->idx = 0;
        }
        rv = bcmimm_entry_lookup(unit, ltid, &in, &out);
        if (rv == SHR_E_NONE) {
            bcmtm_scheduler_profile_fill(unit, *(out.field), &profile[idx]);
        }
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    /*! Inmemory callback functions for TM_SCHEDULER_PROFILE table. */
    static bcmimm_lt_cb_t bcmtm_scheduler_profile_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_scheduler_profile_stage,
    };

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlrd_map_get(unit, TM_SCHEDULER_PROFILEt, &map), SHR_E_UNAVAIL);

    if (SHR_FUNC_VAL_IS(SHR_E_UNAVAIL)|| (!map)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /*! Registers callback functions for Scheduler profile. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SCHEDULER_PROFILEt,
                             &bcmtm_scheduler_profile_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
