/*! \file rm_alpm_common.h
 *
 * Databases for RM ALPM all levels
 *
 * This file contains main routines which are internal to
 * ALPM resource manager
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

#ifndef RM_ALPM_COMMON_H
#define RM_ALPM_COMMON_H

/*******************************************************************************
  Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_dq.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>

#include "rm_alpm_trie.h"

/*******************************************************************************
  Defines
 */
/*!< Maximum TCAM/ALPM memory entry size in words (640 bits) */
#define ALPM_MEM_ENT_MAX     20   /* BCMPTM_MAX_PT_FIELD_WORDS (40) */

/*!< Number of formats for ALPM2_DATA */
#define ALPM2_DATA_FORMATS    8
/*!< Number of formats for ALPM1_DATA */
#define ALPM1_DATA_FORMATS    5

/*! ALPM levels */
#define LEVEL1 0
#define LEVEL2 1
#define LEVEL3 2
#define LEVELS 3
#define MAX_LEVELS       (LEVELS)
#define MAX_PIVOT_LEVELS (LEVELS - 1)

/*! ALPM modes */
#define ALPM_MODE_COMBINED 0
#define ALPM_MODE_PARALLEL 1

#define DBH  0 /* Global High */
#define DB0  1
#define DB1  2
#define DBS  3

#define MAX_DBS_2 2
#define MAX_DBS_3 3

#define MAX_TCAM_BLOCKS 4

#define INVALID_INDEX           (-1)
#define INVALID_GROUP           (-1)
#define INVALID_LOG_BKT         (-1)
#define INVALID_SID             0

/*
 * Logical packing mode values.
 * Ver0: PKM0-64  PKM1-128
 * Ver1: PKM0-32  PKM1-64  PKM2-128
 */
#define PKM0    0
#define PKM1    1
#define PKM2    2

#define ALPM_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)

#define KEY_IS_V4(_p_) ((_p_) == KEY_IPV4)
#define KEY_IS_V6(_p_) ((_p_) == KEY_IPV6_32 || \
                        (_p_) == KEY_IPV6_64 || \
                        (_p_) == KEY_IPV6_128)
#define KT2IPV   KEY_IS_V6
/*!< Max entries in a raw bucket */

#define PREV_LEVEL(n)  ((n) - 1)
#define NEXT_LEVEL(n)  ((n) + 1)

#define ALPM_MIN(a, b)              ((a) < (b) ? (a) : (b))
#define ALPM_MAX(a, b)              ((a) > (b) ? (a) : (b))

#define ALPM_BLOCKS  4

/*
 * Combined mode wvrf layout.
 * DBH: OVERRIDE (0)
 * DB0: PRIVATE + GLOBAL (max_vrf + 1)
 */
#define COMBINED_WVRF_OVERRIDE          (0)
#define COMBINED_WVRF_GLOBAL(max_vrf)   (max_vrf + 1)
#define COMBINED_WVRF_COUNT(max_vrf)    (max_vrf + 2)

/*
 * Parallel mode wvrf layout.
 * DBH: OVERRIDE (0)
 * DB0: PRIVATE
 * DB1: GLOBAL (0)
 */
#define PARALLEL_WVRF_OVERRIDE          COMBINED_WVRF_OVERRIDE
#define PARALLEL_WVRF_GLOBAL(max_vrf)   (0)
#define PARALLEL_WVRF_COUNT(max_vrf)    (max_vrf + 1)

#define WRITE_NORMAL        0
#define WRITE_LATER         1
#define WRITE_NEW           2

/*! ALPM database */
#define ALPM_DB_CNT(u)              (bcmptm_rm_alpm_database_cnt[u])
#define ALPM_DBS(u)                 (bcmptm_rm_alpm_database[u])
#define ALPM_DB(u, d)               (bcmptm_rm_alpm_database[u][d])
#define VRF_HDLS(u, d)              (bcmptm_rm_alpm_database[u][d].vrf_hdl)
#define VRF_HDL(u, d, v, w)         (bcmptm_rm_alpm_database[u][d].vrf_hdl[v][w])

