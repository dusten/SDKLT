/*! \file bcmptm_bcm56960_a0_rm_alpm.c
 *
 * Chip specific functions for PTRM ALPM
 *
 * This file contains the chip specific functions for PTRM ALPM
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

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56960_a0_defs.h>

#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>

#include "../../rm_alpm/rm_alpm.h"
#include "../../rm_alpm/rm_alpm_level1.h"
#include "../../rm_alpm/rm_alpm_leveln.h"
#include "../../rm_alpm/rm_alpm_bucket.h"
#include "../../rm_alpm/rm_alpm_util.h"

#include "bcmptm_bcm56960_a0_rm_alpm.h"
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM

#define UNPAIR_FIELD_SET(_f, _v)      L3_DEFIPm_##_f##_SET(*de, _v)
#define UNPAIR_FIELD_SET2(_f, _v)        \
    L3_DEFIPm_##_f##_SET(*de, &(_v)[0])
#define UNPAIR_URPF_FIELD_SET(_f, _v)  L3_DEFIPm_##_f##_SET(*se, _v)
#define UNPAIR_URPF_FIELD_SET2(_f, _v)   \
    L3_DEFIPm_##_f##_SET(*se, &(_v)[0])

#define UNPAIR_LENGTH(_f)                                         \
    (bcmdrd_pt_field_maxbit(u, L3_DEFIPm, _f) -   \
     bcmdrd_pt_field_minbit(u, L3_DEFIPm, _f) + 1)
#define UNPAIR_MASK(_f)    ((1 << UNPAIR_LENGTH(_f)) - 1)

#define PAIR_FIELD_SET(_f, _v)        L3_DEFIP_PAIR_128m_##_f##_SET(*de, _v)
#define PAIR_FIELD_SET2(_f, _v)   \
    L3_DEFIP_PAIR_128m_##_f##_SET(*de, &(_v)[0])

#define PAIR_URPF_FIELD_SET(_f, _v)   L3_DEFIP_PAIR_128m_##_f##_SET(*se, _v)
#define PAIR_LENGTH(_f)                                    \
    (bcmdrd_pt_field_maxbit(u, L3_DEFIP_PAIR_128m, _f) -   \
     bcmdrd_pt_field_minbit(u, L3_DEFIP_PAIR_128m, _f) + 1)
#define PAIR_MASK(_f)    ((1 << PAIR_LENGTH(_f)) - 1)

#define UNPAIR_FIELD_GET(_f, _v)     _v = L3_DEFIPm_##_f##_GET(*de)
#define UNPAIR_FIELD_ASSERT(_f, _v)  \
    ALPM_ASSERT((uint32_t)(_v) == L3_DEFIPm_##_f##_GET(*de))
#define UNPAIR_FIELD_GET2(_f, _v)  \
    L3_DEFIPm_##_f##_GET(*de, &(_v)[0])
#define UNPAIR_URPF_FIELD_GET(_f, _v) _v = L3_DEFIPm_##_f##_GET(*se)
#define UNPAIR_URPF_FIELD_GET2(_f, _v)  \
    L3_DEFIPm_##_f##_GET(*se, &(_v)[0])

#define PAIR_FIELD_GET(_f, _v)     _v = L3_DEFIP_PAIR_128m_##_f##_GET(*de)
#define PAIR_FIELD_ASSERT(_f, _v)  \
    ALPM_ASSERT((uint32_t)(_v) == L3_DEFIP_PAIR_128m_##_f##_GET(*de))
#define PAIR_FIELD_GET2(_f, _v)   \
    L3_DEFIP_PAIR_128m_##_f##_GET(*de, &(_v)[0])
#define PAIR_URPF_FIELD_GET(_f, _v) _v = L3_DEFIP_PAIR_128m_##_f##_GET(*se)

#define ROUTE_FMT_FULL_FIELD_SET(_n, _f, _v)  \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_SET(*bucket, (_v))
#define ROUTE_FMT_FULL_FIELD_GET(_n, _f, _v)  \
    (_v) = L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_GET(*bucket)
#define ROUTE_FMT_FULL_FIELD_SET2(_n, _f, _v)  \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_SET(*bucket, &(_v)[0])
#define ROUTE_FMT_FULL_FIELD_GET2(_n, _f, _v)  \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_GET(*bucket, &(_v)[0])
#define ROUTE_FMT_FIELD_SET(_n, _f, _v)    \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_SET(*format_buf, (_v))
#define ROUTE_FMT_FIELD_GET(_n, _f, _v)    \
    (_v) = L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_GET(*format_buf)
#define ROUTE_FMT_FIELD_SET2(_n, _f, _v)    \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_SET(*format_buf, &(_v)[0])
#define ROUTE_FMT_FIELD_GET2(_n, _f, _v)    \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_GET(*format_buf, &(_v)[0])

#define FMT_ASSOC_DATA_FIELD_SET(_e, _t, _f, _v)  \
    L3_DEFIP_ALPM_ASSOC_DATA_##_t##_##_f##_SET(*_e, (_v))
#define FMT_ASSOC_DATA_FIELD_GET(_e, _t, _f, _v)  \
    (_v) = L3_DEFIP_ALPM_ASSOC_DATA_##_t##_##_f##_GET(*_e)

#define L2_FIELD_GET(_f, _v)
#define L2_URPF_FIELD_GET(_f, _v)
#define L2_FIELD_SET(_f, _v)
#define L2_URPF_FIELD_SET(_f, _v)

#define ALPM_MAX_VRF 2047
#define ENTRY_TYPE_MASK 0x1
#define MODE_MASK 0x3

#define MODE_IPV6_128   3
#define MODE_IPV6_64    1
#define MODE_IPV4       0
/*******************************************************************************
 * Typedefs
 */

typedef struct format_info_s {
    uint8_t prefix_len;
    uint8_t full;
    uint8_t num_entries;
} format_info_t;

typedef union bcm56960_assoc_data_s {
    L3_DEFIP_ALPM_ASSOC_DATA_FULL_t full;
    L3_DEFIP_ALPM_ASSOC_DATA_REDUCED_t red;
    uint32_t v[2];
} bcm56960_assoc_data_t;

typedef union bcm56960_alpm_route_fmt_s {
    L3_DEFIP_ALPM_ROUTE_FMT1_t fmt1;
    L3_DEFIP_ALPM_ROUTE_FMT2_t fmt2;
    L3_DEFIP_ALPM_ROUTE_FMT3_t fmt3;
    L3_DEFIP_ALPM_ROUTE_FMT4_t fmt4;
    L3_DEFIP_ALPM_ROUTE_FMT5_t fmt5;
    uint32_t v[7];
} bcm56960_alpm_route_fmt_t;

/*******************************************************************************
 * Private variables
 */

static format_info_t route_base_entry_info[] = {
      {.prefix_len = 0,   .full = 0, .num_entries = 0}, /* 0 */
      {.prefix_len = 32,  .full = 0, .num_entries = 6}, /* 1 */
      {.prefix_len = 64,  .full = 0, .num_entries = 4}, /* 2 */
      {.prefix_len = 32,  .full = 1, .num_entries = 4}, /* 3 */
      {.prefix_len = 64,  .full = 1, .num_entries = 3}, /* 4 */
      {.prefix_len = 128, .full = 1, .num_entries = 2}  /* 5 */
};

/****** Combined mode ******/
/* Combined profile */
static bkt_hdl_t combined_bkt_hdl_2_of_2 = {
    .max_banks   = 4,
    .bucket_bits = 13,
    .rbkts_total = 32768,
    .rbkts_used  = 0,
    .format_type = FORMAT_ROUTE,
};
/****** Parallel mode ******/
/* Parallel profile */
static bkt_hdl_t parallel_bkt_hdl_2x_of_21 = {
    .max_banks   = 4,
    .bucket_bits = 13,
    .rbkts_total = 32768,
    .rbkts_used  = 0,
    .format_type = FORMAT_ROUTE,
};
/* Parallel profile */
static bkt_hdl_t parallel_bkt_hdl_2x_of_22 = {
    .max_banks   = 2,
    .bucket_bits = 13,
    .rbkts_total = 16384,
    .rbkts_used  = 0,
    .format_type = FORMAT_ROUTE,
};
static bkt_hdl_t parallel_bkt_hdl_x2_of_22 = {
    .max_banks   = 2,
    .bucket_bits = 13,
    .rbkts_total = 16384,
    .rbkts_used  = 0,
    .format_type = FORMAT_ROUTE,
};

