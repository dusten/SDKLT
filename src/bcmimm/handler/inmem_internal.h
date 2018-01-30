/*! \file inmem_internal.h
 *
 *  In-memory internal header file for front end.
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

#ifndef INMEM_INTERNAL_H
#define INMEM_INTERNAL_H

#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/front_e_ha.h>

/*!
 * \brief Header space for every data block.
 */
#define BUFF_HEADER_SIZE sizeof(uint64_t)

/*!
 * \brief The maximal number of elements in a transaction involving IMM.
 */
#define BCMIMM_MAX_TRANS_SIZE  100

/*!
 * \brief The maximal size (in bytes) of the table key
 */
#define BCMIMM_MAX_KEY_SIZE     32

/*!
 * \brief In-memory data pointer construction.
 */
#define BCMIMM_BUF_PTR(ptr,sub,offset) (ptr) = (sub << 24) | offset

/*!
 * \brief Obtain sub module information from IMM pointer
 */
#define BCMIMM_PTR_SUB_INFO(ptr) (ptr) >> 24

/*!
 * \brief Obtain offset information from IMM pointer
 */
#define BCMIMM_PTR_OFFSET_INFO(ptr) (ptr) & 0xFFFFFF

/*!
 * \brief In-memory pre-defined block sub-module IDs
 */
#define GENERIC_TBL_COMP_ID     0
#define GENERIC_DATA_COMP_ID    1
#define GENERIC_KEY_COMP_ID     2
#define TRANS_TBL_COMP_ID       3
#define AUX_TBL_COMP_ID         4

/*!
 * \brief In-memory dynamic sub-module IDs minimal value
 */
#define MIN_SUB_ID              (AUX_TBL_COMP_ID+1)

/*!
 * \brief In-memory convert bits size to byte size (round up)
 */
#define BCMIMM_BITS_TO_BYTES(x)  (((x) + 7) / 8)

typedef enum bcmimm_tbl_owner_u {
    BCMIMM_TABLE_FREE, /*!< No ownership to the table */
    BCMIMM_TABLE_NB,   /*!< Northbound API owns the table */
    BCMIMM_TABLE_SB    /*!< Southbound API owns the table */
} bcmimm_tbl_owner_t;
/*!
 * \brief In-Memory HA table list.
 *
 * This structure contains the pointers to all the LT tables managed by this
 * module. This structure contains an array of pointers to LT table
 * information for every unit. It also contains pointers to the generic buffer
 * blocks. These buffer blocks are used by every logical table.
 */
