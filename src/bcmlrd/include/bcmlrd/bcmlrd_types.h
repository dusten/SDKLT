/*! \file bcmlrd_types.h
 *
 * \brief Logical Table Types
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

#ifndef BCMLRD_TYPES_H
#define BCMLRD_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/chip/bcmlrd_limits.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Table identifier.
 *
 * Table identifier similar to those used by the DRD.
 */
typedef bcmltd_sid_t bcmlrd_sid_t;   /* Generic table ID local to symbol. */

/*!
 * \brief Field identifier.
 *
 * Field identifiers similar to those used by the DRD.
 */
typedef bcmltd_fid_t bcmlrd_fid_t;   /* DRD compatible field ID local to logical. */

/*!
 * \brief Special index indicator
 *
 * Physical tables do not have a field to specifically indicate an
 * index into a table. BCMLRD_FIELD_INDEX is an indicator that the
 * destination is to be treated as an index. It is expected that this
 * index value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_INDEX ((uint32_t)-1)

/*!
 * \brief Special port indicator
 *
 * Physical tables do not have a field to specifically indicate an
 * port index into a table. BCMLRD_FIELD_PORT is an indicator that
 * the destination is to be treated as an index. It is expected that
 * this index value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_PORT  ((uint32_t)-2)

/*!
 * \brief Special TCAM Entry priority indicator
 *
 * BCMLRD_FIELD_ENTRY_PRIORITY is an indicator that the destination is
 * to be treated as a TCAM entry priority.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_ENTRY_PRIORITY ((uint32_t)-3)

/*!
 * \brief Special table instance identifier
 *
 * BCMLRD_TABLE_INSTANCE is an indicator that the destination is to be
 * treated as a table instance identifier.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_INSTANCE ((uint32_t)-4)

/*!
 * \brief Special table selector identifier
 *
 * BCMLRD_FIELD_TABLE_SEL is an indicator that the destination is to be
 * treated as a table selector identifier.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_TABLE_SEL ((uint32_t)-5)

/*!
 *  \brief No operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_NOP        0x00000000

/*!
 *  \brief Insert operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_INSERT     0x00000001

/*!
 *  \brief Update operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_UPDATE     0x00000002

/*!
 *  \brief Delete operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_DELETE     0x00000004

/*!
 *  \brief Delete operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_LOOKUP     0x00000008

/*!
 *  \brief Delete operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_TRAVERSE   0x00000010

/*!
 * \brief Generic mapping function pointer type
 */

typedef void (*bcmlrd_generic_f)(void);

/*!
 * \brief Generic mapping function pointer argument.
 */

typedef struct bcmlrd_generic_arg_s {
    /*! Number of Generic function arguments. */
    uint32_t values;

    /*! Array of Generic function argument values. */
    const uint32_t *value;
} bcmlrd_generic_arg_t;

/*!
 * \brief Table Map field descriptor
 */
typedef bcmltd_field_desc_t bcmlrd_field_desc_t;

/*!
 * \brief Kind of table in descriptor.
 */
typedef enum bcmlrd_table_kind_e {
    /*! Logical table. */
    BCMLRD_MAP_LOGICAL,

    /*! Physical table. */
    BCMLRD_MAP_PHYSICAL,

    /*! Field Validation. */
    BCMLRD_MAP_VALIDATION,

    /*! Custom Table. */
    BCMLRD_MAP_CUSTOM,

    /*! LTM Managed Table. */
    BCMLRD_MAP_LTM,

    /*! Table Commit Table. */
    BCMLRD_MAP_TABLE_COMMIT,
} bcmlrd_table_kind_t;

/*!
 * \brief Table descriptor.
 */
typedef struct bcmlrd_table_desc_s {
    /*! Table kind. */
    bcmlrd_table_kind_t kind;
    /*! Table ID. */
    uint32_t            id;
} bcmlrd_table_desc_t;

/* Temporary: tcam ordering */
enum {
    LPM,
    ALPM,
};

/*!
 * \brief Table Map entry - per destination field.
 */
