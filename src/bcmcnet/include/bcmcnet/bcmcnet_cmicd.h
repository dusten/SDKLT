/*! \file bcmcnet_cmicd.h
 *
 * CMICd registers and descriptors definitions.
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

#ifndef BCMCNET_CMICD_H
#define BCMCNET_CMICD_H

/*!
 * \name CMICD PDMA HW definitions
 */
/*! \{ */
/*! CMICD CMC number */
#define CMICD_PDMA_CMC_MAX              3
/*! CMICD CMC PDMA channels */
#define CMICD_PDMA_CMC_CHAN             4
/*! CMICD PDMA DCB size */
#define CMICD_PDMA_DCB_SIZE             64
/*! \} */

/*!
 * \name CMICD PDMA register definitions
 */
/*! \{ */
#define CMICD_PDMA_CTRLr                0x0140
#define CMICD_PDMA_STATr                0x0150
#define CMICD_PDMA_STAT_HIr             0x0130
#define CMICD_PDMA_STAT_CLRr            0x01a4
#define CMICD_PDMA_DESCr                0x0158
#define CMICD_PDMA_CURR_DESCr           0x01a8
#define CMICD_PDMA_DESC_HALTr           0x0120
#define CMICD_PDMA_COS_RX0r             0x0168
#define CMICD_PDMA_COS_RX1r             0x016c
#define CMICD_PDMA_COS_MASK0r           0x019c
#define CMICD_PDMA_COS_MASK1r           0x01a0
#define CMICD_PDMA_INTR_COALr           0x0188
#define CMICD_PDMA_RBUF_THREr           0x0110
#define CMICD_PDMA_COUNT_RXr            0x0480
#define CMICD_PDMA_COUNT_TXr            0x0484
#define CMICD_PDMA_COUNT_ALL_RXr        0x04a0
#define CMICD_PDMA_COUNT_ALL_TXr        0x04a4
#define CMICD_IRQ_STATr                 0x0400
#define CMICD_IRQ_PCI_MASKr             0x0414
#define CMICD_IRQ_UC0_MASKr             0x0428
/*! \} */

/*!
 * \name CMICD PDMA register address
 */
/*! \{ */
/*! Base address */
#define CMICD_GRP_BASE(g)               (0x00031000 + 0x1000 * g)
/*! Control register address */
#define CMICD_PDMA_CTRL(g, q)           (CMICD_GRP_BASE(g) + CMICD_PDMA_CTRLr + q * 4)
/*! Status register address */
#define CMICD_PDMA_STAT(g)              (CMICD_GRP_BASE(g) + CMICD_PDMA_STATr)
/*! Status higher register address */
#define CMICD_PDMA_STAT_HI(g)           (CMICD_GRP_BASE(g) + CMICD_PDMA_STAT_HIr)
/*! Status clear register address */
#define CMICD_PDMA_STAT_CLR(g)          (CMICD_GRP_BASE(g) + CMICD_PDMA_STAT_CLRr)
/*! Descriptor register address */
#define CMICD_PDMA_DESC(g, q)           (CMICD_GRP_BASE(g) + CMICD_PDMA_DESCr + q * 4)
/*! Current descriptor register address */
#define CMICD_PDMA_CURR_DESC(g, q)      (CMICD_GRP_BASE(g) + CMICD_PDMA_CURR_DESCr + q * 4)
/*! Descriptor halt register address */
#define CMICD_PDMA_DESC_HALT(g, q)      (CMICD_GRP_BASE(g) + CMICD_PDMA_DESC_HALTr + q * 4)
/*! COS Rx0 register address */
#define CMICD_PDMA_COS_RX0(g, q)        (CMICD_GRP_BASE(g) + CMICD_PDMA_COS_RX0r + q * 8)
/*! COS Rx1 register address */
#define CMICD_PDMA_COS_RX1(g, q)        (CMICD_GRP_BASE(g) + CMICD_PDMA_COS_RX1r + q * 8)
/*! COS Mask0 register address */
#define CMICD_PDMA_COS_MASK0(g)         (CMICD_GRP_BASE(g) + CMICD_PDMA_COS_MASK0r)
/*! COS Mask1 register address */
#define CMICD_PDMA_COS_MASK1(g)         (CMICD_GRP_BASE(g) + CMICD_PDMA_COS_MASK1r)
/*! Interrupt coalesce register address */
#define CMICD_PDMA_INTR_COAL(g, q)      (CMICD_GRP_BASE(g) + CMICD_PDMA_INTR_COALr + q * 4)
/*! Rx buffer threshhold register address */
#define CMICD_PDMA_RBUF_THRE(g, q)      (CMICD_GRP_BASE(g) + CMICD_PDMA_RBUF_THREr + q * 4)
/*! Rx counter register address */
#define CMICD_PDMA_COUNT_RX(g, q)       (CMICD_GRP_BASE(g) + CMICD_PDMA_COUNT_RXr + q * 8)
/*! Tx counter register address */
#define CMICD_PDMA_COUNT_TX(g, q)       (CMICD_GRP_BASE(g) + CMICD_PDMA_COUNT_TXr + q * 8)
/*! Rx global counter register address */
#define CMICD_PDMA_COUNT_ALL_RX(g)      (CMICD_GRP_BASE(g) + CMICD_PDMA_COUNT_ALL_RXr)
/*! Tx gloable counter register address */
#define CMICD_PDMA_COUNT_ALL_TX(g)      (CMICD_GRP_BASE(g) + CMICD_PDMA_COUNT_ALL_TXr)
/*! Interrupt status register address */
#define CMICD_IRQ_STAT(g)               (CMICD_GRP_BASE(g) + CMICD_IRQ_STATr)
/*! Interrupt PCI mask register address */
#define CMICD_IRQ_PCI_MASK(g)           (CMICD_GRP_BASE(g) + CMICD_IRQ_PCI_MASKr)
/*! Interrupt UC0 mask register address */
#define CMICD_IRQ_UC0_MASK(g)           (CMICD_GRP_BASE(g) + CMICD_IRQ_UC0_MASKr)
/*! Rx buffer release register address */
#define CMICD_RXBUF_EPINTF_RELEASE      0x0001a000
/*! Device revision register address */
#define CMICD_DEV_REV_ID                0x00010224
/*! CMIC revison register address */
#define CMICD_CMICM_REV_ID              0x00010228
/*! \} */

