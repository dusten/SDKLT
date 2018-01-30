/*! \file rm_alpm_leveln.c
 *
 * Resource Manager for ALPM Level-N
 *
 * This file contains the resource manager for ALPM Level-N
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

#define LN_INFO(u)              (ln_info[u])
#define LN_SID(u, l)            (LN_INFO(u)->ln_sid[l])
#define LN_HIT_SID(u, l)        (LN_INFO(u)->ln_hit_sid[l])
#define LINFOS(u)               (LN_INFO(u)->lvl_hdl)
#define LVL_HDL(u, d, l)        (LN_INFO(u)->lvl_hdl[d][l])
#define LN_INFO_FD(u, _f)       (LN_INFO(u)->_f)
#define LN_INFO_FDM(u, _f, _i)  ((LN_INFO(u)->_f)[(_i)])
#define LINFO_INDEX(u, d, l)    (LVL_HDL(u, d, l)->level_idx)
#define LINFO_STATS(u, d, l)    (LVL_HDL(u, d, l)->level_stats)
#define SPLIT_MAX_TRIES         10

/*******************************************************************************
 * Typedefs
 */

/* Propagation data structure */
typedef struct propa_data_s {
    key_tuple_t t;
    alpm_route_t *bpm;
    uint32_t replace_bpm_len;
    int db;
    int u;
    trie_propagate_op_t propa_op;
    int level;
    int propa_cnt;
} propa_data_t;

typedef struct pivot_traverse_data_s {
    int u;
    rm_alpm_trie_t *route_trie;
} pivot_traverse_data_t;

/*******************************************************************************
 * Private variables
 */
