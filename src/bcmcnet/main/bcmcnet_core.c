/*! \file bcmcnet_core.c
 *
 * Utility routines for BCMCNET driver.
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

#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>

/*!
 * Initialize a device
 */
int
bcmcnet_pdma_dev_init(struct pdma_dev *dev)
{
    int rv;

    /* Open the device */
    rv = bcmcnet_pdma_open(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    dev->attached = 1;

    return SHR_E_NONE;
}

/*!
 * De-initialize a device
 */
int
bcmcnet_pdma_dev_deinit(struct pdma_dev *dev)
{
    if (!dev->attached) {
        return SHR_E_NONE;
    }

    dev->ops->dev_close(dev);
    dev->ops = NULL;

    dev->attached = 0;

    return SHR_E_NONE;
}

/*!
 * Start a device
 */
int
bcmcnet_pdma_dev_start(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    uint32_t qi;
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    rv = dev->ops->dev_config(dev, ctrl->bm_rxq, ctrl->bm_txq);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Start all the Rx queues */
    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        rv = dev->ops->rx_queue_setup(dev, qi);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        dev->ops->rx_queue_intr_enable(dev, qi);
        dev->ops->rx_queue_start(dev, qi);
    }

    /* Start all the Tx queues */
    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        dev->ops->tx_queue_setup(dev, qi);
        dev->ops->tx_queue_intr_enable(dev, qi);
        dev->ops->tx_queue_start(dev, qi);
        dev->ops->tx_queue_wakeup(dev, qi);
    }

    bcmcnet_pdma_dev_info_get(dev);

    return SHR_E_NONE;
}

/*!
 * Stop a device
 */
int
bcmcnet_pdma_dev_stop(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    uint32_t qi;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    /* Stop all the Rx queues */
    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        dev->ops->rx_queue_stop(dev, qi);
        dev->ops->rx_queue_intr_disable(dev, qi);
        dev->ops->rx_queue_release(dev, qi);
    }

    /* Stop all the Tx queues */
    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        dev->ops->tx_queue_stop(dev, qi);
        dev->ops->tx_queue_wakeup(dev, qi);
        dev->ops->tx_queue_intr_disable(dev, qi);
        dev->ops->tx_queue_release(dev, qi);
    }

    return SHR_E_NONE;
}

/*!
 * Suspend a device
 */
int
bcmcnet_pdma_dev_suspend(struct pdma_dev *dev)
{
    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    return dev->ops->dev_suspend(dev);
}

/*!
 * Resume a device
 */
int
bcmcnet_pdma_dev_resume(struct pdma_dev *dev)
{
    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    return dev->ops->dev_resume(dev);
}

/*!
 * Suspend Rx
 */
int
bcmcnet_pdma_dev_rx_suspend(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    uint32_t qi;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    /* Suspend all the Rx queues */
    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        dev->ops->rx_queue_suspend(dev, qi);
    }

    return SHR_E_NONE;
}

/*!
 * Resume Rx
 */
int
bcmcnet_pdma_dev_rx_resume(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    uint32_t qi;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    /* Resume all the Rx queues */
    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        dev->ops->rx_queue_resume(dev, qi);
    }

    return SHR_E_NONE;
}

/*!
 * Get device information
 */
int
bcmcnet_pdma_dev_info_get(struct pdma_dev *dev)
{
    if (!dev->ops || !dev->ops->dev_info_get) {
        return SHR_E_INTERNAL;
    }

    dev->ops->dev_info_get(dev);

    return SHR_E_NONE;
}

/*!
 * Get device statistics
 */
int
bcmcnet_pdma_dev_stats_get(struct pdma_dev *dev)
{
    if (!dev->ops || !dev->ops->dev_stats_get) {
        return SHR_E_INTERNAL;
    }

    dev->ops->dev_stats_get(dev);

    return SHR_E_NONE;
}

/*!
 * Reset device statistics
 */
int
bcmcnet_pdma_dev_stats_reset(struct pdma_dev *dev)
{
    if (!dev->ops || !dev->ops->dev_stats_reset) {
        return SHR_E_INTERNAL;
    }

    dev->ops->dev_stats_reset(dev);

    return SHR_E_NONE;
}

