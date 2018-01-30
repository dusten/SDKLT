/*! \file bcmpkt_cnet.c
 *
 * Interfaces for hooking up CNET driver onto BCMPKT API.
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
#include <shr/shr_util.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmcnet/bcmcnet_types.h>
#include <bcmcnet/bcmcnet_main.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#define BSL_LOG_MODULE              BSL_LS_BCMPKT_DMA

#define TXPMD_START_HIGIG           3

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

/*! FIXME:
 * CNET doesn't support multiple disable.
 * Here is a workaround for log enable status.
 */
static int dev_enabled[BCMDRD_CONFIG_MAX_UNITS];

/* Each unit supports one callback. */
bcmpkt_rx_cb_info_t rx_cb_info[BCMDRD_CONFIG_MAX_UNITS];

static bool
unet_dev_inited(int unit) {
    if (bcmdrd_dev_exists(unit) && dev_inited[unit] == 1) {
        return true;
    }
    return false;
}

static int
rx_packet_handle(int unit, int queue, bcmpkt_data_buf_t *dbuf, void *ck)
{
    bcmpkt_packet_t pkt;
    bcmpkt_packet_t *packet = &pkt;
    bcmpkt_rcpu_hdr_t *rhdr;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    sal_memset(packet, 0, sizeof(*packet));
    bcmpkt_pmd_format(packet);
    packet->data_buf = dbuf;
    packet->unit = unit;

    /* Copy RXPMD data. */
    rhdr = (bcmpkt_rcpu_hdr_t *)dbuf->data;
    sal_memcpy(packet->pmd.rxpmd, dbuf->data + sizeof(*rhdr), rhdr->meta_len);

    /* Remove RCPU header and meta data from head of packet. */
    if (!bcmpkt_pull(dbuf, sizeof(*rhdr) + rhdr->meta_len)) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /* BCMPKT API only supports one callback. */
    if (rx_cb_info[unit].cb_func && !rx_cb_info[unit].cb_pending) {
        rx_cb_info[unit].cb_func(unit, queue + 1, packet,
                                 rx_cb_info[unit].cb_data);
    }

exit:
    if (!SHR_FUNC_ERR() && packet->data_buf) {
        bcmpkt_data_buf_free(packet->unit, packet->data_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_dev_init(int unit, const bcmpkt_dev_init_t *cfg)
{
    uint32_t i, meta_len;
    bcmcnet_dev_cfg_t dev_cfg;
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
             (BSL_META_U(unit, "Error: cgrp_bmp(0x%x) and cgrp_size(%d)\n"),
              cfg->cgrp_bmp, cfg->cgrp_size));
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_init(unit, &dev_cfg));

    dev_inited[unit] = 1;

    SHR_IF_ERR_EXIT
        (bcmpkt_rcpu_hdr_set(unit, &rhdr));

    SHR_IF_ERR_EXIT
        (bcmcnet_rx_cb_register(unit, rx_packet_handle, NULL));

    LOG_VERBOSE(BSL_LS_SYS_KNET,
                (BSL_META_U(unit,
                            "Knet device initial succeed: %s, groupbitmap 0x%x\n"),
                 dev_cfg.name, dev_cfg.bm_grp));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_dev_enable(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_start(unit));

    dev_enabled[unit] = true;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_dev_disable(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    if (dev_enabled[unit]) {
        SHR_IF_ERR_EXIT
            (bcmcnet_dev_stop(unit));
        dev_enabled[unit] = false;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_dev_cleanup(int unit)
{
    if (unet_dev_inited(unit)) {
        bcmcnet_rx_cb_unregister(unit, rx_packet_handle, NULL);

        bcmpkt_cnet_dev_disable(unit);
        dev_inited[unit] = 0;
        return bcmcnet_dev_cleanup(unit);
    }

    return SHR_E_NONE;
}

static int
bcmpkt_cnet_chan_set(int unit, bcmpkt_dma_chan_t *chan)
{
    bcmcnet_chan_cfg_t chan_cfg;

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
        chan_cfg.chan_ctrl |= BCMCNET_PKT_BYTE_SWAP;
    }
    if (bcmdrd_dev_byte_swap_non_packet_dma_get(unit)) {
        chan_cfg.chan_ctrl |= BCMCNET_OTH_BYTE_SWAP | BCMCNET_HDR_BYTE_SWAP;
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_chan_config(unit, &chan_cfg));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Setup DMA channel %d: %s, ring size %d frame size %d\n"),
                 chan_cfg.chan, (chan_cfg.dir) ? "TX" : "RX",
                 chan_cfg.nb_desc, chan_cfg.rx_buf_size));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_chan_get(int unit, int chan_id, bcmpkt_dma_chan_t *chan)
{
    bcmcnet_chan_cfg_t chan_cfg;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(chan, SHR_E_PARAM);

    chan_cfg.chan = chan_id;
    SHR_IF_ERR_EXIT
        (bcmcnet_dev_chan_query(unit, &chan_cfg));

    sal_memset(chan, 0, sizeof(*chan));
    chan->dir = chan_cfg.dir;
    chan->id = chan_cfg.chan;
    chan->ring_size = chan_cfg.nb_desc;
    chan->max_frame_size = chan_cfg.rx_buf_size;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_chan_get_list(int unit, uint32_t size, bcmpkt_dma_chan_t *chans,
                           uint32_t *num_elements)
{
    uint32_t chan_num = 0;
    uint32_t chan_id;
    uint32_t chans_bmp;
    struct bcmcnet_dev_info *info;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(num_elements, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_info_get(unit, &info));

    SHR_NULL_CHECK(info, SHR_E_CONFIG);

    chan_num = info->nb_rx_queues + info->nb_tx_queues;
    if (size == 0) {
        *num_elements = chan_num;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_NULL_CHECK(chans, SHR_E_PARAM);

    if (chan_num > size) {
        chan_num = size;
    }
    chans_bmp = info->bm_rx_queues | info->bm_tx_queues;

    *num_elements = 0;
    for (chan_id = 0; chan_id < 32; chan_id++) {
        if (*num_elements >= chan_num) {
            break;
        }

        if (chans_bmp & (0x1 << chan_id)) {
            SHR_IF_ERR_EXIT
                (bcmpkt_cnet_chan_get(unit, chan_id, &chans[*num_elements]));
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
bcmpkt_cnet_dev_info_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_info *info;
    bcmpkt_dma_chan_t *chans = NULL;
    uint32_t i, chan_num;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (bcmcnet_dev_info_get(unit, &info));

    SHR_NULL_CHECK(info, SHR_E_CONFIG);
    packet_device_info_dump(info, pb);

    SHR_ALLOC(chans, sizeof(*chans) * info->max_queues, "bcmpktDevInfoDump");
    SHR_NULL_CHECK(chans, SHR_E_MEMORY);

    /* Dump channel configurations. */
    SHR_IF_ERR_EXIT
        (bcmpkt_cnet_chan_get_list(unit, info->max_queues, chans, &chan_num));

    shr_pb_printf(pb, "\tdev_name: %s\n", info->dev_name);
    shr_pb_printf(pb, "\tDMA Channels:\n");
    for (i = 0; i < chan_num; i++) {
        shr_pb_printf(pb, "\t\tID: %d\n", chans[i].id);
        if (chans[i].dir) {
            shr_pb_printf(pb, "\t\tDirection: TX\n");
        } else {
            shr_pb_printf(pb, "\t\tDirection: RX\n");
            shr_pb_printf(pb, "\t\tMaxFrameSize: %d\n", chans[i].max_frame_size);
        }
        shr_pb_printf(pb, "\t\tRing Size: %d\n", chans[i].ring_size);
    }

exit:
    SHR_FREE(chans);
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_dev_stats_get(int unit, bcmpkt_dev_stat_t *stats)
{
    struct bcmcnet_dev_stats *drv_stats;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(stats, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_stats_get(unit, &drv_stats));

    SHR_NULL_CHECK(drv_stats, SHR_E_INTERNAL);

    sal_memset(stats, 0, sizeof(*stats));
    stats->rx_packets = drv_stats->rx_packets;
    stats->rx_dropped = drv_stats->rx_dropped;
    stats->tx_packets = drv_stats->tx_packets;
    stats->tx_dropped = drv_stats->tx_dropped;
    stats->intrs = drv_stats->intrs;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_dev_stats_dump(int unit, shr_pb_t *pb)
{
    struct bcmcnet_dev_stats *stats;

    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (bcmcnet_dev_stats_get(unit, &stats));

    SHR_NULL_CHECK(stats, SHR_E_INTERNAL);

    packet_device_stats_dump(stats, pb);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_dev_stats_clear(int unit)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_dev_stats_reset(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * The netif_id is network interface ID. For CNET driver, DMA queue (channel)
 * ID is the network interface ID. (ID is start from 1.)
 *
 * CNET driver doesn't support RX DMA channel base callback register.
 */
static int
bcmpkt_cnet_rx_register(int unit, int netif_id, uint32_t flags,
                        bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    SHR_FUNC_ENTER(unit);
    if (!unet_dev_inited(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (rx_cb_info[unit].cb_func) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Exists: Support only one callback function\n")));
    }

    rx_cb_info[unit].cb_func = cb_func;
    rx_cb_info[unit].cb_data = cb_data;
    rx_cb_info[unit].flags = flags;
    rx_cb_info[unit].cb_pending = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * The netif_id is network interface ID. For CNET driver, DMA queue (channel)
 * ID is the network interface ID. (ID is start from 1.)
 *
 * CNET driver doesn't support RX DMA channel base callback register.
 */
static int
bcmpkt_cnet_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                          void *cb_data)
{
    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (rx_cb_info[unit].cb_func == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback not registered\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (rx_cb_info[unit].cb_func != cb_func) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback doesn't match\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    rx_cb_info[unit].cb_pending = TRUE;
    sal_usleep(100000);
    rx_cb_info[unit].cb_func = NULL;
    rx_cb_info[unit].cb_data = NULL;
    rx_cb_info[unit].flags = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
header_generate(int unit, bcmpkt_packet_t *packet)
{
    uint32_t hg_size = 0, pstart = 0, hdr_size = 0;
    uint32_t lbhdr_size = 0, lbstart = 0;
    bcmpkt_data_buf_t *dbuf = NULL;
    HIGIG_t *hghdr = NULL;
    uint32_t dev_type;
    uint8_t *data;
    uint32_t *hdr = NULL;
    uint32_t data_size, meta_len = 0;
    bcmpkt_rcpu_hdr_t *rhdr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));

    rhdr = bcmpkt_rcpu_hdr(unit);
    hghdr = (HIGIG_t *)packet->pmd.higig;

    if (hghdr != NULL) {
        uint32_t hstart = HIGIG_STARTf_GET(*hghdr);
        if (hstart == BCMPKT_HIGIG2_SOF) {
            hg_size = HIGIG2_SIZE;
        } else if (hstart == BCMPKT_HIGIG_SOF) {
            hg_size = HIGIG_SIZE;
        }
    }

    if (packet->pmd.txpmd != NULL) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_get(dev_type, packet->pmd.txpmd,
                                    BCMPKT_TXPMD_START, &pstart));

        if (packet->pmd.lbhdr != NULL) {
            SHR_IF_ERR_EXIT
                (bcmpkt_txpmd_field_get(dev_type, packet->pmd.lbhdr,
                                        BCMPKT_LBHDR_START, &lbstart));
            if (lbstart > 0) {
                lbhdr_size = BCMPKT_LBHDR_SIZE_BYTES;
            }
        }
    }

    /* Calculate request space for RCPU, TXPMD and Higig header. */
    if (pstart > 0) {
        hdr_size = sizeof(*rhdr) + rhdr->meta_len + hg_size + lbhdr_size;
    } else if (hg_size > 0) {
        hdr_size = sizeof(*rhdr) + rhdr->meta_len;
    } else {
        hdr_size = sizeof(*rhdr);
    }

    /* If the head room is not enough, try to re-allocate data buffer. */
    if (BCMPKT_PACKET_DATA(packet) < packet->data_buf->head + hdr_size ||
        unit != packet->unit) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Not enough headroom, re-allocate packet buffer\n")));
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_alloc(packet->unit,
                                   BCMPKT_PACKET_LEN(packet) + hdr_size,
                                   &dbuf));
        if (!bcmpkt_reserve(dbuf, hdr_size)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        if (!bcmpkt_put(dbuf, BCMPKT_PACKET_LEN(packet))) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        sal_memcpy(dbuf->data, BCMPKT_PACKET_DATA(packet),
                   BCMPKT_PACKET_LEN(packet));

        SHR_IF_ERR_CONT
            (bcmpkt_data_buf_free(packet->unit, packet->data_buf));
        packet->data_buf = dbuf;
    }

    data_size = BCMPKT_PACKET_LEN(packet);

    /* Encapsulate TXPMD and Higig header. */
    if (pstart > 0) {
        /* Encapsulate Higig header. */
        if (hg_size > 0) {
            if (!bcmpkt_push(packet->data_buf, hg_size)) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
            shr_util_host_to_be32(hdr, packet->pmd.higig, hg_size / 4);
        }

        /* Encapsulate Loopback header. */
        if (lbhdr_size > 0) {
            if (!bcmpkt_push(packet->data_buf, lbhdr_size)) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
            shr_util_host_to_be32(hdr, packet->pmd.lbhdr, lbhdr_size / 4);
        }
        data_size = BCMPKT_PACKET_LEN(packet);

        /* Set TXPMD metadata. */
        if (hg_size > 0) {
            /* Set Higig flag. */
            SHR_IF_ERR_EXIT
                (bcmpkt_txpmd_field_set(dev_type, packet->pmd.txpmd,
                                        BCMPKT_TXPMD_START, TXPMD_START_HIGIG));
        }
        if (!bcmpkt_push(packet->data_buf, rhdr->meta_len)) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            shr_util_host_to_be32(hdr, packet->pmd.txpmd, rhdr->meta_len / 4);
        } else {
            sal_memcpy(hdr, packet->pmd.txpmd, rhdr->meta_len);
        }
        meta_len = rhdr->meta_len;
    } else if (hg_size > 0) {
        if (rhdr->meta_len < hg_size ||
            (!bcmpkt_push(packet->data_buf, hg_size))) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
        hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            shr_util_host_to_be32(hdr, packet->pmd.higig, hg_size / 4);
        } else {
            sal_memcpy(hdr, packet->pmd.higig, hg_size);
        }
        meta_len = rhdr->meta_len;
    }

    /* Encapsulate RCPU header. */
    if (!bcmpkt_push(packet->data_buf, sizeof(*rhdr))) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    data = BCMPKT_PACKET_DATA(packet);
    sal_memset(data, 0, sizeof(*rhdr));
    data[20] = BCMPKT_RCPU_OP_TX;
    if (meta_len > 0) {
        data[21] = BCMPKT_RCPU_F_MODHDR;
    }
    data[24] = data_size >> 8;
    data[25] = data_size & 0xFF;
    data[28] = meta_len;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_cnet_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMPKT_PACKET_DATA(packet), SHR_E_PARAM);

    if (BCMPKT_PACKET_LEN(packet) < BCMPKT_FRAME_SIZE_MIN) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit,
                         "Packet size %"PRIu32" is underrun\n"),
              BCMPKT_PACKET_LEN(packet)));
    }

    if (packet->type == BCMPKT_FWD_T_NORMAL) {
        SHR_IF_ERR_EXIT
            (header_generate(unit, packet));
    }

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb;
        pb = shr_pb_create();
        shr_pb_printf(pb, "Data buf:\n");
        bcmpkt_data_buf_dump(packet->data_buf, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    SHR_IF_ERR_EXIT
        (bcmcnet_tx(unit, netif_id - 1, packet->data_buf));
    
    packet->data_buf = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_cnet_attach(void)
{
    bcmpkt_dev_t dev_vect = {
        .inited = 1,
        .driver_name = "CNET",
        .driver_type = BCMPKT_DEV_DRV_T_CNET,

        .init = bcmpkt_cnet_dev_init,
        .cleanup = bcmpkt_cnet_dev_cleanup,
        .enable = bcmpkt_cnet_dev_enable,
        .disable = bcmpkt_cnet_dev_disable,
        .dma_chan_ops.set = bcmpkt_cnet_chan_set,
        .dma_chan_ops.get = bcmpkt_cnet_chan_get,
        .dma_chan_ops.get_list = bcmpkt_cnet_chan_get_list,
        .info_dump = bcmpkt_cnet_dev_info_dump,
        .stats_get = bcmpkt_cnet_dev_stats_get,
        .stats_dump = bcmpkt_cnet_dev_stats_dump,
        .stats_clear = bcmpkt_cnet_dev_stats_clear
    };

    bcmpkt_net_t net_vect = {
        .inited = 1,
        .driver_name = "CNET",
        .driver_type = BCMPKT_NET_DRV_T_CNET,
        .rx_register = bcmpkt_cnet_rx_register,
        .rx_unregister = bcmpkt_cnet_rx_unregister,
        .tx = bcmpkt_cnet_tx
    };

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_dev_drv_register(&dev_vect), SHR_E_EXISTS);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_net_drv_register(&net_vect), SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_dev_drv_cnet_detach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_unregister(BCMPKT_NET_DRV_T_CNET));

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_unregister(BCMPKT_DEV_DRV_T_CNET));

exit:
    SHR_FUNC_EXIT();
}
