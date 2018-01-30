/*! \file bcmcnet_rxtx.h
 *
 * Generic data structure and macro definitions for BCMCNET Rx/Tx.
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

#ifndef BCMCNET_RXTX_H
#define BCMCNET_RXTX_H

/*! Default descriptor number in each ring */
#define NUM_RING_DESC       64

/*! Maximum number of packets to be handled in one poll call */
#define NUM_RXTX_BUDGET     64

/*!
 * \brief Rx buffer mode definitions.
 */
enum buf_mode {
    /*! Private DMA buffer in user space */
    PDMA_BUF_MODE_PRIV,

    /*! SKB in kernel */
    PDMA_BUF_MODE_SKB,

    /*! Paged buffer in kernel */
    PDMA_BUF_MODE_PAGE,

    /*! Kernel buffer mapped to user space */
    PDMA_BUF_MODE_MAPPED,

    /*! MAX mode */
    PDMA_BUF_MODE_MAX
};

/*!
 * \brief Rx queue statistics.
 */
struct rx_stats {
    /*! Number of received packets */
    uint64_t packets;

    /*! Number of received bytes */
    uint64_t bytes;

    /*! Number of dropped packets */
    uint64_t dropped;

    /*! Number of errors */
    uint64_t errors;

    /*! Number of head errors */
    uint64_t head_errors;

    /*! Number of data errors */
    uint64_t data_errors;

    /*! Number of cell errors */
    uint64_t cell_errors;

    /*! Number of failed allocation */
    uint64_t nomems;
};

/*!
 * Rx queue structure
 */
struct pdma_rx_queue {
    /*! Group index to which this queue belongs */
    uint32_t group_id;

    /*! Global channel index */
    uint32_t chan_id;

    /*! Queue index */
    uint32_t queue_id;

    /*! Pointer to the device control structure */
    struct dev_ctrl *ctrl;

    /*! Rx packet buffer pointers */
    struct pdma_rx_buf *pbuf;

    /*! Rx ring address */
    void *ring;

    /*! Rx ring DMA address */
    dma_addr_t ring_addr;

    /*! Rx ring DMA halt address */
    dma_addr_t halt_addr;

    /*! Rx buffer size */
    uint32_t buf_size;

    /*! Total number of descriptors */
    uint32_t nb_desc;

    /*! Next free ring entry */
    uint32_t curr;

    /*! Halt ring entry */
    uint32_t halt;

    /*! Max free descriptors to hold */
    uint32_t free_thresh;

    /*! Rx interrupt coalesce value */
    uint32_t ic_val;

    /*! Rx interrupt coalescing */
    int intr_coalescing;

    /*! Queue statistics */
    struct rx_stats stats;

    /*! Rx queue spin lock */
    sal_spinlock_t lock;

    /*! Queue state */
    int state;
    /*! Queue is used */
#define PDMA_RX_QUEUE_USED      (1 << 0)
    /*! Queue is active */
#define PDMA_RX_QUEUE_ACTIVE    (1 << 1)
    /*! Queue is setup */
#define PDMA_RX_QUEUE_SETUP     (1 << 2)
    /*! Queue is suspended */
#define PDMA_RX_QUEUE_XOFF      (1 << 3)
    /*! Queue is batch refilled */
#define PDMA_RX_BATCH_REFILL    (1 << 4)

    /*! DMA buffer mode */
    enum buf_mode mode;
};

/*!
 * \brief Tx queue statistics.
 */
struct tx_stats {
    /*! Number of sent packets */
    uint64_t packets;

    /*! Number of sent bytes */
    uint64_t bytes;

    /*! Number of dropped packets */
    uint64_t dropped;

    /*! Number of errors */
    uint64_t errors;

    /*! Number of suspends */
    uint64_t xoffs;
};

/*!
 * \brief Tx queue structure.
 */
struct pdma_tx_queue {
    /*! Group index to which this queue belongs */
    uint32_t group_id;

    /*! Global channel index */
    uint32_t chan_id;

