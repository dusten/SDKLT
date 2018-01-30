/*! \file bcma_bcmpktcmd_tx.h
 *
 * CLI command related to packet transmit.
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

#ifndef BCMA_BCMPKTCMD_TX_H
#define BCMA_BCMPKTCMD_TX_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_TX_DESC \
    "Transmit packets."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_TX_SYNOP \
    "<count> [options]\n" \
    "<object> [options]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_TX_HELP \
    "Transmit one or more packets. The default packet contents can be\n" \
    "overridden using command line options or an input file.\n" \
    "Options:\n" \
    "    NetifID=<value>      - Netif ID from which packets sent out.\n" \
    "    PortList=<portlist>  - Destination port list.\n" \
    "    ForwardType=<value>  - NORMAL: API handles RCPU Header and TXPMD.\n" \
    "                           RAW: API doesn't handle RCPU Header and\n" \
    "                           TXPMD.\n" \
    "    File=<filename>      - Load hex packet data from file and ignore\n" \
    "                           various other pattern parameters below.\n" \
    "    DATA=<packet>        - Parse string given by user as packet payload\n" \
    "    LENgth=<value>       - Specify the total length of the packet,\n" \
    "                           including header, possible tag, and CRC.\n" \
    "    EtherType=<value>    - EtherType for packet generation.\n" \
    "    TPID=<value>         - TPID for packet generation.\n" \
    "    UnTagged=[yes/no]    - Without 802.1Q TAG.\n" \
    "    DEI=<value>          - 802.1Q Drop eligible indicator.\n" \
    "    PCP=<value>          - 802.1Q Priority code point.\n" \
    "    VlanID=<value>       - 802.1Q VLAN identifier.\n" \
    "    IntPrio=<value>      - Internal Priority.\n" \
    "    PATtern=<value>      - Specify 32-bit data pattern used.\n" \
    "    PATternInc=<value>   - Value by which each word of the data\n" \
    "                           pattern is incremented.\n" \
    "    PATternRandom=[yes/no]- Use Random data pattern\n" \
    "    PerPortSrcMac=[yes/no]- Associate specific (different) src macs\n" \
    "                          with each source port.\n" \
    "    SourceMac=<value>    - Source MAC address in packet.\n" \
    "    SourceMacInc=<val>   - Source MAC increment.\n" \
    "    DestMac=<value>      - Destination MAC address in packet.\n" \
    "    DestMacInc=<value>   - Destination MAC increment.\n" \
    "    McQType=[yes/no]     - Egress queue type is MC queue.\n" \
    "    NetifId=<value>      - Network interface ID.\n" \
    "    TXPMDxxx=<value>     - Configure TXPMD field xxx. Refer to supported\n" \
    "                           fields by TxPmdList command.\n" \
    "    LBHDRxxx=<value>     - Configure Loopback header field xxx. Refer to\n" \
    "                           supported fields by LbHdrList command.\n" \
    "    HGxxx=<value>        - Configure Higig field xxx. Refer to\n" \
    "                           supported fields by HiGigList command.\n" \
    "    HG2xxx=<value>       - Configure HG2PMD field xxx. Refer to\n" \
    "                           supported fields by HiGig2List command.\n" \
    "Special objects:\n" \
    "    HiGigList  - list supported Higig field names.\n" \
    "    HiGig2List - list supported Higig2 field names.\n" \
    "    TxPmdList [options]  - list supported TXPMD field names.\n" \
    "        DeviceType=<value> - Device Type (e.g. BCM56960_a0).\n" \
    "        ViewName=<value>   - View name.\n" \
    "    LbHdrList [options]  - list supported Loopback header field names.\n" \
    "        DeviceType=<value> - Device Type (e.g. BCM56960_a0).\n" \
    "        ViewName=<value>   - View name.\n" \
    "\n" \
    "<portlist> is one or more comma-separated ports or port ranges, for\n" \
    "example \"pl=2,4-8,21\". An empty list is specified as \"pl=none\".\n" \
    "When configure packet metadata's multiple view fields, the view type\n" \
    "must be input before it's fields. Example:\n" \
    "tx 1 pl=1 txpmdheader_type=12 txpmdoam_downmep_tx:counter_1_action=1\n"

/*!
 * \brief Packet transmit command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_tx(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_TX_H */
