/*! \file rm_alpm_level1.h
 *
 * Main routines for RM ALPM Level-1
 *
 * This file contains routines manage ALPM Level-1 (TCAM) resouces,
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

#ifndef RM_ALPM_LEVEL1_H
#define RM_ALPM_LEVEL1_H

/*******************************************************************************
   Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */
/*! ALPM Level-1 key inputs */
#define KEY_INPUT_128  0
#define KEY_INPUT_64   1
#define KEY_INPUT_32   2

/*! UNPAIR packing modes */
#define UNPAIR_PKM0  0
#define UNPAIR_PKM1  1
#define UNPAIR_PKMS  2

#define L1_DB0 0
#define L1_DB1 1    /* Valid only if dual_mode, indicating Global */
#define L1_DBS 2

#define HALF_FLAG               (1 << 30)
#define RIGHT_FLAG              (1 << 29)
#define HALF_INDEX_L(i)         (HALF_FLAG | (i))
#define HALF_INDEX_R(i)         (HALF_FLAG | RIGHT_FLAG | (i))
#define FULL_INDEX(i)           (i)
#define IS_FULL_INDEX(i)        ((i != INVALID_INDEX) && !(i & HALF_FLAG))
#define IS_HALF_INDEX(i)        ((i != INVALID_INDEX) && (i & HALF_FLAG))
#define IS_HALF_R_INDEX(i)      (IS_HALF_INDEX(i) && (i & RIGHT_FLAG))
#define IS_HALF_L_INDEX(i)      (IS_HALF_INDEX(i) && !(i & RIGHT_FLAG))
#define HW_MASK                 ~(HALF_FLAG | RIGHT_FLAG)
#define HW_INDEX(i)             (i & HW_MASK)
/*******************************************************************************
  Typedefs
 */

/*!
 * \brief Level-1 statistics for unpair TCAM
 */
typedef struct unpair_stats_s {
    int unpair_used_count[KEY_TYPES][VRF_TYPES];  /*!< Used count */
    int unpair_max_count[KEY_TYPES][VRF_TYPES];   /*!< Max count */
    int unpair_half_count[KEY_TYPES][VRF_TYPES];  /*!< Half count */
} unpair_stats_t;

/*!
 * \brief Level-1 statistics for pair TCAM
 */
typedef struct pair_stats_s {
    int pair_used_count[KEY_TYPES][VRF_TYPES];  /*!< Used count */
    int pair_max_count[KEY_TYPES][VRF_TYPES];   /*!< Max count */
    int pair_half_count[KEY_TYPES][VRF_TYPES];  /*!< Half count */
} pair_stats_t;


/*!
 * \brief Level-1 prefix state for index management on pair TCAM
 */
typedef struct pair_state_s {
    int start;   /*!< Start index for this prefix length */
    int end;     /*!< End index for this prefix length */
    int prev;    /*!< Previous (Lo to Hi) prefix length with entries */
    int next;    /*!< Next (Hi to Lo) prefix length with entries */
    int vent;    /*!< Valid entries, IPv6 (dw based), IPv4 (sw based)*/
    int fent;    /*!< Free entries, IPv6 & IPv4 (both sw based) */
    int hent;    /*!< Half entries (0,1) */
} pair_state_t;

/*!
 * \brief Level-1 prefix state for index management on unpair TCAM
 */
typedef struct unpair_state_s {
    int start;   /*!< Start index for this prefix length */
    int end;     /*!< End index for this prefix length */
    int prev;    /*!< Previous (Lo to Hi) prefix length with entries */
    int next;    /*!< Next (Hi to Lo) prefix length with entries */
    int vent;    /*!< Valid entries */
    int fent;    /*!< Free entries */
    int hent;    /*!< Half entries (0,1) */
} unpair_state_t;



/*!
 * \brief Global ALPM Level-1 info
 *
 * All Level-1 related info are described in this structure
 */
