/*! \file bcmfp_internal.h
 *
 * APIs to manage all kinds of FP objects(control, stages, groups, entries,
 * qualifiers, actions, extractors)
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

#ifndef BCMFP_INTERNAL_H
#define BCMFP_INTERNAL_H

#include <bcmfp/bcmfp_util_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_entry_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>

/*Start
 *Temp code for tomahawk inports, to be replaced with DRD function*/
#define BCMFP_MAX_NUM_PIPES 4
#define BCMFP_TCAM_IPBMP_SIZE 34
/*******************************************************************************
  Macros
 */
 #define BCMFP_ACTION_IN_PROFILE_SET_1(action) \
            ((action == BCMFP_ACTION_NEW_UNTAG_PKT_PRIORITY) \
          || (action == BCMFP_ACTION_NEW_R_COLOR) \
          || (action == BCMFP_ACTION_NEW_Y_COLOR) \
          || (action == BCMFP_ACTION_NEW_G_COLOR) \
          || (action == BCMFP_ACTION_NEW_R_COS_MAP) \
          || (action == BCMFP_ACTION_R_INTPRI_TO_INNER_DOT1P) \
          || (action == BCMFP_ACTION_NEW_R_INTPRI) \
          || (action == BCMFP_ACTION_R_INTPRI_TO_TOS) \
          || (action == BCMFP_ACTION_R_INTPRI_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_NEW_R_UC_COS) \
          || (action == BCMFP_ACTION_NEW_R_MC_COS) \
          || (action == BCMFP_ACTION_NEW_Y_COS_MAP) \
          || (action == BCMFP_ACTION_Y_INTPRI_TO_INNER_DOT1P) \
          || (action == BCMFP_ACTION_NEW_Y_INTPRI) \
          || (action == BCMFP_ACTION_Y_INTPRI_TO_TOS) \
          || (action == BCMFP_ACTION_Y_INTPRI_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_NEW_Y_UC_COS) \
          || (action == BCMFP_ACTION_NEW_Y_MC_COS) \
          || (action == BCMFP_ACTION_NEW_G_COS_MAP) \
          || (action == BCMFP_ACTION_G_INTPRI_TO_INNER_DOT1P) \
          || (action == BCMFP_ACTION_NEW_G_INTPRI) \
          || (action == BCMFP_ACTION_G_INTPRI_TO_TOS) \
          || (action == BCMFP_ACTION_G_INTPRI_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_NEW_G_UC_COS) \
          || (action == BCMFP_ACTION_NEW_G_MC_COS) \
          || (action == BCMFP_ACTION_NEW_G_INTCN) \
          || (action == BCMFP_ACTION_NEW_Y_INTCN) \
          || (action == BCMFP_ACTION_NEW_R_INTCN))

 #define BCMFP_ACTION_IN_PROFILE_SET_2(action) \
            ((action == BCMFP_ACTION_NEW_R_ECN) \
          || (action == BCMFP_ACTION_NEW_Y_ECN) \
          || (action == BCMFP_ACTION_NEW_G_ECN) \
          || (action == BCMFP_ACTION_R_PRESERVE_DOT1P) \
          || (action == BCMFP_ACTION_R_OUTER_DOT1P_TO_INNER_DOT1P) \
          || (action == BCMFP_ACTION_NEW_R_OUTER_DOT1P) \
          || (action == BCMFP_ACTION_R_OUTER_DOT1P_TO_TOS) \
          || (action == BCMFP_ACTION_R_DOT1P_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_Y_PRESERVE_DOT1P) \
          || (action == BCMFP_ACTION_Y_OUTER_DOT1P_TO_INNER_DOT1P) \
          || (action == BCMFP_ACTION_NEW_Y_OUTER_DOT1P) \
          || (action == BCMFP_ACTION_Y_OUTER_DOT1P_TO_TOS) \
          || (action == BCMFP_ACTION_Y_DOT1P_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_G_PRESERVE_DOT1P) \
          || (action == BCMFP_ACTION_G_OUTER_DOT1P_TO_INNER_DOT1P) \
          || (action == BCMFP_ACTION_NEW_G_OUTER_DOT1P) \
          || (action == BCMFP_ACTION_G_OUTER_DOT1P_TO_TOS) \
          || (action == BCMFP_ACTION_G_DOT1P_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_NEW_R_DSCP) \
          || (action == BCMFP_ACTION_R_DSCP_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_R_PRESERVE_DSCP) \
          || (action == BCMFP_ACTION_NEW_Y_DSCP) \
          || (action == BCMFP_ACTION_Y_DSCP_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_Y_PRESERVE_DSCP) \
          || (action == BCMFP_ACTION_NEW_G_DSCP) \
          || (action == BCMFP_ACTION_G_DSCP_UPDATES_CANCEL) \
          || (action == BCMFP_ACTION_G_PRESERVE_DSCP) \
          || (action == BCMFP_ACTION_NEW_G_TOS) \
          || (action == BCMFP_ACTION_G_TOS_TO_PRE_FP_ING_OUTER_DOT1P))

