/*! \file rm_alpm_level1.c
 *
 * Resource Manager for ALPM Level-1
 *
 * This file contains the resource manager for ALPM Level-1
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
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "rm_alpm_common.h"
#include "rm_alpm_device.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"


/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

#define INVALID_PFX -1

/*! Level-1 info */
#define L1_HA(u)                         (alpm_l1_ha[u])
#define L1_INFO(u)                       (alpm_l1_info[u])
#define L1_INFO_FD(u, _f)                L1_INFO(u)->_f
#define L1_INFO_FDM(u, _f, _i)           ((L1_INFO(u)->_f)[(_i)])
#define UNPAIRS(u, db, _upkm)            L1_INFO(u)->unpairs[db][_upkm]
#define PAIRS(u, db)                     L1_INFO(u)->pairs[db]
#define SIZE_PAIR(u)                     L1_INFO(u)->size_pair
#define SIZE_UNPAIR(u, _upkm)            L1_INFO(u)->size_unpair[_upkm]
#define SIZE_PAIR_2(u, db)               L1_INFO(u)->size_pair_2[db]
#define START_PAIR(u, db)                L1_INFO(u)->start_pair[db]
#define SIZE_UNPAIR_2(u, db, _upkm)      L1_INFO(u)->size_unpair_2[db][_upkm]
#define START_UNPAIR(u, db, _upkm)       L1_INFO(u)->start_unpair[db][_upkm]
#define RANGE1_FIRST(u, db)              L1_INFO(u)->range1_first[db]
#define RANGE2_FIRST(u, db)              L1_INFO(u)->range2_first[db]
#define RANGE1_LAST(u, db)               L1_INFO(u)->range1_last[db]
#define RANGE1_LAST_PFX(u, db)           L1_INFO(u)->range1_last_pfx[db]
#define DUAL_MODE(u)                     L1_INFO(u)->dual_mode

/*! Valid entry span. */
#define LAST_PFX_SPAN(u, db)             L1_INFO(u)->last_pfx_span[db]
#define PAIR_MAX_PFX(u, db)              L1_INFO(u)->pair_max_pfx[db]
#define PAIR_MAX_V4_PFX(u, db)           L1_INFO(u)->pair_max_v4_pfx[db]
#define UNPAIR_MAX_PFX(u, db, _upkm)     L1_INFO(u)->unpair_max_pfx[db][_upkm]
#define PKM_128(u)                       L1_INFO(u)->pkm_128
#define PKM_64(u)                        L1_INFO(u)->pkm_64
#define PKM_32(u)                        L1_INFO(u)->pkm_32
#define PAIR_SID(u)                      L1_INFO(u)->pair_sid
#define UNPAIR_SID(u)                    L1_INFO(u)->unpair_sid
#define TCAM_DEPTH(u)                    L1_INFO(u)->tcam_depth
#define MAX_TCAMS(u)                     L1_INFO(u)->tcam_blocks

/*! PAIR statistics */
#define PAIR_STATS(u)                  (L1_INFO(u)->pair_stats)
#define PAIR_USED_COUNT(u, kt, vt)     PAIR_STATS(u).pair_used_count[kt][vt]
#define PAIR_MAX_COUNT(u, kt, vt)      PAIR_STATS(u).pair_max_count[kt][vt]
#define PAIR_HALF_COUNT(u, kt, vt)     PAIR_STATS(u).pair_half_count[kt][vt]

/*! UNPAIR statistics */
#define UNPAIR_STATS(u, _upkm)   (L1_INFO(u)->unpair_stats[_upkm])

#define UNPAIR_USED_COUNT(u, kt, vt, _upkm)   \
    UNPAIR_STATS(u, _upkm).unpair_used_count[kt][vt]

#define UNPAIR_MAX_COUNT(u, kt, vt, _upkm)    \
    UNPAIR_STATS(u, _upkm).unpair_max_count[kt][vt]

#define UNPAIR_HALF_COUNT(u, kt, vt, _upkm)   \
    UNPAIR_STATS(u, _upkm).unpair_half_count[kt][vt]

/*! Total statistics */
#define TOTAL_MAX(u, kt, vt)                   \
    (UNPAIR_MAX_COUNT(u, kt, vt, PKM_64(u)) +  \
     UNPAIR_MAX_COUNT(u, kt, vt, PKM_32(u)) +  \
     PAIR_MAX_COUNT(u, kt, vt))

#define TOTAL_COUNT(u, kt, vt)                 \
    (UNPAIR_USED_COUNT(u, kt, vt, PKM_64(u)) + \
     UNPAIR_USED_COUNT(u, kt, vt, PKM_32(u)) + \
     PAIR_USED_COUNT(u, kt, vt))


/*! UNPAIR pfxs */
#define UNPAIR_IPV4_PFXS              33
#define UNPAIR_IPV6_64_PFXS           65
#define UNPAIR_IPV6_32_PFXS           33

/*! UNPAIR states */
#define UNPAIR_STATE(u, upkm)             (L1_INFO(u)->unpair_state[upkm])
#define UNPAIR_STATE_START(u, pfx, upkm)  (UNPAIR_STATE(u, upkm)[(pfx)].start)
#define UNPAIR_STATE_END(u, pfx, upkm)    (UNPAIR_STATE(u, upkm)[(pfx)].end)
#define UNPAIR_STATE_PREV(u, pfx, upkm)   (UNPAIR_STATE(u, upkm)[(pfx)].prev)
#define UNPAIR_STATE_NEXT(u, pfx, upkm)   (UNPAIR_STATE(u, upkm)[(pfx)].next)
#define UNPAIR_STATE_VENT(u, pfx, upkm)   (UNPAIR_STATE(u, upkm)[(pfx)].vent)
#define UNPAIR_STATE_FENT(u, pfx, upkm)   (UNPAIR_STATE(u, upkm)[(pfx)].fent)
#define UNPAIR_STATE_HENT(u, pfx, upkm)   (UNPAIR_STATE(u, upkm)[(pfx)].hent)

/*! PAIR pfxs */
#define PAIR_IPV4_PFXS              33
#define PAIR_IPV6_128_PFXS          129

/*! PAIR states */
#define PAIR_STATE(u)               (L1_INFO(u)->pair_state)
#define PAIR_STATE_START(u, pfx)    (PAIR_STATE(u)[(pfx)].start)
#define PAIR_STATE_END(u, pfx)      (PAIR_STATE(u)[(pfx)].end)
#define PAIR_STATE_PREV(u, pfx)     (PAIR_STATE(u)[(pfx)].prev)
#define PAIR_STATE_NEXT(u, pfx)     (PAIR_STATE(u)[(pfx)].next)
#define PAIR_STATE_VENT(u, pfx)     (PAIR_STATE(u)[(pfx)].vent)
#define PAIR_STATE_FENT(u, pfx)     (PAIR_STATE(u)[(pfx)].fent)
#define PAIR_STATE_HENT(u, pfx)     (PAIR_STATE(u)[(pfx)].hent)

#define PVT_INFO_ARRAY(u)           (L1_INFO(u)->pivot_info)
#define L1_PVT_INFO(u, index)       (L1_INFO(u)->pivot_info[index])

#define L1_HA_RANGE1_FIRST(u, db)   (alpm_l1_ha[u]->range1_first[db])

/*! Pivot info */
#define PIVOT_INDEX(u, _i)          L1_PVT_INFO(u, _i)->index

#ifdef ALPM_DEBUG
#define SANITY_CHECK_PAIR_ENTRY(u, de, se)          \
    sanity_check_pair_entry(u, de, se)
#define SANITY_CHECK_PAIR_STATE(u, ldb, pfx)        \
    sanity_check_pair_state(u, ldb, pfx)
#define SANITY_CHECK_PAIR_PFX(u, ldb, pfx)          \
    sanity_check_pair_pfx(u, ldb, pfx)
#define SANITY_CHECK_PIVOT(u, pidx, p, f, c)        \
    if (c) sanity_check_pivot(u, pidx, p, f)
#define SANITY_CHECK_UNPAIR_ENTRY(u, de, se, pkm)   \
    sanity_check_unpair_entry(u, de, se, pkm)
#define SANITY_CHECK_UNPAIR_STATE(u, ldb, fx, pkm)  \
    sanity_check_unpair_state(u, ldb, pfx, pkm)
#define SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, pkm)   \
    sanity_check_unpair_pfx(u, ldb, pfx, pkm)
#else
#define SANITY_CHECK_PAIR_ENTRY(u, de, se)
#define SANITY_CHECK_PAIR_STATE(u, ldb, pfx)
#define SANITY_CHECK_PAIR_PFX(u, ldb, pfx)
#define SANITY_CHECK_PIVOT(u, pidx, p, f, c)
#define SANITY_CHECK_UNPAIR_ENTRY(u, de, se, pkm)
#define SANITY_CHECK_UNPAIR_STATE(u, ldb, fx, pkm)
#define SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, pkm)
#endif

/*******************************************************************************
  Typedefs
 */
typedef struct insert_pkms_s {
    int pair;
    int unpair64;
    int unpair32;
} insert_pkms_t;

typedef struct alpm_l1_ha_s {
    int range1_first[L1_DBS];
} alpm_l1_ha_t;
/*******************************************************************************
 * Private variables
 */
static alpm_l1_info_t *alpm_l1_info[BCMDRD_CONFIG_MAX_UNITS] = {0};
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS];

static alpm_l1_ha_t *alpm_l1_ha[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Private Functions (declaration)
 */
static int read_l1_raw(int u, bcmdrd_sid_t sid, int index, uint32_t *de, uint32_t *se);
static int write_l1_raw(int u, bcmdrd_sid_t sid, int index, uint32_t *de, uint32_t *se, int pfx);
static int clear_l1_raw(int u, bcmdrd_sid_t sid, int index);
static int pair_free_slot_move_down(int u, uint8_t ldb, int pfx, int free_pfx);
static int pair_free_slot_move_up(int u, uint8_t ldb, int pfx, int free_pfx);
static int pfx_unpair_to_unpair(int u, int unpair_pfx, int upkm);
static int pfx_unpair_to_pair(int u, int unpair_pfx, int upkm);
static int arr_idx_from_pvt_idx(int u, int pivot_index);
static inline int half_entry_key_type(int u, alpm_key_type_t kt, int upkm);
static inline int pair_pfx_encode(int u, int ipv6, alpm_vrf_type_t vt, int len);
static inline int unpair_pfx_encode(int u, int ipv6, alpm_vrf_type_t vt, int len, int upkm);
static inline int pair_index_encode(int u, int idx);
static inline bool pair_pfx_is_ipv4(int u, uint8_t ldb, int pfx);
/*******************************************************************************
 * Private Functions (definition)
 */

#ifdef ALPM_DEBUG
/*!
 * \brief Sanity check for pivot
 *
 * \param [in] u Device u.
 * \param [in] pivot_idx Pivot index
 * \param [in] pivot Pivot payload
 * \param [in] found Pivot exists
 *
 * \return nothing
 */
static void
sanity_check_pivot(int u, int pivot_idx, alpm_pivot_t *pivot, int found)
{
    int arr_idx;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    arr_idx = arr_idx_from_pvt_idx(u, pivot_idx);

    if (found) {
        ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == pivot);
        ALPM_ASSERT(PIVOT_INDEX(u, arr_idx) == pivot->index);
    } else {
        ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);
    }
}

static void
sanity_check_unpair_pfx(int u, uint8_t ldb, int pfx, int upkm)
{
    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    ALPM_ASSERT(pfx != INVALID_PFX);
    ALPM_ASSERT(pfx <= UNPAIR_MAX_PFX(u, ldb, upkm));
}

static void
sanity_check_pair_pfx(int u, uint8_t ldb, int pfx)
{
    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    ALPM_ASSERT(pfx != INVALID_PFX);
    ALPM_ASSERT(pfx <= PAIR_MAX_PFX(u, ldb));
}

/*!
 * \brief Sanity check for unpair state
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm Unpair packing mode
 *
 * \return nothing
 */
static void
sanity_check_unpair_state(int u, uint8_t ldb, int pfx, int upkm)
{
    int vent;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, upkm);
    vent = UNPAIR_STATE_END(u, pfx, upkm) - UNPAIR_STATE_START(u, pfx, upkm) + 1;
    ALPM_ASSERT(vent == UNPAIR_STATE_VENT(u, pfx, upkm));
    ALPM_ASSERT(UNPAIR_STATE_START(u, pfx, upkm) >= SIZE_PAIR(u));
    ALPM_ASSERT(UNPAIR_STATE_VENT(u, pfx, upkm) >= 0);
    ALPM_ASSERT(UNPAIR_STATE_FENT(u, pfx, upkm) >= 0);
    ALPM_ASSERT(UNPAIR_STATE_HENT(u, pfx, upkm) == 1 ||
                UNPAIR_STATE_HENT(u, pfx, upkm) == 0);

}

/*!
 * \brief Sanity check for pair state
 *
 * \param [in] u Device u.
 * \param [in] ldb Level1 DB
 * \param [in] pfx Translated prefix (Global prefix value).

 * \return nothing
 */
static void
sanity_check_pair_state(int u, uint8_t ldb, int pfx)
{
    int vent;
    int start = PAIR_STATE_START(u, pfx);
    int end = PAIR_STATE_END(u, pfx);

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    ALPM_ASSERT(pfx != INVALID_PFX);
    if (pfx == RANGE1_LAST_PFX(u, ldb) && LAST_PFX_SPAN(u, ldb)) {
        vent = RANGE1_LAST(u, ldb) - start + 1;
        ALPM_ASSERT(vent >= 0);
        vent += end - RANGE2_FIRST(u, ldb) + 1;
        ALPM_ASSERT(vent == PAIR_STATE_VENT(u, pfx));
    } else if (pair_pfx_is_ipv4(u, ldb, pfx)) {
        vent = end - start;
        vent = (vent < 0) ? 0 : (vent + 1);
        ALPM_ASSERT(vent == PAIR_STATE_VENT(u, pfx));
    } else {
        if ((end - start) < 0) {
            vent = 0;
        } else {
            start = pair_index_encode(u, start);
            end = pair_index_encode(u, end);
            vent = end - start + 1;
        }
        ALPM_ASSERT(vent == PAIR_STATE_VENT(u, pfx));
    }

    ALPM_ASSERT(PAIR_STATE_END(u, pfx) < SIZE_PAIR(u));
    ALPM_ASSERT(PAIR_STATE_VENT(u, pfx) >= 0);
    ALPM_ASSERT(PAIR_STATE_FENT(u, pfx) >= 0);
    ALPM_ASSERT(PAIR_STATE_HENT(u, pfx) == 1 ||
                PAIR_STATE_HENT(u, pfx) == 0);

}
/*!
 * \brief Sanity check for unpair entries
 *
 * \param [in] u Device u.
 * \param [in] de DIP unpair entry
 * \param [in] se SIP unpair entry
 * \param [in] pkm Packing mode

 * \return nothing
 */
static void
sanity_check_unpair_entry(int u, uint32_t *de, uint32_t *se, int pkm)
{
    int rv;
    L1_ENTRY_t de2, se2;
    alpm_arg_t arg, arg2;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    sal_memcpy(de2, de, sizeof(de2));
    sal_memcpy(se2, se, sizeof(se2));

    bcmptm_rm_alpm_arg_init(u, &arg);
    rv = bcmptm_rm_alpm_arg_fill(u, &arg, de2, se2, L1V_NARROW_0);
    ALPM_ASSERT(SHR_SUCCESS(rv));

    if (half_entry_key_type(u, arg.key.kt, pkm)) {
        bcmptm_rm_alpm_arg_init(u, &arg2);
        rv = bcmptm_rm_alpm_arg_fill(u, &arg2, de2, se2, L1V_NARROW_1);
        ALPM_ASSERT(SHR_SUCCESS(rv));
        if (arg.valid && arg2.valid) {
            ALPM_ASSERT(arg2.key.t.len == arg.key.t.len);
        }
    }
}


/*!
 * \brief Sanity check for pair entries
 *
 * \param [in] u Device u.
 * \param [in] de DIP unpair entry
 * \param [in] se SIP unpair entry

 * \return nothing
 */
static void
sanity_check_pair_entry(int u, uint32_t *de, uint32_t *se)
{
    int rv;
    L1_ENTRY_t de2, se2;
    alpm_arg_t arg;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }

    sal_memcpy(de2, de, sizeof(de2));
    sal_memcpy(se2, se, sizeof(se2));

    bcmptm_rm_alpm_arg_init(u, &arg);
    rv = bcmptm_rm_alpm_arg_fill(u, &arg, de2, se2, L1V_WIDE);
    ALPM_ASSERT(SHR_SUCCESS(rv));
}

#endif

static inline void
l1_db_get(int u, alpm_arg_t *arg)
{
    if (DUAL_MODE(u) && arg->key.vt != VRF_PRIVATE) {
        arg->l1_db = L1_DB1;
    } else{
        arg->l1_db = L1_DB0;
    }
}

static alpm_key_type_t
key_type_get(int u, int ipv6, int len)
{
    alpm_key_type_t kt;
    if (!ipv6) {
        kt = KEY_IPV4;
    } else if (len <= 32) {
        kt = (PKM_32(u) == PKM_64(u)) ? KEY_IPV6_64 : KEY_IPV6_32;
    } else if (len <= 64) {
        kt = KEY_IPV6_64;
    } else {
        kt = KEY_IPV6_128;
    }
    return kt;
}


/*!
 * \brief Convert index to packing mode
 *
 * \param [in] u Device u.
 * \param [in] index Index
 *
 * \return Packing mode
 */
static int
index_to_pkm(int u, int index)
{
    int pkm;
    index -= SIZE_PAIR(u);
    if (index >= 0) {
        index -= SIZE_UNPAIR(u, PKM_64(u));
        if (index >= 0) {
            pkm = PKM_32(u);
        } else {
            pkm = PKM_64(u);
        }
    } else {
        pkm = PKM_128(u);
    }
    return pkm;
}

static inline int
index_in_pair_tcam(int u, int index)
{
    return index_to_pkm(u, index) == PKM_128(u);
}

static inline int
is_pair_sid(int u, bcmdrd_sid_t sid)
{
    return (sid == PAIR_SID(u));
}

static inline int
half_entry_key_type(int u, alpm_key_type_t kt, int upkm)
{
    return (kt == KEY_IPV4 ||
            (kt == KEY_IPV6_32 && upkm == PKM_32(u)));
}

/*!
 * \brief Encode and write dip & sip entry from common arg.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \param [in] new_add Entry not exists
 *
 * \return SHR_E_XXX.
 */
static int
encode_write_l1_entry(int u, bcmdrd_sid_t sid, alpm_arg_t *arg, bool new_add)
{
    L1_ENTRY_t de, se;
    int index;
    uint8_t l1v;

    SHR_FUNC_ENTER(u);

    index = arg->index[LEVEL1];
    if (IS_HALF_INDEX(index)) {
        if (IS_HALF_L_INDEX(index) && new_add) {
            sal_memcpy(de, L1_INFO_FDM(u, n_template, arg->key.vt),
                       L1_INFO_FD(u, n_size));
        } else {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, sid, HW_INDEX(index), de, se));
        }
        l1v = IS_HALF_R_INDEX(index) ? L1V_NARROW_1 : L1V_NARROW_0;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_encode(u, arg, de, se, l1v));
    } else {
        if (sid == PAIR_SID(u)) {
            l1v = L1V_WIDE;
            sal_memcpy(de, L1_INFO_FDM(u, w_template, arg->key.vt),
                       L1_INFO_FD(u, w_size));
        } else {
            l1v = L1V_NARROW_FULL;
            sal_memcpy(de, L1_INFO_FDM(u, n_template, arg->key.vt),
                       L1_INFO_FD(u, n_size));
        }
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_encode(u, arg, de, se, l1v));
    }

    SHR_IF_ERR_EXIT(
        write_l1_raw(u, sid, HW_INDEX(index), de, se, arg->key.pfx));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read and decode dip & sip entry to common arg.
 *
 * For unpair, normaly read & decode the first half, unless requesting arg2,
 * in which case, both halves are decoded.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] arg2 If set request for both halves, valid for half key type only
 *
 * \return SHR_E_XXX.
 */
static int
decode_read_l1_entry(int u, bcmdrd_sid_t sid, alpm_arg_t *arg, alpm_arg_t *arg2)
{
    L1_ENTRY_t de, se;
    int index;
    int half_kt;
    uint8_t l1v;
    SHR_FUNC_ENTER(u);

    index = arg->index[LEVEL1];

    /* If request arg2, fill both halves */
    half_kt = half_entry_key_type(u, arg->key.kt, arg->upkm);
    if (half_kt && arg2) {
        SHR_IF_ERR_EXIT(
            read_l1_raw(u, sid, HW_INDEX(index), de, se));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_arg_fill(u, arg, &de, &se, L1V_NARROW_0));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_arg_fill(u, arg2, &de, &se, L1V_NARROW_1));

        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(
        read_l1_raw(u, sid, HW_INDEX(index), de, se));
    if (half_kt) {
        l1v = IS_HALF_R_INDEX(index) ? L1V_NARROW_1 : L1V_NARROW_0;
    } else {
        l1v = (sid == PAIR_SID(u)) ? L1V_WIDE : L1V_NARROW_FULL;
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_arg_fill(u, arg, &de, &se, l1v));

exit:
    SHR_FUNC_EXIT();
}


static inline int
tcam_num(int u, int index)
{
    return ((index) / TCAM_DEPTH(u));
}

static inline int
index_in_right_tcam(int u, int index)
{
    return (tcam_num(u, index) & 1);
}


/*!
 * \brief Convert pivot index to array index (subscript) of pivot info
 *
 * \param [in] u Device u.
 * \param [in] pivot_index Pivot index
 *
 * \return Array index (subscript) of pivot info
 */
static int
arr_idx_from_pvt_idx(int u, int pivot_index)
{
    int arr_idx;

    if (pivot_index == INVALID_INDEX) {
        arr_idx = INVALID_INDEX;
    } else {
        arr_idx = HW_INDEX(pivot_index) << 1;
        if (IS_HALF_R_INDEX(pivot_index)) {
            arr_idx++;
        }
    }
    return arr_idx;
}


/*!
 * \brief Convert to array index (subscript) of pivot info to pivot index
 *
 * \param [in] u Device u.
 * \param [in] arr_idx Array index (subscript) of pivot info
 * \param [in] full Full key type
 *
 * \return Pivot index
 */
static inline int
pvt_idx_from_arr_idx(int u, int arr_idx, int full)
{
    int pivot_idx = INVALID_INDEX;

    if (arr_idx != INVALID_INDEX) {
        int hw_index = arr_idx >> 1;

        if (full) {
            pivot_idx = FULL_INDEX(hw_index);
        } else if (arr_idx & 1) {
            pivot_idx = HALF_INDEX_R(hw_index);
        } else {
            pivot_idx = HALF_INDEX_L(hw_index);
        }
    }

    return pivot_idx;
}


/*!
 * \brief Allocate and initialize pivot payload
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot_pyld pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_alloc(int u, alpm_arg_t *arg, alpm_pivot_t **pivot_pyld)
{
    int rv;
    rv = bcmptm_rm_alpm_elem_alloc(u, arg->db, LEVEL1, &arg->key.t, pivot_pyld);
    if (SHR_SUCCESS(rv)) {
        (*pivot_pyld)->key.vt = arg->key.vt;
        (*pivot_pyld)->index = arg->index[LEVEL1];
        if (arg->a1.direct_route) {
            sal_memcpy((*pivot_pyld)->ad, &arg->ad, sizeof(assoc_data_t));
        }
    }
    return rv;
}

/*!
 * \brief Free pivot payload
 *
 * \param [in] u Device u.
 * \param [in] pivot_pyld pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_free(int u, int db, alpm_pivot_t *pivot)
{
    return bcmptm_rm_alpm_elem_free(u, db, LEVEL1, pivot);
}


/*!
 * \brief Move pivot from one position to another.
 *
 * Move pivot (sw pointer) from one position to another, and update
 * pivot index accordingly.
 *
 * \param [in] u Device u.
 * \param [in] from Half entry based array index
 * \param [in] to Half entry based array index
 * \param [in] full Is full entry
 * \param [in] clear A clear opertion if set, else a shift operation
 *
 * \return SHR_E_XXX.
 */
static int
pivot_move(int u, int from, int to, int full, int clear)
{
    alpm_pivot_t *from_pivot;
    alpm_pivot_t *to_pivot;
    SHR_FUNC_ENTER(u);

    from_pivot = L1_PVT_INFO(u, from);
    to_pivot = L1_PVT_INFO(u, to);


    if (clear) { /* from pivot must exist for clear case */
        ALPM_ASSERT(from_pivot != NULL);
    }

    if (from_pivot) {
        if (from == to) {
            ALPM_ASSERT(from_pivot->index == pvt_idx_from_arr_idx(u, to, full));
        }
        from_pivot->index = pvt_idx_from_arr_idx(u, to, full);

    }
    if (clear) {
        /* Target is supposed to be valid during clear */
        ALPM_ASSERT(to_pivot != NULL);
        /* Just for sanity check */
        to_pivot->index = pvt_idx_from_arr_idx(u, from, full);
    } else {
        /* Target is supposed to be empty during shift */
        ALPM_ASSERT(to_pivot == NULL);
    }
    L1_PVT_INFO(u, to) = from_pivot;
    L1_PVT_INFO(u, from) = NULL;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert pivot payload into Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] pivot Pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_insert(int u, alpm_arg_t *arg, alpm_pivot_t *pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_insert(u, arg->db, LEVEL1, &arg->key, pivot);
    SANITY_CHECK_PIVOT(u, pivot->index, NULL, 0, SHR_SUCCESS(rv));
    return rv;
}

/*!
 * \brief Delete pivot payload from Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Deleted pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_delete(int u, alpm_arg_t *arg, alpm_pivot_t **pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_delete(u, arg->db, LEVEL1, &arg->key.t, pivot);
    SANITY_CHECK_PIVOT(u, (*pivot)->index, NULL, 0, pivot);
    return rv;
}

/*!
 * \brief Lookup (exact match) pivot payload in Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_lookup(int u, alpm_arg_t *arg, alpm_pivot_t **pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_lookup(u, arg->db, LEVEL1, &arg->key.t, pivot);
    SANITY_CHECK_PIVOT(u, (*pivot)->index, *pivot, 1, SHR_SUCCESS(rv));
    return rv;
}

/*!
 * \brief Lookup (LPM match) pivot payload in Level-1 pivot trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Pivot payload
 *
 * \return SHR_E_XXX
 */
static inline int
l1_pvt_find(int u, alpm_arg_t *arg, alpm_pivot_t **pivot)
{
    int rv;
    rv = bcmptm_rm_alpm_pvt_find(u, arg->db, LEVEL1, &arg->key.t, pivot);
    SANITY_CHECK_PIVOT(u, (*pivot)->index, *pivot, 1, SHR_SUCCESS(rv));
    return rv;
}

/*!
 * \brief Map used entries of one KEY type to another
 *
 * Multiple KEY types can coexist within a packing block. The used count
 * for any KEY type are the inserted entries of all KEY types.
 * To evaluate all used count for a given KEY type, we need to map
 * count of all KEY types.
 *
 * \param [in] u Device u.
 * \param [in] from_kt From KEY type
 * \param [in] to_kt To KEY type
 * \param [in] vt VRF type
 *
 * \return Mapped used count
 */
