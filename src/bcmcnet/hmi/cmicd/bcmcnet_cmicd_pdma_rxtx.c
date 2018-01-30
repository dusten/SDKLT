/*! \file bcmcnet_cmicd_pdma_rxtx.c
 *
 * Utility routines for BCMCNET hardware (CMICd) specific Rx/Tx.
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
#include <bcmcnet/bcmcnet_cmicd.h>

/*!
 * Configure Rx descriptor
 */
static inline void
cmicd_rx_desc_config(struct cmicd_rx_desc *rd, uint32_t addr, uint32_t len)
{
    rd->addr = addr;
    rd->md.status = 0;
    rd->ctrl = CMICD_DESC_CTRL_CNTLD_INTR | CMICD_DESC_CTRL_CHAIN |
               CMICD_DESC_CTRL_LEN(len);
}

/*!
 * Configure Tx descriptor
 */
static inline void
cmicd_tx_desc_config(struct cmicd_tx_desc *td, uint32_t addr, uint32_t len, uint32_t flags)
{
    td->addr = addr;
    td->md.status = 0;
    td->ctrl = CMICD_DESC_CTRL_CNTLD_INTR | CMICD_DESC_CTRL_CHAIN |
               CMICD_DESC_CTRL_FLAGS(flags) | CMICD_DESC_CTRL_LEN(len);
}

/*!
 * Configure Rx reload descriptor
 */
static inline void
cmicd_rx_rldesc_config(struct cmicd_rx_desc *rd, uint32_t addr)
{
    rd->addr = addr;
    rd->md.status = 0;
    rd->ctrl = CMICD_DESC_CTRL_CNTLD_INTR | CMICD_DESC_CTRL_CHAIN |
               CMICD_DESC_CTRL_RELOAD;
}

/*!
 * Configure Tx reload descriptor
 */
static inline void
cmicd_tx_rldesc_config(struct cmicd_tx_desc *td, uint32_t addr)
{
    td->addr = addr;
    td->md.status = 0;
    td->ctrl = CMICD_DESC_CTRL_CNTLD_INTR | CMICD_DESC_CTRL_CHAIN |
               CMICD_DESC_CTRL_RELOAD;
}

/*!
 * Chain Rx descriptor
 */
static inline void
cmicd_rx_desc_chain(struct cmicd_rx_desc *rd, int chain)
{
    if (chain) {
        rd->ctrl |= CMICD_DESC_CTRL_CHAIN;
    } else {
        rd->ctrl &= ~CMICD_DESC_CTRL_CHAIN;
    }
}

/*!
 * Chain Tx descriptor
 */
static inline void
cmicd_tx_desc_chain(struct cmicd_tx_desc *td, int chain)
{
    if (chain) {
        td->ctrl |= CMICD_DESC_CTRL_CHAIN;
    } else {
        td->ctrl &= ~CMICD_DESC_CTRL_CHAIN;
    }
}

/*!
 * Get unused descriptors in a Rx ring
 */
static inline int
cmicd_pdma_rx_ring_unused(struct pdma_rx_queue *rxq)
{
    /* Leave one descriptor unused so as not to halt */
    return rxq->curr > rxq->halt ?
           rxq->curr - rxq->halt - 1 :
           rxq->nb_desc + rxq->curr - rxq->halt - 1;
}

/*!
 * Get unused descriptors in a Tx ring
 */
static inline int
cmicd_pdma_tx_ring_unused(struct pdma_tx_queue *txq)
{
    /* Leave one descriptor unused so as not to halt */
    return txq->dirt > txq->curr ?
           txq->dirt - txq->curr - 1 :
           txq->nb_desc + txq->dirt - txq->curr - 1;
}

/*!
 * Initialize Rx descriptors
 */
