/*! \file bcm56960_a0_meter_sc_drv.c
 *
 * This file contains bcm56960_a0 chip specific attributes for
 * Storm Control Meter.
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
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmmeter/bcmmeter_sc.h>
#include <bcmmeter/bcmmeter_sysm.h>

#include "bcm56960_a0_meter_sc_drv.h"

#define BSL_LOG_MODULE          BSL_LS_BCMMETER_SC

/*! Convert kilobytes to bytes. */
#define KBYTES          1024
/*! Convert megabytes to bytes. */
#define MBYTES          (1024 * 1024)
/*! Number of bits in a byte. */
#define BITSINBYTE      8
/*! Kilobits in API. */
#define KBITS_IN_API    1000

/*!
 * \brief Token-wise bucket size definition for BCM56960_A0.
 *
 * Each bucket size translates to a specific bucket count.
 *
 * 8 options for Bucket Size.
 * In packet mode, each packet is PQ tokens
 * 0 - 64K tokens
 * 1 - 256K tokens
 * 2 - 1M tokens
 * 3 - 4M tokens
 * 4 - 16M tokens
 * 5 - 64M tokens
 * 6 - 128M tokens
 * 7 - 256M tokens.
 */
static uint32_t
bcm56960_a0_bkt_sz_to_token_map[BCM56960_A0_METER_SC_MAX_BUCKET_SZ] = {
    (64  * KBYTES),
    (256 * KBYTES),
    (1   * MBYTES),
    (4   * MBYTES),
    (16  * MBYTES),
    (64  * MBYTES),
    (128 * MBYTES),
    (256 * MBYTES)
};

/*!
 * \brief Kbit-wise bucket size definition for BCM56960_A0.
 *
 * Each bucket size translates to a specific bucket count.
 *
 * In byte mode, each token is 0.5 bit
 * So the available bucket sizes translate to
 * 0 - 4KB ([64K * token_size / bits_in_bytes] => [64K * 0.5 / 8])
 * 1 - 16KB
 * 2 - 64KB
 * 3 - 256KB
 * 4 - 1MB
 * 5 - 4MB
 * 6 - 8MB
 * 7 - 16MB.
 */
