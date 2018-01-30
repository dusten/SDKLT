/*! \file rm_tcam_fp.h
 *
 * Low Level Functions for FP based TCAMs
 * This file contains low level functions for FP based TCAMs
 * to do operations like TCAM entry Insert/Lookup/Delete/Move.
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

#ifndef RM_TCAM_FP_H
#define RM_TCAM_FP_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_rm_tcam_fp.h>

/*******************************************************************************
 * Defines
 */
/* FP SLICE FLAGS */
#define BCMPTM_SLICE_INTRASLICE_CAPABLE       (1 << 0)
#define BCMPTM_SLICE_IN_USE                   (1 << 1)
#define BCMPTM_SLICE_SUPPORT_AUTO_EXPANSION   (1 << 2)

#define FP_MAX_LT_PARTS 12

#define BCMPTM_FP_ENTRY_SEG_WORDS 1

/*******************************************************************************
 * Typedefs
 */
/*! Entry Segment bitmap structure */
typedef struct bcmptm_fp_entry_seg_bmp_s {
    SHR_BITDCL w[BCMPTM_FP_ENTRY_SEG_WORDS];
} bcmptm_fp_entry_seg_bmp_t;

typedef struct bcmptm_slice_prio_map_s {
    uint16_t slice_id;
    uint16_t slice_priority;
} bcmptm_slice_prio_map_t;

typedef struct bcmptm_rm_tcam_lt_config_s {

    /* valid entry */
    uint8_t valid;

    /* Logical Table ID. */
    uint8_t lt_id;

    /*Group LT ID. */
    uint8_t group_ltid;

    /* Logical Table action priority.      */
    uint8_t lt_action_pri;

    /* Group priority.*/
    int priority;

    /* Pipe where group belongs to */
    int pipe_id;

    /* Logical Table to Physical slice mapping. */
    uint32_t lt_part_map;

    /* Logical table partition priority.   */
    uint8_t lt_part_pri[FP_MAX_LT_PARTS];

} bcmptm_rm_tcam_lt_config_t;

typedef struct bcmptm_rm_tcam_slice_s {

    /* BCMPTM_SLICE_XXX flags */
    uint32_t slice_flags;

    /* Physical Slice Id of the Slice.
     */
    uint8_t slice_id;
    /* when groups are shared, primary group Id will
     * be the first group, to which the current slice
     * is allocated.
     */
    int16_t primary_grp_id;

    /* Number of physical entries in the slice. */
    uint16_t num_entries;

    /* Number of free physical entries in the slice. */
    uint16_t free_entries;

    /* Number of physical presel entries in the slice. */
    uint16_t num_presel_entries;

    /* Number of free physical presel entries in the slice. */
    uint16_t free_presel_entries;

    /* Next slice Id offset in case slice is depth expanded.
     * If slice_id 2 is auto expanded to
     * slice_id 1 then next_slice_id_offset is -1.
     * If slice_id 2 is auto expanded to slice_id 3 then
     * next_slice_id_offset is 1.
     */
    int next_slice_id_offset;

    /* Previous slice Id offset in case slice is depth expanded. */
    int prev_slice_id_offset;

    /* Left Slice id in case of slice is width expanded. */
    int left_slice_id_offset;

    /* Right Slice id in case of slice is width expanded. */
    int right_slice_id_offset;

} bcmptm_rm_tcam_slice_t;

typedef struct bcmptm_rm_tcam_fp_group_s {

    /* valid entry */
    uint8_t valid;

    /*! Group Id. */
    uint32_t group_id;

    /*! next group id sharing same resources. */
    int16_t next_shared_group_id;

    /* group priority */
    uint32_t priority;

    /* group ltid */
    uint8_t group_ltid;

    /* Mode(SINGLE/DBLINTRA/DBLINTER/TRIPLE/QUAD) of
     * the group.
     */
    bcmptm_rm_tcam_group_mode_t group_mode;

    /* Primary Slice ID allocated for the group.
     * More than one slice would be allocated for group
     * if it is either width or depth expanded.
     */
    uint8_t primary_slice_id;

    /* Pipe where group belongs to */
    int pipe_id;

    /* Number of presel entries associated to this group. */
    uint16_t num_presel_entries;

    /* Number of valid entries associated to this group */
    uint32_t num_entries;

   /* Number of flags associated to this group */
    uint32_t flags;

    /* Size of entry hash bucket */
    uint16_t entry_hash_size;

    /* Main FP TCAM entry information of type
     * bcmptm_rm_tcam_prio_eid_info_t.
     */
    uint32_t entry_info_seg;

    /* Entry hash info offset in the
     * memory segment.
     */
    uint32_t entry_hash_seg;

    /* Presel TCAM entry information of type
     * bcmptm_rm_tcam_prio_eid_info_t.
     */
    uint32_t presel_entry_info_seg;

    /* Presel Entry hash info offset in the
     * memory segment.
     */
    uint32_t presel_entry_hash_seg;

    /* Size of presel entry hash bucket */
    uint16_t presel_entry_hash_size;

    /* Bitmap for entry segment used */
    bcmptm_fp_entry_seg_bmp_t entry_seg_bmp;


} bcmptm_rm_tcam_fp_group_t;

