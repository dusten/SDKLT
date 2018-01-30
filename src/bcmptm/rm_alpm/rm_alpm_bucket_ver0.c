/*! \file rm_alpm_bucket_ver0.c
 *
 * Bucket Manager for ALPM Level-N bucket
 *
 * This file contains the bucket manager for ALPM Level-N bucket
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
#include <shr/shr_util.h>

#include "rm_alpm_common.h"
#include "rm_alpm_util.h"
#include "rm_alpm_ts.h"
#include "rm_alpm_device.h"
#include "rm_alpm_bucket.h"
#include "rm_alpm_leveln.h"
#include "rm_alpm_level1.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

/* Max 9 base buckets to shuffle into a free group */
#define MAX_SHUFFLE_NUM             (8 + 1)
#define RBKT_MAX_ENTRIES_VER0       6
#define BASE_ENTRIES_ALLOC_CHUNK_SIZE  2

#define RBKT_EBMP(u, _d, _l, _i)        (RBKT(u, _d, _l, _i).entry_bitmap)
#define RBKT_VBANKS(u, _d, _l, _i)          (RBKT(u, _d, _l, _i).valid_banks)
#define RBKT_FBANKS(u, _d, _l, _i)          (RBKT(u, _d, _l, _i).free_banks)
#define RBKT_FORMAT(u, _d, _l, _i)          (RBKT(u, _d, _l, _i).format)
#define RBKT_IS_HEAD(u, _d, _l, _i)         (RBKT(u, _d, _l, _i).is_head)
#define RBKT_SUB_BKTS(u, _d, _l, _i)        (RBKT(u, _d, _l, _i).sub_bkts)
#define RBKT_ECOUNT(u, _d, _l, _i)          (RBKT(u, _d, _l, _i).count)
#define VER0_RBKT_PIVOT(u, _d, _l, _i, _s)  (RBKT(u, _d, _l, _i).pivots[_s])

#define VER0_SUB_BKT_SHIFT          (3)
#define VER0_SUB_BKT_MASK           (7) /* ((1U << VER0_SUB_BKT_SHIFT) - 1) */
#define VER0_GROUP(u, _l)           ((_l) >> VER0_SUB_BKT_SHIFT)
#define VER0_SUB_BKT(u, _l)         ((_l) & VER0_SUB_BKT_MASK)
#define VER0_LOG_BKT(_p, _s)        (((_p) << VER0_SUB_BKT_SHIFT) | \
                                     ((_s) & VER0_SUB_BKT_MASK))

#define RBKT_SUB_BKT_SET(u, _d, _l, _p, _s)   \
    (RBKT_SUB_BKTS(u, _d, _l, _p) |= (1U << (_s)))
#define RBKT_SUB_BKT_CLR(u, _d, _l, _p, _s)   \
    (RBKT_SUB_BKTS(u, _d, _l, _p) &= ~(1U << (_s)))
#define RBKT_SUB_BKT_IS_SET(u, _d, _l, _p, _s)  \
    (RBKT_SUB_BKTS(u, _d, _l, _p) & (1U << (_s)))

#define VER0_CAN_BUCKET_SHARE(u, _d, _l, _g1, _g2)      \
    (fast_popcount4[RBKT_SUB_BKTS(u, _d, _l, _g1)] +    \
     fast_popcount4[RBKT_SUB_BKTS(u, _d, _l, _g2)] <=   \
     MAX_SUB_BKTS)

#define MAX_FORMAT(u, _p)             (smax_format[u][_p])

/*******************************************************************************
 * Typedefs
 */

typedef struct base_entries_ver0_s {
    LN_ENTRY_t entry[ALPM_MAX_BANKS][RBKT_MAX_ENTRIES_VER0];
    /*!< Pointer to the next element in the list (or NULL). */
    struct base_entries_ver0_s *next;
} base_entries_ver0_t;

/*******************************************************************************
 * Private variables
 */

static bkt_hdl_t *sbkt_hdl[BCMDRD_CONFIG_MAX_UNITS][DBS][LEVELS];
static int bucket_shifts[BCMDRD_CONFIG_MAX_UNITS];
static uint8_t bucket_scale[BCMDRD_CONFIG_MAX_UNITS];
/*!< Max format value  */
static int smax_format[BCMDRD_CONFIG_MAX_UNITS][FROMAT_TYPES];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS];
static format_type_t format_type = FORMAT_ROUTE;
static uint8_t ver0_bank_bits[BCMDRD_CONFIG_MAX_UNITS];
static shr_lmm_hdl_t base_entries_lmem = NULL;

/*******************************************************************************
 * Private Functions (declaration)
 */

static uint8_t ver0_base_entry_count(int u, int db, int ln, uint8_t format_value);

/*******************************************************************************
 * Private Functions (definition)
 */

static uint32_t
ver0_hw_idx_from_rbkt_idx(int u, int db, int ln, rbkt_idx_t rbkt)
{
    int max_banks;
    max_banks = BKT_HDL_MAX_BANKS(u, db, ln);
    if (max_banks == 4) {
        return rbkt;
    } else {
        return ((rbkt % max_banks) | ((rbkt / max_banks) << 2));
    }
}

static uint32_t
ver0_hw_eidx_from_ent_idx(int u, int db, int ln, ln_index_t ent_idx)
{
    uint8_t bkt_bank_bits;
    uint8_t ent_ofs, bank;
    uint32_t phy_bkt;

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    phy_bkt = RBKT_FROM_INDEX(ent_idx) / BKT_HDL_MAX_BANKS(u, db, ln);
    bank = RBKT_FROM_INDEX(ent_idx) % BKT_HDL_MAX_BANKS(u, db, ln);
    bkt_bank_bits = BKT_HDL_BUCKET_BITS(u, db, LEVEL2) + ver0_bank_bits[u];
    return (ent_ofs << bkt_bank_bits) | (phy_bkt << ver0_bank_bits[u]) | bank;
}

static int
ver0_bkt_hdl_get(int u, int db, int ln, bkt_hdl_t **bkt_hdl)
{
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(bkt_hdl);
    ALPM_ASSERT(db < DBS);
    ALPM_ASSERT(ln < LEVELS);
    *bkt_hdl = BKT_HDL(u, db, ln);
    SHR_NULL_CHECK(*bkt_hdl, SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

static inline int
ver0_sub_bucket_alloc(int u, int db, int ln, rbkt_group_t group,
                      uint8_t *sub_bkt)
{
    uint8_t i;
    ALPM_ASSERT(RBKT_SUB_BKTS(u, db, ln, group) != 0xF);
    for (i = 0; i < MAX_SUB_BKTS; i++) {
        if (!RBKT_SUB_BKT_IS_SET(u, db, ln, group, i)) {
            RBKT_SUB_BKT_SET(u, db, ln, group, i);
            *sub_bkt = i;
            return SHR_E_NONE;
        }
    }
    return SHR_E_INTERNAL;
}

static inline void
ver0_sub_bucket_free(int u, int db, int ln, rbkt_group_t group, uint8_t sub_bkt)
{
    RBKT_SUB_BKT_CLR(u, db, ln, group, sub_bkt);
}

static inline void
ver0_bucket_pivot_set(int u, int db, int ln, rbkt_group_t group,
                      uint8_t sub_bkt, alpm_pivot_t *pivot)
{
    VER0_RBKT_PIVOT(u, db, ln, group, sub_bkt) = pivot;
    /* Assume bucket alloc before pivot set */
    ALPM_ASSERT(RBKT_SUB_BKT_IS_SET(u, db, ln, group, sub_bkt));
}

static inline void
ver0_bucket_pivot_clr(int u, int db, int ln, rbkt_group_t group,
                      uint8_t sub_bkt)
{
    /* Assume bucket freed after pivot clr */
    ALPM_ASSERT(RBKT_SUB_BKT_IS_SET(u, db, ln, group, sub_bkt));
    VER0_RBKT_PIVOT(u, db, ln, group, sub_bkt) = NULL;
    ver0_sub_bucket_free(u, db, ln, group, sub_bkt);
}

static alpm_pivot_t *
ver0_bucket_pivot_get(int u, int db, int ln, rbkt_group_t group,
                      uint8_t sub_bkt)
{
    ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, group));
    return VER0_RBKT_PIVOT(u, db, ln, group, sub_bkt);
}

/*!
 * \brief Calculate the max length for new split point.
 *
 * \param [in] u Device u.
 * \param [in] pivot Pivot to be split.
 *
 * \return Max split length
 */
static inline uint32_t
ver0_trie_msl(int u, alpm_ip_ver_t ipv)
{
    return (ipv == IP_VER_4) ? 31 :
           (bcmptm_rm_alpm_pkm128_is_set(u)) ? 127 : 63;
}

/*!
 * \brief Calculate the max count for new split point.
 *
 * \param [in] u Device u.
 * \param [in] pivot Pivot to be split.
 *
 * \return Max split count
 */
static inline uint32_t
ver0_trie_msc(int u, int db, int ln, uint8_t fmt, rbkt_group_t group)
{
    return ver0_base_entry_count(u, db, ln, fmt) -
           RBKT_ECOUNT(u, db, ln, group);
}

static void
ver0_rbkt_group_sanity(int u, int db, int ln, rbkt_group_t group)
{
    rbkt_t *rbkt_group, *rbkt;
    bkt_hdl_t *bkt_hdl;
    shr_dq_t *blist, *elem;
    alpm_bkt_elem_t *curr;
    int bkt_count[ALPM_MAX_BANKS] = {0};
    rbkt_idx_t rbkt_idx;
    int num_entry, i;
    uint8_t vbanks, sub_bkt;
    int count = 0, count2 = 0;
    alpm_pivot_t *pivots[MAX_SUB_BKTS] = {NULL};

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }
    ver0_bkt_hdl_get(u, db, ln, &bkt_hdl);
    if (group == INVALID_GROUP) {
        return;
    }
    rbkt_group = &bkt_hdl->rbkt_array[group];
    assert(rbkt_group->valid_banks == bkt_hdl->max_banks * bucket_scale[u]);
    vbanks = rbkt_group->valid_banks;

    for (i = 0; i < vbanks; i++) {
        rbkt = rbkt_group + i;
        if (i == 0) {
            assert(rbkt->is_head);
            for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                pivots[sub_bkt] = rbkt->pivots[sub_bkt];
                if (rbkt->sub_bkts & (1 << sub_bkt)) {
                    assert(pivots[sub_bkt] && pivots[sub_bkt]->bkt);
                    assert(VER0_GROUP(u, pivots[sub_bkt]->bkt->log_bkt) ==
                           rbkt_group->index);
                    if (rbkt->format == VER0_FORMAT_1 ||
                        rbkt->format == VER0_FORMAT_3) {
                        assert(pivots[sub_bkt]->key.t.ipv == IP_VER_4);
                    } else {
                        if (rbkt->format == VER0_FORMAT_5) {
                            assert(bcmptm_rm_alpm_pkm128_is_set(u));
                        } else {
                            assert(!bcmptm_rm_alpm_pkm128_is_set(u));
                        }
                        assert(pivots[sub_bkt]->key.t.ipv == IP_VER_6);
                    }
                } else {
                    assert(!pivots[sub_bkt]);
                }
            }
        } else {
            assert(!rbkt->is_head);
        }
        count += shr_util_popcount(rbkt->entry_bitmap);
    }

    for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
        if (!pivots[sub_bkt]) {
            continue;
        }
        count2 += bcmptm_rm_alpm_trie_count(pivots[sub_bkt]->bkt->trie);
        blist = &pivots[sub_bkt]->bkt->list;
        SHR_DQ_TRAVERSE(blist, elem)
            curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            if (curr->index == INVALID_INDEX) {
                continue;
            }
            rbkt_idx = RBKT_FROM_INDEX(curr->index);
            bkt_count[rbkt_idx - group]++;
        SHR_DQ_TRAVERSE_END(blist, elem);
    }
    assert(count == count2);

    for (i = 0; i < rbkt_group->valid_banks; i++) {
        rbkt = &bkt_hdl->rbkt_array[group + i];
        bcmptm_rm_alpm_ln_base_entry_info(u, format_type, rbkt->format,
                                          NULL, NULL, &num_entry);
        assert(bkt_count[i] <= num_entry);
    }
}

