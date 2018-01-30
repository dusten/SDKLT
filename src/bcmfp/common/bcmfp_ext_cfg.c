/*! \file bcmfp_ext_cfg.c
 *
 * APIs to operate on FP extractor configurations.
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

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_ext_cfg_t_init(int unit, bcmfp_ext_cfg_t *ext_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);

    sal_memset(ext_cfg, 0, sizeof(bcmfp_ext_cfg_t));

exit:
    SHR_FUNC_EXIT();

}

STATIC int
bcmfp_ext_cfg_add(int unit,
                  bcmfp_stage_t *stage,
                  bcmfp_ext_cfg_mode_t emode,
                  uint8_t level,
                  bcmfp_ext_cfg_t *new_ext_cfg)
{
    uint32_t size = 0;
    bcmfp_ext_cfg_t *ext_cfg = NULL;
    bcmfp_ext_cfg_db_t *ext_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_ext_cfg, SHR_E_PARAM);

    ext_cfg_db = stage->ext_cfg_db_arr[emode];

    if (ext_cfg_db == NULL) {
        size = sizeof(bcmfp_ext_cfg_db_t);
        BCMFP_ALLOC(ext_cfg_db, size, "Extractor Config DB");
        ext_cfg_db->mode = emode;
    }

    size = (ext_cfg_db->conf_size[level] + 1)  * sizeof(bcmfp_ext_cfg_t);
    BCMFP_ALLOC(ext_cfg, size, "Extractor Config");

    if (ext_cfg_db->ext_cfg[level] != NULL) {
        size -= sizeof(bcmfp_ext_cfg_t);
        sal_memcpy(ext_cfg, ext_cfg_db->ext_cfg[level], size);
        SHR_FREE(ext_cfg_db->ext_cfg[level]);
    }

    ext_cfg_db->ext_cfg[level] = ext_cfg;

    ext_cfg_db->ext_cfg[level][ext_cfg_db->conf_size[level]] = *new_ext_cfg;

    ext_cfg_db->conf_size[level]++;

    stage->ext_cfg_db_arr[emode] = ext_cfg_db;

    SHR_FUNC_EXIT();

exit:
    SHR_FREE(ext_cfg);

    SHR_FREE(ext_cfg_db);

    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_ext_section_cfg_add(int unit,
                           bcmfp_stage_t *stage,
                           bcmfp_ext_cfg_mode_t emode,
                           uint8_t level,
                           bcmfp_ext_cfg_t *new_ext_cfg)
{
    uint32_t size = 0;
    bcmfp_ext_section_t in_section = BCMFP_EXT_SECTION_COUNT;
    bcmfp_ext_cfg_db_t *ext_cfg_db = NULL;
    bcmfp_ext_section_cfg_t *sec_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_ext_cfg, SHR_E_PARAM);

    ext_cfg_db = stage->ext_cfg_db_arr[emode];
    SHR_NULL_CHECK(ext_cfg_db, SHR_E_INTERNAL);

    in_section = new_ext_cfg->in_sec;
    sec_cfg = ext_cfg_db->sec_cfg[in_section];

    if (sec_cfg == NULL) {
        size = sizeof(bcmfp_ext_section_cfg_t);
        BCMFP_ALLOC(sec_cfg, size, "Extractor Section Config");

        sec_cfg->sec = new_ext_cfg->in_sec;

        sec_cfg->drain_bits = new_ext_cfg->gran;

        ext_cfg_db->num_sec++;

        ext_cfg_db->sec_cfg[in_section] = sec_cfg;
    } else {
        sec_cfg->drain_bits += new_ext_cfg->gran;
    }

    SHR_FUNC_EXIT();

exit:

    SHR_FREE(sec_cfg);

    SHR_FUNC_EXIT();
}

int
bcmfp_ext_cfg_insert(int unit,
                     bcmfp_stage_t *stage,
                     bcmfp_ext_cfg_mode_t emode,
                     uint8_t level,
                     bcmfp_ext_cfg_t *ext_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);

    

    if (level < BCMFP_EXT_LEVEL_COUNT) {
        SHR_IF_ERR_EXIT(
            bcmfp_ext_cfg_add(unit, stage, emode, level, ext_cfg));
    }

    SHR_IF_ERR_EXIT(
        bcmfp_ext_section_cfg_add(unit, stage, emode, level, ext_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ext_cfg_db_cleanup(int unit,
                         bcmfp_stage_t *stage)
{
    uint8_t level = 0;
    bcmfp_ext_cfg_mode_t emode;
    bcmfp_keygen_mode_t  kgmode;
    bcmfp_ext_section_t section;
    bcmfp_keygen_ext_section_t kgsection;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    for (emode = 0; emode < BCMFP_EXT_CFG_MODE_COUNT; emode++) {

        if (stage->ext_cfg_db_arr[emode] == NULL) {
            continue;
        }

        for (level = 0; level < BCMFP_EXT_LEVEL_COUNT; level++) {
            SHR_FREE(stage->ext_cfg_db_arr[emode]->ext_cfg[level]);
        }

        for (section = 0; section < BCMFP_EXT_SECTION_COUNT; section++) {
            SHR_FREE(stage->ext_cfg_db_arr[emode]->sec_cfg[section]);
        }

        SHR_FREE(stage->ext_cfg_db_arr[emode]);
    }

    for (kgmode = 0; kgmode < BCMFP_KEYGEN_MODE_COUNT; kgmode++) {

        if (stage->kgn_ext_cfg_db_arr[kgmode] == NULL) {
            continue;
        }

        for (level = 0; level < BCMFP_KEYGEN_EXT_LEVEL_COUNT; level++) {
            SHR_FREE(stage->kgn_ext_cfg_db_arr[kgmode]->ext_cfg[level]);
        }

        for (kgsection = 0;
             kgsection < BCMFP_KEYGEN_EXT_SECTION_COUNT;
             kgsection++) {
            SHR_FREE(stage->kgn_ext_cfg_db_arr[kgmode]->sec_cfg[kgsection]);
        }

        SHR_FREE(stage->kgn_ext_cfg_db_arr[kgmode]);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ext_codes_init(int unit, bcmfp_ext_sel_t *ext_codes)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    sal_memset(ext_codes, BCMFP_EXT_SELCODE_DONT_CARE, sizeof(bcmfp_ext_sel_t));

    ext_codes->keygen_index = -1;
    ext_codes->ipbm_source = -1;
    ext_codes->ltid = -1;
    ext_codes->default_policy_enable = 0;

exit:
    SHR_FUNC_EXIT();
}
