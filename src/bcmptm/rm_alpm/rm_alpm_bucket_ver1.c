/*! \file rm_alpm_bucket_ver1.c
 *
 * Resource Manager for ALPM Level-N bucket
 *
 * This file contains the resource manager for ALPM Level-N bucket
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
#define RBKT_MAX_ENTRIES_VER1       16 /* The actual number is 15 */
#define BASE_ENTRIES_ALLOC_CHUNK_SIZE  2

#define RBKT_EBMP(u, _d, _l, _i)        (RBKT(u, _d, _l, _i).entry_bitmap)
#define RBKT_VBANKS(u, _d, _l, _i)      (RBKT(u, _d, _l, _i).valid_banks)
#define RBKT_FBANKS(u, _d, _l, _i)      (RBKT(u, _d, _l, _i).free_banks)
#define RBKT_FORMAT(u, _d, _l, _i)      (RBKT(u, _d, _l, _i).format)
#define RBKT_IS_HEAD(u, _d, _l, _i)     (RBKT(u, _d, _l, _i).is_head)
#define VER1_RBKT_PIVOT(u, _d, _l, _i)  (RBKT(u, _d, _l, _i).pivot)
#define VER1_LOG_BKT(_g)                (log_bkt_t)(_g)
#define VER1_GROUP(_l)                  (rbkt_group_t)(_l)
#define MAX_FORMAT(u, _p)               (smax_format[u][_p])

/*******************************************************************************
 * Typedefs
 */

typedef struct base_entries_ver1_s {
    LN_ENTRY_t entry[ALPM_MAX_BANKS][RBKT_MAX_ENTRIES_VER1];
    /*!< Pointer to the next element in the list (or NULL). */
    struct base_entries_ver1_s *next;
} base_entries_ver1_t;

/*******************************************************************************
 * Private variables
 */

static bkt_hdl_t *sbkt_hdl[BCMDRD_CONFIG_MAX_UNITS][DBS][LEVELS];
static bool max_split_len_cal = FALSE;
static int bucket_shifts[BCMDRD_CONFIG_MAX_UNITS];
/*!< Max format value  */
static int smax_format[BCMDRD_CONFIG_MAX_UNITS][FROMAT_TYPES];
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS];
static shr_lmm_hdl_t base_entries_lmem = NULL;

/*******************************************************************************
 * Private Functions (declaration)
 */
static int
ver1_rbkt_alloc(int u, int db, int ln, rbkt_group_t group, rbkt_idx_t *rbkt);
static int
ver1_rbkt_group_move(int u, int db, int ln,
                     rbkt_group_t src_group,
                     rbkt_group_t dst_group,
                     rbkt_group_t *new_group,
                     bool fuse,
                     rbkt_group_t *free_group);
static int
ver1_rbkt_group_shuffle(int u, int db,
                        int ln,
                        rbkt_group_t group,
                        uint8_t size,
                        rbkt_group_t *free_group);

static int
ver1_rbkt_group_free(int u, int db, int ln, rbkt_group_t group);

static int
ver1_bucket_recover(int u, int ln, alpm_arg_t *arg);

/*******************************************************************************
 * Private Functions (definition)
 */
static uint32_t
ver1_hw_idx_from_rbkt_idx(int u, int db, int ln, rbkt_idx_t rbkt)
{
    int max_banks;
    int bucket_bits;
    max_banks = BKT_HDL_MAX_BANKS(u, db, ln);
    bucket_bits = BKT_HDL_BUCKET_BITS(u, db, ln);
    return ((rbkt % max_banks) << bucket_bits) | (rbkt / max_banks);
}

static uint32_t
ver1_hw_eidx_from_ent_idx(int u, int db, int ln, ln_index_t ent_idx)
{
    return ver1_hw_idx_from_rbkt_idx(u, db, ln, RBKT_FROM_INDEX(ent_idx));
}

static int
ver1_bkt_hdl_get(int u, int db, int ln, bkt_hdl_t **bkt_hdl)
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

/* Get pivot for a rbkt */
static alpm_pivot_t *
ver1_rbkt_pivot_get(int u, int db, int ln, rbkt_idx_t rbkt_idx)
{
    rbkt_idx_t rbkt_idx2 = rbkt_idx;
    ALPM_ASSERT(rbkt_idx2 >= 0 && rbkt_idx2 < BKT_HDL_RBKTS_TOTAL(u, db, ln));
    while (!RBKT_IS_HEAD(u, db, ln, rbkt_idx2) && rbkt_idx2 >= 0) {
        rbkt_idx2--;
    }
    ALPM_ASSERT(rbkt_idx2 >= 0);
    ALPM_ASSERT(rbkt_idx - rbkt_idx2 + 1 <= BKT_HDL_MAX_BANKS(u, db, ln));
    return VER1_RBKT_PIVOT(u, db, ln, rbkt_idx2);
}

static alpm_pivot_t *
ver1_bucket_pivot_get(int u, int db, int ln, ln_index_t ln_index)
{
    rbkt_idx_t rbkt_idx;

    rbkt_idx = RBKT_FROM_INDEX(ln_index);
    return ver1_rbkt_pivot_get(u, db, ln, rbkt_idx);
}

/* Set pivot for a rbkt */
static inline void
ver1_rbkt_pivot_set(int u, int db, int ln, rbkt_group_t group,
                    alpm_pivot_t *pivot)
{
    VER1_RBKT_PIVOT(u, db, ln, group) = pivot;
}

/*!
 * \brief Calculate the max length for new split point.
 *
 * \param [in] u Device u.
 * \param [in] pivot Pivot to be split.
 *
 * \return Max split length
 */
static int
ver1_trie_msl(int u, alpm_pivot_t *pivot)
{
    shr_dq_t *listptr;
    shr_dq_t *elem;
    alpm_bkt_elem_t *curr;
    int max_key_len = 0;

    if (!max_split_len_cal) {
        return 1024;
    }

    listptr = &pivot->bkt->list;
    SHR_DQ_TRAVERSE(listptr, elem)
        curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        if (max_key_len < curr->key.t.len) {
            max_key_len = curr->key.t.len;
        }
    SHR_DQ_TRAVERSE_END(listptr, elem);

    return (pivot->key.t.len + max_key_len + 1) / 2;
}

/*!
 * \brief Calculate the max count for new split point.
 *
 * \param [in] u Device u.
 * \param [in] pivot Pivot to be split.
 *
 * \return Max split count
 */
static inline int
ver1_trie_msc(int u, int ln, alpm_pivot_t *pivot)
{
    return 0xFFFF;
}
/*!
 * \brief Encode Level-1 ALPM data
 *
 * \param [in] u Device u.
 * \param [in] ln Database
 * \param [in] ln Level
 * \param [in] default_route Is default route
 * \param [in] group Rbkt group
 * \param [in] len Pivot length
 * \param [out] a1 ALPM data for level1
 *
 * \return nothing
 */
static void
ver1_alpm1_data_encode(int u, int db, int ln, int default_route,
                       rbkt_group_t group,
                       int len, alpm1_data_t *a1)
{
    int i;
    a1->direct_route = 0; /* to skip Level-1  pivot mgmt */
    a1->default_miss = default_route; /* REVISIT */
    a1->phy_bkt = group / BKT_HDL_MAX_BANKS(u, db, ln);
    a1->rofs = group % BKT_HDL_MAX_BANKS(u, db, ln);
    a1->kshift = len;
    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        a1->fmt[i] = RBKT_FORMAT(u, db, ln, group + i);
    }
    while (i < ALPM1_DATA_FORMATS) {
        ALPM_ASSERT(a1->fmt[i] == DISABLED_FORMAT);
        i++;
    }
}

/*!
 * \brief Encode Level-2 ALPM data
 *
 * \param [in] u Device u.
 * \param [in] ln Database
 * \param [in] ln Level
 * \param [in] default_route Is default route
 * \param [in] group Rbkt group
 * \param [in] len Pivot length
 * \param [out] a2 ALPM data for level2
 *
 * \return nothing
 */
static void
ver1_alpm2_data_encode(int u, int db, int ln, int default_route,
                       rbkt_group_t group,
                       int len, alpm2_data_t *a2)
{
    int i;
    a2->default_miss = default_route; /* REVISIT */
    a2->phy_bkt = group / BKT_HDL_MAX_BANKS(u, db, ln);
    a2->rofs = group % BKT_HDL_MAX_BANKS(u, db, ln);
    a2->kshift = len;
    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        a2->fmt[i] = RBKT_FORMAT(u, db, ln, group + i);
    }
    while (i < ALPM1_DATA_FORMATS) {
        ALPM_ASSERT(a2->fmt[i] == DISABLED_FORMAT);
        i++;
    }
}

/*!
 * \brief Migrate entry indexs from one src_rbkt to dst_rbkt within a group.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] bkt Pivot bucket
 * \param [in] srbkt_idx Source rbkt index
 * \param [in] drbkt_idx Destination rbkt index
 *
 * \return SHR_E_NONE, SHR_E_XXX.
 */
static int
ver1_rbkt_entry_index_migrate(int u, int db, int ln,
                              elem_bkt_t *bkt,
                              rbkt_idx_t srbkt_idx,
                              rbkt_idx_t drbkt_idx,
                              int *map_ent_ofs)
{
    alpm_bkt_elem_t *curr;
    shr_dq_t *elem = NULL;
    int dst_ent_ofs;
    ln_index_t index;
    SHR_FUNC_ENTER(u);

    LOG_V((BSL_META("Migrate: ")));
    SHR_DQ_TRAVERSE(&bkt->list, elem)
        curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        index = curr->index;
        if (RBKT_FROM_INDEX(index) == srbkt_idx) {
            if (map_ent_ofs) {
                dst_ent_ofs = map_ent_ofs[ENT_OFS_FROM_IDX(index)];
            } else {
                dst_ent_ofs = ENT_OFS_FROM_IDX(index);
            }
            curr->index = ALPM_INDEX_ENCODE_VER1(drbkt_idx, dst_ent_ofs);
            LOG_V((BSL_META("[%d -> %d]"), index, curr->index));
        }
    SHR_DQ_TRAVERSE_END(&bkt->list, elem);
    LOG_V((BSL_META("\n")));

    SHR_FUNC_EXIT();
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
static inline void
ver1_rbkt_group_free_bank_update(int u, int db, int ln, rbkt_group_t group,
                                 int diff)
{
    int free_count;

    RBKT_FBANKS(u, db, ln, group) += diff;
    free_count = RBKT_FBANKS(u, db, ln, group);
    ALPM_ASSERT(free_count >= 0);
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, db, ln, group));
    free_count = ALPM_MIN(BKT_HDL_MAX_BANKS(u, db, ln), free_count);
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
static inline void
ver1_rbkt_group_free_bank_set(int u, int db, int ln, rbkt_group_t group,
                              uint32_t free_count)
{
    RBKT_FBANKS(u, db, ln, group) = free_count;
    free_count = ALPM_MIN(BKT_HDL_MAX_BANKS(u, db, ln), free_count);
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
static inline void
ver1_rbkt_group_free_bank_clr(int u, int db, int ln, rbkt_group_t group)
{
    RBKT_FBANKS(u, db, ln, group) = 0;
    SHR_DQ_REMOVE(&RBKT_USG_NODE(u, db, ln, group));
}

/*!
 * \brief Expand the existing group by one rbkt left.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] prev_group Previous group with at least one free rbkt
 * \param [in] group Group to expand.
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_expand_left(int u, int db, int ln,
                      rbkt_group_t prev_group,
                      rbkt_group_t group)
{
    alpm_pivot_t *pivot;
    rbkt_idx_t rbkt = group - 1;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, group) == HEAD);
    /*     ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, rbkt) == BODY); */
    ALPM_ASSERT(RBKT_EBMP(u, db, ln, rbkt) == 0);

    /* Setup shrinking group (prev) */
    ver1_rbkt_group_free_bank_update(u, db, ln, prev_group, -1);

    /* Setup new start (curr) */
    RBKT_VBANKS(u, db, ln, rbkt) = RBKT_VBANKS(u, db, ln, group) + 1;
    ver1_rbkt_group_free_bank_set(u, db, ln, rbkt, RBKT_FBANKS(u, db, ln, group));
    RBKT_FORMAT(u, db, ln, rbkt) = DEFAULT_FORMAT;
    VER1_RBKT_PIVOT(u, db, ln, rbkt)  = VER1_RBKT_PIVOT(u, db, ln, group);
    if (rbkt == 0) {
        /* Do not swap if new head is group 0, as it's already in */
        SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, db, ln, group));
    } else {
        SHR_DQ_SWAP(&RBKT_LIST_NODE(u, db, ln, group),
                    &RBKT_LIST_NODE(u, db, ln, rbkt));
    }
    RBKT_IS_HEAD(u, db, ln, rbkt) = RBKT_IS_HEAD(u, db, ln, group);
    pivot = VER1_RBKT_PIVOT(u, db, ln, rbkt);
    /* Update new group to the pivot */
    pivot->bkt->log_bkt = VER1_LOG_BKT(rbkt);
    ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, rbkt) == HEAD);
    BKT_HDL_RBKTS_USED(u, db, ln)++;

    /* Setup old start (curr) */
    RBKT_VBANKS(u, db, ln, group) = 0;
    ver1_rbkt_group_free_bank_clr(u, db, ln, group);
    RBKT_IS_HEAD(u, db, ln, group) = BODY;
    VER1_RBKT_PIVOT(u, db, ln, group) = NULL;

    /* Update pivot as ROFS changed */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update(u, db, PREV_LEVEL(ln), pivot));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Expand the existing group by one rbkt right.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to expand.
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_expand_right(int u, int db, int ln, rbkt_group_t group)
{
    rbkt_idx_t rbkt = RBKT_VBANKS(u, db, ln, group) + group;
    SHR_FUNC_ENTER(u);

    RBKT_VBANKS(u, db, ln, group) += 1;
    BKT_HDL_RBKTS_USED(u, db, ln)++;
    RBKT_FORMAT(u, db, ln, rbkt) = DEFAULT_FORMAT;
    /* VER1_RBKT_PIVOT(u, db, ln, rbkt)  = VER1_RBKT_PIVOT(u, db, ln, group); */
    ver1_rbkt_group_free_bank_update(u, db, ln, group, -1);

    /* No need to update pivot for right expand */
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy a rbkt from one position to another
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] sbkt_idx Source rbkt index
 * \param [in] dbkt_idx Destination rbkt index
 * \param [in] update_index Migrate the index
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */

static int
ver1_rbkt_copy(int u, int db, int ln,
               rbkt_idx_t srbkt_idx,
               rbkt_idx_t drbkt_idx,
               bool update_index)
{
    LN_ENTRY_t rbkt;
    rbkt_t *src_rbkt, *dst_rbkt;
    alpm_pivot_t *pivot;
    int shw_idx, dhw_idx;
    SHR_FUNC_ENTER(u);

    shw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, srbkt_idx);
    dhw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, drbkt_idx);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, shw_idx, &rbkt[0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, db, ln, dhw_idx, &rbkt[0]));

    src_rbkt = &RBKT(u, db, ln, srbkt_idx);
    dst_rbkt = &RBKT(u, db, ln, drbkt_idx);
    dst_rbkt->entry_bitmap = src_rbkt->entry_bitmap;
    dst_rbkt->format = src_rbkt->format;
    /* dst_rbkt->pivot  = src_rbkt->pivot; */
    /* ALPM_ASSERT(dst_rbkt->pivot); */

    if (update_index) {
        pivot = ver1_rbkt_pivot_get(u, db, ln, drbkt_idx);
        ALPM_ASSERT(pivot);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_entry_index_migrate(u, db, ln, pivot->bkt,
                                          srbkt_idx,
                                          drbkt_idx,
                                          NULL));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Allocate a rbkt for existing group.
 *
 * Basically, this is to expand an existing group by one rbkt.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to expand.
 * \param [out] rbkt New rbkt allocated.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_rbkt_alloc(int u, int db, int ln,
                rbkt_group_t group,
                rbkt_idx_t *rbkt)
{
    int max_banks;
    bkt_hdl_t *bkt_hdl;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        ver1_bkt_hdl_get(u, db, ln, &bkt_hdl));

    max_banks = bkt_hdl->max_banks;
    if (RBKT_VBANKS(u, db, ln, group) == max_banks) {
        /* Already a full group, this is where split should kick in */
        SHR_RETURN(SHR_E_FULL);
    }
    ALPM_ASSERT(RBKT_VBANKS(u, db, ln, group) < max_banks);
    if (bkt_hdl->rbkts_total == bkt_hdl->rbkts_used) {
        SHR_RETURN(SHR_E_FULL);
    }

    if (RBKT_FBANKS(u, db, ln, group) != 0) {
        ALPM_ASSERT(0);
        /* Free rbkts available, just take one */
        *rbkt = group + RBKT_VBANKS(u, db, ln, group);
        RBKT_VBANKS(u, db, ln, group)++;
        ver1_rbkt_group_free_bank_update(u, db, ln, group, -1);
        RBKT_FORMAT(u, db, ln, *rbkt) = DEFAULT_FORMAT;
        ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, *rbkt) == BODY);
        ALPM_ASSERT(RBKT_EBMP(u, db, ln, *rbkt) == 0);
    } else {
        rbkt_t *curr;
        uint32_t req_banks = RBKT_VBANKS(u, db, ln, group) + 1;
        int forward_group = 0;
        int reverse_group = 0;
        int entire_move = 0;
        shr_dq_t *elem = NULL;
        shr_dq_t *start;
        rbkt_group_t prev_group = INVALID_GROUP;
        rbkt_group_t dest_group = INVALID_GROUP;
        rbkt_group_t free_group, new_group;

        /* Choices are:
         * - Check prev group if there is free piece to take.
         *   - Expand group by 1 rbkt left.
         * - Move the target group into another bigger place.
         *   - Group alloc & free
         * - Light shuffle
         *   - Group Alloc & Free
         * - Heavy shuffle
         *   - Expand by 1 left or right.
         *
         * The order above represents the priority of each choice.
         * The sequence is created by, for each choice, evaluating the number
         * of groups to be moved. Goal is to minimize the groups to be moved.
         */

        /* Reverse traverse first! The previous adjcent has an advantage. */
        start = SHR_DQ_PREV(&RBKT_LIST_NODE(u, db, ln, group), shr_dq_t *);
        SHR_DQ_BACK_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, db, ln),
                                  start, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
            ALPM_ASSERT(curr);
            reverse_group++;
            if (reverse_group == 1 && curr->free_banks > 0) {
                prev_group = curr->index;
                break;
                /* Continue to seek entrie move as that's preferred. */
            } else if (curr->free_banks >= req_banks) {
                /* Exists bigger place to move entire group */
                entire_move = 1;
                dest_group = curr->index;
                break;
            }
        SHR_DQ_TRAVERSE_END(list, elem);

        if (prev_group != INVALID_GROUP || entire_move) {
            /* choice 1 or 2 are good to go */
        } else {
            /* Check another direction for choice 1 or 2 */
            start = SHR_DQ_NEXT(&RBKT_LIST_NODE(u, db, ln, group), shr_dq_t *);
            SHR_DQ_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, db, ln),
                                 start, elem)
                curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
                ALPM_ASSERT(curr);
                forward_group++;
                if (curr->free_banks >= req_banks) {
                    /* Exists bigger place to move entire group */
                    entire_move = 1;
                    dest_group = curr->index;
                    break;
                }
            SHR_DQ_TRAVERSE_END(list, elem);
        }

        if (prev_group != INVALID_GROUP) { /* Expand*/
            SHR_IF_ERR_EXIT(
                ver1_rbkt_expand_left(u, db, ln, prev_group, group));
            *rbkt = group - 1;
        } else if (dest_group != INVALID_GROUP) {
            /* Move entire group */
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_move(u, db, ln,
                                     group,
                                     dest_group,
                                     &new_group,
                                     TRUE,
                                     NULL));
            *rbkt = RBKT_VBANKS(u, db, ln, new_group) + new_group;
            SHR_IF_ERR_EXIT(
                ver1_rbkt_expand_right(u, db, ln, new_group));
        } else {
            int size = RBKT_VBANKS(u, db, ln, group) + 1;
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_shuffle(u, db, ln, group, size, &free_group));

            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_move(u, db, ln,
                                     group,
                                     free_group,
                                     &new_group,
                                     TRUE,
                                     NULL));
            *rbkt = RBKT_VBANKS(u, db, ln, new_group) + new_group;
            SHR_IF_ERR_EXIT(
                ver1_rbkt_expand_right(u, db, ln, new_group));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a rbkt for existing group.
 *
 * Basically, this is to shrink an existing group by one rbkt.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to shrink.
 * \param [in] rbkt Rbkt to be freed.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_rbkt_free(int u, int db, int ln,
               rbkt_group_t group,
               rbkt_idx_t rbkt_idx)
{
    SHR_FUNC_ENTER(u);

    RBKT_VBANKS(u, db, ln, group)--;
    BKT_HDL_RBKTS_USED(u, db, ln)--;
    ver1_rbkt_group_free_bank_update(u, db, ln, group, +1);
    /* Let it changed. */
    RBKT_FORMAT(u, db, ln, rbkt_idx) = DISABLED_FORMAT;
    /*  don't reset this for ver1_rbkt_free, since this is head property
     * VER1_RBKT_PIVOT(u, db, ln, rbkt_idx) = NULL;
     */
    RBKT_EBMP(u, db, ln, rbkt_idx) = 0;
    /*  RBKT_IS_HEAD(u, db, ln, rbkt_idx) = BODY; */
    /* ln_stats_incr(c_bankfree); */

    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear a rbkt in HW, and recycle SW resoures.
 *
 * Do not reset the format, as that is not necessary.
 * And do not bother taking care of group vbanks.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] size Requesting size for new group
 * \param [out] free_group Group with free banks of requesting size
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_clear(int u, int db, int ln, int rbkt_idx)
{
    int hw_idx;
    SHR_FUNC_ENTER(u);

    hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_clear_ln_raw(u, db, ln, hw_idx));

    /*    RBKT_FORMAT(u, db, ln, rbkt_idx) = DISABLED_FORMAT; */
    RBKT_EBMP(u, db, ln, rbkt_idx) = 0;
    /* VER1_RBKT_PIVOT(u, db, ln, rbkt_idx) = NULL;
     * Also don't reset for same reason as ver1_rbkt_free
     */
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Merge 2 rbkts within same group into one.
 *
 * More options: 3->2.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_rbkt_idx Source rbkt index
 * \param [in] dst_rbkt_idx Destination rbkt index
 * \param [in] group Rbkt group for both src and dst.
 * \param [out] merged True if merge happens
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */

static int
ver1_rbkt_merge(int u, int db, int ln,
                rbkt_idx_t srbkt_idx,
                rbkt_idx_t drbkt_idx,
                rbkt_group_t group,
                bool *merged)
{
    int i, j;
    LN_ENTRY_t src_rbkt_buf, dst_rbkt_buf;
    rbkt_t *src_rbkt, *dst_rbkt;
    format_type_t format_type;
    int src_prefix_len, src_full, src_total;
    int dst_prefix_len, dst_full, dst_total;
    bool ok_to_merge = FALSE;
    int src_num, dst_num;
    alpm_pivot_t *pvt;
    int ent_ofs_map[RBKT_MAX_ENTRIES_VER1];
    int shw_idx, dhw_idx;

    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(srbkt_idx != drbkt_idx);
    src_rbkt = &RBKT(u, db, ln, srbkt_idx);
    dst_rbkt = &RBKT(u, db, ln, drbkt_idx);

    if (src_rbkt->entry_bitmap == 0 ||
        dst_rbkt->entry_bitmap == 0) {
        /* Do not merge empty rbkts, leave them the last thing to do */
        SHR_EXIT();
    }

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                          src_rbkt->format,
                                          &src_prefix_len,
                                          &src_full,
                                          &src_total));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                          dst_rbkt->format,
                                          &dst_prefix_len,
                                          &dst_full,
                                          &dst_total));
    
    if (dst_full >= src_full && dst_prefix_len >= src_prefix_len) {
        src_num = shr_util_popcount(RBKT_EBMP(u, db, ln, srbkt_idx));
        dst_num = shr_util_popcount(RBKT_EBMP(u, db, ln, drbkt_idx));
        if (src_num + dst_num <= dst_total) {
            /* Do merge only if can save a rbkt => merged = 1. */
            ok_to_merge = TRUE;
        }
    }
    /*
     * Try opposite direction: this moves prefix to less efficient format.
     * else {
     *     SHR_RETURN(
     *        ver1_rbkt_merge(u, db, ln, drbkt_idx, srbkt_idx, group, merged));
     * }
     */

    if (!ok_to_merge) {
        SHR_EXIT();
    }

    *merged = TRUE;

    /* Do merge 2 -> 1 */

    shw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, srbkt_idx);
    dhw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, drbkt_idx);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, shw_idx, &src_rbkt_buf[0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, dhw_idx, &dst_rbkt_buf[0]));

    j = 0;
    pvt = VER1_RBKT_PIVOT(u, db, ln, group);
    for (i = 0; i < dst_total; i++) {
        if (ALPM_BIT_GET(dst_rbkt->entry_bitmap, i)) {
            continue;
        }
        /* Don't reset j for performance */
        while (j < src_total) {
            if (ALPM_BIT_GET(src_rbkt->entry_bitmap, j)) {
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_ln_base_entry_move(u, format_type,
                                                      src_rbkt->format,
                                                      j,
                                                      &pvt->key.t,
                                                      &src_rbkt_buf[0],

                                                      dst_rbkt->format,
                                                      i,
                                                      &pvt->key.t,
                                                      &dst_rbkt_buf[0]));
                ALPM_BIT_CLR(src_rbkt->entry_bitmap, j);
                ALPM_BIT_SET(dst_rbkt->entry_bitmap, i);
                ent_ofs_map[j] = i;
                j++;
                break; /* This dst entry is done, avdance to next */
            }
            j++;
        }
    }
    /* Source rbkt should be empty now */
    ALPM_ASSERT(src_rbkt->entry_bitmap == 0);
    ALPM_ASSERT(src_num + dst_num == shr_util_popcount(dst_rbkt->entry_bitmap));

    /* Write destination rbkt then source rbkt to preserve traffic */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, db, ln, dhw_idx, &dst_rbkt_buf[0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_write_ln_raw(u, db, ln, shw_idx, &src_rbkt_buf[0]));

    SHR_IF_ERR_EXIT(
        ver1_rbkt_free(u, db, ln, group, srbkt_idx));
    /* $ Entry index migration */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_entry_index_migrate(u, db, ln,
                                      VER1_RBKT_PIVOT(u, db, ln, group)->bkt,
                                      srbkt_idx,
                                      drbkt_idx,
                                      ent_ofs_map));

exit:
    SHR_FUNC_EXIT();
}

