/*----------------------------------------------------------------------
 * $Id: sc_field_defines.h  $
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
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : sc_field_defines.h
 * Description: 
 *---------------------------------------------------------------------*/

/*##############################################################################
############################################################################# */
#define TEMOD_PLL_MODE_DIV_40       2
#define TEMOD_PLL_MODE_DIV_42       3
#define TEMOD_PLL_MODE_DIV_48       4
#define TEMOD_PLL_MODE_DIV_50       5
#define TEMOD_PLL_MODE_DIV_52       6
#define TEMOD_PLL_MODE_DIV_54       7
#define TEMOD_PLL_MODE_DIV_60       8
#define TEMOD_PLL_MODE_DIV_64       9
#define TEMOD_PLL_MODE_DIV_66       10
#define TEMOD_PLL_MODE_DIV_68       11
#define TEMOD_PLL_MODE_DIV_70       12
#define TEMOD_PLL_MODE_DIV_80       13
#define TEMOD_PLL_MODE_DIV_92       14
#define TEMOD_PLL_MODE_DIV_100      15
#define TEMOD_PLL_MODE_DIV_82P5     16
#define TEMOD_PLL_MODE_DIV_87P5     17
#define TEMOD_PLL_MODE_ILLEGAL      0 


/*############################################################################# */
#define TEMOD_SCR_MODE_BYPASS       0
#define TEMOD_SCR_MODE_66B          1
#define TEMOD_SCR_MODE_80B          2
#define TEMOD_SCR_MODE_64B          3

/*############################################################################# */
#define TEMOD_ENCODE_MODE_NONE        0
#define TEMOD_ENCODE_MODE_CL48        1
#define TEMOD_ENCODE_MODE_CL48_2_LANE 2
#define TEMOD_ENCODE_MODE_CL36        3
#define TEMOD_ENCODE_MODE_CL82        4
#define TEMOD_ENCODE_MODE_CL49        5
#define TEMOD_ENCODE_MODE_BRCM        6
#define TEMOD_ENCODE_MODE_ILLEGAL     7


/*############################################################################# */
#define TEMOD_CL48_CHECK_END_OFF       0
#define TEMOD_CL48_CHECK_END_ON        1

/*############################################################################# */
#define TEMOD_SIGDET_FILTER_NONCX4     0
#define TEMOD_SIGDET_FILTER_CX4        1

/*############################################################################# */
#define TEMOD_BLOCKSYNC_MODE_NONE      0
#define TEMOD_BLOCKSYNC_MODE_CL49      1
#define TEMOD_BLOCKSYNC_MODE_CL82      2
#define TEMOD_BLOCKSYNC_MODE_8B10B     3
#define TEMOD_BLOCKSYNC_MODE_FEC       4
#define TEMOD_BLOCKSYNC_MODE_BRCM      5
#define TEMOD_BLOCKSYNC_MODE_ILLEGAL   7

/*############################################################################# */
#define TEMOD_R_REORDER_MODE_NONE      0
#define TEMOD_R_REORDER_MODE_CL48      1
#define TEMOD_R_REORDER_MODE_CL36      2
#define TEMOD_R_REORDER_MODE_CL36_CL48 3

/*############################################################################# */
#define TEMOD_CL36_DISABLE             0
#define TEMOD_CL36_ENABLE              1

/*############################################################################# */
#define TEMOD_R_DESCR1_MODE_BYPASS     0
#define TEMOD_R_DESCR1_MODE_66B        1
#define TEMOD_R_DESCR1_MODE_10B        2
#define TEMOD_R_DESCR1_MODE_ILLEGAL    3


/*############################################################################# */
#define TEMOD_DECODER_MODE_NONE       0
#define TEMOD_DECODER_MODE_CL49       1
#define TEMOD_DECODER_MODE_BRCM       2     
#define TEMOD_DECODER_MODE_ALU        3   
#define TEMOD_DECODER_MODE_CL48       4   
#define TEMOD_DECODER_MODE_CL36       5 
#define TEMOD_DECODER_MODE_CL82       6
#define TEMOD_DECODER_MODE_ILLEGAL    7

/*############################################################################# */
#define TEMOD_R_DESKEW_MODE_BYPASS        0
#define TEMOD_R_DESKEW_MODE_8B_10B        1
#define TEMOD_R_DESKEW_MODE_BRCM_66B      2
#define TEMOD_R_DESKEW_MODE_CL82_66B      3
#define TEMOD_R_DESKEW_MODE_CL36_10B      4
#define TEMOD_R_DESKEW_MODE_ILLEGAL       7

