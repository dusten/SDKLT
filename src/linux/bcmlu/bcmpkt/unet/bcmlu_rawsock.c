/*! \file bcmlu_rawsock.c
 *
 * Raw socket wrapper.
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

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_types.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>
#include <sal/sal_assert.h>
#include <shr/shr_types.h>
#include <bsl/bsl.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_unet.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmdrd/bcmdrd_feature.h>

#include "bcmlu_rawsock.h"

#define BSL_LOG_MODULE          BSL_LS_SYS_UNET

#define TXPMD_START_HIGIG       3
#define RX_POLL_TIMEOUT         1000

typedef struct bcmlu_rawsock_s bcmlu_rawsock_t;

typedef int  (*bcmlu_rawsock_read_f)(bcmlu_rawsock_t *rawsock);
typedef int  (*bcmlu_rawsock_send_f)(bcmlu_rawsock_t *rawsock, uint8_t *data,
                                     uint32_t len);
/*!
 * \brief Rawsock handler structure.
 */
struct bcmlu_rawsock_s {

    /*! Host unit number. */
    int unit;

    /*! File descriptor of socket. */
    int fd;

    /*! Host network interface configuration. */
    bcmpkt_netif_t netif;

    /*! RX thread ID. */
    sal_thread_t rx_pid;

    /*! Break RX loop to terminate RX thread. */
    bool rx_break;

    /*! Timeout for poll function. */
    uint32_t poll_timeout;

    /*! Receive function. */
    bcmlu_rawsock_read_f read;

    /*! Transmit function. */
    bcmlu_rawsock_send_f send;

    /*! RX callback handle. */
    bcmpkt_rx_cb_info_t *cb_info;

    /*! Received valid packets on the socket. */
    uint32_t rx_packets;

    /*! Dropped invalid packets on the socket. */
    uint32_t rx_drops;

    /*! Packet dropped due to invalid packet length. */
    uint32_t pkt_len_err;

    /*! Non-RCPU packet dropped due to RCPU encap configured on Netif. */
    uint32_t non_rcpu_pkt;

    /*! RCPU packet dropped due to header check fail. */
    uint32_t rcpu_hdr_err;

    /*! RCPU packet dropped due to length check fail. */
    uint32_t rcpu_len_err;

    /*! Received bytes on the socket. */
    uint32_t rx_bytes;

    /*! Transmit packets on the socket. */
    uint32_t tx_packets;

    /*! Dropped packets to the socket. */
    uint32_t tx_drops;

    /*! Transmit bytes no the socket. */
    uint32_t tx_bytes;

} ;

static bcmlu_rawsock_t *rawsock_handles[BCMDRD_CONFIG_MAX_UNITS][BCMPKT_NETIF_MAX_IDS];

