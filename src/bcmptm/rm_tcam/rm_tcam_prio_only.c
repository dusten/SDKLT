/*! \file rm_tcam_prio_only.c
 *
 * Low Level Functions for pri_key(priority, Entry Key information) based TCAMs
 * This file contains low level functions for pri_key based TCAMs
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
#include <shr/shr_crc.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_traverse.h"
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private variables
 */
static uint32_t g_buf_one[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t g_buf_two[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t entry_words[BCMDRD_CONFIG_MAX_UNITS]
                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS][BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t *entry_words_row[BCMDRD_CONFIG_MAX_UNITS]
                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];


/*******************************************************************************
 * Private Functions
 */
static int
rm_tcam_prio_only_find_loops(int unit, bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             int pipe_id)
{
    int rv = SHR_E_NONE;
    uint32_t idx = 0, idx2 = 0;
    uint32_t *entry_hash = NULL;
    int offset1 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int first_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int offset2 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t num_offsets = 0;
    uint32_t num_entries = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_only_sw_state_t *sw_state = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    rv = bcmptm_rm_tcam_prio_only_sw_state_get(unit, ltid,
                                        ltid_info,
                                        pipe_id,
                                        &sw_state);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid,
                                                 ltid_info,
                                                 pipe_id,
                                                 &entry_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_hash_get(unit, ltid,
                                                 ltid_info,
                                                 pipe_id,
                                                 &entry_hash);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit, ltid,
                                             ltid_info, &num_entries));

    for (idx = 0; idx < sw_state->entry_hash_size; idx++) {

         first_offset = (int)entry_hash[idx];

         if (first_offset < (int)num_entries &&
             first_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
             offset1 = (int)(entry_info[first_offset].offset);
         }

         num_offsets = 0;
         while (offset1 != BCMPTM_RM_TCAM_OFFSET_INVALID) {

             SHR_IF_ERR_EXIT(
                 (offset1 >= (int)num_entries) ? SHR_E_INTERNAL : SHR_E_NONE);

             offset2 = (int)(entry_info[first_offset].offset);

             for (idx2 = 0; idx2 < num_offsets; idx2++) {

                 SHR_IF_ERR_EXIT(
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

/* Function used to verify if the key is same as the one
 * present in the index provided.
 */

static int
rm_tcam_prio_only_entry_compare(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t index,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_tcam_req_t *req_info)
{
    int rv = SHR_E_NONE;
    bcmltd_sid_t rsp_ltid;
    uint32_t sindex;
    uint8_t srow;
    uint8_t part;
    uint8_t num_bytes = 0;
    uint16_t num_bits = 0;
    uint32_t rm_flags = 0;
    uint32_t rsp_flags = 0;
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {0};
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;

    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);

    rv = bcmptm_rm_tcam_tindex_to_sindex(unit, ltid, ltid_info, index,
                                         &sindex, &srow);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
         sal_memset(entry_words[unit][part], 0 ,
                    sizeof(entry_words[unit][part]));
         entry_words_row[unit][part] = entry_words[unit][part];
    }

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pt_dyn_info->tbl_inst,
                                            &tcam_info));

    if (tcam_info->flags &  BCMPTM_RM_TCAM_F_AGGR_VIEW) {

        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;

        /* Read the entry words from from_idx. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_read(unit, ltid, ltid_info,
                                               rm_flags, 0, index,
                                               entry_words_row[unit],
                                               entry_words_size,
                                               pt_dyn_info,
                                               NULL,
                                               req_info->entry_attrs,
                                               &rsp_ltid, &rsp_flags,
                                               rsp_entry_sid,
                                               NULL, NULL));

    } else {
        /* Now move TCAM only portion of LTID. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;

        /* Read the TCAM Only words from PTCache. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_read(unit, ltid, ltid_info,
                                               rm_flags, 0, index,
                                               entry_words_row[unit],
                                               entry_words_size,
                                               pt_dyn_info,
                                               NULL,
                                               req_info->entry_attrs,
                                               &rsp_ltid, &rsp_flags,
                                               rsp_entry_sid,
                                               NULL, NULL));
    }


    sal_memset(g_buf_one[unit], 0, sizeof(g_buf_one[unit]));
    sal_memset(g_buf_two[unit], 0, sizeof(g_buf_two[unit]));

    for (part = 0; part < ltid_info->hw_entry_info->num_key_rows; part++) {

        num_bits = ltid_info->hw_entry_info->key_size;

        bcmdrd_field_get(entry_words[unit][part], 0, (num_bits - 1),
                         g_buf_one[unit]);

        bcmdrd_field_get(req_info->ekw[part], 0, (num_bits - 1),
                         g_buf_two[unit]);

        num_bytes = num_bits / 8;
        num_bits = num_bits % 8;

        if (num_bits) {
            num_bytes++;
        }

        rv = sal_memcmp(g_buf_one[unit], g_buf_two[unit], num_bytes);
        if (rv) {
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/*
 * Get the pointer to tcam information of given TCAM LTID, pipe id.
 */
int
bcmptm_rm_tcam_prio_only_tcam_info_get(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        int pipe_id,
                        bcmptm_rm_tcam_prio_only_info_t **tcam_info)
{
    void *tcam_ptr = NULL;
    int count = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_info_get(unit, ltid, ltid_info,
                                                    &tcam_ptr));
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE
        || pipe_id == 0) {
        *tcam_info = (bcmptm_rm_tcam_prio_only_info_t *) ((uint8_t *)tcam_ptr +
                                        sizeof(bcmptm_rm_tcam_trans_info_t));
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    tcam_ptr =  ((uint8_t*)tcam_ptr + sizeof(bcmptm_rm_tcam_trans_info_t));
    while (count != pipe_id) {
        /* add offset for next pipe */
        tcam_ptr = ((uint8_t *)tcam_ptr
                    + ((bcmptm_rm_tcam_prio_only_info_t *)tcam_ptr)->next_pipe_tcam_info_offset);
        count++;
    }
    *tcam_info = (bcmptm_rm_tcam_prio_only_info_t *)((uint8_t *)tcam_ptr);

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}


/* Function to get the software state for the hash
 * details of the Prio Key TCAM
 */

int
bcmptm_rm_tcam_prio_only_sw_state_get(int unit, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                int pipe_id,
                                bcmptm_rm_tcam_prio_only_sw_state_t **sw_state)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(sw_state, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                            ltid, ltid_info, pipe_id,
                                            &tcam_info));

    offset = tcam_info->sw_state_offset;

    *sw_state = (bcmptm_rm_tcam_prio_only_sw_state_t *)
                ((uint8_t *)tcam_info + offset);