typedef struct bcmptm_rm_tcam_fp_stage_s {

    /* Stage Id */
    bcmptm_rm_tcam_fp_stage_id_t stage_id;

    /* transaction state for the ltid */
    uint8_t trans_state;

    /* Count of Pipes in this stage */
    uint8_t num_pipes;

    /* Count of Slices in this stage */
    uint8_t num_slices;

    /* Size of slice bundle in this stage */
    uint8_t slice_bundle_size;

    /* BCMPTM_STAGE_XXX flags */
    uint32_t flags;

    /* Max number of physical entries per slice */
    uint16_t max_entries_per_slice;

    /* Max number of presel physical entries per slice */
    uint16_t max_presel_entries_per_slice;

    /* Slice offset in the
     * memory segment for Slice
     */
    uint32_t slice_seg;

    /* Slice group id from ser */
    uint32_t slice_group_id;

    /* Total number of groups
     * in this stage.
     */
    uint16_t num_groups;


    /* Slice offset in the
     * memory segment.
     */
    uint32_t group_info_seg;

    /* LT config info offset in
     * the memory segment.
     */
    uint32_t lt_config_seg;

   /* Bitmap for usage of entry hash
    * and info segements
    */
    uint32_t entry_seg_bmp_offset;

    /* Size of entry hash bucket */
    uint16_t entry_hash_size;

    /* Size of presel entry hash bucket */
    uint16_t presel_entry_hash_size;

    /* Entry hash info offset in the
     * memory segment.
     */
    uint32_t entry_hash_seg;

    /* Entry info offset in the
     * memory segment.
     */
    uint32_t entry_info_seg;

    /* Presel Entry hash info offset
     * in the memory segment.
     */
    uint32_t presel_entry_hash_seg;

    /* Presel Entry info offset
     * in the memory segment.
     */
    uint32_t presel_entry_info_seg;

} bcmptm_rm_tcam_fp_stage_t;

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Calculate the memory required for FP based TCAM
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] total_size Memory required for FP.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int bcmptm_rm_tcam_fp_calc_mem_required(int unit,
                                        bcmltd_sid_t ltid,
                                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                                        void *mem_size);

/*!
 * \brief Initialize the metadata of FP based TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] start_ptr Start pointer to TCAM
 *                       resource manager shared memory
 * \param [in] offset Offset bytes from start_ptr to start of req_ltid data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int bcmptm_rm_tcam_fp_init_metadata(int unit, bool warm,
                                           bcmltd_sid_t ltid,
                                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                                           void *start_pointer,
                                           uint32_t *offset);

/*!
 * \brief To Fetch Stage information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] stage id
 * \param [out] stage structure pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int bcmptm_rm_tcam_fp_stage_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                 bcmptm_rm_tcam_fp_stage_t **stage_fc);

/*!
 * \brief To Fetch Slice information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 * \param [out] number of slices in that stage.
 * \param [out] pointer to first slice structure.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int bcmptm_rm_tcam_fp_slice_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t *num_slices,
                                 int pipe_id,
                                 bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                 bcmptm_rm_tcam_slice_t **slice_fc);

/*!
 * \brief To Fetch group information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] stage id
 * \param [in] group id
 * \param [out] group structure pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage/group exists
 */
extern int bcmptm_rm_tcam_fp_group_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint32_t group_id,
                                 bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                 bcmptm_rm_tcam_fp_group_t **group_fc);

/*!
 * \brief Get flags supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] stage_flags Stage flags.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_stage_flags_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint32_t *stage_flags);
/*!
 * \brief Get slice bundle size in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] slice_bundle_size Number of pipes.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_slice_bundle_size_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *slice_bundle_size);

/*!
 * \brief To Fetch lt config information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe number
 * \param [in] stage id
 * \param [out] lt config structure pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage/group exists
 */
extern int bcmptm_rm_tcam_fp_lt_config_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 int pipe_id,
                                 bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                 bcmptm_rm_tcam_lt_config_t **lt_config);

/*!
 * \brief To allocate slice for ifp group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [out] primary_silce_id to be updated in
 *               field group structure
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage/group exists
 * \retval SHR_E_RESOURCE No slice avaliable to allocate
 */
extern int bcmptm_rm_tcam_fp_slice_allocate(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  bcmptm_rm_tcam_entry_attrs_t *entry_attr);

