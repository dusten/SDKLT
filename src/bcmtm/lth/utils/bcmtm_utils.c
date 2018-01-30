/*! \file bcmtm_utils.c
 *
 * bcmtm utility functions
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
 INCLUDES
 */
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmtm/bcmtm_utils_internal.h>

#define BSL_LOG_MODULE 1

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
                      uint64_t *min, uint64_t *max, uint64_t *def)
{
    if (width == 1) {
        *min = (uint64_t)field_data->min->is_true;
        *max = (uint64_t)field_data->max->is_true;
        *def = (uint64_t)field_data->def->is_true;
    } else if (width <= 8) {
        *min = (uint64_t)field_data->min->u8;
        *max = (uint64_t)field_data->max->u8;
        *def = (uint64_t)field_data->def->u8;
    } else if (width <= 16) {
        *min = (uint64_t)field_data->min->u16;
        *max = (uint64_t)field_data->max->u16;
        *def = (uint64_t)field_data->def->u8;
    } else if (width <= 32) {
        *min = (uint64_t)field_data->min->u32;
        *max = (uint64_t)field_data->max->u32;
        *def = (uint64_t)field_data->def->u8;
    } else {
        *min = (uint64_t)field_data->min->u64;
        *max = (uint64_t)field_data->max->u64;
        *def = (uint64_t)field_data->def->u8;
    }
}

/*******************************************************************************
* Public functions
 */
int
bcmtm_fval_range_check(uint64_t fval,
        const bcmlrd_field_data_t *fdata,
        uint32_t width)
{
    uint64_t min, max;
    if (width == 1) {
        min = (uint64_t)fdata->min->is_true;
        max = (uint64_t)fdata->max->is_true;
    } else if (width <= 8) {
        min = (uint64_t)fdata->min->u8;
        max = (uint64_t)fdata->max->u8;
    } else if (width <= 16) {
        min = (uint64_t)fdata->min->u16;
        max = (uint64_t)fdata->max->u16;
    } else if (width <= 32) {
        min = (uint64_t)fdata->min->u32;
        max = (uint64_t)fdata->max->u32;
    } else {
        min = (uint64_t)fdata->min->u64;
        max = (uint64_t)fdata->max->u64;
    }
    return (((fval >= min) && (fval <= max)) ? 1: 0);
}

int
bcmtm_lt_field_validate(int unit,
        bcmlt_opcode_t opcode,
        bcmlrd_sid_t sid,
        const bcmltd_fields_t *in)
{
    const bcmlrd_field_data_t *fdata;
    const bcmlrd_table_rep_t *tbl;
    bcmlrd_fid_t fid = 0;
    size_t count = 0;
    uint16_t input = 0;
    uint16_t key_field = 0;

    SHR_FUNC_ENTER(unit);
    tbl = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(tbl, SHR_E_PARAM);

    while (count < in->count) {
        fid = in->field[count]->id;
        input |= (1 << fid);
        SHR_IF_ERR_EXIT(bcmlrd_field_get(unit, sid, fid, &fdata));
        /*check for max and min values for the fields */
        if(!bcmtm_fval_range_check(in->field[count]->data,
                                   fdata,
                                   tbl->field[fid].width)) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        /* check for read only fields in "in data" for insert and update only */
        if ((opcode == BCMLT_OPCODE_INSERT)|| (opcode == BCMLT_OPCODE_UPDATE)){
            if (fdata->flags & BCMLRD_FIELD_F_READ_ONLY){
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
        count++;
    }
    /* verify whether all key fields are present */
    count = 0;
    while(count < tbl->fields) {
        if (tbl->field[count].flags & BCMLRD_FIELD_F_KEY) {
            key_field |= ( 1 << count);
        }
        count++;
    }
    if ((input & key_field) != key_field) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_opcode_validate(int unit,
        bcmlt_opcode_t opcode,
        int bmap_ret)
{
    SHR_FUNC_ENTER(unit);
    if ((opcode == BCMLT_OPCODE_INSERT) && (bmap_ret != 0)){
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    } else if ((opcode != BCMLT_OPCODE_INSERT) && (bmap_ret == 0)){
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_in_field_parser (int unit,
        const bcmltd_fields_t *in,
        bcmlrd_sid_t sid,
        size_t num_fid,
        uint32_t *in_data)
{
    size_t count = 0;
    uint32_t in_list_cnt = 0;
    const bcmlrd_field_data_t *field;

    SHR_FUNC_ENTER(unit);
    /*populate the values from in list */
    for (count = 0; count < num_fid; count++) {
        /* populate default value if any*/
        SHR_IF_ERR_EXIT(bcmlrd_field_get(unit, sid, count, &field));
        if (field->def->u64) {
            in_data[count] = field->def->u64;
        }

        if (in_list_cnt < in->count) {
            if (in->field[in_list_cnt]->id == count) {
                in_data[count] = in->field[in_list_cnt]->data;
                in_list_cnt++;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmtm_field_list_free(bcmltd_fields_t *flist)
{
    if (flist) {
        SHR_FREE(flist->field[0]);
        SHR_FREE(flist->field);
        flist = NULL;
    }
}

int
bcmtm_field_list_alloc(int unit,
        size_t num_fields,
        bcmltd_fields_t *flist)
{
    bcmltd_field_t *mem = NULL;
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);

    if (!num_fields) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    flist->count = num_fields;
    flist->field = NULL;
    SHR_ALLOC(flist->field, num_fields * sizeof(bcmltd_field_t *), "flist field");
    SHR_NULL_CHECK( flist->field, SHR_E_MEMORY);

    SHR_ALLOC(mem, num_fields * sizeof(bcmltd_field_t), "data field");
    SHR_NULL_CHECK( mem, SHR_E_MEMORY);
    sal_memset( mem, 0, (num_fields * sizeof(bcmltd_field_t)));

    for (count = 0; count < num_fields; count++) {
        flist->field[count] = mem + count;
        if (count > 0) {
            flist->field[count-1]->next = flist->field[count];
        }
    }
exit:
    if (SHR_FUNC_VAL_IS(SHR_E_MEMORY)) {
        SHR_FREE(flist->field);
    }
    SHR_FUNC_EXIT();
}


int
bcmtm_field_value_range_get(int unit, bcmlrd_sid_t sid, uint32_t fid,
                                uint64_t *min, uint64_t *max) {
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;
    uint64_t def;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    lth_field_data_to_u64(field_md->width, field_data, min, max, &def);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_field_default_value_get(int unit, bcmlrd_sid_t sid, uint32_t fid,
                                uint64_t *def)
{
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;
    uint64_t min, max;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_field_get(unit, sid, fid, &field_data);
    if (SHR_FAILURE(rv)) {
        SHR_RETURN_VAL_EXIT(rv);
    }
    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    lth_field_data_to_u64(field_md->width, field_data, &min, &max, def);

exit:
    SHR_FUNC_EXIT();
}


