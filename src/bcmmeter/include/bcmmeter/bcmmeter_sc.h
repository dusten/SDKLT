/*! \file bcmmeter_sc.h
 *
 * This file contains Storm Control Meter Custom handler
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

#ifndef BCMMETER_SC_H
#define BCMMETER_SC_H

#include <bcmltd/bcmltd_handler.h>
#include <shr/shr_bitop.h>
#include <bcmdrd_config.h>

/*! Meter mode bytes. */
#define BCMMETER_SC_BYTE_MODE       true
/*! Meter mode packets. */
#define BCMMETER_SC_PACKET_MODE     false

/*! Maximum number of ports on this unit. */
#define BCMMETER_SC_MAX_PORTS(unit)                       \
    bcmmeter_sc_chip_attr[unit]->max_ports
/*! Number of SC meter per port on this unit. */
#define BCMMETER_SC_NUM_METERS_PER_PORT(unit)             \
    bcmmeter_sc_chip_attr[unit]->meters_per_port
/*! Number of quantum levels this unit. */
#define BCMMETER_SC_PKT_QUANTUM_LEVELS(unit)              \
    bcmmeter_sc_chip_attr[unit]->pkt_quant_levels
/*! Minimum metering rate in byte mode on this unit. */
#define BCMMETER_SC_RATE_KBPS_MIN(unit)                   \
    bcmmeter_sc_chip_attr[unit]->meter_kbps_min
/*! Maximum bucket size on this unit. */
#define BCMMETER_SC_MAX_BUCKET_SZ(unit)                   \
    bcmmeter_sc_chip_attr[unit]->max_bkt_sz
/*! Maximum refresh count on this unit. */
#define BCMMETER_SC_MAX_REFRESH_COUNT(unit)               \
    bcmmeter_sc_chip_attr[unit]->max_refresh_cnt
/*! Number of refresh cycles per second on this unit. */
#define BCMMETER_SC_NUM_REFRESH_PER_SEC(unit)             \
    bcmmeter_sc_chip_attr[unit]->num_refresh_per_sec
/*! Minimum number of toeksn per refresh cycle this unit. */
#define BCMMETER_SC_MIN_TOKENS_PER_REFRESH(unit)          \
    bcmmeter_sc_chip_attr[unit]->min_tokens_per_refresh
/*! Storm control meter configuration register on this unit. */
#define BCMMETER_SC_CONFIG_TABLE_SID(unit)                \
    bcmmeter_sc_chip_attr[unit]->config_sid
/*! Storm control meter table name on this unit. */
#define BCMMETER_SC_TABLE_SID(unit)                       \
    bcmmeter_sc_chip_attr[unit]->meter_sid
/*! Storm control meter byte mode field name on this unit. */
#define BCMMETER_SC_BYTEMODE_FID(unit)                    \
    bcmmeter_sc_chip_attr[unit]->byte_mode_fid
/*! Storm control meter packet quantum field name on this unit. */
#define BCMMETER_SC_PKTQUANTUM_FID(unit)                  \
    bcmmeter_sc_chip_attr[unit]->pkt_quantum_fid
/*! Storm control meter refresh count field name on this unit. */
#define BCMMETER_SC_REFRESHCOUNT_FID(unit)                \
    bcmmeter_sc_chip_attr[unit]->refresh_count_fid
/*! Storm control meter bucket size field name on this unit. */
#define BCMMETER_SC_BUCKETSIZE_FID(unit)                  \
    bcmmeter_sc_chip_attr[unit]->bucket_size_fid
/*! Storm control meter bucket count field name on this unit. */
#define BCMMETER_SC_BUCKETCOUNT_FID(unit)                 \
    bcmmeter_sc_chip_attr[unit]->bucket_cnt_fid
/*! Storm control meter bucket size to kbit mapping on this unit. */
#define BCMMETER_SC_BKT_SZ_TO_KBIT_MAP(unit)              \
    bcmmeter_sc_chip_attr[unit]->bkt_sz_to_kbit_map
/*! Storm control meter bucket size to token mapping on this unit. */
#define BCMMETER_SC_BKT_SZ_TO_TOKEN_MAP(unit)             \
    bcmmeter_sc_chip_attr[unit]->bkt_sz_to_token_map