typedef struct bcmlrd_map_entry_s {
    /*! Map entry type. */
    enum {
        /*! Map entry copies u.mapped.src to desc. */
        BCMLRD_MAP_ENTRY_MAPPED_KEY,

        /*! Map entry copies u.fixed.value to desc. */
        BCMLRD_MAP_ENTRY_FIXED_KEY,

        /*! Map entry copies u.mapped.src to desc. */
        BCMLRD_MAP_ENTRY_MAPPED_VALUE,

        /*! Map entry copies u.fixed.value to desc. */
        BCMLRD_MAP_ENTRY_FIXED_VALUE,

        /*! Forward Key Field Transform Handler */
        BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER,

        /*! Forward Value Field Transform Handler */
        BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER,

        /*! Reverse Key Field Transform Handler */
        BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER,

        /*! Reverse Value Field Transform Handler */
        BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER,

        /*! Key Field Validation */
        BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION,

        /*! Value Field Validation */
        BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION,

        /*! Table mapping is by data in u.table_handler. */
        BCMLRD_MAP_ENTRY_TABLE_HANDLER,

        /*! Field attribute of desc in  u.attribute. */
        BCMLRD_MAP_ENTRY_FIELD_ATTRIBUTE,

        /*! Map entry must be ignored. */
        BCMLRD_MAP_ENTRY_IGNORE,

        /*! Table Commit Handler. */
        BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER,
    } entry_type;

    /*! Mapping field descriptor. */
    bcmlrd_field_desc_t desc;

    /*! Union of mapping attribute structures. */
    union {
        struct {
            /*! Source field descriptor. */
            bcmlrd_field_desc_t src;
        } mapped;

        struct {
            /*! Destination value. */
            uint32_t            value;
            /*! Default value. */
            uint32_t            default_value;
        } fixed;

        struct {
            /*! Field handler. (deprecated) */
            bcmlrd_generic_f handler;

            /*! Handler argument list. */
            const bcmlrd_generic_arg_t *arg;
        } special;

        /*! LTA Handler ID. */
        uint32_t handler_id;
    } u;

} bcmlrd_map_entry_t;

/*!
 * \brief Table Map group - per destination table
 */
typedef struct bcmlrd_map_group_s {
    /*! Map destination type. */
    bcmlrd_table_desc_t dest;

    /*! Number of map entries in this configuration. */
    uint32_t entries;

    /*! Variable length array of maps in this group. */
    const bcmlrd_map_entry_t *entry;
} bcmlrd_map_group_t;

typedef enum bcmlrd_map_table_attr_name_s {
    BCMLRD_MAP_TABLE_ATTRIBUTE_INTERACTIVE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MIN_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MAX_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_SIZE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_TYPE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MIN_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MAX_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_SIZE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MIN_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MAX_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_SIZE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TCAM_ORDERING,
    BCMLRD_MAP_TABLE_ATTRIBUTE_SHARED_INDEX,
} bcmlrd_map_table_attr_name_t;

/*!
 * \brief Table attribute
 */
typedef struct bcmlrd_map_table_attr_s {
    /*! Attribute key. */
    bcmlrd_map_table_attr_name_t key;

    /*! Attribute value. */
    uint32_t value;
} bcmlrd_map_table_attr_t;

/*!
 * \brief Table attributes - per logical table
 */
typedef struct bcmlrd_map_attr_s {
    /*! Number of attributes. */
    uint32_t attributes;

    /*! Array of table attribute. */
    const bcmlrd_map_table_attr_t *attr;
} bcmlrd_map_attr_t;

/*!
 * \brief Table Map field data - per logical table
 */
typedef struct bcmlrd_field_data_s {
    /*! Field flags from mapping. */
    uint32_t flags;

    /*! Minimum field value. */
    const bcmltd_field_data_t *min;

    /*! Default field value. */
    const bcmltd_field_data_t *def;

    /*! Maximum field value. */
    const bcmltd_field_data_t *max;

    /*! Width of field in bits. */
    uint32_t width;

    /*! Depth of field in elements. */
    uint32_t depth;

    /*! Number of enumeration symbols. */
    uint32_t                 num_sym;

    /*! Array of enumeration symbols. */
    const shr_enum_map_t     *sym;
} bcmlrd_field_data_t;

/*!
 * \brief Table Map field data - per logical table
 */
typedef struct bcmlrd_map_field_data_s {
    /*! Number field data entries. */
    uint32_t fields;

    /*! Variable length array of field data. */
    const bcmlrd_field_data_t *field;
} bcmlrd_map_field_data_t;

/*!
 * \brief Mapping condition operators.
 */
typedef enum bcmlrd_cond_op_e {
    /*! Logical AND condition. */
    BCMLRD_CONDITION_AND,

    /*! Logical OR condition. */
    BCMLRD_CONDITION_OR,
} bcmlrd_cond_op_t;

/*!
 * \brief Mapping conditions.
 */
typedef struct bcmlrd_condition_s {
    /*! Condition operator. */
    bcmlrd_cond_op_t op;

    /*! Condition table. */
    bcmlrd_sid_t  table;

    /*! Condition field. */
    bcmlrd_fid_t  field;

    /*! Condition value. */
    uint64_t value;

} bcmlrd_condition_t;

/*!
 * \brief Mapping condition expressions.
 */
typedef struct bcmlrd_cond_expr_s {
    /*! Number of conditions in this expression. */
    size_t num_cond;

    /*! First condition in this expression. */
    const bcmlrd_condition_t *cond;
} bcmlrd_cond_expr_t;


/*!
 * \brief Field condition expressions.
 */
