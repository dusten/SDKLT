/*! \file inmem_internal.c
 *
 * In memory internal functions implementation.
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
#include <sal/sal_thread.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <shr/shr_debug.h>
#include <shr/shr_fmm.h>
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

#define BUFF_HDR_SET(x,len,d_elem,opcode) \
    (x) = (((uint64_t)(len)) << 32) | ((d_elem) << 3) | (uint8_t)opcode

#define BUFF_HDR_LEN(x) (((x) >> 32) & 0xFFFFFFFF)

#define ROUND_TO_WORD_SIZE(len) (((len) + 3) / 4) * 4

/*
 * The following definitions related to multiple processing paths. There are total
 * of two processing paths:
 * 1. Modeled path
 * 2. Interactive path.
 * Since each path being processed with its own thread there should be resources
 * allocated for each path. It is not a good idea to block one path for the
 * other, in particular blocking the modeled path for the interactive path
 * should be avoided.
 */
#define INTERACTIVE_IDX         1
#define MODELED_IDX             0
#define MAX_PROCESSING_PATH     2

/* This is the maximum number of read-only fields per entry */
#define MAX_READ_ONLY_FIELDS 5

static bcmltd_fields_t
    *read_only_resources[MAX_PROCESSING_PATH][BCMDRD_CONFIG_MAX_UNITS];

static bool ha_enabled;

bool bcmimm_big_endian;

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Updates the transaction block information.
 *
 * This function updates the HA dedicated to transaction information with
 * the new transaction information. This HA block tracks the table, the
 * key and data pointers for every element in the transaction. It is assumed
 * that all the elements in the same transaction are of the same unit.
 *
 * \param [in] unit Is the unit associated with this transaction entry.
 * \param [in] tbl Is the table associated with this transaction entry.
 * \param [in] d_ptr This is HA pointer pointing to the data portion of
 * the entry.
 * \param [in] k_ptr This is HA pointer pointing to the key portion of
 * the entry.
 * \param [in] trans_id Is the transaction identifier.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int update_trans_info(int unit,
                             table_info_t *tbl,
                             uint32_t d_ptr,
                             uint32_t k_ptr,
                             uint32_t trans_id)
{
    imm_trans_info_t *blk_hdr = (imm_trans_info_t *)
            (bcmimm_tables_list[unit]->trans_blk + sizeof(generic_array_blk_t));
    imm_trans_elem_t *trans_elem;

    if (blk_hdr->elem_cnt != 0) {
        if (blk_hdr->trans_id != trans_id) {
            return SHR_E_INTERNAL;
        }
        if (blk_hdr->elem_cnt >= BCMIMM_MAX_TRANS_SIZE) {
            return SHR_E_FULL;
        }
    } else {
        blk_hdr->trans_id = trans_id;
        blk_hdr->start_idx = 0;
        blk_hdr->unit = unit;
    }
    trans_elem = &blk_hdr->elem[blk_hdr->elem_cnt++];
    trans_elem->data = d_ptr;
    trans_elem->key = k_ptr;
    trans_elem->sub = tbl->sub;
    tbl->pending_ops++;
    return SHR_E_NONE;
}

/*!
 *\brief Extract entry keys from output BE buffers.
 *
 * This function extracts the key field out from the key buffer.

 * \param [in] key_buf Is the key buffer
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [out] out Is the output field array where the fields will be
 * extracted to.
 * \param [in] tot_count Is the current field total count. It is being
 * used as the index into the output field array.
 *
 * \return None.
 */
static void extract_key_into_flds(uint8_t *key_buf,
                                      table_info_t *tbl,
                                      bcmltd_fields_t *out,
                                      size_t tot_count) {
    uint32_t j;
    int idx;
    bcmltd_field_t *out_fld;

    for (j = 0, idx = 0;
         j < tbl->key_fld_cnt && tot_count < out->count;
         j++, idx++) {
        out_fld = out->field[tot_count++];
        bcmimm_extract_key_fld(tbl, key_buf, idx, out_fld);
    }
}