/*! Storm control meter packet quantum level info on this unit. */
#define BCMMETER_SC_PKT_QUANTUM_INFO(unit)                \
    bcmmeter_sc_chip_attr[unit]->pkt_quant_info
/*! Storm control meter function pointers. */
#define BCMMETER_SC_FN_PTRS(unit)                          \
    bcmmeter_sc_chip_attr[unit]->fn_ptrs

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_meter_sc_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_meter_sc_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Packet quantum information structure.
 *
 * Metering rate and burst size details at each quantum level.
 */
typedef struct bcmmeter_sc_pkt_quantum_info_s {
    /*! Packet quantum value at this quantum level. */
    uint32_t    quantum;
    /*! Minimum metering rate at this quantum level. */
    uint32_t    min_rate;
    /*! Maximum metering rate at this quantum level. */
    uint32_t    max_rate;
} bcmmeter_sc_pkt_quantum_info_t;

/*!
 * \brief Storm control meter HW params.
 *
 * Storm control meter entry HW params
 */
typedef struct bcmmeter_sc_hw_params_s {
    /*! Refresh count. */
    uint32_t    refresh_cnt;
    /*! Bucket size. */
    uint32_t    bkt_sz;
    /*! Bucket count. */
    uint32_t    bkt_cnt;
} bcmmeter_sc_hw_params_t;

/*!
 * \brief Storm control meter entry structure.
 *
 * Storm control meter entry configuration.
 */
typedef struct bcmmeter_sc_entry_s {
    /*! Port number. */
    uint32_t    port_id;
    /*! Per port storm control meter id . */
    uint32_t    meter_id;
    /*! Meter id in use. */
    bool        meter_in_use;
    /*! Metering rate in kbps. */
    uint64_t    meter_rate_kbps;
    /*! Burst size in kbits. */
    uint32_t    burst_size_kbits;
    /*! Metering rate in pps. */
    uint64_t    meter_rate_pps;
    /*! Burst size in packets. */
    uint32_t    burst_size_pkts;
    /*! Metering rate in kbps. */
    uint64_t    meter_rate_kbps_oper;
    /*! Burst size in kbits. */
    uint32_t    burst_size_kbits_oper;
    /*! Metering rate in pps. */
    uint64_t    meter_rate_pps_oper;
    /*! Burst size in packets. */
    uint32_t    burst_size_pkts_oper;
} bcmmeter_sc_entry_t;

/*! Storm control meter driver init. */
typedef int (*bcmmeter_sc_init_f)(int unit, bool cold);

/*! Storm control meter driver cleanup. */
typedef int (*bcmmeter_sc_cleanup_f)(int unit);

/*! Storm control meter insert. */
typedef int(*bcmmeter_sc_insert_f)(int unit,
                                   uint32_t trans_id,
                                   bcmmeter_sc_entry_t *sc_entry);
/*! Storm control meter lookup. */
typedef int(*bcmmeter_sc_lookup_f)(int unit,
                                   uint32_t trans_id,
                                   uint32_t port,
                                   uint32_t meter_id,
                                   bcmmeter_sc_entry_t **sc_entry);
/*! Storm control meter delete. */
typedef int(*bcmmeter_sc_delete_f)(int unit,
                                   uint32_t trans_id,
                                   bcmmeter_sc_entry_t *sc_entry);
/*! Get storm control meter byte mode. */
typedef int(*bcmmeter_sc_bytemode_get_f)(int unit,
                                         uint32_t trans_id,
                                         uint32_t port,
                                         uint32_t *byte_mode);

/*!
 * \brief Storm control meter function pointers structure.
 */
typedef struct bcmmeter_sc_fn_ptrs_s {
    /*! SC meter driver init. */
    bcmmeter_sc_init_f          init;
    /*! SC meter driver cleanup. */
    bcmmeter_sc_cleanup_f       cleanup;
    /*! SC meter insert. */
    bcmmeter_sc_insert_f        ins;
    /*! SC meter lookup. */
    bcmmeter_sc_lookup_f        lkp;
    /*! SC meter delete. */
    bcmmeter_sc_delete_f        del;
    /*! SC meter update. */
    bcmmeter_sc_bytemode_get_f  bytemode_get;
} bcmmeter_sc_fn_ptrs_t;

