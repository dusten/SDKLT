/*! \file bcmcnet_dev.c
 *
 * Utility routines for BCMCNET device.
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
#include <bcmcnet/bcmcnet_rxtx.h>
#include <bcmcnet/bcmcnet_buff.h>

/*!
 * Free resource for a Rx queue
 */
static void
bcn_rx_queues_free(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    int gi, qi;

    for (gi = 0; gi < dev->num_groups; gi++) {
        for (qi = 0; qi < dev->grp_queues; qi++) {
            rxq = (struct pdma_rx_queue *)ctrl->grp[gi].rx_queue[qi];
            if (!rxq) {
                continue;
            }
            sal_free(rxq);
            ctrl->grp[gi].rx_queue[qi] = NULL;
        }
    }
}

/*!
 * Allocate resource for a Rx queue
 */
static int
bcn_rx_queues_alloc(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    int gi, qi;

    for (gi = 0; gi < dev->num_groups; gi++) {
        for (qi = 0; qi < dev->grp_queues; qi++) {
            rxq = sal_alloc(sizeof(*rxq), "rx_queue");
            if (!rxq) {
                goto error;
            }
            sal_memset(rxq, 0, sizeof(*rxq));
            rxq->group_id = gi;
            rxq->chan_id = qi + gi * dev->grp_queues;
            rxq->ctrl = ctrl;
            ctrl->grp[gi].rx_queue[qi] = rxq;
        }
    }

    return SHR_E_NONE;

error:
    bcn_rx_queues_free(dev);

    return SHR_E_MEMORY;
}

/*!
 * Free resource for a Tx queue
 */
static void
bcn_tx_queues_free(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_tx_queue *txq = NULL;
    int gi, qi;

    for (gi = 0; gi < dev->num_groups; gi++) {
        for (qi = 0; qi < dev->grp_queues; qi++) {
            txq = (struct pdma_tx_queue *)ctrl->grp[gi].tx_queue[qi];
            if (!txq) {
                continue;
            }
            sal_free(txq);
            ctrl->grp[gi].tx_queue[qi] = NULL;
        }
    }
}

/*!
 * Allocate resource for a Tx queue
 */
static int
bcn_tx_queues_alloc(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_tx_queue *txq = NULL;
    int gi, qi;

    for (gi = 0; gi < dev->num_groups; gi++) {
        for (qi = 0; qi < dev->grp_queues; qi++) {
            txq = sal_alloc(sizeof(*txq), "tx_queue");
            if (!txq) {
                goto error;
            }
            sal_memset(txq, 0, sizeof(*txq));
            txq->group_id = gi;
            txq->chan_id = qi + gi * dev->grp_queues;
            txq->ctrl = ctrl;
            ctrl->grp[gi].tx_queue[qi] = txq;
        }
    }

    return SHR_E_NONE;

error:
    bcn_tx_queues_free(dev);

    return SHR_E_MEMORY;
}

