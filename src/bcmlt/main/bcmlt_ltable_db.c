/*! \file bcmlt_ltable_db.c
 *
 * Logical tables cache DB contains a forest of elements. The forest
 * is composed of the logical table information. The key for each tree in
 * the forest is the logical table name. Each tree contains the information
 * about all its fields.
 * The implementation is taking advantage of hash table for string keys.
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

#include <shr/shr_hash_str.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <sal/sal_alloc.h>
#include <sal/sal_rwlock.h>
#include <bcmlt/bcmlt.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltm/bcmltm_md.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

#define FIELD_ALLOC_CHUNK_SIZE  512
#define TABLE_ALLOC_CHUNK_SIZE  64

#define FIELD_ARRAY_CHUNK_SIZE  16

typedef struct field_struct_s {
    bcmlt_field_def_t field_def;
    uint32_t    field_id;
    struct field_struct_s *next;
} field_struct_t;

typedef struct table_start_s {
    bcmlt_table_attrib_t attrib;
    shr_hash_str_hdl field_db;
    field_struct_t *fields;
    struct table_start_s *next;
} table_start_t;

typedef struct device_tables_db_s {
    int unit;
    shr_hash_str_hdl table_db;
    sal_rwlock_t  rwlock;
    table_start_t *tables;
} device_tables_db_t;

typedef struct device_table_db_width_s {
    uint32_t mid_size;
    uint32_t max_size;
    shr_lmm_hdl_t mid_size_hdl;
    shr_lmm_hdl_t max_size_hdl;
} device_table_db_width_t;


static device_tables_db_t table_db[BCMDRD_CONFIG_MAX_UNITS];

static device_table_db_width_t table_db_width[BCMDRD_CONFIG_MAX_UNITS];

static shr_lmm_hdl_t field_elements;
static shr_lmm_hdl_t table_elements;

/* Maintain the maximal number of fields in an interactive table */
static uint32_t table_db_interact_tbl_max_fields[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Converts tagged value to uint64_t
 *
 * This function converts old data tagged values into uniform uint64_t values.
 *
 * \param [in] field_data is a union of values.
 * \param [in] dtag is the data tag indicating which union element of
 * \c field_data is relevant.
 *
 * \return On success the value of \c field_data returned. Otherwise, the
 * function returns 0. If the data tag is invalid the function also log an
 * error message.
 */
static uint64_t field_tag_to_u64(bcmlt_field_data_t field_data,
                                 bcmlt_field_data_tag_t  dtag)
{
    switch (dtag) {
    case BCMLT_FIELD_DATA_TAG_BOOL:
        return (uint64_t)field_data.is_true;
    case BCMLT_FIELD_DATA_TAG_U8:
        return (uint64_t)field_data.u8;
    case BCMLT_FIELD_DATA_TAG_U16:
        return (uint64_t)field_data.u16;
    case BCMLT_FIELD_DATA_TAG_U32:
        return (uint64_t)field_data.u32;
    case BCMLT_FIELD_DATA_TAG_U64:
        return field_data.u64;
    case BCMLT_FIELD_DATA_TAG_RAW:
        return 0;
    default:
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unsuported data tag %d\n"),
                   dtag));
        return 0;
    }
}

