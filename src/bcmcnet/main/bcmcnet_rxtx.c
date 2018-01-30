/*! \file bcmcnet_rxtx.c
 *
 * Utility routines for BCMCNET Rx/Tx.
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
 * Free a Rx ring
 */
static void
bcn_rx_ring_free(struct pdma_rx_queue *rxq)
{
    struct dev_ctrl *ctrl = rxq->ctrl;

    if (rxq->lock) {
        sal_spinlock_destroy(rxq->lock);
        rxq->lock = NULL;
    }

    if (rxq->ring) {
        bcmcnet_hal_free(ctrl->dev, ctrl->rx_desc_size * (rxq->nb_desc + 1),
                         rxq->ring, rxq->ring_addr);
        rxq->ring = NULL;
    }

    if (rxq->pbuf) {
        sal_free(rxq->pbuf);
        rxq->pbuf = NULL;
    }
}

/*!
 * Allocate a Rx ring
 */
static int
bcn_rx_ring_alloc(struct pdma_rx_queue *rxq)
{
    struct dev_ctrl *ctrl = rxq->ctrl;

    /* Setup pktbuf ring */
    rxq->pbuf = sal_alloc(sizeof(*rxq->pbuf) * rxq->nb_desc, "rx_pbuf_p");
    if (!rxq->pbuf) {
        goto cleanup;
    }
    sal_memset(rxq->pbuf, 0, sizeof(*rxq->pbuf) * rxq->nb_desc);

    /* Allocate memory for descriptors */
    rxq->ring = bcmcnet_hal_alloc(ctrl->dev, ctrl->rx_desc_size * (rxq->nb_desc + 1),
                                  &rxq->ring_addr);
    if (!rxq->ring) {
        goto cleanup;
    }
    sal_memset(rxq->ring, 0, ctrl->rx_desc_size * (rxq->nb_desc + 1));

    rxq->lock = sal_spinlock_create("rx_queue_lock");
    if (!rxq->lock) {
        goto cleanup;
    }

    return SHR_E_NONE;

cleanup:
    bcn_rx_ring_free(rxq);

    return SHR_E_MEMORY;
}

/*!
 * Free a Tx ring
 */
static void
bcn_tx_ring_free(struct pdma_tx_queue *txq)
{
    struct dev_ctrl *ctrl = txq->ctrl;

    if (txq->sem) {
        sal_sem_destroy(txq->sem);
        txq->sem = NULL;
    }

    if (txq->mutex) {
        sal_spinlock_destroy(txq->mutex);
        txq->mutex = NULL;
    }

    if (txq->lock) {
        sal_spinlock_destroy(txq->lock);
        txq->lock = NULL;
    }

    if (txq->ring) {
        bcmcnet_hal_free(ctrl->dev, ctrl->tx_desc_size * (txq->nb_desc + 1),
                         txq->ring, txq->ring_addr);
        txq->ring = NULL;
    }

    if (txq->pbuf) {
        sal_free(txq->pbuf);
        txq->pbuf = NULL;
    }
}

/*!
 * Allocate a Tx ring
 */
static int
bcn_tx_ring_alloc(struct pdma_tx_queue *txq)
{
    struct dev_ctrl *ctrl = txq->ctrl;

    /* Setup pktbuf ring */
    txq->pbuf = sal_alloc(sizeof(*txq->pbuf) * txq->nb_desc, "tx_pbuf_p");
    if (!txq->pbuf) {
        goto cleanup;
    }
    sal_memset(txq->pbuf, 0, sizeof(*txq->pbuf) * txq->nb_desc);

    /* Allocate memory for descriptors */
    txq->ring = bcmcnet_hal_alloc(ctrl->dev, ctrl->tx_desc_size * (txq->nb_desc + 1),
                                  &txq->ring_addr);
    if (!txq->ring) {
        goto cleanup;
    }
    sal_memset(txq->ring, 0, ctrl->tx_desc_size * (txq->nb_desc + 1));

    txq->lock = sal_spinlock_create("tx_queue_lock");
    if (!txq->lock) {
        goto cleanup;
    }

    txq->mutex = sal_spinlock_create("tx_mutex_lock");
    if (!txq->mutex) {
        goto cleanup;
    }

    txq->sem = sal_sem_create("tx_mutex_sem", SAL_SEM_BINARY, 0);
    if (!txq->sem) {
        goto cleanup;
    }

    return SHR_E_NONE;

cleanup:
    bcn_tx_ring_free(txq);

    return SHR_E_MEMORY;
}

