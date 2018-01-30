/*! \file bcmfp_lt_internal.h
 *
 * This file defines for LT APIs for usage.
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

#ifndef BCMFP_LT_INTERNAL_H
#define BCMFP_LT_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <shr/shr_types.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

/*******************************************************************************
  Defines
 */

/*! Invalid Field */
#define BCMFP_LT_FIELD_INVALID ((uint32_t) -1)

/*! Entry LT Type */
#define BCMFP_LT_TYPE_ENTRY         (0)

/*! Group LT Type */
#define BCMFP_LT_TYPE_GROUP         (1)

/*! Presel Entry LT Type */
#define BCMFP_LT_TYPE_PRESEL_ENTRY  (2)

/*! Rule LT Type */
#define BCMFP_LT_TYPE_RULE          (3)

/*! Policy LT Type */
#define BCMFP_LT_TYPE_POLICY        (4)

/*! Source Class Mode LT Type */
#define BCMFP_LT_SRC_CLASS_MODE     (5)

/*! LT count. */
#define BCMFP_LT_TYPE_COUNT         (6)

/*! Key Size for In-Mem Key Size */
#define BCMFP_LT_IMM_KEY_SZ_4BYTE   (4)

/*!
 * In-Memory field Data Format (1) for In-Mem.
 * Field Idx : 1 Byte.
 * Field data : 3 bytes.
 */
#define BCMFP_LT_IMM_FDATA_FORMAT_I1D3      (1 << 0)

/*! LT flag - In-Mem Support Requires. */
#define BCMFP_LT_FLAGS_INMEM_SUPPORT        (1 << 0)

/*! LT flag - Presel Field Identifier Valid . */
#define BCMFP_LT_FLAGS_PRESEL_VALID         (1 << 1)

/*! LT flag - Virtual Group Valid . */
#define BCMFP_LT_FLAGS_VIRTUAL_SLICE_GRP    (1 << 2)

/*! LT flag - Group Priority Unique. */
#define BCMFP_LT_FLAGS_UNIQUE_GRP_PRI       (1 << 3)

/*! LT flag - Fixed key group. */
#define BCMFP_LT_FLAGS_FIXED_KEY_GRP        (1 << 4)

/*! LT flag - Exact Match group. */
#define BCMFP_LT_FLAGS_HASH_KEY_GRP               (1 << 5)

/*! LT flag - No Meter Id Support. */
#define BCMFP_LT_FLAGS_NO_METER_SUPPORT     (1 << 6)

/*******************************************************************************
  Typedefs
 */

/*! Enum defines sub component Id for in-memory use of various LTs */
typedef enum {
    /*! Sub-Component Id for Ingress Entry LT. */
    BCMFP_LT_IMM_SUBID_ING_ENTRY = 1,

    /*! Sub-Component Id for Ingress Group Template LT. */
    BCMFP_LT_IMM_SUBID_ING_GRP_TEMPLATE = 2,

    /*! Sub-Component Id for Ingress Presel Entry template LT. */
    BCMFP_LT_IMM_SUBID_ING_PRESEL_ENTRY_TEMPLATE = 3,

    /*! Sub-Component Id for Ingress Rule Template LT.  */
    BCMFP_LT_IMM_SUBID_ING_RULE_TEMPLATE = 4,

    /*! Sub-Component Id for Ingress Policy Template LT. */
    BCMFP_LT_IMM_SUBID_ING_POLICY_TEMPLATE = 5,

    /*! Sub-Component Id for Vlan Entry LT. */
    BCMFP_LT_IMM_SUBID_VLAN_ENTRY = 6,

    /*! Sub-Component Id for Vlan Group Template LT. */
    BCMFP_LT_IMM_SUBID_VLAN_GRP_TEMPLATE = 7,

    /*! Sub-Component Id for Vlan Rule Template LT. */
    BCMFP_LT_IMM_SUBID_VLAN_RULE_TEMPLATE = 8,

    /*! Sub-Component Id for Vlan Policy Template LT. */
    BCMFP_LT_IMM_SUBID_VLAN_POLICY_TEMPLATE = 9,

    /*! Sub-Component Id for Egress Entry LT. */
    BCMFP_LT_IMM_SUBID_EGR_ENTRY = 10,

    /*! Sub-Component Id for Egress Group Template LT. */
    BCMFP_LT_IMM_SUBID_EGR_GRP_TEMPLATE = 11,

    /*! Sub-Component Id for Egress Rule Template LT. */
    BCMFP_LT_IMM_SUBID_EGR_RULE_TEMPLATE = 12,

    /*! Sub-Component Id for Egress Policy Template LT. */
    BCMFP_LT_IMM_SUBID_EGR_POLICY_TEMPLATE = 13,

    /*! Sub-Component Id for Ingress Source Class Mode Entry LT. */
    BCMFP_LT_IMM_SUBID_ING_SRC_CLASS_MODE = 14
} bcmfp_lt_imm_sub_id_t;

/*! Generic Function pointer for LT field Mapping */
typedef int (*bcmfp_lt_map_f)(int unit,
                              uint32_t fid,
                              uint32_t idx,
                              void *map);

/*!
 * \brief Information of field in a logical table
 *
 * These information about field in a particular LT is used for validation,
 * populating configuration structure for specified LT and storing field
 * data content to in-mem component.
 */
