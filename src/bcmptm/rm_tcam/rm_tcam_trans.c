/*! \file rm_tcam_trans.c
 *
 * Low Level Functions for TCAM resource manager.
 * This file contains low level functions used for handling
 * transactions in resource manager
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_fp.h"
#include "rm_tcam_fp_entry_mgmt.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <shr/shr_ha.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Public Functions
 */
/*
 * Get the pointer to tcam information of given TCAM LTID.
 */
int
bcmptm_rm_tcam_backup_info_get(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info, void **tcam_info)
{
    uint32_t lt_state_offset = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(tcam_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_lt_state_offset_read(unit, ltid,
                                                &lt_state_offset));

    *tcam_info = (uint8_t*)RM_TCAM_BACKUP_PTR(unit) + lt_state_offset;

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_um_trans_complete(int unit,
                          bcmptm_rm_tcam_trans_info_t *trans_info_global)
{
    bcmltd_sid_t ltid;
    int tmp_rv;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    uint8_t *ltid_start_ptr = NULL;
    uint8_t *ltid_bkp_start_ptr = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    int rsp_index_hw_entry_info;
    uint16_t *rsp_hw_data_type_val;

    SHR_FUNC_ENTER(unit);

    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        tmp_rv = SHR_E_NONE;
        sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        tmp_rv = bcmptm_cmdproc_ptrm_info_get(unit, ltid, 0, 0,
                                                 &ltid_info,
                                                 &rsp_index_hw_entry_info,
                                                 &rsp_hw_data_type_val);
        if (tmp_rv == SHR_E_NONE) {

            switch (ltid_info.pt_type) {
                case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
                case LT_PT_TYPE_TCAM_PRI_ONLY:
                case LT_PT_TYPE_FP:
                    /* Fetch the ltid start pointer */
                    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_info_get(unit,
                                       ltid, &ltid_info,
                                       (void *)&ltid_start_ptr));
                    trans_info = (bcmptm_rm_tcam_trans_info_t *) ltid_start_ptr;
                    if (trans_info->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                        continue;
                    }
                    if (trans_info->trans_state == BCMPTM_RM_TCAM_STATE_UC_A) {
                        /* Fetch the ltid backup start pointer */
                        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_backup_info_get(unit,
                                       ltid, &ltid_info,
                                       (void *)&ltid_bkp_start_ptr));
                        if (trans_info_global->trans_state
                                  == BCMPTM_RM_TCAM_STATE_COPY_A_TO_B) {

                            sal_memcpy(ltid_bkp_start_ptr, ltid_start_ptr,
                                          trans_info->ltid_size);
                        } else {
                            sal_memcpy(ltid_start_ptr, ltid_bkp_start_ptr,
                                          trans_info->ltid_size);
                        }
                        /* change the transaction state to Idle */
                        trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
                        /* change the transaction state to Idle in back up too*/
                        trans_info =
                             (bcmptm_rm_tcam_trans_info_t *) ltid_bkp_start_ptr;
                        trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
                    }
                    break;
                default:
                    break;
            }
        } /* else {
             tmp_rv = SHR_E_UNAVAIL which is ok
             Not having rsrc_mgr_data_struct => it is not managed by PTRM
        } */
    }
    /* update the transaction state to Idle in active DB */
    trans_info_global->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;

    /* update the transaction state to Idle in backup DB too */
    trans_info_global = (bcmptm_rm_tcam_trans_info_t *)RM_TCAM_BACKUP_PTR(unit);
    trans_info_global->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;

