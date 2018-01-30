/*! \file bcmcnet_main.c
 *
 * BCMCNET APIs.
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

#include <bsl/bsl.h>
#include <bcmcnet/bcmcnet_main.h>
#include <bcmcnet/bcmcnet_core.h>
#include "bcmcnet_intr.h"

#define BSL_LOG_MODULE  BSL_LS_BCMBD_DMA

typedef int (*drv_ops_attach)(struct pdma_dev *dev);
typedef int (*drv_ops_intr_enable)(int unit, int cmc, int inum);
typedef int (*drv_ops_intr_connect)(int unit, int cmc, int inum,
                                    bcmcnet_intr_f ifunc, uint32_t idata);

struct bcmcnet_drv_ops {
    const char             *drv_desc;
    drv_ops_attach          drv_attach;
    drv_ops_attach          drv_detach;
    drv_ops_intr_enable     intr_enable;
    drv_ops_intr_enable     intr_disable;
    drv_ops_intr_connect    intr_connect;
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    struct bcmcnet_drv_ops _bd##_cnet_drv_ops = { \
        #_bd, \
        _bd##_cnet_pdma_attach, \
        _bd##_cnet_pdma_detach, \
        _bd##_cnet_pdma_intr_enable, \
        _bd##_cnet_pdma_intr_disable, \
        _bd##_cnet_pdma_intr_connect, \
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

static struct pdma_dev pdma_devices[NUM_PDMA_DEV_MAX];
static struct bcmcnet_rx_cbs_s rx_cbs[NUM_RX_CB_MAX];

/*!
 * Dump packet content for debug
 */
static void
bcmcnet_pkt_dump(uint8_t *data, int len)
{
    char str[128];
    int i;

    len = len > 256 ? 256 : len;

    for (i = 0; i < len; i++) {
        if ((i & 0x1f) == 0) {
            sal_sprintf(str, "%04x: ", i);
        }
        sal_sprintf(&str[sal_strlen(str)], "%02x", data[i]);
        if ((i & 0x1f) == 0x1f) {
            sal_sprintf(&str[sal_strlen(str)], "\n");
            cli_out("%s", str);
            continue;
        }
        if ((i & 0x3) == 0x3) {
            sal_sprintf(&str[sal_strlen(str)], " ");
        }
    }
    if ((i & 0x1f) != 0) {
        sal_sprintf(&str[sal_strlen(str)], "\n");
        cli_out("%s", str);
    }
    cli_out("\n");
}

/*!
 * Read 32-bit device register
 */
static int
bcmcnet_dev_read32(struct pdma_dev *dev, uint32_t addr, uint32_t *data)
{
    return BCMDRD_DEV_READ32(dev->unit, addr, data);
}

/*!
 * Write 32-bit device register
 */
static int
bcmcnet_dev_write32(struct pdma_dev *dev, uint32_t addr, uint32_t data)
{
    return BCMDRD_DEV_WRITE32(dev->unit, addr, data);
}

/*!
 * Process a incoming packet
 */
static int
bcmcnet_rx_pkt_process(struct pdma_dev *dev, bcmpkt_data_buf_t *buf)
{
    struct bcmcnet_rcpu_hdr *rch = (struct bcmcnet_rcpu_hdr *)buf->data;
    struct pkt_buf *pkb = (struct pkt_buf *)buf->data;
    uint16_t data_len = pkb->pkh.data_len - ETH_FCS_LEN;
    unsigned char *rcpu_hdr = (unsigned char *)rch;

    sal_memcpy(rcpu_hdr, &pkb->data + pkb->pkh.meta_len, 12);
    rcpu_hdr[12] = 0x81;
    rcpu_hdr[13] = 0x00;
    rcpu_hdr[14] = 0x00;
    rcpu_hdr[15] = 0x01;
    rcpu_hdr[16] = 0xde;
    rcpu_hdr[17] = 0x08;
    rcpu_hdr[18] = dev->dev_id >> 8;
    rcpu_hdr[19] = dev->dev_id & 0xff;
    rcpu_hdr[20] = RCPU_OPCODE_RX;
    rcpu_hdr[21] = RCPU_FLAG_MODHDR;
    rcpu_hdr[24] = data_len >> 8;
    rcpu_hdr[25] = data_len & 0xff;

    buf->data_len -= ETH_FCS_LEN;

    return SHR_E_NONE;
}

