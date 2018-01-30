/*! \file bcmltm_nc_lt_info.c
 *
 * Logical Table Manager - Logical Table Information.
 *
 * This file contains information on Logical Tables constructed
 * primarily from the LRD.  This information is used
 * for constructing the metadata required for the FA and EE node cookies
 * for LT.
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

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

#include <bcmptm/bcmptm.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_nc_util_internal.h>

#include "bcmltm_nc_lt_map.h"
#include "bcmltm_nc_lt_pt.h"
#include "bcmltm_nc_lt_fv.h"
#include "bcmltm_nc_lta_validate.h"
#include "bcmltm_nc_lta_cth.h"
#include "bcmltm_nc_lta_xfrm.h"
#include "bcmltm_nc_lt_ltm.h"
#include "bcmltm_nc_lt_tc.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Display for working buffer coordinates */
#define WBC_DUMP(_wbc)                                      \
    cli_out("wbc={ofs=%-5d min=%-5d max=%-5d}",             \
            _wbc.buffer_offset, _wbc.minbit, _wbc.maxbit)

/*
 * Accessor macros for Logical Tables info
 */
#define LT_INFO(_u)                      lt_info[_u]
#define LT_INFO_SID_MAX_COUNT(_u)        LT_INFO(_u)->sid_max_count
#define LT_INFO_ATTR(_u, _sid)           LT_INFO(_u)->lt[_sid].attr
#define LT_INFO_GROUPS(_u, _sid)         LT_INFO(_u)->lt[_sid].groups
#define LT_INFO_PT(_u, _sid)             LT_INFO(_u)->lt[_sid].pt
#define LT_INFO_FV(_u, _sid)             LT_INFO(_u)->lt[_sid].fv
#define LT_INFO_TC(_u, _sid)             LT_INFO(_u)->lt[_sid].tc
#define LT_INFO_CTH(_u, _sid)            LT_INFO(_u)->lt[_sid].cth
#define LT_INFO_LTM(_u, _sid)            LT_INFO(_u)->lt[_sid].ltm

#define LT_INFO_PT_LIST(_u, _sid)        LT_INFO_PT(_u, _sid)->pt_list
#define LT_INFO_PT_KEYED_LIST(_u, _sid)  LT_INFO_PT(_u, _sid)->pt_keyed_list
#define LT_INFO_PT_KEYED(_u, _sid, _idx) LT_INFO_PT_KEYED_LIST(_u, _sid)[_idx]
#define LT_INFO_FA_LT_INDEX(_u, _sid)    LT_INFO_PT(_u, _sid)->fa_lt_index
#define LT_INFO_EE_LT_INDEX(_u, _sid)    LT_INFO_PT(_u, _sid)->ee_lt_index
#define LT_INFO_LT_TO_PT_INDEX(_u, _sid)        \
    LT_INFO_PT(_u, _sid)->lt_to_pt_index
#define LT_INFO_PTM_MAPPING_KEYS_IN(_u, _sid)   \
    LT_INFO_PT(_u, _sid)->ptm_mapping_keys_in
#define LT_INFO_PTM_MAPPING_KEYS_OUT(_u, _sid)   \
    LT_INFO_PT(_u, _sid)->ptm_mapping_keys_out
#define LT_INFO_PTM_MAPPING_KEYS_ALLOC_REQUIRED(_u, _sid)    \
    LT_INFO_PT(_u, _sid)->ptm_mapping_keys_alloc_required
#define LT_INFO_PTM_MAPPING_KEYS_ALLOC_OPTIONAL(_u, _sid)    \
    LT_INFO_PT(_u, _sid)->ptm_mapping_keys_alloc_optional
#define LT_INFO_PTM_MAPPING_VALUES(_u, _sid)    \
    LT_INFO_PT(_u, _sid)->ptm_mapping_values
#define LT_INFO_FIXED_FIELDS(_u, _sid)          \
    LT_INFO_PT(_u, _sid)->fixed_fields

#define LT_INFO_VALIDATE_KEY_LIST(_u, _sid)                    \
    LT_INFO_FV(_u, _sid)->validate_keys
#define LT_INFO_VALIDATE_KEY_INFO(_u, _sid, _idx)              \
    LT_INFO_VALIDATE_KEY_LIST(_u, _sid)->validate_info[_idx]
#define LT_INFO_VALIDATE_KEY_NUM(_u, _sid)                     \
    LT_INFO_VALIDATE_KEY_LIST(_u, _sid)->num
#define LT_INFO_LTA_VALIDATE_KEY_PARAMS(_u, _sid, _idx)        \
    LT_INFO_VALIDATE_KEY_INFO(_u, _sid, _idx).validate_params
#define LT_INFO_LTA_VALIDATE_KEY_SRC_FIELDS(_u, _sid, _idx)    \
    LT_INFO_VALIDATE_KEY_INFO(_u, _sid, _idx).fsrc

#define LT_INFO_VALIDATE_VALUE_LIST(_u, _sid)                  \
    LT_INFO_FV(_u, _sid)->validate_values
#define LT_INFO_VALIDATE_VALUE_INFO(_u, _sid, _idx)            \
    LT_INFO_VALIDATE_VALUE_LIST(_u, _sid)->validate_info[_idx]
#define LT_INFO_VALIDATE_VALUE_NUM(_u, _sid)                   \
    LT_INFO_VALIDATE_VALUE_LIST(_u, _sid)->num
#define LT_INFO_LTA_VALIDATE_VALUE_PARAMS(_u, _sid, _idx)      \
    LT_INFO_VALIDATE_VALUE_INFO(_u, _sid, _idx).validate_params
#define LT_INFO_LTA_VALIDATE_VALUE_SRC_FIELDS(_u, _sid, _idx)  \
    LT_INFO_VALIDATE_VALUE_INFO(_u, _sid, _idx).fsrc

#define LT_INFO_VALIDATE_COPY_LIST(_u, _sid)                  \
    LT_INFO_FV(_u, _sid)->copy_validate_info_list
#define LT_INFO_VALIDATE_COPY_INFO(_u, _sid, _from_idx, _from_num, _idx) \
    LT_INFO_VALIDATE_COPY_LIST(_u, _sid)->validate_info[      \
        (((_idx) * (_from_num)) + (_from_idx))]
#define LT_INFO_VALIDATE_COPY_NUM(_u, _sid)                   \
    LT_INFO_VALIDATE_COPY_LIST(_u, _sid)->num
#define LT_INFO_LTA_VALIDATE_COPY_FIELDS(_u, _sid, _from_idx, \
                                     _from_num, _idx)         \
    LT_INFO_VALIDATE_COPY_INFO(_u, _sid, _from_idx,           \
                                _from_num, _idx).fsrc

#define LT_INFO_FWD_KEY_XFRM_INFO_LIST(_u, _sid)               \
    LT_INFO_PT(_u, _sid)->fwd_key_xfrm_info_list
#define LT_INFO_FWD_KEY_XFRM_INFO(_u, _sid, _idx)              \
    LT_INFO_FWD_KEY_XFRM_INFO_LIST(_u, _sid)->xfrm_info[_idx]
#define LT_INFO_FWD_KEY_XFRM_NUM(_u, _sid)                     \
    LT_INFO_FWD_KEY_XFRM_INFO_LIST(_u, _sid)->num

#define LT_INFO_LTA_FWD_KEY_XFRM_TABLE_PARAMS(_u, _sid, _idx)  \
    LT_INFO_FWD_KEY_XFRM_INFO(_u, _sid, _idx).xfrm_params
#define LT_INFO_LTA_FWD_KEY_XFRM_IN_FIELDS(_u, _sid, _idx)     \
    LT_INFO_FWD_KEY_XFRM_INFO(_u, _sid, _idx).fin
#define LT_INFO_LTA_FWD_KEY_XFRM_OUT_FIELDS(_u, _sid, _idx)    \
    LT_INFO_FWD_KEY_XFRM_INFO(_u, _sid, _idx).fout

#define LT_INFO_FWD_VALUE_XFRM_INFO_LIST(_u, _sid)               \
    LT_INFO_PT(_u, _sid)->fwd_value_xfrm_info_list
#define LT_INFO_FWD_VALUE_XFRM_INFO(_u, _sid, _idx)              \
    LT_INFO_FWD_VALUE_XFRM_INFO_LIST(_u, _sid)->xfrm_info[_idx]
#define LT_INFO_FWD_VALUE_XFRM_NUM(_u, _sid)                     \
    LT_INFO_FWD_VALUE_XFRM_INFO_LIST(_u, _sid)->num

#define LT_INFO_LTA_FWD_VALUE_XFRM_TABLE_PARAMS(_u, _sid, _idx)  \
    LT_INFO_FWD_VALUE_XFRM_INFO(_u, _sid, _idx).xfrm_params
#define LT_INFO_LTA_FWD_VALUE_XFRM_IN_FIELDS(_u, _sid, _idx)     \
    LT_INFO_FWD_VALUE_XFRM_INFO(_u, _sid, _idx).fin
#define LT_INFO_LTA_FWD_VALUE_XFRM_OUT_FIELDS(_u, _sid, _idx)    \
    LT_INFO_FWD_VALUE_XFRM_INFO(_u, _sid, _idx).fout

#define LT_INFO_REV_KEY_XFRM_INFO_LIST(_u, _sid)               \
    LT_INFO_PT(_u, _sid)->rev_key_xfrm_info_list
#define LT_INFO_REV_KEY_XFRM_INFO(_u, _sid, _idx)              \
    LT_INFO_REV_KEY_XFRM_INFO_LIST(_u, _sid)->xfrm_info[_idx]
#define LT_INFO_REV_KEY_XFRM_NUM(_u, _sid)                     \
    LT_INFO_REV_KEY_XFRM_INFO_LIST(_u, _sid)->num