exit:
        SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_trans_cmd(int unit, uint32_t flags, uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd) 
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_trans_info_t *trans_info_global = NULL;

    SHR_FUNC_ENTER(unit);

    /* Rsrc_mgr starts in IDLE state.
     * In IDLE state, ptrm_state_cseq_id is dont_care.
     * - if it gets commit()
     *          it stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets abort()
     *          stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets bcmptm_rm_hash_req()
     *          goes into CHANGED state and
     *          remembers cseq_id as ptrm_state_cseq_id
     *
     * - if it gets idle_check:
     *          stays in IDLE state and returns SHR_E_NONE.
     *          cseq_id should be ignored
     *
     * In CHANGED state:
     * - if it gets bcmptm_rm_hash_req()
     *          Must declare SHR_E_FAIL if cseq_id does not match ptrm_state_cseq_id
     *          Remain in CHANGED state.
     *
     * - if it gets commit()
     *          if cseq_id != ptrm_state_cseq_id, ignore commit.
     *          Else, it commits the changes to sw_state and goes to IDLE
     *
     * - if it gets abort()
     *          if cseq_id != ptrm_state_cseq_id, declare SHR_E_FAIL
     *          Else, it aborts the changes to sw_state and goes to IDLE
     *
     * - if it gets idle_check()
     *          it stays in CHANGED state and returns SHR_E_FAIL
     *          (cseq_id should be ignored)
     *
     * PTRM_state_cseq_id must be in HA space - just like rest of PTRM SW_state
 */

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Commit \n")));
            break;
        case BCMPTM_TRANS_CMD_ABORT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Abort \n")));
            break;
        case BCMPTM_TRANS_CMD_IDLE_CHECK:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Idle_Check \n")));
            trans_info_global = (bcmptm_rm_tcam_trans_info_t *)RM_TCAM_PTR(unit);
            SHR_NULL_CHECK(trans_info_global, SHR_E_FAIL);
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            } else {
                rv = SHR_E_FAIL;
            }
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Unknown trans_cmd \n")));
            rv = SHR_E_INTERNAL;
            break;
    }
    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_um_trans_cmd(int unit, uint32_t flags, uint32_t cseq_id,
                            bcmptm_trans_cmd_t trans_cmd) 
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_trans_info_t *trans_info_global = NULL;

    SHR_FUNC_ENTER(unit);

    /* Rsrc_mgr starts in IDLE state.
     * In IDLE state, ptrm_state_cseq_id is dont_care.
     * - if it gets commit()
     *          it stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets abort()
     *          stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets bcmptm_rm_hash_req()
     *          goes into CHANGED state and
     *          remembers cseq_id as ptrm_state_cseq_id
     *
     * - if it gets idle_check:
     *          stays in IDLE state and returns SHR_E_NONE.
     *          cseq_id should be ignored
     *
     * In CHANGED state:
     * - if it gets bcmptm_rm_hash_req()
     *          Must declare SHR_E_FAIL if cseq_id does not match ptrm_state_cseq_id
     *          Remain in CHANGED state.
     *
     * - if it gets commit()
     *          if cseq_id != ptrm_state_cseq_id, ignore commit.
     *          Else, it commits the changes to sw_state and goes to IDLE
     *
     * - if it gets abort()
     *          if cseq_id != ptrm_state_cseq_id, declare SHR_E_FAIL
     *          Else, it aborts the changes to sw_state and goes to IDLE
     *
     * - if it gets idle_check()
     *          it stays in CHANGED state and returns SHR_E_FAIL
     *          (cseq_id should be ignored)
     *
     * PTRM_state_cseq_id must be in HA space - just like rest of PTRM SW_state
 */


    trans_info_global = (bcmptm_rm_tcam_trans_info_t *)RM_TCAM_PTR(unit);
    SHR_NULL_CHECK(trans_info_global, SHR_E_FAIL);

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Commit \n")));
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            }
            else if
                (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_UC_A ||
                 trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_COPY_A_TO_B) {
                 /* change the global transaction state
                  * to copying A to B
                  */
                 trans_info_global->trans_state = BCMPTM_RM_TCAM_STATE_COPY_A_TO_B;
                 /* call function to copy Active to Backup */
                 rv = bcmptm_rm_tcam_um_trans_complete(unit, trans_info_global);
            } else {
                /* control should not come here */
                rv = SHR_E_INTERNAL;
            }
            break;
        case BCMPTM_TRANS_CMD_ABORT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Abort \n")));
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            }
            else if
                (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_UC_A ||
                 trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_COPY_B_TO_A) {
                 /* change the global transaction state
                  * to copying B to A
                  */
                 trans_info_global->trans_state = BCMPTM_RM_TCAM_STATE_COPY_B_TO_A;
                 /* call function to copy Backup to Active */
                 rv = bcmptm_rm_tcam_um_trans_complete(unit, trans_info_global);
            } else {
                /* control should not come here */
                rv = SHR_E_INTERNAL;
            }
            break;
        case BCMPTM_TRANS_CMD_IDLE_CHECK:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Idle_Check \n")));
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            } else {
                rv = SHR_E_FAIL;
            }
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Unknown trans_cmd \n")));
            rv = SHR_E_INTERNAL;
            break;
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}