static alpm_ln_info_t *ln_info[BCMDRD_CONFIG_MAX_UNITS] = {NULL};
extern alpm_db_t *bcmptm_rm_alpm_database[BCMDRD_CONFIG_MAX_UNITS];
extern int bcmptm_rm_alpm_database_cnt[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions (declaration)
 */
static int alpm_insert(int u, int ln, alpm_arg_t *arg,
                       bkt_split_aux_t *split_aux);
static int alpm_delete(int u, int ln, alpm_arg_t *arg);
static inline int pivot_update(int u, int db, int ln, alpm_pivot_t *pivot);
/*******************************************************************************
 * Private Functions (definition)
 */

static int
level_hdl_get(int u, int db, int ln, lvl_hdl_t **lvl_hdl)
{
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(lvl_hdl);
    ALPM_ASSERT(db < DBS);
    ALPM_ASSERT(ln < LEVELS);
    *lvl_hdl = LVL_HDL(u, db, ln);
    SHR_NULL_CHECK(*lvl_hdl, SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

static inline void
format_get(int u, alpm_arg_t *arg)
{
    if (LN_INFO_FD(u, alpm_ver) != ALPM_VERSION_0) {
        return;
    }
    if (arg->key.t.ipv == IP_VER_6) {
        if (bcmptm_rm_alpm_pkm128_is_set(u)) {
            arg->fmt = VER0_FORMAT_5;
        } else {
            arg->fmt = VER0_FORMAT_2;
        }
    } else {
        arg->fmt = VER0_FORMAT_1;
    }
}

/*!
 * \brief Get data type based on arg
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg
 *
 * \return DATA_FULL, DATA_REDUCED.
 */
static inline alpm_data_type_t
data_type_get(int u, alpm_arg_t *arg)
{
    return DATA_REDUCED;
}

/*!
 * \brief Check if data type of arg fit existing database.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg
 *
 * \return SHR_E_NONE, SHR_E_PARAM.
 */
static int
data_type_check(int u, alpm_arg_t *arg)
{
    alpm_vrf_hdl_t *vrf_hdl;
    int fit = 1;

    SHR_FUNC_ENTER(u);

    vrf_hdl = &VRF_HDL(u, arg->db, arg->key.t.ipv, arg->key.t.w_vrf);

    if (vrf_hdl->in_use) {
        switch (vrf_hdl->vrf_data_type) {
        case VRF_FIXED_DATA_FULL:
            fit = 1;
            break;
        case VRF_FIXED_DATA_REDUCED:
            fit = (data_type_get(u, arg) == DATA_REDUCED);
            break;
        case VRF_DYNAMIC_DATA:
        default:
            fit = 1;
            break;
        }
    }
    if (!fit) {
        SHR_RETURN(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
arg_from_pivot(int u, int db, int ln, alpm_pivot_t *pivot,
               bool key_only, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);
    bcmptm_rm_alpm_arg_init(u, arg);
    arg->db = db;
    arg->hit = 0; 
    arg->pivot[ln] = pivot;
    if (ln == LEVEL2) {
        arg->pivot[PREV_LEVEL(ln)] =
            bcmptm_rm_alpm_bucket_pivot_get(u, db, ln, pivot->index);
    }
    /* Key type will be set in bcmptm_rm_alpm_l1_insert */
    arg->key.vt = pivot->key.vt;
    arg->log_bkt[ln] = pivot->bkt->log_bkt;
    arg->valid = 1;
    sal_memcpy(&arg->key.t, &pivot->key.t, sizeof(key_tuple_t));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_alpm_api_key_create(u, arg->key.t.ipv,
                                       &arg->key.t.trie_ip,
                                       arg->key.t.len,
                                       arg->key.ip_addr));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_len_to_mask(u, arg->key.t.ipv, arg->key.t.len,
                                   &arg->key.ip_mask[0]));
    if (key_only) {
        SHR_EXIT();
    }

    sal_memcpy(&arg->ad, pivot->bkt->bpm->ad, sizeof(assoc_data_t));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parameters check for insert
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
insert_validate(int u, alpm_arg_t *arg)
{
    int alpm_mode;
    SHR_FUNC_ENTER(u);

    alpm_mode = bcmptm_rm_alpm_mode(u);
    if (arg->key.t.w_vrf == 0) {
        if (alpm_mode == ALPM_MODE_PARALLEL) {
            /* cannot have 0 as a VRF in parallel mode */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(u,
                                  "VRF=0 cannot be added in current mode\n")));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* combined search mode protection */
    if (arg->key.vt != VRF_GLOBAL) {
        if (alpm_mode == ALPM_MODE_COMBINED) {
            /* for emtpy DB allow insert only if route is default */

            if (bcmptm_rm_alpm_vrf_route_cnt(u, arg->db, arg->key.t.w_vrf,
                                             arg->key.t.ipv) == 0) {
                /* check if route is default, else throw error */
                if (!arg->default_route) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(u,
                                          "VRF %d: First route in a VRF has to "
                                          "be a default route in this mode\n"),
                               arg->key.t.w_vrf));
                    SHR_IF_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }
    }

    if (LN_INFO_FD(u, alpm_ver) == ALPM_VERSION_0) {
        if (!bcmptm_rm_alpm_pkm128_is_set(u)) {
            if (arg->key.t.len > 64) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(u,
                                      "Ip_len %d: must configure pkm128 "
                                      "for ip_len > 64 \n"),
                           arg->key.t.w_vrf));
                SHR_RETURN(SHR_E_PARAM);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parameters check for delete
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
static int
delete_validate(int u, alpm_arg_t *arg)
{
    int route_cnt;
    int alpm_mode;
    SHR_FUNC_ENTER(u);

    route_cnt = bcmptm_rm_alpm_vrf_route_cnt(u, arg->db, arg->key.t.w_vrf,
                                             arg->key.t.ipv);
    alpm_mode = bcmptm_rm_alpm_mode(u);
    if (route_cnt > 1 &&
        alpm_mode == ALPM_MODE_COMBINED &&
        arg->key.vt == VRF_PRIVATE &&
        arg->key.t.len == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(u,
                              "VRF %d: Default route has to "
                              "be the last route to delete in this mode\n"),
                   arg->key.t.w_vrf));

        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Wrapper for adding entry on any level but last level
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for new entry
 * \param [in] split_aux Auxilliary data for split
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
pivot_insert(int u, int ln, alpm_arg_t *arg, bkt_split_aux_t *split_aux)
{
    alpm_pivot_t *pivot;
    SHR_FUNC_ENTER(u);

    if (ln == LEVEL1) {
        pivot = NULL;
        /* Level-1 insert */
        bcmptm_rm_alpm_adata_encode(u, ln, arg, &arg->a1);

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_elem_alloc(u, arg->db, ln, &arg->key.t, &pivot));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_insert(u, arg->db, ln, &arg->key, pivot));
        arg->pivot[ln] = pivot;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_insert(u, arg));
        pivot->index = arg->index[ln];
        pivot->key.vt = arg->key.vt;
    } else {
        int rv;
        ALPM_ASSERT(ln == LEVEL2);
        bcmptm_rm_alpm_adata_encode(u, ln, arg, &arg->a2);
        rv = alpm_insert(u, ln, arg, split_aux);
        if (rv == SHR_E_PARTIAL) {
            SHR_RETURN(rv);
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    ALPM_ASSERT(arg->index[ln] >= 0);
exit:
    if (SHR_FUNC_ERR()) {
        if (ln == LEVEL1) {
            if (pivot) {
                /* Assume fail on l1_insert only */
                bcmptm_rm_alpm_pvt_delete(u, arg->db, ln, &arg->key.t, &pivot);
                bcmptm_rm_alpm_elem_free(u, arg->db, ln, pivot);
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Wrapper for deleting entry on any level but last level.
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for deleting entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
pivot_delete(int u, int ln, alpm_arg_t *arg)
{
    SHR_FUNC_ENTER(u);

    if (ln == LEVEL1) {
        alpm_pivot_t *pivot;
        arg->a1.direct_route = 0; /* to skip Level-1 pivot mgmt */
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_delete(u, arg));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_delete(u, arg->db, ln, &arg->key.t, &pivot));
        /* Save the group so we can free later */
        arg->log_bkt[LEVEL1] = pivot->bkt->log_bkt;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_elem_free(u, arg->db, ln, pivot));
    } else {
        ALPM_ASSERT(ln == LEVEL2);
        SHR_IF_ERR_EXIT(alpm_delete(u, ln, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update elem ad or alpm_data for a given level.
 *
 * The elem should exist, and the key part should be same as before.
 * Everything that needs updated are in the elem, need encode to arg first.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pivot Elem(pivot) for previous level
 * \param [in] split_arg ALPM arg for previous level, honour this if set.
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
pivot_update_with_arg(int u, int db, int ln,
                      alpm_pivot_t *pivot, alpm_arg_t *arg)
{
    alpm_arg_t tmp_arg;

    SHR_FUNC_ENTER(u);

    if (arg) {
        ALPM_ASSERT(db == arg->db);
    } else {
        arg = &tmp_arg;
        arg_from_pivot(u, db, ln, pivot, false, arg);
    }

    arg->index[ln] = pivot->index;

    if (ln == LEVEL1) {
        /* Level-1 update */
        bcmptm_rm_alpm_adata_encode(u, ln, arg, &arg->a1);
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_l1_insert(u, arg));

    } else {
        ALPM_ASSERT(ln == LEVEL2);
        bcmptm_rm_alpm_adata_encode(u, ln, arg, &arg->a2);

        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_entry_write(u, ln, arg));

#ifdef ALPM_DEBUG
        if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
            alpm_pivot_t *pre_pivot;
            alpm_bkt_elem_t *bkt_elem = NULL;
            alpm_pivot_t *pre_pivot2 = NULL;
            pre_pivot = arg->pivot[PREV_LEVEL(ln)];
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_pvt_find(u, db, PREV_LEVEL(ln),
                                        &arg->key.t, &pre_pivot2));
            /* ALPM_ASSERT(pre_pivot == pre_pivot2);  This can be false */
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bkt_lookup(u, &arg->key.t, pre_pivot->bkt,
                                          &bkt_elem));
            ALPM_ASSERT(bkt_elem == pivot);
        }
#endif
    }
exit:
    SHR_FUNC_EXIT();
}

static inline int
pivot_update(int u, int db, int ln, alpm_pivot_t *pivot)
{
    return pivot_update_with_arg(u, db, ln, pivot, NULL);
}

/*!
 * \brief And a new pivot which just split from a parent pivot.
 *
 * Basically, this is to add a new elem for a level.
 * The elem should not exist.
 * This will also set bpm ad to split_arg.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] split_arg Split arg
 * \param [out] split_pivot Split pivot
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
split_pivot_add(int u, int db, int ln, alpm_arg_t *split_arg,
                alpm_pivot_t **split_pivot,
                bkt_split_aux_t *split_aux)
{
    int rv;
    alpm_bkt_elem_t *bpm_elem = NULL;

    SHR_FUNC_ENTER(u);

    rv = bcmptm_rm_alpm_pvt_find(u, db,
                                 DB_LAST_LEVEL(u, db),
                                 &split_arg->key.t,
                                 &bpm_elem);
    if (rv == SHR_E_NOT_FOUND) {
        /* For parallel mode */
        bpm_elem = VRF_HDL_DFT(u, db, split_arg->key.t.ipv,
                               split_arg->key.t.w_vrf);
        ALPM_ASSERT(bpm_elem->key.t.len == -1);
    }

    sal_memcpy(&split_arg->ad, bpm_elem->ad, sizeof(assoc_data_t));
    split_arg->skip_write = WRITE_LATER; /* Activate it later */
    /* Will alloc pivot & assign to arg */
    rv = pivot_insert(u, ln, split_arg, split_aux);
    if (rv == SHR_E_PARTIAL) {
        SHR_RETURN(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    (*split_pivot) = split_arg->pivot[ln];
    (*split_pivot)->bkt->bpm = bpm_elem;
    (*split_pivot)->bkt->log_bkt = split_arg->log_bkt[ln];
    split_arg->skip_write = WRITE_NEW;

    /* Index should be set already */
    ALPM_ASSERT(split_arg->index[ln] >= 0);
    ALPM_ASSERT(split_arg->index[ln] == (*split_pivot)->index);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a new pivot which just split from a parent pivot.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] split_arg Split arg
 * \param [in] split_aux Split auxilliary data
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
split_pivot_delete(int u, int db, int ln, alpm_arg_t *split_arg,
                   bkt_split_aux_t *split_aux)
{
    int rv;

    SHR_FUNC_ENTER(u);

    split_arg->skip_write = WRITE_LATER; /* Don't write */
    rv = pivot_delete(u, ln, split_arg);
    if (rv == SHR_E_PARTIAL) {
        SHR_RETURN(rv);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    split_arg->skip_write = WRITE_NEW;

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Init propagation data
 *
 * \param [in] u Device unit
 * \param [in] arg ALPM arg
 * \param [in] bpm Best prefix match
 * \param [in] op Propagation op code
 * \param [out] propa_data Propagation data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static void
propa_data_init(int u, alpm_arg_t *arg,
                alpm_route_t *bpm,
                trie_propagate_op_t op,
                propa_data_t *propa_data)
{
    sal_memcpy(&propa_data->t, &arg->key.t, sizeof(key_tuple_t));
    propa_data->bpm = bpm;
    propa_data->db = arg->db;
    propa_data->u = u;
    propa_data->propa_op = op;
    return;
}

/*!
 * \brief Callback for insert propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
insert_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int rv, pfx_len;
    propa_data_t *propa_data;
    alpm_pivot_t *pivot;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    propa_data = info->user_data;
    pfx_len = propa_data->t.len;

    /* INSERT propagation targets pivot whose bpm_len <= new ip_len */

    /*
     * pivot->bkt->bpm->key.t.len == pfx_len is possible only when
     * this pivot is just created during split, and it happens to
     * be same as the new prefix.
     */
    if (pivot->bkt->bpm->key.t.len > pfx_len) {
/*        return SHR_E_LIMIT;*/
        return SHR_E_NONE;
    }
    pivot->bkt->bpm = propa_data->bpm;
    ALPM_ASSERT(pfx_len == pivot->bkt->bpm->key.t.len);
    rv = pivot_update(propa_data->u, propa_data->db,
                      propa_data->level, pivot);
    propa_data->propa_cnt++;
    return rv;
}

/*!
 * \brief Callback for delete propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
delete_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int rv, pfx_len;
    propa_data_t *propa_data;
    alpm_pivot_t *pivot;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    propa_data = info->user_data;
    pfx_len = propa_data->t.len;

    /* DELETE propagation targets pivot whose bpm_len == deleting ip_len */
    if (pivot->bkt->bpm->key.t.len != pfx_len) {
/*        return SHR_E_LIMIT;*/
        return SHR_E_NONE;
    }
    pivot->bkt->bpm = propa_data->bpm;
    ALPM_ASSERT(pfx_len > pivot->bkt->bpm->key.t.len);
    rv = pivot_update(propa_data->u, propa_data->db,
                      propa_data->level, pivot);
    propa_data->propa_cnt++;
    return rv;
}

/*!
 * \brief Callback for update propagation
 *
 * \param [in] node Trie node
 * \param [in] info Callback helper data
 *
 * \return SHR_E_NONE/SHR_E_LIMIT/SHR_E_XXX.
 */
static int
update_propa_cb(rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info)
{
    int rv, pfx_len;
    propa_data_t *propa_data;
    alpm_pivot_t *pivot;

    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    propa_data = info->user_data;
    pfx_len = propa_data->t.len;

    /* UPDATE propagation targets pivot whose bpm_len == updating ip_len */
    if (pivot->bkt->bpm->key.t.len != pfx_len) {
/*        return SHR_E_LIMIT;*/
        return SHR_E_NONE;
    }
    /* BPM does not change, thus skip updating it. */
    rv = pivot_update(propa_data->u, propa_data->db,
                      propa_data->level, pivot);
    propa_data->propa_cnt++;
    return rv;
}

/*!
 * \brief Software propagation
 *
 * \param [in] u Device u.
 * \param [in] propa_op Type of propagation
 * \param [in] arg ALPM arg for adding/deleting/updating entry
 * \param [in] bpm Best prefix match
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_propa(int u, trie_propagate_op_t propa_op,
           alpm_arg_t *arg, alpm_route_t *bpm)
{
    int level;
    rm_alpm_trie_t *pivot_trie;
    propa_data_t propa_data;
    int (*cb) (rm_alpm_trie_node_t *node, rm_alpm_trie_bpm_cb_info_t *info);

    SHR_FUNC_ENTER(u);

    propa_data_init(u, arg, bpm, propa_op, &propa_data);

    switch (propa_op) {
    case PROPOGATE_INSERT: cb = insert_propa_cb; break;
    case PROPOGATE_DELETE: cb = delete_propa_cb; break;
    case PROPOGATE_UPDATE: cb = update_propa_cb; break;
    default: cb = NULL; break;
    }

    for (level = 0; level < DB_LAST_LEVEL(u, arg->db); level++) {
        rm_alpm_trie_bpm_cb_info_t cb_info;

        propa_data.level = level;
        bcmptm_rm_alpm_pvt_trie_get(u, arg->db, level,
                                    arg->key.t.w_vrf,
                                    arg->key.t.ipv, &pivot_trie);
        ALPM_ASSERT(pivot_trie);

        cb_info.user_data = &propa_data;
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_trie_propagate(pivot_trie,
                                          &propa_data.t.trie_ip.key[0],
                                          propa_data.t.len,
                                          propa_op,
                                          cb,
                                          &cb_info,
                                          false));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an existing entry on a given level
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for deleting entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_delete(int u, int ln, alpm_arg_t *arg)
{
    int rv = SHR_E_NONE;
    ln_index_t ent_idx;
    alpm_pivot_t *pivot = NULL;
    alpm_bkt_elem_t *bkt_elem = NULL;
    alpm_pivot_t *pivot_elem = NULL;
    alpm_bkt_elem_t *bpm_elem;
    int prev_ln = ln - 1;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(prev_ln >= LEVEL1);
    if (arg->pivot[prev_ln] == NULL) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, arg->db, prev_ln, &arg->key.t, &pivot));
        arg->pivot[prev_ln] = pivot;
    } else {
        pivot = arg->pivot[prev_ln];
    }

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bkt_delete(u, ln, &arg->key.t, pivot->bkt, &bkt_elem));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_delete(u, arg->db, ln, &arg->key.t, &pivot_elem));
    ALPM_ASSERT(bkt_elem == pivot_elem);
    /* Save the group so we can free later */
    if (pivot_elem->bkt) {
        arg->log_bkt[ln] = pivot_elem->bkt->log_bkt;
    }
    ent_idx = bkt_elem->index;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_free(u, arg->db, ln, bkt_elem));

    /* Find the bpm_elem after bucket entry got deleted */
    rv = bcmptm_rm_alpm_pvt_find(u, arg->db,
                                 DB_LAST_LEVEL(u, arg->db),
                                 &arg->key.t,
                                 &bpm_elem);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    if (rv == SHR_E_NOT_FOUND) {
        /* For parallel mode */
        bpm_elem = VRF_HDL_DFT(u, arg->db, arg->key.t.ipv, arg->key.t.w_vrf);
    }
    pivot->bkt->bpm = bpm_elem;

    if (ln == DB_LAST_LEVEL(u, arg->db)) {
        SHR_IF_ERR_EXIT(
            alpm_propa(u, PROPOGATE_DELETE, arg, bpm_elem));
    }

    /*
     * Delete bucket entry after propagation done.
     * Then it is fine to delete bucket entry before deleting pivot.
     */
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bucket_delete(u, arg->db, ln, pivot, ent_idx));

    if (pivot->bkt->trie->trie == NULL || pivot->bkt->trie->trie->count == 1) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_clean(u, arg->db, ln, pivot));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert a new entry on a given level
 *
 * \param [in] u Device u.
 * \param [in] ln Level to insert
 * \param [in] arg ALPM arg for new entry
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_insert(int u, int ln, alpm_arg_t *arg, bkt_split_aux_t *split_aux)
{
    int rv = SHR_E_NONE;
    alpm_pivot_t *elem = NULL;
    alpm_pivot_t *pivot = NULL;
    int prev_ln = ln - 1;
    static int insert = 0;
    enum {
        INSERT_NONE,
        INSERT_ELEM_ALLOC,
        INSERT_ELEM_BKT_INSERT,
        INSERT_ELEM_PVT_INSERT,
    } done = INSERT_NONE;
    SHR_FUNC_ENTER(u);

    if (arg->pivot[prev_ln] == NULL) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, arg->db, prev_ln, &arg->key.t, &pivot));
        arg->pivot[prev_ln] = pivot;
    } else {
#ifdef ALPM_DEBUG
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_pvt_find(u, arg->db, prev_ln, &arg->key.t, &pivot));
        ALPM_ASSERT(pivot == arg->pivot[prev_ln]);
