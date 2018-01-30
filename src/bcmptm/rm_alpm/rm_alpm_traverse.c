/*! \file rm_alpm_traverse.c
 *
 * ALPM Traverse support
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

#include "rm_alpm_common.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"
#include "rm_alpm_trie.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_traverse.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE          BSL_LS_BCMPTM_RMALPM

#define SNAP_INFO(u, l)                (snap_info[u][l])
#define SNAP_NUM_ENTRIES(u, l)         (SNAP_INFO(u, l)->num_entries)
#define SNAP_TRIES(u, l)               (SNAP_INFO(u, l)->trie)
#define SNAP_VRF_S(u, l)               (SNAP_INFO(u, l)->vrf_s)
#define SNAP_VRF_E(u, l)               (SNAP_INFO(u, l)->vrf_e)


#define SNAP_TRIE(u, l, w_vrf)         (SNAP_INFO(u, l)->trie[w_vrf])
#define SNAP_LMM(u, l)                 (SNAP_INFO(u, l)->lmm)
#define SNAP_PEAK_IDX(u, l)            (SNAP_INFO(u, l)->peak_index)

#define SNAP_ENTRIES(u, l)             (SNAP_INFO(u, l)->entries)
#define SNAP_ENTRY(u, l, i)            (SNAP_INFO(u, l)->entries[i])
#define SNAP_ENTRY_INDEX(u, l, i)      (SNAP_ENTRY(u, l, i).index)
#define SNAP_ENTRY_NODE(u, l, i)       (SNAP_ENTRY(u, l, i).node)
#define SNAP_ENTRY_KEY(u, l, i)        (SNAP_ENTRY(u, l, i).key)

#define LT_IPV4_GLOBAL      0
#define LT_IPV4_VRF         1
#define LT_IPV4_OVERRIDE    2
#define LT_IPV6_GLOBAL      3
#define LT_IPV6_VRF         4
#define LT_IPV6_OVERRIDE    5
#define LT_NUM              6
/*******************************************************************************
  Typedefs
 */

typedef struct snap_key_s {
    int                 w_vrf;
    alpm_ip_ver_t       v6;
    int                 len;
    trie_ip_t          trie_key;
} snap_key_t;


typedef struct snap_entry_s {
    rm_alpm_trie_node_t node;
    snap_key_t          key;
    int                 index;
} snap_entry_t;


typedef struct snap_info_s {
    int             peak_index;
    snap_entry_t    *entries;
    int             num_entries;
    rm_alpm_trie_t  **trie;
    int             vrf_s;
    int             vrf_e;
    shr_lmm_hdl_t   lmm;
} snap_info_t;

typedef struct clone_data_s {
    int u;
    int index;
    int lt_idx;
} clone_data_t;


/*******************************************************************************
 * Private variables
 */

static snap_info_t *snap_info[BCMDRD_CONFIG_MAX_UNITS][LT_NUM] = {{NULL}};

/*******************************************************************************
 * Private Functions
 */
static int
route_lt_idx_encode(int u, alpm_vrf_type_t vt, alpm_ip_ver_t ipv)
{
    int idx = 0;

    switch (vt) {
    case VRF_PRIVATE:
        idx = ipv ? LT_IPV6_VRF : LT_IPV4_VRF;
        break;
    case VRF_GLOBAL:
        idx = ipv ? LT_IPV6_GLOBAL : LT_IPV4_GLOBAL;
        break;
    case VRF_OVERRIDE:
    default:
        idx = ipv ? LT_IPV6_OVERRIDE: LT_IPV4_OVERRIDE;
        break;
    }
    return idx;
}

static void
route_lt_idx_decode(int u, int lt_idx, alpm_vrf_type_t *vt, alpm_ip_ver_t *ipv)
{
    alpm_vrf_type_t tmp_vt;
    alpm_ip_ver_t tmp_ipv;

    switch (lt_idx) {
    case LT_IPV4_GLOBAL:
        tmp_vt = VRF_GLOBAL;
        tmp_ipv = IP_VER_4;
        break;
    case LT_IPV4_VRF:
        tmp_vt = VRF_PRIVATE;
        tmp_ipv = IP_VER_4;
        break;
    case LT_IPV4_OVERRIDE:
        tmp_vt = VRF_OVERRIDE;
        tmp_ipv = IP_VER_4;
        break;
    case LT_IPV6_GLOBAL:
        tmp_vt = VRF_GLOBAL;
        tmp_ipv = IP_VER_6;
        break;
    case LT_IPV6_VRF:
        tmp_vt = VRF_PRIVATE;
        tmp_ipv = IP_VER_6;
        break;
    case LT_IPV6_OVERRIDE:
    default:
        tmp_vt = VRF_OVERRIDE;
        tmp_ipv = IP_VER_6;
        break;
    }
    if (vt) {
        *vt = tmp_vt;
    }
    if (ipv) {
        *ipv = tmp_ipv;
    }
}