/*!
 * \brief Parse Rx groups
 *
 * \param [in] dev Device structure point.
 * \param [in] qbm Rx queue bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
bcn_rx_queue_group_parse(struct pdma_dev *dev, uint32_t qbm)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    struct intr_handle *hdl = NULL;
    uint32_t mask;
    int gi, qi, qn;

    ctrl->nb_rxq = 0;
    sal_memset(ctrl->rx_queue, 0, sizeof(ctrl->rx_queue));

    /* Figure out available groups and Rx queues */
    for (gi = 0; gi < dev->num_groups; gi++) {
        if (!ctrl->grp[gi].attached) {
            continue;
        }
        qn = 0;
        mask = 0;
        for (qi = 0; qi < dev->grp_queues; qi++) {
            rxq = (struct pdma_rx_queue *)ctrl->grp[gi].rx_queue[qi];
            hdl = &ctrl->grp[gi].intr_hdl[qi];
            if (1 << (qi + gi * dev->grp_queues) & qbm) {
                /* Set the number of descriptors */
                rxq->nb_desc = ctrl->grp[gi].nb_desc[qi];
                if (!rxq->nb_desc) {
                    rxq->nb_desc = ctrl->nb_desc;
                    ctrl->grp[gi].nb_desc[qi] = rxq->nb_desc;
                }
                /* Set Rx buffer size */
                rxq->buf_size = ctrl->grp[gi].rx_size[qi];
                if (rxq->buf_size < RX_BUF_SIZE_MIN) {
                    rxq->buf_size = RX_BUF_SIZE_MIN;
                    ctrl->grp[gi].rx_size[qi] = rxq->buf_size;
                } else if (rxq->buf_size > RX_BUF_SIZE_MAX) {
                    rxq->buf_size = ctrl->rx_buf_size;
                    ctrl->grp[gi].rx_size[qi] = rxq->buf_size;
                }
                /* Set mode and state for the queue */
                rxq->mode = bcmcnet_rx_buff_mode(rxq->buf_size);
                rxq->state |= PDMA_RX_QUEUE_USED;
                if (dev->flags & PDMA_RX_BATCHING) {
                    rxq->free_thresh = rxq->nb_desc / 4;
                    rxq->state |= PDMA_RX_BATCH_REFILL;
                }
                /* Update queue index */
                rxq->queue_id = ctrl->nb_rxq;
                ctrl->rx_queue[rxq->queue_id] = rxq;
                ctrl->nb_rxq++;
                qn++;
                mask |= 1 << qi;
                /* Set up handler for the queue */
                hdl->queue = rxq->queue_id;
                hdl->dir = PDMA_Q_RX;
                hdl->budget = ctrl->budget < rxq->nb_desc ?
                              ctrl->budget : rxq->nb_desc;
            } else {
                rxq->state &= ~PDMA_RX_QUEUE_USED;
            }
        }

        /* Set group metadata */
        if (qn) {
            ctrl->grp[gi].bm_rxq = mask;
            ctrl->grp[gi].nb_rxq = qn;
        } else {
            ctrl->grp[gi].bm_rxq = 0;
            ctrl->grp[gi].nb_rxq = 0;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Parse Tx groups
 *
 * \param [in] dev Device structure point.
 * \param [in] qbm Tx queue bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
bcn_tx_queue_group_parse(struct pdma_dev *dev, uint32_t qbm)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_tx_queue *txq = NULL;
    struct intr_handle *hdl = NULL;
    uint32_t mask;
    int gi, qi, qn;

    ctrl->nb_txq = 0;
    sal_memset(ctrl->tx_queue, 0, sizeof(ctrl->tx_queue));

    /* Figure out available groups and Tx queues */
    for (gi = 0; gi < dev->num_groups; gi++) {
        if (!ctrl->grp[gi].attached) {
            continue;
        }
        qn = 0;
        mask = 0;
        for (qi = 0; qi < dev->grp_queues; qi++) {
            txq = (struct pdma_tx_queue *)ctrl->grp[gi].tx_queue[qi];
            hdl = &ctrl->grp[gi].intr_hdl[qi];
            if (1 << (qi + gi * dev->grp_queues) & qbm) {
                /* Set the number of descriptors */
                txq->nb_desc = ctrl->grp[gi].nb_desc[qi];
                if (!txq->nb_desc) {
                    txq->nb_desc = ctrl->nb_desc;
                    ctrl->grp[gi].nb_desc[qi] = txq->nb_desc;
                }
                /* Set mode and state for the queue */
                txq->state |= PDMA_TX_QUEUE_USED;
                if (dev->flags & PDMA_TX_POLLING) {
                    txq->free_thresh = txq->nb_desc / 4;
                    txq->state |= PDMA_TX_QUEUE_POLL;
                }
                /* Update queue index */
                txq->queue_id = ctrl->nb_txq;
                ctrl->tx_queue[txq->queue_id] = txq;
                ctrl->nb_txq++;
                qn++;
                mask |= 1 << qi;
                /* Set up handler for the queue */
                hdl->queue = txq->queue_id;
                hdl->dir = PDMA_Q_TX;
                hdl->budget = ctrl->budget < txq->nb_desc ?
                              ctrl->budget : txq->nb_desc;
            } else {
                txq->state &= ~PDMA_TX_QUEUE_USED;
            }
        }

        /* Set group metadata */
        if (qn) {
            ctrl->grp[gi].bm_txq = mask;
            ctrl->grp[gi].nb_txq = qn;
        } else {
            ctrl->grp[gi].bm_txq = 0;
            ctrl->grp[gi].nb_txq = 0;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Configure device
 *
 * \param [in] dev Device structure point.
 * \param [in] bm_rxq Rx queue bitmap.
 * \param [in] bm_txq Tx queue bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
bcmcnet_pdma_config(struct pdma_dev *dev, uint32_t bm_rxq, uint32_t bm_txq)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    int gi;

    if (!bm_rxq || !bm_txq || (bm_rxq & bm_txq)) {
        return SHR_E_PARAM;
    }

    bcn_rx_queue_group_parse(dev, bm_rxq);
    bcn_tx_queue_group_parse(dev, bm_txq);

    for (gi = 0; gi < dev->num_groups; gi++) {
        if (!ctrl->grp[gi].attached) {
            continue;
        }
        /* Update group metadata */
        if (!ctrl->grp[gi].bm_rxq && !ctrl->grp[gi].bm_txq) {
            ctrl->grp[gi].attached = 0;
            ctrl->bm_grp &= ~(1 << gi);
            ctrl->nb_grp--;
            continue;
        }
        ctrl->grp[gi].ctrl = ctrl;
        ctrl->grp[gi].id = gi;
        ctrl->grp[gi].irq_mask = 0;
    }

    return hw->hdls.hw_config(hw);
}

/*!
 * Close device
 */
static int
bcmcnet_pdma_close(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)dev->ctrl.hw;
    int gi;

    hw->hdls.hw_reset(hw);

    for (gi = 0; gi < dev->num_groups; gi++) {
        if (!ctrl->grp[gi].attached) {
            continue;
        }
        /* Reset group metadata */
        ctrl->bm_grp &= ~(1 << gi);
        ctrl->nb_grp--;
        ctrl->grp[gi].irq_mask = 0;
        ctrl->grp[gi].all_done = 1;
        ctrl->grp[gi].attached = 0;
    }

    bcn_rx_queues_free(dev);
    bcn_tx_queues_free(dev);

    return SHR_E_NONE;
}

/*!
 * Suspend device
 */
static int
bcmcnet_pdma_suspend(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    uint32_t qi;

    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        bcmcnet_pdma_rx_queue_suspend(dev, qi);
    }

    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        bcmcnet_pdma_tx_queue_suspend(dev, qi);
    }

    return SHR_E_NONE;
}