static uint32_t
bcm56960_a0_bkt_sz_to_kbit_map[BCM56960_A0_METER_SC_MAX_BUCKET_SZ] = {
    ((4   * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((16  * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((64  * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((256 * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((1   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((4   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((8   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((16  * MBYTES * BITSINBYTE) / KBITS_IN_API)
};

/*!
 * \brief Min-Max definitions per quantum level for BCM56960_A0.
 *
 * Quantum value, min and max meter rates for each Quantum level.
 * Quantum in tokens, min rate in pps, max rate in million pps.
 */
static bcmmeter_sc_pkt_quantum_info_t
bcm56960_a0_pkt_quantum_info[BCM56960_A0_METER_SC_PKT_QUANTUM_LEVELS] = {
    {1, 128000, 268435},
    {2, 64000, 134217},
    {4, 32000, 67108},
    {8, 16000, 33554},
    {16, 8000, 16777},
    {32, 4000, 8388},
    {64, 2000, 4194},
    {128, 1000, 2097},
    {256, 500, 1048},
    {512, 250, 524},
    {1024, 125, 262},
    {2000, 64, 134},
    {4000, 32, 67},
    {8000, 16, 33},
    {16000, 8, 16},
    {32000, 4, 8},
    {64000, 2, 4},
    {128000, 1, 2}
};

/*! BCM56960_A0 specific attribute definitions for Storm Control meter. */
bcmmeter_sc_chip_attributes_t bcm56960_a0_meter_sc_attributes = {
    BCM56960_A0_METER_SC_MAX_PORTS,
    BCM56960_A0_METER_SC_METERS_PER_PORT,
    BCM56960_A0_METER_SC_PKT_QUANTUM_LEVELS,
    BCM56960_A0_METER_SC_METER_RATE_KBIT_MIN,
    BCM56960_A0_METER_SC_MAX_BUCKET_SZ,
    BCM56960_A0_METER_SC_MAX_REFRESH_COUNT,
    BCM56960_A0_METER_SC_NUM_REFRESH_PER_SEC,
    BCM56960_A0_METER_SC_MIN_TOKENS_PER_REFRESH,
    BCM56960_A0_METER_SC_CONFIG_SID,
    BCM56960_A0_METER_SC_METER_SID,
    BCM56960_A0_METER_SC_BYTEMODE_FID,
    BCM56960_A0_METER_SC_PKTQUANTUM_FID,
    BCM56960_A0_METER_SC_REFRESHCOUNT_FID,
    BCM56960_A0_METER_SC_BUCKETSIZE_FID,
    BCM56960_A0_METER_SC_BUCKETCOUNT_FID,
    bcm56960_a0_bkt_sz_to_kbit_map,
    bcm56960_a0_bkt_sz_to_token_map,
    bcm56960_a0_pkt_quantum_info,
    /*! BCM56960_A0 specific function pointers for storm control meter. */
    {
        &bcm56960_a0_meter_sc_init,
        &bcm56960_a0_meter_sc_cleanup,
        &bcm56960_a0_meter_sc_insert,
        &bcm56960_a0_meter_sc_lookup,
        &bcm56960_a0_meter_sc_delete,
        &bcm56960_a0_meter_sc_bytemode_get
    },
    0
};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Read METER_STORM_CONTROL_CONFIG register.
 *
 * Read METER_STORM_CONTROL_CONFIG register.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] port_id Port number.
 * \param [out] buf Data read from hardware.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware read operation unsuccessful.
 */
static int
bcm56960_a0_meter_sc_cfg_read (int unit,
                               uint32_t trans_id,
                               uint32_t port_id,
                               uint8_t *buf)
{
    uint32_t        entry_sz = 0, flags = 0;
    uint32_t        rsp_flags;
    bcmltd_sid_t    rsp_ltid;
    bcmdrd_sid_t    sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    sid = BCMMETER_SC_CONFIG_TABLE_SID(unit);
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = port_id;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    0,
                                    entry_sz,
                                    METER_STORM_CONTROL_ENABLEt,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)buf,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write to METER_STORM_CONTROL_CONFIG register.
 *
 * Write to METER_STORM_CONTROL_CONFIG register.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] port_id Port number.
 * \param [in] buf Pointer to hardware table write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56960_a0_meter_sc_cfg_write (int unit,
                                uint32_t trans_id,
                                uint32_t port_id,
                                uint8_t *buf)
{
    uint32_t        flags = 0;
    uint32_t        rsp_flags;
    bcmltd_sid_t    rsp_ltid;
    bcmdrd_sid_t    sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    sid = BCMMETER_SC_CONFIG_TABLE_SID(unit);
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = port_id;

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)buf,
                                    0,
                                    METER_STORM_CONTROL_ENABLEt,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read METER_STORM_CONTROL.
 *
 * Read from METER_STORM_CONTROL table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] port_id Port number.
 * \param [in] meter_id Per-port Meter ID.
 * \param [out] buf Data read from the hardware.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware read operation unsuccessful.
 */
static int
bcm56960_a0_meter_sc_entry_read (int unit,
                                 uint32_t trans_id,
                                 uint32_t port_id,
                                 int meter_id,
                                 uint8_t *buf)
{
    uint32_t        entry_sz = 0, flags = 0;
    uint32_t        rsp_flags;
    bcmltd_sid_t    rsp_ltid;
    bcmdrd_sid_t    sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    sid = BCMMETER_SC_TABLE_SID(unit);
    pt_dyn_info.index = BCMMETER_SC_GET_METER_IDX(unit, port_id, meter_id);
    pt_dyn_info.tbl_inst = 0;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    0,
                                    entry_sz,
                                    METER_STORM_CONTROLt,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)buf,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write to METER_STORM_CONTROL.
 *
 * Write to METER_STORM_CONTROL table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] port_id Port number.
 * \param [in] meter_id Per-port Meter ID.
 * \param [in] buf Pointer to the data to be written in hardware.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56960_a0_meter_sc_entry_write (int unit,
                                  uint32_t trans_id,
                                  uint32_t port_id,
                                  int meter_id,
                                  uint8_t *buf)
{
    uint32_t        flags = 0;
    uint32_t        final_meter_idx;
    uint32_t        rsp_flags;
    bcmltd_sid_t    rsp_ltid;
    bcmdrd_sid_t    sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    sid = BCMMETER_SC_TABLE_SID(unit);
    final_meter_idx = BCMMETER_SC_GET_METER_IDX(unit, port_id, meter_id);
    pt_dyn_info.index = final_meter_idx;
    pt_dyn_info.tbl_inst = 0;

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)buf,
                                    0,
                                    METER_STORM_CONTROLt,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recalculate hw params.
 *
 * Re-calculate bucket size and refresh count for the other meters
 * of the port to achieve the configured burst size with the new
 * packet quantum.
 *
 * \param [in] unit Unit number.
 * \param [in] meter_idx Per-port Meter ID that is being modified.
 * \param [in] quantum Packet quantum.
 * \param [in] meter_rate Pointer to metering rate array for this port.
 * \param [in] burst_size Pointer to burst size array for this port.
 * \param [out] hw_param Meter config converted to h/w parameters.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56960_a0_meter_sc_recalc_hw_param (int unit,
                                      uint32_t meter_idx,
                                      uint32_t quantum,
                                      uint64_t *meter_rate,
                                      uint32_t *burst_size,
                                      bcmmeter_sc_hw_params_t *hw_param)
{
    uint32_t    i, j;
    uint32_t    burst_pkts = 0;

    SHR_FUNC_ENTER(unit);

    for (j = 0; j < BCMMETER_SC_NUM_METERS_PER_PORT(unit); j++) {
        /*
         * Recalculate refresh count for the other 3 meters of this port
         * Check if the refresh counts are valid
         */
        if (meter_rate[j] != 0 && j != meter_idx) {
            hw_param[j].refresh_cnt =
                BCMMETER_SC_COMPUTE_PKT_REFRESH_COUNT(unit,
                                                      meter_rate[j],
                                                      quantum);
            if (hw_param[j].refresh_cnt == 0) {
                hw_param[j].refresh_cnt = 1;
            } else if (hw_param[j].refresh_cnt >
                       BCMMETER_SC_MAX_REFRESH_COUNT(unit)) {
                hw_param[j].refresh_cnt = BCMMETER_SC_MAX_REFRESH_COUNT(unit);
            }
        }

        /*
         * Recalculate bucket size for the other 3 meters of this port
         * Check if the bucket sizes are valid
         */
        if (burst_size[j] != 0 && j != meter_idx) {
            /*
             * Iterate over all bucket sizes to find the one
             * which gives the required burst size
             */
            for (i = 0; i < BCMMETER_SC_MAX_BUCKET_SZ(unit); i++) {
                burst_pkts =
                    BCMMETER_SC_GET_BURST_SZ_PACKETS_FROM_BKT_SZ(unit,
                                                                 i,
                                                                 quantum);
                if (burst_pkts >= burst_size[j]) {
                    hw_param[j].bkt_sz = i;
                    break;
                }
            }
            if (i == BCMMETER_SC_MAX_BUCKET_SZ(unit)) {
                hw_param[j].bkt_sz = (i - 1);
            }
            hw_param[j].bkt_cnt = BCMMETER_SC_BKT_SZ_TO_TOKEN_MAP(unit)[i];
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Compute packet quantum.
 *
 * Given metering rate, determine packet quantum.
 *
 * \param [in] unit Unit number.
 * \param [in] meter_rate Metering rate.
 * \param [out] quantum Packet quantum
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Valid value for packet quantum could not be found.
 */
static int
bcm56960_a0_meter_sc_compute_pkt_quantum (int unit,
                                          uint64_t meter_rate,
                                          uint32_t *quantum)
{
    int i = 0;
    uint64_t max_rate = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Iterate over different quantum values (factor of 8000)
     */
    for (i = (BCMMETER_SC_PKT_QUANTUM_LEVELS(unit) - 1); i >= 0; i--) {
        max_rate = BCMMETER_SC_QUANTUM_MAX_RATE(unit, i);
        if (max_rate >= meter_rate) {
            break;
        }
    }

    if (i < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *quantum = BCMMETER_SC_QUANTUM_VALUE(unit, i);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compute refresh count, bucket size and quantum.
 *
 * Given metering rate, determine packet quantum and refresh count.
 *
 * \param [in] unit Unit number.
 * \param [in] byte_mode Meter mode, packet or bytes.
 * \param [in] old_quantum Current packet quantum of this port.
 * \param [in] meter_rate Metering rate.
 * \param [in] burst_size Required burst size.
 * \param [out] quantum Packet quantum value.
 * \param [out] hw_param Meter config converted to h/w parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Valid value for packet quantum could not be found.
 */
static int
bcm56960_a0_meter_sc_compute_hw_param (int unit,
                                       int byte_mode,
                                       uint32_t old_quantum,
                                       uint64_t meter_rate,
                                       uint32_t burst_size,
                                       uint32_t *quantum,
                                       bcmmeter_sc_hw_params_t *hw_param)
{
    uint32_t *map_ptr;
    uint32_t burst_val = 0;
    uint32_t i;
    uint64_t max_rate = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Compute refresh count.
     */
    if (byte_mode) {
        hw_param->refresh_cnt =
            BCMMETER_SC_COMPUTE_BYTE_REFRESH_COUNT(unit,
                                                   meter_rate);
    } else {
        /* Check if existing quantum value can be used */
        if (old_quantum) {
            max_rate =
                BCMMETER_SC_COMPUTE_PKT_METER_RATE(
                    unit,
                    BCMMETER_SC_MAX_REFRESH_COUNT(unit),
                    old_quantum);
        }

        if (max_rate > meter_rate) {
            *quantum = old_quantum;
        } else {
            /*
             * If old quantum cannot be used or if it is zero,
             * calculate quantum value
             */
            SHR_IF_ERR_EXIT
                (bcm56960_a0_meter_sc_compute_pkt_quantum(unit,
                                                          meter_rate,
                                                          quantum));
        }
        /*
         * Once quantum is determined, calculate refresh count
         */
        hw_param->refresh_cnt =
            BCMMETER_SC_COMPUTE_PKT_REFRESH_COUNT(unit,
                                                  meter_rate,
                                                  *quantum);
    }


    /*
     * Compute bucket size and bucket count.
     */
    if (byte_mode) {
        map_ptr = BCMMETER_SC_BKT_SZ_TO_KBIT_MAP(unit);
        burst_val = burst_size;
    } else {
        map_ptr = BCMMETER_SC_BKT_SZ_TO_TOKEN_MAP(unit);
        burst_val = (burst_size * (*quantum));
    }

    /*
     * Find the bucket size that can accommodate this burst size
     */
    for (i = 0; i < BCMMETER_SC_MAX_BUCKET_SZ(unit); i++) {
        if (map_ptr[i] >= burst_val) {
            hw_param->bkt_sz = i;
            break;
        }
    }

    if (i == BCMMETER_SC_MAX_BUCKET_SZ(unit)) {
        hw_param->bkt_sz = (i - 1);
    }

    hw_param->bkt_cnt = BCMMETER_SC_BKT_SZ_TO_TOKEN_MAP(unit)[i];

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Insert the storm control meter.
 *
 * Insert storm control meter entry into h/w table.
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id     LT transaction ID.
 * \param [in] sc_entry     Storm control meter config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcm56960_a0_meter_sc_insert (int unit,
                             uint32_t trans_id,
                             bcmmeter_sc_entry_t *sc_entry)
{
    uint8_t         *req_meter_config = NULL, *req_meter = NULL;
    uint32_t        port_id = 0, meter_idx = 0, j = 0;
    uint32_t        byte_mode = 0, burst_size = 0;
    uint32_t        alloc_sz = 0, quantum = 0, old_quantum = 0;
    uint64_t        meter_rate = 0;
    uint64_t        sc_meter_rate[BCMMETER_SC_NUM_METERS_PER_PORT(unit)];
    uint32_t        sc_burst_size[BCMMETER_SC_NUM_METERS_PER_PORT(unit)];
    bcmdrd_sid_t    sid;
    bcmdrd_fid_t    fid;
    bcmmeter_sc_hw_params_t *hw_param = NULL;

    SHR_FUNC_ENTER(unit);

    port_id   = sc_entry->port_id;
    meter_idx = sc_entry->meter_id;

    sal_memset(sc_meter_rate, 0, sizeof(sc_meter_rate));
    sal_memset(sc_burst_size, 0, sizeof(sc_burst_size));

    /*
     * Read METER_STORM_CONTROL_CONFIGr to get current BYTE_MODE and
     * PACKET_QUANTUM
     */
    sid = BCMMETER_SC_CONFIG_TABLE_SID(unit);
    alloc_sz = bcmdrd_pt_entry_wsize(unit, sid);
    alloc_sz *= 4;
    SHR_ALLOC(req_meter_config, alloc_sz, "Sc meter config entry");
    SHR_NULL_CHECK(req_meter_config, SHR_E_MEMORY);
    sal_memset(req_meter_config, 0, alloc_sz);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_cfg_read(unit,
                                       trans_id,
                                       port_id,
                                       req_meter_config));

    fid = BCMMETER_SC_BYTEMODE_FID(unit);
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)req_meter_config,
                        fid, &byte_mode);

    fid = BCMMETER_SC_PKTQUANTUM_FID(unit);
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)req_meter_config,
                        fid, &old_quantum);

    if (byte_mode == BCMMETER_SC_BYTE_MODE) {
        meter_rate = sc_entry->meter_rate_kbps;
        burst_size = sc_entry->burst_size_kbits;
    } else {
        meter_rate = sc_entry->meter_rate_pps;
        burst_size = sc_entry->burst_size_pkts;
    }

    alloc_sz = (BCMMETER_SC_NUM_METERS_PER_PORT(unit) *
                sizeof(bcmmeter_sc_hw_params_t));
    SHR_ALLOC(hw_param, alloc_sz, "SC meter hw params");
    SHR_NULL_CHECK(hw_param, SHR_E_MEMORY);
    sal_memset(hw_param, 0, alloc_sz);

    /*
     * Read all IN_USE meters of this port and retrieve meter rate
     */
    if ((old_quantum != 0) && (byte_mode == BCMMETER_SC_PACKET_MODE)) {
        for (j = 0; j < BCMMETER_SC_NUM_METERS_PER_PORT(unit); j++) {
            if (!BCMMETER_SC_INDEX_USED_GET(unit, port_id, j)) {
                continue;
            }

            sc_meter_rate[j] =
                BCMMETER_SC_GET_METER_RATE_PPS(unit, port_id, j);
            sc_burst_size[j] =
                BCMMETER_SC_GET_BURST_SIZE_PKTS(unit, port_id, j);
        }
    }

    sc_meter_rate[meter_idx] = meter_rate;
    sc_burst_size[meter_idx] = burst_size;

    /*
     * Calculate PACKET_QUANTUM, REFRESHCOUNT and BUCKETSIZE
     */
    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_compute_hw_param(unit,
                                               byte_mode,
                                               old_quantum,
                                               meter_rate,
                                               burst_size,
                                               &quantum,
                                               &hw_param[meter_idx]));

    /*
     * If PACKET_QUANTUM has changed,
     * recalculate refresh count and bucket size for the other
     * meters of this port as PACKET_QUANTUM is a per-port config
     * and not per-meter
     */
    if (old_quantum && quantum && (quantum != old_quantum)) {
        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_sc_recalc_hw_param(unit,
                                                  meter_idx,
                                                  quantum,
                                                  sc_meter_rate,
                                                  sc_burst_size,
                                                  hw_param));
    }

    /* Write back METER_STORM_CONTROL entry as well as
     * METER_STORM_CONTROL_CONFIG entry
     */
    sid = BCMMETER_SC_TABLE_SID(unit);

    alloc_sz = bcmdrd_pt_entry_wsize(unit, sid);
    alloc_sz *= 4;
    SHR_ALLOC(req_meter, alloc_sz, "Sc meter entry");
    SHR_NULL_CHECK(req_meter, SHR_E_MEMORY);

    for (j = 0; j < BCMMETER_SC_NUM_METERS_PER_PORT(unit); j++) {
        if (!hw_param[j].refresh_cnt) {
            continue;
        }
        sal_memset(req_meter, 0, alloc_sz);

        fid = BCMMETER_SC_BUCKETSIZE_FID(unit);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)req_meter,
                            fid, &hw_param[j].bkt_sz);

        fid = BCMMETER_SC_BUCKETCOUNT_FID(unit);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)req_meter,
                            fid, &hw_param[j].bkt_cnt);

        fid = BCMMETER_SC_REFRESHCOUNT_FID(unit);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)req_meter,
                            fid, &hw_param[j].refresh_cnt);

        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_sc_entry_write(unit,
                                              trans_id,
                                              port_id,
                                              j,
                                              req_meter));
    }

    /*
     * Write back config to METER_STORM_CONTROL_CONFIG register
     * if PACKET_QUANTUM has changed.
     */
    if (quantum != old_quantum) {
        sid = BCMMETER_SC_CONFIG_TABLE_SID(unit);

        fid = BCMMETER_SC_PKTQUANTUM_FID(unit);
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)req_meter_config,
                            fid, &quantum);

        SHR_IF_ERR_EXIT
            (bcm56960_a0_meter_sc_cfg_write(unit,
                                            trans_id,
                                            port_id,
                                            req_meter_config));
    }

exit:
    SHR_FREE(hw_param);
    SHR_FREE(req_meter_config);
    SHR_FREE(req_meter);
    SHR_FUNC_EXIT();

}

