/*! \file bcmimm_backend.h
 *
 *  In-memory back-end header file.
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

#ifndef BCMIMM_BACKEND_H
#define BCMIMM_BACKEND_H

#include <sal/sal_types.h>

/*!
 * \brief In-Memory backend field ID size in bytes
 */
#define BCMIMM_BE_FID_SIZE  sizeof(uint32_t)

/*!
 * \brief In-Memory backend table handle type.
 */
typedef uint32_t bcmimm_be_tbl_hdl_t;

/*!
 * \brief Entry fields set.
 */
typedef struct bcmimm_be_fields_s {
    size_t count;   /*!< Number of fields */
    uint32_t *fid;  /*!< Array of size count for field IDs */
    /*!
     * Array of size count of field data. Each array element is of size given
     * to \ref bcmimm_be_table_create()
     */
    void *fdata;
} bcmimm_be_fields_t;

/*!
 * \brief Entry fields update set.
 */
typedef struct bcmimm_be_fields_upd_s {
    /*! Number of fields */
    size_t count;
    /*! Array of size count for field IDs */
    uint32_t *fid;
    /*! Array of size count of field operations */
    bool *del;
    /*!
     * Array of size count of field data. Each array element is of size given
     * to \ref bcmimm_be_table_create()
     */
    void *fdata;
} bcmimm_be_fields_upd_t;

/*!
 * \brief Initializes single back-end unit.
 *
 * This function Initializes the back-end tables associated with the unit.
 * If the warm parameter is true, it recovers the tables state by allocating
 * HA memory blocks.
 *
 * \param [in] unit This is the unit to initialize the back-end.
 * \param [in] comp_id This is the component ID to use (for HA).
 * \param [in] warm Indicates if the initialization is warm (where states
 * should be recovered from HA memory) or cold.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_be_init(int unit, uint8_t comp_id, bool warm);

/*!
 * \brief Closes back-end tables for the given unit.
 *
 * This function is the inverse of \ref bcmimm_be_init(). It frees all
 * previously allocated resources associated with this unit.
 *
 * \param [in] unit This is the unit to close the back-end.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_be_cleanup(int unit);

/*!
 * \brief Create back-end table.
 *
 * This function creates a back-end table. The table is associated with the
 * specified unit and component. The back end will allocate HA blocks using
 * the component ID and the sub component range from \c sub_comp_min to
 * \c sub_comp_max.
 *
 * \param [in] unit This is the unit to associate the table with.
 * \param [in] comp_id This is the component ID to use when allocating HA
 * block.
 * \param [in] sub_id This is the sub component id to use when
 * allocating HA block.
 * \param [in] key_size This is the size in bytes of the key portion. This
 * size is the total key size (meaning that the key is of a fixed size).
 * \param [in] data_size This is the size in bytes of a single data element
 * (field). Note that the size of the data element is fixed for all fields
 * in this table. The 'data' in this context means the following:
 *  - field data: (mandatory) that is the actual data attribute of the field
 *  - field idx: (optional) for tables with field array this indicates the
 *    index of the field in the array.
 * \param [in] rows This is the expected number of rows in the table.
 * \param [in] warm Indicate if it is cold or warm start.
 * \param [out] hdl This is the table handle to use for any other reference
 * to this table.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_create(int unit,
                                  uint8_t comp_id,
                                  uint8_t sub_id,
                                  size_t key_size,
                                  size_t data_size,
                                  size_t rows,
                                  bool warm,
                                  bcmimm_be_tbl_hdl_t *hdl);

/*!
 * \brief Destroy back-end table.
 *
 * This function closes a back-end table and free all its resources.
 * The table must be open first with a call to \ref bcmimm_be_table_create().
 *
 * \param [in] hdl This is the table handle.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_release(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief The number of entries in the table
 *
 * This function returns the number of entries currently available inside
 * the table specified via \c hdl.
 *
 * \param [in] hdl This is the table handle.
 *
 * \return the number of elements in the table. Return of 0 indicates either
 * table is empty or that the handle is invalid.
 */
