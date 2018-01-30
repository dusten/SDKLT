/*! \file bcmcfg_read_device.c
 *
 * Device configuration handler. This uses the device configuration
 * schema to interpret the YAML event stream and populate the software
 * configuration data structures. The device configuration schema is
 * essentially the logical table definition.
 *
 * If the logical table is a "configuration table" (table_type:
 * config), then configuration data is saved to a High Availability
 * memory region, otherwise the data is saved in a temporary storage
 * area for BCMLTP (logical table playback).
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
#include <shr/shr_error.h>
#include <shr/shr_fmm.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/bcmcfg_player.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd_config.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/* Maximum number of elements allowed for a wide field. */
#define MAX_ELEMENTS 8

/* The PT values are from LTM - need to be made available for CFG. */

/*! Index Key field, used for memories or array registers. */
#define BCMCFG_PT_FIELD_KEY_INDEX  ((bcmdrd_fid_t)-1)

/*! Port Key field, used for port-based registers. */
#define BCMCFG_PT_FIELD_KEY_PORT   ((bcmdrd_fid_t)-2)

/*!
 * \brief Kind of table in descriptor.
 */
typedef enum bcmcfg_table_kind_e {
    /*! Unknown table. */
    BCMCFG_TBL_UNKNOWN,

    /*! Config logical table. */
    BCMCFG_TBL_CONFIG,

    /*! Standard Logical table. */
    BCMCFG_TBL_LOGICAL,

    /*! Physical table. */
    BCMCFG_TBL_PHYSICAL,

    /*! Config table as constraint. */
    BCMCFG_TBL_CONSTRAINT,

} bcmcfg_table_kind_t;

/*!
 * \brief Logical table parsing data.
 *
 * Logical table information that is tracked per-level.
 */
typedef struct bcmcfg_tbl_info_s {
    /*! device parser state */
    enum {
        /*! Unit - can transition to table or parent */
        UNIT,

        /*! Table - can transition to field or parent */
        TABLE,

        /*! Field - can transition to array or parent */
        FIELD,

        /*! Array - can transition to parent only */
        ARRAY
    } state;

    /*! Unit - true if unit in the set */
    bool unit_set[BCMDRD_CONFIG_MAX_UNITS];

    /*! Table kind. */
    bcmcfg_table_kind_t kind;

    /*! Logical Table representation. */
    const bcmltd_table_rep_t *lt;

    /*! Table Name. */
    const char *name;

    /*! True if SID is valid. */
    bool sid_valid;

    /*! Table ID. */
    uint32_t sid;

    /*! Field ID. */
    uint32_t fid;

    /*! Implicit field ID. */
    bool ifield;

    /*! Mapped Field ID. */
    uint32_t mfid;

    /*! Mapped Field value. */
    uint64_t mval;

    /*! Array index. */
    uint32_t aidx;

    /*! Physical table unit. */
    int pt_unit;

} bcmcfg_tbl_info_t;

/*!
 * \brief Logical table parser user data.
 */
typedef struct bcmcfg_tbl_user_s {
    /*! Reader stack. */
    bcmcfg_tbl_info_t info[BCMCFG_MAX_LEVEL];

    /*! Config Table Data. */
    uint64_t *config[BCMDRD_CONFIG_MAX_UNITS][BCMLTD_TABLE_COUNT];

    /*! Table playback list head. */
    bcmcfg_playback_list_t *head;

    /*! Table playback list tail. */
    bcmcfg_playback_list_t *tail;

    /*! Append entries to table if true. */
    bool append;

} bcmcfg_tbl_user_t;

