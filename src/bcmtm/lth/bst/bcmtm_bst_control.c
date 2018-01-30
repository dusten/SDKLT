/*! \file bcmtm_bst_control.c
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
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd_config.h>
#include <bcmtm/bst/bcmtm_bst_control.h>
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>

/*******************************************************************************
 * Local definitions
 */
static
SHR_BITDCL bcmtm_bst_control_lt_insert[BCMDRD_CONFIG_MAX_UNITS];

#define BCMTM_BST_CONTROL_LT_INSERT(_u, _class)            \
    SHR_BITSET(&bcmtm_bst_control_lt_insert[_u], _class)

#define BCMTM_BST_CONTROL_LT_IS_SET(_u, _class)            \
    SHR_BITGET(&bcmtm_bst_control_lt_insert[_u], _class)

#define BCMTM_BST_CONTROL_LT_DELETE(_u, _class)            \
    SHR_BITCLR(&bcmtm_bst_control_lt_insert[_u], _class)

bcmtm_bst_drv_t *bcmtm_bst_pt_drv[BCMDRD_CONFIG_MAX_UNITS];



/*******************************************************************************
 * Public functions
 */
int
bcmtm_bst_control_validate (int unit,
            bcmlt_opcode_t opcode,
            const bcmltd_fields_t *in,
            const bcmltd_generic_arg_t *arg)
{
    bcmltd_sid_t sid = arg->sid;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(bcmtm_lt_field_validate(unit, opcode, sid, in));
exit:
    /* Return */
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_control_insert (int unit,
            uint32_t trans_id,
            const bcmltd_fields_t *in,
            const bcmltd_generic_arg_t *arg)
{
    uint32_t in_data[TM_BST_CONTROLt_FIELD_COUNT] = {0};
    size_t num_fid = 0;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    uint32_t ltm_buf[3] = {0};
    uint32_t fval;
    uint8_t class;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t ltid = arg->sid;
    int i = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT(bcmtm_in_field_parser(unit, in, ltid, num_fid, in_data));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, -1);
    class = in_data[TM_BST_CONTROLt_CLASSf];
    sid = BCMTM_BST_CONFIG_PT_SID(unit)[class];
    for(i = 0; i < BST_CONTROL_MAX; i++) {
        fval = 0;
        switch(i) {
            case BST_CONTROL_TRACKING_MODE:
                fval = in_data[TM_BST_CONTROLt_TRACKING_MODEf];
                break;
            case BST_CONTROL_CLEAR_ON_READ:
                fval = in_data[TM_BST_CONTROLt_CLEAR_ON_READf];
                break;
            case BST_CONTROL_SNAPSHOT:
                fval = in_data[TM_BST_CONTROLt_SNAPSHOTf];
                break;
        }
        fid = BCMTM_BST_CONFIG_PT_FID(unit)[class][i];
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltm_buf, &fval));
    }

    /* writing to hardware */
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, ltid,
                (void *)&pt_dyn_info, ltm_buf));

    BCMTM_BST_CONTROL_LT_INSERT(unit, class);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_control_lookup(int unit,
        uint32_t trans_id,
        const bcmltd_fields_t *in,
        bcmltd_fields_t *out,
        const bcmltd_generic_arg_t *arg)
{
    uint32_t i = 0;
    uint32_t ltm_buf[3] = {0};
    bcmltd_sid_t ltid = arg->sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint8_t class;
    bcmdrd_sid_t sid = 0;
    bcmdrd_fid_t fid = 0;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    while ( i < out->count) {
        out->field[i]->id = i;
        i++;
    }

    class = in->field[TM_BST_CONTROLt_CLASSf]->data;
    if (!BCMTM_BST_CONTROL_LT_IS_SET(unit, class)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, -1);
    sid = BCMTM_BST_CONFIG_PT_SID(unit)[class];

    /* retrieve the values from hardware */
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void *)&pt_dyn_info, ltm_buf));

    for(i = 0; i < BST_CONTROL_MAX; i++) {
        fid = BCMTM_BST_CONFIG_PT_FID(unit)[class][i];
        SHR_IF_ERR_EXIT(bcmtm_field_get(unit, sid, fid, ltm_buf, &fval));
        switch(i) {
            case BST_CONTROL_TRACKING_MODE:
                out->field[TM_BST_CONTROLt_TRACKING_MODEf]->data = fval;
                break;
            case BST_CONTROL_CLEAR_ON_READ:
                out->field[TM_BST_CONTROLt_CLEAR_ON_READf]->data = fval;
                break;
            case BST_CONTROL_SNAPSHOT:
                out->field[TM_BST_CONTROLt_SNAPSHOTf]->data = fval;
                break;
        }
    }
    out->field[TM_BST_CONTROLt_CLASSf]->data = class;

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_control_delete(int unit,
        uint32_t trans_id,
        const bcmltd_fields_t *in,
        const bcmltd_generic_arg_t *arg)
{
    uint8_t class;
    uint32_t ltm_buf[3] = {0};
    bcmdrd_sid_t sid = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    class = in->field[TM_BST_CONTROLt_CLASSf]->data;
    if (!BCMTM_BST_CONTROL_LT_IS_SET(unit, class)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, -1);
    sid = BCMTM_BST_CONFIG_PT_SID(unit)[class];

    /* writing to hardware */
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, sid, arg->sid,
                   (void *)&pt_dyn_info, ltm_buf));
    BCMTM_BST_CONTROL_LT_DELETE(unit, class);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_control_update(int unit,
        uint32_t trans_id,
        const bcmltd_fields_t *in,
        const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t out;
    size_t num_fields = 0;
    size_t count = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, arg->sid, &num_fields));

    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, num_fields, &out));

    SHR_IF_ERR_EXIT(
            bcmtm_bst_control_lookup(unit, trans_id, in, &out, arg));

    for (count = 0; count < in->count; count++) {
        out.field[in->field[count]->id]->data = in->field[count]->data;
    }

    SHR_IF_ERR_EXIT(bcmtm_bst_control_insert (unit, trans_id, &out, arg));

exit:
    bcmtm_field_list_free(&out);

    SHR_FUNC_EXIT();
}

int
bcmtm_bst_drv_set(int unit, bcmtm_bst_drv_t *drv)
{
    bcmtm_bst_pt_drv[unit] = drv;
    return SHR_E_NONE;
}

void
bcmtm_bst_drv_clear(int unit)
{
    bcmtm_bst_pt_drv[unit] = NULL;
}

int
bcmtm_bst_drv_get(int unit, bcmtm_bst_drv_t **drv)
{
    if (!drv) {
        return SHR_E_PARAM;
    }
    *drv = bcmtm_bst_pt_drv[unit];
    return SHR_E_NONE;
}