static int
l1_used_count_map(int u, alpm_key_type_t from_kt, alpm_key_type_t to_kt,
                  alpm_vrf_type_t vt)
{
    int count = 0;
    int unpair[UNPAIR_PKMS] = {0}, pair = 0;

    if (from_kt == to_kt) {
        return TOTAL_COUNT(u, from_kt, vt);
    }

    switch (from_kt) {
    case KEY_IPV4:
        if (to_kt == KEY_IPV6_32) { /* KEY_IPV4 -> KEY_IPV6_32 */

            ALPM_ASSERT(PKM_32(u) != PKM_64(u));

            /* PKM32*/
            unpair[UNPAIR_PKM0] = UNPAIR_USED_COUNT(u, from_kt, vt, PKM_32(u)) +
                                  UNPAIR_HALF_COUNT(u, from_kt, vt, PKM_32(u));
            ALPM_ASSERT((unpair[UNPAIR_PKM0] & 0x1) == 0);


            /* PKM64*/
            unpair[UNPAIR_PKM1] = UNPAIR_USED_COUNT(u, from_kt, vt, PKM_64(u)) +
                                  UNPAIR_HALF_COUNT(u, from_kt, vt, PKM_64(u));
            ALPM_ASSERT((unpair[UNPAIR_PKM1] & 0x1) == 0);
            unpair[UNPAIR_PKM1] >>= 1;


            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt) +
                   PAIR_HALF_COUNT(u, from_kt, vt);
            /* Get ceiling & align with 4 */
            pair = (pair + 3) & ~0x3;
            pair >>= 2;

        } else if (to_kt == KEY_IPV6_64) { /* KEY_IPV4 -> KEY_IPV6_64 */

            /* Skip PKM32, as IPv6_64 must not be in */

            /* PKM64*/
            unpair[UNPAIR_PKM1] = UNPAIR_USED_COUNT(u, from_kt, vt, PKM_64(u)) +
                                  UNPAIR_HALF_COUNT(u, from_kt, vt, PKM_64(u));
            ALPM_ASSERT((unpair[UNPAIR_PKM1] & 0x1) == 0);
            unpair[UNPAIR_PKM1] >>= 1;

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt) +
                   PAIR_HALF_COUNT(u, from_kt, vt);
            /* Get ceiling & align with 4 */
            pair = (pair + 3) & ~0x3;
            pair >>= 2;
        } else { /* KEY_IPV4 -> KEY_IPV6_128 */
            /* Skip PKM32 PKM64, as IPv6_128 must not be in */

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt) +
                   PAIR_HALF_COUNT(u, from_kt, vt);
            /* Get ceiling & align with 4 */
            pair = (pair + 3) & ~0x3;
            pair >>= 2;
        }

        break;
    case KEY_IPV6_32:

        if (PKM_32(u) == PKM_64(u)) {
            break;
        }

        if (to_kt == KEY_IPV4) { /* KEY_IPV6_32 -> KEY_IPV4 */
            /* PKM32*/
            unpair[UNPAIR_PKM0] = UNPAIR_USED_COUNT(u, from_kt, vt, PKM_32(u)) +
                                  UNPAIR_HALF_COUNT(u, from_kt, vt, PKM_32(u));
            ALPM_ASSERT((unpair[UNPAIR_PKM0] & 0x1) == 0);

            /* PKM64*/
            unpair[UNPAIR_PKM1] =
                UNPAIR_USED_COUNT(u, from_kt, vt, PKM_64(u)) << 1;

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt) << 2;
        } else if (to_kt == KEY_IPV6_64) { /* KEY_IPV6_32 -> KEY_IPV6_64 */
            ALPM_ASSERT(PKM_32(u) != PKM_64(u));
            /* Skip PKM32, as IPv6_64 must not be in */

            /* PKM64*/
            unpair[UNPAIR_PKM1] = UNPAIR_USED_COUNT(u, from_kt, vt, PKM_64(u));

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt);
        } else { /* KEY_IPV6_32 -> KEY_IPV6_128 */
            /* Skip PKM32 PKM64, as IPv6_128 must not be in */

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt);
        }
        break;

    case KEY_IPV6_64:
        if (to_kt == KEY_IPV4) { /* KEY_IPV6_64 -> KEY_IPV4 */
            /* Skip PKM32, as IPv6_64 must not be in */

            /* PKM64*/
            unpair[UNPAIR_PKM1] =
                UNPAIR_USED_COUNT(u, from_kt, vt, PKM_64(u)) << 1;

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt) << 2;
        } else if (to_kt == KEY_IPV6_32) { /* KEY_IPV6_64 -> KEY_IPV6_32 */
            ALPM_ASSERT(PKM_32(u) != PKM_64(u));
            /* Skip PKM32, as IPv6_64 must not be in */

            /* PKM64*/
            unpair[UNPAIR_PKM1] = UNPAIR_USED_COUNT(u, from_kt, vt, PKM_64(u));

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt);
        } else { /* KEY_IPV6_64 -> KEY_IPV6_128 */
            /* Skip PKM32 PKM64, as IPv6_128 must not be in */

            /* PKM128*/
            pair = PAIR_USED_COUNT(u, from_kt, vt);
        }
        break;

    case KEY_IPV6_128:
    default:
        if (to_kt == KEY_IPV4) { /* KEY_IPV6_128 -> KEY_IPV4 */
            /* Skip PKM32 & PKM64, as IPv6_128 must not be in */

            /* PKM128 */
            pair = PAIR_USED_COUNT(u, from_kt, vt) << 2;
        } else if (to_kt == KEY_IPV6_32) { /* KEY_IPV6_128 -> KEY_IPV6_32 */
            /* Skip PKM32 & PKM64, as IPv6_128 must not be in */

            /* PKM128 */
            pair = PAIR_USED_COUNT(u, from_kt, vt);
        } else { /* KEY_IPV6_128 -> KEY_IPV6_64 */
            /* Skip PKM32 & PKM64, as IPv6_128 must not be in */

            /* PKM128 */
            pair = PAIR_USED_COUNT(u, from_kt, vt);
        }
        break;
    }
    count = unpair[UNPAIR_PKM0] + unpair[UNPAIR_PKM1] + pair;

    return count;
}


static inline bcmdrd_sid_t
unpair_sid(int u, alpm_key_type_t kt)
{
    return UNPAIR_SID(u);
}

static inline void
unpair_half_entry_update(int u, int pfx, int upkm)
{
    UNPAIR_STATE_HENT(u, pfx, upkm) = !UNPAIR_STATE_HENT(u, pfx, upkm);
}

static inline int
unpair_half_entry(int u, int pfx, int upkm)
{
    return UNPAIR_STATE_HENT(u, pfx, upkm);
}

/*!
 * \brief Get number of prefixes in unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 * \param [in] upkm unpair packing mode
 *
 * \return Number of prefixes
 */
static int
unpair_num_pfxs(int u, alpm_key_type_t kt, alpm_vrf_type_t vt, int upkm)
{

    int num_pfxs;

    if (PKM_32(u) == PKM_64(u) || upkm == PKM_64(u)) {
        if (DUAL_MODE(u)) {
            if (vt == VRF_PRIVATE) {
                num_pfxs = (UNPAIR_IPV4_PFXS * 3 +
                           UNPAIR_IPV6_64_PFXS * 3 + 2);
            } else {
                num_pfxs = (UNPAIR_IPV4_PFXS * 2 +
                           UNPAIR_IPV6_64_PFXS * 2 + 1);
            }
        } else {
            num_pfxs = (UNPAIR_IPV4_PFXS * 3 +
                       UNPAIR_IPV6_64_PFXS * 3 + 2);
        }
    } else {
        if (DUAL_MODE(u)) {
            if (vt == VRF_PRIVATE) {
                num_pfxs = (UNPAIR_IPV4_PFXS * 3 +
                           UNPAIR_IPV6_32_PFXS * 3 + 2);
            } else {
                num_pfxs = (UNPAIR_IPV4_PFXS * 2 +
                           UNPAIR_IPV6_32_PFXS * 2 + 1);
            }
        } else {
            num_pfxs = (UNPAIR_IPV4_PFXS * 3 +
                       UNPAIR_IPV6_32_PFXS * 3 + 2);
        }
    }
    return num_pfxs;
}

/*!
 * \brief Get max prefix in unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 * \param [in] upkm unpair packing mode
 *
 * \return Max prefix in unpair TCAM
 */
static inline int
unpair_max_pfx(int u, alpm_key_type_t kt, alpm_vrf_type_t vt, int upkm)
{
    return unpair_num_pfxs(u, kt, vt, upkm) - 1;
}

/*!
 * \brief Get prefix base in unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 * \param [in] upkm unpair packing mode
 *
 * \return Prefix base
 */
static int
unpair_pfx_base(int u, int ipv6, alpm_vrf_type_t vt, int upkm)
{

    int base = 0;
    int unpair_ipv6_pfxs;

    if (PKM_32(u) == PKM_64(u) || upkm == PKM_64(u)) {
        unpair_ipv6_pfxs = UNPAIR_IPV6_64_PFXS;
    } else {
        unpair_ipv6_pfxs = UNPAIR_IPV6_32_PFXS;
    }

    if (DUAL_MODE(u)) {
        /*
         *  UNPAIR_STATE prefix layout [DUAL MODE]
         *  PKM32                     PKM64
         *  +-----------------------+  +-----------------------+
         *  |*MAX                1  |  |*MAX                1  |
         *  | IPV4 Private       33 |  | IPV4 Private       33 |
         *  | IPV6_32 Private    33 |  | IPV6_64 Private    65 |
         *  |*MAX                1  |  |*MAX                1  |
         *  | IPV4 Gl High       33 |  | IPV4 Gl High       33 |
         *  | IPV4 Gl Low        33 |  | IPV4 Gl Low        33 |
         *  | IPV6_32 Gl High    33 |  | IPV6_64 Gl High    65 |
         *  | IPV6_32 Gl Low     33 |  | IPV6_64 Gl Low     65 |
         *  +-----------------------+  +-----------------------+
 */

        if (vt == VRF_PRIVATE) {
            if (ipv6) {
                base = (UNPAIR_IPV4_PFXS * 2 +
                        unpair_ipv6_pfxs * 2 + 1);
            } else {
                base = (UNPAIR_IPV4_PFXS * 2 +
                        unpair_ipv6_pfxs * 3 + 1);
            }
        } else if (vt == VRF_GLOBAL) {
            if (ipv6) {
                base = 0;
            } else {
                base = (unpair_ipv6_pfxs * 2);
            }
        } else {
            if (ipv6) {
                base = unpair_ipv6_pfxs;
            } else {
                base = (unpair_ipv6_pfxs * 2) +
                       UNPAIR_IPV4_PFXS;
            }
        }
    } else {
        /*
         *  UNPAIR_STATE prefix layout [SINGLE MODE]
         *  PKM32                     PKM64
         *  +-----------------------+ +-----------------------+
         *  |*MAX                1  | |*MAX                1  |
         *  | Reserved           1  | | Reserved           1  |
         *  | IPV4 Gl High       33 | | IPV4 Gl High       33 |
         *  | IPV4 Private       33 | | IPV4 Private       33 |
         *  | IPV4 Gl Low        33 | | IPV4 Gl Low        33 |
         *  | IPV6_32 Gl High    33 | | IPV6_64 Gl High    65 |
         *  | IPV6_32 Private    33 | | IPV6_64 Private    65 |
         *  | IPV6_32 Gl Low     33 | | IPV6_64 Gl Low     65 |
         *  +-----------------------+ +-----------------------+
 */
        if (vt == VRF_PRIVATE) {
            if (ipv6) {
                base = unpair_ipv6_pfxs;
            } else {
                base = UNPAIR_IPV4_PFXS +
                       (unpair_ipv6_pfxs * 3);
            }
        } else if (vt == VRF_GLOBAL) {
            if (ipv6) {
                base = 0;
            } else {
                base = (unpair_ipv6_pfxs * 3);
            }
        } else {
            if (ipv6) {
                base = unpair_ipv6_pfxs * 2;
            } else {
                base = (UNPAIR_IPV4_PFXS * 2) +
                       (unpair_ipv6_pfxs * 3);
            }
        }
    }

    return base;
}


/*!
 * \brief Decode prefix in unpair TCAM to KEY type & VRF type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [out] kt KEY type
 * \param [out] vt VRF type
 * \param [out] len Prefix length
 * \param [in] upkm unpair packing mode
 *
 * \return nothing
 */
static void
unpair_pfx_decode(int u, int pfx, alpm_key_type_t *kt, alpm_vrf_type_t *vt,
                  int *len, int upkm)
{
    int v6;
    alpm_vrf_type_t ivt;
    int i;
    int latest_v6 = 0;
    int base = 0, length = -1;

    if (DUAL_MODE(u)) {
        /* Global --> Override + V6 --> V4 */
        alpm_vrf_type_t gvt[2] = {VRF_GLOBAL, VRF_OVERRIDE};

        for (v6 = 1; v6 >= 0; v6--) {
            for (i = 0; i < 2; i++) {
                ivt = gvt[i];
                base = unpair_pfx_base(u, v6, ivt, upkm);
                if (pfx >= base) {
                    length = pfx - base;
                    latest_v6 = v6;
                    *vt = ivt;
                } else {
                    break;
                }
            }
            if (i < 2) {
                break;
            }
        }

        /* Private + V6 --> V4 */
        for (v6 = 1; v6 >= 0; v6--) {

            ivt = VRF_PRIVATE;
            base = unpair_pfx_base(u, v6, ivt, upkm);
            if (pfx >= base) {
                length = pfx - base;
                latest_v6 = v6;
                *vt = ivt;
            } else {
                break;
            }
        }
    } else {
        alpm_vrf_type_t nz_vt[VRF_TYPES] =
                    {VRF_GLOBAL, VRF_PRIVATE, VRF_OVERRIDE};
        for (v6 = 1; v6 >= 0; v6--) {
            for (i = 0; i < VRF_TYPES; i++) {
                ivt = nz_vt[i];
                base = unpair_pfx_base(u, v6, ivt, upkm);
                if (pfx >= base) {
                    length = pfx - base;
                    latest_v6 = v6;
                    *vt = ivt;
                } else {
                    break;
                }
            }

            if (i < VRF_TYPES) {
                break;
            }
        }
    }
    ALPM_ASSERT(length != -1);
    if (len) {
        *len = length;
    }
    *kt = key_type_get(u, latest_v6, length);
}


/*!
 * \brief Decode prefix in unpair TCAM to KEY type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return KEY type
 */
static inline alpm_key_type_t
unpair_pfx_to_key_type(int u, int pfx, int upkm)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    unpair_pfx_decode(u, pfx, &kt, &vt, NULL, upkm);
    return kt;

}


/*!
 * \brief Decode prefix in unpair TCAM to VRF type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return VRF type
 */
static inline alpm_vrf_type_t
unpair_pfx_to_vrf_type(int u, int pfx, int upkm)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    unpair_pfx_decode(u, pfx, &kt, &vt, NULL, upkm);
    return vt;

}

/*!
 * \brief Determine if prefix in unpair TCAM is IPv4
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return true/false
 */
static inline bool
unpair_pfx_is_ipv4(int u, int pfx, int upkm)
{
    return KEY_IS_V4(unpair_pfx_to_key_type(u, pfx, upkm));
}

/*!
 * \brief Determine if prefix in unpair TCAM is strict IPv6 type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return true/false
 */
static inline bool
unpair_pfx_is_strict_ipv6(int u, int pfx, alpm_key_type_t kt, int upkm)
{
    return (unpair_pfx_to_key_type(u, pfx, upkm) == kt);
}

/*!
 * \brief Encode prefix in unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] ipv6 IPv6
 * \param [in] vt VRF type
 * \param [in] len IP MASK length
 * \param [in] upkm unpair packing mode
 *
 * \return Translated prefix (Global prefix value).
 */
static inline int
unpair_pfx_encode(int u, int ipv6, alpm_vrf_type_t vt, int len, int upkm)
{
    return (unpair_pfx_base(u, ipv6, vt, upkm) + len);
}

/*!
 * \brief Get next prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return INVALID_PFX or free pfx
 */
static int
unpair_free_slot_search_next(int u, int pfx, int upkm)
{
    int free_pfx = pfx;

    while (UNPAIR_STATE_FENT(u, free_pfx, upkm) == 0) {
        free_pfx = UNPAIR_STATE_NEXT(u, free_pfx, upkm);
        if (free_pfx == -1) {
            free_pfx = INVALID_PFX;
            break;
        }
    }
    return free_pfx;
}

/*!
 * \brief Get previous prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return INVALID_PFX or free pfx
 */
static int
unpair_free_slot_search_prev(int u, int pfx, int upkm)
{
    int free_pfx = pfx;

    while (UNPAIR_STATE_FENT(u, free_pfx, upkm) == 0) {
        free_pfx = UNPAIR_STATE_PREV(u, free_pfx, upkm);
        if (free_pfx == -1) {
            free_pfx = INVALID_PFX;
            break;
        }
    }
    return free_pfx;
}

/*!
 * \brief Get prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return INVALID_PFX or free pfx
 */
static int
unpair_free_slot_search(int u, int pfx, int upkm)
{
    int free_pfx = unpair_free_slot_search_next(u, pfx, upkm);

    if (free_pfx == INVALID_PFX) {
        free_pfx = unpair_free_slot_search_prev(u, pfx, upkm);
    }
    return free_pfx;
}

/*!
 * \brief Move entry from from_ent to to_ent.
 *
 * A movable prefix is the highest prefix good to move from pair to unpair.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] from_ent From pt index
 * \param [in] to_ent From pt index
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_shift(int u, int pfx, int from_ent, int to_ent, int upkm)
{
    L1_ENTRY_t de, se;
    bcmdrd_sid_t sid;
    int key_type;
    int full;

    SHR_FUNC_ENTER(u);

    if (from_ent == to_ent) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    key_type = unpair_pfx_to_key_type(u, pfx, upkm);
    sid = unpair_sid(u, key_type);
    SHR_IF_ERR_EXIT(read_l1_raw(u, sid, from_ent, de, se));
    SHR_IF_ERR_EXIT(write_l1_raw(u, sid, to_ent, de, se, pfx));

    from_ent <<= 1;
    to_ent <<= 1;

    full = !half_entry_key_type(u, key_type, upkm);

    SHR_IF_ERR_EXIT(pivot_move(u, from_ent, to_ent, full, 0));
    SHR_IF_ERR_EXIT(pivot_move(u, ++from_ent, ++to_ent, full, 0));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry DOWN.
 *
 * Shift prefix entries 1 entry DOWN, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_shift_down(int u, uint8_t ldb, int pfx, int upkm)
{
    int from_ent;
    int to_ent;
    alpm_key_type_t kt;
    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, upkm);

    to_ent = UNPAIR_STATE_END(u, pfx, upkm) + 1;
    kt = unpair_pfx_to_key_type(u, pfx, upkm);
    if (half_entry_key_type(u, kt, upkm)) {
        if (unpair_half_entry(u, pfx, upkm)) {
            from_ent = UNPAIR_STATE_END(u, pfx, upkm);
            SHR_IF_ERR_EXIT(unpair_shift(u, pfx, from_ent, to_ent, upkm));
            to_ent--;
        }
    }

    from_ent = UNPAIR_STATE_START(u, pfx, upkm);
    if (from_ent != to_ent) {
        SHR_IF_ERR_EXIT(unpair_shift(u, pfx, from_ent, to_ent, upkm));
    }
    UNPAIR_STATE_START(u, pfx, upkm) += 1;
    UNPAIR_STATE_END(u, pfx, upkm) += 1;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry UP.
 *
 * Shift prefix entries 1 entry UP, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_shift_up(int u, uint8_t ldb, int pfx, int upkm)
{
    int from_ent;
    int to_ent;
    alpm_key_type_t kt;
    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, upkm);

    to_ent = UNPAIR_STATE_START(u, pfx, upkm) - 1;

    /* Don't move empty prefix . */
    if (UNPAIR_STATE_VENT(u, pfx, upkm) == 0) {
        ALPM_ASSERT(0);
        UNPAIR_STATE_START(u, pfx, upkm) = to_ent;
        UNPAIR_STATE_END(u, pfx, upkm) = to_ent - 1;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    from_ent = UNPAIR_STATE_END(u, pfx, upkm);

    kt = unpair_pfx_to_key_type(u, pfx, upkm);
    if (half_entry_key_type(u, kt, upkm)) {
        if (unpair_half_entry(u, pfx, upkm) && (from_ent - 1 != to_ent)) {
            SHR_IF_ERR_EXIT(unpair_shift(u, pfx, from_ent - 1, to_ent, upkm));
            to_ent = from_ent - 1;
        }
    }

    SHR_IF_ERR_EXIT(unpair_shift(u, pfx, from_ent, to_ent, upkm));

    UNPAIR_STATE_START(u, pfx, upkm) -= 1;
    UNPAIR_STATE_END(u, pfx, upkm) -= 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an empty prefix into the prefix state link.
 *
 * Create an empty prefix into the prefix state link, also take
 * half free entries from previous prefix.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] prev_pfx Prefix that is just larger than pfx
 * \param [in] upkm unpair packing mode
 *
 * \return Nothing.
 */
static void
unpair_group_create(int u, uint8_t ldb, int pfx, int prev_pfx, int upkm)
{
    int next_pfx = INVALID_PFX;
    int start    = INVALID_INDEX;

    if (prev_pfx == INVALID_PFX) {
        ALPM_ASSERT_ERRMSG(0);
        return;
    }
    SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, upkm);
    SANITY_CHECK_UNPAIR_PFX(u, ldb, prev_pfx, upkm);

    ALPM_ASSERT(UNPAIR_STATE_FENT(u, prev_pfx, upkm));

    /* Insert new prefix into list */
    next_pfx = UNPAIR_STATE_NEXT(u, prev_pfx, upkm);
    if (next_pfx != -1) {
        UNPAIR_STATE_PREV(u, next_pfx, upkm) = pfx;
    }
    UNPAIR_STATE_NEXT(u, pfx, upkm) = next_pfx;
    UNPAIR_STATE_PREV(u, pfx, upkm) = prev_pfx;
    UNPAIR_STATE_NEXT(u, prev_pfx, upkm) = pfx;

    /* Assign half fent from prev_pfx */
    UNPAIR_STATE_FENT(u, pfx, upkm) =
        (UNPAIR_STATE_FENT(u, prev_pfx, upkm) + 1) >> 1;
    UNPAIR_STATE_FENT(u, prev_pfx, upkm) -= UNPAIR_STATE_FENT(u, pfx, upkm);

    /* Setup new prefix */
    start = UNPAIR_STATE_END(u, prev_pfx, upkm) +
            UNPAIR_STATE_FENT(u, prev_pfx, upkm) + 1;
    UNPAIR_STATE_START(u, pfx, upkm)  = start;
    UNPAIR_STATE_END(u, pfx, upkm)    =  start - 1;
    UNPAIR_STATE_VENT(u, pfx, upkm)   = 0;

    SANITY_CHECK_UNPAIR_STATE(u, ldb, prev_pfx, upkm);
    SANITY_CHECK_UNPAIR_STATE(u, ldb, pfx, upkm);

}


/*!
 * \brief Remove freed prefix from the prefix state link.
 *
 * Remove empty prefix from the list and give back all free entries.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] upkm unpair packing mode
 *
 * \return Nothing.
 */
static void
unpair_group_destroy(int u, uint8_t ldb, int pfx, int upkm)
{
    int prev_pfx;
    int next_pfx;

    SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, upkm);

    if (UNPAIR_STATE_PREV(u, pfx, upkm) == -1) {
        return;
    }

    if (UNPAIR_STATE_VENT(u, pfx, upkm) == 0) {
        prev_pfx = UNPAIR_STATE_PREV(u, pfx, upkm); /* Always present */
        SANITY_CHECK_UNPAIR_PFX(u, ldb, prev_pfx, upkm);
        next_pfx = UNPAIR_STATE_NEXT(u, pfx, upkm);

        /* Give away fent to prev_pfx */
        UNPAIR_STATE_FENT(u, prev_pfx, upkm) += UNPAIR_STATE_FENT(u, pfx, upkm);

        /* Remove from list */
        UNPAIR_STATE_NEXT(u, prev_pfx, upkm) = next_pfx;
        if (next_pfx != -1) {
            UNPAIR_STATE_PREV(u, next_pfx, upkm) = prev_pfx;
        }

        /* Reset the state */
        UNPAIR_STATE_FENT(u, pfx, upkm)   = 0;
        UNPAIR_STATE_NEXT(u, pfx, upkm)   = -1;
        UNPAIR_STATE_PREV(u, pfx, upkm)   = -1;
        UNPAIR_STATE_START(u, pfx, upkm)  = -1;
        UNPAIR_STATE_END(u, pfx, upkm)    = -1;
    }

    return;
}

