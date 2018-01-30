/*! \file bcmcnet_cmicx.h
 *
 * CMICx registers and descriptors definitions.
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

#ifndef BCMCNET_CMICX_H
#define BCMCNET_CMICX_H

/*!
 * \name CMICX PDMA HW definitions
 */
/*! \{ */
/*! CMICX CMC number */
#define CMICX_PDMA_CMC_MAX              2
/*! CMICX CMC PDMA channels */
#define CMICX_PDMA_CMC_CHAN             8
/*! CMICX PDMA DCB size */
#define CMICX_PDMA_DCB_SIZE             16
/*! \} */

/*!
 * \name CMICX PCIe device address definitions
 */
/*! \{ */
/*! CMICX PCIE offset */
#define CMICX_PCIE_SO_OFFSET            0x10000000
/*! Higher DMA address to bus address */
#define DMA_TO_BUS_HI(dma)              ((dma) | CMICX_PCIE_SO_OFFSET)
/*! Higher bus address to DMA address */
#define BUS_TO_DMA_HI(bus)              ((bus) & ~CMICX_PCIE_SO_OFFSET)
/*! \} */

/*!
 * \name CMICX PDMA register definitions
 */
/*! \{ */
#define CMICX_PDMA_CTRLr                0x2100
#define CMICX_PDMA_STATr                0x2114
#define CMICX_PDMA_DESC_LOr             0x2104
#define CMICX_PDMA_DESC_HIr             0x2108
#define CMICX_PDMA_CURR_DESC_LOr        0x2124
#define CMICX_PDMA_CURR_DESC_HIr        0x2128
#define CMICX_PDMA_DESC_HALT_LOr        0x210c
#define CMICX_PDMA_DESC_HALT_HIr        0x2110
#define CMICX_PDMA_COS_CTRL_RX0r        0x2118
#define CMICX_PDMA_COS_CTRL_RX1r        0x211c
#define CMICX_PDMA_INTR_COALr           0x2120
#define CMICX_PDMA_RBUF_THREr           0x212c
#define CMICX_PDMA_DEBUG_CTRLr          0x2130
#define CMICX_PDMA_DEBUG_SM_STATr       0x2134
#define CMICX_PDMA_DEBUG_STATr          0x2138
#define CMICX_PDMA_COUNT_RXr            0x213c
#define CMICX_PDMA_COUNT_TXr            0x2140
#define CMICX_PDMA_COUNT_RX_DROPr       0x2144
#define CMICX_PDMA_DESC_CNT_REQr        0x2148
#define CMICX_PDMA_DESC_CNT_RXr         0x214c
#define CMICX_PDMA_DESC_CNT_STATr       0x2150
#define CMICX_PDMA_IRQ_STATr            0x106c
#define CMICX_PDMA_IRQ_STAT_CLRr        0x1074
/*! \} */

/*!
 * \name CMICX PDMA register address
 */
