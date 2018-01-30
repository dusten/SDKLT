/*! \file ngknet_main.c
 *
 * NGKNET module entry.
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

/*
 * This module implements a Linux network driver for Broadcom
 * XGS switch devices. The driver simultaneously serves a
 * number of vitual Linux network devices.
 *
 * Packets received from the switch device are sent to a virtual
 * Linux network device based on a set of packet filters.
 *
 * Packets from the virtual Linux network devices are multiplexed
 * with fifo mode if only one Tx queue enabled.
 *
 * A command-based IOCTL interface is used for managing the devices,
 * packet filters and virtual Linux network interfaces.
 *
 * A virtual network interface can be configured to work in RCPU
 * mode, which means that packets from the switch device will
 * be encasulated with a RCPU header and a block of meta data
 * that basically contains the core DCB information. Likewise,
 * packets received from the Linux network stack are assumed to
 * be RCPU encapsulated when going out on an interface in RCPU
 * mode. If a virtual network interface does not work in RCPU
 * mode and transmits to this interface will unmodified go to
 * specified physical switch port, DCB information should be
 * provided when the interface is created.
 *
 * The module implements basic Rx DMA rate control. The rate is
 * specified in packets per second, and different Rx DMA channels
 * can be configured to use different maximum packet rates.
 * The packet rate can be configure as a module parameter, and
 * it can also be changed dynamically through the proc file
 * system (syntax is described in function header comment).
 *
 * For a list of supported module parameters, please see below.
 */

#include <linux/kconfig.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/if.h>
#include <linux/if_vlan.h>
#include <linux/net_tstamp.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/time.h>
#include <linux/random.h>

#include <lkm/ngbde_kapi.h>
#include <lkm/ngknet_dev.h>
#include <lkm/ngknet_ioctl.h>
#include <bcmcnet/bcmcnet_core.h>
#include "ngknet_main.h"
#include "ngknet_extra.h"
#include "ngknet_procfs.h"

/*! \cond */
MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("Network Device Driver Module");
MODULE_LICENSE("GPL");
/*! \endcond */

/*! \cond */
static int debug = 0;
MODULE_PARAM(debug, int, 0);
MODULE_PARM_DESC(debug,
"Debug level (default 0)");
/*! \endcond */

/*! \cond */
static char *base_dev_name = "bcm";
MODULE_PARAM(base_dev_name, charp, 0);
MODULE_PARM_DESC(base_dev_name,
"Base device name (default bcm0, bcm1, etc.)");
/*! \endcond */

/*! \cond */
static char *mac_addr = NULL;
MODULE_PARAM(mac_addr, charp, 0);
MODULE_PARM_DESC(mac_addr,
"Ethernet MAC address (default 02:10:18:xx:xx:xx)");
/*! \endcond */

/*! \cond */
static int default_mtu = 1500;
MODULE_PARAM(default_mtu, int, 0);
MODULE_PARM_DESC(default_mtu,
"Default MTU for NGKNET network interfaces (default 1500)");
/*! \endcond */

/*! \cond */
static int rx_buffer_size = RX_BUF_SIZE_DFLT;
MODULE_PARAM(rx_buffer_size, int, 0);
MODULE_PARM_DESC(rx_buffer_size,
"Default size of RX packet buffers (default 9216)");
/*! \endcond */

/*! \cond */
static int rx_rate_limit = -1;
MODULE_PARAM(rx_rate_limit, int, 0);
MODULE_PARM_DESC(rx_rate_limit,
"Rx rate limit (pps, default -1 no limit)");
/*! \endcond */

/*! \cond */
static int tx_polling = 0;
MODULE_PARAM(tx_polling, int, 0);
MODULE_PARM_DESC(tx_polling,
"Tx polling mode (default 0 in interrupt mode)");
/*! \endcond */

/*! \cond */
static int rx_batching = 0;
MODULE_PARAM(rx_batching, int, 0);
MODULE_PARM_DESC(rx_batching,
"Rx batching mode (default 0 in single fill mode)");
/*! \endcond */

typedef int (*drv_ops_attach)(struct pdma_dev *dev);

struct bcmcnet_drv_ops {
    const char         *drv_desc;
    drv_ops_attach      drv_attach;
    drv_ops_attach      drv_detach;
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    struct bcmcnet_drv_ops _bd##_cnet_drv_ops = { \
        #_bd, \
        _bd##_cnet_pdma_attach, \
        _bd##_cnet_pdma_detach, \
    };
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_cnet_drv_ops,
static struct bcmcnet_drv_ops *drv_ops[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};
static int drv_num = sizeof(drv_ops) / sizeof(drv_ops[0]);

struct ngknet_dev ngknet_devices[NUM_PDMA_DEV_MAX];

/* Default random MAC address has Broadcom OUI with local admin bit set */
static uint8_t ngknet_dev_mac[6] = {0x02, 0x10, 0x18, 0x00, 0x00, 0x00};

/* Interrupt handles */
struct ngknet_intr_handle {
    struct napi_struct napi;
    struct intr_handle *hdl;
};

static struct ngknet_intr_handle priv_hdl[NUM_PDMA_DEV_MAX][NUM_QUE_MAX];

/*!
 * Dump packet content for debug
 */
static void
ngknet_pkt_dump(uint8_t *data, int len)
{
    char str[128];
    int i;

    len = len > 256 ? 256 : len;

    for (i = 0; i < len; i++) {
        if ((i & 0x1f) == 0) {
            sprintf(str, "%04x: ", i);
        }
        sprintf(&str[strlen(str)], "%02x", data[i]);
        if ((i & 0x1f) == 0x1f) {
            sprintf(&str[strlen(str)], "\n");
            printk(str);
            continue;
        }
        if ((i & 0x3) == 0x3) {
            sprintf(&str[strlen(str)], " ");
        }
    }
    if ((i & 0x1f) != 0) {
        sprintf(&str[strlen(str)], "\n");
        printk(str);
    }
    printk("\n");
}

/*!
 * Rx packets rate test for debug
 */
static void
ngknet_pkt_stats(struct pdma_dev *pdev)
{
    static struct timeval tv0, tv1;
    static uint32_t pkts = 0;
    static uint64_t intrs = 0;

    if (pkts == 0) {
        do_gettimeofday(&tv0);
        intrs = pdev->stats.intrs;
    }
    if (++pkts >= 1000000) {
        uint32_t iv_time;
        uint32_t pps;
        do_gettimeofday(&tv1);
        iv_time = (tv1.tv_sec - tv0.tv_sec) * 1000000 + (tv1.tv_usec - tv0.tv_usec);
        pps = 1000000 * 1000 / (iv_time / 1000);
        printk("Rx 1M pkts, time: %d usec, rate: %d pps, intrs: %llu\n",
               iv_time, pps, pdev->stats.intrs - intrs);
        pkts = 0;
    }
}

/*!
 * Read 32-bit register callback
 */
static int
ngknet_dev_read32(struct pdma_dev *dev, uint32_t addr, uint32_t *data)
{
    *data = ngbde_kapi_pio_read32(dev->unit, addr);

    return 0;
}

/*!
 * Write 32-bit register callback
 */
static int
ngknet_dev_write32(struct pdma_dev *dev, uint32_t addr, uint32_t data)
{
    ngbde_kapi_pio_write32(dev->unit, addr, data);

    return 0;
}

/*!
 * \brief Process Rx packet.
 *
 * Add RCPU encapsulation or strip matadata if needed
 *
 * \param [in] ndev Network device structure point.
 * \param [in] skb Rx packet SKB.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_rx_frame_process(struct net_device *ndev, struct sk_buff *skb)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct ngknet_rcpu_hdr *rch = (struct ngknet_rcpu_hdr *)skb->data;
    struct pkt_buf *pkb = (struct pkt_buf *)skb->data;

    if (priv->flags & NGKNET_NETIF_F_RCPU_ENCAP) {
        /* Set up RCPU header */
        memcpy(skb->data, &pkb->data + pkb->pkh.meta_len, 2 * ETH_ALEN);
        if (*(uint32_t *)&dev->rcpu_ctrl.dst_mac[0] != 0 ||
            *(uint16_t *)&dev->rcpu_ctrl.dst_mac[4] != 0) {
            memcpy(skb->data, dev->rcpu_ctrl.dst_mac, ETH_ALEN);
        }
        if (*(uint32_t *)&dev->rcpu_ctrl.src_mac[0] != 0 ||
            *(uint16_t *)&dev->rcpu_ctrl.src_mac[4] != 0) {
            memcpy(&skb->data[6], dev->rcpu_ctrl.src_mac, ETH_ALEN);
        }
        rch->vlan_tpid = htons(dev->rcpu_ctrl.vlan_tpid);
        rch->vlan_tci = htons(dev->rcpu_ctrl.vlan_tci);
        rch->eth_type = htons(dev->rcpu_ctrl.eth_type);
        rch->pkt_sig = htons(dev->rcpu_ctrl.pkt_sig);
        rch->op_code = RCPU_OPCODE_RX;
        rch->flags = RCPU_FLAG_MODHDR;
        rch->trans_id = htons(dev->rcpu_ctrl.trans_id);
        rch->data_len = htons(pkb->pkh.data_len - ETH_FCS_LEN);
    } else {
        /* Remove packet header and meta data */
        skb_pull(skb, PKT_HDR_SIZE + pkb->pkh.meta_len);
    }

    /* Remove FCS from packet length */
    skb_trim(skb, skb->len - ETH_FCS_LEN);

    return SHR_E_NONE;
}

