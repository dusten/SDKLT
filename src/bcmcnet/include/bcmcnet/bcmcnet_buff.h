/*! \file bcmcnet_buff.h
 *
 * Generic data structure definitions for BCMCNET packet buffer management.
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

#ifndef BCMCNET_BUFF_H
#define BCMCNET_BUFF_H

/*!
 * \brief Rx buffer.
 */
struct pdma_rx_buf {
    /*! DMA address */
    dma_addr_t dma;

    /*! Source buffer length */
    uint32_t len;

    /*! Source buffer address */
    void *skb;

    /*! Packet buffer point */
    struct pkt_buf *pkb;

    /*! Packet buffer adjustment */
    uint32_t adj;
};

/*!
 * \brief Tx buffer.
 */
struct pdma_tx_buf {
    /*! DMA address */
    dma_addr_t dma;

    /*! Source buffer length */
    uint32_t len;

    /*! Source buffer address */
    void *skb;

    /*! Packet buffer point */
    struct pkt_buf *pkb;

    /*! Packet buffer adjustment */
    uint32_t adj;
};

/*!
 * \brief Allocate coherent memory.
 *
 * \param [in] dev Device structure point.
 * \param [in] size Memory size.
 * \param [in] dma Memory dma address.
 *
 * \retval Memory virtual address.
 * \retval NULL Allocation failed.
 */
extern void *
bcmcnet_hal_alloc(struct pdma_dev *dev, uint32_t size, dma_addr_t *dma);

/*!
 * \brief Free coherent memory.
 *
 * \param [in] dev Device structure point.
 * \param [in] size Memory size.
 * \param [in] addr Memory virtual address.
 * \param [in] dma Memory dma address.
 */
extern void
bcmcnet_hal_free(struct pdma_dev *dev, uint32_t size, void *addr, dma_addr_t dma);

/*!
 * \brief Allocate Rx buffer.
 *
 * \param [in] rxq Rx queue structure point.
 * \param [in] pbuf Rx buffer structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_buff_alloc(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf);

/*!
 * \brief Get Rx buffer DMA address.
 *
 * \param [in] rxq Rx queue structure point.
 * \param [in] pbuf Rx buffer structure point.
 * \param [in] addr Rx buffer DMA address.
 */
extern void
bcmcnet_rx_buff_dma(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf, dma_addr_t *addr);

/*!
 * \brief Check Rx buffer validity.
 *
 * \param [in] rxq Rx queue structure point.
 * \param [in] pbuf Rx buffer structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_buff_avail(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf);

/*!
 * \brief Get Rx buffer.
 *
 * \param [in] rxq Rx queue structure point.
 * \param [in] pbuf Rx buffer structure point.
 * \param [in] len Rx buffer length.
 *
 * \retval Packet header point.
 * \retval NULL Operation failed.
 */
extern struct pkt_hdr *
bcmcnet_rx_buff_get(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf, int len);

/*!
 * \brief Put Rx buffer.
 *
 * \param [in] rxq Rx queue structure point.
 * \param [in] pbuf Rx buffer structure point.
 * \param [in] len Rx buffer length.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_buff_put(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf, int len);

/*!
 * \brief Free Rx buffer.
 *
 * \param [in] rxq Rx queue structure point.
 * \param [in] pbuf Rx buffer structure point.
 */
extern void
bcmcnet_rx_buff_free(struct pdma_rx_queue *rxq, struct pdma_rx_buf *pbuf);

/*!
 * \brief Get Rx buffer mode.
 *
 * \param [in] size Rx buffer size.
 *
 * \retval PDMA_BUF_MODE_PRIV in user mode.
 */
extern enum buf_mode
bcmcnet_rx_buff_mode(uint32_t size);

/*!
 * \brief Get Tx buffer.
 *
 * \param [in] txq Tx queue structure point.
 * \param [in] pbuf Tx buffer structure point.
 * \param [in] buf Raw buffer.
 */
extern struct pkt_hdr *
bcmcnet_tx_buff_get(struct pdma_tx_queue *txq, struct pdma_tx_buf *pbuf, void *buf);

/*!
 * \brief Get Tx buffer DMA address.
 *
 * \param [in] txq Tx queue structure point.
 * \param [in] pbuf Tx buffer structure point.
 * \param [in] addr Tx buffer DMA address.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern void
bcmcnet_tx_buff_dma(struct pdma_tx_queue *txq, struct pdma_tx_buf *pbuf, dma_addr_t *addr);

/*!
 * \brief Free Tx buffer.
 *
 * \param [in] txq Tx queue structure point.
 * \param [in] pbuf Tx buffer structure point.
 */
extern void
bcmcnet_tx_buff_free(struct pdma_tx_queue *txq, struct pdma_tx_buf *pbuf);

#endif /* BCMCNET_BUFF_H */

