/*! \file bcm56960_a0_meter_fp_ing_drv.c
 *
 * This file contains bcm56960_a0 chip specific attributes for IFP Meter.
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
#include <bcmdrd_config.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmmeter/bcmmeter_fp_ing.h>
#include <bcmmeter/bcmmeter_sysm.h>
#include "bcm56960_a0_meter_fp_common.h"

#include "bcm56960_a0_meter_fp_ing_drv.h"

#define BSL_LOG_MODULE          BSL_LS_BCMMETER_FP
/*!
 * \brief Min-Max definitions in byte mode per granularity for BCM56960_A0.
 *
 * Min/Max rate in kbps, min burst size in bits for IFP Meter.
 */
static bcmmeter_fp_gran_info_t bcm56960_a0_meter_fp_ing_gran_info_bytes[BCM56960_A0_METER_FP_ING_MAX_GRANULARITY] = {
    {8, 16777208, 4096},
    {16, 33554416, 8192},
    {32, 67108832, 16384},
    {64, 134217664, 32768},
    {128, 268435328, 65536},
    {256, 536870656, 131072},
    {512, 1073741312, 262144},
    {1024, 2147482624, 524288},
};

/*!
 * \brief Min-Max definitions in packet mode per granularity for BCM56960_A0.
 *
 * Min/Max rate in pps, min burst size in (pps / 1000) for IFP Meter.
 */
static bcmmeter_fp_gran_info_t bcm56960_a0_meter_fp_ing_gran_info_pkts[BCM56960_A0_METER_FP_ING_MAX_GRANULARITY] = {
    /* min rate in pps, max rate in pps, min burst in (packet / 1000) */
    {1, 2097151, 512},
    {2, 4194302, 1024},
    {4, 8388604, 2048},
    {8, 16777208, 4096},
    {16, 33554416, 8192},
    {64, 134217664, 32768},
    {256, 536870656, 131072},
    {1024, 2147482624, 524288},
};

/*! BCM56960_A0 specific attribute definitions for IFP meter. */
bcmmeter_fp_chip_attributes_t bcm56960_a0_meter_fp_ing_attributes= {
    BCM56960_A0_METER_FP_ING_METERS_PER_PIPE,
    BCM56960_A0_METER_FP_ING_NUM_PIPES,
    BCM56960_A0_METER_FP_ING_METERS_PER_POOL,
    BCM56960_A0_METER_FP_ING_NUM_POOLS,
    BCM56960_A0_METER_FP_ING_MAX_GRANULARITY,
    BCM56960_A0_METER_FP_ING_MAX_REFRESH_COUNT,
    BCM56960_A0_METER_FP_ING_MAX_BUCKET_SIZE,
    BCM56960_A0_METER_FP_ING_BUCKET_SIZE_GRAN,
    BCM56960_A0_METER_FP_ING_REFRESH_COUNT_ADJUST,
    BCM56960_A0_METER_FP_ING_POOL_GLOBAL,
    BCM56960_A0_METER_FP_ING_METER_SID,
    BCM56960_A0_METER_FP_ING_BYTEMODE_FID,
    BCM56960_A0_METER_FP_ING_GRAN_FID,
    BCM56960_A0_METER_FP_ING_REFRESHMODE_FID,
    BCM56960_A0_METER_FP_ING_REFRESHCOUNT_FID,
    BCM56960_A0_METER_FP_ING_BUCKETSIZE_FID,
    BCM56960_A0_METER_FP_ING_BUCKETCOUNT_FID,
    BCM56960_A0_METER_FP_ING_REFRESH_CONFIG_SID,
    BCM56960_A0_METER_FP_ING_REFRESH_EN_FID,
    bcm56960_a0_meter_fp_ing_gran_info_bytes,
    bcm56960_a0_meter_fp_ing_gran_info_pkts,
    NULL,
    /*! BCM56960_A0 specific function pointers for FP meter. */
    {
        &bcm56960_a0_meter_fp_ing_init,
        &bcm56960_a0_meter_fp_ing_cleanup,
        &bcm56960_a0_meter_fp_hw_config,
        &bcm56960_a0_meter_fp_convert_to_hw_params,
        &bcm56960_a0_meter_fp_get_oper_value,
        &bcm56960_a0_meter_fp_hw_index_alloc,
        &bcm56960_a0_meter_fp_hw_index_free,
        &bcm56960_a0_meter_fp_entry_write,
        &bcm56960_a0_meter_fp_entry_clear
    },
    0
};