#define LT_INFO_LTA_REV_KEY_XFRM_TABLE_PARAMS(_u, _sid, _idx)  \
    LT_INFO_REV_KEY_XFRM_INFO(_u, _sid, _idx).xfrm_params
#define LT_INFO_LTA_REV_KEY_XFRM_IN_FIELDS(_u, _sid, _idx)     \
    LT_INFO_REV_KEY_XFRM_INFO(_u, _sid, _idx).fin
#define LT_INFO_LTA_REV_KEY_XFRM_OUT_FIELDS(_u, _sid, _idx)    \
    LT_INFO_REV_KEY_XFRM_INFO(_u, _sid, _idx).fout

#define LT_INFO_REV_VALUE_XFRM_INFO_LIST(_u, _sid)               \
    LT_INFO_PT(_u, _sid)->rev_value_xfrm_info_list
#define LT_INFO_REV_VALUE_XFRM_INFO(_u, _sid, _idx)              \
    LT_INFO_REV_VALUE_XFRM_INFO_LIST(_u, _sid)->xfrm_info[_idx]
#define LT_INFO_REV_VALUE_XFRM_NUM(_u, _sid)                     \
    LT_INFO_REV_VALUE_XFRM_INFO_LIST(_u, _sid)->num

#define LT_INFO_LTA_REV_VALUE_XFRM_TABLE_PARAMS(_u, _sid, _idx)  \
    LT_INFO_REV_VALUE_XFRM_INFO(_u, _sid, _idx).xfrm_params
#define LT_INFO_LTA_REV_VALUE_XFRM_IN_FIELDS(_u, _sid, _idx)     \
    LT_INFO_REV_VALUE_XFRM_INFO(_u, _sid, _idx).fin
#define LT_INFO_LTA_REV_VALUE_XFRM_OUT_FIELDS(_u, _sid, _idx)    \
    LT_INFO_REV_VALUE_XFRM_INFO(_u, _sid, _idx).fout

#define LT_INFO_COPY_XFRM_INFO_LIST(_u, _sid)                    \
    LT_INFO_PT(_u, _sid)->copy_xfrm_info_list
#define LT_INFO_COPY_XFRM_INFO(_u, _sid, _from_idx, _from_num, _to_idx)  \
    LT_INFO_COPY_XFRM_INFO_LIST(_u, _sid)->xfrm_info[            \
        (((_to_idx) * (_from_num)) + (_from_idx))]
#define LT_INFO_COPY_XFRM_NUM(_u, _sid)                          \
    LT_INFO_COPY_XFRM_INFO_LIST(_u, _sid)->num

#define LT_INFO_LTA_COPY_XFRM_FIELDS(_u, _sid, _from_idx,        \
                                     _from_num, _to_idx)         \
    LT_INFO_COPY_XFRM_INFO(_u, _sid, _from_idx,                  \
                                _from_num, _to_idx).fin

#define LT_INFO_TC_HANDLER_LIST(_u, _sid)       \
    LT_INFO_TC(_u, _sid)->tc_list

#define LT_INFO_LTA_CTH_HANDLER(_u, _sid)       \
    LT_INFO_CTH(_u, _sid)->handler
#define LT_INFO_LTA_CTH_TABLE_PARAMS(_u, _sid)  \
    LT_INFO_CTH(_u, _sid)->table_params
#define LT_INFO_LTA_CTH_IN_FIELDS(_u, _sid)     \
    LT_INFO_CTH(_u, _sid)->fin
#define LT_INFO_LTA_CTH_IN_KEY_FIELDS(_u, _sid) \
    LT_INFO_CTH(_u, _sid)->fin_keys
#define LT_INFO_LTA_CTH_OUT_FIELDS(_u, _sid)    \
    LT_INFO_CTH(_u, _sid)->fout

#define LT_INFO_LTM_FA_LT_INDEX(_u, _sid)  LT_INFO_LTM(_u, _sid)->fa_lt_index
#define LT_INFO_LTM_KEY(_u, _sid)     LT_INFO_LTM(_u, _sid)->ltm_key
#define LT_INFO_LTM_FS_LIST(_u, _sid) LT_INFO_LTM(_u, _sid)->fs_list

/*
 * Helper macros
 */
/* Check that info has been initialized */
#define LT_INFO_INIT(_u)                   \
    (LT_INFO(_u) != NULL)

/* Check that symbold id is valid */
#define LT_SID_VALID(_u, _sid)                  \
    ((_sid) < LT_INFO_SID_MAX_COUNT(_u))


/*!
 * \brief Information for a given Logical Table.
 *
 * This structure contains information derived from the LRD for a
 * logical table which is referenced by the LTM metadata and used
 * for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct lt_info_s {
    /*!
     * Table attributes.
     */
    bcmltm_table_attr_t attr;

    /*!
     * Map groups information: Physical, Customer Table Handler.
     */
    bcmltm_map_groups_t groups;

    /*!
     * Information for logical tables with physical table mappings.
     */
    bcmltm_nc_lt_pt_info_t *pt;

    /*!
     * Information for logical tables with Field validation.
     */
    bcmltm_nc_lt_fv_info_t *fv;

    /*!
     * Information for logical tables with Table Commit interfaces.
     */
    bcmltm_nc_lt_tc_info_t *tc;

    /*!
     * Information for logical tables with Custom Table handlers.
     */
    bcmltm_nc_lta_cth_info_t *cth;

    /*!
     * Information for logical tables with LTM group.
     */
    bcmltm_nc_lt_ltm_info_t *ltm;
} lt_info_t;


/*!
 * \brief LT Information.
 */