static void
ver1_rbkt_group_sanity(int u, int db, int ln, rbkt_group_t group,
                       bool with_holes, int vbanks)
{
    rbkt_t *rbkt_group, *rbkt;
    bkt_hdl_t *bkt_hdl;
    shr_dq_t *blist, *elem;
    alpm_bkt_elem_t *curr;
    int bkt_count[ALPM_MAX_BANKS] = {0};
    rbkt_idx_t rbkt_idx;
    format_type_t format_type;
    int fmt_len;
    int num_entry;
    int count = 0, i;
    alpm_pivot_t *pivot = NULL;

    if (!LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        return;
    }
    ver1_bkt_hdl_get(u, db, ln, &bkt_hdl);
    if (group == INVALID_GROUP) {
        return;
    }
    rbkt_group = &bkt_hdl->rbkt_array[group];

    assert(rbkt_group->valid_banks <= bkt_hdl->max_banks);
    if (!with_holes) {
        vbanks = rbkt_group->valid_banks;
    }

    for (i = 0; i < vbanks; i++) {
        rbkt = rbkt_group + i;
        if (i == 0) {
            assert(rbkt->is_head);
            pivot = rbkt->pivot;
            assert(pivot && pivot->bkt);
            assert(VER1_GROUP(pivot->bkt->log_bkt) == rbkt_group->index);
        } else {
            assert(!rbkt->is_head);
            pivot = rbkt->pivot;
            assert(!pivot);
        }
        if (!with_holes) {
            assert(rbkt->entry_bitmap);
        }
        count += shr_util_popcount(rbkt->entry_bitmap);
    }
    assert(!pivot || bcmptm_rm_alpm_trie_count(pivot->bkt->trie) == count);

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    if (!rbkt_group->pivot) {
        return;
    }
    blist = &rbkt_group->pivot->bkt->list;
    SHR_DQ_TRAVERSE(blist, elem)
        curr = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        if (curr->index == INVALID_INDEX) {
            continue;
        }
        rbkt_idx = RBKT_FROM_INDEX(curr->index);
        rbkt = &bkt_hdl->rbkt_array[rbkt_idx];
        bcmptm_rm_alpm_ln_base_entry_info(u, format_type, rbkt->format,
                                          &fmt_len, NULL, NULL);
        assert(curr->key.t.len - rbkt_group->pivot->key.t.len <= fmt_len);
        bkt_count[rbkt_idx - group]++;
    SHR_DQ_TRAVERSE_END(blist, elem);

    for (i = 0; i < rbkt_group->valid_banks; i++) {
        rbkt = &bkt_hdl->rbkt_array[group + i];
        bcmptm_rm_alpm_ln_base_entry_info(u, format_type, rbkt->format,
                                          NULL, NULL, &num_entry);
        assert(bkt_count[i] <= num_entry);
    }
}

/*!
 * \brief Compress a group contains scattered rbkts with sparse entries.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Rbkt group to compress.
 * \param [in] vbanks Vbanks including holes
 * \param [out] format_update True if update pivot format, eles false.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_rbkt_group_compress(int u, int db, int ln,
                         rbkt_group_t group,
                         int vbanks,
                         bool *format_update)
{
    rbkt_idx_t src_rbkt, dst_rbkt;
    int j, i;
    bool merged;
    SHR_FUNC_ENTER(u);

    ver1_rbkt_group_sanity(u, db, ln, group, true, vbanks);

    j = 0;
    for (i = vbanks - 1; i > 0; i--) {
        src_rbkt = group + i;
        if (!RBKT_EBMP(u, db, ln, src_rbkt)) {
            continue;
        }
        for (j = 0; j < i; j++) {
            dst_rbkt = group + j;
            if (!RBKT_EBMP(u, db, ln, dst_rbkt)) {
                continue;
            }
            merged = false;
            SHR_IF_ERR_EXIT(
                ver1_rbkt_merge(u, db, ln, src_rbkt, dst_rbkt, group,
                                &merged));
            if (merged) { /* src_rbkt is gone, advance to next  */
                *format_update = true;
                break;
            }
        }
    }

    j = 0;
    for (i = vbanks - 1; i > j; i--) {
        src_rbkt = group + i;
        if (!RBKT_EBMP(u, db, ln, src_rbkt)) {
            continue;
        }
        while (j < i) {
            dst_rbkt = group + j;
            if (!RBKT_EBMP(u, db, ln, dst_rbkt)) {
                SHR_IF_ERR_EXIT(
                    ver1_rbkt_copy(u, db, ln, src_rbkt, dst_rbkt, true));
                j++;
                *format_update = true;
                break; /* Advance to next dst_rbkt */
            }
            j++;
            /*
             * Rbkts just moved, not freed thus don't call ver1_rbkt_free.
             * Original rbkts' will be cleared (ver1_rbkt_clear) after
             * pivot update.
             */
        }
    }

    /* Right here, if new entry belongs to parent pivot, it is
     * to be added later.
     * ver1_rbkt_group_sanity(u, db, ln, group, false, 0);
     */
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Split a new group from the free group.
 *
 * The target group should be with enough free rbkts, thus no shuffle involved.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] free_group Free group
 * \param [in] size Requesting size for new group
 * \param [out] new_group New group split
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_split(int u, int db, int ln,
                      rbkt_group_t free_group,
                      uint8_t size,
                      rbkt_group_t *new_group)
{
    rbkt_t *free_rbkt;
    rbkt_t *new_rbkt;
    SHR_FUNC_ENTER(u);

    free_rbkt = &RBKT(u, db, ln, free_group);
    /* If no valid banks, must be group[0].
     * Update: not true any more, as we may have pivot with assigned group with
     * all free rbkts. This is the case when we cannot free that pivot (usually
     * level-2), because otherwise we may endup with ill insert (find wrong
     * second level pivot).
     */
    /* ALPM_ASSERT(free_rbkt->valid_banks || free_rbkt->index == 0); */
    ALPM_ASSERT(size <= free_rbkt->free_banks);

    /* Setup split group */
    new_rbkt = &RBKT(u, db, ln, (free_rbkt->index +
                                 free_rbkt->valid_banks +
                                 free_rbkt->free_banks - size));
    if (new_rbkt->index != free_rbkt->index) {
        new_rbkt->is_head = HEAD;
        ver1_rbkt_group_free_bank_set(u, db, ln, new_rbkt->index, size);

        /* Setup original group */
        ver1_rbkt_group_free_bank_update(u, db, ln, free_rbkt->index, -size);

        /* Link the new and free2 */
        SHR_DQ_INSERT_NEXT(&free_rbkt->listnode, &new_rbkt->listnode);
        BKT_HDL_GROUPS_USED(u, db, ln)++;
    } else {
        /* Else take all, and this group is already in the list */
        alpm_pivot_t *original_pivot;
        original_pivot = ver1_rbkt_pivot_get(u, db, ln, free_rbkt->index);
        if (original_pivot) {
            /* This group is now officially taken away */
            original_pivot->bkt->log_bkt = INVALID_LOG_BKT;
        } else {
            ALPM_ASSERT(free_rbkt->index == 0);
        }
    }

    *new_group = new_rbkt->index;
    SHR_FUNC_EXIT();
}


/*!
 * \brief Fuse an existing group with its previous group.
 *
 * Assuming the group is not used any more (empty or others).
 * Merge the resources into previous group. Reset everything
 * in the group, and remove it from the list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Group to be fused
 * \param [out] free_group Group absorbing all resources of target group
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_fuse(int u, int db, int ln,
                     rbkt_group_t group,
                     rbkt_group_t *free_group)
{
    rbkt_t *prev;
    shr_dq_t *dq;
    int i;
    int prev_group;
    SHR_FUNC_ENTER(u);

    /* Cannot fuse for the first group */
    ALPM_ASSERT(group != 0 || group != INVALID_GROUP);

    dq = SHR_DQ_PREV(&RBKT_LIST_NODE(u, db, ln, group), shr_dq_t*);
    prev = SHR_DQ_ELEMENT_GET(rbkt_t*, dq, listnode);
    prev_group = prev->index;

    ver1_rbkt_group_free_bank_update(u, db, ln, prev_group,
                               RBKT_FBANKS(u, db, ln, group) +
                               RBKT_VBANKS(u, db, ln, group));

    for (i = 1; i < RBKT_VBANKS(u, db, ln, group); i++) {
        rbkt_idx_t rbkt_idx = group + i;
        RBKT_FORMAT(u, db, ln, rbkt_idx) = DISABLED_FORMAT;
        RBKT_EBMP(u, db, ln, rbkt_idx) = 0;
    }
    RBKT_VBANKS(u, db, ln, group) = 0;
    ver1_rbkt_group_free_bank_clr(u, db, ln, group);
    RBKT_IS_HEAD(u, db, ln, group) = BODY;
    VER1_RBKT_PIVOT(u, db, ln, group) = NULL;
    SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, db, ln, group));
    BKT_HDL_GROUPS_USED(u, db, ln)--;

    if (free_group) {
        *free_group = prev_group;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Move entrie group to the destination location
 *
 * The src and dst locations are guaranteed not to be overlapped.
 * We only move the valid banks in the move group, and leave those free banks.
 * When fuse is set, the src group is fused with its previous group (if exists),
 * and the free group is the fused one (previous group).
 * When fuse is not set, the src group simply becomes an empty group, and the
 * free group is itself.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] src_group Group to be moved
 * \param [in] dst_group Group to spare *all* free rbkts to src_group.
 * \param [out] new_group New group after moved.
 * \param [in] fuse To fuse the src_group with its previous group.
 * \param [out] free_group Group with free banks increased.
 *
 * \return SHR_E_NONE, SHR_E_XXX.
 */
static int
ver1_rbkt_group_move(int u, int db, int ln,
                     rbkt_group_t src_group,
                     rbkt_group_t dst_group,
                     rbkt_group_t *new_group,
                     bool fuse,
                     rbkt_group_t *free_group)
{
    uint32_t i, vbanks, fbanks;
    alpm_pivot_t *pivot = VER1_RBKT_PIVOT(u, db, ln, src_group);
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(pivot->bkt);
    ALPM_ASSERT(pivot->bkt->log_bkt == VER1_LOG_BKT(src_group));
    ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, src_group) == HEAD);
    ALPM_ASSERT(RBKT_IS_HEAD(u, db, ln, dst_group) == HEAD);
    /* Dst group must be with enough free banks to hold src group */
    ALPM_ASSERT(RBKT_FBANKS(u, db, ln, dst_group) >=
                RBKT_VBANKS(u, db, ln, src_group));
    fbanks = RBKT_FBANKS(u, db, ln, dst_group);

    /* $ Give away all the free banks of dst_group */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_split(u, db, ln, dst_group,
                              RBKT_FBANKS(u, db, ln, dst_group),
                              new_group));
    ALPM_ASSERT(*new_group ==
                RBKT_VBANKS(u, db, ln, dst_group) + dst_group);

    /* $ Update new group to the pivot before pivot update */
    pivot->bkt->log_bkt = VER1_LOG_BKT(*new_group);
    VER1_RBKT_PIVOT(u, db, ln, *new_group) = pivot;
    vbanks = RBKT_VBANKS(u, db, ln, src_group);
    ALPM_ASSERT(vbanks); /* Why move empty group */
    RBKT_VBANKS(u, db, ln, *new_group) = vbanks;
    /* During group_split, new_group has been given the free banks */
    ALPM_ASSERT(RBKT_FBANKS(u, db, ln, *new_group) >= vbanks);
    ver1_rbkt_group_free_bank_update(u, db, ln, *new_group, -vbanks);

    /* $ Copy bucket */
    for (i = 0; i < vbanks; i++) {
        /* Clear old group after pivot updated.
         * Update the entry_index.
         * ver1_rbkt_copy need pivot set in place for new group.
         */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_copy(u, db, ln, src_group + i, *new_group + i, TRUE));
    }

    /* $ Update PIVOT */
    ALPM_ASSERT(pivot);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update(u, db, PREV_LEVEL(ln), pivot));

    /* $ Invalidate old rbkts */
    for (i = 0; i < vbanks; i++) {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_clear(u, db, ln, src_group + i));
    }
    /* $ The recycle for original postion of move group. */
    /* The move group does not get deleted, it simply becomes empty! */
    if (fuse && src_group != 0) {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_fuse(u, db, ln, src_group, free_group));
    } else if (src_group != 0) {
        /* This is where src_group and dst_group are adjcent two groups */
        ALPM_ASSERT(*new_group + (int)fbanks == src_group);
        vbanks = RBKT_VBANKS(u, db, ln, src_group);
        fbanks = RBKT_FBANKS(u, db, ln, src_group);
        for (i = 1; i < vbanks; i++) {
            rbkt_idx_t rbkt_idx = src_group + i;
            RBKT_FORMAT(u, db, ln, rbkt_idx) = DISABLED_FORMAT;
            RBKT_EBMP(u, db, ln, rbkt_idx) = 0;
        }
        ver1_rbkt_group_free_bank_update(u, db, ln, *new_group, vbanks + fbanks);
        do {
            RBKT_VBANKS(u, db, ln, src_group) = 0;
            ver1_rbkt_group_free_bank_clr(u, db, ln, src_group);
            RBKT_IS_HEAD(u, db, ln, src_group) = BODY;
            VER1_RBKT_PIVOT(u, db, ln, src_group) = NULL;
            SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, db, ln, src_group));
            BKT_HDL_GROUPS_USED(u, db, ln)--;
        } while (0);
        if (free_group) {
            *free_group = *new_group;
        }
    } else {
        /* All groups except group 0 always have valid banks */
        ALPM_ASSERT(src_group == 0);
        vbanks = RBKT_VBANKS(u, db, ln, src_group);
        for (i = 1; i < vbanks; i++) {
            rbkt_idx_t rbkt_idx = src_group + i;
            RBKT_FORMAT(u, db, ln, rbkt_idx) = DISABLED_FORMAT;
            RBKT_EBMP(u, db, ln, rbkt_idx) = 0;
        }
        ver1_rbkt_group_free_bank_update(u, db, ln, src_group, vbanks);
        RBKT_VBANKS(u, db, ln, src_group) = 0;
        VER1_RBKT_PIVOT(u, db, ln, src_group) = NULL;
        /*
         * Don't do the following, as we never free group0.
         * RBKT_IS_HEAD(u, db, ln, src_group) = BODY;
         * SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, db, ln, src_group));
         */
        if (free_group) {
            *free_group = src_group;
        }
    }

    /* group_move++; */

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Move entrie group to append the previous group.
 *
 * The src and dst group are adjacent two groups.
 * It is possible that after being appended there are rbkts overlapped with
 * rbkts before being appended.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] dst_group Group to spare *all* free rbkts to src_group.
 * \param [in] src_group Group to be moved
 * \param [out] new_group New group after moved.
 *
 * \return SHR_E_NONE, SHR_E_XXX.
 */