/* User data passed to handler. */
static bcmcfg_tbl_user_t bcmcfg_tbl_user;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Parse a string to a uint32.
 *
 * Parse a string up a uint64_t word.
 *
 * \param [in]     str          String.
 * \param [out]    valuep       Pointer to output value.
 * \param [in]     base         Numeric base.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_str_to_uint32(const char *str, uint32_t *valuep, int base)
{
    uint32_t value;
    char *end;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    value = sal_strtoul(str, &end, base);
    if (*str && *end) {
        /* Invalid conversion */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unable to convert %s\n"), str));
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    } else {
        *valuep = value;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse a string to a uint64.
 *
 * Parse a string up a uint64_t word.
 *
 * \param [in]     str          String.
 * \param [out]    valuep       Pointer to output value.
 * \param [in]     base         Numeric base.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_str_to_uint64(const char *str, uint64_t *valuep, int base)
{
    uint64_t value;
    char *end;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    value = (uint64_t)sal_strtoull(str, &end, base);
    if (*str && *end) {
        /* Invalid conversion */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Unable to convert %s\n"), str));
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    } else {
        *valuep = value;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse a hex integer string.
 *
 * Parse a hex integer string up to 'size' number of uint64_t words.
 *
 * \param [in]     str          String.
 * \param [out]    fval         Pointer to output value array.
 * \param [in]     size         Size of output value array.
 *
 * TODO - eliminate strdup and free, handle conversion errors *
 * pointer count argument reversed from normal
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse_hex(const char *str, uint64_t *fval, size_t size, size_t *actual)
{
    size_t cnt = 0;
    char buf[17];       /* Conversion buffer. */
    int n;              /* Length of string to convert. */
    int m;              /* Offset of string to convert. */
    int len;            /* Number of characters to convert. */

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {
        if (sal_strncmp(str, "0x", 2)) {
            /* String must start with '0x' */
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }
        /* Ignore the leading 0x */
        str += 2;
        n = sal_strlen(str);
        m = 0;
        len = 16;

        if (n > len) {
            /*
             * Multiple pieces to convert, so start at the rightmost
             * 16 bytes.
             */
            m = n - len;
        } else {
            len = n;
        }

        while (n > 0 && cnt < size) {
            sal_memcpy(buf, str + m, len);
            buf[len] = 0;
            SHR_IF_ERR_CONT(bcmcfg_str_to_uint64(buf, fval + cnt, 16));
            if (SHR_FUNC_ERR()) {
                break;
            }

            cnt++;
            n -= len; /* Reduce number of characters converted. */
            if (n < len) {
                len = n;
            }
            m -= len; /* Move offset to the left. */
        }

    } while (0);

    if (!SHR_FUNC_ERR() && cnt > 0) {
        *actual = cnt;
    } else {
        SHR_IF_ERR_CONT(SHR_E_PARAM);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Search for physical table.
 *
 * Search for a physical table by name.
 *
 * \param [in,out] tbl_info     LT data.
 * \param [in]     key          Map key.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_pt_search(bcmcfg_tbl_info_t *tbl_info,
                      const char *key)
{
    int unit;
    uint32_t ptsid;
    bool assigned = false;
    int rv;
    const char *name;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!tbl_info->unit_set[unit]) {
            continue;
        }

        rv = bcmdrd_pt_name_to_sid(unit, key, &ptsid);

        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        if (!assigned) {
            tbl_info->pt_unit = unit;
            tbl_info->sid = ptsid;
            /* Lookup the name to avoid a memory allocation for only
               physical table names. */
            name = bcmdrd_pt_sid_to_name(unit, ptsid);
            if (name == NULL) {
                /* If the reverse lookup to name fails, then something
                 is seriously wrong. */
                SHR_IF_ERR_CONT(SHR_E_INTERNAL);
                break;
            }
            tbl_info->name = name;
            assigned = true;
        } else if (tbl_info->sid != ptsid) {
            /* PT sid inconsistent across units. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Physical table %s SID "
                                "is inconsistent across units\n"), key));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search for a map key.
 *
 * Search for a map key in either the list of logical tables, or the
 * list of physical tables. If the key is found, set the table
 * kind and table SID corresponding to the found key. If the key is
 * not found, return SHR_E_NOT_FOUND.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     key          Map key.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_search(bcmcfg_tbl_info_t *tbl_info,
                          const char *key)
{
    int idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    idx = bcmltd_table_name_to_idx(key);
    if (idx != SHR_E_NOT_FOUND) {
        tbl_info->sid = (bcmltd_sid_t)idx;
        /* Won't know until playback if non-config table is mapped to
           this unit... */
        tbl_info->lt = bcmltd_table_get(tbl_info->sid);
        if (tbl_info->lt == NULL) {
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        } else {
            tbl_info->name = tbl_info->lt->name;
            tbl_info->kind =
                (tbl_info->lt->flags & BCMLTD_TABLE_F_CONFIG) ?
                BCMCFG_TBL_CONFIG : BCMCFG_TBL_LOGICAL;
            tbl_info->sid_valid = true;
        }
    } else if (SHR_SUCCESS(bcmcfg_read_pt_search(tbl_info, key))) {
        tbl_info->kind = BCMCFG_TBL_PHYSICAL;
        tbl_info->sid_valid = true;
    } else {
        SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup for table entries.
 *
 * \param [in,out] info         Reader Data
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_table_setup(bcmcfg_read_level_info_t *info,
                               bcmcfg_tbl_user_t *user,
                               bcmcfg_tbl_info_t *tbl_info)
{
    bcmcfg_tbl_list_t **list = NULL;
    bcmcfg_tbl_list_t *tbl = NULL;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    switch (tbl_info->kind) {
    case BCMCFG_TBL_LOGICAL:
        list = &user->tail->lt;
        break;
    case BCMCFG_TBL_PHYSICAL:
        list = &user->tail->pt;
        break;
    case BCMCFG_TBL_CONFIG:
        /* No setup needed. */
        break;
    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        break;
    }

    if (list) {
        tbl = sal_alloc(sizeof(*tbl), "bcmcfg_read_device_table_setup");
        if (tbl) {
            sal_memset(tbl, 0, sizeof(*tbl));
            tbl->sid = tbl_info->sid;
            tbl->name = tbl_info->name;
            tbl->line = info->line;
            tbl->column = info->column;
            sal_memcpy(tbl->unit_set,
                       tbl_info->unit_set,
                       sizeof(tbl->unit_set));
            tbl->next = *list;
            *list = tbl;
            user->append = true;
        } else {
            SHR_IF_ERR_CONT(SHR_E_MEMORY);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Test if field array.
 *
 * Return true if the current field is a field array.
 *
 * \param [in]     tbl_info     Local LT data.
 * \param [out]    tf           Boolean flag..
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_is_field_array(const bcmcfg_tbl_info_t *tbl_info, bool *tf)
{
    const bcmltd_field_rep_t *field;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {
        *tf = false;

        if (tbl_info->fid >= tbl_info->lt->fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field ID is out of range.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
        }

        field = tbl_info->lt->field + tbl_info->fid;

        /* Field width has to be scalar. */
        if (field->width > BCM_FIELD_SIZE_BITS) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field is too wide.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
        }

        /* Is array if it has any depth. */
        if (field->depth > 0) {
            *tf = true;
        }

    } while (0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by number.
 *
 * Add a unit to the set of units with table assignments.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     unit         Unit number.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit(bcmcfg_tbl_info_t *tbl_info, int unit)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Add a unit */
    if (bcmdrd_dev_exists(unit)) {
        tbl_info->unit_set[unit] = true;
    } else {
        SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add all units.
 *
 * Add all units that currently exist.
 *
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_all_units(bcmcfg_tbl_info_t *tbl_info)
{
    int unit;
    bcmdrd_dev_t *dev;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        dev = bcmdrd_dev_get(unit);
        if (!dev) {
            continue;
        }

        SHR_IF_ERR_CONT(bcmcfg_read_device_add_unit(tbl_info, unit));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add unit by name.
 *
 * Add all units that correspond to the given name.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Unit name.
 * \param [out]    found        Name found flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit_name(bcmcfg_tbl_info_t *tbl_info,
                                 const char *str,
                                 bool *found)
{
    int len;
    int unit;
    int rv;
    bcmdrd_dev_t *dev;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    len = sal_strlen(str);
    *found = false;
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {

        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        dev = bcmdrd_dev_get(unit);
        if (!dev) {
            continue;
        }

        if (!sal_strncmp(dev->name, str, len)) {
            rv = bcmcfg_read_device_add_unit(tbl_info, unit);
            if (SHR_SUCCESS(rv)) {
                *found = true;
            } else {
                SHR_IF_ERR_CONT(rv);
                break;
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by numeric string.
 *
 * Add a unit indicated by a numeric string.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Unit number string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit_num(bcmcfg_tbl_info_t *tbl_info, const char *str)
{
    uint32_t value;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_CONT(bcmcfg_str_to_uint32(str, &value, 0));
    if (!SHR_FUNC_ERR()) {
        SHR_IF_ERR_CONT(bcmcfg_read_device_add_unit(tbl_info, value));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by string.
 *
 * Add a unit to the set of units by string.
 *
 * The string may be an integer or a device name.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Unit string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit_str(bcmcfg_tbl_info_t *tbl_info, const char *str)
{
    int rv;
    bool found = false;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!sal_strcmp(str, "*")) {
        SHR_IF_ERR_CONT(bcmcfg_read_device_add_all_units(tbl_info));
    } else {
        rv = bcmcfg_read_device_add_unit_name(tbl_info, str, &found);
        if (SHR_SUCCESS(rv) && !found) {
            rv = bcmcfg_read_device_add_unit_num(tbl_info, str);
        }
        SHR_IF_ERR_CONT(rv);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup for config field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * Setup configuration table data.
 *
 * This must eventually be allocated from HA memory.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_setup_cfg_field(bcmcfg_tbl_user_t *user,
                                   bcmcfg_tbl_info_t *tbl_info)
{
    size_t size = 0;
    size_t i;
    uint64_t *data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Calculate allocation size. Config tables do not have wide
       fields or arrays, so this is simply the number of fields times
       the size of a scalar value. */
    size = tbl_info->lt->fields * sizeof(uint64_t);

    /* Allocate data for each configured unit. */
    for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
        if (tbl_info->unit_set[i] &&
            user->config[i][tbl_info->sid] == NULL) {

            data = sal_alloc(size, "bcmcfg_read_device_setup_cfg_field");
            sal_memset(data, 0, size);
            if (data) {
                sal_memset(data, 0, size);
                user->config[i][tbl_info->sid] = data;
            } else {
                SHR_IF_ERR_CONT(SHR_E_MEMORY);
                break;
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle implicit key field.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Value string.
 *
 * Handle a implicit key, where the key value is used as a YAML map key.
 *
 * Table must be an index table, and mfid must be within range of the
 * fields, and indicate a non-enum key with a width less than or equal
 * to 64 bits. 'str' must be numeric. Note that an enum values are not
 * supported at this time.
 * Setup configuration table data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_implicit_key(bcmcfg_tbl_info_t *tbl_info,
                                const char *str)
{
    const bcmltd_field_rep_t *field;
    uint64_t value;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {

        /* Has to be an index table. */
        if ((tbl_info->lt->flags & BCMLTD_TABLE_F_TYPE_MASK) !=
            BCMLTD_TABLE_F_TYPE_INDEX) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Not an index table.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
        }

        /* Implicit field ID has to be in range. */
        if (tbl_info->mfid >= tbl_info->lt->fields) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field ID is out of range.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
        }

        field = tbl_info->lt->field + tbl_info->mfid;

        /* Field width has to be scalar. */
        if (field->width > BCM_FIELD_SIZE_BITS) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field is too wide.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
        }

        /* Field cannot be an enum. */
        if (field->flags & BCMLTD_FIELD_F_ENUM) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field cannot be an enum.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
        }

        SHR_IF_ERR_CONT(bcmcfg_str_to_uint64(str, &value, 0));
        /* Field value has to be a valid number. */
        if (SHR_FUNC_ERR()) {
            /* invalid conversion */
            break;
        }

        /* Met all the requirements for implicit field assignment. */
        tbl_info->ifield = true;
        tbl_info->fid = tbl_info->mfid++;
        tbl_info->mval = value;
    } while (0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup logical table field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field string.
 *
 * Lookup a field string in the current table. If not found, assume
 * it may be a field value for an implicit key. If the key is found
 * for a config table, set up the config data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_lookup_lt_field(bcmcfg_tbl_user_t *user,
                                   bcmcfg_tbl_info_t *tbl_info,
                                   const char *str)
{
    size_t i;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (i = 0; i < tbl_info->lt->fields; i++) {
        if (!sal_strcmp(str, tbl_info->lt->field[i].name)) {
            tbl_info->fid = i;
            break;
        }
    }

    if (i == tbl_info->lt->fields) {
        SHR_IF_ERR_CONT(bcmcfg_read_device_implicit_key(tbl_info, str));
    } else if (tbl_info->kind == BCMCFG_TBL_CONFIG) {
        SHR_IF_ERR_CONT(bcmcfg_read_device_setup_cfg_field(user,
                                                        tbl_info));
    }
    /* Non-config fields do not require any setup. */

    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup physical table field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field string.
 *
 * Lookup a field string in the current physical table.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_lookup_pt_field(bcmcfg_tbl_user_t *user,
                                   bcmcfg_tbl_info_t *tbl_info,
                                   const char *str)
{
    int unit;
    uint32_t ptfid = 0;
    bool assigned = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!sal_strcmp(str, "__INDEX")) {
        ptfid = BCMCFG_PT_FIELD_KEY_INDEX;
    } else if (!sal_strcmp(str, "__PORT")) {
        ptfid = BCMCFG_PT_FIELD_KEY_PORT;
    }

    if (ptfid == 0) {
        for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            if (!tbl_info->unit_set[unit]) {
                continue;
            }
            SHR_IF_ERR_CONT(bcmdrd_pt_field_name_to_fid(unit,
                                                        tbl_info->sid,
                                                        str,
                                                        &ptfid));
            if (!assigned) {
                tbl_info->fid = ptfid;
                assigned = true;
            } else if (tbl_info->fid != ptfid) {
                /* PT fid inconsistent across units. */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Physical table FID %s "
                                    "is inconsistent across units\n"), str));
                SHR_IF_ERR_CONT(SHR_E_PARAM);
                break;
            }
        }
    } else {
        tbl_info->fid = ptfid;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field string.
 *
 * Lookup a field string in the current table logical or physical
 * table.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_lookup_field(bcmcfg_tbl_user_t *user,
                                bcmcfg_tbl_info_t *tbl_info,
                                const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    switch (tbl_info->kind) {
    case BCMCFG_TBL_CONFIG:
    case BCMCFG_TBL_LOGICAL:
        SHR_IF_ERR_CONT(bcmcfg_read_device_lookup_lt_field(user, tbl_info, str));
        break;
    case BCMCFG_TBL_PHYSICAL:
        SHR_IF_ERR_CONT(bcmcfg_read_device_lookup_pt_field(user, tbl_info, str));
        break;
    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Link field.
 *
 * \param [in,out] tbl          Playback table data.
 * \param [in]     fid          Field ID.
 * \param [in]     idx          Field array index.
 * \param [in]     value        Field value.
 *
 * Append a single field record to the current table playback list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_link_field(bcmcfg_tbl_list_t *tbl,
                              uint32_t fid,
                              uint32_t idx,
                              uint64_t value)
{
    shr_fmm_t *field;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    field = sal_alloc(sizeof(*field), "bcmcfg_read_device_link_field");

    if (field) {
        sal_memset(field, 0, sizeof(*field));
        field->id = fid;
        field->data = value;
        field->idx = idx;
        field->next = tbl->field;
        tbl->field = field;
    } else {
        SHR_IF_ERR_CONT(SHR_E_MEMORY);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Link fields.
 *
 * \param [in,out] tbl          Playback table data.
 * \param [in]     fid          Field ID.
 * \param [in]     count        Number of elements in field array.
 * \param [in]     value        Field array.
 *
 * Append an array of field values to the current table playback list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_link_fields(bcmcfg_tbl_list_t *tbl,
                               uint32_t fid,
                               size_t count,
                               const uint64_t *value)
{
    size_t idx;
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (idx = 0; idx < count; idx++ ) {
        rv = bcmcfg_read_device_link_field(tbl,
                                           fid,
                                           idx,
                                           value[idx]);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set scalar fields.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     idx          Field index.
 * \param [in]     value        Field value.
 *
 * Set a scalar value of the table and field indicated in the local LT
 * data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_simple_field(bcmcfg_tbl_user_t *user,
                                    bcmcfg_tbl_info_t *tbl_info,
                                    uint32_t idx,
                                    uint64_t value)
{
    size_t i;
    uint32_t sid = tbl_info->sid;
    uint32_t fid = tbl_info->fid;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    switch (tbl_info->kind) {
    case BCMCFG_TBL_CONFIG:
        for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
            if (tbl_info->unit_set[i]) {
                user->config[i][sid][fid] = value;
            }
        }
        break;
    case BCMCFG_TBL_LOGICAL:
        SHR_IF_ERR_CONT(bcmcfg_read_device_link_field(user->tail->lt,
                                                      fid,
                                                      idx,
                                                      value));
        break;
    case BCMCFG_TBL_PHYSICAL:
        SHR_IF_ERR_CONT(bcmcfg_read_device_link_field(user->tail->pt,
                                                      fid,
                                                      idx,
                                                      value));
        break;
    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set LT enum.
 *
 * \param [in,out] user         Global LT data.
 * \param [in]     str          Enum string.
 * \param [out]    out          Enum storage index.
 *
 * Set a non-config LT field to an enum value.
 *
 * LT enums cannot be resolved until the device is mapped, so save the
 * enum string for later resolution. The field value passed via 'out'
 * is the index to this string. Resolve the enum value at playback
 * time.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_lt_enum(bcmcfg_tbl_user_t *user,
                               const char *str,
                               uint64_t *out)
{
    bcmcfg_playback_enum_t *pbenum;
    char *estr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {
        estr = sal_strdup(str);
        if (estr == NULL) {
            SHR_IF_ERR_CONT(SHR_E_MEMORY);
            break;
        }

        pbenum = sal_alloc(sizeof(*pbenum), "bcmcfg_read_device_set_lt_enum");
        if (pbenum == NULL) {
            SHR_IF_ERR_CONT(SHR_E_MEMORY);
            sal_free(estr);
            break;
        }
        sal_memset(pbenum, 0, sizeof(*pbenum));

        *out = user->tail->enum_count;

        pbenum->value = estr;

        /* Link Playback enum to playback enum list. */
        if (user->tail->enum_head == NULL) {
            user->tail->enum_head = pbenum;
        }
        if (user->tail->enum_tail) {
            user->tail->enum_tail->next = pbenum;
        }
        user->tail->enum_tail = pbenum;

        user->tail->enum_count++;
    } while (0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set scalar fields.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     idx          Field index.
 * \param [in]     str          Field value string.
 *
 * Convert the value string to an integer and set a scalar value of
 * the table and field indicated in the local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_simple_field_str(bcmcfg_tbl_user_t *user,
                                        bcmcfg_tbl_info_t *tbl_info,
                                        uint32_t idx,
                                        const char *str)
{
    const bcmltd_field_rep_t *field =
        tbl_info->lt->field + tbl_info->fid;
    uint64_t value = 0;
    int rv;
    uint32_t i;
    const bcmltd_table_rep_t *tbl;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {
        if (field->depth > 0 && idx >= field->depth) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Array index out of range.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }
        /* check for enum */
        if (field->flags & BCMLTD_FIELD_F_ENUM) {
            tbl = bcmltd_table_get(tbl_info->sid);
            if (tbl == NULL) {
                SHR_IF_ERR_CONT(SHR_E_NOT_FOUND);
                break;
            }

            if (tbl->flags & BCMLTD_TABLE_F_CONFIG) {
                field = &tbl->field[tbl_info->fid];
                for (i = 0; i < field->num_sym; i++) {
                    if (!sal_strcmp(str, field->sym[i].name)) {
                        /* Resolved! */
                        value = field->sym[i].val;
                        break;
                    }
                }

                if (i == field->num_sym) {
                    /* Symbol not found. */
                    SHR_IF_ERR_CONT(SHR_E_PARAM);
                    break;
                }
            } else {
                SHR_IF_ERR_CONT(bcmcfg_read_device_set_lt_enum(user,
                                                               str,
                                                               &value));
            }
        } else {
            SHR_IF_ERR_CONT(bcmcfg_str_to_uint64(str, &value, 0));
        }

        if (SHR_FUNC_ERR()) {
            /* If enum or scalar conversion fails. */
            break;
        }
        /* Valid conversion */

        rv = bcmcfg_read_device_set_simple_field(user,
                                                 tbl_info,
                                                 idx,
                                                 value);
        SHR_IF_ERR_CONT(rv);
    } while(0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert array index.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Index value string.
 *
 * Convert the value string to an integer and set the current array
 * index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_aidx(bcmcfg_tbl_info_t *tbl_info, const char *str)
{
    const bcmltd_field_rep_t *field =
        tbl_info->lt->field + tbl_info->fid;
    uint32_t value;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {
        SHR_IF_ERR_CONT(bcmcfg_str_to_uint32(str, &value, 0));
        if (SHR_FUNC_ERR()) {
            /* Invalid conversion */
            break;
        }

        if ((value >= field->depth) || (value != 0 && field->depth == 0)) {
            /* Index out of range for array or != 0 if not array. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Array index out of range.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }

        tbl_info->aidx = value;
    } while(0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add implicit field to table.
 *
 * Add implicit field value to table.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_implicit_field_add(bcmcfg_tbl_user_t *user,
                                      bcmcfg_tbl_info_t *tbl_info)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT(bcmcfg_read_device_set_simple_field(user,
                                                        tbl_info,
                                                        0, tbl_info->mval));
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an LT field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field value.
 *
 * Set a field value, dealing with enum, scalar and wide fields.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_lt_field_str(bcmcfg_tbl_user_t *user,
                                    bcmcfg_tbl_info_t *tbl_info,
                                    const char *str)
{
    const bcmltd_field_rep_t *field =
        tbl_info->lt->field + tbl_info->fid;
    uint32_t elements = bcmltd_field_idx_count_get(field->width);
    int rv;
    size_t count;
    uint64_t a_value[MAX_ELEMENTS];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {

        if (elements != 1 && tbl_info->kind == BCMCFG_TBL_CONFIG) {
            /* FLTG prevents this case. */
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }
        if (field->depth != 0 && tbl_info->kind == BCMCFG_TBL_CONFIG) {
            /* FLTG prevents this case. */
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
            break;
        }

        /* Cannot set a read only field of a non-config table. */
        if (tbl_info->kind != BCMCFG_TBL_CONFIG &&
            field->flags & BCMLTD_FIELD_F_READ_ONLY) {
            /* Not supported at all. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field is read-only.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }

        if (elements == 1) {
            rv = bcmcfg_read_device_set_simple_field_str(user,
                                                         tbl_info,
                                                         0,
                                                         str);
            SHR_IF_ERR_CONT(rv);
        } else if (elements <= MAX_ELEMENTS) {
            sal_memset(a_value, 0, sizeof(a_value));
            rv = bcmcfg_parse_hex(str, a_value, COUNTOF(a_value), &count);
            SHR_IF_ERR_CONT(rv);
            if (SHR_SUCCESS(rv)) {
                if (count <= elements) {
                    SHR_IF_ERR_CONT
                        (bcmcfg_read_device_link_fields(user->tail->lt,
                                                        tbl_info->fid,
                                                        elements,
                                                        a_value));
                } else {
                    /* More words than elements. */
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("Value is too wide.\n")));
                    SHR_IF_ERR_CONT(SHR_E_PARAM);
                }
            }
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Too many elements in table definition.\n")));
            /* Too many elements - not supported at all. */
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        }

    } while(0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a PT field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field value.
 *
 * Set a Physical table field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_pt_field_str(bcmcfg_tbl_user_t *user,
                                    bcmcfg_tbl_info_t *tbl_info,
                                    const char *str)
{
    uint32_t elements = 0;
    int rv;
    size_t count;
    uint64_t a_value[MAX_ELEMENTS];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {

        if ((tbl_info->fid == BCMCFG_PT_FIELD_KEY_INDEX) ||
            (tbl_info->fid == BCMCFG_PT_FIELD_KEY_PORT)) {
            elements = 1;
        } else {
            int minbit;
            int maxbit;

            minbit = bcmdrd_pt_field_minbit(tbl_info->pt_unit,
                                            tbl_info->sid,
                                            tbl_info->fid);

            if (SHR_FAILURE(minbit)) {
                SHR_IF_ERR_CONT(SHR_E_PARAM);
                break;
            }

            maxbit = bcmdrd_pt_field_maxbit(tbl_info->pt_unit,
                                            tbl_info->sid,
                                            tbl_info->fid);

            if (SHR_FAILURE(maxbit)) {
                SHR_IF_ERR_CONT(SHR_E_PARAM);
                break;
            }
            elements = bcmltd_field_idx_count_get(maxbit - minbit + 1);
        }

        if (elements <= MAX_ELEMENTS) {
            sal_memset(a_value, 0, sizeof(a_value));
            rv = bcmcfg_parse_hex(str, a_value, COUNTOF(a_value), &count);
            SHR_IF_ERR_CONT(rv);
            if (SHR_SUCCESS(rv)) {
                if (count <= elements) {
                    SHR_IF_ERR_CONT
                        (bcmcfg_read_device_link_fields(user->tail->pt,
                                                        tbl_info->fid,
                                                        elements,
                                                        a_value));
                } else {
                    /* More words than elements. */
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("Value is too wide.\n")));
                    SHR_IF_ERR_CONT(SHR_E_PARAM);
                }
            }
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Too many elements in physical table.\n")));
            /* Too many elements - not supported at all. */
            SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        }

    } while(0);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field value string.
 *
 * Set a field value for all table types.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_field(bcmcfg_tbl_user_t *user,
                             bcmcfg_tbl_info_t *tbl_info,
                             const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    switch (tbl_info->kind) {
    case BCMCFG_TBL_CONFIG:
    case BCMCFG_TBL_LOGICAL:
        SHR_IF_ERR_CONT(bcmcfg_read_device_set_lt_field_str(user,
                                                            tbl_info,
                                                            str));
        break;
    case BCMCFG_TBL_PHYSICAL:
        SHR_IF_ERR_CONT(bcmcfg_read_device_set_pt_field_str(user,
                                                            tbl_info,
                                                            str));
        break;
    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Handle a field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in]     level        Stack level.
 * \param [in]     read_state   Reader state.
 * \param [in]     str          Field string.
 *
 * Handle a field key, which may be a field name, or a field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_handle_field(bcmcfg_tbl_user_t *user,
                                int level,
                                bcmcfg_read_state_t read_state,
                                const char *str)
{
    bcmcfg_tbl_info_t *tbl_info = user->info + level;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    switch (read_state) {
    case BCMCFG_READ_STATE_MAP:
        SHR_IF_ERR_CONT(bcmcfg_read_device_lookup_field(user, tbl_info, str));
        break;
    case BCMCFG_READ_STATE_SEQ:
    case BCMCFG_READ_STATE_MAP_V:
        SHR_IF_ERR_CONT(bcmcfg_read_device_set_field(user, tbl_info, str));
        break;
    default:
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        break;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an array value.
 *
 * Add a value to a field array.
 *
 * \param [in,out] info         Reader Data
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Value string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_array(bcmcfg_tbl_user_t *user,
                             bcmcfg_read_level_info_t *info,
                             bcmcfg_tbl_info_t *tbl_info,
                             const char *str)
{
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    switch (info->state) {
    case BCMCFG_READ_STATE_SEQ:
        /* In a sequence: sequence ID is array index, str is value. */
        rv = bcmcfg_read_device_set_simple_field_str(user,
                                                     tbl_info,
                                                     (int)info->seq_idx,
                                                     str);
        SHR_IF_ERR_CONT(rv);
        break;
    case BCMCFG_READ_STATE_MAP:
        /* In a map: str is array index. */
        rv = bcmcfg_read_device_set_aidx(tbl_info, str);
        SHR_IF_ERR_CONT(rv);
        break;
    case BCMCFG_READ_STATE_MAP_V:
        /* In a map: str is array value. */
        rv = bcmcfg_read_device_set_simple_field_str(user,
                                                     tbl_info,
                                                     tbl_info->aidx,
                                                     str);
        SHR_IF_ERR_CONT(rv);
        break;
    default:
        /* Weird. */
        SHR_IF_ERR_CONT(SHR_E_INTERNAL);
        break;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief device scalar handler.
 *
 * Scalar events for the device schema occur for map keys, map values,
 * and sequences. Any other context is an error.
 *
 * Map keys:    Search for the key.
 * Map values:  Set the value.
 * Sequences:   Set the value.
 *
 * \param [in]     value        Scalar value.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_scalar(const char *value,
                          bcmcfg_read_context_t *context,
                          void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;
    bcmcfg_tbl_info_t *tbl_info = user->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    switch (tbl_info->state) {
    case UNIT:
        /* Add unit */
        SHR_IF_ERR_CONT(bcmcfg_read_device_add_unit_str(tbl_info, value));
        break;
    case TABLE:
        /* Table value */
        SHR_IF_ERR_CONT(bcmcfg_read_device_search(tbl_info, value));
        /* Start of table context, so stop appending fields. */
        user->append = false;
        break;
    case FIELD:
        /* Field value */
        if (!user->append) {
            SHR_IF_ERR_CONT(bcmcfg_read_device_table_setup(info, user, tbl_info));
        }
        if (!SHR_FUNC_ERR()) {
            SHR_IF_ERR_CONT(bcmcfg_read_device_handle_field(user,
                                                            context->level,
                                                            info->state,
                                                            value));
        }
        break;
    case ARRAY:
        /* Array value */
        SHR_IF_ERR_CONT(bcmcfg_read_device_add_array(user,
                                                     info,
                                                     tbl_info,
                                                     value));
        break;
    default:
        /* Some error */
        break;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy LT data from previous level.
 *
 * If 'start' is true, copy previous level node data to the current
 * level.
 *
 * \param [in]     start        True if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in,out] user         Global LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_copy(bool start,
                        bcmcfg_read_context_t *context,
                        bcmcfg_tbl_user_t *user)
{
    bcmcfg_tbl_info_t *cur_tbl_info;
    bcmcfg_tbl_info_t *prev_tbl_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (start) {
        /* copy previous user data */
        cur_tbl_info = user->info + context->level;
        prev_tbl_info = user->info + context->level - 1;
        sal_memcpy(cur_tbl_info, prev_tbl_info, sizeof(bcmcfg_tbl_info_t));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Manage table state transition.
 *
 * Table parsing states, except FIELD->ARRAY, change on a container
 * push.
 *
 * \param [in]     info         Reader data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static void
next_container_state(const bcmcfg_read_level_info_t *info,
                     bcmcfg_tbl_info_t *tbl_info)
{
    switch (tbl_info->state) {
    case UNIT:
        if (info->state != BCMCFG_READ_STATE_SEQ) {
            tbl_info->state = TABLE;
        }
        break;
    case TABLE:
        if (tbl_info->sid_valid) {
            tbl_info->state = FIELD;
        }
        break;
    default:
        /* Nothing to do. */
        break;
    }
}

/*!
 * \brief Table sequence handler.
 *
 * \param [in]     start        True if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_seq(bool start,
                       bcmcfg_read_context_t *context,
                       void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_read_level_info_t *prev = context->info + context->level - 1;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;
    bcmcfg_tbl_info_t *tbl_info = user->info + context->level;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT(bcmcfg_read_device_copy(start, context, user));
    if (!SHR_FUNC_ERR() && start) {
        switch (prev->state) {
        case BCMCFG_READ_STATE_MAP:
            break;
        case BCMCFG_READ_STATE_MAP_V:
            if (tbl_info->state == FIELD) {
                /* Sequences assigned to a field. */
                bool array;
                int rv;
                rv = bcmcfg_read_device_is_field_array(tbl_info, &array);
                if (array) {
                    tbl_info->state = ARRAY;
                } else {
                    SHR_IF_ERR_CONT(rv);
                }
            }
            break;
        case BCMCFG_READ_STATE_SEQ:
            if (tbl_info->state == UNIT) {
                bcmcfg_read_device_add_unit(tbl_info, (int)info->seq_idx);
            } else if (tbl_info->state == FIELD) {
                tbl_info->fid = info->seq_idx;
            }
            break;
        default:
            break;
        }
        next_container_state(info, tbl_info);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML map event.
 *
 * When starting a map, set the sequence container and start_idx, and
 * calculate storage offset if within a sequence.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_map(bool start,
                       bcmcfg_read_context_t *context,
                       void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_read_level_info_t *prev = context->info + context->level - 1;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT(bcmcfg_read_device_copy(start, context, user));
    if (!SHR_FUNC_ERR() && start) {
        bcmcfg_tbl_info_t *tbl_info = user->info + context->level;

        /* Context.map */
        switch (prev->state) {
        case BCMCFG_READ_STATE_SEQ:
        case BCMCFG_READ_STATE_MAP:
            if (info->state == BCMCFG_READ_STATE_MAP) {
                user->append = false;
            }
            break;
        case BCMCFG_READ_STATE_MAP_V:
            switch (tbl_info->state) {
            case TABLE:
                break;
            case FIELD:
                if (tbl_info->ifield) {
                    SHR_IF_ERR_CONT
                        (bcmcfg_read_device_implicit_field_add(user,
                                                               tbl_info));
                    tbl_info->ifield = false;
                } else {
                    bool array;
                    int rv;
                    rv = bcmcfg_read_device_is_field_array(tbl_info, &array);
                    if (array) {
                        tbl_info->state = ARRAY;
                    } else {
                        SHR_IF_ERR_CONT(rv);
                    }
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        next_container_state(info, tbl_info);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML doc event.
 *
 * Clear the reader stack and prepare a playback list if there is not
 * one already.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_doc(bool start,
                       bcmcfg_read_context_t *context,
                       void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;
    bcmcfg_playback_list_t *list;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(user);
    if (start) {
        /* Clear info stack. */
        sal_memset(bcmcfg_tbl_user.info, 0, sizeof(bcmcfg_tbl_user.info));
        /* Clear append. */
        bcmcfg_tbl_user.append = false;

        /* Only add a playback segment if there is no playback segment
           allocated, or the allocated playback segment is in
           use. This will avoid a chain of empty playback segments. */
        if (user->head == NULL || user->tail->pt || user->tail->lt) {
            list = sal_alloc(sizeof(*list), "bcmcfg_read_device_doc");
            if (list) {
                sal_memset(list, 0, sizeof(*list));
                list->locus = sal_strdup(info->locus);
                if (list->locus) {
                    if (user->head == NULL) {
                        /* Populate head. */
                        user->head = list;
                    }
                    if (user->tail) {
                        /* Link to tail. */
                        user->tail->next = list;
                    }
                    /* Set tail. */
                    user->tail = list;
                } else {
                    /* Out of memory for locus. */
                    sal_free(list);
                    SHR_IF_ERR_CONT(SHR_E_MEMORY);
                }
            } else {
                /* Out of memory for list. */
                SHR_IF_ERR_CONT(SHR_E_MEMORY);
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle reader abort notification.
 *
 * Free any pending playback lists on an abort notification.
 *
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_abort(bcmcfg_read_context_t *context,
                         void *user_data)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    COMPILER_REFERENCE(context);
    COMPILER_REFERENCE(user_data);
    SHR_IF_ERR_CONT(bcmcfg_playback_free());
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup device reader.
 *
 * Device reader setup/cleanup. Called once with start=true, and
 * once with start=false.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_setup(bool start,
                         void *user_data)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (start) {
        sal_memset(&bcmcfg_tbl_user, 0, sizeof(bcmcfg_tbl_user));
    } else {
        SHR_IF_ERR_CONT(bcmcfg_playback_free());
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free playback table.
 *
 * Free playback table element.
 *
 * \param [in,out] t_list       Head of table list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_playback_free_tbl(bcmcfg_tbl_list_t *t_list)
{
    bcmcfg_tbl_list_t *t_ptr;
    shr_fmm_t *f_list;
    shr_fmm_t *f_ptr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (t_list) {
        /* Free fields in table. */
        f_list = t_list->field;
        while (f_list) {
            f_ptr = f_list;
            f_list = f_list->next;
            sal_free(f_ptr);
        }
        /* Free table. */
        t_ptr = t_list;
        t_list = t_list->next;
        sal_free(t_ptr);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free playback enum.
 *
 * Free playback enum element.
 *
 * \param [in,out] list         Head of enum list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_playback_free_enum(bcmcfg_playback_enum_t *list)
{
    bcmcfg_playback_enum_t *ptr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (list) {
        sal_free((void *)list->value);
        ptr = list;
        list = list->next;
        sal_free(ptr);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*
 * Get config table values.
 */
int
bcmcfg_table_get_internal(int unit,
                          bcmltd_sid_t sid,
                          bcmltd_sid_t fid,
                          size_t count,
                          uint64_t *field,
                          size_t *actual)
{
    uint64_t *data;
    size_t i;
    size_t out;
    const bcmltd_table_rep_t *lt;

    SHR_FUNC_ENTER(unit);
    do {
        /* Make sure unit is in range. */
        if (!bcmdrd_dev_exists(unit)) {
            SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
            break;
        }

        lt = bcmltd_table_get(sid);
        /* Make sure table SID is in range. */
        if (lt == NULL) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META("Table not found.\n")));
            SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
            break;
        }

        if (!(lt->flags & BCMLTD_TABLE_F_CONFIG)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Table is not config LT.\n")));
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }

        data = bcmcfg_tbl_user.config[unit][sid];

        /* Copy the fields out starting at FID. */
        for (out = 0, i = fid; i < lt->fields; i++, out++) {
            if (out < count) {
                /* If no data, default is zero. */
                *field++ = data ? data[i] : 0;
            } else {
                break;
            }
        }

        /* Return how many elements were written if requested. */
        if (actual) {
            *actual = i;
        }
    } while (0);
    SHR_FUNC_EXIT();
}

/*
 * Get non-config table playback list.
 */
int
bcmcfg_playback_get(const bcmcfg_playback_list_t **list)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    do {
        if (!list) {
            SHR_IF_ERR_CONT(SHR_E_PARAM);
            break;
        }

        if (!bcmcfg_tbl_user.head) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            /* No break here because static analysis says it is unreachable. */
        }

        if (!bcmcfg_tbl_user.head->lt && !bcmcfg_tbl_user.head->pt) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            /* No break here because static analysis says it is unreachable. */
        }

        *list = bcmcfg_tbl_user.head;

    } while (0);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * Free playback list.
 */
int
bcmcfg_playback_free(void)
{
    bcmcfg_playback_list_t *ptr;
    bcmcfg_playback_list_t *list = bcmcfg_tbl_user.head;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    while (list) {
        bcmcfg_playback_free_tbl(list->lt);
        bcmcfg_playback_free_tbl(list->pt);
        bcmcfg_playback_free_enum(list->enum_head);
        if (list->locus) {
            sal_free((void *)list->locus);
        }
        ptr = list;
        list = list->next;
        sal_free(ptr);
    }
    bcmcfg_tbl_user.head = bcmcfg_tbl_user.tail = NULL;


    SHR_FUNC_EXIT();
}

/*
 * Device reader handler.
 */
const bcmcfg_read_handler_t bcmcfg_read_device = {
    .key       = "device",
    .setup     = bcmcfg_read_device_setup,
    .scalar    = bcmcfg_read_device_scalar,
    .map       = bcmcfg_read_device_map,
    .seq       = bcmcfg_read_device_seq,
    .doc       = bcmcfg_read_device_doc,
    .abort     = bcmcfg_read_device_abort,
    .user_data = &bcmcfg_tbl_user
};