static bcmlu_rawsock_t *
bcmlu_rawsock_handle_get(int unit, int netif_id)
{
    return rawsock_handles[unit][netif_id];
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
    uint8_t *mac_addr = NULL;
    bcmpkt_rcpu_hdr_t *rhdr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));

    rhdr = bcmpkt_rcpu_hdr(unit);

    mac_addr = BCMPKT_PACKET_DATA(packet);
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
    if (BCMPKT_PACKET_DATA(packet) < packet->data_buf->head + hdr_size) {
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
        mac_addr = BCMPKT_PACKET_DATA(packet);
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
    sal_memcpy(data, mac_addr, 12);
    data[12] = rhdr->tpid >> 8;
    data[13] = rhdr->tpid & 0xFF;
    data[14] = rhdr->vlan >> 8;
    data[15] = rhdr->vlan & 0xFF;
    data[16] = rhdr->ethertype >> 8;
    data[17] = rhdr->ethertype & 0xFF;
    data[18] = rhdr->signature >> 8;
    data[19] = rhdr->signature & 0xFF;
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
bcmlu_rawsock_send(bcmlu_rawsock_t *rawsock, uint8_t *data, uint32_t len)
{
    int bytes;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    bytes = send(rawsock->fd, data, len, 0);
    if (bytes <= 0) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Call send failed (%s)\n"),
              strerror(errno)));
    }
    LOG_VERBOSE(BSL_LS_SYS_TX,
                (BSL_META_U(rawsock->unit,
                            "Send: len=%"PRIu32"(%d)\n"), len, bytes));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    bcmlu_rawsock_t *rawsock;
    uint32_t data_len;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMPKT_PACKET_DATA(packet), SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit, "Netif %d doesn't support raw socket\n"),
              netif_id));
    }

    data_len = BCMPKT_PACKET_LEN(packet);
    if (data_len > rawsock->netif.max_frame_size ||
        data_len < BCMPKT_FRAME_SIZE_MIN) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit,
                         "Packet size %"PRIu32" is out of valid range [%d, %"PRIu32"]\n"),
              BCMPKT_PACKET_LEN(packet), BCMPKT_FRAME_SIZE_MIN,
              rawsock->netif.max_frame_size));
    }

    if (packet->type == BCMPKT_FWD_T_NORMAL &&
        (rawsock->netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP)) {
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
        (rawsock->send(rawsock, BCMPKT_PACKET_DATA(packet),
                        BCMPKT_PACKET_LEN(packet)));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_packet_validate(bcmlu_rawsock_t *rawsock, bcmpkt_packet_t *packet)
{
    uint32_t pkt_len, meta_size = 0;
    uint8_t *data = NULL;

    bcmpkt_rcpu_hdr_t *rhdr = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    rhdr = bcmpkt_rcpu_hdr(rawsock->unit);
    assert(rhdr);

    data = BCMPKT_PACKET_DATA(packet);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    /* Perform sanity check. */
    if (BCMPKT_PACKET_LEN(packet) > rawsock->netif.max_frame_size +
                                    BCMPKT_RCPU_RX_ENCAP_SIZE ||
        BCMPKT_PACKET_LEN(packet) < BCMPKT_FRAME_SIZE_MIN) {
        rawsock->pkt_len_err++;
        rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);
    }
    /* RCPU validation. */
    if (data[12] == (rhdr->tpid >> 8) &&
        data[13] == (rhdr->tpid & 0xFF) &&
        data[14] == (rhdr->vlan >> 8) &&
        data[15] == (rhdr->vlan & 0xFF) &&
        data[16] == (rhdr->ethertype >> 8) &&
        data[17] == (rhdr->ethertype & 0xFF)) {
        if (data[18] != (rhdr->signature >> 8) ||
            data[19] != (rhdr->signature & 0xFF) ||
            data[20] != BCMPKT_RCPU_OP_RX ||
            data[21] != BCMPKT_RCPU_F_MODHDR) {
            rawsock->rcpu_hdr_err++;
            rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(rawsock->unit,
                             "RCPU validation failure:\n"
                             "Signature %04x, OPCODE %02x, Flags %02x\n"),
                  (data[18] << 8) + data[19], data[20], data[21]));
        }
        pkt_len = data[25] + (data[24] << 8);
        meta_size = data[28];
        if (pkt_len + meta_size != BCMPKT_PACKET_LEN(packet) - sizeof(*rhdr)) {
            rawsock->rcpu_len_err++;
            rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(rawsock->unit,
                             "Length error: packet length %u bytes\n"
                             "meta length %u bytes\n"
                             "RCPU header length %u bytes\n"
                             "Snap length %u bytes\n"),
                  pkt_len, meta_size, BCMPKT_RCPU_HDR_LEN,
                  BCMPKT_PACKET_LEN(packet)));
        }

        data += sizeof(*rhdr);
        /* Copy RXPMD data. */
        sal_memcpy(packet->pmd.rxpmd, data, meta_size);
        data += meta_size;
        bcmpkt_pull(packet->data_buf, meta_size + sizeof(*rhdr));
    } else { /* Handle as non-RCPU packets. */
        /*
         * Drop non-RCPU packets if BCMPKT_NETIF_F_RCPU_ENCAP flag was
         * configured.
         */
        if (rawsock->netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP) {
            rawsock->non_rcpu_pkt++;
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_packet_handle(bcmlu_rawsock_t *rawsock, bcmpkt_packet_t *packet)
{
    int bytes;
    uint32_t max_frame_size;
    bcmpkt_bpool_status_t bpool_status;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));
    max_frame_size = bpool_status.buf_size - sizeof(bcmpkt_data_buf_t);

    do {
        if (!packet->data_buf) {
            /* Allocate buffer from shared buffer pool. */
            SHR_IF_ERR_EXIT
                (bcmpkt_data_buf_alloc(BCMPKT_BPOOL_SHARED_ID, max_frame_size, &packet->data_buf));
        } else {
            packet->data_buf->data = packet->data_buf->head;
            packet->data_buf->data_len = 0;
        }
        bytes = recv(rawsock->fd, BCMPKT_PACKET_DATA(packet),
                     max_frame_size, MSG_DONTWAIT);
        if (bytes == -1) {
            if (errno == EAGAIN || errno == EINTR) {
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(rawsock->unit, "%s\n"),
                           strerror(errno)));
                SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
            }
        } else if (bytes == 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
        BCMPKT_PACKET_LEN(packet) = bytes;
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_t *pb;
            pb = shr_pb_create();
            shr_pb_printf(pb, "Received packet (len=%u):\n", bytes);
            bcmpkt_data_dump(pb, BCMPKT_PACKET_DATA(packet), bytes);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(rawsock->unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }

        if (SHR_FAILURE(bcmlu_rawsock_packet_validate(rawsock, packet))){
            continue;
        }

        rawsock->rx_packets++;
        rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);

        /* BCMPKT API only supports one callback. */
        if (rawsock->cb_info != NULL && rawsock->cb_info->cb_func &&
            !rawsock->cb_info->cb_pending) {
            rawsock->cb_info->cb_func(rawsock->unit, rawsock->netif.id, packet,
                                      rawsock->cb_info->cb_data);
        }

        /*!
         * If this packet is cloned somewhere,
         * release the data buf and alloc new one for receiving
         */
        if (packet->data_buf && packet->data_buf->ref_count > 1) {
            SHR_IF_ERR_EXIT
                (bcmpkt_data_buf_free(packet->unit, packet->data_buf));
            packet->data_buf = NULL;
        }

        if (rawsock->rx_break) {
            SHR_RETURN_VAL_EXIT(SHR_E_DISABLED);
        }
    } while(TRUE);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_read(bcmlu_rawsock_t *rawsock)
{
    int rv;
    struct pollfd pfd;
    bcmpkt_bpool_status_t bpool_status;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));

    /* Allocate buffer from shared buffer pool. */
    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID, bpool_status.buf_size - sizeof(bcmpkt_data_buf_t),
                      0, &packet));

    sal_memset(&pfd, 0, sizeof(pfd));
    pfd.fd = rawsock->fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    while (!rawsock->rx_break) {

        pfd.revents = 0;
        rv = poll(&pfd, 1, rawsock->poll_timeout);
        if (rv == -1  && errno != EINTR) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit, "Polling failed: %s\n"),
                       strerror(errno)));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLHUP) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit,
                                  "Hangup on packet socket\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLNVAL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit,
                                  "Invalid polling request\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLERR) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit,
                                  "Polling error\n")));
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLIN) {
            rv = bcmlu_rawsock_packet_handle(rawsock, packet);
            if (rv == SHR_E_DISABLED) {
                SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            } else if(SHR_FAILURE(rv)) {
                SHR_RETURN_VAL_EXIT(rv);
            }
        }
        if (rawsock->rx_break) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

