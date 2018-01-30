/*! \file bcmfp_presel_qual_cfg.c
 *
 * Utility APIs to operate with FP preselection qualifers.
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_presel_qual_cfg_t_init(int unit, bcmfp_qual_cfg_t *presel_qual_cfg)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(presel_qual_cfg, SHR_E_PARAM);

    sal_memset(presel_qual_cfg, 0, sizeof(bcmfp_qual_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qual_t qual_id,
                      bcmfp_qual_cfg_t *new_presel_qual_cfg)
{
    bool qual_cfg_info_db_alloc = 0;
    bool qual_cfg_info_alloc = 0;
    uint8_t num_qual_cfg = 0;
    bcmfp_qual_cfg_t *qual_cfg = NULL;
    bcmfp_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_presel_qual_cfg, SHR_E_PARAM);

    qual_cfg_info_db = stage->presel_qual_cfg_info_db;

    if (qual_cfg_info_db == NULL) {
        BCMFP_ALLOC(qual_cfg_info_db,
                    sizeof(bcmfp_qual_cfg_info_db_t),
                    "BCMFP:Qualifier Config DB");
        stage->presel_qual_cfg_info_db = qual_cfg_info_db;
        qual_cfg_info_db_alloc = 1;
    }

    qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual_id];
    if (qual_cfg_info == NULL) {
        BCMFP_ALLOC(qual_cfg_info,
                    sizeof(bcmfp_qual_cfg_info_t),
                    "BCMFP:Qualifier Config Info");
        qual_cfg_info_db->qual_cfg_info[qual_id] = qual_cfg_info;
        qual_cfg_info_alloc = 1;
    }

    num_qual_cfg = qual_cfg_info->num_qual_cfg + 1;

    BCMFP_ALLOC(qual_cfg_arr,
                num_qual_cfg * sizeof(bcmfp_qual_cfg_t),
                "BCMFP:Qualifier config");

    sal_memcpy(qual_cfg_arr,
               qual_cfg_info->qual_cfg_arr,
               sizeof(bcmfp_qual_cfg_t) * (num_qual_cfg - 1));

    qual_cfg = &(qual_cfg_arr[num_qual_cfg - 1]);

    sal_memcpy(qual_cfg, new_presel_qual_cfg, sizeof(bcmfp_qual_cfg_t));

    SHR_FREE(qual_cfg_info->qual_cfg_arr);

    qual_cfg_info->qual_cfg_arr = qual_cfg_arr;
    qual_cfg_info->num_qual_cfg++;

    SHR_FUNC_EXIT();

exit:
    SHR_FREE(qual_cfg_arr);
    if (qual_cfg_info_db_alloc) {
        SHR_FREE(qual_cfg_info_db);
    }
    if (qual_cfg_info_alloc) {
        SHR_FREE(qual_cfg_info);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_qual_cfg_db_cleanup(int unit,
                          bcmfp_stage_t *stage)
{
    bcmfp_qual_t qual = 0;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    qual_cfg_info_db = stage->presel_qual_cfg_info_db;

    if (qual_cfg_info_db != NULL) {

        for (qual = 0; qual < BCMFP_QUAL_COUNT; qual++) {

            qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual];
            if (qual_cfg_info == NULL)  {
                continue;
            }

            SHR_FREE(qual_cfg_info->qual_cfg_arr);
            SHR_FREE(qual_cfg_info);
            qual_cfg_info_db->qual_cfg_info[qual] = NULL;
        }

        SHR_FREE(qual_cfg_info_db);
    }

    stage->presel_qual_cfg_info_db = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_presel_set(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      bcmfp_qual_t qual,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw)
{
    bool is_presel = 1;
    uint8_t part;
    uint8_t entry_parts = 0;
    uint16_t idx;
    uint16_t start_bit;
    uint16_t end_bit;
    bcmfp_stage_t *stage = NULL;
    bcmfp_qual_cfg_t *qual_cfg = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id, group_id, &group_oper_info));

    /* Retrieve group entry parts */
    SHR_IF_ERR_EXIT(
        bcmfp_group_parts_count(unit, group_oper_info->flags, &entry_parts));

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    qual_cfg = stage->presel_qual_cfg_info_db->qual_cfg_info[qual]->qual_cfg_arr;
    SHR_IF_ERR_EXIT((qual_cfg == NULL) ? SHR_E_NOT_FOUND : SHR_E_NONE);

    for (idx = 0; idx < qual_cfg->num_chunks; idx++) {

        for (part = 0; part < entry_parts; part++) {

            SHR_IF_ERR_EXIT(
                bcmfp_entry_key_info_get(unit,
                                         is_presel,
                                         stage_id,
                                         group_id,
                                         &start_bit,
                                         &end_bit));

            start_bit += qual_cfg->e_params[idx].bus_offset;
            end_bit = start_bit + qual_cfg->e_params[idx].width - 1;
            bcmdrd_field_set(ekw[part], start_bit, end_bit, data);

            
            SHR_IF_ERR_EXIT(
                bcmfp_entry_mask_info_get(unit,
                                          is_presel,
                                          stage_id,
                                          group_id,
                                          &start_bit,
                                          &end_bit));

            start_bit += qual_cfg->e_params[idx].bus_offset;
            end_bit = start_bit + qual_cfg->e_params[idx].width - 1;
            bcmdrd_field_set(ekw[part], start_bit, end_bit, mask);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_presel_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      bcmfp_qual_t qual_id,
                      uint32_t *data,
                      uint32_t *mask,
                      uint32_t **ekw)
{
    int start_bit;
    int end_bit;
    uint8_t part;
    uint8_t entry_parts = 0;
    uint16_t idx;
    uint32_t group_flags = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_qual_cfg_t *qual_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    /* Retrieve group entry parts */
    SHR_IF_ERR_EXIT(bcmfp_group_parts_count(unit, group_flags, &entry_parts));

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    qual_cfg = stage->presel_qual_cfg_info_db->qual_cfg_info[qual_id]->qual_cfg_arr;
    SHR_IF_ERR_EXIT((qual_cfg == NULL) ? SHR_E_NOT_FOUND : SHR_E_NONE);

    for (idx = 0; idx < qual_cfg->num_chunks; idx++) {

        for (part = 0; part < entry_parts; part++) {

            start_bit = qual_cfg->e_params[idx].bus_offset;
            end_bit = start_bit + qual_cfg->e_params[idx].width - 1;
            bcmdrd_field_get(ekw[part], start_bit, end_bit, data);

            
        }
    }

exit:
    SHR_FUNC_EXIT();
}


