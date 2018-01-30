/*! \file bcmecmp_db_internal.h
 *
 * This file contains ECMP custom table handler data structures definitions.
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

#ifndef BCMECMP_DB_INTERNAL_H
#define BCMECMP_DB_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmecmp/bcmecmp.h>
#include <bcmecmp/bcmecmp_types.h>

/*!
 * \brief This macro is used to set a field present indicator bit status.
 *
 * Sets the field \c _f present bit indicator in the field bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMECMP_LT_FIELD_SET(_m, _f)    \
            do {                        \
                (_m) |= (1 << _f);      \
            } while (0)

/*!
 * \brief This macro is used to get the field present indicator bit status.
 *
 * Returns the field \c _f present bit indicator status from the field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the field bitmap.
 */
#define BCMECMP_LT_FIELD_GET(_m, _f)  ((_m) & (1 << (_f)))

/*! \brief The max number NextHop paths. */
#define BCMECMP_NHOP_ARRAY_SIZE (2048)

/*!
 * \brief ECMP Load Balance Modes.
 */
typedef enum bcmecmp_lb_mode_e {
    /*! Regular Load Balance Mode. */
    BCMECMP_LB_MODE_REGULAR = 0,

    /*! Resilient Hash Load Balance Mode. */
    BCMECMP_LB_MODE_RESILIENT,

    /*! Random Load Balance Mode. */
    BCMECMP_LB_MODE_RANDOM,

    /*! Round Robin Load Balance Mode. */
    BCMECMP_LB_MODE_ROUNDROBIN,

    /*! Raw Load Balance Mode. */
    BCMECMP_LB_MODE_RAW,

    /*! Must be the last one. */
    BCMECMP_LB_MODE_COUNT
} bcmecmp_lb_mode_t;

/*! \brief ECMP load balancing mode names. */
#define BCMECMP_LB_MODE_NAME {  \
            "REGULAR",          \
            "RESILIENT",        \
            "RANDOM",           \
            "ROUNDROBIN",       \
            "RAW"               \
}

/*!
 * \brief ECMP group lookup types.
 */
typedef enum bcmecmp_grp_type_e {
    BCMECMP_GRP_TYPE_SINGLE = 0,
    BCMECMP_GRP_TYPE_OVERLAY,
    BCMECMP_GRP_TYPE_UNDERLAY,
    BCMECMP_GRP_TYPE_COUNT
} bcmecmp_grp_type_t;

/*! \brief ECMP group level names. */
#define BCMECMP_LEVEL_NAME { \
            "SINGLE",        \
            "OVERLAY",       \
            "UNDERLAY"       \
}

/*!
 * \brief ECMP logical table fields information.
 */
typedef enum bcmecmp_lt_fields_e {

    /*! ECMP group identifier field is set. */
    BCMECMP_LT_FIELD_ECMP_ID = 0,

    /*! ECMP load balancing mode field is set. */
    BCMECMP_LT_FIELD_LB_MODE,

    /*! Sort ECMP next-hop interfaces status field is set. */
    BCMECMP_LT_FIELD_NHOP_SORTED,

    /*! Maximum number of ECMP paths allowed for the group field is set. */
    BCMECMP_LT_FIELD_MAX_PATHS,

    /*! Number of active ECMP paths configured for the group field is set. */
    BCMECMP_LT_FIELD_NUM_PATHS,

    /*! ECMP next-hop indicator field is set. */
    BCMECMP_LT_FIELD_ECMP_NHOP,

    /*! Underlay ECMP group identifier field is set. */
    BCMECMP_LT_FIELD_UECMP_ID,

    /*! Next-hop identifier field is set. */
    BCMECMP_LT_FIELD_NHOP_ID,

    /*! Resilient hashing group size field is set. */
    BCMECMP_LT_FIELD_RH_SIZE,

    /*! Must be the last one. */
    BCMECMP_LT_FIELD_COUNT

} bcmecmp_fields_t;

/*!
 * \brief Structure for ECMP group table DRD fields information.
 */
typedef struct bcmecmp_grp_drd_fields_s {

    /*! Load Balancing Mode DRD field identifier. */
    bcmdrd_fid_t lb_mode;

    /*! Member table Base Pointer DRD field identifier. */
    bcmdrd_fid_t base_ptr;

    /*! Number of members in ECMP group DRD field identifier. */
    bcmdrd_fid_t count;

    /*! Round-robin counter DRD field identifier. */
    bcmdrd_fid_t rrlb_cnt;
 } bcmecmp_grp_drd_fields_t;

/*!
 * \brief Structure for ECMP group table LTD fields information.
 */
