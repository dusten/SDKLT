/*! \file bcmmeter_fp_egr.c
 *
 * This file contains EFP Meter custom handler functions.
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
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <sal/sal_time.h>
#include <bcmmeter/bcmmeter_sysm.h>

#include <bcmmeter/bcmmeter_fp_egr.h>

#define BSL_LOG_MODULE          BSL_LS_BCMMETER_FP

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_meter_fp_egr_attach },
static struct {
    bcmmeter_drv_attach_f attach;
} dev_fp_egr_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_meter_fp_egr_detach },
static struct {
    bcmmeter_drv_detach_f detach;
} dev_fp_egr_detach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*******************************************************************************
 * Local definitions
 */

/*! EFP Meter chip attributes. */
bcmmeter_fp_chip_attributes_t
*bcmmeter_fp_egr_chip_attr[BCMDRD_CONFIG_MAX_UNITS];
/*! EFP Meter S/W meter table. */
bcmmeter_fp_sw_state_t
bcmmeter_fp_egr_sw_state[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief EFP meter validation
 *
 * Validate EFP Meter config parameters.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_validate (int unit,
                          bcmlt_opcode_t opcode,
                          const bcmltd_fields_t *in,
                          const bcmltd_generic_arg_t *arg)
{
    uint32_t meter_id = 0, fid = 0;
    int field_count;
    bcmltd_field_t **flist = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    fid = METER_FP_EGR_TEMPLATEt_METER_FP_EGR_TEMPLATE_IDf;
    SHR_IF_ERR_EXIT
        (bcmmeter_entry_field_value_get(unit,
                                        field_count,
                                        flist,
                                        fid,
                                        &meter_id));

    if (meter_id <= 0 || meter_id > BCMMETER_FP_EGR_METERS(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter insert
 *
 * Insert an EFP Meter in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_insert (int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    int field_count = 0;
    uint32_t meter_id = 0;
    uint32_t pipe_id = 0, meter_pool_id = 0, meter_mode = 0;
    bcmltd_field_t **flist = NULL;
    uint8_t oper_mode = 0;
    bcmmeter_fp_entry_t meter_entry;
    bcmmeter_fp_entry_t *ptr = NULL;
    shr_error_t     rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    sal_memset(&meter_entry, 0, sizeof(meter_entry));

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_init_params(unit,
                                    METER_FP_EGR_TEMPLATEt,
                                    &(meter_entry.fields)));

    /*
     * Parse the input fields and copy them to meter_entry->fields
     */
    SHR_IF_ERR_EXIT
        (bcmmeter_entry_parse_params(unit,
                                     BCMMETER_FP_STAGE_ID_EGRESS,
                                     field_count,
                                     flist,
                                     &(meter_entry.fields)));

    /*
     * If meter id is outside the valid range, return ERROR
     */
    meter_id = meter_entry.fields.meter_id;

    /*
     * If meter is already in use, return ERROR
     */
    if (BCMMETER_FP_EGR_INDEX_USED_GET(unit, meter_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    pipe_id = meter_entry.fields.pipe;
    SHR_IF_ERR_EXIT
        (bcmmeter_fp_oper_mode_get(unit, BCMMETER_FP_STAGE_ID_EGRESS, &oper_mode));

    /*
     * Check global pipe mode and then verify pipe number
     */
    if ((oper_mode == BCMMETER_FP_OPER_MODE_GLOBAL && pipe_id != 0) ||
        (oper_mode == BCMMETER_FP_OPER_MODE_PIPE_UNIQUE &&
         pipe_id >= BCMMETER_FP_EGR_NUM_PIPES(unit))) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    meter_mode = meter_entry.fields.meter_mode;
    if (meter_mode > BCMMETER_FP_MODE_MODTRTCM) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (BCMMETER_FP_EGR_POOL_GLOBAL(unit)) {
        /*
         * If meter pool id is outside the valid range, return ERROR
         */
        meter_pool_id = meter_entry.fields.meter_pool_id;
        if (meter_pool_id >= BCMMETER_FP_EGR_NUM_POOLS(unit)) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
    }

    /*
     * Retrieve the pointer to this meter entry from the database.
     */
    ptr = BCMMETER_FP_EGR_ENTRY_LOOKUP(unit, meter_id);
    if (ptr == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    sal_memcpy(ptr, &meter_entry, sizeof(meter_entry));

    if (BCMMETER_FP_EGR_POOL_GLOBAL(unit)) {
        /*
         * Program H/W meter table if Pool ID is known at this time.
         */
        rv = bcmmeter_entry_insert(unit,
                                   trans_id,
                                   BCMMETER_FP_STAGE_ID_EGRESS,
                                   meter_pool_id,
                                   meter_id);
        if (SHR_FAILURE(rv)) {
            sal_memset(ptr, 0, sizeof(bcmmeter_fp_entry_t));
            SHR_RETURN_VAL_EXIT(rv);
        }
    }
    /*
     * Mark the S/W id as used
     */
    BCMMETER_FP_EGR_INDEX_USED_SET(unit, meter_id);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter lookup
 *
 * Lookup EFP Meter.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_lookup (int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t    meter_id = 0, fid = 0;
    int         field_count = 0;
    bcmltd_field_t **flist = NULL, *foutlist = NULL;
    bcmmeter_fp_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    fid = METER_FP_EGR_TEMPLATEt_METER_FP_EGR_TEMPLATE_IDf;
    SHR_IF_ERR_EXIT
        (bcmmeter_entry_field_value_get(unit,
                                        field_count,
                                        flist,
                                        fid,
                                        &meter_id));
    if (!BCMMETER_FP_EGR_INDEX_USED_GET(unit, meter_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_lookup(unit, BCMMETER_FP_STAGE_ID_EGRESS,
                               meter_id, &ptr));

    foutlist = out->field[0];

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_populate_params(unit,
                                        METER_FP_EGR_TEMPLATEt,
                                        ptr,
                                        foutlist));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter delete
 *
 * Delete an EFP Meter.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_delete (int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t meter_id = 0, fid = 0;
    int field_count = 0;
    bcmltd_field_t **flist = NULL;
    bcmmeter_fp_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    fid = METER_FP_EGR_TEMPLATEt_METER_FP_EGR_TEMPLATE_IDf;
    SHR_IF_ERR_EXIT
        (bcmmeter_entry_field_value_get(unit,
                                        field_count,
                                        flist,
                                        fid,
                                        &meter_id));
    if (!BCMMETER_FP_EGR_INDEX_USED_GET(unit, meter_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    ptr = BCMMETER_FP_EGR_ENTRY_LOOKUP(unit, meter_id);
    if (ptr == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /*
     * If meter entry ref count is not zero, meter is being used,
     * return E_BUSY
     */
    if (BCMMETER_ENTRY_GET_REF_COUNT(ptr) != 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }

    if (BCMMETER_FP_EGR_POOL_GLOBAL(unit)) {
        /*
         * Delete meter from H/w table
         */
        SHR_IF_ERR_EXIT
            (bcmmeter_entry_delete(unit,
                                   trans_id,
                                   BCMMETER_FP_STAGE_ID_EGRESS,
                                   meter_id));
    }

    /*
     * Now free this entry and remove from s/w table
     */
    BCMMETER_FP_EGR_INDEX_USED_CLR(unit, meter_id);
    sal_memset(ptr, 0, sizeof(bcmmeter_fp_entry_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter update
 *
 * Update an already configured EFP meter.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_update (int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t meter_id = 0, fid = 0;
    int field_count = 0;
    bcmltd_field_t **flist = NULL;
    bcmmeter_fp_entry_t *meter_entry = NULL;
    bcmmeter_fp_entry_t old_entry;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    sal_memset(&old_entry, 0, sizeof(old_entry));

    fid = METER_FP_EGR_TEMPLATEt_METER_FP_EGR_TEMPLATE_IDf;
    SHR_IF_ERR_EXIT
        (bcmmeter_entry_field_value_get(unit,
                                        field_count,
                                        flist,
                                        fid,
                                        &meter_id));
    if (!BCMMETER_FP_EGR_INDEX_USED_GET(unit, meter_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    meter_entry = BCMMETER_FP_EGR_ENTRY_LOOKUP(unit, meter_id);
    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }


    sal_memcpy(&old_entry, meter_entry, sizeof(old_entry));

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_parse_params(unit,
                                     BCMMETER_FP_STAGE_ID_EGRESS,
                                     field_count,
                                     flist,
                                     &(meter_entry->fields)));

    if (!sal_memcmp(&old_entry.fields,
                    &meter_entry->fields,
                    sizeof(old_entry.fields))) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /*
     * If Pipe value has changed, return NOT SUPPORTED
     */
    if (old_entry.fields.pipe != meter_entry->fields.pipe) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /*
     * If Meter Pool id has changed, return NOT SUPPORTED
     */
    if (BCMMETER_FP_EGR_POOL_GLOBAL(unit)) {
        if (old_entry.fields.meter_pool_id !=
            meter_entry->fields.meter_pool_id) {
            SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        }
    }

    /*
     * If Meter Mode has changed, return NOT SUPPORTED
     */
    if (old_entry.fields.meter_mode != meter_entry->fields.meter_mode) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /*
     * If Meter Color Mode has changed, return NOT SUPPORTED
     */
    if (old_entry.fields.color_mode != meter_entry->fields.color_mode) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /*
     * Update the meter entry in H/w table if Pool ID is known
     * or if the entry is already programmed in HW table
     */
    if (BCMMETER_FP_EGR_POOL_GLOBAL(unit) ||
        (BCMMETER_ENTRY_GET_REF_COUNT(meter_entry) != 0)) {
        SHR_IF_ERR_EXIT
            (bcmmeter_entry_update(unit,
                                   trans_id,
                                   BCMMETER_FP_STAGE_ID_EGRESS,
                                   meter_id));
    }

exit:
    if (SHR_FUNC_ERR()) {
        /*
         * In case of error, restore the previous S/W meter entry
         */
        if (meter_entry) {
            sal_memcpy(meter_entry, &old_entry, sizeof(old_entry));
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
int
bcmmeter_fp_egr_attach (int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software setup */
    SHR_IF_ERR_EXIT
        (dev_fp_egr_attach[dev_type].attach(unit));

  exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 */
int
bcmmeter_fp_egr_detach (int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software tear-down */
    SHR_IF_ERR_EXIT
        (dev_fp_egr_detach[dev_type].detach(unit));

  exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter init
 *
 * Initialize EFP Meter related data structures
 * for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  cold          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_init(int unit,
                     bool cold)
{
    bcmmeter_fp_chip_attributes_t   *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmmeter_fp_egr_drv_get(unit, &drv));

    if (drv == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (bcmmeter_fp_sw_state_init(unit,
                                   BCMMETER_FP_STAGE_ID_EGRESS,
                                   cold));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter cleanup
 *
 * Cleanup EFP Meter related data structures
 * allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_cleanup(int unit)
{
    bcmmeter_fp_chip_attributes_t   *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmmeter_fp_egr_drv_get(unit, &drv));

    if (drv == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (bcmmeter_fp_sw_state_cleanup(unit,
                                      BCMMETER_FP_STAGE_ID_EGRESS));
exit:
    SHR_FUNC_EXIT();
}

int
bcmmeter_fp_egr_dev_config (int unit)
{
    uint64_t            refresh_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit,
                          METER_FP_EGR_CONFIGt,
                          METER_FP_EGR_CONFIGt_REFRESHf,
                          &refresh_en));
    SHR_IF_ERR_EXIT
        (bcmmeter_fp_dev_config(unit,
                                BCMMETER_FP_STAGE_ID_EGRESS,
                                refresh_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter traverse get first.
 *
 * Get the first EFP meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_first(int unit,
                      uint32_t trans_id,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg)
{
    uint32_t    meter_id = 0, min = 0;
    uint32_t    idx = 0;
    bcmltd_field_t      *foutlist = NULL;
    bcmlrd_field_def_t  field_def;
    bcmmeter_fp_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_EXIT
        (bcmlrd_table_field_def_get(
                unit,
                METER_FP_EGR_TEMPLATEt,
                METER_FP_EGR_TEMPLATEt_METER_FP_EGR_TEMPLATE_IDf,
                &field_def));
    min = BCMMETER_FP_FIELD_MIN(field_def.dtag);

    for (idx = min; idx <= BCMMETER_FP_EGR_METERS(unit); idx++) {
        if (BCMMETER_FP_EGR_INDEX_USED_GET(unit, idx)) {
            meter_id = idx;
            break;
        }
    }

    if (idx > BCMMETER_FP_EGR_METERS(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_lookup(unit, BCMMETER_FP_STAGE_ID_EGRESS,
                               meter_id, &ptr));

    foutlist = out->field[0];

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_populate_params(unit,
                                        METER_FP_EGR_TEMPLATEt,
                                        ptr,
                                        foutlist));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter traverse get next.
 *
 * Get the next EFP meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_next(int unit,
                     uint32_t trans_id,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    uint32_t    meter_id = 0, fid = 0;
    uint32_t    idx = 0, field_count = 0;
    bcmltd_field_t **flist = NULL, *foutlist = NULL;
    bcmmeter_fp_entry_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    fid = METER_FP_EGR_TEMPLATEt_METER_FP_EGR_TEMPLATE_IDf;
    SHR_IF_ERR_EXIT
        (bcmmeter_entry_field_value_get(unit,
                                        field_count,
                                        flist,
                                        fid,
                                        &meter_id));

    for (idx = (meter_id + 1); idx <= BCMMETER_FP_EGR_METERS(unit); idx++) {
        if (BCMMETER_FP_EGR_INDEX_USED_GET(unit, idx)) {
            meter_id = idx;
            break;
        }
    }

    if (idx > BCMMETER_FP_EGR_METERS(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_lookup(unit, BCMMETER_FP_STAGE_ID_EGRESS,
                               meter_id, &ptr));

    foutlist = out->field[0];

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_populate_params(unit,
                                        METER_FP_EGR_TEMPLATEt,
                                        ptr,
                                        foutlist));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter transaction commit
 *
 * Discard the backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_commit (int unit,
                        uint32_t trans_id,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t                table_size;
    uint32_t                num_sw_meters = 0;
    bcmmeter_fp_entry_t     *bkp_ptr = NULL;
    bcmmeter_fp_entry_t     *active_ptr = NULL;
    bcmmeter_fp_hw_idx_t    *hw_idx = NULL;
    bcmmeter_fp_hw_idx_t    *bkp_hw_idx = NULL;

    SHR_FUNC_ENTER(unit);

    /* Meter ID 0 is reserved. */
    num_sw_meters = (BCMMETER_FP_EGR_METERS(unit) + 1);

    active_ptr = BCMMETER_FP_EGR_ACTIVE_ENTRY(unit);
    bkp_ptr = BCMMETER_FP_EGR_BKP_ENTRY(unit);
    hw_idx = BCMMETER_FP_EGR_ACTIVE_HW_IDX(unit);
    bkp_hw_idx = BCMMETER_FP_EGR_BKP_HW_IDX(unit);
    if ((bkp_ptr == NULL) || (active_ptr == NULL) ||
        (hw_idx == NULL) || (bkp_hw_idx == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    table_size = (num_sw_meters * sizeof(bcmmeter_fp_entry_t));
    /* Copy active list to backup list. */
    sal_memcpy(bkp_ptr, active_ptr, table_size);

    table_size = (BCMMETER_FP_EGR_NUM_PIPES(unit) *
                  BCMMETER_FP_EGR_METERS_PER_PIPE(unit) *
                  sizeof(bcmmeter_fp_hw_idx_t));
    /* Copy active hw idx list to backup list. */
    sal_memcpy(bkp_hw_idx, hw_idx, table_size);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFP meter transaction abort
 *
 * Rollback the Meter entry from the backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmmeter_fp_egr_abort (int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg)
{
    uint32_t                table_size;
    uint32_t                num_sw_meters = 0;
    bcmmeter_fp_entry_t     *bkp_ptr = NULL;
    bcmmeter_fp_entry_t     *active_ptr = NULL;
    bcmmeter_fp_hw_idx_t    *hw_idx = NULL;
    bcmmeter_fp_hw_idx_t    *bkp_hw_idx = NULL;

    SHR_FUNC_ENTER(unit);

    /* Meter ID 0 is reserved. */
    num_sw_meters = (BCMMETER_FP_EGR_METERS(unit) + 1);

    active_ptr = BCMMETER_FP_EGR_ACTIVE_ENTRY(unit);
    bkp_ptr = BCMMETER_FP_EGR_BKP_ENTRY(unit);
    hw_idx = BCMMETER_FP_EGR_ACTIVE_HW_IDX(unit);
    bkp_hw_idx = BCMMETER_FP_EGR_BKP_HW_IDX(unit);
    if ((bkp_ptr == NULL) || (active_ptr == NULL) ||
        (hw_idx == NULL) || (bkp_hw_idx == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    table_size = (num_sw_meters * sizeof(bcmmeter_fp_entry_t));
    /* Copy backup list to active list. */
    sal_memcpy(active_ptr, bkp_ptr, table_size);

    table_size = (BCMMETER_FP_EGR_NUM_PIPES(unit) *
                  BCMMETER_FP_EGR_METERS_PER_PIPE(unit) *
                  sizeof(bcmmeter_fp_hw_idx_t));
    /* Copy backup hw idx list to active list. */
    sal_memcpy(hw_idx, bkp_hw_idx, table_size);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pointer to EFP meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] attr Chip driver structure.
 *
 * \return SHR_E_NONE
 */
int
bcmmeter_fp_egr_drv_set (int unit,
                         bcmmeter_fp_chip_attributes_t *attr)
{
    bcmmeter_fp_egr_chip_attr[unit] = attr;

    return SHR_E_NONE;
}

/*!
 * \brief Get pointer to EFP meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] attr Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
int
bcmmeter_fp_egr_drv_get (int unit,
                         bcmmeter_fp_chip_attributes_t **attr)
{
    *attr = bcmmeter_fp_egr_chip_attr[unit];

    return SHR_E_NONE;
}