/*!
 *\brief Update the local DB with a LT fields.
 *
 * This function obtains the list of fields from a particular LT. It then
 * updates the local tree of this table with the fields attributes.
 * The function is called during initialization or when a table being query
 * for the first time.
 *
 * \param [in] unit Is the unit of this LT.
 * \param [in] table Is a pointer to the logical table DB.
 * \param [in] num_of_fields Is the number of expected fields in the table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int obtain_lt_fields(int unit,
                            table_start_t *table,
                            size_t num_of_fields)
{
    uint32_t j;
    field_struct_t *field;
    int rv;
    size_t actual_fields;
    bcmlrd_client_field_info_t *field_array;
    bcmlt_field_data_tag_t  dtag;
    uint32_t max_fields;

    field_array = sal_alloc(sizeof(bcmlrd_client_field_info_t)
                            *num_of_fields, "");
    if (!field_array) {
        return SHR_E_MEMORY;
    }
    rv = bcmlrd_table_fields_def_get(unit,
                                     table->attrib.name,
                                     num_of_fields,
                                     field_array,
                                     &actual_fields);
    if ((rv != SHR_E_NONE) || (num_of_fields != actual_fields)) {
        sal_free(field_array);
        return SHR_E_INTERNAL;  /* LRD is not suppose to fail here */
    }

    table->attrib.max_fid = 0;
    max_fields = 0;
    for (j = 0; j < num_of_fields; j++) {
        field = shr_lmm_alloc(field_elements);
        if (!field) {
            rv = SHR_E_MEMORY;
            break;
        }
        dtag = field_array[j].dtag;
        field->field_id = field_array[j].id;
        field->field_def.name = field_array[j].name;
        field->field_def.access = field_array[j].access;
        field->field_def.min = field_tag_to_u64(field_array[j].min, dtag);
        field->field_def.max = field_tag_to_u64(field_array[j].max, dtag);
        field->field_def.def = field_tag_to_u64(field_array[j].def, dtag);
        field->field_def.dtag = dtag;
        field->field_def.width = field_array[j].width;
        field->field_def.depth = field_array[j].depth;
        field->field_def.elements = field_array[j].elements;
        field->field_def.key = field_array[j].key;
        field->field_def.array = field_array[j].array;
        field->field_def.symbol = field_array[j].symbol;
        if (field->field_def.symbol) {
            rv = bcmlrd_field_value_to_symbol(unit,
                                              table->attrib.table_id,
                                              field->field_id,
                                              field->field_def.def,
                                              &field->field_def.sym_def);
            if (rv != SHR_E_NONE) {
                break;
            }
        } else {
            field->field_def.sym_def = NULL;
        }

        if (field->field_id > table->attrib.max_fid) {
            table->attrib.max_fid = field->field_id;
        }
        /* Add the field into the hash DB */
        rv = shr_hash_str_dict_insert(table->field_db,
                                      field->field_def.name,
                                      field);
        if (rv != 0) {
            rv = SHR_E_MEMORY;
            break;
        }
        /* Add the field into the field list */
        field->next = table->fields;
        table->fields = field;
        max_fields += field_array[j].elements;
    }
    sal_free((void *)field_array);

    if (table->attrib.interactive) {
        if (max_fields > table_db_interact_tbl_max_fields[unit]) {
            table_db_interact_tbl_max_fields[unit] = max_fields;
        }
    }
    return rv;
}