static int
cmicd_pdma_rx_desc_init(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    struct cmicd_rx_desc *ring = (struct cmicd_rx_desc *)rxq->ring;
    dma_addr_t addr;
    uint32_t di;
    int rv;

    for (di = 0; di < rxq->nb_desc; di++) {
        if (!rxq->pbuf[di].dma) {
            /* Allocate pktbuf for ring entry */
            rxq->pbuf[di].adj = CMICD_RX_META_RESV;
            rv = bcmcnet_rx_buff_alloc(rxq, &rxq->pbuf[di]);
            if (SHR_FAILURE(rv)) {
                goto cleanup;
            }
        }
        /* Config receive descriptor ring */
        bcmcnet_rx_buff_dma(rxq, &rxq->pbuf[di], &addr);
        cmicd_rx_desc_config(&ring[di], addr, rxq->buf_size);
        if (hw->dev->flags & PDMA_CHAIN_MODE && di == rxq->nb_desc - 1) {
            cmicd_rx_desc_chain(&ring[di], 0);
        }
    }
    /* Config the last descriptor in the ring as reload descriptor */
    cmicd_rx_rldesc_config(&ring[di], rxq->ring_addr);

    rxq->curr = 0;
    rxq->halt = rxq->state & PDMA_RX_BATCH_REFILL ? 0 : rxq->nb_desc;

    rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicd_rx_desc) * di;
    hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
    hw->hdls.chan_setup(hw, rxq->chan_id, rxq->ring_addr);

    return SHR_E_NONE;

cleanup:
    for (di = 0; di < rxq->nb_desc; di++) {
        if (rxq->pbuf[di].dma) {
            bcmcnet_rx_buff_free(rxq, &rxq->pbuf[di]);
        }
        cmicd_rx_desc_config(&ring[di], 0, 0);
    }

    CNET_PR("RX: Failed to allocate mem\n");

    return SHR_E_MEMORY;
}

/*!
 * Cleanup Rx descriptors
 */
static int
cmicd_pdma_rx_desc_clean(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    struct cmicd_rx_desc *ring = (struct cmicd_rx_desc *)rxq->ring;
    uint32_t di;

    /* Go through all the descriptors and free pktbuf */
    for (di = 0; di < rxq->nb_desc; di++) {
        if (rxq->pbuf[di].dma) {
            bcmcnet_rx_buff_free(rxq, &rxq->pbuf[di]);
        }
        cmicd_rx_desc_config(&ring[di], 0, 0);
    }

    rxq->curr = 0;
    rxq->halt = 0;

    return SHR_E_NONE;
}

/*!
 * Initialize Tx descriptors
 */
static int
cmicd_pdma_tx_desc_init(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    struct cmicd_tx_desc *ring = (struct cmicd_tx_desc *)txq->ring;
    uint32_t di;

    for (di = 0; di < txq->nb_desc; di++) {
        if (txq->pbuf[di].dma) {
            bcmcnet_tx_buff_free(txq, &txq->pbuf[di]);
        }
        /* Config transmit descriptor ring */
        cmicd_tx_desc_config(&ring[di], 0, 0, 0);
        if (hw->dev->flags & PDMA_CHAIN_MODE) {
            cmicd_tx_desc_chain(&ring[di], 0);
        }
    }
    /* Config the last descriptor in the ring as reload descriptor */
    cmicd_tx_rldesc_config(&ring[di], txq->ring_addr);

    txq->curr = 0;
    txq->dirt = 0;
    txq->halt = 0;

    txq->halt_addr = txq->ring_addr;
    hw->hdls.chan_goto(hw, txq->chan_id, txq->halt_addr);
    hw->hdls.chan_setup(hw, txq->chan_id, txq->ring_addr);

    return SHR_E_NONE;
}

/*!
 * Cleanup Tx descriptors
 */
static int
cmicd_pdma_tx_desc_clean(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    struct cmicd_tx_desc *ring = (struct cmicd_tx_desc *)txq->ring;
    uint32_t di;

    /* Go through all the descriptors and free pktbuf */
    for (di = 0; di < txq->nb_desc; di++) {
        if (txq->pbuf[di].dma) {
            bcmcnet_tx_buff_free(txq, &txq->pbuf[di]);
        }
        cmicd_tx_desc_config(&ring[di], 0, 0, 0);
    }

    txq->curr = 0;
    txq->dirt = 0;
    txq->halt = 0;

    return SHR_E_NONE;
}

