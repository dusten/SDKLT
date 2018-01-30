/*! \file ngknet_main.h
 *
 * Data structure and macro definitions for NGKNET kernel module.
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

#ifndef NGKNET_MAIN_H
#define NGKNET_MAIN_H

#include <linux/netdevice.h>
#include <lkm/ngknet_dev.h>
#include <bcmcnet/bcmcnet_core.h>

/*! Maximum number of PDMA devices supported */
#ifdef NGBDE_NUM_SWDEV_MAX
#define NUM_PDMA_DEV_MAX    NGBDE_NUM_SWDEV_MAX
#else
#define NUM_PDMA_DEV_MAX    16
#endif

/*!
 * Debug levels
 */
#define DBG_LVL_VERB        0x0001
#define DBG_LVL_PKT         0x0002
#define DBG_LVL_CMD         0x0004
#define DBG_LVL_IRQ         0x0008
#define DBG_LVL_NAPI        0x0010
#define DBG_LVL_NDEV        0x0020
#define DBG_LVL_FILT        0x0040
#define DBG_LVL_RCPU        0x0080
#define DBG_LVL_WARN        0x0100
#define DBG_LVL_PDMP        0x0200
#define DBG_LVL_RATE        0x0400

#define DBG_VERB(_s)        do { if (debug & DBG_LVL_VERB) printk _s; } while (0)
#define DBG_PKT(_s)         do { if (debug & DBG_LVL_PKT)  printk _s; } while (0)
#define DBG_CMD(_s)         do { if (debug & DBG_LVL_CMD)  printk _s; } while (0)
#define DBG_IRQ(_s)         do { if (debug & DBG_LVL_IRQ)  printk _s; } while (0)
#define DBG_NAPI(_s)        do { if (debug & DBG_LVL_NAPI) printk _s; } while (0)
#define DBG_NDEV(_s)        do { if (debug & DBG_LVL_NDEV) printk _s; } while (0)
#define DBG_FILT(_s)        do { if (debug & DBG_LVL_FILT) printk _s; } while (0)
#define DBG_RCPU(_s)        do { if (debug & DBG_LVL_RCPU) printk _s; } while (0)
#define DBG_WARN(_s)        do { if (debug & DBG_LVL_WARN) printk _s; } while (0)
#define DBG_PDMP(_s)        do { if (debug & DBG_LVL_PDMP) printk _s; } while (0)
#define DBG_RATE(_s)        do { if (debug & DBG_LVL_RATE) printk _s; } while (0)

/*!
 * Device description
 */
struct ngknet_dev {
    /*! Base address for PCI register access */
    volatile void *base_addr;

    /*! Required for DMA memory control */
    struct device *dev;

    /*! Required for PCI memory control */
    struct pci_dev *pci_dev;

    /*! Base network device */
    struct net_device *net_dev;

    /*! PDMA device */
    struct pdma_dev pdma_dev;

    /*! Device number (from BDE) */
    int dev_no;

    /*! Vitual network devices, 0 is reserved */
    struct net_device *vdev[NUM_VDEV_MAX];

    /*! Vitual network devices bound to queue */
    struct net_device *bdev[NUM_QUE_MAX];

    /*! Filter list */
    struct list_head filt_list;

    /*! Filter control, 0 is reserved */
    void *fc[NUM_FILTER_MAX];

    /*! Number of filter priorities per queue */
    int num_rx_prio;

    /*! RCPU control */
    struct ngknet_rcpu_hdr rcpu_ctrl;

    /*! NGKNET wait queue */
    wait_queue_head_t wq;

    /*! NGKNET lock */
    spinlock_t lock;

    /*! Flags */
    int flags;
    /*! NGKNET device is active */
#define NGKNET_DEV_ACTIVE      (1 << 0)
};

/*!
 * Network interface specific private data
 */
struct ngknet_private {
    /*! Network device */
    struct net_device *net_dev;

    /*! Network stats */
    struct net_device_stats stats;

    /*! NGKNET device */
    struct ngknet_dev *bkn_dev;

    /*! Network interface ID */
    int id;

    /*! Network interface type */
    int type;

    /*! Network interface flags */
    uint32_t flags;

    /*! Network interface vlan */
    uint32_t vlan;

    /*! Network interface bound to */
    uint32_t chan;

    /*! Metadata length used to send packets to port */
    uint32_t meta_len;

    /*! Metadata used to send packets to port */
    uint8_t meta_data[NGKNET_NETIF_META_MAX];

    /*! Users of this network interface */
    int users;

    /*! Wait for this network interface free */
    int wait;
};

/*!
 * \brief Get debug level.
 *
 * \return Current debug level.
 */
extern int
ngknet_debug_level_get(void);

/*!
 * \brief Set debug level.
 *
 * \param [in] debug_level Debug level to be set.
 */
extern void
ngknet_debug_level_set(int debug_level);

/*!
 * \brief Get Rx rate limit.
 *
 * \return Current Rx rate limit.
 */
extern int
ngknet_rx_rate_limit_get(void);

/*!
 * \brief Set Rx rate limit.
 *
 * \return rate_limit Rx rate limit to be set.
 */
extern void
ngknet_rx_rate_limit_set(int rate_limit);

#endif /* NGKNET_MAIN_H */

