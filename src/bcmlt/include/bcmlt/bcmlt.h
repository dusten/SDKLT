/*! \file bcmlt.h
 *
 * BCM Logical Table API.
 *
 */
/*
 * Apache License
 * Version 2.0, January 2004
 * http://www.apache.org/licenses/
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
 * 1. Definitions.
 * "License" shall mean the terms and conditions for use, reproduction, and 
 * distribution as defined by Sections 1 through 9 of this document.
 * "Licensor" shall mean the copyright owner or entity authorized by the 
 * copyright owner that is granting the License.
 * "Legal Entity" shall mean the union of the acting entity and all other 
 * entities that control, are controlled by, or are under common control with 
 * that entity. For the purposes of this definition, "control" means (i) the 
 * power, direct or indirect, to cause the direction or management of such 
 * entity, whether by contract or otherwise, or (ii) ownership of fifty 
 * percent (50%) or more of the outstanding shares, or (iii) beneficial 
 * ownership of such entity.
 * "You" (or "Your") shall mean an individual or Legal Entity exercising 
 * permissions granted by this License.
 * "Source" form shall mean the preferred form for making modifications, 
 * including but not limited to software source code, documentation source, 
 * and configuration files.
 * "Object" form shall mean any form resulting from mechanical transformation 
 * or translation of a Source form, including but not limited to compiled 
 * object code, generated documentation, and conversions to other media types.
 * "Work" shall mean the work of authorship, whether in Source or Object form, 
 * made available under the License, as indicated by a copyright notice that 
 * is included in or attached to the work (an example is provided in the 
 * Appendix below).
 * "Derivative Works" shall mean any work, whether in Source or Object form, 
 * that is based on (or derived from) the Work and for which the editorial 
 * revisions, annotations, elaborations, or other modifications represent, as 
 * a whole, an original work of authorship. For the purposes of this License, 
 * Derivative Works shall not include works that remain separable from, or 
 * merely link (or bind by name) to the interfaces of, the Work and Derivative 
 * Works thereof.
 * "Contribution" shall mean any work of authorship, including the original 
 * version of the Work and any modifications or additions to that Work or 
 * Derivative Works thereof, that is intentionally submitted to Licensor for 
 * inclusion in the Work by the copyright owner or by an individual or Legal 
 * Entity authorized to submit on behalf of the copyright owner. For the 
 * purposes of this definition, "submitted" means any form of electronic, 
 * verbal, or written communication sent to the Licensor or its 
 * representatives, including but not limited to communication on electronic 
 * mailing lists, source code control systems, and issue tracking systems that 
 * are managed by, or on behalf of, the Licensor for the purpose of discussing 
 * and improving the Work, but excluding communication that is conspicuously 
 * marked or otherwise designated in writing by the copyright owner as "Not a 
 * Contribution."
 * "Contributor" shall mean Licensor and any individual or Legal Entity on 
 * behalf of whom a Contribution has been received by Licensor and 
 * subsequently incorporated within the Work.
 * 2. Grant of Copyright License. Subject to the terms and conditions of this 
 * License, each Contributor hereby grants to You a perpetual, worldwide, 
 * non-exclusive, no-charge, royalty-free, irrevocable copyright license to 
 * reproduce, prepare Derivative Works of, publicly display, publicly perform, 
 * sublicense, and distribute the Work and such Derivative Works in Source or 
 * Object form.
 * 3. Grant of Patent License. Subject to the terms and conditions of this 
 * License, each Contributor hereby grants to You a perpetual, worldwide, 
 * non-exclusive, no-charge, royalty-free, irrevocable (except as stated in 
 * this section) patent license to make, have made, use, offer to sell, sell, 
 * import, and otherwise transfer the Work, where such license applies only to 
 * those patent claims licensable by such Contributor that are necessarily 
 * infringed by their Contribution(s) alone or by combination of their 
 * Contribution(s) with the Work to which such Contribution(s) was submitted. 
 * If You institute patent litigation against any entity (including a 
 * cross-claim or counterclaim in a lawsuit) alleging that the Work or a 
 * Contribution incorporated within the Work constitutes direct or 
 * contributory patent infringement, then any patent licenses granted to You 
 * under this License for that Work shall terminate as of the date such 
 * litigation is filed.
 * 4. Redistribution. You may reproduce and distribute copies of the Work or 
 * Derivative Works thereof in any medium, with or without modifications, and 
 * in Source or Object form, provided that You meet the following conditions:
 * You must give any other recipients of the Work or Derivative Works a copy 
 * of this License; and
 * You must cause any modified files to carry prominent notices stating that 
 * You changed the files; and
 * You must retain, in the Source form of any Derivative Works that You 
 * distribute, all copyright, patent, trademark, and attribution notices from 
 * the Source form of the Work, excluding those notices that do not pertain to 
 * any part of the Derivative Works; and
 * If the Work includes a "NOTICE" text file as part of its distribution, then 
 * any Derivative Works that You distribute must include a readable copy of 
 * the attribution notices contained within such NOTICE file, excluding those 
 * notices that do not pertain to any part of the Derivative Works, in at 
 * least one of the following places: within a NOTICE text file distributed as 
 * part of the Derivative Works; within the Source form or documentation, if 
 * provided along with the Derivative Works; or, within a display generated by 
 * the Derivative Works, if and wherever such third-party notices normally 
 * appear. The contents of the NOTICE file are for informational purposes only 
 * and do not modify the License. You may add Your own attribution notices 
 * within Derivative Works that You distribute, alongside or as an addendum to 
 * the NOTICE text from the Work, provided that such additional attribution 
 * notices cannot be construed as modifying the License. 
 * 
 * You may add Your own copyright statement to Your modifications and may 
 * provide additional or different license terms and conditions for use, 
 * reproduction, or distribution of Your modifications, or for any such 
 * Derivative Works as a whole, provided Your use, reproduction, and 
 * distribution of the Work otherwise complies with the conditions stated in 
 * this License.
 * 5. Submission of Contributions. Unless You explicitly state otherwise, any 
 * Contribution intentionally submitted for inclusion in the Work by You to 
 * the Licensor shall be under the terms and conditions of this License, 
 * without any additional terms or conditions. Notwithstanding the above, 
 * nothing herein shall supersede or modify the terms of any separate license 
 * agreement you may have executed with Licensor regarding such Contributions.
 * 6. Trademarks. This License does not grant permission to use the trade 
 * names, trademarks, service marks, or product names of the Licensor, except 
 * as required for reasonable and customary use in describing the origin of 
 * the Work and reproducing the content of the NOTICE file.
 * 7. Disclaimer of Warranty. Unless required by applicable law or agreed to 
 * in writing, Licensor provides the Work (and each Contributor provides its 
 * Contributions) on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 * KIND, either express or implied, including, without limitation, any 
 * warranties or conditions of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or 
 * FITNESS FOR A PARTICULAR PURPOSE. You are solely responsible for 
 * determining the appropriateness of using or redistributing the Work and 
 * assume any risks associated with Your exercise of permissions under this 
 * License.
 * 8. Limitation of Liability. In no event and under no legal theory, whether 
 * in tort (including negligence), contract, or otherwise, unless required by 
 * applicable law (such as deliberate and grossly negligent acts) or agreed to 
 * in writing, shall any Contributor be liable to You for damages, including 
 * any direct, indirect, special, incidental, or consequential damages of any 
 * character arising as a result of this License or out of the use or 
 * inability to use the Work (including but not limited to damages for loss of 
 * goodwill, work stoppage, computer failure or malfunction, or any and all 
 * other commercial damages or losses), even if such Contributor has been 
 * advised of the possibility of such damages.
 * 9. Accepting Warranty or Additional Liability. While redistributing the 
 * Work or Derivative Works thereof, You may choose to offer, and charge a fee 
 * for, acceptance of support, warranty, indemnity, or other liability 
 * obligations and/or rights consistent with this License. However, in 
 * accepting such obligations, You may act only on Your own behalf and on Your 
 * sole responsibility, not on behalf of any other Contributor, and only if 
 * You agree to indemnify, defend, and hold each Contributor harmless for any 
 * liability incurred by, or claims asserted against, such Contributor by 
 * reason of your accepting any such warranty or additional liability.
 * END OF TERMS AND CONDITIONS
 * APPENDIX: HOW TO APPLY THE APACHE LICENSE TO YOUR WORK
 * To apply the Apache License to your work, attach the following boilerplate 
 * notice, with the fields enclosed by brackets "[]" replaced with your own 
 * identifying information. (Don't include the brackets!) The text should be 
 * enclosed in the appropriate comment syntax for the file format. We also 
 * recommend that a file or class name and description of purpose be included 
 * on the same "printed page" as the copyright notice for easier 
 * identification within third-party archives.
 * Copyright [yyyy] [name of copyright owner]
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not 
 * use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the 
 * License for the specific language governing permissions and limitations 
 * under the License.
 */

