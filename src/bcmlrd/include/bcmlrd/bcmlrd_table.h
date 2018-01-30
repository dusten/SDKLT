/*! \file bcmlrd_table.h
 *
 * \brief Logical Table Resource Database
 *
 * Logical Table data
 *
 * Data structures are roughly modeled on CDK data. Biggest departure
 * is likely to be the use of "symbol local" field IDs which index
 * into the field structure directly.  This is intended to be used by
 * generated code.
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

#ifndef BCMLRD_TABLE_H
#define BCMLRD_TABLE_H

#include <sal/sal_types.h>
#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief Get a count of all table IDs for a unit.
 *
 * Get a count of all logical table IDs for a given unit.
 *
 * \param [in]  unit            Unit number.
 * \param [out] num_sid         Number of table IDs.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_count_get(int unit, size_t *num_sid);

/*!
 * \brief Get all table IDs for a table.
 *
 * Get a list of all table IDs for a given unit.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  list_max        Table IDs allocated in sid_list.
 * \param [out] sid_list        List of table IDs.
 * \param [out] num_sid         Number of table IDs returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_list_get(int unit, size_t list_max,
                      bcmlrd_sid_t *sid_list, size_t *num_sid);

/*!
 * \brief Get the table size for a logical table.
 *
 * Get the table size for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] size            Size.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_size_get(int unit, bcmlrd_sid_t sid, uint32_t *size);

/*!
 * \brief Get the maximum index for a logical table.
 *
 * Get the maximum index for a logical table.
 *
 * What happens if this is inappropriate for the table? Error?
 * Is this truly a fixed property, or can this be changed on
 * a per-unit basis?
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] max_index       Maximum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_max_index_get(int unit, bcmlrd_sid_t sid, uint32_t *max_index);

/*!
 * \brief Get the minimum index for a logical table.
 *
 * Get the minimum index for a logical table.
 *
 * What happens if this is inappropriate for the table? Error?
 * Is this truly a fixed property, or can this be changed on
 * a per-unit basis?
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] min_index       Minimum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_min_index_get(int unit, bcmlrd_sid_t sid, uint32_t *min_index);

/*!
 * \brief Get the port index range size for a logical table.
 *
 * Get the port index range size for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] size            Size.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_port_size_get(int unit, bcmlrd_sid_t sid, uint32_t *size);

/*!
 * \brief Get the maximum port index for a logical table.
 *
 * Get the maximum port index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] max_index       Maximum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_port_max_index_get(int unit, bcmlrd_sid_t sid, uint32_t *max_index);

/*!
 * \brief Get the minimum port index for a logical table.
 *
 * Get the minimum port index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] min_index       Minimum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_port_min_index_get(int unit, bcmlrd_sid_t sid, uint32_t *min_index);

/*!
 * \brief Get the instance index range size for a logical table.
 *
 * Get the instance index range size for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] size            Size.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_instance_size_get(int unit, bcmlrd_sid_t sid, uint32_t *size);

/*!
 * \brief Get the maximum instance index for a logical table.
 *
 * Get the maximum instance index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] max_index       Maximum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_instance_max_index_get(int unit, bcmlrd_sid_t sid, uint32_t *max_index);

/*!
 * \brief Get the minimum instance index for a logical table.
 *
 * Get the minimum instance index for a logical table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] min_index       Minimum index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_instance_min_index_get(int unit, bcmlrd_sid_t sid, uint32_t *min_index);


/*!
 * \brief Get a count of all field IDs for a unit and table ID
 *
 * Get a count of fields for a given table ID
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] num_fid         Number of field IDs in table.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_count_get(int unit, bcmlrd_sid_t sid, size_t *num_fid);

/*!
 * \brief Get all field IDs for a table.
 *
 * Get a list of all table IDs for a given unit.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [in]  list_max        Number of fields IDs allocated.
 * \param [out] fid_list        List of field IDs.
 * \param [out] num_fid         Number of field IDs returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_list_get(int unit, bcmlrd_sid_t sid, size_t list_max,
                      bcmlrd_fid_t *fid_list, size_t *num_fid);

/*!
 * \brief Get table info for a given table.
 *
 * \param [in ] sid             Table IDs.
 *
 * \retval non-NULL     Table definition.
 * \retval NULL         Illegal Table ID.
 */
