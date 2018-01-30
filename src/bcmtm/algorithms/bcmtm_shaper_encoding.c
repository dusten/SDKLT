/*! \file bcmtm_shaper_encoding.c
 *
 * TM Shaper rate encoding.
 * This converts the user readable values for bandwidth and burst size to
 * physical table readable format.
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

#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
#include <bcmtm/bcmtm_shaper_encoding_internal.h>
/*******************************************************************************
 * Local definitions
 */
#define BCMTM_METER_GRANULARITY_DEFAULT       3
#define BCMTM_METER_GRANULARITY_NUM           8
/*! 8000 bits/second */
#define BCMTM_METER_KBITS_SEC_QUANTUM_MIN     8
/*! 256 bytes */
#define BCMTM_METER_BITS_BURST_MIN            (256 * 8)
#define BCMTM_METER_PACKET_SEC_QUANTUM_MIN    1
/*! 0.256 packet, use 1000x */
#define BCMTM_METER_MMU_PACKET_BURST_MIN      256
/*! 0.512 packet, use 1000x */
#define BCMTM_METER_FP_PACKET_BURST_MIN       512
/*! use 1000x for calculation */
#define BCMTM_METER_PACKET_BURST_DIVISOR      1000
#define BCMTM_METER_NL_SEGMENT_GRANULARITY    256
#define BCMTM_METER_NL_BUCKET_POWER_SHIFT     8
#define BCMTM_METER_NL_BUCKET_RANGE_NUM       16
#define BCMTM_METER_NL_BUCKET_SEGMENT_MASK    0xff
#define BCMTM_METER_NL_BUCKET_POWER_MASK      0xf
/* 2 ** 0xf * (1 + 0xff/256) */
#define BCMTM_METER_NL_BUCKET_MAC_ENCODE_MAX  0xff80
#define BCMTM_REFRESH_MASK_SZ   14
#define BCMTM_BUCKET_MASK_SZ    20

/* Select the minimum */
#define BCMTM_MIN_SELECT(a, b) (a > b ? (a = b) : (a = a))

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief TM  rate unit size and burst unit size based on shaping mode.
 *
 * \param [in] unit           Unit number.
 * \param [in] granularity    Granularity.
 * \param [in] shaping_mode   Shaping mode(packet mode/ byte mode).
 * \param [out] rate_unit_sz  Rate unit size.
 * \param [out] burst_unit_sz Burst unit size.
 */
static void
bcmtm_granularity_params(int unit,
        int granularity,
        uint32_t shaping_mode,
        uint32_t *rate_unit_sz,
        uint32_t *burst_unit_sz)
{
    uint32_t gran_multiple;
    static const uint32_t bcmtm_pkt_mode_gran_multiple[] = {
        1, 2, 4, 8, 16, 64, 256, 1024
    };

    if (shaping_mode != 0) {
        /* packet mode */
        gran_multiple = bcmtm_pkt_mode_gran_multiple[granularity];
        *burst_unit_sz = BCMTM_METER_MMU_PACKET_BURST_MIN * gran_multiple;
        *rate_unit_sz = BCMTM_METER_PACKET_SEC_QUANTUM_MIN * gran_multiple;
    } else {
        /* Byte mode */
        gran_multiple = 1 << granularity;
        *rate_unit_sz =  BCMTM_METER_KBITS_SEC_QUANTUM_MIN * gran_multiple;
        *burst_unit_sz = BCMTM_METER_BITS_BURST_MIN * gran_multiple;
    }
}

/*!
 * \brief TM calculate default burst size based on bandwidth.
 *
 * \param [in] unit       Unit number.
 * \param [in] lport      Logical port number.
 * \param [in] bandwidth  Bandwidth in Kbps.
 *
 * return Burst size in kbits.
 */
