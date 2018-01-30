/*! \file bcma_io_file.h
 *
 * File I/O.
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

#ifndef BCMA_IO_FILE_H
#define BCMA_IO_FILE_H

#include <stdarg.h>

/*! Opaque file handle */
typedef void *bcma_io_file_handle_t;

/*!
 * \brief Open a file.
 *
 * Open a file with name 'file' and mode 'mode'.
 * The supported modes string are :
 *
 * r    Open text file for reading.
 *
 * r+   Open  for  reading  and writing.
 *
 * w    Truncate file to zero length or create text file for writing.
 *
 * w+   Open for reading and writing. The file is created if it does not
 *
 *      exist, otherwise it is truncated.
 *
 * a    Open  for  appending  (writing at end of file).  The file is
 *
 *      created if it does not exist.
 *
 * a+   Open for reading and appending (writing at end of file). The
 *
 *      file is created if it does not exist.
 *
 * \param [in] filename Name of file to open
 * \param [in] mode File mode
 *
 * \return NULL or the opened file handle
 */
extern bcma_io_file_handle_t
bcma_io_file_open(const char *filename, const char *mode);

/*!
 * \brief Close a file.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 *
 * \return non-zero on error
 */
extern int
bcma_io_file_close(bcma_io_file_handle_t fh);

/*!
 * \brief Read data from file.
 *
 * Read 'num' elements of data, each 'size' bytes long from file.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 * \param [out] buf Output buffer contains data read
 * \param [in] size Size of each element of data to read
 * \param [in] num Number of elements of data to read
 *
 * \return the number of elements successfully read, or -1 on error
 */
extern int
bcma_io_file_read(bcma_io_file_handle_t fh, void *buf, int size, int num);

/*!
 * \brief Write data to file.
 *
 * Write 'num' elements of data, each 'size' bytes long to file.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 * \param [in] buf Buffer contains data to write
 * \param [in] size Size of each element of data to write
 * \param [in] num Number of elements of data to write
 *
 * \return the number of elements successfully write, or -1 on error
 */
extern int
bcma_io_file_write(bcma_io_file_handle_t fh, const void *buf, int size, int num);

/*!
 * \brief Read string from file.
 *
 * Read in at most one less than 'size' characters from file
 * and stores them into the buffer. Reading stops after an EOF or a new line.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 * \param [out] buf Output buffer contains data read
 * \param [in] size Size of maximum read characters
 *
 * \return 'buf' on success, and NULL on error or EOF
 */
extern char *
bcma_io_file_gets(bcma_io_file_handle_t fh, char *buf, int size);

/*!
 * \brief Read character from file.
 *
 * Read the next character from file and returns it.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 *
 * \return the character read, or EOF on end of file or error
 */
extern int
bcma_io_file_getc(bcma_io_file_handle_t fh);

/*!
 * \brief Write string to file.
 *
 * Write string 'buf' to file without its trailing '\0'.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 * \param [in] buf String to write
 *
 * \return non-negative on success
 */
extern int
bcma_io_file_puts(bcma_io_file_handle_t fh, const char *buf);

/*!
 * \brief Write character to file.
 *
 * Write the character 'c' to file.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 * \param [in] c Character to write
 *
 * \return the character written on success
 */
extern int
bcma_io_file_putc(bcma_io_file_handle_t fh, int c);

/*!
 * \brief Flush a file.
 *
 * The function forces a write of all user-space buffered-data to file.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 *
 * \return 0 on success
 */
extern int
bcma_io_file_flush(bcma_io_file_handle_t fh);

/*!
 * \brief Print formatted string to file.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 * \param [in] format Format string
 *
 * \return Number fof characters written or negative value on error
 */
extern int
bcma_io_file_printf(bcma_io_file_handle_t fh, const char *format, ...);

/*!
 * \brief Print formatted string to file.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 * \param [in] format Format string (vfprintf-compatible)
 * \param [in] ap Argument list (vfprintf-compatible)
 *
 * \return Number fof characters written or negative value on error
 */
extern int
bcma_io_file_vprintf(bcma_io_file_handle_t fh, const char *format, va_list ap);

/*!
 * \brief Get file size.
 *
 * The file position would be set to the beginning of file after
 * this function is called.
 *
 * \param [in] fh File handle returned from bcma_io_file_open
 *
 * \return File size or -1 on error
 */
extern int
bcma_io_file_size(bcma_io_file_handle_t fh);

/*!
 * \brief Delete a file from the filesystem.
 *
 * \param [in] filename File name to be deleted
 *
 * \return 0 on success or -1 on error
 */
int
bcma_io_file_delete(const char *filename);

/*!
 * \brief Rename a file.
 *
 * \param [in] old_filename Old file name to be renamed from
 * \param [in] new_filename New file name to be renamed to
 *
 * \return 0 on success or -1 on error
 */
int
bcma_io_file_move(const char *old_filename, const char *new_filename);

#endif /* BCMA_IO_FILE_H */
