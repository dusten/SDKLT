/*! \file bcmltm_wb_types_internal.h
 *
 * Logical Table Manager Working Buffer Types Definitions.
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

#ifndef BCMLTM_WB_TYPES_INTERNAL_H
#define BCMLTM_WB_TYPES_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Working Buffer Block identifier.
 */
typedef uint32_t bcmltm_wb_block_id_t;


/*!
 * \brief Invalid working buffer block ID.
 */
#define BCMLTM_WB_BLOCK_ID_INVALID    ((bcmltm_wb_block_id_t)-1)


/*!
 * \brief Invalid working buffer offset.
 */
#define BCMLTM_WB_OFFSET_INVALID    0


/*!
 * \brief Working Buffer Block Types.
 *
 * This specifies the working buffer block type.
 *
 * Each block type has a specific memory layout and offset placement
 * for the different pieces of information required by the type.
 */
typedef enum bcmltm_wb_block_type_e {
    /*! PTM Index Table block type (used for LT and Pass Thru). */
    BCMLTM_WB_BLOCK_TYPE_PTM_INDEX = 1,

    /*! PTM Keyed Pass Thru Table block type. */
    BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU = 2,

    /*! PTM Keyed LT Table block type. */
    BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT = 3,

    /*! LT Index block type. */
    BCMLTM_WB_BLOCK_TYPE_LT_INDEX = 4,

    /*! LTA Custom Table Handler block type. */
    BCMLTM_WB_BLOCK_TYPE_LTA_CTH = 5,

    /*! LTA Field Transform block type. */
    BCMLTM_WB_BLOCK_TYPE_LTA_XFRM = 6,

    /*! LTA Field Validation block type. */
    BCMLTM_WB_BLOCK_TYPE_LTA_VAL = 7,
} bcmltm_wb_block_type_t;


/*!
 * \brief Working Buffer Block for PTM types.
 *
 * This structure contains the working buffer block information for
 * the following PTM block types:
 *     BCMLTM_WB_BLOCK_TYPE_PTM_INDEX
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT
 *
 * Although each specific type has a different information, they
 * all share the basic block information and contains 2 sections:
 *    Parameter section (PTM parameters, such as index, port, etc.)
 *    Entry section (PTM entry buffers, such as Key, Data).
 *
 * This PTM block type has sufficient information to derive
 * individual offsets to parameters and entries key/data.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_ptm_s {
    /*! Physical table ID. */
    bcmdrd_sid_t ptid;

    /*! The number of PTM operations to perform for this PT. */
    uint32_t num_ops;

    /*! Size of param section for each PTM op. */
    uint32_t param_wsize;

    /*! Size of entry section for each PTM op. */
    uint32_t entry_wsize;

    /*! Size for each PTM op. */
    uint32_t op_wsize;

    struct {
        /*! PTM parameter offset for first PTM op. */
        uint32_t param;

        /*! PTM entry offset for first PTM op. */
        uint32_t entry;
    } offset; /*!< PTM offsets for first PTM op.*/

} bcmltm_wb_block_ptm_t;


/*!
 * \brief LT Index Offsets.
 *
 * This structure contains the working buffer offsets for the
 * different information in the LT Index block.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_lt_index_offset_s {
    /*!
     * Offset of effective LT index used for tracking in-use entries.
     *
     * This track index is a result of combining the PT index and
     * additonal memory parameters, such a per-port or per-pipe.
     */
    uint32_t track;

    /*! Offset for the index absent status. */
    uint32_t index_absent;
} bcmltm_wb_lt_index_offset_t;


/*!
 * \brief Working Buffer Block for LT Index type.
 *
 * This structure contains the working buffer block information for
 * the following LT block type:
 *     BCMLTM_WB_BLOCK_TYPE_LT_INDEX
 *
 * This block is used LTs of type Simple Index and Index with Allocation.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_lt_index_s {

    struct {
        /*! Index section. */
        bcmltm_wb_lt_index_offset_t index;
    } offset; /*!< Offsets. */

} bcmltm_wb_block_lt_index_t;