#define DB_LMM_ELEMS(u, d)          (ALPM_DB(u, d).elems)
#define DB_LMM_BKTS(u, d)           (ALPM_DB(u, d).buckets)
#define DB_LMM_ADATAS(u, d)         (ALPM_DB(u, d).adatas)
#define DB_LEVELS(u, d)             (ALPM_DB(u, d).max_levels)
#define DB_LAST_LEVEL(u, d)         (ALPM_DB(u, d).max_levels - 1)
#define DB_VRF_HDL_CNT(u, d)        (ALPM_DB(u, d).vrf_hdl_cnt)
#define DB_VRF_HDL_LIST(u, d, v)    (ALPM_DB(u, d).vrf_hdl_list[v])
#define DB_VRF_HDLS(u, d)           (ALPM_DB(u, d).vrf_hdl)

#define VRF_HDL_DQNODE(u, d, v, w)   (VRF_HDL(u, d, v, w).dqnode)
#define VRF_HDL_PTRIES(u, d, v, w)   (VRF_HDL(u, d, v, w).pivot_trie)
#define VRF_HDL_PTRIE(u, d, v, w, l) (VRF_HDL(u, d, v, w).pivot_trie[l])
#define VRF_HDL_RCNT(u, d, v, w)     (VRF_HDL(u, d, v, w).route_cnt)
#define VRF_HDL_IN_USE(u, d, v, w)   (VRF_HDL(u, d, v, w).in_use)
#define VRF_HDL_IPV(u, d, v, w)      (VRF_HDL(u, d, v, w).v)
#define VRF_HDL_WVRF(u, d, v, w)     (VRF_HDL(u, d, v, w).w)
#define VRF_HDL_DT(u, d, v, w)       (VRF_HDL(u, d, v, w).vrf_data_type)
#define VRF_HDL_DFT(u, d, v, w)      (VRF_HDL(u, d, v, w).parallel_default)

/* #define ALPM_DEBUG_FILE */
/* #define ALPM_DEBUG */
#ifdef ALPM_DEBUG
#define ALPM_ASSERT(_expr)          assert(_expr)
#define ALPM_ASSERT_ERRMSG(_expr)   if (!(_expr)) \
       {LOG_WARN(BSL_LOG_MODULE, (BSL_META("ALPM Check Fail\n")));}
#define LOG_V(C)    LOG_VERBOSE(BSL_LOG_MODULE, C)
#define LOG_D(C)    LOG_DEBUG(BSL_LOG_MODULE, C)
#else
#define ALPM_ASSERT(_expr)
#define ALPM_ASSERT_ERRMSG(_expr)
#define LOG_V(C)
#define LOG_D(C)
#endif

#define SPLIT_EPARTIAL_REASON_UNKNOWN      0
#define SPLIT_EPARTIAL_REASON_RESPLIT      1
#define SPLIT_EPARTIAL_REASON_REINSERT     2
#define SPLIT_EFULL_REASON_UNKNOWN         0
#define SPLIT_EFULL_REASON_PIVOT_FULL      1

#define L1V_NARROW_0        0
#define L1V_NARROW_1        1
#define L1V_NARROW_FULL     2
#define L1V_WIDE            3

#define DEFIP_MAX_PT_FIELD_WORDS  4  /* Replace data 69 bits */

/*******************************************************************************
  Typedefs
 */
/*!
 * \brief Raw bucket index
 */
typedef int rbkt_idx_t;

/*!
 * \brief Raw bucket group
 */
typedef rbkt_idx_t rbkt_group_t;

/*!
 * \brief Logical bucket index
 */
typedef int log_bkt_t;

/*!
 * \brief ALPM level-n index
 */
typedef int ln_index_t;

/*!
 * \brief ALPM KEY types
 */
typedef enum {
     KEY_TYPE_INVALID   = -1,   /*!< INVALID KEY type, force type int */
     KEY_IPV4           = 0,    /*!< IPv4 key type */
     KEY_IPV6_32        = 1,    /*!< Ipv6 len [0, 32] */
     KEY_IPV6_64        = 2,    /*!< Ipv6 len [33, 64] */
     KEY_IPV6_128       = 3,    /*!< Ipv6 len [65, 128] */
     KEY_TYPES                  /*!< Number of KEY types */
} alpm_key_type_t;