/*!
 * \brief Lookup storm control meter.
 *
 * Read storm control meter entry from h/w and return
 * the data.
 *
 * \param [in] unit         Unit num_er.
 * \param [in] trans_id     LT transaction ID.
 * \param [in] port_id      Port number.
 * \param [in] meter_idx    Meter offset.
 * \param [out] ptr         Storm control meter pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcm56960_a0_meter_sc_lookup (int unit,
                             uint32_t trans_id,
                             uint32_t port_id,
                             uint32_t meter_idx,
                             bcmmeter_sc_entry_t **ptr)
{
    uint8_t         *req_meter = NULL;
    uint32_t        refresh_cnt = 0, bucket_size = 0;
    uint32_t        entry_sz = 0, byte_mode = 0, quantum = 1;
    uint64_t        meter_rate_oper = 0, burst_size_oper = 0;
    bcmdrd_sid_t    sid;
    bcmdrd_fid_t    fid;
    bcmmeter_sc_entry_t *sc_entry = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Retrieve the pointer to this meter entry from the database.
     */
    sc_entry = BCMMETER_SC_METER_ENTRY(unit, port_id, meter_idx);
    if (sc_entry == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
    }

    /*
     * Read METER_STORM_CONTROL_CONFIGr to get current BYTE_MODE and
     * PACKET_QUANTUM
     */
    sid = BCMMETER_SC_CONFIG_TABLE_SID(unit);
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(req_meter, entry_sz, "Sc meter config entry");
    SHR_NULL_CHECK(req_meter, SHR_E_MEMORY);

    sal_memset(req_meter, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_cfg_read(unit,
                                       trans_id,
                                       port_id,
                                       req_meter));

    fid = BCMMETER_SC_BYTEMODE_FID(unit);
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)req_meter,
                        fid, &byte_mode);

    fid = BCMMETER_SC_PKTQUANTUM_FID(unit);
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)req_meter,
                        fid, &quantum);

    SHR_FREE(req_meter);

    /*
     * Read METER_STORM_CONTROL
     */
    sid = BCMMETER_SC_TABLE_SID(unit);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(req_meter, entry_sz, "Sc meter entry");
    SHR_NULL_CHECK(req_meter, SHR_E_MEMORY);

    sal_memset(req_meter, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_entry_read(unit,
                                         trans_id,
                                         port_id,
                                         meter_idx,
                                         req_meter));

    fid = BCMMETER_SC_REFRESHCOUNT_FID(unit);
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)req_meter,
                        fid, &refresh_cnt);

    fid = BCMMETER_SC_BUCKETSIZE_FID(unit);
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)req_meter,
                        fid, &bucket_size);

    /*
     * Calculate meter rate and burst size from REFRESHCOUNT, BUCKETSIZE
     * and PACKET_QUANTUM
     */
    meter_rate_oper = BCMMETER_SC_COMPUTE_METER_RATE(
        unit,
        byte_mode,
        refresh_cnt,
        quantum);

    burst_size_oper = BCMMETER_SC_GET_BURST_SZ_FROM_BKT_SZ(
        unit,
        byte_mode,
        bucket_size,
        quantum);

    if (byte_mode == BCMMETER_SC_BYTE_MODE) {
        sc_entry->meter_rate_kbps_oper = meter_rate_oper;
        sc_entry->burst_size_kbits_oper = burst_size_oper;
    } else {
        sc_entry->meter_rate_pps_oper = meter_rate_oper;
        sc_entry->burst_size_pkts_oper = burst_size_oper;
    }

    *ptr = sc_entry;
