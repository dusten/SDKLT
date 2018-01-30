/*! \file bcmfp_profile.c
 *
 * Infrastructure APIs to manage profile tables.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

static uint32_t req_ew[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t rsp_ew[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

int
bcmfp_profile_entry_lookup(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           uint32_t *profile_data,
                           int index)
{
     uint32_t req_flags = 0;
     uint32_t rsp_flags = 0;
     bcmltd_sid_t rsp_ltid;
     size_t rsp_entry_wsize = 0;
     bcmbd_pt_dyn_info_t pt_dyn_info;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(profile_data, SHR_E_PARAM);

     pt_dyn_info.index = index;
     pt_dyn_info.tbl_inst = tbl_inst;

     rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

     sal_memset(rsp_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
     SHR_IF_ERR_EXIT(
         bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    if (sal_memcmp(profile_data, rsp_ew[unit],
            rsp_entry_wsize * sizeof(uint32_t))) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_profile_entry_insert(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           uint32_t *profile_data,
                           int index)
{
     uint32_t req_flags = 0;
     uint32_t rsp_flags = 0;
     bcmltd_sid_t rsp_ltid;
     size_t rsp_entry_wsize = 0;
     bcmbd_pt_dyn_info_t pt_dyn_info;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(profile_data, SHR_E_PARAM);

     pt_dyn_info.index = index;
     pt_dyn_info.tbl_inst = tbl_inst;

     rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

     sal_memset(rsp_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
     SHR_IF_ERR_EXIT(
         bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    profile_data,
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_EXIT((req_ltid != rsp_ltid) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_profile_entry_delete(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           int index)
{
     uint32_t req_flags = 0;
     uint32_t rsp_flags = 0;
     bcmltd_sid_t rsp_ltid;
     size_t rsp_entry_wsize = 0;
     bcmbd_pt_dyn_info_t pt_dyn_info;

     SHR_FUNC_ENTER(unit);

     pt_dyn_info.index = index;
     pt_dyn_info.tbl_inst = tbl_inst;

     rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

     sal_memset(req_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
     sal_memset(rsp_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
     SHR_IF_ERR_EXIT(
         bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    req_ew[unit],
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_EXIT((req_ltid != rsp_ltid) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_profile_index_alloc(int unit,
                          uint32_t trans_id,
                          int tbl_inst,
                          bcmltd_sid_t req_ltid,
                          uint8_t num_profiles,
                          bcmdrd_sid_t *profile_sids,
                          uint32_t *profile_data[],
                          uint32_t *profile_ref_count,
                          int *index)
{
     int idx = 0;
     int min_idx = 0;
     int max_idx = 0;
     int free_idx = 0;
     uint8_t profile = 0;
     shr_error_t rv = SHR_E_NONE;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(index, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_data, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_ref_count, SHR_E_PARAM);

     min_idx = bcmdrd_pt_index_min(unit, profile_sids[0]);
     max_idx = bcmdrd_pt_index_max(unit, profile_sids[0]);
     for (idx = min_idx; idx <= max_idx; idx++) {

         if (!profile_ref_count[idx]) {
             free_idx = idx;
             continue;
         }

         rv = bcmfp_profile_entry_lookup(unit,
                                         trans_id,
                                         tbl_inst,
                                         req_ltid,
                                         profile_sids[0],
                                         profile_data[0],
                                         idx);
         SHR_IF_ERR_EXIT_EXCEPT_IF(rv , SHR_E_NOT_FOUND);
         if (SHR_FAILURE(rv)) {
             continue;
         }

         for (profile = 1; profile < num_profiles; profile++) {
             rv = bcmfp_profile_entry_lookup(unit,
                                             trans_id,
                                             tbl_inst,
                                             req_ltid,
                                             profile_sids[profile],
                                             profile_data[profile],
                                             idx);
             if (SHR_FAILURE(rv)) {
                 break;
             }
         }

         if (profile == num_profiles && SHR_SUCCESS(rv)) {
             *index = idx;
             SHR_RETURN_VAL_EXIT(SHR_E_NONE);
         }
     }

     SHR_IF_ERR_EXIT((free_idx == -1) ? SHR_E_RESOURCE : SHR_E_NONE);
     *index = free_idx;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_profile_add(int unit,
                  uint32_t trans_id,
                  int tbl_inst,
                  bcmltd_sid_t req_ltid,
                  uint8_t num_profiles,
                  bcmdrd_sid_t *profile_sids,
                  uint32_t *profile_data[],
                  uint32_t *profile_ref_count,
                  int *profile_index)
{
    int index = 0;
    uint8_t profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_data, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_index, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_profile_index_alloc(unit,
                                  trans_id,
                                  tbl_inst,
                                  req_ltid,
                                  num_profiles,
                                  profile_sids,
                                  profile_data,
                                  profile_ref_count,
                                  &index));

    if (profile_ref_count[index]) {
        profile_ref_count[index] += 1;
        *profile_index = index;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (profile = 0; profile < num_profiles; profile++) {
        SHR_IF_ERR_EXIT(
            bcmfp_profile_entry_insert(unit,
                                       trans_id,
                                       tbl_inst,
                                       req_ltid,
                                       profile_sids[profile],
                                       profile_data[profile],
                                       index));
    }

    profile_ref_count[index] += 1;
    *profile_index = index;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_profile_delete(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     uint8_t num_profiles,
                     bcmltd_sid_t *profile_sids,
                     uint32_t *profile_ref_count,
                     int profile_index)
{
    uint8_t profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_EXIT((profile_index == -1) ? SHR_E_PARAM : SHR_E_NONE);

    if (!profile_ref_count[profile_index]) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    profile_ref_count[profile_index] -= 1;

    for (profile = 0; profile < num_profiles; profile++) {
        SHR_IF_ERR_EXIT(
            bcmfp_profile_entry_delete(unit,
                                       trans_id,
                                       tbl_inst,
                                       req_ltid,
                                       profile_sids[profile],
                                       profile_index));
    }

exit:
    SHR_FUNC_EXIT();
}