static L3_DEFIP_PAIR_128m_t wide_private_template;
static L3_DEFIP_PAIR_128m_t wide_global_template;
static L3_DEFIP_PAIR_128m_t wide_override_template;
static L3_DEFIPm_t narrow_private_template;
static L3_DEFIPm_t narrow_global_template;
static L3_DEFIPm_t narrow_override_template;

/*******************************************************************************
 * Private Functions
 */
static int
bcm56960_bkt_hdl_combined(int u,
                          int db0_max_lvls,
                          int db,
                          int lvl,
                          bkt_hdl_t **bkt_hdl)
{
    ALPM_ASSERT(db0_max_lvls == 2 && lvl == LEVEL2);
    switch (db0_max_lvls) {
    case 2:
        *bkt_hdl = &combined_bkt_hdl_2_of_2;
        break;
    default:
        ALPM_ASSERT(0);
    }
    return SHR_E_NONE;
}


static int
bcm56960_bkt_hdl_parallel(int u,
                          int db0_max_lvls,
                          int db1_max_lvls,
                          int db,
                          int lvl,
                          bkt_hdl_t **bkt_hdl)
{
    ALPM_ASSERT(db1_max_lvls <= db0_max_lvls);
    ALPM_ASSERT(db0_max_lvls == 2 && lvl == LEVEL2);
    switch (db0_max_lvls) {
    case 2:
        switch (db1_max_lvls) {
        case 1: /* (2 1) */
            *bkt_hdl = &parallel_bkt_hdl_2x_of_21;
            break;
        case 2: /* (2 2) */
            *bkt_hdl =
                (db == DB0) ? &parallel_bkt_hdl_2x_of_22
                            : &parallel_bkt_hdl_x2_of_22;
            break;
        default:
            ALPM_ASSERT(0);
            break;
        }
        break;
    default:
        ALPM_ASSERT(0);
    }
    return SHR_E_NONE;
}

static int
bcm56960_bkt_hdl_profile(int u,
                         int alpm_mode,
                         int db0_max_lvls,
                         int db1_max_lvls,
                         int db,
                         int lvl,
                         void **bkt_hdl)
{
    bkt_hdl_t *bkt_hdl_profile = NULL;
    if (alpm_mode == ALPM_MODE_COMBINED) {
        bcm56960_bkt_hdl_combined(u, db0_max_lvls, db, lvl, &bkt_hdl_profile);
    } else {
        bcm56960_bkt_hdl_parallel(u, db0_max_lvls, db1_max_lvls,
                                  db, lvl, &bkt_hdl_profile);
    }
    *bkt_hdl = bkt_hdl_profile;
    return SHR_E_NONE;
}

/*!
 * \brief Encode arg into second half of narrow entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return nothing.
 */
static void
bcm56960_l1_narrow_encode_n1(int u, alpm_arg_t *arg, L3_DEFIPm_t *de,
                           L3_DEFIPm_t *se)
{
    /* Do not zero buffers. */

    UNPAIR_FIELD_ASSERT(ENTRY_TYPE1f, 0);
    UNPAIR_FIELD_ASSERT(ENTRY_TYPE_MASK1f, ENTRY_TYPE_MASK);
    UNPAIR_FIELD_ASSERT(MODE_MASK1f, MODE_MASK);

    UNPAIR_FIELD_SET(MODE1f, arg->key.t.ipv);
    UNPAIR_FIELD_SET(VALID1f, arg->valid);
    if (arg->key.vt == VRF_PRIVATE) {
        UNPAIR_FIELD_SET(VRF_ID_1f, arg->key.t.w_vrf);
    }

#ifdef ALPM_DEBUG
    if (arg->key.vt == VRF_OVERRIDE) {
        UNPAIR_FIELD_ASSERT(GLOBAL_HIGH1f, 0x1);
        UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_1f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_MASK1f, 0);
    } else if (arg->key.vt == VRF_GLOBAL) {
        UNPAIR_FIELD_ASSERT(GLOBAL_HIGH1f, 0);
        UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, 0x1);
        UNPAIR_FIELD_ASSERT(VRF_ID_1f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_MASK1f, 0);
    } else {
        UNPAIR_FIELD_ASSERT(GLOBAL_HIGH1f, 0);
        UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_1f, arg->key.t.w_vrf);
        UNPAIR_FIELD_ASSERT(VRF_ID_MASK1f, ALPM_MAX_VRF);
    }
#endif
    UNPAIR_FIELD_SET(IP_ADDR1f, arg->key.ip_addr[0]);
    UNPAIR_FIELD_SET(IP_ADDR_MASK1f, arg->key.ip_mask[0]);

    /* Do not set GLOBAL_HIGH field, assuming both halves are same. */
    UNPAIR_FIELD_SET2(REPLACE_DATA1f, arg->ad.fmt_ad);
    /* Placed after replace data */
    UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, arg->key.vt == VRF_GLOBAL);

    UNPAIR_FIELD_SET(HIT1f, arg->hit);
    UNPAIR_FIELD_SET(ENTRY_VIEW1f, arg->ad.flex_stat_valid);
    if (!arg->a1.direct_route) { /* Multi-levels */
        UNPAIR_FIELD_SET(ALG_BKT_PTR1f, arg->a1.phy_bkt);
        UNPAIR_FIELD_SET(ALG_HIT_IDX1f, arg->a1.hit_idx);
        UNPAIR_FIELD_SET(ALG_SUB_BKT_PTR1f, arg->a1.sub_bkt);
    }

    if (bcmptm_rm_alpm_is_urpf(u)) {
        sal_memcpy(se, de, bcmdrd_pt_entry_size(u, L3_DEFIPm));
        UNPAIR_URPF_FIELD_SET(SRC_DISCARD1f, arg->ad.src_discard);
        UNPAIR_URPF_FIELD_SET(DEFAULTROUTE1f, arg->default_route);
    }
}


/*!
 * \brief Encode arg into narrow entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return nothing.
 */
static void
bcm56960_l1_narrow_encode(int u, alpm_arg_t *arg, L3_DEFIPm_t *de,
                           L3_DEFIPm_t *se)
{

    /* Do not zero buffers. */

    UNPAIR_FIELD_ASSERT(ENTRY_TYPE0f, 0);
    UNPAIR_FIELD_ASSERT(ENTRY_TYPE_MASK0f, UNPAIR_MASK(ENTRY_TYPE_MASK0f));
    UNPAIR_FIELD_SET(VALID0f, arg->valid);
    if (arg->key.vt == VRF_PRIVATE) {
        UNPAIR_FIELD_SET(VRF_ID_0f, arg->key.t.w_vrf);
    }

#ifdef ALPM_DEBUG
    if (arg->key.vt == VRF_OVERRIDE) {
        UNPAIR_FIELD_ASSERT(GLOBAL_HIGH0f, 0x1);
        UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE0f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_0f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_MASK0f, 0);
    } else if (arg->key.vt == VRF_GLOBAL) {
        UNPAIR_FIELD_ASSERT(GLOBAL_HIGH0f, 0);
        UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE0f, 0x1);
        UNPAIR_FIELD_ASSERT(VRF_ID_0f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_MASK0f, 0);
    } else {
        UNPAIR_FIELD_ASSERT(GLOBAL_HIGH0f, 0);
        UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE0f, 0);
        UNPAIR_FIELD_ASSERT(VRF_ID_0f, arg->key.t.w_vrf);
        UNPAIR_FIELD_ASSERT(VRF_ID_MASK0f, ALPM_MAX_VRF);
    }
#endif

    if (arg->key.t.ipv == IP_VER_4) {
        UNPAIR_FIELD_SET(IP_ADDR0f, arg->key.ip_addr[0]);
        UNPAIR_FIELD_SET(IP_ADDR_MASK0f, arg->key.ip_mask[0]);
        UNPAIR_FIELD_SET(MODE0f, MODE_IPV4);
        UNPAIR_FIELD_ASSERT(MODE_MASK0f, MODE_MASK);
    } else {
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);

        UNPAIR_FIELD_SET(IP_ADDR0f, arg->key.ip_addr[2]);
        UNPAIR_FIELD_SET(IP_ADDR_MASK0f, arg->key.ip_mask[2]);

        UNPAIR_FIELD_SET(IP_ADDR1f, arg->key.ip_addr[3]);
        UNPAIR_FIELD_SET(IP_ADDR_MASK1f, arg->key.ip_mask[3]);
        UNPAIR_FIELD_SET(MODE0f, MODE_IPV6_64);
        UNPAIR_FIELD_ASSERT(MODE_MASK0f, MODE_MASK);
        UNPAIR_FIELD_SET(MODE1f, MODE_IPV6_64);
        UNPAIR_FIELD_ASSERT(MODE_MASK1f, MODE_MASK);
        UNPAIR_FIELD_SET(VALID1f, arg->valid);
        if (arg->key.vt == VRF_PRIVATE) {
            UNPAIR_FIELD_SET(VRF_ID_1f, arg->key.t.w_vrf);
        }

