/*! \file bcmtm_imm_ct_port.c
 *
 * In-memory call back function for cut-through port settings for front panel
 * ports.
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
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/ct/bcmtm_ct_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
/*******************************************************************************
 * Local definitions
 */
bcmtm_ct_hdlr_t bcmtm_ct_hdlr[BCMDRD_DEV_T_COUNT];

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Cut-through physical table configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] ltid Logical table ID.
 * \param [in] ct_enable Enable/disable cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_ct_port_pt_set(int unit,
        bcmltd_sid_t ltid,
        bcmtm_lport_t lport,
        uint32_t ct_enable)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    /* update physical table. */
    if (bcmtm_ct_hdlr[dev_type].port_set) {
        SHR_IF_ERR_EXIT
            (bcmtm_ct_hdlr[dev_type].port_set(unit, ltid, lport, ct_enable));
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
bcmtm_ct_port_reset(int unit,
        bcmltd_sid_t ltid,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data)
{
    bcmtm_lport_t lport;
    uint32_t ct_enable = 0;
    uint64_t fval = 0;
    int mport;

    SHR_FUNC_ENTER(unit);

   /* TM_CUT_THROUGH_PORTt_PORT_IDf */
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, TM_CUT_THROUGH_PORTt_PORT_IDf,
                                         key, &fval));
    lport = (bcmtm_lport_t)fval;

    if (SHR_FAILURE(bcmtm_port_mmu_local_port_get(unit, lport, &mport))) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Physical table update */
    SHR_IF_ERR_EXIT
        (bcmtm_ct_port_pt_set(unit, ltid, lport, ct_enable));
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
bcmtm_ct_port_set(int unit,
        bcmltd_sid_t ltid,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        bcmltd_fields_t *output_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint32_t ct_enable = 0;
    int mport;
    uint64_t fval = 0;
    uint8_t opcode = 1;

    SHR_FUNC_ENTER(unit);

    /* TM_CUT_THROUGH_PORTt_PORT_IDf */
    fid = TM_CUT_THROUGH_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* TM local port information.
     * Returns error if local port to physical port map is not available.
     * Updates opeartional state.
     */
    if (SHR_FAILURE(bcmtm_port_mmu_local_port_get(unit, lport, &mport))) {
        goto op_stage;
    }

    if (data) {
        fid = TM_CUT_THROUGH_PORTt_CUT_THROUGHf;
        SHR_IF_ERR_EXIT
            (bcmtm_fval_get_from_field_array(unit, fid, data, &fval));
        ct_enable = (uint32_t)fval;
    }
    /* PT update */
    SHR_IF_ERR_EXIT
        (bcmtm_ct_port_pt_set(unit, ltid, lport, ct_enable));

    /* opcode success */
    opcode = 0;
op_stage:
    fid = TM_CUT_THROUGH_PORTt_OPERATIONAL_STATEf;
    bcmtm_field_populate(unit, output_fields, fid, 0, opcode);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_CUT_THROUGH_PORT logical table callback function for staging.
 *
 * Handle TM_CUT_THROUGH_PORT IMM table change events.
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
bcmtm_ct_port_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    switch(event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            SHR_IF_ERR_EXIT
                (bcmtm_ct_port_set(unit, ltid, key, data, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            /* disable cut-through. */
            SHR_IF_ERR_EXIT
                (bcmtm_ct_port_reset(unit, ltid, key, data));
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
bcmtm_ct_port_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_CUT_THROUGH_PORTt logical table. */
    bcmimm_lt_cb_t bcmtm_ct_port_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_ct_port_stage,
    };
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlrd_map_get(unit, TM_CUT_THROUGH_PORTt, &map), SHR_E_UNAVAIL);

    if (SHR_FUNC_VAL_IS(SHR_E_UNAVAIL) || (!map)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /*! Registers callback functions for cut-through settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_CUT_THROUGH_PORTt,
                             &bcmtm_ct_port_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