typedef struct bcmecmp_grp_ltd_fields_t_s {

    /*! ECMP group identifier LTD field identifier. */
    bcmecmp_ltd_fid_t ecmp_id_fid;

    /*! Load Balancing Mode LTD field identifier. */
    bcmecmp_ltd_fid_t lb_mode_fid;

    /*! Next-hop identifiers sorted indicator. */
    bcmecmp_ltd_fid_t nhop_sorted_fid;

    /*! Maximum ECMP paths LTD field identifier. */
    bcmecmp_ltd_fid_t max_paths_fid;

    /*! Number of ECMP paths configured LTD field identifier. */
    bcmecmp_ltd_fid_t num_paths_fid;

    /*! Underlay ECMP next-hops indicator. */
    bcmecmp_ltd_fid_t ecmp_nhop_fid;

    /*! Underlay ECMP group identifier LTD field identifier. */
    bcmecmp_ltd_fid_t uecmp_id_fid;

    /*! Next-hop identifier LTD field identifier. */
    bcmecmp_ltd_fid_t nhop_id_fid;

    /*! Resilient hashing group size LTD field identifier. */
    bcmecmp_ltd_fid_t rh_size_fid;
} bcmecmp_grp_ltd_fields_t;

/*!
 * \brief Structure for ECMP member table DRD fields information.
 */
typedef struct bcmecmp_memb_drd_fields_s {
    /*! Next Hop ID or ECMP Group ID indicator DRD field identifier. */
    bcmdrd_fid_t ecmp_flag;

    /*! Underlay ECMP Group ID DRD field identifier. */
    bcmdrd_fid_t group_id;

    /*! Next Hop ID DRD field identifier. */
    bcmdrd_fid_t nhop_id;
} bcmecmp_memb_drd_fields_t;

/*!
 * \brief ECMP hardware tables resource manager operation flags.
 */
typedef enum bcmecmp_oper_e {
    /*! No operation for Init value. */
    BCMECMP_OPER_NONE       = 0,

    /*! Entry add operation. */
    BCMECMP_OPER_ADD        = (1 << 1),

    /*! Entry delete operation. */
    BCMECMP_OPER_DEL        = (1 << 2),

    /*! Entry add operation with skip hardware index zero option. */
    BCMECMP_OPER_SKIP_ZERO  = (1 << 3),

    /*! Must be last one. */
    BCMECMP_OPER_COUNT
} bcmecmp_oper_t;

/*!
 * \brief Structure for ECMP logical table fields information.
 */
typedef struct bcmecmp_lt_field_attrs_s {
    /*! Minimum ECMP Group Identifier value supported. */
    int min_ecmp_id;

    /*! Maximum ECMP Group Identifier value supported. */
    int max_ecmp_id;

    /*! Minimum ECMP load balancing mode value supported. */
    uint32_t min_lb_mode;

    /*! Maximum ECMP load balancing mode value supported. */
    uint32_t max_lb_mode;

    /*! Load balancing modes supported for the logical table. */
    uint32_t lb_mode[BCMECMP_LB_MODE_COUNT];

    /*! Paths count (MAX_PATHS & NUM_PATHS) must be in powers-of-two. */
    bool pow_two_paths;

    /*! Minimum ECMP paths allowed for groups. */
    uint32_t min_paths;

    /*! Maximum ECMP paths supported for groups (hardware limit). */
    uint32_t max_paths;

    /*! Minimum value for configured ECMP paths allowed for groups. */
    uint32_t min_npaths;

    /*! Maximum value for configured ECMP paths allowed for groups. */
    uint32_t max_npaths;

    /*! Default ECMP next-hop field value for ECMP_OVERLAY logical table. */
    bool def_ecmp_nhop;

    /*! Minimum Underlay ECMP identifier value supported by this device. */
    int min_uecmp_id;

    /*! Maximum Underlay ECMP identifier value supported by this device. */
    int max_uecmp_id;

    /*! Minimum next-hop identifier value supported by this device. */
    int min_nhop_id;

    /*! Maximum next-hop identifier value supported by this device. */
    int max_nhop_id;

    /*! Minimum resilient hashing group size supported by the device. */
    uint32_t min_rh_size_enc;

    /*! Maximum resilient hashing group size supported by the device. */
    uint32_t max_rh_size_enc;
} bcmecmp_lt_field_attrs_t;

/*!
 * \brief Structure for ECMP logical table entry information.
 */