/*!
 * \brief Storm control meter chip attribute structure.
 *
 * Chip specific attributes for storm control meter.
 */
typedef struct bcmmeter_sc_chip_attributes_s {
    /*! Maximum number of ports. */
    uint8_t     max_ports;
    /*! Number of meters per port. */
    uint8_t     meters_per_port;
    /*! Number of packet quantum levels. */
    uint8_t     pkt_quant_levels;
    /*! Min metering rate in kbps. */
    uint32_t    meter_kbps_min;
    /*! Max bucket size. */
    uint32_t    max_bkt_sz;
    /*! Max refresh count. */
    uint32_t    max_refresh_cnt;
    /*! Number of meter refresh cycles per second */
    uint32_t    num_refresh_per_sec;
    /*! Minimum number of tokens in each refresh cycle. */
    uint32_t    min_tokens_per_refresh;
    /*! Storm control register name. */
    uint32_t    config_sid;
    /*! Storm control meter name. */
    uint32_t    meter_sid;
    /*! Storm control meter byte mode field name. */
    uint32_t    byte_mode_fid;
    /*! Storm control meter packet quantum field name. */
    uint32_t    pkt_quantum_fid;
    /*! Storm control meter refresh count field name. */
    uint32_t    refresh_count_fid;
    /*! Storm control meter bucket size field name. */
    uint32_t    bucket_size_fid;
    /*! Storm control meter bucket count field name. */
    uint32_t    bucket_cnt_fid;
    /*! Storm control meter bucket size to kbit mapping. */
    uint32_t    *bkt_sz_to_kbit_map;
    /*! Storm control meter bucket size to token mapping. */
    uint32_t    *bkt_sz_to_token_map;
    /*! Storm control meter packet quantum structure. */
    bcmmeter_sc_pkt_quantum_info_t *pkt_quant_info;
    /*! Storm control meter function pointers. */
    bcmmeter_sc_fn_ptrs_t fn_ptrs;
    /*! End marker. */
    uint32_t    zero;
} bcmmeter_sc_chip_attributes_t;

/*!
 * \brief Storm control meter state structure.
 *
 * Meter state of all storm control meters of all ports.
 */
typedef struct bcmmeter_sc_sw_state_s {
    /*! Storm control meter entries list. */
    bcmmeter_sc_entry_t     *meter_entry;
    /*! Storm control meter entries backup list. */
    bcmmeter_sc_entry_t     *bkp_entry;
} bcmmeter_sc_sw_state_t;

/*! Storm control Meter chip attributes. */
extern bcmmeter_sc_chip_attributes_t
    *bcmmeter_sc_chip_attr[BCMDRD_CONFIG_MAX_UNITS];
/*! Storm control Meter state. */
extern bcmmeter_sc_sw_state_t
    bcmmeter_sc_sw_state[BCMDRD_CONFIG_MAX_UNITS];

/*! Storm control meter entry structure version. */
#define BCMMETER_SC_ENTRY_T_VER             1
/*! Storm control meter entry structure version. */
#define BCMMETER_SC_BKP_ENTRY_T_VER         1
/*! Storm control meter entry structure signature. */
#define BCMMETER_SC_ENTRY_T_SIG             0x65899886130cbf44
/*! Storm control backup entry structure signature. */
#define BCMMETER_SC_BKP_ENTRY_T_SIG         0x65899886130cae33

/*! Total number of Storm control meters on this unit. */
#define BCMMETER_SC_NUM_METERS(unit)                                \
        (BCMMETER_SC_MAX_PORTS(unit) * BCMMETER_SC_NUM_METERS_PER_PORT(unit))

/*!
 * \brief Get Storm control meter index.
 *
 * Derive the index into Storm Control Meter hardware table.
 *
 * \param [in] unit Unit number.
 * \param [in] port_id Port number.
 * \param [in] meter_idx Meter index relative to the port.
 */
