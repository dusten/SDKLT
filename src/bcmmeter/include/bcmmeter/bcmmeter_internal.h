/*! \file bcmmeter_internal.h
 *
 * This file contains FP Meter Custom handler
 * data structures and macros.
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

#ifndef BCMMETER_INTERNAL_H
#define BCMMETER_INTERNAL_H

#include <shr/shr_bitop.h>

/*! Ingress FP stage. */
#define BCMMETER_FP_STAGE_ID_INGRESS    0
/*! Egress FP stage. */
#define BCMMETER_FP_STAGE_ID_EGRESS     1

/*! Meter LT insert operation. */
#define BCMMETER_FP_LT_INSERT       0
/*! Meter LT update operation. */
#define BCMMETER_FP_LT_UPDATE       1

/*! Default meter id used to detach. */
#define BCMMETER_DEFAULT_METER_ID   0

/*! Meter LT meter mode default. */
#define BCMMETER_FP_MODE_DEFAULT    0
/*! Meter LT meter mode flow. */
#define BCMMETER_FP_MODE_FLOW       1
/*! Meter LT meter mode srtcm. */
#define BCMMETER_FP_MODE_SRTCM      2
/*! Meter LT meter mode modsrtcm. */
#define BCMMETER_FP_MODE_MODSRTCM   3
/*! Meter LT meter mode trtcm. */
#define BCMMETER_FP_MODE_TRTCM      4
/*! Meter LT meter mode modtrtcm. */
#define BCMMETER_FP_MODE_MODTRTCM   5

/*! Meter LT meter mode color blind. */
#define BCMMETER_FP_COLOR_BLIND     0
/*! Meter LT meter mode color aware. */
#define BCMMETER_FP_COLOR_AWARE     1

/*! Meter LT meter byte mode. */
#define BCMMETER_FP_MODE_BYTES      false
/*! Meter LT meter packet mode. */
#define BCMMETER_FP_MODE_PKTS       true

/*! FP Meter number of meters in pairing mode. */
#define BCMMETER_FP_PAIR_NUM        2
/*! FP Meter meter pair odd meter index. */
#define BCMMETER_FP_INDEX_ODD       1
/*! FP Meter meter pair even meter index. */
#define BCMMETER_FP_INDEX_EVEN      0

/*! FP mode global. */
#define BCMMETER_FP_OPER_MODE_GLOBAL        0
/*! FP mode pipe unique. */
#define BCMMETER_FP_OPER_MODE_PIPE_UNIQUE   1

/*! FP Meter meters per pipe on this unit for this FP stage. */
#define BCMMETER_FP_METERS_PER_PIPE(unit, stage)                \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_METERS_PER_PIPE(unit) :                    \
     BCMMETER_FP_EGR_METERS_PER_PIPE(unit))

/*! FP Meter number of pipes on this unit. */
#define BCMMETER_FP_NUM_PIPES(unit, stage)                      \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_NUM_PIPES(unit) :                          \
     BCMMETER_FP_EGR_NUM_PIPES(unit))

/*! FP Meter meters per pool on this unit for this FP stage. */
#define BCMMETER_FP_METERS_PER_POOL(unit, stage)                \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_METERS_PER_POOL(unit) :                    \
     BCMMETER_FP_EGR_METERS_PER_POOL(unit))

/*! FP Meter number of meter pools on this unit for this FP stage. */
#define BCMMETER_FP_NUM_POOLS(unit, stage)                      \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_NUM_POOLS(unit) :                          \
     BCMMETER_FP_EGR_NUM_POOLS(unit))

/*! FP Meter maximum granularity on this unit for this FP stage. */
#define BCMMETER_FP_MAX_GRAN(unit, stage)                       \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_MAX_GRAN(unit) :                           \
     BCMMETER_FP_EGR_MAX_GRAN(unit))

/*! FP Meter maximum refresh count on this unit for this FP stage. */
#define BCMMETER_FP_MAX_REFRESHCOUNT(unit, stage)               \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_MAX_REFRESHCOUNT(unit) :                   \
     BCMMETER_FP_EGR_MAX_REFRESHCOUNT(unit))

/*! FP Meter maximum bucket size on this unit for this FP stage. */
#define BCMMETER_FP_MAX_BUCKETSIZE(unit, stage)                 \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_MAX_BUCKETSIZE(unit) :                     \
     BCMMETER_FP_EGR_MAX_BUCKETSIZE(unit))

