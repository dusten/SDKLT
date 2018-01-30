/*! \file bcmlu_knet.c
 *
 * Linux user mode KNET driver.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_unet.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcnet/bcmcnet_types.h>
#include <lkm/ngknet_dev.h>

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <unistd.h>

#include <bcmlu/bcmlu_ngknet.h>
#include <bcmlu/bcmpkt/bcmlu_knet.h>

#define BSL_LOG_MODULE              BSL_LS_SYS_KNET

#define DEV_CTRL_NETIF_ID           0

#define TXPMD_START_IHEADER         2
#define TXPMD_HEADER_TYPE_FROM_CPU  1
#define TXPMD_UNICAST               1

/*! Print structure integer member. */
#define PRINT_ST_I(_pb, _s, _m) \
    shr_pb_printf(_pb, "\t%s: %d\n", #_m, _s->_m)
#define PRINT_ST_H(_pb, _s, _m) \
        shr_pb_printf(_pb, "\t%s: 0x%x\n", #_m, _s->_m)
/*! Print queues. */
#define PRINT_ST_I_PER_QUEUE(_pb, _s, _m) \
    { \
        int i; \
        shr_pb_printf(_pb, "\t%s:\n", #_m); \
        for (i = 0; i < NUM_QUE_MAX; i++) { \
            if (_s->_m[i]) \
                shr_pb_printf(_pb, "\t\tqueue_%d: %d\n", i, _s->_m[i]); \
        } \
    }

/*! Print structure uint64 member. */
#define PRINT_ST_U64(_pb, _s, _m) \
    shr_pb_printf(_pb, "\t%s: %" PRIu64 "\n" , #_m, _s->_m)

#define PRINT_ST_U64_PER_QUEUE(_pb, _s, _m) \
    { \
        int i; \
        shr_pb_printf(_pb, "\t%s:\n", #_m); \
        for (i = 0; i < NUM_QUE_MAX; i++) { \
            if (_s->_m[i]) \
                shr_pb_printf(_pb, "\t\tqueue_%d: %" PRIu64 "\n" , i, \
                              _s->_m[i]); \
        } \
    }

static int dev_inited[BCMDRD_CONFIG_MAX_UNITS];

static int
txpmd_encap(int unit, int port, int queue, uint32_t *txpmd)
{
    uint32_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_START, TXPMD_START_IHEADER));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_HEADER_TYPE,
                                TXPMD_HEADER_TYPE_FROM_CPU));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_LOCAL_DEST_PORT, port));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_COS, queue));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_UNICAST, TXPMD_UNICAST));
exit:
    SHR_FUNC_EXIT();
}

/*! FIXME:
 * This is for HIGIG encapsulation for sending from linux protocol
 * stack. Leave for future.
 */
static int
higig_encap(int port, uint8_t *mh)
{
    return SHR_E_UNAVAIL;
}

/*! FIXME:
 * This is for HIGIG2 encapsulation for sending from linux protocol
 * stack. Leave for future.
 */
static int
higig2_encap(int port, uint8_t *mh)
{
    return SHR_E_UNAVAIL;
}

static bool
unet_dev_inited(int unit) {
    if (bcmdrd_dev_exists(unit) && dev_inited[unit] == 1) {
        return true;
    }
    return false;
}

