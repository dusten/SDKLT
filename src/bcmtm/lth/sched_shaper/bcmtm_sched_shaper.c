/*! \file bcmtm_sched_shaper.c
 *
 * APIs for Scheduler Shaper handlers for Front panel ports/ Nodes, CPU ports/
 * nodes and Managment and loop back ports.
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
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_shaper.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm_sched_shaper/bcmtm_sched_shaper_internal.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_utils_internal.h>
#include <bcmtm/sched_shaper/bcmtm_rate_encoding_internal.h>

/*******************************************************************************
 * Local definitions
 */
bcmtm_shaper_burst_t
bcmtm_node_shaper_burst[BCMTM_MAX_SCHED_LEVEL][BCMTM_MAX_FP_NODE];
struct bcmtm_sched_shaper_config bcmtm_sched_shaper_config;
#define BCMTM_SCHED_SHAPER_INDEX(port, cos)  \
    index = port * BCMTM_MAX_COS + cos; \


/*******************************************************************************
 * Public functions
 */
int
bcmtm_scheduler_shaper_validate (int unit,
            bcmlt_opcode_t opcode,
            const bcmltd_fields_t *in,
            const bcmltd_generic_arg_t *arg)
{
    int node_type = 0;
    uint16_t cos = 0, port_id = 0;
    int index = 0;
    int bmap_ret = 0;
    bcmdrd_sid_t sid = arg->sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmtm_lt_field_validate(unit, opcode, sid, in));

    /*since keys are mandate and the list is in sorted order*/
    port_id = in->field[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf]->data;
    cos = in->field[TM_SCHEDULER_SHAPER_NODESt_COSf]->data;
    node_type = in->field[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]->data;

    BCMTM_SCHED_SHAPER_INDEX(port_id, cos);
    bmap_ret =
        SHR_BITGET(bcmtm_sched_shaper_config.entry_bitmap[node_type], index);

    SHR_IF_ERR_EXIT(bcmtm_lt_opcode_validate(unit, opcode, bmap_ret));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_shaper_insert (int unit,
            uint32_t trans_id,
            const bcmltd_fields_t *in,
            const bcmltd_generic_arg_t *arg)
{
    uint32_t in_data[TM_SCHEDULER_SHAPER_NODESt_FIELD_COUNT] = { 0 };
    size_t num_fid = 0;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    uint32_t max_refresh_rate, max_granularity, max_bucket_size;
    uint32_t min_refresh_rate, min_granularity, min_bucket_size;
    int index = 0;
    uint32_t ltm_buf[3] = {0};
    uint32_t refresh_bitsize = 0, bucket_bitsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    int i = 0;
    uint32_t fval = 0;
    uint32_t itu_mode = 0;
    uint16_t  node_type, cos, port;
    int mport, pipe;
    bcmltd_sid_t ltid = arg->sid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT(bcmtm_in_field_parser(unit, in, ltid, num_fid, in_data));

    node_type = in_data[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf];
    cos = in_data[TM_SCHEDULER_SHAPER_NODESt_COSf];
    port = in_data[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf];

    SHR_IF_ERR_EXIT(bcmtm_port_mmu_local_port_get(unit, port, &mport));

    SHR_IF_ERR_EXIT(bcmtm_port_pipe_get(unit, port, &pipe));

    index =  ((node_type == 2) ? FP_MC_BASE : 0);
    index += mport * BCMTM_MAX_COS + cos;

    SHR_IF_ERR_CONT(bcmtm_shaper_conf_get(unit, trans_id, &itu_mode));
    sid =
        bcmtm_node_shaper_sid[in_data[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]];

    SHR_IF_ERR_EXIT
        (bcmtm_pt_fid_size_get(unit, sid,
                bcmtm_node_shaper_fid[BCMTM_NODE_SHAPER_MAX_REFRESHRATE], &refresh_bitsize));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_fid_size_get(unit, sid,
                bcmtm_node_shaper_fid[BCMTM_NODE_SHAPER_MAX_BUCKETSIZE], &bucket_bitsize));

    if ((in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BANDWIDTH_KBPSf] <
            in_data[TM_SCHEDULER_SHAPER_NODESt_MIN_BANDWIDTH_KBPSf]) &&
        (in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BANDWIDTH_KBPSf])) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if ((in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BURST_SIZE_KBITSf] <
            in_data[TM_SCHEDULER_SHAPER_NODESt_MIN_BURST_SIZE_KBITSf]) &&
        (in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BURST_SIZE_KBITSf])){
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }


    SHR_IF_ERR_EXIT(bcmtm_rate_to_bucket_set(unit, port,
                refresh_bitsize, bucket_bitsize, itu_mode,
                in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BANDWIDTH_KBPSf],
                in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BURST_SIZE_KBITSf],
                in_data[TM_SCHEDULER_SHAPER_NODESt_BURST_SIZE_AUTOf],
                in_data[TM_SCHEDULER_SHAPER_NODESt_SHAPING_MODEf],
                &max_refresh_rate, &max_granularity, &max_bucket_size));

    SHR_IF_ERR_EXIT(bcmtm_rate_to_bucket_set(unit, port,
                refresh_bitsize, bucket_bitsize, itu_mode,
                in_data[TM_SCHEDULER_SHAPER_NODESt_MIN_BANDWIDTH_KBPSf],
                in_data[TM_SCHEDULER_SHAPER_NODESt_MIN_BURST_SIZE_KBITSf],
                in_data[TM_SCHEDULER_SHAPER_NODESt_BURST_SIZE_AUTOf],
                in_data[TM_SCHEDULER_SHAPER_NODESt_SHAPING_MODEf],
                &min_refresh_rate, &min_granularity, &min_bucket_size));

    /* populate the values */
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
    for ( i = 0; i < BCMTM_NODE_SHAPER_MAXFID; i++){
        fval = 0;
        switch(i) {
            case BCMTM_NODE_SHAPER_EAV:
                fval = in_data[TM_SCHEDULER_SHAPER_NODESt_ETHERNET_AVf];
                break;
            case BCMTM_NODE_SHAPER_MODE:
                fval = in_data[TM_SCHEDULER_SHAPER_NODESt_SHAPING_MODEf];
                break;
            case BCMTM_NODE_SHAPER_MAX_REFRESHRATE:
                fval = max_refresh_rate;
                break;
            case BCMTM_NODE_SHAPER_MAX_GRANULARITY:
                fval = max_granularity;
                break;
            case BCMTM_NODE_SHAPER_MAX_BUCKETSIZE:
                fval = max_bucket_size;
                break;
            case BCMTM_NODE_SHAPER_MIN_REFRESHRATE:
                fval = min_refresh_rate;
                break;
            case BCMTM_NODE_SHAPER_MIN_GRANULARITY:
                fval = min_granularity;
                break;
            case BCMTM_NODE_SHAPER_MIN_BUCKETSIZE:
                fval = min_bucket_size;
                break;
        }
        fid = bcmtm_node_shaper_fid[i];
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));
    }

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                (void *)&pt_dyn_info, ltm_buf));

    sid =
        bcmtm_sched_weight_sid[node_type];

    sal_memset(ltm_buf, 0, sizeof(ltm_buf));

    fval = in_data[TM_SCHEDULER_SHAPER_NODESt_WEIGHTf];
    fid = bcmtm_sched_weight_fid[BCMTM_SCHED_WEIGHT];

    if (in_data[TM_SCHEDULER_SHAPER_NODESt_SCHED_MODEf] == 0) {
        fval = 0;
    }

    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                (void *)&pt_dyn_info, ltm_buf));

    sal_memset(ltm_buf, 0, sizeof(ltm_buf));
    /*per port indexed registers */
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);

    fid = bcmtm_fp_scheduler_fid[BCMTM_FP_SCHED_WRR];
    if (in_data[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf] == 0) {
        sid = bcmtm_fp_scheduler_sid[0];
        fval = in_data[TM_SCHEDULER_SHAPER_NODESt_SCHED_MODEf];
        index = 0;
    } else {
        index =  cos;
        sid = bcmtm_fp_scheduler_sid[1];
        SHR_IF_ERR_EXIT(
                bcmtm_pt_indexed_read(unit, sid, ltid,
                    (void *)&pt_dyn_info, ltm_buf));
        SHR_IF_ERR_EXIT(bcmtm_field_get(unit, sid, fid, ltm_buf, &fval));
    }
    fval &= ~(1 << index);
    if (in_data[TM_SCHEDULER_SHAPER_NODESt_SCHED_MODEf] == 1) {
        fval |= (1 << index);
    }

    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                (void *)&pt_dyn_info, ltm_buf));
    BCMTM_SCHED_SHAPER_INDEX(port, cos);
    bcmtm_node_shaper_burst[node_type][index].max_burst =
        in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BURST_SIZE_KBITSf];
    bcmtm_node_shaper_burst[node_type][index].min_burst =
        in_data[TM_SCHEDULER_SHAPER_NODESt_MIN_BURST_SIZE_KBITSf];
    bcmtm_node_shaper_burst[node_type][index].burst_auto=
        in_data[TM_SCHEDULER_SHAPER_NODESt_BURST_SIZE_AUTOf];
    bcmtm_node_shaper_burst[node_type][index].max_bandwidth=
        in_data[TM_SCHEDULER_SHAPER_NODESt_MAX_BANDWIDTH_KBPSf];
    bcmtm_node_shaper_burst[node_type][index].min_bandwidth=
        in_data[TM_SCHEDULER_SHAPER_NODESt_MIN_BANDWIDTH_KBPSf];

    SHR_BITSET(bcmtm_sched_shaper_config.entry_bitmap[node_type],index);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_shaper_lookup(int unit,
        uint32_t trans_id,
        const bcmltd_fields_t *in,
        bcmltd_fields_t *out,
        const bcmltd_generic_arg_t *arg)
{
    bcmdrd_sid_t sid = 0;
    int index = 0;
    size_t count = 0;
    uint32_t ltm_buf[3] = {0};
    uint16_t cos, port, node_type;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    int weight = 0;
    int i = 0, mport, pipe;
    uint32_t itu_mode = 0;
    uint32_t fval = 0;
    bcmdrd_fid_t fid  = 0;
    bcmltd_sid_t ltid = TM_SCHEDULER_SHAPER_NODESt;
    uint32_t max_refresh = 0, max_bucket = 0, max_gran = 0;
    uint32_t min_refresh = 0, min_bucket = 0, min_gran = 0;
    uint32_t max_bandwidth = 0, max_burst = 0;
    uint32_t min_bandwidth, min_burst = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* parsing the in list */
    while (count < out->count) {
        out->field[count]->id = count;
        count++;
    }

    SHR_IF_ERR_CONT(bcmtm_shaper_conf_get(unit, trans_id, &itu_mode));

    node_type = in->field[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]->data;
    cos = in->field[TM_SCHEDULER_SHAPER_NODESt_COSf]->data;
    port = in->field[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf]->data;

    out->field[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]->data = node_type;
    out->field[TM_SCHEDULER_SHAPER_NODESt_COSf]->data = cos;
    out->field[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf]->data = port;

    SHR_IF_ERR_EXIT(bcmtm_port_mmu_local_port_get(unit, port, &mport));
    SHR_IF_ERR_EXIT(bcmtm_port_pipe_get(unit, port, &pipe));

    index = (mport * BCMTM_MAX_COS)+ cos;

    /* value transform function */
    /* get the sid based on node_type*/
    sid = bcmtm_node_shaper_sid[node_type];

    /* populate the values */
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                (void *)&pt_dyn_info, ltm_buf));

    for ( i = 0; i < BCMTM_NODE_SHAPER_MAXFID; i++){
        fid = bcmtm_node_shaper_fid[i];
        SHR_IF_ERR_EXIT(bcmtm_field_get(unit, sid, fid, ltm_buf, &fval));
        switch(i) {
            case BCMTM_NODE_SHAPER_EAV:
                out->field[TM_SCHEDULER_SHAPER_NODESt_ETHERNET_AVf]->data = fval;
                break;
            case BCMTM_NODE_SHAPER_MODE:
                out->field[TM_SCHEDULER_SHAPER_NODESt_SHAPING_MODEf]->data = fval;
                break;
            case BCMTM_NODE_SHAPER_MAX_REFRESHRATE:
                max_refresh = fval;
                break;
            case BCMTM_NODE_SHAPER_MAX_GRANULARITY:
                max_gran = fval;
                break;
            case BCMTM_NODE_SHAPER_MAX_BUCKETSIZE:
                max_bucket = fval;
                break;
            case BCMTM_NODE_SHAPER_MIN_REFRESHRATE:
                min_refresh = fval;
                break;
            case BCMTM_NODE_SHAPER_MIN_GRANULARITY:
                min_gran = fval;
                break;
            case BCMTM_NODE_SHAPER_MIN_BUCKETSIZE:
                min_bucket = fval;
                break;
        }
    }
    /*calculating max shaper rate */
    SHR_IF_ERR_EXIT(bcmtm_bucket_to_rate_get(unit, port, itu_mode,
                  out->field[TM_SCHEDULER_SHAPER_NODESt_SHAPING_MODEf]->data,
                  max_refresh, max_bucket, max_gran,
                  &max_bandwidth, &max_burst));
    /*calculating max shaper rate */
    SHR_IF_ERR_EXIT(bcmtm_bucket_to_rate_get(unit, port, itu_mode,
                  out->field[TM_SCHEDULER_SHAPER_NODESt_SHAPING_MODEf]->data,
                  min_refresh, min_bucket, min_gran,
                  &min_bandwidth, &min_burst));


    /* store the values in local data structures */
    sid = bcmtm_sched_weight_sid[node_type];
    sal_memset(ltm_buf, 0, sizeof(ltm_buf));

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                (void *)&pt_dyn_info, ltm_buf));
    fval = 0;
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, sid, bcmtm_sched_weight_fid[0], ltm_buf, &fval));
    out->field[TM_SCHEDULER_SHAPER_NODESt_WEIGHTf]->data = fval;
    weight = fval;
    sal_memset(ltm_buf, 0, sizeof(ltm_buf));
    fval = 0;

    /*per port indexed registers */
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);

    if (node_type == 0) {
        sid = bcmtm_fp_scheduler_sid[0];
        index = 0;
    } else {
        sid = bcmtm_fp_scheduler_sid[1];
        index = cos;
    }
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                (void *)&pt_dyn_info, ltm_buf));
    fid = bcmtm_fp_scheduler_fid[BCMTM_FP_SCHED_WRR];
    SHR_IF_ERR_EXIT(bcmtm_field_get(unit, sid, fid, ltm_buf, &fval));
    fval &= (1 << index);

    if ((fval > 0) && (weight > 0)){
        out->field[TM_SCHEDULER_SHAPER_NODESt_SCHED_MODEf]->data = 1;
    } else if (weight > 0) {
        out->field[TM_SCHEDULER_SHAPER_NODESt_SCHED_MODEf]->data = 2;
    } else if (weight == 0){
        out->field[TM_SCHEDULER_SHAPER_NODESt_SCHED_MODEf]->data = 0;
    }

    BCMTM_SCHED_SHAPER_INDEX(port, cos);
    out->field[TM_SCHEDULER_SHAPER_NODESt_MAX_BANDWIDTH_KBPS_OPERf]->data = max_bandwidth;
    out->field[TM_SCHEDULER_SHAPER_NODESt_MAX_BURST_SIZE_KBITS_OPERf]->data = max_burst;
    out->field[TM_SCHEDULER_SHAPER_NODESt_MAX_BURST_SIZE_KBITSf]->data =
        bcmtm_node_shaper_burst[node_type][index].max_burst;
    out->field[TM_SCHEDULER_SHAPER_NODESt_MAX_BANDWIDTH_KBPSf]->data =
        bcmtm_node_shaper_burst[node_type][index].max_bandwidth;
    out->field[TM_SCHEDULER_SHAPER_NODESt_MIN_BANDWIDTH_KBPS_OPERf]->data = min_bandwidth;
    out->field[TM_SCHEDULER_SHAPER_NODESt_MIN_BURST_SIZE_KBITS_OPERf]->data = min_burst;
    out->field[TM_SCHEDULER_SHAPER_NODESt_MIN_BURST_SIZE_KBITSf]->data =
        bcmtm_node_shaper_burst[node_type][index].min_burst;
    out->field[TM_SCHEDULER_SHAPER_NODESt_MIN_BANDWIDTH_KBPSf]->data =
        bcmtm_node_shaper_burst[node_type][index].min_bandwidth;
    out->field[TM_SCHEDULER_SHAPER_NODESt_BURST_SIZE_AUTOf]->data =
        bcmtm_node_shaper_burst[node_type][index].burst_auto;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_shaper_delete(int unit,
        uint32_t trans_id,
        const bcmltd_fields_t *in,
        const bcmltd_generic_arg_t *arg)
{
    bcmdrd_sid_t sid = 0;
    int index = 0, pipe, mport;
    uint32_t ltm_buf[3] = {0};
    uint16_t  port, cos, node_type;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = TM_SCHEDULER_SHAPER_NODESt;

    SHR_FUNC_ENTER(unit);

    /* parsing the in list */
    port = in->field[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf]->data;
    cos = in->field[TM_SCHEDULER_SHAPER_NODESt_COSf]->data;
    node_type = in->field[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]->data;

    SHR_IF_ERR_EXIT(bcmtm_port_mmu_local_port_get(unit, port, &mport));
    SHR_IF_ERR_EXIT(bcmtm_port_pipe_get(unit, port, &pipe));

    index = ((node_type == 2) ? FP_MC_BASE : 0 );
    index += mport * BCMTM_MAX_COS + cos;

    BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);

    /* prepare ltm_buffer */
    sid = bcmtm_node_shaper_sid[node_type];

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                            (void *)&pt_dyn_info, ltm_buf));

    sid = bcmtm_sched_weight_sid[node_type];
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                            (void *)&pt_dyn_info, ltm_buf));

    /* per port indexed registers */
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    if (node_type == 0) {
        sid = bcmtm_fp_scheduler_sid[0];
    } else {
        sid = bcmtm_fp_scheduler_sid[1];
    }
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                            (void *)&pt_dyn_info, ltm_buf));

    BCMTM_SCHED_SHAPER_INDEX(port, cos);
    sal_memset(&(bcmtm_node_shaper_burst[node_type][index]), 0,
                                    sizeof(bcmtm_shaper_burst_t));
    SHR_BITCLR(bcmtm_sched_shaper_config.entry_bitmap[node_type],index);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_shaper_update(int unit,
        uint32_t trans_id,
        const bcmltd_fields_t *in,
        const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t out;
    size_t num_fields = 0;
    size_t count = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(
      bcmlrd_field_count_get(unit, arg->sid, &num_fields));

    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, num_fields, &out));

    SHR_IF_ERR_EXIT(
            bcmtm_scheduler_shaper_lookup(unit, trans_id, in, &out, arg));

    for (count = 0; count < in->count; count++) {
        out.field[in->field[count]->id]->data = in->field[count]->data;
    }

    SHR_IF_ERR_EXIT(bcmtm_scheduler_shaper_insert (unit, trans_id, &out, arg));