typedef struct lt_info_list_s {
    /*! Total number of symbols. */
    uint32_t sid_max_count;

    /*!
     * Array of LT information.
     *
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    lt_info_t lt[];
} lt_info_list_t;


/* Logical Table Information Data */
static lt_info_list_t *lt_info[BCMLTM_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Return name of physical symbol ID.
 *
 * This routine gets the name for the given physical symbol ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical symbol ID.
 *
 * \retval Physical symbol name.
 */
static const char *
pt_name_get(int unit, bcmdrd_sid_t sid)
{
    const char *name;

    name = bcmdrd_pt_sid_to_name(unit, sid);

    if (name == NULL) {
        name = "UNKNOWN";
    }

    return name;
}


/*!
 * \brief Return field name for given logical table and field id.
 *
 * This routine gets the field name for given logical table and field id
 * from the LRD.
 *
 * \param [in] sid Table ID.
 * \param [in] fid Field ID.
 *
 * \retval Logical field name.
 */
static const char *
lt_field_name_get(bcmlrd_sid_t sid, uint32_t fid)
{
    const bcmlrd_table_rep_t *tbl;
    const char *name = "UNKNOWN";

    tbl = bcmlrd_table_get(sid);
    if ((tbl != NULL) && (fid < tbl->fields )) {
        name = tbl->field[fid].name;
    }

    return name;
}


/*!
 * \brief Display given list of uint32_t values.
 *
 * This routine displays the given array of uint32_t values.
 *
 * \param [in] count Number of elements in array.
 * \param [in] value Array to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
array_dump(uint32_t count, const uint32_t *value, const char *prefix)
{
    uint32_t i;

    if (count == 0) {
        return;
    }

    for (i = 0; i < count; i++) {
        if (i == 0) {
            cli_out("%s", prefix);
        } else {
            if ((i % 4) == 0) {
                cli_out("\n%s", prefix);
            } else {
                cli_out(" ");
            }
        }
        cli_out("[%3d]=%-8d", i, value[i]);
    }

    cli_out("\n");
}


/*!
 * \brief Display table attributes for given table ID.
 *
 * This routine displays the table attributes defined for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
attr_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_table_attr_t *attr;
    char *str;

    cli_out("--- Table Attributes ---\n");

    attr = &LT_INFO_ATTR(unit, sid);

    /* Table Type */
    cli_out("Table Type: 0x%x\n", attr->type);
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_CATG_PT) {
        cli_out("  PT Pass Thru\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_CATG_LT) {
        cli_out("  Logical\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_MAP_PT) {
        cli_out("  Direct PT Map\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_MAP_CTH) {
        cli_out("  Custom Table Handler Map\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_MAP_LTM) {
        cli_out("  LTM Map\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_KEY_SIMPLE_INDEX) {
        cli_out("  Simple Index\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_KEY_ALLOC_INDEX) {
        cli_out("  Index with Allocation\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_KEY_HASH) {
        cli_out("  Hash\n");
    }
    if (attr->type & BCMLTM_TABLE_TYPE_FLAG_KEY_TCAM) {
        cli_out("  TCAM\n");
    }

    /* Operating Mode */
    if (attr->mode == BCMLTM_TABLE_MODE_INTERACTIVE) {
        str = "Interactive";
    } else {
        str = "Modeled";
    }
    cli_out("Operating Mode: %s (%d)\n", str, attr->mode);

    /* Table flags */
    cli_out("Flags: 0x%x\n", attr->flag);
    if (attr->flag & BCMLTM_TABLE_FLAG_READ_ONLY) {
        cli_out("  Read only\n");
    }
    if (attr->flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        cli_out("  Overlay\n");
    }

    /* Table limits */
    cli_out("Index: min=%d max=%d\n",
            attr->index_min, attr->index_max);
    cli_out("Memory Parameter: min=%d max=%d\n",
            attr->mem_param_min, attr->mem_param_max);

    /* Global LT ID (for overlay) */
    cli_out("Global LT ID: %d",
            attr->global_ltid);
    if (attr->flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        const bcmlrd_table_rep_t *tbl;
        const char *name;

        tbl = bcmlrd_table_get(attr->global_ltid);
        if (tbl == NULL) {
            name = "";
        } else {
            name = tbl->name;
        }
        cli_out(" %s\n", name);
    }

    return;
}


/*!
 * \brief Display working buffer information for given table ID.
 *
 * This routine displays the working buffer information for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
wb_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_wb_dump(bcmltm_wb_lt_handle_get(unit, sid));

    return;
}


/*!
 * \brief Display map groups for given table ID.
 *
 * This routine displays the map groups defined for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
groups_dump(int unit, bcmlrd_sid_t sid)
{
    uint32_t i;
    uint32_t num_groups;
    bcmltm_map_pt_info_t *pt;
    const char *pt_name;

    cli_out("--- Map Groups ---\n");

    /* Physical Table Groups */
    num_groups = LT_INFO_GROUPS(unit, sid).pt ?
        LT_INFO_GROUPS(unit, sid).pt->num_pt : 0;
    cli_out("Physical Groups: %d\n", num_groups);
    for (i = 0; i < num_groups; i++) {
        pt = &LT_INFO_GROUPS(unit, sid).pt->info[i];
        pt_name = pt_name_get(unit, pt->sid);

        cli_out("  [%3d]: pt_id=%d %s\n", i, pt->sid, pt_name);
        cli_out("         flags=0x%x num_ops=%d wb_block_id=%d\n",
                pt->flags, pt->num_ops, pt->wb_block_id);
    }

    /* Field Validation Groups */
    num_groups = LT_INFO_GROUPS(unit, sid).fv ?
        LT_INFO_GROUPS(unit, sid).fv->num : 0;
    cli_out("Field Validation Groups: %d\n", num_groups);

    /* Table Commit Groups */
    num_groups = LT_INFO_GROUPS(unit, sid).tc ?
        LT_INFO_GROUPS(unit, sid).tc->num : 0;
    cli_out("Table Commit Groups: %d\n", num_groups);

    /* Custom Table Handler Groups */
    num_groups = LT_INFO_GROUPS(unit, sid).cth ?
        LT_INFO_GROUPS(unit, sid).cth->num : 0;
    cli_out("Custom Table Handler Groups: %d\n", num_groups);

    /* LTM Groups */
    num_groups = LT_INFO_GROUPS(unit, sid).ltm ?
        LT_INFO_GROUPS(unit, sid).ltm->num : 0;
    cli_out("LTM Groups: %d\n", num_groups);

    return;
}


/*!
 * \brief Display PT memory arguments structure.
 *
 * This routine displays given PT memory arguments structure.
 *
 * \param [in] mem_arg PT memory arguments structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
pt_mem_args_dump(const bcmltm_pt_mem_args_t *mem_args, char *prefix)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info;

    cli_out("%sMemory args:\n", prefix);
    if (mem_args == NULL) {
        return;
    }

    pt_dyn_info = mem_args->pt_dyn_info;
    pt_dyn_hash_info = mem_args->pt_dyn_hash_info;

    cli_out("%s  flags=0x%x pt_id=%d\n",
            prefix, mem_args->flags, mem_args->pt);

    cli_out("%s  pt_dyn_info: ", prefix);
    if (pt_dyn_info != NULL) {
        cli_out(" index=%d tbl_inst=%d\n",
                pt_dyn_info->index, pt_dyn_info->tbl_inst);
    } else {
        cli_out("\n");
    }

    cli_out("%s  pt_dyn_hash_info: ", prefix);
    if (pt_dyn_hash_info != NULL) {
        cli_out(" tbl_inst=%d bank=%d\n",
                pt_dyn_hash_info->tbl_inst, pt_dyn_hash_info->bank);
    } else {
        cli_out("\n");
    }

    return;
}


/*!
 * \brief Display PT list for given table ID.
 *
 * This routine displays the node cookie information related
 * to the PTM list for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
pt_list_dump(int unit, bcmlrd_sid_t sid)
{
    uint32_t i;
    uint32_t op;
    char *prefix;
    bcmltm_pt_list_t *list;
    bcmltm_pt_mem_args_t *mem_args;
    bcmltm_pt_op_list_t *op_list;

    list = LT_INFO_PT_LIST(unit, sid);
    if (list == NULL) {
        return;
    }

    cli_out("PT List: %d\n", list->num_pt_view);

    prefix = "         ";
    for (i = 0; i < list->num_pt_view; i++) {
        cli_out("  [%3d]:\n", i);

        mem_args = list->mem_args[i];
        pt_mem_args_dump(mem_args, prefix);

        op_list = list->memop_lists[i];
        cli_out("%sMemory ops:\n", prefix);
        cli_out("%s  flags=0x%x num_ops=%d pt_wsize=%d keys=%d entries=%d\n",
                prefix,
                op_list->flags, op_list->num_pt_ops, op_list->word_size,
                op_list->key_entries, op_list->data_entries);
        for (op = 0; op < op_list->num_pt_ops; op++) {
            cli_out("%s  op[%3d]: param_ofs=%-5d buffer_ofs=%-5d\n",
                    prefix, op,
                    op_list->param_offsets[op],
                    op_list->buffer_offsets[op]);
        }
    }

    return;
}


/*!
 * \brief Display PT keyed operations information for given table ID.
 *
 * This routine displays the node cookie information related
 * to the PTM keyed operations for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
pt_keyed_list_dump(int unit, bcmlrd_sid_t sid)
{
    uint32_t idx;
    uint32_t i;
    char *prefix;
    char *opcode;
    bcmltm_pt_keyed_op_info_t *info;
    bcmltm_pt_mem_args_t *mem_args;

    if (LT_INFO_PT_KEYED_LIST(unit, sid) == NULL) {
        return;
    }

    cli_out("PT Keyed Operations Info:\n");

    for (idx = 0; idx < BCMLTM_NC_PT_KEYED_NUM; idx++) {
        info = LT_INFO_PT_KEYED(unit, sid, idx);
        if (info == NULL) {
            continue;
        }

        prefix = "  ";
        if (idx == BCMLTM_NC_PT_KEYED_INS_DEL) {
            opcode = "Insert/Delete";
        } else if (idx == BCMLTM_NC_PT_KEYED_LOOK_TRAV) {
            opcode = "Lookup/Get-First/Get-Next";
        }
        cli_out("%s[%d]: %s\n", prefix, idx, opcode);

        prefix = "       ";
        mem_args = info->mem_args;
        pt_mem_args_dump(mem_args, prefix);

        cli_out("%sflags=0x%x pt_key_wsize=%d pt_data_wsize=%d\n",
                prefix,
                info->flags, info->key_word_size, info->data_word_size);
        cli_out("%sparam_ofs=%-5d\n",
                prefix,
                info->param_offset);

        cli_out("%sinput_key_ofs  ", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            cli_out("  [%d]=%-5d", i, info->input_key_buffer_offset[i]);
        }
        cli_out("\n");

        cli_out("%sinput_data_ofs ", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            cli_out("  [%d]=%-5d", i, info->input_data_buffer_offset[i]);
        }
        cli_out("\n");

        cli_out("%soutput_key_ofs ", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            cli_out("  [%d]=%-5d", i, info->output_key_buffer_offset[i]);
        }
        cli_out("\n");

        cli_out("%soutput_data_ofs", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            cli_out("  [%d]=%-5d", i, info->output_data_buffer_offset[i]);
        }
        cli_out("\n");
    }

    return;
}


/*!
 * \brief Display given FA LT index.
 *
 * This routine displays given FA LT index node cookie.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
fa_lt_index_dump(bcmltm_fa_lt_index_t *info)
{
    if (info == NULL) {
        return;
    }

    cli_out("FA LT Index:\n");
    cli_out("  lt_index_ofs=%d  track_index_ofs=%d  mem_param_ofs=%d\n",
            info->lt_index_offset, info->track_index_offset,
            info->mem_param_offset);
    cli_out("  index_mult=%d  index_absent_ofs=%d\n",
            info->index_multiplier, info->index_absent_offset);

    return;
}


/*!
 * \brief Display FA LT index information for given table ID with PT groups.
 *
 * This routine displays the node cookie information used
 * for handling the FA LT index for a given table ID that has direct PT group
 * maps.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
pt_fa_lt_index_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_fa_lt_index_t *info;

    info = LT_INFO_FA_LT_INDEX(unit, sid);
    fa_lt_index_dump(info);

    return;
}


/*!
 * \brief Display EE LT index information for given table ID.
 *
 * This routine displays the node cookie information used
 * for handling the EE LT index for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
ee_lt_index_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_ee_index_info_t *info;

    info = LT_INFO_EE_LT_INDEX(unit, sid);
    if (info == NULL) {
        return;
    }

    cli_out("EE LT Index:\n");
    cli_out("  index_ofs=%d\n", info->index_offset);

    return;
}


/*!
 * \brief Display LT to PT index copy list for given table ID.
 *
 * This routine displays the node cookie information used
 * to copy the LT index/param into the corresponding PTM working buffer
 * locations.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
lt_to_pt_index_dump(int unit, bcmlrd_sid_t sid)
{
    uint32_t i;
    bcmltm_wb_copy_list_t *info;
    bcmltm_wb_copy_t *index;

    info = LT_INFO_LT_TO_PT_INDEX(unit, sid);
    if (info == NULL) {
        return;
    }

    cli_out("LT to PT Index WB Copy:\n");
    cli_out("  num_wb_copy=%d\n", info->num_wb_copy);
    cli_out("  From                                : To\n");
    for (i = 0; i < info->num_wb_copy; i++) {
        index = &info->wb_copy[i];
        cli_out("  ");
        WBC_DUMP(index->wbc_source);
        cli_out(" : ");
        WBC_DUMP(index->wbc_destination);
        cli_out("\n");
    }

    return;
}


/*!
 * \brief Display given PTM field mapping.
 *
 * This routine displays the node cookie information related
 * to the PTM mapping entries.
 *
 * \param [in] sid Table ID.
 * \param [in] ptm_map PTM field map to display.
 * \param [in] type Field type string.
 */
static void
ptm_mapping_dump(bcmlrd_sid_t sid,
                 bcmltm_to_ptm_mapping_t *ptm_map, char *type)
{
    uint32_t i;
    char *field_prefix;
    uint32_t prev_fid = 0xffffffff;
    const char *fname = "";
    bcmltm_ptm_field_map_t *field;

    cli_out("PTM %s Fields: %d\n",
             type, ptm_map->field_list->num_fields);
    cli_out("  index_absent_ofs=%d\n",
            ptm_map->field_list->index_absent_offset);

    field_prefix = "      ";
    for (i = 0; i < ptm_map->field_list->num_fields; i++) {
        field = &ptm_map->field_list->field_maps[i];

        while (field != NULL) {
            if (field->api_field_id != prev_fid) {
                fname = lt_field_name_get(sid, field->api_field_id);
                cli_out("  %s\n", fname);
            }

            cli_out("    id=%-5d index=%-3d\n",
                    field->api_field_id, field->field_idx);

            cli_out("%sdef=0x%-8" PRIx64
                    " min=0x%-8" PRIx64 " max=0x%-8" PRIx64 "\n",
                    field_prefix,
                    field->default_value,
                    field->minimum_value, field->maximum_value);
            cli_out("%spt_chg_ofs=%-5d\n",
                    field_prefix,
                    field->pt_changed_offset);
            cli_out("%s", field_prefix);
            WBC_DUMP(field->wbc);
            cli_out("\n");

            prev_fid = field->api_field_id;
            field = field->next_map;
        }
    }

    return;
}


/*!
 * \brief Display PTM mapping fields for given table ID.
 *
 * This routine displays the node cookie information related
 * to the PTM mapping entries for keys and values for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
ptm_mappings_dump(int unit, bcmlrd_sid_t sid)
{
    char *str;
    bcmltm_to_ptm_mapping_t *key_in_map;
    bcmltm_to_ptm_mapping_t *key_out_map;
    bcmltm_to_ptm_mapping_t *key_alloc_req_map;
    bcmltm_to_ptm_mapping_t *key_alloc_opt_map;
    bcmltm_to_ptm_mapping_t *value_map;

    /* Key fields list */
    key_in_map = LT_INFO_PTM_MAPPING_KEYS_IN(unit, sid);
    key_out_map = LT_INFO_PTM_MAPPING_KEYS_OUT(unit, sid);
    if (key_in_map != NULL) {
        if (key_in_map != key_out_map) {
            str = "Key Input";
        } else {
            str = "Key Input/Output";
        }
        ptm_mapping_dump(sid, key_in_map, str);
    }
    if ((key_out_map != NULL) && (key_out_map != key_in_map)){
        ptm_mapping_dump(sid, key_out_map, "Key Output");
    }

    /* Required/Optional Key fields list */
    key_alloc_req_map = LT_INFO_PTM_MAPPING_KEYS_ALLOC_REQUIRED(unit, sid);
    key_alloc_opt_map = LT_INFO_PTM_MAPPING_KEYS_ALLOC_OPTIONAL(unit, sid);
    if (key_alloc_req_map != NULL) {
        ptm_mapping_dump(sid, key_alloc_req_map, "Key Allocation Required");
    }
    if (key_alloc_opt_map != NULL) {
        ptm_mapping_dump(sid, key_alloc_opt_map, "Key Allocation Optional");
    }

    /* Value fields list */
    value_map = LT_INFO_PTM_MAPPING_VALUES(unit, sid);
    if (value_map != NULL) {
        ptm_mapping_dump(sid, value_map, "Value Input/Output");
    }

    return;
}


/*!
 * \brief Display fixed fields mapping for given table ID.
 *
 * This routine displays the node cookie information related
 * to fixed field map entries for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
fixed_fields_dump(int unit, bcmlrd_sid_t sid)
{
    uint32_t i;
    bcmltm_fixed_field_list_t *fixed;

    fixed = LT_INFO_FIXED_FIELDS(unit, sid);
    if (fixed != NULL) {
        cli_out("Fixed Fields: %d\n", fixed->num_fixed_fields);
        for (i = 0; i < fixed->num_fixed_fields; i++) {
            cli_out("  ");
            cli_out("value=0x%-8x  ", fixed->fixed_fields[i].field_value);
            WBC_DUMP(fixed->fixed_fields[i].wbc);
            cli_out("\n");
        }
    }

    return;
}


/*!
 * \brief Display given LTD generic argument structure.
 *
 * This routine displays the given LTD generic argument information.
 *
 * \param [in] arg LTD generic argument structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
ltd_generic_arg_dump(const bcmltd_generic_arg_t *arg, char *prefix)
{
    char arg_prefix[30];

    cli_out("%sltd_generic_arg:\n", prefix);
    if (arg == NULL) {
        return;
    }

    cli_out("%s  sid=%d user_data=0x%lx\n",
            prefix, arg->sid, (unsigned long)arg->user_data);

    sal_sprintf(arg_prefix, "%s    ", prefix);
    cli_out("%s  values: %d\n", prefix, arg->values);
    array_dump(arg->values, arg->value, arg_prefix);

    return;
}


/*!
 * \brief Display given LTD transform argument structure.
 *
 * This routine displays the given LTD transform argument information.
 *
 * \param [in] arg LTD transform structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
ltd_transform_arg_dump(bcmltd_transform_arg_t *arg, char *prefix)
{
    char arg_prefix[30];

    cli_out("%sltd_transform_arg:\n", prefix);
    if (arg == NULL) {
        return;
    }

    sal_sprintf(arg_prefix, "%s    ", prefix);

    cli_out("%s  values: %d\n", prefix, arg->values);
    array_dump(arg->values, arg->value, arg_prefix);

    cli_out("%s  tables: %d\n", prefix, arg->tables);
    array_dump(arg->tables, arg->table, arg_prefix);

    cli_out("%s  fields: %d\n", prefix, arg->fields);
    array_dump(arg->fields, arg->field, arg_prefix);

    cli_out("%s  rfields: %d\n", prefix, arg->rfields);
    array_dump(arg->rfields, arg->rfield, arg_prefix);

    sal_sprintf(arg_prefix, "%s  ", prefix);
    ltd_generic_arg_dump(arg->comp_data, arg_prefix);

    return;
}

/*!
 * \brief Display given LTD field validation argument structure.
 *
 * This routine displays the given LTD field validation argument information.
 *
 * \param [in] arg LTD field validation structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
ltd_field_val_arg_dump(bcmltd_field_val_arg_t *arg, char *prefix)
{
    char arg_prefix[30];

    cli_out("%sltd_field_val_arg:\n", prefix);
    if (arg == NULL) {
        return;
    }

    sal_sprintf(arg_prefix, "%s    ", prefix);

    cli_out("%s  values: %d\n", prefix, arg->values);
    array_dump(arg->values, arg->value, arg_prefix);

    cli_out("%s  fields: %d\n", prefix, arg->fields);
    array_dump(arg->fields, arg->field, arg_prefix);

    sal_sprintf(arg_prefix, "%s  ", prefix);
    ltd_generic_arg_dump(arg->comp_data, arg_prefix);

    return;
}


/*!
 * \brief Display given LTA field list.
 *
 * This routine displays the given LTA field list information.
 *
 * \param [in] sid Table ID.
 * \param [in] list LTA field list to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in] type Field type string.
 * \param [in] logical Indicates if field list refers to logical fields.
 */
static void
lta_field_list_dump(bcmlrd_sid_t sid,
                    bcmltm_lta_field_list_t *list,
                    char *prefix, char *type, bool logical)
{
    uint32_t i;
    char field_prefix[80];
    uint32_t prev_fid = 0xffffffff;
    const char *fname = "";
    bcmltm_lta_field_map_t *field;

    cli_out("%s%s Fields:\n", prefix, type);
    if (list == NULL) {
        return;
    }

    cli_out("%s  fields_param_ofs=%-5d target_fields_param_ofs=%-5d\n",
            prefix,
            list->fields_parameter_offset,
            list->target_fields_parameter_offset);
    cli_out("%s  fields_ptrs_ofs=%-5d index_absent_ofs=%-5d\n",
            prefix,
            list->fields_ptrs_offset,
            list->index_absent_offset);

    cli_out("%s  Fields: max=%d num=%d\n",
            prefix, list->max_fields, list->num_maps);

    sal_sprintf(field_prefix, "%s        ", prefix);
    for (i = 0; i < list->max_fields; i++) {
        field = &list->field_maps[i];

        if (logical) {
            if (field->api_field_id != prev_fid) {
                fname = lt_field_name_get(sid, field->api_field_id);
                cli_out("%s    %s\n", prefix, fname);
            }
        }

        cli_out("%s      id=%-5d index=%-3d\n",
                prefix,
                field->api_field_id,
                field->field_idx);

        cli_out("%sdef=0x%-8" PRIx64
                " min=0x%-8" PRIx64 " max=0x%-8" PRIx64 "\n",
                field_prefix,
                field->default_value,
                field->minimum_value,
                field->maximum_value);

        cli_out("%spt_chg_ofs=%-5d field_ofs=%-5d\n",
                field_prefix,
                field->pt_changed_offset,
                field->field_offset);

        cli_out("%s", field_prefix);
        WBC_DUMP(field->wbc);
        cli_out("\n");

        prev_fid = field->api_field_id;
    }

    return;
}


/*!
 * \brief Display given LTA validate params structure.
 *
 * This routine displays the given LTA validate params information.
 *
 * \param [in] params LTA validate params structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
validate_params_dump(bcmltm_lta_validate_params_t *params, char *prefix)
{
    char arg_prefix[30];

    cli_out("%sParams:\n", prefix);
    if (params == NULL) {
        return;
    }

    cli_out("%s  validate_f=0x%lx fields_in_ofs=%-5d\n",
            prefix,
            (unsigned long)params->lta_validate,
            params->fields_input_offset);

    sal_sprintf(arg_prefix, "%s  ", prefix);
    ltd_field_val_arg_dump(params->lta_args, arg_prefix);

    return;
}


/*!
 * \brief Display given field validation list.
 *
 * This routine displays the node cookie information related
 * to field validation map entries.
 *
 * \param [in] sid Table ID.
 * \param [in] info_list List of field validations.
 * \param [in] type Field type string.
 */
static void
validate_list_dump(bcmlrd_sid_t sid,
                   bcmltm_nc_lta_validate_info_list_t *info_list,
                   char *type)
{
    uint32_t i;
    char *prefix;
    bcmltm_nc_lta_validate_info_t *info;

    cli_out("Field %s Validations: %d\n", type, info_list->num);

    prefix = "         ";
    for (i = 0; i < info_list->num; i++) {
        info = &info_list->validate_info[i];

        cli_out("  [%3d]: ", i);

        cli_out("Working Buffer Block ID: %d\n", info->wb_block_id);

        validate_params_dump(info->validate_params, prefix);

        cli_out("%sIndexes: %d\n", prefix, info->total_indices);
        lta_field_list_dump(sid, info->fsrc, prefix, "Input", TRUE);
    }

    return;
}


/*!
 * \brief Display field validations mapping for given table ID.
 *
 * This routine displays the node cookie information related
 * to field validation map entries for keys and values
 * for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
validate_lists_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_nc_lta_validate_info_list_t *info_list;

    info_list = LT_INFO_VALIDATE_KEY_LIST(unit, sid);
    if (info_list != NULL) {
        validate_list_dump(sid, info_list, "Key");
    }

    info_list = LT_INFO_VALIDATE_VALUE_LIST(unit, sid);
    if (info_list != NULL) {
        validate_list_dump(sid, info_list, "Value");
    }

    info_list = LT_INFO_VALIDATE_COPY_LIST(unit, sid);
    if (info_list != NULL) {
        validate_list_dump(sid, info_list, "Copy");
    }

    return;
}


/*!
 * \brief Display given LTA transform params structure.
 *
 * This routine displays the given LTA transform params information.
 *
 * \param [in] params LTA transform params structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
xfrm_params_dump(bcmltm_lta_transform_params_t *params, char *prefix)
{
    char arg_prefix[30];

    cli_out("%sParams:\n", prefix);
    if (params == NULL) {
        return;
    }

    cli_out("%s  val_f=0x%lx  fields_in_ofs=%-5d fields_out_ofs=%-5d\n",
            prefix,
            (unsigned long)params->lta_transform,
            params->fields_input_offset,
            params->fields_output_offset);
    cli_out("%s  pt_sid_sel_ofs=%-5d pt_sid_over_ofs=%-5d\n",
            prefix,
            params->pt_sid_selector_offset,
            params->pt_sid_override_offset);
    cli_out("%s  index_absent_ofs=%-5d\n",
            prefix,
            params->index_absent_offset);

    sal_sprintf(arg_prefix, "%s  ", prefix);
    ltd_transform_arg_dump(params->lta_args, arg_prefix);

    return;
}


/*!
 * \brief Display given field transform list.
 *
 * This routine displays the node cookie information related
 * to field transform map entries.
 *
 * \param [in] sid Table ID.
 * \param [in] info_list List of field transform.
 * \param [in] type Field Transform type string.
 * \param [in] forward Indicates transform is forward or reverse.
 */
static void
xfrm_list_dump(bcmlrd_sid_t sid,
               bcmltm_nc_lta_xfrm_info_list_t *info_list,
               char *type,
               bool forward)
{
    bcmltm_nc_lta_xfrm_info_t *info;
    bcmltm_lta_transform_params_t *params;
    uint32_t i;
    char *prefix;
    bool in_logical;
    bool out_logical;

    if (forward) {
        in_logical = TRUE;
        out_logical = FALSE;
    } else {
        in_logical = FALSE;
        out_logical = TRUE;
    }

    cli_out("Field %s Transforms: %d\n", type, info_list->num);

    prefix = "         ";
    for (i = 0; i < info_list->num; i++) {
        info = &info_list->xfrm_info[i];

        cli_out("  [%3d]: ", i);

        cli_out("Working Buffer Block ID: %d\n", info->wb_block_id);

        params = info->xfrm_params;
        xfrm_params_dump(params, prefix);

        lta_field_list_dump(sid, info->fin, prefix, "Input", in_logical);
        lta_field_list_dump(sid, info->fout, prefix, "Output", out_logical);
    }

    return;
}

/*!
 * \brief Display field transform mappings for given table ID.
 *
 * This routine displays the node cookie information related
 * to field transform map entries, forward and reverse,
 * for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
xfrm_lists_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_nc_lta_xfrm_info_list_t *info_list;

    info_list = LT_INFO_FWD_KEY_XFRM_INFO_LIST(unit, sid);
    if (info_list != NULL) {
        xfrm_list_dump(sid, info_list, "Forward Key", TRUE);
    }

    info_list = LT_INFO_FWD_VALUE_XFRM_INFO_LIST(unit, sid);
    if (info_list != NULL) {
        xfrm_list_dump(sid, info_list, "Forward Value", TRUE);
    }

    info_list = LT_INFO_REV_KEY_XFRM_INFO_LIST(unit, sid);
    if (info_list != NULL) {
        xfrm_list_dump(sid, info_list, "Reverse Key", FALSE);
    }

    info_list = LT_INFO_REV_VALUE_XFRM_INFO_LIST(unit, sid);
    if (info_list != NULL) {
        xfrm_list_dump(sid, info_list, "Reverse Value", FALSE);
    }

    info_list = LT_INFO_COPY_XFRM_INFO_LIST(unit, sid);
    if (info_list != NULL) {
        xfrm_list_dump(sid, info_list, "Copy", TRUE);
    }

    return;
}


/*!
 * \brief Display LT PT groups info for given table ID.
 *
 * This routine displays the node cookie information related
 * to PT map groups defined for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
pt_info_dump(int unit, bcmlrd_sid_t sid)
{

    cli_out("--- PT Groups Info ---\n");

    if (LT_INFO_PT(unit, sid) == NULL) {
        return;
    }

    /* LT index working buffer block ID */
    cli_out("LT Index Working Buffer Block ID: %d\n",
            LT_INFO_PT(unit, sid)->lt_index_wb_block_id);

    /* FA LT index */
    pt_fa_lt_index_dump(unit, sid);

    /* EE LT index */
    ee_lt_index_dump(unit, sid);

    /* LT to PT index wb copy */
    lt_to_pt_index_dump(unit, sid);

    /* PT List (Index) */
    pt_list_dump(unit, sid);

    /* PT Keyed List */
    pt_keyed_list_dump(unit, sid);

    /* PTM Mappings: Key and Value fields */
    ptm_mappings_dump(unit, sid);

    /* Fixed fields */
    fixed_fields_dump(unit, sid);

    /* Field Transforms */
    xfrm_lists_dump(unit, sid);

    return;
}

/*!
 * \brief Display LT FV groups info for given table ID.
 *
 * This routine displays the node cookie information related
 * to FV map groups defined for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
fv_info_dump(int unit, bcmlrd_sid_t sid)
{

    cli_out("--- FV Groups Info ---\n");

    if (LT_INFO_FV(unit, sid) == NULL) {
        return;
    }

    /* Field Validations */
    validate_lists_dump(unit, sid);

    return;
}


/*!
 * \brief Display LTD table commit handler structure.
 *
 * This routine displays the given LTD table commit handler structure.
 *
 * \param [in] handler LTD table commit handler structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
ltd_table_commit_handler_dump(const bcmltd_table_commit_handler_t *handler,
                              char *prefix)
{
    char arg_prefix[30];

    cli_out("handler (0x%lx):\n", (unsigned long)handler);
    if (handler == NULL) {
        return;
    }

    cli_out("%s  prepare_f=0x%lx commit_f=0x%lx commit_f=0x%lx\n",
            prefix,
            (unsigned long)handler->prepare,
            (unsigned long)handler->commit,
            (unsigned long)handler->abort);

    sal_sprintf(arg_prefix, "%s  ", prefix);
    ltd_generic_arg_dump(handler->arg, arg_prefix);

    return;
}


/*!
 * \brief Display LTM Table Commit list for given table ID.
 *
 * This routine displays the list of Table Commit handler list
 * for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
tc_handler_list_dump(int unit, bcmlrd_sid_t sid)
{
    uint32_t i;
    bcmltm_table_commit_list_t *tc_list;

    tc_list = LT_INFO_TC_HANDLER_LIST(unit, sid);
    if (tc_list == NULL) {
        return;
    }

    cli_out("Table Commit List: %d\n", tc_list->num_handlers);
    for (i = 0; i < tc_list->num_handlers; i++) {
        cli_out("  [%3d]: ", i);
        ltd_table_commit_handler_dump(tc_list->handlers[i], "         ");
    }

    return;
}


/*!
 * \brief Display LT Table Commit groups info for given table ID.
 *
 * This routine displays the node cookie information related
 * to table commit interfaces for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
tc_info_dump(int unit, bcmlrd_sid_t sid)
{

    cli_out("--- Table Commit Groups Info ---\n");

    if (LT_INFO_TC(unit, sid) == NULL) {
        return;
    }

    /* Table Commit Handlers */
    tc_handler_list_dump(unit, sid);

    return;
}


/*!
 * \brief Display LTD table handler structure.
 *
 * This routine displays the given LTD table handler structure.
 *
 * \param [in] handler LTD table handler structure to display.
 * \param [in] prefix Prefix indentation for output.
 */
static void
ltd_table_handler_dump(const bcmltd_table_handler_t *handler, char *prefix)
{
    char arg_prefix[30];

    cli_out("%shandler (0x%lx):\n", prefix, (unsigned long)handler);
    if (handler == NULL) {
        return;
    }

    cli_out("%s  validate_f=0x%lx\n",
            prefix,
            (unsigned long)handler->validate);
    cli_out("%s  insert_f=0x%lx lookup_f=0x%lx\n",
            prefix,
            (unsigned long)handler->insert,
            (unsigned long)handler->lookup);
    cli_out("%s  delete_f=0x%lx update_f=0x%lx\n",
            prefix,
            (unsigned long)handler->delete,
            (unsigned long)handler->update);

    cli_out("%s  traverse_f=0x%lx ha_f=0x%lx\n",
            prefix,
            (unsigned long)handler->traverse,
            (unsigned long)handler->ha);

    sal_sprintf(arg_prefix, "%s  ", prefix);
    ltd_generic_arg_dump(handler->arg, arg_prefix);

    return;
}


/*!
 * \brief Display LT Custom Table Handler groups info for given table ID.
 *
 * This routine displays the node cookie information related
 * to Custom Table Handler map groups defined for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
cth_info_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_lta_table_params_t *table_params;
    bcmltm_lta_field_list_t *lta_fields;

    cli_out("--- LTA Custom Table Handler Group Info ---\n");

    if (LT_INFO_CTH(unit, sid) == NULL) {
        return;
    }

    /* CTH Working Buffer */
    cli_out("Working Buffer Block ID: %d\n",
            LT_INFO_CTH(unit, sid)->wb_block_id);

    /* CTH Table Parameters */
    table_params = LT_INFO_LTA_CTH_TABLE_PARAMS(unit, sid);
    if (table_params != NULL) {
        cli_out("Table Parameters:\n");
        cli_out("  fields_in_ofs=%-5d fields_out_ofs=%-5d\n",
                table_params->fields_input_offset,
                table_params->fields_output_offset);

        ltd_table_handler_dump(table_params->handler, "  ");
    }

    /* CTH Input field list */
    lta_fields = LT_INFO_LTA_CTH_IN_FIELDS(unit, sid);
    lta_field_list_dump(sid, lta_fields, "", "Input", TRUE);

    /* CTH Output field list */
    lta_fields = LT_INFO_LTA_CTH_OUT_FIELDS(unit, sid);
    lta_field_list_dump(sid, lta_fields, "", "Output", TRUE);

    return;
}


/*!
 * \brief Display LTM FA LT index information for given table ID.
 *
 * This routine displays the node cookie information used
 * for handling the FA LT index for a given table ID that has
 * LTM group map.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
ltm_fa_lt_index_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_fa_lt_index_t *info;

    info = LT_INFO_LTM_FA_LT_INDEX(unit, sid);
    fa_lt_index_dump(info);

    return;
}


/*!
 * \brief Display field spec list.
 *
 * This routine displays the field spec list.
 *
 * \param [in] list Field spec list to display.
 */
static void
field_spec_list_dump(bcmltm_field_spec_list_t *list)
{
    uint32_t i;

    cli_out("LTM Group Field Specifier List:\n");
    cli_out("  ltid_ofs=%d\n", list->ltid_offset);

    cli_out("  Field Specifiers: %d\n", list->num_field_specs);
    for (i = 0; i < list->num_field_specs; i++) {
        cli_out("    id=%-5d index=%-3d\n",
                list->field_specs[i].field_id,
                list->field_specs[i].field_idx);
    }

    return;
}


/*!
 * \brief Display LTM groups info for given table ID.
 *
 * This routine displays the node cookie information related
 * to LTM map groups defined for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
ltm_info_dump(int unit, bcmlrd_sid_t sid)
{
    bcmltm_to_ptm_mapping_t *ltm_key;
    bcmltm_field_spec_list_t *fs_list;

    cli_out("--- LTA LTM Group Info ---\n");

    if (LT_INFO_LTM(unit, sid) == NULL) {
        return;
    }

    /* LTM Working Buffer */
    cli_out("Working Buffer Block ID: %d\n",
            LT_INFO_LTM(unit, sid)->wb_block_id);

    /* LTM fa lt index */
    ltm_fa_lt_index_dump(unit, sid);

    /* LTM keys */
    ltm_key = LT_INFO_LTM_KEY(unit, sid);
    ptm_mapping_dump(sid, ltm_key, "LTM Group Key");

    /* LTM Field spec list */
    fs_list = LT_INFO_LTM_FS_LIST(unit, sid);
    field_spec_list_dump(fs_list);

    return;
}