#ifdef ALPM_DEBUG
        UNPAIR_FIELD_ASSERT(ENTRY_TYPE1f, 0);
        UNPAIR_FIELD_ASSERT(ENTRY_TYPE_MASK1f, UNPAIR_MASK(ENTRY_TYPE_MASK1f));
        UNPAIR_FIELD_ASSERT(VALID1f, arg->valid);

        if (arg->key.vt == VRF_OVERRIDE) {
            UNPAIR_FIELD_ASSERT(GLOBAL_HIGH1f, 0x1);
            UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, 0);
            UNPAIR_FIELD_ASSERT(VRF_ID_1f, 0);
            UNPAIR_FIELD_ASSERT(VRF_ID_MASK1f, 0);
        } else if (arg->key.vt == VRF_GLOBAL) {
            UNPAIR_FIELD_ASSERT(GLOBAL_HIGH1f, 0);
            UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, 0x1);
            UNPAIR_FIELD_ASSERT(VRF_ID_1f, 0);
            UNPAIR_FIELD_ASSERT(VRF_ID_MASK1f, 0);
        } else {
            UNPAIR_FIELD_ASSERT(GLOBAL_HIGH1f, 0);
            UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, 0);
            UNPAIR_FIELD_ASSERT(VRF_ID_1f, arg->key.t.w_vrf);
            UNPAIR_FIELD_ASSERT(VRF_ID_MASK1f, ALPM_MAX_VRF);
        }
#endif
    }

    /* Do not set GLOBAL_HIGH field, assuming both halves are same. */
    UNPAIR_FIELD_SET2(REPLACE_DATA0f, arg->ad.fmt_ad);
    /* Placed after replace data */
    UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE0f, arg->key.vt == VRF_GLOBAL);

    UNPAIR_FIELD_SET(HIT0f, arg->hit);
    UNPAIR_FIELD_SET(ENTRY_VIEW0f, arg->ad.flex_stat_valid);
    if (!arg->a1.direct_route) { /* Multi-levels */
        UNPAIR_FIELD_SET(ALG_BKT_PTR0f, arg->a1.phy_bkt);
        UNPAIR_FIELD_SET(ALG_HIT_IDX0f, arg->a1.hit_idx);
        UNPAIR_FIELD_SET(ALG_SUB_BKT_PTR0f, arg->a1.sub_bkt);
    }

    if (arg->key.t.ipv == IP_VER_6) {
        /* Just in case.. */
        UNPAIR_FIELD_SET2(REPLACE_DATA1f, arg->ad.fmt_ad);
        UNPAIR_FIELD_ASSERT(GLOBAL_ROUTE1f, arg->key.vt == VRF_GLOBAL);
        UNPAIR_FIELD_SET(HIT1f, arg->hit);
        UNPAIR_FIELD_SET(ENTRY_VIEW1f, arg->ad.flex_stat_valid);
        if (!arg->a1.direct_route) { /* Multi-levels */
            UNPAIR_FIELD_SET(ALG_BKT_PTR1f, arg->a1.phy_bkt);
            UNPAIR_FIELD_SET(ALG_HIT_IDX1f, arg->a1.hit_idx);
            UNPAIR_FIELD_SET(ALG_SUB_BKT_PTR1f, arg->a1.sub_bkt);
        }
    }

    if (bcmptm_rm_alpm_is_urpf(u)) {
        sal_memcpy(se, de, bcmdrd_pt_entry_size(u, L3_DEFIPm));
        UNPAIR_URPF_FIELD_SET(SRC_DISCARD0f, arg->ad.src_discard);
        UNPAIR_URPF_FIELD_SET(DEFAULTROUTE0f, arg->default_route);
        if (arg->key.t.ipv == IP_VER_6) {
            UNPAIR_URPF_FIELD_SET(SRC_DISCARD1f, arg->ad.src_discard);
            UNPAIR_URPF_FIELD_SET(DEFAULTROUTE1f, arg->default_route);
        }
    }
}

/*!
 * \brief Parse arg from second half part of narrow entry
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return nothing.
 */
static void
bcm56960_l1_narrow_decode_n1(int u, alpm_arg_t *arg,
                             L3_DEFIPm_t *de, L3_DEFIPm_t *se)
{
    int gh, gr;
    UNPAIR_FIELD_GET(MODE1f, arg->key.t.ipv);
    UNPAIR_FIELD_GET(GLOBAL_HIGH1f, gh);
    UNPAIR_FIELD_GET(GLOBAL_ROUTE1f, gr);
    if (gh) {
        arg->key.vt = VRF_OVERRIDE;
        arg->key.t.w_vrf = COMBINED_WVRF_OVERRIDE;
    } else if (gr) {
        arg->key.vt = VRF_GLOBAL;
        if (bcmptm_rm_alpm_mode(u) == ALPM_MODE_COMBINED) {
            arg->key.t.w_vrf = COMBINED_WVRF_GLOBAL(ALPM_MAX_VRF);
        } else {
            arg->key.t.w_vrf = PARALLEL_WVRF_GLOBAL(ALPM_MAX_VRF);
        }
    } else {
        arg->key.vt = VRF_PRIVATE;
        UNPAIR_FIELD_GET(VRF_ID_1f, arg->key.t.w_vrf);
    }

    UNPAIR_FIELD_GET(VALID1f, arg->valid);
    UNPAIR_FIELD_GET(IP_ADDR1f, arg->key.ip_addr[0]);
    UNPAIR_FIELD_GET(IP_ADDR_MASK1f, arg->key.ip_mask[0]);

    UNPAIR_FIELD_GET2(REPLACE_DATA1f, arg->ad.fmt_ad);
    UNPAIR_FIELD_GET(HIT1f, arg->hit);
    UNPAIR_FIELD_GET(ENTRY_VIEW1f, arg->ad.flex_stat_valid);
    UNPAIR_FIELD_GET(ALG_BKT_PTR1f, arg->a1.phy_bkt);
    if (arg->a1.phy_bkt) { /* Multi-levels */
        arg->a1.direct_route = 0;
        UNPAIR_FIELD_GET(ALG_HIT_IDX1f, arg->a1.hit_idx);
        UNPAIR_FIELD_GET(ALG_SUB_BKT_PTR1f, arg->a1.sub_bkt);
    } else {
        arg->a1.direct_route = 1;
    }

    if (bcmptm_rm_alpm_is_urpf(u)) {
        UNPAIR_URPF_FIELD_GET(SRC_DISCARD1f, arg->ad.src_discard);
        UNPAIR_URPF_FIELD_GET(DEFAULTROUTE1f, arg->default_route);
    }
}

/*!
 * \brief Parse arg from narrow entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return nothing.
 */
