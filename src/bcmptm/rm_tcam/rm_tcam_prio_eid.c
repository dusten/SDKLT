/*! \file rm_tcam_prio_eid.c
 *
 * Low Level Functions for Prioirty based TCAMs
 * This file contains low level functions for priority based TCAMs
 * to do opertaions like TCAM entry Insert/Lookup/Delete/Move.
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
#include "rm_tcam.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_traverse.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include "rm_tcam_fp_utils.h"
#include "rm_tcam_fp.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private variables
 */
static uint32_t entry_words[BCMDRD_CONFIG_MAX_UNITS]
                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS][BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t *entry_words_row[BCMDRD_CONFIG_MAX_UNITS]
                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];


/*******************************************************************************
 * Private variables
 */
/*
 * Get the pointer to tcam information of given TCAM LTID, pipe id.
 */
static int
rm_tcam_prio_eid_tcam_info_get(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        int pipe_id,
                        bcmptm_rm_tcam_prio_eid_info_t **tcam_info)
{
    void *tcam_ptr = NULL;
    uint32_t num_entries = 0;
    int count = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_info_get(unit, ltid, ltid_info,
                                                    &tcam_ptr));

    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE
        || pipe_id == 0) {
        *tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *) ((uint8_t*)tcam_ptr +
                                        sizeof(bcmptm_rm_tcam_trans_info_t));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info,
                                                   &num_entries));

    tcam_ptr = ((uint8_t *)tcam_ptr + sizeof(bcmptm_rm_tcam_trans_info_t));

    while (count != pipe_id) {
        /* add offset for next pipe */
        tcam_ptr = ((uint8_t *)tcam_ptr
                    + ((bcmptm_rm_tcam_prio_eid_info_t *)tcam_ptr)->next_pipe_tcam_info_offset);
        count++;
    }
    *tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)((uint8_t *)tcam_ptr);

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_entry_total_count_get(int unit, bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         void *entry_attrs,
                                         uint32_t *total_count)
{

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_total_count_get
                                                             (unit,
                                                              ltid, ltid_info,
                                                              entry_attrs,
                                                              total_count));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit,
                                                 ltid, ltid_info, total_count));
    }

exit:
    SHR_FUNC_EXIT();

}

static int
rm_tcam_prio_find_loops(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info, void *attrs)
{
    int rv = SHR_E_NONE;
    uint32_t idx = 0, idx2 = 0;
    int *entry_hash = NULL;
    int offset1 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int first_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int offset2 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t num_offsets = 0;
    uint32_t num_entries = 0;
    uint32_t entry_hash_size = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    rv = bcmptm_rm_tcam_prio_entry_total_count_get(unit, ltid, ltid_info,
                                                   attrs,
                                                   &num_entries);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                            attrs, &entry_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_entry_hash_get(unit, ltid, ltid_info,
                                            attrs, &entry_hash);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_entry_hash_size_get(unit, ltid, ltid_info,
                                                 attrs,
                                                 &entry_hash_size);
    SHR_IF_ERR_VERBOSE_EXIT(rv);


    for (idx = 0; idx < entry_hash_size; idx++) {

         first_offset = (int)entry_hash[idx];

         if (first_offset < (int)num_entries &&
             first_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
             offset1 = (int)(entry_info[first_offset].offset);
         }

         num_offsets = 0;
         while (offset1 != BCMPTM_RM_TCAM_OFFSET_INVALID) {

             SHR_IF_ERR_VERBOSE_EXIT(
                 (offset1 >= (int)num_entries) ? SHR_E_INTERNAL : SHR_E_NONE);

             offset2 = (int)(entry_info[first_offset].offset);

             for (idx2 = 0; idx2 < num_offsets; idx2++) {

                 SHR_IF_ERR_VERBOSE_EXIT(
                     (offset2 >= (int)num_entries)
                              ? SHR_E_INTERNAL : SHR_E_NONE);

                 if (offset1 == offset2) {
                      SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                 }

                 offset2 = (int)(entry_info[offset2].offset);
             }

             num_offsets++;
             offset1 = (int)(entry_info[offset1].offset);
         }

         offset1 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_entry_flags_get(int unit, bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   void *attrs,
                                   uint32_t *flags)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_flags_get(unit, ltid,
                                                               ltid_info,
                                                               attrs, flags));
    } else {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

        /* Fetch the TCAM information for the given ltid, pipe_id. */
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_eid_tcam_info_get(unit, ltid,
                                            ltid_info,
                                            entry_attrs->pipe_id,
                                            &tcam_info));
        *flags |= tcam_info->flags;
    }

exit:

    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_prio_entry_free_count_incr(int unit, bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          uint32_t index,
                                          void *attrs)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_incr(unit, ltid,
                                                                     ltid_info,
                                                                     index,
                                                                     attrs));
    } else {

        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

        /* Fetch the TCAM information for the given ltid, pipe_id. */
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_eid_tcam_info_get(unit, ltid,
                                            ltid_info,
                                            entry_attrs->pipe_id,
                                            &tcam_info));
        tcam_info->free_entries += 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_entry_free_count_decr(int unit, bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          uint32_t index,
                                          void *attrs)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_decr(unit, ltid,
                                                                     ltid_info,
                                                                     index,
                                                                     attrs));
    } else {

        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

        /* Fetch the TCAM information for the given ltid, pipe_id. */
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_eid_tcam_info_get(unit, ltid,
                                            ltid_info,
                                            entry_attrs->pipe_id,
                                            &tcam_info));
        tcam_info->free_entries -=1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_entry_pipe_count_get(int unit, bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         void *entry_attrs,
                                         uint8_t *pipe_count)
{

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_pipe_count_get
                                                               (unit,
                                                                ltid, ltid_info,
                                                                entry_attrs,
                                                                pipe_count));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Fetch the address to TCAM entry information of the given LTID.
 * \n This address points to address of zeroth TCAM index entry information.
 * \n To get the address to TCAM entry information of any specific TCAM index,
 * \n increment this pointer by those many TCAM indexes as memory created to
 * \n hold TCAM entry information of all TCAM indexes is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id
 * \param [out] entry_info double pointer to hold pointer to
 * \n           start address of TCAM entry information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
