/*! \file bcmfp_oper_internal.h
 *
 * APIs to SET/GET opertaional information saved in HA area.
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

#ifndef BCMFP_OPER_INTERNAL_H
#define BCMFP_OPER_INTERNAL_H

#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>

/* Group Operation information which may keep changing
 * in the run time due to user configurations.
 */
typedef struct bcmfp_group_oper_info_s {
    /* Valid Group */
    bool valid;
    /* Group ID */
    bcmfp_group_id_t group_id;
    /* Group Priority */
    uint32_t group_prio;
    /* Group Mode */
    bcmfp_group_mode_t group_mode;
    /* Group Slice Mode */
    bcmfp_group_slice_mode_t group_slice_mode;
    /* Pipe to which group belongs to */
    int tbl_inst;
    /* Group Flags */
    uint32_t flags;
    /* Number of presel Ids. */
    uint32_t num_presel_ids;
    /* Port Packet type */
    uint8_t port_pkt_type;
    /* Slice Id*/
    uint8_t slice_id;
    /* Number of entries created in the group */
    uint32_t ref_count;
    /* action resolution Id used in resolving conflicting actions
     * b/w groups. Each action_res_id will be assigned one unique
     * priority.
     */
    uint16_t action_res_id;
    /* offset and width information of different qualifiers in
     * different parts with in the group.
     */
    bcmfp_ha_blk_info_t qual_offset_info;
    /* Extractors(in use) Information in all parts of a group. */
    bcmfp_ext_sel_t ext_codes[BCMFP_ENTRY_PARTS_MAX];
} bcmfp_group_oper_info_t;

/* Per BCMFP stage(INGRESS/EGRESS/VLAN/EM) operation information
 * that needs to retained on device reboots.
 */
typedef struct bcmfp_stage_oper_info_s {
    /* Transaction state of the stage. */
    bcmfp_trans_state_t trans_state;

    /* Stage operational mode */
    bcmfp_stage_oper_mode_t oper_mode;

    /* Number of groups created in the stage. */
    uint16_t num_groups;

    /* Per Group operational information */
    uint32_t group_oper_info_seg;

    /* Number of entries in keygen profile table */
    uint16_t num_keygen_prof;

    /* Reference Count for entries in keygen profile table.
     * This is applicable for stages which have Tomahawk IFP
     * style TCAM key generation algorithm.
     */
    uint32_t keygen_prof_ref_count_seg;

    /* Number of entries in action profile table */
    uint16_t num_action_prof;

    /* Reference Count for entries in action profile table.
     * This is applicable for stages which have PDD style
     * policy table.
     */
    uint32_t action_prof_ref_count_seg;

    /* Number of entries in QOS profile table */
    uint16_t num_qos_prof;

    /* Reference Count for entries in QOS action profile table.
     * This is applicable for stages which have PDD style
     * policy table.
     */
    uint32_t qos_prof_ref_count_seg;
} bcmfp_stage_oper_info_t;