static int
ver1_rbkt_group_append(int u, int db, int ln,
                       rbkt_group_t dst_group, /* prev group */
                       rbkt_group_t src_group,
                       rbkt_group_t *new_group
                       )
{
    int vbanks, fbanks;
    rbkt_idx_t srbkt_idx, drbkt_idx;
    alpm_pivot_t *pivot = VER1_RBKT_PIVOT(u, db, ln, src_group);
    static int append;
    SHR_FUNC_ENTER(u);
    ALPM_ASSERT(RBKT_FBANKS(u, db, ln, dst_group)); /* Otherwise why append. */
    if (RBKT_FBANKS(u, db, ln, dst_group) >=
        RBKT_VBANKS(u, db, ln, src_group)) {
        rbkt_group_t free_group;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_move(u, db, ln,
                                 src_group,
                                 dst_group,
                                 new_group,
                                 false,
                                 &free_group));
        ALPM_ASSERT(free_group == *new_group);
        SHR_EXIT();
    }

    fbanks = RBKT_FBANKS(u, db, ln, dst_group);
    /* $ Give away all the free banks of dst_group */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_split(u, db, ln, dst_group,
                              RBKT_FBANKS(u, db, ln, dst_group),
                              new_group));
    ALPM_ASSERT(*new_group ==
                RBKT_VBANKS(u, db, ln, dst_group) + dst_group);

    /* $ Update new group to the pivot before pivot update */
    pivot->bkt->log_bkt = VER1_LOG_BKT(*new_group);
    VER1_RBKT_PIVOT(u, db, ln, *new_group) = pivot;
    vbanks = RBKT_VBANKS(u, db, ln, src_group);
    ALPM_ASSERT(vbanks); /* Why move empty group */
    RBKT_VBANKS(u, db, ln, *new_group) = vbanks;
    /* Add previously possessed free banks */

    /* $ Copy bucket */
    drbkt_idx = *new_group;
    srbkt_idx = src_group + vbanks - 1;
    while (drbkt_idx < srbkt_idx && drbkt_idx < src_group) {
        /* Clear old group after pivot updated.
         * Update the entry_index.
         * ver1_rbkt_copy need pivot set in place for new group.
         */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_copy(u, db, ln, srbkt_idx, drbkt_idx, TRUE));
        srbkt_idx--;
        drbkt_idx++;
    }

    /* $ Update PIVOT */
    ALPM_ASSERT(pivot);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update(u, db, PREV_LEVEL(ln), pivot));

    /* $ Invalidate old rbkts */
    while (++srbkt_idx < src_group + vbanks) {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_clear(u, db, ln, srbkt_idx));

        RBKT_FORMAT(u, db, ln, srbkt_idx) = DISABLED_FORMAT;
        RBKT_EBMP(u, db, ln, srbkt_idx) = 0; /* Can be saved */
    }

    /* This is where src_group and dst_group are adjcent two groups */
    ALPM_ASSERT(*new_group + fbanks == src_group);
    fbanks = RBKT_FBANKS(u, db, ln, src_group);
    ver1_rbkt_group_free_bank_update(u, db, ln, *new_group, fbanks);
    do {
    RBKT_VBANKS(u, db, ln, src_group) = 0;
    ver1_rbkt_group_free_bank_clr(u, db, ln, src_group);
    RBKT_IS_HEAD(u, db, ln, src_group) = BODY;
    VER1_RBKT_PIVOT(u, db, ln, src_group) = NULL;
    SHR_DQ_REMOVE(&RBKT_LIST_NODE(u, db, ln, src_group));
    } while (0);
    BKT_HDL_GROUPS_USED(u, db, ln)--;

exit:
    append++;
    SHR_FUNC_EXIT();
}


static int
ver1_rbkt_group_prepend(int u, int db, int ln,
                        rbkt_group_t move_group,
                        rbkt_group_t next_group)
{
    SHR_FUNC_ENTER(u);
    ALPM_ASSERT(RBKT_FBANKS(u, db, ln, next_group)); /* Otherwise why append. */
    SHR_RETURN(SHR_E_FULL);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Shuffle existing groups to create a free group with requesting size.
 *
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] start_group Travese start group
 * \param [in] stop_group Travese end group
 * \param [in] size Requesting size for new group
 * \param [out] free_group Group with free banks of requesting size
 *
 * \return SHR_E_NONE/SHR_E_FULL.
 */
static int
ver1_rbkt_group_defrag(int u, int db, int ln,
                       rbkt_group_t start_group,
                       rbkt_group_t stop_group,
                       uint8_t size,
                       rbkt_group_t *free_group)
{
    shr_dq_t *elem, *ielem;
    rbkt_t *icurr, *ocurr, *fcurr, *bcurr;
    int slide_f_rbkts, slide_groups, slide_v_rbkts;
    int collect_fbanks = -1, collect_groups = 0;
    int fwd_groups;
    int min_vbanks = MAX_INT_VALUE;
    rbkt_group_t group_a = 0, end_group;
    rbkt_group_t prev_group, next_group;
    shr_dq_t *start;
    rbkt_group_t new_group;
    SHR_FUNC_ENTER(u);

    end_group = start_group;
    slide_f_rbkts = RBKT_FBANKS(u, db, ln, start_group);
    slide_v_rbkts = RBKT_VBANKS(u, db, ln, start_group);
    slide_groups = 1;
    SHR_DQ_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, db, ln),
                         &RBKT_LIST_NODE(u, db, ln, start_group), elem)
        ocurr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        ALPM_ASSERT(ocurr->is_head == HEAD);
        if (slide_f_rbkts < size) {
            if (end_group == stop_group) {
                break;
            }
            start = SHR_DQ_NEXT(&RBKT_LIST_NODE(u, db, ln, end_group), void *);
            SHR_DQ_TRAVERSE_FROM2(&BKT_HDL_GROUP_LIST(u, db, ln), start, ielem)
                icurr = SHR_DQ_ELEMENT_GET(rbkt_t*, ielem, listnode);
                if (icurr->index == stop_group) {
                    break;
                }
                ALPM_ASSERT(icurr->is_head == HEAD);
                slide_f_rbkts += icurr->free_banks;
                slide_v_rbkts += icurr->valid_banks;
                slide_groups++;
                /*
                 * Renew end_group so that next traverse will continue
                 * based on this new end_group.
                 */
                end_group = icurr->index;
                if (slide_f_rbkts >= size) {
                    break;
                }
            SHR_DQ_TRAVERSE_END(list, ielem);
        }

        if (slide_f_rbkts >= size) {
            if (slide_v_rbkts < min_vbanks) {
                group_a = ocurr->index;
                /* group_c = end_group; */
                min_vbanks = slide_v_rbkts;
                collect_fbanks = slide_f_rbkts;
                collect_groups = slide_groups;
            }
        }
        slide_f_rbkts -= ocurr->free_banks;
        slide_v_rbkts -= ocurr->valid_banks;
        slide_groups--;
    SHR_DQ_TRAVERSE_END(list, elem);

    if (collect_fbanks < size) {
        SHR_RETURN(SHR_E_FULL);
    }

    /* fwd_groups = collect_groups >> 1; */
    fwd_groups = collect_groups;
    prev_group = INVALID_GROUP;
    start = &RBKT_LIST_NODE(u, db, ln, group_a);
    new_group = group_a; /* For coverity */
    SHR_DQ_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, db, ln), start, elem)
        fcurr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        ALPM_ASSERT(fcurr->index != prev_group);
        if (prev_group != INVALID_GROUP) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_append(u, db, ln, prev_group,
                                       fcurr->index, &new_group));
            ver1_rbkt_group_sanity(u, db, ln, prev_group, false, 0);
            ver1_rbkt_group_sanity(u, db, ln, new_group, false, 0);
            prev_group = new_group;
        } else {
            prev_group = fcurr->index;
        }
        if (--fwd_groups <= 0) {
            break;
        }
    SHR_DQ_TRAVERSE_END(list, elem);

    next_group = INVALID_GROUP;
    start = &RBKT_LIST_NODE(u, db, ln, new_group); /* group_c */
    SHR_DQ_BACK_TRAVERSE_FROM(&BKT_HDL_GROUP_LIST(u, db, ln), start, elem)
        bcurr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, listnode);
        if (bcurr->index == new_group) {
            /*
             * Now fcurr is the last rbkt which moved,
             * the back traverse should stop at occur->index.
             */
            break;
        }
        ALPM_ASSERT(bcurr->index != next_group);
        if (next_group != INVALID_GROUP) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_group_prepend(u, db, ln, bcurr->index, next_group));
        }
        next_group = bcurr->index;
    SHR_DQ_TRAVERSE_END(list, elem);
    ALPM_ASSERT(RBKT_FBANKS(u, db, ln, new_group) >= size);

    *free_group = new_group;
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Shuffle existing groups to create a free group with requesting size.
 *
 * If shuffle is not set, just search free list to find a one group to
 * carve out a new one from it.
 * If shuffle is set, simply shuffle existing groups to create new one.
 * Shuffle will not move group with no free banks, that means,
 * the target group will not be selected to the light shuffle list.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Target group, which should be avoided to be moved.
 * \param [in] size Requesting size for new group
 * \param [out] free_group Group with free banks of requesting size
 *
 * \return SHR_E_NONE/SHR_E_FULL.
 */
static int
ver1_rbkt_group_shuffle(int u, int db,
                        int ln,
                        rbkt_group_t group,
                        uint8_t size,
                        rbkt_group_t *free_group)
{
    rbkt_t *curr, *prev;
    rbkt_t *shuffle_groups[MAX_SHUFFLE_NUM] = {NULL};
    rbkt_t *prev_groups[MAX_SHUFFLE_NUM] = {NULL};
    int collect_count = 0;
    int collect_groups = 0;
    shr_dq_t *elem = NULL;
    shr_dq_t *group_list;
    int free_count;
    int tmp_count, i;
    static int shuffle_count;
    SHR_FUNC_ENTER(u);

    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        /* Dump sw state before shuffle */
    }
    shuffle_count++;
    /* Shuffle groups as follows:
     * - Traverse from the most to the least free list.
     * - Find the first rbkt and store it to a temporay list.
     * - Find a rbkt that can be merged with a previously found rbkt so that
     *   that rbkt can be with free banks more than previously collected
     *   free banks.
     * - Once collected free banks are enough (>= size), job done.
     * - Otherwise, fails.
     */
    for (free_count = BKT_HDL_MAX_BANKS(u, db, ln); free_count >= 0;
         free_count--) {
        group_list = &GROUP_USAGE_LIST(u, db, ln, free_count);

        SHR_DQ_TRAVERSE(group_list, elem)
            curr = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
            i = 0;
            while (prev_groups[i] && prev_groups[i] != curr) {
                i++;
            }
            if (prev_groups[i]) {
                continue; /* skip this curr, as it is in prev list */
            }
            if (shuffle_groups[0] == NULL) {
                /* The first group can be used anyway */
                shuffle_groups[0] = curr;
                collect_count = free_count;
            } else if (curr->valid_banks <= collect_count) {
                if (curr->index == 0) { /* No prev group for Group 0 */
                    tmp_count = curr->valid_banks + curr->free_banks;
                    prev = NULL;
                } else {
                    prev = SHR_DQ_ELEMENT_GET(rbkt_t*,
                                              SHR_DQ_PREV(&curr->listnode,
                                                          shr_dq_t*),
                                              listnode);
                    i = 0;
                    while (shuffle_groups[i] && shuffle_groups[i] != prev) {
                        i++;
                    };
                    tmp_count = curr->valid_banks + curr->free_banks;
                    if (!shuffle_groups[i]) { /* prev not in the collect list */
                        /* Ok to add previous free banks as well */
                        tmp_count += prev->free_banks;
                    }
                }
                if (tmp_count > collect_count) {
                    /*
                     * If curr can be moved, and more free count after it moved.
                     * Increase the free count, and add curr to shuffle list
                     */
                    collect_count = tmp_count;
                    prev_groups[collect_groups] = prev;
                    shuffle_groups[++collect_groups] = curr;
                }
            }
            if (collect_count >= size) {
                /* Collect enough */
                break;
            }
        SHR_DQ_TRAVERSE_END(group_list, elem);

        if (collect_count >= size) {
            break;
        }
    }

    if (collect_count >= size) { /* Shuffle list collected */
        rbkt_group_t src_group;
        rbkt_group_t dst_group;
        rbkt_group_t free_group2 = INVALID_GROUP;
        rbkt_group_t new_group;
        for (i = 1; i <= collect_groups; i++) {
            if (shuffle_groups[i - 1]) { /* To skip Group[0] if it is not in the
                                            list */
                /* Merge i to (i - 1) */
                src_group = shuffle_groups[i]->index;

                if (free_group2 != INVALID_GROUP) {
                    dst_group = free_group2;
                } else {
                    /* Only first occurrence */
                    dst_group = shuffle_groups[i - 1]->index;
                }

                /* Fuse them */
                SHR_IF_ERR_EXIT(
                    ver1_rbkt_group_move(u, db, ln, src_group, dst_group,
                                         &new_group,
                                         TRUE, &free_group2));
            }
        }
        ALPM_ASSERT(RBKT_FBANKS(u, db, ln, free_group2) >= size);
        *free_group = free_group2;
    } else {
        /* The utltimate shuffle */
        int rv;
        
        rv = ver1_rbkt_group_defrag(u, db, ln, 0, group, size, free_group);
        if (rv == SHR_E_FULL && group != INVALID_GROUP ) {
            shr_dq_t *next;
            rbkt_t *next_rbkt;
            next = SHR_DQ_NEXT(&RBKT_LIST_NODE(u, db, ln, group), void *);
            next_rbkt = SHR_DQ_ELEMENT_GET(rbkt_t *, next, listnode);
            rv = ver1_rbkt_group_defrag(u, db, ln, next_rbkt->index,
                                        INVALID_GROUP, size, free_group);
        }
        SHR_RETURN(rv);
    }

    /* stats: shuffle++*/

