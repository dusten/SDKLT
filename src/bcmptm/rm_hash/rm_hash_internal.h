/*! \file rm_hash_internal.h
 *
 * Utils, defines internal to RM Hash state
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
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

#ifndef RM_HASH_INTERNAL_H
#define RM_HASH_INTERNAL_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Defines
 */

#define RM_HASH_MAX_BANK_COUNT                  8
#define RM_HASH_DUAL_BANK_COUNT                 2
#define RM_HASH_MAX_HW_ENTRY_INFO_COUNT         4
#define RM_HASH_MAX_ENTRY_NUM_PER_BUCKET        4
#define RM_HASH_MAX_KEY_TYPE_COUNT              32
#define RM_HASH_MAX_RSRC_GRP_COUNT              2

/*
 *! \brief Bit flags indicating if a specific bucket mode has been set.
 */
#define RM_HASH_BMN                            (1 << 0)
#define RM_HASH_BM0                            (1 << 1)
#define RM_HASH_BM1                            (1 << 2)
#define RM_HASH_BM2                            (1 << 3)

/*
 *! \brief Number of base buckets for each bucket mode.
 */
#define RM_HASH_BMN_BASE_BKT_NUM               1
#define RM_HASH_BM0_BASE_BKT_NUM               1
#define RM_HASH_BM1_BASE_BKT_NUM               2
#define RM_HASH_BM2_BASE_BKT_NUM               4

#define COMPILER_64_SET(dst, src_hi, src_lo) \
    ((dst) = (((uint64_t) ((uint32_t)(src_hi))) << 32) | ((uint64_t) ((uint32_t)(src_lo))))
#define COMPILER_64_BITTEST(val, n)     \
    ((((uint64_t)val) & (((uint64_t) 1)<<(n))) != ((uint64_t) 0))
#define COMPILER_64_SHL(dst, bits)    ((dst) <<= (bits))
#define COMPILER_64_SHR(dst, bits)    ((dst) >>= (bits))
#define COMPILER_64_AND(dst, src)     ((dst) &= (src))
#define COMPILER_64_LO(src)           ((uint32_t) (src))

/*******************************************************************************
 * Typedefs
 */
/*
 *! \brief Transaction state for rm hash
 */
typedef enum rm_hash_trans_state_e{
    /*! \brief Idle state */
    BCMPTM_RM_HASH_STATE_IDLE = 0,

    /*! \brief Uncommited changes in active DB */
    BCMPTM_RM_HASH_STATE_UC_A = 1,

    /*! \brief Commiting changes */
    BCMPTM_RM_HASH_STATE_INVALIDATING_U = 2,

    /*! \brief Aborting changes */
    BCMPTM_RM_HASH_STATE_RESTORING_U = 3
} rm_hash_trans_state_t;

/*
 *! \brief Opcode for undo instruction
 */
typedef enum rm_hash_trans_undo_opcode_e{
    /*! \brief Undo an entry insert operation */
    BCMPTM_RM_HASH_UNDO_ENT_INSERT = 0,

    /*! \brief Undo an entry delete operation */
    BCMPTM_RM_HASH_UNDO_ENT_DELETE = 1
} rm_hash_trans_undo_opcode_t;

/*
 *! \brief Enum for all the possible the bucket modes
 *
 * This data structure describes all the possible hardware bucket modes.
 */
typedef enum rm_hash_bm_e {
    RM_HASH_BM_N = 0,
    RM_HASH_BM_0,
    RM_HASH_BM_1,
    RM_HASH_BM_2,
    RM_HASH_BM_CNT
} rm_hash_bm_t;

/*
 *! \brief Size of the entry in the unit of base entry
 *
 * This data structure describes the size of all the possible entries.
 */
typedef struct rm_hash_ent_size_map_s {
    /*! \brief Base entry bitmap is valid or not */
    bool valid;

    /*! \brief Size of the entry */
    uint8_t size;
} rm_hash_ent_size_map_t;

/*!
 * \brief Information about a requested entry
 *
 * This data structure describes the information about the request entry.
 */
typedef struct rm_hash_req_ent_info_s {
    /*! \brief Size of the requested entry in unit of base entry */
    uint8_t e_size;

    /*! \brief Valid base bucket bitmap in SLB that the entry can reside */
    uint8_t vbb_bmp;

    /*! \brief Bucket mode of the entry */
    rm_hash_bm_t e_bm;
} rm_hash_req_ent_info_t;

/*
 *! \brief Information about the software logical bucket.
 *
 * This data structure describes the software logical bucket used for
 * managing hash buckets for various devices in a uniform manner.
 */
typedef struct rm_hash_ent_slb_info_s {
    /*! \brief Software logical bucket index */
    uint32_t slb;

    /*! \brief Valid hardware base bucket(s) for this entry */
    uint8_t vbb_bmp;
} rm_hash_ent_slb_info_t;

/*
 *! \brief Location info for an entry
 *
 * This data structure describes the location within a software logical
 * bucket where an entry exists.
 */
typedef struct rm_hash_ent_loc_s {
    /*! \brief Bank on which the entry exists */
    uint8_t lbank;

    /*! \brief Base bucket on which the entry exists */
    uint8_t bb_bmp;

    /*! \brief Base entries occupied by the entry in the base bucket */
    uint8_t be_bmp;

    /*! \brief logical entries occupied by the entry in the base bucket */
    uint8_t le_bmp;
} rm_hash_ent_loc_t;

/*
 *! \brief Bucket mode for a specific key type value
 *
 * This data structure describes the bucket mode for a specific key type.
 */
