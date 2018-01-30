/*! \file bcmltm_internal.h
 *
 * Logical Table Manager internal functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Logical Table Manager.
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

#ifndef BCMLTM_INTERNAL_H
#define BCMLTM_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_md_internal.h>

/*! Convert a number of bits to a number of (8-bit) bytes. */
#define BCMLTM_BITS2BYTES(x)    (((x) + 7) / 8)

/*! Convert a number of (8-bit) bytes to a number of bits. */
#define BCMLTM_BYTES2BITS(x)    ((x) * 8)

/*! Convert a number of bits to a number of 32-bit words. */
#define BCMLTM_BITS2WORDS(x)    (((x) + 31) / 32)

/*! Convert a number of 32-bit words to a number of bits. */
#define BCMLTM_WORDS2BITS(x)    ((x) * 32)

/*! Convert a number of (8-bit) bytes to a number of 32-bit words. */
#define BCMLTM_BYTES2WORDS(x)   (((x) + 3) / 4)

/*! Convert a number of 32-bit words to a number of (8-bit) bytes. */
#define BCMLTM_WORDS2BYTES(x)   ((x) * 4)

/*! Round up (8-bit) bytes to the next 32-bit word. */
#define BCMLTM_WORDSIZEBYTES(x)   ((((x) + 3) / 4) * 4)


/*!
 * \brief Logical table classes.
 *
 * The general format of the LT to PT mapping of a Logical Table.
 */
typedef enum bcmltm_lt_class_e {
    /*! Direct PassThru index to HW. */
    BCMLTM_CLASS_PT_PASSTHRU,

    /*! The index is a combination of key fields. */
    BCMLTM_CLASS_SIMPLE_INDEX,

    /*! The index may be requested via resource allocation. */
    BCMLTM_CLASS_RESOURCE_INDEX,

    /*! The key fields are supplied to hash functions. */
    BCMLTM_CLASS_HASH,

    /*! The key and mask fields plus priority indicate entry ordering. */
    BCMLTM_CLASS_SIMPLE_TCAM,

    /*!
     * The key and mask fields plus priority are combined with
     * some more involved algorithm to determine entry ordering.
     */
    BCMLTM_CLASS_COMPLEX_TCAM,

    /*!
     * A legacy runtime capability is exposed which requires modifying
     * FlexPP configuration tables.
     */
    BCMLTM_CLASS_CCH,

    /*! A FlexPP configuration table. (Perhaps unnecesary with LRD?). */
    BCMLTM_CLASS_FLEX,

    /*!
     * A high level logical table which is composed of lower level
     * tables and procedures.
     */
    BCMLTM_CLASS_HIGH_LEVEL,

    /*!
     * A special-case logical table preserving certain complex behaviors,
     * e.g., FP or LPM.
     */
    BCMLTM_CLASS_SPECIAL,

    /*! Number of LTM classes. */
    BCMLTM_CLASS_NUM
} bcmltm_lt_class_t;


/*!
 * \name LTM Table Type Flags.
 * \anchor BCMLTM_TABLE_TYPE_FLAG_xxx
 *
 * The table type flags describe the following characteristics in a
 * table:
 *     Category:  PT PassThru or Logical table.
 *     Map:       Direct PT map, Custom Table Handler, LTM based.
 *     Key:       Simple Index, Index with Allocation, Hash, or TCAM.
 *
 * Index tables include: Simple Index, Index with Allocation.
 * Keyed tables include: Hash, TCAM.
 *
 * Note that only certain combination of flags are valid (based on the
 * nature of what the flag describes).  For instance, a CTH map would
 * not apply to a PT PassThru table.
 */

/*! \{ */

/*! Physical Table, direct PT PassThru to HW. */
#define BCMLTM_TABLE_TYPE_FLAG_CATG_PT            (1 << 0)

/*! Logical Table. */
#define BCMLTM_TABLE_TYPE_FLAG_CATG_LT            (1 << 1)

/*! Table with Direct PT map. */
#define BCMLTM_TABLE_TYPE_FLAG_MAP_PT             (1 << 2)

/*! Table with Custom Table Handler map. */
#define BCMLTM_TABLE_TYPE_FLAG_MAP_CTH            (1 << 3)

/*! Table with LTM map. */
#define BCMLTM_TABLE_TYPE_FLAG_MAP_LTM            (1 << 4)

/*! Simple Index Table, key fields map to index and memory parameter. */
#define BCMLTM_TABLE_TYPE_FLAG_KEY_SIMPLE_INDEX   (1 << 5)

