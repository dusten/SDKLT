/*! \file tomahawk_mmu_defines.h
 *
 * Macros used in APIs of MMU for Tomahawk.
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

#ifndef TOMAHAWK_MMU_DEFINES_H
#define TOMAHAWK_MMU_DEFINES_H

/*! @file tomahawk_mmu_defines.h
 *  @brief Macros used in APIs of MMU for Tomahawk.
 *  Details are shown below.
 */

#define TH_MMU_NUM_L0_NODES_PER_HSP_PORT 10
#define TH_MMU_NUM_L0_NODES_FOR_CPU_PORT 48
#define TH_MMU_NUM_UCQ_PER_HSP_PORT 10
#define TH_MMU_NUM_MCQ_PER_HSP_PORT 10
#define TH_MMU_NUM_MCQ_FOR_CPU_PORT 48
#define TH_MMU_CPU_PORT_NUM 32
#define TH_MMU_MGMT_PORT_NUM 32
#define TH_MMU_MAX_PORTS_PER_PIPE 34
#define TH_MMU_NUM_HSP_PORTS_PER_PIPE 32
#define TH_MMU_LPBK_PORT_NUM 33
#define TH_INVALID_MMU_PORT 255
#define TH_INVALID_PHY_PORT_NUMBER 511
#define TH_INVALID_DEV_PORT_NUMBER 255
#define TH_MMU_PORT_PIPE_OFFSET 32
#define TH_MMU_MCQ_NUMBER_START 330
#define TH_MMU_NUM_UCQ 330
#define TH_MMU_NUM_MC_QUEUES 378
#define TH_MMU_NUM_MCQ_FP_PORTS 320
#define TH_MMU_LPBK_PORT_L1_START TH_MMU_MCQ_NUMBER_START+ \
    TH_MMU_NUM_MCQ_FP_PORTS
#define TH_MMU_MGMT_CPU_PORT_L1_START TH_MMU_LPBK_PORT_L1_START+ \
    TH_MMU_NUM_MCQ_PER_HSP_PORT
#define TH_MMU_PIPE1_QUEUE_START TH_MMU_NUM_UC_QUEUES+TH_MMU_NUM_MC_QUEUES


#define TH_MMU_BST_TRACK_MODE 0
#define TH_MMU_BST_WMARK_MODE 1
#define TH_MMU_MEM_ADDR__MEMIDX_WIDTH 16
#define TH_MMU_NUM_XPES 4
#define TH_MMU_NUM_XPES_PER_PIPE 2
#define TH_MMU_NUM_PGS 8
#define TH_MMU_NUM_SPS 4
#define TH_MMU_NUM_COS 10
#define TH_MMU_NUM_UC_QUEUES 330
#define TH_MMU_NUM_QUEUES 330
#define TH_MMU_NUM_PORTS_PER_PIPE 34
#define TH_MMU_ACC_TYPE__ADDR_SPLIT_DIST 10
#define TH_MMU_ACC_TYPE__ADDR_SPLIT_SPLIT 12
#define TH_MMU_ACC_TYPE__DATA_SPLIT 14
#define TH_MMU_ACC_TYPE__DUPLICATE 9
#define TH_MMU_ACC_TYPE__SINGLE 20
#define TH_MMU_ACC_TYPE__UNIQUE 31
#define TH_MMU_ACC_TYPE__UNIQUE_PIPE0 0
#define TH_MMU_ACC_TYPE__UNIQUE_PIPE1 1
#define TH_MMU_ACC_TYPE__UNIQUE_PIPE2 2
#define TH_MMU_ACC_TYPE__UNIQUE_PIPE3 3
#define TH_MMU_BASE_TYPE__XPE 5
#define TH_MMU_BASE_TYPE__CHIP 4
#define TH_MMU_BASE_TYPE__SLICE 6
#define TH_MMU_BASE_TYPE__EPORT 1
#define TH_MMU_BASE_TYPE__IPIPE 2
#define TH_MMU_BASE_TYPE__EPIPE 3
#define TH_MMU_BASE_TYPE__IPORT 0
#define TH_MMU_BASE_TYPE__LAYER 7
#define TH_MMU_THDU_UCQ_PER_PORT 10
#define TH_MMU_THDI_PG_MIN_LIMIT_LOSSLESS 8
#define TH_MMU_THDI_PG_MIN_LIMIT_LOSSY 0
#define TH_MMU_THDI_PG_HDRM_LIMIT_LOSSY 0
#define TH_MMU_THDI_SP_MAX_LIMIT 53248
#define TH_MMU_THDI_SP_RESUME_LIMIT 53232
#define TH_MMU_NUM_SPEEDS 6
#define TH_MMU_NUM_PORT_COLOR_DROP_ENTRIES 5
#define TH_OTP_OFF_NUM_PIPES 2

