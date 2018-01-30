/*! \file bcmltm_wb_ptm_internal.h
 *
 * Logical Table Manager Working Buffer Definitions for PTM interface.
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

#ifndef BCMLTM_WB_PTM_INTERNAL_H
#define BCMLTM_WB_PTM_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmptm/bcmptm_types.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

/*!
 * \brief PTM Parameter Offsets.
 *
 * This structure contains the working buffer offsets for the
 * different memory parameters used on PTM operations.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_param_offset_s {
    /*! Offset for the INDEX parameter. */
    uint32_t index;

    /*! Offset for the INDEX_NUM parameter. */
    uint32_t index_num;

    /*! Offset for the PORT parameter. */
    uint32_t port;

    /*! Offset for the TABLE_INST parameter. */
    uint32_t table_inst;

    /*! Offset for the PRIORITY parameter. */
    uint32_t priority;

    /*! Offset for the BANK parameter. */
    uint32_t bank;

    /*! Offset for the ACCTYPE parameter. */
    uint32_t acctype;

    /*! Offset for the PDD bits parameter. */
    uint32_t pdd_bits;

    /*! Offset for the PT SID override parameter. */
    uint32_t pt_sid_override;

    /*! Offset for the PT SID selector parameter. */
    uint32_t pt_sid_selector;

    /*! Offset for the PT entry Changed status. */
    uint32_t pt_changed;
} bcmltm_wb_ptm_param_offset_t;


/*!
 * \brief PTM Entry Offsets for Index PT.
 *
 * This structure contains the working buffer offsets for the
 * entry buffer used for PTM operations on Index physical tables.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_entry_index_offset_s {
    /*! Offset for the entry data for Indexed physical tables. */
    uint32_t data;
} bcmltm_wb_ptm_entry_index_offset_t;


/*!
 * \brief PTM Entry Offsets for Keyed PT Pass Thru.
 *
 * This structure contains the working buffer offsets for the
 * entry buffers used for PTM operations on Keyed PT Pass Thru (Hash/TCAM).
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_entry_keyed_pthru_offset_s {
    /*! Offsets for the KEY entry for Keyed tables (TCAM, Hash). */
    uint32_t key;

    /*! Offsets for the DATA entry for Keyed tables (TCAM, Hash). */
    uint32_t data;
} bcmltm_wb_ptm_entry_keyed_pthru_offset_t;


/*!
 * Presently, the total number of entry buffers required for a
 * LT keyed table operation is 3 sets and are used as follows
 * for the different PTM operations:
 *
 *                 LOOKUP/TRAVERSE    INSERT/DELETE
 *    Input Key          K0                 K0
 *    Input Data         D0                 D1
 *    Output Key         K1                 K2
 *    Output Data        D1                 D2
 *
 * The PTM operations for TRAVERSE are GET_FIRST and GET_NEXT.
 *
 * Note that there is not an explicit PTM UPDATE operation,
 * The LT UPDATE translates to a PTM LOOKUP and INSERT.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_NUM 3

/*!
 * The first set of PTM entries are for the LOOKUP/TRAVERSE input (search key).
 * For INSERT and DELETE ops, this key set is used.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_0 0

/*!
 * The second set of PTM entries are for the LOOKUP/TRAVERSE output.
 * For INSERT operations, this data set is used.
 * For DELETE, there may be reference counts to adjust.
 * Since an LT UPDATE results in a PTM INSERT operation, this data set
 * will be modified.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_1 1

/*!
 * The third set of PTM entries are for the INSERT/DELETE opcode output.
 * While this output is not expected to be relevant, since the
 * lookup already determined the previous entry to be replaced,
 * some future use case may require this output for some analysis.
 * Space is reserved to avoid overwriting the other two entry buffers.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_2 2


/*!
 * \brief PTM Entry Offsets for Keyed LT.
 *
 * This structure contains the working buffer offsets for the
 * entry buffers used for PTM operations on Keyed LT (Hash/TCAM).
 *
 * Offsets are returned based on the three copy keyed table model
 * described for the BCMLTM_WB_PTM_KEYED_ENTRY_SET_* flags.
 * Each set contains 4 PT entries.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_entry_keyed_lt_offset_s {
    /*! Offsets for the KEY entries for Keyed tables (TCAM, Hash). */
    uint32_t key[BCMLTM_WB_PTM_KEYED_ENTRY_SET_NUM][BCMLTM_PT_ENTRY_LIST_MAX];

    /*! Offsets for the DATA entries for Keyed tables (TCAM, Hash). */
    uint32_t data[BCMLTM_WB_PTM_KEYED_ENTRY_SET_NUM][BCMLTM_PT_ENTRY_LIST_MAX];
} bcmltm_wb_ptm_entry_keyed_lt_offset_t;


