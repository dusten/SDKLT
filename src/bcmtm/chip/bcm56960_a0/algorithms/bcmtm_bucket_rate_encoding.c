/*! \file bcmtm_bucket_rate_encoding.c
 *
 * algorithms for bucket information to rate conversion or vice versa.
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

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmtm/sched_shaper/bcmtm_rate_encoding_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define METER_GRANULARITY_DEFAULT       3
#define METER_GRANULARITY_NUM           8
#define METER_KBITS_SEC_QUANTUM_MIN     8       /* 8000 bits/second */
#define METER_BITS_BURST_MIN            (256 * 8) /* 256 bytes */
#define METER_PACKET_SEC_QUANTUM_MIN    1
#define METER_MMU_PACKET_BURST_MIN      256     /* 0.256 packet, use 1000x */
#define METER_FP_PACKET_BURST_MIN       512     /* 0.512 packet, use 1000x */
#define METER_PACKET_BURST_DIVISOR      1000    /* use 1000x for calculation */
#define METER_NL_SEGMENT_GRANULARITY    256
#define METER_NL_BUCKET_POWER_SHIFT     8
#define METER_NL_BUCKET_RANGE_NUM       16
#define METER_NL_BUCKET_SEGMENT_MASK    0xff
#define METER_NL_BUCKET_POWER_MASK      0xf
#define METER_NL_BUCKET_MAC_ENCODE_MAX  0xff80 /* 2 ** 0xf * (1 + 0xff/256) */

#define BCMTM_MIN(a, b) \
do{ \
    if(a > b) {\
        a = b;\
    }\
}while(0);

/*******************************************************************************
 * Private functions
 */
static
void bcmtm_granularity_params(int unit, int granularity, uint8_t shaping_mode,
                    uint32_t *rate_unit_size, uint32_t *burst_unit_size)
{
    uint32_t gran_multiple;
    static const uint32_t mmu_pkt_mode_gran_multiple[] = {
        1, 2, 4, 8, 16, 64, 256, 1024
    };

    if (shaping_mode == 0) /*packet mode*/{
        gran_multiple = mmu_pkt_mode_gran_multiple[granularity];
        *burst_unit_size = METER_MMU_PACKET_BURST_MIN * gran_multiple;
        *rate_unit_size = METER_PACKET_SEC_QUANTUM_MIN * gran_multiple;
    } else if (shaping_mode == 1)/*byte mode*/{
        gran_multiple = 1 << granularity;
        *rate_unit_size = METER_KBITS_SEC_QUANTUM_MIN * gran_multiple;
        *burst_unit_size = METER_BITS_BURST_MIN * gran_multiple;
    }
}

static
int bcmtm_rate_to_bucket_encoding( int unit, int port,
                                uint32_t refresh_bitsize,
                                uint32_t bucket_bitsize,
                                uint32_t itu_mode,
                                uint32_t bandwidth, uint32_t burst_size,
                                uint8_t shaping_mode, uint32_t *refresh_rate,
                                uint32_t *granularity, uint32_t *bucket_size)
{
    uint32_t refresh_mask = 0;
    uint32_t bucket_mask = 0;
    uint32_t refresh_max = 0;
    uint32_t bucket_max = 0;
    uint32_t rate_unit_size = 1;
    uint32_t burst_unit_size = 0;
    uint32_t burst = 0;
    uint32_t buckettop, encoding;
    int gran = 0;
    int bucket_segment_size, i = 0;

    SHR_FUNC_ENTER(unit);

    refresh_mask = 0xffffffff >> (32 - refresh_bitsize);
    bucket_mask = 0xffffffff >> (32 - bucket_bitsize);

    if ((burst_size == 0) && (bandwidth == 0)){
        *refresh_rate = 0;
        *bucket_size = 0;
        *granularity = METER_GRANULARITY_DEFAULT;
        goto exit;
    }
    if (shaping_mode == 0 ) /*packet mode*/ {
        burst = burst_size * METER_PACKET_BURST_DIVISOR;
    } else if (shaping_mode == 1) /*byte mode*/ {
        if (burst_size > (0xffffffff / 1000)) {
            burst = 0xffffffff;
        } else {
            burst = burst_size * 1000;
        }
    }
    for (gran = 0; gran <= METER_GRANULARITY_NUM -1; gran++) {
        bcmtm_granularity_params(unit, gran, shaping_mode, &rate_unit_size,
                &burst_unit_size);
        refresh_max = refresh_mask * rate_unit_size;
        if (itu_mode) {
            bucket_max = METER_NL_BUCKET_MAC_ENCODE_MAX * burst_unit_size;
        } else {
            bucket_max = (bucket_mask + 1) * burst_unit_size;
        }
        if ((bandwidth <= refresh_max) && (burst <= bucket_max)) {
            break;
        }
    }
    if (gran > METER_GRANULARITY_NUM -1) {
        /* Saturate! */
        gran = METER_GRANULARITY_NUM -1;
        BCMTM_MIN(bandwidth, refresh_max);
        BCMTM_MIN(burst, bucket_max);
    }
    *granularity = gran;
    if (bandwidth > (0xffffffff - (rate_unit_size - 1))) {
        bandwidth = 0xffffffff - (rate_unit_size - 1);
    }

    *refresh_rate = (bandwidth + (rate_unit_size - 1)) / rate_unit_size;
    BCMTM_MIN(*refresh_rate, refresh_mask);

    if (itu_mode)/*itu_mode enable non linear mode*/ {
        if (burst <= burst_unit_size) {
            *bucket_size = burst ? 1 : 0; /* 0 kbits means disable */
        } else {
            encoding = bucket_mask;
            buckettop = burst_unit_size;
            for (i = 0; i < METER_NL_BUCKET_RANGE_NUM; i++, buckettop *= 2) {
                bucket_segment_size =
                    buckettop / METER_NL_SEGMENT_GRANULARITY;
                if (burst <= (buckettop * 2 - bucket_segment_size)) {
                    encoding =
                        (burst - buckettop + (bucket_segment_size - 1)) /
                        bucket_segment_size;
                    encoding |= i << METER_NL_BUCKET_POWER_SHIFT;
                    break;
                }
            }
            *bucket_size = (i < METER_NL_BUCKET_RANGE_NUM) ? encoding :
                bucket_mask;
        }
    } else {
        encoding = (burst + (burst_unit_size - 1)) / burst_unit_size;
        if (encoding > bucket_mask) {
            encoding = bucket_mask;
        }
        *bucket_size = encoding;
    }
exit:
    SHR_FUNC_EXIT();
}