/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx > pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_move_down(int u, uint8_t ldb, int pfx, int free_pfx, int upkm)
{
    int next_pfx;
    SHR_FUNC_ENTER(u);

    while (free_pfx > pfx) {
        next_pfx = UNPAIR_STATE_NEXT(u, free_pfx, upkm);
        SHR_IF_ERR_EXIT(unpair_shift_up(u, ldb, next_pfx, upkm));
        UNPAIR_STATE_FENT(u, free_pfx, upkm) -= 1;
        UNPAIR_STATE_FENT(u, next_pfx, upkm) += 1;
        free_pfx = next_pfx;
        SANITY_CHECK_UNPAIR_STATE(u, ldb, next_pfx, upkm);
        SANITY_CHECK_UNPAIR_STATE(u, ldb, free_pfx, upkm);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx < pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_move_up(int u, uint8_t ldb, int pfx, int free_pfx, int upkm)
{
    int prev_pfx;
    SHR_FUNC_ENTER(u);

    while (free_pfx < pfx) {
        prev_pfx = UNPAIR_STATE_PREV(u, free_pfx, upkm);
        SHR_IF_ERR_EXIT(unpair_shift_down(u, ldb, free_pfx, upkm));
        UNPAIR_STATE_FENT(u, free_pfx, upkm) -= 1;
        UNPAIR_STATE_FENT(u, prev_pfx, upkm) += 1;
        free_pfx = prev_pfx;
        SANITY_CHECK_UNPAIR_STATE(u, ldb, prev_pfx, upkm);
        SANITY_CHECK_UNPAIR_STATE(u, ldb, free_pfx, upkm);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a slot for the prefix.
 *
 * Free a slot for the prefix, rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] del_slot Translated index
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_free(int u, uint8_t ldb, int pfx, int del_slot, int upkm)
{
    int         from_ent;
    int         to_ent;
    L1_ENTRY_t  de1, de2, se1, se2;
    uint32_t    *from_de, *from_se, *to_de, *to_se;
    int         src_slot;
    int         full;
    int         half_kt;
    bcmdrd_sid_t sid;
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;

    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, upkm);

    from_ent = UNPAIR_STATE_END(u, pfx, upkm);
    to_ent = HW_INDEX(del_slot);
    unpair_pfx_decode(u, pfx, &kt, &vt, NULL, upkm);
    sid = unpair_sid(u, kt);

    to_de = de1;
    to_se = se1;
    if (from_ent == to_ent) {
        from_de = to_de;
        from_se = to_se;
    } else {
        from_de = de2;
        from_se = se2;
    }

    half_kt = half_entry_key_type(u, kt, upkm);
    if (half_kt) { /* KEY_IPV4 or KEY_IPV6_32 on PKM0 */
        int from_v1, to_v1;

        SHR_IF_ERR_EXIT(read_l1_raw(u, sid, to_ent, to_de, to_se));
        if (from_ent != to_ent) {
            SHR_IF_ERR_EXIT(read_l1_raw(u, sid, from_ent, from_de, from_se));
        }

        from_v1 = !unpair_half_entry(u, pfx, upkm);
        to_v1 = IS_HALF_R_INDEX(del_slot);
        src_slot = (from_ent << 1) + from_v1;
        del_slot = (to_ent << 1) + to_v1;
        ALPM_ASSERT(src_slot >= del_slot);

        if (src_slot == del_slot) {
            /* Effectively, just clear */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_half_entry_move(u, from_de, to_de, from_v1, to_v1));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_half_entry_move(u, from_se, to_se, from_v1, to_v1));
        } else {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_half_entry_move(u, from_de, to_de, from_v1, to_v1));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_l1_half_entry_move(u, from_se, to_se, from_v1, to_v1));
        }

        SHR_IF_ERR_EXIT(
            write_l1_raw(u, sid, to_ent, to_de, to_se, pfx));

        if (from_ent != to_ent) {
            SHR_IF_ERR_EXIT(
                write_l1_raw(u, sid, from_ent, from_de, from_se, pfx));
        }

        unpair_half_entry_update(u, pfx, upkm);

        if (from_v1) {
            /* First half of full entry is deleted, generating half entry.*/
            UNPAIR_HALF_COUNT(u, kt, vt, upkm)++;
        } else {

            /* Remaining half entry got deleted. Decrement half entry count.*/
            UNPAIR_HALF_COUNT(u, kt, vt, upkm)--;
            UNPAIR_STATE_VENT(u, pfx, upkm) -= 1;
            UNPAIR_STATE_FENT(u, pfx, upkm) += 1;
            UNPAIR_STATE_END(u, pfx, upkm)  -= 1;
        }
    } else { /* IPV6 */
        if (to_ent != from_ent) {
            SHR_IF_ERR_EXIT(read_l1_raw(u, sid, from_ent, from_de, from_se));
            SHR_IF_ERR_EXIT(
                write_l1_raw(u, sid, to_ent, from_de, from_se, pfx));
        }
        src_slot = (from_ent << 1);
        del_slot = (to_ent << 1);
        SHR_IF_ERR_EXIT(clear_l1_raw(u, sid, from_ent));

        UNPAIR_STATE_VENT(u, pfx, upkm) -= 1;
        UNPAIR_STATE_FENT(u, pfx, upkm) += 1;
        UNPAIR_STATE_END(u, pfx, upkm) -= 1;
    }

    full = unpair_pfx_to_key_type(u, pfx, upkm);
    full = !half_entry_key_type(u, full, upkm);

    SHR_IF_ERR_EXIT(pivot_move(u, src_slot, del_slot, full, 1));

    if (UNPAIR_STATE_VENT(u, pfx, upkm) == 0) {
        unpair_group_destroy(u, ldb, pfx, upkm);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allocate a slot for the new prefix.
 *
 * Allocate a slot for the new prefix rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [out] free_slot Translated index
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX.
 */
static int
unpair_free_slot_alloc(int u, uint8_t ldb, int pfx, int *free_slot, int upkm)
{

    int free_pfx, pfx2;
    int prev_pfx = INVALID_PFX;
    int end;
    int half_kt;
    alpm_key_type_t kt;
    SHR_FUNC_ENTER(u);

    SANITY_CHECK_UNPAIR_PFX(u, ldb, pfx, upkm);
    kt = unpair_pfx_to_key_type(u, pfx, upkm);
    half_kt = half_entry_key_type(u, kt, upkm);

    if (half_kt && unpair_half_entry(u, pfx, upkm)) {
        end = UNPAIR_STATE_END(u, pfx, upkm);
        *free_slot = HALF_INDEX_R(end);
        unpair_half_entry_update(u, pfx, upkm);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (UNPAIR_STATE_FENT(u, pfx, upkm) == 0) {
        pfx2 = pfx;

        if (UNPAIR_STATE_VENT(u, pfx, upkm) == 0) {
            alpm_vrf_type_t vt;

            /* Find prev pfx */
            vt = unpair_pfx_to_vrf_type(u, pfx, upkm);
            prev_pfx = unpair_max_pfx(u, kt, vt, upkm);

            while (UNPAIR_STATE_NEXT(u, prev_pfx, upkm) > pfx) {
                prev_pfx = UNPAIR_STATE_NEXT(u, prev_pfx, upkm);
            }
            pfx2 = prev_pfx;
        }

        free_pfx = unpair_free_slot_search(u, pfx2, upkm);

        if (free_pfx == INVALID_PFX) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        } else if (free_pfx > pfx2) {
            SHR_IF_ERR_EXIT(
                unpair_free_slot_move_down(u, ldb, pfx2, free_pfx, upkm));
        } else if (free_pfx < pfx2) {
            SHR_IF_ERR_EXIT(
                unpair_free_slot_move_up(u, ldb, pfx2, free_pfx, upkm));
        }
    }

    if (UNPAIR_STATE_VENT(u, pfx, upkm) == 0 && prev_pfx != INVALID_PFX) {
        unpair_group_create(u, ldb, pfx, prev_pfx, upkm);
    }

    UNPAIR_STATE_VENT(u, pfx, upkm) += 1;
    UNPAIR_STATE_FENT(u, pfx, upkm) -= 1;
    UNPAIR_STATE_END(u, pfx, upkm)  += 1;
    end = UNPAIR_STATE_END(u, pfx, upkm);
    if (half_kt) {
        unpair_half_entry_update(u, pfx, upkm);
    }

    SANITY_CHECK_UNPAIR_STATE(u, ldb, pfx, upkm);

    *free_slot = half_kt ? HALF_INDEX_L(end) : FULL_INDEX(end);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an entry into unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be inserted
 * \param [in] upkm unpair packing mode
 *
 * \return SHR_E_XXX
 */
static int
unpair_insert(int u, alpm_arg_t *arg, int upkm)
{
    bool new_add;
    SHR_FUNC_ENTER(u);

    if (arg->index[LEVEL1] == INVALID_INDEX) {
        int rv;
        arg->key.pfx = unpair_pfx_encode(u, arg->key.t.ipv, arg->key.vt,
                                         arg->key.t.len, upkm);
        rv = unpair_free_slot_alloc(u, arg->l1_db,
                                    arg->key.pfx, &arg->index[LEVEL1], upkm);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_FULL) {
            /* Shut error messages prints, as E_FULL is really not an error */
                SHR_RETURN_VAL_EXIT(SHR_E_FULL);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        new_add = true;
    } else {
        new_add = false;
    }

    if (arg->skip_write == WRITE_NORMAL || arg->skip_write == WRITE_NEW) {
        bcmdrd_sid_t sid = unpair_sid(u, arg->key.kt);
        arg->upkm = upkm; /* for Ver1 L1 encode */
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry(u, sid, arg,
                                  new_add || arg->skip_write == WRITE_NEW));
    }

    if (new_add) {
        alpm_key_type_t kt;
        alpm_vrf_type_t vt;

        if (!arg->in_move) {
            alpm_pivot_t *pivot;
            int index2;
            if (arg->a1.direct_route) {
                SHR_IF_ERR_EXIT(l1_pvt_alloc(u, arg, &pivot));
                SHR_IF_ERR_EXIT(l1_pvt_insert(u, arg, pivot));
                arg->pivot[LEVEL1] = pivot;
            } else {
                pivot = arg->pivot[LEVEL1];
            }
            ALPM_ASSERT(pivot);
            index2 = arr_idx_from_pvt_idx(u, arg->index[LEVEL1]);
            L1_PVT_INFO(u, index2) = pivot;
        }

        kt = arg->key.kt;
        vt = arg->key.vt;
        ALPM_ASSERT_ERRMSG(kt > KEY_TYPE_INVALID && kt < KEY_TYPES);
        ALPM_ASSERT_ERRMSG(vt > VRF_TYPE_INVALID && vt < VRF_TYPES);
        UNPAIR_USED_COUNT(u, kt, vt, upkm)++;
        if (half_entry_key_type(u, kt, upkm)) {
            if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
                UNPAIR_HALF_COUNT(u, kt, vt, upkm)--;
            } else {
                UNPAIR_HALF_COUNT(u, kt, vt, upkm)++;
            }
        }
        /* VRF_PIVOT_REF_INC(u, vrf_id, vrf, kt);*/
    } else if (arg->a1.direct_route) {
        ALPM_ASSERT(arg->pivot[LEVEL1]);
        sal_memcpy(arg->pivot[LEVEL1]->ad, &arg->ad, sizeof(assoc_data_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from unpair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be deleted
 *
 * \return SHR_E_XXX
 */
static int
unpair_delete(int u, alpm_arg_t *arg)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    alpm_pivot_t    *pivot = NULL;
    int upkm;

    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(arg->index[LEVEL1] != INVALID_INDEX);

    upkm = index_to_pkm(u, HW_INDEX(arg->index[LEVEL1]));
    arg->key.pfx = unpair_pfx_encode(u, arg->key.t.ipv, arg->key.vt, arg->key.t.len, upkm);

    SHR_IF_ERR_EXIT(
        unpair_free_slot_free(u, arg->l1_db,
                              arg->key.pfx, arg->index[LEVEL1], upkm));

    if (!arg->in_move && arg->a1.direct_route) {
        SHR_IF_ERR_EXIT(l1_pvt_delete(u, arg, &pivot));
        SHR_IF_ERR_EXIT(l1_pvt_free(u, arg->db, pivot));
    }

    kt = arg->key.kt;
    vt = arg->key.vt;
    UNPAIR_USED_COUNT(u, kt, vt, upkm)--;

    /* VRF_PIVOT_REF_DEC(u, w_vrf, kt); */
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get args from unpair TCAM for a given index
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM arg with index set
 *
 * \return SHR_E_XXX
 */
static inline int
unpair_get(int u, alpm_arg_t *arg)
{
    bcmdrd_sid_t sid = unpair_sid(u, arg->key.kt);
    arg->upkm = index_to_pkm(u, HW_INDEX(arg->index[LEVEL1]));
    return decode_read_l1_entry(u, sid, arg, NULL);
}

/*!
 * \brief Get the larget prefix in unpair TCAM
 *
 * Get the largest IPv4 prefix, if not exist,
 * get the largest IPv6 prefix.
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type
 * \param [in] upkm Unpair packing mode
 *
 * \return SHR_E_XXX
 */
static inline int
unpair_largest_prefix(int u, alpm_vrf_type_t vt, int upkm)
{
    int max_pfx;
    max_pfx = unpair_max_pfx(u, KEY_IPV4, vt, upkm);
    return UNPAIR_STATE_NEXT(u, max_pfx, upkm);
}

static int
unpair64_smallest_prefix(int u, alpm_vrf_type_t vt)
{
    int curr_pfx;
    int move_pfx = -1;

    /* Try to get IPv6_32 first */
    curr_pfx = unpair_max_pfx(u, KEY_IPV6_128, vt, PKM_64(u));
    while (curr_pfx != -1) {
        if (unpair_pfx_is_strict_ipv6(u, curr_pfx, KEY_IPV6_32, PKM_64(u))) {
            move_pfx = curr_pfx;
        }
        curr_pfx = UNPAIR_STATE_NEXT(u, curr_pfx, PKM_64(u));
    }

    /* Try to get IPv4 second */
    if (move_pfx == -1) {
        curr_pfx = unpair_max_pfx(u, KEY_IPV4, vt, PKM_64(u));
        while (curr_pfx != -1) {
            if (unpair_pfx_is_ipv4(u, curr_pfx, PKM_64(u))) {
                move_pfx = curr_pfx;
            }
            curr_pfx = UNPAIR_STATE_NEXT(u, curr_pfx, PKM_64(u));
        }
    }

    return move_pfx;
}

static int
unpair_state_init(int u, bool recover)
{
    int i;
    int max_pfx;
    int pfx_state_size;
    int fent;
    int upkm;
    int start = 0;
    SHR_FUNC_ENTER(u);

    upkm = PKM_64(u);
    fent = SIZE_PAIR(u);

    do {
        max_pfx = unpair_num_pfxs(u, KEY_IPV4, VRF_PRIVATE, upkm);
        pfx_state_size = sizeof(unpair_state_t) * (max_pfx);

        ALPM_ALLOC(UNPAIR_STATE(u, upkm), pfx_state_size, "LPM prefix info");
        sal_memset(UNPAIR_STATE(u, upkm), 0, pfx_state_size);
        for (i = 0; i < max_pfx; i++) {
            UNPAIR_STATE_START(u, i, upkm) = -1;
            UNPAIR_STATE_END(u, i, upkm)   = -1;
            UNPAIR_STATE_PREV(u, i, upkm)  = -1;
            UNPAIR_STATE_NEXT(u, i, upkm)  = -1;
            UNPAIR_STATE_VENT(u, i, upkm)  = 0;
            UNPAIR_STATE_FENT(u, i, upkm)  = 0;
        }

        start += fent;
        fent = SIZE_UNPAIR(u, upkm);
        if (DUAL_MODE(u)) {
            UNPAIR_MAX_PFX(u, L1_DB0, upkm) =
                unpair_max_pfx(u, KEY_IPV4, VRF_PRIVATE, upkm);
            UNPAIR_MAX_PFX(u, L1_DB1, upkm) =
                unpair_max_pfx(u, KEY_IPV4, VRF_GLOBAL, upkm);

            max_pfx = UNPAIR_MAX_PFX(u, L1_DB0, upkm);
            UNPAIR_STATE_FENT(u, max_pfx, upkm) = fent >> 1;
            UNPAIR_STATE_START(u, max_pfx, upkm) = start;
            UNPAIR_STATE_END(u, max_pfx, upkm) = start - 1;

            max_pfx = UNPAIR_MAX_PFX(u, L1_DB1, upkm);
            UNPAIR_STATE_FENT(u, max_pfx, upkm) = fent - (fent >> 1);
            UNPAIR_STATE_START(u, max_pfx, upkm) = start + (fent >> 1);
            UNPAIR_STATE_END(u, max_pfx, upkm) =
                UNPAIR_STATE_START(u, max_pfx, upkm) - 1;

            if (recover) {
                UNPAIR_STATE_FENT(u, UNPAIR_MAX_PFX(u, L1_DB0, upkm), upkm) = 0;
                UNPAIR_STATE_FENT(u, UNPAIR_MAX_PFX(u, L1_DB1, upkm), upkm) = 0;
            }
        } else {
            UNPAIR_MAX_PFX(u, L1_DB0, upkm) =
                unpair_max_pfx(u, KEY_IPV4, VRF_PRIVATE, upkm);
            UNPAIR_MAX_PFX(u, L1_DB1, upkm) = -1;
            max_pfx = UNPAIR_MAX_PFX(u, L1_DB0, upkm);
            UNPAIR_STATE_FENT(u, max_pfx, upkm) = fent;
            UNPAIR_STATE_START(u, max_pfx, upkm) = start;
            UNPAIR_STATE_END(u, max_pfx, upkm) = start - 1;
            if (recover) {
                UNPAIR_STATE_FENT(u, max_pfx, upkm) = 0;
            }
        }

        upkm = (upkm == PKM_32(u)) ? PKM_64(u) : PKM_32(u);
    } while (upkm != PKM_64(u));

exit:
    SHR_FUNC_EXIT();
}

static int
unpair_stats_init(int u, bool recover)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    int upkm;

    SHR_FUNC_ENTER(u);

    upkm = PKM_64(u);

    do {
        for (vt = VRF_PRIVATE; vt < VRF_TYPES; vt++) {
            for (kt = KEY_IPV4; kt < KEY_TYPES; kt++) {
                UNPAIR_USED_COUNT(u, kt, vt, upkm) = 0;
                UNPAIR_HALF_COUNT(u, kt, vt, upkm) = 0;
            }
        }

        if (!DUAL_MODE(u)) {
            UNPAIR_MAX_COUNT(u, KEY_IPV4, VRF_PRIVATE, upkm) =
                SIZE_UNPAIR(u, upkm) * 2;

            UNPAIR_MAX_COUNT(u, KEY_IPV6_64, VRF_PRIVATE, upkm) =
                SIZE_UNPAIR(u, upkm);

            UNPAIR_MAX_COUNT(u, KEY_IPV6_128, VRF_PRIVATE, upkm) = 0;
        } else {
            UNPAIR_MAX_COUNT(u, KEY_IPV4, VRF_PRIVATE, upkm) =
                SIZE_UNPAIR(u, upkm);

            UNPAIR_MAX_COUNT(u, KEY_IPV6_64, VRF_PRIVATE, upkm) =
                SIZE_UNPAIR(u, upkm) >> 1;

            UNPAIR_MAX_COUNT(u, KEY_IPV6_128, VRF_PRIVATE, upkm) = 0;

            UNPAIR_MAX_COUNT(u, KEY_IPV4, VRF_GLOBAL, upkm)  =
                SIZE_UNPAIR(u, upkm);

            UNPAIR_MAX_COUNT(u, KEY_IPV6_64, VRF_GLOBAL, upkm)  =
                SIZE_UNPAIR(u, upkm) >> 1;

            UNPAIR_MAX_COUNT(u, KEY_IPV6_128, VRF_GLOBAL, upkm)  = 0;
        }

        upkm = (upkm == PKM_32(u)) ? PKM_64(u) : PKM_32(u);
    } while (upkm != PKM_64(u));


    SHR_FUNC_EXIT();
}

static void
unpair_state_cleanup(int u)
{
    int upkm;

    if (L1_INFO(u)) {
        upkm = PKM_64(u);
        do {
            SHR_FREE(UNPAIR_STATE(u, upkm));
            upkm = (upkm == PKM_32(u)) ? PKM_64(u) : PKM_32(u);
        } while (upkm != PKM_64(u));
    }
}

static void
unpair_stats_cleanup(int u)
{
    return;
}


static inline bcmdrd_sid_t
pair_sid(int u, alpm_key_type_t key_type)
{
    return (key_type == KEY_IPV4) ? UNPAIR_SID(u) : PAIR_SID(u);
}

static inline void
pair_half_entry_update(int u, int pfx)
{
    PAIR_STATE_HENT(u, pfx) = !PAIR_STATE_HENT(u, pfx);
}

static inline int
pair_half_entry(int u, int pfx)
{
    return PAIR_STATE_HENT(u, pfx);
}

/*!
 * \brief Determine whether a given index belongs to IPv6 range
 *
 * \param [in] u Device u.
 * \param [in] index PAIR state index
 *
 * \return true/false
 */
static inline int
pair_index_is_ipv6(int u, uint8_t ldb, int index)
{
    int v6 = 0;

    if ((index < RANGE1_FIRST(u, ldb)) ||
        (index > RANGE1_LAST(u, ldb) && index < RANGE2_FIRST(u, ldb))) {
        v6 = 1;
    } else {
        v6 = 0;
    }
    return v6;
}

/*!
 * \brief Map single-wide index to dual-wide index
 *
 * \param [in] u Device u.
 * \param [in] idx single-wide index
 *
 * \return Dual-wide index
 */
static inline int
pair_index_encode(int u, int idx)
{
    return (idx & (TCAM_DEPTH(u) - 1)) + ((idx >> 1) & ~(TCAM_DEPTH(u) - 1));
}

/*!
 * \brief Map dual-wide index to single-wide index
 *
 * \param [in] u Device u.
 * \param [in] idx dual-wide index
 *
 * \return Single-wide index
 */
static inline int
pair_index_decode(int u, int idx)
{
    return (idx & (TCAM_DEPTH(u) - 1)) +
           ((idx << 1) & ~(2 * TCAM_DEPTH(u) - 1));
}

/*!
 * \brief Get number of prefixes in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 *
 * \return Number of prefixes
 */
static int
pair_num_pfxs(int u, alpm_key_type_t key_type, alpm_vrf_type_t vrf_type)
{
    int num_pfx;
    int v6 = KEY_IS_V6(key_type);

    if (DUAL_MODE(u)) {
        if (vrf_type == VRF_PRIVATE) {
            num_pfx = v6 ? ((PAIR_IPV4_PFXS * 3) +
                            (PAIR_IPV6_128_PFXS * 3) + 4)
                         : ((PAIR_IPV4_PFXS * 3) +
                            (PAIR_IPV6_128_PFXS * 2) + 3);
        } else {
            num_pfx = v6 ? ((PAIR_IPV4_PFXS * 2) +
                            (PAIR_IPV6_128_PFXS * 2) + 2)
                         : ((PAIR_IPV4_PFXS * 2) + 1);
        }
    } else {
        num_pfx = v6 ? ((PAIR_IPV4_PFXS * 3) +
                        (PAIR_IPV6_128_PFXS * 3) + 4)
                     : ((PAIR_IPV4_PFXS * 3) + 2);
    }

    return num_pfx;
}


/*!
 * \brief Get max prefix in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 *
 * \return MAX prefix in pair TCAM
 */
static inline int
pair_max_pfx(int u, alpm_key_type_t key_type, alpm_vrf_type_t vrf_type)
{

    return pair_num_pfxs(u, key_type, vrf_type) - 1;
}

/*!
 * \brief Get min valid prefix in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 *
 * \return MIN valid prefix in pair TCAM
 */
static inline int
pair_min_pfx(int u, alpm_key_type_t kt, alpm_vrf_type_t vt)
{
    int min_pfx = pair_max_pfx(u, kt, vt);

    while (PAIR_STATE_NEXT(u, min_pfx) != -1) {
        min_pfx = PAIR_STATE_NEXT(u, min_pfx);
    }
    return min_pfx;
}

/*!
 * \brief Get prefix base in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 *
 * \return Prefix base
 */
static int
pair_pfx_base(int u, int ipv6, alpm_vrf_type_t vrf_type)
{
    int base = 0;

    if (DUAL_MODE(u)) {
        /*
         *  PAIR_STATE prefix layout [DUAL MODE]:
         *  +------------------------+
         *  |* PRIVATE_MAX_128    1  |
         *  |  IPV6_128 Private   129|
         *  |* PRIVATE_MAX_V4     1  |
         *  |  IPV4 Private       33 |
         *  |* GLOBAL_MAX_128     1  |
         *  |  IPV6_128 Gl High   129|
         *  |  IPV6_128 Gl Low    129|
         *  |* GLOBAL_MAX_V4      1  |
         *  |  IPV4 Gl High       33 |
         *  |  IPV4 Gl Low        33 |
         *  +------------------------+
 */
        if (vrf_type == VRF_PRIVATE) {
            if (ipv6) {
                base = (PAIR_IPV4_PFXS * 3) +
                       (PAIR_IPV6_128_PFXS * 2) + 3;
            } else {
                base = (PAIR_IPV4_PFXS * 2) +
                       (PAIR_IPV6_128_PFXS * 2) + 2;
            }
        } else if (vrf_type == VRF_GLOBAL) {
            if (ipv6) {
                base = (PAIR_IPV4_PFXS * 2) + 1;
            } else {
                base = 0;
            }
        } else {
            if (ipv6) {
                base = (PAIR_IPV4_PFXS * 2) +
                        PAIR_IPV6_128_PFXS + 1;
            } else {
                base = PAIR_IPV4_PFXS;
            }
        }
    } else {
        /*
         *  PAIR_STATE prefix layout [SINGLE MODE]:
         *  +------------------------+
         *  |* MAX_128            1  |
         *  |* Reserved           1  |
         *  |  IPV6_128 Gl High   129|
         *  |  IPV6_128 Private   129|
         *  |  IPV6_128 Gl Low    129|
         *  |* MAX_V4             1  |
         *  |* Reserved           1  |
         *  |  IPV4 Gl High       33 |
         *  |  IPV4 Private       33 |
         *  |  IPV4 Gl Low        33 |
         *  +------------------------+
 */
        if (vrf_type == VRF_PRIVATE) {
            if (ipv6) {
                base = (PAIR_IPV4_PFXS * 3) +
                       PAIR_IPV6_128_PFXS + 2;
            } else {
                base = PAIR_IPV4_PFXS;
            }
        } else if (vrf_type == VRF_GLOBAL) {
            if (ipv6) {
                base = (PAIR_IPV4_PFXS * 3) + 2;
            } else {
                base = 0;
            }
        } else {
            if (ipv6) {
                base = (PAIR_IPV4_PFXS * 3) +
                       (PAIR_IPV6_128_PFXS * 2) + 2;
            } else {
                base = PAIR_IPV4_PFXS * 2;
            }
        }
    }

    return base;
}


/*!
 * \brief Decode prefix in pair TCAM to KEY type & VRF type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [out] kt KEY type
 * \param [out] vt VRF type
 * \param [out] len Prefix length
 *
 * \return nothing
 */
static void
pair_pfx_decode(int u, int pfx, alpm_key_type_t *kt,
                alpm_vrf_type_t *vt, int *len)
{
    int v6, length = -1;
    alpm_vrf_type_t ivt;
    int i;
    int latest_v6 = 0;
    int base = 0;

    if (DUAL_MODE(u)) {
        /* Global --> Override + V4 --> V6_128*/
        alpm_vrf_type_t gvt[2] = {VRF_GLOBAL, VRF_OVERRIDE};

        for (v6 = 0; v6 < 2; v6++) {
            for (i = 0; i < 2; i++) {
                ivt = gvt[i];
                base = pair_pfx_base(u, v6, ivt);
                if (pfx >= base) {
                    length = pfx - base;
                    latest_v6 = v6;
                    *vt = ivt;
                } else {
                    break;
                }
            }
            if (i < 2) {
                break;
            }
        }

        /* Private + V4 --> V6_128*/
        for (v6 = 0; v6 < 2; v6++) {
            ivt = VRF_PRIVATE;
            base = pair_pfx_base(u, v6, ivt);
            if (pfx >= base) {
                length = pfx - base;
                latest_v6 = v6;
                *vt = ivt;
            } else {
                break;
            }

        }
    } else {
        alpm_vrf_type_t nz_vt[VRF_TYPES] = {VRF_GLOBAL, VRF_PRIVATE, VRF_OVERRIDE};
        for (v6 = 0; v6 < 2; v6++) {
            for (i = 0; i < VRF_TYPES; i++) {
                ivt = nz_vt[i];
                base = pair_pfx_base(u, v6, ivt);
                if (pfx >= base) {
                    length = pfx - base;
                    latest_v6 = v6;
                    *vt = ivt;
                } else {
                    break;
                }
            }

            if (i < VRF_TYPES) {
                break;
            }
        }
    }

    ALPM_ASSERT(length != -1);
    if (len) {
        *len = length;
    }
    *kt = key_type_get(u, latest_v6, length);
}


/*!
 * \brief Decode prefix in pair TCAM to VRF type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return VRF type
 */
static inline alpm_vrf_type_t
pair_pfx_to_vrf_type(int u, int pfx)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    pair_pfx_decode(u, pfx, &kt, &vt, NULL);
    return vt;
}

/*!
 * \brief Decode prefix in pair TCAM to KEY type
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return KEY type
 */
static inline alpm_key_type_t
pair_pfx_to_key_type(int u, int pfx)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    pair_pfx_decode(u, pfx, &kt, &vt, NULL);
    return kt;
}

/*!
 * \brief Encode prefix in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] ipv6 IPv6
 * \param [in] vt VRF type
 * \param [in] len IP MASK length
 *
 * \return Translated prefix (Global prefix value).
 */
static inline int
pair_pfx_encode(int u, int ipv6, alpm_vrf_type_t vt, int len)
{
    return (pair_pfx_base(u, ipv6, vt) + len);
}


/*!
 * \brief Determine if prefix in pair TCAM is IPv4
 * Including IPv4 Max prefix.
 *
 * \param [in] u Device u.
 * \param [in] ldb Level1 DB
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return true/false
 */
static inline bool
pair_pfx_is_ipv4(int u, uint8_t ldb, int pfx)
{
    if (pfx != PAIR_MAX_PFX(u, ldb)) {

        if (pfx == PAIR_MAX_V4_PFX(u, ldb) ||
            KEY_IS_V4(pair_pfx_to_key_type(u, pfx))) {
            return true;
        }
    }
    return false;
}

/*!
 * \brief Determine if prefix in pair TCAM is IPv6
 * Including IPv6 Max prefix.
 *
 * \param [in] u Device u.
 * \param [in] ldb Level1 DB
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return true/false
 */
static inline int
pair_pfx_is_ipv6(int u, uint8_t ldb, int pfx)
{
    return !pair_pfx_is_ipv4(u, ldb, pfx);
}



/*!
 * \brief Determine if prefix in pair TCAM is strict IPv6_64
 * Excluding IPv6 Max prefix.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] kt KEY type
 *
 * \return true/false
 */
static inline int
pair_pfx_is_strict_ipv6(int u, uint8_t ldb, int pfx, alpm_key_type_t kt)
{
    int strict_v6 = 0;

    if (pfx >= 0 &&
        pfx != PAIR_MAX_PFX(u, ldb) &&
        pfx != PAIR_MAX_V4_PFX(u, ldb)) {

        strict_v6 = (pair_pfx_to_key_type(u, pfx) == kt);
    }
    return strict_v6;
}

/*!
 * \brief Determine if prefix in pair TCAM is loose IPv6_64
 * Excluding IPv6 Max prefix.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return true/false
 */
static inline int
pair_pfx_is_ipv6_64(int u, uint8_t ldb, int pfx)
{
    return pair_pfx_is_strict_ipv6(u, ldb, pfx, KEY_IPV6_32) ||
           pair_pfx_is_strict_ipv6(u, ldb, pfx, KEY_IPV6_64);
}


/*!
 * \brief Get next pair state index for a given index
 *
 * \param [in] u Device u.
 * \param [in] index state index
 *
 * \return Next state index
 */
static inline int
pair_next_index(int u, uint8_t ldb, int index)
{
    int next_idx;

    if (pair_index_is_ipv6(u, ldb, index)) {
        next_idx = index + 1;
        if (index_in_right_tcam(u, next_idx)) {
            next_idx += TCAM_DEPTH(u);
        }
    } else {
        if (index == RANGE1_LAST(u, ldb)) {
            next_idx = RANGE2_FIRST(u, ldb);
        } else {
            next_idx = index + 1;
        }
    }

    /* The equal is for a special case - max_v4_pfx */
    ALPM_ASSERT(next_idx <= SIZE_PAIR(u));

    return next_idx;
}

/*!
 * \brief Get previous pair state index for a given index
 *
 * \param [in] u Device u.
 * \param [in] index state index
 *
 * \return Previous state index
 */
static int
pair_prev_index(int u, uint8_t ldb, int index)
{
    int prev_idx;

    if (pair_index_is_ipv6(u, ldb, index)) {
        prev_idx = index - 1;
        if (index_in_right_tcam(u, prev_idx)) {
            prev_idx -= TCAM_DEPTH(u);
        }
    } else {

        if (index == RANGE2_FIRST(u, ldb)) {
            prev_idx = RANGE1_LAST(u, ldb);
        } else {
            prev_idx = index - 1;
        }
    }

    return prev_idx;
}


/*!
 * \brief Get previous prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] req_fent Requesting number of free entries.
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_prev(int u, int pfx, int req_fent)
{
    int free_pfx = pfx;
    int fent = PAIR_STATE_FENT(u, pfx);

    while (fent < req_fent) {
        free_pfx = PAIR_STATE_PREV(u, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on left side */
            free_pfx = INVALID_PFX;
            break;
        }
        fent += PAIR_STATE_FENT(u, free_pfx);
    }
    return free_pfx;
}


