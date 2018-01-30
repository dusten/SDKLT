/*! \file bcmfp_lt_src_class_mode.c
 *
 * APIs for LT Source Class Mode entry
 *
 * This file contains function definitions for Source Class Mode entry LT.
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
  Includes
 */
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_lt_util.h>
#include <bcmfp/bcmfp_lt_inmem.h>
#include <bcmfp/bcmfp_lt_src_class_mode.h>
#include <bcmfp/map/bcmfp_lt_src_class_mode_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_lt_src_class_mode_insert(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t sid,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data)
{
    int pipe_id = -1;
    uint32_t fid = 0;
    uint32_t mode = 0;
    uint64_t data_val = 0;
    uint64_t key_val = 0;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_stage_id_t stage_id;
    bcmfp_lt_src_class_mode_sp_fids_t *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    fid_list = (bcmfp_lt_src_class_mode_sp_fids_t *) lt_info->sp_fids;
  /*Get the key value from the key array */
    fid = fid_list->pipe_id_fid;
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_key_get_from_key_array(unit, fid, key, &key_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(key_val, pipe_id);
    }

    fid = fid_list->src_class_mode_fid;
    SHR_IF_ERR_EXIT
        (bcmfp_lt_field_data_get_from_data_array(unit, fid, data, &data_val));
    BCMFP_UINT64_TO_UINT32(data_val, mode);
    stage_id = lt_info->stage_id;
    SHR_IF_ERR_EXIT
        (bcmfp_src_class_mode_set(unit, trans_id, stage_id, sid,
                                  pipe_id, mode));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}

int
bcmfp_lt_src_class_mode_delete(int unit,
                               uint32_t trans_id,
                               bcmltd_sid_t sid,
                               const bcmltd_field_t *key)
{
    int pipe_id = -1;
    uint32_t fid = 0;
    uint64_t key_val = 0;
    uint32_t mode = 0;
    bcmfp_lt_info_t *lt_info = NULL;
    bcmfp_lt_src_class_mode_sp_fids_t *fid_list = NULL;
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_lt_info_get(unit, sid, &lt_info));
    fid_list = (bcmfp_lt_src_class_mode_sp_fids_t *) lt_info->sp_fids;
    stage_id = lt_info->stage_id;

    fid = fid_list->pipe_id_fid;
    BCMFP_RET_VAL_ASSIGN
        (bcmfp_lt_field_key_get_from_key_array(unit, fid, key, &key_val));
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        BCMFP_UINT64_TO_UINT32(key_val, pipe_id);
    }

    /* Update the SRC_CLASS_MODE to its default value. */
    SHR_IF_ERR_EXIT
        (bcmfp_src_class_mode_set(unit, trans_id, stage_id, sid,
                                  pipe_id, mode));
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:

    SHR_FUNC_EXIT();
}