#endif
        pivot = arg->pivot[prev_ln];
    }

    bcmptm_rm_alpm_bucket_sanity(u, arg->db, ln, pivot->bkt->log_bkt);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, arg->db, ln, &arg->key.t, &elem));
    done = INSERT_ELEM_ALLOC;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bkt_insert(u, ln, &arg->key, pivot->bkt, elem));
    done = INSERT_ELEM_BKT_INSERT;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_insert(u, arg->db, ln, &arg->key, elem));
    done = INSERT_ELEM_PVT_INSERT;

    insert++;
    rv = bcmptm_rm_alpm_bucket_insert(u, ln, arg, pivot);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_FULL);
    if (rv == SHR_E_FULL) {
        int try_cnt = 0;
        bkt_split_aux_t split_aux2;
        bkt_split_aux_t split_aux3;
        for (try_cnt = 0; try_cnt < SPLIT_MAX_TRIES; try_cnt++) {
            rv = bcmptm_rm_alpm_bucket_split(u, ln, arg, pivot, split_aux);
            if (SHR_SUCCESS(rv)) {
                ALPM_ASSERT(arg->index[ln] >= 0);
                break;
            } else if (rv == SHR_E_BADID) { /* Continue split */
                /* Imply new in parent, we are just fine to continue split */
                continue;
            } else if (rv == SHR_E_PARTIAL) {
                /* Just ignore it for non-level split */
                if (ln != DB_LAST_LEVEL(u, arg->db)) {
                    SHR_RETURN(rv);
                }
                arg->a1.rsv0 = 1;
                if (split_aux->aux_parent_pivot[ln] == pivot) {
                    /*
                     * Special split scenario 2.
                     * No second split, but still need resplit the last level to
                     * be the same split point as previous level split point.
                     */
                    rv = bcmptm_rm_alpm_bucket_split(u, ln, arg, pivot,
                                                     split_aux);
                    if (rv == SHR_E_BADID) { /* Retry */
                        /* This time, no special split */
                        sal_memset(&split_aux3, 0, sizeof(split_aux3));
                        split_aux = &split_aux3;
                        continue;
                    } else {
                        SHR_IF_ERR_EXIT(rv);
                        break;
                    }
                } else {
                    /*
                     * Special split scenario 1.
                     * Do the second split is as neeeded.
                     */
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_alpm_bucket_split(u, ln, arg,
                            split_aux->aux_parent_pivot[ln], split_aux));
                    sal_memset(&split_aux2, 0, sizeof(split_aux2));
                    split_aux = &split_aux2;
                    try_cnt--; /* Second split does not count */
                    continue;
                }
            } else if (rv == SHR_E_FULL) {
                 /* For SPLIT_EFULL_REASON_PIVOT_FULL */
                SHR_IF_ERR_EXIT(rv);
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        if (try_cnt == SPLIT_MAX_TRIES && SHR_FAILURE(rv)) {
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }
    }

    elem->index = arg->index[ln];
    arg->pivot[ln] = elem;
    bcmptm_rm_alpm_bucket_sanity(u, arg->db, ln, pivot->bkt->log_bkt);

    /* Do insert propagation to update existing default datas */
    if (ln == DB_LAST_LEVEL(u, arg->db)) {
        sal_memcpy(elem->ad, &arg->ad, sizeof(assoc_data_t));
        SHR_IF_ERR_EXIT(
            alpm_propa(u, PROPOGATE_INSERT, arg, elem));
    }

#if 0
    if (ALPM_INFO(u)->alpm_dbg_ctrl.f.pivot_sanity_node) {
        rv = alpm_pivot_sanity_cb(u, ln, pivot, "CBInsert");
    }
#endif

exit:
    if (SHR_FUNC_ERR()) {
        switch (done) {
        case INSERT_ELEM_PVT_INSERT:
            bcmptm_rm_alpm_bkt_delete(u, ln, &arg->key.t, pivot->bkt, &elem);
        case INSERT_ELEM_BKT_INSERT:
            bcmptm_rm_alpm_pvt_delete(u, arg->db, ln, &arg->key.t, &elem);
        case INSERT_ELEM_ALLOC:
            bcmptm_rm_alpm_elem_free(u, arg->db, ln, elem);
        case INSERT_NONE:
        default:
            /* nothing to roll back */
            break;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update a route's assoc data
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg with new assoc data and index
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_update(int u, alpm_arg_t *arg)
{
    int ln = DB_LAST_LEVEL(u, arg->db);
    SHR_FUNC_ENTER(u);

    /* Pivot should be set in position during search */
    ALPM_ASSERT(arg->pivot[PREV_LEVEL(ln)]);

    /*
     * When update, we always update the info in the last level, therefore
     * we need to make sure all info for the last level are prepared in arg,
     * Currently, the only info that beyonds key and assoc data is sub_bkt
     * on ver0 device.
     *
     * Sub_bkt is supposed to be fetched from last level entry and then encode
     * to tcam entry during lookup. When performing update, it should be
     * decoded into arg already, thus no worry figuring it out either.
     */

    /* Set hit */
    arg->set_hit = 1;
    arg->index[ln] = arg->pivot[ln]->index;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_entry_write(u, ln, arg));

    SHR_IF_ERR_EXIT(
        alpm_propa(u, PROPOGATE_UPDATE, arg, NULL));

    *(arg->pivot[ln]->ad) = arg->ad;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup pivot(s) for default route for a new VRF.
 *
 * Basically, this is to add a default (0/0) route pivot(s).
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] key Arg key
 * \param [in] vrf_hdl Pointer to vrf handle
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_vrf_add(int u, alpm_arg_t *arg, alpm_vrf_hdl_t *vrf_hdl)
{
    alpm_arg_t root_arg;
    int ln = -1; /* for proper roll back */
    alpm_route_t *bpm_elem = NULL;
    alpm_pivot_t *pivot;
    arg_key_t *key;
    int db;
    SHR_FUNC_ENTER(u);

    /* Default route: Key = 0, Mask = 0 */
    bcmptm_rm_alpm_arg_init(u, &root_arg);
    db = arg->db;
    key = &arg->key;
    root_arg.db = db;
    root_arg.key.t.ipv = key->t.ipv;
    root_arg.key.t.w_vrf = key->t.w_vrf;
    /* Key type will be set in bcmptm_rm_alpm_l1_insert */
    root_arg.key.vt = key->vt;
    root_arg.default_route = 1;
    root_arg.valid = 1;

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, db, DB_LAST_LEVEL(u, db),
                                  &root_arg.key.t, &bpm_elem));
    bpm_elem->key.t.len = -1; /* Indicating this is a fake one */

    ln = LEVEL1;
    while (ln < DB_LAST_LEVEL(u, db)) {
        bcmptm_rm_alpm_bucket_alloc(u, NEXT_LEVEL(ln), arg, &root_arg.log_bkt[ln]);
        SHR_IF_ERR_EXIT(
            pivot_insert(u, ln, &root_arg, NULL));
        pivot = root_arg.pivot[ln];
        pivot->bkt->bpm = bpm_elem;
        pivot->bkt->log_bkt = root_arg.log_bkt[ln];
        ln++;
        /* Next Level */
        bcmptm_rm_alpm_bucket_pivot_set(u, db, ln, pivot->bkt->log_bkt, pivot);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_create(u, db, ln, key->t.ipv, key->t.w_vrf));

    /*
     * Note that the prefix is not inserted in this routine,
     * only the pivots are inserted.
     */
    vrf_hdl->ipv = key->t.ipv;
    vrf_hdl->w_vrf = key->t.w_vrf;
    vrf_hdl->in_use = TRUE;
    
    vrf_hdl->vrf_data_type = VRF_FIXED_DATA_FULL;
    vrf_hdl->parallel_default = bpm_elem;
    SHR_DQ_INSERT_HEAD(&DB_VRF_HDL_LIST(u, db, key->t.ipv), &vrf_hdl->dqnode);

exit:
    if (SHR_FUNC_ERR()) {
        while (ln >= LEVEL1) {
            if (ln < DB_LAST_LEVEL(u, db)) {
                if (root_arg.pivot[ln]) {
                    pivot_delete(u, ln, &root_arg);
                }
                if (root_arg.log_bkt[ln] != INVALID_GROUP) {
                    bcmptm_rm_alpm_bucket_free(u, db, NEXT_LEVEL(ln),
                                               root_arg.log_bkt[ln]);
                }
            }
            ln--;
        }
        if (bpm_elem) {
            bcmptm_rm_alpm_elem_free(u, db, DB_LAST_LEVEL(u, db), bpm_elem);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove pivot(s) for default route for an existing VRF.
 *
 * Basically, this is to delete a default (0/0) route pivots.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] key Arg key
 * \param [in] vrf_hdl Pointer to vrf handle
 *
 * \return SHR_E_NONE/SHR_E_XXX.
 */
static int
alpm_vrf_delete(int u, int db, arg_key_t *key, alpm_vrf_hdl_t *vrf_hdl)
{
    alpm_arg_t arg;
    int ln;

    SHR_FUNC_ENTER(u);

    /* Default route: Key = 0, Mask = 0 */
    bcmptm_rm_alpm_arg_init(u, &arg);
    arg.db = db;
    arg.key.t.ipv = key->t.ipv;
    arg.key.t.w_vrf = key->t.w_vrf;
    arg.key.vt = key->vt;
    arg.default_route = 1;

    ln = DB_LAST_LEVEL(u, db) - 1;
    while (ln >= LEVEL1) {
        SHR_IF_ERR_EXIT(
            pivot_delete(u, ln, &arg));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_free(u, db, NEXT_LEVEL(ln), arg.log_bkt[ln]));
        ln--;
    }
    ln = DB_LAST_LEVEL(u, db);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_destroy(u, db, ln, key->t.ipv, key->t.w_vrf));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_free(u, db, ln, vrf_hdl->parallel_default));
    vrf_hdl->in_use = FALSE;
    vrf_hdl->parallel_default = NULL;
    SHR_DQ_REMOVE(&vrf_hdl->dqnode);