/*!
 * \brief Get next prefix with free slot
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 * \param [in] req_fent Requesting number of free entries.
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_next(int u, int pfx, int req_fent)
{
    int free_pfx = pfx;
    int fent = PAIR_STATE_FENT(u, pfx);

    while (fent < req_fent) {
        free_pfx = PAIR_STATE_NEXT(u, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on right side as well */
            free_pfx = INVALID_PFX;
            break;
        }
        fent += PAIR_STATE_FENT(u, free_pfx);
    }
    return free_pfx;
}

/*!
 * \brief Get prefix with free slot for IPv6 in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_for_v6(int u, int pfx)
{
    int free_pfx, free_pfx2;

    free_pfx = pair_free_slot_search_prev(u, pfx, 2);

    if (free_pfx == INVALID_PFX) {
        free_pfx = pair_free_slot_search_next(u, pfx, 2);
    }

    if (free_pfx == INVALID_PFX) {
        alpm_vrf_type_t vt;
        int v4_max_pfx;

        vt = pair_pfx_to_vrf_type(u, pfx);
        v4_max_pfx = pair_max_pfx(u, KEY_IPV4, vt);

        free_pfx = pair_free_slot_search_next(u, v4_max_pfx, 1);

        if (free_pfx != INVALID_PFX && PAIR_STATE_FENT(u, free_pfx) == 1) {
            /* just to ensure free entries are enough, still return
             * the first found free_pfx
             */
            free_pfx2 = PAIR_STATE_NEXT(u, free_pfx);
            if (free_pfx2 != INVALID_PFX) {
                free_pfx2 = pair_free_slot_search_next(u, free_pfx2, 1);

                ALPM_ASSERT(free_pfx2 != free_pfx);
            }
            if (free_pfx2 == INVALID_PFX) {
                free_pfx = INVALID_PFX; /* no enough free entries */
            }
        }
    }
    return free_pfx;
}

/*!
 * \brief Get prefix with free slot for IPv4 in pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] pfx Starting translated prefix (Global prefix value).
 *
 * \return INVALID_PFX or free pfx
 */
static int
pair_free_slot_search_for_v4(int u, int pfx)
{
    int free_pfx;

    free_pfx = pair_free_slot_search_next(u, pfx, 1);

    if (free_pfx == INVALID_PFX) {
        free_pfx = pair_free_slot_search_prev(u, pfx, 1);
    }

    if (free_pfx == INVALID_PFX) {
        alpm_vrf_type_t vrf_type;
        int v6_min_pfx;

        vrf_type = pair_pfx_to_vrf_type(u, pfx);
        v6_min_pfx = pair_min_pfx(u, KEY_IPV6_64, vrf_type);

        free_pfx = pair_free_slot_search_prev(u, v6_min_pfx, 1);
    }

    return free_pfx;
}


