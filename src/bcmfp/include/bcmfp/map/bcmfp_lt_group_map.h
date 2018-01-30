/*! \file bcmfp_lt_group_map.h
 *
 * APIs, defines for Group LT Initialization and LT Field mapping
 *
 * This file contains APIs, defines for handling of initialization of
 * Group LT and LT fields to qualifier mapping for Logical table.
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

#ifndef BCMFP_LT_GROUP_MAP_H
#define BCMFP_LT_GROUP_MAP_H

/*******************************************************************************
  Includes
 */
#include <bcmfp/bcmfp_lt_internal.h>

/*******************************************************************************
  Typedefs
 */

/*!
 * \brief FP Group LTs important fields identifier structure.
 *
 * Structure for special fids in Group LT which needs validation and are
 * needs to be handled individually.
 */

typedef struct bcmfp_lt_group_sp_fids_s {
    /*! Field Identifier for Group Id. */
    uint32_t group_id_fid;

    /*! Field Identifier for policy Id. */
    uint32_t policy_id_fid;

    /*! Field Identifier for Pipe. */
    uint32_t pipe_fid;

    /*! Field Identifier for Group Priority. */
    uint32_t priority_fid;

    /*! Field Identifier for Group User Configured Mode. */
    uint32_t mode_fid;

    /*!
     * Field Identifier for Group Mode Auto. This indicates if algorithm
     * need to use user given \c mode_fid or use auto mode.
     */
    uint32_t mode_auto_fid;

    /*! Field Identifier for Group operating mode. */
    uint32_t mode_oper_fid;

    /*! Field Identifier for Virtual Slice Group. */
    uint32_t virtual_slice_grp_fid;

    /*! Field Identifier for Group's Port Packet Type. */
    uint32_t port_pkt_type_fid;

    /*! Field Identifier for count of active Presel Ids from
     * FP_XXX_PRESEL_ENTRY_TEMPATE LTs. */
    uint32_t presel_ids_set_count_fid;

    /*! Field Identifier for Presel Ids from FP_XXX_PRESEL_ENTRY_TEMPATE LTs. */
    uint32_t presel_ids_set_fid;

    /*! Table Identifier for Presel for which \c presel_ids_set_fid is given. */
    uint32_t presel_lt_sid;

    /*!
     * Field Identifier for Reference count in FP_XXX_PRESEL_ENTRY_TEMPLATE
     * LTs for which  \c presel_ids_set_fid is given.
     */
    uint32_t presel_lt_ref_count_fid;

    /*! Field Identifier for Presel Id. */
    uint32_t presel_lt_id_fid;

    /*!
     * Field Identifier for mixed source class mask qualifier  in
     * FP_XXX_PRESEL_ENTRY_TEMPLATE
     */
    uint32_t presel_lt_mixed_src_class_mask_fid;

    /*! Table Identifier for SRC_CLASS_MODE for which REF_COUNT to be updated.*/
    uint32_t src_class_mode_lt_sid;

    /*!
     * Field Identifier for EM Group lookup in FP_EM_GRP_TEMPLATE LT
     */
    uint32_t group_lookup_id_fid;

    /*!
     * Field Identifier for Group LTID in FP_EM_GRP_TEMPLATE LT
     */
    uint32_t group_ltid_fid;

    /*!
     * Field Identifier for auto Group LTID in FP_EM_GRP_TEMPLATE LT.
     * This determine if the algorithm has to use the auto LTID or user given LTID
     */
    uint32_t group_ltid_auto_fid;

    /*!
     * Field Identifier for operating Group LTID in FP_EM_GRP_TEMPLATE LT
     */
    uint32_t group_ltid_oper_fid;

} bcmfp_lt_group_sp_fids_t;