#ifndef BCMLT_H
#define BCMLT_H

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_lt_types.h>

/*! \brief Overview
 *
 * The SDK uses logical tables (LT) to enable the application to configure
 * the underlying device. Each LT is associated with a unique name.
 * A LT is composed of one or more columns and rows.
 * BCMLT is the API to manage the various LT by manipulating the field values
 * of its rows.
 * At a high level, the API interacts with four types of objects:
 * 1. Fields - these are the logical table columns. There are two types of
 *    fields: data and key. A key field (if it exists) must be used for every
 *    operation.
 * 2. Entries - these are the logical table rows. An entry is a collection of
 *    fields that form a row in a logical table.
 * 3. Transactions - these are a collection of entries that can be bundled
 *    together for a single operation. A transaction might contain entries from
 *    multiple logical tables.
 * 4. Tables - these are the logical tables.
 *
 * A simpler mode of LT operation involves the following steps:
 * a. Create an entry.
 * b. Add the fields to the entry.
 * c. Commit the entry.
 * Nothing will be changed in the state of the system until the commit operation
 * is performed. The operations prior to the commit are merely involved in
 * creating the entry container and filling it with fields.
 * The commit operation pushes the content of the entry container onto the
 * LT in the context of the commit operation code (insert, delete, update
 * or lookup).
 * Once the commit function returns successfully, the logical table has been
 * updated or searched.
 *
 * The API allows the commit operation to be performed synchronously or
 * asynchronously. Synchronous operations block until the operation has
 * completed. Asynchronous operations return immediately and will invoke an
 * application supplied callback function as the operation progresses.
 * Asynchronous operations can perform better since they allow the application
 * to submit multiple requests without waiting for their completion. With that
 * the application can achieve a higher degree of parallel processing where one
 * thread processes the transaction and queues the generated hardware
 * instructions, while the other thread (WAL receive thread) reads the hardware
 * instructions from its queue and uses them to program the h/w.
 *
 * Transactions are more sophisticated. There are two types of
 * transactions: batch and atomic.
 * A batch transaction is simply a bundle of entries that executes sequentially.
 * A failure of one entry has no impact on the activity of other entries.
 * An atomic transaction is an all or nothing type of transaction. It means that
 * all of the entry operations are successful or none of them are applied. A failed
 * atomic transaction cleans up and leaves the system state exactly as it was
 * prior to the transaction operation.
 * Similar to entries, transactions can be committed synchronously or
 * asynchronously.
 *
 * For tables, the API provides a few distinct types of operations:
 * 1. Information - that is the ability to query the structure of the table,
 * its fields, and any restrictions associated with each field.
 * The information also contains field attributes as well as the physical
 * tables that are mapped by the LT.
 * 2. Event - the application may request to be notified when a particular
 * table changes.
 * 3. Traverse - the application may traverse the entire content of a table.
 */

/*!
 * \brief Attach a device.
 *
 * This function creates a new instance of the system manager that will
 * initialize the device. The device is fully functioning once this function
 * returns successfully.
 *
 * \param [in] unit Is the unit number of the device.
 * \param [in] warm Indicates if the operation is for cold or warm attachment.
 * In cold attach the SDK resets the device to bring all its tables into
 * default state. The SDK also sets all its resource managers into the same
 * default states.
 * In warm attach, the SDK will not reset the device keeping its operation
 * uninterrupted. The SDK will synchronize it resource managers (through HA
 * memory) to match the content of the device.
 *
 * \return SHR_E_NONE success, otherwise failure attaching a unit.
 */
extern int bcmlt_device_attach(int unit, bool warm);

/*!
 * \brief Detach a device.
 *
 * This function stops and then shuts down the device by calling the system
 * manager functions with the proper instance. All of the tables (LT or PT)
 * associated with this device will not be available once the function
 * returns successfully.
 * The function will also free all the resources associated with the unit,
 * including all the transactions and entries that were allocated for this
 * unit.
 *
 * \param [in] unit Is the unit number of the device.
 *
 * \return SHR_E_NONE success, otherwise failure detaching a unit.
 */
extern int bcmlt_device_detach(int unit);

/*!
 * \brief Check if a unit is attached.
 *
 * This function returns true is the unit, specified as input, is attached.
 *
 * \param [in] unit Is the unit number of the device.
 *
 * \return True if the unit is attached and false otherwise.
 */
extern bool bcmlt_is_unit_attached(int unit);


/*!
 * \brief Clears all inactive entries/transactions.
 *
 * This function free the resources of all the transactions and entries,
 * associated with a particular unit, that are currently not active. Active
 * transaction/entry are transactions/entries that had been committed and the
 * commit operation had not been completed.
 * Note that transactions/entries that already completed their commit processing
 * are also considered inactive and therefore will be freed.
 *
 * The purpose of this API is to enable recovery of a remote application that
 * lost connection with the SDK. Upon return, the remote application will issue
 * the API clean operation to avoid memory leaks of stale transactions/entries.
 *
 * The application must be careful not to alter an entry/transaction on one
 * thread while calling this API from another thread as there is no protection
 * between the two threads.
 *
 * \param [in] unit is the unit number of the device.
 *
 * \return SHR_E_NONE on success and SHR_E_UNIT if the unit parameter is
 * invalid.
 */
extern int bcmlt_api_clear(int unit);

/********************************************************************/
/*             F I E L D    F U N C T I O N A L I T Y               */
/********************************************************************/
/*!
 * \brief basic field information
 *
 * This data structure used to identify the field, its type and value
 */
typedef struct bcmlt_field_data_info_s {
    bool            symbol;  /*!< If true sym_val is valid                    */
    bool            array;   /*!< Indicating if the field is part of an array */
    uint32_t        idx;     /*!< If array indicating the index of the element*/
    union {
        uint64_t    val;     /*!< data is the actual data                     */
        const char  *sym_val;/*!< name is the field name                      */
    } data;          /*!< The field data portion is a union of possible types */
} bcmlt_field_data_info_t;


/*! \brief Field attribute data structure.
 *
 * This information data structure provides the callers with the details of
 * a field's attributes.
 */
typedef struct bcmlt_field_def_s {
    const char              *name;      /*!< The field name.                */
    bcmlt_field_acc_t       access;     /*!< Access type of the field.      */
    uint64_t                min;        /*!< Minimum value allowed.         */
    uint64_t                max;        /*!< Maximum value allowed.         */
    uint64_t                def;        /*!< Default value for the field.   */
    const char              *sym_def;   /*!< Default symbol value.          */
    bcmlt_field_data_tag_t  dtag;       /*!< Data tag to identify type.     */
    uint32_t                width;      /*!< Number of bits in the field.   */
    uint32_t                depth;      /*!< Number of elements in the array. */
    /*! Number of scalar elements needed for field. */
    uint32_t                elements;
    bool                    key;        /*!< True if the field part of the key. */
    bool                    array;      /*!< True if the field is an array. */
    bool                    symbol;     /*!< True if the field is a symbol. */
} bcmlt_field_def_t;

/*! Maximum length of PT map information string. */
#define BCMLT_MAPPING_STR_LEN_MAX 400

/*! Maximum length of type and handler strings. */
#define BCMLT_TYPE_STR_LEN_MAX 20

/*!
 * \brief Field map information.
 *
 * This data structure used to hold field map information.
 *
 */
typedef struct bcmlt_field_map_info_s {
    char        fmap[BCMLT_MAPPING_STR_LEN_MAX];  /*!< PT field mapping. */
    uint32_t    minbit; /*!< Minbit of field in PT. */
    uint32_t    maxbit; /*!< Maxbit of field in PT. */
} bcmlt_field_map_info_t;

/*!
 * \brief Field map handler information.
 *
 * This data structure is used to hold field map information.
 * For multiple PT mappings, the map information for each field
 * is stored in map_info.
 *
 */
typedef struct bcmlt_field_map_hdl_s {
    char        handler[BCMLT_TYPE_STR_LEN_MAX];  /*!< Handler type string. */
    /*! Internal field type string. */
    char        int_fld_type[BCMLT_TYPE_STR_LEN_MAX];
    uint32_t    num_map; /*!< Number of field mapping. */
    bcmlt_field_map_info_t map_info[]; /*!< Field map information. */
} bcmlt_field_map_hdl_t;

/*!
 * \brief Add a field to an entry.
 *
 * This function is used to add a field to an entry. The field can be up to
 * 64 bits in size. Use field array for larger fields (see
 * \ref bcmlt_entry_field_array_add)
 *
 * \param [in] entry_hdl Handle to the entry for adding the field.
 * \param [in] field_name The name of the field.
 * \param [in] data 64-bit value of the field to be added.
 *
 * \return SHR_E_NONE success, otherwise failure in adding the field to the
 * entry. SHR_E_PARAM indicates invalid entry or entry in the wrong state or
 * invalid field name. SHR_E_MEMORY indicates that there is not sufficient
 * memory to allocate resources for this action.
 */