/*!
 * \brief Check that unit and given table ID are valid.
 *
 * Validates:
 *    - LT node cookie information has been initalized.
 *    - Logical Table ID is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_info_valid(int unit, bcmlrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);

    if (!LT_INFO_INIT(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (!LT_SID_VALID(unit, sid)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy LT information for given table ID.
 *
 *  Destroy LT information for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
static void
bcmltm_nc_lt_info_destroy(int unit, bcmlrd_sid_t sid)
{
    /* Destroy PT group info */
    bcmltm_nc_lt_pt_info_destroy(LT_INFO_PT(unit, sid));
    LT_INFO_PT(unit, sid) = NULL;

    /* Destroy LTM group info */
    bcmltm_nc_lt_ltm_info_destroy(LT_INFO_LTM(unit, sid));
    LT_INFO_LTM(unit, sid) = NULL;

    /* Destroy Custom Table Handler group info */
    bcmltm_nc_lta_cth_info_destroy(LT_INFO_CTH(unit, sid));
    LT_INFO_CTH(unit, sid) = NULL;

    /* Destroy Table Commit group info */
    bcmltm_nc_lt_tc_info_destroy(LT_INFO_TC(unit, sid));
    LT_INFO_TC(unit, sid) = NULL;

    /* Destroy Field Validation group info */
    bcmltm_nc_lt_fv_info_destroy(LT_INFO_FV(unit, sid));
    LT_INFO_FV(unit, sid) = NULL;

    /* Destroy Group Mappings */
    bcmltm_nc_lt_map_groups_cleanup(&LT_INFO_GROUPS(unit, sid));

    return;
}

