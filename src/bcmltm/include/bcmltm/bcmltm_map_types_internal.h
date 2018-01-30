/*! \file bcmltm_map_types_internal.h
 *
 * Logical Table Manager Mapping definitions.
 *
 * The following definitions are used to determine the mapping
 * for a given table.  Most types are used primarily for Logical
 * Tables when parsing the mapping information from the LRD.
 *
 * This information is eventually used to construct the LTM metadata
 * for the creation of the FA and EE cookies.
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

#ifndef BCMLTM_MAP_TYPES_INTERNAL_H
#define BCMLTM_MAP_TYPES_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltd/bcmltd_handler.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

/*!
 * \brief Physical Table Information.
 *
 * This is used to construct the LTM metadata.  For instance, the
 * information indicates the Working Buffer content to use for the
 * PTM interface.
 */
typedef struct bcmltm_map_pt_info_s {
    /*! Physical table ID. */
    bcmdrd_sid_t sid;

    /*!
     * Bit flags to denote physical table parameters:
     *     BCMLTM_PT_OP_FLAGS_xxx
     * These are used to indicate the content and format
     * for the PTM parameter list and the entry data sections of the
     * generic working buffer layout.
     */
    uint32_t flags;

    /*! The number of PTM operations to perform for this PT. */
    uint32_t num_ops;

    /*! Working buffer block ID for this PT. */
    bcmltm_wb_block_id_t wb_block_id;
} bcmltm_map_pt_info_t;

/*!
 * \brief Information for all PTs associated with a LTM table.
 *
 * This information is used to construct the LTM metadata.
 */
typedef struct bcmltm_map_pt_s {
    /*! Number of different physical tables mapped to given LTM table. */
    uint32_t num_pt;

    /*!
     * Variable length array of PT info.
     * The index for a given PT is the same index used to construct
     * the PT information in bcmltm_pt_list_t.
     */
    bcmltm_map_pt_info_t info[];
} bcmltm_map_pt_t;

/*!
 * \brief Information for all Field Validation groups for a LT.
 *
 * This information is used to construct the LTM metadata.
 */
typedef struct bcmltm_map_fv_s {
    /*! Number of Field Validation groups for given LT. */
    uint32_t num;
} bcmltm_map_fv_t;

/*!
 * \brief Information for all Table Commit groups for a LT.
 *
 * This information is used to construct the LTM metadata.
 */
typedef struct bcmltm_map_tc_s {
    /*! Number of Table Commit groups for given LT. */
    uint32_t num;
} bcmltm_map_tc_t;

/*!
 * \brief Information for all Customer Table Handler groups for a LT.
 *
 * This information is used to construct the LTM metadata.
 */
typedef struct bcmltm_map_cth_s {
    /*! Number of custom table handlers groups for given LT. */
    uint32_t num;
} bcmltm_map_cth_t;

/*!
 * \brief Information for all LTM map groups for a LT.
 *
 * This information is used to construct the LTM metadata.
 */
typedef struct bcmltm_map_ltm_s {
    /*! Number of LTM map groups for given LT. */
    uint32_t num;
} bcmltm_map_ltm_t;

/*!
 * \brief Map groups for a LTM table.
 */
typedef struct bcmltm_map_groups_s {
    /*! Physical Groups Mapping. */
    bcmltm_map_pt_t *pt;

    /*! Field Validation Groups Mapping. */
    bcmltm_map_fv_t *fv;

    /*! Table Commit Groups Mapping. */
    bcmltm_map_tc_t *tc;

    /*! Customer Table Handler Groups Mapping. */
    bcmltm_map_cth_t *cth;

    /*! LTM Groups Mapping. */
    bcmltm_map_ltm_t *ltm;
} bcmltm_map_groups_t;


/*!
 * \brief Field map for direct mapped field.
 *
 * Contains field map information from parsed LRD map entries of type:
 *     BCMLRD_MAP_ENTRY_MAPPED_KEY
 *     BCMLRD_MAP_ENTRY_MAPPED_VALUE
 */
