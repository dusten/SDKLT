/*! \file bcm56960_a0_fp_selcodes_init.c
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

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

static const bcmdrd_fid_t l1_32_sel[] = /* 32 bit extractors. */
    {
        L1_E32_SEL_0f,
        L1_E32_SEL_1f,
        L1_E32_SEL_2f,
        L1_E32_SEL_3f
    };
static const bcmdrd_fid_t l1_16_sel[] = /* 16 bit extractors. */
    {
        L1_E16_SEL_0f,
        L1_E16_SEL_1f,
        L1_E16_SEL_2f,
        L1_E16_SEL_3f,
        L1_E16_SEL_4f,
        L1_E16_SEL_5f,
        L1_E16_SEL_6f
    };
static const bcmdrd_fid_t l1_8_sel[] = /* 8 bit extractors. */
    {
        L1_E8_SEL_0f,
        L1_E8_SEL_1f,
        L1_E8_SEL_2f,
        L1_E8_SEL_3f,
        L1_E8_SEL_4f,
        L1_E8_SEL_5f,
        L1_E8_SEL_6f
    };
static const bcmdrd_fid_t l1_4_sel[] = /* 4 bit extractors. */
    {
        L1_E4_SEL_0f,
        L1_E4_SEL_1f,
        L1_E4_SEL_2f,
        L1_E4_SEL_3f,
        L1_E4_SEL_4f,
        L1_E4_SEL_5f,
        L1_E4_SEL_6f,
        L1_E4_SEL_7f
    };
static const bcmdrd_fid_t l1_2_sel[] = /* 2 bit extractors. */
    {
        L1_E2_SEL_0f,
        L1_E2_SEL_1f,
        L1_E2_SEL_2f,
        L1_E2_SEL_3f,
        L1_E2_SEL_4f,
        L1_E2_SEL_5f,
        L1_E2_SEL_6f,
        L1_E2_SEL_7f
    };
static const bcmdrd_fid_t l2_16_sel[] = /* 16 bit extractors. */
    {
        L2_E16_SEL_0f,
        L2_E16_SEL_1f,
        L2_E16_SEL_2f,
        L2_E16_SEL_3f,
        L2_E16_SEL_4f,
        L2_E16_SEL_5f,
        L2_E16_SEL_6f,
        L2_E16_SEL_7f,
        L2_E16_SEL_8f,
        L2_E16_SEL_9f
    };
static const bcmdrd_fid_t l3_4_sel[] = /* 4 bit extractors. */
    {
        L3_E4_SEL_0f,
        L3_E4_SEL_1f,
        L3_E4_SEL_2f,
        L3_E4_SEL_3f,
        L3_E4_SEL_4f,
        L3_E4_SEL_5f,
        L3_E4_SEL_6f,
        L3_E4_SEL_7f,
        L3_E4_SEL_8f,
        L3_E4_SEL_9f,
        L3_E4_SEL_10f,
        L3_E4_SEL_11f,
        L3_E4_SEL_12f,
        L3_E4_SEL_13f,
        L3_E4_SEL_14f,
        L3_E4_SEL_15f,
        L3_E4_SEL_16f,
        L3_E4_SEL_17f,
        L3_E4_SEL_18f,
        L3_E4_SEL_19f,
        L3_E4_SEL_20f
    };
static const bcmdrd_fid_t l3_2_sel[] = /* 2 bit extractors. */
    {
        L3_E2_SEL_0f,
        L3_E2_SEL_1f,
        L3_E2_SEL_2f,
        L3_E2_SEL_3f,
        L3_E2_SEL_4f
    };
static const bcmdrd_fid_t l3_1_sel[] = /* 1 bit extractors. */
    {
        L3_E1_SEL_0f,
        L3_E1_SEL_1f
    };

int
bcmfp_bcm56960_a0_ext_codes_init(int unit,
                                 bcmfp_group_t *fg,
                                 uint8_t part,
                                 bcmdrd_sid_t kgp_sid,
                                 uint32_t *kgp_entry)
{
    uint8_t idx = 0;
    uint32_t value = 0;
    uint32_t *prof_entry = NULL;
    bcmdrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    prof_entry = kgp_entry;
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

	/* Set 32bit extractor selcode values. */
    for (idx = 0; idx < 4; idx++) {
        if (-1 != fg->ext_codes[part].l1_e32_sel[idx]) {
            value = fg->ext_codes[part].l1_e32_sel[idx];
            fid = l1_32_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (-1 != fg->ext_codes[part].l1_e16_sel[idx]) {
            value = fg->ext_codes[part].l1_e16_sel[idx];
            fid = l1_16_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 8bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (-1 != fg->ext_codes[part].l1_e8_sel[idx]) {
            value = fg->ext_codes[part].l1_e8_sel[idx];
            fid = l1_8_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (-1 != fg->ext_codes[part].l1_e4_sel[idx]) {
            value = fg->ext_codes[part].l1_e4_sel[idx];
            fid = l1_4_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (-1 != fg->ext_codes[part].l1_e2_sel[idx]) {
            value = fg->ext_codes[part].l1_e2_sel[idx];
            fid = l1_2_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (-1 != fg->ext_codes[part].l2_e16_sel[idx]) {
            value = fg->ext_codes[part].l2_e16_sel[idx];
            fid = l2_16_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 21; idx++) {
        if (-1 != fg->ext_codes[part].l3_e4_sel[idx]) {
            value = fg->ext_codes[part].l3_e4_sel[idx];
            fid = l3_4_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (-1 != fg->ext_codes[part].l3_e2_sel[idx]) {
            value = fg->ext_codes[part].l3_e2_sel[idx];
            fid = l3_2_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 1bit extractor selcode values. */
    for (idx = 0; idx < 2; idx++) {
        if (-1 != fg->ext_codes[part].l3_e1_sel[idx]) {
            value = fg->ext_codes[part].l3_e1_sel[idx];
            fid = l3_1_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }
exit:
    SHR_FUNC_EXIT();
}
