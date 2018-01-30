/*! \file bcmltm_fa_tree_nodes_internal.h
 *
 * Logical Table Manager Binary Trees.
 * Field Adaptation Nodes.
 *
 * A set of functions and definitions forming the nodes of
 * LTM FA binary trees.
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

#ifndef BCMLTM_FA_TREE_NODES_INTERNAL_H
#define BCMLTM_FA_TREE_NODES_INTERNAL_H

#include <bcmltm/bcmltm_md_internal.h>


/*!
 * \brief Extract field list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_A2S_FIELD_LIST(_nc_) \
    (((bcmltm_to_ptm_mapping_t *)(_nc_))->field_list)


/*!
 * \brief Extract fixed value field list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_FIXED_FIELD_LIST(_nc_) \
    ((bcmltm_fixed_field_list_t *)(_nc_))


/*!
 * \brief Extract WB copy list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_WB_COPY_LIST(_nc_) \
    ((bcmltm_wb_copy_list_t *)(_nc_))


/*!
 * \brief Extract LT index params from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_LT_INDEX(_nc_) \
    ((bcmltm_fa_lt_index_t *)(_nc_))


/*!
 * \brief Extract Tracking Index params from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_TRACK_INDEX(_nc_) \
    ((bcmltm_track_index_t *)(_nc_))


/*!
 * \brief Extract lta field map list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_LTA_FIELD_LIST(_nc_) \
    ((bcmltm_lta_field_list_t *)(_nc_))


/*!
 * \brief Retrieve pointer to LTA transfrom parameters structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_LTA_TRANSFORM_PARAMS(_nc_) \
    ((bcmltm_lta_transform_params_t *)(_nc_))


/*!
 * \brief Retrieve pointer to LTA validate parameters structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_LTA_VALIDATE_PARAMS(_nc_) \
    ((bcmltm_lta_validate_params_t *)(_nc_))

/*!
 * \brief Existence requirements for input fields.
 *
 * Types of API field validation for copying values to the Working Buffer
 * or LTA.
 */
typedef enum bcmltm_fields_required_e {
    BCMLTM_FIELDS_REQUIRED_NONE,
    BCMLTM_FIELDS_REQUIRED_UNSET,
    BCMLTM_FIELDS_REQUIRED_DEFAULT,
    BCMLTM_FIELDS_REQUIRED_ALL,
    BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE,
} bcmltm_fields_required_t;

/**
 * Basic FA node operations
 * - Tree traversal invocation
 * - Individual node processing
 */

