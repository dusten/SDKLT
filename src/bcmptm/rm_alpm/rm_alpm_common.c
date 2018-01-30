/*! \file rm_alpm_common.c
 *
 * Databases for ALPM
 *
 * This file contains the databases for ALPM all levels.
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
#include <shr/shr_dq.h>

#include "rm_alpm.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM

#define BUCKETS_CHUNK_SIZE   512
#define ELEMS_CHUNK_SIZE     1024
#define ADATAS_CHUNK_SIZE    1024

/*******************************************************************************
 * Private variables
 */

/*!< ALPM databases */
alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS];
int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Private Functions
 */

static int
trie_mkl(int u, int v6)
{
    return v6 ? RM_ALPM_IPV6_KEY_SIZE : RM_ALPM_IPV4_KEY_SIZE;
}

/*!
 * \brief Initialize ALPM database
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX
 */
static int
database_init(int u, alpm_config_t *config)
{
    int db_idx;
    alpm_dev_info_t *dev_info;
    int vrf_cnt;

    SHR_FUNC_ENTER(u);

    dev_info = bcmptm_rm_alpm_device_info_get(u);

    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    /* Allocate and init ALPM Database */
    if (config->alpm_mode == ALPM_MODE_COMBINED) {
        ALPM_DB_CNT(u) = MAX_DBS_2;
    } else {
        ALPM_DB_CNT(u) = MAX_DBS_3;
    }

    ALPM_ALLOC(ALPM_DBS(u), sizeof(alpm_db_t) * ALPM_DB_CNT(u), "alpm db");
    sal_memset(ALPM_DBS(u), 0, sizeof(alpm_db_t) * ALPM_DB_CNT(u));

    /* Allocate and init ALPM Database */
    if (ALPM_DB_CNT(u) == MAX_DBS_2) {
        /* Global High */
        DB_VRF_HDL_CNT(u, DBH) = 1;
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DBH, IP_VER_4));
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DBH, IP_VER_6));

        /* Global Low + Private */
        DB_VRF_HDL_CNT(u, DB0) = COMBINED_WVRF_COUNT(dev_info->max_vrf);
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DB0, IP_VER_4));
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DB0, IP_VER_6));

    } else {
        assert(ALPM_DB_CNT(u) == MAX_DBS_3);
        /* Global High */
        DB_VRF_HDL_CNT(u, DBH) = 1;
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DBH, IP_VER_4));
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DBH, IP_VER_6));

        /* Private */
        DB_VRF_HDL_CNT(u, DB0) = PARALLEL_WVRF_COUNT(dev_info->max_vrf);
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DB0, IP_VER_4));
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DB0, IP_VER_6));

        /* Global Low */
        DB_VRF_HDL_CNT(u, DB1) = 1;
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DB1, IP_VER_4));
        SHR_DQ_INIT(&DB_VRF_HDL_LIST(u, DB1, IP_VER_6));
    }

    /* Database initialization */
    for (db_idx = 0; db_idx < ALPM_DB_CNT(u); db_idx++) {
        alpm_db_t *database_ptr = &ALPM_DB(u, db_idx);
        int rv;

        database_ptr->max_levels = config->db_levels[db_idx];

        /* VRF handle initialization */
        vrf_cnt = DB_VRF_HDL_CNT(u, db_idx);
        ALPM_ALLOC(database_ptr->vrf_hdl[IP_VER_4],
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt,
                   "IPv4 pivot hdl");
        ALPM_ALLOC(database_ptr->vrf_hdl[IP_VER_6],
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt,
                   "IPv6 pivot hdl");
        sal_memset(database_ptr->vrf_hdl[IP_VER_4], 0,
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt);
        sal_memset(database_ptr->vrf_hdl[IP_VER_6], 0,
                   sizeof(alpm_vrf_hdl_t) * vrf_cnt);

        LMEM_MGR_INIT
            (alpm_elem_t, database_ptr->elems, ELEMS_CHUNK_SIZE,
            false, rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }

        LMEM_MGR_INIT
            (elem_bkt_t, database_ptr->buckets, BUCKETS_CHUNK_SIZE,
            false, rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }

        LMEM_MGR_INIT
            (assoc_data_t, database_ptr->adatas, ADATAS_CHUNK_SIZE,
            false, rv);
        if (rv != 0) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }

    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup ALPM database
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX
 */