/* Update bucket usage bookeeping info */
static void
ver0_rbkt_group_free_count_update(int u, int db, int ln, rbkt_group_t group,
                                  int incr)
{
    int count;
    uint8_t fmt;
    static uint32_t update_count = 0;

    if (incr == 0) {
        return;
    }
    count = RBKT_ECOUNT(u, db, ln, group);
    ALPM_ASSERT(count >= 0);
    fmt = RBKT_FORMAT(u, db, ln, group);
    ALPM_ASSERT(fmt != DISABLED_FORMAT);
    if (!SHR_DQ_NULL(&RBKT_FMT_USG_NODE(u, db, ln, group))) {
        SHR_DQ_REMOVE(&RBKT_FMT_USG_NODE(u, db, ln, group));
    } else {
        ALPM_ASSERT(count == 0);
    }

    RBKT_ECOUNT(u, db, ln, group) += incr;
    update_count++;

    count = RBKT_ECOUNT(u, db, ln, group);
    if (RBKT_SUB_BKTS(u, db, ln, group) == 0xF) { /* For performance */
        ALPM_ASSERT(&FMT_GROUP_USAGE_LIST(u, db, ln, fmt, 0) !=
                    &RBKT_FMT_USG_NODE(u, db, ln, group));
        SHR_DQ_INSERT_HEAD(&FMT_GROUP_USAGE_LIST(u, db, ln, fmt, 0),
                           &RBKT_FMT_USG_NODE(u, db, ln, group));
    } else {
        ALPM_ASSERT(&FMT_GROUP_USAGE_LIST(u, db, ln, fmt, count) !=
                    &RBKT_FMT_USG_NODE(u, db, ln, group));
        SHR_DQ_INSERT_HEAD(&FMT_GROUP_USAGE_LIST(u, db, ln, fmt, count),
                           &RBKT_FMT_USG_NODE(u, db, ln, group));
    }
    ALPM_ASSERT(!SHR_DQ_NULL(&RBKT_FMT_USG_NODE(u, db, ln, group)));
}

/*!
 * \brief Update the free bank for an existing group.
 *
 * Also, update the postion in the usage list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Target group
 * \param [in] diff Number diff of free banks, negative if shrink else postive.
 *
 * \return nothing.
 */
static void
ver0_rbkt_group_free_bank_update(int u, int db, int ln, rbkt_group_t group,
                                 int diff)
{
    int free_count;

    RBKT_FBANKS(u, db, ln, group) += diff;
    free_count = RBKT_FBANKS(u, db, ln, group);
    ALPM_ASSERT(free_count >= 0);
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, db, ln, group));
    free_count = ALPM_MIN(bucket_scale[u],
                          free_count / BKT_HDL_MAX_BANKS(u, db, ln));
    ALPM_ASSERT(free_count < GROUP_USAGE_CNT(u, db, ln));
    SHR_DQ_INSERT_HEAD(&GROUP_USAGE_LIST(u, db, ln, free_count),
                       &RBKT_USG_NODE(u, db, ln, group));
}

/*!
 * \brief Set free bank for a newly create existing group.
 *
 * Also, add into the usage list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Target group
 * \param [in] free_count Number of free banks.
 *
 * \return nothing.
 */
static void
ver0_rbkt_group_free_bank_set(int u, int db, int ln, rbkt_group_t group,
                              uint32_t free_count)
{
    RBKT_FBANKS(u, db, ln, group) = free_count;
    free_count = ALPM_MIN(bucket_scale[u],
                          free_count / BKT_HDL_MAX_BANKS(u, db, ln));
    ALPM_ASSERT(free_count < GROUP_USAGE_CNT(u, db, ln));
    SHR_DQ_INSERT_HEAD(&GROUP_USAGE_LIST(u, db, ln, free_count),
                       &RBKT_USG_NODE(u, db, ln, group));
}

/*!
 * \brief Reset free banks for an existing group.
 *
 * Also, delete from the usage list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Target group
 *
 * \return nothing.
 */
static void
ver0_rbkt_group_free_bank_clr(int u, int db, int ln, rbkt_group_t group)
{
    RBKT_FBANKS(u, db, ln, group) = 0;
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, db, ln, group));
    SHR_DQ_REMOVE(&RBKT_FMT_USG_NODE(u, db, ln, group));
}

/*!
 * \brief Free an existing group.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to be freed
 *
 * \return SHR_E_NONE only.
 */
static int
ver0_rbkt_group_free(int u, int db, int ln, rbkt_group_t group)
{
    rbkt_t *prev;
    shr_dq_t *dq;
    int i;
    int prev_group;

    SHR_FUNC_ENTER(u);
    if (group == INVALID_GROUP) { /* Invalid group does not exist */
        SHR_EXIT();
    }

    ALPM_ASSERT(RBKT_VBANKS(u, db, ln, group) != 0);
    if (group == 0) { /* Don't fuse group */
#ifdef ALPM_DEBUG
        do {
            uint8_t sub_bkt;
            for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                ALPM_ASSERT(VER0_RBKT_PIVOT(u, db, ln, group, sub_bkt) == NULL);
            }
        } while (0);
#endif
    } else {
        /* Cannot fuse for the first group */
        ALPM_ASSERT(group != 0 || group != INVALID_GROUP);

        dq = SHR_DQ_PREV(&RBKT_LIST_NODE(u, db, ln, group), shr_dq_t*);
        prev = SHR_DQ_ELEMENT_GET(rbkt_t*, dq, listnode);
        prev_group = prev->index;

        ver0_rbkt_group_free_bank_update(u, db, ln, prev_group,
                                   RBKT_FBANKS(u, db, ln, group) +
                                   RBKT_VBANKS(u, db, ln, group));

        for (i = 1; i < RBKT_VBANKS(u, db, ln, group); i++) {
            rbkt_idx_t rbkt_idx = group + i;
            RBKT_FORMAT(u, db, ln, rbkt_idx) = DISABLED_FORMAT;
            RBKT_EBMP(u, db, ln, rbkt_idx) = 0;
        }
        RBKT_VBANKS(u, db, ln, group) = 0;
        ver0_rbkt_group_free_bank_clr(u, db, ln, group);
        RBKT_IS_HEAD(u, db, ln, group) = BODY;
#ifdef ALPM_DEBUG
        do {
            uint8_t sub_bkt;
            for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                ALPM_ASSERT(VER0_RBKT_PIVOT(u, db, ln, group, sub_bkt) == NULL);
            }
        } while (0);
#endif
        SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, db, ln, group));
        BKT_HDL_GROUPS_USED(u, db, ln)--;
    }
exit:
    SHR_FUNC_EXIT();
}

static inline bool
ver0_rbkt_group_splitable(int u, int db, int ln, rbkt_t *free_group,
                          uint8_t size, rbkt_idx_t *new_group,
                          uint16_t *take_count)
{
    uint8_t max_banks = BKT_HDL_MAX_BANKS(u, db, ln);
    int next_group_begin;


    next_group_begin = free_group->index + free_group->valid_banks +
                       free_group->free_banks;
    /*
     * When ALPM_DEBUG, we can make this to be a index allocation for mixed case
     * with different size buckets.
     * Currently, this mixed case does not hold, and all buckets are with
     * same size, all scale equal to 1, or 2. Therefore whenere a free group
     * is found, it is always splitable.
     * next_group_begin = (next_group_begin / max_banks) * max_banks;
     */
    ALPM_ASSERT(next_group_begin == (next_group_begin / max_banks) * max_banks);
    *new_group = next_group_begin - (size * max_banks);
    ALPM_ASSERT(*new_group >= free_group->valid_banks + free_group->index);
    *take_count = next_group_begin - *new_group;
    return *new_group; /* False if group 0 */
}


/*!
 * \brief Allocate a new group.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] size Requesting size for new group
 * \param [bool] shuffle Shuffle groups to create new
 * \param [out] new_group New group allocated
 *
 * \return SHR_E_NONE only.
 */
static int
ver0_rbkt_group_alloc(int u, int db,
                      int ln, uint8_t fmt,
                      uint8_t size,
                      bool shuffle,
                      rbkt_group_t *new_group)
{
    uint8_t free_count, max_banks;
    uint16_t take_count;
    shr_dq_t *groups_ptr;
    shr_dq_t *elem = NULL;
    rbkt_t *free_rbkt, *new_rbkt;
    uint8_t i;

    SHR_FUNC_ENTER(u);

    if (size == 0) {
        SHR_RETURN(SHR_E_NONE);
    }
    /*
     * Traverse the FMT_GROUP_USAGE array starting from max free count.
     * Each elem in the array has a list attached. Each list connects all
     * groups with same number of free base buckets. The subscript of the
     * elem equals to the count of free base buckets.
     * The max free count equals to the max banks available for the given
     * db & ln, if any of the new_group has free base buckets more than max
     * banks,
     * it will join those groups with just max banks of free buckets.
     * This is because, a new_group can only have up to max banks of free
     * buckets,
     * any free count beyond that does not make it 'more free'.
     */
    max_banks = BKT_HDL_MAX_BANKS(u, db, ln);
    for (free_count = bucket_scale[u]; free_count >= size; free_count--) {
        groups_ptr = &GROUP_USAGE_LIST(u, db, ln, free_count);
        if (!SHR_DQ_EMPTY(groups_ptr)) {
            /* Stop at the first new_group */
            elem = SHR_DQ_HEAD(groups_ptr, shr_dq_t*);
            free_rbkt = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
            if (!ver0_rbkt_group_splitable
                    (u, db, ln, free_rbkt, size, new_group, &take_count)) {
                    continue;
            }

            /* If no valid banks, must be group[0].
             * Update: not true any more, as we may have pivot with assigned group with
             * all free rbkts. This is the case when we cannot free that pivot (usually
             * level-2), because otherwise we may endup with ill insert (find wrong
             * second level pivot).
             */
            /* ALPM_ASSERT(free_rbkt->valid_banks || free_rbkt->index == 0); */
            /* Setup split group */
            new_rbkt = &RBKT(u, db, ln, *new_group);
            if (new_rbkt->index != free_rbkt->index) {
                new_rbkt->is_head = HEAD;
                size *= max_banks; /* Scale to bank size */
                ALPM_ASSERT(take_count == size);
                new_rbkt->valid_banks = size;
                SHR_DQ_ELEM_INIT(&RBKT_FMT_USG_NODE(u, db, ln, new_rbkt->index));
                ver0_rbkt_group_free_bank_set(u, db, ln, new_rbkt->index,
                                              take_count - size);
                /* Setup original group */
                ver0_rbkt_group_free_bank_update(u, db, ln, free_rbkt->index,
                                                 -take_count);
                /* Link the new and free2 */
                SHR_DQ_INSERT_NEXT(&free_rbkt->listnode,
                                   &new_rbkt->listnode);
                BKT_HDL_GROUPS_USED(u, db, ln)++;

                
                for (i = 0; i < size; i++, new_rbkt++) {
                    new_rbkt->format = fmt;
                }
            } else {
                /* Else take all, and this group is already in the list */
                alpm_pivot_t *pivot;
                uint8_t sub_bkt;
                for (sub_bkt = 0; sub_bkt < MAX_SUB_BKTS; sub_bkt++) {
                    pivot = ver0_bucket_pivot_get(u, db, ln, free_rbkt->index,
                                                  sub_bkt);
                    if (pivot) {
                        /* This group is now officially taken away */
                        pivot->bkt->log_bkt = INVALID_LOG_BKT;
                    }
                }
            }
            SHR_EXIT();
        }
    }

    SHR_RETURN(SHR_E_FULL);
exit:
    SHR_FUNC_EXIT();
}