/*!
 * \brief Move entry from from_ent to to_ent.
 *
 * A movable prefix is the highest prefix good to move from pair to unpair.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] from_ent From pt index
 * \param [in] to_ent From pt index
 * \param [in] erase Clear from entry
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift(int u, int pfx, int from_ent, int to_ent, int erase)
{
    int full;
    int key_type;
    L1_ENTRY_t de, se;
    bcmdrd_sid_t sid;
    SHR_FUNC_ENTER(u);

    if (from_ent == to_ent) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    key_type = pair_pfx_to_key_type(u, pfx);
    sid = pair_sid(u, key_type);
    SHR_IF_ERR_EXIT(read_l1_raw(u, sid, from_ent, de, se));
    SHR_IF_ERR_EXIT(write_l1_raw(u, sid, to_ent, de, se, pfx));

    if (erase) {
        SHR_IF_ERR_EXIT(clear_l1_raw(u, sid, from_ent));
    }

    from_ent <<= 1;
    to_ent <<= 1;

    full = !half_entry_key_type(u, key_type, PKM_128(u));

    SHR_IF_ERR_EXIT(pivot_move(u, from_ent, to_ent, full, 0));
    SHR_IF_ERR_EXIT(pivot_move(u, ++from_ent, ++to_ent, full, 0));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check and update range1 last pfx if required.
 *
 * Update last pfx and its span state based on a given pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_range1_last_pfx_update(int u, uint8_t ldb, int pfx)
{
    int prev_pfx;
    int next_pfx;
    int start, end;
    SHR_FUNC_ENTER(u);

    start = PAIR_STATE_START(u, pfx);
    end = PAIR_STATE_END(u, pfx);

    if (LAST_PFX_SPAN(u, ldb)) {
        if (RANGE1_LAST_PFX(u, ldb) == pfx) {
            if (end <= RANGE1_LAST(u, ldb)) {
                LAST_PFX_SPAN(u, ldb) = 0;

            } else if (start >= RANGE2_FIRST(u, ldb)) {
                LAST_PFX_SPAN(u, ldb) = 0;
                RANGE1_LAST_PFX(u, ldb) =
                    PAIR_STATE_PREV(u, RANGE1_LAST_PFX(u, ldb));
            }
        }
    } else {
        if (start <= RANGE1_LAST(u, ldb) && end >= RANGE2_FIRST(u, ldb)) {
            LAST_PFX_SPAN(u, ldb) = 1;
            RANGE1_LAST_PFX(u, ldb) = pfx;

        } else if (start <= RANGE1_LAST(u, ldb)) {
            next_pfx = PAIR_STATE_NEXT(u, pfx);

            if (next_pfx == INVALID_PFX ||
                PAIR_STATE_START(u, next_pfx) >= RANGE2_FIRST(u, ldb)) {
                RANGE1_LAST_PFX(u, ldb) = pfx;
            }

        } else if (start >= RANGE2_FIRST(u, ldb)) {
            SANITY_CHECK_PAIR_PFX(u, ldb, pfx);

            prev_pfx = PAIR_STATE_PREV(u, pfx);
            ALPM_ASSERT(prev_pfx != INVALID_PFX);
            if (PAIR_STATE_END(u, prev_pfx) <= RANGE1_LAST(u, ldb)) {
                RANGE1_LAST_PFX(u, ldb) = prev_pfx;
            }
        }
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift IPv4 prefix entries 1 entry DOWN.
 *
 * Shift IPv4 prefix entries 1 entry DOWN, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_v4_down(int u, uint8_t ldb, int pfx)
{
    int end;
    int end_next;
    int start;
    int start_next;
    SHR_FUNC_ENTER(u);

    start = PAIR_STATE_START(u, pfx);
    end = PAIR_STATE_END(u, pfx);

    SANITY_CHECK_PAIR_PFX(u, ldb, pfx);

    end_next   = pair_next_index(u, ldb, end);
    start_next = pair_next_index(u, ldb, start);

    ALPM_ASSERT(PAIR_STATE_VENT(u, pfx));

    if (pair_half_entry(u, pfx)) {
        /* Last entry is half full -> keep it last. */
        SHR_IF_ERR_EXIT(pair_shift(u, pfx, end, end_next, 0));
        SHR_IF_ERR_EXIT(pair_shift(u, pfx, start, end, 0));
    } else {
        /* No need to erase start entry, as it will be done by caller. */
        SHR_IF_ERR_EXIT(pair_shift(u, pfx, start, end_next, 0));
    }

    PAIR_STATE_START(u, pfx) = start_next;
    PAIR_STATE_END(u, pfx)   = end_next;

    pair_range1_last_pfx_update(u, ldb, pfx);

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Shift IPv4 prefix entries 1 entry UP.
 *
 * Shift IPv4 prefix entries 1 entry UP, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_v4_up(int u, uint8_t ldb, int pfx)
{
    int end;
    int end_prev;
    int start;
    int start_prev = -1;
    SHR_FUNC_ENTER(u);

    start = PAIR_STATE_START(u, pfx);
    end   = PAIR_STATE_END(u, pfx);

    end_prev   = pair_prev_index(u, ldb, end);
    start_prev = pair_prev_index(u, ldb, start);

    /* Don't move empty prefix .
     * This condition is hit if pfx is created for the first time
     */
    if (PAIR_STATE_VENT(u, pfx) == 0) {
        ALPM_ASSERT(0);

        PAIR_STATE_START(u, pfx) = start_prev;
        PAIR_STATE_END(u, pfx) = pair_prev_index(u, ldb, start_prev);
        pair_range1_last_pfx_update(u, ldb, pfx);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }


    if (pair_half_entry(u, pfx)) {
        SHR_IF_ERR_EXIT(pair_shift(u, pfx, end_prev, start_prev, 0));
        SHR_IF_ERR_EXIT(pair_shift(u, pfx, end, end_prev, 1));
    } else {
        SHR_IF_ERR_EXIT(pair_shift(u, pfx, end, start_prev, 1));
    }

    PAIR_STATE_START(u, pfx) = start_prev;
    PAIR_STATE_END(u, pfx)   = end_prev;

    pair_range1_last_pfx_update(u, ldb, pfx);


exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry DOWN.
 *
 * Shift prefix entries 1 entry DOWN, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_down(int u, uint8_t ldb, int pfx)
{
    int start;
    int start_next;
    int end;
    int end_next;
    SHR_FUNC_ENTER(u);

    if (pair_pfx_is_ipv4(u, ldb, pfx)) {
        SHR_RETURN_VAL_EXIT(pair_shift_v4_down(u, ldb, pfx));
    }

    start = PAIR_STATE_START(u, pfx);
    end   = PAIR_STATE_END(u, pfx);
    end_next   = pair_next_index(u, ldb, end);
    start_next = pair_next_index(u, ldb, start);

    SHR_IF_ERR_EXIT(pair_shift(u, pfx, start, end_next, 0));

    PAIR_STATE_START(u, pfx) = start_next;
    PAIR_STATE_END(u, pfx) = end_next;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shift prefix entries 1 entry UP.
 *
 * Shift prefix entries 1 entry UP, while preserving
 * last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_up(int u, uint8_t ldb, int pfx)
{
    int end;
    int end_prev;
    int start;
    int start_prev;
    SHR_FUNC_ENTER(u);

    if (pair_pfx_is_ipv4(u, ldb, pfx)) {
        SHR_RETURN_VAL_EXIT(pair_shift_v4_up(u, ldb, pfx));
    }

    start = PAIR_STATE_START(u, pfx);
    end   = PAIR_STATE_END(u, pfx);
    start_prev = pair_prev_index(u, ldb, start);
    end_prev = pair_prev_index(u, ldb, end);

    /* Don't move empty prefix .
     * This condition is hit if pfx is created for the first time
     */
    if (PAIR_STATE_VENT(u, pfx) == 0) {
        ALPM_ASSERT(0);
        PAIR_STATE_START(u, pfx) = start_prev;
        PAIR_STATE_END(u, pfx) = start_prev - 1;
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(pair_shift(u, pfx, end, start_prev, 0));

    PAIR_STATE_START(u, pfx) = start_prev;
    PAIR_STATE_END(u, pfx)   = end_prev;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create an empty prefix into the prefix state link.
 *
 * Create an empty prefix into the prefix state link, also take
 * half free entries from previous prefix.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 * \param [in] prev_pfx Prefix that is just larger than pfx
 *
 * \return Nothing.
 */
static void
pair_group_create(int u, uint8_t ldb, int pfx, int prev_pfx)
{
    int next_pfx = INVALID_PFX;
    int start    = INVALID_INDEX;
#ifdef ALPM_DEBUG
    int kt = pair_pfx_to_key_type(u, pfx);
#endif
    if (prev_pfx == INVALID_PFX) {
        ALPM_ASSERT_ERRMSG(0);
        return;
    }
    ALPM_ASSERT(PAIR_STATE_FENT(u, prev_pfx) >=
                half_entry_key_type(u, kt, PKM_128(u)) ? 1 : 2);

    /* Insert new prefix into list */
    next_pfx = PAIR_STATE_NEXT(u, prev_pfx);
    if (next_pfx != -1) {
        PAIR_STATE_PREV(u, next_pfx) = pfx;
    }
    PAIR_STATE_NEXT(u, pfx) = next_pfx;
    PAIR_STATE_PREV(u, pfx) = prev_pfx;
    PAIR_STATE_NEXT(u, prev_pfx) = pfx;

    /* Assign half fent from prev_pfx */
    if (pair_pfx_is_ipv6(u, ldb, pfx)) {

        /* To make sure fent are always even for ipv6, also make sure
         * if v6 fent cannot be split evenly ((fent>>1) % 2 != 0), give
         * remaining to new pfx.
         */
        PAIR_STATE_FENT(u, pfx) = ((PAIR_STATE_FENT(u, prev_pfx) >> 1) + 1) >> 1;
        PAIR_STATE_FENT(u, pfx) <<= 1;
        PAIR_STATE_FENT(u, prev_pfx) -= PAIR_STATE_FENT(u, pfx);

        start = PAIR_STATE_END(u, prev_pfx);
        start = pair_index_encode(u, start);
        start += (PAIR_STATE_FENT(u, prev_pfx) >> 1) + 1;
        start = pair_index_decode(u, start);
    } else {
        /* If fent cannot be split evenly, give remaining to new_pfx */
        PAIR_STATE_FENT(u, pfx) = (PAIR_STATE_FENT(u, prev_pfx) + 1) >> 1;
        PAIR_STATE_FENT(u, prev_pfx) -= PAIR_STATE_FENT(u, pfx);

        start = PAIR_STATE_END(u, prev_pfx);
        start += PAIR_STATE_FENT(u, prev_pfx) + 1;

        /*
         * If prev_pfx is last_pfx and it does not span, the fent
         * given could span. If the fent does span, start
         * should be compensated with IPv6 space, else the
         * last_pfx should be changed to new pfx.
 */
        if (RANGE1_LAST_PFX(u, ldb) == prev_pfx && !LAST_PFX_SPAN(u, ldb)) {
            if (start > RANGE1_LAST(u, ldb)) {
                start += RANGE2_FIRST(u, ldb) - (RANGE1_LAST(u, ldb) + 1);
            } else {
                RANGE1_LAST_PFX(u, ldb) = pfx;
            }
        }
    }

    /* Setup new prefix */
    PAIR_STATE_START(u, pfx) = start;
    PAIR_STATE_END(u, pfx) = pair_prev_index(u, ldb, start);
    PAIR_STATE_VENT(u, pfx) = 0;

    SANITY_CHECK_PAIR_STATE(u, ldb, prev_pfx);
    SANITY_CHECK_PAIR_STATE(u, ldb, pfx);
}


/*!
 * \brief Remove freed prefix from the prefix state link.
 *
 * Remove empty prefix from the list and give back all free entries.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix (Global prefix value).
 *
 * \return Nothing.
 */
static void
pair_group_destroy(int u, uint8_t ldb, int pfx)
{
    int prev_pfx;
    int next_pfx;

    if (PAIR_STATE_PREV(u, pfx) == -1) {
        return;
    }

    if (PAIR_STATE_VENT(u, pfx) == 0) {
        /* prev_pfx always present */
        prev_pfx = PAIR_STATE_PREV(u, pfx);
        next_pfx = PAIR_STATE_NEXT(u, pfx);

        /* Give away fent to prev_pfx */
        PAIR_STATE_FENT(u, prev_pfx) += PAIR_STATE_FENT(u, pfx);

        /* Remove from list */
        PAIR_STATE_NEXT(u, prev_pfx) = next_pfx;
        if (next_pfx != -1) {
            PAIR_STATE_PREV(u, next_pfx) = prev_pfx;
        }

        /* Reset the state */
        PAIR_STATE_FENT(u, pfx) = 0;
        PAIR_STATE_NEXT(u, pfx) = -1;
        PAIR_STATE_PREV(u, pfx) = -1;
        PAIR_STATE_START(u, pfx) = -1;
        PAIR_STATE_END(u, pfx) = -1;

        /* Reset last_pfx if possible */
        if (pair_pfx_is_ipv4(u, ldb, pfx)) {
            if (RANGE1_LAST_PFX(u, ldb) == pfx) {
                RANGE1_LAST_PFX(u, ldb) = prev_pfx;
            }
        }
    }

    return;
}


/*!
 * \brief Move part of IPv4 prefix entries in right tcam 1 entry DOWN.
 *
 * Shift part2 IPv4 prefix (last pfx & span) entries 1 entry DOWN,
 * while preserving last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] last_pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_move_v4_part2_down(int u, uint8_t ldb, int part2_pfx)
{
    int free_pfx;
    int end, end_next;
    int new_start = INVALID_PFX;

    SHR_FUNC_ENTER(u);

    if (LAST_PFX_SPAN(u, ldb)) {
        new_start = INVALID_PFX;
    } else {
        new_start = pair_next_index(u, ldb, RANGE2_FIRST(u, ldb));
    }

    free_pfx = pair_free_slot_search_for_v4(u, part2_pfx);

    if (free_pfx == INVALID_PFX) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else if (free_pfx < part2_pfx) {
        SHR_IF_ERR_EXIT(pair_free_slot_move_up(u, ldb, part2_pfx, free_pfx));

    } else if (free_pfx > part2_pfx) {
        SHR_IF_ERR_EXIT(pair_free_slot_move_down(u, ldb, part2_pfx, free_pfx));

        ALPM_ASSERT(new_start != PAIR_STATE_START(u, part2_pfx));
        new_start = PAIR_STATE_START(u, part2_pfx);
    }

    end = PAIR_STATE_END(u, part2_pfx);
    end_next = pair_next_index(u, ldb, end);

    if (end_next == RANGE2_FIRST(u, ldb)) {  /* just freed during move_down */
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (pair_half_entry(u, part2_pfx))  {
        /* Last entry is half full -> keep it last. */
        SHR_IF_ERR_EXIT(pair_shift(u, part2_pfx, end, end_next, 0));
        SHR_IF_ERR_EXIT(pair_shift(u, part2_pfx, RANGE2_FIRST(u, ldb), end, 0));
    } else {
        SHR_IF_ERR_EXIT(pair_shift(u, part2_pfx, RANGE2_FIRST(u, ldb), end_next, 0));
    }

    PAIR_STATE_END(u, part2_pfx) = end_next;
    if (new_start != INVALID_PFX) {
        PAIR_STATE_START(u, part2_pfx) = new_start;
    }

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Shift part of IPv4 prefix entries in right tcam 1 entry UP.
 *
 * Shift part2 IPv4 prefix (last pfx & span) entries 1 entry UP,
 * while preserving last half empty entry if any.
 *
 * \param [in] u Device u.
 * \param [in] last_pfx Translated prefix (Global prefix value).
 *
 * \return SHR_E_XXX.
 */
static int
pair_shift_v4_part2_up(int u, uint8_t ldb, int last_pfx)
{
    int from_ent, to_ent;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(LAST_PFX_SPAN(u, ldb) && last_pfx == RANGE1_LAST_PFX(u, ldb));

    to_ent   = RANGE2_FIRST(u, ldb);
    from_ent = PAIR_STATE_END(u, last_pfx);

    ALPM_ASSERT(from_ent > to_ent);

    /* Move half first if that half entry is not the only existing one */
    if (pair_half_entry(u, last_pfx) && (from_ent - 1 > to_ent)) {
        SHR_IF_ERR_EXIT(pair_shift(u, last_pfx, from_ent - 1, to_ent, 1));
        to_ent = from_ent - 1;
    }
    SHR_IF_ERR_EXIT(pair_shift(u, last_pfx, from_ent, to_ent, 1));
    PAIR_STATE_END(u, last_pfx) -= 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx > pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_move_down(int u, uint8_t ldb, int pfx, int free_pfx)
{
    int next_pfx = -1;
    int fent_incr = 0;
    SHR_FUNC_ENTER(u);

    fent_incr = pair_pfx_is_ipv6(u, ldb, free_pfx) ? 2 : 1;

    while (free_pfx > pfx && free_pfx >= 0) {
        next_pfx = PAIR_STATE_NEXT(u, free_pfx);

        if (next_pfx == -1) {
            alpm_vrf_type_t vt;
            int max_v4_pfx;

            vt = pair_pfx_to_vrf_type(u, pfx);
            max_v4_pfx = pair_max_pfx(u, KEY_IPV4, vt);

            /* Update RANGE1_FIRST, RANGE1_LAST, RANGE1_LAST_PFX accordingly */

            RANGE1_FIRST(u, ldb)--; /* boundary up */
            if (index_in_right_tcam(u, RANGE1_FIRST(u, ldb))) {
                LAST_PFX_SPAN(u, ldb) = 0;
                RANGE1_LAST_PFX(u, ldb) = max_v4_pfx;

                RANGE1_FIRST(u, ldb) -= TCAM_DEPTH(u);
                RANGE1_LAST(u, ldb) = RANGE1_FIRST(u, ldb);
                RANGE2_FIRST(u, ldb) = RANGE1_FIRST(u, ldb) + TCAM_DEPTH(u);
            } else {
                RANGE2_FIRST(u, ldb) = RANGE1_FIRST(u, ldb) + TCAM_DEPTH(u);
            }
            /* Reset position of max_v4_pfx */
            PAIR_STATE_START(u, max_v4_pfx) = RANGE1_FIRST(u, ldb);
            PAIR_STATE_END(u, max_v4_pfx) = RANGE1_FIRST(u, ldb) - 1;

            next_pfx = PAIR_STATE_NEXT(u, max_v4_pfx);

            if (next_pfx != INVALID_PFX) {
                /*
                 * Take care of pfx == max_v4_pfx first to make sure for this
                 * case, the pfx can always have free entries assigned.
                 */
                if (pfx == max_v4_pfx) {
                    PAIR_STATE_FENT(u, max_v4_pfx) += 1;
                    if (LAST_PFX_SPAN(u, ldb)) {
                        SHR_IF_ERR_EXIT(
                            pair_shift_v4_part2_up(u, ldb, RANGE1_LAST_PFX(u, ldb)));
                    }
                    PAIR_STATE_FENT(u, RANGE1_LAST_PFX(u, ldb)) += 1;
                    if (RANGE1_LAST_PFX(u, ldb) >= pfx) {
                        next_pfx = RANGE1_LAST_PFX(u, ldb); /* a little thrust */
                    }

                } else if (LAST_PFX_SPAN(u, ldb) &&
                           RANGE1_LAST_PFX(u, ldb) == next_pfx) {
                    SHR_IF_ERR_EXIT(pair_shift_v4_part2_up(u, ldb, next_pfx));

                    PAIR_STATE_FENT(u, next_pfx) += 1;

                    SHR_IF_ERR_EXIT(pair_shift_v4_up(u, ldb, next_pfx));
                    PAIR_STATE_FENT(u, next_pfx) += 1;
                } else {

                    SHR_IF_ERR_EXIT(pair_shift_v4_up(u, ldb, next_pfx));
                    PAIR_STATE_FENT(u, next_pfx) += 1;
                    if (index_in_right_tcam(u, PAIR_STATE_START(u, next_pfx))) {
                        SHR_IF_ERR_EXIT(pair_shift_v4_up(u, ldb, next_pfx));
                        PAIR_STATE_FENT(u, next_pfx) += 1;

                        ALPM_ASSERT(RANGE1_LAST_PFX(u, ldb) == next_pfx);
                    } else {
                        if (LAST_PFX_SPAN(u, ldb)) {
                            SHR_IF_ERR_EXIT(
                                pair_shift_v4_part2_up(u, ldb, RANGE1_LAST_PFX(u, ldb)));
                        }
                        PAIR_STATE_FENT(u, RANGE1_LAST_PFX(u, ldb)) += 1;
                        if (RANGE1_LAST_PFX(u, ldb) >= pfx) {
                            next_pfx = RANGE1_LAST_PFX(u, ldb); /* a little thrust */
                        }
                    }
                }
                SANITY_CHECK_PAIR_STATE(u, ldb, next_pfx);
            } else {
                PAIR_STATE_FENT(u, max_v4_pfx) += 2;
            }

            PAIR_STATE_FENT(u, free_pfx) -= 2;

            SANITY_CHECK_PAIR_STATE(u, ldb, max_v4_pfx);
            SANITY_CHECK_PAIR_STATE(u, ldb, free_pfx);

            fent_incr = 1;
        } else {
            SHR_IF_ERR_EXIT(pair_shift_up(u, ldb, next_pfx));

            PAIR_STATE_FENT(u, free_pfx) -= fent_incr;
            PAIR_STATE_FENT(u, next_pfx) += fent_incr;
            SANITY_CHECK_PAIR_STATE(u, ldb, next_pfx);
            SANITY_CHECK_PAIR_STATE(u, ldb, free_pfx);
        }

        free_pfx = next_pfx;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Move free slot from free_pfx to pfx (free_pfx < pfx).
 *
 * Move free slot from free_pfx to pfx.
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] free_pfx Translated prefix with free entries
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_move_up(int u, uint8_t ldb, int pfx, int free_pfx)
{
    int prev_pfx = -1;
    int fent_incr = 0;
    alpm_vrf_type_t vt;
    int max_v4_pfx;

    SHR_FUNC_ENTER(u);

    fent_incr = pair_pfx_is_ipv4(u, ldb, free_pfx) ? 1 : 2;

    while (free_pfx < pfx) {
        prev_pfx = PAIR_STATE_PREV(u, free_pfx);

        if (prev_pfx == -1) {
            int min_v6_pfx;
            int last_next_pfx = INVALID_PFX;

            vt = pair_pfx_to_vrf_type(u, pfx);
            min_v6_pfx = pair_min_pfx(u, KEY_IPV6_64, vt);
            max_v4_pfx = pair_max_pfx(u, KEY_IPV4, vt);
            prev_pfx = min_v6_pfx;

            ALPM_ASSERT(free_pfx == max_v4_pfx);

            ALPM_ASSERT(PAIR_STATE_FENT(u, free_pfx) >= 1);

            do {
                int start, end;
                start = PAIR_STATE_START(u, max_v4_pfx);
                end = PAIR_STATE_END(u, max_v4_pfx);
                PAIR_STATE_START(u, max_v4_pfx) = pair_next_index(u, ldb, start);
                PAIR_STATE_END(u, max_v4_pfx)   = pair_next_index(u, ldb, end);
                PAIR_STATE_FENT(u, max_v4_pfx) -= 1;
            } while (0);

            LOG_D((BSL_META("MOVE UP: Free pfx[%d] down, "), free_pfx));

            if (LAST_PFX_SPAN(u, ldb)) {
                int rv;
                rv = pair_move_v4_part2_down(u, ldb, RANGE1_LAST_PFX(u, ldb));
                ALPM_ASSERT(SHR_SUCCESS(rv));
                SHR_IF_ERR_EXIT(rv);

                PAIR_STATE_FENT(u, RANGE1_LAST_PFX(u, ldb)) -= 1;
                LOG_D((BSL_META("Last pfx[%d] down, "), RANGE1_LAST_PFX(u, ldb)));

            } else if (PAIR_STATE_START(u, max_v4_pfx) == RANGE2_FIRST(u, ldb)) {
                /*
                 * Corner case where just shift max_v4_pfx 1 entry DOWN,
                 * and even the max_v4_pfx resides in range2.
                 */
                int rv;
                rv = pair_move_v4_part2_down(u, ldb, max_v4_pfx);
                ALPM_ASSERT(SHR_SUCCESS(rv));
                SHR_IF_ERR_EXIT(rv);

                PAIR_STATE_FENT(u, max_v4_pfx) -= 1;
                LOG_D((BSL_META("Max v4 pfx[%d] down again, "), max_v4_pfx));

            }  else {
                last_next_pfx = PAIR_STATE_NEXT(u, RANGE1_LAST_PFX(u, ldb));

                if (last_next_pfx != INVALID_PFX &&
                    PAIR_STATE_START(u, last_next_pfx) == RANGE2_FIRST(u, ldb)) {
                    SHR_IF_ERR_EXIT(
                        pair_move_v4_part2_down(u, ldb, last_next_pfx));

                    PAIR_STATE_FENT(u, last_next_pfx) -= 1;

                } else {
                    PAIR_STATE_FENT(u, RANGE1_LAST_PFX(u, ldb)) -= 1;
                }
                LOG_D((BSL_META("Last next pfx[%d] down. last pfx [%d], "),
                       last_next_pfx, RANGE1_LAST_PFX(u, ldb)));
            }

            /* Update RANGE1_FIRST, RANGE1_LAST, RANGE1_LAST_PFX accordingly */

            RANGE1_FIRST(u, ldb)++; /* boundary down */
            if (index_in_right_tcam(u, RANGE1_FIRST(u, ldb))) {
                int last_pfx, next_pfx;

                /* Adjust boundary */
                RANGE1_FIRST(u, ldb) += TCAM_DEPTH(u);
                RANGE1_LAST(u, ldb)   = RANGE1_FIRST(u, ldb) + TCAM_DEPTH(u) - 1;
                RANGE2_FIRST(u, ldb)  = RANGE1_FIRST(u, ldb) + TCAM_DEPTH(u);

                /* Search for new range1_last_pfx */
                last_pfx = free_pfx;
                next_pfx = PAIR_STATE_NEXT(u, last_pfx);

                while (next_pfx != -1 &&
                       PAIR_STATE_START(u, next_pfx) <= RANGE1_LAST(u, ldb)) {

                    last_pfx = next_pfx;
                    next_pfx = PAIR_STATE_NEXT(u, last_pfx);
                }
                RANGE1_LAST_PFX(u, ldb) = last_pfx;

                /* Then new last_pfx_span */
                if (PAIR_STATE_END(u, last_pfx) >= RANGE2_FIRST(u, ldb)) {
                    LAST_PFX_SPAN(u, ldb) = 1;
                } else {
                    LAST_PFX_SPAN(u, ldb) = 0;
                }

            } else {
                /* Adjust boundary */
                RANGE2_FIRST(u, ldb) = RANGE1_FIRST(u, ldb) + TCAM_DEPTH(u);
            }

            LOG_D((BSL_META("=>last pfx %d span %d \n"),
                   RANGE1_LAST_PFX(u, ldb), LAST_PFX_SPAN(u, ldb)));


            PAIR_STATE_FENT(u, prev_pfx) += 2;

            /* Reset position of max_v4_pfx */
            PAIR_STATE_START(u, max_v4_pfx) = RANGE1_FIRST(u, ldb);
            PAIR_STATE_END(u, max_v4_pfx) = RANGE1_FIRST(u, ldb) - 1;

            if (last_next_pfx != INVALID_PFX) {
                SANITY_CHECK_PAIR_STATE(u, ldb, last_next_pfx);
            }
            SANITY_CHECK_PAIR_STATE(u, ldb, RANGE1_LAST_PFX(u, ldb));
            SANITY_CHECK_PAIR_STATE(u, ldb, max_v4_pfx);

            fent_incr = 2;
        } else {

            SHR_IF_ERR_EXIT(pair_shift_down(u, ldb, free_pfx));
            PAIR_STATE_FENT(u, free_pfx) -= fent_incr;
            PAIR_STATE_FENT(u, prev_pfx) += fent_incr;
            SANITY_CHECK_PAIR_STATE(u, ldb, prev_pfx);
            SANITY_CHECK_PAIR_STATE(u, ldb, free_pfx);
        }

        free_pfx = prev_pfx;
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allocate a slot for the new IPv4 prefix.
 *
 * Allocate a slot for the new IPv4 prefix rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [out] free_slot Translated index
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_alloc_v4(int u, uint8_t ldb, int pfx, int *free_slot)
{
    int free_pfx, pfx2;
    int prev_pfx = INVALID_PFX;
    int end;
    SHR_FUNC_ENTER(u);

    /* Check if half of the entry is free. if yes add to it */
    if (pair_half_entry(u, pfx)) {
        end = PAIR_STATE_END(u, pfx);
        *free_slot = HALF_INDEX_R(end);
        pair_half_entry_update(u, pfx);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (PAIR_STATE_FENT(u, pfx) < 1) {
        pfx2 = pfx;

        if (PAIR_STATE_VENT(u, pfx) == 0) {
            alpm_key_type_t kt;
            alpm_vrf_type_t vt;

            /* Find prev pfx */
            kt = pair_pfx_to_key_type(u, pfx);
            vt = pair_pfx_to_vrf_type(u, pfx);

            prev_pfx = pair_max_pfx(u, kt, vt);
            while (PAIR_STATE_NEXT(u, prev_pfx) > pfx) {
                prev_pfx = PAIR_STATE_NEXT(u, prev_pfx);
            }
            pfx2 = prev_pfx;
        }

        free_pfx = pair_free_slot_search_for_v4(u, pfx2);

        if (free_pfx == INVALID_PFX) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        } else if (free_pfx < pfx2) {
            SHR_IF_ERR_EXIT(pair_free_slot_move_up(u, ldb, pfx2, free_pfx));
        } else if (free_pfx > pfx2) {
            SHR_IF_ERR_EXIT(pair_free_slot_move_down(u, ldb, pfx2, free_pfx));
        }
    }

    if (PAIR_STATE_VENT(u, pfx) == 0 && prev_pfx != INVALID_PFX) {
        pair_group_create(u, ldb, pfx, prev_pfx);
    }

    end = PAIR_STATE_END(u, pfx);
    end = pair_next_index(u, ldb, end);
    PAIR_STATE_END(u, pfx) = end;
    PAIR_STATE_VENT(u, pfx) += 1;
    PAIR_STATE_FENT(u, pfx) -= 1;

    ALPM_ASSERT(end < SIZE_PAIR(u));

    pair_half_entry_update(u, pfx);

    pair_range1_last_pfx_update(u, ldb, pfx);

    SANITY_CHECK_PAIR_STATE(u, ldb, pfx);

    *free_slot = HALF_INDEX_L(end);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allocate a slot for the new prefix.
 *
 * Allocate a slot for the new prefix rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [out] free_slot Translated index
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_alloc(int u, uint8_t ldb, int pfx, int *free_slot)
{
    int free_pfx, pfx2;
    int prev_pfx = INVALID_PFX;
    int new_end;
    SHR_FUNC_ENTER(u);

    if (pair_pfx_is_ipv4(u, ldb, pfx)) {
        SHR_RETURN_VAL_EXIT(pair_free_slot_alloc_v4(u, ldb, pfx, free_slot));
    }

    if (PAIR_STATE_FENT(u, pfx) < 2) {
        pfx2 = pfx;

        if (PAIR_STATE_VENT(u, pfx) == 0) {
            alpm_key_type_t kt;
            alpm_vrf_type_t vt;

            /* Find prev pfx */
            kt = pair_pfx_to_key_type(u, pfx);
            vt = pair_pfx_to_vrf_type(u, pfx);

            prev_pfx = pair_max_pfx(u, kt, vt);
            while (PAIR_STATE_NEXT(u, prev_pfx) > pfx) {
                prev_pfx = PAIR_STATE_NEXT(u, prev_pfx);
            }
            pfx2 = prev_pfx;
        }

        free_pfx = pair_free_slot_search_for_v6(u, pfx2);

        if (free_pfx == INVALID_PFX) {
            SHR_RETURN_VAL_EXIT(SHR_E_FULL);
        } else if (free_pfx > pfx2) {
            SHR_IF_ERR_EXIT(
                pair_free_slot_move_down(u, ldb, pfx2, free_pfx));
        } else if (free_pfx < pfx2) {
            SHR_IF_ERR_EXIT(
                pair_free_slot_move_up(u, ldb, pfx2, free_pfx));
        }

    }

    if (PAIR_STATE_VENT(u, pfx) == 0 && prev_pfx != INVALID_PFX) {
        pair_group_create(u, ldb, pfx, prev_pfx);
    }

    ALPM_ASSERT((PAIR_STATE_FENT(u, pfx) >= 2) &&
                (PAIR_STATE_FENT(u, pfx) & 1) == 0);

    new_end = pair_next_index(u, ldb, PAIR_STATE_END(u, pfx));
    PAIR_STATE_END(u, pfx) = new_end;
    PAIR_STATE_VENT(u, pfx) += 1;
    PAIR_STATE_FENT(u, pfx) -= 2;

    SANITY_CHECK_PAIR_STATE(u, ldb, pfx);

    *free_slot = FULL_INDEX(new_end);
exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Free a slot for the IPv4 prefix.
 *
 * Free a slot for the IPv4 prefix, rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] del_slot Translated index
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_free_v4(int u, uint8_t ldb, int pfx, int del_slot)
{
    int           from;
    int           from_prev;
    int           to;
    L1_ENTRY_t    de1, de2, se1, se2;
    uint32_t      *from_de, *from_se, *to_de, *to_se;
    int           from_v1, to_v1;
    int           src_slot;
    int           full;
    bcmdrd_sid_t  sid;
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    SHR_FUNC_ENTER(u);

    from = PAIR_STATE_END(u, pfx);
    to = HW_INDEX(del_slot);
    pair_pfx_decode(u, pfx, &kt, &vt, NULL);

    sid = pair_sid(u, kt);
    from_prev = pair_prev_index(u, ldb, from);

    to_de = de1;
    to_se = se1;
    if (from == to) {
        from_de = to_de;
        from_se = to_se;
    } else {
        from_de = de2;
        from_se = se2;
    }

    SHR_IF_ERR_EXIT(read_l1_raw(u, sid, to, to_de, to_se));
    if (from != to) {
        SHR_IF_ERR_EXIT(read_l1_raw(u, sid, from, from_de, from_se));
    }

    from_v1 = !pair_half_entry(u, pfx);
    to_v1 = IS_HALF_R_INDEX(del_slot);
    del_slot = (to << 1) + to_v1;
    src_slot = (from << 1) + from_v1;

    ALPM_ASSERT(src_slot >= del_slot);

    if (src_slot == del_slot) {
        /* Effectively, just clear */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_half_entry_move(u, from_de, to_de, from_v1, to_v1));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_half_entry_move(u, from_se, to_se, from_v1, to_v1));
    } else {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_half_entry_move(u, from_de, to_de, from_v1, to_v1));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_half_entry_move(u, from_se, to_se, from_v1, to_v1));
    }

    if (from_v1) {
        PAIR_HALF_COUNT(u, kt, vt)++;
        ALPM_ASSERT(PAIR_HALF_COUNT(u, kt, vt) >= 0);
    } else {
        /* Remaining half entry got deleted. Decrement half entry count.*/
        PAIR_HALF_COUNT(u, kt, vt)--;
        ALPM_ASSERT(PAIR_HALF_COUNT(u, kt, vt) >= 0);
        PAIR_STATE_VENT(u, pfx) -= 1;
        PAIR_STATE_FENT(u, pfx) += 1;

        PAIR_STATE_END(u, pfx) = from_prev;

        pair_range1_last_pfx_update(u, ldb, pfx);
    }

    SHR_IF_ERR_EXIT(write_l1_raw(u, sid, to, to_de, to_se, pfx));

    if (to != from) {
        SHR_IF_ERR_EXIT(write_l1_raw(u, sid, from, from_de, from_se, pfx));
    }

    /* Update half free entry */
    pair_half_entry_update(u, pfx);

    full = pair_pfx_to_key_type(u, pfx);
    full = !half_entry_key_type(u, full, PKM_128(u));

    SHR_IF_ERR_EXIT(pivot_move(u, src_slot, del_slot, full, 1));

    if (PAIR_STATE_VENT(u, pfx) == 0) {
        pair_group_destroy(u, ldb, pfx);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Free a slot for the prefix.
 *
 * Free a slot for the prefix, rippling the entries if required
 *
 * \param [in] u Device u.
 * \param [in] pfx Translated prefix requesting free entries.
 * \param [in] del_slot Translated index
 *
 * \return SHR_E_XXX.
 */
static int
pair_free_slot_free(int u, uint8_t ldb, int pfx, int del_slot)
{
    int           from_ent;
    int           to_ent;
    int           full;
    int           key_type;
    L1_ENTRY_t    from_de, from_se;
    bcmdrd_sid_t  sid;
    SHR_FUNC_ENTER(u);

    if (pair_pfx_is_ipv4(u, ldb, pfx)) {
        SHR_RETURN_VAL_EXIT(pair_free_slot_free_v4(u, ldb, pfx, del_slot));
    }

    from_ent = PAIR_STATE_END(u, pfx);
    to_ent   = HW_INDEX(del_slot);
    PAIR_STATE_VENT(u, pfx) -= 1;
    PAIR_STATE_FENT(u, pfx) += 2;

    PAIR_STATE_END(u, pfx) = pair_prev_index(u, ldb, from_ent);

    key_type = pair_pfx_to_key_type(u, pfx);
    sid = pair_sid(u, key_type);
    if (to_ent != from_ent) {
        SHR_IF_ERR_EXIT(read_l1_raw(u, sid, from_ent, from_de, from_se));
        SHR_IF_ERR_EXIT(write_l1_raw(u, sid, to_ent, from_de, from_se, pfx));
    }

    SHR_IF_ERR_EXIT(clear_l1_raw(u, sid, from_ent));

    full = !half_entry_key_type(u, key_type, PKM_128(u));

    SHR_IF_ERR_EXIT(pivot_move(u, from_ent << 1, to_ent << 1, full, 1));

    if (PAIR_STATE_VENT(u, pfx) == 0) {
        pair_group_destroy(u, ldb, pfx);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the smallest prefix.
 *
 * Get the smallest IPv6_64 prefix, if not exist,
 * get the smallest IPv4 prefix.
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type.
 *
 * \return SHR_E_XXX.
 */
static int
pair_smallest_prefix(int u, uint8_t ldb, alpm_vrf_type_t vt)
{
    int curr_pfx;
    int move_pfx = -1;

    /* Try to get IPv6_64 first */
    curr_pfx = pair_max_pfx(u, KEY_IPV6_128, vt);
    while (curr_pfx != -1) {
        if (pair_pfx_is_ipv6_64(u, ldb, curr_pfx)) {
            move_pfx = curr_pfx;
        }
        curr_pfx = PAIR_STATE_NEXT(u, curr_pfx);
    }

    /* Try to get IPv4 second */
    if (move_pfx == -1) {
        curr_pfx = pair_max_pfx(u, KEY_IPV4, vt);
        while (curr_pfx != -1) {
            if (pair_pfx_is_ipv4(u, ldb, curr_pfx)) {
                move_pfx = curr_pfx;
            }
            curr_pfx = PAIR_STATE_NEXT(u, curr_pfx);
        }
    }

    return move_pfx;
}


static int
pair_stats_init(int u, bool recover)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;

    SHR_FUNC_ENTER(u);

    for (vt = VRF_PRIVATE; vt < VRF_TYPES; vt++) {
        for (kt = KEY_IPV4; kt < KEY_TYPES; kt++) {
            PAIR_USED_COUNT(u, kt, vt) = 0;
            PAIR_HALF_COUNT(u, kt, vt) = 0;
        }
    }

    if (!DUAL_MODE(u)) {
        PAIR_MAX_COUNT(u, KEY_IPV4, VRF_PRIVATE) = SIZE_PAIR(u) << 1;
        PAIR_MAX_COUNT(u, KEY_IPV6_64, VRF_PRIVATE) = SIZE_PAIR(u) >> 1;
        PAIR_MAX_COUNT(u, KEY_IPV6_128, VRF_PRIVATE) = SIZE_PAIR(u) >> 1;
    } else {
        PAIR_MAX_COUNT(u, KEY_IPV4, VRF_PRIVATE) = SIZE_PAIR(u);
        PAIR_MAX_COUNT(u, KEY_IPV6_64, VRF_PRIVATE) = SIZE_PAIR(u) >> 2;
        PAIR_MAX_COUNT(u, KEY_IPV6_128, VRF_PRIVATE) = SIZE_PAIR(u) >> 2;

        PAIR_MAX_COUNT(u, KEY_IPV4, VRF_GLOBAL)  = SIZE_PAIR(u);
        PAIR_MAX_COUNT(u, KEY_IPV6_64, VRF_GLOBAL)  = SIZE_PAIR(u) >> 2;
        PAIR_MAX_COUNT(u, KEY_IPV6_128, VRF_GLOBAL)  = SIZE_PAIR(u) >> 2;
    }

    SHR_FUNC_EXIT();

}


static int
pair_state_init(int u, bool recover)
{
    int sz;
    int i;
    int max_pfx_v4, max_pfx;
    SHR_FUNC_ENTER(u);

    max_pfx = pair_num_pfxs(u, KEY_IPV6_128, VRF_PRIVATE);
    sz = sizeof(pair_state_t) * (max_pfx);

    ALPM_ALLOC(PAIR_STATE(u), sz, "State");
    sal_memset(PAIR_STATE(u), 0, sz);

    for(i = 0; i < max_pfx; i++) {
        PAIR_STATE_START(u, i) = -1;
        PAIR_STATE_END(u, i)   = -1;
        PAIR_STATE_PREV(u, i)  = -1;
        PAIR_STATE_NEXT(u, i)  = -1;
        PAIR_STATE_VENT(u, i)  = 0;
        PAIR_STATE_FENT(u, i)  = 0;
    }


    if (DUAL_MODE(u)) {
        int fent_db0, fent_db1;

        fent_db0 = SIZE_PAIR_2(u, L1_DB0);
        fent_db1 = SIZE_PAIR_2(u, L1_DB1);
        PAIR_MAX_PFX(u, L1_DB0) = pair_max_pfx(u, KEY_IPV6_128, VRF_PRIVATE);
        PAIR_MAX_V4_PFX(u, L1_DB0) = pair_max_pfx(u, KEY_IPV4, VRF_PRIVATE);
        PAIR_MAX_PFX(u, L1_DB1) = pair_max_pfx(u, KEY_IPV6_128, VRF_GLOBAL);
        PAIR_MAX_V4_PFX(u, L1_DB1) = pair_max_pfx(u, KEY_IPV4, VRF_GLOBAL);

        max_pfx = PAIR_MAX_PFX(u, L1_DB0);
        PAIR_STATE_FENT(u, max_pfx) = fent_db0;
        PAIR_STATE_START(u, max_pfx) = 0;
        PAIR_STATE_END(u, max_pfx) = -1;

        max_pfx_v4 = PAIR_MAX_V4_PFX(u, L1_DB0);
        PAIR_STATE_FENT(u, max_pfx_v4) = 0;
        PAIR_STATE_START(u, max_pfx_v4) = fent_db0;
        PAIR_STATE_END(u, max_pfx_v4) = fent_db0 - 1;
        RANGE1_FIRST(u, L1_DB0)    = PAIR_STATE_START(u, max_pfx_v4);
        RANGE2_FIRST(u, L1_DB0)    = RANGE1_FIRST(u, L1_DB0) + TCAM_DEPTH(u);
        RANGE1_LAST(u, L1_DB0)     = RANGE2_FIRST(u, L1_DB0) - 1;
        RANGE1_LAST_PFX(u, L1_DB0) = max_pfx_v4;
        LAST_PFX_SPAN(u, L1_DB0)   = 0;

        max_pfx = PAIR_MAX_PFX(u, L1_DB1);
        PAIR_STATE_FENT(u, max_pfx) = fent_db1;
        PAIR_STATE_START(u, max_pfx) = fent_db0;
        PAIR_STATE_END(u, max_pfx) = fent_db0 - 1;

        max_pfx_v4 = PAIR_MAX_V4_PFX(u, L1_DB1);
        PAIR_STATE_FENT(u, max_pfx_v4) = 0;
        PAIR_STATE_START(u, max_pfx_v4) = fent_db1 + fent_db0;
        PAIR_STATE_END(u, max_pfx_v4) = fent_db1 + fent_db0 - 1;
        RANGE1_FIRST(u, L1_DB1)    = PAIR_STATE_START(u, max_pfx_v4);
        RANGE2_FIRST(u, L1_DB1)    = RANGE1_FIRST(u, L1_DB1) + TCAM_DEPTH(u);
        RANGE1_LAST(u, L1_DB1)     = RANGE2_FIRST(u, L1_DB1) - 1;
        RANGE1_LAST_PFX(u, L1_DB1) = max_pfx_v4;
        LAST_PFX_SPAN(u, L1_DB1)   = 0;
/*
        if (recover) {
            PAIR_STATE_FENT(u, PAIR_MAX_PFX(u, L1_DB0)) = 0;
            PAIR_STATE_FENT(u, PAIR_MAX_V4_PFX(u, L1_DB0)) = 0;
            PAIR_STATE_FENT(u, PAIR_MAX_PFX(u, L1_DB1)) = 0;
            PAIR_STATE_FENT(u, PAIR_MAX_V4_PFX(u, L1_DB1)) = 0;
        }
*/
    } else {
        int fent = SIZE_PAIR(u);
        PAIR_MAX_PFX(u, L1_DB0) = pair_max_pfx(u, KEY_IPV6_128, VRF_PRIVATE);
        PAIR_MAX_V4_PFX(u, L1_DB0) = pair_max_pfx(u, KEY_IPV4, VRF_PRIVATE);
        PAIR_MAX_PFX(u, L1_DB1) = -1;
        PAIR_MAX_V4_PFX(u, L1_DB1) = -1;

        max_pfx = PAIR_MAX_PFX(u, L1_DB0);
        PAIR_STATE_FENT(u, max_pfx) = fent;
        PAIR_STATE_START(u, max_pfx) = 0;
        PAIR_STATE_END(u, max_pfx) = -1;

        max_pfx_v4 = PAIR_MAX_V4_PFX(u, L1_DB0);
        PAIR_STATE_FENT(u, max_pfx_v4)  = 0;
        PAIR_STATE_START(u, max_pfx_v4) = fent;
        PAIR_STATE_END(u, max_pfx_v4)   = fent - 1;

        RANGE1_FIRST(u, L1_DB0)    = PAIR_STATE_START(u, max_pfx_v4);
        RANGE2_FIRST(u, L1_DB0)    = RANGE1_FIRST(u, L1_DB0) + TCAM_DEPTH(u);
        RANGE1_LAST(u, L1_DB0)     = RANGE2_FIRST(u, L1_DB0) - 1;
        RANGE1_LAST_PFX(u, L1_DB0) = max_pfx_v4;
        LAST_PFX_SPAN(u, L1_DB0)   = 0;

/*
        if (recover) {
            PAIR_STATE_FENT(u, PAIR_MAX_PFX(u, L1_DB0)) = 0;
            PAIR_STATE_FENT(u, PAIR_MAX_V4_PFX(u, L1_DB0)) = 0;
        }
*/
    }

exit:
    SHR_FUNC_EXIT();

}

static void
pair_state_cleanup(int u)
{
    if (L1_INFO(u)) {
        SHR_FREE(PAIR_STATE(u));
    }

}


static void
pair_stats_cleanup(int u)
{
    return;
}


/*!
 * \brief Insert an entry into pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be inserted
 *
 * \return SHR_E_XXX
 */
static int
pair_insert(int u, alpm_arg_t *arg)
{
    bool new_add;
    SHR_FUNC_ENTER(u);

    if (arg->index[LEVEL1] == INVALID_INDEX) {
        int rv;
        arg->key.pfx = pair_pfx_encode(u, arg->key.t.ipv, arg->key.vt,
                                       arg->key.t.len);

        rv = pair_free_slot_alloc(u, arg->l1_db, arg->key.pfx, &arg->index[LEVEL1]);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_FULL) {
            /* Shut error messages prints, as E_FULL is really not an error */
                SHR_RETURN_VAL_EXIT(SHR_E_FULL);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        new_add = true;
    } else {
        new_add = false;
    }

    if (arg->skip_write == WRITE_NORMAL || arg->skip_write == WRITE_NEW) {
        bcmdrd_sid_t sid = pair_sid(u, arg->key.kt);
        SHR_IF_ERR_EXIT(
            encode_write_l1_entry(u, sid, arg,
                                  new_add || arg->skip_write == WRITE_NEW));
    }

    if (new_add) {
        alpm_key_type_t kt;
        alpm_vrf_type_t vt;

        if (!arg->in_move) {
            alpm_pivot_t *pivot;
            int index2;
            if (arg->a1.direct_route) {
                SHR_IF_ERR_EXIT(l1_pvt_alloc(u, arg, &pivot));
                SHR_IF_ERR_EXIT(l1_pvt_insert(u, arg, pivot));
                arg->pivot[LEVEL1] = pivot;
            } else {
                pivot = arg->pivot[LEVEL1];
            }
            ALPM_ASSERT(pivot);
            index2 = arr_idx_from_pvt_idx(u, arg->index[LEVEL1]);
            L1_PVT_INFO(u, index2) = pivot;
        }

        kt = arg->key.kt;
        vt = arg->key.vt;
        ALPM_ASSERT_ERRMSG(kt > KEY_TYPE_INVALID && kt < KEY_TYPES);
        ALPM_ASSERT_ERRMSG(vt > VRF_TYPE_INVALID && vt < VRF_TYPES);
        PAIR_USED_COUNT(u, kt, vt)++;
        if (half_entry_key_type(u, kt, PKM_128(u))) {
            ALPM_ASSERT(IS_HALF_INDEX(arg->index[LEVEL1]));
            if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
                PAIR_HALF_COUNT(u, kt, vt)--;
                ALPM_ASSERT(PAIR_HALF_COUNT(u, kt, vt) >= 0);
            } else {
                PAIR_HALF_COUNT(u, kt, vt)++;
                ALPM_ASSERT(PAIR_HALF_COUNT(u, kt, vt) >= 0);
            }
        }
        /* VRF_PIVOT_REF_INC(u, vrf_id, vrf, kt); */
    } else if (arg->a1.direct_route) {
        ALPM_ASSERT(arg->pivot[LEVEL1]);
        sal_memcpy(arg->pivot[LEVEL1]->ad, &arg->ad, sizeof(assoc_data_t));
    }

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Delete an entry from pair TCAM
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg ready to be deleted
 *
 * \return SHR_E_XXX
 */
static int
pair_delete(int u, alpm_arg_t *arg)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    alpm_pivot_t    *pivot = NULL;
    uint8_t ldb;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(arg->index[LEVEL1] != INVALID_INDEX);

    arg->key.pfx = pair_pfx_encode(u, arg->key.t.ipv, arg->key.vt, arg->key.t.len);
    if (DUAL_MODE(u) && arg->key.vt != VRF_PRIVATE) {
        ldb = L1_DB1;
    } else{
        ldb = L1_DB0;
    }
    SHR_IF_ERR_EXIT(
        pair_free_slot_free(u, ldb, arg->key.pfx, arg->index[LEVEL1]));

    if (!arg->in_move && arg->a1.direct_route) {
        SHR_IF_ERR_EXIT(l1_pvt_delete(u, arg, &pivot));
        SHR_IF_ERR_EXIT(l1_pvt_free(u, arg->db, pivot));
    }

    kt = arg->key.kt;
    vt = arg->key.vt;
    PAIR_USED_COUNT(u, kt, vt)--;

    /* VRF_PIVOT_REF_DEC(u, w_vrf, kt); */
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get args from pair TCAM for a given index
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM arg with index set
 *
 * \return SHR_E_XXX
 */
static inline int
pair_get(int u, alpm_arg_t *arg)
{
    bcmdrd_sid_t sid = pair_sid(u, arg->key.kt);
    return decode_read_l1_entry(u, sid, arg, NULL);
}

/*!
 * \brief Deterine if we can move unpair32 1 entry UP
 *
 * \param [in] u Device u.
 * \param [in] in_pfx In unpair32 prefix
 * \param [out] in_pfx Out unpair32 prefix, if it is ok to move
 *
 * \return True if ok to move, else false.
 */
static int
can_unpair32_move_up(int u, int in_pfx, int *out_pfx)
{
    int move = 0;
    int v6;
    alpm_vrf_type_t vt;
    int move_pfx;

    if ((PKM_32(u) == PKM_64(u)) || !SIZE_UNPAIR(u, PKM_32(u))) {
        return 0;
    }

    unpair_pfx_decode(u, in_pfx, &v6, &vt, NULL, PKM_32(u));

    move_pfx = unpair_largest_prefix(u, vt, PKM_32(u));
    if (move_pfx == INVALID_PFX) {
        return 0;
    }

    if (v6) {
        /* If unpair pfx is IPv4 then we should move IPv4, as it's more impact */
        if (unpair_pfx_is_ipv4(u, move_pfx, PKM_32(u))) {
            move = 1;
        } else {
            /*
             * Indicating no IPv4.
             * Move the entry of top IPv6 pfx, if that pfx is
             * bigger than insert pfx.
             */
            move = move_pfx >= in_pfx;
        }
    } else {
        if (unpair_pfx_is_ipv4(u, move_pfx, PKM_32(u))) {
            /*
             * If unpair pfx is v4,
             * Move the entry of that unpair pfx, if that unpair pfx is
             * bigger than insert pfx.
             */
            move = move_pfx >= in_pfx;
        } else {
            move = 1;
        }
    }

    if (out_pfx) {
        *out_pfx = move_pfx;
    }

    return move;
}

/*!
 * \brief Deterine if we can move unpair64 1 entry DOWN
 *
 * \param [in] u Device u.
 * \param [in] in_pfx In unpair64 prefix
 * \param [out] in_pfx Out unpair64 prefix, if it is ok to move
 *
 * \return True if ok to move, else false.
 */
static int
can_unpair64_move_down(int u, int in_pfx, int *out_pfx)
{
    int move = 0;
    int v6;
    alpm_vrf_type_t vt;
    int move_pfx;

    if ((PKM_32(u) == PKM_64(u)) || !SIZE_UNPAIR(u, PKM_32(u))) {
        return 0;
    }

    unpair_pfx_decode(u, in_pfx, &v6, &vt, NULL, PKM_64(u));

    move_pfx = unpair64_smallest_prefix(u, vt);

    if (move_pfx == INVALID_PFX) {
        return 0;
    }

    if (v6) {
        if (unpair_pfx_is_ipv4(u, move_pfx, PKM_64(u))) {
            /*
             * If pair pfx is IPv4, and insert pfx is IPv6,
             * then we should move IPv4 to unpair, as we assume
             * that when move free entry down, the insert pfx
             * are too long to fit in the more compact packing block.
             */
            move = 1;
        } else {
            /*
             * If pair pfx is IPv6,
             * Move the entry of bottom IPv6 pfx, if that pfx is
             * smaller than insert pfx.
             */
            move = move_pfx <= in_pfx;
        }
    } else {
        if (unpair_pfx_is_ipv4(u, move_pfx, PKM_64(u))) {

            /*
             * Indicating no IPv6. Both are IPv4.
             * Move the entry of bottom IPv4 pfx, if that pfx is
             * smaller than insert IPv4 pfx.
             */
            move = move_pfx <= in_pfx;
        } else {
            /*
             * If pair pfx is IPv6 (V6_64), and insert pfx is IPv4,
             * then we should move IPv6 to unpair, as IPv4 is equally compact to
             * be inserted anywhere but IPv6 is more compact in unpair.
             */
            move = 1;
        }
    }

    if (out_pfx) {
        *out_pfx = move_pfx;
    }

    return move;
}

/*!
 * \brief Deterine if we can move unpair64 1 entry UP
 *
 * \param [in] u Device u.
 * \param [in] in_pfx In unpair64 prefix
 * \param [out] in_pfx Out unpair64 prefix, if it is ok to move
 *
 * \return True if ok to move, else false.
 */
static int
can_unpair64_move_up(int u, int in_pfx, int *out_pfx)
{
    int move = 0;
    int v6;
    alpm_vrf_type_t vt;
    int move_pfx;

    if (!SIZE_PAIR(u)) {
        return 0;
    }

    unpair_pfx_decode(u, in_pfx, &v6, &vt, NULL, PKM_64(u));

    move_pfx = unpair_largest_prefix(u, vt, PKM_64(u));
    if (move_pfx == INVALID_PFX) {
        return 0;
    }

    if (v6) {
        /* If unpair pfx is v4 then we should move ipv4, as it's more impact */
        if (unpair_pfx_is_ipv4(u, move_pfx, PKM_64(u))) {
            move = 1;
        } else {
            /*
             * Indicating no IPv4.
             * Move the entry of top IPv6 pfx, if that pfx is
             * bigger than insert pfx.
             */
            move = move_pfx >= in_pfx;
        }
    } else {
        if (unpair_pfx_is_ipv4(u, move_pfx, PKM_64(u))) {
            /*
             * If unpair pfx is v4,
             * Move the entry of that unpair pfx, if that unpair pfx is
             * bigger than insert pfx.
             */
            move = move_pfx >= in_pfx;
        } else {
            move = 1;
        }
    }

    if (out_pfx) {
        *out_pfx = move_pfx;
    }

    return move;
}

/*!
 * \brief Deterine if we can move pair 1 entry DOWN
 *
 * \param [in] u Device u.
 * \param [in] in_pfx In pair prefix
 * \param [out] in_pfx Out pair prefix, if it is ok to move
 *
 * \return True if ok to move, else false.
 */
static int
can_pair_move_down(int u, uint8_t ldb, int in_pfx, int *out_pfx)
{
    int move_pfx = INVALID_PFX;
    int move = 0;
    int v6;
    alpm_vrf_type_t vt;

    if (!SIZE_UNPAIR(u, PKM_64(u))) {
        return 0;
    }

    pair_pfx_decode(u, in_pfx, &v6, &vt, NULL);

    move_pfx = pair_smallest_prefix(u, ldb, vt);

    if (move_pfx == INVALID_PFX) {
        return 0;
    }

    if (v6) {
        if (pair_pfx_is_ipv4(u, ldb, move_pfx)) {
            /*
             * If pair pfx is IPv4, and insert pfx is IPv6,
             * then we should move IPv4 to unpair, as we assume
             * that when move free entry down, the insert pfx
             * are too long to fit in the more compact packing block.
             */
            move = 1;
        } else {
            /*
             * If pair pfx is IPv6,
             * Move the entry of bottom IPv6 pfx, if that pfx is
             * smaller than insert pfx.
             */
            move = move_pfx <= in_pfx;
        }
    } else {
        if (pair_pfx_is_ipv4(u, ldb, move_pfx)) {

            /*
             * Indicating no IPv6. Both are IPv4.
             * Move the entry of bottom IPv4 pfx, if that pfx is
             * smaller than insert IPv4 pfx.
             */
            move = move_pfx <= in_pfx;
        } else {
            /*
             * If pair pfx is IPv6 (V6_64), and insert pfx is IPv4,
             * then we should move IPv6 to unpair, as IPv4 is equally compact to
             * be inserted anywhere but IPv6 is more compact in unpair.
             */
            move = 1;
        }
    }

    

    if (out_pfx) {
        *out_pfx = move_pfx;
    }

    return move;
}


/*!
 * \brief Move entries from unpair-32 TCAM to unpair-64 TCAM
 *
 * Move IPv6_64 or IPv4 entries from upair-32 TCAM to unpair-64 TCAM if
 * possible.
 * This is to create space for unpair-32 TCAM.
 *
 * \param [in] u Device u.
 * \param [in] move_pfx Move pfx
 *
 * \return SHR_E_XXX.
 */
static int
unpair32_move_up(int u, int move_pfx, int *remain_count)
{
    alpm_arg_t unpair_arg[2];
    int count = 0;
    int idx = 0;
    int half_kt;
    bcmdrd_sid_t sid;
    alpm_key_type_t kt;

    SHR_FUNC_ENTER(u);

    bcmptm_rm_alpm_arg_init(u, &unpair_arg[0]);
    bcmptm_rm_alpm_arg_init(u, &unpair_arg[1]);

    /* HW index will do */
    unpair_arg[0].index[LEVEL1] = UNPAIR_STATE_END(u, move_pfx, PKM_32(u));
    unpair_arg[0].key.pfx = move_pfx;
    unpair_arg[0].key.kt  = unpair_pfx_to_key_type(u, move_pfx, PKM_32(u));
    sid = unpair_sid(u, unpair_arg[0].key.kt);
    unpair_arg[0].key.t.ipv  = unpair_arg[0].key.kt != KEY_IPV4;
    unpair_arg[0].upkm = PKM_32(u);
    SHR_IF_ERR_EXIT(decode_read_l1_entry(u, sid, &unpair_arg[0], &unpair_arg[1]));
    unpair_arg[0].db = bcmptm_rm_alpm_db(u, unpair_arg[0].key.vt);
    unpair_arg[1].db = unpair_arg[0].db;
    l1_db_get(u, &unpair_arg[0]);
    unpair_arg[0].l1_db = unpair_arg[1].l1_db;
    *remain_count = unpair_arg[0].valid + unpair_arg[1].valid;

    kt = unpair_pfx_to_key_type(u, move_pfx, PKM_32(u));
    half_kt = half_entry_key_type(u, kt, PKM_32(u));
    count = half_kt ? 2 : 1;

    /* Reverse order has a performance advantage */
    for (idx = count - 1; idx >= 0; idx--) {
        if (unpair_arg[idx].valid) {
            alpm_pivot_t    *pivot = NULL;
            int new_index, arr_idx;
            int rv;

            SHR_IF_ERR_EXIT(l1_pvt_lookup(u, &unpair_arg[idx], &pivot));

            /* To skip pivot alloc/insert/free/delete */
            unpair_arg[idx].in_move = 1;

            unpair_arg[idx].index[LEVEL1] = INVALID_INDEX;
            rv = unpair_insert(u, &unpair_arg[idx], PKM_64(u));
            if (rv == SHR_E_FULL) {
                SHR_RETURN_VAL_EXIT(SHR_E_FULL);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
            (*remain_count)--;
            LOG_D((BSL_META("unpair32_move_up to new_pfx %d "),
                    unpair_arg[idx].key.pfx));
            new_index = unpair_arg[idx].index[LEVEL1];

            unpair_arg[idx].index[LEVEL1] = pivot->index;
            SHR_IF_ERR_EXIT(unpair_delete(u, &unpair_arg[idx]));
            LOG_D((BSL_META("from old_pfx %d - %d\n"),
                unpair_arg[idx].key.pfx,
                pfx_unpair_to_unpair(u, unpair_arg[idx].key.pfx, PKM_32(u))
                ));

            arr_idx = arr_idx_from_pvt_idx(u, pivot->index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            arr_idx = arr_idx_from_pvt_idx(u, new_index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            pivot->index = new_index;
            L1_PVT_INFO(u, arr_idx) = pivot;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move entries from unpair-64 TCAM to unpair-32 TCAM
 *
 * Move IPv6_64 or IPv4 entries from upair-64 TCAM to unpair-32 TCAM if
 * possible.
 * This is to create space for unpair-64 TCAM.
 *
 * \param [in] u Device u.
 * \param [in] move_pfx Move pfx
 *
 * \return SHR_E_XXX.
 */
static int
unpair64_move_down(int u, int move_pfx, int *remain_count)
{
    alpm_arg_t unpair_arg[2];
    int count = 0;
    int idx = 0;
    int half_kt;
    alpm_key_type_t kt;
    bcmdrd_sid_t sid;
    SHR_FUNC_ENTER(u);

    bcmptm_rm_alpm_arg_init(u, &unpair_arg[0]);
    bcmptm_rm_alpm_arg_init(u, &unpair_arg[1]);

    /* HW index will do */
    unpair_arg[0].index[LEVEL1] = UNPAIR_STATE_END(u, move_pfx, PKM_64(u));
    unpair_arg[0].key.pfx = move_pfx;
    unpair_arg[0].key.kt  = unpair_pfx_to_key_type(u, move_pfx, PKM_64(u));
    sid = unpair_sid(u, unpair_arg[0].key.kt);
    unpair_arg[0].key.t.ipv  = unpair_arg[0].key.kt != KEY_IPV4;
    unpair_arg[0].upkm = PKM_64(u);
    SHR_IF_ERR_EXIT(decode_read_l1_entry(u, sid, &unpair_arg[0], &unpair_arg[1]));
    unpair_arg[0].db = bcmptm_rm_alpm_db(u, unpair_arg[0].key.vt);
    unpair_arg[1].db = unpair_arg[0].db;
    l1_db_get(u, &unpair_arg[0]);
    unpair_arg[0].l1_db = unpair_arg[1].l1_db;
    *remain_count = unpair_arg[0].valid + unpair_arg[1].valid;

    kt = unpair_pfx_to_key_type(u, move_pfx, PKM_64(u));
    half_kt = half_entry_key_type(u, kt, PKM_64(u));
    count = half_kt ? 2 : 1;

    /* Reverse order has a performance advantage */
    for (idx = count - 1; idx >= 0; idx--) {
        if (unpair_arg[idx].valid) {
            alpm_pivot_t *pivot = NULL;
            int new_index, arr_idx;
            int rv;

            SHR_IF_ERR_EXIT(l1_pvt_lookup(u, &unpair_arg[idx], &pivot));

            /* To skip pivot alloc/insert/free/delete */
            unpair_arg[idx].in_move = 1;
            unpair_arg[idx].index[LEVEL1] = INVALID_INDEX;
            rv = unpair_insert(u, &unpair_arg[idx], PKM_32(u));
            if (rv == SHR_E_FULL) {
                SHR_RETURN_VAL_EXIT(SHR_E_FULL);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
            (*remain_count)--;
            LOG_D((BSL_META("unpair64_move_down to new_pfx %d "),
                    unpair_arg[idx].key.pfx));
            new_index = unpair_arg[idx].index[LEVEL1];

            unpair_arg[idx].index[LEVEL1] = pivot->index;
            SHR_IF_ERR_EXIT(unpair_delete(u, &unpair_arg[idx]));
            LOG_D((BSL_META("from old_pfx %d - %d\n"),
                unpair_arg[idx].key.pfx,
                pfx_unpair_to_unpair(u, unpair_arg[idx].key.pfx, PKM_64(u))
                ));

            arr_idx = arr_idx_from_pvt_idx(u, pivot->index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            arr_idx = arr_idx_from_pvt_idx(u, new_index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            pivot->index = new_index;
            L1_PVT_INFO(u, arr_idx) = pivot;
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Move entries from unpair-64 TCAM to pair TCAM
 *
 * Move IPv6_64 or IPv4 entries from upair-64 TCAM to pair TCAM if possible.
 * This is to create space for unpair-64 TCAM.
 *
 * \param [in] u Device u.
 * \param [in] move_pfx Move pfx
 *
 * \return SHR_E_XXX.
 */
static int
unpair64_move_up(int u, int move_pfx, int *remain_count)
{
    alpm_arg_t unpair_arg[2];
    int count = 0;
    int idx = 0;
    int half_kt;
    alpm_key_type_t kt;
    bcmdrd_sid_t sid;
    SHR_FUNC_ENTER(u);

    bcmptm_rm_alpm_arg_init(u, &unpair_arg[0]);
    bcmptm_rm_alpm_arg_init(u, &unpair_arg[1]);

    /* HW index will do */
    unpair_arg[0].index[LEVEL1] = UNPAIR_STATE_END(u, move_pfx, PKM_64(u));
    unpair_arg[0].key.pfx = move_pfx;
    unpair_arg[0].key.kt  = unpair_pfx_to_key_type(u, move_pfx, PKM_64(u));
    sid = unpair_sid(u, unpair_arg[0].key.kt);
    unpair_arg[0].key.t.ipv  = unpair_arg[0].key.kt != KEY_IPV4;
    unpair_arg[0].upkm = PKM_64(u);
    SHR_IF_ERR_EXIT(decode_read_l1_entry(u, sid, &unpair_arg[0], &unpair_arg[1]));
    unpair_arg[0].db = bcmptm_rm_alpm_db(u, unpair_arg[0].key.vt);
    unpair_arg[1].db = unpair_arg[0].db;
    l1_db_get(u, &unpair_arg[0]);
    unpair_arg[0].l1_db = unpair_arg[1].l1_db;

    *remain_count = unpair_arg[0].valid + unpair_arg[1].valid;

    kt = unpair_pfx_to_key_type(u, move_pfx, PKM_64(u));
    half_kt = half_entry_key_type(u, kt, PKM_64(u));
    count = half_kt ? 2 : 1;

    /* Reverse order has a performance advantage */
    for (idx = count - 1; idx >= 0; idx--) {
        if (unpair_arg[idx].valid) {
            alpm_pivot_t    *pivot = NULL;
            int new_index, arr_idx;
            int rv;

            SHR_IF_ERR_EXIT(l1_pvt_lookup(u, &unpair_arg[idx], &pivot));

            /* To skip pivot alloc/insert/free/delete */
            unpair_arg[idx].in_move = 1;

            unpair_arg[idx].index[LEVEL1] = INVALID_INDEX;
            rv = pair_insert(u, &unpair_arg[idx]);
            if (rv == SHR_E_FULL) {
                SHR_RETURN_VAL_EXIT(SHR_E_FULL);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
            (*remain_count)--;
            LOG_D((BSL_META("unpair64_move_up to new_pfx %d "),
                    unpair_arg[idx].key.pfx));
            new_index = unpair_arg[idx].index[LEVEL1];

            unpair_arg[idx].index[LEVEL1] = pivot->index;
            SHR_IF_ERR_EXIT(unpair_delete(u, &unpair_arg[idx]));
            LOG_D((BSL_META("from old_pfx %d - %d\n"),
                unpair_arg[idx].key.pfx,
                pfx_unpair_to_pair(u, unpair_arg[idx].key.pfx, PKM_64(u))
                ));

            arr_idx = arr_idx_from_pvt_idx(u, pivot->index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            arr_idx = arr_idx_from_pvt_idx(u, new_index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            pivot->index = new_index;
            L1_PVT_INFO(u, arr_idx) = pivot;
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Move entries from pair TCAM to unpair-64 TCAM
 *
 * Move IPv6_64 or IPv4 entries from pair TCAM to unpair-64 TCAM if possible.
 * This is to create space for pair TCAM.
 *
 * \param [in] u Device u.
 * \param [in] move_pfx Move pfx
 *
 * \return SHR_E_XXX.
 */
static int
pair_move_down(int u, uint8_t ldb, int move_pfx, int *remain_count)
{
    alpm_arg_t pair_arg[2];
    int count = 0;
    int idx = 0;
    int full;
    bcmdrd_sid_t sid;
    SHR_FUNC_ENTER(u);

    LOG_D((BSL_META("pair_move_down \n")));

    bcmptm_rm_alpm_arg_init(u, &pair_arg[0]);
    bcmptm_rm_alpm_arg_init(u, &pair_arg[1]);

    /* HW index will do */
    pair_arg[0].index[LEVEL1] = PAIR_STATE_END(u, move_pfx);
    pair_arg[0].key.pfx = move_pfx;
    pair_arg[0].key.kt  = pair_pfx_to_key_type(u, move_pfx);
    sid = pair_sid(u, pair_arg[0].key.kt);
    pair_arg[0].key.t.ipv  = pair_arg[0].key.kt != KEY_IPV4;
    *remain_count = pair_arg[0].valid + pair_arg[1].valid;

    SHR_IF_ERR_EXIT(decode_read_l1_entry(u, sid, &pair_arg[0], &pair_arg[1]));
    pair_arg[0].db = bcmptm_rm_alpm_db(u, pair_arg[0].key.vt);
    pair_arg[1].db = pair_arg[0].db;
    l1_db_get(u, &pair_arg[0]);
    pair_arg[0].l1_db = pair_arg[1].l1_db;

    full = pair_pfx_is_ipv6(u, ldb, move_pfx);
    count = full ? 1 : 2;

    /* Reverse order has a performance advantage */
    for (idx = count - 1; idx >= 0; idx--) {
        if (pair_arg[idx].valid) {
            alpm_pivot_t *pivot = NULL;
            int new_index, arr_idx;
            int rv;

            SHR_IF_ERR_EXIT(l1_pvt_lookup(u, &pair_arg[idx], &pivot));

            /* To skip pivot alloc/insert/free/delete */
            pair_arg[idx].in_move = 1;
            pair_arg[idx].index[LEVEL1] = INVALID_INDEX;
            rv = unpair_insert(u, &pair_arg[idx], PKM_64(u));
            if (rv == SHR_E_FULL) {
                SHR_RETURN_VAL_EXIT(SHR_E_FULL);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
            (*remain_count)--;
            new_index = pair_arg[idx].index[LEVEL1];

            pair_arg[idx].index[LEVEL1] = pivot->index;
            SHR_IF_ERR_EXIT(pair_delete(u, &pair_arg[idx]));

            arr_idx = arr_idx_from_pvt_idx(u, pivot->index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            arr_idx = arr_idx_from_pvt_idx(u, new_index);
            ALPM_ASSERT(L1_PVT_INFO(u, arr_idx) == NULL);

            pivot->index = new_index;
            L1_PVT_INFO(u, arr_idx) = pivot;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get SIP index from DIP index
 *
 * \param [in] u Device u.
 * \param [in] dip_index DIP index
 *
 * \return SIP index.
 */
static int
l1_urpf_index(int u, int dip_index)
{
    int half_size = MAX_TCAMS(u) * TCAM_DEPTH(u);

    ALPM_ASSERT(dip_index < half_size);

    return (dip_index + half_size);
}

/*!
 * \brief Read dip/sip entry from cmdproc for a given sid & index.
 *
 * Expect index as raw pt index of single-wide view.
 *
 * \param [in] u Device u.
 * \param [in] sid DRD symbol id
 * \param [in] index raw pt index of single_wide view
 * \param [out] de dip entry
 * \param [out] se sip entry
 *
 * \return SHR_E_XXX.
 */
static int
read_l1_raw(int u, bcmdrd_sid_t sid, int index, uint32_t *de, uint32_t *se)
{
    SHR_FUNC_ENTER(u);

    /* Map single-wide index to dual-wide index */
    if (is_pair_sid(u, sid)) {
        index = pair_index_encode(u, index);
    }

    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_read(u, sid, index, de));

    if (bcmptm_rm_alpm_is_urpf(u)) {
        index = l1_urpf_index(u, index);
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_read(u, sid, index, se));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write dip/sip entry to cmdproc for a given sid & index.
 *
 * Expect index as raw pt index of single-wide view.
 *
 * \param [in] u Device u.
 * \param [in] sid DRD symbol id
 * \param [in] index raw pt index of single_wide view
 * \param [in] de dip entry
 * \param [in] se sip entry
 * \param [in] pfx Translated prefix for target entry.
 *
 * \return SHR_E_XXX.
 */
static int
write_l1_raw(int u, bcmdrd_sid_t sid, int index,
             uint32_t *de, uint32_t *se, int pfx)
{
    static int wc[BCMDRD_CONFIG_MAX_UNITS] = {0};
    SHR_FUNC_ENTER(u);

    LOG_V((BSL_META(" Write[%d] [sid %d index %d"
                    " %08x %08x %08x %08x] \n"),
                    wc[u], sid, index, de[0], de[1], de[2], de[3]));
    wc[u]++;
    if (is_pair_sid(u, sid)) {
        index = pair_index_encode(u, index);
        SANITY_CHECK_PAIR_ENTRY(u, de, se);
    } else {
        SANITY_CHECK_UNPAIR_ENTRY(u, de, se, index_to_pkm(u, index));
    }

    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_write(u, sid, index, de));

    if (bcmptm_rm_alpm_is_urpf(u)) {
        int urpf_index = l1_urpf_index(u, index);
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_write(u, sid, urpf_index, se));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Clear dip/sip entry to cmdproc for a given sid & index.
 *
 * Expect index as raw pt index of single-wide view.
 *
 * \param [in] u Device u.
 * \param [in] sid DRD symbol id
 * \param [in] index raw pt index of single_wide view
 *
 * \return SHR_E_XXX.
 */
static int
clear_l1_raw(int u, bcmdrd_sid_t sid, int index)
{
    static L1_ENTRY_t de = {0};
    static L1_ENTRY_t se = {0};
    return write_l1_raw(u, sid, index, &de[0], &se[0], -1);
}

static void
l1_info_cleanup(int u)
{
    SHR_FREE(PVT_INFO_ARRAY(u));
    SHR_FREE(L1_INFO(u));
}


/*!
 * \brief Initiliaze ALPM level-1 info
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
static int
l1_info_init(int u, bool recover)
{
    int sz;
    uint8_t i, ldb;
    alpm_dev_info_t *dev_info;
    alpm_info_t *alpm_info;
    int pkm_blocks[3] = {0};
    int block_start;

    SHR_FUNC_ENTER(u);

    sz = sizeof(alpm_l1_info_t);
    ALPM_ALLOC(L1_INFO(u), sz, "Level-1 info");
    sal_memset(L1_INFO(u), 0, sz);

    alpm_info = bcmptm_rm_alpm_info_get(u);
    SHR_NULL_CHECK(alpm_info, SHR_E_INIT);

    dev_info = bcmptm_rm_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    MAX_TCAMS(u) = dev_info->tcam_blocks;
    TCAM_DEPTH(u) = dev_info->tcam_depth;
    PAIR_SID(u) = dev_info->pair_sid;
    UNPAIR_SID(u) = dev_info->unpair_sid;
    L1_INFO_FDM(u, w_template, VRF_OVERRIDE) = dev_info->wo_template;
    L1_INFO_FDM(u, w_template, VRF_PRIVATE) = dev_info->wp_template;
    L1_INFO_FDM(u, w_template, VRF_GLOBAL) = dev_info->wg_template;
    L1_INFO_FDM(u, n_template, VRF_OVERRIDE) = dev_info->no_template;
    L1_INFO_FDM(u, n_template, VRF_PRIVATE) = dev_info->np_template;
    L1_INFO_FDM(u, n_template, VRF_GLOBAL) = dev_info->ng_template;
    L1_INFO_FD(u, n_size) = dev_info->n_size;
    L1_INFO_FD(u, w_size) = dev_info->w_size;

    if (dev_info->feature_pkms == 3) {
        PKM_32(u) = PKM0;
        PKM_64(u) = PKM1;
        PKM_128(u) = PKM2;
    } else {
        PKM_32(u) = PKM0;
        PKM_64(u) = PKM0;
        PKM_128(u) = PKM1;
    }

    block_start = 0;
    START_PAIR(u, L1_DB0) = -1;
    START_UNPAIR(u, L1_DB0, PKM_64(u)) = -1;
    START_UNPAIR(u, L1_DB0, PKM_32(u)) = -1;
    START_PAIR(u, L1_DB1) = -1;
    START_UNPAIR(u, L1_DB1, PKM_64(u)) = -1;
    START_UNPAIR(u, L1_DB1, PKM_32(u)) = -1;

    for (i = 0; i < ALPM_BLOCKS; i++) {
        ldb = alpm_info->config.l1_db[i] == L1_DB0 ? L1_DB0 : L1_DB1;
        switch (alpm_info->config.l1_key_input[i]) {
        case KEY_INPUT_128:
            if (START_PAIR(u, ldb) == -1) {
                START_PAIR(u, ldb) = block_start;
            }
            PAIRS(u, ldb)++;
            pkm_blocks[0]++;
            alpm_info->config.ver0_128_enable = true;
            break;
        case KEY_INPUT_64:
            if (START_UNPAIR(u, ldb, PKM_64(u)) == -1) {
                START_UNPAIR(u, ldb, PKM_64(u)) = block_start;
            }
            START_UNPAIR(u, ldb, PKM_64(u)) = block_start;
            UNPAIRS(u, ldb, PKM_64(u))++;
            pkm_blocks[1]++;
            break;
        case KEY_INPUT_32:
        default:
            if (START_UNPAIR(u, ldb, PKM_32(u)) == -1) {
                START_UNPAIR(u, ldb, PKM_32(u)) = block_start;
            }
            UNPAIRS(u, ldb, PKM_32(u))++;
            pkm_blocks[2]++;
            break;
        }
        block_start += TCAM_DEPTH(u) << 1;
    }

    SHR_IF_ERR_EXIT(
        (MAX_TCAMS(u) >> 1 == (pkm_blocks[0] + pkm_blocks[1] + pkm_blocks[2])) ?
         SHR_E_NONE : SHR_E_PARAM);

    DUAL_MODE(u) = (bcmptm_rm_alpm_mode(u) == ALPM_MODE_PARALLEL);

    if (bcmptm_rm_alpm_is_urpf(u)) {
        if (DUAL_MODE(u) && dev_info->alpm_ver == ALPM_VERSION_0) {
            for (i = 0; i < 3; i++) {
                if (pkm_blocks[i] & 0x3) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
            }
        } else {
            for (i = 0; i < 3; i++) {
                if (pkm_blocks[i] & 0x1) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
            }
        }
    } else {
        if (DUAL_MODE(u) && dev_info->alpm_ver == ALPM_VERSION_0) {
            for (i = 0; i < 3; i++) {
                if (pkm_blocks[i] & 0x1) {
                    SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
                }
            }
        }
    }

    sz = 0;
    for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
        SIZE_UNPAIR_2(u, ldb, PKM_64(u)) =
            UNPAIRS(u, ldb, PKM_64(u)) * TCAM_DEPTH(u) * 2;

        SIZE_UNPAIR_2(u, ldb, PKM_32(u)) =
            UNPAIRS(u, ldb, PKM_32(u)) * TCAM_DEPTH(u) * 2;

        SIZE_PAIR_2(u, ldb) = PAIRS(u, ldb) * TCAM_DEPTH(u) * 2;

        if (bcmptm_rm_alpm_is_urpf(u)) {
            SIZE_UNPAIR_2(u, ldb, PKM_64(u)) >>= 1;
            if (PKM_64(u) != PKM_32(u)) {
                SIZE_UNPAIR_2(u, ldb, PKM_32(u)) >>= 1;
            }
            SIZE_PAIR_2(u, ldb) >>= 1;
        }

        /* Initialize pivot info. sz: half entry based */
        sz += (SIZE_PAIR_2(u, ldb) + SIZE_UNPAIR_2(u, ldb, PKM_64(u))) << 1;
        if (PKM_64(u) != PKM_32(u)) {
            sz += SIZE_UNPAIR_2(u, ldb, PKM_32(u)) << 1;
        }
    }
    sz = sz * sizeof(alpm_pivot_t *);
    ALPM_ALLOC(PVT_INFO_ARRAY(u), sz, "ALPM pivots");
    sal_memset(PVT_INFO_ARRAY(u), 0, sz);

    SIZE_PAIR(u) = SIZE_PAIR_2(u, L1_DB0) + SIZE_PAIR_2(u, L1_DB1);
    SIZE_UNPAIR(u, PKM_64(u)) = SIZE_UNPAIR_2(u, L1_DB0, PKM_64(u)) +
                                SIZE_UNPAIR_2(u, L1_DB1, PKM_64(u));
    SIZE_UNPAIR(u, PKM_32(u)) = SIZE_UNPAIR_2(u, L1_DB0, PKM_32(u)) +
                                SIZE_UNPAIR_2(u, L1_DB1, PKM_32(u));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initiliaze ALPM level-1 HA info
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
static int
l1_ha_init(int u, bool recover)
{
    uint32_t req_size = sizeof(alpm_l1_ha_t);
    uint32_t alloc_size = sizeof(alpm_l1_ha_t);
    uint8_t ldb;

    SHR_FUNC_ENTER(u);

    L1_HA(u) = shr_ha_mem_alloc(u,
                                BCMMGMT_PTM_COMP_ID,
                                BCMPTM_HA_SUBID_RM_ALPM_L1,
                                &alloc_size);
    SHR_NULL_CHECK(L1_HA(u), SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (recover) {
        ALPM_ASSERT(L1_HA(u));
        for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Recover: L1_RANGE_FIRST %d \n"),
                         L1_HA_RANGE1_FIRST(u, ldb)));
        }
    } else {
        for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
            L1_HA_RANGE1_FIRST(u, ldb) = RANGE1_FIRST(u, ldb);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
l1_ha_cleanup(int u, bool warm)
{
    if (warm) {
        uint8_t ldb;
        for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Warmexit: L1_RANGE_FIRST %d \n"),
                         L1_HA_RANGE1_FIRST(u, ldb)));
        }
    }
}

/*!
 * \brief Check if priorities will continue to be qualified after entry inserted
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return True: ok to go. False: check fail.
 */
static bool
priorities_check(int u, alpm_arg_t *arg)
{
    /*
     * No support for routes matching AFTER vrf specific.
     * By not allowing VRF_GLOBAL for 128-V6 routes,
     * we make sure that 64-V6 private entries dont get
     * priotized over 128-V6 public/global entries.
     */

#if 0
    if ((VRF_GLOBAL == arg->key.vt) && (arg->key.t.len > 64)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
    if ((VRF_OVERRIDE == arg->key.vt) && (arg->key.t.len <= 64)) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }
#endif
    return true;
}

/*!
 * \brief Get the min and max pfxs for IPv4
 *
 * \param [in] u Device u.
 * \param [in] pkm Packing mode.
 * \param [in] vt VRF type
 * \param [out] max_pfx Max IPv4 pfx
 * \param [out] min_pfx Min IPv4 pfx
 *
 * \return nothing
 */
static void
ipv4_bound_pfx_get(int u, uint8_t ldb, int pkm, int vt,
                   int *max_pfx, int *min_pfx)
{
    int pfx;
    int max_v4_pfx = INVALID_PFX;
    int min_v4_pfx = INVALID_PFX;

    if (pkm == PKM_128(u)) {
        pfx =  PAIR_MAX_V4_PFX(u, ldb);
        pfx = PAIR_STATE_NEXT(u, pfx);

        max_v4_pfx = pfx;

        while (pfx != INVALID_PFX) {
            min_v4_pfx = pfx;
            pfx = PAIR_STATE_NEXT(u, pfx);
        }
    } else if (pkm == PKM_64(u)) {
        pfx = UNPAIR_MAX_PFX(u, ldb, PKM_64(u));
        pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_64(u));

        while (pfx != INVALID_PFX) {
            if (unpair_pfx_is_ipv4(u, pfx, PKM_64(u))) {
                if (max_v4_pfx == INVALID_PFX) {
                    max_v4_pfx = pfx;
                }
                min_v4_pfx = pfx;
            }
            pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_64(u));
        }
    } else { /* Implies PKM_32(u) != PKM_64(u) */
        pfx = UNPAIR_MAX_PFX(u, ldb, PKM_32(u));
        pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_32(u));

        while (pfx != INVALID_PFX) {
            if (unpair_pfx_is_ipv4(u, pfx, PKM_32(u))) {
                if (max_v4_pfx == INVALID_PFX) {
                    max_v4_pfx = pfx;
                }
                min_v4_pfx = pfx;
            }
            pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_32(u));
        }
    }

    if (max_pfx) {
        *max_pfx = max_v4_pfx;
    }
    if (min_pfx) {
        *min_pfx = min_v4_pfx;
    }
}
/*!
 * \brief Get the min and max pfxs for IPv6 strict type.
 *
 * \param [in] u Device u.
 * \param [in] pkm Packing mode.
 * \param [in] kt KEY type (strict)
 * \param [in] vt VRF type
 * \param [out] max_pfx Max IPv4 pfx
 * \param [out] min_pfx Min IPv4 pfx
 *
 * \return nothing
 */
static void
ipv6_bound_pfx_get(int u,
                   uint8_t ldb,
                   int pkm,
                   alpm_key_type_t kt,
                   alpm_vrf_type_t vt,
                   int *max_pfx,
                   int *min_pfx)
{
    int pfx;
    int max_v6_pfx = INVALID_PFX;
    int min_v6_pfx = INVALID_PFX;

    ALPM_ASSERT(KEY_IS_V6(kt));

    if (pkm == PKM_128(u)) {
        pfx = PAIR_MAX_PFX(u, ldb);
        pfx = PAIR_STATE_NEXT(u, pfx);

        while (pfx != INVALID_PFX) {
            if (pair_pfx_is_strict_ipv6(u, ldb, pfx, kt)) {
                if (max_v6_pfx == INVALID_PFX) {
                    max_v6_pfx = pfx;
                }
                min_v6_pfx = pfx;
            }
            pfx = PAIR_STATE_NEXT(u, pfx);
        }

    } else if (pkm == PKM_64(u)) {
        pfx = UNPAIR_MAX_PFX(u, ldb, PKM_64(u));
        pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_64(u));

        while (pfx != INVALID_PFX) {
            if (unpair_pfx_is_strict_ipv6(u, pfx, kt, PKM_64(u))) {
                if (max_v6_pfx == INVALID_PFX) {
                    max_v6_pfx = pfx;
                }
                min_v6_pfx = pfx;
            }
            pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_64(u));
        }
    } else { /* Implies PKM_32(u) != PKM_64(u) */
        ALPM_ASSERT(kt == KEY_IPV6_32);

        pfx = UNPAIR_MAX_PFX(u, ldb, PKM_32(u));
        pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_32(u));

        while (pfx != INVALID_PFX) {
            if (unpair_pfx_is_strict_ipv6(u, pfx, kt, PKM_32(u))) {
                if (max_v6_pfx == INVALID_PFX) {
                    max_v6_pfx = pfx;
                }
                min_v6_pfx = pfx;
            }
            pfx = UNPAIR_STATE_NEXT(u, pfx, PKM_32(u));
        }
    }

    if (max_pfx) {
        *max_pfx = max_v6_pfx;
    }
    if (min_pfx) {
        *min_pfx = min_v6_pfx;
    }
}

/*!
 * \brief Map unpair prefix to pair prefix
 *
 * \param [in] u Device u.
 * \param [in] unpair_pfx Unpair prefix
 * \param [in] upkm unpair packing mode
 *
 * \return pair prefix
 */
static int
pfx_unpair_to_pair(int u, int unpair_pfx, int upkm)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    int len;
    int pair_pfx;

    if (unpair_pfx == INVALID_PFX) {
        pair_pfx = INVALID_PFX;
    } else {
        unpair_pfx_decode(u, unpair_pfx, &kt, &vt, &len, upkm);
        pair_pfx = pair_pfx_encode(u, kt, vt, len);
    }
    return pair_pfx;
}

/*!
 * \brief Map pair prefix to unpair prefix
 *
 * \param [in] u Device u.
 * \param [in] pair_pfx Pair prefix
 * \param [in] upkm unpair packing mode
 *
 * \return Unpair prefix
 */
static int
pfx_pair_to_unpair(int u, int pair_pfx, int upkm)
{
    alpm_key_type_t kt = 0;
    alpm_vrf_type_t vt = 0;
    int len = 0;
    int unpair_pfx;

    if (pair_pfx == INVALID_PFX) {
        unpair_pfx = INVALID_PFX;
    } else {
        pair_pfx_decode(u, pair_pfx, &kt, &vt, &len);
        unpair_pfx = unpair_pfx_encode(u, kt, vt, len, upkm);
    }
    return unpair_pfx;
}

/*!
 * \brief Map unpair prefix to another unpair prefix
 *
 * \param [in] u Device u.
 * \param [in] unpair_pfx Pair prefix
 * \param [in] upkm Source unpair packing mode
 *
 * \return Prefix of the other unpair block
 */
static int
pfx_unpair_to_unpair(int u, int unpair_pfx, int upkm)
{
    alpm_key_type_t kt;
    alpm_vrf_type_t vt;
    int len;
    int unpair_pfx2;

    if (unpair_pfx == INVALID_PFX) {
        unpair_pfx2 = INVALID_PFX;
    } else {
        unpair_pfx_decode(u, unpair_pfx, &kt, &vt, &len, upkm);
        unpair_pfx2 = unpair_pfx_encode(u, kt, vt, len, !upkm);
    }
    return unpair_pfx2;
}

/*!
 * \brief Retry insert after rippling entries from unpair32 to other blocks
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
ripple_insert_unpair32(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    int move_u32_pfx;
    int rem_cnt;
    int rem_cnt2;
    SHR_FUNC_ENTER(u);

    if (can_unpair32_move_up(u, arg->key.pfx, &move_u32_pfx)) {
        rv = unpair32_move_up(u, move_u32_pfx, &rem_cnt);
        if (rv == SHR_E_FULL) {
            int move_u64_pfx;
            int in_u64_pfx;
            int rem_cnt_64;
            in_u64_pfx = pfx_unpair_to_unpair(u, move_u32_pfx, PKM_32(u));
            while(rem_cnt > 0 /* until no remaining count */
               && can_unpair64_move_up(u, in_u64_pfx, &move_u64_pfx)) {
                /* Only succeeding or failing matters, but not rem_cnt */
                rv = unpair64_move_up(u, move_u64_pfx, &rem_cnt_64);
                if (SHR_SUCCESS(rv)) {
                    rv = unpair32_move_up(u, move_u32_pfx, &rem_cnt2);
                    /* If E_FULL, we try again */
                    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_FULL);
                    ALPM_ASSERT(rem_cnt2 < rem_cnt);
                    rem_cnt = rem_cnt2;
                    continue;
                }
                break;
            }

        }

        if (SHR_SUCCESS(rv)) {
            rv = unpair_insert(u, arg, PKM_32(u));
        }
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retry insert after rippling entries from unpair64 to other blocks
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
ripple_insert_unpair64(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    int move_u64_pfx;
    int rem_cnt;
    SHR_FUNC_ENTER(u);

    if (can_unpair64_move_down(u, arg->key.pfx, &move_u64_pfx)) {
        rv = unpair64_move_down(u, move_u64_pfx, &rem_cnt);
        if (SHR_SUCCESS(rv)) {
            rv = unpair_insert(u, arg, PKM_64(u));
        }
    }

    if (rv == SHR_E_FULL) {
        if (can_unpair64_move_up(u, arg->key.pfx, &move_u64_pfx)) {
            rv = unpair64_move_up(u, move_u64_pfx, &rem_cnt);
            if (SHR_SUCCESS(rv)) {
                rv = unpair_insert(u, arg, PKM_64(u));
            }
        }
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retry insert after rippling entries from pair to other blocks
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
ripple_insert_pair(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    int move_pair_pfx;
    int rem_cnt, rem_cnt2;
    uint8_t ldb = arg->l1_db;
    SHR_FUNC_ENTER(u);

    if (can_pair_move_down(u, ldb, arg->key.pfx, &move_pair_pfx)) {
        rv = pair_move_down(u, ldb, move_pair_pfx, &rem_cnt);
        if (rv == SHR_E_FULL) {
            int move_u64_pfx;
            int in_u64_pfx;
            int rem_cnt_64;
            in_u64_pfx = pfx_pair_to_unpair(u, move_pair_pfx, PKM_64(u));
            while(rem_cnt > 0 /* until no remaining count */
               && can_unpair64_move_down(u, in_u64_pfx, &move_u64_pfx)) {
                /* Only succeeding or failing matters, but not rem_cnt */
                rv = unpair64_move_down(u, move_u64_pfx, &rem_cnt_64);
                if (SHR_SUCCESS(rv)) {
                    rv = unpair64_move_down(u, move_u64_pfx, &rem_cnt_64);
                }
                if (SHR_SUCCESS(rv)) {
                    rv = pair_move_down(u, ldb, move_pair_pfx, &rem_cnt2);
                    /* If E_FULL, we try again */
                    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_FULL);
                    ALPM_ASSERT(rem_cnt2 < rem_cnt);
                    rem_cnt = rem_cnt2;
                    continue;
                }
                break;
            }
        }

        if (SHR_SUCCESS(rv)) {
            rv = pair_insert(u, arg);
        }
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get target pkm based on upper_bound
 *
 * \param [in] u Device u.
 * \param [in] upper_bound Block bound number holding prefix bigger than target
 * \param [in] lower_bound Block bound number holding prefix smaller than target
 * \param [out] pkms Target packing mode
 *
 * \return SHR_E_XXX
 */
static void
bound_pkm_get(int u, int upper_bound, int lower_bound, insert_pkms_t *pkms)
{
    int a, b, i;

    if (upper_bound == -1) {
        a = 0;
    } else {
        a = (upper_bound) / 2;
    }

    if (lower_bound == -1) {
        b = 2;
    } else {
        b = (lower_bound) / 2;
    }
    ALPM_ASSERT(a <= b);
    for (i = a; i <= b; i++) {
        switch (i) {
        case 0:
            pkms->pair = 1;
            break;
        case 1:
            pkms->unpair64 = 1;
            break;
        case 2:
            pkms->unpair32 = 1;
            break;
        default:
            ALPM_ASSERT(0);
            break;
        }
    }
}

/*!
 * \brief Get target pkm to insert for strict IPv6_32
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pkms Target packing mode
 *
 * \return SHR_E_XXX
 */
static int
insert_strict_ipv6_32_pkm_get(int u, alpm_arg_t *arg, insert_pkms_t *pkms)
{
    int bound[6] = {-1,-1, -1, -1, -1, -1};
    int bound_count = 6;
    int lower_bound = -1;
    int first_bound = -1;
    int upper_bound = -1;
    int cmp_pfx;
    int i;
    uint8_t ldb = arg->l1_db;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(PKM_64(u) != PKM_32(u));

    sal_memset(pkms, 0, sizeof(*pkms));

    cmp_pfx = pair_pfx_encode(u, arg->key.t.ipv, arg->key.vt, arg->key.t.len);

    ipv6_bound_pfx_get(u, ldb, PKM_128(u), KEY_IPV6_32, arg->key.vt,
                       &bound[0], &bound[1]);
    ALPM_ASSERT(bound[0] >= bound[1]);

    ipv6_bound_pfx_get(u, ldb, PKM_64(u), KEY_IPV6_32, arg->key.vt,
                       &bound[2], &bound[3]);
    bound[2] = pfx_unpair_to_pair(u, bound[2], PKM_64(u));
    bound[3] = pfx_unpair_to_pair(u, bound[3], PKM_64(u));
    ALPM_ASSERT(bound[1] >= bound[2] || bound[1] == INVALID_PFX);
    ALPM_ASSERT(bound[2] >= bound[3]);

    ipv6_bound_pfx_get(u, ldb, PKM_32(u), KEY_IPV6_32, arg->key.vt,
                       &bound[4], &bound[5]);
    bound[4] = pfx_unpair_to_pair(u, bound[4], PKM_32(u));
    bound[5] = pfx_unpair_to_pair(u, bound[5], PKM_32(u));
    ALPM_ASSERT(bound[3] >= bound[4] || bound[3] == INVALID_PFX);
    ALPM_ASSERT(bound[4] >= bound[5]);

    /* Get first bound for ipv6_32 */
    for (i = 0; i < bound_count; i++) {
        if (bound[i] != INVALID_PFX) {
            first_bound = i;
            break;
        }
    }

    if (i == bound_count) { /* no ipv6_32 exists, try all */
        pkms->pair = 1;
        pkms->unpair64 = 1;
        pkms->unpair32 = 1;
    } else {
        ALPM_ASSERT(bound[i] != INVALID_PFX);
        for (i = first_bound; i < bound_count; i++) {
            if (bound[i] > cmp_pfx) {
                upper_bound = i;
            } else if (bound[i] < cmp_pfx) {
                if (lower_bound == -1) {
                    lower_bound = i;
                }
            }
        }
        bound_pkm_get(u, upper_bound, lower_bound, pkms);
    }

    pkms->unpair64 = SIZE_UNPAIR(u, PKM_64(u)) && pkms->unpair64;
    pkms->unpair32 = SIZE_UNPAIR(u, PKM_32(u)) && pkms->unpair32;
    pkms->unpair32 = (PKM_64(u) != PKM_32(u)) && pkms->unpair32;
    pkms->pair = SIZE_PAIR(u) && pkms->pair;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get target pkm to insert for strict IPv6_64
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pkms Target packing mode
 *
 * \return SHR_E_XXX
 */
static int
insert_strict_ipv6_64_pkm_get(int u, alpm_arg_t *arg, insert_pkms_t *pkms)
{
    int bound[4] = {-1,-1, -1, -1};
    int bound_count = 4;
    int lower_bound = -1;
    int first_bound = -1;
    int upper_bound = -1;
    int cmp_pfx;
    int i;
    uint8_t ldb = arg->l1_db;
    SHR_FUNC_ENTER(u);

    sal_memset(pkms, 0, sizeof(*pkms));

    cmp_pfx = pair_pfx_encode(u, arg->key.t.ipv, arg->key.vt, arg->key.t.len);

    ipv6_bound_pfx_get(u, ldb, PKM_128(u), KEY_IPV6_64, arg->key.vt,
                       &bound[0], &bound[1]);
    ALPM_ASSERT(bound[0] >= bound[1]);

    ipv6_bound_pfx_get(u, ldb, PKM_64(u), KEY_IPV6_64, arg->key.vt,
                       &bound[2], &bound[3]);
    bound[2] = pfx_unpair_to_pair(u, bound[2], PKM_64(u));
    bound[3] = pfx_unpair_to_pair(u, bound[3], PKM_64(u));
    ALPM_ASSERT(bound[1] >= bound[2] || bound[1] == INVALID_PFX);
    ALPM_ASSERT(bound[2] >= bound[3]);

    /* Get first bound for ipv6_64 */
    for (i = 0; i < bound_count; i++) {
        if (bound[i] != INVALID_PFX) {
            first_bound = i;
            break;
        }
    }

    if (i == bound_count) { /* no ipv6_64 exists, try all */
        pkms->pair = 1;
        pkms->unpair64 = 1;
    } else {
        ALPM_ASSERT(bound[i] != INVALID_PFX);
        for (i = first_bound; i < bound_count; i++) {
            if (bound[i] > cmp_pfx) {
                upper_bound = i;
            } else if (bound[i] < cmp_pfx) {
                if (lower_bound == -1) {
                    lower_bound = i;
                }
            }
        }
        bound_pkm_get(u, upper_bound, lower_bound, pkms);
        /* Clear unpair32 if any */
        pkms->unpair32 = 0;
    }

    pkms->unpair64 = SIZE_UNPAIR(u, PKM_64(u)) && pkms->unpair64;
    pkms->pair = SIZE_PAIR(u) && pkms->pair;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get target pkm to insert for IPv4
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pkms Target packing mode
 *
 * \return SHR_E_XXX
 */
static int
insert_ipv4_pkm_get(int u, alpm_arg_t *arg, insert_pkms_t *pkms)
{
    int bound[6] = {-1,-1, -1, -1, -1, -1};
    int bound_count = 6;
    int first_bound = -1;
    int upper_bound = -1;
    int lower_bound = -1;
    int cmp_pfx;
    int i;
    uint8_t ldb = arg->l1_db;
    SHR_FUNC_ENTER(u);

    sal_memset(pkms, 0, sizeof(*pkms));

    cmp_pfx = pair_pfx_encode(u, arg->key.t.ipv, arg->key.vt, arg->key.t.len);

    ipv4_bound_pfx_get(u, ldb, PKM_128(u), arg->key.vt, &bound[0], &bound[1]);
    ALPM_ASSERT(bound[0] >= bound[1]);

    ipv4_bound_pfx_get(u, ldb, PKM_64(u), arg->key.vt, &bound[2], &bound[3]);
    bound[2] = pfx_unpair_to_pair(u, bound[2], PKM_64(u));
    bound[3] = pfx_unpair_to_pair(u, bound[3], PKM_64(u));
    ALPM_ASSERT(bound[1] >= bound[2] || bound[1] == INVALID_PFX);
    ALPM_ASSERT(bound[2] >= bound[3]);

    if (PKM_64(u) != PKM_32(u)) {
        bound_count = 6;
        ipv4_bound_pfx_get(u, ldb, PKM_32(u), arg->key.vt, &bound[4], &bound[5]);
        bound[4] = pfx_unpair_to_pair(u, bound[4], PKM_32(u));
        bound[5] = pfx_unpair_to_pair(u, bound[5], PKM_32(u));
        ALPM_ASSERT(bound[3] >= bound[4] || bound[3] == INVALID_PFX);
        ALPM_ASSERT(bound[4] >= bound[5]);
    } else {
        bound_count = 4;
    }

    /* Get first bound for ipv4 */
    for (i = 0; i < bound_count; i++) {
        if (bound[i] != INVALID_PFX) {
            first_bound = i;
            break;
        }
    }

    if (i == bound_count) { /* no ipv4 exists, try all */
        pkms->pair = 1;
        pkms->unpair64 = 1;
        pkms->unpair32 = 1;
    } else {
        ALPM_ASSERT(bound[i] != INVALID_PFX);
        for (i = first_bound; i < bound_count && i >= 0; i++) {
            if (bound[i] > cmp_pfx) {
                upper_bound = i;
            } else if (bound[i] < cmp_pfx) {
                if (lower_bound == -1) {
                    lower_bound = i;
                }
            }
        }
        bound_pkm_get(u, upper_bound, lower_bound, pkms);
    }

    pkms->unpair64 = SIZE_UNPAIR(u, PKM_64(u)) && pkms->unpair64;
    pkms->unpair32 = SIZE_UNPAIR(u, PKM_32(u)) && pkms->unpair32;
    pkms->unpair32 = (PKM_64(u) != PKM_32(u)) && pkms->unpair32;
    pkms->pair = SIZE_PAIR(u) && pkms->pair;

    SHR_FUNC_EXIT();
}


/*!
 * \brief Insert strict IPv6_32 route
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
l1_insert_strict_ipv6_32(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    insert_pkms_t pkms;
    int rippled = 0;

    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(insert_strict_ipv6_32_pkm_get(u, arg, &pkms));

    if (pkms.unpair32) {
        rv = unpair_insert(u, arg, PKM_32(u));
        if (rv == SHR_E_FULL) {
            rv = ripple_insert_unpair32(u, arg);
            rippled = 1;
        }
    }

    if (rv == SHR_E_FULL && pkms.unpair64) {
        rv = unpair_insert(u, arg, PKM_64(u));
        if (rv == SHR_E_FULL && !rippled) {
            rv = ripple_insert_unpair64(u, arg);
            rippled = 1;
        }
    }

    if (rv == SHR_E_FULL && pkms.pair) {
        rv = pair_insert(u, arg);
        if (rv == SHR_E_FULL && !rippled) {
            rv = ripple_insert_pair(u, arg);
        }
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert strict IPv6_64 route
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
l1_insert_strict_ipv6_64(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    insert_pkms_t pkms;
    int rippled = 0;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(insert_strict_ipv6_64_pkm_get(u, arg, &pkms));

    if (pkms.unpair64) {
        rv = unpair_insert(u, arg, PKM_64(u));
        if (rv == SHR_E_FULL) {
            rv = ripple_insert_unpair64(u, arg);
            rippled = 1;
        }
    }

    if (rv == SHR_E_FULL && pkms.pair) {
        rv = pair_insert(u, arg);
        if (rv == SHR_E_FULL && !rippled) {
            rv = ripple_insert_pair(u, arg);
        }
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert strict IPv6_128 route
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
l1_insert_strict_ipv6_128(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    SHR_FUNC_ENTER(u);

    /* Have no choice but to try pair */
    if (SIZE_PAIR(u)) {
        rv = pair_insert(u, arg);
        if (rv == SHR_E_FULL) {
            rv = ripple_insert_pair(u, arg);
        }
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert strict IPv4 route
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
static int
l1_insert_ipv4(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    insert_pkms_t pkms;
    int u32_pfx = INVALID_PFX;
    int u64_pfx = INVALID_PFX;
    int pair_pfx = INVALID_PFX;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(insert_ipv4_pkm_get(u, arg, &pkms));

    if (pkms.unpair32) {
        rv = unpair_insert(u, arg, PKM_32(u));
        u32_pfx = arg->key.pfx;
    }

    if (rv == SHR_E_FULL && pkms.unpair64) {
        rv = unpair_insert(u, arg, PKM_64(u));
        u64_pfx = arg->key.pfx;
    }

    if (rv == SHR_E_FULL && pkms.pair) {
        rv = pair_insert(u, arg);
        pair_pfx = arg->key.pfx;
    }

    if (rv == SHR_E_FULL) {
        if (pkms.unpair32) {
            ALPM_ASSERT(u32_pfx != INVALID_PFX);
            arg->key.pfx = u32_pfx;
            rv = ripple_insert_unpair32(u, arg);
        } else if (pkms.unpair64) {
            ALPM_ASSERT(u64_pfx != INVALID_PFX);
            arg->key.pfx = u64_pfx;
            rv = ripple_insert_unpair64(u, arg);
        } else if (pkms.pair) {
            ALPM_ASSERT(pair_pfx != INVALID_PFX);
            arg->key.pfx = pair_pfx;
            rv = ripple_insert_pair(u, arg);
        }
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}


static int
l1_pivot_recover(int u, alpm_arg_t * arg)
{
    alpm_pivot_t *pivot;
    int index;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(l1_pvt_alloc(u, arg, &pivot));
    SHR_IF_ERR_EXIT(l1_pvt_insert(u, arg, pivot));
    ALPM_ASSERT(pivot);
    index = arr_idx_from_pvt_idx(u, arg->index[LEVEL1]);
    L1_PVT_INFO(u, index) = pivot;
    arg->pivot[LEVEL1] = pivot;
    if (!arg->a1.direct_route) {
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_ln_recover(u, LEVEL2, arg));
        ALPM_ASSERT(pivot->bkt);
        pivot->bkt->log_bkt = arg->log_bkt[LEVEL1];
    }

exit:
    SHR_FUNC_EXIT();
}

static int
unpair_state_recover(int u, alpm_arg_t *arg, int upkm)
{
    int pfx;
    int idx = HW_INDEX(arg->index[LEVEL1]);
    alpm_key_type_t kt = arg->key.kt;
    alpm_vrf_type_t vt = arg->key.vt;

    SHR_FUNC_ENTER(u);
    pfx = unpair_pfx_encode(u, arg->key.t.ipv, vt,
                            arg->key.t.len,
                            upkm);
    if (UNPAIR_STATE_VENT(u, pfx, upkm) == 0) {
        UNPAIR_STATE_START(u, pfx, upkm) = idx;
        UNPAIR_STATE_END(u, pfx, upkm) = idx;
    } else {
        UNPAIR_STATE_END(u, pfx, upkm) = idx;
    }

    UNPAIR_USED_COUNT(u, kt, vt, upkm)++;
    if (!IS_HALF_R_INDEX(arg->index[LEVEL1])) {
        UNPAIR_STATE_VENT(u, pfx, upkm)++;
    }
    if (half_entry_key_type(u, kt, upkm)) {
        if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
            UNPAIR_HALF_COUNT(u, kt, vt, upkm)--;
            ALPM_ASSERT(UNPAIR_HALF_COUNT(u, kt, vt, upkm) >= 0);
        } else {
            UNPAIR_HALF_COUNT(u, kt, vt, upkm)++;
        }
        unpair_half_entry_update(u, pfx, upkm);
    }
    SHR_FUNC_EXIT();
}

static int
unpair_state_recover_done(int u, uint8_t ldb, int upkm)
{
    int pfx, prev_pfx;
    int i;
    int low_bound, low_bound2;
    int max_pfx[] = {
        -1,
        UNPAIR_MAX_PFX(u, ldb, upkm)};

    ALPM_ASSERT(!DUAL_MODE(u) || UNPAIR_MAX_PFX(u, L1_DB1, upkm) == -1);
    low_bound = SIZE_PAIR(u) + SIZE_UNPAIR(u, upkm);
    if (PKM_64(u) != PKM_32(u) && upkm == PKM_32(u)) {
        low_bound += SIZE_UNPAIR(u, PKM_64(u));
    }
    for (i = 1; i < COUNTOF(max_pfx); i++) {
        if (max_pfx[i] == -1) {
            continue;
        }
        UNPAIR_STATE_PREV(u, max_pfx[i], upkm) = -1;
        low_bound2 = -1;
        for (pfx = prev_pfx = max_pfx[i]; pfx > max_pfx[i - 1]; pfx--) {
            if (-1 == UNPAIR_STATE_START(u, pfx, upkm)) {
                continue;
            }

            /*
             * The upper bound and lower bound for a given pfx zone (Private,
             * Global) do not have to be exactly
             * same as the state in the previous run.
             * Example, when the fent(max_pfx) != 0, those free entries,
             * when recovered, will be shifted to the previous zone.
             * The only exception is the toppest max_pfx in the tcam block.
             */
            if (max_pfx[i] != UNPAIR_MAX_PFX(u, ldb, upkm)) {
                UNPAIR_STATE_START(u, max_pfx[i], upkm) = low_bound2;
                UNPAIR_STATE_END(u, max_pfx[i], upkm) = low_bound2 - 1;
            }

            if (low_bound2 == -1) {
                low_bound2 = UNPAIR_STATE_START(u, pfx, upkm);
            }
            if (prev_pfx != pfx) {
                UNPAIR_STATE_PREV(u, pfx, upkm) = prev_pfx;
                UNPAIR_STATE_NEXT(u, prev_pfx, upkm) = pfx;
                UNPAIR_STATE_FENT(u, prev_pfx, upkm) =
                    UNPAIR_STATE_START(u, pfx, upkm) -
                    UNPAIR_STATE_END(u, prev_pfx, upkm) - 1;
                ALPM_ASSERT(UNPAIR_STATE_FENT(u, prev_pfx, upkm) >= 0);
            }
            prev_pfx = pfx;
        }
        UNPAIR_STATE_NEXT(u, prev_pfx, upkm) = -1;
        UNPAIR_STATE_FENT(u, prev_pfx, upkm) =
            low_bound - UNPAIR_STATE_END(u, prev_pfx, upkm) - 1;
        low_bound = low_bound2;
    }

    return SHR_E_NONE;
}

static int
unpair_recover(int u, uint8_t ldb)
{
    alpm_arg_t arg = {{0}}, arg2 = {{0}};
    size_t index;
    bcmdrd_sid_t sid = UNPAIR_SID(u);
    size_t size_unpair;
    bool is_ipv6;
    L1_ENTRY_t de, se;

    SHR_FUNC_ENTER(u);
    index = SIZE_PAIR(u);
    size_unpair = SIZE_UNPAIR(u, PKM_64(u)) + SIZE_PAIR(u);
    is_ipv6 = false;
    while (index < size_unpair) {
        if (!is_ipv6) {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, sid, index, de, se));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg, &de, &se, L1V_NARROW_0));
            if (arg.valid) {
                if (arg.key.t.ipv == IP_VER_6) {
                    is_ipv6 = true;
                    /* Clear again to prevent ip mask pollution */
                    sal_memset(&arg, 0, sizeof(arg));
                    continue;
                }
                arg.index[LEVEL1] = HALF_INDEX_L(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, &arg, PKM_64(u)));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg));
                sal_memset(&arg.key.t.trie_ip, 0, sizeof(arg.key.t.trie_ip));
            } else {
                sal_memset(&arg, 0, sizeof(arg));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg2, &de, &se, L1V_NARROW_1));
            if (arg2.valid) {
                arg2.index[LEVEL1] = HALF_INDEX_R(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, &arg2, PKM_64(u)));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg2));
                sal_memset(&arg2.key.t.trie_ip, 0, sizeof(arg2.key.t.trie_ip));
            } else {
                sal_memset(&arg2, 0, sizeof(arg2));
            }
        } else {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, sid, index, de, se));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg, &de, &se, L1V_NARROW_FULL));
            if (arg.valid) {
                arg.index[LEVEL1] = FULL_INDEX(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, &arg, PKM_64(u)));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg));
                sal_memset(&arg.key.t.trie_ip, 0, sizeof(arg.key.t.trie_ip));
            } else {
                sal_memset(&arg, 0, sizeof(arg));
            }
        }
        index++;
    }
    SHR_IF_ERR_EXIT(unpair_state_recover_done(u, ldb, PKM_64(u)));

    if (PKM_64(u) != PKM_32(u)) {
        /* Clear again to prevent ip mask pollution */
        sal_memset(&arg, 0, sizeof(arg));
        sal_memset(&arg2, 0, sizeof(arg2));
        ALPM_ASSERT(index == size_unpair);
        size_unpair = SIZE_UNPAIR(u, PKM_32(u)) + index;
        while (index < size_unpair) {
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, sid, index, de, se));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg, &de, &se, L1V_NARROW_0));
            if (arg.valid) {
                arg.index[LEVEL1] = HALF_INDEX_L(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, &arg, PKM_32(u)));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg));
                sal_memset(&arg.key.t.trie_ip, 0, sizeof(arg.key.t.trie_ip));
            } else {
                sal_memset(&arg, 0, sizeof(arg));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg2, &de, &se, L1V_NARROW_1));
            if (arg2.valid) {
                arg2.index[LEVEL1] = HALF_INDEX_R(index);
                SHR_IF_ERR_EXIT(unpair_state_recover(u, &arg2, PKM_32(u)));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg2));
                sal_memset(&arg2.key.t.trie_ip, 0, sizeof(arg2.key.t.trie_ip));
            } else {
                sal_memset(&arg2, 0, sizeof(arg2));
            }
            index++;
        }
        SHR_IF_ERR_EXIT(unpair_state_recover_done(u, ldb, PKM_32(u)));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