/*!
 * \brief Fast compare a given snap key to a peak index
 *
 * \param [in] snk Snapshot key
 * \param [in] lt_idx Route LT index
 * \param [in] index Peak index
 *
 * \return SHR_E_XXX
 */
static int
snap_key_cmp(int u, int lt_idx, snap_key_t *snk, int index)
{
    snap_key_t *snk2;
    int diff = 1;

    snk2 = &SNAP_ENTRY_KEY(u, lt_idx, index);

    if (snk2->w_vrf == snk->w_vrf &&
        snk2->len == snk->len &&
        snk2->v6 == snk->v6 &&
        !sal_memcmp(&snk2->trie_key, &snk->trie_key, sizeof(snk->trie_key))) {
        diff = 0;
    }

    return diff;
}


/*!
 * \brief Search a given key into the snapshot
 *
 * \param [in] snk Snapshot key
 * \param [in] lt_idx Route LT index
 * \param [in] snk Snap key
 * \param [out] index Found index as a subscript of the snapshot array
 *
 * \return SHR_E_XXX
 */
static int
snap_key_search(int u, int lt_idx, snap_key_t *snk, int *index)
{
    rm_alpm_trie_node_t *sn_node = NULL;
    rm_alpm_trie_t *trie;
    snap_entry_t *sne;

    SHR_FUNC_ENTER(u);

    if (!snap_key_cmp(u, lt_idx, snk, SNAP_PEAK_IDX(u, lt_idx))) {
        *index = SNAP_PEAK_IDX(u, lt_idx);
    } else {
        alpm_vrf_type_t vt;
        route_lt_idx_decode(u, lt_idx, &vt, NULL);
        if (vt == VRF_PRIVATE) {
            trie = SNAP_TRIE(u, lt_idx, snk->w_vrf);
        } else {
            trie = SNAP_TRIE(u, lt_idx, 0);
        }
        if (trie) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_trie_search(trie,
                                           snk->trie_key.key,
                                           snk->len,
                                           &sn_node));
            sne = TRIE_ELEMENT_GET(snap_entry_t*, sn_node, node);
            *index = sne->index;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief Snapshot entry clone callback
 *
 * \param [in] trie Source node
 * \param [out] clone_trie Cloned node
 * \param [in|out] datum User data
 *
 * \return SHR_E_XXX
 */
static int
entry_clone(rm_alpm_trie_node_t *trie,
            rm_alpm_trie_node_t **clone_trie,
            void *datum)
{
    alpm_pivot_t *payload = NULL;
    snap_entry_t *entry = NULL;
    clone_data_t *info = NULL;

    payload = (alpm_pivot_t *) trie;
    info = (clone_data_t *) datum;
    entry = &SNAP_ENTRY(info->u, info->lt_idx, info->index);

    if (entry && payload) {
        entry->key.w_vrf = payload->key.t.w_vrf;
        entry->key.v6 = payload->key.t.ipv;
        entry->key.len = payload->key.t.len;
        sal_memcpy(entry->key.trie_key.key, payload->key.t.trie_ip.key,
                   sizeof(payload->key.t.trie_ip));
        entry->index = info->index++;

        if (clone_trie) {
            *clone_trie = &entry->node;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Create ALPM snapshot
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_create(int u, int lt_idx, int max_levels)
{
    int w_vrf = 0;
    int w_vrf_s = 0, w_vrf_e = 0;
    alpm_ip_ver_t ipv = 0;
    int num_entries = 0;
    rm_alpm_trie_t *trie_src = NULL;
    alpm_dev_info_t *dev_info;
    clone_data_t clone_data;
    alpm_vrf_type_t vt;
    int vrf_count;
    int db, last_level;

    SHR_FUNC_ENTER(u);

    route_lt_idx_decode(u, lt_idx, &vt, &ipv);

    dev_info = bcmptm_rm_alpm_device_info_get(u);

    SHR_NULL_CHECK(dev_info, SHR_E_INTERNAL);

    switch (vt) {
    case VRF_PRIVATE:
        w_vrf_s = 0;
        w_vrf_e = dev_info->max_vrf;
        break;
    case VRF_GLOBAL:
        if (bcmptm_rm_alpm_mode(u) == ALPM_MODE_COMBINED) {
            w_vrf_s = w_vrf_e = COMBINED_WVRF_GLOBAL(dev_info->max_vrf);
        } else {
            w_vrf_s = w_vrf_e = PARALLEL_WVRF_GLOBAL(dev_info->max_vrf);
        }
        break;
    case VRF_OVERRIDE:
    default:
        w_vrf_s = w_vrf_e = COMBINED_WVRF_OVERRIDE;
        break;
    }

    db = bcmptm_rm_alpm_db(u, vt);
    last_level = max_levels - 1;

    /* Check if table empty before allocating anything */
    for (w_vrf = w_vrf_s; w_vrf <= w_vrf_e; w_vrf++) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_get(u, db, last_level,
                                        w_vrf, ipv, &trie_src));
        if (!trie_src) {
            continue;
        }
        
        num_entries += bcmptm_rm_alpm_trie_count(trie_src);
    }

    if (num_entries == 0) {
        SHR_EXIT();
    }

    ALPM_ALLOC(SNAP_INFO(u, lt_idx), sizeof(snap_info_t), "snap info");
    sal_memset(SNAP_INFO(u, lt_idx), 0, sizeof(snap_info_t));

    SNAP_VRF_S(u, lt_idx) = w_vrf_s;
    SNAP_VRF_E(u, lt_idx) = w_vrf_e;

    vrf_count = w_vrf_e - w_vrf_s + 1;

    ALPM_ALLOC(SNAP_TRIES(u, lt_idx),
               sizeof(rm_alpm_trie_t *) * vrf_count,
               "Tries");

    sal_memset(SNAP_TRIES(u, lt_idx), 0,
               sizeof(rm_alpm_trie_t *) * vrf_count);

    SNAP_NUM_ENTRIES(u, lt_idx) = num_entries;

    ALPM_ALLOC(SNAP_ENTRIES(u, lt_idx),
               sizeof(snap_entry_t) * SNAP_NUM_ENTRIES(u, lt_idx),
               "clone snap entries");
    sal_memset(SNAP_ENTRIES(u, lt_idx), 0,
               sizeof(snap_entry_t) * SNAP_NUM_ENTRIES(u, lt_idx));

    sal_memset(&clone_data, 0, sizeof(clone_data));
    clone_data.u = u;
    clone_data.lt_idx = lt_idx;

    for (w_vrf = w_vrf_s; w_vrf <= w_vrf_e; w_vrf++) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_get(u, db, last_level,
                                        w_vrf, ipv, &trie_src));
        if (!trie_src) {
            continue;
        }

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_clone_create(trie_src,
                                             entry_clone,
                                             &clone_data,
                                             &SNAP_TRIE(u, lt_idx, w_vrf - w_vrf_s),
                                             &SNAP_LMM(u, lt_idx)));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (SNAP_INFO(u, lt_idx)) {
            if (SNAP_TRIES(u, lt_idx)) {
                SHR_FREE(SNAP_TRIES(u, lt_idx));
            }

            if (SNAP_ENTRIES(u, lt_idx)) {
                SHR_FREE(SNAP_ENTRIES(u, lt_idx));
            }

            SHR_FREE(SNAP_INFO(u, lt_idx));
        }
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Sanity check against snapshot
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 *
 * \return SHR_E_XXX
 */
static int
snapshot_check(int u, int lt_idx)
{
    int i;
    SHR_FUNC_ENTER(u);

    if (!LOG_CHECK_INFO(BSL_LS_BCMPTM_RMALPM)) {
        SHR_EXIT();
    }

    for (i = 0; i < SNAP_NUM_ENTRIES(u, lt_idx); i++) {
        if (SNAP_ENTRY_INDEX(u, lt_idx, i) != i) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Destroy the snapshot
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 *
 * \return nothing
 */
static void
snapshot_destroy(int u, int lt_idx)
{
    int w_vrf = 0;
    int w_vrf_s = 0;

    if (!SNAP_INFO(u, lt_idx)) {
        return;
    }

    w_vrf_s = SNAP_VRF_S(u, lt_idx);
    for (w_vrf = w_vrf_s; w_vrf <= SNAP_VRF_E(u, lt_idx); w_vrf++) {
        bcmptm_rm_alpm_trie_clone_destroy(SNAP_TRIE(u, lt_idx, w_vrf - w_vrf_s),
                                          SNAP_LMM(u, lt_idx));
    }

    SHR_FREE(SNAP_ENTRIES(u, lt_idx));
    SHR_FREE(SNAP_TRIES(u, lt_idx));
    SHR_FREE(SNAP_INFO(u, lt_idx));

}

/*!
 * \brief Refresh the snapshot if already exists
 *
 * \param [in] u Device u.
 * \param [in] lt_idx Route LT index
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_refresh(int u, int lt_idx, int max_levels)
{
    SHR_FUNC_ENTER(u);

    snapshot_destroy(u, lt_idx);
    SHR_IF_ERR_EXIT(snapshot_create(u, lt_idx, max_levels));

exit:
    SHR_FUNC_EXIT();

}

static void
snap_key_to_arg(int u, snap_key_t *snk, alpm_arg_t *match_arg)
{
    if (match_arg && snk) {
        match_arg->key.t.w_vrf = snk->w_vrf;
        match_arg->key.t.ipv = snk->v6;
        match_arg->key.t.len = snk->len;
        sal_memcpy(&match_arg->key.t.trie_ip, &snk->trie_key,
                   sizeof(snk->trie_key));
    }
}

static void
snap_arg_to_key(int u, alpm_arg_t *arg, snap_key_t *snk)
{
    if (arg && snk) {
        snk->w_vrf = arg->key.t.w_vrf;
        snk->v6  = arg->key.t.ipv;
        snk->len = arg->key.t.len;
        sal_memcpy(snk->trie_key.key, arg->key.t.trie_ip.key, sizeof(trie_ip_t));
    }
}


/*!
 * \brief Create (or refresh) snapshot and get first entry in it
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_get_first(int u, alpm_arg_t *arg, int max_levels)
{
    int rv;
    int lt_idx;
    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    lt_idx = route_lt_idx_encode(u, arg->key.vt, arg->key.t.ipv);
    if (SNAP_INFO(u, lt_idx) == NULL) {
        SHR_IF_ERR_EXIT(snapshot_create(u, lt_idx, max_levels));
    } else {
        SHR_IF_ERR_EXIT(snapshot_refresh(u, lt_idx, max_levels));
    }

    if (SNAP_INFO(u, lt_idx) == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(snapshot_check(u, lt_idx));

    snap_key_to_arg(u, &SNAP_ENTRY_KEY(u, lt_idx, 0), arg);
    arg->set_key = 1;
    ALPM_ASSERT(arg->valid);
    rv = bcmptm_rm_alpm_match(u, arg, max_levels);

    if (SHR_SUCCESS(rv)) {
        SNAP_PEAK_IDX(u, lt_idx) = 0;
    }
    SHR_RETURN_VAL_EXIT(rv);

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get next entry in the snapshot based on given key.
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
static int
snapshot_get_next(int u, alpm_arg_t *arg, int max_levels)
{
    int idx = 0;
    int rv;
    snap_key_t snk;
    alpm_arg_t match_arg;
    int lt_idx;

    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    lt_idx = route_lt_idx_encode(u, arg->key.vt, arg->key.t.ipv);

    SHR_NULL_CHECK(SNAP_INFO(u, lt_idx), SHR_E_UNAVAIL);

    snap_arg_to_key(u, arg, &snk);

    rv = snap_key_search(u, lt_idx, &snk, &idx);

    if (rv == SHR_E_NOT_FOUND ||
        (rv == SHR_E_NONE && (idx + 1) >= SNAP_NUM_ENTRIES(u, lt_idx))) {
        snapshot_destroy(u, lt_idx);

        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

    } else if (SHR_FAILURE(rv)) {
        SHR_RETURN_VAL_EXIT(rv);
    }

    do {
        bcmptm_rm_alpm_arg_init(u, &match_arg);
        snap_key_to_arg(u, &SNAP_ENTRY_KEY(u, lt_idx, ++idx), &match_arg);
        match_arg.key.kt = arg->key.kt;
        match_arg.key.vt = arg->key.vt;

        match_arg.db = bcmptm_rm_alpm_db(u, arg->key.vt);
        match_arg.set_key = 1;
        match_arg.valid = 1;
        rv = bcmptm_rm_alpm_match(u, &match_arg, max_levels);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    } while (rv == SHR_E_NOT_FOUND && (idx + 1) < SNAP_NUM_ENTRIES(u, lt_idx));

    if (rv == SHR_E_NOT_FOUND) {
        snapshot_destroy(u, lt_idx);
        SHR_RETURN_VAL_EXIT(rv);
    }
    sal_memcpy(arg, &match_arg, sizeof(alpm_arg_t));
    SNAP_PEAK_IDX(u, lt_idx) = idx;

exit:
    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        snapshot_destroy(u, lt_idx);
    }

    SHR_FUNC_EXIT();

}


/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_get_first(int u, alpm_arg_t *arg, int max_levels)
{
    SHR_FUNC_ENTER(u);

    SHR_RETURN_VAL_EXIT(snapshot_get_first(u, arg, max_levels));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_get_next(int u, alpm_arg_t *arg, int max_levels)
{
    SHR_FUNC_ENTER(u);

    SHR_RETURN_VAL_EXIT(snapshot_get_next(u, arg, max_levels));
exit:
    SHR_FUNC_EXIT();

}