#define BCMMETER_SC_GET_METER_IDX(unit, port_id, meter_idx)         \
    ((BCMMETER_SC_NUM_METERS_PER_PORT(unit) * port_id) + meter_idx)

/*! Get Pointer to Storm Control meter state. */
#define BCMMETER_SC_METER_ENTRY(unit, port, id)                     \
    (&bcmmeter_sc_sw_state[unit].meter_entry[BCMMETER_SC_GET_METER_IDX(unit, port, id)])

/*! Get Pointer to Storm Control meter active entry. */
#define BCMMETER_SC_ACTIVE_STATE(unit)                              \
    (bcmmeter_sc_sw_state[unit].meter_entry)

/*! Get Pointer to Storm Control meter backup entry. */
#define BCMMETER_SC_BKP_STATE(unit)                                 \
    (bcmmeter_sc_sw_state[unit].bkp_entry)

/*! Get configured metering rate in kbps. */
#define BCMMETER_SC_GET_METER_RATE_KBPS(unit, port, id)             \
    BCMMETER_SC_METER_ENTRY(unit, port, id)->meter_rate_kbps

/*! Get configured metering rate in pps. */
#define BCMMETER_SC_GET_METER_RATE_PPS(unit, port, id)              \
    BCMMETER_SC_METER_ENTRY(unit, port, id)->meter_rate_pps

/*! Get configured burst size in kbits. */
#define BCMMETER_SC_GET_BURST_SIZE_KBITS(unit, port, id)            \
    BCMMETER_SC_METER_ENTRY(unit, port, id)->burst_size_kbits

/*! Get configured burst size in pkts. */
#define BCMMETER_SC_GET_BURST_SIZE_PKTS(unit, port, id)             \
    BCMMETER_SC_METER_ENTRY(unit, port, id)->burst_size_pkts

/*!
 * \brief Mark meter index as used.
 *
 * Mark storm control meter index as used.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] id   Per-port meter index.
 */
#define BCMMETER_SC_INDEX_USED_SET(unit, port, id)                  \
    BCMMETER_SC_METER_ENTRY(unit, port, id)->meter_in_use = true;
/*!
 * \brief Get meter index status.
 *
 * Check if meter index is in use.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] id   Per-port meter index.
 */
#define BCMMETER_SC_INDEX_USED_GET(unit, port, id)                  \
    (BCMMETER_SC_METER_ENTRY(unit, port, id)->meter_in_use)
/*!
 * \brief Mark meter index as free.
 *
 * Mark storm control meter index as free.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] id   Per-port meter index.
 */
#define BCMMETER_SC_INDEX_USED_CLR(unit, port, id)                  \
    BCMMETER_SC_METER_ENTRY(unit, port, id)->meter_in_use = false;

/*!
 * \brief Get minimum number of tokens per second.
 *
 * Calculate the number of tokens per second when refresh count
 * is minimum (RC = 1).
 * 2 x REFRESHCOUNT tokens on every refresh cycle ie. 15.625us
 * tokens per sec (tps) = ((2 * rc) / 15.625 us) = ((2 * rc * 10^6) / 15.625)
 * tps = rc * 128000
 * min_tps = 128000 (when rc = 1)
 *
 * In packet mode, 1 packet = PACKET_QUANTUM tokens
 * min_pps = (128000 / PQ)
 * pps = (rc * min_pps)
 * rc  = (pps / min_pps))
 *
 * \param [in] unit Unit number
 */
#define BCMMETER_SC_TPS_MIN(unit)                                         \
    (BCMMETER_SC_NUM_REFRESH_PER_SEC(unit) *                              \
     BCMMETER_SC_MIN_TOKENS_PER_REFRESH(unit))

/*!
 * \brief Get minimum number of packets per second.
 *
 * Calculate the minimum number of packets per second
 * with a particular packet quantum value.
 *
 * \param [in] unit Unit number.
 * \param [in] quantum Packet quantum.
 */
#define BCMMETER_SC_PPS_MIN(unit, quantum)                                \
    (BCMMETER_SC_TPS_MIN(unit) / quantum)