typedef struct bcmltm_map_field_mapped_s {
    /*! API-facing field ID. */
    uint32_t src_field_id;

    /*! The index of the field in an array. */
    uint32_t src_field_idx;

    /*! The default value for this field. */
    uint64_t default_value;

    /*! The minimum value for this field. */
    uint64_t minimum_value;

    /*! The maximum value for this field. */
    uint64_t maximum_value;

    /*! Destination field ID (PT). */
    uint32_t dst_field_id;

    /*! PTM WB block for this field map. */
    const bcmltm_wb_block_t *ptm_wb_block;

    /*! Keyed PT entry index (extended physical field). */
    uint16_t pt_entry_idx;

    /*! The WBC minimum bit position of this field. */
    uint16_t wbc_minbit;

    /*! The WBC maximum bit position of this field. */
    uint16_t wbc_maxbit;

    /*! Next field map. */
    struct bcmltm_map_field_mapped_s *next;
} bcmltm_map_field_mapped_t;


/*!
 * \brief Field map list for direct mapped fields LT to PT.
 *
 * Contains list of field maps information from parsed LRD map entries of type:
 *     BCMLRD_MAP_ENTRY_MAPPED_KEY
 *     BCMLRD_MAP_ENTRY_MAPPED_VALUE
 */
typedef struct bcmltm_map_field_mapped_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid fields in array. */
    uint32_t num;

    /*! Array of field map. */
    bcmltm_map_field_mapped_t *field_maps;
} bcmltm_map_field_mapped_list_t;


/*!
 * \brief LT mapping for fixed fields.
 *
 * Contains mapping entries of type:
 *     BCMLRD_MAP_ENTRY_FIXED_KEY
 *     BCMLRD_MAP_ENTRY_FIXED_VALUE
 */
typedef struct bcmltm_map_field_fixed_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid fields in array. */
    uint32_t num;

    /*! Array of fixed field map. */
    bcmltm_fixed_field_t *field_maps;
} bcmltm_map_field_fixed_list_t;

/*!
 * \brief Physical field information for Field Transform.
 *
 * Contains field wbc information for Field Transform.
 */
typedef struct bcmltm_xfrm_dst_field_s {
    /*! Array of Field IDs. */
    uint32_t field_id;

    /*! Physical field information. */
    bcmltm_wb_coordinate_t wbc;

} bcmltm_xfrm_dst_field_t;

/*!
 * \brief All physical fields information for Field Transform.
 *
 * Contains all field wbc information for Field Transform.
 */
typedef struct bcmltm_xfrm_dst_fields_s {
    /*! Physical table ID of destination fields. */
    bcmdrd_sid_t ptid;

    /*! Number of fields. */
    uint32_t num;

    /*! Array of physical field(wbc) information. */
    bcmltm_xfrm_dst_field_t *field;

} bcmltm_xfrm_dst_fields_t;

/*!
 * \brief LT mapping for Transform functions.
 *
 * Contains mapping entries of type:
 *     BCMLRD_MAP_ENTRY_FWD/REV_KEY/VALUE_FIELD_XFRM_HANDLER.
 */
typedef struct bcmltm_map_field_xfrm_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid xfrm in array. */
    uint32_t num;

    /*! Array of xfrm handler. */
    bcmltd_xfrm_handler_t *xfrm;

    /*! Destination field(wbc) info per xfrm. */
    bcmltm_xfrm_dst_fields_t *xfrm_dst;

} bcmltm_map_field_xfrm_list_t;

/*!
 * \brief LT mapping for Field Valiadtion.
 *
 * Contains mapping entries of type:
 *     BCMLRD_MAP_ENTRY_KEY/VALUE_FIELD_VALIDATION.
 */
typedef struct bcmltm_map_fv_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid field validation in array. */
    uint32_t num;

    /*! Array of field validation handler. */
    bcmltd_field_val_handler_t *fv;

} bcmltm_map_fv_list_t;