/*!
 * Process a outgoing packet
 */
static int
bcmcnet_tx_pkt_process(struct pdma_dev *dev, bcmpkt_data_buf_t *buf)
{
    struct bcmcnet_rcpu_hdr *rch = (struct bcmcnet_rcpu_hdr *)buf->data;
    struct pkt_hdr *pkh = (struct pkt_hdr *)buf->data;
    unsigned char *rcpu_hdr = (unsigned char *)rch;

    pkh->data_len = (rcpu_hdr[24] << 8 | rcpu_hdr[25]) + ETH_FCS_LEN;
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

    return SHR_E_NONE;
}

/*!
 * Handle a incoming packet
 */
static int
bcmcnet_pkt_recv(struct pdma_dev *dev, int queue, void *buf)
{
    struct bcmcnet_rx_cbs_s *cbs;
    struct pkt_hdr *pkh = NULL;
    uint32_t len;
    int ci;
    int rv;

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pkh = (struct pkt_hdr *)((bcmpkt_data_buf_t *)buf)->data;
        len = pkh->meta_len + pkh->data_len;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(dev->unit, "Rx packet (%d bytes).\n"), len));
        bcmcnet_pkt_dump((uint8_t *)pkh + PKT_HDR_SIZE, len);
    }

    bcmcnet_rx_pkt_process(dev, (bcmpkt_data_buf_t *)buf);

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        len = ((bcmpkt_data_buf_t *)buf)->data_len;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(dev->unit, "Packet sent up (%d bytes).\n"), len));
        bcmcnet_pkt_dump((uint8_t *)pkh, len);
    }

    for (ci = 0; ci < NUM_RX_CB_MAX; ci++) {
        cbs = &rx_cbs[ci];
        if (cbs->cb != NULL) {
            rv = cbs->cb(dev->unit, queue, (bcmpkt_data_buf_t *)buf, cbs->ck);
            if (rv == 0) {
                return SHR_E_NONE;
            }
        }
    }

    return SHR_E_FAIL;
}

/*!
 * Enable a set of interrupts
 */
static void
bcmcnet_sys_intr_enable(struct pdma_dev *dev, int cmc, int chan,
                        uint32_t reg, uint32_t mask)
{
    int intr_num = dev->ctrl.grp[cmc].intr_hdl[chan].intr_num;

    drv_ops[dev->dev_type]->intr_enable(dev->unit, cmc, intr_num);
}

/*!
 * Disable a set of interrupts
 */
static void
bcmcnet_sys_intr_disable(struct pdma_dev *dev, int cmc, int chan,
                         uint32_t reg, uint32_t mask)
{
    int intr_num = dev->ctrl.grp[cmc].intr_hdl[chan].intr_num;

    drv_ops[dev->dev_type]->intr_disable(dev->unit, cmc, intr_num);
}

/*!
 * Polling handler
 */
static int
bcmcnet_dev_poll(struct intr_handle *hdl, void *ck)
{
    struct pdma_dev *dev = (struct pdma_dev *)ck;
    int work_done;

    work_done = bcmcnet_queue_poll(dev, hdl, hdl->budget);
    if (work_done < hdl->budget) {
        bcmcnet_poll_complete(hdl);
        bcmcnet_queue_intr_enable(dev, hdl);
    }

    return work_done;
}

/*!
 * Interrupt handler
 */
static void
bcmcnet_dev_isr(int unit, uint32_t data)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    struct intr_handle *hdl;
    int chan = data;

    hdl = &dev->ctrl.grp[chan / dev->grp_queues].intr_hdl[chan % dev->grp_queues];
    if (hdl->chan != chan) {
        return;
    }

    dev->stats.intrs++;

    bcmcnet_queue_intr_disable(dev, hdl);
    bcmcnet_poll_schedule(hdl);
}

/*!
 * Initialize a device
 */
