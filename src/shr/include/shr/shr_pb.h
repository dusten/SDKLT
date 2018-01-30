/*! \file shr_pb.h
 *
 * This file contains APIs for dynamic print buffer support, which is
 * mainly intended for use with the BSL framework.
 *
 * The basic idea is to have a sprintf-like function, which always
 * allocates a buffer sufficiently large to hold the resulting string.
 *
 * Furthermore, subsequent writes to the same buffer will be appended
 * to the existing string instead of overwriting it. This is useful
 * e.g. when a driver wants to build a log message from multiple
 * pieces before sending it to the system log.
 *
 * Example demonstrating how entire lines are sent to printf:
 *
 * \code{.c}
 * {
 *     shr_pb_t *pb;
 *     int idx;
 *
 *     pb = shr_pb_create();
 *     for (idx = 0; idx < 23; idx++) {
 *         if (idx > 0 && (idx & 0x7) == 0) {
 *             shr_pb_printf(pb, "\n");
 *             printf("%s", shr_pb_str(pb));
 *             shr_pb_reset(pb);
 *         }
 *         shr_pb_printf(pb, "0x%02x ", idx);
 *     }
 *     shr_pb_printf(pb, "\n");
 *     printf("%s", shr_pb_str(pb));
 *     shr_pb_destroy(pb);
 * }
 * \endcode
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

#ifndef SHR_PB_H
#define SHR_PB_H

/*! Format checks do not need to be portable */
#ifndef SHR_PB_ATTR
#  if defined(__GNUC__) && !defined(__PEDANTIC__)
#    define SHR_PB_ATTR(_a) __attribute__(_a)
#  else
#    define SHR_PB_ATTR(_a)
#  endif
#endif

/*! By default the print buffer is expanded in chunks of this size. */
#define SHR_PB_CHUNK_SIZE_DEFAULT       512

/*! Minimum chunk size (\ref SHR_PB_CHUNK_SIZE_DEFAULT). */
#define SHR_PB_CHUNK_SIZE_MIN           8

/*! Maximum chunk size (\ref SHR_PB_CHUNK_SIZE_DEFAULT). */
#define SHR_PB_CHUNK_SIZE_MAX           (2 * 1024)

/*! Error string to indicate failure from the print buffer. */
#define SHR_PB_ERR_STR                  "<PBERR>"

/*!
 * \brief Print buffer object.
 */
typedef struct shr_pb_s {

    /*! Allocated buffer holding the current string. */
    char *buf;

    /*! Size of currently allocated buffer. */
    int bufsz;

    /*! Length of the current string. */
    int len;

    /*! Reset marker. */
    int mark;

} shr_pb_t;

/*!
 * \brief Create print buffer object.
 *
 * After use, the object should be destroyed by calling \ref
 * shr_pb_destroy.
 *
 * \return Pointer to print buffer object, or NULL on error.
 */
extern shr_pb_t *
shr_pb_create(void);

/*!
 * \brief Destroy print buffer object.
 *
 * Destroy a print buffer object created using \ref shr_pb_create.
 *
 * \param [in] pb Print buffer object
 */
extern void
shr_pb_destroy(shr_pb_t *pb);

/*!
 * \brief Append formatted string to a print buffer.
 *
 * This function appends a formatted string to the existing print
 * buffer using the standard printf syntax.
 *
 * \param [in] pb Print buffer object
 * \param [in] fmt printf format string
 *
 * \return Number of charaters added, or -1 on error.
 */
extern int
shr_pb_printf(shr_pb_t *pb, const char *fmt, ...)
    SHR_PB_ATTR ((format (printf, 2, 3)));

/*!
 * \brief Set reset marker position in a print buffer.
 *
 * The marker allows the caller to reset the print buffer to a
 * specific length. The feature can be used to recover an earlier
 * state or to remove one or more characters from the tail of the
 * print buffer.
 *
 * If a negative value is passed in, the marker will be set at the end
 * of the current string in the print buffer.
 *
 * \param [in] pb Print buffer object.
 * \param [in] offs New position of reset marker.
 *
 * \return Offset of the current reset marker.
 */
extern int
shr_pb_mark(shr_pb_t *pb, int offs);

/*!
 * \brief Reset print buffer object.
 *
 * Sets the current string length to zero. This function should be
 * called when a new string is created using an existing print buffer.
 *
 * \param [in] pb Print buffer object
 *
 * \return Nothing.
 */
extern void
shr_pb_reset(shr_pb_t *pb);

/*!
 * \brief Get current string in the print buffer.
 *
 * Note that this function will always return a valid string, so no
 * error checking is necessary. In case of an error a special error
 * string will be returned.
 *
 * \param [in] pb Print buffer object
 *
 * \return Pointer to the current string, or "<PBERR>" if error.
 */
extern const char *
shr_pb_str(shr_pb_t *pb);

/*!
 * \brief Set the print buffer chunk size.
 *
 * If the print buffer is too small to hold a string, then it will be
 * expanded in chunks of this size.
 *
 * To get the current chunk size, pass in a value of zero.
 *
 * To reset the chunk size to the default value, pass in -1.
 *
 * \param [in] csize New chunk size.
 *
 * \return Current chunk size.
 */
extern int
shr_pb_chunk_size(int csize);

#endif  /* !SHR_PB_H */