static int
ver0_rbkt_recover(int u, int db, int ln, rbkt_idx_t rbkt_idx,
                  uint8_t pivot_len,
                  LN_ENTRY_t *base_entry,
                  uint8_t *entry_count,
                  alpm_arg_t *arg,
                  uint8_t sub_bkt)
{
    int num_entries, i;
    uint8_t format = RBKT_FORMAT(u, db, ln, rbkt_idx);

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                          format,
                                          NULL,
                                          NULL,
                                          &num_entries));
    for (i = 0; i < num_entries; i++) {
        alpm_arg_t entry_arg = *arg;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, format_type,
                                                format,
                                                pivot_len,
                                                &entry_arg,
                                                base_entry[i]));
        /* No need to do.
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_alpm_mask_to_len(u, arg->key.t.ipv,
                                        arg->key.ip_mask,
                                        &arg->key.t.len));
                                        */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_key_create(u, entry_arg.key.t.ipv,
                                           entry_arg.key.ip_addr,
                                           entry_arg.key.t.len,
                                           &entry_arg.key.t.trie_ip));

        if (entry_arg.valid && sub_bkt == entry_arg.a1.sub_bkt) {
            ALPM_BIT_SET(RBKT_EBMP(u, db, ln, rbkt_idx), i);
            entry_arg.index[ln] =
                ALPM_INDEX_ENCODE_VER0(rbkt_idx, entry_arg.a1.sub_bkt, i);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_pivot_recover(u, ln, &entry_arg));
            (*entry_count)++;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
ver0_log_bucket_recover(int u, int db, int ln,
                        rbkt_group_t group, uint8_t sub_bkt,
                        alpm_arg_t *arg)
{
    LN_ENTRY_t *base_entry;
    LN_ENTRY_t rbkt_buf[ALPM_MAX_BANKS];
    base_entries_ver0_t *base_entries;
    uint8_t pivot_len, i, entry_count, max_banks;
    alpm_pivot_t *pivot;

    SHR_FUNC_ENTER(u);
    base_entries = shr_lmm_alloc(base_entries_lmem);

    RBKT_IS_HEAD(u, db, ln, group) = true;
    pivot = arg->pivot[PREV_LEVEL(ln)];
    ALPM_ASSERT(pivot);
#if 0
    for (i = 0; i < MAX_SUB_BKTS; i++) {
        if (ver0_bucket_pivot_get(u, db, ln, group, i)) {
            /*
             * Performance boost approach.
             * This group is traversed by another pivot,
             * which means everything about the sub bucket is already recovered
             * except for its pivot pointer.
             */
            RBKT_SUB_BKT_SET(u, db, ln, group, sub_bkt);
            ver0_bucket_pivot_set(u, db, ln, group, sub_bkt, pivot);
            arg->log_bkt[PREV_LEVEL(ln)] = VER0_LOG_BKT(group, sub_bkt);
            SHR_EXIT();
        }
    }
#endif
    RBKT_SUB_BKT_SET(u, db, ln, group, sub_bkt);
    ALPM_ASSERT(!ver0_bucket_pivot_get(u, db, ln, group, sub_bkt));
    ver0_bucket_pivot_set(u, db, ln, group, sub_bkt, pivot);
    arg->log_bkt[PREV_LEVEL(ln)] = VER0_LOG_BKT(group, sub_bkt);
    pivot_len = pivot->key.t.len;
    entry_count = 0;
    max_banks = bucket_scale[u] * BKT_HDL_MAX_BANKS(u, db, ln);
    for (i = 0; i < max_banks; i++) {
        uint32_t hw_idx;
        rbkt_idx_t rbkt_idx;
        hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, group + i);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[i][0]));

        base_entry = &base_entries->entry[i][0];
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_raw_bucket_decode(u, format_type,
                                                arg->fmt,
                                                base_entry,
                                                RBKT_MAX_ENTRIES_VER0,
                                                &rbkt_buf[i][0]));
        rbkt_idx = group + i;
        RBKT_FORMAT(u, db, ln, rbkt_idx) = arg->fmt;
        RBKT_VBANKS(u, db, ln, group) = max_banks;
        ALPM_ASSERT(RBKT_VBANKS(u, db, ln, group) <= max_banks);
        SHR_IF_ERR_EXIT(
            ver0_rbkt_recover(u, db, ln, rbkt_idx, pivot_len, base_entry,
                              &entry_count, arg, sub_bkt));
    }
    if (entry_count) {
        if (RBKT_ECOUNT(u, db, ln, group) == 0) {
            SHR_DQ_INIT(&RBKT_FMT_USG_NODE(u, db, ln, group));
        }
        ver0_rbkt_group_free_count_update(u, db, ln, group, entry_count);
    }

exit:
    shr_lmm_free(base_entries_lmem, base_entries);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback to group bkt elems based on IP Length.
 *
 * \param [in] node Trie node
 * \param [in] user_data User data
 *
 * \return SHR_E_NONE only.
 */
static int
ver0_bkt_elems_connect_cb(rm_alpm_trie_node_t *node, void *user_data)
{
    bucket_traverse_data_t *trvs_data;
    trvs_data = (bucket_traverse_data_t *) user_data;
    if (node->type == PAYLOAD) {
        alpm_bkt_elem_t *bkt_elem;
        bkt_elem = TRIE_ELEMENT_GET(alpm_bkt_elem_t*, node, bktnode);
        if (SHR_DQ_HEAD(&trvs_data->dq[0], void *) == NULL) {
            SHR_DQ_INIT(&trvs_data->dq[0]);
        }
        if (bkt_elem->index == INVALID_INDEX) {
            trvs_data->found_new = true;
            SHR_DQ_INSERT_TAIL(&trvs_data->dq[0],
                               &bkt_elem->dqnode);
        } else {
            SHR_DQ_INSERT_HEAD(&trvs_data->dq[0],
                               &bkt_elem->dqnode);
        }

        trvs_data->count++;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Connect bkt elems to bucket_list with IP length group sorted.
 *
 * \param [in] u Device u.
 * \param [in] list Bucket element List
 * \param [in] trie Bucket trie
 *
 * \return SHR_E_NONE.
 */
static int
ver0_bkt_elems_connect(int u, shr_dq_t *list, rm_alpm_trie_t *trie,
                       bool *found_new)
{
    bucket_traverse_data_t trvs_data;
    SHR_FUNC_ENTER(u);
    sal_memset(&trvs_data, 0, sizeof(trvs_data));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_traverse(trie,
                                     ver0_bkt_elems_connect_cb,
                                     &trvs_data,
                                     TRAVERSE_BFS));
    SHR_DQ_INIT(list);
    
    SHR_DQ_JOIN_TAIL(list, &trvs_data.dq[0]);
    *found_new = trvs_data.found_new;
    ALPM_ASSERT((trie->trie && (trvs_data.count == trie->trie->count)) ||
                (trie->trie == NULL && trvs_data.count == 0));
exit:
    SHR_FUNC_EXIT();
}

/* Get entry No. per bucket based on various setting */
/*!
 * \brief Get base entry count per group based on various setting.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] format_value Format value for base entry
 *
 * \return Base entry count per group
 */
static uint8_t
ver0_base_entry_count(int u, int db, int ln, uint8_t format_value)
{
    uint8_t bkt_ent_max = 0;
    int prefix_len;
    int full;
    int number;

    /* Base entry number */
    full = bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                           format_value,
                                           &prefix_len,
                                           &full,
                                           &number);
    ALPM_ASSERT(full == SHR_E_NONE);

    bkt_ent_max = number * BKT_HDL_MAX_BANKS(u, db, ln);
    bkt_ent_max *= bucket_scale[u];
    if (bcmptm_rm_alpm_mode(u) == ALPM_MODE_PARALLEL &&
        bcmptm_rm_alpm_is_urpf(u)) {
        bkt_ent_max >>= 1;
    }
    return bkt_ent_max;
}

