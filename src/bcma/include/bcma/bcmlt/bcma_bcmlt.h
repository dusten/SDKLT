/*! \file bcma_bcmlt.h
 *
 * Functions related to bcmlt module.
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

#ifndef BCMA_BCMLT_H
#define BCMA_BCMLT_H

#include <shr/shr_error.h>
#include <shr/shr_pb.h>

#include <bcma/cli/bcma_cli.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmlt/bcmlt.h>

/*! Format to display shr_errmsg with error code. */
#define BCMA_BCMLT_ERRMSG_FMT             "%s (error code %d)"

/*!
 * Macro used in print data function combined with the format
 * \ref BCMA_BCMLT_ERRMSG_FMT to display shr_errmsg with error code.
 */
#define BCMA_BCMLT_ERRMSG(_err)           shr_errmsg(_err), _err

/*! Table entry operation result display mode. */
typedef enum bcma_bcmlt_display_mode_e {
    BCMA_BCMLT_DISP_MODE_HEX_DEC = 0,
    BCMA_BCMLT_DISP_MODE_HEX = 1,
    BCMA_BCMLT_DISP_MODE_DEC = 2
} bcma_bcmlt_display_mode_t;

/*! Encode field and array field display mode into a single display word. */
#define BCMA_BCMLT_DISP_WORD_SET(_field_disp, _array_field_disp) \
    ((_field_disp) | (_array_field_disp) << 8)

/*! Get field display mode from display word. */
#define BCMA_BCMLT_FIELD_DISP_MODE_GET(_disp_word) \
    ((_disp_word) & 0xff)

/*! Get array field display mode from display word. */
#define BCMA_BCMLT_ARRAY_FIELD_DISP_MODE_GET(_disp_word) \
    (((_disp_word) >> 8) & 0xff)

/*! Match mode option in \ref bcma_bcmlt_tables_search function. */
typedef enum bcma_bcmlt_match_mode_e {
    BCMA_BCMLT_MATCH_EXACT = 1,
    BCMA_BCMLT_MATCH_SUBSTR = 2
} bcma_bcmlt_match_mode_t;

/*! Flag to identify table search scope including logical table. */
#define BCMA_BCMLT_SCOPE_F_LTBL           (1 << 0)

/*! Flag to identify table search scope including physical table. */
#define BCMA_BCMLT_SCOPE_F_PTBL           (1 << 1)

/*!
 * \name Entry operation options
 * \anchor BCMA_BCMLT_TBL_LIST_OPT_xxx
 */

/*! \{ */

/*! Flag to display only field names of logical tables. */
#define  BCMA_BCMLT_TBL_LIST_OPT_FIELD_NAME_ONLY     (1 << 0)

/*! Flag to display logical info of fields in logical tables. */
#define  BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO          (1 << 1)

/*! Flag to display physical info of fields in logical tables. */
#define  BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO       (1 << 2)

/*! Flag to display description of logical tables and fields. */
#define  BCMA_BCMLT_TBL_LIST_OPT_DESC                (1 << 3)

/*! \} */

/*!
 * \name Entry operation options
 * \anchor BCMA_BCMLT_ENTRY_OP_OPT_xxx
 */

/*! \{ */

/*! Perform INSERT operation after an entry not found UPDATE. */
#define BCMA_BCMLT_ENTRY_OP_OPT_INS_UPDT  (1 << 0)

/*! Automatically assign unspecified key fields with default values. */
#define BCMA_BCMLT_ENTRY_OP_OPT_AUTO_KFS  (1 << 1)

/*! Perform LOOKUP operation for each traversed entry. */
#define BCMA_BCMLT_ENTRY_OP_OPT_TRVS_LKUP (1 << 2)

/*! Perform UPDATE operation for each traversed entry. */
#define BCMA_BCMLT_ENTRY_OP_OPT_TRVS_UPD  (1 << 3)

/*! Perform DELETE operation for each traversed entry. */
#define BCMA_BCMLT_ENTRY_OP_OPT_TRVS_DEL  (1 << 4)

/*! \} */


/*!
 * \name Field flags
 * \anchor BCMA_BCMLT_FIELD_F_xxx
 */