/*!
 * \brief LT entries mapping information for Physical destination groups.
 *
 * This structure is used to hold information parsed from the LRD map entries.
 *
 * The information is then copied into the static internal
 * data structure which will contain the adequate array
 * size for the different field map lists.  The static internal data
 * structure lt_info holds information used and referenced by the LTM
 * metadata, such as cookies for the different FA and EE nodes.
 */
typedef struct bcmltm_map_entries_pt_s {
    /*! Indicates if table supports access per instance. */
    bool table_inst;

    /*! Indicates if this pt group has table transform. */
    bool table_xfrm;

    /*! Key field direct maps. */
    bcmltm_map_field_mapped_list_t keys;

    /*! Value field direct maps. */
    bcmltm_map_field_mapped_list_t values;

    /*! Fixed field maps. */
    bcmltm_map_field_fixed_list_t fixed;

    /*! Forward Key Transform handler. */
    bcmltm_map_field_xfrm_list_t fwd_key_xfrms;

    /*! Forward Value Transform handler. */
    bcmltm_map_field_xfrm_list_t fwd_value_xfrms;

    /*! Reverse Key Transform handler. */
    bcmltm_map_field_xfrm_list_t rev_key_xfrms;

    /*! Reverse Value Transform handler. */
    bcmltm_map_field_xfrm_list_t rev_value_xfrms;
} bcmltm_map_entries_pt_t;


/*!
 * \brief Logical Field.
 *
 * A given logical field is identified by its Field ID and its index.
 */
typedef struct bcmltm_map_field_s {
    /*! Field ID. */
    uint32_t fid;

    /*! The index of the field in an array. */
    uint32_t idx;

    /*! The minimum value for this field. */
    uint64_t minimum_value;

    /*! The maximum value for this field. */
    uint64_t maximum_value;
} bcmltm_map_field_t;

/*!
 * \brief LT entries mapping information for Field Validation groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the Field Validation Handler groups.
 */
typedef struct bcmltm_map_entries_fv_s {

    /*! Field Validation map for Key fields. */
    bcmltm_map_fv_list_t validate_keys;

    /*! Field Validation map for Value fields. */
    bcmltm_map_fv_list_t validate_values;

} bcmltm_map_entries_fv_t;

/*!
 * \brief LT entries mapping information for Table Commit groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the Table Commit Handler groups.
 */
typedef struct bcmltm_map_entries_tc_s {
    /*! Number of Table Commit handlers. */
    uint32_t num_handlers;

    /*! Array of Table Commit handler ids. */
    uint32_t *handler_id;
} bcmltm_map_entries_tc_t;

/*!
 * \brief LT entries mapping information for Customer Table Handler groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the Custom Table Handler groups.
 */
typedef struct bcmltm_map_entries_cth_s {
    /*! Custom Table Handler. */
    const bcmltd_table_handler_t *handler;

    /*! Number of total key field elements (fid, idx). */
    uint32_t num_key_fields;

    /*! Array of fields. */
    bcmltm_map_field_t *key_field;

    /*! Number of total field elements (fid, idx). */
    uint32_t num_fields;

    /*! Array of fields. */
    bcmltm_map_field_t *field;
} bcmltm_map_entries_cth_t;

/*!
 * \brief LT entries mapping information for LTM groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the LTM group.
 */
typedef struct bcmltm_map_entries_ltm_s {
    /*! Number of total key field elements (fid, idx). */
    uint32_t num_key_fields;

    /*! Array of fields. */
    bcmltm_map_field_t *key_field;

    /*! Number of total field elements (fid, idx). */
    uint32_t num_value_fields;

    /*! Array of fields. */
    bcmltm_map_field_t *value_field;

} bcmltm_map_entries_ltm_t;


#endif /* BCMLTM_MAP_TYPES_INTERNAL_H */
