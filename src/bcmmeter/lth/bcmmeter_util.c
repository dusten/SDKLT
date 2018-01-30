/*! \file bcmmeter_util.c
 *
 * This file defines HW interface handler functions for
 * FP Meters.
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
#include <bcmmeter/bcmmeter_fp_ing.h>
#include <bcmmeter/bcmmeter_fp_egr.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmmeter/bcmmeter_internal.h>

#define BSL_LOG_MODULE          BSL_LS_BCMMETER_COMMON

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Insert meter into hardware table.
 *
 * Find a free index and write this meter into hardware table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] pool Meter pool id.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 * \retval SHR_E_FULL Hardware meter table full.
 * \retval SHR_E_PARAM Invalid meter param.
 */
int
bcmmeter_entry_insert (int unit,
                       uint32_t trans_id,
                       int stage,
                       int pool,
                       uint32_t meter_id)
{
    uint32_t hw_index = 0;
    uint32_t meter_mode = 0;
    int      pipe_id = 0;
    bool meter_pairing = false;
    bcmmeter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_entry = BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id);
    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Meter has already been programmed in hardware, Return
     */
    if (BCMMETER_ENTRY_GET_REF_COUNT(meter_entry)) {
        if ((BCMMETER_FP_POOL_GLOBAL(unit, stage) == false) &&
            (pool != meter_entry->hw_fields.pool)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        SHR_EXIT();
    }

    /*
     * Find a free index
     */
    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_idx_alloc != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_idx_alloc(unit,
                                                           stage,
                                                           pool,
                                                           meter_entry,
                                                           &hw_index));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    meter_entry->hw_fields.meter_hw_index = hw_index;
    meter_entry->hw_fields.meter_pair_index = (hw_index / 2);
    meter_entry->hw_fields.pool = pool;

    /*
     * Convert to HW parameters
     * Find granularity, calculate refresh count and bucket size
     */
    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_param_compute != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_param_compute(unit,
                                                               stage,
                                                               meter_entry));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /*
     * Actual write to h/w
     */
    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_write != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_write(unit,
                                                       trans_id,
                                                       stage,
                                                       meter_entry));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    meter_mode = meter_entry->fields.meter_mode;
    pipe_id = meter_entry->fields.pipe;
    BCMMETER_FP_PAIRING_CHECK_ASSIGN(meter_mode, meter_pairing);

    BCMMETER_FP_HW_INDEX_USED_SET(unit, stage, pipe_id, hw_index);
    if (meter_pairing == true) {
        BCMMETER_FP_HW_INDEX_USED_SET(unit, stage, pipe_id, (hw_index + 1));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach an FP entry to the meter.
 *
 * Increment the ref count of this meter entry to keep track of
 * number of FP entries pointing to this meter.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_attach (int unit,
                       int stage,
                       uint32_t meter_id)
{
    bcmmeter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_entry = BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id);
    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Called from FP Entry insert/update.
     * Increment ref count and return
     */
    BCMMETER_ENTRY_INC_REF_COUNT(meter_entry);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup an FP entry.
 *
 * Lookup the meter entry from Software meter table
 * and retrieve meter parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 * \param [out] meter_entry Pointer to meter entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_lookup (int unit,
                       int stage,
                       uint32_t meter_id,
                       bcmmeter_fp_entry_t **meter_entry)
{
    SHR_FUNC_ENTER(unit);

    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (!BCMMETER_FP_INDEX_USED_GET(unit, stage, meter_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    *meter_entry = BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id);

    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_oper_get != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_oper_get(unit,
                                                          stage,
                                                          *meter_entry));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a meter entry.
 *
 * Delete the meter entry from hardware if ref count is zero and free the
 * hardware index.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_delete (int unit,
                       uint32_t trans_id,
                       int stage,
                       uint32_t meter_id)
{
    uint32_t hw_meter_idx = 0;
    bcmmeter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_entry = BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id);
    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * If ref count is not zero, return
     */
    if (BCMMETER_ENTRY_GET_REF_COUNT(meter_entry) != 0) {
        SHR_EXIT();
    }

    /*
     * Clear meter entry.
     */
    hw_meter_idx = meter_entry->hw_fields.meter_hw_index;

    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_clear != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_clear(unit,
                                                       trans_id,
                                                       stage,
                                                       meter_entry));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    /*
     * MArk hw index as free.
     */
    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_idx_free != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_idx_free(unit,
                                                          stage,
                                                          hw_meter_idx,
                                                          meter_entry));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    meter_entry->hw_fields.meter_hw_index = 0;
    meter_entry->hw_fields.meter_pair_index = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach an FP entry from the meter.
 *
 * Decrement the ref count of this meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_detach (int unit,
                       int stage,
                       uint32_t meter_id)
{
    bcmmeter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_entry = BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id);
    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Called from FP Entry delete/update.
     * Decrement ref count and return
     */
    if (BCMMETER_ENTRY_GET_REF_COUNT(meter_entry) != 0) {
        BCMMETER_ENTRY_DEC_REF_COUNT(meter_entry);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move a meter entry from one pool to another.
 *
 * If ref count is zero, move the meter entry to the new pool.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] pool Meter pool id.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 * \retval SHR_E_BUSY Meter ref count is not zero.
 */
