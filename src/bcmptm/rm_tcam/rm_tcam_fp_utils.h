/*! \file rm_tcam_fp_utils.h
 *
 * Utils, defines internal to FP TCAMs resource manager.
 * This file contains utils, defines which are internal to
 * FP TCAMs resource manager(i.e. these are not visible outside RM-TCAM)
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

#ifndef RM_TCAM_FP_UTILS_H
#define RM_TCAM_FP_UTILS_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include "rm_tcam_prio_eid.h"


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Function declarations (prototypes)
 */

/*!
 * \brief Fetch the address to TCAM entry information of the given LTID.
 * \n This address points to address of zeroth TCAM index entry information.
 * \n To get the address to TCAM entry information of any specific TCAM index,
 * \n increment this pointer by those many TCAM indexes as memory created to
 * \n hold TCAM entry information of all TCAM indexes is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] entry_info double pointer to hold pointer to
 * \n           start address of TCAM entry information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_info_get(int unit, bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      void *attrs,
                                      bcmptm_rm_tcam_prio_entry_info_t **entry_info);

/*!
 * \brief Fetch the address to TCAM entry hash of the given Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid..
 * \n This address points to the address of hash zero. To get the address to
 * \n any other valid hash, increment this start pointer by hash times as memory
 * \n created to hold the hash information of all valid hash values is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] entry_hash Double pointer to hold pointer to start address
 * \n           of TCAM hash information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_hash_get(int unit,
                                      bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      void *attrs,
                                      int **entry_hash);

/*!
 * \brief Fetch the TCAM flags for a given entry
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] flags TCAM flags
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_flags_get(int unit, bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       void *attrs,
                                       uint32_t *flags);
/*!
 * \brief Fetch the number of free entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] free_entries Number of free_entries.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_free_count_get(int unit, bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            void *attrs,
                                            uint32_t *free_entries);

/*!
 * \brief Fetch the number of valid entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] num_entries Number of valid entries.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_valid_count_get(int unit, bcmltd_sid_t ltid,
                                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                                             void *attrs,
                                             uint32_t *num_entries);


/*!
 * \brief Fetch the hash size of entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] entry_hash_size Hash size of the entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_hash_size_get(int unit, bcmltd_sid_t ltid,
                                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                                           void *attrs,
                                           uint32_t *entry_hash_size);
/*!
 * \brief Increment the entry count in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry logical index
 * \param [in] attrs Entry Attribute information.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_free_count_incr(int unit, bcmltd_sid_t ltid,
                                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                                             uint32_t ent_logical_idx,
                                             void *attrs);

/*!
 * \brief Decrement the entry count in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry logical index
 * \param [in] attrs Entry Attribute information.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_free_count_decr(int unit, bcmltd_sid_t ltid,
                                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                                             uint32_t ent_logical_idx,
                                             void *attrs);
/*!
 * \brief Fetch the number of pipes in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] pipe_count Number of pipes.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_pipe_count_get(int unit, bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            void *attrs,
                                            uint8_t *pipe_count);

/*!
 * \brief Fetch the total number of entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] total_count Number of entries in the TCAM.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */

extern int
bcmptm_rm_tcam_prio_fp_entry_total_count_get(int unit, bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         void *entry_attrs,
                                         uint32_t *total_count);

#endif /* RM_TCAM_FP_UTILS_H */