typedef struct bcmfp_lt_field_info_s {
    /*! Field Identifier. */
    uint32_t fid;

    /*! Access type */
    bcmltd_field_acc_t access;

    /*! Field data type */
    bcmltd_field_data_tag_t dtag;

    /*! Minimum value for field data */
    bcmltd_field_data_t min;

    /*! Maximum value for field data */
    bcmltd_field_data_t max;

    /*! Default value for field */
    bcmltd_field_data_t def;

    /*! Maximum number of bits in field data */
    uint32_t width;

    /*! Depth param used with field is array */
    uint32_t depth;

    /*! Number of blocks required to store */
    uint32_t elements;

    /*! is field array */
    bool is_array;

    /*! is field symbol */
    bool is_symbol;

    /*! Index from first Field */
    uint32_t field_coord;

} bcmfp_lt_field_info_t;

/*!
 * \brief FP logical Table global information structure.
 *
 * This structure is populated at init time with all relevent LTs and its
 * configuration.
 */
typedef struct bcmfp_lt_info_s {
    /*! LT Flags - BCMFP_LT_FLAGS_XXX */
    uint32_t flags;

    /*! Logical table identifier */
    bcmltd_sid_t ltid;

    /*! Key Fid */
    bcmltd_fid_t key_fid;

    /*! Lt Type - BCMFP_LT_TYPE_XXX. */
    uint32_t lt_type;

    /*! FP Stage to which this LT belongs to. */
    bcmfp_stage_id_t stage_id;

    /*! Last field Index valid in logical table */
    uint32_t fields;

    /*! Pointer to the array of fields */
    bcmfp_lt_field_info_t *fields_info;

    /*! Number of fields to populate in bcmltd_fields_t */
    uint32_t out_fields;

    /* Function for LT field to internal qualifier/action mapping */
    bcmfp_lt_map_f  map_get;

    /*! List of Field Ids which has special meaning. */
    void *sp_fids;

    /*! Pointer to the array of readonly field's fid. */
    uint32_t *add_fields_fid;

    /*! Number of additional fields. */
    uint32_t add_fields_count;

    /*! Component id */
    uint8_t comp_id;

    /*! Sub Component Id */
    uint8_t sub_id;

    /*! Key Size */
    size_t key_sz;

    /*! Data format used for saving in in-mem */
    uint32_t fdata_format;

    /*! Initial number of Rows specified for creating in-mem db */
    size_t rows;

    /*! is In-memory backend table created */
    bool imm_init;

    /*! In-memory backend table handler */
    bcmimm_be_tbl_hdl_t imm_hdl;

    /*! Number of in-memory entry fields to populate bcmimm_be_fields_t */
    uint32_t imm_out_fields;

} bcmfp_lt_info_t;

/*! Function pointer for initializing LT specific information. */
typedef int (*bcmfp_lt_init_f)(int unit,
                               bcmfp_lt_info_t *lt_info);

/*! Function pointer for de-initializing LT specific information. */
typedef int (*bcmfp_lt_deinit_f)(int unit,
                                 bcmfp_lt_info_t *lt_info);

/*! FP LTs control */
typedef struct bcmfp_lt_control_s {
    /*! If FP_CONFIG LT is inserted once */
    bool fp_config_initialized;

    /*! List of Lt information */
    bcmfp_lt_info_t *lt_info_list[BCMFP_STAGE_ID_COUNT][BCMFP_LT_TYPE_COUNT];
} bcmfp_lt_control_t;

/*!
 * \brief Initialize LT control for FP
 *
 * This API initialize LT control for FP on a particular device.
 *
 * \param [in] unit Device number for the table of interest.
 *
 * \retval SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_init(int unit);

/*!
 * \brief De-initialize LT control for FP
 *
 * This API cleanup LT control for FP on a particular device. If
 * inmem DB for any LT is not closed before calling this API,
 * information is lost.
 *
 * \param [in] unit Device number for the table of interest.
 *
 * \retval SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_cleanup(int unit);

/*!
 * \brief Returns pointer to LT information for LT Identifier
 *
 * This API returns a pointer to LT information for specified LT on a
 * particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] ltid     Logical table identifier
 * \param [in] lt_info  Pointer Addrress for LT information
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_info_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmfp_lt_info_t **lt_info);

/*!
 * \brief Returns pointer to LT information for stage and LT type
 *
 * This API returns a pointer to LT information for specified stage and LT type
 * on a particular device.
 *
 * \param [in] unit     Device number for the table of interest.
 * \param [in] stage_id FP stage
 * \param [in] lt_type  FP Logical Table Type
 * \param [in] lt_info  Pointer Addrress for LT information
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_info_get_by_type(int unit,
                  bcmfp_stage_id_t stage_id,
                  uint32_t lt_type,
                  bcmfp_lt_info_t **lt_info);

/*!
 * \brief Retrieve Init status of FP_CONFIG LT
 *
 * This API retrieves init status of FP_CONFIG LT on a particular device.
 * Here Init means that INSERT operation is called on the LT. This state
 * maintainance is required as all opcodes - INSERT/LOOKUP/DELETE/UPDATE
 * must works like any other LTs.
 *
 * \param [in] unit device number for the table of interest.
 * \param [out] init pointer to init status of FP_CONFIG LT.
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_config_init_get(int unit,
                      bool *init);

/*!
 * \brief Sets init status of FP_CONFIG LT
 *
 * This API sets init status of FP_CONFIG LT on a particular device.
 * Here Init means that INSERT operation is called on the LT. This state
 * maintainance is required as all opcodes - INSERT/LOOKUP/DELETE/UPDATE
 * must works like any other LTs.
 *
 * \param [in] unit device number for the table of interest.
 * \param [in] init init status of FP_CONFIG LT.
 *
 * \return SHR_E_NONE success, otherwise failure in obtaining information
 */
extern int
bcmfp_config_init_set(int unit,
                      bool init);

#endif /* BCMFP_LT_INTERNAL_H */