exit:
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
pivot_bpm_find_cb(rm_alpm_trie_node_t *node, void *user_data)
{
    pivot_traverse_data_t *trvs_data = (pivot_traverse_data_t *) user_data;
    alpm_pivot_t *pivot;
    alpm_route_t *lpm;
    rm_alpm_trie_node_t *lpm_node;

    SHR_FUNC_ENTER(trvs_data->u);
    pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, pvtnode);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_trie_find_lpm(trvs_data->route_trie,
                                     &pivot->key.t.trie_ip.key[0],
                                     pivot->key.t.len,
                                     &lpm_node,
                                     NULL,
                                     NULL));
    lpm = TRIE_ELEMENT_GET(alpm_pivot_t*, lpm_node, pvtnode);
    ALPM_ASSERT(pivot->bkt);
    pivot->bkt->bpm = lpm;
exit:
    SHR_FUNC_EXIT();
}

static int
alpm_vrf_recover(int u, alpm_arg_t *arg, alpm_vrf_hdl_t *vrf_hdl)
{
    alpm_arg_t root_arg;
    int ln;
    alpm_route_t *bpm_elem = NULL;
    arg_key_t *key;
    int db;
    SHR_FUNC_ENTER(u);

    /* Default route: Key = 0, Mask = 0 */
    bcmptm_rm_alpm_arg_init(u, &root_arg);
    db = arg->db;
    key = &arg->key;
    root_arg.db = db;
    root_arg.key.t.ipv = key->t.ipv;
    root_arg.key.t.w_vrf = key->t.w_vrf;
    /* Key type will be set in bcmptm_rm_alpm_l1_insert */
    root_arg.key.vt = key->vt;
    root_arg.default_route = 1;
    root_arg.valid = 1;

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, db, DB_LAST_LEVEL(u, db),
                                  &root_arg.key.t, &bpm_elem));
    bpm_elem->key.t.len = -1; /* Indicating this is a fake one */

    ln = DB_LAST_LEVEL(u, db);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_trie_create(u, db, ln, key->t.ipv, key->t.w_vrf));

    vrf_hdl->ipv = key->t.ipv;
    vrf_hdl->w_vrf = key->t.w_vrf;
    vrf_hdl->in_use = TRUE;
    
    vrf_hdl->vrf_data_type = VRF_FIXED_DATA_FULL;
    vrf_hdl->parallel_default = bpm_elem;
    SHR_DQ_INSERT_HEAD(&DB_VRF_HDL_LIST(u, db, key->t.ipv), &vrf_hdl->dqnode);