static
int bcmtm_default_burst_size(int unit, int port, uint32_t bandwidth)
{
    int proposed1, proposed2, proposed;
    uint32_t max_port_speed = 100000; /*this comes from port configuration*/
    int mtu = 9216; /*this should come from the configuration*/
    int numq = 8; /*this is based on port type highgig/hsp ports */

    SHR_FUNC_ENTER(unit);
    /* based on port id get information from the port module
     max_port_speed = bcmtm_dummy_max_port_speed(unit, port);
     based on port info get the number of queues available
     numq = bcmtm_dummy_port_numq(unit, port);*/

    max_port_speed *= 1000;
    if (max_port_speed <= 0)
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);

    if (max_port_speed > bandwidth) {
        proposed1 = (mtu * numq)/(max_port_speed/bandwidth);
    } else {
        if (((max_port_speed + bandwidth - 1)/bandwidth) == 0) {
            return 0;
        }
        proposed1 = (mtu * numq)/((max_port_speed + bandwidth- 1)/bandwidth);
    }

    proposed2 = bandwidth /(8 * 256);
    proposed = (proposed1 > proposed2) ? proposed1 : proposed2;
    proposed = (proposed * 8)/1000;
    return (proposed > 0) ? proposed : 1;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int bcmtm_rate_to_bucket_set(int unit, int port,
                     uint32_t refresh_bitsize, uint32_t bucket_bitsize,
                     int itu_mode, uint32_t bandwidth, uint32_t burst_size,
                     uint8_t auto_update, uint32_t shaping_mode,
                     uint32_t *refresh_rate, uint32_t *granularity,
                     uint32_t *bucket_size)
{
    SHR_FUNC_ENTER(unit);

    /*calculate the burst rate possible based on the bandwidth allocated */
    if (auto_update){
        /* based on auto update variable take the decision and
           calculate the refresh_rate, granularity and bucket_size */
        burst_size = (bandwidth > 0)?
            bcmtm_default_burst_size(unit, port, bandwidth) : 0;
    }
    SHR_IF_ERR_EXIT(
        bcmtm_rate_to_bucket_encoding(unit, port, refresh_bitsize,
                                  bucket_bitsize, itu_mode,
                                  bandwidth, burst_size,
                                  shaping_mode, refresh_rate,
                                  granularity, bucket_size));
exit:
    SHR_FUNC_EXIT();
}

int bcmtm_bucket_to_rate_get(int unit, int port,
                                  uint32_t itu_mode, uint8_t shaping_mode,
                                  uint32_t refresh_rate, uint32_t bucket,
                                  uint32_t granularity,
                                  uint32_t *bandwidth, uint32_t  *burst)
{
    uint32_t rate_unit_size = 0;
    uint32_t burst_unit_size = 0;
    uint32_t power, segment, bucketsize;

    SHR_FUNC_ENTER(unit);

    bcmtm_granularity_params(unit, granularity, shaping_mode, &rate_unit_size,
            &burst_unit_size);

    *bandwidth = refresh_rate * rate_unit_size;

    if (itu_mode) {
        if (bucket == 0) { /* 0 means disabled */
            bucketsize = 0;
        } else {
            segment = bucket & METER_NL_BUCKET_SEGMENT_MASK;
            power = (bucket >> METER_NL_BUCKET_POWER_SHIFT) &
                METER_NL_BUCKET_POWER_MASK;

            /* Calculate raw bits */
            bucketsize = (1 << power) *
                (burst_unit_size / METER_NL_SEGMENT_GRANULARITY) *
                (METER_NL_SEGMENT_GRANULARITY + segment);
        }
    } else {
        bucketsize = bucket * burst_unit_size;
    }

    if (shaping_mode == 0) {
        *burst = bucketsize / METER_PACKET_BURST_DIVISOR;
    } else {
        *burst = bucketsize / 1000;
    }
    SHR_FUNC_EXIT();
}
