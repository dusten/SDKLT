/*! \file bcmptm_ser_misc_internal.h
 *
 * Interface functions for TCAM entry mode or slice mode change
 *
 * SER TCAM S/W scan the entry with right mode (narrow or wide).
 *
 * Interface functions only can be used by SER sub-component.
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

#ifndef BCMPTM_SER_MISC_INTERNAL_H
#define BCMPTM_SER_MISC_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <bcmptm/bcmptm_internal.h>

/******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Defines
 */
/*!
 * \brief Allows RM to provide info about Tcam slices that it is managing.
 * \n Can be used by both FP, L3_DEFIP RM.
 *
 * \param [in] unit Logical device id
 * \param [in] slice_depth Array with 'num_slice' elements. Nth element
 * specifies depth of Nth slice. Slice depth could be different for different
 * slices in IFP.
 * \param [in] num_slice Num of elements in slice_depth array.
 * \param [in] sid_list Array with 'num_sid' elements. Identifies narrow, wide
 * view that will be used to access the slices
 * \param [in] num_sid Num of elements in sid_list array.
 * \param [in] slice_mode Initial slice_mode consistent with 'initial'
 * slice_mode as maintained in RM. Same value for all slices.
 * \param [in] is_overlay_tcam In the same slice, some entries can be used as 'wide' mode,
 * others can be used as 'narrow' mode, e.g: L3_DEFIPm/L3_DEFIP_PAIR_128m.
 *
 * \param [out] slice_group_id ID by which RM can identify slice mode resources
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_slice_group_register(int unit,
                                int *slice_depth, int num_slice,
                                bcmdrd_sid_t *sid_list, int num_sid,
                                bcmptm_rm_slice_mode_t slice_mode,
                                int is_overlay_tcam,

                                uint32_t *slice_group_id);

/*!
 * \brief Provides mutex corresponding to slice_group_id
 * \n Will be called by WAL reader before sending slice_mode change related ops
 * to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] slice_group_id ID of slice_group managed by RM
 *
 * \param [out] slice_mode_mutex mutex corresponding to slice_group_id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_slice_mode_mutex_info(int unit, uint32_t slice_group_id,
                                 sal_mutex_t *slice_mode_mutex);

/*!
 * \brief Provides mutex corresponding to tables for which we track mode on per
 * entry-basis (L3_DEFIP).
 * \n SERC logic maintains common mutex for L3_DEFIP tables related slice_mode,
 * entry_mode changes.
 *
 * \param [in] unit Logical device id.
 *
 * \param [out] entry_mode_group_id ID of corresponding L3_DEFIP tables slice
 * group
 * \param [out] entry_mode_mutex mutex corresponding to L3_DEFIP tables.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_entry_mode_mutex_info(int unit,
                                 uint32_t *entry_mode_group_id,
                                 sal_mutex_t *entry_mode_mutex);

/*!
 * \brief Provides slice_mode_change info corresponding to slice_group_id
 *
 * \param [in] unit Logical device id
 * \param [in] slice_mode_change_info Info needed for SERC to update slice_mode
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_slice_mode_change(int unit,
                             bcmptm_rm_slice_change_t *slice_mode_change_info);

/*!
 * \brief Provides entry_mode_change info
 *
 * \param [in] unit Logical device id
 * \param [in] entry_mode_change_info Info needed for SERC to update entry_mode
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_entry_mode_change(int unit,
                             bcmptm_rm_entry_mode_change_t *entry_mode_change_info);

/*!
 * \brief This API is used only by TCAM memscan, if index is invalid of wide or narrow view PT,
 * index will be adjust as valid index. This API is used for IFPm.
 *
 * \param [in] unit                        Logical device id
 * \param [in] sid                         SID
 * \param [in & out] index             the beginning of scan index
 * \param [out] stride                   Number of scan index
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_slice_index_adjust(int unit, bcmdrd_sid_t sid,
                              int *index, int *stride);

/*!
 * \brief This API is used only by TCAM memscan, if index is located in narrow slices,
 * the 'stride' will be 'slice_depth-index % slice_depth'.
 * If the index is located in wide slices, and the corresponding entry mode of index
 * is matched with 'sid', the 'stride' will be 1, otherwise, the 'stride' will be 0.
 * This API is used for L3_DEFIPm/L3_DEF_IP_PAIR_128m only.
 *
 * \param [in] unit                        Logical device id
 * \param [in] sid                         SID
 * \param [in] index                      the beginning of scan index
 * \param [out] stride                   Number of scan index
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_overlay_index_stride_get(int unit, bcmdrd_sid_t sid,
                                    int index, int *stride);

/*!
 * \brief Print all slice mode information.
 *
 * \param [in] unit                        Logical device id
 * \param [in] overlay_info_print     Print entry bitmap of overlay TCAM
 *
 * \retval
 */
extern void
bcmptm_ser_slice_mode_info_print(int unit, int overlay_info_print);

/*!
 * \brief Check index valid for TCAMs which support narrow mode and wide mode.
 *
 * \param [in] unit          Logical device id.
 * \param [in] sid           PT ID.
 * \param [in] index        index needs to be checked.
 * \param [out] valid       index is valid or is in using in SID view.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_paired_tcam_index_valid(int unit, bcmdrd_sid_t sid,
                                   int index, int *valid);

/*!
 * \brief Allocate memory space used to save slice mode information.
 *
 * \param [in] unit          Logical device id.
 * \param [in] warm        Warm boot.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_slice_mode_info_init(int unit, bool warm);


/*!
 * \brief Free memory space used to save slice mode information.
 *
 * \param [in] unit          Logical device id.
 *
 * \retval
 */
extern void
bcmptm_ser_slice_mode_info_deinit(int unit, bool warm);

/*!
 * \brief Get slice_mode_mutex according to physical table ID.
 *
 * \param [in] unit          Logical device id.
 * \param [in] sid           PT ID.
 * \param [out] slice_mode_mutex           Slice mode mutex.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_slice_mode_mutex_get(int unit, bcmdrd_sid_t sid, sal_mutex_t *slice_mode_mutex);

#endif /* BCMPTM_SER_MISC_INTERNAL_H */