exit:
    SHR_FREE(req_meter);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete storm control meter.
 *
 * Delete storm control meter entry from h/w table.
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id     LT transaction ID.
 * \param [in] sc_entry     Storm control meter config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcm56960_a0_meter_sc_delete (int unit,
                             uint32_t trans_id,
                             bcmmeter_sc_entry_t *sc_entry)
{
    int         port_in_use = 0;
    uint8_t     *req_meter = NULL;
    uint32_t    entry_sz, quantum;
    uint32_t    port_id = 0, meter_idx = 0, i = 0;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    port_id   = sc_entry->port_id;
    meter_idx = sc_entry->meter_id;

    sid = BCMMETER_SC_TABLE_SID(unit);
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(req_meter, entry_sz, "Sc meter entry");
    SHR_NULL_CHECK(req_meter, SHR_E_MEMORY);

    sal_memset(req_meter, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_entry_write(unit,
                                          trans_id,
                                          port_id,
                                          meter_idx,
                                          req_meter));

    SHR_FREE(req_meter);

    /*
     * Check if all meters associated with the port have been deleted
     * If no more meters, clear PACKET_QUANTUM
     */
    for (i = 0; i < BCMMETER_SC_NUM_METERS_PER_PORT(unit); i++) {
        if (BCMMETER_SC_INDEX_USED_GET(unit, port_id, i)) {
            port_in_use = 1;
        }
    }

    if (port_in_use) {
        SHR_EXIT();
    }

    /*
     * Delete PACKET_QUANTUM from METER_STORM_CONTROL_CONFIGr
     */
    sid = BCMMETER_SC_CONFIG_TABLE_SID(unit);
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(req_meter, entry_sz, "Sc meter config entry");
    SHR_NULL_CHECK(req_meter, SHR_E_MEMORY);
    sal_memset(req_meter, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_cfg_read(unit,
                                       trans_id,
                                       port_id,
                                       req_meter));

    quantum = 0;
    fid = BCMMETER_SC_PKTQUANTUM_FID(unit);
    bcmdrd_pt_field_set(unit, sid, (uint32_t *)req_meter,
                        fid, &quantum);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_cfg_write(unit,
                                        trans_id,
                                        port_id,
                                        req_meter));

