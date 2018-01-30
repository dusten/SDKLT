/*! \file bcmltm_md_lta_xfrm.c
 *
 * Logical Table Manager - LTA Field Transform.
 *
 * This file contains implementation to support metadata construction
 * for logical tables with Field Transform.
 *
 * FA Field Transform Cluster
 * --------------------------
 *
 * The Field Transform cluster (for Keys and Values) is
 * composed of a number of subtrees formed as follows:
 *
 *                         [Parent]
 *                             |
 *                   +---------+---------+
 *                   |                   |
 *            [Xfrm_1 Subtree]          ===
 *                   |
 *          +--------+--------+
 *          |                 |
 *         ...               ===
 *    [Xfrm_N Subtree]
 *
 * Where "N" is the number of field transforms for the given
 * transform type.
 *
 * Each transform subtree is composed of a set of nodes.
 * For details, refer to diagram in corresponding function
 * that constructs the subtree.
 *
 * The order of execution in the cluster follow the same LTM
 * tree traverse order (i.e. post-order), which is:
 * Xfrm_N subtree, Xfrm_(N-1) subtree, ..., Xfrm_1 subtree.
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

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_nc_lt_info_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_lta_xfrm.h"
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
 * \brief Create the FA subtree for Forward Field Transform.
 *
 * Create the FA sub-tree for a Forward Field Transform.
 * This sub-tree is inserted in Left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] xfrm_idx Transform Index.
 * \param [in] field_def Indicates how to populate the LTA input field list.
 * \param [in] opcode LT opcode.
 * \param [in] fa_node_parent Parent node for this transform tree.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_fa_node_tree_fwd_create(int unit,
                             bcmlrd_sid_t sid,
                             int xfrm_type,
                             uint32_t xfrm_idx,
                             bcmltm_md_xfrm_default_t field_def,
                             bcmlt_opcode_t opcode,
                             bcmltm_node_t *fa_node_parent,
                             bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    /* #1 */
    const bcmltm_lta_field_list_t *out_fields = NULL;
    bcmltm_fa_node_t *init_out_node_data = NULL;
    bcmltm_node_t    *init_out_node = NULL;
    /* #2 */
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    /* #3 */
    const bcmltm_lta_field_list_t *copy_fields = NULL;
    bcmltm_fa_node_t *copy_node_data = NULL;
    bcmltm_node_t    *copy_node = NULL;
    bcmltm_node_t    *copy_node_root = NULL;
    uint32_t rev_xfrm_idx;
    uint32_t num_rev_xfrm = 0;
    int rev_xfrm_type = BCMLTM_LTA_XFRM_TYPE_REV_VALUE_TRANSFORM;
    /* #4 */
    const bcmltm_lta_field_list_t *in_fields = NULL;
    bcmltm_fa_node_t *api_to_lta_node_data = NULL;
    bcmltm_node_t    *api_to_lta_node = NULL;
    /* #5 */
    const bcmltm_lta_transform_params_t *lta_xfrm_params = NULL;
    bcmltm_fa_node_t *lta_xfrm_param_data = NULL;
    bcmltm_node_t    *lta_xfrm_param_node = NULL;
    bcmltm_fa_node_f lta_transform_func = NULL;
    /* #6 */
    bcmltm_fa_node_t *lta_to_wbc_node_data = NULL;
    bcmltm_node_t    *lta_to_wbc_node = NULL;
    bcmltm_fa_node_f api_to_lta_func = NULL;
    /* #7 */
    bcmltm_fa_node_t *lta_pt_sid_map_node_data = NULL;
    bcmltm_node_t    *lta_pt_sid_map_node = NULL;

    bcmltm_node_t    *root_node = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * FA FORWARD XFRM Sub Tree
     * ---------------------------------------------------------------
     *   Tree_0:                           [Parent]
     *                                        |
     *                                        |
     *                               +--------+-------+
     *                               |                |
     *                          [Left_Node]      [Right_Node]
     *                              #6               #7
     *                               |
     *                 +-------------+-------------+
     *                 |                           |
     *            [Left_Node]                 [Right_Node]
     *                #2                          #5
     *                 |                           |
     *         +-------+------+            +-------+------+
     *         |              |            |              |
     *    [Left_Node]    [Right_Node]  [Left_Node]   [Right_Node]
     *        #0             #1            #3            #4
     *
     * #0. empty node : chain to additional LTA transform subtrees.
     * #1. bcmltm_fa_node_lta_output_init   : Init LTA out(dst) fields(LTA-PTM)
     * #2. bcmltm_fa_node_lta_input_init    : Init LTA in(src) fields
     * #3. bcmltm_fa_node_lta_to_lta_fields : Copy LTA fields to input in(src)
     * #4. <fields_to_lta>                  : Init LTA in(src) fields
     *                                        Set LTA field list as follows:
     *     Depending on the field default param, one of these
     *     FA node functions is installed:
     *     - DEFAULT_NONE:
     *       If xfrm_type is Key
     *         bcmltm_fa_node_api_key_fields_to_lta()
     *         API entry input key fields are copied to the LTA field list.
     *       If xfrm_type is Value
     *         bcmltm_fa_node_api_value_fields_to_lta()
     *         API entry input value fields are copied to the LTA field list.
     *     - DEFAULT_UPDATE:
     *       bcmltm_fa_node_api_fields_update_to_lta()
     *       API entry input fields are copied to an exising LTA
     *       field list.  The API fields values must overwrite matching
     *       field elements in the list.
     *     - DEFAULT_ADD:
     *       bcmltm_fa_node_api_fields_default_to_lta()
     *       API entry input fields are copied to the LTA field list and,
     *       in addition, field default values are added to the LTA list for
     *       those fields which are not present in the input list.
     *       is not present in the input.
     *     - DEFAULT_ALL:
     *       bcmltm_fa_node_default_fields_to_lta()
     *       The LTA field list is populated with all default field values.
     *     - DEFAULT_ALLOC:
     *       bcmltm_fa_node_api_alloc_key_fields_to_lta()
     *       The LTA field list contains optional fields that can be
     *       allocated.
     * #5. bcmltm_fa_node_lta_transform     : Invoke LTA Transform function
     *     or
     *     bcmltm_fa_node_lta_traverse_transform (for Key on TRAVERSE)
     * #6. bcmltm_fa_node_lta_fields_to_wbc : CopyLTA transform output fields to
     *                                        PTM entry fields.
     * #7. bcmltm_fa_node_lta_pt_sid_map    : Convert LTA PT select index to
     *                                        PT SID
     *   -------------------------------------------------------------
     *
     * NOTE:
     * The Forward Transform function is invoked.
     * As such, the transform function expects the following:
     *    Input:  Source fields
     *    Ouput:  Destination fields
     */

    root_node = bcmltm_tree_left_node_create(fa_node_parent,
                                             (void *)NULL);
    SHR_NULL_CHECK(root_node, SHR_E_MEMORY);

    /**************************************************************************
     * #7. bcmltm_fa_node_lta_pt_sid_map: Convert LTA PT select index to PT SID
 */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_params_retrieve(unit, sid, xfrm_type, xfrm_idx,
                                            &lta_xfrm_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_pt_sid_map,
                                       (void *)lta_xfrm_params,
                                       &lta_pt_sid_map_node_data));
    lta_pt_sid_map_node = bcmltm_tree_right_node_create(root_node,
                                       (void *)lta_pt_sid_map_node_data);
    SHR_NULL_CHECK(lta_pt_sid_map_node, SHR_E_MEMORY);

    /**************************************************************************
     * #6. bcmltm_fa_node_lta_fields_to_wbc : Copy LTA transform output fields
     *                                        to PTM entry fields.
 */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_out_fields_retrieve(unit, sid, xfrm_type, xfrm_idx,
                                                &out_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_fields_to_wbc,
                                       (void *)out_fields,
                                       &lta_to_wbc_node_data));
    lta_to_wbc_node = bcmltm_tree_left_node_create(root_node,
                                       (void *)lta_to_wbc_node_data);
    SHR_NULL_CHECK(lta_to_wbc_node, SHR_E_MEMORY);

    /**************************************************************************
     * #5. <transform> : Invoke LTA Transform function
     *     - bcmltm_fa_node_lta_traverse_transform (for TRAVERSE)
     *     or
     *     - bcmltm_fa_node_lta_transform (for non-TRAVERSE)
 */
    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        lta_transform_func = bcmltm_fa_node_lta_traverse_transform;
    } else {
        lta_transform_func = bcmltm_fa_node_lta_transform;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       lta_transform_func,
                                       (void *)lta_xfrm_params,
                                       &lta_xfrm_param_data));
    lta_xfrm_param_node = bcmltm_tree_right_node_create(lta_to_wbc_node,
                                                (void *)lta_xfrm_param_data);
    SHR_NULL_CHECK(lta_xfrm_param_node, SHR_E_MEMORY);

    /**************************************************************************
     * #4. <fields_to_lta> : Init LTA in(src) fields.
     *                       Populate LTA field list with one of these:
     *                       - LTM entry input only, overwrite
     *                       - LTM entry input and defaults (unspecified fields)
     *                       - Defaults only
     *                       - LTM entry input only, keys optional (alloc)
     *                       - LTM entry input only, keys required
     *                       - LTM entry input only, values (optional)
 */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_in_fields_retrieve(unit, sid, xfrm_type, xfrm_idx,
                                               &in_fields));
    if (field_def == BCMLTM_MD_XFRM_DEFAULT_UPDATE) {
        api_to_lta_func = bcmltm_fa_node_api_fields_update_to_lta;
    } else if (field_def == BCMLTM_MD_XFRM_DEFAULT_ADD) {
        api_to_lta_func = bcmltm_fa_node_api_fields_default_to_lta;
    } else if (field_def == BCMLTM_MD_XFRM_DEFAULT_ALL) {
        api_to_lta_func = bcmltm_fa_node_default_fields_to_lta;
    } else if (field_def == BCMLTM_MD_XFRM_DEFAULT_ALLOC) {
        api_to_lta_func = bcmltm_fa_node_api_alloc_key_fields_to_lta;
    } else {
        /* DEFAULT_NONE */
        if (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) {
            api_to_lta_func = bcmltm_fa_node_api_key_fields_to_lta;
        } else {
            api_to_lta_func = bcmltm_fa_node_api_value_fields_to_lta;
        }
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       api_to_lta_func,
                                       (void *)in_fields,
                                       &api_to_lta_node_data));
    api_to_lta_node = bcmltm_tree_right_node_create(lta_xfrm_param_node,
                                       (void *)api_to_lta_node_data);
    SHR_NULL_CHECK(api_to_lta_node, SHR_E_MEMORY);

    /**************************************************************************
     * #3. bcmltm_fa_node_lta_to_lta_fields : Sub-tree of list copies
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
        (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM) &&
        (field_def == BCMLTM_MD_XFRM_DEFAULT_UPDATE)) {
        SHR_IF_ERR_EXIT
            (bcmltm_nc_lta_xfrm_num_get(unit, sid,
                                        rev_xfrm_type, &num_rev_xfrm));

        if (num_rev_xfrm > 0) {
            /* Add Copy subtrees */
            copy_node_root = lta_xfrm_param_node;
            for (rev_xfrm_idx = 0;
                 rev_xfrm_idx < num_rev_xfrm;
                 rev_xfrm_idx++) {
                SHR_IF_ERR_EXIT
                    (bcmltm_nc_lta_xfrm_copy_fields_retrieve(unit, sid,
                                                             xfrm_type,
                                                             xfrm_idx,
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
     * #2. bcmltm_fa_node_lta_input_init    : Init LTA Input fields
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                      bcmltm_fa_node_lta_input_init,
                                       (void *)in_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(lta_to_wbc_node,
                                                 (void *)init_in_node_data);
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /**************************************************************************
     * #0. empty node : Chain to additional LTA transform subtrees.
 */
    next_chain_node = bcmltm_tree_left_node_create(init_in_node,
                                                   (void *)NULL);
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /**************************************************************************
     * #1. bcmltm_fa_node_lta_output_init   : Init LTA Output fields
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)out_fields,
                                       &init_out_node_data));
    init_out_node = bcmltm_tree_right_node_create(init_in_node,
                                                (void *)init_out_node_data);
    SHR_NULL_CHECK(init_out_node, SHR_E_MEMORY);

    *next_chain = next_chain_node;
 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if ((lta_to_wbc_node == NULL) && (lta_to_wbc_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lta_to_wbc_node_data);
        }

        if ((api_to_lta_node == NULL) && (api_to_lta_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(api_to_lta_node_data);
        }

        if ((lta_xfrm_param_node == NULL) && (lta_xfrm_param_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lta_xfrm_param_data);
        }

        if ((init_out_node == NULL) && (init_out_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(init_out_node_data);
        }
        if ((init_in_node == NULL) && (init_in_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(init_in_node_data);
        }
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA subtree for Reverse Field Transform.
 *
 * Create the FA sub-tree for a Reverse Field Transform.
 * This sub-tree is inserted in Left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] xfrm_idx Transform Index.
 * \param [in] opcode LT opcode.
 * \param [in] fa_node_parent Parent node for this transform tree.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_fa_node_tree_rev_create(int unit,
                             bcmlrd_sid_t sid,
                             int xfrm_type,
                             uint32_t xfrm_idx,
                             bcmlt_opcode_t opcode,
                             bcmltm_node_t *fa_node_parent,
                             bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    const bcmltm_lta_field_list_t *out_fields = NULL; /* Used by several */
    const bcmltm_lta_field_list_t *in_fields = NULL; /* Used by several */
    /* #1 */
    bcmltm_fa_node_t *init_out_node_data = NULL;
    bcmltm_node_t    *init_out_node = NULL;
    /* #2 */
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    /* #3 */
    bcmltm_fa_node_t *wbc_to_lta_node_data = NULL;
    bcmltm_node_t    *wbc_to_lta_node = NULL;
    /* #4 */
    const bcmltm_lta_transform_params_t *lta_xfrm_params = NULL;
    bcmltm_fa_node_t *lta_xfrm_param_data = NULL;
    bcmltm_node_t    *lta_xfrm_param_node = NULL;
    /* #5 */
    bcmltm_fa_node_t *lta_to_api_node_data = NULL;
    bcmltm_node_t    *lta_to_api_node = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * FA REVERSE XFRM Sub Tree
     * ---------------------------------------------------------------
     *   Tree_0:                            [Parent]
     *                                         |
     *                                         |
     *                                +--------+-------+
     *                                |               ===
     *                           [Left_Node]
     *                               #5
     *                                |
     *                 +--------------+------------+
     *                 |                           |
     *            [Left_Node]                 [Right_Node]
     *                #2                          #4
     *                 |                           |
     *         +-------+------+            +-------+------+
     *         |              |            |              |
     *    [Left_Node]    [Right_Node] [Left_Node]        ===
     *        #0             #1           #3
     *
     * #0. next chain node : chain to additional LTA transform subtrees.
     * #1. bcmltm_fa_node_lta_output_init   : Init LTA out(src) fields.
     * #2. bcmltm_fa_node_lta_input_init    : Init LTA in(dst) fields.
     * #3. bcmltm_fa_node_wbc_to_lta_fields : Copy PTM to LTA in(dst) fields.
     * #4. bcmltm_fa_node_lta_transform     : Invoke LTA rev Transform function
     * #5. <lta_to_api_fields>              : Copy LTA out(src) fields to LTM.
     *     bcmltm_fa_node_lta_to_api_fields()
     *     bcmltm_fa_node_lta_to_api_alloc_fields()
     *   -------------------------------------------------------------
     * NOTE:
     * The Reverse Transform function is invoked.
     * As such, the reverse transform function expects the following:
     *    Input:  Destination fields
     *    Ouput:  Source fields
     */
    /**************************************************************************
     * #5. <lta_to_api_fields> : Copy LTA out(src) fields to API field list.
     *     - bcmltm_fa_node_lta_to_api_fields()
     *       Copy all LTA out(src) fields to API field list.
     *       Valid for TRAVERSE/UPDATE opcodes.
     *     - bcmltm_fa_node_lta_to_api_alloc_fields()
     *       Copy all LTA out(src) fields to API field list only if
     *       fields were absent during INSERT.
     *       Valid for INSERT on IwA LTs.
     *
     *     Note:  Not present for UPDATE opcode.  Replace with empty node.
 */
    /* We need the out fields list later for the LTA output init */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_out_fields_retrieve(unit, sid, xfrm_type,
                                                xfrm_idx, &out_fields));
    if ((opcode == BCMLT_OPCODE_TRAVERSE) ||
        (opcode == BCMLT_OPCODE_LOOKUP)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_lta_to_api_fields,
                                           (void *)out_fields,
                                           &lta_to_api_node_data));
    }  else if (opcode == BCMLT_OPCODE_INSERT) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                    bcmltm_fa_node_lta_to_api_alloc_fields,
                                           (void *)out_fields,
                                           &lta_to_api_node_data));
    }  /* Else, null node data, tree traverse will skip */
    lta_to_api_node = bcmltm_tree_left_node_create(fa_node_parent,
                                           (void *)lta_to_api_node_data);
    SHR_NULL_CHECK(lta_to_api_node, SHR_E_MEMORY);

    /**************************************************************************
     * #2. bcmltm_fa_node_lta_input_init : Init LTA in(dst) fields (input).
 */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_in_fields_retrieve(unit, sid, xfrm_type, xfrm_idx,
                                               &in_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)in_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(lta_to_api_node,
                                                  (void *)init_in_node_data);
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);


    /**************************************************************************
     * #4. bcmltm_fa_node_lta_transform     : Invoke LTA rev Transform function
 */
    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_params_retrieve(unit, sid, xfrm_type, xfrm_idx,
                                            &lta_xfrm_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_transform,
                                       (void *)lta_xfrm_params,
                                       &lta_xfrm_param_data));
    lta_xfrm_param_node = bcmltm_tree_right_node_create(lta_to_api_node,
                                                (void *)lta_xfrm_param_data);
    SHR_NULL_CHECK(lta_xfrm_param_node, SHR_E_MEMORY);

    /**************************************************************************
     * #0. next chain node : chain to additional LTA transform subtrees.
 */
    next_chain_node = bcmltm_tree_left_node_create(init_in_node,
                                               (void *)NULL);
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /**************************************************************************
     * #1. bcmltm_fa_node_lta_output_init   : Init LTA out(src) fields (output).
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)out_fields,
                                       &init_out_node_data));
    init_out_node = bcmltm_tree_right_node_create(init_in_node,
                                                  (void *)init_out_node_data);
    SHR_NULL_CHECK(init_out_node, SHR_E_MEMORY);

    /**************************************************************************
     * #3. bcmltm_fa_node_wbc_to_lta_fields : Copy PTM to LTA in(dst) fields.
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wbc_to_lta_fields,
                                       (void *)in_fields,
                                       &wbc_to_lta_node_data));
    wbc_to_lta_node = bcmltm_tree_left_node_create(lta_xfrm_param_node,
                                                  (void *)wbc_to_lta_node_data);
    SHR_NULL_CHECK(wbc_to_lta_node, SHR_E_MEMORY);

    *next_chain = next_chain_node;
 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        /* #5 */
        if ((lta_to_api_node == NULL) && (lta_to_api_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lta_to_api_node_data);
        }
        /* #2 */
        if ((init_in_node == NULL) && (init_in_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(init_in_node_data);
        }
        /* #4 */
        if ((lta_xfrm_param_node == NULL) && (lta_xfrm_param_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(lta_xfrm_param_data);
        }
        /* #1 */
        if ((init_out_node == NULL) && (init_out_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(init_out_node_data);
        }
        /* #3 */
        if ((wbc_to_lta_node == NULL) && (wbc_to_lta_node_data != NULL)) {
            bcmltm_md_fa_node_data_destroy(wbc_to_lta_node_data);
        }
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Check if given transform contains required or optional keys.
 *
 * This routine checks if given transform contains required key fields
 * on an INSERT for Index with Allocation LTs.
 *
 * On Index with Allocation LTs, some keys can be absent during INSERT,
 * and if so, they are allocated by LTM.
 *
 * - Required keys:  those that correspond to the LT memory parameters,
 *                   i.e. PT port, PT instance.
 * - Optional keys:  those that correspond to the LT index,
 *                   i.e. PT index.
 *
 * \param [in] xfrm_type Transform type, Forward/Reverse - Key/Value.
 * \param [in] xfrm_arg Transform argument.
 *
 * \retval TRUE, if transform contains required key fields for INSERT on IwA.
 * \retval FALSE, if transform contains optional key fields for INSERT on IwA.
 */
static bool
xfrm_alloc_keys_required(int xfrm_type,
                         const bcmltd_transform_arg_t *xfrm_arg)
{
    const uint32_t *fields;
    uint32_t num_fields;
    uint32_t fidx;

    /* Destination field list (PT) based on forward or reverse transform */
    if ((xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM) ||
        (xfrm_type == BCMLTM_LTA_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
        fields = xfrm_arg->rfield;
        num_fields = xfrm_arg->rfields;
    } else {
        fields = xfrm_arg->field;
        num_fields = xfrm_arg->fields;
    }

    /* Search field list */
    for (fidx = 0; fidx < num_fields; fidx++) {
        if (fields[fidx] == BCMLRD_FIELD_INDEX) {
            /* Transform contains optional key, i.e. index */
            return FALSE;
        }
    }

    return TRUE;
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_xfrm_fa_trees_fwd_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmltm_md_xfrm_default_t field_def,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {
            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_fwd_create(unit, sid,
                                              xfrm_type, idx, field_def,
                                              opcode,
                                              parent_node, next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_xfrm_fa_trees_rev_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {
            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_rev_create(unit, sid,
                                              xfrm_type, idx,
                                              opcode,
                                              parent_node, next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_xfrm_fa_trees_fwd_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;
    int xfrm_type = BCMLTM_LTA_XFRM_TYPE_FWD_KEY_TRANSFORM;
    bcmltm_md_xfrm_default_t field_def;
    const bcmltm_lta_transform_params_t *xfrm_params = NULL;
    bool required;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {

            /*
             * Determine if transform contains required or optional key fields.
             * Assumes transform construction is correct, i.e. IwA
             * transform should not contain a mix of required and optional
             * fields.
             */
            SHR_IF_ERR_EXIT
                (bcmltm_nc_lta_xfrm_params_retrieve(unit, sid,
                                                    xfrm_type, idx,
                                                    &xfrm_params));
            required = xfrm_alloc_keys_required(xfrm_type,
                                                xfrm_params->lta_args);

            if (required) {
                field_def = BCMLTM_MD_XFRM_DEFAULT_NONE;
            } else {
                field_def = BCMLTM_MD_XFRM_DEFAULT_ALLOC;
            }

            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_fwd_create(unit, sid,
                                              xfrm_type, idx, field_def,
                                              opcode,
                                              parent_node, next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_xfrm_fa_trees_rev_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;
    int xfrm_type = BCMLTM_LTA_XFRM_TYPE_REV_KEY_TRANSFORM;
    const bcmltm_lta_transform_params_t *xfrm_params = NULL;
    bool required;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    /* Sanity check */
    if (opcode != BCMLT_OPCODE_INSERT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid opcode for "
                              "xfrm_fa_trees_rev_alloc_create: "
                              "sid=%d expected_op=%d actual_op=%d\n"),
                   sid, BCMLT_OPCODE_INSERT, opcode));
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_nc_lta_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {

            /*
             * Install only transforms that contain the optional key fields.
             * If optional key fields are absent during input, they
             * are allocated and returned in the API field list.
             */
            SHR_IF_ERR_EXIT
                (bcmltm_nc_lta_xfrm_params_retrieve(unit, sid,
                                                    xfrm_type, idx,
                                                    &xfrm_params));
            required = xfrm_alloc_keys_required(xfrm_type,
                                                xfrm_params->lta_args);
            if (required) {
                continue;
            }

            /* Transform contains optional keys */
            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_rev_create(unit, sid,
                                              xfrm_type, idx,
                                              opcode,
                                              parent_node, next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}