#define TH_MMU_THDI_PORT_XON_ENABLE_VAL 65535
#define TH_MMU_THDU_QGRP_MIN_LIMIT_LOSELESS 16
#define TH_MMU_THDU_QGRP_MIN_LIMIT_LOSSY TH_MMU_THDU_QGRP_MIN_LIMIT_LOSELESS
#define TH_MMU_THDU_QUEUE_SHARED_ALPHA_LOSSY 8
#define TH_MMU_THDU_QUEUE_SHARED_ALPHA_LOSSLESS 52296
#define TH_MMU_THDU_QUEUE_SHARED_LIMIT_LOSSY 8
#define TH_MMU_THDU_QUEUE_SHARED_LIMIT_LOSSLESS 52296
#define TH_MMU_THDU_QUEUE_LIMIT_DYNAMIC_LOSSY 1
#define TH_MMU_THDU_QUEUE_LIMIT_DYNAMIC_LOSSLESS 0
#define TH_MMU_THDU_QUEUE_COLOR_LIMIT_DYNAMIC_LOSSY 1
#define TH_MMU_THDU_QUEUE_COLOR_LIMIT_DYNAMIC_LOSSLESS 0
#define TH_MMU_THDU_QUEUE_LIMIT_ENABLE_LOSSY 1
#define TH_MMU_THDU_QUEUE_LIMIT_ENABLE_LOSSLESS 0
#define TH_MMU_NUM_CT_SPEED_GROUPS 12
#define TH_MAX_NUM_CELLS 46 /* (9416-144)/208 + 1 SOP Cell */
#define TH_CT_FIFO_WAIT 15 /*Refer to port_down seq. in TH flexport arch */
#define TH_MMU_PAUSE_PG 7  /*PG7-Used for Initial PG_Min/hdrm + for Pause */

typedef enum pipe_s
{
    TH_IPIPE,
    TH_EPIPE
} pipe_t;

#define TH_MMU_MEM_ADDR__MEMIDX_WIDTH 16
#define TH_MMU_FLUSH_OFF 0
#define TH_MMU_FLUSH_ON  1
#define TH_PKTSTATS_ON   1
#define TH_MMU_THD_RX_ENABLED 1
#define TH_MMU_THD_RX_DISABLED 0

#define TH_MMU_IS_CPU_PORT(pipe, port) ((pipe == 0) && \
                                         (port == TH_MMU_CPU_PORT_NUM))
#define TH_MMU_IS_MGMT_PORT(pipe, \
                             port) ((pipe == 1 || \
                                     pipe == 2) && \
                                    (port == TH_MMU_MGMT_PORT_NUM))
#define TH_MMU_IS_LPBK_PORT(port) (port == TH_MMU_LPBK_PORT_NUM)
#define TH_MMU_GET_PIPE_NUM(port, pipe) \
    pipe = port/TH_MMU_PORT_PIPE_OFFSET
#define TH_MMU_PIPE_NUM(port) port/TH_MMU_PORT_PIPE_OFFSET

#define TH_MMU_HANDLE_ENUMS_EPIPE_DUPLICATE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _PIPE0m, \
      -1}, \
     {mem_name ## _PIPE1m, \
      -1}, {mem_name ## _PIPE2m, -1}, {mem_name ## _PIPE3m, -1}}

#define TH_MMU_HANDLE_ENUMS_IPIPE_DUPLICATE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _PIPE0m, \
      -1}, \
     {mem_name ## _PIPE1m, \
      -1}, {mem_name ## _PIPE2m, -1}, {mem_name ## _PIPE3m, -1}}

#define TH_MMU_HANDLE_ENUMS_EPIPE_UNIQUE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _XPE0_PIPE0m, \
      mem_name ## _XPE2_PIPE0m}, \
     {mem_name ## _XPE0_PIPE1m, \
      mem_name ## _XPE2_PIPE1m}, \
     {mem_name ## _XPE1_PIPE2m, \
      mem_name ## _XPE3_PIPE2m}, \
     {mem_name ## _XPE1_PIPE3m, mem_name ## _XPE3_PIPE3m}}; \

#define TH_MMU_HANDLE_ENUMS_IPIPE_UNIQUE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _XPE0_PIPE0m, \
      mem_name ## _XPE1_PIPE0m}, \
     {mem_name ## _XPE2_PIPE1m, \
      mem_name ## _XPE3_PIPE1m}, \
     {mem_name ## _XPE2_PIPE2m, \
      mem_name ## _XPE3_PIPE2m}, \
     {mem_name ## _XPE0_PIPE3m, mem_name ## _XPE1_PIPE3m}}; \

#define TH_MMU_GET_HANDLE(mem_name, base_index, acc_index, handle) \
    handle = mem_name ## _enums[base_index][acc_index]

/* START - FlexPort types as defined in SDK. */
typedef enum {
    TH_SPEED_10G_ETH_DECODE  = 1,
    TH_SPEED_10G_HG_DECODE   = 2,
    TH_SPEED_20G_ETH_DECODE  = 3,
    TH_SPEED_20G_HG_DECODE   = 4,
    TH_SPEED_25G_ETH_DECODE  = 5,
    TH_SPEED_25G_HG_DECODE   = 6,
    TH_SPEED_40G_ETH_DECODE  = 7,
    TH_SPEED_40G_HG_DECODE   = 8,
    TH_SPEED_50G_ETH_DECODE  = 9,
    TH_SPEED_50G_HG_DECODE   = 10,
    TH_SPEED_100G_ETH_DECODE = 11,
    TH_SPEED_100G_HG_DECODE  = 12
} _soc_th_ct_speed_decode_e;

#endif /* TOMAHAWK_MMU_DEFINES_H */