exit:
    SHR_FREE(req_meter);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Meter operating mode.
 *
 * Read the hardware register and get the meter mode
 * of this port.
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id     LT transaction ID.
 * \param [in] port_id      Port number.
 * \param [out] byte_mode   Byte mode to be returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware read fails.
 */
int
bcm56960_a0_meter_sc_bytemode_get (int unit,
                                   uint32_t trans_id,
                                   uint32_t port_id,
                                   uint32_t *byte_mode)
{
    uint8_t     *req_meter_config = NULL;
    uint32_t    entry_sz = 0;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /*
     * Read METER_STORM_CONTROL_CONFIGr to get current BYTE_MODE and
     * PACKET_QUANTUM
     */
    sid = BCMMETER_SC_CONFIG_TABLE_SID(unit);
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(req_meter_config, entry_sz, "Sc meter config entry");
    SHR_NULL_CHECK(req_meter_config, SHR_E_MEMORY);
    sal_memset(req_meter_config, 0, entry_sz);

    SHR_IF_ERR_EXIT
        (bcm56960_a0_meter_sc_cfg_read(unit,
                                       trans_id,
                                       port_id,
                                       req_meter_config));

    fid = BCMMETER_SC_BYTEMODE_FID(unit);
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)req_meter_config,
                        fid, byte_mode);