/*!
 * \brief Clean Rx ring
 *
 * \param [in] hw HW structure point.
 * \param [in] rxq Rx queue structure point.
 * \param [in] budget Polling budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
cmicd_pdma_rx_ring_clean(struct pdma_hw *hw, struct pdma_rx_queue *rxq, int budget)
{
    struct pdma_dev *dev = hw->dev;
    struct cmicd_rx_desc *ring = (struct cmicd_rx_desc *)rxq->ring;
    struct pdma_rx_buf *pbuf = NULL;
    struct pkt_hdr *pkh = NULL;
    dma_addr_t addr;
    uint32_t stat, curr, halt;
    int unused, len, done = 0;
    int rv;

    curr = rxq->curr;
    while (CMICD_DESC_STAT_DONE(ring[curr].md.status)) {
        if (!(rxq->state & PDMA_RX_BATCH_REFILL) &&
            !(rxq->state & PDMA_RX_QUEUE_XOFF)) {
            /* Descriptor cherry pick */
            rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicd_rx_desc) * curr;
            hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
            rxq->halt = curr;
        }
        if (budget-- <= 0) {
            break;
        }

        /* Get the current pktbuf to process */
        pbuf = &rxq->pbuf[curr];
        stat = ring[curr].md.status;
        len = CMICD_DESC_STAT_LEN(stat);
        pkh = bcmcnet_rx_buff_get(rxq, pbuf, len);
        if (!pkh) {
            rxq->curr = curr;
            rxq->stats.nomems++;
            goto fail;
        }

        /* Setup packet header */
        pkh->data_len = len;
        pkh->meta_len = pbuf->adj;
        pkh->queue_id = rxq->queue_id;
        pkh->attrs = CMICD_DESC_STAT_FLAGS(stat);
        sal_memcpy(&pbuf->pkb->data, &ring[curr].md, sizeof(struct rx_metadata));

        /* Send up the packet */
        rv = dev->pkt_recv(dev, rxq->queue_id, (void *)pbuf->skb);
        if (SHR_FAILURE(rv)) {
            rxq->stats.dropped++;
            bcmcnet_rx_buff_put(rxq, pbuf, len);
        }

        /* Count the packets/bytes */
        rxq->stats.packets++;
        rxq->stats.bytes += len;

        /* Count the errors if any */
        if (stat & CMICD_DESC_STAT_ERR_MASK) {
            rxq->stats.errors++;
            if (stat & CMICD_DESC_STAT_HEAD_ERR) {
                rxq->stats.head_errors++;
            }
            if (stat & CMICD_DESC_STAT_DATA_ERR) {
                rxq->stats.data_errors++;
            }
            if (stat & CMICD_DESC_STAT_CELL_ERR) {
                rxq->stats.cell_errors++;
            }
        }

        /* Setup the new descriptor */
        if (!(rxq->state & PDMA_RX_BATCH_REFILL)) {
            if (!bcmcnet_rx_buff_avail(rxq, pbuf)) {
                rv = bcmcnet_rx_buff_alloc(rxq, pbuf);
                if (SHR_FAILURE(rv)) {
                    rxq->curr = curr;
                    rxq->stats.nomems++;
                    goto fail;
                }
            }
            bcmcnet_rx_buff_dma(rxq, pbuf, &addr);
            cmicd_rx_desc_config(&ring[curr], addr, rxq->buf_size);
        } else {
            cmicd_rx_desc_config(&ring[curr], 0, 0);
        }

        /* Update the indicators */
        if (!(rxq->state & PDMA_RX_BATCH_REFILL) && rxq->halt != curr) {
            sal_spinlock_lock(rxq->lock);
            if (!(rxq->state & PDMA_RX_QUEUE_XOFF)) {
                /* Descriptor cherry pick */
                rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicd_rx_desc) * curr;
                hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
                rxq->halt = curr;
            }
            curr = (curr + 1) % rxq->nb_desc;
            sal_spinlock_unlock(rxq->lock);
        } else {
            curr = (curr + 1) % rxq->nb_desc;
        }
        done++;

        /* Restart DMA if in chain mode */
        if (dev->flags & PDMA_CHAIN_MODE && curr == 0 &&
            !(rxq->state & PDMA_RX_QUEUE_XOFF)) {
            hw->hdls.chan_stop(hw, rxq->chan_id);
            hw->hdls.chan_start(hw, rxq->chan_id);
        }
    }
    rxq->curr = curr;

    /* In batching mode, replenish all the unused descriptors */
    if ((rxq->state & PDMA_RX_BATCH_REFILL) &&
        (unused = cmicd_pdma_rx_ring_unused(rxq)) >= (int)rxq->free_thresh) {
        for (halt = rxq->halt; halt < rxq->halt + unused; halt++) {
            pbuf = &rxq->pbuf[halt % rxq->nb_desc];
            /* Allocate a new pktbuf */
            if (!bcmcnet_rx_buff_avail(rxq, pbuf)) {
                rv = bcmcnet_rx_buff_alloc(rxq, pbuf);
                if (SHR_FAILURE(rv)) {
                    rxq->halt = halt % rxq->nb_desc;
                    rxq->stats.nomems++;
                    goto fail;
                }
            }
            /* Setup the new descriptor */
            bcmcnet_rx_buff_dma(rxq, pbuf, &addr);
            cmicd_rx_desc_config(&ring[halt % rxq->nb_desc], addr, rxq->buf_size);
        }
        sal_spinlock_lock(rxq->lock);
        rxq->halt = halt % rxq->nb_desc;
        if (!(rxq->state & PDMA_RX_QUEUE_XOFF)) {
            /* Descriptor cherry pick */
            rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicd_rx_desc) * rxq->halt;
            hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
        }
        sal_spinlock_unlock(rxq->lock);
    }

    return done;