/*!
 * \brief Get stage operational information of a given stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] stage_oper_info Stage operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_info_get(int unit,
                 bcmfp_stage_id_t stage_id,
                 bcmfp_stage_oper_info_t **stage_oper_info);

/*!
 * \brief Get operational mode(Global/PerPipe) of a given stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] oper_mode Stage operational mode.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_mode_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t *oper_mode);
/*!
 * \brief Get group operational information in a given stage id.
 *   Since group ids are unique across all pipes, pipe id is not
 *   required in fetching the group operational information.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] oinfo Group operational nformation.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      bcmfp_group_oper_info_t **oinfo);
/*!
 * \brief Get the reference count array for keygen profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] keygen_prof_ref_count pointer to array of reference
 *              counts saved for all entries in keygen profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_keygen_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             uint32_t **keygen_prof_ref_count);
/*!
 * \brief Get the reference count array for action profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] keygen_prof_ref_count pointer to array of reference
 *              counts saved for all entries in action profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_action_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             uint32_t **action_prof_ref_count);
/*!
 * \brief Get the reference count array for qos action profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] keygen_prof_ref_count pointer to array of reference
 *              counts saved for all entries in qos action profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_qos_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             uint32_t **qos_prof_ref_count);
/*!
 * \brief Get the action resolution id(h/w LTID) for the given
 *  group id and stage id. Since group ids are unique across all
 *  pipes, pipe id is not required in fetching the action resolution id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] action_res_id Groups action resolution id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_action_res_id_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t *action_res_id);
/*!
 * \brief Each qualifier in a group has offset(s) and width(s) information
 *   in the final key. Thease offset(s) and width(s) may spread across
 *   multiple parts if group is multiwide. This function is to get the
 *   qualifiers offset(s) and width(s) information of a qualifier in a
 *   given part_id, group_id and stage_id combination.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] part_id First/second/Third Slice assigned to group.
 * \param [in] qual_id Qualifier id.
 * \param [out] q_offset offset(s) and width(s) information of a qualifier.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_qual_offset_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_qual_t qual_id,
                        bcmfp_qual_offset_info_t *q_offset);
/*!
 * \brief To check if a particular qualifier is already present in the
 *   group or not. qualifier can present in any part.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] qual_id Qualifier id.
 * \param [out] qual_present Flag to indicate whether qual_id is present
 *              in the group or not. TRUE means present, FALSE means not
 *              present.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_qual_present(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id,
                         bcmfp_qual_t qual_id,
                         bool *qual_present);

/*!
 * \brief Get the flags assigned to a group.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] group_flags group flags.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_flags_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      uint32_t *group_flags);
/*!
 * \brief Get the pipe to which group belongs to.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] tbl_inst groups pipe id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_tbl_inst_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      int *tbl_inst);
/*!
 * \brief Get the flags assigned to a group.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] part_id First/second/Third Slice assigned to group.
 * \param [out] ext_codes Extractor controls.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_ext_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_ext_sel_t **ext_codes);
/*!
 * \brief  Save the group action resolution id to HA space.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] action_res_id Action resolution id to be saved.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_action_res_id_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t action_res_id);
/*!
 * \brief  Save the groups operational data, generated after group
 *  is successfully created, to HA space.
 *
 * \param [in] unit Logical device id.
 * \param [in] fg Group operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_set(int unit,
                          bcmfp_group_t *fg);
/*!
 * \brief  Update the groups operational data, generated after group
 *  is successfully updated, to HA space.
 *
 * \param [in] unit Logical device id.
 * \param [in] fg Group operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_update(int unit,
                             bcmfp_group_t *fg);
/*!
 * \brief  Save the different parameters of the group.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id to which group belongs to.
 * \param [in] group_id group id created in stage_id.
 * \param [in] group_prio group priority.
 * \param [in] group_slice_mode Group slice mode.
 * \param [in] port_pkt_type Incoming port type(HIGIG/Ethernet),
 *             packet type(V4L3/V6L3/FCoE/L2) to which rules in the
 *             groups applies.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_params_set(int unit,
                       bcmfp_stage_id_t stage_id,
                       int pipe_id,
                       bcmfp_group_id_t group_id,
                       uint32_t group_prio,
                       bcmfp_group_slice_mode_t group_slice_mode,
                       uint8_t port_pkt_type);
/*!
 * \brief Set operational mode(Global/PerPipe) of a given stage id.
 *  This function is being called only during FP component
 *  initialization time. This state information comes from FP_CONFIG
 *  LT. Since field values in FP_CONFIG values are save in IMM HA, FP
 *  need not save it again in HA. So eventually this funtion will be
 *  removed.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] oper_mode Stage operational mode.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_mode_set(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t oper_mode);


/*!
 * \brief  Delete the group operational information saved in
 *  HA memory. This happens when group is deleted.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id);
/*!
 * \brief  Reset the different parameters of the group, which are
 *  saved in HA, to default values. This happens when group is
 *  deleted.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_params_reset(int unit,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_group_id_t group_id);

/*!
 * \brief During FP component initialization, each supported
 *  stage will be allocated one HA block. These HA blocks have
 *  the data layout as below. This function is o intialize the
 *  fields in each data segment depicted in this picture.
 *
 *   --------------------------------------
 *  |                                      |
 *  |           HA block header            |
 *  |         (bcmfp_ha_blk_hdr_t)         |
 *  |                                      |
 *   --------------------------------------
 *  |                                      |
 *  |   stage operational information      |
 *  |     (bcmfp_stage_oper_info_t)        |
 *  |                                      |
 *   --------------------------------------
 *  |                                      |
 *  |    group oper info of group id 0     |
 *  |      (bcmfp_group_oper_info_t)       |
 *  |                                      |
 *   --------------------------------------
 *  |                                      |
 *  |    group oper info of group id 1     |
 *  |      (bcmfp_group_oper_info_t)       |
 *  |                                      |
 *   --------------------------------------
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                                      |
 *   --------------------------------------
 *  |                                      |
 *  |   group oper info of group id N-1    |
 *  |      (bcmfp_group_oper_info_t)       |
 *  |                                      |
 *   --------------------------------------
 *  |                                      |
 *  |     keygen profile table entries     |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |                                      |
 *  |     action profile table entries     |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |                                      |
 *  |     qos profile table entries        |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] blk_size size of the HA block id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_info_init(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t blk_size);

#endif /* BCMFP_OPER_INTERNAL_H */
