/*! \file rm_alpm_leveln.h
 *
 * Main routines for RM ALPM Level-N
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

#ifndef RM_ALPM_LEVELN_H
#define RM_ALPM_LEVELN_H


/*******************************************************************************
   Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */

#define ALPM_MAX_BANKS      8

/* LN INDEX format: [RBKT | SUB_BKT | ENT_OFS] */
#define ENT_OFS_BITS    5
#define ENT_OFS_MASK    0x1F
#define SUB_BKT_BITS    2
#define SUB_BKT_MASK    0x3
#define SUB_BKT_SHIFT   ENT_OFS_BITS
#define RBKT_SHIFT      7
#define ENT_OFS_FROM_IDX(index)   ((index) & ENT_OFS_MASK)
#define SUB_BKT_FROM_IDX(index)   (((index) >> SUB_BKT_SHIFT) & SUB_BKT_MASK)
#define RBKT_FROM_INDEX(index)    ((index) >> RBKT_SHIFT)
#define ALPM_INDEX_ENCODE_VER1(_rbkt, _ofs)   \
    (((_rbkt) << RBKT_SHIFT) | (_ofs))
#define ALPM_INDEX_ENCODE_VER0(_rbkt, _sub, _ofs)   \
    (((_rbkt) << RBKT_SHIFT) | ((_sub) << SUB_BKT_SHIFT) | (_ofs))

/*******************************************************************************
  Typedefs
 */
/*!
 * \brief Level statistics
 */
typedef struct lvl_stat_s {
    uint32_t c_merge;
    uint32_t c_expand;
    uint32_t c_split;
    uint32_t c_shuffle;
} lvl_stat_t;

/*!
 * \brief ALPM Level Handle
 *
 * ALPM level handle contains the info describing Level-N
 */
typedef struct lvl_hdl_s {
    /*!< Level index */
    uint8_t lvl_idx;

    /*!< Level stats */
    lvl_stat_t lvl_stats;

    /*!< Level bucket handle */
    void *bkt_hdl; /* bkt_hdl_t */

    
} lvl_hdl_t;

/*!
 * \brief ALPM Level N info
 *
 * ALPM level-n info contains the info describing level-n sub-module
 */
typedef struct alpm_ln_info_s {
    /*!< Level handles */
    lvl_hdl_t *lvl_hdl[DBS][LEVELS];

    /*!< SID for LN level */
    bcmdrd_sid_t ln_sid[LEVELS];

    /*!< SID for LN level hit table */
    bcmdrd_sid_t ln_hit_sid[LEVELS];

    /*!< ALPM bucket mgmt version */
    uint8_t alpm_ver;
} alpm_ln_info_t;
/*******************************************************************************
   Function prototypes
 */
/*!
 * \brief Get alpm level-n info
 *
 * \param [in] u Device u.
 * \param [out] linfo Level-n info
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND.
 */
extern int
bcmptm_rm_alpm_ln_info_get(int u, alpm_ln_info_t *linfo);

/*!
 * \brief Get alpm level-n sid
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 *
 * \return Symbol id
 */
extern bcmdrd_sid_t
bcmptm_rm_alpm_ln_sid_get(int u, int ln);

/*!
 * \brief Get alpm level handle for level-n
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] lvl_hdl Level handle
 *
 * \return SHR_E_NONE, SHR_E_NOT_FOUND.
 */
extern int
bcmptm_rm_alpm_lvl_hdl_get(int u, int db, int ln,
                              lvl_hdl_t **lvl_hdl);


/*!
 * \brief ALPM find operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_find(int u, alpm_arg_t *arg);

/*!
 * \brief ALPM match operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_match(int u, alpm_arg_t *arg);

/*!
 * \brief ALPM insert operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_insert(int u, alpm_arg_t *arg);


/*!
 * \brief ALPM delete operation in multi-level mode
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_ln_delete(int u, alpm_arg_t *arg);

/*!
 * \brief Cleanup ALPM level-n module.
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX.
 */
extern void
bcmptm_rm_alpm_ln_cleanup(int u);

/*!
 * \brief Initiliaze ALPM level-n module.
 *
 * \param [in] u Device u.
 * \param [in] recover True if it is warm boot, ISSU, crash or abort.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_init(int u, bool recover);

extern int
bcmptm_rm_alpm_split_pivot_delete(int u, int db, int ln, alpm_arg_t *split_arg,
                               bkt_split_aux_t *split_aux);
extern int
bcmptm_rm_alpm_split_pivot_add(int u, int db, int ln, alpm_arg_t *split_arg,
                               alpm_pivot_t **split_pivot,
                               bkt_split_aux_t *split_aux);
extern int
bcmptm_rm_alpm_pivot_update(int u, int db, int ln, alpm_pivot_t *pivot);


/*!
 * \brief Read dip/sip entry from cmdproc for a given index.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [out] de dip entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_read_ln_raw(int u, int db, int ln, int hw_idx, uint32_t *de);

/*!
 * \brief Read hit entry for given index.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [out] e Hit entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_read_ln_hit(int u, int db, int ln, int hw_idx, uint32_t *e);

/*!
 * \brief Write hit entry for a given index.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [in] de dip entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_write_ln_hit(int u, int db, int ln, int hw_idx, uint32_t *e);

/*!
 * \brief Write dip/sip entry to cmdproc for a given index.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 * \param [in] de dip entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_write_ln_raw(int u, int db, int ln, int hw_idx, uint32_t *de);
/*!
 * \brief Clear dip/sip entry to cmdproc for a given index.
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] hw_idx Pt table index
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_clear_ln_raw(int u, int db, int ln, int hw_idx);

extern int
bcmptm_rm_alpm_pivot_update_with_arg(int u, int db, int ln,
                      alpm_pivot_t *pivot, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_pivot_delete(int u, int ln, alpm_arg_t *arg);
extern int
bcmptm_rm_alpm_arg_from_pivot(int u, int db, int ln, alpm_pivot_t *pivot,
                              bool key_only, alpm_arg_t *arg);
extern int
bcmptm_rm_alpm_bucket_clean(int u, int db, int ln, alpm_pivot_t *pivot);

extern int
bcmptm_rm_alpm_ln_pivot_recover(int u, int ln, alpm_arg_t *arg);
extern int
bcmptm_rm_alpm_ln_recover(int u, int ln, alpm_arg_t *arg);
extern int
bcmptm_rm_alpm_ln_recover_done(int u, bool full);

#endif /* RM_ALPM_LEVELN_H */
