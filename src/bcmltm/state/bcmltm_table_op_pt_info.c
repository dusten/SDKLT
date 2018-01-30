/*! \file bcmltm_table_op_pt_info.c
 *
 * Interface to update in-memory table with PT status.
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
#include <bcmltm/bcmltm_state_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffffL
#endif

/* Arbitary high table field length, to accomodate for array depth */
#define TABLE_OP_PT_INFO_MAX_FIELD_LEN 32

static shr_famm_hdl_t table_op_pt_info_arr_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static int
bcmltm_table_op_pt_stats_read(int unit, bcmltm_table_op_pt_info_t *ptrm_status,
                              uint32_t attr, bcmltd_field_t **flds,
                              uint32_t *fld_count, uint32_t index)
{
    if (ptrm_status == NULL) {
        /* Entry not found. */
        return SHR_E_PARAM;
    }

    switch (attr) {
    case TABLE_OP_PT_INFOt_TABLE_IDf:
        flds[*fld_count]->data = ptrm_status->lt_id;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_AGGREGATE_VIEWf:
        flds[*fld_count]->data = ptrm_status->aggregate_view;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_INSTANCEf:
        flds[*fld_count]->data = ptrm_status->pt_instance;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_ID_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_sid_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_IDf:
        if (index < ptrm_status->pt_sid_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_sid[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_CNTf:
        flds[*fld_count]->data = ptrm_status->pt_index_cnt;
        (*fld_count)++;
        break;
    case TABLE_OP_PT_INFOt_PT_INDEXf:
        if (index < ptrm_status->pt_index_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_index[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_ID_DATA_CNTf:
        if (!ptrm_status->aggregate_view) {
            flds[*fld_count]->data = ptrm_status->pt_sid_data_cnt;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_ID_DATAf:
        if (!ptrm_status->aggregate_view &&
            index < ptrm_status->pt_sid_data_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_sid_data[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_DATA_CNTf:
        if (!ptrm_status->aggregate_view) {
            flds[*fld_count]->data = ptrm_status->pt_index_data_cnt;
            (*fld_count)++;
        }
        break;
    case TABLE_OP_PT_INFOt_PT_INDEX_DATAf:
        if (!ptrm_status->aggregate_view &&
            index < ptrm_status->pt_index_data_cnt) {
            flds[*fld_count]->data = ptrm_status->pt_index_data[index];
            flds[*fld_count]->idx = index;
            (*fld_count)++;
        }
        break;

    default:
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

static int
bcmltm_table_op_pt_info_update_data(int unit,
                                 bcmltm_table_op_pt_info_t *ptrm_status)
{
    bcmltd_fields_t in_flds, out_flds;
    uint32_t fld, fld_count=0, index;
    int entry_exists = 0, hi_pri = 1;
    bcmlrd_field_def_t  field_def;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Init for error return. */
    out_flds.field = NULL;

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(table_op_pt_info_arr_hdl[unit], 1);

    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = TABLE_OP_PT_INFO_MAX_FIELD_LEN;
    out_flds.field = shr_famm_alloc(table_op_pt_info_arr_hdl[unit],
                                    TABLE_OP_PT_INFO_MAX_FIELD_LEN);

    if (out_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    /* Set logical table ID */
    fld = TABLE_OP_PT_INFOt_TABLE_IDf;
    in_flds.field[0]->id = fld;
    if (bcmltm_table_op_pt_stats_read(unit, ptrm_status, fld,
                                      in_flds.field, &fld_count, 0) ==
            SHR_E_UNAVAIL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Lookup the entry. Update the entry if exists. */
    if (bcmimm_entry_lookup(unit, TABLE_OP_PT_INFOt,
                                &in_flds, &out_flds) != SHR_E_NOT_FOUND) {
        entry_exists = 1;
    }

    shr_famm_free(table_op_pt_info_arr_hdl[unit], in_flds.field, in_flds.count);

    in_flds.count = TABLE_OP_PT_INFO_MAX_FIELD_LEN;
    in_flds.field = shr_famm_alloc(table_op_pt_info_arr_hdl[unit],
                                   TABLE_OP_PT_INFO_MAX_FIELD_LEN);

    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    /* Fill complete field set. */
    fld_count = 0;
    for (fld = 0; fld < TABLE_OP_PT_INFOt_FIELD_COUNT; fld++) {
        sal_memset(&field_def, 0, sizeof(field_def));

        rv = bcmlrd_table_field_def_get(unit, TABLE_OP_PT_INFOt, fld,
                                        &field_def);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_UNAVAIL) {
                continue;  /* Skip unmapped field */
            } else {
                SHR_RETURN_VAL_EXIT(rv);
            }
        }

        for (index = 0; index < (field_def.depth ? field_def.depth : 1);
             index++) {
            in_flds.field[fld_count]->id = fld;
            if (bcmltm_table_op_pt_stats_read(unit, ptrm_status, fld,
                                              in_flds.field,
                                              &fld_count, index) ==
                    SHR_E_UNAVAIL) {
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    }
    in_flds.count = fld_count;

    if (entry_exists) {
        SHR_IF_ERR_EXIT(bcmimm_entry_update(unit, hi_pri,
                                            TABLE_OP_PT_INFOt, &in_flds));
    } else {
        SHR_IF_ERR_EXIT(bcmimm_entry_insert(unit, TABLE_OP_PT_INFOt, &in_flds));
    }

exit:
    if (in_flds.field) {
        shr_famm_free(table_op_pt_info_arr_hdl[unit], in_flds.field,
                      TABLE_OP_PT_INFO_MAX_FIELD_LEN);
    }
    if (out_flds.field) {
        shr_famm_free(table_op_pt_info_arr_hdl[unit], out_flds.field,
                      TABLE_OP_PT_INFO_MAX_FIELD_LEN);
    }
    SHR_FUNC_EXIT();
}

static int
bcmltm_pt_status_info_check(int unit, uint32_t lt_id)
{
    int rv;
    bcmltm_lt_state_t *lt_state = NULL;

    rv = bcmltm_state_lt_get(unit, lt_id, &lt_state);
    if (SHR_FAILURE(rv) || (lt_state == NULL)) {
        return FALSE;
    }

    return lt_state->table_op_pt_info_enable;
}

/*******************************************************************************
 * Public Functions
 */

int
bcmltm_table_op_pt_info_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!table_op_pt_info_arr_hdl[unit]) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(TABLE_OP_PT_INFO_MAX_FIELD_LEN,
                               &table_op_pt_info_arr_hdl[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_table_op_pt_info_cleanup(int unit)
{
    if (table_op_pt_info_arr_hdl[unit]) {
        shr_famm_hdl_delete(table_op_pt_info_arr_hdl[unit]);
        table_op_pt_info_arr_hdl[unit] = 0;
    }

    return SHR_E_NONE;
}

int
bcmltm_pt_status_mreq_keyed_lt(int unit, bcmltd_sid_t ltid,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_keyed_lt_mrsp_info_t *mrsp_info)
{
    bcmltm_table_op_pt_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_pt_status_info_check(unit, ltid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.aggregate_view = mrsp_info->aggregate_view;
    ptrm_status.pt_instance = pt_dyn_info->tbl_inst;
    ptrm_status.pt_sid_cnt = mrsp_info->rsp_entry_sid_cnt;
    sal_memcpy(ptrm_status.pt_sid, mrsp_info->rsp_entry_sid,
               sizeof(ptrm_status.pt_sid));
    ptrm_status.pt_index_cnt = mrsp_info->rsp_entry_index_cnt;
    sal_memcpy(ptrm_status.pt_index, mrsp_info->rsp_entry_index,
               sizeof(ptrm_status.pt_index));
    if (!ptrm_status.aggregate_view) {
        ptrm_status.pt_sid_data_cnt = mrsp_info->rsp_entry_sid_data_cnt;
        sal_memcpy(ptrm_status.pt_sid_data, mrsp_info->rsp_entry_sid_data,
                   sizeof(ptrm_status.pt_sid_data));
        ptrm_status.pt_index_data_cnt = mrsp_info->rsp_entry_index_data_cnt;
        sal_memcpy(ptrm_status.pt_index_data, mrsp_info->rsp_entry_index,
                   sizeof(ptrm_status.pt_index_data));
    }

    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_pt_status_mreq_indexed_lt(int unit, bcmltd_sid_t ltid,
                                 uint32_t ptid,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    bcmltm_table_op_pt_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_pt_status_info_check(unit, ltid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.aggregate_view = TRUE;
    ptrm_status.pt_sid_cnt = 1;
    ptrm_status.pt_sid[0] = ptid;
    ptrm_status.pt_index_cnt = 1;
    ptrm_status.pt_index[0] = pt_dyn_info->index;
    ptrm_status.pt_instance = pt_dyn_info->tbl_inst;

    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_pt_status_ireq_op(int unit, bcmltd_sid_t ltid,
                         uint32_t ptid,
                         bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmltm_pt_status_mreq_indexed_lt(unit, ltid,
                                                     ptid, pt_dyn_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_pt_status_ireq_hash_lt(int unit, bcmltd_sid_t ltid,
                              uint32_t ptid, uint32_t entry_index,
                              bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info)
{
    bcmltm_table_op_pt_info_t ptrm_status;

    SHR_FUNC_ENTER(unit);

    if (!bcmltm_pt_status_info_check(unit, ltid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    sal_memset(&ptrm_status, 0, sizeof(ptrm_status));

    ptrm_status.lt_id = ltid;
    ptrm_status.aggregate_view = TRUE;
    ptrm_status.pt_sid_cnt = 1;
    ptrm_status.pt_sid[0] = ptid;
    ptrm_status.pt_index_cnt = 1;
    ptrm_status.pt_index[0] = entry_index;
    ptrm_status.pt_instance = pt_dyn_hash_info->tbl_inst;

    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_update_data(unit, &ptrm_status));

exit:
    SHR_FUNC_EXIT();
}