/*! FP Meter bucket size granularity on this unit for this FP stage. */
#define BCMMETER_FP_BUCKETSIZE_GRAN(unit, stage)                \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_BUCKETSIZE_GRAN(unit) :                    \
     BCMMETER_FP_EGR_BUCKETSIZE_GRAN(unit))

/*! FP Meter refresh count adjustment flag for this FP stage. */
#define BCMMETER_FP_REFRESHCOUNT_ADJUST(unit, stage)            \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_REFRESHCOUNT_ADJUST(unit) :                \
     BCMMETER_FP_EGR_REFRESHCOUNT_ADJUST(unit))

/*! FP Meter meter pool global status for this FP stage. */
#define BCMMETER_FP_POOL_GLOBAL(unit, stage)                    \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_POOL_GLOBAL(unit) :                        \
     BCMMETER_FP_EGR_POOL_GLOBAL(unit))

/*! FP Meter hardware meter table id. */
#define BCMMETER_FP_METER_SID(unit, stage)                      \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_METER_SID(unit) :                          \
     BCMMETER_FP_EGR_METER_SID(unit))

/*! FP Meter meter byte mode field name. */
#define BCMMETER_FP_BYTEMODE_FID(unit, stage)                   \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_BYTEMODE_FID(unit) :                       \
     BCMMETER_FP_EGR_BYTEMODE_FID(unit))

/*! FP Meter meter granularity field name. */
#define BCMMETER_FP_GRAN_FID(unit, stage)                       \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_GRAN_FID(unit) :                           \
     BCMMETER_FP_EGR_GRAN_FID(unit))

/*! FP Meter meter refresh mode field name. */
#define BCMMETER_FP_REFRESHMODE_FID(unit, stage)                \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_REFRESHMODE_FID(unit) :                    \
     BCMMETER_FP_EGR_REFRESHMODE_FID(unit))

/*! FP Meter meter refresh count field name. */
#define BCMMETER_FP_REFRESHCOUNT_FID(unit, stage)               \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_REFRESHCOUNT_FID(unit) :                   \
     BCMMETER_FP_EGR_REFRESHCOUNT_FID(unit))

/*! FP Meter meter bucket size field name. */
#define BCMMETER_FP_BUCKETSIZE_FID(unit, stage)                 \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_BUCKETSIZE_FID(unit) :                     \
     BCMMETER_FP_EGR_BUCKETSIZE_FID(unit))

/*! FP Meter meter bucket count field name. */
#define BCMMETER_FP_BUCKETCOUNT_FID(unit, stage)                \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_BUCKETCOUNT_FID(unit) :                    \
     BCMMETER_FP_EGR_BUCKETCOUNT_FID(unit))

/*! FP Meter refresh config table id. */
#define BCMMETER_FP_REFRESH_CONFIG_SID(unit, stage)             \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_REFRESH_CONFIG_SID(unit) :                 \
     BCMMETER_FP_EGR_REFRESH_CONFIG_SID(unit))

/*! FP Meter refresh enable field name. */
#define BCMMETER_FP_REFRESH_EN_FID(unit, stage)                 \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_REFRESH_EN_FID(unit) :                     \
     BCMMETER_FP_EGR_REFRESH_EN_FID(unit))

/*! FP Meter meter granularity information in byte mode. */
#define BCMMETER_FP_GRAN_INFO_BYTES(unit, stage)                \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_GRAN_INFO_BYTES(unit) :                    \
     BCMMETER_FP_EGR_GRAN_INFO_BYTES(unit))

/*! FP Meter meter granularity information in packet mode. */
#define BCMMETER_FP_GRAN_INFO_PKTS(unit, stage)                 \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_GRAN_INFO_PKTS(unit) :                     \
     BCMMETER_FP_EGR_GRAN_INFO_PKTS(unit))

/*! FP Meter meter granularity information. */
#define BCMMETER_FP_GRAN_INFO(unit, stage, mode, gran)          \
    ((mode == BCMMETER_FP_MODE_BYTES) ?                         \
     BCMMETER_FP_GRAN_INFO_BYTES(unit, stage)[gran] :           \
     BCMMETER_FP_GRAN_INFO_PKTS(unit, stage)[gran])

/*! FP meter slice control. */
#define BCMMETER_FP_SLICE_CONTROL(unit, stage)                  \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_SLICE_CONTROL(unit):                       \
     BCMMETER_FP_EGR_SLICE_CONTROL(unit))

