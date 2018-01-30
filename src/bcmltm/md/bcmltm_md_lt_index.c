/*! \file bcmltm_md_lt_index.c
 *
 * Logical Table Manager - Index Logical Table.
 *
 * This file contains implementation to support metadata construction
 * for Index Logical Tables (Simple Index and Index with Allocation)
 * with Physical Table destination groups mapping.
 *
 * LT Field Mappings
 * -----------------
 * LT field maps can fall into either of these categories:
 *   1) Full
 *      The LT mapping completely describes one or more physical tables.
 *      This means that all physical fields in a physical table is
 *      mapped within a LT.
 *
 *   2) Split-Entry
 *      PT fields are not fully mapped in a LT.
 *      The LT maps only to a subset of fields in the physical table(s),
 *      so the field mappings do not completely cover all the physical fields
 *      in the corresponding mapped PTs.
 *
 * Example for Full Map:
 *   PT1: has fields           PT1_F1 PT1_F2
 *   LT1: has fields mappes to PT1_F1 PT1_F2 (all the PT fields).
 *
 * Example for Split-Entry Map:
 *   PT1: has fields  PT1_F1 PT1_F2 PT1_F3 PT1_F4
 *   LTs: LT1: fields map to PT1_F1
 *        LT2: fields map to PT1_F2 and PT1_F3
 * Note that even if LT2 did not exist (i.e. only LT1 is defined),
 * LT1 will still need be handled as a LT with a split-entry PT map.
 *
 * The split-entry characteristic affects the tree structures for
 * the INSERT and DELETE opcodes slightly.  A split-entry mapped LT
 * needs an addition PTM read operation to get the entire HW entry
 * so only those unmapped PT fields are not modified.
 *
 * The LOOKUP and UPDATE tree structres are the same for split-entry
 * or full maps.
 *
 * FIXME:
 * Future improvement is to have an LRD indication to identify
 * if a LT mapping is either full or split.
 * For now, all the opcodes assume LTs have split-entry mapping.
 * This results in an extra PTM read operation on LTs that
 * do not need it (i.e. full map).
 *
 * Transforms and Validations
 * --------------------------
 * Field Transforms (Xfrm) and Field Validations clusters are
 * added to the different opcode trees as needed to process the
 * arguments.
 *
 * For more details, see corresponding description in routines
 * that create the clusters of Field Transform and Field Validation
 * trees.
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

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_map_types_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_op.h"
#include "bcmltm_md_lta_xfrm.h"
#include "bcmltm_md_lta_validate.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/* Number of roots for INSERT operation */
#define LT_OP_INSERT_NUM_ROOTS   2

/* Number of roots for LOOKUP operation */
#define LT_OP_LOOKUP_NUM_ROOTS   2

/* Number of roots for DELETE operation */
#define LT_OP_DELETE_NUM_ROOTS   2

/* Number of roots for UPDATE operation */
#define LT_OP_UPDATE_NUM_ROOTS   2