static int
bcmtm_default_burst_size(int unit,
        bcmtm_lport_t lport,
        uint32_t bandwidth)
{
#define BCMTM_PORT_MTU    9216
    uint32_t max_port_speed;
    bcmtm_drv_info_t *drv_info;
    int proposed1, proposed2, proposed;
    int numq;

    bcmtm_drv_info_get(unit, &drv_info);
    max_port_speed = drv_info->port_speed_max[lport] * 1000;
    
    numq = 8;
    if (max_port_speed == 0)
        return 0;
    if (max_port_speed > bandwidth) {
        proposed1 = (BCMTM_PORT_MTU * numq)/(max_port_speed/bandwidth);
    } else {
        if (((max_port_speed + bandwidth - 1) / bandwidth) == 0){
            return 0;
        }
        proposed1 = (BCMTM_PORT_MTU * numq)/
                    ((max_port_speed + bandwidth - 1)/bandwidth);
    }
    proposed2 = bandwidth / (8 * 256);

    proposed = (proposed1 > proposed2) ? proposed1 : proposed2;
    proposed = (proposed * 8)/1000;

    return (proposed > 0) ? proposed : 1;
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_shaper_bucket_to_rate(int unit,
        uint32_t shaping_mode,
        bcmtm_shaper_bucket_encode_t *bucket_encode)
{
    uint32_t rate_unit_sz = 0, burst_unit_sz = 0;
    uint32_t power, segment, bucket_sz;
    uint64_t itu_mode;

    SHR_FUNC_ENTER(unit);
    bcmtm_granularity_params(unit, bucket_encode->granularity, shaping_mode,
                &rate_unit_sz, &burst_unit_sz);

    bucket_encode->bandwidth = (bucket_encode->refresh_rate) * rate_unit_sz;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, TM_SHAPER_CONFIGt,
                          TM_SHAPER_CONFIGt_ITU_MODEf, &itu_mode));
    if (itu_mode) {
        if (bucket_encode->bucket_sz == 0) {
            bucket_sz = 0;
        } else {
            segment =
                bucket_encode->bucket_sz & BCMTM_METER_NL_BUCKET_SEGMENT_MASK;
            power =
                (bucket_encode->bucket_sz >> BCMTM_METER_NL_BUCKET_POWER_SHIFT)
                & BCMTM_METER_NL_BUCKET_POWER_MASK;
            /* Calculate raw bits */
            bucket_sz = (1 << power) *
                (burst_unit_sz / BCMTM_METER_NL_SEGMENT_GRANULARITY) *
                (BCMTM_METER_NL_SEGMENT_GRANULARITY + segment);
        }
    } else {
        bucket_sz = bucket_encode->bucket_sz * burst_unit_sz;
    }
    if (shaping_mode != 0) {
        /* packet mode */
        bucket_encode->burst = bucket_sz / BCMTM_METER_PACKET_BURST_DIVISOR;
    } else {
        bucket_encode->burst = bucket_sz / 1000;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_shaper_rate_to_bucket(int unit,
        bcmtm_lport_t lport,
        uint32_t shaping_mode,
        uint8_t burst_auto,
        bcmtm_shaper_bucket_encode_t *bucket_encode)
{
    uint32_t refresh_mask = 0, bucket_mask = 0;
    uint32_t refresh_max = 0, bucket_max = 0;
    uint32_t refresh_unit_sz = 0, burst_unit_sz = 0;
    uint64_t itu_mode;
    uint32_t burst, gran, bucket_top, encoding;
    int bucket_segment_size, i = 0;
    uint32_t bandwidth = bucket_encode->bandwidth;
    uint32_t burst_size = bucket_encode->burst;

    SHR_FUNC_ENTER(unit);
    refresh_mask = 0xffffffff >> BCMTM_REFRESH_MASK_SZ;
    bucket_mask = 0xffffffff >> BCMTM_BUCKET_MASK_SZ;

    /* calculate burst rate based on bandwidth allocated */
    if (burst_auto) {
        burst_size = (bandwidth > 0) ?
            bcmtm_default_burst_size(unit, lport, bandwidth) : 0 ;
    }

    if ((burst_size  == 0) && (bandwidth == 0)) {
        bucket_encode->refresh_rate = 0;
        bucket_encode->bucket_sz = 0;
        bucket_encode->granularity = BCMTM_METER_GRANULARITY_DEFAULT;
    }

    /* Packet mode */
    if (shaping_mode != 0) {
        burst = burst_size * BCMTM_METER_PACKET_BURST_DIVISOR;
    } else {
        /* Byte mode */
        if (burst_size > (0xffffffff / 1000))
            burst = 0xffffffff;
        else
            burst = burst_size * 1000;
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, TM_SHAPER_CONFIGt,
                          TM_SHAPER_CONFIGt_ITU_MODEf, &itu_mode));
    /* granularity */
    for (gran = 0; gran <= BCMTM_METER_GRANULARITY_NUM - 1; gran++) {
        bcmtm_granularity_params(unit, gran, shaping_mode,
                &refresh_unit_sz, &burst_unit_sz);
        refresh_max = refresh_mask * refresh_unit_sz;
        if (itu_mode)
            bucket_max = BCMTM_METER_NL_BUCKET_MAC_ENCODE_MAX * burst_unit_sz;
        else
            bucket_max = (bucket_mask + 1) * burst_unit_sz;

        if ((bandwidth <= refresh_max) && (burst <= bucket_max)) {
            break;
        }
    }
    if (gran > BCMTM_METER_GRANULARITY_NUM - 1) {
        /* Saturate */
        gran = BCMTM_METER_GRANULARITY_NUM - 1;
        BCMTM_MIN_SELECT(bandwidth, refresh_max);
        BCMTM_MIN_SELECT(burst, bucket_max);
    }
    bucket_encode->granularity = gran;

    if (bandwidth > (0xffffffff - (refresh_unit_sz - 1)))
        bandwidth = 0xffffffff - (refresh_unit_sz - 1);
    bucket_encode->refresh_rate =
            (bandwidth + refresh_unit_sz - 1) / refresh_unit_sz;
    BCMTM_MIN_SELECT(bucket_encode->refresh_rate, refresh_mask);

    if (itu_mode) {
        /* ITU mode enabled. (Non-Linear mode) */
        if (burst <= burst_unit_sz) {
            bucket_encode->bucket_sz = burst ? 1 : 0; /* 0 kb means disable */
        } else {
            encoding = bucket_mask;
            bucket_top = burst_unit_sz;
            for (i = 0; i < BCMTM_METER_NL_BUCKET_RANGE_NUM;
                    i++, bucket_top *= 2) {
                 bucket_segment_size =
                    bucket_top / BCMTM_METER_NL_SEGMENT_GRANULARITY;
                if (burst <= (bucket_top * 2 - bucket_segment_size)) {
                    encoding =
                        (burst - bucket_top + (bucket_segment_size - 1)) /
                        bucket_segment_size;
                    encoding |= i << BCMTM_METER_NL_BUCKET_POWER_SHIFT;
                    break;
                }
            }
            bucket_encode->bucket_sz =
                (i < BCMTM_METER_NL_BUCKET_RANGE_NUM) ? encoding : bucket_mask;
        }
    } else {
        encoding = (burst + (burst_unit_sz - 1)) / burst_unit_sz;
        if (encoding > bucket_mask) {
            encoding = bucket_mask;
        }
        bucket_encode->bucket_sz = encoding;
    }
exit:
    SHR_FUNC_EXIT();
}