/*! \{ */

/*! Field is a key of table. */
#define BCMA_BCMLT_FIELD_F_KEY            (1 << 0)

/*! Field value can be represented by a symbol string. */
#define BCMA_BCMLT_FIELD_F_SYMBOL         (1 << 1)

/*! Field contents are represented by an array */
#define BCMA_BCMLT_FIELD_F_ARRAY          (1 << 2)

/*! Field width is greater than 64-bit length */
#define BCMA_BCMLT_FIELD_F_WIDE           (1 << 3)

/*! \} */

/*!
 * Macro to indicate that union u.array is to be used in
 * \ref bcma_bcmlt_field_t
 */
#define BCMA_BCMLT_FIELD_IS_ARRAY(_f) \
    (((_f)->flags & (BCMA_BCMLT_FIELD_F_ARRAY | BCMA_BCMLT_FIELD_F_WIDE)) != 0)

/*!
 * \brief LT Field information structure used for array field.
 *
 * This structure defines the information needed for array field.
 */
typedef struct bcma_bcmlt_field_array_s {

    /*! Union of array field types */
    union {

        /*!
         * Field values array which is dynamically allocated.
         * Might be used for field which is a scalar array or
         * wide-field (field width > 64-bit)
         */
        uint64_t *vals;

        /*! Field symbol strings array which is dynamically allocated */
        const char **strs;

    } u;

    /*!
     * Dynamically allocated bool array to indicate if an element
     * in scalar array or symbol strings array is valid or not.
     */
    bool *elements;

    /*! Field array size allocated for \c vals or \c strs */
    int size;

} bcma_bcmlt_field_array_t;

/*!
 * \brief LT Field information structure.
 *
 * This structure defines a single field within a table.
 */
typedef struct bcma_bcmlt_field_s {

    /*! Field name */
    const char *name;

    /*! Field width */
    uint32_t width;

    /*! Union of entry field types */
    union {

        /*! Field value used for non-array field */
        uint64_t val;

        /*! Field symbol string used for non-array field */
        const char *str;

        /*!
         * Dynamically allocated structure used for field
         * which data is represented in an array.
         */
        bcma_bcmlt_field_array_t *array;

    } u;

    /*! Field attribute (\ref BCMA_BCMLT_FIELD_F_xxx) */
    uint32_t flags;

} bcma_bcmlt_field_t;

/*!
 * \brief Entry information.
 *
 * This structure is used to store entry information while
 * performing logical or physical table operation.
 *
 * This structure should be initialized and cleaned up by functions
 * \ref bcma_bcmlt_entry_info_create and \ref bcma_bcmlt_entry_info_destroy
 * respectively.
 */
typedef struct bcma_bcmlt_entry_info_s {

    /*! Unit number the entry performs on */
    int unit;

    /*! Logical or physical table name the entry operates on */
    const char *name;

    /*! Entry handle used in bcmlt functions */
    bcmlt_entry_handle_t eh;

    /*! Default fields information array */
    bcmlt_field_def_t *def_fields;

    /*! Default number of fields information array */
    int num_def_fields;

    /*! Number of key fields for this table */
    int num_key_fields;

    /*! Fields information array */
    bcma_bcmlt_field_t *fields;

    /*! Fields information array size */
    int szfields;

    /*! User specified number of fields information array */
    int num_fields;

    /*!
     * Number of fields added to the entry before commit for LT.
     * Queried from bcmlt_entry_field_count() in <bcmlt/bcmlt.h>.
     * Default value is -1 if not used.
     */
    int num_commit_fields;

    /*! Identify the entry is logical or physical table */
    bool logical;

    /*!
     * Attributes of the entry table.
     * Refer to BCMLT_ENT_ATTR_xxx in <bcmlt/bcmlt.h>.
     */
    uint32_t attr;

    /*! Options of the entry operation. */
    uint32_t opts;

} bcma_bcmlt_entry_info_t;