exit:
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        /* Dump sw state before shuffle */
    }
    SHR_FUNC_EXIT();
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
ver1_rbkt_group_free(int u, int db, int ln, rbkt_group_t group)
{
    rbkt_group_t prev_group;

    SHR_FUNC_ENTER(u);
    if (group == INVALID_GROUP) { /* Invalid group does not exist */
        SHR_EXIT();
    }

    ALPM_ASSERT(RBKT_VBANKS(u, db, ln, group) == 0);
    if (group == 0) { /* Don't fuse group */
        VER1_RBKT_PIVOT(u, db, ln, group) = NULL;
    } else {
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_fuse(u, db, ln, group, &prev_group));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a new group.
 *
 * If shuffle is not set, just search free list to find a one group to
 * carve out a new one from it.
 * If shuffle is set, simply shuffle existing groups to create new one.
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
ver1_rbkt_group_alloc(int u, int db,
                      int ln,
                      uint8_t size,
                      bool shuffle,
                      rbkt_group_t *new_group)
{
    int free_count;
    int max_banks;
    shr_dq_t *groups_ptr;
    shr_dq_t *elem = NULL;
    rbkt_t *free_rbkt;
    rbkt_idx_t free_group;
    bkt_hdl_t *bkt_hdl;

    SHR_FUNC_ENTER(u);

    if (size == 0) {
        SHR_RETURN(SHR_E_NONE);
    }

    SHR_IF_ERR_EXIT(
        ver1_bkt_hdl_get(u, db, ln,  &bkt_hdl));

    if (bkt_hdl->rbkts_total - bkt_hdl->rbkts_used < size) {
        SHR_RETURN(SHR_E_FULL);
    }

    if (shuffle) {
        /* No new_group can spare bank of requesting size */
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_shuffle(u, db, ln, INVALID_GROUP, size,
                                    &free_group));
        ALPM_ASSERT(free_group == RBKT_IDX(u, db, ln, free_group));
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_split(u, db, ln, free_group, size, new_group));
        SHR_EXIT();
    }

    max_banks = bkt_hdl->max_banks;

    /*
     * Traverse the GROUP_USAGE array starting from max free count.
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
    for (free_count = max_banks; free_count >= size; free_count--) {
        groups_ptr = &GROUP_USAGE_LIST(u, db, ln, free_count);

        if (!SHR_DQ_EMPTY(groups_ptr)) {
            /* Stop at the first new_group */
            elem = SHR_DQ_HEAD(groups_ptr, shr_dq_t*);
            break;
        }
    }

    if (elem) {
        free_rbkt = SHR_DQ_ELEMENT_GET(rbkt_t*, elem, usgnode);
        free_group = free_rbkt->index;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_split(u, db, ln, free_group, size, new_group));
    } else {
        SHR_RETURN(SHR_E_FULL);
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Allocate a new group with various sizes.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] best_size Best requesting size (biggest size)
 * \param [in] shuffle_size Size kicks off shuffle (middle size)
 * \param [in] least_size  Least size (Guaranteed size)
 * \param [out] new_group New group allocated
 *
 * \return SHR_E_NONE only.
 */
static int
ver1_rbkt_group_alloc_best_effort(int u, int db, int ln,
                                  uint8_t best_size,
                                  uint8_t shuffle_size,
                                  uint8_t least_size,
                                  rbkt_group_t *new_group)
{
    int rv = SHR_E_FULL;

    SHR_FUNC_ENTER(u);
    while (rv == SHR_E_FULL) {
        rv = ver1_rbkt_group_alloc(u, db, ln, best_size, FALSE, new_group);
        best_size >>= 1;
        if (best_size <= least_size) {
            if (rv == SHR_E_FULL) {
                rv = ver1_rbkt_group_alloc(u, db, ln,
                                           shuffle_size,
                                           TRUE,
                                           new_group);
                if (rv == SHR_E_FULL && least_size < shuffle_size) {
                    SHR_IF_ERR_VERBOSE_EXIT(
                        ver1_rbkt_group_alloc(u, db, ln,
                                              least_size,
                                              FALSE,
                                              new_group));
                }
            }
            /* Succeed on shuffle or last try */
            break;
        }
    }
    SHR_RETURN(rv);
    /* Succeed */
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_rbkt_recover(int u, int db, int ln, rbkt_idx_t rbkt_idx,
                  LN_ENTRY_t *base_entry,
                  uint8_t *entry_count,
                  alpm_arg_t *arg)

{
    format_type_t format_type;
    int num_entries, i;
    uint8_t format = RBKT_FORMAT(u, db, ln, rbkt_idx);
    alpm_pivot_t *pivot;

    SHR_FUNC_ENTER(u);
    pivot = arg->pivot[PREV_LEVEL(ln)];
    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                          format,
                                          NULL,
                                          NULL,
                                          &num_entries));
    for (i = 0; i < num_entries; i++) {
        alpm_arg_t entry_arg = *arg;
        entry_arg.key.t.trie_ip = pivot->key.t.trie_ip;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, format_type,
                                                format,
                                                pivot->key.t.len,
                                                &entry_arg,
                                                base_entry[i]));
        /* No need to do.
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_alpm_mask_to_len(u, arg->key.t.ipv,
                                        arg->key.ip_mask,
                                        &arg->key.t.len));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_key_create(u, entry_arg.key.t.ipv,
                                           entry_arg.key.ip_addr,
                                           entry_arg.key.t.len,
                                           &entry_arg.key.t.trie_ip));*/
        if (entry_arg.valid) {
            ALPM_BIT_SET(RBKT_EBMP(u, db, ln, rbkt_idx), i);
            entry_arg.index[ln] = ALPM_INDEX_ENCODE_VER1(rbkt_idx, i);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_pivot_recover(u, ln, &entry_arg));
            (*entry_count)++;
            if (format_type == FORMAT_PIVOT) {
                int next_ln = NEXT_LEVEL(ln);
                SHR_IF_ERR_EXIT(
                    ver1_bucket_recover(u, next_ln, &entry_arg));
                ALPM_ASSERT(entry_arg.pivot[ln] &&
                            entry_arg.pivot[ln]->bkt);
                entry_arg.pivot[ln]->bkt->log_bkt =
                    entry_arg.log_bkt[ln];
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_rbkt_group_recover(int u, int db, int ln, rbkt_group_t group,
                        alpm_arg_t *arg)
{
    LN_ENTRY_t *base_entry;
    LN_ENTRY_t rbkt_buf[ALPM_MAX_BANKS];
    format_type_t format_type;
    base_entries_ver1_t *base_entries;
    uint8_t i;
    uint8_t *fmts;
    alpm_pivot_t *pivot;

    SHR_FUNC_ENTER(u);
    base_entries = shr_lmm_alloc(base_entries_lmem);
    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);

    RBKT_IS_HEAD(u, db, ln, group) = true;
    pivot = arg->pivot[PREV_LEVEL(ln)];
    ALPM_ASSERT(pivot);
    ALPM_ASSERT(!VER1_RBKT_PIVOT(u, db, ln, group));
    ver1_rbkt_pivot_set(u, db, ln, group, pivot);
    arg->log_bkt[PREV_LEVEL(ln)] = VER1_LOG_BKT(group);
    fmts = (PREV_LEVEL(ln) == LEVEL1)? arg->a1.fmt : arg->a2.fmt;
    for (i = 0; i < BKT_HDL_MAX_BANKS(u, db, ln); i++) {
        uint32_t hw_idx;
        uint8_t fmt = fmts[i];
        rbkt_idx_t rbkt_idx;
        uint8_t entry_count;
        if (fmt == DISABLED_FORMAT) {
            break; /* Assuming rbkts are contiguous within a given group */
        }
        hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, group + i);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[i][0]));

        base_entry = &base_entries->entry[i][0];
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_raw_bucket_decode(u, format_type,
                                                fmt,
                                                base_entry,
                                                RBKT_MAX_ENTRIES_VER1,
                                                &rbkt_buf[i][0]));
        rbkt_idx = group + i;
        RBKT_FORMAT(u, db, ln, rbkt_idx) = fmt;
        ALPM_ASSERT(RBKT_VBANKS(u, db, ln, group) <= BKT_HDL_MAX_BANKS(u, db, ln));
        entry_count = 0;
        SHR_IF_ERR_EXIT(
            ver1_rbkt_recover(u, db, ln, rbkt_idx, base_entry,
                              &entry_count, arg));
        if (entry_count) {
            RBKT_VBANKS(u, db, ln, group)++;
        }
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
ver1_bkt_elems_connect_cb(rm_alpm_trie_node_t *node, void *user_data)
{
    bucket_traverse_data_t *trvs_data;
    trvs_data = (bucket_traverse_data_t *) user_data;
    if (node->type == PAYLOAD) {
        alpm_bkt_elem_t *bkt_elem;
        int idx;
        bkt_elem = TRIE_ELEMENT_GET(alpm_bkt_elem_t*, node, bktnode);
        idx = bkt_elem->key.t.len >> IP_LENGTH_GROUP_SHIFT;
        if (SHR_DQ_HEAD(&trvs_data->dq[idx], void *) == NULL) {
            SHR_DQ_INIT(&trvs_data->dq[idx]);
        }
        SHR_DQ_INSERT_HEAD(&trvs_data->dq[idx],
                           &bkt_elem->dqnode);
        if (bkt_elem->index == INVALID_INDEX) {
            trvs_data->found_new = true;
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
ver1_bkt_elems_connect(int u, shr_dq_t *list, rm_alpm_trie_t *trie,
                       bool *found_new)
{
    bucket_traverse_data_t trvs_data;
    int i;
    SHR_FUNC_ENTER(u);
    sal_memset(&trvs_data, 0, sizeof(trvs_data));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_traverse(trie,
                                     ver1_bkt_elems_connect_cb,
                                     &trvs_data,
                                     TRAVERSE_BFS));
    SHR_DQ_INIT(list);
    
    for (i = 0; i < IP_LENGTH_GROUP_MAX; i++) {
        SHR_DQ_JOIN_TAIL(list, &trvs_data.dq[i]);
    }
    *found_new = trvs_data.found_new;
    ALPM_ASSERT((trie->trie && (trvs_data.count == trie->trie->count)) ||
                (trie->trie == NULL && trvs_data.count == 0));
exit:
    SHR_FUNC_EXIT();
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
ver1_base_entries_free(int u, int db, int ln,
                       rbkt_group_t group,
                       ln_index_t *ent_idx,
                       int count,
                       bool *format_update)
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

        if (rbkt->entry_bitmap == 0) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_free(u, db, ln, group, remove_bkt));
            if (format_update) {
                *format_update = true;
            }
        }
    }

exit:
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
ver1_base_entry_free(int u, int db, int ln,
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
    if (rbkt->entry_bitmap == 0) {
        rbkt_idx_t last_bkt = group + RBKT_VBANKS(u, db, ln, group) - 1;
        if (last_bkt != remove_bkt) {
            /*
             * If a mid rbkt becomes empty, copy the last to that empty place.
             * but we don't clear the last immediatelly, as to prevent packet
             * misses. We should clear it after pivot update done.
             */

            SHR_IF_ERR_EXIT(
                ver1_rbkt_copy(u, db, ln, last_bkt, remove_bkt, TRUE));
        }
        SHR_IF_ERR_EXIT(
            ver1_rbkt_free(u, db, ln, group, last_bkt));
        *format_update = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign a free entry slot for given rbkt.
 *
 * Traverse the existing rbkt resources and assign free entry.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] entry_len IP length of requesting entry.
 * \param [in] entry_full Data type (full/reduced) of requesting entry.
 * \param [out] ent_idx Entry index
 * \param [out] format_update True if update pivot format, eles false.
 * \param [out] format_value New format value.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_base_entry_assign(int u, int db, int ln,
                       int entry_len,
                       int entry_full,
                       int rbkt_idx,
                       int *ent_idx,
                       bool *format_update,
                       int *format_value)
{
    int prefix_len;
    int full;
    int format;
    int max_format, format_type;
    SHR_FUNC_ENTER(u);

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    max_format = MAX_FORMAT(u, format_type);

    /* From most efficient format to least efficient format */
    for (format = START_FORMAT; format <= max_format; format++) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                              format,
                                              &prefix_len,
                                              &full,
                                              NULL));
        if (prefix_len >= entry_len && full >= entry_full) {
            RBKT_FORMAT(u, db, ln, rbkt_idx) = format;
            ALPM_BIT_SET(RBKT_EBMP(u, db, ln, rbkt_idx), 0);
            *ent_idx = ALPM_INDEX_ENCODE_VER1(rbkt_idx, 0);

            if (format_update) {
                *format_update = TRUE;
            }
            *format_value = format;
            SHR_EXIT();
        }
    }
    ALPM_ASSERT(0);
