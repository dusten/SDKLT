/*! \file bcmfp_qual_cfg.c
 *
 * Utility APIs to operate with FP qualifers.
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
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

int
bcmfp_qual_cfg_t_init(int unit, bcmfp_qual_cfg_t *qual_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_cfg, SHR_E_PARAM);

    sal_memset(qual_cfg, 0, sizeof(bcmfp_qual_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_cfg_insert(int unit,
                      bcmfp_stage_t *stage,
                      bcmfp_qual_t qual_id,
                      bcmfp_qual_cfg_t *new_qual_cfg)
{
    bool qual_cfg_info_db_alloc = 0;
    bool qual_cfg_info_alloc = 0;
    uint8_t chunk = 0;
    uint8_t num_qual_cfg = 0;
    bcmfp_qual_cfg_t *qual_cfg = NULL;
    bcmfp_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(new_qual_cfg, SHR_E_PARAM);

    qual_cfg_info_db = stage->qual_cfg_info_db;

    if (qual_cfg_info_db == NULL) {
        BCMFP_ALLOC(qual_cfg_info_db,
                    sizeof(bcmfp_qual_cfg_info_db_t),
                    "BCMFP:Qualifier Config DB");
        stage->qual_cfg_info_db = qual_cfg_info_db;
        qual_cfg_info_db_alloc = 1;
    }

    qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual_id];
    if (qual_cfg_info == NULL) {
        BCMFP_ALLOC(qual_cfg_info,
                    sizeof(bcmfp_qual_cfg_info_t),
                    "BCMFP:Qualifier Config Info");
        qual_cfg_info_db->qual_cfg_info[qual_id] = qual_cfg_info;
        qual_cfg_info->qual = qual_id;
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

    for (chunk = 0; chunk < new_qual_cfg->num_chunks; chunk++) {
        if (BCMFP_EXT_SECTION_L1E32 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 32;
        } else if (BCMFP_EXT_SECTION_L1E16 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 16;
        } else if (BCMFP_EXT_SECTION_L1E8 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 8;
        } else if (BCMFP_EXT_SECTION_L1E4 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 4;
        } else if (BCMFP_EXT_SECTION_L1E2 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 2;
        }
    }

    sal_memcpy(qual_cfg, new_qual_cfg, sizeof(bcmfp_qual_cfg_t));

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
bcmfp_qual_cfg_db_cleanup(int unit,
                          bcmfp_stage_t *stage)
{
    bcmfp_qual_t qual = 0;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db = NULL;
    bcmfp_keygen_qual_cfg_info_t *kgn_qual_cfg_info = NULL;
    bcmfp_keygen_qual_cfg_info_db_t *kgn_qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    qual_cfg_info_db = stage->qual_cfg_info_db;
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
    stage->qual_cfg_info_db = NULL;

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
        stage->presel_qual_cfg_info_db = NULL;
    }
    stage->presel_qual_cfg_info_db = NULL;

    kgn_qual_cfg_info_db = stage->kgn_qual_cfg_info_db;
    if (kgn_qual_cfg_info_db != NULL) {

        for (qual = 0; qual < BCMFP_QUAL_COUNT; qual++) {

            kgn_qual_cfg_info = kgn_qual_cfg_info_db->qual_cfg_info[qual];
            if (kgn_qual_cfg_info == NULL)  {
                continue;
            }

            SHR_FREE(kgn_qual_cfg_info->qual_cfg_arr);
            SHR_FREE(kgn_qual_cfg_info);
            kgn_qual_cfg_info_db->qual_cfg_info[qual] = NULL;
        }

        SHR_FREE(kgn_qual_cfg_info_db);
    }
    stage->kgn_qual_cfg_info_db = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_set(int unit,
               bcmfp_stage_id_t stage_id,
               bcmfp_group_id_t group_id,
               bcmfp_entry_type_t entry_type,
               bcmfp_qual_t qual,
               uint32_t *bitmap,
               uint32_t *data,
               uint32_t *mask,
               uint32_t **ekw)
{
    bool       is_presel = 0;
    uint8_t    miss_count = 0;
    uint8_t    parts_cnt = 0;
    uint8_t    part_idx = 0;
    int16_t    get_start_bit = -1;
    uint16_t   set_start_bit = 0;
    uint16_t   key_start_bit = 0;
    uint16_t   mask_start_bit = 0;
    uint16_t   key_end_bit = 0;
    uint16_t   mask_end_bit = 0;
    uint16_t   idx = 0;
    uint16_t   width = 0;
    uint16_t   chunk_size[BCMFP_QUAL_OFFSETS_MAX] = { 0 };
    uint16_t   chunk_offset[BCMFP_QUAL_OFFSETS_MAX] = { 0 };
    uint32_t   start_offset = 0;
    uint32_t   group_flags = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmfp_qual_data_t out_data1;
    bcmfp_qual_data_t out_mask1;
    bcmfp_qual_data_t actual_data;
    bcmfp_qual_data_t actual_mask;
    bcmfp_qual_data_t qual_bitmap;
    bcmfp_qual_offset_info_t *q_offset = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(ekw, SHR_E_PARAM);

    rv = bcmfp_group_flags_get(unit, stage_id, group_id, &group_flags);
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    /* Retrieve group entry parts */
    rv = bcmfp_group_parts_count(unit, group_flags, &parts_cnt);
    SHR_IF_ERR_EXIT(rv);

    miss_count = 0;
    get_start_bit = -1;
    set_start_bit = 0;
    width = 0;
    BCMFP_ALLOC(q_offset, sizeof(bcmfp_qual_offset_info_t),
                "Qualifier offset information in any part");
    /* Update each chunk size of the qualifier. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));
        /* Retrieve the Qualifier offsets for the particular
         * entry part.
         */
        rv = bcmfp_group_qual_offset_info_get(unit,
                                              stage_id,
                                              group_id,
                                              part_idx,
                                              qual,
                                              q_offset);

        if (rv == SHR_E_NOT_FOUND) {
            continue;
        } else if (SHR_FAILURE(rv)) {
            return rv;
        }

        for (idx = 0; idx < q_offset->num_offsets; idx++) {
            if (q_offset->width[idx]) {
                chunk_size[idx] = q_offset->width[idx];
            }
        }
    }
    /* Update each chunk offset of the qualifier. offsets in q_offset
     * are local to the partition but chunk_offset array will have
     * offsets across the partitions.
     */
    for (idx = 0; idx < BCMFP_QUAL_OFFSETS_MAX; idx++) {
        if (idx == 0) {
            chunk_offset[idx] = 0;
            continue;
        }
        if (chunk_size[idx]) {
           chunk_offset[idx] = chunk_offset[idx-1] + chunk_size[idx - 1];
        } else {
           break;
        }
    }

    /* Get the actual dat and mask after removing the
     * unwanted bits.
     */
    BCMFP_QUAL_DATA_CLEAR(actual_data);
    BCMFP_QUAL_DATA_CLEAR(actual_mask);
    BCMFP_QUAL_DATA_CLEAR(qual_bitmap);
    if (bitmap == NULL) {
        SHR_IF_ERR_EXIT(
            bcmfp_group_qual_bitmap_get(unit,
                                        stage_id,
                                        group_id,
                                        qual,
                                        qual_bitmap));
    } else {
        sal_memcpy(qual_bitmap, bitmap,
            sizeof(uint32_t) * BCMFP_QUAL_BITMAP_MAX_WORDS);
    }
    for (idx = 0; idx < BCMFP_QUAL_BITMAP_MAX_WORDS * 32; idx++) {
        if (SHR_BITGET(qual_bitmap, idx)) {
           if (-1 == get_start_bit) {
              get_start_bit = idx;
           }
           width++;
        } else {
           if (-1 != get_start_bit) {

              BCMFP_QUAL_DATA_CLEAR(out_data1);
              BCMFP_QUAL_DATA_CLEAR(out_mask1);

              bcmdrd_field_get(data,
                               get_start_bit,
                               get_start_bit + width - 1,
                               out_data1);

              bcmdrd_field_set(actual_data,
                               set_start_bit,
                               set_start_bit + width - 1,
                               out_data1);

              bcmdrd_field_get(mask,
                               get_start_bit,
                               get_start_bit + width - 1,
                               out_mask1);

              bcmdrd_field_set(actual_mask,
                               set_start_bit,
                               set_start_bit + width - 1,
                               out_mask1);

              set_start_bit += width;
              get_start_bit = -1;
              width = 0;
           }
        }
    }

    /* Update the ekw. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));
        rv = bcmfp_group_qual_offset_info_get(unit,
                                              stage_id,
                                              group_id,
                                              part_idx,
                                              qual,
                                              q_offset);
        if (rv == SHR_E_NOT_FOUND) {
            miss_count += 1;
            continue;
        } else if (SHR_FAILURE(rv)) {
           return rv;
        }
        start_offset = 0;
        SHR_IF_ERR_EXIT(
            bcmfp_entry_key_info_get(unit,
                                     is_presel,
                                     stage_id,
                                     group_id,
                                     &key_start_bit,
                                     &key_end_bit));
        SHR_IF_ERR_EXIT(
            bcmfp_entry_mask_info_get(unit,
                                      is_presel,
                                      stage_id,
                                      group_id,
                                      &mask_start_bit,
                                      &mask_end_bit));
        /* Extract relevent data for this partion from
         * user passed data.
         */
        for (idx = 0; idx < q_offset->num_offsets; idx++) {
            if (0 == q_offset->width[idx]) {
                continue;
            }
            BCMFP_QUAL_DATA_CLEAR(out_data1);
            BCMFP_QUAL_DATA_CLEAR(out_mask1);

            bcmdrd_field_get(actual_data,
                             chunk_offset[idx],
                             (chunk_offset[idx] +
                             q_offset->width[idx] - 1),
                             out_data1);

            bcmdrd_field_set(ekw[part_idx],
                             (key_start_bit +
                              q_offset->offset[idx]),
                             (key_start_bit +
                              q_offset->offset[idx] +
                              q_offset->width[idx] - 1),
                             out_data1);

            bcmdrd_field_get(actual_mask,
                             chunk_offset[idx],
                             (chunk_offset[idx] +
                             q_offset->width[idx] - 1),
                             out_mask1);
            if (!(stage->flags & BCMFP_STAGE_ENTRY_TYPE_HASH)) {
                bcmdrd_field_set(ekw[part_idx],
                             (mask_start_bit +
                              q_offset->offset[idx]),
                             (mask_start_bit +
                              q_offset->offset[idx] +
                              q_offset->width[idx] - 1),
                             out_mask1);
            }
            start_offset += q_offset->width[idx];
        }
    }

    if (miss_count == parts_cnt) {
        return SHR_E_NOT_FOUND;
    }