static void
bcm56960_l1_narrow_decode(int u, alpm_arg_t *arg, L3_DEFIPm_t *de, L3_DEFIPm_t *se)
{
    int gh, gr;
    int valid[2];

    UNPAIR_FIELD_GET(MODE0f, arg->key.t.ipv);
    if (arg->key.t.ipv == IP_VER_4) {
        UNPAIR_FIELD_GET(VALID0f, arg->valid);
        UNPAIR_FIELD_GET(IP_ADDR0f, arg->key.ip_addr[0]);
        UNPAIR_FIELD_GET(IP_ADDR_MASK0f, arg->key.ip_mask[0]);
    } else {
        UNPAIR_FIELD_GET(VALID0f, valid[0]);
        UNPAIR_FIELD_GET(VALID1f, valid[1]);
        arg->valid = valid[0] && valid[1];
        UNPAIR_FIELD_GET(IP_ADDR0f, arg->key.ip_addr[2]);
        UNPAIR_FIELD_GET(IP_ADDR1f, arg->key.ip_addr[3]);
        UNPAIR_FIELD_GET(IP_ADDR_MASK0f, arg->key.ip_mask[2]);
        UNPAIR_FIELD_GET(IP_ADDR_MASK1f, arg->key.ip_mask[3]);
    }
    UNPAIR_FIELD_GET(GLOBAL_HIGH0f, gh);
    UNPAIR_FIELD_GET(GLOBAL_ROUTE0f, gr);
    if (gh) {
        arg->key.vt = VRF_OVERRIDE;
        arg->key.t.w_vrf = COMBINED_WVRF_OVERRIDE;
    } else if (gr) {
        arg->key.vt = VRF_GLOBAL;
        if (bcmptm_rm_alpm_mode(u) == ALPM_MODE_COMBINED) {
            arg->key.t.w_vrf = COMBINED_WVRF_GLOBAL(ALPM_MAX_VRF);
        } else {
            arg->key.t.w_vrf = PARALLEL_WVRF_GLOBAL(ALPM_MAX_VRF);
        }
    } else {
        arg->key.vt = VRF_PRIVATE;
        UNPAIR_FIELD_GET(VRF_ID_0f, arg->key.t.w_vrf);
    }

    UNPAIR_FIELD_GET2(REPLACE_DATA0f, arg->ad.fmt_ad);
    UNPAIR_FIELD_GET(HIT0f, arg->hit);
    UNPAIR_FIELD_GET(ENTRY_VIEW0f, arg->ad.flex_stat_valid);
    UNPAIR_FIELD_GET(ALG_BKT_PTR0f, arg->a1.phy_bkt);
    if (arg->a1.phy_bkt) { /* Multi-levels */
        arg->a1.direct_route = 0;
        UNPAIR_FIELD_GET(ALG_HIT_IDX0f, arg->a1.hit_idx);
        UNPAIR_FIELD_GET(ALG_SUB_BKT_PTR0f, arg->a1.sub_bkt);
    } else {
        arg->a1.direct_route = 1;
    }

    if (arg->key.t.ipv) {
        /* Get hit bit from the second part of the entry. */
        if (!arg->hit) {
            UNPAIR_FIELD_GET(HIT1f, arg->hit);
        }
    }

    if (bcmptm_rm_alpm_is_urpf(u)) {
        UNPAIR_URPF_FIELD_GET(SRC_DISCARD0f, arg->ad.src_discard);
        UNPAIR_URPF_FIELD_GET(DEFAULTROUTE0f, arg->default_route);
    }
}


/*!
 * \brief Encode key part into double-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return nothing
 */
static void
bcm56960_l1_wide_encode(int u, alpm_arg_t *arg, L3_DEFIP_PAIR_128m_t *de,
                        L3_DEFIP_PAIR_128m_t *se)
{

    /* Do not zero buffers. */
#ifdef ALPM_DEBUG
    if (arg->key.vt == VRF_OVERRIDE) {
        PAIR_FIELD_ASSERT(GLOBAL_HIGHf, 0x1);
        PAIR_FIELD_ASSERT(GLOBAL_ROUTEf, 0);

        PAIR_FIELD_ASSERT(VRF_ID_0_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_1_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_0_UPRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_1_UPRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK0_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK1_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK0_UPRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK1_UPRf, 0);
    } else if (arg->key.vt == VRF_GLOBAL) {
        PAIR_FIELD_ASSERT(GLOBAL_HIGHf, 0);
        PAIR_FIELD_ASSERT(GLOBAL_ROUTEf, 0x1);

        PAIR_FIELD_ASSERT(VRF_ID_0_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_1_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_0_UPRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_1_UPRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK0_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK1_LWRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK0_UPRf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK1_UPRf, 0);
    } else {
        PAIR_FIELD_ASSERT(GLOBAL_HIGHf, 0);
        PAIR_FIELD_ASSERT(GLOBAL_ROUTEf, 0);
        PAIR_FIELD_ASSERT(VRF_ID_MASK0_LWRf, ALPM_MAX_VRF);
        PAIR_FIELD_ASSERT(VRF_ID_MASK1_LWRf, ALPM_MAX_VRF);
        PAIR_FIELD_ASSERT(VRF_ID_MASK0_UPRf, ALPM_MAX_VRF);
        PAIR_FIELD_ASSERT(VRF_ID_MASK1_UPRf, ALPM_MAX_VRF);
    }
#endif
    if (arg->key.vt == VRF_PRIVATE) {
        PAIR_FIELD_SET(VRF_ID_0_LWRf, arg->key.t.w_vrf);
        PAIR_FIELD_SET(VRF_ID_1_LWRf, arg->key.t.w_vrf);
        PAIR_FIELD_SET(VRF_ID_0_UPRf, arg->key.t.w_vrf);
        PAIR_FIELD_SET(VRF_ID_1_UPRf, arg->key.t.w_vrf);
    }

    PAIR_FIELD_SET(IP_ADDR0_LWRf, arg->key.ip_addr[0]);
    PAIR_FIELD_SET(IP_ADDR1_LWRf, arg->key.ip_addr[1]);
    PAIR_FIELD_SET(IP_ADDR0_UPRf, arg->key.ip_addr[2]);
    PAIR_FIELD_SET(IP_ADDR1_UPRf, arg->key.ip_addr[3]);

    PAIR_FIELD_SET(IP_ADDR_MASK0_LWRf, arg->key.ip_mask[0]);
    PAIR_FIELD_SET(IP_ADDR_MASK1_LWRf, arg->key.ip_mask[1]);
    PAIR_FIELD_SET(IP_ADDR_MASK0_UPRf, arg->key.ip_mask[2]);
    PAIR_FIELD_SET(IP_ADDR_MASK1_UPRf, arg->key.ip_mask[3]);

    PAIR_FIELD_ASSERT(VALID0_LWRf, arg->valid);
    PAIR_FIELD_ASSERT(VALID0_UPRf, arg->valid);
    PAIR_FIELD_ASSERT(VALID1_LWRf, arg->valid);
    PAIR_FIELD_ASSERT(VALID1_UPRf, arg->valid);

    PAIR_FIELD_ASSERT(MODE0_LWRf, MODE_IPV6_128);
    PAIR_FIELD_ASSERT(MODE0_UPRf, MODE_IPV6_128);
    PAIR_FIELD_ASSERT(MODE1_LWRf, MODE_IPV6_128);
    PAIR_FIELD_ASSERT(MODE1_UPRf, MODE_IPV6_128);
    PAIR_FIELD_ASSERT(MODE_MASK0_LWRf, MODE_MASK);
    PAIR_FIELD_ASSERT(MODE_MASK0_UPRf, MODE_MASK);
    PAIR_FIELD_ASSERT(MODE_MASK1_LWRf, MODE_MASK);
    PAIR_FIELD_ASSERT(MODE_MASK1_UPRf, MODE_MASK);

    PAIR_FIELD_ASSERT(ENTRY_TYPE0_LWRf, 0);
    PAIR_FIELD_ASSERT(ENTRY_TYPE0_UPRf, 0);
    PAIR_FIELD_ASSERT(ENTRY_TYPE1_LWRf, 0);
    PAIR_FIELD_ASSERT(ENTRY_TYPE1_UPRf, 0);

    PAIR_FIELD_ASSERT(ENTRY_TYPE_MASK0_LWRf, ENTRY_TYPE_MASK);
    PAIR_FIELD_ASSERT(ENTRY_TYPE_MASK0_UPRf, ENTRY_TYPE_MASK);
    PAIR_FIELD_ASSERT(ENTRY_TYPE_MASK1_LWRf, ENTRY_TYPE_MASK);
    PAIR_FIELD_ASSERT(ENTRY_TYPE_MASK1_UPRf, ENTRY_TYPE_MASK);

    PAIR_FIELD_SET(REPLACE_DATAf, arg->ad.fmt_ad);
    PAIR_FIELD_ASSERT(GLOBAL_ROUTEf, arg->key.vt == VRF_GLOBAL);

    PAIR_FIELD_SET(HITf, arg->hit);
    PAIR_FIELD_SET(ENTRY_VIEWf, arg->ad.flex_stat_valid);
    if (!arg->a1.direct_route) { /* Multi-levels */
        PAIR_FIELD_SET(ALG_BKT_PTRf, arg->a1.phy_bkt);
        PAIR_FIELD_SET(ALG_SUB_BKT_PTRf, arg->a1.sub_bkt);
        PAIR_FIELD_SET(ALG_HIT_IDXf, arg->a1.hit_idx);
    }

    if (bcmptm_rm_alpm_is_urpf(u)) {
        sal_memcpy(se, de, bcmdrd_pt_entry_size(u, L3_DEFIP_PAIR_128m));
        PAIR_URPF_FIELD_SET(SRC_DISCARDf, arg->ad.src_discard);
        PAIR_URPF_FIELD_SET(DEFAULTROUTEf, arg->default_route);
    }
}

