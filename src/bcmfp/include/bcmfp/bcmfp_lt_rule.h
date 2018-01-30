/*! \file bcmfp_lt_rule.h
 *
 * APIs, defines for FP rule LT
 *
 * This file contains APIs, defines for handling of FP rule
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

#ifndef BCMFP_LT_RULE_H
#define BCMFP_LT_RULE_H

/*******************************************************************************
  Includes
 */
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_lt_group.h>
#include <bcmimm/bcmimm_int_comp.h>

/*******************************************************************************
  Typedefs
 */

/*!
 * \brief Rule Qualifer structure.
 *
 * Linked-list of Rule Qualifier.
 */

typedef struct bcmfp_lt_rule_data_s {
    /*! Qualifier */
    bcmfp_qual_t qual;

    /*! Associated Data */
    uint32_t data[BCMFP_MAX_WSIZE];

    /*! Associated Mask */
    uint32_t mask[BCMFP_MAX_WSIZE];

    /* Required bits in data and mask */
    uint32_t bitmap[BCMFP_MAX_WSIZE];

    /*! Pointer to next rule */
    struct bcmfp_lt_rule_data_s * next;
} bcmfp_lt_rule_data_t;

/*!
 * \brief Rule LT configuration structure.
 *
 * Structure to be filled with Rule LT configuration with user given
 * qualifier, data and mask.
 */
typedef struct bcmfp_lt_rule_config_s {
    /*! Is this structure valid */
    bool valid;

    /*! LT identifider */
    bcmltd_sid_t ltid;

    /*! Key */
    uint32_t rule_id;

    /*! Stage Id */
    bcmfp_stage_id_t stage_id;

    /*! Is Qualifier Inports Present. Valid in INGRESS stage */
    bool is_qual_inports;

    /*! Inports Qualifier pbmp data. Valid in INGRESS Stage */
    bcmdrd_pbmp_t inports_pbmp_data;

    /*! Inports Qualifier pbmp Mask. Valid In INGRESS Stage */
    bcmdrd_pbmp_t inports_pbmp_mask;

    /*! Pointer to data */
    bcmfp_lt_rule_data_t *data;
} bcmfp_lt_rule_config_t;

/*******************************************************************************
 Function Prototype
 */

/*!
 * \brief Function to free allocated memory from rule config
 * \param [in] unit Logical device id
 * \param [in] rule_config Configuration which is to be freed
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_rule_config_free(int unit,
                          bcmfp_lt_rule_config_t *rule_config);


/*!
 * \brief Function to extract rule configuration from LT template entry
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] stage_id Stage Id to which rule config belongs to
 * \param [in] rule_id Key for Rule LT
 * \param [in] eid Entry Id refering the rule template
 * \param [in] template_data This is a linked list of the data fields of
 * the template being inserted/updated.
 * \param [out] rule_config Rule config of type bcmfp_lt_rule_config_t
 *
 * \retval SHR_E_NONE Success, otherwise failure in obtaining information
 */
extern int
bcmfp_lt_rule_config_get(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         uint32_t rule_id,
                         uint32_t eid,
                         const bcmltd_field_t *template_data,
                         bcmfp_lt_group_config_t *group_config,
                         bcmfp_lt_rule_config_t *rule_config);

/*!
 * \brief Function insert entry to the entry list of given rule Id
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] stage_id FP stage Id
 * \param [in] rule_id  Rule ID
 * \param [in] eid      Entry key
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_rule_template_info_insert(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage,
                                   uint32_t rule_id,
                                   bcmfp_entry_id_t eid);
/*!
 * \brief Function deletes entry from the entry list of given rule Id
 * \param [in] unit     Logical device id
 * \param [in] trans_id LT Transaction Id
 * \param [in] stage_id FP stage Id
 * \param [in] rule_id  Rule ID
 * \param [in] eid      Entry key
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_lt_rule_template_info_delete(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage,
                                   uint32_t rule_id,
                                   bcmfp_entry_id_t eid);

#endif /* BCMFP_LT_RULE_H */