extern int bcmlt_entry_field_add(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 uint64_t data);
/*!
 * \brief Add a symbol field to an entry.
 *
 * Symbol field values are represented as a string taken from
 * a well defined set of strings. The notion of symbols provides an
 * enumerated value for a field instead of direct integer value.
 * This function is used to add a symbol field to an entry.
 *
 * \param [in] entry_hdl Handle to the entry for adding the symbol field.
 * \param [in] field_name The name of the field.
 * \param [in] data The string value of the field to be added.
 *
 * \return SHR_E_NONE success, otherwise failure in adding the field to the
 * entry. SHR_E_PARAM indicates invalid entry or entry in the wrong state or
 * invalid field name. SHR_E_MEMORY indicates that there is not sufficient
 * memory to allocate resources for this action.
 */
extern int bcmlt_entry_field_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                        const char *field_name,
                                        const char *data);


/*!
 * \brief Unset a field from a table entry.
 *
 * This function is used to unset a field from an entry in a table. The field
 * would have been previously being set by calling \ref bcmlt_entry_field_add()
 * or \ref bcmlt_entry_field_symbol_add() followed by entry commit. This function
 * will set the field value on the LT to the field default value. This
 * operation might reduce the width of the entry, in particular for fields
 * that were defined using PDD.
 * It is important to note that this function will not clear the field from
 * the entry container. To do that the user should use the function
 * \ref bcmlt_entry_field_remove() which is the exact opposit of the functions
 * \ref bcmlt_entry_field_add() and \ref bcmlt_entry_field_symbol_add().
 *
 * \param [in] entry_hdl Handle to the entry for unsetting the field.
 * \param [in] field_name The name of the field to unset.
 *
 * \return SHR_E_NONE success.
 * \return SHR_E_PARAM invalid entry or entry in the wrong state or
 * invalid field name.
 * \return SHR_E_MEMORY there is not sufficient memory to allocate resources
 * for this action.
 */
extern int bcmlt_entry_field_unset(bcmlt_entry_handle_t entry_hdl,
                                   const char *field_name);

/*!
 * \brief Get the field value from an entry.
 *
 * This function is used to get a field value from an entry. The field can be
 * up to 64 bits in size. Use field array for larger fields (see
 * \ref bcmlt_entry_field_array_get)
 *
 * \param [in] entry_hdl Handle to the entry for getting the field.
 * \param [in] field_name The name of the field.
 * \param [out] data Up to 64-bit value of the field to get.
 *
 * \return SHR_E_NONE on success.
 * \return SHR_E_NOT_FOUND Field was not found in the entry.
 * \return SHR_E_PARAM Invalid field name or wrong entry handle or data is NULL.
 */
extern int bcmlt_entry_field_get(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 uint64_t *data);

/*!
 * \brief Get a field symbol from an entry.
 *
 * This function is used to get the value of a symbol field from an entry.
 * The field is of type symbol and its value is a string. The string is one
 * in a well defined set of strings.
 *
 * \param [in] entry_hdl Handle to the entry for getting the symbol field.
 * \param [in] field_name The name of the field to get.
 * \param [out] data The field value will be written into this parameter.
 *
 * \return SHR_E_NONE on success, SHR_E_NOT_FOUND or SHR_E_PARAM otherwise.
 */
extern int bcmlt_entry_field_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                        const char *field_name,
                                        const char **data);
/*!
 * \brief Add a field array to an entry.
 *
 * This function is used to add a field array to an entry. Each element of
 * the field array can be up to 64 bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to insert two
 * elements at index 3 & 4 the application will set \c start_idx = 3 and
 * \c num_of_elem = 2. Note that the array index starts from index 0.
 * The array might have holes.
 *
 * \param [in] entry_hdl Handle to the entry for adding the array fields.
 * \param [in] field_name The name of the field array.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] data Is an array of 64 bits value element. These values will be
 * set into the field.
 * \param [in] num_of_elem Indicates the number of array elements pointed by
 * \c data.
 *
 * \return SHR_E_NONE success, otherwise failure in adding the field to the
 * entry. SHR_E_PARAM indicates invalid entry or entry in the wrong state or
 * invalid field name. SHR_E_MEMORY indicates that there is not sufficient
 * memory to allocate resources for this action.
 */
extern int bcmlt_entry_field_array_add(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       uint64_t *data,
                                       uint32_t num_of_elem);

/*!
 * \brief Add a field symbols array to an entry.
 *
 * This is a similar function to \ref bcmlt_entry_field_array_add() except
 * that the array elements are symbols and not scalars.
 * This function is used to add a field symbols array to an entry. Each element
 * of the field array is a symbol represented by a string. This function can
 * be called multiple times with different \c start_idx values to construct a
 * complete array of elements. For example, if the application wants to insert
 * two elements at index 3 & 4 the application will set \c start_idx = 3 and
 * \c num_of_elem = 2. Note that the array index starts from index 0.
 * The array might have holes.
 *
 * \param [in] entry_hdl Handle to the entry for adding the symbol fields.
 * \param [in] field_name The name of the symbol array field.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] data Is an array of symbols (strings). These symbols will
 * be set into the field.
 * \param [in] num_of_elem Indicates the number of array elements pointed by
 * \c data.
 *
 * \return SHR_E_NONE success, otherwise failure in adding the field to the
 * entry. SHR_E_PARAM indicates invalid entry or entry in the wrong state or
 * invalid field name. SHR_E_MEMORY indicates that there is not sufficient
 * memory to allocate resources for this action.
 */
extern int bcmlt_entry_field_array_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                              const char *field_name,
                                              uint32_t start_idx,
                                              const char **data,
                                              uint32_t num_of_elem);
/*!
 * \brief Get a field array from an entry.
 *
 * This function is used to get a field array from an entry. Each element of
 * the field array may be up to 64 bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to get the values
 * of two elements in index 3 & 4 the application will set \c start_idx = 3
 * and \c num_of_elem = 2. Note that the array index starts from index 0.
 * The array might have holes.
 *
 * \param [in] entry_hdl Handle to the entry for getting the array fields.
 * \param [in] field_name The name of the array field.
 * \param [in] start_idx is the target array index where elements should start
 * being applied.
 * \param [out] data Pointer to an empty array of u64 elements. The field array
 * values will be written into the \c data array.
 * \param [in] num_of_elem Indicates the maximum number of array elements that
 * the parameter \c data can have.
 * \param [out] r_elem_cnt Read element count is the actual number of array
 * elements that were written into the \c data array.
 *
 * \return SHR_E_NONE on success, SHR_E_NOT_FOUND or SHR_E_PARAM otherwise.
 */
extern int bcmlt_entry_field_array_get(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       uint64_t *data,
                                       uint32_t num_of_elem,
                                       uint32_t *r_elem_cnt);

/*!
 * \brief Get a symbol field array from an entry.
 *
 * This function is similar to the function
 * \ref bcmlt_entry_field_array_get() except that the array is array of
 * symbols and not scalars. Each
 * element of the field array is a symbol represented by a string. This
 * function can be called multiple times with different \c start_idx values
 * to construct a complete array of elements. For example, if the application
 * wants to get the values of two elements in index 3 & 4 the application
 * will set \c start_idx = 3 and \c num_of_elem = 2. Note that the array index
 * starts from index 0.
 * The array might have holes.
 *
 * \param [in] entry_hdl Handle to the entry for getting the symbol array fields.
 * \param [in] field_name The name of the symbol array field.
 * \param [in] start_idx The target array index where elements should start
 * being applied.
 * \param [out] data Pointer to an empty array of symbol elements. The field
 * array values will be written into the \c data array. Note that the data
 * content can not be modified by the application (hence it is constant).
 * \param [in] num_of_elem Indicates the maximum number of array elements
 * that can fit into the array \c data.
 * \param [out] r_elem_cnt Read element count is the actual number of array
 * elements that were written into the data array.
 *
 * \return SHR_E_NONE on success, SHR_E_NOT_FOUND, SHR_E_PARAM or
 * SHR_E_INTERNAL otherwise.
 */
extern int bcmlt_entry_field_array_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                              const char *field_name,
                                              uint32_t start_idx,
                                              const char **data,
                                              uint32_t num_of_elem,
                                              uint32_t *r_elem_cnt);

