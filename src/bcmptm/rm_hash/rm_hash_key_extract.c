/*! \file rm_hash_key_extract.c
 *
 * Key extraction implementation for hash table
 *
 * This file contains the functions that implements key extraction for hash table
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "rm_hash_internal.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static void
rm_hash_robust_hash_get(int unit,
                        bcmptm_rm_hash_rhash_cfg_t *rhcfg,
                        int rh_idx,
                        uint8_t *key,
                        int n_bits)
{
    int ix, n_bytes, byte_offset = 0;
    int append = 0;
    int nibble_swapped_key = 0, key_size = 0;
    int remap_table_index = 0, action_table_index = 0;
    uint32_t remap_val;
    uint64_t action;
    int sr_bits = 0;
    int sl_bits = 0;

    action = 0;
    n_bytes = (n_bits + 7) / 8;

    /* Generate Remap table index */
    for(ix = 0; ix < n_bytes; ix++) {
        remap_table_index = remap_table_index ^ key[ix];
    }

    /* Generate Action table index */
    for(ix = 0; ix < n_bytes; ix++) {
        nibble_swapped_key = ((key[ix] & 0x0F) << 4) |
                             ((key[ix] & 0xF0) >> 4 );
        action_table_index = action_table_index ^ nibble_swapped_key;
    }

    /* Read Remap and Action table values from SW cache*/
    if (rh_idx == 1) {
        remap_val = rhcfg->remap_data_b[remap_table_index];
        action = rhcfg->action_data_b[action_table_index /
                                        BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH];
    } else {
        remap_val = rhcfg->remap_data_a[remap_table_index];
        action = rhcfg->action_data_a[action_table_index /
                                        BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH];
    }

    /* Robust hash key transformation */
    if (COMPILER_64_BITTEST(action,
                (action_table_index % BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH)) == 0) {
        /* PREPEND */
        for (ix = n_bytes; ix > 0; ix--) {
            key[ix + 1] = key[ix - 1];
        }
        byte_offset = n_bytes + (BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH / 8);
    } else if (COMPILER_64_BITTEST(action,
                (action_table_index % BCMPTM_RM_HASH_RHASH_ACTION_TABLE_WIDTH)) == 1) {
        /* APPEND */
        key_size = n_bytes;
        byte_offset = n_bytes;
        append = 1;
    }

    /* Some devices don't have byte boundary aligned robust hash values */
    if (rhcfg->alignment == TRUE && (append == 1)){
        key[n_bits / 8] = (remap_val & 0xFF) << (n_bits % 8);
        key[(n_bits / 8) + 1] = ((remap_val & 0xFF) >> (8 - (n_bits % 8))) | ((((remap_val >> 8) & 0xF) |
            ((action_table_index & 0xF) << 4)) << (n_bits % 8));
        if (n_bits % 8) {
            key[(n_bits / 8) + 2] = (((remap_val >> 8) & 0xF) |
            ((action_table_index & 0xF) << 4)) >> (8 - n_bits % 8);
        }
    } else {
        key[key_size] = remap_val & 0xFF;
        key[key_size + 1] = ((remap_val >> 8) & 0xF) |
                            ((action_table_index & 0xF) << 4);
    }

    if (rhcfg->alignment == FALSE) {
        /* Adjust the byte boundary */
        if (n_bits % 8 != 0) {
            sr_bits = n_bits % 8;
            sl_bits = 8 - sr_bits;
            for (ix = byte_offset - 1; ix > 0; ix--) {
                key[ix] = key[ix] << sl_bits;
                key[ix] |= key[ix - 1] >> sr_bits;
            }
            key[ix] = key[ix] << sl_bits;
        }
    }
}

/*******************************************************************************
 * Public Functions
 */
