/*! \file shr_util.c
 *
 * Common utility routines.
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

#include <shr/shr_util.h>

void
shr_util_format_uint32(char *buf, uint32_t n, int min_digits, int base)
{
    static char *digit_char = "0123456789abcdef";
    unsigned int tmp;
    int digit, needed_digits = 0;

    for (tmp = n, needed_digits = 0; tmp; needed_digits++) {
        tmp /= base;
    }

    if (needed_digits > min_digits) {
        min_digits = needed_digits;
    }

    buf[min_digits] = 0;

    for (digit = min_digits - 1; digit >= 0; digit--) {
        buf[digit] = digit_char[n % base];
        n /= base;
    }
}

void
shr_util_format_uint32_array(char *buf, uint32_t *val, int nval)
{
    int i = SHR_BYTES2WORDS(nval) - 1;

    /*
     * Don't skip leading 0's, as this makes packet buffers
     * and other buffer memories difficult to read and to determine
     * what the contents are.
     */

    if (i == 0 && val[i] < 10) { /* Only a single word < 10 */
        buf[0] = '0' + val[i];
        buf[1] = 0;
    } else {
        /* Print first word */
        buf[0] = '0';
        buf[1] = 'x';

        /*
         * Don't use the previous value of 1 min digit for multiple
         * integer values, as that makes a long integer of all zeroes
         * print as just '0', which makes memory contents hard to read.
         * Only use mindigit of 1 if only one integer to print
         */
        if (i == 0) {
            shr_util_format_uint32(buf + 2, val[i], 1, 16);
        } else {
            if ((nval % 4) == 0) {
                shr_util_format_uint32(buf + 2, val[i], 8, 16);
            } else {
                shr_util_format_uint32(buf + 2, val[i],
                                       (2 * (nval % 4)), 16);
            }
        }

        /* Print rest of words, if any */
        while (--i >= 0) {
            while (*buf) {
                buf++;
            }
            shr_util_format_uint32(buf, val[i], 8, 16);
        }
    }
}

void
shr_util_format_uint8_array(char *buf, const uint8_t *val, int nval)
{
    static char *digit_char = "0123456789abcdef";
    int byte;

    /* Print first word */
    buf[0] = '0';
    buf[1] = 'x';

    for (byte = nval - 1; byte >= 0; byte--) {
        buf[2 * (nval - byte)] = digit_char[(val[byte] / 16) % 16];
        buf[(2 * (nval - byte)) + 1] = digit_char[val[byte] % 16];
    }

    /* Null string terminate */
    buf[(2 * nval) + 2] = 0;
}

int
shr_util_popcount(uint32_t n)
{
    n = (n & 0x55555555) + ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n + (n >> 4)) & 0x0f0f0f0f;
    n = n + (n >> 8);

    return (n + (n >> 16)) & 0xff;
}

int
shr_util_ffs(uint32_t n)
{
    int i16, i8, i4, i2, i1, i0;

    i16 = !(n & 0xffff) << 4;
    n >>= i16;
    i8 = !(n & 0xff) << 3;
    n >>= i8;
    i4 = !(n & 0xf) << 2;
    n >>= i4;
    i2 = !(n & 0x3) << 1;
    n >>= i2;
    i1 = !(n & 0x1);
    i0 = (n >> i1) & 1 ? 0 : -32;

    return i16 + i8 + i4 + i2 + i1 + i0;
}

int
shr_util_xch2i(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return (ch - '0'     );
    }
    if (ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + 10);
    }
    if (ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + 10);
    }
    return -1;
}
