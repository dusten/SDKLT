/*! \file bcmfp_lt_entry.h
 *
 * APIs, defines for LT entry
 *
 * This file contains APIs, defines for handling of FP entry
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

#ifndef BCMFP_LT_ENTRY_H
#define BCMFP_LT_ENTRY_H

/*******************************************************************************
 Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_lt_internal.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_group.h>
#include <bcmfp/bcmfp_lt_rule.h>
#include <bcmfp/bcmfp_lt_policy.h>

/*******************************************************************************
 Tyepdefs
 */

/*!
 * \brief Entry LT configuration structure.
 *
 * Structure to be filled with Entry LT configuration when Entry INSERT
 * is called. Information populated in this structure is used to call
 * internal FP APIs for actual hw configuration.
 */
typedef struct bcmfp_lt_entry_config_s {
    /*! Entry Logical Table Identifier */
    bcmdrd_sid_t ltid;

    /*! Stage Id */
    bcmfp_stage_id_t stage_id;

    /*! Entry Identifier */
    uint32_t eid;

    /*! Entry Priority */
    uint32_t priority;

    /*! Group Configuration */
    bcmfp_lt_group_config_t group_config;

    /*! Rule Configuration */
    bcmfp_lt_rule_config_t rule_config;

    /*! Policy Configuration */
    bcmfp_lt_policy_config_t policy_config;

    /*! Is Meter Is Valid */
    bool meter_valid;

    /*! Meter Id */
    uint64_t meter_id;

    /*! Is Counter Id Valid */
    bool ctr_valid;

    /*! Counter Id */
    uint64_t ctr_id;

    /* Operation state of entry */
    uint32_t oper_state;
} bcmfp_lt_entry_config_t;

/*******************************************************************************
 Function Prototype
 */


