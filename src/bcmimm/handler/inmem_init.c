/*! \file inmem_init.c
 *
 *  Initializes in memory component front end.
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
#include <sal/sal_sem.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/front_e_ha.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmimm/bcmimm_int_comp.h>
#include "inmem_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

/* This defines the largest supported transaction size. */
#define MAX_AUX_ENTRIES 100

#define BLK_SIGNATURE   0xd582ab66

typedef struct {
    uint8_t *buff;
    sal_sem_t sync;
} working_buff_t;

static working_buff_t bcmimm_working_buffers[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t supported_tables[BCMDRD_CONFIG_MAX_UNITS];

/* Indication, per unit, if the unit was initialized (work around) */
static bool initialized[BCMDRD_CONFIG_MAX_UNITS];

/*
 * This is a running number per unit. For every table created, this
 * number increments by one. This array keeps the last value per unit.
 */
static uint8_t imm_fe_sub_id[BCMDRD_CONFIG_MAX_UNITS];

/* This is the in-memory component ID. It is applicable for every unit. */
static uint8_t imm_fe_comp_id = BCMMGMT_IMM_FE_COMP_ID;

/*
 * This array records the input parameter of the CTH init function. This
 * information being used later in the system manager init function.
 */
static bcmltd_handler_lifecycle_arg_t init_args[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Global Variables
 */
imm_ha_blk_t *bcmimm_tables_list[BCMDRD_CONFIG_MAX_UNITS];

bcmimm_lt_event_t *bcmimm_tables_cb[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Allocate generic buffer HA block.
 *
 * \param [in] unit Is the unit to allocate the block for.
 * \param [in] warm indicates if this is cold or warm boot.
 * \param [in] sub Is the sub module to use for HA block allocation.
 * \param [out] blk Returned pointer to the allocated HA block.
 * \param [in] len Contains the desired block length as input and being
 * update with the actual allocated length for output.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int generic_blk_alloc (int unit,
                             bool warm,
                             uint8_t sub,
                             uint8_t **blk,
                             uint32_t *len)
{
    generic_array_blk_t *blk_hdr;

    SHR_FUNC_ENTER(unit);
    *blk = shr_ha_mem_alloc(unit,
                            imm_fe_comp_id,
                            sub,
                            len);
    SHR_NULL_CHECK(*blk, SHR_E_MEMORY);

    blk_hdr = (generic_array_blk_t *)*blk;
    if (!warm || (blk_hdr->signature != BLK_SIGNATURE)) {
        sal_memset(blk_hdr, 0, *len);
        blk_hdr->blk_size = *len;
        blk_hdr->free_offset = sizeof(generic_array_blk_t);
        blk_hdr->signature = BLK_SIGNATURE;
    }
    blk_hdr->syc_obj = sal_mutex_create("INMEM");
    SHR_NULL_CHECK(blk_hdr->syc_obj, SHR_E_MEMORY);
exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Extract data field value to a single uint64.
 *
 * \param [in] src Is the data field to extract its value.
 * \param [in] dtag Is the data dag indicating the type of the value in \c src.
 *
 * \return The data value of the field as uint64 type.
 */
static uint64_t extract_union_value(bcmltd_field_data_t *src,
                                    bcmltd_field_data_tag_t dtag)
{
    uint64_t val;

    switch (dtag) {
    case BCMLT_FIELD_DATA_TAG_BOOL:
        val = (uint64_t)src->is_true;
        break;
    case BCMLT_FIELD_DATA_TAG_U8:
        val = (uint64_t)src->u8;
        break;
    case BCMLT_FIELD_DATA_TAG_U16:
        val = (uint64_t)src->u16;
        break;
    case BCMLT_FIELD_DATA_TAG_U32:
        val = (uint64_t)src->u32;
        break;
    case BCMLT_FIELD_DATA_TAG_U64:
        val = src->u64;
        break;
    default:
        val = 0;
        break;
    }

    return val;
}


/*!
 * \brief Allocate fixed HA blocks.
 *
 * This function allocates known HA block for the unit. The block
 * being used to manage all the FE tables for the given unit.
 *
 * \param [in] unit Is the unit assiciated with the blocks.
 * \param [out] cold Indicates if it is a cold or warm boot.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int ha_blk_table_alloc(int unit, bool *warm)
{
    uint32_t len = supported_tables[unit] * sizeof(table_info_t) +
                    sizeof(imm_ha_blk_t);
    uint32_t actual_len = len;
    imm_ha_blk_t *unit_tbl;

    SHR_FUNC_ENTER(unit);
    unit_tbl = shr_ha_mem_alloc(unit,
                                imm_fe_comp_id,
                                GENERIC_TBL_COMP_ID,
                                &actual_len);
    SHR_NULL_CHECK(unit_tbl, SHR_E_MEMORY);
    if (actual_len < len) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }

    if (!(*warm) || unit_tbl->signature != BLK_SIGNATURE) {
        *warm = false;
        sal_memset(unit_tbl, 0, len);
        unit_tbl->array_size = supported_tables[unit];
        unit_tbl->signature = BLK_SIGNATURE;
    } else {
        /* Clear all the real pointers */
        unit_tbl->aux_blk = NULL;
        unit_tbl->data_blk = NULL;
        unit_tbl->key_blk = NULL;
        unit_tbl->trans_blk = NULL;
    }
    bcmimm_tables_list[unit] = unit_tbl;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate Generic HA blocks for data, key and transaction buffers.
 *
 * This function allocates HA blocks for the unit. These blocks
 * being used as buffers for the data and key portions for all the tables.
 * The auxilury table being used in the case were a single table require
 * more than one entry in a transaction. This entry will be kept in a buffer(s)
 * provided by the auxilury block.
 *
 * \param [in] unit Is the unit assiciated with the blocks.
 * \param [in] data_size Is the desired block size for the data buffers.
 * \param [in] key_size Is the desired block size for the key buffers.
 * \param [in] aux_size Is the desired block size to handle extra key and data
 * buffers.
 * \param [in] warm Indicates if it is a cold or warm boot.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int ha_buff_blk_alloc(int unit,
                             size_t data_size,
                             size_t key_size,
                             size_t aux_size,
                             bool warm)
{
    imm_ha_blk_t *tbl_blk;
    uint32_t len;

    SHR_FUNC_ENTER(unit);

    tbl_blk = bcmimm_tables_list[unit];
    /* Allocate data table */
    len = data_size + sizeof(generic_array_blk_t);
    SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                      warm,
                                      GENERIC_DATA_COMP_ID,
                                      &tbl_blk->data_blk,
                                      &len));

    /* Allocate key table */
    len = key_size + sizeof(generic_array_blk_t);
    SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                      warm,
                                      GENERIC_KEY_COMP_ID,
                                      &tbl_blk->key_blk,
                                      &len));

    /*
     * Allocate the auxiluary table. This will be used to store data/keys for
     * transactions with more than a single entry per table. The size should be
     * calculated to be N time the largest entry.
     */
    len = aux_size + sizeof(generic_array_blk_t);
    SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                      warm,
                                      AUX_TBL_COMP_ID,
                                      &tbl_blk->aux_blk,
                                      &len));
    /*
     * Allocate the transaction table.
     */
    len = BCMIMM_MAX_TRANS_SIZE * sizeof(imm_trans_elem_t) +
            sizeof(generic_array_blk_t) + sizeof(imm_trans_info_t);
    SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                      warm,
                                      TRANS_TBL_COMP_ID,
                                      &tbl_blk->trans_blk,
                                      &len));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate table information pointer from the management block.
 *
 * This function allocates a table information block from the table array
 * managed by the tables list.
 *
 * \param [in] unit Is the unit assiciated with the blocks.
 *
 * \return Pointer into requested table info.
 */