/*!
 * Index with Allocation Table, key fields map to index and memory parameter.
 * The index key is optional and is allocated if not provided during input.
 * The memory parameter key is mandatory.
 */
#define BCMLTM_TABLE_TYPE_FLAG_KEY_ALLOC_INDEX    (1 << 6)

/*! Hash Table, key fields are supplied to hash functions. */
#define BCMLTM_TABLE_TYPE_FLAG_KEY_HASH           (1 << 7)

/*! TCAM, key and mask fields are supplied to indicate entry ordering. */
#define BCMLTM_TABLE_TYPE_FLAG_KEY_TCAM           (1 << 8)

/*! \} */

/*! Mask for table category. */
#define BCMLTM_TABLE_TYPE_FLAG_CATG_MASK           \
    BCMLTM_TABLE_TYPE_FLAG_CATG_PT |               \
    BCMLTM_TABLE_TYPE_FLAG_CATG_LT

/*! Mask for table map implementation. */
#define BCMLTM_TABLE_TYPE_FLAG_MAP_MASK             \
    BCMLTM_TABLE_TYPE_FLAG_MAP_PT |                 \
    BCMLTM_TABLE_TYPE_FLAG_MAP_CTH |                \
    BCMLTM_TABLE_TYPE_FLAG_MAP_LTM

/*! Mask for table key type. */
#define BCMLTM_TABLE_TYPE_FLAG_KEY_MASK                    \
    BCMLTM_TABLE_TYPE_FLAG_KEY_SIMPLE_INDEX |              \
    BCMLTM_TABLE_TYPE_FLAG_KEY_ALLOC_INDEX |               \
    BCMLTM_TABLE_TYPE_FLAG_KEY_HASH |                      \
    BCMLTM_TABLE_TYPE_FLAG_KEY_TCAM

/*! LT with direct PT map. */
#define BCMLTM_TABLE_TYPE_FLAG_LT_PT                                    \
    (BCMLTM_TABLE_TYPE_FLAG_CATG_LT |  BCMLTM_TABLE_TYPE_FLAG_MAP_PT)

/*! LT with Custom Table Handler implementation. */
#define BCMLTM_TABLE_TYPE_FLAG_LT_CTH                                   \
    (BCMLTM_TABLE_TYPE_FLAG_CATG_LT |  BCMLTM_TABLE_TYPE_FLAG_MAP_CTH)

/*! LT with LTM map implementation. */
#define BCMLTM_TABLE_TYPE_FLAG_LT_LTM                                   \
    (BCMLTM_TABLE_TYPE_FLAG_CATG_LT |  BCMLTM_TABLE_TYPE_FLAG_MAP_LTM)

/*! LT Simple Index, key fields map to index and memory parameter. */
#define BCMLTM_TABLE_TYPE_FLAG_LT_SIMPLE_INDEX                          \
    (BCMLTM_TABLE_TYPE_FLAG_CATG_LT |  BCMLTM_TABLE_TYPE_FLAG_KEY_SIMPLE_INDEX)

/*! LT Index with Allocation, key fields map to index and memory parameter. */
#define BCMLTM_TABLE_TYPE_FLAG_LT_ALLOC_INDEX                           \
    (BCMLTM_TABLE_TYPE_FLAG_CATG_LT |  BCMLTM_TABLE_TYPE_FLAG_KEY_ALLOC_INDEX)

/*! LT Hash, key fields are supplied to hash functions. */
#define BCMLTM_TABLE_TYPE_FLAG_LT_HASH                                  \
    (BCMLTM_TABLE_TYPE_FLAG_CATG_LT |  BCMLTM_TABLE_TYPE_FLAG_KEY_HASH)

/*! LT TCAM, key and mask fields are supplied to indicate entry ordering. */
#define BCMLTM_TABLE_TYPE_FLAG_LT_TCAM                                  \
    (BCMLTM_TABLE_TYPE_FLAG_CATG_LT |  BCMLTM_TABLE_TYPE_FLAG_KEY_TCAM)

/*! Check for table type: Logical Table with direct PT map. */
#define BCMLTM_TABLE_TYPE_LT_PT(_t)                                     \
    (((_t) &                                                            \
      (BCMLTM_TABLE_TYPE_FLAG_CATG_MASK | BCMLTM_TABLE_TYPE_FLAG_MAP_MASK)) \
     == BCMLTM_TABLE_TYPE_FLAG_LT_PT)

/*! Check for table type: Logical Table with Custom Table Handler map. */
#define BCMLTM_TABLE_TYPE_LT_CTH(_t)                                    \
    (((_t) &                                                            \
      (BCMLTM_TABLE_TYPE_FLAG_CATG_MASK | BCMLTM_TABLE_TYPE_FLAG_MAP_MASK)) \
     == BCMLTM_TABLE_TYPE_FLAG_LT_CTH)