int
bcmcnet_dev_init(int unit, bcmcnet_dev_cfg_t *dev_cfg)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    struct dev_ctrl *ctrl = &dev->ctrl;
    int dt, gi;
    int rv;

    if (!bcmdrd_dev_exists(unit)) {
        return SHR_E_ACCESS;
    }

    if (!dev_cfg->name[0] || !dev_cfg->nb_grp || !dev_cfg->bm_grp ||
        dev_cfg->nb_grp > NUM_GRP_MAX || dev_cfg->bm_grp >= (1 << NUM_GRP_MAX)) {
        return SHR_E_PARAM;
    }

    if (dev->attached) {
        return SHR_E_NONE;
    }

    /* Set up the device data */
    sal_memset(dev, 0, sizeof(*dev));
    sal_strncpy(dev->name, dev_cfg->name, sizeof(dev->name) - 1);
    dev->dev_id = dev_cfg->dev_id;
    for (dt = 0; dt < drv_num; dt++) {
        if (!drv_ops[dt]) {
            continue;
        }
        if (!sal_strcasecmp(dev_cfg->type_str, drv_ops[dt]->drv_desc)) {
            dev->dev_type = dt;
            break;
        }
    }
    if (dev->dev_type <= BCMDRD_DEV_T_NONE ||
        dev->dev_type >= BCMDRD_DEV_T_COUNT) {
        return SHR_E_PARAM;
    }
    dev->unit = unit;
    dev->ctrl.nb_grp = dev_cfg->nb_grp;
    dev->ctrl.bm_grp = dev_cfg->bm_grp;
    for (gi = 0; gi < NUM_GRP_MAX; gi++) {
        if ((1 << gi) & dev_cfg->bm_grp) {
            dev->ctrl.grp[gi].attached = 1;
            dev->num_groups = gi + 1;
        }
    }

    ctrl->dev = dev;
    ctrl->rx_buf_size = RX_BUF_SIZE_DFLT;

    dev->dev_read32 = bcmcnet_dev_read32;
    dev->dev_write32 = bcmcnet_dev_write32;
    dev->pkt_recv = bcmcnet_pkt_recv;
    dev->intr_unmask = bcmcnet_sys_intr_enable;
    dev->intr_mask = bcmcnet_sys_intr_disable;

    dev->flags |= PDMA_TX_POLLING;
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
        dev->flags |= PDMA_CHAIN_MODE;
    }

    /* Device driver attach */
    rv = drv_ops[dev->dev_type]->drv_attach(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Device initialization */
    rv = bcmcnet_pdma_dev_init(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Interrupt control initialization */
    rv = bcmcnet_intr_ctrl_init(dev);
    if (SHR_FAILURE(rv)) {
        bcmcnet_pdma_dev_deinit(dev);
        return rv;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET device initialize\n")));

    return SHR_E_NONE;
}

/*!
 * Clean up a device
 */
int
bcmcnet_dev_cleanup(int unit)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    /* Device cleanup */
    rv = bcmcnet_pdma_dev_deinit(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Interrupt control cleanup */
    bcmcnet_intr_ctrl_deinit(dev);

    /* Device driver detach */
    drv_ops[dev->dev_type]->drv_detach(dev);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET device clean up\n")));

    return SHR_E_NONE;
}

/*!
 * Configure a channel
 */
int
bcmcnet_dev_chan_config(int unit, bcmcnet_chan_cfg_t *chan_cfg)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    int gi, qi;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    gi = chan_cfg->chan / dev->grp_queues;
    if (!(1 << gi & dev->ctrl.bm_grp) ||
        (1 << chan_cfg->chan & dev->ctrl.bm_rxq) ||
        (1 << chan_cfg->chan & dev->ctrl.bm_txq)) {
        return SHR_E_PARAM;
    }

    if (chan_cfg->rx_buf_size > dev->ctrl.rx_buf_size) {
        return SHR_E_PARAM;
    }

    if (chan_cfg->dir == PDMA_Q_RX) {
        dev->ctrl.bm_rxq |= 1 << chan_cfg->chan;
        dev->ctrl.nb_rxq++;
    } else {
        dev->ctrl.bm_txq |= 1 << chan_cfg->chan;
        dev->ctrl.nb_txq++;
    }
    qi = chan_cfg->chan % dev->grp_queues;
    dev->ctrl.grp[gi].nb_desc[qi] = chan_cfg->nb_desc;
    dev->ctrl.grp[gi].rx_size[qi] = chan_cfg->rx_buf_size;
    if (chan_cfg->chan_ctrl & BCMCNET_PKT_BYTE_SWAP) {
        dev->ctrl.grp[gi].que_ctrl[qi] |= PDMA_PKT_BYTE_SWAP;
    }
    if (chan_cfg->chan_ctrl & BCMCNET_OTH_BYTE_SWAP) {
        dev->ctrl.grp[gi].que_ctrl[qi] |= PDMA_OTH_BYTE_SWAP;
    }
    if (chan_cfg->chan_ctrl & BCMCNET_HDR_BYTE_SWAP) {
        dev->ctrl.grp[gi].que_ctrl[qi] |= PDMA_HDR_BYTE_SWAP;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET queue configure\n")));

    return SHR_E_NONE;
}

/*!
 * Query a channel
 */
int
bcmcnet_dev_chan_query(int unit, bcmcnet_chan_cfg_t *chan_cfg)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    int gi, qi;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    if (1 << chan_cfg->chan & dev->ctrl.bm_rxq) {
        chan_cfg->dir = PDMA_Q_RX;
    } else if (1 << chan_cfg->chan & dev->ctrl.bm_txq) {
        chan_cfg->dir = PDMA_Q_TX;
    } else {
        return SHR_E_UNAVAIL;
    }

    gi = chan_cfg->chan / dev->grp_queues;
    qi = chan_cfg->chan % dev->grp_queues;

    chan_cfg->nb_desc = dev->ctrl.grp[gi].nb_desc[qi];
    chan_cfg->chan_ctrl = dev->ctrl.grp[gi].que_ctrl[qi];

    if (chan_cfg->dir == PDMA_Q_RX) {
        chan_cfg->rx_buf_size = dev->ctrl.grp[gi].rx_size[qi];
    } else {
        chan_cfg->rx_buf_size = 0;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET queue query\n")));

    return SHR_E_NONE;
}

/*!
 * Start a device
 */
int
bcmcnet_dev_start(int unit)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct intr_handle *hdl = NULL;
    int chan, gi, qi;
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->bm_rxq || !ctrl->bm_txq) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Not config Rx or Tx queue yet!\n")));
        return SHR_E_CONFIG;
    }

    for (chan = 0; chan < dev->num_queues; chan++) {
        if (1 << chan & (ctrl->bm_rxq | ctrl->bm_txq)) {
            gi = chan / dev->grp_queues;
            qi = chan % dev->grp_queues;
            hdl = &ctrl->grp[gi].intr_hdl[qi];
            bcmcnet_intr_cb_register(hdl, bcmcnet_dev_poll, (void *)dev);
            bcmcnet_poll_enable(hdl);
            drv_ops[dev->dev_type]->intr_connect(dev->unit, gi, hdl->intr_num,
                                                 bcmcnet_dev_isr, chan);
        }
    }

    rv = bcmcnet_pdma_dev_start(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET device start\n")));

    return SHR_E_NONE;
}

