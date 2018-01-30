/*! \file shr_pb_format_uint64.c
 *
 * Format uint64_t value(s) to print buffer.
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

#include <shr/shr_pb_format.h>

const char *
shr_pb_format_uint64(shr_pb_t *pb, const char *prefix,
                     uint64_t *val, int count, uint32_t bitwidth)
{
    int idx, bit;
    int start = 0, minwidth = 0;
    uint32_t datawidth = 0;

    if (pb == NULL || count <= 0) {
        return SHR_PB_ERR_STR;
    }

    if (prefix) {
        shr_pb_printf(pb, "%s", prefix);
    }

    if (bitwidth > 0) {
        /*
         * If the format bit width is specified, get the number of hex-digits
         * for the most significant uint64 to support leading zeros format.
         */
        minwidth = ((bitwidth + 3) / 4) % 16;
        if (minwidth == 0) {
            minwidth = 16;
        }
        start = (bitwidth - 1) / 64;

        /* Check the bit width of the given data array. */
        for (idx = count - 1; idx >= 0; idx--) {
            if (val[idx] == 0) {
                continue;
            }
            if (datawidth == 0) {
                for (bit = 63; bit >= 0; bit--) {
                    if (val[idx] >> bit) {
                        datawidth = bit + 1;
                        break;
                    }
                }
            } else {
                datawidth += 64;
            }
        }

        /* Ignore leading zeros format if data exceeds the format width. */
        if (datawidth > bitwidth) {
            minwidth = 0;
        }
    }

    if (minwidth > 0) {
        uint64_t val64;

        /*
         * Left-justify with the leading zeros to
         * match the specified bit width.
         */
        for (idx = start; idx >= 0; idx--) {
            if (idx >= count) {
                val64 = 0;
            } else {
                val64 = val[idx];
            }
            if (idx == start) {
                if (minwidth > 8) {
                    shr_pb_printf(pb, "0x%.*"PRIx64, minwidth - 8, val64 >> 32);
                    shr_pb_printf(pb, "_%08"PRIx64, val64 & 0xffffffff);
                } else {
                    shr_pb_printf(pb, "0x%.*"PRIx64, minwidth, val64);
                }
            } else {
                shr_pb_printf(pb, "_%08"PRIx64, val64 >> 32);
                shr_pb_printf(pb, "_%08"PRIx64, val64 & 0xffffffff);
            }
        }
    } else {
        /*
         * Format values without leading zeros.
         */
        for (start = -1, idx = count - 1; idx >= 0; idx--) {
            if (start < 0) {
                if (idx == 0 && val[idx] < 10) {
                    shr_pb_printf(pb, "%"PRIu64, val[idx]);
                    return shr_pb_str(pb);
                }
                /* Skip the leading zero elements. */
                if (val[idx] == 0) {
                    continue;
                }
                /* format the first non-zero element. */
                if (val[idx] > 0xffffffff) {
                    shr_pb_printf(pb, "0x%"PRIx64, val[idx] >> 32);
                    shr_pb_printf(pb, "_%08"PRIx64, val[idx] & 0xffffffff);
                } else {
                    shr_pb_printf(pb, "0x%"PRIx64, val[idx]);
                }
                start = idx;
                continue;
            }
            shr_pb_printf(pb, "_%08"PRIx64, val[idx] >> 32);
            shr_pb_printf(pb, "_%08"PRIx64, val[idx] & 0xffffffff);
        }
    }

    return shr_pb_str(pb);
}