fail:
    CNET_PR("RX: Failed to allocate mem\n");

    return done;
}

/*!
 * \brief Clean Tx ring
 *
 * \param [in] hw HW structure point.
 * \param [in] txq Tx queue structure point.
 * \param [in] budget Polling budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
cmicd_pdma_tx_ring_clean(struct pdma_hw *hw, struct pdma_tx_queue *txq, int budget)
{
    struct pdma_dev *dev = hw->dev;
    struct cmicd_tx_desc *ring = (struct cmicd_tx_desc *)txq->ring;
    uint32_t dirt;
    int done = 0;

    dirt = txq->dirt;
    while (txq->pbuf[dirt].dma) {
        if (!CMICD_DESC_STAT_DONE(ring[dirt].md.status)) {
            break;
        }
        if (budget-- <= 0) {
            break;
        }

        /* Free the done pktbuf */
        bcmcnet_tx_buff_free(txq, &txq->pbuf[dirt]);
        cmicd_tx_desc_config(&ring[dirt], 0, 0, 0);

        /* Update the indicators */
        dirt = (dirt + 1) % txq->nb_desc;
        txq->dirt = dirt;
        done++;
    }

    /* Resume Tx if any */
    sal_spinlock_lock(txq->lock);
    if (cmicd_pdma_tx_ring_unused(txq) && txq->state & PDMA_TX_QUEUE_XOFF) {
        txq->state &= ~PDMA_TX_QUEUE_XOFF;
        sal_spinlock_unlock(txq->lock);
        if (dev->tx_resume) {
            dev->tx_resume(dev, txq->queue_id);
        } else {
            sal_sem_give(txq->sem);
        }
        return done;
    }
    sal_spinlock_unlock(txq->lock);

    return done;
}

/*!
 * Dump Rx ring
 */
static int
cmicd_pdma_rx_ring_dump(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    struct cmicd_rx_desc *ring = (struct cmicd_rx_desc *)rxq->ring;
    struct cmicd_rx_desc *rd;
    uint32_t di;

    CNET_PR("\nRX: queue=%d, chan=%d, curr=%d, halt=%d, halt@%p\n",
            rxq->queue_id, rxq->chan_id, rxq->curr, rxq->halt, (void *)&ring[rxq->halt]);
    CNET_PR("----------------------------------------------------------------\n");
    for (di = 0; di < rxq->nb_desc; di++) {
        rd = &ring[di];
        CNET_PR("DESC[%03d]: (%p)->%08x %08x ... %08x\n",
                di, (void *)rd, rd->addr, rd->ctrl, rd->md.status);
    }

    return SHR_E_NONE;
}

/*!
 * Dump Tx ring
 */
static int
cmicd_pdma_tx_ring_dump(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    struct cmicd_tx_desc *ring = (struct cmicd_tx_desc *)txq->ring;
    struct cmicd_tx_desc *td;
    uint32_t di;

    CNET_PR("\nTX: queue=%d, chan=%d, curr=%d, dirt=%d, halt@%p\n",
            txq->queue_id, txq->chan_id, txq->curr, txq->dirt, (void *)&ring[txq->curr]);
    CNET_PR("----------------------------------------------------------------\n");
    for (di = 0; di < txq->nb_desc; di++) {
        td = &ring[di];
        CNET_PR("DESC[%03d]: (%p)->%08x %08x ... %08x\n",
                di, (void *)td, td->addr, td->ctrl, td->md.status);
    }

    return SHR_E_NONE;
}

/*!
 * Check Tx ring
 */
static int
cmicd_pdma_tx_ring_check(struct pdma_hw *hw, struct pdma_tx_queue *txq)
{
    if (cmicd_pdma_tx_ring_unused(txq)) {
        return SHR_E_NONE;
    }

    sal_spinlock_lock(txq->lock);
    if (!cmicd_pdma_tx_ring_unused(txq)) {
        txq->state |= PDMA_TX_QUEUE_XOFF;
        txq->stats.xoffs++;
        sal_spinlock_unlock(txq->lock);
        if (hw->dev->tx_suspend) {
            hw->dev->tx_suspend(hw->dev, txq->queue_id);
        }
        return SHR_E_BUSY;
    }
    sal_spinlock_unlock(txq->lock);

    return SHR_E_NONE;
}

