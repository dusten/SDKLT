/*! \file bcmfp_lt_group.h
 *
 * APIs, defines for LT group
 *
 * This file contains APIs, defines for handling of FP group
 * Logical table
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

#ifndef BCMFP_LT_GROUP_H
#define BCMFP_LT_GROUP_H

/*******************************************************************************
  Includes
 */
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmfp/bcmfp_lt_policy.h>

/*******************************************************************************
  Typedefs
 */

/*!
 * \brief Group LT configuration structure.
 *
 * This structure is populated with User given Group information to be used
 * in Group Creation in HW.
 */
typedef struct bcmfp_lt_group_config_s {
    /*! Is group valid */
    bool valid;

    /*! Logical table information */
    uint32_t flag;

    /*! Group Identifier */
    uint32_t group_id;

    /*! bcmfp_stage_id_t stage */
    bcmfp_stage_id_t stage_id;

    /*! Policy Configuration */
    bcmfp_lt_policy_config_t policy_config;

    /*! Is policy Valid */
    bool policy_valid;

    /*! Pipe Id */
    int pipe_id;

    /*! group Priority */
    uint32_t priority;

    /*! Virtual Slice Group. */
    uint8_t virtual_slice_grp;

    /*! Mode of group */
    uint32_t mode;

    /*! Slice Mode of group */
    uint32_t slice_mode;

    /*! Port pkt type of group */
    uint32_t port_pkt_type;

    /*! Number of Presel in the group */
    uint32_t num_presel_ids;

    /*! Presel Id List */
    uint32_t presel_ids[BCMFP_GROUP_MAX_PRESEL];

    /*! Exact Match Look up partitition Id */
    uint8_t lookup_id;

    /*! Qualifier set */
    bcmfp_qset_t qset;

    /*! Action set */
    bcmfp_aset_t aset;

    /*! Qualifier set whose bitmap is partial */
    bcmfp_qset_t partial_qset;

    /*! List of partial bitmap qualifier */
    bcmfp_qual_bitmap_t *qual_bitmap;

    /*! Num entries */
    uint32_t num_entries;

    /*! Group LTID*/
    uint32_t group_ltid;
} bcmfp_lt_group_config_t;

/*******************************************************************************
 Function Prototype
 */

/*!
 * \brief Function validate input fields/data of Group LT on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] sid This is the logical table ID.
 * \param [in] action BCMIMM action
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *

 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_validate(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_entry_event_t action,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data);

/*!
 * \brief Function resets the configuration required for IFP group validation on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */

extern int
bcmfp_lt_ingress_grp_config_delete(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t sid,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);

/*!
 * \brief Function Update the IFP grooup config for a particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_ingress_grp_config_update(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t sid,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data);

/*!
 * \brief Function insert the IFP grooup config for a particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_ingress_grp_config_insert(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t sid,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);
/*!
 * \brief Function insert operation of an Entry in Group LT on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] in       User input of LT fields
 * \param [in] arg      Arguements
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_insert(int unit,
                      uint32_t trans_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg);
/*!
 * \brief Function perform lookup operation for an entry in Group LT on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] in       User input of LT fields
 * \param [out] out     Buffer to populate for LT fields and data values
 * \param [in] arg      Arguements
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_lookup(int unit,
                      uint32_t trans_id,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief Function delete an Entry from Group LT on a particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] in       User input of LT fields
 * \param [in] arg      Arguements
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_delete(int unit,
                      uint32_t trans_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief Function Update Entry in a Group LT for a particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] in       User input of LT fields
 * \param [in] arg      Arguements
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_update(int unit,
                      uint32_t trans_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg);


/*!
 * \brief Function to free allocated memory from group configuration
 * \param [in] unit Logical device id
 * \param [in] group_config Configuration which is to be freed
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_config_free(int unit,
                           bcmfp_lt_group_config_t *group_config);

/*!
 * \brief Function to checks if Group id is present or not in in-mem DB
 * \param [in] unit Logical device id
 * \param [in] stage_id FP stage
 * \param [in] gid Group identifier
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_exists(int unit,
                      bcmfp_stage_id_t stage_id,
                      uint32_t gid);

/*!
 * \brief Function increment num entries for group entry in in-mem DB
 * \param [in] unit Logical device id
 * \param [in] stage_id FP stage
 * \param [in] gid Group identifier
 * \param [in] group_mode Group Mode for Group Id
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_ref_increment(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t gid,
                             bcmfp_group_mode_t group_mode);

/*!
 * \brief Function decrement num entries for group entry in in-mem DB
 * \param [in] unit Logical device id
 * \param [in] stage_id FP stage
 * \param [in] gid Group identifier
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_group_ref_decrement(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t gid);

/*!
 * \brief Function Restrict two groups in same pipe having same group_prio(same
 * \param [in] unit Logical device id
 * \param [in] action BCMIMM action
 * \param [in] lt_info  Pointer to LT information
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_fixed_key_group_validate(int unit,
                               bcmimm_entry_event_t action,
                               bcmfp_lt_info_t *lt_info,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data);

/*!
 * \brief Function Restrict two groups in same pipe having same priority
 * in stage with flag BCMFP_LT_FLAGS_UNIQUE_GRP_PRI set
 * \param [in] unit Logical device id
 * \param [in] action BCMIMM action
 * \param [in] lt_info  Pointer to LT information
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_priority_validate (int unit,
                               bcmimm_entry_event_t action,
                               bcmfp_lt_info_t *lt_info,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data);

/*!
 * \brief Function to extract group configuration from LT template entry
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] stage_id Stage Id to which group config belongs to
 * \param [in] group_id Key for Group LT
 * \param [out] group_config Group config of type bcmfp_lt_group_config_t
 *
 * \retval SHR_E_NONE Success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_group_config_get(int unit,
                          uint32_t trans_id,
                          bcmfp_stage_id_t stage_id,
                          uint32_t group_id,
                          bcmfp_lt_group_config_t *group_config);

/*!
 * \brief Function update the configuration required for EFP group validation on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_egress_grp_config_update(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t sid,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);

/*!
 * \brief Function resets the configuration required for EFP group validation on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_egress_grp_config_delete(int unit,
                                  uint32_t trans_id,
                                  bcmltd_sid_t sid,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);

/*!
 * \brief Function update the configuration required for VFP group validation on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_vlan_grp_config_update(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t sid,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data);

/*!
 * \brief Function resets the configuration required for VFP group validation on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] sid This is the logical table ID.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_vlan_grp_config_delete(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t sid,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data);

#endif /* BCMFP_LT_GROUP_H */