static int
netif_to_bnetif_translate(int unit, bcmpkt_netif_t *netif, ngknet_netif_t *bnetif)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(bnetif, 0, sizeof(*bnetif));
    sal_memcpy(bnetif->macaddr, netif->mac_addr, sizeof(bnetif->macaddr));
    sal_strncpy(bnetif->name, netif->name, sizeof(bnetif->name) - 1);
    bnetif->vlan = netif->vlan;
    bnetif->mtu = netif->max_frame_size;

    if (netif->flags & BCMPKT_NETIF_F_RCPU_ENCAP) {
        bnetif->flags |= NGKNET_NETIF_F_RCPU_ENCAP;
        bnetif->mtu += BCMPKT_RCPU_MAX_ENCAP_SIZE;
    }

    /* 0 means not add VLAN tag. */
    if (netif->vlan != 0) {
        bnetif->flags |= NGKNET_NETIF_F_ADD_TAG;
    }

    if (netif->flags & BCMPKT_NETIF_F_BIND_RX_CH) {
        bnetif->chan = netif->dma_chan_id;
        bnetif->flags |= NGKNET_NETIF_F_BIND_CHAN;
    }
    if (netif->flags & BCMPKT_NETIF_F_BIND_TX_PORT) {
        uint32_t *txpmd = NULL;

        bnetif->type = NGKNET_NETIF_T_PORT;
        txpmd = (uint32_t *)bnetif->meta_data;
        SHR_IF_ERR_EXIT
            (txpmd_encap(unit, netif->port, netif->port_queue, txpmd));

        bnetif->meta_len = NGKNET_NETIF_META_MAX;
        /* KNET driver doesn't support module header, leave for future. */
        if (!sal_strcasecmp("higig2", netif->port_encap)) {
            SHR_IF_ERR_EXIT
                (higig2_encap(netif->port, NULL));
        } else if (!sal_strcasecmp("higig", netif->port_encap)) {
            SHR_IF_ERR_EXIT
                (higig_encap(netif->port, NULL));
        } else if (sal_strcasecmp("ieee", netif->port_encap)) {/* Non-IEEE. */
            if (sal_strlen(netif->port_encap)) {
                SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bnetif_to_netif_translate(int unit, ngknet_netif_t *bnetif, bcmpkt_netif_t *netif)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(netif, 0, sizeof(*netif));
    netif->id = bnetif->id;
    sal_memcpy(netif->mac_addr, bnetif->macaddr, sizeof(netif->mac_addr));
    sal_strncpy(netif->name, bnetif->name, sizeof(netif->name) - 1);
    netif->vlan = bnetif->vlan;
    netif->max_frame_size = bnetif->mtu;

    if (bnetif->flags & NGKNET_NETIF_F_RCPU_ENCAP) {
        netif->flags |= BCMPKT_NETIF_F_RCPU_ENCAP;
        if (netif->max_frame_size > BCMPKT_RCPU_MAX_ENCAP_SIZE) {
            netif->max_frame_size -= BCMPKT_RCPU_MAX_ENCAP_SIZE;
        }
    }

    if (bnetif->flags & NGKNET_NETIF_F_BIND_CHAN) {
        netif->dma_chan_id = bnetif->chan;
        netif->flags |= BCMPKT_NETIF_F_BIND_RX_CH;
    }
    if (bnetif->type & NGKNET_NETIF_T_PORT) {
        uint32_t txpmd[BCMPKT_TXPMD_SIZE_WORDS];
        uint32_t dev_type;

        SHR_IF_ERR_EXIT
            (bcmpkt_dev_type_get(unit, &dev_type));

        sal_memset(txpmd, 0, BCMPKT_TXPMD_SIZE_BYTES);
        sal_memcpy(txpmd, bnetif->meta_data, bnetif->meta_len);

        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_get(dev_type, txpmd,
                                    BCMPKT_TXPMD_LOCAL_DEST_PORT,
                                    (uint32_t *)&netif->port));
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_get(dev_type, txpmd,
                                    BCMPKT_TXPMD_COS,
                                    (uint32_t *)&netif->port_queue));
        /* Current support IEEE only. */
        sal_strcpy(netif->port_encap, "IEEE");
        netif->flags |= BCMPKT_NETIF_F_BIND_TX_PORT;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_create(int unit, bcmpkt_netif_t *netif)
{
    ngknet_netif_t bnetif;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (netif_to_bnetif_translate(unit, netif, &bnetif));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_create(unit, &bnetif));

    netif->id = bnetif.id;
    sal_strcpy(netif->name, bnetif.name);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_get(int unit, int netif_id, bcmpkt_netif_t *netif)
{
    ngknet_netif_t bnetif;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }
    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    sal_memset(&bnetif, 0, sizeof(bnetif));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_get(unit, netif_id, &bnetif));

    SHR_IF_ERR_EXIT
        (bnetif_to_netif_translate(unit, &bnetif, netif));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_traverse(int unit, bcmpkt_netif_traverse_cb_f cb_func, void *cb_data)
{
    ngknet_netif_t bnetif;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    /* Only handle other than id == 0. */
    sal_memset(&bnetif, 0, sizeof(bnetif));
    do {
        SHR_IF_ERR_EXIT
            (bcmlu_ngknet_netif_get_next(unit, &bnetif));

        if (bnetif.id == 0) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bnetif_to_netif_translate(unit, &bnetif, &netif));

        SHR_IF_ERR_EXIT
            (cb_func(unit, &netif, cb_data));
    } while (bnetif.next != 0);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_netif_destroy(int unit, int netif_id)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    /* Destroy a UNET netif is prohibit. */
    if (bcmpkt_unet_exists(unit, netif_id)) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_RESOURCE,
             (BSL_META_U(unit, "Destroy a UNET netif (netif %d) is prohibit\n"),
              netif_id));
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_destroy(unit, netif_id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_dev_ctrl_intf_get(int unit, bcmpkt_netif_t *netif)
{
    ngknet_netif_t bnetif;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(netif, SHR_E_PARAM);

    sal_memset(&bnetif, 0, sizeof(bnetif));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_netif_get(unit, DEV_CTRL_NETIF_ID, &bnetif));

    SHR_IF_ERR_EXIT
        (bnetif_to_netif_translate(unit, &bnetif, netif));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_chan_set(int unit, bcmpkt_dma_chan_t *chan)
{
    ngknet_chan_cfg_t chan_cfg;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(chan, SHR_E_PARAM);

    sal_memset(&chan_cfg, 0, sizeof(chan_cfg));
    chan_cfg.chan = chan->id;
    chan_cfg.dir = chan->dir;
    chan_cfg.nb_desc = chan->ring_size;
    chan_cfg.rx_buf_size = chan->max_frame_size;

    if (bcmdrd_dev_byte_swap_packet_dma_get(unit)) {
        chan_cfg.chan_ctrl |= NGKNET_PKT_BYTE_SWAP;
    }
    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        chan_cfg.chan_ctrl |= NGKNET_OTH_BYTE_SWAP | NGKNET_HDR_BYTE_SWAP;
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_chan_config(unit, &chan_cfg));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Setup DMA channel %d: %s, ring size %"PRIu32" frame size %"PRIu32"\n"),
                 chan_cfg.chan, (chan_cfg.dir) ? "TX" : "RX",
                 chan_cfg.nb_desc, chan_cfg.rx_buf_size));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_chan_get(int unit, int chan_id, bcmpkt_dma_chan_t *chan)
{
    ngknet_chan_cfg_t chan_cfg;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(chan, SHR_E_PARAM);

    sal_memset(&chan_cfg, 0, sizeof(chan_cfg));
    chan_cfg.chan = chan_id;
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_chan_query(unit, &chan_cfg));

    chan->dir = chan_cfg.dir;
    chan->id = chan_cfg.chan;
    chan->ring_size = chan_cfg.nb_desc;
    chan->max_frame_size = chan_cfg.rx_buf_size;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief DMA channel list retrieve function.
 *
 * Retrieve a list of DMA channels' configurations.
 *
 * \param [in] unit Switch unit number.
 * \param [in] size  The maximum number of \ref bcmpkt_dma_chan_t elements
 *                   can be held.
 * \param [out] chans The \ref bcmpkt_dma_chan_t elements array.
 * \param [out] num_elements The number of \ref bcmpkt_dma_chan_t elements
 *                           copied to 'chans', if size > 0; total number of
 *                           DMA channel in the unit can be used by packet
 *                           APIs, if size = 0.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM A parameter was invalid.
 */