/*!
 * \brief IFP meter init
 *
 * Initialize IFP Meter related data structures
 * for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  cold          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_fp_ing_init (int unit, bool cold)
{
    uint32_t    ha_alloc_size = 0;
    uint32_t    ha_req_size = 0;
    uint32_t    num_sw_meters = 0;

    SHR_FUNC_ENTER(unit);

    /* Meter ID 0 is reserved. */
    num_sw_meters = (BCMMETER_FP_ING_METERS(unit) + 1);

    /* Allocate HA memory for the backup entry. */
    ha_req_size = (num_sw_meters * sizeof(bcmmeter_fp_entry_t));
    ha_alloc_size = ha_req_size;
    ING_METER_TABLE(unit).bkp_list =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         BCMMETER_FP_ING_BKP_SUB_COMP_ID,
                         &ha_alloc_size);
    SHR_NULL_CHECK(ING_METER_TABLE(unit).bkp_list, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (cold) {
        sal_memset(ING_METER_TABLE(unit).bkp_list, 0, ha_alloc_size);
    }
    BCMMETER_FP_ING_BKP_INDEX_USED_SET(unit, BCMMETER_DEFAULT_METER_ID);

    /*
     * Allocate memory for ING meter table.
     */
    SHR_ALLOC(ING_METER_TABLE(unit).list,
              ha_alloc_size,
              "ING Meter SW state");
    SHR_NULL_CHECK(ING_METER_TABLE(unit).list, SHR_E_MEMORY);
    sal_memset(ING_METER_TABLE(unit).list, 0, ha_alloc_size);

    sal_memcpy(ING_METER_TABLE(unit).list,
               ING_METER_TABLE(unit).bkp_list,
               ha_alloc_size);

    /*
     * Allocate HA memory for IFP meter table H/W index usage.
     */
    ha_req_size = (BCMMETER_FP_ING_NUM_PIPES(unit) *
                   BCMMETER_FP_ING_METERS_PER_PIPE(unit) *
                   sizeof(bcmmeter_fp_hw_idx_t));
    ha_alloc_size = ha_req_size;

    ING_METER_TABLE(unit).bkp_hw_idx =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         BCMMETER_FP_ING_HW_IDX_BKP_SUB_COMP_ID,
                         &ha_alloc_size);
    SHR_NULL_CHECK(ING_METER_TABLE(unit).bkp_hw_idx, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (cold) {
        sal_memset(ING_METER_TABLE(unit).bkp_hw_idx, 0, ha_alloc_size);
    }

    /*
     * Allocate memory for IFP meter table H/W index usage.
     */
    SHR_ALLOC(ING_METER_TABLE(unit).hw_idx,
              ha_alloc_size,
              "HW index usage");
    SHR_NULL_CHECK(ING_METER_TABLE(unit).hw_idx, SHR_E_MEMORY);
    sal_memset(ING_METER_TABLE(unit).hw_idx, 0, ha_alloc_size);

    sal_memcpy(ING_METER_TABLE(unit).hw_idx,
               ING_METER_TABLE(unit).bkp_hw_idx,
               ha_alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        if (ING_METER_TABLE(unit).bkp_hw_idx != NULL) {
            shr_ha_mem_free(unit, ING_METER_TABLE(unit).bkp_hw_idx);
        }
        if (ING_METER_TABLE(unit).bkp_list != NULL) {
            shr_ha_mem_free(unit, ING_METER_TABLE(unit).bkp_list);
        }
        if (ING_METER_TABLE(unit).hw_idx != NULL) {
            SHR_FREE(ING_METER_TABLE(unit).hw_idx);
        }
        if (ING_METER_TABLE(unit).list != NULL) {
            SHR_FREE(ING_METER_TABLE(unit).list);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief IFP meter cleanup
 *
 * Cleanup IFP Meter related data structures
 * allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_fp_ing_cleanup (int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ING_METER_TABLE(unit).hw_idx != NULL) {
        SHR_FREE(ING_METER_TABLE(unit).hw_idx);
    }
    if (ING_METER_TABLE(unit).list != NULL) {
        SHR_FREE(ING_METER_TABLE(unit).list);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief IFP meter driver attach
 *
 * Attach IFP Meter driver for this unit
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_fp_ing_attach (int unit)
{
    return bcmmeter_fp_ing_drv_set(unit,
                                   &bcm56960_a0_meter_fp_ing_attributes);
}

/*!
 * \brief IFP meter driver detach
 *
 * Detach IFP Meter driver from this unit
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_fp_ing_detach (int unit)
{
    return bcmmeter_fp_ing_drv_set(unit, NULL);
}
