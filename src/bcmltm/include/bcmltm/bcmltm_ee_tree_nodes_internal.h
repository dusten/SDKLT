/*! \file bcmltm_ee_tree_nodes_internal.h
 *
 * Logical Table Manager Binary Trees.
 * Execution Engine Nodes.
 *
 * A set of functions and definitions forming the nodes of
 * LTM EE binary trees.
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

#ifndef BCMLTM_EE_TREE_NODES_INTERNAL_H
#define BCMLTM_EE_TREE_NODES_INTERNAL_H

#include <bcmbd/bcmbd.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>

/*!
 * Indexed and PT PassThru table Metadata from node cookie
 */

/*!
 * \brief Number of Physical Tables.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_PT_LIST_NUM(_nc_) \
    (((bcmltm_pt_list_t *)(_nc_))->num_pt_view)


/*!
 * \brief Retrieve pointer to Memory Arguments structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 * \param [in] \_view\_ Index number of PT, in range 0...(num_pt_view -1).
 */
#define BCMLTM_PT_LIST_MEM_ARGS(_nc_, _view_) \
    ((((bcmltm_pt_list_t *)(_nc_))->mem_args)[(_view_)])

/*!
 * \brief Retrieve pointer to Memory Operations structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 * \param [in] \_view\_ Index number of PT, in range 0...(num_pt_view -1).
 */
#define BCMLTM_PT_LIST_OPS_LIST(_nc_, _view_) \
    ((((bcmltm_pt_list_t *)(_nc_))->memop_lists)[(_view_)])


/*!
 * Keyed table Metadata from node cookie
 */

/*!
 * \brief Retrieve pointer to Memory Arguments structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_KEYED_OP_INFO(_nc_) ((bcmltm_pt_keyed_op_info_t *)(_nc_))


/*!
 * \brief Retrieve pointer to EE index information from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_EE_INDEX_INFO(_nc_) \
    ((bcmltm_ee_index_info_t *)(_nc_))


/*!
 * \brief Extract field spec list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_EE_FIELD_SPEC_LIST(_nc_) \
    ((bcmltm_field_spec_list_t *)(_nc_))


/*!
 * LTA special handler table Metadata from node cookie
 */

/*!
 * \brief Retrieve pointer to LTA table parameters structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_LTA_TABLE_PARAMS(_nc_) \
    ((bcmltm_lta_table_params_t *)(_nc_))

/*!
 * \brief The traverse cookie for EE stage operations.
 */
typedef struct bcmltm_ee_traverse_info_s {
    /*! Pointer to the metadata for this LT. */
    bcmltm_lt_md_t *lt_md;

    /*! Pointer to the runtime state for this LT. */
    bcmltm_lt_state_t *lt_state;

    /*! Pointer to the working buffer for this entry. */
    uint32_t *ltm_buffer;
} bcmltm_ee_traverse_info_t;


/**
 * EE node support functions
 */

/*!
 * \brief Convert parameter offset list and LTM buffer to PTM dynamic info
 *
 * This function extracts the PTM parameters from the Working Buffer
 * based on the parsed parameter offsets and returns the information
 * in the dynamic info structure to be used by the PTM operation.
 *
 * \param [in] pt_flags BCMLTM_PT_OP_FLAGS_* to parse parameters.
 * \param [in] offset The processed set of LTM buffer offsets that
 *                    indicate the location of each memory paramter.
 * \param [in] ltm_buffer Pointer to the LTM Working Buffer for this op.
 * \param [out] dyn_info The PTM dynamic information structure to be
 *                     filled out with the provided memory parameters.
 *
 * \retval None.
 */
extern void
bcmltm_pt_dyn_info_get(uint32_t pt_flags,
                       bcmltm_wb_ptm_param_offset_t *offset,
                       uint32_t *ltm_buffer,
                       bcmbd_pt_dyn_info_t *dyn_info);

/**
 * Basic EE node operations
 * - Tree traversal invocation
 * - Individual node processing
 */

/*!
 * \brief Invoke a single EE stage tree for traversal.
 *
 * Provide the root node, node traversal callback function, and tree
 * cookie to the general binary tree traversal function in order to
 * conduct an Execution Engine stage of the LTM.
 *
 * \param [in] unit Unit number.
 * \param [in] root_node The root node of the binary tree to traverse.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_stage(int unit,
                bcmltm_node_t *root_node,
                bcmltm_lt_md_t *lt_md,
                bcmltm_lt_state_t *lt_state,
                bcmltm_entry_t *lt_entry,
                uint32_t *ltm_buffer);

/*!
 * \brief Invoke a EE stage tree node.
 *
 * Using the generic tree node information of an EE stage tree, perform the
 * encoded node operation to accomplish an EE task.
 *
 * Note - this is the traverse function which transforms a generic LTM tree
 * into an EE stage tree.
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
bcmltm_ee_node(int unit, void *node_data, void *traverse_cookie);


/*******************************************************************************
 * PT PassThru functions
 */

/*!
 * \brief An EE tree node to perform SCHAN write operations with
 *        memory parameter flags.
 *
 * An EE tree node which dispatches a set of SCHAN writes to the PTM
 * using the entries already assembled in the LTM working buffer.
 *
 * This version of schan_write can handle different PT parameters in the
 * Working Buffer list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_schan_write(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie);

/*!
 * \brief An EE tree node to perform SCHAN read operations with
 *        memory parameter flags.
 *
 * An EE tree node which dispatches a set of SCHAN reads to the PTM
 * using the indexes listed in the LTM working buffer.  The read data
 * is stored in the LTM working buffer.
 *
 * This version of schan_read can handle different PT parameters in the
 * Working Buffer list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in,out] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_schan_read(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*******************************************************************************
 * PT hash table functions
 */

