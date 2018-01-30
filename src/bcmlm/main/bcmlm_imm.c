/*! \file bcmlm_imm.c
 *
 * BCMLM interface to in-memory table.
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
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include "bcmlm_internal.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLM_IMM

/*! LM fields array lmm handler. */
static shr_famm_hdl_t lm_fld_arr_hdl;

/*! The maximum field count of LM logical tables */
#define LM_FIELD_COUNT_MAX 3

/*! Data structure to save the info of LM_CONTROLt entry */
typedef struct lm_cfg_s {

    /*! Bitmap of Fileds to be operated */
    SHR_BITDCLNAME(fbmp, BCMLM_FIELDS_MAX);

    /*! Start/stop Link manager thread. */
    bool enable;

    /*! Time between scans(us). */
    uint32_t interval;

} lm_cfg_t;

/*! Data structure to save the info of LM_PORT_CONTROLt entry */
typedef struct lm_port_cfg_s {

    /*! Bitmap of Fileds to be operated */
    SHR_BITDCLNAME(fbmp, BCMLM_FIELDS_MAX);

    /*! Logical port. */
    shr_port_t port;

    /*! Linkscan mode of logical port. */
    bcmlm_linkscan_mode_t linkscan_mode;

    /*! Link status of those ports with override mode. */
    bool override_link_state;

} lm_port_cfg_t;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief imm LM_CONTROLt notification input fields parsing.
 *
 * Parse imm LM_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] ltcfg Link manager port config data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
lm_cfg_lt_fields_parse(int unit,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       lm_cfg_t *ltcfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(ltcfg, 0, sizeof(*ltcfg));

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case LM_CONTROLt_SCAN_ENABLEf:
            ltcfg->enable = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        case LM_CONTROLt_SCAN_INTERVALf:
            ltcfg->interval = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_CONTROL IMM table change callback function for staging.
 *
 * Handle LM_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
lm_config_imm_stage_callback(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             bcmimm_entry_event_t event_reason,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             void *context,
                             bcmltd_fields_t *output_fields)
{
    lm_cfg_t cfg;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (lm_cfg_lt_fields_parse(unit, key, data, &cfg));

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (SHR_BITGET(cfg.fbmp, LM_CONTROLt_SCAN_INTERVALf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_interval_update(unit, cfg.interval));
        }

        if (SHR_BITGET(cfg.fbmp, LM_CONTROLt_SCAN_ENABLEf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_linkscan_enable(unit, cfg.enable));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlm_ctrl_linkscan_enable(unit, 0));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to LM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t lm_config_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = lm_config_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm LM_PORT_CONTROLt notification input fields parsing.
 *
 * Parse imm LM_PORT_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] ltcfg Link manager port config data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
lm_port_cfg_lt_fields_parse(int unit,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            lm_port_cfg_t *ltcfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(ltcfg, 0, sizeof(*ltcfg));

    /* Parse Key field */
    ltcfg->port = key->data;

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case LM_PORT_CONTROLt_LINKSCAN_MODEf:
            ltcfg->linkscan_mode = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        case LM_PORT_CONTROLt_OVERRIDE_LINK_STATEf:
            ltcfg->override_link_state = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_PORT_CONTROL IMM table change callback function for staging.
 *
 * Handle LM_PORT_CONTROL IMM logical table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
lm_port_config_imm_stage_callback(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data,
                                  void *context,
                                  bcmltd_fields_t *output_fields)
{
    lm_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (lm_port_cfg_lt_fields_parse(unit, key, data, &pcfg));

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (SHR_BITGET(pcfg.fbmp, LM_PORT_CONTROLt_LINKSCAN_MODEf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_linkscan_mode_update(unit,
                                                 pcfg.port,
                                                 pcfg.linkscan_mode));
        }

        if (SHR_BITGET(pcfg.fbmp, LM_PORT_CONTROLt_OVERRIDE_LINK_STATEf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_override_link_state_update(unit,
                                                       pcfg.port,
                                                       pcfg.override_link_state));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlm_ctrl_linkscan_mode_update(unit,
                                             pcfg.port,
                                             BCMLM_LINKSCAN_MODE_NONE));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_PORT_CONTROL IMM logical table validate callback function.
 *
 * Validate the field values of the LM_PORT_CONTROL logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
lm_port_config_imm_validate(int unit,
                            bcmltd_sid_t sid,
                            bcmimm_entry_event_t action,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Validate the port is already existing in Port Control */
    if (key->id != LM_PORT_CONTROLt_PORT_IDf) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlm_linkscan_port_validate(unit, key->data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_PORT_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to LM_PORT_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t lm_port_config_imm_callback = {

    /*! Validate function. */
    .validate = lm_port_config_imm_validate,

    /*! Staging function. */
    .stage = lm_port_config_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*******************************************************************************
 * Public Functions
 */

int
bcmlm_imm_db_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!lm_fld_arr_hdl) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(LM_FIELD_COUNT_MAX, &lm_fld_arr_hdl));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmlm_imm_db_cleanup(int unit)
{
    if (lm_fld_arr_hdl) {
        shr_famm_hdl_delete(lm_fld_arr_hdl);
        lm_fld_arr_hdl = 0;
    }
}

int
bcmlm_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for LM LTs here.
     */
    rv = bcmlrd_map_get(unit, LM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 LM_CONTROLt,
                                 &lm_config_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, LM_PORT_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 LM_PORT_CONTROLt,
                                 &lm_port_config_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_imm_link_state_insert(int unit, shr_port_t port, int link)
{
    bcmltd_fields_t in_flds;

    SHR_FUNC_ENTER(unit);

    in_flds.count = 2;
    in_flds.field = shr_famm_alloc(lm_fld_arr_hdl, 2);
    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = LM_LINK_STATEt_PORT_IDf;
    in_flds.field[0]->data = port;

    in_flds.field[1]->id = LM_LINK_STATEt_LINK_STATEf;
    in_flds.field[1]->data = link;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, LM_LINK_STATEt, &in_flds));

exit:
    shr_famm_free(lm_fld_arr_hdl, in_flds.field, in_flds.count);
    SHR_FUNC_EXIT();
}

int
bcmlm_imm_link_state_update(int unit, shr_port_t port, int link)
{
    bcmltd_fields_t in_flds;

    SHR_FUNC_ENTER(unit);

    in_flds.count = 2;
    in_flds.field = shr_famm_alloc(lm_fld_arr_hdl, 2);
    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = LM_LINK_STATEt_PORT_IDf;
    in_flds.field[0]->data = port;

    in_flds.field[1]->id = LM_LINK_STATEt_LINK_STATEf;
    in_flds.field[1]->data = link;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, true, LM_LINK_STATEt, &in_flds));

exit:
    shr_famm_free(lm_fld_arr_hdl, in_flds.field, in_flds.count);
    SHR_FUNC_EXIT();
}

int
bcmlm_imm_link_state_delete(int unit, shr_port_t port)
{
    bcmltd_fields_t in_flds;

    SHR_FUNC_ENTER(unit);

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(lm_fld_arr_hdl, 1);
    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = LM_LINK_STATEt_PORT_IDf;
    in_flds.field[0]->data = port;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_delete(unit, LM_LINK_STATEt, &in_flds));

exit:
    shr_famm_free(lm_fld_arr_hdl, in_flds.field, in_flds.count);
    SHR_FUNC_EXIT();
}