/*! FP meter function pointers. */
#define BCMMETER_FP_FN_PTRS(unit, stage)                        \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_FN_PTRS(unit) :                            \
     BCMMETER_FP_EGR_FN_PTRS(unit))

/*! Check meter mode and determine if meter is in paired mode. */
#define BCMMETER_FP_PAIRING_CHECK_ASSIGN(meter_mode, pairing)   \
    if (meter_mode > BCMMETER_FP_MODE_FLOW) {                   \
        pairing = true;                                         \
    }

/*! Lookup meter entry from software ingress meter table. */
#define BCMMETER_FP_ING_ENTRY_LOOKUP(unit, meter_id)            \
    (&bcmmeter_fp_ing_sw_state[unit].list[meter_id])

/*! Lookup meter entry from software egress meter table. */
#define BCMMETER_FP_EGR_ENTRY_LOOKUP(unit, meter_id)            \
    (&bcmmeter_fp_egr_sw_state[unit].list[meter_id])

/*! Lookup meter entry from software meter table. */
#define BCMMETER_ENTRY_LOOKUP(unit, stage, meter_id)            \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_ENTRY_LOOKUP(unit, meter_id) :             \
     BCMMETER_FP_EGR_ENTRY_LOOKUP(unit, meter_id))

/*! Get meter entry reference count. */
#define BCMMETER_ENTRY_GET_REF_COUNT(entry)                     \
    (entry->ref_count)
/*! Increment meter entry reference count. */
#define BCMMETER_ENTRY_INC_REF_COUNT(entry)                     \
    entry->ref_count++;
/*! Decrement meter entry reference count. */
#define BCMMETER_ENTRY_DEC_REF_COUNT(entry)                     \
    entry->ref_count--;

/*! Pointer to ingress meter table for this unit. */
#define ING_METER_TABLE(unit)   bcmmeter_fp_ing_sw_state[unit]
/*! Pointer to egress meter table for this unit. */
#define EGR_METER_TABLE(unit)   bcmmeter_fp_egr_sw_state[unit]

/*! Mark ingress software meter id as used. */
#define BCMMETER_FP_ING_BKP_INDEX_USED_SET(unit, meter_idx)     \
    (ING_METER_TABLE(unit).bkp_list[meter_idx].meter_in_use = true);

/*! Mark egress software meter id as used. */
#define BCMMETER_FP_EGR_BKP_INDEX_USED_SET(unit, meter_idx)     \
    (EGR_METER_TABLE(unit).bkp_list[meter_idx].meter_in_use = true);

/*! Mark ingress software meter id as used. */
#define BCMMETER_FP_ING_INDEX_USED_SET(unit, meter_idx)         \
    (ING_METER_TABLE(unit).list[meter_idx].meter_in_use = true);
/*! Get ingress software meter id used status. */
#define BCMMETER_FP_ING_INDEX_USED_GET(unit, meter_idx)         \
    (ING_METER_TABLE(unit).list[meter_idx].meter_in_use)
/*! Mark ingress software meter id as free. */
#define BCMMETER_FP_ING_INDEX_USED_CLR(unit, meter_idx)         \
    (ING_METER_TABLE(unit).list[meter_idx].meter_in_use = false);

/*! Mark egress software meter id as used. */
#define BCMMETER_FP_EGR_INDEX_USED_SET(unit, meter_idx)         \
    (EGR_METER_TABLE(unit).list[meter_idx].meter_in_use = true);
/*! Get egress software meter id used status. */
#define BCMMETER_FP_EGR_INDEX_USED_GET(unit, meter_idx)         \
    (EGR_METER_TABLE(unit).list[meter_idx].meter_in_use)
/*! Mark egress software meter id as free. */
#define BCMMETER_FP_EGR_INDEX_USED_CLR(unit, meter_idx)         \
    (EGR_METER_TABLE(unit).list[meter_idx].meter_in_use = false);

/*! Mark software meter id as used. */
#define BCMMETER_FP_INDEX_USED_SET(unit, stage, meter_idx)      \
    if (stage == BCMMETER_FP_STAGE_ID_INGRESS) {                \
        BCMMETER_FP_ING_INDEX_USED_SET(unit, meter_idx);        \
    } else {                                                    \
        BCMMETER_FP_EGR_INDEX_USED_SET(unit, meter_idx);        \
    }