/*!
 *\brief Update the local DB with a PT fields.
 *
 * This function obtains the list of fields from a particular PT. It then
 * updates the local tree of this table with the fields attributes.
 * The function is called during initialization or when a table being query
 * for the first time.
 *
 * \param [in] unit Is the unit of this PT.
 * \param [in] table Is a pointer to the logical table DB.
 * \param [in] num_of_fields Is the number of expected fields in the table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int obtain_pt_fields (int unit,
                             table_start_t *table,
                             size_t num_of_fields)
{
    uint32_t j;
    field_struct_t *field;
    int rv;
    size_t actual_fields;
    uint32_t *field_array;
    bcmdrd_sym_field_info_t f_info;
    uint32_t num_of_bits;

    field_array = sal_alloc(sizeof(uint32_t)*num_of_fields, "");
    if (!field_array) {
        return SHR_E_MEMORY;
    }
    rv = bcmltm_md_pt_field_list_get(unit,
                           table->attrib.table_id,
                           num_of_fields,
                           field_array,
                           &actual_fields);
    if ((rv != SHR_E_NONE) || (num_of_fields != actual_fields)) {
        sal_free(field_array);
        return SHR_E_MEMORY;
    }

    for (j = 0; j < num_of_fields; j++) {
        field = shr_lmm_alloc(field_elements);
        if (!field) {
            rv = SHR_E_MEMORY;
            break;
        }
        sal_memset(field, 0, sizeof(*field));
        if ((rv = bcmltm_md_pt_field_info_get(unit,
                                              table->attrib.table_id,
                                              field_array[j],
                                              &f_info)) != SHR_E_NONE) {
            break;
        }
        field->field_id = f_info.fid;
        field->field_def.name = f_info.name;
        /* Number of bits includes the min bit so add 1 */
        num_of_bits = (f_info.maxbit - f_info.minbit) + 1;
        if (num_of_bits == 1) {
            field->field_def.dtag = BCMLT_FIELD_DATA_TAG_BOOL;
        } else if (num_of_bits <= 8) {
            field->field_def.dtag = BCMLT_FIELD_DATA_TAG_U8;
        } else if (num_of_bits <= 16) {
            field->field_def.dtag = BCMLT_FIELD_DATA_TAG_U16;
        } else if (num_of_bits <= 32) {
            field->field_def.dtag = BCMLT_FIELD_DATA_TAG_U32;
        } else {
            field->field_def.dtag = BCMLT_FIELD_DATA_TAG_U64;
        }
        field->field_def.width = num_of_bits;
        field->field_def.key = f_info.flags & BCMDRD_SYMBOL_FIELD_FLAG_KEY;
        field->field_def.symbol = false;  /* PT can't have symbol */
        field->field_def.array = true;
        field->field_def.depth = 0;
        field->field_def.elements = (num_of_bits + (BCM_FIELD_SIZE_BITS - 1)) /
                                 BCM_FIELD_SIZE_BITS;
        field->field_def.min = 0;
        field->field_def.max = (num_of_bits >= BCM_FIELD_SIZE_BITS ?
                                (uint64_t)-1 :  /* Max value uint64_t */
                                (((uint64_t)1 << num_of_bits) - 1));
        field->field_def.def = 0; 
        
        field->field_def.access = BCMLT_FIELD_ACCESS_READ_WRITE;

        /* Add the field into the hash DB */
        rv = shr_hash_str_dict_insert(table->field_db,
                                 field->field_def.name,
                                 field);
        if (rv != SHR_E_NONE) {
            rv = SHR_E_MEMORY;
            break;
        }
        /* Add the field into the field list */
        field->next = table->fields;
        table->fields = field;
    }
    sal_free((void *)field_array);
    return rv;
}

/*!
 *\brief Prepare local DB fields.
 *
 * This function creates and set the given table local DB with the
 * list of the table fields. The table can be PT or LT.
 *
 * \param [in] unit Is the unit of this table.
 * \param [in] table Is a pointer to the logical table DB.
 * \param [in] num_of_fields Is the number of expected fields in the table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int local_db_fields_prep(int unit,
                                table_start_t *table,
                                size_t num_of_fields)
{
    int rv;

    /* Now create the field DB */
    rv = shr_hash_str_dict_alloc(num_of_fields*12/10, &table->field_db);
    if (SHR_E_NONE != rv) {
        return SHR_E_MEMORY;
    }

    /* Obtain the field list */
    if (table->attrib.pt) {
        rv = obtain_pt_fields(unit, table, num_of_fields);
    } else {
        rv = obtain_lt_fields(unit, table, num_of_fields);
    }
    if (0 != rv) {
        shr_hash_str_dict_free(table->field_db);
    }
    return rv;
}

static int comp_func(const void *a, const void *b)
{
    return (int)(*(int16_t *)a - *(int16_t *)b);
}

