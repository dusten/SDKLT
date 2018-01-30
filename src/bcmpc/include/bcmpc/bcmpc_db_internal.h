/*! \file bcmpc_db_internal.h
 *
 * BCMPC Internal Database Library.
 *
 * Declaration of the structures and functions for BCMPC internal database.
 *
 * There are two sets of database APIs in the BCMPC.
 *
 * 1. Regular-memory-based DB APIs
 *    The regular-memory-based APIs provide a list structure,
 *     \ref bcmpc_db_entry_t to link the elements/entries of a table.
 *
 * 2. IMM-based DB APIs
 *    The IMM-based APIs rely on In-memory (IMM) inter-component functions.
 *    The data in the IMM DB is HA-enabled and accessible across components.
 *
 * The IMM-based data is preserved across a warmboot/crash, while the
 * regular-memory-based data is not.
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

#ifndef BCMPC_DB_INTERNAL_H
#define BCMPC_DB_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>


/*******************************************************************************
 * Regular-memory-based DB APIs
 */

/*!
 * \brief The max number of key fields per table
 */
#define BCMPC_DB_NUM_KEYS_MAX (3)

/*!
 * \brief To get the field position in a structure.
 *
 * \param [in] _st Structure name.
 * \param [in] _f Field name.
 *
 * \returns The position of \c _f in \c _st
 */
#define BCMPC_DB_OFFSETOF(_st, _f) \
            ((size_t)((char *)&((_st *)(0))->_f - (char *)0))

/*!
 * \brief Get structure pointer from its db_entry member.
 *
 * \param [in] _ptr Pointer to the address of \c _f.
 * \param [in] _st Structure name.
 * \param [in] _f Field name for bcmpc_db_entry_t in \c _st.
 *
 * \returns The address of \c _st
 */
#define BCMPC_DB_ENTRY(_ptr, _st, _f) \
            ((_st *)((char *)_ptr - BCMPC_DB_OFFSETOF(_st, _f)))

/*!
 * \brief Structure for database entry list
 *
 * This structure is used to construct a linked list via \c next.
 * And each entry could be identified by the \c key.
 */
typedef struct bcmpc_db_entry_s {

    /*! Pointer to the next element in the list (or NULL) */
    struct bcmpc_db_entry_s *next;

    /*! Reference count of the entry */
    int refcnt;

    /*! Key to identify the entry */
    uint32_t keys[BCMPC_DB_NUM_KEYS_MAX];

} bcmpc_db_entry_t;


/*!
 * \brief Search the entry with the given key.
 *
 * To retrieve the entry with key from table.
 *
 * \param [in] table The table to search.
 * \param [in] num_keys The number of keys.
 * \param [in] keys Key values to search.
 * \param [out] prev The pointer to the previous entry.
 *
 * \returns Pointer to the retrieved entry, or NULL when not found.
 */
extern bcmpc_db_entry_t *
bcmpc_db_entry_search(bcmpc_db_entry_t *table, size_t num_keys, uint32_t *keys,
                      bcmpc_db_entry_t **prev);


/*!
 * \brief Add an entry to the given table.
 *
 * To add a new entry to the given table.
 *
 * \param [in] table The table to add an entry.
 * \param [in] new The new entry to add.
 *
 * \returns Nothing.
 */
extern void
bcmpc_db_entry_add(bcmpc_db_entry_t **table, bcmpc_db_entry_t *new);


/*!
 * \brief Delete an entry from the given table.
 *
 * To delete an entry from the given table.
 *
 * \param [in] table The table to delete an entry.
 * \param [in] num_keys The number of keys.
 * \param [in] keys Key values to search.
 *
 * \returns Pointer to the entry to be deleted.
 */
extern bcmpc_db_entry_t *
bcmpc_db_entry_del(bcmpc_db_entry_t **table, size_t num_keys, uint32_t *keys);


/*!
 * \brief Get the next entry from the given table.
 *
 * To retrieve the next entry of \c cur.
 *
 * When \c cur is NULL, return the pointer to the first entry.
 *
 * \param [in] table The table to search.
 * \param [in] cur The current entry pointer.
 *
 * \returns Pointer to the retrieved entry or NULL.
 */
extern bcmpc_db_entry_t *
bcmpc_db_entry_next(bcmpc_db_entry_t *table, bcmpc_db_entry_t *cur);

/*!
 * \brief Increment the entry reference count with the given key.
 *
 * \param [in] table The table to search.
 * \param [in] num_keys The number of keys.
 * \param [in] keys Key values to search.
 *
 * \returns 0 when No errors, otherwise -1.
 */
int
bcmpc_db_entry_refcnt_incr(bcmpc_db_entry_t *table, size_t num_keys,
                           uint32_t *keys);

/*!
 * \brief Decrement the entry reference count with the given key.
 *
 * \param [in] table The table to search.
 * \param [in] num_keys The number of keys.
 * \param [in] keys Key values to search.
 *
 * \returns 0 when No errors, otherwise a negative value.
 */
int
bcmpc_db_entry_refcnt_decr(bcmpc_db_entry_t *table, size_t num_keys,
                           uint32_t *keys);