static void
database_cleanup(int u)
{
    int i;

    if (!ALPM_DBS(u)) {
        return;
    }

    for (i = 0; i < ALPM_DB_CNT(u); i++) {
        alpm_db_t *database_ptr = &ALPM_DB(u, i);

        SHR_FREE(database_ptr->vrf_hdl[IP_VER_4]);
        SHR_FREE(database_ptr->vrf_hdl[IP_VER_6]);

        shr_lmm_delete(database_ptr->elems);
        shr_lmm_delete(database_ptr->buckets);
        shr_lmm_delete(database_ptr->adatas);
    }

    SHR_FREE(ALPM_DBS(u));
}

/*******************************************************************************
 * Public Functions
 */
format_type_t
bcmptm_rm_alpm_format_type_get(int u, int db, int ln)
{
    if (DB_LAST_LEVEL(u, db) == ln) {
        return FORMAT_ROUTE;
    } else {
        return FORMAT_PIVOT;
    }
}

int
bcmptm_rm_alpm_db(int u, alpm_vrf_type_t vt)
{
    ALPM_ASSERT(ALPM_DB_CNT(u) == MAX_DBS_2 || ALPM_DB_CNT(u) == MAX_DBS_3);
    switch (vt) {
    case VRF_PRIVATE:
        return DB0;
    case VRF_OVERRIDE:
        return DBH;
    case VRF_GLOBAL:
    default:
        ALPM_ASSERT(vt == VRF_GLOBAL);
        return (ALPM_DB_CNT(u) == MAX_DBS_2) ? DB0 : DB1;
    }
}

