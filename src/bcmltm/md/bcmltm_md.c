/*! \file bcmltm_md.c
 *
 * Logical Table Manager Metadata.
 *
 * This file contains the initialization for the LTM metadata.
 *
 * The LTM support the following main table categories:
 *
 * 1) Physical Table Pass Through (PT Pass Thru)
 * This is the most basic Logical Table case.
 * Each physical symbol will have a 'logical table' representation,
 * with a 1-1 mapping of all its physical fields to logical fields.
 *
 * 2) Logical Table
 * In this category, the logical tables are defined through various
 * mechanisms to produce a mapping from the API fields to
 * either the physical resources on the device or an internal
 * database.
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

#include <bsl/bsl.h>

#include <bcmdrd_config.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_md_pthru_internal.h>
#include <bcmltm/bcmltm_md_logical_internal.h>
#include <bcmltm/bcmltm_md.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*
 * Check that table category is valid.
 */
#define LTM_MD_TABLE_CATG_VALID(_c)             \
    ((_c) < BCMLTM_TABLE_CATG_COUNT)


/*
 * Check that unit is valid.  Function exits if unit is invalid.
 * This macro needs to be used in conjunction with the SHR_ debug macros.
 */
#define LTM_MD_UNIT_CHECK(_u)                                    \
    do {                                                         \
        if (!BCMLTM_UNIT_VALID(_u)) {                            \
            LOG_ERROR(BSL_LOG_MODULE,                            \
                      (BSL_META_U((_u),                          \
                                  "Invalid unit %d\n"),          \
                       (_u)));                                   \
            SHR_RETURN_VAL_EXIT(SHR_E_UNIT);                     \
        }                                                        \
    } while (0)

/*
 * Check that table category is valid.  Function exits if table category
 * is invalid.
 * This macro needs to be used in conjunction with the SHR_ debug macros.
 */
#define LTM_MD_TABLE_CATG_CHECK(_u, _c)                             \
    do {                                                            \
        if (!LTM_MD_TABLE_CATG_VALID(_c)) {                         \
            LOG_ERROR(BSL_LOG_MODULE,                               \
                      (BSL_META_U((_u),                             \
                                  "Invalid table category %d\n"),   \
                       (_c)));                                      \
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                       \
        }                                                           \
    } while (0)


/* Table category string */
static char *table_catg_str[] = {
    "PT Pass Thru",
    "Logical"
};


/*******************************************************************************
 * Private functions
 */


/*!
 * \brief Destroy the given LTM metadata data structure on a given unit.
 *
 * Destroy the LTM metadata data structure and cleanup/free
 * any memory previously allocated during the create process.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] catg Table category (BCMLTM_TABLE_CATG_...).
 *
 * \retval SHR_E_NONE No errors
 * \retval !SHR_E_NONE Failure
 */
static int
ltm_md_destroy(int unit, bcmltm_table_catg_t catg)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM metadata destroy: %s(%d)\n"),
                 table_catg_str[catg], catg));

    /* Check table category */
    LTM_MD_TABLE_CATG_CHECK(unit, catg);

    switch(catg) {
    case BCMLTM_TABLE_CATG_PTHRU:
        SHR_IF_ERR_EXIT(bcmltm_md_pthru_destroy(unit));
        break;
    case BCMLTM_TABLE_CATG_LOGICAL:
        SHR_IF_ERR_EXIT(bcmltm_md_logical_destroy(unit));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the LTM metadata data structure on a given unit.
 *
 * Create the LTM metadata  for the specified table category on a given unit.
 * Memory is allocated if necessary.
 *
 * Assumes: unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [in] catg Table category (BCMLTM_TABLE_CATG_...).
 *
 * \retval SHR_E_NONE No errors
 * \retval !SHR_E_NONE Failure
 */
static int
ltm_md_create(int unit, bool warm, bcmltm_table_catg_t catg)
{
    SHR_FUNC_ENTER(unit);

    /* Check table category */
    LTM_MD_TABLE_CATG_CHECK(unit, catg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "LTM metadata create: %s(%d)\n"),
                 table_catg_str[catg], catg));

    switch(catg) {
    case BCMLTM_TABLE_CATG_PTHRU:
        SHR_IF_ERR_EXIT(bcmltm_md_pthru_create(unit));
        break;
    case BCMLTM_TABLE_CATG_LOGICAL:
        SHR_IF_ERR_EXIT(bcmltm_md_logical_create(unit, warm));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        break;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        ltm_md_destroy(unit, catg);
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "LTM Metadata Initialization\n")));

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Create LTM PT Pass Thru metadata */
    SHR_IF_ERR_EXIT
        (ltm_md_create(unit, warm, BCMLTM_TABLE_CATG_PTHRU));

    /* Create LTM Logical metadata */
    SHR_IF_ERR_EXIT
        (ltm_md_create(unit, warm, BCMLTM_TABLE_CATG_LOGICAL));

    /* Initialize handle for PT status IMM operation. */
    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_init(unit));

 exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmltm_md_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}


