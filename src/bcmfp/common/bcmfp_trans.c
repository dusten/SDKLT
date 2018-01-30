/*! \file bcmfp_trans.c
 *
 * BCMFP component transaction management APIs
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

#include <bsl/bsl.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmfp/bcmfp_lt_entry.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_TRANS

/* Transaction state of all HA blocks created during FP
 * component initialization time.
 * This variable holds only either IDLE or UC_A states.
 * If any of the HA blokc is in UC_A the this will be set
 * to UC_A.
 * If all of the HA blocks have transaction state IDLE,
 * the this will be set to IDLE.
 */
bcmfp_trans_state_t trans_state_fixed[BCMDRD_CONFIG_MAX_UNITS]
                                     = { BCMFP_TRANS_STATE_UC_A };
/* Transaction state of all HA blocks created during FP
 * component run time.
 * This variable holds only either IDLE or UC_A states.
 * If any of the HA blokc is in UC_A the this will be set
 * to UC_A.
 * If all of the HA blocks have transaction state IDLE,
 * the this will be set to IDLE.
 */
bcmfp_trans_state_t trans_state_dynamic[BCMDRD_CONFIG_MAX_UNITS]
                                     = { BCMFP_TRANS_STATE_UC_A };

STATIC int
bcmfp_trans_commit(int unit, bcmfp_stage_id_t stage_id)
{
    bool idle_flag = TRUE;
    void *ha_mem = NULL;
    void *backup_ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Copy the active HA block to back up HA block
     * corresponding to the given stage.
     */
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, backup_blk_id, &backup_blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, backup_blk_id, &backup_ha_mem));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_A_TO_B;
    /* No need to set the trans_state of active block to IDLE as
     * trans_state of backup block is always IDLE.
     */
    sal_memcpy(backup_ha_mem, ha_mem, blk_hdr->blk_size);

    /* If all the fixed HA blocks are in IDLE state then set the global
     * transaction state to IDLE.
     */
    for (blk_id = BCMFP_HA_BLK_ID_FIXED_MIN;
        blk_id < BCMFP_HA_BLK_ID_FIXED_MAX;
        blk_id++) {
        _func_rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_NOT_FOUND);
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            continue;
        }
        if (blk_hdr->trans_state != BCMFP_TRANS_STATE_IDLE) {
            idle_flag = FALSE;
            break;
        }
    }
    if (idle_flag == TRUE) {
        trans_state_fixed[unit] = BCMFP_TRANS_STATE_IDLE;
    }

    /* If global transaction state of dynamic HA blocks is IDLE,
     * then nothing pending.
     */
    if (trans_state_dynamic[unit] == BCMFP_TRANS_STATE_IDLE) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Commit all the non IDLE dynamic HA blocks. If more than one fixed
     * HA block is modified in the transaction,then all the dynamic blocks
     * are committed when committing the first fixed HA block.
     */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MIN;
        blk_id < BCMFP_HA_BLK_ID_DYNAMIC_MAX;
        blk_id++) {
        _func_rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_NOT_FOUND);
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            continue;
        }
        if (blk_hdr->trans_state == BCMFP_TRANS_STATE_IDLE) {
            continue;
        }
        backup_blk_id = blk_hdr->backup_blk_id;
        if (backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA back up block ID:%0d "),
                                    backup_blk_id));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_blk_hdr_get(unit, backup_blk_id,
                                    &backup_blk_hdr));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, backup_blk_id,
                                 &backup_ha_mem));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
        blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_A_TO_B;
        sal_memcpy(backup_ha_mem, ha_mem, blk_hdr->blk_size);
        backup_blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    }
    trans_state_dynamic[unit] = BCMFP_TRANS_STATE_IDLE;
    /* This is required because _func_rv could be set to
     * SHR_E_NOT_FOUND in the abouve loop which is OK.
     */
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_trans_abort(int unit, bcmfp_stage_id_t stage_id)
{
    bool idle_flag = TRUE;
    void *ha_mem = NULL;
    void *backup_ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Copy the back up HA block to active HA block
     * corresponding to the given stage.
     */
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, backup_blk_id, &backup_blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, backup_blk_id, &backup_ha_mem));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_B_TO_A;
    /* No need to set the trans_state of active block to IDLE as
     * trans_state of backup block is always IDLE.
     */
    sal_memcpy(ha_mem, backup_ha_mem, blk_hdr->blk_size);

    /* If all the fixed HA blocks are in IDLE state then set the global
     * transaction state to IDLE.
     */
    for (blk_id = BCMFP_HA_BLK_ID_FIXED_MIN;
        blk_id < BCMFP_HA_BLK_ID_FIXED_MAX;
        blk_id++) {
        _func_rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_NOT_FOUND);
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            continue;
        }
        if (blk_hdr->trans_state != BCMFP_TRANS_STATE_IDLE) {
            idle_flag = FALSE;
            break;
        }
    }
    if (idle_flag == TRUE) {
        trans_state_fixed[unit] = BCMFP_TRANS_STATE_IDLE;
    }

    /* If global transaction state of dynamic HA blocks is IDLE,
     * then nothing pending.
     */
    if (trans_state_dynamic[unit] == BCMFP_TRANS_STATE_IDLE) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Abort all the dynamic HA blocks. If more than one fixed HA block
     * is modified in the transaction,then all the dynamic blocks are
     * aborted when aborting the first fixed HA block.
     */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MIN;
        blk_id < BCMFP_HA_BLK_ID_DYNAMIC_MAX;
        blk_id++) {
        _func_rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_NOT_FOUND);
        if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
            continue;
        }
        if (blk_hdr->trans_state == BCMFP_TRANS_STATE_IDLE) {
            continue;
        }

        backup_blk_id = blk_hdr->backup_blk_id;
        if (backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP:HA back up block ID  :%0d "),
                                    backup_blk_id));
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_blk_hdr_get(unit, backup_blk_id,
                                    &backup_blk_hdr));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, backup_blk_id,
                                 &backup_ha_mem));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
        blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_B_TO_A;
        sal_memcpy(ha_mem, backup_ha_mem, blk_hdr->blk_size);
        backup_blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    }
    trans_state_dynamic[unit] = BCMFP_TRANS_STATE_IDLE;
    /* This is required because _func_rv could be set to
     * SHR_E_NOT_FOUND in the abouve loop which is OK.
     */
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_trans_idle_check(int unit, bcmfp_stage_id_t stage_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    if (trans_state_fixed[unit] == BCMFP_TRANS_STATE_IDLE &&
        trans_state_dynamic[unit] == BCMFP_TRANS_STATE_IDLE) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr));
    if (blk_hdr->trans_state == BCMFP_TRANS_STATE_UC_A ||
        trans_state_dynamic[unit] == BCMFP_TRANS_STATE_UC_A) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_trans_cmd(int unit,
                bcmfp_stage_id_t stage_id,
                bcmfp_trans_cmd_t trans_cmd)
{
    SHR_FUNC_ENTER(unit);

    switch (trans_cmd) {
        case BCMFP_TRANS_CMD_IDLE_CHECK:
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_trans_idle_check(unit, stage_id));
            break;
        case BCMFP_TRANS_CMD_COMMIT:
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_trans_commit(unit, stage_id));
            break;
        case BCMFP_TRANS_CMD_ABORT:
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_trans_abort(unit, stage_id));
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_trans_state_set(int unit, uint8_t blk_id,
                      bcmfp_trans_state_t trans_state)
{
    void *ha_mem = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
    blk_hdr->trans_state = trans_state;

    if (BCMFP_HA_BLK_ID_IS_FIXED(blk_id) &&
        (trans_state == BCMFP_TRANS_STATE_UC_A)) {
        trans_state_fixed[unit] = trans_state;
    }
    if (BCMFP_HA_BLK_ID_IS_DYNAMIC(blk_id) &&
        (trans_state == BCMFP_TRANS_STATE_UC_A)) {
        trans_state_dynamic[unit] = trans_state;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ing_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Insert the updated operational status of entry into IMM */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_insert(unit,
                                           BCMFP_STAGE_ID_INGRESS));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_clear(unit,
                                          BCMFP_STAGE_ID_INGRESS));

    _func_rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_INGRESS,
                               BCMFP_TRANS_CMD_IDLE_CHECK);
    SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_FAIL);
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_trans_cmd(unit,
                        BCMFP_STAGE_ID_INGRESS,
                        BCMFP_TRANS_CMD_COMMIT));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_ing_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{
    shr_error_t rv = SHR_E_NONE;

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_INGRESS,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_INGRESS,
                              BCMFP_TRANS_CMD_ABORT);
    }
}