/*!
 * \brief Get the entry reference count with the given key.
 *
 * \param [in] table The table to search.
 * \param [in] num_keys The number of keys.
 * \param [in] keys Key values to search.
 *
 * \returns The reference count or -1 when error.
 */
extern int
bcmpc_db_entry_refcnt_get(bcmpc_db_entry_t *table, size_t num_keys,
                          uint32_t *keys);


/*******************************************************************************
 * IMM-based DB APIs
 */

/*!
 * \brief Parameter for IMM-based DB functions.
 *
 * This is a convenience macro for using the IMM-based DB functions.
 * By this macro, the parameter \c _p will attach with its size.
 *
 * \param [in] _p Pointer to the parameter.
 *
 * \return size the parameter and its value.
 */
#define P(_p) sizeof(*(_p)), _p

/*!
 * \brief Parse LT field and save to the corresponding PC structure.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmpc_db_imm_entry_parse_f)(int unit, const bcmltd_fields_t *flds,
                              size_t key_size, void *key,
                              size_t data_size, void *data);

/*!
 * \brief Fill LT field according to the corresponding PC structure.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int
(*bcmpc_db_imm_entry_fill_f)(int unit,
                             size_t key_size, void *key,
                             size_t data_size, void *data,
                             bcmltd_fields_t *flds);

/*!
 * \brief LT schema handler.
 *
 * The data saved in the IMM DB is in LT fields format, and the data used in PC
 * is PC-specific data structures.
 *
 * The schema handler is a translator that converts the data in LT fields
 * from/to PC internal data structure members. Each PC LT should define its own
 * schema handler to handle how the internal data type are mapped to LT fields.
 */
typedef struct bcmpc_db_imm_schema_handler_s {

    /*! Parse LT fields. */
    bcmpc_db_imm_entry_parse_f entry_parse;

    /*! Fill LT fields. */
    bcmpc_db_imm_entry_fill_f entry_fill;

} bcmpc_db_imm_schema_handler_t;


/*!
 * \brief IMM-based database initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
int
bcmpc_db_imm_init(int unit);

/*!
 * \brief IMM-based database cleanup.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
int
bcmpc_db_imm_cleanup(int unit);

/*!
 * \brief Register the schema handler for the given LT.
 *
 * The associated handler will be overwrite when doing register for more than
 * one time.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] hdl Schema Handler, \ref bcmpc_db_imm_schema_handler_t.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_db_imm_schema_reg(int unit, bcmltd_sid_t sid,
                        bcmpc_db_imm_schema_handler_t *hdl);

/*!
 * \brief Check the given table is valid in IMM DB.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 *
 * \return SHR_E_NONE The table is valid.
 * \return SHR_E_NO_HANDLER No handler for the table.
 */
extern int
bcmpc_db_imm_tbl_valid(int unit, bcmltd_sid_t sid);

/*!
 * \brief Lookup for the entry with the given key.
 *
 * To retrieve the \c data of the entry with \c key in \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [out] data Data value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_db_imm_entry_lookup(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key,
                          size_t data_size, void *data);

/*!
 * \brief Update the entry with the given key and data.
 *
 * To retrive \c data of the entry with \c key in \c sid.
 *
 * When \c data is NULL, this function is used to check the existence of
 * the given \c key.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_db_imm_entry_update(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key,
                          size_t data_size, void *data);

/*!
 * \brief Delete the entry with the given key.
 *
 * To delete the entry with \c key from \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_db_imm_entry_delete(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key);

/*!
 * \brief Insert an entry with the given key and data.
 *
 * To insert an entry with \c key and \c data to \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_db_imm_entry_insert(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key,
                          size_t data_size, void *data);

/*!
 * \brief Lock the entry with the given key.
 *
 * To look the entry with \c key in \c sid.
 *
 * Locked entry can not be modified (or deleted). This functionality is
 * useful if the entry being referred by another field in a different logical
 * table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_db_imm_entry_lock(int unit, bcmltd_sid_t sid,
                        size_t key_size, void *key);

/*!
 * \brief Unlock the entry with the given key.
 *
 * To unlook the entry with \c key in \c sid.
 *
 * This function unlocks an entry that was previously locked. Once unlocked,
 * the entry can be modified or deleted.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_db_imm_entry_unlock(int unit, bcmltd_sid_t sid,
                          size_t key_size, void *key);

/*!
 * \brief Callback function for entry traversing.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [in] cookie User data for this traverse function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure during executing the callback function.
 */
typedef int
(*bcmpc_db_imm_traverse_cb_f)(int unit, size_t key_size, void *key,
                              size_t data_size, void *data,
                              void *cookie);

/*!
 * \brief Traverse the entries in the given table.
 *
 * This function will traverse the entries in \c sid and pass each entry to
 * \c cb_func.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key_size Key size.
 * \param [in] data_size Data size.
 * \param [in] cb_func Callback function.
 * \param [in] cookie User data for the traverse function \c cb_func.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_FAIL Fail to traverse all entries.
 */
extern int
bcmpc_db_imm_entry_traverse(int unit, bcmltd_sid_t sid,
                            size_t key_size, size_t data_size,
                            bcmpc_db_imm_traverse_cb_f cb_func, void *cookie);

#endif /* BCMPC_DB_INTERNAL_H */