/*!
 * \brief Function Update one LT field Entry for Entry LT on a
 *        particular  stage on a particular device.
 * \param [in] unit Logical device id
 * \param [in] ltid Entry LT Identifier
 * \param [in] in_field user input of LT field
 * \param [in] out_fields Memory allocated in imm_fields
 * \param [out] imm_fields Buffer to update fields
 * \param [out] flags saves change made for field
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_field_update(int unit,
                            bcmltd_sid_t ltid,
                            const bcmltd_field_t * in_field,
                            uint32_t out_fields,
                            bcmltd_fields_t *imm_fields,
                            uint8_t *flags);

/*!
 * \brief Function to extract entry configuration from LT template entry
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] stage_id     FP stage Id
 * \param [in] lt_info pointer to LT field identifier
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 * \param [in] template_fid Fid of the template's Id being inserted/updated
 * \param [in] template_data This is a linked list of the data fields of
 * the template being inserted/updated.
 * \param [out] entry_config Entry config of type bcmfp_lt_entry_config_t
 *
 * \retval SHR_E_NONE Success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_entry_config_get(int unit,
                          uint32_t trans_id,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_lt_info_t *lt_info,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          uint32_t template_fid,
                          const bcmltd_field_t *template_data,
                          bcmfp_lt_entry_config_t *entry_config);

/*
 * \brief Function to Lock the templates Id used in Entry
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id     FP stage Id
 * \param [out] entry_config Entry config of type bcmfp_lt_entry_config_t
 *
 * \retval SHR_E_NONE Success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_lock_templates_id(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_lt_entry_config_t *entry_config);

/*!
 * \brief Function insert operation for Entry Id of Entry LT on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] ltid Entry LT Identifier
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_insert(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t ltid,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data);
/*!
 * \brief Function update operation for Entry Id of Entry LT on a
 *        particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] ltid Entry LT Identifier
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_update(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t ltid,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data);
/*!
 * \brief Function validate input fields/data of Entry LT on a
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
bcmfp_lt_entry_validate(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_entry_event_t action,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data);

/*!
 * \brief Function delete Entry Id in Entry LT on a particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] ltid Entry LT Identifier
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_delete(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t ltid,
                      const bcmltd_field_t *key);
/*!
 * \brief Function to free allocated memory from entry configuration
 * \param [in] unit Logical device id
 * \param [in] entry_config Configuration which is to be freed
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_config_free(int unit,
                           bcmfp_lt_entry_config_t *entry_config);


/*!
 * \brief Function to configure h/w Entry
 * \param [in] unit Logical device id
 * \param [in] trans_id     Transaction ID
 * \param [in] entry_config Entry Configuration
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_hw_insert(int unit, uint32_t trans_id,
                         bcmfp_lt_entry_config_t *entry_config);


/*!
 * \brief Function to build the key words from the group/rule config
 * \param [in] unit Logical device id
 * \param [in] trans_id     Transaction ID
 * \param [in] req_ltid     Entry LT Identifier
 * \param [in] group_id     Group Identifier
 * \param [in] entry_id     Entry Identifier
 * \param [in] group_config Group configuration
 * \param [in] rule_config  Rule Configuration
 * \param [out] ekw         Pointer to Key words array
 *
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_em_entry_build_ekw(int unit,
                         uint32_t trans_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_group_id_t group_id,
                         bcmfp_entry_id_t entry_id,
                         bcmfp_lt_group_config_t *group_config,
                         bcmfp_lt_rule_config_t *rule_config,
                         uint32_t **ekw);
/*!
 * \brief Function to delete h/w Entry configuration
 * \param [in] unit         Logical device id
 * \param [in] trans_id     Transaction ID
 * \param [in] stage_id     FP stage Id
 * \param [in] entry_ltid   Logical table Identifier
 * \param [in] eid          Entry key
 * \param [in] gid          Group key
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_hw_delete(int unit, uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         bcmdrd_sid_t entry_ltid,
                         uint32_t eid,
                         uint32_t gid);

/*!
 * \brief Function to get the rule ID associated to the entry ID.
 * \param [in] unit         Logical device id
 * \param [in] trans_id     Transaction ID
 * \param [in] stage_id     FP stage Id
 * \param [in] entry_ltid   Logical table Identifier
 * \param [in] entry_id     Entry ID
 * \param [out] rule_id     Rule ID
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_rule_id_get(int unit,
                     uint32_t trans_id,
                     bcmfp_stage_id_t stage_id,
                     bcmltd_sid_t entry_ltid,
                     bcmfp_entry_id_t entry_id,
                     uint32_t *rule_id);
/*!
 * \brief Function to update the operational status of the entry.
 * \param [in] unit         Logical device id
 * \param [in] trans_id     Transaction ID
 * \param [in] stage_id     FP stage Id
 * \param [in] eid          Entry ID
 * \param [in] oper_state  Operational status of the entry.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_oprtnl_state_update(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_id_t stage_id,
                                   uint32_t eid,
                                   uint32_t oper_state);

/*!
 * \brief Function to delete the operation status of an entry.
 * \param [in] unit         Logical device id
 * \param [in] trans_id     Transaction ID
 * \param [in] stage_id     FP stage Id
 * \param [in] eid          Entry ID
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_oprtnl_state_delete(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_id_t stage_id,
                                   uint32_t eid);
/*!
 * \brief Function to get the operational status of a staged entry.
 * \param [in] unit         Logical device id
 * \param [in] stage_id     FP stage Id
 * \param [in] eid          Entry ID
 * \param [out] oper_state  Operational status of the entry.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_oprtnl_state_get(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t eid,
                                uint32_t *oper_state);
/*!
 * \brief Function to Clear the operational status of all the enries in the
 * stage.
 * \param [in] unit         Logical device id
 * \param [in] stage_id     FP stage Id
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_oprtnl_state_clear(int unit,
                                  bcmfp_stage_id_t stage_id);
/*!
 * \brief Function to Function to update the operation status of the staged
 * entries into the IMM
 * stage.
 * \param [in] unit         Logical device id
 * \param [in] stage_id     FP stage Id
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_entry_oprtnl_state_insert(int unit,
                                   bcmfp_stage_id_t stage_id);
/*!
 * \brief Function update the associated entries in hardware when a template is
 *  inserted/updated on a particular device.
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_type LT type
 * \param [in] ltid Entry LT Identifier
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields
 * \param [in] event_reason Call back reason.
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_update_entries(int unit,
                        uint32_t trans_id,
                        int lt_type,
                        bcmltd_sid_t ltid,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmimm_entry_event_t event_reason);

#endif /* BCMFP_LT_ENTRY_H */