pair_state_recover(int u, alpm_arg_t * arg)
{
    int pfx;
    int idx = HW_INDEX(arg->index[LEVEL1]);
    alpm_key_type_t kt = arg->key.kt;
    alpm_vrf_type_t vt = arg->key.vt;

    SHR_FUNC_ENTER(u);
    pfx = pair_pfx_encode(u, arg->key.t.ipv, vt,
                          arg->key.t.len);
    if (PAIR_STATE_VENT(u, pfx) == 0) {
        PAIR_STATE_START(u, pfx) = idx;
        PAIR_STATE_END(u, pfx) = idx;
    } else {
        PAIR_STATE_END(u, pfx) = idx;
    }
    if (!IS_HALF_R_INDEX(arg->index[LEVEL1])) {
        PAIR_STATE_VENT(u, pfx)++;
    }

    PAIR_USED_COUNT(u, kt, vt)++;
    if (half_entry_key_type(u, kt, PKM_128(u))) {
        pair_half_entry_update(u, pfx);
        if (IS_HALF_R_INDEX(arg->index[LEVEL1])) {
            PAIR_HALF_COUNT(u, kt, vt)--;
            ALPM_ASSERT(PAIR_HALF_COUNT(u, kt, vt) >= 0);
        } else {
            PAIR_HALF_COUNT(u, kt, vt)++;
        }
    }
    SHR_FUNC_EXIT();
}