exit:
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
ver1_base_entry_alloc(int u, int db, int ln,
                      rbkt_group_t group,
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
    format_type_t format_type;
    SHR_FUNC_ENTER(u);

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        rbkt_t *rbkt = &RBKT(u, db, ln, group + i);

        /* Less format value, more compact */
        if (rbkt->format < min_format_value) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                                  rbkt->format,
                                                  &prefix_len,
                                                  &full,
                                                  &number));
            if (prefix_len >= entry_len &&
                rbkt->entry_bitmap != (uint32_t)((1 << number) - 1) &&
                full >= entry_full) {
                min_format_value = rbkt->format;
                rbkt_idx = group + i;
            }
        }
    }

    if (min_format_value != INVALID_FORMAT) {/* Free entry in valid banks */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_info(u, format_type,
                                              min_format_value,
                                              &prefix_len,
                                              &full,
                                              &number));
        for (i = 0; i < number; i++) {
            if (!ALPM_BIT_GET(RBKT_EBMP(u, db, ln, rbkt_idx), i)) {
                *ent_idx = ALPM_INDEX_ENCODE_VER1(rbkt_idx, i);
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

    /* Assign free banks */
    if (RBKT_FBANKS(u, db, ln, group) && i < BKT_HDL_MAX_BANKS(u, db, ln)) {
        rbkt_idx = group + RBKT_VBANKS(u, db, ln, group);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_expand_right(u, db, ln, group));
        SHR_IF_ERR_EXIT(
            ver1_base_entry_assign(u, db, ln,
                                   entry_len,
                                   entry_full,
                                   rbkt_idx,
                                   ent_idx,
                                   format_update,
                                   format_value));
    } else {
        SHR_RETURN(SHR_E_FULL);
    }
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
ver1_base_entries_clear(int u, int db, int ln,
                        rbkt_group_t group,
                        ln_index_t *ent_idx,
                        int count)
{
    int i, j, ent_ofs;
    LN_ENTRY_t rbkt_buf[ALPM_MAX_BANKS];
    base_entries_ver1_t *base_entries;
    LN_ENTRY_t *base_entry;
    rbkt_idx_t rbkt_idx[ALPM_MAX_BANKS];
    rbkt_t *rbkt[ALPM_MAX_BANKS] = {NULL};
    format_type_t format_type;
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
    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        if (rbkt_idx[i] != -1) {
            hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[i][0]));

            rbkt[i] = &RBKT(u, db, ln, rbkt_idx[i]);
            base_entry = &base_entries->entry[i][0];
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_raw_bucket_decode(u, format_type,
                                                    rbkt[i]->format,
                                                    base_entry,
                                                    RBKT_MAX_ENTRIES_VER1,
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
        sal_memset(base_entry, 0, sizeof(LN_ENTRY_t));
    }

    /* Encode base_entries array to each rbkt, and then write all */
    for (i = 0; i < RBKT_VBANKS(u, db, ln, group); i++) {
        if (rbkt_idx[i] != -1) {
            base_entry = &base_entries->entry[i][0];
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_raw_bucket_encode(u, format_type,
                                                    rbkt[i]->format,
                                                    base_entry,
                                                    RBKT_MAX_ENTRIES_VER1,
                                                    &rbkt_buf[i][0]));

            hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx[i]);
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
ver1_base_entry_clear(int u, int db, int ln, ln_index_t ent_idx)
{
    int rbkt_idx;
    LN_ENTRY_t rbkt_buf;
    LN_ENTRY_t base_entry = {0};
    rbkt_t *rbkt;
    format_type_t format_type;
    int ent_ofs;
    int hw_idx;
    SHR_FUNC_ENTER(u);

    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));
    rbkt = &RBKT(u, db, ln, rbkt_idx);

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
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
ver1_bucket_copy(int u, int db, int ln,
                 rbkt_group_t src_group,
                 key_tuple_t *src_t,
                 rbkt_group_t dst_group,
                 key_tuple_t *dst_t,
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
    int format_type;
    int src_bank, dst_bank;
    int src_ent, dst_ent;
    int i = 0, hw_idx;
    SHR_FUNC_ENTER(u);

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    sal_memset(drbkt_buf, 0, sizeof(drbkt_buf));
    sal_memset(dst_rbkts, -1, sizeof(dst_rbkts));
    ALPM_ASSERT(dst_rbkts[0] == -1);

    SHR_DQ_BACK_TRAVERSE(list, elem)
        bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
        src_idx = bkt_elem->index;
        old_idx[i] = src_idx;
        SHR_IF_ERR_EXIT(
            ver1_base_entry_alloc(u, db, ln, dst_group,
                                  bkt_elem->key.t.len - dst_t->len,
                                  DATA_REDUCED,
                                  &dst_idx,
                                  NULL, /* Will write anyway */
                                  &dst_format));

        dst_rbkt_idx = RBKT_FROM_INDEX(dst_idx);
        dst_bank = dst_rbkt_idx - dst_group;
        dst_ent = ENT_OFS_FROM_IDX(dst_idx);
        if (src_idx != INVALID_INDEX) {
            src_rbkt_idx = RBKT_FROM_INDEX(src_idx);
            src_ent = ENT_OFS_FROM_IDX(src_idx);
            src_bank = src_rbkt_idx - src_group;
            ALPM_ASSERT(src_bank >= 0 && src_bank < ALPM_MAX_BANKS);

            if (src_rbkts[src_bank] == 0) {
                hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, src_rbkt_idx);
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
                                                  src_t,
                                                  &srbkt_buf[src_bank][0],

                                                  dst_format,
                                                  dst_ent,
                                                  dst_t,
                                                  &drbkt_buf[dst_bank][0]
                                                  ));
        } else {
            LN_ENTRY_t base_entry = {0};
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_ln_base_entry_encode(u, format_type,
                                                    dst_format,
                                                    dst_t->len,
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
        if (dst_rbkts[dst_bank] == -1) {
            dst_rbkts[dst_bank] = dst_rbkt_idx;
        }
        i++;
    SHR_DQ_TRAVERSE_END(list, elem);

    ALPM_ASSERT(i == count);

    /* Write dest rbkts */
    for (i = 0; i < ALPM_MAX_BANKS; i++) {
        if (dst_rbkts[i] != -1) {
            hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, dst_rbkts[i]);
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_write_ln_raw(u, db, ln, hw_idx,
                                            &drbkt_buf[i][0]));
        }
    }

exit:
    if (SHR_FUNC_ERR() && i) {
        bool format_update;
        ALPM_ASSERT(i < count); /* Assume just entry alloc would fail */
        count = i;
        i = 0;
        SHR_DQ_BACK_TRAVERSE(list, elem)
            bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, elem, dqnode);
            src_idx = bkt_elem->index;
            SHR_IF_ERR_EXIT(
                ver1_base_entry_free(u, db, ln, dst_group, src_idx,
                                     &format_update));
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
 * \param [in] src_t Source tuple
 * \param [in] dst_group Dest group
 * \param [in] dst_t Dest tuple
 * \param [in] list Move list
 * \param [in] count Count of bak_idx array
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [in] dst_pivot_arg Arg for dst pivot.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_move(int u, int db, int ln,
                 rbkt_group_t src_group,
                 key_tuple_t *src_t,
                 rbkt_group_t dst_group,
                 key_tuple_t *dst_t,
                 shr_dq_t *list,
                 int count,
                 alpm_arg_t *arg,
                 alpm_arg_t *dst_pivot_arg,
                 bool *format_update)
{
    alpm_pivot_t *dst_pivot;
    ln_index_t *old_idx = NULL;
    SHR_FUNC_ENTER(u);

    /* $ Save old ent_idx of prefixes */
    ALPM_ALLOC(old_idx, count * sizeof(ln_index_t), "old_idx");
    sal_memset(old_idx, -1, count * sizeof(ln_index_t));

    /* $ (HW) Copy split prefixes from old bucket to new bucket */
    SHR_IF_ERR_EXIT(
        ver1_bucket_copy(u, db, ln,
                         src_group,
                         src_t,
                         dst_group,
                         dst_t,
                         list,
                         count,
                         arg,
                         old_idx));

    /* $ Activate the pivot */
    dst_pivot = ver1_rbkt_pivot_get(u, db, ln, dst_group);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pivot_update_with_arg(u, db, PREV_LEVEL(ln), dst_pivot,
                                             dst_pivot_arg));

    /* $ Invalidate moved elems from old bucket in HW */
    SHR_IF_ERR_EXIT(
        ver1_base_entries_clear(u, db, ln, src_group, old_idx, count));

    /* $ Free those moved elems from parent buckets */
    SHR_IF_ERR_EXIT(
        ver1_base_entries_free(u, db, ln, src_group, old_idx, count,
                               format_update));

exit:
    /* Assume just bucket copy are subject to fail */
    if (old_idx != NULL) {
        SHR_FREE(old_idx);
    }
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
 * \param [in] arg New entry arg, in case new entry in split bucket.
 * \param [out] dst_group Destination group
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
ver1_bucket_alloc_and_move(int u, int db, int ln,
                           rbkt_group_t src_group,
                           alpm_arg_t *arg,
                           rbkt_group_t *dst_group)
{
    int count, max_banks;
    rbkt_group_t new_group;
    alpm_pivot_t *src_pivot;
    enum {
        ALLOC_AND_MOVE_NONE,
        ALLOC_AND_MOVE_ALLOC,
        ALLOC_AND_MOVE_MOVE
    } done = ALLOC_AND_MOVE_NONE;
    SHR_FUNC_ENTER(u);

    /*
     * Found a new place to hold remaining entries in parent bucket.
     * This parent group should be skipped in shuffle process
     * if required during group alloc.
     * Note: this group should be already full so as not to be
     * be shuffled, otherwise we will end up with ill state.
     */
    if (RBKT_FBANKS(u, db, ln, src_group)) {
        SHR_RETURN(SHR_E_FULL);
    }
    max_banks = BKT_HDL_MAX_BANKS(u, db, ln);
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_alloc_best_effort(u, db, ln,
                                          max_banks,
                                          max_banks,
                                          max_banks,
                                          &new_group));

    done = ALLOC_AND_MOVE_ALLOC;
    /* $ Save old ent_idx of prefixes */
    src_pivot = VER1_RBKT_PIVOT(u, db, ln, src_group);
    count = bcmptm_rm_alpm_trie_count(src_pivot->bkt->trie);
    VER1_RBKT_PIVOT(u, db, ln, new_group) = src_pivot;

    SHR_IF_ERR_EXIT(
        ver1_bucket_move(u, db, ln,
                         src_group,
                         &src_pivot->key.t,
                         new_group,
                         &src_pivot->key.t,
                         &src_pivot->bkt->list,
                         count,
                         arg,
                         NULL,
                         NULL /* No need to update format for alloc_and_move */
                         ));

    done = ALLOC_AND_MOVE_MOVE;
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_free(u, db, ln, src_group));
    src_pivot->bkt->log_bkt = VER1_LOG_BKT(new_group);
    if (dst_group) {
        *dst_group = new_group;
    }
exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case ALLOC_AND_MOVE_MOVE:
            ALPM_ASSERT(0); /* not supposed to fail */
        case ALLOC_AND_MOVE_ALLOC:
            ver1_rbkt_group_free(u, db, ln, new_group);
        case ALLOC_AND_MOVE_NONE:
        default:
            break;
        }

    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Callback Functions
 */

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
ver1_base_entry_encode_write(int u, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    LN_ENTRY_t rbkt_buf;
    LN_ENTRY_t base_entry = {0};
    rbkt_t *rbkt;
    format_type_t format_type;
    ln_index_t ent_idx;
    int ent_ofs, hw_idx;
    int pivot_len, db;
    SHR_FUNC_ENTER(u);

    if (arg->skip_write == WRITE_LATER) {
        SHR_EXIT();
    }

    ent_idx = arg->index[ln];

    db = arg->db;
    pivot_len = arg->pivot[PREV_LEVEL(ln)]->key.t.len;
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));

    rbkt = &RBKT(u, db, ln, rbkt_idx);

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_encode(u, format_type,
                                            rbkt->format,
                                            pivot_len,
                                            arg,
                                            &base_entry[0]));
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        alpm_arg_t arg2;
        bcmptm_rm_alpm_arg_init(u, &arg2);
        /* Decode needs: ipv, previous level trie_ip */
        arg2.key.t.ipv = arg->key.t.ipv;
        arg2.key.t.w_vrf = arg->key.t.w_vrf;
        sal_memcpy(&arg2.key.t.trie_ip,
                   &arg->pivot[PREV_LEVEL(ln)]->key.t.trie_ip,
                   sizeof(trie_ip_t));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_ln_base_entry_decode(u, format_type,
                                                rbkt->format,
                                                pivot_len,
                                                &arg2,
                                                &base_entry[0]));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_alpm_api_key_create(u, arg2.key.t.ipv,
                                           &arg2.key.t.trie_ip,
                                           arg2.key.t.len,
                                           arg2.key.ip_addr));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_len_to_mask(u, arg2.key.t.ipv, arg2.key.t.len,
                                       &arg2.key.ip_mask[0]));

        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_addr, arg->key.ip_addr,
                                sizeof(alpm_ip_t)));
        ALPM_ASSERT(!sal_memcmp(arg2.key.ip_mask, arg->key.ip_mask,
                                sizeof(alpm_ip_t)));
        ALPM_ASSERT(!sal_memcmp(&arg2.key.t, &arg->key.t, sizeof(key_tuple_t)));
    }

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
ver1_base_entry_decode_read(int u, int ln, alpm_arg_t *arg)
{
    int rbkt_idx;
    LN_ENTRY_t rbkt_buf;
    LN_ENTRY_t base_entry = {0};
    rbkt_t *rbkt;
    format_type_t format_type;
    int ent_ofs, hw_idx;
    ln_index_t ent_idx;
    int pivot_len;
    trie_ip_t *pivot_ip;
    alpm_pivot_t *pivot;
    int db;
    SHR_FUNC_ENTER(u);

    /* Decode needs: ipv, previous level trie_ip */
    db = arg->db;
    ent_idx = arg->index[ln];
    pivot = arg->pivot[PREV_LEVEL(ln)];
    pivot_len = pivot->key.t.len;
    pivot_ip = &pivot->key.t.trie_ip;
    sal_memcpy(&arg->key.t.trie_ip, pivot_ip, sizeof(trie_ip_t));
    arg->index[ln] = ent_idx;
    rbkt_idx = RBKT_FROM_INDEX(ent_idx);
    hw_idx = ver1_hw_idx_from_rbkt_idx(u, db, ln, rbkt_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_read_ln_raw(u, db, ln, hw_idx, &rbkt_buf[0]));

    rbkt = &RBKT(u, db, ln, rbkt_idx);

    format_type = bcmptm_rm_alpm_format_type_get(u, db, ln);
    ent_ofs = ENT_OFS_FROM_IDX(ent_idx);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_raw_bucket_entry_get(u, format_type,
                                               rbkt->format,
                                               &base_entry[0],
                                               ent_ofs,
                                               &rbkt_buf[0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_ln_base_entry_decode(u, format_type,
                                            rbkt->format,
                                            pivot_len,
                                            arg,
                                            &base_entry[0]));

    ALPM_ASSERT(ALPM_BIT_GET(rbkt->entry_bitmap, ent_ofs));

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_api_key_create(u, arg->key.t.ipv,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      &arg->key.ip_addr[0]));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_len_to_mask(u, arg->key.t.ipv,
                                   arg->key.t.len,
                                   &arg->key.ip_mask[0]));

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
ver1_bucket_insert(int u, int ln, alpm_arg_t *arg,
                   alpm_pivot_t *pivot)
{
    bool format_update = FALSE;
    int format_value;
    int bucket_len = arg->key.t.len - pivot->key.t.len;
    rbkt_group_t group;
    rbkt_idx_t new_rbkt = 0;
    ln_index_t ent_idx = INVALID_INDEX;
    int rv;
    SHR_FUNC_ENTER(u);

    if (pivot->bkt->log_bkt == INVALID_LOG_BKT) {
        /*
         * If pivot exists, but bucket group is invalid,
         * this implies the pivot is just itself without a bucket.
         * So we should alloc a bucket group for it first.
         */
        int max_banks;

        ALPM_ASSERT(pivot->bkt->trie->trie == NULL);
        max_banks = BKT_HDL_MAX_BANKS(u, arg->db, ln);
        SHR_IF_ERR_EXIT(
            ver1_rbkt_group_alloc_best_effort(u, arg->db, ln,
                                              max_banks,
                                              max_banks,
                                              max_banks,
                                              &group));
        VER1_RBKT_PIVOT(u, arg->db, ln, group) = pivot;
        pivot->bkt->log_bkt = VER1_LOG_BKT(group);
    } else {
        group = VER1_GROUP(pivot->bkt->log_bkt);
    }
    rv = ver1_base_entry_alloc(u, arg->db, ln, group,
                               bucket_len,
                               DATA_REDUCED,
                               &ent_idx,
                               &format_update,
                               &format_value);
    if (rv == SHR_E_FULL) {
        /*
         * We've run out of banks, expand it.
         * E_FULL is still a normal return value when the group is full,
         * in which case split should happen.
         */
        SHR_IF_ERR_VERBOSE_EXIT(
            ver1_rbkt_alloc(u, arg->db, ln, group, &new_rbkt));
        ALPM_ASSERT(RBKT_EBMP(u, arg->db, ln, new_rbkt) == 0);
        SHR_IF_ERR_EXIT(
            ver1_base_entry_assign(u, arg->db, ln,
                                   bucket_len,
                                   DATA_REDUCED,
                                   new_rbkt,
                                   &ent_idx,
                                   &format_update,
                                   &format_value));
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    ALPM_ASSERT(ent_idx != INVALID_INDEX);
    arg->index[ln] = ent_idx;
    arg->pivot[PREV_LEVEL(ln)] = pivot; /* Set pivot for pivot_len */
    SHR_IF_ERR_EXIT(
        ver1_base_entry_encode_write(u, ln, arg));
    if (format_update) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, arg->db, PREV_LEVEL(ln), pivot));
    }

exit:
    SHR_FUNC_EXIT();
}

static void
ver1_split_arg_init(int u,
                    int ln,
                    alpm_arg_t *arg,
                    trie_ip_t *pivot_key,
                    uint32_t pivot_len,
                    rbkt_group_t new_group,
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
    split_arg->log_bkt[prev_ln] = VER1_LOG_BKT(new_group);
    split_arg->valid = 1;
    split_arg->db = arg->db;
    /*
     * This is to get the correct first level pivot,
     * it also makes sure no pivot find for the first level
     */
    if (ln == LEVEL3) {
        split_arg->pivot[PREV_LEVEL(prev_ln)] =
            ver1_bucket_pivot_get(u, arg->db, prev_ln, parent_pivot->index);
    }
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
ver1_bucket_delete(int u, int db, int ln,
                   alpm_pivot_t *pivot,
                   ln_index_t ent_idx)
{
    rbkt_group_t group = VER1_GROUP(pivot->bkt->log_bkt);
    bool format_update = FALSE;
    rbkt_idx_t vbanks;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        ver1_base_entry_clear(u, db, ln, ent_idx));

    vbanks = RBKT_VBANKS(u, db, ln, group);
    SHR_IF_ERR_EXIT(
        ver1_base_entry_free(u, db, ln, group, ent_idx, &format_update));

    if (format_update) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, db, PREV_LEVEL(ln), pivot));
        /* Clear rbkt after format update */
        if (RBKT_VBANKS(u, db, ln, group) < vbanks) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_clear(u, db, ln, group + vbanks - 1));
        }
    }

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
ver1_bucket_split(int u, int ln, alpm_arg_t *arg, alpm_pivot_t *parent_pivot,
                  bkt_split_aux_t *split_aux)
{
    int rv;
    rm_alpm_trie_t *split_trie = NULL;
    rbkt_group_t new_group;
    alpm_pivot_t *split_pivot = NULL;
    int split_count;
    int msl, msc;
    bool format_update = FALSE;
    rm_alpm_trie_node_t *split_root = NULL;
    uint32_t pivot_len, tmp_pivot_len;
    trie_ip_t pivot_key, tmp_pivot_key;
    alpm_arg_t split_arg;
    int prev_ln = ln - 1;
    int vbanks, i;
    rbkt_group_t parent_group;
    int max_banks;
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
    max_banks = BKT_HDL_MAX_BANKS(u, arg->db, ln);
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_alloc_best_effort(u, arg->db, ln,
                                          max_banks,
                                          max_banks,
                                          max_banks,
                                          &new_group));
    arg->log_bkt[prev_ln] = VER1_LOG_BKT(new_group);
    done = SPLIT_ALLOC_GROUP;

    /* $ Trie split */
    /*
     * max_split_len and max_split_count.
     * The larger the split_len, the less efficient format it takes from
     * previous level.
     * The more split payloads, the more rbkts it needs.
     * Ideally, the max_split_count should be a result of function considering
     * new group rbkts number, and the optimal formats for split prefixes.
     * However the split prefixes are a result of the split process accepting
     * max_split_count as input.
     * Ideally, the max_split_len should be just perfect to fit the formats
     * for split prefixes, again the split prefixes are a result of the split
     * process accepting max_split_len as input.
     * We are facing with a situation called causality dilemma, aka,
     * a chicken or the egg situation.
     * The possible best way is to try different parameters until a optimial
     * solution found. But, as a SDK algorithm, we don't have time bandwith
     * for such complex algorithm.
     */
    /*    parent_pivot = (alpm_pivot_t *)arg->pivot[prev_ln]; */
    parent_group = VER1_GROUP(parent_pivot->bkt->log_bkt);
    msl = ver1_trie_msl(u, parent_pivot);
    msc = ver1_trie_msc(u, ln, parent_pivot);
    /* Can be saved for performance */
    sal_memset(&pivot_key, 0, sizeof(pivot_key));
    if (split_aux && split_aux->aux_parent_pivot[ln] == parent_pivot) {
        /*
         * Special split -
         * A split point is already decided, we should just use
         * split at that point. This should be guaranteed to
         * succeed.
         */
        rv = bcmptm_rm_alpm_trie_split2(parent_pivot->bkt->trie,
                                        &split_aux->aux_pivot_key.key[0],
                                        split_aux->aux_pivot_len,
                                        &tmp_pivot_key.key[0],
                                        &tmp_pivot_len,
                                        &split_aux->aux_split_root);
        SHR_IF_ERR_EXIT(rv); /* Not expected to fail */
        ALPM_ASSERT(split_aux->aux_pivot_len == tmp_pivot_len);
        ALPM_ASSERT(split_aux->aux_split_root);

        sal_memcpy(&pivot_key, &split_aux->aux_pivot_key, sizeof(trie_ip_t));
        pivot_len = split_aux->aux_pivot_len;
        split_root = split_aux->aux_split_root;
    } else {
        /* For non-last level, payload split can save us a lot of trouble */
        rv = bcmptm_rm_alpm_trie_split(parent_pivot->bkt->trie,
                                       msl,
                                       &pivot_key.key[0],
                                       &pivot_len,
                                       &split_root,
                                       NULL,
                                       ln != DB_LAST_LEVEL(u, arg->db),
                                       msc);
    }
    if (SHR_SUCCESS(rv)) {
        done = SPLIT_TRIE_SPLIT;
    } else if (split_aux && rv == SHR_E_NOT_FOUND &&
               ln != DB_LAST_LEVEL(u, arg->db)) {
        /*
         * We have tried payload split, but failed. Terefore the split point
         * will be an internal node, we have to turn it to a payload as this is
         * a non-last level.
         *
         * This non-payload split point come with two scenarios:
         *
         * a) The internal split point will affect one level-3 bucket in a way
         * that, some l3 entries in that bucket can be now in the wrong bucket
         * due to this new internal (level-2 bucket) split point. To fix this,
         * that level-3 bucket should also be spilt, and those l3 entries
         * should be moved to the new split bucket.
         * We now have to split two level-3 buckets: the one holding
         * to-be-inserted entry, and the one holding entrie that is affected by
         * the first split. The solution is to find the split point for second
         * split, and then fall back to the original split entrance and redo
         * the two level-3 bucket splits.
         * We choose to use a special error code SHR_E_PARTIAL to flag
         * this case.
         *
         * b) The internal split point does not affect any other l3 bucket.
         * This can be that, there is no affeted pivot at all, or the pivot
         * exits but there is no entry affected. Either way, we still need to
         * fall back to re-split the previous level (level-3) bucket, in order
         * to ensure that level-1 pivot always be a payload in level-2. For
         * this to work, we need to resplit the level-3 bucket to honor the
         * same (internal) split point we got here for level-1 pivot.
         */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_split(parent_pivot->bkt->trie,
                                      msl,
                                      &pivot_key.key[0],
                                      &pivot_len,
                                      &split_root,
                                      NULL,
                                      false,
                                      msc));
        done = SPLIT_TRIE_SPLIT;
        do { /* The spivot is internal node, we must make it payload */
        rm_alpm_trie_node_t *split_lpm_node;
        rv = bcmptm_rm_alpm_trie_find_lpm2(parent_pivot->bkt->trie,
                                      &pivot_key.key[0],
                                      pivot_len,
                                      &split_lpm_node);
        if (SHR_SUCCESS(rv)) {
            /* Extract those prefixes in found_lpm */
            alpm_pivot_t *sub_parent_pivot;

            sub_parent_pivot = TRIE_ELEMENT_GET(alpm_pivot_t*,
                            split_lpm_node, bktnode);
            rv = bcmptm_rm_alpm_trie_split2(sub_parent_pivot->bkt->trie,
                                        &pivot_key.key[0],
                                        pivot_len,
                                        &split_aux->aux_pivot_key.key[0],
                                        &split_aux->aux_pivot_len,
                                        &split_aux->aux_split_root);
            if (rv == SHR_E_NONE) {
                ALPM_ASSERT(split_aux->aux_pivot_len == pivot_len);
                ALPM_ASSERT(split_aux->aux_split_root);
                split_aux->aux_parent_pivot[LEVEL3] = sub_parent_pivot;
                split_aux->aux_parent_pivot[LEVEL2] = parent_pivot;
                split_aux->set=2;

                bcmptm_rm_alpm_trie_merge(sub_parent_pivot->bkt->trie,
                                            split_aux->aux_split_root,
                                            &pivot_key.key[0],
                                            pivot_len);
                SHR_RETURN(SHR_E_PARTIAL);
            } else if (rv == SHR_E_NOT_FOUND) {
                /* LPM exists, but no affected entries */
            } else { /* not expect */
                ALPM_ASSERT(0);
                SHR_IF_ERR_EXIT(rv);
            }
        } else { /* LPM does not exists */
        }
        split_aux->aux_parent_pivot[LEVEL2] = parent_pivot;
        split_aux->set=4;

        /* $ merge back */
        SHR_RETURN(SHR_E_PARTIAL);
        } while (0);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    /* $ Split arg init */
    ver1_split_arg_init(u, ln, arg, &pivot_key, pivot_len, new_group,
                        parent_pivot, &split_arg);

    /* $ Pivot insert
     * E_FULL can be returned for pivot full. Properly roll back.
     */

    rv = bcmptm_rm_alpm_split_pivot_add(u, arg->db, prev_ln, &split_arg,
                                        &split_pivot, split_aux);
    if (rv == SHR_E_PARTIAL && split_aux) {
        ALPM_ASSERT(ln == LEVEL3);
        if (split_aux->aux_parent_pivot[LEVEL3] == NULL) {
            ALPM_ASSERT(split_aux->set == 4);
            sal_memcpy(&split_aux->aux_pivot_key, &pivot_key,
                       sizeof(pivot_key));
            split_aux->aux_pivot_len = pivot_len;
            split_aux->aux_parent_pivot[LEVEL3] = parent_pivot;
        }
        SHR_RETURN(rv);
    }
    SHR_IF_ERR_EXIT(rv);
    split_trie = split_pivot->bkt->trie;
    ALPM_ASSERT(split_trie);
    split_trie->trie = split_root;
    split_count = split_root->count;
    VER1_RBKT_PIVOT(u, arg->db, ln, new_group) = split_pivot;
    done = SPLIT_PIVOT_INSERT;

    /* $ Connect bkt elems in new split bucket */
    SHR_IF_ERR_EXIT(
        ver1_bkt_elems_connect(u, &split_pivot->bkt->list, split_trie,
                               &new_in_split));

    /* $ Connect bkt elems in parent bucket */
    SHR_IF_ERR_EXIT(
        ver1_bkt_elems_connect(u, &parent_pivot->bkt->list,
                               parent_pivot->bkt->trie,
                               &new_in_parent));
    if (new_in_split) {
        arg->pivot[prev_ln] = split_pivot;
    }
    done = SPLIT_BUCKET_CONNECT;

    vbanks = RBKT_VBANKS(u, arg->db, ln, parent_group);
    SHR_IF_ERR_EXIT(
        ver1_bucket_move(u, arg->db, ln,
                         parent_group,
                         &parent_pivot->key.t,
                         new_group,
                         &split_pivot->key.t,
                         &split_pivot->bkt->list,
                         split_count,
                         arg,
                         &split_arg,
                         &format_update));
    done = SPLIT_BUCKET_MOVE_AND_ACTIVATE;

    /* $ Compress the parent buckets to make them continuous */
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_compress(u, arg->db, ln, parent_group,
                                 vbanks, &format_update));

    if (RBKT_VBANKS(u, arg->db, ln, parent_group) == 0) {
        ALPM_ASSERT(parent_pivot->bkt->trie->trie == NULL);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_clean(u, arg->db, ln, parent_pivot));
    }
    /* $ Update parent pivot if required */
    if (format_update) {
        int delete_rbkts;
        rbkt_idx_t last_rbkt;

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pivot_update(u, arg->db, PREV_LEVEL(
                                            ln), parent_pivot));
        delete_rbkts = vbanks - RBKT_VBANKS(u, arg->db, ln, parent_group);
        last_rbkt = parent_group + vbanks - 1;

        /* Clear those just-become-empty rbtks after pivot updated */
        for (i = 0; i < delete_rbkts; i++) {
            SHR_IF_ERR_EXIT(
                ver1_rbkt_clear(u, arg->db, ln, last_rbkt - i));
        }
    }
    done = SPLIT_OLD_BUCKET_CLEANUP;
    
    if (new_in_parent) {
        rv = ver1_bucket_insert(u, ln, arg, parent_pivot);
        if (rv == SHR_E_FULL) {
            rbkt_group_t new_group2;
            /* Take the chance to reorganize */
            rv = ver1_bucket_alloc_and_move(u, arg->db, ln, parent_group,
                                            arg, &new_group2);
            if (rv == SHR_E_FULL) {
                rv = SHR_E_BADID;
            }
        }
        SHR_IF_ERR_EXIT(rv);
    }
    ver1_rbkt_group_sanity(u, arg->db, ln, new_group, false, 0);
    ver1_rbkt_group_sanity(u, arg->db, ln, parent_group, false, 0);
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
            VER1_RBKT_PIVOT(u, arg->db, ln, new_group) = NULL;
            rv = bcmptm_rm_alpm_split_pivot_delete(u, arg->db, prev_ln,
                                                   &split_arg, NULL);
            ALPM_ASSERT(rv == 0);
            /* Fall through */
        case SPLIT_TRIE_SPLIT:
            bcmptm_rm_alpm_trie_merge(parent_pivot->bkt->trie,
                                      split_root,
                                      &pivot_key.key[0],
                                      pivot_len);
            ver1_bkt_elems_connect(u, &parent_pivot->bkt->list,
                                   parent_pivot->bkt->trie,
                                   &new_in_parent);
            ALPM_ASSERT(new_in_parent);
            /* Fall through */
        case SPLIT_ALLOC_GROUP:
            ver1_rbkt_group_free(u, arg->db, ln, new_group);
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
ver1_bucket_free(int u, int db, int ln, rbkt_group_t group)
{
    return ver1_rbkt_group_free(u, db, ln, group);
}