/*!
 *\brief Allocates data and key buffers.
 *
 * This function allocates the data and key buffers. The initial buffers
 * already associated with the table. However, for transaction with more than
 * a single entry the buffers will be allocated from the auxiliary block.
 * The buffer allocation doesn't specifies the exact buffer size required for
 * data (key has fixed size buffer derived from the table information). It is
 * therefore assumed that data should be the largest possible buffer
 * (determined during table initialization). The actual size will be set
 * once the function parsed the data.
 *
 * Since the auxiliary block being shared by all tables under the unit, it
 * is require to protect the block from multiple simultaneous access. In
 * particular between the step of buffer allocation and the time where the
 * allocation length is set (see \ref update_aux_blk()).
 * The function also calculates the HA pointers for these buffers.
 *
 * \param [in] unit Is the unit associated with the operation.
 * \param [in] tbl Is the table associated with the operation.
 * \param [out] d_ptr This is HA pointer pointing to the data portion of
 * the entry.
 * \param [out] k_ptr This is HA pointer pointing to the key portion of
 * the entry.
 * \param [out] k_buf This will point to the key buffer.
 * \param [out] aux Indicates if the buffers where allocated from the
 * auxiliary block or not.
 *
 * \return A pointer to the data buffer if success and NULL otherwise.
 */
static uint8_t *buffs_alloc(int unit,
                            table_info_t *tbl,
                            uint32_t *d_ptr,
                            uint32_t *k_ptr,
                            uint8_t **k_buf,
                            bool *aux)
{
    uint8_t *buf;
    imm_ha_blk_t *blk_ctrl = bcmimm_tables_list[unit];

    if (tbl->pending_ops == 0) {
        buf = blk_ctrl->data_blk + tbl->data_bits;
        BCMIMM_BUF_PTR(*d_ptr, GENERIC_DATA_COMP_ID, tbl->data_bits);
        *k_buf = blk_ctrl->key_blk + tbl->key_bits;
        BCMIMM_BUF_PTR(*k_ptr, GENERIC_KEY_COMP_ID, tbl->key_bits);
        *aux = false;
    } else {    /* Dynamic allocation */
        generic_array_blk_t *aux_blk;
        uint32_t len;
        uint32_t key_size = 1;

        if (tbl->key_fld_cnt) {
            key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
        }
        aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk;
        len = tbl->data_bits_len + BUFF_HEADER_SIZE;  /* Data buffer */
        len += key_size;   /* Add key buffer */
        len = ROUND_TO_WORD_SIZE(len);
        sal_mutex_take(aux_blk->syc_obj, SAL_MUTEX_FOREVER);
        if (aux_blk->blk_size - aux_blk->free_offset < len) {
            sal_mutex_give(aux_blk->syc_obj);
            return NULL;
        }
        *k_buf = blk_ctrl->aux_blk + aux_blk->free_offset;
        buf = *k_buf + key_size;
        BCMIMM_BUF_PTR(*k_ptr, AUX_TBL_COMP_ID, aux_blk->free_offset);
        *d_ptr = *k_ptr + key_size;
        *aux = true;
    }
    return buf;
}

/*!
 *\brief Free data and key buffers.
 *
 * This function frees previously allocated buffer.
 *
 * \param [in] unit Is the unit associated with the operation.
 * \param [in] tbl Is the table where the action taken place.
 * \param [in] d_len Is the data length portion of the buffer.
 * \param [in] k_len Is the key length portion of the buffer.
 * \param [in] aux Indicates if the buffer was allocated from the auxiliary
 * blocks or not.
 *
 * \return None.
 */
static void buffs_free(int unit,
                       table_info_t *tbl,
                       uint32_t d_len,
                       uint32_t k_len,
                       bool aux)
{
    imm_ha_blk_t *blk_ctrl = bcmimm_tables_list[unit];
    generic_array_blk_t *aux_blk;

    if (tbl->pending_ops > 0) {
        tbl->pending_ops--;
    }

    if (!aux) {
        return;   /* Nothing to do for dedicated memory */
    }
    aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk;
    aux_blk->free_offset -= ROUND_TO_WORD_SIZE(d_len + k_len);
}


/*!
 *\brief Cancel the buffer usage.
 *
 * This function being called after a buffer was allocated (using
 * \ref buffs_alloc()) but the buffer was never set using \ref buf_hdr_update().
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] tbl Is the table where the action taken place.
 * \param [in] aux Indicates if the buffer was allocated from the auxiliary
 * blocks or not.
 *
 * \return None.
 */

static void buffs_cancel(int unit, table_info_t *tbl, bool aux)
{
    imm_ha_blk_t *blk_ctrl = bcmimm_tables_list[unit];
    generic_array_blk_t *aux_blk;

    /*
     * Auxilury blocks are locked until the data length has been determined
     * and the value of free_offset has been set.
     * However, if we cancel then the free offset was never set. So we will
     * continue to use the old value, but we need to unlock the auxilury block
     * mutex.
     */
    if (aux) {
        aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk;
        sal_mutex_give(aux_blk->syc_obj);
    }
}

