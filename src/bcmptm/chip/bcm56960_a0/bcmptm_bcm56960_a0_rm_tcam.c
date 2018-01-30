/*! \file bcmptm_bcm56960_a0_rm_tcam.c
 *
 * Miscellaneous utilities for PTRM
 * This file contains the misc utils for PTRM
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
#include <bcmptm/bcmptm_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include "./bcmptm_bcm56960_a0_rm_tcam.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC


/*******************************************************************************
 * Private variables
 */
/* List of PTs which are directly configured by PTRM */
static const bcmdrd_sid_t bcm56960_a0_list[BCMPTM_RM_TCAM_SID_COUNT] = {
    IFP_LOGICAL_TABLE_ACTION_PRIORITYm,
    IFP_LOGICAL_TABLE_CONFIGr,
    IFP_CONFIGr,
    EFP_SLICE_MAPr,
    EFP_SLICE_CONTROLr,
    EFP_CLASSID_SELECTORr,
    EFP_KEY4_DVP_SELECTORr,
    EFP_KEY8_DVP_SELECTORr,
    EFP_KEY4_MDL_SELECTORr,
    VFP_SLICE_MAPr,
    VFP_SLICE_CONTROLr,
    VFP_KEY_CONTROL_1r,
    VFP_KEY_CONTROL_2r
};


/*******************************************************************************
 * Public Functions
 */
