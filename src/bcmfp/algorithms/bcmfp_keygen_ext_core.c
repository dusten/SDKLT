/*! \file bcmfp_keygen_ext_core.c
 *
 * Core Algorithmic APIS to allocate extractors.
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

#define BCMFP_KEYGEN_EXT_FINFO_COUNT (1024 * 1)

bcmfp_keygen_ext_field_info_t *finfo_free_list[BCMDRD_CONFIG_MAX_UNITS];

bcmfp_keygen_ext_field_info_t finfo_pool[BCMDRD_CONFIG_MAX_UNITS]
                                 [BCMFP_KEYGEN_EXT_FINFO_COUNT];

static uint8_t debug_enable = 0;
static uint16_t finfo_free_count = 0;
static uint16_t finfo_used_count = 0;

#define DUMP_FINFO_COUNT(free) {  \
    if (free) {                   \
        if (debug_enable) {       \
            finfo_free_count++;   \
            finfo_used_count--;   \
        }                         \
    } else {                      \
        if (debug_enable) {       \
            finfo_free_count--;   \
            finfo_used_count++;   \
        }                         \
    }                             \
}

STATIC int
bcmfp_keygen_bit_stream_get(int unit,
                     SHR_BITDCL *buffer,
                     uint16_t start_bit,
                     uint16_t size,
                     uint16_t  *bs_count,
                     uint16_t *bs_offset,
                     uint16_t *bs_width)
{
    int offset = -1;
    uint16_t bit = 0;
    uint16_t width = 0;
    uint16_t end_bit = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(bs_count, SHR_E_PARAM);
    SHR_NULL_CHECK(bs_width, SHR_E_PARAM);
    SHR_NULL_CHECK(bs_offset, SHR_E_PARAM);

    width = 0;
    offset = -1;
    *bs_count = 0;
    end_bit = start_bit + size;
    for (bit = start_bit; bit < end_bit; bit++)  {
        if (SHR_BITGET(buffer, bit)) {
           if (offset == -1) {
               offset = bit;
           }
           width++;
           if (bit == end_bit - 1) {
               bs_offset[*bs_count] = offset;
               bs_width[*bs_count] = width;
               (*bs_count)++;
           }
        } else {
           if (offset != -1) {
               bs_offset[*bs_count] = offset;
               bs_width[*bs_count] = width;
               (*bs_count)++;
               offset = -1;
               width = 0;
           }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

#define ORDER_TYPE_ASCENDING     (1 << 0)
#define ORDER_TYPE_DESCENDING    (1 << 1)
#define ORDER_TYPE_END_OF_LIST   (1 << 2)
#define ORDER_TYPE_START_OF_LIST (1 << 3)

STATIC int
bcmfp_keygen_ext_finfo_node_add(int unit,
                  uint8_t order_type,
                  bcmfp_keygen_ext_field_info_t *in_node,
                  bcmfp_keygen_ext_field_info_t **head_node)
{
    bcmfp_keygen_ext_field_info_t *temp_node1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_node2 = NULL;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in_node, SHR_E_PARAM);
    SHR_NULL_CHECK(head_node, SHR_E_PARAM);

    in_node->next = NULL;
    if (*head_node == NULL) {
        *head_node = in_node;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if(order_type == ORDER_TYPE_ASCENDING) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    } else if(order_type == ORDER_TYPE_DESCENDING) {
        temp_node1 = temp_node2 = *head_node;
        while (temp_node1 != NULL) {
             if (temp_node1->req_bit_count <
                 in_node->req_bit_count) {
                 in_node->next = temp_node1;
                 if (temp_node1 == temp_node2) {
                     *head_node = in_node;
                     break;
                 }
                 temp_node2->next = in_node;
                 break;
             }
             temp_node2 = temp_node1;
             temp_node1 = temp_node1->next;
        }
        if (temp_node1 == NULL) {
            in_node->next = NULL;
            temp_node2->next = in_node;
        }
    } else if (order_type == ORDER_TYPE_END_OF_LIST) {
        temp_node1 = *head_node;
        while (temp_node1->next != NULL) {
            temp_node1 = temp_node1->next;
        }
        temp_node1->next = in_node;
    } else if (order_type == ORDER_TYPE_START_OF_LIST) {
        in_node->next = *head_node;
        *head_node = in_node;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC void
bcmfp_keygen_ext_finfo_release(int unit,
                             bcmfp_keygen_ext_field_info_t *finfo)
{
    bcmfp_keygen_ext_field_info_t *temp_finfo = NULL;

    while (finfo != NULL) {
        temp_finfo = finfo;
        finfo = finfo->next;
        sal_memset(temp_finfo, 0, sizeof(bcmfp_keygen_ext_field_info_t));
        temp_finfo->next = finfo_free_list[unit];
        finfo_free_list[unit] = temp_finfo;
        DUMP_FINFO_COUNT(1);
    }
}

STATIC int
bcmfp_keygen_ext_fields_get_last(int unit,
                              uint8_t part,
                              uint8_t level,
                              bcmfp_keygen_ext_section_t section,
                              uint8_t sec_val,
                              bcmfp_keygen_ext_ctrl_sel_t ctrl_sel,
                              uint16_t ctrl_sel_val,
                              bcmfp_keygen_md_t *keygen_md,
                              bcmfp_keygen_ext_field_info_t **finfo_list)
{
    uint8_t ext_part = 0;
    uint32_t ext_idx = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo3 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo4 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(finfo_list, SHR_E_PARAM);

    *finfo_list = NULL;
    for (ext_idx = 0;
         ext_idx < keygen_md->ext_cfg_db->conf_size[level];
         ext_idx++) {
         ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
         ext_part = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
         if (ext_part != part || ext_cfg->in_use == FALSE) {
            continue;
         }
         if ((temp_finfo1 = ext_cfg->finfo) == NULL) {
            continue;
         }
         if (section == temp_finfo1->l0_section &&
             sec_val == temp_finfo1->l0_sec_val &&
             ctrl_sel == temp_finfo1->pri_ctrl_sel &&
             ctrl_sel_val == temp_finfo1->pri_ctrl_sel_val) {
             temp_finfo2 = finfo_free_list[unit];
             if (temp_finfo2 == NULL)  {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             }
             finfo_free_list[unit] = temp_finfo2->next;
             DUMP_FINFO_COUNT(0);
             sal_memcpy(temp_finfo2, temp_finfo1,
                        sizeof(bcmfp_keygen_ext_field_info_t));
             temp_finfo2->next = NULL;
             /* New node is the first node in the list. */
             if (temp_finfo_list == NULL) {
                temp_finfo_list = temp_finfo2;
                continue;
             }
             /* New node has to be at the head_node of the list. */
             temp_finfo3 = temp_finfo_list;
             if (temp_finfo3->l0_field_offset >
                 temp_finfo2->l0_field_offset) {
                 temp_finfo2->next = temp_finfo3;
                 temp_finfo_list = temp_finfo2;
                 continue;
             }
             /* New node has to be some where in the middle of the list. */
             temp_finfo4 = temp_finfo_list;
             while (temp_finfo3 != NULL) {
                 if (temp_finfo3->l0_field_offset >
                     temp_finfo2->l0_field_offset) {
                     temp_finfo4->next = temp_finfo2;
                     temp_finfo2->next = temp_finfo3;
                     break;
                 }
                 temp_finfo4 = temp_finfo3;
                 temp_finfo3 = temp_finfo3->next;
             }
             /* New node has to be at the end of the list. */
            if (temp_finfo3 == NULL) {
               temp_finfo4->next = temp_finfo2;
            }
        }
    }
    *finfo_list = temp_finfo_list;

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_qual_offset_get(int unit,
                          bcmfp_keygen_cfg_t *keygen_cfg,
                          bcmfp_keygen_oper_t *keygen_oper,
                          bcmfp_keygen_md_t *keygen_md,
                          bcmfp_qual_t qual_id,
                          bcmfp_keygen_qual_cfg_t *qual_cfg)
{
    int bit_count = 0;
    uint8_t idx = 0;
    uint8_t f_idx = 0;
    uint8_t bs_idx = 0;
    uint8_t qid_idx = 0;
    uint8_t finfo_idx = 0;
    uint8_t offset_idx = 0;
    uint8_t part = 0;
    uint8_t level = 0;
    uint8_t sec_val = 0;
    uint8_t f_width = 0;
    uint8_t q_bit = 0;
    uint8_t f_bit = 0;
    uint16_t f_offset = 0;
    uint16_t ctrl_sel_val = 0;
    uint16_t bs_count = 0;
    uint16_t field_offset = 0;
    uint16_t field_width = 0;
    uint16_t bs_offset[BCMFP_KEYGEN_EXT_FIELD_OFFSET_COUNT] = { 0 };
    uint16_t bs_width[BCMFP_KEYGEN_EXT_FIELD_OFFSET_COUNT] = { 0 };
    uint32_t req_bmp[BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT] = { 0 };
    uint32_t ext_bmp[BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT] = { 0 };
    uint8_t qid_found[BCMFP_KEYGEN_ENTRY_PARTS_MAX] = { FALSE };
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t *finfo_list = NULL;
    bcmfp_keygen_qual_offset_t *offset_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_cfg, SHR_E_PARAM);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        if (keygen_cfg->qual_info_arr[idx].qual_id == qual_id) {
            break;
        }
    }
    SHR_IF_ERR_EXIT(
        (idx == keygen_cfg->qual_info_count) ? SHR_E_INTERNAL : SHR_E_NONE);
    for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
        section = qual_cfg->e_params[f_idx].section;
        sec_val = qual_cfg->e_params[f_idx].sec_val;
        ctrl_sel = qual_cfg->pri_ctrl_sel;
        ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            field_width = qual_cfg->e_params[f_idx].width;
            bit_count = 0;
            SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                               bit_count, field_offset, field_width);
            field_offset += qual_cfg->e_params[f_idx].width;
            if (bit_count == 0) {
                continue;
            }
        }
        for (finfo_idx = 0;
             finfo_idx < BCMFP_KEYGEN_EXT_FIELDS_MAX;
             finfo_idx++) {
             if (section == keygen_md->finfo[finfo_idx].l0_section &&
                 sec_val == keygen_md->finfo[finfo_idx].l0_sec_val &&
                 ctrl_sel == keygen_md->finfo[finfo_idx].pri_ctrl_sel &&
                 ctrl_sel_val == keygen_md->finfo[finfo_idx].pri_ctrl_sel_val) {
                 finfo = &(keygen_md->finfo[finfo_idx]);
                 break;
             }
        }
        SHR_IF_ERR_EXIT((finfo == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);
        part = finfo->part;
        qid_idx = keygen_oper->qual_offset_info[part].size;
        offset_info = &(keygen_oper->qual_offset_info[part].offset_arr[qid_idx]);
        keygen_oper->qual_offset_info[part].qid_arr[qid_idx] = qual_id;
        if (qid_found[part] == FALSE) {
            qid_found[part] = TRUE;
        }
        level = keygen_md->num_ext_levels;
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_fields_get_last(unit, part, level,
                                          section, sec_val,
                                          ctrl_sel, ctrl_sel_val,
                                          keygen_md, &finfo_list));
        f_offset = (qual_cfg->e_params[f_idx].offset %
                    qual_cfg->e_params[f_idx].size);
        f_width = qual_cfg->e_params[f_idx].width;
        sal_memset(req_bmp, 0x0, sizeof(req_bmp));
        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            q_bit = field_offset - f_width;
            for (f_bit = f_offset;
                 f_bit < (f_offset + f_width);
                 f_bit++) {
                if (SHR_BITGET(
                    keygen_cfg->qual_info_arr[idx].bitmap, q_bit)) {
                    SHR_BITSET(req_bmp, f_bit);
                }
                q_bit++;
            }
        } else {
            SHR_BITSET_RANGE(req_bmp, f_offset, f_width);
        }

        finfo = finfo_list;
        while (finfo != NULL) {
            ext_cfg = finfo->ext_cfg;
            SHR_BITCOPY_RANGE(ext_bmp, finfo->l0_field_offset,
                             finfo->req_bmp, 0, finfo->size);
            SHR_BITAND_RANGE(ext_bmp, req_bmp, 0,
                             sizeof(uint32_t) * BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT * 8,
                             ext_bmp);
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_bit_stream_get(unit, ext_bmp,
                                     finfo->l0_field_offset,
                                     finfo->size, &bs_count,
                                     bs_offset, bs_width));
            for (bs_idx = 0; bs_idx < bs_count; bs_idx++) {
                offset_info->offset[offset_idx] = ext_cfg->offset +
                                                  bs_offset[bs_idx] -
                                                  finfo->l0_field_offset;
                offset_info->width[offset_idx] = bs_width[bs_idx];
                offset_info->num_offsets++;
                offset_idx++;
            }
            finfo = finfo->next;
        }
        if (finfo_list != NULL) {
            (void)bcmfp_keygen_ext_finfo_release(unit, finfo_list);
            finfo_list = NULL;
        }
    }

    for (part = 0; part < BCMFP_KEYGEN_ENTRY_PARTS_MAX; part++) {
        if (qid_found[part] == TRUE) {
             keygen_oper->qual_offset_info[part].size++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_qual_id_count_get(int unit,
                            bcmfp_keygen_md_t *keygen_md,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            uint8_t *qual_id_count)
{
    int bit_count = 0;
    uint8_t idx = 0;
    uint8_t f_idx = 0;
    uint8_t finfo_idx = 0;
    uint8_t part = 0;
    uint8_t sec_val = 0;
    uint8_t qual_cfg_id = 0;
    uint16_t ctrl_sel_val = 0;
    uint16_t field_offset = 0;
    uint16_t field_width = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_qual_t qual_id = BCMFP_QUAL_COUNT;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_keygen_pmux_info_t *pmux_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_id_count, SHR_E_PARAM);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_pmux_info_get(unit,
                                    qual_id,
                                    keygen_md->num_pmux_qual,
                                    keygen_md->pmux_info,
                                    &pmux_info));
            qual_id_count[pmux_info->pmux_part] += 1;
            continue;
        }
        field_offset = 0;
        for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
            section = qual_cfg->e_params[f_idx].section;
            sec_val = qual_cfg->e_params[f_idx].sec_val;
            ctrl_sel = qual_cfg->pri_ctrl_sel;
            ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                field_width = qual_cfg->e_params[f_idx].width;
                bit_count = 0;
                SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                                   bit_count, field_offset, field_width);
                field_offset += qual_cfg->e_params[f_idx].width;
                if (bit_count == 0) {
                    continue;
                }
            }
            for (finfo_idx = 0;
                finfo_idx < BCMFP_KEYGEN_EXT_FIELDS_MAX;
                finfo_idx++) {
                if (section == keygen_md->finfo[finfo_idx].l0_section &&
                    sec_val == keygen_md->finfo[finfo_idx].l0_sec_val &&
                    ctrl_sel == keygen_md->finfo[finfo_idx].pri_ctrl_sel &&
                    ctrl_sel_val == keygen_md->finfo[finfo_idx].pri_ctrl_sel_val) {
                    finfo = &(keygen_md->finfo[finfo_idx]);
                    break;
                }
            }
            SHR_IF_ERR_EXIT((finfo == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);
            part = finfo->part;
            qual_id_count[part]++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_qual_offset_create(int unit,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_oper_t *keygen_oper,
                             bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t part = 0;
    uint8_t parts_count = 0;
    uint8_t qual_cfg_id = 0;
    uint8_t qual_id_count[BCMFP_KEYGEN_ENTRY_PARTS_MAX];
    bcmfp_qual_t *qid_arr = NULL;
    bcmfp_keygen_qual_offset_t *offset_arr = NULL;
    bcmfp_qual_t qual_id = BCMFP_QUAL_COUNT;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_keygen_qual_offset_t *qual_offset = NULL;
    bcmfp_keygen_qual_offset_info_t *offset_info = NULL;
    bcmfp_keygen_pmux_info_t *pmux_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    sal_memset(qual_id_count, 0, sizeof(uint8_t) * BCMFP_KEYGEN_ENTRY_PARTS_MAX);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_qual_id_count_get(unit, keygen_md, keygen_cfg, qual_id_count));
    parts_count = keygen_cfg->num_parts;
    BCMFP_ALLOC(keygen_oper->qual_offset_info,
                      sizeof(bcmfp_keygen_qual_offset_info_t) * parts_count,
                      "Keygen operational qualifier offset information");
    for (part = 0; part < parts_count; part++) {
        qid_arr = NULL;
        offset_arr = NULL;
        BCMFP_ALLOC(qid_arr,
                    qual_id_count[part] * sizeof(bcmfp_qual_t),
                    "Group qual id array in each part");
        keygen_oper->qual_offset_info[part].qid_arr = qid_arr;
        BCMFP_ALLOC(offset_arr,
                    qual_id_count[part] * sizeof(bcmfp_keygen_qual_offset_t),
                    "Group qual offset info array in each part");
        keygen_oper->qual_offset_info[part].offset_arr = offset_arr;
    }

    /* Go through all qualifiers */
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_pmux_info_get(unit,
                                    qual_id,
                                    keygen_md->num_pmux_qual,
                                    keygen_md->pmux_info,
                                    &pmux_info));
            offset_info = &(keygen_oper->qual_offset_info[pmux_info->pmux_part]);
            qual_offset =  &(offset_info->offset_arr[offset_info->size]);
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_pmux_qual_offset_get(unit,
                                                 qual_id,
                                                 keygen_md,
                                                 keygen_cfg,
                                                 qual_offset));
            offset_info->qid_arr[offset_info->size] = qual_id;
           offset_info->size++;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_qual_offset_get(unit, keygen_cfg,
                                            keygen_oper, keygen_md,
                                            qual_id, qual_cfg));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_fields_get_first(int unit,
                               bcmfp_keygen_cfg_t *keygen_cfg,
                               bcmfp_keygen_md_t *keygen_md)
{
    int bit_count = 0;
    uint8_t idx = 0;
    uint8_t f_idx = 0;
    uint8_t i = 0, j = 0;
    uint8_t field_count = 0;
    uint8_t finfo_idx = 0;
    uint8_t f_width = 0;
    uint8_t sec_val = 0;
    uint8_t found = FALSE;
    uint8_t qual_cfg_id = 0;
    uint8_t q_bit = 0;
    uint8_t f_bit = 0;
    uint16_t f_offset = 0;
    uint16_t finfo_size = 0;
    uint16_t ctrl_sel_val = 0;
    uint16_t field_offset = 0;
    uint16_t field_width = 0;
    uint32_t qual_flags = 0;
    uint32_t finfo_flags = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t temp_finfo;
    bcmfp_qual_t qual_id = BCMFP_QUAL_COUNT;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    finfo_size = sizeof(bcmfp_keygen_ext_field_info_t) * BCMFP_KEYGEN_EXT_FIELDS_MAX;
    sal_memset(keygen_md->finfo, 0, finfo_size);
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_flags = keygen_cfg->qual_info_arr[idx].flags;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        field_offset = 0;
        for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
            section = qual_cfg->e_params[f_idx].section;
            sec_val = qual_cfg->e_params[f_idx].sec_val;
            ctrl_sel = qual_cfg->pri_ctrl_sel;
            ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                field_width = qual_cfg->e_params[f_idx].width;
                bit_count = 0;
                SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                                   bit_count, field_offset, field_width);
                field_offset += qual_cfg->e_params[f_idx].width;
                if (bit_count == 0) {
                    continue;
                }
            }
            for (finfo_idx = 0; finfo_idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; finfo_idx++) {
                 finfo = &(keygen_md->finfo[finfo_idx]);
                 if ((finfo->section == section &&
                      finfo->sec_val == sec_val &&
                      finfo->pri_ctrl_sel == ctrl_sel &&
                      finfo->pri_ctrl_sel_val == ctrl_sel_val)) {
                      found = TRUE;
                 }
                 /* Ignore 'found' if this flag is set */
                 if (qual_flags & BCMFP_KEYGEN_QUAL_F_USE_FINFO_NEW) {
                    found = FALSE;
                 }
                 if ((found == TRUE) ||
                    (finfo->section == BCMFP_KEYGEN_EXT_SECTION_DISABLE)) {
                     break;
                 }
                 continue;
            }
            finfo_flags = 0;
            finfo_flags |= (qual_flags & BCMFP_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16) ?
                                BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN16: 0;
            finfo_flags |= (qual_flags & BCMFP_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8) ?
                                BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN8: 0;
            if (found == FALSE) {
                finfo->size = qual_cfg->e_params[f_idx].size;
                finfo->section = section;
                finfo->sec_val = sec_val;
                finfo->pri_ctrl_sel = ctrl_sel;
                finfo->pri_ctrl_sel_val = ctrl_sel_val;
                finfo->part = -1;
                finfo->ext_bmp[0] = 0;
                finfo->l0_section = section;
                finfo->l0_sec_val = sec_val;
                finfo->l0_field_offset = 0;
                field_count++;
            }
            f_offset = (qual_cfg->e_params[f_idx].offset %
                        qual_cfg->e_params[f_idx].size);
            f_width = qual_cfg->e_params[f_idx].width;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                q_bit = field_offset - f_width;
                for (f_bit = f_offset;
                     f_bit < (f_offset + f_width);
                     f_bit++) {
                    if (SHR_BITGET(
                        keygen_cfg->qual_info_arr[idx].bitmap, q_bit)) {
                        SHR_BITSET(finfo->req_bmp, f_bit);
                    }
                    q_bit++;
                }
            } else {
                SHR_BITSET_RANGE(finfo->req_bmp, (int)f_offset, (int)f_width);
            }
            found = FALSE;
        }
    }

    /* Sort the finfo List */
    for (i = 0; i < field_count; i++) {
        for (j = (i + 1); j < field_count; j++) {
            if (keygen_md->finfo[j].size >
                keygen_md->finfo[i].size) {
                sal_memcpy(&temp_finfo, &keygen_md->finfo[i],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[i], &keygen_md->finfo[j],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[j], &temp_finfo,
                           sizeof(bcmfp_keygen_ext_field_info_t));
            }
        }
    }

    j = 0;
    field_width = keygen_md->finfo[0].size;
    for (i = 0; i < field_count; i++) {
        if (field_width != keygen_md->finfo[i].size) {
            field_width = keygen_md->finfo[i].size;
            j = i;
        }
        if (keygen_md->finfo[i].pri_ctrl_sel !=
            BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            if (j != i) {
                sal_memcpy(&temp_finfo, &keygen_md->finfo[j],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[j], &keygen_md->finfo[i],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[i], &temp_finfo,
                           sizeof(bcmfp_keygen_ext_field_info_t));
            }
            j++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_fields_validate(int unit,
                                uint8_t part,
                                uint8_t level,
                                bcmfp_keygen_ext_section_t section,
                                bcmfp_keygen_md_t *keygen_md)
{
    uint8_t equal = 0;
    uint8_t ext_part = 0;
    uint16_t ext_idx = 0;
    uint16_t bit_count = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    bit_count = BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT * sizeof(uint32_t) * 8;
    for (ext_idx = 0;
         ext_idx < keygen_md->ext_cfg_db->conf_size[level];
         ext_idx++) {
         ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
         ext_part =  BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
         if (ext_cfg->out_sec != section ||
             ext_cfg->in_use == FALSE ||
             ext_part != part) {
             continue;
         }
         temp_finfo1 = ext_cfg->finfo;
         if (temp_finfo1 == NULL) {
             if (SHR_BITGET(ext_cfg->ext_attrs.w,
                     BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                 continue;
             } else {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             }
         }
         equal = SHR_BITEQ_RANGE(temp_finfo1->req_bmp,
                                 temp_finfo1->ext_bmp,
                                 0, bit_count);
         if (!equal) {
             SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
         }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Notes:
 * Default behavior of this function is to allow any granularity
 * if finfo flags are not set
 */
STATIC int
bcmfp_keygen_finfo_flags_verify(int unit,
                          uint8_t level,
                          uint8_t gran,
                          bcmfp_keygen_ext_field_info_t *finfo)
{
    uint8_t g_idx = 0;
    uint8_t index = 0;
    uint8_t found = TRUE;
    uint8_t exp_level = 0;
    uint8_t exp_gran[BCMFP_KEYGEN_EXT_GRANULAR_ARR_SIZE] = {0};

    if (finfo->flags & BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN16) {
        exp_level = 1;
        exp_gran[index] = 16;
        index++;
    }
    if (finfo->flags & BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN8) {
        exp_level = 1;
        exp_gran[index] = 8;
        index++;
    }

    if ((exp_level == level) && (index != 0)) {
        found = FALSE;
        for (g_idx = 0; g_idx < index; g_idx++) {
            if (exp_gran[g_idx] == gran) {
                found = TRUE;
                break;
            }
        }
    }

    return found;
}

STATIC int
bcmfp_keygen_ext_fields_create(int unit,
                            uint8_t part,
                            uint8_t level,
                            bcmfp_keygen_ext_section_t section,
                            uint8_t gran,
                            uint8_t num_ext,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            bcmfp_keygen_md_t *keygen_md)
{
    int count = 0;
    uint8_t idx = 0;
    uint8_t chunk = 0;
    uint8_t num_chunks = 0;
    uint8_t order_type = 0;
    uint8_t flags_check = FALSE;
    uint16_t ext_idx = 0;
    uint16_t ext_offset = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo3 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo4 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (ext_idx = 0;
         ext_idx < keygen_md->ext_cfg_db->conf_size[level];
         ext_idx++) {
         ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
         if (ext_cfg->out_sec != section || ext_cfg->in_use == FALSE) {
             continue;
         }
         temp_finfo1 = ext_cfg->finfo;
         if (temp_finfo1 == NULL) {
             if (SHR_BITGET(ext_cfg->ext_attrs.w,
                     BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                 continue;
             } else {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             }
         }
         if (temp_finfo1->size < gran) {
             continue;
         }
         flags_check = bcmfp_keygen_finfo_flags_verify(unit,
                                    level, gran, temp_finfo1);
         if (flags_check == FALSE) {
            continue;
         }
         num_chunks = temp_finfo1->size / gran;
         ext_offset = ext_cfg->offset;
         for (chunk = 0; chunk < num_chunks; chunk++) {
             SHR_BITCOUNT_RANGE(temp_finfo1->req_bmp,
                                count, chunk * gran, gran);
             if (count == 0) {
                 continue;
             }
             if (SHR_BITEQ_RANGE(temp_finfo1->req_bmp,
                                 temp_finfo1->ext_bmp,
                                 chunk * gran, gran)) {
                 continue;
             }
             temp_finfo2 = finfo_free_list[unit];
             if (temp_finfo2 == NULL) {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             }
             finfo_free_list[unit] = temp_finfo2->next;
             DUMP_FINFO_COUNT(0);
             temp_finfo2->flags = temp_finfo1->flags;
             temp_finfo2->section = section;
             temp_finfo2->sec_val = (ext_offset + (chunk * gran)) / gran;
             temp_finfo2->pri_ctrl_sel = temp_finfo1->pri_ctrl_sel;
             temp_finfo2->pri_ctrl_sel_val = temp_finfo1->pri_ctrl_sel_val;
             temp_finfo2->size = gran;
             temp_finfo2->extracted = FALSE;
             temp_finfo2->req_bit_count = count;
             temp_finfo2->l0_section = temp_finfo1->l0_section;
             temp_finfo2->l0_sec_val = temp_finfo1->l0_sec_val;
             temp_finfo2->l0_field_offset =
                           temp_finfo1->l0_field_offset + (chunk * gran);
             SHR_BITCOPY_RANGE(temp_finfo2->req_bmp, 0,
                               temp_finfo1->req_bmp, chunk * gran, gran);
             SHR_BITCOPY_RANGE(temp_finfo1->ext_bmp, chunk * gran,
                               temp_finfo1->req_bmp, chunk * gran, gran);
             temp_finfo2->prev = temp_finfo1;
             temp_finfo2->prev_offset = chunk * gran;
             temp_finfo2->next = NULL;
             /* add the new finfo node to the list in the
              * sorted(descending) order.
              */
             sec_cfg = keygen_md->ext_cfg_db->sec_cfg[section];
             if (sec_cfg->flags & BCMFP_KEYGEN_EXT_SECTION_PASS_THRU) {
                 order_type = ORDER_TYPE_END_OF_LIST;
             } else {
                 order_type = ORDER_TYPE_DESCENDING;
             }
             SHR_IF_ERR_VERBOSE_EXIT(
                 bcmfp_keygen_ext_finfo_node_add(unit, order_type,
                                        temp_finfo2, &temp_finfo_list));
         }
    }

    /* Keep only num_ext number of nodes in the finfo list
     * and add the remaining nodes to free list.
     */
     temp_finfo3 = temp_finfo_list;
     temp_finfo2 = NULL;
     while (temp_finfo3 != NULL) {
         idx++;
         temp_finfo2 = temp_finfo3;
         temp_finfo3 = temp_finfo3->next;
         if (idx == num_ext) {
             break;
         }
     }
     while (temp_finfo3 != NULL) {
         temp_finfo4 = temp_finfo3;
         temp_finfo3 = temp_finfo3->next;
         SHR_BITCLR_RANGE(temp_finfo4->prev->ext_bmp,
                          temp_finfo4->prev_offset,
                          temp_finfo4->size);
         sal_memset(temp_finfo4, 0, sizeof(bcmfp_keygen_ext_field_info_t));
         temp_finfo4->next = finfo_free_list[unit];
         finfo_free_list[unit] = temp_finfo4;
         DUMP_FINFO_COUNT(1);
     }

     /* Add the list of chunks to the section in given level and part. */
     if (temp_finfo2 != NULL) {
         temp_finfo2->next = keygen_md->ext_finfo_db[part][level].finfo[section];
         keygen_md->ext_finfo_db[part][level].finfo[section] = temp_finfo_list;
     }

    SHR_FUNC_EXIT();
exit:

    SHR_FUNC_EXIT();
}

STATIC void
bcmfp_keygen_ext_section_finfo_reset(int unit,
                                    uint8_t part,
                                    uint8_t level,
                                    bcmfp_keygen_ext_section_t section,
                                    bcmfp_keygen_md_t *keygen_md)
{
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo = NULL;

    finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
    while(finfo != NULL) {
        temp_finfo = finfo;
        finfo = finfo->next;

        SHR_BITCLR_RANGE(temp_finfo->prev->ext_bmp,
                temp_finfo->prev_offset,
                temp_finfo->size);
    }

    (void)bcmfp_keygen_ext_finfo_release(unit,
            keygen_md->ext_finfo_db[part][level].finfo[section]);

    keygen_md->ext_finfo_db[part][level].finfo[section] = NULL;
}

/*
 * Note: This function assumes that in a section either single gran
 * extractor or multi-gran extractor are present and not both.
 * Also, It is assumed that only one multi-gran extractor is
 * present in the section. In future, if a section has both types
 * of extractors or more then one multi-gran extractor, then
 * function will be required to enhanced. Calcuate number of
 * combinations of fixed and variable gran extractors and try each
 * combination to extract required bits
 */
STATIC int
bcmfp_keygen_ext_fields_get_next(int unit,
                              uint8_t part,
                              uint8_t level,
                              bcmfp_keygen_cfg_t *keygen_cfg,
                              bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t gran = 0;
    uint8_t gran_count = 0;
    uint8_t sec_idx = 0;
    uint8_t num_ext = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_section_gran_t *gran_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (sec_idx = 0; sec_idx < BCMFP_KEYGEN_EXT_SECTION_COUNT; sec_idx++) {
        section = keygen_md->sections[part][level + 1][sec_idx];
        if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        gran_count =
           keygen_md->section_gran_info[section].out_gran_info_count;
        gran_info = keygen_md->section_gran_info[section].out_gran_info;
        for (idx = 0; idx < gran_count; idx++) {
            gran = gran_info[idx].gran;
            num_ext = gran_info[idx].num_extractors;
            /* Divide the unextracted bits of the section into chunks of
             * given granularity.
             */
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_ext_fields_create(unit, part, level + 1,
                                              section, gran, num_ext,
                                              keygen_cfg, keygen_md));
        }

        /* Multigran extractors */
        gran_count =
            keygen_md->section_gran_info[section].out_multigran_info_count;
        gran_info = keygen_md->section_gran_info[section].out_multigran_info;
        for (idx = 0; idx < gran_count; idx++) {
            gran = gran_info[idx].gran;
            num_ext = gran_info[idx].num_extractors;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_keygen_ext_fields_create(unit, part, level + 1, section,
                        gran, num_ext, keygen_cfg, keygen_md));
            _func_rv = bcmfp_keygen_ext_fields_validate(unit, part, level + 1,
                        section, keygen_md);
            if (_func_rv == SHR_E_RESOURCE) {
                /* Reset previous finfo ext bitmap */
                (void) bcmfp_keygen_ext_section_finfo_reset(unit, part,
                                level + 1, section, keygen_md);
            } else {
                break;
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_fields_validate(unit, part, level + 1,
                                           section, keygen_md));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_section_validate(int unit,
                                 bcmfp_keygen_ext_cfg_t *ext_cfg,
                                 bcmfp_keygen_ext_field_info_t *finfo,
                                 bcmfp_keygen_md_t *keygen_md)
{
    int gran = 0;
    uint16_t fill_bits = 0, parent_fill_bits = 0;
    uint16_t drain_bits = 0, parent_drain_bits = 0;
    bcmfp_keygen_ext_section_t out_sec;
    bcmfp_keygen_ext_section_t parent_sec;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);

    out_sec = ext_cfg->out_sec;
    fill_bits = keygen_md->ext_cfg_db->sec_cfg[out_sec]->fill_bits;
    drain_bits = keygen_md->ext_cfg_db->sec_cfg[out_sec]->drain_bits;

    parent_sec = keygen_md->ext_cfg_db->sec_cfg[out_sec]->parent_sec;
    parent_fill_bits = keygen_md->ext_cfg_db->sec_cfg[parent_sec]->fill_bits;
    parent_drain_bits = keygen_md->ext_cfg_db->sec_cfg[parent_sec]->drain_bits;

    if (((fill_bits + gran) > drain_bits) ||
        ((parent_fill_bits + gran) > parent_drain_bits)) {
        SHR_RETURN_VAL_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_alloc_3(int unit,
                      uint8_t part,
                      uint8_t level,
                      bcmfp_keygen_ext_section_t section,
                      bcmfp_keygen_cfg_t *keygen_cfg,
                      bcmfp_keygen_md_t *keygen_md)
{
    int g = 0, p = 0, l = 0;
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_section_t out_sec;
    bcmfp_keygen_ext_section_t parent_sec;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    /* Assign extractors to each field of the section. */
    finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
    while (finfo != NULL) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level + 1];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_idx;
            g = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
            p = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
            if ((p != part) ||
                (l != (level + 1)) ||
                (g != finfo->size) ||
                (section != ext_cfg->in_sec)) {
               continue;
            }
            _func_rv = bcmfp_keygen_ext_validate(unit, p, keygen_md, ext_cfg);
            if (SHR_FUNC_VAL_IS(SHR_E_CONFIG)) {
                continue;
            }
             SHR_IF_ERR_VERBOSE_EXIT(_func_rv);
            _func_rv = bcmfp_keygen_ext_section_validate(unit,
                                            ext_cfg, finfo, keygen_md);
            if (SHR_FUNC_VAL_IS(SHR_E_RESOURCE)) {
                continue;
            }
            out_sec = ext_cfg->out_sec;
            parent_sec = keygen_md->ext_cfg_db->sec_cfg[out_sec]->parent_sec;
            keygen_md->ext_cfg_db->sec_cfg[out_sec]->fill_bits += g;
            if (parent_sec != out_sec) {
                keygen_md->ext_cfg_db->sec_cfg[parent_sec]->fill_bits += g;
            }
            ext_cfg->finfo = finfo;
            ext_cfg->in_use = TRUE;
            finfo->extracted = TRUE;
            finfo->ext_cfg = ext_cfg;
            break;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            (finfo->extracted == FALSE) ? SHR_E_RESOURCE : SHR_E_NONE);
        finfo = finfo->next;
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_alloc_2(int unit,
                      uint8_t part,
                      uint8_t level,
                      bcmfp_keygen_cfg_t *keygen_cfg,
                      bcmfp_keygen_md_t *keygen_md)
{
    uint8_t sec_idx = 0;
    bcmfp_keygen_ext_section_t section;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (sec_idx = 0; sec_idx < BCMFP_KEYGEN_EXT_SECTION_COUNT; sec_idx++) {
        section = keygen_md->sections[part][level][sec_idx];
        if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_alloc_3(unit, part, level,
                                  section, keygen_cfg, keygen_md));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_finfo_db_free(int unit,
                             uint8_t part,
                             uint8_t level,
                             bcmfp_keygen_md_t *keygen_md)
{
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t *next_finfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    for (section = 0; section < BCMFP_KEYGEN_EXT_SECTION_COUNT; section++) {
        finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
        while (finfo != NULL) {
            next_finfo = finfo->next;
            sal_memset(finfo, 0, sizeof(bcmfp_keygen_ext_field_info_t));
            finfo->next = finfo_free_list[unit];
            finfo_free_list[unit] = finfo;
            DUMP_FINFO_COUNT(1);
            finfo = next_finfo;
        }
        keygen_md->ext_finfo_db[part][level].finfo[section] = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_finfo_add(int unit,
                        uint8_t part,
                        bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo3 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
       temp_finfo1 = &(keygen_md->finfo[idx]);
       section = temp_finfo1->section;
       if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (temp_finfo1->part != part)  {
           continue;
       }
       temp_finfo2 = finfo_free_list[unit];
       SHR_IF_ERR_EXIT((temp_finfo2 == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);
       finfo_free_list[unit] = finfo_free_list[unit]->next;
       DUMP_FINFO_COUNT(0);
       sal_memcpy(temp_finfo2, temp_finfo1, sizeof(bcmfp_keygen_ext_field_info_t));
       temp_finfo2->next = NULL;
       if (keygen_md->ext_finfo_db[part][0].finfo[section] == NULL) {
           keygen_md->ext_finfo_db[part][0].finfo[section] = temp_finfo2;
       } else {
           temp_finfo3 = keygen_md->ext_finfo_db[part][0].finfo[section];
           while (temp_finfo3->next != NULL) {
               temp_finfo3 = temp_finfo3->next;
           }
           temp_finfo3->next = temp_finfo2;
       }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_alloc_1(int unit,
                      uint8_t part,
                      bcmfp_keygen_cfg_t *keygen_cfg,
                      bcmfp_keygen_md_t *keygen_md)
{
    uint8_t level = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_finfo_db_free(unit, part, level, keygen_md));
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_ext_cfg_reset_part(unit, part, keygen_cfg, keygen_md));
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_ext_finfo_add(unit, part, keygen_md));
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_keygen_ext_codes_validate(unit, part, keygen_cfg, keygen_md));
    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_alloc_2(unit, part, level, keygen_cfg, keygen_md));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_fields_get_next(unit, part, level, keygen_cfg, keygen_md));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC void
bcmfp_keygen_ext_finfo_list_init(int unit)
{
    uint16_t idx = 0;

    sal_memset(&finfo_pool[unit], 0,
        sizeof(bcmfp_keygen_ext_field_info_t) * BCMFP_KEYGEN_EXT_FINFO_COUNT);
    for (idx = 0; idx < (BCMFP_KEYGEN_EXT_FINFO_COUNT - 1); idx++) {
         finfo_pool[unit][idx].next = &(finfo_pool[unit][idx + 1]);
    }
    finfo_pool[unit][BCMFP_KEYGEN_EXT_FINFO_COUNT - 1].next = NULL;
    finfo_free_list[unit] = &(finfo_pool[unit][0]);
    finfo_free_count = 1024;
    finfo_used_count = 0;
}

STATIC int
bcmfp_keygen_ext_finfo_list_verify(int unit)
{
    uint16_t idx = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    finfo = finfo_free_list[unit];
    while (finfo != NULL) {
        idx++;
        finfo = finfo->next;
    }

    if (idx != BCMFP_KEYGEN_EXT_FINFO_COUNT) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_validate(int unit,
                       uint8_t part,
                       bcmfp_keygen_md_t *keygen_md,
                       bcmfp_keygen_ext_cfg_t *ext_cfg)
{
    uint8_t p = part;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);

    if (ext_cfg->in_use == TRUE) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_MODE_HALF) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_MODE_SINGLE_ASET_WIDE) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_IPBM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0_HM)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1_HM)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB_HM)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_A) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_B) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_DROP) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_PKT_RESOLUTION) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_C) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_D) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_INT_PRI) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_IFP_DROP_VECTOR) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_MH_OPCODE) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_alloc(int unit,
                    bcmfp_keygen_cfg_t *keygen_cfg,
                    bcmfp_keygen_oper_t *keygen_oper,
                    bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t part = 0;
    uint8_t level = 0;
    uint16_t size = 0;
    shr_error_t rv = SHR_E_NONE;
    uint8_t field_extracted = FALSE;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    bcmfp_keygen_ext_finfo_list_init(unit);

    size = (sizeof(uint32_t) * BCMFP_KEYGEN_ENTRY_PARTS_MAX *
                               BCMFP_KEYGEN_EXT_GRAN_COUNT);
    sal_memset(keygen_md->bits_extracted, 0, size);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_ext_fields_get_first(unit, keygen_cfg, keygen_md));

    if (keygen_cfg->num_parts == 1) {
        for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
            finfo = &(keygen_md->finfo[idx]);
            section = finfo->section;
            if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                break;
            }
            finfo->part = 0;
        }
    } else {
        for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
            finfo = &(keygen_md->finfo[idx]);
            field_extracted = FALSE;
            section = finfo->section;
            if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                break;
            }
            keygen_md->tried_part_bmp[0] = 0;
            for (part = 0; part < keygen_cfg->num_parts; part++) {
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmfp_keygen_ext_part_get(unit, keygen_cfg,
                                             keygen_md, finfo));
                if (finfo->part < 0) {
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
                _func_rv = bcmfp_keygen_ext_alloc_1(unit, finfo->part,
                                                   keygen_cfg, keygen_md);
                if (SHR_FUNC_VAL_IS(SHR_E_RESOURCE)) {
                   finfo->part = -1;
                   continue;
                } else if (SHR_FAILURE(_func_rv)) {
                    SHR_IF_ERR_VERBOSE_EXIT(_func_rv);
                } else {
                   field_extracted = TRUE;
                   if (finfo->part < 0) {
                       SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                   }
                   keygen_md->bits_extracted[finfo->part][0] += finfo->size;
                   break;
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT(
                (field_extracted == FALSE) ? SHR_E_RESOURCE : SHR_E_NONE);
        }
    }

    for (part = 0; part < keygen_cfg->num_parts; part++) {
         SHR_IF_ERR_VERBOSE_EXIT(
             bcmfp_keygen_ext_alloc_1(unit, part, keygen_cfg, keygen_md));
    }
    /* Assign the offsets in the final key for all the qualifiers. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_qual_offset_create(unit, keygen_cfg, keygen_oper, keygen_md));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_ext_codes_create(unit, keygen_cfg, keygen_md, keygen_oper));
exit:
    for (part = 0; part < keygen_cfg->num_parts; part++) {
         for (level = 0; level < keygen_md->num_ext_levels; level++) {
             (void)bcmfp_keygen_ext_finfo_db_free(unit, part, level, keygen_md);
         }
         (void)bcmfp_keygen_ext_cfg_reset_part(unit, part, keygen_cfg, keygen_md);
    }
    rv = bcmfp_keygen_ext_finfo_list_verify(unit);
    if (SHR_FAILURE(rv)) {
        _func_rv = rv;
    }
    SHR_FUNC_EXIT();
}