/*!
 *\brief Updates the buffers with final length information.
 *
 * This function will be called after a call to \ref buffs_alloc().
 * The function updates the first word of the data buffer to contain the
 * opcode and the length of the data buffer. If the buffers were allocated
 * from the auxiliary block, the function also updates the actual length
 * that was consumed and free the lock from the auxiliary block to be used
 * for another entries.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] data_buf is a pointer to the data buffer.
 * \param [in] d_len This is the actual used data length.
 * \param [in] k_len This is the actual key length.
 * \param [in] d_elem This is the number of data elements.
 * \param [in] opcode This is the opcode associated with this entry.
 * \param [in] aux This indicates if the auxiliary block was used.
 *
 * \return None.
 */
static void buf_hdr_update(int unit,
                           uint8_t *data_buf,
                           uint32_t d_len,
                           uint32_t k_len,
                           uint32_t d_elem,
                           uint8_t opcode,
                           bool aux)
{
    uint64_t *buf_hdr;
    imm_ha_blk_t *blk_ctrl = bcmimm_tables_list[unit];
    generic_array_blk_t *aux_blk;

    buf_hdr = (uint64_t *)(data_buf - BUFF_HEADER_SIZE);
    if (aux) {
        aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk;
        aux_blk->free_offset += ROUND_TO_WORD_SIZE(d_len + k_len);
        sal_mutex_give(aux_blk->syc_obj);
        BUFF_HDR_SET(*buf_hdr, d_len, d_elem, opcode);
    } else {
        BUFF_HDR_SET(*buf_hdr, d_len, d_elem, opcode);
    }
}


/*******************************************************************************
 * External functions local to this module
 */
void bcmimm_ha_enable_set(bool enable)
{
    ha_enabled = enable;
}

uint32_t bcmimm_compress_fields_into_buffer(const bcmltd_fields_t *in,
                                            table_info_t *tbl,
                                            uint8_t *key_buf,
                                            uint8_t *f_data,
                                            uint8_t *fid_buf)
{
    int idx;
    size_t j;
    const bcmltd_field_t *fld;
    uint32_t data_fld_idx = 0;
    uint32_t *fid = (uint32_t *)fid_buf;
    uint32_t data_f_size = tbl->f_data_size;

    for (j = 0; j < in->count; j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, fld->id)) != -1) {
            bcmimm_insert_key_fld(tbl, key_buf, idx, fld->data);
        } else {
            /*
             * Copy the data into the data array. The size of each data
             * element is data_f_size.
            */
            uint8_t *fld_const;

            fld_const = f_data + data_f_size * data_fld_idx;

            /* Copy the index touple (field ID, idx) where idx is LS portion */
            *fid = 0;
            if (tbl->array_idx_size > 0) {
                *fid = fld->idx;
            }
            *fid |= fld->id << (tbl->array_idx_size * 8);
            fid++;

            /* Now copy the data portion */
            if (bcmimm_big_endian && (tbl->f_data_size < sizeof(fld->data))) {
                uint8_t *data = (uint8_t *)&fld->data;

                data += sizeof(fld->data) - tbl->f_data_size;
                sal_memcpy(fld_const, data, tbl->f_data_size);
            } else {
                sal_memcpy(fld_const, &fld->data, tbl->f_data_size);
            }
            data_fld_idx++;
        }
    }
    return data_fld_idx;
}

int bcmimm_compress_ro_fields_into_buffer(bcmltd_fields_t *ro_fields,
                                          table_info_t *tbl,
                                          uint32_t idx,
                                          uint8_t *f_data,
                                          uint8_t *fid_buf)
{
    size_t j;
    uint32_t *fid = (uint32_t *)fid_buf;
    uint32_t data_f_size = tbl->f_data_size;
    bcmltd_field_t *fld;
    uint8_t *fld_const;

    fid += idx;
    for (j = 0; j < ro_fields->count; j++) {
        fld = ro_fields->field[j];
        /*
         * Copy the data into the data array. The size of each data
         * element is data_f_size.
        */
        fld_const = f_data + data_f_size * idx;

        /* Copy the index touple (field ID, idx) where idx is LS portion */
        *fid = 0;
        if (tbl->array_idx_size > 0) {
            *fid = fld->idx;
        }
        *fid |= fld->id << (tbl->array_idx_size * 8);
        fid++;

        /* Now copy the data portion */
        if (bcmimm_big_endian && (tbl->f_data_size < sizeof(fld->data))) {
            uint8_t *data = (uint8_t *)&fld->data;

            data += sizeof(fld->data) - tbl->f_data_size;
            sal_memcpy(fld_const, data, tbl->f_data_size);
        } else {
            sal_memcpy(fld_const, &fld->data, tbl->f_data_size);
        }
        idx++;
    }
    return (int)idx;
}