/*! Check for table type: Logical Table with LTM map. */
#define BCMLTM_TABLE_TYPE_LT_LTM(_t)                                    \
    (((_t) &                                                            \
      (BCMLTM_TABLE_TYPE_FLAG_CATG_MASK | BCMLTM_TABLE_TYPE_FLAG_MAP_MASK)) \
     == BCMLTM_TABLE_TYPE_FLAG_LT_LTM)

/*! Check for table type: Logical Table with Simple Index keys. */
#define BCMLTM_TABLE_TYPE_LT_SIMPLE_INDEX(_t)                           \
    (((_t) &                                                            \
      (BCMLTM_TABLE_TYPE_FLAG_CATG_MASK | BCMLTM_TABLE_TYPE_FLAG_KEY_MASK)) \
      == BCMLTM_TABLE_TYPE_FLAG_LT_SIMPLE_INDEX)

/*! Check for table type: Logical Table with Index with Allocation keys. */
#define BCMLTM_TABLE_TYPE_LT_ALLOC_INDEX(_t)                            \
    (((_t) &                                                            \
      (BCMLTM_TABLE_TYPE_FLAG_CATG_MASK | BCMLTM_TABLE_TYPE_FLAG_KEY_MASK)) \
      == BCMLTM_TABLE_TYPE_FLAG_LT_ALLOC_INDEX)

/*! Check for table type: Logical Table with Index (Simple/Alloc) keys. */
#define BCMLTM_TABLE_TYPE_LT_INDEX(_t)                                  \
    (BCMLTM_TABLE_TYPE_LT_SIMPLE_INDEX(_t) || \
     BCMLTM_TABLE_TYPE_LT_ALLOC_INDEX(_t))

/*! Check for table type: Logical Table with Hash keys. */
#define BCMLTM_TABLE_TYPE_LT_HASH(_t)                                   \
    (((_t) &                                                            \
      (BCMLTM_TABLE_TYPE_FLAG_CATG_MASK | BCMLTM_TABLE_TYPE_FLAG_KEY_MASK)) \
     == BCMLTM_TABLE_TYPE_FLAG_LT_HASH)

/*! Check for table type: Logical Table with TCAM keys. */
#define BCMLTM_TABLE_TYPE_LT_TCAM(_t)                                   \
    (((_t) &                                                            \
     (BCMLTM_TABLE_TYPE_FLAG_CATG_MASK | BCMLTM_TABLE_TYPE_FLAG_KEY_MASK)) \
     == BCMLTM_TABLE_TYPE_FLAG_LT_TCAM)

/*! Check for table type: Logical Table with Hash/TCAM keys. */
#define BCMLTM_TABLE_TYPE_LT_KEYED(_t)                                  \
    (BCMLTM_TABLE_TYPE_LT_HASH(_t) || BCMLTM_TABLE_TYPE_LT_TCAM(_t))


/*!
 * \brief LTM Table Type.
 *
 * This specifies the table type based on the flags
 * defined in BCMLTM_TABLE_TYPE_FLAG_xxx.
 */
typedef uint32_t bcmltm_table_type_t;


/*!
 * \brief Table Operating Modes.
 *
 * A Logical table has 2 modes of operations which is specified
 * in the LT map file:
 *    Modeled
 *    Interactive
 *
 * This mode indicates the corresponding PTM interface, modeled or
 * interative, to be used during the PTM operation.
 *
 * Note that PT Pass Thru always operates on interactive mode.
 */
typedef enum bcmltm_table_mode_e {
    /*! Modeled mode. */
    BCMLTM_TABLE_MODE_MODELED = 0,

    /*! Interactive mode. */
    BCMLTM_TABLE_MODE_INTERACTIVE = 1,
} bcmltm_table_mode_t;


/* Table Flags */
/*! Read only Table flag. */
#define BCMLTM_TABLE_FLAG_READ_ONLY    0x00000001

/*! Overlay Table flag (table share device resource entries). */
#define BCMLTM_TABLE_FLAG_OVERLAY      0x00000002

/*!
 * \brief Table Attributes.
 *
 * This structure contains the table attributes.
 * For LTs, the table attributes are derived from the LT map file.
 */
