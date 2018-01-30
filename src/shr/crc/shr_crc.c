/*! \file shr_crc.c
 *
 * Ethernet CRC Algorithm. Initially for hash vector computing.
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

#include <sal/sal_libc.h>
#include <shr/shr_crc.h>

static uint32_t
shr_swap32(uint32_t i)
{
    i = (i << 16) | (i >> 16);

    return (i & 0xff00ffff) >> 8 | (i & 0xffff00ff) << 8;
}

uint16_t
shr_bit_rev16(uint16_t n)
{
    n = (((n & 0xaaaa) >> 1) | ((n & 0x5555) << 1));
    n = (((n & 0xcccc) >> 2) | ((n & 0x3333) << 2));
    n = (((n & 0xf0f0) >> 4) | ((n & 0x0f0f) << 4));
    n = (((n & 0xff00) >> 8) | ((n & 0x00ff) << 8));
    return n;
}

uint32_t
shr_bit_rev_by_byte_word32(uint32_t n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    return n;
}


static int shr_crc_table_inited;
static uint32_t shr_crc_table[256];

uint32_t
shr_crc32(uint32_t crc, uint8_t *data, int len)
{
    int i;

    if (!shr_crc_table_inited) {
        int j;
        uint32_t accum;

        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                if (accum & 1) {
                    accum = accum >> 1 ^ 0xedb88320UL;
                } else {
                    accum = accum >> 1;
                }
            }

            shr_crc_table[i] = shr_swap32(accum);
        }

        shr_crc_table_inited = 1;
    }

    for (i = 0; i < len; i++) {
        crc = crc << 8 ^ shr_crc_table[crc >> 24 ^ data[i]];
    }

    return crc;
}

uint32_t
shr_crc32b(uint32_t crc, uint8_t *data, int nbits)
{
    int i;
    int j;
    uint32_t accum;
    uint32_t poly = 0xedb88320UL;
    int last_nbits;

    if (!shr_crc_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            shr_crc_table[i] = shr_swap32(accum);
        }
        shr_crc_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
        crc = (crc << 8) ^ shr_crc_table[data[i] ^ ((crc >> 24) & 0x000FF)];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = ((crc >> (32 - last_nbits)) & ((1 << last_nbits) - 1)) ^
                (data[i]  & ((1 << last_nbits) - 1));
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc << last_nbits) ^ shr_swap32(accum);
    }

    return crc;
}

static uint16_t shr_crc_16_table[16] = {
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
};

uint16_t
shr_crc16(int start, uint8_t *p, int n)
{
    uint16_t crc = start;
    int r;

    /* while there is more data to process */
    while (n-- > 0) {

        /* compute checksum of lower four bits of *p */
        r = shr_crc_16_table[crc & 0xF];
        crc = (crc >> 4) & 0x0FFF;
        crc = crc ^ r ^ shr_crc_16_table[*p & 0xF];

        /* now compute checksum of upper four bits of *p */
        r = shr_crc_16_table[crc & 0xF];
        crc = (crc >> 4) & 0x0FFF;
        crc = crc ^ r ^ shr_crc_16_table[(*p >> 4) & 0xF];

        /* next... */
        p++;
    }

    return(crc);
}

static int shr_crc16b_table_inited;
static uint32_t shr_crc_16btable[256];

uint16_t
shr_crc16b(int crc, uint8_t *data, int nbits)
{
    int i;
    int j;
    uint32_t accum;
    uint32_t poly = 0xa001;
    int last_nbits;

    if (!shr_crc16b_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            shr_crc_16btable[i] = accum;
        }
        shr_crc16b_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
        crc = (crc >> 8) ^ shr_crc_16btable[((data[i] & 0x000000FF) << 0) ^
                                             (crc & 0x000000FF)];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = (crc & ((1 << last_nbits) - 1)) ^
                (data[i]  & ((1 << last_nbits) - 1));
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc >> last_nbits) ^ accum;
    }

    return crc;
}
