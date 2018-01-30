/*! \file bcma_rx_module_header.c
 *
 * Demonstrate usage of RX and RX module header APIs for NGSDK.
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
 * Purpose: A set of RX routines demonstrating the use of basic NGSDK RX
 * and RX module header (HiGIg/HiGig2) APIs.
 *
 * Description: This test registers a callback function that prints a received
 * packet and the associated module header for that packet.  This example
 * also includes a corresponding "unregister" function to unregister this
 * same callback function.
 *
 * Setup: This test assumes that the switch device is configured to send
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
 *     support any bcmpkt_*higig2() functions at this time.
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

/* Console I/O routines defined here */
#include <bsl/bsl.h>

/* Basic data types and error codes */
#include <shr/shr_types.h>
#include <shr/shr_error.h>

/* Function prototype for bcmpkt_rx_register() defined here */
#include <bcmpkt/bcmpkt_net.h>

/* Function prototypes for this file are here */
#include <bcma/customer/bcma_rx_module_header.h>

/* Local Function Prototypes */
static shr_error_t packet_module_header_printer(int unit, int netif_id,
                                                bcmpkt_packet_t * packet, void *cookie);
static void print_packet(bcmpkt_packet_t * packet);
static void print_packet_module_header(int unit, bcmpkt_packet_t * packet);

/*
 * Customer RX callback function
 *
 * This callback prints the received packet and module header associates with
 * that packet.  Only one callback function can be registered at a time for
 * each network interface.
 *
 * Parameters:
 * unit [input]:     Device unit number for received packet
 * netif_id [input]: Network interface ID for received packet
 * packet [input]:   Pointer to packet structure that contains received packet
 * cookie [input]:   Pointer to user defined data [unused]
 *
 * Returns: Always returns SHR_E_NONE;
 */
static      shr_error_t
packet_module_header_printer(int unit, int netif_id, bcmpkt_packet_t * packet,
                             void *cookie)
{
    cli_out("\n%s: Received a %d byte packet on interface %d\n", __func__,
            BCMPKT_PACKET_LEN(packet), netif_id);

    print_packet_module_header(unit, packet);
    print_packet(packet);

    return SHR_E_NONE;
}

/*
 * Simple Packet Printer
 *
 * This routine prints the packet as received by the CPU.
 *
 * Paramters:
 * packet [input]: A pointer to the packet structure. This data structure
 * include pointers to the packet data.
 */
static void
print_packet(bcmpkt_packet_t * packet)
{
    /* Don't display more than some fixed amount of the incoming packet. */
    const int   max_display_bytes = 192;

    /* Get a packet data pointer from packet structure. */
    uint8_t    *data = BCMPKT_PACKET_DATA(packet);

    /* Get the packet data size in bytes from packet structure. */
    uint32_t    data_len = BCMPKT_PACKET_LEN(packet);

    /* Data buffer index used for packet initialization. */
    int         idx;

    cli_out("\nPacket Data:\n");
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
}

/*
 * Print Packet Module Header
 *
 * This routine displays the module header for the incoming packet. Only
 * packets that come in on a HiGig port will have a valid module header.
 * The module header will be or type HiGig2 or HiGig/HiGig+ depending on how
 * the port is configured.  The module header is embedded 3 or 4 contiguous
 * words of the RX DMA Control Block (DCB) that is tranfered with the packet
 * as part of the DMA process.  The Programmers's Register Guide for each
 * device provides descriptions for the RX DMA Control Block and all types
 * of module headers for that device.
 *
 * Paramters:
 * unit [input]: The unit number associated with the incoming packet.
 * packet [input]: A pointer to the packet structure. This data structure
 * include pointers to the packet metadata.
 */
