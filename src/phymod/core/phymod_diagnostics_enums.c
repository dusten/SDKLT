/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 2013/09/12 10:43:06 nirf Exp $
 * 
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
 *     
 * DO NOT EDIT THIS FILE!
 */

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_DIAG

enum_mapping_t phymod_prbs_poly_t_mapping[] = {
    {"7", phymodPrbsPoly7},
    {"9", phymodPrbsPoly9},
    {"11", phymodPrbsPoly11},
    {"15", phymodPrbsPoly15},
    {"23", phymodPrbsPoly23},
    {"31", phymodPrbsPoly31},
    {"58", phymodPrbsPoly58},
    {"49", phymodPrbsPoly49},
    {"10", phymodPrbsPoly10},
    {"20", phymodPrbsPoly20},
    {"13", phymodPrbsPoly13},
    {"Q13", phymodPrbsPolyQ13},
    {NULL, 0}
};

enum_mapping_t phymod_pmd_mode_t_mapping[] = {
    {"Os", phymodPmdModeOs},
    {"OsDfe", phymodPmdModeOsDfe},
    {"BrDfe", phymodPmdModeBrDfe},
    {NULL, 0}
};

enum_mapping_t phymod_eyescan_mode_t_mapping[] = {
    {"Fast", phymodEyescanModeFast},
    {"LowBER", phymodEyescanModeLowBER},
    {"BERProj", phymodEyescanModeBERProj},
    {NULL, 0}
};

enum_mapping_t phymod_link_monitor_mode_t_mapping[] = {
    {"PCS49_1x10G", phymodLinkMonPCS49_1x10G},
    {"PCS82_4x10G", phymodLinkMonPCS82_4x10G},
    {"PCS82_2x25G", phymodLinkMonPCS82_2x25G},
    {"PCS82_4x25G", phymodLinkMonPCS82_4x25G},
    {"FC4", phymodLinkMonFC4},
    {"FC8", phymodLinkMonFC8},
    {"FC16", phymodLinkMonFC16},
    {"FC32", phymodLinkMonFC32},
    {NULL, 0}
};

enum_mapping_t phymod_diag_type_t_mapping[] = {
    {"GENERAL", phymod_diag_GENERAL},
    {"TOPOLOGY", phymod_diag_TOPOLOGY},
    {"LINK ", phymod_diag_LINK },
    {"SPEED", phymod_diag_SPEED},
    {"ANEG", phymod_diag_ANEG},
    {"TFC", phymod_diag_TFC},
    {"AN_TIMERS", phymod_diag_AN_TIMERS},
    {"STATE", phymod_diag_STATE},
    {"DEBUG", phymod_diag_DEBUG},
    {"IEEE", phymod_diag_IEEE},
    {"EEE", phymod_diag_EEE},
    {"BER", phymod_diag_BER},
    {"CFG", phymod_diag_CFG},
    {"CL72", phymod_diag_CL72},
    {"DSC", phymod_diag_DSC},
    {"DSC_STD", phymod_diag_DSC_STD},
    {"ALL", phymod_diag_ALL},
    {NULL, 0}
};

enum_mapping_t phymod_PAM4_tx_pattern_t_mapping[] = {
    {"PAM4TxPattern_JP03B", phymod_PAM4TxPattern_JP03B},
    {"PAM4TxPattern_Linear", phymod_PAM4TxPattern_Linear},
    {NULL, 0}
};

enum_mapping_t phymod_ber_proj_mode_t_mapping[] = {
    {"PostFEC", phymodBERProjModePostFEC},
    {NULL, 0}
};

#endif /*PHYMOD_DIAG*/