int
bcmptm_rm_tcam_prio_entry_info_get(int unit, bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   void *attrs,
                                   bcmptm_rm_tcam_prio_entry_info_t **entry_info)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_info_get(unit, ltid,
                                                          ltid_info,
                                                          attrs, entry_info));
    } else {

        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

        /* Fetch the TCAM information for the given ltid, pipe_id. */
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_eid_tcam_info_get(unit, ltid,
                                            ltid_info,
                                            entry_attrs->pipe_id,
                                            &tcam_info));

        *entry_info = NULL;

        offset = tcam_info->entry_info_offset;

        *entry_info = (bcmptm_rm_tcam_prio_entry_info_t*)
                       ((uint8_t *)tcam_info + offset);

    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Fetch the address to TCAM entry hash of the given Logical Table id.
 * \n This address points to the address of hash zero. To get the address to
 * \n any other valid hash, increment this start pointer by hash times
      as memory
 * \n created to hold the hash information of all valid hash values
      is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id
 * \param [out] entry_hash Double pointer to hold pointer to start address
 * \n           of TCAM hash information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
int
bcmptm_rm_tcam_prio_entry_hash_get(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *attrs,
                                int **entry_hash)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_hash, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_hash_get(unit, ltid,
                                                              ltid_info,
                                                              attrs, entry_hash));

    } else {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

        /* Fetch the TCAM information for the given ltid, pipe_id. */
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_eid_tcam_info_get(unit, ltid,
                                            ltid_info,
                                            entry_attrs->pipe_id,
                                            &tcam_info));
        *entry_hash = NULL;

        offset = tcam_info->entry_hash_offset;

        *entry_hash = (int*)((uint8_t *)tcam_info + offset);

    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_entry_free_count_get(int unit, bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   void *attrs,
                                   uint32_t *free_entries)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(attrs, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_free_count_get(unit, ltid,
                                                                 ltid_info,
                                                                 attrs,
                                                                 free_entries));
    } else {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)attrs;

        /* Fetch the TCAM information for the given ltid, pipe_id. */
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_eid_tcam_info_get(unit, ltid,
                                            ltid_info,
                                            entry_attrs->pipe_id,
                                            &tcam_info));

        *free_entries |= tcam_info->free_entries;
    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_entry_valid_count_get(int unit, bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   void *attrs,
                                   uint32_t *num_entries)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_valid_count_get(unit, ltid,
                                                                 ltid_info,
                                                                 attrs,
                                                                 num_entries));
    } else {

        /* Fetch the TCAM information for the given ltid. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_info_get(unit, ltid,
                                                ltid_info, (void *)&tcam_info));
        *num_entries |= tcam_info->num_entries;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_entry_hash_size_get(int unit, bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   void *attrs,
                                   uint32_t *entry_hash_size)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);


    if (ltid_info->pt_type == LT_PT_TYPE_FP) {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_fp_entry_hash_size_get(unit, ltid,
                                                              ltid_info,
                                                              attrs,
                                                              entry_hash_size));
    } else {

        /* Fetch the TCAM information for the given ltid. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_info_get(unit, ltid, ltid_info,
                                                (void *)&tcam_info));
        *entry_hash_size |= tcam_info->entry_hash_size;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Fetch the attributes of the entry ->
 * entry id, priority and index
 */

int
bcmptm_rm_tcam_prio_entry_attr_get(int unit, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint16_t entry_id,
                                   void *attrs,
                                   uint16_t *entry_pri, uint32_t *entry_index)
{
    int *entry_hash = NULL;
    uint16_t hash_val;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    int offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t entry_hash_size = 1;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check */
    SHR_NULL_CHECK(entry_pri, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_size_get(
                                            unit, ltid, ltid_info,
                                            attrs, &entry_hash_size));

    /* Check if entry id  exists or not. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                                       attrs,
                                                       &entry_info));

    hash_val = (entry_id) % (entry_hash_size);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_get(unit, ltid, ltid_info,
                                                       attrs,
                                                       &entry_hash));

    offset = entry_hash[hash_val];

    while (offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        if (entry_info[offset].entry_id == entry_id) {
            break;
        }
        offset = entry_info[offset].offset;
    }

    if (offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *entry_pri = entry_info[offset].entry_pri;
    *entry_index = offset;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Move TCAM entry from one index to the other index.
 * Both HW and SW state will be updated.
 */
int
bcmptm_rm_tcam_prio_entry_move(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               uint8_t partition,
                               uint32_t prefix,
                               uint32_t from_idx,
                               uint32_t to_idx,
                               void *attrs,
                               bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    bcmltd_sid_t rsp_ltid = -1;
    int *entry_hash = NULL;
    int orig_tbl_inst = -1;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint8_t part = 0;
    uint16_t hash_val;
    uint32_t rm_flags = 0;
    uint32_t rsp_flags = 0;
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {0};
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    uint32_t tcam_flags = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    uint32_t entry_hash_size = 1;
    uint8_t pipe = 0;
    uint8_t pipe_count = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given LTID. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_flags_get(unit, ltid, ltid_info,
                                                        attrs,
                                                        &tcam_flags));

    /* Fetch the TCAM entry information for the givem LTID. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                                       attrs,
                                                       &entry_info));
    /* If the entry is empty no need to move. */
    if (entry_info[from_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
         sal_memset(entry_words[unit][part], 0,
                    sizeof(entry_words[unit][part]));
         entry_words_row[unit][part] = entry_words[unit][part];
    }

    if (entry_info[from_idx].global_to_all_pipes == 0) {
        /* global view only */
        pipe_count = 1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_pipe_count_get(unit, ltid,
                                                                 ltid_info,
                                                                 attrs,
                                                                 &pipe_count));
    }

    orig_tbl_inst = pt_dyn_info->tbl_inst;
    for (pipe = 0; pipe < pipe_count; pipe++) {

        if (entry_info[from_idx].global_to_all_pipes == TRUE) {
            pt_dyn_info->tbl_inst = pipe;
        }

        if (tcam_flags &  BCMPTM_RM_TCAM_F_AGGR_VIEW) {

            rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;

            /* Read the entry words from from_idx. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_read(unit, ltid, ltid_info,
                                                   rm_flags, 0, from_idx,
                                                   entry_words_row[unit],
                                                   entry_words_size,
                                                   pt_dyn_info,
                                                   NULL, attrs,
                                                   &rsp_ltid, &rsp_flags,
                                                   rsp_entry_sid,
                                                   NULL,
                                                   NULL));

            /* Write the entry words to to_idx. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                                    rm_flags, 0, to_idx,
                                                    entry_words_row[unit],
                                                    pt_dyn_info,
                                                    NULL, attrs,
                                                    &rsp_ltid, &rsp_flags,
                                                    rsp_entry_sid));
        } else {
            /* First move associated data portion of LTID. */
            rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;

            /* Read the Associated data words from PTcache. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_read(unit, ltid, ltid_info,
                                                   rm_flags, 0, from_idx,
                                                   entry_words_row[unit],
                                                   entry_words_size,
                                                   pt_dyn_info,
                                                   NULL, attrs,
                                                   &rsp_ltid, &rsp_flags,
                                                   rsp_entry_sid,
                                                   NULL,
                                                   NULL));

            /* Write the associated data words to PTcache.  */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                                    rm_flags, 0, to_idx,
                                                    entry_words_row[unit],
                                                    pt_dyn_info,
                                                    NULL, attrs,
                                                    &rsp_ltid, &rsp_flags,
                                                    rsp_entry_sid));

            /* Now move TCAM only portion of LTID. */
            rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;

            /* Read the TCAM Only words from PTCache. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_read(unit, ltid, ltid_info,
                                                   rm_flags, 0, from_idx,
                                                   entry_words_row[unit],
                                                   entry_words_size,
                                                   pt_dyn_info,
                                                   NULL, attrs,
                                                   &rsp_ltid, &rsp_flags,
                                                   rsp_entry_sid,
                                                   NULL,
                                                   NULL));

            /* Write the TCAM only words to PTCache. */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                                    rm_flags, 0, to_idx,
                                                    entry_words_row[unit],
                                                    pt_dyn_info,
                                                    NULL, attrs,
                                                    &rsp_ltid, &rsp_flags,
                                                    rsp_entry_sid));
        }
    }

    pt_dyn_info->tbl_inst = orig_tbl_inst;
    /* Update the SW state in three steps. */

    /* 1. Update the TCAM entry information at to_idx. */
    entry_info[to_idx].entry_id = entry_info[from_idx].entry_id;
    entry_info[to_idx].entry_pri = entry_info[from_idx].entry_pri;
    entry_info[to_idx].offset = entry_info[from_idx].offset;
    entry_info[to_idx].global_to_all_pipes =
                         entry_info[from_idx].global_to_all_pipes;

    /* 2. Update the the TCAM SW hash table. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_size_get(
                                            unit, ltid, ltid_info,
                                            attrs,
                                            &entry_hash_size));
    hash_val = (entry_info[to_idx].entry_id) % (entry_hash_size);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_get(unit, ltid, ltid_info,
                                                       attrs,
                                                       &entry_hash));

    curr_offset = entry_hash[hash_val];
    while (curr_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        if (entry_info[curr_offset].entry_id != entry_info[to_idx].entry_id) {
            prev_offset = curr_offset;
            curr_offset = entry_info[curr_offset].offset;
        } else {
            break;
        }
    }

    if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        entry_info[prev_offset].offset = to_idx;
    } else {
        entry_hash[hash_val] = to_idx;
    }

    /* 3. Clear the TCAM entry information at from_idx. */
    entry_info[from_idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
    entry_info[from_idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
    entry_info[from_idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    entry_info[from_idx].global_to_all_pipes = 0;

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_find_loops(unit, ltid, ltid_info,
                                                        attrs));
    }
exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Write the entry words to a particulat TCAM index.
 * Updates both HW and SW state
 */
int
bcmptm_rm_tcam_prio_entry_write(int unit, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t req_flags,
                                uint8_t hw_only, int index,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_tcam_req_t *req_info,
                                bcmltd_sid_t *rsp_ltid,
                                uint32_t *rsp_flags,
                                bcmdrd_sid_t *rsp_entry_sid)

{
    int *entry_hash = NULL;
    uint16_t hash_val;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    uint32_t tcam_flags = 0;
    uint32_t entry_hash_size = 1;
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /* Fetch the TCAM information for the given LTID. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_flags_get(unit, ltid, ltid_info,
                                                        req_info->entry_attrs,
                                                        &tcam_flags));

    /* Update the HW */
    if (tcam_flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {

        /* Write the entry words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit,
                                                ltid, ltid_info, rm_flags,
                                                req_flags, index,
                                                req_info->ekw,
                                                pt_dyn_info,
                                                NULL,
                                                req_info->entry_attrs,
                                                rsp_ltid, rsp_flags,
                                                rsp_entry_sid));
    } else {

        /*  First write the associated data to index. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit,
                                                ltid, ltid_info, rm_flags,
                                                req_flags, index,
                                                req_info->edw,
                                                pt_dyn_info,
                                                NULL,
                                                req_info->entry_attrs,
                                                rsp_ltid, rsp_flags,
                                                rsp_entry_sid));
        /* Next write the key words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit,
                                                ltid, ltid_info, rm_flags,
                                                req_flags, index,
                                                req_info->ekw,
                                                pt_dyn_info,
                                                NULL,
                                                req_info->entry_attrs,
                                                rsp_ltid, rsp_flags,
                                                rsp_entry_sid));
    }

    if (hw_only) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Update the SW state in two steps. */
    /* 1. Update the TCAM entry information. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_info));

    entry_info[index].entry_id = req_info->entry_id;
    entry_info[index].entry_pri = req_info->entry_pri;
    entry_info[index].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    if (req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL) {
        entry_info[index].global_to_all_pipes = 1;
    } else {
        entry_info[index].global_to_all_pipes = 0;
    }

    /* 2. Update the TCAM SW hash table. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_size_get(
                                            unit, ltid, ltid_info,
                                            req_info->entry_attrs,
                                            &entry_hash_size));
    hash_val = (entry_info[index].entry_id) % (entry_hash_size);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_get(unit, ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_hash));

    entry_info[index].offset = entry_hash[hash_val];
    entry_hash[hash_val] = index;

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_find_loops(unit, ltid, ltid_info,
                                                        req_info->entry_attrs));
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Read the entry words from a particular TCAM index.
 */
int
bcmptm_rm_tcam_prio_entry_read(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               uint32_t req_flags, int index,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_rm_tcam_req_t *req_info,
                               bcmptm_rm_tcam_rsp_t *rsp_info,
                               bcmltd_sid_t *rsp_ltid,
                               uint32_t *rsp_flags)
{
    int rv = SHR_E_NONE;
    uint32_t rm_flags = 0;
    uint32_t tcam_flags = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);


    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_flags_get(unit, ltid, ltid_info,
                                                        req_info->entry_attrs,
                                                        &tcam_flags));

    /* Read the TCAM entry words. */
    if (tcam_flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {

        /* Read the  the entry words from index using aggregated view. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        rv = bcmptm_rm_tcam_hw_read(unit,
                                    ltid, ltid_info, rm_flags,
                                    req_flags, index,
                                    rsp_info->rsp_ekw,
                                    req_info->rsp_ekw_buf_wsize,
                                    pt_dyn_info,
                                    NULL, req_info->entry_attrs,
                                    rsp_ltid, rsp_flags,
                                    rsp_info->rsp_entry_sid,
                                    rsp_info->rsp_entry_index,
                                    &rsp_info->rsp_entry_index_count);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

    } else {
        /* Read the  the entry words from index using TCAM only view. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        rv = bcmptm_rm_tcam_hw_read(unit,
                                    ltid, ltid_info, rm_flags,
                                    req_flags, index,
                                    rsp_info->rsp_ekw,
                                    req_info->rsp_ekw_buf_wsize,
                                    pt_dyn_info,
                                    NULL, req_info->entry_attrs,
                                    rsp_ltid, rsp_flags,
                                    rsp_info->rsp_entry_sid,
                                    rsp_info->rsp_entry_index,
                                    &rsp_info->rsp_entry_index_count);

        SHR_IF_ERR_VERBOSE_EXIT(rv);

        /* Read the  the data words from index using DATA only view. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        rv = bcmptm_rm_tcam_hw_read(unit,
                                    ltid, ltid_info, rm_flags,
                                    req_flags, index,
                                    rsp_info->rsp_edw,
                                    req_info->rsp_edw_buf_wsize,
                                    pt_dyn_info,
                                    NULL, req_info->entry_attrs,
                                    rsp_ltid, rsp_flags,
                                    rsp_info->rsp_entry_data_sid,
                                    rsp_info->rsp_entry_data_index,
                                    &rsp_info->rsp_entry_data_index_count);

        SHR_IF_ERR_VERBOSE_EXIT(rv);

    }

    if (pt_dyn_info->tbl_inst == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        rsp_info->rsp_entry_pipe = 0;
    } else {
        rsp_info->rsp_entry_pipe = pt_dyn_info->tbl_inst;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Clear a TCAM entry at specific index. Involves writing all zeros to a
 * particular TCAM index. Updates both HW and SW state.
 */
int
bcmptm_rm_tcam_prio_entry_clear(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t req_flags,
                                uint8_t entry_update_flags,
                                uint32_t index,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_tcam_req_t *req_info,
                                bcmltd_sid_t *rsp_ltid, uint32_t *rsp_flags,
                                bcmdrd_sid_t *rsp_entry_sid)
{
    int *entry_hash = NULL;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint8_t part;
    uint16_t hash_val;
    uint32_t rm_flags = 0;
    uint32_t tcam_flags = 0;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    uint32_t entry_hash_size = 1;
    uint8_t hw_only;
    uint8_t sw_only;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_flags_get(unit, ltid, ltid_info,
                                                        req_info->entry_attrs,
                                                        &tcam_flags));

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_info));

    sw_only = entry_update_flags & BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY;
    hw_only = entry_update_flags & BCMPTM_RM_TCAM_ENTRY_OP_HW_ONLY;

    /* if entry is invalid no need to clear it. */
    if (entry_info[index].entry_id == BCMPTM_RM_TCAM_EID_INVALID && !hw_only) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (!sw_only) {
        /* Update the HW with NULL data. */
        for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
            sal_memset(entry_words[unit][part], 0,
                       sizeof(entry_words[unit][part]));
            entry_words_row[unit][part] = entry_words[unit][part];
        }


        if (tcam_flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {

            /* Write the entry words to index. */
            rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                        rm_flags, req_flags, index,
                                        entry_words_row[unit],
                                        pt_dyn_info,
                                        NULL, req_info->entry_attrs,
                                        rsp_ltid, rsp_flags,
                                        rsp_entry_sid));

        } else {

            /*  First write the associated data to index. */
            rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                        rm_flags, req_flags, index,
                                        entry_words_row[unit],
                                        pt_dyn_info,
                                        NULL, req_info->entry_attrs,
                                        rsp_ltid, rsp_flags,
                                        rsp_entry_sid));


            /* Next write the key words to index. */
            rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                        rm_flags, req_flags, index,
                                        entry_words_row[unit],
                                        pt_dyn_info,
                                        NULL, req_info->entry_attrs,
                                        rsp_ltid, rsp_flags,
                                        rsp_entry_sid));
        }

        if (hw_only) {
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }

    /* Update the SW state in two steps. */
    /* 1. Update the TCAM SW hash table. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_size_get(
                                            unit, ltid, ltid_info,
                                            req_info->entry_attrs,
                                            &entry_hash_size));
    hash_val = (req_info->entry_id) % (entry_hash_size);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_get(unit, ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_hash));

    curr_offset = entry_hash[hash_val];
    while (entry_info[curr_offset].entry_id != req_info->entry_id) {
        prev_offset = curr_offset;
        curr_offset = entry_info[curr_offset].offset;
    }

    if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        entry_info[prev_offset].offset = entry_info[curr_offset].offset;
    } else {
        entry_hash[hash_val] = entry_info[curr_offset].offset;
    }

    /* 2. Update the TCAM entry information. */
    entry_info[index].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
    entry_info[index].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
    entry_info[index].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    entry_info[index].global_to_all_pipes = FALSE;

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_find_loops(unit, ltid, ltid_info,
                                                        req_info->entry_attrs));
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Insert the new TCAM entry or update the existing TCAM entry in
 * priority based TCAMs.
 *
 * Algorithm:
 *  1.Application provides TCAM Entry Data.
 *    1.1.For priority based TCAM; Entry Data contains
 *         KEY + MASK + POLICY + entry_id + PRIOIRTY.
 *  2.Do Lookup for the given TCAM entry Data to find the entry index.
 *    2.1.For priority based TCAMs, lookup happens with entry_id.
 *  3.If entry index is Non Negative (meaning entry existed)?
 *    3.1.If application is asking for Entry Replace?
 *        3.1.1.Call south bound interface to give the write command with
 *              update entry data (New KEY + New MASK + New POLICY) at
 *              entry index.
 *        3.1.2. Return SUCCESS
 *    3.2.If application is not asking for Entry Replace?
 *        3.2.1.Return error code ENTRY EXISTS back to upper layers.
 *  4.If entry index is Negative (meaning entry does not exist)?
 *    4.1.Check if at least one free slot is available in TCAM by checking TCAM
 *        free indexes status.
 *    4.2.If at least one free slot is available run the algorithm to find the
 *        target_index to use for the entry.
 *        4.2.1.Search the last entry with priority greater than given
 *              priority. That entries index + 1 become the target_index.
 *        4.2.2.If target_index is free then insert the entry at that free slot.
 *        4.2.3.If target_index is not free, find first free slot after and
 *              before the target_index.
 *        4.2.4.Whichever direction (up/down) takes less number of moves then
 *              move all entries from target_index to free index move up/down
 *              one index to create a free slot for the target TCAM entry.
 *        4.2.5.Call the south bound interface to give the write command with
 *              new entry data (KEY + MASK + POLICY) at new index allocated.
 *        4.2.6.Return SUCCESS.
 *    4.3.If at least one free index is not available?
 *        4.3.1.Return error code TABLE FULL.
 */