/*!
 * \brief Free used entries' slots for given group.
 *
 * Do not rearrange group resource, once done rbkts holes may exist.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to alloc base entry
 * \param [in] ent_idx Entry index array
 * \param [in] count Number of array elements
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_base_entries_free(int u, int db, int ln,
                       rbkt_group_t group,
                       ln_index_t *ent_idx,
                       int count)
{
    rbkt_idx_t remove_bkt;
    rbkt_t *rbkt;
    int i;

    SHR_FUNC_ENTER(u);
    for (i = 0; i < count; i++) {
        if (ent_idx[i] == INVALID_INDEX) {
            continue;
        }
        remove_bkt = RBKT_FROM_INDEX(ent_idx[i]);
        rbkt = &RBKT(u, db, ln, remove_bkt);

        ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap,
                                 ENT_OFS_FROM_IDX(ent_idx[i])));
        ALPM_BIT_CLR(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx[i]));
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a used entry slot for given group.
 *
 * Rearrange group resource if necessary, as to keep rbkts contiguous.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to alloc base entry
 * \param [in] ent_idx Entry index
 * \param [out] format_update True if update pivot format, eles false.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_base_entry_free(int u, int db, int ln,
                     rbkt_group_t group,
                     ln_index_t ent_idx,
                     bool *format_update)
{
    rbkt_idx_t remove_bkt;
    rbkt_t *rbkt;

    SHR_FUNC_ENTER(u);
    remove_bkt = RBKT_FROM_INDEX(ent_idx);
    rbkt = &RBKT(u, db, ln, remove_bkt);

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx)));
    ALPM_BIT_CLR(rbkt->entry_bitmap, ENT_OFS_FROM_IDX(ent_idx));

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a free entry slot for given group.
 *
 * Assign resources that group currently have. Does not try
 * to expand the group resource.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to alloc base entry
 * \param [in] entry_len IP length of requesting entry.
 * \param [in] entry_full Data type (full/reduced) of requesting entry.
 * \param [out] ent_idx Entry index
 * \param [out] format_update True if update pivot format, eles false.
 * \param [out] format_value Format value for this base entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_base_entry_alloc(int u, int db, int ln,
                      rbkt_group_t group,
                      uint8_t sub_bkt,
                      int entry_len,
                      int entry_full,
                      ln_index_t *ent_idx,
                      bool *format_update,
                      int *format_value)
{
    int i;
    int min_format_value = INVALID_FORMAT;
    int prefix_len;
    int full;
    int number;
    rbkt_idx_t rbkt_idx = 0;
    SHR_FUNC_ENTER(u);

    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        rbkt_t *rbkt = &RBKT(u, db, ln, group + i);
        /* All rbkts' formats should be same as head */
        ALPM_ASSERT(rbkt->format != DISABLED_FORMAT);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                              rbkt->format,
                                              &prefix_len,
                                              &full,
                                              &number));
        if (rbkt->entry_bitmap != (uint32_t)((1 << number) - 1) &&
            full >= entry_full) {
            min_format_value = rbkt->format;
            rbkt_idx = group + i;
            break;
        }
    }

    if (min_format_value != INVALID_FORMAT) {/* Free entry in valid banks */
        for (i = 0; i < number; i++) {
            if (!ALPM_BIT_GET(RBKT_EBMP(u, db, ln, rbkt_idx), i)) {
                *ent_idx = ALPM_INDEX_ENCODE_VER0(rbkt_idx, sub_bkt, i);
                if (format_update) {
                    *format_update = FALSE;
                }
                ALPM_BIT_SET(RBKT_EBMP(u, db, ln, rbkt_idx), i);
                *format_value = min_format_value;
                SHR_EXIT();
            }
        }
        ALPM_ASSERT(0); /* Should exit already */
    }

    ALPM_ASSERT(i == RBKT_VBANKS(u, db, ln, group));
    SHR_RETURN(SHR_E_FULL);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear a batch of base entries in HW.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group of deleting entries
 * \param [in] ent_idx Pointer to entry index array
 * \param [in] count Number of entry indexes
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_base_entries_clear(int u, int db, int ln,
                        rbkt_group_t group,
                        ln_index_t *ent_idx,
                        int count)
{
    int i, j, ent_ofs;
    LN_ENTRY_t rbkt_buf[ALPM_MAX_BANKS];
    base_entries_ver0_t *base_entries;
    LN_ENTRY_t *base_entry;
    rbkt_idx_t rbkt_idx[ALPM_MAX_BANKS];
    rbkt_t *rbkt[ALPM_MAX_BANKS] = {NULL};
    int hw_idx;
    SHR_FUNC_ENTER(u);

    sal_memset(rbkt_idx, -1, sizeof(rbkt_idx));
    base_entries = shr_lmm_alloc(base_entries_lmem);

    /* Flag those rbkts with deleting entries */
    for (i = 0; i < count; i++) {
        if (ent_idx[i] == INVALID_INDEX) {
            continue;
        }
        j = RBKT_FROM_INDEX(ent_idx[i]) - group;
        if (rbkt_idx[j] == -1) {
            rbkt_idx[j] = RBKT_FROM_INDEX(ent_idx[i]);
            ALPM_ASSERT(rbkt_idx[j] != -1);
        }
    }

    /* Read rbkts and decode each to a base_entries array */
    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        if (rbkt_idx[i] != -1) {
            hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[i][0]));

            rbkt[i] = &RBKT(u, db, ln, rbkt_idx[i]);
            base_entry = &base_entries->entry[i][0];
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_raw_bucket_decode(u, format_type,
                                                    rbkt[i]->format,
                                                    base_entry,
                                                    RBKT_MAX_ENTRIES_VER0,
                                                    &rbkt_buf[i][0]));
        }
    }

    /* Reset base_entry based on given ent_idx array */
    for (i = 0; i < count; i++) {
        if (ent_idx[i] == INVALID_INDEX) {
            continue;
        }
        j = RBKT_FROM_INDEX(ent_idx[i]) - group;
        ent_ofs = ENT_OFS_FROM_IDX(ent_idx[i]);
        base_entry = &base_entries->entry[j][ent_ofs];
        sal_memset(base_entry, 0, sizeof(*base_entry));
    }

    /* Encode base_entries array to each rbkt, and then write all */
    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        if (rbkt_idx[i] != -1) {
            base_entry = &base_entries->entry[i][0];
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_raw_bucket_encode(u, format_type,
                                                    rbkt[i]->format,
                                                    base_entry,
                                                    RBKT_MAX_ENTRIES_VER0,
                                                    &rbkt_buf[i][0]));

            hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_raw(u, db, ln, hw_idx,
                                            &rbkt_buf[i][0]));
        }
    }

exit:
    shr_lmm_free(base_entries_lmem, base_entries);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear one base entry in HW.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] ent_idx Entry index
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_base_entry_clear(int u, int db, int ln, ln_index_t ent_idx)
{
    int rbkt_idx;
    LN_ENTRY_t rbkt_buf;
    LN_ENTRY_t base_entry = {0};
    rbkt_t *rbkt;
    int ent_ofs;
    int hw_idx;
    SHR_FUNC_ENTER(u);

    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));
    rbkt = &RBKT(u, db, ln, rbkt_idx);

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &rbkt_buf[0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Callback Functions
 */

/*!
 * \brief (HW) Copy split bkt elems from old bucket to new bucket
 *
 * The ent_index in the elems are updated to new locations, and old
 * ent_index are saved to an array.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] src_t Source tuple
 * \param [in] dst_group Dest group
 * \param [in] dst_t Dest tuple
 * \param [in] list Copy list
 * \param [in] count Count of bak_idx array
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [out] old_idx Array to save original indexes
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_copy(int u, int db, int ln,
                 rbkt_group_t src_group, uint8_t src_sub,
                 rbkt_group_t dst_group, uint8_t dst_sub,
                 shr_dq_t *list,
                 int count,
                 alpm_arg_t *arg,
                 ln_index_t *old_idx)
{
    ln_index_t src_idx, dst_idx;
    shr_dq_t *elem;
    alpm_bkt_elem_t *bkt_elem;
    int src_format, dst_format;
    LN_ENTRY_t srbkt_buf[ALPM_MAX_BANKS];
    LN_ENTRY_t drbkt_buf[ALPM_MAX_BANKS];
    int src_rbkts[ALPM_MAX_BANKS] = {0};
    int dst_rbkts[ALPM_MAX_BANKS];
    rbkt_idx_t src_rbkt_idx, dst_rbkt_idx;
    int src_bank, dst_bank;
    int src_ent, dst_ent;
    int i = 0, hw_idx;
    key_tuple_t sub_tp = {.len = dst_sub}; /* Used for transfering sub_bkt */
    SHR_FUNC_ENTER(u);

    sal_memset(dst_rbkts, -1, sizeof(dst_rbkts));
    ALPM_ASSERT(dst_rbkts[0] == -1);

    SHR_DQ_BACK_TRAVERSE(list, elem) /* Longest pick format first */
        bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        src_idx = bkt_elem->index;
        old_idx[i] = src_idx;
        SHR_IF_ERR_EXIT(
            ver0_base_entry_alloc(u, db, ln, dst_group, dst_sub,
                                  bkt_elem->key.t.len,
                                  DATA_REDUCED,
                                  &dst_idx,
                                  NULL, /* Will write anyway */
                                  &dst_format));

        dst_rbkt_idx = RBKT_FROM_INDEX(dst_idx);
        dst_bank = dst_rbkt_idx - dst_group;
        dst_ent = ENT_OFS_FROM_IDX(dst_idx);

        if (dst_rbkts[dst_bank] == -1) {
            hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, dst_rbkt_idx);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx,
                                           &drbkt_buf[dst_bank][0]));
            dst_rbkts[dst_bank] = dst_rbkt_idx;
        }
        if (src_idx != INVALID_INDEX) {
            src_rbkt_idx = RBKT_FROM_INDEX(src_idx);
            src_ent = ENT_OFS_FROM_IDX(src_idx);
            src_bank = src_rbkt_idx - src_group;
            ALPM_ASSERT(src_bank >= 0 && src_bank < ALPM_MAX_BANKS);

            if (src_rbkts[src_bank] == 0) {
                hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, src_rbkt_idx);
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx,
                                               &srbkt_buf[src_bank][0]));
                src_rbkts[src_bank] = 1;
            }

            src_format = RBKT_FORMAT(u, db, ln, src_rbkt_idx);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_move(u, format_type,
                                                  src_format,
                                                  src_ent,
                                                  NULL,
                                                  &srbkt_buf[src_bank][0],

                                                  dst_format,
                                                  dst_ent,
                                                  &sub_tp,
                                                  &drbkt_buf[dst_bank][0]
                                                  ));
        } else {
            LN_ENTRY_t base_entry = {0}; 
            assert(i == 0); /* New entry, if any, must be the first one */
            arg->a1.sub_bkt = dst_sub; /* Set sub bucket for bucket entry */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_encode(u, format_type,
                                                    dst_format,
                                                    0,
                                                    arg,
                                                    &base_entry[0]));

            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, format_type,
                                                       dst_format,
                                                       &base_entry[0],
                                                       dst_ent,
                                                       &drbkt_buf[dst_bank][0]
                                                       ));
            arg->index[ln] = dst_idx;
        }
        bkt_elem->index = dst_idx;
        i++;
    SHR_DQ_TRAVERSE_END(list, elem);

    ALPM_ASSERT(i == count);

    /* Write dest rbkts */
    for (i = 0; i < ALPM_MAX_BANKS; i++) {
        if (dst_rbkts[i] != -1) {
            hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, dst_rbkts[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_raw(u, db, ln, hw_idx,
                                            &drbkt_buf[i][0]));
        }
    }

exit:
    if (SHR_FUNC_ERR() && i) {
        ALPM_ASSERT(i < count); /* Assume just entry alloc would fail */
        count = i;
        i = 0;
        SHR_DQ_BACK_TRAVERSE(list, elem)
            bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            src_idx = bkt_elem->index;
            SHR_IF_ERR_EXIT(
                ver0_base_entry_free(u, db, ln, dst_group, src_idx,
                                     NULL));
            bkt_elem->index = old_idx[i++];
            if (i == count) {
                break;
            }
        SHR_DQ_TRAVERSE_END(list, elem);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief (HW) Move bucket elems from old bucket to new bucket
 *
 * The ent_index in the elems are updated to new locations.
 * Elems moved are deleted from old bucket.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] src_sub Source sub bucket
 * \param [in] dst_group Dest group
 * \param [in] dst_sub Destination sub bucket
 * \param [in] list Move list
 * \param [in] count Count of bak_idx array
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [in] dst_pivot_arg Arg for dst pivot.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_move(int u, int db, int ln,
                 rbkt_group_t src_group, uint8_t src_sub,
                 rbkt_group_t dst_group, uint8_t dst_sub,
                 shr_dq_t *list,
                 int count,
                 alpm_arg_t *arg,
                 alpm_arg_t *dst_pivot_arg)
{
    alpm_pivot_t *dst_pivot;
    ln_index_t *old_idx = NULL;
    uint8_t decr_count;
    SHR_FUNC_ENTER(u);

    /* $ Save old ent_idx of prefixes */
    if (count == 0) {
        SHR_EXIT();
    }

    ALPM_ALLOC(old_idx, count * sizeof(ln_index_t), "old_idx");
    sal_memset(old_idx, -1, count * sizeof(ln_index_t));

    /* $ (HW) Copy split prefixes from old bucket to new bucket */
    SHR_IF_ERR_EXIT(
        ver0_bucket_copy(u, db, ln,
                         src_group, src_sub,
                         dst_group, dst_sub,
                         list,
                         count,
                         arg,
                         old_idx));

    /* $ Activate the pivot */
    dst_pivot = ver0_bucket_pivot_get(u, db, ln, dst_group, dst_sub);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update_with_arg(u, db, PREV_LEVEL(ln), dst_pivot,
                                             dst_pivot_arg));
    /* $ Invalidate moved elems from old bucket in HW */
    SHR_IF_ERR_EXIT(
        ver0_base_entries_clear(u, db, ln, src_group, old_idx, count));

    /* $ Free those moved elems from parent buckets */
    SHR_IF_ERR_EXIT(
        ver0_base_entries_free(u, db, ln, src_group, old_idx, count));

    if (old_idx[0] == INVALID_INDEX) { /* Exists new entry */
        decr_count = count - 1;
    } else {
        decr_count = count;
    }
    ver0_rbkt_group_free_count_update(u, db, ln, src_group, -decr_count); /* decr */
    ver0_rbkt_group_free_count_update(u, db, ln, dst_group, count);