/*!
 * \brief Remove a field from the entry container.
 *
 * Fields are identified using the field name.
 *
 * If the field exists in the entry container, it will be removed from the
 * entry. Note that this operation will not impact LT values or hardware
 * values. The field is only deleted from the entry container and not from
 * the LT. To remove a field from a particular entry in the LT use
 * \ref bcmlt_entry_field_unset().
 *
 * \param [in] entry_hdl Handle to the entry for removing the field.
 * \param [in] field_name The name of the field to remove from the entry.
 *
 * \return SHR_E_NONE success, otherwise failure in deleting the field
 * from the entry.
 */
extern int bcmlt_entry_field_remove(bcmlt_entry_handle_t entry_hdl,
                                    const char *field_name);

/*!
 * \brief Retrieves all valid symbol names for a field.
 *
 * This function retrieves all the valid symbol names for a given field. To
 * determine the number of symbols the application can call this function
 * twice. First is with \c sym_names equal to NULL to only retrieve the number
 * of symbol names available for this field. The number will be placed in
 * the output parameter \c actual_size. For the second call, the application
 * should allocate a \c sym_names array that is large enough to hold the
 * pointers for all the names. The application should make the second call
 * with the allocated array and specify the number of available elements in
 * the array.
 *
 * \param [in] entry_hdl Handle of the entry for querying the symbol field
 * information.
 * \param [in] field_name The name of the field.
 * \param [in] sym_names_size Contains the number of names that can be written
 * into the array \c sym_names.
 * \param [out] sym_names This array to place the symbol names pointer into it.
 * \param [out] actual_size This output specifies what is the actual number of
 * symbol names available for this field.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmlt_entry_field_symbol_info_get(bcmlt_entry_handle_t entry_hdl,
                                             const char *field_name,
                                             uint32_t sym_names_size,
                                             const char *sym_names[],
                                             uint32_t *actual_size);

/*!
 * \brief Traverse all the fields in an entry.
 *
 * This function traverse all the fields in an entry container. To start the
 * search the caller must start the value of the search_hdl to NULL. After the
 * first field retrieved the caller should continue to use the same value of
 * \c search_hdl has it was returned before. The function uses the value of
 * \c search_hdl for its internal context.
 * The application can continue to call this function and retrieve all the
 * fields that are currently available to the entry. After the last field was
 * traversed the function returns the error SHR_E_NOT_FOUND.
 *
 * \param [in] entry_hdl Handle of the entry to perform the traverse operation.
 * \param [out] field_name Is the next traversed field name.
 * \param [out] data Is the next traversed field data and some field attributes.
 * \param [out] search_hdl Is the traverse function context and should not be
 * changed after the first call to the field traverse function. The value of
 * this field must be set to NULL (i.e. *search_hdl = NULL) before calling the
 * field traverse function for the first time.
 *
 * \return SHR_E_NONE success.
 * \return SHR_E_NOT_FOUND After the last field was retrieved.
 * \return SHR_E_PARAM If one of the parameters is invalid.
 */
extern int bcmlt_entry_field_traverse(bcmlt_entry_handle_t entry_hdl,
                                      const char **field_name,
                                      bcmlt_field_data_info_t *data,
                                      uint32_t **search_hdl);

/*!
 * \brief Returns the number of fields in an entry container.
 *
 * This function returns the number of fields that are available within the
 * entry container. Note that each element in an array will be counted.
 *
 *
 * \param [in] entry_hdl Handle of the entry to find the field count.
 * \param [out] field_count Is the number of fields available in the entry
 * container.
 *
 * \return SHR_E_NONE success.
 * \return SHR_E_PARAM If one of the parameters is invalid.
 */
extern int bcmlt_entry_field_count(bcmlt_entry_handle_t entry_hdl,
                                   uint32_t *field_count);

/********************************************************************/
/*              E N T R Y   F U N C T I O N A L I T Y               */
/********************************************************************/

/*!
 * \brief Definition of invalid handle.
 */
#define BCMLT_INVALID_HDL   0

/*!
 * \brief Get entry information.
 *
 * Retrieve the information for a given entry. Field values can be retrieved
 * via other APIs (such as \ref bcmlt_entry_field_get()).
 * Typical use case of this function is to check the result of a previous
 * synchronous entry commit operation. Also can be used after a batch transaction
 * commit operation.
 *
 * \param [in] entry_hdl Is the handle of the entry.
 * \param [out] entry_info Is a pointer to the entry information info. The
 * entry status will be copied into this parameter.
 *
 * \return SHR_E_NONE success, SHR_E_PARAM otherwise.
 */
extern int bcmlt_entry_info_get(bcmlt_entry_handle_t entry_hdl,
                                bcmlt_entry_info_t  *entry_info);

/*!
 * \brief Allocate an entry.
 *
 * This function allocates a new table entry container for the device and table
 * type specified by the caller. The entry handle is returned in the argument.
 * Any subsequent references to the entry should be via the entry handle.
 *
 * Entry is a container of fields, fields need to be added explicitly to build
 * an entry for operation.
 *
 * The handle is only valid if the return code is successful.
 *
 * \param [in] unit The device number for which the entry is associated.
 * \param [in] table_name Unique table name(string) identifying a LT or PT.
 * \param [out] entry_hdl Pointer to return value of the allocated entry
 * handle.
 *
 * \return SHR_E_NONE success, otherwise failure in allocating the new entry.
 * Failure error codes:
 * SHR_E_INIT - The API layer has not initialized yet.
 * SHR_E_UNIT - The unit has not initialized yet.
 * SHR_E_PARAM - Missing table name,
 *             - Null of entry_hdl.
 * SHR_E_NOT_FOUND - Table not found.
 * SHR_E_MEMORY - Failed to allocate memory.
 */
extern int bcmlt_entry_allocate(int unit, const char *table_name,
                                bcmlt_entry_handle_t *entry_hdl);

/*!
 * \brief Free allocated entry.
 *
 * This function frees an allocated entry. All allocated entries must be freed
 * once the operation associated with the entry had been concluded. Failing
 * to free allocated entries will result in memory leaks.
 *
 * After freeing an entry the application should not assume the content of
 * the entry is valid. The entry can be reused by a different thread upon
 * allocation using \ref bcmlt_entry_allocate().
 *
 * An entry involved in a synchronous commit can not be freed while it is still
 * active (i.e. in commit state). However, an entry involved in an asynchronous
 * commit can be freed at any time. For asynchronous operations, freeing an entry
 * indicates to the system that once the entry completed its operation
 * (moved into idle state), it can be freed.
 * Also, entries belonging to a transaction can not be individually freed. Instead
 * they will be freed only once the transaction is freed.
 *
 * \param [in] entry_hdl Handle to the entry to be freed.
 *
 * \return SHR_E_NONE success, otherwise failure in releasing the entry.
 * Failure error codes:
 * SHR_E_PARAM - Invalid entry handle or entry in wrong state.
 * SHR_E_NOT_FOUND - Possible that the entry was already freed.
 * SHR_E_BUSY - Entry is part of a transaction.
 */
extern int bcmlt_entry_free(bcmlt_entry_handle_t entry_hdl);


/*!
 * \brief Clear entry content
 *
 * This function clears the entry from its content. The entry content will be
 * restored into the same state of the entry after calling
 * \ref bcmlt_entry_allocate().
 * All the resources that were allocated during a calls to
 * \ref bcmlt_entry_field_add() will be freed.
 * The entry must be in idle state for it to be cleared. Also the entry can't
 * be part of a transaction.
 *
 * \param [in] entry_hdl The entry handle.
 *
 * \return SHR_E_NONE success, otherwise failure in clearing the entry.
 * Failure error codes:
 * SHR_E_PARAM - Invalid entry handle or entry in wrong state.
 * SHR_E_BUSY - Entry is part of a transaction.
 */
extern int bcmlt_entry_clear(bcmlt_entry_handle_t entry_hdl);


/*!
 * \brief Entry attributes definitions.
 *
 * It is possible to assign some attributes to an entry using the function
 * \ref bcmlt_entry_attrib_set(). Setting an attribute will impact the
 * operations performed on the entry as defined in the attributes below.
 */
/*! Obtain the entry values directly from the h/w and not from cache */
#define BCMLT_ENT_ATTR_GET_FROM_HW  0x1

/*!
 * \brief Attribute to filter out fields with default values.
 *
 * Set this flag, using \ref bcmlt_entry_attrib_set(), to receive
 * only the values of the fields with non-default values. This flag applies
 * to lookup or traverse (\c BCMLT_OPCODE_LOOKUP and
 * \c BCMLT_OPCODE_TRAVERSE) operations.
 */
#define BCMLT_ENT_ATTR_FILTER_DEFAULTS  0x2

/*!
 * \brief Set entry attributes.
 *
 * This function sets entry attributes. Multiple attributes can be set within
 * a single call. This function can be called multiple times to set different
 * attributes. Attributes can not be cleared directly. To clear an attribute the
 * application can use the function \ref bcmlt_entry_clear().
 *
 * \param [in] entry_hdl The handle associated with the entry.
 * \param [in] attrib A logical or of all the attributes to set.
 *
 * \return SHR_E_NONE success.
 * \return SHR_E_PARAM - Invalid entry handle or entry in wrong state.
 */
