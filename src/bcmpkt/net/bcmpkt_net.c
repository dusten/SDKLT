/*! \file bcmpkt_net.c
 *
 * NET access.
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

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_unet.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_buf.h>

#define TXPMD_START_IHEADER           2
#define TXPMD_HEADER_TYPE_FROM_CPU    1
#define TXPMD_UNICAST                 1

#define BSL_LOG_MODULE BSL_LS_BCMPKT_NET

static bcmpkt_net_t nvect[BCMPKT_NET_DRV_T_COUNT];

int
bcmpkt_net_drv_register(bcmpkt_net_t *net_drv)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(net_drv, SHR_E_PARAM);

    if (net_drv->driver_type >= BCMPKT_NET_DRV_T_COUNT ||
        net_drv->driver_type <= BCMPKT_NET_DRV_T_AUTO) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (net_drv->inited != 1) {
        SHR_RETURN_VAL_EXIT(SHR_E_CONFIG);
    }
    if (nvect[net_drv->driver_type].inited) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    nvect[net_drv->driver_type] = *net_drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_net_drv_unregister(bcmpkt_net_drv_types_t type)
{
    bcmpkt_dev_drv_types_t dev_drv_type = type - BCMPKT_NET_DRV_T_NONE;
    bcmpkt_unet_drv_types_t unet_drv_type = type - BCMPKT_NET_DRV_T_NONE;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (type >= BCMPKT_NET_DRV_T_COUNT ||
        type <= BCMPKT_NET_DRV_T_AUTO) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if ((type < BCMPKT_NET_DRV_T_TPKT && bcmpkt_dev_drv_inuse(dev_drv_type)) ||
        (type >= BCMPKT_NET_DRV_T_TPKT && bcmpkt_unet_drv_inuse(unet_drv_type)))
        {
        SHR_RETURN_VAL_EXIT(SHR_E_BUSY);
    }

    if (nvect[type].inited) {
        sal_memset(&nvect[type], 0, sizeof(bcmpkt_net_t));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpkt_net_drv_get(int unit, bcmpkt_net_t **net)
{
    uint32_t drv_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));

    if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmpkt_unet_drv_type_get(unit, &drv_type));
    }

    *net = &nvect[drv_type];
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_get(bcmpkt_packet_t *packet, uint32_t **rxpmd)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);

    *rxpmd = packet->pmd.rxpmd;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_get(bcmpkt_packet_t *packet, uint32_t **txpmd)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.txpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(txpmd, SHR_E_PARAM);

    *txpmd = packet->pmd.txpmd;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_get(bcmpkt_packet_t *packet, uint32_t **lbhdr)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.lbhdr, SHR_E_PARAM);
    SHR_NULL_CHECK(lbhdr, SHR_E_PARAM);

    *lbhdr = packet->pmd.lbhdr;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_higig_get(bcmpkt_packet_t *packet, uint32_t **hg_hdr)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->pmd.higig, SHR_E_PARAM);
    SHR_NULL_CHECK(hg_hdr, SHR_E_PARAM);

    *hg_hdr = packet->pmd.higig;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_pmd_format(bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    packet->pmd.rxpmd = packet->pmd.data;
    packet->pmd.txpmd = packet->pmd.rxpmd + BCMPKT_RCPU_RXPMD_SIZE / 4;
    packet->pmd.higig = packet->pmd.txpmd + BCMPKT_RCPU_TX_MH_SIZE / 4;
    packet->pmd.lbhdr = packet->pmd.higig + BCMPKT_RCPU_TX_MH_SIZE / 4;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_fwd_port_set(bcmdrd_dev_type_t dev_type, int port,
                    bcmpkt_packet_t *packet)
{
    uint32_t *txpmd;
    uint32_t val;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_get(packet, &txpmd));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_get(dev_type, txpmd, BCMPKT_TXPMD_START, &val));

    /* If not initialized. */
    if (val == 0) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                     BCMPKT_TXPMD_START, TXPMD_START_IHEADER));
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                     BCMPKT_TXPMD_HEADER_TYPE,
                                     TXPMD_HEADER_TYPE_FROM_CPU));
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                     BCMPKT_TXPMD_UNICAST, TXPMD_UNICAST));
    }
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                 BCMPKT_TXPMD_LOCAL_DEST_PORT, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_fwd_type_set(bcmpkt_fwd_types_t type, bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (type >= BCMPKT_FWD_T_COUNT) {
        LOG_ERROR(BSL_LS_BCMPKT_NET, (BSL_META("Unknown type %08x\n"), type));
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    packet->type = type;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_register(int unit, int netif_id, uint32_t flags,
                   bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);
    SHR_NULL_CHECK(net->rx_register, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (net->rx_register(unit, netif_id, flags, cb_func, cb_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                     void *cb_data)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);
    SHR_NULL_CHECK(net->rx_unregister, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (net->rx_unregister(unit, netif_id, cb_func, cb_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);
    SHR_NULL_CHECK(net->tx, SHR_E_CONFIG);
    SHR_IF_ERR_EXIT
        (net->tx(unit, netif_id, packet));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_net_drv_type_get(int unit, bcmpkt_net_drv_types_t *type)
{
    bcmpkt_net_t *net;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_get(unit, &net));
    SHR_NULL_CHECK(net, SHR_E_CONFIG);

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    *type = net->driver_type;
exit:
    SHR_FUNC_EXIT();

}

int
bcmpkt_data_dump(shr_pb_t *pb, const uint8_t *data, int size)
{
    int idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

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
exit:
    SHR_FUNC_EXIT();
}

/* Debug. */
void
bcmpkt_data_buf_dump(bcmpkt_data_buf_t *dbuf, shr_pb_t *pb)
{
    shr_pb_printf(pb, "head - %p\n", dbuf->head);
    shr_pb_printf(pb, "data - %p\n", dbuf->data);
    shr_pb_printf(pb, "len - %"PRIu32"\n", dbuf->len);
    shr_pb_printf(pb, "data_len - %"PRIu32"\n", dbuf->data_len);
    shr_pb_printf(pb, "refcnt - %d\n", dbuf->ref_count);
    bcmpkt_data_dump(pb, dbuf->data, dbuf->data_len);
}