/*!
 * Stop a device
 */
int
bcmcnet_dev_stop(int unit)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct intr_handle *hdl = NULL;
    int chan, gi, qi;
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    bcmcnet_pdma_dev_suspend(dev);

    for (chan = 0; chan < dev->num_queues; chan++) {
        if (1 << chan & (ctrl->bm_rxq | ctrl->bm_txq)) {
            gi = chan / dev->grp_queues;
            qi = chan % dev->grp_queues;
            hdl = &ctrl->grp[gi].intr_hdl[qi];
            bcmcnet_poll_disable(hdl);
            while (bcmcnet_poll_check(hdl)) {
                sal_usleep(1);
            }
        }
    }

    rv = bcmcnet_pdma_dev_stop(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    for (chan = 0; chan < dev->num_queues; chan++) {
        if (1 << chan & (ctrl->bm_rxq | ctrl->bm_txq)) {
            gi = chan / dev->grp_queues;
            qi = chan % dev->grp_queues;
            hdl = &ctrl->grp[gi].intr_hdl[qi];
            bcmcnet_intr_cb_unregister(hdl, bcmcnet_dev_poll, (void *)dev);
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET device stop\n")));

    return SHR_E_NONE;
}

/*!
 * Suspend a device
 */
int
bcmcnet_dev_suspend(int unit)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    rv = bcmcnet_pdma_dev_suspend(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET device suspend\n")));

    return SHR_E_NONE;
}

/*!
 * Resume a device
 */
int
bcmcnet_dev_resume(int unit)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    rv = bcmcnet_pdma_dev_resume(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(dev->unit, "CNET device resume\n")));

    return SHR_E_NONE;
}

