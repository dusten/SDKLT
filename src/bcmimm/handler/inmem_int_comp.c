/*! \file inmem_int_comp.c
 *
 *  In-memory inter component API
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

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <shr/shr_fmm.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/generated/front_e_ha.h>
#include "inmem_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND


static int setup_key(table_info_t *tbl,
                     uint8_t *key_buf,
                     const bcmltd_fields_t *in)
{
    int idx;
    const bcmltd_field_t *fld;
    field_info_t *k_field;
    size_t j;
    uint32_t fields_left = tbl->key_fld_cnt;

    for (j = 0; (j < in->count) && (fields_left > 0); j++) {
        fld = in->field[j];

        /* search for the index of the key field */
        for (idx = 0, k_field = tbl->key_fields;
              idx < (int)tbl->key_fld_cnt;
              idx++, k_field++) {
            if (k_field->fid == fld->id) {
                bcmimm_insert_key_fld(tbl, key_buf, idx, fld->data);
                fields_left--;
                break;
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Send table change notification to the notify component.
 *
 * This function prepares the parameters to send notification to upper
 * layers via the notify component.
 *
 * \param [in] unit Is the unit associated with the notification.
 * \param [in] opcode Indicates the operation associated with the notification.
 * \param [in] hi_pri Indicates if the notification should be high priority.
 * \param [in] table_id Is the LT ID.
 * \param [in] in Is the input fields of the modified entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int notify_change(int unit,
                         bcmlt_opcode_t opcode,
                         bool hi_pri,
                         uint32_t table_id,
                         const bcmltd_fields_t *in)
{
    size_t j;
    bcmltm_field_list_t *cur_field = NULL;
    bcmltm_field_list_t *gen_field = NULL;
    bcmltm_field_list_t *fields = NULL;
    bcmltd_field_t *in_fld;
    int rv;

    for (j = 0; j < in->count; j++) {
        in_fld = in->field[j];
        gen_field = shr_fmm_alloc();
        if (!gen_field) {
            bcmimm_internal_free_lists(NULL, fields);
            return SHR_E_MEMORY;
        }
        sal_memcpy(gen_field, in_fld, sizeof(*gen_field));
        if (!fields) {
            fields = gen_field;
            cur_field = gen_field;
        } else {
            cur_field->next = gen_field;
            cur_field = gen_field;
        }
    }
    gen_field->next = NULL;  /* Null terminate the list */

    /* Make the notification call */
    rv = bcmevm_table_notify(unit, hi_pri, table_id, opcode, fields);

    /* No need to free the field list. It will be free by the TRM */
    return rv;
}

int bcmimm_entry_lookup(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out)
{
    const bcmltd_field_t *fld;
    size_t j;
    table_info_t *tbl;
    uint8_t *f_data;
    uint8_t *key_buf;   /* Buffer to place the key */
    uint8_t *data_buf = NULL;  /* Buffer for output */
    int idx;
    uint32_t data_f_size;
    bcmimm_be_fields_t be_out;
    uint8_t key_field_count = 0;
    uint32_t key_size = 1;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);     /* Must have valid 'in' */
    if (out->count == 0 || !out->field) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /*
     * This function should not be blocked by the NB. Therefore the
     * implementation is done directly here instead of reusing the NB path.
     */
    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_PARAM);

    if (tbl->key_fld_cnt > 0) {
        /*
         * For non keyless tables perform sanity check on the input as
         * it should hold the key.
         */
        if (in->count == 0 || !in->field) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    /* Allocate the data buffer. Should be optimized to avoid malloc */
    data_buf = bcmimm_working_buff_alloc(unit);
    sal_memset(data_buf, 0, tbl->data_bits_len + key_size);

    key_buf = data_buf + tbl->data_bits_len;

    /* Build the key */
    for (j = 0; j < in->count && key_field_count < tbl->key_fld_cnt; j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, fld->id)) != -1) {
            bcmimm_insert_key_fld(tbl, key_buf, idx, fld->data);
            key_field_count++;
        }
    }

    data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;
    be_out.count = tbl->data_bits_len / data_f_size;
    f_data = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
    be_out.fid = (uint32_t *)data_buf;
    be_out.fdata = f_data;

    /* Get the data from the back-end */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_be_entry_lookup(tbl->blk_hdl,
                                key_buf,
                                &be_out));
    if (be_out.count > out->count) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

    /*
     * Extract the data fields.
     */
    bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    out->count = be_out.count;

