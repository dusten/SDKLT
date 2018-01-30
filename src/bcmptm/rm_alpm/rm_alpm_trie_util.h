/*! \file rm_alpm_trie_util.h
 *
 * Utilities internal to RM ALPM
 *
 * This file contains utilities which are internal to
 * ALPM Resource Manager
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

#ifndef RM_ALPM_TRIE_UTIL_H
#define RM_ALPM_TRIE_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>

/*******************************************************************************
 * Defines
 */

/* loop up key size */
#define RM_ALPM_IPV4_MAX_VRF_SIZE (16)
#define RM_ALPM_IPV4_PFX_SIZE (32)
#define RM_ALPM_IPV4_KEY_SIZE (RM_ALPM_IPV4_MAX_VRF_SIZE + \
                               RM_ALPM_IPV4_PFX_SIZE)
#define RM_ALPM_IPV4_KEY_SIZE_WORDS 2 /* (((RM_ALPM_IPV4_KEY_SIZE)+31) / 32) */

#define RM_ALPM_IPV6_MAX_VRF_SIZE (16)
#define RM_ALPM_IPV6_PFX_SIZE (128)
#define RM_ALPM_IPV6_KEY_SIZE (RM_ALPM_IPV6_MAX_VRF_SIZE + \
                               RM_ALPM_IPV6_PFX_SIZE)
#define RM_ALPM_IPV6_KEY_SIZE_WORDS 5 /* (((RM_ALPM_IPV6_KEY_SIZE)+31) / 32) */
#define RM_ALPM_MAX_KEY_SIZE       (RM_ALPM_IPV6_KEY_SIZE)
#define RM_ALPM_MAX_KEY_SIZE_WORDS  5 /* (((RM_ALPM_MAX_KEY_SIZE)+31) / 32) */

#define SHL(data, shift, max) \
    (((shift) >= (max)) ? 0 : ((data) << (shift)))

#define SHR(data, shift, max) \
    (((shift) >= (max)) ? 0 : ((data) >> (shift)))

#define MASK(len) \
    (((len) >= 32 || (len) == 0) ? 0xFFFFFFFF : ((1U << (len)) - 1))

#define BITMASK(len) \
    (((len) >= 32) ? 0xFFFFFFFF : ((1U << (len)) - 1))

#define ABS(n) ((((int)(n)) < 0) ? -(n) : (n))

#define NUM_WORD_BITS (32)
#define NUM_WORD_MASK (0x1F)
#define MAX_SKIP_LEN  (31)

/*
 * bit 0                                  - 0
 * bit [1, _MAX_SKIP_LEN]                 - 1
 * bit [_MAX_SKIP_LEN+1, 2*_MAX_SKIP_LEN] - 2...
 */
#define BITS2SKIPOFF(x) (((x) + MAX_SKIP_LEN - 1) / MAX_SKIP_LEN)

/* (internal) Generic operation macro on bit array _a, with bit _b */
#define        BITOP(_a, _b, _op)        \
    ((_a) _op (1U << ((_b) & NUM_WORD_MASK)))

/* Specific operations */
#define        BITGET(_a, _b)        BITOP(_a, _b, &)
#define        BITSET(_a, _b)        BITOP(_a, _b, |=)
#define        BITCLR(_a, _b)        BITOP(_a, _b, &= ~)

/* get the bit position of the LSB set in bit 0 to bit "msb" of "data"
 * (max 32 bits), "lsb" is set to -1 if no bit is set in "data".
 */
#define BITGETLSBSET(_data, _msb, _lsb)             \
    {                                               \
        _lsb = 0;                                   \
        while ((_lsb) <= (_msb)) {                  \
            if ((_data) & (1U << (_lsb)))     {     \
                break;                              \
            } else { (_lsb)++;}                     \
        }                                           \
        _lsb = ((_lsb) > (_msb)) ? -1U : (_lsb);    \
    }

#define BITS2WORDS(x)        (((x) + 31) >> 5)

/* key packing expetations:
 * eg., 144 bit key
 * - 0x10/8          -> key[0]=0, key[1]=0, key[2]=0, key[3]=0, key[0]=0x10
 * - 0x123456789a/48 -> key[0]=0, key[1]=0, key[2]=0, key[3] = 0x12
 *                      key[4] = 0x3456789a
 * eg., 48 bit key
 * - 10/8         -> key[0]=0, key[1]=8
 * - 0x123456789a -> key[0] = 0x12 key[1] = 0x3456789a
 * length - represents number of valid bits from farther to lower index ie.,
 *          1->0
 * NOTE: bit_pos is 1 based.
 */

