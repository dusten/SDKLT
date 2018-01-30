/*! \file rm_alpm_bucket.h
 *
 * Main routines for RM ALPM Level-N bucket
 *
 * This file contains routines manage ALPM Level-N (SRAM) resouces,
 * which are internal to ALPM resource manager
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

#ifndef RM_ALPM_BUCKET_H
#define RM_ALPM_BUCKET_H


/*******************************************************************************
   Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */
#define HEAD 1
#define BODY 0
#define MAX_INT_VALUE           0x7FFFFFFF
#define DISABLED_FORMAT         0
#define DEFAULT_FORMAT          1
#define START_FORMAT            1
#define INVALID_FORMAT          0xFF
#define VER0_FORMAT_5           5
#define VER0_FORMAT_4           4
#define VER0_FORMAT_3           3
#define VER0_FORMAT_2           2
#define VER0_FORMAT_1           1

#define MAX_SUB_BKTS            4
#define IP_LENGTH_GROUP_MAX     33  /* (128 / 4) + 1) */
#define IP_LENGTH_GROUP_SHIFT   2

#define ALPM_BIT_GET(_a, _b)          ((_a) & (1 << (_b)))
#define ALPM_BIT_SET(_a, _b)          ((_a) |= (1 << (_b)))
#define ALPM_BIT_CLR(_a, _b)          ((_a) &= ~(1 << (_b)))

#define BKT_HDL(u, _d, _l)            (sbkt_hdl[u][_d][_l])
#define RBKTS(u, _d, _l)              (BKT_HDL(u, _d, _l)->rbkt_array)
#define RBKT(u, _d, _l, _i)           (BKT_HDL(u, _d, _l)->rbkt_array[_i])
#define GROUP_USAGES(u, _d, _l)       (BKT_HDL(u, _d, _l)->group_usage)
#define GROUP_USAGE(u, _d, _l, _c)    (BKT_HDL(u, _d, _l)->group_usage[_c])
#define GROUP_USAGE_CNT(u, _d, _l)    (BKT_HDL(u, _d, _l)->group_usage_count)
#define FMT_GROUP_USAGES(u, _d, _l, _f)     \
    (BKT_HDL(u, _d, _l)->fmt_group_usage[_f])
#define FMT_GROUP_USAGE(u, _d, _l, _f, _c)  \
    (BKT_HDL(u, _d, _l)->fmt_group_usage[_f][_c])

#define BKT_HDL_GROUPS_USED(u, _d, _l)  (BKT_HDL(u, _d, _l)->groups_used)
#define BKT_HDL_RBKTS_TOTAL(u, _d, _l)  (BKT_HDL(u, _d, _l)->rbkts_total)
#define BKT_HDL_RBKTS_USED(u, _d, _l)   (BKT_HDL(u, _d, _l)->rbkts_used)
#define BKT_HDL_MAX_BANKS(u, _d, _l)    (BKT_HDL(u, _d, _l)->max_banks)
#define BKT_HDL_BUCKET_BITS(u, _d, _l)  (BKT_HDL(u, _d, _l)->bucket_bits)
#define BKT_HDL_FORMAT_TYPE(u, _d, _l)  (BKT_HDL(u, _d, _l)->format_type)
#define BKT_HDL_GROUP_LIST(u, _d, _l)   (BKT_HDL(u, _d, _l)->global_group_list)

#define GROUP_USAGE_LIST(u, _d, _l, _c)  (GROUP_USAGE(u, _d, _l, _c).groups)
#define GROUP_USAGE_COUNT(u, _d, _l, _c) (GROUP_USAGE(u, _d, _l, _c).count)
#define FMT_GROUP_USAGE_LIST(u, _d, _l, _f, _c)  \
    (FMT_GROUP_USAGE(u, _d, _l, _f, _c).groups)
#define FMT_GROUP_USAGE_COUNT(u, _d, _l, _f, _c) \
    (FMT_GROUP_USAGE(u, _d, _l, _f, _c).count)

#define RBKT_IDX(u, _d, _l, _i)          (RBKT(u, _d, _l, _i).index)
#define RBKT_USG_NODE(u, _d, _l, _i)     (RBKT(u, _d, _l, _i).usgnode)
#define RBKT_FMT_USG_NODE(u, _d, _l, _i) (RBKT(u, _d, _l, _i).fmt_usgnode)
#define RBKT_LIST_NODE(u, _d, _l, _i)    (RBKT(u, _d, _l, _i).listnode)