/*!
 *\brief Prepare local DB of all the logical tables of this unit.
 *
 * This function creates and set the entire local DB of all the LT under
 * the given \c unit.
 *
 * \param [in] unit Is the unit of this table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int get_lt_table_list(int unit)
{
    int rv;
    size_t num_of_tables;
    size_t actual_num;
    const char **buf;
    size_t j;
    const char **tbl_name;
    bcmlrd_table_attrib_t lattrib;
    table_start_t *table;
    uint16_t *tables_width;

    /* Need to get logical and pass through tables */
    rv = bcmlrd_table_count_get(unit, &num_of_tables);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    if (0 == num_of_tables) {
        return SHR_E_NONE;
    }
    buf = sal_alloc(sizeof(char *) * num_of_tables, "");
    if (!buf) {
        return SHR_E_MEMORY;
    }
    tables_width = sal_alloc(sizeof(uint16_t) * num_of_tables, "");
    if (!tables_width) {
        sal_free((void *)buf);
        return SHR_E_MEMORY;
    }

    rv = bcmlrd_table_names_get(unit, num_of_tables, buf,
                                &actual_num);
    if ((SHR_E_NONE != rv) || (actual_num != num_of_tables)){
        sal_free((void *)tables_width);
        sal_free((void *)buf);
        return SHR_E_NOT_FOUND;
    }
    for (j = 0,
         tbl_name = buf; j < num_of_tables;
         j++, tbl_name++) {
        rv = bcmlrd_table_attributes_get(unit, *tbl_name, &lattrib);
        if (rv != SHR_E_NONE) {
            break;
        }
        table = shr_lmm_alloc(table_elements);
        if (!table) {
            rv = SHR_E_MEMORY;
            break;
        }
        table->fields = NULL;
        table->attrib.pt = false;
        table->attrib.name = *tbl_name;
        table->attrib.table_id = lattrib.id;
        table->attrib.interactive = lattrib.interactive;
        table->attrib.num_of_fields = lattrib.number_of_fields;
        rv = shr_hash_str_dict_insert(table_db[unit].table_db,
                                       *tbl_name,
                                       (void *)table);
        if (rv != SHR_E_NONE) {
            break;
        }

        rv = local_db_fields_prep(unit, table, lattrib.number_of_fields);
        if (rv != SHR_E_NONE) {
            break;
        }

        /*
         * Determine the array size require to hold the fields by finding the
         * maximal field ID
         */
        tables_width[j] = (uint16_t)table->attrib.max_fid + 1;

        /* Add the table to the table list */
        table->next = table_db[unit].tables;
        table_db[unit].tables = table;
    }

    if (rv == SHR_E_NONE) {
        /* Sort the table width so we can find the median value. */
        sal_qsort(tables_width, num_of_tables, sizeof(uint16_t), comp_func);

        table_db_width[unit].max_size = tables_width[num_of_tables - 1];
        table_db_width[unit].mid_size = tables_width[num_of_tables/2];

        /* Allocate buffers for max and median size tables */
        if (shr_lmm_init(FIELD_ARRAY_CHUNK_SIZE,
                         sizeof(void *) * (table_db_width[unit].max_size + 1),
                         sizeof(void *) * table_db_width[unit].max_size,
                         true,
                         &table_db_width[unit].max_size_hdl) != SHR_E_NONE) {
            rv = SHR_E_MEMORY;
        } else if (shr_lmm_init(FIELD_ARRAY_CHUNK_SIZE,
                         sizeof(void *) * (table_db_width[unit].mid_size + 1),
                         sizeof(void *) * table_db_width[unit].mid_size,
                         true,
                         &table_db_width[unit].mid_size_hdl) != SHR_E_NONE) {
            rv = SHR_E_MEMORY;
        }
    }


    sal_free(tables_width);
    sal_free((void *)buf);

    return rv;
}


