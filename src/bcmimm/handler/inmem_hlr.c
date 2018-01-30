/*! \file inmem_hlr.c
 *
 * In memory table handler.
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
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/generated/front_e_ha.h>
#include "inmem_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

/*
 * Allow 3 bits for opcode (LS bits), followed by 29 bits for elements count.
 * The upper word is 32 bits for len buffer length
 */
#define BUFF_HDR_OPCODE(x) ((x) & 0x7)
#define BUFF_HDR_D_ELEM(x) (((x) >> 3) & 0x1FFFFFFF)

#define VALIDATE_FIELD_DATA(_f, _d) \
    ((_f)->width <= 64 &&      \
     (_d > (_f)->max_val || _d < (_f)->min_val))

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Finds a table that matches the sub parameter.
 *
 * This function searches the tables list associated with the unit for a
 * logical table that was created with the sub-component that matches the
 * input \c sub parameter.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] sub Is the sub-component that was used to create the logical
 * table HA block.
 * \param [out] idx Is the index in the bcmimm_tables_list of the matched table.
 *
 * \return Pointer to the table with matched sub or NULL when table not found.
 */
static table_info_t *tbl_sub_find(int unit, uint8_t sub, uint32_t *idx)
{
    table_info_t *tbl;
    uint32_t j;

    for (j = 0; j < bcmimm_tables_list[unit]->allocated; j++) {
        tbl = &bcmimm_tables_list[unit]->tbl[j];
        if (sub == tbl->sub) {
            if (idx) {
                *idx = j;
            }
            return tbl;
        }
    }
    return NULL;
}

/*!
 *\brief Maps fixed sub-module to fixed block address.
 *
 * This function maps sub (component) value to the address of a fixed HA block
 * that is associated with storing fixed transaction buffers.
 *
 * \param [in] blk_ctrl This is the block control associated with all the
 * fixed HA transaction blocks.
 * \param [in] sub Is the sub-component to match.
 *
 * \return Pointer to the fixed table with associated sub. This function
 * should not fail.
 */
static uint8_t *sub_to_blk(imm_ha_blk_t *blk_ctrl, uint8_t sub)
{
    uint8_t *blk = NULL;

    switch (sub) {
    case GENERIC_DATA_COMP_ID:
        blk = blk_ctrl->data_blk;
        break;
    case GENERIC_KEY_COMP_ID:
        blk = blk_ctrl->key_blk;
        break;
    case AUX_TBL_COMP_ID:
        blk = blk_ctrl->aux_blk;
        break;
    default:
        assert(0);
    }
    return blk;
}

/*!
 *\brief Retrieves buffers pointers from their corresponding HA pointers.
 *
 * This function retrieves the data and key buffers based on the HA pointers
 * provided as input parameters.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] d_ptr Is the HA data pointer to the data buffer for the entry.
 * \param [in] d_ptr Is the HA key pointer.
 * \param [out] k_buf Is a pointer that will be set to point to the buffer
 * of the key for the entry.
 *
 * \return Pointer to the data buffer when success and NULL otherwise.
 */
static uint8_t *buffers_retrieve(int unit,
                                 uint32_t d_ptr,
                                 uint32_t k_ptr,
                                 uint8_t **k_buf)
{
    uint8_t *d_buf;
    uint8_t sub = BCMIMM_PTR_SUB_INFO(d_ptr);
    uint32_t offset = BCMIMM_PTR_OFFSET_INFO(d_ptr);
    uint8_t *blk = sub_to_blk(bcmimm_tables_list[unit], sub);

    d_buf = blk + offset;
    /* Now get the key blk */
    sub = BCMIMM_PTR_SUB_INFO(k_ptr);
    offset = BCMIMM_PTR_OFFSET_INFO(k_ptr);
    blk = sub_to_blk(bcmimm_tables_list[unit], sub);
    *k_buf = blk + offset;
    return d_buf;
}