static int
bcmlu_chan_get_list(int unit, uint32_t size, bcmpkt_dma_chan_t *chans,
                    uint32_t *num_elements)
{
    uint32_t chan_num = 0;
    uint32_t chan_id;
    uint32_t chans_bmp;
    struct bcmcnet_dev_info info;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(num_elements, SHR_E_PARAM);

    sal_memset(&info, 0, sizeof(info));
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_info_get(unit, &info));

    chan_num = info.nb_rx_queues + info.nb_tx_queues;
    if (size == 0) {
        *num_elements = chan_num;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_NULL_CHECK(chans, SHR_E_PARAM);

    if (chan_num > size) {
        chan_num = size;
    }
    chans_bmp = info.bm_rx_queues | info.bm_tx_queues;

    *num_elements = 0;
    for (chan_id = 0; chan_id < 32; chan_id++) {
        if (*num_elements >= chan_num) {
            break;
        }

        if (chans_bmp & (0x1 << chan_id)) {
            SHR_IF_ERR_EXIT
                (bcmlu_chan_get(unit, chan_id, &chans[*num_elements]));
            (*num_elements)++;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
packet_device_info_dump(struct bcmcnet_dev_info *info, shr_pb_t *pb)
{
    shr_pb_printf(pb, "\tdev_name: %s\n", info->dev_name);
    PRINT_ST_I(pb, info, max_groups);
    PRINT_ST_I(pb, info, max_queues);
    PRINT_ST_H(pb, info, bm_groups);
    PRINT_ST_H(pb, info, bm_rx_queues);
    PRINT_ST_H(pb, info, bm_tx_queues);
    PRINT_ST_I(pb, info, nb_groups);
    PRINT_ST_I(pb, info, nb_rx_queues);
    PRINT_ST_I(pb, info, nb_tx_queues);
    PRINT_ST_I(pb, info, rx_desc_size);
    PRINT_ST_I(pb, info, tx_desc_size);
    PRINT_ST_I(pb, info, rx_buf_dflt);
    PRINT_ST_I(pb, info, nb_desc_dflt);

    PRINT_ST_I_PER_QUEUE(pb, info, rx_buf_size);
    PRINT_ST_I_PER_QUEUE(pb, info, nb_rx_desc);
    PRINT_ST_I_PER_QUEUE(pb, info, nb_tx_desc);
}

static void
packet_device_stats_dump(struct bcmcnet_dev_stats *stats, shr_pb_t *pb)
{
    PRINT_ST_U64(pb, stats, rx_packets);
    PRINT_ST_U64(pb, stats, rx_bytes);
    PRINT_ST_U64(pb, stats, rx_dropped);
    PRINT_ST_U64(pb, stats, rx_errors);
    PRINT_ST_U64(pb, stats, rx_head_errors);
    PRINT_ST_U64(pb, stats, rx_data_errors);
    PRINT_ST_U64(pb, stats, rx_cell_errors);
    PRINT_ST_U64(pb, stats, rx_nomems);
    PRINT_ST_U64(pb, stats, tx_packets);
    PRINT_ST_U64(pb, stats, tx_bytes);
    PRINT_ST_U64(pb, stats, tx_dropped);
    PRINT_ST_U64(pb, stats, tx_errors);
    PRINT_ST_U64(pb, stats, tx_xoffs);
    PRINT_ST_U64(pb, stats, intrs);

    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_packets);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_bytes);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_dropped);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_head_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_data_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_cell_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, rxq_nomems);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_packets);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_bytes);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_dropped);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_errors);
    PRINT_ST_U64_PER_QUEUE(pb, stats, txq_xoffs);
}

