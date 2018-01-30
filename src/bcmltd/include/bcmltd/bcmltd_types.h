/*! \file bcmltd_types.h
 *
 * Logical Table Data Types header iile
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

#ifndef BCMLTD_TYPES_H
#define BCMLTD_TYPES_H

#include <shr/shr_types.h>
#include <shr/shr_fmm.h>
#include <bcmltd/bcmltd_lt_types.h>

/*! Maximum number of bytes for field raw type. */
#define BCM_MAX_FIELD_SIZE   32

/*! Maximum number of bits for a single field value */
#define BCM_FIELD_SIZE_BITS  64

/*!
 * \brief Field values raw data structure.
 */
typedef struct bcmltd_binary_field_data_s {
    /*! Number of bits in field value. */
    uint32_t  num_of_bits;

    /*! Byte array containing raw data value. */
    uint8_t bytes[BCM_MAX_FIELD_SIZE];
} bcmltd_binary_field_data_t;

/*!
 * \brief Basic field data value.
 *
 * This data structure used to assign values to fields in an entry.
 */
typedef union bcmltd_field_data_u {

    /*! Boolean type. */
    bool is_true;

    /*! uint8_t. */
    uint8_t u8;

    /*! uint16_t. */
    uint16_t u16;

    /*! uint32_t. */
    uint32_t u32;

    /*! uint64_t. */
    uint64_t u64;

    /*! Raw catch-all type. */
    bcmltd_binary_field_data_t binary;
} bcmltd_field_data_t;

/*!
 * \brief Field data tag type.
 *
 * This value is used to interpret a field data type.
 */
typedef bcmlt_field_data_tag_t bcmltd_field_data_tag_t;

/*!
 * \brief Field access modes.
 */
typedef bcmlt_field_acc_t bcmltd_field_acc_t;

/*! Allow read and write access. */
#define BCMLTD_FIELD_ACCESS_READ_WRITE BCMLT_FIELD_ACCESS_READ_WRITE

/*! Read only access mode.       */
#define BCMLTD_FIELD_ACCESS_READ_ONLY  BCMLT_FIELD_ACCESS_READ_ONLY

/*!
 * \brief Logical table ID type.
 */
typedef uint32_t bcmltd_sid_t;

/*!
 * \brief Logical field ID type.
 */
typedef uint32_t bcmltd_fid_t;

/*!
 * \brief Invalid logical table ID.
 */
#define BCMLTD_SID_INVALID       ((bcmltd_sid_t)-1)

/*!
 * \brief Index table flag.
 */
#define BCMLTD_TABLE_F_TYPE_INDEX       0x00000000

/*!
 * \brief TCAM table flag.
 */
#define BCMLTD_TABLE_F_TYPE_TCAM        0x00000001

/*!
 * \brief HASH table flag.
 */
#define BCMLTD_TABLE_F_TYPE_HASH        0x00000002

/*!
 * \brief Table type mask.
 */
#define BCMLTD_TABLE_F_TYPE_MASK        0x00000003

/*!
 * \brief Table read-only flag.
 */
#define BCMLTD_TABLE_F_READ_ONLY        0x00000004

/*!
 * \brief Table type index allocate.
 */
#define BCMLTD_TABLE_F_TYPE_INDEX_ALLOC 0x00000010

/*!
 * \brief Table configuration flag.
 */
#define BCMLTD_TABLE_F_CONFIG           0x00000020

/*!
 * \brief Table type mapped.
 */
#define BCMLTD_TABLE_F_TYPE_MAPPED      0x00000040

/*!
 * \brief Field untyped flag.
 */
#define BCMLTD_FIELD_F_UNTYPED          0x00000000

/*!
 * \brief Field special type mask.
 */
#define BCMLTD_FIELD_F_TYPE_MASK        0x00000003

/*!
 * \brief Field read-only flag.
 */
#define BCMLTD_FIELD_F_READ_ONLY        0x00000004

/*!
 * \brief Field "is key" flag.
 */
#define BCMLTD_FIELD_F_KEY              0x00000008

/*!
 * \brief Field "is unmapped" flag.
 */
#define BCMLTD_FIELD_F_UNMAPPED         0x00000010