exit:
    if (packet) {
        bcmpkt_free(packet->unit, packet);
        packet = NULL;
    }
    SHR_FUNC_EXIT();
}

static void
bcmlu_rawsock_rx_thread(void *p)
{
    bcmlu_rawsock_t *rawsock = (bcmlu_rawsock_t *)p;

    if (rawsock == NULL) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL pointer\n")));
        return;
    }

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(rawsock->unit, "Enter RX thread\n")));
    rawsock->read(rawsock);

    rawsock->rx_pid = SAL_THREAD_ERROR;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit, "RX thread closed (unit %d netif %d)\n"),
                 rawsock->unit, rawsock->netif.id));
    sal_thread_exit(0);
}

static int
iface_get_id(int sock_fd, const char *device)
{
    struct ifreq  ifr;

    sal_memset(&ifr, 0, sizeof(struct ifreq));
    sal_strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name) - 1);

    if (ioctl(sock_fd, SIOCGIFINDEX, &ifr) == -1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("SIOCGIFINDEX failure: %s"), strerror(errno)));
        return -1;
    }

    return ifr.ifr_ifindex;
}

static int
bcmlu_rawsock_exists(int unit, int netif_id)
{
    if (!bcmdrd_dev_exists(unit)) {
        return 0;
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        return 0;
    }

    return (rawsock_handles[unit][netif_id] != NULL);
}