typedef struct bcmecmp_lt_entry_s {

    /*! ECMP group logial table source identifier. */
    bcmltd_sid_t glt_sid;

    /*! Logial table operation transaction identifier value. */
    uint32_t trans_id;

    /*! Logical table entry field set/present indicator Bit Map. */
    SHR_BITDCL fbmp;

    /*! Logical table entry group type - BCMECMP_GRP_TYPE_XXX . */
    bcmecmp_grp_type_t grp_type;

    /*! ECMP group identifier - Entry Key field value. */
    bcmecmp_id_t ecmp_id;

    /*! Load Balance mode field value. */
    uint32_t lb_mode;

    /*!
     * Field value indicates next-hops must be sorted prior to installation in
     * member table in hardware.
     */
    bool nhop_sorted;

    /*! Maximum number of paths supported by the group field value. */
    uint32_t max_paths;

    /*! Numumber of paths configured for this group field value. */
    uint32_t num_paths;

    /*! ECMP next-hop indicator field value. */
    bool ecmp_nhop;

    /*! Underlay ECMP group identifier field array elements. */
    bcmecmp_id_t uecmp_id[BCMECMP_NHOP_ARRAY_SIZE];

    /*! Input ECMP_UNDERLAY_IDs count. */
    uint32_t uecmp_ids_count;

    /*! Next-hop indices field array elements. */
    bcmecmp_nhop_id_t nhop_id[BCMECMP_NHOP_ARRAY_SIZE];

    /*! Input NHOP_IDs count. */
    uint32_t nhop_ids_count;

    /*! Resilient hashing group size hardware encoding value. */
    uint32_t rh_size_enc;

    /*! Resilient hashing group size value in member entries count. */
    uint32_t rh_entries_cnt;

    /*! Array of resilient hashing load balanced members. */
    int *rh_entries_arr;

    /*! Array of RH-ECMP group entry's mapped member's array index. */
    uint32_t *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by a next-hop member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t *rh_memb_ecnt_arr;

    /*! Member table start index allocated for this ECMP group. */
    int mstart_idx;
} bcmecmp_lt_entry_t;

/*!
 * \brief Structure for ECMP hardware table entry information.
 */
typedef struct bcmecmp_hw_entry_info_s {

    /*! Entry reference count information. */
    uint16_t ref_cnt;

    /*! Entry flags information. */
    uint8_t flags;
} bcmecmp_hw_entry_info_t;

/*!
 * \brief Structure for ECMP hardware table information.
 */
typedef struct bcmecmp_tbl_prop_s {

    /*! Device Resouce Database table Symbol ID */
    bcmdrd_sid_t drd_sid;

    /*! Logical table Database Symbol ID. */
    bcmltd_sid_t ltd_sid;

    /*! Table minimum valid index value. */
    int imin;

    /*! Table maximum valid index value. */
    int imax;

    /*! Max used index. */
    int imax_used;

    /*! ECM group table size. */
    int tbl_size;

    /*! Table Entry Size in words. */
    uint32_t ent_size;

    /*! Pointer to logical table DRD field ID - bcmecmp_grp_drd_fields_t. */
    void *drd_fids;

    /*! Pointer to logical table LTD field ID - bcmecmp_grp_ltd_fields_t. */
    void *ltd_fids;

    /*! Pointer to logical table field attributes - bcmecmp_lt_field_attrs_t. */
    void *lt_fattrs;

    /*! Entry information array. */
    bcmecmp_hw_entry_info_t *ent_arr;

    /*! Table entry backup array elements base pointer.  */
    bcmecmp_hw_entry_info_t *ent_bk_arr;
} bcmecmp_tbl_prop_t;

/*!
 * \brief Structure for ECMP hardware table information.
 */
typedef struct bcmecmp_reg_prop_s {

    /*! Device Resouce Database register Symbol ID */
    bcmdrd_sid_t drd_sid;

    /*! Logical table Database Symbol ID. */
    bcmltd_sid_t ltd_sid;

    /*! Register entry size in words. */
    uint32_t ent_size;

    /*! ECMP resolution mode DRD field identifier. */
    bcmdrd_fid_t mode_fid;

    /*! Device ECMP resolution mode hardware encoding value.  */
    uint32_t mode_hw_val[BCMECMP_MODE_COUNT];
} bcmecmp_reg_prop_t;

/*!
 * \brief Structure for ECMP hardware table information.
 */
typedef struct bcmecmp_tbl_op_s {

    /*! Table pointer. */
    bcmecmp_tbl_prop_t *tbl_ptr;

    /*! Type of Operation. */
    uint32_t oper_flags;

    /*! Number of entries to be allocated. */
    uint32_t num_ent;

    /*! Allocated entry index. */
    int free_idx;
} bcmecmp_tbl_op_t;

/*!
 * \brief Structure for Resilient hashing ECMP group information.
 */
typedef struct bcmecmp_rh_grp_info_s {

    /*! Array of resilient hashing load balanced members. */
    int *rh_entries_arr;

    /*! Array of RH-ECMP group entry's mapped member's array index. */
    uint32_t *rh_mindex_arr;

    /*!
     * Number of member table entries occupied by a next-hop member in a
     * resilient hashing load balancing mode group.
     */
    uint32_t *rh_memb_ecnt_arr;
} bcmecmp_rh_grp_info_t;

/*!
 * \brief Structure for ECMP group information.
 */
