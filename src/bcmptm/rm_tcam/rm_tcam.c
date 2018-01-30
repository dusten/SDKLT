/*! \file rm_tcam.c
 *
 * Low Level Functions for TCAM resource manager.
 * This file contains low level functions used across all types
 * (priority or prefix based) of TCAM entry management.
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
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/chip/bcm56960_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_fp.h"
#include "rm_tcam_fp_entry_mgmt.h"
#include "rm_tcam_traverse.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <shr/shr_ha.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Typedefs
 */
/* Global data structure to hold memory offsets of all LTIDs on
 * all Logical device ids
 */
typedef struct req_info_actual_s {
    uint32_t **ekw;
    uint32_t **edw;
} req_info_actual_t;

typedef struct req_info_updated_s {
    uint32_t updated_ekw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t updated_edw[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t *ekw[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    uint32_t *edw[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
} req_info_updated_t;


/*******************************************************************************
 * Global variables
 */
req_info_actual_t actual_req_info[BCMDRD_CONFIG_MAX_UNITS];

req_info_updated_t updated_req_info[BCMDRD_CONFIG_MAX_UNITS];

/* To hold previously looked up index */
bcmptm_rm_tcam_same_key_data_t same_key_info[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief LT index traverse callback function
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table id
 * \param [in] user_data memory address to data provided by caller to
 *             bcmptm_ltindex_traverse.
 *
 * \retval SHR_E_NONE Success
 */

typedef int (*bcmptm_ltindex_traverse_cb)(int unit, int lt_index,
                                          void *user_data);

/* Function pointers to different operations of prority based TCAMs */
bcmptm_rm_tcam_funct_t prio_funct_ptr =
    {
         bcmptm_rm_tcam_prio_entry_insert,
         bcmptm_rm_tcam_prio_entry_lookup,
         bcmptm_rm_tcam_prio_entry_delete,
         bcmptm_rm_tcam_prio_entry_move,
         bcmptm_rm_tcam_traverse_info_entry_get_first,
         bcmptm_rm_tcam_traverse_info_entry_get_next,
    };

bcmptm_rm_tcam_funct_t prio_only_funct_ptr =
    {
         bcmptm_rm_tcam_prio_only_entry_insert,
         bcmptm_rm_tcam_prio_only_entry_lookup,
         bcmptm_rm_tcam_prio_only_entry_delete,
         bcmptm_rm_tcam_prio_only_entry_move,
         bcmptm_rm_tcam_traverse_info_entry_get_first,
         bcmptm_rm_tcam_traverse_info_entry_get_next,
    };
bcmptm_rm_tcam_funct_t fp_funct_ptr =
    {
         bcmptm_rm_tcam_fp_entry_insert,
         bcmptm_rm_tcam_fp_entry_lookup,
         bcmptm_rm_tcam_fp_entry_delete,
         bcmptm_rm_tcam_fp_entry_move,
         NULL,
         NULL,
    };


/*******************************************************************************
 * Public Functions
 */
/* To extract bits from a particular position to a particular length */
int
bcmptm_rm_tcam_data_get(int unit,
                        uint32_t *in_data,
                        uint16_t start_pos,
                        uint16_t len,
                        uint32_t *out_data)
{
    int current_word, bit_pos, result_word;
    int i;
    int max_size = BCMPTM_MAX_PT_FIELD_WORDS;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in_data, SHR_E_PARAM);
    SHR_NULL_CHECK(out_data, SHR_E_PARAM);

    /* Input parameters check. */
    if ((NULL == in_data) || (NULL == out_data) ||
        ((start_pos + len) > (BCMPTM_MAX_PT_FIELD_WORDS * 32))) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    current_word = start_pos / 32;
    bit_pos = start_pos - current_word * 32;

    sal_memset(out_data, 0, max_size);

    for (i = 0, result_word = 0; i < len;
         i += 32, current_word++, result_word++) {

        out_data[result_word] = in_data[current_word] >> bit_pos;

        if ((bit_pos != 0) && ((bit_pos + (len - i)) > 32)) {
            out_data[result_word] |= (in_data[current_word + 1]
                                     << (32 - bit_pos));
        }
    }
    /* This check may be redundant as -1 = 0xffffffff */
    if (i != len) {
        i -= 32;
        out_data[result_word - 1] &= ((1 << (len - i)) - 1);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Fetch the hw entry information of an LT based on LT entries mode. */
int
bcmptm_rm_tcam_lt_hw_entry_info_get(int unit,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  void *attrs,
                  const bcmptm_rm_tcam_hw_entry_info_t **hw_entry_info)
{
    bcmptm_rm_tcam_group_mode_t mode;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_entry_info, SHR_E_PARAM);

    if ((attrs != NULL) && (ltid_info->pt_type == LT_PT_TYPE_FP)) {
       entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;
       mode = entry_attrs->group_mode;
    } else {
       mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    *hw_entry_info = &(ltid_info->hw_entry_info[mode]);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Fetch the number of physical indexs needed to write the entry
 * \n for the given Logical Table id.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] num physical indexes needed
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
int
bcmptm_rm_tcam_num_index_required_get(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t *num_index_req)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(num_index_req, SHR_E_PARAM);

    *num_index_req = ltid_info->hw_entry_info->num_key_rows;

exit:

    SHR_FUNC_EXIT();
}

/*!
 *  \brief TCAM LTID can be depth expanded and every TCAM index maps
 *  \n to a physical slice and index in that slice. This routine is to
 *  \n fetch the slice roe and index in that slice row for a given TCAM index.
 *
 *  \param [in] unit Logical device id
 *  \param [in] ltid Logical Table enum that is accessing the table.
 *  \param [in] ltid_info LRD information for the ltid.
 *  \param [in] tindex TCAM index
 *  \param [out] sindex Slice index corresponding to tindex
 *  \param [out] slice_row Slice row corresponds to tindex
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Inavlid parameters
 */
int
bcmptm_rm_tcam_tindex_to_sindex(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t tindex,
                                uint32_t *sindex, uint8_t *slice_row)
{
    uint8_t s_row = 0;
    uint16_t min_idx = 0;
    uint16_t max_idx = 0;
    uint16_t num_entries = 0;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(sindex, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_row, SHR_E_PARAM);

    *sindex = tindex;
    for (s_row = 0; s_row < ltid_info->hw_entry_info->num_depth_inst;
         s_row++) {

          sid = ltid_info->hw_entry_info->sid[s_row][0];
          min_idx = bcmdrd_pt_index_min(unit, sid);
          max_idx = bcmdrd_pt_index_max(unit, sid);
          num_entries = (max_idx - min_idx + 1);

         if ((*sindex) > num_entries) {
             (*sindex) -= num_entries;
         } else {
             *slice_row = s_row;
             break;
         }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Fetch the total number of entries in a TCAM
 */
int
bcmptm_rm_tcam_depth_get(int unit, bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         uint32_t *num_entries)
{
    uint8_t s_row = 0;
    uint16_t min_idx = 0;
    uint16_t max_idx = 0;
    bcmdrd_sid_t sid;


    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(num_entries, SHR_E_PARAM);

    *num_entries = 0;
    for (s_row = 0; s_row < ltid_info->hw_entry_info->num_depth_inst;
         s_row++) {
          sid = ltid_info->hw_entry_info->sid[s_row][0];
          min_idx = bcmdrd_pt_index_min(unit, sid);
          max_idx = bcmdrd_pt_index_max(unit, sid);
          *num_entries += (max_idx - min_idx + 1);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Read the entry words from hardware for the given TCAM index and TCAM LTID.
 */
int
bcmptm_rm_tcam_hw_read(int unit,
                       bcmltd_sid_t ltid,
                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                       uint32_t rm_flags,
                       uint32_t req_flags,
                       uint32_t index,
                       uint32_t **entry_words,
                       size_t entry_words_size,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       void *ovrr_info,
                       void *attrs,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags,
                       bcmdrd_sid_t *rsp_entry_sid,
                       int *hw_index,
                       int *rsp_index_count)
{
    int rv = SHR_E_NONE;
    uint32_t sindex = -1;
    uint8_t part;
    uint8_t slice_row = 0;

    bcmdrd_sid_t sid;
    bool rsp_entry_cache_vbit = 0;
    uint16_t rsp_dfield_format_id;
    int num_parts = -1;
    uint8_t mode = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters check. */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);

    if ((attrs != NULL) && (ltid_info->pt_type == LT_PT_TYPE_FP)) {
       entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;
       mode = entry_attrs->group_mode;
    } else {
       mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
        num_parts = ltid_info->hw_entry_info[mode].num_data_rows;
    } else {
        /* This includes AGGR_VIEW and TCAM view */
        num_parts = ltid_info->hw_entry_info[mode].num_key_rows;
    }

    if (rsp_index_count) {
        *rsp_index_count = num_parts;
    }

    for (part = 0; part < num_parts; part++) {

         if (ltid_info->pt_type != LT_PT_TYPE_FP) {
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_tindex_to_sindex(unit, ltid,
                                                             ltid_info,
                                                             index,
                                                             &sindex,
                                                             &slice_row));
             /* WIDTH expansion use same index for other parts */
             if (ltid_info->rm_more_info->flags &
                 BCMPTM_RM_TCAM_F_WIDTH_EXPANSION) {
                 pt_dyn_info->index = sindex;
             } else {
                 pt_dyn_info->index = sindex + part;
             }
         } else {
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                                ltid, ltid_info,
                                                                attrs,
                                                                rm_flags,
                                                                index, part,
                                                                &slice_row,
                                                                &sindex));
             pt_dyn_info->index = sindex;
         }


         if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
             sid = ltid_info->hw_entry_info[mode].sid_data_only[slice_row][part];
         } else {
             sid = ltid_info->hw_entry_info[mode].sid[slice_row][part];
         }

         if (part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS) {
             rsp_entry_sid[part] = sid;

             if (hw_index) {
                 hw_index[part] = pt_dyn_info->index;
             }
         }

         /* Insufficient buffer */
         if (entry_words_size < bcmdrd_pt_entry_wsize(unit, sid)) {
             SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
         }

         rv = bcmptm_cmdproc_read(unit,
                                  req_flags,
                                  0, 
                                  sid,
                                  pt_dyn_info,
                                  ovrr_info,
                                  NULL, /* misc_info */
                                  entry_words_size,
                                  entry_words[part],
                                  &rsp_entry_cache_vbit,
                                  rsp_ltid,
                                  &rsp_dfield_format_id,
                                  rsp_flags);
         SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Write the entry words to hardware for the given TCAM index and TCAM LTID.
 */
int
bcmptm_rm_tcam_hw_write(int unit,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        uint32_t rm_flags,
                        uint32_t req_flags,
                        uint32_t index,
                        uint32_t **entry_words,
                        bcmbd_pt_dyn_info_t *pt_dyn_info,
                        void *ovrr_info,
                        void *attrs,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags,
                        bcmdrd_sid_t *rsp_entry_sid)

{
    int rv = SHR_E_NONE;
    uint32_t sindex = -1;
    uint8_t part;
    uint8_t slice_row = 0;

    bcmdrd_sid_t sid;
    int num_parts = -1;
    uint8_t mode = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the entry of the function. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters check. */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);

    if ((attrs != NULL) && (ltid_info->pt_type == LT_PT_TYPE_FP)) {
       entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;
       mode = entry_attrs->group_mode;
    } else {
       mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
        num_parts = ltid_info->hw_entry_info[mode].num_data_rows;
    } else {
        /* This includes AGGR_VIEW and TCAM view */
        num_parts = ltid_info->hw_entry_info[mode].num_key_rows;
    }


    for (part = 0; part < num_parts; part++) {

         if (ltid_info->pt_type != LT_PT_TYPE_FP) {
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_tindex_to_sindex(unit, ltid,
                                                             ltid_info,
                                                             index,
                                                             &sindex,
                                                             &slice_row));
             /* WIDTH expansion use same index for other parts */
             if (ltid_info->rm_more_info->flags &
                 BCMPTM_RM_TCAM_F_WIDTH_EXPANSION) {
                 pt_dyn_info->index = sindex;
             } else {
                 pt_dyn_info->index = sindex + part;
             }
         } else {
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_tindex_to_sindex(unit,
                                                                ltid, ltid_info,
                                                                attrs,
                                                                rm_flags,
                                                                index, part,
                                                                &slice_row,
                                                                &sindex));
             pt_dyn_info->index = sindex;
         }


         if (rm_flags == BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW) {
             sid = ltid_info->hw_entry_info[mode].sid_data_only[slice_row][part];
         } else {
             sid = ltid_info->hw_entry_info[mode].sid[slice_row][part];
         }

         rv = bcmptm_cmdproc_write(unit, req_flags,
                                    0, 
                                    sid, pt_dyn_info, ovrr_info,
                                    NULL, /* misc_info */
                                    entry_words[part], 1, 1, 1,
                                    BCMPTM_RM_OP_NORMAL, NULL,
                                    ltid, 0, rsp_flags);
         SHR_IF_ERR_VERBOSE_EXIT(rv);
         *rsp_entry_sid = ltid_info->hw_entry_info[mode].sid[slice_row][part];

    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *  Move the range of TCAM entries one index up or down.
 */
int
bcmptm_rm_tcam_entry_move_range(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint8_t partition,
                                uint32_t idx_start,
                                uint32_t idx_end,
                                void *attrs,
                                bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    int rv = SHR_E_NONE;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    /* Shift entries in down ward direction(i.e. one index up). */
    if (idx_end > idx_start) {

        for (idx = (idx_end - 1); (idx >= (int)idx_start)
             && (idx < (int)idx_end); idx--) {
            rv = bcmptm_rm_tcam_entry_move(unit, ltid, ltid_info, partition,
                                           idx, idx + 1, attrs, pt_dyn_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    /* Shift entries in upward direction(i.e. one index down). */
    if (idx_end < idx_start) {

        for (idx = idx_end + 1; idx <= (int)idx_start; idx++) {
            rv = bcmptm_rm_tcam_entry_move(unit, ltid, ltid_info, partition,
                                           idx, idx - 1, attrs, pt_dyn_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Fetch the pointer that holds function pointers to basic
 * TCAM operations of a TCAM.
 */
int
bcmptm_rm_tcam_funct_get(int unit, bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info, void **funct)
{


    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(funct, SHR_E_PARAM);

    switch (ltid_info->pt_type) {
        case LT_PT_TYPE_TCAM_PRI_ONLY:
              *funct = &prio_only_funct_ptr;
             break;
        case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
             *funct = &prio_funct_ptr;
             break;
        case LT_PT_TYPE_FP:
             *funct = &fp_funct_ptr;
             break;
        default:
             break;
    }

    if ((*funct) == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * TCAM resource manager supports three basic operations(lookup, insert and
 * delete). This function is to request one of the opeartions for any TCAM.
 * This will dispatch the request to corresponding operations function pointer
 * of the TCAM.
 *
 * Mostly all priority based TCAMs have one set of function pointers.
 * Some TCAMs may have its own set of function pointers.
 */

int
bcmptm_rm_tcam_req(int unit,
                   uint32_t req_flags,
                   uint32_t cseq_id, 
                   bcmltd_sid_t ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_tcam_req_t *req_info,
                   bcmptm_rm_tcam_rsp_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags) {

    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_funct_t *funct = NULL;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bcmptm_rm_tcam_entry_attrs_t entry_attrs;
    bcmptm_rm_tcam_trans_info_t *ltid_trans_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info_global = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_cmdproc_ptrm_info_get(unit, ltid, 0, 0,
                                                 ((void *)&ltid_info),
                                                 NULL, NULL));

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_funct_get(unit, ltid, &ltid_info,
                                             (void *)&funct));
    /* set the pipe_id in entry_attrs structure */
    if (req_info->entry_attrs == NULL) {
        /* For global mode, use pipe_id 0 */
        if (pt_dyn_info->tbl_inst == -1) {
            entry_attrs.pipe_id = 0;
        } else {
            entry_attrs.pipe_id = pt_dyn_info->tbl_inst;
        }
        req_info->entry_attrs = (void *) &entry_attrs;
    }
    /* Dispatch the operation corresponding function pointer. */
    switch (req_op) {
        case BCMPTM_OP_LOOKUP:
            if (funct->entry_lookup != NULL) {
                return funct->entry_lookup(unit,
                                           req_flags, ltid,
                                           &ltid_info,
                                           pt_dyn_info,
                                           req_info, rsp_info,
                                           rsp_ltid, rsp_flags, NULL);
            }
            break;
        case BCMPTM_OP_INSERT:
            if (funct->entry_insert != NULL) {
                rv = funct->entry_insert(unit,
                                           req_flags, ltid,
                                           &ltid_info,
                                           pt_dyn_info,
                                           req_info, rsp_info,
                                           rsp_ltid, rsp_flags);
            }
            break;
        case BCMPTM_OP_DELETE:
            if (funct->entry_delete != NULL) {
                rv = funct->entry_delete(unit,
                                           req_flags, ltid,
                                           &ltid_info,
                                           pt_dyn_info,
                                           req_info, rsp_info,
                                           rsp_ltid, rsp_flags);
            }
            break;
        case BCMPTM_OP_GET_FIRST:
            if (funct->entry_traverse_get_first != NULL) {
                return funct->entry_traverse_get_first(unit,
                                                       req_flags, ltid,
                                                       &ltid_info,
                                                       pt_dyn_info,
                                                       req_info, rsp_info,
                                                       rsp_ltid, rsp_flags);
            }
            break;
        case BCMPTM_OP_GET_NEXT:
            if (funct->entry_traverse_get_next != NULL) {
                return funct->entry_traverse_get_next(unit,
                                                      req_flags, ltid,
                                                      &ltid_info,
                                                      pt_dyn_info,
                                                      req_info, rsp_info,
                                                      rsp_ltid, rsp_flags);
            }
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (req_op == BCMPTM_OP_INSERT ||
        req_op == BCMPTM_OP_DELETE) {
        /* Fetch the transaction state and update it*/
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_info_get(unit,
                                       ltid, &ltid_info,
                                       (void *)&ltid_trans_info));
        if (ltid_trans_info->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
            ltid_trans_info->trans_state = BCMPTM_RM_TCAM_STATE_UC_A;
            trans_info_global = (bcmptm_rm_tcam_trans_info_t *)RM_TCAM_PTR(unit);
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                trans_info_global->trans_state = BCMPTM_RM_TCAM_STATE_UC_A;
            }
        }
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Move the TCAM entry from one TCAM index to the other TCAM index.
 * This function will disaptch the move request to TCAM specific
 * function pointer. Mostly all priority based TCAMs have one set of function
 * pointers.
 * Some TCAMs may have its own set of function pointers.
 */
int
bcmptm_rm_tcam_entry_move(int unit,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          uint8_t partition,
                          uint32_t from_idx, uint32_t to_idx,
                          void *attrs,
                          bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    bcmptm_rm_tcam_funct_t *funct = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_funct_get(unit, ltid, ltid_info,
                                             (void *)&funct));

    if (funct->entry_move != NULL) {
        /* Call the low level function to perform ltid specific lookup. */
        SHR_IF_ERR_VERBOSE_EXIT(funct->entry_move(unit, ltid, ltid_info,
                                          partition, -1,
                                          from_idx, to_idx,
                                          attrs,
                                          pt_dyn_info));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

