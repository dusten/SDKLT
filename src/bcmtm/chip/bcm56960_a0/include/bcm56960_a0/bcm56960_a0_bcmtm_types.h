/*! \file bcm56960_a0_bcmtm_types.h
 *
 * Chip specific defines.
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

#ifndef BCM56960_A0_BCMTM_TYPES_H
#define BCM56960_A0_BCMTM_TYPES_H

#define BCM56960_A0_NUM_PIPES   4
#define BCM56960_A0_NUM_XPES    4
#define BCM56960_A0_NUM_LAYERS  2

#define BCM56960_A0_NUM_PORTS   256

#ifdef SOC_MAX_NUM_PIPES
#undef SOC_MAX_NUM_PIPES
#define SOC_MAX_NUM_PIPES           BCM56960_A0_NUM_PIPES
#endif

#ifdef SOC_MAX_NUM_PORTS
#undef SOC_MAX_NUM_PORTS
#define SOC_MAX_NUM_PORTS           BCM56960_A0_NUM_PORTS
#endif

#ifdef SOC_MAX_NUM_MMU_PORTS
#undef SOC_MAX_NUM_MMU_PORTS
#define SOC_MAX_NUM_MMU_PORTS       BCM56960_A0_NUM_PORTS
#endif

#ifdef SOC_PORT_RESOURCE_LANES_MAX
#undef SOC_PORT_RESOURCE_LANES_MAX
#define SOC_PORT_RESOURCE_LANES_MAX 4
#endif


/*  General Physical port */
#define _TH_PORTS_PER_PBLK             4
#define _TH_PBLKS_PER_PIPE             8
#define _TH_PBLKS_PER_HPIPE            8
#define _TH_PIPES_PER_DEV              4
#define _TH_XPES_PER_DEV               4

#define _TH_PBLKS_PER_DEV              \
    (_TH_PBLKS_PER_PIPE * _TH_PIPES_PER_DEV)

#define _TH_PORTS_PER_PIPE             \
    (_TH_PORTS_PER_PBLK * _TH_PBLKS_PER_PIPE)
#define _TH_PORTS_PER_DEV              \
    (_TH_PORTS_PER_PIPE * _TH_PIPES_PER_DEV)

/*  32 Falcon +  1 Eagle core*/
#define _TH_PM_PER_DEV                 (_TH_PBLKS_PER_DEV + 1)


#define _TH_TDM_LENGTH                 256
#define _TH_OVS_GROUP_COUNT_PER_HPIPE  6
#define _TH_OVS_GROUP_COUNT_PER_PIPE   6
#define _TH_OVS_GROUP_TDM_LENGTH       12
#define _TH_OVS_HPIPE_COUNT_PER_PIPE   1
#define _TH_PKT_SCH_LENGTH             160

#define _TH_TDM_CALENDAR_UNIVERSAL             0
#define _TH_TDM_CALENDAR_ETHERNET_OPTIMIZED    1

#define _TH_TDM_SPEED_CLASS_MAX                4

/*  Special port number used by TDM */
#define _SOC_TH_TDM_OVERSUB_TOKEN    0x22
#define _SOC_TH_TDM_NULL_TOKEN       0x23
#define _SOC_TH_TDM_IDL1_TOKEN       0x24
#define _SOC_TH_TDM_IDL2_TOKEN       0x25
#define _SOC_TH_TDM_UNUSED_TOKEN     0x3f

#define _SOC_TH_TDM_CPU_MNG_TOKEN  0x20
#define _SOC_TH_TDM_LB_TOKEN       0x21
#define _SOC_TH_TDM_OVERSUB_TOKEN  0x22

enum soc_th_port_ratio_e {
    SOC_TH_PORT_RATIO_SINGLE,
    SOC_TH_PORT_RATIO_DUAL_1_1,
    SOC_TH_PORT_RATIO_DUAL_2_1,
    SOC_TH_PORT_RATIO_DUAL_1_2,
    SOC_TH_PORT_RATIO_TRI_023_2_1_1,
    SOC_TH_PORT_RATIO_TRI_023_4_1_1,
    SOC_TH_PORT_RATIO_TRI_012_1_1_2,
    SOC_TH_PORT_RATIO_TRI_012_1_1_4,
    SOC_TH_PORT_RATIO_QUAD,
    SOC_TH_PORT_RATIO_COUNT
};

#endif /* BCM56960_A0_BCMTM_TYPES_H */