/*!
 * \brief Network interface Rx function.
 *
 * After processing the packet, send it up to network stack.
 *
 * \param [in] ndev Network device structure point.
 * \param [in] skb Rx packet SKB.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_netif_recv(struct net_device *ndev, struct sk_buff *skb)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct pdma_dev *pdev = &dev->pdma_dev;
    struct pkt_hdr *pkh = (struct pkt_hdr *)skb->data;
    struct napi_struct *napi = NULL;
    uint16_t proto;
    unsigned long flags;
    int chan, gi, qi;

    /* Handle one incoming packet */
    ngknet_rx_frame_process(ndev, skb);

    DBG_VERB(("Rx packet sent up to ndev%d (%d bytes).\n", priv->id, skb->len));
    if (debug & DBG_LVL_PDMP) {
        ngknet_pkt_dump(skb->data, skb->len);
    }

    if (ndev->features & NETIF_F_RXCSUM) {
        if ((pkh->attrs & (PDMA_RX_TU_CSUM | PDMA_RX_IP_CSUM)) ==
            (PDMA_RX_TU_CSUM | PDMA_RX_IP_CSUM)) {
            skb->ip_summed = CHECKSUM_UNNECESSARY;
        } else {
            skb_checksum_none_assert(skb);
        }
    }

    if (priv->flags & NGKNET_NETIF_F_RCPU_ENCAP) {
        kal_vlan_hwaccel_put_tag(skb, skb->data[12] << 8 | skb->data[13],
                                 skb->data[14] << 8 | skb->data[15]);
    } else if (ndev->features & NETIF_F_HW_VLAN_CTAG_RX &&
               pkh->attrs & PDMA_RX_STRIP_TAG) {
        kal_vlan_hwaccel_put_tag(skb, ETH_P_8021Q, priv->vlan);
    }

    proto = eth_type_trans(skb, ndev);
    if (!(pkh->attrs & PDMA_RX_SET_PROTO) || !skb->protocol) {
        skb->protocol = proto;
    }

    if (priv->id <= 0) {
        skb_record_rx_queue(skb, pkh->queue_id);
    } else {
        skb_record_rx_queue(skb, 0);
    }

    bcmcnet_pdma_dev_queue_to_chan(pdev, pkh->queue_id, PDMA_Q_RX, &chan);
    gi = chan / pdev->grp_queues;
    if (pdev->flags & PDMA_GROUP_INTR) {
        napi = (struct napi_struct *)pdev->ctrl.grp[gi].intr_hdl[0].priv;
    } else {
        qi = pkh->queue_id;
        napi = (struct napi_struct *)pdev->ctrl.grp[gi].intr_hdl[qi].priv;
    }
    napi_gro_receive(napi, skb);

    if (priv->id > 0) {
        spin_lock_irqsave(&dev->lock, flags);
        priv->users--;
        if (!priv->users && priv->wait) {
            wake_up(&dev->wq);
        }
        spin_unlock_irqrestore(&dev->lock, flags);
    }

    /* Update accounting */
    priv->stats.rx_packets++;
    priv->stats.rx_bytes += skb->len;

    /* Rate limit */
    if (rx_rate_limit >= 0) {
        if (!ngknet_rx_rate_limit_started()) {
            ngknet_rx_rate_limit_start(dev);
        }
        ngknet_rx_rate_limit(dev, rx_rate_limit);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Driver Rx callback.
 *
 * After processing the packet, send it up to network stack.
 *
 * \param [in] pdev Packet DMA device structure point.
 * \param [in] buf Raw Rx buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_frame_recv(struct pdma_dev *pdev, int queue, void *buf)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)pdev->priv;
    struct sk_buff *skb = (struct sk_buff *)buf, *mskb = NULL;
    struct net_device *ndev = NULL, *mndev = NULL;
    int rv;

    DBG_VERB(("Rx packet (%d bytes).\n", skb->len));
    if (debug & DBG_LVL_PDMP) {
        ngknet_pkt_dump(skb->data, skb->len);
    }
    if (debug & DBG_LVL_RATE) {
        ngknet_pkt_stats(pdev);
        return SHR_E_FAIL;
    }

    /* Go through the filters */
    rv = ngknet_rx_pkt_filter(dev, skb, &ndev, &mndev, &mskb);
    if (SHR_FAILURE(rv) || !ndev) {
        return SHR_E_FAIL;
    }

    /* Populate header, checksum status, VLAN, and protocol */
    ngknet_netif_recv(ndev, skb);

    /* Handle mirrored packet */
    if (mndev && mskb) {
        ngknet_netif_recv(mndev, mskb);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Process Tx packet.
 *
 * Strip RCPU encapsulation, setup CNET packet buffer, add vlan tag
 * or pad the packet.
 *
 * \param [in] ndev Network device structure point.
 * \param [in] oskb Tx packet SKB.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_tx_frame_process(struct net_device *ndev, struct sk_buff **oskb)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct sk_buff *skb = *oskb;
    struct ngknet_rcpu_hdr *rch = (struct ngknet_rcpu_hdr *)skb->data;
    struct pkt_hdr *pkh = (struct pkt_hdr *)skb->data;
    struct sk_buff *nskb = NULL;
    char *data = NULL;
    uint32_t copy_len, meta_len, data_len, pkt_len, tag_len;
    uint16_t tpid;

    /* Set up packet header */
    if (priv->flags & NGKNET_NETIF_F_RCPU_ENCAP) {
        /* RCPU encapsulation packet */
        data_len = pkh->attrs & PDMA_TX_HDR_COOKED ? pkh->data_len : ntohs(rch->data_len);
        pkt_len = PKT_HDR_SIZE + rch->meta_len + data_len;
        if (skb->len != pkt_len || skb->len < (PKT_HDR_SIZE + 14)) {
            DBG_WARN(("Tx drop: Invalid RCPU encapsulation\n"));
            return SHR_E_UNAVAIL;
        }
        if (dev->rcpu_ctrl.pkt_sig && dev->rcpu_ctrl.pkt_sig != ntohs(rch->pkt_sig)) {
            DBG_WARN(("Tx drop: Invalid RCPU signature\n"));
            return SHR_E_UNAVAIL;
        }
        if (pkh->attrs & PDMA_TX_HDR_COOKED) {
            /* Resumed packet */
            return SHR_E_NONE;
        }
        pkh->data_len = data_len + ETH_FCS_LEN;
        pkh->meta_len = rch->meta_len;
        pkh->attrs = 0;
        if (pkh->meta_len && rch->flags & RCPU_FLAG_MODHDR) {
            pkh->attrs |= PDMA_TX_HIGIG_PKT;
        }
        if (rch->flags & RCPU_FLAG_PAUSE) {
            pkh->attrs |= PDMA_TX_PAUSE_PKT;
        }
        if (rch->flags & RCPU_FLAG_PURGE) {
            pkh->attrs |= PDMA_TX_PURGE_PKT;
        }
        if (rch->flags & RCPU_FLAG_BIND_QUE) {
            pkh->attrs |= PDMA_TX_BIND_QUE;
        }
    } else {
        /* Non-RCPU encapsulation packet */
        data_len = pkh->data_len;
        pkt_len = PKT_HDR_SIZE + pkh->meta_len + data_len;
        if (skb->len == pkt_len && pkh->attrs & PDMA_TX_HDR_COOKED &&
            pkh->pkt_sig == dev->rcpu_ctrl.pkt_sig) {
            /* Resumed packet */
            return SHR_E_NONE;
        }
        meta_len = 0;
        if (priv->type == NGKNET_NETIF_T_PORT) {
            meta_len = dev->pdma_dev.info.tx_ph_size;
            if (!meta_len) {
                meta_len = NGKNET_NETIF_META_MAX;
            }
        }
        if (skb_header_cloned(skb) ||
            skb_headroom(skb) < (PKT_HDR_SIZE + meta_len + VLAN_HLEN) ||
            skb_tailroom(skb) < ETH_FCS_LEN) {
            nskb = netdev_alloc_skb(ndev,
                                    skb->len + PKT_HDR_SIZE + meta_len +
                                    VLAN_HLEN + ETH_FCS_LEN);
            if (!nskb) {
                return SHR_E_MEMORY;
            }
            skb_reserve(nskb, PKT_HDR_SIZE + meta_len + VLAN_HLEN);
            memcpy(nskb->data, skb->data, skb->len);
            skb_put(nskb, skb->len);
            *oskb = skb = nskb;
        }
        skb_push(skb, PKT_HDR_SIZE + meta_len);
        memset(skb->data, 0, PKT_HDR_SIZE + meta_len);
        pkh = (struct pkt_hdr *)skb->data;
        pkh->data_len = skb->len - PKT_HDR_SIZE - meta_len + ETH_FCS_LEN;
        pkh->meta_len = meta_len;
        pkh->attrs = 0;
        if (priv->type == NGKNET_NETIF_T_PORT) {
            /* Send to physical port using netif metadata */
            if (priv->meta_len && priv->meta_len <= meta_len) {
                memcpy(skb->data + PKT_HDR_SIZE, priv->meta_data, priv->meta_len);
            } else {
                printk("Tx abort: no metadata\n");
                return SHR_E_UNAVAIL;
            }
            pkh->attrs |= PDMA_TX_HIGIG_PKT;
        }
        pkh->pkt_sig = dev->rcpu_ctrl.pkt_sig;
    }

    /* Packet header done here */
    pkh->attrs |= PDMA_TX_HDR_COOKED;

    data = skb->data + PKT_HDR_SIZE + pkh->meta_len;
    tpid = data[12] << 8 | data[13];
    tag_len = (tpid == ETH_P_8021Q || tpid == ETH_P_8021AD) ? VLAN_HLEN : 0;

    /* Need to add VLAN tag if packet is untagged */
    if (!tag_len && (!(pkh->attrs & PDMA_TX_HIGIG_PKT) || priv->flags & NGKNET_NETIF_F_ADD_TAG)) {
        copy_len = PKT_HDR_SIZE + pkh->meta_len + 2 * ETH_ALEN;
        if (skb_header_cloned(skb) || skb_headroom(skb) < VLAN_HLEN) {
            data_len = pkh->data_len;
            meta_len = pkh->meta_len;
            nskb = netdev_alloc_skb(ndev, PKT_HDR_SIZE + meta_len + data_len + VLAN_HLEN);
            if (!nskb) {
                return SHR_E_MEMORY;
            }
            memcpy(nskb->data, skb->data, copy_len);
            memcpy(&nskb->data[copy_len + VLAN_HLEN], &skb->data[copy_len],
                   data_len - 2 * ETH_ALEN);
            skb_put(nskb, PKT_HDR_SIZE + meta_len + data_len + VLAN_HLEN);
            *oskb = skb = nskb;
        } else {
            skb_push(skb, VLAN_HLEN);
            memmove(skb->data, skb->data + VLAN_HLEN, copy_len);
        }
        pkh = (struct pkt_hdr *)skb->data;
        data = skb->data + PKT_HDR_SIZE + pkh->meta_len;
        data[12] = 0x81;
        data[13] = 0x00;
        data[14] = priv->vlan >> 8 & 0xf;
        data[15] = priv->vlan & 0xff;
        pkh->data_len += VLAN_HLEN;
        tag_len = VLAN_HLEN;
    }

    /* Pad packet if needed */
    if (pkh->data_len < (64 + tag_len)) {
        pkh->data_len = 64 + tag_len;
        if (skb_padto(skb, PKT_HDR_SIZE + pkh->meta_len + pkh->data_len)) {
            return SHR_E_MEMORY;
        }
    }

    return SHR_E_NONE;
}