/*!
 * \brief Create LT information for given table ID.
 *
 * Create LT information for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_nc_lt_info_create(int unit, bcmlrd_sid_t sid)
{
    const bcmltm_map_groups_t *groups;
    const bcmltm_table_attr_t *attr;

    SHR_FUNC_ENTER(unit);

    /* Get Table Attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_map_attr_parse(unit, sid,
                                     &LT_INFO_ATTR(unit, sid)));
    attr = &LT_INFO_ATTR(unit, sid);

    /* Get LT Group Mappings */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_map_groups_parse(unit, sid, attr,
                                       &LT_INFO_GROUPS(unit, sid)));
    groups = &LT_INFO_GROUPS(unit, sid);

    if ((groups->fv != NULL) && (groups->fv->num > 0)) {
        /* Create nc info for Field Validation group */
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lt_fv_info_create(unit, sid, groups->fv,
                                         &LT_INFO_FV(unit, sid)));

    }

    /* Create LT information for Table Commit group */
    if ((groups->tc != NULL) && (groups->tc->num > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lt_tc_info_create(unit, sid, groups->tc,
                                         &LT_INFO_TC(unit, sid)));

    }

    /*
     * Create LT Node Cookies information.
     * For the moment, allow only ONE type of map group kind.
     *
     * Order of priority:
     *   - Custom Table Handler group
     *   - LTM group
     *   - PT groups (direct PT map)
     */
    if ((groups->cth != NULL) && (groups->cth->num > 0)) {
        /* Create nc info for Custom Table Handler group */
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lta_cth_info_create(unit, sid, groups->cth,
                                           &LT_INFO_CTH(unit, sid)));
    } else if ((groups->ltm != NULL) && (groups->ltm->num > 0)) {
        /* Create nc info for LTM group */
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lt_ltm_info_create(unit, sid, groups->ltm,
                                          &LT_INFO_LTM(unit, sid)));
    } else if ((groups->pt != NULL) && (groups->pt->num_pt > 0)) {
        /* Create nc info for PT groups */
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lt_pt_info_create(unit, sid, attr, groups->pt,
                                         &LT_INFO_PT(unit, sid)));

        if ((groups->fv != NULL) && (groups->fv->num > 0)) {
            /* Create LTA Validate Copy Field Information (FA node cookie) */
            SHR_IF_ERR_EXIT
                (bcmltm_nc_lta_validate_copy_info_create(unit, sid,
                           LT_INFO_PT(unit, sid)->rev_value_xfrm_info_list,
                           LT_INFO_FV(unit, sid)->validate_values,
                       &(LT_INFO_FV(unit, sid)->copy_validate_info_list)));
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lt_info_destroy(unit, sid);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_nc_lt_info_init(int unit, uint32_t sid_max_count)
{
    lt_info_list_t *info = NULL;
    unsigned int size;
    bcmlrd_sid_t *sid_list = NULL;
    size_t num_sid;
    size_t idx;
    size_t sid;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Check if it has been initialized */
    if (LT_INFO_INIT(unit)) {
        SHR_EXIT();
    }

    /* Allocate */
    size = sizeof(*info) + (sizeof(lt_info_t) * sid_max_count);
    SHR_ALLOC(info, size, "LTM Logical Table info");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, size);
    LT_INFO(unit) = info;

    /* Initialize information */
    info->sid_max_count = sid_max_count;

    /* Allocate sid list array */
    size = sizeof(*sid_list) * sid_max_count;
    SHR_ALLOC(sid_list, size, "LTM Logical Table sid_list");
    SHR_NULL_CHECK(sid_list, SHR_E_MEMORY);
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    rv = bcmlrd_table_list_get(unit, sid_max_count, sid_list, &num_sid);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            /* No tables are defined */
            num_sid = 0;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Could not get table ID list rv=%d\n"),
                       rv));
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    /* Create LT NC info for each table */
    for (idx = 0; idx < num_sid; idx++) {
        sid = sid_list[idx];
        /* Create LT info */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmltm_nc_lt_info_create(unit, sid), SHR_E_UNAVAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_nc_lt_info_cleanup(unit);
    }

    SHR_FREE(sid_list);
    SHR_FUNC_EXIT();
}