/*!
 * \brief Invoke a single FA stage tree for traversal.
 *
 * Provide the root node, node traversal callback function, and tree
 * cookie to the general binary tree traversal function in order to
 * conduct an Field Adaptation stage of the LTM.
 *
 * \param [in] unit Unit number.
 * \param [in] root_node The root node of the binary tree to traverse.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_stage(int unit, bcmltm_node_t *root_node,
                bcmltm_lt_md_t *lt_md,
                bcmltm_lt_state_t *lt_state,
                bcmltm_entry_t *lt_entry,
                uint32_t *ltm_buffer);

/*!
 * \brief Invoke a FA stage tree node.
 *
 * Using the generic tree node information of an FA stage tree, perform the
 * encoded node operation to accomplish an FA task.
 *
 * Note - this is the traverse function which transforms a generic LTM tree
 * into an FA stage tree.
 *
 * \param [in] unit Unit number.
 * \param [in] node_data The information which allows an operation
 *             to be performed at the node.
 * \param [in] traverse_cookie A pointer to the context information for this
 *        traverse function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node(int unit, void *node_data, void *traverse_cookie);

/*!
 * \brief An FA tree node to copy API key fields into the Working Buffer.
 *
 * An FA tree node which copies the API key fields of an entry into
 * locations within the Working Buffer.
 * This function will return an error if any of the mapped fields are
 * missing from the API field list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_key_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);
/*!
 * \brief An FA tree node to copy API alloc key fields into the WB.
 *
 * An FA tree node which copies the API alloc key fields of an entry into
 * locations within the Working Buffer.
 * If no key fields are present, the keys absent flag is set in the WB.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_alloc_key_fields_to_wb(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie);

/*!
 * \brief An FA tree node to copy API value fields into the Working Buffer.
 *
 * An FA tree node which copies the API value fields of an entry into
 * locations within the Working Buffer.
 * This function will not return an error if any mapped fields are
 * missing from the API field list.  They are supplied by the default
 * value FA node later.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_value_fields_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to copy Working Buffer values to API fields.
 *
 * An FA tree node which copies values within the Working Buffer to
 * an API field list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wb_to_api_fields(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An FA tree node to conditionally copy Working Buffer values
 *        to API fields.
 *
 * An FA tree node which conditionally copies index keys within the
 * Working Buffer to an API field list.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wb_to_api_alloc_fields(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to place fixed field values into the WB.
 *
 * An FA tree node which writes fixed field values into the Working
 * Buffer.  These are most likely fixed physical field values --
 * such as key type, data type, or mode selection --
 * that are fixed for the specific LT.  But they may be arguments to
 * other logic that need to be placed into the WB, say inputs to the
 * flex counter specifying the type of resource.
 * The maximum value size of one element in the list is 32-bit, but
 * several entries may be added to produce a larger value.  This
 * is expected to be a very rare case, so it is not optimized.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_fixed_fields_to_wb(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An FA tree node to clear fixed field values in the WB.
 *
 * An FA tree node which writes fixed field values into the Working
 * Buffer.  These are most likely fixed physical field values --
 * such as key type, data type, or mode selection --
 * that are fixed for the specific LT.  But they may be arguments to
 * other logic that need to be placed into the WB, say inputs to the
 * flex counter specifying the type of resource.
 * The maximum value size of one element in the list is 32-bit, but
 * several entries may be added to produce a larger value.  This
 * is expected to be a very rare case, so it is not optimized.
 *
 * This function copies the default values for DELETE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_delete_fixed_fields_to_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to insert default field values into the WB
 *
 * An FA tree node which writes the default field values into the Working
 * Buffer.  The default values are supplied during metadata initialization
 * from the software defaults of the LT fields.
 *
 * If this becomes runtime adjustable state, the storage in metadata will
 * need adjustment.  For now, this function uses a node cookie
 * of bcmltm_to_ptm_mapping_t.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_default_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);

/*!
 * \brief An FA tree node to revert to default field values into the WB
 *
 * An FA tree node which writes default field values into the Working
 * Buffer for selected fields during UPDATE ops.  If the
 * SHR_FMM_FIELD_DEL field flag is set for an input value field,
 * replace the current entry value(s) with the field default.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_unset_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to copy a value in a specific Working Buffer
 * location to a different specific Working Buffer location.
 *
 * An FA tree node which extracts a value from a specific Working
 * Buffer location, then copies the value to a different
 * specific Working Buffer location.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wbc_to_wbc(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An FA tree node to conditionally copy a list of values in
 * specific Working Buffer locations to different specific
 * Working Buffer locations.
 *
 * An FA tree node which conditionally copies WB values based upon
 * the Index Absent flag used for Index with Allocation LT INSERT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_wbc_to_wbc(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An FA tree node to insert default fields to LTA field arrays.
 *
 * An FA tree node which inserts default fields to an array of
 * Logical Table Adapter fields.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_default_fields_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to convert API key fields to LTA field arrays.
 *
 * An FA tree node which converts the API key fields to an array of
 * Logical Table Adapter fields.
 * This node is used for key fields only, and requires all API
 * fields in the metadata list are present.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_key_fields_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to convert API alloc key fields to
 *        LTA field arrays.
 *
 * An FA tree node which converts the API alloc key fields to an array of
 * Logical Table Adapter fields.
 * If no key fields are present, the keys absent flag is set in the WB.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_alloc_key_fields_to_lta(int unit,
                                           bcmltm_lt_md_t *lt_md,
                                           bcmltm_lt_state_t *lt_state,
                                           bcmltm_entry_t *lt_entry,
                                           uint32_t *ltm_buffer,
                                           void *node_cookie);

/*!
 * \brief An FA tree node to convert API value fields to LTA field arrays.
 *
 * An FA tree node which converts the API value fields to an array of
 * Logical Table Adapter fields.
 * This node is used for value fields only, so not all API
 * fields in the metadata list must be present.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_value_fields_to_lta(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie);

/*!
 * \brief An FA tree node to convert API fields or default values
 * to LTA field arrays.
 *
 * An FA tree node which converts the API fields to an array of
 * Logical Table Adapter fields.  If an API field is not present
 * in the input list, the default value is added to the LTA list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_default_to_lta(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to update API fields values within an
 * existing LTA field array.
 *
 * An FA tree node which transfers the API fields provided to an
 * existing array of Logical Table Adapter fields.  The API field ids
 * must be present already in the LTA field list.
 * This operation is required to perform the read-modify-write step
 * of an UPDATE opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_update_to_lta(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie);

/*!
 * \brief An FA tree node to convert an LTA field array to API fields.
 *
 * An FA tree node which converts an array of Logical Table Adapter
 * fields into API fields of an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_to_api_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to conditionally convert an LTA field array
 *        to API fields.
 *
 * An FA tree node which conditionally converts an array of
 * Logical Table Adapter fields into API fields of an entry.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_to_api_alloc_fields(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie);

/*!
 * \brief An FA tree node to prepare an input LTA field list.
 *
 * An FA tree node which connects the Working Buffer locations
 * reserved for the input field list of an LTA function.
 * The parameter array structure is initialized, and the count of
 * fields is reset.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_input_init(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to prepare an output LTA field list.
 *
 * An FA tree node which connects the Working Buffer locations
 * reserved for the output field list of an LTA function.
 * The parameter structure is configured with a number of elements
 * for the maximum number of output values, and the field value pointer
 * array is filled in.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_output_init(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie);

/*!
 * \brief An FA tree node to copy an LTA field array element to
 * another LTA field structure.
 *
 * An FA tree node which searches for field array elements in
 * a Logical Table Adapter fields list and appends them to a different
 * LTA fields list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to copy an LTA field array element to
 * a specific Working Buffer location.
 *
 * An FA tree node which searches for field array elements in
 * a Logical Table Adapter fields list, then copies the field value
 * to a specific Working Buffer location.  The location is generally
 * a PTM entry or memory parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_fields_to_wbc(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to copy an a specific Working Buffer location
 * to a LTA field array element.
 *
 * An FA tree node which transcribes a specific Working Buffer location
 * to a LTA field structure within the WB, then appends the structure
 * to a Logical Table Adapter fields list.  The WB location is generally
 * a PTM entry or memory parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wbc_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Transform function.
 *
 * An FA tree node which passes the LTA input and output parameters
 * constructed by other FA nodes to a Logical Table Adapter Transform
 * function.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_transform(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Transform function during
 *        TRAVERSE operations.
 *
 * An FA tree node which passes the LTA input and output parameters
 * constructed by other FA nodes to a Logical Table Adapter Transform
 * function.  This node is specifically for forward key transforms
 * during TRAVERSE.  It checks if the operation is TRAVERSE_START, and
 * skips the LTA Transform if so due to the absence of any key fields
 * on the first call of a table TRAVERSE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_traverse_transform(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to convert the PT select of a LTA Transform.
 *
 * An FA tree node which retrieves the PT select index produced by
 * a LTA Transform operation where the the PT is determined by the
 * LT key fields, converts it to the PT SID required by the PTM,
 * and places it in the Working Buffer location for that PT op.
 *
 * If the PT SID is needed by multiple PT ops, then the WB to WB copy
 * node should be used to propagate the PT SID value.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_pt_sid_map(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Validate function.
 *
 * An FA tree node which passes the LTA input parameters
 * constructed by other FA nodes to a Logical Table Adapter
 * Validate function.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_validate(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);

/*!
 * \brief An FA tree node to determine the LT index.
 *
 * An FA tree node which calculates the LT index, when it is
 * different from any PT index accessed.  This is the index used
 * to track in-use and allocation.
 *
 * When the physical resources have an additional component beyond
 * just the PT index, such as per-port, per-pipe, or table instance,
 * this LT index represents the unique number which distinguishes the
 * physical resource to which the LT entry is mapped.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lt_index(int unit,
                        bcmltm_lt_md_t *lt_md,
                        bcmltm_lt_state_t *lt_state,
                        bcmltm_entry_t *lt_entry,
                        uint32_t *ltm_buffer,
                        void *node_cookie);

/*!
 * \brief An FA tree node to determine the next LT index.
 *
 * An FA tree node which finds the next in-use LT index from the
 * provided index of an existing traverse operation, or the first
 * LT index when starting a traverse.  Accounts for the extra
 * PT parameters as necessary.
 *
 * If an in-use index is found, the values for the LT index, allocation
 * index, and memory parameter are copied into the WB.  These are used
 * to perform a PT read, and copied into the API field output list of
 * the corresponding key fields.
 *
 * When the physical resources have an additional component beyond
 * just the PT index, such as per-port, per-pipe, or table instance,
 * this LT index represents the unique number which distinguishes the
 * physical resource to which the LT entry is mapped.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No further in-use index.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_traverse_index(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to determine the LT index for w/Allocation LTs.
 *
 * An FA tree node which calculates the LT index, from the input API
 * fields provided.  If the index fields are not present, a free
 * index is determined and returned at completion of the INSERT
 * operation.  This function is only applicable for Index w/Allocation
 * Logical Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lt_alloc_index(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to determine the LT index for w/Allocation LTs
 *        using shared resources.
 *
 * An FA tree node which calculates the LT index, from the input API
 * fields provided.  If the index fields are not present, a free
 * index is determined and returned at completion of the INSERT
 * operation.
 * This function is only applicable for Index w/Allocation
 * Logical Tables.
 * This function is only applicable to shared resource IwA LTs.  It
 * employs the global inuse bitmap of the master LT for this
 * shared resource.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lt_global_alloc_index(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to validate the API index key field bounds.
 *
 * An FA tree node which compares Index LT key field values to
 * the configured min/max field boundaries set in
 * TABLE_INDEX_KEY_FIELD_CONTROL.MIN/MAX.
 * This function is only applicable for Index Logical Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_index_keys_bounds(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to determine the Tracking Index.
 *
 * An FA tree node which calculates the Tracking Index from values
 * already present in the Working Buffer.
 * The Tracking Index is a combination of the device parameters
 * required to identify a unique device resource corresponding to
 * a unique set of LT API Key fields.
 *
 * The Key fields are copied or Transformed into the device physical
 * parameters.  These may include memory index, register array index,
 * register port, pipeline number, PT select, and overlay select.
 * However many there may be, the resulting Tracking Index should
 * fit within a single 32-bit value.
 *
 * The Tracking Index is the offset into the in-use,
 * global in-use, and valid entry bitmaps.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wbc_to_track(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);

/*!
 * \brief An FA tree node to decompose the Tracking Index.
 *
 * An FA tree node which separates the Tracking Index into values
 * then placed in the Working Buffer.  This reverses the process
 * in bcmltm_fa_node_wbc_to_track which assembles different
 * physical parameters into a single Tracking Index.
 *
 * This function is used for TRAVERSE where the key fields are returned
 * via the LT entry output.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_track_to_wbc(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);

/*!
 * \brief An FA tree node to conditionally decompose the Tracking Index.
 *
 * An FA tree node which conditionally separates the Tracking Index
 * into values for the allocated components and places them in the
 * Working Buffer.
 *
 * This function is used for INSERT into Index with Allocation LTs
 * where the allocatable key fields are not provided in the LT entry input.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_track_to_wbc(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An FA tree node to determine the next Tracking Index.
 *
 * An FA tree node which finds the next in-use LT Tracking Index from the
 * provided Tracking Index of an existing traverse operation, or the first
 * Tracking Index when starting a traverse.
 *
 * If an in-use Tracking Index is found, bcmltm_fa_node_track_to_wbc
 * should be employed to copy the device parameters into the proper
 * locations in the Working Buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No further in-use index.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_traverse_track(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to determine Tracking index for w/Allocation LTs.
 *
 * An FA tree node which calculates the Tracking index, from the input API
 * fields provided.  If allocatable key fields are not present, a free
 * Tracking Index is determined and returned at completion of the INSERT
 * operation.  This function is only applicable for Index w/Allocation
 * Logical Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_track(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie);

/*!
 * \brief An FA tree node to determine the Tracking Index for
 *        w/Allocation LTs using shared resources.
 *
 * An FA tree node which calculates the Tracking Index, from the input API
 * fields provided.  If allocatable key fields are not present, a free
 * Tracking Index is determined and returned at completion of the INSERT
 * operation.
 * This function is only applicable for Index w/Allocation
 * Logical Tables.
 * This function is only applicable to shared resource IwA LTs.  It
 * employs the global inuse bitmap of the master LT for this
 * shared resource.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_global_alloc_track(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An FA tree node to determine the next LT ID.
 *
 * An FA tree node which finds the next LT ID from the
 * provided LT ID of an existing traverse operation on one of the
 * TABLE_* LTs which use the LT ID itself as the key.  Will determine
 * the first LT ID when starting a traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No further LT ID.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_traverse_ltid(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

#endif /* BCMLTM_FA_TREE_NODES_INTERNAL_H */
