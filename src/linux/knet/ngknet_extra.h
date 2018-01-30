/*! \file ngknet_extra.h
 *
 * Generic data structure definitions for NGKNET enhancement.
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

#ifndef NGKNET_EXTRA_H
#define NGKNET_EXTRA_H

/*!
 * \brief Filter control.
 */
struct filt_ctrl {
    /*! List head */
    struct list_head list;

    /*! Device number */
    int dev_no;

    /*! Number of hits */
    uint64_t hits;

    /*! Filter description */
    ngknet_filter_t filt;
};

/*!
 * \brief Create filter.
 *
 * \param [in] dev Device structure point.
 * \param [in] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_filter_create(struct ngknet_dev *dev, ngknet_filter_t *filter);

/*!
 * \brief Destroy filter.
 *
 * \param [in] dev Device structure point.
 * \param [in] id Filter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_filter_destroy(struct ngknet_dev *dev, int id);

/*!
 * \brief Destroy all the filters.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_filter_destroy_all(struct ngknet_dev *dev);

/*!
 * \brief Get filter.
 *
 * \param [in] dev Device structure point.
 * \param [in] id Filter ID.
 * \param [out] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_filter_get(struct ngknet_dev *dev, int id, ngknet_filter_t *filter);

/*!
 * \brief Get the next filter.
 *
 * \param [in] dev Device structure point.
 * \param [out] filter Filter structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_filter_get_next(struct ngknet_dev *dev, ngknet_filter_t *filter);

/*!
 * \brief Filter packet.
 *
 * \param [in] dev Device structure point.
 * \param [in] skb Rx packet SKB.
 * \param [out] mndev Mirror network interface.
 * \param [out] mskb Mirror Rx packet SKB.
 *
 * \retval Matched network interface.
 * \retval NULL No matched network interface.
 */
extern int
ngknet_rx_pkt_filter(struct ngknet_dev *dev, struct sk_buff *skb, struct net_device **ndev,
                     struct net_device **mndev, struct sk_buff **mskb);

/*!
 * \brief Rx rate limit control.
 *
 * This contains all the control information for Rx rate limit such as
 * the number of Rx packets, status related to Rx rate limit, etc.
 *
 * The rate limit is kernel-oriented, i.e. all the Rx packets from any
 * device/channel will be accounted for. Once the received packets reach
 * the limit value in an 1-Sec interval, the driver API XXXX_rx_suspend()
 * will be called to suspend Rx. The 1-Sec basis timer will call the driver
 * API XXXX_rx_resume() to resume Rx and reset rate-related status/counters
 * at the begin of the next 1-Sec interval.
 *
 * The NGKNET module parameter 'rx_rate_limit' is used to decide the maximum
 * Rx rate. Disable Rx rate limit if set 0. It can be set when inserting
 * NGKNET module or modified using its SYSFS attributions.
 */
struct ngknet_rl_ctrl {
    /*! Rx packets */
    int rx_pkts;

    /*! Rx overruns */
    int rx_overruns;

    /*! Rx ticks */
    int rx_ticks;

    /*! Active devices under rate control */
    int dev_active[NUM_PDMA_DEV_MAX];

    /*! Paused devices due to no Rx credit */
    int dev_paused[NUM_PDMA_DEV_MAX];

    /*! Rate limit timer */
    struct timer_list timer;

    /*! Rate limit lock */
    spinlock_t lock;

    /*! Devices */
    struct ngknet_dev *devs;

    /*! Rate limit status indicator */
    int started;
};

/*!
 * \brief Initialize Rx rate limit.
 *
 * \param [in] devs Devices array.
 */
extern void
ngknet_rx_rate_limit_init(struct ngknet_dev *devs);

/*!
 * \brief Cleanup Rx rate limit.
 */
extern void
ngknet_rx_rate_limit_cleanup(void);

/*!
 * \brief Get Rx rate limit state.
 */
extern int
ngknet_rx_rate_limit_started(void);

/*!
 * \brief Start Rx rate limit.
 *
 * \param [in] dev Device structure point.
 */
extern void
ngknet_rx_rate_limit_start(struct ngknet_dev *dev);

/*!
 * \brief Stop Rx rate limit.
 *
 * \param [in] dev Device structure point.
 */
extern void
ngknet_rx_rate_limit_stop(struct ngknet_dev *dev);

/*!
 * \brief Limit Rx rate.
 *
 * \param [in] dev Device structure point.
 */
extern void
ngknet_rx_rate_limit(struct ngknet_dev *dev, int limit);

/*!
 * \brief Schedule Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 */
extern void
ngknet_tx_queue_schedule(struct ngknet_dev *dev, struct sk_buff *skb, int *queue);

#endif /* NGKNET_EXTRA_H */