int bcmimm_extract_buf_into_flds(bcmimm_be_fields_t *be_out,
                                 table_info_t *tbl,
                                 bcmltd_fields_t *out)
{
    size_t j;
    bcmltd_field_t *out_fld;
    uint8_t *buf_ptr = be_out->fdata;
    uint32_t *fid = be_out->fid;
    uint32_t mask;
    int fld_cnt = 0;

    /* Extract all the data fields from the BE output */
    for (j = 0; j < be_out->count; j++) {
        out_fld = out->field[j];

        /* Extract the index and FID */
        if (tbl->array_idx_size > 0) {
            out_fld->id = (*fid) >> (tbl->array_idx_size * 8);
            mask = (1 << (tbl->array_idx_size * 8)) - 1;
            out_fld->idx = (*fid) & mask;
        } else {
            out_fld->id = *fid;
            out_fld->idx = 0;
        }
        fid++;

        /* Count the number of different field IDs */
        if (out_fld->idx == 0) {
            fld_cnt++;
        }

        out_fld->data = 0;  /* Make sure the data starts with 0 */
        /* Extract the data portion */
        if (bcmimm_big_endian && (tbl->f_data_size < sizeof(out_fld->data))) {
            uint8_t *data = (uint8_t *)&out_fld->data;

            data += sizeof(out_fld->data) - tbl->f_data_size;
            sal_memcpy(data, buf_ptr, tbl->f_data_size);
        } else {
            sal_memcpy(&out_fld->data, buf_ptr, tbl->f_data_size);
        }
        buf_ptr += tbl->f_data_size;
    }

    return fld_cnt;
}

int bcmimm_internal_init(void)
{
    uint32_t tmp = 1;
    uint8_t *tmp_str = (uint8_t *)&tmp;

    bcmimm_big_endian = (*tmp_str == 0);

    return SHR_E_NONE;
}

void bcmimm_internal_shutdown(void)
{
}

