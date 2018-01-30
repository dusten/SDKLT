/*! \file bcma_simple_rx.c
 *
 * Purpose: A very simple RX routine demonstrating the use of basic NGSDK
 * rx APIs.
 *
 * Description: This test registers a simple callback function that prints
 * a packet when it is received by the network interface.
 *
 * Setup: The test assumes that the switch device is configured to send
 * packets to the CPU and that the user has the ability to inject packets
 * that will be forwarded to the CPU.
 *
 * Ingress port(s): This test requires that at least one port is up and able
 * to pass traffic to the CPU.
 *
 * Egress ports(s): Not applicable.
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
 *     support asynchronous callback functions at this time so it is not
 *     possible to verify RX functionality using CINT. CINT does not
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

/* Console I/O routines defined here */
#include <bsl/bsl.h>

/* Basic data types and error codes */
#include <shr/shr_types.h>
#include <shr/shr_error.h>

/* Function prototype for bcmpkt_rx_register() defined here */
#include <bcmpkt/bcmpkt_net.h>

/* Function prototypes for this file are here */
#include <bcma/customer/bcma_simple_rx.h>

/*
 * Customer RX Call Back Function
 *
 * This very simple callback just prints the received packet. It does not
 * interpret packet metadata. Only one callback function can be registered
 * at a time for each network interface.
 *
 * Parameters:
 * unit [input]:     Device unit number for received packet [unused]
 * netif_id [input]: Network interface ID for received packet
 * packet [input]:   Pointer to packet structure that contains received packet
 * cookie [input]:   Pointer to user defined data [unused]
 */
static      shr_error_t
customer_packet_watcher(int unit, int netif_id, bcmpkt_packet_t * packet, void *cookie)
{
    /* Don't display more than some fixed amount of the incoming packet. */
    const int   max_display_bytes = 192;

    /* Get a packet data pointer from packet structure. */
    uint8_t    *data = BCMPKT_PACKET_DATA(packet);

    /* Get the packet data size in bytes from packet structure. */
    uint32_t    data_len = BCMPKT_PACKET_LEN(packet);

    /* Data buffer index used for packet initialization. */
    int         idx;

    cli_out("\n%s: Received a %d byte packet on interface %d\n", __func__, data_len,
            netif_id);

    if (data_len > max_display_bytes) {
        /* Don't display entire packet if it is very large. */
        cli_out("  Displaying first %d bytes of packet\n", max_display_bytes);
        data_len = max_display_bytes;
    }

    /* Print packet buffer */
    for (idx = 0; idx < data_len; idx++) {
        if ((idx & 0xf) == 0) {
            /* Print packet buffer index at start of line. */
            cli_out("%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            /* Separator */
            cli_out("- ");
        }
        cli_out("%02x ", data[idx]);    /* Print packet data byte */
        if ((idx & 0xf) == 0xf) {
            /* Print 16 bytes per line */
            cli_out("\n");
        }
    }
    if ((idx & 0xf) != 0) {
        /* Add last line terminator if necessary */
        cli_out("\n");
    }

    return SHR_E_NONE;
}

/*
 * A simple example for registering an RX callback function.
 *
 * The function bcma_simple_rx() is used to register an RX callback
 * function. When a packet is received, a pointer to the packet along with
 * other information is passed to the specified callback function. For
 * this simple test case, the callback prints the data contained in the
 * packet. Only one RX callback at a time can be registered to a specific
 * network interface.
 *
 * unit: Unit number for received packets.
 * netif_id: Network interface to use for packet reception.
 *
 * Returns SHR_E_NONE on success.
 */

shr_error_t
bcma_simple_rx(int unit, int netif_id)
{
    shr_error_t rv;

    cli_out("%s: Register simple RX callback handler on unit: %d; netifId: %d\n",
            __func__, unit, netif_id);

    /*
     * Register the callback back function with the system.
     *
     * The callback function signature must match the signature defined
     * for the type bcmpkt_rx_cb_f.
     *
     * The "flags" and "cb_data" parameters are unused and set to 0.
     */
    if (SHR_FAILURE
        (rv = bcmpkt_rx_register(unit, netif_id, 0, customer_packet_watcher, 0))) {
        cli_out("%s() FAILED[bcmpkt_rx_register]: %s\n", __func__, shr_errmsg(rv));
        return rv;
    }

    return SHR_E_NONE;
}