static int
bcmlu_unet_dev_info_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_info info;
    bcmpkt_dma_chan_t *chans = NULL;
    uint32_t i, chan_num;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    sal_memset(&info, 0, sizeof(info) );
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_info_get(unit, &info));

    packet_device_info_dump(&info, pb);

    SHR_ALLOC(chans, sizeof(*chans) * info.max_queues, "bcmpktDevInfoDump");
    SHR_NULL_CHECK(chans, SHR_E_MEMORY);

    /* Dump channel configurations. */
    SHR_IF_ERR_EXIT
        (bcmlu_chan_get_list(unit, info.max_queues, chans, &chan_num));

    shr_pb_printf(pb, "\tDMA Channels:\n");
    for (i = 0; i < chan_num; i++) {
        shr_pb_printf(pb, "\t\tID: %d\n", chans[i].id);
        if (chans[i].dir) {
            shr_pb_printf(pb, "\t\tDirection: TX\n");
        } else {
            shr_pb_printf(pb, "\t\tDirection: RX\n");
            shr_pb_printf(pb, "\t\tMaxFrameSize: %"PRIu32"\n", chans[i].max_frame_size);
        }
        shr_pb_printf(pb, "\t\tRing Size: %"PRIu32"\n", chans[i].ring_size);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_dev_ctrl_intf_get(unit, &netif));
    shr_pb_printf(pb, "\tNetwork interface:\n");
    shr_pb_printf(pb, "\t\tID: %d\n", netif.id);
    shr_pb_printf(pb, "\t\tName: %s\n", netif.name);
    shr_pb_printf(pb, "\t\tMACaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  netif.mac_addr[0], netif.mac_addr[1], netif.mac_addr[2],
                  netif.mac_addr[3], netif.mac_addr[4], netif.mac_addr[5]);

exit:
    SHR_FREE(chans);
    SHR_FUNC_EXIT();
}

static int
bcmlu_unet_dev_stats_get(int unit, bcmpkt_dev_stat_t *stats)
{
    struct bcmcnet_dev_stats drv_stats;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(stats, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_stats_get(unit, &drv_stats));

    sal_memset(stats, 0, sizeof(*stats));
    stats->rx_packets = drv_stats.rx_packets;
    stats->rx_dropped = drv_stats.rx_dropped;
    stats->tx_packets = drv_stats.tx_packets;
    stats->tx_dropped = drv_stats.tx_dropped;
    stats->intrs = drv_stats.intrs;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_unet_dev_stats_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_stats stats;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_stats_get(unit, &stats));

    packet_device_stats_dump(&stats, pb);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_unet_dev_stats_clear(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_stats_reset(unit));
exit:
    SHR_FUNC_EXIT();
}

static int
netif_enable(int unit, bool enable)
{
    int sock_fd = -1;
    struct ifreq ifr;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlu_dev_ctrl_intf_get(unit, &netif));

    /* Create socket. */
    sock_fd = socket(PF_PACKET, SOCK_RAW, 0);
    if (sock_fd == -1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Create socket failed (%s)\n"), strerror(errno)));

        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strncpy(ifr.ifr_name, netif.name, sizeof(ifr.ifr_name) - 1);
    if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
        close(sock_fd);
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Ioctl error\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* Set interface configuration. */
    if (enable) {
        ifr.ifr_flags |= IFF_UP;
    }
    else {
        ifr.ifr_flags &= ~IFF_UP;
    }
    if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
        close(sock_fd);
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Ioctl error\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s packet device %s\n"),
                 (enable) ? "Open" : "Close", netif.name));
    close(sock_fd);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_unet_dev_enable(int unit)
{
    struct bcmcnet_dev_info dev_info;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_info_get(unit, &dev_info));
    if (dev_info.nb_rx_queues == 0 || dev_info.nb_tx_queues == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Not config Rx or Tx queue yet\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (netif_enable(unit, 1));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_unet_dev_disable(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (netif_enable(unit, 0));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_unet_dev_init(int unit, const bcmpkt_dev_init_t *cfg)
{
    uint32_t i, meta_len;
    ngknet_dev_cfg_t dev_cfg;
    bcmdrd_dev_type_t dev_type;
    const char *dev_name = NULL;
    bcmpkt_rcpu_hdr_t rhdr = {
        .tpid       = BCMPKT_RCPU_TPID,
        .vlan       = BCMPKT_RCPU_VLAN,
        .ethertype  = BCMPKT_RCPU_ETYPE,
        .flags      = BCMPKT_RCPU_F_MODHDR,
    };

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    if (dev_inited[unit] == 1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Network device has already been inited\n")));
    }

    sal_memset(&dev_cfg, 0, sizeof(dev_cfg));
    if (sal_strlen(cfg->name) > 0) {
        sal_strncpy(dev_cfg.name, cfg->name, sizeof(dev_cfg.name) - 1);
    }
    else {
        sal_sprintf(dev_cfg.name, "%s%d", "bcm", unit);
    }

    dev_name = bcmdrd_dev_type_str(unit);
    sal_strncpy(dev_cfg.type_str, dev_name, sizeof(dev_cfg.type_str) - 1);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_id_get(unit, &dev_cfg.dev_id));
    if (dev_cfg.dev_id == 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_len_get(dev_type, &meta_len));
    if (meta_len > BCMPKT_RCPU_TX_MH_SIZE) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    rhdr.meta_len = meta_len & 0xff;

    /* Set RCPU header signature with device ID. */
    rhdr.signature = dev_cfg.dev_id;
    dev_cfg.bm_grp = cfg->cgrp_bmp;
    dev_cfg.nb_grp = 0;
    i = dev_cfg.bm_grp;
    while (i != 0) {
        dev_cfg.nb_grp++;
        i &= (i - 1);
    }

    if (dev_cfg.nb_grp == 0){
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Error: cgrp_bmp(0x%x) and cgrp_size(%"PRIu32")\n"),
              cfg->cgrp_bmp, cfg->cgrp_size));
    }

    sal_memcpy(dev_cfg.base_netif.macaddr, cfg->mac_addr, 6);
    sal_strncpy(dev_cfg.base_netif.name, dev_cfg.name,
        sizeof(dev_cfg.base_netif.name) - 1);
    dev_cfg.base_netif.vlan = 1;
    dev_cfg.base_netif.id = -1;
    dev_cfg.base_netif.mtu = 1500;
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_init(unit, &dev_cfg));

    dev_inited[unit] = 1;

    /* Configure RCPU header. */
    SHR_IF_ERR_EXIT
        (bcmpkt_rcpu_hdr_set(unit, &rhdr));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Knet device initial succeed: %s, groupbitmap 0x%x\n"),
                 dev_cfg.name, dev_cfg.bm_grp));

