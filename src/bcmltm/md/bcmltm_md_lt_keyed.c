/*! \file bcmltm_md_lt_keyed.c
 *
 * Logical Table Manager - Keyed Logical Table (Hash and TCAM).
 *
 * This file contains implementation to support metadata construction
 * for Keyed, Hash and TCAM Logical Tables.
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
#define LT_OP_DELETE_NUM_ROOTS   1

/* Number of roots for UPDATE operation */
#define LT_OP_UPDATE_NUM_ROOTS   2

/* Number of roots for TRAVERSE operation */
#define LT_OP_TRAVERSE_NUM_ROOTS 2


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the FA tree roots for a INSERT operation.
 *
 * Create the FA tree roots for a INSERT operation for given table ID.
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
lt_keyed_fa_node_roots_insert_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *def_values_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *fixed_node_data = NULL;
    bcmltm_fa_node_t *def_values_node_data = NULL;
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
     * INSERT
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                     [Fixed]
     *                     |                     (Set Fixed fields)
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
     *  [Default Values]      ===
     *  (Set table values to
     *   SW defaults from LRD)
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Keys: Root
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
    keys_node = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

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

    /*
     * Tree_0 - Fixed Fields: Right node of Keys
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
    fixed_node = bcmltm_tree_right_node_create(keys_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);


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
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
        }
        if ((values_node == NULL) && (values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(values_node_data);
        }
        if ((def_values_node == NULL) && (def_values_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(def_values_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a INSERT operation.
 *
 * Create the EE tree roots for a INSERT operation for given table ID.
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
lt_keyed_ee_node_roots_insert_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_node_t *insert_node = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    bcmltm_ee_node_t *insert_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;
    const bcmltm_pt_keyed_op_info_t *insert_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots INSERT");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * INSERT
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Lookup]
     *                (PTM lookup operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          [Root:Insert]
     *                (PTM insert operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Lookup: Root
     *
     * PTM lookup operation (entry should not exist).
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_keyed_retrieve(unit, sid, BCMPTM_OP_LOOKUP,
                                             &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create
            (unit,
             bcmltm_ee_node_model_keyed_lookup_not_found,
             (void *)lookup_info, &lookup_node_data));
    lookup_node = bcmltm_tree_node_create((void *)lookup_node_data);
    SHR_NULL_CHECK(lookup_node, SHR_E_MEMORY);
    ee_node_roots[0] = lookup_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Insert: Root
     *
     * Execute PTM insert operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_keyed_retrieve(unit, sid, BCMPTM_OP_INSERT,
                                             &insert_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_insert,
                                       (void *)insert_info,
                                       &insert_node_data));
    insert_node = bcmltm_tree_node_create((void *)insert_node_data);
    SHR_NULL_CHECK(insert_node, SHR_E_MEMORY);
    ee_node_roots[1] = insert_node;

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((lookup_node == NULL) && (lookup_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(lookup_node_data);
        }
        if ((insert_node == NULL) && (insert_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(insert_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the INSERT opcode.
 *
 * Create the operation metadata for given table ID to use for the INSERT
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
lt_keyed_op_insert_create(int unit, bcmlrd_sid_t sid,
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
        (lt_keyed_fa_node_roots_insert_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_insert_create(unit, sid,
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
lt_keyed_fa_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
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
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots LOOKUP");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                     [Fixed]
     *                     |                     (Set Fixed fields)
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
     * Tree_0 - Keys: Root
     *
     * Convert API Key fields into physical fields in Working Buffer.
     * Keys are needed by the PTM lookup operation.
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
    keys_node = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

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

    /*
     * Tree_0 - Fixed Fields: Right node of Keys
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
    fixed_node = bcmltm_tree_right_node_create(keys_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);


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
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
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
lt_keyed_ee_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots LOOKUP");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Lookup]
     *                (PTM lookup operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Lookup: Root
     *
     * Execute PTM lookup operation to get HW data.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_keyed_retrieve(unit, sid, BCMPTM_OP_LOOKUP,
                                             &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_lookup,
                                       (void *)lookup_info,
                                       &lookup_node_data));
    lookup_node = bcmltm_tree_node_create((void *)lookup_node_data);
    SHR_NULL_CHECK(lookup_node, SHR_E_MEMORY);
    ee_node_roots[0] = lookup_node;

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
        if ((lookup_node == NULL) && (lookup_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(lookup_node_data);
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
lt_keyed_op_lookup_create(int unit, bcmlrd_sid_t sid,
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
        (lt_keyed_fa_node_roots_lookup_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_lookup_create(unit, sid,
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
 * \brief Create the FA tree roots for a DELETE operation.
 *
 * Create the FA tree roots for a DELETE operation for given table ID.
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
lt_keyed_fa_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *fixed_node_data = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map = NULL;
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
     * DELETE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                     [Fixed]
     *                     |                     (Set Fixed fields)
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
 */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Keys: Root
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
    keys_node = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

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

    /*
     * Tree_0 - Fixed Fields: Right node of Keys
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
    fixed_node = bcmltm_tree_right_node_create(keys_node,
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
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a DELETE operation.
 *
 * Create the EE tree roots for a DELETE operation for given table ID.
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
lt_keyed_ee_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *delete_node = NULL;
    bcmltm_ee_node_t *delete_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *delete_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots DELETE");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * DELETE
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Delete]
     *                 (PTM delete operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Delete: Root
     *
     * Execute PTM delete operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_keyed_retrieve(unit, sid, BCMPTM_OP_DELETE,
                                             &delete_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_delete,
                                       (void *)delete_info,
                                       &delete_node_data));
    delete_node = bcmltm_tree_node_create((void *)delete_node_data);
    SHR_NULL_CHECK(delete_node, SHR_E_MEMORY);
    ee_node_roots[0] = delete_node;

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((delete_node == NULL) && (delete_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(delete_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the DELETE opcode.
 *
 * Create the operation metadata for given table ID to use for the DELETE
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
lt_keyed_op_delete_create(int unit, bcmlrd_sid_t sid,
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
        (lt_keyed_fa_node_roots_delete_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_delete_create(unit, sid,
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
lt_keyed_fa_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
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
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots UPDATE");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * UPDATE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                     [Fixed]
     *                     |                     (Set Fixed fields)
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
     * Tree_0 - Keys: Root
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
    keys_node = bcmltm_tree_node_create((void *)keys_node_data);
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;


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

    /*
     * Tree_0 - Fixed Fields: Right node of Keys
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
    fixed_node = bcmltm_tree_right_node_create(keys_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);


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
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
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
lt_keyed_ee_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t num_roots,
                                     bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_node_t *insert_node = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    bcmltm_ee_node_t *insert_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;
    const bcmltm_pt_keyed_op_info_t *insert_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots UPDATE");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * UPDATE
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Lookup]
     *                (PTM lookup operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:           [Root:Insert]
     *                (PTM insert operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Lookup: Root
     *
     * Execute PTM lookup operation to get HW data.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_keyed_retrieve(unit, sid, BCMPTM_OP_LOOKUP,
                                             &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_lookup,
                                       (void *)lookup_info,
                                       &lookup_node_data));
    lookup_node = bcmltm_tree_node_create((void *)lookup_node_data);
    SHR_NULL_CHECK(lookup_node, SHR_E_MEMORY);
    ee_node_roots[0] = lookup_node;


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Insert: Root
     *
     * Execute PTM insert operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_keyed_retrieve(unit, sid, BCMPTM_OP_INSERT,
                                             &insert_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_insert,
                                       (void *)insert_info,
                                       &insert_node_data));
    insert_node = bcmltm_tree_node_create((void *)insert_node_data);
    SHR_NULL_CHECK(insert_node, SHR_E_MEMORY);
    ee_node_roots[1] = insert_node;

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((lookup_node == NULL) && (lookup_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(lookup_node_data);
        }
        if ((insert_node == NULL) && (insert_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(insert_node_data);
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
lt_keyed_op_update_create(int unit, bcmlrd_sid_t sid,
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
        (lt_keyed_fa_node_roots_update_create(unit, sid,
                                              op_md->num_roots,
                                              &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_update_create(unit, sid,
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
 * Create the FA tree roots for a TRAVERSE operation for given table ID.
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
lt_keyed_fa_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       uint32_t num_roots,
                                       bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *prev_keys_node = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *prev_keys_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *fixed_node_data = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map_in = NULL;
    const bcmltm_to_ptm_mapping_t *keys_map_out = NULL;
    const bcmltm_to_ptm_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_fields = NULL;
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
     *   Tree_0:                     [Root:Prev Keys]
     *                             (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                     [Fixed]
     *                     |                     (Set Fixed fields)
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
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
     * Tree_0 - Prev Keys: Root
     *
     * Convert API Key fields into physical fields in Working Buffer.
     * Keys are needed by the PTM operation to get the next entry.
     *
     * Note that on first traverse (start), there are no input keys.
     * In this case, the operation returns the first entry.
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
    prev_keys_node = bcmltm_tree_node_create((void *)prev_keys_node_data);
    SHR_NULL_CHECK(prev_keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = prev_keys_node;

    /* Set node for next chain of Key transforms and validates */
    next_chain = prev_keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Prev Keys
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
     * Tree_0 - Fixed Fields: Right node of Prev Keys
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
    fixed_node = bcmltm_tree_right_node_create(prev_keys_node,
                                               (void *)fixed_node_data);
    SHR_NULL_CHECK(fixed_node, SHR_E_MEMORY);


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
        if ((prev_keys_node == NULL) && (prev_keys_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(prev_keys_node_data);
        }
        if ((fixed_node == NULL) && (fixed_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(fixed_node_data);
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
 * Create the EE tree roots for a TRAVERSE operation for given table ID.
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
lt_keyed_ee_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       uint32_t num_roots,
                                       bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *traverse_node = NULL;
    bcmltm_ee_node_t *traverse_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *traverse_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots TRAVERSE");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Traverse]
     *                (PTM traverse operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Traverse: Root
     *
     * Execute PTM traverse operation to get HW data.
     * Note that Get-First and Get-Next use same node cookie.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_pt_keyed_retrieve(unit, sid, BCMPTM_OP_GET_FIRST,
                                             &traverse_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_traverse,
                                       (void *)traverse_info,
                                       &traverse_node_data));
    traverse_node = bcmltm_tree_node_create((void *)traverse_node_data);
    SHR_NULL_CHECK(traverse_node, SHR_E_MEMORY);
    ee_node_roots[0] = traverse_node;

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
        if ((traverse_node == NULL) && (traverse_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(traverse_node_data);
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
lt_keyed_op_traverse_create(int unit, bcmlrd_sid_t sid,
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
        (lt_keyed_fa_node_roots_traverse_create(unit, sid,
                                                op_md->num_roots,
                                                &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_traverse_create(unit, sid,
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
 * LT Metadata Function Vector
 */
static bcmltm_md_lt_drv_t lt_keyed_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lt_keyed_op_insert_create,
    /* op_lookup_create */      lt_keyed_op_lookup_create,
    /* op_delete_create */      lt_keyed_op_delete_create,
    /* op_update_create */      lt_keyed_op_update_create,
    /* op_traverse_create */    lt_keyed_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_keyed_drv_get(void)
{
    return &lt_keyed_drv;
}