int
bcm56960_a0_rm_tcam_memreg_update(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_t sid_enum,
                              bcmptm_rm_tcam_sid_info_t *sid_info)
{
    bcmdrd_sid_t sid;
    uint32_t entry_buffer[BCMPTM_MAX_PT_FIELD_WORDS] = { 0 };
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t rsp_flags = 0;
    bool rsp_entry_cache_vbit= 0;
    uint16_t rsp_dfield_format_id= 0;
    bcmltd_sid_t rsp_ltid;
    uint8_t field_idx, start_bit, width, vs_offset;
    uint32_t field_val;
    bcmptm_rm_tcam_fp_group_selcode_t   *selcode = NULL;
    uint8_t slice_idx, pri_slice_idx = 0;
    uint32_t data = 0;
    uint32_t data_arr[2] = {0,1};
    int slice_en = 0;
    int slice_lookup_en = 1;
    int part = 0;
    bcmptm_rm_slice_change_t slice_mode_change;
    bcmptm_rm_op_t op_type = BCMPTM_RM_OP_NORMAL;
    bcmptm_rm_slice_change_t *slice_mode_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(sid_info, SHR_E_PARAM);

    sid = bcm56960_a0_list[sid_enum];
    dyn_info.tbl_inst = sid_info->tbl_inst;

    if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_ACTION_PRIO_UPDATE) {
        start_bit = 0;
        width =5;
        /* Modify the contents of the sid. */
        for (field_idx = 0;
             field_idx < sid_info->lt_action_pri_count;
             field_idx++) {
             /* update the action priority in entry buffer */
             field_val = sid_info->lt_action_pri[field_idx];
             bcmdrd_field_set(entry_buffer,
                             start_bit,
                             start_bit + width - 1,
                             &field_val);
             start_bit = start_bit + width;
        }
        /* Execute the Instruction. */
        dyn_info.index = 0;
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_PARTITION_PRIO_UPDATE) {

        /* Set lt partition map */
        bcmdrd_field_set(entry_buffer,
                    0,
                    12 - 1,
                    (uint32_t *)&(sid_info->lt_part_map));

        start_bit = 12;
        width =4;
        /* Modify the contents of the sid. */
        for (field_idx = 0;
             field_idx < sid_info->num_slices;
             field_idx++) {
             /* update the partition priority in entry buffer */
             field_val = sid_info->lt_part_pri[field_idx];
             bcmdrd_field_set(entry_buffer,
                    start_bit,
                    start_bit + width - 1,
                    &field_val);
             start_bit = start_bit + width;
        }
        /* Execute the Instruction. */
        dyn_info.index = sid_info->group_ltid[0];

    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SELCODE_INSTALL) {

        /* Execute the Instruction. */
        dyn_info.index = 0;

        /* Read the contents of the sid from h/w. */
        SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                            0,
                                            0, 
                                            sid,
                                            &dyn_info,
                                            NULL,
                                            NULL, /* misc_info */
                                            BCMPTM_MAX_PT_FIELD_WORDS,
                                            entry_buffer,
                                            &rsp_entry_cache_vbit,
                                            &rsp_ltid,
                                            &rsp_dfield_format_id,
                                            &rsp_flags));

        for (part = 0; part < sid_info->parts_count; part++) {
            slice_idx = sid_info->slice_id + (part >> sid_info->intraslice);
            selcode
             = (bcmptm_rm_tcam_fp_group_selcode_t *)((uint8_t *)sid_info->selcode
                + (sizeof(bcmptm_rm_tcam_fp_group_selcode_t) * part));

            if (sid_enum == BCMPTM_RM_TCAM_EFP_SLICE_CONTROL) {
                /* SLICE MODE */
                if (selcode->fpf2 >= 0) {
                    field_val = selcode->fpf2;
                    bcmdrd_field_set(entry_buffer,
                    (3 * slice_idx) + 8, (3 * slice_idx) + 10, &field_val);
                }
                /* IPV6 mode */
                if (selcode->egr_ip_addr_sel >= 0) {
                    field_val = selcode->egr_ip_addr_sel;
                    bcmdrd_field_set(entry_buffer,
                    (2 * slice_idx) + 20, (2 * slice_idx) + 21, &field_val);
                }
            } else if (sid_enum == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1) {
                /* F2  or DWF2*/
                if (selcode->fpf2 >= 0) {
                    field_val = selcode->fpf2;
                    if (sid_info->intraslice && (part % 2)) {
                        bcmdrd_field_set(entry_buffer,
                        (2 * slice_idx) + 34, (2 * slice_idx) + 35,
                        &field_val);
                    } else {
                        bcmdrd_field_set(entry_buffer,
                        (7 * slice_idx), (7 * slice_idx) + 3, &field_val);
                    }
                }
                /* F3 */
                if (selcode->fpf3 >= 0) {
                    field_val = selcode->fpf3;
                    bcmdrd_field_set(entry_buffer,
                    (7 * slice_idx) + 4, (7 * slice_idx) + 6, &field_val);
                }
                /* Double wide mode */
                if (sid_info->intraslice && (part % 2 == 0)) {
                    bcmdrd_field_set(entry_buffer,
                    slice_idx + 28, slice_idx + 28, &data_arr[1]);
                }
                /* Paring mode */
                if (sid_info->pairing && (part % 2 == 0)) {
                    bcmdrd_field_set(entry_buffer,
                    (slice_idx / 2)  + 32, (slice_idx / 2) + 32, &data_arr[1]);
                }
                /* Src Sel mode */
                if (selcode->src_sel >= 0) {
                    field_val = selcode->src_sel;
                    bcmdrd_field_set(entry_buffer,
                    (3 * slice_idx) + 42, (3 * slice_idx) + 44, &field_val);
                }
            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR) {
                /* KEY1 class id Sel */
                if (selcode->egr_key1_classid_sel >= 0) {
                    field_val = selcode->egr_key1_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                     2 * slice_idx, (2 * slice_idx) + 1, &field_val);
                }
                /* KEY2 class id Sel */
                if (selcode->egr_key2_classid_sel >= 0) {
                    field_val = selcode->egr_key2_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                    (3 * slice_idx) + 8, (3 * slice_idx) + 10, &field_val);
                }
                /* KEY3 class id Sel */
                if (selcode->egr_key3_classid_sel >= 0) {
                    field_val = selcode->egr_key3_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                    (2 * slice_idx) + 20, (2 * slice_idx) + 21, &field_val);
                }
                /* KEY4 class id Sel */
                if (selcode->egr_key4_classid_sel >= 0) {
                    field_val = selcode->egr_key4_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                    (2 * slice_idx) + 28, (2 * slice_idx) + 29, &field_val);
                }
                /* KEY6 class id Sel */
                if (selcode->egr_key6_classid_sel >= 0) {
                    field_val = selcode->egr_key6_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                    (2 * slice_idx) + 36, (2 * slice_idx) + 37, &field_val);
                }
                /* KEY7 class id Sel */
                if (selcode->egr_key7_classid_sel >= 0) {
                    field_val = selcode->egr_key7_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                    (3 * slice_idx) + 44, (3 * slice_idx) + 46, &field_val);
                }
                /* KEY8 class id Sel */
                if (selcode->egr_key8_classid_sel >= 0) {
                    field_val = selcode->egr_key8_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                    (2 * slice_idx) + 56, (2 * slice_idx) + 57, &field_val);
                }
            } else if (sid_enum == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2) {
                /* IP header Sel mode */
                if (selcode->ip_header_sel >= 0) {
                    field_val = selcode->ip_header_sel;
                    bcmdrd_field_set(entry_buffer, slice_idx,
                                     slice_idx, &field_val);
                }
            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR) {
                /* KEY4 DVP Sel */
                if (selcode->egr_key4_dvp_sel >= 0) {
                    field_val = selcode->egr_key4_dvp_sel;
                    bcmdrd_field_set(entry_buffer,
                    (2 * slice_idx), (2 * slice_idx) + 1, &field_val);
                }
            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR) {
                /* KEY8 DVP Sel */
                if (selcode->egr_key8_dvp_sel >= 0) {
                    field_val = selcode->egr_key8_dvp_sel;
                    bcmdrd_field_set(entry_buffer,
                    (2 * slice_idx), (2 * slice_idx) + 1, &field_val);
                }
            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR) {
                /* KEY4 MDL Sel */
                if (selcode->egr_key4_mdl_sel >= 0) {
                    field_val = selcode->egr_key4_mdl_sel;
                    bcmdrd_field_set(entry_buffer, slice_idx,
                                     slice_idx, &field_val);
                }
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SELCODE_RESET) {

        /* Execute the Instruction. */
        dyn_info.index = 0;

        /* Read the contents of the sid from h/w. */
        SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                            0,
                                            0, 
                                            sid,
                                            &dyn_info,
                                            NULL,
                                            NULL, /* misc_info */
                                            BCMPTM_MAX_PT_FIELD_WORDS,
                                            entry_buffer,
                                            &rsp_entry_cache_vbit,
                                            &rsp_ltid,
                                            &rsp_dfield_format_id,
                                            &rsp_flags));

        for (part = 0; part < sid_info->parts_count; part++) {
            slice_idx = sid_info->slice_id + (part >> sid_info->intraslice);
            if (sid_enum == BCMPTM_RM_TCAM_EFP_SLICE_CONTROL) {
                /* SLICE MODE */
                bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 8,
                                  (3 * slice_idx) + 10, &data);
                bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 20,
                                  (2 * slice_idx) + 21, &data);
            } else if (sid_enum == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1) {
                /* F2 */
                bcmdrd_field_set(entry_buffer, 7 * slice_idx,
                                 (7 * slice_idx) + 3, &data);
                /* DWF2*/
                bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 34,
                                  (2 * slice_idx) + 35, &data);
                /* F3 */
                bcmdrd_field_set(entry_buffer, (7 * slice_idx) + 4,
                                  (7 * slice_idx) + 6, &data);
                /* Double wide mode */
                bcmdrd_field_set(entry_buffer, slice_idx + 28,
                                 slice_idx + 28, &data);
                /* Paring mode */
                if (part % 2 == 0) {
                    bcmdrd_field_set(entry_buffer, (slice_idx / 2)  + 32,
                                  (slice_idx / 2) + 32, &data);
                }
                /* Src Sel mode */
                bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 42,
                                  (3 * slice_idx) + 44, &data);

            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR) {
                /* KEY1 class id Sel */
                bcmdrd_field_set(entry_buffer, 2 * slice_idx,
                                  (2 * slice_idx) + 1, &data);
                /* KEY2 class id Sel */
                bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 8,
                                  (3 * slice_idx) + 10, &data);
                /* KEY3 class id Sel */
                bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 20,
                                  (2 * slice_idx) + 21, &data);
                /* KEY4 class id Sel */
                bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 28,
                                  (2 * slice_idx) + 29, &data);
                /* KEY6 class id Sel */
                bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 36,
                                  (2 * slice_idx) + 37, &data);
                /* KEY7 class id Sel */
                bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 44,
                                  (3 * slice_idx) + 46, &data);
                /* KEY8 class id Sel */
                bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 56,
                                  (2 * slice_idx) + 57, &data);
            } else if (sid_enum == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2) {

                /* IP header Sel mode */
                bcmdrd_field_set(entry_buffer, slice_idx, slice_idx, &data);
            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR) {
                /* KEY4 DVP Sel */
                bcmdrd_field_set(entry_buffer, (2 * slice_idx),
                                      (2 * slice_idx) + 1, &data);
            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR) {
                /* KEY8 DVP Sel */
                bcmdrd_field_set(entry_buffer, (2 * slice_idx),
                                      (2 * slice_idx) + 1, &data);
            } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR) {
                /* KEY4 MDL Sel */
                bcmdrd_field_set(entry_buffer, slice_idx, slice_idx,
                                 &data);
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SELCODE_COPY) {

        /* Execute the Instruction. */
        dyn_info.index = 0;

        /* Read the contents of the sid from h/w. */
        SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                            0,
                                            0, 
                                            sid,
                                            &dyn_info,
                                            NULL,
                                            NULL, /* misc_info */
                                            BCMPTM_MAX_PT_FIELD_WORDS,
                                            entry_buffer,
                                            &rsp_entry_cache_vbit,
                                            &rsp_ltid,
                                            &rsp_dfield_format_id,
                                            &rsp_flags));

       for (part = 0; part < sid_info->parts_count; part++) {
                slice_idx = sid_info->slice_id + (part >> sid_info->intraslice);
                pri_slice_idx = sid_info-> pri_slice_id
                               + (part >> sid_info->intraslice);
                if (sid_enum == BCMPTM_RM_TCAM_EFP_SLICE_CONTROL) {
                    /* SLICE MODE */
                    bcmdrd_field_get(entry_buffer, (3 * pri_slice_idx) + 8,
                                      (3 * pri_slice_idx) + 10, &data);

                    bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 8,
                                      (3 * slice_idx) + 10, &data);
                    /* IPV6 mode */
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx) + 20,
                                      (2 * pri_slice_idx) + 21, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 20,
                                      (2 * slice_idx) + 21, &data);
                } else if (sid_enum == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1) {
                    /* F2 */
                    bcmdrd_field_get(entry_buffer, 7 * pri_slice_idx,
                                      (7 * pri_slice_idx) + 3, &data);
                    bcmdrd_field_set(entry_buffer, 7 * slice_idx,
                                      (7 * slice_idx) + 3, &data);
                    /* DWF2*/
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx) + 34,
                                      (2 * pri_slice_idx) + 35, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 34,
                                      (2 * slice_idx) + 35, &data);
                    /* F3 */
                    bcmdrd_field_get(entry_buffer, (7 * pri_slice_idx) + 4,
                                      (7 * pri_slice_idx) + 6, &data);
                    bcmdrd_field_set(entry_buffer, (7 * slice_idx) + 4,
                                      (7 * slice_idx) + 6, &data);
                    /* Double wide mode */
                    bcmdrd_field_get(entry_buffer, pri_slice_idx + 28,
                                     pri_slice_idx + 28, &data);
                    bcmdrd_field_set(entry_buffer, slice_idx + 28,
                                     slice_idx + 28, &data);
                    /* Paring mode */
                    if (part % 2 == 0) {
                        bcmdrd_field_get(entry_buffer, (pri_slice_idx / 2)  + 32,
                                      (pri_slice_idx / 2) + 32, &data);
                        bcmdrd_field_set(entry_buffer, (slice_idx / 2)  + 32,
                                      (slice_idx / 2) + 32, &data);
                    }
                    /* Src Sel mode */
                    bcmdrd_field_get(entry_buffer, (3 * pri_slice_idx) + 42,
                                      (3 * pri_slice_idx) + 44, &data);
                    bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 42,
                                      (3 * slice_idx) + 44, &data);

                } else if (sid_enum == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR) {
                    /* KEY1 class id Sel */
                    bcmdrd_field_get(entry_buffer, 2 * pri_slice_idx,
                                      (2 * pri_slice_idx) + 1, &data);
                    bcmdrd_field_set(entry_buffer, 2 * slice_idx,
                                      (2 * slice_idx) + 1, &data);
                    /* KEY2 class id Sel */
                    bcmdrd_field_get(entry_buffer, (3 * pri_slice_idx) + 8,
                                      (3 * pri_slice_idx) + 10, &data);
                    bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 8,
                                      (3 * slice_idx) + 10, &data);
                    /* KEY3 class id Sel */
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx) + 20,
                                      (2 * pri_slice_idx) + 21, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 20,
                                      (2 * slice_idx) + 21, &data);
                    /* KEY4 class id Sel */
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx) + 28,
                                      (2 * pri_slice_idx) + 29, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 28,
                                      (2 * slice_idx) + 29, &data);
                    /* KEY6 class id Sel */
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx) + 36,
                                      (2 * pri_slice_idx) + 37, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 36,
                                      (2 * slice_idx) + 37, &data);
                    /* KEY7 class id Sel */
                    bcmdrd_field_get(entry_buffer, (3 * pri_slice_idx) + 44,
                                      (3 * pri_slice_idx) + 46, &data);
                    bcmdrd_field_set(entry_buffer, (3 * slice_idx) + 44,
                                      (3 * slice_idx) + 46, &data);
                    /* KEY8 class id Sel */
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx) + 56,
                                      (2 * pri_slice_idx) + 57, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx) + 56,
                                      (2 * slice_idx) + 57, &data);

                } else if (sid_enum == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2) {
                    /* IP header Sel mode */
                    bcmdrd_field_get(entry_buffer, pri_slice_idx,
                                     pri_slice_idx, &data);
                    bcmdrd_field_set(entry_buffer, slice_idx, slice_idx, &data);

                } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR) {
                    /* KEY4 DVP Sel */
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx),
                                          (2 * pri_slice_idx) + 1, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx),
                                          (2 * slice_idx) + 1, &data);

                } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR) {
                    /* KEY8 DVP Sel */
                    bcmdrd_field_get(entry_buffer, (2 * pri_slice_idx),
                                          (2 * pri_slice_idx) + 1, &data);
                    bcmdrd_field_set(entry_buffer, (2 * slice_idx),
                                          (2 * slice_idx) + 1, &data);

                } else if (sid_enum == BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR) {
                    /* KEY4 MDL Sel */
                    bcmdrd_field_get(entry_buffer, pri_slice_idx,
                                     slice_idx, &data);
                    bcmdrd_field_set(entry_buffer, slice_idx, slice_idx,
                                     &data);
                } else {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
    } else if ((sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_DISABLE) ||
               (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_ENABLE)) {

        /* Execute the Instruction. */
        if (sid_info->stage_flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
            dyn_info.index = sid_info->slice_id;
        } else {
            dyn_info.index = 0;
        }

        if (sid_info->stage_flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
            slice_en = sid_info->slice_id;
            slice_lookup_en = sid_info->slice_id + 4;
            /* Read the contents of the sid from h/w. */
            SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                                0,
                                                0, 
                                                sid,
                                                &dyn_info,
                                                NULL,
                                                NULL, /* misc_info */
                                                BCMPTM_MAX_PT_FIELD_WORDS,
                                                entry_buffer,
                                                &rsp_entry_cache_vbit,
                                                &rsp_ltid,
                                                &rsp_dfield_format_id,
                                                &rsp_flags));

        }

        if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_ENABLE) {
            /* SLICE_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_en, slice_en, &data_arr[1]);

            /* SLICE_LOOKUP_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_lookup_en, slice_lookup_en,
                             &data_arr[1]);

            if (sid_info->stage_flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                /* IFP_SLICE_MODE
                 * 0 Narrow mode
                 * 1 Wide mode
                 */
                if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == sid_info->group_mode) {
                    /* for narrow mode */
                    bcmdrd_field_set(entry_buffer, 2, 2, &data_arr[0]);
                } else {
                    /* for wide mode */
                    bcmdrd_field_set(entry_buffer, 2, 2, &data_arr[1]);
                }
            }
        } else {
            /* RESET SLICE_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_en, slice_en, &data_arr[0]);

            /* RESET SLICE_LOOKUP_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_lookup_en, slice_lookup_en,
                             &data_arr[0]);

            if (sid_info->stage_flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                /* RESET IFP_SLICE_MODE */
                bcmdrd_field_set(entry_buffer, 2, 2, &data_arr[1]);
            }
        }

        if (sid_info->stage_flags & BCMPTM_STAGE_MULTI_VIEW_TCAM) {

            /* Set slice num requiring change */
            slice_mode_change.slice_num0 = sid_info->slice_id;
            slice_mode_change.slice_num1 = -1;

            /* Set new slice mode */
            if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_DISABLE) {
                slice_mode_change.slice_mode = BCMPTM_TCAM_SLICE_DISABLED;
            } else {
                if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == sid_info->group_mode) {
                    slice_mode_change.slice_mode = BCMPTM_TCAM_SLICE_NARROW;
                } else {
                    slice_mode_change.slice_mode = BCMPTM_TCAM_SLICE_WIDE;
                }
            }

            /* Set slice group id */
            slice_mode_change.slice_group_id = sid_info->slice_group_id;
            slice_mode_ptr = &slice_mode_change;
            op_type = BCMPTM_RM_OP_SLICE_MODE_CHANGE;
        }
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_MAP_UPDATE) {
             start_bit = 0;
             width = 2;
             vs_offset = 8;
             /* Modify the contents of the sid. */
             for (field_idx = 0; field_idx < sid_info->num_slices; field_idx++) {

                 /*Update the Virtual Group No. in entry buffer*/
                 field_val = sid_info->group_ltid[field_idx];
                 bcmdrd_field_set(entry_buffer,
                                  start_bit,
                                  start_bit + width - 1,
                                  &field_val);

                 /*Update the physical slice No. in entry buffer*/
                 field_val = sid_info->lt_action_pri[field_idx];
                 bcmdrd_field_set(entry_buffer,
                                  start_bit + vs_offset,
                                  start_bit + width + vs_offset - 1,
                                  &field_val);
                 start_bit = start_bit + width;
             }
             /* Execute the Instruction. */
             dyn_info.index = 0;

    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* write the contents of the sid back into h/w */
    SHR_IF_ERR_EXIT(bcmptm_cmdproc_write(unit,
                                         0,
                                         0, 
                                         sid,
                                         &dyn_info,
                                         NULL,
                                         NULL, /* misc_info */
                                         entry_buffer,
                                         1,
                                         1,
                                         1,
                                         op_type, slice_mode_ptr,
                                         ltid,
                                         0,
                                         &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}