static void
print_packet_module_header(int unit, bcmpkt_packet_t * packet)
{
    /*
     * There is no explicit way to determine the module header type based
     * on information in DCB.  Current solution is to check the SOP field,
     * if it is 0xFB, the packet module header is some sort of HiGig (1 or
     * 2). If the HGI field from HiGig/HiGig+ header is non-zero, packet
     * must be HiGig/HiGig+, otherwise assume HiGig2.
     */
    int         mh_type;
    shr_error_t rv;
    uint32_t    dev_type;
    uint32_t   *raw_hg_hdr;
    uint32_t   *rxpmd;

    /*
     * STEP 1: Get the packet metadata for this packet.
     * Don't need to check return status.
     */
    bcmpkt_rxpmd_get(packet, &rxpmd);

    /*
     * STEP 2: Get the device type for this unit.
     */
    if (SHR_FAILURE(rv = bcmpkt_dev_type_get(unit, &dev_type))) {
        cli_out("%s() FAILED[bcmpkt_dev_type_get]: %s\n", __func__, shr_errmsg(rv));
        return;
    }

    /*
     * STEP 3: Get raw module header pointer from packet metadata.
     *
     * Pass the packet metadata obtained in step 1 and the device type
     * obtained in step 2 to bcmpkt_rxpmd_mh_get() to get a pointer to
     * the module header block from the DCB. The raw header needs to be
     * mapped to HiGig or HiGig2 depending on the type of header it is.
     */
    if (SHR_FAILURE(rv = bcmpkt_rxpmd_mh_get(dev_type, rxpmd, &raw_hg_hdr))) {
        cli_out("%s() FAILED[bcmpkt_rxpmd_mh_get]: %s\n", __func__, shr_errmsg(rv));
        return;
    }

    /*
     * STEP 4:
     * Using hueristics described above, try to determine the module header
     * type based on information in DCB alone.
     */
    if (HIGIG_STARTf_GET(*((HIGIG_t *) raw_hg_hdr)) == 0xFB) {
        if (HIGIG_HGIf_GET(*((HIGIG_t *) raw_hg_hdr)) == 0) {
            mh_type = 2;
        } else {
            mh_type = 1;
        }
    } else {
        mh_type = 0;
    }

    /*
     * STEP 5:
     * Print all module header fields for the module header type determined in step 4.
     */
    if (mh_type == 0) {
        cli_out("No Module Header information (port not HiGig?)\n");
    } else if (mh_type == 1) {
        /* HiGig/HiGig+ */
        const HIGIG_t *const higig = (HIGIG_t *) raw_hg_hdr;

        cli_out("Module Type: HiGig/HiGIg+\n");
        cli_out("%-16s 0x%04X\n", "START", HIGIG_STARTf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "HGI", HIGIG_HGIf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "CNG1", HIGIG_CNG1f_GET(*higig));
        cli_out("%-16s 0x%04X\n", "HDR_EXT_LEN", HIGIG_HDR_EXT_LENf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "SRC_MODID_6", HIGIG_SRC_MODID_6f_GET(*higig));
        cli_out("%-16s 0x%04X\n", "DST_MODID_6", HIGIG_DST_MODID_6f_GET(*higig));
        cli_out("%-16s 0x%04X\n", "VTAG", HIGIG_VTAGf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "SRC_MODID_LS", HIGIG_SRC_MODID_LSf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "OPCODE", HIGIG_OPCODEf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "PFM", HIGIG_PFMf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "SRC_PORT_TGID", HIGIG_SRC_PORT_TGIDf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "DST_PORT", HIGIG_DST_PORTf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "PRIORITY", HIGIG_PRIORITYf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "HEADER_TYPE", HIGIG_HEADER_TYPEf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "CNG", HIGIG_CNGf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "DST_MODID_LS", HIGIG_DST_MODID_LSf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "DST_T", HIGIG_DST_Tf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "DST_TGID", HIGIG_DST_TGIDf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "INGRESS_TAGGED", HIGIG_INGRESS_TAGGEDf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "MIRROR_ONLY", HIGIG_MIRROR_ONLYf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "MIRROR_DONE", HIGIG_MIRROR_DONEf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "MIRROR", HIGIG_MIRRORf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "SRC_MODID_5", HIGIG_SRC_MODID_5f_GET(*higig));
        cli_out("%-16s 0x%04X\n", "DST_MODID_5", HIGIG_DST_MODID_5f_GET(*higig));
        cli_out("%-16s 0x%04X\n", "L3", HIGIG_L3f_GET(*higig));
        cli_out("%-16s 0x%04X\n", "LABEL_PRESENT", HIGIG_LABEL_PRESENTf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "CLASSIFICATION_TAG",
                HIGIG_CLASSIFICATION_TAGf_GET(*higig));
        cli_out("%-16s 0x%04X\n", "VC_LABEL", HIGIG_VC_LABELf_GET(*higig));
    } else if (mh_type == 2) {
        const HIGIG2_t *const higig2 = (HIGIG2_t *) raw_hg_hdr;
        const uint32_t ppdtype = HIGIG2_PPD_TYPEf_GET(*higig2);

        cli_out("Module Type: HiGig2\n");
        cli_out("%-24s: 0x%X\n", "START", HIGIG2_STARTf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "MCST", HIGIG2_MCSTf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "TC", HIGIG2_TCf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "DST_MODID_MGIDH",
                HIGIG2_DST_MODID_MGIDHf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "DST_PORT_MGIDL", HIGIG2_DST_PORT_MGIDLf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "SRC_MODID", HIGIG2_SRC_MODIDf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "SRC_PID", HIGIG2_SRC_PIDf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "LBID", HIGIG2_LBIDf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "DP", HIGIG2_DPf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "EHV", HIGIG2_EHVf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "CC", HIGIG2_CCf_GET(*higig2));
        cli_out("%-24s: 0x%X\n", "PPD_TYPE", HIGIG2_PPD_TYPEf_GET(*higig2));
        if (ppdtype == 3) {
            cli_out("%-24s: 0x%X\n", "PPD3_PPD3_DATA_CONTAINER",
                    HIGIG2_PPD3_PPD3_DATA_CONTAINERf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_SOURCE_INFO",
                    HIGIG2_PPD3_SOURCE_INFOf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_PRESERVE_DSCP",
                    HIGIG2_PPD3_PRESERVE_DSCPf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_PRESERVE_DOT1P",
                    HIGIG2_PPD3_PRESERVE_DOT1Pf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_DO_NOT_LEARN",
                    HIGIG2_PPD3_DO_NOT_LEARNf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_RSVD_28", HIGIG2_PPD3_RSVD_28f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_SOURCE_TYPE",
                    HIGIG2_PPD3_SOURCE_TYPEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_OPCODE", HIGIG2_PPD3_OPCODEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_RSVD_7_4",
                    HIGIG2_PPD3_RSVD_7_4f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD3_PPD3_CONTAINER_TYPE",
                    HIGIG2_PPD3_PPD3_CONTAINER_TYPEf_GET(*higig2));
        } else if (ppdtype == 2) {
            cli_out("%-24s: 0x%X\n", "PPD2_MULTIPOINT",
                    HIGIG2_PPD2_MULTIPOINTf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_FWD_TYPE",
                    HIGIG2_PPD2_FWD_TYPEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_VNI_MID", HIGIG2_PPD2_VNI_MIDf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_VNI_LOW", HIGIG2_PPD2_VNI_LOWf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_REP_ID_15_0",
                    HIGIG2_PPD2_REP_ID_15_0f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_DEST_VP", HIGIG2_PPD2_DEST_VPf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_SOURCE_VP",
                    HIGIG2_PPD2_SOURCE_VPf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_MIRROR", HIGIG2_PPD2_MIRRORf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_DO_NOT_MODIFY",
                    HIGIG2_PPD2_DO_NOT_MODIFYf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_DO_NOT_LEARN",
                    HIGIG2_PPD2_DO_NOT_LEARNf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_LAG_FAILOVER",
                    HIGIG2_PPD2_LAG_FAILOVERf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_RSVD_11", HIGIG2_PPD2_RSVD_11f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_REP_ID_17_16",
                    HIGIG2_PPD2_REP_ID_17_16f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_OPCODE", HIGIG2_PPD2_OPCODEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_RSVD_7_6",
                    HIGIG2_PPD2_RSVD_7_6f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_VNI_HIGH",
                    HIGIG2_PPD2_VNI_HIGHf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_PRESERVE_DSCP",
                    HIGIG2_PPD2_PRESERVE_DSCPf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_PRESERVE_DOT1P",
                    HIGIG2_PPD2_PRESERVE_DOT1Pf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_DEST_TYPE",
                    HIGIG2_PPD2_DEST_TYPEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD2_SOURCE_TYPE",
                    HIGIG2_PPD2_SOURCE_TYPEf_GET(*higig2));
        } else if (ppdtype == 1) {
            cli_out("%-24s: 0x%X\n", "PPD1_CLASSIFICATION_TAG",
                    HIGIG2_PPD1_CLASSIFICATION_TAGf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_RSVD_48_32",
                    HIGIG2_PPD1_RSVD_48_32f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_VID_HIGH",
                    HIGIG2_PPD1_VID_HIGHf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_VID_LOW", HIGIG2_PPD1_VID_LOWf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_PFM", HIGIG2_PPD1_PFMf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_SRC_T", HIGIG2_PPD1_SRC_Tf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_RSVD_12_11",
                    HIGIG2_PPD1_RSVD_12_11f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_OPCODE", HIGIG2_PPD1_OPCODEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_HDR_EXT_LEN",
                    HIGIG2_PPD1_HDR_EXT_LENf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD1_RSVD_4_0",
                    HIGIG2_PPD1_RSVD_4_0f_GET(*higig2));
        } else if (ppdtype == 0) {
            cli_out("%-24s: 0x%X\n", "PPD0_DST_T", HIGIG2_PPD0_DST_Tf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_DST_TGID",
                    HIGIG2_PPD0_DST_TGIDf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_INGRESS_TAGGED",
                    HIGIG2_PPD0_INGRESS_TAGGEDf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_MIRROR_ONLY",
                    HIGIG2_PPD0_MIRROR_ONLYf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_MIRROR_DONE",
                    HIGIG2_PPD0_MIRROR_DONEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_MIRROR", HIGIG2_PPD0_MIRRORf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_LABEL_OVERLAY_TYPE",
                    HIGIG2_PPD0_LABEL_OVERLAY_TYPEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_L3", HIGIG2_PPD0_L3f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_LABEL_PRESENT",
                    HIGIG2_PPD0_LABEL_PRESENTf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_VC_LABEL_19_16",
                    HIGIG2_PPD0_VC_LABEL_19_16f_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_ORIG_SRC_MODID",
                    HIGIG2_PPD0_ORIG_SRC_MODIDf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_ORIG_SRC_PORT",
                    HIGIG2_PPD0_ORIG_SRC_PORTf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_MIRROR_CLASSIFICATION_TAG",
                    HIGIG2_PPD0_MIRROR_CLASSIFICATION_TAGf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_VC_LABEL",
                    HIGIG2_PPD0_VC_LABELf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_REP_ID", HIGIG2_PPD0_REP_IDf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_VID_HIGH",
                    HIGIG2_PPD0_VID_HIGHf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_VID_LOW", HIGIG2_PPD0_VID_LOWf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_PFM", HIGIG2_PPD0_PFMf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_SRC_T", HIGIG2_PPD0_SRC_Tf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_PRESERVE_DSCP",
                    HIGIG2_PPD0_PRESERVE_DSCPf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_PRESERVE_DOT1P",
                    HIGIG2_PPD0_PRESERVE_DOT1Pf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_OPCODE", HIGIG2_PPD0_OPCODEf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_HDR_EXT_LEN",
                    HIGIG2_PPD0_HDR_EXT_LENf_GET(*higig2));
            cli_out("%-24s: 0x%X\n", "PPD0_RSVD_4_0",
                    HIGIG2_PPD0_RSVD_4_0f_GET(*higig2));
        } else {
            cli_out("Unknown PPD type: %u\n", ppdtype);
        }
    }
}