exit:
    /* Assume just bucket copy are subject to fail */
    if (old_idx != NULL) {
        SHR_FREE(old_idx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Merge entries from one bucket to another.
 *
 * The original bucket will be freed, and elems in new bucket are likely
 * reorganized to be more efficient.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] src_sub Source sub bucket
 * \param [in] dst_group Dest group
 * \param [in] dst_sub Destination sub bucket
 * \param [in] merge_type Merge type
 * \param [out] shuffled Buckets shuffled
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_merge(int u, int db, int ln,
                  rbkt_group_t src_group, uint8_t src_sub,
                  rbkt_group_t dst_group, uint8_t dst_sub,
                  bucket_merge_type_t merge_type,
                  bool *shuffled)
{
    uint8_t count;
    alpm_pivot_t *src_pivot, *dst_pivot;
    alpm_pivot_t *parent_pivot, *child_pivot;
    enum {
        BUCKET_MERGE_NONE,
        BUCKET_MERGE_COPY,
        BUCKET_MERGE_CHILD_TRIE_MERGE,
        BUCKET_MERGE_CHILD_PIVOT_DELETE,
    } done = BUCKET_MERGE_NONE;
    SHR_FUNC_ENTER(u);

    src_pivot = ver0_bucket_pivot_get(u, db, ln, src_group, src_sub);
    dst_pivot = ver0_bucket_pivot_get(u, db, ln, dst_group, dst_sub);
    ALPM_ASSERT(src_pivot);
    ALPM_ASSERT((merge_type != BUCKET_SHARE && dst_pivot) ||
                (merge_type == BUCKET_SHARE && !dst_pivot));

    count = bcmptm_rm_alpm_trie_count(src_pivot->bkt->trie);
    if (merge_type != BUCKET_MERGE_CHILD_TO_PARENT) {
        ver0_bucket_pivot_set(u, db, ln, dst_group, dst_sub, src_pivot);
    }
    /* Set new logical bucket. Do this ahead for tcam entry link.
     * If merge child to parent, then the src_pivot is child pivot, in which
     * case, it will be freed.
     */
    src_pivot->bkt->log_bkt = VER0_LOG_BKT(dst_group, dst_sub);

    SHR_IF_ERR_EXIT(
        ver0_bucket_move(u, db, ln,
                         src_group,
                         src_sub,
                         dst_group,
                         dst_sub,
                         &src_pivot->bkt->list,
                         count,
                         NULL, /* no new entry */
                         NULL));
    done = BUCKET_MERGE_COPY;
    if (merge_type != BUCKET_SHARE) { /* For true merge */
        alpm_arg_t arg2;

        if (merge_type == BUCKET_MERGE_PARENT_TO_CHILD) {
            parent_pivot = src_pivot;
            child_pivot  = dst_pivot;
        } else if (merge_type == BUCKET_MERGE_CHILD_TO_PARENT) {
            child_pivot  = src_pivot;
            parent_pivot = dst_pivot;
        }

        /* SW bucket merge: not expect to fail */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_merge(parent_pivot->bkt->trie,
                                      child_pivot->bkt->trie->trie,
                                      &child_pivot->key.t.trie_ip.key[0],
                                      child_pivot->key.t.len));
        done = BUCKET_MERGE_CHILD_TRIE_MERGE;
        bcmptm_rm_alpm_arg_init(u, &arg2);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_arg_from_pivot(u, db, PREV_LEVEL(ln), child_pivot,
                                          true, &arg2));
        /* Child pivot will be deleted and freed */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_delete(u, PREV_LEVEL(ln), &arg2));
        done = BUCKET_MERGE_CHILD_PIVOT_DELETE;
        if (shuffled) {
            *shuffled = true;
        }
    }

    ver0_bucket_pivot_clr(u, db, ln, src_group, src_sub);
    /* do not use ver0_bucket_free, because we still want this group.
    SHR_IF_ERR_EXIT(
        ver0_bucket_free(u, db, ln, src_group, src_sub));
    */