typedef struct bcmltm_table_attr_s {
    /*!
     * Table type.
     */
    bcmltm_table_type_t  type;

    /*!
     * Operation mode: Modeled or Interactive.
     */
    bcmltm_table_mode_t mode;

    /*!
     * Table flag.
     */
    uint32_t flag;

    /*!
     * Minimum table index allowed.
     * This value normally corresponds to the PT index.
     */
    uint32_t index_min;

    /*!
     * Maximum table index allowed.
     * This value normally corresponds to the PT index.
     */
    uint32_t index_max;

    /*!
     * Minimum memory parameter value allowed.
     * This value normally corresponds to the PT memory port or instance.
     */
    uint32_t mem_param_min;

    /*!
     * Maximum memory parameter value allowed.
     * This value normally corresponds to the PT memory port or instance.
     */
    uint32_t mem_param_max;

    /*!
     * Global LT ID.
     * Overlay tables share device resource entries and requires
     * a table to maintain the global in-use bitmap.  This is the ID
     * of such global table.
     */
    bcmltd_sid_t global_ltid;
} bcmltm_table_attr_t;


/*!
 * \brief LTM Field Types.
 *
 * LTM has the following field map types:
 *    Keys
 *    Values
 *
 * LTM constructs different metadata field lists depending on the field
 * map type in order to process the field map at the proper time and
 * for the appropriate opcode.
 *
 * For instance, Key field maps are processed ealier.  In addition,
 * only Key field maps are used for certain operations, such as
 * LOOKUP and DELETE.
 *
 * Note this type reflects the type of the field map, not how the field
 * is defined. For example, an LT field is defined as a Key, but it
 * maps to a PT Key and a PT Value, then there will be 2 field maps,
 * one of type Key and another of type Value:
 *     LT:  LTF1: PT1.__INDEX  --> Key
 *          LTF1: PT1.PTF5     --> Value
 */
typedef enum bcmltm_field_type_e {
    /*! Field maps to a Key. */
    BCMLTM_FIELD_TYPE_KEY = 0,

    /*! Field maps to a Value. */
    BCMLTM_FIELD_TYPE_VALUE = 1,
} bcmltm_field_type_t;


/*!
 * \brief LTM Field Direction.
 *
 * Indicates if the field direction is INPUT or OUTPUT.
 *
 * This information is needed for cases where the working buffer offset
 * of the field differs depending on the direction of a field operation.
 * For instance, Keyed LTs use different buffers for the PTM
 * input and output entries.
 */
typedef enum bcmltm_field_dir_e {
    /*! Input field. */
    BCMLTM_FIELD_DIR_IN = 0,

    /*! Output field. */
    BCMLTM_FIELD_DIR_OUT = 1,
} bcmltm_field_dir_t;


/*!
 * \brief Maximum supported LTs referenced in a single transaction
 */
#define BCMLTM_TRANSACTION_LT_MAX       8

/*!
 * \brief Transaction tracking structure.
 *
 * This structure caches the information that must be preserved over
 * the duration of a transaction.
 * Note that only an Atomic Transaction will have multiple LT entries.
 * The transaction ID must be identical for each of the LT entries
 * in a transaction.
 */
typedef struct bcmltm_transaction_status_s {
    /*!
     * Transaction ID for the LT entry specificaion of this operation.
     * Note that multiple entries may have the same ID.
     * Shared ID will bundle the entries into an atomic operation.
     */
    uint32_t trans_id;

    /*! Number of LTIDs included in this transaction so far. */
    uint8_t lt_num;

    /*! Logical Table ID. */
    uint32_t *ltid_list;

} bcmltm_transaction_status_t;

/*!
 * \brief Retrieve statistics array of a LT.
 *
 * Fetch the statistics array of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_stats_p Pointer to the statistics array for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int bcmltm_internal_stats_lt_get(int unit,
                                         uint32_t ltid,
                                         uint32_t **lt_stats_p);

/*!
 * \brief Retrieve total state of a LT.
 *
 * Fetch the total working state of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_state Pointer to the state for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int bcmltm_internal_state_lt_get(int unit,
                                         uint32_t ltid,
                                         bcmltm_lt_state_t **lt_state);

/*!
 * \brief Retrieve info values for a LT.
 *
 * Fetch the specific info value of a particular Logical Table.
 * This function is the high-level implementation which is registered
 * via callback to provide access to lower-level LTM submodules.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Target Logical Table ID.
 * \param [in] field_id The ID number for this TABLE_INFO field.
 * \param [in] field_idx The array index for this TABLE_INFO field.
 * \param [out] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int bcmltm_internal_info_lt_get(int unit,
                                        uint32_t table_id,
                                        uint32_t field_id,
                                        uint32_t field_idx,
                                        uint64_t *data_value);

#endif /* BCMLTM_INTERNAL_H */