/*
 * Register an RX callback function that prints the packet and packet
 * module header.
 *
 * The function bcma_rx_module_header() is used to register an RX callback
 * function. When a packet is received, a pointer to the packet along with
 * other information is passed to the specified callback function. For this
 * example, the callback prints the data contained in the packet and the
 * module header defined for the incoming packet. Only one RX callback
 * at a time can be registered to a specific network interface. Before
 * a different RX callback function can be registered, the currently
 * registered callback function must be unregistered. Use the function
 * bcma_rx_module_header_unregister() to unregister this callback function.
 *
 * unit: Unit number for received packets.
 * netif_id: Network interface to use for packet reception.
 *
 * Returns SHR_E_NONE on success.
 */

shr_error_t
bcma_rx_module_header(int unit, int netif_id)
{
    shr_error_t rv;

    cli_out("%s: Register RX module header callback handler on unit: %d; netifId: %d\n",
            __func__, unit, netif_id);

    /*
     * Register the callback function with the system.
     *
     * The callback function signature must match the signature defined
     * for the type bcmpkt_rx_cb_f defined in bcmpkt_net.h.
     *
     * The "flags" and "cb_data" parameters are unused and set to 0.
     */
    if (SHR_FAILURE
        (rv = bcmpkt_rx_register(unit, netif_id, 0, packet_module_header_printer, 0))) {
        cli_out("%s() FAILED[bcmpkt_rx_register]: %s\n", __func__, shr_errmsg(rv));
        return rv;
    }

    return SHR_E_NONE;
}