exit:
    SHR_FREE(req_meter_config);
    SHR_FUNC_EXIT();
}

/*!
 * \brief SC Meter init.
 *
 * Initialize Storm Control Meter related data structures
 * for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  cold          Cold/Warm boot
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_sc_init (int unit, bool cold)
{
    uint32_t    ha_req_size, ha_alloc_size;

    SHR_FUNC_ENTER(unit);

    ha_req_size = (BCMMETER_SC_NUM_METERS(unit) * sizeof(bcmmeter_sc_entry_t));
    ha_alloc_size = ha_req_size;

    bcmmeter_sc_sw_state[unit].bkp_entry =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         BCMMETER_SC_BKP_SUB_COMP_ID,
                         &ha_alloc_size);
    SHR_NULL_CHECK(bcmmeter_sc_sw_state[unit].bkp_entry, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (cold) {
        sal_memset(bcmmeter_sc_sw_state[unit].bkp_entry, 0, ha_alloc_size);
    }

    SHR_ALLOC(bcmmeter_sc_sw_state[unit].meter_entry,
              ha_alloc_size,
              "SC meter sw state");
    SHR_NULL_CHECK(bcmmeter_sc_sw_state[unit].meter_entry, SHR_E_MEMORY);
    sal_memset(bcmmeter_sc_sw_state[unit].meter_entry, 0, ha_alloc_size);

    sal_memcpy(bcmmeter_sc_sw_state[unit].meter_entry,
               bcmmeter_sc_sw_state[unit].bkp_entry,
               ha_alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        if (bcmmeter_sc_sw_state[unit].meter_entry != NULL) {
            SHR_FREE(bcmmeter_sc_sw_state[unit].meter_entry);
        }
        if (bcmmeter_sc_sw_state[unit].bkp_entry != NULL) {
            shr_ha_mem_free(unit, bcmmeter_sc_sw_state[unit].bkp_entry);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief SC Meter cleanup.
 *
 * Cleanup Storm Control Meter related data structures
 * allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_sc_cleanup (int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmmeter_sc_sw_state[unit].meter_entry != NULL) {
        SHR_FREE(bcmmeter_sc_sw_state[unit].meter_entry);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Storm control meter driver attach
 *
 * Attach storm control meter driver for this unit
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_sc_attach (int unit)
{
    return bcmmeter_sc_drv_set(unit, &bcm56960_a0_meter_sc_attributes);
}

/*!
 * \brief Storm control meter driver detach
 *
 * Detach storm control meter driver from this unit
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56960_a0_meter_sc_detach (int unit)
{
    return bcmmeter_sc_drv_set(unit, NULL);
}