/*!
 * Rx polling
 */
static int
bcn_rx_poll(struct pdma_rx_queue *rxq, int budget)
{
    struct dev_ctrl *ctrl = rxq->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;

    return hw->dops.rx_ring_clean(hw, rxq, budget);
}

/*!
 * Tx polling
 */
static int
bcn_tx_poll(struct pdma_tx_queue *txq, int budget)
{
    struct dev_ctrl *ctrl = txq->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;

    return hw->dops.tx_ring_clean(hw, txq, budget);
}

/*!
 * Setup a Rx queue
 */
int
bcmcnet_pdma_rx_queue_setup(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;
    int rv;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    if (rxq->state & PDMA_RX_QUEUE_SETUP) {
        return SHR_E_NONE;
    }

    rv = bcn_rx_ring_alloc(rxq);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = hw->dops.rx_desc_init(hw, rxq);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rxq->state |= PDMA_RX_QUEUE_SETUP;

    return SHR_E_NONE;
}

/*!
 * Release a Rx queue
 */
int
bcmcnet_pdma_rx_queue_release(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    if (rxq->state & PDMA_RX_QUEUE_SETUP) {
        hw->dops.rx_desc_clean(hw, rxq);
        bcn_rx_ring_free(rxq);
        rxq->state &= ~PDMA_RX_QUEUE_SETUP;
    }

    return SHR_E_NONE;
}

/*!
 * Restore a Rx queue
 */
int
bcmcnet_pdma_rx_queue_restore(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    if (rxq->state & PDMA_RX_QUEUE_SETUP) {
        hw->dops.rx_desc_init(hw, rxq);
    }

    return SHR_E_NONE;
}

/*!
 * Setup a Tx queue
 */
int
bcmcnet_pdma_tx_queue_setup(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;
    int rv;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (txq->state & PDMA_TX_QUEUE_SETUP) {
        return SHR_E_NONE;
    }

    rv = bcn_tx_ring_alloc(txq);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = hw->dops.tx_desc_init(hw, txq);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    txq->state |= PDMA_TX_QUEUE_SETUP;

    return SHR_E_NONE;
}

/*!
 * Release a Tx queue
 */
int
bcmcnet_pdma_tx_queue_release(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (txq->state & PDMA_TX_QUEUE_SETUP) {
        hw->dops.tx_desc_clean(hw, txq);
        bcn_tx_ring_free(txq);
        txq->state &= ~PDMA_TX_QUEUE_SETUP;
    }

    return SHR_E_NONE;
}

/*!
 * Restore a Tx queue
 */
int
bcmcnet_pdma_tx_queue_restore(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (txq->state & PDMA_TX_QUEUE_SETUP) {
        hw->dops.tx_desc_init(hw, txq);
    }

    return SHR_E_NONE;
}

/*!
 * Suspend a Rx queue
 */
int
bcmcnet_pdma_rx_queue_suspend(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    if (!rxq || !(rxq->state & PDMA_RX_QUEUE_ACTIVE)) {
        return SHR_E_UNAVAIL;
    }

    return hw->dops.rx_suspend(hw, rxq);
}

/*!
 * Resume a Rx queue
 */
int
bcmcnet_pdma_rx_queue_resume(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    if (!rxq || !(rxq->state & PDMA_RX_QUEUE_ACTIVE)) {
        return SHR_E_UNAVAIL;
    }

    return hw->dops.rx_resume(hw, rxq);
}

/*!
 * Suspend a Tx queue
 */
int
bcmcnet_pdma_tx_queue_suspend(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (!txq || !(txq->state & PDMA_TX_QUEUE_ACTIVE)) {
        return SHR_E_UNAVAIL;
    }

    if (txq->sem) {
        sal_sem_take(txq->sem, SAL_SEM_FOREVER);
    }
    if (dev->tx_suspend) {
        dev->tx_suspend(dev, txq->queue_id);
    }

    return SHR_E_NONE;
}

/*!
 * Resume a Tx queue
 */
int
bcmcnet_pdma_tx_queue_resume(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (!txq || !(txq->state & PDMA_TX_QUEUE_ACTIVE)) {
        return SHR_E_UNAVAIL;
    }

    if (txq->sem) {
        sal_sem_give(txq->sem);
    }
    if (dev->tx_resume) {
        dev->tx_resume(dev, txq->queue_id);
    }

    return SHR_E_NONE;
}