static table_info_t *ha_table_block_alloc(int unit)
{
    if (bcmimm_tables_list[unit]->array_size <= bcmimm_tables_list[unit]->allocated) {
        return NULL;
    }
    bcmimm_tables_list[unit]->allocated++;
    return &bcmimm_tables_list[unit]->tbl[bcmimm_tables_list[unit]->free_idx++];
}

/*!
 * \brief Free last table information pointer back to the management block.
 *
 * This function allocates a table information block from the table array
 * managed by the tables list.
 *
 * \param [in] unit Is the unit assiciated with the blocks.
 *
 * \return Pointer into requested table info.
 */
static void ha_table_last_block_free(int unit, table_info_t *blk)
{
    /* Verify that it is indeed the last allocated block */
    if (blk !=
        &bcmimm_tables_list[unit]->tbl[bcmimm_tables_list[unit]->free_idx-1]) {
        return;
    }
    bcmimm_tables_list[unit]->allocated--;
    bcmimm_tables_list[unit]->free_idx--;
}

/*!
 * \brief Allocate dynamic buffers to support in-memory logical table.
 *
 * This function allocates all the dynamic buffers associated with an in-memory
 * logical table. The dynamic tables being allocated from HA memory (key and
 * data buffers) as well as regular memory (data and key field information).
 * The HA memory being allocated from known data and key buffer blocks.
 * See \ref ha_fixed_blk_alloc() for HA block allocations.
 *
 * \param [in] unit Is the unit assiciated with the table.
 * \param [in] t_info Pointer to the table structure.
 * \param [in] use_array Indicating if any of the fields of the table is an
 * array.
 * \param [in] max_depth If there is an array field this parameter indicates
 * the maximal number of elements in the array. This can indicates how many
 * bytes require to maintain the index parameter.
 * \param [in] max_field_size Indicates how many bytes required to hold a
 * copy of the field data (max size).
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int table_buffers_alloc(int unit,
                         table_info_t *t_info)
{
    imm_ha_blk_t *blk_ctrl = bcmimm_tables_list[unit];
    generic_array_blk_t *gen_blk;
    uint32_t key_buf_len;
    uint32_t d_buf_len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    d_buf_len = t_info->data_bits_len + BUFF_HEADER_SIZE;
    gen_blk = (generic_array_blk_t *)blk_ctrl->data_blk;
    if (gen_blk->free_offset + d_buf_len > gen_blk->blk_size) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    /* Allocate the data bits */
    t_info->data_bits = gen_blk->free_offset;
    /* Each data buffer has an header  */
    gen_blk->free_offset += d_buf_len;

    if (t_info->key_fld_cnt > 0) {
        key_buf_len = BCMIMM_BITS_TO_BYTES(t_info->key_arry_size);
    } else {
        key_buf_len = 1;
    }
    gen_blk = (generic_array_blk_t *)blk_ctrl->key_blk;
    if (gen_blk->free_offset + key_buf_len > gen_blk->blk_size) {
        SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
    }
    t_info->key_bits = gen_blk->free_offset;
    gen_blk->free_offset += key_buf_len;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find table dimensions.
 *
 * This function calculates the max field size, the key length as well as
 * the max number of array elements (for field arrays). It does it using the
 * information provided by the LRD for this table.
 *
 * \param [in] unit Is the unit assiciated with the blocks.
 * \param [in] t_info Pointer to the table structure.
 * \param [in] num_fid The number of fields in the array \c f_info.
 * \param [in] f_info Is an array containing the information on all the fields
 * in the logical table.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int table_dimention_find(int unit,
                                table_info_t *t_info,
                                size_t num_fid,
                                bcmlrd_client_field_info_t *f_info)
{
    size_t j;
    bcmlrd_client_field_info_t *info;
    bool use_array = false;
    uint32_t field_size;
    uint32_t max_depth = 0;
    uint32_t max_field_size = 0;
    uint32_t max_num_of_flds = 0; /* The maximal number of fields in an entry */
    uint32_t data_f_len; /* Data field length */

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    /* Go through all the fields */
    for (j = 0, info = f_info; j < num_fid; j++, info++) {
        if (info->key) {
            t_info->key_fld_cnt++;
            t_info->key_arry_size += info->width;  /* Width in bits */
        } else {
            t_info->data_fld_cnt++;
            /* Convert width from bits to bytes */
            field_size = BCMIMM_BITS_TO_BYTES(info->width);
            if (field_size > max_field_size) {
                max_field_size = field_size;
            }
            if (info->depth > max_depth) {
                max_depth = info->depth;
            }
            if ((!use_array) && ((info->depth) || (info->elements > 1))) {
                use_array = true;
            }
            if (info->depth > 0) {
                max_num_of_flds += info->depth;
            } else {
                max_num_of_flds += info->elements;
            }
        }
    }

    t_info->max_num_field = max_num_of_flds;

    if (max_field_size < sizeof(uint64_t)) {
        /* Round the data size up to the next 4 bytes */
        t_info->f_data_size = ((max_field_size + 3) / 4) * 4;
    }  else {
        t_info->f_data_size = sizeof(uint64_t);/* This is the maximal scalar */
    }

    if (use_array) {
        if (max_depth < 0x100) {
            t_info->array_idx_size = 1;
        } else if (max_depth < 0x10000) {
            t_info->array_idx_size = 2;
        } else {  /* Max size */
            t_info->array_idx_size = 3;
        }
    } else {
        t_info->array_idx_size = 0;
    }

    /*
     * The cache memory for each field is as follows:
     * field data size + field ID + boolean.
     * The purpose of the boolean is to indicate true for fields that should
     * be deleted.
     */
    data_f_len = t_info->f_data_size +
                BCMIMM_BE_FID_SIZE + sizeof(bool);
    t_info->data_bits_len = max_num_of_flds * data_f_len;

    if (t_info->data_fld_cnt > 0) {
        SHR_ALLOC(t_info->data_fields,
                  sizeof(field_info_t) * t_info->data_fld_cnt,
                  "");
        SHR_NULL_CHECK(t_info->data_fields, SHR_E_MEMORY);
    } else {
        t_info->data_fields = NULL;
    }

    /* Allocate key fields if exist */
    if (t_info->key_fld_cnt) {
        SHR_ALLOC(t_info->key_fields,
                  sizeof(field_info_t) * t_info->key_fld_cnt,
                  "");
        SHR_NULL_CHECK(t_info->key_fields, SHR_E_MEMORY);
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Go through the field list and set the attribute of each one.
 * Attributes include min/max/default values, field ID and width.
 */
static void set_field_attrib(table_info_t *tbl,
                             bcmlrd_client_field_info_t *f_info,
                             size_t num_fid)
{
    field_info_t *key = tbl->key_fields;
    field_info_t *data = tbl->data_fields;
    size_t j;
    bcmlrd_client_field_info_t *info;

    for (j = 0, info = f_info; j < num_fid; j++, info++) {
        if (info->key) {
            key->fid = info->id;
            key->width = info->width;
            key->read_only = false;
            key->min_val = extract_union_value(&info->min, info->dtag);
            key->max_val = extract_union_value(&info->max, info->dtag);
            key->def_val = extract_union_value(&info->def, info->dtag);
            key++;
        } else {
            assert (data != NULL);
            data->fid = info->id;
            data->width = info->width;
            data->read_only = (info->access == BCMLTD_FIELD_ACCESS_READ_ONLY);
            data->min_val = extract_union_value(&info->min, info->dtag);
            data->max_val = extract_union_value(&info->max, info->dtag);
            data->def_val = extract_union_value(&info->def, info->dtag);
            data++;
        }
    }
}

static int allocate_working_buffe(int unit, size_t size)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate the working buffer for each unit */
    SHR_ALLOC(bcmimm_working_buffers[unit].buff, size, "ImmWorkBuf");
    SHR_NULL_CHECK(bcmimm_working_buffers[unit].buff, SHR_E_MEMORY);

    bcmimm_working_buffers[unit].sync = sal_sem_create("ImmWBufSync",
                                                       SAL_SEM_BINARY, 1);
    SHR_NULL_CHECK(bcmimm_working_buffers[unit].sync, SHR_E_MEMORY);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes in-memory logical table
 *
 * This function being called to initialize an in-memory LT for cold start.
 * The function parses all the fields to determine the size of the key buffer
 * and the maximal size required to host all the data fields. It then
 * initializes all the data and key fields array with the field IDs and
 * field width.
 *
 * \param [in] unit Is the unit assiciated with the blocks.
 * \param [in] tname is the logical table name.
 * \param [out] tbl_info Is the newly created pointer to the table information.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int table_init(int unit,
                      uint32_t sid,
                      table_info_t **tbl_info)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    table_info_t *t_info = NULL;
    bcmlrd_table_attrib_t tbl_attr;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, sid, &num_fid));
    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit, lt_info->name, &tbl_attr));

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid, "");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, lt_info->name,
                                     num_fid, f_info, &num_fid));

    t_info = ha_table_block_alloc(unit);
    SHR_NULL_CHECK(t_info, SHR_E_INTERNAL);
    sal_memset(t_info, 0, sizeof(*t_info));
    /* Coverity
     * while the string may not be null terminated, the following
     * statement checks this condition and post an error in this case.
     */
    /* coverity[buffer_size_warning : FALSE] */
    sal_strncpy(t_info->tbl_name, lt_info->name, sizeof(t_info->tbl_name));
    if (t_info->tbl_name[sizeof(t_info->tbl_name)-1] != '\0') {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Table name %s is too long\n"),
                  lt_info->name));
        t_info->tbl_name[0] = '\0';  /* clean up the table name */
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    t_info->sid = sid;
    t_info->interactive = tbl_attr.interactive;

    SHR_IF_ERR_EXIT(table_dimention_find(unit, t_info, num_fid, f_info));

    set_field_attrib(t_info, f_info, num_fid);

    *tbl_info = t_info;