/*!
 * \brief An EE tree node to insert a hash table entry for the
 * INSERT  commands.
 *
 * An EE tree node which presents the separated key and data PT entry
 * arrays to the PTM for a PT insert operation.  This PT op may be used
 * as part of the INSERT opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_insert(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node to delete a hash table entry for the
 * DELETE command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT delete operation.  This PT op is used as part of the
 * DELETE opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_delete(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node to retrieve a hash table entry for the
 * LOOKUP or UPDATE commands.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This PT op is used as part of the
 * LOOKUP opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_lookup(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node to serach for an existing hash table entry
 * for the INSERT command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This node expects the PT lookup
 * to return a not found status.  If the lookup succeeds, the status
 * is converted to an exists error.
 * This PT op is used as part of the INSERT opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No matching entry exists.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_lookup_not_found(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie);

/*******************************************************************************
 * LT Modeled functions
 */

/*******************************************************************************
 * PT PassThru functions
 */

/*!
 * \brief An EE tree node to perform modeled index write operations.
 *
 * An EE tree node which dispatches a set of modeled index writes
 * to the PTM using the entries already assembled in the LTM
 * working buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_write(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An EE tree node to perform modeled index read operations.
 *
 * An EE tree node which dispatches a set of modeled index reads to the PTM
 * using the indexes listed in the LTM working buffer.  The read data
 * is stored in the LTM working buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in,out] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_read(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*******************************************************************************
 * LT simple index table functions
 */

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * simple index table on an INSERT operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is available for a new modeled LT entry to be placed into a simple
 * index managed table.  The insert is rejected if an entry already
 * exists at this index.  Update should be used instead.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_insert(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * simple index table on a DELETE operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is in use with an entry in a simple index managed table.
 * The delete fails if no entry already exists at this index.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_delete(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to validate the index usage of a
 * simple index table on an UPDATE or LOOKUP operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is in use with an entry in a simple index managed table.
 * Fails if no entry already exists at this index.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_exists(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * shared resource simple index table on an INSERT operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is available for a new modeled LT entry to be placed into a shared
 * resource simple index managed table.  The insert is rejected if an
 * entry already exists for this index in any of the LTs sharing this
 * device resource.
 *
 * This node updates the global resource in-use bitmap maintained by
 * the master LT of the resource group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_global_index_insert(int unit,
                                   bcmltm_lt_md_t *lt_md,
                                   bcmltm_lt_state_t *lt_state,
                                   bcmltm_entry_t *lt_entry,
                                   uint32_t *ltm_buffer,
                                   void *node_cookie);

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * shared resource simple index table on a DELETE operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is in use by an entry in a shared resource simple index managed table.
 * The delete fails if no entry already exists at this index.
 *
 * This node updates the global resource in-use bitmap maintained by
 * the master LT of the resource group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_global_index_delete(int unit,
                                   bcmltm_lt_md_t *lt_md,
                                   bcmltm_lt_state_t *lt_state,
                                   bcmltm_entry_t *lt_entry,
                                   uint32_t *ltm_buffer,
                                   void *node_cookie);


/*******************************************************************************
 * LT keyed table functions
 */

/*!
 * \brief An EE tree node to insert a keyed table entry for the
 * INSERT or UPDATE commands.
 *
 * An EE tree node which presents the separated key and data PT entry
 * arrays to the PTM for a PT insert operation.  This PT op may be used
 * as part of the INSERT or UPDATE opcodes.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_insert(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to delete a keyed table entry for the
 * DELETE command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT delete operation.  This PT op is used as part of the
 * DELETE opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_delete(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to retrieve a keyed table entry for the
 * LOOKUP or UPDATE commands.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This PT op is used as part of the
 * LOOKUP or UPDATE opcodes.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_lookup(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to serach for an existing hash table entry
 * for the INSERT command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This node expects the PT lookup
 * to return a not found status.  If the lookup succeeds, the status
 * is converted to an exists error.
 * This PT op is used as part of the INSERT opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_lookup_not_found(int unit,
                                            bcmltm_lt_md_t *lt_md,
                                            bcmltm_lt_state_t *lt_state,
                                            bcmltm_entry_t *lt_entry,
                                            uint32_t *ltm_buffer,
                                            void *node_cookie);

/*!
 * \brief An EE tree node to fetch the next keyed table entry for the
 * TRAVERSE command.
 *
 * An EE tree node which presents either the key PT entry arrays
 * to the PTM or a no key entry for a PT GET_NEXT or GET_FIRST
 * operation.  This PT op is used as part of the TRAVERSE opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_traverse(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);



/*******************************************************************************
 * LTA dispatch nodes
 */

/*!
 * \brief An EE tree node wrapper for the LTA custom table insert interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_insert(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table alloc insert intf.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface insert with allocation operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_alloc_insert(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table lookup interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface lookup operation.
 * The returned fields are appended to the LTM entry output fields list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_lookup(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table delete interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_delete(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table update interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface update operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_update(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table traverse interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface traverse operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_traverse(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);


/*******************************************************************************
 * LT statistics nodes
 */

/*!
 * \brief An EE tree node wrapper for the LT TABLE_* writes.
 *
 * An EE tree node that updates LT TABLE_* values and stores
 * the values in an HA-protected resource.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lt_table_write(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LT TABLE_* read.
 *
 * An EE tree node that retrieves LT TABLE_* values from
 * an HA-protected resource.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lt_table_read(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

#endif /* BCMLTM_EE_TREE_NODES_INTERNAL_H */