/*!
 * \brief Parse key part from double-wide entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56960_l1_wide_decode(int u, alpm_arg_t *arg,
                        L3_DEFIP_PAIR_128m_t *de,
                        L3_DEFIP_PAIR_128m_t *se)
{

    int gh, gr;
#ifdef ALPM_DEBUG
    uint8_t mode;
    PAIR_FIELD_GET(MODE0_LWRf, mode);
    ALPM_ASSERT(mode == MODE_IPV6_128 || mode == MODE_IPV4);
#endif
    arg->key.t.ipv = IP_VER_6;
    PAIR_FIELD_GET(VALID0_LWRf, arg->valid);
    PAIR_FIELD_ASSERT(VALID1_LWRf, arg->valid);
    PAIR_FIELD_ASSERT(VALID0_UPRf, arg->valid);
    PAIR_FIELD_ASSERT(VALID1_UPRf, arg->valid);

    PAIR_FIELD_GET(IP_ADDR0_LWRf, arg->key.ip_addr[0]);
    PAIR_FIELD_GET(IP_ADDR1_LWRf, arg->key.ip_addr[1]);
    PAIR_FIELD_GET(IP_ADDR0_UPRf, arg->key.ip_addr[2]);
    PAIR_FIELD_GET(IP_ADDR1_UPRf, arg->key.ip_addr[3]);

    PAIR_FIELD_GET(IP_ADDR_MASK0_LWRf, arg->key.ip_mask[0]);
    PAIR_FIELD_GET(IP_ADDR_MASK1_LWRf, arg->key.ip_mask[1]);
    PAIR_FIELD_GET(IP_ADDR_MASK0_UPRf, arg->key.ip_mask[2]);
    PAIR_FIELD_GET(IP_ADDR_MASK1_UPRf, arg->key.ip_mask[3]);

    PAIR_FIELD_GET(GLOBAL_HIGHf, gh);
    PAIR_FIELD_GET(GLOBAL_ROUTEf, gr);
    if (gh) {
        arg->key.vt = VRF_OVERRIDE;
        arg->key.t.w_vrf = COMBINED_WVRF_OVERRIDE;
    } else if (gr) {
        arg->key.vt = VRF_GLOBAL;
        if (bcmptm_rm_alpm_mode(u) == ALPM_MODE_COMBINED) {
            arg->key.t.w_vrf = COMBINED_WVRF_GLOBAL(ALPM_MAX_VRF);
        } else {
            arg->key.t.w_vrf = PARALLEL_WVRF_GLOBAL(ALPM_MAX_VRF);
        }
    } else {
        arg->key.vt = VRF_PRIVATE;
        PAIR_FIELD_GET(VRF_ID_0_LWRf, arg->key.t.w_vrf);
    }

    /* Do not zero buffers. */
    PAIR_FIELD_GET2(REPLACE_DATAf, arg->ad.fmt_ad);
    PAIR_FIELD_GET(HITf, arg->hit);
    PAIR_FIELD_GET(ENTRY_VIEWf, arg->ad.flex_stat_valid);
    PAIR_FIELD_GET(ALG_BKT_PTRf, arg->a1.phy_bkt);
    if (arg->a1.phy_bkt) { /* Multi-levels */
        arg->a1.direct_route = 0;
        PAIR_FIELD_GET(ALG_HIT_IDXf, arg->a1.hit_idx);
        PAIR_FIELD_GET(ALG_SUB_BKT_PTRf, arg->a1.sub_bkt);
    } else {
        arg->a1.direct_route = 1;
    }

    if (bcmptm_rm_alpm_is_urpf(u)) {
        PAIR_URPF_FIELD_GET(SRC_DISCARDf, arg->ad.src_discard);
        PAIR_URPF_FIELD_GET(DEFAULTROUTEf, arg->default_route);
    }
}

static void
bcm56960_l1_wide_template_init(int u)
{
    L3_DEFIP_PAIR_128m_t *de;

    /* Do not zero buffers. */
    de = &wide_override_template;
    PAIR_FIELD_SET(VALID0_LWRf, 1);
    PAIR_FIELD_SET(VALID0_UPRf, 1);
    PAIR_FIELD_SET(VALID1_LWRf, 1);
    PAIR_FIELD_SET(VALID1_UPRf, 1);

    PAIR_FIELD_SET(MODE0_LWRf, 3);
    PAIR_FIELD_SET(MODE0_UPRf, 3);
    PAIR_FIELD_SET(MODE1_LWRf, 3);
    PAIR_FIELD_SET(MODE1_UPRf, 3);

    PAIR_FIELD_SET(MODE_MASK0_LWRf, MODE_MASK);
    PAIR_FIELD_SET(MODE_MASK0_UPRf, MODE_MASK);
    PAIR_FIELD_SET(MODE_MASK1_LWRf, MODE_MASK);
    PAIR_FIELD_SET(MODE_MASK1_UPRf, MODE_MASK);

    PAIR_FIELD_SET(ENTRY_TYPE0_LWRf, 0);
    PAIR_FIELD_SET(ENTRY_TYPE0_UPRf, 0);
    PAIR_FIELD_SET(ENTRY_TYPE1_LWRf, 0);
    PAIR_FIELD_SET(ENTRY_TYPE1_UPRf, 0);

    PAIR_FIELD_SET(ENTRY_TYPE_MASK0_LWRf, ENTRY_TYPE_MASK);
    PAIR_FIELD_SET(ENTRY_TYPE_MASK0_UPRf, ENTRY_TYPE_MASK);
    PAIR_FIELD_SET(ENTRY_TYPE_MASK1_LWRf, ENTRY_TYPE_MASK);
    PAIR_FIELD_SET(ENTRY_TYPE_MASK1_UPRf, ENTRY_TYPE_MASK);

    PAIR_FIELD_SET(GLOBAL_HIGHf, 0x1);
    PAIR_FIELD_SET(GLOBAL_ROUTEf, 0);
    PAIR_FIELD_SET(VRF_ID_0_LWRf, 0);
    PAIR_FIELD_SET(VRF_ID_1_LWRf, 0);
    PAIR_FIELD_SET(VRF_ID_0_UPRf, 0);
    PAIR_FIELD_SET(VRF_ID_1_UPRf, 0);
    PAIR_FIELD_SET(VRF_ID_MASK0_LWRf, 0);
    PAIR_FIELD_SET(VRF_ID_MASK1_LWRf, 0);
    PAIR_FIELD_SET(VRF_ID_MASK0_UPRf, 0);
    PAIR_FIELD_SET(VRF_ID_MASK1_UPRf, 0);

    de = &wide_global_template;
    sal_memcpy(de, &wide_override_template, sizeof(L3_DEFIP_PAIR_128m_t));
    PAIR_FIELD_SET(GLOBAL_HIGHf, 0);
    PAIR_FIELD_SET(GLOBAL_ROUTEf, 0x1);

    de = &wide_private_template;
    sal_memcpy(de, &wide_override_template, sizeof(L3_DEFIP_PAIR_128m_t));
    PAIR_FIELD_SET(GLOBAL_HIGHf, 0);
    PAIR_FIELD_SET(GLOBAL_ROUTEf, 0);
    PAIR_FIELD_SET(VRF_ID_MASK0_LWRf, ALPM_MAX_VRF);
    PAIR_FIELD_SET(VRF_ID_MASK1_LWRf, ALPM_MAX_VRF);
    PAIR_FIELD_SET(VRF_ID_MASK0_UPRf, ALPM_MAX_VRF);
    PAIR_FIELD_SET(VRF_ID_MASK1_UPRf, ALPM_MAX_VRF);

}

