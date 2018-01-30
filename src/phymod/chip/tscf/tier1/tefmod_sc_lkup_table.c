/*----------------------------------------------------------------------
 * $Id: tefmod_sc_lkup_table.c, $
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
#ifndef _SDK_TEFMOD_
#ifndef _DV_TB_
 #define _SDK_TEFMOD_ 1
#endif
#endif
 
#ifdef _SDK_TEFMOD_
#include "tefmod_enum_defines.h"
#include "tefmod.h"
#include <phymod/phymod.h>
#endif

#ifdef _SDK_TEFMOD_
#define PHYMOD_ST const phymod_access_t
#else
#define PHYMOD_ST tefmod_st
#endif

#include "tefmod_sc_defines.h"
#include "tfPCSRegEnums.h"

#include "tefmod_sc_lkup_table.h"

const sc_pmd_entry_st sc_pmd_entry[] = {
  /*`SPEED_10G_CR1: 0*/             { OS_MODE_2, 4},
  /*`SPEED_10G_KR1: 1*/             { OS_MODE_2, 4},
  /*`SPEED_10G_X1: 2*/              { OS_MODE_2, 4},
  /*`NULL ENTRY: 3*/                {         0, 0},
  /*`SPEED_10G_HG2_CR1: 4*/         { OS_MODE_2, 5},
  /*`SPEED_10G_HG2_KR1: 5*/         { OS_MODE_2, 5},
  /*`SPEED_10G_HG2_X1: 6*/          { OS_MODE_2, 5},
  /*`NULL ENTRY: 7*/                {         0, 0},
  /*`SPEED_20G_CR1: 8*/             { OS_MODE_1, 4},
  /*`SPEED_20G_KR1: 9 */            { OS_MODE_1, 4},
  /*`SPEED_20G_X1: 10*/             { OS_MODE_1, 4},
  /*`NULL ENTRY: 11*/               {         0, 0},
  /*`SPEED_20G_HG2_CR1: 12*/        { OS_MODE_1, 5},
  /*`SPEED_20G_HG2_KR1: 13*/        { OS_MODE_1, 5},
  /*`SPEED_20G_HG2_X1: 14*/         { OS_MODE_1, 5},
  /*`NULL ENTRY: 15*/               {         0, 0},
  /*`SPEED_25G_CR1: 16*/            { OS_MODE_1, 7},
  /*`SPEED_25G_KR1:  17*/           { OS_MODE_1, 7},
  /*`SPEED_25G_X1: 18*/             { OS_MODE_1, 7},
  /*`NULL ENTRY: 19*/               {         0, 0},
  /*`SPEED_25G_HG2_CR1: 20*/        { OS_MODE_1, 10},
  /*`SPEED_25G_HG2_KR1: 21*/        { OS_MODE_1, 10},
  /*`SPEED_25G_HG2_X1: 22*/         { OS_MODE_1, 10},
  /*`NULL ENTRY: 23*/               {         0, 0},
  /*`SPEED_20G_CR2:  24*/           { OS_MODE_2, 4},
  /*`SPEED_20G_KR2: 25*/            { OS_MODE_2, 4},
  /*`SPEED_20G_X2: 26*/             { OS_MODE_2, 4},
  /*`NULL ENTRY: 27*/               {         0, 0},
  /*`SPEED_20G_HG2_CR2: 28*/        { OS_MODE_2, 5},
  /*`SPEED_20G_HG2_KR2: 29*/        { OS_MODE_2, 5},
  /*`SPEED_20G_HG2_X2: 30*/         { OS_MODE_2, 5},
  /*`NULL ENTRY: 31*/               {         0, 0},
  /*`SPEED_40G_CR2: 32*/            { OS_MODE_1, 4},
  /*`SPEED_40G_KR2: 33*/            { OS_MODE_1, 4},
  /*`SPEED_40G_X2: 34*/             { OS_MODE_1, 4},
  /*`NULL ENTRY: 35*/               {         0, 0},
  /*`SPEED_40G_HG2_CR2: 36*/        { OS_MODE_1, 5},
  /*`SPEED_40G_HG2_KR2: 37*/        { OS_MODE_1, 5},
  /*`SPEED_40G_HG2_X2: 38*/         { OS_MODE_1, 5},
  /*`NULL ENTRY: 39*/               {         0, 0},
  /*`SPEED_40G_CR4:  40*/           { OS_MODE_2, 4},
  /*`SPEED_40G_KR4:  41*/           { OS_MODE_2, 4},
  /*`SPEED_40G_X4: 42*/             { OS_MODE_2, 4},
  /*`NULL ENTRY: 43*/               {         0, 0},
  /*`SPEED_40G_HG2_CR4: 44*/        { OS_MODE_2, 5},
  /*`SPEED_40G_HG2_KR4: 45*/        { OS_MODE_2, 5},
  /*`SPEED_40G_HG2_X4: 46*/         { OS_MODE_2, 5},
  /*`NULL ENTRY: 47*/               {         0, 0},
  /*`SPEED_50G_CR2: 48*/            { OS_MODE_1, 7},
  /*`SPEED_50G_KR2: 49*/            { OS_MODE_1, 7},
  /*`SPEED_50G_X2: 50*/             { OS_MODE_1, 7},
  /*`NULL ENTRY: 51*/               {         0, 0},
  /*`SPEED_50G_HG2_CR2: 52*/        { OS_MODE_1, 10},
  /*`SPEED_50G_HG2_KR2: 53*/        { OS_MODE_1, 10},
  /*`SPEED_50G_HG2_X2: 54*/         { OS_MODE_1, 10},
  /*`NULL ENTRY: 55*/               {         0, 0},
  /*`SPEED_50G_CR4: 56*/            { OS_MODE_2, 7},
  /*`SPEED_50G_KR4:  57*/           { OS_MODE_2, 7},
  /*`SPEED_50G_X4: 58*/             { OS_MODE_2, 7},
  /*`NULL ENTRY: 59*/               {         0, 0},
  /*`SPEED_50G_HG2_CR4: 60*/        { OS_MODE_2, 7},
  /*`SPEED_50G_HG2_KR4: 61*/        { OS_MODE_2, 7},
  /*`SPEED_50G_HG2_X4: 62*/         { OS_MODE_2, 7},
  /*`NULL ENTRY: 63*/               {         0, 0},
  /*`SPEED_100G_CR4: 64*/           { OS_MODE_1, 7},
  /*`SPEED_100G_KR4: 65*/           { OS_MODE_1, 7},
  /*`SPEED_100G_X4: 66*/            { OS_MODE_1, 7},
  /*`NULL ENTRY: 67*/               {         0, 0},
  /*`SPEED_100G_HG2_CR4: 68*/       { OS_MODE_1, 10},
  /*`SPEED_100G_HG2_KR4: 69*/       { OS_MODE_1, 10},
  /*`SPEED_100G_HG2_X4: 70*/        { OS_MODE_1, 10},
  /*`NULL ENTRY: 71*/               {         0, 0},
  /*`SPEED_CL73_20GVCO: 72*/        { OS_MODE_16p5, 5},
  /*`NULL ENTRY: 73*/               {         0, 0},
  /*`NULL ENTRY: 74*/               {         0, 0},
  /*`NULL ENTRY: 75*/               {         0, 0},
  /*`NULL ENTRY: 76*/               {         0, 0},
  /*`NULL ENTRY: 77*/               {         0, 0},
  /*`NULL ENTRY: 78*/               {         0, 0},
  /*`NULL ENTRY: 79*/               {         0, 0},
  /*`SPEED_CL73_25GVCO: 80*/        { OS_MODE_20p625, 5},
  /*`NULL ENTRY: 81*/               {         0, 0},
  /*`NULL ENTRY: 82*/               {         0, 0},
  /*`NULL ENTRY: 83*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 86*/               {         0, 0},
  /*`NULL ENTRY: 87*/               {         0, 0},
  /*`SPEED_1G_20GVCO: 88*/          { OS_MODE_16p5, 4},
  /*`NULL ENTRY: 89*/               {         0, 0},
  /*`NULL ENTRY: 90*/               {         0, 0},
  /*`NULL ENTRY: 91*/               {         0, 0},
  /*`NULL ENTRY: 92*/               {         0, 0},
  /*`NULL ENTRY: 93*/               {         0, 0},
  /*`NULL ENTRY: 94*/               {         0, 0},
  /*`NULL ENTRY: 95*/               {         0, 0},
  /*`SPEED_1G_25GVCO: 96*/          { OS_MODE_20p625, 7},
  /*`NULL ENTRY: 97*/               {         0, 0},
  /*`SPEED_10G_25GVCO: 98*/         { OS_MODE_8, 6}
};