extern size_t bcmimm_be_table_size_get(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Get the first entry of the table.
 *
 * This function enumerates the entries of the table specified via \c hdl and
 * returns the first entry of that table. If another context was in a process
 * of enumerating the entries and getting them one by one then this function
 * will interrupt that process and will reset it back to the first entry.
 *
 * \param [in] hdl This is the table handle.
 * \param [out] key This is a pointer to store the key value. The size of
 * the key memory must be at least as it was provided during
 * \ref bcmimm_be_table_create().
 * \param [out] out This is the structure that contains the content of the
 * first entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_get_first(bcmimm_be_tbl_hdl_t hdl,
                                     void *key,
                                     bcmimm_be_fields_t *out);

/*!
 * \brief Get the next entry of the table.
 *
 * This function returns the next entry of the table specified via \c hdl. The
 * next entry is the following entry from the previous call to
 * \ref bcmimm_be_table_get_first() or \ref bcmimm_be_table_get_next().
 *
 * \param [in] hdl This is the table handle.
 * \param [out] key This is a pointer to store the key value. The size of
 * the key memory must be at least as it was provided during
 * \ref bcmimm_be_table_create().
 * \param [out] out This is the structure that contains the content of the
 * next entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_get_next(bcmimm_be_tbl_hdl_t hdl,
                                    void *key,
                                    bcmimm_be_fields_t *out);

/*!
 * \brief Get the first entry of the table using context.
 *
 * This function enumerates the entries of the table specified via \c hdl and
 * returns the first entry of that table. Unlike the function
 * \ref bcmimm_be_table_get_first that will reset the enumeration of the table
 * this function simply gets the first element of the table. Another thread may
 * traverse this table without impacting the output of the first thread.
 *
 * \param [in] hdl This is the table handle.
 * \param [out] key This is a pointer to store the key value. The size of
 * the key memory must be at least as it was provided during
 * \ref bcmimm_be_table_create().
 * \param [out] out This is the structure that contains the content of the
 * first entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_ctx_get_first(bcmimm_be_tbl_hdl_t hdl,
                                         void *key,
                                         bcmimm_be_fields_t *out);

/*!
 * \brief Get the next entry of the table based on context.
 *
 * This function returns the next entry of the table specified via \c hdl. The
 * next entry is the entry followed by the entry that matches the input key.
 * Using the input key as context allows multiple threads to simultanously
 * traverse the same table.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] in_key This is the key of the previous entry that was traversed.
 * \param [out] out_key This is a pointer to store the key value. The size of
 * the key memory must be at least as it was provided during
 * \ref bcmimm_be_table_create().
 * \param [out] out This is the structure that contains the content of the
 * next entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_ctx_get_next(bcmimm_be_tbl_hdl_t hdl,
                                        void *in_key,
                                        void *out_key,
                                        bcmimm_be_fields_t *out);

/*!
 * \brief Clear the content of the table.
 *
 * This function clears the content of the table by deleting all the entries
 * associated with this table. The table state identical to the state of the
 * table after \ref bcmimm_be_table_create().
 *
 * \param [in] hdl This is the table handle.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_clear(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Insert an entry to the table.
 *
 * This function inserts an entry into the table specified via \c hdl. The
 * entry composed of key and data portions, where the data has
 * \c data_elements elements. If the entry already exists, it will be
 * override by the new values.
 * If a field was present before and it is no longer valid, it will be
 * deleted.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [in] in This is the input to the entry. It contains two arrays, both
 * with \c in->count elements. The first array contains the field IDs and the
 * second contains the field data. See \ref bcmimm_be_fields_t for more
 * information.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_insert(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  const bcmimm_be_fields_t *in);

/*!
 * \brief Delete an entry from the table.
 *
 * This function deletes an entry from the table specified via \c hdl. The
 * deletion is done based on the key value.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_delete(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key);

/*!
 * \brief Lookup an entry in the table.
 *
 * This function looks up for an entry in the table specified via \c hdl. The
 * lookup is done based on the key value.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [out] out This is the structure that contains the content of the
 * entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the \c out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_lookup(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  bcmimm_be_fields_t *out);

/*!
 * \brief Update an entry in the table.
 *
 * This function updates the content of an entry in the table specified via
 * \c hdl. The entry identified based on the key value. Once found, the fields
 * will be updated according to their specified operation (in->oper). Existing
 * fields that are not specified by the \c in parameter will remain the same.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [in] in This is the structure that contains the content of the
 * modified fields. The count field of \c in contains the arrays size.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_update(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  const bcmimm_be_fields_upd_t *in);

/*!
 * \brief Lock an entry of the table.
 *
 * This function locks an entry from the table specified via \c hdl. Locked
 * entry can not be modified (or deleted). This functionality is useful
 * if the entry being referred by another field in a different logical table.
 * By default every entry is unlocked unless explicitly locked.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_lock(bcmimm_be_tbl_hdl_t hdl,
                                const void *key);

/*!
 * \brief Unlock an entry of the table.
 *
 * This function unlocks an entry that was previously locked. Once unlocked,
 * the entry can be modified or deleted.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_unlock(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key);

/*!
 * \brief Obtain entry lock reference count.
 *
 * This function retrieves the lock reference count of an entry.
 * On success, the value will be placed into \c lock_cnt.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [out] lock_cnt The value of the lock reference count will be placed
 * into this output variable.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_get_lock_cnt(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  uint32_t *lock_cnt);

/*!
 * \brief Resize (up) the data size
 *
 * This function typically will call after s/w upgrade where the LT
 * changes causes the data to be larger (possibly by adding array field).
 * The function will replace all the fields into larger fields.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] d_size This is the desired (new) data size.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_data_resize(bcmimm_be_tbl_hdl_t hdl, size_t d_size);

#endif /* BCMIMM_BACKEND_H */