/* Valid PortBitmap */
#define BCMFP_FIELD_VALID_PIPE_PBMP_GET(_inst_, _pbm_)                 \
        {                                                                   \
           int _ct_;                                                        \
           int _min_p_, _max_p_;                                            \
           if (_inst_ == -1) {                                              \
             _max_p_ = (BCMFP_MAX_NUM_PIPES * BCMFP_TCAM_IPBMP_SIZE);           \
             _min_p_ = 0;                                                   \
           } else {                                                         \
             _max_p_ = ((_inst_ + 1) * BCMFP_TCAM_IPBMP_SIZE);                \
             _min_p_ = (_inst_ * BCMFP_TCAM_IPBMP_SIZE);                      \
           }                                                                \
           for (_ct_ = _min_p_; _ct_ < _max_p_; _ct_++) {                   \
              BCMDRD_PBMP_PORT_ADD(_pbm_, _ct_);                            \
           }                                                                \
        }
/*End*/



/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_fp_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize chip specific function pointers.
 *
 * Initialize chip specific funciton pointers.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmfp_attach(int unit);

/*! Umberella Structure for FP module. This structure points to
 *- s/w state(Stored in HA and non HA memory) of different field
 *- processors supported on the device.
 */
typedef struct bcmfp_control_s {

    /*! Ingress Field Processor s/w state */
    bcmfp_stage_t *ifp;

    /*! Vlan Field Processor s/w state */
    bcmfp_stage_t *vfp;

    /*! Egress Field Processor s/w state */
    bcmfp_stage_t *efp;

    /*! Exact Match Field Processor s/w state */
    bcmfp_stage_t *emfp;
} bcmfp_control_t;

extern int
bcmfp_init(int unit, bool warm);

extern int
bcmfp_cleanup(int unit, bool warm);

extern int
bcmfp_imm_reg(int unit);

/*! Allocate and Initialize the memory required for
 *  per unit global variable of type bcmfp_control_t.
 */
extern int
bcmfp_common_init(int unit);

/*! De allocate the memory created for per unit global
 *  variable of type bcmfp_control_t.
 */
extern int
bcmfp_common_cleanup(int unit);

/*! Fetch the address to the FP module s/w state. */
extern int
bcmfp_control_get(int unit, bcmfp_control_t **fc);

/*! Assign the memory allocated for the given FP stage's s/w state in
 *- the given unit's FP module s/w state(of type bcmfp_control_t).
 */
extern int
bcmfp_stage_set(int unit, bcmfp_stage_id_t stage_id, bcmfp_stage_t *stage);

/*! Get the s/w state of the given stage and unit. */
extern int
bcmfp_stage_get(int unit, bcmfp_stage_id_t stage_id, bcmfp_stage_t **stage);

/*! Initialize the h/w specific attributes of the FP stage. */
extern int
bcmfp_stage_init(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for
 *- stage HW entry information. */
extern int
bcmfp_stage_hw_entry_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for
 *- stage miscellaneous information. */
extern int
bcmfp_stage_misc_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory(is part of HA area) allocated for
 *- stage operational information. */
extern int
bcmfp_stage_oper_info_cleanup(int unit, bcmfp_stage_t *stage);

/*! Assign the FP stage operational mode(Global/PipeLocal) */
extern int
bcmfp_stage_oper_mode_set(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t oper_mode);

/*! Get the FP stage operational mode(Global/PipeLocal) */
extern int
bcmfp_stage_oper_mode_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t *oper_mode);

/*! Initilize the extractors(or Multiplexers) available in all hirarchial
 *- levels of all group modes(SINGLE/DBLINTRA/DBLINTER/TRIPLE). In multiwide
 *- group modes(DBLINTER/TRIPLE) extractors for second and third slice will
 *- also be initialized in addition to the extractors of first slice. Memory
 *- allocated to hold this information is stored in non HA memory as this
 *- information will not be changed during the run time. So we build this
 *- information on every the SDK boot(both COLD and WARM boot).
 */