/*!
 *\brief Prepare local DB of all the physical tables of this unit.
 *
 * This function creates and set the entire local DB of all the PT under
 * the given \c unit.
 *
 * \param [in] unit Is the unit of this table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int get_pt_table_list(int unit)
{
    int rv;
    table_start_t *table;
    size_t num_of_tables;
    size_t actual_num;
    uint8_t *buf;
    size_t j;
    bcmdrd_sid_t *table_id;
    bcmdrd_sym_info_t info;

    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &num_of_tables);
    if (SHR_E_NONE != rv) {
        return rv;
    }
    if (num_of_tables == 0) {
        return 0;
    }

    buf = sal_alloc(sizeof(bcmdrd_sid_t *) * num_of_tables, "");
    if (!buf) {
        return SHR_E_MEMORY;
    }
    rv = bcmdrd_pt_sid_list_get(unit, num_of_tables,
                                (bcmdrd_sid_t *)buf, &actual_num);
    if (SHR_FAILURE(rv) || actual_num != num_of_tables) {
        sal_free(buf);
        return SHR_E_NOT_FOUND;
    }

    for (j = 0,
         table_id = (bcmdrd_sid_t *)buf; j < num_of_tables;
         j++, table_id++) {

        /* Verify that the sid is of a valid table */
        if (!bcmdrd_pt_is_valid(unit, *table_id)) {
            continue;
        }

        rv = bcmdrd_pt_info_get(unit, *table_id, &info);
        if (SHR_FAILURE(rv)) {
            break;
        }

        table = shr_lmm_alloc(table_elements);
        if (!table) {
            rv = SHR_E_MEMORY;
            break;
        }
        table->fields = NULL;
        table->attrib.pt = true;
        table->attrib.name = info.name;
        table->attrib.table_id = *table_id;
        table->attrib.interactive = false;
        table->attrib.num_of_fields = 0;
        rv = shr_hash_str_dict_insert(table_db[unit].table_db,
                                       info.name,
                                       (void *)table);
        if (rv != SHR_E_NONE) {
            break;
        }
        /* Add the table to the table list */
        table->next = table_db[unit].tables;
        table_db[unit].tables = table;
    }
    sal_free(buf);
    return rv;
}

/*!
 *\brief Updates local table DB with all the tables under this unit.
 *
 * This function obtains the table lists from lrd and drd and establishes
 * a single DB for all tables under this unit.
 * This function is not thread safe and therefore the caller should take
 * care of multiple threads issues.
 *
 * \param [in] unit Is the unit of this table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int table_db_update(int unit)
{
    int rv;
    if ((rv = get_lt_table_list(unit)) != 0) {
        return rv;
    }
    if ((rv = get_pt_table_list(unit)) != 0) {
        return rv;
    }
    return SHR_E_NONE;
}

/*!
 *\brief Search for table that was created on the fly (dynamic).
 *
 * This function being invoked to retrieve table information for tables that
 * were dynamically created. These tables will not be found in the DB that
 * was statically initialized during initialization. If the table is found
 * it will be added into the local table DB for future references.
 *
 * \param [in] unit Is the unit of this table.
 * \param [in] table_name Is the string identify the table.
 * \param [out] new_table Is pointing into a new table object that was
 * created after the table was detected.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int search_for_dynamic_tbl(int unit,
                                  const char *table_name,
                                  table_start_t **new_table)
{
    int rv;
    bcmlrd_table_attrib_t lattrib;
    table_start_t *table = NULL;
    void *p_tbl;

    sal_rwlock_wlock (table_db[unit].rwlock, SAL_RWLOCK_FOREVER);
    do {
        /*
         * repeat the search. If multiple thread were waiting then the
         * first one would create the table while the others can succeed
         * finding it
         */
        rv = shr_hash_str_dict_lookup (table_db[unit].table_db,
                                       table_name,
                                       &p_tbl);
        if (rv == 0) {
            table = (table_start_t *)p_tbl;
            break;
        }

        /* Dynamic tables can only be in LRD */
        if (bcmlrd_table_attributes_get(unit, table_name, &lattrib) != 0) {
            /* If here the table was not found */
            rv = SHR_E_PARAM;
            break;
        }

        table = shr_lmm_alloc(table_elements);
        if (!table) {
            rv = SHR_E_MEMORY;
            break;
        }
        table->fields = NULL;
        table->attrib.pt = false;
        table->attrib.num_of_fields = lattrib.number_of_fields;
        table->attrib.name = table_name;
        table->attrib.table_id = lattrib.id;
        table->attrib.interactive = lattrib.interactive;

        rv = shr_hash_str_dict_alloc(lattrib.number_of_fields * 12 / 10,
                                     &table->field_db);
        if (0 != rv) {
            shr_lmm_free(table_elements, (void *)table);
            break;
        }
        /* Add the table to the table list */
        table->next = table_db[unit].tables;
        table_db[unit].tables = table;
    } while (0);

    sal_rwlock_give(table_db[unit].rwlock);
    *new_table = table;
    return rv;
}

