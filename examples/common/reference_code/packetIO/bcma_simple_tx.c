/*! \file bcma_simple_tx.c
 *
 * Purpose: A very simple TX routine demonstrating the use of basic NGSDK
 * TX APIs.
 *
 * Setup: The test assumes that the switch device is configured to send
 * packets between the CPU and switch. No test equipment or external
 * connections are necessary.
 *
 * Ingress port(s): Not applicable.
 *
 * Egress ports(s): At least one port must be up so that the switch will
 * forward the transmitted packets. The actual egress port is determined as
 * the packet passes through the ingress pipeline by the switch device logic.
 * For this test case, it is assumed that packets into the pipeline will
 * be broadcast to all ports in the packet VLAN that are in a "link up"
 * state due to L2 destination lookup failue (DLF).
 *
 * Egress CoS queue: Not applicable.
 *
 * Device: This functionality was developed using a BCM56960 (Tomahawk)
 * device but can be used with any device with no modification to the source.
 *
 * SDK Version: This test was developed and tested using SDK version 0.7.
 *
 * Notes:
 *     CINT Support: The current revision of the "C" interpreter does not
 *     support asynchronous call back functions at this time so it is not
 *     possible to verify Tx functionality using CINT. CINT does not
 *     support any bcmpkt_XX() functions at this time.
 *
 *     Network Device Initialization: This sample routine assumes that the
 *     network devices referred to by "netif_id" has been initialized, either
 *     by calls to the appropriate APIs or with the CLI.
 *     CLI commands:
 *     BCMLT.0> pktdev init
 *     BCMLT.0> pktdev chan queuemap 1 highword=0xffff lowword=0xffffffff
 *
 * Verification method: This functionality was verified by temporarily
 * integrating this function into the NGSDK source.
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

/* For cli_out() */
#include <bsl/bsl.h>

/* Basic data types and error codes */
#include <shr/shr_types.h>
#include <shr/shr_error.h>

/* Function prototypes for:
 *   bcmpkt_tx                   */
#include <bcmpkt/bcmpkt_net.h>

/* Function prototypes for:
 *   bcmpkt_bpool_create
 *   bcmpkt_alloc
 *   bcmpkt_reserve
 *   bcmpkt_free                 */
#include <bcmpkt/bcmpkt_buf.h>

/* Function prototypes for global functions defined here */
#include <bcma/customer/bcma_simple_tx.h>

/* For this example, always use a default TPID of 0x8100 in the header */
#define TPID          0x8100

/* For this example, always use a default ethertype of 0x0806 in the
 * header */
#define ETHERTYPE     0x0806

/* For this example, 32 bytes need to be reserved for packet metadata */
#define RESERVE_SPACE_HEAD 32

/* For this example, no reserve space is needed at end of the packet. */
#define RESERVE_SPACE_END  0

/* Compute total additional space to reserve in addition to the packet
 * itself
 */
#define RESERVE_SPACE      (RESERVE_SPACE_HEAD + RESERVE_SPACE_END)

/*!
 * \brief Simple example for packet transmission.
 *
 * The \c bcma_simple_tx function injects a packet from the CPU into the
 * ingress pipeline.
 *
 * \param [in] unit Unit number to send packet to.
 * \param [in] netif_id Network interface to use for packet transmission.
 * \param [in] packet_size Size of packet (including head) in bytes.
 * \param [in] vlan VLAN tag (includes priority, CFI and VLAN ID).
 * \param [in] dmac Destination MAC address.
 * \param [in] smac Source MAC address.
 *
 * \return Returns SHR_E_NONE on success or propagates error status from
 * failed ngsdk call.
 */