extern const bcmlrd_table_rep_t *
bcmlrd_table_get(bcmlrd_sid_t sid);

/*!
 * \brief Initialize the Logical Resource Database.
 *
 * Initialize the Logical Resource Database.
 *
 * Must be called first before any other LRD calls.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_init(void);

/*!
 * \brief Clean up the Logical Resource Database.
 *
 * Release any resources allocated by the LRD. No other LRD calls
 * other than bcmlrd_init can be called after this.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_cleanup(void);

/*!
 * \brief Get the number of field indexes for given field width.
 *
 * Return the number of field indexes required for given field width.
 *
 * \param [in] width Field width in bits.
 *
 * \retval Number of field indexes.
 */
uint32_t
bcmlrd_width_field_idx_count_get(uint32_t width);

/*!
 * \brief Get the number of field indexes for given field ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [in]  fid             Field ID.
 * \param [out] idx_count       Number of field indexes.
 *
 * \retval idx_count Number of field indexes.
 */
extern uint32_t
bcmlrd_field_idx_count_get(int unit, bcmlrd_sid_t sid, bcmlrd_fid_t fid);

/*!
 * \brief Get field def information.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [in]  fid             Field ID.
 * \param [out] field_def       Field Def data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_field_def_get(int unit,
                           const bcmlrd_sid_t sid,
                           const bcmlrd_fid_t fid,
                           bcmlrd_field_def_t *field_def);

/*!
 * \brief Get field def information by given table field data
 *        and map field data.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  fid             Field ID.
 * \param [in]  tbl_field       Table Field data.
 * \param [in]  map_field       Map Field data.
 * \param [out] field_def       Field Def data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_def_get(int unit,
                     const bcmlrd_fid_t field_id,
                     const bcmlrd_table_rep_t *tbl,
                     const bcmlrd_map_t *map,
                     bcmlrd_field_def_t *field_def);

/*!
 * \brief Get the interactive flag for a table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 * \param [out] interactive     Interactive flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_interactive_get(int unit, uint32_t sid, bool *interactive);

/*!
 * \brief Get field default data
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_def       Pointer to field default data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
int
bcmlrd_field_default_get(int unit,
                         const bcmlrd_sid_t sid,
                         const bcmlrd_fid_t fid,
                         const uint32_t num_alloc,
                         uint64_t* field_def,
                         uint32_t* num_actual);

/*!
 * \brief Get field max data
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_max       Pointer to field max data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
int
bcmlrd_field_max_get(int unit,
                     const bcmlrd_sid_t sid,
                     const bcmlrd_fid_t fid,
                     const uint32_t num_alloc,
                     uint64_t* field_def,
                     uint32_t* num_actual);

/*!
 * \brief Get field min data
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Source Table ID.
 * \param [in]  fid             Field ID.
 * \param [in]  num_alloc       Number of elements allocated.
 * \param [out] field_min       Pointer to field min data.
 * \param [out] num_actual      Number of actual elements.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
int
bcmlrd_field_min_get(int unit,
                     const bcmlrd_sid_t sid,
                     const bcmlrd_fid_t fid,
                     const uint32_t num_alloc,
                     uint64_t* field_def,
                     uint32_t* num_actual);

/*!
 * \brief Function to check if the logical field in unmapped.
 *
 * This routine checks if the logical field is unmapped
 * for the given unit, sid, and fid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical Table symbol ID.
 * \param [in]  fid             Logical Field ID.
 * \param [out] unmapped        Logical field MAP status. TRUE for UNMAPPED.
 *
 * \retval SHR_E_NONE           field MAP status is valid.
 * \retval SHR_E_UNAVAIL        unit/table/field not found.
 */
extern int bcmlrd_field_is_unmapped(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmlrd_fid_t fid,
                                    bool *unmapped);

#endif /* BCMLRD_TABLE_H */
