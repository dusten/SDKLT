/*! \file rm_hash_vec_compute.c
 *
 * Routines that implement hash vector computing.
 *
 * This file contains the functions that implements hash vector computing
 * for hash table.
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
#include <shr/shr_crc.h>
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
/*!
 * \brief Compute CRC16 hash vector.
 *
 * \param [in] data Pointer to the buffer that contains the key.
 * \param [in] data_nbits Key size in unit of bit.
 */
static uint16_t
rm_hash_crc16b(uint8_t *data, int data_nbits)
{
    uint16_t rv;
    rv = shr_crc16b(0, data, data_nbits);
    rv = shr_bit_rev16(rv);
    return rv;
}

/*!
 * \brief Compute CRC32 hash vector.
 *
 * \param [in] data Pointer to the buffer that contains the key.
 * \param [in] data_nbits Key size in unit of bit.
 */
static uint32_t
rm_hash_crc32b(uint8_t *data, int data_nbits)
{
    uint32_t rv;
    rv = shr_crc32b(0, data, data_nbits);
    rv = shr_bit_rev_by_byte_word32(rv);
    return rv;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_vector_compute(int unit,
                              uint32_t *entry,
                              uint8_t *key_a,
                              uint8_t *key_b,
                              int key_size,
                              uint8_t *bank_list,
                              uint8_t num_banks,
                              const bcmptm_rm_hash_more_info_t *more_info,
                              rm_hash_pt_info_t *pt_info,
                              uint32_t *bucket)
{
    uint32_t lsb_val = 0, crc16_val = 0, crc32_val = 0;
    uint32_t vector, upper_half = 0, lower_half = 0;
    uint16_t sbit, ebit, uppermost = 0;
    uint8_t idx;
    const bcmptm_rm_hash_vector_attr_t *table_attr = NULL;
    rm_hash_vector_info_t *vec_info = NULL;
    rm_hash_bm_t e_bm;
    bool upper_half_computed = FALSE;
    bool lower_half_computed = FALSE;
    uint64_t hash_vector, hash_mask;

    SHR_FUNC_ENTER(unit);

    table_attr = more_info->hash_vector_attr;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_bkt_mode_get(unit,
                                         more_info,
                                         pt_info,
                                         &e_bm));
    for (idx = 0; idx < num_banks; idx++) {
        bcmptm_rm_hash_vector_type_t type;
        uint16_t offset;
        uint32_t mask;
        uint8_t bank;
        bank = bank_list[idx];
        vec_info = &pt_info->vec_info[bank];
        type = vec_info->type;
        offset = vec_info->offset;
        /*
         * The mask used for generating hash vector should be in the unit of
         * hardware logical buckets.
         */
        switch (e_bm) {
        case RM_HASH_BM_N:
        case RM_HASH_BM_0:
            mask = vec_info->mask;
            break;
        case RM_HASH_BM_1:
            mask = (vec_info->mask + 1) / 2 - 1;
            break;
        case RM_HASH_BM_2:
            mask = (vec_info->mask + 1) / 4 - 1;
            break;
        default:
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }

        switch (type) {
        case BCMPTM_RM_HASH_VEC_CRC16:
            if (bank == 0) {
                crc16_val = rm_hash_crc16b(key_a, key_size);
            } else {
                crc16_val = rm_hash_crc16b(key_b, key_size);
            }
            bucket[bank] = (crc16_val >> offset) & mask;
            break;
        case BCMPTM_RM_HASH_VEC_CRC32:
            if (bank == 0) {
                crc32_val = rm_hash_crc32b(key_a, key_size);
            } else {
                crc32_val = rm_hash_crc32b(key_b, key_size);
            }
            bucket[bank] = (crc32_val >> offset) & mask;
            break;
        case BCMPTM_RM_HASH_VEC_ZERO:
            bucket[bank] = 0;
            break;
        case BCMPTM_RM_HASH_VEC_LSB:
            sbit = table_attr->hw_lsb_field_start_bit;
            ebit = sbit + table_attr->hw_lsb_field_width - 1;
            bcmdrd_field_get(entry, sbit, ebit, &lsb_val);
            bucket[bank] = lsb_val;
            break;
        /* 64-bit length shared hash vector */
        case BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB:
            sbit = table_attr->hw_lsb_field_start_bit;
            ebit = sbit + table_attr->hw_lsb_field_width - 1;
            bcmdrd_field_get(entry, sbit, ebit, &lsb_val);
            uppermost = (uint16_t)lsb_val;
        /* coverity[fallthrough: FALSE] */
        case BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO:
            if (offset >= 48) {
                vector = uppermost;
                if (offset > 48) {
                    vector >>= offset - 48;
                }
                bucket[bank] = vector & mask;
            } else if (offset >= 32) {
                if (upper_half_computed == FALSE) {
                    upper_half = rm_hash_crc16b(key_a, key_size) |
                             (uppermost << 16);
                    upper_half_computed = TRUE;
                }
                vector = upper_half;
                if (offset > 32) {
                    vector >>= offset - 32;
                }
                bucket[bank] = vector & mask;
            } else {
                if (lower_half_computed == FALSE) {
                    lower_half = rm_hash_crc32b(key_a, key_size);
                    lower_half_computed = TRUE;
                }
                if (upper_half_computed == FALSE) {
                    upper_half = rm_hash_crc16b(key_a, key_size) |
                             (uppermost << 16);
                    upper_half_computed = TRUE;
                }
                vector = lower_half;
                if (offset > 0) {
                    vector >>= offset;
                    vector |= upper_half << (32 - offset);
                }
                bucket[bank] = vector & mask;
            }
            break;
        case BCMPTM_RM_HASH_VEC_CRC32A_CRC32B:
            /*  64 vector {crc32_B[31:0], crc32_A[31:0]} */
            upper_half = rm_hash_crc32b(key_b, key_size);
            lower_half = rm_hash_crc32b(key_a, key_size);
            COMPILER_64_SET(hash_vector, upper_half, lower_half);
            COMPILER_64_SET(hash_mask, 0, mask);
            COMPILER_64_SHR(hash_vector, offset);
            COMPILER_64_AND(hash_vector, hash_mask);
            bucket[bank] = (uint32_t)(COMPILER_64_LO(hash_vector));
            break;
        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