const sc_pmd_entry_st sc_pmd_entry_125M_ref[] = {
  /*`SPEED_10G_CR1: 0*/             { OS_MODE_2, 7},
  /*`SPEED_10G_KR1: 1*/             { OS_MODE_2, 7},
  /*`SPEED_10G_X1: 2*/              { OS_MODE_2, 7},
  /*`NULL ENTRY: 3*/                {         0, 0},
  /*`SPEED_10G_HG2_CR1: 4*/         { OS_MODE_2, 10},
  /*`SPEED_10G_HG2_KR1: 5*/         { OS_MODE_2, 10},
  /*`SPEED_10G_HG2_X1: 6*/          { OS_MODE_2, 10},
  /*`NULL ENTRY: 7*/                {         0, 0},
  /*`SPEED_20G_CR1: 8*/             { OS_MODE_1, 7},
  /*`SPEED_20G_KR1: 9 */            { OS_MODE_1, 7},
  /*`SPEED_20G_X1: 10*/             { OS_MODE_1, 7},
  /*`NULL ENTRY: 11*/               {         0, 0},
  /*`SPEED_20G_HG2_CR1: 12*/        { OS_MODE_1, 10},
  /*`SPEED_20G_HG2_KR1: 13*/        { OS_MODE_1, 10},
  /*`SPEED_20G_HG2_X1: 14*/         { OS_MODE_1, 10},
  /*`NULL ENTRY: 15*/               {         0, 0},
  /*`SPEED_25G_CR1: 16*/            { OS_MODE_1, 7},
  /*`SPEED_25G_KR1:  17*/           { OS_MODE_1, 7},
  /*`SPEED_25G_X1: 18*/             { OS_MODE_1, 7},
  /*`NULL ENTRY: 19*/               {         0, 0},
  /*`SPEED_25G_HG2_CR1: 20*/        { OS_MODE_1, 10},
  /*`SPEED_25G_HG2_KR1: 21*/        { OS_MODE_1, 10},
  /*`SPEED_25G_HG2_X1: 22*/         { OS_MODE_1, 10},
  /*`NULL ENTRY: 23*/               {         0, 0},
  /*`SPEED_20G_CR2:  24*/           { OS_MODE_2, 7},
  /*`SPEED_20G_KR2: 25*/            { OS_MODE_2, 7},
  /*`SPEED_20G_X2: 26*/             { OS_MODE_2, 7},
  /*`NULL ENTRY: 27*/               {         0, 0},
  /*`SPEED_20G_HG2_CR2: 28*/        { OS_MODE_2, 10},
  /*`SPEED_20G_HG2_KR2: 29*/        { OS_MODE_2, 10},
  /*`SPEED_20G_HG2_X2: 30*/         { OS_MODE_2, 10},
  /*`NULL ENTRY: 31*/               {         0, 0},
  /*`SPEED_40G_CR2: 32*/            { OS_MODE_1, 7},
  /*`SPEED_40G_KR2: 33*/            { OS_MODE_1, 7},
  /*`SPEED_40G_X2: 34*/             { OS_MODE_1, 7},
  /*`NULL ENTRY: 35*/               {         0, 0},
  /*`SPEED_40G_HG2_CR2: 36*/        { OS_MODE_1, 10},
  /*`SPEED_40G_HG2_KR2: 37*/        { OS_MODE_1, 10},
  /*`SPEED_40G_HG2_X2: 38*/         { OS_MODE_1, 10},
  /*`NULL ENTRY: 39*/               {         0, 0},
  /*`SPEED_40G_CR4:  40*/           { OS_MODE_2, 7},
  /*`SPEED_40G_KR4:  41*/           { OS_MODE_2, 7},
  /*`SPEED_40G_X4: 42*/             { OS_MODE_2, 7},
  /*`NULL ENTRY: 43*/               {         0, 0},
  /*`SPEED_40G_HG2_CR4: 44*/        { OS_MODE_2, 10},
  /*`SPEED_40G_HG2_KR4: 45*/        { OS_MODE_2, 10},
  /*`SPEED_40G_HG2_X4: 46*/         { OS_MODE_2, 10},
  /*`NULL ENTRY: 47*/               {         0, 0},
  /*`SPEED_50G_CR2: 48*/            { OS_MODE_1, 7},
  /*`SPEED_50G_KR2: 49*/            { OS_MODE_1, 7},
  /*`SPEED_50G_X2: 50*/             { OS_MODE_1, 7},
  /*`NULL ENTRY: 51*/               {         0, 0},
  /*`SPEED_50G_HG2_CR2: 52*/        { OS_MODE_1, 10},
  /*`SPEED_50G_HG2_KR2: 53*/        { OS_MODE_1, 10},
  /*`SPEED_50G_HG2_X2: 54*/         { OS_MODE_1, 10},
  /*`NULL ENTRY: 55*/               {         0, 0},
  /*`SPEED_50G_CR4: 56*/            { OS_MODE_2, 7},
  /*`SPEED_50G_KR4:  57*/           { OS_MODE_2, 7},
  /*`SPEED_50G_X4: 58*/             { OS_MODE_2, 7},
  /*`NULL ENTRY: 59*/               {         0, 0},
  /*`SPEED_50G_HG2_CR4: 60*/        { OS_MODE_2, 7},
  /*`SPEED_50G_HG2_KR4: 61*/        { OS_MODE_2, 7},
  /*`SPEED_50G_HG2_X4: 62*/         { OS_MODE_2, 7},
  /*`NULL ENTRY: 63*/               {         0, 0},
  /*`SPEED_100G_CR4: 64*/           { OS_MODE_1, 7},
  /*`SPEED_100G_KR4: 65*/           { OS_MODE_1, 7},
  /*`SPEED_100G_X4: 66*/            { OS_MODE_1, 7},
  /*`NULL ENTRY: 67*/               {         0, 0},
  /*`SPEED_100G_HG2_CR4: 68*/       { OS_MODE_1, 10},
  /*`SPEED_100G_HG2_KR4: 69*/       { OS_MODE_1, 10},
  /*`SPEED_100G_HG2_X4: 70*/        { OS_MODE_1, 10},
  /*`NULL ENTRY: 71*/               {         0, 0},
  /*`SPEED_CL73_20GVCO: 72*/        { OS_MODE_16p5, 10},
  /*`NULL ENTRY: 73*/               {         0, 0},
  /*`NULL ENTRY: 74*/               {         0, 0},
  /*`NULL ENTRY: 75*/               {         0, 0},
  /*`NULL ENTRY: 76*/               {         0, 0},
  /*`NULL ENTRY: 77*/               {         0, 0},
  /*`NULL ENTRY: 78*/               {         0, 0},
  /*`NULL ENTRY: 79*/               {         0, 0},
  /*`SPEED_CL73_25GVCO: 80*/        { OS_MODE_20p625, 10},
  /*`NULL ENTRY: 81*/               {         0, 0},
  /*`NULL ENTRY: 82*/               {         0, 0},
  /*`NULL ENTRY: 83*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 84*/               {         0, 0},
  /*`NULL ENTRY: 86*/               {         0, 0},
  /*`NULL ENTRY: 87*/               {         0, 0},
  /*`SPEED_1G_20GVCO: 88*/          { OS_MODE_16p5, 7},
  /*`NULL ENTRY: 89*/               {         0, 0},
  /*`NULL ENTRY: 90*/               {         0, 0},
  /*`NULL ENTRY: 91*/               {         0, 0},
  /*`NULL ENTRY: 92*/               {         0, 0},
  /*`NULL ENTRY: 93*/               {         0, 0},
  /*`NULL ENTRY: 94*/               {         0, 0},
  /*`NULL ENTRY: 95*/               {         0, 0},
  /*`SPEED_1G_25GVCO: 96*/          { OS_MODE_20p625, 7},
  /*`NULL ENTRY: 97*/               {         0, 0},
  /*`SPEED_10G_25GVCO: 98*/         { OS_MODE_8, 13}
};