exit:
    SHR_FUNC_EXIT();
}

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
bcmptm_rm_tcam_prio_only_entry_info_get(int unit, bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         int pipe_id,
                         bcmptm_rm_tcam_prio_only_entry_info_t **entry_info)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_sw_state_t *sw_state = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pipe_id,
                                            &tcam_info));

    tcam_info =  (bcmptm_rm_tcam_prio_only_info_t*)((uint8_t *)tcam_info);

    offset = tcam_info->sw_state_offset;

    sw_state = (bcmptm_rm_tcam_prio_only_sw_state_t*)
                ((uint8_t *)tcam_info + offset);

    *entry_info = NULL;
    offset = sw_state->entry_info_offset;
    *entry_info = (bcmptm_rm_tcam_prio_only_entry_info_t*)((uint8_t*)tcam_info
                   + offset);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fetch the address to TCAM entry hash of the given Logical Table id.
 * \n This address points to the address of hash zero. To get the address to
 * \n any other valid hash, increment this start pointer by hash times as memory
 * \n created to hold the hash information of all valid hash values is consecutive.
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
bcmptm_rm_tcam_prio_only_entry_hash_get(int unit,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       int pipe_id,
                                       uint32_t **entry_hash)
{
    uint32_t offset = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_sw_state_t *sw_state = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(entry_hash, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pipe_id,
                                            &tcam_info));

    tcam_info =  (bcmptm_rm_tcam_prio_only_info_t*)((uint8_t *)tcam_info);

    offset = tcam_info->sw_state_offset;

    sw_state = (bcmptm_rm_tcam_prio_only_sw_state_t*)
                ((uint8_t *)tcam_info + offset);

    *entry_hash = NULL;

    offset = sw_state->entry_hash_offset;

    *entry_hash = (uint32_t*) ((uint8_t*)tcam_info
                                + offset);

exit:
    SHR_FUNC_EXIT();
}

/* Calculate the hash value for the entry key and data */
int
bcmptm_rm_tcam_prio_only_entry_hash_value_get(int unit, bcmltd_sid_t ltid,
                                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                                           uint32_t **ekw,
                                           uint32_t **edw,
                                           uint32_t *hash_val)
{
    int rv = SHR_E_NONE;
    uint8_t part;
    uint16_t num_bits = 0;
    uint32_t total_bits;

    bcmptm_rm_tcam_prio_only_sw_state_t *sw_state = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(ekw, SHR_E_PARAM);
    SHR_NULL_CHECK(hash_val, SHR_E_PARAM);

    sal_memset(g_buf_one[unit], 0, sizeof(g_buf_one[unit]));
    sal_memset(g_buf_two[unit], 0, sizeof(g_buf_two[unit]));

    /* Calculate the number of bits for hashing */
    total_bits = 0;
    for (part = 0; part < ltid_info->hw_entry_info->num_key_rows; part++) {

        num_bits = ltid_info->hw_entry_info->key_size;

        bcmdrd_field_get(ekw[part], 0, (num_bits - 1), g_buf_one[unit]);
        bcmdrd_field_set(g_buf_two[unit], total_bits, total_bits + (num_bits -1),
                         g_buf_one[unit]);
        total_bits += num_bits;
    }

    /* Calculate the hash */
    *hash_val = shr_crc16b(0,
                           (uint8_t *)&g_buf_two[unit],
                           total_bits);

    rv = bcmptm_rm_tcam_prio_only_sw_state_get(unit, ltid,
                                        ltid_info,
                                        BCMPTM_RM_TCAM_GLOBAL_PIPE,
                                        &sw_state);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    *hash_val = (*hash_val % sw_state->entry_hash_size);

exit:
    SHR_FUNC_EXIT();
}

/* Function used to verify if the request key is same as the one
 * present in the index provided in traverse snapshot
 */