/*!
 * \brief Callback function in \ref bcma_bcmlt_tables_search.
 *
 * The callback function passes table name and default fields information,
 * including fields name, data type, access type, etc for advanced usage.
 *
 * \param [in] cookie User data through \ref bcma_bcmlt_tables_search
 * \param [in] table_name Table name
 * \param [in] fields Default fields array of table
 * \param [in] num_fields Number of default fields array of table
 *
 * \return Always 0
 */
typedef int (*bcma_bcmlt_table_info_f)(void *cookie,
                                       const char *table_name,
                                       bcmlt_field_def_t *fields,
                                       uint32_t num_fields);

/*!
 * \brief Tables search function.
 *
 * Search tables on specific device by search_name. If the search matches,
 * The matched table and default fields information will be passed through
 * callback function.
 *
 * Two match modes for search are supported.
 * If \c match_mode is BCMA_BCMLT_MATCH_EXACT, the table
 * will be matched if search_name is exactly the same to table name.
 * If \c match_mode is BCMA_BCMLT_MATCH_SUBSTR, the
 * table will be matched if table_name contains search_name where case is
 * ignored.
 *
 * Special characters starting in \c search_name can identify different match
 * cases in BCMA_BCMLT_MATCH_SUBSTR.
 * 1. @: Exact match
 * 2. ^: Match from start
 * 3. *: Match in string
 *
 * If \ref BCMA_BCMLT_SCOPE_F_LTBL is set in \c search_scope,
 * logical tables will be included in the table search scope.
 * If \ref BCMA_BCMLT_SCOPE_F_PTBL is set in \c search_scope,
 * physical tables will be included in the table search scope.
 * If none of these two flags are set in \c search_scope, the search scope will
 * be equivalent to both flags are set. i.e. The seach scope is in both
 * logical and physical tables.
 *
 * \param [in] unit Unit number
 * \param [in] search_name Table name or sub-string to be searched
 * \param [in] match_mode Match mode in search
 * \param [in] search_scope Search in logical or/and physical tables
 * \param [in] cb Callback function when \c search_name matches
 * \param [in] cookie User data for callback function
 *
 * \return Number of tables matching the search criteria
 */
extern int
bcma_bcmlt_tables_search(int unit, const char *search_name,
                         bcma_bcmlt_match_mode_t match_mode,
                         uint32_t search_scope,
                         bcma_bcmlt_table_info_f cb, void *cookie);

/*!
 * \brief Callback function in \ref bcma_bcmlt_table_traverse.
 *
 * The traversed entry infromation and the index number the entry is
 * traversed are passed to the traverse callback function.
 *
 * \param [in] cookie User data through \ref bcma_bcmlt_table_traverse.
 * \param [in] ei The traversed entry information.
 * \param [in] entry_no The index number of the traversed entry.
 *
 * \return 0 on success otherwise -1.
 */
typedef int (*bcma_bcmlt_traverse_f)(void *cookie,
                                     bcma_bcmlt_entry_info_t *ei,
                                     int entry_no);

/*!
 * \brief Call a user-defined function for all entries in a table.
 *
 * This function traverses a logical table \c lt_name and
 * calls back to \ref bcma_bcmlt_traverse_f \c cb on every
 * traversed entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name Logical table name to be traversed.
 * \param [in] fields Default fields array of table.
 * \param [in] num_fields Number of default fields array of table.
 * \param [in] cb Callback function for each traversed entry.
 * \param [in] cookie User data for callback function.
 *
 * \return Total number of entries traversed if > 0, otherwise
 *         SHR_E_xxx error codes.
 */
extern int
bcma_bcmlt_table_traverse(int unit, const char *lt_name,
                          bcmlt_field_def_t *fields, uint32_t num_fields,
                          bcma_bcmlt_traverse_f cb, void *cookie);