/*#############################################################################*/
#define TEMOD_DESC2_MODE_NONE             0
#define TEMOD_DESC2_MODE_CL49             1
#define TEMOD_DESC2_MODE_BRCM             2
#define TEMOD_DESC2_MODE_ALU              3
#define TEMOD_DESC2_MODE_CL48             4
#define TEMOD_DESC2_MODE_CL36             5
#define TEMOD_DESC2_MODE_CL82             6
#define TEMOD_DESC2_MODE_ILLEGAL          7


/*#############################################################################*/

#define TEMOD_R_DESC2_BYTE_DELETION_100M     0
#define TEMOD_R_DESC2_BYTE_DELETION_10M      1
#define TEMOD_R_DESC2_BYTE_DELETION_NONE     2

/*############################################################################*/

#define TEMOD_R_DEC1_DESCR_MODE_NONE         0
#define TEMOD_R_DEC1_DESCR_MODE_BRCM64B66B   1

/*#############################################################################*/

#define RTL_SPEED_100G_CR10                         0x29
#define RTL_SPEED_40G_CR4                           0x28
#define RTL_SPEED_40G_KR4                           0x27
#define RTL_SPEED_40G_X4                            0x1C
#define RTL_SPEED_32p7G_X4                          0x21
#define RTL_SPEED_31p5G_X4                          0x20
#define RTL_SPEED_25p45G_X4                         0x14
#define RTL_SPEED_21G_X4                            0x13
#define RTL_SPEED_20G_CR2                           0x3A
#define RTL_SPEED_20G_KR2                           0x39
#define RTL_SPEED_20G_X2                            0x1D
#define RTL_SPEED_20G_X4                            0x22
#define RTL_SPEED_20G_CX2                           0x1E
#define RTL_SPEED_20G_CX4                           0x12
#define RTL_SPEED_16G_X4                            0x0C
#define RTL_SPEED_15p75G_X2                         0x2C
#define RTL_SPEED_15G_X4                            0x0B
#define RTL_SPEED_13G_X4                            0x0A
#define RTL_SPEED_12p7G_X2                          0x19
#define RTL_SPEED_12p5G_X4                          0x09
#define RTL_SPEED_12G_X4                            0x08
#define RTL_SPEED_10p5G_X2                          0x17
#define RTL_SPEED_10G_KR1                           0x0F
#define RTL_SPEED_10G_CX2                           0x24
#define RTL_SPEED_10G_X2                            0x23
#define RTL_SPEED_10G_KX4                           0x0E
#define RTL_SPEED_10G_CX4                           0x07
#define RTL_SPEED_10G_X4                            0x06
#define RTL_SPEED_10G_CX1                           0x33
#define RTL_SPEED_6G_X4                             0x05
#define RTL_SPEED_5G_X4                             0x04
#define RTL_SPEED_5G_X2                             0x2A
#define RTL_SPEED_2p5G_X1                           0x03
#define RTL_SPEED_1G_KX1                            0x0D
#define RTL_SPEED_1000M                             0x02
#define RTL_SPEED_1G_CX1                            0x34
#define RTL_SPEED_100M                              0x01
#define RTL_SPEED_10M                               0x00
#define RTL_SPEED_5G_X1                             0x10
#define RTL_SPEED_6p36G_X1                          0x11
#define RTL_SPEED_10G_X2_NOSCRAMBLE                 0x15
#define RTL_SPEED_10G_CX2_NOSCRAMBLE                0x16
#define RTL_SPEED_10p5G_CX2_NOSCRAMBLE              0x18
#define RTL_SPEED_12p7G_CX2                         0x1A
#define RTL_SPEED_10G_X1                            0x1B
#define RTL_SPEED_10G_SFI                           0x1F
#define RTL_SPEED_3p125G_IL                         0x3C
#define RTL_SPEED_6p25_IL                           0x3D
#define RTL_SPEED_10p31G_IL                         0x3E
#define RTL_SPEED_10p96G_IL                         0x3F
#define RTL_SPEED_12p5G_IL                          0x40
#define RTL_SPEED_11p5G_IL                          0x41
#define RTL_SPEED_1p25G_IL                          0x42
#define RTL_SPEED_2G_FC                             0x2E
#define RTL_SPEED_4G_FC                             0x2F
#define RTL_SPEED_8G_FC                             0x30
#define RTL_SPEED_1G_CL73                           0x3B
#define RTL_SPEED_1000M_SGMII                       0x2D
#define RTL_SPEED_NONE                              0x2B


/*#############################################################################*/