int
bcmptm_rm_tcam_traverse_prio_only_entry_compare(int unit,
                                                bcmltd_sid_t ltid,
                                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                                uint32_t index,
                                                bcmptm_rm_tcam_req_t *req_info,
                                                uint32_t *key_db,
                                                uint16_t key_size)
{
    int rv = SHR_E_NONE;
    uint8_t part;
    uint8_t num_bytes = 0;
    uint16_t num_bits = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(key_db, SHR_E_PARAM);

    sal_memset(g_buf_one[unit], 0, sizeof(g_buf_one[unit]));
    sal_memset(g_buf_two[unit], 0, sizeof(g_buf_two[unit]));

    num_bits = ltid_info->hw_entry_info->key_size;

    num_bytes = (num_bits + 7) / 8;

    for (part = 0; part < ltid_info->num_ekw; part++) {

        bcmdrd_field_get((key_db + ((index + part) * key_size)),
                         0, (num_bits - 1), g_buf_one[unit]);

        bcmdrd_field_get(req_info->ekw[part], 0, (num_bits - 1),
                         g_buf_two[unit]);

        rv = sal_memcmp(g_buf_one[unit], g_buf_two[unit], num_bytes);
        if (rv) {
            SHR_RETURN_VAL_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Move TCAM entry from one index to the other index.
 * Both HW and SW state will be updated.
 */
int
bcmptm_rm_tcam_prio_only_entry_move(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint8_t partition,
                                   uint32_t prefix,
                                   uint32_t from_idx,
                                   uint32_t to_idx,
                                   void *attrs,
                                   bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    int rv = SHR_E_NONE;
    bcmltd_sid_t rsp_ltid = -1;
    uint32_t *entry_hash = NULL;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint8_t part = 0;
    uint32_t hash_val;
    uint32_t rm_flags = 0;
    uint32_t rsp_flags = 0;
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {0};
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint8_t entry_type;
    uint32_t idx;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);


    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pt_dyn_info->tbl_inst,
                                            &tcam_info));

    /* Fetch the TCAM entry information for the givem LTID. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid,
                                                          ltid_info,
                                                          pt_dyn_info->tbl_inst,
                                                          &entry_info));

    /* If the entry is empty no need to move. */
    if (entry_info[from_idx].entry_pri == BCMPTM_RM_TCAM_EID_INVALID ||
        (ltid_info->hw_entry_info->entry_boundary != 0 &&
         from_idx % ltid_info->hw_entry_info->entry_boundary != 0)) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
        sal_memset(entry_words[unit][part], 0, sizeof(entry_words[unit][part]));
        entry_words_row[unit][part] = entry_words[unit][part];
    }

    if (tcam_info->flags &  BCMPTM_RM_TCAM_F_AGGR_VIEW) {

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
                                               NULL, NULL));

        /* Write the entry words to to_idx. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                                rm_flags, 0, to_idx,
                                                entry_words_row[unit],
                                                pt_dyn_info,
                                                NULL,  attrs,
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
                                               NULL,  attrs,
                                               &rsp_ltid, &rsp_flags,
                                               rsp_entry_sid,
                                               NULL, NULL));

        /* Write the associated data words to PTcache.  */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                                rm_flags, 0, to_idx,
                                                entry_words_row[unit],
                                                pt_dyn_info,
                                                NULL,  attrs,
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
                                               NULL,  attrs,
                                               &rsp_ltid, &rsp_flags,
                                               rsp_entry_sid,
                                               NULL, NULL));

        /* Write the TCAM only words to PTCache. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit, ltid, ltid_info,
                                                rm_flags, 0, to_idx,
                                                entry_words_row[unit],
                                                pt_dyn_info,
                                                NULL,  attrs,
                                                &rsp_ltid, &rsp_flags,
                                                rsp_entry_sid));
    }

    /* Update the SW state in three steps. */

    /* 1. Update the TCAM entry information at to_idx. */
    entry_type = ltid_info->hw_entry_info->num_key_rows;
    entry_info[to_idx].offset = entry_info[from_idx].offset;

    for (idx = to_idx; idx < (to_idx + entry_type); idx++) {
        entry_info[idx].entry_pri = entry_info[from_idx].entry_pri;
        entry_info[idx].entry_type = entry_type;
    }
    /* 2. Update the the TCAM SW hash table. */
    rv = bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit, ltid, ltid_info,
                                                  entry_words_row[unit],
                                                  NULL, &hash_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_hash_get(unit, ltid, ltid_info,
                                             pt_dyn_info->tbl_inst,
                                             &entry_hash);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    curr_offset = entry_hash[hash_val];
    while (curr_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        if (curr_offset != (int)from_idx) {
            prev_offset = curr_offset;
            curr_offset = entry_info[curr_offset].offset;
        } else {
            break;
        }
    }

    if ((curr_offset == BCMPTM_RM_TCAM_OFFSET_INVALID)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        entry_info[prev_offset].offset = to_idx;
    } else {
        entry_hash[hash_val] = to_idx;
    }

    /* 3. Clear the TCAM entry information at from_idx. */
    for (idx = from_idx; idx < (from_idx + entry_type); idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_only_find_loops(unit, ltid,
                                                             ltid_info,
                                                             pt_dyn_info->tbl_inst));
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Write the entry words to a particular TCAM index.
 * Updates both HW and SW state
 */
