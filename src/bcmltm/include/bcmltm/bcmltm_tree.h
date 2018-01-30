/*! \file bcmltm_tree.h
 *
 * Logical Table Manager Binary Tree.
 *
 * A set of functions and definitions for binary trees used for the LTM
 * stages.
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

#ifndef BCMLTM_TREE_H
#define BCMLTM_TREE_H


/*!
 * \brief  Basic LTM tree structure component.
 */
typedef struct bcmltm_node_s {
    /*! Untyped pointer to structure comprising this node. */
    void *node_data;

    /*! Pointer to a subnode on the "left" side of the binary tree. */
    struct bcmltm_node_s *left;

    /*! Pointer to a subnode on the "right" side of the binary tree. */
    struct bcmltm_node_s *right;
} bcmltm_node_t;

/*!
 * \brief Node operation function callback.
 *
 * The traversal operation function to invoke on each node
 * of an LTM binary tree in post-order.
 *
 * \param [in] unit Unit number.
 * \param [in] node_data The context information supplied to the
 *             per-node operational function.
 * \param [in] traverse_cookie A pointer to the context information for this
 *             traverse function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*bcmltm_tree_traverse_op_f)(int unit,
                                         void *node_data,
                                         void *traverse_cookie);

/*!
 * \brief Create a generic LTM binary tree node.
 *
 * Allocate and initialize a node which may be part of a binary tree.
 *
 * \param [in] node_data Generic pointer to the structure for this node.
 *
 * \retval Pointer to the new allocated tree, or NULL if failure to allocate.
 */
extern bcmltm_node_t *
bcmltm_tree_node_create(void *node_data);

/*!
 * \brief Create a left-hand generic LTM binary tree node.
 *
 * Allocate and initialize a node which may be part of a binary tree.
 * Attach the new node to the left-hand node pointer of the provided node.
 *
 * \param [in] parent_node An existing tree node to which a new
 *        left-hand node should be attached.
 * \param [in] node_data A generic pointer to information describing
 *        the new node.
 *
 * \retval Pointer to the new allocated tree, or NULL if failure to allocate.
 */
extern bcmltm_node_t *
bcmltm_tree_left_node_create(bcmltm_node_t *parent_node,
                             void *node_data);

/*!
 * \brief Create a right-hand generic LTM binary tree node.
 *
 * Allocate and initialize a node which may be part of a binary tree.
 * Attach the new node to the right-hand node pointer of the provided node.
 *
 * \param [in] parent_node An existing tree node to which a new
 *        right-hand node should be attached.
 * \param [in] node_data A generic pointer to information describing
 *        the new node.
 *
 * \retval Pointer to the new allocated tree, or NULL if failure to allocate.
 */
extern bcmltm_node_t *
bcmltm_tree_right_node_create(bcmltm_node_t *parent_node,
                              void *node_data);

/*!
 * \brief Traverse an LTM binary tree.
 *
 * Use the provided traversal operation function and cookie on each node
 * of an LTM binary tree in post-order.
 *
 * \param [in] unit Unit number.
 * \param [in] root_node The root node of the binary tree to traverse.
 * \param [in] traverse_op The function to execute at each node of the tree.
 * \param [in] traverse_cookie A pointer to the context information for this
 *        traverse function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_tree_traverse(int unit,
                     bcmltm_node_t *root_node,
                     bcmltm_tree_traverse_op_f traverse_op,
                     void *traverse_cookie);

/*!
 * \brief Deallocate a binary tree node.
 *
 * Deallocate a node of a binary tree.
 *
 * \param [in] node An existing tree node to eliminate.
 */
extern void
bcmltm_tree_node_destroy(bcmltm_node_t *node);


/*!
 * \brief Deallocate a binary tree structure.
 *
 * Deallocate each of the nodes of a binary tree.
 *
 * \param [in] node The root node of an existing tree to eliminate.
 */
extern void
bcmltm_tree_destroy(bcmltm_node_t *node);

#endif /* BCMLTM_TREE_H */