/* Per route */
typedef enum alpm_data_type_e {
     DATA_TYPE_INVALID   = -1,   /*!< INVALID KEY type, force int */
     DATA_REDUCED        = 0,    /*!< Ipv6 len [0, 32] */
     DATA_FULL           = 1,    /*!< IPv4 key type */
     DATA_TYPES                  /*!< Number of KEY types */
} alpm_data_type_t;

/*!
 * \brief ALPM VRF types
 */
typedef enum {
    VRF_TYPE_INVALID    = -1,   /*!< INVALID VRF type, force type int */
    VRF_PRIVATE         = 0,    /*!< PRIVATE VRF */
    VRF_GLOBAL          = 1,    /*!< GLOBAL VRF */
    VRF_OVERRIDE        = 2,    /*!< OVERRIDE VRF */
    VRF_TYPES                   /*!< Number of VRF types */
} alpm_vrf_type_t;

/*!
 * \brief ALPM trie format prefix
 */
typedef union trie_ip_u {
    uint32_t v4_key[2];     /*!< IPv4 Key */
    uint32_t v6_key[5];     /*!< IPv6 Key */
    uint32_t key[5];        /*!< Common Key */
} trie_ip_t;

/*!
 * \brief ALPM VRF
 */
typedef uint32_t alpm_vrf_t;

/*!
 * \brief ALPM IP address
 */
typedef uint32_t alpm_ip_t[4];

/*!
 * \brief ALPM IP address
 */
typedef uint32_t alpm_format_ip_t[8];

/*!
 * \brief ALPM Level-1 pt entry
 */
typedef uint32_t L1_ENTRY_t[ALPM_MEM_ENT_MAX];

/*!
 * \brief ALPM Level-2 pt entry
 */
typedef uint32_t L2_ENTRY_t[ALPM_MEM_ENT_MAX];

/*!
 * \brief ALPM Level-3 pt entry
 */
typedef uint32_t L3_ENTRY_t[ALPM_MEM_ENT_MAX];


/*!
 * \brief ALPM Level-N pt entry
 */
typedef uint32_t LN_ENTRY_t[ALPM_MEM_ENT_MAX];


/*!
 * \brief ALPM Level-N pt entry
 */
typedef uint32_t LN_ENTRY_16t[16];

/*!
 * \brief ALPM Level-N pt entry
 */
typedef uint32_t LN_ENTRY_8t[8];

/*!
 * \brief ALPM Format buffer
 */
typedef uint32_t FORMAT_ENTRYt[ALPM_MEM_ENT_MAX];
typedef uint32_t FORMAT_ENTRY_4t[4];
typedef uint32_t FORMAT_ENTRY_2t[2];

/*!
 * \brief Bucket elem.
 */
typedef struct alpm_elem_s alpm_bkt_elem_t;

/*!
 * \brief Pivot
 */
typedef struct alpm_elem_s alpm_pivot_t;

/*!
 * \brief Route.
 */
typedef struct alpm_elem_s alpm_route_t;
typedef struct bkt_split_aux_s {
    uint32_t aux_pivot_len;
    trie_ip_t aux_pivot_key;
    alpm_pivot_t *aux_parent_pivot[LEVELS];
    rm_alpm_trie_node_t *aux_split_root;
    uint8_t set;
    uint8_t epartial_reason;
    uint8_t efull_reason;
} bkt_split_aux_t;
/*!
 * \brief ALPM IP versions
 */
typedef enum {
    IP_VER_4 = 0,    /*!< IPv4 */
    IP_VER_6 = 1,    /*!< IPv6 */
    IP_VERS  = 2     /*!< Number of IP versions */
} alpm_ip_ver_t;

/*!
 * \brief ALPM key tuple
 */
typedef struct key_tuple_s {
    int           w_vrf;       /*!< Weighted VRF */
    trie_ip_t     trie_ip;     /*!< Trie format key */
    alpm_ip_ver_t ipv;         /*!< IP version */
    int           len;         /*!< Prefix len */
} key_tuple_t;