extern imm_ha_blk_t *bcmimm_tables_list[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief In-Memory LT event structure.
 * This structure conatins all the callback functions and context that are
 * corresponding to specific table.
 */
typedef struct bcmimm_lt_event_s {
    bcmimm_lt_cb_t cb;
    void *context;
} bcmimm_lt_event_t;

/*!
 * \brief In-Memory table change event callback functions.
 *
 * This structure contains the pointers to all the components callback
 * functions. These functions will be invoke when the table associated
 * with them is changing.
 */
extern bcmimm_lt_event_t *bcmimm_tables_cb[BCMDRD_CONFIG_MAX_UNITS];


/*!
 * \brief In-Memory big endian indicator.
 *
 * This variable indicates if the environment is big or little endian.
 * The value of this variable is important when trying to compress uint64_t
 * type variables into smaller size.
 */
extern bool bcmimm_big_endian;

/*!
 *\brief Find a table within the LT served by this module.
 *
 * \param [in] unit Is the unit to search the table in.
 * \param [in] sid Is the table ID to find.
 * \param [out] idx Is the index in the tables list array of where the
 * table was found. If the caller doesn't care about the index this parameter
 * can be NULL.
 *
 * \return A pointer to the table information on success and NULL otherwise.
 */
extern table_info_t *bcmimm_tbl_find(int unit, uint32_t sid, uint32_t *idx);

/*!
 * \brief Call registered stage callback function.
 *
 * This function notifies a component that is registered to receive staging
 * events for a table.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] in This is the field list of the entry that is being changed.
 * \param [in] tbl This is the table being changed.
 * \param [in] idx Is the index in table in the table callback list. The
 * index is the index of the table in the master \c table_list[].
 * \param [in] trans_id Is the transaction ID associated with this callback.
 * \param [in] reason This value maps to one of the opcodes
 * (insert/delete/update).
 * \param [out] returned_fields If the component wants to provide an extra list
 * of fields (typically read only fields) it should provide them via this
 * parameter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_notify_stage(int unit,
                               const bcmltd_fields_t *in,
                               table_info_t *tbl,
                               uint32_t idx,
                               uint32_t trans_id,
                               bcmimm_entry_event_t reason,
                               bcmltd_fields_t *returned_fields);

/*!
 *\brief Insert key field into the key buffer.
 *
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [in] key_buf Is the key buffer where the key value should be
 * inserted to.
 * \param [in] idx Is the index of the key field in the table.
 * \param [in] data Is the value of the key to be inserted.
 *
 * \return None
 */
extern void bcmimm_insert_key_fld(table_info_t *tbl,
                                  uint8_t *key_buf,
                                  int idx,
                                  uint64_t data);

/*!
 *\brief Extract key field from the key buffer.
 *
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [in] key_buf Is the key buffer where the key value should be
 * extracted from.
 * \param [in] idx Is the index of the key field in the table.
 * \param [in] fld Is a pointer to the field of which its value should be
 * extracted.
 *
 * \return None
 */
extern void bcmimm_extract_key_fld(table_info_t *tbl,
                                   uint8_t *key_buf,
                                   int idx,
                                   bcmltd_field_t *fld);

/*!
 * \brief Initializes the in-memory logical tables associated for this unit.
 *
 * This function called by the system manager initializes tables associated
 * with this unit.
 *
 * \param [in] unit Is the unit to initialize.
 * \param [in] comp_id Is the component ID of this component.
 * \param [in] warm Indicates if the init is cold or warm.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_unit_init(int unit, uint8_t comp_id, bool warm);

/*!
 * \brief Shutdown the in-memory logical tables associated with this unit.
 *
 * This function shutdown all the in-memory LT associated with this unit.
 * It frees all associated resources. HA tables are not freed.
 *
 * \param [in] unit Is the unit to shutdown.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_unit_shutdown(int unit);

/*!
 *\brief Allocating working, non HA buffer.
 *
 * This function allocates a working buffer that can be used to stage an entry
 * into the back-end.
 *
 * \param [in] unit Is the unit associated with the operation.
 *
 * \return Pointer to the working buffer.
 */
extern uint8_t *bcmimm_working_buff_alloc(int unit);

/*!
 *\brief Free the working buffer.
 *
 * This function returned the working buffer to the general poll. Any threads
 * that were waiting to allocate the working buffer might get a change to
 * complete the allocation.
 *
 * \param [in] unit Is the unit associated with the operation.
 *
 * \return None.
 */
extern void bcmimm_working_buff_free(int unit);

/*!
 *\brief Extract entry fields from output BE buffers.
 *
 * \param [in] be_out Is the backend output after calling the BE lookup.
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [out] out Is the output field array where the fields will be
 * extracted to.
 *
 * \return The number of different fields (based on their IDs) that were
 * extracted. Array fields will only be counted once.
 */
extern int bcmimm_extract_buf_into_flds(bcmimm_be_fields_t *be_out,
                                        table_info_t *tbl,
                                        bcmltd_fields_t *out);

/*!
 *\brief Determines if a given field is a key field.
 *
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [in] fid Is the field ID to check.
 *
 * \return The index of the field if the field is a key and -1 otherwise.
 */
extern int bcmimm_is_key_field(table_info_t *tbl, uint32_t fid);

/*!
 *\brief Fills in default values.
 *
 * This function being call to complete the field list with fields that
 * has the default values. These fields were not explicitly set but in
 * some cases might be required by the application. This functionality
 * is only applicable for lookup operations.
 *
 * \param [in,out] out Is the current output list of non-default fields.
 * \param [in] start Is the index in the \c out structure where fields with
 * default values can be placed.
 * \param [in] tbl Is the logical table associated with this entry.
 *
 * \return The number of fields that were added to the output parameter.
 */
extern uint32_t bcmimm_def_vals_fill(bcmltd_fields_t *out,
                                     uint32_t start,
                                     table_info_t *tbl);

/*!
 *\brief Initialize internal structure.
 *
 * This function being call once during init to create some resources
 * for the internal front end implementation.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_internal_init(void);

/*!
 *\brief Release internal resources before shutdown.
 *
 * This function being call once to release all the resources that were
 * internally used.
 *
 * \return None.
 */
extern void bcmimm_internal_shutdown(void);

/*!
 *\brief Allocate internal resources per unit.
 *
 * This function is called by the \ref bcmimm_unit_init function to allocate
 * some internal resources.
 *
 * \return SHR_E_NONE on success.
 * \return SHR_E_MEMORY on failure to allocate resources.
 */
extern int bcmimm_internal_unit_init(int unit);

/*!
 *\brief Release per unit internal resources.
 *
 * This function being call during the shutdown of a unit to free the IMM
 * internal resources.
 *
 * \return None.
 */
extern void bcmimm_internal_unit_shutdown(int unit);

/*!
 *\brief Entry lookup function.
 *
 * This function being call to perform lookup of a particular entry in a
 * specified LT and unit. The function can be called by north or south bound
 * API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] trans_id Is the transaction ID associated with this lookup
 * operation.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters, typically the key fields.
 * \param [out] out Is the output structure where the entry that was looked up
 * will be placed. Note that the output will not contain the key fields that
 * were part of the input.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_lookup(int unit,
                                      uint32_t trans_id,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Entry delete function.
 *
 * This function being call to delete a particular entry in a
 * specified LT and unit. The function can be called by north or south bound
 * API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] trans_id Is the transaction ID associated with this delete
 * operation.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters, typically the key fields.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_delete(int unit,
                                      uint32_t trans_id,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Entry update function.
 *
 * This function being call to perform an update of a particular entry in a
 * specified LT and unit. The function can be called by north or south bound
 * API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] trans_id Is the transaction ID associated with this update
 * operation.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters. The input must include the key
 * fields and one or more data field.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_update(int unit,
                                      uint32_t trans_id,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Free field lists
 *
 * This function frees the elements of the key and data field lists back
 * into the field memory manager.
 *
 * \param [in] k_fields This is a list feels with key fields.
 * \param [in] d_fields This is a list feels with data fields.
 *
 * \return None
 */
extern void bcmimm_internal_free_lists(bcmltd_field_t *k_fields,
                                     bcmltd_field_t *d_fields);
/*!
 *\brief Entry insert function.
 *
 * This function being call to perform entry insert into specified LT and
 * unit. The function can be called by north or south bound API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] trans_id Is the transaction ID associated with this insert
 * operation.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters. The input must include the key
 * fields and one or more data field.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_insert(int unit,
                                      uint32_t trans_id,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Compress input fields into backend format.
 *
 * This function being called to compress the fields from the input \c in
 * into the back-end format. The back-end format has few buffers:
 * a. key_buf is the key buffer containing only the key information
 * b. f_data contains the fields data only
 * c. fid_buf contains information of the field IDs and index.
 *
 * \param [in] in Is the input parameters. The input must include the key
 * fields and one or more data field.
 * \param [in] tbl Is the relevant table structure.
 * \param [out] key_buf Is the buffer to place the key fields data into.
 * \param [out] f_data Is the buffer where the data field values will be
 * placed into.
 * \param [out] fid_buf Is the buffer where the data field's ID will be placed
 * into.
 *
 * \return Return the total number of data fields that were written into the
 * output buffers.
 */
extern uint32_t bcmimm_compress_fields_into_buffer(const bcmltd_fields_t *in,
                                            table_info_t *tbl,
                                            uint8_t *key_buf,
                                            uint8_t *f_data,
                                            uint8_t *fid_buf);

/*!
 *\brief Compress read-only input fields into backend format.
 *
 * This function being called to compress read-only fields provided by the
 * component callback into the back-end format. The back-end format has few
 * buffers:
 * a. key_buf is the key buffer containing only the key information
 * b. f_data contains the fields data only
 * c. fid_buf contains information of the field IDs and index.
 *
 * Read only fields can not be part of the key, therefore there is no reference
 * to the key fields or buffer.
 *
 * \param [in] ro_fields Is a linked list of read-only fields generated by the
 * component staging callback.
 * \param [in] tbl Is the relevant table structure.
 * \param [in] idx Is the number of fields that already inside the output
 * buffers \c f_data and \c fid_buf.
 * \param [out] f_data Is the buffer where the data field values will be
 * placed into.
 * \param [out] fid_buf Is the buffer where the data field's ID will be placed
 * into.
 *
 * \return Return the total number of data fields that are currently at the
 * output buffers.
 */
int bcmimm_compress_ro_fields_into_buffer(bcmltd_fields_t *ro_fields,
                                          table_info_t *tbl,
                                          uint32_t idx,
                                          uint8_t *f_data,
                                          uint8_t *fid_buf);

/*!
 *\brief Entry get first (traverse start).
 *
 * This function finds the first entry of the table with matching sid.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] sid Is the LT ID of the traversed table.
 * \param [out] out Is the output where the first entry fields will be placed.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_get_first(int unit,
                                         bcmltd_sid_t sid,
                                         bcmltd_fields_t *out,
                                         bcmimm_tbl_owner_t owner);

/*!
 *\brief Entry get next (traverse continue).
 *
 * This function finds the entry based on the input parameter \c in. It then
 * search for the entry followed the current entry.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] sid Is the LT ID of the traversed table.
 * \param [in] in Is the input containing the key value of the last entry that
 * was traversed.
 * \param [out] out Is the output where the next entry fields will be placed.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_get_next(int unit,
                                        bcmltd_sid_t sid,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        bcmimm_tbl_owner_t owner);


/*!
 *\brief Enable/Disable HA support
 *
 * This function sets the support of HA feature within the IMM. Supporting HA
 * means that all the operations that can change LT should be reversable in
 * the case of abort.
 *
 * \param [in] enable Indicates if the feature is enabled (=true) or disabled.
 *
 * \return None.
 */
extern void bcmimm_ha_enable_set(bool enable);

#endif /* INMEM_INTERNAL_H */