typedef enum {
    BUCKET_SHARE = 0,
    BUCKET_MERGE_CHILD_TO_PARENT,
    BUCKET_MERGE_PARENT_TO_CHILD
} bucket_merge_type_t;

/*******************************************************************************
  Typedefs
 */
typedef struct bucket_traverse_data_s {
    shr_dq_t dq[IP_LENGTH_GROUP_MAX];
    uint32_t count;
    bool found_new;
} bucket_traverse_data_t;

/*!
 * \brief ALPM raw bucket
 */
typedef struct rbkt_s {
    /*!< Rbkt index for this rbkt, only inited once */
    int index;

    /*!< Entry bitmap */
    uint32_t entry_bitmap;

    /*!< Format value */
    uint8_t format;

    /*!< Is current rbkt head of group */
    uint8_t is_head;

    /*!< Valid banks for this group, valid for HEAD only */
    uint8_t valid_banks;

    /*!< Free banks for this group, valid for HEAD only */
    uint32_t free_banks;

    /*!< DQ node to link all group heads in the Global List */
    shr_dq_t listnode;

    /*!< DQ node to link all group heads in the Usage List */
    shr_dq_t usgnode;

    /*!< DQ node to link all group heads in the Usage List */
    shr_dq_t fmt_usgnode;

    /*!< Pivot pointer for this group, valid for HEAD only */
    alpm_pivot_t *pivot;

    /*!< No. of entries in group */
    uint8_t count;
     /*!< Bitmap of logical buckets */
    uint8_t sub_bkts;
     /*!< Pivot indices corresponding to logical buckets */
    alpm_pivot_t *pivots[MAX_SUB_BKTS];
} rbkt_t;

/*!
 * \brief ALPM group usage list
 */
typedef struct usage_list_s {
    /*!< Free count of all these groups in the dq list.
     * The biggest count is the max_banks, if a group's
     * free buckets is more than the max_banks, that group will also
     * add to the same list with count equals to max_banks.
     * Therefore, the count is not accurate for these groups, which
     * may have larger count.
     */
    int count;

    /*!< Group DQ list to link all groups with same free count */
    shr_dq_t groups;
} usage_list_t;

/*!
 * \brief ALPM Bucket Handle
 *
 * ALPM bucket handle contains the info describing bucket info in a level
 */
typedef struct bkt_hdl_s {
    /*!< Number of used rbkt groups */
    int groups_used;

    /*!< Total number of rbkts */
    int rbkts_total;

    /*!< Used number of rbkts */
    int rbkts_used;

    /*!< Big array of rbkts */
    rbkt_t *rbkt_array;

    /*!< Max banks  */
    uint8_t max_banks;

    /*!< Bucket bits */
    uint8_t bucket_bits;

    /*!< Format type */
    format_type_t format_type;

    /*!< Global group list to link all groups in a sequential way */
    shr_dq_t global_group_list;

    /*!< Array of usage list, each with different free count */
    usage_list_t *group_usage;

    /*!< Count of array group_usage */
    uint8_t group_usage_count;

    /*!< Array of usage list, each with different format & free count */
    usage_list_t **fmt_group_usage;
} bkt_hdl_t;


typedef void (*bkt_hdl_cleanup_f)(int u);
typedef int (*bkt_hdl_init_f)(int u, bool recover);
typedef int (*bkt_hdl_get_f)(int u, int db, int ln, bkt_hdl_t **bkt_hdl);
typedef void (*alpm_data_encode_f)(int u, int ln, alpm_arg_t *arg, void *adata);
typedef alpm_pivot_t *(*bucket_pivot_get_f)(int u, int db, int ln,
                                            ln_index_t ln_index);
typedef void (*bucket_pivot_set_f)(int u, int db, int ln, log_bkt_t group,
                                   alpm_pivot_t *pivot);
typedef int (*bucket_free_f)(int u, int db, int ln, log_bkt_t group);
typedef int (*bucket_alloc_f)(int u, int ln, alpm_arg_t *arg, log_bkt_t *group);
typedef int (*bucket_delete_f)(int u, int db, int ln, alpm_pivot_t *pivot,
                               ln_index_t ent_idx);
typedef int (*bucket_insert_f)(int u, int ln, alpm_arg_t *arg,
                               alpm_pivot_t *pivot);
typedef int (*bucket_split_f)(int u, int ln, alpm_arg_t *arg,
                              alpm_pivot_t *parent_pivot,
                              bkt_split_aux_t *split_aux);