/*! Get software meter id used status. */
#define BCMMETER_FP_INDEX_USED_GET(unit, stage, meter_idx)      \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                  \
     BCMMETER_FP_ING_INDEX_USED_GET(unit, meter_idx) :          \
     BCMMETER_FP_EGR_INDEX_USED_GET(unit, meter_idx))

/*! Mark software meter id as free. */
#define BCMMETER_FP_INDEX_USED_CLR(unit, stage, meter_idx)      \
    if (stage == BCMMETER_FP_STAGE_ID_INGRESS) {                \
        BCMMETER_FP_ING_INDEX_USED_CLR(unit, meter_idx);        \
    } else {                                                    \
        BCMMETER_FP_EGR_INDEX_USED_CLR(unit, meter_idx);        \
    }

/*! Get global hardware index from ING pipe specific index. */
#define BCMMETER_FP_ING_ACTUAL_HW_INDEX(unit, pipe, idx)        \
    ((BCMMETER_FP_ING_METERS_PER_PIPE(unit) * pipe) + idx)

/*! Get global hardware index from EGR pipe specific index. */
#define BCMMETER_FP_EGR_ACTUAL_HW_INDEX(unit, pipe, idx)        \
    ((BCMMETER_FP_EGR_METERS_PER_PIPE(unit) * pipe) + idx)

/*! Mark ingress hardware meter id as used. */
#define BCMMETER_FP_ING_HW_INDEX_USED_SET(unit, pipe, idx)              \
    (ING_METER_TABLE(unit).hw_idx                                       \
     [BCMMETER_FP_ING_ACTUAL_HW_INDEX(unit, pipe, idx)].in_use = true);

/*! Get ingress hardware meter id used status. */
#define BCMMETER_FP_ING_HW_INDEX_USED_GET(unit, pipe, idx)              \
    (ING_METER_TABLE(unit).hw_idx                                       \
     [BCMMETER_FP_ING_ACTUAL_HW_INDEX(unit, pipe, idx)].in_use)

/*! Mark ingress hardware meter id as free. */
#define BCMMETER_FP_ING_HW_INDEX_USED_CLR(unit, pipe, idx)              \
    (ING_METER_TABLE(unit).hw_idx                                       \
     [BCMMETER_FP_ING_ACTUAL_HW_INDEX(unit, pipe, idx)].in_use = false);

/*! Mark egress hardware meter id as used. */
#define BCMMETER_FP_EGR_HW_INDEX_USED_SET(unit, pipe, idx)              \
    (EGR_METER_TABLE(unit).hw_idx                                       \
     [BCMMETER_FP_EGR_ACTUAL_HW_INDEX(unit, pipe, idx)].in_use = true);

/*! Get egress hardware meter id used status. */
#define BCMMETER_FP_EGR_HW_INDEX_USED_GET(unit, pipe, idx)              \
    (EGR_METER_TABLE(unit).hw_idx                                       \
     [BCMMETER_FP_EGR_ACTUAL_HW_INDEX(unit, pipe, idx)].in_use)

/*! Mark egress hardware meter id as free. */
#define BCMMETER_FP_EGR_HW_INDEX_USED_CLR(unit, pipe, idx)              \
    (EGR_METER_TABLE(unit).hw_idx                                       \
     [BCMMETER_FP_EGR_ACTUAL_HW_INDEX(unit, pipe, idx)].in_use = false);

/*! Mark hardware meter id as used. */
#define BCMMETER_FP_HW_INDEX_USED_SET(unit, stage, pipe, idx)           \
    if (stage == BCMMETER_FP_STAGE_ID_INGRESS) {                        \
        BCMMETER_FP_ING_HW_INDEX_USED_SET(unit, pipe, idx);             \
    } else {                                                            \
        BCMMETER_FP_EGR_HW_INDEX_USED_SET(unit, pipe, idx);             \
    }

/*! Get hardware meter id used status. */
#define BCMMETER_FP_HW_INDEX_USED_GET(unit, stage, pipe, idx)           \
    ((stage == BCMMETER_FP_STAGE_ID_INGRESS) ?                          \
     BCMMETER_FP_ING_HW_INDEX_USED_GET(unit, pipe, idx) :               \
     BCMMETER_FP_EGR_HW_INDEX_USED_GET(unit, pipe, idx))

/*! Mark hardware meter id as free. */
#define BCMMETER_FP_HW_INDEX_USED_CLR(unit, stage, pipe, idx)           \
    if (stage == BCMMETER_FP_STAGE_ID_INGRESS) {                        \
        BCMMETER_FP_ING_HW_INDEX_USED_CLR(unit, pipe, idx);             \
    } else {                                                            \
        BCMMETER_FP_EGR_HW_INDEX_USED_CLR(unit, pipe, idx);             \
    }

