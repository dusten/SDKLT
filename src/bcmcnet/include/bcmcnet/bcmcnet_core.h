/*! \file bcmcnet_core.h
 *
 * Generic data structure definitions and APIs for BCMCNET driver.
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

#ifndef BCMCNET_CORE_H
#define BCMCNET_CORE_H

#include <bcmcnet/bcmcnet_types.h>

/*!
 * \brief Packet header structure.
 */
struct pkt_hdr {
    /*! Meta data or outer header */
    uint8_t meta_data[16];

    /* Reserved */
    uint16_t rsvd0;

    /* Packet signature */
    uint16_t pkt_sig;

    /* Reserved */
    uint32_t rsvd1;

    /*! Data length */
    uint16_t data_len;

    /* Reserved */
    uint16_t rsvd2;

    /*! Meta length */
    uint8_t meta_len;

    /*! Queue index */
    uint8_t queue_id;

    /*! Attributes */
    uint16_t attrs;
    /*! Tx higig packet */
#define PDMA_TX_HIGIG_PKT   (1 << 0)
    /*! Tx pause packet */
#define PDMA_TX_PAUSE_PKT   (1 << 1)
    /*! Tx purge packet */
#define PDMA_TX_PURGE_PKT   (1 << 2)
    /*! Tx queue number */
#define PDMA_TX_BIND_QUE    (1 << 3)
    /*! Tx cookded header */
#define PDMA_TX_HDR_COOKED  (1 << 4)
    /*! Rx strip vlan tag */
#define PDMA_RX_STRIP_TAG   (1 << 11)
    /*! Rx set protocol type */
#define PDMA_RX_SET_PROTO   (1 << 12)
    /*! Rx IP checksum */
#define PDMA_RX_IP_CSUM     (1 << 13)
    /*! Rx TCPUDP checksum */
#define PDMA_RX_TU_CSUM     (1 << 14)
};

/*! Packet header size */
#define PKT_HDR_SIZE        sizeof(struct pkt_hdr)

/*!
 * \brief Packet buffer structure.
 */
struct pkt_buf {
    /*! Packet header */
    struct pkt_hdr pkh;

    /*! Packet data */
    uint8_t data;
};

/*!
 * \brief Interrupt handle.
 */
struct intr_handle {
    /*! Device number */
    int unit;

    /*! Group number */
    int group;

    /*! Channel number */
    int chan;

    /*! Queue number */
    int queue;

    /*! Direction */
    int dir;

    /*! Polling budget */
    int budget;

    /*! Device point */
    void *dev;

    /*! Private point */
    void *priv;

    /*! Interrupt number */
    int intr_num;

    /*! Interrupt mask shift */
    int mask_shift;

    /*! Interrupt flags */
    uint32_t intr_flags;
};

/*!
 * \brief Queue group structure.
 */
struct queue_group {
    /*! Pointer to the device control structure */
    struct dev_ctrl *ctrl;

    /*! Interrupt handles */
    struct intr_handle intr_hdl[NUM_Q_PER_GRP];

    /*! Rx queue pointers */
    void *rx_queue[NUM_Q_PER_GRP];

    /*! Tx queue pointers */
    void *tx_queue[NUM_Q_PER_GRP];

    /*! Bitmap for Rx queues at work */
    uint32_t bm_rxq;

    /*! Bitmap for Tx queues at work */
    uint32_t bm_txq;

    /*! Number of Rx queues at work */
    uint32_t nb_rxq;

    /*! Number of Tx queues at work */
    uint32_t nb_txq;

    /*! Number of descriptors */
    uint32_t nb_desc[NUM_Q_PER_GRP];

    /*! Rx buffer size */
    uint32_t rx_size[NUM_Q_PER_GRP];

    /*! Queue mode */
    uint32_t que_ctrl[NUM_Q_PER_GRP];
    /*! Packet_byte_swap */
#define PDMA_PKT_BYTE_SWAP  (1 << 0)
    /*! Non packet_byte_swap */
#define PDMA_OTH_BYTE_SWAP  (1 << 1)
    /*! Header_byte_swap */
#define PDMA_HDR_BYTE_SWAP  (1 << 2)