typedef struct bcmecmp_grp_info_s {

    /*! Transaction identifer related to this group. */
    uint32_t trans_id;

    /*! Logical table entry group type - BCMECMP_GRP_TYPE_XXX . */
    bcmecmp_grp_type_t grp_type;

    /*! Group logial table source identifier. */
    bcmltd_sid_t glt_sid;

    /*! Load balancing mode configured for this group. */
    uint32_t lb_mode;

    /*! Group next-hops sorted status indicator. */
    bool nhop_sorted;

    /*! Maximum ECMP paths supported by this group. */
    uint32_t max_paths;

    /*! Number of ECMP paths configured for this group. */
    uint32_t num_paths;

    /*! Next-hop is ECMP next hop. */
    bool ecmp_nhop;

    /*! Resilient hashing group size configured for this group - HW encoding. */
    uint32_t rh_size_enc;

    /*! Maximum resilient hashing group size by this group. */
    uint32_t rh_entries_cnt;

    /*! Resilient hashing group mode details. */
    bcmecmp_rh_grp_info_t *rhg;

    /* Group next-hop members array. */
    bcmecmp_nhop_id_t *nhop_arr;

    /* Group underlay ECMP_ID members array. */
    bcmecmp_id_t *uecmp_arr;

    /*! Member table start index for this group. */
    int mstart_idx;
} bcmecmp_grp_info_t;

/*!
 * \brief Structure for ECMP group and hardware tables information.
 */
typedef struct bcmecmp_info_s {

    /*! Device ECMP mode. */
    uint32_t ecmp_mode;

    /* Resilient hashing load balancing mode pseudo-random number generator. */
    uint32_t rh_rand_seed;

    /*! Device ECMP config register information. */
    bcmecmp_reg_prop_t config;

    /*! Pre-IFP ECMP group table information. */
    bcmecmp_tbl_prop_t init_group[BCMECMP_GRP_TYPE_COUNT];

    /*! Pre-IFP ECMP member table information. */
    bcmecmp_tbl_prop_t init_member[BCMECMP_GRP_TYPE_COUNT];

    /*! Post-IFP ECMP group table information. */
    bcmecmp_tbl_prop_t group[BCMECMP_GRP_TYPE_COUNT];

    /*! Post-IFP ECMP member table information. */
    bcmecmp_tbl_prop_t member[BCMECMP_GRP_TYPE_COUNT];

    /*! ECMP Round-Robin load balancing count table information. */
    bcmecmp_tbl_prop_t rrlb_cnt[BCMECMP_GRP_TYPE_COUNT];

    /*! Widest entry size among all ECMP hardware tables. */
    uint32_t ent_wsize_max;

    /*! ECMP logical table entry information. */
    bcmecmp_lt_entry_t *lt_ent;

    /*! ECMP group information. */
    bcmecmp_grp_info_t *grp_arr;

    /*! ECMP group entires backup array. */
    bcmecmp_grp_info_t *grp_bk_arr;
} bcmecmp_info_t;

/*!
 * \brief Structure for ECMP control information.
 */
typedef struct bcmecmp_control_s {

    /*! ECMP feature initalized status indicator. */
    bool init;

    /*! ECMP module information. */
    bcmecmp_info_t ecmp_info;

    /*! ECMP device specific functions. */
    bcmecmp_drv_t *ecmp_drv;
} bcmecmp_control_t;

/*!
 * \brief ECMP Per-BCM unit control structure declaration.
 */
extern bcmecmp_control_t *ecmp_ctrl[BCMECMP_NUM_UNITS_MAX];

/*!
 * \brief Get BCM unit ECMP control structure pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to BCM unit's \c _u ECMP Control Structure.
 */
#define BCMECMP_CTRL(_u) (ecmp_ctrl[(_u)])

/*!
 * \brief Get ECMP feature initialized status.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP feature status on the BCM unit.
 */
#define BCMECMP_INIT(_u)                         \
            if (NULL == BCMECMP_CTRL(_u) ||      \
                !(BCMECMP_CTRL(_u)->init)) {     \
                SHR_RETURN_VAL_EXIT(SHR_E_INIT); \
            }

/*!
 * \brief Get ECMP information structure for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure of the unit.
 */
#define BCMECMP_INFO(_u) (ecmp_ctrl[_u]->ecmp_info)

/*!
 * \brief Get ECMP information structure pointer for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure pointer of the unit.
 */
#define BCMECMP_INFO_PTR(_u) (&(ecmp_ctrl[_u]->ecmp_info))

/*!
 * \brief Get ECMP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_LT_ENT_PTR(_u) ((BCMECMP_INFO_PTR(_u))->lt_ent)

/*!
 * \brief Get ECMP table structure pointer for a given table name.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Table name.
 *
 * \returns Pointer to the ECMP table structure.
 */
#define BCMECMP_TBL_PTR(_u, _var, _t) (&(BCMECMP_INFO(_u)._var[_t]))

