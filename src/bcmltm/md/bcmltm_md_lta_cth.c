/*! \file bcmltm_md_lta_cth.c
 *
 * Logical Table Manager - LTA Custom Table Handler.
 *
 * This file contains implementation to support metadata construction
 * for logical tables with Custom Table Handler.
 *
 * Custom Table Handlers are only supported on Simple Index logical tables.
 * LTM uses the LTA Logical Table Adaptor interface to perform the
 * corresponding operation on the table.
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
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_op.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Number of roots for INSERT/DELETE/UPDATE operation
 * Note: The INSERT is applicable on Simple Index LTs.
 */
#define LT_OP_INS_DEL_UPD_NUM_ROOTS   1

/* Number of roots for INSERT operation on Index with Allocation LTs */
#define LT_OP_ALLOC_INDEX_INSERT_NUM_ROOTS   2

/* Number of roots for LOOKUP operation */
#define LT_OP_LOOKUP_NUM_ROOTS   2

/* Number of roots for TRAVERSE operation */
#define LT_OP_TRAVERSE_NUM_ROOTS    2


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the FA tree roots for INSERT/DELETE/UPDATE operation.
 *
 * Create the FA tree roots for any of the following operations:
 *     INSERT (this is for Simple Index LT, i.e. non IwA)
 *     DELETE
 *     UPDATE
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode INSERT, DELETE or UPDATE.
 * \param [in] num_roots Number of tree roots.
 * \param [out] fa_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lta_cth_fa_node_roots_create(int unit,
                             bcmlrd_sid_t sid,
                             bcmlt_opcode_t opcode,
                             uint32_t num_roots,
                             bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    /* #1 */
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /* Check for valid opcode */
    if ((opcode != BCMLT_OPCODE_INSERT) &&
        (opcode != BCMLT_OPCODE_DELETE) &&
        (opcode != BCMLT_OPCODE_UPDATE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /*
     * INSERT/DELETE/UPDATE
     *
     * FA Tree Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert to
     *                     LTA Input fields)
     *                       |
     *                       |
     *              +--------+-------+
     *              |                |
     *         [Left_Node]
     *    (Init LTA Input fields)
     *             #1
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    if ((opcode == BCMLT_OPCODE_INSERT) ||
        (opcode == BCMLT_OPCODE_UPDATE)) {
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lta_cth_in_fields_retrieve(unit, sid, &input_fields));
    } else if (opcode == BCMLT_OPCODE_DELETE) {
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lta_cth_in_key_fields_retrieve(unit, sid, &input_fields));
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_lta,
                                       (void *)input_fields,
                                       &to_lta_node_data));
    fa_node_roots[0] = bcmltm_tree_node_create((void *)to_lta_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)input_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(fa_node_roots[0],
                                                (void *)init_in_node_data);
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((fa_node_roots[0] == NULL) && (to_lta_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(to_lta_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a INSERT/DELETE/UPDATE operation.
 *
 * Create the EE tree roots for any of the following operations:
 *     INSERT (this is for Simple Index LT, i.e. non IwA)
 *     DELETE
 *     UPDATE
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode INSERT, DELETE or UPDATE.
 * \param [in] num_roots Number of tree roots.
 * \param [out] ee_node_roots_ptr Returning pointer to list of tree roots.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lta_cth_ee_node_roots_create(int unit,
                             bcmlrd_sid_t sid,
                             bcmlt_opcode_t opcode,
                             uint32_t num_roots,
                             bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *op_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;
    bcmltm_ee_node_f node_function = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /* Check for valid opcode */
    if ((opcode != BCMLT_OPCODE_INSERT) &&
        (opcode != BCMLT_OPCODE_DELETE) &&
        (opcode != BCMLT_OPCODE_UPDATE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    /*
     * INSERT/DELETE/UPDATE
     *
     * EE Trees Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH operation)
 */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler operation.
     */
    if (opcode == BCMLT_OPCODE_INSERT) {
        node_function = bcmltm_ee_node_lta_insert;
    } else if (opcode == BCMLT_OPCODE_DELETE) {
        node_function = bcmltm_ee_node_lta_delete;
    } else if (opcode == BCMLT_OPCODE_UPDATE) {
        node_function = bcmltm_ee_node_lta_update;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_table_params_retrieve(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       node_function,
                                       (void *)table_params,
                                       &op_node_data));
    ee_node_roots[0] = bcmltm_tree_node_create((void *)op_node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

    /* Set returning pointer */
    *ee_node_roots_ptr = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((ee_node_roots[0] == NULL) && (op_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(op_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create the FA tree roots for INSERT on IwA LT.
 *
 * Create the FA tree roots for an INSERT operation for given
 * Index with Allocation logical table.
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
lta_cth_alloc_index_fa_node_roots_insert_create(int unit,
                                   bcmlrd_sid_t sid,
                                   uint32_t num_roots,
                                   bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *left_node_0 = NULL;
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    bcmltm_fa_node_t *init_output_node_data = NULL;
    bcmltm_fa_node_t *to_api_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    const bcmltm_lta_field_list_t *output_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(fa_node_roots, size, "LTM FA node roots INSERT");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * INSERT - IwA
     *
     * FA Tree Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert to
     *                     LTA Input fields)
     *                       |
     *                       |
     *         +-------------+-------------+
     *         |                           |
     *    [Left_Node]                 [Right_Node]
     *    (Init LTA Output fields)    (Init LTA Input fields)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (Copy LTA Output fields
     *                     to API fields list)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_in_fields_retrieve(unit, sid, &input_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_lta,
                                       (void *)input_fields,
                                       &to_lta_node_data));
    fa_node_roots[0] = bcmltm_tree_node_create((void *)to_lta_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    /*
     * Tree_0 - Left_Node
     * Initialize the LTA Output fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_out_fields_retrieve(unit, sid, &output_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)output_fields,
                                       &init_output_node_data));
    left_node_0 = bcmltm_tree_left_node_create(fa_node_roots[0],
                                               (void *)init_output_node_data);
    SHR_NULL_CHECK(left_node_0, SHR_E_MEMORY);

    /*
     * Tree_0 - Right_Node
     * Initialize the LTA Input fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)input_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_right_node_create(fa_node_roots[0],
                                                 (void *)init_in_node_data);
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Copy the LTA Output fields list to API fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_to_api_fields,
                                       (void *)output_fields,
                                       &to_api_node_data));
    fa_node_roots[1] = bcmltm_tree_node_create((void *)to_api_node_data);
    SHR_NULL_CHECK(fa_node_roots[1], SHR_E_MEMORY);

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((fa_node_roots[0] == NULL) && (to_lta_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(to_lta_node_data);
        }
        if ((left_node_0 == NULL) && (init_output_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(init_output_node_data);
        }
        if ((fa_node_roots[1] == NULL) && (to_api_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(to_api_node_data);
        }

        bcmltm_md_fa_node_roots_destroy(num_roots, fa_node_roots);
        *fa_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for INSERT on IwA LT.
 *
 * Create the EE tree roots for an INSERT operation for given
 * Index with Allocation logical table.
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
lta_cth_alloc_index_ee_node_roots_insert_create(int unit,
                                    bcmlrd_sid_t sid,
                                    uint32_t num_roots,
                                    bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *insert_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * num_roots;
    SHR_ALLOC(ee_node_roots, size, "LTM EE node roots INSERT");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * INSERT - IwA
     *
     * EE Trees Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH insert with allocation operation)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler insert with allocation operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_table_params_retrieve(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lta_alloc_insert,
                                       (void *)table_params,
                                       &insert_node_data));
    ee_node_roots[0] = bcmltm_tree_node_create((void *)insert_node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

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
        if ((ee_node_roots[0] == NULL) && (insert_node_data != NULL)) {
            bcmltm_md_ee_node_data_destroy(insert_node_data);
        }

        bcmltm_md_ee_node_roots_destroy(num_roots, ee_node_roots);
        *ee_node_roots_ptr = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the INSERT opcode on Simple Index LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Simple Index LTs.
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
lta_cth_simple_index_op_insert_create(int unit, bcmlrd_sid_t sid,
                                      bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP INSERT metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INS_DEL_UPD_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_INSERT,
                                      op_md->num_roots,
                                      &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_INSERT,
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
 * \brief Create operation metadata for the INSERT opcode on IwA LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Index with Allocation LTs.
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
lta_cth_alloc_index_op_insert_create(int unit, bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP INSERT metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_ALLOC_INDEX_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_alloc_index_fa_node_roots_insert_create(unit, sid,
                                                  op_md->num_roots,
                                                  &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_alloc_index_ee_node_roots_insert_create(unit, sid,
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
lta_cth_op_insert_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_table_type_t type;

    SHR_FUNC_ENTER(unit);

    /* Get table type */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lt_info_table_type_get(unit, sid, &type));

    if (BCMLTM_TABLE_TYPE_LT_ALLOC_INDEX(type)) {
        /* IwA CTH */
        SHR_IF_ERR_EXIT
            (lta_cth_alloc_index_op_insert_create(unit, sid,
                                                  op_md_ptr));
    } else {
        /* SI CTH */
        SHR_IF_ERR_EXIT
            (lta_cth_simple_index_op_insert_create(unit, sid,
                                                   op_md_ptr));
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
lta_cth_fa_node_roots_lookup_create(int unit,
                                    bcmlrd_sid_t sid,
                                    uint32_t num_roots,
                                    bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *left_node_0 = NULL;
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    bcmltm_fa_node_t *init_output_node_data = NULL;
    bcmltm_fa_node_t *to_api_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    const bcmltm_lta_field_list_t *output_fields = NULL;

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
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert to
     *                     LTA Input fields)
     *                       |
     *                       |
     *         +-------------+-------------+
     *         |                           |
     *    [Left_Node]                 [Right_Node]
     *    (Init LTA Output fields)    (Init LTA Input fields)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (Copy LTA Output fields
     *                     to API fields list)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_in_key_fields_retrieve(unit, sid, &input_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_lta,
                                       (void *)input_fields,
                                       &to_lta_node_data));
    fa_node_roots[0] = bcmltm_tree_node_create((void *)to_lta_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    /*
     * Tree_0 - Left_Node
     * Initialize the LTA Output fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_out_fields_retrieve(unit, sid, &output_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)output_fields,
                                       &init_output_node_data));
    left_node_0 = bcmltm_tree_left_node_create(fa_node_roots[0],
                                               (void *)init_output_node_data);
    SHR_NULL_CHECK(left_node_0, SHR_E_MEMORY);

    /*
     * Tree_0 - Right_Node
     * Initialize the LTA Input fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)input_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_right_node_create(fa_node_roots[0],
                                                 (void *)init_in_node_data);
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Copy the LTA Output fields list to API fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_to_api_fields,
                                       (void *)output_fields,
                                       &to_api_node_data));
    fa_node_roots[1] = bcmltm_tree_node_create((void *)to_api_node_data);
    SHR_NULL_CHECK(fa_node_roots[1], SHR_E_MEMORY);

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((fa_node_roots[0] == NULL) && (to_lta_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(to_lta_node_data);
        }
        if ((left_node_0 == NULL) && (init_output_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(init_output_node_data);
        }
        if ((fa_node_roots[1] == NULL) && (to_api_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(to_api_node_data);
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
lta_cth_ee_node_roots_lookup_create(int unit,
                                    bcmlrd_sid_t sid,
                                    uint32_t num_roots,
                                    bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;

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
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH lookup operation)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler lookup operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_table_params_retrieve(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lta_lookup,
                                       (void *)table_params,
                                       &lookup_node_data));
    ee_node_roots[0] = bcmltm_tree_node_create((void *)lookup_node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

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
        if ((ee_node_roots[0] == NULL) && (lookup_node_data != NULL)) {
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
lta_cth_op_lookup_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP LOOKUP metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_lookup_create(unit, sid,
                                             op_md->num_roots,
                                             &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_lookup_create(unit, sid,
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
lta_cth_op_delete_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP DELETE metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INS_DEL_UPD_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_DELETE,
                                      op_md->num_roots,
                                      &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_DELETE,
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
lta_cth_op_update_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP UPDATE metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INS_DEL_UPD_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_UPDATE,
                                      op_md->num_roots,
                                      &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_UPDATE,
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
lta_cth_fa_node_roots_traverse_create(int unit,
                                      bcmlrd_sid_t sid,
                                      uint32_t num_roots,
                                      bcmltm_node_t ***fa_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *left_node_0 = NULL;
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    bcmltm_fa_node_t *init_output_node_data = NULL;
    bcmltm_fa_node_t *to_api_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    const bcmltm_lta_field_list_t *output_fields = NULL;

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
     * -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert to
     *                     LTA Input fields)
     *                       |
     *                       |
     *         +-------------+-------------+
     *         |                           |
     *    [Left_Node]                 [Right_Node]
     *    (Init LTA Output fields)    (Init LTA Input fields)
     *
     * -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (Copy LTA Output fields
     *                     to API fields list)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_in_key_fields_retrieve(unit, sid, &input_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_lta,
                                       (void *)input_fields,
                                       &to_lta_node_data));
    fa_node_roots[0] = bcmltm_tree_node_create((void *)to_lta_node_data);
    SHR_NULL_CHECK(fa_node_roots[0], SHR_E_MEMORY);

    /*
     * Tree_0 - Left_Node
     * Initialize the LTA Output fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_out_fields_retrieve(unit, sid, &output_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)output_fields,
                                       &init_output_node_data));
    left_node_0 = bcmltm_tree_left_node_create(fa_node_roots[0],
                                               (void *)init_output_node_data);
    SHR_NULL_CHECK(left_node_0, SHR_E_MEMORY);

    /*
     * Tree_0 - Right_Node
     * Initialize the LTA Input fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)input_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_right_node_create(fa_node_roots[0],
                                                 (void *)init_in_node_data);
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Copy the LTA Output fields list to API fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_to_api_fields,
                                       (void *)output_fields,
                                       &to_api_node_data));
    fa_node_roots[1] = bcmltm_tree_node_create((void *)to_api_node_data);
    SHR_NULL_CHECK(fa_node_roots[1], SHR_E_MEMORY);

    /* Set returning pointer */
    *fa_node_roots_ptr = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((fa_node_roots[0] == NULL) && (to_lta_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(to_lta_node_data);
        }
        if ((left_node_0 == NULL) && (init_output_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(init_output_node_data);
        }
        if ((fa_node_roots[1] == NULL) && (to_api_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(to_api_node_data);
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
lta_cth_ee_node_roots_traverse_create(int unit,
                                      bcmlrd_sid_t sid,
                                      uint32_t num_roots,
                                      bcmltm_node_t ***ee_node_roots_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_ee_node_t *traverse_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;

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
     * -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH traverse operation)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler traverse operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_cth_table_params_retrieve(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lta_traverse,
                                       (void *)table_params,
                                       &traverse_node_data));
    ee_node_roots[0] = bcmltm_tree_node_create((void *)traverse_node_data);
    SHR_NULL_CHECK(ee_node_roots[0], SHR_E_MEMORY);

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
        if ((ee_node_roots[0] == NULL) && (traverse_node_data != NULL)) {
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
lta_cth_op_traverse_create(int unit, bcmlrd_sid_t sid,
                           bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;
    const bcmltd_table_handler_t *cth_handler = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get CTH handler */
    SHR_IF_ERR_EXIT(bcmltm_nc_lta_cth_handler_retrieve(unit, sid,
                                                       &cth_handler));

    /* Check for NULL traverse handler */
    if ((cth_handler == NULL) || (cth_handler->traverse == NULL)) {
        *op_md_ptr = NULL;
        SHR_EXIT();
    }

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "LTM Logical Table OP TRAVERSE metadata");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_TRAVERSE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_traverse_create(unit, sid,
                                               op_md->num_roots,
                                               &op_md->fa_node_roots));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_traverse_create(unit, sid,
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
static bcmltm_md_lt_drv_t lta_cth_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lta_cth_op_insert_create,
    /* op_lookup_create */      lta_cth_op_lookup_create,
    /* op_delete_create */      lta_cth_op_delete_create,
    /* op_update_create */      lta_cth_op_update_create,
    /* op_traverse_create */    lta_cth_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lta_cth_drv_get(void)
{
    return &lta_cth_drv;
}