int
bcmfp_egr_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Insert the updated operational status of entry into IMM */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_insert(unit,
                                           BCMFP_STAGE_ID_EGRESS));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_clear(unit,
                                          BCMFP_STAGE_ID_EGRESS));

    _func_rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EGRESS,
                               BCMFP_TRANS_CMD_IDLE_CHECK);
    SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_FAIL);
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_trans_cmd(unit,
                        BCMFP_STAGE_ID_EGRESS,
                        BCMFP_TRANS_CMD_COMMIT));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_egr_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{
    shr_error_t rv = SHR_E_NONE;

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EGRESS,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_EGRESS,
                              BCMFP_TRANS_CMD_ABORT);
    }

}

int
bcmfp_vlan_trans_commit(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Insert the updated operational status of entry into IMM */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_insert(unit,
                                           BCMFP_STAGE_ID_LOOKUP));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_clear(unit,
                                          BCMFP_STAGE_ID_LOOKUP));

    _func_rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_LOOKUP,
                               BCMFP_TRANS_CMD_IDLE_CHECK);
    SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_FAIL);
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_trans_cmd(unit,
                        BCMFP_STAGE_ID_LOOKUP,
                        BCMFP_TRANS_CMD_COMMIT));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_vlan_trans_abort(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    shr_error_t rv = SHR_E_NONE;

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_LOOKUP,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_LOOKUP,
                              BCMFP_TRANS_CMD_ABORT);
    }

}

int
bcmfp_em_trans_commit(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Insert the updated operational status of entry into IMM */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_insert(unit,
                                           BCMFP_STAGE_ID_EXACT_MATCH));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_lt_entry_oprtnl_state_clear(unit,
                                          BCMFP_STAGE_ID_EXACT_MATCH));

    _func_rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EXACT_MATCH,
                               BCMFP_TRANS_CMD_IDLE_CHECK);
    SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_FAIL);
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_trans_cmd(unit,
                        BCMFP_STAGE_ID_EXACT_MATCH,
                        BCMFP_TRANS_CMD_COMMIT));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_em_trans_abort(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     void *context)
{
    shr_error_t rv = SHR_E_NONE;

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EXACT_MATCH,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_EXACT_MATCH,
                              BCMFP_TRANS_CMD_ABORT);
    }

}