/*!
 * \brief Set/Get the minimum entry index value supported for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Minimum entry index value supported by the table.
 */
#define BCMECMP_TBL_IDX_MIN(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).imin)

/*!
 * \brief Set/Get the maximum entry index value supported for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Maximum entry index value supported by the table.
 */
#define BCMECMP_TBL_IDX_MAX(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).imax)

/*!
 * \brief Set/Get the maximum entry index value in use for a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Maximum entry index value in use for the table.
 */
#define BCMECMP_TBL_IDX_MAX_USED(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).imax_used)

/*!
 * \brief Set/Get the maximum entry ID value supported by a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Maximum entry ID value supported by the table.
 */
#define BCMECMP_TBL_SIZE(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[_t]).tbl_size)

/*!
 * \brief Set/Get DRD symbol identifier value of a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns DRD symbol identifier of the table.
 */
#define BCMECMP_TBL_DRD_SID(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).drd_sid)

/*!
 * \brief Set/Get DRD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table Name of bcmecmp_tbl_prop_t type.
 *
 * \returns DRD symbol identifier values of fields in the table.
 */
#define BCMECMP_TBL_DRD_FIELDS(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).drd_fids)

/*!
 * \brief Set/Get the LTD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table Name of bcmecmp_tbl_prop_t type.
 *
 * \returns LTD symbol identifier values of fields in the table.
 */
#define BCMECMP_TBL_LTD_FIELDS(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).ltd_fids)

/*!
 * \brief Set/Get the LTD source identifier value of a given table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns LTD source identifier value of the table.
 */
#define BCMECMP_TBL_LTD_SID(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).ltd_sid)

/*!
 * \brief Set/Get the entry size in words for a given physical table/memory.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware table name of bcmecmp_tbl_prop_t type.
 *
 * \returns Entry size in words of the table/memory.
 */
#define BCMECMP_TBL_ENT_SIZE(_u, _var, _t) \
            ((BCMECMP_INFO(_u)._var[(_t)]).ent_size)

/*!
 * \brief Set/Get maximum entry size in words among all ECMP table entries.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum entry size in words among all ECMP table entries.
 */
#define BCMECMP_TBL_ENT_SIZE_MAX(_u) (BCMECMP_INFO(_u).ent_wsize_max)

/*!
 * \brief ECMP group logical table field attributes structure void * pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Void * pointer to logical table field attributes structure.
 */
#define BCMECMP_LT_FIELD_ATTRS_VOID_PTR(_u, _g, _t) \
            ((BCMECMP_INFO(_u)._g[(_t)]).lt_fattrs)

/*!
 * \brief ECMP group logical table field attributes structure pointer.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Pointer to ECMP Group logical table field attributes structure.
 */
#define BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t) \
            ((bcmecmp_lt_field_attrs_t*)((BCMECMP_INFO(_u)._g[(_t)]).lt_fattrs))

/*!
 * \brief Minimum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum ECMP ID value supported by the logical table.
 */
#define BCMECMP_LT_MIN_ECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_ecmp_id)

/*!
 * \brief Maximum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum ECMP ID value supported by the logical table.
 */
#define BCMECMP_LT_MAX_ECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_ecmp_id)

/*!
 * \brief Minimum value supported for LB_MODE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum LB_MODE value supported by the logical table.
 */
#define BCMECMP_LT_MIN_LB_MODE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_lb_mode)

/*!
 * \brief Maximum value supported for LB_MODE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum LB_MODE value supported by the logical table.
 */
#define BCMECMP_LT_MAX_LB_MODE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_lb_mode)

/*!
 * \brief LB_MODE name for a given LB_MODE enum value of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns LB_MODE name for the given LB_MODE enum value.
 */
#define BCMECMP_LT_LB_MODE(_u, _g, _t, _lbm) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->lb_mode[(_lbm)])

/*!
 * \brief Paths count (MAX_PATHS & NUM_PATHS) must be in powers-of-two.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Powers-of-two paths count requirement status.
 */
#define BCMECMP_LT_POW_TWO_PATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->pow_two_paths)

/*!
 * \brief Minimum value supported for MAX_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MIN_PATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_paths)

/*!
 * \brief Maximum value supported for MAX_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MAX_PATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_paths)

/*!
 * \brief Minimum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MIN_NPATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_npaths)

/*!
 * \brief Maximum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_LT_MAX_NPATHS(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_npaths)

/*!
 * \brief Default ECMP_NHOP field value for ECMP_OVERLAY LT entries.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Default value of ECMP_NHOP field for ECMP_OVERLAY LT entries.
 */
#define BCMECMP_LT_DEF_ECMP_NHOP(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->def_ecmp_nhop)

/*!
 * \brief Minimum value supported for ECMP_UNDERLAY_ID logical field.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for ECMP_UNDERLAY_ID logical field.
 */