int
bcmptm_rm_tcam_prio_entry_insert(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags)
{
    int rv = SHR_E_NONE;
    uint32_t found_index = -1;
    uint16_t target_idx = -1;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    if (req_info->entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if entry already exists or not. */
    if (req_info->same_key == true) {
        /* If already looked up, use the results */
        if (same_key_info[unit].index != -1) {
            found_index = same_key_info[unit].index;
            rv = SHR_E_NONE;
        } else {
            found_index = -1;
            rv = SHR_E_NOT_FOUND;
        }
        /* For prio update check */
        rsp_info->rsp_entry_pri = same_key_info[unit].priority;

    } else {
        /* Check if entry id exists or not. */
        rv = bcmptm_rm_tcam_prio_entry_lookup(unit,
                                              req_flags, ltid, ltid_info,
                                              pt_dyn_info,
                                              req_info, rsp_info,
                                              rsp_ltid, rsp_flags,
                                              &found_index);

        if (SHR_FAILURE(rv) && rv != SHR_E_NOT_FOUND) {
            SHR_RETURN_VAL_EXIT(rv);
        }
    }

    if (rv == SHR_E_NONE) {

        /* Update the TCAM entry */
        SHR_RETURN_VAL_EXIT((bcmptm_rm_tcam_prio_entry_update(unit,
                                          req_flags, ltid, ltid_info,
                                          pt_dyn_info,
                                          req_info, rsp_info,
                                          rsp_ltid, rsp_flags,
                                          found_index)));
    } else {

       /* Find Index to Insert the TCAM entry */
       rv = (bcmptm_rm_tcam_prio_entry_index_allocate(unit,
                                                      req_flags,
                                                      ltid, ltid_info,
                                                      0,
                                                      pt_dyn_info,
                                                      req_info,
                                                      rsp_info,
                                                      rsp_ltid,
                                                      rsp_flags,
                                                      &target_idx
                                                      ));
       /* 0 free entries*/
       if (rv != SHR_E_NONE) {
           SHR_RETURN_VAL_EXIT(rv);
       }
       /* Insert the new entry at target_idx. */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_write(unit,
                                                           ltid, ltid_info,
                                                           req_flags,
                                                           0,
                                                           target_idx,
                                                           pt_dyn_info,
                                                           req_info,
                                                           rsp_ltid,
                                                           rsp_flags,
                                                           rsp_info->rsp_entry_sid));
       /* Read the inserted data to the response */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_read(unit,
                                                          ltid, ltid_info,
                                                          req_flags,
                                                          target_idx,
                                                          pt_dyn_info,
                                                          req_info,
                                                          rsp_info,
                                                          rsp_ltid,
                                                          rsp_flags));

       /* Fill in the entry attributes to the response */
       rsp_info->rsp_entry_pri = req_info->entry_pri;

       if (req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL &&
           req_info->same_key == 0) {
           same_key_info[unit].index = target_idx;
           same_key_info[unit].priority = req_info->entry_pri;
       }
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/* Create a new free slot to insert an entry. */

int
bcmptm_rm_tcam_prio_entry_index_allocate(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t prio_change,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 uint16_t *target_index)
{
    int target_idx = -1;
    uint32_t idx = -1;
    int up_free_block_first_idx = -1;
    int up_free_idx = -1;
    int down_free_idx = -1;
    int from_idx = -1;
    int to_idx = -1;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    uint32_t free_entries = 0;
    uint32_t num_entries = 0;
    uint8_t mode = 0;
    int num_parts = -1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_FP) {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)req_info->entry_attrs;
        mode = entry_attrs->group_mode;
    } else {
        mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;
    }

    num_parts = ltid_info->hw_entry_info[mode].num_key_rows;

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_free_count_get(unit, ltid,
                                            ltid_info,
                                            req_info->entry_attrs,
                                            &free_entries));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_valid_count_get(unit, ltid,
                                            ltid_info,
                                            req_info->entry_attrs,
                                            &num_entries));

    /*
     * If free entries are not available, entry cannot be inserted.
     * If HW atomicity is supported then full range can be used.
     * For entries which require more than one key row atomicity
     * can't be achieved even if HW supports.
     */
    if ((prio_change == 0) &&
        ((ltid_info->rm_more_info->hw_atomicity_support == 0) ||
         (num_parts > 1) || ltid_info->non_aggr) && (free_entries == 1)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
    }

    if (free_entries == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
    }

    /* If no valid entries allocate the very first entry in TCAM. */
    if (free_entries == num_entries) {
        *target_index = 0;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_free_count_decr(unit,
                                                     ltid, ltid_info,
                                                     0,
                                                     req_info->entry_attrs));
        SHR_FUNC_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit,
                                                       ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_info));

    for (idx = 0; idx < num_entries; idx++) {

        /* Record the last free tcam index before target index is found. */
        if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            if (entry_info[idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
                up_free_idx = idx;
                /* Record first index when there a block of free entries */
                if (up_free_block_first_idx == -1) {
                    up_free_block_first_idx = idx;
                }
                continue;
            }
        }

        /* Record the target index for the given prioirty. */
        if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            if (entry_info[idx].entry_pri < req_info->entry_pri) {
                target_idx = idx;
                /* If free entries block is just above the target index
                 * up_free_block_first_idx is the target index
                 * for the new entry */
                if (up_free_block_first_idx != -1) {
                    break;
                }
                continue;
            } else if (up_free_block_first_idx != -1) {
                /* Free entries block is just before entry with same prio.
                 * use up_free_block_first_idx as the target index*/
                if (entry_info[idx].entry_pri == req_info->entry_pri) {
                    break;
                }
                up_free_block_first_idx = -1;
            }
        }

        /* Record the first free tcam index after target index is found. */
        if (entry_info[idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
            down_free_idx = idx;
            break;
        }

    }

    /* Ideally should not hit this case as the above loop will be
     * executed only if atleast one free index is available in TCAM.
     */
    if (up_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Free index is found only in up ward direction. */
    if (up_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {

        /* Free entries block just before the actual target index or
         * free entries block is till the end of slice */
        if (up_free_block_first_idx != -1) {
            target_idx = up_free_block_first_idx;
            up_free_idx = up_free_block_first_idx;
        }
        /* All existing valid entries have greater priority than
         * target entry.
         */
        else if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            target_idx =  num_entries - 1;
        } else {
            target_idx--;
        }


        /* Move entries up ward direction. */
        from_idx = target_idx;
        to_idx = up_free_idx;
    }

    /* Free index is found only in down ward direction. */
    if (up_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID) {

        /* Move entries down ward direction. */
        from_idx = target_idx;
        to_idx = down_free_idx;

    }

    /* Free index is found in both up and down ward direction. */
    if (up_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID) {

        if ((target_idx - up_free_idx) >=
            (down_free_idx - target_idx)) {
            /* Move entries down ward direction. */
            from_idx = target_idx;
            to_idx = down_free_idx;
        } else {
            /* Move entries up ward direction. */
            target_idx--;
            from_idx = target_idx;
            to_idx = up_free_idx;
        }
    }

    /* Move entries either up or down ward direction. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_entry_move_range(unit,
                                                    ltid, ltid_info,
                                                    0, from_idx,
                                                    to_idx,
                                                    req_info->entry_attrs,
                                                    pt_dyn_info));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_free_count_decr(unit,
                                                     ltid, ltid_info,
                                                     to_idx,
                                                     req_info->entry_attrs));
    *target_index = target_idx;

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}
/*
 * Delete the TCAM entry for a given Entry id.
 *
 * Algorithm:
 *  1. Application provides required information about the TCAM Entry
 *     to identify the entry location in HW.
 *     1.1. For priority based TCAM; this information contains entry_id
 *  2. Do Lookup for the given TCAM information to find the entry index.
 *     2.1. For priority based TCAM lookup happens on entry_id.
 *  3. If entry index is Non Negative (meaning entry is existed)?
 *     3.1. Call south bound interface to give the write command with
 *          NULL entry data (meaning all zero) at entry index.
 *     3.2. Return SUCCESS
 *  4. If entry index is Negative (meaning entry does not exist)?
 *     4.1. Return the error code ENTRY NOT FOUND.
 */
int
bcmptm_rm_tcam_prio_entry_delete(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags)
{
    uint32_t index = BCMPTM_RM_TCAM_INDEX_INVALID;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /* Check if entry id exists or not. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_lookup(unit,
                                                     req_flags, ltid,
                                                     ltid_info,
                                                     pt_dyn_info,
                                                     req_info, rsp_info,
                                                     rsp_ltid, rsp_flags,
                                                     &index));

    /* Clear the data in HW and SW state */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_clear(unit, ltid, ltid_info,
                                                    req_flags,
                                                    0, index,
                                                    pt_dyn_info, req_info,
                                                    rsp_ltid, rsp_flags,
                                                    rsp_info->rsp_entry_sid));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_free_count_incr(unit, ltid,
                                                             ltid_info,
                                                             index,
                                                             req_info->entry_attrs));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_entry_delete(unit, ltid, ltid_info,
                                                  pt_dyn_info,
                                                  req_info));

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Do search for given TCAM entry id and read the corresponding entry words.
 *
 * Algorithm:
 *  1. Application provides required information about the TCAM Entry
 *     to identify the entry location in HW.
 *     1.1. For priority based TCAM; this information contains entry_id
 *  2. Caliculate the hash for the given TCAM entry Id.
 *  3. Check TCAM entry id is present in linked list of Tcam entry ids
 *     having the same hash value.
 *  4. If entry_id exists, get the TCAM entry Index from TCAM entry metadata.
 *     4.1. If TCAM Entry Index is non negative read entry data (KEY + MASK +
 *          POLICY) will be given back to application.
 *     4.2. If TCAM entry index is negative, then there is a bug in the code and
 *          return error code Internal Error
 *  5. If entry_id does not exist, return error code Entry not Found.
 */