static int
bcmlu_rawsock_netif_bind(int sock_fd, bcmlu_rawsock_t *rawsock)
{
    int err;
    socklen_t errlen = sizeof(err);
    struct sockaddr_ll sll;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(rawsock, SHR_E_PARAM);

    sal_memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family    = PF_PACKET;
    sll.sll_protocol  = htons(ETH_P_ALL);
    sll.sll_ifindex   = iface_get_id(sock_fd, rawsock->netif.name);
    if (sll.sll_ifindex < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }
    sll.sll_halen = 6;
    sal_memcpy(sll.sll_addr, rawsock->netif.mac_addr, 6);

    err = bind(sock_fd, (struct sockaddr *) &sll, sizeof(struct sockaddr_ll));
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit,
                         "bind failed (%s)\n"), strerror(errno)));
    }
    err = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &err, &errlen);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit,
                         "getsockopt failed (%s)\n"), strerror(errno)));
    }

    if (err > 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit,
                         "getsockopt SO_ERROR (%s)\n"), strerror(err)));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit,
                            "Bind socket (%d) to netif (%s) success\n"),
                 sock_fd, rawsock->netif.name));

exit:
    SHR_FUNC_EXIT();
}

/* Create raw socket on a netif. */
static int
bcmlu_rawsock_setup(bcmlu_rawsock_t *rawsock)
{
    char name[128];
    int err;
    int sock_fd = -1;
    struct ifreq ifr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(rawsock, SHR_E_PARAM);

    /* Create socket */
    sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_fd == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Create socket failed (%s)\n"),
              strerror(errno)));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit,
                            "Create socket (%d) successfully\n"),
                 sock_fd));

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strncpy(ifr.ifr_name, rawsock->netif.name, sizeof(ifr.ifr_name) - 1);
    err = ioctl(sock_fd, SIOCGIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCGIFFLAGS error\n")));
    }

#if defined(IFF_PROMISC) && defined(IFF_UP) && defined(IFF_RUNNING)
    /* Set interface configuration. */
    ifr.ifr_flags |= (IFF_PROMISC | IFF_UP | IFF_RUNNING);
    err = ioctl(sock_fd, SIOCSIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCSIFFLAGS error\n")));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit, "Open netif %s\n"),
                 rawsock->netif.name));
#endif

    /*! Bind socket to the netif. */
    SHR_IF_ERR_EXIT
        (bcmlu_rawsock_netif_bind(sock_fd, rawsock));

    rawsock->fd = sock_fd;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit, "Bond socket %d with netif %s\n"),
                 rawsock->fd, rawsock->netif.name));

    rawsock->send = bcmlu_rawsock_send;
    rawsock->read = bcmlu_rawsock_read;

    sal_sprintf(name, "Bcmpkt_raw_socket_rx_%s", rawsock->netif.name);
    rawsock->rx_pid = sal_thread_create(name, 0x1000, -1,
                                        bcmlu_rawsock_rx_thread,
                                        (void*)rawsock);
    if (rawsock->rx_pid == SAL_THREAD_ERROR) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(rawsock->unit,
                                        "Could not start RX thread!")));
    }

    exit:
    if (SHR_FUNC_ERR() && sock_fd >= 0) {
        close(sock_fd);
    }
    SHR_FUNC_EXIT();

}