/* Number of roots for TRAVERSE operation */
#define LT_OP_TRAVERSE_NUM_ROOTS 2


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the FA node data for the FA LT Index.
 *
 * This routine creates the FA node data for the FA LT Index for given
 * logical table.
 *
 * This node is required to calculate the 'track' index which
 * is used by LTM to track if an LT entry exists.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] index_node_data_ptr Returning pointer to FA LT Index node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fa_lt_index_node_data_create(int unit,
                             bcmlrd_sid_t sid,
                             bcmltm_fa_node_t **index_node_data_ptr)
{
    bcmltm_fa_node_t *index_node_data = NULL;
    const bcmltm_fa_lt_index_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    *index_node_data_ptr = NULL;

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fa_lt_index_retrieve(unit, sid, &index_info));

    /* Check if node is needed */
    if (index_info == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit, bcmltm_fa_node_lt_index,
                                       (void *)index_info, &index_node_data));
    *index_node_data_ptr = index_node_data;

 exit:
    if (SHR_FUNC_ERR()) {
        if (index_node_data != NULL) {
            bcmltm_md_fa_node_data_destroy(index_node_data);
        }
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the EE node functions for PTM read and write access.
 *
 * This routine gets the EE node functions for the corresponding
 * PTM read and write interfaces based on the table operating mode,
 * Modeled or Interactive.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] read_func EE node function for PTM read operation.
 * \param [out] write_func EE node function for PTM write operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ee_node_ptm_func_get(int unit, bcmlrd_sid_t sid,
                     bcmltm_ee_node_f *read_func,
                     bcmltm_ee_node_f *write_func)
{
    bcmltm_table_attr_t table_attr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_attr_get(unit, sid, &table_attr));

    /* Set corresponding EE node function based on table operating mode */
    if (table_attr.mode == BCMLTM_TABLE_MODE_INTERACTIVE) {
        /* Interactive path */
        if (read_func != NULL) {
            *read_func = bcmltm_ee_node_schan_read;
        }
        if (write_func != NULL) {
            *write_func = bcmltm_ee_node_schan_write;
        }

    } else {
        /* Modeled path */
        if (read_func != NULL) {
            *read_func = bcmltm_ee_node_model_index_read;
        }
        if (write_func != NULL) {
            *write_func = bcmltm_ee_node_model_index_write;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create FA tree roots for INSERT on Simple Index Non-Overlay LT.
 *
 * This function creates the FA tree roots for a INSERT operation
 * on given Simple Index Non-Overlay logical table.
 *
 * Unspecified input fields values are reset to the SW
 * defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_simple_index_fa_node_roots_insert_create(int unit,
                                            bcmlrd_sid_t sid,
                                            uint32_t num_roots,
                                            bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *def_values_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *index_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *def_values_node_data = NULL;
    bcmltm_fa_node_t *fixed_node_data = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_fields = NULL;
    int xfrm_type;
    bcmltm_md_xfrm_default_t xfrm_default;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots INSERT");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * INSERT - Disjoint
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                             [Root:Index]
     *                                    (Calculate Index)
     *                                            |
     *                                            |
     *                                    +-------+-------+
     *                                    |               |
     *                                 [Keys]            ===
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                      ===
     *                     |
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Values]
     *                           (Convert API Values)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]            [Fixed]
     *                         |            (Set Fixed fields)
     *                         |
     *                +--------+--------+
     *                |                 |
     *        [Validate Values]        ===
     *                |
     *                |
     *        +-------+-------+
     *        |               |
     *  [Default Values]     ===
     *  (Set table values to
     *   SW defaults from LRD)
     *
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Index: Root
     *
     * Calculate the index used by LTM to track the entry.
     */
    SHR_IF_ERR_EXIT
        (fa_lt_index_node_data_create(unit, sid,
                                      &index_node_data));
    index_node = bcmltm_tree_node_create((void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);
    fa_node_roots[0] = index_node;

    /*
     * Tree_0 - Keys: Left node of Index
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_KEY,
                                                BCMLTM_FIELD_DIR_IN,
                                                &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_left_node_create(index_node,
                                             (void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_NONE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Convert API Value fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_VALUE,
                                                BCMLTM_FIELD_DIR_IN,
                                                &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_wb,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_ADD;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Validate Values: Left node of Xfrm Values
     *
     * Process field validations for Values.
     */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Default Values: Left node of Validate Values
     *
     * Set table values to SW defaults from LRD.
     *
     * Note that this node could be attached directly to Values
     * or to the Xfrm Values node depending on whether the table has
     * any value field transforms or validations.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_default_fields_to_wb,
                                       (void *)values_map,
                                       &def_values_node_data));
    def_values_node = bcmltm_tree_left_node_create(next_chain,
                                            (void *)def_values_node_data);
    SHR_NULL_CHECK(def_values_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Fixed Fields: Right node of Values
     *
     * Set Fixed fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fixed_fields_retrieve(unit, sid, &fixed_fields));
    if ((fixed_fields != NULL) && (fixed_fields->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_fields,
                                           &fixed_node_data));
    }
    fixed_node = bcmltm_tree_right_node_create(values_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);


    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((keys_node == NULL) && (keys_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_node_data);
        }
        if ((values_node == NULL) && (values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(values_node_data);
        }
        if ((def_values_node == NULL) && (def_values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(def_values_node_data);
        }
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create FA tree roots for INSERT on Simple Index Overlay LT.
 *
 * This function creates the FA tree roots for a INSERT operation
 * on given Simple Index Overlay logical table (shared index).
 *
 * Unspecified input fields values are reset to the SW
 * defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_simple_index_shared_fa_node_roots_insert_create(int unit,
                                                   bcmlrd_sid_t sid,
                                                   uint32_t num_roots,
                                  bcmltm_node_t ***fa_node_roots_ptr)
{
    /*
     * The Overlay and Non-Overlay INSERT FA trees
     * for Simple Index are the same.
     */
    return lt_simple_index_fa_node_roots_insert_create(unit, sid,
                                                       num_roots,
                                                       fa_node_roots_ptr);
}


/*!
 * \brief Create FA tree roots for INSERT on IwA Non-Overlay LT.
 *
 * This function creates the FA tree roots for a INSERT operation
 * on given Index with Allocation Non-Overlay logical table.
 *
 * Unspecified input fields values are reset to the SW
 * defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_alloc_index_fa_node_roots_insert_create(int unit,
                                            bcmlrd_sid_t sid,
                                            uint32_t num_roots,
                                            bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *lt_to_pt_index_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *keys_required_node = NULL;
    bcmltm_node_t *keys_optional_node = NULL;
    bcmltm_node_t *keys_optional_out_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *def_values_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *lt_to_pt_index_node_data = NULL;
    bcmltm_fa_node_t *index_node_data = NULL;
    bcmltm_fa_node_t *keys_required_node_data = NULL;
    bcmltm_fa_node_t *keys_optional_node_data = NULL;
    bcmltm_fa_node_t *keys_optional_out_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *def_values_node_data = NULL;
    bcmltm_fa_node_t *fixed_node_data = NULL;
    const bcmltm_wb_copy_list_t *lt_to_pt_index_list = NULL;
    const bcmltm_fa_lt_index_t *index_info = NULL;
    const bcmltm_to_ptm_mapping_t *keys_required_map = NULL;
    const bcmltm_to_ptm_mapping_t *keys_optional_map = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_fields = NULL;
    int xfrm_type;
    bcmltm_md_xfrm_default_t xfrm_default;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots INSERT");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * INSERT - Disjoint
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                               [Root:LT to PT Index]
     *                                          (WBC copy LT to PT)
     *                                                  |
     *                                                  |
     *                                            +------+-------+
     *                                            |              |
     *                                         [Index]          ===
     *                                    (Calculate Index)
     *                                            |
     *                                            |
     *                                    +-------+--------+
     *                                    |                |
     *                             [Keys Required]     [Keys Optional]
     *                             (Convert API Keys)  (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                      ===
     *                     |
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                       [Root:Values]
     *                             (Convert API Values)
     *                                      |
     *                                      |
     *                         +------------+-------------+
     *                         |                          |
     *                  [Xfrm Values]                 [Fixed]
     *                         |                 (Set Fixed fields)
     *                         |                          |
     *                +--------+-------+       +----------+---------+
     *                |                |       |                    |
     *        [Validate Values]       ===  [Xfrm Keys]        [Keys Optional]
     *                |                    (Return API Keys)  (Return API keys)
     *                |
     *        +-------+-------+
     *        |               |
     *  [Default Values]     ===
     *  (Set table values to
     *   SW defaults from LRD)
     *
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - LT to PT Index: Root
     *
     * Propagate LT index into corresponding PTM working buffer locations
     * for PT index.  This is needed when index was not present in
     * input API keys and was allocated by LTM.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_lt_to_pt_index_retrieve(unit, sid,
                                                   &lt_to_pt_index_list));
    if (lt_to_pt_index_list != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_wbc,
                                           (void *)lt_to_pt_index_list,
                                           &lt_to_pt_index_node_data));
    }
    lt_to_pt_index_node = bcmltm_tree_node_create((void *)
                                                  lt_to_pt_index_node_data);
    SHR_NULL_CHECK(lt_to_pt_index_node, SHR_E_MEMORY);
    fa_node_roots[0] = lt_to_pt_index_node;

    /*
     * Tree_0 - Index: Left node of LT to PT Index
     *
     * Calculate the index used by LTM to track the entry.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fa_lt_index_retrieve(unit, sid, &index_info));
    if (index_info != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_lt_alloc_index,
                                           (void *)index_info,
                                           &index_node_data));
    }
    index_node = bcmltm_tree_left_node_create(lt_to_pt_index_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /*
     * Tree_0 - Keys Required: Left node of Index
     *
     * Convert API Required Key fields into physical fields in
     * Working Buffer (i.e. memory parameters).
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_keys_alloc_retrieve(unit, sid,
                                                           TRUE,
                                                           &keys_required_map));
    if (keys_required_map != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_api_key_fields_to_wb,
                                           (void *)keys_required_map,
                                           &keys_required_node_data));
    }
    keys_required_node = bcmltm_tree_left_node_create(index_node,
                                             (void *)keys_required_node_data);
    SHR_NULL_CHECK(keys_required_node, SHR_E_MEMORY);

    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_required_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys Required
     *
     * Process field transforms for Keys on IwA LTs.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_alloc_create(unit, sid,
                                                  BCMLT_OPCODE_INSERT,
                                                  next_chain, &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Keys Optional: Right node of Index
     *
     * Convert API Optional Key fields into physical fields in
     * Working Buffer (i.e. index).
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_keys_alloc_retrieve(unit, sid,
                                                           FALSE,
                                                           &keys_optional_map));
    if (keys_optional_map != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                 bcmltm_fa_node_api_alloc_key_fields_to_wb,
                                 (void *)keys_optional_map,
                                  &keys_optional_node_data));
    }
    keys_optional_node = bcmltm_tree_right_node_create(index_node,
                                             (void *)keys_optional_node_data);
    SHR_NULL_CHECK(keys_optional_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Convert API Value fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_VALUE,
                                                BCMLTM_FIELD_DIR_IN,
                                                &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_wb,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_ADD;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Validate Values: Left node of Xfrm Values
     *
     * Process field validations for Values.
     */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Default Values: Left node of Validate Values
     *
     * Set table values to SW defaults from LRD.
     *
     * Note that this node could be attached directly to Values
     * or to the Xfrm Values node depending on whether the table has
     * any value field transforms or validations.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_default_fields_to_wb,
                                       (void *)values_map,
                                       &def_values_node_data));
    def_values_node = bcmltm_tree_left_node_create(next_chain,
                                            (void *)def_values_node_data);
    SHR_NULL_CHECK(def_values_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Fixed Fields: Right node of Values
     *
     * Set Fixed fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fixed_fields_retrieve(unit, sid, &fixed_fields));
    if ((fixed_fields != NULL) && (fixed_fields->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_fields,
                                           &fixed_node_data));
    }
    fixed_node = bcmltm_tree_right_node_create(values_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Xfrm Keys: Left node of Fixed Fields
     *
     * Process reverse field transforms for Optional Keys on IwA LTs.
     */
    next_chain = fixed_node;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_alloc_create(unit, sid,
                                                  BCMLT_OPCODE_INSERT,
                                                  next_chain, &next_chain));

    /*
     * Tree_1 - Keys Optional: Right node of Fixed Fields
     *
     * Return allocated optional keys.
     */
    if (keys_optional_map != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_alloc_fields,
                                       (void *)keys_optional_map,
                                       &keys_optional_out_node_data));
    }
    keys_optional_out_node = bcmltm_tree_right_node_create(fixed_node,
                                     (void *)keys_optional_out_node_data);
    SHR_NULL_CHECK(keys_optional_out_node, SHR_E_MEMORY);


    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((lt_to_pt_index_node == NULL) &&
            (lt_to_pt_index_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lt_to_pt_index_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(index_node_data);
        }
        if ((keys_required_node == NULL) && (keys_required_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_required_node_data);
        }
        if ((keys_optional_node == NULL) && (keys_optional_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_optional_node_data);
        }
        if ((keys_optional_out_node == NULL) &&
            (keys_optional_out_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_optional_out_node_data);
        }
        if ((values_node == NULL) && (values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(values_node_data);
        }
        if ((def_values_node == NULL) && (def_values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(def_values_node_data);
        }
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create FA tree roots for INSERT on IwA Overlay LT.
 *
 * This function creates the FA tree roots for a INSERT operation
 * on given Index with Allocation Overlay logical table (shared index).
 *
 * Unspecified input fields values are reset to the SW
 * defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_alloc_index_shared_fa_node_roots_insert_create(int unit,
                                            bcmlrd_sid_t sid,
                                            uint32_t num_roots,
                                            bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *lt_to_pt_index_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *keys_required_node = NULL;
    bcmltm_node_t *keys_optional_node = NULL;
    bcmltm_node_t *keys_optional_out_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *def_values_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *lt_to_pt_index_node_data = NULL;
    bcmltm_fa_node_t *index_node_data = NULL;
    bcmltm_fa_node_t *keys_required_node_data = NULL;
    bcmltm_fa_node_t *keys_optional_node_data = NULL;
    bcmltm_fa_node_t *keys_optional_out_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *def_values_node_data = NULL;
    bcmltm_fa_node_t *fixed_node_data = NULL;
    const bcmltm_wb_copy_list_t *lt_to_pt_index_list = NULL;
    const bcmltm_fa_lt_index_t *index_info = NULL;
    const bcmltm_to_ptm_mapping_t *keys_required_map = NULL;
    const bcmltm_to_ptm_mapping_t *keys_optional_map = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_fields = NULL;
    int xfrm_type;
    bcmltm_md_xfrm_default_t xfrm_default;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots INSERT");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * INSERT - Disjoint
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                               [Root:LT to PT Index]
     *                                          (WBC copy LT to PT)
     *                                                  |
     *                                                  |
     *                                            +------+-------+
     *                                            |              |
     *                                         [Index]          ===
     *                               (Calculate Global Index)
     *                                            |
     *                                            |
     *                                    +-------+--------+
     *                                    |                |
     *                             [Keys Required]     [Keys Optional]
     *                             (Convert API Keys)  (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                      ===
     *                     |
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                       [Root:Values]
     *                             (Convert API Values)
     *                                      |
     *                                      |
     *                         +------------+-------------+
     *                         |                          |
     *                  [Xfrm Values]                 [Fixed]
     *                         |                 (Set Fixed fields)
     *                         |                          |
     *                +--------+-------+       +----------+---------+
     *                |                |       |                    |
     *        [Validate Values]       ===  [Xfrm Keys]        [Keys Optional]
     *                |                    (Return API Keys)  (Return API keys)
     *                |
     *        +-------+-------+
     *        |               |
     *  [Default Values]     ===
     *  (Set table values to
     *   SW defaults from LRD)
     *
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - LT to PT Index: Root
     *
     * Propagate LT index into corresponding PTM working buffer locations
     * for PT index.  This is needed when index was not present in
     * input API keys and was allocated by LTM.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_lt_to_pt_index_retrieve(unit, sid,
                                                   &lt_to_pt_index_list));
    if (lt_to_pt_index_list != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_wbc,
                                           (void *)lt_to_pt_index_list,
                                           &lt_to_pt_index_node_data));
    }
    lt_to_pt_index_node = bcmltm_tree_node_create((void *)
                                                  lt_to_pt_index_node_data);
    SHR_NULL_CHECK(lt_to_pt_index_node, SHR_E_MEMORY);
    fa_node_roots[0] = lt_to_pt_index_node;

    /*
     * Tree_0 - Index: Left node of LT to PT Index
     *
     * Calculate the index used by LTM to track the entry using the
     * global inuse bitmap.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fa_lt_index_retrieve(unit, sid, &index_info));
    if (index_info != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_lt_global_alloc_index,
                                           (void *)index_info,
                                           &index_node_data));
    }
    index_node = bcmltm_tree_left_node_create(lt_to_pt_index_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /*
     * Tree_0 - Keys Required: Left node of Index
     *
     * Convert API Required Key fields into physical fields in
     * Working Buffer (i.e. memory parameters).
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_keys_alloc_retrieve(unit, sid,
                                                           TRUE,
                                                           &keys_required_map));
    if (keys_required_map != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_api_key_fields_to_wb,
                                           (void *)keys_required_map,
                                           &keys_required_node_data));
    }
    keys_required_node = bcmltm_tree_left_node_create(index_node,
                                             (void *)keys_required_node_data);
    SHR_NULL_CHECK(keys_required_node, SHR_E_MEMORY);

    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_required_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys Required
     *
     * Process field transforms for Keys on IwA LTs.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_alloc_create(unit, sid,
                                                  BCMLT_OPCODE_INSERT,
                                                  next_chain, &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Keys Optional: Right node of Index
     *
     * Convert API Optional Key fields into physical fields in
     * Working Buffer (i.e. index).
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_keys_alloc_retrieve(unit, sid,
                                                           FALSE,
                                                           &keys_optional_map));
    if (keys_optional_map != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                 bcmltm_fa_node_api_alloc_key_fields_to_wb,
                                 (void *)keys_optional_map,
                                  &keys_optional_node_data));
    }
    keys_optional_node = bcmltm_tree_right_node_create(index_node,
                                             (void *)keys_optional_node_data);
    SHR_NULL_CHECK(keys_optional_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Convert API Value fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_VALUE,
                                                BCMLTM_FIELD_DIR_IN,
                                                &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_wb,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_ADD;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Validate Values: Left node of Xfrm Values
     *
     * Process field validations for Values.
     */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Default Values: Left node of Validate Values
     *
     * Set table values to SW defaults from LRD.
     *
     * Note that this node could be attached directly to Values
     * or to the Xfrm Values node depending on whether the table has
     * any value field transforms or validations.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_default_fields_to_wb,
                                       (void *)values_map,
                                       &def_values_node_data));
    def_values_node = bcmltm_tree_left_node_create(next_chain,
                                            (void *)def_values_node_data);
    SHR_NULL_CHECK(def_values_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Fixed Fields: Right node of Values
     *
     * Set Fixed fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fixed_fields_retrieve(unit, sid, &fixed_fields));
    if ((fixed_fields != NULL) && (fixed_fields->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_fields,
                                           &fixed_node_data));
    }
    fixed_node = bcmltm_tree_right_node_create(values_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Xfrm Keys: Left node of Fixed Fields
     *
     * Process reverse field transforms for Optional Keys on IwA LTs.
     */
    next_chain = fixed_node;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_alloc_create(unit, sid,
                                                  BCMLT_OPCODE_INSERT,
                                                  next_chain, &next_chain));

    /*
     * Tree_1 - Keys Optional: Right node of Fixed Fields
     *
     * Return allocated optional keys.
     */
    if (keys_optional_map != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_alloc_fields,
                                       (void *)keys_optional_map,
                                       &keys_optional_out_node_data));
    }
    keys_optional_out_node = bcmltm_tree_right_node_create(fixed_node,
                                     (void *)keys_optional_out_node_data);
    SHR_NULL_CHECK(keys_optional_out_node, SHR_E_MEMORY);


    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((lt_to_pt_index_node == NULL) &&
            (lt_to_pt_index_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lt_to_pt_index_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(index_node_data);
        }
        if ((keys_required_node == NULL) && (keys_required_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_required_node_data);
        }
        if ((keys_optional_node == NULL) && (keys_optional_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_optional_node_data);
        }
        if ((keys_optional_out_node == NULL) &&
            (keys_optional_out_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_optional_out_node_data);
        }
        if ((values_node == NULL) && (values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(values_node_data);
        }
        if ((def_values_node == NULL) && (def_values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(def_values_node_data);
        }
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create EE tree roots for INSERT on Index Non-Overlay LT.
 *
 * This function creates the EE tree roots for a INSERT operation
 * on given Index (SI and IwA) Non-Overlay logical table.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_insert_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_t *write_node_data = NULL;
    bcmltm_ee_node_t *index_node_data = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots INSERT");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (ee_node_ptm_func_get(unit, sid, &read_func, &write_func));

    /*
     * INSERT - Disjoint
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Read]
     *                 (PTM read operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:           [Root:Write]
     *                 (PTM write operation)
     *                          |
     *                          |
     *          +---------------+-------------+
     *          |                             |
     *    [Index Insert]                     ===
     *    (LT index insert
     *    resource operation)
     *
     * NOTE:
     * The LT index insert operation is placed in Tree_1 (rather than Tree_0)
     * in order to allow Interactive LTs to use this flow.
     * Commit/abort are not applicable to Interactive LTs.  As such,
     * processing of all value fields must be done prior to the index
     * allocation because there is no rollback mechanism to undo the
     * index allocation.
     */


    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Read: Root
     *
     * Execute PTM read operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Write: Root
     *
     * Execute PTM write operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_node_data));
    write_node = bcmltm_tree_node_create((void *)write_node_data);
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;

    /*
     * Tree_1 - Index Insert: Left node of Write
     *
     * LT index insert resource operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ee_lt_index_retrieve(unit, sid,
                                                &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_insert,
                                       (void *)index_info,
                                       &index_node_data));
    index_node = bcmltm_tree_left_node_create(write_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((read_node == NULL) && (read_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(read_node_data);
        }
        if ((write_node == NULL) && (write_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(write_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(index_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create EE tree roots for INSERT on Index Overlay LT.
 *
 * This function creates the EE tree roots for a INSERT operation
 * on given Index (SI and IwA) Overlay logical table (shared index).
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_shared_ee_node_roots_insert_create(int unit,
                                            bcmlrd_sid_t sid,
                                            uint32_t num_roots,
                                            bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *global_index_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_t *write_node_data = NULL;
    bcmltm_ee_node_t *index_node_data = NULL;
    bcmltm_ee_node_t *global_index_node_data = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots INSERT");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (ee_node_ptm_func_get(unit, sid, &read_func, &write_func));

    /*
     * INSERT - Disjoint
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:                    [Root:Read]
     *                          (PTM read operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Write]
     *                          (PTM write operation)
     *                                     |
     *                                     |
     *                          +----------+---------+
     *                          |                    |
     *               [Index Insert]                 ===
     *               (LT index insert
     *               resource operation)
     *                          |
     *                          |
     *          +--------------+-------------+
     *          |                            |
     *    [Global Index Insert]             ===
     *    (LT shared index insert
     *    resource operation)
     *
     * NOTE:
     * The LT index insert operations are placed in Tree_1 (rather than Tree_0)
     * in order to allow Interactive LTs to use this flow.
     * Commit/abort are not applicable to Interactive LTs.  As such,
     * processing of all value fields must be done prior to the index
     * allocation because there is no rollback mechanism to undo the
     * index allocation.
     */


    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Read: Root
     *
     * Execute PTM read operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Write: Root
     *
     * Execute PTM write operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_node_data));
    write_node = bcmltm_tree_node_create((void *)write_node_data);
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;

    /*
     * Tree_1 - Index Insert: Left node of Write
     *
     * LT index insert resource operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ee_lt_index_retrieve(unit, sid,
                                                &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_insert,
                                       (void *)index_info,
                                       &index_node_data));
    index_node = bcmltm_tree_left_node_create(write_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Global Index Insert: Left node of Index Insert
     *
     * LT global index insert operation for shared resources.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_global_index_insert,
                                       (void *)index_info,
                                       &global_index_node_data));
    global_index_node = bcmltm_tree_left_node_create(index_node,
                                             (void *)global_index_node_data);
    SHR_NULL_CHECK(global_index_node, SHR_E_MEMORY);

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((read_node == NULL) && (read_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(read_node_data);
        }
        if ((write_node == NULL) && (write_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(write_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(index_node_data);
        }
        if ((global_index_node == NULL) && (global_index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(global_index_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create INSERT operation metadata for Simple Index Non-Overlay LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Simple Index Non-Overlay LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_simple_index_op_insert_create(int unit, bcmlrd_sid_t sid,
                                 bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP INSERT metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_simple_index_fa_node_roots_insert_create(unit, sid,
                                                     op_md->num_roots,
                                                     &op_md->fa_node_roots));
    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_insert_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create INSERT operation metadata for Simple Index Overlay LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Simple Index Overlay LTs (shared index).
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_simple_index_shared_op_insert_create(int unit, bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP INSERT metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_simple_index_shared_fa_node_roots_insert_create(unit, sid,
                                                     op_md->num_roots,
                                                     &op_md->fa_node_roots));
    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_shared_ee_node_roots_insert_create(unit, sid,
                                                     op_md->num_roots,
                                                     &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create INSERT operation metadata for Simple Index LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Simple Index LTs, Overlay and Non-Overlay.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_simple_index_all_op_insert_create(int unit, bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_table_attr_t table_attr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_attr_get(unit, sid, &table_attr));

    /* Create trees for Overlay or non-Overlay case */
    if (table_attr.flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        SHR_IF_ERR_EXIT
            (lt_simple_index_shared_op_insert_create(unit, sid, op_md_ptr));

    } else {
        SHR_IF_ERR_EXIT
            (lt_simple_index_op_insert_create(unit, sid, op_md_ptr));
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create INSERT operation metadata for IwA Non-Overlay LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Index with Allocation Non-Overlay LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_alloc_index_op_insert_create(int unit, bcmlrd_sid_t sid,
                                bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP INSERT metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_alloc_index_fa_node_roots_insert_create(unit, sid,
                                                    op_md->num_roots,
                                                    &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_insert_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create INSERT operation metadata for IwA Overlay LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Index with Allocation Overlay LTs (shared index).
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_alloc_index_shared_op_insert_create(int unit, bcmlrd_sid_t sid,
                                       bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP INSERT metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_alloc_index_shared_fa_node_roots_insert_create(unit, sid,
                                                    op_md->num_roots,
                                                    &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_shared_ee_node_roots_insert_create(unit, sid,
                                                     op_md->num_roots,
                                                     &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create INSERT operation metadata for IwA LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Index with Allocation LTs, Overlay and Non-Overlay.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_alloc_index_all_op_insert_create(int unit, bcmlrd_sid_t sid,
                                    bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_table_attr_t table_attr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_attr_get(unit, sid, &table_attr));

    /* Create trees for Overlay or non-Overlay case */
    if (table_attr.flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        SHR_IF_ERR_EXIT
            (lt_alloc_index_shared_op_insert_create(unit, sid, op_md_ptr));

    } else {
        SHR_IF_ERR_EXIT
            (lt_alloc_index_op_insert_create(unit, sid, op_md_ptr));
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a LOOKUP operation.
 *
 * Create the FA tree roots for a LOOKUP operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *index_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    int xfrm_type;
    bcmltm_md_xfrm_default_t xfrm_default;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots LOOKUP");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                             [Root:Index]
     *                                    (Calculate Index)
     *                                            |
     *                                            |
     *                                    +-------+-------+
     *                                    |               |
     *                                 [Keys]            ===
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                      ===
     *                     |
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Values]
     *                         (Convert HW fields to return
     *                          API Values fields list)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]              ===
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Index: Root
     *
     * Calculate the index used by LTM to track the entry.
     */
    SHR_IF_ERR_EXIT
        (fa_lt_index_node_data_create(unit, sid,
                                      &index_node_data));
    index_node = bcmltm_tree_node_create((void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);
    fa_node_roots[0] = index_node;

    /*
     * Tree_0 - Keys: Left node of Index
     *
     * Convert API Key fields into physical fields in Working Buffer.
     * Keys are needed by the PTM read operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_KEY,
                                                BCMLTM_FIELD_DIR_IN,
                                                &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_left_node_create(index_node,
                                             (void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_NONE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_LOOKUP,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_LOOKUP,
                                            next_chain, &next_chain));

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Convert the HW information read from the EE stage into
     * the returning list of API Value fields.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_VALUE,
                                                BCMLTM_FIELD_DIR_OUT,
                                                &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_LOOKUP,
                                            next_chain, &next_chain));

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((keys_node == NULL) && (keys_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_node_data);
        }
        if ((values_node == NULL) && (values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(values_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a LOOKUP operation.
 *
 * Create the EE tree roots for a LOOKUP operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_t *index_node_data = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;
    bcmltm_ee_node_f read_func = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots LOOKUP");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (ee_node_ptm_func_get(unit, sid, &read_func, NULL));

    /*
     * LOOKUP
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Read]
     *                 (PTM read operation)
     *                       |
     *                       |
     *         +-------------+-------------+
     *         |                           |
     *    [Index Lookup]                  ===
     *    (LT index lookup
     *     resource operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:            NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Read: Root
     *
     * Execute PTM read operation to get HW data.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*
     * Tree_0 - Index Lookup: Left node of Read
     *
     * LT index lookup resource operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ee_lt_index_retrieve(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_exists,
                                       (void *)index_info,
                                       &index_node_data));
    index_node = bcmltm_tree_left_node_create(read_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * No actions.
     */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((read_node == NULL) && (read_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(read_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(index_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the LOOKUP opcode.
 *
 * Create the operation metadata for given table ID to use for the LOOKUP
 * opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_lookup_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP LOOKUP metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_lookup_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_lookup_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create FA tree roots for DELETE operation on Index Non-Overlay LT.
 *
 * This function creates the FA tree roots for a DELETE operation
 * on given Index (SI or IwA) Non-Overlay logical table.
 *
 * The field values are reset to the SW defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *def_values_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_fa_node_t *index_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *def_values_node_data = NULL;
    bcmltm_fa_node_t *fixed_node_data = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_fields = NULL;
    int xfrm_type;
    bcmltm_md_xfrm_default_t xfrm_default;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots DELETE");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * DELETE - Disjoint
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                             [Root:Index]
     *                                    (Calculate Index)
     *                                            |
     *                                            |
     *                                    +-------+-------+
     *                                    |               |
     *                                 [Keys]            ===
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                      ===
     *                     |
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                        [Root]
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Default Values]    [Delete Fixed]
     *                         |                 (Set delete value)
     *                         |
     *                +--------+--------+
     *                |                 |
     *       [Default Values]          ===
     *     (Set table values to
     *      SW defaults from LRD)
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Index: Root
     *
     * Calculate the index used by LTM to track the entry.
     */
    SHR_IF_ERR_EXIT
        (fa_lt_index_node_data_create(unit, sid,
                                      &index_node_data));
    index_node = bcmltm_tree_node_create((void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);
    fa_node_roots[0] = index_node;

    /*
     * Tree_0 - Keys: Left node of Index
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_KEY,
                                                BCMLTM_FIELD_DIR_IN,
                                                &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_left_node_create(index_node,
                                             (void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_NONE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_DELETE,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_DELETE,
                                            next_chain, &next_chain));

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Nothing to do.  Used for chaining subtrees.
     */
    values_node = bcmltm_tree_node_create((void *)NULL);
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Default Values: Left node of Values
     *
     * Process field transforms for Default Values.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_ALL;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_DELETE,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Default Values: Left node of Xfrm Default Values
     *
     * Set table values to SW defaults from LRD.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_VALUE,
                                                BCMLTM_FIELD_DIR_IN,
                                                &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_default_fields_to_wb,
                                       (void *)values_map,
                                       &def_values_node_data));
    def_values_node = bcmltm_tree_left_node_create(next_chain,
                                            (void *)def_values_node_data);
    SHR_NULL_CHECK(def_values_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Delete Fixed: Right node of Root
     *
     * Set delete values for fixed fields.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fixed_fields_retrieve(unit, sid, &fixed_fields));
    if ((fixed_fields != NULL) && (fixed_fields->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                    bcmltm_fa_node_delete_fixed_fields_to_wb,
                                           (void *)fixed_fields,
                                           &fixed_node_data));
    }
    fixed_node = bcmltm_tree_right_node_create(values_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((keys_node == NULL) && (keys_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_node_data);
        }
        if ((def_values_node == NULL) && (def_values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(def_values_node_data);
        }
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create FA tree roots for DELETE on Index Overlay LT.
 *
 * This function creates the FA tree roots for a DELETE operation
 * on given Index (SI and IwA) Overlay logical table (shared index).
 *
 * The field values are reset to the SW defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_shared_fa_node_roots_delete_create(int unit,
                                            bcmlrd_sid_t sid,
                                            uint32_t num_roots,
                                            bcmltm_node_t ***fa_node_roots_ptr)
{
    /*
     * The Overlay and Non-Overlay DELETE FA trees
     * for Simple Index are the same.
     */
    return lt_index_fa_node_roots_delete_create(unit, sid,
                                                num_roots,
                                                fa_node_roots_ptr);
}


/*!
 * \brief Create EE tree roots for DELETE on Index Non-Overlay LT.
 *
 * This function creates the EE tree roots for a DELETE operation
 * on given Index Non-Overlay logical table.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_t *write_node_data = NULL;
    bcmltm_ee_node_t *index_node_data = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots DELETE");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (ee_node_ptm_func_get(unit, sid, &read_func, &write_func));

    /*
     * DELETE - Disjoint
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Read]
     *                 (PTM read operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:           [Root:Write]
     *                 (PTM write operation)
     *                          |
     *                          |
     *          +---------------+-------------+
     *          |                             |
     *    [Index Delete]                     ===
     *    (LT index delete
     *    resource operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Read: Root
     *
     * Execute PTM read operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Write: Root
     *
     * Execute PTM write operation to set default values.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_node_data));
    write_node = bcmltm_tree_node_create((void *)write_node_data);
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;

    /*
     * Tree_1 - Index Delete: Left node of Write
     *
     * LT index delete resource operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ee_lt_index_retrieve(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_delete,
                                       (void *)index_info,
                                       &index_node_data));
    index_node = bcmltm_tree_left_node_create(write_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((read_node == NULL) && (read_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(read_node_data);
        }
        if ((write_node == NULL) && (write_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(write_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(index_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create EE tree roots for DELETE on Index Overlay LT.
 *
 * This function creates the EE tree roots for a DELETE operation
 * on given Index Overlay logical table (shared index).
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_shared_ee_node_roots_delete_create(int unit,
                                            bcmlrd_sid_t sid,
                                            uint32_t num_roots,
                                            bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *global_index_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_t *write_node_data = NULL;
    bcmltm_ee_node_t *index_node_data = NULL;
    bcmltm_ee_node_t *global_index_node_data = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots DELETE");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (ee_node_ptm_func_get(unit, sid, &read_func, &write_func));

    /*
     * DELETE - Disjoint
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Read]
     *                 (PTM read operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:           [Root:Write]
     *                 (PTM write operation)
     *                          |
     *                          |
     *          +---------------+-------------+
     *          |                             |
     *    [Index Delete]               [Global Index Delete]
     *    (LT index delete             (LT shared index delete
     *    resource operation)           resource operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Read: Root
     *
     * Execute PTM read operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Write: Root
     *
     * Execute PTM write operation to set default values.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_node_data));
    write_node = bcmltm_tree_node_create((void *)write_node_data);
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;

    /*
     * Tree_1 - Index Delete: Left node of Write
     *
     * LT index delete resource operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ee_lt_index_retrieve(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_delete,
                                       (void *)index_info,
                                       &index_node_data));
    index_node = bcmltm_tree_left_node_create(write_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Global Index Delete: Right node of Write
     *
     * LT global index delete operation for shared resources.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_global_index_delete,
                                       (void *)index_info,
                                       &global_index_node_data));
    global_index_node = bcmltm_tree_right_node_create(write_node,
                                             (void *)global_index_node_data);
    SHR_NULL_CHECK(global_index_node, SHR_E_MEMORY);

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((read_node == NULL) && (read_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(read_node_data);
        }
        if ((write_node == NULL) && (write_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(write_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(index_node_data);
        }
        if ((global_index_node == NULL) && (global_index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(global_index_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create DELETE operation metadata for Index Non-Overlay LTs.
 *
 * Create the operation metadata for given table ID to use for the DELETE
 * opcode on Index Non-Overlay LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_delete_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP DELETE metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_DELETE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_delete_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_delete_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create DELETE operation metadata for Index Overlay LTs.
 *
 * Create the operation metadata for given table ID to use for the DELETE
 * opcode on Index Overlay LTs (shared index).
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_shared_op_delete_create(int unit, bcmlrd_sid_t sid,
                                 bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP DELETE metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_DELETE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_shared_fa_node_roots_delete_create(unit, sid,
                                                     op_md->num_roots,
                                                     &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_shared_ee_node_roots_delete_create(unit, sid,
                                                     op_md->num_roots,
                                                     &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create DELETE operation metadata for Index LT.
 *
 * Create the operation metadata for given table ID to use for the DELETE
 * opcode on Index LTs, Overlay and Non-Overlay.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_all_op_delete_create(int unit, bcmlrd_sid_t sid,
                              bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_table_attr_t table_attr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_attr_get(unit, sid, &table_attr));

    /* Create trees for Overlay or non-Overlay case */
    if (table_attr.flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        SHR_IF_ERR_EXIT
            (lt_index_shared_op_delete_create(unit, sid, op_md_ptr));

    } else {
        SHR_IF_ERR_EXIT
            (lt_index_op_delete_create(unit, sid, op_md_ptr));
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a UPDATE operation.
 *
 * Create the FA tree roots for a UPDATE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *index_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    int xfrm_type;
    bcmltm_md_xfrm_default_t xfrm_default;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots UPDATE");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * UPDATE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                             [Root:Index]
     *                                    (Calculate Index)
     *                                            |
     *                                            |
     *                                    +-------+-------+
     *                                    |               |
     *                                 [Keys]            ===
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                      ===
     *                     |
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Values]
     *                           (Update API Values)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]              ===
     *                         |
     *                         |
     *                +--------+--------+
     *                |                 |
     *        [Validate Values]        ===
     *                |
     *                |
     *        +-------+-------+
     *        |               |
     * [Rev Xfrm Values]     ===
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Index: Root
     *
     * Calculate the index used by LTM to track the entry.
     */
    SHR_IF_ERR_EXIT
        (fa_lt_index_node_data_create(unit, sid,
                                      &index_node_data));
    index_node = bcmltm_tree_node_create((void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);
    fa_node_roots[0] = index_node;

    /*
     * Tree_0 - Keys: Left node of Index
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_KEY,
                                                BCMLTM_FIELD_DIR_IN,
                                                &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_left_node_create(index_node,
                                             (void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_NONE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain, &next_chain));


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Update API Value fields into physical fields in Working Buffer.
     * Unset marked API Value fields (revert to default) for UPDATE.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_VALUE,
                                                BCMLTM_FIELD_DIR_IN,
                                                &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_fields_unset_to_wb,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data);
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_UPDATE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Validate Values: Left node of Xfrm Values
     *
     * Process field validations for Values.
     */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Rev Xfrm Values: Left node of Validate Values
     *
     * Process reverse field transforms for Values read from PTM.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain, &next_chain));

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((keys_node == NULL) && (keys_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_node_data);
        }
        if ((values_node == NULL) && (values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(values_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a UPDATE operation.
 *
 * Create the EE tree roots for a UPDATE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_node = NULL;
    bcmltm_ee_node_t *index_node_data = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_t *write_node_data = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots UPDATE");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (ee_node_ptm_func_get(unit, sid, &read_func, &write_func));

    /*
     * UPDATE
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Read]
     *                 (PTM read operation)
     *                          |
     *                          |
     *            +-------------+-------------+
     *            |                           |
     *    [Index Lookup]                     ===
     *    (LT index lookup
     *     resource operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:           [Root:Write]
     *                 (PTM write operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Read: Root
     *
     * Execute PTM read operation to get HW data.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*
     * Tree_0 - Index Insert: Left node of Read
     *
     * LT index lookup resource operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ee_lt_index_retrieve(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_exists,
                                       (void *)index_info,
                                       &index_node_data));
    index_node = bcmltm_tree_left_node_create(read_node,
                                              (void *)index_node_data);
    SHR_NULL_CHECK(index_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Write: Root
     *
     * Execute PTM write operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_node_data));
    write_node = bcmltm_tree_node_create((void *)write_node_data);
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;


    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((read_node == NULL) && (read_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(read_node_data);
        }
        if ((index_node == NULL) && (index_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(index_node_data);
        }
        if ((write_node == NULL) && (write_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(write_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the UPDATE opcode.
 *
 * Create the operation metadata for given table ID to use for the UPDATE
 * opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_update_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP UPDATE metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_UPDATE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_update_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_update_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a TRAVERSE operation.
 *
 * This function creates the FA tree roots for a TRAVERSE operation
 * for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       uint32_t num_roots,
                                       bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *lt_to_pt_index_node = NULL;
    bcmltm_node_t *traverse_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_node_t *prev_keys_node = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_fa_node_t *lt_to_pt_index_node_data = NULL;
    bcmltm_fa_node_t *traverse_node_data = NULL;
    bcmltm_fa_node_t *prev_keys_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    const bcmltm_wb_copy_list_t *lt_to_pt_index_list = NULL;
    const bcmltm_fa_lt_index_t *lt_index_info = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map_in = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map_out = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    int xfrm_type;
    bcmltm_md_xfrm_default_t xfrm_default;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots TRAVERSE");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                        [Root:LT to PT Index]
     *                                   (WBC copy LT to PT)
     *                                            |
     *                                            |
     *                                +-----------+----------+
     *                                |                      |
     *                                |                     ===
     *                         [Traverse Index]
     *                            (LTM Index)
     *                                |
     *                                |
     *                 +--------------+-------------+
     *                 |                            |
     *            [Xfrm Keys]                    [Prev Keys]
     *                 |                     (Convert API Keys)
     *                 |
     *         +-------+-------+
     *         |               |
     *  [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Keys]
     *                         (Convert HW fields to return
     *                          API Keys fields list)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]            [Values]
     *                         |          (Convert HW fields to return
     *                         |            API Values fields list)
     *                 +-------+-------+
     *                 |               |
     *            [Xfrm Keys]         ===
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - LT to PT Index: Root
     *
     * Copy the LT index and memory parameters into the PTM working
     * buffer locations corresponding to the different PT index
     * and memory parameters.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_lt_to_pt_index_retrieve(unit, sid,
                                                   &lt_to_pt_index_list));
    
    if (lt_to_pt_index_list != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_wbc,
                                           (void *)lt_to_pt_index_list,
                                           &lt_to_pt_index_node_data));
    }
    lt_to_pt_index_node = bcmltm_tree_node_create((void *)
                                                  lt_to_pt_index_node_data);
    SHR_NULL_CHECK(lt_to_pt_index_node, SHR_E_MEMORY);
    fa_node_roots[0] = lt_to_pt_index_node;

    /*
     * Tree_0 - Traverse Index: Left node of LT to PT Index
     *
     * LTM Index traverse operation.  On first traverse,
     * the first entry index is obtained.  Subsequent traverse
     * will obtained the next entry index.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_fa_lt_index_retrieve(unit, sid,
                                                &lt_index_info));
    
    if (lt_index_info != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_traverse_index,
                                           (void *)lt_index_info,
                                           &traverse_node_data));
    }
    traverse_node = bcmltm_tree_left_node_create(lt_to_pt_index_node,
                                                 (void *)traverse_node_data);
    SHR_NULL_CHECK(traverse_node, SHR_E_MEMORY);

    /* Set node for next chain of Key transforms and validates */
    next_chain = traverse_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Traverse Index
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM;
    xfrm_default = BCMLTM_MD_XFRM_DEFAULT_NONE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_default,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain, &next_chain));

    /*
     * Tree_0 - Prev Keys: Right node of Traverse Index
     *
     * Convert API Key fields into corresponding Working Buffer locations.
     * Keys are needed by the traverse index operation to get the next entry.
     *
     * Note that on first traverse (start), no input keys are expected.
     * In this case, the traverse operation returns the first entry.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_KEY,
                                                BCMLTM_FIELD_DIR_IN,
                                                &keys_map_in));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map_in,
                                       &prev_keys_node_data));
    prev_keys_node = bcmltm_tree_right_node_create(traverse_node,
                                                   (void *)prev_keys_node_data);
    SHR_NULL_CHECK(prev_keys_node, SHR_E_MEMORY);


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Keys: Root
     *
     * Convert the HW information read from the EE stage into
     * the returning list of API Key fields.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_KEY,
                                                BCMLTM_FIELD_DIR_OUT,
                                                &keys_map_out));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)keys_map_out,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[1] = keys_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Keys
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Xfrm Keys: Left node of Xfrm Values
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain, &next_chain));

    /*
     * Tree_1 - Values: Right node of Keys
     *
     * Convert the HW information read from the EE stage into
     * the returning list of API Value fields.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_ptm_mapping_retrieve(unit, sid,
                                                BCMLTM_FIELD_TYPE_VALUE,
                                                BCMLTM_FIELD_DIR_OUT,
                                                &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_right_node_create(keys_node,
                                                (void *)values_node_data);
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((lt_to_pt_index_node == NULL) &&
            (lt_to_pt_index_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lt_to_pt_index_node_data);
        }
        if ((traverse_node == NULL) && (traverse_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(traverse_node_data);
        }
        if ((prev_keys_node == NULL) && (prev_keys_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(prev_keys_node_data);
        }
        if ((keys_node == NULL) && (keys_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(keys_node_data);
        }
        if ((values_node == NULL) && (values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(values_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a TRAVERSE operation.
 *
 * This function creates the EE tree roots for a TRAVERSE operation
 * for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       uint32_t num_roots,
                                       bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    bcmltm_ee_node_f read_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots TRAVERSE");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (ee_node_ptm_func_get(unit, sid, &read_func, NULL));

    /*
     * TRAVERSE
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Read]
     *                (PTM read operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Read: Root
     *
     * Execute PTM read operation to get HW data.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_list_retrieve(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data);
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * No actions.
     */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((read_node == NULL) && (read_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(read_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create operation metadata for the TRAVERSE opcode.
 *
 * Create the operation metadata for given table ID to use for the TRAVERSE
 * opcode.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_traverse_create(int unit, bcmlrd_sid_t sid,
                            bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP TRAVERSE metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_TRAVERSE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_traverse_create(unit, sid,
                                                op_md->num_roots,
                                                &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_traverse_create(unit, sid,
                                                op_md->num_roots,
                                                &op_md->ee_node_roots));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*
 * LT Metadata Function Vector for Simple Index LT with PT groups mapping.
 */
static bcmltm_md_lt_drv_t lt_simple_index_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lt_simple_index_all_op_insert_create,
    /* op_lookup_create */      lt_index_op_lookup_create,
    /* op_delete_create */      lt_index_all_op_delete_create,
    /* op_update_create */      lt_index_op_update_create,
    /* op_traverse_create */    lt_index_op_traverse_create,
};

/*
 * LT Metadata Function Vector for Index with Allocation LT
 * with PT groups mapping.
 */
static bcmltm_md_lt_drv_t lt_alloc_index_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lt_alloc_index_all_op_insert_create,
    /* op_lookup_create */      lt_index_op_lookup_create,
    /* op_delete_create */      lt_index_all_op_delete_create,
    /* op_update_create */      lt_index_op_update_create,
    /* op_traverse_create */    lt_index_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_simple_index_drv_get(void)
{
    return &lt_simple_index_drv;
}

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_alloc_index_drv_get(void)
{
    return &lt_alloc_index_drv;
}