/*!
 * Register a Rx callback
 */
int
bcmcnet_rx_cb_register(int unit, bcmcnet_rx_cb_t cb, void *ck)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    struct bcmcnet_rx_cbs_s *cbs;
    int ci;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    for (ci = 0; ci < NUM_RX_CB_MAX; ci++) {
        cbs = &rx_cbs[ci];
        if (cbs->cb == NULL) {
            cbs->cb = cb;
            cbs->ck = ck;
            return SHR_E_NONE;
        }
    }

    return SHR_E_RESOURCE;
}

/*!
 * Unregister a Rx callback
 */
int
bcmcnet_rx_cb_unregister(int unit, bcmcnet_rx_cb_t cb, void *ck)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    struct bcmcnet_rx_cbs_s *cbs;
    int ci;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    for (ci = 0; ci < NUM_RX_CB_MAX; ci++) {
        cbs = &rx_cbs[ci];
        if (cbs->cb == cb && cbs->ck == ck) {
            cbs->cb = NULL;
            cbs->ck = NULL;
        }
    }

    return SHR_E_NONE;
}

/*!
 * Send a packet on a Tx queue
 */
int
bcmcnet_tx(int unit, int queue, bcmpkt_data_buf_t *buf)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    struct dev_ctrl *ctrl = &dev->ctrl;
    struct pkt_hdr *pkh = NULL;
    uint32_t len;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    if (queue < 0 || queue > (int)ctrl->nb_txq || buf == NULL) {
        return SHR_E_PARAM;
    }

    if (!dev->pkt_xmit) {
        return SHR_E_INTERNAL;
    }

    pkh = (struct pkt_hdr *)buf->data;

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        len = buf->data_len;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(dev->unit, "Packet got from (%d bytes).\n"), len));
        bcmcnet_pkt_dump((uint8_t *)pkh, len);
    }

    bcmcnet_tx_pkt_process(dev, buf);

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        len = pkh->meta_len + pkh->data_len;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(dev->unit, "Tx packet (%d bytes).\n"), len));
        bcmcnet_pkt_dump((uint8_t *)pkh + PKT_HDR_SIZE, len);
    }

    if (pkh->attrs & PDMA_TX_BIND_QUE) {
        queue = pkh->queue_id;
    }

    return dev->pkt_xmit(dev, queue, buf);
}

/*!
 * Get device information
 */
int
bcmcnet_dev_info_get(int unit, struct bcmcnet_dev_info **info)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    rv = bcmcnet_pdma_dev_info_get(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    *info = &dev->info;

    return SHR_E_NONE;
}

/*!
 * Get device statistics
 */
int
bcmcnet_dev_stats_get(int unit, struct bcmcnet_dev_stats **stats)
{
    struct pdma_dev *dev = &pdma_devices[unit];
    int rv;

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    rv = bcmcnet_pdma_dev_stats_get(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    *stats = &dev->stats;

    return SHR_E_NONE;
}

/*!
 * Reset device statistics
 */
int
bcmcnet_dev_stats_reset(int unit)
{
    struct pdma_dev *dev = &pdma_devices[unit];

    if (!dev->attached) {
        return SHR_E_UNAVAIL;
    }

    return bcmcnet_pdma_dev_stats_reset(dev);
}