    /*! Group ID */
    int id;

    /*! All work done */
    int all_done;

    /*! Active IRQs for DMA control */
    uint32_t irq_mask;

    /*! Indicating the group is attached */
    int attached;
};

/*!
 * \brief Device control structure.
 */
struct dev_ctrl {
    /*! Pointer to the device structure */
    struct pdma_dev *dev;

    /*! Pointer to hardware-specific data */
    void *hw;

    /*! HW base address */
    volatile void *hw_addr;

    /*! Queue groups */
    struct queue_group grp[NUM_GRP_MAX];

    /*! Pointers to Rx queues */
    void *rx_queue[NUM_QUE_MAX];

    /*! Pointers to Tx queues */
    void *tx_queue[NUM_QUE_MAX];

    /*! Bitmap of groups at work */
    uint32_t bm_grp;

    /*! Bitmap of Rx queues at work */
    uint32_t bm_rxq;

    /*! Bitmap of Tx queues at work */
    uint32_t bm_txq;

    /*! Number of groups at work */
    uint32_t nb_grp;

    /*! Number of Rx queues at work */
    uint32_t nb_rxq;

    /*! Number of Tx queues at work */
    uint32_t nb_txq;

    /*! Number of descriptors for a queue */
    uint32_t nb_desc;

    /*! Budget for once queue processing */
    uint32_t budget;

    /*! Common Rx buffer size for all queues */
    uint32_t rx_buf_size;

    /*! Rx descriptor size */
    uint32_t rx_desc_size;

    /*! Tx descriptor size */
    uint32_t tx_desc_size;
};

/*!
 * Definitions of all functions exported.
 */
/*! Configure device */
typedef int (*pdma_dev_config_t)(struct pdma_dev *dev, uint32_t bm_rxq, uint32_t bm_txq);
/*! Start device */
typedef int (*pdma_dev_start_t)(struct pdma_dev *dev);
/*! Stop device */
typedef int (*pdma_dev_stop_t)(struct pdma_dev *dev);
/*! Close device */
typedef int (*pdma_dev_close_t)(struct pdma_dev *dev);
/*! Suspend device */
typedef int (*pdma_dev_suspend_t)(struct pdma_dev *dev);
/*! Resume device */
typedef int (*pdma_dev_resume_t)(struct pdma_dev *dev);
/*! Get device information */
typedef void (*pdma_dev_info_get_t)(struct pdma_dev *dev);
/*! Get device statistics */
typedef void (*pdma_dev_stats_get_t)(struct pdma_dev *dev);
/*! Reset device statistics */
typedef void (*pdma_dev_stats_reset_t)(struct pdma_dev *dev);
/*! Logic queue to physical queue */
typedef int (*pdma_dev_lq2pq_t)(struct pdma_dev *dev, int queue, int dir, int *chan);
/*! Physical queue to logic queue */
typedef int (*pdma_dev_pq2lq_t)(struct pdma_dev *dev, int chan, int *queue, int *dir);
/*! Start queue */
typedef int (*pdma_queue_start_t)(struct pdma_dev *dev, int queue);
/*! Stop queue */
typedef int (*pdma_queue_stop_t)(struct pdma_dev *dev, int queue);
/*! Set up queue */
typedef int (*pdma_queue_setup_t)(struct pdma_dev *dev, int queue);
/*! Release queue */
typedef int (*pdma_queue_release_t)(struct pdma_dev *dev, int queue);
/*! Restore queue */
typedef int (*pdma_queue_restore_t)(struct pdma_dev *dev, int queue);
/*! Enable queue interrupt */
typedef int (*pdma_intr_enable_t)(struct pdma_dev *dev, int queue);
/*! Disable queue interrupt */
typedef int (*pdma_intr_disable_t)(struct pdma_dev *dev, int queue);
/*! Acknowledge queue interrupt */
typedef int (*pdma_intr_ack_t)(struct pdma_dev *dev, int queue);
/*! Query queue interrupt status */
typedef int (*pdma_intr_query_t)(struct pdma_dev *dev, int queue);
/*! Check queue interrupt validity */
typedef int (*pdma_intr_check_t)(struct pdma_dev *dev, int queue);
/*! Suspend Rx queue */
typedef int (*pdma_rx_queue_suspend_t)(struct pdma_dev *dev, int queue);
/*! Resume Rx queue */
typedef int (*pdma_rx_queue_resume_t)(struct pdma_dev *dev, int queue);
/*! Wake up Tx queue */
typedef int (*pdma_tx_queue_wakeup_t)(struct pdma_dev *dev, int queue);
/*! Poll Rx queue */
typedef int (*pdma_rx_queue_poll_t)(struct pdma_dev *dev, int queue, int budget);
/*! Poll Tx queue */
typedef int (*pdma_tx_queue_poll_t)(struct pdma_dev *dev, int queue, int budget);
/*! Poll group */
typedef int (*pdma_group_poll_t)(struct pdma_dev *dev, int group, int budget);

