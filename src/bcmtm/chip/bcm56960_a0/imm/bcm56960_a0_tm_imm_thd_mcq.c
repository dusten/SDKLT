/*! \file bcm56960_a0_tm_imm_thd_mcq.c
 *
 * TM multicast queue threshold configuration for 56960_a0.
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

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include "bcm56960_a0_tm_imm_thd_defines.h"

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcm56960_a0_tm_mcq_reset(int unit,
        bcmltd_sid_t ltid,
        const bcmltd_field_t *key)
{
    bcmdrd_sid_t sid;
    bcmdrd_sid_t sid_resume[] = {MMU_THDM_DB_QUEUE_RESUME_MMU_XPE0_EPIPE0m,
            MMU_THDM_DB_QUEUE_RESUME_MMU_XPE0_EPIPE1m,
            MMU_THDM_DB_QUEUE_RESUME_MMU_XPE1_EPIPE2m,
            MMU_THDM_DB_QUEUE_RESUME_MMU_XPE1_EPIPE3m};
    bcmltd_fid_t ltfid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint64_t mcq_id;
    uint64_t lport;
    uint32_t ltmbuf_arr[3] = { 0 }, fval = 0;
    int qbase, pipe, index;

    SHR_FUNC_ENTER(unit);

    /* TM_THD_MC_Qt_PORT_IDf */
    ltfid = TM_THD_MC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, key, (uint64_t *)&lport));

    /* TM_THD_MC_Qt_TM_THD_MC_Q_IDf */
    ltfid = TM_THD_MC_Qt_TM_THD_MC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, key, &mcq_id));

    if ((SHR_FAILURE(bcmtm_port_mc_cosq_base_get(unit, lport, &qbase))) ||
            (SHR_FAILURE(bcmtm_port_pipe_get(unit, lport, &pipe)))) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    index = qbase + mcq_id;
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);

    /* MMU_THDM_DB_QUEUE_CONFIGm */
    sid = MMU_THDM_DB_QUEUE_CONFIGm;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid,
                               (void *)&pt_dyn_info, ltmbuf_arr));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, Q_SHARED_LIMITf, ltmbuf_arr, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, Q_MIN_LIMITf, ltmbuf_arr, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, Q_LIMIT_DYNAMICf, ltmbuf_arr, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, Q_COLOR_LIMIT_DYNAMICf, ltmbuf_arr, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, RED_SHARED_LIMITf, ltmbuf_arr, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, YELLOW_SHARED_LIMITf, ltmbuf_arr, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,
                                (void *)&pt_dyn_info, ltmbuf_arr));
    /* MMU_THDM_DB_QUEUE_OFFSETm */
    sal_memset(ltmbuf_arr, 0, sizeof(uint32_t) * 3);
    sid = MMU_THDM_DB_QUEUE_OFFSETm;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,
                                (void *)&pt_dyn_info, ltmbuf_arr));
    /* MMU_THDM_DB_QUEUE_RESUMEm */
    sid = sid_resume[pipe];
    BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid,
                                (void *)&pt_dyn_info, ltmbuf_arr));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] output_fields This is linked list of extra added data fields
 * (mostly read only fields.)
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcm56960_a0_tm_mcq_set(int unit,
        bcmltd_sid_t ltid,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        bcmltd_fields_t *output_fields)
{
    bcmdrd_sid_t sid;
    bcmdrd_sid_t sid_resume[] = {MMU_THDM_DB_QUEUE_RESUME_MMU_XPE0_EPIPE0m,
            MMU_THDM_DB_QUEUE_RESUME_MMU_XPE0_EPIPE1m,
            MMU_THDM_DB_QUEUE_RESUME_MMU_XPE1_EPIPE2m,
            MMU_THDM_DB_QUEUE_RESUME_MMU_XPE1_EPIPE3m};
    bcmdrd_fid_t fid;
    bcmltd_fid_t ltfid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint64_t mcq_id;
    uint64_t lport;
    uint32_t ltmbuf_arr[3] = {0}, fval;
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    int qbase, pipe = 0, opcode = 1, index;

    SHR_FUNC_ENTER(unit);

    /* TM_THD_MC_Qt_PORT_IDf */
    ltfid = TM_THD_MC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, key, (uint64_t *)&lport));

    /* TM_THD_MC_Qt_TM_THD_MC_Q_IDf */
    ltfid = TM_THD_MC_Qt_TM_THD_MC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, key, &mcq_id));

    if ((SHR_FAILURE(bcmtm_port_mc_cosq_base_get(unit, lport, &qbase))) ||
        (SHR_FAILURE(bcmtm_port_pipe_get(unit, lport, &pipe)))) {
        goto op_stage;
    }
    index = qbase + mcq_id;

    while(cur_data) {
        fval = cur_data->data;
        switch(cur_data->id) {
            case TM_THD_MC_Qt_MIN_GUARANTEE_CELLSf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = Q_MIN_LIMITf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_USE_DYNAMIC_SHARED_LIMITSf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = Q_LIMIT_DYNAMICf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_SHARED_LIMIT_CELLS_STATICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = Q_SHARED_LIMITf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_SHARED_LIMIT_DYNAMICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = Q_SHARED_ALPHAf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_USE_COLOR_SPECIFIC_DYNAMIC_LIMITSf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = Q_COLOR_LIMIT_DYNAMICf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_YELLOW_LIMIT_CELLS_STATICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = YELLOW_SHARED_LIMITf;
                fval = fval / 8;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_YELLOW_LIMIT_DYNAMICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = YELLOW_SHARED_LIMITf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_RED_LIMIT_CELLS_STATICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = RED_SHARED_LIMITf;
                fval = fval / 8;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_RED_LIMIT_DYNAMICf:
                sid = MMU_THDM_DB_QUEUE_CONFIGm;
                fid = RED_SHARED_LIMITf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_RESET_OFFSET_CELLSf:
                sid = MMU_THDM_DB_QUEUE_OFFSETm;
                fid = RESUME_OFFSETf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_RESET_FLOOR_CELLSf:
                sid = sid_resume[pipe];
                fid = QDRP_RESETf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
                break;
            case TM_THD_MC_Qt_RESET_OFFSET_YELLOW_CELLSf:
                sid = MMU_THDM_DB_QUEUE_OFFSETm;
                fid = YELLOW_RESUME_OFFSET_PROFILE_SELf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_RESET_FLOOR_YELLOW_CELLSf:
                sid = sid_resume[pipe];
                fid = QYEL_RESETf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
                break;
            case TM_THD_MC_Qt_RESET_OFFSET_RED_CELLSf:
                sid = MMU_THDM_DB_QUEUE_OFFSETm;
                fid = RED_RESUME_OFFSET_PROFILE_SELf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, pipe);
                break;
            case TM_THD_MC_Qt_RESET_FLOOR_RED_CELLSf:
                sid = sid_resume[pipe];
                fid = QRED_RESETf;
                BCMTM_PT_DYN_INFO(pt_dyn_info, index, 0);
                break;
            default:
                sid = fid = 0;
                break;
        }
        if (sid && fid ) {
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid, ltid,
                                       (void *)&pt_dyn_info, ltmbuf_arr));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid,  ltmbuf_arr, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, sid, ltid,
                                        (void *)&pt_dyn_info, ltmbuf_arr));
        }
        cur_data = cur_data->next;
    }
    opcode = 0;
op_stage:
    ltfid = TM_THD_MC_Qt_OPERATIONAL_STATEf;
    bcmtm_field_populate(unit, output_fields, ltfid, 0, opcode);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_THD_ING_HEADROOM_POOL callback function for staging.
 *
 * Handle TM_THD_ING_HEADROOM_POOL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcm56960_a0_tm_mcq_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    switch(event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            SHR_IF_ERR_EXIT
                (bcm56960_a0_tm_mcq_set(unit, ltid, key,
                                                  data, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcm56960_a0_tm_mcq_reset(unit, ltid, key));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */
int
bcm56960_a0_tm_mcq_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_THD_MC_Qt . */
    static bcmimm_lt_cb_t bcmtm_mcq_imm_cb = {
        /*! Staging function. */
        .stage = bcm56960_a0_tm_mcq_stage,
    };
    int rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    /*! Registers callback functions for multicast queue thresholds. */
    rv = bcmlrd_map_get(unit, TM_THD_MC_Qt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                            TM_THD_MC_Qt,
                                            &bcmtm_mcq_imm_cb, NULL));
    }
exit:
    SHR_FUNC_EXIT();
}