/*!
 * \brief ALPM assoc data
 */
typedef struct assoc_data_s {
    uint32_t src_discard:1;      /*!< Source discard */
    uint32_t flex_stat_valid:1;  /*!< Flex counter valid */
    uint32_t default_miss:1;     /*!< For Ver0 */
    uint32_t global_route:1;     /*!< For Ver0 */
    uint32_t fmt_ad3:20;         /*!< For Ver0 */
    uint32_t rsv0:8;
    uint16_t fmt_ad1;            /*!< For Ver0 */
    uint16_t fmt_ad2;            /*!< For Ver0 */
    uint32_t fmt_ad[DEFIP_MAX_PT_FIELD_WORDS];

    /*!< Pointer to the next element in the list (or NULL). */
    struct assoc_data_s *next;
} assoc_data_t;

/*!
 * \brief ALPM data for level-2 pivot
 */
typedef struct alpm2_data_s {
    uint32_t phy_bkt:16;
    uint32_t kshift:8;
    uint32_t rofs:3;
    uint32_t rsv0:4;
    uint32_t default_miss:1;
    uint8_t fmt[ALPM2_DATA_FORMATS];
} alpm2_data_t;

/*!
 * \brief ALPM data for level-1 pivot
 */
typedef struct alpm1_data_s {
    uint32_t phy_bkt:16;
    uint32_t kshift:8;
    uint32_t rofs:3;
    uint32_t sub_bkt:2;
    uint32_t rsv0:1;
    uint32_t default_miss:1;
    uint32_t direct_route:1;

    uint32_t hit_idx:24;
    uint32_t rsv1:8;
    uint8_t fmt[ALPM1_DATA_FORMATS];
} alpm1_data_t;


/*!< Key part of elem */
typedef struct elem_key_s {
    /*!< Key tuple */
    key_tuple_t t;

     /*!< VRF type, for Level-1 only */
    alpm_vrf_type_t vt;
} elem_key_t;

/*!< Bucket part of elem, only for pivot levels. i.e., non-last level */
typedef struct elem_bkt_s {
    /*! To link all bkt_elem in same bucket */
    shr_dq_t list;

    /*!< Bucket trie */
    rm_alpm_trie_t *trie;

    /*!< Bucket default, i.e., BPM  */
    alpm_route_t *bpm;

    /*!< Raw bucket group */
    /*
     * For Ver0: Group | Sub Bkt
     * For Ver1: Group
     */
    log_bkt_t log_bkt;

    /*!< Pointer to the next element in the list (or NULL). */
    struct elem_bkt_s *next;
} elem_bkt_t;

/*!
 * \brief ALPM pivot node structure
 */
typedef struct alpm_elem_s {
    /*!< trie node */
    rm_alpm_trie_node_t pvtnode;

    /* Bucket trie node in same bucket */
    rm_alpm_trie_node_t bktnode;

    /* Dq node in same bucket, for faster traverse */
    shr_dq_t dqnode;

    /*!< Element key */
    elem_key_t key;

    /*!< Entry index */
    ln_index_t index;

    /*!< Bucket info for pivot */
    elem_bkt_t *bkt;

    /*!< Assoc data */
    assoc_data_t *ad;

    /*!< Pointer to the next element in the list (or NULL). */
    struct alpm_elem_s *next;
} alpm_elem_t;