/*!
 * \brief To convert logical entry_id to
 * \ Physical entry id
 *
 * \param [in]  unit Logical device id
 * \param [in]  ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in]  entry attrs structure
 * \param [in]  rm flags
 * \param [in]  logical entry idx.
 * \param [in]  width part number.
 * \param [out] depth part number.
 * \param [out] physical entry idx.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int bcmptm_rm_tcam_fp_tindex_to_sindex(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                                 uint32_t rm_flags,
                                 uint32_t logical_idx,
                                 uint8_t width_part,
                                 uint8_t *depth_part,
                                 uint32_t *phy_idx);

/*!
 * \brief To get entry parts count from group mode
 *
 * \param [in]  unit Logical device id
 * \param [in]  group mode
 * \param [in]  stage flags
 * \param [out] entry part count.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(int unit,
                                 uint32_t group_mode,
                                 uint32_t stage_flags,
                                 int *part_count);
/*!
 * \brief To get entry depth parts count from entry attrs
 *
 * \param [in]  unit Logical device id
 * \param [in]  ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in]  entry attrs
 * \param [out] entry depth part count.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int
bcmptm_rm_tcam_fp_entry_tcam_depth_parts_count(int unit,
                      bcmltd_sid_t ltid,
                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                      bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                      int *part_count);

/*!
 * \brief To expand an existing group
 *  Allocate new slice/slices for existing ifp group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [out] updated the num_entires in
 *               field group structure
 * \param [out] new slice id allocated
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_RESOURCE No slice availble to expand
 */
extern int bcmptm_rm_tcam_fp_slice_expand(int unit,
                     bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                     uint8_t *new_slice_id);
/*!
 * \brief To expand an existing group
 *  free unused slice/slices from existing ifp groups
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_NOT_FOUND No group/stage/slice exists
 */
extern int
bcmptm_rm_tcam_fp_group_compress(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *attrs);
/*!
 * \brief To map the virtual slices with the physical slices
 *
 * \param [in] unit Logical device id
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_NOT_FOUND No group/stage/slice exists
 */
extern int
bcmptm_rm_tcam_fp_virtual_slice_map_install(int unit,
                                         bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         int pipe_id,
                                         bcmptm_rm_tcam_fp_stage_id_t stage_id);
/*!
 * \brief Write Slice information to hardware.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 * \param [in] group_mode
 * \param [in] slice_idx
 * \param [in] enable
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_enable_set(int unit, bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               int pipe_id,
                               bcmptm_rm_tcam_fp_stage_id_t stage_id,
                               uint32_t group_mode,
                               uint8_t pri_slice_idx, int enable);
/*!
 * \brief Write logical table action priortiy information to hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage structure pointer.
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_group_lt_action_prio_install(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               int pipe_id,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc);
/*!
 * \brief Write logical table action priortiy information to hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_group_lt_partition_prio_install(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               uint8_t group_ltid,
                               int pipe_id,
                               bcmptm_rm_tcam_fp_stage_id_t stage_id);
/*!
 * \brief Write Slice selcodes information to hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [in] slice id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_selcodes_install(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t slice_id);
/*!
 * \brief Copy Slice selcodes information from primary to new slice in hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [in] slice id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_selcodes_copy(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t pri_slice_id,
                          uint8_t slice_id);
/*!
 * \brief Reset Slice selcodes information in hardware.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [in] slice id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_selcodes_reset(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t slice_id);
/*!
 * \brief Compare two group priorities linked to logical tables.
 *
 * \param [in] a Pointer to lt config
 * \param [in]  b Pointer to lt config
 *
 * \retval -1, 0 or 1
 */
extern int
bcmptm_rm_tcam_fp_lt_group_priority_compare(const void *a, const void *b);
/*!
 * \brief To delete the hw presel entries of the given slice
 *  if it is primary slice, clear the s/w hash too
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] slice_id Slice Number
 * \param [in] entry attrs structure
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_NOT_FOUND No group/stage/slice exists
 */
int
bcmptm_rm_tcam_fp_presel_entries_clear(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t slice_id,
                                 bcmptm_rm_tcam_entry_attrs_t *entry_attrs);
/*!
 * \brief To release the group's resources when no entries are
 *    present in the group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Failure
 */
int
bcmptm_rm_tcam_fp_free_group_resources(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             bcmptm_rm_tcam_entry_attrs_t *entry_attr);
/*!
 * \brief To reset lt_config structure when a group is deleted
 *    Also clear group's slice's enable flags
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] stage_fc Field stage structure.
 * \param [in] pri_slice_idx Primary slice index
 * \param [in] fg  Field group structure.
 * \param [in] slice_fc Field slice structure.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Failure
 */

int
bcmptm_rm_tcam_fp_group_priority_reset(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_fp_group_t *fg,
                              bcmptm_rm_tcam_slice_t *slice_fc);
/*
 * \brief To deallocate group unused slices.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry_attr  Entry attr structure.
 * \param [in] fg          Field group structure.
 * \param [in] slice_ptr   Field slice structure.
 * \param [in] clear_presel Flag to free presel entires
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Failure
 */
int
bcmptm_rm_tcam_fp_group_free_unused_slices(int unit,
                            bcmltd_sid_t ltid,
                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                            bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                            bcmptm_rm_tcam_fp_group_t *fg,
                            bcmptm_rm_tcam_slice_t *slice_ptr,
                            uint8_t *count_free_slices,
                            uint8_t clear_presel);

#endif /* RM_TCAM_FP_H */
