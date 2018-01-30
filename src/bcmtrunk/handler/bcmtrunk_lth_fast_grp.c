/*! \file bcmtrunk_lth_fast_grp.c
 *
 * Purpose:     handler implementation for TRUNK_FAST LT.
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
 * Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmtrunk/bcmtrunk_types.h>
#include <bcmtrunk/bcmtrunk_lth_fast_grp.h>
#include <bcmtrunk/bcmtrunk_util.h>
#include <bcmtrunk/bcmtrunk_db.h>
#include <bcmtrunk/bcmtrunk_chips.h>

/*******************************************************************************
 * Private functions
 */
static bcmtrunk_lth_fgrp_bk_t *lth_fgrp_bk[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK bookkeeping temporary structure. */
static bcmtrunk_lth_fgrp_bk_t * lth_fgrp_bk_temp[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK fast group SW bitmap changed in one commit. */
SHR_BITDCL *bcmtrunk_fgrp_bitmap_diff[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to trunk fast group state. */
#define GRP_FAST_HA(unit)                 lth_fgrp_bk[unit]

/*! Pointer to trunk fast group temporary state. */
#define GRP_FAST_TEMP(unit)               lth_fgrp_bk_temp[unit]

/*! Pointer to trunk fast group temporary state. */
#define GRP_FAST_DIFF(unit)               bcmtrunk_fgrp_bitmap_diff[unit]


#define GRP_FAST_MAX_MEMBER(unit)         GRP_FAST_HA(unit)->max_members

#define BCMTRUNK_GRP_INFO(_u_) \
    (GRP_FAST_TEMP(unit)->grp)
#define BCMTRUNK_GRP(_u_, _idx_) \
    (&(GRP_FAST_TEMP(unit)->grp[_idx_]))
#define GRP_EXIST(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->inserted)
#define GRP_LB_MODE(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->lb_mode)
#define GRP_UC_RTAG(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_rtag)
#define GRP_MAX_MEMBERS(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_max_members)
#define GRP_BASE_PTR(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_base_ptr)
#define GRP_UC_AGM_ID(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_agm_id)
#define GRP_UC_MEMBER_CNT(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_cnt)
#define GRP_UC_MEMBER_MODID_PTR(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_modid)
#define GRP_UC_MEMBER_MODPORT_PTR(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_modport)
#define GRP_UC_MEMBER_MODID(_u_, _idx_, _px_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_modid[_px_])
#define GRP_UC_MEMBER_MODPORT(_u_, _idx_, _px_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->uc_modport[_px_])

#define GRP_NONUC_MEMBER_CNT(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->nonuc_cnt)
#define GRP_NONUC_MEMBER_MODID_PTR(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->nonuc_modid)
#define GRP_NONUC_MEMBER_MODPORT_PTR(_u_, _idx_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->nonuc_modport)
#define GRP_NONUC_MEMBER_MODID(_u_, _idx_, _px_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->nonuc_modid[_px_])
#define GRP_NONUC_MEMBER_MODPORT(_u_, _idx_, _px_)  \
    (BCMTRUNK_GRP(_u_, _idx_)->nonuc_modport[_px_])


/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMTRUNK_TABLE


static int
bcmtrunk_fast_field_parse(int unit, const bcmltd_fields_t *in,
                          bcmtrunk_fast_group_param_t **param)
{
    uint32_t idx = 0, id;
    uint32_t arr_idx, fval;

    SHR_FUNC_ENTER(unit);

    if ((in == NULL) || (param == NULL)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    (*param) = sal_alloc(sizeof(bcmtrunk_fast_group_param_t),
                         "bcmtrunk_fast_group_param_t");
    if ((*param) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    sal_memset((*param), 0, sizeof(bcmtrunk_fast_group_param_t));


    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "bcmtrunk_fast_field_parse:CNT=%d.\n"),
                    (uint32_t)(in->count)));

    for (idx = 0; idx < in->count; idx++) {
        id      = in->field[idx]->id;
        arr_idx = in->field[idx]->idx;
        fval    = in->field[idx]->data;
        switch (id) {
            case TRUNK_FASTt_TRUNK_IDf:
                (*param)->id = fval;
                (*param)->id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_TRUNK_IDf = %d.\n"), fval));
                break;
            case TRUNK_FASTt_LB_MODEf:
                (*param)->lb_mode = fval;
                (*param)->lb_mode_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_LB_MODEf = %d.\n"), fval));
                break;
            case TRUNK_FASTt_UC_RTAGf:
                (*param)->uc_rtag = fval;
                (*param)->uc_rtag_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_RTAGf = %d.\n"), fval));
                break;
            case TRUNK_FASTt_UC_MEMBER_CNTf:
                if (fval > GRP_FAST_MAX_MEMBER(unit)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_cnt = fval;
                (*param)->uc_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_MEMBER_CNTf = %d.\n"),
                                fval));
                break;
            case TRUNK_FASTt_UC_MEMBER_MODIDf:
                if (arr_idx >= BCMTRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "UC_MODPORT[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_modid[arr_idx] = fval;
                (*param)->uc_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_MODIDf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNK_FASTt_UC_MEMBER_MODPORTf:
                if (arr_idx >= BCMTRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "UC_MODPORT[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->uc_modport[arr_idx] = fval;
                (*param)->uc_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_MODPORTf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNK_FASTt_UC_AGM_IDf:
                (*param)->uc_agm_id = fval;
                (*param)->uc_agm_id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_UC_AGM_IDf = %d.\n"), fval));
                break;
             case TRUNK_FASTt_NONUC_MEMBER_CNTf:
                if (fval > GRP_FAST_MAX_MEMBER(unit)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_cnt = fval;
                (*param)->nonuc_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_NONUC_MEMBER_CNTf = %d.\n"),
                                fval));
                break;
            case TRUNK_FASTt_NONUC_MEMBER_MODIDf:
                if (arr_idx >= BCMTRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "NONUC_MODID[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_modid[arr_idx] = fval;
                (*param)->nonuc_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_NONUC_MODIDf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            case TRUNK_FASTt_NONUC_MEMBER_MODPORTf:
                if (arr_idx >= BCMTRUNK_FAST_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "NONUC_MODPORT[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->nonuc_modport[arr_idx] = fval;
                (*param)->nonuc_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FASTt_NONUC_MODPORTf[%d] = %d.\n"),
                                arr_idx, fval));
                break;
            default:
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                break;
        }
    }


exit:
    if (SHR_FUNC_ERR()) {
        if ((param != NULL) && ((*param) != NULL)) {
            sal_free((*param));
            (*param) = NULL;
        }
    }
    SHR_FUNC_EXIT();
}


static int
bcmtrunk_fast_grp_to_api(int unit,
                         bcmtrunk_fast_group_t *grp,
                         bcmltd_fields_t *out)
{
    uint32_t idx = 0, px;
    uint32_t num_actual = 0;
    uint64_t def_val = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if ((!grp) || (!out)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    out->field[idx]->id   = TRUNK_FASTt_LB_MODEf;
    out->field[idx]->data = grp->lb_mode;
    out->field[idx]->idx  = 0;
    idx++;

    /* Invalid map fields should not be returned. */
    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt, TRUNK_FASTt_UC_RTAGf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        out->field[idx]->id   = TRUNK_FASTt_UC_RTAGf;
        out->field[idx]->data = grp->uc_rtag;
        out->field[idx]->idx  = 0;
        idx++;
    }

    out->field[idx]->id   = TRUNK_FASTt_UC_MEMBER_CNTf;
    out->field[idx]->data = grp->uc_cnt;
    out->field[idx]->idx  = 0;
    idx++;

    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_UC_MEMBER_MODIDf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        /* All array member should be returned. */
        for (px = 0; px < BCMTRUNK_FAST_MAX_MEMBERS; px++) {
            out->field[idx]->id   = TRUNK_FASTt_UC_MEMBER_MODIDf;
            out->field[idx]->data = grp->uc_modid[px];
            out->field[idx]->idx  = px;
            idx++;
        }
    }
    for (px = 0; px < BCMTRUNK_FAST_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNK_FASTt_UC_MEMBER_MODPORTf;
        out->field[idx]->data = grp->uc_modport[px];
        out->field[idx]->idx  = px;
        idx++;
    }

    out->field[idx]->id   = TRUNK_FASTt_UC_AGM_IDf;
    out->field[idx]->data = grp->uc_agm_id;
    out->field[idx]->idx  = 0;
    idx++;

    /* nonuc member fields. */
    out->field[idx]->id   = TRUNK_FASTt_NONUC_MEMBER_CNTf;
    out->field[idx]->data = grp->nonuc_cnt;
    out->field[idx]->idx  = 0;
    idx++;

    /* Invalid map fields should not be returned. */
    rv = bcmlrd_field_default_get(unit, TRUNK_FASTt,
                                  TRUNK_FASTt_NONUC_MEMBER_MODIDf,
                                  1, &def_val, &num_actual);
    if (rv == SHR_E_NONE) {
        /* All array member should be returned. */
        for (px = 0; px < BCMTRUNK_FAST_MAX_MEMBERS; px++) {
            out->field[idx]->id   = TRUNK_FASTt_NONUC_MEMBER_MODIDf;
            out->field[idx]->data = grp->nonuc_modid[px];
            out->field[idx]->idx  = px;
            idx++;
        }
    }
    for (px = 0; px < BCMTRUNK_FAST_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNK_FASTt_NONUC_MEMBER_MODPORTf;
        out->field[idx]->data = grp->nonuc_modport[px];
        out->field[idx]->idx  = px;
        idx++;
    }

    out->count = idx;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmtrunk_fast_sw_update(int unit, bcmtrunk_fast_group_param_t *param,
                        bcmtrunk_fast_group_t *grp)
{
    int px = 0;

    SHR_FUNC_ENTER(unit);
    if (param == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    grp->inserted = TRUE;

    if (param->lb_mode_valid) {
        grp->lb_mode = param->lb_mode;
    }
    if (param->uc_rtag_valid) {
        grp->uc_rtag = param->uc_rtag;
    }
    if (param->uc_agm_id_valid) {
        grp->uc_agm_id = param->uc_agm_id;
    }

    if (param->uc_cnt_valid) {
        grp->uc_cnt = param->uc_cnt;
    }

    if (param->nonuc_cnt_valid) {
        grp->nonuc_cnt = param->nonuc_cnt;
    }

    for (px = 0; px < BCMTRUNK_FAST_MAX_MEMBERS; px++) {
        if (param->uc_modid_valid[px]) {
            grp->uc_modid[px] = param->uc_modid[px];
        }
        if (param->uc_modport_valid[px]) {
            grp->uc_modport[px] = param->uc_modport[px];
        }
    }
    for (px = 0; px < BCMTRUNK_FAST_MAX_MEMBERS; px++) {
        if (param->nonuc_modid_valid[px]) {
            grp->nonuc_modid[px] = param->nonuc_modid[px];
        }
        if (param->nonuc_modport_valid[px]) {
            grp->nonuc_modport[px] = param->nonuc_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmtrunk_lth_fast_grp_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    bcmtrunk_fast_group_param_t *param = NULL;
    int trunk_id, old_uc_cnt;
    int uc_cnt;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (in == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (!lth_fgrp_bk[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(
       bcmtrunk_fast_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
       SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;
    if ((trunk_id < 0) || (trunk_id >= BCMTRUNK_MAX_TRUNK)) {
       SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            if (GRP_EXIST(unit, trunk_id)) {
                SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
            }

            /* The member cnt must be bigger than 0. */
            if (param->uc_cnt > BCMTRUNK_FAST_MAX_MEMBERS) {
               return SHR_E_PARAM;
            }
            break;
        case BCMLT_OPCODE_LOOKUP:
            if (!GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMLT_OPCODE_DELETE:
            if (!GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMLT_OPCODE_UPDATE:
            if (!GRP_EXIST(unit, trunk_id)) {
               LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
            old_uc_cnt = GRP_UC_MEMBER_CNT(unit, trunk_id);
            if (param->uc_cnt_valid) {
                uc_cnt = param->uc_cnt;
            } else {
                uc_cnt = old_uc_cnt;
            }

            if (uc_cnt > BCMTRUNK_FAST_MAX_MEMBERS) {
               return SHR_E_PARAM;
            }
            break;
        case BCMLT_OPCODE_TRAVERSE:
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            break;
   }

exit:
   if (param != NULL) {
       sal_free(param);
       param = NULL;
   }
   SHR_FUNC_EXIT();

}

int
bcmtrunk_lth_fast_grp_insert(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmtrunk_fast_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
        bcmtrunk_fast_field_parse(unit, in, &param));
    trunk_id = param->id;

    SHR_IF_ERR_EXIT(
        bcmtrunk_fast_grp_add(unit, trans_id, TRUNK_FASTt, param));

    SHR_IF_ERR_EXIT(
        bcmtrunk_fast_sw_update(unit, param, BCMTRUNK_GRP(unit, trunk_id)));

    SHR_BITSET(GRP_FAST_DIFF(unit), trunk_id);

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmtrunk_lth_fast_grp_lookup(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmtrunk_fast_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
       bcmtrunk_fast_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;

    SHR_IF_ERR_EXIT(
       bcmtrunk_fast_grp_to_api(unit, BCMTRUNK_GRP(unit, trunk_id), out));

exit:
    if (param != NULL) {
        sal_free(param);
        param = NULL;
    }
    SHR_FUNC_EXIT();

}

int
bcmtrunk_lth_fast_grp_delete(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    bcmtrunk_fast_group_param_t *param = NULL;
    int trunk_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(bcmtrunk_fast_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    trunk_id = param->id;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmtrunk_fast_grp_del(unit, trans_id, TRUNK_FASTt,
                              param, BCMTRUNK_GRP(unit, trunk_id)));

    sal_memset(BCMTRUNK_GRP(unit, trunk_id), 0, sizeof(bcmtrunk_fast_group_t));

    SHR_BITSET(GRP_FAST_DIFF(unit), trunk_id);

exit:
   if (param != NULL) {
       sal_free(param);
       param = NULL;
   }
   SHR_FUNC_EXIT();
}


int
bcmtrunk_lth_fast_grp_update(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    int trunk_id;
    bcmtrunk_fast_group_param_t *param = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(
        bcmtrunk_fast_field_parse(unit, in, &param));

    trunk_id = param->id;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmtrunk_fast_grp_update(unit, trans_id, TRUNK_FASTt,
                                 param, BCMTRUNK_GRP(unit, trunk_id)));

    SHR_IF_ERR_EXIT(
        bcmtrunk_fast_sw_update(unit, param, BCMTRUNK_GRP(unit, trunk_id)));

    SHR_BITSET(GRP_FAST_DIFF(unit), trunk_id);

exit:
    if (param != NULL) {
        sal_free(param);
        param = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_fast_grp_first(int unit,
                            uint32_t trans_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t trunk_id, min, max;

    SHR_FUNC_ENTER(unit);

    if (!lth_fgrp_bk[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtrunk_lth_field_value_range_get(unit, TRUNK_FASTt,
                                            TRUNK_FASTt_TRUNK_IDf,
                                            &min, &max));

    for (trunk_id = min; trunk_id < max; trunk_id++) {
        if (GRP_EXIST(unit, trunk_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FASTt_TRUNK_IDf;
            flds.field[0]->data = trunk_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtrunk_lth_fast_grp_lookup(unit, trans_id, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FASTt_TRUNK_IDf;
            out->field[out->count]->data = trunk_id;
            out->count++;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_fast_grp_next(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t trunk_id, min, max;

    SHR_FUNC_ENTER(unit);

    if (!lth_fgrp_bk[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtrunk_lth_field_value_range_get(unit, TRUNK_FASTt,
                                            TRUNK_FASTt_TRUNK_IDf,
                                            &min, &max));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtrunk_lth_field_get(unit, in, TRUNK_FASTt_TRUNK_IDf, &trunk_id));

    trunk_id++;

    /* Stop traverse when exceeding the max trunk_id group entry. */
    for ( ; trunk_id <= max; trunk_id++) {
        /* Exit the current iteration once we get a valid entry. */
        if (GRP_EXIST(unit, trunk_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FASTt_TRUNK_IDf;
            flds.field[0]->data = trunk_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtrunk_lth_fast_grp_lookup(unit, trans_id, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FASTt_TRUNK_IDf;
            out->field[out->count]->data = trunk_id;
            out->count++;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtrunk_lth_grp_commit
 *
 * \brief TRUNK LT Commit function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  arg             Handler arguments.
 *
 * Commit software structures for TRUNK group.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmtrunk_lth_fast_grp_commit(int unit,
                        uint32_t trans_id,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMTRUNK_MAX_TRUNK);

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!GRP_FAST_HA(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < BCMTRUNK_MAX_TRUNK; idx++) {
        if (SHR_BITGET(GRP_FAST_DIFF(unit), idx)) {
            GRP_FAST_HA(unit)->grp[idx] = GRP_FAST_TEMP(unit)->grp[idx];
        }
    }

    sal_memset(GRP_FAST_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtrunk_lth_fast_grp_abort
 *
 * \brief TRUNK LT abort function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  arg             Handler arguments.
 *
 * Abort software structures for TRUNK groups.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmtrunk_lth_fast_grp_abort(int unit,
                            uint32_t trans_id,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMTRUNK_MAX_TRUNK);

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!GRP_FAST_HA(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < BCMTRUNK_MAX_TRUNK; idx++) {
        if (SHR_BITGET(GRP_FAST_DIFF(unit), idx)) {
            GRP_FAST_TEMP(unit)->grp[idx] = GRP_FAST_HA(unit)->grp[idx];
        }
    }

    sal_memset(GRP_FAST_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_fast_grp_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    if (!GRP_FAST_HA(unit)) {
        alloc_size = sizeof(bcmtrunk_lth_fgrp_bk_t);
        SHR_ALLOC(GRP_FAST_HA(unit), alloc_size, "bcmtrunk_lth_fgrp_bk_t");
        SHR_NULL_CHECK(GRP_FAST_HA(unit), SHR_E_MEMORY);
        sal_memset(GRP_FAST_HA(unit), 0, alloc_size);

        SHR_ALLOC(GRP_FAST_TEMP(unit), alloc_size,
                  "bcmtrunk_lth_fgrp_bk_t temp");
        SHR_NULL_CHECK(GRP_FAST_TEMP(unit), SHR_E_MEMORY);
        sal_memset(GRP_FAST_TEMP(unit), 0, alloc_size);


        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtrunk_fast_grp_init(unit, GRP_FAST_TEMP(unit)));
        GRP_FAST_HA(unit)->max_members = GRP_FAST_TEMP(unit)->max_members;

        /* HA structure. */
        ha_req_size = BCMTRUNK_MAX_TRUNK * sizeof(bcmtrunk_fast_group_t);
        ha_alloc_size = ha_req_size;
        GRP_FAST_HA(unit)->grp = shr_ha_mem_alloc(unit,
                                             BCMMGMT_TRUNK_COMP_ID,
                                             BCMTRUNK_FAST_GRP_SUB_COMP_ID,
                                             &ha_alloc_size);
        SHR_NULL_CHECK(GRP_FAST_HA(unit)->grp, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(GRP_FAST_HA(unit)->grp, 0, ha_alloc_size);
        }

        /* Temporary structure. */
        alloc_size = BCMTRUNK_MAX_TRUNK * sizeof(bcmtrunk_fast_group_t);
        SHR_ALLOC(GRP_FAST_TEMP(unit)->grp, alloc_size,
                  "bcmtrunk temp fast group info");
        SHR_NULL_CHECK(GRP_FAST_TEMP(unit)->grp, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(GRP_FAST_TEMP(unit)->grp, 0, alloc_size);
        } else {
            sal_memcpy(GRP_FAST_TEMP(unit)->grp,
                       GRP_FAST_HA(unit)->grp,
                       alloc_size);
        }

        /* TRUNK_FAST bitmap diff in one commit.*/
        alloc_size = SHR_BITALLOCSIZE(BCMTRUNK_MAX_TRUNK);
        SHR_ALLOC(GRP_FAST_DIFF(unit), alloc_size, "TRUNK_FAST bitmap diff");
        SHR_NULL_CHECK(GRP_FAST_DIFF(unit), SHR_E_MEMORY);
        sal_memset(GRP_FAST_DIFF(unit), 0, alloc_size);
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void) bcmtrunk_lth_fast_grp_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_fast_grp_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (GRP_FAST_HA(unit) != NULL) {
        if (GRP_FAST_HA(unit)->grp != NULL) {
            shr_ha_mem_free(unit, GRP_FAST_HA(unit)->grp);
            GRP_FAST_HA(unit)->grp = NULL;
        }
        SHR_FREE(GRP_FAST_HA(unit));
    }

    if (GRP_FAST_TEMP(unit) != NULL) {
        SHR_FREE(GRP_FAST_TEMP(unit)->grp);
    }
    SHR_FREE(GRP_FAST_TEMP(unit));
    SHR_FREE(GRP_FAST_DIFF(unit));

    SHR_FUNC_EXIT();
}