/*!
 *\brief Seperate the input fields to key and data field lists.
 *
 * This function constructs key and data field lists from the input
 * that contains all the fields.
 *
 * \param [in] in Is the input parameter containing the key and data fields
 * mixed together.
 * \param [out] out_k_fields Is the list containing only key fields.
 * \param [out] out_d_fields Is the list containing only data fields.
 * \param [in] tbl Is the in-memory logical table associated with the data.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int build_field_lists(const bcmltd_fields_t *in,
                             bcmltd_field_t **out_k_fields,
                             bcmltd_field_t **out_d_fields,
                             table_info_t *tbl)
{
    bcmltd_field_t *k_field = NULL;    /* Last key field */
    bcmltd_field_t *d_field = NULL;    /* Last data field */
    bcmltd_field_t *gen_field;  /* Generic field (data or key) */
    bcmltd_field_t *output_list;
    const bcmltd_field_t *fld;
    bool is_key;
    size_t j;
    int key_count = 0;

    *out_k_fields = NULL;
    *out_d_fields = NULL;

    /* Each field will be added at the end of its relevant list */
    for (j = 0; j < in->count; j++) {
        fld = in->field[j];
        if (key_count < tbl->key_fld_cnt) {
            is_key = (bcmimm_is_key_field(tbl, fld->id) != -1);
        } else {
            is_key = false;
        }

        if (is_key) {
            key_count++;
            output_list = *out_k_fields;
        } else {
            output_list = *out_d_fields;
        }

        /* Allocate a new field */
        gen_field = shr_fmm_alloc();
        if (!gen_field) {
            bcmimm_internal_free_lists(*out_k_fields, *out_d_fields);
            return SHR_E_MEMORY;
        }

        /* Assign the field info */
        gen_field->data = fld->data;
        gen_field->id = fld->id;
        gen_field->idx = fld->idx;
        gen_field->flags = 0;
        gen_field->next = NULL;

        /* If the relevant list is empty */
        if (!output_list) {
            if (is_key) {
                k_field = gen_field;
                *out_k_fields = gen_field;
            } else {
                d_field = gen_field;
                *out_d_fields = gen_field;
            }
        } else {
            if (is_key) {
                k_field->next = gen_field;
                k_field = gen_field;
            } else {
                d_field->next = gen_field;
                d_field = gen_field;
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 *\brief Invokes validate function callback.
 *
 * This function construct the key and data field lists as input parameters
 * for the validation callback function.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] sid Is the LT ID.
 * \param [in] action Is the action associated with this entry.
 * \param [in] idx Is the index of the in-mem logical table.
 * \param [in] in Is the input fields to validate.
 * \param [in] tbl Is the LT informaiton structure.
 *
 * \return SHR_E_NONE if succesful and error code otherwise.
 */
static int invoke_validate_cb(int unit,
                              bcmltd_sid_t sid,
                              bcmimm_entry_event_t action,
                              uint32_t idx,
                              const bcmltd_fields_t *in,
                              table_info_t *tbl)
{
    bcmltd_field_t *out_k_fields; /* Key fields output list */
    bcmltd_field_t *out_d_fields; /* Data fields output list */
    bcmimm_lt_validate_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.validate;
    int rv;

    if (!cb_func) {
        return SHR_E_NONE;
    }

    rv = build_field_lists(in, &out_k_fields, &out_d_fields, tbl);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    rv = cb_func(unit, sid, action, out_k_fields,
                 out_d_fields, bcmimm_tables_cb[unit][idx].context);

    bcmimm_internal_free_lists(out_k_fields, out_d_fields);
    return rv;
}

/*!
 *\brief Validate input field value.
 *
 * This function compares the input field values to the min and max values
 * set for each field. Input field can be key or data field.
 *
 * \param [in] in Is the input fields to validate.
 * \param [in] tbl Is the LT informaiton structure.
 *
 * \return SHR_E_NONE if succesful and error code otherwise.
 */
static int validate_values(const bcmltd_fields_t *in,
                           table_info_t *tbl)
{
    field_info_t *fld;
    bcmltd_field_t *in_fld;
    size_t j;
    uint32_t fld_idx;
    bool found;

    for (j = 0; j < in->count; j++) {
        in_fld = in->field[j];
        found = false;
        /* Search the data fields */
        fld = tbl->data_fields;
        for (fld_idx = 0; fld_idx < tbl->data_fld_cnt; fld_idx++) {
            if (fld->fid == in_fld->id) {
                if (fld->read_only) {
                    return SHR_E_ACCESS;  /* Can't provide read-only field */
                }
                if (VALIDATE_FIELD_DATA(fld, in_fld->data)) {
                    return SHR_E_PARAM;
                }
                found = true;
                break;
            }
            fld++;
        }

        if (found) {
            continue;
        }
        /* search the key fields */
        fld = tbl->key_fields;
        for (fld_idx = 0; fld_idx < tbl->key_fld_cnt; fld_idx++) {
            if (fld->fid == in_fld->id) {
                if (VALIDATE_FIELD_DATA(fld, in_fld->data)) {
                    return SHR_E_PARAM;
                }
                break;
            }
            fld++;
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Call registered commit callback function.
 *
 * This function notifies a component that is registered to receive commit
 * events for a table.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] tbl This is the table being changed.
 * \param [in] idx Is the index in table in the table callback list. The
 * index is the index of the table in the master \c table_list[].
 * \param [in] trans_id Is the transaction ID associated with this callback.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int bcmimm_notify_commit(int unit,
                                table_info_t *tbl,
                                uint32_t idx,
                                uint32_t trans_id)
{
    bcmimm_lt_complete_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.commit;

    if (!cb_func) {
        return SHR_E_NONE;
    }

    /* Call the component callback function */
    return cb_func(unit, tbl->sid, trans_id,
                   bcmimm_tables_cb[unit][idx].context);
}

/*!
 * \brief Call registered abort callback function.
 *
 * This function notifies a component that is registered to receive abort
 * events for a table.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] tbl This is the table being changed.
 * \param [in] idx Is the index in table in the table callback list. The
 * index is the index of the table in the master \c table_list[].
 * \param [in] trans_id Is the transaction ID associated with this callback.
 *
 * \return None.
 */
static void bcmimm_notify_abort(int unit,
                               table_info_t *tbl,
                               uint32_t idx,
                               uint32_t trans_id)
{
    bcmimm_lt_abort_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.abort;

    if (!cb_func) {
        return;
    }

    /* Call the component callback abort function */
    cb_func(unit, tbl->sid, trans_id, bcmimm_tables_cb[unit][idx].context);
}

/*******************************************************************************
 * External functions local to this module
 */
int bcmimm_notify_stage(int unit,
                        const bcmltd_fields_t *in,
                        table_info_t *tbl,
                        uint32_t idx,
                        uint32_t trans_id,
                        bcmimm_entry_event_t reason,
                        bcmltd_fields_t *returned_fields)
{
    bcmltd_field_t *out_k_fields; /* Key fields output list */
    bcmltd_field_t *out_d_fields; /* Data fields output list */
    bcmimm_lt_stage_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.stage;
    int rv;

    if (!cb_func) {
        if (returned_fields) {
            returned_fields->count = 0;
        }
        return SHR_E_NONE;
    }

    if (build_field_lists(in, &out_k_fields,
                          &out_d_fields, tbl) != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }

    /* Call the component callback staging function */
    rv = cb_func(unit,
                 tbl->sid,
                 trans_id,
                 reason,
                 out_k_fields,
                 out_d_fields,
                 bcmimm_tables_cb[unit][idx].context,
                 returned_fields);

    /* Free the temporary fields */
    bcmimm_internal_free_lists(out_k_fields, out_d_fields);
    return rv;
}

uint32_t bcmimm_def_vals_fill(bcmltd_fields_t *out,
                              uint32_t start,
                              table_info_t *tbl)
{
    uint32_t left = out->count - start;
    int j;
    uint32_t k;
    field_info_t *fld;
    uint32_t fields_added = 0;
    bcmltd_field_t *out_fld;

    fld = tbl->data_fields;
    for (j = 0; (j < tbl->data_fld_cnt) && (left > 0); j++, fld++) {
        for (k = 0; k < start; k++) {
            if (out->field[k]->id == fld->fid) {
                break;
            }
        }
        if (k == start) {   /* Field was not in output */
            int idx;
            int size;
            /*
             * Wide fields should be treated as array elements but all indexes
             * should be reported.
             */
            if (BCMIMM_BITS_TO_BYTES(fld->width) <= sizeof(out_fld->data)) {
                size = 1;
            } else {
                size = (BCMIMM_BITS_TO_BYTES(fld->width) +
                        sizeof(out_fld->data) - 1) / sizeof(out_fld->data);
            }
            for (idx = 0; idx < size; idx++) {
                if (left == 0) {
                    break;
                }
                out_fld = out->field[start + fields_added];
                out_fld->data = fld->def_val;
                out_fld->id = fld->fid;
                out_fld->idx = idx;
                out_fld->next = NULL;
                left--;
                fields_added++;
            }
        }
    }
    return fields_added;
}

int bcmimm_is_key_field(table_info_t *tbl, uint32_t fid)
{
    int idx;
    field_info_t *key;

    for (idx = 0, key = tbl->key_fields;
          idx < (int)tbl->key_fld_cnt;
          idx++, key++) {
        if (key->fid == fid) {
            return idx;
        }
    }
    return -1;
}

table_info_t *bcmimm_tbl_find(int unit, uint32_t sid, uint32_t *idx)
{
    table_info_t *tbl;
    uint32_t j;

    if (!bcmimm_tables_list[unit]) {
        return NULL;
    }
    for (j = 0; j < bcmimm_tables_list[unit]->allocated; j++) {
        tbl = &bcmimm_tables_list[unit]->tbl[j];
        if (sid == tbl->sid) {
            if (idx) {
                *idx = j;
            }
            return tbl;
        }
    }
    return NULL;
}

void bcmimm_insert_key_fld(table_info_t *tbl,
                    uint8_t *key_buf,
                    int idx,
                    uint64_t data)
{
    uint32_t byte_offset;
    uint32_t bit_offset;
    uint32_t k;
    uint8_t *buf;
    uint32_t width; /* The width (in bits) of the key field */
    uint32_t bytes_width;
    uint8_t *data_ptr;

    /* The first key field is simple copy of the data field */
    if (idx == 0) {
        bytes_width = BCMIMM_BITS_TO_BYTES(tbl->key_fields[0].width);
        if (bcmimm_big_endian && (bytes_width < sizeof(data))) {
            data_ptr = (uint8_t *)&data;
            data_ptr += sizeof(data) - 1;
            for (k = 0; k < bytes_width; k++, data_ptr--) {
                key_buf[k] = *data_ptr;
            }
        } else {
            sal_memcpy(key_buf, &data, bytes_width);
        }
        return;
    }

    /* For second key field and above */
    bit_offset = 0;
    for (k = 0; k < (uint32_t)idx; k++) {
        bit_offset += tbl->key_fields[k].width;
    }
    width = tbl->key_fields[idx].width;
    byte_offset = bit_offset / 8;
    bit_offset &= 0x7;     /* Like mod 8 */
    buf = key_buf + byte_offset;
    if (bit_offset) {
        uint8_t mask = (1 << (8 - bit_offset)) - 1;
        if (bcmimm_big_endian) {
            data_ptr = (uint8_t *)&data;
            data_ptr += sizeof(data) - 1;
            *buf |= (*data_ptr & mask) << bit_offset;
        } else {
            *buf |= (data & mask) << bit_offset;
        }
        buf++;
        data >>= (8 - bit_offset);
        if (width > 8 - bit_offset) {
            width -= 8 - bit_offset;
        } else {
            return; /* Nothing left in the data to be copied */
        }
    }
    bytes_width = BCMIMM_BITS_TO_BYTES(width);
    if (bytes_width) {
        if (bcmimm_big_endian) {
            data_ptr = (uint8_t *)&data;
            data_ptr += sizeof(data)- 1;
            for (k = 0; k < bytes_width; k++, data_ptr--, buf++) {
                *buf = *data_ptr;
            }
        } else {
            sal_memcpy(buf, &data, bytes_width);
        }
    }
}

void bcmimm_extract_key_fld(table_info_t *tbl,
                     uint8_t *key_buf,
                     int idx,
                     bcmltd_field_t *fld)
{
    uint32_t byte_offset;
    uint32_t bit_offset;
    uint32_t k;
    uint8_t *buf;
    uint32_t width; /* The width (in bits) of the key field */
    uint32_t bytes_width;
    uint8_t *data_ptr;
    uint64_t data = 0;
    uint8_t mask;
    uint32_t final_bit_shift = 0;

    fld->idx = 0;
    fld->id = tbl->key_fields[idx].fid;
    fld->data = 0;
    bit_offset = 0;

    for (k = 0; k < (uint32_t)idx; k++) {
        bit_offset += tbl->key_fields[k].width;
    }
    width = tbl->key_fields[idx].width;
    byte_offset = bit_offset / 8;
    bit_offset &= 0x7;     /* Like mod 8 */
    buf = key_buf + byte_offset;

    if (bit_offset) {
        mask = (1 << (8 - bit_offset)) - 1;

        if (bcmimm_big_endian) {
            data_ptr = (uint8_t *)&fld->data;
            data_ptr += sizeof(fld->data) - 1;
            *data_ptr = (*buf >> bit_offset) & mask;
        } else {
            fld->data = (*buf >> bit_offset) & mask;
        }
        buf++;
        if (width > 8 - bit_offset) {
            width -= 8 - bit_offset;
        } else {    /* width < 8 */
            fld->data &= (1 << (width)) - 1;
            return; /* Nothing left in the data to be copied */
        }
        final_bit_shift = 8 - bit_offset;
    }

    bytes_width = BCMIMM_BITS_TO_BYTES(width);
    if (bytes_width) {
        if (bcmimm_big_endian) {
            data_ptr = (uint8_t *)&data;
            data_ptr += sizeof(data) - 1;
            for (k = 0; k < bytes_width; k++, data_ptr--) {
                *data_ptr = buf[k];
            }
        } else {
            sal_memcpy(&data, buf, bytes_width);
        }

        data &= ((uint64_t)1 << width) - 1;
        data <<= final_bit_shift;
        fld->data |= data;
    }
}

/*******************************************************************************
 * Public functions
 */
int bcmimm_basic_validate(int unit,
                          bcmlt_opcode_t opcode,
                          const bcmltd_fields_t *in,
                          const bcmltd_generic_arg_t *arg)
{
    table_info_t *tbl;
    const bcmltd_field_t *fld;
    size_t j;
    int idx;
    int rv;
    uint8_t key_buf[BCMIMM_MAX_KEY_SIZE];
    uint32_t key_fields;
    uint32_t tbl_idx;
    bcmimm_entry_event_t action;
    uint32_t lock_cnt;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, arg->sid, &tbl_idx);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (BCMIMM_BITS_TO_BYTES(tbl->key_arry_size) > BCMIMM_MAX_KEY_SIZE) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);   /* Key size too large */
    }

    SHR_IF_ERR_VERBOSE_EXIT(validate_values(in, tbl));

    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    sal_memset(key_buf, 0, sizeof(key_buf));
    /*
     * Build the key, if exist
     */
    key_fields = tbl->key_fld_cnt;
    for (j = 0; (j < in->count) && (key_fields > 0); j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, fld->id)) != -1) {
            bcmimm_insert_key_fld(tbl, key_buf, idx, fld->data);
            key_fields--;
        }
    }
    if (key_fields != 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    switch (opcode) {
    case BCMLT_OPCODE_DELETE:
    case BCMLT_OPCODE_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_be_get_lock_cnt(tbl->blk_hdl, key_buf, &lock_cnt));
        if (lock_cnt > 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
        }
        if (opcode == BCMLT_OPCODE_DELETE) {
            action = BCMIMM_ENTRY_DELETE;
        } else {
            action = BCMIMM_ENTRY_UPDATE;
        }
        break;
    case BCMLT_OPCODE_LOOKUP:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_be_get_lock_cnt(tbl->blk_hdl, key_buf, &lock_cnt));
        action = BCMIMM_ENTRY_LOOKUP;
        break;
    case BCMLT_OPCODE_INSERT:
        rv = bcmimm_be_get_lock_cnt(tbl->blk_hdl, key_buf, &lock_cnt);
        if (rv == SHR_E_NONE) {
            SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
        }
        action = BCMIMM_ENTRY_INSERT;
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        invoke_validate_cb(unit, arg->sid, action, tbl_idx, in, tbl));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_insert(int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_insert(unit,
                                   trans_id,
                                   arg->sid,
                                   in,
                                   BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_lookup(int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_lookup(unit,
                                   trans_id,
                                   arg->sid,
                                   in,
                                   out,
                                   BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_delete(int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_delete(unit,
                                   trans_id,
                                   arg->sid,
                                   in,
                                   BCMIMM_TABLE_NB));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_update(int unit,
                        uint32_t trans_id,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_update(unit,
                                   trans_id,
                                   arg->sid,
                                   in,
                                   BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_abort(int unit,
                        uint32_t trans_id,
                        const bcmltd_generic_arg_t *arg)
{
    imm_trans_info_t *blk_hdr = (imm_trans_info_t *)
            (bcmimm_tables_list[unit]->trans_blk + sizeof(generic_array_blk_t));
    imm_trans_elem_t *trans_elem;
    uint8_t *d_buf;
    uint8_t *k_buf;
    uint8_t opcode;
    table_info_t *tbl;
    uint64_t *d_hdr;
    generic_array_blk_t *aux_blk;
    uint32_t *fid;
    uint8_t *f_data;
    bcmimm_be_fields_t be_param;
    uint32_t idx;
    uint32_t elem_cnt = blk_hdr->elem_cnt;

    SHR_FUNC_ENTER(unit);

    if (blk_hdr->trans_id != trans_id) {
        /* Silently discard unknown transaction IDs */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    while (blk_hdr->elem_cnt > blk_hdr->start_idx) {
        trans_elem = &blk_hdr->elem[blk_hdr->elem_cnt-1];
        tbl = tbl_sub_find(unit, trans_elem->sub, &idx);
        SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

        bcmimm_notify_abort(unit, tbl, idx, trans_id);

        d_buf = buffers_retrieve(unit,
                                 trans_elem->data,
                                 trans_elem->key,
                                 &k_buf);

        /* data buffer might be NULL when HA is disabled */
        if (d_buf) {
            d_hdr = (uint64_t *)d_buf;
            d_buf += BUFF_HEADER_SIZE;
            opcode = BUFF_HDR_OPCODE(*d_hdr);
            be_param.count = BUFF_HDR_D_ELEM(*d_hdr);
            fid = (uint32_t *)d_buf;
            f_data = d_buf + BCMIMM_BE_FID_SIZE * be_param.count;
            switch (opcode) {
            case BCMLT_OPCODE_INSERT:
                /*
                 * Need to delete the entry based on the key we kept. No need to
                 * verify the return value as this can fail.
                 */
                bcmimm_be_entry_delete(tbl->blk_hdl, k_buf);
                break;
            case BCMLT_OPCODE_LOOKUP:
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                break;
            case BCMLT_OPCODE_DELETE:
                /* Restore the deleted entry. */
                be_param.fid = fid;
                be_param.fdata = f_data;
                SHR_IF_ERR_EXIT(
                    bcmimm_be_entry_insert(tbl->blk_hdl, k_buf, &be_param));
                break;
            case BCMLT_OPCODE_UPDATE:
                /*
                 * Restore the previous entry by deleting the last and
                 * inserting the previous one.
                 */
                bcmimm_be_entry_delete(tbl->blk_hdl, k_buf);
                be_param.fid = fid;
                be_param.fdata = f_data;
                SHR_IF_ERR_EXIT(
                    bcmimm_be_entry_insert(tbl->blk_hdl, k_buf, &be_param));

                break;
            default:
                assert(0);
                break;
            }
        }
        tbl->pending_ops--;
        blk_hdr->elem_cnt--;
    }
exit:
    /*
     * This loop just verifies that the tables are indeed cleaned from pending
     * activities. If everything works well, the pending_ops field should be
     * zero for every table in the transaction. However, if we crash during the
     * abort we should continue to execute the loop above. For this reason we
     * can't set the field pending_ops to zero above but only to decrement it.
     */
    while (elem_cnt > blk_hdr->start_idx) {
        trans_elem = &blk_hdr->elem[blk_hdr->start_idx];
        tbl = tbl_sub_find(unit, trans_elem->sub, &idx);
        SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);
        tbl->pending_ops = 0;
        blk_hdr->start_idx++;
    }
    blk_hdr->elem_cnt = 0;  /* All done, clear the table */
    blk_hdr->start_idx = 0;

    /* Clear the auxiliary block */
    aux_blk = (generic_array_blk_t *)bcmimm_tables_list[unit]->aux_blk;
    sal_mutex_take(aux_blk->syc_obj, SAL_MUTEX_FOREVER);
    aux_blk->free_offset = sizeof(generic_array_blk_t);
    sal_mutex_give(aux_blk->syc_obj);
    SHR_FUNC_EXIT();
}

int bcmimm_basic_commit(int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg)
{
    imm_trans_info_t *blk_hdr = (imm_trans_info_t *)
            (bcmimm_tables_list[unit]->trans_blk + sizeof(generic_array_blk_t));
    imm_trans_elem_t *trans_elem;
    table_info_t *tbl;
    generic_array_blk_t *aux_blk;
    uint32_t tbl_idx;

    SHR_FUNC_ENTER(unit);
    if (blk_hdr->trans_id != trans_id) {
        /* Silently discard unknown transaction IDs */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    while (blk_hdr->elem_cnt > blk_hdr->start_idx) {
        trans_elem = &blk_hdr->elem[blk_hdr->start_idx];
        tbl = tbl_sub_find(unit, trans_elem->sub, &tbl_idx);
        SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmimm_notify_commit(unit, tbl, tbl_idx, trans_id));
        tbl->pending_ops = 0;
        blk_hdr->start_idx++;
    }

    blk_hdr->elem_cnt = 0;  /* All done, clear the table */
    blk_hdr->start_idx = 0;

    /* Clear the auxiliary block */
    aux_blk = (generic_array_blk_t *)bcmimm_tables_list[unit]->aux_blk;
    sal_mutex_take(aux_blk->syc_obj, SAL_MUTEX_FOREVER);
    aux_blk->free_offset = sizeof(generic_array_blk_t);
    sal_mutex_give(aux_blk->syc_obj);
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_first(int unit,
                       uint32_t trans_id,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_get_first(unit,
                                      arg->sid,
                                      out,
                                      BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_next(int unit,
                      uint32_t trans_id,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_get_next(unit,
                                     arg->sid,
                                     in,
                                     out,
                                     BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}