/*!
 * \brief Calculate meter refresh count.
 *
 * Calculate refresh count from meter rate and packet quantum
 * in packet mode.
 *
 * \param [in] unit Unit number.
 * \param [in] rps Metering rate.
 * \param [in] quantum Packet quantum.
 */
#define BCMMETER_SC_COMPUTE_PKT_REFRESH_COUNT(unit, rps, quantum)         \
    ((rps + (BCMMETER_SC_PPS_MIN(unit, quantum) - 1)) /                   \
     BCMMETER_SC_PPS_MIN(unit, quantum))

/*!
 * \brief Calculate meter rate.
 *
 * Calculate meter rate from refresh count and packet quantum
 * in packet mode.
 *
 * \param [in] unit Unit number.
 * \param [in] rc Refresh count.
 * \param [in] quantum Packet quantum.
 */
#define BCMMETER_SC_COMPUTE_PKT_METER_RATE(unit, rc, quantum)             \
    ((uint64_t)rc * (uint64_t)BCMMETER_SC_PPS_MIN(unit, quantum))

/*!
 * \brief Get meter burst size..
 *
 * Calculate burst size supported by the meter with a particular
 * bucket size.
 *
 * \param [in] unit Unit number.
 * \param [in] bktsz Bucket size.
 * \param [in] quantum Packet quantum.
 */
#define BCMMETER_SC_GET_BURST_SZ_PACKETS_FROM_BKT_SZ(unit, bktsz, quantum)\
    (BCMMETER_SC_BKT_SZ_TO_TOKEN_MAP(unit)[bktsz] / quantum)

/*!
 * \brief Calculate meter refresh count.
 *
 * Calculate refresh count from meter rate in byte mode.
 *
 * In byte mode, 1 token = 0.5 bits
 * bps = (tps * 0.5) = (rc * 128000 * 0.5) = (rc * 64000)
 *
 * Meter rate = 64kbps when rc = 1
 * bps = (rc * 64000)
 * rc  = (bps / 64000)
 *
 * \param [in] unit Unit number.
 * \param [in] rps Metering rate.
 */
#define BCMMETER_SC_COMPUTE_BYTE_REFRESH_COUNT(unit, rps)                   \
    ((rps + (BCMMETER_SC_RATE_KBPS_MIN(unit) - 1)) /                        \
     BCMMETER_SC_RATE_KBPS_MIN(unit))

/*!
 * \brief Calculate meter rate.
 *
 * Calculate meter rate from refresh count in byte mode.
 *
 * \param [in] unit Unit number.
 * \param [in] rc Refresh count.
 */
#define BCMMETER_SC_COMPUTE_BYTE_METER_RATE(unit, rc)                       \
    ((uint64_t)rc * (uint64_t)BCMMETER_SC_RATE_KBPS_MIN(unit))
/*!
 * \brief Get meter burst size..
 *
 * Get burst size supported by the meter with a particular
 * bucket size.
 *
 * Computing burst size from meter rate
 * rps => (rps * 15.625 / 10^6) => (rps / 64000) per 15.625 us
 *
 * In byte mode, if rps is bps
 * burst (bits) = (rps / 64000)
 *
 * \param [in] unit Unit number.
 * \param [in] bktsz Bucket size.
 */
#define BCMMETER_SC_GET_BURST_SZ_KBITS_FROM_BKT_SZ(unit, bktsz)             \
    (BCMMETER_SC_BKT_SZ_TO_KBIT_MAP(unit)[bktsz])

/*! Calculate meter rate from refresh count and packet quantum. */
#define BCMMETER_SC_COMPUTE_METER_RATE(unit, mode, rc, quantum)             \
    ((mode) ? (BCMMETER_SC_COMPUTE_BYTE_METER_RATE(unit, rc)) :             \
     (BCMMETER_SC_COMPUTE_PKT_METER_RATE(unit, rc, quantum)))
/*! Get burst size from bucket size and packet quantum. */
#define BCMMETER_SC_GET_BURST_SZ_FROM_BKT_SZ(unit, mode, bktsz, quantum)    \
    ((mode) ? (BCMMETER_SC_GET_BURST_SZ_KBITS_FROM_BKT_SZ(unit, bktsz)) :   \
     (BCMMETER_SC_GET_BURST_SZ_PACKETS_FROM_BKT_SZ(unit, bktsz, quantum)))