/*!
 * \brief Get the PTM operation flags BCMLTM_PT_OP_FLAGS_ for given PT.
 *
 * This routine returns the PTM operation flags BCMLTM_PT_OP_FLAGS_
 * corresponding to given PT.  These flags indicate the
 * parameters and entries required for PTM operations.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol (Physical Table) ID.
 *
 * \retval Working Buffer flags BCMLTM_PT_OP_FLAGS_
 */
extern uint32_t
bcmltm_wb_ptm_op_flags_get(int unit, bcmdrd_sid_t sid);


/*!
 * \brief Add a PTM working buffer block for a PT Pass Thru table.
 *
 * This function adds a working buffer block required for PTM operation
 * for a PT Pass Thru table.  The corresponding PTM block (index or
 * keyed) is added based on the PT flags provided.
 *
 * \param [in] unit Unit number.
 * \param [in] ptid Physical table ID.
 * \param [in] flags PT operation BCMLTM_PT_OP_FLAGS_ for table.
 * \param [in] num_ops Number of PTM operations required for this table.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_ptm_pthru_add(int unit,
                              bcmdrd_sid_t ptid,
                              uint32_t flags, uint32_t num_ops,
                              bcmltm_wb_handle_t *handle,
                              bcmltm_wb_block_id_t *block_id,
                              bcmltm_wb_block_t **block_ptr);

/*!
 * \brief Add a PTM working buffer block for a Logical Table.
 *
 * This function adds a working buffer block required for PTM operation
 * for a LT table.  The corresponding PTM block (index or
 * keyed) is added based on the LT type provided.
 *
 * \param [in] unit Unit number.
 * \param [in] ptid Physical table ID.
 * \param [in] lt_type Table type.
 * \param [in] num_ops Number of PTM operations required for this table.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_ptm_lt_add(int unit,
                           bcmdrd_sid_t ptid,
                           bcmltm_table_type_t lt_type,
                           uint32_t num_ops,
                           bcmltm_wb_handle_t *handle,
                           bcmltm_wb_block_id_t *block_id,
                           bcmltm_wb_block_t **block_ptr);

/*!
 * \brief Get size of one PT entry element allocated in the working buffer.
 *
 * This function gets the size allocated in the working buffer for
 * one PT entry element in the PTM Entry section.
 *
 * Note that depending on the table type, the PTM entry section
 * can contain several PT entries.  For instance, the PTM entry
 * section for Keyed LTs has 6 PT entries (3 sets for each Key and Data
 * buffers).
 *
 * \retval Size, in words, of a PT entry element.
 */
extern uint32_t
bcmltm_wb_ptm_pt_entry_wsize(void);