/*!
 * Suspend Tx queue callback
 */
static void
ngknet_tx_suspend(struct pdma_dev *pdev, int queue)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)pdev->priv;
    unsigned long flags;
    int vdi;

    netif_stop_subqueue(dev->net_dev, queue);

    spin_lock_irqsave(&dev->lock, flags);
    for (vdi = 1; vdi < NUM_VDEV_MAX; vdi++) {
        if (!dev->vdev[vdi]) {
            continue;
        }
        netif_stop_subqueue(dev->vdev[vdi], queue);
    }
    spin_unlock_irqrestore(&dev->lock, flags);
}

/*!
 * Resume Tx queue callback
 */
static void
ngknet_tx_resume(struct pdma_dev *pdev, int queue)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)pdev->priv;
    unsigned long flags;
    int vdi;

    if (__netif_subqueue_stopped(dev->net_dev, queue)) {
        netif_wake_subqueue(dev->net_dev, queue);
    }

    spin_lock_irqsave(&dev->lock, flags);
    for (vdi = 1; vdi < NUM_VDEV_MAX; vdi++) {
        if (!dev->vdev[vdi]) {
            continue;
        }
        if (__netif_subqueue_stopped(dev->vdev[vdi], queue)) {
            netif_wake_subqueue(dev->vdev[vdi], queue);
        }
    }
    spin_unlock_irqrestore(&dev->lock, flags);
}

/*!
 * Enable interrupt callback
 */
static void
ngknet_intr_enable(struct pdma_dev *pdev, int cmc, int chan,
                   uint32_t reg, uint32_t mask)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)pdev->priv;

    pdev->ctrl.grp[cmc].irq_mask |= mask;

    ngbde_kapi_intr_mask_write(dev->dev_no, 0, reg, pdev->ctrl.grp[cmc].irq_mask);
}

/*!
 * Disable interrupt callback
 */
static void
ngknet_intr_disable(struct pdma_dev *pdev, int cmc, int chan,
                    uint32_t reg, uint32_t mask)
{
    struct ngknet_dev *dev = (struct ngknet_dev *)pdev->priv;

    pdev->ctrl.grp[cmc].irq_mask &= ~mask;

    ngbde_kapi_intr_mask_write(dev->dev_no, 0, reg, pdev->ctrl.grp[cmc].irq_mask);
}

/*!
 * NAPI polling function
 */
static int
ngknet_poll(struct napi_struct *napi, int budget)
{
    struct intr_handle *hdl = ((struct ngknet_intr_handle *)napi)->hdl;
    struct pdma_dev *pdev = (struct pdma_dev *)hdl->dev;
    struct ngknet_dev *dev = (struct ngknet_dev *)pdev->priv;
    unsigned long flags;
    int work_done;

    DBG_NAPI(("Scheduled NAPI on queue %d.\n", hdl->queue));

    if (pdev->flags & PDMA_GROUP_INTR) {
        work_done = bcmcnet_group_poll(pdev, hdl->group, budget);
        if (work_done < budget) {
            napi_complete(napi);
            spin_lock_irqsave(&dev->lock, flags);
            bcmcnet_group_intr_enable(pdev, hdl->group);
            spin_unlock_irqrestore(&dev->lock, flags);
        }
    } else {
        work_done = bcmcnet_queue_poll(pdev, hdl, budget);
        if (work_done < budget) {
            napi_complete(napi);
            spin_lock_irqsave(&dev->lock, flags);
            bcmcnet_queue_intr_enable(pdev, hdl);
            spin_unlock_irqrestore(&dev->lock, flags);
        }
    }

    return work_done;
}

/*!
 * NGKNET ISR
 */
