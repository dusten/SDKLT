/*! \file bcmltd_bitop_range_copy.c
 *
 * Bit array operations.
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

#include <bcmltd/bcmltd_bitop.h>

/*!
 * INTERNAL USE ONLY.
 *
 * Same as bcmltd_bitop_range_copy, but for a single BCMLTD_BITDCL.
 */
static inline void
bcmltd_bitop_range_copy_one_bitdcl(BCMLTD_BITDCL *a1,
        int offs1,
        BCMLTD_BITDCL a2,
        int offs2, int n)
{
    BCMLTD_BITDCL data;
    BCMLTD_BITDCL mask;
    /* Get the data */
    data = a2 >> offs2;

    /* Align the data to the place it may be inserted */
    data <<= offs1;

    /*
     * We might have bits in a2 above offs2 + n that need
     * to be cleared.
     */
    mask = ~0;
    mask >>= BCMLTD_BITWID - n;
    mask <<= offs1;
    data &= mask;
    *a1 &= ~mask;
    *a1 |= data;
}

/*!
 * \brief Copy a range of bits between two bit arrays.
 *
 * INTERNAL USE ONLY.
 *
 * Refer to \ref BCMLTD_BITCOPY_RANGE macro.
 */
void
bcmltd_bitop_range_copy(BCMLTD_BITDCL *a1,
        int offs1,
        const BCMLTD_BITDCL *a2,
        int offs2, int n)
{
    BCMLTD_BITDCL *cur_dst;
    const BCMLTD_BITDCL *cur_src;
    int woff_src, woff_dst, wremain;

    if (n <= 0) {
        return;
    }

    if ((offs1 % BCMLTD_BITWID) == 0 &&
        (offs2 % BCMLTD_BITWID) == 0 &&
        (n % BCMLTD_BITWID) == 0) {
        /* Both source, destination and size are BCMLTD_BITDCL aligned */
        sal_memcpy(&(a1[offs1 / BCMLTD_BITWID]), &(a2[offs2 / BCMLTD_BITWID]),
                   BCMLTD_BIT_ALLOCSIZE(n));
        return;
    }

    cur_dst = a1 + (offs1 / BCMLTD_BITWID);
    cur_src = a2 + (offs2 / BCMLTD_BITWID);

    woff_src = offs2 % BCMLTD_BITWID;
    woff_dst = offs1 % BCMLTD_BITWID;

    if (woff_dst >= woff_src) {
        wremain = BCMLTD_BITWID - woff_dst;
    } else {
        wremain = BCMLTD_BITWID - woff_src;
    }
    if (n <= wremain) {
        bcmltd_bitop_range_copy_one_bitdcl(cur_dst, woff_dst,
                                        *cur_src, woff_src, n);
        return;
    }
    bcmltd_bitop_range_copy_one_bitdcl(cur_dst, woff_dst,
                                    *cur_src, woff_src, wremain);
    n -= wremain;
    while (n >= BCMLTD_BITWID) {
        if (woff_dst >= woff_src) {
            ++cur_dst;
            wremain = woff_dst - woff_src;
            if (wremain > 0) {
                bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                                BCMLTD_BITWID - wremain, wremain);
            }
        } else {
            ++cur_src;
            wremain = woff_src - woff_dst;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                            *cur_src, 0, wremain);
        }
        n -= wremain;
        wremain = BCMLTD_BITWID - wremain;
        if (woff_dst >= woff_src) {
            ++cur_src;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                            *cur_src, 0, wremain);
        } else {
            ++cur_dst;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            BCMLTD_BITWID - wremain, wremain);
        }
        n -= wremain;
    }

    if (woff_dst >= woff_src) {
        ++cur_dst;
        wremain = woff_dst - woff_src;
        if (n <= wremain) {
            if (n > 0) {
                bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                                BCMLTD_BITWID - wremain, n);
            }
            return;
        }
        if (wremain > 0) {
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            BCMLTD_BITWID - wremain, wremain);
        }
    } else {
        ++cur_src;
        wremain = woff_src - woff_dst;
        if (n <= wremain) {
            if (n > 0) {
                bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                                *cur_src, 0, n);
            }
            return;
        }
        bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                        *cur_src, 0, wremain);
    }
    n -= wremain;

    if (n > 0) {
        wremain = BCMLTD_BITWID - wremain;
        if (woff_dst >= woff_src) {
            ++cur_src;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                            *cur_src, 0, n);
        } else {
            ++cur_dst;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            BCMLTD_BITWID - wremain, n);
        }
    }
}
