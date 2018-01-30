/*! \file bcm56960_a0_tm_tdm_defines_internal.h
 *
 * Defines and Typedefs for TDM chip-specific algorithm.
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

#ifndef BCM56960_A0_TM_TDM_DEFINES_INTERNAL_H
#define BCM56960_A0_TM_TDM_DEFINES_INTERNAL_H

/********************************************************************
 * Defines
 */
/*! Frequency. */
#define TH_CLK_950MHZ 950
#define TH_CLK_850MHZ 850
#define TH_CLK_765MHZ 765
#define TH_CLK_672MHZ 672
#define TH_CLK_645MHZ 645
#define TH_CLK_545MHZ 545

/*! Calendar length. */
#define TH_LEN_950MHZ_HG 243
#define TH_LEN_950MHZ_EN 250
#define TH_LEN_850MHZ_HG 215
#define TH_LEN_850MHZ_EN 224
#define TH_LEN_765MHZ_HG 194
#define TH_LEN_765MHZ_EN 202
#define TH_LEN_672MHZ_HG 170
#define TH_LEN_672MHZ_EN 177
#define TH_LEN_645MHZ_HG 163
#define TH_LEN_645MHZ_EN 170
#define TH_LEN_545MHZ_HG 138
#define TH_LEN_545MHZ_EN 143

/*! Number of PMs in chip. */
#define TH_NUM_PM_MOD 33

/*! Number of front panel port PMs. */
#define TH_NUM_PHY_PM 32

/*! Number of lanes per PM. */
#define TH_NUM_PM_LNS 4

/*! Number of logical ports. */
#define TH_NUM_EXT_PORTS 136

/*! Number of front panel physical ports. */
#define TH_NUM_PHY_PORTS 128

/*! Number of scheduling pipes. */
#define TH_NUM_PIPE 4

/*! Number of Subpipes per pipe. */
#define TH_NUM_SPIPE 1

/*! Number of ancillary slots in calendar. */
#define TH_NUM_ANCL 10
#define TH_NUM_ANCL_CPU 4
#define TH_NUM_ANCL_MGMT 4
#define TH_NUM_ANCL_LPBK 2
#define TH_NUM_ANCL_IDB_OPT1 2
#define TH_NUM_ANCL_IDB_NULL 1
#define TH_NUM_ANCL_IDB_IDLE 1
#define TH_NUM_ANCL_MMU_OPT1 1
#define TH_NUM_ANCL_MMU_NULL 3
#define TH_NUM_ANCL_MMU_IDLE 0

/*! Max number of linerate calendar slots. */
#define TH_LR_CAL_MAX_LEN 512

/*! Number of oversub speed groups per pipe. */
#define TH_OS_VBS_GRP_NUM 8

/*! Length of each oversub speed group. */
#define TH_OS_VBS_GRP_LEN 12

/*! Min sister port spacing. */
#define TH_MIN_SPACING_SISTER_PORT 4

/*! Min same port spacing. */
#define TH_MIN_SPACING_SAME_PORT 4

/*! Ancillary port token: CPU. */
#define TH_CMIC_TOKEN 0

/*! Ancillary port token: management port 1. */
#define TH_MGM1_TOKEN (TH_NUM_EXT_PORTS-7)

/*! Ancillary port token: reservation. */
#define TH_RSVD_TOKEN (TH_NUM_EXT_PORTS-6)

/*! Ancillary port token: management port 2. */
#define TH_MGM2_TOKEN (TH_NUM_EXT_PORTS-5)

/*! Ancillary port token: pipe 0 loopback. */
#define TH_LPB0_TOKEN (TH_NUM_EXT_PORTS-4)

/*! Ancillary port token: pipe 1 loopback. */
#define TH_LPB1_TOKEN (TH_NUM_EXT_PORTS-3)

/*! Ancillary port token: pipe 2 loopback. */
#define TH_LPB2_TOKEN (TH_NUM_EXT_PORTS-2)

/*! Ancillary port token: pipe 3 loopback. */
#define TH_LPB3_TOKEN (TH_NUM_EXT_PORTS-1)

/*! Ancillary port token: idle slot. */
#define TH_IDLE_TOKEN (TH_NUM_EXT_PORTS+2)

/*! Ancillary port token: opportunistic1 slot. */
#define TH_OPP1_TOKEN (TH_NUM_EXT_PORTS+3)

/*! Ancillary port token: internal ancillary slot. */
#define TH_ANCL_TOKEN (TH_NUM_EXT_PORTS+10)

#ifdef _TDM_STANDALONE

    /*! Ancillary port token: oversub.    137 */
    #define TH_OVSB_TOKEN (TH_NUM_EXT_PORTS+1)

    /*! Ancillary port token: idle1 slot. 138 */
    #define TH_IDL1_TOKEN (TH_NUM_EXT_PORTS+2)

    /*! Ancillary port token: idle2 slot. 139 */
    #define TH_IDL2_TOKEN (TH_NUM_EXT_PORTS+3)

    /*! Ancillary port token: null slot.  140 */
    #define TH_NULL_TOKEN (TH_NUM_EXT_PORTS+4)

#else
    /*! Ancillary port token: oversub.    250 */
    #define TH_OVSB_TOKEN 250

    /*! Ancillary port token: idle1 slot. 251 */
    #define TH_IDL1_TOKEN 251

    /*! Ancillary port token: idle2 slot. 252 */
    #define TH_IDL2_TOKEN 252

    /*! Ancillary port token: null slot.  253 */
    #define TH_NULL_TOKEN 253
#endif

/*! Calendar ID. */
#define TH_IDB_PIPE_0_CAL_ID 0
#define TH_IDB_PIPE_1_CAL_ID 1
#define TH_IDB_PIPE_2_CAL_ID 2
#define TH_IDB_PIPE_3_CAL_ID 3
#define TH_MMU_PIPE_0_CAL_ID 4
#define TH_MMU_PIPE_1_CAL_ID 5
#define TH_MMU_PIPE_2_CAL_ID 6
#define TH_MMU_PIPE_3_CAL_ID 7

/*! Number of calendars. */
#define TH_NUM_CALS 8

/*! Physical port number range per calendar. */
#define TH_CAL_0_PORT_LO 1
#define TH_CAL_0_PORT_HI 32
#define TH_CAL_1_PORT_LO 33
#define TH_CAL_1_PORT_HI 64
#define TH_CAL_2_PORT_LO 65
#define TH_CAL_2_PORT_HI 96
#define TH_CAL_3_PORT_LO 97
#define TH_CAL_3_PORT_HI 128
#define TH_CAL_4_PORT_LO 1
#define TH_CAL_4_PORT_HI 32
#define TH_CAL_5_PORT_LO 33
#define TH_CAL_5_PORT_HI 64
#define TH_CAL_6_PORT_LO 65
#define TH_CAL_6_PORT_HI 96
#define TH_CAL_7_PORT_LO 97
#define TH_CAL_7_PORT_HI 128

/*! Linerate jitter threshold. */
#define TH_LR_JITTER_TRHD 1

/*! Linerate calendar printout format. */
#define TH_PRINT_LR_WID 20

#endif /* BCM56960_A0_TM_TDM_DEFINES_INTERNAL_H */