exit:
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_shaper_first(int unit,
                             uint32_t trans_id,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int index = 0;
    int bmap_ret = 0;
    int cos = 0;
    int port = 0;
    int node_type = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* get the index */
    for (node_type = 0; node_type < BCMTM_MAX_SCHED_LEVEL; node_type++) {
        for (index = 0; index < BCMTM_MAX_FP_NODE; index++) {
            bmap_ret = SHR_BITGET(
                bcmtm_sched_shaper_config.entry_bitmap[node_type], index);
            if (bmap_ret) {
                cos = index% BCMTM_MAX_COS;
                port = index/BCMTM_MAX_COS;
                break;
            }
        }
        /* entry found .*/
        if (port)
            break;
    }
    if (!port)
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

    /* prepare the in field params for lookup */
    out->field[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf]->data = port;
    out->field[TM_SCHEDULER_SHAPER_NODESt_COSf]->data = cos;
    out->field[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]->data = node_type;

    /* entry lookup */
    SHR_IF_ERR_EXIT(
        bcmtm_scheduler_shaper_lookup(unit, trans_id, out, out, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_shaper_next(int unit,
                            uint32_t trans_id,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    int cur_idx, next_idx;
    int node_type, cos;
    int port = 0;
    int bmap_ret;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    port = in->field[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf]->data;
    cos = in->field[TM_SCHEDULER_SHAPER_NODESt_COSf]->data;
    node_type = in->field[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]->data;

    cur_idx = port * BCMTM_MAX_COS + cos;
    port = 0;
    /* get the next index */
    for ( ; node_type < BCMTM_MAX_SCHED_LEVEL; node_type++) {
        for (next_idx = ++cur_idx; next_idx < BCMTM_MAX_FP_NODE; next_idx++) {
            bmap_ret = SHR_BITGET(
                bcmtm_sched_shaper_config.entry_bitmap[node_type], next_idx);
            if (bmap_ret) {
                cos = next_idx% BCMTM_MAX_COS;
                port = next_idx/BCMTM_MAX_COS;
                break;
            }
        }
        /* entry found .*/
        if (port)
            break;
        else
            cur_idx = -1;
    }
    if (!port)
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

    /* prepare the in field params for lookup */
    out->field[TM_SCHEDULER_SHAPER_NODESt_PORT_IDf]->data = port;
    out->field[TM_SCHEDULER_SHAPER_NODESt_COSf]->data = cos;
    out->field[TM_SCHEDULER_SHAPER_NODESt_NODE_TYPEf]->data = node_type;

    /* entry lookup */
    SHR_IF_ERR_EXIT(
        bcmtm_scheduler_shaper_lookup(unit, trans_id, out, out, arg));

exit:
    SHR_FUNC_EXIT();
}