/*!< VRF data types, predefined
 *
 * Hardware restirtion:
 *      All entries in same bucket must use same data type. Thus data_type
 *          of one entry changes must lead to all entries in that whole
 *          bucket changes.
 *      BPM of the route must also be of same data type as that route.
 *          The BPM is in a previous level, thus this could affect the
 *          bucket in prev level holding that BPM.
 *      Pivot of the route must also be of same data type as that route.
 *          The pivot is in a previous level, thus this could affect the
 *          bucket in prev level holding that pivot.
 *      From above, we know that one entry changes will change its siblings
 *          and its pivot and pivot siblings (let's just call it), and its
 *          BPM and BPM siblings.
 *          Each of its siblings will lead to same changes to previous
 *          level, just like how the route lead to changes to its Pivot and
 *          BPM. PLEASE NOTE, though the *real* data type of the siblings
 *          do not change, but its format has changed, I feel the least
 *          error-prone approach is to honour its format, rather than
 *          the *real* data type -- e.g, using full format but without
 *          setting full-format-only fields like RPE.
 *          Thus we see how this can be a chain action.
 *      From above, we also know that, the changes in this level has a
 *          minimal impact - just entries in same bucket. The changes in
 *          previous levels can be multi-buckets, in fact they are the
 *          buckets where all it and its siblings' parents (BPM & Pivot)
 *          belong.
 *          If we have three levels, then the previous-previous level, i.e.,
 *          the first level will have even more impacts. Luckily, Level-1
 *          always use full data format.
 *      We don't want the multi-levels chain action, as this can be
 *          very complex, and very error-prone. In my view, the most
 *          practical way to support this, is we fixed the level-2 as full
 *          data, same as level-1. We only change the level-3 (the route).
 *
 * Proposal:
 *      1) Per VRF data type: the data type are fixed within one VRF.
 *              No support for dynamical data_type change.
 *              Users need to delete all the routes in that VRF, and add
 *              back with different format.
 *      2) Per route data type (Level-3 only): the data type are fixed to
 *              full in Level-2, and changable in Level-3.
 *
 *  Approach 1) has advantage in scenario a) for some VRFs reduced format
 *             are enough, they will have the biggest capacity. b) for some
 *             VRFs full format are desired, they will have full data
 *             support. Another advantage is this does not bring complex
 *             chain (multi-levels) actions.
 *  Approach 2) has the advantage in scenario, where reduced and full
 *              data are used interchangeably within a VRF.
 *
 *  Given above discussion, the recommomended proposal is
 *      3) Per VRF fixed or dynamic. If fixed, use 1), else use 2).
 *         IMHO, this approach achieved the best trade-off between
 *         flexlibity, capacity and complexity.
 */
typedef enum vrf_data_type_s {
    /*!< VRF data type fixed as FULL */
    VRF_FIXED_DATA_FULL,

    /*!< VRF data type fixed as REDUCED */
    VRF_FIXED_DATA_REDUCED,

    /*!< VRF data type dynamically changed */
    VRF_DYNAMIC_DATA
} vrf_data_type_t;


/*!
 * \brief Global ALPM vrf handles
 *
 * Most ALPM resources are VRF based, this structure describes the VRF
 * related info.
 */
typedef struct alpm_vrf_hdl_s {
    /*!< DQ node to link all in-use VRF hdls */
    shr_dq_t dqnode;

    /*!< Pivot trie for all levels */
    rm_alpm_trie_t *pivot_trie[MAX_LEVELS];

    /*!< Route count */
    uint32_t route_cnt;

    /*!< VRF hdls in-use */
    bool in_use;

    /*!< IP version */
    alpm_ip_ver_t ipv;

    /*!< Weighted VRF */
    int w_vrf;

    /*!< Default route for parallel mode where no real default route exists */
    alpm_route_t *parallel_default;

    /*!< VRF data type */
    vrf_data_type_t vrf_data_type;

#if 0
        /* Debug counter */
        uint32_t cnt_add;
        uint32_t cnt_del;
        uint32_t cnt_split;
        uint32_t cnt_pivot;
#endif

} alpm_vrf_hdl_t;


/*!
 * \brief ALPM database
 *
 * ALPM provides the ability to allow different database co-exist.
 * Each database are compeletely seperated and completely independant
 * to have different levels.
 */
typedef struct alpm_db_s {
    /*!< Elems LMM hdls. */
    shr_lmm_hdl_t elems;

    /*!< Buckets LMM hdls. */
    shr_lmm_hdl_t buckets;

    /*!< Assoc data LMM hdls. */
    shr_lmm_hdl_t adatas;

    /*!< Max level per database. */
    int max_levels;

    /*!< VRF handles per database. */
    alpm_vrf_hdl_t *vrf_hdl[IP_VERS];

    /*!< VRF handle count per database. */
    int vrf_hdl_cnt;

    /*!< List for all in-use VRF hdls, for faster traverse. */
    shr_dq_t vrf_hdl_list[IP_VERS];
} alpm_db_t;