static int
ngknet_isr(void *isr_data)
{
    struct ngknet_dev *dev = isr_data;
    struct pdma_dev *pdev = &dev->pdma_dev;
    struct intr_handle *hdl = NULL;
    struct napi_struct *napi = NULL;
    unsigned long flags;
    int gi, qi;
    int iv = 0;

    for (gi = 0; gi < pdev->num_groups; gi++) {
        if (!pdev->ctrl.grp[gi].attached) {
            continue;
        }
        for (qi = 0; qi < pdev->grp_queues; qi++) {
            hdl = &pdev->ctrl.grp[gi].intr_hdl[qi];
            napi = (struct napi_struct *)hdl->priv;
            if (pdev->flags & PDMA_GROUP_INTR) {
                if (!bcmcnet_group_intr_check(pdev, gi)) {
                    break;
                }
                if (likely(napi_schedule_prep(napi))) {
                    spin_lock_irqsave(&dev->lock, flags);
                    bcmcnet_group_intr_disable(pdev, gi);
                    spin_unlock_irqrestore(&dev->lock, flags);
                    __napi_schedule(napi);
                } else {
                    bcmcnet_group_intr_ack(pdev, gi);
                }
                iv++;
                break;
            }
            if (!bcmcnet_queue_intr_check(pdev, hdl)) {
                continue;
            }
            if (likely(napi_schedule_prep(napi))) {
                spin_lock_irqsave(&dev->lock, flags);
                bcmcnet_queue_intr_disable(pdev, hdl);
                spin_unlock_irqrestore(&dev->lock, flags);
                __napi_schedule(napi);
            } else {
                bcmcnet_queue_intr_ack(pdev, hdl);
            }
            iv++;
        }
    }

    if (iv) {
        DBG_IRQ(("Got interrupt on device %d.\n", dev->dev_no));
        pdev->stats.intrs++;
        return IRQ_HANDLED;
    } else {
        return IRQ_NONE;
    }
}

/*!
 * Open network device
 */
static int
ngknet_enet_open(struct net_device *ndev)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct pdma_dev *pdev = &dev->pdma_dev;
    struct napi_struct *napi = NULL;
    unsigned long bm_queue;
    int gi, qi;
    int rv;

    if (!pdev->ctrl.bm_rxq || !pdev->ctrl.bm_txq) {
        printk("Not config Rx or Tx queue yet!\n");
        return -EPERM;
    }

    if (priv->id <= 0) {
        /* Register interrupt handler */
        ngbde_kapi_intr_connect(dev->dev_no, 0, ngknet_isr, dev);

        /* Start PDMA device */
        rv = bcmcnet_pdma_dev_start(pdev);
        if (SHR_FAILURE(rv)) {
            ngbde_kapi_intr_disconnect(dev->dev_no, 0);
            return -EPERM;
        }

        /* Start rate limit */
        if (rx_rate_limit >= 0) {
            ngknet_rx_rate_limit_start(dev);
        }

        /* Notify the stack of the actual queue counts. */
        rv = netif_set_real_num_rx_queues(dev->net_dev, pdev->ctrl.nb_rxq);
        if (rv < 0) {
            ngbde_kapi_intr_disconnect(dev->dev_no, 0);
            return rv;
        }
        rv = netif_set_real_num_tx_queues(dev->net_dev, pdev->ctrl.nb_txq);
        if (rv < 0) {
            ngbde_kapi_intr_disconnect(dev->dev_no, 0);
            return rv;
        }

        for (gi = 0; gi < pdev->num_groups; gi++) {
            if (!pdev->ctrl.grp[gi].attached) {
                continue;
            }
            bm_queue = pdev->ctrl.grp[gi].bm_rxq | pdev->ctrl.grp[gi].bm_txq;
            for (qi = 0; qi < pdev->grp_queues; qi++) {
                napi = (struct napi_struct *)pdev->ctrl.grp[gi].intr_hdl[qi].priv;
                if (pdev->flags & PDMA_GROUP_INTR) {
                    napi_enable(napi);
                    break;
                }
                if (1 << qi & bm_queue) {
                    napi_enable(napi);
                }
            }
        }
    } else {
        /* Notify the stack of the actual queue counts. */
        rv = netif_set_real_num_rx_queues(ndev, pdev->ctrl.nb_rxq);
        if (rv < 0) {
            return rv;
        }
        rv = netif_set_real_num_tx_queues(ndev, pdev->ctrl.nb_txq);
        if (rv < 0) {
            return rv;
        }
    }

    /* Prevent tx timeout */
    ndev->trans_start = jiffies;

    netif_tx_wake_all_queues(ndev);

    return 0;
}

/*!
 * Stop network device
 */
static int
ngknet_enet_stop(struct net_device *ndev)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct pdma_dev *pdev = &dev->pdma_dev;
    struct napi_struct *napi = NULL;
    unsigned long bm_queue;
    int gi, qi;

    netif_tx_stop_all_queues(ndev);

    if (priv->id <= 0) {
        /* Stop rate limit */
        if (rx_rate_limit >= 0) {
            ngknet_rx_rate_limit_stop(dev);
        }

        /* Suspend PDMA device */
        bcmcnet_pdma_dev_suspend(pdev);

        for (gi = 0; gi < pdev->num_groups; gi++) {
            if (!pdev->ctrl.grp[gi].attached) {
                continue;
            }
            bm_queue = pdev->ctrl.grp[gi].bm_rxq | pdev->ctrl.grp[gi].bm_txq;
            for (qi = 0; qi < pdev->grp_queues; qi++) {
                napi = (struct napi_struct *)pdev->ctrl.grp[gi].intr_hdl[qi].priv;
                if (pdev->flags & PDMA_GROUP_INTR) {
                    napi_disable(napi);
                    break;
                }
                if (1 << qi & bm_queue) {
                    napi_disable(napi);
                }
            }
        }

        /* Stop PDMA device */
        bcmcnet_pdma_dev_stop(pdev);

        /* Unregister interrupt handler */
        ngbde_kapi_intr_disconnect(dev->dev_no, 0);
    }

    return 0;
}

/*!
 * Start transmission
 */
static int
ngknet_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
    struct ngknet_private *priv = netdev_priv(ndev);
    struct ngknet_dev *dev = priv->bkn_dev;
    struct pdma_dev *pdev = &dev->pdma_dev;
    struct sk_buff *bskb = skb;
    uint32_t len = skb->len;
    struct pkt_hdr *pkh = NULL;
    uint32_t *meta = NULL;
    int queue;
    unsigned long flags;
    int chan, gi, qi;
    int i;
    int rv;

    DBG_VERB(("Tx packet from ndev%d (%d bytes).\n", priv->id, skb->len));
    if (debug & DBG_LVL_PDMP) {
        ngknet_pkt_dump(skb->data, skb->len);
    }

    queue = skb->queue_mapping;

    /* Handle one outgoing packet */
    rv = ngknet_tx_frame_process(ndev, &skb);
    if (SHR_FAILURE(rv)) {
        priv->stats.tx_dropped++;
        dev_kfree_skb_any(skb);
        return NETDEV_TX_OK;
    }

    if (priv->id > 0) {
        spin_lock_irqsave(&dev->lock, flags);
        priv->users++;
        spin_unlock_irqrestore(&dev->lock, flags);
    }

    /* Schedule Tx queue */
    ngknet_tx_queue_schedule(dev, skb, &queue);
    skb->queue_mapping = queue;

    /* Swap header if any */
    if (priv->type == NGKNET_NETIF_T_PORT && dev->pdma_dev.info.tx_ph_size) {
        bcmcnet_pdma_dev_queue_to_chan(pdev, queue, PDMA_Q_TX, &chan);
        gi = chan / pdev->grp_queues;
        qi = chan % pdev->grp_queues;
        if (pdev->ctrl.grp[gi].que_ctrl[qi] & PDMA_HDR_BYTE_SWAP) {
            pkh = (struct pkt_hdr *)skb->data;
            meta = (uint32_t *)(skb->data + PKT_HDR_SIZE);
            for (i = 0; i < pkh->meta_len / sizeof(uint32_t); i++) {
                 meta[i] = htonl(meta[i]);
            }
        }
    }

    DBG_VERB(("Tx packet (%d bytes).\n", skb->len));
    if (debug & DBG_LVL_PDMP) {
        ngknet_pkt_dump(skb->data, skb->len);
    }

    rv = pdev->pkt_xmit(pdev, queue, skb);

    if (priv->id > 0) {
        spin_lock_irqsave(&dev->lock, flags);
        priv->users--;
        if (!priv->users && priv->wait) {
            wake_up(&dev->wq);
        }
        spin_unlock_irqrestore(&dev->lock, flags);
    }

    if (rv == SHR_E_UNAVAIL) {
        DBG_WARN(("Tx drop: DMA device not ready\n"));
        priv->stats.tx_dropped++;
        dev_kfree_skb_any(skb);
        return NETDEV_TX_OK;
    }

    if (rv == SHR_E_BUSY) {
        DBG_WARN(("Tx suspend: No DMA resources\n"));
        priv->stats.tx_fifo_errors++;
        if (skb != bskb) {
            dev_kfree_skb_any(skb);
            skb = bskb;
        }
        return NETDEV_TX_BUSY;
    } else {
        if (skb != bskb) {
            dev_kfree_skb_any(bskb);
        }
    }

    /* Update accounting */
    priv->stats.tx_packets++;
    priv->stats.tx_bytes += len;

    return NETDEV_TX_OK;
}