/*!
 * \brief Get working buffer PTM parameter base offset for given PT op.
 *
 * This function gets the PTM parameter section base offset in the
 * working buffer for the given PTM operation index.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] op_idx PTM operation index.
 * \param [out] offset Returning PTM parameter base offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_param_base_offset_get(const bcmltm_wb_block_t *wb_block,
                                    uint32_t op_idx,
                                    uint32_t *offset);

/*!
 * \brief Get working buffer PTM entry base offset for given PT op.
 *
 * This function gets the PTM entry section base offset in the
 * working buffer for the given PTM operation index.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] op_idx PTM operation index.
 * \param [out] offset Returning PTM entry base offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_entry_base_offset_get(const bcmltm_wb_block_t *wb_block,
                                    uint32_t op_idx,
                                    uint32_t *offset);

/*!
 * \brief Get working buffer PTM parameter offsets.
 *
 * This function gets the PTM parameters working buffer offsets
 * for the given base offset.
 *
 * \param [in] base_offset Base offset of parameter section.
 * \param [out] offset Returning PTM parameter offsets.
 *
 * \retval Size, in words, of PTM parameter section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_param_offset_get(uint32_t base_offset,
                               bcmltm_wb_ptm_param_offset_t *offset);

/*!
 * \brief Get working buffer PTM entry offsets for Index table.
 *
 * This function gets the PTM entry buffer offsets on an Index table
 * for the given base offset.
 *
 * \param [in] base_offset Base offset of entry section.
 * \param [out] offset Returning PTM entry offsets.
 *
 * \retval Size, in words, of PTM entry section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_entry_index_offset_get(uint32_t base_offset,
                                     bcmltm_wb_ptm_entry_index_offset_t
                                     *offset);

/*!
 * \brief Get working buffer PTM entry offsets for Keyed PT Pass Thru table.
 *
 * This function gets the PTM entry buffer offsets on a PT Pass Thru Keyed
 * table for the given base offset.
 *
 * \param [in] base_offset Base offset of entry section.
 * \param [out] offset Returning PTM entry offsets.
 *
 * \retval Size, in words, of PTM entry section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_entry_keyed_pthru_offset_get(uint32_t base_offset,
                          bcmltm_wb_ptm_entry_keyed_pthru_offset_t *offset);

/*!
 * \brief Get working buffer PTM entry offsets for Keyed LT.
 *
 * This function gets the PTM entry buffer offsets on a LT Keyed
 * table for the given base offset.
 *
 * \param [in] base_offset Base offset of entry section.
 * \param [out] offset Returning PTM entry offsets.
 *
 * \retval Size, in words, of PTM entry section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_entry_keyed_lt_offset_get(uint32_t base_offset,
                           bcmltm_wb_ptm_entry_keyed_lt_offset_t *offset);

/*!
 * \brief Get working buffer offset for given field for PT Pass Thru.
 *
 * This function gets the working buffer offset for the specified
 * PT field ID in given PTM working buffer block on PT Pass Thru.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] fid Physical field ID.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_field_pthru_offset_get(const bcmltm_wb_block_t *wb_block,
                                     bcmdrd_fid_t fid,
                                     uint32_t *offset);

/*!
 * \brief Get working buffer offset for given field for Logical Table.
 *
 * This function gets the working buffer offset for the specified
 * PT field ID in given PTM working buffer block on logical tables.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] fid Physical field ID.
 * \param [in] field_type Indicates key or value field map type.
 *                        This is only relevant for the entry section of
 *                        a Keyed PTM block.
 * \param [in] field_dir Indicates input or output field direction,
 *                       This is only relevant for the entry section of
 *                       a Keyed PTM block on Key fields.
 * \param [in] pt_entry_idx Keyed PT entry index.
 *                          This is only relevant for the entry section of
 *                          a Keyed PTM block.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_field_lt_offset_get(const bcmltm_wb_block_t *wb_block,
                                  uint32_t fid,
                                  bcmltm_field_type_t field_type,
                                  bcmltm_field_dir_t field_dir,
                                  uint16_t pt_entry_idx,
                                  uint32_t *offset);

/*!
 * \brief Get working buffer offset for PT SID override.
 *
 * This function gets the working buffer offset for the PT SID override
 * parameter in given PTM working buffer block.
 *
 * \param [in] wb_block Working buffer block.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_pt_sid_override_offset_get(const bcmltm_wb_block_t *wb_block,
                                         uint32_t *offset);

/*!
 * \brief Get working buffer offset for PT Entry Changed status.
 *
 * This function gets the working buffer offset for the PT Entry Changed
 * status parameter in given PTM working buffer block for
 * the given table type and field type.
 *
 * The PT changed status is not applicable for all table types or
 * field types (Key/Value).  If the parameter is not applicable,
 * the returning offset is set to INVALID.
 *
 * \param [in] table_type Table type.
 * \param [in] field_type Indicates key or value field type.
 * \param [in] wb_block Working buffer block.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_pt_changed_offset_get(bcmltm_table_type_t table_type,
                                    bcmltm_field_type_t field_type,
                                    const bcmltm_wb_block_t *wb_block,
                                    uint32_t *offset);

/*!
 * \brief Indicate if PT Entry Changed flag is valid for the table type.
 *
 * This function checks if the PT Entry Changed opcode flag
 * BCMLTM_PT_OP_FLAGS_PT_CHANGED is applicable for the given table type.
 *
 * \param [in] type Table type.
 *
 * \retval TRUE if flag is applicable.
 * \retval FALSE if flag is not applicable.
 */
extern bool
bcmltm_wb_ptm_pt_changed_flag_valid(bcmltm_table_type_t type);

/*!
 * \brief Get the PTM entry key set index for given PTM operation and field.
 *
 * This function gets the index for the corresponding PTM Keyed entry
 * set assigned to the given PTM operation and field type,
 * Key/Data, Input/Output.
 *
 * \param [in] ptm_op PTM operation.
 * \param [in] field_type Indicates key or value field map type.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed PTM block on Key fields).
 * \param [out] set_idx Returning index for PTM keyed set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_entry_keyed_idx_get(bcmptm_op_type_t ptm_op,
                                  bcmltm_field_type_t field_type,
                                  bcmltm_field_dir_t field_dir,
                                  uint32_t *set_idx);

#endif /* BCMLTM_WB_PTM_INTERNAL_H */