typedef struct rm_hash_key_type_attrib_s {
    /*! \brief Valid or not */
    bool valid;

    /*! \brief Bucket mode for this key type */
    rm_hash_bm_t bm;
} rm_hash_key_type_attrib_t;

/*
 *! \brief Entry node for shared hash table entry
 *
 * This data structure describes the entry node for an entry of shared
 * hash tables.
 */
typedef struct rm_hash_shr_ent_node_s {
    /*! \brief Physical table sid thru which the entry was installed */
    bcmdrd_sid_t sid;

    /*! \brief Physical table index of the entry */
    uint32_t e_idx;

    /*! \brief Signature for this entry */
    uint32_t sig;

    /*! \brief List of all the logical buckets for the entry */
    rm_hash_ent_slb_info_t slb_info[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Bucket mode of the entry */
    rm_hash_bm_t bm;

    /*! \brief Location of the entry */
    rm_hash_ent_loc_t e_loc;
} rm_hash_shr_ent_node_t;

/*
 *! \brief Entry node for dual hash table entry
 *
 * This data structure describes the entry node for an entry of dual
 * hash tables.
 */
typedef struct rm_hash_dual_ent_node_s {
    /*! \brief Physical table sid thru which the entry was installed */
    bcmdrd_sid_t sid;

    /*! \brief Physical table index of the entry */
    uint32_t e_idx;

    /*! \brief Signature for this entry */
    uint32_t sig;

    /*! \brief List of all the logical buckets for the entry */
    rm_hash_ent_slb_info_t slb_info[RM_HASH_DUAL_BANK_COUNT];

    /*! \brief Bucket mode of the entry */
    rm_hash_bm_t bm;

    /*! \brief Location of the entry */
    rm_hash_ent_loc_t e_loc;
} rm_hash_dual_ent_node_t;

/*
 *! \brief Entry info for either shared hash entry or the dual hash entry
 *
 * This data structure describes the entry info for either shared entry node
 * or for an entry of dual hash tables.
 */
typedef union rm_hash_ent_node_u {
    /*! \brief Dual hash entry node */
    rm_hash_dual_ent_node_t de_node;

    /*! \brief Shared hash entry node */
    rm_hash_shr_ent_node_t se_node;
} rm_hash_ent_node_t;

/*!
 * \brief Information about base entry and logical entry
 *
 * This data structure describes which base entries and
 * logical entries have been used in a software logical bucket.
 */
typedef struct rm_hash_slb_ent_bmp_s {
    /*! \brief Used base entries in this slb */
    uint16_t be_bmp;

    /*! \brief Used logical entries in this slb */
    uint16_t le_bmp[RM_HASH_BM_CNT];
} rm_hash_slb_ent_bmp_t;

/*
 *! \brief Information about a software logical bucket.
 *
 * This data structure describes the information about a software logical
 * bucket - slb.
 */
typedef struct rm_hash_slb_state_s {
    /*! \brief Offset of the 1st entry node belonging to this slb */
    uint32_t e_node_offset;

    /*! \brief Entry base entry and logical entry info */
    rm_hash_slb_ent_bmp_t e_bmp;

    /*! \brief Count of existing valid entries in this slb */
    uint8_t ve_cnt;
} rm_hash_slb_state_t;

/*
 *! \brief Information about used base entry counter for a device
 *
 * This data structure describes the counter of base entries for all the
 * hash tables for a device.
 */
typedef struct rm_hash_dev_ent_ctr_desc_s {
    /*! \brief Flag indicating if hash collision will be distinguished with
     * real table full condition.
     */
    bool differ_collision;

    /*! \brief Number of counters used for recording used base entry */
    uint16_t cnt;

    /*! \brief Array of counters used for recording used base entry */
    uint32_t array[1];
} rm_hash_dev_ent_ctr_desc_t;

/*
 * ! \brief Counter of various software states used in hash resource manager
 * for a device.
 *
 * This data structure describes the amount of various software state
 */
typedef struct rm_hash_state_elems_amount_s {
    /*! \brief Count of rm_hash_slb_state_t objects */
    uint32_t slb_cnt;

    /*! \brief Count of rm_hash_shr_ent_node_t objects */
    uint32_t shr_e_node_cnt;

    /*! \brief Count of rm_hash_dual_ent_node_t objects */
    uint32_t dual_e_node_cnt;

    /*! \brief Count of counters used for recording used based entries */
    uint16_t uent_ctr_cnt;
} rm_hash_state_elems_amount_t;

/*
 * ! \brief Structure used for describing hash vector computation.
 *
 * This structure is used for describing the hash vector computation.
 */
typedef struct rm_hash_vector_info_s {
    /*! \brief Vector type */
    bcmptm_rm_hash_vector_type_t type;

    /*! \brief Offset into hash vector */
    uint16_t offset;

    /*! \brief Mask in the unit of hardware base bucket */
    uint32_t mask;
} rm_hash_vector_info_t;

/*
 * ! \brief Structure used for describing the attributes of a physical hash table.
 *
 * This structure is defined on per physical sid basis, so for each ptsid,
 * there will be a structure object for it. In addition, its field value will
 * be set during hash resource manager initialization, and will not change
 * during runtime.
 */
typedef struct rm_hash_pt_info_s {
    /*! \brief If this structure has been initialized */
    bool init;

    /*! \brief Hash bank mode for a given LTID */
    bcmptm_rm_hash_type_t hash_type;

    /*! \brief All the supported bucket modes */
    uint8_t bmf;

    /*! \brief Number of valid logical banks */
    uint8_t num_lbanks;

    /*! \brief Key attribute list */
    rm_hash_key_type_attrib_t key_attrib[RM_HASH_MAX_KEY_TYPE_COUNT];

    /*! \brief Logical bank list, using eligible bank to index */
    uint8_t lbank_list[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Eligible bank list, using valid logical bank to index */
    uint8_t ebank_list[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Logical to phyiscal bank mapping. */
    uint8_t pbank_list[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Number of slbs for each logical bank */
    uint32_t lbank_size[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Base slb of each logical bank */
    uint32_t lbank_base[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Vector info, using eligible bank to index */
    rm_hash_vector_info_t vec_info[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Total number of slbs */
    uint32_t num_slbs;

    /*! \brief Size of the slb in the unit of base entry */
    uint8_t slb_size;

    /*! \brief HA sub module ID for slb state. */
    uint8_t b_state_hdl;

    /*! \brief HA sub module ID for entry node. */
    uint8_t e_node_hdl;

    /*! \brief Pointer to the bucket state. */
    rm_hash_slb_state_t *b_state;

    /*! \brief Pointer to the entry node. */
    void *e_node;

    /*! \brief Used entry counter */
    uint32_t e_ctr_offset;

    /*! \brief Robust hash config */
    bcmptm_rm_hash_rhash_cfg_t *rh_cfg;
} rm_hash_pt_info_t;

/*
 * ! \brief Structure used for storing an entry snapshot.
 */
typedef struct rm_hash_ent_snap_s {
    /*! \brief Buffer used for storing an entry snapshot */
    uint32_t ent_words[BCMPTM_MAX_PT_ENTRY_WORDS];
} rm_hash_ent_snap_t;

/*
 * ! \brief Structure used for describing the snapshot info for a table
 * instance.
 */
typedef struct rm_hash_tbl_snap_s {
    /*! \brief Max possible entry count for a table instance */
    uint32_t ent_snap_max;

    /*! \brief Count of entries found for a table instance */
    uint32_t ent_snap_cnt;

    /*! \brief Buffer containg pointers to the sorted snapshot
     * for a table instance.
     */
    rm_hash_ent_snap_t **sorted_ent_snap_base;

    /*! \brief Buffer containing the entry snapshot */
    rm_hash_ent_snap_t ent_snap_array[1];
} rm_hash_tbl_snap_t;

/*
 * ! \brief Structure used for describing the snapshot info for a LT.
 */
typedef struct rm_hash_lt_travs_info_s {
    /*! \brief Count of table instance snapshot for the LT.
     * For example ExactMatch based LTs will have multiple tbl_inst.
     */
    int tbl_snap_cnt;

    /*! \brief Snapshot array for all the table instances */
    rm_hash_tbl_snap_t *tbl_snap_array[1];
} rm_hash_lt_travs_info_t;

/*
 * ! \brief All the required information for a group that a LT can map.
 *
 * A LT can map to 1 or multiple separated physical tables. This structure is
 * used for describing a single physical table.
 */
typedef struct rm_hash_grp_info_s {
    /*! \brief SID of the resource group */
    bcmdrd_sid_t sid;

    /*! \brief pt info used for describing the resource group. */
    rm_hash_pt_info_t *pt_info;

    /*! \brief List of valid hardware entry info */
    uint8_t vhei_idx[RM_HASH_MAX_HW_ENTRY_INFO_COUNT];

    /*! \brief Number of valid hardware entry info */
    uint8_t num_vhei;
} rm_hash_grp_info_t;

/*
 * ! \brief All the required information for a ltid for a hash operation.
 *
 * This structure is defined on per ltid basis, so for each ltid, there
 * will be a structure for it. In additional, its field value will be set
 * during hash resource manager initialization, and will not change during
 * runtime.
 */
typedef struct rm_hash_lt_ctrl_s {
    /*! \brief LTID used for a key for searching */
    bcmltd_sid_t ltid;

    /*! \brief bcmptm_rm_hash_lt_info_t object for a ltid */
    bcmptm_rm_hash_lt_info_t lt_info;

    /*! \brief Number of resource groups */
    uint8_t num_grp;

    /*! \brief Array of all the resource group info. */
    rm_hash_grp_info_t grp_info[RM_HASH_MAX_RSRC_GRP_COUNT];

    /*! \brief Hash reordering move depth for a ltid. */
    uint8_t move_depth;

    /*! \brief Traversal info for this LT */
    rm_hash_lt_travs_info_t *travs_info;
} rm_hash_lt_ctrl_t;

/*
 *! \brief Information about lt ctrl of a device.
 *
 * This data structure describes the information about hash lt ctrl for a device.
 */
typedef struct rm_hash_dev_lt_ctrl_s {
    /*! \brief Count of lt ctrl for a device */
    uint16_t cnt;

    /*! \brief Current number of registered ltid */
    uint16_t curr;

    /*! \brief Array of lt ctrls */
    rm_hash_lt_ctrl_t array[1];
} rm_hash_dev_lt_ctrl_t;

/*
 *! \brief Information about PT sid and corresponding physical attribute
 *
 * This data structure describes the rsrc manager physical table information.
 * In other words, it describes the mapping between physical hash table sid and
 * its corresponding physical table info. The instance of this structure will
 * be allocated, configured and used during system initialization. This structure
 * will be organized on per physical table sid basis.
 */
typedef struct rm_hash_pt_reg_info_s {
    /*! \brief Physical table sid belonging to a logical table */
    bcmdrd_sid_t sid;

    /*! \brief Overlay physical table sid */
    bcmdrd_sid_t ovly_sid;

    /*! \brief Physical table information from resource manager perspective */
    rm_hash_pt_info_t *pt_info;

    /*! \brief Pointer to the next rm_hash_pt_reg_info_t object */
    struct rm_hash_pt_reg_info_s *next;
}rm_hash_pt_reg_info_t;

/*
 *! \brief Information about PT register information for a device.
 *
 * This data structure describes the rsrc manager physical table information
 * for a device.
 */
typedef struct rm_hash_dev_pt_reg_info_s {
    /*! \brief Number of LTID */
    uint16_t num_ltids;

    /*! \brief Pointer to the next rm_hash_pt_reg_info_t object */
    rm_hash_pt_reg_info_t *head;
}rm_hash_dev_pt_reg_info_t;

/*
 * ! \brief Information about the physical and logical bucket list
 * in which the entry search will be performed.
 */
typedef struct rm_hash_srch_bkt_info_s {
    /*! \brief Hardware logical bucket list */
    uint32_t hlb_list[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Software logical bucket list */
    rm_hash_ent_slb_info_t slb_info_list[RM_HASH_MAX_BANK_COUNT];
} rm_hash_srch_bkt_info_t;

/*
 *! \brief Information about the physical and logical bank list
 * in which the entry search will be performed.
 */
typedef struct rm_hash_srch_bank_info_s {
    /*! \brief Logical bank list on which the search will be performed */
    uint8_t lbank_list[RM_HASH_MAX_BANK_COUNT];

    /*! \brief Number of logical banks on which the search will be performed */
    uint8_t num_lbanks;
} rm_hash_srch_bank_info_t;

/*
 *! \brief Information about hash table search result
 */
typedef struct rm_hash_srch_result_s {
    /*! \brief Flag indicating if entry has been found or not */
    bool e_exist;

    /*! \brief Entry node of the found entry */
    rm_hash_ent_node_t e_node;

    /*! \brief Ltid of the entry obtainded from PTcache */
    bcmltd_sid_t ltid;

    /*! \brief Index into hw entry info (view info) list */
    uint8_t hei_idx;

    /*! \brief Data format id from PTcache */
    uint16_t dft_id;

    /*! \brief Buffer containing the entry content */
    uint32_t e_words[BCMPTM_MAX_PT_ENTRY_WORDS];
} rm_hash_srch_result_t;

/*
 *! \brief Information about transaction maintained by rm hash
 */
typedef struct rm_hash_trans_record_s {
    /*! \brief Current transaction sequence id */
    uint32_t cseq_id;

    /*! \brief Current transaction state */
    rm_hash_trans_state_t trans_state;
} rm_hash_trans_record_t;

/*
 *! \brief Information about an undo operation in the undo list
 */
typedef struct rm_hash_trans_undo_instr_s {
    /*! \brief Opcode of an undo operation */
    rm_hash_trans_undo_opcode_t opcode;

    /*! \brief Offset of the bucket state within which the undo will operate */
    uint32_t b_state_offset;

    /*! \brief Offset of base entry counter */
    uint16_t ube_ctr_offset;

    /*!  \brief Hash type */
    bcmptm_rm_hash_type_t hash_type;

    /*! \brief Entry node for which the undo operation will be performed */
    rm_hash_ent_node_t e_node;
} rm_hash_trans_undo_instr_t;

/*
 *! \brief Information about transaction undo list.
 *
 * This data structure describes the information about transaction undo list.
 */
typedef struct rm_hash_trans_undo_list_s {
    /*! \brief Current count of undo instructions in the undo list */
    uint32_t curr;

    /*! \brief Length of undo instruction list */
    uint32_t cnt;

    /*! \brief Array of undo instructions */
    rm_hash_trans_undo_instr_t array[1];
} rm_hash_trans_undo_list_t;

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get the configuration if the real table full will be distinguished
 * with hash collision.
 *
 * \param [in] unit Unit number.
 * \param [out] differ_collision Pointer to buffer to record the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_differ_collision_get(int unit, bool *differ_collision);

/*!
 * \brief Set the configuration if the real table full will be distinguished
 * with hash collision.
 *
 * \param [in] unit Unit number.
 * \param [in] differ_collision If distinguish table full with collision.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_differ_collision_set(int unit, bool differ_collision);

/*!
 * \brief Get the number of used base entries
 * with hash collision.
 *
 * \param [in] unit Unit number.
 * \param [in] ctr_offset Entry counter offset.
 * \param [out] pctr Pointer to the entry counter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmptm_rm_hash_used_bent_ctr_get(int unit,
                                 uint16_t ctr_offset,
                                 uint32_t **pctr);

/*!
 * \brief Update transaction record when bcmptm_rm_hash_req is called.
 *
 * \param [in] unit Unit number.
 * \param [in] cseq_id Current transaction id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_ent_req_cmd(int unit, uint32_t cseq_id);

/*!
 * \brief Get the transaction record of a device.
 *
 * \param [in] unit Unit number.
 * \param [in] ptrans_rec Pointer to the buffer to record the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_record_get(int unit,
                                rm_hash_trans_record_t **ptrans_rec);

/*!
 * \brief Get the number and the list of all the valid hardware entry info
 * for a ltid.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] ltid Requested ltid.
 * \param [in] lt_info Pointer to the bcmptm_rm_hash_lt_info_t structure.
 * \param [out] cnt Pointer to the buffer to record the info count.
 * \param [out] idx_list Pointer to the buffer to record the info list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_hei_get(int unit,
                       bcmptm_rm_hash_req_t *req_info,
                       bcmltd_sid_t ltid,
                       const bcmptm_rm_hash_lt_info_t *lt_info,
                       uint8_t *cnt,
                       uint8_t *idx_list);

/*!
 * \brief Insert an entry thru moving existing entry in a table
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Operation flags passed by PTM.
 * \param [in] req_ltid Requested ltid.
 * \param [in] ptsid Corresponding SID.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] depth Depth of the entry move operation.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lbank Logical bank number.
 * \param [in] slb_info_list Software logical bucket list.
 * \param [in] ent_size Entry size.
 * \param [in] e_bm Entry bucket mode.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_insert_with_ent_move(int unit,
                                    uint32_t req_flags,
                                    bcmltd_sid_t req_ltid,
                                    bcmdrd_sid_t ptsid,
                                    rm_hash_pt_info_t *pt_info,
                                    int depth,
                                    const bcmptm_rm_hash_more_info_t *more_info,
                                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                                    bcmptm_rm_hash_req_t *req_info,
                                    uint8_t lbank,
                                    rm_hash_ent_slb_info_t *slb_info_list,
                                    uint8_t ent_size,
                                    rm_hash_bm_t e_bm,
                                    bcmptm_rm_hash_rsp_t *rsp_info);

/*!
 * \brief Specify the bank list on which the entry search will be performed.
 *
 * \param [in] unit Unit number.
 * \param [in] lbank_list Search bank list.
 * \param [in] num_lbanks Search bank number.
 * \param [out] sbank_info Pointer to rm_hash_srch_bank_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_ent_srch_bank_info_init(int unit,
                                       uint8_t *lbank_list,
                                       uint8_t num_lbanks,
                                       rm_hash_srch_bank_info_t *sbank_info);

/*!
 * \brief Search the requested entry from the hash table.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Flags specified by LTM/PTM.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_info Pointer to rm_hash_grp_info_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] sbkt_info Pointer to rm_hash_srch_bkt_info_t structure.
 * \param [in] sbank_info Pointer to rm_hash_srch_bank_info_t structure.
 * \param [out] sresult Pointer to rm_hash_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_search(int unit,
                          uint32_t req_flags,
                          bcmptm_rm_hash_req_t *req_info,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          const bcmptm_rm_hash_lt_info_t *lt_info,
                          rm_hash_grp_info_t *grp_info,
                          const bcmptm_rm_hash_more_info_t *more_info,
                          rm_hash_srch_bkt_info_t *sbkt_info,
                          rm_hash_srch_bank_info_t *sbank_info,
                          rm_hash_srch_result_t *sresult);

/*!
 * \brief Set up the bucket list context in which an entry will be searched.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [out] sbkt_info Pointer to rm_hash_srch_bkt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_srch_slb_info_get(int unit,
                                     bcmptm_rm_hash_req_t *req_info,
                                     const bcmptm_rm_hash_lt_info_t *lt_info,
                                     rm_hash_pt_info_t *pt_info,
                                     const bcmptm_rm_hash_more_info_t *more_info,
                                     rm_hash_srch_bkt_info_t *sbkt_info);
/*!
 * \brief Compute hash vector for a given hash table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to entry.
 * \param [in] key_a Pointer to entry key a.
 * \param [in] key_b Pointer to entry key b.
 * \param [in] key_size Key size.
 * \param [in] bank_list Pointer to bank list.
 * \param [in] num_banks Number of banks to compute vector.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] bucket Pointer to hash vector.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_vector_compute(int unit,
                              uint32_t *entry,
                              uint8_t *key_a,
                              uint8_t *key_b,
                              int key_size,
                              uint8_t *bank_list,
                              uint8_t num_banks,
                              const bcmptm_rm_hash_more_info_t *more_info,
                              rm_hash_pt_info_t *pt_info,
                              uint32_t *bucket);

/*!
 * \brief Dump the detailed information for all the physical hash tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid logical table id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_dump(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Get rm more info from entry request info
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [out] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_more_info_get(int unit,
                                 bcmptm_rm_hash_req_t *req_info,
                                 rm_hash_lt_ctrl_t *lt_ctrl,
                                 const bcmptm_rm_hash_more_info_t **more_info);

/*!
 * \brief Get all the valid hardware info list for a given rm more info
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] rm_more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [out] cnt Count of valid hardware entry info.
 * \param [out] idx_list Pointer to the list of valid hardware entry info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_given_key_ent_info_get(int unit,
                                      const bcmptm_rm_hash_lt_info_t *lt_info,
                                      const bcmptm_rm_hash_more_info_t *rm_more_info,
                                      uint8_t *cnt,
                                      uint8_t *idx_list);

/*!
 * \brief Get the maxium bucket mode for a given hash table
 *
 *
 * \param [in] unit Unit number.
 * \param [in] bmf Flag indicating all the supported bucket mode for a table.
 * \param [out] max_bm Buffer to contain the maximum bucket mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_max_bm_get(int unit, uint8_t bmf, rm_hash_bm_t *max_bm);

/*!
 * \brief Get software logical bucket based on hardware logical bucket
 *
 * For the provided entry bucket mode and hardware logical bucket list,
 * this function will calculate the corresponding software logical
 * bucket list and valid base bucket bitmap within each software logical
 * bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_bm Entry bucket mode.
 * \param [in] hlb_list Pointer to hardware logical bucket list.
 * \param [out] slb_info_list List of calculated software logical bucket.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_hlb_to_slb_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              rm_hash_bm_t e_bm,
                              uint32_t *hlb_list,
                              rm_hash_ent_slb_info_t *slb_info_list);

/*!
 * \brief Get the logical bank number corresponding to a software logical bucket
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb_info Pointer to rm_hash_ent_slb_info_t structure.
 * \param [out] lbank Pointer to a buffer to contain the calculated bank.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_slb_to_lbank_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint32_t slb,
                                uint8_t *lbank);

/*!
 * \brief Get entry index from a physical table perspective
 *
 * This function get physical table index for an entry, based on the following
 * location information: software logical bucket, bank number, base bucket
 * bitmap within the software logical bucket, base entry bitmap within the
 * base bucket, etc.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [in] e_size Size of the entry in the unit of base entry.
 * \param [out] e_index Pointer to a buffer to contain the calculated entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_index_get(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint32_t slb,
                             rm_hash_ent_loc_t *e_loc,
                             uint8_t e_size,
                             uint32_t *e_index);

/*!
 * \brief Get entry bucket mode
 *
 * This function get bucket mode for an entry. For a hash operation requested
 * by LTM, the corresponding rm more info structure will be determined. Thus
 * the key type value will be determined. And thus the key type will determine
 * the bucket mode of the entry.
 *
 * \param [in] unit Unit number.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [out] e_bm Pointer to rm_hash_bm_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
bcmptm_rm_hash_ent_bkt_mode_get(int unit,
                                const bcmptm_rm_hash_more_info_t *more_info,
                                rm_hash_pt_info_t *pt_info,
                                rm_hash_bm_t *e_bm);

/*!
 * \brief Get lt_ctrl grp info for a specific hash table ltid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table sid.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [out] grp_info Pointer to rm_hash_grp_info_t* structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_grp_info_get(int unit,
                                   bcmdrd_sid_t sid,
                                   rm_hash_lt_ctrl_t *lt_ctrl,
                                   rm_hash_grp_info_t **grp_info);

/*!
 * \brief Get lt_ctrl for a specific hash table ltid.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id.
 * \param [out] plt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_get(int unit,
                           bcmltd_sid_t ltid,
                           rm_hash_lt_ctrl_t **plt_ctrl);

/*!
 * \brief Instantiate an object of rm_hash_lt_ctrl_t.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_add(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Init resource manager lt control for hash table
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_init(int unit);

/*!
 * \brief Clean up resource manager lt control for hash table
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lt_ctrl_cleanup(int unit);

/*!
 * \brief Allocate and partially initialize an object of the structure
 * rm_hash_flex_pt_reg_info_t.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_reg_info_add(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Get the hash table memory info for a given sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table symbol id.
 * \param [out] reg_info Pointer to rm_hash_pt_reg_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_reg_info_get(int unit,
                               bcmdrd_sid_t sid,
                               rm_hash_pt_reg_info_t **reg_info);

/*!
 * \brief Get the header of the rm_hash_pt_reg_info_t list for a device.
 *
 * \param [in] unit Unit number.
 * \param [out] reg_info Pointer to rm_hash_pt_reg_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_dev_pt_reg_info_get(int unit,
                                   rm_hash_pt_reg_info_t **reg_info);

/*!
 * \brief Get number of ltid registered.
 *
 * \param [in] unit Unit number.
 * \param [out] num_ltids Number of ltid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_num_ltids_get(int unit, uint16_t *num_ltids);

/*!
 * \brief Cleanup pt register info
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_reg_info_cleanup(int unit);

/*!
 * \brief * Dump the detailed information for all the physical hash tables.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pt_reg_info_dump(int unit);

/*!
 * \brief Get the bitmap of used base entries for an entry
 *
 * This function will get the bitmap of used base entries for an entry
 * from the software logical bucket point of view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_node Pointer to rm_hash_shr_ent_node_t structure.
 * \param [out] slb_be_bmp Pointer to a buffer to contain the calculated bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_slb_view_be_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_be_bmp);

/*!
 * \brief Get the bitmap of logical entry occupied by an entry
 *
 * This function will get the bitmap of logical entry for an entry
 * from the software logical bucket point of view.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_node Pointer to rm_hash_shr_ent_node_t structure.
 * \param [out] slb_le_bmp Pointer to a buffer to contain the calculated bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_slb_view_le_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_le_bmp);

/*!
 * \brief Allocate an entry node
 *
 * The entry node for each hash bucket is allocated in HA memory space during
 * system initialization. This routine finds the free entry node from the entry
 * node set belonging to the given hash bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [out] pe_node Pointer to a buffer to contain the address of node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_shr_ent_node_allocate(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     rm_hash_slb_state_t *b_state,
                                     rm_hash_shr_ent_node_t **pe_node);

/*!
 * \brief Allocate an entry node
 *
 * The entry node for each hash bucket is allocated in HA memory space during
 * system initialization. This routine finds the free entry node from the entry
 * node set belonging to the given hash bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [out] pe_node Pointer to a buffer to contain the address of node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_dual_ent_node_allocate(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      rm_hash_slb_state_t *b_state,
                                      rm_hash_dual_ent_node_t **pe_node);


/*!
 * \brief Insert an entry node
 *
 * Insert an entry node into the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] e_node Pointer to a buffer to contain the address of node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_shr_ent_node_insert(int unit,
                                   rm_hash_pt_info_t *pt_info,
                                   rm_hash_slb_state_t *b_state,
                                   rm_hash_shr_ent_node_t *e_node);

/*!
 * \brief Insert an entry node
 *
 * Insert an entry node into the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] e_node Pointer to a buffer to contain the address of node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_dual_ent_node_insert(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    rm_hash_slb_state_t *b_state,
                                    rm_hash_dual_ent_node_t *de_node);

/*!
 * \brief Delete an entry node
 *
 * Delete an entry node from the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] bb_bmp base bucket bitmap.
 * \param [in] be_bmp base entry bitmap.
 * \param [out] pe_node Pointer to an entry node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_shr_ent_node_delete(int unit,
                                   rm_hash_pt_info_t *pt_info,
                                   rm_hash_slb_state_t *b_state,
                                   uint8_t bb_bmp,
                                   uint8_t be_bmp,
                                   rm_hash_shr_ent_node_t **pe_node);

/*!
 * \brief Delete an entry node
 *
 * Delete an entry node from the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] bb_bmp base bucket bitmap.
 * \param [in] be_bmp base entry bitmap.
 * \param [out] pe_node Pointer to an entry node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_dual_ent_node_delete(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    rm_hash_slb_state_t *b_state,
                                    uint8_t bb_bmp,
                                    uint16_t be_bmp,
                                    rm_hash_dual_ent_node_t **pe_node);

/*!
 * \brief Get an entry node.
 *
 * Get valid ent count, base ent offset within the bucket for each valid
 * ent, and size (num of base ent) of each valid ent.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_node_offset Offset of the requested entry node.
 * \param [out] pe_node Pointer to an entry node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_shr_ent_node_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint32_t e_node_offset,
                                rm_hash_shr_ent_node_t **pe_node);

/*!
 * \brief Get an entry node.
 *
 * Get valid ent count, base ent offset within the bucket for each valid
 * ent, and size (num of base ent) of each valid ent.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_node_offset Offset of the requested entry node.
 * \param [out] pe_node Pointer to an entry node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_dual_ent_node_get(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint32_t e_node_offset,
                                 rm_hash_dual_ent_node_t **pe_node);

/*!
 * \brief Examine if there is enough space in a given bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb_e_bmp Pointer to rm_hash_slb_ent_bmp_t structure.
 * \param [in] e_info Pointer to rm_hash_req_ent_info_t structure.
 * \param [out] accom Pointer to bool variable.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_bkt_free_ent_space_get(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      rm_hash_slb_ent_bmp_t *slb_e_bmp,
                                      rm_hash_req_ent_info_t *e_info,
                                      bool *accom,
                                      rm_hash_ent_loc_t *e_loc);

/*!
 * \brief Examine if there is enough space in a given bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] he_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] e_bm Entry bucket mode.
 * \param [out] e_loc_found Indicates if free slot is found.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_free_ent_space_search(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     const bcmptm_rm_hash_hw_entry_info_t *he_info,
                                     rm_hash_ent_slb_info_t *slb_info_list,
                                     rm_hash_bm_t e_bm,
                                     bool *e_loc_found,
                                     rm_hash_ent_loc_t *e_loc);

/*!
 * \brief Check if the physical table has been real table full status.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_size Entry size.
 * \param [out] full Indicates if table is full.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_table_fullness_get(int unit,
                                  rm_hash_pt_info_t *pt_info,
                                  uint8_t e_size,
                                  bool *full);

/*!
 * \brief Update the bucket state after an entry is inserted.
 *
 * After a new entry is inserted, a corresponding entry node which records
 * the state of the entry from the resource management perspective will be
 * allocated, populated and placed at the appropriate location.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [in] slb_info Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] sid Physical table symbol id.
 * \param [in] e_idx Entry index.
 * \param [in] e_sig Signature of the entry node.
 * \param [in] bm Bucket node of the entry.
 * \param [in] e_size Size of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_bkt_state_set(int unit,
                             rm_hash_pt_info_t *pt_info,
                             rm_hash_ent_loc_t *e_loc,
                             rm_hash_ent_slb_info_t *slb_info,
                             bcmdrd_sid_t sid,
                             uint32_t e_idx,
                             uint32_t e_sig,
                             rm_hash_bm_t bm,
                             uint8_t e_size);

/*!
 * \brief Update the bucket state after an ent is deleted.
 *
 * After a new entry is deleted, the corresponding entry node which records
 * the state of the entry from the resource management perspective will be
 * deleted accordingly.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb Software logical bucket.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_bkt_state_clear(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint32_t slb,
                               rm_hash_ent_loc_t *e_loc);

/*!
 * \brief Free memory space for various software state elements.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_state_elems_cleanup(int unit);

/*!
 * \brief Determine if the given entry needs to be compared.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_info Pointer to rm_hash_grp_info_t structure.
 * \param [in] e_node Pointer to rm_hash_shr_ent_node_t structure.
 * \param [in] e_bm Entry bucket mode.
 * \param [in] e_sig Entry signature.
 * \param [out] eligible If the entry needs to be compared.
 * \param [out] vhei_idx HW entry info index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_eligible_for_shr_ent_compare(int unit,
                                            const bcmptm_rm_hash_lt_info_t *lt_info,
                                            rm_hash_grp_info_t *grp_info,
                                            rm_hash_shr_ent_node_t *e_node,
                                            rm_hash_bm_t e_bm,
                                            uint32_t e_sig,
                                            bool *eligible,
                                            uint8_t *vhei_idx);

/*!
 * \brief bcmptm_rm_hash_eligible_for_ent_compare
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_info Pointer to rm_hash_grp_info_t structure.
 * \param [in] e_node Pointer to rm_hash_dual_ent_node_t structure.
 * \param [in] e_bm Entry bucket mode.
 * \param [in] e_sig Entry signature.
 * \param [out] eligible If the entry needs to be compared.
 * \param [out] vhei_idx HW entry info index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_eligible_for_dual_ent_compare(int unit,
                                             const bcmptm_rm_hash_lt_info_t *lt_info,
                                             rm_hash_grp_info_t *grp_info,
                                             rm_hash_dual_ent_node_t *e_node,
                                             rm_hash_bm_t e_bm,
                                             uint32_t e_sig,
                                             bool *eligible,
                                             uint8_t *vhei_idx);

/*!
 * \brief Get the transaction undo list of a device.
 *
 * \param [in] unit Unit number.
 * \param [out] ptrans_ulist Pointer to structure rm_hash_trans_undo_list_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_undo_list_get(int unit,
                                   rm_hash_trans_undo_list_t **ptrans_ulist);

/*!
 * \brief Invalidate the entire transaction undo list.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_ulist Pointer to structure rm_hash_trans_undo_list_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_undo_list_invalidate(int unit,
                                          rm_hash_trans_undo_list_t *trans_ulist);

/*!
 * \brief Insert an undo operation into the transaction undo list.
 *
 * \param [in] unit Unit number.
 * \param [in] b_state_offset Bucket state offset.
 * \param [in] ue_ctr_offset Used base entry counter offset.
 * \param [in] de_node Pointer to rm_hash_dual_ent_node_t structure.
 * \param [in] opcode Operation code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_ulist_dual_ent_insert(int unit,
                                           uint32_t b_state_offset,
                                           uint16_t ue_ctr_offset,
                                           rm_hash_dual_ent_node_t *de_node,
                                           rm_hash_trans_undo_opcode_t opcode);

/*!
 * \brief Insert an undo operation into the transaction undo list.
 *
 * \param [in] unit Unit number.
 * \param [in] b_state_offset Bucket state offset.
 * \param [in] ue_ctr_offset Used base entry counter offset.
 * \param [in] de_node Pointer to rm_hash_dual_ent_node_t structure.
 * \param [in] opcode Operation code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_ulist_shr_ent_insert(int unit,
                                          uint32_t b_state_offset,
                                          uint16_t ue_ctr_offset,
                                          rm_hash_shr_ent_node_t *de_node,
                                          rm_hash_trans_undo_opcode_t opcode);

/*!
 * \brief Execute all the undo operations in the transaction undo list.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_ulist Pointer to structure rm_hash_trans_undo_list_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_undo_list_restore(int unit,
                                       rm_hash_trans_undo_list_t *trans_ulist);

/*!
 * \brief Extract key from entry buffer
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry buffer.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [in] ent_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [in] sid_idx Index of the sid.
 * \param [out] key Buffer to record the key.
 * \param [out] key_size Buffer to record the key size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_key_extract(int unit,
                           uint32_t *entry,
                           const bcmptm_rm_hash_more_info_t *m_info,
                           const bcmptm_rm_hash_hw_entry_info_t *e_info,
                           bcmptm_rm_hash_rhash_cfg_t *rhcfg,
                           uint8_t rh_idx,
                           uint8_t sid_idx,
                           uint8_t *key,
                           int *key_size);

/*!
 * \brief Fill the key type value into entry buffer
 *
 * \param [in] unit Unit number.
 * \param [in] key_type Key type.
 * \param [in] ent_info Pointer to bcmptm_rm_hash_hw_entry_info_t structure.
 * \param [out] entry Entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_key_type_fill(int unit,
                             uint16_t key_type,
                             const bcmptm_rm_hash_hw_entry_info_t *ent_info,
                             uint32_t *entry);

/*!
 * \brief Generate signature for an entry
 *
 * \param [in] unit Unit number.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure corresponding
 *             to a entry.
 * \param [in] entry Entry buffer.
 * \param [out] ent_sig Pointer to buffer for recording entry signature.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_sig_generate(int unit,
                                const bcmptm_rm_hash_more_info_t *more_info,
                                uint32_t *entry,
                                uint32_t *ent_sig);

/*!
 * \brief Update transaction record when bcmptm_rm_hash_req is called
 *
 * \param [in] unit Unit number.
 * \param [in] cseq_id Current sequence id
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_ent_req_cmd(int unit,
                                 uint32_t cseq_id);

/*!
 * \brief Initialize entry move related resource.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_move_info_init(int unit);

/*!
 * \brief Cleanup entry move related resource.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_move_info_cleanup(int unit);

/*!
 * \brief Hash lookup request handler
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lookup(int unit,
                      uint32_t req_flags,
                      bcmltd_sid_t req_ltid,
                      bcmbd_pt_dyn_info_t *pt_dyn_info,
                      bcmptm_rm_hash_req_t *req_info,
                      size_t rsp_entry_buf_wsize,
                      uint32_t *rsp_entry_words,
                      bcmptm_rm_hash_rsp_t *rsp_info,
                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags);

/*!
 * \brief Process GET_FIRST/GET_NEXT opcode for LT traversal operation
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] cseq_id Current sequence id.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_op Operation code.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_travs_cmd_proc(int unit,
                              uint32_t req_flags,
                              uint32_t cseq_id,
                              bcmltd_sid_t req_ltid,
                              bcmbd_pt_dyn_info_t *pt_dyn_info,
                              bcmptm_op_type_t req_op,
                              bcmptm_rm_hash_req_t *req_info,
                              size_t rsp_entry_buf_wsize,
                              uint32_t *rsp_entry_words,
                              bcmptm_rm_hash_rsp_t *rsp_info,
                              bcmltd_sid_t *rsp_ltid,
                              uint32_t *rsp_flags);

/*!
 * \brief Get the overlay sid
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical hash table sid.
 * \param [out] ovly_sid Physical hash table overlay sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ovly_sid_get(int unit,
                            bcmdrd_sid_t sid,
                            bcmdrd_sid_t *ovly_sid);

/*!
 * \brief Get the SID used for recording pt info in lt ctrl structure.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] more_info Pointer to bcmptm_rm_hash_more_info_t structure.
 * \param [out] grp_sid Pointer to physical hash table sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_group_sid_get(int unit,
                             const bcmptm_rm_hash_lt_info_t *lt_info,
                             const bcmptm_rm_hash_more_info_t *more_info,
                             bcmdrd_sid_t *grp_sid);

#endif /* RM_HASH_INTERNAL_H */