/*!
 * Get network device stats
 */
static struct net_device_stats *
ngknet_get_stats(struct net_device *ndev)
{
    struct ngknet_private *priv = netdev_priv(ndev);

    return &priv->stats;
}

/*!
 * Set network device MC list
 */
static void
ngknet_set_multicast_list(struct net_device *ndev)
{
    return;
}

/*!
 * Set network device MAC address
 */
static int
ngknet_set_mac_address(struct net_device *ndev, void *addr)
{
    if (!is_valid_ether_addr(((struct sockaddr *)addr)->sa_data)) {
        return -EINVAL;
    }

    netdev_info(ndev, "Setting new MAC address\n");
    memcpy(ndev->dev_addr, ((struct sockaddr *)addr)->sa_data, ndev->addr_len);

    return 0;
}

/*!
 * Change network device MTU
 */
static int
ngknet_change_mtu(struct net_device *ndev, int new_mtu)
{
    int frame_size = new_mtu + ETH_HLEN + VLAN_HLEN + ETH_FCS_LEN;

    if (frame_size < 68 || frame_size > rx_buffer_size) {
        return -EINVAL;
    }

    netdev_info(ndev, "Changing MTU from %d to %d\n", ndev->mtu, new_mtu);
    ndev->mtu = new_mtu;

    return 0;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/*!
 * Poll network device
 */
static void
ngknet_poll_controller(struct net_device *ndev)
{
    struct ngknet_private *priv = netdev_priv(ndev);

    disable_irq(ndev->irq);
    ngknet_isr(priv->bkn_dev);
    enable_irq(ndev->irq);
}
#endif

static const struct net_device_ops ngknet_netdev_ops = {
    .ndo_open            = ngknet_enet_open,
    .ndo_stop            = ngknet_enet_stop,
    .ndo_start_xmit      = ngknet_start_xmit,
    .ndo_get_stats       = ngknet_get_stats,
    .ndo_validate_addr   = eth_validate_addr,
    .ndo_set_rx_mode     = ngknet_set_multicast_list,
    .ndo_set_mac_address = ngknet_set_mac_address,
    .ndo_change_mtu      = ngknet_change_mtu,
    .ndo_set_features    = NULL,
    .ndo_do_ioctl        = NULL,
    .ndo_tx_timeout      = NULL,
#ifdef CONFIG_NET_POLL_CONTROLLER
    .ndo_poll_controller = ngknet_poll_controller,
#endif
};

/*!
 * \brief Initialize network device.
 *
 * \param [in] name Network device name.
 * \param [in] mac Network device MAC address.
 * \param [out] nd New registered network device.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_ndev_init(ngknet_netif_t *netif, struct net_device **nd)
{
    struct net_device *ndev = NULL;
    uint8_t *ma;
    int rv;

    ndev = alloc_etherdev_mq(sizeof(struct ngknet_private), NUM_QUE_MAX);
    if (!ndev) {
        DBG_WARN(("Error allocating network device.\n"));
        return SHR_E_MEMORY;
    }

    /* Device information -- not available right now */
    ndev->irq = 0;
    ndev->base_addr = 0;

    /* Fill in the dev structure */
    ndev->watchdog_timeo = 5 * HZ;

    /* Default MTU should not exceed MTU of switch front-panel ports */
    ndev->mtu = netif->mtu;
    if (!ndev->mtu) {
        ndev->mtu = default_mtu ? default_mtu : rx_buffer_size;
    }

    ndev->netdev_ops = &ngknet_netdev_ops;
    /*ndev->ethtool_ops = &ngknet_ethtool_ops;*/

    /* Network device name */
    if (netif->name && *netif->name) {
        strncpy(ndev->name, netif->name, IFNAMSIZ - 1);
    }

    /* Set the device MAC address */
    ma = netif->macaddr;
    if ((ma[0] | ma[1] | ma[2] | ma[3] | ma[4] | ma[5]) == 0) {
        ngknet_dev_mac[5]++;
        ma = ngknet_dev_mac;
    }
    memcpy(ndev->dev_addr, ma, ETH_ALEN);

    /* Initialize the device features */
    ndev->hw_features = NETIF_F_IP_CSUM | NETIF_F_SG | NETIF_F_RXCSUM |
                        NETIF_F_HW_VLAN_CTAG_RX | NETIF_F_HW_VLAN_CTAG_TX;
    ndev->features |= NETIF_F_IP_CSUM | NETIF_F_SG | NETIF_F_RXCSUM |
                      NETIF_F_HIGHDMA | NETIF_F_HW_VLAN_CTAG_RX;

    /* Register the kernel network device */
    rv = register_netdev(ndev);
    if (rv < 0) {
        DBG_WARN(("Error registering network device %s.\n", ndev->name));
        free_netdev(ndev);
        return SHR_E_FAIL;
    }

    *nd = ndev;

    DBG_VERB(("Created network device %s.\n", ndev->name));

    return SHR_E_NONE;
}

/*!
 * \brief Initialize Packet DMA device.
 *
 * \param [in] dev NGKNET device structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_pdev_init(struct ngknet_dev *dev)
{
    struct pdma_dev *pdev = &dev->pdma_dev;
    int rv;

    /* Initialize PDMA control structure */
    pdev->unit = dev->dev_no;
    pdev->priv = dev;
    pdev->ctrl.dev = pdev;
    pdev->ctrl.hw_addr = dev->base_addr;
    pdev->ctrl.rx_buf_size = rx_buffer_size;

    /* Hook callbacks */
    pdev->dev_read32 = ngknet_dev_read32;
    pdev->dev_write32 = ngknet_dev_write32;
    pdev->pkt_recv = ngknet_frame_recv;
    pdev->tx_suspend = ngknet_tx_suspend;
    pdev->tx_resume = ngknet_tx_resume;
    pdev->intr_unmask = ngknet_intr_enable;
    pdev->intr_mask = ngknet_intr_disable;

    pdev->flags |= PDMA_GROUP_INTR;
    if (tx_polling) {
        pdev->flags |= PDMA_TX_POLLING;
    }
    if (rx_batching) {
        pdev->flags |= PDMA_RX_BATCHING;
    }

    /* Attach PDMA driver */
    rv = drv_ops[pdev->dev_type]->drv_attach(pdev);
    if (SHR_FAILURE(rv)) {
        DBG_WARN(("Attach DMA driver failed.\n"));
        return rv;
    }

    /* Initialize PDMA device */
    rv = bcmcnet_pdma_dev_init(pdev);
    if (SHR_FAILURE(rv)) {
        DBG_WARN(("Init DMA device.failed.\n"));
        return rv;
    }

    DBG_VERB(("Attached DMA device %s.\n", pdev->name));

    return SHR_E_NONE;
}