static int
bcmlu_rawsock_socket_destroy(bcmlu_rawsock_t *rawsock)
{
    int err;
    struct ifreq ifr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(rawsock, SHR_E_PARAM);

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strncpy(ifr.ifr_name, rawsock->netif.name, sizeof(ifr.ifr_name) - 1);
    err = ioctl(rawsock->fd, SIOCGIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCGIFFLAGS error\n")));
    }

#if defined(IFF_PROMISC) && defined(IFF_UP) && defined(IFF_RUNNING)
    /* Disable netif. */
    ifr.ifr_flags &= (~IFF_PROMISC) & (~IFF_RUNNING) & (~IFF_UP);
    err = ioctl(rawsock->fd, SIOCSIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCSIFFLAGS error\n")));
    }
#endif

exit:
    if (rawsock->fd >= 0) {
        close(rawsock->fd);
        rawsock->fd = -1;
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_create(int unit, int netif_id)
{
    bcmlu_rawsock_t *rawsock = NULL;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    if (bcmlu_rawsock_handle_get(unit, netif_id) != NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "UNET exists on the netif\n")));
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_netif_get(unit, netif_id, &netif));

    /* RCPU encapsulation is requested for UNET. */
    if (!(netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP)) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "The netif should support RCPU header\n")));
    }

    SHR_ALLOC(rawsock, sizeof(*rawsock), "bcmluRawSocketCreate");
    SHR_NULL_CHECK(rawsock, SHR_E_MEMORY);

    sal_memset(rawsock, 0, sizeof(*rawsock));

    rawsock->unit = unit;
    /* Init to invalid value. */
    rawsock->fd = -1;
    rawsock->poll_timeout = RX_POLL_TIMEOUT;
    sal_memcpy(&rawsock->netif, &netif, sizeof(netif));

    SHR_IF_ERR_EXIT
        (bcmlu_rawsock_setup(rawsock));

    rawsock_handles[unit][netif.id] = rawsock;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(rawsock);
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_destroy(int unit, int netif_id)
{
    int i = 0;
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Not found Raw Socket on netif %d\n"),
                     netif_id));
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    if (rawsock->rx_pid != SAL_THREAD_ERROR) {
        rawsock->rx_break = TRUE;
        while (rawsock->rx_pid != SAL_THREAD_ERROR) {
            sal_usleep(100);
            i++;
            if (i > 10000) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Break loop failed\n")));
                break;
            }
        }
        rawsock->rx_break = FALSE;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Break waiting %d times\n"), i));
    }

    bcmlu_rawsock_socket_destroy(rawsock);

    SHR_FREE(rawsock->cb_info);
    SHR_FREE(rawsock);

    /* Set to NULL after free. */
    rawsock_handles[unit][netif_id] = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_stats_dump(int unit, int netif_id, shr_pb_t *pb)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    /* The netif_id = -1 means for all TXRX stats.  */
    if (netif_id == -1) {
        int id = 0;
        while (id < BCMPKT_NETIF_MAX_IDS) {
            rawsock = bcmlu_rawsock_handle_get(unit, id++);
            if (rawsock == NULL ) {
                continue;
            }
            rawsock->rx_drops = rawsock->pkt_len_err  +
                                rawsock->non_rcpu_pkt +
                                rawsock->rcpu_hdr_err +
                                rawsock->rcpu_len_err;

            shr_pb_printf(pb, "\tRX received %"PRIu32" (%"PRIu32" bytes)\n"
                          "\tRX dropped %"PRIu32"\n"
                          "\t    Packet length check fail %"PRIu32"\n"
                          "\t    Non RCPU packet %"PRIu32"\n"
                          "\t    RCPU header check fail %"PRIu32"\n"
                          "\t    RCPU length check fail %"PRIu32"\n"
                          "\tTX Transmit %"PRIu32" (%"PRIu32" bytes)\n"
                          "\tTX dropped %"PRIu32"\n",
                          rawsock->rx_packets, rawsock->rx_bytes,
                          rawsock->rx_drops, rawsock->pkt_len_err,
                          rawsock->non_rcpu_pkt, rawsock->rcpu_hdr_err,
                          rawsock->rcpu_len_err, rawsock->tx_packets,
                          rawsock->tx_bytes, rawsock->tx_drops);
        }
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_UNAVAIL,
             (BSL_META_U(unit, "Netif %d doesn't support Tpacket\n"),
              netif_id));
    }

    rawsock->rx_drops = rawsock->pkt_len_err  +
                        rawsock->non_rcpu_pkt +
                        rawsock->rcpu_hdr_err +
                        rawsock->rcpu_len_err;

    shr_pb_printf(pb, "\tRX received %"PRIu32" (%"PRIu32" bytes)\n"
                  "\tRX dropped %"PRIu32"\n"
                  "\t    Packet length check fail %"PRIu32"\n"
                  "\t    Non RCPU packet %"PRIu32"\n"
                  "\t    RCPU header check fail %"PRIu32"\n"
                  "\t    RCPU length check fail %"PRIu32"\n"
                  "\tTX Transmit %"PRIu32" (%"PRIu32" bytes)\n"
                  "\tTX dropped %"PRIu32"\n",
                  rawsock->rx_packets, rawsock->rx_bytes, rawsock->rx_drops,
                  rawsock->pkt_len_err, rawsock->non_rcpu_pkt,
                  rawsock->rcpu_hdr_err, rawsock->rcpu_len_err,
                  rawsock->tx_packets, rawsock->tx_bytes, rawsock->tx_drops);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_stats_clear(int unit, int netif_id)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    /* The netif_id = -1 means for all TXRX stats.  */
    if (netif_id == -1) {
        int id = 0;
        while (id < BCMPKT_NETIF_MAX_IDS) {
            rawsock = bcmlu_rawsock_handle_get(unit, id++);
            if (rawsock == NULL) {
                continue;
            }

            rawsock->rx_packets = 0;
            rawsock->rx_bytes = 0;
            rawsock->rx_drops = 0;
            rawsock->tx_packets = 0;
            rawsock->tx_bytes = 0;
            rawsock->tx_drops = 0;
        }
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_VERBOSE_MSG_EXIT
            (SHR_E_UNAVAIL,
             (BSL_META_U(unit, "Netif %d doesn't support Raw Socket\n"),
              netif_id));
    }

    rawsock->rx_packets = 0;
    rawsock->rx_bytes = 0;
    rawsock->rx_drops = 0;
    rawsock->tx_packets = 0;
    rawsock->tx_bytes = 0;
    rawsock->tx_drops = 0;

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function do nothing now. Leave for future purpose.
 */
