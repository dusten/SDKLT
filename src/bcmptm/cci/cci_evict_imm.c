/*! \file cci_evict_imm.c
 *
 * CTR_xxx_CONTROL LTs interface to in-memory table
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
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_cci_evict_internal.h>

/*******************************************************************************
 * Local definitions
 */
/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CCI


/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Fill CTR control structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_entry_set(int unit,
                      bcmltd_sid_t sid,
                      uint32_t fid,
                      uint64_t fval,
                      ctr_control_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    switch (sid) {
        case CTR_ING_FLEX_POOL_CONTROLt:
            entry->pool = CTR_POOL_ING_FLEX;
            switch(fid) {
                case CTR_ING_FLEX_POOL_CONTROLt_CTR_ING_FLEX_POOL_IDf:
                    entry->pool_id = fval;
                    break;
                case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_MODEf:
                    entry->evict_mode = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_MODE;
                    break;
                case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf:
                    entry->evict_thd_bytes = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
                    break;
                case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf:
                    entry->evict_thd_pkts = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
                    break;
                case CTR_ING_FLEX_POOL_CONTROLt_EVICTION_SEEDf:
                    entry->evict_seed = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_SEED;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt:
            entry->pool = CTR_POOL_EGR_FLEX;
            switch(fid) {
                case CTR_EGR_FLEX_POOL_CONTROLt_CTR_EGR_FLEX_POOL_IDf:
                    entry->pool_id = fval;
                    break;
                case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_MODEf:
                    entry->evict_mode = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_MODE;
                    break;
                case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf:
                    entry->evict_thd_bytes = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
                    break;
                case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf:
                    entry->evict_thd_pkts = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
                    break;
                case CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_SEEDf:
                    entry->evict_seed = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_SEED;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case CTR_EGR_Q_CONTROLt:
            entry->pool = CTR_POOL_EGR_PERQ;
            switch(fid) {
                case CTR_EGR_Q_CONTROLt_EVICTION_MODEf:
                    entry->evict_mode = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_MODE;
                    break;
                case CTR_EGR_Q_CONTROLt_EVICTION_THD_BYTESf:
                    entry->evict_thd_bytes = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
                    break;
                case CTR_EGR_Q_CONTROLt_EVICTION_THD_PKTSf:
                    entry->evict_thd_pkts = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
                    break;
                case CTR_EGR_Q_CONTROLt_EVICTION_SEEDf:
                    entry->evict_seed = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_SEED;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case CTR_EGR_FP_CONTROLt:
            entry->pool = CTR_POOL_EGR_FP;
            switch(fid) {
                case CTR_EGR_FP_CONTROLt_EVICTION_MODEf:
                    entry->evict_mode = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_MODE;
                    break;
                case CTR_EGR_FP_CONTROLt_EVICTION_THD_BYTESf:
                    entry->evict_thd_bytes = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
                    break;
                case CTR_EGR_FP_CONTROLt_EVICTION_THD_PKTSf:
                    entry->evict_thd_pkts = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
                    break;
                case CTR_EGR_FP_CONTROLt_EVICTION_SEEDf:
                    entry->evict_seed = fval;
                    entry->update_flags |= CTR_UPDATE_F_E_SEED;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure according to the given LT field default value.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] update  evicit update field.
 * \param [out] def_val Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_contrl_lrd_fid_default_get(int unit,
                               bcmltd_sid_t sid,
                               ctr_evict_update_t update,
                               uint64_t *def_val)
{
    uint32_t num;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(def_val, SHR_E_PARAM);

    switch (sid) {
       case CTR_ING_FLEX_POOL_CONTROLt:
            switch (update) {
                case CTR_UPDATE_EVICT_MODE:
                    fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_MODEf;
                    break;
                case CTR_UPDATE_EVICT_THD_BYTES:
                    fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf;
                    break;
                case CTR_UPDATE_EVICT_THD_PKTS:
                    fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf;
                    break;
                case CTR_UPDATE_EVICT_SEED:
                    fid = CTR_ING_FLEX_POOL_CONTROLt_EVICTION_SEEDf;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case CTR_EGR_FLEX_POOL_CONTROLt:
            switch (update) {
                case CTR_UPDATE_EVICT_MODE:
                    fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_MODEf;
                    break;
                case CTR_UPDATE_EVICT_THD_BYTES:
                    fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_BYTESf;
                    break;
                case CTR_UPDATE_EVICT_THD_PKTS:
                    fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_THD_PKTSf;
                    break;
                case CTR_UPDATE_EVICT_SEED:
                    fid = CTR_EGR_FLEX_POOL_CONTROLt_EVICTION_SEEDf;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case CTR_EGR_Q_CONTROLt:
            switch (update) {
                case CTR_UPDATE_EVICT_MODE:
                    fid = CTR_EGR_Q_CONTROLt_EVICTION_MODEf;
                    break;
                case CTR_UPDATE_EVICT_THD_BYTES:
                    fid = CTR_EGR_Q_CONTROLt_EVICTION_THD_BYTESf;
                    break;
                case CTR_UPDATE_EVICT_THD_PKTS:
                    fid = CTR_EGR_Q_CONTROLt_EVICTION_THD_PKTSf;
                    break;
                case CTR_UPDATE_EVICT_SEED:
                    fid = CTR_EGR_Q_CONTROLt_EVICTION_SEEDf;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        case CTR_EGR_FP_CONTROLt:
            switch (update) {
                case CTR_UPDATE_EVICT_MODE:
                    fid = CTR_EGR_FP_CONTROLt_EVICTION_MODEf;
                    break;
                case CTR_UPDATE_EVICT_THD_BYTES:
                    fid = CTR_EGR_FP_CONTROLt_EVICTION_THD_BYTESf;
                    break;
                case CTR_UPDATE_EVICT_THD_PKTS:
                    fid = CTR_EGR_FP_CONTROLt_EVICTION_THD_PKTSf;
                    break;
                case CTR_UPDATE_EVICT_SEED:
                    fid = CTR_EGR_FP_CONTROLt_EVICTION_SEEDf;
                    break;
                default:
                    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT(
        bcmlrd_field_default_get(unit, sid, fid, 1, def_val, &num));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill non-updated CTR control parameter with default values
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_lt_field_fill_default(int unit,
                                  bcmltd_sid_t sid,
                                  ctr_control_entry_t *entry)
{
    uint64_t    def_val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if ((entry->update_flags & CTR_UPDATE_F_E_MODE) == 0) {
        SHR_IF_ERR_EXIT(
            ctr_contrl_lrd_fid_default_get(unit, sid,
                                           CTR_UPDATE_EVICT_MODE,
                                           &def_val));
        entry->evict_mode = (def_val >> 32) & 0xFFFFFFFF;
        entry->update_flags |= CTR_UPDATE_F_E_MODE;
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_THD_BYTES) == 0) {
        SHR_IF_ERR_EXIT(
            ctr_contrl_lrd_fid_default_get(unit, sid,
                                           CTR_UPDATE_EVICT_THD_BYTES,
                                           &def_val));
        entry->evict_thd_bytes = def_val;
        entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_THD_PKTS) == 0) {
        SHR_IF_ERR_EXIT(
            ctr_contrl_lrd_fid_default_get(unit, sid,
                                           CTR_UPDATE_EVICT_THD_PKTS,
                                           &def_val));
        entry->evict_thd_pkts = (def_val >> 32) & 0xFFFFFFFF;
        entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;
    }
    if ((entry->update_flags & CTR_UPDATE_F_E_SEED) == 0) {
        SHR_IF_ERR_EXIT(
            ctr_contrl_lrd_fid_default_get(unit, sid,
                                           CTR_UPDATE_EVICT_SEED,
                                           &def_val));
        entry->evict_seed = def_val;
        entry->update_flags |= CTR_UPDATE_F_E_SEED;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill CTR control structure with default values
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
ctr_control_lt_field_restore_default(int unit,
                                     bcmltd_sid_t sid,
                                     ctr_control_entry_t *entry)
{
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        ctr_contrl_lrd_fid_default_get(unit, sid,
                                       CTR_UPDATE_EVICT_MODE,
                                       &def_val));
    entry->evict_mode = (def_val >> 32) & 0xFFFFFFFF;
    entry->update_flags |= CTR_UPDATE_F_E_MODE;

    SHR_IF_ERR_EXIT(
        ctr_contrl_lrd_fid_default_get(unit, sid,
                                       CTR_UPDATE_EVICT_THD_BYTES,
                                       &def_val));
    entry->evict_thd_bytes = def_val;
    entry->update_flags |= CTR_UPDATE_F_E_THD_BYTES;

    SHR_IF_ERR_EXIT(
        ctr_contrl_lrd_fid_default_get(unit, sid,
                                       CTR_UPDATE_EVICT_THD_PKTS,
                                       &def_val));
    entry->evict_thd_pkts = (def_val >> 32) & 0xFFFFFFFF;
    entry->update_flags |= CTR_UPDATE_F_E_THD_PKTS;

    SHR_IF_ERR_EXIT(
        ctr_contrl_lrd_fid_default_get(unit, sid,
                                       CTR_UPDATE_EVICT_SEED,
                                       &def_val));
    entry->evict_seed = def_val;
    entry->update_flags |= CTR_UPDATE_F_E_SEED;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to ctr_pool_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry CTR control data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
ctr_control_lt_fields_parse(int unit,
                            bcmltd_sid_t sid,
                            const bcmltd_field_t *key_flds,
                            const bcmltd_field_t *data_flds,
                            ctr_control_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (ctr_control_entry_set(unit, sid, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (ctr_control_entry_set(unit, sid, fid, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_XXX_CONTROL IMM table change callback function for staging.
 *
 * Handle CTR_XXX_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id Transaction ID associated with this operation.
 * \param [in] event BCMIMM entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context Pointer to the context given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
ctr_control_imm_callback_stage(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  bcmimm_entry_event_t event,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data,
                                  void *context,
                                  bcmltd_fields_t *output_fields)
{
    ctr_control_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ctr_control_entry_t));
    entry.pool = CTR_INVALID_POOL;
    SHR_IF_ERR_VERBOSE_EXIT
        (ctr_control_lt_fields_parse(unit, sid, key, data, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT(
                ctr_control_lt_field_fill_default(unit, sid, &entry));
            SHR_IF_ERR_EXIT(
                bcmptm_cci_evict_entry_update(unit, sid, trans_id, &entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT(
                bcmptm_cci_evict_entry_update(unit, sid, trans_id, &entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT(
                ctr_control_lt_field_restore_default(unit, sid, &entry));
            SHR_IF_ERR_EXIT(
                bcmptm_cci_evict_entry_update(unit, sid, trans_id, &entry));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CCI Eviction In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to CTR_XXX_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t cci_eviction_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = ctr_control_imm_callback_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_cci_evict_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for CTR CONTROL LTs here.
     */
    rv = bcmlrd_map_get(unit, CTR_ING_FLEX_POOL_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 CTR_ING_FLEX_POOL_CONTROLt,
                                 &cci_eviction_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, CTR_EGR_FLEX_POOL_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 CTR_EGR_FLEX_POOL_CONTROLt,
                                 &cci_eviction_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, CTR_EGR_Q_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(unit,
                                CTR_EGR_Q_CONTROLt,
                                &cci_eviction_imm_callback,
                                NULL));
    }

    rv = bcmlrd_map_get(unit, CTR_EGR_FP_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(unit,
                                CTR_EGR_FP_CONTROLt,
                                &cci_eviction_imm_callback,
                                NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