extern int bcmlt_entry_attrib_set(bcmlt_entry_handle_t entry_hdl,
                                  uint32_t attrib);

/*!
 * \brief Get entry attributes.
 *
 * This function retrieves the entry attributes. Multiple attributes can be
 * obtained within a single call, if multiple were set.
 * Attributes can not be cleared directly. To clear an attribute the
 * application can use the function \ref bcmlt_entry_clear().
 *
 * \param [in] entry_hdl The handle associated with the entry.
 * \param [out] attrib A logical or of all the entry attributes.
 *
 * \return SHR_E_NONE success.
 * \return SHR_E_PARAM - Invalid entry handle or entry in wrong state.
 */
extern int bcmlt_entry_attrib_get(bcmlt_entry_handle_t entry_hdl,
                                  uint32_t *attrib);

/*!
 * \brief Unified operation code data type.
 */
typedef struct bcmlt_unified_opcode_s {
    bool pt;               /*!< Indicate which field in \c opcode is relevant.*/
    union {
        bcmlt_opcode_t lt;    /*!< opcode for logical table entries.          */
        bcmlt_pt_opcode_t pt; /*!< opcode for physical table entries.         */
    } opcode;              /*!< The opcode as a union of opcode types.        */
} bcmlt_unified_opcode_t;

/*!
 * \brief Retrieve the operation code of an entry.
 *
 * This function retrieve the operation code associated with the entry. This
 * functionality is mostly relevant in the case of capture replay operation (see
 * \ref bcmlt_capture_replay() and \ref replay_action_f()). However, it can also
 * been used on entries that were committed.
 *
 * \param [in] entry_hdl The handle associated with the entry.
 * \param [out] uni_opcode The operation code associated with this entry. The
 * caller should provide a pointer to the unified opcode output.
 *
 * \return SHR_E_NONE success, otherwise failure in clearing the entry.
 * \return SHR_E_PARAM - Invalid entry handle or entry in wrong state.
 */
extern int bcmlt_entry_oper_get(bcmlt_entry_handle_t entry_hdl,
                                bcmlt_unified_opcode_t *uni_opcode);

/*!
 * \brief LT Entry asynchronous commit.
 *
 * This function dispatches a single entry operation for processing.
 * This method is to be used when clients prefer individual operation
 * flexibility and control. The operation is asynchronous, meaning that
 * the function will return immediately. The application can provide a callback
 * function to monitor the progress of the operation along with the stages of
 * the processing for which it would like to be notified.
 * Options for notification are: to get notified after the entry was
 * committed into PTCache, after the hardware was modified, or both. The caller
 * may also decide to receive no notification.
 *
 * The caller context may be part of the callback if the caller provide its
 * context as an opaque pointer. This same pointer will be provided as one of
 * the callback arguments.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/alloc...
 * \param [in] user_data Opaque client context.
 * \param [in] notif_opt Notification option (after commit, h/w, both or none)
 * \param [in] notif_fn Override Callback function for status update. Can be
 * NULL if no notification needed.
 * \param [in] priority Indicates normal or high priority operation. If the
 * application commits multiple entries back to back the system will queue
 * these requests up until it has finished all previous processing. Using
 * the high priority flag will place the entry at the top of the queue.
 *
 * \return SHR_E_NONE success, otherwise failure in committing the entry
 * operation. Note that this function might return before the operation has
 * been executed. Therefore, the return value of this function only indicates
 * that the operation was successfully posted into the transaction manager for
 * execution. Once completed, the application should test the entry status to
 * determine if the operation was successful or not.
 */
extern int bcmlt_entry_commit_async(bcmlt_entry_handle_t entry_hdl,
                                    bcmlt_opcode_t opcode,
                                    void *user_data,
                                    bcmlt_notif_option_t notif_opt,
                                    bcmlt_entry_cb notif_fn,
                                    bcmlt_priority_level_t priority);

/*!
 * \brief LT Synchronous entry commit.
 *
 * This function is a synchronous version of the function
 * \ref bcmlt_entry_commit_async(). It will block the caller until the entry
 * had been processed or until an error was discovered.
 *
 * While the performance of this function is potentially slower, this is the
 * simplest form of committing an entry. If the calling thread has
 * many entries to process, the asynch model would provide better
 * performance. However, if it is not the case then this is the prefered method
 * due to its simplicity. Note that a few parallel threads that are using this
 * synchronous API will perform similarly to the asynch form.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/alloc...
 * \param [in] priority Not used.
 *
 * \return SHR_E_NONE success, otherwise failure in committing the entry
 * operation. Note, however, that this function will be successful if the
 * operation had been executed regardless the actual result of the operation.
 * It is therefore possible that the operation itself had failed and the
 * function had succeeded. For this reason it is also require to validate the
 * entry status after the function returns. Use the \ref bcmlt_entry_info_get()
 * to obtain the entry information and check the \c entry_info->status.
 *
 * \code the following code shows how to use this function to traverses all
 * the entries in the VLAN table and prints out the STG ID.
 * Note that the operation should be verified by examining the entry status.
 *
 * bcmlt_entry_handle_t e_hdl;
 * uint64_t vlan_id;
 * uint64_t stg_id;
 * bcmlt_entry_info_t e_info;
 * int rv;
 *
 * rv = bcmlt_entry_allocate(0, "VLAN", &e_hdl);
 * if (rv != SHR_E_NONE) {
 *    return rv;
 * }
 *
 * while ((rv = bcmlt_entry_commit(e_hdl,
 *                                 BCMLT_OPCODE_TRAVERSE,
 *                                 BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
 *    if (bcmlt_entry_info_get(ent_hdl, &e_info) != SHR_E_NONE ||
 *        e_info.status != SHR_E_NONE) {
 *       break;
 *    }
 *
 *    if (bcmlt_entry_field_get(e_hdl, "VLAN_ID", &vlan_id) != SHR_E_NONE) {
 *       break;
 *    }
 *    if (bcmlt_entry_field_get(e_hdl, "VLAN_STG_ID", &stg_id) != SHR_E_NONE) {
 *       break;
 *    }
 *    printf("vlan ID: %"PRId64", stg ID: %"PRId64"\n":, vlan_id, stg_id);
 * }
 *
 * bcmlt_entry_free(e_hdl);   This will stop the search
 * \endcode
 *
 */
extern int bcmlt_entry_commit(bcmlt_entry_handle_t entry_hdl,
                              bcmlt_opcode_t opcode,
                              bcmlt_priority_level_t priority);


/*!
 * \brief PT Entry asynchronous commit
 *
 * This function is the PT version of \ref bcmlt_entry_commit_async(). It
 * differs from the LT version by the opcode and notification function parameters
 * but behaves similarly. The function is a passthough meaning that the only true
 * notification is h/w complete as the entry passes though the PTCache stage.
 *
 * Note that after using this function it is possible that the application can
 * not continue its normal operation unless the operations are reversed.
 * That is since the device state and the PTCache state might be different.
 *
 * This is an asynchronous API meaning that the function returns before the
 * processing occurs. The caller, if desired, should specify the notification
 * option and callback function to be notified about the progress and success/
 * failure of the operation.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/modify...
 * \param [in] user_data Opaque client context.
 * \param [in] notif_opt Notification option (after commit, h/w, both or none)
 * \param [in] notif_fn Override Callback function for status update.
 * \param [in] priority Indicates normal or high priority operation. If the
 * application commits multiple entries back to back, the system will queue
 * these requests up until it has finished all previous processing. Using
 * the high priority flag will place the entry at the top of the queue.
 *
 * \return SHR_E_NONE success, otherwise failure in committing the entry
 * operation. Note, that this function might return before the operation has
 * been executed. Therefore, the return value of this function only indicates
 * that the operation was successfully posted into the transaction manager for
 * execution. Once completed, the application should test the entry status to
 * determine if the operation was successful or not.
 */
extern int bcmlt_pt_entry_commit_async(bcmlt_entry_handle_t entry_hdl,
                                       bcmlt_pt_opcode_t opcode,
                                       void *user_data,
                                       bcmlt_notif_option_t notif_opt,
                                       bcmlt_entry_pt_cb notif_fn,
                                       bcmlt_priority_level_t priority);

