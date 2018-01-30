/*! \file rm_tcam.h
 *
 * Utils, defines internal to RM-TCAM
 * This file contains utils, defines which are internal to
 * TCAM resource manager(i.e. these are not visible outside RM-TCAM)
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

#ifndef RM_TCAM_H
#define RM_TCAM_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_um_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Global variables
 */
/* Global data structure to hold pointers to TCAM resource manager
 *  on all Logical device ids
 */
void *rm_tcam_ptr[BCMDRD_CONFIG_MAX_UNITS];

/* Global data structure to hold back up pointers to TCAM resource manager
 *  on all Logical device ids
 */
void *rm_tcam_backup_ptr[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Defines
 */
/*! Divison factor to caliculate the depth of hash table for priority
 *  based TCAMs.
 */
#define BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR (4)

/*! Invalid TCAM index. */
#define BCMPTM_RM_TCAM_INDEX_INVALID (-1)

/*! Invalid EID(Entry ID). */
#define BCMPTM_RM_TCAM_EID_INVALID (-1)

/*! Invalid entry priority. */
#define BCMPTM_RM_TCAM_PRIO_INVALID (-1)

/*! Invalid memory offset. */
#define BCMPTM_RM_TCAM_OFFSET_INVALID (-1)

/*! Entry operation in HW only. */
#define BCMPTM_RM_TCAM_ENTRY_OP_HW_ONLY (1 << 0)
/*! Entry operation in SW only. */
#define BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY (1 << 1)

/*! Only key part of the entry is modified. */
#define BCMPTM_RM_TCAM_ENTRY_KEY_ONLY_UPDATE (1 << 0)
/*! Only data part of the entry is modified. */
#define BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE (1 << 1)

/*! Global data structure to hold pointers to TCAM resource manager.*/
#define RM_TCAM_PTR(unit) rm_tcam_ptr[unit]

/*! Global data structure to hold back up pointers to TCAM resource manager.*/
#define RM_TCAM_BACKUP_PTR(unit) rm_tcam_backup_ptr[unit]


/*******************************************************************************
 * Typedefs
 */
/*! TCAM Function pointers. */
typedef struct bcmptm_rm_tcam_funct_s {

     /*! TCAM entry insert function pointer */
     int (*entry_insert)(int unit,
                         uint32_t flags,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         bcmbd_pt_dyn_info_t *pt_dyn_info,
                         bcmptm_rm_tcam_req_t *req_info,
                         bcmptm_rm_tcam_rsp_t *rsp_info,
                         bcmltd_sid_t *rsp_ltid, uint32_t *rsp_flags);

     /*! TCAM entry lookup function pointer */
     int (*entry_lookup) (int unit,
                          uint32_t flags,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          bcmptm_rm_tcam_req_t *req_info,
                          bcmptm_rm_tcam_rsp_t *rsp_info,
                          bcmltd_sid_t *rsp_ltid,
                          uint32_t *rsp_flags,
                          uint32_t *index);

     /*! TCAM entry delete function pointer */
     int (*entry_delete) (int unit,
                          uint32_t flags,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          bcmptm_rm_tcam_req_t *req_info,
                          bcmptm_rm_tcam_rsp_t *rsp_info,
                          bcmltd_sid_t *rsp_ltid, uint32_t *rsp_flags);

     /*! TCAM entry move function pointer */
     int (*entry_move)(int unit,
                       bcmltd_sid_t ltid,
                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                       uint8_t partition,
                       uint32_t prefix,
                       uint32_t from_index,
                       uint32_t to_index,
                       void *attrs,
                       bcmbd_pt_dyn_info_t *pt_dyn_info);

     /*! TCAM entry delete function pointer */
     int (*entry_traverse_get_first) (int unit,
                                      uint32_t flags,
                                      bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      bcmbd_pt_dyn_info_t *pt_dyn_info,
                                      bcmptm_rm_tcam_req_t *req_info,
                                      bcmptm_rm_tcam_rsp_t *rsp_info,
                                      bcmltd_sid_t *rsp_ltid,
                                      uint32_t *rsp_flags);

     /*! TCAM entry delete function pointer */
     int (*entry_traverse_get_next) (int unit,
                                     uint32_t flags,
                                     bcmltd_sid_t ltid,
                                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                                     bcmptm_rm_tcam_req_t *req_info,
                                     bcmptm_rm_tcam_rsp_t *rsp_info,
                                     bcmltd_sid_t *rsp_ltid,
                                     uint32_t *rsp_flags);

} bcmptm_rm_tcam_funct_t;

/*! Structure to hold same key related info */
typedef struct bcmptm_rm_tcam_same_key_data_s {

    /* Same key index */
    int index;

    /* Associated Priority */
    int priority;
} bcmptm_rm_tcam_same_key_data_t;

/* To hold previously looked up index */
extern bcmptm_rm_tcam_same_key_data_t same_key_info[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Fetch the pointer that holds function pointers to basic TCAM
 *  \n operations of a TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] ltid_info LRD information
 * \param [out] funct Pointer to the required function
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_NOT_FOUND Function Not found
 */
extern int
bcmptm_rm_tcam_funct_get(int unit, bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info, void **funct);


/*!
 * \brief To compare two entries of Tcam, src and dst info.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] flags Flags for special handling
 * \param [in] src_ent_key_words Key Part of Src Data
 * \param [in] src_ent_data_words Data Part of Src Data
 * \param [in] dst_ent_key_words Key Part of Dst Data
 * \param [in] dst_ent_data_words Data Part of Dst Data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FAIL Compare failed
 */

extern int
bcmptm_rm_tcam_entry_compare(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             uint32_t flags,
                             uint32_t **src_ent_key_words,
                             uint32_t **src_ent_data_words,
                             uint32_t **dst_ent_key_words,
                             uint32_t **dst_ent_data_words);
/*!
 * \brief Fetch the Memory address offset information of a given LTID.
 *\n offset is offset value form the start address of the single memory
 *\n chunk created for the whole RM TCAM module.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] tcam_info memory address to tcam information.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_info_get(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info, void **tcam_info);

/*!
 * \brief Fetch the Memory address offset information of a given LTID.
 *\n offset is offset value form the start address of the single memory
 *\n chunk created for the backup of RM TCAM module.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] tcam_info memory address to tcam information.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_backup_info_get(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info, void **tcam_info);

/*!
 * \brief Move the range of TCAM entries one index up or downward
 * \n direction. This will internally call bcmptm_rm_tcam_entry_move
 * \n function for each individual entry move.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] idx_start Start TCAM index to be move in the range.
 * \param [in] idx_end Last TCAM index to be moved in the range.
 * \param [in] attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_entry_move_range(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint8_t partition,
                                uint32_t idx_start,
                                uint32_t idx_end,
                                void *attrs,
                                bcmbd_pt_dyn_info_t *pt_dyn_info);

/*!
 * \brief Move the TCAM entry from one index to the other. This function
 * \n will internall call TCAM specific move function.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] from_idx TCAM index from which entry has to be moved
 * \param [in] to_idx TCAM index to which entry has to be moved
 * \param [in] attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_entry_move(int unit,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          uint8_t partition,
                          uint32_t from_idx,
                          uint32_t to_idx,
                          void *attrs,
                          bcmbd_pt_dyn_info_t *pt_dyn_info);


/*!
 * \brief Read entry words from all width expanded physical tables
 * \n mapped to ltid.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] rm_flags Flags of type BCMPTM_RM_TCAM_XXX to select the sid view
 *             (TCAM Aggregated View/TCAM only view/DATA only view)
 * \param [in] req_flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] index TCAM index to be written
 * \param [in] entry_words Array holding entry words of all width
               expanded sid's
 * \param [in] entry_words_size Number of words in entry_words array
 * \param [in] dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] ovrr_info Pointer to bcmbd_pt_ovrr_info_t
 * \param [in] attrs Attributes of entry.
 * \param [out] rsp_ltid ltid to which entry_words are actually written
 * \param [out] rsp_flags Flags used in processing TCAM table request
 * \param [out] rsp_sid SID for the LTID passed
 * \param [out] hw_index HW index of entry
 * \param [out] hw_index_count NUmber of HW index in hw_index array
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_hw_read(int unit,
                       bcmltd_sid_t ltid,
                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                       uint32_t rm_flags,
                       uint32_t req_flags,
                       uint32_t index,
                       uint32_t **entry_words,
                       size_t entry_words_size,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       void *ovrr_info,
                       void *attrs,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags,
                       bcmdrd_sid_t *rsp_entry_sid,
                       int *hw_index,
                       int *hw_index_count);

/*!
 * \brief Write the entry words to actual phyiscal tables mapped to givel ltid.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] rm_flags Flags of type BCMPTM_RM_TCAM_XXX to select the sid view
 *             (TCAM Aggregated View/TCAM only view/DATA only view)
 * \param [in] req_flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] index TCAM index to be written
 * \param [in] entry_words Array holding entry words of all width
               expanded sid's
 * \param [in] dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] ovrr_info Pointer to bcmbd_pt_ovrr_info_t
 * \param [in] attrs Attributes of entry.
 * \param [out] rsp_ltid ltid to which entry_words are actually written
 * \param [out] rsp_flags Flags used in processing TCAM table request
 * \param [out] rsp_sid SID for the LTID passed
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_hw_write(int unit,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        uint32_t rm_flags,
                        uint32_t req_flags,
                        uint32_t index,
                        uint32_t **entry_words,
                        bcmbd_pt_dyn_info_t *pt_dyn_info,
                        void *ovrr_info,
                        void *attrs,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags,
                        bcmdrd_sid_t *rsp_entry_sid);

/*!
 * \brief Calculate the total memory required for SW state of
   \n all TCAM LTIDs together.
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] user_data User Data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_INTERNAL initialization issues
 */
extern int
bcmptm_rm_tcam_prio_calc_mem_required(int unit, bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  void *user_data);

/*!
 * \brief Initialize the metadata of priority based TCAM in
 *        bcmptm_rm_tcam_prio_eid_info_t format.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] start_ptr Start pointer to TCAM
 *                       resource manager shared memory
 * \param [in] offset Offset bytes from start_ptr to start of ltid data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_init_metadata(int unit, bool warm,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  void *start_ptr,
                                  uint32_t *offset);

/*!
 *  \brief TCAM LTID can be depth expanded and every TCAM index maps
 *  \n to a physical slice and index in that slice. This routine is to
 *  \n fetch the slice roe and index in that slice row for a given TCAM index.
 *
 *  \param [in] unit Logical device id
 *  \param [in] ltid Logical Table enum that is accessing the table.
 *  \param [in] ltid_info LRD information for the ltid.
 *  \param [in] tindex TCAM index
 *  \param [out] sindex Slice index corresponding to tindex
 *  \param [out] slice_row Slice row corresponds to tindex
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Inavlid parameters
 */
extern int
bcmptm_rm_tcam_tindex_to_sindex(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t tindex,
                                uint32_t *sindex, uint8_t *slice_row);

/*!
 *  \brief Internal function to extract bits
 *  \n from given start positionm to given length.
 *
 *  \param [in] unit Logical device id
 *  \param [in] in_data Input Data of words
 *  \param [in] start_pos Starting Position
 *  \param [in] len Length to be extracted.
 *  \param [out] out_data Output Data of extracted bits.
 *
 *  \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_data_get(int unit,
                        uint32_t *in_data,
                        uint16_t start_pos,
                        uint16_t len,
                        uint32_t *out_data);

/*!
 * \brief Fetch the hw entry information of an LT based on LT entries mode.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Attributes of entry.
 * \param [out] hw_entry_info LT entry chip specific information.
 *
 *  \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_lt_hw_entry_info_get(int unit,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  void *attrs,
                  const bcmptm_rm_tcam_hw_entry_info_t **hw_entry_info);

/*!
 * \brief To handle the transaction at RM TCAM module.
 *
 * \param [in] unit Logical device id
 * \param [in] transaction global info.
 *
 *  \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_um_trans_complete(int unit,
                          bcmptm_rm_tcam_trans_info_t *trans_info_global);
/*!
 * \brief  Fetch the number of physical indexs needed to write the entry
 * \n for the given Logical Table id.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] num physical indexes needed
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_num_index_required_get(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              uint32_t *num_index_req);

#endif /* RM_TCAM_H */