/*!
 * \brief Calculate meter rate.
 *
 * Calculate meter rate from refresh count and granularity.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] mode Byte mode.
 * \param [in] gran Meter granularity.
 * \param [in] rc Refresh count.
 */
#define BCMMETER_COMPUTE_METER_RATE(unit, stage, mode, gran, rc)        \
    (BCMMETER_FP_GRAN_INFO(unit, stage, mode, gran).min_rate * rc)

/*!
 * \brief Calculate burst size.
 *
 * Calculate burst size from bucket size and granularity.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] mode Byte mode.
 * \param [in] gran Meter granularity.
 * \param [in] bktsz Bucket size.
 */
#define BCMMETER_COMPUTE_BURST_SIZE(unit, stage, mode, gran, bktsz)     \
    ((BCMMETER_FP_GRAN_INFO(unit, stage, mode, gran).min_burst * bktsz) / 1000)

/*! Get the default value of an LT field. */
#define BCMMETER_FP_FIELD_DEF(dtag)   ((dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                       (field_def.def.u8) :                     \
                                       ((dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                        (field_def.def.u16) :                   \
                                        ((dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                         (field_def.def.u32) :                  \
                                         ((dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                          (field_def.def.u64) :                 \
                                          (field_def.def.is_true)))))

/*! Get the minimum supported value of an LT field. */
#define BCMMETER_FP_FIELD_MIN(dtag)   ((dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                       (field_def.min.u8) :                     \
                                       ((dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                        (field_def.min.u16) :                   \
                                        ((dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                         (field_def.min.u32) :                  \
                                         ((dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                          (field_def.min.u64) :                 \
                                          (field_def.min.is_true)))))

/*! Get the maximum supported value of an LT field. */
#define BCMMETER_FP_FIELD_MAX(dtag)   ((dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                       (field_def.max.u8) :                     \
                                       ((dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                        (field_def.max.u16) :                   \
                                        ((dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                         (field_def.max.u32) :                  \
                                         ((dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                          (field_def.max.u64) :                 \
                                          (field_def.max.is_true)))))

/*!
 * \brief FP Meter LT fields structure.
 *
 * LT parameters of FP meters are stored in this structure.
 */
typedef struct bcmmeter_sw_params_s {
    /*! Software meter id. */
    uint16_t    meter_id;
    /*! Pipe number. */
    uint8_t     pipe;
    /*! Meter pool id. */
    uint8_t     meter_pool_id;
    /*! Meter mode, byte or packet. */
    uint8_t     byte_mode;
    /*! Meter operating mode. */
    uint8_t     meter_mode;
    /*! Meter color mode. */
    uint8_t     color_mode;
    /*! Committed meter rate. */
    uint32_t    min_rate_kbps;
    /*! Committed burst size. */
    uint32_t    min_burst_kbits;
    /*! Excess meter rate. */
    uint32_t    max_rate_kbps;
    /*! Excess burst size. */
    uint32_t    max_burst_kbits;
    /*! Committed meter rate. */
    uint32_t    min_rate_pps;
    /*! Committed burst size. */
    uint32_t    min_burst_pkts;
    /*! Excess meter rate. */
    uint32_t    max_rate_pps;
    /*! Excess burst size. */
    uint32_t    max_burst_pkts;
    /*! Committed meter burst size. */
    uint32_t    min_burst_oper;
    /*! Excess meter burst size. */
    uint32_t    max_burst_oper;
    /*! Committed meter operational rate. */
    uint32_t    min_rate_oper;
    /*! Excess meter operational rate. */
    uint32_t    max_rate_oper;
} bcmmeter_sw_params_t;

/*!
 * \brief FP Meter hardware parameters structure.
 *
 * LT fields of FP meters are converted to hardware table fields
 * and stored in this structure.
 */
typedef struct bcmmeter_hw_params_s {
    /*! Meter pair index. */
    uint32_t    meter_pair_index;
    /*! Meter hw index. */
    uint32_t    meter_hw_index;
    /*! Meter pool id. */
    uint8_t     pool;
    /*! Meter mode. */
    uint8_t     meter_pair_mode;
    /*! Meter mode modifier. */
    uint8_t     mode_modifier;
    /*! Meter update odd. */
    uint8_t     meter_update_odd;
    /*! Meter test odd. */
    uint8_t     meter_test_odd;
    /*! Meter update even. */
    uint8_t     meter_update_even;
    /*! Meter test even. */
    uint8_t     meter_test_even;

    /*! Meter mode, bytes or packets. */
    uint8_t     byte_mode;
    /*! Meter refresh mode. */
    uint8_t     refresh_mode;
    /*! Meter granularity. */
    uint8_t     gran[BCMMETER_FP_PAIR_NUM];
    /*! Meter refresh count. */
    uint32_t    refresh_count[BCMMETER_FP_PAIR_NUM];
    /*! Meter bucket size. */
    uint32_t    bucket_size[BCMMETER_FP_PAIR_NUM];
    /*! Meter bucket count. */
    uint32_t    bucket_count[BCMMETER_FP_PAIR_NUM];
} bcmmeter_hw_params_t;

/*!
 * \brief FP Meter meter entry structure.
 *
 * Both LT/Software fields and hardware fields of each FP meter
 * are stored in this structure.
 */
typedef struct bcmmeter_fp_entry_s {
    /*! Meter in use. */
    bool        meter_in_use;
    /*! FP meter Software/LT fields. */
    bcmmeter_sw_params_t fields;
    /*! FP meter Hardware fields. */
    bcmmeter_hw_params_t hw_fields;
    /*! FP meter reference count. */
    uint32_t ref_count;
} bcmmeter_fp_entry_t;

/*!
 * \brief FP per pipe structure.
 */
typedef struct bcmmeter_fp_hw_idx_s {
    /*! Hw meter index in use status. */
    bool    in_use;
} bcmmeter_fp_hw_idx_t;

/*!
 * \brief FP Meter meter table structure.
 *
 * This structure holds the software and hardware fields of
 * all configured FP meters along with bitmap of meter ids that
 * are used.
 */
typedef struct bcmmeter_fp_sw_state_s {
    /*! Structure to mark in-use status of hardware meter ids. */
    bcmmeter_fp_hw_idx_t    *hw_idx;

    /*! List of meter entries configured. */
    bcmmeter_fp_entry_t     *list;

    /*! Backup structure to mark in-use status of hardware meter ids. */
    bcmmeter_fp_hw_idx_t    *bkp_hw_idx;

    /*! Backup list. */
    bcmmeter_fp_entry_t     *bkp_list;
} bcmmeter_fp_sw_state_t;

/*!
 * \brief FP Meter meter granularity information structure.
 *
 * Maintains min and max meter rate information at each granularity
 * level.
 */
typedef struct bcmmeter_fp_gran_info_s {
    /*! Minimum meter rate. */
    uint32_t    min_rate;
    /*! Maximum meter rate. */
    uint32_t    max_rate;
    /*! Minimum burst size. */
    uint32_t    min_burst;
} bcmmeter_fp_gran_info_t;

/*!FP meter driver init. */
typedef int (*bcmmeter_fp_drv_init_f)(int unit, bool cold);

/*!FP meter driver cleanup. */
typedef int (*bcmmeter_fp_drv_cleanup_f)(int unit);

/*! FP meter convert to HW params. */
typedef int (*bcmmeter_fp_compute_hw_param_f)(int unit,
                                              int stage,
                                              bcmmeter_fp_entry_t *meter_entry);
/*! FP meter get HW oper values. */
typedef int (*bcmmeter_fp_get_oper_param_f)(int unit,
                                            int stage,
                                            bcmmeter_fp_entry_t *meter_entry);
/*! FP meter alloc HW index. */
typedef int (*bcmmeter_fp_index_alloc_f)(int unit,
                                         int stage,
                                         int pool,
                                         bcmmeter_fp_entry_t *meter_entry,
                                         uint32_t *index);
/*! FP meter free HW index. */
typedef int (*bcmmeter_fp_index_free_f)(int unit,
                                        int stage,
                                        uint32_t index,
                                        bcmmeter_fp_entry_t *meter_entry);
/*! FP meter write HW entry. */
typedef int (*bcmmeter_fp_entry_write_f)(int unit,
                                         uint32_t trans_id,
                                         int stage,
                                         bcmmeter_fp_entry_t *meter_entry);
/*! FP meter clear HW entry. */
typedef int (*bcmmeter_fp_entry_clear_f)(int unit,
                                         uint32_t trans_id,
                                         int stage,
                                         bcmmeter_fp_entry_t *meter_entry);
/*! FP meter device config. */
typedef int (*bcmmeter_fp_hw_config_f)(int unit,
                                       int stage,
                                       uint32_t refresh_en);

/*!
 * \brief FP meter function pointers structure.
 */
typedef struct bcmmeter_fp_fn_ptrs_s {
    /*!FP meter drv init. */
    bcmmeter_fp_drv_init_f            init;
    /*!FP meter drv cleanup. */
    bcmmeter_fp_drv_cleanup_f         cleanup;
    /*!FP meter device config. */
    bcmmeter_fp_hw_config_f           hw_config;
    /*! FP meter convert to HW params. */
    bcmmeter_fp_compute_hw_param_f    hw_param_compute;
    /*! FP meter get actual oper rate. */
    bcmmeter_fp_get_oper_param_f      hw_oper_get;
    /*! FP meter alloc HW index. */
    bcmmeter_fp_index_alloc_f         hw_idx_alloc;
    /*! FP meter free HW index. */
    bcmmeter_fp_index_free_f          hw_idx_free;
    /*! FP meter HW write. */
    bcmmeter_fp_entry_write_f         hw_write;
    /*! FP meter HW read. */
    bcmmeter_fp_entry_clear_f         hw_clear;
} bcmmeter_fp_fn_ptrs_t;

/*!
 * \brief Enable FP slice for h/w access.
 */
typedef struct bcmmeter_fp_slice_ctrl_s {
    /*! Per-slice control register id. */
    uint32_t    sid;
    /*! Number of fields. */
    uint32_t    fid_count;
    /*! Array of fids. */
    uint32_t    *fids;
} bcmmeter_fp_slice_ctrl_t;

/*!
 * \brief FP Meter chip specific attributes structure.
 *
 * Maintains information about chip specific FP meter attributes.
 */
typedef struct bcmmeter_fp_chip_attributes_s {
    /*! Number of meters per pipe. */
    uint32_t    num_meters_per_pipe;
    /*! Number of pipes. */
    uint32_t    num_pipes;
    /*! Number of meters per pool. */
    uint32_t    num_meters_per_pool;
    /*! Number of pools.*/
    uint32_t    num_pools;
    /*! Maximum granularity. */
    uint32_t    max_granularity;
    /*! Maximum refresh count. */
    uint32_t    max_refresh_cnt;
    /*! Maximum bucket size. */
    uint32_t    max_bucket_size;
    /*! Bucket size granularity in tokens. */
    uint32_t    bucket_size_gran;
    /*! Refresh count adjustment required. */
    bool        refreshcount_adjust;
    /*! Meter pool global status. */
    bool        meter_pool_global;
    /*! Hardware meter table id. */
    uint32_t    meter_sid;
    /*! Hardware meter byte mode field name. */
    uint32_t    byte_mode_fid;
    /*! Hardware meter granularity field name. */
    uint32_t    meter_gran_fid;
    /*! Hardware meter refreh mode field name. */
    uint32_t    refresh_mode_fid;
    /*! Hardware meter refreh count field name. */
    uint32_t    refresh_count_fid;
    /*! Hardware meter bucket size field name. */
    uint32_t    bucket_size_fid;
    /*! Hardware meter bucket count field name. */
    uint32_t    bucket_count_fid;
    /*! Meter refresh enable configuration regsiter. */
    uint32_t    refresh_cfg_sid;
    /*! Meter refresh enable field name. */
    uint32_t    refresh_en_fid;
    /*! Pointer to meter granularity information structure in byte mode. */
    bcmmeter_fp_gran_info_t   *gran_info_bytes;
    /*! Pointer to meter granularity information structure in packet mode. */
    bcmmeter_fp_gran_info_t   *gran_info_pkts;
    /*! FP meter slice control. */
    bcmmeter_fp_slice_ctrl_t  *slice_ctrl;
    /*! FP meter function pointers. */
    bcmmeter_fp_fn_ptrs_t      fn_ptrs;
    /*! End marker. */
    uint32_t    zero;
} bcmmeter_fp_chip_attributes_t;

/*!
 * \brief Insert meter into hardware table.
 *
 * Find a free index and write this meter into hardware table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] pool Meter pool id.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 * \retval SHR_E_FULL Hardware meter table full.
 * \retval SHR_E_PARAM Invalid meter param.
 */
int
bcmmeter_entry_insert (int unit,
                       uint32_t trans_id,
                       int stage,
                       int pool,
                       uint32_t meter_id);

/*!
 * \brief Lookup an FP entry.
 *
 * Lookup the meter entry from Software meter table
 * and retrieve meter parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 * \param [out] meter_entry Pointer to meter entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_lookup (int unit,
                       int stage,
                       uint32_t meter_id,
                       bcmmeter_fp_entry_t **meter_entry);

/*!
 * \brief Delete a meter entry.
 *
 * Delete the meter entry from hardware if ref count is zero and free the
 * hardware index.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_delete (int unit,
                       uint32_t trans_id,
                       int stage,
                       uint32_t meter_id);

/*!
 * \brief Detach an FP entry from the meter.
 *
 * Decrement the ref count of this meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_detach (int unit,
                       int stage,
                       uint32_t meter_id);

/*!
 * \brief Attach an FP entry to the meter.
 *
 * Increment the ref count of this meter entry to keep track of
 * number of FP entries pointing to this meter.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_attach (int unit,
                       int stage,
                       uint32_t meter_id);

/*!
 * \brief Move a meter entry from one pool to another.
 *
 * If ref count is zero, move the meter entry to the new pool.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] pool Meter pool id.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 * \retval SHR_E_BUSY Meter ref count is not zero.
 */
int
bcmmeter_entry_move (int unit,
                     uint32_t trans_id,
                     int stage,
                     int pool,
                     uint32_t meter_id);

/*!
 * \brief Update Meter entry
 *
 * Update meter entry in H/W table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmmeter_entry_update (int unit,
                       uint32_t trans_id,
                       int stage,
                       uint32_t meter_id);

/*!
 * \brief Validate meter id.
 *
 * Check if the meter entry is present in s/w table.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software Meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Meter entry not present in s/w table.
 */
int
bcmmeter_entry_valid (int unit,
                      int stage,
                      uint32_t meter_id);

/*!
 * \brief Return global status of meter pool.
 *
 * Check the chip specific attributes and return whether the meter pool
 * is global or not.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] global Global pool status of meters in this FP stage.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmmeter_pool_global (int unit,
                      int stage,
                      bool *global);

/*!
 * \brief Get meter field value.
 *
 * Parse user input and get the field corresponding to
 * a particular fid.
 *
 * \param [in] unit Unit number.
 * \param [in] field_count Number of fields.
 * \param [in] flist Pointer to input LT field list.
 * \param [in] fid Lt field id
 * \param [out] rval Pointer to return field value
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_field_value_get(int unit,
                               int field_count,
                               bcmltd_field_t **flist,
                               uint32_t fid,
                               uint32_t *rval);

/*!
 * \brief Initialize meter entry.
 *
 * Initialize meter entry with default values from map file.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table identifier.
 * \param [out] out Pointer to output param structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_init_params (int unit,
                            bcmlrd_sid_t sid,
                            bcmmeter_sw_params_t *out);

/*!
 * \brief Get meter config parameters.
 *
 * Parse user input and retrieve Meter LT params.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] field_count Number of fields.
 * \param [in] flist Pointer to input LT field list.
 * \param [out] out Pointer to output param structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_parse_params (int unit,
                             int stage,
                             int field_count,
                             bcmltd_field_t **flist,
                             bcmmeter_sw_params_t *out);

/*!
 * \brief Return meter config parameters.
 *
 * Read software meter table and return meter entry
 * parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table identifier.
 * \param [in] meter_entry Pointer to meter entry.
 * \param [out] flist Pointer to output fieldds list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmmeter_entry_populate_params (int unit,
                                bcmlrd_sid_t sid,
                                bcmmeter_fp_entry_t *meter_entry,
                                bcmltd_field_t *flist);

/*!
 * \brief Configure FP meter related device info.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmmeter_fp_dev_config (int unit, int stage, uint32_t refresh_en);


/*!
 * \brief Get FP Pipe mode cofnigured.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] mode Operational mode.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmmeter_fp_oper_mode_get (int unit,
                           int stage,
                           uint8_t *mode);

/*!
 * \brief Initialize FP meter s/w resources.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] cold Warm/Coldboot mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL if handler for init is NULL.
 */
int
bcmmeter_fp_sw_state_init (int unit,
                           int stage,
                           bool cold);

/*!
 * \brief Cleanup FP meter s/w resources.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL if handler for cleanup is NULL.
 */
int
bcmmeter_fp_sw_state_cleanup (int unit,
                              int stage);

#endif /* BCMMETER_INTERNAL_H */