static void
bcm56960_l1_narrow_template_init(int u)
{
    L3_DEFIPm_t *de;
    /* Do not zero buffers. */

    de = &narrow_override_template;
    UNPAIR_FIELD_SET(ENTRY_TYPE0f, 0);
    UNPAIR_FIELD_SET(ENTRY_TYPE1f, 0);
    UNPAIR_FIELD_SET(ENTRY_TYPE_MASK0f, ENTRY_TYPE_MASK);
    UNPAIR_FIELD_SET(ENTRY_TYPE_MASK1f, ENTRY_TYPE_MASK);

    UNPAIR_FIELD_SET(MODE_MASK0f, MODE_MASK);
    UNPAIR_FIELD_SET(MODE_MASK1f, MODE_MASK);


    UNPAIR_FIELD_SET(GLOBAL_HIGH0f, 0x1);
    UNPAIR_FIELD_SET(GLOBAL_ROUTE0f, 0);
    UNPAIR_FIELD_SET(VRF_ID_0f, 0);
    UNPAIR_FIELD_SET(VRF_ID_MASK0f, 0);
    UNPAIR_FIELD_SET(GLOBAL_HIGH1f, 0x1);
    UNPAIR_FIELD_SET(GLOBAL_ROUTE1f, 0);
    UNPAIR_FIELD_SET(VRF_ID_1f, 0);
    UNPAIR_FIELD_SET(VRF_ID_MASK1f, 0);

    de = &narrow_global_template;
    sal_memcpy(de, &narrow_override_template, sizeof(L3_DEFIPm_t));
    UNPAIR_FIELD_SET(GLOBAL_HIGH0f, 0);
    UNPAIR_FIELD_SET(GLOBAL_ROUTE0f, 0x1);
    UNPAIR_FIELD_SET(VRF_ID_0f, 0);
    UNPAIR_FIELD_SET(VRF_ID_MASK0f, 0);
    UNPAIR_FIELD_SET(GLOBAL_HIGH1f, 0);
    UNPAIR_FIELD_SET(GLOBAL_ROUTE1f, 0x1);
    UNPAIR_FIELD_SET(VRF_ID_1f, 0);
    UNPAIR_FIELD_SET(VRF_ID_MASK1f, 0);

    de = &narrow_private_template;
    sal_memcpy(de, &narrow_override_template, sizeof(L3_DEFIPm_t));
    UNPAIR_FIELD_SET(GLOBAL_HIGH0f, 0);
    UNPAIR_FIELD_SET(GLOBAL_ROUTE0f, 0);
    UNPAIR_FIELD_SET(VRF_ID_MASK0f, ALPM_MAX_VRF);
    UNPAIR_FIELD_SET(GLOBAL_HIGH1f, 0);
    UNPAIR_FIELD_SET(GLOBAL_ROUTE1f, 0);
    UNPAIR_FIELD_SET(VRF_ID_MASK1f, ALPM_MAX_VRF);
}

static void
bcm56960_l1_replace_data_encode(int u, assoc_data_t *ad)
{
    L3_DEFIP_REPLACE_DATA_t *rep_data = (L3_DEFIP_REPLACE_DATA_t *)ad->fmt_ad;
    L3_DEFIP_REPLACE_DATA_ASSOC_DATA_1f_SET(*rep_data, ad->fmt_ad1);
    L3_DEFIP_REPLACE_DATA_ASSOC_DATA_2f_SET(*rep_data, ad->fmt_ad2);
    L3_DEFIP_REPLACE_DATA_ASSOC_DATA_3f_SET(*rep_data, ad->fmt_ad3);
    L3_DEFIP_REPLACE_DATA_GLOBAL_ROUTEf_SET(*rep_data, ad->global_route);
    L3_DEFIP_REPLACE_DATA_DEFAULT_MISSf_SET(*rep_data, ad->default_miss);
    /* L3_DEFIP_REPLACE_DATA_ALG_HIT_IDXf_GET; */
}

/*!
 * \brief Encode Level-1 entry (both key and data parts).
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56960_l1_encode(int u, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;
    arg->repl_data_encode = 1;
    arg->ad.global_route = (arg->key.vt == VRF_GLOBAL);
    if (arg->repl_data_encode) {
        bcm56960_l1_replace_data_encode(u, &arg->ad);
    }
    if (l1v == L1V_WIDE) {
        bcm56960_l1_wide_encode(u, arg, de, se);
    } else if (l1v == L1V_NARROW_1) {
        bcm56960_l1_narrow_encode_n1(u, arg, de, se);
    } else {
        bcm56960_l1_narrow_encode(u, arg, de, se);
    }
    return SHR_E_NONE;
}

static void
bcm56960_l1_replace_data_decode(int u, assoc_data_t *ad)
{
    L3_DEFIP_REPLACE_DATA_t *rep_data = (L3_DEFIP_REPLACE_DATA_t *)ad->fmt_ad;
    ad->fmt_ad1 = L3_DEFIP_REPLACE_DATA_ASSOC_DATA_1f_GET(*rep_data);
    ad->fmt_ad2 = L3_DEFIP_REPLACE_DATA_ASSOC_DATA_2f_GET(*rep_data);
    ad->fmt_ad3 = L3_DEFIP_REPLACE_DATA_ASSOC_DATA_3f_GET(*rep_data);
    ad->global_route = L3_DEFIP_REPLACE_DATA_GLOBAL_ROUTEf_GET(*rep_data);
    ad->default_miss = L3_DEFIP_REPLACE_DATA_DEFAULT_MISSf_GET(*rep_data);
    /* L3_DEFIP_REPLACE_DATA_ALG_HIT_IDXf_GET; */
}

/*!
 * \brief Encode Level-1 entry (both key and data parts).
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56960_l1_decode(int u, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;
    if (l1v == L1V_WIDE) {
        bcm56960_l1_wide_decode(u, arg, de, se);
    } else if (l1v == L1V_NARROW_1) {
        bcm56960_l1_narrow_decode_n1(u, arg, de, se);
    } else {
        bcm56960_l1_narrow_decode(u, arg, de, se);
    }
    arg->repl_data_decode = 1;
    if (arg->repl_data_decode) {
        bcm56960_l1_replace_data_decode(u, &arg->ad);
    }
    return SHR_E_NONE;
}



/*!
 * \brief Move half part of one entry to half part of another.
 *
 * Source entry and destiation entry can be same. The source half entry
 * will be cleared when done.
 *
 * \param [in] u Device u.
 * \param [in] src Source entry
 * \param [in] dst Destination entry
 * \param [in] x Source half
 * \param [in] y Destination half
 *
 * \return SHR_E_NONE.
 */
static int
bcm56960_l1_half_entry_move(int u, void *src, void *dst, int x, int y)
{
    uint32_t v1, v2, v3, v4, v5, v6;
    uint32_t v21[DEFIP_MAX_PT_FIELD_WORDS];
    uint32_t v22[DEFIP_MAX_PT_FIELD_WORDS];
    uint32_t v23[DEFIP_MAX_PT_FIELD_WORDS];
    L3_DEFIPm_t *se = src;
    L3_DEFIPm_t *de = dst;

    if (x) {
        UNPAIR_URPF_FIELD_GET(VALID1f, v1);
        UNPAIR_URPF_FIELD_GET(ALG_BKT_PTR1f, v2);
        UNPAIR_URPF_FIELD_GET(ALG_SUB_BKT_PTR1f, v3);
        UNPAIR_URPF_FIELD_GET(GLOBAL_HIGH1f, v4);
        UNPAIR_URPF_FIELD_GET(ENTRY_VIEW1f, v5);
        UNPAIR_URPF_FIELD_GET(HIT1f, v6);
        UNPAIR_URPF_FIELD_GET2(KEY1f, v21);
        UNPAIR_URPF_FIELD_GET2(MASK1f, v22);
        UNPAIR_URPF_FIELD_GET2(REPLACE_DATA1f, v23);
    } else {
        UNPAIR_URPF_FIELD_GET(VALID0f, v1);
        UNPAIR_URPF_FIELD_GET(ALG_BKT_PTR0f, v2);
        UNPAIR_URPF_FIELD_GET(ALG_SUB_BKT_PTR0f, v3);
        UNPAIR_URPF_FIELD_GET(GLOBAL_HIGH0f, v4);
        UNPAIR_URPF_FIELD_GET(ENTRY_VIEW0f, v5);
        UNPAIR_URPF_FIELD_GET(HIT0f, v6);
        UNPAIR_URPF_FIELD_GET2(KEY0f, v21);
        UNPAIR_URPF_FIELD_GET2(MASK0f, v22);
        UNPAIR_URPF_FIELD_GET2(REPLACE_DATA0f, v23);
    }

    if (y) {
        UNPAIR_FIELD_SET(VALID1f, v1);
        UNPAIR_FIELD_SET(ALG_BKT_PTR1f, v2);
        UNPAIR_FIELD_SET(ALG_SUB_BKT_PTR1f, v3);
        UNPAIR_FIELD_SET(GLOBAL_HIGH1f, v4);
        UNPAIR_FIELD_SET(ENTRY_VIEW1f, v5);
        UNPAIR_FIELD_SET(HIT1f, v6);
        UNPAIR_FIELD_SET2(KEY1f, v21);
        UNPAIR_FIELD_SET2(MASK1f, v22);
        UNPAIR_FIELD_SET2(REPLACE_DATA1f, v23);
    } else {
        UNPAIR_FIELD_SET(VALID0f, v1);
        UNPAIR_FIELD_SET(ALG_BKT_PTR0f, v2);
        UNPAIR_FIELD_SET(ALG_SUB_BKT_PTR0f, v3);
        UNPAIR_FIELD_SET(GLOBAL_HIGH0f, v4);
        UNPAIR_FIELD_SET(ENTRY_VIEW0f, v5);
        UNPAIR_FIELD_SET(HIT0f, v6);
        UNPAIR_FIELD_SET2(KEY0f, v21);
        UNPAIR_FIELD_SET2(MASK0f, v22);
        UNPAIR_FIELD_SET2(REPLACE_DATA0f, v23);
    }

    /* Clear source */
    if (x) {
        UNPAIR_URPF_FIELD_SET(VALID1f, 0);
    } else {
        UNPAIR_URPF_FIELD_SET(VALID0f, 0);
    }

    return SHR_E_NONE;
}