exit:
    SHR_FREE(f_info);
    if (SHR_FUNC_ERR() && t_info != NULL) {
        ha_table_last_block_free(unit, t_info);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Warm Initialization of in-memory logical tables
 *
 * This function being called to initialize an in-memory LTs in warm boot.
 * Unlike cold boot, the tables had already been initialized and resides in
 * HA memory. What left is to retrieve these tables and to allocate the
 * resources that are stored in regular memory.
 *
 * \param [in] unit Is the unit assiciated with the blocks.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int init_warm(int unit, const bcmltd_handler_lifecycle_arg_t *arg)
{
    uint32_t idx;
    table_info_t *tbl;
    bcmlrd_table_attrib_t t_attrib;
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info = NULL;
    size_t key_size;
    size_t max_entry_size = 0;

    SHR_FUNC_ENTER(unit);
    /* Tables_list for this unit was already allocated */
    for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
        tbl = &bcmimm_tables_list[unit]->tbl[idx];
        /* We have the table name - get the sid */
        SHR_IF_ERR_EXIT
            (bcmlrd_table_attributes_get(unit, tbl->tbl_name, &t_attrib));
        /* Refresh the sid as it will not stay the same across reboot */
        tbl->sid = t_attrib.id;

        tbl->data_fields = NULL;
        if (tbl->data_fld_cnt > 0) {
            SHR_ALLOC(tbl->data_fields,
                      sizeof(field_info_t) * tbl->data_fld_cnt,
                      "");
            SHR_NULL_CHECK(tbl->data_fields, SHR_E_MEMORY);
        }

        tbl->key_fields = NULL;
        if (tbl->key_fld_cnt > 0) {
            SHR_ALLOC(tbl->key_fields,
                      sizeof(field_info_t) * tbl->key_fld_cnt,
                      "immKey");
            SHR_NULL_CHECK(tbl->key_fields, SHR_E_MEMORY);
            key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
        } else {
            key_size = 1;  /* Key size must be minimum 1 byte */
        }

        /* Now allocate the backend block */
        SHR_IF_ERR_EXIT
            (bcmimm_be_table_create(unit,
                                    imm_fe_comp_id,
                                    imm_fe_sub_id[unit]++,
                                    key_size,
                                    tbl->f_data_size,
                                    arg->generic_arg[idx]->value[0],
                                    true,
                                    (bcmimm_be_tbl_hdl_t *)&tbl->blk_hdl));

        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, tbl->sid, &num_fid));
        SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid, "");
        SHR_NULL_CHECK(f_info, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(bcmlrd_table_fields_def_get(unit,
                                                    tbl->tbl_name, num_fid,
                                                    f_info, &num_fid));

        set_field_attrib(tbl, f_info, num_fid);
        SHR_FREE(f_info);  /* Can be optimized by checking the desired size */
        f_info = NULL;

        if (max_entry_size < tbl->data_bits_len) {
            max_entry_size = tbl->data_bits_len;
        }
    }

    max_entry_size += BUFF_HEADER_SIZE + sizeof(uint64_t);
    SHR_IF_ERR_EXIT(allocate_working_buffe(unit, max_entry_size));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialization of all in-memory logical tables for a specific unit.
 *
 * This function is being called to initialize all the in-memory LTs associated
 * with a particular unit.
 *
 * \param [in] unit Is the unit to initialize all its associated in-memory
 * tables.
 * \param [in] warm Indicates if the init is cold or warm.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int init_unit_tables(int unit, bool warm)
{
    bcmltd_handler_lifecycle_arg_t *arg;
    uint32_t j, k;
    table_info_t *tbl_info = NULL;
    size_t total_data_len = 0;
    size_t total_key_len = 0;
    size_t max_data_len = 0;
    size_t aux_size;
    bcmlrd_map_t const *map;
    uint32_t unmapped_tbls = 0;

    SHR_FUNC_ENTER(unit);

    if (init_args[unit].generic_args == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);  /* Possible not to have IMM tables */
    }
    arg = &init_args[unit];

    supported_tables[unit] = arg->generic_args;

    imm_fe_sub_id[unit] = MIN_SUB_ID;

    /* Start by allocating global resources */
    SHR_ALLOC(bcmimm_tables_cb[unit],
              arg->generic_args * sizeof(bcmimm_lt_event_t),
              "IMM_CB");
    SHR_NULL_CHECK(bcmimm_tables_cb[unit], SHR_E_MEMORY);
    sal_memset(bcmimm_tables_cb[unit], 0,
               arg->generic_args * sizeof(bcmimm_lt_event_t));

    SHR_IF_ERR_EXIT(ha_blk_table_alloc(unit, &warm));

    bcmimm_be_init(unit, BCMMGMT_IMM_BE_COMP_ID, warm);
    if (warm) {
        /*
         * Now allocate the generic HA buffer blocks. At this point we don't
         * know the desired buffer size. Fortunately, since this is warm boot
         * the HA blocks already have their proper size, so we simply tapping
         * to them.
         */
        SHR_IF_ERR_EXIT(ha_buff_blk_alloc(unit,
                                          1000,
                                          100,
                                          100 * MAX_AUX_ENTRIES,
                                          warm));
        SHR_IF_ERR_EXIT(init_warm(unit, arg));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /*
     * This loop calculates the resources require by every table. It than
     * adds the buffer resources to the total required by all tables.
     * These totals will be used when allocating the generic data and
     * key buffers.
     */
    for (j = 0; j < arg->generic_args; j++) {
        /* Verify that the table is mapped. Continue if not */
        if (bcmlrd_map_get(unit,
                           arg->generic_arg[j]->sid,
                           &map) == SHR_E_UNAVAIL) {
            unmapped_tbls++;
            continue;
        }
        SHR_IF_ERR_EXIT(table_init(unit, arg->generic_arg[j]->sid, &tbl_info));
        /* Track the largest data length. Used for auxiluary calculation. */
        if (max_data_len < tbl_info->data_bits_len) {
            max_data_len = tbl_info->data_bits_len;
        }
        total_data_len += tbl_info->data_bits_len + BUFF_HEADER_SIZE;
        if (tbl_info->key_fld_cnt > 0) {
            total_key_len += BCMIMM_BITS_TO_BYTES(tbl_info->key_arry_size);
        } else {
            total_key_len++;
        }
    }

    /* Auxiliary table entries includes data + buffer header + key size. */
    aux_size = max_data_len + BUFF_HEADER_SIZE + sizeof(uint64_t);

    SHR_IF_ERR_EXIT(allocate_working_buffe(unit, aux_size));

    /* Now allocate the generic HA buffer blocks */
    SHR_IF_ERR_EXIT(ha_buff_blk_alloc(unit,
                                      total_data_len,
                                      total_key_len,
                                      aux_size * MAX_AUX_ENTRIES,
                                      warm));

    /* Sanity check */
    assert(arg->generic_args ==
           bcmimm_tables_list[unit]->allocated + unmapped_tbls);

    /*
     * Last allocate the buffers for each table and create all the backend
     * tables.
     */
    for (j = 0, k = 0; j < arg->generic_args; j++) {
        size_t rows;
        size_t key_size;
        uint8_t sub_id;

        /* Verify that the table is mapped. Continue if not */
        if (bcmlrd_map_get(unit,
                           arg->generic_arg[j]->sid,
                           &map) == SHR_E_UNAVAIL) {
            continue;
        }
        if (arg->generic_arg[j]->values == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
        }
        rows = arg->generic_arg[j]->value[0];
        if (rows == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
        }

        tbl_info = &bcmimm_tables_list[unit]->tbl[k++];
        assert(tbl_info->sid == arg->generic_arg[j]->sid);
        SHR_IF_ERR_EXIT(table_buffers_alloc(unit, tbl_info));
        if (tbl_info->sub != 0) {
            sub_id = tbl_info->sub; /* Reuse the same sub ID */
        } else {
            tbl_info->sub = imm_fe_sub_id[unit];
            sub_id = imm_fe_sub_id[unit]++;
        }
        if (tbl_info->key_fld_cnt == 0) {
            key_size = 1;  /* Key size must be minimum 1 byte */
        } else {
            key_size = BCMIMM_BITS_TO_BYTES(tbl_info->key_arry_size);
        }
        SHR_IF_ERR_EXIT
            (bcmimm_be_table_create(unit,
                                    imm_fe_comp_id,
                                    sub_id,
                                    key_size,
                                    tbl_info->f_data_size,
                                    rows,
                                    warm,
                                    (bcmimm_be_tbl_hdl_t *)&tbl_info->blk_hdl));
    }

    /* Clean the table information. */
    for (j = 0; j < arg->generic_args; j++) {
        if (arg->generic_arg[j]->values > 0) {
            sal_free((void *)arg->generic_arg[j]->value);
        }
        sal_free((void *)arg->generic_arg[j]);
    }
    sal_free(arg->generic_arg);
exit:
    if (SHR_FUNC_ERR()) {
        if (bcmimm_working_buffers[unit].buff) {
            SHR_FREE(bcmimm_working_buffers[unit].buff);
            bcmimm_working_buffers[unit].buff = NULL;
        }
        if (bcmimm_working_buffers[unit].sync) {
            sal_sem_destroy(bcmimm_working_buffers[unit].sync);
            bcmimm_working_buffers[unit].sync = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * External functions local to this module
 */
int bcmimm_unit_init(int unit, uint8_t comp_id, bool warm)
{
    const bcmcfg_feature_ctl_config_t *feature_conf;

    SHR_FUNC_ENTER(unit);

    imm_fe_comp_id = comp_id;
    imm_fe_sub_id[unit] = MIN_SUB_ID;

    SHR_IF_ERR_EXIT(init_unit_tables(unit, warm));

    feature_conf = bcmcfg_feature_ctl_config_get();
    if (feature_conf == NULL || feature_conf->dis_stomic_trans == 0) {
        bcmimm_ha_enable_set(true);
    } else {
        bcmimm_ha_enable_set(false);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_unit_shutdown(int unit)
{
    uint32_t idx;
    table_info_t *tbl;
    imm_ha_blk_t *tbl_blk;
    generic_array_blk_t *blk_hdr;

    SHR_FUNC_ENTER(unit);

    if (supported_tables[unit] > 0) {
        /*
         * The cleanup activity doesn't close or delete HA files. The reason is
         * that this activity will be done by the application if it wants to start
         * cold (i.e. open new HA file). If the application wants warm boot then
         * everything is available.
        */
        for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
            tbl = &bcmimm_tables_list[unit]->tbl[idx];
            /* Close the back-end table */
            SHR_IF_ERR_EXIT(bcmimm_be_table_release(tbl->blk_hdl));
            if (tbl->key_fields) {
                SHR_FREE(tbl->key_fields);
            }
            SHR_FREE(tbl->data_fields);
        }
        tbl_blk = bcmimm_tables_list[unit];
        blk_hdr = (generic_array_blk_t *)tbl_blk->data_blk;
        sal_mutex_destroy(blk_hdr->syc_obj);
        blk_hdr = (generic_array_blk_t *)tbl_blk->aux_blk;
        sal_mutex_destroy(blk_hdr->syc_obj);
        blk_hdr = (generic_array_blk_t *)tbl_blk->key_blk;
        sal_mutex_destroy(blk_hdr->syc_obj);
        blk_hdr = (generic_array_blk_t *)tbl_blk->trans_blk;
        sal_mutex_destroy(blk_hdr->syc_obj);

        if (bcmimm_working_buffers[unit].buff) {
            SHR_FREE(bcmimm_working_buffers[unit].buff);
            bcmimm_working_buffers[unit].buff = NULL;
        }
        if (bcmimm_working_buffers[unit].sync) {
            sal_sem_destroy(bcmimm_working_buffers[unit].sync);
            bcmimm_working_buffers[unit].sync = NULL;
        }

        SHR_FREE(bcmimm_tables_cb[unit]);
    }

exit:
    SHR_FUNC_EXIT();
}

uint8_t *bcmimm_working_buff_alloc(int unit)
{
    sal_sem_take(bcmimm_working_buffers[unit].sync, SAL_SEM_FOREVER);
    return bcmimm_working_buffers[unit].buff;
}

void bcmimm_working_buff_free(int unit)
{
    sal_sem_give(bcmimm_working_buffers[unit].sync);
}

/*******************************************************************************
 * Public functions
 */
int bcmimm_basic_init(int unit,
                      const bcmltd_handler_lifecycle_arg_t *arg) {
    bcmltd_handler_lifecycle_arg_t input_arg;
    uint32_t j;
    uint32_t *value;
    bcmltd_generic_arg_t **gen_arg_array = NULL;
    bcmltd_generic_arg_t *gen_arg;

    SHR_FUNC_ENTER(unit);

    if (initialized[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    input_arg.generic_args = arg->generic_args;
    SHR_ALLOC(gen_arg_array,
              arg->generic_args * sizeof(bcmltd_generic_arg_t *),
              "immInitArgArry");
    SHR_NULL_CHECK(gen_arg_array, SHR_E_MEMORY);
    sal_memset(gen_arg_array, 0,
               arg->generic_args * sizeof(bcmltd_generic_arg_t *));
    for (j = 0; j < arg->generic_args; j++) {
        gen_arg = NULL;
        SHR_ALLOC(gen_arg, sizeof(bcmltd_generic_arg_t), "immInitArg");
        SHR_NULL_CHECK(gen_arg, SHR_E_MEMORY);
        sal_memset(gen_arg, 0, sizeof(bcmltd_generic_arg_t));
        gen_arg_array[j] = gen_arg;
        gen_arg->sid = arg->generic_arg[j]->sid;
        gen_arg->values = arg->generic_arg[j]->values;
        if (arg->generic_arg[j]->values > 0) {
            value = NULL;
            SHR_ALLOC(value,
                      sizeof(uint32_t) * arg->generic_arg[j]->values,
                      "immInitVal");
            SHR_NULL_CHECK(value, SHR_E_MEMORY);
            sal_memcpy(value,
                       arg->generic_arg[j]->value,
                       sizeof(uint32_t) * arg->generic_arg[j]->values);
            gen_arg->value = value;
        }
    }
    input_arg.generic_arg = (const bcmltd_generic_arg_t **)gen_arg_array;
    init_args[unit] = input_arg;

    initialized[unit] = true;

exit:
    if (SHR_FUNC_ERR() && gen_arg_array) {
        for (j = 0; j < arg->generic_args; j++) {
            gen_arg = gen_arg_array[j];
            if (gen_arg) {
                SHR_FREE(gen_arg->value);
                SHR_FREE(gen_arg_array[j]);
            }
        }
        SHR_FREE(gen_arg_array);
    }
    SHR_FUNC_EXIT();
}

int bcmimm_basic_cleanup(int unit,
                         const bcmltd_handler_lifecycle_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (!initialized[unit]) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    initialized[unit] = false;
exit:
    SHR_FUNC_EXIT();
}