exit:
    /* Configure RCPU header failed, cleanup. */
    if (SHR_FUNC_ERR() &&
        (!SHR_FUNC_VAL_IS(SHR_E_EXISTS)) &&
        dev_inited[unit] == 1) {
        bcmlu_ngknet_dev_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

static int
netif_delete(int unit, const bcmpkt_netif_t *netif, void *data)
{
    if (netif != NULL) {
        /*
         * If some netif could not be removed, Error log would be output in
         * bcmlu_netif_destroy, and here just ignore it and destroy others.
         */
        bcmlu_netif_destroy(unit, netif->id);
    }

    return SHR_E_NONE;
}

static int
bcmlu_unet_dev_cleanup(int unit)
{
    if (unet_dev_inited(unit)) {

        bcmpkt_unet_cleanup(unit);

        bcmlu_netif_traverse(unit, netif_delete, NULL);

        dev_inited[unit] = 0;

        return bcmlu_ngknet_dev_cleanup(unit);
    }

    return SHR_E_NONE;
}

/*
 * Parse data, ignore front and 0 bytes
 * data -  data for parsing.
 * data_len - data maximum size.
 * offset - the first non 0 byte position.
 * size - the size from the first !0 byte to the last !0 byte.
 */
static void
parse_data_range(const uint8_t *data, int data_len, uint16_t *offset, uint16_t *size)
{
    uint16_t i;
    int found = 0;

    for (i = 0; i < data_len; i++) {
        if (data[i] != 0) {
            if (found == 0) {
                *offset = i;
                break;
            }
        }
    }
    *size = data_len - *offset;
}

/*
 * bfilter->oob_data is full 16 words dcb.
 * bfilter->oob_data_offset - dcb[0]
 */
static int
filter_to_bfilter_translate(int unit, bcmpkt_filter_t *filter,
                            ngknet_filter_t *bfilter)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(bfilter, 0, sizeof(*bfilter));
    bfilter->id = filter->id;
    bfilter->type = filter->type;
    bfilter->priority = filter->priority;
    sal_strcpy(bfilter->desc, filter->desc);
    bfilter->flags = filter->flags;
    bfilter->dest_type = filter->dest_type;
    bfilter->dest_id = filter->dest_id;
    bfilter->dest_proto = filter->dest_proto;
    bfilter->mirror_type = filter->mirror_type;
    bfilter->mirror_id = filter->mirror_id;
    bfilter->mirror_proto = filter->mirror_proto;

    if (filter->match_flags &
        (BCMPKT_FILTER_M_VLAN
        | BCMPKT_FILTER_M_INGPORT
        | BCMPKT_FILTER_M_SRC_MODPORT
        | BCMPKT_FILTER_M_SRC_MODID
        | BCMPKT_FILTER_M_REASON
        | BCMPKT_FILTER_M_FP_RULE
        | BCMPKT_FILTER_M_ERROR
        | BCMPKT_FILTER_M_CPU_QUEUE)) {
        uint32_t mask_val;
        HIGIG_t *hg, *hg_mask;
        uint32_t *u32p = NULL;
        uint32_t rxpmd[BCMPKT_RXMETA_SIZE_WORDS] = {0,};
        uint32_t rxpmd_mask[BCMPKT_RXMETA_SIZE_WORDS] = {0,};
        uint32_t rxpmd_ref[BCMPKT_RXMETA_SIZE_WORDS] = {0,};
        uint32_t dev_type;

        SHR_IF_ERR_EXIT
            (bcmpkt_dev_type_get(unit, &dev_type));

        /* This is for create field mask. */
        sal_memset(rxpmd_ref, 0xff, BCMPKT_RXMETA_SIZE_BYTES);

        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_mh_get(dev_type, rxpmd, &u32p));
        hg = (HIGIG_t *)u32p;
        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_mh_get(dev_type, rxpmd_mask, &u32p));
        hg_mask = (HIGIG_t *)u32p;

        if (filter->match_flags & BCMPKT_FILTER_M_VLAN) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_OUTER_VID, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_OUTER_VID, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_OUTER_VID, filter->m_vlan));
        }
        if (filter->match_flags & BCMPKT_FILTER_M_INGPORT) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_SRC_PORT_NUM, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_SRC_PORT_NUM, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_SRC_PORT_NUM,
                                        filter->m_ingport));
        }
        if (filter->match_flags & BCMPKT_FILTER_M_SRC_MODPORT) {
            HIGIG_SRC_PORT_TGIDf_SET(*hg, filter->m_src_modport);
            HIGIG_SRC_PORT_TGIDf_SET(*hg_mask, 0x3f);
        }
        if (filter->match_flags & BCMPKT_FILTER_M_SRC_MODID) {
            HIGIG_SRC_MODID_LSf_SET(*hg, filter->m_src_modid & 0x1f);
            HIGIG_SRC_MODID_LSf_SET(*hg_mask, 0x1f);
            HIGIG_SRC_MODID_5f_SET(*hg, ((filter->m_src_modid & 0x20) != 0));
            HIGIG_SRC_MODID_5f_SET(*hg_mask, 0x1);
            HIGIG_SRC_MODID_6f_SET(*hg, ((filter->m_src_modid & 0x40) != 0));
            HIGIG_SRC_MODID_6f_SET(*hg_mask, 0x1);
        }
        if (filter->match_flags & BCMPKT_FILTER_M_REASON) {
            bcmpkt_rx_reasons_t reasons;
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_reasons_get(dev_type, rxpmd_ref, &reasons));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_reasons_set(dev_type, &reasons, rxpmd_mask));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_reasons_set(dev_type, &filter->m_reason, rxpmd));
        }
        if (filter->match_flags & BCMPKT_FILTER_M_FP_RULE) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_MATCHED_RULE, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_MATCHED_RULE, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_MATCHED_RULE,
                                        filter->m_fp_rule));
        }
        if (filter->match_flags & BCMPKT_FILTER_M_CPU_QUEUE) {
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_get(dev_type, rxpmd_ref,
                                        BCMPKT_RXPMD_CPU_COS, &mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd_mask,
                                        BCMPKT_RXPMD_CPU_COS, mask_val));
            SHR_IF_ERR_EXIT
                (bcmpkt_rxpmd_field_set(dev_type, rxpmd,
                                        BCMPKT_RXPMD_CPU_COS,
                                        filter->m_cpu_queue));
        }
        if (filter->match_flags & BCMPKT_FILTER_M_ERROR) {
            rxpmd[BCMPKT_RXMETA_SIZE_WORDS - 1] = BCMPKT_RXMETA_ST_CELL_ERROR;
            rxpmd_mask[BCMPKT_RXMETA_SIZE_WORDS - 1] = BCMPKT_RXMETA_ST_CELL_ERROR;
        }

        /* Search offset and data size */
        parse_data_range((uint8_t *)rxpmd_mask, BCMPKT_RXMETA_SIZE_BYTES,
                         &bfilter->oob_data_offset, &bfilter->oob_data_size);
        sal_memcpy(bfilter->data.b,
                   &((uint8_t *)rxpmd)[bfilter->oob_data_offset],
                   bfilter->oob_data_size);
        sal_memcpy(bfilter->mask.b,
                   &((uint8_t *)rxpmd_mask)[bfilter->oob_data_offset],
                   bfilter->oob_data_size);
    }

    if (filter->match_flags & BCMPKT_FILTER_M_RAW) {
        uint32_t i;
        for (i = 0; i < filter->raw_size; i++) {
            if (!filter->m_raw_mask[i]) {
                filter->m_raw_data[i] = 0;
            }
        }
        parse_data_range(filter->m_raw_mask, filter->raw_size,
                         &bfilter->pkt_data_offset, &bfilter->pkt_data_size);
        if (bfilter->oob_data_size + bfilter->pkt_data_size >
            NGKNET_FILTER_BYTES_MAX) {
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        sal_memcpy(&bfilter->data.b[bfilter->oob_data_size],
                   &filter->m_raw_data[bfilter->pkt_data_offset],
                   bfilter->pkt_data_size);
        sal_memcpy(&bfilter->mask.b[bfilter->oob_data_size],
                   &filter->m_raw_mask[bfilter->pkt_data_offset],
                   bfilter->pkt_data_size);
    }
    bfilter->chan = filter->dma_chan;

exit:
    SHR_FUNC_EXIT();
}

