/*! \file bcmtrunk_lth_failover.c
 *
 * Purpose:     Handler implementation for TRUNK_FAILOVER.
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
#include <bcmtrunk/bcmtrunk_lth_failover.h>
#include <bcmtrunk/bcmtrunk_util.h>
#include <bcmtrunk/bcmtrunk_db.h>
#include <bcmtrunk/bcmtrunk_chips.h>

#include <bcmpc/bcmpc_lport.h>

/*******************************************************************************
 * Defines
 */
/* BSL Module. */
#define BSL_LOG_MODULE BSL_LS_BCMTRUNK_TABLE

/*******************************************************************************
 * Private variables
 */
/*! Failover bookkeeping HA structure. */
static bcmtrunk_failover_bk_t *failover_bk[BCMDRD_CONFIG_MAX_UNITS];

/*! Failover bookkeeping temporary structure. */
static bcmtrunk_failover_bk_t *failover_bk_temp[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK failover SW bitmap changed in one commit. */
SHR_BITDCL *bcmtrunk_failover_bitmap_diff[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to trunk group state. */
#define FAILOVER_HA(unit)                 failover_bk[unit]

/*! Pointer to trunk group temporary state. */
#define FAILOVER_TEMP(unit)               failover_bk_temp[unit]

/*! Pointer to trunk group temporary state. */
#define FAILOVER_DIFF(unit)               bcmtrunk_failover_bitmap_diff[unit]

#define BCMTRUNK_FAILOVER_INFO(_u_)  \
        (FAILOVER_TEMP(unit)->entry)
#define BCMTRUNK_FAILOVER_PTR(_u_, _idx_)  \
        (&(FAILOVER_TEMP(unit)->entry[_idx_]))
#define BCMTRUNK_FAILOVER_EXIST(_u_, _idx_)  \
        (BCMTRUNK_FAILOVER_PTR(_u_, _idx_)->set)
#define BCMTRUNK_FAILOVER_RTAG(_u_, _idx_)  \
        (BCMTRUNK_FAILOVER_PTR(_u_, _idx_)->rtag)
#define BCMTRUNK_FAILOVER_CNT(_u_, _idx_)  \
        (BCMTRUNK_FAILOVER_PTR(_u_, _idx_)->failover_cnt)


/*******************************************************************************
 * Private functions
 */
static int
bcmtrunk_failover_field_parse(int unit, const bcmltd_fields_t *in,
                              bcmtrunk_failover_param_t **param)
{
    uint32_t idx = 0, id;
    uint32_t arr_idx;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    if ((!in) || (!param)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "bcmtrunk_failover_field_parse:CNT=%d.\n"),
                    (uint32_t)(in->count)));

    (*param) = sal_alloc(sizeof(bcmtrunk_failover_param_t),
                         "bcmtrunk_group_param_t");
    if (!(*param)) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    sal_memset((*param), 0, sizeof(bcmtrunk_failover_param_t));

    for (idx = 0; idx < in->count; idx++) {
        id      = in->field[idx]->id;
        arr_idx = in->field[idx]->idx;
        fval    = in->field[idx]->data;
        switch (id) {
            case TRUNK_FAILOVERt_PORT_IDf:
                if (fval > (failover_bk[unit]->max_ports)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PORT);
                }
                (*param)->id = fval;
                (*param)->id_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FAILOVERt_PORT_IDf = %d.\n"),
                                fval));
                break;
            case TRUNK_FAILOVERt_RTAGf:
                (*param)->rtag = fval;
                (*param)->rtag_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FAILOVERt_RTAGf = %d.\n"),
                                fval));
                break;
            case TRUNK_FAILOVERt_FAILOVER_CNTf:
                if (fval > BCMTRUNK_FAILOVER_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "FAILOVER_CNTf = %d idx invalid.\n"),
                                    fval));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->failover_cnt = fval;
                (*param)->failover_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_FAILOVERt_FAILOVER_CNTf = %d.\n"),
                                fval));
                break;
            case TRUNK_FAILOVERt_FAILOVER_MODIDf:
                if (arr_idx >= BCMTRUNK_FAILOVER_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "FAILOVER_MODIDf[%d] idx invalid.\n"),
                                    fval));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->failover_modid[arr_idx] = fval;
                (*param)->failover_modid_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_FAILOVERt_FAILOVER_MODIDf[%d] = %d.\n"),
                     arr_idx, fval));
                break;
            case TRUNK_FAILOVERt_FAILOVER_MODPORTf:
                if (arr_idx >= BCMTRUNK_FAILOVER_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "FAILOVER_MODIDf[%d] idx invalid.\n"),
                                    fval));
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
                (*param)->failover_modport[arr_idx] = fval;
                (*param)->failover_modport_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_FAILOVERt_FAILOVER_MODPORTf[%d] = %d.\n"),
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
bcmtrunk_failover_sw_update(int unit, bcmtrunk_failover_param_t *param,
                            bcmtrunk_failover_t *entry)
{
    int px = 0;

    SHR_FUNC_ENTER(unit);

    if (param == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    entry->set = TRUE;

    if (param->rtag_valid) {
        entry->rtag = param->rtag;
    }
    if (param->failover_cnt_valid) {
        entry->failover_cnt = param->failover_cnt;
    }

    for (px = 0; px < BCMTRUNK_FAILOVER_MAX_MEMBERS; px++) {
        if (param->failover_modid_valid[px]) {
            entry->failover_modid[px] = param->failover_modid[px];
        }
        if (param->failover_modport_valid[px]) {
            entry->failover_modport[px] = param->failover_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtrunk_lth_failover_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg)
{
    bcmtrunk_failover_param_t *param = NULL;
    int port_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (in == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (!failover_bk[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(
       bcmtrunk_failover_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
       SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    port_id = param->id;

    switch (opcode) {
        case BCMLT_OPCODE_INSERT:
            if (BCMTRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
            }
            break;
       case BCMLT_OPCODE_LOOKUP:
            if (!BCMTRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
            break;
       case BCMLT_OPCODE_DELETE:
            if (!BCMTRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
            }
            break;
       case BCMLT_OPCODE_UPDATE:
            if (!BCMTRUNK_FAILOVER_EXIST(unit, port_id)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
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
bcmtrunk_lth_failover_insert(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    bcmtrunk_failover_param_t *param = NULL;
    int port_id;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_field_parse(unit, in, &param));

    port_id = param->id;
    if (param->failover_cnt > 0) {
        SHR_IF_ERR_EXIT(
            bcmpc_port_failover_enable_set(unit, port_id, TRUE));
    }
    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_add(unit, trans_id, TRUNK_FAILOVERt, param));


    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_sw_update(unit, param,
                                    BCMTRUNK_FAILOVER_PTR(unit, port_id)));
    SHR_BITSET(FAILOVER_DIFF(unit), port_id);

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmtrunk_lth_failover_lookup(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    int port_id;
    bcmtrunk_failover_param_t *param = NULL;
    int idx = 0, px;
    bcmtrunk_failover_t *entry = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    port_id = param->id;
    entry = BCMTRUNK_FAILOVER_PTR(unit, port_id);

    idx = 0;
    out->field[idx]->id   = TRUNK_FAILOVERt_RTAGf;
    out->field[idx]->data = entry->rtag;
    out->field[idx]->idx  = 0;
    idx++;

    out->field[idx]->id   = TRUNK_FAILOVERt_FAILOVER_CNTf;
    out->field[idx]->data = entry->failover_cnt;
    out->field[idx]->idx  = 0;
    idx++;

    /* All array member should be returned. */
    for (px = 0; px < BCMTRUNK_FAILOVER_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNK_FAILOVERt_FAILOVER_MODIDf;
        out->field[idx]->data = entry->failover_modid[px];
        out->field[idx]->idx  = px;
        idx++;
    }

    for (px = 0; px < BCMTRUNK_FAILOVER_MAX_MEMBERS; px++) {
        out->field[idx]->id   = TRUNK_FAILOVERt_FAILOVER_MODPORTf;
        out->field[idx]->data = entry->failover_modport[px];
        out->field[idx]->idx  = px;
        idx++;
    }

    out->count = idx;

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmtrunk_lth_failover_delete(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    bcmtrunk_failover_param_t *param = NULL;
    int port_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_field_parse(unit, in, &param));
    if (!(param->id_valid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    port_id = param->id;

    /* Failover entry is valid in HW when the cnt is larger than 0. */
    if (BCMTRUNK_FAILOVER_CNT(unit, port_id) > 0) {
        SHR_IF_ERR_EXIT(
            bcmpc_port_failover_enable_set(unit, port_id, FALSE));
    }
    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_del(unit, trans_id, TRUNK_FAILOVERt, param));

    sal_memset(BCMTRUNK_FAILOVER_PTR(unit, port_id),
               0, sizeof(bcmtrunk_failover_t));

    SHR_BITSET(FAILOVER_DIFF(unit), port_id);

exit:
   if (param != NULL) {
       sal_free(param);
       param = NULL;
   }
   SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_failover_update(int unit,
                             uint32_t trans_id,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    int port_id;
    bcmtrunk_failover_param_t *param = NULL;
    int old_cnt, cnt;


    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_field_parse(unit, in, &param));
    port_id = param->id;

    old_cnt = BCMTRUNK_FAILOVER_CNT(unit, port_id);
    if (param->failover_cnt_valid) {
        cnt = param->failover_cnt;
    } else {
        cnt = old_cnt;
    }

    if ((cnt == 0) && (old_cnt != 0)) {
            SHR_IF_ERR_EXIT(
                bcmpc_port_failover_enable_set(unit, port_id, FALSE));
    }
    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_update(unit, trans_id,
                                 TRUNK_FAILOVERt, param,
                                 BCMTRUNK_FAILOVER_PTR(unit, port_id)));

    SHR_IF_ERR_EXIT(
        bcmtrunk_failover_sw_update(unit, param,
                                    BCMTRUNK_FAILOVER_PTR(unit, port_id)));

    SHR_BITSET(FAILOVER_DIFF(unit), port_id);

exit:
    if (param != NULL) {
       sal_free(param);
       param = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_failover_first(int unit,
                            uint32_t trans_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t port_id, min, max;

    SHR_FUNC_ENTER(unit);

    if (!failover_bk[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtrunk_lth_field_value_range_get(unit, TRUNK_FAILOVERt,
                                            TRUNK_FAILOVERt_PORT_IDf,
                                            &min, &max));

    for (port_id = min; port_id < max; port_id++) {
        if (BCMTRUNK_FAILOVER_EXIST(unit, port_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FAILOVERt_PORT_IDf;
            flds.field[0]->data = port_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtrunk_lth_failover_lookup(unit, trans_id, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FAILOVERt_PORT_IDf;
            out->field[out->count]->data = port_id;
            out->count++;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_failover_next(int unit,
                           uint32_t trans_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t flds;
    bcmltd_field_t *flist, fld;
    uint64_t port_id, min, max;

    SHR_FUNC_ENTER(unit);

    if (!failover_bk[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtrunk_lth_field_value_range_get(unit, TRUNK_FAILOVERt,
                                            TRUNK_FAILOVERt_PORT_IDf,
                                            &min, &max));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtrunk_lth_field_get(unit, in, TRUNK_FAILOVERt_PORT_IDf, &port_id));

    port_id++;

    /* Stop traverse when exceeding the max trunk_id group entry. */
    for ( ; port_id < max; port_id++) {
        /* Exit the current iteration once we get a valid entry. */
        if (BCMTRUNK_FAILOVER_EXIST(unit, port_id)) {
            flist = &fld;
            flds.field = &flist;
            flds.field[0]->id = TRUNK_FAILOVERt_PORT_IDf;
            flds.field[0]->data = port_id;
            flds.count = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtrunk_lth_failover_lookup(unit, trans_id, &flds, out, arg));
            /* In lookup operation, we don't return key. */
            out->field[out->count]->id = TRUNK_FAILOVERt_PORT_IDf;
            out->field[out->count]->data = port_id;
            out->count++;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtrunk_lth_failover_commit
 *
 * \brief TRUNK_FAILOVER LT Commit function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  arg             Handler arguments.
 *
 * Commit software structures for TRUNK failover.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmtrunk_lth_failover_commit(int unit,
                             uint32_t trans_id,
                             const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!FAILOVER_HA(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    size = SHR_BITALLOCSIZE(FAILOVER_HA(unit)->max_ports);
    for (idx = 0; idx < FAILOVER_HA(unit)->max_ports; idx++) {
        if (SHR_BITGET(FAILOVER_DIFF(unit), idx)) {
            FAILOVER_HA(unit)->entry[idx] = FAILOVER_TEMP(unit)->entry[idx];
        }
    }

    sal_memset(FAILOVER_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmtrunk_lth_grp_abort
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
bcmtrunk_lth_failover_abort(int unit,
                            uint32_t trans_id,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t idx;
    uint32_t size;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    if (!FAILOVER_HA(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    size = SHR_BITALLOCSIZE(FAILOVER_HA(unit)->max_ports);
    for (idx = 0; idx < FAILOVER_HA(unit)->max_ports; idx++) {
        if (SHR_BITGET(FAILOVER_DIFF(unit), idx)) {
            FAILOVER_TEMP(unit)->entry[idx] = FAILOVER_HA(unit)->entry[idx];
        }
    }

    sal_memset(FAILOVER_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_failover_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    if (!FAILOVER_HA(unit)) {
        alloc_size = sizeof(bcmtrunk_failover_bk_t);
        SHR_ALLOC(FAILOVER_HA(unit), alloc_size, "bcmtrunk_failover_bk_t");
        SHR_NULL_CHECK(FAILOVER_HA(unit), SHR_E_MEMORY);
        sal_memset(FAILOVER_HA(unit), 0, alloc_size);

        SHR_ALLOC(FAILOVER_TEMP(unit), alloc_size,
                  "bcmtrunk_failover_bk_t temp");
        SHR_NULL_CHECK(FAILOVER_TEMP(unit), SHR_E_MEMORY);
        sal_memset(FAILOVER_TEMP(unit), 0, alloc_size);

        SHR_IF_ERR_EXIT(
            bcmtrunk_failover_init(unit, FAILOVER_HA(unit)));
        FAILOVER_TEMP(unit)->max_ports = FAILOVER_HA(unit)->max_ports;

        /* HA structure. */
        ha_req_size = (FAILOVER_HA(unit)->max_ports) *
                       sizeof(bcmtrunk_failover_t);
        ha_alloc_size = ha_req_size;
        FAILOVER_HA(unit)->entry = shr_ha_mem_alloc(unit,
                                             BCMMGMT_TRUNK_COMP_ID,
                                             BCMTRUNK_FAILOVER_SUB_COMP_ID,
                                             &ha_alloc_size);
        SHR_NULL_CHECK(FAILOVER_HA(unit)->entry, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(FAILOVER_HA(unit)->entry, 0, ha_alloc_size);
        }

        /* Temporary structure. */
        alloc_size = (FAILOVER_TEMP(unit)->max_ports) *
                     sizeof(bcmtrunk_failover_t);
        SHR_ALLOC(FAILOVER_TEMP(unit)->entry, alloc_size,
                  "bcmtrunk failover info");
        SHR_NULL_CHECK(FAILOVER_TEMP(unit)->entry, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(FAILOVER_TEMP(unit)->entry, 0, alloc_size);
        } else {
            sal_memcpy(FAILOVER_TEMP(unit)->entry,
                       FAILOVER_HA(unit)->entry,
                       alloc_size);
        }

        /* TRUNK bitmap diff in one commit.*/
        alloc_size = SHR_BITALLOCSIZE(FAILOVER_HA(unit)->max_ports);
        SHR_ALLOC(FAILOVER_DIFF(unit), alloc_size, "failover bitmap diff");
        SHR_NULL_CHECK(FAILOVER_DIFF(unit), SHR_E_MEMORY);
        sal_memset(FAILOVER_DIFF(unit), 0, alloc_size);

    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmtrunk_lth_failover_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmtrunk_lth_failover_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (FAILOVER_HA(unit) != NULL) {
        if (FAILOVER_HA(unit)->entry != NULL) {
            shr_ha_mem_free(unit, FAILOVER_HA(unit)->entry);
            FAILOVER_HA(unit)->entry = NULL;
        }
        SHR_FREE(FAILOVER_HA(unit));
    }

    if (FAILOVER_TEMP(unit) != NULL) {
        SHR_FREE(FAILOVER_TEMP(unit)->entry);
    }
    SHR_FREE(FAILOVER_TEMP(unit));
    SHR_FREE(FAILOVER_DIFF(unit));

    SHR_FUNC_EXIT();
}