int bcmimm_internal_unit_init(int unit)
{
    int j, k;
    bcmltd_field_t *flds;

    SHR_FUNC_ENTER(unit);

    for (j = 0; j < MAX_PROCESSING_PATH; j++) {
        SHR_ALLOC(read_only_resources[j][unit],
                  sizeof(bcmltd_fields_t), "immRo");
        SHR_NULL_CHECK(read_only_resources[j][unit], SHR_E_MEMORY);

        read_only_resources[j][unit]->field = NULL;
        SHR_ALLOC(read_only_resources[j][unit]->field,
                  sizeof(bcmltd_field_t *) * MAX_READ_ONLY_FIELDS, "immRo");
        SHR_NULL_CHECK(read_only_resources[j][unit]->field, SHR_E_MEMORY);

        flds = NULL;
        SHR_ALLOC(flds,  sizeof(*flds) * MAX_READ_ONLY_FIELDS, "immRo");
        SHR_NULL_CHECK(flds, SHR_E_MEMORY);
        read_only_resources[j][unit]->count = MAX_READ_ONLY_FIELDS;
        for (k = 0; k < MAX_READ_ONLY_FIELDS; k++) {
            read_only_resources[j][unit]->field[k] = flds++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

void bcmimm_internal_unit_shutdown(int unit)
{
    int j;

    for (j = 0; j < MAX_PROCESSING_PATH; j++) {
        SHR_FREE(read_only_resources[j][unit]->field[0]);

        SHR_FREE(read_only_resources[j][unit]->field);

        SHR_FREE(read_only_resources[j][unit]);
    }
}

void bcmimm_internal_free_lists(bcmltd_field_t *k_fields,
                                bcmltd_field_t *d_fields)
{
    bcmltd_field_t *gen_field;  /* Generic field (data or key) */

    /* Free the key fields */
    while (k_fields) {
        gen_field = k_fields;
        k_fields = gen_field->next;
        shr_fmm_free(gen_field);
    }

    /* Free the data fields */
    while (d_fields) {
        gen_field = d_fields;
        d_fields = gen_field->next;
        shr_fmm_free(gen_field);
    }
}

int bcmimm_intern_entry_delete(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    size_t j;
    int idx;
    const bcmltd_field_t *fld;
    uint8_t *key_buf = NULL;
    uint8_t *data_buf = NULL;
    bool aux;
    uint32_t d_ptr;
    uint32_t k_ptr;
    uint32_t tbl_idx;
    uint32_t key_size = 1;   /* Single byte for keyless tables */
    uint8_t *f_data;
    uint32_t data_f_size;
    bcmimm_be_fields_t be_out;
    bool enable_ha;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    enable_ha = ha_enabled && !tbl->interactive;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_notify_stage(unit, in, tbl, tbl_idx,
                             trans_id, BCMIMM_ENTRY_DELETE, NULL));

    if (tbl->interactive) {
        SHR_ALLOC(key_buf, key_size, "bcmimmInter");
    } else {
        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;
    }

    sal_memset(key_buf, 0, key_size);

    /* Build the key */
    for (j = 0; j < in->count; j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, fld->id)) != -1) {
            bcmimm_insert_key_fld(tbl, key_buf, idx, fld->data);
        }
    }


    if (enable_ha) {
        data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;

        be_out.count = tbl->data_bits_len / data_f_size;
        f_data = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
        be_out.fid = (uint32_t *)data_buf;
        be_out.fdata = f_data;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_be_entry_lookup(tbl->blk_hdl, key_buf, &be_out));

        /*
         * Now we need to copy the output data from the working buff into the
         * HA buffer. We needed this since there was no way to determine how
         * many data fields will be in this entry.
         */
        be_out.fdata = data_buf + be_out.count * BCMIMM_BE_FID_SIZE;
        sal_memmove(be_out.fdata, f_data, be_out.count * tbl->f_data_size);

        buf_hdr_update(unit,
                       data_buf,
                       tbl->data_bits_len + BUFF_HEADER_SIZE,
                       key_size,
                       be_out.count,
                       BCMLT_OPCODE_DELETE,
                       aux);

        SHR_IF_ERR_EXIT(update_trans_info(unit, tbl, d_ptr, k_ptr, trans_id));
    }

    /* Delete the entry from the back-end */
    SHR_IF_ERR_EXIT(bcmimm_be_entry_delete(tbl->blk_hdl, key_buf));

    if (tbl->interactive) {
        SHR_FREE(key_buf);
    } else if (!enable_ha) {
        /* For the commit we need to add this entry to the transaction */
        SHR_IF_ERR_EXIT(update_trans_info(unit, tbl, 0, 0, trans_id));
        buffs_cancel(unit, tbl, aux);
    }