#define KEY_BIT2IDX(max_key_len, bit_pos)  \
    ((((BITS2WORDS(max_key_len)) << 5) - (bit_pos)) >> 5)

#define KEY_BRANCH_BIT(key, max_key_len, bit_pos)       \
    ((key)[KEY_BIT2IDX(max_key_len, (bit_pos))] &       \
     (1U << (((bit_pos) - 1) & NUM_WORD_MASK))) ? 1 : 0

#define KEY_BIT_GET(key, bit_pos, max_len)    \
    (((key)[KEY_BIT2IDX(max_len, bit_pos)] &  \
      (1U << ((bit_pos) & NUM_WORD_MASK))) >> \
     ((bit_pos) & NUM_WORD_MASK))

#define KEY_BIT_SET(key, bit_pos, max_len) \
    ((key)[KEY_BIT2IDX(max_len, bit_pos)] |= (1U << ((bit_pos) & NUM_WORD_MASK)))


#define KEY_BITS(bits, max_key_len, key, msb, len, skip_len_check)  \
do {                                                                \
        uint32_t lsb, lsb_idx, data;                                \
        lsb = msb - len; /* 0based */                               \
        lsb_idx = KEY_BIT2IDX(max_key_len, lsb + 1);                \
        /* coverity[result_independent_of_operands] */              \
        lsb = lsb & NUM_WORD_MASK;                                  \
        data = SHR(key[lsb_idx], lsb, NUM_WORD_BITS);               \
        if (lsb_idx >= 1) {                                         \
            data |= SHL(key[lsb_idx - 1], 32 - lsb, NUM_WORD_BITS); \
        }                                                           \
        bits = data & (BITMASK((len)));                             \
} while (0)


#define LCP_LEN(lcp, max_key_len, key, len1, skip_addr, len2)          \
do {                                                                   \
    uint32_t diff;                                                     \
    if ((len1 == 0) || (len2 == 0)) {                                  \
        lcp = 0;                                                       \
    } else {                                                           \
        lcp = len1 < len2 ? len1 : len2;                               \
        KEY_BITS(diff, max_key_len, key, len1, lcp, 0);                \
        diff ^= (SHR(skip_addr, len2 - lcp, MAX_SKIP_LEN) & MASK(lcp));\
        while (diff) {                                                 \
            diff >>= 1;                                                \
            --lcp;                                                     \
        }                                                              \
    }                                                                  \
} while (0)

/*******************************************************************************
 * Typedefs
 */

typedef struct rm_alpm_ipv4_prefix_s {
    uint32_t length:8;
    uint32_t vrf:16;
    uint32_t key;
} rm_alpm_ipv4_prefix_t;

typedef struct rm_alpm_ipv6_prefix_s {
    uint32_t length:8;
    uint32_t vrf:16;
    uint32_t key[4];
} rm_alpm_ipv6_prefix_t;

/*******************************************************************************
 * Function prototypes
 */

extern int bcmptm_rm_alpm_prefix_show(uint32_t max_key_size,
                                      uint32_t *key,
                                      uint32_t length);

extern int bcmptm_rm_alpm_key_shift(uint32_t max_key_size,
                                    uint32_t *key,
                                    uint32_t length,
                                    int      shift);

extern int bcmptm_rm_alpm_key_match(uint32_t max_key_size,
                                    uint32_t *key1, uint32_t length1,
                                    uint32_t *key2, uint32_t length2);

extern int bcmptm_rm_alpm_bpm_pfx_get(uint32_t *bpm,
                                      uint32_t key_len,
                                      uint32_t max_key_len,
                                      uint32_t *pfx_len);

extern int bcmptm_rm_alpm_key_append(uint32_t max_key_len,
                                     uint32_t *key,
                                     uint32_t *length,
                                     uint32_t skip_addr,
                                     uint32_t skip_len);

extern int bcmptm_rm_alpm_bpm_append(uint32_t max_key_len,
                                     uint32_t *key,
                                     uint32_t *length,
                                     uint32_t skip_addr,
                                     uint32_t skip_len);

extern uint32_t bcmptm_rm_alpm_key_bits_get(uint32_t max_key_len,
                                            uint32_t *key,
                                            uint32_t msb, /* 1based, msb bit
                                                             position */
                                            uint32_t len,
                                            uint32_t skip_len_check);

extern uint32_t bcmptm_rm_alpm_lcplen(uint32_t max_key_len,
                                      uint32_t *key,
                                      uint32_t len1,
                                      uint32_t skip_addr,
                                      uint32_t len2);

#endif /* RM_ALPM_TRIE_UTIL_H */