static int
bcmlu_rawsock_init(int unit, const bcmpkt_unet_init_t *init_cfg)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(init_cfg, SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_cleanup(int unit)
{
    int netif_id;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    for (netif_id = 1; netif_id < BCMPKT_NETIF_MAX_IDS; netif_id++) {
        if (bcmlu_rawsock_exists(unit, netif_id)) {
            bcmlu_rawsock_destroy(unit, netif_id);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_rx_register(int unit, int netif_id, uint32_t flags,
                          bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit, "Netif %d doesn't support Raw Socket\n"),
              netif_id));
    }
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (rawsock->cb_info != NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Overwriting callback function is prohibit\n")));
    }
    SHR_ALLOC(rawsock->cb_info, sizeof(bcmpkt_rx_cb_info_t), "bcmluRawSocketRxReg");
    SHR_NULL_CHECK(rawsock->cb_info, SHR_E_MEMORY);

    rawsock->cb_info->flags   = flags;
    rawsock->cb_info->cb_func = cb_func;
    rawsock->cb_info->cb_data = cb_data;
    rawsock->cb_info->cb_pending = FALSE;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                            void *cb_data)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNIT);
    }

    if (!BCMPKT_NETIF_VALID(netif_id)) {
        SHR_RETURN_VAL_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit, "Netif %d doesn't support RawSocket\n"),
              netif_id));
    }

    if (rawsock->cb_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback not registered\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (rawsock->cb_info->cb_func != cb_func) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback doesn't match\n")));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    rawsock->cb_info->cb_pending = TRUE;
    sal_usleep(100000);
    SHR_FREE(rawsock->cb_info);
    LOG_VERBOSE(BSL_LS_SYS_RX,
                (BSL_META_U(unit, "Unregister succeed\n")));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_rawsock_attach(void)
{
    bcmpkt_unet_t unet_vect = {
        .inited = 1,
        .driver_name = "RAWSOCK",
        .driver_type = BCMPKT_UNET_DRV_T_RAWSOCK,
        .init = bcmlu_rawsock_init,
        .cleanup = bcmlu_rawsock_cleanup,
        .create = bcmlu_rawsock_create,
        .destroy = bcmlu_rawsock_destroy,
        .stats_dump = bcmlu_rawsock_stats_dump,
        .stats_clear = bcmlu_rawsock_stats_clear,
        .exists = bcmlu_rawsock_exists
    };
    bcmpkt_net_t net_vect = {
        .inited = 1,
        .driver_name = "RAWSOCK",
        .driver_type = BCMPKT_UNET_DRV_T_RAWSOCK,
        .rx_register = bcmlu_rawsock_rx_register,
        .rx_unregister = bcmlu_rawsock_rx_unregister,
        .tx = bcmlu_rawsock_tx
    };

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_unet_drv_register(&unet_vect), SHR_E_EXISTS);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_net_drv_register(&net_vect), SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_rawsock_detach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_unregister(BCMPKT_NET_DRV_T_RAWSOCK));

    SHR_IF_ERR_EXIT
        (bcmpkt_unet_drv_unregister(BCMPKT_UNET_DRV_T_RAWSOCK));

exit:
    SHR_FUNC_EXIT();
}