int
bcmmeter_entry_move (int unit,
                     uint32_t trans_id,
                     int stage,
                     int pool,
                     uint32_t meter_id)
{
    bcmmeter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_entry = BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id);
    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    if (BCMMETER_ENTRY_GET_REF_COUNT(meter_entry) != 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_delete(unit, trans_id, stage, meter_id));

    SHR_IF_ERR_EXIT
        (bcmmeter_entry_insert(unit, trans_id, stage, pool, meter_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update Meter entry
 *
 * Update meter entry in H/W table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_update (int unit,
                       uint32_t trans_id,
                       int stage,
                       uint32_t meter_id)
{
    bcmmeter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_entry = BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id);
    if (meter_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Get the HW values for the new meter params and write to H/W
     */
    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_param_compute != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_param_compute(unit,
                                                               stage,
                                                               meter_entry));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }


    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_write != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_write(unit,
                                                       trans_id,
                                                       stage,
                                                       meter_entry));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }


exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize meter entry.
 *
 * Initialize meter entry with default values from map file.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table identifier.
 * \param [out] out Pointer to output param structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_init_params (int unit,
                            bcmlrd_sid_t sid,
                            bcmmeter_sw_params_t *out)
{
    bcmlrd_fid_t *fid_list = NULL;
    bcmlrd_field_def_t field_def;
    size_t num_fid = 0, count = 0;
    uint32_t i = 0;
    uint32_t table_sz = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit, sid, num_fid, fid_list, &count));

    for (i = 0; i < count; i++) {
        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid_list[i], &field_def));
        switch (fid_list[i]) {
        case METER_FP_ING_TEMPLATEt_METER_FP_ING_TEMPLATE_IDf:
            out->meter_id = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_PIPEf:
            out->pipe = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_METER_FP_ING_POOL_IDf:
            out->meter_pool_id = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_PKT_MODEf:
            out->byte_mode = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_METER_MODEf:
            out->meter_mode = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_COLOR_MODEf:
            out->color_mode = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MIN_RATE_KBPSf:
            out->min_rate_kbps = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_KBITSf:
            out->min_burst_kbits = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MAX_RATE_KBPSf:
            out->max_rate_kbps = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_KBITSf:
            out->max_burst_kbits = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MIN_RATE_PPSf:
            out->min_rate_pps = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_PKTSf:
            out->min_burst_pkts = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MAX_RATE_PPSf:
            out->max_rate_pps = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_PKTSf:
            out->max_burst_pkts = BCMMETER_FP_FIELD_DEF(field_def.dtag);
            break;
        default:
            break;
        }
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get meter field value.
 *
 * Parse user input and get the field corresponding to
 * a particular fid.
 *
 * \param [in] unit         Unit number.
 * \param [in] field_count  Number of fields.
 * \param [in] flist        Pointer to input LT field list.
 * \param [in] fid          field id
 * \param [out] rval        Pointer to return field value
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_field_value_get(int unit,
                               int field_count,
                               bcmltd_field_t **flist,
                               uint32_t fid,
                               uint32_t *rval)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        if (flist[i]->id == fid) {
            *rval = flist[i]->data;
            break;
        }
    }
    if (i == field_count) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get meter config parameters.
 *
 * Parse user input and retrieve Meter LT params.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] field_count Number of fields.
 * \param [in] flist Pointer to input LT field list.
 * \param [out] out Pointer to output param structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_parse_params (int unit,
                             int stage,
                             int field_count,
                             bcmltd_field_t **flist,
                             bcmmeter_sw_params_t *out)
{
    int                 i;
    bool                byte_fid = false;
    bool                pkt_fid = false;
    bool                min_rate_chg = false;
    bool                max_rate_chg = false;
    bool                config_err = false;
    uint32_t            fid;
    uint32_t            fval;
    uint32_t            *min_rate, *max_rate;
    uint32_t            *min_burst, *max_burst;
    bcmlrd_sid_t        sid;
    bcmlrd_field_def_t  field_def;

    SHR_FUNC_ENTER(unit);

    if (stage == BCMMETER_FP_STAGE_ID_INGRESS) {
        sid = METER_FP_ING_TEMPLATEt;
    } else {
        sid = METER_FP_EGR_TEMPLATEt;
    }

    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        fid = flist[i]->id;
        fval = flist[i]->data;

        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid, &field_def));

        /*
         * Validate the field value is within the range.
         * Allowing "0" as a valid value even though it is less than min.
         */
        if (fval > BCMMETER_FP_FIELD_MAX(field_def.dtag)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        } else if (fval < BCMMETER_FP_FIELD_MIN(field_def.dtag)) {
            if (fval != 0) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }

        switch (fid) {
        case METER_FP_ING_TEMPLATEt_METER_FP_ING_TEMPLATE_IDf:
            out->meter_id = fval;
            break;
        case METER_FP_ING_TEMPLATEt_PIPEf:
            out->pipe = fval;
            break;
        case METER_FP_ING_TEMPLATEt_METER_FP_ING_POOL_IDf:
            out->meter_pool_id = fval;
            break;
        case METER_FP_ING_TEMPLATEt_PKT_MODEf:
            out->byte_mode = fval;
            break;
        case METER_FP_ING_TEMPLATEt_METER_MODEf:
            out->meter_mode = fval;
            break;
        case METER_FP_ING_TEMPLATEt_COLOR_MODEf:
            out->color_mode = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_RATE_KBPSf:
            byte_fid = true;
            min_rate_chg = true;
            out->min_rate_kbps = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_KBITSf:
            byte_fid = true;
            out->min_burst_kbits = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_RATE_KBPSf:
            byte_fid = true;
            max_rate_chg = true;
            out->max_rate_kbps = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_KBITSf:
            byte_fid = true;
            out->max_burst_kbits = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_RATE_PPSf:
            pkt_fid = true;
            min_rate_chg = true;
            out->min_rate_pps = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_PKTSf:
            pkt_fid = true;
            out->min_burst_pkts = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_RATE_PPSf:
            pkt_fid = true;
            max_rate_chg = true;
            out->max_rate_pps = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_PKTSf:
            pkt_fid = true;
            out->max_burst_pkts = fval;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_RATE_KBPS_OPERf:
        case METER_FP_ING_TEMPLATEt_MAX_RATE_KBPS_OPERf:
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_KBITS_OPERf:
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_KBITS_OPERf:
        case METER_FP_ING_TEMPLATEt_MIN_RATE_PPS_OPERf:
        case METER_FP_ING_TEMPLATEt_MAX_RATE_PPS_OPERf:
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_PKTS_OPERf:
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_PKTS_OPERf:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
        default:
            break;
        }
    }

    /*
     * Validate that _PPS and _PKTS fields are specified for
     * PKT_MODE = 1 and _KBPS and _KBITS fields are specified
     * for PKT_MODE = 0.
     */
    if (out->byte_mode == BCMMETER_FP_MODE_PKTS) {
        if (byte_fid == true) {
            config_err = true;
        }

        if (!config_err) {
            out->min_rate_kbps   = 0;
            out->max_rate_kbps   = 0;
            out->min_burst_kbits = 0;
            out->max_burst_kbits = 0;
        }

        min_rate = &out->min_rate_pps;
        max_rate = &out->max_rate_pps;
        min_burst = &out->min_burst_pkts;
        max_burst = &out->max_burst_pkts;
    } else {
        if (pkt_fid == true) {
            config_err = true;
        }
        if (!config_err) {
            out->min_rate_pps   = 0;
            out->max_rate_pps   = 0;
            out->min_burst_pkts = 0;
            out->max_burst_pkts = 0;
        }

        min_rate = &out->min_rate_kbps;
        max_rate = &out->max_rate_kbps;
        min_burst = &out->min_burst_kbits;
        max_burst = &out->max_burst_kbits;
    }

    /*
     * Validate meter rate and burst size for each mode.
     */
    switch (out->meter_mode) {
    case BCMMETER_FP_MODE_DEFAULT:
        /*
         * Default meter mode. No validation
         * for metering rate and burst size.
         */
        out->max_rate_kbps = out->max_rate_pps = 0;
        out->max_burst_kbits = out->max_burst_pkts = 0;
        break;
    case BCMMETER_FP_MODE_FLOW:
        /*
         * Flow mode, validate that metering rate
         * and burst size are non-zero.
         */
        if (!(*min_rate) || !(*min_burst)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        out->max_rate_kbps = out->max_rate_pps = 0;
        out->max_burst_kbits = out->max_burst_pkts = 0;
        break;
    case BCMMETER_FP_MODE_SRTCM:
    case BCMMETER_FP_MODE_MODSRTCM:
        /*
         * SRTCM and MODSRTCM require CIR,
         * CBS and EBS. EBS can be zero.
         * Validate that CIR and CBS are
         * non-zero.
         */
        if (!(*min_rate) || !(*max_rate) || !(*min_burst)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        /*
         * EIR should be same as CIR for SRTCM.
         */
        if (min_rate_chg && max_rate_chg) {
            if (*min_rate != *max_rate) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        } else if (min_rate_chg) {
            *max_rate = *min_rate;
        } else if (max_rate_chg) {
            *min_rate = *max_rate;
        } else {
            break;
        }
        break;
    case BCMMETER_FP_MODE_TRTCM:
        /*
         * CIR, PIR, CBS and PBS need to
         * be provided for TRTCM.
         */
        if (!(*min_rate) || !(*min_burst) || !(*max_rate) || !(*max_burst)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        break;
    case BCMMETER_FP_MODE_MODTRTCM:
        /*
         * MODTRTCM can have any one of the committed or peak
         * rates to be zero.
         */
        if (!(*min_rate | *max_rate) || !(*min_burst | *max_burst)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Return meter config parameters.
 *
 * Read software meter table and return meter entry
 * parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table identifier.
 * \param [in] meter_entry Pointer to meter entry.
 * \param [out] flist Pointer to output fieldds list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_populate_params (int unit,
                                bcmlrd_sid_t sid,
                                bcmmeter_fp_entry_t *meter_entry,
                                bcmltd_field_t *flist)
{
    uint8_t     byte_mode;
    size_t      num_fid = 0, count = 0;
    uint32_t    idx = 0, fid = 0;
    uint32_t    table_sz = 0;
    bcmlrd_fid_t         *fid_list = NULL;
    bcmmeter_sw_params_t *in = NULL;

    SHR_FUNC_ENTER(unit);

    in = &meter_entry->fields;
    byte_mode = in->byte_mode;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "Field list");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit, sid, num_fid, fid_list, &count));

    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        flist[idx].id = fid;
        switch (fid) {
        case METER_FP_ING_TEMPLATEt_METER_FP_ING_TEMPLATE_IDf:
            flist[idx].data = in->meter_id;
            break;
        case METER_FP_ING_TEMPLATEt_PIPEf:
            flist[idx].data = in->pipe;
            break;
        case METER_FP_ING_TEMPLATEt_METER_FP_ING_POOL_IDf:
            flist[idx].data = in->meter_pool_id;
            break;
        case METER_FP_ING_TEMPLATEt_PKT_MODEf:
            flist[idx].data = in->byte_mode;
            break;
        case METER_FP_ING_TEMPLATEt_METER_MODEf:
            flist[idx].data = in->meter_mode;
            break;
        case METER_FP_ING_TEMPLATEt_COLOR_MODEf:
            flist[idx].data = in->color_mode;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_RATE_KBPSf:
            flist[idx].data = in->min_rate_kbps;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_KBITSf:
            flist[idx].data = in->min_burst_kbits;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_RATE_KBPSf:
            flist[idx].data = in->max_rate_kbps;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_KBITSf:
            flist[idx].data = in->max_burst_kbits;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_RATE_PPSf:
            flist[idx].data = in->min_rate_pps;
            break;
        case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_PKTSf:
            flist[idx].data = in->min_burst_pkts;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_RATE_PPSf:
            flist[idx].data = in->max_rate_pps;
            break;
        case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_PKTSf:
            flist[idx].data = in->max_burst_pkts;
            break;
        default:
            break;
        }
        if (byte_mode == BCMMETER_FP_MODE_BYTES) {
            switch (fid) {
             case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_KBITS_OPERf:
                flist[idx].data = in->min_burst_oper;
                break;
            case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_KBITS_OPERf:
                flist[idx].data = in->max_burst_oper;
                break;
            case METER_FP_ING_TEMPLATEt_MIN_RATE_KBPS_OPERf:
                flist[idx].data = in->min_rate_oper;
                break;
            case METER_FP_ING_TEMPLATEt_MAX_RATE_KBPS_OPERf:
                flist[idx].data = in->max_rate_oper;
                break;
            default:
                break;
            }
        } else {
            switch (fid) {
            case METER_FP_ING_TEMPLATEt_MIN_BURST_SIZE_PKTS_OPERf:
                flist[idx].data = in->min_burst_oper;
                break;
            case METER_FP_ING_TEMPLATEt_MAX_BURST_SIZE_PKTS_OPERf:
                flist[idx].data = in->max_burst_oper;
                break;
            case METER_FP_ING_TEMPLATEt_MIN_RATE_PPS_OPERf:
                flist[idx].data = in->min_rate_oper;
                break;
            case METER_FP_ING_TEMPLATEt_MAX_RATE_PPS_OPERf:
                flist[idx].data = in->max_rate_oper;
                break;
            default:
                break;
            }
        }
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate meter id.
 *
 * Check if the meter entry is present in s/w table.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software Meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Meter entry not present in s/w table.
 */
int
bcmmeter_entry_valid (int unit,
                      int stage,
                      uint32_t meter_id)
{
    SHR_FUNC_ENTER(unit);

    if (!BCMMETER_FP_INDEX_USED_GET(unit, stage, meter_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Return global status of meter pool.
 *
 * Check the chip specific attributes and return whether the meter pool
 * is global or not.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] global Global pool status of meters in this FP stage.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmmeter_pool_global (int unit,
                      int stage,
                      bool *global)
{
    SHR_FUNC_ENTER(unit);

    *global = (BCMMETER_FP_POOL_GLOBAL(unit, stage));

    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure FP meter related device info.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmmeter_fp_dev_config (int unit,
                        int stage,
                        uint32_t refresh_en)
{
    SHR_FUNC_ENTER(unit);

    if (BCMMETER_FP_FN_PTRS(unit, stage).hw_config != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).hw_config(unit,
                                                        stage,
                                                        refresh_en));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get FP Pipe mode cofnigured.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] mode Operational mode.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmmeter_fp_oper_mode_get (int unit,
                           int stage,
                           uint8_t *mode)
{
    int         rv = SHR_E_NONE;
    uint64_t    val = 0;
    const       bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    *mode = BCMMETER_FP_OPER_MODE_GLOBAL;

    rv = bcmlrd_map_get(unit, FP_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        if (stage == BCMMETER_FP_STAGE_ID_INGRESS) {
            rv = bcmcfg_field_get(unit,
                                  FP_CONFIGt,
                                  FP_CONFIGt_FP_ING_OPERMODEf,
                                  &val);
        } else {
            rv = bcmcfg_field_get(unit,
                                  FP_CONFIGt,
                                  FP_CONFIGt_FP_EGR_OPERMODE_PIPEUNIQUEf,
                                  &val);
        }
    }

    if (rv == SHR_E_NONE) {
        *mode = val;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize FP meter s/w resources.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] warm Warm/Coldboot mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL if handler for init is NULL.
 */
int
bcmmeter_fp_sw_state_init (int unit,
                           int stage,
                           bool cold)
{
    SHR_FUNC_ENTER(unit);

    if (BCMMETER_FP_FN_PTRS(unit, stage).init != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).init(unit,
                                                   cold));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup FP meter s/w resources.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL if handler for cleanup is NULL.
 */
int
bcmmeter_fp_sw_state_cleanup (int unit,
                              int stage)
{
    SHR_FUNC_ENTER(unit);

    if (BCMMETER_FP_FN_PTRS(unit, stage).cleanup != NULL) {
        SHR_IF_ERR_EXIT
            (BCMMETER_FP_FN_PTRS(unit, stage).cleanup(unit));
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
