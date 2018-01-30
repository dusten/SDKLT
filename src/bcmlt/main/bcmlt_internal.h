/*! \file bcmlt_internal.h
 *
 *  This file contains data structure definition and definitions of functions
 *  that are internal to the BCMLT client API
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

#ifndef BCMLT_INTERNAL_H
#define BCMLT_INTERNAL_H

#include <sal/sal_mutex.h>
#include <shr/shr_types.h>
#include <shr/shr_lmem_mgr.h>
#include <bsl/bsl.h>
#include <bcmtrm/trm_api.h>

/* Macros used for common parameter checking */
#define ENTRY_VALIDATE(entry) \
    if (!entry || (entry->state != E_ACTIVE)) {                             \
        if (!entry) {                                                       \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Invalid entry handle\n")));              \
        } else {                                                            \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Entry was committed\n")));               \
        }                                                                   \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                                   \
    }

#define UNIT_VALIDATION(unit)   \
    if (!bcmlt_is_unit_attached(unit)) {                                    \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit, "Unit not attached\n")));             \
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);                                    \
    }

#define LT_OPCODE_VALIDATE(opcode, unit) \
    if ((opcode <= BCMLT_OPCODE_NOP) || (opcode >= BCMLT_OPCODE_NUM)) {     \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid opcode %d\n"), opcode));           \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                                   \
    }

#define PT_OPCODE_VALIDATE(opcode, unit) \
    if ((opcode <= BCMLT_PT_OPCODE_NOP) || (opcode >= BCMLT_PT_OPCODE_NUM)) {\
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid opcode %d\n"), opcode));           \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                                   \
    }

#define PRIORITY_VALIDATE(priority, unit) \
    if ((priority < BCMLT_PRIORITY_NORMAL) ||                               \
        (priority > BCMLT_PRIORITY_HIGH)) {                                 \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid priority %d\n"), priority));       \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                                   \
    }

#define NOTIFICATION_VALIDATE(notif_opt, notif_fn, unit) \
    if ((notif_opt >= BCMLT_NOTIF_OPTION_NUM) ||                            \
        (notif_opt < BCMLT_NOTIF_OPTION_NO_NOTIF) ||                        \
        ((notif_opt > BCMLT_NOTIF_OPTION_NO_NOTIF) && !notif_fn)) {         \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid notification option %d\n"),        \
                     notif_opt));                                           \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                                   \
    }

#define PT_NOTIFICATION_VALIDATE(notif_opt, notif_fn, unit) \
    if ((notif_opt >= BCMLT_NOTIF_OPTION_NUM) ||                            \
        (notif_opt < BCMLT_NOTIF_OPTION_NO_NOTIF) ||                        \
        (notif_opt == BCMLT_NOTIF_OPTION_COMMIT) ||                        \
        ((notif_opt > BCMLT_NOTIF_OPTION_NO_NOTIF) && !notif_fn)) {         \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid notification option %d\n"),        \
                     notif_opt));                                           \
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);                                   \
    }


typedef enum bcmlt_table_acc_e {
    BCMLT_TABLE_ACCESS_READ_WRITE = 0,  /*!< Allow read and write access */
    BCMLT_TABLE_ACCESS_READ_ONLY        /*!< Read only access mode       */
} bcmlt_table_acc_t;

/*!
 * \brief Table attribute structure.
 *
 * This structure contains attributes required for the control path.
 */
typedef struct {
    uint32_t          table_id;     /*!< Table ID                      */
    const char        *name;        /*!< Table name                    */
    bool              interactive;  /*!< interactive table             */
    bcmlt_table_acc_t access;       /*!< Access attribute of the table */
    bool              pt;           /*!< Table is pass through?        */
    uint32_t          num_of_fields;/*!< Table number of fields        */
    uint32_t          max_fid;      /*!< Maximum field ID */
} bcmlt_table_attrib_t;

/*!
 * \brief Module init function.
 *
 * This function must be called during the s/w initialization. It should
 * be called only once. Before calling this function none of the
 * bcmlt_db_xxx functions can be called.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_init(void);

/*!
 * \brief Module delete function.
 *
 * This function must be called during the s/w shutdown. It should
 * be called only once. No other bcmlt_db_xxx function should be
 * called after this function.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_delete(void);

/*!
 * \brief Device init function.
 *
 * This function initializes the bcmlt_db for this unit. Only after calling
 * this function the application may call other function in this library
 * associated with this unit.
 *
 * \param [in] unit Is the device unit.
 *
 * \return SHR_E_NONE on success and error code otherwise. Reason for failure
 * can be:
 * a. Insufficient memory resources.
 * b. DRD was not initialized for this unit.
 * c. LRD was not initialized for this unit.
 */
extern int bcmlt_tables_db_init (int unit);

/*!
 * \brief Device delete function.
 *
 * This function deletes the _bcmlt_db for this unit. It deletes
 * all the tables associated with this unit and free all the
 * table resources. After calling
 * this function the application should not call other function in
 * this library associated with this unit (result in failure).
 *
 * \param [in] unit Is the device unit.
 *
 * \return 0 for success, otherwise failure.
 */
extern int bcmlt_tables_db_delete (int unit);

/*!
 * \brief Clean out all data fields
 *
 * This function filters out all the data fields and leaving the entry
 * only with key fields. This function is typically being used for lookup
 * or traverse operations where residue from previous operations is unwanted.
 *
 * \param [in] entry Is the entry to process its fields.
 *
 * \return SHR_E_NONE on success and SHR_E_UNAVAIL otherwise.
 */
extern int bcmlt_entry_clean_data_fields(bcmtrm_entry_t *entry);

/*!
 * \brief Fill out the field IDs of all key fields
 *
 * This function go through the table fields and copy the field IDs of all
 * key fields into the array \c keys. It also updated the parameter
 * \c num_of_keys with the number of key fields found.
 *
 * \param [in] unit Is the device unit.
 * \param [in] hdl Is the table database handle, typically obtained via the
 * function \ref bcmlt_db_table_info_get().
 * \param [in] array_size Is the number of element that can be contained in
 * the array \c keys.
 * \param [out] keys Is an array where the key fields ID will be placed.
 * \param [out] num_of_keys This parameter will contain the actual number of
 * key fields that were copied into the array \c keys.
 *
 * \return None.
 */
extern void bcmlt_db_table_keys_get(int unit,
                                    void *hdl,
                                    uint32_t array_size,
                                    uint32_t *keys,
                                    uint32_t *num_of_keys);
/*!
 * \brief Obtain specific table attributes.
 *
 * This function provides the attributes of a specific table on specific unit
 *
 * \param [in] unit Is the device unit.
 * \param [in] table_name Is the table name.
 * \param [out] attrib Is a double pointer pointing to returned attributes.
 * \param [out] fld_array_hdl Is the memory handle to use for field array
 * allocation.
 * \param [out] hdl Is a double pointer pointing to returned context. This
 *              context should be use in consecutive call to
 *              bcmlt_db_table_info_get().
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_table_info_get(int unit,
                                   const char *table_name,
                                   bcmlt_table_attrib_t **attrib,
                                   shr_lmm_hdl_t *fld_array_hdl,
                                   void **hdl);


/*!
 * \brief Obtain specific table attributes.
 *
 * This function provides the attributes of a specific table on specific unit.
 *
 * \param [in] unit Is the device unit.
 * \param [in] name Is the field name.
 * \param [in] hdl Is the table database handle, typically obtained via the
 * function \ref bcmlt_db_table_info_get().
 * \param [out] f_attr Is a double pointer pointing to returned field
 *              attributes.
 * \param [out] field_id Is a pointer pointing to returned field ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_field_info_get(int unit,
                                   const char *name,
                                   void *hdl,
                                   bcmlt_field_def_t **f_attr,
                                   uint32_t *field_id);

/*!
 * \brief Start table enumeration
 *
 * This API starts the retrieval of all available logical table names
 * on a particular device. The function returns the name of one table.
 * Use the table search next to retrieve the rest of the tables.
 *
 * \param [in] unit Is the device number for the table of interest.
 * \param [in] flags Can be used to filter the output.
 * \param [out] name Pointer to the first table name.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_table_name_get_first(int unit,
                                         uint32_t flags,
                                         char **name);

/*!
 * \brief Continue table enumeration
 *
 * This API continue the retrieval of all available logical table names
 * on a particular device. The function returns the name of one table.
 * Use this function again to retrieve more names.
 *
 * \param [in] unit Is the device number for the table of interest.
 * \param [in] flags Can be used to filter the output.
 * \param [out] name Pointer to the table name that was found next.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_table_name_get_next(int unit,
                                        uint32_t flags,
                                        char **name);

/*!
 * \brief Obtain table fields definition.
 *
 * This function obtains the attributes of the fields in a given table.
 * The attributes will be copied into caller provided buffer. The
 * caller must also provide the size of the output buffer. If the
 * caller doesn't know apriori the required size it can call with NULL
 * pointer for the output buffer and 0 for the fields_array_size. In
 * any case the require_array_size will contain the desired space
 * (number of elements) to obtain the fields information
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] table_name Is the table name
 * \param [in] fields_array_size Is the number of fields the output
 * array can hold.
 * \param [out] field_defs_array Pointer to caller provided memory
 * where the fields attributes should be copied to.
 * \param [out] require_array_size Is a pointer where the function
 * places the number of fields in the table.
 *
 * \return SHR_E_NONE on success, otherwise failure finding the table
 */
extern int bcmlt_db_table_field_defs_get(int unit,
                                         const char *table_name,
                                         uint32_t fields_array_size,
                                         bcmlt_field_def_t *field_defs_array,
                                         uint32_t *require_array_size);

/*!
 * \brief Obtain the maximal number of fields in an interactive table.
 *
 * This function obtains the maximal number of fields that can be in any
 * interactive table of this unit.
 *
 * \param [in] unit Is the device number the query reffers to.
 *
 * \return The maximal number of fields at any interactive table. The function
 * returns 0 for wrong unit or if no interactive tables exist for this unit.
 */
extern uint32_t bcmlt_db_interact_tbl_max_fields(int unit);


/*!
 * \brief Indicates if bcmlt component had been initialized.
 *
 * This function returns true if the bcmlt component was initialized and false
 * otherwise.
 *
 * \return true if bcmlt library was succesfully initialized and
 * false otherwise.
 */
extern bool bcmlt_is_initialized(void);

/*!
 * \brief Find field in a given entry.
 *
 * This function searches for a particular field, based on \c field_id in
 * a given entry. If found and if \c prev is not NULL, it also provide the
 * field that is predecessor to the desired field.
 *
 * \param [in] entry Is the entry to search for the field.
 * \param [in] field_id Is the search criteria.
 * \param [out] prev Can be NULL. If not NULL and the field was found, then
 * \c prev will contain the field that is previous to the desired field in
 * the linked list of fields.
 *
 * \return pointer to the desired field if the field was found and NULL
 * otherwise.
 */
extern shr_fmm_t *bcmlt_find_field_in_entry(bcmtrm_entry_t *entry,
                                            uint32_t field_id,
                                            shr_fmm_t **prev);

/*
 * Replay functions
*/

/*!
 * \brief Record entry operation.
 *
 * This function records the entry operation so it can be played back at a
 * later time. The function \c bcmlt_replay_record_start() must be called
 * prior to calling this function for this function to succeed.
 *
 * \param [in] entry Is the entry to record. The entry structure
 * contains all the information and action required to replay this instruction.
 *
 * \return SHR_E_NONE if the entry was recorded and error code otherwise.
 */
extern int bcmlt_replay_entry_record(bcmtrm_entry_t *entry);

/*!
 * \brief Record transaction operation.
 *
 * This function records the transaction operation so it can be played back
 * at a later time. The function \c bcmlt_replay_record_start() must be called
 * prior to calling this function for this function to succeed.
 *
 * \param [in] trans Is the transaction to record. The transaction
 * structure contains all the information and action required to replay this
 * transaction action.
 *
 * \return SHR_E_NONE if the transaction was recorded and error code otherwise.
 */
extern int bcmlt_replay_trans_record(bcmtrm_trans_t *trans);

/*!
 * \brief Initialize the handle structure.
 *
 * This function initializes the handle structure.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern int bcmlt_hdl_init(void);

/*!
 * \brief Shutdown the handle structure.
 *
 * This function shutdown the handle structure and free
 * all the associated resources.
 *
 * \return None.
 */
extern void bcmlt_hdl_shutdown(void);

/*!
 * \brief Allocates handle from the free handle poll.
 *
 * This function allocates a new handle and initializes
 * the handle content with a given pointer to the data. The caller can use
 * the returned handle to obtain the pointer to the data using the function
 * \ref bcmlt_hdl_data_get()
 *
 * \param [in] data Is a pointer for the caller that should be associated
 * with the new allocated handle.
 * \param [out] hdl Can be used later to obtain the pointer of \c data.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern int bcmlt_hdl_alloc(void *data, uint32_t *hdl);

/*!
 * \brief Free previously allocated handle.
 *
 * This function frees a previously allocated handle and returned the handle
 * to the free list to be reused.
 *
 * \param [in] hdl Is the handle to free.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern int bcmlt_hdl_free(uint32_t hdl);

/*!
 * \brief Obtain the data associated with a handle.
 *
 * This function initializes the handle structure for the given unit.
 *
 * \param [in] hdl Is the handle that was previously allocated.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern void *bcmlt_hdl_data_get(uint32_t hdl);

/*!
 * \brief BCMLT module Init.
 *
 * This is the first function to invoke from client application.
 *
 * A library init is only allowed once per lifecycle of the application process.
 * Subsequent inits following a successful library init call are rejected.
 *
 * Client library will allocate and initialize the data structure and resources
 * required by the library.
 *
 * \return SHR_E_NONE success, otherwise failure in allocating resources
 */
extern int bcmlt_init(void);

/*!
 * \brief BCMLT module Close
 *
 * This is the last function to invoke from client application.
 *
 * A library close will clean up all the resources used by the library
 * including recycle all the active transactions
 *
 * \return SHR_E_NONE success, otherwise failure in freeing resources
 */
extern int bcmlt_close(void);


#endif /* BCMLT_INTERNAL_H */
