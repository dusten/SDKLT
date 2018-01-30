/*! \file bcmltd_handler.h
 *
 * Data types for transform and custom table handlers.
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

#ifndef BCMLTD_HANDLER_H
#define BCMLTD_HANDLER_H

#include <bcmltd/bcmltd_types.h>

/* BCMLTA types */

/*!
 * \brief Field descriptor.
 *
 * In the BCMLTD context, each instance of a bcmlt_field_list_t is treated
 * as an array element, and the 'next' element should be ignored, as the
 * array may be the underlying list with elements reordered.
 */

typedef bcmlt_field_list_t bcmltd_field_t;

/*!
 * \brief Field set descriptor.
 */

typedef struct bcmltd_fields_s {
    /*! Number of fields. */
    size_t                    count;

    /*! Array of pointer to fields. */
    bcmltd_field_t            **field;

} bcmltd_fields_t;

/*!
 * \brief Generic mapping function pointer argument.
 */

typedef struct bcmltd_generic_arg_s {
    /*! Source table. */
    bcmltd_sid_t sid;

    /*! Number of Generic function arguments. */
    uint32_t values;

    /*! Array of Generic function argument values. */
    const uint32_t *value;

    /*! Component user data. */
    void *user_data;
} bcmltd_generic_arg_t;

/*!
 * \brief Field descriptor
 */
typedef struct bcmltd_field_desc_s {
    /*! Field ID. */
    uint32_t field_id;

    /*! Field index. */
    uint32_t field_idx;

    /*! Lowest bit position. */
    uint16_t minbit;

    /*! Highest bit position. */
    uint16_t maxbit;

    /*! Entry index for extended physical field */
    uint16_t entry_idx;

    /*! Reserved */
    uint16_t reserved;
} bcmltd_field_desc_t;

/*!
 * \brief Field list for LTA Transforms.
 *
 * Known use case:
 * This is supplied to the field demux/mux (1:M & N:1) Transforms
 * as the bcmltd_generic_arg_t.user_data in the transform arguments list.
 */
typedef struct bcmltd_field_list_s {
    /*! Number of subfields in list. */
    uint32_t field_num;

    /*! List of pointers to bcmcfg_field_desc_t specifiers. */
    const bcmltd_field_desc_t *field_array;
} bcmltd_field_list_t;

/*!
 * \brief Generic mapping function pointer argument.
 */

typedef struct bcmltd_transform_arg_s {
    /*! Number of transform function arguments. */
    uint32_t values;

    /*! Array of transform function argument values. */
    const uint32_t *value;

    /*! Number of transform tables. */
    uint32_t tables;

    /*! Array of transform tables. */
    const uint32_t *table;

    /*! Number of transform fields. */
    uint32_t fields;

    /*! Array of transform fields. */
    const uint32_t *field;

    /*! Array of fields descriptor. */
    const bcmltd_field_list_t *field_list;

    /*! Number of transform return fields. */
    uint32_t rfields;

    /*! Array of transform return fields. */
    const uint32_t *rfield;

    /*! Array of return fields descriptor. */
    const bcmltd_field_list_t *rfield_list;

    /*! Component user data. */
    const bcmltd_generic_arg_t *comp_data;
} bcmltd_transform_arg_t;

/*!
 * \brief Field validation arguments.
 */

typedef struct bcmltd_field_val_arg_s {
    /*! Number of validate arguments. */
    uint32_t values;

    /*! Array of validate argument values. */
    const uint32_t *value;

    /*! Number of validate fields. */
    uint32_t fields;

    /*! Array of validate fields. */
    const uint32_t *field;

    /*! Field validation data. */
    const bcmltd_generic_arg_t *comp_data;
} bcmltd_field_val_arg_t;