exit:
    if (SHR_FUNC_ERR()) {
        /*  Not expecting any failure after pivot deleted */
        assert(done != BUCKET_MERGE_CHILD_PIVOT_DELETE);
        switch (done)  {
        case BUCKET_MERGE_CHILD_TRIE_MERGE:
            if (merge_type != BUCKET_SHARE) {
                uint32_t tmp_pivot_len;
                trie_ip_t tmp_pivot_key;
                bcmptm_rm_alpm_trie_split2(parent_pivot->bkt->trie,
                                           &child_pivot->key.t.trie_ip.key[0],
                                           child_pivot->key.t.len,
                                           &tmp_pivot_key.key[0],
                                           &tmp_pivot_len,
                                           &child_pivot->bkt->trie->trie);
            }
            /* Fall through */
        case BUCKET_MERGE_COPY:
            if (merge_type != BUCKET_MERGE_CHILD_TO_PARENT) {
                ver0_bucket_pivot_clr(u, db, ln, dst_group, dst_sub);
            }
            /* Fall through */
        case BUCKET_MERGE_NONE:
        default:
            break;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find two qualified group and merge, such that one become free.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] free_group Free group
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_share(int u, int db, int ln, rbkt_group_t *free_group)
{
    int cnt, cnt2;
    uint8_t bkt_cnt;
    rbkt_group_t src_group, dst_group;
    uint8_t fmt, first_round, src_sbkts;
    shr_dq_t *group_list, *group_list2;
    shr_dq_t *elem;
    static uint32_t bucket_share;
    static uint8_t fast_popcount4[] = {
         0,  /* 0000 */
         1,  /* 0001 */
         1,  /* 0010 */
         2,  /* 0011 */
         1,  /* 0100 */
         2,  /* 0101 */
         2,  /* 0110 */
         3,  /* 0111 */
         1,  /* 1000 */
         2,  /* 1001 */
         2,  /* 1010 */
         3,  /* 1011 */
         2,  /* 1100 */
         3,  /* 1101 */
         3,  /* 1110 */
         4   /* 1111 */
    };
    SHR_FUNC_ENTER(u);

    dst_group = 0;

    for (fmt = VER0_FORMAT_1; fmt <= MAX_FORMAT(u, FORMAT_ROUTE); fmt++) {

    bkt_cnt= ver0_base_entry_count(u, db, ln, fmt);
    /* Assure total count of two buckets won't exceed max */
    first_round = 1;
    src_group = 0;
    src_sbkts = 0;
    for (cnt = 1; cnt <= (bkt_cnt >> 1); cnt++) {
        rbkt_t *curr;
        group_list = &FMT_GROUP_USAGE_LIST(u, db, ln, fmt, cnt);

        SHR_DQ_TRAVERSE(group_list, elem) /* Group 0 won't be traversed */
            curr = SHR_DQ_ELEMENT_GET(rbkt_t *, elem, fmt_usgnode);
            ALPM_ASSERT(curr->sub_bkts && curr->sub_bkts != 0xF);
            ALPM_ASSERT(curr->count == cnt);
            if (first_round) {
                if (src_group == 0) {
                    src_group = curr->index;
                    src_sbkts = curr->sub_bkts;
                } else if (
                    VER0_CAN_BUCKET_SHARE(u, db, ln, src_group, curr->index)) {
                    dst_group = curr->index;
                    goto done;
                } else if (fast_popcount4[curr->sub_bkts] <
                           fast_popcount4[src_sbkts]) {
                    src_group = curr->index;
                    src_sbkts = curr->sub_bkts;
                }
            } else {
                if (src_group &&
                    VER0_CAN_BUCKET_SHARE(u, db, ln, src_group, curr->index)) {
                    dst_group = curr->index;
                    goto done;
                } else if (fast_popcount4[curr->sub_bkts] <
                           fast_popcount4[src_sbkts]) {
                    /*
                     * We have found a src_group with least entries in the
                     * first round, but there is no dst_group can share with
                     * it. In the following rounds, the src_group will be with
                     * more entries, therefore the only chance to share is to
                     * look for a src_group that is with less sub_buckets.
                     */
                    src_group = curr->index;
                    src_sbkts = curr->sub_bkts;
                }
            }
        SHR_DQ_TRAVERSE_END(group_list, elem);

        if (src_group == 0) {
            continue;
        }

        first_round = 0;
        /* Reverse order to make group compact */
        for (cnt2 = (bkt_cnt - cnt); cnt2 >= cnt + 1; cnt2--) {
/*        for (cnt2 = cnt +1 ; cnt2 <= (bkt_cnt - cnt); cnt2++) { */
            rbkt_t *curr2;
            group_list2 = &FMT_GROUP_USAGE_LIST(u, db, ln, fmt, cnt2);
            SHR_DQ_TRAVERSE(group_list2, elem) /* Group 0 won't be traversed */
                curr2 = SHR_DQ_ELEMENT_GET(rbkt_t *, elem, fmt_usgnode);
                ALPM_ASSERT(curr2->sub_bkts && curr2->sub_bkts != 0xF);
                if (VER0_CAN_BUCKET_SHARE(u, db, ln, src_group, curr2->index)) {
                    dst_group = curr2->index;
                    goto done;
                }
            SHR_DQ_TRAVERSE_END(group_list, elem);
        }
        src_group = 0;
    }
    }
done:
    if (dst_group) {
        uint8_t src_sub, dst_sub;
        for (src_sub = 0; src_sub < MAX_SUB_BKTS; src_sub++) {
            if ((RBKT_SUB_BKT_IS_SET(u, db, ln, src_group, src_sub))) {
                SHR_IF_ERR_EXIT(
                    ver0_sub_bucket_alloc(u, db, ln, dst_group, &dst_sub));
                SHR_IF_ERR_EXIT(
                   ver0_bucket_merge(u, db, ln, src_group, src_sub,
                                     dst_group, dst_sub, BUCKET_SHARE, NULL));
            }
        }

        *free_group = src_group;
    } else {
        SHR_RETURN(SHR_E_FULL);
    }
    bucket_share++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find the most free rbkt group and carve out a sub bucket from it.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] fmt Format value
 * \param [out] group Group with most free entries
 * \param [out] sub_bkt Sub bucket carved from group
 *
 * \return SHR_E_NONE only.
 */
static int
ver0_bucket_carve(int u, int db, int ln, uint8_t fmt,
                  rbkt_group_t *group, uint8_t *sub_bkt)
{
    uint8_t cnt, max_cnt;
    shr_dq_t *group_list, *elem;
    rbkt_t *curr;
    SHR_FUNC_ENTER(u);

    /*
     * Try to alloc a sub bucket from least occupied bucket of
     * same memory fmt.
     */
    ALPM_ASSERT(fmt != DISABLED_FORMAT);
    max_cnt = ver0_base_entry_count(u, db, ln, fmt);
    for (cnt = 1; cnt < max_cnt - 1; cnt++) { /* at least two free entry */
        group_list = &FMT_GROUP_USAGE_LIST(u, db, ln, fmt, cnt);
        SHR_DQ_TRAVERSE(group_list, elem) 
            curr = SHR_DQ_ELEMENT_GET(rbkt_t *, elem, fmt_usgnode);
            ALPM_ASSERT(curr->sub_bkts != 0xF);
            *group = curr->index;
            return ver0_sub_bucket_alloc(u, db, ln, *group, sub_bkt);
        SHR_DQ_TRAVERSE_END(group_list, elem);
    }
    SHR_RETURN(SHR_E_FULL);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a logical bucket.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] group Group
 * \param [out] sub_bkt Sub bucket
 *
 * \return SHR_E_NONE only.
 */
static inline int
ver0_bucket_free(int u, int db, int ln, rbkt_group_t group, uint8_t sub_bkt)
{
    ver0_bucket_pivot_clr(u, db, ln, group, sub_bkt);

    if (RBKT_ECOUNT(u, db, ln, group) == 0) {
        return ver0_rbkt_group_free(u, db, ln, group);
    }
    return SHR_E_NONE;
}

/*!
 * \brief Allocate a new logical bucket with various sizes.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] fmt Format value
 * \param [out] group New group allocated
 * \param [out] sub_bkt New sub bucket allocated
 *
 * \return SHR_E_NONE only.
 */
static int
ver0_bucket_alloc(int u, int db, int ln, uint8_t fmt,
                  rbkt_group_t *group, uint8_t *sub_bkt)
{
    int rv;
    uint8_t size;
    SHR_FUNC_ENTER(u);

    size = bucket_scale[u];
    rv = ver0_rbkt_group_alloc(u, db, ln, fmt, size, false, group);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT(
            ver0_sub_bucket_alloc(u, db, ln, *group, sub_bkt));
    } else if (rv == SHR_E_FULL) {
        rv = ver0_bucket_share(u, db, ln, group);
        if (SHR_SUCCESS(rv)) {
            uint8_t i;
            ALPM_ASSERT(RBKT_VBANKS(u, db, ln, *group) ==
                        size * BKT_HDL_MAX_BANKS(u, db, ln));
            for (i = 0; i < RBKT_VBANKS(u, db, ln, *group); i++) {
                RBKT_FORMAT(u, db, ln, i + *group) = fmt;
            }
            SHR_IF_ERR_EXIT(
                ver0_sub_bucket_alloc(u, db, ln, *group, sub_bkt));
            ALPM_ASSERT(*sub_bkt == 0);
            ALPM_ASSERT(RBKT_ECOUNT(u, db, ln, *group) == 0);
        } else if (rv == SHR_E_FULL) {
            SHR_IF_ERR_EXIT(
                ver0_bucket_carve(u, db, ln, fmt, group, sub_bkt));
            ALPM_ASSERT(RBKT_ECOUNT(u, db, ln, *group));
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
        /* Reclaim the physical bucket in case it has been freed */
        ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, *group));
    }
    /* Succeed */
    ALPM_ASSERT(RBKT_FORMAT(u, db, ln, *group) == fmt);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allcoate a new bucket and move entries in old bucket to this new one.
 *
 * The original bucket will be freed, and elems in new bucket are likely
 * reorganized to be more efficient.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Source group
 * \param [in] src_sub Source sub bucket
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [out] dst_group Destination group
 * \param [out] dst_sub Destination sub bucket
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_alloc_and_move(int u, int db, int ln,
                           rbkt_group_t src_group,
                           uint8_t src_sub,
                           alpm_arg_t *arg,
                           rbkt_group_t *dst_group,
                           uint8_t *dst_sub)
{
    int count;
    rbkt_group_t new_group;
    uint8_t new_sub, fmt;
    alpm_pivot_t *src_pivot;
    enum {
        ALLOC_AND_MOVE_NONE,
        ALLOC_AND_MOVE_ALLOC,
        ALLOC_AND_MOVE_MOVE
    } done = ALLOC_AND_MOVE_NONE;
    SHR_FUNC_ENTER(u);

    src_pivot = ver0_bucket_pivot_get(u, db, ln, src_group, src_sub);
    fmt = RBKT_FORMAT(u, db, ln, src_group);
    SHR_IF_ERR_EXIT(
        ver0_bucket_alloc(u, db, ln, fmt, &new_group, &new_sub));
    ver0_bucket_pivot_set(u, db, ln, new_group, new_sub, src_pivot);
    done = ALLOC_AND_MOVE_ALLOC;

    /* $ Save old ent_idx of prefixes */
    count = bcmptm_rm_alpm_trie_count(src_pivot->bkt->trie);
    SHR_IF_ERR_EXIT(
        ver0_bucket_move(u, db, ln,
                         src_group,
                         src_sub,
                         new_group,
                         new_sub,
                         &src_pivot->bkt->list,
                         count,
                         arg, /* New entry */
                         NULL));

    done = ALLOC_AND_MOVE_MOVE;
    SHR_IF_ERR_EXIT(
        ver0_bucket_free(u, db, ln, src_group, src_sub));
    src_pivot->bkt->log_bkt = VER0_LOG_BKT(new_group, new_sub);
    *dst_group = new_group;
    *dst_sub = new_sub;
exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case ALLOC_AND_MOVE_MOVE:
            ALPM_ASSERT(0); /* not supposed to fail */
        case ALLOC_AND_MOVE_ALLOC:
            ver0_bucket_free(u, db, ln, new_group, new_sub);
        case ALLOC_AND_MOVE_NONE:
        default:
            break;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Encode and write the base entry into HW.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] arg ALPM arg
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_base_entry_encode_write(int u, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    LN_ENTRY_t rbkt_buf;
    LN_ENTRY_t base_entry = {0};
    rbkt_t *rbkt;
    ln_index_t ent_idx;
    int ent_ofs, hw_idx;
    int pivot_len, db;
    alpm_pivot_t *pivot;
    SHR_FUNC_ENTER(u);

    if (arg->skip_write == WRITE_LATER) {
        SHR_EXIT();
    }

    ent_idx = arg->index[ln];

    db = arg->db;
    pivot = arg->pivot[PREV_LEVEL(ln)];
    pivot_len = pivot->key.t.len;
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));

    rbkt = &RBKT(u, db, ln, rbkt_idx);

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    arg->a1.sub_bkt = VER0_SUB_BKT(u, pivot->bkt->log_bkt);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_encode(u, format_type,
                                            rbkt->format,
                                            pivot_len,
                                            arg,
                                            &base_entry[0]));
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        alpm_arg_t arg2;
        bcmptm_rm_alpm_arg_init(u, &arg2);
        /* Decode needs non-info except the rbkt info */
        arg2.key.t.ipv = arg->key.t.ipv;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, format_type,
                                                rbkt->format,
                                                pivot_len,
                                                &arg2,
                                                &base_entry[0]));
        /* Ver0 stores full ip in bucket, thus don't need api key create*/
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_len_to_mask(u, arg2.key.t.ipv, arg2.key.t.len,
                                       &arg2.key.ip_mask[0]));

        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_addr, arg->key.ip_addr,
                                sizeof(alpm_ip_t)));
        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_mask, arg->key.ip_mask,
                                sizeof(alpm_ip_t)));
        ALPM_ASSERT(arg2.key.t.len == arg->key.t.len);
        ALPM_ASSERT(arg2.key.t.ipv == arg->key.t.ipv);
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_set(u, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &rbkt_buf[0]));

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));

    if (arg->set_hit) { /* Only for entry update */
        uint32_t eidx, hit_entry[1]; /* For coverity */
        eidx = ver0_hw_eidx_from_ent_idx(u, db, ln, ent_idx);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_hit(u, db, ln, eidx >> 2, hit_entry));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_hit_set(u, ln, (eidx & 3), hit_entry, arg->hit));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_write_ln_hit(u, db, ln, eidx >> 2, hit_entry));
    }

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Read and decode the base entry from HW.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [out] arg ALPM arg
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_base_entry_decode_read(int u, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    LN_ENTRY_t rbkt_buf;
    LN_ENTRY_t base_entry = {0};
    rbkt_t *rbkt;
    int ent_ofs, hw_idx;
    uint32_t eidx;
    int db = arg->db;
    ln_index_t ent_idx;
    uint32_t hit_entry[1]; /* For coverity */
    uint8_t hit;
    SHR_FUNC_ENTER(u);

    ent_idx = arg->index[ln];
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver0_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));

    rbkt = &RBKT(u, db, ln, rbkt_idx);

    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_get(u, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &rbkt_buf[0]));
    /* Decode needs non-info except the rbkt info */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_decode(u, format_type,
                                            rbkt->format,
                                            0, /* Not used */
                                            arg,
                                            &base_entry[0]));

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));

    /* Ver0 stores full ip in bucket, thus don't need api key create. */
    /*
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_api_key_create(u, ipv,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      &arg->key.ip_addr[0]));
                                      */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_len_to_mask(u, arg->key.t.ipv,
                                   arg->key.t.len,
                                   &arg->key.ip_mask[0]));

    eidx = ver0_hw_eidx_from_ent_idx(u, db, ln, ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_hit(u, db, ln, eidx >> 2, hit_entry));
    /* Get hit bit */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_hit_get(u, ln, (eidx & 3), hit_entry, &hit));
    arg->hit = hit;
exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Insert an entry into a bucket.
 *
 * This routine insert both SW and HW resoureces.
 * Ideally, just take free base entry. If that is not possible,
 * take free bank, and pick the optimal format. If that is again
 * not possible, expand the bucket group.
 * By expanding the group, it could expand left/right, or move, or
 * shuffle then move.
 * Must ensure no packet misses during expand/move/shuffle process.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pivot Pivot pointing the target bucket.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_insert(int u, int ln, alpm_arg_t *arg,
                   alpm_pivot_t *pivot)
{
    int format_value;
    int bucket_len = arg->key.t.len - pivot->key.t.len;
    rbkt_group_t group;
    ln_index_t ent_idx = INVALID_INDEX;
    int rv;
    uint8_t sub_bkt;
    SHR_FUNC_ENTER(u);

    if (pivot->bkt->log_bkt == INVALID_LOG_BKT) {
        /*
         * If pivot exists, but bucket group is invalid,
         * this implies the pivot is just itself without a bucket.
         * So we should alloc a bucket group for it first.
         */
        ALPM_ASSERT(pivot->bkt->trie->trie == NULL);
        SHR_IF_ERR_EXIT(
            ver0_bucket_alloc(u, arg->db, ln, arg->fmt, &group, &sub_bkt));
        ver0_bucket_pivot_set(u, arg->db, ln, group, sub_bkt, pivot);
        pivot->bkt->log_bkt = VER0_LOG_BKT(group, sub_bkt);
    } else {
        group = VER0_GROUP(u, pivot->bkt->log_bkt);
        sub_bkt = VER0_SUB_BKT(u, pivot->bkt->log_bkt);
    }
    rv = ver0_base_entry_alloc(u, arg->db, ln, group, sub_bkt,
                               bucket_len,
                               DATA_REDUCED,
                               &ent_idx,
                               NULL,
                               &format_value);
    if (rv == SHR_E_FULL) {
        SHR_RETURN(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    ALPM_ASSERT(ent_idx != INVALID_INDEX);
    arg->index[ln] = ent_idx;
    arg->pivot[PREV_LEVEL(ln)] = pivot; /* Set pivot for pivot_len */
    SHR_IF_ERR_EXIT(
        ver0_base_entry_encode_write(u, ln, arg));
    ver0_rbkt_group_free_count_update(u, arg->db, ln, group, 1);
exit:
    SHR_FUNC_EXIT();
}
static void
ver0_split_arg_init(int u,
                    int ln,
                    alpm_arg_t *arg,
                    trie_ip_t *pivot_key,
                    uint32_t pivot_len,
                    rbkt_group_t new_group,
                    uint8_t new_sub_bkt,
                    alpm_pivot_t *parent_pivot,
                    alpm_arg_t *split_arg
                    )
{
    int prev_ln = ln - 1;
    bcmptm_rm_alpm_arg_init(u, split_arg);
    split_arg->key.t.ipv = arg->key.t.ipv;
    split_arg->key.t.w_vrf = arg->key.t.w_vrf;
    /* Key type will be set in bcmptm_rm_alpm_l1_insert */
    split_arg->key.vt = arg->key.vt;
    split_arg->key.t.len = pivot_len;
    sal_memcpy(&split_arg->key.t.trie_ip, pivot_key, sizeof(trie_ip_t));
    split_arg->log_bkt[prev_ln] = VER0_LOG_BKT(new_group, new_sub_bkt);
    split_arg->valid = 1;
    split_arg->db = arg->db;

    bcmptm_rm_alpm_api_key_create(u, arg->key.t.ipv,
                                  pivot_key,
                                  pivot_len,
                                  split_arg->key.ip_addr);
    bcmptm_rm_alpm_len_to_mask(u, arg->key.t.ipv,
                               pivot_len,
                               &split_arg->key.ip_mask[0]);
}



/*!
 * \brief Delete an entry within a bucket.
 *
 * This routine delete both SW and HW resoureces. After entry deleted
 * the rbkts should continue to be contiguous.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pivot Pivot pointing the target bucket.
 * \param [in] ent_idx Entry index to delete
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_delete(int u, int db, int ln,
                   alpm_pivot_t *pivot,
                   ln_index_t ent_idx)
{
    rbkt_group_t group = VER0_GROUP(u, pivot->bkt->log_bkt);
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        ver0_base_entry_clear(u, db, ln, ent_idx));

    SHR_IF_ERR_EXIT(
        ver0_base_entry_free(u, db, ln, group, ent_idx, NULL));

    ver0_rbkt_group_free_count_update(u, db, ln, group, -1);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Split a bucket into two
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] arg ALPM arg for new entry
 * \param [in] parent_pivot Pivot whose bucket to be split
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver0_bucket_split(int u, int ln, alpm_arg_t *arg, alpm_pivot_t *parent_pivot,
                  bkt_split_aux_t *split_aux)
{
    int rv;
    rm_alpm_trie_t *split_trie = NULL;
    rbkt_group_t new_group, parent_group;
    uint8_t new_sub, parent_sub;
    alpm_pivot_t *split_pivot = NULL;
    int split_count;
    uint32_t msl, msc;
    rm_alpm_trie_node_t *split_root = NULL;
    uint32_t pivot_len;
    trie_ip_t pivot_key;
    alpm_arg_t split_arg;
    int prev_ln = ln - 1;
    enum {
        SPLIT_NONE,
        SPLIT_ALLOC_GROUP,
        SPLIT_TRIE_SPLIT,
        SPLIT_PIVOT_INSERT,
        SPLIT_BUCKET_CONNECT,
        SPLIT_BUCKET_MOVE_AND_ACTIVATE,
        SPLIT_OLD_BUCKET_CLEANUP,
    } done = SPLIT_NONE;
    bool new_in_parent = false, new_in_split = false;
    static int split;
    SHR_FUNC_ENTER(u);

    /*
     * $ Allocate a new group.
     * First try with simple alloc (no shuffle) for several sizes.
     * The shuffle alloc kicks in right before trying with least size.
     */
    SHR_IF_ERR_EXIT(
        ver0_bucket_alloc(u, arg->db, ln, arg->fmt, &new_group, &new_sub));
    arg->log_bkt[prev_ln] = VER0_LOG_BKT(new_group, new_sub);
    done = SPLIT_ALLOC_GROUP;

    /* $ Trie split */
    parent_group = VER0_GROUP(u, parent_pivot->bkt->log_bkt);
    parent_sub = VER0_SUB_BKT(u, parent_pivot->bkt->log_bkt);
    msl = ver0_trie_msl(u, arg->key.t.ipv);
    msc = ver0_trie_msc(u, arg->db, ln, arg->fmt, new_group);

    /* Can be saved for performance */
    sal_memset(&pivot_key, 0, sizeof(pivot_key));

    /* For non-last level, payload split can save us a lot of trouble */
    rv = bcmptm_rm_alpm_trie_split(parent_pivot->bkt->trie,
                                   msl,
                                   &pivot_key.key[0],
                                   &pivot_len,
                                   &split_root,
                                   NULL,
                                   false,
                                   msc);
    if (SHR_SUCCESS(rv)) {
        if (pivot_len > msl) {
            SHR_RETURN(SHR_E_FULL);
        }
        done = SPLIT_TRIE_SPLIT;
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    /* $ Split arg init */
    ver0_split_arg_init(u, ln, arg, &pivot_key, pivot_len, new_group, new_sub,
                        parent_pivot, &split_arg);

    /* $ Pivot insert
     * E_FULL can be returned for pivot full. Properly roll back.
     */

    rv = bcmptm_rm_alpm_split_pivot_add(u, arg->db, prev_ln, &split_arg,
                                        &split_pivot, split_aux);
    SHR_IF_ERR_EXIT(rv);
    split_trie = split_pivot->bkt->trie;
    ALPM_ASSERT(split_trie);
    split_trie->trie = split_root;
    split_count = split_root->count;
    ver0_bucket_pivot_set(u, arg->db, ln, new_group, new_sub, split_pivot);
    done = SPLIT_PIVOT_INSERT;

    /* $ Connect bkt elems in new split bucket */
    SHR_IF_ERR_EXIT(
        ver0_bkt_elems_connect(u, &split_pivot->bkt->list, split_trie,
                               &new_in_split));

    /* $ Connect bkt elems in parent bucket */
    SHR_IF_ERR_EXIT(
        ver0_bkt_elems_connect(u, &parent_pivot->bkt->list,
                               parent_pivot->bkt->trie,
                               &new_in_parent));
    if (new_in_split) {
        arg->pivot[prev_ln] = split_pivot;
    }
    done = SPLIT_BUCKET_CONNECT;

    SHR_IF_ERR_EXIT(
        ver0_bucket_move(u, arg->db, ln,
                         parent_group,
                         parent_sub,
                         new_group,
                         new_sub,
                         &split_pivot->bkt->list,
                         split_count,
                         arg, /* May have new entry */
                         &split_arg));
    done = SPLIT_BUCKET_MOVE_AND_ACTIVATE;


    if (RBKT_ECOUNT(u, arg->db, ln, parent_group) == 0) {
        ALPM_ASSERT(parent_pivot->bkt->trie->trie == NULL);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_clean(u, arg->db, ln, parent_pivot));
    }
    done = SPLIT_OLD_BUCKET_CLEANUP;
    
    if (new_in_parent) {
        rv = ver0_bucket_insert(u, ln, arg, parent_pivot);
        if (rv == SHR_E_FULL) {
            rbkt_group_t new_group2;
            uint8_t new_sub2;
            /* Take the chance to reorganize */
            rv = ver0_bucket_alloc_and_move(u, arg->db, ln,
                                            parent_group, parent_sub,
                                            arg, &new_group2, &new_sub2);
            /* Retry is meanless for ver0
            if (rv == SHR_E_FULL) {
                rv = SHR_E_BADID;
            }*/
        }
        SHR_IF_ERR_EXIT(rv);
    }
    ver0_rbkt_group_sanity(u, arg->db, ln, new_group);
    ver0_rbkt_group_sanity(u, arg->db, ln, parent_group);
    split++;
#if 0
    if (ALPM_INFO(u)->alpm_dbg_ctrl.f.pivot_sanity_node) {
        rv = alpm_pivot_sanity_cb(u, ln, parent_pivot, "SplitOpivot");
        if (SHR_SUCCESS(rv)) {
            rv = alpm_pivot_sanity_cb(u, ln, split_pivot, "SplitNpivot");
        }
    }
#endif

    /*    c_split++; */

exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case SPLIT_OLD_BUCKET_CLEANUP:
            /* That's ok once split is done. Break. */
            break;
        case SPLIT_BUCKET_MOVE_AND_ACTIVATE:
            /* not supposed to fail */
            ALPM_ASSERT(0);
        case SPLIT_BUCKET_CONNECT:
        /* Just do nothing, the elems will be reconnected in following op */
        case SPLIT_PIVOT_INSERT:
            split_trie->trie = NULL;
            /*
             * Will be done in ver0_bucket_free
             * ver0_bucket_pivot_clr(u, arg->db, ln, new_group, new_sub);
             */
            rv = bcmptm_rm_alpm_split_pivot_delete(u, arg->db, prev_ln,
                                                   &split_arg, NULL);
            ALPM_ASSERT(rv == 0);
            /* Fall through */
        case SPLIT_TRIE_SPLIT:
            bcmptm_rm_alpm_trie_merge(parent_pivot->bkt->trie,
                                      split_root,
                                      &pivot_key.key[0],
                                      pivot_len);
            ver0_bkt_elems_connect(u, &parent_pivot->bkt->list,
                                   parent_pivot->bkt->trie,
                                   &new_in_parent);
            ALPM_ASSERT(new_in_parent);
            /* Fall through */
        case SPLIT_ALLOC_GROUP:
            ver0_bucket_free(u, arg->db, ln, new_group, new_sub);
            break;
        default:
            assert(done == SPLIT_NONE);
            /* nothing to roll back */
            break;
        }
    } else if (new_in_split || new_in_parent) {
        ALPM_ASSERT(arg->index[ln] >= 0);
    }
    SHR_FUNC_EXIT();
}

static inline int
ver0_log_bucket_free(int u, int db, int ln, log_bkt_t log_bkt)
{
    return ver0_bucket_free(u, db, ln, VER0_GROUP(u, log_bkt),
                            VER0_SUB_BKT(u, log_bkt));
}

static int
ver0_log_bucket_alloc(int u, int ln, alpm_arg_t *arg, log_bkt_t *log_bkt)
{
    rbkt_group_t group;
    uint8_t sub_bkt;
    int rv;
    rv = ver0_bucket_alloc(u, arg->db, ln, arg->fmt, &group, &sub_bkt);
    if (SHR_SUCCESS(rv)) {
        *log_bkt = VER0_LOG_BKT(group, sub_bkt);
    }
    return rv;
}

static inline void
ver0_log_bucket_pivot_set(int u, int db, int ln, log_bkt_t log_bkt,
                          alpm_pivot_t *pivot)
{
    ver0_bucket_pivot_set(u, db, ln, VER0_GROUP(u, log_bkt),
                          VER0_SUB_BKT(u, log_bkt), pivot);
}

static alpm_pivot_t *
ver0_log_bucket_pivot_get(int u, int db, int ln, ln_index_t index)
{
    rbkt_idx_t rbkt_idx;
    uint8_t sub_bkt;

    
    rbkt_idx = RBKT_FROM_INDEX(index);
    ALPM_ASSERT(rbkt_idx >= 0 && rbkt_idx < BKT_HDL_RBKTS_TOTAL(u, db, ln));
    while (!RBKT_IS_HEAD(u, db, ln, rbkt_idx) && rbkt_idx >= 0) {
        rbkt_idx--;
    }
    ALPM_ASSERT(rbkt_idx >= 0);

    sub_bkt = SUB_BKT_FROM_IDX(index);
    return ver0_bucket_pivot_get(u, db, ln, rbkt_idx, sub_bkt);
}

