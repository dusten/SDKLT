/*! \file bcmtrunk_util.c
 *
 * bcmtrunk utility function
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
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm.h>
#include <bcmtrunk/bcmtrunk_types.h>
#include <bcmtrunk/bcmtrunk_util.h>

/*******************************************************************************
 * Defines
 */
/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMTRUNK_TABLE

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Convert the union value bcmlt_field_data_t to uint64_t.
 *
 * \param [in] width Field width in bits.
 * \param [in] field_data Field map data which includes the max and min field
 *                        value in bcmlt_field_data_t.
 * \param [out] min Minimum field value in uint64_t.
 * \param [out] max Maximum field value in uint64_t.
 */
static void
lth_field_data_to_u64(uint32_t width, const bcmlrd_field_data_t *field_data,
                      uint64_t *min, uint64_t *max)
{
    if (width == 1) {
        *min = (uint64_t)field_data->min->is_true;
        *max = (uint64_t)field_data->max->is_true;
    } else if (width <= 8) {
        *min = (uint64_t)field_data->min->u8;
        *max = (uint64_t)field_data->max->u8;
    } else if (width <= 16) {
        *min = (uint64_t)field_data->min->u16;
        *max = (uint64_t)field_data->max->u16;
    } else if (width <= 32) {
        *min = (uint64_t)field_data->min->u32;
        *max = (uint64_t)field_data->max->u32;
    } else {
        *min = (uint64_t)field_data->min->u64;
        *max = (uint64_t)field_data->max->u64;
    }
}

/*******************************************************************************
 * Public Functions
 */