/*!
 * \brief Start packet transmission
 *
 * \param [in] hw HW structure point.
 * \param [in] txq Tx queue structure point.
 * \param [in] buf Tx packet buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
cmicd_pdma_pkt_xmit(struct pdma_hw *hw, struct pdma_tx_queue *txq, void *buf)
{
    struct pdma_dev *dev = hw->dev;
    struct cmicd_tx_desc *ring = (struct cmicd_tx_desc *)txq->ring;
    struct pdma_tx_buf *pbuf = NULL;
    struct pkt_hdr *pkh = NULL;
    dma_addr_t addr;
    uint32_t curr, flags = 0;
    int rv;

    if (!(txq->state & PDMA_TX_QUEUE_ACTIVE)) {
        return SHR_E_UNAVAIL;
    }

    if (dev->tx_suspend) {
        sal_spinlock_lock(txq->mutex);
    } else {
        sal_sem_take(txq->sem, SAL_SEM_FOREVER);
    }

    /* Suspend Tx if no resource */
    if (dev->tx_suspend) {
        rv = cmicd_pdma_tx_ring_check(hw, txq);
        if (SHR_FAILURE(rv)) {
            sal_spinlock_unlock(txq->mutex);
            return rv;
        }
    }

    /* Setup the new descriptor */
    curr = txq->curr;
    pbuf = &txq->pbuf[curr];
    pbuf->adj = 0;
    pkh = bcmcnet_tx_buff_get(txq, pbuf, buf);
    if (!pkh) {
        txq->stats.dropped++;
        if (dev->tx_suspend) {
            sal_spinlock_unlock(txq->mutex);
        } else {
            sal_sem_give(txq->sem);
        }
        return SHR_E_NONE;
    }
    bcmcnet_tx_buff_dma(txq, pbuf, &addr);
    flags |= pkh->attrs & PDMA_TX_HIGIG_PKT ? CMICD_DESC_TX_HIGIG_PKT : 0;
    flags |= pkh->attrs & PDMA_TX_PAUSE_PKT ? CMICD_DESC_TX_PAUSE_PKT : 0;
    flags |= pkh->attrs & PDMA_TX_PURGE_PKT ? CMICD_DESC_TX_PURGE_PKT : 0;
    cmicd_tx_desc_config(&ring[curr], addr, pbuf->len, flags);
    if (pkh->meta_len) {
        sal_memcpy(&ring[curr].md, &pbuf->pkb->data, sizeof(ring->md.data));
    }

    /* Start DMA if in chain mode */
    if (dev->flags & PDMA_CHAIN_MODE) {
        if (txq->state & PDMA_TX_QUEUE_POLL) {
            cmicd_pdma_tx_ring_clean(hw, txq, txq->nb_desc);
        }
        if (txq->dirt == txq->halt) {
            hw->hdls.chan_stop(hw, txq->chan_id);
            cmicd_tx_desc_chain(&ring[curr], 0);
            hw->hdls.chan_setup(hw, txq->chan_id,
                                txq->ring_addr + sizeof(struct cmicd_tx_desc) * txq->halt);
            hw->hdls.chan_start(hw, txq->chan_id);
            txq->halt = (curr + 1) % txq->nb_desc;
        }
    }

    /* Count the packets/bytes */
    txq->stats.packets++;
    txq->stats.bytes += pbuf->len;

    /* Update the indicators */
    curr = (curr + 1) % txq->nb_desc;
    txq->curr = curr;

    /* Kick off DMA */
    txq->halt_addr = txq->ring_addr + sizeof(struct cmicd_tx_desc) * curr;
    hw->hdls.chan_goto(hw, txq->chan_id, txq->halt_addr);

    if (txq->state & PDMA_TX_QUEUE_POLL &&
        cmicd_pdma_tx_ring_unused(txq) <= (int)txq->free_thresh) {
        cmicd_pdma_tx_ring_clean(hw, txq, txq->nb_desc - txq->free_thresh);
    }

    /* Suspend Tx if no resource */
    rv = cmicd_pdma_tx_ring_check(hw, txq);
    if (SHR_FAILURE(rv)) {
        /* In polling mode, must wait till the ring is available */
        if (txq->state & PDMA_TX_QUEUE_POLL) {
            do {
                cmicd_pdma_tx_ring_clean(hw, txq, txq->free_thresh);
            } while (txq->state & PDMA_TX_QUEUE_XOFF);
        }

        if (!dev->tx_suspend) {
            return SHR_E_NONE;
        }
    }

    if (dev->tx_suspend) {
        sal_spinlock_unlock(txq->mutex);
    } else {
        sal_sem_give(txq->sem);
    }

    return SHR_E_NONE;
}

