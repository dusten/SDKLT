/*! \file bcmlu_ngknet.h
 *
 * Generic data structure and macro definitions for user interfaces.
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

#ifndef BCMLU_NGKNET_H
#define BCMLU_NGKNET_H

/*!
 * \brief Check the kernel networking subsystem validity.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Check failed.
 */
extern int
bcmlu_ngknet_check(void);

/*!
 * \brief Set Rx rate limit.
 *
 * \param [in] rate_max Max Rx packets per second.
 * \param [in] set Set Rx rate limit if TRUE, or get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_rx_rate_limit(int *rate_max, int set);

/*!
 * \brief Initialize the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] dev_cfg Device configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_init(int unit, ngknet_dev_cfg_t *dev_cfg);

/*!
 * \brief Clean up the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_cleanup(int unit);

/*!
 * \brief Configure the kernel networking subsystem channel.
 *
 * \param [in] unit Device number.
 * \param [in] chan_cfg Channel configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_chan_config(int unit, ngknet_chan_cfg_t *chan_cfg);

/*!
 * \brief Query the kernel networking subsystem channel.
 *
 * \param [in] unit Device number.
 * \param [in] chan_cfg Channel configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_chan_query(int unit, ngknet_chan_cfg_t *chan_cfg);

/*!
 * \brief Suspend the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_suspend(int unit);

/*!
 * \brief Resume the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_resume(int unit);

/*!
 * \brief Configure RCPU header for the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] rcpu RCPU configure data structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_rcpu_config(int unit, struct ngknet_rcpu_hdr *rcpu);

/*!
 * \brief Get RCPU header for the kernel networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] rcpu RCPU configure data structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_rcpu_get(int unit, struct ngknet_rcpu_hdr *rcpu);

/*!
 * \brief Get the kernel networking subsystem information.
 *
 * \param [in] unit Device number.
 * \param [in] info Device information structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_info_get(int unit, struct bcmcnet_dev_info *info);

/*!
 * \brief Get the kernel networking subsystem stats.
 *
 * \param [in] unit Device number.
 * \param [in] stats Device stats structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_stats_get(int unit, struct bcmcnet_dev_stats *stats);

/*!
 * \brief Reset the kernel networking subsystem stats.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_dev_stats_reset(int unit);

/*!
 * \brief Create a kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_create(int unit, ngknet_netif_t *netif);

/*!
 * \brief Destroy a kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif_id Network interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_destroy(int unit, int netif_id);

/*!
 * \brief Get a kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif_id Network interface ID.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_get(int unit, int netif_id, ngknet_netif_t *netif);

/*!
 * \brief Get the next kernel network interface.
 *
 * \param [in] unit Device number.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_netif_get_next(int unit, ngknet_netif_t *netif);

/*!
 * \brief Create a kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_create(int unit, ngknet_filter_t *filter);

/*!
 * \brief Destroy a kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter_id Filter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_destroy(int unit, int filter_id);

/*!
 * \brief Get a kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter_id Filter ID.
 * \param [in] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_get(int unit, int filter_id, ngknet_filter_t *filter);

/*!
 * \brief Get the next kernel packet filter.
 *
 * \param [in] unit Device number.
 * \param [in] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_get_next(int unit, ngknet_filter_t *filter);

/*!
 * \brief Set the number of filter priorities per Rx queue.
 *
 * \param [in] unit Device number.
 * \param [in] num_prio Priorities number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL IOCTL failed.
 */
extern int
bcmlu_ngknet_filter_prio_num_set(int unit, int num_prio);

#endif /* BCMLU_NGKNET_H */