static int
pair_state_recover_done(int u, uint8_t ldb)
{
    int pfx, prev_pfx, i;
    int prev_end, curr_start;
    int low_bound, low_bound2;
    bool is_ipv4;
    int max_pfx[] = {
        -1,
        PAIR_MAX_V4_PFX(u, ldb), /* Always valid */
        PAIR_MAX_PFX(u, ldb) /* Always valid */
    };

    ALPM_ASSERT(!DUAL_MODE(u) || PAIR_MAX_PFX(u, L1_DB1) == -1);
    ALPM_ASSERT(!DUAL_MODE(u) || PAIR_MAX_V4_PFX(u, L1_DB1) == -1);
    low_bound = SIZE_PAIR(u);
    for (i = 1; i < COUNTOF(max_pfx); i++) {
        if (max_pfx[i] == -1) {
            continue;
        }
        PAIR_STATE_PREV(u, max_pfx[i]) = -1;
        is_ipv4 = pair_pfx_is_ipv4(u, ldb, max_pfx[i]);
        low_bound2 = -1;
        for (pfx = prev_pfx = max_pfx[i]; pfx > max_pfx[i - 1]; pfx--) {
            if (-1 == PAIR_STATE_START(u, pfx)) {
                continue;
            }
            if (low_bound2 == -1) {
                low_bound2 = PAIR_STATE_START(u, pfx);
                /*
                 * The upper bound and lower bound for a given pfx zone (V4,
                 * V4_Global, V6, V6_Global) do not have to be exactly
                 * same as the state in the previous run.
                 * Example, when the fent(max_pfx) != 0, those free entries,
                 * when recovered, will be shifted to the previous zone.
                 * The only exception is the toppest max_pfx in the tcam block.
                 */
                if (max_pfx[i] != PAIR_MAX_PFX(u, ldb)) {
                    PAIR_STATE_START(u, max_pfx[i]) = low_bound2;
                    PAIR_STATE_END(u, max_pfx[i]) = low_bound2 - 1;
                }
            }
            if (prev_pfx != pfx) {
                PAIR_STATE_PREV(u, pfx) = prev_pfx;
                PAIR_STATE_NEXT(u, prev_pfx) = pfx;
                ALPM_ASSERT(pair_pfx_is_ipv4(u, ldb, pfx) == is_ipv4);
                if (is_ipv4) {
                    curr_start = PAIR_STATE_START(u, pfx);
                    prev_end = PAIR_STATE_END(u, prev_pfx);
                    if (!LAST_PFX_SPAN(u, ldb) &&
                        prev_pfx == RANGE1_LAST_PFX(u, ldb)) {
                        PAIR_STATE_FENT(u, prev_pfx) =
                            RANGE1_LAST(u, ldb) - prev_end;
                        PAIR_STATE_FENT(u, prev_pfx) +=
                            curr_start - RANGE2_FIRST(u, ldb);
                    } else {
                        PAIR_STATE_FENT(u, prev_pfx) =
                            curr_start - prev_end - 1;
                    }
                } else {
                    curr_start = pair_index_encode(u, PAIR_STATE_START(u, pfx));
                    prev_end = pair_index_encode(u, PAIR_STATE_END(u, prev_pfx));
                    PAIR_STATE_FENT(u, prev_pfx) = (curr_start - prev_end - 1);
                    PAIR_STATE_FENT(u, prev_pfx) <<= 1;
                }
                ALPM_ASSERT(PAIR_STATE_FENT(u, prev_pfx) >= 0);
            }
            prev_pfx = pfx;
        }
        PAIR_STATE_NEXT(u, prev_pfx) = -1;
        if (is_ipv4) {
            if (!LAST_PFX_SPAN(u, ldb) &&
                prev_pfx == RANGE1_LAST_PFX(u, ldb)) {
                PAIR_STATE_FENT(u, prev_pfx) =
                    RANGE1_LAST(u, ldb) - PAIR_STATE_END(u, prev_pfx);
                PAIR_STATE_FENT(u, prev_pfx) +=
                    low_bound - RANGE2_FIRST(u, ldb);
            } else {
                PAIR_STATE_FENT(u, prev_pfx) =
                    low_bound - PAIR_STATE_END(u, prev_pfx) - 1;
            }
        } else {
            PAIR_STATE_FENT(u, prev_pfx) =
               (pair_index_encode(u, low_bound) -
                pair_index_encode(u, PAIR_STATE_END(u, prev_pfx)) - 1) << 1;
        }
        ALPM_ASSERT(PAIR_STATE_FENT(u, prev_pfx) >= 0);
        low_bound = low_bound2;
    }

    return SHR_E_NONE;
}