exit:
    if (data_buf) {
        bcmimm_working_buff_free(unit);
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_update(int unit,
                        bool hi_pri,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in) {
    table_info_t *tbl;
    uint32_t data_f_size;   /* The size of a field entry (data+idx+id) */
    uint32_t data_fld_idx = 0;
    uint8_t *f_data;
    bool *f_del;
    bcmimm_be_fields_upd_t be_in;
    uint8_t *key_buf;
    uint32_t data_field_cnt;
    uint32_t key_size = 1;    /* Single byte for keyless tables */
    uint8_t *working_buf = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    /*
     * Field ID size if for the touple (field SID, idx) where idx may not
     * exist for tables without array.
     */
    data_f_size = tbl->f_data_size;
    data_field_cnt = in->count - tbl->key_fld_cnt;
    /* Check that the input is not too large */
    if (tbl->data_bits_len / (data_f_size  + BCMIMM_BE_FID_SIZE) <
            data_field_cnt) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    working_buf = bcmimm_working_buff_alloc(unit);

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    sal_memset(working_buf, 0, tbl->data_bits_len + key_size);

    f_data = working_buf + BCMIMM_BE_FID_SIZE * data_field_cnt;
    f_del = (bool *)(f_data + tbl->f_data_size * data_field_cnt);
    key_buf = ((uint8_t *)f_del) + data_field_cnt * sizeof(bool);

    /* Build the input to the back end in be_in. */
    be_in.fid = (uint32_t *)working_buf;
    be_in.del = f_del;
    be_in.fdata = f_data;

    /* Prepare the data buffer */
    data_fld_idx = bcmimm_compress_fields_into_buffer(in, tbl, key_buf,
                                                      f_data, working_buf);

    be_in.count = (size_t)data_fld_idx;

    SHR_IF_ERR_EXIT(bcmimm_be_entry_update(tbl->blk_hdl, key_buf, &be_in));

    /* Return the wrking buffer */
    bcmimm_working_buff_free(unit);

    SHR_IF_ERR_EXIT(notify_change(unit, BCMLT_OPCODE_UPDATE, hi_pri, sid, in));
exit:
    if (SHR_FUNC_ERR()) {
        if (working_buf) {
            bcmimm_working_buff_free(unit);
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_delete(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in) {
    table_info_t *tbl;
    size_t j;
    int idx;
    const bcmltd_field_t *fld;
    uint8_t *key_buf = NULL;
    uint32_t key_size = 1;   /* Single byte for keyless tables */

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    key_buf = bcmimm_working_buff_alloc(unit);

    sal_memset(key_buf, 0, key_size);

    /* Build the key */
    for (j = 0; j < in->count; j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, fld->id)) != -1) {
            bcmimm_insert_key_fld(tbl, key_buf, idx, fld->data);
        }
    }

    /* Delete the entry from the back-end */
    SHR_IF_ERR_EXIT(bcmimm_be_entry_delete(tbl->blk_hdl, key_buf));

    /* Return the wrking buffer */
    bcmimm_working_buff_free(unit);

    SHR_IF_ERR_EXIT(notify_change(unit, BCMLT_OPCODE_DELETE, false, sid, in));

exit:
    if (SHR_FUNC_ERR()) {
        if (key_buf) {
            bcmimm_working_buff_free(unit);
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_insert(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in)
{
    table_info_t *tbl;
    uint32_t data_f_size;
    uint8_t *f_data;
    bcmimm_be_fields_t be_in;
    uint32_t data_fld_idx = 0;
    uint32_t key_size = 1;   /* Single byte for keyless tables */
    uint8_t *key_buf;
    uint8_t *working_buf = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    if (in->count == 0 || in->field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    /*
     * Field ID size is for the 2-tuple (field SID, idx) where idx may not
     * exist for tables without array. This is another input validation.
     */
    data_f_size = tbl->f_data_size;
    /* Check that the input is not too large */
    if (tbl->data_bits_len / (data_f_size  + BCMIMM_BE_FID_SIZE) <
            in->count - tbl->key_fld_cnt) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    working_buf = bcmimm_working_buff_alloc(unit);

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    /* Clean up the working buffer */
    sal_memset(working_buf, 0, tbl->data_bits_len + key_size);

    key_buf = working_buf + tbl->data_bits_len;

    /* The data portion starts after the field ID tuple */
    f_data = working_buf + BCMIMM_BE_FID_SIZE * (in->count - tbl->key_fld_cnt);

    be_in.fid = (uint32_t *)working_buf;
    be_in.fdata = f_data;

    data_fld_idx = bcmimm_compress_fields_into_buffer(in, tbl, key_buf,
                                               f_data, working_buf);

    be_in.count = (size_t)data_fld_idx;
    SHR_IF_ERR_EXIT(
        bcmimm_be_entry_insert(tbl->blk_hdl,
                               key_buf,
                               &be_in));

    /* Return the wrking buffer */
    bcmimm_working_buff_free(unit);

    SHR_IF_ERR_EXIT(notify_change(unit, BCMLT_OPCODE_INSERT, false, sid, in));

exit:
    if (SHR_FUNC_ERR()) {
        if (working_buf) {
            bcmimm_working_buff_free(unit);
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_lock(int unit,
                      bcmltd_sid_t sid,
                      const bcmltd_fields_t *in) {
    table_info_t *tbl = NULL;
    uint8_t *key_buf;
    imm_ha_blk_t *blk_ctrl = bcmimm_tables_list[unit];
    uint32_t key_size;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_PARAM);

    key_buf = blk_ctrl->key_blk + tbl->key_bits;

    if (tbl->key_fld_cnt > 0) {
        /*
         * For non keyless tables perform sanity check on the input as
         * it should hold the key.
         */
        if (in->count == 0 || in->field == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    } else {
        key_size = 1;
    }
    sal_memset(key_buf, 0, key_size);
    if (tbl->key_fld_cnt > 0) {
        SHR_IF_ERR_EXIT(setup_key(tbl, key_buf, in));
    }
    SHR_IF_ERR_EXIT(bcmimm_be_entry_lock(tbl->blk_hdl, key_buf));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_entry_unlock(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in) {
    table_info_t *tbl = NULL;
    uint8_t *key_buf;
    imm_ha_blk_t *blk_ctrl = bcmimm_tables_list[unit];
    uint32_t key_size;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    if (in->count == 0 || in->field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_PARAM);

    key_buf = blk_ctrl->key_blk + tbl->key_bits;

    if (tbl->key_fld_cnt > 0) {
        /*
         * For non keyless tables perform sanity check on the input as
         * it should hold the key.
         */
        if (in->count == 0 || in->field == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    } else {
        key_size = 1;
    }

    sal_memset(key_buf, 0, key_size);
    if (tbl->key_fld_cnt > 0) {
        SHR_IF_ERR_EXIT(setup_key(tbl, key_buf, in));
    }
    SHR_IF_ERR_EXIT(bcmimm_be_entry_unlock(tbl->blk_hdl, key_buf));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_entry_get_first(int unit,
                           bcmltd_sid_t sid,
                           bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    if (out->count == 0 || out->field == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_get_first(unit, sid, out, BCMIMM_TABLE_SB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_entry_get_next(int unit,
                          bcmltd_sid_t sid,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    if (in->count == 0 || out->count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (!in->field || !out->field) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_get_next(unit, sid, in, out, BCMIMM_TABLE_SB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_lt_event_reg(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_lt_cb_t *cb,
                        void *context)
{
    table_info_t *tbl;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (!cb) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    tbl = bcmimm_tbl_find(unit, sid, &idx);
    SHR_NULL_CHECK(tbl, SHR_E_PARAM);
    bcmimm_tables_cb[unit][idx].cb = *cb;
    bcmimm_tables_cb[unit][idx].context = context;
exit:
    SHR_FUNC_EXIT();
}