int
bcmtrunk_hw_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                 bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_READ,
                                   NULL, rsp_entry_wsize, lt_id,
                                   trans_id, NULL, NULL,
                                   entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmtrunk_hw_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                   bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_pt_hw_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_READ,
                                   NULL, rsp_entry_wsize, lt_id,
                                   trans_id, NULL, NULL,
                                   entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_pt_hw_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                     bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = index;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_ireq_read(int unit, bcmltd_sid_t lt_id,
                   bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_ireq_read(unit, 0, pt_id, &pt_info,
                             NULL, rsp_entry_wsize, lt_id,
                             entry_data, &rsp_ltid, &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_ireq_write(int unit, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                              entry_data, lt_id, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

/*
 * Infields from LT API may just have partial fields of a LT.
 * This function is to update members of parameters.
 */
int
bcmtrunk_param_update(int unit, bcmtrunk_group_param_t *param,
                      bcmtrunk_group_t *grp)
{
    int px;

    SHR_FUNC_ENTER(unit);

    if ((!param) || (!grp)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (!param->lb_mode_valid) {
        param->lb_mode = grp->lb_mode;
    }
    if (!param->uc_rtag_valid) {
        param->uc_rtag = grp->uc_rtag;
    }
    if (!param->uc_max_members_valid) {
        param->uc_max_members = grp->uc_max_members;
    }
    if (!param->uc_agm_id_valid) {
        param->uc_agm_id = grp->uc_agm_id;
    }
    if (!param->uc_cnt_valid) {
        param->uc_cnt = grp->uc_cnt;
    }
    if (!param->nonuc_cnt_valid) {
        param->nonuc_cnt = grp->nonuc_cnt;
    }
    for (px = 0; px < param->uc_cnt; px++) {
        if (!param->uc_modid_valid[px]) {
            param->uc_modid[px] = grp->uc_modid[px];
        }
        if (!param->uc_modport_valid[px]) {
            param->uc_modport[px] = grp->uc_modport[px];
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        if (!param->nonuc_modid_valid[px]) {
            param->nonuc_modid[px] = grp->nonuc_modid[px];
        }
        if (!param->nonuc_modport_valid[px]) {
            param->nonuc_modport[px] = grp->nonuc_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Infields from LT API may just have partial fields of a LT.
 * This function is to update members of parameters.
 */
int
bcmtrunk_fast_param_update(int unit, bcmtrunk_fast_group_param_t *param,
                           bcmtrunk_fast_group_t *grp)
{
    int px;

    SHR_FUNC_ENTER(unit);

    if (param == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (!param->lb_mode_valid) {
        param->lb_mode = grp->lb_mode;
    }
    if (!param->uc_rtag_valid) {
        param->uc_rtag = grp->uc_rtag;
    }
    if (!param->uc_agm_id_valid) {
        param->uc_agm_id = grp->uc_agm_id;
    }
    if (!param->uc_cnt_valid) {
        param->uc_cnt = grp->uc_cnt;
    }
    if (!param->nonuc_cnt_valid) {
        param->nonuc_cnt = grp->nonuc_cnt;
    }
    for (px = 0; px < param->uc_cnt; px++) {
        if (!param->uc_modid_valid[px]) {
            param->uc_modid[px] = grp->uc_modid[px];
        }
        if (!param->uc_modport_valid[px]) {
            param->uc_modport[px] = grp->uc_modport[px];
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        if (!param->nonuc_modid_valid[px]) {
            param->nonuc_modid[px] = grp->nonuc_modid[px];
        }
        if (!param->nonuc_modport_valid[px]) {
            param->nonuc_modport[px] = grp->nonuc_modport[px];
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*
 * Infields from LT API may just have partial fields of a LT.
 * This function is to update members of parameters.
 */
int
bcmtrunk_failover_param_update(int unit, bcmtrunk_failover_param_t *param,
                               bcmtrunk_failover_t *entry)
{
    int px;

    SHR_FUNC_ENTER(unit);

    if ((!param) || (!entry)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (!param->rtag_valid) {
        param->rtag = entry->rtag;
    }
    if (!param->failover_cnt_valid) {
        param->failover_cnt = entry->failover_cnt;
    }
    for (px = 0; px < param->failover_cnt; px++) {
        if (!param->failover_modid_valid[px]) {
            param->failover_modid[px] = entry->failover_modid[px];
        }
        if (!param->failover_modport_valid[px]) {
            param->failover_modport[px] = entry->failover_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmtrunk_lth_field_value_range_get(int unit, uint32_t sid, uint32_t fid,
                                   uint64_t *min, uint64_t *max)
{
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    lth_field_data_to_u64(field_md->width, field_data, min, max);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_field_get(int unit, const bcmltd_fields_t *in, uint32_t fid,
                       uint64_t *fval)
{
    size_t i;

    SHR_FUNC_ENTER(unit);

    *fval = 0;

    for (i = 0; i < in->count; i++) {
        if (fid == in->field[i]->id) {
            *fval = in->field[i]->data;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_member_free(int unit, SHR_BITDCL *bmp, uint32_t base, int num_entries)
{
    SHR_FUNC_ENTER(unit);

    SHR_BITCLR_RANGE(bmp, base, num_entries);

    SHR_FUNC_EXIT();
}

int
bcmtrunk_member_base_get(int unit, SHR_BITDCL *bmp,
                         int tbl_size, int num_entries, uint32_t *base)
{
    int base_ptr, max_base_ptr;

    SHR_FUNC_ENTER(unit);

    base_ptr = 0;
    max_base_ptr = tbl_size - num_entries;

    for (; base_ptr <= max_base_ptr; base_ptr++) {
        /* Check if the contiguous region of TRUNK_MEMBERm table from
         * index base_ptr to (base_ptr + num_entries - 1) is free.
         */
        if (SHR_BITNULL_RANGE(bmp, base_ptr, num_entries)) {
            break;
        }
    }

    if (base_ptr > max_base_ptr) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

    *base = base_ptr;
    SHR_BITSET_RANGE(bmp, base_ptr, num_entries);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_max_members_update(int unit, SHR_BITDCL *bmp,
                            uint32_t tbl_size,
                            uint32_t old_max, uint32_t old_base,
                            uint32_t new_max, uint32_t *new_base_ptr)
{
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (new_max < old_max) {
        *new_base_ptr = old_base;
        SHR_BITCLR_RANGE(bmp, (old_base + new_max), (old_max - new_max));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Check if it is possible to increase max grp size in-place. */
    for (idx = old_base + old_max;
         (idx < (old_base + new_max)) && (idx <= tbl_size); idx++) {
        if (SHR_BITGET(bmp, idx)) {
            break;
        }
    }
    if (idx == (old_base + new_max)) {
        *new_base_ptr = old_base;
        SHR_BITSET_RANGE(bmp, (old_base + old_max), (new_max - old_max));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT
        (bcmtrunk_member_base_get(unit, bmp, tbl_size, new_max, new_base_ptr));

 exit:
    SHR_FUNC_EXIT();
}