#define BCMECMP_LT_MIN_UECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_uecmp_id)

/*!
 * \brief Maximum value supported for ECMP_UNDERLAY_ID logical field.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for ECMP_UNDERLAY_ID logical field.
 */
#define BCMECMP_LT_MAX_UECMP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_uecmp_id)

/*!
 * \brief Minimum value supported for NHOP_ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for NHOP_ID field of the logical table.
 */
#define BCMECMP_LT_MIN_NHOP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_nhop_id)

/*!
 * \brief Maximum value supported for NHOP_ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for NHOP_ID field of the logical table.
 */
#define BCMECMP_LT_MAX_NHOP_ID(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_nhop_id)

/*!
 * \brief Minimum value supported for RH_SIZE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Minimum value supported for RH_SIZE field of the logical table.
 */
#define BCMECMP_LT_MIN_RH_SIZE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->min_rh_size_enc)

/*!
 * \brief Maximum value supported for RH_SIZE field of the logical table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _g ECMP Group table array of bcmecmp_tbl_prop_t type.
 * \param [in] _t ECMP Group type.
 *
 * \returns Maximum value supported for RH_SIZE field of the logical table.
 */
#define BCMECMP_LT_MAX_RH_SIZE(_u, _g, _t) \
            (BCMECMP_LT_FIELD_ATTRS_PTR(_u, _g, _t)->max_rh_size_enc)

/*!
 * \brief ECMP group array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_GRP_INFO_PTR(_u) ((BCMECMP_INFO_PTR(_u))->grp_arr)

/*!
 * \brief Group ECMP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP resolution type of this group.
 */
#define BCMECMP_GRP_TYPE(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].grp_type)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_GRP_LT_SID(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief Get the load balancing mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_GRP_LB_MODE(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].lb_mode)

/*!
 * \brief Get next-hop identifiers sorted status for given group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of next-hop identifiers sorted status of the group.
 */
#define BCMECMP_GRP_NHOP_SORTED(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].nhop_sorted)

/*!
 * \brief Maximum ECMP paths supported by this ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Maximum ECMP paths supported by the group.
 */
#define BCMECMP_GRP_MAX_PATHS(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].max_paths)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_GRP_NUM_PATHS(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].num_paths)

/*!
 * \brief Next-hop is Underlay (Level 2) ECMP Group indicator.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of Underlay ECMP Group ID as Next-hop.
 */
#define BCMECMP_GRP_ECMP_NHOP(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].ecmp_nhop)

/*!
 * \brief Resilient hashing group size hardware encoding value of the group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group size hardware encoding value of the group.
 */
#define BCMECMP_GRP_RH_SIZE(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].rh_size_enc)

/*!
 * \brief Resilient hashing group total number of members count.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group total number of members count.
 */
#define BCMECMP_GRP_RH_ENTRIES_CNT(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].rh_entries_cnt)

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_GRP_RHG_PTR(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].rhg)

/*!
 * \brief Pointer to resilient hash load balanced members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced members array.
 */
#define BCMECMP_GRP_RH_ENTRIES_PTR(_u, _id) \
            (((BCMECMP_GRP_INFO_PTR(_u))[(_id)].rhg)->rh_entries_arr)

/*!
 * \brief Pointer to resilient hash load balanced member indices array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced member indices array.
 */
#define BCMECMP_GRP_RH_MINDEX_PTR(_u, _id) \
            (((BCMECMP_GRP_INFO_PTR(_u))[(_id)].rhg)->rh_mindex_arr)

/*!
 * \brief Number of member table entries occupied by a RH group member.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of member table entries occupied by a RH group member.
 */
#define BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(_u, _id) \
            (((BCMECMP_GRP_INFO_PTR(_u))[(_id)].rhg)->rh_memb_ecnt_arr)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_GRP_MEMB_TBL_START_IDX(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Pointer to group next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group next-hop members array.
 */
#define BCMECMP_GRP_NHOP_PTR(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].nhop_arr)

/*!
 * \brief Pointer to group underlay next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group underlay next-hop members array.
 */
#define BCMECMP_GRP_UECMP_PTR(_u, _id) \
            ((BCMECMP_GRP_INFO_PTR(_u))[(_id)].uecmp_arr)

/*!
 * \brief ECMP group entries backup array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group high availability data array base pointer.
 */
#define BCMECMP_GRP_INFO_BK_PTR(_u) ((BCMECMP_INFO_PTR(_u))->grp_bk_arr)

/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_GRP_TRANS_ID_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].trans_id)
/*!
 * \brief Group ECMP resolution type.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP resolution type of this group.
 */
#define BCMECMP_GRP_TYPE_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].grp_type)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_GRP_LT_SID_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief Get the load balancing mode of a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Load balancing mode of the ECMP group.
 */