/*!
 * Resume device
 */
static int
bcmcnet_pdma_resume(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    uint32_t qi;

    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        bcmcnet_pdma_rx_queue_resume(dev, qi);
    }

    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        bcmcnet_pdma_tx_queue_resume(dev, qi);
    }

    return SHR_E_NONE;
}

/*!
 * Get device information
 */
static void
bcmcnet_pdma_info_get(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;
    struct pdma_tx_queue *txq = NULL;
    uint32_t qi;

    sal_strncpy(dev->info.dev_name, dev->name, sizeof(dev->info.dev_name) - 1);
    dev->info.dev_id = dev->dev_id;
    dev->info.dev_type = dev->dev_type;
    dev->info.max_groups = hw->info.num_cmcs;
    dev->info.max_queues = hw->info.num_chans;
    dev->info.bm_groups = ctrl->bm_grp;
    dev->info.bm_rx_queues = ctrl->bm_rxq;
    dev->info.bm_tx_queues = ctrl->bm_txq;
    dev->info.nb_groups = ctrl->nb_grp;
    dev->info.nb_rx_queues = ctrl->nb_rxq;
    dev->info.nb_tx_queues = ctrl->nb_txq;
    dev->info.rx_desc_size = hw->info.rx_dcb_size;
    dev->info.tx_desc_size = hw->info.tx_dcb_size;
    dev->info.rx_ph_size = hw->info.rx_ph_size;
    dev->info.tx_ph_size = hw->info.tx_ph_size;
    dev->info.rx_buf_dflt = ctrl->rx_buf_size;
    dev->info.nb_desc_dflt = ctrl->nb_desc;

    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        rxq = (struct pdma_rx_queue *)ctrl->rx_queue[qi];
        if (!rxq) {
            continue;
        }
        dev->info.rx_buf_size[qi] = rxq->buf_size;
        dev->info.nb_rx_desc[qi] = rxq->nb_desc;
    }

    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        txq = (struct pdma_tx_queue *)ctrl->tx_queue[qi];
        if (!txq) {
            continue;
        }
        dev->info.nb_tx_desc[qi] = txq->nb_desc;
    }
}

