/*! \file bcmfp_keygen_qual_cfg.c
 *
 * Utility functions to operate on Group
 * qualifier configurations.
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

#include "bcmfp_keygen_md.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_COMMON

STATIC int
bcmfp_keygen_qual_cfg_info_free(int unit,
                             bcmfp_keygen_qual_cfg_info_t **qual_cfg_info)
{
    bcmfp_qual_t qual_id;

    SHR_FUNC_ENTER(unit);

    if (qual_cfg_info) {
        for (qual_id = 0; qual_id < BCMFP_QUAL_COUNT; qual_id++) {
            if (qual_cfg_info[qual_id]) {
                SHR_FREE(qual_cfg_info[qual_id]->qual_cfg_arr);
                SHR_FREE(qual_cfg_info[qual_id]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_qual_cfg_info_db_free(int unit,
                  bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db)
{
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_cfg_info_db, SHR_E_NONE);

    /* Free bus qids sec_cfg memory. */
    qual_cfg_info = qual_cfg_info_db->qual_cfg_info;
    bcmfp_keygen_qual_cfg_info_free(unit, qual_cfg_info);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_md_free(int unit,
                     bcmfp_keygen_cfg_t *keygen_cfg,
                     bcmfp_keygen_md_t *keygen_md)
{
    uint8_t level = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_NONE);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_NONE);

    SHR_FREE(keygen_md->qual_cfg_id_arr);
    SHR_FREE(keygen_md->pmux_info);

    (void)bcmfp_keygen_qual_cfg_info_free(unit, keygen_md->qual_cfg_info);
    for (level = 0; level < BCMFP_KEYGEN_EXT_LEVEL_COUNT; level++) {
        if (keygen_md->qual_cfg_info_db[level] == NULL) {
            continue;
        }
        (void)bcmfp_keygen_qual_cfg_info_db_free(unit,
                         keygen_md->qual_cfg_info_db[level]);
         SHR_FREE(keygen_md->qual_cfg_info_db[level]);
    }

    if (keygen_md->ext_cfg_db != NULL) {
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_ext_cfg_db_free(unit, keygen_md->ext_cfg_db));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_qual_cfg_is_valid(int unit,
                            bcmfp_qual_t qual_id,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            bcmfp_keygen_qual_cfg_t *qual_cfg,
                            uint16_t qual_cfg_max_size,
                            uint8_t *qual_cfg_is_valid)
{
    uint8_t idx = 0;
    uint8_t qual_cfg_chunk_id = 0;
    uint16_t bit = 0;
    uint16_t start_bit = 0;
    uint16_t end_bit = 0;
    uint16_t width = 0;
    uint32_t *qual_bitmap = NULL;
    uint32_t qual_bitmap1[BCMFP_KEYGEN_QUAL_BITMAP_MAX_WORDS] = {0};
    uint32_t qual_bitmap2[BCMFP_KEYGEN_QUAL_BITMAP_MAX_WORDS] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_cfg_is_valid, SHR_E_PARAM);

    *qual_cfg_is_valid = 0;

    if (keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE) {
       if (SHR_BITGET(qual_cfg->qual_flags.w,
                      BCMFP_KEYGEN_QUAL_FLAGS_NOT_IN_SINGLE)) {
           SHR_RETURN_VAL_EXIT(SHR_E_NONE);
       }
    }

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        if (keygen_cfg->qual_info_arr[idx].qual_id == qual_id &&
            keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            qual_bitmap = keygen_cfg->qual_info_arr[idx].bitmap;
            break;
        }
    }

    if (qual_bitmap == NULL) {
       sal_memset(qual_bitmap2, 0, sizeof(qual_bitmap2));
       for (bit = 0; bit < qual_cfg_max_size; bit++) {
           SHR_BITSET(qual_bitmap2, bit);
       }
       qual_bitmap = qual_bitmap2;
    }

    start_bit = 0;
    end_bit = 0;
    width = 0;
    sal_memset(qual_bitmap1, 0, sizeof(qual_bitmap1));
    for (qual_cfg_chunk_id = 0;
         qual_cfg_chunk_id < qual_cfg->num_chunks;
         qual_cfg_chunk_id++) {
        start_bit += width;
        width = qual_cfg->e_params[qual_cfg_chunk_id].width;
        end_bit += width;
        for (bit = start_bit; bit < end_bit; bit++) {
            SHR_BITSET(qual_bitmap1, bit);
        }
    }

    *qual_cfg_is_valid = TRUE;
    SHR_BIT_ITER(qual_bitmap, qual_cfg_max_size - 1, bit) {
       if (!SHR_BITGET(qual_bitmap1, bit)) {
          *qual_cfg_is_valid = FALSE;
          break;
       }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_qual_cfg_info_alloc(int unit,
                              bcmfp_keygen_cfg_t *keygen_cfg,
                              bcmfp_keygen_md_t *keygen_md)
{
    uint8_t qual_cfg_is_valid = FALSE;
    uint8_t qual_cfg_count = 0;
    uint8_t valid_qual_cfg_count = 0;
    uint8_t qual_cfg_id = 0;
    uint16_t idx = 0;
    uint16_t qual_cfg_max_size = 0;
    uint32_t qual_cfg_id_bmp = 0;
    bcmfp_qual_t qual_id;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info1 = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info2 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    qual_cfg_info1 = keygen_cfg->qual_cfg_info_db->qual_cfg_info;

    SHR_IF_ERR_EXIT((qual_cfg_info1 == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {

        qual_id =  keygen_cfg->qual_info_arr[idx].qual_id;

        if (qual_cfg_info1[qual_id] == NULL) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_qual_cfg_max_size_get(unit,
                                        keygen_cfg,
                                        qual_id,
                                        &qual_cfg_max_size));

        valid_qual_cfg_count = 0;
        qual_cfg_id_bmp = 0;
        for (qual_cfg_id = 0;
             qual_cfg_id < qual_cfg_info1[qual_id]->num_qual_cfg;
             qual_cfg_id++) {

            qual_cfg = &(qual_cfg_info1[qual_id]->qual_cfg_arr[qual_cfg_id]);
            qual_cfg_is_valid = FALSE;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_qual_cfg_is_valid(unit,
                                            qual_id,
                                            keygen_cfg,
                                            qual_cfg,
                                            qual_cfg_max_size,
                                            &qual_cfg_is_valid));

            if (qual_cfg_is_valid == TRUE) {
                SHR_BITSET(&qual_cfg_id_bmp, qual_cfg_id);
                valid_qual_cfg_count++;
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            (valid_qual_cfg_count == 0) ? SHR_E_CONFIG : SHR_E_NONE);

        qual_cfg_info2 = NULL;
        BCMFP_ALLOC(qual_cfg_info2,
                  sizeof(bcmfp_keygen_qual_cfg_info_t),
                  "BCMFP_KEYGEN: Group Qual Config Information");
        keygen_md->qual_cfg_info[qual_id] = qual_cfg_info2;

        qual_cfg_arr = NULL;
        BCMFP_ALLOC(qual_cfg_arr,
                  valid_qual_cfg_count * sizeof(bcmfp_keygen_qual_cfg_t),
                  "BCMFP_KEYGEN:Group Qual Configuration array");
        qual_cfg_info2->num_qual_cfg = valid_qual_cfg_count;
        qual_cfg_info2->qual_cfg_arr = qual_cfg_arr;

        qual_cfg_count = 0;
        for (qual_cfg_id = 0;
             qual_cfg_id < qual_cfg_info1[qual_id]->num_qual_cfg;
             qual_cfg_id++) {

            qual_cfg =
                &(qual_cfg_info1[qual_id]->qual_cfg_arr[qual_cfg_id]);

            if (SHR_BITGET(&qual_cfg_id_bmp, qual_cfg_id)) {
                qual_cfg =
                    &(qual_cfg_info1[qual_id]->qual_cfg_arr[qual_cfg_id]);
                sal_memcpy(&(qual_cfg_arr[qual_cfg_count]),
                           qual_cfg, sizeof(bcmfp_keygen_qual_cfg_t));
                qual_cfg_count++;
            }
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmfp_keygen_qual_cfg_info_free(unit, keygen_md->qual_cfg_info);
    }

    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_qual_cfg_info_db_alloc(int unit,
                                 bcmfp_keygen_cfg_t *keygen_cfg,
                                 bcmfp_keygen_md_t *keygen_md)
{
    uint8_t level = 0;
    uint16_t idx = 0;
    bcmfp_qual_t qual_id;
    bcmfp_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (level = 0; level < BCMFP_KEYGEN_EXT_LEVEL_COUNT; level++) {
        keygen_md->qual_cfg_info_db[level] = NULL;
    }

    BCMFP_ALLOC(keygen_md->qual_cfg_info_db[0],
                sizeof(bcmfp_keygen_qual_cfg_info_db_t),
                "BCMFP_KEYGEN:Group Qualifer Config Information DB");

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        if (keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id] == NULL) {
            continue;
        }
        BCMFP_ALLOC(qual_cfg_info,
                    sizeof(bcmfp_keygen_qual_cfg_info_t),
                    "BCMFP_KEYGEN:Group Qual Config Information");
        keygen_md->qual_cfg_info_db[0]->qual_cfg_info[qual_id] = qual_cfg_info;
        BCMFP_ALLOC(qual_cfg_arr,
                    sizeof(bcmfp_keygen_qual_cfg_t),
                    "BCMFP_KEYGEN:Group Qual Config");
        qual_cfg_info->qual_cfg_arr = qual_cfg_arr;
        qual_cfg_info->num_qual_cfg = 1;
        qual_cfg_info = NULL;
        qual_cfg_arr = NULL;
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmfp_keygen_qual_cfg_info_db_free(unit,
                          keygen_md->qual_cfg_info_db[0]);
        SHR_FREE(keygen_md->qual_cfg_info_db[0]);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_qual_cfg_id_array_alloc(int unit,
                                  bcmfp_keygen_cfg_t *keygen_cfg,
                                  bcmfp_keygen_md_t *keygen_md)
{
    uint16_t idx = 0;
    bcmfp_qual_t qual_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    BCMFP_ALLOC(keygen_md->qual_cfg_id_arr,
                sizeof(uint8_t) * keygen_cfg->qual_info_count,
                "Group Qual Config ID Tracker");

    keygen_md->qual_cfg_combinations = 1;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        keygen_md->qual_cfg_combinations *=
              keygen_md->qual_cfg_info[qual_id]->num_qual_cfg;
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_acl_attrs_update(int unit,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_md_t *keygen_md)
{
    uint8_t part = 0;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    if (keygen_cfg->ext_ctrl_sel_info == NULL ||
        keygen_cfg->ext_ctrl_sel_info_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    for (idx = 0; idx < keygen_cfg->ext_ctrl_sel_info_count; idx++) {
        part = keygen_cfg->ext_ctrl_sel_info[idx].part;
        if (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel ==
            BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            SHR_BITSET(keygen_md->acl_attrs[part].w,
                       BCMFP_KEYGEN_ACL_ATTR_PMUX_IPBM);
        }
        if (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel ==
            BCMFP_KEYGEN_EXT_CTRL_SEL_PMUX) {
            switch (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel_val) {
                case 0:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_DROP);
                    break;
                case 1:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED);
                    break;
                case 2:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID);
                    break;
                case 3:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT);
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT);
                    break;
                case 4:
                    if (keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE) {
                        SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM);
                    } else {
                        SHR_BITSET(keygen_md->acl_attrs[part].w,
                                  BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB);
                    }
                    break;
                case 12:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM);
                    break;
                case 14:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0);
                    break;
                case 11:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM);
                    break;
                case 13:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1);
                    break;
                case 8:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
                    break;
                case 9:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
                    break;
                case 10:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
                    break;
                case 5:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3);
                    break;
                case 6:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7);
                    break;
                case 7:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11);
                    break;
                default:
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_acl_attrs_init(int unit,
                           bcmfp_keygen_cfg_t *keygen_cfg,
                           bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t chunk = 0;
    bcmfp_keygen_ext_section_t pmux_section;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_pmux_info_t *pmux_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    if ((keygen_cfg->flags & BCMFP_KEYGEN_CFG_FLAGS_QSET_UPDATE)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_acl_attrs_update(unit, keygen_cfg, keygen_md));
    }

    for (idx = 0; idx < keygen_md->num_pmux_qual; idx++) {
        pmux_info = &(keygen_md->pmux_info[idx]);
        qual_cfg = pmux_info->pmux_qual_cfg;
        for (chunk = 0; chunk < qual_cfg->num_chunks; chunk++) {
            pmux_section = qual_cfg->e_params[chunk].section;
            if (pmux_section ==
                BCMFP_KEYGEN_EXT_SECTION_IPBM) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_IPBM);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_DROP) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_DROP);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_NAT_NEEDED) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_NAT_DST_REALMID) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0 &&
                       keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1 &&
                       keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB &&
                       keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_0_3) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_4_7) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_8_11) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_0_3) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_4_7) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_8_11) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_A) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_A);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_B) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_B);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
            } else if (pmux_section ==
                     BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_PKT_RESOLUTION) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMFP_KEYGEN_ACL_ATTR_PMUX_PKT_RESOLUTION);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_C) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_C);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_D) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_D);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_INT_PRI) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMFP_KEYGEN_ACL_ATTR_PMUX_INT_PRI);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_IFP_DROP_VECTOR) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMFP_KEYGEN_ACL_ATTR_PMUX_IFP_DROP_VECTOR);
            } else if (pmux_section ==
                       BCMFP_KEYGEN_EXT_SECTION_MH_OPCODE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMFP_KEYGEN_ACL_ATTR_PMUX_MH_OPCODE);
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            continue;
        }
    }

    if (keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE) {
        SHR_BITSET(keygen_md->acl_attrs[0].w, BCMFP_KEYGEN_ACL_ATTR_MODE_HALF);
    }

    if (keygen_cfg->mode == BCMFP_KEYGEN_MODE_SINGLE_ASET_WIDE) {
        SHR_BITSET(keygen_md->acl_attrs[0].w,
                   BCMFP_KEYGEN_ACL_ATTR_MODE_SINGLE_ASET_WIDE);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_md_alloc(int unit,
                   bcmfp_keygen_cfg_t *keygen_cfg,
                   bcmfp_keygen_md_t **keygen_md)
{
    uint8_t part = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    BCMFP_ALLOC(*keygen_md, sizeof(bcmfp_keygen_md_t), "Group MetaData");

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_qual_cfg_info_alloc(unit, keygen_cfg, *keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_qual_cfg_info_db_alloc(unit, keygen_cfg, *keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_ext_cfg_alloc(unit, keygen_cfg, *keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_qual_cfg_id_array_alloc(unit, keygen_cfg, *keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_ext_sections_get(unit, keygen_cfg, *keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_pmux_process(unit, keygen_cfg, *keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_acl_attrs_init(unit, keygen_cfg, *keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_ext_section_gran_info_init(unit, *keygen_md));

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_codes_init(unit, &((*keygen_md)->ext_codes[part])));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_cfg_md_process(int unit,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_oper_t *keygen_oper,
                         bcmfp_keygen_md_t *keygen_md)
{
    uint8_t qual_cfg_id = 0;
    uint8_t *qual_cfg_id_arr = NULL;
    uint16_t idx = 0;
    uint32_t qual_cfg_combinations = 0;
    uint32_t qual_cfg_combinations_tried = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmfp_qual_t qual_id = BCMFP_QUAL_COUNT;
    bcmfp_keygen_qual_cfg_t *qual_cfg1 = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg2 = NULL;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info = NULL;
    bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    qual_cfg_info = keygen_md->qual_cfg_info;
    qual_cfg_info_db = keygen_md->qual_cfg_info_db[0];
    qual_id = keygen_cfg->qual_info_arr[0].qual_id;
    qual_cfg_id_arr = keygen_md->qual_cfg_id_arr;
    qual_cfg_combinations = keygen_md->qual_cfg_combinations;

    while (TRUE) {

        for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
            qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
            qual_cfg1 = qual_cfg_info_db->qual_cfg_info[qual_id]->qual_cfg_arr;
            qual_cfg_id = qual_cfg_id_arr[idx];
            qual_cfg2 = &(qual_cfg_info[qual_id]->qual_cfg_arr[qual_cfg_id]);
            sal_memcpy(qual_cfg1, qual_cfg2, sizeof(bcmfp_keygen_qual_cfg_t));
        }

        rv = bcmfp_keygen_ext_alloc(unit, keygen_cfg, keygen_oper, keygen_md);
        if (SHR_FAILURE(_func_rv) && (_func_rv != SHR_E_RESOURCE)) {
            SHR_IF_ERR_VERBOSE_EXIT(_func_rv);
        } else if (SHR_SUCCESS(rv)) {
           break;
        }

        qual_cfg_combinations_tried++;

        /* all combinations are tried , but still no success. */
        if (qual_cfg_combinations_tried == qual_cfg_combinations) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        /* Identify the enxt configuration of each qualifer to try with. */
        qual_id = keygen_cfg->qual_info_arr[0].qual_id;

        if (qual_cfg_id_arr[0] ==
            qual_cfg_info[qual_id]->num_qual_cfg - 1) {

           qual_cfg_id_arr[0] = 0;

           idx = 1;
           while (idx < keygen_cfg->qual_info_count) {
               qual_id = keygen_cfg->qual_info_arr[idx].qual_id;

               if (qual_cfg_id_arr[idx] <
                   qual_cfg_info[qual_id]->num_qual_cfg - 1) {
                   qual_cfg_id_arr[idx]++;
                   break;
               } else {
                   qual_cfg_id_arr[idx] = 0;
               }
               idx++;
           }
        } else {
           qual_cfg_id_arr[0]++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_qual_cfg_max_size_get(int unit,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            bcmfp_qual_t qual_id,
                            uint16_t *qual_cfg_max_size)
{
    uint8_t qual_cfg_id = 0;
    uint8_t qual_cfg_chunk_id = 0;
    uint16_t qual_cfg_size = 0;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_cfg_max_size, SHR_E_PARAM);

    *qual_cfg_max_size = 0;

    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id];

    for (qual_cfg_id = 0;
         qual_cfg_id < qual_cfg_info->num_qual_cfg;
         qual_cfg_id++) {
         qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
         qual_cfg_size = 0;
         for (qual_cfg_chunk_id = 0;
              qual_cfg_chunk_id < qual_cfg->num_chunks;
              qual_cfg_chunk_id++) {
              qual_cfg_size += qual_cfg->e_params[qual_cfg_chunk_id].width;
         }

         if (*qual_cfg_max_size < qual_cfg_size) {
             *qual_cfg_max_size = qual_cfg_size;
         }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_keysize_get(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        uint16_t *rkey_size)
{
    int key_size = 0;
    uint8_t chunk = 0;
    uint16_t idx = 0;
    bcmfp_qual_t qual_id;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(rkey_size, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    *rkey_size = 0;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id];
        if (qual_cfg_info == NULL) {
            continue;
        }
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[0]);
        if (qual_cfg == NULL) {
            continue;
        }
        if (qual_cfg->pri_ctrl_sel != BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            continue;
        }
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            key_size = 0;
            SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap, key_size,
                         0, sizeof(uint32_t) * BCMFP_KEYGEN_QUAL_BITMAP_MAX_WORDS);
            *rkey_size += key_size;
        } else {
            for (chunk = 0; chunk < qual_cfg->num_chunks; chunk++) {
                *rkey_size += qual_cfg->e_params[chunk].width;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_oper_free(int unit,
                      uint16_t num_parts,
                      bcmfp_keygen_oper_t *keygen_oper)
{
    uint16_t part = 0;
    bcmfp_keygen_qual_offset_info_t *offset_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (keygen_oper == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (keygen_oper->qual_offset_info == NULL &&
        keygen_oper->ext_ctrl_sel_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    }

    if (keygen_oper->qual_offset_info != NULL) {
        for (part = 0; part < num_parts; part++) {
            offset_info = &(keygen_oper->qual_offset_info[part]);
            SHR_FREE(offset_info->qid_arr);
            SHR_FREE(offset_info->offset_arr);
        }
        SHR_FREE(keygen_oper->qual_offset_info);
    }

    if (keygen_oper->ext_ctrl_sel_info != NULL) {
        SHR_FREE(keygen_oper->ext_ctrl_sel_info);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_qual_cfg_t_init(int unit, bcmfp_keygen_qual_cfg_t *qual_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_cfg, SHR_E_PARAM);

    sal_memset(qual_cfg, 0, sizeof(bcmfp_keygen_qual_cfg_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_qual_cfg_insert(int unit,
                 bcmfp_qual_t qual_id,
                 bcmfp_keygen_qual_cfg_t *new_qual_cfg,
                 bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db)
{
    uint8_t qual_cfg_info_db_alloc = 0;
    uint8_t qual_cfg_info_alloc = 0;
    uint8_t chunk = 0;
    uint8_t num_qual_cfg = 0;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(new_qual_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_cfg_info_db, SHR_E_PARAM);

    qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual_id];
    if (qual_cfg_info == NULL) {
        BCMFP_ALLOC(qual_cfg_info,
                    sizeof(bcmfp_keygen_qual_cfg_info_t),
                    "BCMFP_KEYGEN:Qualifier Config Info");
        qual_cfg_info_db->qual_cfg_info[qual_id] = qual_cfg_info;
        qual_cfg_info->qual = qual_id;
        qual_cfg_info_alloc = 1;
    }

    num_qual_cfg = qual_cfg_info->num_qual_cfg + 1;

    BCMFP_ALLOC(qual_cfg_arr,
                num_qual_cfg * sizeof(bcmfp_keygen_qual_cfg_t),
                "BCMFP_KEYGEN:Qualifier config");

    sal_memcpy(qual_cfg_arr,
               qual_cfg_info->qual_cfg_arr,
               sizeof(bcmfp_keygen_qual_cfg_t) * (num_qual_cfg - 1));

    new_qual_cfg->size = 0;
    qual_cfg = &(qual_cfg_arr[num_qual_cfg - 1]);
    new_qual_cfg->size = 0;

    for (chunk = 0; chunk < new_qual_cfg->num_chunks; chunk++) {
        if (BCMFP_KEYGEN_EXT_SECTION_L1E32 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 32;
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E16 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 16;
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E8 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 8;
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E4 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 4;
        } else if (BCMFP_KEYGEN_EXT_SECTION_L1E2 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 2;
        }
        new_qual_cfg->size += new_qual_cfg->e_params[chunk].width;
    }

    sal_memcpy(qual_cfg, new_qual_cfg, sizeof(bcmfp_keygen_qual_cfg_t));

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
bcmfp_keygen_cfg_process(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        bcmfp_keygen_oper_t *keygen_oper)
{

    bcmfp_keygen_md_t *keygen_md = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    if (keygen_cfg->qual_info_count == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_md_alloc(unit, keygen_cfg, &keygen_md));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_cfg_md_process(unit, keygen_cfg, keygen_oper, keygen_md));

exit:
    (void)bcmfp_keygen_md_free(unit, keygen_cfg, keygen_md);
    SHR_FREE(keygen_md);
    SHR_FUNC_EXIT();
}
