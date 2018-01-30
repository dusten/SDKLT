/*! \file bcmfp_trans_internal.h
 *
 * BCMFP component transaction management data structures and
 * functions are present in this file.
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

#ifndef BCMFP_TRANS_INTERNAL_H
#define BCMFP_TRANS_INTERNAL_H

#include <bcmimm/bcmimm_int_comp.h>
#include <bcmfp/bcmfp_stage_internal.h>

/*! Transaction commands for BCMFP */
typedef enum bcmfp_trans_cmd_s {
    /* To check if the stage operational information  is in
     * idle state before transaction is committed. If not means
     * something went wrong in previous transaction.
     */
    BCMFP_TRANS_CMD_IDLE_CHECK = 1,
    /* Free the back up operational state. */
    BCMFP_TRANS_CMD_COMMIT = 2,
    /* Copy back up operational information to active. */
    BCMFP_TRANS_CMD_ABORT = 3
} bcmfp_trans_cmd_t;

/*! Transaction states for BCMFP */
typedef enum bcmfp_trans_state_s {
    /*! Idle state */
    BCMFP_TRANS_STATE_IDLE = 0,
    /*! Uncommited changes in Active DB */
    BCMFP_TRANS_STATE_UC_A = 1,
    /*! Commiting changes. i/e. copying Active to Backup */
    BCMFP_TRANS_STATE_COPY_A_TO_B = 2,
    /*! Aborting changes. i/e. copying Backup to Active */
    BCMFP_TRANS_STATE_COPY_B_TO_A = 3
} bcmfp_trans_state_t;

/*!
 * \brief BCMFP transaction commands execution for all stages.
 *
 * This function is called from commit and abort call back functions
 * (bcmfp_XXX_trans_commit and bcmfp_XXX_trans_abort) corresponding to
 * all BCMFP stages which are registered with in-memory component.
 * Based on the transaction command it will either free the back up
 * operational state or copy the back up to active operational state.
 *
 * \param [in] unit This is device unit number.
 * \param [in] stage_id This is one of the BCMFP stage id.
 * \param [in] trans_cmd This is one of the BCMFP transaction commands.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_trans_cmd(int unit,
                bcmfp_stage_id_t stage_id,
                bcmfp_trans_cmd_t trans_cmd);
/*!
 * \brief Transaction commit callback function for ingress stage
 * logical table(FP_ING_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * ingress stage logical table(FP_ING_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * ingress logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of ingress stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_ing_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for ingress
 * stage logical table(FP_ING_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every ingress logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of ingress stage operational state to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_ing_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context);
/*!
 * \brief Transaction commit callback function for egress stage
 * logical table(FP_EGR_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * egress stage logical table(FP_EGR_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * egress logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of egress stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_egr_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for egress
 * stage logical table(FP_EGR_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every egress logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of egress stage operational information to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_egr_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context);
/*!
 * \brief Transaction commit callback function for vlan stage
 * logical table(FP_VLAN_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * vlan stage logical table(FP_VLAN_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * vlan logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of vlan stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_vlan_trans_commit(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for vlan
 * stage logical table(FP_VLAN_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every vlan logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of vlan stage operational state to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_vlan_trans_abort(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context);
/*!
 * \brief Transaction commit callback function for exact match stage
 * logical table(FP_EM_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * exact match stage logical table(FP_EM_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * exact match logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of exact match stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_em_trans_commit(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for exact match
 * stage logical table(FP_EM_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every exact match logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of exact match stage operational state to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_em_trans_abort(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     void *context);
/*!
 * \brief Set the transaction state of the HA block created either init/run
 *  time.Following are the different reasons for any HA block to be in any
 *  of the valid transaction states.
 *  ----------------------------------------------------------------------------
 *  |                | 1. When HA block is created(Either component init time) |
 *  |    IDLE        |    OR run time).                                        |
 *  |                | 2. When Transaction is commited/aborted successfully.   |
 *  ----------------------------------------------------------------------------
 *  |                | 1. If any filed value in HA block(that is created init  |
 *  |    UC_A        |    time) is changed in the transaction.                 |
 *  | (Uncommitted   | 2. When HA elements are "acquired from"/"release to"    |
 *  |  Active)       |    HA block(that is created run time).                  |
 *  ----------------------------------------------------------------------------
 *  |                | 1. Before started copying the active HA block to backup |
 *  |  COPY_A_TO_B   |    block. This is triggered when Transaction manager    |
 *  | (Copy Active   |    dispatches transaction commit command.               |
 *  |  to backup)    |                                                         |
 *  ----------------------------------------------------------------------------
 *  |                | 1. Before started copying the backup HA block to active |
 *  |  COPY_B_TO_A   |    block. This is triggered when Transaction manager    |
 *  | (Copy Backup   |    dispatches transaction abort command.                |
 *  |  to active)    |                                                         |
 *  ----------------------------------------------------------------------------
 *
 * \param [in] unit This is device unit number.
 * \param [in] blk_id HA block id(created at either init/run time).
 * \param [in] trans_state Transaction state(IDLE/UC_A/
 *                         COPY_A_TO_B/COPY_B_TO_A).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_trans_state_set(int unit,
                      uint8_t blk_id,
                      bcmfp_trans_state_t trans_state);
/*!
 * \brief Get the transaction state of the HA block created either
 *  FP component init/run time.
 *
 * \param [in] unit This is device unit number.
 * \param [in] blk_id HA block id(created at either init/run time).
 * \param [in] trans_state Transaction state(IDLE/UC_A/
 *                         COPY_A_TO_B/COPY_B_TO_A).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_trans_state_get(int unit,
                      uint8_t blk_id,
                      bcmfp_trans_state_t *trans_state);

#endif /* BCMFP_TRANS_INTERNAL_H */
