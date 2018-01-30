/*! \file bcma_bcmpktcmd_knet.h
 *
 * CLI command related to KNET.
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

#ifndef BCMA_BCMPKTCMD_KNET_H
#define BCMA_BCMPKTCMD_KNET_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_KNET_DESC \
    "KNET access."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_KNET_SYNOP \
    "<object> <action> [<id>] [options]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_KNET_HELP \
    "This command is used to manage KNET objects such as virtual network\n" \
    "interfaces and packet filters.\n" \
    "    Netif - Network interface operations.\n" \
    "        Create [options] - Create a new network interface.\n" \
    "            Name=<value>        - Netif name string.\n" \
    "            RcpuEncap=[yes/no]  - Enable RCPU header encapsulation.\n" \
    "            BindrxCHannel=[yes/no] - Bind RX Channel.\n" \
    "            BindtxPort=[yes/no] - Bind TX port.\n" \
    "            Vlan=<value>        - VLAN ID.\n" \
    "            MACaddr=<value>     - MAC address.\n" \
    "            Port=<value>        - Port number for TX binding.\n" \
    "            PortEncap=<value>   - The binding port's encapsulation type\n" \
    "                                  (\"higig\" | \"higig2\" | \"ieee\").\n" \
    "            Queue=<value>       - Output queue ID of the binding port.\n" \
    "            CHan=<value>        - Channel ID for RX Channel binding.\n" \
    "            FrameSize=<value>   - Maximum frame size allowed.\n" \
    "        Destroy <id>   - Destroy a network interface.\n" \
    "        Info [<id>]    - Show a network interface's configuraiton.\n" \
    "    Filter - Filter operations.\n" \
    "        Create [options] - Create a filter.\n" \
    "            DESC=<value>        - Filter's description.\n" \
    "            Priority=<value>    - Filter's priority.\n" \
    "            DestType=<type>     - Destination type to forward.\n" \
    "            DestID=<value>      - Destination ID to forward.\n" \
    "            DestProto=<value>   - Overwrite protocol.\n" \
    "            MirrorType=<value>  - Mirroring destination type.\n" \
    "            MirrorID=<value>    - Mirroring destination ID.\n" \
    "            MirrorProto=<value> - Overwrite protocol for mirror copy.\n" \
    "            StripTag=[yes/no]   - Enable to trip VLAN Tag.\n" \
    "            Vlan=<value>        - 16 bit TCI to match.\n" \
    "            IngressPort=<value> - Ingress port ID to match.\n" \
    "            SrcModePort=<value> - Source module port to match.\n" \
    "            SrcModeID=<value>   - Source module ID to match.\n" \
    "            Queue=<value>       - Source CPU port queue to match.\n" \
    "            CHan=<value>        - Source DMA Channel ID. \n" \
    "            FPRule=<value>      - FP rule number to match.\n" \
    "            Reason=<reason>     - Specific reason to match.\n" \
    "            RawData=<data>      - Raw data string to match.\n" \
    "            RawMask=<mask>      - Raw data mask string to match.\n" \
    "        Destroy <id>   - Destroy a filter.\n" \
    "        Info [<id>]    - Show a filter configuraiton.\n" \
    "Examples:\n" \
    "knet netif create name=\"bcm0_1\" mac=00:11:22:33:44:55 framesize=1518\n" \
    "knet n c n=\"bcm0_2\" mac=00:01:02:03:04:05 fs=1518\n" \
    "knet filter create chan=1 priority=10 desttype=netif destid=1\n" \
    "                   ingressport=1\n" \
    "knet f c ch=1 p=20 dt=netif did=1 q=2\n" \
    "knet f destroy 1\n"

/*!
 * \brief KNET management command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_knet(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_KNET_H */