int tefmod_get_mapped_speed(tefmod_spd_intfc_type_t spd_intf, int *speed) 
{
  /* speeds covered in Forced-speed */
  if(spd_intf == TEFMOD_SPD_10000_XFI)           *speed = sc_x4_control_sc_S_10G_X1;
  if(spd_intf == TEFMOD_SPD_10600_XFI_HG)        *speed = sc_x4_control_sc_S_10G_HG2_X1;
  if(spd_intf == TEFMOD_SPD_20000_XFI)           *speed = sc_x4_control_sc_S_20G_X1;
  if(spd_intf == TEFMOD_SPD_21200_XFI_HG)        *speed = sc_x4_control_sc_S_20G_HG2_X1;
  if(spd_intf == TEFMOD_SPD_25000_XFI)           *speed = sc_x4_control_sc_S_25G_X1;
  if(spd_intf == TEFMOD_SPD_26500_XFI_HG)        *speed = sc_x4_control_sc_S_25G_HG2_X1;
  if(spd_intf == TEFMOD_SPD_20G_MLD_X2)          *speed = sc_x4_control_sc_S_20G_X2;
  if(spd_intf == TEFMOD_SPD_21G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_20G_HG2_X2;
  if(spd_intf == TEFMOD_SPD_40G_MLD_X2)          *speed = sc_x4_control_sc_S_40G_X2;
  if(spd_intf == TEFMOD_SPD_42G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_40G_HG2_X2;
  if(spd_intf == TEFMOD_SPD_40G_MLD_X4)          *speed = sc_x4_control_sc_S_40G_X4;
  if(spd_intf == TEFMOD_SPD_42G_MLD_HG_X4)       *speed = sc_x4_control_sc_S_40G_HG2_X4;
  if(spd_intf == TEFMOD_SPD_50G_MLD_X2)          *speed = sc_x4_control_sc_S_50G_X2;
  if(spd_intf == TEFMOD_SPD_53G_MLD_HG_X2)       *speed = sc_x4_control_sc_S_50G_HG2_X2;
  if(spd_intf == TEFMOD_SPD_50G_MLD_X4)          *speed = sc_x4_control_sc_S_50G_X4;
  if(spd_intf == TEFMOD_SPD_53G_MLD_HG_X4)       *speed = sc_x4_control_sc_S_50G_HG2_X4;
  if(spd_intf == TEFMOD_SPD_100G_MLD_X4)         *speed = sc_x4_control_sc_S_100G_X4;
  if(spd_intf == TEFMOD_SPD_106G_MLD_HG_X4)      *speed = sc_x4_control_sc_S_100G_HG2_X4;
  /*  These are the speeds that are not in forced mode or AN mode */
  if(spd_intf == TEFMOD_SPD_10000_XFI_CR1)       *speed = sc_x4_control_sc_S_10G_CR1;
  if(spd_intf == TEFMOD_SPD_10600_XFI_HG_CR1)    *speed = sc_x4_control_sc_S_10G_HG2_CR1;
  if(spd_intf == TEFMOD_SPD_CL73_20G)            *speed = sc_x4_control_sc_S_CL73_20GVCO;
  if(spd_intf == TEFMOD_SPD_CL73_25G)            *speed = sc_x4_control_sc_S_CL73_25GVCO;
  if(spd_intf == TEFMOD_SPD_1G_20G)              *speed = sc_x4_control_sc_S_CL36_20GVCO;
  if(spd_intf == TEFMOD_SPD_1G_25G)              *speed = sc_x4_control_sc_S_CL36_25GVCO;
  if(spd_intf == TEFMOD_SPD_10G_25G)             *speed = sc_x4_control_sc_S_10G_25GVCO;


  return PHYMOD_E_NONE;
}
#ifdef _DV_TB_
int tefmod_get_ht_entries(PHYMOD_ST* pc) 
{

  int or_en;
  or_en = pc->per_field_override_en;


  if((or_en & 0x00010000) == 0x00010000) {
  } else {
    pc->num_lanes =  	sc_ht_pcs[pc->speed].num_lanes;
  }

  if(((or_en & 0x00020000)   == 0x00020000)  && (or_en >> 16 == 2)) {
  } else {
    pc->os_mode =  	sc_ht_pcs[pc->speed].os_mode;
  }

  if(((or_en & 0x00040000) == 0x00040000)  && (or_en >> 16 == 4)) {
  } else {
    pc->t_fifo_mode =  	sc_ht_pcs[pc->speed].t_fifo_mode;
  }

  if(((or_en & 0x00080000) == 0x00080000)  && (or_en >> 16 == 8)) {
  } else {
    pc->t_enc_mode =  	sc_ht_pcs[pc->speed].t_enc_mode;
  }

  if(((or_en & 0x00100000)  == 0x00100000)  && (or_en >> 20 == 1)) {
  } else {
    pc->t_HG2_ENABLE =  	sc_ht_pcs[pc->speed].t_HG2_ENABLE;
  }

  if(((or_en & 0x00200000)  == 0x00200000)  && (or_en >> 20 == 2)) {
  } else {
    pc->t_pma_btmx_mode = sc_ht_pcs[pc->speed].t_pma_btmx_mode;
  }

  if(((or_en & 0x00400000)  == 0x00400000)  && (or_en >> 20 == 4)) {
  } else {
    pc->scr_mode =  	sc_ht_pcs[pc->speed].scr_mode;
  }
  if(((or_en & 0x01000000) == 0x01000000)  && (or_en >> 24 == 1)) {
  } else {
    pc->descr_mode =  	sc_ht_pcs[pc->speed].descr_mode;
  }
  if(((or_en & 0x02000000) == 0x02000000)  && (or_en >> 24 == 2)) {
  } else {
    pc->dec_tl_mode =  	sc_ht_pcs[pc->speed].dec_tl_mode;
  }
  if(((or_en & 0x04000000) == 0x04000000)  && (or_en >> 24 == 4)) {
  } else {
    pc->deskew_mode =  	sc_ht_pcs[pc->speed].deskew_mode;
    if((pc->speed == sc_x1_speed_override0_sc_S_100G_CR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_KR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_X4     ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_CR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_KR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_X4 )) {
      if((pc->cl91_en == 1) || (pc->an_en)) {
        pc->deskew_mode =  	R_DESKEW_MODE_CL91;
      }
    }
  }
  if(((or_en & 0x08000000) == 0x08000000)  && (or_en >> 24 == 8)) {
  } else {
    pc->dec_fsm_mode =  	sc_ht_pcs[pc->speed].dec_fsm_mode;
  }
    pc->r_HG2_ENABLE =  	sc_ht_pcs[pc->speed].r_HG2_ENABLE;
  if((or_en & 0x80020000) == 0x80020000) {
  } else {
    pc->bs_sm_sync_mode = sc_ht_pcs[pc->speed].bs_sm_sync_mode;
  }
  if((or_en & 0x80040000) == 0x80040000) {
  } else {
    pc->bs_sync_en =      sc_ht_pcs[pc->speed].bs_sync_en;
    if((pc->speed == sc_x1_speed_override0_sc_S_100G_CR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_KR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_X4     ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_CR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_KR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_X4 )) {
      if((pc->cl91_en == 1) || (pc->an_en == 1)) {
        pc->bs_sync_en = 0;
      }
    }
  }
  if((or_en & 0x80080000) == 0x80080000) {
  } else {
    pc->bs_dist_mode =    sc_ht_pcs[pc->speed].bs_dist_mode;
    if((pc->speed == sc_x1_speed_override0_sc_S_100G_CR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_KR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_X4     ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_CR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_KR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_X4 )) {
      if((pc->cl91_en == 1) || (pc->an_en == 1)) {
        pc->bs_dist_mode = 0;
      }
    }
  }
  if((or_en & 0x80100000) == 0x80100000) {
  } else {
    pc->bs_btmx_mode =    sc_ht_pcs[pc->speed].bs_btmx_mode;
    if((pc->speed == sc_x1_speed_override0_sc_S_100G_CR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_KR4    ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_X4     ) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_CR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_KR4) ||
       (pc->speed == sc_x1_speed_override0_sc_S_100G_HG2_X4 )) {
      if((pc->cl91_en == 1) || (pc->an_en == 1)) {
        pc->bs_btmx_mode = 0;
      }
    }
  }
  if((or_en & 0x80200000) == 0x80200000) {
  } else {
    pc->cl72_en =         sc_ht_pcs[pc->speed].cl72_en;
  }
  if((or_en & 0xf0400000) == 0xf0400000) {
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->clkcnt0 =  	sc_ht_pcs[pc->speed].clkcnt0;
    } else {
      pc->clkcnt0 =  	sc_ht_pcs[pc->speed].clkcnt0_by48;
    }
  }
  if((or_en & 0x80800000) == 0x80800000) {
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->clkcnt1 =  	sc_ht_pcs[pc->speed].clkcnt1;
    } else {
      pc->clkcnt1 =  	sc_ht_pcs[pc->speed].clkcnt1_by48;
    }
  }
  if((or_en & 0x81000000) == 0x81000000) {
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->lpcnt0 =  	sc_ht_pcs[pc->speed].lpcnt0;
    } else {
      pc->lpcnt0 =  	sc_ht_pcs[pc->speed].lpcnt0_by48;
    }
  }
  if((or_en & 0x82000000) == 0x82000000) {
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->lpcnt1 =  	sc_ht_pcs[pc->speed].lpcnt1;
    } else {
      pc->lpcnt1 =  	sc_ht_pcs[pc->speed].lpcnt1_by48;
    }
  }
  if((or_en & 0x84000000) == 0x84000000) {
  } else {
    if(pc->tsc_clk_freq_pll_by_48 == 0) {
      pc->cgc =  		sc_ht_pcs[pc->speed].cgc;
    } else {
      pc->cgc =  		sc_ht_pcs[pc->speed].cgc_by48;
    }
  }

  return PHYMOD_E_NONE;
}