int
bcmptm_rm_alpm_bkt_insert(int u, int ln, arg_key_t *key,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t *bkt_elem)
{
    SHR_FUNC_ENTER(u);
    /*
     * No bkt_elem in Level-1
     */
    ALPM_ASSERT(ln == LEVEL3 || ln == LEVEL2);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_insert(bkt->trie,
                                   &key->t.trie_ip.key[0],
                                   NULL,
                                   key->t.len,
                                   &bkt_elem->bktnode));
    SHR_DQ_INSERT_HEAD(&bkt->list, &bkt_elem->dqnode);
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_bkt_delete(int u, int ln, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bucket)
{
    rm_alpm_trie_node_t *bkt_elem_node = NULL;
    alpm_bkt_elem_t *bkt_elem;

    SHR_FUNC_ENTER(u);
    /*
     * No bucket in Level-1
     */
    ALPM_ASSERT(ln == LEVEL3 || ln == LEVEL2);
    SHR_NULL_CHECK(bkt->trie, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_delete(bkt->trie,
                                   tp->trie_ip.key,
                                   tp->len,
                                   &bkt_elem_node));
    bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, bkt_elem_node, bktnode);
    SHR_DQ_REMOVE(&bkt_elem->dqnode);
    if (bucket) {
        *bucket = bkt_elem;
    }

    /* Leat it to the bcmptm_rm_alpm_elem_free */
    /*
       if (bucket_trie->trie == NULL) {
        bcmptm_rm_alpm_trie_destroy(bucket_trie);
       }
     */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_bkt_lookup(int u, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bkt_elem)
{
    int rv;
    rm_alpm_trie_node_t *bkt_elem_node = NULL;
    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(bkt->trie, SHR_E_NOT_FOUND);
    rv = bcmptm_rm_alpm_trie_search(bkt->trie,
                                    tp->trie_ip.key,
                                    tp->len,
                                    &bkt_elem_node);
    if (SHR_SUCCESS(rv)) {
        *bkt_elem = SHR_DQ_ELEMENT_GET(alpm_bkt_elem_t*, bkt_elem_node, bktnode);
    }

    SHR_RETURN_VAL_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_elem_alloc(int u, int db, int ln,
                          key_tuple_t *t,
                          alpm_elem_t **elem_pyld)
{
    int mkl;
    alpm_elem_t *elem = NULL;
    elem_bkt_t *bkt = NULL;
    assoc_data_t *ad = NULL;
    SHR_FUNC_ENTER(u);

    elem = shr_lmm_alloc(DB_LMM_ELEMS(u, db));
    SHR_NULL_CHECK(elem, SHR_E_MEMORY);
    sal_memset(elem, 0, sizeof(alpm_elem_t));

    if (ln != DB_LAST_LEVEL(u, db)) {
        rm_alpm_trie_t *bucket_trie = NULL;
        bkt = shr_lmm_alloc(DB_LMM_BKTS(u, db));
        SHR_NULL_CHECK(bkt, SHR_E_MEMORY);
        sal_memset(bkt, 0, sizeof(*bkt));
        mkl = trie_mkl(u, t->ipv);
        bcmptm_rm_alpm_trie_create(mkl, &bucket_trie);
        bkt->trie = bucket_trie;
        SHR_DQ_INIT(&bkt->list);
    } else {
        ad = shr_lmm_alloc(DB_LMM_ADATAS(u, db));
        SHR_NULL_CHECK(ad, SHR_E_MEMORY);
        sal_memset(ad, 0, sizeof(*ad));
    }

    elem->ad = ad;
    elem->bkt = bkt;
    elem->index = INVALID_INDEX;

    /* Pivot key */
    sal_memcpy(&elem->key.t, t, sizeof(key_tuple_t));
    *elem_pyld = elem;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_elem_free(int u, int db, int ln, alpm_elem_t *elem)
{
    ALPM_ASSERT(elem);
    if (ln != DB_LAST_LEVEL(u, db)) {
        ALPM_ASSERT(elem->bkt && elem->bkt->trie && !elem->bkt->trie->trie);
        bcmptm_rm_alpm_trie_destroy(elem->bkt->trie);
        shr_lmm_free(DB_LMM_BKTS(u, db), elem->bkt);
    } else {
        ALPM_ASSERT(elem->ad);
        shr_lmm_free(DB_LMM_ADATAS(u, db), elem->ad);
    }

    shr_lmm_free(DB_LMM_ELEMS(u, db), elem);

    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_pvt_delete(int u, int db, int ln,
                          key_tuple_t *tp,
                          alpm_pivot_t **pivot)
{
    rm_alpm_trie_t *pivot_trie;
    rm_alpm_trie_node_t *pivot_node;

    SHR_FUNC_ENTER(u);

    pivot_trie = VRF_HDL_PTRIE(u, db, tp->ipv, tp->w_vrf, ln);
    SHR_NULL_CHECK(pivot_trie, SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_delete(pivot_trie,
                                   &tp->trie_ip.key[0],
                                   tp->len,
                                   &pivot_node));
    if (pivot) {
        *pivot = TRIE_ELEMENT_GET(alpm_pivot_t*, pivot_node, pvtnode);
    }

    if (pivot_trie->trie == NULL) { /* all nodes deleted */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_destroy(u, db, ln, tp->ipv, tp->w_vrf));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_lookup(int u, int db, int ln,
                          key_tuple_t *tp,
                          alpm_pivot_t **pivot)
{
    int rv;
    rm_alpm_trie_t *pivot_trie = NULL;
    rm_alpm_trie_node_t *pivot_node;

    SHR_FUNC_ENTER(u);

    pivot_trie = VRF_HDL_PTRIE(u, db, tp->ipv, tp->w_vrf, ln);
    if (pivot_trie) {
        rv = bcmptm_rm_alpm_trie_search(pivot_trie,
                                        &tp->trie_ip.key[0],
                                        tp->len,
                                        &pivot_node);
        if (SHR_SUCCESS(rv)) {
            if (pivot) {
                *pivot = TRIE_ELEMENT_GET(alpm_pivot_t*, pivot_node, pvtnode);
            }
        }
    } else {
        rv = SHR_E_NOT_FOUND;
    }
    SHR_RETURN_VAL_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_find(int u, int db, int ln,
                        key_tuple_t *tp,
                        alpm_pivot_t **pivot)
{
    int rv;
    rm_alpm_trie_t *pivot_trie;
    uint32_t lpm_pfx[RM_ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t lpm_len = 0;
    rm_alpm_trie_node_t *pivot_node;

    SHR_FUNC_ENTER(u);

    pivot_trie = VRF_HDL_PTRIE(u, db, tp->ipv, tp->w_vrf, ln);
    if (pivot_trie) {
        rv = bcmptm_rm_alpm_trie_find_lpm(pivot_trie,
                                          &tp->trie_ip.key[0],
                                          tp->len,
                                          &pivot_node,
                                          &lpm_pfx[0],
                                          &lpm_len);
        if (SHR_SUCCESS(rv)) {
            if (pivot) {
                *pivot = TRIE_ELEMENT_GET(alpm_pivot_t*, pivot_node, pvtnode);
                /* ALPM_ASSERT(pivot->key.t.len == lpm_len); */
            }
        }
    } else {
        rv = SHR_E_NOT_FOUND;
    }
    SHR_RETURN_VAL_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_insert(int u, int db, int ln,
                          arg_key_t *key,
                          alpm_pivot_t *pivot)
{
    rm_alpm_trie_t *pivot_trie;
    SHR_FUNC_ENTER(u);

    pivot_trie = VRF_HDL_PTRIE(u, db, key->t.ipv, key->t.w_vrf, ln);
    if (pivot_trie == NULL) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_trie_create(u, db, ln,
                                           key->t.ipv,
                                           key->t.w_vrf));
        pivot_trie = VRF_HDL_PTRIE(u, db, key->t.ipv, key->t.w_vrf, ln);
    }
    SHR_NULL_CHECK(pivot_trie, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_insert(pivot_trie,
                                   &key->t.trie_ip.key[0],
                                   NULL,
                                   key->t.len,
                                   &pivot->pvtnode));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pvt_trie_create(int u, int db, int ln, int ipv, int w_vrf)
{
    rm_alpm_trie_t *pivot_trie;
    uint32_t mkl = trie_mkl(u, ipv);
    bcmptm_rm_alpm_trie_create(mkl, &pivot_trie);
    VRF_HDL_PTRIE(u, db, ipv, w_vrf, ln) = pivot_trie;
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_pvt_trie_destroy(int u, int db, int ln, int ipv, int w_vrf)
{
    bcmptm_rm_alpm_trie_destroy(VRF_HDL_PTRIE(u, db, ipv, w_vrf, ln));
    VRF_HDL_PTRIE(u, db, ipv, w_vrf, ln) = NULL;
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_pvt_trie_get(int u, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t **pivot_trie)
{
    if (pivot_trie) {
        *pivot_trie = VRF_HDL_PTRIE(u, db, ipv, w_vrf, ln);
    }
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_pvt_trie_set(int u, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t *pivot_trie)
{
    VRF_HDL_PTRIE(u, db, ipv, w_vrf, ln) = pivot_trie;
    return SHR_E_NONE;
}

void
bcmptm_rm_alpm_vrf_route_inc(int u, int db, int w_vrf, int ipv, int *cnt)
{
    *cnt = ++VRF_HDL_RCNT(u, db, ipv, w_vrf);
}

void
bcmptm_rm_alpm_vrf_route_dec(int u, int db, int w_vrf, int ipv, int *cnt)
{
    *cnt = --VRF_HDL_RCNT(u, db, ipv, w_vrf);
}

int
bcmptm_rm_alpm_vrf_route_cnt(int u, int db, int w_vrf, int ipv)
{
    int count;
    count = VRF_HDL_RCNT(u, db, ipv, w_vrf);

    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        int last_level = DB_LAST_LEVEL(u, db);
        rm_alpm_trie_t *ptrie;

        bcmptm_rm_alpm_pvt_trie_get(u, db, last_level,
                                    w_vrf, ipv, &ptrie);
        if (ptrie) {
            ALPM_ASSERT(bcmptm_rm_alpm_trie_count(ptrie) == count);
        } else {
            ALPM_ASSERT(count == 0);
        }
    }
    return count;
}

alpm_db_t*
bcmptm_rm_alpm_database_get(int u, int db)
{
    return &ALPM_DB(u, db);
}

int
bcmptm_rm_alpm_database_count(int u)
{
    return ALPM_DB_CNT(u);
}

int
bcmptm_rm_alpm_database_init(int u, alpm_config_t *config)
{
    return database_init(u, config);
}

void
bcmptm_rm_alpm_database_cleanup(int u)
{
    database_cleanup(u);
}