typedef struct alpm_l1_info_s {
    /*!< UNPAIR TCAM size, single-view based */
    int size_unpair_2[L1_DBS][UNPAIR_PKMS];

    /*!< UNPAIR TCAM size, single-view based */
    int size_unpair[UNPAIR_PKMS];

    /*!< Start index for a given DB & upkm in unpair tcam, single-view based */
    int start_unpair[L1_DBS][UNPAIR_PKMS];

    /*!< PAIR TCAM size, single-view based */
    int size_pair_2[L1_DBS];

    /*!< Start index for a given DB in pair tcam, single-view based */
    int start_pair[L1_DBS];

    /*!< PAIR TCAM size, single-view based */
    int size_pair;

    /*!< Number of PAIR blocks, double blocks based */
    int pairs[L1_DBS];

    /*!< Number of UNPAIR blocks, double blocks based */
    int unpairs[L1_DBS][UNPAIR_PKMS];

    /*!< PAIR TCAM range1 first index */
    int range1_first[L1_DBS];

    /*!< PAIR TCAM range2 first index */
    int range2_first[L1_DBS];

    /*!< PAIR TCAM range1 last index */
    int range1_last[L1_DBS];

    /*!< PAIR TCAM range1 last prefix */
    int range1_last_pfx[L1_DBS];

    /*!< PAIR TCAM range1 last prefix (valid entries) span onto Range2. */
    int last_pfx_span[L1_DBS];

    /*!< PAIR TCAM max prefix */
    int pair_max_pfx[L1_DBS];

    /*!< PAIR TCAM max IPv4 prefix */
    int pair_max_v4_pfx[L1_DBS];

    /*!< UNPAIR TCAM max prefix */
    int unpair_max_pfx[L1_DBS][UNPAIR_PKMS];

    /*!< Packing mode for PAIR TCAM, i.e., IPv6_128 */
    int pkm_128;

    /*!< Packing mode for UNPAIR TCAM-64 i.e., IPv6_64 */
    int pkm_64;

    /*!< Packing mode for UnPAIR TCAM-32 i.e., IPv6_32 */
    int pkm_32;

    /*!< Statistics for UNPAIR TCAMs */
    unpair_stats_t    unpair_stats[UNPAIR_PKMS];

    /*!< Prefix state array for UNPAIR TCAMs */
    unpair_state_t    *unpair_state[UNPAIR_PKMS];

    /*!< Statistics for PAIR TCAM */
    pair_stats_t  pair_stats;

    /*!< Prefix state array for PAIR TCAM */
    pair_state_t  *pair_state;

    /*!< Pivot info array (half-wide pt idx as subscript ) */
    alpm_pivot_t  **pivot_info;

    /*!< TCAM depth */
    int tcam_depth;

    /*!< TCAM blocks */
    int tcam_blocks;

    /*!< Single-wide SID */
    bcmdrd_sid_t unpair_sid;

    /*!< Double-wide SID */
    bcmdrd_sid_t pair_sid;

    /*!< Dual mode (dual databases) */
    int dual_mode;

    /*!< Template wide entry */
    void *w_template[VRF_TYPES];

    /*!< Template narrow entry */
    void *n_template[VRF_TYPES];

    /*!< Template wide entry size */
    uint8_t w_size;

    /*!< Template narrow entry size */
    uint8_t n_size;
} alpm_l1_info_t;

/*******************************************************************************
   Function prototypes for unittests
 */
extern void
bcmptm_rm_alpm_l1_ut_unpair_pfx_decode(int u, int pfx,
                                       alpm_key_type_t *kt,
                                       alpm_vrf_type_t *vt,
                                       int *len,
                                       int upkm);
extern int
bcmptm_rm_alpm_l1_ut_unpair_pfx_encode(int u, int ipv6, alpm_vrf_type_t vt,
                                       int len, int upkm);

extern void
bcmptm_rm_alpm_l1_ut_pair_pfx_decode(int u, int pfx,
                                     alpm_key_type_t *kt,
                                     alpm_vrf_type_t *vt,
                                     int *len);
extern int
bcmptm_rm_alpm_l1_ut_pair_pfx_encode(int u, int ipv6, alpm_vrf_type_t vt,
                                     int len);

/*******************************************************************************
   Function prototypes
 */
/*!
 * \brief Sync up level-1 ha memory.
 *
 * \param [in] u Device unit.
 *
 * \return nothing.
 */
void
bcmptm_rm_alpm_l1_ha_sync(int u);

/*!
 * \brief Insert level-1 entry
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_insert(int u, alpm_arg_t *arg);


/*!
 * \brief Delete level-1 entry
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_delete(int u, alpm_arg_t *arg);

/*!
 * \brief Lookup (Exact match) and decode level-1 entry
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_match(int u, alpm_arg_t *arg);

/*!
 * \brief Lookup (LPM match) and decode level-1 entry
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_l1_find(int u, alpm_arg_t *arg);

/*!
 * \brief Calculate free count for given key type & vrf type
 *
 * Can be used as a reference for insert decision
 *
 * \param [in] u Device u.
 * \param [in] kt KEY type
 * \param [in] vt VRF type
 *
 * \return Free count.
 */
extern int
bcmptm_rm_alpm_l1_free_count(int u, alpm_key_type_t pkm, alpm_vrf_type_t vt);

/*!
 * \brief Cleanup ALPM level-1 module.
 *
 * \param [in] u Device u.
 * \param [in] warm Warm cleanup
 *
 * \return SHR_E_XXX.
 */
extern void
bcmptm_rm_alpm_l1_cleanup(int u, bool warm);

/*!
 * \brief Initiliaze ALPM level-1 module.
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_init(int u, bool recover);


/*!
 * \brief Get pointer of ALPM level-1 info
 *
 * \return ALPM Level-1 info ptr
 */
extern alpm_l1_info_t *
bcmptm_rm_alpm_l1_info_get(int u);


/*!
 * \brief Decode Level-1 entry (both key and data parts).
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 * \param [in] l1v Level-1 view type
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_arg_fill(int u, alpm_arg_t *arg, void *de, void *se, uint8_t l1v);

/*!
 * \brief Get level-1 sid & hw index
 *
 * \param [in] u Device u.
 * \param [in] index Entry index
 * \param [in] ipv Ip version
 * \param [out] sid Symbol ID
 * \param [out] hw_index Hardware index
 *
 * \return Level1 symbol id
 */
extern int
bcmptm_rm_alpm_l1_sid_hidx_get(int u, ln_index_t index, alpm_ip_ver_t ipv,
                               bcmdrd_sid_t *sid, uint32_t *hw_index);


extern int
bcmptm_rm_alpm_l1_recover(int u, bool full);

#endif /* RM_ALPM_LEVEL1_H */

