/*! \file tomahawk_flexport_defines.h
 *
 * Tomahawk flex port defines.
 * <description>
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

#ifndef TOMAHAWK_FLEXPORT_DEFINES_H
#define TOMAHAWK_FLEXPORT_DEFINES_H


#define _TH_PHY_PORT_CPU               0
#define _TH_PHY_PORT_MNG0              129
#define _TH_PHY_PORT_MNG1              131
#define _TH_LOG_PORT_MNG0              66
#define _TH_LOG_PORT_MNG1              100
#define _TH_PHY_PORT_LPBK0             132
#define _TH_PHY_PORT_LPBK1             133
#define _TH_PHY_PORT_LPBK2             134
#define _TH_PHY_PORT_LPBK3             135
#define _TH_FORCE_SAF_TIMER_ENTRY_CNT  5
#define _TH_OBM_PRIORITY_LOSSY_LO      0
#define _TH_OBM_PRIORITY_LOSSY_HI      1
#define _TH_OBM_PRIORITY_LOSSLESS0     2
#define _TH_OBM_PRIORITY_LOSSLESS1     3
#ifndef COUNTOF
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#endif
#define _TH_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=128))

/* General Physical port */
#define _TH_PORTS_PER_PBLK             4
#define _TH_PBLKS_PER_PIPE             8
#define _TH_PBLKS_PER_HPIPE            8
#define _TH_PIPES_PER_DEV              4
#define _TH_XPES_PER_DEV               4
#define _TH_GPHY_PORTS_PER_PIPE        \
    (_TH_PORTS_PER_PBLK * _TH_PBLKS_PER_PIPE)
#define _TH_PHY_PORTS_PER_PIPE         (_TH_GPHY_PORTS_PER_PIPE + 2)
#define _TH_PBLKS_PER_DEV              \
    (_TH_PBLKS_PER_PIPE * _TH_PIPES_PER_DEV)
#define _TH_PHY_PORTS_PER_DEV          \
    (_TH_PHY_PORTS_PER_PIPE * _TH_PIPES_PER_DEV)


/* Device port
 * 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TH_GDEV_PORTS_PER_PIPE        32
#define _TH_DEV_PORTS_PER_PIPE         (_TH_GDEV_PORTS_PER_PIPE + 2)
#define _TH_DEV_PORTS_PER_DEV          \
    (_TH_DEV_PORTS_PER_PIPE * _TH_PIPES_PER_DEV)

/* MMU port */
#define _TH_MMU_PORTS_OFFSET_PER_PIPE  32
#define _TH_MMU_PORTS_PER_DEV          (32 * 4)

/* TDM */
#define _TH_TDM_LENGTH                 256
#define _TH_OVS_GROUP_COUNT_PER_HPIPE  6
#define _TH_OVS_GROUP_COUNT_PER_PIPE   6
#define _TH_OVS_GROUP_TDM_LENGTH       12
#define _TH_OVS_HPIPE_COUNT_PER_PIPE   1

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* TOMAHAWK_FLEXPORT_DEFINES_H */