int
bcmptm_rm_tcam_prio_only_entry_write(int unit, bcmltd_sid_t ltid,
                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                    uint32_t req_flags,
                                    uint8_t hw_only, int index,
                                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                                    bcmptm_rm_tcam_req_t *req_info,
                                    bcmltd_sid_t *rsp_ltid, uint32_t *rsp_flags,
                                    bcmdrd_sid_t *rsp_entry_sid)
{
    int rv = SHR_E_NONE;
    uint32_t *entry_hash = NULL;
    uint32_t hash_val;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint8_t entry_type = 0;
    int idx;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pt_dyn_info->tbl_inst,
                                            &tcam_info));
    /* Update the HW */
    if (tcam_info->flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {

        /* Write the entry words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit,
                                               ltid, ltid_info, rm_flags,
                                               req_flags, index,
                                               req_info->ekw,
                                               pt_dyn_info,
                                               NULL, req_info->entry_attrs,
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
                                                NULL, req_info->entry_attrs,
                                                rsp_ltid, rsp_flags,
                                                rsp_entry_sid));

        /* Next write the key words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_hw_write(unit,
                                                ltid, ltid_info, rm_flags,
                                                req_flags, index,
                                                req_info->ekw,
                                                pt_dyn_info,
                                                NULL, req_info->entry_attrs,
                                                rsp_ltid, rsp_flags,
                                                rsp_entry_sid));

    }

    if (hw_only) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }
    /* Update the SW state in two steps. */

    /* 1. Update the TCAM entry information. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid,
                                                            ltid_info,
                                                            pt_dyn_info->tbl_inst,
                                                            &entry_info));

    entry_info[index].entry_pri = req_info->entry_pri;
    entry_info[index].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;

    /* 2. Update the TCAM SW hash table. */
    rv = bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit, ltid, ltid_info,
                                                      req_info->ekw,
                                                      NULL, &hash_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_hash_get(unit, ltid, ltid_info,
                                                 pt_dyn_info->tbl_inst,
                                                 &entry_hash);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    entry_type = ltid_info->hw_entry_info->num_key_rows;
    entry_info[index].offset = entry_hash[hash_val];
    entry_hash[hash_val] = index;

    for (idx = index; idx < (index + entry_type); idx++) {
        entry_info[idx].entry_pri = req_info->entry_pri;
        entry_info[idx].entry_type = entry_type;
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_only_find_loops(unit, ltid, ltid_info,
                                                             pt_dyn_info->tbl_inst));
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Read the entry words from a particular TCAM index.
 */
int
bcmptm_rm_tcam_prio_only_entry_read(int unit, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint32_t req_flags, int index,
                                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                                   bcmptm_rm_tcam_req_t *req_info,
                                   bcmptm_rm_tcam_rsp_t *rsp_info,
                                   bcmltd_sid_t *rsp_ltid, uint32_t *rsp_flags)
{
    int rv = SHR_E_NONE;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pt_dyn_info->tbl_inst,
                                            &tcam_info));
    /* Read the TCAM entry words. */
    if (tcam_info->flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {

        /* Read the  the entry words from index using aggregated view. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        rv = bcmptm_rm_tcam_hw_read(unit,
                                    ltid, ltid_info, rm_flags, req_flags, index,
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
                                    ltid, ltid_info, rm_flags, req_flags, index,
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
                                    ltid, ltid_info, rm_flags, req_flags, index,
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

/*
 * Clear a TCAM entry at specific index. Involves writing all zeros to a
 * particulat TCAM index. Updates both HW and SW state.
 */
int
bcmptm_rm_tcam_prio_only_entry_clear(int unit,
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
    int rv = SHR_E_NONE;
    uint32_t *entry_hash = NULL;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint8_t part;
    uint32_t hash_val;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint8_t entry_type = 0;
    uint32_t idx;
    uint8_t hw_only;
    uint8_t sw_only;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pt_dyn_info->tbl_inst,
                                            &tcam_info));


    /* Fetch the TCAM information for the given ltid. */
    rv = bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid, ltid_info,
                                                 pt_dyn_info->tbl_inst,
                                                 &entry_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    sw_only = entry_update_flags & BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY;
    hw_only = entry_update_flags & BCMPTM_RM_TCAM_ENTRY_OP_HW_ONLY;

    /* if entry is invalid no need to clear it. */
    if (entry_info[index].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID && !hw_only) {
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (!sw_only) {
        /* Update the HW with NULL data. */
        for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
            sal_memset(entry_words[unit][part], 0,
                       sizeof(entry_words[unit][part]));
            entry_words_row[unit][part] = entry_words[unit][part];
        }


        if (tcam_info->flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {

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
    rv = bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit, ltid, ltid_info,
                                                      req_info->ekw,
                                                      NULL, &hash_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_hash_get(unit, ltid, ltid_info,
                                                 pt_dyn_info->tbl_inst,
                                                 &entry_hash);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    curr_offset = entry_hash[hash_val];
    while (curr_offset != (int)index) {
        prev_offset = curr_offset;
        curr_offset = entry_info[curr_offset].offset;
    }

    if (curr_offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        entry_info[prev_offset].offset = entry_info[curr_offset].offset;
    } else {
        entry_hash[hash_val] = entry_info[curr_offset].offset;
    }

    /* 2. Update the TCAM entry information. */
    entry_type = ltid_info->hw_entry_info->num_key_rows;

    for (idx = index; idx < (index + entry_type); idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = 1;
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT(rm_tcam_prio_only_find_loops(unit, ltid, ltid_info,
                                                             pt_dyn_info->tbl_inst));
    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_prio_only_compress
 * Purpose:
 *     Compress entries in order to free entries for 2x,3x and 4x type
 *     entries.
 * Paramters:
 *     unit          - (IN) BCM device number
 *     ltid          - (IN) Logical Table enum that is accessing the table
 *     entry_info    - (IN) Field entry structure.
 *     entry_attr    - (IN) Entry attrs structure
 *
 * Returns:
 *     SHR_E_XXX
 */
int
bcmptm_rm_tcam_prio_only_compress(int unit,
                        bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        bcmbd_pt_dyn_info_t *pt_dyn_info,
                        void *attrs)
{
    int rv = SHR_E_NONE;
    int eidx;
    uint32_t idx;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint32_t num_entries;
    bcmltd_sid_t other_ltid = 0;
    bcmptm_rm_tcam_lt_info_t other_ltid_info;
    uint8_t shared_count = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);


    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                                   pt_dyn_info->tbl_inst,
                                                   &tcam_info));


    /* Fetch the TCAM information for the given ltid. */
    rv = bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid, ltid_info,
                                                 pt_dyn_info->tbl_inst,
                                                 &entry_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info,
                                             &num_entries));
    /* get the ltid and ltid_info of ipv4 ltid */
    if (ltid_info->rm_more_info->shared_lt_count) {
        shared_count = ltid_info->rm_more_info->shared_lt_count;
        for (idx = 0 ; idx < shared_count ; idx++) {
            if (ltid_info->rm_more_info->shared_lt_info[idx] != ltid) {
                other_ltid = ltid_info->rm_more_info->shared_lt_info[idx];
            }
        }
    }
    if (other_ltid == 0) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&other_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_cmdproc_ptrm_info_get(unit, other_ltid, 0, 0,
                                                 ((void *)&other_ltid_info),
                                                 NULL, NULL));


    eidx = -1;

    for (idx = 0; idx < num_entries; idx++) {
        /* Find an empty slot. */
        if (entry_info[idx].entry_pri == BCMPTM_RM_TCAM_EID_INVALID) {
            if (eidx == -1) {
                /*check whether it is at correct start boundary */
                if (entry_info[idx].entry_type == 1) {
                    eidx = idx;
                }
            }
            continue;
        }
        if (eidx != -1 &&
            entry_info[idx].entry_type == 1) {
            /*
             * move the current entry to empty index found
             * Move the entry in hardware.
             */
             SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_entry_move(
                                                    unit,
                                                    other_ltid,
                                                    &other_ltid_info,
                                                    0,
                                                    idx, eidx,
                                                    attrs,
                                                    pt_dyn_info));
             idx = eidx;
             eidx = -1;
        }
    }