exit:
    if (SHR_FUNC_ERR()) {
        if (bpm_elem) {
            bcmptm_rm_alpm_elem_free(u, db, DB_LAST_LEVEL(u, db), bpm_elem);
        }
    }
    SHR_FUNC_EXIT();
}

static int
alpm_bpm_recover_done(int u, int db)
{
    uint8_t lvl, last_lvl;
    shr_dq_t *elem;
    alpm_vrf_hdl_t *vrf_hdl;
    rm_alpm_trie_t *pivot_trie;
    pivot_traverse_data_t trvs_data;
    alpm_ip_ver_t ipv;

    SHR_FUNC_ENTER(u);
    trvs_data.u = u;
    last_lvl = DB_LAST_LEVEL(u, db);

    for (ipv = IP_VER_4; ipv < IP_VERS; ipv++) {
        SHR_DQ_TRAVERSE(&DB_VRF_HDL_LIST(u, db, ipv), elem)
            vrf_hdl = SHR_DQ_ELEMENT_GET(alpm_vrf_hdl_t *, elem, dqnode);
            trvs_data.route_trie = vrf_hdl->pivot_trie[last_lvl];
            for (lvl = LEVEL1; lvl < last_lvl; lvl++) {
                pivot_trie = vrf_hdl->pivot_trie[lvl];
                ALPM_ASSERT(pivot_trie);
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_alpm_trie_traverse(pivot_trie,
                                                 pivot_bpm_find_cb,
                                                 &trvs_data,
                                                 TRAVERSE_BFS));
            }
        SHR_DQ_TRAVERSE_END(&DB_VRF_HDL_LIST(u, db, key->t.ipv), elem);
    }
