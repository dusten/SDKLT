/*----------------------------------------------------------------------
 * $Id: tefmod_sc_defines.h, $
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
 * 
 * 
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
#ifndef tefmod_sc_defines_H_
#define tefmod_sc_defines_H_


#define T_PMA_BITMUX_MODE_1to1   0
#define T_PMA_BITMUX_MODE_2to1   1
#define T_PMA_BITMUX_MODE_5to1   2

#define T_SCR_MODE_BYPASS        0
#define T_SCR_MODE_CL49          1
#define T_SCR_MODE_40G_2_LANE    2
#define T_SCR_MODE_100G          3
#define T_SCR_MODE_20G           4
#define T_SCR_MODE_40G_4_LANE    5

#define R_DESCR_MODE_BYPASS      0
#define R_DESCR_MODE_CL49        1
#define R_DESCR_MODE_CL82        2

#define R_DEC_TL_MODE_BYPASS     0
#define R_DEC_TL_MODE_CL49       1
#define R_DEC_TL_MODE_CL82       2

#define R_DEC_FSM_MODE_BYPASS    0
#define R_DEC_FSM_MODE_CL49      1
#define R_DEC_FSM_MODE_CL82      2

#define BS_DIST_MODE_5_LANE_TDM          0
#define BS_DIST_MODE_2_LANE_TDM_2_VLANE  1
#define BS_DIST_MODE_2_LANE_TDM_1_VLANE  2
#define BS_DIST_MODE_NO_TDM              3

#define BS_BTMX_MODE_1to1        0
#define BS_BTMX_MODE_2to1        1
#define BS_BTMX_MODE_5to1        2

#define IDLE_DELETION_MODE_BYPASS       0 
#define IDLE_DELETION_MODE_40G          1 
#define IDLE_DELETION_MODE_100G         2
#define IDLE_DELETION_MODE_20G          3

#define T_FIFO_MODE_BYPASS       0 
#define T_FIFO_MODE_40G          1 
#define T_FIFO_MODE_100G         2
#define T_FIFO_MODE_20G          3

#define T_ENC_MODE_BYPASS        0
#define T_ENC_MODE_CL49          1
#define T_ENC_MODE_CL82          2

#define R_DESKEW_MODE_BYPASS     0
#define R_DESKEW_MODE_20G        1
#define R_DESKEW_MODE_40G_4_LANE 2
#define R_DESKEW_MODE_40G_2_LANE 3
#define R_DESKEW_MODE_100G       4
#define R_DESKEW_MODE_CL49       5
#define R_DESKEW_MODE_CL91       6

#define AM_MODE_20G              1
#define AM_MODE_40G              2
#define AM_MODE_100G             3

#define NUM_LANES_01                            0
#define NUM_LANES_02                            1
#define NUM_LANES_04                            2


#define PORT_MODE_FOUR                          0
#define PORT_MODE_THREE_3_2_COMBO               1
#define PORT_MODE_THREE_1_0_COMBO               2
#define PORT_MODE_TWO                           3
#define PORT_MODE_ONE                           4

#define OS_MODE_1                               0
#define OS_MODE_2                               1
#define OS_MODE_4                               2
#define OS_MODE_8                               5
#define OS_MODE_16p5                            8
#define OS_MODE_20p625                          12

#define REG_ACCESS_TYPE_MDIO                    0
#define REG_ACCESS_TYPE_UC                      1
#define REG_ACCESS_TYPE_EXT                     2


#define BS_CL82_SYNC_MODE 0
#define BS_CL49_SYNC_MODE 1
#define BLKSIZE           66
#define DSC_DATA_WID      40

#endif

