/*! \file bcm56960_a0_fp_ing_selcodes.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy the IFP group for
 * Tomahawk device(56960_A0).
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56960_a0_fp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

static uint32_t kgp_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t kgp2_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

static bcmdrd_sid_t ing_profile_sids[] = {
                                    IFP_KEY_GEN_PROGRAM_PROFILEm,
                                    IFP_KEY_GEN_PROGRAM_PROFILE2m };

static const bcmdrd_fid_t pmux_sel[] = /* Post mux selectors. */
    {
        POST_EXTRACTOR_MUX_0_SELf,
        POST_EXTRACTOR_MUX_1_SELf,
        POST_EXTRACTOR_MUX_2_SELf,
        POST_EXTRACTOR_MUX_3_SELf,
        POST_EXTRACTOR_MUX_4_SELf,
        POST_EXTRACTOR_MUX_5_SELf,
        POST_EXTRACTOR_MUX_6_SELf,
        POST_EXTRACTOR_MUX_7_SELf,
        POST_EXTRACTOR_MUX_8_SELf,
        POST_EXTRACTOR_MUX_9_SELf,
        POST_EXTRACTOR_MUX_10_SELf,
        POST_EXTRACTOR_MUX_11_SELf,
        POST_EXTRACTOR_MUX_12_SELf,
        POST_EXTRACTOR_MUX_13_SELf,
        POST_EXTRACTOR_MUX_14_SELf
    };

int
bcmfp_bcm56960_a0_ingress_group_install(int unit,
                                        uint32_t trans_id,
                                        bcmfp_group_t *fg)
{
    int index = 0;
    uint8_t idx = 0;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint32_t value = 0;
    uint32_t *prof_entry = NULL;
    uint32_t *kgp_ref_count = NULL;
    bcmdrd_fid_t fid;
    bcmdrd_sid_t kgp_sid = ing_profile_sids[0];
    bcmdrd_sid_t kgp2_sid = ing_profile_sids[1];
    uint32_t *profile_data[] = { kgp_entry[unit], kgp2_entry[unit] };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_group_parts_count(unit, fg->flags, &num_parts));

    for (part = 0; part < num_parts; part++) {
        prof_entry = kgp_entry[unit];
        bcmfp_bcm56960_a0_ext_codes_init(unit, fg, part, kgp_sid, prof_entry);

        prof_entry = kgp2_entry[unit];
        sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

        /* Set Post MUX extractor selcode values. */
        for (idx = 0; idx < 15; idx++) {
            if (-1 != fg->ext_codes[part].pmux_sel[idx]) {
                value = fg->ext_codes[part].pmux_sel[idx];
                fid = pmux_sel[idx];
                bcmdrd_pt_field_set(unit, kgp2_sid, prof_entry, fid, &value);
            }
        }

        index = -1;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_prof_ref_count_get(unit,
                                  fg->stage_id, fg->pipe_id, &kgp_ref_count));
        SHR_IF_ERR_EXIT(
            bcmfp_profile_add(unit,
                              trans_id,
                              fg->pipe_id,
                              fg->ltid,
                              2,
                              ing_profile_sids,
                              profile_data,
                              kgp_ref_count,
                              &index));
        fg->ext_codes[part].keygen_index = (uint32_t)index;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56960_a0_ingress_group_uninstall(int unit, uint32_t trans_id,
                                          bcmfp_group_id_t group_id)
{
    int index = -1;
    int tbl_inst = -1;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint32_t *kgp_ref_count = NULL;
    bcmltd_sid_t req_ltid = FP_ING_ENTRYt;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_INGRESS;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  group_id,
                                  &group_oper_info));

    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit,
                                group_oper_info->flags,
                                &num_parts));

    tbl_inst = group_oper_info->tbl_inst;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_prof_ref_count_get(unit,
                                        stage_id,
                                        tbl_inst,
                                        &kgp_ref_count));
    for (part = 0; part < num_parts; part++) {
        index = (int)group_oper_info->ext_codes[part].keygen_index;
        SHR_IF_ERR_EXIT(
            bcmfp_profile_delete(unit,
                                 trans_id,
                                 tbl_inst,
                                 req_ltid,
                                 2,
                                 ing_profile_sids,
                                 kgp_ref_count,
                                 index));
    }

exit:
    SHR_FUNC_EXIT();
}