exit:
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
 *    2.1.For priority key based TCAMs, lookup happens with entry Data.
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
bcmptm_rm_tcam_prio_only_entry_insert(int unit,
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
    uint32_t target_idx = -1;
    uint32_t found_index = -1;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

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
        rv = bcmptm_rm_tcam_prio_only_entry_lookup(unit,
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
        SHR_RETURN_VAL_EXIT(bcmptm_rm_tcam_prio_entry_update(unit,
                                          req_flags, ltid, ltid_info,
                                          pt_dyn_info,
                                          req_info, rsp_info,
                                          rsp_ltid, rsp_flags,
                                          found_index));
    } else {

       /* Fetch the TCAM information for the given ltid, pipe. */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid,
                                               ltid_info, pt_dyn_info->tbl_inst,
                                               &tcam_info));

       /* Find Index to Insert the TCAM entry */
       rv = bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                                          req_flags,
                                                          ltid,
                                                          ltid_info,
                                                          0,
                                                          pt_dyn_info,
                                                          req_info,
                                                          rsp_info,
                                                          rsp_ltid,
                                                          rsp_flags,
                                                          &target_idx);
       SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
       if (rv == SHR_E_RESOURCE) {
           /*call compress API and call allocate again */
           SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_compress(unit,
                                                                    ltid,
                                                                    ltid_info,
                                                                    pt_dyn_info,
                                                                    req_info->entry_attrs));

           SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                                                  req_flags,
                                                                  ltid,
                                                                  ltid_info,
                                                                  0,
                                                                  pt_dyn_info,
                                                                  req_info,
                                                                  rsp_info,
                                                                  rsp_ltid,
                                                                  rsp_flags,
                                                                  &target_idx));
       }
       /* Insert the new entry at target_idx. */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_write(unit,
                                                           ltid, ltid_info,
                                                           req_flags, 0,
                                                           target_idx,
                                                           pt_dyn_info,
                                                           req_info,
                                                           rsp_ltid,
                                                           rsp_flags,
                                                           rsp_info->rsp_entry_sid));

       /* Read the inserted data to the response */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_read(unit,
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
       tcam_info->free_entries -= ltid_info->hw_entry_info->num_key_rows;

    }

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 *  Move the range of TCAM entries one index up or down.
 */
