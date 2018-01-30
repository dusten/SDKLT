/*! \file bcm56960_a0_bcmtm_flexport_glue.c
 *
 * File containing glue functions for Flexport and TDM library
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



#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_internal_socmem_glue.h>

#include <bcm56960_a0/bcm56960_a0_bcmtm_drv.h>
#include <bcm56960_a0/bcm56960_a0_bcmtm_sid_alias.h>


/*******************************************************************************
 * Private functions
 */
static void
bcm56960_a0_bcmtm_mmu_pt_sid_rename(int unit, bcmdrd_sid_t in_sid, int in_inst,
                                    bcmdrd_sid_t *out_sid, int *instance)
{

    int inst_out = *instance;
    soc_mem_t sid_out;

    switch (in_sid) {
    case CT_PURGE_CNT_XPE0r :
        if (in_inst == 0) {
            sid_out = CT_PURGE_CNT_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = CT_PURGE_CNT_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case CT_PURGE_CNT_XPE1r :
        if (in_inst == 0) {
            sid_out = CT_PURGE_CNT_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = CT_PURGE_CNT_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case CT_PURGE_CNT_XPE2r :
        if (in_inst == 0) {
            sid_out = CT_PURGE_CNT_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = CT_PURGE_CNT_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case CT_PURGE_CNT_XPE3r :
        if (in_inst == 0) {
            sid_out = CT_PURGE_CNT_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = CT_PURGE_CNT_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE0r :
        sid_out = MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE0_SLICE0r;
        inst_out = 0;
        break;
    case MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE1r :
        sid_out = MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE1_SLICE1r;
        inst_out = 0;
        break;
    case MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE2r :
        sid_out = MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE2_SLICE0r;
        inst_out = 0;
        break;
    case MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE3r :
        sid_out = MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE3_SLICE1r;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_MCUC_BST_THRESHOLD_XPE0r :
        sid_out = MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_MCUC_BST_THRESHOLD_XPE1r :
        sid_out = MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_MCUC_BST_THRESHOLD_XPE2r :
        sid_out = MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_MCUC_BST_THRESHOLD_XPE3r :
        sid_out = MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_POOL_MC_BST_THRESHOLD_XPE0r :
        sid_out = MMU_THDM_DB_POOL_MC_BST_THRESHOLDr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_MC_BST_THRESHOLD_XPE1r :
        sid_out = MMU_THDM_DB_POOL_MC_BST_THRESHOLDr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_MC_BST_THRESHOLD_XPE2r :
        sid_out = MMU_THDM_DB_POOL_MC_BST_THRESHOLDr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_MC_BST_THRESHOLD_XPE3r :
        sid_out = MMU_THDM_DB_POOL_MC_BST_THRESHOLDr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_POOL_RED_RESUME_LIMIT_XPE0r :
        sid_out = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_RED_RESUME_LIMIT_XPE1r :
        sid_out = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_RED_RESUME_LIMIT_XPE2r :
        sid_out = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_RED_RESUME_LIMIT_XPE3r :
        sid_out = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_POOL_RED_SHARED_LIMIT_XPE0r :
        sid_out = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_RED_SHARED_LIMIT_XPE1r :
        sid_out = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_RED_SHARED_LIMIT_XPE2r :
        sid_out = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_RED_SHARED_LIMIT_XPE3r :
        sid_out = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_POOL_RESUME_LIMIT_XPE0r :
        sid_out = MMU_THDM_DB_POOL_RESUME_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_RESUME_LIMIT_XPE1r :
        sid_out = MMU_THDM_DB_POOL_RESUME_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_RESUME_LIMIT_XPE2r :
        sid_out = MMU_THDM_DB_POOL_RESUME_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_RESUME_LIMIT_XPE3r :
        sid_out = MMU_THDM_DB_POOL_RESUME_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_POOL_SHARED_LIMIT_XPE0r :
        sid_out = MMU_THDM_DB_POOL_SHARED_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_SHARED_LIMIT_XPE1r :
        sid_out = MMU_THDM_DB_POOL_SHARED_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_SHARED_LIMIT_XPE2r :
        sid_out = MMU_THDM_DB_POOL_SHARED_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_SHARED_LIMIT_XPE3r :
        sid_out = MMU_THDM_DB_POOL_SHARED_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_POOL_YELLOW_RESUME_LIMIT_XPE0r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_YELLOW_RESUME_LIMIT_XPE1r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_YELLOW_RESUME_LIMIT_XPE2r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_YELLOW_RESUME_LIMIT_XPE3r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_POOL_YELLOW_SHARED_LIMIT_XPE0r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_POOL_YELLOW_SHARED_LIMIT_XPE1r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_POOL_YELLOW_SHARED_LIMIT_XPE2r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_POOL_YELLOW_SHARED_LIMIT_XPE3r :
        sid_out = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_PORTSP_BST_THRESHOLD_XPE0r :
        sid_out = MMU_THDM_DB_PORTSP_BST_THRESHOLDr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_THRESHOLD_XPE1r :
        sid_out = MMU_THDM_DB_PORTSP_BST_THRESHOLDr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_BST_THRESHOLD_XPE2r :
        sid_out = MMU_THDM_DB_PORTSP_BST_THRESHOLDr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_PORTSP_BST_THRESHOLD_XPE3r :
        sid_out = MMU_THDM_DB_PORTSP_BST_THRESHOLDr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_XPE0r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_XPE1r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_XPE2r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_XPE3r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_DB_PORTSP_DROP_STATE_BMP_64_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_XPE0r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_XPE1r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_XPE2r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_XPE3r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_SHARED_COUNT_XPE0r :
        sid_out = MMU_THDM_DB_PORTSP_SHARED_COUNTr;
            inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_SHARED_COUNT_XPE1r :
        sid_out = MMU_THDM_DB_PORTSP_SHARED_COUNTr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_SHARED_COUNT_XPE2r :
        sid_out = MMU_THDM_DB_PORTSP_SHARED_COUNTr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_PORTSP_SHARED_COUNT_XPE3r :
        sid_out = MMU_THDM_DB_PORTSP_SHARED_COUNTr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SEL_XPE0r :
        sid_out = MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SEL_XPE1r :
        sid_out = MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SEL_XPE2r :
        sid_out = MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SEL_XPE3r :
        sid_out = MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 3;
        break;
    case MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_XPE0r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_XPE1r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_XPE2r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_XPE3r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_DB_PORTSP_YELLOW_DROP_STATE_BMP_64_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE0r :
        sid_out = MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE1r :
        sid_out = MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE2r :
        sid_out = MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE3r :
        sid_out = MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE0r :
        sid_out =
MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE0_SLICE0r;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE1r :
        sid_out =
MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE1_SLICE1r;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE2r :
        sid_out =
MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE2_SLICE0r;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE3r :
        sid_out =
MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE_MMU_XPE3_SLICE1r;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_MC_BST_THRESHOLD_XPE0r :
        sid_out = MMU_THDM_MCQE_POOL_MC_BST_THRESHOLDr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_MC_BST_THRESHOLD_XPE1r :
        sid_out = MMU_THDM_MCQE_POOL_MC_BST_THRESHOLDr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_POOL_MC_BST_THRESHOLD_XPE2r :
        sid_out = MMU_THDM_MCQE_POOL_MC_BST_THRESHOLDr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_POOL_MC_BST_THRESHOLD_XPE3r :
        sid_out = MMU_THDM_MCQE_POOL_MC_BST_THRESHOLDr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_POOL_RED_RESUME_LIMIT_XPE0r :
        sid_out = MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_RED_RESUME_LIMIT_XPE1r :
        sid_out = MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_POOL_RED_RESUME_LIMIT_XPE2r :
        sid_out = MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_POOL_RED_RESUME_LIMIT_XPE3r :
        sid_out = MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_POOL_RED_SHARED_LIMIT_XPE0r :
        sid_out = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_RED_SHARED_LIMIT_XPE1r :
        sid_out = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_POOL_RED_SHARED_LIMIT_XPE2r :
        sid_out = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_POOL_RED_SHARED_LIMIT_XPE3r :
        sid_out = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_POOL_RESUME_LIMIT_XPE0r :
        sid_out = MMU_THDM_MCQE_POOL_RESUME_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_RESUME_LIMIT_XPE1r :
        sid_out = MMU_THDM_MCQE_POOL_RESUME_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_POOL_RESUME_LIMIT_XPE2r :
        sid_out = MMU_THDM_MCQE_POOL_RESUME_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_POOL_RESUME_LIMIT_XPE3r :
        sid_out = MMU_THDM_MCQE_POOL_RESUME_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_POOL_SHARED_LIMIT_XPE0r :
        sid_out = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_SHARED_LIMIT_XPE1r :
        sid_out = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_POOL_SHARED_LIMIT_XPE2r :
        sid_out = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_POOL_SHARED_LIMIT_XPE3r :
        sid_out = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMIT_XPE0r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMIT_XPE1r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMIT_XPE2r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMIT_XPE3r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMIT_XPE0r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMIT_XPE1r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMIT_XPE2r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMIT_XPE3r :
        sid_out = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_THRESHOLD_XPE0r :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_THRESHOLDr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_THRESHOLD_XPE1r :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_THRESHOLDr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_THRESHOLD_XPE2r :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_THRESHOLDr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_THRESHOLD_XPE3r :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_THRESHOLDr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_XPE0r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_XPE1r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_XPE2r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_XPE3r :
        if (in_inst == 0) {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = MMU_THDM_MCQE_PORTSP_DROP_STATE_BMP_64_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_XPE0r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_XPE1r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_XPE2r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_XPE3r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_RED_DROP_STATE_BMP_64_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_SHARED_COUNT_XPE0r :
        sid_out = MMU_THDM_MCQE_PORTSP_SHARED_COUNTr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_SHARED_COUNT_XPE1r :
        sid_out = MMU_THDM_MCQE_PORTSP_SHARED_COUNTr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_PORTSP_SHARED_COUNT_XPE2r :
        sid_out = MMU_THDM_MCQE_PORTSP_SHARED_COUNTr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_PORTSP_SHARED_COUNT_XPE3r :
        sid_out = MMU_THDM_MCQE_PORTSP_SHARED_COUNTr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SEL_XPE0r :
        sid_out = MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SEL_XPE1r :
        sid_out = MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SEL_XPE2r :
        sid_out = MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SEL_XPE3r :
        sid_out = MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SELr;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_XPE0r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_XPE1r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_XPE2r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_XPE3r :
        if (in_inst == 0) {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out =
MMU_THDM_MCQE_PORTSP_YELLOW_DROP_STATE_BMP0_64_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE0r :
        sid_out = MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE1r :
        sid_out = MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE2r :
        sid_out = MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILE_XPE3r :
        sid_out = MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILEr;
        inst_out = 3;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_PRIQ_XPE0r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_PRIQr;
        inst_out = 0;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_PRIQ_XPE1r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_PRIQr;
        inst_out = 1;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_PRIQ_XPE2r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_PRIQr;
        inst_out = 2;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_PRIQ_XPE3r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_PRIQr;
        inst_out = 3;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_SP_XPE0r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_SPr;
        inst_out = 0;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_SP_XPE1r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_SPr;
        inst_out = 1;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_SP_XPE2r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_SPr;
        inst_out = 2;
        break;
    case MMU_THDR_DB_BST_THRESHOLD_SP_XPE3r :
        sid_out = MMU_THDR_DB_BST_THRESHOLD_SPr;
        inst_out = 3;
        break;
    case MMU_THDR_DB_CONFIG_SP_XPE0r :
        sid_out = MMU_THDR_DB_CONFIG_SPr;
        inst_out = 0;
        break;
    case MMU_THDR_DB_CONFIG_SP_XPE1r :
        sid_out = MMU_THDR_DB_CONFIG_SPr;
        inst_out = 1;
        break;
    case MMU_THDR_DB_CONFIG_SP_XPE2r :
        sid_out = MMU_THDR_DB_CONFIG_SPr;
        inst_out = 2;
        break;
    case MMU_THDR_DB_CONFIG_SP_XPE3r :
        sid_out = MMU_THDR_DB_CONFIG_SPr;
        inst_out = 3;
        break;
    case MMU_THDR_DB_RESUME_COLOR_LIMIT_SP_XPE0r :
        sid_out = MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr;
        inst_out = 0;
        break;
    case MMU_THDR_DB_RESUME_COLOR_LIMIT_SP_XPE1r :
        sid_out = MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr;
        inst_out = 1;
        break;
    case MMU_THDR_DB_RESUME_COLOR_LIMIT_SP_XPE2r :
        sid_out = MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr;
        inst_out = 2;
        break;
    case MMU_THDR_DB_RESUME_COLOR_LIMIT_SP_XPE3r :
        sid_out = MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr;
        inst_out = 3;
        break;
    case MMU_THDR_DB_SP_SHARED_LIMIT_XPE0r :
        sid_out = MMU_THDR_DB_SP_SHARED_LIMITr;
        inst_out = 0;
        break;
    case MMU_THDR_DB_SP_SHARED_LIMIT_XPE1r :
        sid_out = MMU_THDR_DB_SP_SHARED_LIMITr;
        inst_out = 1;
        break;
    case MMU_THDR_DB_SP_SHARED_LIMIT_XPE2r :
        sid_out = MMU_THDR_DB_SP_SHARED_LIMITr;
        inst_out = 2;
        break;
    case MMU_THDR_DB_SP_SHARED_LIMIT_XPE3r :
        sid_out = MMU_THDR_DB_SP_SHARED_LIMITr;
        inst_out = 3;
        break;
    case OP_UC_PORT_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_RED_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_RED_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_RED_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_RED_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_RED_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_YELLOW_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_YELLOW_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_YELLOW_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_PORT_YELLOW_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_PORT_YELLOW_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_RED_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_RED_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_RED_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_RED_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_RED_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_YELLOW_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_YELLOW_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_YELLOW_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QGROUP_YELLOW_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QGROUP_YELLOW_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_RED_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_RED_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_RED_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_RED_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_RED_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_YELLOW_DROP_STATE_XPE0r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_YELLOW_DROP_STATE_XPE1r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_YELLOW_DROP_STATE_XPE2r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case OP_UC_QUEUE_YELLOW_DROP_STATE_XPE3r :
        if (in_inst == 0) {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = OP_UC_QUEUE_YELLOW_DROP_STATE_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_BST_PG_HDRM_PROFILE_XPE0r :
        sid_out = THDI_BST_PG_HDRM_PROFILEr;
        inst_out = 0;
        break;
    case THDI_BST_PG_HDRM_PROFILE_XPE1r :
        sid_out = THDI_BST_PG_HDRM_PROFILEr;
        inst_out = 1;
        break;
    case THDI_BST_PG_HDRM_PROFILE_XPE2r :
        sid_out = THDI_BST_PG_HDRM_PROFILEr;
        inst_out = 2;
        break;
    case THDI_BST_PG_HDRM_PROFILE_XPE3r :
        sid_out = THDI_BST_PG_HDRM_PROFILEr;
        inst_out = 3;
        break;
    case THDI_BST_PG_SHARED_PROFILE_XPE0r :
        sid_out = THDI_BST_PG_SHARED_PROFILEr;
        inst_out = 0;
        break;
    case THDI_BST_PG_SHARED_PROFILE_XPE1r :
        sid_out = THDI_BST_PG_SHARED_PROFILEr;
        inst_out = 1;
        break;
    case THDI_BST_PG_SHARED_PROFILE_XPE2r :
        sid_out = THDI_BST_PG_SHARED_PROFILEr;
        inst_out = 2;
        break;
    case THDI_BST_PG_SHARED_PROFILE_XPE3r :
        sid_out = THDI_BST_PG_SHARED_PROFILEr;
        inst_out = 3;
        break;
    case THDI_BST_SP_GLOBAL_SHARED_PROFILE_XPE0r :
        sid_out = THDI_BST_SP_GLOBAL_SHARED_PROFILEr;
        inst_out = 0;
        break;
    case THDI_BST_SP_GLOBAL_SHARED_PROFILE_XPE1r :
        sid_out = THDI_BST_SP_GLOBAL_SHARED_PROFILEr;
        inst_out = 1;
        break;
    case THDI_BST_SP_GLOBAL_SHARED_PROFILE_XPE2r :
        sid_out = THDI_BST_SP_GLOBAL_SHARED_PROFILEr;
        inst_out = 2;
        break;
    case THDI_BST_SP_GLOBAL_SHARED_PROFILE_XPE3r :
        sid_out = THDI_BST_SP_GLOBAL_SHARED_PROFILEr;
        inst_out = 3;
        break;
    case THDI_BST_SP_SHARED_PROFILE_XPE0r :
        sid_out = THDI_BST_SP_SHARED_PROFILEr;
        inst_out = 0;
        break;
    case THDI_BST_SP_SHARED_PROFILE_XPE1r :
        sid_out = THDI_BST_SP_SHARED_PROFILEr;
        inst_out = 1;
        break;
    case THDI_BST_SP_SHARED_PROFILE_XPE2r :
        sid_out = THDI_BST_SP_SHARED_PROFILEr;
        inst_out = 2;
        break;
    case THDI_BST_SP_SHARED_PROFILE_XPE3r :
        sid_out = THDI_BST_SP_SHARED_PROFILEr;
        inst_out = 3;
        break;
    case THDI_BST_TRIGGER_STATUS_32_XPE0r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE0_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE0_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_BST_TRIGGER_STATUS_32_XPE1r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE1_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE1_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_BST_TRIGGER_STATUS_32_XPE2r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE2_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE2_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_BST_TRIGGER_STATUS_32_XPE3r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE3_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_32_MMU_XPE3_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_BST_TRIGGER_STATUS_TYPE_XPE0r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE0_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE0_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_BST_TRIGGER_STATUS_TYPE_XPE1r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE1_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE1_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_BST_TRIGGER_STATUS_TYPE_XPE2r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE2_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE2_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_BST_TRIGGER_STATUS_TYPE_XPE3r :
        if (in_inst == 0) {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE3_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_BST_TRIGGER_STATUS_TYPE_MMU_XPE3_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_BUFFER_CELL_LIMIT_PUBLIC_POOL_XPE0r :
        sid_out = THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr;
        inst_out = 0;
        break;
    case THDI_BUFFER_CELL_LIMIT_PUBLIC_POOL_XPE1r :
        sid_out = THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr;
        inst_out = 1;
        break;
    case THDI_BUFFER_CELL_LIMIT_PUBLIC_POOL_XPE2r :
        sid_out = THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr;
        inst_out = 2;
        break;
    case THDI_BUFFER_CELL_LIMIT_PUBLIC_POOL_XPE3r :
        sid_out = THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr;
        inst_out = 3;
        break;
    case THDI_BUFFER_CELL_LIMIT_SP_XPE0r :
        sid_out = THDI_BUFFER_CELL_LIMIT_SPr;
        inst_out = 0;
        break;
    case THDI_BUFFER_CELL_LIMIT_SP_XPE1r :
        sid_out = THDI_BUFFER_CELL_LIMIT_SPr;
        inst_out = 1;
        break;
    case THDI_BUFFER_CELL_LIMIT_SP_XPE2r :
        sid_out = THDI_BUFFER_CELL_LIMIT_SPr;
        inst_out = 2;
        break;
    case THDI_BUFFER_CELL_LIMIT_SP_XPE3r :
        sid_out = THDI_BUFFER_CELL_LIMIT_SPr;
        inst_out = 3;
        break;
    case THDI_CELL_RESET_LIMIT_OFFSET_SP_XPE0r :
        sid_out = THDI_CELL_RESET_LIMIT_OFFSET_SPr;
        inst_out = 0;
        break;
    case THDI_CELL_RESET_LIMIT_OFFSET_SP_XPE1r :
        sid_out = THDI_CELL_RESET_LIMIT_OFFSET_SPr;
        inst_out = 1;
        break;
    case THDI_CELL_RESET_LIMIT_OFFSET_SP_XPE2r :
        sid_out = THDI_CELL_RESET_LIMIT_OFFSET_SPr;
        inst_out = 2;
        break;
    case THDI_CELL_RESET_LIMIT_OFFSET_SP_XPE3r :
        sid_out = THDI_CELL_RESET_LIMIT_OFFSET_SPr;
        inst_out = 3;
        break;
    case THDI_CELL_SPAP_RED_OFFSET_SP_XPE0r :
        sid_out = THDI_CELL_SPAP_RED_OFFSET_SPr;
        inst_out = 0;
        break;
    case THDI_CELL_SPAP_RED_OFFSET_SP_XPE1r :
        sid_out = THDI_CELL_SPAP_RED_OFFSET_SPr;
        inst_out = 1;
        break;
    case THDI_CELL_SPAP_RED_OFFSET_SP_XPE2r :
        sid_out = THDI_CELL_SPAP_RED_OFFSET_SPr;
        inst_out = 2;
        break;
    case THDI_CELL_SPAP_RED_OFFSET_SP_XPE3r :
        sid_out = THDI_CELL_SPAP_RED_OFFSET_SPr;
        inst_out = 3;
        break;
    case THDI_CELL_SPAP_YELLOW_OFFSET_SP_XPE0r :
        sid_out = THDI_CELL_SPAP_YELLOW_OFFSET_SPr;
        inst_out = 0;
        break;
    case THDI_CELL_SPAP_YELLOW_OFFSET_SP_XPE1r :
        sid_out = THDI_CELL_SPAP_YELLOW_OFFSET_SPr;
        inst_out = 1;
        break;
    case THDI_CELL_SPAP_YELLOW_OFFSET_SP_XPE2r :
        sid_out = THDI_CELL_SPAP_YELLOW_OFFSET_SPr;
        inst_out = 2;
        break;
    case THDI_CELL_SPAP_YELLOW_OFFSET_SP_XPE3r :
        sid_out = THDI_CELL_SPAP_YELLOW_OFFSET_SPr;
        inst_out = 3;
        break;
    case THDI_FLOW_CONTROL_XOFF_STATE_XPE0r :
        sid_out = THDI_FLOW_CONTROL_XOFF_STATEr;
        inst_out = 0;
        break;
    case THDI_FLOW_CONTROL_XOFF_STATE_XPE1r :
        sid_out = THDI_FLOW_CONTROL_XOFF_STATEr;
        inst_out = 1;
        break;
    case THDI_FLOW_CONTROL_XOFF_STATE_XPE2r :
        sid_out = THDI_FLOW_CONTROL_XOFF_STATEr;
        inst_out = 2;
        break;
    case THDI_FLOW_CONTROL_XOFF_STATE_XPE3r :
        sid_out = THDI_FLOW_CONTROL_XOFF_STATEr;
        inst_out = 3;
        break;
    case THDI_GLOBAL_HDRM_COUNT_XPE0r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE0_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE0_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_GLOBAL_HDRM_COUNT_XPE1r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE1_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE1_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_GLOBAL_HDRM_COUNT_XPE2r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE2_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE2_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_GLOBAL_HDRM_COUNT_XPE3r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE3_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_COUNT_MMU_XPE3_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_GLOBAL_HDRM_RESERVED_XPE0r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE0_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE0_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_GLOBAL_HDRM_RESERVED_XPE1r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE1_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE1_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_GLOBAL_HDRM_RESERVED_XPE2r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE2_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE2_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_GLOBAL_HDRM_RESERVED_XPE3r :
        if (in_inst == 0) {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE3_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_GLOBAL_HDRM_RESERVED_MMU_XPE3_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_HDRM_BUFFER_CELL_LIMIT_HP_XPE0r :
        sid_out = THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
        inst_out = 0;
        break;
    case THDI_HDRM_BUFFER_CELL_LIMIT_HP_XPE1r :
        sid_out = THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
        inst_out = 1;
        break;
    case THDI_HDRM_BUFFER_CELL_LIMIT_HP_XPE2r :
        sid_out = THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
        inst_out = 2;
        break;
    case THDI_HDRM_BUFFER_CELL_LIMIT_HP_XPE3r :
        sid_out = THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
        inst_out = 3;
        break;
    case THDI_MEM_INIT_STATUS_XPE0r :
        if (in_inst == 0) {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE0_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE0_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_MEM_INIT_STATUS_XPE1r :
        if (in_inst == 0) {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE1_IPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE1_IPIPE3r;
            inst_out = 0;
        }
        break;
    case THDI_MEM_INIT_STATUS_XPE2r :
        if (in_inst == 0) {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE2_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE2_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_MEM_INIT_STATUS_XPE3r :
        if (in_inst == 0) {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE3_IPIPE1r;
            inst_out = 0;
        } else {
            sid_out = THDI_MEM_INIT_STATUS_MMU_XPE3_IPIPE2r;
            inst_out = 0;
        }
        break;
    case THDI_PORT_LIMIT_STATES_XPE0r :
        sid_out = THDI_PORT_LIMIT_STATESr;
        inst_out = 0;
        break;
    case THDI_PORT_LIMIT_STATES_XPE1r :
        sid_out = THDI_PORT_LIMIT_STATESr;
        inst_out = 1;
        break;
    case THDI_PORT_LIMIT_STATES_XPE2r :
        sid_out = THDI_PORT_LIMIT_STATESr;
        inst_out = 2;
        break;
    case THDI_PORT_LIMIT_STATES_XPE3r :
        sid_out = THDI_PORT_LIMIT_STATESr;
        inst_out = 3;
        break;
    case THDU_CNG_STATE_RESET_XPE0r :
        if (in_inst == 0) {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case THDU_CNG_STATE_RESET_XPE1r :
        if (in_inst == 0) {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case THDU_CNG_STATE_RESET_XPE2r :
        if (in_inst == 0) {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case THDU_CNG_STATE_RESET_XPE3r :
        if (in_inst == 0) {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = THDU_CNG_STATE_RESET_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case THDU_OUTPUT_PORT_RX_ENABLE_64_XPE0r :
        if (in_inst == 0) {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE0_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE0_EPIPE1r;
            inst_out = 0;
        }
        break;
    case THDU_OUTPUT_PORT_RX_ENABLE_64_XPE1r :
        if (in_inst == 0) {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE1_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE1_EPIPE3r;
            inst_out = 0;
        }
        break;
    case THDU_OUTPUT_PORT_RX_ENABLE_64_XPE2r :
        if (in_inst == 0) {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE2_EPIPE0r;
            inst_out = 0;
        } else {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE2_EPIPE1r;
            inst_out = 0;
        }
        break;
    case THDU_OUTPUT_PORT_RX_ENABLE_64_XPE3r :
        if (in_inst == 0) {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE3_EPIPE2r;
            inst_out = 0;
        } else {
            sid_out = THDU_OUTPUT_PORT_RX_ENABLE_64_MMU_XPE3_EPIPE3r;
            inst_out = 0;
        }
        break;
    case WRED_POOL_INST_CONG_LIMIT_0_XPE0r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_0r;
        inst_out = 0;
        break;
    case WRED_POOL_INST_CONG_LIMIT_0_XPE1r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_0r;
        inst_out = 1;
        break;
    case WRED_POOL_INST_CONG_LIMIT_0_XPE2r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_0r;
        inst_out = 2;
        break;
    case WRED_POOL_INST_CONG_LIMIT_0_XPE3r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_0r;
        inst_out = 3;
        break;
    case WRED_POOL_INST_CONG_LIMIT_1_XPE0r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_1r;
        inst_out = 0;
        break;
    case WRED_POOL_INST_CONG_LIMIT_1_XPE1r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_1r;
        inst_out = 1;
        break;
    case WRED_POOL_INST_CONG_LIMIT_1_XPE2r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_1r;
        inst_out = 2;
        break;
    case WRED_POOL_INST_CONG_LIMIT_1_XPE3r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_1r;
        inst_out = 3;
        break;
    case WRED_POOL_INST_CONG_LIMIT_2_XPE0r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_2r;
        inst_out = 0;
        break;
    case WRED_POOL_INST_CONG_LIMIT_2_XPE1r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_2r;
        inst_out = 1;
        break;
    case WRED_POOL_INST_CONG_LIMIT_2_XPE2r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_2r;
        inst_out = 2;
        break;
    case WRED_POOL_INST_CONG_LIMIT_2_XPE3r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_2r;
        inst_out = 3;
        break;
    case WRED_POOL_INST_CONG_LIMIT_3_XPE0r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_3r;
        inst_out = 0;
        break;
    case WRED_POOL_INST_CONG_LIMIT_3_XPE1r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_3r;
        inst_out = 1;
        break;
    case WRED_POOL_INST_CONG_LIMIT_3_XPE2r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_3r;
        inst_out = 2;
        break;
    case WRED_POOL_INST_CONG_LIMIT_3_XPE3r :
        sid_out = WRED_POOL_INST_CONG_LIMIT_3r;
        inst_out = 3;




        break;
    case MMU_CCP_RESEQ_MEM_XPE0_PIPE0m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CCP_RESEQ_MEM_XPE0_PIPE1m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CCP_RESEQ_MEM_XPE1_PIPE2m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CCP_RESEQ_MEM_XPE1_PIPE3m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_CCP_RESEQ_MEM_XPE2_PIPE0m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CCP_RESEQ_MEM_XPE2_PIPE1m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CCP_RESEQ_MEM_XPE3_PIPE2m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CCP_RESEQ_MEM_XPE3_PIPE3m :
        sid_out = MMU_CCP_RESEQ_MEM_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE0_PIPE0m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE0_PIPE1m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE1_PIPE2m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE1_PIPE3m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE2_PIPE0m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE2_PIPE1m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE3_PIPE2m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CTR_MC_DROP_MEM_XPE3_PIPE3m :
        sid_out = MMU_CTR_MC_DROP_MEM_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE0_PIPE0m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE0_PIPE1m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE1_PIPE2m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE1_PIPE3m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE2_PIPE0m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE2_PIPE1m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE3_PIPE2m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CTR_UC_DROP_MEM_XPE3_PIPE3m :
        sid_out = MMU_CTR_UC_DROP_MEM_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE0_PIPE0m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE0_PIPE1m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE1_PIPE2m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE1_PIPE3m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE2_PIPE0m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE2_PIPE1m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE3_PIPE2m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_CTR_WRED_DROP_MEM_XPE3_PIPE3m :
        sid_out = MMU_CTR_WRED_DROP_MEM_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_ENQS_PBI_DB_SC0_PIPE0m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC0_IPIPE0m;
        inst_out = 0;
        break;
    case MMU_ENQS_PBI_DB_SC0_PIPE1m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC0_IPIPE1m;
        inst_out = 1;
        break;
    case MMU_ENQS_PBI_DB_SC0_PIPE2m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC0_IPIPE2m;
        inst_out = 2;
        break;
    case MMU_ENQS_PBI_DB_SC0_PIPE3m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC0_IPIPE3m;
        inst_out = 3;
        break;
    case MMU_ENQS_PBI_DB_SC1_PIPE0m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC1_IPIPE0m;
        inst_out = 0;
        break;
    case MMU_ENQS_PBI_DB_SC1_PIPE1m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC1_IPIPE1m;
        inst_out = 1;
        break;
    case MMU_ENQS_PBI_DB_SC1_PIPE2m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC1_IPIPE2m;
        inst_out = 2;
        break;
    case MMU_ENQS_PBI_DB_SC1_PIPE3m :
        sid_out = MMU_ENQS_PBI_DB_MMU_SC1_IPIPE3m;
        inst_out = 3;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE0_PIPE0m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE0_PIPE3m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE1_PIPE0m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE1_PIPE3m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE2_PIPE1m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE2_PIPE2m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE3_PIPE1m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_HI_XPE3_PIPE2m :
        sid_out = MMU_ENQX_PIPEMEM_HI_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE0_PIPE0m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE0_PIPE3m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE1_PIPE0m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE1_PIPE3m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE2_PIPE1m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE2_PIPE2m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE3_PIPE1m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case MMU_ENQX_PIPEMEM_LO_XPE3_PIPE2m :
        sid_out = MMU_ENQX_PIPEMEM_LO_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case MMU_EPRG_MEM_XPE0m :
        sid_out = MMU_EPRG_MEMm;
        inst_out = 0;
        break;
    case MMU_EPRG_MEM_XPE1m :
        sid_out = MMU_EPRG_MEMm;
        inst_out = 1;
        break;
    case MMU_EPRG_MEM_XPE2m :
        sid_out = MMU_EPRG_MEMm;
        inst_out = 2;
        break;
    case MMU_EPRG_MEM_XPE3m :
        sid_out = MMU_EPRG_MEMm;
        inst_out = 3;
        break;
    case MMU_PQE0_MEM_XPE0m :
        sid_out = MMU_PQE0_MEMm;
        inst_out = 0;
        break;
    case MMU_PQE0_MEM_XPE1m :
        sid_out = MMU_PQE0_MEMm;
        inst_out = 1;
        break;
    case MMU_PQE0_MEM_XPE2m :
        sid_out = MMU_PQE0_MEMm;
        inst_out = 2;
        break;
    case MMU_PQE0_MEM_XPE3m :
        sid_out = MMU_PQE0_MEMm;
        inst_out = 3;
        break;
    case MMU_PQE1_MEM_XPE0m :
        sid_out = MMU_PQE1_MEMm;
        inst_out = 0;
        break;
    case MMU_PQE1_MEM_XPE1m :
        sid_out = MMU_PQE1_MEMm;
        inst_out = 1;
        break;
    case MMU_PQE1_MEM_XPE2m :
        sid_out = MMU_PQE1_MEMm;
        inst_out = 2;
        break;
    case MMU_PQE1_MEM_XPE3m :
        sid_out = MMU_PQE1_MEMm;
        inst_out = 3;
        break;
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m:
        sid_out = MMU_REPL_GROUP_INITIAL_COPY_COUNTm;
        inst_out = 0;
        break;
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m:
        sid_out = MMU_REPL_GROUP_INITIAL_COPY_COUNTm;
        inst_out = 1;
        break;
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC0m:
        sid_out = MMU_REPL_GROUP_INITIAL_COPY_COUNT0m;
        inst_out = 0;
        break;
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC1m:
        sid_out = MMU_REPL_GROUP_INITIAL_COPY_COUNT0m;
        inst_out = 1;
        break;
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC0m:
        sid_out = MMU_REPL_GROUP_INITIAL_COPY_COUNT1m;
        inst_out = 0;
        break;
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC1m:
        sid_out = MMU_REPL_GROUP_INITIAL_COPY_COUNT1m;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE0_PIPE0m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE0_PIPE1m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE1_PIPE2m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE1_PIPE3m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE2_PIPE0m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE2_PIPE1m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE3_PIPE2m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_BST_XPE3_PIPE3m :
        sid_out = MMU_THDM_DB_PORTSP_BST_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_PIPE0m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIGm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_PIPE1m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIGm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_PIPE2m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIGm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_PIPE3m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIGm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE0m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Am;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE1m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Am;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE2m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Am;
        inst_out = 2;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE3m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Am;
        inst_out = 3;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE0m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Bm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE1m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Bm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE2m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Bm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE3m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Bm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE0m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Cm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE1m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Cm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE2m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Cm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE3m :
        sid_out = MMU_THDM_DB_PORTSP_CONFIG_Cm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE0_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE0_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE1_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE1_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE2_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE2_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE3_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_BST_XPE3_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_BST_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIGm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIGm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIGm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIGm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Am;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Am;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Am;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Am;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Bm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Bm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Bm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Bm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Cm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Cm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Cm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_CONFIG_Cm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE0_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE0_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE1_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE1_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE2_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE2_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE3_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_COUNT_XPE3_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_COUNT_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSETm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSETm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSETm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSETm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Am;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Am;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Am;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Am;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Bm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Bm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Bm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Bm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Cm;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Cm;
        inst_out = 1;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Cm;
        inst_out = 2;
        break;
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_OFFSET_Cm;
        inst_out = 3;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE0_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE0_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE1_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE1_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE2_PIPE0m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE2_PIPE1m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE3_PIPE2m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_DB_QUEUE_RESUME_XPE3_PIPE3m :
        sid_out = MMU_THDM_DB_QUEUE_RESUME_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE0_PIPE0m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE0_PIPE1m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE1_PIPE2m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE1_PIPE3m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE2_PIPE0m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE2_PIPE1m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE3_PIPE2m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_BST_XPE3_PIPE3m :
        sid_out = MMU_THDM_MCQE_PORTSP_BST_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_PIPE0m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIGm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_PIPE1m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIGm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_PIPE2m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIGm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_PIPE3m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIGm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE0m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Am;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE1m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Am;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE2m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Am;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE3m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Am;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE0m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Bm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE1m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Bm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE2m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Bm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE3m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Bm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE0m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Cm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE1m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Cm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE2m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Cm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE3m :
        sid_out = MMU_THDM_MCQE_PORTSP_CONFIG_Cm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE0_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE0_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE1_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE1_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE2_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE2_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE3_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_BST_XPE3_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_BST_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIGm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIGm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIGm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIGm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Am;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Am;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Am;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Am;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Bm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Bm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Bm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Bm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Cm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Cm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Cm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_CONFIG_Cm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE0_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE0_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE1_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE1_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE2_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE2_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE3_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_COUNT_XPE3_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_COUNT_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSETm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSETm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSETm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSETm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Am;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Am;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Am;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Am;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Bm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Bm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Bm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Bm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Cm;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Cm;
        inst_out = 1;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Cm;
        inst_out = 2;
        break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_OFFSET_Cm;
        inst_out = 3;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE0_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE0_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE1_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE1_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE2_PIPE0m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE2_PIPE1m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE3_PIPE2m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDM_MCQE_QUEUE_RESUME_XPE3_PIPE3m :
        sid_out = MMU_THDM_MCQE_QUEUE_RESUME_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE0_PIPE0m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE0_PIPE1m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE1_PIPE2m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE1_PIPE3m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE2_PIPE0m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE2_PIPE1m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE3_PIPE2m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_PORT_XPE3_PIPE3m :
        sid_out = MMU_THDU_BST_PORT_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE0_PIPE0m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE0_PIPE1m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE1_PIPE2m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE1_PIPE3m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE2_PIPE0m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE2_PIPE1m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE3_PIPE2m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QGROUP_XPE3_PIPE3m :
        sid_out = MMU_THDU_BST_QGROUP_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE0_PIPE0m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE0_PIPE1m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE1_PIPE2m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE1_PIPE3m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE2_PIPE0m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE2_PIPE1m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE3_PIPE2m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_BST_QUEUE_XPE3_PIPE3m :
        sid_out = MMU_THDU_BST_QUEUE_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_PORT_PIPE0m :
        sid_out = MMU_THDU_CONFIG_PORTm;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_PORT_PIPE1m :
        sid_out = MMU_THDU_CONFIG_PORTm;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_PORT_PIPE2m :
        sid_out = MMU_THDU_CONFIG_PORTm;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_PORT_PIPE3m :
        sid_out = MMU_THDU_CONFIG_PORTm;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_PORT0_PIPE0m :
        sid_out = MMU_THDU_CONFIG_PORT0m;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_PORT0_PIPE1m :
        sid_out = MMU_THDU_CONFIG_PORT0m;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_PORT0_PIPE2m :
        sid_out = MMU_THDU_CONFIG_PORT0m;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_PORT0_PIPE3m :
        sid_out = MMU_THDU_CONFIG_PORT0m;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_PORT1_PIPE0m :
        sid_out = MMU_THDU_CONFIG_PORT1m;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_PORT1_PIPE1m :
        sid_out = MMU_THDU_CONFIG_PORT1m;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_PORT1_PIPE2m :
        sid_out = MMU_THDU_CONFIG_PORT1m;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_PORT1_PIPE3m :
        sid_out = MMU_THDU_CONFIG_PORT1m;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_QGROUP_PIPE0m :
        sid_out = MMU_THDU_CONFIG_QGROUPm;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_QGROUP_PIPE1m :
        sid_out = MMU_THDU_CONFIG_QGROUPm;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_QGROUP_PIPE2m :
        sid_out = MMU_THDU_CONFIG_QGROUPm;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_QGROUP_PIPE3m :
        sid_out = MMU_THDU_CONFIG_QGROUPm;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE0m :
        sid_out = MMU_THDU_CONFIG_QGROUP0m;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE1m :
        sid_out = MMU_THDU_CONFIG_QGROUP0m;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE2m :
        sid_out = MMU_THDU_CONFIG_QGROUP0m;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE3m :
        sid_out = MMU_THDU_CONFIG_QGROUP0m;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_QGROUP1_PIPE0m :
        sid_out = MMU_THDU_CONFIG_QGROUP1m;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_QGROUP1_PIPE1m :
        sid_out = MMU_THDU_CONFIG_QGROUP1m;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_QGROUP1_PIPE2m :
        sid_out = MMU_THDU_CONFIG_QGROUP1m;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_QGROUP1_PIPE3m :
        sid_out = MMU_THDU_CONFIG_QGROUP1m;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_QUEUE_PIPE0m :
        sid_out = MMU_THDU_CONFIG_QUEUEm;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_QUEUE_PIPE1m :
        sid_out = MMU_THDU_CONFIG_QUEUEm;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_QUEUE_PIPE2m :
        sid_out = MMU_THDU_CONFIG_QUEUEm;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_QUEUE_PIPE3m :
        sid_out = MMU_THDU_CONFIG_QUEUEm;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE0m :
        sid_out = MMU_THDU_CONFIG_QUEUE0m;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE1m :
        sid_out = MMU_THDU_CONFIG_QUEUE0m;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE2m :
        sid_out = MMU_THDU_CONFIG_QUEUE0m;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE3m :
        sid_out = MMU_THDU_CONFIG_QUEUE0m;
        inst_out = 3;
        break;
    case MMU_THDU_CONFIG_QUEUE1_PIPE0m :
        sid_out = MMU_THDU_CONFIG_QUEUE1m;
        inst_out = 0;
        break;
    case MMU_THDU_CONFIG_QUEUE1_PIPE1m :
        sid_out = MMU_THDU_CONFIG_QUEUE1m;
        inst_out = 1;
        break;
    case MMU_THDU_CONFIG_QUEUE1_PIPE2m :
        sid_out = MMU_THDU_CONFIG_QUEUE1m;
        inst_out = 2;
        break;
    case MMU_THDU_CONFIG_QUEUE1_PIPE3m :
        sid_out = MMU_THDU_CONFIG_QUEUE1m;
        inst_out = 3;
        break;
    case MMU_THDU_COUNTER_PORT_XPE0_PIPE0m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_PORT_XPE0_PIPE1m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_PORT_XPE1_PIPE2m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_PORT_XPE1_PIPE3m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_PORT_XPE2_PIPE0m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_PORT_XPE2_PIPE1m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_PORT_XPE3_PIPE2m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_PORT_XPE3_PIPE3m :
        sid_out = MMU_THDU_COUNTER_PORT_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE0_PIPE0m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE0_PIPE1m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE1_PIPE2m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE1_PIPE3m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE2_PIPE0m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE2_PIPE1m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE3_PIPE2m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QGROUP_XPE3_PIPE3m :
        sid_out = MMU_THDU_COUNTER_QGROUP_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE0_PIPE0m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE0_PIPE1m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE1_PIPE2m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE1_PIPE3m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE2_PIPE0m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE2_PIPE1m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE3_PIPE2m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_COUNTER_QUEUE_XPE3_PIPE3m :
        sid_out = MMU_THDU_COUNTER_QUEUE_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_OFFSET_QGROUP_PIPE0m :
        sid_out = MMU_THDU_OFFSET_QGROUPm;
        inst_out = 0;
        break;
    case MMU_THDU_OFFSET_QGROUP_PIPE1m :
        sid_out = MMU_THDU_OFFSET_QGROUPm;
        inst_out = 1;
        break;
    case MMU_THDU_OFFSET_QGROUP_PIPE2m :
        sid_out = MMU_THDU_OFFSET_QGROUPm;
        inst_out = 2;
        break;
    case MMU_THDU_OFFSET_QGROUP_PIPE3m :
        sid_out = MMU_THDU_OFFSET_QGROUPm;
        inst_out = 3;
        break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE0m :
        sid_out = MMU_THDU_OFFSET_QGROUP0m;
        inst_out = 0;
        break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE1m :
        sid_out = MMU_THDU_OFFSET_QGROUP0m;
        inst_out = 1;
        break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE2m :
        sid_out = MMU_THDU_OFFSET_QGROUP0m;
        inst_out = 2;
        break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE3m :
        sid_out = MMU_THDU_OFFSET_QGROUP0m;
        inst_out = 3;
        break;
    case MMU_THDU_OFFSET_QGROUP1_PIPE0m :
        sid_out = MMU_THDU_OFFSET_QGROUP1m;
        inst_out = 0;
        break;
    case MMU_THDU_OFFSET_QGROUP1_PIPE1m :
        sid_out = MMU_THDU_OFFSET_QGROUP1m;
        inst_out = 1;
        break;
    case MMU_THDU_OFFSET_QGROUP1_PIPE2m :
        sid_out = MMU_THDU_OFFSET_QGROUP1m;
        inst_out = 2;
        break;
    case MMU_THDU_OFFSET_QGROUP1_PIPE3m :
        sid_out = MMU_THDU_OFFSET_QGROUP1m;
        inst_out = 3;
        break;
    case MMU_THDU_OFFSET_QUEUE_PIPE0m :
        sid_out = MMU_THDU_OFFSET_QUEUEm;
        inst_out = 0;
        break;
    case MMU_THDU_OFFSET_QUEUE_PIPE1m :
        sid_out = MMU_THDU_OFFSET_QUEUEm;
        inst_out = 1;
        break;
    case MMU_THDU_OFFSET_QUEUE_PIPE2m :
        sid_out = MMU_THDU_OFFSET_QUEUEm;
        inst_out = 2;
        break;
    case MMU_THDU_OFFSET_QUEUE_PIPE3m :
        sid_out = MMU_THDU_OFFSET_QUEUEm;
        inst_out = 3;
        break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE0m :
        sid_out = MMU_THDU_OFFSET_QUEUE0m;
        inst_out = 0;
        break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE1m :
        sid_out = MMU_THDU_OFFSET_QUEUE0m;
        inst_out = 1;
        break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE2m :
        sid_out = MMU_THDU_OFFSET_QUEUE0m;
        inst_out = 2;
        break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE3m :
        sid_out = MMU_THDU_OFFSET_QUEUE0m;
        inst_out = 3;
        break;
    case MMU_THDU_OFFSET_QUEUE1_PIPE0m :
        sid_out = MMU_THDU_OFFSET_QUEUE1m;
        inst_out = 0;
        break;
    case MMU_THDU_OFFSET_QUEUE1_PIPE1m :
        sid_out = MMU_THDU_OFFSET_QUEUE1m;
        inst_out = 1;
        break;
    case MMU_THDU_OFFSET_QUEUE1_PIPE2m :
        sid_out = MMU_THDU_OFFSET_QUEUE1m;
        inst_out = 2;
        break;
    case MMU_THDU_OFFSET_QUEUE1_PIPE3m :
        sid_out = MMU_THDU_OFFSET_QUEUE1m;
        inst_out = 3;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP_PIPE0m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAPm;
        inst_out = 0;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP_PIPE1m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAPm;
        inst_out = 1;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP_PIPE2m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAPm;
        inst_out = 2;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP_PIPE3m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAPm;
        inst_out = 3;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE0m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP0m;
        inst_out = 0;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE1m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP0m;
        inst_out = 1;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE2m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP0m;
        inst_out = 2;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE3m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP0m;
        inst_out = 3;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE0m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP1m;
        inst_out = 0;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE1m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP1m;
        inst_out = 1;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE2m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP1m;
        inst_out = 2;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE3m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP1m;
        inst_out = 3;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE0m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP2m;
        inst_out = 0;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE1m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP2m;
        inst_out = 1;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE2m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP2m;
        inst_out = 2;
        break;
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE3m :
        sid_out = MMU_THDU_Q_TO_QGRP_MAP2m;
        inst_out = 3;
        break;
    case MMU_THDU_RESUME_PORT_PIPE0m :
        sid_out = MMU_THDU_RESUME_PORTm;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_PORT_PIPE1m :
        sid_out = MMU_THDU_RESUME_PORTm;
        inst_out = 1;
        break;
    case MMU_THDU_RESUME_PORT_PIPE2m :
        sid_out = MMU_THDU_RESUME_PORTm;
        inst_out = 2;
        break;
    case MMU_THDU_RESUME_PORT_PIPE3m :
        sid_out = MMU_THDU_RESUME_PORTm;
        inst_out = 3;
        break;
    case MMU_THDU_RESUME_PORT0_PIPE0m :
        sid_out = MMU_THDU_RESUME_PORT0m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_PORT0_PIPE1m :
        sid_out = MMU_THDU_RESUME_PORT0m;
        inst_out = 1;
        break;
    case MMU_THDU_RESUME_PORT0_PIPE2m :
        sid_out = MMU_THDU_RESUME_PORT0m;
        inst_out = 2;
        break;
    case MMU_THDU_RESUME_PORT0_PIPE3m :
        sid_out = MMU_THDU_RESUME_PORT0m;
        inst_out = 3;
        break;
    case MMU_THDU_RESUME_PORT1_PIPE0m :
        sid_out = MMU_THDU_RESUME_PORT1m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_PORT1_PIPE1m :
        sid_out = MMU_THDU_RESUME_PORT1m;
        inst_out = 1;
        break;
    case MMU_THDU_RESUME_PORT1_PIPE2m :
        sid_out = MMU_THDU_RESUME_PORT1m;
        inst_out = 2;
        break;
    case MMU_THDU_RESUME_PORT1_PIPE3m :
        sid_out = MMU_THDU_RESUME_PORT1m;
        inst_out = 3;
        break;
    case MMU_THDU_RESUME_PORT2_PIPE0m :
        sid_out = MMU_THDU_RESUME_PORT2m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_PORT2_PIPE1m :
        sid_out = MMU_THDU_RESUME_PORT2m;
        inst_out = 1;
        break;
    case MMU_THDU_RESUME_PORT2_PIPE2m :
        sid_out = MMU_THDU_RESUME_PORT2m;
        inst_out = 2;
        break;
    case MMU_THDU_RESUME_PORT2_PIPE3m :
        sid_out = MMU_THDU_RESUME_PORT2m;
        inst_out = 3;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE0_PIPE0m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE0_PIPE1m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE1_PIPE2m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE1_PIPE3m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE2_PIPE0m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE2_PIPE1m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE3_PIPE2m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QGROUP_XPE3_PIPE3m :
        sid_out = MMU_THDU_RESUME_QGROUP_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE0_PIPE0m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE0_PIPE1m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE1_PIPE2m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE1_PIPE3m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE2_PIPE0m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE2_PIPE1m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE3_PIPE2m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_THDU_RESUME_QUEUE_XPE3_PIPE3m :
        sid_out = MMU_THDU_RESUME_QUEUE_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE0_PIPE0m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE0_PIPE1m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE1_PIPE2m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE1_PIPE3m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE2_PIPE0m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE2_PIPE1m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE3_PIPE2m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_AVG_QSIZE_XPE3_PIPE3m :
        sid_out = MMU_WRED_AVG_QSIZE_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE0_PIPE0m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE0_PIPE1m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE1_PIPE2m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE1_PIPE3m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE2_PIPE0m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE2_PIPE1m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE3_PIPE2m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_CONFIG_XPE3_PIPE3m :
        sid_out = MMU_WRED_CONFIG_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE0_PIPE0m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE0_PIPE1m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE1_PIPE2m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE1_PIPE3m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE2_PIPE0m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE2_PIPE1m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE3_PIPE2m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_XPE3_PIPE3m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE0_PIPE0m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE0_PIPE1m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE1_PIPE2m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE1_PIPE3m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE2_PIPE0m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE2_PIPE1m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE3_PIPE2m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_DROP_THD_MARK_XPE3_PIPE3m :
        sid_out = MMU_WRED_PORT_SP_DROP_THD_MARK_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE0m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE1m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE2m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE3m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE0m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE1m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE2m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE3m :
        sid_out = MMU_WRED_PORT_SP_SHARED_COUNT_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE0_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE0_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE1_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE1_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE2_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE2_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE3_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_0_XPE3_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_0_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE0_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE0_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE1_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE1_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE2_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE2_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE3_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_1_XPE3_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_1_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE0_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE0_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE1_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE1_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE2_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE2_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE3_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE3_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_DROP_THD_MARK_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE0_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE0_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE1_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE1_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE2_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE2_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE3_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE3_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE0_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE0_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE0_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE0_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE1_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE1_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE1_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE1_EPIPE3m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE2_PIPE0m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE2_EPIPE0m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE2_PIPE1m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE2_EPIPE1m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE3_PIPE2m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE3_EPIPE2m;
        inst_out = 0;
        break;
    case MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE3_PIPE3m :
        sid_out = MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_MMU_XPE3_EPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE0_PIPE0m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE0_PIPE3m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE1_PIPE0m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE1_PIPE3m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE2_PIPE1m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE2_PIPE2m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE3_PIPE1m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_BST_XPE3_PIPE2m :
        sid_out = THDI_PORT_PG_BST_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE0_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE0_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE1_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE1_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE2_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE2_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE3_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT1_XPE3_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_RT1_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE0_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE0_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE1_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE1_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE2_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE2_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE3_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_RT2_XPE3_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_RT2_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE0_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE0_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE1_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE1_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE2_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE2_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE3_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH1_XPE3_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_SH1_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE0_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE0_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE1_PIPE0m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE1_PIPE3m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE2_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE2_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE3_PIPE1m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CNTRS_SH2_XPE3_PIPE2m :
        sid_out = THDI_PORT_PG_CNTRS_SH2_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CONFIG_PIPE0m :
        sid_out = THDI_PORT_PG_CONFIGm;
        inst_out = 0;
        break;
    case THDI_PORT_PG_CONFIG_PIPE1m :
        sid_out = THDI_PORT_PG_CONFIGm;
        inst_out = 1;
        break;
    case THDI_PORT_PG_CONFIG_PIPE2m :
        sid_out = THDI_PORT_PG_CONFIGm;
        inst_out = 2;
        break;
    case THDI_PORT_PG_CONFIG_PIPE3m :
        sid_out = THDI_PORT_PG_CONFIGm;
        inst_out = 3;
        break;
    case THDI_PORT_SP_BST_XPE0_PIPE0m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_BST_XPE0_PIPE3m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_BST_XPE1_PIPE0m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_BST_XPE1_PIPE3m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_BST_XPE2_PIPE1m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_BST_XPE2_PIPE2m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_BST_XPE3_PIPE1m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_BST_XPE3_PIPE2m :
        sid_out = THDI_PORT_SP_BST_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE0_PIPE0m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE0_PIPE3m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE1_PIPE0m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE1_PIPE3m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE2_PIPE1m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE2_PIPE2m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE3_PIPE1m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_RT_XPE3_PIPE2m :
        sid_out = THDI_PORT_SP_CNTRS_RT_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE0_PIPE0m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE0_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE0_PIPE3m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE0_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE1_PIPE0m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE1_IPIPE0m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE1_PIPE3m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE1_IPIPE3m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE2_PIPE1m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE2_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE2_PIPE2m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE2_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE3_PIPE1m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE3_IPIPE1m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CNTRS_SH_XPE3_PIPE2m :
        sid_out = THDI_PORT_SP_CNTRS_SH_MMU_XPE3_IPIPE2m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CONFIG_PIPE0m :
        sid_out = THDI_PORT_SP_CONFIGm;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CONFIG_PIPE1m :
        sid_out = THDI_PORT_SP_CONFIGm;
        inst_out = 1;
        break;
    case THDI_PORT_SP_CONFIG_PIPE2m :
        sid_out = THDI_PORT_SP_CONFIGm;
        inst_out = 2;
        break;
    case THDI_PORT_SP_CONFIG_PIPE3m :
        sid_out = THDI_PORT_SP_CONFIGm;
        inst_out = 3;
        break;
    case THDI_PORT_SP_CONFIG0_PIPE0m :
        sid_out = THDI_PORT_SP_CONFIG0m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CONFIG0_PIPE1m :
        sid_out = THDI_PORT_SP_CONFIG0m;
        inst_out = 1;
        break;
    case THDI_PORT_SP_CONFIG0_PIPE2m :
        sid_out = THDI_PORT_SP_CONFIG0m;
        inst_out = 2;
        break;
    case THDI_PORT_SP_CONFIG0_PIPE3m :
        sid_out = THDI_PORT_SP_CONFIG0m;
        inst_out = 3;
        break;
    case THDI_PORT_SP_CONFIG1_PIPE0m :
        sid_out = THDI_PORT_SP_CONFIG1m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CONFIG1_PIPE1m :
        sid_out = THDI_PORT_SP_CONFIG1m;
        inst_out = 1;
        break;
    case THDI_PORT_SP_CONFIG1_PIPE2m :
        sid_out = THDI_PORT_SP_CONFIG1m;
        inst_out = 2;
        break;
    case THDI_PORT_SP_CONFIG1_PIPE3m :
        sid_out = THDI_PORT_SP_CONFIG1m;
        inst_out = 3;
        break;
    case THDI_PORT_SP_CONFIG2_PIPE0m :
        sid_out = THDI_PORT_SP_CONFIG2m;
        inst_out = 0;
        break;
    case THDI_PORT_SP_CONFIG2_PIPE1m :
        sid_out = THDI_PORT_SP_CONFIG2m;
        inst_out = 1;
        break;
    case THDI_PORT_SP_CONFIG2_PIPE2m :
        sid_out = THDI_PORT_SP_CONFIG2m;
        inst_out = 2;
        break;
    case THDI_PORT_SP_CONFIG2_PIPE3m :
        sid_out = THDI_PORT_SP_CONFIG2m;
        inst_out = 3;
        break;

    case MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE0m :
        sid_out = MMU_MTRO_EGRMETERINGBUCKET_MEMm;
        inst_out = 0;
        break;
    case MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE1m :
        sid_out = MMU_MTRO_EGRMETERINGBUCKET_MEMm;
        inst_out = 1;
        break;
    case MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE2m :
        sid_out = MMU_MTRO_EGRMETERINGBUCKET_MEMm;
        inst_out = 2;
        break;
    case MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE3m :
        sid_out = MMU_MTRO_EGRMETERINGBUCKET_MEMm;
        inst_out = 3;
        break;

    case MMU_MTRO_BUCKET_L0_MEM_PIPE0m :
        sid_out = MMU_MTRO_BUCKET_L0_MEMm;
        inst_out = 0;
        break;
    case MMU_MTRO_BUCKET_L0_MEM_PIPE1m :
        sid_out = MMU_MTRO_BUCKET_L0_MEMm;
        inst_out = 1;
        break;
    case MMU_MTRO_BUCKET_L0_MEM_PIPE2m :
        sid_out = MMU_MTRO_BUCKET_L0_MEMm;
        inst_out = 2;
        break;
    case MMU_MTRO_BUCKET_L0_MEM_PIPE3m :
        sid_out = MMU_MTRO_BUCKET_L0_MEMm;
        inst_out = 3;
        break;

    case MMU_MTRO_BUCKET_L1_MEM_PIPE0m :
        sid_out = MMU_MTRO_BUCKET_L1_MEMm;
        inst_out = 0;
        break;
    case MMU_MTRO_BUCKET_L1_MEM_PIPE1m :
        sid_out = MMU_MTRO_BUCKET_L1_MEMm;
        inst_out = 1;
        break;
    case MMU_MTRO_BUCKET_L1_MEM_PIPE2m :
        sid_out = MMU_MTRO_BUCKET_L1_MEMm;
        inst_out = 2;
        break;
    case MMU_MTRO_BUCKET_L1_MEM_PIPE3m :
        sid_out = MMU_MTRO_BUCKET_L1_MEMm;
        inst_out = 3;
        break;

    case Q_SCHED_L0_ACCUM_COMP_MEM_PIPE0m :
        sid_out = Q_SCHED_L0_ACCUM_COMP_MEMm;
        inst_out = 0;
        break;
    case Q_SCHED_L0_ACCUM_COMP_MEM_PIPE1m :
        sid_out = Q_SCHED_L0_ACCUM_COMP_MEMm;
        inst_out = 1;
        break;
    case Q_SCHED_L0_ACCUM_COMP_MEM_PIPE2m :
        sid_out = Q_SCHED_L0_ACCUM_COMP_MEMm;
        inst_out = 2;
        break;
    case Q_SCHED_L0_ACCUM_COMP_MEM_PIPE3m :
        sid_out = Q_SCHED_L0_ACCUM_COMP_MEMm;
        inst_out = 3;
        break;

    case Q_SCHED_L0_CREDIT_MEM_PIPE0m :
        sid_out = Q_SCHED_L0_CREDIT_MEMm;
        inst_out = 0;
        break;
    case Q_SCHED_L0_CREDIT_MEM_PIPE1m :
        sid_out = Q_SCHED_L0_CREDIT_MEMm;
        inst_out = 1;
        break;
    case Q_SCHED_L0_CREDIT_MEM_PIPE2m :
        sid_out = Q_SCHED_L0_CREDIT_MEMm;
        inst_out = 2;
        break;
    case Q_SCHED_L0_CREDIT_MEM_PIPE3m :
        sid_out = Q_SCHED_L0_CREDIT_MEMm;
        inst_out = 3;
        break;

    case Q_SCHED_L0_WEIGHT_MEM_PIPE0m :
        sid_out = Q_SCHED_L0_WEIGHT_MEMm;
        inst_out = 0;
        break;
    case Q_SCHED_L0_WEIGHT_MEM_PIPE1m :
        sid_out = Q_SCHED_L0_WEIGHT_MEMm;
        inst_out = 1;
        break;
    case Q_SCHED_L0_WEIGHT_MEM_PIPE2m :
        sid_out = Q_SCHED_L0_WEIGHT_MEMm;
        inst_out = 2;
        break;
    case Q_SCHED_L0_WEIGHT_MEM_PIPE3m :
        sid_out = Q_SCHED_L0_WEIGHT_MEMm;
        inst_out = 3;
        break;

    case Q_SCHED_L1_ACCUM_COMP_MEM_PIPE0m :
        sid_out = Q_SCHED_L1_ACCUM_COMP_MEMm;
        inst_out = 0;
        break;
    case Q_SCHED_L1_ACCUM_COMP_MEM_PIPE1m :
        sid_out = Q_SCHED_L1_ACCUM_COMP_MEMm;
        inst_out = 1;
        break;
    case Q_SCHED_L1_ACCUM_COMP_MEM_PIPE2m :
        sid_out = Q_SCHED_L1_ACCUM_COMP_MEMm;
        inst_out = 2;
        break;
    case Q_SCHED_L1_ACCUM_COMP_MEM_PIPE3m :
        sid_out = Q_SCHED_L1_ACCUM_COMP_MEMm;
        inst_out = 3;
        break;

    case Q_SCHED_L1_CREDIT_MEM_PIPE0m :
        sid_out = Q_SCHED_L1_CREDIT_MEMm;
        inst_out = 0;
        break;
    case Q_SCHED_L1_CREDIT_MEM_PIPE1m :
        sid_out = Q_SCHED_L1_CREDIT_MEMm;
        inst_out = 1;
        break;
    case Q_SCHED_L1_CREDIT_MEM_PIPE2m :
        sid_out = Q_SCHED_L1_CREDIT_MEMm;
        inst_out = 2;
        break;
    case Q_SCHED_L1_CREDIT_MEM_PIPE3m :
        sid_out = Q_SCHED_L1_CREDIT_MEMm;
        inst_out = 3;
        break;

    case Q_SCHED_L2_ACCUM_COMP_MEM_PIPE0m :
        sid_out = Q_SCHED_L2_ACCUM_COMP_MEMm;
        inst_out = 0;
        break;
    case Q_SCHED_L2_ACCUM_COMP_MEM_PIPE1m :
        sid_out = Q_SCHED_L2_ACCUM_COMP_MEMm;
        inst_out = 1;
        break;
    case Q_SCHED_L2_ACCUM_COMP_MEM_PIPE2m :
        sid_out = Q_SCHED_L2_ACCUM_COMP_MEMm;
        inst_out = 2;
        break;
    case Q_SCHED_L2_ACCUM_COMP_MEM_PIPE3m :
        sid_out = Q_SCHED_L2_ACCUM_COMP_MEMm;
        inst_out = 3;
        break;

    case Q_SCHED_L2_CREDIT_MEM_PIPE0m :
        sid_out = Q_SCHED_L2_CREDIT_MEMm;
        inst_out = 0;
        break;
    case Q_SCHED_L2_CREDIT_MEM_PIPE1m :
        sid_out = Q_SCHED_L2_CREDIT_MEMm;
        inst_out = 1;
        break;
    case Q_SCHED_L2_CREDIT_MEM_PIPE2m :
        sid_out = Q_SCHED_L2_CREDIT_MEMm;
        inst_out = 2;
        break;
    case Q_SCHED_L2_CREDIT_MEM_PIPE3m :
        sid_out = Q_SCHED_L2_CREDIT_MEMm;
        inst_out = 3;
        break;

    case MMU_CTR_ING_DROP_MEM_XPE0_PIPE0m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 0;
        break;
    case MMU_CTR_ING_DROP_MEM_XPE0_PIPE3m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 1;
        break;
    case MMU_CTR_ING_DROP_MEM_XPE1_PIPE0m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 2;
        break;
    case MMU_CTR_ING_DROP_MEM_XPE1_PIPE3m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 3;
        break;
    case MMU_CTR_ING_DROP_MEM_XPE2_PIPE1m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 4;
        break;
    case MMU_CTR_ING_DROP_MEM_XPE2_PIPE2m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 5;
        break;
    case MMU_CTR_ING_DROP_MEM_XPE3_PIPE1m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 6;
        break;
    case MMU_CTR_ING_DROP_MEM_XPE3_PIPE2m :
        sid_out = MMU_CTR_ING_DROP_MEMm;
        inst_out = 7;
        break;

    case MMU_CTR_COLOR_DROP_MEM_XPE0_PIPE0m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 0;
        break;
    case MMU_CTR_COLOR_DROP_MEM_XPE0_PIPE3m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 1;
        break;
    case MMU_CTR_COLOR_DROP_MEM_XPE1_PIPE0m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 2;
        break;
    case MMU_CTR_COLOR_DROP_MEM_XPE1_PIPE3m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 3;
        break;
    case MMU_CTR_COLOR_DROP_MEM_XPE2_PIPE1m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 4;
        break;
    case MMU_CTR_COLOR_DROP_MEM_XPE2_PIPE2m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 5;
        break;
    case MMU_CTR_COLOR_DROP_MEM_XPE3_PIPE1m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 6;
        break;
    case MMU_CTR_COLOR_DROP_MEM_XPE3_PIPE2m :
        sid_out = MMU_CTR_COLOR_DROP_MEMm;
        inst_out = 7;
        break;

    default: sid_out = in_sid; inst_out = 0; break;

    }

    *out_sid = sid_out;
    *instance = inst_out;

    return;
}

/*******************************************************************************
 * Public functions
 */

void
bcm56960_a0_bcmtm_pt_sid_rename(int unit, bcmdrd_sid_t in_sid, int in_inst,
                                bcmdrd_sid_t *out_sid, int *instance)
{
    int inst = *instance;
    bcmdrd_sid_t sid;

    switch (in_sid) {
        case IDB_OBM0_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM0_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM0_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM0_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM0_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM0_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM0_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM0_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM0_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM0_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM0_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM0_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM0_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM0_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM0_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM0_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM0_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM0_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM0_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM0_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM0_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM0_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM0_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM0_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM0_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM0_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM0_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM0_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM0_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM0_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM0_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM0_PRI_MAP_PORT3m; inst = 3; break;

        case IDB_OBM1_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM1_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM1_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM1_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM1_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM1_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM1_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM1_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM1_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM1_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM1_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM1_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM1_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM1_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM1_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM1_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM1_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM1_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM1_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM1_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM1_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM1_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM1_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM1_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM1_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM1_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM1_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM1_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM1_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM1_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM1_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM1_PRI_MAP_PORT3m; inst = 3; break;

        case IDB_OBM2_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM2_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM2_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM2_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM2_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM2_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM2_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM2_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM2_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM2_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM2_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM2_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM2_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM2_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM2_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM2_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM2_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM2_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM2_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM2_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM2_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM2_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM2_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM2_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM2_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM2_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM2_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM2_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM2_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM2_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM2_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM2_PRI_MAP_PORT3m; inst = 3; break;

        case IDB_OBM3_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM3_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM3_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM3_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM3_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM3_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM3_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM3_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM3_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM3_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM3_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM3_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM3_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM3_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM3_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM3_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM3_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM3_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM3_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM3_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM3_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM3_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM3_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM3_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM3_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM3_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM3_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM3_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM3_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM3_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM3_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM3_PRI_MAP_PORT3m; inst = 3; break;

        case IDB_OBM4_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM4_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM4_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM4_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM4_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM4_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM4_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM4_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM4_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM4_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM4_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM4_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM4_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM4_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM4_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM4_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM4_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM4_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM4_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM4_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM4_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM4_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM4_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM4_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM4_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM4_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM4_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM4_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM4_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM4_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM4_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM4_PRI_MAP_PORT3m; inst = 3; break;

        case IDB_OBM5_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM5_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM5_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM5_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM5_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM5_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM5_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM5_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM5_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM5_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM5_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM5_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM5_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM5_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM5_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM5_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM5_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM5_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM5_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM5_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM5_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM5_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM5_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM5_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM5_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM5_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM5_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM5_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM5_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM5_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM5_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM5_PRI_MAP_PORT3m; inst = 3; break;

        case IDB_OBM6_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM6_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM6_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM6_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM6_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM6_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM6_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM6_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM6_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM6_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM6_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM6_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM6_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM6_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM6_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM6_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM6_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM6_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM6_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM6_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM6_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM6_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM6_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM6_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM6_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM6_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM6_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM6_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM6_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM6_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM6_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM6_PRI_MAP_PORT3m; inst = 3; break;

        case IDB_OBM7_PRI_MAP_PORT0_PIPE0m:
                sid = IDB_OBM7_PRI_MAP_PORT0m; inst = 0; break;
        case IDB_OBM7_PRI_MAP_PORT1_PIPE0m:
                sid = IDB_OBM7_PRI_MAP_PORT1m; inst = 0; break;
        case IDB_OBM7_PRI_MAP_PORT2_PIPE0m:
                sid = IDB_OBM7_PRI_MAP_PORT2m; inst = 0; break;
        case IDB_OBM7_PRI_MAP_PORT3_PIPE0m:
                sid = IDB_OBM7_PRI_MAP_PORT3m; inst = 0; break;

        case IDB_OBM7_PRI_MAP_PORT0_PIPE1m:
                sid = IDB_OBM7_PRI_MAP_PORT0m; inst = 1; break;
        case IDB_OBM7_PRI_MAP_PORT1_PIPE1m:
                sid = IDB_OBM7_PRI_MAP_PORT1m; inst = 1; break;
        case IDB_OBM7_PRI_MAP_PORT2_PIPE1m:
                sid = IDB_OBM7_PRI_MAP_PORT2m; inst = 1; break;
        case IDB_OBM7_PRI_MAP_PORT3_PIPE1m:
                sid = IDB_OBM7_PRI_MAP_PORT3m; inst = 1; break;

        case IDB_OBM7_PRI_MAP_PORT0_PIPE2m:
                sid = IDB_OBM7_PRI_MAP_PORT0m; inst = 2; break;
        case IDB_OBM7_PRI_MAP_PORT1_PIPE2m:
                sid = IDB_OBM7_PRI_MAP_PORT1m; inst = 2; break;
        case IDB_OBM7_PRI_MAP_PORT2_PIPE2m:
                sid = IDB_OBM7_PRI_MAP_PORT2m; inst = 2; break;
        case IDB_OBM7_PRI_MAP_PORT3_PIPE2m:
                sid = IDB_OBM7_PRI_MAP_PORT3m; inst = 2; break;

        case IDB_OBM7_PRI_MAP_PORT0_PIPE3m:
                sid = IDB_OBM7_PRI_MAP_PORT0m; inst = 3; break;
        case IDB_OBM7_PRI_MAP_PORT1_PIPE3m:
                sid = IDB_OBM7_PRI_MAP_PORT1m; inst = 3; break;
        case IDB_OBM7_PRI_MAP_PORT2_PIPE3m:
                sid = IDB_OBM7_PRI_MAP_PORT2m; inst = 3; break;
        case IDB_OBM7_PRI_MAP_PORT3_PIPE3m:
                sid = IDB_OBM7_PRI_MAP_PORT3m; inst = 3; break;

        case IS_TDM_CALENDAR0_PIPE0m:
               sid = IS_TDM_CALENDAR0m; inst = 0; break;
        case IS_TDM_CALENDAR0_PIPE1m:
               sid = IS_TDM_CALENDAR0m; inst = 1; break;
        case IS_TDM_CALENDAR0_PIPE2m:
               sid = IS_TDM_CALENDAR0m; inst = 2; break;
        case IS_TDM_CALENDAR0_PIPE3m:
               sid = IS_TDM_CALENDAR0m; inst = 3; break;

        case IS_TDM_CALENDAR1_PIPE0m:
               sid = IS_TDM_CALENDAR1m; inst = 0; break;
        case IS_TDM_CALENDAR1_PIPE1m:
               sid = IS_TDM_CALENDAR1m; inst = 1; break;
        case IS_TDM_CALENDAR1_PIPE2m:
               sid = IS_TDM_CALENDAR1m; inst = 2; break;
        case IS_TDM_CALENDAR1_PIPE3m:
               sid = IS_TDM_CALENDAR1m; inst = 3; break;


        case TDM_CALENDAR1_PIPE0m:
               sid = TDM_CALENDAR1m; inst = 0; break;
        case TDM_CALENDAR1_PIPE1m:
               sid = TDM_CALENDAR1m; inst = 1; break;
        case TDM_CALENDAR1_PIPE2m:
               sid = TDM_CALENDAR1m; inst = 2; break;
        case TDM_CALENDAR1_PIPE3m:
               sid = TDM_CALENDAR1m; inst = 3; break;

        case TDM_CALENDAR0_PIPE0m:
               sid = TDM_CALENDAR0m; inst = 0; break;
        case TDM_CALENDAR0_PIPE1m:
               sid = TDM_CALENDAR0m; inst = 1; break;
        case TDM_CALENDAR0_PIPE2m:
               sid = TDM_CALENDAR0m; inst = 2; break;
        case TDM_CALENDAR0_PIPE3m:
               sid = TDM_CALENDAR0m; inst = 3; break;


        case EGR_XMIT_START_COUNT_PIPE0m:
                sid = EGR_XMIT_START_COUNTm; inst = 0; break;
        case EGR_XMIT_START_COUNT_PIPE1m:
                sid = EGR_XMIT_START_COUNTm; inst = 1; break;
        case EGR_XMIT_START_COUNT_PIPE2m:
                sid = EGR_XMIT_START_COUNTm; inst = 2; break;
        case EGR_XMIT_START_COUNT_PIPE3m:
                sid = EGR_XMIT_START_COUNTm; inst = 3; break;


        case IDB_OBM0_CONTROL_PIPE0r:   sid = IDB_OBM0_CONTROLr;   inst = 0; break;
        case IDB_OBM1_CONTROL_PIPE0r:   sid = IDB_OBM1_CONTROLr;   inst = 0; break;
        case IDB_OBM2_CONTROL_PIPE0r:   sid = IDB_OBM2_CONTROLr;   inst = 0; break;
        case IDB_OBM3_CONTROL_PIPE0r:   sid = IDB_OBM3_CONTROLr;   inst = 0; break;
        case IDB_OBM4_CONTROL_PIPE0r:   sid = IDB_OBM4_CONTROLr;   inst = 0; break;
        case IDB_OBM5_CONTROL_PIPE0r:   sid = IDB_OBM5_CONTROLr;   inst = 0; break;
        case IDB_OBM6_CONTROL_PIPE0r:   sid = IDB_OBM6_CONTROLr;   inst = 0; break;
        case IDB_OBM7_CONTROL_PIPE0r:   sid = IDB_OBM7_CONTROLr;   inst = 0; break;
        case IDB_OBM0_CONTROL_PIPE1r:   sid = IDB_OBM0_CONTROLr;   inst = 1; break;
        case IDB_OBM1_CONTROL_PIPE1r:   sid = IDB_OBM1_CONTROLr;   inst = 1; break;
        case IDB_OBM2_CONTROL_PIPE1r:   sid = IDB_OBM2_CONTROLr;   inst = 1; break;
        case IDB_OBM3_CONTROL_PIPE1r:   sid = IDB_OBM3_CONTROLr;   inst = 1; break;
        case IDB_OBM4_CONTROL_PIPE1r:   sid = IDB_OBM4_CONTROLr;   inst = 1; break;
        case IDB_OBM5_CONTROL_PIPE1r:   sid = IDB_OBM5_CONTROLr;   inst = 1; break;
        case IDB_OBM6_CONTROL_PIPE1r:   sid = IDB_OBM6_CONTROLr;   inst = 1; break;
        case IDB_OBM7_CONTROL_PIPE1r:   sid = IDB_OBM7_CONTROLr;   inst = 1; break;
        case IDB_OBM0_CONTROL_PIPE2r:   sid = IDB_OBM0_CONTROLr;   inst = 2; break;
        case IDB_OBM1_CONTROL_PIPE2r:   sid = IDB_OBM1_CONTROLr;   inst = 2; break;
        case IDB_OBM2_CONTROL_PIPE2r:   sid = IDB_OBM2_CONTROLr;   inst = 2; break;
        case IDB_OBM3_CONTROL_PIPE2r:   sid = IDB_OBM3_CONTROLr;   inst = 2; break;
        case IDB_OBM4_CONTROL_PIPE2r:   sid = IDB_OBM4_CONTROLr;   inst = 2; break;
        case IDB_OBM5_CONTROL_PIPE2r:   sid = IDB_OBM5_CONTROLr;   inst = 2; break;
        case IDB_OBM6_CONTROL_PIPE2r:   sid = IDB_OBM6_CONTROLr;   inst = 2; break;
        case IDB_OBM7_CONTROL_PIPE2r:   sid = IDB_OBM7_CONTROLr;   inst = 2; break;
        case IDB_OBM0_CONTROL_PIPE3r:   sid = IDB_OBM0_CONTROLr;   inst = 3; break;
        case IDB_OBM1_CONTROL_PIPE3r:   sid = IDB_OBM1_CONTROLr;   inst = 3; break;
        case IDB_OBM2_CONTROL_PIPE3r:   sid = IDB_OBM2_CONTROLr;   inst = 3; break;
        case IDB_OBM3_CONTROL_PIPE3r:   sid = IDB_OBM3_CONTROLr;   inst = 3; break;
        case IDB_OBM4_CONTROL_PIPE3r:   sid = IDB_OBM4_CONTROLr;   inst = 3; break;
        case IDB_OBM5_CONTROL_PIPE3r:   sid = IDB_OBM5_CONTROLr;   inst = 3; break;
        case IDB_OBM6_CONTROL_PIPE3r:   sid = IDB_OBM6_CONTROLr;   inst = 3; break;
        case IDB_OBM7_CONTROL_PIPE3r:   sid = IDB_OBM7_CONTROLr;   inst = 3; break;


        case IDB_OBM0_CA_CONTROL_PIPE0r:   sid = IDB_OBM0_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM1_CA_CONTROL_PIPE0r:   sid = IDB_OBM1_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM2_CA_CONTROL_PIPE0r:   sid = IDB_OBM2_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM3_CA_CONTROL_PIPE0r:   sid = IDB_OBM3_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM4_CA_CONTROL_PIPE0r:   sid = IDB_OBM4_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM5_CA_CONTROL_PIPE0r:   sid = IDB_OBM5_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM6_CA_CONTROL_PIPE0r:   sid = IDB_OBM6_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM7_CA_CONTROL_PIPE0r:   sid = IDB_OBM7_CA_CONTROLr;   inst = 0; break;
        case IDB_OBM0_CA_CONTROL_PIPE1r:   sid = IDB_OBM0_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM1_CA_CONTROL_PIPE1r:   sid = IDB_OBM1_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM2_CA_CONTROL_PIPE1r:   sid = IDB_OBM2_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM3_CA_CONTROL_PIPE1r:   sid = IDB_OBM3_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM4_CA_CONTROL_PIPE1r:   sid = IDB_OBM4_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM5_CA_CONTROL_PIPE1r:   sid = IDB_OBM5_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM6_CA_CONTROL_PIPE1r:   sid = IDB_OBM6_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM7_CA_CONTROL_PIPE1r:   sid = IDB_OBM7_CA_CONTROLr;   inst = 1; break;
        case IDB_OBM0_CA_CONTROL_PIPE2r:   sid = IDB_OBM0_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM1_CA_CONTROL_PIPE2r:   sid = IDB_OBM1_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM2_CA_CONTROL_PIPE2r:   sid = IDB_OBM2_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM3_CA_CONTROL_PIPE2r:   sid = IDB_OBM3_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM4_CA_CONTROL_PIPE2r:   sid = IDB_OBM4_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM5_CA_CONTROL_PIPE2r:   sid = IDB_OBM5_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM6_CA_CONTROL_PIPE2r:   sid = IDB_OBM6_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM7_CA_CONTROL_PIPE2r:   sid = IDB_OBM7_CA_CONTROLr;   inst = 2; break;
        case IDB_OBM0_CA_CONTROL_PIPE3r:   sid = IDB_OBM0_CA_CONTROLr;   inst = 3; break;
        case IDB_OBM1_CA_CONTROL_PIPE3r:   sid = IDB_OBM1_CA_CONTROLr;   inst = 3; break;
        case IDB_OBM2_CA_CONTROL_PIPE3r:   sid = IDB_OBM2_CA_CONTROLr;   inst = 3; break;
        case IDB_OBM3_CA_CONTROL_PIPE3r:   sid = IDB_OBM3_CA_CONTROLr;   inst = 3; break;
        case IDB_OBM4_CA_CONTROL_PIPE3r:   sid = IDB_OBM4_CA_CONTROLr;   inst = 3; break;
        case IDB_OBM5_CA_CONTROL_PIPE3r:   sid = IDB_OBM5_CA_CONTROLr;   inst = 3; break;
        case IDB_OBM6_CA_CONTROL_PIPE3r:   sid = IDB_OBM6_CA_CONTROLr;   inst = 3; break;
        case IDB_OBM7_CA_CONTROL_PIPE3r:   sid = IDB_OBM7_CA_CONTROLr;   inst = 3; break;


        case IDB_CA_CPU_CONTROL_PIPE0r:  sid = IDB_CA_CPU_CONTROLr; inst = 0; break;
        case IDB_CA_CPU_CONTROL_PIPE1r:  sid = IDB_CA_CPU_CONTROLr; inst = 1; break;
        case IDB_CA_CPU_CONTROL_PIPE2r:  sid = IDB_CA_CPU_CONTROLr; inst = 2; break;
        case IDB_CA_CPU_CONTROL_PIPE3r:  sid = IDB_CA_CPU_CONTROLr; inst = 3; break;

        case IDB_CA_LPBK_CONTROL_PIPE0r: sid = IDB_CA_LPBK_CONTROLr; inst = 0; break;
        case IDB_CA_LPBK_CONTROL_PIPE1r: sid = IDB_CA_LPBK_CONTROLr; inst = 1; break;
        case IDB_CA_LPBK_CONTROL_PIPE2r: sid = IDB_CA_LPBK_CONTROLr; inst = 2; break;
        case IDB_CA_LPBK_CONTROL_PIPE3r: sid = IDB_CA_LPBK_CONTROLr; inst = 3; break;


        case IDB_OBM0_MAX_USAGE_PIPE0r:   sid = IDB_OBM0_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM1_MAX_USAGE_PIPE0r:   sid = IDB_OBM1_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM2_MAX_USAGE_PIPE0r:   sid = IDB_OBM2_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM3_MAX_USAGE_PIPE0r:   sid = IDB_OBM3_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM4_MAX_USAGE_PIPE0r:   sid = IDB_OBM4_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM5_MAX_USAGE_PIPE0r:   sid = IDB_OBM5_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM6_MAX_USAGE_PIPE0r:   sid = IDB_OBM6_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM7_MAX_USAGE_PIPE0r:   sid = IDB_OBM7_MAX_USAGEr;   inst = 0; break;
        case IDB_OBM0_MAX_USAGE_PIPE1r:   sid = IDB_OBM0_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM1_MAX_USAGE_PIPE1r:   sid = IDB_OBM1_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM2_MAX_USAGE_PIPE1r:   sid = IDB_OBM2_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM3_MAX_USAGE_PIPE1r:   sid = IDB_OBM3_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM4_MAX_USAGE_PIPE1r:   sid = IDB_OBM4_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM5_MAX_USAGE_PIPE1r:   sid = IDB_OBM5_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM6_MAX_USAGE_PIPE1r:   sid = IDB_OBM6_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM7_MAX_USAGE_PIPE1r:   sid = IDB_OBM7_MAX_USAGEr;   inst = 1; break;
        case IDB_OBM0_MAX_USAGE_PIPE2r:   sid = IDB_OBM0_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM1_MAX_USAGE_PIPE2r:   sid = IDB_OBM1_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM2_MAX_USAGE_PIPE2r:   sid = IDB_OBM2_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM3_MAX_USAGE_PIPE2r:   sid = IDB_OBM3_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM4_MAX_USAGE_PIPE2r:   sid = IDB_OBM4_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM5_MAX_USAGE_PIPE2r:   sid = IDB_OBM5_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM6_MAX_USAGE_PIPE2r:   sid = IDB_OBM6_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM7_MAX_USAGE_PIPE2r:   sid = IDB_OBM7_MAX_USAGEr;   inst = 2; break;
        case IDB_OBM0_MAX_USAGE_PIPE3r:   sid = IDB_OBM0_MAX_USAGEr;   inst = 3; break;
        case IDB_OBM1_MAX_USAGE_PIPE3r:   sid = IDB_OBM1_MAX_USAGEr;   inst = 3; break;
        case IDB_OBM2_MAX_USAGE_PIPE3r:   sid = IDB_OBM2_MAX_USAGEr;   inst = 3; break;
        case IDB_OBM3_MAX_USAGE_PIPE3r:   sid = IDB_OBM3_MAX_USAGEr;   inst = 3; break;
        case IDB_OBM4_MAX_USAGE_PIPE3r:   sid = IDB_OBM4_MAX_USAGEr;   inst = 3; break;
        case IDB_OBM5_MAX_USAGE_PIPE3r:   sid = IDB_OBM5_MAX_USAGEr;   inst = 3; break;
        case IDB_OBM6_MAX_USAGE_PIPE3r:   sid = IDB_OBM6_MAX_USAGEr;   inst = 3; break;
        case IDB_OBM7_MAX_USAGE_PIPE3r:   sid = IDB_OBM7_MAX_USAGEr;   inst = 3; break;


        case IDB_OBM0_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM0_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM1_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM1_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM2_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM2_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM3_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM3_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM4_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM4_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM5_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM5_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM6_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM6_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM7_SHARED_CONFIG_PIPE0r:   sid = IDB_OBM7_SHARED_CONFIGr;   inst = 0; break;
        case IDB_OBM0_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM0_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM1_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM1_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM2_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM2_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM3_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM3_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM4_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM4_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM5_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM5_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM6_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM6_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM7_SHARED_CONFIG_PIPE1r:   sid = IDB_OBM7_SHARED_CONFIGr;   inst = 1; break;
        case IDB_OBM0_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM0_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM1_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM1_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM2_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM2_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM3_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM3_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM4_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM4_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM5_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM5_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM6_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM6_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM7_SHARED_CONFIG_PIPE2r:   sid = IDB_OBM7_SHARED_CONFIGr;   inst = 2; break;
        case IDB_OBM0_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM0_SHARED_CONFIGr;   inst = 3; break;
        case IDB_OBM1_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM1_SHARED_CONFIGr;   inst = 3; break;
        case IDB_OBM2_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM2_SHARED_CONFIGr;   inst = 3; break;
        case IDB_OBM3_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM3_SHARED_CONFIGr;   inst = 3; break;
        case IDB_OBM4_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM4_SHARED_CONFIGr;   inst = 3; break;
        case IDB_OBM5_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM5_SHARED_CONFIGr;   inst = 3; break;
        case IDB_OBM6_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM6_SHARED_CONFIGr;   inst = 3; break;
        case IDB_OBM7_SHARED_CONFIG_PIPE3r:   sid = IDB_OBM7_SHARED_CONFIGr;   inst = 3; break;


        case IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM0_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM1_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM2_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM3_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM4_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM5_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM6_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r:   sid = IDB_OBM7_FLOW_CONTROL_CONFIGr;   inst = 0; break;
        case IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM0_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM1_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM2_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM3_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM4_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM5_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM6_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE1r:   sid = IDB_OBM7_FLOW_CONTROL_CONFIGr;   inst = 1; break;
        case IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM0_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM1_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM2_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM3_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM4_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM5_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM6_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE2r:   sid = IDB_OBM7_FLOW_CONTROL_CONFIGr;   inst = 2; break;
        case IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM0_FLOW_CONTROL_CONFIGr;   inst = 3; break;
        case IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM1_FLOW_CONTROL_CONFIGr;   inst = 3; break;
        case IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM2_FLOW_CONTROL_CONFIGr;   inst = 3; break;
        case IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM3_FLOW_CONTROL_CONFIGr;   inst = 3; break;
        case IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM4_FLOW_CONTROL_CONFIGr;   inst = 3; break;
        case IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM5_FLOW_CONTROL_CONFIGr;   inst = 3; break;
        case IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM6_FLOW_CONTROL_CONFIGr;   inst = 3; break;
        case IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE3r:   sid = IDB_OBM7_FLOW_CONTROL_CONFIGr;   inst = 3; break;


        case IDB_OBM0_THRESHOLD_PIPE0r:   sid = IDB_OBM0_THRESHOLDr;   inst = 0; break;
        case IDB_OBM1_THRESHOLD_PIPE0r:   sid = IDB_OBM1_THRESHOLDr;   inst = 0; break;
        case IDB_OBM2_THRESHOLD_PIPE0r:   sid = IDB_OBM2_THRESHOLDr;   inst = 0; break;
        case IDB_OBM3_THRESHOLD_PIPE0r:   sid = IDB_OBM3_THRESHOLDr;   inst = 0; break;
        case IDB_OBM4_THRESHOLD_PIPE0r:   sid = IDB_OBM4_THRESHOLDr;   inst = 0; break;
        case IDB_OBM5_THRESHOLD_PIPE0r:   sid = IDB_OBM5_THRESHOLDr;   inst = 0; break;
        case IDB_OBM6_THRESHOLD_PIPE0r:   sid = IDB_OBM6_THRESHOLDr;   inst = 0; break;
        case IDB_OBM7_THRESHOLD_PIPE0r:   sid = IDB_OBM7_THRESHOLDr;   inst = 0; break;
        case IDB_OBM0_THRESHOLD_PIPE1r:   sid = IDB_OBM0_THRESHOLDr;   inst = 1; break;
        case IDB_OBM1_THRESHOLD_PIPE1r:   sid = IDB_OBM1_THRESHOLDr;   inst = 1; break;
        case IDB_OBM2_THRESHOLD_PIPE1r:   sid = IDB_OBM2_THRESHOLDr;   inst = 1; break;
        case IDB_OBM3_THRESHOLD_PIPE1r:   sid = IDB_OBM3_THRESHOLDr;   inst = 1; break;
        case IDB_OBM4_THRESHOLD_PIPE1r:   sid = IDB_OBM4_THRESHOLDr;   inst = 1; break;
        case IDB_OBM5_THRESHOLD_PIPE1r:   sid = IDB_OBM5_THRESHOLDr;   inst = 1; break;
        case IDB_OBM6_THRESHOLD_PIPE1r:   sid = IDB_OBM6_THRESHOLDr;   inst = 1; break;
        case IDB_OBM7_THRESHOLD_PIPE1r:   sid = IDB_OBM7_THRESHOLDr;   inst = 1; break;
        case IDB_OBM0_THRESHOLD_PIPE2r:   sid = IDB_OBM0_THRESHOLDr;   inst = 2; break;
        case IDB_OBM1_THRESHOLD_PIPE2r:   sid = IDB_OBM1_THRESHOLDr;   inst = 2; break;
        case IDB_OBM2_THRESHOLD_PIPE2r:   sid = IDB_OBM2_THRESHOLDr;   inst = 2; break;
        case IDB_OBM3_THRESHOLD_PIPE2r:   sid = IDB_OBM3_THRESHOLDr;   inst = 2; break;
        case IDB_OBM4_THRESHOLD_PIPE2r:   sid = IDB_OBM4_THRESHOLDr;   inst = 2; break;
        case IDB_OBM5_THRESHOLD_PIPE2r:   sid = IDB_OBM5_THRESHOLDr;   inst = 2; break;
        case IDB_OBM6_THRESHOLD_PIPE2r:   sid = IDB_OBM6_THRESHOLDr;   inst = 2; break;
        case IDB_OBM7_THRESHOLD_PIPE2r:   sid = IDB_OBM7_THRESHOLDr;   inst = 2; break;
        case IDB_OBM0_THRESHOLD_PIPE3r:   sid = IDB_OBM0_THRESHOLDr;   inst = 3; break;
        case IDB_OBM1_THRESHOLD_PIPE3r:   sid = IDB_OBM1_THRESHOLDr;   inst = 3; break;
        case IDB_OBM2_THRESHOLD_PIPE3r:   sid = IDB_OBM2_THRESHOLDr;   inst = 3; break;
        case IDB_OBM3_THRESHOLD_PIPE3r:   sid = IDB_OBM3_THRESHOLDr;   inst = 3; break;
        case IDB_OBM4_THRESHOLD_PIPE3r:   sid = IDB_OBM4_THRESHOLDr;   inst = 3; break;
        case IDB_OBM5_THRESHOLD_PIPE3r:   sid = IDB_OBM5_THRESHOLDr;   inst = 3; break;
        case IDB_OBM6_THRESHOLD_PIPE3r:   sid = IDB_OBM6_THRESHOLDr;   inst = 3; break;
        case IDB_OBM7_THRESHOLD_PIPE3r:   sid = IDB_OBM7_THRESHOLDr;   inst = 3; break;


        case IDB_OBM0_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM0_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM1_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM1_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM2_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM2_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM3_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM3_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM4_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM4_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM5_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM5_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM6_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM6_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM7_FC_THRESHOLD_PIPE0r:   sid = IDB_OBM7_FC_THRESHOLDr;   inst = 0; break;
        case IDB_OBM0_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM0_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM1_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM1_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM2_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM2_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM3_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM3_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM4_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM4_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM5_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM5_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM6_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM6_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM7_FC_THRESHOLD_PIPE1r:   sid = IDB_OBM7_FC_THRESHOLDr;   inst = 1; break;
        case IDB_OBM0_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM0_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM1_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM1_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM2_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM2_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM3_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM3_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM4_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM4_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM5_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM5_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM6_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM6_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM7_FC_THRESHOLD_PIPE2r:   sid = IDB_OBM7_FC_THRESHOLDr;   inst = 2; break;
        case IDB_OBM0_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM0_FC_THRESHOLDr;   inst = 3; break;
        case IDB_OBM1_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM1_FC_THRESHOLDr;   inst = 3; break;
        case IDB_OBM2_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM2_FC_THRESHOLDr;   inst = 3; break;
        case IDB_OBM3_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM3_FC_THRESHOLDr;   inst = 3; break;
        case IDB_OBM4_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM4_FC_THRESHOLDr;   inst = 3; break;
        case IDB_OBM5_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM5_FC_THRESHOLDr;   inst = 3; break;
        case IDB_OBM6_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM6_FC_THRESHOLDr;   inst = 3; break;
        case IDB_OBM7_FC_THRESHOLD_PIPE3r:   sid = IDB_OBM7_FC_THRESHOLDr;   inst = 3; break;


        case IDB_OBM0_USAGE_PIPE0r:   sid = IDB_OBM0_USAGEr;   inst = 0; break;
        case IDB_OBM1_USAGE_PIPE0r:   sid = IDB_OBM1_USAGEr;   inst = 0; break;
        case IDB_OBM2_USAGE_PIPE0r:   sid = IDB_OBM2_USAGEr;   inst = 0; break;
        case IDB_OBM3_USAGE_PIPE0r:   sid = IDB_OBM3_USAGEr;   inst = 0; break;
        case IDB_OBM4_USAGE_PIPE0r:   sid = IDB_OBM4_USAGEr;   inst = 0; break;
        case IDB_OBM5_USAGE_PIPE0r:   sid = IDB_OBM5_USAGEr;   inst = 0; break;
        case IDB_OBM6_USAGE_PIPE0r:   sid = IDB_OBM6_USAGEr;   inst = 0; break;
        case IDB_OBM7_USAGE_PIPE0r:   sid = IDB_OBM7_USAGEr;   inst = 0; break;
        case IDB_OBM0_USAGE_PIPE1r:   sid = IDB_OBM0_USAGEr;   inst = 1; break;
        case IDB_OBM1_USAGE_PIPE1r:   sid = IDB_OBM1_USAGEr;   inst = 1; break;
        case IDB_OBM2_USAGE_PIPE1r:   sid = IDB_OBM2_USAGEr;   inst = 1; break;
        case IDB_OBM3_USAGE_PIPE1r:   sid = IDB_OBM3_USAGEr;   inst = 1; break;
        case IDB_OBM4_USAGE_PIPE1r:   sid = IDB_OBM4_USAGEr;   inst = 1; break;
        case IDB_OBM5_USAGE_PIPE1r:   sid = IDB_OBM5_USAGEr;   inst = 1; break;
        case IDB_OBM6_USAGE_PIPE1r:   sid = IDB_OBM6_USAGEr;   inst = 1; break;
        case IDB_OBM7_USAGE_PIPE1r:   sid = IDB_OBM7_USAGEr;   inst = 1; break;
        case IDB_OBM0_USAGE_PIPE2r:   sid = IDB_OBM0_USAGEr;   inst = 2; break;
        case IDB_OBM1_USAGE_PIPE2r:   sid = IDB_OBM1_USAGEr;   inst = 2; break;
        case IDB_OBM2_USAGE_PIPE2r:   sid = IDB_OBM2_USAGEr;   inst = 2; break;
        case IDB_OBM3_USAGE_PIPE2r:   sid = IDB_OBM3_USAGEr;   inst = 2; break;
        case IDB_OBM4_USAGE_PIPE2r:   sid = IDB_OBM4_USAGEr;   inst = 2; break;
        case IDB_OBM5_USAGE_PIPE2r:   sid = IDB_OBM5_USAGEr;   inst = 2; break;
        case IDB_OBM6_USAGE_PIPE2r:   sid = IDB_OBM6_USAGEr;   inst = 2; break;
        case IDB_OBM7_USAGE_PIPE2r:   sid = IDB_OBM7_USAGEr;   inst = 2; break;
        case IDB_OBM0_USAGE_PIPE3r:   sid = IDB_OBM0_USAGEr;   inst = 3; break;
        case IDB_OBM1_USAGE_PIPE3r:   sid = IDB_OBM1_USAGEr;   inst = 3; break;
        case IDB_OBM2_USAGE_PIPE3r:   sid = IDB_OBM2_USAGEr;   inst = 3; break;
        case IDB_OBM3_USAGE_PIPE3r:   sid = IDB_OBM3_USAGEr;   inst = 3; break;
        case IDB_OBM4_USAGE_PIPE3r:   sid = IDB_OBM4_USAGEr;   inst = 3; break;
        case IDB_OBM5_USAGE_PIPE3r:   sid = IDB_OBM5_USAGEr;   inst = 3; break;
        case IDB_OBM6_USAGE_PIPE3r:   sid = IDB_OBM6_USAGEr;   inst = 3; break;
        case IDB_OBM7_USAGE_PIPE3r:   sid = IDB_OBM7_USAGEr;   inst = 3; break;


        case IDB_OBM0_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM0_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM1_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM1_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM2_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM2_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM3_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM3_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM4_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM4_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM5_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM5_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM6_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM6_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM7_CA_HW_STATUS_PIPE0r:   sid = IDB_OBM7_CA_HW_STATUSr;   inst = 0; break;
        case IDB_OBM0_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM0_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM1_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM1_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM2_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM2_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM3_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM3_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM4_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM4_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM5_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM5_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM6_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM6_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM7_CA_HW_STATUS_PIPE1r:   sid = IDB_OBM7_CA_HW_STATUSr;   inst = 1; break;
        case IDB_OBM0_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM0_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM1_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM1_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM2_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM2_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM3_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM3_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM4_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM4_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM5_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM5_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM6_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM6_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM7_CA_HW_STATUS_PIPE2r:   sid = IDB_OBM7_CA_HW_STATUSr;   inst = 2; break;
        case IDB_OBM0_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM0_CA_HW_STATUSr;   inst = 3; break;
        case IDB_OBM1_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM1_CA_HW_STATUSr;   inst = 3; break;
        case IDB_OBM2_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM2_CA_HW_STATUSr;   inst = 3; break;
        case IDB_OBM3_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM3_CA_HW_STATUSr;   inst = 3; break;
        case IDB_OBM4_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM4_CA_HW_STATUSr;   inst = 3; break;
        case IDB_OBM5_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM5_CA_HW_STATUSr;   inst = 3; break;
        case IDB_OBM6_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM6_CA_HW_STATUSr;   inst = 3; break;
        case IDB_OBM7_CA_HW_STATUS_PIPE3r:   sid = IDB_OBM7_CA_HW_STATUSr;   inst = 3; break;


        case IDB_OBM0_PORT_CONFIG_PIPE0r:   sid = IDB_OBM0_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM1_PORT_CONFIG_PIPE0r:   sid = IDB_OBM1_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM2_PORT_CONFIG_PIPE0r:   sid = IDB_OBM2_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM3_PORT_CONFIG_PIPE0r:   sid = IDB_OBM3_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM4_PORT_CONFIG_PIPE0r:   sid = IDB_OBM4_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM5_PORT_CONFIG_PIPE0r:   sid = IDB_OBM5_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM6_PORT_CONFIG_PIPE0r:   sid = IDB_OBM6_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM7_PORT_CONFIG_PIPE0r:   sid = IDB_OBM7_PORT_CONFIGr;   inst = 0; break;
        case IDB_OBM0_PORT_CONFIG_PIPE1r:   sid = IDB_OBM0_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM1_PORT_CONFIG_PIPE1r:   sid = IDB_OBM1_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM2_PORT_CONFIG_PIPE1r:   sid = IDB_OBM2_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM3_PORT_CONFIG_PIPE1r:   sid = IDB_OBM3_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM4_PORT_CONFIG_PIPE1r:   sid = IDB_OBM4_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM5_PORT_CONFIG_PIPE1r:   sid = IDB_OBM5_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM6_PORT_CONFIG_PIPE1r:   sid = IDB_OBM6_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM7_PORT_CONFIG_PIPE1r:   sid = IDB_OBM7_PORT_CONFIGr;   inst = 1; break;
        case IDB_OBM0_PORT_CONFIG_PIPE2r:   sid = IDB_OBM0_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM1_PORT_CONFIG_PIPE2r:   sid = IDB_OBM1_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM2_PORT_CONFIG_PIPE2r:   sid = IDB_OBM2_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM3_PORT_CONFIG_PIPE2r:   sid = IDB_OBM3_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM4_PORT_CONFIG_PIPE2r:   sid = IDB_OBM4_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM5_PORT_CONFIG_PIPE2r:   sid = IDB_OBM5_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM6_PORT_CONFIG_PIPE2r:   sid = IDB_OBM6_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM7_PORT_CONFIG_PIPE2r:   sid = IDB_OBM7_PORT_CONFIGr;   inst = 2; break;
        case IDB_OBM0_PORT_CONFIG_PIPE3r:   sid = IDB_OBM0_PORT_CONFIGr;   inst = 3; break;
        case IDB_OBM1_PORT_CONFIG_PIPE3r:   sid = IDB_OBM1_PORT_CONFIGr;   inst = 3; break;
        case IDB_OBM2_PORT_CONFIG_PIPE3r:   sid = IDB_OBM2_PORT_CONFIGr;   inst = 3; break;
        case IDB_OBM3_PORT_CONFIG_PIPE3r:   sid = IDB_OBM3_PORT_CONFIGr;   inst = 3; break;
        case IDB_OBM4_PORT_CONFIG_PIPE3r:   sid = IDB_OBM4_PORT_CONFIGr;   inst = 3; break;
        case IDB_OBM5_PORT_CONFIG_PIPE3r:   sid = IDB_OBM5_PORT_CONFIGr;   inst = 3; break;
        case IDB_OBM6_PORT_CONFIG_PIPE3r:   sid = IDB_OBM6_PORT_CONFIGr;   inst = 3; break;
        case IDB_OBM7_PORT_CONFIG_PIPE3r:   sid = IDB_OBM7_PORT_CONFIGr;   inst = 3; break;


        case IDB_OBM0_DBG_A_PIPE0r:   sid = IDB_OBM0_DBG_Ar;   inst = 0; break;
        case IDB_OBM1_DBG_A_PIPE0r:   sid = IDB_OBM1_DBG_Ar;   inst = 0; break;
        case IDB_OBM2_DBG_A_PIPE0r:   sid = IDB_OBM2_DBG_Ar;   inst = 0; break;
        case IDB_OBM3_DBG_A_PIPE0r:   sid = IDB_OBM3_DBG_Ar;   inst = 0; break;
        case IDB_OBM4_DBG_A_PIPE0r:   sid = IDB_OBM4_DBG_Ar;   inst = 0; break;
        case IDB_OBM5_DBG_A_PIPE0r:   sid = IDB_OBM5_DBG_Ar;   inst = 0; break;
        case IDB_OBM6_DBG_A_PIPE0r:   sid = IDB_OBM6_DBG_Ar;   inst = 0; break;
        case IDB_OBM7_DBG_A_PIPE0r:   sid = IDB_OBM7_DBG_Ar;   inst = 0; break;
        case IDB_OBM0_DBG_A_PIPE1r:   sid = IDB_OBM0_DBG_Ar;   inst = 1; break;
        case IDB_OBM1_DBG_A_PIPE1r:   sid = IDB_OBM1_DBG_Ar;   inst = 1; break;
        case IDB_OBM2_DBG_A_PIPE1r:   sid = IDB_OBM2_DBG_Ar;   inst = 1; break;
        case IDB_OBM3_DBG_A_PIPE1r:   sid = IDB_OBM3_DBG_Ar;   inst = 1; break;
        case IDB_OBM4_DBG_A_PIPE1r:   sid = IDB_OBM4_DBG_Ar;   inst = 1; break;
        case IDB_OBM5_DBG_A_PIPE1r:   sid = IDB_OBM5_DBG_Ar;   inst = 1; break;
        case IDB_OBM6_DBG_A_PIPE1r:   sid = IDB_OBM6_DBG_Ar;   inst = 1; break;
        case IDB_OBM7_DBG_A_PIPE1r:   sid = IDB_OBM7_DBG_Ar;   inst = 1; break;
        case IDB_OBM0_DBG_A_PIPE2r:   sid = IDB_OBM0_DBG_Ar;   inst = 2; break;
        case IDB_OBM1_DBG_A_PIPE2r:   sid = IDB_OBM1_DBG_Ar;   inst = 2; break;
        case IDB_OBM2_DBG_A_PIPE2r:   sid = IDB_OBM2_DBG_Ar;   inst = 2; break;
        case IDB_OBM3_DBG_A_PIPE2r:   sid = IDB_OBM3_DBG_Ar;   inst = 2; break;
        case IDB_OBM4_DBG_A_PIPE2r:   sid = IDB_OBM4_DBG_Ar;   inst = 2; break;
        case IDB_OBM5_DBG_A_PIPE2r:   sid = IDB_OBM5_DBG_Ar;   inst = 2; break;
        case IDB_OBM6_DBG_A_PIPE2r:   sid = IDB_OBM6_DBG_Ar;   inst = 2; break;
        case IDB_OBM7_DBG_A_PIPE2r:   sid = IDB_OBM7_DBG_Ar;   inst = 2; break;
        case IDB_OBM0_DBG_A_PIPE3r:   sid = IDB_OBM0_DBG_Ar;   inst = 3; break;
        case IDB_OBM1_DBG_A_PIPE3r:   sid = IDB_OBM1_DBG_Ar;   inst = 3; break;
        case IDB_OBM2_DBG_A_PIPE3r:   sid = IDB_OBM2_DBG_Ar;   inst = 3; break;
        case IDB_OBM3_DBG_A_PIPE3r:   sid = IDB_OBM3_DBG_Ar;   inst = 3; break;
        case IDB_OBM4_DBG_A_PIPE3r:   sid = IDB_OBM4_DBG_Ar;   inst = 3; break;
        case IDB_OBM5_DBG_A_PIPE3r:   sid = IDB_OBM5_DBG_Ar;   inst = 3; break;
        case IDB_OBM6_DBG_A_PIPE3r:   sid = IDB_OBM6_DBG_Ar;   inst = 3; break;
        case IDB_OBM7_DBG_A_PIPE3r:   sid = IDB_OBM7_DBG_Ar;   inst = 3; break;


        case IDB_CA_CPU_HW_STATUS_PIPE0r:  sid = IDB_CA_CPU_HW_STATUSr; inst = 0; break;
        case IDB_CA_CPU_HW_STATUS_PIPE1r:  sid = IDB_CA_CPU_HW_STATUSr; inst = 1; break;
        case IDB_CA_CPU_HW_STATUS_PIPE2r:  sid = IDB_CA_CPU_HW_STATUSr; inst = 2; break;
        case IDB_CA_CPU_HW_STATUS_PIPE3r:  sid = IDB_CA_CPU_HW_STATUSr; inst = 3; break;

        case IDB_CA_LPBK_HW_STATUS_PIPE0r: sid = IDB_CA_LPBK_HW_STATUSr; inst = 0; break;
        case IDB_CA_LPBK_HW_STATUS_PIPE1r: sid = IDB_CA_LPBK_HW_STATUSr; inst = 1; break;
        case IDB_CA_LPBK_HW_STATUS_PIPE2r: sid = IDB_CA_LPBK_HW_STATUSr; inst = 2; break;
        case IDB_CA_LPBK_HW_STATUS_PIPE3r: sid = IDB_CA_LPBK_HW_STATUSr; inst = 3; break;


        case IDB_DBG_B_PIPE0r:  sid = IDB_DBG_Br; inst = 0; break;
        case IDB_DBG_B_PIPE1r:  sid = IDB_DBG_Br; inst = 1; break;
        case IDB_DBG_B_PIPE2r:  sid = IDB_DBG_Br; inst = 2; break;
        case IDB_DBG_B_PIPE3r:  sid = IDB_DBG_Br; inst = 3; break;

        case IS_PBLK0_CALENDAR_PIPE0r: sid = IS_PBLK0_CALENDARr; inst = 0; break;
        case IS_PBLK0_CALENDAR_PIPE1r: sid = IS_PBLK0_CALENDARr; inst = 1; break;
        case IS_PBLK0_CALENDAR_PIPE2r: sid = IS_PBLK0_CALENDARr; inst = 2; break;
        case IS_PBLK0_CALENDAR_PIPE3r: sid = IS_PBLK0_CALENDARr; inst = 3; break;

        case IS_PBLK1_CALENDAR_PIPE0r: sid = IS_PBLK1_CALENDARr; inst = 0; break;
        case IS_PBLK1_CALENDAR_PIPE1r: sid = IS_PBLK1_CALENDARr; inst = 1; break;
        case IS_PBLK1_CALENDAR_PIPE2r: sid = IS_PBLK1_CALENDARr; inst = 2; break;
        case IS_PBLK1_CALENDAR_PIPE3r: sid = IS_PBLK1_CALENDARr; inst = 3; break;

        case IS_PBLK2_CALENDAR_PIPE0r: sid = IS_PBLK2_CALENDARr; inst = 0; break;
        case IS_PBLK2_CALENDAR_PIPE1r: sid = IS_PBLK2_CALENDARr; inst = 1; break;
        case IS_PBLK2_CALENDAR_PIPE2r: sid = IS_PBLK2_CALENDARr; inst = 2; break;
        case IS_PBLK2_CALENDAR_PIPE3r: sid = IS_PBLK2_CALENDARr; inst = 3; break;

        case IS_PBLK3_CALENDAR_PIPE0r: sid = IS_PBLK3_CALENDARr; inst = 0; break;
        case IS_PBLK3_CALENDAR_PIPE1r: sid = IS_PBLK3_CALENDARr; inst = 1; break;
        case IS_PBLK3_CALENDAR_PIPE2r: sid = IS_PBLK3_CALENDARr; inst = 2; break;
        case IS_PBLK3_CALENDAR_PIPE3r: sid = IS_PBLK3_CALENDARr; inst = 3; break;

        case IS_PBLK4_CALENDAR_PIPE0r: sid = IS_PBLK4_CALENDARr; inst = 0; break;
        case IS_PBLK4_CALENDAR_PIPE1r: sid = IS_PBLK4_CALENDARr; inst = 1; break;
        case IS_PBLK4_CALENDAR_PIPE2r: sid = IS_PBLK4_CALENDARr; inst = 2; break;
        case IS_PBLK4_CALENDAR_PIPE3r: sid = IS_PBLK4_CALENDARr; inst = 3; break;

        case IS_PBLK5_CALENDAR_PIPE0r: sid = IS_PBLK5_CALENDARr; inst = 0; break;
        case IS_PBLK5_CALENDAR_PIPE1r: sid = IS_PBLK5_CALENDARr; inst = 1; break;
        case IS_PBLK5_CALENDAR_PIPE2r: sid = IS_PBLK5_CALENDARr; inst = 2; break;
        case IS_PBLK5_CALENDAR_PIPE3r: sid = IS_PBLK5_CALENDARr; inst = 3; break;

        case IS_PBLK6_CALENDAR_PIPE0r: sid = IS_PBLK6_CALENDARr; inst = 0; break;
        case IS_PBLK6_CALENDAR_PIPE1r: sid = IS_PBLK6_CALENDARr; inst = 1; break;
        case IS_PBLK6_CALENDAR_PIPE2r: sid = IS_PBLK6_CALENDARr; inst = 2; break;
        case IS_PBLK6_CALENDAR_PIPE3r: sid = IS_PBLK6_CALENDARr; inst = 3; break;

        case IS_PBLK7_CALENDAR_PIPE0r: sid = IS_PBLK7_CALENDARr; inst = 0; break;
        case IS_PBLK7_CALENDAR_PIPE1r: sid = IS_PBLK7_CALENDARr; inst = 1; break;
        case IS_PBLK7_CALENDAR_PIPE2r: sid = IS_PBLK7_CALENDARr; inst = 2; break;
        case IS_PBLK7_CALENDAR_PIPE3r: sid = IS_PBLK7_CALENDARr; inst = 3; break;

        case IS_OVR_SUB_GRP_CFG_PIPE0r: sid = IS_OVR_SUB_GRP_CFGr; inst = 0; break;
        case IS_OVR_SUB_GRP_CFG_PIPE1r: sid = IS_OVR_SUB_GRP_CFGr; inst = 1; break;
        case IS_OVR_SUB_GRP_CFG_PIPE2r: sid = IS_OVR_SUB_GRP_CFGr; inst = 2; break;
        case IS_OVR_SUB_GRP_CFG_PIPE3r: sid = IS_OVR_SUB_GRP_CFGr; inst = 3; break;

        case IS_OVR_SUB_GRP0_TBL_PIPE0r: sid = IS_OVR_SUB_GRP0_TBLr; inst = 0; break;
        case IS_OVR_SUB_GRP1_TBL_PIPE0r: sid = IS_OVR_SUB_GRP1_TBLr; inst = 0; break;
        case IS_OVR_SUB_GRP2_TBL_PIPE0r: sid = IS_OVR_SUB_GRP2_TBLr; inst = 0; break;
        case IS_OVR_SUB_GRP3_TBL_PIPE0r: sid = IS_OVR_SUB_GRP3_TBLr; inst = 0; break;
        case IS_OVR_SUB_GRP4_TBL_PIPE0r: sid = IS_OVR_SUB_GRP4_TBLr; inst = 0; break;
        case IS_OVR_SUB_GRP5_TBL_PIPE0r: sid = IS_OVR_SUB_GRP5_TBLr; inst = 0; break;
        case IS_OVR_SUB_GRP0_TBL_PIPE1r: sid = IS_OVR_SUB_GRP0_TBLr; inst = 1; break;
        case IS_OVR_SUB_GRP1_TBL_PIPE1r: sid = IS_OVR_SUB_GRP1_TBLr; inst = 1; break;
        case IS_OVR_SUB_GRP2_TBL_PIPE1r: sid = IS_OVR_SUB_GRP2_TBLr; inst = 1; break;
        case IS_OVR_SUB_GRP3_TBL_PIPE1r: sid = IS_OVR_SUB_GRP3_TBLr; inst = 1; break;
        case IS_OVR_SUB_GRP4_TBL_PIPE1r: sid = IS_OVR_SUB_GRP4_TBLr; inst = 1; break;
        case IS_OVR_SUB_GRP5_TBL_PIPE1r: sid = IS_OVR_SUB_GRP5_TBLr; inst = 1; break;
        case IS_OVR_SUB_GRP0_TBL_PIPE2r: sid = IS_OVR_SUB_GRP0_TBLr; inst = 2; break;
        case IS_OVR_SUB_GRP1_TBL_PIPE2r: sid = IS_OVR_SUB_GRP1_TBLr; inst = 2; break;
        case IS_OVR_SUB_GRP2_TBL_PIPE2r: sid = IS_OVR_SUB_GRP2_TBLr; inst = 2; break;
        case IS_OVR_SUB_GRP3_TBL_PIPE2r: sid = IS_OVR_SUB_GRP3_TBLr; inst = 2; break;
        case IS_OVR_SUB_GRP4_TBL_PIPE2r: sid = IS_OVR_SUB_GRP4_TBLr; inst = 2; break;
        case IS_OVR_SUB_GRP5_TBL_PIPE2r: sid = IS_OVR_SUB_GRP5_TBLr; inst = 2; break;
        case IS_OVR_SUB_GRP0_TBL_PIPE3r: sid = IS_OVR_SUB_GRP0_TBLr; inst = 3; break;
        case IS_OVR_SUB_GRP1_TBL_PIPE3r: sid = IS_OVR_SUB_GRP1_TBLr; inst = 3; break;
        case IS_OVR_SUB_GRP2_TBL_PIPE3r: sid = IS_OVR_SUB_GRP2_TBLr; inst = 3; break;
        case IS_OVR_SUB_GRP3_TBL_PIPE3r: sid = IS_OVR_SUB_GRP3_TBLr; inst = 3; break;
        case IS_OVR_SUB_GRP4_TBL_PIPE3r: sid = IS_OVR_SUB_GRP4_TBLr; inst = 3; break;
        case IS_OVR_SUB_GRP5_TBL_PIPE3r: sid = IS_OVR_SUB_GRP5_TBLr; inst = 3; break;

        case IS_TDM_CONFIG_PIPE0r: sid = IS_TDM_CONFIGr; inst = 0; break;
        case IS_TDM_CONFIG_PIPE1r: sid = IS_TDM_CONFIGr; inst = 1; break;
        case IS_TDM_CONFIG_PIPE2r: sid = IS_TDM_CONFIGr; inst = 2; break;
        case IS_TDM_CONFIG_PIPE3r: sid = IS_TDM_CONFIGr; inst = 3; break;

        case IS_CPU_LB_OPP_CFG_PIPE0r: sid = IS_CPU_LB_OPP_CFGr; inst = 0; break;
        case IS_CPU_LB_OPP_CFG_PIPE1r: sid = IS_CPU_LB_OPP_CFGr; inst = 1; break;
        case IS_CPU_LB_OPP_CFG_PIPE2r: sid = IS_CPU_LB_OPP_CFGr; inst = 2; break;
        case IS_CPU_LB_OPP_CFG_PIPE3r: sid = IS_CPU_LB_OPP_CFGr; inst = 3; break;


        case IS_OPP_SCHED_CFG_PIPE0r: sid = IS_OPP_SCHED_CFGr; inst = 0; break;
        case IS_OPP_SCHED_CFG_PIPE1r: sid = IS_OPP_SCHED_CFGr; inst = 1; break;
        case IS_OPP_SCHED_CFG_PIPE2r: sid = IS_OPP_SCHED_CFGr; inst = 2; break;
        case IS_OPP_SCHED_CFG_PIPE3r: sid = IS_OPP_SCHED_CFGr; inst = 3; break;

        case IS_TDM_HSP_PIPE0r: sid = IS_TDM_HSPr; inst = 0; break;
        case IS_TDM_HSP_PIPE1r: sid = IS_TDM_HSPr; inst = 1; break;
        case IS_TDM_HSP_PIPE2r: sid = IS_TDM_HSPr; inst = 2; break;
        case IS_TDM_HSP_PIPE3r: sid = IS_TDM_HSPr; inst = 3; break;

        case EGR_EDB_MISC_CTRL_PIPE0r:  sid = EGR_EDB_MISC_CTRLr; inst = 0; break;
        case EGR_EDB_MISC_CTRL_PIPE1r:  sid = EGR_EDB_MISC_CTRLr; inst = 1; break;
        case EGR_EDB_MISC_CTRL_PIPE2r:  sid = EGR_EDB_MISC_CTRLr; inst = 2; break;
        case EGR_EDB_MISC_CTRL_PIPE3r:  sid = EGR_EDB_MISC_CTRLr; inst = 3; break;

        case EGR_PORT_BUFFER_SFT_RESET_0_PIPE0r:  sid = EGR_PORT_BUFFER_SFT_RESET_0r; inst = 0; break;
        case EGR_PORT_BUFFER_SFT_RESET_0_PIPE1r:  sid = EGR_PORT_BUFFER_SFT_RESET_0r; inst = 1; break;
        case EGR_PORT_BUFFER_SFT_RESET_0_PIPE2r:  sid = EGR_PORT_BUFFER_SFT_RESET_0r; inst = 2; break;
        case EGR_PORT_BUFFER_SFT_RESET_0_PIPE3r:  sid = EGR_PORT_BUFFER_SFT_RESET_0r; inst = 3; break;

        case EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr: sid = EGR_DEV_TO_PHYS_MAPr; inst = -1; break;

        case MMU_PORT_TO_DEVICE_PORT_MAPPINGr:  sid = MMU_TO_DEV_MAPr; inst = 0; break;
        case MMU_PORT_TO_PHY_PORT_MAPPINGr:     sid = MMU_TO_PHYS_MAPr; inst = 0; break;


        default:
            bcm56960_a0_bcmtm_mmu_pt_sid_rename(unit, in_sid, in_inst, &sid, &inst);
    }

    *out_sid = sid;
    *instance = inst;

    return;
}
