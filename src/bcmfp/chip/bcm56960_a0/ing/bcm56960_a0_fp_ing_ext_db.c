/*! \file bcm56960_a0_fp_ing_ext_db.c
 *
 * IFP multi hierarchy extractors configuration DB
 * initialization function for Tomahawk(56960_A0).
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
#include <bcmfp/bcmfp_keygen_api.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_bcm56960_a0_ingress_ext_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage)
{
    int idx = 0;                     /* Index iterator.                     */
    int level = 0;                   /* Extractor hierarchy level.          */
    int gran = 0;                    /* Extractor granularity.              */
    int ext_num = 0;                 /* Extractor number.                   */
    int part;                        /* Entry part number.                  */
    bcmfp_keygen_ext_attrs_t ext_attrs;
    bcmfp_keygen_ext_attrs_t temp_attrs;
    bcmfp_keygen_ext_attrs_t *attrs = &temp_attrs;
    uint32_t ext_attrs_size = 0;
    BCMFP_KEYGEN_EXT_CFG_DECL;              /* Extractors config declaration.      */
    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    ext_attrs_size = sizeof(bcmfp_keygen_ext_attrs_t);
    sal_memset(attrs, 0, ext_attrs_size);

    BCMFP_ALLOC(ext_cfg_db,
                sizeof(bcmfp_keygen_ext_cfg_db_t), "Field Extractors DB");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_DBLINTRA] = ext_cfg_db;

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S1, 96, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 224, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 8, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 24, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 40, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 48, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 56, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 60, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 68, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 72, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 76, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 84, attrs);

    /* Initialize Level1 2-bit extractors section. */
    level = 1;
    gran = 2;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 88, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 90, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 92, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 94, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 98, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 100, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2S2, 102, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 32, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 48, &ext_attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S2, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S2, 80, attrs);

    /* Initialize Level_2 8-bit extractors section. */
    level = 2;
    gran = 8;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 144, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 156, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 160, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 164, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 168, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 172, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 176, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 180, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 184, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 186, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 188, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 190, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 192, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 194, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 196, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S2, 198, &ext_attrs);

    /* Initialize two Level_3 16-bit extractors section. */
    level = 3;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3S1, BCMFP_KEYGEN_EXT_SECTION_FK, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L3S1, BCMFP_KEYGEN_EXT_SECTION_FK, 20, &ext_attrs);

    /* Initialize four Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    for (idx = 1; idx < 4; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
            36 + (gran * (idx - 1)) , &ext_attrs);
    }

    /* Initialize Level_3 16-bit extractors section. */
    level = 3;
    gran = 16;
    ext_num = 2;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3S1, BCMFP_KEYGEN_EXT_SECTION_FK, 48, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L3S1, BCMFP_KEYGEN_EXT_SECTION_FK, 64, &ext_attrs);

    /* Initialize seventeen Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 2; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
            80 + (idx * gran), attrs);
    }
    ext_num = 4;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0_HM);
    for (idx = 2; idx < 6; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
            80 + (idx * gran), &ext_attrs);
    }
    ext_num = 4;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1_HM);
    for (idx = 6; idx < 10; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
            80 + (idx * gran), &ext_attrs);
    }
    ext_num = 4;
    idx = 10;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
        80 + (idx * gran), &ext_attrs);
    idx = 11;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
        80 + (idx * gran), &ext_attrs);
    idx = 12;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
        80 + (idx * gran), &ext_attrs);

    ext_num = 4;
    idx = 13;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
        80 + (idx * gran), &ext_attrs);

    idx = 14;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
        80 + (idx * gran), &ext_attrs);

    idx = 15;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
        80 + (idx * gran), &ext_attrs);

    ext_num = 4;
    for (idx = 16; idx < 17; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
            80 + (idx * gran), attrs);
    }

    /* Initialize 5 Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK,
            148 + (ext_num * gran), attrs);
    }

    /* Overlay with BCMFP_KEYGEN_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB_HM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK, 152, &ext_attrs);

    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK, 154, &ext_attrs);

    /* Overlay with NAT_DST_REALM_ID. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK, 156, &ext_attrs);

    /* Initialize 2 Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED);
    /* Overlay with NAT_NEEDED. */
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK, 158, &ext_attrs);

    /* Overlay with DROP. */
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3S2, BCMFP_KEYGEN_EXT_SECTION_FK, 159, &ext_attrs);

    /* Initialize Level_4 extractor section. */
    level = 4;
    gran = 160;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FK, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

    /* 320 bits mode - Initialize Level_1 32-bit extractor section. */
    ext_cfg_db = NULL;
    BCMFP_ALLOC(ext_cfg_db,
                sizeof(bcmfp_keygen_ext_cfg_db_t), "Field Extractors DB");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_DBLINTER] = ext_cfg_db;

    level = 1;
    gran = 32;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 32, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 96, attrs);

    /* Initialize Level_1 16-bit extractor section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 144, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 176, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 208, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 224, attrs);

    /* Initialize Level_1 8-bit extractor section. */
    level = 1;
    gran = 8;
    ext_num = 0;

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 8, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 8, attrs);


    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 24, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 24, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 40, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 40, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 48, attrs);

    /* Initialize Level_1 4-bit extractor section. */
    level = 1;
    gran = 4;
    ext_num = 0;

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 56, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 56, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 60, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 60, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 68, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 68, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 72, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 72, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 76, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 76, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 84, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 84, attrs);

    /* Initialize Level_1 2-bit extractor section. */
    level = 1;
    gran = 2;
    ext_num = 0;

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 88, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 88, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 90, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 90, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 92, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 92, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 94, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 94, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 98, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 98, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 100, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 100, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 102, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 102, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 16, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 32, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 48, &ext_attrs);

    for (ext_num = 4; ext_num < 10; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS2,
            ((ext_num - 4) * gran), attrs);
    }

    level = 2;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 16, attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 32, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 48, &ext_attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
            BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
            BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
            BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 7,
            BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 8,
            BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 9,
            BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 80, attrs);


    /* Initialize Level_2 8-bit extractors section. */
    level = 2;
    gran = 8;
    ext_num = 0;
    part = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2,104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 144, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 156, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 160, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 164, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 168, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 172, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 176, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 180, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 184, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 186, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 188, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 190, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 192, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 194, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 196, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 198, &ext_attrs);


    /* Initialize Level_2 8-bit extractors section. */
    level = 2;
    gran = 8;
    ext_num = 0;
    part = 1;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2,104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 144, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 156, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 160, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 164, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 168, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 172, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 176, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 180, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 184, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 186, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 188, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 190, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 192, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 194, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 196, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 198, &ext_attrs);

    /* Initialize Level_3 16-bit extractors section - SLICE_A. */
    level = 3;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 20, &ext_attrs);

    /* Overlay with BCMFP_KEYGEN_POST_MUX_SRC_DST_CONT_0/1. */
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 48, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 64, &ext_attrs);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 16, &ext_attrs);
    for (idx = 1; idx < 4; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            36 + (gran * (idx - 1)), attrs);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 10; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), attrs);
    }

    ext_num = 4;
    idx = 10;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
        80 + (idx * gran), &ext_attrs);

    idx = 11;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
        80 + (idx * gran), &ext_attrs);

    idx = 12;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
        (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
        80 + (idx * gran), &ext_attrs);

    ext_num = 4;
    idx = 13;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3);
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    idx = 14;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7);
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    idx = 15;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11);
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    ext_num = 4;
    for (idx = 16; idx < 17; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), attrs);
    }

    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            148 + (ext_num * gran), attrs);
    }

    /* Overlay with BCMFP_KEYGEN_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 152, &ext_attrs);

    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 154, &ext_attrs);

    /* Overlay with NAT_DST_REALM_ID. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 156, &ext_attrs);

    /* Initialize 2 Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    ext_num = 0;
    /* Overlay with NAT_NEEDED. */
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 158, &ext_attrs);
    /* Overlay with DROP. */
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 159, &ext_attrs);

    /* Initialize Level_3 16-bit extractors section - SLICE_B. */
    level = 3;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 48, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 64, &ext_attrs);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 16, attrs);

    for (idx = 1; idx < 4; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            36 + (gran * (idx - 1)), attrs);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            80 + (idx * gran), attrs);
    }

    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            148 + (ext_num * gran), attrs);
    }

    /* Overlay with BCMFP_KEYGEN_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 152, &ext_attrs);

    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 154, attrs);

    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 156, attrs);

    /* Initialize 2 Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            158 + (ext_num * gran), attrs);
    }

    /* Initialize Level_4 extractor section. */
    level = 4;
    gran = 160;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FK, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FKA, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FKB, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

    /* 480 bit mode - Initialize Level_1 32-bit extractor section. */
    ext_cfg_db = NULL;
    BCMFP_ALLOC(ext_cfg_db,
                sizeof(bcmfp_keygen_ext_cfg_db_t), "Field Extractors DB");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_TRIPLE] = ext_cfg_db;

    level = 1;
    gran = 32;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 32, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 96, attrs);

    /* Initialize Level_1 16-bit extractor section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 144, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 176, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 208, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 224, attrs);

    /* Initialize Level_1 8-bit extractor section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 8, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 8, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 8, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 24, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 24, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 24, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 40, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 40, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 40, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 48, attrs);

    /* Initialize Level_1 4-bit extractor section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 56, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 60, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 56, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 60, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 56, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 60, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 68, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 68, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 68, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 72, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 76, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 72, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 76, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 72, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 76, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 84, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 84, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 84, attrs);

    /* Initialize Level_1 2-bit extractor section. */
    level = 1;
    gran = 2;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 88, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 90, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 88, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 90, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 88, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 90, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 92, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 94, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 92, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 94, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 92, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 94, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 98, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 98, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 98, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 100, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 102, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 100, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 102, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 100, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L1E2, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 102, attrs);

    /* Initialize Level_2 16-bit extractors section - Slice_A. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 32, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db,  0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 48, &ext_attrs);
    for (ext_num = 4; ext_num < 10; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS2,
            ((ext_num - 4) * gran), attrs);
    }

    /* Initialize Level_2 8-bit extractors section. */
    level = 2;
    gran = 8;
    ext_num = 0;
    part = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2,104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 144, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 156, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 160, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 164, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 168, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 172, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 176, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 180, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 184, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 186, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 188, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 190, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 192, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 194, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 196, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS2, 198, &ext_attrs);

    /* Initialize Level_2 16-bit extractors section - Slice_B. */
    level = 2;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 16, attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 32, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 48, &ext_attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 8,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 9,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 80, attrs);

    /* Initialize Level_2 8-bit extractors section. */
    level = 2;
    gran = 8;
    ext_num = 0;
    part = 1;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2,104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 144, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 156, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 160, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 164, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 168, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 172, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 176, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 180, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 184, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 186, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 188, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 190, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 192, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 194, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 196, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS2, 198, &ext_attrs);

    /* Initialize Level_2 16-bit extractors section - Slice_C. */
    level = 2;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 16, attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 32, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 48, &ext_attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 5,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 6,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 7,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 8,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 9,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 80, attrs);

    /* Initialize Level_2 8-bit extractors section. */
    level = 2;
    gran = 8;
    ext_num = 0;
    part = 2;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2,104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 144, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 156, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 160, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 164, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 168, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 172, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 176, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 180, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 184, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 186, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 188, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 190, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 192, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 194, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 196, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS2, 198, &ext_attrs);

    /* Initialize Level_3 16-bit extractors section - SLICE_A. */
    level = 3;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 48, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_FKA, 64, &ext_attrs);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 16, &ext_attrs);
    for (idx = 1; idx < 4; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            36 + (gran * (idx - 1)), attrs);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 10; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), attrs);
    }

    ext_num = 4;
    idx = 10;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    idx = 11;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    idx = 12;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    ext_num = 4;
    idx = 13;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    idx = 14;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    idx = 15;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
       BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            80 + (idx * gran), &ext_attrs);

    ext_num = 4;
    for (idx = 16; idx < 17; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran,
                (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
                80 + (idx * gran), attrs);
    }
    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA,
            148 + (ext_num * gran), attrs);
    }

    /* Overlay with BCMFP_KEYGEN_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 152, &ext_attrs);
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 154, &ext_attrs);

    /* Overlay with NAT_DST_REALM_ID. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 156, &ext_attrs);

    /* Initialize Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    ext_num = 0;
    /* Overlay with NAT_NEEDED. */
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 158, &ext_attrs);
    /* Overlay with DROP. */
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3AS2, BCMFP_KEYGEN_EXT_SECTION_FKA, 159, &ext_attrs);

    /* Initialize Level_3 16-bit extractors section - SLICE_B. */
    level = 3;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 48, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_FKB, 64, &ext_attrs);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 16, attrs);
    for (idx = 1; idx < 4; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            36 + (gran * (idx - 1)), attrs);
    }
    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            80 + (idx * gran), attrs);
    }
    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            148 + (gran * ext_num), attrs);
    }

    /* Overlay with BCMFP_KEYGEN_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 152, &ext_attrs);

    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 154, attrs);

    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB, 156, attrs);

    /* Initialize Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3BS2, BCMFP_KEYGEN_EXT_SECTION_FKB,
            158 + (gran * ext_num), attrs);
    }

    /* Initialize Level_3 16-bit extractors section - SLICE_C. */
    level = 3;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3CS1, BCMFP_KEYGEN_EXT_SECTION_FKC, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 1,
        BCMFP_KEYGEN_EXT_SECTION_L3CS1, BCMFP_KEYGEN_EXT_SECTION_FKC, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3CS1, BCMFP_KEYGEN_EXT_SECTION_FKC, 48, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_PASS_THRU);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3CS1, BCMFP_KEYGEN_EXT_SECTION_FKC, 64, &ext_attrs);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC, 16, attrs);
    for (idx = 1; idx < 4; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC,
            36 + (gran * (idx - 1)), attrs);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran,
            (ext_num + idx), BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC,
            80 + (idx * gran), attrs);
    }

    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC,
            148 + (gran * ext_num), attrs);
    }

    /* Overlay with BCMFP_KEYGEN_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 2,
        BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC, 152, &ext_attrs);

    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 3,
        BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC, 154, attrs);

    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num + 4,
        BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC, 156, attrs);


    /* Initialize Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, ext_num,
            BCMFP_KEYGEN_EXT_SECTION_L3CS2, BCMFP_KEYGEN_EXT_SECTION_FKC,
            158 + (gran * ext_num), attrs);
    }

    /* Initialize Level_4 extractor section. */
    level = 4;
    gran = 160;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FK, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FKA, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FKB, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_FKC, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
exit:
    SHR_FUNC_EXIT();
}