/*!
 * \name Control register definitions
 */
/*! \{ */
/*! Continuous DMA mode */
#define CMICD_PDMA_CONTINUOUS           0x00000200
/*! Controlled interrupt */
#define CMICD_PDMA_CNTLD_INTR           0x00000100
/*! Update status on reload */
#define CMICD_PDMA_RLD_STAT_DIS         0x00000080
/*! Dropped on chain end */
#define CMICD_PDMA_DROP_ON_END          0x00000040
/*! Descriptor big endianess */
#define CMICD_PDMA_DESC_BIG_ENDIAN      0x00000020
/*! Packet DMA big endianess */
#define CMICD_PDMA_PKT_BIG_ENDIAN       0x00000010
/*! Interrupt after descriptor */
#define CMICD_PDMA_INTR_ON_DESC         0x00000008
/*! Abort DMA */
#define CMICD_PDMA_ABORT                0x00000004
/*! Enable DMA */
#define CMICD_PDMA_ENABLE               0x00000002
/*! DMA direction */
#define CMICD_PDMA_DIR                  0x00000001
/*! \} */

/*!
 * \name Status register definitions
 */
/*! \{ */
/*! Chain done */
#define CMICD_PDMA_CHAIN_DONE(q)        (0x00000001 << (q))
/*! Descriptor done */
#define CMICD_PDMA_DESC_DONE(q)         (0x00000010 << (q))
/*! Active */
#define CMICD_PDMA_ACTIVE(q)            (0x00000100 << (q))
/*! \} */

/*!
 * \name Status clear register definitions
 */
/*! \{ */
/*! Clear completed interrupt */
#define CMICD_PDMA_DESC_CMPLT(q)        (0x00000001 << (q))
/*! Clear controlled interrupt */
#define CMICD_PDMA_DESC_CNTLD(q)        (0x00000100 << (q))
/*! \} */

/*!
 * \name Interrupt_coalesce register definitions
 */
/*! \{ */
/*! Interrupt coalesce enable */
#define CMICD_PDMA_INTR_COAL_ENA        (1 << 31)
/*! Interrupt coalesce threshhold */
#define CMICD_PDMA_INTR_THRESH(cnt)     (((cnt) & 0x7fff) << 16)
/*! Interrupt coalesce timeout */
#define CMICD_PDMA_INTR_TIMER(tmr)      ((tmr) & 0xffff)
/*! \} */

/*!
 * \name Interrupt status&mask register definitions
 */
/*! \{ */
/*! Interrupt mask */
#define CMICD_PDMA_IRQ_MASK             0x78000000
/*! Descriptor done */
#define CMICD_IRQ_DESC_DONE(q)          (0x00000100 << (2 * (q)))
/*! Chain done */
#define CMICD_IRQ_CHAIN_DONE(q)         (0x00000200 << (2 * (q)))
/*! Controlled interrupt */
#define CMICD_IRQ_DESC_CNTLD(q)         (0x08000000 << (q))
/*! Interrupt start number */
#define CMICD_IRQ_START_NUM             27
/*! Interrupt number offset */
#define CMICD_IRQ_NUM_OFFSET            1
/*! Interrupt mask shift */
#define CMICD_IRQ_MASK_SHIFT            0
/*! Interrupt mask zeroing */
#define CMICD_IRQ_ACT_CHAN(mask)        (((mask) & CMICD_PDMA_IRQ_MASK) >> CMICD_IRQ_START_NUM)
/*! \} */