/*! Enum to define type of LT field in Exact Match Group Template */
typedef enum bcmfp_lt_em_group_field_type_e {
    /*! Rule LT field is Qualifier */
    BCMFP_LT_EM_GROUP_FIELD_TYPE_QUAL = 0,

    /*! Rule LT field is Action */
    BCMFP_LT_EM_GROUP_FIELD_TYPE_ACTION,
} bcmfp_lt_em_group_field_type_t;

/*! Structure for group LT field to Qualifier Mapping */
typedef struct bcmfp_lt_group_qual_map_s {
    /*! Field Identifier */
    uint32_t fid;

    /*! Corresponding qual for Field */
    bcmfp_qual_t qual;

    /*! Corresponding action for Field */
    bcmfp_action_t action;

    /*! is field Qualifier or Action*/
    bcmfp_lt_em_group_field_type_t field_type;
} bcmfp_lt_group_qual_map_t;


/*******************************************************************************
 Function Prototype
 */

/*!
 * \brief Function for Exact Match Group LT Initialization on a particular device.
 * \param [in] unit     Logical device id
 * \param [inout] lt_info Logical table metadata strucuture
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_em_group_init(int unit,
                    bcmfp_lt_info_t *lt_info);
/*!
 * \brief Function for Ingress Group LT Initialization on a particular device.
 * \param [in] unit     Logical device id
 * \param [inout] lt_info Logical table metadata strucuture
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_ing_group_init(int unit,
                     bcmfp_lt_info_t *lt_info);

/*!
 * \brief Function to get mapping for given LT field for Exact Match FP Group
 * \param [in] unit Logical device id
 * \param [in] fid  Field Identifier
 * \param [in] idx  Field index, valid for array field
 * \param [out] map Pointer to be updated with corresponding map
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmfp_em_group_field_map_get(int unit,
                             uint32_t fid,
                             uint32_t idx,
                             void *map);
/*!
 * \brief Function for Vlan Group LT Initialization on a particular device.
 * \param [in] unit     Logical device id
 * \param [inout] lt_info Logical table metadata strucuture
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_vlan_group_init(int unit,
                      bcmfp_lt_info_t *lt_info);


/*!
 * \brief Function for Egress Group LT Initialization on a particular device.
 * \param [in] unit     Logical device id
 * \param [inout] lt_info Logical table metadata strucuture
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_egr_group_init(int unit,
                     bcmfp_lt_info_t *lt_info);

/*!
 * \brief Function to get mapping for given LT field for Ingress FP Group
 * \param [in] unit Logical device id
 * \param [in] fid  Field Identifier
 * \param [in] idx  Field index, valid for array field
 * \param [out] map Pointer to be updated with corresponding map
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmfp_ing_group_qual_map_get(int unit,
                             uint32_t fid,
                             uint32_t idx,
                             void *map);

/*!
 * \brief Function to get mapping for given LT field for Exact Match FP Group
 * \param [in] unit Logical device id
 * \param [in] fid  Field Identifier
 * \param [in] idx  Field index, valid for array field
 * \param [out] map Pointer to be updated with corresponding map
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmfp_em_group_field_map_get(int unit,
                             uint32_t fid,
                             uint32_t idx,
                             void *map);
/*!
 * \brief Function to get mapping for given LT field for Vlan FP group
 * \param [in] unit Logical device id
 * \param [in] fid  Field Identifier
 * \param [in] idx  Field index, valid for array field
 * \param [out] map Pointer to be updated with corresponding map
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmfp_vlan_group_qual_map_get(int unit,
                              uint32_t fid,
                              uint32_t idx,
                              void *map);

/*!
 * \brief Function to get mapping for given LT field for Egress FP group
 * \param [in] unit Logical device id
 * \param [in] fid  Field Identifier
 * \param [in] idx  Field index, valid for array field
 * \param [out] map Pointer to be updated with corresponding map
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmfp_egr_group_qual_map_get(int unit,
                             uint32_t fid,
                             uint32_t idx,
                             void *map);

#endif /* BCMFP_LT_GROUP_MAP_H */