static int
pair_recover(int u, uint8_t ldb)
{
    alpm_arg_t arg = {{0}}, arg2 = {{0}};
    int index;
    bcmdrd_sid_t sid = PAIR_SID(u);
    int size_pair;
    bool is_ipv6;
    L1_ENTRY_t de, se;
    int max_v4_pfx;

    SHR_FUNC_ENTER(u);
    size_pair = SIZE_PAIR_2(u, ldb);
    index = START_PAIR(u, ldb);
    is_ipv6 = true;
    while (index < size_pair) {
        if (is_ipv6) {
            if (index == L1_HA_RANGE1_FIRST(u, ldb)) {
                /* If size_pair equals to zero, none matters. */
                sid = UNPAIR_SID(u);
                RANGE1_FIRST(u, ldb) = index;
                RANGE2_FIRST(u, ldb) = RANGE1_FIRST(u, ldb) + TCAM_DEPTH(u);
                RANGE1_LAST(u, ldb) =
                    ((tcam_num(u, RANGE1_FIRST(u, ldb)) + 1) * TCAM_DEPTH(u)) - 1;
                max_v4_pfx = pair_max_pfx(u, KEY_IPV4, arg.key.vt);
                PAIR_STATE_START(u, max_v4_pfx) = RANGE1_FIRST(u, ldb);
                PAIR_STATE_END(u, max_v4_pfx) = RANGE1_FIRST(u, ldb) - 1;

                is_ipv6 = false;
                /* Clear again to prevent ip mask pollution */
                sal_memset(&arg, 0, sizeof(arg));
                continue;
            }
            SHR_IF_ERR_EXIT(
                read_l1_raw(u, sid, index, de, se));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg, &de, &se, L1V_WIDE));
            if (arg.valid) {
                arg.index[LEVEL1] = FULL_INDEX(index);
                SHR_IF_ERR_EXIT(pair_state_recover(u, &arg));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg));
                sal_memset(&arg.key.t.trie_ip, 0, sizeof(arg.key.t.trie_ip));
            } else {
                sal_memset(&arg, 0, sizeof(arg));
            }
            index++;
            if (index_in_right_tcam(u, index)) {
                index += TCAM_DEPTH(u);
            }
        } else { /* IPv4 */

            SHR_IF_ERR_EXIT(
                read_l1_raw(u, sid, index, de, se));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg, &de, &se, L1V_NARROW_0));
            if (arg.valid) {
                arg.index[LEVEL1] = HALF_INDEX_L(index);
                SHR_IF_ERR_EXIT(pair_state_recover(u, &arg));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg));
                sal_memset(&arg.key.t.trie_ip, 0, sizeof(arg.key.t.trie_ip));
            } else {
                sal_memset(&arg, 0, sizeof(arg));
            }
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_arg_fill(u, &arg2, &de, &se, L1V_NARROW_1));
            if (arg2.valid) {
                arg2.index[LEVEL1] = HALF_INDEX_R(index);
                SHR_IF_ERR_EXIT(pair_state_recover(u, &arg2));
                SHR_IF_ERR_EXIT(l1_pivot_recover(u, &arg2));
                sal_memset(&arg2.key.t.trie_ip, 0, sizeof(arg2.key.t.trie_ip));
            } else {
                sal_memset(&arg2, 0, sizeof(arg2));
            }
            if (arg.valid) {
                if (index <= RANGE1_LAST(u, ldb)) {
                    RANGE1_LAST_PFX(u, ldb) =
                        pair_pfx_encode(u, IP_VER_4, arg.key.vt, arg.key.t.len);
                    if (index == RANGE1_LAST(u, ldb)) {
                        index = RANGE2_FIRST(u, ldb);
                        continue;
                    }
                } else if (index == RANGE2_FIRST(u, ldb)) {
                    if (RANGE1_LAST_PFX(u, ldb) ==
                        pair_pfx_encode(u, IP_VER_4, arg.key.vt, arg.key.t.len)) {
                        LAST_PFX_SPAN(u, ldb) = 1;
                    }
                }
            }
            index++;
        }
    }
    SHR_IF_ERR_EXIT(pair_state_recover_done(u, ldb));
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions for unit-test
 */
void
bcmptm_rm_alpm_l1_ut_unpair_pfx_decode(int u, int pfx,
                                       alpm_key_type_t *kt,
                                       alpm_vrf_type_t *vt,
                                       int *len,
                                       int upkm)
{
    unpair_pfx_decode(u, pfx, kt, vt, len, upkm);
    return;
}

int
bcmptm_rm_alpm_l1_ut_unpair_pfx_encode(int u, int ipv6,
                                       alpm_vrf_type_t vt,
                                       int len,
                                       int upkm)
{
    return unpair_pfx_encode(u, ipv6, vt, len, upkm);
}

void
bcmptm_rm_alpm_l1_ut_pair_pfx_decode(int u, int pfx,
                                     alpm_key_type_t *kt,
                                     alpm_vrf_type_t *vt,
                                     int *len)
{
    pair_pfx_decode(u, pfx, kt, vt, len);
    return;
}

int
bcmptm_rm_alpm_l1_ut_pair_pfx_encode(int u, int ipv6,
                                     alpm_vrf_type_t vt,
                                     int len)
{
    return pair_pfx_encode(u, ipv6, vt, len);
}


/*******************************************************************************
 * Public Functions
 */
void
bcmptm_rm_alpm_l1_ha_sync(int u)
{
    uint8_t ldb;
    for (ldb = L1_DB0; ldb < L1_DBS; ldb++) {
        L1_HA_RANGE1_FIRST(u, ldb) = RANGE1_FIRST(u, ldb);

    }
}

int
bcmptm_rm_alpm_l1_insert(int u, alpm_arg_t *arg)
{
    int rv = SHR_E_FULL;
    SHR_FUNC_ENTER(u);

    if (!priorities_check(u, arg)) {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Override key type with Level-1 only */
    arg->key.kt = key_type_get(u, arg->key.t.ipv, arg->key.t.len);

    l1_db_get(u, arg);

    /* Assume ltm will do lookup ahead as always */
    if (arg->pivot[LEVEL1] && arg->pivot[LEVEL1]->index != INVALID_INDEX) {
        /* Replace operation */
        int pkm;
        arg->index[LEVEL1] = arg->pivot[LEVEL1]->index;
        pkm = index_to_pkm(u, HW_INDEX(arg->index[LEVEL1]));
        if (pkm == PKM_128(u)) {
            rv = pair_insert(u, arg);
        } else {
            rv = unpair_insert(u, arg, pkm);
        }
        SHR_RETURN_VAL_EXIT(rv);
    }

    /* From now on, new add operation */
    switch (arg->key.kt) {
    case KEY_IPV6_128:
        rv = l1_insert_strict_ipv6_128(u, arg);
        break;
    case KEY_IPV6_64:
        rv = l1_insert_strict_ipv6_64(u, arg);
        break;
    case KEY_IPV6_32:
        rv = l1_insert_strict_ipv6_32(u, arg);
        break;
    case KEY_IPV4:
    default:
        rv = l1_insert_ipv4(u, arg);
        break;
    }

    if (rv == SHR_E_FULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_FULL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    /* SHR_IF_ERR_EXIT(pair_state_verify(u)); */

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_l1_delete(int u, alpm_arg_t *arg)
{
    int rv;
    SHR_FUNC_ENTER(u);

    l1_db_get(u, arg);

    if (arg->index[LEVEL1] == INVALID_INDEX) {
        alpm_pivot_t *pivot;
        SHR_IF_ERR_EXIT(
            l1_pvt_lookup(u, arg, &pivot));
        arg->index[LEVEL1] = pivot->index;
        arg->key.kt = key_type_get(u, arg->key.t.ipv, arg->key.t.len);
    }

    if (index_in_pair_tcam(u, HW_INDEX(arg->index[LEVEL1]))) {
        rv = pair_delete(u, arg);
    } else {
        rv = unpair_delete(u, arg);
    }
    SHR_IF_ERR_EXIT(rv);

    /* SHR_IF_ERR_EXIT(pair_state_verify(u)); */

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_l1_match(int u, alpm_arg_t *arg)
{
    int rv;
    alpm_pivot_t *pivot;
    SHR_FUNC_ENTER(u);

    rv = l1_pvt_lookup(u, arg, &pivot);
    if (SHR_SUCCESS(rv)) {
        alpm_info_t *info = bcmptm_rm_alpm_info_get(u);
        arg->pivot[LEVEL1] = pivot;
        arg->index[LEVEL1] = pivot->index;
        arg->key.kt = key_type_get(u, arg->key.t.ipv, arg->key.t.len);
        SHR_NULL_CHECK(info, SHR_E_PARAM);
        if (info->cmd_ctrl.req_flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) {
            if (index_in_pair_tcam(u, HW_INDEX(pivot->index))) {
                rv = pair_get(u, arg);
            } else {
                rv = unpair_get(u, arg);
            }
        } else {
            if (arg->set_key) {
                (void)bcmptm_rm_alpm_api_key_create(u, arg->key.t.ipv,
                                                    &arg->key.t.trie_ip,
                                                    arg->key.t.len,
                                                    arg->key.ip_addr);
                (void)bcmptm_rm_alpm_len_to_mask(u, arg->key.t.ipv,
                                                 arg->key.t.len,
                                                 &arg->key.ip_mask[0]);
            }
            sal_memcpy(&arg->ad, pivot->ad, sizeof(arg->ad));
        }
    }
    SHR_RETURN_VAL_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_l1_find(int u, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_l1_free_count(int u, alpm_key_type_t kt, alpm_vrf_type_t vt)
{
    int free_cnt = 0, i = 0;

    if (!DUAL_MODE(u) || vt == VRF_PRIVATE) {
        vt = VRF_PRIVATE;
        free_cnt = TOTAL_MAX(u, kt, vt);
        for (i = 0; i < KEY_TYPES; i++) {
            free_cnt -= (l1_used_count_map(u, i, kt, vt));
        }
    } else {
        vt = VRF_GLOBAL;
        free_cnt = TOTAL_MAX(u, kt, vt);
        for (i = 0; i < KEY_TYPES; i++) {
            free_cnt -= (l1_used_count_map(u, i, kt, vt));
        }

        vt = VRF_OVERRIDE;
        free_cnt += TOTAL_MAX(u, kt, vt);
        for (i = 0; i < KEY_TYPES; i++) {
            free_cnt -= (l1_used_count_map(u, i, kt, vt));
        }
    }

    return free_cnt;
}

void
bcmptm_rm_alpm_l1_cleanup(int u, bool warm)
{
    if (L1_HA(u)) {
        l1_ha_cleanup(u, warm);
    }

    if (L1_INFO(u)) {
        unpair_state_cleanup(u);
        unpair_stats_cleanup(u);

        pair_state_cleanup(u);
        pair_stats_cleanup(u);

        l1_info_cleanup(u);
    }
}

int
bcmptm_rm_alpm_l1_init(int u, bool recover)
{
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(l1_info_init(u, recover));

    SHR_IF_ERR_EXIT(unpair_state_init(u, recover));
    SHR_IF_ERR_EXIT(unpair_stats_init(u, false));

    SHR_IF_ERR_EXIT(pair_state_init(u, recover));
    SHR_IF_ERR_EXIT(pair_stats_init(u, false));

    SHR_IF_ERR_EXIT(l1_ha_init(u, recover));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_l1_recover(int u, bool full)
{
    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(full ? SHR_E_NONE: SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(unpair_recover(u, L1_DB0));
    SHR_IF_ERR_EXIT(pair_recover(u, L1_DB0));
    if (DUAL_MODE(u)) {
        SHR_IF_ERR_EXIT(unpair_recover(u, L1_DB1));
        SHR_IF_ERR_EXIT(pair_recover(u, L1_DB1));
    }
exit:
    SHR_FUNC_EXIT();
}

alpm_l1_info_t *
bcmptm_rm_alpm_l1_info_get(int u)
{
    return L1_INFO(u);
}

int
bcmptm_rm_alpm_arg_fill(int u, alpm_arg_t *arg, void *de, void *se, uint8_t l1v)
{
    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_alpm_l1_decode(u, arg, de, se, l1v));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_alpm_mask_to_len(u, arg->key.t.ipv,
                                    arg->key.ip_mask,
                                    &arg->key.t.len));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_alpm_trie_key_create(u, arg->key.t.ipv,
                                        arg->key.ip_addr,
                                        arg->key.t.len,
                                        &arg->key.t.trie_ip));
    arg->key.kt = key_type_get(u, arg->key.t.ipv, arg->key.t.len);
    arg->default_route = (arg->key.t.len == 0);
    arg->db = bcmptm_rm_alpm_db(u, arg->key.vt);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_l1_sid_hidx_get(int u, ln_index_t index, alpm_ip_ver_t ipv,
                               bcmdrd_sid_t *sid, uint32_t *hw_index)
{
    alpm_key_type_t kt = (ipv == IP_VER_4) ? KEY_IPV4 : KEY_IPV6_128;

    SHR_FUNC_ENTER(u);
    *hw_index = HW_INDEX(index);
    if (index_in_pair_tcam(u, *hw_index)) {
        *sid = pair_sid(u, kt);
    } else {
        *sid = unpair_sid(u, kt);
    }
    if (is_pair_sid(u, *sid)) {
        *hw_index = pair_index_encode(u, *hw_index);
    }
    SHR_FUNC_EXIT();
}