/*!
 * \brief Field enumeration flag.
 */
#define BCMLTD_FIELD_F_ENUM             0x00000020

/*!
 * \brief Field non-cacheable flag.
 */
#define BCMLTD_FIELD_F_NON_CACHEABLE    0x00000040


/*!
 * \brief Field storage representation
 */
typedef struct bcmltd_field_rep_s {
    /*! Field name in string pool. */
    const char *name;

    /*! BCMLTD_FIELD_F_ flags. */
    uint32_t flags;

    /*! Width of field in bits. */
    uint32_t width;

    /*! Depth of field in elements. */
    uint32_t depth;

    /*! Number of enumeration symbols. */
    uint32_t num_sym;

    /*! Array of enumeration symbols. */
    const shr_enum_map_t *sym;

    /*! Field description. */
    const char *desc;
} bcmltd_field_rep_t;

/*!
 * \brief Table storage representation
 */
typedef struct bcmltd_table_rep_s {
    /*! Table name in string pool. */
    const char *name;

    /*! BCMLTD_TABLE_F_ flags. */
    uint32_t flags;

    /*! Number of fields in this table. */
    uint32_t fields;

    /*! Variable length array of fields in this table. */
    const bcmltd_field_rep_t *field;

    /*! Table description. */
    const char *desc;
} bcmltd_table_rep_t;

/*!
 * \brief Table configuration
 */
typedef struct bcmltd_table_conf_s {
    /*! Number of tables. */
    uint32_t tables;

    /*! Variable length array of tables in this table configuration. */
    const bcmltd_table_rep_t **table;
} bcmltd_table_conf_t;

/*!
 * \brief Logical table field information.
 *
 * Logical table field information suitable for client APIs.
 */
typedef struct bcmltd_field_def_s {

    /*! Field ID. */
    uint32_t                id;

    /*! Field name. */
    const char              *name;

    /*! Access type of the field. */
    bcmltd_field_acc_t      access;

    /* stubbed out until these types are defined */
    bcmltd_field_data_t     min;

    /*! Maximum value allowed. */
    bcmltd_field_data_t     max;

    /*! Default value for the field. */
    bcmltd_field_data_t     def;

    /*! Data tag to identify type. */
    bcmltd_field_data_tag_t dtag;

    /*! True if field part of the key. */
    bool                    key;

    /*! True if the field is an array. */
    bool                    array;

    /*! True if the field has associated symbols. */
    bool                    symbol;

    /*! Width of field in bits. */
    uint32_t                width;

    /*! Depth of field array in elements. */
    uint32_t                depth;

    /*! Number of scalar elements needed for field. */
    uint32_t                elements;

} bcmltd_field_def_t;

/*!
 * \brief Transaction and Entry Status
 *
 * The operation status for the entry is returned to the client using these
 * enumerated values. The entry accessor APIs can be used to get the status
 * for the entry operation.
 *
 * For transactions, entry_hdl status represents the combined status. Each entry in
 * the transaction has it's own status for the operation. For Batch transactions
 * if some entry operations failed, the overall status will be set to partial
 * success.
 */
typedef enum bcmlt_status_e {
    BCMLT_STATUS_NOT_HANDLED = 0,
    BCMLT_STATUS_SUCCESS,       /*!< Requested operation succeeded.    */
    BCMLT_STATUS_PART_SUCCESS,  /*!< For Transactions in batch.        */
    BCMLT_STATUS_NOTALLOWED,    /*!< Permissions or locked table.      */
    BCMLT_STATUS_NORESOURCE,    /*!< For allocation/insert operations. */
    BCMLT_STATUS_INVALID,       /*!< Invalid table/field(s).           */
    BCMLT_STATUS_HWERROR,       /*!< Could not access Hardware.        */
    BCMLT_STATUS_NUM
} bcmlt_status_t;

/*!
 * \brief Basic field data value.
 *
 * This data structure used to assign values to fields in an entry.
 */
typedef bcmltd_field_data_t bcmlt_field_data_t;

typedef shr_fmm_t bcmlt_field_list_t;

#endif /* BCMLTD_TYPES_H */