/*!
 * Convert a queue index to channel index
 */
int
bcmcnet_pdma_dev_queue_to_chan(struct pdma_dev *dev, int queue, int dir, int *chan)
{
    struct dev_ctrl *ctrl = &dev->ctrl;

    if (dir == PDMA_Q_RX) {
        if (queue < 0 || queue > (int)ctrl->nb_rxq || chan == NULL) {
            return SHR_E_PARAM;
        }
    } else {
        if (queue < 0 || queue > (int)ctrl->nb_txq || chan == NULL) {
            return SHR_E_PARAM;
        }
    }

    if (!dev->ops || !dev->ops->dev_lq_to_pq) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->dev_lq_to_pq(dev, queue, dir, chan);
}

/*!
 * Convert a channel index to queue index
 */
int
bcmcnet_pdma_dev_chan_to_queue(struct pdma_dev *dev, int chan, int *queue, int *dir)
{
    if (chan < 0 || chan > dev->num_queues || queue == NULL || dir == NULL) {
        return SHR_E_PARAM;
    }

    if (!dev->ops || !dev->ops->dev_pq_to_lq) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->dev_pq_to_lq(dev, chan, queue, dir);
}

/*!
 * Enable interrupt for a Rx queue
 */
int
bcmcnet_rx_queue_intr_enable(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->rx_queue_intr_enable) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->rx_queue_intr_enable(dev, queue);
}

/*!
 * Disable interrupt for a Rx queue
 */
int
bcmcnet_rx_queue_intr_disable(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->rx_queue_intr_disable) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->rx_queue_intr_disable(dev, queue);
}

/*!
 * Acknowledge interrupt for a Rx queue
 */
int
bcmcnet_rx_queue_intr_ack(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->rx_queue_intr_ack) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->rx_queue_intr_ack(dev, queue);
}

/*!
 * Check interrupt for a Rx queue
 */
int
bcmcnet_rx_queue_intr_check(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->rx_queue_intr_check) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->rx_queue_intr_check(dev, queue);
}

/*!
 * Enable interrupt for a Tx queue
 */
int
bcmcnet_tx_queue_intr_enable(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->tx_queue_intr_enable) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->tx_queue_intr_enable(dev, queue);
}

/*!
 * Disable interrupt for a Tx queue
 */
int
bcmcnet_tx_queue_intr_disable(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->tx_queue_intr_disable) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->tx_queue_intr_disable(dev, queue);
}

/*!
 * Acknowledge interrupt for a Tx queue
 */
int
bcmcnet_tx_queue_intr_ack(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->tx_queue_intr_ack) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->tx_queue_intr_ack(dev, queue);
}

/*!
 * Check interrupt for a Tx queue
 */
int
bcmcnet_tx_queue_intr_check(struct pdma_dev *dev, int queue)
{
    if (!dev->ops || !dev->ops->tx_queue_intr_check) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->tx_queue_intr_check(dev, queue);
}

/*!
 * Enable interrupt for a queue
 */
int
bcmcnet_queue_intr_enable(struct pdma_dev *dev, struct intr_handle *hdl)
{
    if (hdl->dir == PDMA_Q_RX) {
        return bcmcnet_rx_queue_intr_enable(dev, hdl->queue);
    } else {
        return bcmcnet_tx_queue_intr_enable(dev, hdl->queue);
    }
}

/*!
 * Disable interrupt for a queue
 */
int
bcmcnet_queue_intr_disable(struct pdma_dev *dev, struct intr_handle *hdl)
{
    if (hdl->dir == PDMA_Q_RX) {
        return bcmcnet_rx_queue_intr_disable(dev, hdl->queue);
    } else {
        return bcmcnet_tx_queue_intr_disable(dev, hdl->queue);
    }
}

/*!
 * Acknowledge interrupt for a queue
 */
int
bcmcnet_queue_intr_ack(struct pdma_dev *dev, struct intr_handle *hdl)
{
    if (hdl->dir == PDMA_Q_RX) {
        return bcmcnet_rx_queue_intr_ack(dev, hdl->queue);
    } else {
        return bcmcnet_tx_queue_intr_ack(dev, hdl->queue);
    }
}