/*!
 * \brief Get device information from BDE.
 *
 * \param [in] dn Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_dev_info_get(int dn)
{
    struct ngknet_dev *dev = &ngknet_devices[dn];

    dev->base_addr = ngbde_kapi_pio_membase(dn);
    dev->dev = ngbde_kapi_dma_dev_get(dn);
    dev->pci_dev = ngbde_kapi_pci_dev_get(dn);

    if (!dev->base_addr || !dev->dev || !dev->pci_dev) {
        return SHR_E_ACCESS;
    }

    dev->dev_no = dn;

    return SHR_E_NONE;
}

/*!
 * \brief Probe device.
 *
 * Get the information from BDE, initialize Packet DMA device,
 * initialize base network device and allocate other resources.
 *
 * \param [in] dn Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_dev_probe(int dn, ngknet_netif_t *netif)
{
    struct ngknet_dev *dev = &ngknet_devices[dn];
    struct pdma_dev *pdev = &dev->pdma_dev;
    struct net_device *ndev = NULL;
    struct ngknet_private *priv = NULL;
    struct intr_handle *hdl = NULL;
    int gi, qi;
    int rv;

    DBG_VERB(("%s: dev %d\n",__FUNCTION__, dn));

    /* Get device information */
    rv = ngknet_dev_info_get(dn);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Initialize PDMA device */
    rv = ngknet_pdev_init(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Get base network device name */
    if (netif->name[0] == '\0') {
        /* Reserve 6 vacancies for base&vitual device number, i.e. nameAB_XYZ */
        if (strlen(base_dev_name) < IFNAMSIZ - 6) {
            snprintf(netif->name, IFNAMSIZ, "%s%d", base_dev_name, dn);
        } else {
            DBG_WARN(("Too long network device name: %s.\n", base_dev_name));
            return SHR_E_PARAM;
        }
    }

    rv = ngknet_ndev_init(netif, &ndev);
    if (SHR_FAILURE(rv)) {
        bcmcnet_pdma_dev_deinit(pdev);
        return rv;
    }
    dev->net_dev = ndev;

    /* Initialize private information for base network device */
    priv = netdev_priv(ndev);
    priv->net_dev = ndev;
    priv->bkn_dev = dev;
    priv->id = 0;
    priv->type = netif->type;
    if (priv->type == NGKNET_NETIF_T_PORT) {
        priv->meta_len = netif->meta_len;
        memcpy(priv->meta_data, netif->meta_data, priv->meta_len);
    }
    priv->flags = netif->flags;
    priv->vlan = netif->vlan;
    priv->chan = netif->chan;

    netif->id = priv->id;
    memcpy(netif->macaddr, ndev->dev_addr, ETH_ALEN);
    netif->mtu = ndev->mtu;
    memcpy(netif->name, ndev->name, NGKNET_NETIF_NAME_MAX - 1);

    if (priv->flags & NGKNET_NETIF_F_BIND_CHAN) {
        dev->bdev[priv->chan] = ndev;
    }

    /* Register for napi */
    for (gi = 0; gi < pdev->num_groups; gi++) {
        if (!pdev->ctrl.grp[gi].attached) {
            continue;
        }
        for (qi = 0; qi < pdev->grp_queues; qi++) {
            hdl = &pdev->ctrl.grp[gi].intr_hdl[qi];
            priv_hdl[hdl->unit][hdl->chan].hdl = hdl;
            hdl->priv = &priv_hdl[hdl->unit][hdl->chan];
            netif_napi_add(ndev, (struct napi_struct *)hdl->priv,
                           ngknet_poll, pdev->ctrl.budget);
            if (pdev->flags & PDMA_GROUP_INTR) {
                break;
            }
        }
    }

    INIT_LIST_HEAD(&dev->filt_list);
    init_waitqueue_head(&dev->wq);
    spin_lock_init(&dev->lock);

    dev->num_rx_prio = 1;
    dev->flags |= NGKNET_DEV_ACTIVE;

    DBG_NDEV(("Broadcom NGKNET Attached\n"));
    DBG_NDEV(("MAC: %pM\n", ndev->dev_addr));
    DBG_NDEV(("Running with NAPI enabled\n"));

    return SHR_E_NONE;
}

/*!
 * \brief Remove device.
 *
 * Suspend device firstly, destroy all virtual network devices
 * and filters, deinitialize Packet DMA device.
 *
 * \param [in] dn Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_dev_remove(int dn)
{
    struct ngknet_dev *dev = &ngknet_devices[dn];
    struct pdma_dev *pdev = &dev->pdma_dev;
    struct net_device *ndev = NULL;
    struct intr_handle *hdl = NULL;
    int di, gi, qi;
    int rv;

    if (!(dev->flags & NGKNET_DEV_ACTIVE)) {
        return SHR_E_NONE;
    }

    DBG_VERB(("%s: dev %d\n",__FUNCTION__, dn));

    /* Destroy all the filters */
    ngknet_filter_destroy_all(dev);

    /* Destroy all the virtual devices */
    for (di = 1; di < NUM_VDEV_MAX; di++) {
        ndev = dev->vdev[di];
        if (ndev) {
            unregister_netdev(ndev);
            free_netdev(ndev);
            dev->vdev[di] = NULL;
        }
    }
    dev->vdev[0] = NULL;

    DBG_VERB(("Removing base network device %s.\n", dev->net_dev->name));

    /* Destroy the base network device */
    ndev = dev->net_dev;
    unregister_netdev(ndev);
    free_netdev(ndev);

    for (qi = 0; qi < NUM_QUE_MAX; qi++) {
        dev->bdev[qi] = NULL;
    }

    for (gi = 0; gi < pdev->num_groups; gi++) {
        if (!pdev->ctrl.grp[gi].attached) {
            continue;
        }
        for (qi = 0; qi < pdev->grp_queues; qi++) {
            hdl = &pdev->ctrl.grp[gi].intr_hdl[qi];
            netif_napi_del((struct napi_struct *)hdl->priv);
            priv_hdl[hdl->unit][hdl->chan].hdl = NULL;
            if (pdev->flags & PDMA_GROUP_INTR) {
                break;
            }
        }
    }

    /* Deinitialize PDMA device */
    bcmcnet_pdma_dev_deinit(pdev);

    /* Detach PDMA driver */
    rv = drv_ops[pdev->dev_type]->drv_detach(pdev);
    if (SHR_FAILURE(rv)) {
        DBG_WARN(("Detach DMA driver failed.\n"));
    }

    dev->flags &= ~NGKNET_DEV_ACTIVE;

    return rv;
}

/*!
 * \brief Create network interface.
 *
 * Suspend device firstly, destroy all virtual network devices
 * and filters, deinitialize Packet DMA device.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_netif_create(struct ngknet_dev *dev, ngknet_netif_t *netif)
{
    struct net_device *ndev = NULL;
    struct ngknet_private *priv = NULL;
    unsigned long flags;
    int num, id;
    int rv;

    switch (netif->type) {
    case NGKNET_NETIF_T_VLAN:
    case NGKNET_NETIF_T_PORT:
    case NGKNET_NETIF_T_META:
        break;
    default:
        return SHR_E_UNAVAIL;
    }

    /* Get vitual network device name */
    if (netif->name[0] == '\0') {
        /* Reserve 6 vacancies for base&vitual device number, i.e. nameAB_XYZ */
        if (strlen(base_dev_name) < IFNAMSIZ - 6) {
            snprintf(netif->name, IFNAMSIZ, "%s%d%s", base_dev_name, dev->dev_no, "_");
            strncat(netif->name, "%d", 2);
        } else {
            DBG_WARN(("Too long network device name: %s.\n", base_dev_name));
            return SHR_E_PARAM;
        }
    }

    rv = ngknet_ndev_init(netif, &ndev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    spin_lock_irqsave(&dev->lock, flags);

    num = (long)dev->vdev[0];
    for (id = 1; id < num + 1; id++) {
        if (!dev->vdev[id]) {
            break;
        }
    }
    if (id >= NUM_VDEV_MAX) {
        spin_unlock_irqrestore(&dev->lock, flags);
        unregister_netdev(ndev);
        free_netdev(ndev);
        return SHR_E_RESOURCE;
    }

    dev->vdev[id] = ndev;
    num += id == (num + 1) ? 1 : 0;
    dev->vdev[0] = (struct net_device *)(long)num;

    spin_unlock_irqrestore(&dev->lock, flags);

    priv = netdev_priv(ndev);
    priv->net_dev = ndev;
    priv->bkn_dev = dev;
    priv->id = id;
    priv->type = netif->type;
    if (priv->type == NGKNET_NETIF_T_PORT) {
        priv->meta_len = netif->meta_len;
        memcpy(priv->meta_data, netif->meta_data, priv->meta_len);
    }
    priv->flags = netif->flags;
    priv->vlan = netif->vlan;
    priv->chan = netif->chan;

    netif->id = priv->id;
    memcpy(netif->macaddr, ndev->dev_addr, ETH_ALEN);
    netif->mtu = ndev->mtu;
    memcpy(netif->name, ndev->name, NGKNET_NETIF_NAME_MAX - 1);

    if (priv->flags & NGKNET_NETIF_F_BIND_CHAN) {
        dev->bdev[priv->chan] = ndev;
    }

    DBG_VERB(("Created virtual network device %s (%d).\n", ndev->name, priv->id));

    return SHR_E_NONE;
}