typedef int (*entry_write_f)(int u, int ln, alpm_arg_t *arg);
typedef int (*entry_read_f)(int u, int ln, alpm_arg_t *arg);
typedef uint32_t (*entry_hw_idx_get_f)(int u, int db, int ln,
                                       ln_index_t ent_idx);
typedef void (*bucket_sanity_f)(int u, int db, int ln, log_bkt_t log_bkt);
typedef int (*bucket_recover_f)(int u, int ln, alpm_arg_t *arg);
typedef int (*bucket_recover_done_f)(int u, int db, int ln);

typedef struct bucket_mgmt_s {
    bkt_hdl_cleanup_f bkt_hdl_cleanup;
    bkt_hdl_init_f bkt_hdl_init;
    bkt_hdl_get_f  bkt_hdl_get;
    alpm_data_encode_f alpm_data_encode;
    bucket_pivot_get_f bucket_pivot_get;
    bucket_pivot_set_f bucket_pivot_set;
    bucket_free_f bucket_free;
    bucket_alloc_f bucket_alloc;
    bucket_delete_f bucket_delete;
    bucket_insert_f bucket_insert;
    bucket_split_f  bucket_split;
    entry_write_f entry_write;
    entry_read_f entry_read;
    entry_hw_idx_get_f entry_hw_idx_get;
    bucket_sanity_f bucket_sanity;
    bucket_recover_f bucket_recover;
    bucket_recover_done_f bucket_recover_done;
} bucket_mgmt_t;
/*******************************************************************************
   Function prototypes
 */
/*!
 * \brief Cleanup ALPM bucket handle.
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX.
 */
extern void
bcmptm_rm_alpm_bkt_hdl_cleanup(int u);

/*!
 * \brief Initiliaze ALPM bucket handle.
 *
 * \param [in] u Device u.
 * \param [in] recover True if recover process.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_bkt_hdl_init(int u, bool recover);

/*!
 * \brief Get alpm bkt handle for level-n
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] bkt_hdl Bucket handle
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND.
 */
extern int
bcmptm_rm_alpm_bkt_hdl_get(int u, int db, int ln,
                           bkt_hdl_t **bkt_hdl);


extern void
bcmptm_rm_alpm_adata_encode(int u, int ln, alpm_arg_t *arg, void *adata);

extern alpm_pivot_t *
bcmptm_rm_alpm_bucket_pivot_get(int u, int db, int ln, ln_index_t ln_index);
extern void
bcmptm_rm_alpm_bucket_pivot_set(int u, int db, int ln, log_bkt_t group,
                                alpm_pivot_t *pivot);
extern int
bcmptm_rm_alpm_bucket_free(int u, int db, int ln, log_bkt_t group);
extern int
bcmptm_rm_alpm_bucket_alloc(int u, int ln, alpm_arg_t *arg, log_bkt_t *group);

extern int
bcmptm_rm_alpm_bucket_delete(int u, int db, int ln, alpm_pivot_t *pivot,
                             ln_index_t ent_idx);

extern int
bcmptm_rm_alpm_bucket_insert(int u, int ln, alpm_arg_t *arg,
                             alpm_pivot_t *pivot);

extern int
bcmptm_rm_alpm_bucket_split(int u, int ln, alpm_arg_t *arg,
                            alpm_pivot_t *parent_pivot,
                            bkt_split_aux_t *split_aux);

extern int
bcmptm_rm_alpm_entry_write(int u, int ln, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_entry_read(int u, int ln, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_entry_hw_idx_get(int u, int db, int ln, ln_index_t ent_idx,
                                uint32_t *hw_idx);

extern int
bcmptm_rm_alpm_bucket_recover(int u, int ln, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_bucket_recover_done(int u, int db, int ln);

/*!
 * \brief Sanity check for a given rbkt group
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Rbkt group
 * \param [in] with_holes If true, there exists holes in the group, else false.
 * \param [in] vbanks Valid banks
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_bucket_sanity(int u, int db, int ln, log_bkt_t log_bkt);

extern void
bcmptm_rm_alpm_all_buckets_sanity(int u, int db, int ln);

extern int
bcmptm_rm_alpm_bucket_ver0_register(int u, bucket_mgmt_t *bmgmt);
extern int
bcmptm_rm_alpm_bucket_ver1_register(int u, bucket_mgmt_t *bmgmt);
extern int
bcmptm_rm_alpm_bucket_init(int u);
extern void
bcmptm_rm_alpm_bucket_cleanup(int u);

#endif /* RM_ALPM_BUCKET_H */