/*! 32-bit register read */
#define DEV_READ32(_c, _a, _p) \
    do { \
        *(_p) = ((volatile uint32_t *)(_c)->hw_addr)[(_a) / 4]; \
    } while (0)

/*! 32-bit register write */
#define DEV_WRITE32(_c, _a, _v) \
    do { \
        ((volatile uint32_t *)(_c)->hw_addr)[(_a) / 4] = (_v); \
    } while (0)

/*!
 * Rx metadata in descriptor
 */
struct rx_metadata {
    /*! Metadata */
    volatile uint32_t data[13];

    /*! Status */
    volatile uint32_t status;
} __attribute__((packed));

/*!
 * Rx descriptor
 */
struct cmicd_rx_desc {
    /*! Packet address */
    volatile uint32_t addr;

    /*! Packet control */
    volatile uint32_t ctrl;

    /*! Metadata fields */
    struct rx_metadata md;
} __attribute__((packed));

/*! Reserve Rx meta data size in packet buffer */
#define CMICD_RX_META_RESV              64

/*!
 * Tx metadata in descriptor
 */
struct tx_metadata {
    /*! Metadata */
    volatile uint32_t data[4];

    /*! Reserved */
    volatile uint32_t rsvd[9];

    /*! Status */
    volatile uint32_t status;
} __attribute__((packed));

/*!
 * Tx descriptor
 */
struct cmicd_tx_desc {
    /*! Packet address */
    volatile uint32_t addr;

    /*! Packet control */
    volatile uint32_t ctrl;

    /*! Metadata fields */
    struct tx_metadata md;
} __attribute__((packed));

/*! Reserve Tx meta data size in packet buffer */
#define CMICD_TX_META_RESV              16

/*!
 * Flags related to descriptors.
 */
/*! Controlled interrupt */
#define CMICD_DESC_CTRL_CNTLD_INTR      (1 << 24)
/*! Completed interrupt */
#define CMICD_DESC_CTRL_CMPLT_INTR      (1 << 23)
/*! Reload DCB */
#define CMICD_DESC_CTRL_RELOAD          (1 << 18)
/*! Scatter DCB */
#define CMICD_DESC_CTRL_SCATTER         (1 << 17)
/*! Chained DCB */
#define CMICD_DESC_CTRL_CHAIN           (1 << 16)
/*! Control flags */
#define CMICD_DESC_CTRL_FLAGS(f)        (((f) & 0xffff) << 16)
/*! Purge packet */
#define CMICD_DESC_TX_PURGE_PKT         (1 << 6)
/*! Pause packet */
#define CMICD_DESC_TX_PAUSE_PKT         (1 << 5)
/*! Higig packet */
#define CMICD_DESC_TX_HIGIG_PKT         (1 << 3)
/*! Packet length */
#define CMICD_DESC_CTRL_LEN(len)        ((len) & 0xffff)
/*! Done */
#define CMICD_DESC_STAT_RTX_DONE        (1 << 31)
/*! Head error */
#define CMICD_DESC_STAT_HEAD_ERR        (1 << 20)
/*! Data error */
#define CMICD_DESC_STAT_DATA_ERR        (1 << 19)
/*! Cell error */
#define CMICD_DESC_STAT_CELL_ERR        (1 << 18)
/*! Error mask */
#define CMICD_DESC_STAT_ERR_MASK        (CMICD_DESC_STAT_HEAD_ERR | \
                                         CMICD_DESC_STAT_DATA_ERR | \
                                         CMICD_DESC_STAT_CELL_ERR)
/*! Packet start */
#define CMICD_DESC_STAT_PKT_START       (1 << 17)
/*! Packet end */
#define CMICD_DESC_STAT_PKT_END         (1 << 16)
/*! Get done state */
#define CMICD_DESC_STAT_DONE(stat)      ((stat) & CMICD_DESC_STAT_RTX_DONE)
/*! Get flags */
#define CMICD_DESC_STAT_FLAGS(stat)     (((stat) >> 16) & ~0x8003)
/*! Get packet length */
#define CMICD_DESC_STAT_LEN(stat)       ((stat) & 0xffff)

/*! HW access retry times */
#define CMICD_HW_RETRY_TIMES            100000

/*!
 * \brief Initialize HW handles.
 *
 * \param [in] hw HW structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicd_pdma_hw_hdls_init(struct pdma_hw *hw);

/*!
 * \brief Initialize descriptor operations.
 *
 * \param [in] hw HW structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicd_pdma_desc_ops_init(struct pdma_hw *hw);

/*!
 * \brief Attach device driver.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicd_pdma_driver_attach(struct pdma_dev *dev);

/*!
 * \brief Detach device driver.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicd_pdma_driver_detach(struct pdma_dev *dev);

#endif /* BCMCNET_CMICD_H */