extern int
bcmfp_ext_cfg_db_init(int unit, bcmfp_stage_t *stage);

/*! Deallocate the memory allocated for extractor data base allocated in
 *- bcmfp_ext_cfg_db_init API.
 */
extern int
bcmfp_ext_cfg_db_cleanup(int unit, bcmfp_stage_t *stage);

/*! Initialize all the variables in bcmfp_ext_cfg_t structure to default
 *- values.
 */
extern int
bcmfp_ext_cfg_t_init(int unit, bcmfp_ext_cfg_t *ext_cfg);

/*! Initialize all the variables in bcmfp_ext_sel_t structure to default
 *- values.
 */
extern int
bcmfp_ext_codes_init(int unit, bcmfp_ext_sel_t *ext_codes);

/*! Allocate memory for extractor configuration to copy the data
 *- in ext_cfg to it and insert that into the extractor configuration
 *- database of the stage.
 */
extern int
bcmfp_ext_cfg_insert(int unit,
                     bcmfp_stage_t *stage,
                     bcmfp_ext_cfg_mode_t emode,
                     uint8_t level,
                     bcmfp_ext_cfg_t *ext_cfg);

extern int
bcmfp_qual_cfg_db_init(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_qual_cfg_db_cleanup(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_qual_cfg_t_init(int unit,
                      bcmfp_qual_cfg_t *qual_cfg);

extern int
bcmfp_presel_qual_cfg_t_init(int unit,
                      bcmfp_qual_cfg_t *presel_qual_cfg);

extern int
bcmfp_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qual_t qid,
                      bcmfp_qual_cfg_t *new_qual_cfg);

extern int
bcmfp_presel_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qual_t qid,
                      bcmfp_qual_cfg_t *new_qual_cfg);

extern int
bcmfp_presel_qual_cfg_db_cleanup(int unit,
                          bcmfp_stage_t *stage);

extern int
bcmfp_qual_set(int unit,
               bcmfp_stage_id_t stage_id,
               bcmfp_group_id_t group,
               bcmfp_entry_type_t entry_type,
               bcmfp_qual_t qual,
               uint32_t *bitmap,
               uint32_t *data,
               uint32_t *mask,
               uint32_t **ekw);

extern int
bcmfp_qual_get(int unit,
               bcmfp_stage_id_t stage_id,
               bcmfp_group_id_t group,
               bcmfp_entry_type_t entry_type,
               bcmfp_qual_t qual_id,
               uint32_t *data,
               uint32_t *mask,
               uint32_t **ekw);

extern int
bcmfp_qual_data_size_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qual_t qual_id,
                         uint32_t *size);
extern int
bcmfp_ipbm_validate(int unit,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_group_id_t group_id,
                    bcmdrd_pbmp_t device_pbmp);
extern int
bcmfp_device_ipbm_to_pipe_ipbmp(int unit,
                                bcmdrd_pbmp_t device_pbmp,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_pbmp);

extern int
bcmfp_pipe_ipbmp_to_device_pbmp(int unit,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_pbmp,
                                bcmdrd_pbmp_t *device_pbmp);

extern int
bcmfp_qual_presel_set(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group,
                      bcmfp_qual_t qual,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw);

extern int
bcmfp_qual_presel_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group,
                      bcmfp_qual_t qual_id,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw);

extern int
bcmfp_qualifier_enum_set(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qual_t qual,
                         int value,
                         uint32_t *data,
                         uint32_t *mask);

extern int
bcmfp_qualifier_enum_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qual_t qual,
                         uint32_t data,
                         uint32_t mask,
                         int *value);

extern int
bcmfp_action_cfg_db_init(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_action_conflict_check(int unit,
                            bcmfp_stage_t *stage,
                            bcmfp_action_t action1,
                            bcmfp_action_t action2);

extern int
bcmfp_action_cfg_db_cleanup(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_action_cfg_t_init(int unit,
                        bcmfp_action_cfg_t *action_cfg);

extern int
bcmfp_presel_action_cfg_t_init(int unit,
                        bcmfp_action_cfg_t *presel_action_cfg);

extern int
bcmfp_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *action_cfg);

extern int
bcmfp_presel_action_cfg_insert(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_cfg_t *presel_action_cfg);
extern int
bcmfp_presel_action_cfg_db_cleanup(int unit,
                            bcmfp_stage_t *stage);
extern int
bcmfp_action_data_size_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_action_t action_id,
                           uint32_t *size);