exit:
    /* In case of an error free ownership if owner is south bound */
    if (SHR_FUNC_ERR()) {
        if (tbl) {
            /* Clean up the buffer*/
            if (data_buf) {
                if (enable_ha) {
                    buffs_free(unit,
                               tbl,
                               tbl->data_bits_len + BUFF_HEADER_SIZE,
                               key_size,
                               aux);
                } else {
                    buffs_cancel(unit, tbl, aux);
                }
            } else {
                SHR_FREE(key_buf);
            }
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_lookup(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               bcmimm_tbl_owner_t owner)
{
    const bcmltd_field_t *fld;
    size_t j;
    table_info_t *tbl;
    uint8_t *f_data;
    bcmimm_be_fields_t be_out;
    uint32_t data_f_size;
    int idx;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    uint8_t key_field_count = 0;
    bool aux;
    uint32_t d_ptr;
    uint32_t k_ptr;
    int extr_fld_cnt;
    uint32_t key_size;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    } else {
        key_size = 1;  /* Single byte for keyless tables */
    }

    if (tbl->interactive) {
        SHR_ALLOC(data_buf, tbl->data_bits_len + key_size, "bcmimmInter");
        key_buf = data_buf + tbl->data_bits_len;
    } else {
        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;
    }

    sal_memset(key_buf, 0, key_size);
    sal_memset(data_buf, 0, tbl->data_bits_len);
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
     * Extract the data fields. Extracted field count (extr_fld_cnt)
     * is the number of different fields (i.e. different field ID)
     * that were extracted.
     * Note that be_out->count is the total number of fields that were
     * retrieved from the BE entry. This counts each array element.
     */
    extr_fld_cnt = bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    if (owner == BCMIMM_TABLE_NB) {
        if (extr_fld_cnt < tbl->data_fld_cnt) {
            out->count = be_out.count +
                bcmimm_def_vals_fill(out, be_out.count, tbl);
        } else {
            out->count = be_out.count;
        }
    } else {
        out->count = be_out.count;
    }

exit:
    if (data_buf) {
        if (tbl->interactive) {
            SHR_FREE(data_buf);
        } else {
            buffs_cancel(unit, tbl, aux);
        }
    }

    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_update(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    uint32_t data_f_size;   /* The size of a field entry (data+idx+id) */
    uint32_t data_fld_idx = 0;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    bool aux;
    uint32_t d_ptr;
    uint32_t k_ptr;
    uint8_t *f_data;
    bool *f_del;
    bcmimm_be_fields_upd_t be_in;
    bcmimm_be_fields_t be_out;
    uint32_t tbl_idx;
    uint32_t data_field_cnt;
    uint32_t key_size = 1;    /* Single byte for keyless tables */
    uint8_t *working_buf = NULL;
    uint32_t max_fields;
    bool enable_ha;
    bcmltd_fields_t *returned_fields;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
    }

    enable_ha = ha_enabled && !tbl->interactive;

    if (tbl->interactive) {
        returned_fields = read_only_resources[INTERACTIVE_IDX][unit];
    } else {
        returned_fields = read_only_resources[MODELED_IDX][unit];
    }
    returned_fields->count = MAX_READ_ONLY_FIELDS;

    /*
     * Field ID size if for the touple (field SID, idx) where idx may not
     * exist for tables without array.
     */
    data_f_size = tbl->f_data_size;
    data_field_cnt = in->count - tbl->key_fld_cnt;
    max_fields = tbl->data_bits_len /
         (data_f_size  + BCMIMM_BE_FID_SIZE + sizeof(bool));
    /* Check that the input is not too large */
    if (max_fields < data_field_cnt) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    /*
     * Notify the change before allocating the buffer. This allows the
     * notified component to read some values before we are taking the
     * working buffer.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_notify_stage(unit, in, tbl, tbl_idx,
                             trans_id, BCMIMM_ENTRY_UPDATE, returned_fields));
    if (returned_fields->count > 0) {
        data_field_cnt += returned_fields->count;
    }

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (tbl->interactive) {
        SHR_ALLOC(working_buf, tbl->data_bits_len + key_size, "bcmimmInter");
        key_buf = working_buf + tbl->data_bits_len;
    } else {
        working_buf = bcmimm_working_buff_alloc(unit);

        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;

        sal_memset(key_buf, 0, key_size);
        sal_memset(data_buf, 0, tbl->data_bits_len);
    }

    f_data = working_buf + BCMIMM_BE_FID_SIZE * data_field_cnt;
    f_del = (bool *)(f_data + tbl->f_data_size * data_field_cnt);

    /* Build the input to the back end in be_in. */
    be_in.fid = (uint32_t *)working_buf;
    be_in.del = f_del;
    be_in.fdata = f_data;

    /* Prepare the update data buffer */
    sal_memset(working_buf, 0, tbl->data_bits_len);
    data_fld_idx = bcmimm_compress_fields_into_buffer(in, tbl, key_buf,
                                                      f_data, working_buf);
    if (returned_fields->count > 0) {
        data_fld_idx = bcmimm_compress_ro_fields_into_buffer(returned_fields,
                                                             tbl,
                                                             data_fld_idx,
                                                             f_data,
                                                             working_buf);
    }

    be_in.count = (size_t)data_fld_idx;

    if (enable_ha) {
        /* Keep the current entry values in be_out using HA mem */
        be_out.fid = (uint32_t *)data_buf;
        be_out.fdata = data_buf + (BCMIMM_BE_FID_SIZE * max_fields);
        be_out.count = max_fields;

        /* Read the content of the current entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_be_entry_lookup(tbl->blk_hdl,
                                    key_buf,
                                    &be_out));

        /*
         * Now we need to copy the output data from the working buff into the
         * HA buffer. We needed this since there was no way to determine how
         * many data fields will be in this entry.
         */
        f_data = be_out.fdata;
        be_out.fdata = data_buf + be_out.count * BCMIMM_BE_FID_SIZE;
        sal_memmove(be_out.fdata, f_data, be_out.count * tbl->f_data_size);

        /* Now cache the old entry in case it is needed to recover from abort */
        buf_hdr_update(unit,
                       data_buf,
                       tbl->data_bits_len + BUFF_HEADER_SIZE,
                       key_size,
                       be_out.count,
                       BCMLT_OPCODE_UPDATE,
                       aux);

        /* Update the transaction information */
        SHR_IF_ERR_EXIT(update_trans_info(unit, tbl, d_ptr, k_ptr, trans_id));
    }

    SHR_IF_ERR_EXIT(bcmimm_be_entry_update(tbl->blk_hdl, key_buf, &be_in));

    /* Return the working buffer */
    if (tbl->interactive) {
        SHR_FREE(working_buf);
    } else {
        bcmimm_working_buff_free(unit);
        if (!ha_enabled) {
            /* For the commit we need to add this entry to the transaction */
            SHR_IF_ERR_EXIT(update_trans_info(unit, tbl, 0, 0, trans_id));
            buffs_cancel(unit, tbl, aux);
        }
    }

exit:
    /* In case of an error free ownership if owner is south bound */
    if (SHR_FUNC_ERR()) {
        if (tbl) {
            /* Clean up the buffer*/
            if (data_buf) {
                if (enable_ha) {
                    buffs_free(unit,
                               tbl,
                               tbl->data_bits_len + BUFF_HEADER_SIZE,
                               key_size,
                               aux);
                } else {
                    buffs_cancel(unit, tbl, aux);
                }
            }

            if (working_buf) {
                if (tbl->interactive) {
                    SHR_FREE(working_buf);
                } else {
                    bcmimm_working_buff_free(unit);
                }
            }
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_insert(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    uint32_t data_f_size;
    uint32_t data_fld_idx = 0;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    uint32_t data_field_cnt;
    bool aux;
    uint32_t d_ptr; /* Extended HA pointer contains also the data block type */
    uint32_t k_ptr;
    uint8_t *f_data;
    bcmimm_be_fields_t be_in;
    uint32_t tbl_idx;
    uint32_t key_size = 1;   /* Single byte for keyless tables */
    bool enable_ha;
    bcmltd_fields_t *returned_fields;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    /*
     * Field ID size is for the 2-tuple (field SID, idx) where idx may not
     * exist for tables without array. This is another input validation.
     */
    data_f_size = tbl->f_data_size;
    data_field_cnt = in->count - tbl->key_fld_cnt;
    /* Check that the input is not too large */
    if (tbl->data_bits_len / (data_f_size  + BCMIMM_BE_FID_SIZE) <
            in->count - tbl->key_fld_cnt) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    enable_ha = ha_enabled && !tbl->interactive;
    if (tbl->interactive) {
        returned_fields = read_only_resources[INTERACTIVE_IDX][unit];
    } else {
        returned_fields = read_only_resources[MODELED_IDX][unit];
    }
    returned_fields->count = MAX_READ_ONLY_FIELDS;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_notify_stage(unit, in, tbl, tbl_idx,
                             trans_id, BCMIMM_ENTRY_INSERT, returned_fields));
    if (returned_fields->count > 0) {
        data_field_cnt += returned_fields->count;
    }

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    if (tbl->interactive) {
        SHR_ALLOC(data_buf, tbl->data_bits_len + key_size, "bcmimmInter");
        key_buf = data_buf + tbl->data_bits_len;
    } else {
        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;
    }

    /* The data portion starts after the field ID tuple */
    f_data = data_buf + BCMIMM_BE_FID_SIZE * data_field_cnt;

    be_in.fid = (uint32_t *)data_buf;
    be_in.fdata = f_data;

    data_fld_idx = bcmimm_compress_fields_into_buffer(in, tbl, key_buf,
                                                      f_data, data_buf);

    if (returned_fields->count > 0) {
        data_fld_idx = bcmimm_compress_ro_fields_into_buffer(returned_fields,
                                                             tbl,
                                                             data_fld_idx,
                                                             f_data,
                                                             data_buf);
    }

    /* Update the transaction information */
    if (enable_ha) {
        buf_hdr_update(unit,
                       data_buf,
                       BUFF_HEADER_SIZE, /* No data to store */
                       key_size,
                       0,               /* 0 data elements */
                       BCMLT_OPCODE_INSERT,
                       aux);

        SHR_IF_ERR_EXIT(update_trans_info(unit, tbl, d_ptr, k_ptr, trans_id));
    }

    be_in.count = (size_t)data_fld_idx;
    SHR_IF_ERR_EXIT(bcmimm_be_entry_insert(tbl->blk_hdl, key_buf, &be_in));

    if (tbl->interactive) {
        SHR_FREE(data_buf);
    } else if (!ha_enabled) {
        /* For the commit we need to add this entry to the transaction */
        SHR_IF_ERR_EXIT(update_trans_info(unit, tbl, 0, 0, trans_id));
        buffs_cancel(unit, tbl, aux);
    }
exit:
    /* In case of an error free ownership if owner is south bound */
    if (SHR_FUNC_ERR()) {
        if (tbl) {
            /* Clean up the buffer*/
            if (data_buf) {
                if (enable_ha) {
                    buffs_free(unit, tbl, BUFF_HEADER_SIZE, key_size, aux);
                } else {
                    if (tbl->interactive) {
                        SHR_FREE(data_buf);
                    } else {
                        buffs_cancel(unit, tbl, aux);
                    }
                }
            }
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_get_first(int unit,
                                  bcmltd_sid_t sid,
                                  bcmltd_fields_t *out,
                                  bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    bcmimm_be_fields_t be_out;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    uint32_t buf_size;
    uint32_t key_size = 1;
    uint32_t data_f_size;
    int extr_fld_cnt;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, NULL);

    SHR_NULL_CHECK(tbl, SHR_E_PARAM);

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    buf_size = key_size + tbl->data_bits_len;

    SHR_ALLOC(data_buf, buf_size, "bcmimmDataBuf");
    SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
    sal_memset(data_buf, 0, buf_size);

    data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;
    be_out.count = tbl->data_bits_len / data_f_size;
    be_out.fid = (uint32_t *)data_buf;
    be_out.fdata = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
    key_buf = data_buf + tbl->data_bits_len; /* Key at the buffer ends */

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_be_table_ctx_get_first(tbl->blk_hdl, key_buf, &be_out));
    if (be_out.count + tbl->key_fld_cnt > out->count) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

    /*
     * Extract the data fields. Extracted field count (extr_fld_cnt)
     * is the number of different fields (i.e. different field ID)
     * that were extracted.
     * Note that be_out->count is the total number of fields that were
     * retrieved from the BE entry. This counts each array element.
     */
    extr_fld_cnt = bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    extract_key_into_flds(key_buf, tbl, out, be_out.count);
    if (owner == BCMIMM_TABLE_NB) {
        if (extr_fld_cnt < tbl->data_fld_cnt) {
            out->count = be_out.count + tbl->key_fld_cnt +
                bcmimm_def_vals_fill(out, be_out.count + tbl->key_fld_cnt, tbl);
        } else {
            out->count = be_out.count + tbl->key_fld_cnt;
        }
    } else {
        out->count = be_out.count + tbl->key_fld_cnt;
    }

exit:

    SHR_FREE(data_buf);
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_get_next(int unit,
                                 bcmltd_sid_t sid,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    bcmimm_be_fields_t be_out;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    uint32_t buf_size;
    uint8_t *in_key_buf;
    uint32_t key_size = 1;
    uint32_t data_f_size;
    int extr_fld_cnt;
    uint32_t key_fields;
    size_t j;
    int idx;
    const bcmltd_field_t *fld;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, NULL);

    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    /* Key size being taken twice, once for input and once for output */
    buf_size = key_size * 2 + tbl->data_bits_len;

    SHR_ALLOC(data_buf, buf_size, "bcmimmDataBuf");
    SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
    sal_memset(data_buf, 0, buf_size);

    data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;
    be_out.count = tbl->data_bits_len / data_f_size;
    be_out.fid = (uint32_t *)data_buf;
    be_out.fdata = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
    in_key_buf = data_buf + tbl->data_bits_len;
    key_buf = in_key_buf + key_size;

    /* Extract the key field into the input key (in_key_buf) */
    key_fields = tbl->key_fld_cnt;
    for (j = 0; (j < in->count) && (key_fields > 0); j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, fld->id)) != -1) {
            bcmimm_insert_key_fld(tbl, in_key_buf, idx, fld->data);
            key_fields--;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_be_table_ctx_get_next(tbl->blk_hdl,
                                                         in_key_buf,
                                                         key_buf,
                                                         &be_out));
    if (be_out.count + tbl->key_fld_cnt > out->count) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

    /*
     * Extract the data fields. Extracted field count (extr_fld_cnt)
     * is the number of different fields (i.e. different field ID)
     * that were extracted.
     * Note that be_out->count is the total number of fields that were
     * retrieved from the BE entry. This counts each array element.
     */
    extr_fld_cnt = bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    extract_key_into_flds(key_buf, tbl, out, be_out.count);
    if (owner == BCMIMM_TABLE_NB) {
        if (extr_fld_cnt < tbl->data_fld_cnt) {
            out->count = be_out.count + tbl->key_fld_cnt +
                bcmimm_def_vals_fill(out, be_out.count + tbl->key_fld_cnt, tbl);
        } else {
            out->count = be_out.count + tbl->key_fld_cnt;
        }
    } else {
        out->count = be_out.count + tbl->key_fld_cnt;
    }

exit:
    SHR_FREE(data_buf);
    SHR_FUNC_EXIT();
}