/*!
 * \brief Create and initialize LT or PT entry information object.
 *
 * This function allocates \ref bcma_bcmlt_entry_info_t instance and
 * allocates entry from bcmlt module with \c table_name
 * for the specified device.
 *
 * If the default fields information \c fields is NULL, this function will
 * search the given table name among all tables through \ref
 * bcma_bcmlt_tables_search to verify the given table name and
 * retrieve the default fields information of the table. If this function
 * is called within a callback function of \ref bcma_bcmlt_tables_search,
 * the caller should specify the default fields information \c fields and
 * \c num_fields to avoid recursive calls to \ref bcma_bcmlt_tables_search.
 *
 * \param [in] unit Unit number.
 * \param [in] table_name Name of table of the allocated entry.
 * \param [in] logical Specify if the entry is in a logical table
 *                     or a physical table.
 * \param [in] attr Entry attributes (BCMLT_ENT_ATTR_xxx).
 * \param [in] fields Default fields information of the given table.
 * \param [in] num_fields Number of Default fields in the given table.
 *
 * \return Pointer to allocated \ref bcma_bcmlt_entry_info_t instance,
 *         otherwise NULL.
 */
extern bcma_bcmlt_entry_info_t *
bcma_bcmlt_entry_info_create(int unit, const char *table_name,
                             bool logical, uint32_t attr,
                             bcmlt_field_def_t *fields, uint32_t num_fields);

/*!
 * \brief Destroy entry information object.
 *
 * This function frees entry in bcmlt module and frees
 * \ref bcma_bcmlt_entry_info_t instance for specified device.
 *
 * \param [in] ei Entry information to be freed.
 *
 * \return 0 No errors, otherwise failure on inavlid argument \c ei or
 *         fail to free entry in bcmlt.
 */