int
bcmlm_imm_link_state_clear(int unit)
{
    bcmltd_fields_t in_flds;
    bcmltd_fields_t out_flds;
    int i, rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in_flds, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_flds, 0, sizeof(bcmltd_fields_t));

    out_flds.count = LM_FIELD_COUNT_MAX;
    out_flds.field = shr_famm_alloc(lm_fld_arr_hdl, out_flds.count);
    if (out_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(lm_fld_arr_hdl, in_flds.count);
    if (in_flds.field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    in_flds.field[0]->id = LM_LINK_STATEt_PORT_IDf;

    SHR_IF_ERR_EXIT(bcmimm_entry_get_first(unit, LM_LINK_STATEt, &out_flds));

    do {
        for (i = 0; i <  LM_LINK_STATEt_FIELD_COUNT; i++) {
            if (out_flds.field[i]->id == LM_LINK_STATEt_PORT_IDf) {
                in_flds.field[0]->data = out_flds.field[i]->data;
            }
        }

        rv = bcmimm_entry_get_next(unit, LM_LINK_STATEt, &in_flds, &out_flds);

        /* Delete the entry */
        if (SHR_FAILURE(bcmimm_entry_delete(unit, LM_LINK_STATEt, &in_flds))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Failed to delete link state entry of "
                                  "port %"PRIu64" from IMM.\n"),
                       in_flds.field[0]->data));
        }
    } while (rv == SHR_E_NONE);

exit:
    if (in_flds.field) {
        shr_famm_free(lm_fld_arr_hdl, in_flds.field, 1);
    }
    if (out_flds.field) {
        shr_famm_free(lm_fld_arr_hdl, out_flds.field, LM_FIELD_COUNT_MAX);
    }
    SHR_FUNC_EXIT();
}