/*!
 * \brief PT Synchronous entry commit.
 *
 * This function is the PT version of \ref bcmlt_entry_commit(). It
 * only differs in the opcode parameter but behaves similarly otherwise.
 * Typical use of this function is for debug purposes as this function bypasses
 * (passthough) the PTCache and access the h/w directly. The caller must have
 * good knowledge of the underlining device to use this function.
 *
 * Note that after using this function it is possible that the application can
 * not continue its normal operation unless the operations are reversed.
 * That is since the device state and the PTCache state might be different.
 *
 * This function will block until the operation had completed.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/modify...
 * \param [in] priority Unused parameter.
 *
 * \return SHR_E_NONE success, otherwise failure in committing the entry
 * operation. Note, however, that this function will return successful if the
 * operation is executed regardless the actual result of the operation.
 * It is therefore possible that the operation itself can fail and the
 * function succeed. For this reason it is also required to validate the
 * entry status after the function returns. Use the \ref bcmlt_entry_info_get()
 * to obtain the entry information that contains the operation status.
 */
extern int bcmlt_pt_entry_commit(bcmlt_entry_handle_t entry_hdl,
                                 bcmlt_pt_opcode_t opcode,
                                 bcmlt_priority_level_t priority);


/********************************************************************/
/*              T A B L E   F U N C T I O N A L I T Y               */
/********************************************************************/
/*!
 * This group of functions provide table based functionality. These are
 * functions that can operate on an entire table or tables.
 */
/*!
 * \brief Table search criteria.
 *
 * The following flags can be used to control the output of the table search
 * functions.
 * Multiple flags can be used together. For example to find all the PT and LT
 * tables the user should provide
 * BCMLT_TABLE_SEARCH_PT | BCMLT_TABLE_SEARCH_LT as input
 */
#define BCMLT_TABLE_SEARCH_PT            0x1  /*!< Search PT tables          */
#define BCMLT_TABLE_SEARCH_LT            0x2  /*!< Search logical tables     */
#define BCMLT_TABLE_SEARCH_INTERACTIVE   0x4  /*!< Search interactive tables */

/*!
 * \brief Start table enumeration.
 *
 * This function starts the retrieval of all available logical or physical
 * table names on a particular device. The function returns the name of the
 * first table in the list of tables. The caller can set the proper search
 * criteria by setting the \c flags parameter.
 *
 * To retrieve the rest of the tables, one by one, use the function
 * \ref bcmlt_table_names_get_next().
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] flags Control the search results by selecting the desired table
 * types to be enumerated.
 * \param [out] name The name of the table that was retrieved first.
 *
 * \return SHR_E_NONE success, otherwise proper error code.
 * Potential error codes are:
 * SHR_E_UNIT - Unit is not attached.
 * SHR_E_PARAM - Name input parameter was NULL.
 * SHR_E_NOT_FOUND - No table was found.
 */
extern int bcmlt_table_names_get_first(int unit,
                                       uint32_t flags,
                                       char **name);

/*!
 * \brief Continue table enumeration.
 *
 * This function continues the retrieval of all available logical or physical
 * table names on a particular device. This function should be called after a
 * call to \ref bcmlt_table_names_get_first() to retrieve the first table. The
 * caller can control the type of tables to retrieve via the \c flags parameter.
 *
 * This function returns the name of the next table.
 * Use this function again to retrieve more names.
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] flags Control the search results by selecting the desired table
 * types to be enumerated.
 * \param [out] name The name of the table that was retrieved.
 *
 * \return SHR_E_NONE success, otherwise proper error code.
 * Potential error codes are:
 * SHR_E_UNIT - Unit is not attached.
 * SHR_E_PARAM - Name input parameter was NULL.
 * SHR_E_NOT_FOUND - No table was found.
 */
extern int bcmlt_table_names_get_next(int unit,
                                      uint32_t flags,
                                      char **name);


/*!
 * \brief Retrieve table field definitions.
 *
 * This function retrieves all of the field information associated with
 * a particular table on a particular device
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] table_name Table name for subscribing to updates.
 * \param [in] fields_array_size Specifies the number of elements
 *              available in the array field_defs_array.
 * \param [out] field_defs_array Pointer to user provided buffer where the
 *              field definitions will be copied too. The application must
 *              set the num_fields to indicate the number
 *              of fields that can be placed within entry_hdl buffer.
 * \param [out] require_array_size Used by the function to place the required
 *              array size in term of elements
 *              (byte size = sizeof(bcmlt_field_def_t) * require_array_size)
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining the table fields.
 *
 * \code the following code shows how to use this function. It calls the
 * function \c bcmlt_table_field_defs_get() twice. Once to retrieve the number
 * of fields in the table, and a second time to retrieve the field definition.
 * Assume that the unit and t_name (table name) provided to this example
 *
 * uint32_t number_of_elements;
 * bcmlt_field_def_t *buffer;
 * bcmlt_field_def_t *field_def;
 * uint32_t actual_number;
 * int rv;
 * uint32_t idx;
 * rv = bcmlt_table_field_defs_get(unit, t_name, 0, &number_of_elements);
 * if (rv != SHR_E_NONE) {
 *    return rv;
 * }
 * buffer = malloc(number_of_elements * sizeof(bcmlt_field_def_t));
 * if (!buffer) {
 *    return SHR_E_MEMORY;
 * }
 * rv = bcmlt_table_field_defs_get(unit,
 *                                 t_name,
 *                                 number_of_elements,
 *                                 buffer,
 *                                 &actual_number);
 * if ((rv != SHR_E_NONE) || (actual_number != number_of_elements)) {
 *    return SHR_E_INTERNAL;
 * }
 * for (idx = 0, field_def = buffer;
        idx < number_of_elements;
        idx++, field_def++) {
 * }
 * free (buffer);
 * return SHR_E_NONE;
 * \endcode
 *
 */
extern int bcmlt_table_field_defs_get(int unit,
                                      const char *table_name,
                                      uint32_t    fields_array_size,
                                      bcmlt_field_def_t *field_defs_array,
                                      uint32_t *require_array_size);

/*
 * Table Notification APIs
 */

/*!
 * \brief Subscribe for table updates.
 *
 * This function registers a callback to get notified on any changes of a
 * particular table within a unit.
 * The application may provide a pointer to an application context that can be
 * used during the callback event.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] table_name The table name for subscribing to updates.
 * \param [in] callback Notification handler to be invoked.
 * \param [in] user_data Application opaque context for notifications.
 *
 * \return SHR_E_NONE success, otherwise failure in subscribing to table
 * updates.
 */
extern int bcmlt_table_subscribe(int unit,
                                 const char *table_name,
                                 bcmlt_table_sub_cb callback,
                                 void *user_data);

/*!
 * \brief Unsubscribe for table updates.
 *
 * This function deregisters a callback to get notified on any table updates.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] table_name The table name for unsubscribing from any updates.
 *
 * \return SHR_E_NONE success, otherwise failure in unsubscribing to table
 *          updates.
 */
extern int bcmlt_table_unsubscribe(int unit, const char *table_name);

/********************************************************************/
/*        T R A N S A C T I O N S   F U N C T I O N A L I T Y       */
/********************************************************************/
/*
 * Transaction is a collection of entries that can be bundled together.
 * The execution of the entries wil be done sequentially according to the order
 * they were inserted into the transaction.
 * There are two types of transactions: Atomic and Batch
 * A batch transaction simply stages and commits every entry in the transaction
 * according to the entry order. If an entry operation fails, the transaction
 * simply jumps to the next entry.
 * An atomic transaction is all or nothing, meaning that either all of the
 * entries are staged correctly or the system is placed in the state that
 * proceeded the call to committing the transaction. Unlike a batch transaction
 * that stages and commits each entry seperately, an atomic transaction only
 * stages each entry seperately but commits the entire transaction in a single
 * step.
 *
 * The operational flow of a transaction is an extension to the entry operation.
 * A typical transaction operation sequence looks as follows:
 * a. Allocate the transaction.
 * b. Allocate an entry.
 * c. Set the entry fields.
 * d. Add the entry into the transaction. The desired entry operation is
 *    specified here.
 * e. If more operations are required go back to step (b).
 * f. Commit the transaction (synchronous or asynchronous).
 * g. Check the transaction state and individual entries (if needed).
 * h. Free the transaction.
 */
/*!
 * \brief Transaction allocation.
 *
 * This function allocates a new transaction resources for a type of
 * transaction specified by the caller. The transaction handle is returned in
 * the argument.
 *
 * The handle is only valid on successful call return values.
 * The transaction is a container that may contain multiple entries.
 * Note that to prevent memory leaks the caller should free the transaction
 * once it completed its operations using the function
 * \ref bcmlt_transaction_free().
 *
 * \param [in] type The transaction type.
 * \param [out] trans_hdl Pointer to the variable to hold the transaction
 *                      handle. A valid pointer to a transaction handle
 *                      storage location is required. The contents of
 *                      entry_hdl location pointed at will be updated with
 *                      the valid transaction handle on success.
 *
 * \return SHR_E_NONE success, otherwise one of the following error code will
 * be return:
 * SHR_E_INIT - Indicates that the API layer had not been started yet.
 * SHR_E_PARAM - Indicates that the type parameter is invalid.
 * SHR_E_MEMORY - Indicates that there is insufficient memory or the number
 * of allocated transactions exceeded the specified limit (configuration).
 */