/*! Get minimum metering rate supported at a given quantum level. */
#define BCMMETER_SC_QUANTUM_MIN_RATE(unit, level)                           \
    (BCMMETER_SC_PKT_QUANTUM_INFO(unit)[level].min_rate)
/*! Get maximum metering rate supported at a given quantum level. */
#define BCMMETER_SC_QUANTUM_MAX_RATE(unit, level)                           \
    ((uint64_t)BCMMETER_SC_PKT_QUANTUM_INFO(unit)[level].max_rate *         \
     (uint64_t)1000000)
/*! Get packet quantum value at a given quantum level. */
#define BCMMETER_SC_QUANTUM_VALUE(unit, level)                              \
    (BCMMETER_SC_PKT_QUANTUM_INFO(unit)[level].quantum)

/*! Get the default value of an LT field. */
#define BCMMETER_SC_FIELD_DEF(dtag)   ((dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                       (field_def.def.u8) :                     \
                                       ((dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                        (field_def.def.u16) :                   \
                                        ((dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                         (field_def.def.u32) :                  \
                                         ((dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                          (field_def.def.u64) :                 \
                                          (field_def.def.is_true)))))

/*! Get the minimum supported value of an LT field. */
#define BCMMETER_SC_FIELD_MIN(dtag)   ((dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                       (field_def.min.u8) :                     \
                                       ((dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                        (field_def.min.u16) :                   \
                                        ((dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                         (field_def.min.u32) :                  \
                                         ((dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                          (field_def.min.u64) :                 \
                                          (field_def.min.is_true)))))

/*! Get the maximum supported value of an LT field. */
#define BCMMETER_SC_FIELD_MAX(dtag)   ((dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                       (field_def.max.u8) :                     \
                                       ((dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                        (field_def.max.u16) :                   \
                                        ((dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                         (field_def.max.u32) :                  \
                                         ((dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                          (field_def.max.u64) :                 \
                                          (field_def.max.is_true)))))

/*!
 * \brief SC Meter validation
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_validate(int unit,
                     bcmlt_opcode_t opcode,
                     const bcmltd_fields_t *in,
                     const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter insert
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_insert(int unit,
                   uint32_t trans_id,
                   const bcmltd_fields_t *in,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter lookup
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_lookup(int unit,
                   uint32_t trans_id,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter delete
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_delete(int unit,
                   uint32_t trans_id,
                   const bcmltd_fields_t *in,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter update
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_update(int unit,
                   uint32_t trans_id,
                   const bcmltd_fields_t *in,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter init
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  cold          Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_init(int unit,
                 bool cold);

/*!
 * \brief SC Meter cleanup
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_cleanup(int unit);

/*!
 * \brief Storm control meter traverse get first.
 *
 * Get the first Storm control meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_first (int unit,
                   uint32_t trans_id,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg);
/*!
 * \brief Storm control meter traverse get next.
 *
 * Get the next Storm control meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_next (int unit,
                  uint32_t trans_id,
                  const bcmltd_fields_t *in,
                  bcmltd_fields_t *out,
                  const bcmltd_generic_arg_t *arg);

/*!
 * \brief Storm control meter transaction commit
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_commit(int unit,
                   uint32_t trans_id,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief Storm control meter transaction abort
 *
 * Rollback the Meter entry from the backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmmeter_sc_abort(int unit,
                  uint32_t trans_id,
                  const bcmltd_generic_arg_t *arg);

/*!
 * \brief Set pointer to storm control meter driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE
 */
extern int
bcmmeter_sc_attach (int unit);

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmmeter_sc_detach (int unit);

/*!
 * \brief Set pointer to storm control meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] attr Chip driver structure.
 *
 * \return SHR_E_NONE
 */
extern int
bcmmeter_sc_drv_set (int unit,
                     bcmmeter_sc_chip_attributes_t *attr);

/*!
 * \brief Get pointer to storm control meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] attr Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmmeter_sc_drv_get (int unit,
                     bcmmeter_sc_chip_attributes_t **attr);

#endif /* BCMMETER_SC_H */