static void
ver0_alpm_data_encode(int u, int ln, alpm_arg_t *arg, void *adata)
{
    alpm1_data_t *a1;
    rbkt_group_t group = VER0_GROUP(u, arg->log_bkt[LEVEL1]);

    a1 = (alpm1_data_t *)adata;
    a1->direct_route = 0; /* to skip Level-1  pivot mgmt */
    a1->default_miss = arg->default_route; /* REVISIT */
    a1->phy_bkt = group / BKT_HDL_MAX_BANKS(u, arg->db, NEXT_LEVEL(ln));
    a1->sub_bkt = VER0_SUB_BKT(u, arg->log_bkt[LEVEL1]);
}

static int
ver0_bucket_recover(int u, int ln, alpm_arg_t *arg)
{
    uint8_t sub_bkt;
    rbkt_group_t group;
    int db = arg->db;

    SHR_FUNC_ENTER(u);
    group = arg->a1.phy_bkt * BKT_HDL_MAX_BANKS(u, arg->db, ln);
    sub_bkt = arg->a1.sub_bkt;

    SHR_IF_ERR_EXIT(
        ver0_log_bucket_recover(u, db, ln, group, sub_bkt, arg));
exit:
    SHR_FUNC_EXIT();
}

static int
ver0_bucket_recover_done(int u, int db, int ln)
{
    uint32_t free_banks;
    rbkt_t *curr_rbkt, *prev_rbkt;
    rbkt_group_t prev_group;
    rbkt_group_t group;

    /* Skip group 0 */
    prev_group = 0;
    for (group = 1; group < BKT_HDL_RBKTS_TOTAL(u, db, ln); group++) {
        if (!RBKT_IS_HEAD(u, db, ln, group)) {
            continue;
        }
        BKT_HDL_GROUPS_USED(u, db, ln)++;
        curr_rbkt = &RBKT(u, db, ln, group);
        prev_rbkt = &RBKT(u, db, ln, prev_group);
        SHR_DQ_INSERT_NEXT(&prev_rbkt->listnode, &curr_rbkt->listnode);
        free_banks =
            curr_rbkt->index - prev_rbkt->index - prev_rbkt->valid_banks;
        ALPM_ASSERT((int)free_banks >= 0);
        ver0_rbkt_group_free_bank_set(u, db, ln, prev_rbkt->index, free_banks);
        prev_group = group;
    }
    prev_rbkt = &RBKT(u, db, ln, prev_group);
    free_banks = BKT_HDL_RBKTS_TOTAL(u, db, ln) -
                 prev_rbkt->index - prev_rbkt->valid_banks;
    ALPM_ASSERT((int)free_banks >= 0);
    ver0_rbkt_group_free_bank_set(u, db, ln, prev_rbkt->index, free_banks);

    return SHR_E_NONE;
}

static int
ver0_bkt_hdl_init(int u, bool recover)
{
    alpm_dev_info_t *dev_info;
    int db_idx, level;
    int db_count;
    int db_max_levels[DBS] = {0};
    uint8_t max_count, fmt;
    int rv, alpm_mode;
    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);
    MAX_FORMAT(u, FORMAT_PIVOT) = dev_info->pivot_max_format;
    MAX_FORMAT(u, FORMAT_ROUTE) = dev_info->route_max_format;

    if ((bcmptm_rm_alpm_mode(u) != ALPM_MODE_PARALLEL) &&
        !bcmptm_rm_alpm_is_urpf(u)) {
        bucket_scale[u] = 2;
    } else {
        bucket_scale[u] = 1;
    }

    db_count = bcmptm_rm_alpm_database_count(u);
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, db_idx);
        db_max_levels[db_idx] = database_ptr->max_levels;
        if (db_max_levels[db_idx] > 2) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }
    alpm_mode = bcmptm_rm_alpm_mode(u);
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        /* Start from Level2 */
        for (level = LEVEL2; level < db_max_levels[db_idx]; level++) {
            int i;
            bkt_hdl_t *bkt_hdl = NULL;
            void *bkt_hdl_profile;
            int alloc_sz;
            uint8_t max_formats;
            rbkt_t *rbkt;

            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bkt_hdl_profile(u, alpm_mode,
                                               db_max_levels[DB0],
                                               db_max_levels[DB1],
                                               db_idx,
                                               level,
                                               &bkt_hdl_profile));

            ALPM_ALLOC(bkt_hdl, sizeof(bkt_hdl_t), "Ver0 bucket handle");
            sal_memcpy(bkt_hdl, bkt_hdl_profile, sizeof(*bkt_hdl));
            BKT_HDL(u, db_idx, level) = bkt_hdl;

            /* Bucket info */
            bkt_hdl->rbkts_total >>= bucket_shifts[u];
            bkt_hdl->bucket_bits -= bucket_shifts[u];
            bkt_hdl->groups_used = 1;
            ALPM_ASSERT(bkt_hdl->rbkts_total ==
                        bkt_hdl->max_banks * (1 << bkt_hdl->bucket_bits));

            SHR_DQ_INIT(&bkt_hdl->global_group_list);
            alloc_sz = (bucket_scale[u] + 1) * sizeof(usage_list_t);
            ALPM_ASSERT(bkt_hdl->group_usage == NULL);
            ALPM_ALLOC(bkt_hdl->group_usage, alloc_sz, "group usage");
            sal_memset(bkt_hdl->group_usage, 0, alloc_sz);
            for (i = 0; i <= bucket_scale[u]; i++) {
                SHR_DQ_INIT(&bkt_hdl->group_usage[i].groups);
            }
            bkt_hdl->group_usage_count = (bucket_scale[u] + 1);

            max_formats = MAX_FORMAT(u, FORMAT_ROUTE);
            ALPM_ASSERT(bkt_hdl->fmt_group_usage == NULL);
            alloc_sz = (max_formats + 1) * sizeof(usage_list_t *);
            ALPM_ALLOC(bkt_hdl->fmt_group_usage, alloc_sz, "group usage");
            sal_memset(bkt_hdl->fmt_group_usage, 0, alloc_sz);

            for (fmt = VER0_FORMAT_1; fmt <= max_formats; fmt++) {
                max_count = ver0_base_entry_count(u, db_idx, level, fmt);
                alloc_sz = (max_count + 1) * sizeof(usage_list_t);
                ALPM_ASSERT(bkt_hdl->fmt_group_usage[fmt] == NULL);
                ALPM_ALLOC(bkt_hdl->fmt_group_usage[fmt], alloc_sz, "group usage");
                sal_memset(bkt_hdl->fmt_group_usage[fmt], 0, alloc_sz);
                for (i = 0; i <= max_count; i++) {
                    SHR_DQ_INIT(&bkt_hdl->fmt_group_usage[fmt][i].groups);
                }
            }

            alloc_sz = bkt_hdl->rbkts_total * sizeof(rbkt_t);
            ALPM_ASSERT(bkt_hdl->rbkt_array == NULL);
            ALPM_ALLOC(bkt_hdl->rbkt_array, alloc_sz,
                       "base bucket array");
            sal_memset(bkt_hdl->rbkt_array, 0, alloc_sz);

            /* Setup rbkt[0] as the first group assigned with all free rbkts */
            rbkt = &bkt_hdl->rbkt_array[0];
            rbkt->index = 0;
            rbkt->is_head = HEAD;
            /* Do not skip if recover */
            SHR_DQ_INSERT_HEAD(&bkt_hdl->global_group_list,
                               &rbkt->listnode);
            /* Skip if recover, as it will be handled in later recovery process */
            if (!recover) {
                rbkt->free_banks = bkt_hdl->rbkts_total;
                SHR_DQ_INSERT_HEAD(&bkt_hdl->group_usage[bucket_scale[u]].groups,
                                   &rbkt->usgnode);
                for (fmt = VER0_FORMAT_1; fmt <= max_formats; fmt++) {
                    max_count = ver0_base_entry_count(u, db_idx, level, fmt);
                    SHR_DQ_INSERT_HEAD(&bkt_hdl->fmt_group_usage[fmt][max_count].groups,
                                       &rbkt->fmt_usgnode);
                }
            }

            for (i = 1; i < bkt_hdl->rbkts_total; i++) {
                rbkt_t *bbt = &bkt_hdl->rbkt_array[i];
                bbt->index = i;
            }
        }
    }

    LMEM_MGR_INIT(base_entries_ver0_t,
                  base_entries_lmem,
                  BASE_ENTRIES_ALLOC_CHUNK_SIZE,
                  false,
                  rv);
    if (rv != SHR_E_NONE) {
        SHR_RETURN(SHR_E_MEMORY);
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_rm_alpm_bkt_hdl_cleanup(u);
    }
    SHR_FUNC_EXIT();
}

static void
ver0_bkt_hdl_cleanup(int u)
{
    int i, j;
    int db_count;
    bkt_hdl_t *bkt_hdl;
    uint8_t fmt;

    if (base_entries_lmem) {
        shr_lmm_delete(base_entries_lmem);
        base_entries_lmem = NULL;
    }

    db_count = bcmptm_rm_alpm_database_count(u);
    for (i = 0; i < db_count; i++) {
        alpm_db_t *database_ptr;
        database_ptr = bcmptm_rm_alpm_database_get(u, i);

        /* Start from Level2 */
        for (j = LEVEL2; j < database_ptr->max_levels; j++) {
            bkt_hdl = BKT_HDL(u, i, j);
            if (bkt_hdl) {
                SHR_FREE(bkt_hdl->group_usage);
                for (fmt = VER0_FORMAT_1; fmt <= MAX_FORMAT(u, FORMAT_ROUTE);
                     fmt++) {
                    SHR_FREE(bkt_hdl->fmt_group_usage[fmt]);
                }
                SHR_FREE(bkt_hdl->fmt_group_usage);
                SHR_FREE(bkt_hdl->rbkt_array);
                SHR_FREE(bkt_hdl);
            }
        }
    }
}


static void
ver0_bucket_sanity(int u, int db, int ln, log_bkt_t log_bkt)
{
    ver0_rbkt_group_sanity(u, db, ln, VER0_GROUP(u, log_bkt));
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_bucket_ver0_register(int u, bucket_mgmt_t *bmgmt)
{
    SHR_FUNC_ENTER(u);

    if (bmgmt) {
        bmgmt->bkt_hdl_cleanup = ver0_bkt_hdl_cleanup;
        bmgmt->bkt_hdl_init = ver0_bkt_hdl_init;
        bmgmt->bkt_hdl_get = ver0_bkt_hdl_get;
        bmgmt->alpm_data_encode = ver0_alpm_data_encode;
        bmgmt->bucket_pivot_get = ver0_log_bucket_pivot_get;
        bmgmt->bucket_pivot_set = ver0_log_bucket_pivot_set;
        bmgmt->bucket_free = ver0_log_bucket_free;
        bmgmt->bucket_alloc = ver0_log_bucket_alloc;
        bmgmt->bucket_delete = ver0_bucket_delete;
        bmgmt->bucket_insert = ver0_bucket_insert;
        bmgmt->bucket_split = ver0_bucket_split;
        bmgmt->entry_write = ver0_base_entry_encode_write;
        bmgmt->entry_read = ver0_base_entry_decode_read;
        bmgmt->entry_hw_idx_get = ver0_hw_eidx_from_ent_idx;
        bmgmt->bucket_sanity = ver0_bucket_sanity;
        bmgmt->bucket_recover = ver0_bucket_recover;
        bmgmt->bucket_recover_done = ver0_bucket_recover_done;

        switch (bcmptm_rm_alpm_uft_banks(u, LEVEL2)) {
        case 2: ver0_bank_bits[u] = 1; break;
        case 4: ver0_bank_bits[u] = 2; break;
        case 8: ver0_bank_bits[u] = 3; break;
        default: SHR_RETURN(SHR_E_PARAM);
        }
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}