/*!
 *\brief Retrieve all the fields of a table into its local DB.
 *
 * This function retrieve the fields attribute of a given table. The
 * table can be logical or physical table.
 *
 * \param [in] unit Is the unit of this table.
 * \param [in] table Is the table structure where the field DB should be added.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int table_fields_retrieve(int unit, table_start_t *table)
{
    size_t num_of_fields;
    int rv;

    /* Obtain write lock to the table DB */
    sal_rwlock_wlock(table_db[unit].rwlock, SAL_RWLOCK_FOREVER);
    /* Check that no fields even after we took the write lock */
    rv = 0;
    if (table->fields == NULL) {
        if (table->attrib.pt) {
            rv = bcmltm_md_pt_field_count_get(unit,
                                        table->attrib.table_id,
                                        &num_of_fields);
        } else {
            rv = bcmlrd_field_count_get(unit,
                                        table->attrib.table_id,
                                        &num_of_fields);
        }

        if ((0 == rv) && (num_of_fields > 0)) {
            table->attrib.num_of_fields = num_of_fields;
            rv = local_db_fields_prep(unit, table, num_of_fields);
        }
    }
    sal_rwlock_give(table_db[unit].rwlock);
    return rv;
}

/*!
 *\brief Search for the next table that matches the desired attributes.
 *
 * This function scan through the tables (utilizing the hash string functionality)
 * until it finds a table with matching attributes to the input flags.
 *
 * \param [in] unit Is the unit of this table.
 * \param [in] flags Are attribute that can be use to restrict the search
 * output.
 * \param [out] table Is a point to the table that matches the search critiria.
 * \param [in] rv Is the result of the previous search with
 * \c shr_hash_str_get_next() as it performed by the caller. If this seach
 * fails there is no reason to continue.
 * \param [out] name Is the matched table name.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int search_for_attrib(int unit,
                             uint32_t flags,
                             table_start_t **table,
                             int rv,
                             char **name)
{
    while (rv == SHR_E_NONE) {
        if (((*table)->attrib.pt && (flags & BCMLT_TABLE_SEARCH_PT)) ||
            (!(*table)->attrib.pt && (flags & BCMLT_TABLE_SEARCH_LT)) ||
            ((*table)->attrib.interactive &&
              (flags & BCMLT_TABLE_SEARCH_INTERACTIVE))) {
            break;
        }
        rv = shr_hash_str_get_next(table_db[unit].table_db, name, (void **)table);
    }
    return rv;
}


/*******************************************************************************
 * External functions local to this module
 */
int bcmlt_db_init()
{
    int rv;
    /* Init the field elements pool */
    LMEM_MGR_INIT(field_struct_t,
                  field_elements,
                  FIELD_ALLOC_CHUNK_SIZE,
                  true,
                  rv);
    if (rv != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }

    /* Init the table element pool */
    LMEM_MGR_INIT(table_start_t,
                  table_elements,
                  TABLE_ALLOC_CHUNK_SIZE,
                  true,
                  rv);
    if (rv != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }
    return SHR_E_NONE;
}

int bcmlt_db_delete()
{
    shr_lmm_delete(table_elements);
    shr_lmm_delete(field_elements);
    return SHR_E_NONE;
}

int bcmlt_tables_db_init (int unit)
{
    int rv;
    size_t num_of_ltbls;
    size_t num_of_ptbls;

    rv = bcmlrd_table_count_get(unit, &num_of_ltbls);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    rv =  bcmdrd_pt_sid_list_get(unit, 0, NULL, &num_of_ptbls);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    table_db[unit].rwlock = sal_rwlock_create("");
    if (!table_db[unit].rwlock) {
        return SHR_E_MEMORY;
    }
    if (num_of_ltbls + num_of_ptbls == 0) {
        return SHR_E_INIT;
    }
    if (0 != shr_hash_str_dict_alloc(num_of_ltbls + num_of_ptbls,
                                     &table_db[unit].table_db)) {
        return SHR_E_MEMORY;
    }
    table_db[unit].unit = unit;
    table_db[unit].tables = NULL;

    return table_db_update(unit);
}

