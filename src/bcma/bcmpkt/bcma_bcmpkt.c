/*! \file bcma_bcmpkt.c
 *
 * General functions for Packet I/O command Lines.
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
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_internal.h>
#include "cmd/bcma_bcmpktcmd_internal.h"
#include <bcma/io/bcma_io_file.h>

#define BSL_LOG_MODULE BSL_LS_APPL_PACKET

/*! Convert two hex-characters to an integer */
static int
xbyte2i(int xch_hi, int xch_lo)
{
    if ((xch_hi = shr_util_xch2i(xch_hi)) < 0) {
        xch_hi = 0;
    }
    if ((xch_lo = shr_util_xch2i(xch_lo)) < 0) {
        xch_lo = 0;
    }
    return (xch_hi << 4) | xch_lo;
}

/* Ignore comments. */
static int
file_discard_comments(bcma_io_file_handle_t fh)
{
    int c;

    do {
        if ((c = bcma_io_file_getc(fh)) == -1) {
            return c;
        }
    } while ( c != '\n');

    return c;
}

/*
 * Load each byte of packet data from file:
 * c - byte high 4 bits.
 * d - byte low 4 bits.
 */
static int
file_load_one_byte(bcma_io_file_handle_t fh, uint8_t *byte)
{
    int c, d;

    /* Parse first char. */
    do {
        if ((c = bcma_io_file_getc(fh)) == -1) {
            return -1;
        }
        else if (c == '#') { /* Ignore comments. */
            if ((c = file_discard_comments(fh)) == -1) {
                return -1;
            }
        }
    } while (!sal_isxdigit(c));

    /* Parse second char. */
    do {
        if ((d = bcma_io_file_getc(fh)) == -1) {
            return -1;
        }
        else if (d == '#') {/* Ignore comments. */
            if ((d = file_discard_comments(fh)) == '\0') {
                return -1;
            }
        }
    } while (!sal_isxdigit(d));

    /* Compose a byte. */
    *byte = xbyte2i(c, d);

    return 0;
}

/*
 * Random pattern.
 */
static void
packet_payload_fill_random(uint8_t *buf, int size)
{
    int      pat_off = 24;
    /* coverity[dont_call] */
    uint32_t pat = sal_rand();

    while (size > 3) {
        *buf++ = pat >> 24;
        *buf++ = pat >> 16;
        *buf++ = pat >> 8;
        *buf++ = pat & 0xff;
        /* coverity[dont_call] */
        pat = sal_rand();
        size -= 4;
    }

    while (size > 0) {
        *buf++ = pat >> pat_off;
        pat_off -= 8;
        size--;
    }
}

/* Pattern increase by word. */
static uint32_t
packet_payload_fill_pattern(uint8_t *buf, int size, uint32_t pat, uint32_t inc)
{
    int pat_off = 24;

    while (size > 3) {
        *buf++ = pat >> 24;
        *buf++ = pat >> 16;
        *buf++ = pat >> 8;
        *buf++ = pat & 0xff;
        pat += inc;
        size-=4;
    }

    while (size > 0) {
        *buf++ = pat >> pat_off;
        pat_off -= 8;
        size--;
    }

    return pat;
}

void
bcma_bcmpkt_macaddr_inc(shr_mac_t macaddr, int amount)
{
    int i, v;
    for (i = 5; i >= 0; i--) {
        v = (int) macaddr[i] + amount;
        macaddr[i] = v;
        if (v >= 0 && v <= 255)
            break;
        amount = v >> 8;
    }
}