extern int
bcmfp_action_value_set(int unit,
                       uint32_t *entbuf,
                       bcmfp_action_offset_cfg_t *a_cfg_ptr);
extern int
bcmfp_action_offset_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_action_t action,
                        bcmfp_action_offset_cfg_t *a_cfg_ptr);

extern int
bcmfp_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t group_id,
                 uint32_t action,
                 uint32_t param,
                 uint32_t **edw);
extern int
bcmfp_action_param_check(int unit,
                         uint32_t group_id,
                         uint32_t stage_id,
                         uint32_t action,
                         uint32_t param);
extern int
bcmfp_presel_action_set(int unit,
                 uint32_t stage_id,
                 uint32_t group_id,
                 uint32_t action,
                 uint32_t param,
                 uint32_t **edw);

extern int
bcmfp_action_presel_init(int unit,
                         uint32_t stage_id,
                         uint32_t group_id,
                         uint32_t entry_id,
                         uint32_t **edw,
                         uint8_t  flag);

extern int
bcmfp_fn_ptrs_cleanup(int unit, bcmfp_stage_t *stage);

extern int
bcmfp_group_parts_count(int unit,
                        uint32_t group_flags,
                        uint8_t *part_count);

extern int
bcmfp_group_create(int unit, uint32_t trans_id,
                   bcmfp_group_t *fg);

extern int
bcmfp_group_update(int unit, uint32_t trans_id,
                   bcmfp_group_t *fg);
extern int
bcmfp_group_delete(int unit, uint32_t trans_id,
                   bcmfp_stage_id_t stage_id,
                   bcmfp_group_id_t group_id);

extern int
bcmfp_group_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_oper_info_t *group_oper_info,
                        bcmptm_rm_tcam_fp_group_selcode_t *selcode);
extern int
bcmfp_group_combined_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_t *fg);
extern int
bcmfp_group_selcodes_install(int unit,
                             bcmfp_group_t *fg);

extern int
bcmfp_group_entry_install(int unit,
                          bcmfp_group_t *fg,
                          uint32_t **ekw,
                          uint32_t **edw);

extern int
bcmfp_group_oper_info_insert(int unit,
                             bcmfp_group_t *fg);

extern int
bcmfp_group_oper_info_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id);

extern int
bcmfp_group_presel_entry_install(int unit,
                                 bcmfp_group_t *fg);

extern int
bcmfp_group_flags_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      uint32_t *group_flags);
extern int
bcmfp_group_tbl_inst_get(int unit,
                      bcmfp_group_id_t group_id,
                      bcmfp_stage_id_t stage_id,
                      int *tbl_inst);

extern int
bcmfp_group_qual_part_offset_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_group_id_t group_id,
                                 bcmfp_entry_type_t entry_type,
                                 uint8_t part_idx,
                                 bcmfp_qual_t qual,
                                 bcmfp_qual_offset_info_t **q_offset);

extern int
bcmfp_group_qual_bitmap_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_qual_t qual,
                            uint32_t *qual_bitmap);

extern int
bcmfp_group_oper_info_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t **group_oper_info);

extern int
bcmfp_group_install(int unit, uint32_t trans_id,
                    bcmfp_stage_t *stage,
                    bcmfp_group_t *fg);

extern int
bcmfp_group_uninstall(int unit, uint32_t trans_id,
                      bcmfp_stage_t *stage,
                      bcmfp_group_id_t group_id);

extern int
bcmfp_group_action_res_id_allocate(int unit,
                                   int tbl_inst,
                                   bcmfp_stage_id_t stage_id,
                                   uint8_t *action_res_id);
extern int
bcmfp_group_selcode_key_get(int unit,
                            bcmfp_stage_t *stage,
                            bcmfp_group_t *fg);
extern int
bcmfp_entry_key_match_type_set(int unit,
                               bcmfp_stage_t *stage,
                               bcmfp_group_id_t group_id,
                               bcmfp_group_slice_mode_t slice_mode,
                               bcmfp_group_type_t  port_pkt_type,
                               uint32_t **ekw);
extern int
bcmfp_entry_insert(int unit,
                   uint32_t flags,
                   uint32_t trans_id,
                   int tbl_inst,
                   bcmfp_stage_id_t stage_id,
                   bcmltd_sid_t req_ltid,
                   bcmfp_group_id_t group_id,
                   bcmfp_entry_id_t entry_id,
                   uint32_t entry_prio,
                   uint32_t **ekw,
                   uint32_t **edw);