int bcmlt_tables_db_delete (int unit)
{
    field_struct_t *field;
    table_start_t *table;

    if (!table_db[unit].rwlock) {
        return SHR_E_UNIT;
    }
    /* Clean all the tables */
    while (table_db[unit].tables) {
        table = table_db[unit].tables;
        table_db[unit].tables = table->next;
        /* Clean all the fields */
        while (table->fields) {
            field = table->fields;
            table->fields = field->next;
            /* Free the field element to the free list */
            shr_lmm_free(field_elements, field);
        }
        /* Release the field DB */
        shr_hash_str_dict_free(table->field_db);
        /* Free the table element to the free list */
        shr_lmm_free (table_elements, table);
    }

    /* Release the table fields buffers */
    if (table_db_width[unit].max_size_hdl) {
        table_db_width[unit].max_size = 0;
        shr_lmm_delete(table_db_width[unit].max_size_hdl);
        table_db_width[unit].max_size_hdl = 0;
    }
    if (table_db_width[unit].mid_size_hdl) {
        table_db_width[unit].mid_size = 0;
        shr_lmm_delete(table_db_width[unit].mid_size_hdl);
        table_db_width[unit].mid_size_hdl = 0;
    }

    sal_rwlock_destroy(table_db[unit].rwlock);
    table_db[unit].rwlock = NULL;
    shr_hash_str_dict_free(table_db[unit].table_db);
    table_db[unit].table_db = NULL;
    return SHR_E_NONE;
}

int bcmlt_db_table_info_get(int unit,
                            const char *table_name,
                            bcmlt_table_attrib_t **attrib,
                            shr_lmm_hdl_t *fld_array_hdl,
                            void **hdl)
{
    int rv;
    table_start_t *table;
    void *p_tbl;

    if (!table_db[unit].rwlock) {
        return SHR_E_INIT;
    }
    /* Search local DB */
    sal_rwlock_rlock(table_db[unit].rwlock, SAL_RWLOCK_FOREVER);
    rv = shr_hash_str_dict_lookup (table_db[unit].table_db,
                                   table_name,
                                   &p_tbl);
    sal_rwlock_give(table_db[unit].rwlock);
    table = (table_start_t *)p_tbl;
    if (rv == 0) {
        *attrib = &table->attrib;
        *hdl = (void *)table;
    } else {
        /*
         * if the table was created at runtime it will not be in the db.
         * Search for it in the different DB to see if it is there
         */
        rv = search_for_dynamic_tbl(unit, table_name, &table);
        if (rv == 0) {
            *attrib = &table->attrib;
            *hdl = (void *)table;
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Can't find table %s\n"),
                         table_name));
        }
    }

    if (rv == 0 && fld_array_hdl) {
        if (table->attrib.pt) {  /* PT field IDs are not consequtive */
            *fld_array_hdl = 0;  /* So don't bother to allocate array. */
        } else {
            if (table->attrib.max_fid + 1 <= table_db_width[unit].mid_size) {
                *fld_array_hdl = table_db_width[unit].mid_size_hdl;
            } else if (table->attrib.max_fid + 1 <=
                       table_db_width[unit].max_size){
                *fld_array_hdl = table_db_width[unit].max_size_hdl;
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Unsuported table width %d\n"),
                           table->attrib.num_of_fields));
            }
        }
    }

    return rv;
}


void bcmlt_db_table_keys_get(int unit,
                             void *hdl,
                             uint32_t array_size,
                             uint32_t *keys,
                             uint32_t *num_of_keys)
{
    table_start_t *table = (table_start_t *)hdl;
    field_struct_t *field;
    uint32_t idx = 0;

    if (table == NULL || table->fields == NULL || !table_db[unit].rwlock) {
        *num_of_keys = 0;
        return;
    }

    field = table->fields;
    while (field) {
        if (field->field_def.key) {
            if (idx < array_size) {
                keys[idx++] = field->field_id;
            } else {
                idx++;
            }
        }
        field = field->next;
    }
    *num_of_keys = idx;
}