/*!
 * \brief Exported functions structure.
 */
struct dev_ops {
    /*! Configure device */
    pdma_dev_config_t           dev_config;

    /*! Start device */
    pdma_dev_start_t            dev_start;

    /*! Stop device */
    pdma_dev_stop_t             dev_stop;

    /*! Close device */
    pdma_dev_close_t            dev_close;

    /*! Suspend device */
    pdma_dev_suspend_t          dev_suspend;

    /*! Resume device */
    pdma_dev_resume_t           dev_resume;

    /*! Get device information */
    pdma_dev_info_get_t         dev_info_get;

    /*! Get device statistics */
    pdma_dev_stats_get_t        dev_stats_get;

    /*! Reset device statistics */
    pdma_dev_stats_reset_t      dev_stats_reset;

    /*! Logic queue to physical queue */
    pdma_dev_lq2pq_t            dev_lq_to_pq;

    /*! Physical queue to logic queue */
    pdma_dev_pq2lq_t            dev_pq_to_lq;

    /*! Start Rx for a queue */
    pdma_queue_start_t          rx_queue_start;

    /*! Stop Rx for a queue */
    pdma_queue_stop_t           rx_queue_stop;

    /*! Start Tx for a queue */
    pdma_queue_start_t          tx_queue_start;

    /*! Stop Tx for a queue */
    pdma_queue_stop_t           tx_queue_stop;

    /*! Set up Rx queue */
    pdma_queue_setup_t          rx_queue_setup;

    /*! Release Rx queue */
    pdma_queue_release_t        rx_queue_release;

    /*! Restore stopped Rx queue */
    pdma_queue_restore_t        rx_queue_restore;

    /*! Set up Tx queue */
    pdma_queue_setup_t          tx_queue_setup;

    /*! Release Tx queue */
    pdma_queue_release_t        tx_queue_release;

    /*! Restore stopped Tx queue */
    pdma_queue_restore_t        tx_queue_restore;

    /*! Enable Rx queue interrupt */
    pdma_intr_enable_t          rx_queue_intr_enable;

    /*! Disable Rx queue interrupt */
    pdma_intr_disable_t         rx_queue_intr_disable;

    /*! Acknowledge interrupt for Rx queue */
    pdma_intr_ack_t             rx_queue_intr_ack;

    /*! Query interrupt status for Rx queue */
    pdma_intr_query_t           rx_queue_intr_query;

    /*! Check interrupt validity for Rx queue */
    pdma_intr_check_t           rx_queue_intr_check;

    /*! Enable Tx queue interrupt */
    pdma_intr_enable_t          tx_queue_intr_enable;

    /*! Disable Tx queue interrupt */
    pdma_intr_disable_t         tx_queue_intr_disable;

    /*! Acknowledge interrupt for Tx queue */
    pdma_intr_ack_t             tx_queue_intr_ack;

    /*! Query interrupt status for Tx queue */
    pdma_intr_query_t           tx_queue_intr_query;

    /*! Check interrupt validity for Tx queue */
    pdma_intr_check_t           tx_queue_intr_check;