#define BCMECMP_GRP_LB_MODE_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].lb_mode)

/*!
 * \brief Get next-hop identifiers sorted status for given group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of next-hop identifiers sorted status of the group.
 */
#define BCMECMP_GRP_NHOP_SORTED_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].nhop_sorted)

/*!
 * \brief Maximum ECMP paths supported by this ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Maximum ECMP paths supported by the group.
 */
#define BCMECMP_GRP_MAX_PATHS_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].max_paths)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_GRP_NUM_PATHS_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].num_paths)

/*!
 * \brief Next-hop is Underlay (Level 2) ECMP Group indicator.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Status of Underlay ECMP Group ID as Next-hop.
 */
#define BCMECMP_GRP_ECMP_NHOP_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].ecmp_nhop)

/*!
 * \brief Resilient hashing group size hardware encoding value of the group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group size hardware encoding value of the group.
 */
#define BCMECMP_GRP_RH_SIZE_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].rh_size_enc)

/*!
 * \brief Resilient hashing group total number of members count.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Resilient hashing group total number of members count.
 */
#define BCMECMP_GRP_RH_ENTRIES_CNT_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].rh_entries_cnt)

/*!
 * \brief Pointer to resilient hashing group data.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hashing group data.
 */
#define BCMECMP_GRP_RHG_BK_PTR(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].rhg)

/*!
 * \brief Pointer to resilient hash load balanced members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced members array.
 */
#define BCMECMP_GRP_RH_ENTRIES_BK_PTR(_u, _id) \
            (((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].rhg)->rh_entries_arr)

/*!
 * \brief Pointer to resilient hash load balanced member indices array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to resilient hash load balanced member indices array.
 */
#define BCMECMP_GRP_RH_MINDEX_BK_PTR(_u, _id) \
            (((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].rhg)->rh_mindex_arr)

/*!
 * \brief Number of member table entries occupied by a RH group member.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of member table entries occupied by a RH group member.
 */
#define BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(_u, _id) \
            (((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].rhg)->rh_memb_ecnt_arr)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Pointer to group next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group next-hop members array.
 */
#define BCMECMP_GRP_NHOP_BK_PTR(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].nhop_arr)

/*!
 * \brief Pointer to group underlay next-hop members array.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Pointer to group underlay next-hop members array.
 */
#define BCMECMP_GRP_UECMP_BK_PTR(_u, _id) \
            ((BCMECMP_GRP_INFO_BK_PTR(_u))[(_id)].uecmp_arr)

/*!
 * \brief Pointer to the ECMP functions of a BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to the ECMP functions of the unit.
 */
#define BCMECMP_FNCALL_PTR(_u) (ecmp_ctrl[_u]->ecmp_drv)

/*!
 * \brief Function pointer of the specified ECMP function and BCM unit.
 *
 * \param [in] _u BCM unit number.
 * \param [in] FN ECMP function name.
 *
 * \returns Pointer to the requested ECMP function of the given BCM unit.
 */
#define BCMECMP_FNCALL(_u, FN) (BCMECMP_FNCALL_PTR(_u)->FN)

/*!
 * \brief Verify ECMP base function pointer and a specified function pointer are
 * initialized for a given BCM unit.
 *
 * \param [in] _u BCM unit number.
 * \param [in] FN ECMP function name.
 *
 * \returns TRUE if base function pointer and specified function's pointer are
 * initialized.
 * \returns FALSE if base function pointer and specified function's pointer are
 *  not initialized.
 */
#define BCMECMP_FNCALL_CHECK(_u, FN)                    \
            (BCMECMP_CTRL(_u) && BCMECMP_FNCALL_PTR(_u) \
             && BCMECMP_FNCALL(_u, FN))

/*!
 * \brief Execute an ECMP function already initialized for a BCM unit.
 *
 * \param [in] _u BCM unit number.
 * \param [in] FN ECMP function name.
 *
 * \returns Executes \c FN already initialized for the BCM unit \c _u.
 */
#define BCMECMP_FNCALL_EXEC(_u, FN) \
            (*(BCMECMP_FNCALL_PTR(_u)->FN))

/*!
 * \brief Get the reference count of an entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the reference count of the entry.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Reference count of the entry at offset index \c _i in \c _tptr
 * table.
 */
#define BCMECMP_TBL_REF_CNT(_tptr, _i)    \
            (((_tptr)->ent_arr[(_i)]).ref_cnt)

/*!
 * \brief Get the reference count of the entry from the backup array.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the reference count of the entry from the backup array.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Reference count of the entry at offset index \c _i in \c _tptr
 * table.
 */
#define BCMECMP_TBL_BK_REF_CNT(_tptr, _i)    \
            (((_tptr)->ent_bk_arr[(_i)]).ref_cnt)