int
bcmptm_rm_tcam_prio_entry_lookup(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 uint32_t *index)
{
    int *entry_hash = NULL;
    int offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint16_t hash_val;
    uint32_t entry_hash_size = 1;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    if (req_info->entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    same_key_info[unit].index = -1;
    same_key_info[unit].priority = -1;

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_size_get(
                                            unit, ltid, ltid_info,
                                            req_info->entry_attrs,
                                            &entry_hash_size));

    /* Check if entry id  exists or not. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_info_get(unit, ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_info));

    /* Calculate the hash */
    hash_val = (req_info->entry_id) % (entry_hash_size);

    /* Fetch the entry hash details */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_hash_get(unit, ltid, ltid_info,
                                                       req_info->entry_attrs,
                                                       &entry_hash));

    /* Get the offset */
    offset = entry_hash[hash_val];

    /*  Search the list of entries till the id matches */
    while (offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        if (entry_info[offset].entry_id == req_info->entry_id) {
            break;
        }
        offset = entry_info[offset].offset;
    }

    if (offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    if (index != NULL) {
        *index = offset;
    }

    /* Read the entry to set the response fields */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_entry_read(unit,
                                                   ltid, ltid_info, req_flags,
                                                   offset,
                                                   pt_dyn_info,
                                                   req_info, rsp_info,
                                                   rsp_ltid, rsp_flags));

   rsp_info->rsp_entry_pri = entry_info[offset].entry_pri;
   same_key_info[unit].index = offset;
   same_key_info[unit].priority = entry_info[offset].entry_pri;

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/* Calculate memory size required for Prio Only type TCAMs */
int
bcmptm_rm_tcam_prio_calc_mem_required(int unit, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *user_data)
{
    uint32_t size = 0;
    uint32_t trans_req_size = 0;
    uint32_t num_entries = 0;
    uint32_t *total_size = user_data;
    int8_t num_pipes;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check. */
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    /* This should always be the first structure
     * memory allocation for every LTID
     */
    /* memory required to save Transaction related info */
    trans_req_size = sizeof(bcmptm_rm_tcam_trans_info_t);

    num_pipes = ltid_info->rm_more_info[0].pipe_count;

    size += sizeof(bcmptm_rm_tcam_prio_eid_info_t);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info,
                                                 &num_entries));
    /* Memory required for TCAM entry information consists of
     * (eid, priority and offset)
     */
    size += (num_entries *
                      sizeof(bcmptm_rm_tcam_prio_entry_info_t));

    /* Memory space required for TCAM hash table. */
    size += (sizeof(int) * (num_entries / 4));

    /* Memory space required for all pipes. */
    (*total_size) += trans_req_size + (num_pipes * size);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the metadata of priority based TCAM in
 * bcmptm_rm_tcam_prio_eid_info_t format.
 */