/*!
 * \brief Component validation operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          Table opcode.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are valid with respect to the given opcode and handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_val_op_f)(int unit,
                                       bcmlt_opcode_t opcode,
                                       const bcmltd_fields_t *in,
                                       const bcmltd_generic_arg_t *arg);
/*!
 * \brief Component input operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are accepted as input for the relevant operation according to the
 * handler (used for insert, delete, update).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_in_op_f)(int unit,
                                      uint32_t trans_id,
                                      const bcmltd_fields_t *in,
                                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component output operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are accepted as input for the relevant operation  according to the
 * handler (used for traverse).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_out_op_f)(int unit,
                                       uint32_t trans_id,
                                       bcmltd_fields_t *out,
                                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component input/output operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are accepted as input for the relevant operation  according to the
 * handler (used for lookup).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_inout_op_f)(int unit,
                                         uint32_t trans_id,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component High-Availability operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if operation was performed.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_ha_op_f)(int unit,
                                      uint32_t trans_id,
                                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * Handler should return SHR_E_NONE if the fields in the field set are
 * accepted as input for the transform operation. Transformed (output
 * physical) fields should be set in 'out', along with an updated
 * destination table in 'out_sid' if any.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_transform_op_f)(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg);

/*!
 * \brief Component field validation operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          Table opcode.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are valid with respect to the given opcode and handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_field_val_op_f)(int unit,
                                             bcmlt_opcode_t opcode,
                                             const bcmltd_fields_t *in,
                                             const bcmltd_field_val_arg_t *arg);

/*!
 * \brief Transform handler interface type
 */
typedef struct bcmltd_xfrm_handler_s {
    /*! Transform function. */
    const bcmltd_handler_transform_op_f transform;
    /*! Transform Handler arguments. */
    const bcmltd_transform_arg_t *arg;
} bcmltd_xfrm_handler_t;

/*!
 * \brief Field validation handler interface type
 */
typedef struct bcmltd_field_val_handler_s {
    /*! Field validation function. */
    const bcmltd_handler_field_val_op_f validate;
    /*! Field validation arguments. */
    const bcmltd_field_val_arg_t *arg;
} bcmltd_field_val_handler_t;

/*!
 * \brief Table commit handler interface type
 */
typedef struct bcmltd_table_commit_handler_s {
    /*! Prepare transaction function: initiate and record transaction ID. */
    const bcmltd_handler_ha_op_f prepare;

    /*! Commit function. */
    const bcmltd_handler_ha_op_f commit;

    /*! Abort function. */
    const bcmltd_handler_ha_op_f abort;

    /*! Handler arguments. */
    const bcmltd_generic_arg_t *arg;
} bcmltd_table_commit_handler_t;

/*!
 * \brief Optional custom table handler HA operations
 */
typedef struct bcmltd_table_ha_handler_s {
    /*! Commit function. */
    const bcmltd_handler_ha_op_f      commit;
    /*! Abort function. */
    const bcmltd_handler_ha_op_f      abort;
} bcmltd_table_ha_handler_t;

typedef struct bcmltd_table_traverse_handler_s {
    /*! First function. */
    const bcmltd_handler_out_op_f     first;
    /*! Next function. */
    const bcmltd_handler_inout_op_f   next;
} bcmltd_table_traverse_handler_t;

/*!
 * \brief Custom table handler interface type
 */
typedef struct bcmltd_table_handler_s {
    /*! Validation function. */
    const bcmltd_handler_val_op_f     validate;
    /*! Insert function. */
    const bcmltd_handler_in_op_f      insert;
    /*! Insert with Allocate function. */
    const bcmltd_handler_inout_op_f   insert_alloc;
    /*! Lookup function. */
    const bcmltd_handler_inout_op_f   lookup;
    /*! Delete function. */
    const bcmltd_handler_in_op_f      delete;
    /*! Update function. */
    const bcmltd_handler_in_op_f      update;
    /*! HA handlers. */
    const bcmltd_table_ha_handler_t   *ha;
    /*! Traverse handlers. */
    const bcmltd_table_traverse_handler_t   *traverse;
    /*! Handler arguments. */
    const bcmltd_generic_arg_t  *arg;
} bcmltd_table_handler_t;

/*!
 * \brief Custom Table Handler initialization function argument.
 */

typedef struct bcmltd_handler_lifecycle_arg_s {
    /*! Number of generic_args. */
    uint32_t generic_args;

    /*! Component table handler pointers. */
    const bcmltd_generic_arg_t **generic_arg;
} bcmltd_handler_lifecycle_arg_t;

/*!
 * \brief Component life cycle operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if operation was performed.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_lifecycle_op_f)(int unit,
                                      const bcmltd_handler_lifecycle_arg_t *arg);

/*!
 * \brief Optional custom table lifecycle handler
 */