static int
ver1_bucket_alloc(int u, int ln, alpm_arg_t *arg, log_bkt_t *log_bkt)
{
    int max_banks, rv;
    rbkt_group_t group;
    max_banks = BKT_HDL_MAX_BANKS(u, arg->db, ln);
    rv = ver1_rbkt_group_alloc_best_effort(u, arg->db, ln,
                                           max_banks,
                                           max_banks >> 1,
                                           1,
                                           &group);
    if (SHR_SUCCESS(rv)) {
        *log_bkt = VER1_LOG_BKT(group);
    }
    return rv;
}
static void
ver1_bucket_pivot_set(int u, int db, int ln, rbkt_group_t group,
                      alpm_pivot_t *pivot)
{
    ver1_rbkt_pivot_set(u, db, ln, group, pivot);
}

static void
ver1_alpm_data_encode(int u, int ln, alpm_arg_t *arg, void *adata)
{
    ALPM_ASSERT(ln == LEVEL1 || ln == LEVEL2);
    if (ln == LEVEL1) {
        ver1_alpm1_data_encode(u, arg->db, NEXT_LEVEL(ln), arg->default_route,
                               VER1_GROUP(arg->log_bkt[LEVEL1]),
                               arg->key.t.len, adata);
    } else {
        ver1_alpm2_data_encode(u, arg->db, NEXT_LEVEL(ln), arg->default_route,
                               VER1_GROUP(arg->log_bkt[LEVEL2]),
                               arg->key.t.len, adata);
    }
}

static int
ver1_bucket_recover(int u, int ln, alpm_arg_t *arg)
{
    uint32_t phy_bkt;
    uint8_t ros;
    rbkt_group_t group;
    int db = arg->db;

    SHR_FUNC_ENTER(u);
    if (PREV_LEVEL(ln) == LEVEL1) {
        phy_bkt = arg->a1.phy_bkt;
        ros = arg->a1.rofs;
    } else {
        phy_bkt = arg->a2.phy_bkt;
        ros = arg->a2.rofs;
    }
    group = (phy_bkt * BKT_HDL_MAX_BANKS(u, db, ln)) + ros;
    SHR_IF_ERR_EXIT(
        ver1_rbkt_group_recover(u, db, ln, group, arg));
exit:
    SHR_FUNC_EXIT();
}

static int
ver1_bucket_recover_done(int u, int db, int ln)
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
        ver1_rbkt_group_free_bank_set(u, db, ln, prev_rbkt->index, free_banks);
        prev_group = group;
    }
    prev_rbkt = &RBKT(u, db, ln, prev_group);
    free_banks = BKT_HDL_RBKTS_TOTAL(u, db, ln) -
                 prev_rbkt->index - prev_rbkt->valid_banks;
    ALPM_ASSERT((int)free_banks >= 0);
    ver1_rbkt_group_free_bank_set(u, db, ln, prev_rbkt->index, free_banks);

    return SHR_E_NONE;
}

static int
ver1_bkt_hdl_init(int u, bool recover)
{
    alpm_dev_info_t *dev_info;
    int db_idx, level;
    int db_count;
    int db_max_levels[DBS] = {0};
    int rv, alpm_mode;
    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);
    MAX_FORMAT(u, FORMAT_PIVOT) = dev_info->pivot_max_format;
    MAX_FORMAT(u, FORMAT_ROUTE) = dev_info->route_max_format;

    db_count = bcmptm_rm_alpm_database_count(u);
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, db_idx);
        db_max_levels[db_idx] = database_ptr->max_levels;
        if (db_max_levels[db_idx] > 3) {
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
            int max_banks;
            int alloc_sz;
            rbkt_t *rbkt;

            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bkt_hdl_profile(u, alpm_mode,
                                               db_max_levels[DB0],
                                               db_max_levels[DB1],
                                               db_idx,
                                               level,
                                               &bkt_hdl_profile));

            ALPM_ALLOC(bkt_hdl, sizeof(bkt_hdl_t), "Ver1 bucket handle");
            sal_memcpy(bkt_hdl, bkt_hdl_profile, sizeof(*bkt_hdl));
            BKT_HDL(u, db_idx, level) = bkt_hdl;

            /* Bucket info */
            bkt_hdl->rbkts_total >>= bucket_shifts[u];
            bkt_hdl->bucket_bits -= bucket_shifts[u];
            bkt_hdl->groups_used = 1;
            ALPM_ASSERT(bkt_hdl->rbkts_total ==
                        bkt_hdl->max_banks * (1 << bkt_hdl->bucket_bits));

            SHR_DQ_INIT(&bkt_hdl->global_group_list);
            max_banks = bkt_hdl->max_banks;
            alloc_sz = (max_banks + 1) * sizeof(usage_list_t);
            ALPM_ASSERT(bkt_hdl->group_usage == NULL);
            ALPM_ALLOC(bkt_hdl->group_usage, alloc_sz, "group usage");
            sal_memset(bkt_hdl->group_usage, 0, alloc_sz);
            for (i = 0; i <= max_banks; i++) {
                SHR_DQ_INIT(&bkt_hdl->group_usage[i].groups);
            }
            bkt_hdl->group_usage_count = (max_banks + 1);

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
                SHR_DQ_INSERT_HEAD(&bkt_hdl->group_usage[max_banks].groups,
                                   &rbkt->usgnode);
            }

            for (i = 1; i < bkt_hdl->rbkts_total; i++) {
                rbkt_t *bbt = &bkt_hdl->rbkt_array[i];
                bbt->index = i;
            }
        }
    }
    LMEM_MGR_INIT(base_entries_ver1_t,
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
ver1_bkt_hdl_cleanup(int u)
{
    int i, j;
    int db_count;
    bkt_hdl_t *bkt_hdl;

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
                SHR_FREE(bkt_hdl->rbkt_array);
                SHR_FREE(bkt_hdl);
            }
        }
    }
}

static void
ver1_bucket_sanity(int u, int db, int ln, log_bkt_t log_bkt)
{
    ver1_rbkt_group_sanity(u, db, ln, VER1_GROUP(log_bkt), false, 0);
}
/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_bucket_ver1_register(int u, bucket_mgmt_t *bmgmt)
{
    SHR_FUNC_ENTER(u);

    if (bmgmt) {
        bmgmt->bkt_hdl_cleanup = ver1_bkt_hdl_cleanup;
        bmgmt->bkt_hdl_init = ver1_bkt_hdl_init;
        bmgmt->bkt_hdl_get = ver1_bkt_hdl_get;
        bmgmt->alpm_data_encode = ver1_alpm_data_encode;
        bmgmt->bucket_pivot_get = ver1_bucket_pivot_get;
        bmgmt->bucket_pivot_set = ver1_bucket_pivot_set;
        bmgmt->bucket_free = ver1_bucket_free;
        bmgmt->bucket_alloc = ver1_bucket_alloc;
        bmgmt->bucket_delete = ver1_bucket_delete;
        bmgmt->bucket_insert = ver1_bucket_insert;
        bmgmt->bucket_split = ver1_bucket_split;
        bmgmt->entry_write = ver1_base_entry_encode_write;
        bmgmt->entry_read = ver1_base_entry_decode_read;
        bmgmt->entry_hw_idx_get = ver1_hw_eidx_from_ent_idx;
        bmgmt->bucket_sanity= ver1_bucket_sanity;
        bmgmt->bucket_recover = ver1_bucket_recover;
        bmgmt->bucket_recover_done = ver1_bucket_recover_done;
    } else {
        SHR_RETURN_VAL_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