#endif

int tefmod_print_sc_lkup_table(PHYMOD_ST* pc) 
{
  #ifdef _DV_TB_
  printf("PRINTING THE SC TABLE .....\n");
  printf("NUM_LANES is %d\n", pc->num_lanes);
  printf("OS_MODE is %d\n", pc->os_mode);
  printf("T_FIFO_MODE is %d\n", pc->t_fifo_mode);
  printf("T_ENC_MODE is %d\n", pc->t_enc_mode);
  printf("T_HG2_ENABLE is %d\n", pc->t_HG2_ENABLE);
  printf("T_PMA_BTMX_MODE is %d\n", pc->t_pma_btmx_mode);
  printf("SCR_MODE is %d\n", pc->scr_mode);
  printf("DESCR_MODE is %d\n", pc->descr_mode);
  printf("DEC_TL_MODE is %d\n", pc->dec_tl_mode);
  printf("DESKEW_MODE is %d\n", pc->deskew_mode);
  printf("DEC_FSM_MODE is %d\n", pc->dec_fsm_mode);
  printf("R_HG2_ENABLE is %d\n", pc->r_HG2_ENABLE);
  printf("BS_SM_SYNC_MODE is %d\n", pc->bs_sm_sync_mode);
  printf("BS_SYNC_EN is %d\n", pc->bs_sync_en);
  printf("BS_DIST_MODE is %d\n", pc->bs_dist_mode);
  printf("BS_BTMX_MODE is %d\n", pc->bs_btmx_mode);
  printf("CL72_EN is %d\n", pc->cl72_en);
  printf("CLKCNT0 is %d\n", pc->clkcnt0);
  printf("CLKCNT1 is %d\n", pc->clkcnt1);
  printf("LOOPCNT0 is %d\n", pc->lpcnt0);
  printf("LOOPCNT1 is %d\n", pc->lpcnt1);
  printf("CGC is %d\n", pc->cgc);
  #endif

  return PHYMOD_E_NONE;
}


#ifdef _DV_TB_
int tefmod_sc_lkup_table(PHYMOD_ST* pc)
{
  int mapped_speed;
  tefmod_get_mapped_speed(pc->spd_intf, &mapped_speed);
  tefmod_get_ht_entries(pc);
  return PHYMOD_E_NONE;
}
#endif

#ifdef _DV_TB_
int tefmod_fill_lkup_exp(PHYMOD_ST* pc)
{
     tefmod_sc_lkup_table(pc);

 return PHYMOD_E_NONE;
}
#endif