/*!
 * Check interrupt for a queue
 */
int
bcmcnet_queue_intr_check(struct pdma_dev *dev, struct intr_handle *hdl)
{
    if (hdl->dir == PDMA_Q_RX) {
        return bcmcnet_rx_queue_intr_check(dev, hdl->queue);
    } else {
        return bcmcnet_tx_queue_intr_check(dev, hdl->queue);
    }
}

/*!
 * Enable interrupt for a queue group
 */
int
bcmcnet_group_intr_enable(struct pdma_dev *dev, int group)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct queue_group *grp = &ctrl->grp[group];
    int queue, dir;
    int i;

    if (!dev->ops) {
        return SHR_E_INTERNAL;
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_rxq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            dev->ops->rx_queue_intr_enable(dev, queue);
        }
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_txq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            dev->ops->tx_queue_intr_enable(dev, queue);
        }
    }

    return SHR_E_NONE;
}

/*!
 * Disable interrupt for a queue group
 */
int
bcmcnet_group_intr_disable(struct pdma_dev *dev, int group)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct queue_group *grp = &ctrl->grp[group];
    int queue, dir;
    int i;

    if (!dev->ops) {
        return SHR_E_INTERNAL;
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_rxq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            dev->ops->rx_queue_intr_disable(dev, queue);
        }
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_txq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            dev->ops->tx_queue_intr_disable(dev, queue);
        }
    }

    return SHR_E_NONE;
}

/*!
 * Acknowledge interrupt for a queue group
 */
int
bcmcnet_group_intr_ack(struct pdma_dev *dev, int group)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct queue_group *grp = &ctrl->grp[group];
    int queue, dir;
    int i;

    if (!dev->ops) {
        return SHR_E_INTERNAL;
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_rxq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            dev->ops->rx_queue_intr_ack(dev, queue);
        }
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_txq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            dev->ops->tx_queue_intr_ack(dev, queue);
        }
    }

    return SHR_E_NONE;
}

/*!
 * Check interrupt for a queue group
 */
int
bcmcnet_group_intr_check(struct pdma_dev *dev, int group)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct queue_group *grp = &ctrl->grp[group];
    int queue, dir;
    int i;

    if (!dev->ops) {
        return SHR_E_INTERNAL;
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_rxq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            if (dev->ops->rx_queue_intr_check(dev, queue)) {
                return TRUE;
            }
        }
    }

    for (i = 0; i < dev->grp_queues; i++) {
        if (1 << i & grp->bm_txq) {
            dev->ops->dev_pq_to_lq(dev, i + group * dev->grp_queues, &queue, &dir);
            if (dev->ops->tx_queue_intr_check(dev, queue)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*!
 * Poll a Rx queue
 */
int
bcmcnet_rx_queue_poll(struct pdma_dev *dev, int queue, int budget)
{
    if (!dev->ops || !dev->ops->rx_queue_poll) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->rx_queue_poll(dev, queue, budget);
}

/*!
 * Poll a Tx queue
 */
int
bcmcnet_tx_queue_poll(struct pdma_dev *dev, int queue, int budget)
{
    if (!dev->ops || !dev->ops->tx_queue_poll) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->tx_queue_poll(dev, queue, budget);
}

/*!
 * Poll a queue
 */
int
bcmcnet_queue_poll(struct pdma_dev *dev, struct intr_handle *hdl, int budget)
{
    if (hdl->dir == PDMA_Q_RX) {
        return bcmcnet_rx_queue_poll(dev, hdl->queue, budget);
    } else {
        return bcmcnet_tx_queue_poll(dev, hdl->queue, budget);
    }
}

/*!
 * Poll a queue group
 */
int
bcmcnet_group_poll(struct pdma_dev *dev, int group, int budget)
{
    if (!dev->ops || !dev->ops->group_poll) {
        return SHR_E_INTERNAL;
    }

    return dev->ops->group_poll(dev, group, budget);
}