    /*! Queue index */
    uint32_t queue_id;

    /*! pointer to the device control structure */
    struct dev_ctrl *ctrl;

    /*! Tx packet buffer pointers */
    struct pdma_tx_buf *pbuf;

    /*! Tx ring address */
    void *ring;

    /*! Tx ring DMA address */
    dma_addr_t ring_addr;

    /*! Tx ring DMA halt address */
    dma_addr_t halt_addr;

    /*! Total number of descriptors */
    uint32_t nb_desc;

    /*! Next free ring entry */
    uint32_t curr;

    /*! First entry to be transmitted */
    uint32_t dirt;

    /*! Halt ring entry */
    uint32_t halt;

    /*! Max free descriptors to hold in non-intr mode */
    uint32_t free_thresh;

    /*! Tx interrupt coalesce value */
    uint32_t ic_val;

    /*! Tx interrupt coalescing */
    int intr_coalescing;

    /*! Queue statistics */
    struct tx_stats stats;

    /*! Tx queue spin lock */
    sal_spinlock_t lock;

    /*! Tx mutex spin lock */
    sal_spinlock_t mutex;

    /*! Tx mutex and flow control semaphore */
    sal_sem_t sem;

    /*! Queue state */
    int state;
    /*! Queue is used */
#define PDMA_TX_QUEUE_USED      (1 << 0)
    /*! Queue is active */
#define PDMA_TX_QUEUE_ACTIVE    (1 << 1)
    /*! Queue is setup */
#define PDMA_TX_QUEUE_SETUP     (1 << 2)
    /*! Queue is suspended */
#define PDMA_TX_QUEUE_XOFF      (1 << 3)
    /*! Queue is poll mode */
#define PDMA_TX_QUEUE_POLL      (1 << 4)

    /*! DMA buffer mode */
    enum buf_mode mode;
};

/*!
 * \brief Setup Rx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_rx_queue_setup(struct pdma_dev *dev, int queue);

/*!
 * \brief Release Rx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_rx_queue_release(struct pdma_dev *dev, int queue);

/*!
 * \brief Restore Rx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_rx_queue_restore(struct pdma_dev *dev, int queue);

/*!
 * \brief Setup Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_setup(struct pdma_dev *dev, int queue);

/*!
 * \brief Release Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_release(struct pdma_dev *dev, int queue);

/*!
 * \brief Restore Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_restore(struct pdma_dev *dev, int queue);

/*!
 * \brief Suspend Rx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_rx_queue_suspend(struct pdma_dev *dev, int queue);

/*!
 * \brief Resume Rx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_rx_queue_resume(struct pdma_dev *dev, int queue);

/*!
 * \brief Suspend Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_suspend(struct pdma_dev *dev, int queue);

/*!
 * \brief Resume Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_resume(struct pdma_dev *dev, int queue);

/*!
 * \brief Wakeup Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_wakeup(struct pdma_dev *dev, int queue);

/*!
 * \brief Start Tx queue transmission.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 * \param [in] buf Tx packet buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_xmit(struct pdma_dev *dev, int queue, void *buf);

/*!
 * \brief Poll Rx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 * \param [in] budget Poll budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_rx_queue_poll(struct pdma_dev *dev, int queue, int budget);

/*!
 * \brief Poll Tx queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 * \param [in] budget Poll budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_tx_queue_poll(struct pdma_dev *dev, int queue, int budget);

/*!
 * \brief Poll queue group.
 *
 * \param [in] dev Device structure point.
 * \param [in] group Group number.
 * \param [in] budget Poll budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_group_poll(struct pdma_dev *dev, int group, int budget);

/*!
 * \brief Dump Rx ring.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern void
bcmcnet_pdma_rx_ring_dump(struct pdma_dev *dev, int queue);

/*!
 * \brief Dump Tx ring.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern void
bcmcnet_pdma_tx_ring_dump(struct pdma_dev *dev, int queue);

#endif /* BCMCNET_RXTX_H */