exit:
    SHR_FUNC_EXIT();
}

static void
ln_info_cleanup(int u)
{
    int i, j;
    int db_count;
    lvl_hdl_t *lvl_hdl = NULL;

    db_count = bcmptm_rm_alpm_database_count(u);
    for (i = 0; i < db_count; i++) {
        alpm_db_t *database_ptr;
        database_ptr = bcmptm_rm_alpm_database_get(u, i);

        /* Start from Level2 */
        for (j = LEVEL2; j < database_ptr->max_levels; j++) {
            lvl_hdl = LVL_HDL(u, i, j);
            if (lvl_hdl) {
                SHR_FREE(lvl_hdl);
            }
        }
    }
    SHR_FREE(LN_INFO(u));
    bcmptm_rm_alpm_bkt_hdl_cleanup(u);
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
ln_info_init(int u, bool recover)
{
    int sz;
    alpm_dev_info_t *dev_info;
    int db_idx, level;
    int db_count;
    int db_max_levels[DBS] = {0};
    alpm_info_t *alpm_info;

    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u);
    SHR_NULL_CHECK(alpm_info, SHR_E_INIT);

    dev_info = bcmptm_rm_alpm_device_info_get(u);
    SHR_NULL_CHECK(dev_info, SHR_E_INIT);

    sz = sizeof(alpm_ln_info_t);
    ALPM_ALLOC(LN_INFO(u), sz, "Level-N info");
    sal_memset(LN_INFO(u), 0, sz);

    LN_SID(u, LEVEL1) = INVALID_SID;
    LN_SID(u, LEVEL2) = dev_info->l2_sid;
    LN_HIT_SID(u, LEVEL2) = dev_info->l2_hit_sid;
    LN_SID(u, LEVEL3) = dev_info->l3_sid;
    LN_HIT_SID(u, LEVEL3) = dev_info->l3_hit_sid;
    LN_INFO_FD(u, alpm_ver) = dev_info->alpm_ver;

    /* Level info initialization */
    db_count = bcmptm_rm_alpm_database_count(u);
    for (db_idx = 0; db_idx < db_count; db_idx++) {
        alpm_db_t *database_ptr = bcmptm_rm_alpm_database_get(u, db_idx);
        db_max_levels[db_idx] = database_ptr->max_levels;
    }

    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_bkt_hdl_init(u, recover));

    for (db_idx = 0; db_idx < db_count; db_idx++) {
        /* Start from Level2 */
        for (level = LEVEL2; level < db_max_levels[db_idx]; level++) {
            lvl_hdl_t *lvl_hdl = NULL;
            bkt_hdl_t *bkt_hdl;

            ALPM_ALLOC(lvl_hdl, sizeof(lvl_hdl_t), "alpm_lvl_hdl");
            sal_memset(lvl_hdl, 0, sizeof(lvl_hdl_t));
            LVL_HDL(u, db_idx, level) = lvl_hdl;
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bkt_hdl_get(u,
                                           db_idx,
                                           level,
                                           &bkt_hdl));
            lvl_hdl->bkt_hdl = bkt_hdl;
            lvl_hdl->lvl_idx = level;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (LN_INFO(u)) {
            ln_info_cleanup(u);
        }
    }
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public Functions
 */