exit:
    SHR_FREE(q_offset);
    SHR_FUNC_EXIT();
}

int
bcmfp_qual_data_size_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_qual_t qual_id,
                         uint32_t *size)
{
    uint8_t qual_cfg_idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_keygen_qual_cfg_t *kgn_qual_cfg_arr = NULL;
    bcmfp_keygen_qual_cfg_info_t *kgn_qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->flags & BCMFP_STAGE_KEY_TYPE_PRESEL) {
        kgn_qual_cfg_info = stage->kgn_qual_cfg_info_db->qual_cfg_info[qual_id];
        kgn_qual_cfg_arr = kgn_qual_cfg_info->qual_cfg_arr;
        *size = 0;
        for (qual_cfg_idx = 0;
            qual_cfg_idx < kgn_qual_cfg_info->num_qual_cfg;
            qual_cfg_idx++) {
            if (*size < kgn_qual_cfg_arr[qual_cfg_idx].size) {
                *size = kgn_qual_cfg_arr[qual_cfg_idx].size;
            }
        }
    } else {
        qual_cfg_info = stage->qual_cfg_info_db->qual_cfg_info[qual_id];
        qual_cfg_arr = qual_cfg_info->qual_cfg_arr;
        *size = 0;
        for (qual_cfg_idx = 0;
            qual_cfg_idx < qual_cfg_info->num_qual_cfg;
            qual_cfg_idx++) {
            if (*size < qual_cfg_arr[qual_cfg_idx].size) {
                *size = qual_cfg_arr[qual_cfg_idx].size;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ipbm_validate(int unit,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_group_id_t group_id,
                    bcmdrd_pbmp_t device_pbmp)
{
    bcmdrd_pbmp_t valid_pbmp;
    bcmdrd_pbmp_t temp_pbm;
    bcmfp_stage_t *stage = NULL;
    int tbl_inst = -1;

    SHR_FUNC_ENTER(unit);

    sal_memset(&valid_pbmp, 0, sizeof(bcmdrd_pbmp_t));

    SHR_IF_ERR_EXIT(
            bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_EXIT(
            bcmfp_group_tbl_inst_get(unit, stage_id, group_id, &tbl_inst));

    BCMFP_FIELD_VALID_PIPE_PBMP_GET(tbl_inst, valid_pbmp);

    BCMDRD_PBMP_ASSIGN(temp_pbm, valid_pbmp);
    BCMDRD_PBMP_OR(temp_pbm, device_pbmp);
    BCMDRD_PBMP_XOR(temp_pbm, valid_pbmp);
    if (!BCMDRD_PBMP_IS_NULL(temp_pbm)){
        return SHR_E_CONFIG;
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_device_ipbm_to_pipe_ipbmp(int unit,
                                bcmdrd_pbmp_t device_pbmp,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_local_pbmp)
{
    int base_port = -1;
    int pipe_no = tbl_inst;
    uint16_t port = 0;
    uint16_t pipe_port = 0;
    bcmdrd_pbmp_t valid_pbmp;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pipe_local_pbmp, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(valid_pbmp);
    SHR_IF_ERR_EXIT(
        bcmdrd_dev_pipe_phys_pbmp(unit, pipe_no, &valid_pbmp));

    BCMDRD_PBMP_CLEAR(*pipe_local_pbmp);

    BCMDRD_PBMP_ITER(valid_pbmp, port) {
        if (base_port == -1) {
            base_port = port;
        }
        if (BCMDRD_PBMP_MEMBER(device_pbmp, port)) {
            pipe_port = port - base_port;
            BCMDRD_PBMP_PORT_ADD(*pipe_local_pbmp, pipe_port);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pipe_ipbmp_to_device_pbmp(int unit,
                                int8_t tbl_inst,
                                bcmdrd_pbmp_t *pipe_local_pbmp,
                                bcmdrd_pbmp_t *device_pbmp)
{
    bcmdrd_pbmp_t pipe_dpbmp;
    int pipe = 0;
    int num_pipes = 4;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pipe_local_pbmp, SHR_E_PARAM);

    SHR_NULL_CHECK(device_pbmp, SHR_E_PARAM);

    sal_memset(&pipe_dpbmp, 0, sizeof(bcmdrd_pbmp_t));

    if (tbl_inst == -1) {
        /* All pipes */
/*Get the valid pbmp forall the ports in global mode*/
        BCMFP_FIELD_VALID_PIPE_PBMP_GET(tbl_inst, pipe_dpbmp);
        for (pipe = 0; pipe < num_pipes; pipe++) {
/*Update the device_pbmp with the ports in pipe_local_pbmp[pipe]*/
            BCMDRD_PBMP_OR(*device_pbmp, pipe_local_pbmp[pipe]);
        }
/*To unset if any invalid ports are there in the device_pbmp*/
        BCMDRD_PBMP_AND(*device_pbmp, pipe_dpbmp);
    } else {
/*Get the valid pbmp for the given tbl instnace*/
        BCMFP_FIELD_VALID_PIPE_PBMP_GET(tbl_inst,  *device_pbmp);
/*Update the device_pbmp with the ports in pipe_local_pbmp[tbl_inst]*/
        BCMDRD_PBMP_OR(*device_pbmp, pipe_local_pbmp[tbl_inst]);
    }

exit:
    SHR_FUNC_EXIT();
}