/*!
 * Suspend Rx queue
 */
static int
cmicd_pdma_rx_suspend(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    sal_spinlock_lock(rxq->lock);
    rxq->state |= PDMA_RX_QUEUE_XOFF;
    if (hw->dev->flags & PDMA_CHAIN_MODE) {
        hw->hdls.chan_stop(hw, rxq->chan_id);
    }
    sal_spinlock_unlock(rxq->lock);

    return SHR_E_NONE;
}

/*!
 * Resume Rx queue
 */
static int
cmicd_pdma_rx_resume(struct pdma_hw *hw, struct pdma_rx_queue *rxq)
{
    sal_spinlock_lock(rxq->lock);
    if (!(rxq->state & PDMA_RX_QUEUE_XOFF)) {
        sal_spinlock_unlock(rxq->lock);
        return SHR_E_NONE;
    }
    if (rxq->state & PDMA_RX_BATCH_REFILL) {
        rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicd_rx_desc) * rxq->halt;
        hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
    } else if (rxq->halt == rxq->curr || (rxq->halt == rxq->nb_desc && rxq->curr == 0)) {
        rxq->halt = (rxq->curr + 1) % rxq->nb_desc;
        rxq->halt_addr = rxq->ring_addr + sizeof(struct cmicd_rx_desc) * rxq->halt;
        hw->hdls.chan_goto(hw, rxq->chan_id, rxq->halt_addr);
    }
    if (hw->dev->flags & PDMA_CHAIN_MODE) {
        rxq->curr = 0;
        hw->hdls.chan_start(hw, rxq->chan_id);
    }
    rxq->state &= ~PDMA_RX_QUEUE_XOFF;
    sal_spinlock_unlock(rxq->lock);

    return SHR_E_NONE;
}

/*!
 * Initialize function pointers
 */
int
bcmcnet_cmicd_pdma_desc_ops_init(struct pdma_hw *hw)
{
    if (!hw) {
        return SHR_E_PARAM;
    }

    hw->dops.rx_desc_init = cmicd_pdma_rx_desc_init;
    hw->dops.rx_desc_clean = cmicd_pdma_rx_desc_clean;
    hw->dops.rx_ring_clean = cmicd_pdma_rx_ring_clean;
    hw->dops.rx_ring_dump = cmicd_pdma_rx_ring_dump;
    hw->dops.rx_suspend = cmicd_pdma_rx_suspend;
    hw->dops.rx_resume = cmicd_pdma_rx_resume;
    hw->dops.tx_desc_init = cmicd_pdma_tx_desc_init;
    hw->dops.tx_desc_clean = cmicd_pdma_tx_desc_clean;
    hw->dops.tx_ring_clean = cmicd_pdma_tx_ring_clean;
    hw->dops.tx_ring_dump = cmicd_pdma_tx_ring_dump;
    hw->dops.pkt_xmit = cmicd_pdma_pkt_xmit;

    return SHR_E_NONE;
}

/*!
 * Attach device driver
 */
int
bcmcnet_cmicd_pdma_driver_attach(struct pdma_dev *dev)
{
    struct pdma_hw *hw = NULL;

    /* Allocate memory for HW data */
    hw = sal_alloc(sizeof(*hw), "pdma_hw");
    if (!hw) {
        return SHR_E_MEMORY;
    }
    sal_memset(hw, 0, sizeof(*hw));
    hw->unit = dev->unit;
    hw->dev = dev;
    dev->ctrl.hw = hw;

    bcmcnet_cmicd_pdma_hw_hdls_init(hw);
    bcmcnet_cmicd_pdma_desc_ops_init(hw);

    return SHR_E_NONE;
}

/*!
 * Detach device driver
 */
int
bcmcnet_cmicd_pdma_driver_detach(struct pdma_dev *dev)
{
    if (dev->ctrl.hw) {
        sal_free(dev->ctrl.hw);
    }
    dev->ctrl.hw = NULL;

    return SHR_E_NONE;
}