/*!
 * \brief Destroy network interface.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [in] id Network interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_netif_destroy(struct ngknet_dev *dev, int id)
{
    struct net_device *ndev = NULL;
    struct ngknet_private *priv = NULL;
    unsigned long flags;
    int num;
    DECLARE_WAITQUEUE(wait, current);

    if (id <= 0 || id >= NUM_VDEV_MAX) {
        return SHR_E_PARAM;
    }

    spin_lock_irqsave(&dev->lock, flags);

    ndev = dev->vdev[id];
    if (!ndev) {
        spin_unlock_irqrestore(&dev->lock, flags);
        return SHR_E_NOT_FOUND;
    }
    priv = netdev_priv(ndev);

    add_wait_queue(&dev->wq, &wait);

    while (priv->users) {
        priv->wait = 1;
        set_current_state(TASK_INTERRUPTIBLE);
        spin_unlock_irqrestore(&dev->lock, flags);
        schedule();
        spin_lock_irqsave(&dev->lock, flags);
        priv->wait = 0;
        set_current_state(TASK_RUNNING);
    }

    if (priv->flags & NGKNET_NETIF_F_BIND_CHAN) {
        dev->bdev[priv->chan] = NULL;
    }

    dev->vdev[id] = NULL;
    num = (long)dev->vdev[0];
    while (num-- == id--) {
        if (dev->vdev[id]) {
            dev->vdev[0] = (struct net_device *)(long)num;
            break;
        }
    }

    spin_unlock_irqrestore(&dev->lock, flags);

    remove_wait_queue(&dev->wq, &wait);

    DBG_VERB(("Removing virtual network device %s (%d).\n", ndev->name, priv->id));

    unregister_netdev(ndev);
    free_netdev(ndev);

    return SHR_E_NONE;
}

/*!
 * \brief Get network interface.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [in] id Network interface ID.
 * \param [out] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_netif_get(struct ngknet_dev *dev, int id, ngknet_netif_t *netif)
{
    struct net_device *ndev = NULL;
    struct ngknet_private *priv = NULL;
    unsigned long flags;
    int num;

    if (id < 0 || id >= NUM_VDEV_MAX) {
        return SHR_E_PARAM;
    }

    spin_lock_irqsave(&dev->lock, flags);

    ndev = id == 0 ? dev->net_dev : dev->vdev[id];
    if (!ndev) {
        spin_unlock_irqrestore(&dev->lock, flags);
        return SHR_E_NOT_FOUND;
    }

    priv = netdev_priv(ndev);
    netif->id = priv->id;
    netif->type = priv->type;
    netif->flags = priv->flags;
    netif->vlan = priv->vlan;
    memcpy(netif->macaddr, priv->net_dev->dev_addr, ETH_ALEN);
    netif->mtu = priv->net_dev->mtu;
    netif->chan = priv->chan;
    memcpy(netif->name, priv->net_dev->name, NGKNET_NETIF_NAME_MAX - 1);
    netif->meta_len = priv->meta_len;
    memcpy(netif->meta_data, priv->meta_data, netif->meta_len);

    num = (long)dev->vdev[0];
    for (id++; id < num + 1; id++) {
        if (dev->vdev[id]) {
            break;
        }
    }
    netif->next = id == (num + 1) ? 0 : id;

    spin_unlock_irqrestore(&dev->lock, flags);

    DBG_VERB(("Got virtual network device %s (%d).\n", ndev->name, priv->id));

    return SHR_E_NONE;
}

/*!
 * \brief Get the next network interface.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [out] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
static int
ngknet_netif_get_next(struct ngknet_dev *dev, ngknet_netif_t *netif)
{
    return ngknet_netif_get(dev, netif->next, netif);
}

/*!
 * System control interfaces
 */

int
ngknet_debug_level_get(void)
{
    return debug;
}

void
ngknet_debug_level_set(int debug_level)
{
    debug = debug_level;
}

int
ngknet_rx_rate_limit_get(void)
{
    return rx_rate_limit;
}

void
ngknet_rx_rate_limit_set(int rate_limit)
{
    rx_rate_limit = rate_limit;
}

/*!
 * Generic module functions
 */