/*! \{ */
/*! Base address */
#define CMICX_GRP_BASE(g)               (0x00000000 + 0x3000 * g)
/*! Control register address */
#define CMICX_PDMA_CTRL(g, q)           (CMICX_GRP_BASE(g) + CMICX_PDMA_CTRLr + q * 0x80)
/*! Status register address */
#define CMICX_PDMA_STAT(g, q)           (CMICX_GRP_BASE(g) + CMICX_PDMA_STATr + q * 0x80)
/*! Descriptor Address Lower register address */
#define CMICX_PDMA_DESC_LO(g, q)        (CMICX_GRP_BASE(g) + CMICX_PDMA_DESC_LOr + q * 0x80)
/*! Descriptor Address Higher register address */
#define CMICX_PDMA_DESC_HI(g, q)        (CMICX_GRP_BASE(g) + CMICX_PDMA_DESC_HIr + q * 0x80)
/*! Current Descriptor Address Lower register address */
#define CMICX_PDMA_CURR_DESC_LO(g, q)   (CMICX_GRP_BASE(g) + CMICX_PDMA_CURR_DESC_LOr + q * 0x80)
/*! Current Descriptor Address Higher register address */
#define CMICX_PDMA_CURR_DESC_HI(g, q)   (CMICX_GRP_BASE(g) + CMICX_PDMA_CURR_DESC_HIr + q * 0x80)
/*! Descriptor Halt Address Lower register address */
#define CMICX_PDMA_DESC_HALT_LO(g, q)   (CMICX_GRP_BASE(g) + CMICX_PDMA_DESC_HALT_LOr + q * 0x80)
/*! Descriptor Halt Address Higher register address */
#define CMICX_PDMA_DESC_HALT_HI(g, q)   (CMICX_GRP_BASE(g) + CMICX_PDMA_DESC_HALT_HIr + q * 0x80)
/*! COS Control Rx0 register address */
#define CMICX_PDMA_COS_CTRL_RX0(g, q)   (CMICX_GRP_BASE(g) + CMICX_PDMA_COS_CTRL_RX0r + q * 0x80)
/*! COS Control Rx1 register address */
#define CMICX_PDMA_COS_CTRL_RX1(g, q)   (CMICX_GRP_BASE(g) + CMICX_PDMA_COS_CTRL_RX1r + q * 0x80)
/*! Interrupt Coalesce register address */
#define CMICX_PDMA_INTR_COAL(g, q)      (CMICX_GRP_BASE(g) + CMICX_PDMA_INTR_COALr + q * 0x80)
/*! Rx Buffer Threshhold register address */
#define CMICX_PDMA_RBUF_THRE(g, q)      (CMICX_GRP_BASE(g) + CMICX_PDMA_RBUF_THREr + q * 0x80)
/*! Debug Control register address */
#define CMICX_PDMA_DEBUG_CTRL(g, q)     (CMICX_GRP_BASE(g) + CMICX_PDMA_DEBUG_CTRLr + q * 0x80)
/*! Debug Status register address */
#define CMICX_PDMA_DEBUG_STAT(g, q)     (CMICX_GRP_BASE(g) + CMICX_PDMA_DEBUG_STATr + q * 0x80)
/*! Debug State Machine Status register address */
#define CMICX_PDMA_DEBUG_SM_STAT(g, q)  (CMICX_GRP_BASE(g) + CMICX_PDMA_DEBUG_SM_STATr + q * 0x80)
/*! Rx Packet Count register address */
#define CMICX_PDMA_COUNT_RX(g, q)       (CMICX_GRP_BASE(g) + CMICX_PDMA_COUNT_RXr + q * 0x80)
/*! Tx Packet Count register address */
#define CMICX_PDMA_COUNT_TX(g, q)       (CMICX_GRP_BASE(g) + CMICX_PDMA_COUNT_TXr + q * 0x80)
/*! Dropped Rx Packet Count register address */
#define CMICX_PDMA_COUNT_RX_DROP(g, q)  (CMICX_GRP_BASE(g) + CMICX_PDMA_COUNT_RX_DROPr + q * 0x80)
/*! Requested Descriptor Count register address */
#define CMICX_PDMA_DESC_CNT_REQ(g, q)   (CMICX_GRP_BASE(g) + CMICX_PDMA_DESC_CNT_REQr + q * 0x80)
/*! Received Descriptor Count register address */
#define CMICX_PDMA_DESC_CNT_RX(g, q)    (CMICX_GRP_BASE(g) + CMICX_PDMA_DESC_CNT_RXr + q * 0x80)
/*! Updated Descriptor Count register address */
#define CMICX_PDMA_DESC_CNT_STAT(g, q)  (CMICX_GRP_BASE(g) + CMICX_PDMA_DESC_CNT_STATr + q * 0x80)
/*! Interrupt Status register address */
#define CMICX_PDMA_IRQ_STAT(g)          (CMICX_GRP_BASE(g) + CMICX_PDMA_IRQ_STATr)
/*! Interrupt Status Clear register address */
#define CMICX_PDMA_IRQ_STAT_CLR(g)      (CMICX_GRP_BASE(g) + CMICX_PDMA_IRQ_STAT_CLRr)
/*! Interrupt Enable register address0 */
#define CMICX_PDMA_IRQ_ENAB0            0x18013100
/*! Interrupt Enable register address1 */
#define CMICX_PDMA_IRQ_ENAB1            0x18013104
/*! Interrupt Enable register address2 */
#define CMICX_PDMA_IRQ_ENAB2            0x18013108
/*! Interrupt raw status register address0 */
#define CMICX_PDMA_IRQ_RAW_STAT0        0x18013150
/*! Interrupt raw status register address1 */
#define CMICX_PDMA_IRQ_RAW_STAT1        0x18013154
/*! Interrupt raw status register address2 */
#define CMICX_PDMA_IRQ_RAW_STAT2        0x18013158
/*! EP_TO_CPU Header Size register address */
#define CMICX_EP_TO_CPU_HEADER_SIZE     0x00000004
/*! Rx buffer release register address */
#define CMICX_RXBUF_EPINTF_RELEASE      0x0000006c
/*! \} */

