/*! \file shr_util.h
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

#ifndef SHR_UTIL_H
#define SHR_UTIL_H

#include <shr/shr_types.h>

/*!
 * \brief Convert a count of bytes to a number of uint32_t words
 *
 * \param [in] \_bytes\_ count of bytes
 */
#define SHR_BYTES2WORDS(_bytes_)   (((_bytes_) + 3) / 4)

/*!
 * \brief Format a uint32_t as a string of ASCII digits.
 *
 * Prepare a formatted string of ASCII digits to express a uint32_t
 * quantity for use in BSL output.
 *
 * \param [in,out] buf Character arry to hold output string
 * \param [in] n Input uint32_t value to render as a string
 * \param [in] min_digits Minimum ASCII digits to use as string expression
 * \param [in] base Numeric base in which to express output
 *
 * Note - this function is used in the implementation of
 * shr_format_long_integer
 */
extern void
shr_util_format_uint32(char *buf, uint32_t n, int min_digits, int base);

/*!
 * \brief Format an arbitrary precision long integer.
 *
 * Prepare a formatted string of ASCII digits to express an
 * arbitrary-length integer encoded within a uint32_t array for use
 * in BSL output.
 *
 * If the value is less than 10, prints one decimal digit;
 * otherwise output is in hex format with 0x prefix.
 *
 * \param [in,out] buf Character arry to hold output string
 * \param [in] val Input uint32_t array holding the number to render
 *                 val[0] is the least significant word.
 * \param [in] nval the number of bytes in the value.
 *
 * Note - this function uses shr_format_long_integer
 */
extern void
shr_util_format_uint32_array(char *buf, uint32_t *val, int nval);

/*!
 * \brief Format an byte array for hexadecimal ASCII output.
 *
 * Output is in hex format with 0x prefix, null terminated.
 *
 * \param [out] buf String buffer for output
 * \param [in] val  Pointer to byte array holding the value
 *             val[0] is the least significant byte.
 * \param [in] nval is the number of bytes in the value.
 */
extern void
shr_util_format_uint8_array(char *buf, const uint8_t *val, int nval);

/*!
 * \brief Return the number of bits set in a uint32_t.
 *
 * \param [in] n A unit32_t value to check the set bits.
 *
 * \return The number of bits set in \c n.
 */
extern int
shr_util_popcount(uint32_t n);

/*!
 * \brief Find the first set bit.
 *
 * This function will return the position of the first set bit in a uint32_t
 * value. The position starts from 0.
 *
 * \param [in] n A uint32_t value to check the set bit.
 *
 * \return The the position of the first set bit or -1 when \c n is 0.
 */
extern int
shr_util_ffs(uint32_t n);

/*!
 * \brief Convert ASCII hex character to integer.
 *
 * This function will convert an ASCII hexadecimal character to an
 * integer value, for example '3' will be converted to 3 and 'b' will
 * be converted to 11.
 *
 * \param [in] ch ASCII hexadecimal character.
 *
 * \return Integer value of input character or -1 if invalid input.
 */
extern int
shr_util_xch2i(int ch);

/*!
 * \brief Convert a 32-bit word from host endian to big endian.
 *
 * \param [in] _dptr Pointer to destination word.
 * \param [in] _sptr Pointer to source word.
 *
 * \return Nothing.
 */
#define SHR_UTIL_HOST_WORD_TO_BE32(_dptr, _sptr) \
    do {                                         \
        uint8_t *_dst = (uint8_t *)(_dptr);      \
        uint32_t _d32 = *((uint32_t *)(_sptr));  \
        *_dst++ = (_d32 >> 24) & 0xff;           \
        *_dst++ = (_d32 >> 16) & 0xff;           \
        *_dst++ = (_d32 >> 8) & 0xff;            \
        *_dst++ = _d32 & 0xff;                   \
    } while (0)

/*!
 * \brief Convert a 32-bit word from host endian to little endian.
 *
 * \param [in] _dptr Pointer to destination word.
 * \param [in] _sptr Pointer to source word.
 *
 * \return Nothing.
 */
#define SHR_UTIL_HOST_WORD_TO_LE32(_dptr, _sptr) \
    do {                                         \
        uint8_t *_dst = (uint8_t *)(_dptr);      \
        uint32_t _d32 = *((uint32_t *)(_sptr));  \
        *_dst++ = _d32 & 0xff;                   \
        *_dst++ = (_d32 >> 8) & 0xff;            \
        *_dst++ = (_d32 >> 16) & 0xff;           \
        *_dst++ = (_d32 >> 24) & 0xff;           \
    } while (0)

/*!
 * \brief Copy buffer from host endian to big endian.
 *
 * When device DMA requires endianess handling in software, we
 * typically need to copy from host endian to a fixed endian
 * (determined by the device).
 *
 * This function copies a 32-bit-aligned buffer from host endian
 * layout to big endian layout and vice versa.
 *
 * \param [in] dst 32-bit-aligned destination buffer.
 * \param [in] src 32-bit-aligned source buffer.
 * \param [in] wlen Buffer length in 32-bit words.
 *
 * \retval 0 No errors.
 */
int
shr_util_host_to_be32(uint32_t *dst, uint32_t *src, size_t wlen);

/*!
 * \brief Copy buffer from host endian to little endian.
 *
 * When device DMA requires endianess handling in software, we
 * typically need to copy from host endian to a fixed endian
 * (determined by the device).
 *
 * This function copies a 32-bit-aligned buffer from host endian
 * layout to little endian layout and vice versa.
 *
 * \param [in] dst 32-bit-aligned destination buffer.
 * \param [in] src 32-bit-aligned source buffer.
 * \param [in] wlen Buffer length in 32-bit words.
 *
 * \retval 0 No errors.
 */
int
shr_util_host_to_le32(uint32_t *dst, uint32_t *src, size_t wlen);

/*!
 * \brief Implementation of realloc on top of sal_alloc API.
 *
 * Grow or shrink a block of memory while preserving the contents.
 *
 * If \c old_ptr is NULL, the call is equivalent to sal_alloc.
 *
 * If the function returns NULL, then the existing block of memory is
 * left unchanged.
 *
 * \param [in] old_ptr Pointer to current block of memory (or NULL).
 * \param [in] old_size Size of current block of memory.
 * \param [in] size Requested new size of memory block.
 * \param [in] str ID string for \c sal_alloc.
 *
 * \return Pointer to new block of memory or NULL on error.
 */
extern void *
shr_util_realloc(void *old_ptr, size_t old_size, size_t size, char *str);

#endif /* SHR_UTIL_H */
