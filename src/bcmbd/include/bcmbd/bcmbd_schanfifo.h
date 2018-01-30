/*! \file bcmbd_schanfifo.h
 *
 * SCHAN FIFO APIs
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

#ifndef BCMBD_SCHANFIFO_H
#define BCMBD_SCHANFIFO_H

/*! Maximum number of SCHAN FIFO devices supported. */
#define SCHANFIFO_DEV_NUM_MAX           BCMDRD_CONFIG_MAX_UNITS

/*!
 * \name Initialization control flags.
 * \anchor SCHANFIFO_IF_xxx
 */

/*! \{ */
/*! Poll wait for done status, otherwise use interrupt. */
#define SCHANFIFO_IF_POLL_WAIT          (1 << 0)
/*! Mask the SER related events from resulting in channel abort. */
#define SCHANFIFO_IF_IGNORE_SER_ABORT   (1 << 1)
/*! Use CCMDMA to write commands, otherwise use PIO write. */
#define SCHANFIFO_IF_CCMDMA_WR          (1 << 2)
/*! \} */

/*!
 * \name Operation control flags.
 * \anchor SCHANFIFO_OF_xxx
 */

/*! \{ */
/*! Set start bit, only for ops_send(). */
#define SCHANFIFO_OF_SET_START          (1 << 0)
/*! Wait for HW to complete, only for status_get(). */
#define SCHANFIFO_OF_WAIT_COMPLETE      (1 << 1)
/*! Clear start bit, only for status_get(). */
#define SCHANFIFO_OF_CLEAR_START        (1 << 2)
/*! \} */

/*!
 * \brief Attach SCHAN FIFO driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_schanfifo_attach(int unit);

/*!
 * \brief Detach SCHAN FIFO driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_schanfifo_detach(int unit);

/*!
 * \brief Get SCHAN FIFO information.
 *
 * \param [in] unit Unit number.
 * \param [out] num_chans Number of channels.
 * \param [out] cmd_mem_wsize Command memory size in 32-bit words.
 *
 * This function should always be called first to check if SCHAN FIFO
 * is supported. The resources supported by hardware will be returned if
 * successful.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_info_get(int unit, int *num_chans, size_t *cmd_mem_wsize);

/*!
 * \brief Initialize SCHAN FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] max_polls Maximum poll times for operations to complete.
 * \param [in] flags Control flags (\ref SCHANFIFO_IF_xxx).
 *
 * The function will reset all channels and save the control flags
 * which will take effect on all the subsequent ops_send() callings
 * until this function gets invoked again.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_init(int unit, uint32_t max_polls, uint32_t flags);

/*!
 * \brief Send operations to SCHAN FIFO channel.
 *
 * \param [in] unit Unit number.
 * \param [in] chan Channel number.
 * \param [in] num_ops Number of operations.
 * \param [in] req_buff Requested operations buffer.
 * \param [in] req_wsize Size of operations buffer in 32-bit words.
 * \param [in] flags Control flags (\ref SCHANFIFO_OF_xxx).
 *
 * This function will write command memory, configure registers, etc.
 * This function does not support SCHANFIFO_OF_WAIT_COMPLETE and
 * SCHANFIFO_OF_CLEAR_START. START bit will be set if SCHANFIFO_OF_SET_START
 * is asserted after sending the operations.
 * This function will not wait for HW to complete.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_ops_send(int unit, int chan, uint32_t num_ops,
                         uint32_t *req_buff, size_t req_wsize, uint32_t flags);

/*!
 * \brief Set start for SCHAN FIFO channel.
 *
 * \param [in] unit Unit number.
 * \param [in] chan Channel number.
 * \param [in] start Start value.
 *
 * This function will set the start value in HW register.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_set_start(int unit, int chan, bool start);

/*!
 * \brief Get SCHAN FIFO channel status.
 *
 * \param [in] unit Unit number.
 * \param [in] chan Channel number.
 * \param [in] num_ops Number of operations.
 * \param [in] flags Control flags (\ref SCHANFIFO_OF_xxx).
 * \param [out] done_ops Number of successful operations.
 * \param [out] resp_buff Responses buffer.
 *
 * This function will read the status register and return the number of
 * successful operations. The response buffer pointer will be returned
 * if resp_buff is not NULL.
 * The channel will be reset if operations fail.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_FAIL, Operation failed.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_status_get(int unit, int chan, uint32_t num_ops, uint32_t flags,
                           uint32_t *done_ops, uint32_t **resp_buff);

#endif /* BCMBD_SCHANFIFO_H */