int
bcmptm_rm_tcam_prio_init_metadata(int unit, bool warm, bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  void *start_pointer, uint32_t *offset)
{
    int *entry_hash = NULL;
    uint32_t num_entries = 0;
    uint32_t idx;
    uint32_t entry_info_size = sizeof(bcmptm_rm_tcam_prio_entry_info_t);
    int8_t num_pipes, pipe;

    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    uint8_t *start_ptr = NULL;
    uint32_t size = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(start_pointer, SHR_E_PARAM);
    SHR_NULL_CHECK(offset, SHR_E_PARAM);

    start_ptr = (uint8_t*) start_pointer;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info,
                                             &num_entries));
    num_pipes = ltid_info->rm_more_info[0].pipe_count;

    /* set the Transaction state to Idle */
    trans_info = (bcmptm_rm_tcam_trans_info_t *)(start_ptr + *offset);

    if (warm == FALSE) {
        trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
    } else {
        if (trans_info->sign_word != BCMPTM_HA_SIGN_LT_OFFSET_INFO) {
            SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
        }
    }

    /* set the size */
    trans_info->ltid_size = sizeof(bcmptm_rm_tcam_trans_info_t);

    /* update offset */
    *offset += sizeof(bcmptm_rm_tcam_trans_info_t);

    for (pipe = 0; pipe < num_pipes; pipe++)
    {
        /* Set the TCAM info position */
        tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)(start_ptr + *offset);

        /*
         * Initalize only during coldboot.
         * During warmboot, reinitialize only offset fields, size and offset.
         * num_entries are expected to not change across warmboot versions.
         */
        if (warm == FALSE) {
            tcam_info->ltid = ltid;

            tcam_info->flags = 0;

            /* Set tcam info flags according to LT_INFO flags */
            if (ltid_info->non_aggr == 0) {
                tcam_info->flags |= BCMPTM_RM_TCAM_F_AGGR_VIEW;
            }
            if (ltid_info->non_aggr == 1) {
                tcam_info->flags |= BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
                tcam_info->flags |= BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
            }

            /* Set the enty counts */
            tcam_info->num_entries = num_entries;
            tcam_info->free_entries =  num_entries;

            /* Set the entry info and hash details */
            if (tcam_info->num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
                tcam_info->entry_hash_size = (num_entries /
                                       BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
            } else {
                tcam_info->entry_hash_size = num_entries;
            }
        } else {
            /*
             * Ensure that ltid, num_entries and hash size is not changed
             * during warmboot.
             */
            if (tcam_info->ltid != ltid ||
                tcam_info->num_entries != num_entries) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
            if (num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
                if (tcam_info->entry_hash_size != (num_entries /
                                     BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
            } else if (tcam_info->entry_hash_size != num_entries) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }

        tcam_info->entry_info_offset = sizeof(bcmptm_rm_tcam_prio_eid_info_t);

        tcam_info->entry_hash_offset = tcam_info->entry_info_offset +
                                   (tcam_info->num_entries * entry_info_size);

        /* Initalize only during coldboot. */
        if (warm == FALSE) {
            entry_info =  (bcmptm_rm_tcam_prio_entry_info_t *)
                          (start_ptr + *offset + tcam_info->entry_info_offset);
            for (idx = 0; idx < tcam_info->num_entries; idx++) {
                entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
                entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
                entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
            }

            entry_hash = (int*) (start_ptr + *offset
                                 + tcam_info->entry_hash_offset);

            for (idx = 0; idx < tcam_info->entry_hash_size; idx++) {
                entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
            }
        }

        size = (sizeof(bcmptm_rm_tcam_prio_eid_info_t) +
                (sizeof(bcmptm_rm_tcam_prio_entry_info_t) * num_entries) +
                (sizeof(int) * tcam_info->entry_hash_size));

        tcam_info->next_pipe_tcam_info_offset = size;

        (trans_info->ltid_size) += size;

        (*offset) += size;
    }

    if (warm == FALSE) {
        trans_info->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;
    }

exit:
     SHR_FUNC_EXIT();
 }
