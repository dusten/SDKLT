/*! \file bcmpc_lth_pm_lane_status.c
 *
 * Logical Table Custom Handlers for PC_PM_LANE_STATUS.
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

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_lth_pm_lane_status.h>
#include <bcmpc/bcmpc_lth_internal.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH

/*! Logical table ID for this handler. */
#define LTH_SID PC_PM_LANE_STATUSt

/*! Data structure to save the data of PC_PM_LANE_STATUSt entry. */
typedef struct pc_pm_lane_st_data_s {
    bcmpc_pm_lane_t pm_lane;
    bcmpc_pm_lane_status_t lane_st;
} pc_pm_lane_st_data_t;


/*******************************************************************************
 * Private functions
 */

static void
pc_pm_lane_st_data_t_init(pc_pm_lane_st_data_t *lane_st_data)
{
    lane_st_data->pm_lane.pm_id = -1;
    lane_st_data->pm_lane.core_idx = -1;
    lane_st_data->pm_lane.core_lane = -1;
    bcmpc_pm_lane_status_t_init(&(lane_st_data->lane_st));
}

/*!
 * \brief Get the number of cores and the lane per core for the given PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM_ID.
 * \param [out] num_cores The number of cores.
 * \param [out] lanes_per_core The number of lanes per core.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_pm_rsrc_get(int unit, int pm_id, int *num_cores, int *lanes_per_core)
{
    bcmpc_pm_info_t pm_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));
    *num_cores = pm_info.prop.num_aggr;
    *lanes_per_core = pm_info.prop.num_lanes / pm_info.prop.num_aggr;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LTA input fields parsing.
 *
 * Parse LTA input fields and save the data to pc_pm_lane_st_data_t.
 *
 * \param [in] unit Unit number.
 * \param [in] in LTA input field array.
 * \param [out] lane_st_data PM lane status data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c lane_st_data.
 */
