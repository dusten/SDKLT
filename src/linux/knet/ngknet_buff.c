/*! \file ngknet_buff.c
 *
 * Utility routines for NGKNET packet buffer management in Linux kernel mode.
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
#include "ngknet_main.h"
#include "ngknet_buff.h"

/*!
 * Allocate coherent memory
 */
void *
bcmcnet_hal_alloc(struct pdma_dev *dev, uint32_t size, dma_addr_t *dma)
{
    struct ngknet_dev *bdev = (struct ngknet_dev *)dev->priv;

    return dma_alloc_coherent(bdev->dev, size, dma, GFP_KERNEL);
}

/*!
 * Free coherent memory
 */
void
bcmcnet_hal_free(struct pdma_dev *dev, uint32_t size, void *addr, dma_addr_t dma)
{
    struct ngknet_dev *bdev = (struct ngknet_dev *)dev->priv;

    dma_free_coherent(bdev->dev, size, addr, dma);
}

/*!
 * Allocate Rx buffer
 */
int
bcmcnet_rx_buff_alloc(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)rxq->ctrl->dev->priv;
    dma_addr_t dma;
    struct page *page;
    struct sk_buff *skb;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        page = kal_dev_alloc_page();
        if (unlikely(!page)) {
            return SHR_E_MEMORY;
        }
        dma = dma_map_page(dev->dev, page, 0, PAGE_SIZE, DMA_FROM_DEVICE);
        if (unlikely(dma_mapping_error(dev->dev, dma))) {
            __free_page(page);
            return SHR_E_MEMORY;
        }
        pbuf->dma = dma;
        pbuf->page = page;
        pbuf->page_offset = 0;
    } else {
        skb = netdev_alloc_skb(dev->net_dev, PDMA_RXB_RESV + pbuf->adj + rxq->buf_size);
        if (unlikely(!skb)) {
            return SHR_E_MEMORY;
        }
        skb_reserve(skb, PDMA_RXB_ALIGN - (((unsigned long)skb->data) & (PDMA_RXB_ALIGN - 1)));
        pbuf->skb = skb;
        pbuf->pkb = (struct pkt_buf *)skb->data;
        dma = dma_map_single(dev->dev, &pbuf->pkb->data + pbuf->adj, rxq->buf_size, DMA_FROM_DEVICE);
        if (unlikely(dma_mapping_error(dev->dev, dma))) {
            dev_kfree_skb_any(skb);
            return SHR_E_MEMORY;
        }
        pbuf->dma = dma;
    }

    return SHR_E_NONE;
}

/*!
 * Get Rx buffer DMA address
 */
void
bcmcnet_rx_buff_dma(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf, dma_addr_t *addr)
{
    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        *addr = pbuf->dma + pbuf->page_offset + PDMA_RXB_RESV + pbuf->adj;
    } else {
        *addr = pbuf->dma;
    }
}

/*!
 * Check Rx buffer
 */
int
bcmcnet_rx_buff_avail(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf)
{
    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        pbuf->skb = NULL;
    }

    return pbuf->dma != 0;
}

/*!
 * Get Rx buffer
 */
struct pkt_hdr *
bcmcnet_rx_buff_get(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf, int len)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)rxq->ctrl->dev->priv;
    struct sk_buff *skb;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        if (pbuf->skb) {
            return &pbuf->pkb->pkh;
        }
        skb = kal_build_skb(page_address(pbuf->page) + pbuf->page_offset,
                            PDMA_SKB_RESV + rxq->buf_size);
        if (unlikely(!skb)) {
            return NULL;
        }
        skb_reserve(skb, PDMA_RXB_ALIGN);
        dma_sync_single_range_for_cpu(dev->dev, pbuf->dma, pbuf->page_offset,
                                      PDMA_RXB_SIZE_MAX, DMA_FROM_DEVICE);
        pbuf->skb = skb;
        pbuf->pkb = (struct pkt_buf *)skb->data;

        /* Try to reuse this page */
        if (unlikely(page_count(pbuf->page) != 1)) {
            dma_unmap_page(dev->dev, pbuf->dma, PAGE_SIZE, DMA_FROM_DEVICE);
            pbuf->dma = 0;
        } else {
            pbuf->page_offset ^= PDMA_RXB_SIZE_MAX;
            atomic_inc(&pbuf->page->_count);
            dma_sync_single_range_for_device(dev->dev, pbuf->dma, pbuf->page_offset,
                                             PDMA_RXB_SIZE_MAX, DMA_FROM_DEVICE);
        }
    } else {
        if (!pbuf->dma) {
            return &pbuf->pkb->pkh;
        }
        skb = pbuf->skb;
        dma_unmap_single(dev->dev, pbuf->dma, rxq->buf_size, DMA_FROM_DEVICE);
        pbuf->dma = 0;
    }

    skb_put(skb, PKT_HDR_SIZE + pbuf->adj + len);

    return &pbuf->pkb->pkh;
}