/*!
 * Get device statistics
 */
static void
bcmcnet_pdma_stats_get(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    struct pdma_tx_queue *txq = NULL;
    uint32_t packets = 0, bytes = 0, dropped = 0, errors = 0, nomems = 0, xoffs = 0;
    uint32_t head_errors = 0, data_errors = 0, cell_errors = 0;
    uint32_t qi;

    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        rxq = (struct pdma_rx_queue *)ctrl->rx_queue[qi];
        if (!rxq) {
            continue;
        }
        packets += rxq->stats.packets;
        bytes += rxq->stats.bytes;
        dropped += rxq->stats.dropped;
        errors += rxq->stats.errors;
        head_errors += rxq->stats.head_errors;
        data_errors += rxq->stats.data_errors;
        cell_errors += rxq->stats.cell_errors;
        nomems += rxq->stats.nomems;
        dev->stats.rxq_packets[qi] = rxq->stats.packets;
        dev->stats.rxq_bytes[qi] = rxq->stats.bytes;
        dev->stats.rxq_dropped[qi] = rxq->stats.dropped;
        dev->stats.rxq_errors[qi] = rxq->stats.errors;
        dev->stats.rxq_head_errors[qi] = rxq->stats.head_errors;
        dev->stats.rxq_data_errors[qi] = rxq->stats.data_errors;
        dev->stats.rxq_cell_errors[qi] = rxq->stats.cell_errors;
        dev->stats.rxq_nomems[qi] = rxq->stats.nomems;
    }

    dev->stats.rx_packets = packets;
    dev->stats.rx_bytes = bytes;
    dev->stats.rx_dropped = dropped;
    dev->stats.rx_errors = errors;
    dev->stats.rx_head_errors = head_errors;
    dev->stats.rx_data_errors = data_errors;
    dev->stats.rx_cell_errors = cell_errors;
    dev->stats.rx_nomems = nomems;

    packets = bytes = dropped = errors = 0;
    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        txq = (struct pdma_tx_queue *)ctrl->tx_queue[qi];
        if (!txq) {
            continue;
        }
        packets += txq->stats.packets;
        bytes += txq->stats.bytes;
        dropped += txq->stats.dropped;
        errors += txq->stats.errors;
        xoffs += txq->stats.xoffs;
        dev->stats.txq_packets[qi] = txq->stats.packets;
        dev->stats.txq_bytes[qi] = txq->stats.bytes;
        dev->stats.txq_dropped[qi] = txq->stats.dropped;
        dev->stats.txq_errors[qi] = txq->stats.errors;
        dev->stats.txq_xoffs[qi] = txq->stats.xoffs;
    }

    dev->stats.tx_packets = packets;
    dev->stats.tx_bytes = bytes;
    dev->stats.tx_dropped = dropped;
    dev->stats.tx_errors = errors;
    dev->stats.tx_xoffs = xoffs;
}

/*!
 * Reset device statistics
 */
static void
bcmcnet_pdma_stats_reset(struct pdma_dev *dev)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    struct pdma_tx_queue *txq = NULL;
    uint32_t qi;

    sal_memset(&dev->stats, 0, sizeof(struct bcmcnet_dev_stats));

    for (qi = 0; qi < ctrl->nb_rxq; qi++) {
        rxq = (struct pdma_rx_queue *)ctrl->rx_queue[qi];
        if (!rxq) {
            continue;
        }
        rxq->stats.packets = 0;
        rxq->stats.bytes = 0;
        rxq->stats.dropped = 0;
        rxq->stats.errors = 0;
        rxq->stats.head_errors = 0;
        rxq->stats.data_errors = 0;
        rxq->stats.cell_errors = 0;
        rxq->stats.nomems = 0;
    }

    for (qi = 0; qi < ctrl->nb_txq; qi++) {
        txq = (struct pdma_tx_queue *)ctrl->tx_queue[qi];
        if (!txq) {
            continue;
        }
        txq->stats.packets = 0;
        txq->stats.bytes = 0;
        txq->stats.dropped = 0;
        txq->stats.errors = 0;
        txq->stats.xoffs = 0;
    }
}