/*!
 * \brief ALPM arg key
 */
typedef struct arg_key_s {
    alpm_vrf_type_t     vt;          /*!< VRF type */
    alpm_ip_t           ip_addr;     /*!< IP address */
    alpm_ip_t           ip_mask;     /*!< IP address */
    alpm_key_type_t     kt;          /*!< Key type, for Level-1 only */
    key_tuple_t         t;           /*!< Key tuple */
    int                 pfx;         /*!< Weighted prefix len, for Level-1 */
} arg_key_t;

/*!
 * \brief ALPM arguments containing all info describing a route
 */
typedef struct alpm_arg_s {
/* Associate Data Part */
    assoc_data_t   ad;
    alpm1_data_t   a1;
    alpm2_data_t   a2;
#if 0
    FORMAT_ENTRYt  fmt_ad;
    FORMAT_ENTRYt  fmt_a1;
    FORMAT_ENTRYt  fmt_a2;
#endif
    /*!< Route Hit */
    uint32_t hit:1;
    /*!< Default route (len == 0) */
    uint32_t default_route:1;
    /*!< Skip writing into cache/hardware */
    uint32_t skip_write:2;
    /*!< Valid Field */
    uint32_t valid:1;
    /*!< In move between level-1 blocks */
    uint32_t in_move:1;
    /*!< Set key as well for match */
    uint32_t set_key:1;
    /*!< Unpair TCAM packing mode, for encoding Level-1 unpair key */
    uint32_t upkm:1;
    /*!< Database index */
    uint32_t db:2;
    uint32_t repl_data_encode:1;
    uint32_t repl_data_decode:1;
    uint32_t fmt:3;
    uint32_t set_hit:1;
    uint32_t l1_db:2;
    uint32_t rsv0:14;

    
    /* int  full_data; */
    /*!< Kep part */
    arg_key_t key;
    /*!< Group for bucket, attached with pivot */
    log_bkt_t log_bkt[LEVELS];
    /*!< Pt index of levels */
    ln_index_t index[LEVELS];
    /*!< Pivot */
    alpm_pivot_t *pivot[LEVELS];
} alpm_arg_t;

/*!
 * \brief ALPM configs
 */
typedef struct alpm_config_s {
    /*!< ALPM mode */
    int alpm_mode;

    /*!< Num of levels  */
    int db_levels[DBS];

    /*!< Num of uft banks for LN level */
    int banks[LEVELS];

    /*!< Is uRPF enabled */
    int is_urpf;

    /*!< Level1 key input */
    int l1_key_input[ALPM_BLOCKS];

    /*!< Level1 DB select */
    int l1_db[ALPM_BLOCKS];

    /*!< Is PKM128 enabled for ALPM ver0 */
    bool ver0_128_enable;
} alpm_config_t;

/*!
 * \brief ALPM format type
 */
typedef enum format_type_e {
    FORMAT_PIVOT,
    FORMAT_ROUTE,
    FROMAT_TYPES
} format_type_t;


/*******************************************************************************
  Function prototypes
 */
/*!
 * \brief Format type for a given database & level
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 *
 * \return Format type.
 */
extern format_type_t
bcmptm_rm_alpm_format_type_get(int u, int db, int ln);

/*!
 * \brief Get database index for vt
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type
 *
 * \return database index
 */
extern int
bcmptm_rm_alpm_db(int u, alpm_vrf_type_t vt);