/*!
 * Put Rx buffer
 */
int
bcmcnet_rx_buff_put(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf, int len)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)rxq->ctrl->dev->priv;
    dma_addr_t dma;
    struct sk_buff *skb;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        dev_kfree_skb_any(pbuf->skb);
    } else {
        skb = pbuf->skb;
        dma = dma_map_single(dev->dev, &pbuf->pkb->data + pbuf->adj,
                             rxq->buf_size, DMA_FROM_DEVICE);
        if (unlikely(dma_mapping_error(dev->dev, dma))) {
            dev_kfree_skb_any(skb);
            pbuf->dma = 0;
            return SHR_E_MEMORY;
        }
        pbuf->dma = dma;
        skb_trim(skb, skb->len - (PKT_HDR_SIZE + pbuf->adj + len));
    }

    return SHR_E_NONE;
}

/*!
 * Free Rx buffer
 */
void
bcmcnet_rx_buff_free(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)rxq->ctrl->dev->priv;

    if (rxq->mode == PDMA_BUF_MODE_PAGE) {
        dma_unmap_single(dev->dev, pbuf->dma, PAGE_SIZE, DMA_FROM_DEVICE);
        __free_page(pbuf->page);
    } else {
        dma_unmap_single(dev->dev, pbuf->dma, rxq->buf_size, DMA_FROM_DEVICE);
        dev_kfree_skb_any(pbuf->skb);
    }

    pbuf->dma = 0;
    pbuf->page = NULL;
    pbuf->page_offset = 0;
    pbuf->skb = NULL;
    pbuf->pkb = NULL;
    pbuf->adj = 0;
}

/*!
 * Get Rx buffer mode
 */
enum buf_mode
bcmcnet_rx_buff_mode(uint32_t size)
{
    return PDMA_RXB_SIZE(size) <= PDMA_RXB_SIZE_MAX && PAGE_SIZE < 8192 &&
           kal_support_paged_skb() ?
           PDMA_BUF_MODE_PAGE : PDMA_BUF_MODE_SKB;
}

/*!
 * Get Tx buffer
 */
struct pkt_hdr *
bcmcnet_tx_buff_get(struct pdma_tx_queue *txq, struct pdma_tx_buf *pbuf, void *buf)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)txq->ctrl->dev->priv;
    struct sk_buff *skb = (struct sk_buff *)buf;
    struct pkt_buf *pkb = (struct pkt_buf *)skb->data;
    dma_addr_t dma;

    pbuf->len = pkb->pkh.data_len + (pbuf->adj ? pkb->pkh.meta_len : 0);
    dma = dma_map_single(dev->dev, &pkb->data + (pbuf->adj ? 0 : pkb->pkh.meta_len),
                         pbuf->len, DMA_TO_DEVICE);
    if (unlikely(dma_mapping_error(dev->dev, dma))) {
        dev_kfree_skb_any(skb);
        return NULL;
    }
    pbuf->dma = dma;
    pbuf->skb = skb;
    pbuf->pkb = pkb;

    return &pkb->pkh;
}

/*!
 * Get Tx buffer DMA address
 */
void
bcmcnet_tx_buff_dma(struct pdma_tx_queue *txq, struct pdma_tx_buf *pbuf, dma_addr_t *addr)
{
    *addr = pbuf->dma;
}

/*!
 * Free Tx buffer
 */
void
bcmcnet_tx_buff_free(struct pdma_tx_queue *txq, struct pdma_tx_buf *pbuf)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)txq->ctrl->dev->priv;

    dma_unmap_single(dev->dev, pbuf->dma, pbuf->len, DMA_TO_DEVICE);
    dev_kfree_skb_any(pbuf->skb);

    pbuf->dma = 0;
    pbuf->len = 0;
    pbuf->skb = NULL;
    pbuf->pkb = NULL;
    pbuf->adj = 0;
}