static int
ngknet_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int
ngknet_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static long
ngknet_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct ngknet_ioctl ioc;
    struct ngknet_dev *dev = NULL;
    struct pdma_dev *pdev = NULL;
    ngknet_dev_cfg_t dev_cfg;
    ngknet_chan_cfg_t chan_cfg;
    ngknet_netif_t netif;
    ngknet_filter_t filter;
    int dt, gi, qi;

    if (_IOC_TYPE(cmd) != NGKNET_IOC_MAGIC) {
        DBG_WARN(("Unsupported command (cmd=%d)\n", cmd));
        return -EINVAL;
    }

    if (copy_from_user(&ioc, (void *)arg, sizeof(ioc))) {
        return -EFAULT;
    }

    ioc.rc = SHR_E_NONE;

    dev = &ngknet_devices[ioc.unit];
    pdev = &dev->pdma_dev;

    if (cmd != NGKNET_VERSION_GET &&
        cmd != NGKNET_RX_RATE_LIMIT &&
        cmd != NGKNET_DEV_INIT &&
        !(dev->flags & NGKNET_DEV_ACTIVE)) {
        ioc.rc = SHR_E_UNAVAIL;
        if (copy_to_user((void *)arg, &ioc, sizeof(ioc))) {
            return -EFAULT;
        }
        return 0;
    }

    switch (cmd) {
    case NGKNET_VERSION_GET:
        DBG_CMD(("NGKNET_VERSION_GET\n"));
        ioc.op.info.version = NGKNET_IOC_VERSION;
        break;
    case NGKNET_RX_RATE_LIMIT:
        DBG_CMD(("NGKNET_RX_RATE_LIMIT\n"));
        if (ioc.iarg[0]) {
            ngknet_rx_rate_limit_set(ioc.iarg[1]);
        } else {
            ioc.iarg[1] = ngknet_rx_rate_limit_get();
        }
        break;
    case NGKNET_DEV_INIT:
        DBG_CMD(("NGKNET_DEV_INIT\n"));
        if (dev->flags & NGKNET_DEV_ACTIVE) {
            strncpy(dev_cfg.name, pdev->name, sizeof(dev_cfg.name) - 1);
            dev_cfg.dev_id = pdev->dev_id;
            dev_cfg.nb_grp = pdev->ctrl.nb_grp;
            dev_cfg.bm_grp = pdev->ctrl.bm_grp;
            ioc.rc = ngknet_netif_get(dev, 0, &dev_cfg.base_netif);
            if (SHR_FAILURE((int)ioc.rc)) {
                break;
            }
            if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &dev_cfg,
                             ioc.op.data.len)) {
                return -EFAULT;
            }
            break;
        }
        if (copy_from_user(&dev_cfg, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        if (!dev_cfg.name[0] || !dev_cfg.nb_grp || !dev_cfg.bm_grp ||
            dev_cfg.nb_grp > NUM_GRP_MAX || dev_cfg.bm_grp >= (1 << NUM_GRP_MAX)) {
            ioc.rc = SHR_E_PARAM;
            break;
        }
        memset(pdev, 0, sizeof(*pdev));
        strncpy(pdev->name, dev_cfg.name, sizeof(pdev->name) - 1);
        pdev->dev_id = dev_cfg.dev_id;
        for (dt = 0; dt < drv_num; dt++) {
            if (!drv_ops[dt]) {
                continue;
            }
            if (!strcasecmp(dev_cfg.type_str, drv_ops[dt]->drv_desc)) {
                pdev->dev_type = dt;
                break;
            }
        }
        if (pdev->dev_type <= NGKNET_DEV_T_NONE ||
            pdev->dev_type >= NGKNET_DEV_T_COUNT) {
            ioc.rc = SHR_E_PARAM;
            break;
        }
        pdev->ctrl.nb_grp = dev_cfg.nb_grp;
        pdev->ctrl.bm_grp = dev_cfg.bm_grp;
        for (gi = 0; gi < NUM_GRP_MAX; gi++) {
            if (1 << gi & dev_cfg.bm_grp) {
                pdev->ctrl.grp[gi].attached = 1;
                pdev->num_groups = gi + 1;
            }
        }
        ioc.rc = ngknet_dev_probe(ioc.unit, &dev_cfg.base_netif);
        if (SHR_FAILURE((int)ioc.rc)) {
            break;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &dev_cfg,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_DEV_DEINIT:
        DBG_CMD(("NGKNET_DEV_DEINIT\n"));
        if (dev->flags & NGKNET_DEV_ACTIVE) {
            ioc.rc = ngknet_dev_remove(ioc.unit);
        }
        break;
    case NGKNET_QUEUE_CONFIG:
        DBG_CMD(("NGKNET_QUEUE_CONFIG\n"));
        if (copy_from_user(&chan_cfg, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        gi = chan_cfg.chan / pdev->grp_queues;
        if (!(1 << gi & pdev->ctrl.bm_grp) ||
            (1 << chan_cfg.chan & pdev->ctrl.bm_rxq) ||
            (1 << chan_cfg.chan & pdev->ctrl.bm_txq)) {
            ioc.rc = SHR_E_PARAM;
            break;
        }
        if (chan_cfg.rx_buf_size > pdev->ctrl.rx_buf_size) {
            ioc.rc = SHR_E_PARAM;
            break;
        }
        if (chan_cfg.dir == PDMA_Q_RX) {
            pdev->ctrl.bm_rxq |= 1 << chan_cfg.chan;
            pdev->ctrl.nb_rxq++;
        } else {
            pdev->ctrl.bm_txq |= 1 << chan_cfg.chan;
            pdev->ctrl.nb_txq++;
        }
        qi = chan_cfg.chan % pdev->grp_queues;
        pdev->ctrl.grp[gi].nb_desc[qi] = chan_cfg.nb_desc;
        pdev->ctrl.grp[gi].rx_size[qi] = chan_cfg.rx_buf_size;
        if (chan_cfg.chan_ctrl & NGKNET_PKT_BYTE_SWAP) {
            pdev->ctrl.grp[gi].que_ctrl[qi] |= PDMA_PKT_BYTE_SWAP;
        }
        if (chan_cfg.chan_ctrl & NGKNET_OTH_BYTE_SWAP) {
            pdev->ctrl.grp[gi].que_ctrl[qi] |= PDMA_OTH_BYTE_SWAP;
        }
        if (chan_cfg.chan_ctrl & NGKNET_HDR_BYTE_SWAP) {
            pdev->ctrl.grp[gi].que_ctrl[qi] |= PDMA_HDR_BYTE_SWAP;
        }
        break;
    case NGKNET_QUEUE_QUERY:
        DBG_CMD(("NGKNET_QUEUE_QUERY\n"));
        if (copy_from_user(&chan_cfg, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        if (1 << chan_cfg.chan & pdev->ctrl.bm_rxq) {
            chan_cfg.dir = PDMA_Q_RX;
        } else if (1 << chan_cfg.chan & pdev->ctrl.bm_txq) {
            chan_cfg.dir = PDMA_Q_TX;
        } else {
            ioc.rc = SHR_E_UNAVAIL;
            break;
        }
        gi = chan_cfg.chan / pdev->grp_queues;
        qi = chan_cfg.chan % pdev->grp_queues;
        chan_cfg.nb_desc = pdev->ctrl.grp[gi].nb_desc[qi];
        chan_cfg.chan_ctrl = pdev->ctrl.grp[gi].que_ctrl[qi];
        if (chan_cfg.dir == PDMA_Q_RX) {
            chan_cfg.rx_buf_size = pdev->ctrl.grp[gi].rx_size[qi];
        } else {
            chan_cfg.rx_buf_size = 0;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &chan_cfg,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_DEV_SUSPEND:
        DBG_CMD(("NGKNET_DEV_SUSPEND\n"));
        if (rx_rate_limit >= 0) {
            ngknet_rx_rate_limit_stop(dev);
        }
        ioc.rc = bcmcnet_pdma_dev_suspend(pdev);
        break;
    case NGKNET_DEV_RESUME:
        DBG_CMD(("NGKNET_DEV_RESUME\n"));
        ioc.rc = bcmcnet_pdma_dev_resume(pdev);
        if (rx_rate_limit >= 0) {
            ngknet_rx_rate_limit_start(dev);
        }
        break;
    case NGKNET_RCPU_CONFIG:
        DBG_CMD(("NGKNET_RCPU_CONFIG\n"));
        if (copy_from_user(&dev->rcpu_ctrl, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_RCPU_GET:
        DBG_CMD(("NGKNET_RCPU_GET\n"));
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &dev->rcpu_ctrl,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_INFO_GET:
        DBG_CMD(("NGKNET_INFO_GET\n"));
        bcmcnet_pdma_dev_info_get(pdev);
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &pdev->info,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_STATS_GET:
        DBG_CMD(("NGKNET_STATS_GET\n"));
        bcmcnet_pdma_dev_stats_get(pdev);
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &pdev->stats,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_STATS_RESET:
        DBG_CMD(("NGKNET_STATS_RESET\n"));
        bcmcnet_pdma_dev_stats_reset(pdev);
        break;
    case NGKNET_NETIF_CREATE:
        DBG_CMD(("NGKNET_NETIF_CREATE\n"));
        if (copy_from_user(&netif, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        ioc.rc = ngknet_netif_create(dev, &netif);
        if (SHR_FAILURE((int)ioc.rc)) {
            break;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &netif,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_NETIF_DESTROY:
        DBG_CMD(("NGKNET_NETIF_DESTROY\n"));
        ioc.rc = ngknet_netif_destroy(dev, ioc.iarg[0]);
        break;
    case NGKNET_NETIF_GET:
        DBG_CMD(("NGKNET_NETIF_GET\n"));
        ioc.rc = ngknet_netif_get(dev, ioc.iarg[0], &netif);
        if (SHR_FAILURE((int)ioc.rc)) {
            break;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &netif,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_NETIF_NEXT:
        DBG_CMD(("NGKNET_NETIF_NEXT\n"));
        if (copy_from_user(&netif, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        ioc.rc = ngknet_netif_get_next(dev, &netif);
        if (SHR_FAILURE((int)ioc.rc)) {
            break;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &netif,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_FILT_CREATE:
        DBG_CMD(("NGKNET_FILT_CREATE\n"));
        if (copy_from_user(&filter, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        ioc.rc = ngknet_filter_create(dev, &filter);
        if (SHR_FAILURE((int)ioc.rc)) {
            break;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &filter,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_FILT_DESTROY:
        DBG_CMD(("NGKNET_FILT_DESTROY\n"));
        ioc.rc = ngknet_filter_destroy(dev, ioc.iarg[0]);
        break;
    case NGKNET_FILT_GET:
        DBG_CMD(("NGKNET_FILT_GET\n"));
        ioc.rc = ngknet_filter_get(dev, ioc.iarg[0], &filter);
        if (SHR_FAILURE((int)ioc.rc)) {
            break;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &filter,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_FILT_NEXT:
        DBG_CMD(("NGKNET_FILT_NEXT\n"));
        if (copy_from_user(&filter, (void *)(unsigned long)ioc.op.data.buf,
                           ioc.op.data.len)) {
            return -EFAULT;
        }
        ioc.rc = ngknet_filter_get_next(dev, &filter);
        if (SHR_FAILURE((int)ioc.rc)) {
            break;
        }
        if (copy_to_user((void *)(unsigned long)ioc.op.data.buf, &filter,
                         ioc.op.data.len)) {
            return -EFAULT;
        }
        break;
    case NGKNET_FILT_PN_SET:
        DBG_CMD(("NGKNET_FILT_PN_SET\n"));
        dev->num_rx_prio = ioc.iarg[0];
        break;
    default:
        ioc.rc = SHR_E_UNAVAIL;
        printk("Invalid IOCTL");
        break;
    }

    if (copy_to_user((void *)arg, &ioc, sizeof(ioc))) {
        return -EFAULT;
    }

    return 0;
}

static int
ngknet_mmap(struct file *filp, struct vm_area_struct *vma)
{
    return 0;
}

static struct file_operations ngknet_fops = {
    .open = ngknet_open,
    .release = ngknet_release,
    .unlocked_ioctl = ngknet_ioctl,
    .compat_ioctl = ngknet_ioctl,
    .mmap = ngknet_mmap,
};

static int __init
ngknet_init_module(void)
{
    int idx;
    int rv;

    rv = register_chrdev(NGKNET_MODULE_MAJOR, NGKNET_MODULE_NAME, &ngknet_fops);
    if (rv < 0) {
        printk(KERN_WARNING "%s: can't get major %d\n",
               NGKNET_MODULE_NAME, NGKNET_MODULE_MAJOR);
        return rv;
    }

    /* Randomize lower 3 bytes of the MAC address (TESTING ONLY) */
    get_random_bytes(&ngknet_dev_mac[3], 3);

    /* Check for user-supplied MAC address (recommended) */
    if (mac_addr != NULL && strlen(mac_addr) == 17) {
        for (idx = 0; idx < 6; idx++) {
            ngknet_dev_mac[idx] = simple_strtoul(&mac_addr[idx * 3], NULL, 16);
        }
        /* Do not allow multicast address */
        ngknet_dev_mac[0] &= ~0x01;
    }

    /* Initialize procfs */
    ngknet_procfs_init();

    /* Initialize Rx rate limit */
    ngknet_rx_rate_limit_init(ngknet_devices);

    return 0;
}

static void __exit
ngknet_exit_module(void)
{
    int idx;

    /* Cleanup Rx rate limit */
    ngknet_rx_rate_limit_cleanup();

    /* Cleanup procfs */
    ngknet_procfs_cleanup();

    /* Remove all the devices */
    for (idx = 0; idx < NUM_PDMA_DEV_MAX; idx++) {
        ngknet_dev_remove(idx);
    }

    unregister_chrdev(NGKNET_MODULE_MAJOR, NGKNET_MODULE_NAME);
}

module_init(ngknet_init_module);
module_exit(ngknet_exit_module);