static void
bcm56960_ln_assoc_data_format_encode(int u, bcm56960_assoc_data_t *buf,
                                     int full,
                                     assoc_data_t *ad)
{
    if (full) {
        L3_DEFIP_ALPM_ASSOC_DATA_FULL_t *full_buf = &buf->full;
        FMT_ASSOC_DATA_FIELD_SET(full_buf, FULL, ASSOC_DATA_1f, ad->fmt_ad1);
        FMT_ASSOC_DATA_FIELD_SET(full_buf, FULL, ASSOC_DATA_2f, ad->fmt_ad2);
        FMT_ASSOC_DATA_FIELD_SET(full_buf, FULL, ASSOC_DATA_3f, ad->fmt_ad3);
    } else {
        L3_DEFIP_ALPM_ASSOC_DATA_REDUCED_t *red_buf = &buf->red;
        FMT_ASSOC_DATA_FIELD_SET(red_buf, REDUCED, ASSOC_DATA_1f, ad->fmt_ad1);
        FMT_ASSOC_DATA_FIELD_SET(red_buf, REDUCED, ASSOC_DATA_2f, ad->fmt_ad2);
    }
}

static void
bcm56960_ln_assoc_data_format_decode(int u, bcm56960_assoc_data_t *buf,
                                     int full,
                                     assoc_data_t *ad)
{
    if (full) {
        L3_DEFIP_ALPM_ASSOC_DATA_FULL_t *full_buf = &buf->full;
        FMT_ASSOC_DATA_FIELD_GET(full_buf, FULL, ASSOC_DATA_1f, ad->fmt_ad1);
        FMT_ASSOC_DATA_FIELD_GET(full_buf, FULL, ASSOC_DATA_2f, ad->fmt_ad2);
        FMT_ASSOC_DATA_FIELD_GET(full_buf, FULL, ASSOC_DATA_3f, ad->fmt_ad3);
    } else {
        L3_DEFIP_ALPM_ASSOC_DATA_REDUCED_t *red_buf = &buf->red;
        FMT_ASSOC_DATA_FIELD_GET(red_buf, REDUCED, ASSOC_DATA_1f, ad->fmt_ad1);
        FMT_ASSOC_DATA_FIELD_GET(red_buf, REDUCED, ASSOC_DATA_2f, ad->fmt_ad2);
    }
}

static inline void
bcm56960_ln_route_base_entry_info(int u, uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *number)
{
    ALPM_ASSERT(format_value < COUNTOF(route_base_entry_info));
    *prefix_len = route_base_entry_info[format_value].prefix_len;
    *full = route_base_entry_info[format_value].full;
    *number = route_base_entry_info[format_value].num_entries;
}

static int
bcm56960_ln_base_entry_info(int u,
                            format_type_t format_type,
                            uint8_t format_value,
                            int *prefix_len,
                            int *full,
                            int *num)
{
    int a, b, c;
    if (!prefix_len) {
        prefix_len = &a;
    }
    if (!full) {
        full = &b;
    }
    if (!num) {
        num = &c;
    }
    bcm56960_ln_route_base_entry_info(u, format_value, prefix_len, full, num);
    return SHR_E_NONE;
}

/*
 * This function is so short that the compiler might optimize it to an inline
 * function, in which case, the middle (void *) between the (uint32_t *)
 * and (struct_t *) is removed, and will cause a strict aliasing.
 * For that reason, we need an union struct.
 */
static void
bcm56960_ln_sub_bkt_set(uint8_t format_value,
                        uint8_t sub_bkt,
                        bcm56960_alpm_route_fmt_t *buf)
{
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = &buf->fmt1;
        ROUTE_FMT_FIELD_SET(1, SUB_BKT_PTRf, sub_bkt);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = &buf->fmt2;
        ROUTE_FMT_FIELD_SET(2, SUB_BKT_PTRf, sub_bkt);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = &buf->fmt3;
        ROUTE_FMT_FIELD_SET(3, SUB_BKT_PTRf, sub_bkt);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = &buf->fmt4;
        ROUTE_FMT_FIELD_SET(4, SUB_BKT_PTRf, sub_bkt);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = &buf->fmt5;
        ROUTE_FMT_FIELD_SET(5, SUB_BKT_PTRf, sub_bkt);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}