/*!
 * \brief Insert bucket elem into trie & usage list.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] key Trie key of adding elem
 * \param [in] bkt Target bucket
 * \param [in] bkt_elem Payload to insert
 *
 * \return SHR_E_NONE, SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_bkt_insert(int u, int ln, arg_key_t *key,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t *bkt_elem);

/*!
 * \brief Delete bucket elem from trie & usage list.
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] key Trie key of deleting elem
 * \param [in] bkt Target bucket
 * \param [out] bkt_elem Payload deleted
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_bkt_delete(int u, int ln, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bucket);

/*!
 * \brief Lookup (exact match) bucket elem in trie
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] pivot Pivot payload
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_bkt_lookup(int u, key_tuple_t *tp,
                          elem_bkt_t *bkt,
                          alpm_bkt_elem_t **bkt_elem);

/*!
 * \brief Allocate level elem based on a level, also init it.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] t Key tuple
 * \param [out] elem Elem payload
 *
 * \return SHR_E_NONE, SHR_E_MEMORY
 */
extern int
bcmptm_rm_alpm_elem_alloc(int u, int db, int ln,
                          key_tuple_t *t,
                          alpm_elem_t **elem);

/*!
 * \brief Free pivot payload
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] elem Elem payload
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_elem_free(int u, int db, int ln, alpm_elem_t *elem);

/*!
 * \brief Insert pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Inserting pivot payload
 *
 * \return SHR_E_NONE, SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_pvt_insert(int u, int db, int ln,
                          arg_key_t *key, alpm_pivot_t *pivot);

/*!
 * \brief Delete pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Deleted pivot payload
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_delete(int u, int db, int ln,
                          key_tuple_t *tp, alpm_pivot_t **pivot);

/*!
 * \brief Lookup (exact match) pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Pivot payload found
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_lookup(int u, int db, int ln,
                          key_tuple_t *tp, alpm_pivot_t **pivot);

/*!
 * \brief Lookup (LPM match) pivot payload in pivot trie
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] tp ALPM key tuple
 * \param [out] pivot Pivot payload found
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_find(int u, int db, int ln,
                        key_tuple_t *tp, alpm_pivot_t **pivot);

/*!
 * \brief Create pivot trie for a level
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] ipv IP version
 * \param [in] w_vrf Weighted VRF
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND
 */
extern int
bcmptm_rm_alpm_pvt_trie_create(int u, int db, int ln, int ipv, int w_vrf);

/*!
 * \brief Destroy pivot trie for a level
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] ipv IP version
 * \param [in] w_vrf Weighted VRF
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_pvt_trie_destroy(int u, int db, int ln, int ipv, int w_vrf);

/*!
 * \brief Get ALPM pivot trie
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] ln Level
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [out] pivot_trie Pivot trie
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_pvt_trie_get(int u, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t **pivot_trie);

/*!
 * \brief Set ALPM pivot trie
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] ln Level
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [in] pivot_trie Pivot trie
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_pvt_trie_set(int u, int db, int ln, int w_vrf, int ipv,
                            rm_alpm_trie_t *pivot_trie);

/*!
 * \brief Increase route count
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [out] cnt Count after incresead
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_vrf_route_inc(int u, int db, int w_vrf, int ipv, int *cnt);

/*!
 * \brief Decrease route count
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 * \param [out] cnt Count after decreased
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_vrf_route_dec(int u, int db, int w_vrf, int ipv, int *cnt);

/*!
 * \brief Get route count
 *
 * \param [in] u Device u.
 * \param [in] database Database index
 * \param [in] w_vrf Weight vrf
 * \param [in] ipv IP version
 *
 * \return route count
 */
extern int
bcmptm_rm_alpm_vrf_route_cnt(int u, int db, int w_vrf, int ipv);

/*!
 * \brief Get database
 *
 * \param [in] u Device u.
 * \param [in] db Database
 *
 * \return Pointer to database
 */
extern alpm_db_t*
bcmptm_rm_alpm_database_get(int u, int db);

/*!
 * \brief Get database count
 *
 * \param [in] u Device u.
 *
 * \return Number of databases
 */
extern int
bcmptm_rm_alpm_database_count(int u);


/*!
 * \brief Init level common database
 *
 * \param [in] u Device u.
 * \param [in] config ALPM config
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_database_init(int u, alpm_config_t *config);

/*!
 * \brief Cleanup level common database
 *
 * \param [in] u Device u.
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_database_cleanup(int u);

#endif /* RM_ALPM_COMMON_H */