extern int
bcmfp_entry_delete(int unit,
                   uint32_t flags,
                   uint32_t trans_id,
                   bcmfp_stage_id_t stage_id,
                   bcmltd_sid_t req_ltid,
                   bcmfp_group_id_t group_id,
                   bcmfp_entry_id_t entry_id,
                   uint32_t **ekw);

extern int
bcmfp_entry_valid_bit_set(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          uint32_t **entry_words);
extern int
bcmfp_entry_key_info_get(int unit,
                         bool is_presel,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id,
                         uint16_t *start_bit,
                         uint16_t *end_bit);
extern int
bcmfp_entry_mask_info_get(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          uint16_t *start_bit,
                          uint16_t *end_bit);

extern int
bcmfp_group_mode_max_entries(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_mode_t group_mode,
                             uint32_t *max_entries);

extern int
bcmfp_group_tcam_size_get(int unit,
                          bool is_presel,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          uint32_t *size);
extern int
bcmfp_group_policy_size_get(int unit,
                            bool is_presel,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            uint32_t *size);

extern int
bcmfp_profile_add(int unit,
                  uint32_t trans_id,
                  int tbl_inst,
                  bcmltd_sid_t req_ltid,
                  uint8_t num_profiles,
                  bcmdrd_sid_t *profile_sids,
                  uint32_t *profile_data[],
                  uint32_t *profile_ref_count,
                  int *profile_index);
extern int
bcmfp_profile_delete(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     uint8_t num_profiles,
                     bcmltd_sid_t *profile_sids,
                     uint32_t *profile_ref_count,
                     int profile_index);
extern int
bcmfp_profile_entry_lookup(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           uint32_t *profile_data,
                           int index);
extern int
bcmfp_profile_entry_insert(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           uint32_t *profile_data,
                           int index);
extern int
bcmfp_profile_entry_delete(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           int index);
extern int
bcmfp_profile_index_alloc(int unit,
                          uint32_t trans_id,
                          int tbl_inst,
                          bcmltd_sid_t req_ltid,
                          uint8_t num_profiles,
                          bcmdrd_sid_t *profile_sids,
                          uint32_t *profile_data[],
                          uint32_t *profile_ref_count,
                          int *index);
extern int
bcmfp_udf_qual_id_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      uint8_t udf_chunk_id,
                      bcmfp_qual_t *qual_id);

extern int
bcmfp_src_class_mode_set(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_sid_t sid,
                         int pipe_id,
                         uint32_t mode);
extern int
bcmfp_group_action_profile_install(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_t *stage,
                         bcmfp_group_t *fg);
extern int
bcmfp_group_action_profile_uninstall(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_t *stage,
                         bcmfp_group_id_t group_id);
extern int
bcmfp_entry_qos_profile_install(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_t *stage,
                         bcmltd_sid_t ltid,
                         int pipe_id,
                         uint32_t flag,
                         bcmfp_group_id_t group_id,
                         uint32_t *prof_1_data,
                         uint32_t *prof_2_data,
                         int *qos_prof_idx);
extern int
bcmfp_entry_qos_profile_uninstall(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_t *stage,
                         bcmfp_group_id_t group_id,
                         int qos_prof_idx);
extern int
bcmfp_entry_qos_profile_id_get(int unit,
                            uint32_t trans_id,
                            bcmfp_stage_t *stage,
                            bcmfp_group_id_t group_id,
                            uint32_t **edw,
                            int *qos_prof_idx);
extern int
bcmfp_custom_entry_set(int unit,
                   uint32_t trans_id,
                   bcmfp_stage_t *stage,
                   bcmfp_group_id_t gid,
                   bool only_ekw,
                   int qos_prof_idx,
                   int em_class_id,
                   uint32_t **ekw,
                   uint32_t **edw);
extern int
bcmfp_default_policy_install(int unit,
                   uint32_t trans_id,
                   bcmfp_stage_t *stage,
                   bcmfp_group_t *fg,
                   int qos_prof_idx,
                   int em_class_id,
                   uint32_t **edw);
extern int
bcmfp_default_policy_uninstall(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_t *stage,
                         bcmfp_group_id_t group_id,
                         int default_policy_idx);
extern int
bcmfp_group_presel_entry_validate(int unit,
                         int tbl_inst,
                         bcmfp_stage_id_t stage_id,
                         uint8_t lookup_id,
                         uint8_t num_presel_ids);

/* Returns a string name corresponding to action enumeration.
 * This is mostly used in LOG messages.
 */
extern char *bcmfp_action_string(bcmfp_action_t action);

#endif /* BCMFP_INTERNAL_H */