/*!
 * \brief LTA Field Offsets.
 *
 * This structure contains the working buffer offsets for the
 * different information in a LTA field structure bcmltd_fields_t.
 *
 * The LTA field type bcmltd_fields_t is used as argument
 * to pass set of fields into the LTA interfaces such as
 * custom table handlers, field transforms, and field validations.
 *
 * There are 3 sections in the working buffer for a LTA fields struct:
 *     Fields container:              bcmltd_fields_t
 *     Array of pointers to field:   (bcmltd_field_t *) x num_fields
 *     Array of field structs:       (bcmltd_field_t)   x num_fields
 *
 * The number 'num_fields' is the count of the array large
 * enough to accommodate all field elements (fid, idx).
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_lta_fields_offset_s {
    /*!
     * Offset for the LTA fields structure (bcmltd_fields_t)
     * which contains the set of fields.
     */
    uint32_t param;

    /*!
     * Offset for the array of pointers to fields (bcmltd_field_t),
     * to be inserted into the fields structure above.
     */
    uint32_t ptrs;

    /*!
     * Offset for the array of fields bcmltd_field_t (bcmlt_field_list_t),
     * to be referenced by the array of pointers above.
     */
    uint32_t field;
} bcmltm_wb_lta_fields_offset_t;


/*!
 * \brief Working Buffer Block for LTA Input/Output block types.
 *
 * This structure contains the working buffer block information for
 * the block types which require input and output fields:
 *     BCMLTM_WB_BLOCK_TYPE_LTA_CTH
 *     BCMLTM_WB_BLOCK_TYPE_LTA_XFRM
 *     BCMLTM_WB_BLOCK_TYPE_LTA_VAL
 *
 * The LTA block contains 2 sections:
 *     Input Fields
 *     Output Fields
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_lta_s {
    /*! The number of Input fields. */
    uint32_t in_num_fields;

    /*! The number of Output fields. */
    uint32_t out_num_fields;

    struct {
        /*! Offset for the Input fields. */
        bcmltm_wb_lta_fields_offset_t in;

        /*! Offset for the Output fields. */
        bcmltm_wb_lta_fields_offset_t out;
    } offset; /*!< Offsets for LTA sections: Input and Output. */

} bcmltm_wb_block_lta_t;


/*!
 * \brief Working Buffer Block.
 *
 * This structure contains the working buffer information for
 * a given block.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_s {
    /*! Working buffer block ID. */
    bcmltm_wb_block_id_t id;

    /*! Working buffer size of this block in words. */
    uint32_t wsize;

    /*! Working buffer base offset for this block. */
    uint32_t base_offset;

    /*! Block type. */
    bcmltm_wb_block_type_t type;

    union {
        /*!
         * PTM block types:
         *     BCMLTM_WB_BLOCK_TYPE_PTM_INDEX
         *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU
         *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT
         */
        bcmltm_wb_block_ptm_t ptm;

        /*!
         * LT block type:
         *     BCMLTM_WB_BLOCK_TYPE_LT_INDEX
         */
        bcmltm_wb_block_lt_index_t lt_index;

        /*!
         * LTA block types:
         *     BCMLTM_WB_BLOCK_TYPE_LTA_CTH
         *     BCMLTM_WB_BLOCK_TYPE_LTA_XFRM
         *     BCMLTM_WB_BLOCK_TYPE_LTA_VAL
         */
        bcmltm_wb_block_lta_t lta;
    } u; /*!< Union of block type structures. */


    /*! Next working buffer block. */
    struct bcmltm_wb_block_s *next;
} bcmltm_wb_block_t;


/*!
 * \brief Working Buffer Handle.
 *
 * This structure contains the working buffer information for
 * a given table.
 *
 * This handle maintains the list of all the working
 * buffer blocks that pertains to the table.
 *
 * All size and offset information are in words.
 * LTM working buffer offset calculations are all based on uint32_t.
 *
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_handle_s {
    /*! Total working buffer size in words. */
    uint32_t wsize;

    /*! Table ID. */
    uint32_t sid;

    /*! Number of blocks. */
    uint32_t num_blocks;

    /*! List of working buffer blocks. */
    bcmltm_wb_block_t *blocks;
} bcmltm_wb_handle_t;


#endif /* BCMLTM_WB_TYPES_INTERNAL_H */