    /*! Suspend a Rx queue */
    pdma_rx_queue_suspend_t     rx_queue_suspend;

    /*! Resume a Rx queue */
    pdma_rx_queue_resume_t      rx_queue_resume;

    /*! Wake up a Tx queue to transmit */
    pdma_tx_queue_wakeup_t      tx_queue_wakeup;

    /*! Poll for a Rx queue */
    pdma_rx_queue_poll_t        rx_queue_poll;

    /*! Poll for a Tx queue */
    pdma_tx_queue_poll_t        tx_queue_poll;

    /*! Poll for a group */
    pdma_group_poll_t           group_poll;
};

/*! Read 32-bit device register */
typedef int (*reg32_read_t)(struct pdma_dev *dev, uint32_t addr, uint32_t *data);
/*! Write 32-bit device register */
typedef int (*reg32_write_t)(struct pdma_dev *dev, uint32_t addr, uint32_t data);
/*! Packet reception */
typedef int (*pdma_rx_t)(struct pdma_dev *dev, int queue, void *buf);
/*! Packet transmission */
typedef int (*pdma_tx_t)(struct pdma_dev *dev, int queue, void *buf);
/*! Tx suspend */
typedef void (*sys_tx_suspend_t)(struct pdma_dev *dev, int queue);
/*! Tx resume */
typedef void (*sys_tx_resume_t)(struct pdma_dev *dev, int queue);
/*! Enable a set of interrupts */
typedef void (*sys_intr_unmask_t)(struct pdma_dev *dev, int group, int chan,
                                  uint32_t reg, uint32_t mask);
/*! Disable a set of interrupts */
typedef void (*sys_intr_mask_t)(struct pdma_dev *dev, int group, int chan,
                                uint32_t reg, uint32_t mask);

/*!
 * \brief Device structure.
 */
struct pdma_dev {
    /*! Device name */
    char name[DEV_NAME_LEN_MAX];

    /*! Device ID */
    uint32_t dev_id;

    /*! Device type */
    uint32_t dev_type;

    /*! Device Number */
    int unit;

    /*! Device control structure */
    struct dev_ctrl ctrl;

    /*! Pointer to the exported funtions structure */
    struct dev_ops *ops;

    /*! Device information */
    struct bcmcnet_dev_info info;

    /*! Device statistics data */
    struct bcmcnet_dev_stats stats;

    /*! Private data */
    void *priv;

    /*! Read 32-bit device register */
    reg32_read_t dev_read32;

    /*! Write 32-bit device register */
    reg32_write_t dev_write32;

    /*! Packet reception */
    pdma_rx_t pkt_recv;

    /*! Packet transmission */
    pdma_tx_t pkt_xmit;

    /*! Tx suspend */
    sys_tx_suspend_t tx_suspend;

    /*! Tx resume */
    sys_tx_resume_t tx_resume;

    /*! Enable a set of interrupts */
    sys_intr_unmask_t intr_unmask;

    /*! Disable a set of interrupts */
    sys_intr_mask_t intr_mask;

    /*! Maximum number of groups */
    int num_groups;

    /*! Maximum number of group queues */
    int grp_queues;

    /*! Maximum number of queues */
    int num_queues;

    /*! Flags */
    uint32_t flags;
    /*! Interrupt processing per group */
#define PDMA_GROUP_INTR     (1 << 0)
    /*! Tx polling mode */
#define PDMA_TX_POLLING     (1 << 1)
    /*! Rx batch refilling */
#define PDMA_RX_BATCHING    (1 << 2)
    /*! DMA chain mode */
#define PDMA_CHAIN_MODE     (1 << 3)

    /*! Device mode */
    uint32_t mode;

    /*! Attaching indicator */
    int attached;
};