/*!
 * Wake up a Tx queue
 */
int
bcmcnet_pdma_tx_queue_wakeup(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (txq->sem) {
        sal_sem_give(txq->sem);
    }

    return SHR_E_NONE;
}

/*!
 * Transmit a outputing packet
 */
int
bcmcnet_pdma_tx_queue_xmit(struct pdma_dev *dev, int queue, void *buf)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (!txq || !(txq->state & PDMA_TX_QUEUE_ACTIVE)) {
        return SHR_E_UNAVAIL;
    }

    return hw->dops.pkt_xmit(hw, txq, buf);
}

/*!
 * Poll a Rx queues
 */
int
bcmcnet_pdma_rx_queue_poll(struct pdma_dev *dev, int queue, int budget)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = ctrl->rx_queue[queue];

    /* Acknowledge the interrupt */
    hw->hdls.chan_clear(hw, rxq->chan_id);

    return bcn_rx_poll(rxq, budget);
}

/*!
 * Poll a Tx queues
 */
int
bcmcnet_pdma_tx_queue_poll(struct pdma_dev *dev, int queue, int budget)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_tx_queue *txq = ctrl->tx_queue[queue];

    /* Acknowledge the interrupt */
    hw->hdls.chan_clear(hw, txq->chan_id);

    return bcn_tx_poll(txq, budget);
}

/*!
 * Poll for Rx/Tx queues in a group
 */
int
bcmcnet_pdma_group_poll(struct pdma_dev *dev, int group, int budget)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = (struct pdma_hw *)ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;
    struct pdma_tx_queue *txq = NULL;
    struct queue_group *grp = &ctrl->grp[group];
    int done = 0, done_que = 0, act_rxqs = 0, budget_que = budget;
    int i;

    grp->all_done = 1;

    /* Acknowledge the interrupts */
    for (i = 0; i < dev->grp_queues; i++) {
        rxq = grp->rx_queue[i];
        if (rxq->state & PDMA_RX_QUEUE_ACTIVE) {
            if (hw->hdls.chan_intr_query(hw, rxq->chan_id)) {
                hw->hdls.chan_clear(hw, rxq->chan_id);
                act_rxqs++;
            }
            continue;
        }
        txq = grp->tx_queue[i];
        if (txq->state & PDMA_TX_QUEUE_ACTIVE) {
            if (hw->hdls.chan_intr_query(hw, txq->chan_id)) {
                hw->hdls.chan_clear(hw, txq->chan_id);
            }
        }
    }

    if (act_rxqs) {
        budget_que = budget / act_rxqs;
    }

    for (i = 0; i < dev->grp_queues; i++) {
        rxq = grp->rx_queue[i];
        if (1 << i & grp->bm_rxq) {
            done_que = bcn_rx_poll(rxq, budget_que);
            if (done_que >= budget_que) {
                grp->all_done = 0;
            }
            done += done_que;
        }
    }

    for (i = 0; i < dev->grp_queues; i++) {
        txq = grp->tx_queue[i];
        if (1 << i & grp->bm_txq && !txq->free_thresh) {
            if (bcn_tx_poll(txq, budget) >= budget) {
                grp->all_done = 0;
            }
        }
    }

    return grp->all_done && done < budget ? done : budget;
}

/*!
 * Dump a Rx ring
 */
void
bcmcnet_pdma_rx_ring_dump(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = ctrl->hw;
    struct pdma_rx_queue *rxq = NULL;

    rxq = (struct pdma_rx_queue *)ctrl->rx_queue[queue];
    if (rxq->state & PDMA_RX_QUEUE_ACTIVE) {
        hw->dops.rx_ring_dump(hw, rxq);
    }
}

/*!
 * Dump a Tx ring
 */
void
bcmcnet_pdma_tx_ring_dump(struct pdma_dev *dev, int queue)
{
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pdma_hw *hw = ctrl->hw;
    struct pdma_tx_queue *txq = NULL;

    txq = (struct pdma_tx_queue *)ctrl->tx_queue[queue];
    if (txq->state & PDMA_TX_QUEUE_ACTIVE) {
        hw->dops.tx_ring_dump(hw, txq);
    }
}

