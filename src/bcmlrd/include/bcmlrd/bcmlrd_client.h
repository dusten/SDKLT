/*! \file bcmlrd_client.h
 *
 * \brief Logical Table Resource Database Client API interfaces
 *
 * APIs tailored for Client API needs
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

#ifndef BCMLRD_CLIENT_H
#define BCMLRD_CLIENT_H

#include <bcmltd/bcmltd_lt_types.h>
#include <bcmlrd/bcmlrd_table.h>

/*!
 * \brief Logical table attribute
 *
 * High level table attributes
 */
typedef struct bcmlrd_table_attrib_s {
    /*! Table ID. */
    bcmlrd_sid_t id;

    /*! The type of the memory associated with the table. */
    enum {HASH, INDEX, TCAM} type;

    /*! Maximum number of entries. */
    size_t    max_entries;

    /*! Table access type. */
    enum {READ_ONLY, READ_WRITE} access_type;

    /*! Number of fields in table. */
    size_t    number_of_fields;

    /*! True for interactive table. */
    bool interactive;
} bcmlrd_table_attrib_t;

/*!
 * \brief Logical table field information.
 *
 * Logical table field information suitable for client APIs.
 */
typedef bcmltd_field_def_t bcmlrd_client_field_info_t;

/*!
 * \brief Get all table attributes for a table.
 *
 * Get a list of all table attributes for a given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Table name.
 * \param [out] attrib          Attributes.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_attributes_get(int unit,
                            const char *table_name,
                            bcmlrd_table_attrib_t *attrib);

/*!
 * \brief Get the field definition for a table.
 *
 * Get a field definition for a given table.
 *
 * \param [in]  unit                    Unit number.
 * \param [in]  table_name              Table name.
 * \param [in]  number_of_fields        Number of fields alloc'ed in fields_info.
 * \param [out] fields_info             Array of field information.
 * \param [out] actual                  Number of fields in fields_info.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_fields_def_get(int unit,
                            const char *table_name,
                            size_t number_of_fields,
                            bcmlrd_client_field_info_t *fields_info,
                            size_t *actual);

/*!
 * \brief Get the field definition for a table.
 *
 * Get a table definition for a given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  num             Table name.
 * \param [out] names           Array of name pointers.
 * \param [out] actual          Number of names returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_table_names_get(int unit, size_t num, const char *names[],size_t *actual);

/*!
 * \brief Convert field symbol string to value.
 *
 * Convert a field symbol string to a uint32_t value.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [in]  sym             Field symbol.
 * \param [out] val             Field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_symbol_to_value(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             const char *sym,
                             uint32_t *val);

/*!
 * \brief Convert field value to symbol string.
 *
 * Convert a field symbol string to a uint32_t value.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [in]  sym             Field symbol.
 * \param [out] val             Field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_value_to_symbol(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             uint32_t val,
                             const char **sym);


/*!
 * \brief Get number of symbol values for a field.
 *
 * Get the number of symbol values for a given field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [out] count           Number of symbols.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_symbols_count_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmlrd_fid_t fid,
                               size_t *count);


/*!
 * \brief Get symbol values for a field.
 *
 * Get all of the symbol values for a given field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table symbol ID.
 * \param [in]  fid             Table field ID.
 * \param [in]  num             Size of symbol map array.
 * \param [out] symbol_map      Array of symbol maps.
 * \param [out] actual          Number of symbol maps returned.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_symbols_get(int unit,
                         bcmlrd_sid_t sid,
                         bcmlrd_fid_t fid,
                         size_t num,
                         shr_enum_map_t symbol_map[],
                         size_t *actual);

/*!
 * \brief Get maximum number of symbol values for any fields.
 *
 * Get the maximum number of symbol values for any field.
 *
 * \param [out] count           Number of symbols.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmlrd_field_max_symbols_count_get(size_t *count);


/*!
 * \brief Return the mapped field data.
 *
 * Get a pointer to the mapped field data for the given unit, sid, and fid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical Table symbol ID.
 * \param [in]  fid             Logical Field ID.
 * \param [out] field           Pointer to returned field data.
 *
 * \retval SHR_E_NONE           Returned field data.
 * \retval SHR_E_UNAVAIL        unit/table/field not found.
 */
extern int bcmlrd_field_get(int unit,
                            bcmlrd_sid_t sid,
                            bcmlrd_fid_t fid,
                            const bcmlrd_field_data_t **field);

#endif /* BCMLRD_CLIENT_H */