extern int bcmlt_transaction_allocate(bcmlt_trans_type_t type,
                                      bcmlt_transaction_hdl_t *trans_hdl);


/*!
 * \brief Free the transaction.
 *
 * This function frees a previously allocated transaction. Failure to free
 * allocated transactions leads to memory leaks.
 *
 * \param [in] trans_hdl The handle to the transaction to be released.
 *
 * \return SHR_E_NONE success, otherwise failure in releasing the transaction.
 */
extern int bcmlt_transaction_free(bcmlt_transaction_hdl_t trans_hdl);


/*!
 * \brief Add an entry to a transaction.
 *
 * This function inserts a single entry operation into a transaction. The
 * transaction needs to be committed to take effect. Upon transaction commit,
 * the entries will be staged in the same order as they were added to the
 * transaction.
 * The transaction state should be idle for this function to succeed.
 * If the transaction is in commit state the operation will fail.
 *
 * The current implementation only supports entries belonging to the same
 * unit.
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] opcode The LT operations code (i.e. insert/lookup/update...).
 * \param [in] entry_hdl The entry handle that was obtained by calling
 * \ref bcmlt_entry_allocate().
 *
 * \return SHR_E_NONE success, otherwise error code will be returned. Possible
 * error codes are:
 * SHR_E_PARAM - may occur if the transaction or entry handles are invalid.
 * May also occur if the entry is a PT entry or if the entry already belongs to a
 * transaction (same or different). This error also occurs if the op code is
 * invalid or if the transaction already contains entry of different table type
 * (i.e. can't mix PT entries with LT entries).
 * SHR_E_UNAVAIL - will occur if attempting to insert an entry from
 * a different unit that the transaction associated with. The transaction is
 * automatically associated with the unit of the first entry.
 */
extern int bcmlt_transaction_entry_add(bcmlt_transaction_hdl_t  trans_hdl,
                                       bcmlt_opcode_t    opcode,
                                       bcmlt_entry_handle_t  entry_hdl);

/*!
 * \brief Add a PT entry to a transaction.
 *
 * This function is similar to the function \ref bcmlt_transaction_entry_add()
 * except that it applied for pass through entries.
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] opcode The PT operation code (i.e. set/modify/get...).
 * \param [in] entry_hdl The entry handle that was obtained by calling
 * \ref bcmlt_entry_allocate().
 *
 * \return SHR_E_NONE success, otherwise error code will be returned. Possible
 * error codes are:
 * SHR_E_PARAM - may occur if the transaction or entry handles are invalid.
 * May also occur if the entry is a PT entry or if the entry already belongs to a
 * transaction (same or different). This error also occurs if the op code is
 * invalid or if the transaction already contains an entry of a different table
 * type (i.e. can't mix PT entries with LT entries).
 * SHR_E_UNAVAIL - Occurs if attempting to insert and entry from
 * a different unit than the transaction is associated with. The transaction is
 * automatically associated with the unit of the first entry.
 */
extern int bcmlt_transaction_pt_entry_add(bcmlt_transaction_hdl_t  trans_hdl,
                                          bcmlt_pt_opcode_t    opcode,
                                          bcmlt_entry_handle_t  entry_hdl);

/*!
 * \brief Remove an entry from a transaction.
 *
 * This function removes a single entry operation from the transaction. This can
 * be done only on transactions in an idle state (either before committing or
 * after the transaction is committed). This allows client code to modify
 * the transaction with complete flexibility until the logic is ready to commit
 * the transaction.
 * This function is the reverse of the functions
 * \ref bcmlt_transaction_entry_add and \ref bcmlt_transaction_pt_entry_add().
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] entry_hdl The entry handle that was obtained by calling
 * \ref bcmlt_entry_allocate().
 *
 * \return SHR_E_NONE success, otherwise error code will be returned. Possible
 * error codes are:
 * SHR_E_PARAM - may occur if the transaction or entry handles are invalid.
 * May also occur if the entry was not found.
 */
extern int bcmlt_transaction_entry_delete(bcmlt_transaction_hdl_t  trans_hdl,
                                          bcmlt_entry_handle_t  entry_hdl);

/*!
 * \brief Get transaction information.
 *
 * Retrieve the transaction information for the given transaction.
 * Individual entries can be retrieved via the function
 * \ref bcmlt_transaction_entry_num_get().
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [out] trans_info The transaction information. See
 * bcmlt_transaction_info_t for more information.
 *
 * \return SHR_E_NONE success, otherwise failure in retrieving the
 * transaction information.
 */
extern int bcmlt_transaction_info_get(bcmlt_transaction_hdl_t trans_hdl,
                                      bcmlt_transaction_info_t  *trans_info);


/*!
 * \brief Transaction asynchronous commit.
 *
 * This function commits a transaction for asynchronous processing.
 * On successful return from this call, clients are assured the transaction
 * is queued for processing. Since this function is asynchronous, the API
 * enables the application to trace the progress of this operation by receiving
 * a callback for the specified event (see bcmlt_trans_cb()).
 *
 * User_data can be used as context for application callback events
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] notif_opt Notification option for transaction.
 * \param [in] user_data  Caller context to be provided with the event callback
 * function.
 * \param [in] notif_fn  Processing progress callback function.
 * This function will be called on transaction status updates for events
 * requested with the \c notif_opt parameter.
 * \param [in] priority Indicates normal or high priority operation. High
 * priority operations are queued in front of all normal priority transactions.
 *
 * \return SHR_E_NONE success, otherwise failure in committing the transaction.
 * Note, that this function might return before the operation has
 * been executed. Therefore, the return value of this function only indicates
 * that the transaction was successfully posted into the transaction manager
 * for execution. Once completed, the application should test the transaction
 * status to determine if the operation was successful or not (see
 * \ref bcmlt_transaction_info_get()). If the transaction status is partially
 * successful the application should check the individual entries status
 * (for batch transactions) to determine which one failed.
 */
extern int bcmlt_transaction_commit_async(bcmlt_transaction_hdl_t trans_hdl,
                                          bcmlt_notif_option_t notif_opt,
                                          void *user_data,
                                          bcmlt_trans_cb notif_fn,
                                          bcmlt_priority_level_t priority);

/*!
 * \brief Transaction synchronous commit.
 *
 * This function commits a transaction. Since this is a synchronous
 * operation the function will block until all entries in the transaction are
 * committed into the hardware.
 *
 * This function handles all types of transactions.
 *
 * Upon return, the application can query the success of the transaction
 * operation as well as the status of each entry. For more information see
 * \ref bcmlt_transaction_info_get() and \ref bcmlt_transaction_entry_num_get().
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] priority Unused.
 *
 * \return SHR_E_NONE success, otherwise failure in committing the transaction.
 * Note, however, that this function returns success if the
 * transaction is executed regardless the actual result of the
 * operations. It is therefore possible that the operation itself fails
 * and the function succeeds. For this reason it is also required to
 * validate the transaction status after the function returns (see
 * \ref bcmlt_transaction_info_get()). If the status is partial success (for
 * batch transactions) use the \ref bcmlt_entry_info_get() to obtain
 * the information of every entry to determine which one had failed.
 */
extern int bcmlt_transaction_commit(bcmlt_transaction_hdl_t trans_hdl,
                                    bcmlt_priority_level_t priority);


/*!
 * \brief Get the n'th entry of a transaction.
 *
 * This function enables the caller to control each specific entry in a
 * transaction. By obtaining the entry handle the caller can verify the entry
 * operational status as well as to obtain fields values for lookup operations.
 *
 * \param [in] trans_hdl Handle to the transaction.
 * \param [in] entry_num Nth entry to retrieve.
 * \param [out] entry_info Pointer to memory containing nth entry information.
 *
 * \return SHR_E_NONE success, otherwise failure in retrieving the n'th entry of
 * the transaction. The following error codes can be returned:
 * SHR_E_PARAM - This error code may accur in multiple scenarios such as:
 *  - Invalid transaction handle.
 *  - entry_info is NULL (invalid).
 *  - The transaction is not in idle state.
 *  - Entry number doesn't exist.
 */
extern int bcmlt_transaction_entry_num_get(bcmlt_transaction_hdl_t trans_hdl,
                                           uint32_t entry_num,
                                           bcmlt_entry_info_t *entry_info);


/*!
 * \brief Replay functions (debug).
 *
 * This set of functions can be used to enable operation capture and replay.
 * The main purpose of these functions is to be able to reproduce specific state
 * of the system for debug purposes.
 * The capture function uses the application IO functions to save each
 * commit operation in non-volatile memory.
 * The replay function will replay the captured operations in the exact order
 * (and possibly with the exact timing).
 */