int
bcma_bcmpkt_packet_payload_fill(bcma_bcmpkt_pktgen_cfg_t *cfg,
                                bcmpkt_data_buf_t *dbuf)
{
    int payload_len;
    int offset = 0;
    uint8_t *filladdr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    offset = ENET_MAC_SIZE * 2 + 2 /* Ethertype. */;

    /* Tagged. */
    if (!cfg->untagged) {
        offset += 4;
    }

    payload_len = dbuf->data_len - offset;
    filladdr = dbuf->data + offset;
    /* Fill pattern */
    if (cfg->pat_random) {
        packet_payload_fill_random(filladdr, payload_len);
    }
    else {
        cfg->pattern = packet_payload_fill_pattern(filladdr, payload_len,
                                                   cfg->pattern, cfg->pat_inc);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Load data from input string. */
int
bcma_bcmpkt_load_data_from_istr(char *istr, uint8_t *buf, uint32_t buf_size,
                                uint32_t *data_size)
{
    uint32_t byte, chr, len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(istr, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);
    SHR_NULL_CHECK(data_size, SHR_E_PARAM);

    len = sal_strlen(istr);
    if (len % 2) {
        LOG_ERROR(BSL_LS_APPL_PACKET,
                  (BSL_META("Input data byte error(odd char)\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    byte = 0;
    for (chr = 0; chr < (len - 1); chr += 2) {
        if (!sal_isxdigit(istr[chr]) || !sal_isxdigit(istr[chr + 1])) {
            LOG_ERROR(BSL_LS_APPL_PACKET,
                      (BSL_META("input data error %s%s\n"), &istr[chr],
                       &istr[chr + 1]));
            SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
        }
        buf[byte++] = xbyte2i(istr[chr], istr[chr + 1]);

        if (byte == buf_size) {
            break;
        }
    }

    *data_size = byte;
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_load_data_from_file(char *fname, bcmpkt_data_buf_t *dbuf)
{
    bcma_io_file_handle_t fh;
    uint32_t i, size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(fname, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    if ((fh = bcma_io_file_open(fname, "r")) == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET,
                  (BSL_META("Open file %s failed\n"), fname));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    size = bcmpkt_tailroom(dbuf);

    for (i = 0; i < size; i++) {
        if (file_load_one_byte(fh, &dbuf->data[i]) < 0)
            break;
    }

    dbuf->data_len = i;

    bcma_io_file_close(fh);

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_packet_generate(bcma_bcmpkt_pktgen_cfg_t *cfg,
                            bcmpkt_data_buf_t *dbuf)
{
    uint8_t *filladdr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf, SHR_E_PARAM);
    SHR_NULL_CHECK(dbuf->data, SHR_E_PARAM);

    if (bcmpkt_tailroom(dbuf) < cfg->len) {
        LOG_ERROR(BSL_LS_APPL_PACKET,
                  (BSL_META("Configure length is too big\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    if(bcmpkt_put(dbuf, cfg->len) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    sal_memset(dbuf->data, 0, cfg->len);
    filladdr = dbuf->data;
    sal_memcpy(filladdr, cfg->dmac, ENET_MAC_SIZE);
    filladdr += ENET_MAC_SIZE;
    sal_memcpy(filladdr, cfg->smac, ENET_MAC_SIZE);
    filladdr += ENET_MAC_SIZE;

    /* Tagged. */
    if (!cfg->untagged) {
        *filladdr++ = cfg->tpid >> 8;
        *filladdr++ = cfg->tpid & 0xff;
        *filladdr++ = (cfg->dei << 4) | (cfg->pcp << 5) | (cfg->vlan >> 8);
        *filladdr++ = cfg->vlan & 0xff;
    }
    *filladdr++ = cfg->ethertype >> 8;
    *filladdr++ = cfg->ethertype & 0xff;

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_packet_payload_fill(cfg, dbuf));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_lmatch_check(const char *dst, const char *src, int size)
{
    int len;
    char *name = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(dst, SHR_E_PARAM);
    SHR_NULL_CHECK(src, SHR_E_PARAM);

    len = sal_strlen(dst);
    if (len >= size) {
        SHR_ALLOC(name, size + 1, "bcmaBcmpktLmatchCheck");
        if (name != NULL) {
            sal_memcpy(name, dst, size);
            name[size] = '\0';
            if (!sal_strcasecmp(name, src)) {
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            }
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
exit:
    SHR_FREE(name);
    SHR_FUNC_EXIT();
}

void
bcma_bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size)
{
    int idx;

    if (data == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer\n")));
        return;
    }

    if (size > 256) {
        size = 256;
    }

    for (idx = 0; idx < size; idx++) {
        if ((idx & 0xf) == 0) {
            shr_pb_printf(pb, "%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            shr_pb_printf(pb, "- ");
        }
        shr_pb_printf(pb, "%02x ", data[idx]);
        if ((idx & 0xf) == 0xf) {
            shr_pb_printf(pb, "\n");
        }
    }
    if ((idx & 0xf) != 0) {
        shr_pb_printf(pb, "\n");
    }
}

int
bcma_bcmpkt_watcher(int unit, int netif_id, bcmpkt_packet_t *packet,
                    void *cookie)
{
    uint32_t *rxpmd;
    bcmpkt_packet_t *pkt = NULL;
    watcher_data_t *wdata = (watcher_data_t *)cookie;
    shr_pb_t *pb;
    uint32_t dev_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_get(packet, &rxpmd));
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));

    /* Display packet Data. */
    if (wdata != NULL && (wdata->debug_mode & WATCHER_DEBUG_SPD)) {
        cli_out("\nWatch information: (interface %d)\n", netif_id);
        pb = shr_pb_create();
        shr_pb_printf(pb, "[RX metadata information]\n");
        bcmpkt_rxpmd_dump(dev_type, rxpmd, BCMPKT_RXPMD_DUMP_F_NONE_ZERO, pb);
        shr_pb_printf(pb, "[RX reasons]\n");
        bcmpkt_rx_reason_dump(dev_type, rxpmd, pb);
        shr_pb_printf(pb, "Packet raw data (%d):\n", BCMPKT_PACKET_LEN(packet));
        bcma_bcmpkt_data_dump(pb, BCMPKT_PACKET_DATA(packet), BCMPKT_PACKET_LEN(packet));
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
    }
    /* Loopback packet Data to TX. */
    if (wdata != NULL && (wdata->debug_mode & WATCHER_DEBUG_LD)) {
        /*   shr_mac_t mac;*/
        int port;

        bcmpkt_packet_claim(packet->unit, packet, &pkt);

        /* Get source port for loopback. */
        SHR_IF_ERR_EXIT
            (bcmpkt_rxpmd_field_get(dev_type, rxpmd,
                                    BCMPKT_RXPMD_SRC_PORT_NUM,
                                    (uint32_t *)&port));

        SHR_IF_ERR_EXIT
            (bcmpkt_fwd_port_set(dev_type, port, pkt));

    /*! FIXME:
     * Currently, MAC loopback doesn't switch MAC Address;
     * If switch address here, traffic will block. Need to double check.
     */
#if 0
        sal_memcpy(mac, packet->data, 6);
        sal_memcpy(packet->data, packet->data + 6, 6);
        sal_memcpy(packet->data + 6, mac, 6);
#endif

        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "Data buf: (Line %d)\n", __LINE__);
            bcmpkt_data_buf_dump(pkt->data_buf, pb);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }

        SHR_IF_ERR_EXIT
            (bcmpkt_tx(unit, netif_id, pkt));
    }

    /* Display RX rate for every 100k packets. */
    if (wdata != NULL && (wdata->debug_mode & WATCHER_DEBUG_SRR)) {
        wdata->rx_packets++;
        if (wdata->rx_packets == 1) {
            wdata->start_time = sal_time_usecs();
        }
        if (wdata->rx_packets == 100000) {
            sal_usecs_t delt_ms = (sal_time_usecs() - wdata->start_time) / 1000;
             /* Show rate when speed no less than 1k pps. */
            if (delt_ms < 100000) {
                cli_out("Netif %d receive rate is %lu PPS \n", netif_id,
                        100000000/delt_ms);
            }
            wdata->rx_packets = 0;
        }
    }

exit:
    if (pkt) {
        bcmpkt_free(pkt->unit, pkt);
        pkt = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_txpmd_init(uint32_t dev_type, uint32_t *txpmd)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_START,
                                BCMPKT_TXPMD_START_IHEADER));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_HEADER_TYPE,
                                BCMPKT_TXPMD_HEADER_TYPE_FROM_CPU));
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd, BCMPKT_TXPMD_UNICAST, 1));
exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_lbhdr_init(uint32_t dev_type, uint32_t *lbhdr)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                BCMPKT_LBHDR_START,
                                BCMPKT_LBHDR_START_CODE));
exit:
    SHR_FUNC_EXIT();
}

void
bcma_bcmpkt_flag_set(uint32_t *flags, uint32_t this_flag, int do_set)
{
    *flags &= ~this_flag;
    if (do_set) {
        *flags |= this_flag;
    }
}

bool
bcma_bcmpkt_flag_status_get(const uint32_t flags, uint32_t this_flag)
{
    return ((flags & this_flag) != 0);
}

