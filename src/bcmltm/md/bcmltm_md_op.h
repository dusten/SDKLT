/*! \file bcmltm_md_op.h
 *
 * Logical Table Manager - Operation Metadata Definitions.
 *
 * This file contains implementations that are generic and
 * can be used by different types of Logical Tables during the
 * metadata construction.
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

#ifndef BCMLTM_MD_OP_H
#define BCMLTM_MD_OP_H

#include <shr/shr_types.h>

#include <bcmltm/lt_types.h>
#include <bcmltm/bcmltm_md_internal.h>

/*!
 * \brief Create FA node data.
 *
 * Create FA node data with given information.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function FA function for node.
 * \param [in] node_cookie Context information for given FA function.
 * \param [out] fa_node_ptr Returning pointer to FA node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fa_node_data_create(int unit,
                              bcmltm_fa_node_f node_function,
                              void *node_cookie,
                              bcmltm_fa_node_t **fa_node_ptr);


/*!
 * \brief Destroy FA node data.
 *
 * Destroy FA node data.
 *
 * \param [in] fa_node FA node data to destroy.
 */
extern void
bcmltm_md_fa_node_data_destroy(bcmltm_fa_node_t *fa_node);


/*!
 * \brief Destroy all the FA node data for the given tree.
 *
 * Destroy the the FA node data for all the nodes in the given tree.
 *
 * The function traverses the binary tree, and destroys the node_data for
 * each node in the tree.  This uses post-order recursive traversal
 * of the tree.
 *
 * \param [in] node FA tree to destroy.
 */
extern void
bcmltm_md_fa_tree_node_data_destroy(bcmltm_node_t *node);


/*!
 * \brief Destroy the list of given FA tree roots.
 *
 * Destroy the list of given FA tree roots.
 *
 * This function destroys the node_data stored in each tree along
 * with the tree itself.
 *
 * \param [in] num_roots Number of tree roots.
 * \param [in] fa_node_roots List of tree roots to destroy.
 */
extern void
bcmltm_md_fa_node_roots_destroy(size_t num_roots,
                                bcmltm_node_t **fa_node_roots);


/*!
 * \brief Create EE node data.
 *
 * Create EE node data with given information.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function EE function for node.
 * \param [in] node_cookie Context information for given EE function.
 * \param [out] ee_node_ptr Returning pointer to EE node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_ee_node_data_create(int unit,
                              bcmltm_ee_node_f node_function,
                              void *node_cookie,
                              bcmltm_ee_node_t **ee_node_ptr);


/*!
 * \brief Destroy EE node data.
 *
 * Destroy EE node data.
 *
 * \param [in] ee_node EE node data to destroy.
 */
extern void
bcmltm_md_ee_node_data_destroy(bcmltm_ee_node_t *ee_node);


/*!
 * \brief Destroy all the EE node data for the given tree.
 *
 * Destroy the EE node data for all the nodes in the given tree.
 *
 * The function traverses the binary tree, and destroys the node_data for
 * each node in the tree.  This uses post-order recursive traversal
 * of the tree.
 *
 * \param [in] node EE tree to destroy.
 */
extern void
bcmltm_md_ee_tree_node_data_destroy(bcmltm_node_t *node);


/*!
 * \brief Destroy the list of given EE tree roots.
 *
 * Destroy the list of given EE tree roots.
 *
 * This function destroys the node_data stored in each tree along
 * with the tree itself.
 *
 * \param [in] num_roots Number of tree roots.
 * \param [in] ee_node_roots List of tree roots to destroy.
 */
extern void
bcmltm_md_ee_node_roots_destroy(size_t num_roots,
                                bcmltm_node_t **ee_node_roots);


/*!
 * \brief Destroy given operation metadata object.
 *
 * Destroy given operation metadata.
 *
 * \param [in] op_md Operation metadata to destroy.
 */
extern void
bcmltm_md_op_destroy(bcmltm_lt_op_md_t *op_md);


#endif /* BCMLTM_MD_OP_H */