/*!
 * Convert logic queue to physical queue
 */
static int
bcmcnet_pdma_lq_to_pq(struct pdma_dev *dev, int queue, int dir, int *chan)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    struct pdma_tx_queue *txq = NULL;

    if (dir == PDMA_Q_RX) {
        rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
        if (rxq->state & PDMA_RX_QUEUE_USED) {
            *chan = rxq->chan_id;
            return SHR_E_NONE;
        }
    } else {
        txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
        if (txq->state & PDMA_TX_QUEUE_USED) {
            *chan = txq->chan_id;
            return SHR_E_NONE;
        }
    }

    return SHR_E_UNAVAIL;
}

/*!
 * Convert physical queue to logic queue
 */
static int
bcmcnet_pdma_pq_to_lq(struct pdma_dev *dev, int chan, int *queue, int *dir)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_rx_queue *rxq = NULL;
    struct pdma_tx_queue *txq = NULL;

    rxq = ctrl->grp[chan / dev->grp_queues].rx_queue[chan % dev->grp_queues];
    if (rxq->state & PDMA_RX_QUEUE_USED) {
        *queue = rxq->queue_id;
        *dir = PDMA_Q_RX;
        return SHR_E_NONE;
    }

    txq = ctrl->grp[chan / dev->grp_queues].tx_queue[chan % dev->grp_queues];
    if (txq->state & PDMA_TX_QUEUE_USED) {
        *queue = txq->queue_id;
        *dir = PDMA_Q_TX;
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}

/*!
 * Start Rx queue
 */
static int
bcmcnet_pdma_rx_queue_start(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    rxq->state |= PDMA_RX_QUEUE_ACTIVE;

    return hw->hdls.chan_start(hw, rxq->chan_id);
}

/*!
 * Stop Rx queue
 */
static int
bcmcnet_pdma_rx_queue_stop(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    rxq->state &= ~PDMA_RX_QUEUE_ACTIVE;

    return hw->hdls.chan_stop(hw, rxq->chan_id);
}

/*!
 * Start Tx queue
 */
static int
bcmcnet_pdma_tx_queue_start(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    txq->state |= PDMA_TX_QUEUE_ACTIVE;

    return dev->flags & PDMA_CHAIN_MODE ? SHR_E_NONE :
           hw->hdls.chan_start(hw, txq->chan_id);
}

/*!
 * Stop Tx queue
 */
static int
bcmcnet_pdma_tx_queue_stop(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    txq->state &= ~PDMA_TX_QUEUE_ACTIVE;

    return hw->hdls.chan_stop(hw, txq->chan_id);
}

/*!
 * Enable Rx queue interrupt
 */
static int
bcmcnet_pdma_rx_queue_intr_enable(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];

    return hw->hdls.chan_intr_enable(hw, rxq->chan_id);
}

/*!
 * Disable Rx queue interrupt
 */
static int
bcmcnet_pdma_rx_queue_intr_disable(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];

    return hw->hdls.chan_intr_disable(hw, rxq->chan_id);
}

/*!
 * Acknowledge Rx queue interrupt
 */
static int
bcmcnet_pdma_rx_queue_intr_ack(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];

    return hw->hdls.chan_clear(hw, rxq->chan_id);
}

/*!
 * Query Rx queue interrupt
 */
static int
bcmcnet_pdma_rx_queue_intr_query(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];

    return hw->hdls.chan_intr_query(hw, rxq->chan_id);
}

/*!
 * Check Rx queue interrupt
 */