int bcmlt_db_field_info_get(int unit,
                          const char *name,
                          void *hdl,
                          bcmlt_field_def_t **f_attr,
                          uint32_t *field_id)
{
    int rv;
    table_start_t *table = (table_start_t *)hdl;
    field_struct_t *field;
    void *p_fld;

    if (table == NULL) {
        return SHR_E_INTERNAL;
    }
    if (!table_db[unit].rwlock) {
        return SHR_E_UNIT;
    }

    /*
     * During init we are starting the list of tables, but not the
     * fields for each table. So the first time the application calls
     * this table it will find that the field list is empty.
    */
    if (table->fields == NULL) {
        /* Need to obtain the fields for this table */
        rv = table_fields_retrieve(unit, table);
        if (rv != 0) {
            return SHR_E_PARAM;
        }
    }
    /* At this point we know that the fields are there. Obtain read lock */
    sal_rwlock_rlock(table_db[unit].rwlock, SAL_RWLOCK_FOREVER);
    rv = shr_hash_str_dict_lookup (table->field_db,
                                   name,
                                   &p_fld);
    sal_rwlock_give(table_db[unit].rwlock);
    field = (field_struct_t *)p_fld;
    if (rv == 0) {
        if (f_attr) {
            *f_attr = &field->field_def;
        }
        if (field_id) {
            *field_id = field->field_id;
        }
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "field %s is not part of table %s\n"),
                     name, table->attrib.name));
    }
    return rv;
}

int bcmlt_db_table_name_get_first(int unit, uint32_t flags, char **name)
{
    int rv;
    table_start_t *table;
    void *p_tbl;

    rv = shr_hash_str_get_first(table_db[unit].table_db, name, &p_tbl);
    if (rv != 0) {
        return SHR_E_NOT_FOUND;
    }

    table = (table_start_t *)p_tbl;
    return search_for_attrib(unit, flags, &table, rv, name);
}

int bcmlt_db_table_name_get_next(int unit, uint32_t flags, char **name)
{
    int rv;
    table_start_t *table;
    void *p_tbl;

    rv = shr_hash_str_get_next(table_db[unit].table_db, name, &p_tbl);
    if (rv != 0) {
        return SHR_E_NOT_FOUND;
    }
    table = (table_start_t *)p_tbl;
    return search_for_attrib(unit, flags, &table, rv, name);
}

int bcmlt_db_table_field_defs_get(int unit,
        const char *table_name,
        uint32_t    fields_array_size,
        bcmlt_field_def_t *field_defs_array,
        uint32_t *require_array_size)
{
    int rv;
    table_start_t *table;
    field_struct_t *field;
    bcmlt_field_def_t *array;
    void *p_tbl;


    /* Search local DB */
    sal_rwlock_rlock(table_db[unit].rwlock, SAL_RWLOCK_FOREVER);
    rv = shr_hash_str_dict_lookup (table_db[unit].table_db,
                                   table_name,
                                   &p_tbl);
    sal_rwlock_give(table_db[unit].rwlock);
    if (rv != 0) {
        return SHR_E_PARAM;
    }

    table = (table_start_t *)p_tbl;
    if (table->fields == NULL) {
        /* Need to obtain the fields for this table. Obtain write lock */
        rv = table_fields_retrieve(unit, table);
        if (SHR_E_NONE != rv) {
            return rv;
        }
    }

    /* If insufficient space update desired space and return */
    if (fields_array_size < table->attrib.num_of_fields) {
        if (!require_array_size) {
            return SHR_E_PARAM;
        }
        *require_array_size = table->attrib.num_of_fields;
        return SHR_E_NONE;
    }
    /* Copy the information into the array */
    for (field = table->fields, array = field_defs_array;
          field;
          field = field->next, array++) {
        sal_memcpy(array, &field->field_def, sizeof(*array));
    }
    if (require_array_size) {
        *require_array_size = table->attrib.num_of_fields;
    }
    return SHR_E_NONE;
}

uint32_t bcmlt_db_interact_tbl_max_fields(int unit)
{
    if (unit < 0 || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return 0;
    }
    return table_db_interact_tbl_max_fields[unit];
}