/*
 * An example for unregistering a previously registered RX callback function.
 *
 * The function bcma_rx_module_header_unregister() is used to unregister the RX
 * callback function previously registered with bcma_rx_module_header(). RX callback
 * functions must be unregistered before another callback function can be
 * registered. Only one callback function can be registered for a specific
 * network interface at a time.
 *
 * unit: Unit number for received packets.
 * netif_id: Network interface to use for packet reception.
 *
 * Returns SHR_E_NONE on success.
 */

shr_error_t
bcma_rx_module_header_unregister(int unit, int netif_id)
{
    shr_error_t rv;

    cli_out("%s: Unregister RX PMD callback handler on unit: %d; netifId: %d\n",
            __func__, unit, netif_id);

    /*
     * Unregister the callback back function with the system.
     *
     * The callback function signature must match the signature defined for
     * the type bcmpkt_rx_cb_f defined in bcmpkt_net.h. The function pointer
     * passed here must matched the previously registered function pointer.
     *
     * The "cb_data" parameters is unused and set to 0.
     */
    if (SHR_FAILURE
        (rv = bcmpkt_rx_unregister(unit, netif_id, packet_module_header_printer, 0))) {
        cli_out("%s() FAILED[bcmpkt_rx_unregister]: %s\n", __func__, shr_errmsg(rv));
        return rv;
    }

    return SHR_E_NONE;
}