static int
bcmcnet_pdma_rx_queue_intr_check(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];

    return hw->hdls.chan_intr_check(hw, rxq->chan_id);
}

/*!
 * Enable Tx queue interrupt
 */
static int
bcmcnet_pdma_tx_queue_intr_enable(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];

    if (txq->state & PDMA_TX_QUEUE_POLL) {
        return SHR_E_NONE;
    } else {
        return hw->hdls.chan_intr_enable(hw, txq->chan_id);
    }
}

/*!
 * Disable Tx queue interrupt
 */
static int
bcmcnet_pdma_tx_queue_intr_disable(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];

    return hw->hdls.chan_intr_disable(hw, txq->chan_id);
}

/*!
 * Acknowledge Tx queue interrupt
 */
static int
bcmcnet_pdma_tx_queue_intr_ack(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];

    return hw->hdls.chan_clear(hw, txq->chan_id);
}

/*!
 * Query Tx queue interrupt
 */
static int
bcmcnet_pdma_tx_queue_intr_query(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];

    return hw->hdls.chan_intr_query(hw, txq->chan_id);
}

/*!
 * Check Tx queue interrupt
 */
static int
bcmcnet_pdma_tx_queue_intr_check(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];

    return hw->hdls.chan_intr_check(hw, txq->chan_id);
}

/*!
 * Initialize HW
 */
static int
bcn_pdma_hw_init(struct pdma_dev *dev)
{
    struct pdma_hw *hw = (struct pdma_hw *)dev->ctrl.hw;

    hw->hdls.hw_reset(hw);
    hw->hdls.hw_init(hw);

    return SHR_E_NONE;
}

/*!
 * \brief Device operation functions.
 */
static const struct dev_ops pdma_dev_ops = {
    .dev_config                 = bcmcnet_pdma_config,
    .dev_close                  = bcmcnet_pdma_close,
    .dev_suspend                = bcmcnet_pdma_suspend,
    .dev_resume                 = bcmcnet_pdma_resume,
    .dev_info_get               = bcmcnet_pdma_info_get,
    .dev_stats_get              = bcmcnet_pdma_stats_get,
    .dev_stats_reset            = bcmcnet_pdma_stats_reset,
    .dev_lq_to_pq               = bcmcnet_pdma_lq_to_pq,
    .dev_pq_to_lq               = bcmcnet_pdma_pq_to_lq,
    .rx_queue_start             = bcmcnet_pdma_rx_queue_start,
    .rx_queue_stop              = bcmcnet_pdma_rx_queue_stop,
    .tx_queue_start             = bcmcnet_pdma_tx_queue_start,
    .tx_queue_stop              = bcmcnet_pdma_tx_queue_stop,
    .rx_queue_setup             = bcmcnet_pdma_rx_queue_setup,
    .rx_queue_release           = bcmcnet_pdma_rx_queue_release,
    .rx_queue_restore           = bcmcnet_pdma_rx_queue_restore,
    .tx_queue_setup             = bcmcnet_pdma_tx_queue_setup,
    .tx_queue_release           = bcmcnet_pdma_tx_queue_release,
    .tx_queue_restore           = bcmcnet_pdma_tx_queue_restore,
    .rx_queue_intr_enable       = bcmcnet_pdma_rx_queue_intr_enable,
    .rx_queue_intr_disable      = bcmcnet_pdma_rx_queue_intr_disable,
    .rx_queue_intr_ack          = bcmcnet_pdma_rx_queue_intr_ack,
    .rx_queue_intr_query        = bcmcnet_pdma_rx_queue_intr_query,
    .rx_queue_intr_check        = bcmcnet_pdma_rx_queue_intr_check,
    .tx_queue_intr_enable       = bcmcnet_pdma_tx_queue_intr_enable,
    .tx_queue_intr_disable      = bcmcnet_pdma_tx_queue_intr_disable,
    .tx_queue_intr_ack          = bcmcnet_pdma_tx_queue_intr_ack,
    .tx_queue_intr_query        = bcmcnet_pdma_tx_queue_intr_query,
    .tx_queue_intr_check        = bcmcnet_pdma_tx_queue_intr_check,
    .rx_queue_suspend           = bcmcnet_pdma_rx_queue_suspend,
    .rx_queue_resume            = bcmcnet_pdma_rx_queue_resume,
    .tx_queue_wakeup            = bcmcnet_pdma_tx_queue_wakeup,
    .rx_queue_poll              = bcmcnet_pdma_rx_queue_poll,
    .tx_queue_poll              = bcmcnet_pdma_tx_queue_poll,
    .group_poll                 = bcmcnet_pdma_group_poll,
};