/*!
 * \name Control register definitions
 */
/*! \{ */
/*! Disable abort on error */
#define CMICX_PDMA_NO_ABORT_ON_ERR      0x00002000
/*! EP_TO_CPU header big endianess */
#define CMICX_PDMA_HDR_BIG_ENDIAN       0x00001000
/*! Continuous descriptor mode */
#define CMICX_PDMA_CONTINUOUS_DESC      0x00000200
/*! Continuous DMA mode */
#define CMICX_PDMA_CONTINUOUS           0x00000100
/*! Interrupt after descriptor */
#define CMICX_PDMA_INTR_ON_DESC         0x00000080
/*! Update status on reload */
#define CMICX_PDMA_RLD_STAT_DIS         0x00000040
/*! Dropped on chain end */
#define CMICX_PDMA_DROP_ON_END          0x00000020
/*! Descriptor big endianess */
#define CMICX_PDMA_DESC_BIG_ENDIAN      0x00000010
/*! Packet DMA big endianess */
#define CMICX_PDMA_PKT_BIG_ENDIAN       0x00000008
/*! Abort DMA */
#define CMICX_PDMA_ABORT                0x00000004
/*! Enable DMA */
#define CMICX_PDMA_ENABLE               0x00000002
/*! DMA direction */
#define CMICX_PDMA_DIR                  0x00000001
/*! EP_TO_CPU header alignment bytes */
#define CMICX_PDMA_HDR_ALMNT(bytes)     (((bytes) & 0x3) << 10)
/*! \} */

/*!
 * \name Status register definitions
 */
/*! \{ */
/*! Channel in halt */
#define CMICX_PDMA_IN_HALT              0x00000040
/*! Channel active */
#define CMICX_PDMA_IS_ACTIVE            0x00000002
/*! Chain done */
#define CMICX_PDMA_CHAIN_DONE           0x00000001
/*! \} */

/*!
 * \name Interrupt_coalesce register definitions
 */
/*! \{ */
/*! Interrupt coalesce enable */
#define CMICX_PDMA_INTR_COAL_ENA        (1 << 31)
/*! Interrupt coalesce threshhold */
#define CMICX_PDMA_INTR_THRESH(cnt)     (((cnt) & 0x7fff) << 16)
/*! Interrupt coalesce timeout */
#define CMICX_PDMA_INTR_TIMER(tmr)      ((tmr) & 0xffff)
/*! \} */

/*!
 * \name Interrupt status&clear register definitions
 */
/*! \{ */
/*! Descriptor done */
#define CMICX_PDMA_IRQ_DESC_DONE(q)     (0x00000001 << ((q) * 4))
/*! Chain done */
#define CMICX_PDMA_IRQ_CHAIN_DONE(q)    (0x00000002 << ((q) * 4))
/*! Coalescing interrupt */
#define CMICX_PDMA_IRQ_COALESCE_INTR(q) (0x00000004 << ((q) * 4))
/*! Controlled interrupt */
#define CMICX_PDMA_IRQ_CTRLD_INTR(q)    (0x00000008 << ((q) * 4))
/*! Interrupt mask */
#define CMICX_PDMA_IRQ_MASK(q)          (0xf << ((q) * 4))
/*! Interrupt start number */
#define CMICX_IRQ_START_NUM             (128 + 3)
/*! Interrupt number offset */
#define CMICX_IRQ_NUM_OFFSET            4
/*! Interrupt mask shift */
#define CMICX_IRQ_MASK_SHIFT            16
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
 * Rx descriptor
 */