typedef struct bcmlrd_cond_field_s {
    /*! Conditioned field. */
    bcmlrd_fid_t field;

    /*! Condition expression. */
    const bcmlrd_cond_expr_t *expr;

    /*! Groups/Entries associated with field. */
    const uint32_t *edit;

} bcmlrd_cond_field_t;

/*!
 * \brief Field condition expressions.
 */
typedef struct bcmlrd_cond_data_s {
    /*! Table condition expression. */
    const bcmlrd_cond_expr_t *table_expr;

    /*! Number of field conditions. */
    size_t num_field_cond;

    /*! Field condition expressions. */
    const bcmlrd_cond_field_t *field_cond;

} bcmlrd_cond_data_t;

/*!
 * \brief Table Map - per logical table.
 */
typedef struct bcmlrd_map_s {
    /*! Logical Table source ID. */
    uint32_t src_id;

    /*! Field data. */
    const bcmlrd_map_field_data_t *field_data;

    /*! Number of map groups. */
    uint32_t groups;

    /*! Variable length array of map groups. */
    const bcmlrd_map_group_t *group;

    /*! Table attributes. */
    const bcmlrd_map_attr_t *table_attr;

    /*! Table conditions. */
    const bcmlrd_cond_data_t *cond;

    /*! Table entry opcodes. */
    uint32_t entry_ops;
} bcmlrd_map_t;

/*!
 * \brief Table Map configuration storage representation
 */
typedef struct bcmlrd_map_conf_rep_s {
    /*! Map configuration name. */
    const char *name;

    /*! Probe function for configuration. */
    int (*probe)(int unit);

    /*! Number of LTM table init IDs */
    uint32_t table_init_ids;

    /*! Variable length array of LTM table init IDs */
    const uint32_t *table_init_id;

    /*! Fixed length array of maps in this configuration index by lsid. */
    const bcmlrd_map_t *map[BCMLRD_TABLE_COUNT];

} bcmlrd_map_conf_rep_t;

/*!
 * \brief Index table flag.
 */
#define BCMLRD_TABLE_F_TYPE_INDEX BCMLTD_TABLE_F_TYPE_INDEX

/*!
 * \brief TCAM table flag.
 */
#define BCMLRD_TABLE_F_TYPE_TCAM BCMLTD_TABLE_F_TYPE_TCAM

/*!
 * \brief HASH table flag.
 */
#define BCMLRD_TABLE_F_TYPE_HASH BCMLTD_TABLE_F_TYPE_HASH

/*!
 * \brief Table type mask.
 */
#define BCMLRD_TABLE_F_TYPE_MASK BCMLTD_TABLE_F_TYPE_MASK

/*!
 * \brief Table type mapped.
 */
#define BCMLRD_TABLE_F_TYPE_MAPPED BCMLTD_TABLE_F_TYPE_MAPPED

/*!
 * \brief Table read-only flag.
 */
#define BCMLRD_TABLE_F_READ_ONLY BCMLTD_TABLE_F_READ_ONLY

/*!
 * \brief Table type mask.
 */
#define BCMLRD_TABLE_F_TYPE_INDEX_ALLOC BCMLTD_TABLE_F_TYPE_INDEX_ALLOC

/*!
 * \brief Field untyped flag.
 */
#define BCMLRD_FIELD_F_UNTYPED BCMLTD_FIELD_F_UNTYPED

/*!
 * \brief Field special type mask.
 */
#define BCMLRD_FIELD_F_TYPE_MASK BCMLTD_FIELD_F_TYPE_MASK

/*!
 * \brief Field read-only flag.
 */
#define BCMLRD_FIELD_F_READ_ONLY BCMLTD_FIELD_F_READ_ONLY

/*!
 * \brief Field "is key" flag.
 */
#define BCMLRD_FIELD_F_KEY BCMLTD_FIELD_F_KEY

/*!
 * \brief Field "is unmapped" flag.
 */
#define BCMLRD_FIELD_F_UNMAPPED BCMLTD_FIELD_F_UNMAPPED

/*!
 * \brief Field enumeration flag.
 */
#define BCMLRD_FIELD_F_ENUM BCMLTD_FIELD_F_ENUM

/*!
 * \brief Field non-cacheable flag.
 */
#define BCMLRD_FIELD_F_NON_CACHEABLE BCMLTD_FIELD_F_NON_CACHEABLE



/*!
 * \brief Field storage representation for components using LRD types
 */
typedef bcmltd_field_rep_t bcmlrd_field_rep_t;

/*!
 * \brief Table storage representation for components using LRD types
 */
typedef bcmltd_table_rep_t bcmlrd_table_rep_t;

/*!
 * \brief Field default type
 */
typedef bcmltd_field_def_t bcmlrd_field_def_t;

#endif /* BCMLRD_TYPES_H */