/*!
 * \brief Set the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_TBL_BK_ENT_STAGED_SET(_tptr, _i)    \
            (((_tptr)->ent_bk_arr[(_i)]).flags |= (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Clear the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_TBL_BK_ENT_STAGED_CLEAR(_tptr, _i)    \
            (((_tptr)->ent_bk_arr[(_i)]).flags &= (~BCMECMP_ENTRY_STAGED))

/*!
 * \brief Get entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry staged status.
 *
 * \param [in] _tptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMECMP_TBL_BK_ENT_STAGED_GET(_tptr, _i)    \
            (((_tptr)->ent_bk_arr[(_i)]).flags & (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Increment the reference count for a range of entries in member table.
 *
 * Increment the reference count for a range of entries in ECMP Member table,
 * given the table poiner \c tbl_ptr, the start index \c start_idx of the range
 * and the count of entries \c count belonging to the group.
 *
 * \param [in] tbl_ptr Table pointer.
 * \param [in] start_idx Group start index.
 * \param [in] count Count of entries belonging to the group.
 *
 * \returns SHR_E_NONE Success.
 * \returns !SHR_E_NONE Failure.
 */
static inline int bcmecmp_tbl_ref_cnt_incr(bcmecmp_tbl_prop_t *tbl_ptr,
                                           int start_idx,
                                           uint32_t count) {
    uint32_t offset;

    if (!tbl_ptr) {
        return (SHR_E_INTERNAL);
    }

    for (offset = 0; offset < count; offset++) {
        tbl_ptr->ent_arr[(start_idx + (int)offset)].ref_cnt++;
    }
    return (SHR_E_NONE);
}

/*!
 * \brief Decrement the reference count for a range of entries.
 *
 * Decrement the reference count for a range of entries belonging to a table,
 * given the range start index \c _s_, the width of the range \c _w_ and the
 * table pointer.
 *
 * \param [in] tbl_ptr Table pointer.
 * \param [in] start_idx Group start index.
 * \param [in] count Count of entries belonging to the group.
 *
 * \returns SHR_E_NONE Success.
 * \returns !SHR_E_NONE Failure.
 */
static inline int bcmecmp_tbl_ref_cnt_decr(bcmecmp_tbl_prop_t *tbl_ptr,
                                           int start_idx,
                                           uint32_t count) {
    uint32_t offset;

    if (!tbl_ptr) {
        return (SHR_E_INTERNAL);
    }

    for (offset = 0; offset < count; offset++) {
        if ((tbl_ptr->ent_arr[(start_idx + (int)offset)].ref_cnt) > 0) {
            tbl_ptr->ent_arr[(start_idx + (int)offset)].ref_cnt--;
        }
    }
    return (SHR_E_NONE);
}

/*!
 * \brief Set/Get the entry size in words for a given ECMP register.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Hardware register name of bcmecmp_reg_prop_t type.
 *
 * \returns Given ECMP register entry size in words.
 */
#define BCMECMP_REG_ENT_SIZE(_u, _var) \
            (BCMECMP_INFO(_u)._var.ent_size)

/*!
 * \brief Set/Get ECMP config register DRD symbol identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 *
 * \returns ECMP config register DRD symbol identifier value.
 */
#define BCMECMP_REG_DRD_SID(_u, _var) \
            (BCMECMP_INFO(_u)._var.drd_sid)

/*!
 * \brief Set/Get ECMP config register LTD source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 *
 * \returns ECMP config register LTD source identifier value.
 */
#define BCMECMP_REG_LTD_SID(_u, _var) \
            (BCMECMP_INFO(_u)._var.ltd_sid)

/*!
 * \brief Set/Get device ECMP mode field DRD field identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 *
 * \returns ECMP config register LTD source identifier value.
 */
#define BCMECMP_MODE_DRD_FID(_u, _var) \
            (BCMECMP_INFO(_u)._var.mode_fid)

/*!
 * \brief Set/Get device ECMP resolution mode hardware encoding value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var ECMP config register name of bcmecmp_reg_prop_t type.
 * \param [in] _m Software ECMP mode value.
 *
 * \returns Device ECMP resolution mode hardware encoding value.
 */
#define BCMECMP_MODE_HW_VAL(_u, _var, _m) \
            (BCMECMP_INFO(_u)._var.mode_hw_val[(_m)])

/*!
 * \brief Set/Get device current ECMP resolution mode software value.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Device ECMP resolution mode software value.
 */
#define BCMECMP_MODE(_u) \
            (BCMECMP_INFO(_u).ecmp_mode)

/*!
 * \brief Device resilient hashing pseudo-random number generator seed value.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Device resilient hashing pseudo-random number generator seed value.
 */
#define BCMECMP_RH_RAND_SEED(_u) \
            (BCMECMP_INFO(_u).rh_rand_seed)

#endif /* BCMECMP_DB_INTERNAL_H */