extern int
bcma_bcmlt_entry_info_destroy(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Create entry fields in entry information object.
 *
 * This function creates \c num_fields array size for \ref bcma_bcmlt_field_t
 * as the member \c fields in \ref bcma_bcmlt_entry_info_t.
 * The minumum array size is the total table fields number this entry
 * presents for.
 *
 * If this function is called multiple times, the previous created \c fields
 * will be destroyed and a clean \c fields array will be allocated.
 *
 * \param [in] ei Entry information.
 * \param [in] num_fields Array count to be allocated for \c fields.
 *
 * \return 0 No errors, otherwise failure.
 */
extern int
bcma_bcmlt_entry_fields_create(bcma_bcmlt_entry_info_t *ei,
                               uint32_t num_fields);

/*!
 * \brief Destroy entry fields in entry information object.
 *
 * This function frees the member \c fields (array of \ref bcma_bcmlt_field_t)
 * in the specified entry information \ref bcma_bcmlt_entry_info_t.
 *
 * \param [in] ei Entry information.
 *
 * \return 0 No errors, otherwise failure.
 */
int
bcma_bcmlt_entry_fields_destroy(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Initialize field information from the matching default field data.
 *
 * \param [in] field Field information
 * \param [in] df Default field information
 *
 * \return 0 No errors, otherwise negative value
 */
extern int
bcma_bcmlt_entry_field_init(bcma_bcmlt_field_t *field, bcmlt_field_def_t *df);

/*!
 * \brief List entry information
 *
 * This function lists the values of all fields in the specified entry.
 *
 * \param [in] ei Specified entry information
 * \param [in] prefix Prefix string to be added for each field if not NULL
 * \param [in] key_only List key fields only if non-zero
 * \param [in] disp_word CLI output display word \ref BCMA_BCMLT_DISP_WORD_SET
 *
 * \return 0 No errors, otherwise -1
 */
extern int
bcma_bcmlt_entry_list(bcma_bcmlt_entry_info_t *ei, const char *prefix,
                      bool key_only, uint32_t disp_word);

/*!
 * \brief Set default value to the field structure \ref bcma_bcmlt_field_t.
 *
 * Set the field default value in entry \c ei to the field information
 * structure \c field according to the specified field type.
 *
 * \param [in] ei Specified entry information
 * \param [in] df Specified default field information
 * \param [in,out] field Specified field information
 *
 * \return 0 No errors, otherwise negtive value
 */
extern int
bcma_bcmlt_entry_field_default_val_set(bcma_bcmlt_entry_info_t *ei,
                                       bcmlt_field_def_t *df,
                                       bcma_bcmlt_field_t *field);

/*!
 * \brief Parse a field assignment string into \ref bcma_bcmlt_field_t
 *
 * Find the matching field name and parse the field assigned value
 * from the field assignment string.
 *
 * \param [in,out] ei Specified entry information
 * \param [in,out] field Specified field information
 * \param [in] field_arg Field assignment string to be parsed
 *
 * \return 0 No errors, otherwise negtive value
 */
extern int
bcma_bcmlt_entry_field_parse(bcma_bcmlt_entry_info_t *ei,
                             bcma_bcmlt_field_t *field, char *field_arg);

/*!
 * \brief Add a field to a table entry
 *
 * Add specified field value to entry. Note that the field name and
 * field flags should be properly assigned first.
 *
 * \param [in] ei Entry information
 * \param [in] field Field information to be added to entry
 *
 * \return 0 No errors, otherwise -1
 */
extern int
bcma_bcmlt_entry_field_add(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field);

/*!
 * \brief Get a field value from a table entry.
 *
 * Get specified field value from entry. Note that the field name and
 * field flags should be properly assigned first.
 *
 * \param [in] ei Specified entry information
 * \param [in,out] field Specified field information
 *
 * \return 0 No errors, otherwise -1
 */
extern int
bcma_bcmlt_entry_field_get(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field);

/*!
 * \brief Parse field value from string
 *
 * Parse field value from string \c field_arg to field data.
 * The string \c field_arg is in one of the following formats:
 * - field=val
 * - field[r1]=v1{,field[r2]=v2,...}
 *
 * \param [in] field_arg String of a field assignment
 * \param [out] field Field information to hold the parsed result
 *
 * \return 0 No errors, otherwise -1
 */
extern int
bcma_bcmlt_field_parse(char *field_arg, bcma_bcmlt_field_t *field);

/*!
 * \brief Format field value to string
 *
 * Format field value in \c field to print buffer \c pb.
 *
 * \param [out] pb Print buffer object which is already created
 * \param [in] field Specified field information contianing field value to
 *                   be formatted
 * \param [in] disp_word CLI output display word \ref BCMA_BCMLT_DISP_WORD_SET
 *
 * \return \c Formatted string
 */
extern const char *
bcma_bcmlt_field_format(shr_pb_t *pb, bcma_bcmlt_field_t *field,
                        uint32_t disp_word);

/*!
 * \brief Format field default information to string
 *
 * Format a givin field default information in table \c tbl_name to
 * print buffer \c pb. A prefix string can be specified through \c pf_str
 * to customize the output format.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] pf_str Prefix string of the output format.
 * \param [in] unit Unit number.
 * \param [in] tbl_name Table name of the field.
 * \param [in] df Default field information.
 *
 * \return \c Formatted string.
 */
extern const char *
bcma_bcmlt_entry_field_default_info_format(shr_pb_t *pb, const char *pf_str,
                                           int unit, const char *tbl_name,
                                           bcmlt_field_def_t *df);

/*!
 * \brief Get symbol strings of a specified symbol field.
 *
 * This function will return the symbol strings array which is valid for
 * symbol field \c field_name in table \c tbl_name.
 *
 * Note the caller is responsible for freeing the returned array
 * by sal_free().
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_name Table name of the symbol field.
 * \param [in] field_name The specified symbol field name.
 * \param [out] elems The returned array size.
 *
 * \return Pointer to the symbol strings array or NULL on failure.
 */
extern const char **
bcma_bcmlt_entry_field_symbol_strs_get(int unit, const char *tbl_name,
                                       const char *field_name,
                                       uint32_t *elems);

/*!
 * \brief Format the output in row x column matrix.
 *
 * Format the strings in array strs in row x column matrix.
 * The columns are calculated dynamically according to the current
 * window width and the maximum string length in the array. If the
 * array size is too large, the calculation will be skipped and the
 * strings will be listed in one column per line.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] prefix Prefix for the output to be displayed.
 * \param [in] strs Array of string that needs to be displayed.
 * \param [in] cnt Number of elements in the array.
 *
 * \return NONE.
 */
extern void
bcma_bcmlt_format_strs_array(shr_pb_t *pb, const char *prefix,
                             const char **strs, int cnt);

#endif /* BCMA_BCMLT_H */