void
bcmltm_nc_lt_info_cleanup(int unit)
{
    bcmlrd_sid_t sid;
    bcmlrd_sid_t sid_max;

    if (!LT_INFO_INIT(unit)) {
        return;
    }

    /*
     * This implementation is based upon the symbols IDs
     * being in a compact numbering space from [0 .. (sid_max_count-1)]
     */
    sid_max = LT_INFO_SID_MAX_COUNT(unit) - 1;

    /* Destroy internal information */
    for (sid = 0; sid <= sid_max; sid++) {
        /* LRD symbol ID is the same */
        bcmltm_nc_lt_info_destroy(unit, sid);
    }

    SHR_FREE(LT_INFO(unit));
    LT_INFO(unit) = NULL;

    return;
}



int
bcmltm_nc_lt_info_table_attr_get(int unit, bcmlrd_sid_t sid,
                                 bcmltm_table_attr_t *attr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    sal_memcpy(attr, &LT_INFO_ATTR(unit, sid), sizeof(*attr));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_table_type_get(int unit, bcmlrd_sid_t sid,
                                 bcmltm_table_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *type = LT_INFO_ATTR(unit, sid).type;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_map_groups_retrieve(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_map_groups_t **map_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *map_ptr = &LT_INFO_GROUPS(unit, sid);

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_map_pt_retrieve(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_map_pt_t **pt_map_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *pt_map_ptr = LT_INFO_GROUPS(unit, sid).pt;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_pt_list_retrieve(int unit, bcmlrd_sid_t sid,
                                   const bcmltm_pt_list_t **pt_list_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT(unit, sid) == NULL) {
        *pt_list_ptr = NULL;
    } else {
        *pt_list_ptr = LT_INFO_PT_LIST(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_pt_keyed_retrieve(int unit, bcmlrd_sid_t sid,
                                    bcmptm_op_type_t ptm_op,
                              const bcmltm_pt_keyed_op_info_t **pt_keyed_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT_KEYED_LIST(unit, sid) == NULL) {
        *pt_keyed_ptr = NULL;
    } else {
        uint32_t idx;

        switch (ptm_op) {
        case BCMPTM_OP_INSERT:
        case BCMPTM_OP_DELETE:
            idx = BCMLTM_NC_PT_KEYED_INS_DEL;
        break;

        case BCMPTM_OP_LOOKUP:
        case BCMPTM_OP_GET_NEXT:
        case BCMPTM_OP_GET_FIRST:
            idx = BCMLTM_NC_PT_KEYED_LOOK_TRAV;
            break;

        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            break;
        }

        *pt_keyed_ptr = LT_INFO_PT_KEYED(unit, sid, idx);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_fa_lt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                       const bcmltm_fa_lt_index_t **index_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT(unit, sid) == NULL) {
        *index_ptr = NULL;
    } else {
        *index_ptr = LT_INFO_FA_LT_INDEX(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_ee_lt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                       const bcmltm_ee_index_info_t **index_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT(unit, sid) == NULL) {
        *index_ptr = NULL;
    } else {
        *index_ptr = LT_INFO_EE_LT_INDEX(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_lt_to_pt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                    const bcmltm_wb_copy_list_t **list_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT(unit, sid) == NULL) {
        *list_ptr = NULL;
    } else {
        *list_ptr = LT_INFO_LT_TO_PT_INDEX(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_ptm_mapping_retrieve(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_field_type_t field_type,
                                       bcmltm_field_dir_t field_dir,
                                       const bcmltm_to_ptm_mapping_t
                                       **ptm_mapping_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT(unit, sid) == NULL) {
        *ptm_mapping_ptr = NULL;
    } else {
        if (field_type == BCMLTM_FIELD_TYPE_KEY) {
            if (field_dir == BCMLTM_FIELD_DIR_IN) {
                *ptm_mapping_ptr = LT_INFO_PTM_MAPPING_KEYS_IN(unit, sid);
            } else {
                *ptm_mapping_ptr = LT_INFO_PTM_MAPPING_KEYS_OUT(unit, sid);
            }
        } else {
            /* Both input and output are the same for values */
            *ptm_mapping_ptr = LT_INFO_PTM_MAPPING_VALUES(unit, sid);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_ptm_mapping_keys_alloc_retrieve(int unit,
                                                  bcmlrd_sid_t sid,
                                                  bool required,
                                                  const bcmltm_to_ptm_mapping_t
                                                  **ptm_mapping_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT(unit, sid) == NULL) {
        *ptm_mapping_ptr = NULL;
    } else {
        *ptm_mapping_ptr = required ?
            LT_INFO_PTM_MAPPING_KEYS_ALLOC_REQUIRED(unit, sid):
            LT_INFO_PTM_MAPPING_KEYS_ALLOC_OPTIONAL(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_info_fixed_fields_retrieve(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_fixed_field_list_t
                                        **fixed_fields_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_PT(unit, sid) == NULL) {
        *fixed_fields_ptr = NULL;
    } else {
        *fixed_fields_ptr = LT_INFO_FIXED_FIELDS(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_validate_field_num_get(int unit, bcmlrd_sid_t sid,
                                     bcmltm_field_type_t field_type,
                                     uint32_t *num)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *num = 0;
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        if ((LT_INFO_FV(unit, sid) != NULL) &&
            (LT_INFO_VALIDATE_KEY_LIST(unit, sid) != NULL)) {
            *num = LT_INFO_VALIDATE_KEY_NUM(unit, sid);
        }
    } else {
        if ((LT_INFO_FV(unit, sid) != NULL) &&
            (LT_INFO_VALIDATE_VALUE_LIST(unit, sid) != NULL)) {
            *num = LT_INFO_VALIDATE_VALUE_NUM(unit, sid);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_validate_params_retrieve(int unit, bcmlrd_sid_t sid,
                                       bcmltm_field_type_t field_type,
                                       uint32_t fld_idx,
                                       const bcmltm_lta_validate_params_t
                                       **table_params_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        if ((LT_INFO_VALIDATE_KEY_LIST(unit, sid) == NULL) ||
            (fld_idx >= LT_INFO_VALIDATE_KEY_NUM(unit, sid))) {
            *table_params_ptr = NULL;
        } else {
            *table_params_ptr = LT_INFO_LTA_VALIDATE_KEY_PARAMS(unit, sid,
                                                                fld_idx);
        }
    } else {
        if ((LT_INFO_VALIDATE_VALUE_LIST(unit, sid) == NULL) ||
            (fld_idx >= LT_INFO_VALIDATE_VALUE_NUM(unit, sid))) {
            *table_params_ptr = NULL;
        } else {
            *table_params_ptr = LT_INFO_LTA_VALIDATE_VALUE_PARAMS(unit, sid,
                                                                  fld_idx);
        }
    }
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_validate_src_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                           bcmltm_field_type_t field_type,
                                           uint32_t fld_idx,
                                           const bcmltm_lta_field_list_t
                                           **fields_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        if ((LT_INFO_VALIDATE_KEY_LIST(unit, sid) == NULL) ||
            (fld_idx >= LT_INFO_VALIDATE_KEY_NUM(unit, sid))) {
            *fields_ptr = NULL;
        } else {
            *fields_ptr = LT_INFO_LTA_VALIDATE_KEY_SRC_FIELDS(unit, sid,
                                                              fld_idx);
        }
    } else {
        if ((LT_INFO_VALIDATE_VALUE_LIST(unit, sid) == NULL) ||
            (fld_idx >= LT_INFO_VALIDATE_VALUE_NUM(unit, sid))) {
            *fields_ptr = NULL;
        } else {
            *fields_ptr = LT_INFO_LTA_VALIDATE_VALUE_SRC_FIELDS(unit, sid,
                                                                fld_idx);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_validate_copy_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                            bcmltm_field_type_t field_type,
                                            uint32_t validate_idx,
                                            int from_xfrm_type,
                                            uint32_t from_xfrm_idx,
                                            const bcmltm_lta_field_list_t
                                            **fields_ptr)
{
    uint32_t num_rev = 0;
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (from_xfrm_type != BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if ((LT_INFO_PT(unit, sid) != NULL) &&
        (LT_INFO_REV_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
        (from_xfrm_idx < LT_INFO_REV_VALUE_XFRM_NUM(unit, sid))) {
        num_rev = LT_INFO_REV_VALUE_XFRM_NUM(unit, sid);
    }

    if ((LT_INFO_VALIDATE_COPY_LIST(unit, sid) == NULL) ||
        (validate_idx >= LT_INFO_VALIDATE_COPY_NUM(unit, sid))) {
        *fields_ptr = NULL;
    } else {
        *fields_ptr = LT_INFO_LTA_VALIDATE_COPY_FIELDS(unit, sid,
                              from_xfrm_idx, num_rev, validate_idx);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_xfrm_num_get(int unit, bcmlrd_sid_t sid,
                           int xfrm_type,
                           uint32_t *num)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *num = 0;
    if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        if (LT_INFO_PT(unit, sid) != NULL &&
            LT_INFO_FWD_KEY_XFRM_INFO_LIST(unit, sid) != NULL) {
            *num = LT_INFO_FWD_KEY_XFRM_NUM(unit, sid);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        if (LT_INFO_PT(unit, sid) != NULL &&
            LT_INFO_FWD_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) {
            *num = LT_INFO_FWD_VALUE_XFRM_NUM(unit, sid);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM) {
        if (LT_INFO_PT(unit, sid) != NULL &&
            LT_INFO_REV_KEY_XFRM_INFO_LIST(unit, sid) != NULL) {
            *num = LT_INFO_REV_KEY_XFRM_NUM(unit, sid);
        }
    } else {
        if (LT_INFO_PT(unit, sid) != NULL &&
            LT_INFO_REV_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) {
            *num = LT_INFO_REV_VALUE_XFRM_NUM(unit, sid);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lta_xfrm_params_retrieve(int unit, bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   uint32_t xfrm_idx,
                                   const bcmltm_lta_transform_params_t
                                   **table_params_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *table_params_ptr = NULL;
    if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_FWD_KEY_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_FWD_KEY_XFRM_NUM(unit, sid))) {
            *table_params_ptr = LT_INFO_LTA_FWD_KEY_XFRM_TABLE_PARAMS(unit, sid, xfrm_idx);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_FWD_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_FWD_VALUE_XFRM_NUM(unit, sid))) {
            *table_params_ptr = LT_INFO_LTA_FWD_VALUE_XFRM_TABLE_PARAMS(unit, sid, xfrm_idx);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_REV_KEY_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_REV_KEY_XFRM_NUM(unit, sid))) {
            *table_params_ptr = LT_INFO_LTA_REV_KEY_XFRM_TABLE_PARAMS(unit, sid, xfrm_idx);
        }
    } else {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_REV_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_REV_VALUE_XFRM_NUM(unit, sid))) {
            *table_params_ptr = LT_INFO_LTA_REV_VALUE_XFRM_TABLE_PARAMS(unit, sid, xfrm_idx);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_xfrm_in_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                      int xfrm_type,
                                      uint32_t xfrm_idx,
                                      const bcmltm_lta_field_list_t
                                      **fields_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *fields_ptr = NULL;
    if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_FWD_KEY_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_FWD_KEY_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_FWD_KEY_XFRM_IN_FIELDS(unit, sid, xfrm_idx);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_FWD_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_FWD_VALUE_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_FWD_VALUE_XFRM_IN_FIELDS(unit, sid, xfrm_idx);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_REV_KEY_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_REV_KEY_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_REV_KEY_XFRM_IN_FIELDS(unit, sid, xfrm_idx);
        }
    } else {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_REV_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_REV_VALUE_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_REV_VALUE_XFRM_IN_FIELDS(unit, sid, xfrm_idx);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_xfrm_out_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                       int xfrm_type,
                                       uint32_t xfrm_idx,
                                       const bcmltm_lta_field_list_t
                                       **fields_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *fields_ptr = NULL;
    if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_FWD_KEY_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_FWD_KEY_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_FWD_KEY_XFRM_OUT_FIELDS(unit, sid, xfrm_idx);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_FWD_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_FWD_VALUE_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_FWD_VALUE_XFRM_OUT_FIELDS(unit, sid, xfrm_idx);
        }
    } else if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM) {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_REV_KEY_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_REV_KEY_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_REV_KEY_XFRM_OUT_FIELDS(unit, sid, xfrm_idx);
        }
    } else {
        if ((LT_INFO_PT(unit, sid) != NULL) &&
            (LT_INFO_REV_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_REV_VALUE_XFRM_NUM(unit, sid))) {
            *fields_ptr = LT_INFO_LTA_REV_VALUE_XFRM_OUT_FIELDS(unit, sid, xfrm_idx);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_xfrm_copy_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                        int xfrm_type,
                                        uint32_t xfrm_idx,
                                        int from_xfrm_type,
                                        uint32_t from_xfrm_idx,
                                        const bcmltm_lta_field_list_t
                                        **fields_ptr)
{
    uint32_t num_fwd = 0;
    uint32_t num_rev = 0;
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    *fields_ptr = NULL;
    if (xfrm_type != BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (from_xfrm_type != BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    if (LT_INFO_PT(unit, sid) == NULL) {
        /* Nothing to do */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if ((LT_INFO_REV_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
        (from_xfrm_idx < LT_INFO_REV_VALUE_XFRM_NUM(unit, sid))) {
        num_rev = LT_INFO_REV_VALUE_XFRM_NUM(unit, sid);
    }

    if ((LT_INFO_FWD_VALUE_XFRM_INFO_LIST(unit, sid) != NULL) &&
        (xfrm_idx < LT_INFO_FWD_VALUE_XFRM_NUM(unit, sid))) {
        num_fwd = LT_INFO_FWD_VALUE_XFRM_NUM(unit, sid);
    }

    if ((num_fwd != 0) && (num_rev != 0)) {
        if ((LT_INFO_COPY_XFRM_INFO_LIST(unit, sid) != NULL) &&
            (xfrm_idx < LT_INFO_COPY_XFRM_NUM(unit, sid))) {
            *fields_ptr =
                LT_INFO_LTA_COPY_XFRM_FIELDS(unit, sid, from_xfrm_idx,
                                             num_rev, xfrm_idx);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_table_commit_list_retrieve(int unit, bcmlrd_sid_t sid,
                               const bcmltm_table_commit_list_t **tc_list_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_TC(unit, sid) == NULL) {
        *tc_list_ptr = NULL;
    } else {
        *tc_list_ptr = LT_INFO_TC_HANDLER_LIST(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lta_cth_handler_retrieve(int unit, bcmlrd_sid_t sid,
                                   const bcmltd_table_handler_t **handler_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_CTH(unit, sid) == NULL) {
        *handler_ptr = NULL;
    } else {
        *handler_ptr = LT_INFO_LTA_CTH_HANDLER(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lta_cth_table_params_retrieve(int unit, bcmlrd_sid_t sid,
                                        const bcmltm_lta_table_params_t
                                        **table_params_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_CTH(unit, sid) == NULL) {
        *table_params_ptr = NULL;
    } else {
        *table_params_ptr = LT_INFO_LTA_CTH_TABLE_PARAMS(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lta_cth_in_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                     const bcmltm_lta_field_list_t
                                     **fields_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_CTH(unit, sid) == NULL) {
        *fields_ptr = NULL;
    } else {
        *fields_ptr = LT_INFO_LTA_CTH_IN_FIELDS(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_cth_in_key_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                         const bcmltm_lta_field_list_t
                                         **fields_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_CTH(unit, sid) == NULL) {
        *fields_ptr = NULL;
    } else {
        *fields_ptr = LT_INFO_LTA_CTH_IN_KEY_FIELDS(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lta_cth_out_fields_retrieve(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_lta_field_list_t
                                      **fields_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_CTH(unit, sid) == NULL) {
        *fields_ptr = NULL;
    } else {
        *fields_ptr = LT_INFO_LTA_CTH_OUT_FIELDS(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_ltm_fa_lt_index_retrieve(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_fa_lt_index_t **index_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_LTM(unit, sid) == NULL) {
        *index_ptr = NULL;
    } else {
        *index_ptr = LT_INFO_LTM_FA_LT_INDEX(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_nc_lt_ltm_key_retrieve(int unit, bcmlrd_sid_t sid,
                              const bcmltm_to_ptm_mapping_t **ptm_mapping_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_LTM(unit, sid) == NULL) {
        *ptm_mapping_ptr = NULL;
    } else {
        *ptm_mapping_ptr = LT_INFO_LTM_KEY(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_nc_lt_ltm_fs_list_retrieve(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_field_spec_list_t **fs_list_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Sanity check */
    SHR_IF_ERR_EXIT(lt_info_valid(unit, sid));

    if (LT_INFO_LTM(unit, sid) == NULL) {
        *fs_list_ptr = NULL;
    } else {
        *fs_list_ptr = LT_INFO_LTM_FS_LIST(unit, sid);
    }

 exit:
    SHR_FUNC_EXIT();
}

void
bcmltm_nc_lt_info_dump(int unit, bcmlrd_sid_t sid)
{
    const bcmlrd_table_rep_t *tbl;
    const char *name;

    /* Sanity check */
    if (!LT_INFO_INIT(unit)) {
        return;
    }

    if (!LT_SID_VALID(unit, sid)) {
        return;
    }

    tbl = bcmlrd_table_get(sid);
    if (tbl == NULL) {
        name = "";
    } else {
        name = tbl->name;
    }

    cli_out("=== LT ID %d: %s ===\n", sid, name);

    /* Table Attributes */
    attr_dump(unit, sid);
    cli_out("\n");

    /* Working Buffer information */
    wb_dump(unit, sid);
    cli_out("\n");

    /* Groups Map Information */
    groups_dump(unit, sid);
    cli_out("\n");

    /* PT Groups Info */
    pt_info_dump(unit, sid);
    cli_out("\n");

    /* Field Validation Group Info */
    fv_info_dump(unit, sid);
    cli_out("\n");

    /* Table Commit Info */
    tc_info_dump(unit, sid);
    cli_out("\n");

    /* Custom Table Handler Group Info */
    cth_info_dump(unit, sid);
    cli_out("\n");

    /* LTM Group Info */
    ltm_info_dump(unit, sid);
    cli_out("\n");

    return;
}


void
bcmltm_nc_lt_info_all_dump(int unit)
{
    bcmlrd_sid_t sid;

    if (!LT_INFO_INIT(unit)) {
        return;
    }

    for (sid = 0; sid < LT_INFO_SID_MAX_COUNT(unit); sid++) {
        bcmltm_nc_lt_info_dump(unit, sid);
    }

    return;
}


void
bcmltm_nc_lt_info_dump_by_name(int unit, const char *name)
{
    uint32_t ltid_count;
    bcmlrd_sid_t ltid;
    const bcmlrd_table_rep_t *tbl;

    ltid_count = bcmltm_logical_table_count_get();

    for (ltid = 0; ltid < ltid_count; ltid++) {
        tbl = bcmlrd_table_get(ltid);
        if (tbl == NULL) {
            return;
        }

        if (sal_strcmp(tbl->name, name) == 0) {
            bcmltm_nc_lt_info_dump(unit, ltid);
        }
    }

    return;
}