static int
bfilter_to_filter_translate(int unit, ngknet_filter_t *bfilter,
                            bcmpkt_filter_t *filter)
{
    HIGIG_t *hg, *hg_mask;
    uint32_t *u32p = NULL;
    uint32_t mask_val;
    bcmpkt_rx_reasons_t reason_mask;
    int reason_count = 0;
    uint32_t rxpmd[BCMPKT_RXMETA_SIZE_WORDS] = {0,};
    uint32_t rxpmd_mask[BCMPKT_RXMETA_SIZE_WORDS] = {0,};
    uint32_t dev_type;
    uint32_t val;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));

    sal_memset(filter, 0, sizeof(*filter));
    filter->id = bfilter->id;
    filter->type = bfilter->type;
    filter->priority = bfilter->priority;
    sal_strcpy(filter->desc, bfilter->desc);
    filter->flags = bfilter->flags;
    filter->dest_type = bfilter->dest_type;
    filter->dest_id = bfilter->dest_id;
    filter->dest_proto = bfilter->dest_proto;
    filter->mirror_type = bfilter->mirror_type;
    filter->mirror_id = bfilter->mirror_id;
    filter->mirror_proto = bfilter->mirror_proto;
    filter->dma_chan = bfilter->chan;

    if (bfilter->pkt_data_size > 0) {
        filter->raw_size = bfilter->pkt_data_size + bfilter->pkt_data_offset;
        sal_memcpy(&filter->m_raw_data[bfilter->pkt_data_offset],
                   &bfilter->data.b[bfilter->oob_data_size], bfilter->pkt_data_size);
        sal_memcpy(&filter->m_raw_mask[bfilter->pkt_data_offset],
                   &bfilter->mask.b[bfilter->oob_data_size], bfilter->pkt_data_size);
        filter->match_flags |= BCMPKT_FILTER_M_RAW;
    }

    sal_memcpy(&((uint8_t *)rxpmd)[bfilter->oob_data_offset],
               bfilter->data.b,
               bfilter->oob_data_size);
    sal_memcpy(&((uint8_t *)rxpmd_mask)[bfilter->oob_data_offset],
               bfilter->mask.b,
               bfilter->oob_data_size);

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_mh_get(dev_type, rxpmd, &u32p));
    hg = (HIGIG_t *)u32p;
    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_mh_get(dev_type, rxpmd_mask, &u32p));
    hg_mask = (HIGIG_t *)u32p;

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                BCMPKT_RXPMD_OUTER_VID, &mask_val));
    if (mask_val) {
        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                    BCMPKT_RXPMD_OUTER_VID, &val));
        filter->m_vlan = val & 0xffff;
        filter->match_flags |= BCMPKT_FILTER_M_VLAN;
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                BCMPKT_RXPMD_SRC_PORT_NUM, &mask_val));
    if (mask_val) {
        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                    BCMPKT_RXPMD_SRC_PORT_NUM, &val));
        filter->m_ingport = (int)val;
        filter->match_flags |= BCMPKT_FILTER_M_INGPORT;
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                BCMPKT_RXPMD_CPU_COS, &mask_val));
    if (mask_val) {
        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                    BCMPKT_RXPMD_CPU_COS, &val));
        filter->m_cpu_queue = (int)val;
        filter->match_flags |= BCMPKT_FILTER_M_CPU_QUEUE;
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_field_get(dev_type, rxpmd_mask,
                                BCMPKT_RXPMD_MATCHED_RULE, &mask_val));
    if (mask_val) {
        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                    BCMPKT_RXPMD_MATCHED_RULE, &val));
        filter->m_fp_rule = val;
        filter->match_flags |= BCMPKT_FILTER_M_FP_RULE;
    }

    if (HIGIG_SRC_PORT_TGIDf_GET(*hg_mask) > 0) {
        filter->m_src_modport = HIGIG_SRC_PORT_TGIDf_GET(*hg);
        filter->match_flags |= BCMPKT_FILTER_M_SRC_MODPORT;
    }

    if ((HIGIG_SRC_MODID_LSf_GET(*hg_mask) & 0x1f) > 0) {
        filter->m_src_modid = (HIGIG_SRC_MODID_LSf_GET(*hg) & 0x1f)
                            | (HIGIG_SRC_MODID_5f_GET(*hg) << 5)
                            | (HIGIG_SRC_MODID_6f_GET(*hg) << 6);
        filter->match_flags |= BCMPKT_FILTER_M_SRC_MODID;
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_reasons_get(dev_type, rxpmd_mask, &reason_mask));

    SHR_BITCOUNT_RANGE(reason_mask.pbits, reason_count,
                       0, BCMPKT_RX_REASON_COUNT);
    if (reason_count > 0) {
        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_reasons_get(dev_type, rxpmd, &filter->m_reason));
        filter->match_flags |= BCMPKT_FILTER_M_REASON;
    }

    /* Support error match, but not support ~error match.
     * To support ~error match, need to add a variable.
     */
    if (rxpmd[BCMPKT_RXMETA_SIZE_WORDS - 1] &
        rxpmd_mask[BCMPKT_RXMETA_SIZE_WORDS - 1] &
        BCMPKT_RXMETA_ST_CELL_ERROR) {
        filter->match_flags |= BCMPKT_FILTER_M_ERROR;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_create(int unit, bcmpkt_filter_t *filter)
{
    ngknet_filter_t bfilter;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (filter_to_bfilter_translate(unit, filter, &bfilter));

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_filter_create(unit, &bfilter));

    filter->id = bfilter.id;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_destroy(int unit, int filter_id)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_filter_destroy(unit, filter_id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_get(int unit, int filter_id, bcmpkt_filter_t *filter)
{
    ngknet_filter_t bfilter;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    sal_memset(&bfilter, 0, sizeof(bfilter));
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_filter_get(unit, filter_id, &bfilter));

    SHR_IF_ERR_EXIT
        (bfilter_to_filter_translate(unit, &bfilter, filter));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_filter_traverse(int unit, bcmpkt_filter_traverse_cb_f cb_func,
                      void *cb_data)
{
    ngknet_filter_t bfilter;
    bcmpkt_filter_t filter;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    sal_memset(&bfilter, 0, sizeof(bfilter));
    do {
        int rv = bcmlu_ngknet_filter_get_next(unit, &bfilter);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (rv, SHR_E_NOT_FOUND);

        /* No entry, return. */
        if (rv == SHR_E_NOT_FOUND) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "No filter entry\n")));
            break;
        }

        SHR_IF_ERR_EXIT
            (bfilter_to_filter_translate(unit, &bfilter, &filter));

        cb_func(unit, &filter, cb_data);
    } while (bfilter.next != 0);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rx_rate_limit_set(int unit, int rate_limit)
{

    SHR_FUNC_ENTER(unit);

    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_rx_rate_limit(&rate_limit, true));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rx_rate_limit_get(int unit, int *rate_limit)
{

    SHR_FUNC_ENTER(unit);

    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_rx_rate_limit(rate_limit, false));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rcpu_hdr_set(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    struct ngknet_rcpu_hdr nhdr;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(hdr, SHR_E_PARAM);

    sal_memcpy(&nhdr, hdr, sizeof(nhdr));
    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_rcpu_config(unit, &nhdr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rcpu_hdr_get(int unit, bcmpkt_rcpu_hdr_t *hdr)
{
    struct ngknet_rcpu_hdr nhdr;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(hdr, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_dev_rcpu_get(unit, &nhdr));

    sal_memcpy(hdr, &nhdr, sizeof(nhdr));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_knet_attach(void)
{
    bcmpkt_knet_t knet_vect = {
        .inited = 1,
        .driver_name = "KNET",
        .netif_ops.create = bcmlu_netif_create,
        .netif_ops.get = bcmlu_netif_get,
        .netif_ops.traverse = bcmlu_netif_traverse,
        .netif_ops.destroy = bcmlu_netif_destroy,
        .filter_ops.create = bcmlu_filter_create,
        .filter_ops.get = bcmlu_filter_get,
        .filter_ops.traverse = bcmlu_filter_traverse,
        .filter_ops.destroy = bcmlu_filter_destroy
    };

    bcmpkt_dev_t dev_vect = {
        .inited = 1,
        .driver_name = "KNET",
        .driver_type = BCMPKT_DEV_DRV_T_KNET,

        .init = bcmlu_unet_dev_init,
        .cleanup = bcmlu_unet_dev_cleanup,
        .enable = bcmlu_unet_dev_enable,
        .disable = bcmlu_unet_dev_disable,
        .dma_chan_ops.set = bcmlu_chan_set,
        .dma_chan_ops.get = bcmlu_chan_get,
        .dma_chan_ops.get_list = bcmlu_chan_get_list,
        .info_dump = bcmlu_unet_dev_info_dump,
        .stats_get = bcmlu_unet_dev_stats_get,
        .stats_dump = bcmlu_unet_dev_stats_dump,
        .stats_clear = bcmlu_unet_dev_stats_clear,
        .rcpu_hdr_set = bcmlu_rcpu_hdr_set,
        .rcpu_hdr_get = bcmlu_rcpu_hdr_get,
        .rx_rate_limit_ops.set = bcmlu_rx_rate_limit_set,
        .rx_rate_limit_ops.get = bcmlu_rx_rate_limit_get
    };

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmlu_ngknet_check());

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_dev_drv_register(&dev_vect), SHR_E_EXISTS);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_knet_drv_register(&knet_vect), SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_knet_detach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_knet_drv_unregister());

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_unregister(BCMPKT_DEV_DRV_T_KNET));

exit:
    SHR_FUNC_EXIT();
}