static int
lt_fields_parse(int unit, const bcmltd_fields_t *in,
                pc_pm_lane_st_data_t *lane_st_data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    bcmpc_pm_lane_t *pm_lane = &lane_st_data->pm_lane;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        fid = in->field[i]->id;
        fval = in->field[i]->data;

        switch (fid) {
            case PC_PM_LANE_STATUSt_PC_PM_IDf:
                pm_lane->pm_id = fval;
                break;
            case PC_PM_LANE_STATUSt_CORE_INDEXf:
                pm_lane->core_idx = fval;
                break;
            case PC_PM_LANE_STATUSt_CORE_LANEf:
                pm_lane->core_lane = fval;
                break;
            default:
                continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_lth_field_validate(unit, LTH_SID, in->field[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the input keys of LT operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lane_st_data Lane status data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more keys are not given.
 * \retval SHR_E_PARAM One or more key values are invalid.
 */
static int
lt_key_fields_validate(int unit, pc_pm_lane_st_data_t *lane_st_data)
{
    int num_cores, lanes_per_core;
    bcmpc_pm_lane_t *pm_lane = &lane_st_data->pm_lane;

    SHR_FUNC_ENTER(unit);

    if (pm_lane->pm_id == -1) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "PC_PM_ID is not specified.\n")));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pm_rsrc_get(unit, pm_lane->pm_id, &num_cores, &lanes_per_core));

    if (pm_lane->core_idx == -1) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "CORE_INDEX is not specified.\n")));
    }

    if (pm_lane->core_idx >= num_cores) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Invalid CORE_INDEX. PC_PM_ID %d only have %d "
                         "cores.\n"),
              pm_lane->pm_id, num_cores));
    }

    if (pm_lane->core_lane == -1) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "CORE_LANE is not specified.\n")));
    }

    if (pm_lane->core_lane >= lanes_per_core) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Invalid CORE_LNAE. PC_PM_ID %d only have %d "
                         "lanes per core.\n"),
              pm_lane->pm_id, lanes_per_core));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_pm_lane_status_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    pc_pm_lane_st_data_t lane_st_data;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(arg);

    pc_pm_lane_st_data_t_init(&lane_st_data);
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &lane_st_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_key_fields_validate(unit, &lane_st_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_lane_status_insert(int unit,
                                uint32_t trans_id,
                                const bcmltd_fields_t *in,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_lane_status_lookup(int unit,
                                uint32_t trans_id,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    int fcnt = 0, idx;
    pc_pm_lane_st_data_t lane_st_data;
    bcmpc_pm_lane_t *pm_lane = &lane_st_data.pm_lane;
    bcmpc_pm_lane_status_t *lane_st = &lane_st_data.lane_st;
    bcmpc_pm_tx_lane_prof_t *tx_st = &lane_st->tx;
    bcmpc_pm_rx_lane_prof_t *rx_st = &lane_st->rx;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(arg);

    pc_pm_lane_st_data_t_init(&lane_st_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_fields_parse(unit, in, &lane_st_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_lane_status_get(unit, pm_lane, lane_st));

    /* Key fields. */
    BCMPC_LTH_FIELD_ADD(out, PC_PHYS_PORTt_PC_PHYS_PORT_IDf, 0,
                        pm_lane->pm_id, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_CORE_INDEXf, 0,
                        pm_lane->core_idx, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_CORE_LANEf, 0,
                        pm_lane->core_lane, fcnt);

    /* Tx lane status. */
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_TX_PREf, 0,
                        tx_st->pre, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_TX_MAINf, 0,
                        tx_st->main, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_TX_POSTf, 0,
                        tx_st->post, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_TX_POST2f, 0,
                        tx_st->post2, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_TX_POST3f, 0,
                        tx_st->post3, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_TX_AMPf, 0,
                        tx_st->amp, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_TX_DRV_MODEf, 0,
                        tx_st->drv_mode, fcnt);

    /* Rx lane status. */
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_VGAf, 0,
                        rx_st->rx_vga, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_VGA_OVERRIDEf, 0,
                        rx_st->rx_vga_override, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_PEAK_FILTERf, 0,
                        rx_st->rx_peak_filter, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_PEAK_FILTER_OVERRIDEf, 0,
                        rx_st->rx_peak_filter_override, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_LF_PEAK_FILTERf, 0,
                        rx_st->rx_lf_peak_filter, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_LF_PEAK_FILTER_OVERRIDEf, 0,
                        rx_st->rx_lf_peak_filter_override, fcnt);
    BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_ADAPf, 0,
                        rx_st->rx_adap, fcnt);
    for (idx = 0; idx < BCMPC_NUM_RX_DFE_TAPS_MAX; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_DFEf, idx,
                            rx_st->rx_def[idx], fcnt);
    }
    for (idx = 0; idx < BCMPC_NUM_RX_DFE_TAPS_MAX; idx++) {
        BCMPC_LTH_FIELD_ADD(out, PC_PM_LANE_STATUSt_RX_DFE_OVERRIDEf, idx,
                            rx_st->rx_def_override[idx], fcnt);
    }

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_lane_status_delete(int unit,
                                uint32_t trans_id,
                                const bcmltd_fields_t *in,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_lane_status_update(int unit,
                                uint32_t trans_id,
                                const bcmltd_fields_t *in,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_lane_status_first(int unit,
                               uint32_t trans_id,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg)
{
    int fcnt;
    bcmltd_fields_t flds;
    bcmltd_field_t *flist[3], fld[3];
    uint64_t min, max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PM_LANE_STATUSt_PC_PM_IDf,
                                         &min, &max));

    fcnt = 0;
    flds.field = flist;
    flds.field[fcnt] = &fld[fcnt];
    flds.field[fcnt]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
    flds.field[fcnt]->data = min;
    fcnt++;
    flds.field[fcnt] = &fld[fcnt];
    flds.field[fcnt]->id = PC_PM_LANE_STATUSt_CORE_INDEXf;
    flds.field[fcnt]->data = 0;
    fcnt++;
    flds.field[fcnt] = &fld[fcnt];
    flds.field[fcnt]->id = PC_PM_LANE_STATUSt_CORE_LANEf;
    flds.field[fcnt]->data = 0;
    fcnt++;
    flds.count = fcnt;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_pm_lane_status_lookup(unit, trans_id, &flds, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_pm_lane_status_next(int unit,
                              uint32_t trans_id,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    int rv, fcnt;
    bcmltd_fields_t flds;
    bcmltd_field_t *flist[3], fld[3];
    uint64_t pm_id, core_idx, core_lane;
    uint64_t min, max;
    int num_cores, lanes_per_core;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PM_LANE_STATUSt_PC_PM_IDf,
                             &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PM_LANE_STATUSt_CORE_INDEXf,
                             &core_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_get(unit, in, PC_PM_LANE_STATUSt_CORE_LANEf,
                             &core_lane));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lth_field_value_range_get(unit, LTH_SID,
                                         PC_PM_LANE_STATUSt_PC_PM_IDf,
                                         &min, &max));

    /* Get the number of the cores and the lanes per core for the PM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pm_rsrc_get(unit, pm_id, &num_cores, &lanes_per_core));

    core_lane++;

    /* Stop traverse when exceeding the max PM ID. */
    while (pm_id <= max) {
        if (core_lane >= (uint64_t)lanes_per_core) {
            core_idx++;
            core_lane = 0;
        }
        if (core_idx >= (uint64_t)num_cores) {
            pm_id++;
            core_idx = 0;
            /* Update the PM resource according to the new PM ID. */
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_pm_rsrc_get(unit, pm_id, &num_cores, &lanes_per_core));
        }

        /* Exit the current iteration once we get a valid entry. */
        fcnt = 0;
        flds.field = flist;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PHYS_PORTt_PC_PHYS_PORT_IDf;
        flds.field[fcnt]->data = pm_id;
        fcnt++;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PM_LANE_STATUSt_CORE_INDEXf;
        flds.field[fcnt]->data = core_idx;
        fcnt++;
        flds.field[fcnt] = &fld[fcnt];
        flds.field[fcnt]->id = PC_PM_LANE_STATUSt_CORE_LANEf;
        flds.field[fcnt]->data = core_lane;
        fcnt++;
        flds.count = fcnt;
        rv = bcmpc_lth_pm_lane_status_lookup(unit, trans_id, &flds, out, arg);
        if (SHR_SUCCESS(rv)) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        core_lane++;
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}