struct cmicx_rx_desc {
    /*! Packet address lower */
    volatile uint32_t addr_lo;

    /*! Packet address higher */
    volatile uint32_t addr_hi;

    /*! Packet control */
    volatile uint32_t ctrl;

    /*! Packet status */
    volatile uint32_t status;
} __attribute__((packed));

/*!
 * Tx descriptor
 */
struct cmicx_tx_desc {
    /*! Packet address lower */
    volatile uint32_t addr_lo;

    /*! Packet address higher */
    volatile uint32_t addr_hi;

    /*! Packet control */
    volatile uint32_t ctrl;

    /*! Packet status */
    volatile uint32_t status;
} __attribute__((packed));

/*!
 * Flags related to descriptors.
 */
/*! Disable descriptor status write */
#define CMICX_DESC_CTRL_STAT_WR_DIS     (1 << 29)
/*! Descriptors remaining */
#define CMICX_DESC_CTRL_REMAIN(cnt)     (((cnt) & 0xf) << 25)
/*! Controlled interrupt */
#define CMICX_DESC_CTRL_CNTLD_INTR      (1 << 24)
/*! Completed interrupt */
#define CMICX_DESC_CTRL_CMPLT_INTR      (1 << 23)
/*! Reload DCB */
#define CMICX_DESC_CTRL_RELOAD          (1 << 18)
/*! Scatter DCB */
#define CMICX_DESC_CTRL_SCATTER         (1 << 17)
/*! Chained DCB */
#define CMICX_DESC_CTRL_CHAIN           (1 << 16)
/*! Control flags */
#define CMICX_DESC_CTRL_FLAGS(f)        (((f) & 0xffff) << 16)
/*! Purge packet */
#define CMICX_DESC_TX_PURGE_PKT         (1 << 6)
/*! Higig packet */
#define CMICX_DESC_TX_HIGIG_PKT         (1 << 3)
/*! Packet length */
#define CMICX_DESC_CTRL_LEN(len)        ((len) & 0xffff)
/*! Done */
#define CMICX_DESC_STAT_RTX_DONE        (1 << 31)
/*! Ecc error */
#define CMICX_DESC_STAT_DATA_ERR        (1 << 19)
/*! Cell error */
#define CMICX_DESC_STAT_CELL_ERR        (1 << 18)
/*! Error mask */
#define CMICX_DESC_STAT_ERR_MASK        (CMICX_DESC_STAT_DATA_ERR | \
                                         CMICX_DESC_STAT_CELL_ERR)
/*! Packet start */
#define CMICX_DESC_STAT_PKT_START       (1 << 17)
/*! Packet end */
#define CMICX_DESC_STAT_PKT_END         (1 << 16)
/*! Get done state */
#define CMICX_DESC_STAT_DONE(stat)      ((stat) & CMICX_DESC_STAT_RTX_DONE)
/*! Get flags */
#define CMICX_DESC_STAT_FLAGS(stat)     (((stat) >> 16) & ~0x8003)
/*! Get packet length */
#define CMICX_DESC_STAT_LEN(stat)       ((stat) & 0xffff)

/*! Tx packet header size */
#define CMICX_TX_PKT_HDR_SIZE           16

/*! HW access retry times */
#define CMICX_HW_RETRY_TIMES            100000

/*!
 * \brief Initialize HW handles.
 *
 * \param [in] hw HW structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicx_pdma_hw_hdls_init(struct pdma_hw *hw);

/*!
 * \brief Initialize descriptor operations.
 *
 * \param [in] hw HW structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicx_pdma_desc_ops_init(struct pdma_hw *hw);

/*!
 * \brief Attach device driver.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicx_pdma_driver_attach(struct pdma_dev *dev);

/*!
 * \brief Detach device driver.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_cmicx_pdma_driver_detach(struct pdma_dev *dev);

#endif /* BCMCNET_CMICX_H */