/*!
 * \brief ALPM find operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_find(int u, alpm_arg_t *arg)
{
    int ln = DB_LAST_LEVEL(u, arg->db);
    int cur_ln;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_find(u, arg->db, ln, &arg->key.t,
                                &arg->pivot[ln]));
    /* Get last level index */
    arg->index[ln] = arg->pivot[ln]->index;

    /* Get previous level (until Level-1) pivot & index */
    cur_ln = ln;
    while(cur_ln >= LEVEL2) {
        arg->pivot[PREV_LEVEL(cur_ln)] =
            bcmptm_rm_alpm_bucket_pivot_get(u, arg->db, cur_ln, arg->index[cur_ln]);
        arg->index[PREV_LEVEL(cur_ln)] = arg->pivot[PREV_LEVEL(cur_ln)]->index;
        cur_ln--;
    }

    /* Map trie format ip & mask to api format */
    ALPM_ASSERT(arg->index[ln] != INVALID_INDEX);
    SHR_IF_ERR_EXIT
        (bcmptm_rm_alpm_api_key_create(u, arg->key.t.ipv,
                                       &arg->key.t.trie_ip,
                                       arg->key.t.len,
                                       arg->key.ip_addr));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_len_to_mask(u, arg->key.t.ipv, arg->key.t.len,
                                   &arg->key.ip_mask[0]));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM match operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_match(int u, alpm_arg_t *arg)
{
    int ln = DB_LAST_LEVEL(u, arg->db);
    int cur_ln;
    int ipv;
    alpm_info_t *info;

    SHR_FUNC_ENTER(u);

    /* Get last level pivot */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_alpm_pvt_lookup(u, arg->db, ln, &arg->key.t,
                                  &arg->pivot[ln]));

    /* Get last level index */
    arg->index[ln] = arg->pivot[ln]->index;

    /* Get previous level (until Level-1) pivot & index */
    cur_ln = PREV_LEVEL(ln);
    while(cur_ln >= LEVEL1) {
        arg->pivot[cur_ln] =
            bcmptm_rm_alpm_bucket_pivot_get(u, arg->db,
                                            NEXT_LEVEL(cur_ln),
                                            arg->index[NEXT_LEVEL(cur_ln)]);

        arg->index[cur_ln] = arg->pivot[cur_ln]->index;
        cur_ln--;
    }

    /* Map trie format ip & mask to api format */
    ALPM_ASSERT(arg->index[ln] != INVALID_INDEX);
    ipv = arg->key.t.ipv;

    info = bcmptm_rm_alpm_info_get(u);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (info->cmd_ctrl.req_flags & BCMPTM_REQ_FLAGS_DONT_USE_CACHE) {
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_entry_read(u, ln, arg));
    } else {
        if (arg->set_key) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_alpm_api_key_create(u, ipv,
                                               &arg->key.t.trie_ip,
                                               arg->key.t.len,
                                               arg->key.ip_addr));
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_len_to_mask(u, ipv, arg->key.t.len,
                                           &arg->key.ip_mask[0]));
        }
        sal_memcpy(&arg->ad, arg->pivot[ln]->ad, sizeof(arg->ad));
    }

#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        alpm_pivot_t *pre_pivot = NULL;
        cur_ln = PREV_LEVEL(ln);
        while (cur_ln >= LEVEL1) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_pvt_find(u, arg->db, cur_ln, &arg->key.t,
                                        &pre_pivot));
            ALPM_ASSERT(arg->pivot[cur_ln] == pre_pivot);
            cur_ln--;
        }
    }
#endif

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief ALPM insert operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_insert(int u, alpm_arg_t *arg)
{
    alpm_vrf_hdl_t *vrf_hdl = NULL;
    int ln;
    int cnt;
    uint8_t vrf_add_flag = 0;
    bkt_split_aux_t split_aux = {0};
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        insert_validate(u, arg));
    SHR_IF_ERR_EXIT(
        data_type_check(u, arg));
    format_get(u, arg);
    /* Assume ltm will always do lookup ahead for insert/update */
    if ((arg->pivot[LEVEL2] && arg->pivot[LEVEL2]->index != INVALID_INDEX) ||
        (arg->pivot[LEVEL3] && arg->pivot[LEVEL3]->index != INVALID_INDEX)) {
        SHR_RETURN(alpm_update(u, arg));
    }

    /* From now on, new add operation */
    vrf_hdl = &VRF_HDL(u, arg->db, arg->key.t.ipv, arg->key.t.w_vrf);
    if (vrf_hdl->in_use == FALSE) {
        SHR_IF_ERR_EXIT(
            alpm_vrf_add(u, arg, vrf_hdl));
        vrf_add_flag = 1;
    }

    ln = DB_LAST_LEVEL(u, arg->db);
    split_aux.set = 1;
    SHR_IF_ERR_EXIT(
        alpm_insert(u, ln, arg, &split_aux));

    bcmptm_rm_alpm_vrf_route_inc(u, arg->db, arg->key.t.w_vrf,
                                 arg->key.t.ipv, &cnt);

    /* alpm_sanity(u, arg);*/

