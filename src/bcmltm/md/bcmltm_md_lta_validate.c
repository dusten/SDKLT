/*! \file bcmltm_md_lta_validate.c
 *
 * Logical Table Manager - LTA Field Validation.
 *
 * This file contains implementation to support metadata construction
 * for logical tables with Field Validation.
 *
 * FA Field Validation Cluster
 * ---------------------------
 *
 * The Field Validate cluster (for Keys and Values) is
 * composed of a number of subtrees formed as follows:
 *
 *                         [Parent]
 *                             |
 *                   +---------+---------+
 *                   |                   |
 *            [Validate_1 Subtree]      ===
 *                   |
 *          +--------+--------+
 *          |                 |
 *         ...               ===
 *   [Validate_N Subtree]
 *
 * Where "N" is the number of field validations for the given
 * field type.
 *
 * Each validation subtree is composed of a set of nodes.
 * For details, refer to diagram in corresponding function
 * that constructs the subtree.
 *
 * The order of execution in the cluster follow the same LTM
 * tree traverse order (i.e. post-order), which is:
 * Validate_N subtree, Validate_(N-1) subtree, ..., Validate_1 subtree.
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

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_lta_validate.h"
#include "bcmltm_md_op.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the FA subtree for Field Validation.
 *
 * Create the FA sub-tree for Field Validation.
 * This sub-tree is inserted in Left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Field type.
 * \param [in] field_idx The index of validate field.
 * \param [in] opcode LT opcode.
 * \param [in] fa_node_parent Parent node for the field validation sub tree.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_fa_node_tree_create(int unit,
                             bcmlrd_sid_t sid,
                             bcmltm_field_type_t field_type,
                             uint32_t field_idx,
                             bcmlt_opcode_t opcode,
                             bcmltm_node_t *fa_node_parent,
                             bcmltm_node_t **next_chain)
{
    bcmltm_node_t *parent_node = NULL;
    bcmltm_node_t *next_chain_node = NULL;
    /* #1 */
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    /* #2 */
    const bcmltm_lta_field_list_t *copy_fields = NULL;
    bcmltm_fa_node_t *copy_node_data = NULL;
    bcmltm_node_t    *copy_node = NULL;
    bcmltm_node_t    *copy_node_root = NULL;
    uint32_t rev_xfrm_idx;
    uint32_t num_rev_xfrm = 0;
    int rev_xfrm_type = BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM;
    /* #3 */
    const bcmltm_lta_field_list_t *src_fields = NULL;
    bcmltm_fa_node_t *api_to_lta_node_data = NULL;
    bcmltm_node_t    *api_to_lta_node = NULL;
    bcmltm_fa_node_f api_to_lta_func = NULL;
    /* #4 */
    const bcmltm_lta_validate_params_t *lta_validate_params = NULL;
    bcmltm_fa_node_t *lta_validate_param_data = NULL;
    bcmltm_node_t    *lta_validate_param_node = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * FA Field Validation Sub Tree
     * ---------------------------------------------------------------
     *   Tree_0:                           [Parent]
     *                                        |
     *                                        |
     *                               +--------+-------+
     *                               |                |
     *                          [Left_Node]
     *                              #5
     *                               |
     *                 +-------------+----------------+
     *                 |                              |
     *            [Left_Node]                    [Right_Node]
     *                #1                             #4
     *                 |                              |
     *         +-------+------+           +-----------+-----------+
     *         |              |           |                       |
     *    [Left_Node]        ===     [Left_Node]             [Right_Node]
     *        #0                         #2                      #3
     *
     * #0. next chain node : chain to additional LTA Field Validation subtrees.
     * #1. bcmltm_fa_node_lta_input_init    : Init LTA src fields
     * #2. bcmltm_fa_node_lta_to_lta_fields : Copy LTA rev xfrm fields src
     * #3. bcmltm_fa_node_api_fields_to_lta : Copy LTM entry input fields to
     *                                        LTA(LTA-LTM).
     * #4. bcmltm_fa_node_lta_validate      : Invoke LTA valiate function.
     * #5. bcmltm_fa_node_lta_fields_to_wbc : See below NOTE section.
     * ---------------------------------------------------------------
     * NOTE:
     * #5 lta_fields_to_wbc node is currently NULL node because the API field is
     * copied into PTM by the api_to_schan node.
     */

    /**************************************************************************
     * #5. bcmltm_fa_node_lta_fields_to_wbc : See above NOTE section.
 */
    parent_node = bcmltm_tree_left_node_create(fa_node_parent, (void *)NULL);
    SHR_NULL_CHECK(parent_node, SHR_E_MEMORY);
    /**************************************************************************
     * #4. bcmltm_fa_node_lta_validate      : Invoke LTA validate function.
 */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_validate_params_retrieve(unit, sid, field_type,
                                       field_idx, &lta_validate_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit, bcmltm_fa_node_lta_validate,
                                       (void *)lta_validate_params,
                                       &lta_validate_param_data));
    lta_validate_param_node = bcmltm_tree_right_node_create(parent_node,
                                       (void *)lta_validate_param_data);
    SHR_NULL_CHECK(lta_validate_param_node, SHR_E_MEMORY);

    /**************************************************************************
     * #3. bcmltm_fa_node_api_fields_to_lta : Copy LTM entry input fields to
     *                                        LTA(LTA-LTM).
 */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_validate_src_fields_retrieve(unit, sid, field_type,
                                                    field_idx, &src_fields));
    if (opcode == BCMLT_OPCODE_UPDATE) {
        api_to_lta_func = bcmltm_fa_node_api_fields_update_to_lta;
    } else if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        api_to_lta_func = bcmltm_fa_node_api_key_fields_to_lta;
    } else {
        api_to_lta_func = bcmltm_fa_node_api_value_fields_to_lta;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       api_to_lta_func,
                                       (void *)src_fields,
                                       &api_to_lta_node_data));
    api_to_lta_node = bcmltm_tree_right_node_create(lta_validate_param_node,
                                       (void *)api_to_lta_node_data);
    SHR_NULL_CHECK(api_to_lta_node, SHR_E_MEMORY);

    /**************************************************************************
     * #2. bcmltm_fa_node_lta_to_lta_fields : Sub-tree of list copies
 */
    /* We need to iterate over all _reverse_ transforms' out_fields lists
     * to see if any of our input fields are from those outputs.
     * They should be, since the reverse transforms should be
     * undoing the work of the forward transforms.
     * We do not presume that the forward and reverse transforms
     * are a 1-1 correspondence.  Only that all fields transformed
     * forward are transformed in reverse.  That is, the overall
     * collection of forward transforms result in the same set of fields
     * upon which the overall collection of reverse transform work.
     * But an individual forward transform does not correspond to
     * any particular subset of reverse transforms, even though this is
     * likely the common case.
     */
    if ((opcode == BCMLT_OPCODE_UPDATE) &&
        (field_type == BCMLTM_FIELD_TYPE_VALUE)) {
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lta_xfrm_num_get(unit, sid,
                                        rev_xfrm_type, &num_rev_xfrm));

        if (num_rev_xfrm > 0) {
            /* Add Copy subtrees */
            copy_node_root = lta_validate_param_node;
            for (rev_xfrm_idx = 0;
                 rev_xfrm_idx < num_rev_xfrm;
                 rev_xfrm_idx++) {
                SHR_IF_ERR_EXIT
                    (bcmltm_nc_lta_validate_copy_fields_retrieve(unit, sid,
                                                             field_type,
                                                             field_idx,
                                                             rev_xfrm_type,
                                                             rev_xfrm_idx,
                                                             &copy_fields));

                if (copy_fields == NULL) {
                    /* No field in this crossbar, skip. */
                    continue;
                }

                SHR_IF_ERR_EXIT
                    (bcmltm_md_fa_node_data_create(unit,
                                            bcmltm_fa_node_lta_to_lta_fields,
                                                   (void *)copy_fields,
                                                   &copy_node_data));
                copy_node = bcmltm_tree_left_node_create(copy_node_root,
                                                  (void *)copy_node_data);
                SHR_NULL_CHECK(copy_node, SHR_E_MEMORY);
                /* Build a tree downward as needed */
                copy_node_root = copy_node;
            }
        }
    }

     /**************************************************************************
     * #1. bcmltm_fa_node_lta_input_init    : Init LTA Input fields
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)src_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(parent_node,
                                                 (void *)init_in_node_data);
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /**************************************************************************
     * #0. empty node : chain to additional LTA validation subtrees.
 */
    next_chain_node = bcmltm_tree_left_node_create(init_in_node,
                                                   (void *)NULL);
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((api_to_lta_node == NULL) && (api_to_lta_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(api_to_lta_node_data);
        }

        if ((lta_validate_param_node == NULL) &&
            (lta_validate_param_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lta_validate_param_data);
        }
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_validate_fa_trees_create(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_field_type_t field_type,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_validate = 0;
    bcmltm_node_t *parent_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_validate_field_num_get(unit, sid, field_type,
                                              &num_validate));
    if (num_validate > 0) {
        /* Set parent node for Validate */
        parent_node = parent_chain;

        /* Add Validate subtrees */
        for (idx = 0; idx < num_validate; idx++) {
            SHR_IF_ERR_EXIT
                (validate_fa_node_tree_create(unit, sid,
                                              field_type, idx, opcode,
                                              parent_node, next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