/*!
 * Open a device
 */
int
bcmcnet_pdma_open(struct pdma_dev *dev)
{
    struct pdma_hw *hw = (struct pdma_hw *)dev->ctrl.hw;
    struct intr_handle *hdl = NULL;
    int start_num, num_offset;
    int chan, gi, qi;

    if (!hw) {
        return SHR_E_INIT;
    }

    dev->ops = (struct dev_ops *)&pdma_dev_ops;

    /* Initialize the hardware */
    bcn_pdma_hw_init(dev);

    dev->grp_queues = hw->info.cmc_chans;
    dev->num_queues = hw->info.num_chans;
    dev->ctrl.nb_desc = NUM_RING_DESC;
    dev->ctrl.budget = NUM_RXTX_BUDGET;
    dev->ctrl.rx_desc_size = hw->info.rx_dcb_size;
    dev->ctrl.tx_desc_size = hw->info.tx_dcb_size;

    /* Initialize interrupt handler */
    for (chan = 0; chan < dev->num_queues; chan++) {
        gi = chan / dev->grp_queues;
        qi = chan % dev->grp_queues;
        hdl = &dev->ctrl.grp[gi].intr_hdl[qi];
        hdl->unit = dev->unit;
        hdl->group = gi;
        hdl->chan = chan;
        hdl->dev = dev;
        hdl->mask_shift = 0;
        if (gi > 0 && hw->info.intr_mask_shift) {
            hdl->mask_shift = hw->info.intr_mask_shift + gi * 32;
        }
        start_num = hw->info.intr_start_num + hdl->mask_shift;
        num_offset = hw->info.intr_num_offset;
        hdl->intr_num = start_num + qi * num_offset;
    }

    /* Allocate all the queues */
    bcn_rx_queues_alloc(dev);
    bcn_tx_queues_alloc(dev);

    dev->pkt_xmit = bcmcnet_pdma_tx_queue_xmit;

    return SHR_E_NONE;
}

/*!
 * Coalesce Rx interrupt
 */
int
bcmcnet_pdma_rx_queue_int_coalesce(struct pdma_dev *dev, int queue, int count, int timer)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    if (queue < 0 || queue > (int)ctrl->nb_rxq) {
        return SHR_E_PARAM;
    }

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    rxq->intr_coalescing = 1;
    rxq->ic_val = (count & 0x7fff) << 16 | (timer & 0xffff);

    return hw->hdls.chan_intr_coalesce(hw, rxq->chan_id, count, timer);
}

/*!
 * Coalesce Tx interrupt
 */
int
bcmcnet_pdma_tx_queue_int_coalesce(struct pdma_dev *dev, int queue, int count, int timer)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    if (queue < 0 || queue > (int)ctrl->nb_txq) {
        return SHR_E_PARAM;
    }

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    txq->intr_coalescing = 1;
    txq->ic_val = (count & 0x7fff) << 16 | (timer & 0xffff);

    return hw->hdls.chan_intr_coalesce(hw, txq->chan_id, count, timer);
}

/*!
 * Dump Rx queue registers
 */
int
bcmcnet_pdma_rx_queue_reg_dump(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    if (queue < 0 || queue > (int)ctrl->nb_rxq) {
        return SHR_E_PARAM;
    }

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];

    return hw->hdls.chan_reg_dump(hw, rxq->chan_id);
}

/*!
 * Dump Tx queue registers
 */
int
bcmcnet_pdma_tx_queue_reg_dump(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    if (queue < 0 || queue > (int)ctrl->nb_txq) {
        return SHR_E_PARAM;
    }

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];

    return hw->hdls.chan_reg_dump(hw, txq->chan_id);
}