int
bcmptm_rm_tcam_prio_only_entry_move_range(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint8_t partition,
                                uint32_t idx_start,
                                uint32_t idx_end,
                                uint8_t entry_type,
                                void *attrs,
                                bcmbd_pt_dyn_info_t *pt_dyn_info)
{
    int rv = SHR_E_NONE;
    int idx = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    int count = 0;
    int num_key_rows = 0;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                            ltid, ltid_info,
                                                            pt_dyn_info->tbl_inst,
                                                            &entry_info));
    num_key_rows = ltid_info->hw_entry_info->num_key_rows;

    /* Shift entries in down ward direction(i.e. one index up). */
    if (idx_end > idx_start) {

        for (idx = (idx_end - 1); (idx >= (int)idx_start)
             && (idx < (int)idx_end); idx--) {
            if (entry_info[idx].entry_type == entry_type &&
                entry_info[idx + num_key_rows + count].entry_type == entry_type) {
                rv = bcmptm_rm_tcam_entry_move(unit, ltid, ltid_info, partition,
                                              idx - num_key_rows + 1,
                                              idx + 1 + count,
                                              attrs, pt_dyn_info);
                idx = idx - num_key_rows + 1;
                count = 0;
            } else {
                count++;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    /* Shift entries in upward direction(i.e. one index down). */
    if (idx_end < idx_start) {

        for (idx = idx_end + num_key_rows; idx < (int)idx_start+num_key_rows; idx++) {
            if (entry_info[idx].entry_type == entry_type &&
                entry_info[idx - num_key_rows - count].entry_type == entry_type) {
                rv = bcmptm_rm_tcam_entry_move(unit, ltid, ltid_info, partition,
                                           idx, idx - num_key_rows - count,
                                           attrs, pt_dyn_info);
                idx = idx + num_key_rows - 1;
                count = 0;
            } else {
                count++;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Create a new free slot to insert an entry. */
int
bcmptm_rm_tcam_prio_only_entry_index_allocate(int unit,
                                 uint32_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t prio_change,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 uint32_t *target_index)
{
    int target_idx = -1;
    int idx = -1;
    int idx_temp = -1;
    int up_free_block_first_idx = -1;
    int up_free_idx = -1;
    int down_free_idx = -1;
    int from_idx = -1;
    int to_idx = -1;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint32_t num_key_rows;
    uint32_t count = 0;
    uint32_t entry_start_boundary = 0;
    int last_higher_prio_entry_type_idx = -1;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                               pt_dyn_info->tbl_inst,
                                               &tcam_info));

    entry_start_boundary = ltid_info->hw_entry_info->entry_boundary;
    /* Fetch the entry type details */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_num_index_required_get(unit,
                                                       ltid, ltid_info,
                                                       &num_key_rows));


    /*
     * If free entries are not available, entry cannot be inserted.
     * If HW atomicity is supported then full range can be used.
     * For entries which require more than one key row atomicity
     * can't be achieved even if HW supports.
     */
    if ((prio_change == 0) &&
        ((ltid_info->rm_more_info->hw_atomicity_support == 0) ||
         (tcam_info->unused_entries > 1) || ltid_info->non_aggr) &&
        (tcam_info->free_entries < (tcam_info->unused_entries + num_key_rows))) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    if (tcam_info->free_entries < num_key_rows) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    /* If no valid entries allocate the very first entry in TCAM. */
    if (tcam_info->free_entries == tcam_info->num_entries) {
        *target_index = 0;
        SHR_FUNC_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                        ltid, ltid_info,
                                                        pt_dyn_info->tbl_inst,
                                                        &entry_info));

    for (idx = 0; idx < tcam_info->num_entries; idx += num_key_rows) {

        /* Record the last free tcam index before target index is found. */
        if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            if (entry_info[idx].entry_pri == BCMPTM_RM_TCAM_EID_INVALID) {
                if (num_key_rows == 1 &&
                    entry_start_boundary == 0) {
                    up_free_idx = idx;
                    /* Record first index when there a block of free entries */
                    if (up_free_block_first_idx == -1) {
                        up_free_block_first_idx = idx;
                    }
                    continue;
                } else if ((idx % entry_start_boundary) == 0) {
                    /* entry index should be multiple of start boundary */
                    /* check for entry type number of free entries*/
                    count = 1;
                    idx_temp = idx;
                    idx_temp++;
                    while (count != num_key_rows) {
                        if (entry_info[idx_temp].entry_pri == BCMPTM_RM_TCAM_EID_INVALID) {
                            count++;
                            idx_temp++;
                        } else {
                            /* count get 4 continious entries*/
                            break;
                        }
                    }
                    if (count == num_key_rows) {
                        up_free_idx = idx;
                        /* Record first index when there a block of free entries */
                        if (up_free_block_first_idx == -1) {
                            up_free_block_first_idx = idx;
                        }
                        continue;
                    }
                }
                /* Reset up_free_block_first_idx to -1 */
                if (up_free_block_first_idx != -1) {
                    up_free_block_first_idx = -1;
                }
                continue;
            }
        }

        /* Record the target index for the given prioirty. */
        if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            if(entry_info[idx].entry_type == num_key_rows) {
                if (entry_info[idx].entry_pri < req_info->entry_pri) {
                    target_idx = idx;
                    /* If free entries block is just above the target index
                     * up_free_block_first_idx is the target index
                     * for the new entry */
                    if (up_free_block_first_idx != -1) {
                        break;
                    }
                    continue;
                } else {
                    /* Free entries block is just before entry with same prio.
                     * use up_free_block_first_idx as the target index*/
                    if ((up_free_block_first_idx != -1) &&
                        (entry_info[idx].entry_pri == req_info->entry_pri)) {
                        break;
                    }
                    up_free_block_first_idx = -1;
                    last_higher_prio_entry_type_idx = idx;
                    continue;
                }
            }
            /* Reset up_free_block_first_idx to -1 */
            if (up_free_block_first_idx != -1) {
                up_free_block_first_idx = -1;
            }
        }

        /* Record the first free tcam index after target index is found. */
        if (entry_info[idx].entry_pri == BCMPTM_RM_TCAM_EID_INVALID) {
            if (num_key_rows == 1 &&
                entry_start_boundary == 0) {
                down_free_idx = idx;
                break;
            } else if ((idx % entry_start_boundary) == 0) {
                /* entry index should be multiple of start boundary */
                /* check for entry type number of free entries*/
                count = 1;
                idx_temp = idx;
                idx_temp++;
                while (count != num_key_rows) {
                    if (entry_info[idx_temp].entry_pri == BCMPTM_RM_TCAM_EID_INVALID) {
                        count++;
                        idx_temp++;
                    } else {
                        /* count get 4 continious entries*/
                        break;
                    }
                }
                if (count == num_key_rows) {
                    down_free_idx = idx;
                    break;
                }
            }
        }
    }

    /* Ideally should not hit this case as the above loop will be
     * executed only if atleast one free index is available in TCAM.
     */
    if (up_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        num_key_rows == 1) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } else if (up_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        num_key_rows > 1){
        /* Free entries are present but couldnot allocate
         * as they are not continuous. call compression API
         */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
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
            /* Approach: find the first entry from bottom of the
             * entry_info[idx] which is free or which is of its entry_type
             * where the first entry of this type is found.
             */
            /* check if last higher prio entry is -1, if so then
             * this entry is first entry of this entry_type
             * so set target_idx = up_free_idx found
             */
             if (last_higher_prio_entry_type_idx == -1) {
                  target_idx = up_free_idx;
             }
             /* some entries of this entry_type exists but this is the
              * least priority entry. so last higher prio entry type idx
              * will have the last entry position. check if the
              * up_free_idx is after this entry. if so we can write directly
              * to up_free_idx place.
              * else we have to move the all the entries from last higher
              * prio entry type idx to up_free_idx and write the new entry
              * at last higher prio entry type idx
              */
             else if( last_higher_prio_entry_type_idx < up_free_idx) {
                 target_idx = up_free_idx;
             } else {
                 target_idx = last_higher_prio_entry_type_idx;
             }
        } else {
            /* check if last higher prio entry is less than up_free_idx,
             * if so this entry need to be inserted at top for this entry_type
             * so set target_idx = up_free_idx found
             */
             if (last_higher_prio_entry_type_idx < up_free_idx) {
                  target_idx = up_free_idx;
             } else {
                 target_idx = last_higher_prio_entry_type_idx;
             }
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
            if (last_higher_prio_entry_type_idx < up_free_idx) {
                target_idx = up_free_idx;
            } else {
                target_idx = last_higher_prio_entry_type_idx;
            }
            from_idx = target_idx;
            to_idx = up_free_idx;
        }
    }
    /* update the entry_type at target_idx */
    for (idx = 0; idx < (int) num_key_rows; idx++) {
        entry_info[to_idx + idx].entry_type = num_key_rows;
    }
    if (from_idx != to_idx) {
        /* Move entries either up or down ward direction. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_move_range(unit,
                                                        ltid, ltid_info,
                                                        0, from_idx,
                                                        to_idx,
                                                        num_key_rows,
                                                        req_info->entry_attrs,
                                                        pt_dyn_info));
    }

    *target_index = target_idx;

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/*
 * Do search for given TCAM entry key and read the corresponding entry index.
 *
 * Algorithm:
 *  1. Application provides required information about the TCAM Entry
 *     to identify the entry location in HW.
 *     1.1. For priority based TCAM; this information contains entry_key
 *  2. Caliculate the hash for the given TCAM entry Id.
 *  3. Check TCAM entry id is present in linked list of Tcam entry ids
 *     having the same hash value.
 *  4. If entry_id exists, get the TCAM entry Index from TCAM entry metadata.
 *     4.1. If TCAM Entry Index is non negative read entry data (KEY + MASK +
 *          POLICY + PRI + OFFSET) will be given back to application.
 *     4.2. If TCAM entry index is negative, then there is a bug in the code and
 *          return error code Internal Error
 *  5. If entry_id does not exist, return error code Entry not Found.
 */

int
bcmptm_rm_tcam_prio_only_entry_lookup(int unit,
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
    int rv = SHR_E_NONE;
    uint32_t *entry_hash = NULL;
    int offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t hash_val = 0;
    uint32_t entry_type;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    same_key_info[unit].index = -1;
    same_key_info[unit].priority = -1;

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pt_dyn_info->tbl_inst,
                                            &tcam_info));

    /* Check if entry id  exists or not. */
    rv = bcmptm_rm_tcam_prio_only_entry_info_get(unit, ltid, ltid_info,
                                                 pt_dyn_info->tbl_inst,
                                                 &entry_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_hash_get(unit, ltid, ltid_info,
                                                 pt_dyn_info->tbl_inst,
                                                 &entry_hash);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_rm_tcam_prio_only_entry_hash_value_get(unit, ltid, ltid_info,
                                                      req_info->ekw,
                                                      NULL, &hash_val);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    offset = entry_hash[hash_val];

    /* Fetch the entry type details */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_num_index_required_get(unit,
                                                       ltid, ltid_info,
                                                       &entry_type));
    while (offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {

        if(entry_info[offset].entry_type == entry_type &&
            (offset + entry_type) <= tcam_info->num_entries) {

            rv = rm_tcam_prio_only_entry_compare(unit, ltid, ltid_info, offset,
                                                   pt_dyn_info, req_info);

            if (SHR_SUCCESS(rv)) {
                if (index != NULL) {
                    *index = offset;
                }
                break;
            }
        }
        offset = entry_info[offset].offset;
    }

    if (offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
        SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                       ltid, ltid_info,
                                                       req_flags, offset,
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

/*
 * Delete the TCAM entry for a given Entry Key.
 *
 * Algorithm:
 *  1. Application provides required information about the TCAM Entry
 *     to identify the entry location in HW.
 *     1.1. For priority based TCAM; this information contains entry_key
 *  2. Do Lookup for the given TCAM information to find the entry index.
 *     2.1. For priority based TCAM lookup happens on entry_key.
 *  3. If entry index is Non Negative (meaning entry is existed)?
 *     3.1. Call south bound interface to give the write command with
 *          NULL entry data (meaning all zero) at entry index.
 *     3.2. Return SUCCESS
 *  4. If entry index is Negative (meaning entry does not exist)?
 *     4.1. Return the error code ENTRY NOT FOUND.
 */

int
bcmptm_rm_tcam_prio_only_entry_delete(int unit,
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
    uint32_t index = BCMPTM_RM_TCAM_INDEX_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_tcam_info_get(unit, ltid, ltid_info,
                                            pt_dyn_info->tbl_inst,
                                            &tcam_info));

    /* Check if entry id exists or not. */
    rv = bcmptm_rm_tcam_prio_only_entry_lookup(unit,
                                              req_flags, ltid, ltid_info,
                                              pt_dyn_info,
                                              req_info, rsp_info,
                                              rsp_ltid, rsp_flags, &index);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* Clear the data in HW and SW state */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_prio_only_entry_clear(unit, ltid,
                                                        ltid_info,
                                                        req_flags,
                                                        0, index,
                                                        pt_dyn_info, req_info,
                                                        rsp_ltid, rsp_flags,
                                                        rsp_info->rsp_entry_sid));

    tcam_info->free_entries += ltid_info->hw_entry_info->num_key_rows;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_entry_delete(unit, ltid, ltid_info,
                                                  pt_dyn_info,
                                                  req_info));

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