void
bcmptm_rm_hash_key_extract(int unit,
                           uint32_t *entry,
                           const bcmptm_rm_hash_more_info_t *m_info,
                           const bcmptm_rm_hash_hw_entry_info_t *e_info,
                           bcmptm_rm_hash_rhash_cfg_t *rhcfg,
                           uint8_t rh_idx,
                           uint8_t sid_idx,
                           uint8_t *key,
                           int *key_size)
{
    const bcmlrd_hw_field_list_t *key_flds = NULL;
    uint16_t num, idx;
    const bcmptm_rm_hash_vector_attr_t * attrib = NULL;
    int key_index, val_index, fval_index;
    int rshft_cnt, lshft_cnt;
    uint32_t s_ent[BCMPTM_MAX_PT_ENTRY_WORDS];
    uint32_t val[BCMPTM_MAX_PT_ENTRY_WORDS];
    int val_bits, fld_width, max_key_len, bits;
    uint32_t *pent = entry;

    if ((m_info->key_type_val != NULL) &&
        (m_info->lt_hash_ktype_as_dtype == TRUE)) {
        int sbit, ebit, width;
        size_t ent_size;
        uint32_t key_type, fld[BCMPTM_MAX_PT_FIELD_WORDS] = {0};

        pent = &s_ent[0];
        ent_size = (e_info->entry_bitsize + 7) / 8;
        sal_memcpy(pent, entry, ent_size);
        sbit = e_info->hw_ktype_field_info->field_start_bit[0];
        width = e_info->hw_ktype_field_info->field_width;
        ebit = sbit + width - 1;
        key_type = m_info->key_type_val[sid_idx];
        key_type = key_type & (~0x1);
        fld[0] = key_type;
        bcmdrd_field_set(pent, sbit, ebit, fld);
    }
    /*
     * For a given LTID, different views (1X, 2X, 4X) may have the same key type
     * and key field format. In other words, all elements in the hw_entry_info
     * array have the same information in terms of key type and key fields.
     */
    key_flds = m_info->hw_key_fields;
    num = key_flds->num_fields;
    /*
     * For Hash table that has multiple KEY_TYPE fields, only the first
     * KEY_TYPE field will participate in hash vector computing. Get the pure
     * length of key fields. For hash tables which have corresponding robust
     * hash action and remap tables, the pure key field length will minus 16.
     */
    attrib = m_info->hash_vector_attr;
    if (rhcfg != NULL) {
        /*
         * For physical hash tables which have robust hash feature, obtain
         *  its original max key length by subtracting the robust hash length.
         */
        max_key_len = attrib->key_length - BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH;
    } else {
        max_key_len = attrib->key_length;
    }
    if ((rhcfg != NULL) && ((rhcfg->alignment == TRUE) ||
        (rhcfg->enable == TRUE))) {
        /*
         * For most devices, each hash table has its robust hash remap and action,
         * and they have key word alignment requirement.
         * For some devices, a few hash tables support robust hash, For all the
         * above cases, remains its max key len.
         */
        bits = max_key_len;
        val_bits = 0;
    } else {
        bits = (max_key_len + 7) & ~0x7;
        val_bits = bits - max_key_len;
    }
    sal_memset(val, 0, sizeof(val));
    for (idx = 0; idx < num; idx++) {
        int sbit, ebit;
        uint32_t key_field_val[BCMPTM_MAX_PT_ENTRY_WORDS] = {0};

        sbit = key_flds->field_start_bit[idx];
        ebit = key_flds->field_start_bit[idx] + key_flds->field_width[idx] - 1;
        bcmdrd_field_get(pent, sbit, ebit, key_field_val);
        fld_width = key_flds->field_width[idx];
        val_index = val_bits >> 5;
        fval_index = 0;
        lshft_cnt = val_bits & 0x1f;
        rshft_cnt = 32 - lshft_cnt;
        val_bits += fld_width;
        if (lshft_cnt) {
            for (; fld_width > 0; fld_width -= 32) {
                val[val_index++] |= key_field_val[fval_index] << lshft_cnt;
                val[val_index] |= key_field_val[fval_index++] >> rshft_cnt;
            }
        } else {
            for (; fld_width > 0; fld_width -= 32) {
                val[val_index++] = key_field_val[fval_index++];
            }
        }
    }
    key_index = 0;
    for (val_index = 0; val_bits > 0; val_index++) {
        for (rshft_cnt = 0; rshft_cnt < 32;
             rshft_cnt += 8) {
            if (val_bits <= 0) {
                break;
            }
            key[key_index++] = (val[val_index] >> rshft_cnt) & 0xff;
            val_bits -= 8;
        }
    }
    if ((bits + 7) / 8 > key_index) {
        sal_memset(&key[key_index], 0, (bits + 7) / 8 - key_index);
    }
    if ((rhcfg != NULL) && (rhcfg->alignment == FALSE)) {
        if (rhcfg->enable) {
            rm_hash_robust_hash_get(unit, rhcfg, rh_idx, key, bits);
        }
        /*
         * crc generator has the same key length irrespective
         * of whether robust hash is enabled. And for some device,
         * there is no word alignment requirement.
         */
        bits = (bits + 7) & ~0x7;
        bits += BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH;
    } else if ((rhcfg != NULL) && (rhcfg->alignment == TRUE)) {
        if (rhcfg->enable) {
            rm_hash_robust_hash_get(unit, rhcfg, rh_idx, key, max_key_len);
        }
        /*
         * Some devices have word alignment requirement, no matter if
         * robust hash is enabled.
         */
        bits = (((max_key_len - 1) / 32) + 1) * 32;
        bits += BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH;
    }
    *key_size = bits;
}