shr_error_t
bcma_simple_tx(int unit, int netif_id, int packet_size, int vlan, shr_mac_t dmac,
               shr_mac_t smac)
{
    const int   max_packet_size = 9216;
    const int   buffer_count = 512;

    bcmpkt_packet_t *pkt = NULL;
    shr_error_t rv;
    uint32_t    flags = 0;
    uint8_t    *data;
    uint8_t    *data_end;
    uint8_t    *data_ptr;
    uint8_t     fill;

    cli_out("%s: Transmit %d byte packet on interface %d\n", __func__, packet_size,
            netif_id);
    /*
     * Create a buffer pool for this unit. There is also a "shared" buffer
     * pool that is accessed with unit=-1. Only needs to  be called once.
     * In this case, create "buffer_count" (512) packet buffers, each
     * "max_packet_size" (9216) bytes in length.
     */
    if (SHR_FAILURE(rv = bcmpkt_bpool_create(unit, max_packet_size, buffer_count))) {
        if (rv == SHR_E_EXISTS) {
            /* returns SHR_E_EXISTS if pool already created */
            cli_out("%s:%d; Skipping buffer pool creation, pool already allocated\n",
                    __func__, __LINE__);
        } else {
            cli_out("%s() FAILED[bcmpkt_bpool_create]: %s\n", __func__, shr_errmsg(rv));
            return rv;
        }
    }
    /*
     * Allocate packet object including data buffer and private header space
     * (internal use only), and format Packet MetaData structure (including
     * Higig information).  The pointer to "pkt" must initially be NULL. Unit
     * may be -1 if you want to use the shared buffer.
     */
    if (SHR_FAILURE(rv = bcmpkt_alloc(unit, packet_size + RESERVE_SPACE, flags, &pkt))) {
        cli_out("%s() FAILED[bcmpkt_alloc]: %s\n", __func__, shr_errmsg(rv));
        return rv;
    }

    /*
     * Reserve space at the beginning of the packet for possible encapsulation,
     * additional tags and packet metadata. In this case, we just need to
     * allocate space for packet metadata (PMD).
     */
    if (NULL == bcmpkt_reserve(pkt->data_buf, RESERVE_SPACE_HEAD)) {
        cli_out("%s() FAILED[bcmpkt_reserve]: %s\n", __func__,
                shr_errmsg(SHR_E_MEMORY));
        return SHR_E_MEMORY;
    }

    /* Set packet size in packet structure */
    BCMPKT_PACKET_LEN(pkt) = packet_size;

    /* Initialize packet header and payload */
    data = BCMPKT_PACKET_DATA(pkt);     /* Get start of packet */

    /* Copy destination MAC into packet */
    sal_memcpy(&data[0], dmac, sizeof(shr_mac_t));

    /* Copy source MAC into packet */
    sal_memcpy(&data[sizeof(shr_mac_t)], smac, sizeof(shr_mac_t));

    /* The rest of the header follows MAC addresses */
    data_ptr = &data[sizeof(shr_mac_t) * 2];

    /* Set TPID in header */
    *data_ptr++ = TPID >> 8;
    *data_ptr++ = TPID & 0xff;

    /* Set VLAN ID in header */
    *data_ptr++ = vlan >> 8;
    *data_ptr++ = vlan & 0xff;

    /* Set Ethertype in header */
    *data_ptr++ = ETHERTYPE >> 8;
    *data_ptr++ = ETHERTYPE & 0xff;

    /* Initialize payload */
    data_end = &data[packet_size];      /* End of packet */
    for (fill = 0; data_ptr < data_end; fill = (fill + 1) & 0xFF) {
        *data_ptr++ = fill;
    }

    /*
     * Transmit packet
     *
     * Because packet is going to go into the ingress pipeline, it is not
     * necessary to provide packet metadata configuration. The switch will
     * determine the egress port(s) for the outgoing packet.
     */
    if (SHR_FAILURE(rv = bcmpkt_tx(unit, netif_id, pkt))) {
        cli_out("%s() FAILED[bcmpkt_tx]: %s\n", __func__, shr_errmsg(rv));
        (void) bcmpkt_free(unit, pkt);
        return rv;
    }

    /* Free packet resources */
    if (SHR_FAILURE(rv = bcmpkt_free(unit, pkt))) {
        cli_out("%s() FAILED[bcmpkt_free]: %s\n", __func__, shr_errmsg(rv));
        return rv;
    }
    return SHR_E_NONE;
}