/* Calculate memory size required for Prio Only type TCAMs */
int
bcmptm_rm_tcam_prio_only_calc_mem_required(int unit, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *user_data)
{
    uint32_t size = 0;
    uint32_t trans_req_size = 0;
    uint32_t num_entries = 0;
    uint32_t entry_hash_size = 0;
    uint32_t *total_size = user_data;
    int8_t num_pipes;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check. */
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    num_pipes = ltid_info->rm_more_info[0].pipe_count;

    /* This should always be the first structure
     * memory allocation for every LTID
     */
    /* memory required to save Transaction related info */
    trans_req_size = sizeof(bcmptm_rm_tcam_trans_info_t);

    size += sizeof(bcmptm_rm_tcam_prio_only_info_t);

    size += sizeof(bcmptm_rm_tcam_prio_only_sw_state_t);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info,
                                             &num_entries));

    /* Memory required for TCAM entry information consists of
     * (priority and offset)
     */
    size += (num_entries * sizeof(bcmptm_rm_tcam_prio_only_entry_info_t));

    if (num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
        entry_hash_size = (num_entries / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
    } else {
        entry_hash_size = num_entries;
    }

    /* Memory space required for TCAM hash table. */
    size += (sizeof(uint32_t) * entry_hash_size);

    (*total_size) += trans_req_size + (num_pipes * size);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the SW state of pri_key based TCAM in
 * bcmptm_rm_tcam_prio_only_info_t format.
 */
int
bcmptm_rm_tcam_prio_only_init_metadata(int unit, bool warm, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      void *start_pointer, uint32_t *offset)
{
    uint32_t  *entry_hash = NULL;
    uint32_t num_entries = 0;
    uint32_t idx;
    uint32_t entry_info_size = sizeof(bcmptm_rm_tcam_prio_only_entry_info_t);
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_sw_state_t *sw_state = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    uint8_t *start_ptr = NULL;
    uint32_t num_pipes = 0;
    uint32_t pipe = 0;
    uint32_t size = 0;
    uint32_t unused_entries = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(start_pointer, SHR_E_PARAM);
    SHR_NULL_CHECK(offset, SHR_E_PARAM);

    start_ptr = (uint8_t*) start_pointer;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info,
                                             &num_entries));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_num_index_required_get(unit,
                                             ltid, ltid_info,
                                             &unused_entries));

    num_pipes = ltid_info->rm_more_info[0].pipe_count;

    /* set the transaction info */
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
        tcam_info = (bcmptm_rm_tcam_prio_only_info_t *)(start_ptr + *offset);

        /*
         * Initalize only during coldboot.
         * During warmboot, reinitialize only offset fields, size and offset.
         * num_entries are expected to not change across warmboot versions.
         */
        if (warm == FALSE) {
            tcam_info->ltid = ltid;

            /* Set tcam info flags according to LT_INFO flags */
            tcam_info->flags = 0;
            if (ltid_info->non_aggr == 0) {
                tcam_info->flags |= BCMPTM_RM_TCAM_F_AGGR_VIEW;
            }
            if (ltid_info->non_aggr == 1) {
                tcam_info->flags |= BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
                tcam_info->flags |= BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
            }

            /* Set the entry counts */
            tcam_info->num_entries = num_entries;
            tcam_info->free_entries =  num_entries;
            tcam_info->unused_entries = unused_entries;
        } else {
            /*
             * Ensure that ltid and num_entries is not changed
             * during warmboot.
             */
            if (tcam_info->ltid != ltid ||
                tcam_info->num_entries != num_entries) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }

        tcam_info->sw_state_offset = sizeof(bcmptm_rm_tcam_prio_only_info_t);

        sw_state = (bcmptm_rm_tcam_prio_only_sw_state_t*) (start_ptr + *offset
                                 + tcam_info->sw_state_offset);

        sw_state->entry_info_offset = tcam_info->sw_state_offset +
                                  sizeof(bcmptm_rm_tcam_prio_only_sw_state_t);

        /* Initalize only during coldboot. */
        if (warm == FALSE) {
            if (tcam_info->num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
                sw_state->entry_hash_size = (num_entries /
                                       BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
            } else {
                sw_state->entry_hash_size = num_entries;
            }
        } else {
            /*
             * Ensure that hash_size is not changed
             * during warmboot.
             */
            if (num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
                if (sw_state->entry_hash_size != (num_entries /
                                   BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)) {
                    SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
                }
            } else if (sw_state->entry_hash_size != num_entries) {
                SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
            }
        }

        /* Set the entry info and hash details */
        sw_state->entry_hash_offset = sw_state->entry_info_offset +
                                   (tcam_info->num_entries * entry_info_size);

        /* Initalize only during coldboot. */
        if (warm == FALSE) {
            entry_info = (bcmptm_rm_tcam_prio_only_entry_info_t*)
                         (start_ptr + *offset + sw_state->entry_info_offset);

            for (idx = 0; idx < tcam_info->num_entries; idx++) {
                entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
                entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
                entry_info[idx].entry_type = 1;
            }

            entry_hash = (uint32_t *) (start_ptr + *offset
                                       + sw_state->entry_hash_offset);
            for (idx = 0; idx < sw_state->entry_hash_size; idx++) {
                entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
            }
        }

        size =  (sizeof(bcmptm_rm_tcam_prio_only_info_t) +
                 (sizeof(bcmptm_rm_tcam_prio_only_sw_state_t)) +
                 (sizeof(bcmptm_rm_tcam_prio_only_entry_info_t) * num_entries) +
                 (sizeof(uint32_t) * sw_state->entry_hash_size));

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

/*
 * Initialize the Max key rows needed for an entry in TCAM in
 * bcmptm_rm_tcam_prio_only_info_t format.
 */
int
bcmptm_rm_tcam_prio_only_update_unused_entries(int unit, bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint32_t unused_entries)
{
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    uint32_t num_pipes = 0;
    uint32_t pipe = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    num_pipes = ltid_info->rm_more_info[0].pipe_count;

    for (pipe = 0; pipe < num_pipes; pipe++)
    {
        /* Fetch the TCAM information for the given ltid. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                   ltid, ltid_info, pipe,
                                                   &tcam_info));
        if (unused_entries > tcam_info->unused_entries) {
            tcam_info->unused_entries = unused_entries;
        }
    }
exit:

    SHR_FUNC_EXIT();
}