/*!
 * \brief BCMLT operations capture callback structure.
 *
 * This data structure should be passed to the capture start function. The
 * purpose of this structure is to enable the capture function to perform
 * IO operations that are outside the scope of the SDK. These operations
 * includes in particular the ability to save the captured data into
 * non-volatile memory where it can be read back at a later time.
 */
typedef struct bcmlt_capture_cb_s {
    /*! File descriptor of a file with write permission */
    void *fd;
    /*!
      * Write function to write a number of bytes (nbyte) from the buffer into
      * the file using the file descriptor fd. The function returns the number
      * of bytes that were actually written.
      */
    uint32_t (*write)(void *fd, void *buffer, uint32_t nbyte);
} bcmlt_capture_cb_t;

/*!
 * \brief BCMLT operations replay callback structure.
 *
 * This data structure should be passed to the replay function. The
 * purpose of this structure is to enable the replay function to perform
 * IO operations that are outside the scope of the SDK. These operations
 * includes in particular reading the captured data.
 */
typedef struct bcmlt_replay_cb_s {
    /*! File descriptor of a file to replay with read permission */
    void *fd;
    /*!
      * Read function to read number of bytes (nbyte) from the file into
      * the buffer. The function returns the number of bytes that were
      * actually read.
      */
    uint32_t (*read)(void *fd, void *buffer, uint32_t nbyte);
} bcmlt_replay_cb_t;

/*!
 * \brief Start operation capturing.
 *
 * This function starts the capture of all the entry and transaction
 * operations for a given unit. The capture of the activity will continue
 * until the application calls \ref bcmlt_capture_stop(). After
 * that the application can play back the entire captured sequence by
 * calling \ref bcmlt_capture_replay().
 * The application should provide the file descriptor and a write function to
 * be used to store the trace information. If the file descriptor represents
 * a file the application should make sure to truncate older file content.
 * Note that the content of the file will be binary data.
 *
 * \param [in] unit The unit number of the device for which to start
 * capturing.
 * \param [in] app_cb A data structure that provides the IO
 * instrumentation required by the capture functionality. This functionality
 * is implemented by the application.
 *
 * \return SHR_E_NONE success, error code otherwise. Potential error codes:
 * SHR_E_UNIT - Invalid unit parameter.
 * SHR_E_BUSY - Currently capturing or playing back the activity for this unit.
 */
extern int bcmlt_capture_start(int unit, bcmlt_capture_cb_t *app_cb);

/*!
 * \brief Stop operations capture.
 *
 * This function should stop operations capturing on the specified unit.
 * The operations capture should start by calling
 * \ref bcmlt_capture_start().
 * Note that calling \ref bcmlt_capture_start() again after calling this
 * function will erase the entired captured history for this device.
 *
 * \param [in] unit The unit number of the device for which to stop
 * capturing
 *
 * \return SHR_E_NONE success, error code otherwise. Potential error codes:
 * SHR_E_UNIT - Invalid unit parameter.
 */
extern int bcmlt_capture_stop(int unit);

/*!
 * \brief BCMLT Generic object handle.
 *
 * This data structure contains the handle of entry or transaction objects.
 * The entry field indicates of the relevant portion of the \c hdl field is
 * an entry or trans.
 */
typedef struct bcmlt_object_hdl_t {
    bool entry;          /*!< Selector indicating if the hdl is entry or not. */
    union {
        bcmlt_entry_handle_t entry;  /*!< An entry handle                     */
        bcmlt_transaction_hdl_t trans; /*!< A transaction handle              */
    } hdl;               /*!< Object handle can be entry or transaction       */
} bcmlt_object_hdl_t;

/*!
 * \brief Pre-Commit notification for replay object.
 *
 * This callback function can be passed to the function
 * \ref bcmlt_capture_replay to receive the handle of the object that is
 * going to be replayed. The object can be a transaction or an entry.
 * From the callback the user can further query the object and to obtain all
 * the details associated with the objects. The following functions should
 * be useful for this effort:
 * \ref bcmlt_entry_field_traverse() to extract the fields and their data from
 * the entry.
 * \ref bcmlt_entry_info_get() to obtain entry and table information.
 * \ref bcmlt_transaction_info_get() to obtain the transaction type and number
 * of entries in the transaction.
 * \ref bcmlt_transaction_entry_num_get() to obtain the handle for a particular
 * entry in a transaction.
 * \ref bcmlt_entry_oper_get() to obtain the operation associated with the
 * entry.
 */
typedef void (replay_action_f)(int unit, bcmlt_object_hdl_t *object);

/*!
 * \brief Play back previously captured operations
 *
 * This function will play back the exact same sequence of operations that
 * was previously captured using \ref bcmlt_capture_start() and
 * \ref bcmlt_capture_stop() functions. The playback maintains the
 * timing of the original operation (and will therefore takes the same time
 * to execute), unless the parameter \c timing is set to false.
 *
 * \param [in] timing Timing set to true to maintain similar timing as the
 * original operations (max delay between operations is 1 second).
 * Otherwise, the execution will be as soon as possible.
 * \param [in] cb Callback function to be called by the playback engine for
 * every entry or transaction that is about to execute. This function may use
 * other APIs to retrieve and display the content of the entry/transaction in
 * a human readable format. See \ref replay_action_f for more information.
 * \param [in] view_only Control if the actions will be install on the device
 * or if they will be retrieved for information only (see \c cb parameter).
 * \param [in] app_cb The app_cb is a data structure that provides the IO
 * instrumentation required by the capture replay function. This functionality
 * is implemented by the application.
 *
 * \return SHR_E_NONE success, error code otherwise. Potential error codes:
 * SHR_E_PARAM - Invalid app_cb parameter.
 */
extern int bcmlt_capture_replay(bool timing,
                                replay_action_f *cb,
                                bool view_only,
                                bcmlt_replay_cb_t *app_cb);

/*!
 * \brief Retrieve physical table names associated with a given LT.
 *
 * Retrieve the physical table information for the LT
 * which have direct physical table mapping.
 * For LTs with CTH, PT count is set to 0.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name(string).
 * \param [in,out] pt_counter_ptr The input value indicates the
 * array size of \c pt_names. The output value indicates
 * how many elements in the array \c pt_names were used.
 * \param [out] pt_names Pointer to the list of PT names.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_FULL when number of mapped physical tables
 *  is more than array size of \c pt_names.
 * \retval SHR_E_PARAM on NULL table or field name.
 * \retval SHR_E_INIT on sanity check failure.
 * \retval SHR_E_UNIT for invalid unit number.
 */
extern int bcmlt_table_pt_name_get(int unit,
                                   const char *tbl_name,
                                   uint32_t *pt_counter_ptr,
                                   const char *pt_names[]);

/*!
 * \brief Retrieve physical field mapping information.
 *
 * Retrieve the physical field information for the field
 * in LT which has direct physical table mapping.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name(string).
 * \param [in] fld_name The field name(string).
 * \param [in,out] instance For input, this parameter indicates
 * how many elements are in the array \c fld_info.
 * Upon return, this parameter contains the actual number
 * of field maps that were used.
 * \param [in,out] fld_info Pointer array to field map information.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_FULL for instance > number of elements in \c fld_info.
 * \retval SHR_E_PARAM on NULL table or field name.
 * \retval SHR_E_INIT on sanity check failure.
 * \retval SHR_E_UNIT for invalid unit number.
 */
extern int bcmlt_field_pt_map_get(int unit,
                                  const char *tbl_name,
                                  const char *fld_name,
                                  uint32_t *instance,
                                  bcmlt_field_map_hdl_t *fld_info);

/*!
 * \brief Retrieve logical table description.
 *
 * Retrieve the description for given logical table.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name.
 * \param [out] tbl_desc Logical table description.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM on NULL table name.
 * \retval SHR_E_NOT_FOUND when table is not found for device.
 * \retval SHR_E_INIT on sanity check failure.
 * \retval SHR_E_UNIT for invalid unit number.
 */
extern int bcmlt_table_desc_get(int unit,
                                const char *tbl_name,
                                const char **tbl_desc);

/*!
 * \brief Retrieve field description of logical table.
 *
 * Retrieve the description for given field in the logical table.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name.
 * \param [in] fld_name The field name.
 * \param [out] fld_desc Field description.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM on NULL table name.
 * \retval SHR_E_NOT_FOUND when table/field is not found for device.
 * \retval SHR_E_INIT on sanity check failure.
 * \retval SHR_E_UNIT for invalid unit number.
 */
extern int bcmlt_field_desc_get(int unit,
                                const char *tbl_name,
                                const char *fld_name,
                                const char **fld_desc);

#endif /* BCMLT_H */