static void
bcm56960_ln_route_format_set(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_arg_t *arg,
                             void *buf)
{
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(1, KEYf, arg->key.ip_addr[0]);
        ROUTE_FMT_FIELD_SET(1, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_SET(1, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_SET(1, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_SET(1, DATAf, ad_format[0]);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(2, KEYf, &arg->key.ip_addr[2]);
        ROUTE_FMT_FIELD_SET(2, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_SET(2, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_SET(2, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_SET(2, DATAf, ad_format[0]);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(3, KEYf, arg->key.ip_addr[0]);
        ROUTE_FMT_FIELD_SET(3, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_SET(3, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_SET(3, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_SET2(3, DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(4, KEYf, &arg->key.ip_addr[2]);
        ROUTE_FMT_FIELD_SET(4, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_SET(4, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_SET(4, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_SET2(4, DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(5, KEYf, arg->key.ip_addr);
        ROUTE_FMT_FIELD_SET(5, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_SET(5, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_SET(5, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_SET2(5, DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56960_ln_route_format_get(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_arg_t *arg,
                             void *buf)
{
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(1, KEYf, arg->key.ip_addr[0]);
        ROUTE_FMT_FIELD_GET(1, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_GET(1, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_GET(1, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_GET(1, DATAf, ad_format[0]);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(2, KEYf, &arg->key.ip_addr[2]);
        ROUTE_FMT_FIELD_GET(2, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_GET(2, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_GET(2, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_GET(2, DATAf, ad_format[0]);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(3, KEYf, arg->key.ip_addr[0]);
        ROUTE_FMT_FIELD_GET(3, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_GET(3, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_GET(3, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_GET2(3, DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(4, KEYf, &arg->key.ip_addr[2]);
        ROUTE_FMT_FIELD_GET(4, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_GET(4, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_GET(4, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_GET2(4, DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(5, KEYf, arg->key.ip_addr);
        ROUTE_FMT_FIELD_GET(5, LENGTHf, arg->key.t.len);
        ROUTE_FMT_FIELD_GET(5, VALIDf, arg->valid);
        ROUTE_FMT_FIELD_GET(5, SUB_BKT_PTRf, arg->a1.sub_bkt);
        ROUTE_FMT_FIELD_GET2(5, DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static int
bcm56960_ln_raw_bucket_encode(int u, format_type_t format_type,
                              uint8_t format_value,
                              FORMAT_ENTRYt *base_entry,
                              int entry_count,
                              void *buf)
{
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
    return SHR_E_NONE;
}


static int
bcm56960_ln_raw_bucket_entry_set(int u, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *buf)
{
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET2(1, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
    return SHR_E_NONE;
}


static int
bcm56960_ln_raw_bucket_decode(int u, format_type_t format_type,
                              uint8_t format_value,
                              FORMAT_ENTRYt *base_entry,
                              int entry_count,
                              void *buf)
{
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
    return SHR_E_NONE;
}



static int
bcm56960_ln_raw_bucket_entry_get(int u, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *buf)
{
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET2(1, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
    return SHR_E_NONE;
}


static int
bcm56960_ln_base_entry_encode(int u, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    int full;
    int max_len;
    int number;
    bcm56960_assoc_data_t ad_format = {.v={0}};
    bcm56960_ln_route_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    bcm56960_ln_assoc_data_format_encode(u, &ad_format, full, &(arg->ad));
    bcm56960_ln_route_format_set(format_value,
                                 &ad_format.v[0],
                                 arg,
                                 entry);
    return SHR_E_NONE;
}


static int
bcm56960_ln_base_entry_decode(int u, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    int full;
    int max_len;
    int number;
    bcm56960_assoc_data_t ad_format = {.v={0}};
    bcm56960_ln_route_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    bcm56960_ln_route_format_get(format_value,
                                 &ad_format.v[0],
                                 arg,
                                 entry);
    bcm56960_ln_assoc_data_format_decode(u, &ad_format, full, &(arg->ad));
    return SHR_E_NONE;
}

static int
bcm56960_ln_base_entry_move(int u, format_type_t format_type,
                            uint8_t src_format_value,
                            int src_ent_ofs,
                            key_tuple_t *src_t,
                            void *src_bucket,

                            uint8_t dst_format_value,
                            int dst_ent_ofs,
                            key_tuple_t *dst_t,
                            void *dst_bucket)
{
    bcm56960_alpm_route_fmt_t src_base_entry;
    bcm56960_alpm_route_fmt_t dst_base_entry;
    SHR_FUNC_ENTER(u);

#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        sal_memset(&src_base_entry, 0, sizeof(src_base_entry));
        sal_memset(&dst_base_entry, 0, sizeof(dst_base_entry));
    }
#endif

    SHR_IF_ERR_EXIT(
        bcm56960_ln_raw_bucket_entry_get(u, format_type,
                                         src_format_value,
                                         &src_base_entry.v[0],
                                         src_ent_ofs,
                                         src_bucket));

    if (src_format_value == dst_format_value) {
#ifdef ALPM_DEBUG
        if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
            alpm_arg_t arg;
            bcmptm_rm_alpm_arg_init(u, &arg);

            SHR_IF_ERR_EXIT(
                bcm56960_ln_base_entry_decode(u, format_type,
                                              src_format_value,
                                              0,
                                              &arg,
                                              &src_base_entry.v[0]));
             SHR_IF_ERR_EXIT(
                bcm56960_ln_base_entry_encode(u, format_type,
                                              dst_format_value,
                                              0,
                                              &arg,
                                              &dst_base_entry.v[0]));
            ALPM_ASSERT(!sal_memcmp(&dst_base_entry, &src_base_entry,
                                    sizeof(dst_base_entry)));
            arg.a1.sub_bkt = dst_t->len;
             SHR_IF_ERR_EXIT(
                bcm56960_ln_base_entry_encode(u, format_type,
                                              dst_format_value,
                                              0,
                                              &arg,
                                              &dst_base_entry.v[0]));
        } else
#endif
        {
            sal_memcpy(&dst_base_entry, &src_base_entry,
                       sizeof(dst_base_entry));
            bcm56960_ln_sub_bkt_set(dst_format_value, dst_t->len,
                                    &dst_base_entry);
        }
    } else {
        alpm_arg_t arg;
        bcmptm_rm_alpm_arg_init(u, &arg);

        SHR_IF_ERR_EXIT(
            bcm56960_ln_base_entry_decode(u, format_type,
                                          src_format_value,
                                          0,
                                          &arg,
                                          &src_base_entry.v[0]));
        arg.a1.sub_bkt = dst_t->len;
         SHR_IF_ERR_EXIT(
            bcm56960_ln_base_entry_encode(u, format_type,
                                          dst_format_value,
                                          0,
                                          &arg,
                                          &dst_base_entry.v[0]));
    }
    SHR_IF_ERR_EXIT(
        bcm56960_ln_raw_bucket_entry_set(u, format_type,
                                         dst_format_value,
                                         &dst_base_entry.v[0],
                                         dst_ent_ofs,
                                         dst_bucket));
    sal_memset(&src_base_entry, 0, sizeof(src_base_entry));
    SHR_IF_ERR_EXIT(
        bcm56960_ln_raw_bucket_entry_set(u, format_type,
                                         src_format_value,
                                         &src_base_entry.v[0],
                                         src_ent_ofs,
                                         src_bucket));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56960_ln_hit_get(int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                    uint8_t *hit)
{
    L3_DEFIP_ALPM_HIT_ONLYm_t *e = (L3_DEFIP_ALPM_HIT_ONLYm_t *) hit_entry;
    if (ln == LEVEL2) {
        switch (ofs) {
        case 0: *hit = L3_DEFIP_ALPM_HIT_ONLYm_HIT_0f_GET(*e); break;
        case 1: *hit = L3_DEFIP_ALPM_HIT_ONLYm_HIT_1f_GET(*e); break;
        case 2: *hit = L3_DEFIP_ALPM_HIT_ONLYm_HIT_2f_GET(*e); break;
        case 3: *hit = L3_DEFIP_ALPM_HIT_ONLYm_HIT_3f_GET(*e); break;
        default: return SHR_E_PARAM;
        }
    } else {
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

static int
bcm56960_ln_hit_set(int u, int ln, uint8_t ofs, uint32_t *hit_entry,
                    uint8_t hit)
{
    L3_DEFIP_ALPM_HIT_ONLYm_t *e = (L3_DEFIP_ALPM_HIT_ONLYm_t *) hit_entry;
    if (ln == LEVEL2) {
        switch (ofs) {
        case 0: L3_DEFIP_ALPM_HIT_ONLYm_HIT_0f_SET(*e, hit); break;
        case 1: L3_DEFIP_ALPM_HIT_ONLYm_HIT_1f_SET(*e, hit); break;
        case 2: L3_DEFIP_ALPM_HIT_ONLYm_HIT_2f_SET(*e, hit); break;
        case 3: L3_DEFIP_ALPM_HIT_ONLYm_HIT_3f_SET(*e, hit); break;
        default: return SHR_E_PARAM;
        }
    } else {
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Initialize device specific info
 *
 * \param [in] u Device u.
 * \param [out] dev Device specific info.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56960_device_info_init(int u, alpm_dev_info_t *dev)
{
    SHR_FUNC_ENTER(u);

    if (dev) {
        dev->max_vrf = ALPM_MAX_VRF;
        dev->tcam_blocks = 8;
        dev->tcam_depth = 1024;
        dev->pair_sid = L3_DEFIP_PAIR_128m;
        dev->unpair_sid = L3_DEFIPm;
        dev->l2_sid = L3_DEFIP_ALPM_RAWm;
        dev->l2_hit_sid = L3_DEFIP_ALPM_HIT_ONLYm;
        dev->l3_sid = INVALIDm;
        dev->l3_hit_sid = INVALIDm;
        dev->feature_pkms = 2;
        dev->feature_urpf = 1;
        dev->pivot_max_format = 0;
        dev->route_max_format = 5;
        dev->wo_template = &wide_override_template;
        dev->wg_template = &wide_global_template;
        dev->wp_template = &wide_private_template;
        dev->np_template = &narrow_private_template;
        dev->ng_template = &narrow_global_template;
        dev->no_template = &narrow_override_template;
        dev->w_size = sizeof(L3_DEFIP_PAIR_128m_t);
        dev->n_size = sizeof(L3_DEFIPm_t);
        dev->alpm_ver = ALPM_VERSION_0;

        bcm56960_l1_wide_template_init(u);
        bcm56960_l1_narrow_template_init(u);
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56960_a0_rm_alpm_driver_register(int u, alpm_driver_t *drv)
{
    SHR_FUNC_ENTER(u);

    if (drv) {
        drv->l1_encode = bcm56960_l1_encode;
        drv->l1_decode = bcm56960_l1_decode;
        drv->l1_half_entry_move = bcm56960_l1_half_entry_move;
        drv->ln_base_entry_decode = bcm56960_ln_base_entry_decode;
        drv->ln_base_entry_encode = bcm56960_ln_base_entry_encode;
        drv->ln_raw_bucket_decode = bcm56960_ln_raw_bucket_decode;
        drv->ln_raw_bucket_encode = bcm56960_ln_raw_bucket_encode;
        drv->ln_raw_bucket_entry_get = bcm56960_ln_raw_bucket_entry_get;
        drv->ln_raw_bucket_entry_set = bcm56960_ln_raw_bucket_entry_set;
        drv->ln_base_entry_info = bcm56960_ln_base_entry_info;
        drv->ln_base_entry_move = bcm56960_ln_base_entry_move;
        drv->device_info_init   = bcm56960_device_info_init;
        drv->bkt_hdl_profile = bcm56960_bkt_hdl_profile;
        drv->ln_hit_get = bcm56960_ln_hit_get;
        drv->ln_hit_set = bcm56960_ln_hit_set;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}