exit:
    if (SHR_FUNC_ERR()) {
        if (vrf_add_flag) {
            alpm_vrf_delete(u, arg->db, &arg->key, vrf_hdl);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM delete operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
int
bcmptm_rm_alpm_ln_delete(int u, alpm_arg_t *arg)
{
    int ln, cnt;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(delete_validate(u, arg));

    ln = DB_LAST_LEVEL(u, arg->db);
    /*
       SHR_IF_ERR_EXIT(
        LEVEL_ELEM_DELETE(alpm_pivot_t *, u, ln, arg, &route));*/
    SHR_IF_ERR_EXIT(
        alpm_delete(u, ln, arg));
    bcmptm_rm_alpm_vrf_route_dec(u, arg->db, arg->key.t.w_vrf,
                                 arg->key.t.ipv, &cnt);

    if (cnt == 0) {
        alpm_vrf_hdl_t *vrf_hdl;
        vrf_hdl = &VRF_HDL(u, arg->db, arg->key.t.ipv, arg->key.t.w_vrf);
        ALPM_ASSERT(vrf_hdl->in_use == TRUE);
        SHR_IF_ERR_EXIT(
            alpm_vrf_delete(u, arg->db, &arg->key, vrf_hdl));
    }
    /*    alpm_sanity(u, arg);*/
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_ln_cleanup(int u)
{
    if (!LN_INFO(u)) {
        return;
    }
    ln_info_cleanup(u);
    bcmptm_rm_alpm_bucket_cleanup(u);
}

int
bcmptm_rm_alpm_ln_init(int u, bool recover)
{
    SHR_FUNC_ENTER(u);

    if (bcmptm_rm_alpm_uft_banks(u, LEVEL2) == 0) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bucket_init(u));
    SHR_IF_ERR_EXIT(
        ln_info_init(u, recover));
exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_alpm_ln_info_get(int u, alpm_ln_info_t *linfo)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(LN_INFO(u), SHR_E_NOT_FOUND);
    sal_memcpy(linfo, LN_INFO(u), sizeof(alpm_ln_info_t));
exit:
    SHR_FUNC_EXIT();
}

bcmdrd_sid_t
bcmptm_rm_alpm_ln_sid_get(int u, int ln)
{
    if (LN_INFO(u)) {
        return LN_SID(u, ln);
    } else {
        return INVALIDm;
    }
}

int
bcmptm_rm_alpm_lvl_hdl_get(int u, int db, int ln, lvl_hdl_t **lvl_hdl)
{
    return level_hdl_get(u, db, ln, lvl_hdl);
}


int
bcmptm_rm_alpm_split_pivot_add(int u, int db, int ln, alpm_arg_t *split_arg,
                               alpm_pivot_t **split_pivot,
                               bkt_split_aux_t *split_aux)
{
    return split_pivot_add(u, db, ln, split_arg, split_pivot, split_aux);
}

int
bcmptm_rm_alpm_split_pivot_delete(int u, int db, int ln, alpm_arg_t *split_arg,
                                  bkt_split_aux_t *split_aux)
{
    return split_pivot_delete(u, db, ln, split_arg, split_aux);
}

int
bcmptm_rm_alpm_pivot_update(int u, int db, int ln, alpm_pivot_t *pivot)
{
    return pivot_update(u, db, ln, pivot);
}


int
bcmptm_rm_alpm_read_ln_raw(int u, int db, int ln, int hw_idx, uint32_t *de)
{
    return bcmptm_rm_alpm_read(u, LN_SID(u, ln), hw_idx, de);
}


int
bcmptm_rm_alpm_read_ln_hit(int u, int db, int ln, int hw_idx, uint32_t *e)
{
    return bcmptm_rm_alpm_ireq_read(u, LN_HIT_SID(u, ln), hw_idx, e);
}

int
bcmptm_rm_alpm_write_ln_hit(int u, int db, int ln, int hw_idx, uint32_t *e)
{
    return bcmptm_rm_alpm_ireq_write(u, LN_HIT_SID(u, ln), hw_idx, e);
}


int
bcmptm_rm_alpm_write_ln_raw(int u, int db, int ln, int hw_idx, uint32_t *de)
{
#ifdef ALPM_DEBUG
    static int ln_write;
    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        cli_out(" ln_write %d hw_idx %d \n", ln_write++, hw_idx);
        bcmptm_rm_alpm_pt_dump(u, LN_SID(u, ln), de);
    }
#endif
    return bcmptm_rm_alpm_write(u, LN_SID(u, ln), hw_idx, de);
}

int
bcmptm_rm_alpm_clear_ln_raw(int u, int db, int ln, int hw_idx)
{
    static LN_ENTRY_t de = {0};
    return bcmptm_rm_alpm_write(u, LN_SID(u, ln), hw_idx, de);
}

int
bcmptm_rm_alpm_bucket_clean(int u, int db, int ln, alpm_pivot_t *pivot)
{
    /*
     * If bucket is empty, delete pivot with tail recursion.
     * Handle default route (len = 0) case in vrf delete.
     */
    int prev_ln = PREV_LEVEL(ln);
    int bucket_empty = 0;
    alpm_pivot_t *next_level_pivot;
    alpm_arg_t arg2;
    rm_alpm_trie_t *bucket_trie;
    SHR_FUNC_ENTER(u);

    bucket_trie = pivot->bkt->trie;
    ALPM_ASSERT(bucket_trie->trie == NULL || bucket_trie->trie->count == 1);
    if (ln == DB_LAST_LEVEL(u, db)) {
        if (ln == LEVEL3) { /* Three levels mode */
            alpm_pivot_t *pre_pivot;
            pre_pivot = bcmptm_rm_alpm_bucket_pivot_get(u, db, prev_ln,
                                                        pivot->index);
            if (pivot->key.t.len != 0 && bucket_trie->trie == NULL) {
                if (pivot->key.t.len != pre_pivot->key.t.len) {
                    bucket_empty = 1;
                } else if (pre_pivot->bkt->trie->trie->count == 1) {
                    bucket_empty = 1;
                }
            }
        } else { /* Two levels mode, no pre-pivot */
            if (pivot->key.t.len != 0 && bucket_trie->trie == NULL) {
                bucket_empty = 1;
            }
        }
    } else {
        /*
         * Bucket entries all deleted or just one left, that one must be the
         * same as pivot
         */
        if (pivot->key.t.len != 0 && bucket_trie->trie &&
            bucket_trie->trie->count == 1) {
            rm_alpm_trie_node_t *node = bucket_trie->trie;
            ALPM_ASSERT(node->type == PAYLOAD);
            next_level_pivot = TRIE_ELEMENT_GET(alpm_pivot_t *, node, bktnode);
            if (next_level_pivot->key.t.len != 0 &&
                !next_level_pivot->bkt->trie->trie) {
                bucket_empty = 1;
                /* Only when the first level pivot deleted, the second level can be freed */
                /* Reset to use common logic to free previous bucket. */
                pivot = next_level_pivot;
                prev_ln = ln;
                ln = NEXT_LEVEL(ln);
            }
        } else if (pivot->key.t.len != 0 && bucket_trie->trie == NULL) {
            bucket_empty = 1;
        }
    }

    if (bucket_empty) {
        bcmptm_rm_alpm_arg_init(u, &arg2);
        arg_from_pivot(u, db, prev_ln, pivot, true, &arg2);
        SHR_IF_ERR_EXIT(
            pivot_delete(u, prev_ln, &arg2));
        SHR_IF_ERR_EXIT(
            bcmptm_rm_alpm_bucket_free(u, db, ln, arg2.log_bkt[prev_ln]));
        ALPM_ASSERT(bucket_trie->trie == 0);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_pivot_update_with_arg(int u, int db, int ln,
                                     alpm_pivot_t *pivot, alpm_arg_t *arg)
{
    return pivot_update_with_arg(u, db, ln, pivot, arg);
}

int
bcmptm_rm_alpm_pivot_delete(int u, int ln, alpm_arg_t *arg)
{
    return pivot_delete(u, ln, arg);
}

int
bcmptm_rm_alpm_arg_from_pivot(int u, int db, int ln, alpm_pivot_t *pivot,
                              bool key_only, alpm_arg_t *arg)
{
    return arg_from_pivot(u, db, ln, pivot, key_only, arg);
}

int
bcmptm_rm_alpm_ln_recover(int u, int ln, alpm_arg_t *arg)
{
    alpm_vrf_hdl_t *vrf_hdl;

    SHR_FUNC_ENTER(u);
    format_get(u, arg);
    vrf_hdl = &VRF_HDL(u, arg->db, arg->key.t.ipv, arg->key.t.w_vrf);
    if (vrf_hdl->in_use == FALSE) {
        SHR_IF_ERR_EXIT(
            alpm_vrf_recover(u, arg, vrf_hdl));
    }
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_bucket_recover(u, ln, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_ln_pivot_recover(int u, int ln, alpm_arg_t *arg)
{
    alpm_elem_t *elem;
    alpm_pivot_t *pivot;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_elem_alloc(u, arg->db, ln, &arg->key.t, &elem));
    pivot = arg->pivot[PREV_LEVEL(ln)];
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_bkt_insert(u, ln, &arg->key, pivot->bkt, elem));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_alpm_pvt_insert(u, arg->db, ln, &arg->key, elem));
    arg->pivot[ln] = elem;
    elem->index = arg->index[ln];

    if (ln == DB_LAST_LEVEL(u, arg->db)) {
        int cnt;
        sal_memcpy(elem->ad, &arg->ad, sizeof(assoc_data_t));
        bcmptm_rm_alpm_vrf_route_inc(u, arg->db, arg->key.t.w_vrf,
                                     arg->key.t.ipv, &cnt);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_ln_recover_done(int u, bool full)
{
    int db, ln;

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(full ? SHR_E_NONE: SHR_E_UNAVAIL);
    for (db = DB0; db < bcmptm_rm_alpm_database_count(u); db++) {
        for (ln = LEVEL2; ln <= DB_LAST_LEVEL(u, db); ln++) {
            SHR_IF_ERR_EXIT(
                bcmptm_rm_alpm_bucket_recover_done(u, db, ln));
        }
        SHR_IF_ERR_EXIT(alpm_bpm_recover_done(u, db));
    }
exit:
    SHR_FUNC_EXIT();
}
