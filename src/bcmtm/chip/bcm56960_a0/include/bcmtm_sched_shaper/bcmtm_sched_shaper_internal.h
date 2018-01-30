/*! \file bcmtm_sched_shaper_internal.h
 *
 * contains logical table  and hardware table mapping information for front
 * panel port scheduler and shapers (BCM56960_A0).
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

#ifndef BCMTM_SCHED_SHAPER_INTERNAL_H
#define BCMTM_SCHED_SHAPER_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>

#define BCMTM_MAX_PORT    135
#define BCMTM_MAX_FP_NODE    3900
#define BCMTM_MAX_COS          10

#define BCMTM_MAX_PIPE  4
#define BCMTM_MAX_SLICE 2
#define BCMTM_MAX_SCHED_LEVEL 3
#define BCMTM_MAX_SCHED_MODE 4


#define FP_MC_BASE 330

struct bcmtm_sched_shaper_config {
    uint32_t entry_bitmap[BCMTM_MAX_SCHED_LEVEL][(BCMTM_MAX_FP_NODE/32)+1];
};

/*Logical table fields order for node shapers*/
enum bcmtm_node_shaper {
    BCMTM_NODE_SHAPER_EAV = 0,
    BCMTM_NODE_SHAPER_MODE,
    BCMTM_NODE_SHAPER_MAX_REFRESHRATE,
    BCMTM_NODE_SHAPER_MAX_GRANULARITY,
    BCMTM_NODE_SHAPER_MAX_BUCKETSIZE,
    BCMTM_NODE_SHAPER_MIN_REFRESHRATE,
    BCMTM_NODE_SHAPER_MIN_GRANULARITY,
    BCMTM_NODE_SHAPER_MIN_BUCKETSIZE,
    BCMTM_NODE_SHAPER_MAXFID,
};

/* Physical memory for node shapers */
static
int bcmtm_node_shaper_sid[BCMTM_MAX_SCHED_LEVEL] = {
    MMU_MTRO_L0_MEMm,
    MMU_MTRO_L1_MEMm,
    MMU_MTRO_L1_MEMm
};

/* Hardware field id for node shaper memory */
static
int bcmtm_node_shaper_fid[BCMTM_NODE_SHAPER_MAXFID] = {
    EAV_ENABLEf,
    SHAPER_CONTROLf,
    MAX_REFRESHf,
    MAX_METER_GRANf,
    MAX_THD_SELf,
    MIN_REFRESHf,
    MIN_METER_GRANf,
    MIN_THD_SELf
};

/* Scheduler Weight fields */
enum bcmtm_sched_weight{
    BCMTM_SCHED_WEIGHT = 0,
    BCMTM_SCHED_WEIGHT_MAXFID,
};

/* Hardware memory for Scheduler weights */
static
int bcmtm_sched_weight_sid[BCMTM_MAX_SCHED_LEVEL] = {
    Q_SCHED_L0_WEIGHT_MEMm,
    Q_SCHED_L1_WEIGHT_MEMm,
    Q_SCHED_L1_WEIGHT_MEMm
};

/* Field id for the Scheduler weights */
static
int bcmtm_sched_weight_fid[BCMTM_SCHED_WEIGHT_MAXFID] = {
    WEIGHTf,
};

/* FP scheduler lt fields */
enum bcmtm_fp_scheduler {
    BCMTM_FP_SCHED_WRR = 0,
    BCMTM_FP_SCHED_SP,
    BCMTM_FP_SCHED_MASK,
    BCMTM_FP_SCHED_MAXFID,
};

/* FP scheduler hardware register */
static
int bcmtm_fp_scheduler_sid[BCMTM_MAX_SCHED_MODE] = {
    Q_SCHED_PORT_CONFIGr,
    Q_SCHED_L0_NODE_CONFIGr,
    Q_SCHED_L1_UC_QUEUE_CONFIGr,
    Q_SCHED_L1_MC_QUEUE_CONFIGr
};

/* Field Id for FP scheduler registers */
static
int bcmtm_fp_scheduler_fid[BCMTM_FP_SCHED_MAXFID] = {
    ENABLE_WRRf,
    ENABLE_SP_IN_MINf,
    MASKf
};

#endif /* BCMTM_SCHED_SHAPER_INTERNAL_H */