typedef struct bcmltd_table_lifecycle_handler_s {
    /*! Initialization function. */
    const bcmltd_handler_lifecycle_op_f    init;
    /*! Cleanup function. */
    const bcmltd_handler_lifecycle_op_f    cleanup;
    /*! Handler arguments. */
    const bcmltd_handler_lifecycle_arg_t   *arg;
} bcmltd_table_lifecycle_handler_t;

/*!
 * \brief Transform interface array
 */
typedef struct bcmltd_xfrm_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_xfrm_handler_t            **handler;
} bcmltd_xfrm_intf_t;

/*!
 * \brief Field validation interface array
 */
typedef struct bcmltd_field_val_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_field_val_handler_t       **handler;
} bcmltd_field_val_intf_t;

/*!
 * \brief Table commit interface array
 */
typedef struct bcmltd_table_commit_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_table_commit_handler_t    **handler;
} bcmltd_table_commit_intf_t;

/*!
 * \brief Custom table handler interface array
 */
typedef struct bcmltd_table_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_table_handler_t           **handler;
} bcmltd_table_intf_t;

/*!
 * \brief Custom table lifecycle interface array
 */
typedef struct bcmltd_table_lifecycle_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_table_lifecycle_handler_t **handler;
} bcmltd_table_lifecycle_intf_t;

/*!
 * \brief LTM LTA interfaces
 */
typedef struct bcmltd_ltm_intf_s {
    /*! Transform interfaces. */
    const bcmltd_xfrm_intf_t            *xfrm;
    /*! Field validation interfaces. */
    const bcmltd_field_val_intf_t       *val;
    /*! Table commit interfaces. */
    const bcmltd_table_commit_intf_t    *table_commit;
    /*! Custom table handler interfaces. */
    const bcmltd_table_intf_t           *table;
    /*! Custom table lifecycle interfaces. */
    const bcmltd_table_lifecycle_intf_t *table_lc;
} bcmltd_ltm_intf_t;

/*!
 * \brief Convert field to uint32
 *
 * Convert a LTD field descriptor to uint32
 *
 * \param [in]  field           Pointer to field descriptor.
 * \param [out] value           Pointer to uint32_t value.
 *
 *
 * A scalar value in the field descriptor is converted to a uint32_t
 * value. An error is returned if the value is not a scalar or is not
 * directly convertible to a uint32_t value.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_field_to_uint32(const bcmltd_field_t *field, uint32_t *value);

/*!
 * \brief Convert uint32 to field
 *
 * Convert a uint32 to an LTD field descriptor
 *
 * \param [out]  field           Pointer to field descriptor.
 * \param [in]   value           Pointer to uint32_t value.
 *
 *
 * A uint32_t value is converted to an LTD field descriptor
 * scalar value. An error is returned if the value is not convertible
 * for some reason.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_uint32_to_field(bcmltd_field_t *field, const  uint32_t *value);

/*!
 * \brief Copy a field value
 *
 * Copy an LRD field value
 *
 * \param [in]   in              Input field.
 * \param [out]  out             Output field.
 *
 *
 * Copy an input field value to an output field.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_field_copy(const bcmltd_field_t *in, bcmltd_field_t *out);

/*!
 * \brief Check for number of fields
 *
 * Return success if there are at least 'num' many fields provided
 * in field array.
 *
 * \param [in]   fields          Field array.
 * \param [in]   num             Number of fields required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_fields_required(const bcmltd_fields_t *fields, uint32_t num);

/*!
 * \brief Check for number of generic arguments
 *
 * Return success if there are at least 'count' many generic arguments
 * provided in the generic argument structure.
 *
 * \param [in]   arg             Generic argument structure.
 * \param [in]   num             Number of arguments required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_generic_args_required(const bcmltd_generic_arg_t *arg, uint32_t num);

/*!
 * \brief Check for number of transform arguments
 *
 * Return success if there are at least 'count' many transform arguments
 * provided in the transform argument structure.
 *
 * \param [in]   arg             Transform argument structure.
 * \param [in]   num             Number of arguments required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_transform_args_required(const bcmltd_transform_arg_t *arg, uint32_t num);

/*!
 * \brief Check for number of transform table arguments
 *
 * Return success if there are at least 'count' many transform table
 * arguments provided in the transform argument structure.
 *
 * \param [in]   arg             Transform argument structure.
 * \param [in]   num             Number of tables required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_transform_tables_required(const bcmltd_transform_arg_t *arg,
                                 uint32_t num);


#endif /* BCMLTD_HANDLER_H */