int
bcmltm_md_cleanup(int unit)
{
    int catg;
    int rv;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "LTM Metadata Cleanup\n")));

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    for (catg = 0; catg < BCMLTM_TABLE_CATG_COUNT; catg++) {
        rv = ltm_md_destroy(unit, catg);
        if (rv == SHR_E_UNAVAIL) {
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_CONT(rv);
    }

    /* Cleanup handle for PT status IMM operation. */
    SHR_IF_ERR_EXIT(bcmltm_table_op_pt_info_cleanup(unit));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_lt_retrieve(int unit, bcmltm_table_catg_t catg,
                      uint32_t ltid,
                      bcmltm_lt_md_t **ltm_md_ptr)
{
    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check table category */
    LTM_MD_TABLE_CATG_CHECK(unit, catg);

    switch(catg) {
    case BCMLTM_TABLE_CATG_PTHRU:
        SHR_IF_ERR_EXIT(bcmltm_md_pthru_lt_retrieve(unit, ltid, ltm_md_ptr));
        break;
    case BCMLTM_TABLE_CATG_LOGICAL:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmltm_md_logical_lt_retrieve(unit, ltid, ltm_md_ptr));
        break;
    default:
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
        break;
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pt_field_count_get(int unit, bcmdrd_sid_t sid, size_t *num_fid)
{
    const bcmltm_ptm_field_list_t *key_list;
    const bcmltm_ptm_field_list_t *value_list;
    size_t fid_count = 0;
    size_t map_idx;

    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check NULL */
    SHR_NULL_CHECK(num_fid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_key_field_list_retrieve(unit, sid,
                                                 &key_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_value_field_list_retrieve(unit, sid,
                                                   &value_list));

    /* Keys */
    for (map_idx = 0; map_idx < key_list->num_fields; map_idx++) {
        /* Count distint field IDs (1st index) */
        if (key_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_count++;
    }

    /* Values */
    for (map_idx = 0; map_idx < value_list->num_fields; map_idx++) {
        /* Count distint field IDs (1st index) */
        if (value_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_count++;
    }

    *num_fid = fid_count;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pt_field_list_get(int unit, bcmdrd_sid_t sid, size_t list_max,
                            bcmdrd_fid_t *fid_list, size_t *num_fid)
{
    const bcmltm_ptm_field_list_t *field_list;
    size_t fid_idx = 0;
    size_t map_idx;

    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    if (list_max == 0) {
        SHR_EXIT();
    }

    /* Check NULL */
    SHR_NULL_CHECK(fid_list, SHR_E_PARAM);
    SHR_NULL_CHECK(num_fid, SHR_E_PARAM);

    /* Keys */
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_key_field_list_retrieve(unit, sid,
                                                 &field_list));
    for (map_idx = 0;
         (fid_idx < list_max) && (map_idx < field_list->num_fields);
         map_idx++) {
        /* Get only maps with distinct field IDs (1st index) */
        if (field_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_list[fid_idx] = field_list->field_maps[map_idx].api_field_id;
        fid_idx++;
    }

    /* Values */
    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_value_field_list_retrieve(unit, sid,
                                                   &field_list));
    for (map_idx = 0;
         (fid_idx < list_max) && (map_idx < field_list->num_fields);
         map_idx++) {
        /* Get only maps with distinct field IDs (1st index) */
        if (field_list->field_maps[map_idx].field_idx > 0) {
            continue;
        }
        fid_list[fid_idx] = field_list->field_maps[map_idx].api_field_id;
        fid_idx++;
    }

    *num_fid = fid_idx;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_pt_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                            bcmdrd_sym_field_info_t *finfo)
{
    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check NULL */
    SHR_NULL_CHECK(finfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmltm_md_pthru_field_info_get(unit, sid, fid, finfo));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_xfrm_field_get(int unit, const bcmlrd_map_entry_t *entry, bool src,
                         uint32_t *num_fields,
                         const bcmltd_field_list_t **flist)
{
    const bcmltd_xfrm_handler_t *xfrm = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check unit */
    LTM_MD_UNIT_CHECK(unit);

    /* Check NULL */
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Transform Handler */
    xfrm = bcmltm_lta_intf_xfrm_handler_get(unit, entry->u.handler_id);
    SHR_NULL_CHECK(xfrm, SHR_E_PARAM);

    /* Forward transform */
    if ((entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||
        (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER)) {
        if (src) {
            *flist = xfrm->arg->field_list;
            *num_fields = xfrm->arg->fields;
        } else {
            *flist = xfrm->arg->rfield_list;
            *num_fields = xfrm->arg->rfields;
        }
    } else { /* Reverse transform */
        if (src) {
            *flist = xfrm->arg->rfield_list;
            *num_fields = xfrm->arg->rfields;
        } else {
            *flist = xfrm->arg->field_list;
            *num_fields = xfrm->arg->fields;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