/*!
 * \brief Initialize device.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_init(struct pdma_dev *dev);

/*!
 * \brief Deinitialize device.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_deinit(struct pdma_dev *dev);

/*!
 * \brief Start device.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_start(struct pdma_dev *dev);

/*!
 * \brief Stop device.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_stop(struct pdma_dev *dev);

/*!
 * \brief Suspend device.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_suspend(struct pdma_dev *dev);

/*!
 * \brief Resume device.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_resume(struct pdma_dev *dev);

/*!
 * \brief Suspend device Rx.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_rx_suspend(struct pdma_dev *dev);

/*!
 * \brief Resume device Rx.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_rx_resume(struct pdma_dev *dev);

/*!
 * \brief Get device information.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_info_get(struct pdma_dev *dev);

/*!
 * \brief Get device statistics.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_stats_get(struct pdma_dev *dev);

/*!
 * \brief Reset device statistics.
 *
 * \param [in] dev Device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_stats_reset(struct pdma_dev *dev);

/*!
 * \brief Change queue number to channel number.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Queue number.
 * \param [in] dir Transmit direction.
 * \param [out] chan Channel number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_queue_to_chan(struct pdma_dev *dev, int queue, int dir, int *chan);

/*!
 * \brief Change channel number to queue number.
 *
 * \param [in] dev Device structure point.
 * \param [in] chan Channel number.
 * \param [out] queue Queue number.
 * \param [out] dir Transmit direction.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_pdma_dev_chan_to_queue(struct pdma_dev *dev, int chan, int *queue, int *dir);

/*!
 * \brief Enable Rx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_queue_intr_enable(struct pdma_dev *dev, int queue);

/*!
 * \brief Disable Rx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_queue_intr_disable(struct pdma_dev *dev, int queue);

/*!
 * \brief Acknowledge Rx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_queue_intr_ack(struct pdma_dev *dev, int queue);

/*!
 * \brief Check Rx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Rx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_queue_intr_check(struct pdma_dev *dev, int queue);

/*!
 * \brief Enable Tx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_tx_queue_intr_enable(struct pdma_dev *dev, int queue);

/*!
 * \brief Disable Tx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_tx_queue_intr_disable(struct pdma_dev *dev, int queue);

/*!
 * \brief Acknowledge Tx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_tx_queue_intr_ack(struct pdma_dev *dev, int queue);

/*!
 * \brief Check Tx queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] queue Tx queue number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_tx_queue_intr_check(struct pdma_dev *dev, int queue);

/*!
 * \brief Enable queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] hdl Queue interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_queue_intr_enable(struct pdma_dev *dev, struct intr_handle *hdl);

/*!
 * \brief Disable queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] hdl Queue interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_queue_intr_disable(struct pdma_dev *dev, struct intr_handle *hdl);

/*!
 * \brief Acknowledge queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] hdl Queue interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_queue_intr_ack(struct pdma_dev *dev, struct intr_handle *hdl);

/*!
 * \brief Check queue interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] hdl Queue interrupt handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_queue_intr_check(struct pdma_dev *dev, struct intr_handle *hdl);

/*!
 * \brief Enable group interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] group Group number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_group_intr_enable(struct pdma_dev *dev, int group);

/*!
 * \brief Disable group interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] group Group number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_group_intr_disable(struct pdma_dev *dev, int group);

/*!
 * \brief Acknowledge group interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] group Group number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_group_intr_ack(struct pdma_dev *dev, int group);

/*!
 * \brief Check group interrupt.
 *
 * \param [in] dev Device structure point.
 * \param [in] group Group number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_group_intr_check(struct pdma_dev *dev, int group);

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
bcmcnet_rx_queue_poll(struct pdma_dev *dev, int queue, int budget);

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
bcmcnet_tx_queue_poll(struct pdma_dev *dev, int queue, int budget);

/*!
 * \brief Poll queue.
 *
 * \param [in] dev Device structure point.
 * \param [in] hdl Queue interrupt handle.
 * \param [in] budget Poll budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_queue_poll(struct pdma_dev *dev, struct intr_handle *hdl, int budget);

/*!
 * \brief Poll group.
 *
 * \param [in] dev Device structure point.
 * \param [in] group Group number.
 * \param [in] budget Poll budget.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_group_poll(struct pdma_dev *dev, int group, int budget);

#endif /* BCMCNET_CORE_H */

